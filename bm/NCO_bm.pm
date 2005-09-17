package NCO_bm;
# nco_bm.pm - the library module supporting the nco_bm.pl benchmark and regression tests.
# this module contains the following functions in approximate order of their usage:

#   usage()..............dumps usage text for the utility
#   verbosity()..........small fn() to print to both screen and log
#   fl_cr8_dat_init()....initializes the data used to create the test files
#   fl_cr8().............creates the test files
#   smrz_fl_cr8_rslt()...summarizes the results of the file creation tests
#   set_dat_dir()........figures out where to write output data
#   initialize().........initialization, sets which NCOs are to be tested under different conditions
#   tst_hirez()..........almost ready-to-delete test of the HiRes fn() on opterons
#   go().................takes care of executing both regressions and benchmarks set up in same format
#   smrz_rgr_rslt()......summarizes the results of both regression and benchmark tests
#   check_nco_results()..checks the output via md5/wc validation

# $Header: /data/zender/nco_20150216/nco/bm/NCO_bm.pm,v 1.3 2005-09-17 00:22:26 mangalam Exp $

require 5.6.1 or die "This script requires Perl version >= 5.6.1, stopped";
use English; # WCS96 p. 403 makes incomprehensible Perl errors sort of comprehensible
use Cwd 'abs_path';

use strict;
require Exporter;
our @ISA = qw(Exporter);
#export functions (top) and variables (bottom)
our @EXPORT = qw (
	go
	initialize
	verbosity
	failed
	smrz_rgr_rslt
	set_dat_dir $dta_dir
	usage
	smrz_fl_cr8_rslt
	check_nco_results
	fl_cr8
	fl_cr8_dat_init
	wat4inpt
	dbg_msg

	@fl_cr8_dat @fl_tmg $prefix $opr_nm $opr_sng_mpi $md5 $md5found $bm_dir $mpi_prc $mpi_fke
	$nsr_xpc @tst_cmd %tst_nbr $dbg_lvl $wnt_log $dsc_sng $outfile $fl_pth $tmr_app $fke_prefix
);

use vars qw(
	$dbg_lvl  $dot_fmt  $dot_nbr  $dot_nbr_min  $dot_sng  $dsc_fmt
	$dsc_lng_max  $dsc_sng $fke_prefix $hiresfound  $md5  $mpi_prc  $mpi_prfx
	$MY_BIN_DIR  $nsr_xpc  $opr_fmt  $opr_lng_max  @opr_lst
	@opr_lst_all  @opr_lst_mpi $mpi_fke $opr_nm  $opr_rgr_mpi  $opr_sng_mpi
	$outfile  $prefix  %real_tme  $result  $spc_fmt  $spc_nbr
	$spc_nbr_min  $spc_sng  %subbenchmarks  %success  @sys_tim_arr
	$sys_time  %sys_tme  $timed  %totbenchmarks  @tst_cmd  $tst_fmt
	$tst_id_sng  %tst_nbr  %usr_tme $wnt_log $timestamp
	$bm_dir $caseid $cmd_ln $dta_dir @fl_cr8_dat $fl_pth @fl_tmg $md5found
	%MD5_tbl $nco_D_flg $NUM_FLS $prfxd $que $server_ip $sock $thr_nbr $dbg_sgn $err_sgn
	$tmr_app $udp_rpt %wc_tbl $prfxd $nvr_my_bin_dir $prg_nm $arg_nbr @fl_cr8_dat @fl_tmg
);

print "\nINFO: Testing for required modules\n";
BEGIN {eval "use Time::HiRes qw(usleep ualarm gettimeofday tv_interval)"; $hiresfound = $@ ? 0 : 1}
#$hiresfound = 0;  # uncomment to simulate not found
if ($hiresfound == 0) {
    print "\nOoops! Time::HiRes (needed for accurate timing) not found\nContinuing without timing.";
} else {
    print "\tTime::HiRes ... found.\n";
} # $hiresfound

# print "\$md5 = $md5\n";
# print "\$outfile = $outfile\n";

# BEGIN {eval "use Digest::MD5"; $md5found = $@ ? 0 : 1}
# # $md5found = 0;  # uncomment to simulate no MD5
# if ($md5 == 1) {
# 	if ($md5found == 0) {
# 		print "\nOoops! Digest::MD5 module not found - continuing with simpler error checking\n\n" ;
# 	} else {
# 		print "\tDigest::MD5 ... found.\n";
# 	}
# } else {
# 	print "\tMD5 NOT requested; continuing with ncks checking of single values.\n";
# }

$bm_dir = `pwd`; chomp $bm_dir;
$prefix = '';
$err_sgn = "";
if($dbg_lvl > 3){$nco_D_flg = "-D" .  "$dbg_lvl";}

# Initializations
# Re-constitute commandline
$prg_nm=$0; # $0 is program name Camel p. 136
$cmd_ln = "$0 "; $arg_nbr = @ARGV;
for (my $i=0; $i<$arg_nbr; $i++){ $cmd_ln .= "$ARGV[$i] ";}

# make sure that the $fl_pth gets set to a reasonable defalt
$fl_pth = "$dta_dir";

# # Pass in the MPI prefix if --mpi is set. Should be able to just plug into the commandline
# # MPI'ed nco's: mpirun -np 4 mpncbo etc
# #               MPI prefix <--++--> regular command line
# $nvr_my_bin_dir=$ENV{'MY_BIN_DIR'} ? $ENV{'MY_BIN_DIR'} : '';
# $MY_BIN_DIR = $nvr_my_bin_dir;
#
# # set the $fke_prefix to allow for running the mpnc* as a non-mpi'ed  executable
# $fke_prefix = " $MY_BIN_DIR/mp";
# # $prefix expects to find an regular nco in MY_BIN_DIR
# $prefix = " $MY_BIN_DIR/";
# #  $mpi_prfx will always have the mpirun directive.PLUS the MPI'ed nco
# $mpi_prfx = " mpirun -np $mpi_prc  $MY_BIN_DIR/mp";
# $prfxd = 1; $timed = 1;
#
# dbg_msg(1, "*\$tmr_app = $tmr_app, \$prefix = $prefix,\$mpi_fke = $mpi_fke, \$mpi_prfx = $mpi_prfx");

#wat4inpt(__LINE__,"NCO_bm.pm inits finished.");

# usage - informational blurb for script
sub usage {
    print << 'USAGE';

Usage:
nco_bm.pl (options) [list of operators to test from the following list]

ncap ncatted ncbo ncflint ncea ncecat
ncks ncpdq ncra ncrcat ncrename ncwa net      (default tests all)

where (options) are:
    --usage || -h ...dumps this help
    --debug {1-3) ...puts the script into debug mode; emits more and (hopefully)
                     more useful info.  Also inserts the comparable -D flag into
                     the nco commandline.
    --caseid {short id string}
                     this string can be used to identity and separate results
                     from different runs.
    --dap {OPeNDAP url} ...retrieve test files from OPeNDAP server URL
    --opendap "               "
    --log ..........requests that the debug info is logged to 'nctest.log'
                     as well as spat to STDOUT.
    --mpi {#>0}.....number of MPI processes to spawn (incompatible with --thr_nbr)
    --udpreport.....requests that the test results are communicated back to
                     NCO Central to add your test, timing, and build results.
                            NB: This option uses udp port 29659 and may set off
                            firewall alarms if used unless that port is open.
    --test_files....requests the testing & excercise of the file creation script
                     'ncgen' and the Left Hand Casting ability of ncap.
                            Currently gives the option to test 4 files of increasing
                            size and complexity:
                            0 - none; skip this section
                            1 - example long skinny nc   ~ 50MB  ~few sec
                            2 - small Satellite data set ~100MB  ~several sec
                            3 - 5km Satellite data set   ~300MB  ~min
                            4 - IPCC Daily T85 data set  ~  4GB  ~several min
                            A - All
    --thr_nbr {#>0}....Number of OMP threads to use (incompatible with --mpi)
    --regress.......do the regression tests
    --benchmark.....do the benchmarks

nco_bm.pl is a semi-automated script for testing the accuracy and
robustness of the nco (netCDF Operators), typically after they are
built, using the 'make benchmark' command.  In this mode, a user should
never have to see this message, so this is all I'll say about it.

In nco debug/testing  mode, it tries to validate the NCO's for both
accuracy and robustness.  It also can collect benchmark statistics via
sending test results to a UDP server.

NB: When adding tests, be sure to use -O to overwrite files.
Otherwise, script hangs waiting for interactive response to
overwrite queries. Also, unless history is required, use '-h' to inhibit
appending history to file.

This script is part of the netCDF Operators package:
  http://nco.sourceforge.net

Copyright © 1994-2005 Charlie 'my surname is' Zender (surname@uci.edu)

USAGE
exit(0);
} # end usage()


##
## initializes the NCOs that need to be tested for particular conditions
##

####################
sub initialize($$){
	use vars qw($prg_nm %sym_link %failure);
	my $bch_flg; # [flg] Batch behavior
	my $dbg_lvl; # [flg] Debugging level
	($bch_flg,$dbg_lvl)=@_;
# Enumerate operators to test
	@opr_lst_all = qw( ncap ncdiff ncatted ncbo ncflint ncea ncecat ncks ncpdq ncra ncrcat ncrename ncwa net );
	@opr_lst_mpi = qw( ncbo ncecat ncflint ncpdq ncra ncwa ncpdq ncra);
	$opr_sng_mpi = "ncbo ncdiff ncecat ncflint ncwa ncpdq ncra"; # ncpdq ncra MPI, but fail bench
	$opr_rgr_mpi = "ncbo ncdiff ncecat ncflint ncpdq ncea ncrcat ncra ncwa"; # need all of them for regression

	if (scalar @ARGV > 0){@opr_lst=@ARGV;}else{@opr_lst=@opr_lst_all;}
	if (defined $ENV{'MY_BIN_DIR'} &&  $ENV{'MY_BIN_DIR'} ne ""){$MY_BIN_DIR=$ENV{'MY_BIN_DIR'};}
	else{
# Set and verify MY_BIN_DIR
		$MY_BIN_DIR=abs_path("../src/nco");
		printf "ENV var 'MY_BIN_DIR' not specified, so using default\n";
#		if($bch_flg){
#			die "unable to continue in batch mode without MY_BIN_DIR\n stopped\n";
#		}else{ # !bch_flg
#			$MY_BIN_DIR=abs_path("../src/nco");
#			printf "use $MY_BIN_DIR?\n['y' or specify] ";
#			my $ans = <STDIN>;
#			chomp $ans;
#			$MY_BIN_DIR = $ans unless (lc($ans) eq "y" || lc($ans) eq '');
#		} # !bch_flg
} # !$MY_BIN_DIR

dbg_msg(1,"$prg_nm: initialize() reports:\n\t \$MY_BIN_DIR = $MY_BIN_DIR, \n\t \@opr_lst = @opr_lst\n\t \$opr_sng_mpi = $opr_sng_mpi\n\t \$opr_rgr_mpi = $opr_rgr_mpi\n");

# Die if this path still does not work
	die "$MY_BIN_DIR/$opr_lst[0] does not exist\n stopped" unless (-e "$MY_BIN_DIR/$opr_lst[0]" || $opr_lst[0] eq "net");

# Create symbolic links for testing
# If libtool created shared libraries, then point to real executables
# in ../src/nco/.libs
	my $dotlib = '';
	$dotlib = ".libs/lt-" if `head -1 $MY_BIN_DIR/ncatted` =~ m/sh/;
	$sym_link{ncdiff}=$dotlib . "ncbo";
	$sym_link{ncea}=$dotlib . "ncra";
	$sym_link{ncrcat}=$dotlib . "ncra";
	foreach(keys %sym_link) {
		system("cd $MY_BIN_DIR && ln -s -f $sym_link{$_} $_ || (/bin/rm -f $_ && ln -s -f $sym_link{$_} $_)");
}

# Go to data directory where tests are actually run
	my $data_dir = "../data";
	chdir $data_dir or die "$OS_ERROR\n stopped";

# Make sure in.nc exists, make it if possible, or die
# ncgen is not part of
	unless (-e "in.nc"){
		system("ncgen -o in.nc in.cdl") if (`which ncgen` and -e "in.cdl");
} die "The netCDF file \"in.nc\" is necessary for testing NCO, however, it could not be found in \"$data_dir\".  Also, it could not be generated because \"ncgen\" could not be found in your path and/or the file \"$data_dir/in.cdl\" does not exist.\n stopped" unless (-e "in.nc");

# Initialize hashes for each operator to test
	foreach(@opr_lst) {
		$tst_nbr{$_}=0;
		$success{$_}=0;
		$failure{$_}=0;
}
} # end of initialize()


##
## Ouptut string to either stdout, log, or both
##
####################
sub verbosity {
	my $dbg_lvl = shift;
	my $wnt_log - shift;
	my $ts = shift;
#	my $wnt_log; # why should this be required?
	if($dbg_lvl > 0){printf ("$ts");}
	if($wnt_log) {printf (LOG "$ts");}
} # end of verbosity($dbg_lvl, $wnt_log, informational string to output )

##
## fl_cr8_dat_init() sets up the data for the (currently) 4 files that are created for later tests.
##

sub fl_cr8_dat_init {
	for (my $i = 0; $i < $NUM_FLS; $i++) { $fl_tmg[$i][1] = $fl_tmg[$i][2] = " omitted "; }

#	$fl_cr8_dat[0][0] = "example gene expression"; # option descriptor
#	$fl_cr8_dat[0][1] = "~50MB";                   # file size
#	$fl_cr8_dat[0][2] = $fl_tmg[0][0] = "gne_exp";                 # file name root
#	$fl_cr8_dat[0][3] = "\'base[ge_atoms,rep,treat,cell,params]=5.67f\'";

	$fl_cr8_dat[0][0] = "long skinny file"; # option descriptor
	$fl_cr8_dat[0][1] = "~52MB";                   # file size
	$fl_cr8_dat[0][2] = $fl_tmg[0][0] = "skn_lgs";                 # file name root
	$fl_cr8_dat[0][3] = "\'time[time]=1.0f;hmdty[time]=98.3f;PO2[time]=18.7f;PCO2[time]=1.92f;PN2[time]=77.4f;w_vel[time]=14.8f;w_dir[time]=321.3f;temp[time]=23.5f;lmbda_260[time]=684.2f\'";

	$fl_cr8_dat[1][0] = "small satellite";         # option descriptor
	$fl_cr8_dat[1][1] = "~100MB";                  # file size
	$fl_cr8_dat[1][2] = $fl_tmg[1][0] = "sml_stl";                 # file name root
	$fl_cr8_dat[1][3] = "\'d2_00[lat,lon]=16.37f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=3.8f;\'";

	$fl_cr8_dat[2][0] = "5km satellite";           # option descriptor
	$fl_cr8_dat[2][1] = "~300MB";                  # file size
	$fl_cr8_dat[2][2] = $fl_tmg[2][0] = "stl_5km";                 # file name root
	$fl_cr8_dat[2][3] = "\'weepy=1.23456f;d2_00[lat,lon]=2.8f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=2.8f;d2_03[lat,lon]=2.8f;d2_04[lat,lon]=2.8f;d2_05[lat,lon]=2.8f;d2_06[lat,lon]=2.8f;d2_07[lat,lon]=2.8f;\'";

	$fl_cr8_dat[3][0] = "IPCC Daily";              # option descriptor
	$fl_cr8_dat[3][1] = "~4GB";                    # file size
	$fl_cr8_dat[3][2] = $fl_tmg[3][0] = "ipcc_dly_T85";            # file name root
	$fl_cr8_dat[3][3] = "\'weepy=0.8f;dopey=0.8f;sleepy=0.8f;grouchy=0.8f;sneezy=0.8f;doc=0.8f;wanky=0.8f;skanky=0.8f;d1_00[time]=1.8f;d1_01[time]=1.8f;d1_02[time]=1.8f;d1_03[time]=1.8f;d1_04[time]=1.8f;d1_05[time]=1.8f;d1_06[time]=1.8f;d1_07[time]=1.8f;d2_00[lat,lon]=16.2f;d2_01[lat,lon]=16.2f;d2_02[lat,lon]=16.2f;d2_03[lat,lon]=16.2f;d2_04[lat,lon]=16.2f;d2_05[lat,lon]=16.2f;d2_06[lat,lon]=16.2f;d2_07[lat,lon]=16.2f;d2_08[lat,lon]=16.2f;d2_09[lat,lon]=16.2f;d2_10[lat,lon]=16.2f;d2_11[lat,lon]=16.2f;d2_12[lat,lon]=16.2f;d2_13[lat,lon]=16.2f;d2_14[lat,lon]=16.2f;d2_15[lat,lon]=16.2f;d3_00[time,lat,lon]=64.0f;d3_01[time,lat,lon]=64.0f;d3_02[time,lat,lon]=64.0f;d3_03[time,lat,lon]=64.0f;d3_04[time,lat,lon]=64.0f;d3_05[time,lat,lon]=64.0f;d3_06[time,lat,lon]=64.0f;d3_07[time,lat,lon]=64.0f;d3_08[time,lat,lon]=64.0f;d3_09[time,lat,lon]=64.0f;d3_10[time,lat,lon]=64.0f;d3_11[time,lat,lon]=64.0f;d3_12[time,lat,lon]=64.0f;d3_13[time,lat,lon]=64.0f;d3_14[time,lat,lon]=64.0f;d3_15[time,lat,lon]=64.0f;d3_16[time,lat,lon]=64.0f;d3_17[time,lat,lon]=64.0f;d3_18[time,lat,lon]=64.0f;d3_19[time,lat,lon]=64.0f;d3_20[time,lat,lon]=64.0f;d3_21[time,lat,lon]=64.0f;d3_22[time,lat,lon]=64.0f;d3_23[time,lat,lon]=64.0f;d3_24[time,lat,lon]=64.0f;d3_25[time,lat,lon]=64.0f;d3_26[time,lat,lon]=64.0f;d3_27[time,lat,lon]=64.0f;d3_28[time,lat,lon]=64.0f;d3_29[time,lat,lon]=64.0f;d3_30[time,lat,lon]=64.0f;d3_31[time,lat,lon]=64.0f;d3_32[time,lat,lon]=64.0f;d3_33[time,lat,lon]=64.0f;d3_34[time,lat,lon]=64.0f;d3_35[time,lat,lon]=64.0f;d3_36[time,lat,lon]=64.0f;d3_37[time,lat,lon]=64.0f;d3_38[time,lat,lon]=64.0f;d3_39[time,lat,lon]=64.0f;d3_40[time,lat,lon]=64.0f;d3_41[time,lat,lon]=64.0f;d3_42[time,lat,lon]=64.0f;d3_43[time,lat,lon]=64.0f;d3_44[time,lat,lon]=64.0f;d3_45[time,lat,lon]=64.0f;d3_46[time,lat,lon]=64.0f;d3_47[time,lat,lon]=64.0f;d3_48[time,lat,lon]=64.0f;d3_49[time,lat,lon]=64.0f;d3_50[time,lat,lon]=64.0f;d3_51[time,lat,lon]=64.0f;d3_52[time,lat,lon]=64.0f;d3_53[time,lat,lon]=64.0f;d3_54[time,lat,lon]=64.0f;d3_55[time,lat,lon]=64.0f;d3_56[time,lat,lon]=64.0f;d3_57[time,lat,lon]=64.0f;d3_58[time,lat,lon]=64.0f;d3_59[time,lat,lon]=64.0f;d3_60[time,lat,lon]=64.0f;d3_61[time,lat,lon]=64.0f;d3_62[time,lat,lon]=64.0f;d3_63[time,lat,lon]=64.0f;d4_00[time,lev,lat,lon]=1.1f;d4_01[time,lev,lat,lon]=1.2f;d4_02[time,lev,lat,lon]=1.3f;d4_03[time,lev,lat,lon]=1.4f;d4_04[time,lev,lat,lon]=1.5f;d4_05[time,lev,lat,lon]=1.6f;d4_06[time,lev,lat,lon]=1.7f;d4_07[time,lev,lat,lon]=1.8f;d4_08[time,lev,lat,lon]=1.9f;d4_09[time,lev,lat,lon]=1.11f;d4_10[time,lev,lat,lon]=1.12f;d4_11[time,lev,lat,lon]=1.13f;d4_12[time,lev,lat,lon]=1.14f;d4_13[time,lev,lat,lon]=1.15f;d4_14[time,lev,lat,lon]=1.16f;d4_15[time,lev,lat,lon]=1.17f;d4_16[time,lev,lat,lon]=1.18f;d4_17[time,lev,lat,lon]=1.19f;d4_18[time,lev,lat,lon]=1.21f;d4_19[time,lev,lat,lon]=1.22f;d4_20[time,lev,lat,lon]=1.23f;d4_21[time,lev,lat,lon]=1.24f;d4_22[time,lev,lat,lon]=1.25f;d4_23[time,lev,lat,lon]=1.26f;d4_24[time,lev,lat,lon]=1.27f;d4_25[time,lev,lat,lon]=1.28f;d4_26[time,lev,lat,lon]=1.29f;d4_27[time,lev,lat,lon]=1.312f;d4_28[time,lev,lat,lon]=1.322f;d4_29[time,lev,lat,lon]=1.332f;d4_30[time,lev,lat,lon]=1.342f;d4_31[time,lev,lat,lon]=1.352f;\'";
}; # end of fl_cr8_dat_init()


##
## fl_cr8 creates files from the CDL templates an populates them for the benchmarks
##

sub fl_cr8 {
#print "fl_cr8: prefix=$prefix\n";
	my $idx = shift;
	my $t0;
	my $elapsed;
	my $fl_in = my $fl_out = "$dta_dir/$fl_cr8_dat[$idx][2].nc" ;
	print "==== Creating $fl_cr8_dat[$idx][0] data file from template in [$dta_dir]\n";
	print "Executing: $tmr_app ncgen -b -o $fl_out $bm_dir/$fl_cr8_dat[$idx][2].cdl\n";
	if ($hiresfound) {$t0 = [gettimeofday];}
	else {$t0 = time;}
# File creation now timed
	system  "$tmr_app ncgen -b -o $fl_out   $bm_dir/$fl_cr8_dat[$idx][2].cdl";
	if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
	else {$elapsed = time - $t0;}
# log it to common timing array
	$fl_tmg[$idx][0] = "$fl_cr8_dat[$idx][2]"; # name root
	$fl_tmg[$idx][1] = $elapsed; # creation time
	if ($idx == 0) { # skn_lgs needs some extra massaging
		if ($dbg_lvl > 0) {print "\nextra steps for skn_lgs - ncecat...\n";}
		system "$prefix/ncecat -O -h                         $fl_in    $fl_out";  # inserts a record dimension
		if ($dbg_lvl > 0) {print "\nncpdq...\n";}
		system "$prefix/ncpdq -O -h -a time,record             $fl_in    $fl_out"; # swaps time and 'record'
		if ($dbg_lvl > 0) {print "\nncwa...\n";}
		system "$prefix/ncwa -O -h -a record   $fl_in    $fl_out"; # renames 'record' out of the way
# now skn_lgs ready for ncap'ing
}
	print "\n==== Populating $fl_out file.\nTiming results:\n";
#print "fl_cr8: prefix = $prefix\n";
	print "Executing: $tmr_app $prefix/ncap -h -O $nco_D_flg -s $fl_cr8_dat[$idx][3] $fl_in $fl_out\n";
	if ($hiresfound) {$t0 = [gettimeofday];}
	else {$t0 = time;}
	system "$tmr_app $prefix/ncap -O -h -s $fl_cr8_dat[$idx][3] $fl_in $fl_out";
	if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
	else {$elapsed = time - $t0;}
	$fl_tmg[$idx][2] = $elapsed; # population time
	print "==========================\nEnd of $fl_cr8_dat[$idx][2] section\n==========================\n\n\n";
} # end sub fl_cr8


##
## Summarizes the timing results of the file creation tests
##

sub smrz_fl_cr8_rslt {
	if ($dbg_lvl > 0){print "Summarizing results of file creation\n";}
	my $CC = `../src/nco/ncks --compiler`;
	my $CCinfo = '';
	if ($CC =~ /gcc/) {$CCinfo = `gcc --version |grep -i gcc`;}
	elsif ($CC =~ /xlc/) {$CCinfo = "xlc version ??";}
	elsif ($CC =~ /icc/) {$CCinfo = "Intel C Compiler version ??";}
	my $reportstr = '';
	my $idstring = `uname -a` . "using: " . $CCinfo; chomp $idstring;
	my $udp_dat = "File Creation | $timestamp | $idstring | ";
	$reportstr .= "\n\nNCO File Creation Test Result Summary: [$timestamp]\n$idstring\n";
	$reportstr .=  "      Test                       Total Wallclock Time (s) \n";
	$reportstr .=  "=====================================================\n";

	for (my $i=0; $i<$NUM_FLS; $i++) {
		$reportstr .= sprintf "Creating   %15s:           %6.4f \n", $fl_tmg[$i][0], $fl_tmg[$i][1];
		$reportstr .= sprintf "Populating %15s:           %6.4f \n", $fl_tmg[$i][0], $fl_tmg[$i][2];
		$udp_dat   .= sprintf "%s : %6.4f : %6.4f",$fl_tmg[$i][0], $fl_tmg[$i][1], $fl_tmg[$i][2];
}
	$reportstr .= sprintf "\n\n";
	print $reportstr;
	if ($udp_rpt) {
		$sock->send($udp_dat);
		if ($dbg_lvl > 0) { print "File Creation: udp stream sent to $server_ip:\n$udp_dat\n";}
} # and send it back separately
} # end of smrz_fl_cr8_rslt

##
## set_dat_dir() tries to answer the question of where to write data
##

sub set_dat_dir {
	my $tmp;
	my $datadir;
	my $umask = umask;
	# does user have a DATA dir defined in his env?  It has to be readable and
# writable to be usable for these tests, so if it isn't just bail, with a nasty msg
	if ($caseid ne "") {
		$caseid =~ s/[^\w]/_/g;
	}

	if (defined $ENV{'DATA'} && $ENV{'DATA'} ne "") { # then is it readwritable?
		if (-w $ENV{'DATA'} && -r $ENV{'DATA'}) {
			if ($que == 0) {print "INFO: Using your environment variable DATA \n\t   [$ENV{'DATA'}]\n\t as the root DATA directory for this series of tests.\n\n";}
			if ($caseid ne "") {
				$dta_dir = "$ENV{'DATA'}/nco_bm/$caseid";
				my $err = `mkdir -p -m0777 $dta_dir`;
				if ($err ne "") {die "mkdir err: $dta_dir\n";}
			} else { # just dump it into nco_bm
				$dta_dir = "$ENV{'DATA'}/nco_bm";
				my $err = `mkdir -p -m0777 $dta_dir`;
				if ($err ne "") {die "mkdir err: $dta_dir\n";}
			}
		} else {
			die "You have defined a DATA dir ($ENV{'DATA'}) that cannot be written to or read\nfrom or both - please try again.\n stopped";
		}
	} elsif ($que == 0) {
		$tmp = 'notset';
		print "You do not have a DATA dir defined and some of the test files can be several GB. \nWhere would you like to write the test data?  It will be placed in the indicated directory,\nunder nco_bm, using the '--caseid' option to set the name of the subdirectory. \n[$ENV{'HOME'}] or specify: ";
		$tmp = <STDIN>;
		chomp $tmp;
		print "You entered [$tmp] \n";
		if ($tmp eq '') {
			$dta_dir = "$ENV{'HOME'}/nco_bm/$caseid";  # if $caseid not set, then it decays to $ENV{'HOME'}/nco_bm/
			if (-e "$dta_dir") {
				print "$dta_dir already exists - OK to re-use?\n[N/y] ";
				$tmp = <STDIN>;
				chomp $tmp;
				if ($tmp =~ "[nN]" || $tmp eq '') {
					die "\nFine - decide what to use and start over again - bye! stopped";
				} else { print "\n";	}
			} else { # have to make it
				print "Making $dta_dir & continuing\n";
				my $err = `mkdir -p -m0777 $dta_dir`;
				if ($err ne "") {die "mkdir err: $dta_dir\n";}
			}
		} else {
			$dta_dir = "$tmp/nco_bm/$caseid";
			# and now test it
			if (-w $dta_dir && -r $dta_dir) {
				print "OK - we will use [$dta_dir] to write to.\n\n";
			} else { # we'll have to make it
				print "[$dta_dir] doesn't exist - will try to make it.\n";
				my $err = `mkdir -p -m0777 $dta_dir`;
				if ($err ne "") {die "mkdir err: $dta_dir\n";}
				if (-w $dta_dir && -r $dta_dir) {
					print "OK - [$dta_dir] is available to write to\n";
				} else {	die "ERROR - [$dta_dir] could not be made - check this and try again.\n stopped";}
			}
		}
	} else { # que != 0
		die "You MUST define a DATA environment variable to run this in a queue\n stopped";
	} # !defined $ENV{'DATA'})
} # end set_dat_dir()


# go() consumes the @tst_cmd array that contains a series of tests and executes them in order
sub go {
	$dbg_sgn = "";
	$err_sgn = "";

	# twiddle the $prefix to allow for running the mpnc* as a non-mpi'ed  executable
	if ($mpi_fke) {$fke_prefix = " $MY_BIN_DIR/mp"; }
	else {         $prefix = " $MY_BIN_DIR";}
	#  $mpi_prfx will always have the mpirun directive.
	$mpi_prfx = " mpirun -np $mpi_prc  $MY_BIN_DIR/mp";
	$prfxd = 1; $timed = 1;

# Perform tests of requested operator; default is all
	if (!defined $tst_nbr{$opr_nm}) {
		@tst_cmd=();  # Clear test array
		# and init the timing hashes
		$real_tme{$opr_nm} = 0;
		$usr_tme{$opr_nm}  = 0;
		$sys_tme{$opr_nm}  = 0;
		return;
	}

	$subbenchmarks{$opr_nm} = 0;
	$tst_nbr{$opr_nm}++;
	my $tst_cmdcnt = 0;
	my $t = 0;
	my $lst_cmd = @tst_cmd;
	$lst_cmd--;

	dbg_msg(4,"\n nsr_xpc = $nsr_xpc\n dbg_lvl = $dbg_lvl\n wnt_log = $wnt_log\n tst_cmd = @tst_cmd");

	&verbosity($dbg_lvl, $wnt_log, "\n\n============ New Test ==================\n");

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
	$tst_id_sng=sprintf("$opr_fmt$spc_fmt",$opr_nm,$spc_sng).sprintf("test %02d: ",$tst_nbr{$opr_nm});
	if($dsc_lng_max-length($dsc_sng)>0){$dot_nbr+=$dsc_lng_max-length($dsc_sng);}
	$dsc_fmt=sprintf('%%.%ds',$dsc_lng_max);
	$dot_fmt=sprintf("%%.%ds",$dot_nbr);
	$tst_fmt="$tst_id_sng$dsc_fmt$dot_fmt";
	printf STDERR ($tst_fmt,$dsc_sng,$dot_sng);
	# csz--
	foreach (@tst_cmd){
		my $md5_chk = 1;
		$dbg_sgn .= "\nDEBUG: Full commandline for part $tst_cmdcnt:\n";

		if ($_ !~ /foo.nc/) {$md5_chk = 0;}
		my $opcnt = 0;
		my $md5_dsc_sng = $dsc_sng . "_$tst_cmdcnt";
		# Add $prefix only to NCO operator commands, not things like 'cut'.


		foreach my $op (@opr_lst_all) {
#			print "\$op = $op\n";
			if ($_ =~ m/^$op/ ) { # if the op is in the main list
				if ($mpi_prc > 0 && $opr_sng_mpi =~ /$op/) { $_ = $tmr_app . $mpi_prfx . $_; } # and in the mpi list
				elsif ($mpi_fke  && $opr_sng_mpi =~ /$op/) { $_ = $tmr_app . $fke_prefix . $_; } # the fake prefix
				else  { $_ = $tmr_app . $prefix . $_; } # the std prefix
				dbg_msg(2, "URGENT:cmdline= $_ \n");
			}
		}

#wat4inpt(__LINE__);

		$dbg_sgn .= "DEBUG:$_\n";


# timing code using Time::HiRes
		my $t0;
		if ($hiresfound) {$t0 = [gettimeofday];}
		else {$t0 = time;}
		#####################################################################################
		# and execute the command, splitting off stderr to file 'nco-stderror'
		$result=`($_) 2> nco-stderror`; # stderr should contain timing info if it exists.
		#####################################################################################
		chomp $result;
		if ($timed) {
			$sys_time = `cat nco-stderror`;
			$sys_time =~ s/\n/ /g;
			@sys_tim_arr = split(" ", $sys_time); # [0]real [1]0.00 [2]user [3]0.00 [4]sys [5]0.00
			$real_tme{$opr_nm} += $sys_tim_arr[1];
			$usr_tme{$opr_nm}  += $sys_tim_arr[3];
			$sys_tme{$opr_nm}  += $sys_tim_arr[5];
		}
		my $elapsed;
		if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
		else {$elapsed = time - $t0;}

		#print "inter benchmark for $opr_nm = $subbenchmarks{$opr_nm} \n";
		$subbenchmarks{$opr_nm} += $elapsed;
#		$tst_idx = $tst_nbr{$opr_nm}-1;
		if($dbg_lvl > 3){print "\t$opr_nm subtest [$t] took $elapsed seconds\n";}
		$dbg_sgn .= "DEBUG: Result = [$result]\n";

		#and here, check results by md5 checksum for each step - insert guts of check_nco_results()
		# have to mod the input string -  suffix with the cycle#
		# follow check only if the MD5 module is present, there's a foo.nc to check ($outfile = 'foo.nc')
		# & non-terminal cmd (the terminal command is ncks which is expected to return a single value or string)
		dbg_msg(3,"check_nco_results(): \$md5 = $md5, \$md5_chk = $md5_chk, \$tst_cmdcnt ($tst_cmdcnt) < \$lst_cmd ($lst_cmd)");
		if ($md5 && $md5_chk && $tst_cmdcnt < $lst_cmd) {
			dbg_msg(2,"Entering check_nco_results() with \$outfile=$outfile");
			check_nco_results($outfile, $md5_dsc_sng);
		}
		if ($md5_chk == 0 && $dbg_lvl > 0) { $dbg_sgn .= "WARN: No MD5/wc check on intermediate file.\n";}

		# else the oldstyle check has already been done and the results are in $result, so process normally
		$tst_cmdcnt++;
		if ($dbg_lvl > 2) {
			print "\ngo: test cycle held - hit <Enter> to continue\n";
			my $wait = <STDIN>;
		}

	} # end loop over sub-tests

	$dbg_sgn .= "DEBUG: Total time for $opr_nm [$tst_nbr{$opr_nm}] = $subbenchmarks{$opr_nm} s\n";
	$totbenchmarks{$opr_nm}+=$subbenchmarks{$opr_nm};

	 # this comparing of the results shouldn't even be necessary as we're validating the whole file,
	 # not just a single value.
	chomp $result;  # Remove trailing newline for easier regex comparison

	# Compare numeric results
	if ($nsr_xpc =~/\d/) { # && it equals the expected value
#if ($dbg_lvl > 1){print STDERR "\$nsr_xpc assumed to be numeric: $nsr_xpc\n\$result = $result\n";}
		$dbg_sgn .= "DEBUG: \$nsr_xpc assumed to be numeric: $nsr_xpc\n\$result = [$result]\n";

		if ($nsr_xpc == $result) {
			$success{$opr_nm}++;
			printf STDERR (" SVn ok\n");
			$dbg_sgn .= "DEBUG: PASSED (Numeric output)\n";
		} elsif (abs($result - $nsr_xpc) < 0.02) {
			$success{$opr_nm}++;
			printf STDERR (" SVn prov. ok\n");
	 		$dbg_sgn .= "DEBUG: PASSED PROVISIONALLY (Numeric output):[$nsr_xpc vs $result]\n";
		} else {
			printf STDERR (" FAILED!\n");
			&failed($nsr_xpc);
			$dbg_sgn .= "DEBUG: !!FAILED (Numeric output) [expected: $nsr_xpc vs result: $result]\n";
		}
	} elsif ($nsr_xpc =~/\D/)  {# Compare non-numeric tests
		dbg_msg(2,"DEBUG: expected value assumed to be alphabetic: $nsr_xpc\n\$result = $result\n");

		# Compare $result with $nsr_xpc
		if (substr($result,0,length($nsr_xpc)) eq $nsr_xpc) {
			$success{$opr_nm}++;
			printf STDERR (" SVa ok\n");
			$dbg_sgn .= "DEBUG: PASSED Alphabetic output";
		} else {
			&failed($nsr_xpc);
			$dbg_sgn .= "DEBUG: !!FAILED Alphabetic output (expected: $nsr_xpc vs result: $result) ";
		}
	}  else {  # No result at all?
		&failed();
		$dbg_sgn .= "DEBUG: !!FAILED - No result from [$opr_nm]\n";
	}
	print $err_sgn;
	if ($dbg_lvl > 0) {print $dbg_sgn;}
	print LOG $dbg_sgn;
	@tst_cmd=(); # Clear test
#	print Total $totbenchmarks{$opr_nm}
} # end go()

####################

####################
sub failed {
	my $nsr_xpc = shift;
	$failure{$opr_nm}++;
	$err_sgn .= "\nERR: FAILURE in $opr_nm failure: $dsc_sng\n";
	foreach(@tst_cmd) { $err_sgn .= "\t$_\n";}
	if ($nsr_xpc) { $err_sgn .= "ERR::EXPLAIN: Result: [$result] != Expected: [$nsr_xpc]\n\n" ; }
	else { $err_sgn .= "ERR::EXPLAIN: command produced no results.\n\n"; }
	return;
}

sub smrz_rgr_rslt {
	my $CC = `$MY_BIN_DIR/ncks --compiler`;
	my $CCinfo = '';
	if ($CC =~ /gcc/) {$CCinfo = `gcc --version |grep -i gcc`;}
	elsif ($CC =~ /xlc/) {$CCinfo = "xlc version ??";}
	elsif ($CC =~ /icc/) {
		my $icc_ver = `icc --version`; chomp $icc_ver;
		$CCinfo = "Intel C Compiler version $icc_ver";
	}
	chomp $CCinfo;
	my $idstring = `uname -a`; chomp($idstring);
	my $reportstr = "\n\n" . $idstring . "; " . $CCinfo . "; " . $timestamp . "\n";
	$reportstr .= "\n                    Test Results                Seconds to complete\n";
	$reportstr .=      "             --------------------------   ----------------------------------------\n";
	$reportstr .=      "      Test   Success    Failure   Total   WallClock    Real   User  System    Diff";
	if ($thr_nbr > 0) {$reportstr .= "   (OMP threads = $thr_nbr)\n";}
	else {$reportstr .= "\n";}
	my $udp_dat = $idstring . " using: " . $CCinfo . "|" . $cmd_ln . "|";
	foreach(@opr_lst) {
		my $total = $success{$_}+$failure{$_};
		my $fal_cnt = '';
		if ($failure{$_} == 0){	$fal_cnt = "   "; }
		else {$fal_cnt = sprintf "%3d", $failure{$_};}
		#printf "$_:\tsuccess: $success{$_} of $total\n";
		if ($total > 0) {
			my $io_tm = $real_tme{$_} - $usr_tme{$_} - $sys_tme{$_};
			$reportstr .= sprintf "%10s:      %3d        %3s     %3d     %6.2f   %6.2f %6.2f  %6.2f  %6.2f\n", $_, $success{$_},  $fal_cnt, $total, $totbenchmarks{$_}, $real_tme{$_}, $usr_tme{$_}, $sys_tme{$_}, $io_tm;

			$udp_dat   .= sprintf "%s %3d %3d %6.2f %6.2f %6.2f %6.2f %6.2f:",$_, $success{$_}, $total, $totbenchmarks{$_},$real_tme{$_}, $usr_tme{$_}, $sys_tme{$_}, $io_tm ;
			# above line uses whitespace sep, with ':' separating test names.
		}
	}
	$reportstr .= sprintf "\nNB:MD5: test passes MD5 checksum on file(s) May be more than one intermediate file.\nSVx: test passes single terminal value check SVn=numeric, SVa=alphabetic\n";
	chdir "../bld";
	if ($dbg_lvl == 0) {print $reportstr;}
	else { &verbosity($dbg_lvl, $wnt_log, $reportstr); }
#	$udp_dat .= "@";  # use an infrequent char as separator token
print "Regression: udp stream sent:\n$udp_dat\n";
	if ($udp_rpt) {
		$sock->send($udp_dat);
		if ($dbg_lvl > 0) { print "Regression: udp stream sent to $server_ip:\n$udp_dat\n";}
	}
} # end of sub smrz_rgr_rslt




sub check_nco_results {
# taken substantially from process_tacg_results (in testtacg.pl), hjm
	dbg_msg(3, "check_nco_results()::\$fl_pth = $fl_pth");
	my $file = shift;  # 1st arg
	my $testtype = shift; # 2nd arg
# 	my $md5found = shift; # 3rd arg
	my $prefix = "$MY_BIN_DIR"; $prfxd = 1; #embed the timer command and local bin in cmd
	my $cmdline = $_;
	my $return_value = $result; # this should be the return value of executing the non-terminal cmds
	my $hash = "";
	my @wc_lst;
	my $wc = "";
   if ($return_value != 0) { # nco.h:131:#define EXIT_SUCCESS 0
		print LOG "NonZero return value = $cmdline\n";
} else {
# 1st do an ncks dump on the 1st 111111 lines (will cause a sig13 due to the head cmd)
		system("$prefix/ncks -P  $file |head -111111  > $fl_pth/wc_out");
		@wc_lst = split(/\s+/, `wc $fl_pth/wc_out`);
		$wc = $wc_lst[1] . " " . $wc_lst[2] . " " . $wc_lst[3];

		# however we can do an md5 fo the entire output file
		if ($md5found == 1) {
			open(FILE, "$file") or die "Can't open $fl_pth/wc_out\n";
			binmode(FILE);
			$hash = Digest::MD5->new->addfile(*FILE)->hexdigest ;
		} else { $hash = "UNDEFINED"; }
		print LOG "\n\"$testtype\" => ", "\"$hash\", #MD5\n\"$testtype\" => ", "\"$wc\", #wc\n$cmdline\n\n";
}
	if ($md5found == 1) {
		if ( $MD5_tbl{$testtype} eq $hash ) { print " MD5"; verbosity " MD5"; }
		else {
			print " MD5 fail,"; verbosity " MD5 fail,";  # test: $testtype\n";
			if ($dbg_lvl > 1) {verbosity "MD5 sig: $hash should be: $MD5_tbl{$testtype}\n";}
			if ($wc eq $wc_tbl{$testtype}) { print "WC PASS "; verbosity "WC PASS "; }
			else { print " WC fail,"; verbosity " WC fail,"; }
			my $errfile = "$file" . ".MD5.err"; # will get overwritten; halt test if want to keep it.
			system("cp $file $errfile");
		}
	} else {
		if ($wc_tbl{$testtype} eq $wc) { print "passed wc \n"; verbosity "passed wc \n"; }
		else {
			print " WC fail,";verbosity " WC fail,";
			my $errfile = "$testtype" . ".wc.err";
			print "\n\ncp $fl_pth/out $fl_pth/$errfile\n\n";
			system("cp $fl_pth/out $fl_pth/$errfile");
		}
	}
	return $hash;
} # end check_nco_results()


sub wat4inpt{
	my $line = shift;
	my $msg = shift;
	if ($msg eq "") {$msg = "no additional info"}
	if ($dbg_lvl > 2) {
		print "\n\n-------------------------------\nExecution halted near line $line - hit a key to continue.\n[$msg]\n-------------------------------\n";
		my $tmp=<STDIN>;
	}
} # end wat4inpt()

sub dbg_msg {
	my $okdbg = shift;
	my $msg = shift;
	if ($dbg_lvl >= $okdbg) {
		print "\nDEBUG[$okdbg]: $msg\n\n";
	}
}



# the following 2 lines are required to provide a 'true' value at the end of the package.
1;
__END__