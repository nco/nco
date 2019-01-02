package NCO_bm;

# $Header$

# Purpose: Library for nco_bm.pl benchmark and regression tests
# Module contains following functions:

# bm_ntl().........initialization, set NCOs to be tested under different conditions
# bm_usg().........dumps usage text
# bm_vrb().........small fnc() to print to both screen and log
# drc_dat_set()....figures out where to write output data
# fl_mk()..........creates test files
# fl_mtd_ntl().....Initialize meta-data used to create test files
# rsl_chk_MD5_wc().checks output via md5/wc validation
# rsl_smr_fl_mk()..summarize results of file creation tests
# rsl_smr_rgr()....summarize results of both regression and benchmark tests
# tst_run()........execute regressions and benchmarks in same format
# tst_tm_hrz().....almost ready-to-delete test of HiRes fnc() on Opterons
# vrs_sng_get()....create two-part release and date string e.g., "3.0.3 / 20051004"

require 5.6.1 or die "This script requires Perl version >= 5.6.1, stopped";
use English; # WCS96 p. 403 makes Perl errors more comprehensible
use Cwd 'abs_path';
#use NCO_benchmarks qw($tw_prt_bm);
use strict;

use NCO_rgr qw(
	       tst_rgr
	       $dodap $pfx_cmd $pfx_cmd_crr $opr_sng_mpi $opr_nm $dsc_sng $prsrv_fl
	       $fl_out
	       );
# $foo1_fl $foo_fl $foo_tst

# module that contains tst_rgr()
#use NCO_benchmarks; #module that contains the actual benchmark code
# use warnings;

require Exporter;
our @ISA = qw(Exporter);
#export functions (top) and variables (bottom)
our @EXPORT = qw (
		  tst_run dbg_msg drc_dat_set bm_ntl
		  $pfx_cmd $pfx_cmd_crr $drc_dat @fl_mtd_sct $opr_sng_mpi $opr_nm $dsc_sng %NCO_RC
		  $prsrv_fl  $srv_sd $hiresfound $dodap $bm $dbg_lvl $sock $udp_rpt
		  $mpi_prc $pfx_mpi $mpi_fk $mpi_upx
		  );

use vars qw(
	    $aix_mpi_nvr_pfx $aix_mpi_sgl_nvr_pfx $bm $dbg_lvl $dodap $dot_fmt
	    $dot_nbr $dot_nbr_min $dot_sng $dsc_fmt $dsc_lng_max $dsc_sng
	    $pfx_fk $hiresfound $md5 $mpi_prc $pfx_mpi $mpi_fk $mpi_upx
        $MY_BIN_DIR %NCO_RC $nsr_xpc
	    $opr_fmt $opr_lng_max @opr_lst @opr_lst_all @opr_lst_mpi
	    $opr_nm $opr_rgr_mpi $opr_sng_mpi $os_nm  $pfx_cmd $pfx_cmd_crr %real_tme
	    $cmd_rsl $spc_fmt $spc_nbr $spc_nbr_min $spc_sng %subbenchmarks %success
	    @sys_tim_arr $sys_time %sys_tme $timed %totbenchmarks @tst_cmd $tst_fmt
	    $tst_id_sng %tst_nbr %usr_tme $wnt_log $timestamp $bm_drc $caseid
	    $cmd_ln $drc_dat @fl_mtd_sct $fl_pth @fl_tmg $md5found %MD5_tbl
	    $nco_D_flg $fl_nbr $pfxd $prsrv_fl $que $server_ip $sock $thr_nbr
	    $dbg_sng $err_sng $tmr_app $udp_rpt %wc_tbl $pfxd $nvr_my_bin_dir
	    $prg_nm $arg_nbr $tw_prt_bm $srv_sd @cmd_lst
	    
	    );
# $fl_out
print "\nINFO: Testing for required modules\n";
BEGIN {eval "use Time::HiRes qw(usleep ualarm gettimeofday tv_interval)"; $hiresfound = $@ ? 0 : 1}
#$hiresfound = 0;  # uncomment to simulate not found
if ($hiresfound == 0) {
    print "\nOoops! Time::HiRes (needed for accurate timing) not found\nContinuing without timing.";
} else {
    print "\tTime::HiRes ... found.\n";
} # $hiresfound

# bm_usg - informational blurb for script
sub bm_usg {
    print << 'USAGE';
    
  Usage:
    nco_bm.pl (options) [list of operators to test from the following list]
	
ncap2 ncatted ncbo ncflint nces ncecat
ncks ncpdq ncra ncrcat ncrename ncwa net      (default tests all)
	
where (options) are:

--usage || -h ...dumps this help
--debug {1-3) ...puts the script into debug mode; emits more and (hopefully)
                 more useful info.  Also inserts the comparable -D flag into
                 the nco commandline.
--caseid {short id string}  this string can be used to identity and separate results from different runs.
--dap {OPeNDAP url} ...retrieve test files from OPeNDAP server URL
--opendap..............ditto.  Both take a URL of the form:
                       http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata
                       (i.e., URL points points thru the cgi-bin to the data dir)
--dust_user.....use this to define a user who CAN log into dust for testing
--fl_fmt........sets the file type to test.  One of:
                classic, 64bit, netcdf4, netcdf4_classic
--log ..........requests that debug info is logged to 'nco_bm.log'
                as well as spat to STDOUT.
--mpi_prc {#>0}..number of MPI processes to spawn
--mpi_fake.......run mpi executable as single process for debugging.
--fake_mpi.......ditto
--udpreport.....requests that the test results are communicated back to
                NCO Central to add your test, timing, and build results.
                NB: This option uses udp port 29659 and may set off
                firewall alarms if used unless that port is open.
--scaling.......Run ncwa benchmarks with 1/2, 1/4, 1/8 of the variables
--serverside {server URL}..requests that benchmarks be run on server side
      		   points to dust.ess.uci.edu unless alternate URL is given [EXPERIMENTAL]
--test_files....Test and excercise file creation script
                'ncgen' and the Left Hand Casting ability of ncap.
                Currently gives the option to test 4 files of increasing size and complexity:
                   0 - none; skip this section
		   1 - example long skinny nc   ~ 50MB  ~few sec
		   2 - small Satellite data set ~100MB  ~several sec
		   3 - 5km Satellite data set   ~300MB  ~min
		   4 - IPCC Daily T85 data set  ~  4GB  ~several min
		   A - All
--thr_nbr {#>0}....Number of OpenMP threads to use
--xdata.........Explicit data path set from command line
                (overrides DATA environment variable)
--regress.......Perform regression tests
--benchmark.....Perform benchmarks

nco_bm.pl is a semi-automated script for testing the accuracy and
robustness of the NCO (netCDF Operators), typically after they are
built, using the 'make benchmark' command.
This script can also collect benchmark statistics via sending 
test results to a UDP server.

NB: When adding tests, use NCO's '-O' switch to overwrite files.
Otherwise, the script hangs waiting for interactive responses.
Also, unless history is required, use '-h' to inhibit
appending history to file.

This script is part of the netCDF Operators package:
http://nco.sf.net

Copyright 1995--present Charlie Zender

USAGE
exit(0);
} # end bm_usg()
# ' <-- to make emacs happy after the USAGE block

# Initialize NCOs that need to be tested for particular conditions
sub bm_ntl($$){
    use vars qw($prg_nm %sym_link %failure);
    my $bch_flg; # [flg] Batch behavior
    my $dbg_lvl; # [flg] Debugging level
    ($bch_flg,$dbg_lvl)=@_;
# Enumerate operators to test
    @opr_lst_all = qw( ncap2 ncdiff ncatted ncbo ncclimo ncflint nces ncecat ncks ncpdq ncra ncrcat ncremap ncrename ncwa net);
    @opr_lst_mpi = qw( ncbo ncecat ncflint ncpdq ncra ncwa ncpdq ncra );
    $opr_sng_mpi = "ncbo ncdiff ncecat ncflint nces ncpdq ncra ncwa "; # ncpdq ncra MPI, but fail bench
    $opr_rgr_mpi = "ncbo ncdiff ncecat ncflint nces ncpdq ncra ncwa ncrcat"; # need all of them for regression
    
    if(scalar @ARGV > 0){@opr_lst=@ARGV;}else{@opr_lst=@opr_lst_all;}
    if(defined $ENV{'MY_BIN_DIR'} && $ENV{'MY_BIN_DIR'} ne ""){$MY_BIN_DIR=$ENV{'MY_BIN_DIR'};
    }else{
# Set and verify MY_BIN_DIR
	$MY_BIN_DIR=abs_path("../src/nco");
	dbg_msg(2, "ENV var 'MY_BIN_DIR' not specified, so using default [$MY_BIN_DIR].");
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
    
    dbg_msg(1,"$prg_nm: bm_ntl() reports:\n\t \$MY_BIN_DIR = $MY_BIN_DIR, \n\t \@opr_lst = @opr_lst\n\t \$opr_sng_mpi = $opr_sng_mpi\n\t \$opr_rgr_mpi = $opr_rgr_mpi\n");
    
# Die if this path still does not work
    if($opr_lst[0] ne 'ncap2'){
	die "$MY_BIN_DIR/$opr_lst[0] does not exist\n stopped" unless (-e "$MY_BIN_DIR/$opr_lst[0]" || $opr_lst[0] eq "net");
    }
    
# Create symbolic links for testing
# If libtool created shared libraries, then point to real executables
# in ../src/nco/.libs
    my $dotlib = '';
# 20170608: Removed at suggestion of Daniel Neumann
#    $dotlib = ".libs/lt-" if `head -1 $MY_BIN_DIR/ncatted` =~ m/sh/;
    $sym_link{ncdiff}=$dotlib . "ncbo";
    $sym_link{nces}=$dotlib . "ncra";
    $sym_link{ncrcat}=$dotlib . "ncra";
    foreach(keys %sym_link){
	system("cd $MY_BIN_DIR && ln -s -f $sym_link{$_} $_ || (/bin/rm -f $_ && ln -s -f $sym_link{$_} $_)");
    }
    
# Go to data directory where tests are actually run
    my $drc_in_nc = "../data";
    chdir $drc_in_nc or die "$OS_ERROR\n stopped";
    
# Make sure in.nc exists, make it if possible, or die
# ncgen is not part of
    unless (-e "in.nc"){
	system("ncgen -o in.nc in.cdl") if (`which ncgen` and -e "in.cdl");
    } die "The netCDF file \"in.nc\" is necessary for testing NCO, however, it could not be found in \"$drc_in_nc\".  Also, it could not be generated because \"ncgen\" could not be found in your path and/or the file \"$drc_in_nc/in.cdl\" does not exist.\n stopped" unless (-e "in.nc");
#create large files openMP testing
    if( !(-e "lrg_bm.nc") && (-e "lrg_bm.in") && (-e "$MY_BIN_DIR/ncap2") ){
# make lrg_bm.nc
	system("$MY_BIN_DIR/ncap2 -C -v -O -t1 -S lrg_bm.in in.nc lrg_bm.nc");
# make lrg_bm1.nc
	system("$MY_BIN_DIR/ncap2 -C -v -O -t1 -S lrg_bm.in lrg_bm.nc lrg_bm1.nc");
    } 
      
    
# Initialize hashes for each operator to test
    foreach(@opr_lst) {
	$tst_nbr{$_}=0;
	$success{$_}=0;
	$failure{$_}=0;
    }
} # end of bm_ntl()

# Output string to either stdout, log, or both
sub bm_vrb {
    my $dbg_lvl = shift;
    my $wnt_log = shift;
    my $ts = shift;
#	my $wnt_log; # why should this be required?
    if($dbg_lvl > 0){printf ("$ts");}
    if($wnt_log) {printf (LOG "$ts");}
} # end of bm_vrb($dbg_lvl, $wnt_log, informational string to output )

sub fl_mtd_ntl {
# Purpose: fl_mtd_ntl() initializes metadata for all test files
    my $fl_nbr=3;
    my $idx_stl_5km=0;
    my $idx_gcm_dly=1;
    my $idx_tms_lng=2;
    dbg_msg(1,"fl_mtd_ntl: \$fl_nbr = $fl_nbr");
    
    if ($dbg_lvl > 2) {
	print "\nWaiting for keypress to proceed.\n";
	my $tmp = <STDIN>;
    }
    
    for (my $i = 0; $i < $fl_nbr; $i++) { $fl_tmg[$i][1] = $fl_tmg[$i][2] = " omitted "; }
    
#	$fl_mtd_sct[$idx_gn_xpr][0] = "example gene expression"; # option descriptor
#	$fl_mtd_sct[$idx_gn_xpr][1] = "~50MB";                   # file size
#	$fl_mtd_sct[$idx_gn_xpr][2] = $fl_tmg[$idx_gn_xpr][0] = "gn_xpr";
#	$fl_mtd_sct[$idx_gn_xpr][3] = "\'base[ge_atoms,rep,treat,cell,params]=5.67f\'";
    
#    $fl_mtd_sct[$idx_stl_sml][0] = "Satellite small";         # option descriptor
#    $fl_mtd_sct[$idx_stl_sml][1] = "~100MB";                  # file size
#    $fl_mtd_sct[$idx_stl_sml][2] = $fl_tmg[$idx_stl_sml][0] = "stl_sml";                 # file name root
#    $fl_mtd_sct[$idx_stl_sml][3] = "\'d2_00[lat,lon]=16.37f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=3.8f;\'";
    
    $fl_mtd_sct[$idx_stl_5km][0] = "Satellite 5km";           # option descriptor
    $fl_mtd_sct[$idx_stl_5km][1] = "~300MB";                  # file size
    $fl_mtd_sct[$idx_stl_5km][2] = $fl_tmg[$idx_stl_5km][0] = "stl_5km";                 # file name root
    $fl_mtd_sct[$idx_stl_5km][3] = "\'weepy=1.23456f;d2_00[lat,lon]=2.8f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=2.8f;d2_03[lat,lon]=2.8f;d2_04[lat,lon]=2.8f;d2_05[lat,lon]=2.8f;d2_06[lat,lon]=2.8f;d2_07[lat,lon]=2.8f;\'";
    
    $fl_mtd_sct[$idx_gcm_dly][0] = "GCM T85";              # option descriptor
    $fl_mtd_sct[$idx_gcm_dly][1] = "~1.2GB";                    # file size
    $fl_mtd_sct[$idx_gcm_dly][2] = $fl_tmg[$idx_gcm_dly][0] = "gcm_T85";            # file name root
    $fl_mtd_sct[$idx_gcm_dly][3] = "\'weepy=0.8f;dopey=0.8f;sleepy=0.8f;grouchy=0.8f;sneezy=0.8f;doc=0.8f;wanky=0.8f;skanky=0.8f;d1_00[time]=1.8f;d1_01[time]=1.8f;d1_02[time]=1.8f;d1_03[time]=1.8f;d1_04[time]=1.8f;d1_05[time]=1.8f;d1_06[time]=1.8f;d1_07[time]=1.8f;d2_00[lat,lon]=16.2f;d2_01[lat,lon]=16.2f;d2_02[lat,lon]=16.2f;d2_03[lat,lon]=16.2f;d2_04[lat,lon]=16.2f;d2_05[lat,lon]=16.2f;d2_06[lat,lon]=16.2f;d2_07[lat,lon]=16.2f;d2_08[lat,lon]=16.2f;d2_09[lat,lon]=16.2f;d2_10[lat,lon]=16.2f;d2_11[lat,lon]=16.2f;d2_12[lat,lon]=16.2f;d2_13[lat,lon]=16.2f;d2_14[lat,lon]=16.2f;d2_15[lat,lon]=16.2f;d3_00[time,lat,lon]=64.0f;d3_01[time,lat,lon]=64.0f;d3_02[time,lat,lon]=64.0f;d3_03[time,lat,lon]=64.0f;d3_04[time,lat,lon]=64.0f;d3_05[time,lat,lon]=64.0f;d3_06[time,lat,lon]=64.0f;d3_07[time,lat,lon]=64.0f;d3_08[time,lat,lon]=64.0f;d3_09[time,lat,lon]=64.0f;d3_10[time,lat,lon]=64.0f;d3_11[time,lat,lon]=64.0f;d3_12[time,lat,lon]=64.0f;d3_13[time,lat,lon]=64.0f;d3_14[time,lat,lon]=64.0f;d3_15[time,lat,lon]=64.0f;d3_16[time,lat,lon]=64.0f;d3_17[time,lat,lon]=64.0f;d3_18[time,lat,lon]=64.0f;d3_19[time,lat,lon]=64.0f;d3_20[time,lat,lon]=64.0f;d3_21[time,lat,lon]=64.0f;d3_22[time,lat,lon]=64.0f;d3_23[time,lat,lon]=64.0f;d3_24[time,lat,lon]=64.0f;d3_25[time,lat,lon]=64.0f;d3_26[time,lat,lon]=64.0f;d3_27[time,lat,lon]=64.0f;d3_28[time,lat,lon]=64.0f;d3_29[time,lat,lon]=64.0f;d3_30[time,lat,lon]=64.0f;d3_31[time,lat,lon]=64.0f;d3_32[time,lat,lon]=64.0f;d3_33[time,lat,lon]=64.0f;d3_34[time,lat,lon]=64.0f;d3_35[time,lat,lon]=64.0f;d3_36[time,lat,lon]=64.0f;d3_37[time,lat,lon]=64.0f;d3_38[time,lat,lon]=64.0f;d3_39[time,lat,lon]=64.0f;d3_40[time,lat,lon]=64.0f;d3_41[time,lat,lon]=64.0f;d3_42[time,lat,lon]=64.0f;d3_43[time,lat,lon]=64.0f;d3_44[time,lat,lon]=64.0f;d3_45[time,lat,lon]=64.0f;d3_46[time,lat,lon]=64.0f;d3_47[time,lat,lon]=64.0f;d3_48[time,lat,lon]=64.0f;d3_49[time,lat,lon]=64.0f;d3_50[time,lat,lon]=64.0f;d3_51[time,lat,lon]=64.0f;d3_52[time,lat,lon]=64.0f;d3_53[time,lat,lon]=64.0f;d3_54[time,lat,lon]=64.0f;d3_55[time,lat,lon]=64.0f;d3_56[time,lat,lon]=64.0f;d3_57[time,lat,lon]=64.0f;d3_58[time,lat,lon]=64.0f;d3_59[time,lat,lon]=64.0f;d3_60[time,lat,lon]=64.0f;d3_61[time,lat,lon]=64.0f;d3_62[time,lat,lon]=64.0f;d3_63[time,lat,lon]=64.0f;d4_00[time,lev,lat,lon]=1.1f;d4_01[time,lev,lat,lon]=1.2f;d4_02[time,lev,lat,lon]=1.3f;d4_03[time,lev,lat,lon]=1.4f;d4_04[time,lev,lat,lon]=1.5f;d4_05[time,lev,lat,lon]=1.6f;d4_06[time,lev,lat,lon]=1.7f;d4_07[time,lev,lat,lon]=1.8f;d4_08[time,lev,lat,lon]=1.9f;d4_09[time,lev,lat,lon]=1.11f;d4_10[time,lev,lat,lon]=1.12f;d4_11[time,lev,lat,lon]=1.13f;d4_12[time,lev,lat,lon]=1.14f;d4_13[time,lev,lat,lon]=1.15f;d4_14[time,lev,lat,lon]=1.16f;d4_15[time,lev,lat,lon]=1.17f;d4_16[time,lev,lat,lon]=1.18f;d4_17[time,lev,lat,lon]=1.19f;d4_18[time,lev,lat,lon]=1.21f;d4_19[time,lev,lat,lon]=1.22f;d4_20[time,lev,lat,lon]=1.23f;d4_21[time,lev,lat,lon]=1.24f;d4_22[time,lev,lat,lon]=1.25f;d4_23[time,lev,lat,lon]=1.26f;d4_24[time,lev,lat,lon]=1.27f;d4_25[time,lev,lat,lon]=1.28f;d4_26[time,lev,lat,lon]=1.29f;d4_27[time,lev,lat,lon]=1.312f;d4_28[time,lev,lat,lon]=1.322f;d4_29[time,lev,lat,lon]=1.332f;d4_30[time,lev,lat,lon]=1.342f;d4_31[time,lev,lat,lon]=1.352f;\'";

    $fl_mtd_sct[$idx_tms_lng][0] = "Long timeseries"; # option descriptor
    $fl_mtd_sct[$idx_tms_lng][1] = "~52MB";                   # file size
    $fl_mtd_sct[$idx_tms_lng][2] = $fl_tmg[$idx_tms_lng][0] = "tms_lng";                 # file name root
    $fl_mtd_sct[$idx_tms_lng][3] = "\'time[time]=1.0f;hmdty[time]=98.3f;PO2[time]=18.7f;PCO2[time]=1.92f;PN2[time]=77.4f;w_vel[time]=14.8f;w_dir[time]=321.3f;temp[time]=23.5f;lmbda_260[time]=684.2f\'";
    
    return @fl_tmg;
}; # end of fl_mtd_ntl()

sub fl_mk {
# Purpose: Create files from CDL templates and populate them for benchmarks
# Must be called after fl_mtd_ntl()
    my $idx = shift;
    my $fl_nbr = 3;
    $pfx_cmd = "$MY_BIN_DIR";
    
    if ($dbg_lvl > 2) {
	print "\nWaiting for keypress to proceed.\n";
	my $tmp = <STDIN>;
    }
    
    my $t0;
    my $elapsed;
    
    my $fl_in = my $fl_out = "$drc_dat/$fl_mtd_sct[$idx][2].nc" ;
    $bm_drc = "../bm"; 
    print "==== Creating $fl_mtd_sct[$idx][0] data file from template in [$bm_drc]\n";
    print "Executing: $tmr_app ncgen -b -o $fl_out $bm_drc/$fl_mtd_sct[$idx][2].cdl\n";
    if ($hiresfound) {$t0 = [gettimeofday()];}
    else {$t0 = time;}
# File creation now timed
    system  "$tmr_app ncgen -b -o $fl_out $bm_drc/$fl_mtd_sct[$idx][2].cdl";
    if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday()]);}
    else {$elapsed = time - $t0;}
# log it to common timing array
    $fl_tmg[$idx][0] = "$fl_mtd_sct[$idx][2]"; # name root
    $fl_tmg[$idx][1] = $elapsed; # creation time
    if ($idx == 2) { # tms_lng needs extra massaging
# tms_lng ready for ncap'ing
	if ($dbg_lvl > 0) {print "extra steps for tms_lng - ncecat...\n";}
	system "$pfx_cmd/ncecat -O -h $fl_in $fl_out";  # inserts a record dimension
	if ($dbg_lvl > 0) {print "ncpdq...\n";}
	system "$pfx_cmd/ncpdq -O -h -a time,record $fl_in $fl_out"; # swaps time and 'record'
	if ($dbg_lvl > 0) {print "ncwa...\n";}
	system "$pfx_cmd/ncwa -O -h -a record $fl_in $fl_out"; # averages 'record'
    }
    print "\n==== Populating $fl_out file.\nTiming results:\n";
    if ($hiresfound) {$t0 = [gettimeofday()];}
    else {$t0 = time;}
# csz 20061024: Changed ncap to ncap2 with no speed penalty (double-parsing works)
    print "Executing: $tmr_app $pfx_cmd_crr/ncap2 -h -O $nco_D_flg -s $fl_mtd_sct[$idx][3] $fl_in $fl_out\n";
    system "$tmr_app $pfx_cmd_crr/ncap2 -O -h -s $fl_mtd_sct[$idx][3] $fl_in $fl_out";
    if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday()]);}
    else {$elapsed = time - $t0;}
    $fl_tmg[$idx][2] = $elapsed; # population time
    print "==========================\nEnd of $fl_mtd_sct[$idx][2] section\n==========================\n";
    return @fl_tmg;
} # end sub fl_mk

# Summarize timing results of file creation tests
sub rsl_smr_fl_mk {
    $fl_nbr = 3;
#    print " in rsl_smr_fl_mk,  \$fl_tmg[1][0] = $fl_tmg[1][0] & \$fl_nbr = $fl_nbr\n";
    if ($dbg_lvl > 0){print "Summarizing results of file creation\n";}
    my $CC = `../src/nco/ncks --compiler`;
    my $CCinfo = '';
    if ($CC =~ /gcc/) {$CCinfo = `gcc --version |grep -i gcc`;}
    elsif ($CC =~ /xlc/) {$CCinfo = "xlc version ??";}
    elsif ($CC =~ /icc/) {$CCinfo = "Intel C Compiler version ??";}
    my $reportstr = '';
    my $idstring = `uname -a` . "using: " . $CCinfo; chomp $idstring;
    my $udp_dat = "File Creation | $timestamp | $idstring | ";
    $reportstr .= "NCO File Creation Test Result Summary: [$timestamp]\n$idstring\n";
    $reportstr .=  "      Test                       Total Wallclock Time (s) \n";
    $reportstr .=  "=====================================================\n";
    
    for (my $fl_idx=0; $fl_idx<$fl_nbr; $fl_idx++) {
	$reportstr .= sprintf "Creating   %15s:           %6.4f \n", $fl_tmg[$fl_idx][0], $fl_tmg[$fl_idx][1];
	$reportstr .= sprintf "Populating %15s:           %6.4f \n", $fl_tmg[$fl_idx][0], $fl_tmg[$fl_idx][2];
	$udp_dat   .= sprintf "%s : %6.4f : %6.4f",$fl_tmg[$fl_idx][0], $fl_tmg[$fl_idx][1], $fl_tmg[$fl_idx][2];
    }
    $reportstr .= sprintf "\n\n";
    print $reportstr;
    if ($udp_rpt) {
	$sock->send($udp_dat);
	if ($dbg_lvl > 0) { print "File Creation: udp stream sent to $server_ip:\n$udp_dat\n";}
    } # and send it back separately
} # end of rsl_smr_fl_mk

# drc_dat_set(): figures out where to write data
sub drc_dat_set{
    $caseid = shift;
    my $tmp;
    my $umask = umask;
# Does user have $DATA defined in his environment?
# Bail with nasty message if it is not readable and writable
# 	if ($caseid ne "") {
# 		$caseid =~ s/[^\w]/_/g;
# 	}
    
    if (defined $ENV{'DATA'} && $ENV{'DATA'} ne "") { # then is it readwritable?
	if (-w $ENV{'DATA'} && -r $ENV{'DATA'}) {
	    if ($que == 0) {print "INFO: Using your environment variable DATA \n\t   [$ENV{'DATA'}]\n\t as the root DATA directory for this series of tests.\n";}
	    if ($caseid ne "") {
		$drc_dat = "$ENV{'DATA'}/nco_bm/$caseid";
		my $err = `mkdir -p -m0777 $drc_dat`;
		if ($err ne "") {die "mkdir err: $drc_dat\n";}
	    } else { # just dump it into nco_bm
		$drc_dat = "$ENV{'DATA'}/nco_bm";
		my $err = `mkdir -p -m0777 $drc_dat`;
		if ($err ne "") {die "mkdir err: $drc_dat\n";}
	    }
	} else {
	    die "You have defined a DATA directory ($ENV{'DATA'}) that cannot be written to or read\nfrom or both - please try again.\n stopped";
	}
    } elsif ($que == 0) {
	$tmp = 'notset';
	print "You do not have a \$DATA directory defined and the test files are several GB. \nWhere would you like to write the test data?  Data will be placed in the indicated directory,\nunder nco_bm, using the '--caseid' option to set the name of the subdirectory, if any. \n[$ENV{'HOME'}] or specify: ";
	$tmp = <STDIN>;
	chomp $tmp;
	print "You entered [$tmp] \n";
	if ($tmp eq '') {
	    $drc_dat = "$ENV{'HOME'}/nco_bm/$caseid";  # if $caseid not set, then it decays to $ENV{'HOME'}/nco_bm/
	    if (-e "$drc_dat") {
		print "$drc_dat already exists - OK to re-use?\n[N/y] ";
		$tmp = <STDIN>;
		chomp $tmp;
		if ($tmp =~ "[nN]" || $tmp eq '') {
		    die "\nFine - decide what to use and start over again - bye! stopped";
		} else { print "\n";	}
	    } else { # have to make it
		print "Making $drc_dat & continuing\n";
		my $err = `mkdir -p -m0777 $drc_dat`;
		if ($err ne "") {die "mkdir err: $drc_dat\n";}
	    }
	} else {
	    $drc_dat = "$tmp/nco_bm/$caseid";
	    # and now test it
	    if (-w $drc_dat && -r $drc_dat) {
		print "OK - we will use [$drc_dat] to write to.\n\n";
	    } else { # must make it
		print "[$drc_dat] doesn't exist - will try to make it.\n";
		my $err = `mkdir -p -m0777 $drc_dat`;
		if ($err ne "") {die "mkdir err: $drc_dat\n";}
		if (-w $drc_dat && -r $drc_dat) {
		    print "OK - [$drc_dat] is available to write to\n";
		} else {	die "ERROR - [$drc_dat] could not be made - check this and try again.\n stopped";}
	    }
	}
    } else { # que != 0
	die "You MUST define a DATA environment variable to run this in a queue\n stopped";
    } # !defined $ENV{'DATA'})
} # end drc_dat_set()

#########################  subroutine tst_run ()  ##############################
# tst_run() consumes @tst_cmd array and executes them in order
#################################################################################
sub tst_run {
    my %fl_nm_lcl = ( # fl_nm_lcl = local_file_name
		'%stdouterr%'   => "", # stdouterr has to be left to generate stderr
		'%tmp_fl_00%'    => "$drc_dat/tmp_fl_00.nc", # Default replacement for $fl_out
		'%tmp_fl_01%'    => "$drc_dat/tmp_fl_01.nc",
		'%tmp_fl_02%'    => "$drc_dat/tmp_fl_02.nc",
		'%tmp_fl_03%'    => "$drc_dat/tmp_fl_03.nc",
		'%tmp_fl_04%'    => "$drc_dat/tmp_fl_04.nc",
		'%tmp_fl_05%'    => "$drc_dat/tmp_fl_05.nc",
# Currently no use for more than 05
		'%tmp_fl_06%'    => "$drc_dat/tmp_fl_06.nc",
		'%tmp_fl_07%'    => "$drc_dat/tmp_fl_07.nc",
		'%tmp_fl_08%'    => "$drc_dat/tmp_fl_08.nc",
		'%tmp_fl_09%'    => "$drc_dat/tmp_fl_09.nc",
		'%tmp_fl_10%'    => "$drc_dat/tmp_fl_10.nc",
		'%tmp_fl_11%'    => "$drc_dat/tmp_fl_11.nc",
		'%tmp_fl_12%'    => "$drc_dat/tmp_fl_12.nc",
		);
    
# If executign on client side, replace special purpose
# filenames with names like $fl_nm_lcl{'%tmp_fl_00%'}
    
    # fxm: WTF do these vars require this treatment?!??
    *dbg_lvl = *main::dbg_lvl;
    *fl_out = *main::fl_out;
    *mpi_prc = *main::mpi_prc;
    
    if($dbg_lvl > 0){
	print "\n\n\n### New tst_run() cycle [$opr_nm: $dsc_sng] ###\n";
	if($fl_nm_lcl{'%tmp_fl_00%'} eq ""){
	    print "fl_out undefined!\n";
	} # else {	print "\$fl_nm_lcl{'%tmp_fl_00%'} = [$fl_nm_lcl{'%tmp_fl_00%'}] \n";}
    }

# tst_run() requires that regression tests provide expected values
# If last element is not SS_OK, tst_run() pops off expected value and 
# processes commands same as it did previously
    my $arr_ref = shift; # Pass benchmark()'s @tst_cmd via reference to maintain coherence
    my @cmd_lst= @$arr_ref; # Dereference to new array name
    # Clear variables
    my $ssdwrap_cmd = $dbg_sng = $err_sng = $pfx_mpi = $pfx_fk = "";
    my $cmd_rsl_is_nbr = 1; my $xpc_is_nbr = 1; # for extra return value checks
    
    # Twiddle $pfx_cmd to allow running mpnc* as non-MPI'd  executable
    if ($mpi_fk) {$pfx_fk = "$MY_BIN_DIR/mp"; }
    $pfx_cmd = "$MY_BIN_DIR/";
    # $pfx_mpi always has mpirun directive
    
    # AIX can run naked command as long as environment has been set up
    # NB: This is for regression testing on interactive node, 
    # not for benchmarking under POE - intercepted and handled at startup
    # on AIX, non-MPI ops compiled with MPI will atttempt to run MP_PROCS.  
    # To hold them to one process, must add explicit prefix ($aix_mpi_sgl_nvr), added below
    my $aix = 0; if ($os_nm =~ /AIX/) {$aix = 1;} # yafv for aix
    if($aix){$pfx_mpi=" $aix_mpi_nvr_pfx $MY_BIN_DIR/mp";}
    elsif($mpi_upx eq "") {
	# Assume Linux-like MPI
        $pfx_mpi = "mpirun -np $mpi_prc $MY_BIN_DIR/mp";
    }else{
	$pfx_mpi = " ". $mpi_upx . "  $MY_BIN_DIR/mp";
    } # use user-supplied prefix
    $pfxd = 1; 
    $timed = 1;
    
    my $pwd=`pwd`; chomp $pwd;
    dbg_msg(1,"\$pwd=$pwd | \$pfx_cmd=$pfx_cmd | \$pfx_mpi=$pfx_mpi | \$pfx_fk=$pfx_fk");
    
    # Delete everything in DAP subdir to force DAP retrieval
    # $drc_dat has by now been directed to $drc_dat/DAP_DIR
    
#	print "DEBUG[tst_run]:\$dodap = [$dodap], \$prsrv_fl = [$prsrv_fl]\n";
    if ($dodap ne "FALSE" && !$prsrv_fl) {
	print "\nWARN: about to unlink everything in $drc_dat ! Continue? [Ny]\n";
	my $wait = <STDIN>; 
	if ($wait !~ /[Yy]/){ die "Make sure of the commandline options!\n";}
	my $unlink_cnt = unlink <$drc_dat/*>;
	print "\nINFO: OK - unlinked $unlink_cnt files\n";
    }
#	print "just past unlinking stage \n";  my $wait = <STDIN>;
    
    if($dbg_lvl > 0){
	for(my $cmd_idx=0; $cmd_idx <= $#cmd_lst; $cmd_idx++) {print "### cmd_lst[$cmd_idx] = $cmd_lst[$cmd_idx] ###\n";}
    }
    
# Perform tests of requested operator; default is all
    if (!defined $tst_nbr{$opr_nm}) {
	#print "DEBUG: \$tst_nbr{\$opr_nm} not defined - going to exit!\n";
	@cmd_lst=();  # Clear test array
	# and init the timing hashes
	$real_tme{$opr_nm}=0;
	$usr_tme{$opr_nm}=0;
	$sys_tme{$opr_nm}=0;
	return;
    }else{
	#print "\$tst_nbr{\$opr_nm} = $tst_nbr{$opr_nm}\n";
    }
#	print "DEBUG: for $opr_nm, \$tst_nbr{\$opr_nm} = $tst_nbr{$opr_nm}\n";
    
    $subbenchmarks{$opr_nm} = 0;
    $tst_nbr{$opr_nm}++;
    my $cmd_lst_cnt = 0;
    my $t = 0;
    my $lst_cmd = @cmd_lst;
    my $elapsed;
    $lst_cmd--;
    
    dbg_msg(4,"\n nsr_xpc = $nsr_xpc\n dbg_lvl = $dbg_lvl\n wnt_log = $wnt_log\n cmd_lst = @cmd_lst");
    
    &bm_vrb($dbg_lvl, $wnt_log, "\n\n============ New Test ==================\n");
    
    # csz++
    $dot_nbr_min=3; # Minimum number of dots between description and "ok" result
    $dot_sng='.....................................................................';
    $dsc_lng_max=80; # Maximum length of description printed
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
    
# SS checks and balances
# $ncks_chk = SS_gnarly_pything(\@cmd_lst)
    my $SS_nsr_xpc = 0;
    my $SS_OK = 1;
    if ($cmd_lst[$#cmd_lst] ne "SS_OK") {$SS_OK = 0;} # check on last el whether cmds can be SS'ed
    if ($SS_OK && $srv_sd ne "SSNOTSET" ) {
        SS_prepare(); # make sure things are init'ed
        # Send for processing and get back string or single value to check
        $SS_nsr_xpc=SS_gnarly_pything(\@cmd_lst);
        
        $cmd_rsl=$SS_nsr_xpc; # do this in one step later
        # and undef the last one to leave the expected value as last value
        delete $cmd_lst[$#cmd_lst];
        $nsr_xpc=$cmd_lst[$#cmd_lst]; # pop last value to provide exepected answer
#		print "\n##DEBUG:\t$nsr_xpc (expt)\n\t\t$cmd_rsl (SS)\n";
    } else {
	# delete SS value to leave "expected value" as last
	delete $cmd_lst[$#cmd_lst];
	# regardless, pop the next value off to provide the 'expected value'
	$nsr_xpc = $cmd_lst[$#cmd_lst]; # pop the next value off \
	delete $cmd_lst[$#cmd_lst]; # and now the $cmd_lst is the same as it ever was..
	
#print "\nDEBUG:in tst_run:613, \$mpi_prc=[$mpi_prc] \$pfx_mpi=[$pfx_mpi] \$mpi_fk=[$mpi_fk]\n";
	foreach (@cmd_lst){
#			print "\nforeach cmd_lst = $_\n";
	    my $md5_chk = 1;
	    $dbg_sng .= "\nDEBUG: Full commandline for part $cmd_lst_cnt:\n";
	    if ($_ !~ /foo.nc/) {$md5_chk = 0;}
	    my $opcnt = 0;
	    my $md5_dsc_sng = $dsc_sng . "_$cmd_lst_cnt";
	    
	    # Substitute real for fake file names (%*%)
	    my $r = 0; 	my $N = my @L = split;
	    while ($r <= $N) { if ($L[$r] =~ /\%.{8,9}\%/){ $L[$r] = $fl_nm_lcl{$L[$r]};}	$r++;	}
	    $_ = ""; # zero and then reconstitute $_
	    for ($r=0; $r<= $N; $r++) {$_ .= $L[$r] . " ";}
	    #print "DEBUG: reconstituted \$_ = $_\n";
	    
	    # Add $pfx_cmd only to NCO operator commands, not things like 'cut'.
	    foreach my $op (@opr_lst_all) {
		$pfx_cmd_crr=$pfx_cmd;
		if ($op eq 'ncap2'){
		    # 20100915 Hack fix for ncap2 with Chad
		    if (defined $ENV{'MY_BIN_DIR'} && $ENV{'MY_BIN_DIR'} ne ""){
#			$pfx_cmd_crr=$ENV{'MY_BIN_DIR'};
			$pfx_cmd_crr=$MY_BIN_DIR.'/';
#			$pfx_cmd_crr=$pfx_cmd;
		    }else{
			$pfx_cmd_crr='../src/nco++'.'/';
#			$pfx_cmd_crr=$pfx_cmd;
		    }
		} # endif ncap2
		    if ($_ =~ m/$op/ ) { # If op is anywhere in main list
		    if ($mpi_prc > 0 && $opr_sng_mpi =~ /$op/) {
			$_ = $tmr_app . $pfx_mpi . $_; } # ...and in MPI list...
		    elsif ($mpi_fk  && $opr_sng_mpi =~ /$op/) {
			$_ = $tmr_app . $pfx_fk . $_; } # Fake prefix
		    # Non-MPI applications compiled w/MPI need special prefix to hold them to single process
		    elsif ($aix) {$_ = $tmr_app . $aix_mpi_sgl_nvr_pfx . $pfx_cmd_crr . $_;}
		    else         {$_ = $tmr_app . $pfx_cmd_crr . $_; } # Standard prefix
		    dbg_msg(1, "URGENT:before execution, cmd_ln= $_ \n");
		    last;
		}
	    } # end of foreach my $op (@opr_lst_all)
	    $dbg_sng .= "DEBUG:$_\n";
	    
	    # NB: May require ONLY HiRes timing since SERVERSIDE will be hard to do otherwise
	    # timing code using Time::HiRes
	    my $t0;
	    if($hiresfound) {$t0 = [gettimeofday()];} else{$t0 = time;}
	    
	    # Execute command, split off stderr to file 'nco_bm.stderr'
	    $cmd_rsl = `($_) 2> nco_bm.stderr`; # stderr contains timing info, if any

#  	print "\nDEBUG: cmd = $_ \n and \$cmd_rsl = $cmd_rsl\n ";
	    if ($dbg_lvl >= 1) {print "\nDEBUG: result of [$_]\n = [$cmd_rsl]\n";}
	    chomp $cmd_rsl;
	    
# 			# still newlines in $cmd_rsl? -> a multiline result & only want the last one.
# 			if ($cmd_rsl =~/\n/) {
# 				my @rsl_arr = split(/\n/, $cmd_rsl);
# 				$cmd_rsl = $rsl_arr[$#rsl_arr]; # take the last line
# 				if ($dbg_lvl >= 1) {print "\nprocessed multiline \$cmd_rsl = [$cmd_rsl]\n";}
# 			}
# 			# figure out if $cmd_rsl is numeric or alpha
# 			if ($cmd_rsl =~ /-{0,1}\d{0,9}\.{0,1}\d{0,9}/ &&
# 				$cmd_rsl !~ /[a-df-zA-DF-Z ,]/) { $cmd_rsl_is_nbr = 1;}
# 			else { #print "DEBUG: \$cmd_rsl is not numeric: $cmd_rsl \n";
# 				$cmd_rsl_is_nbr = 0;
# 			}
# 			# figure out if $nsr_xpc is numeric or alpha
# 			if ($nsr_xpc =~ /-{0,1}\d{0,9}\.{0,1}\d{0,9}/ &&
# 				$nsr_xpc !~ /[a-df-zA-DF-Z ,]/) { $xpc_is_nbr = 1;}
# 			else { #print "DEBUG: \$nsr_xpc is not numeric: $nsr_xpc \n";
# 				$xpc_is_nbr = 0;
# 			}
	    
	    if ($timed) {
		$sys_time = `cat nco_bm.stderr`;
		if ($sys_time ne "") {
		    if ($sys_time =~ /ERR/ ) {last;}
		    $sys_time =~ s/\n/ /g;
		    #	print "\$sys_time = [$sys_time]\n";
		    @sys_tim_arr = split(" ", $sys_time); # [0]real [1]0.00 [2]user [3]0.00 [4]sys [5]0.00
		    my @rev_sys_tim_arr = reverse @sys_tim_arr;
		    # print"\@revsys_tim_arr = @rev_sys_tim_arr\n";
		    # this will fail if an error occurs which offsets the time info, so you have to step thru the list until hit 'real' which syncs the array.  or do it from the back end.
		    $real_tme{$opr_nm} += $rev_sys_tim_arr[0] + 0; # '+0 forces conversion to a nbr
		    $usr_tme{$opr_nm}  += $rev_sys_tim_arr[2] + 0;
		    $sys_tme{$opr_nm}  += $rev_sys_tim_arr[4] + 0;
		}
	    }
	    if($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday()]);}
	    else{$elapsed = time - $t0;}
	    
	    #print "inter benchmark for $opr_nm = $subbenchmarks{$opr_nm} \n";
	    $subbenchmarks{$opr_nm} += $elapsed;
	    #		$tst_idx = $tst_nbr{$opr_nm}-1;
	    if($dbg_lvl > 3){print "\t$opr_nm subtest [$t] took $elapsed seconds\n";}
	    $dbg_sng .= "DEBUG: Result = [$cmd_rsl]\n";
	    
	    #and here, check results by md5 checksum for each step - insert guts of rsl_chk_MD5_wc()
	    # have to mod the input string -  suffix with the cycle#
	    # follow check only if the MD5 module is present, there's a foo.nc to check ($fl_nm_lcl{'%tmp_fl_00%'} = 'foo.nc')
	    # & non-terminal cmd (the terminal command is ncks which is expected to return a single value or string)
# 			dbg_msg(3,"rsl_chk_MD5_wc(): \$md5 = $md5, \$md5_chk = $md5_chk, \$cmd_lst_cnt ($cmd_lst_cnt) < \$lst_cmd ($lst_cmd)");
# 			if ($md5 && $md5_chk && $cmd_lst_cnt < $lst_cmd) {
# 				dbg_msg(2,"Entering rsl_chk_MD5_wc() with \$fl_nm_lcl{'%tmp_fl_00%'}=$fl_nm_lcl{'%tmp_fl_00%'}");
# 				rsl_chk_MD5_wc($fl_nm_lcl{'%tmp_fl_00%'}, $md5_dsc_sng);
# 			}
# 			if ($md5_chk == 0 && $dbg_lvl > 0) { $dbg_sng .= "WARN: No MD5/wc check on intermediate file.\n";}
	    
	    # else old-style check has already been done and results are in $cmd_rsl, so process normally
	    $cmd_lst_cnt++;
	    if ($dbg_lvl > 2) {
		print "\ntst_run: test cycle held - hit <Enter> to continue\n";
		#		my $wait = <STDIN>;
	    }
	} # end loop: 	foreach (@cmd_lst)
    } # end of client side 'else'
    
    $dbg_sng.="DEBUG: Total time for $opr_nm [$tst_nbr{$opr_nm}] = $subbenchmarks{$opr_nm} s\n";
    $totbenchmarks{$opr_nm}+=$subbenchmarks{$opr_nm};
    
    # Results comparison should not be necessary to validate whole file,
    # not just a single value.
#	chomp $cmd_rsl;  # Remove trailing newline for easier regex comparison
    
    # Still newlines in $cmd_rsl? -> a multiline result & only want the last one.
    if($cmd_rsl =~/\n/){
	my @rsl_arr = split(/\n/, $cmd_rsl);
	$cmd_rsl = $rsl_arr[$#rsl_arr]; # take the last line
	if($dbg_lvl >= 1){print "\nprocessed multiline \$cmd_rsl = [$cmd_rsl]\n";}
    }
    # Is $cmd_rsl numeric or alpha?
    if($cmd_rsl =~ /-{0,1}\d{0,9}\.{0,1}\d{0,9}/ &&
	$cmd_rsl !~ /[a-df-zA-DF-Z ,]/) { $cmd_rsl_is_nbr = 1;}
    else{ #print "DEBUG: \$cmd_rsl is not numeric: $cmd_rsl \n";
	$cmd_rsl_is_nbr = 0;
    }
    # Is $nsr_xpc numeric or alpha?
    if($nsr_xpc =~ /-{0,1}\d{0,9}\.{0,1}\d{0,9}/ &&
	$nsr_xpc !~ /[a-df-zA-DF-Z ,]/) { $xpc_is_nbr = 1;}
    else{ #print "DEBUG: \$nsr_xpc is not numeric: $nsr_xpc \n";
	$xpc_is_nbr = 0;
    }
    # Compare numeric results
    if($cmd_rsl_is_nbr && $xpc_is_nbr && length($cmd_rsl)>0){ # && it equals the expected value
#print "\n \$nsr_xpc [$nsr_xpc] considered a number\n";
	$dbg_sng.="DEBUG: \$nsr_xpc assumed to be numeric: $nsr_xpc & actual  \$cmd_rsl = [$cmd_rsl]\n";
	if($nsr_xpc == $cmd_rsl){
	    $success{$opr_nm}++;
	    printf STDERR (" SVn ok\n");
	    $dbg_sng .= "DEBUG: PASSED (Numeric output)\n";
	}elsif(abs($cmd_rsl - $nsr_xpc) < 0.02){
	    $success{$opr_nm}++;
	    printf STDERR (" SVn prov. ok\n");
	    $dbg_sng .= "DEBUG: PASSED PROVISIONALLY (Numeric output):[$nsr_xpc vs $cmd_rsl]\n";
	}else{
	    printf STDERR (" FAILED!\n");
	    &failed($nsr_xpc);
	    my $diff = abs($nsr_xpc - $cmd_rsl);
	    $dbg_sng .= "DEBUG: !!FAILED (Numeric output) [expected: $nsr_xpc vs result: $cmd_rsl].  Difference = $diff.\n";
	}
    }elsif(!$cmd_rsl_is_nbr && !$xpc_is_nbr && length($cmd_rsl)>0){# Compare non-numeric tests
	 dbg_msg(2,"DEBUG: expected value assumed to be alphabetic: $nsr_xpc\n\$cmd_rsl = $cmd_rsl\n");
#print "\n \$nsr_xpc [$nsr_xpc] considered a string\n";
	 
	 # Compare $cmd_rsl with $nsr_xpc
#		if ($cmd_rsl =~ $nsr_xpc) {
	 if(substr($cmd_rsl,0,length($nsr_xpc)) eq $nsr_xpc){
	     $success{$opr_nm}++;
	     printf STDERR (" SVa ok\n");
	     $dbg_sng .= "DEBUG: PASSED Alphabetic output";
	 }else{
	     &failed($nsr_xpc);
	     $dbg_sng .= "DEBUG: !!FAILED Alphabetic output (expected: $nsr_xpc vs result: $cmd_rsl) ";
	 }
     }else{  # No result at all?
	 print STDERR " !!FAILED\n  \$cmd_rsl_is_nbr = $cmd_rsl_is_nbr and \$xpc_is_nbr = $xpc_is_nbr\n";
	 &failed();
	 $dbg_sng .= "DEBUG: !!FAILED - No result from [$opr_nm]\n";
						   }
    print $err_sng;
    if($dbg_lvl > 0){print $dbg_sng;}
    if($wnt_log){print LOG $dbg_sng;}
    @cmd_lst=(); # Clear test
    if(!$bm){$prsrv_fl = 0;} # reset so files will be deleted unless doing benchmarks
    if(-e $fl_nm_lcl{'%tmp_fl_00%'} && -w $fl_nm_lcl{'%tmp_fl_00%'}){unlink $fl_nm_lcl{'%tmp_fl_00%'};}
} # end tst_run()

####################
# %SSD : a hash/dict that contains (configurable) settings for server-side dap. 
# %SSD is used as a namespace in lieu of a package namespace.  Quick measurements 
# of accessing by hash show a 2x exec time increase--> 
# 2 normal var access = 1 hash access, which is small (compare to C/C++ 100x? diff)
our %SSD; 

# Initialize ssd variables.
sub SS_init {
    # no params for SS_init now.

    use File::Spec;
    $SSD{"url"} = "http://dust.ess.uci.edu/cgi-bin/dods/nph-dods";
    $SSD{"inpathfrom"} = "../data"; 
    # inpathfrom ($in_pth) needs to match $in_pth in NCO_rgr.pm 
    # and NCO_benchmarks.pm
    $SSD{"inpathto"} = "dodsdata";
    # inpathto needs to match the server's path to benchmarking data

    # temp-script filename -- probably want to munge to prevent conflict
    $SSD{"tfname"} = File::Spec->catfile(".", "nco_rgr_tmp_4scriptwrap");
    # path to scriptwrap.py (or equivalent)
    $SSD{"scrwrp_pth"} = File::Spec->catfile('..','src','ssdap','scriptwrap.py');

    # remember that we got initialized
    $SSD{"initialized"} = 1;

    # now do a quick sanity check.
    if (my $errorstring = SS_sanity_check()) {
        die "SS sanity check failed.: \n$errorstring\n" ;
    }
}

# if SS_OK && the user requests a SS attempt ..
# this needs to be functionized to:
# -  breathe in the cmd_lst,
# - replace the fl_out with the %tmp_flile% params (most '$fl_out's -> '%temp_00%'
# - change the in_pth arg to look for files in the dodsdata dir (replace -p xxx to -p dodsdata)
# - write that block to disk,
# - execute the scriptwrap cmd and breathe back in the returned value, currently just the ncks single value
# $ncks_chk = SS_gnarly_thing(\@cmd_lst);

sub SS_gnarly_pything {
    
    my $arr_ref = shift; # now passing in tst_run()'s cmd_lst via a ref to maintain NS separation
    my $tfname = $SSD{"tfname"};
    SS_make_script($tfname, $arr_ref);
    #print "TF should be done - waiting for action\n"; #my $wait = <STDIN>;#my $wait = <STDIN>;
# print "\n##SS cmd: $MY_BIN_DIR/scriptwrap.py  $tfname $SS_URL\nand waiting for key to tst_run"; my $wait = <STDIN>;
    # and finally EXECUTE it
    # FXM/FIXME : for now, look for scriptwrap.py in src/ssdap dir.

    my $xpct_val = SS_send_script($tfname, $SSD{"url"});
    # and now (unfortunately), write it to disk and then execute the scriptwrap.py to get a value.
    #unlink "nco_rgr_tmp_4scriptwrap" or die "Can't unlink the temp file: 'nco_rgr_tmp_4scriptwrap'\n";
    #print "returned value = $xpct_val \n"; #my $wait = <STDIN>;
    return $xpct_val;
}

# $ncks_chk = SS_gnarly_thing($tfname, \@cmd_lst);
sub SS_make_script {
    my $tfname = shift; # file to write script to.
    my $arr_ref = shift; # now passing in tst_run()'s cmd_lst via a ref to maintain NS separation
    my @sscmd_lst= @$arr_ref; # deref the ref to a new array name
    my $in_pth = $SSD{"inpathfrom"};
    my $dodsdata = $SSD{"inpathto"};
    # Write out  array replacing each $fl_out with the %temp% spec
    # First command must specify starting datadir, but client may not know it,
    # so substitute any '-p URL' with '-p %datadir%' which may be escaped at 
    # some level.
    # Further commands act on %fl_out%, so no '-p' substitution is necessary
    # Assume that '-p URL' is in first command, but check all commands for '-p'
    # because of mixed programming model, 
    # Simplest to write to disk and send via pything.
    # File name can be re-useable: 'nco_regr_temp_4scriptwrap'
    # NB: Chew off extra bits before processing actual scripts
    # i.e., as above, ignore last two entries (return value and SS_OK status)
#	print "MY_BIN_DIR = $MY_BIN_DIR\n";
#	print "DATA_DIR = $drc_dat\n";
    my $lst_scrt_idx = $#sscmd_lst - 2; # last script index that has content to be sent to the server.
    local *TF;
    open(TF, "> $tfname") or die "\nUnable to open temp file '$tfname'.\n";
    my $r = 0;
    my $sscl = ""; # 'server side cmd line' holds the SS version of the individual command lines
    while ($r <= $lst_scrt_idx) {
        #print "before chang'g line [$r]:\n$sscmd_lst[$r] \n";
        my $skip = 0;
        local $_;
        foreach (split (/\s+/, $sscmd_lst[$r])) {
            if ($skip == 1) { 
                $skip = 0;
            } elsif (/^\Q$in_pth/) { # this should be more robust
                s/\Q$in_pth/$dodsdata/; # substitute $in_pth with $dodsdata
                $sscl .= $_ . " ";
            } elsif ( /^\-\-mmr_cln/ ) {
                #don't pass --mmr_cln option to server
                # until server can gracefully drop this.
            } else { $sscl .= $_ . " ";} # copy each term over to ss cl
        }
        if ($r == $lst_scrt_idx && $sscmd_lst[$r] =~ /ncks/) {
            $sscl .= " %stdouterr% "; # SS needs this as the final term to return a value.
        }
        #print " cmd_ln [$r] munged for SS:\n$sscl \n"; #my $wait = <STDIN>;
        # and write it to the temp files
        print TF "$sscl\n";
        $r++;
        $sscl = "";
    }
    close TF;
    #print `cat $tfname`; # for debugging.
}

# SS_send_script($filename, $url)
sub SS_send_script {
    my $fname = shift;
    my $url = shift;
    my $scrwrp_pth = $SSD{"scrwrp_pth"};

    my $xpct_val = `$scrwrp_pth  $fname $url`;
    return $xpct_val;
}

# SS_sanity_check() returns an error string on failure; nothing if things are okay.
# example:
#if (my $errorstring = SS_sanity_check()) {
#    die "SS sanity check failed.: \n$errorstring\n" ;
#}
sub SS_sanity_check {
    local *F;
    my $fname = $SSD{"tfname"};
    open(F, ">$fname") || return "Can't open $fname for writing script file.";
    print F "ncks --version %stdouterr%\n";
    close(F);
    my $versionblock = SS_send_script($fname, $SSD{"url"});
    if ($versionblock =~ m/ncks version/) { return ;}
    else {
        return $versionblock;
    }
    return "Unhandled code path";
}

# call if we haven't been initialized.
sub SS_prepare {
    SS_init() unless $SSD{"initialized"};
}

####################
sub failed {
    $failure{$opr_nm}++;
    $err_sng .= "   ERR: FAILED in $opr_nm test: $dsc_sng\n";
    foreach(@cmd_lst) { $err_sng .= "   $_\n";}
    $err_sng .= "   ERR::EXPLAIN: Result: [$cmd_rsl] != Expected: [$nsr_xpc]\n\n" ;
    return;
}

sub rsl_smr_rgr {
    my $ansr='';
    my $nco_vrs_sng = vrs_sng_get();
    my $CC = `$MY_BIN_DIR/ncks --compiler`;
    my $idstring = "";
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
# csz++
#	*thr_nbr=*main::thr_nbr;
# csz--
    if ($thr_nbr > 0) {$reportstr .= " (OpenMP threads = $thr_nbr)\n";}
    else {$reportstr .= "\n";}
    my $udp_dat = $idstring . " using: " . $CCinfo . "|" . "NCO ver: $nco_vrs_sng" . "|" . $cmd_ln . "|";
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
    else { &bm_vrb($dbg_lvl, $wnt_log, $reportstr); }
    
    my $skp_rpt = $ENV{'NCO_AVOID_BM_REPORT'};
    #print "skp_rpt=", $skp_rpt;
    #print "udp_rpt=", $udp_rpt;
    if ($skp_rpt == 1){
     $udp_rpt = 0;
     #print "udp_rpt=", $udp_rpt;
     }

    if ($udp_rpt == 1 ) { # set either explicitly (1st time) or set in ~/.ncorc after user agreed to it
	
	print "\n\nThe log-formatted result from this regression test is:\n$udp_dat\n\n";
	print << "REQ_REGR_PACKET";
	
	--== REQUEST for TEST RESULTS ==--
	    
	    The NCO team would REALLY appreciate it if you would allow us to have
	    the above results of this regression test.  If you type 'yes' in
	    response to this question, the results of this regression test will
	    be sent verbatim as above, anonymously, to the NCO dev team via UDP on
	    port 29659.  If your institution has a firewall that prohibits this,
	    it will simply fail silently.  We want this info so we can fix failures,
	    improve efficiency, mitigate global warming, and bring about world peace. If
	    you DO NOT want to participate in this, type in anything OTHER than
   [y, Y, yes or YES].  <Enter> will also decline sending the data.

   Do you allow the above regression data to be sent back to the NCO dev team?
   [default is No, y, Y, yes, YES to send it]
REQ_REGR_PACKET
		print "Answer: ";
		$ansr = <STDIN>; chomp $ansr;
	}

	if ($udp_rpt || $ansr eq "y" || $ansr eq "yes" || $ansr eq "Y" ||$ansr eq "YES") {
		$sock->send($udp_dat);
		print "\nUDP Data sent!  The NCO dev team thanks you!!\nHave a good one, eh!?\n\n\n";
		if ($dbg_lvl > 0) { print "Regression: udp stream sent to $server_ip:\n$udp_dat\n";}
		# and write this agreement to the ~/.ncorc file so user isn't bothered by this again.
	if (!-e "~/.ncorc") {
	    #my $HOME = $ENV{'HOME'}
	    open(RC, "> $ENV{'HOME'}/.ncorc") or die "can't create a .ncorc file in home dir!\n";
	    print RC "udp_report=yes\n";
	    close RC;
	}
    } else {
	print "\nOK - data NOT sent, thanks for using NCO anyway - bye!\n\n";
    }
} # end of sub rsl_smr_rgr

sub rsl_chk_MD5_wc {
# taken substantially from process_tacg_results (in testtacg.pl), hjm
    dbg_msg(3, "rsl_chk_MD5_wc()::\$fl_pth = $fl_pth");
    my $file = shift;  # 1st arg
    my $testtype = shift; # 2nd arg
# 	my $md5found = shift; # 3rd arg
    my $pfx_cmd = "$MY_BIN_DIR/"; $pfxd = 1; # embed timer command and local bin directory
    my $cmd_ln = $_;
    my $return_value = $cmd_rsl; # return value of executing non-terminal commands
    my $hash = "";
    my @wc_lst;
    my $wc = "";
    if ($return_value != 0) { # nco.h:131:#define EXIT_SUCCESS 0
	print LOG "NonZero return value = $cmd_ln\n";
    } else {
# 1st do an ncks dump on the 1st 111111 lines (will cause a sig13 due to the head cmd)
	system("$pfx_cmd/ncks -P $file |head -111111 > $fl_pth/wc_out");
	@wc_lst = split(/\s+/, `wc $fl_pth/wc_out`);
	$wc = $wc_lst[1] . " " . $wc_lst[2] . " " . $wc_lst[3];
	
	# however we can do an md5 fo the entire output file
	if ($md5found == 1) {
	    open(FILE, "$file") or die "Can't open $fl_pth/wc_out\n";
	    binmode(FILE);
	    $hash = Digest::MD5->new->addfile(*FILE)->hexdigest ;
	} else { $hash = "UNDEFINED"; }
	print LOG "\n\"$testtype\" => ", "\"$hash\", #MD5\n\"$testtype\" => ", "\"$wc\", #wc\n$cmd_ln\n\n";
    }
    if ($md5found == 1) {
	if ( $MD5_tbl{$testtype} eq $hash ) { print " MD5"; bm_vrb " MD5"; }
	else {
	    print " MD5 fail,"; bm_vrb " MD5 fail,";  # test: $testtype\n";
	    if ($dbg_lvl > 1) {bm_vrb "MD5 sig: $hash should be: $MD5_tbl{$testtype}\n";}
	    if ($wc eq $wc_tbl{$testtype}) { print "WC PASS "; bm_vrb "WC PASS "; }
	    else { print " WC fail,"; bm_vrb " WC fail,"; }
	    my $errfile = "$file" . ".MD5.err"; # will get overwritten; halt test if want to keep it.
	    system("cp $file $errfile");
	}
    } else {
	if ($wc_tbl{$testtype} eq $wc) { print "passed wc \n"; bm_vrb "passed wc \n"; }
	else {
	    print " WC fail,";bm_vrb " WC fail,";
	    my $errfile = "$testtype" . ".wc.err";
	    print "\n\ncp $fl_pth/out $fl_pth/$errfile\n\n";
	    system("cp $fl_pth/out $fl_pth/$errfile");
	}
    }
    return $hash;
} # end rsl_chk_MD5_wc()


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
	print "\nDEBUG[bm:$okdbg]: $msg\n\n";
    }
}

# Grab NCO version and conmogrify it into something like: "3.0.1 / 20051003"
# Requires a string variable to absorb returned string
sub vrs_sng_get{
    my @nco_vrs;
    my $tmp_sng = `ncks --version  2>&1 |  grep version | head -2`; # long string sep by a newline.
    $tmp_sng =~ s/\n/ /g;
    my @tmp_lst = split (/\s+/, $tmp_sng);
    $nco_vrs[0] = $tmp_lst[4];
    $nco_vrs[0] =~ s/"//g;
	$nco_vrs[1] = $tmp_lst[scalar(@tmp_lst) - 1];
	# print "NCO release version: $nco_vrs[0], NCO date version: $nco_vrs[1]\n";
	$tmp_sng = "$nco_vrs[0]" . "/" . "$nco_vrs[1]";
	return $tmp_sng;
}


# Following two lines required to provide 'true' value at package end
1;
__END__
