#!/usr/bin/env perl

# $Header: /data/zender/nco_20150216/nco/bm/nco_bm.pl,v 1.24 2005-06-06 22:47:22 mangalam Exp $

# Usage:  (see usage() below for more info)
# <BUILD_ROOT>/nco/bld/nco_bm.pl # Tests all operators
# <BUILD_ROOT>/nco/bld/nco_bm.pl ncra # Test one operator

# NB: When adding tests, _be sure to use -O to overwrite files_
# Otherwise, script hangs waiting for interactive response to overwrite queries
require 5.6.1 or die "This script requires Perl version >= 5.6.1";
use Cwd 'abs_path';
use Getopt::Long; #qw(:config no_ignore_case bundling);
use strict;

# Declare vars for strict

use vars qw($dsc_lng_max $dot_nbr $dot_nbr_min $dot_fmt $dot_sng $dsc_fmt $tst_fmt
	    $spc_fmt $spc_nbr $spc_nbr_min $spc_sng $opr_lng_max $opr_fmt $tst_id_sng $tst_idx
 $dbg_lvl $wnt_log $usg $opr_nm @tst_cmd $dsc_sng $nsr_xpc
 @opr_lst_all @opr_lst $MY_BIN_DIR %sym_link %tst_nbr %success %failure
 $result $server_name $server_ip $server_port $sock $udp_reprt $tst_fl_cr8
 $dta_dir $bm_dir $tmr_app %subbenchmarks %totbenchmarks $que $rgr $bm $itmp 
 @bm_cmd_ary @ifls $localhostname $notbodi $prfxd $prefix $thr_nbr $cmd_ln $arg_nbr
 $omp_flg $dodods $fl_pth
);

my @fl_cr8_dat;  # holds the strings for the fl_cr8() routine 
my @fl_tmg; # holds the timing data for the fl_cr8() routines.


# initializations
# re-constitute the commandline
$cmd_ln = "$0 "; $arg_nbr = @ARGV;
for (my $i=0; $i<$arg_nbr; $i++){ $cmd_ln .= "$ARGV[$i] ";}

$bm_dir = `pwd`; chomp $bm_dir;
# option flag inits
$dbg_lvl = 0; # [enm] Print tests during execution for debugging
$wnt_log = 0;
$usg   = 0;
$tst_fl_cr8 = "0";
$que = 0;
$udp_reprt = 0;
$thr_nbr = 0; # if zero, don't want to do threads 
$dodods = "";  $fl_pth = "";

# other inits
$localhostname = `hostname`;
$notbodi = 0; # specific for hjm's puny laptop
$prfxd = 0;
$prefix = "";
if ($localhostname !~ "bodi") {$notbodi = 1} # spare the poor laptop
$ARGV = @ARGV;

if ($ARGV == 0) {usage(); die "We need some more info to be a useful member of society\n"; }

# set up options and switches
&GetOptions(
	"debug=i"      => \$dbg_lvl,    # debug level
	"log"          => \$wnt_log,    # set if want output logged
	"udpreport"    => \$udp_reprt,  # punt the timing data back to udpserver on sand
	"test_files=s" => \$tst_fl_cr8,    # create the test files "134" does 1,3,4
	"regress"      => \$rgr,        # test regression 
	#if dods is set && string is NULL do tests with sand / DODS instead of local files , 
	#if string is NOT NULL, use URL to grab files.
	"dods=s"       => \$dodods,     
	"benchmark"    => \$bm,         # do the real benchmarks 
	"queue"        => \$que,        # if set, bypasses all interactive stuff
	"thr_nbr=i"    => \$thr_nbr,    # number of threads to use in benchmark (not in regress)
	"usage"        => \$usg,        # explains how to use this thang
	"help"         => \$usg,        # explains how to use this thang
	"h"            => \$usg,        # explains how to use this thang
);

my $NUM_FLS = 4; # max number of files in the file creation series

#test nonfatally for useful modules
my $hiresfound;
print "\n===== Testing for required modules\n";
BEGIN {eval "use Time::HiRes qw(usleep ualarm gettimeofday tv_interval)"; $hiresfound = $@ ? 0 : 1}
#$hiresfound = 0;  # uncomment to simulate not found
if ($hiresfound == 0) {
    print "\nOoops! Time::HiRes (needed for accurate timing) not found\nContinuing without timing.";
} else {
    print "Time::HiRes ... found!\n";
}
my $iosockfound;
BEGIN {eval "use IO::Socket"; $iosockfound = $@ ? 0 : 1}
#$iosockfound = 0;  # uncomment to simulate not found
if ($iosockfound == 0) {
    print "\nOoops! IO::Socket module not found - continuing with no udp logging.\n";
} else {
    print "IO::Socket ... found!\n\n";
}

# the real udping server
$server_name = "sand.ess.uci.edu";
$server_ip = "128.200.14.132";
$server_port = 29659;

if ($usg) { usage()};   # dump usage blurb
if (0) { tst_hirez(); } # tests the hires timer - needs explict code mod to do this

if ($iosockfound) {
    $sock = IO::Socket::INET->new (
				   Proto    => 'udp',
				   PeerAddr => $server_ip,
				   PeerPort => $server_port
				   ) or die "\nCan't get the socket!\n\n";
} else {$udp_reprt = 0;}

if ($wnt_log) { 
    open(LOG, ">nctest.log") or die "\nCan't open the log file 'nctest.log' - check permissions on it \nor the directory you're in.\n\n";
}

# check for requested threads.
if ($thr_nbr >= 0) { $omp_flg = " -t=$thr_nbr ";} # thread of 1 doesn't help things
else { $omp_flg = "";}

# examine env DATA and talk to user to figure where $DATA  should be
set_dat_dir(); # now $dta_dir is set to 

if ($dodods eq "") {
	$fl_pth = "$dta_dir";
} elsif ($dodods =~ /http:\/\//) {
	$fl_pth = "$dodods";
} else {
	print "'--dods' option ($dodods) doesn't tastelike a real URL - typo or thinko?\nContinuing by trying to find the local files in the $dta_dir dir.\n"; 
	$fl_pth = "$dta_dir";
}

#initialize & set up some vars
initialize();
# now start the real tests
# first the file creation tests:

# also want to try to grok /usr/bin/time, as in the shell scripts
if (-e "/usr/bin/time" && -x "/usr/bin/time") {
	$tmr_app = "/usr/bin/time";
	if (`uname` =~ "inux" && $dbg_lvl > 1) { $tmr_app .= " -v ";}
} else { # just use whatever the shell thinks is the time app
	$tmr_app = "time"; #could be the bash builtin or another 'time'-like app (AIX)
}

#set_dat_dir(); # examine env DATA and talk to user to figure where $DATA  should be

if ($rgr){  # if want regression tests 
	perform_tests();
	smrz_rgr_rslt();
}

# test to see if the necessary files are available - if so, can skip the creation tests
fl_cr8_dat_init(); # initialize the data strings & timing array for files

# checks if files have already been created.  If so, can skip file creation if not requested
if ($bm && $tst_fl_cr8 == 0) { 
	for (my $i = 0; $i < $NUM_FLS; $i++) {
		my $fl = $fl_cr8_dat[$i][2] . ".nc"; #file root name stored in $fl_cr8_dat[$i][2] 
		if (-e "$dta_dir/$fl" && -r "$dta_dir/$fl") {
			print "$fl exists - can skip creation\n";
		} else {
			my $e = $i+1;
			$tst_fl_cr8 .= "$e";
		}
	}
}


# file creation tests
if ($tst_fl_cr8 ne "0"){
#	set_dat_dir();      # examine env DATA and talk to user to figure where $DATA  should be
    # so want to so some - check for disk space on DATA dir 1st - nah - assume personal responsibility
    my $fc = 0;
    if ($tst_fl_cr8 =~ "[Aa]") { $tst_fl_cr8 = "1234";}
    if ($tst_fl_cr8 =~ /1/){ fl_cr8(0); $fc++; }
    if ($tst_fl_cr8 =~ /2/){ fl_cr8(1); $fc++; }
    if ($tst_fl_cr8 =~ /3/){ fl_cr8(2); $fc++; }
    if ($notbodi && $tst_fl_cr8 =~ /4/) { fl_cr8(3); $fc++; }	
#	print "Summarizing results of file creation\n";
    if ($fc >0) {smrz_fl_cr8_rslt(); } # prints and udpreports file creation timing
} #elsif (!$rgr) { # just for now - will be removed
#	print "\nFile creation tests skipped.\n\n";
#}

# and now, the REAL benchmarks, set up as the regression tests below to use go() and smrz_rgr_rslt()
if ($bm) {
	$prefix = "$tmr_app $MY_BIN_DIR"; $prfxd = 1; #embed the timer command and local bin in cmd
	print "\nStarting Benchmarks now\n";
	#################### begin cz benchmark list #8
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order reversal the file ';
	####################	
	$tst_cmd[0] = "$prefix/ncpdq -O $omp_flg -a -lat -p $fl_pth ipcc_dly_T85.nc  $dta_dir/ipcc_dly_T85-ncpdq.nc";
	# ~2m on sand
	$tst_cmd[1] = "$prefix/ncks -C -H -s \"%f\" -v dopey $dta_dir/ipcc_dly_T85-ncpdq.nc"; 
	$nsr_xpc = "0.800000";
	go();
	
	#################### begin cz benchmark list #7
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq packing a file';
	####################	
	$tst_cmd[0] = "$prefix/ncpdq -O $omp_flg -P all_new  -p $fl_pth ipcc_dly_T85.nc  $dta_dir/ipcc_dly_T85-ncpdq.nc";
	# ~41 s on sand
	$tst_cmd[1] = "$prefix/ncks -C -H -s \"%f\" -v dopey $dta_dir/ipcc_dly_T85-ncpdq.nc"; 
	$nsr_xpc = "0.000000";
	go();
	
	#################### begin cz benchmark list #6
	$opr_nm='ncap';
	$dsc_sng = 'ncap long algebraic operation ';
	####################	
	$tst_cmd[0] = "$prefix/ncap -O -s  \"nu_var[time,lat,lon,lev]=d4_01*d4_02*(d4_03**2)-(d4_05/d4_06)\"   -p $fl_pth ipcc_dly_T85.nc  $dta_dir/ipcc_dly_T85.ncap.nc";
	$tst_cmd[1] = "$prefix/ncwa -O $omp_flg -y sqrt -a lat,lon $dta_dir/ipcc_dly_T85.ncap.nc  $dta_dir/ipcc_dly_T85.ncwa.nc ";
	$tst_cmd[2] = "$prefix/ncks -C -H -s \"%f\" -v d2_00  $dta_dir/ipcc_dly_T85.ncwa.nc"; 
	$nsr_xpc = "4.024271";
	go();

	#################### begin cz benchmark list #4
	$opr_nm='ncbo';
	$dsc_sng = 'ncbo differencing two files';
	####################	
	$tst_cmd[0] = "$prefix/ncbo -O $omp_flg --op_typ='*' -p $fl_pth ipcc_dly_T85.nc ipcc_dly_T85_00.nc $dta_dir/ipcc_dly_T85-ncbo.nc";
	$tst_cmd[1] = "$prefix/ncks -C -H -s \"%f\" -v sleepy $dta_dir/ipcc_dly_T85-ncbo.nc";
	$nsr_xpc = "0.640000";	
	go();

	#################### begin cz benchmark list #3
	$opr_nm='ncea';
	$dsc_sng = 'ncea averaging 2^5 files';
	####################	
	$tst_cmd[0] = "$prefix/ncea -O $omp_flg  -p $fl_pth stl_5km.nc $dta_dir/stl_5km-ncwa.nc";
	$tst_cmd[1] = "$prefix/ncwa -O $omp_flg -y sqrt -a lat,lon $dta_dir/stl_5km-ncwa.nc $dta_dir/stl_5km-ncea.nc";
	$tst_cmd[2] = "$prefix/ncks -C -H -s \"%f\" -v d2_00  $dta_dir/stl_5km-ncea.nc"; 
	$nsr_xpc = "1.604304";
	go();
	
	#################### begin cz benchmark list #2
	$opr_nm='ncra';
	$dsc_sng = 'ncra time-averaging 2^5 (i.e. one month) ipcc files';
	####################	
	# have to set up the symlinks for the benchmark
	# do this initially for all files, use only small ones initially then maybe the larger files
	for (my $f=0; $f<$NUM_FLS; $f++) {
		my $rel_fle = "$dta_dir/$fl_cr8_dat[$f][2]" . ".nc" ;
		my $ldz = "0"; # leading zero for #s < 10
		if ($dbg_lvl > 0) {print "\tsymlinking $rel_fle\n";}
		for (my $n=0; $n<32; $n ++) {
			if ($n>9) {$ldz ="";}
			my $lnk_fl_nme = "$dta_dir/$fl_cr8_dat[$f][2]" . "_" . "$ldz" . "$n" . ".nc";
			if (-r $rel_fle && -d $dta_dir && -w $dta_dir){
				symlink $rel_fle, $lnk_fl_nme;
			}
		}
	}

	if ($notbodi) { # too big for bodi
		$tst_cmd[0] = "$prefix/ncra -O $omp_flg $fl_pth/ipcc_dly_T85_*.nc $dta_dir/ipcc_ncra-av.nc";
		# ~4m on sand.
		if ($dbg_lvl >0) {print "Executing \n";}
		$tst_cmd[1] =  "$prefix/ncks -C -H -s \"%f\" -v d1_03    $dta_dir/ipcc_ncra-av.nc ";
		$nsr_xpc = "1.800001";
		go();
	}

	#################### begin cz benchmark list #1
	$opr_nm='ncwa';
	$dsc_sng = 'ncwa averaging all variables to scalars';
	####################	
	
	# using sml_stl.nc
	$tst_cmd[0] = "$prefix/ncwa -O $omp_flg -y sqrt -a lat,lon -p $fl_pth sml_stl.nc $dta_dir/sml_stl-ncwa.nc";
	$tst_cmd[1] = "$prefix/ncks -C -H -s \"%f\" -v d2_02  $dta_dir/sml_stl-ncwa.nc";
	$nsr_xpc = "1.974842";
    
	if ($notbodi) { # too big for bodi
		$tst_cmd[0] = "$prefix/ncra -O $dta_dir/ipcc_dly_T85_*.nc $dta_dir/ipcc_ncra-av.nc";
		# ~4m on sand.
		if ($dbg_lvl >0) {print "Executing \n";}
		$tst_cmd[1] =  "$prefix/ncks -C -H -s \"%f\" -v d1_03    $dta_dir/ipcc_ncra-av.nc ";
		$nsr_xpc = "1.800001";
		go();
	
		# using stl_5km.nc
		$tst_cmd[0] = "$prefix/ncwa -O $omp_flg -y rms  -a lat,lon -p $fl_pth stl_5km.nc 	$dta_dir/stl_5km-ncwa.nc";
		$tst_cmd[1] = "$prefix/ncks -C -H -s \"%f\" -v d2_03  $dta_dir/stl_5km-ncwa.nc"; 
		$nsr_xpc = "2.826392";
		go();
	
		#using ipcc_dly_T85.nc
		$tst_cmd[0] = "$prefix/ncwa -O $omp_flg -y sqrt   -a lat,lon -p $fl_pth ipcc_dly_T85.nc $dta_dir/ipcc_dly_T85-ncwa.nc";
		$tst_cmd[1] = "$prefix/ncks -C -H -s \"%f\" -v skanky  $dta_dir/ipcc_dly_T85-ncwa.nc"; 
		$nsr_xpc = "0.800000";
		go();
	smrz_rgr_rslt(); # and summarize the benchmarks
    }  
}	

$prfxd = 0;

sub go {
    
# Perform tests of requested operator; default is all
    if (!defined $tst_nbr{$opr_nm}) { 
	# Clear test array
	@tst_cmd=();
	return; 
    }
    &verbosity('\n\n====================================================================\nResult Stanza for [$opr_nm] ($dsc_sng)\nSubtest [$tst_nbr{$opr_nm}] :\n===========\n');
    
    $subbenchmarks{$opr_nm} = 0;
    
    $tst_nbr{$opr_nm}++;
    my $tst_cmdcnt = 0;
    my $t = 0;
    foreach (@tst_cmd){
	&verbosity("\n\n\t## Part $tst_cmdcnt ## \n");
	    my $opcnt = 0;
	# Add MY_BIN_DIR to NCO operator commands only, not things like 'cut'
	foreach my $op (@opr_lst_all) {
	    if ($_ =~ m/^$op/ && !$prfxd) {
		$_ = "$MY_BIN_DIR/$_" ;
	    }
	}
	# Perform an individual test
	&verbosity("\tFull commandline for [$opr_nm]:\n\t$_\n");
	# timing code using Time::HiRes
	my $t0;
	if ($hiresfound) {$t0 = [gettimeofday];}
	else {$t0 = time;}
	# and execute the command
	$result=`$_` ;
	
	my $elapsed;
	if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
	else {$elapsed = time - $t0;}
	
#	 print "inter benchmark for $opr_nm = $subbenchmarks{$opr_nm} \n";
	$subbenchmarks{$opr_nm} += $elapsed;
	$tst_idx = $tst_nbr{$opr_nm}-1;
	if($dbg_lvl > 3){print "\t$opr_nm subtest [$t] took $elapsed seconds\n";}
	&verbosity("\t$result");
	$tst_cmdcnt++;
    } # end loop over sub-tests
    # csz++
    $dot_nbr_min=3; # Minimum number of dots between description and "ok" result
    $dot_sng='.....................................................................';
    $dsc_lng_max=50; # Maximum length of description printed
    $opr_lng_max=7; # Maximum length of operator name
    $spc_nbr_min=1; # Minimum number of spaces between test ID and description
    $spc_sng='       ';

    $dot_nbr=$dot_nbr_min; # Number of dots printed
    $spc_nbr=$spc_nbr_min; # Number of spaces printed
    $opr_fmt=sprintf("%%.%ds",$opr_lng_max);
    if($opr_lng_max-length($opr_nm)>0){$spc_nbr+=$opr_lng_max-length($opr_nm);}
    $spc_fmt=sprintf("%%.%ds",$spc_nbr);
    $tst_id_sng=sprintf("$opr_fmt$spc_fmt",$opr_nm,$spc_sng).sprintf("test %02d: ",$tst_idx);
    if($dsc_lng_max-length($dsc_sng)>0){$dot_nbr+=$dsc_lng_max-length($dsc_sng);}
    $dsc_fmt=sprintf('%%.%ds',$dsc_lng_max);
    $dot_fmt=sprintf("%%.%ds",$dot_nbr);
    $tst_fmt="$tst_id_sng$dsc_fmt$dot_fmt";
    printf STDERR ($tst_fmt,$dsc_sng,$dot_sng);
    # csz--
    if($dbg_lvl > 2){print STDERR '\tTotal time for $opr_nm [$tst_idx] = $subbenchmarks{$opr_nm} s\n\n';}
    $totbenchmarks{$opr_nm}+=$subbenchmarks{$opr_nm};
    
    # Remove trailing newline for easier regex comparison
    chomp $result;
    
    # Compare numeric results
	if ($nsr_xpc =~/\d/) { # && it equals the expected value
		if ($nsr_xpc == $result) {
		    $success{$opr_nm}++;
			printf STDERR ("ok\n");
	 		&verbosity("\n\tPASSED - Numeric output: [$opr_nm]:\n");
		} elsif (abs($result - $nsr_xpc) < 0.02) {
			$success{$opr_nm}++;
			printf STDERR ("ok\n");
	 		&verbosity("\n\t!!!! PASSED PROVISIONALLY ($nsr_xpc vs $result) - Numeric output: [$opr_nm]:\n");
		} else {
      	&failed($nsr_xpc);
			&verbosity("\n\tFAILED (expected: $nsr_xpc vs result: $result)- Numeric output: [$opr_nm]:\n");
		}
	} elsif ($nsr_xpc =~/\D/)  {# Compare non-numeric tests
		# Compare $result with $nsr_xpc
		if (substr($result,0,length($nsr_xpc)) eq $nsr_xpc) {
			$success{$opr_nm}++;
			printf STDERR ("ok\n");
			&verbosity("\n\tPASSED Alphabetic output: [$opr_nm]:\n");
		} else {
			&failed($nsr_xpc);
			&verbosity("\n\tFAILED (expected: $nsr_xpc vs result: $result) Alphabetic output: [$opr_nm]\n");
		}
	}  else {  # No result at all?
		&failed();
		&verbosity("\nFAILED - No result from [$opr_nm]\n");
	}
	@tst_cmd=(); # Clear test
	print Total $totbenchmarks{$opr_nm}
} # end go()

sub perform_tests
{
# Tests are in alphabetical order by operator name
    
# The following tests are organized and laid out as follows:
# - $tst_cmd[] holds command lines for each operator being tested
# - $dsc_sng holds test description line
# - $nsr_xpc is string or expression that is correct result of command
# - go() is function which executes each test
    
####################
#### ncap tests ####
####################
    $opr_nm='ncap';
####################
    $tst_cmd[0]='ncap $omp_flg -O -v -S ncap.in in.nc foo.nc';
    $dsc_sng='running ncap.in script into nco_tst.pl';
    $nsr_xpc ="ncap: WARNING Replacing missing value data in variable val_half_half";
    &go();

    $tst_cmd[0]='ncap $omp_flg -O -C -v -s "tpt_mod=tpt%273.0f" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%.1f " foo.nc';
    $dsc_sng='Testing float modulo float';
    $nsr_xpc ="0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0";
    &go();
    
    $tst_cmd[0]='ncap $omp_flg -O -C -v -s "foo=log(e_flt)^1" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%.6f\n" foo.nc';
    $dsc_sng='Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)';
    $nsr_xpc ="1.000000";
    &go();
    
    $tst_cmd[0]='ncap $omp_flg -O -C -v -s "foo=log(e_dbl)^1" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%.12f\n" foo.nc';
    $dsc_sng='Testing foo=log(e_dbl)^1';
    $nsr_xpc ="1.000000000000";
    &go();
    
    $tst_cmd[0]='ncap $omp_flg -O -C -v -s "foo=4*atan(1)" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%.12f\n" foo.nc';
    $dsc_sng='Testing foo=4*atan(1)';
    $nsr_xpc ="3.141592741013";
    &go();

    $tst_cmd[0]='ncap $omp_flg -O -C -v -s "foo=erf(1)" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%.12f\n" foo.nc';
    $dsc_sng='Testing foo=erf(1)';
    $nsr_xpc ="0.842701";
    &go();

    $tst_cmd[0]='ncap $omp_flg -O -C -v -s "foo=gamma(0.5)" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%.12f\n" foo.nc';
    $dsc_sng='Testing foo=gamma(0.5)';
    $nsr_xpc ="1.772453851";
    &go();
    
    $tst_cmd[0]='ncap $omp_flg -O -C -v -s "pi=4*atan(1);foo=sin(pi/2)" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -v foo -s "%.12f\n" foo.nc';
    $dsc_sng='Testing foo=sin(pi/2)';
    $nsr_xpc ="1.000000000000";
    &go();

    $tst_cmd[0]='ncap $omp_flg -O -C -v -s "pi=4*atan(1);foo=cos(pi)" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -v foo -s "%.12f\n" foo.nc';
    $dsc_sng='Testing foo=cos(pi)';
    $nsr_xpc ="-1.000000000000";
    &go();

####################
#### ncatted tests #
####################
    $opr_nm='ncatted';
####################
    $tst_cmd[0]='ncatted $omp_flg -O -a units,,m,c,"meter second-1" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%s" -v lev foo.nc | grep units | cut -d " " -f 11-12';
    $dsc_sng='Modify all existing units attributes to meter second-1';
    $nsr_xpc="meter second-1";
    &go();
    
####################
#### ncbo tests ####
####################
    $opr_nm='ncbo';
####################
    $tst_cmd[0]='ncbo $omp_flg -O --op_typ="-" -v mss_val_scl in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v mss_val_scl foo.nc';
    $dsc_sng='difference scalar missing value';
    $nsr_xpc= 1.0e36 ; 
    &go();
    
    $tst_cmd[0]='ncbo $omp_flg -O --op_typ="-" -d lon,1 -v mss_val in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v mss_val foo.nc';
    $dsc_sng='difference with missing value attribute';
    $nsr_xpc= 1.0e36 ; 
    &go();
    
    $tst_cmd[0]='ncbo $omp_flg -O --op_typ="-" -d lon,0 -v no_mss_val in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v no_mss_val foo.nc';
    $dsc_sng='difference without missing value attribute';
    $nsr_xpc= 0 ; 
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -v mss_val_fst in.nc foo.nc';
    $tst_cmd[1]='ncrename $omp_flg -O -v mss_val_fst,mss_val foo.nc';
    $tst_cmd[2]='ncbo $omp_flg -O -y '-' -v mss_val foo.nc in.nc foo.nc';
    $tst_cmd[3]='ncks -C -H -s "%f," -v mss_val foo.nc';
    $dsc_sng='missing_values differ between files';
    $nsr_xpc= "-999,-999,-999,-999" ; 
    &go();
    
    $tst_cmd[0]='ncdiff $omp_flg -O -d lon,1 -v mss_val in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v mss_val foo.nc';
    $dsc_sng='ncdiff symbolically linked to ncbo';
    $nsr_xpc= 1.0e36 ; 
    &go();
    
    $tst_cmd[0]='ncdiff $omp_flg -O -d lon,1 -v mss_val in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v mss_val foo.nc';
    $dsc_sng='difference with missing value attribute';
    $nsr_xpc= 1.0e36 ; 
    &go();
    
    $tst_cmd[0]='ncdiff $omp_flg -O -d lon,0 -v no_mss_val in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v no_mss_val foo.nc';
    $dsc_sng='difference without missing value attribute';
    $nsr_xpc= 0 ; 
    &go();
    
####################
#### ncea tests ####
####################
    $opr_nm='ncea';
####################
    $tst_cmd[0]='ncra -Y ncea $omp_flg -O -v one_dmn_rec_var -d time,4 in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d" -v one_dmn_rec_var foo.nc';
    $dsc_sng='ensemble mean of int across two files';
    $nsr_xpc= 5 ; 
    &go();
    
    $tst_cmd[0]='ncra -Y ncea $omp_flg -O -v rec_var_flt_mss_val_flt -d time,0 in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v rec_var_flt_mss_val_flt foo.nc';
    $dsc_sng='ensemble mean with missing values across two files';
    $nsr_xpc= 1.0e36 ; 
    &go();
    
    $tst_cmd[0]='ncra -Y ncea $omp_flg -O -y min -v rec_var_flt_mss_val_dbl -d time,1 in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%e" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='ensemble min of float across two files';
    $nsr_xpc= 2 ; 
    &go();
    
    $tst_cmd[0]='ncra -Y ncea $omp_flg -O -C -v pck in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%e" -v pck foo.nc';
    $dsc_sng='scale factor + add_offset packing/unpacking';
    $nsr_xpc= 3 ; 
    &go();
    
####################
## ncecat tests ####
####################
    $opr_nm='ncecat';
####################
    $tst_cmd[0]='ncks $omp_flg -O -v one in.nc foo1.nc';
    $tst_cmd[1]='ncks -O -v one in.nc foo2.nc';
    $tst_cmd[2]='ncecat $omp_flg -O foo1.nc foo2.nc foo.nc';
    $tst_cmd[3]='ncks -C -H -s "%f, " -v one foo.nc';
    $dsc_sng='concatenate two files containing only scalar variables';
    $nsr_xpc= "1, 1" ; 
    &go();
    
####################
## ncflint tests ###
####################
    $opr_nm='ncflint';
####################
    $tst_cmd[0]='ncflint $omp_flg -O -w 3,-2 -v one in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%e" -v one foo.nc';
    $dsc_sng='identity weighting';
    $nsr_xpc= 1.0 ; 
    &go();
    
    $tst_cmd[0]='ncrename $omp_flg -O -v zero,foo in.nc foo1.nc';
    $tst_cmd[1]='ncrename $omp_flg -O -v one,foo in.nc foo.nc';
    $tst_cmd[2]='ncflint $omp_flg -O -i foo,0.5 -v two foo1.nc foo.nc foo.nc';
    $tst_cmd[3]='ncks -C -H -s "%e" -v two foo.nc';
    $dsc_sng='identity interpolation';
    $nsr_xpc= 2.0 ; 
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -d lon,1 -v mss_val in.nc foo_x.nc';
    $tst_cmd[1]='ncks -O -C -d lon,0 -v mss_val in.nc foo_y.nc';
    $tst_cmd[2]='ncflint $omp_flg -O -w 0.5,0.5 foo_x.nc foo_y.nc foo_xy.nc';
    $tst_cmd[3]='ncflint $omp_flg -O -w 0.5,0.5 foo_y.nc foo_x.nc foo_yx.nc';
    $tst_cmd[4]='ncdiff $omp_flg -O foo_xy.nc foo_yx.nc foo_xymyx.nc';
    $tst_cmd[5]='ncks -C -H -s "%g" -v mss_val foo_xymyx.nc';
    $dsc_sng='switch order of occurrence to test for commutivity';
    $nsr_xpc= 1e+36 ; 
    &go();
    
####################
#### ncks tests ####
####################
    $opr_nm='ncks';
####################
    $tst_cmd[0]='ncks $omp_flg -O -v lat_T42,lon_T42,gw_T42 in.nc foo_T42.nc';
    $tst_cmd[1]='ncrename $omp_flg -O -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon foo_T42.nc';
    $tst_cmd[2]='ncap $omp_flg -O -s "one[lat,lon]=lat*lon*0.0+1.0" -s "zero[lat,lon]=lat*lon*0.0" foo_T42.nc foo_T42.nc';
    $tst_cmd[3]='ncks -C -H -s "%g" -v one -F -d lon,128 -d lat,64 foo_T42.nc';
    $nsr_xpc="1";
    $dsc_sng='Create T42 variable named one, uniformly 1.0 over globe in foo_T42.nc';
    &go();
    $tst_cmd[0]='ncks -C -H -s "%c" -v fl_nm in.nc';
    $dsc_sng='extract filename string';
    $nsr_xpc= "/home/zender/nco/data/in.cdl" ;
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -v lev in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f," -v lev foo.nc';
    $dsc_sng='extract a dimension';
    $nsr_xpc= "100.000000,500.000000,1000.000000" ; 
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -v three_dmn_var in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,3,3 foo.nc';
    $dsc_sng='extract a variable with limits';
    $nsr_xpc= 23;
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -v int_var in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d" -v int_var foo.nc';
    $dsc_sng='extract variable of type NC_INT';
    $nsr_xpc= "10" ;
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -v three_dmn_var -d lat,1,1 -d lev,0,0 -d lev,2,2 -d lon,0,,2 -d lon,1,,2 in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%4.1f," -v three_dmn_var foo.nc';
    $dsc_sng='Multi-slab lat and lon with srd';
    $nsr_xpc= "12.0,13.0,14.0,15.0,20.0,21.0,22.0,23.0";
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0,3 -d lon,1,3 in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%4.1f," -v three_dmn_var foo.nc';
    $dsc_sng='Multi-slab with redundant hyperslabs';
    $nsr_xpc= "20.0,21.0,22.0,23.0";
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0.,,2 -d lon,90.,,2 in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%4.1f," -v three_dmn_var foo.nc';
    $dsc_sng='Multi-slab with coordinates';
    $nsr_xpc= "20.0,21.0,22.0,23.0";
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -v three_dmn_var -d lat,1,1 -d lev,800.,200. -d lon,270.,0. in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%4.1f," -v three_dmn_var foo.nc';
    $dsc_sng='Double-wrapped hyperslab';
    $nsr_xpc= "23.0,20.0,15.0,12.0";
    &go();
    
# $tst_cmd[0]='ncks $omp_flg -O -C -v three_double_dmn -d lon,2,2 -d time,8,8  in.nc foo.nc';
# $tst_cmd[1]='ncks -C -H -s "%f," -v three_double_dmn foo.nc';
# $dsc_sng='Hyperslab of a variable that has two identical dims';
# $nsr_xpc= 59.5;
# &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -d time_udunits,"1999-12-08 12:00:0.0","1999-12-09 00:00:0.0" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%6.0f" -d time_udunits,"1999-12-08 18:00:0.0","1999-12-09 12:00:0.0",2 -v time_udunits in.nc';
    $dsc_sng='dimension slice using UDUnits library (fails without UDUnits library support)';
    $nsr_xpc= 876018;
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -H -v wvl -d wvl,"0.4 micron","0.7 micron" -s "%3.1e" in.nc';
    $dsc_sng='dimension slice using UDUnit conversion (fails without UDUnits library support)';
    $nsr_xpc= 1.0e-06;
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -v "^three_*" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -C -v three foo.nc';
    $dsc_sng='variable wildcards (fails without regex library)';
    $nsr_xpc= 3 ;
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -v "^[a-z]{3}_[a-z]{3}_[a-z]{3,}$" in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d" -C -v val_one_int foo.nc';
    $dsc_sng='variable wildcards (fails without regex library)';
    $nsr_xpc= 1;
    &go();
    
    $tst_cmd[0]='ncks $omp_flg -O -C -d time,0,1 -v time in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -C -d time,2, foo.nc';
    $dsc_sng='Offset past end of file';
    $nsr_xpc='ncks: ERROR User-specified dimension index range 2 <= time <=  does not fall within valid dimension index range 0 <= time <= 1';
    &go();
    
    $tst_cmd[0]='ncks -C -H -s "%d" -v byte_var in.nc';
    $dsc_sng='Print byte value';
    $nsr_xpc= 122 ;
    &go();
    
####################
#### ncpdq tests ####
####################
    $opr_nm='ncpdq';
####################
    $tst_cmd[0]='ncpdq $omp_flg -O -a -lat -v lat in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v lat -d lat,0 foo.nc';
    $dsc_sng='reverse coordinate';
    $nsr_xpc= 90 ; 
    &go();
    
    $tst_cmd[0]='ncpdq $omp_flg -O -a -lat,-lev,-lon -v three_dmn_var in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v three_dmn_var -d lat,0 -d lev,0 -d lon,0 foo.nc';
    $dsc_sng='reverse three dimensional variable';
    $nsr_xpc= 23 ; 
    &go();
    
    $tst_cmd[0]='ncpdq $omp_flg -O -a lon,lat -v three_dmn_var in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v three_dmn_var -d lat,0 -d lev,2 -d lon,3 foo.nc';
    $dsc_sng='re-order three dimensional variable';
    $nsr_xpc= 11 ; 
    &go();
    
    $tst_cmd[0]='ncpdq $omp_flg -O -P all_new -v upk in.nc foo.nc';
    $tst_cmd[1]='ncpdq $omp_flg -O -P upk -v upk foo.nc foo.nc';
    $tst_cmd[2]='ncks -C -H -s "%g" -v upk foo.nc';
    $dsc_sng='Pack and then unpack scalar (uses only add_offset)';
    $nsr_xpc= 3 ; 
    &go();
    
####################
#### ncra tests ####
####################
    $opr_nm='ncra';
####################
    $tst_cmd[0]='ncra $omp_flg -O -v one_dmn_rec_var in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d" -v one_dmn_rec_var foo.nc';
    $dsc_sng='record mean of int across two files';
    $nsr_xpc= 5 ; 
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -v rec_var_flt_mss_val_dbl in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='record mean of float with double missing values';
    $nsr_xpc= 5 ; 
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -v rec_var_flt_mss_val_int in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_int foo.nc';
    $dsc_sng='record mean of float with integer missing values';
    $nsr_xpc= 5 ; 
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -v rec_var_int_mss_val_int in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d" -v rec_var_int_mss_val_int foo.nc';
    $dsc_sng='record mean of integer with integer missing values';
    $nsr_xpc= 5 ; 
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -v rec_var_int_mss_val_flt in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d" -v rec_var_int_mss_val_flt foo.nc';
    $dsc_sng='record mean of integer with float missing values';
    $nsr_xpc= 5 ; 
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -v rec_var_dbl_mss_val_dbl_pck in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v rec_var_dbl_mss_val_dbl_pck foo.nc';
    $dsc_sng='record mean of packed double with double missing values';
    $nsr_xpc= 5 ;
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -v rec_var_dbl_pck in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v rec_var_dbl_pck foo.nc';
    $dsc_sng='record mean of packed double to test precision';
    $nsr_xpc= 100.55 ;
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -v rec_var_flt_pck in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%3.2f" -v rec_var_flt_pck foo.nc';
    $dsc_sng='record mean of packed float to test precision';
    $nsr_xpc= 100.55 ;
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -y avg -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='record mean of float with double missing values across two files';
    $nsr_xpc= 5 ; 
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -y min -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='record min of float with double missing values across two files';
    $nsr_xpc= 2 ; 
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -y max -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='record max of float with double missing values across two files';
    $nsr_xpc= 8 ; 
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -y ttl -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='record ttl of float with double missing values across two files';
    $nsr_xpc= 70 ;
    &go();
    
    $tst_cmd[0]='ncra $omp_flg -O -y rms -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%1.5f" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='record rms of float with double missing values across two files';
    $nsr_xpc= 5.38516 ;
    &go();
    
    $tst_cmd[0]='ncra -Y ncrcat $omp_flg -O -v rec_var_flt_mss_val_dbl in.nc in.nc foo1.nc 2>foo.tst';
    $tst_cmd[1]='ncra $omp_flg -O -y avg -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
    $tst_cmd[2]='ncwa $omp_flg -O -a time foo.nc foo.nc';
    $tst_cmd[3]='ncdiff $omp_flg -O -v rec_var_flt_mss_val_dbl foo1.nc foo.nc foo.nc';
    $tst_cmd[4]='ncra $omp_flg -O -y rms -v rec_var_flt_mss_val_dbl foo.nc foo.nc';
    $tst_cmd[5]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='record sdn of float with double missing values across two files';
    $nsr_xpc= 2 ;
    &go();
    
####################
#### ncwa tests ####
####################
    $opr_nm='ncwa';
####################
    $tst_cmd[0]='ncwa $omp_flg -O -a lat,lon -w gw -d lat,0.0,90.0 foo_T42.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v one foo.nc';
    $dsc_sng='normalize by denominator upper hemisphere';
    $nsr_xpc= 1;
    &go();
    
#${MY_BIN_DIR}/ncwa -n $omp_flg -O -a lat,lon -w gw foo_T42.nc foo.nc';
#$tst_cmd[1]='ncks -C -H -s "%f" -v one foo.nc';
#$dsc_sng='normalize by tally but not weight';
#$nsr_xpc= 0.0312495 ; 
#&go();
#${MY_BIN_DIR}/ncwa -W $omp_flg -O -a lat,lon -w gw foo_T42.nc foo.nc';
#$tst_cmd[1]='ncks -C -H -s "%f" -v one foo.nc';
#$dsc_sng='normalize by weight but not tally';
#$nsr_xpc= 8192 ; 
#&go();
    $tst_cmd[0]='ncwa -N $omp_flg -O -a lat,lon -w gw in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v mask foo.nc';
    $dsc_sng='do not normalize by denominator';
    $nsr_xpc= 50 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -a lon -v mss_val in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f" -v mss_val foo.nc';
    $dsc_sng='average with missing value attribute';
    $nsr_xpc= 73 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -a lon -v no_mss_val in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v no_mss_val foo.nc';
    $dsc_sng='average without missing value attribute';
    $nsr_xpc= 5.0e35 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -v lat -m lat -M 90.0 -T eq -a lat in.nc foo.nc'; 
    $tst_cmd[1]='ncks -C -H -s "%e" -v lat foo.nc';
    $dsc_sng='average masked coordinate';
    $nsr_xpc= 90.0 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -v lat_var -m lat -M 90.0 -T eq -a lat in.nc foo.nc'; 
    $tst_cmd[1]='ncks -C -H -s "%e" -v lat_var foo.nc';
    $dsc_sng='average masked variable';
    $nsr_xpc= 2.0 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -v lev -m lev -M 100.0 -T eq -a lev -w lev_wgt in.nc foo.nc'; 
    $tst_cmd[1]='ncks -C -H -s "%e" -v lev foo.nc';
    $dsc_sng='average masked, weighted coordinate';
    $nsr_xpc= 100.0 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -v lev_var -m lev -M 100.0 -T gt -a lev -w lev_wgt in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%e" -v lev_var foo.nc';
    $dsc_sng='average masked, weighted variable';
    $nsr_xpc= 666.6667 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -v lat -a lat -w gw -d lat,0 in.nc foo.nc'; 
    $tst_cmd[1]='ncks -C -H -s "%e" -v lat foo.nc';
    $dsc_sng='weight conforms to var first time';
    $nsr_xpc= -90.0 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -v mss_val_all -a lon -w lon in.nc foo.nc'; 
    $tst_cmd[1]='ncks -C -H -s "%g" -v mss_val_all foo.nc';
    $dsc_sng='average all missing values with weights';
    $nsr_xpc= 1.0e36 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -v val_one_mss -a lat -w wgt_one in.nc foo.nc'; 
    $tst_cmd[1]='ncks -C -H -s "%e" -v val_one_mss foo.nc';
    $dsc_sng='average some missing values with unity weights';
    $nsr_xpc= 1.0 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -v msk_prt_mss_prt -m msk_prt_mss_prt -M 1.0 -T lt -a lon in.nc foo.nc'; 
    $tst_cmd[1]='ncks -C -H -s "%e" -v msk_prt_mss_prt foo.nc';
    $dsc_sng='average masked variable with some missing values';
    $nsr_xpc= 0.5 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y min -v rec_var_flt_mss_val_dbl in.nc foo.nc 2>foo.tst';
    $tst_cmd[1]='ncks -C -H -s "%e" -v rec_var_flt_mss_val_dbl foo.nc';
    $dsc_sng='min switch on type double, some missing values';
    $nsr_xpc= 2 ; 
    &go();
    
    $tst_cmd[0]='ncwa  $omp_flg -O -y min -v three_dmn_var_dbl -a lon in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f," -v three_dmn_var_dbl foo.nc >foo';
    $tst_cmd[2]='cut -d, -f 7 foo';
    $dsc_sng='Dimension reduction with min switch and missing values';
    $nsr_xpc= -99 ;
    &go();
    $tst_cmd[0]='cut -d, -f 20 foo';
    $dsc_sng='Dimension reduction with min switch ';
    $nsr_xpc= 77 ;
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y min -v three_dmn_var_int -a lon in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d," -v three_dmn_var_int foo.nc >foo';
    $tst_cmd[2]='cut -d, -f 5 foo';
    $dsc_sng='Dimension reduction on type int with min switch and missing values';
    $nsr_xpc= -99 ;
    &go();
    $tst_cmd[0]='cut -d, -f 7 foo';
    $dsc_sng='Dimension reduction on type int variable';
    $nsr_xpc= 25 ;
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y min -v three_dmn_var_sht -a lon in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d," -v three_dmn_var_sht foo.nc >foo';
    $tst_cmd[2]='cut -d, -f 20 foo';
    $dsc_sng='Dimension reduction on type short variable with min switch and missing values';
    $nsr_xpc= -99 ;
    &go();
    $tst_cmd[0]='cut -d, -f 8 foo';
    $dsc_sng='Dimension reduction on type short variable';
    $nsr_xpc= 29 ;
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y min -v three_dmn_rec_var in.nc foo.nc 2>foo.tst';
    $tst_cmd[1]='ncks -C -H -s "%f" -v three_dmn_rec_var foo.nc';
    $dsc_sng='Dimension reduction with min flag on type float variable';
    $nsr_xpc= 1 ;
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y max -v four_dmn_rec_var in.nc foo.nc 2> foo.tst';
    $tst_cmd[1]='ncks -C -H -s "%f" -v four_dmn_rec_var foo.nc';
    $dsc_sng='Max flag on type float variable';
    $nsr_xpc= 240 ;
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y max -v three_dmn_var_dbl -a lat,lon in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%f," -v three_dmn_var_dbl foo.nc >foo';
    $tst_cmd[2]='cut -d, -f 4 foo';
    $dsc_sng='Dimension reduction on type double variable with max switch and missing values';
    $nsr_xpc= -99 ;
    &go();
    $tst_cmd[0]='cut -d, -f 5 foo';
    $dsc_sng='Dimension reduction on type double variable';
    $nsr_xpc= 40 ;
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y max -v three_dmn_var_int -a lat in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d," -v three_dmn_var_int foo.nc >foo';
    $tst_cmd[2]='cut -d, -f 9 foo';
    $dsc_sng='Dimension reduction on type int variable with min switch and missing values';
    $nsr_xpc= -99 ;
    &go();
    $tst_cmd[0]='cut -d, -f 13 foo';
    $dsc_sng='Dimension reduction on type int variable';
    $nsr_xpc= 29 ;
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y max -v three_dmn_var_sht -a lat in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%d," -v three_dmn_var_sht foo.nc >foo';
    $tst_cmd[2]='cut -d, -f 37 foo';
    $dsc_sng='Dimension reduction on type short variable with max switch and missing values';
    $nsr_xpc= -99 ;
    &go();
    $tst_cmd[0]='cut -d, -f 33 foo';
    $dsc_sng='Dimension reduction on type short, max switch variable';
    $nsr_xpc= 69 ;
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y rms -w lat_wgt -v lat in.nc foo.nc 2>foo.tst';
    $tst_cmd[1]='ncks -C -H -s "%f" -v lat foo.nc';
    $dsc_sng='rms with weights';
    $nsr_xpc= 90 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -w val_half_half -v val_one_one_int in.nc foo.nc 2> foo.tst';
    $tst_cmd[1]='ncks -C -H -s "%ld" -v val_one_one_int foo.nc';
    $dsc_sng='weights would cause SIGFPE without dbl_prc patch';
    $nsr_xpc= 1 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y avg -v val_max_max_sht in.nc foo.nc 2> foo.tst';
    $tst_cmd[1]='ncks -C -H -s "%d" -v val_max_max_sht foo.nc';
    $dsc_sng='avg would overflow without dbl_prc patch';
    $nsr_xpc= 17000 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y ttl -v val_max_max_sht in.nc foo.nc 2> foo.tst';
    $tst_cmd[1]='ncks -C -H -s "%d" -v val_max_max_sht foo.nc';
    $dsc_sng='ttl would overflow without dbl_prc patch, wraps anyway so exact value not important (failure on expected/OK on Xeon chips because of different wrap behavior)';
    $nsr_xpc= -31536 ; # Expected on Pentium IV GCC Debian 3.4.3-13, PowerPC xlc
#    $nsr_xpc= -32768 ; # Expected on Xeon GCC Fedora 3.4.2-6.fc3
    &go();

    $tst_cmd[0]='ncwa $omp_flg -O -y min -a lat -v lat -w gw in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v lat foo.nc';
    $dsc_sng='min with weights';
    $nsr_xpc= -900 ; 
    &go();
    
    $tst_cmd[0]='ncwa $omp_flg -O -y max -a lat -v lat -w gw in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%g" -v lat foo.nc';
    $dsc_sng='max with weights';
    $nsr_xpc= 900 ; 
    &go();
    
####################
##### net tests ####
####################
    $opr_nm='net';
####################
    $tst_cmd[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
    $tst_cmd[1]='ncks $omp_flg -O -v one -p ftp://dust.ess.uci.edu/pub/zender/nco -l ./ in.nc foo.nc';
    $tst_cmd[2]='ncks -C -H -s "%e" -v one foo.nc';
    $tst_cmd[3]='mv in_tmp.nc in.nc';
    $dsc_sng='nco 1: FTP protocol (fails if unable to anonymous FTP to dust.ess.uci.edu)';
    $nsr_xpc= 1;
    &go();
    $tst_cmd[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
    $tst_cmd[1]='ncks $omp_flg -O -v one -p goldhill.cgd.ucar.edu:/home/zender/nco/data -l ./ in.nc foo.nc';
    $tst_cmd[2]='ncks -C -H -s "%e" -v one foo.nc';
    $tst_cmd[3]='mv in_tmp.nc in.nc';
    $dsc_sng='nco 2: scp/rcp protocol(fails if no SSH/RSH access to goldhill.cgd.ucar.edu)';
    $nsr_xpc= 1;
    &go();
    
    $tst_cmd[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
    $tst_cmd[0]='ncks $omp_flg -O -v one -p mss:/ZENDER/nc -l ./ in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%e" -v one foo.nc';
    $tst_cmd[3]='mv in_tmp.nc in.nc';
    $dsc_sng='nco 3: msrcp protocol(fails if not at NCAR)';
    $nsr_xpc= 1; 
    &go();
    $tst_cmd[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
    $tst_cmd[0]='ncks $omp_flg -O -v one -p http://dust.ess.uci.edu/pub/zender/nco -l ./ in.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%e" -v one foo.nc';
    $tst_cmd[3]='mv in_tmp.nc in.nc';
    $dsc_sng='nco 4: HTTP protocol (Will always fail until HTTP implemented in NCO) ';
    $nsr_xpc= 1; 
    &go();
    
    $tst_cmd[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
    $tst_cmd[0]='ncks -C -d lon,0 -v lon -l ./ -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc foo.nc';
    $tst_cmd[1]='ncks -C -H -s "%e" -v lon foo.nc';
    $tst_cmd[3]='mv in_tmp.nc in.nc';
    $dsc_sng='nco 5: HTTP/DODS protocol (fails if not compiled on Linux with make DODS=Y)';
    $nsr_xpc= 0;
    &go();
} # end of perform_test()

# usage - informational blurb for script
sub usage {
    print << 'USAGE';
    
Usage:
nco_bm.pl (options) [list of operators to test from the following list]

ncap ncatted ncbo ncflint ncea ncecat
ncks ncpdq ncra ncrcat ncrename ncwa          (default tests all)

where (options) are:
    --usage || -h ...dumps this help
    --debug {1-3) ...puts the script into debug mode; emits more and (hopefully)
                     more useful info.
    --dods {DODS url} ...do the tests with files retrieved from DODS rather than with 
	                       local files.
    --log ..........requests that the debug info is logged to 'nctest.log'
                     as well as spat to STDOUT.
    --udpreport.....requests that the test results are communicated back to
                     NCO Central to add your test, timing, and build results.
                            NB: This option uses udp port 29659 and may set off
                            firewall alarms if used unless that port is open.
    --test_files....requests the testing & excercise of the file creation script
                     'ncgen' and the Left Hand Casting ability of ncap.  
                            Currently gives the option to test 4 files of increasing
                            size and complexity:
                            0 - none; skip this section
                            1 - example Gene Expression  ~ 50MB  ~few sec
                            2 - small Satellite data set ~100MB  ~several sec
                            3 - 5km Satellite data set   ~300MB  ~min
                            4 - IPCC Daily T85 data set  ~  4GB  ~several min
                            A - All 
    --thr_nbr{#>2}....sets the # of threads to use in the benchmarks, if running them.
	                 has no effect on regression tests (yet) 
    --regress.......do the regression tests
    --benchmark.....do the benchmarks

nco_bm.pl is a semi-automated script for testing the accuracy and
robustness of the nco (netCDF Operators), typically after they are
built, using the 'make benchmark' command.  In this mode, a user should
never have to see this message, so this is all I'll say about it.

In nco debug/testing  mode, it tries to validate the NCO's for both
accuracy and robustness.  It also can collect benchmark statistics via
sending test results to a UDP server.

NB: When adding tests, be sure to use $omp_flg -O to overwrite files.
Otherwise, script hangs waiting for interactive response to
overwrite queries.

This script is part of the netCDF Operators package:
  http://nco.sourceforge.net

Copyright © 1994-2005 Charlie 'my surname is' Zender (surname@uci.edu)

USAGE
exit(0);
} # end of usage()
    
####################
    sub initialize
    {
	# list below enumerates the nco's to be tested; does not set up the tests for the
	# operators.
	@opr_lst_all = qw( ncap ncatted ncbo ncflint ncea ncecat ncks ncpdq ncra ncrcat ncrename ncwa);
	if (scalar @ARGV > 0) 
	{
	    @opr_lst=@ARGV;
	} else {
	    @opr_lst=@opr_lst_all;
	}
	if (defined $ENV{MY_BIN_DIR})
	{
	    $MY_BIN_DIR=$ENV{MY_BIN_DIR};
	} else {
	    # set and verify MY_BIN_DIR
	    $MY_BIN_DIR = abs_path("../src/nco");
	    print "MY_BIN_DIR not specified, use $MY_BIN_DIR? ('y' or specify) ";
	    my $ans = <STDIN>;
	    chomp $ans;
	    $MY_BIN_DIR = $ans unless (lc($ans) eq "y" ||lc($ans) eq "");
	}
	# Die if this path still does not work
	die "$MY_BIN_DIR/$opr_lst[0] doesn't exist\n" unless (-e "$MY_BIN_DIR/$opr_lst[0]");
	
	# create symbolic links for testing
	# if shared libraries were created, then the real executables are
	# in src/nco/.libs, so point to them instead
	my $dotlib = "";
	$dotlib = ".libs/lt-" if `head -1 $MY_BIN_DIR/ncatted` =~ m/sh/;
        $sym_link{ncdiff}=$dotlib . "ncbo";
	$sym_link{ncea}=$dotlib . "ncra";
	$sym_link{ncrcat}=$dotlib . "ncra";
	foreach(keys %sym_link) {
	    system("cd $MY_BIN_DIR && ln -s -f $sym_link{$_} $_ || (rm -f $_ && ln -s -f $sym_link{$_} $_)");
	}
	
# Go to data directory where tests are actually run
	my $data_dir = "../data";
	
	chdir $data_dir or die "$!\n";
	
	# Make sure in.nc exists, make it if possible, or die
	unless (-e "in.nc") {
	    system("ncgen -o in.nc in.cdl") if (`which ncgen` and -e "in.cdl");
	}
	
	die "The netCDF file \"in.nc\" is necessary for testing NCO, however, it could not be found in \"$data_dir\".  Also, it could not be generated because \"ncgen\" could not be found in your path and/or the file \"$data_dir/in.cdl\" does not exist.\n"
	    unless (-e "in.nc");
	
	# Initialize some hashes for each operator that will be tested
	foreach(@opr_lst) 
	{
	    $tst_nbr{$_}=0;
	    $success{$_}=0;
	    $failure{$_}=0;
	}
#  print "***NCO Test Suite***\n";
    } # end of initialize()

####################
sub verbosity {
    my $ts = shift;
    if($dbg_lvl > 0){printf ("$ts");}
    if ($wnt_log) {printf (LOG "$ts");}
} # end of verbosity()

####################

####################
sub failed {
    my $nsr_xpc = shift;
    
    $failure{$opr_nm}++;
    print "FAILED\n$opr_nm failure: $dsc_sng\n";
    foreach(@tst_cmd) {
	print "\t$_\n";
    }
    
    if ($nsr_xpc) {
	print "\tResult: [$result] != Expected: [$nsr_xpc]\n" ;
    } else {
	print "\tproduced no results\n";
    }
    print "\t-----------\n";
    return;
}

####################
sub smrz_fl_cr8_rslt {
    if ($dbg_lvl > 0) { print "Summarizing results of file creation\n";}
    my $CC = `../src/nco/ncks --compiler`;
    my $CCinfo = "";
    if ($CC =~ /gcc/) {$CCinfo = `gcc --version |grep -i gcc`;}
    elsif ($CC =~ /xlc/) {$CCinfo = "xlc version ??";}
    elsif ($CC =~ /icc/) {$CCinfo = "Intel C Compiler version ??";}
    
    my $reportstr = "";
    my $udp_dat = "";
    my $idstring = `uname -a` . "using: " . $CCinfo; chomp $idstring;
    $udp_dat .= $idstring . "|";
    $reportstr .= "\n\nNCO Test Result Summary for:\n$idstring\n";
    $reportstr .=  "      Test                       Total Wallclock Time (s) \n";
    $reportstr .=  "=====================================================\n";
    
    for (my $i=0; $i<$NUM_FLS; $i++) {
	$reportstr .= sprintf "Creating   %15s:           %6.4f \n", $fl_tmg[$i][0], $fl_tmg[$i][1];
	$reportstr .= sprintf "Populating %15s:           %6.4f \n", $fl_tmg[$i][0], $fl_tmg[$i][2];
	$udp_dat   .= sprintf "%s,%6.4f,%6.4f:",$fl_tmg[$i][0], $fl_tmg[$i][1], $fl_tmg[$i][2];
    }
    $reportstr .= sprintf "\n\n";
    $udp_dat   .= sprintf "@";
    
    print $reportstr;
    if ($udp_reprt) { 
	$sock->send($udp_dat); 
	if ($dbg_lvl > 0) { print "File Creation: udp stream sent:\n$udp_dat\n";}
    } # and send it back separately
} # end of smrz_fl_cr8_rslt 

sub smrz_bm_rslt {
    
} # end of sub smrz_bm_rslt

sub smrz_rgr_rslt {

	my $CC = `../src/nco/ncks --compiler`;
	my $CCinfo = "";
	if ($CC =~ /gcc/) {$CCinfo = `gcc --version |grep -i gcc`;}
	elsif ($CC =~ /xlc/) {$CCinfo = "xlc version ??";}
	elsif ($CC =~ /icc/) {$CCinfo = "Intel C Compiler version ??";}
  
	my $reportstr = "\n      Test   Success    Failure   Total       Time";
	if ($thr_nbr > 1 ) {$reportstr .= "   (OMP threads = $thr_nbr)\n";}
	else {$reportstr .= "\n";}
	
	my $udp_dat = "";
	my $idstring = `uname -a` . "using: " . $CCinfo; chomp $idstring;
	$udp_dat .= $idstring . "|" . $cmd_ln . "|";
    
	foreach(@opr_lst) {
		my $total = $success{$_}+$failure{$_};
		my $fal_cnt = "";
		if ($failure{$_} == 0){	$fal_cnt = "   "; }
		else {$fal_cnt = sprintf "%3d", $failure{$_};}
		#printf "$_:\tsuccess: $success{$_} of $total\n";
		if ($total > 0) {
			$reportstr .= sprintf "%10s:      %3d        %3s     %3d     %6.4f \n", $_, $success{$_},  $fal_cnt, $total, $totbenchmarks{$_};
			$udp_dat   .= sprintf "%s,%3d,%3d,%6.4f:",$_, $success{$_}, $total, $totbenchmarks{$_};
		}
	}
	$reportstr .= sprintf "\nNB: Time in WALLCLOCK seconds, not CPU time; see instantaneous measurements for user & system CPU usage.\n";
	chdir "../bld";
	if ($dbg_lvl == 0) {print $reportstr;}
	else { &verbosity($reportstr); }
	$udp_dat .= "@";  # use an infrequent char as separator token
	if ($udp_reprt) { 
		$sock->send($udp_dat); 
		if ($dbg_lvl > 0) { print "Regression: udp stream sent:\n$udp_dat\n";}
	}
} # end of sub smrz_rgr_rslt

sub set_dat_dir {
    my $tmp;
    my $datadir;
#	umask 0000;
    my $umask = umask;
    if ($que == 0) {
	# all this interactive stuff has to be ripped out or made bypassable 
#	print "your umask is $umask\n\n";
	print "===== Setting up the directory to write the results of the file creation tests.\n";
	print "Note that this is your current mounted disk space usage:\n\n";
	my $dsk_spc = `df`;
	print "$dsk_spc\n";
	print "Based on the free space above, make sure that the test data will be written\nto a filesystem that can hold it or the test will fail.\n\n";
    }
    # does user have a DATA dir defined in his env?  It has to be readable and 
    # writable to be usable for these tests, so if it isn't just bail, with a nasty msg
    if (defined $ENV{'DATA'}) { # then is it readwritable?
	if (-w $ENV{'DATA'} && -r $ENV{'DATA'}) {
	    if ($que == 0) {print "Using your environment variable DATA ($ENV{'DATA'}) as the root DATA dir for this series of tests\n\n";}
	    $dta_dir = "$ENV{'DATA'}/nco_test";
	    mkdir "$dta_dir",0777;
	} else {
	    die "You've defined a DATA dir ($ENV{'DATA'}) that can't be written to or read\nfrom or both - please try again.\n\n";
	}
    } elsif ($que == 0) {
	$tmp = 'notset';
	print "You don't have a DATA dir defined and some of the test files can be several GB. \nWhere would you like to write the test data?  It'll be placed in the indicated dir,\nunder nco_test. \n[$ENV{'HOME'}]  ";
	$tmp = <STDIN>;
	chomp $tmp;
#		print "You entered [$tmp] \n";
	if ($tmp eq "") {
	    $dta_dir = "$ENV{'HOME'}/nco_test";
	    if (-e "$ENV{'HOME'}/nco_test") {
		print "[$ENV{'HOME'}/nco_test] already exists - OK to re-use?\n[N/y]";
		$tmp = <STDIN>;
		chomp $tmp;
		if ($tmp =~ "[nN]" || $tmp eq "") {
		    die "\nFine - decide what to use and start over again - bye!\n";
		} else {
		    print "\n";
		}
	    } else { # have to make it
		print "Making $ENV{'HOME'}/nco_test & continuing\n";
		mkdir "$ENV{'HOME'}/nco_test",0777;
	    }
#			print "I'll be using $dta_dir for your DATA dir\n";
	} else {
	    $dta_dir = "$tmp/nco_test"; 
	    # and now test it
	    if (-w $dta_dir && -r $dta_dir) {
		print "OK - we'll use [$dta_dir] to write to.\n\n";
	    } else { # we'll have to make it
		print "[$dta_dir] doesn't exist - will try to make it.\n";
		"$dta_dir",0777;
#				system "ls -lad $dta_dir";
		if (-w $dta_dir && -r $dta_dir) {
		    print "OK - [$dta_dir] is available to write to\n";
		} else {
		    die "ERROR - [$dta_dir] could not be made - check this and try again.\n\n\n";
		}
	    }
	}
    } else {
	die "You MUST define a DATA environment variable to run this in a queue\n";
    }
} # end of sub set_dat_dir

sub fl_cr8_dat_init {
    for (my $i = 0; $i < $NUM_FLS; $i++) {
	$fl_tmg[$i][1] = $fl_tmg[$i][2] = " omitted "; 
    }
    
    $fl_cr8_dat[0][0] = "example gene expression"; # option descriptor
    $fl_cr8_dat[0][1] = "~50MB";                   # file size
    $fl_cr8_dat[0][2] = $fl_tmg[0][0] = "gne_exp";                 # file name root
    $fl_cr8_dat[0][3] = "\'base[ge_atoms,rep,treat,cell,params]=5.67f\'";
    
    $fl_cr8_dat[1][0] = "small satellite";         # option descriptor
    $fl_cr8_dat[1][1] = "~100MB";                  # file size
    $fl_cr8_dat[1][2] = $fl_tmg[1][0] = "sml_stl";                 # file name root
    $fl_cr8_dat[1][3] = "\'d2_00[lat,lon]=16.37f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=3.8f;\'";
    
    $fl_cr8_dat[2][0] = "5km satellite";           # option descriptor
    $fl_cr8_dat[2][1] = "~300MB";                  # file size
    $fl_cr8_dat[2][2] = $fl_tmg[2][0] = "stl_5km";                 # file name root
    $fl_cr8_dat[2][3] = "\'d2_00[lat,lon]=2.8f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=2.8f;d2_03[lat,lon]=2.8f;d2_04[lat,lon]=2.8f;d2_05[lat,lon]=2.8f;d2_06[lat,lon]=2.8f;d2_07[lat,lon]=2.8f;\'";
    
    $fl_cr8_dat[3][0] = "IPCC Daily";              # option descriptor
    $fl_cr8_dat[3][1] = "~4GB";                    # file size
    $fl_cr8_dat[3][2] = $fl_tmg[3][0] = "ipcc_dly_T85";            # file name root
    $fl_cr8_dat[3][3] = "\'weepy=0.8f;dopey=0.8f;sleepy=0.8f;grouchy=0.8f;sneezy=0.8f;doc=0.8f;wanky=0.8f;skanky=0.8f;d1_00[time]=1.8f;d1_01[time]=1.8f;d1_02[time]=1.8f;d1_03[time]=1.8f;d1_04[time]=1.8f;d1_05[time]=1.8f;d1_06[time]=1.8f;d1_07[time]=1.8f;d2_00[lat,lon]=16.2f;d2_01[lat,lon]=16.2f;d2_02[lat,lon]=16.2f;d2_03[lat,lon]=16.2f;d2_04[lat,lon]=16.2f;d2_05[lat,lon]=16.2f;d2_06[lat,lon]=16.2f;d2_07[lat,lon]=16.2f;d2_08[lat,lon]=16.2f;d2_09[lat,lon]=16.2f;d2_10[lat,lon]=16.2f;d2_11[lat,lon]=16.2f;d2_12[lat,lon]=16.2f;d2_13[lat,lon]=16.2f;d2_14[lat,lon]=16.2f;d2_15[lat,lon]=16.2f;d3_00[time,lat,lon]=64.0f;d3_01[time,lat,lon]=64.0f;d3_02[time,lat,lon]=64.0f;d3_03[time,lat,lon]=64.0f;d3_04[time,lat,lon]=64.0f;d3_05[time,lat,lon]=64.0f;d3_06[time,lat,lon]=64.0f;d3_07[time,lat,lon]=64.0f;d3_08[time,lat,lon]=64.0f;d3_09[time,lat,lon]=64.0f;d3_10[time,lat,lon]=64.0f;d3_11[time,lat,lon]=64.0f;d3_12[time,lat,lon]=64.0f;d3_13[time,lat,lon]=64.0f;d3_14[time,lat,lon]=64.0f;d3_15[time,lat,lon]=64.0f;d3_16[time,lat,lon]=64.0f;d3_17[time,lat,lon]=64.0f;d3_18[time,lat,lon]=64.0f;d3_19[time,lat,lon]=64.0f;d3_20[time,lat,lon]=64.0f;d3_21[time,lat,lon]=64.0f;d3_22[time,lat,lon]=64.0f;d3_23[time,lat,lon]=64.0f;d3_24[time,lat,lon]=64.0f;d3_25[time,lat,lon]=64.0f;d3_26[time,lat,lon]=64.0f;d3_27[time,lat,lon]=64.0f;d3_28[time,lat,lon]=64.0f;d3_29[time,lat,lon]=64.0f;d3_30[time,lat,lon]=64.0f;d3_31[time,lat,lon]=64.0f;d3_32[time,lat,lon]=64.0f;d3_33[time,lat,lon]=64.0f;d3_34[time,lat,lon]=64.0f;d3_35[time,lat,lon]=64.0f;d3_36[time,lat,lon]=64.0f;d3_37[time,lat,lon]=64.0f;d3_38[time,lat,lon]=64.0f;d3_39[time,lat,lon]=64.0f;d3_40[time,lat,lon]=64.0f;d3_41[time,lat,lon]=64.0f;d3_42[time,lat,lon]=64.0f;d3_43[time,lat,lon]=64.0f;d3_44[time,lat,lon]=64.0f;d3_45[time,lat,lon]=64.0f;d3_46[time,lat,lon]=64.0f;d3_47[time,lat,lon]=64.0f;d3_48[time,lat,lon]=64.0f;d3_49[time,lat,lon]=64.0f;d3_50[time,lat,lon]=64.0f;d3_51[time,lat,lon]=64.0f;d3_52[time,lat,lon]=64.0f;d3_53[time,lat,lon]=64.0f;d3_54[time,lat,lon]=64.0f;d3_55[time,lat,lon]=64.0f;d3_56[time,lat,lon]=64.0f;d3_57[time,lat,lon]=64.0f;d3_58[time,lat,lon]=64.0f;d3_59[time,lat,lon]=64.0f;d3_60[time,lat,lon]=64.0f;d3_61[time,lat,lon]=64.0f;d3_62[time,lat,lon]=64.0f;d3_63[time,lat,lon]=64.0f;d4_00[time,lev,lat,lon]=1.1f;d4_01[time,lev,lat,lon]=1.2f;d4_02[time,lev,lat,lon]=1.3f;d4_03[time,lev,lat,lon]=1.4f;d4_04[time,lev,lat,lon]=1.5f;d4_05[time,lev,lat,lon]=1.6f;d4_06[time,lev,lat,lon]=1.7f;d4_07[time,lev,lat,lon]=1.8f;d4_08[time,lev,lat,lon]=1.9f;d4_09[time,lev,lat,lon]=1.11f;d4_10[time,lev,lat,lon]=1.12f;d4_11[time,lev,lat,lon]=1.13f;d4_12[time,lev,lat,lon]=1.14f;d4_13[time,lev,lat,lon]=1.15f;d4_14[time,lev,lat,lon]=1.16f;d4_15[time,lev,lat,lon]=1.17f;d4_16[time,lev,lat,lon]=1.18f;d4_17[time,lev,lat,lon]=1.19f;d4_18[time,lev,lat,lon]=1.21f;d4_19[time,lev,lat,lon]=1.22f;d4_20[time,lev,lat,lon]=1.23f;d4_21[time,lev,lat,lon]=1.24f;d4_22[time,lev,lat,lon]=1.25f;d4_23[time,lev,lat,lon]=1.26f;d4_24[time,lev,lat,lon]=1.27f;d4_25[time,lev,lat,lon]=1.28f;d4_26[time,lev,lat,lon]=1.29f;d4_27[time,lev,lat,lon]=1.312f;d4_28[time,lev,lat,lon]=1.322f;d4_29[time,lev,lat,lon]=1.332f;d4_30[time,lev,lat,lon]=1.342f;d4_31[time,lev,lat,lon]=1.352f;\'";
}; # end of fl_cr8_dat_init()

# tst_hirez just excercises the hirez perl timer to make sure that there's no funny biz going on.
sub tst_hirez{
    for (my $W=0; $W<50; $W++) {
	my $t0;
	if ($hiresfound) {$t0 = [gettimeofday];}
	else {$t0 = time;}
	my $E = 0.0;
	for (my $R=0; $R<999999; $R++) {
	    my $E = $E * 1.788830478347;
	}
	
	my $elapsed;
	if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
	else {$elapsed = time - $t0;}
	print " Run $W - Elapsed time =  $elapsed \n" ; 
    }
    if ($dbg_lvl == 1) {die "that's all folk!!\n";}
}

# fl_cr8 does the file creation to provide populated files for the benchmarks
sub fl_cr8 {
    my $idx = shift;
    my $t0;
    my $elapsed;
    
    my $fl_in = my $fl_out = "$dta_dir/$fl_cr8_dat[$idx][2].nc" ;
    print "==== Creating $fl_cr8_dat[$idx][0] data file from template in [$dta_dir]\n";
    print "Executing : $tmr_app ncgen -b -o $fl_out   $bm_dir/$fl_cr8_dat[$idx][2].cdl\n";
    
    if ($hiresfound) {$t0 = [gettimeofday];}
    else {$t0 = time;}
    # File creation now timed
    system  "$tmr_app ncgen -b -o $fl_out   $bm_dir/$fl_cr8_dat[$idx][2].cdl";
    if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
    else {$elapsed = time - $t0;}
    # log it to common timing array
    $fl_tmg[$idx][0] = "$fl_cr8_dat[$idx][2]"; # name root
    $fl_tmg[$idx][1] = $elapsed; # creation time	
    print "\n==== Populating $fl_out file.\nTiming results:\n";
#	print "the -s script chunk is:\n";
    print "Executing: $tmr_app ncap $omp_flg -O -s $fl_cr8_dat[$idx][3] $fl_in $fl_out\n";
    
    if ($hiresfound) {$t0 = [gettimeofday];}
    else {$t0 = time;}
    system "$tmr_app ncap $omp_flg -O -s $fl_cr8_dat[$idx][3] $fl_in $fl_out";
    if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
    else {$elapsed = time - $t0;}
    
    $fl_tmg[$idx][2] = $elapsed; # population time
    
    
    print "==========================\nEnd of $fl_cr8_dat[$idx][2] section\n==========================\n\n\n";
} # end sub fl_cr8

