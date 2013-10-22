package NCO_benchmarks;

# $Header: /data/zender/nco_20150216/nco/bm/NCO_benchmarks.pm,v 1.20 2013-10-22 03:03:34 zender Exp $

# Purpose: library module supporting nco_bm.pl benchmark and regression tests
# File contains BENCHMARK code (as opposed to the REGRESSION tests in "NCO_rgr.pm")
# Module contains following functions in approximate order of usage:

# benchmarks().........Entire set of NCO benchmarks

# NB: When adding tests, _be sure to use -O to overwrite files_
# Otherwise, script hangs waiting for interactive response to overwrite queries

require 5.6.1 or die "This script requires Perl version >= 5.6.1, stopped";
use English; # WCS96 p. 403 makes incomprehensible Perl errors sort of comprehensible
use Cwd 'abs_path';
use warnings;
use strict;
#use NCO_rgr qw( tst_rgr ); # module that contains tst_rgr()

use NCO_bm qw(dbg_msg tst_run
	$drc_dat @fl_mtd_sct $opr_sng_mpi $opr_nm $dsc_sng $prsrv_fl  $srv_sd $dodap
);

require Exporter;
our @ISA = qw(Exporter);
#export functions (top) and variables (bottom)
our @EXPORT = qw(
	benchmarks
	$srv_sd $dodap $fl_nbr $nco_dbg_lvl $bm  $mpi_prc $opr_sng_mpi $omp_flg $fl_fmt
	$nco_D_flg $fl_out $tw_prt_bm @tst_cmd $opr_nm $dsc_sng $nsr_xpc $fl_cnt %NCO_RC
);

use vars qw(
$drc_dat  $f  @fl_mtd_sct  $in_pth  $in_pth_arg  $ipcc_dm_sz  $ldz  $lnk_fl_nme
$MY_BIN_DIR $n  $nd  $fl_nbr  $r  $rel_fle  $ssdwrap  $var_pfx  $var_sfx
$var_sng @var_sz   $wait  $tw_prt_bm $srv_sd $opr_nm $dsc_sng $mpi_prc $fl_out
$bm $nco_dbg_lvl $dodap $fl_cnt @fl_mtd_sct $fl_fmt $fl_pth %NCO_RC $nco_D_flg
$ncwa_scl_tst $notbodi $nsr_xpc $omp_flg $opr_sng_mpi  @tst_cmd
);

sub benchmarks{

	print "\nINFO: Starting Benchmarks now:\n";
#	"\t+serverside status = $srv_sd\n\t+dodap = $dodap\n\t+$drc_dat = $drc_dat";

	# set up the input path and argument string
	$in_pth = "";
	$in_pth_arg = "";
	if ($dodap eq "FALSE") { $in_pth_arg = "-p $drc_dat"; }
	elsif ($dodap ne "" && $fl_pth =~ /http/ ) { $in_pth_arg = " -p $fl_pth "; }
	elsif ($dodap eq "") { $in_pth_arg = " -p  http://sand.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata "; }
	# hardcode for now until we come up with a robut method for doing this
	if ($srv_sd ne "SSNOTSET") { $in_pth_arg = "-p dodsdata"; }


	################### Set up the symlinks ###################

	if ($bm && $dodap eq "FALSE" && $srv_sd eq "SSNOTSET") {
		if ($nco_dbg_lvl > 0) {print "\nINFO: Setting up symlinks for test nc files\n";}
		for (my $fl_idx=0; $fl_idx<$fl_nbr; $fl_idx++) {
			my $rel_fle = "$drc_dat/$fl_mtd_sct[$fl_idx][2]" . ".nc" ;
			my $ldz = "0"; # leading zero for #s < 10
			if ($nco_dbg_lvl > 0) {print "\tsymlinking $rel_fle\n";}
			for (my $n=0; $n<32; $n ++) {
				if ($n>9) {$ldz ="";}
				my $lnk_fl_nme = "$drc_dat/$fl_mtd_sct[$fl_idx][2]" . "_" . "$ldz" . "$n" . ".nc";
				if (-r $rel_fle && -d $drc_dat && -w $drc_dat){
					symlink $rel_fle, $lnk_fl_nme;
				}
			}
		}
	}

# The general format for the benchmarks is the same as for the regressions:
# 	#################### begin ncea benchmark
# 	$opr_nm='nco_name';
# 	$dsc_sng = 'unique descriptor string for this benchmark';
# 	####################
#  #usually require the follow test to protect against MPI explosions
# 	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#	# next, fill the $tst_cmd[] array with the nco commands that you want to test in order.
#	# You can use as many as you want, with the proviso that the output file variables are
#	# named uniquely using the ones described in nco_bm.pl: 336-350 (foo...).
#	# The last nco command is usually an ncks command that examines a single value generated
#	# from the preceding chain of commands
# 		$tst_cmd[0] = "ncea -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg stl_5km_00.nc $fl_out";
# 		if($nco_dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
# 		$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y sqrt -a lat,lon $fl_out $fl_out";
# 		$tst_cmd[2] = "ncks -C -H -s '%f' -v d2_00 $fl_out";
# 		$tst_cmd[3] = "1.604304";
# 		$tst_cmd[4] = "NO_SS";
# 		tst_run(\@tst_cmd);
# 		$#tst_cmd=0;  # reset the array
# 		if($nco_dbg_lvl >  0){print "\n[past benchmark stanza - $dsc_sng\n";}
# 	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}
#
# if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

# Note that that the last SERVERSIDE executable nco statement has to end with a '%stdouterr%' to
# have it return data.  For both benchmarks and regressions, this is added in the
# SS_gnarly_pything() sub from tst_run() that handles the SS manipulations.
# Also note that in order for Daniel's ssdwrap code to work at least for now, all the files passed in have
# to be named differently in order to keep things straight.  So we can't name everything $fl_out
# (or %tmp_fl_00% and have it work. All output must be named differently in a script for the
#script to work correctly.

if (0) { # DEBUGGING to skip these
} # [DEBUGGING to skip these] down to here

	#################### begin ncap2 benchmark
	$opr_nm='ncap2';
	$dsc_sng = 'ncap2 long algebraic operation';
	###################
	dbg_msg(2,"mpi_prc = $mpi_prc\nopr_sng_mpi = $opr_sng_mpi");
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
# csz 20061024: Changed ncap to ncap2 with 40% speed penalty
# Not sure why ncap2 is much slower at long arithmetic expressions
		$tst_cmd[0] = "ncap2 -h -O $fl_fmt $nco_D_flg -s \"nu_var1[time,lev,lat,lon]=d4_01*d4_02*(d4_03^2)-(d4_05/d4_06)\" -s \"nu_var2[time,lev,lat,lon]=(d4_13/d4_02)*((d4_03^2)-(d4_05/d4_06))\" -s \"nu_var3[time,lat,lon]=(d3_08*d3_01)-(d3_05^3)-(d3_11*d3_16)\" -s \"nu_var4[time,lat,lon]=(d3_08+d3_01)-((d3_05*3)-d3_11-17.33)\" $in_pth_arg gcm_T85.nc %tmp_fl_01%";
		$tst_cmd[1] = "ncwa -O $omp_flg -y sqrt -a lat,lon %tmp_fl_01% %tmp_fl_02%";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v d2_00  %tmp_fl_02% ";
# as noted above, for serverside prep, the final '%stdouterr%' il be added in SS_gnarly_pything()
# note that the 2 additions to the list are the expected value and the indicator whether it
# can be used as a server side operation:
# "NO_SS" sez don't even try.  "SS_OK" sez to  try IF the --serverside flag is set
		$tst_cmd[3] = "4.024271";
#		$tst_cmd[4] = "NO_SS";
		$tst_cmd[4] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0; # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng]\n";}
	} else {print "Skipping Benchmark because $opr_nm does not support MPI\n";}

if ($nco_dbg_lvl >= 1) {print "paused after ncap2 - hit return to continue"; $wait = <STDIN>;}

	#################### begin ncbo benchmark
	$opr_nm='ncbo';
	$dsc_sng = 'ncbo differencing two files';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncbo -h -O $fl_fmt $nco_D_flg $omp_flg --op_typ='-' $in_pth_arg gcm_T85.nc gcm_T85_00.nc %tmp_fl_00%";
#		$tst_cmd[0] = "ncbo -h -O $fl_fmt $nco_D_flg $omp_flg --op_typ='-' $in_pth_arg stl_sml.nc stl_sml_00.nc %tmp_fl_00%";  # smaller test file
		if($nco_dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncks -C -H -s '%f' -v sleepy %tmp_fl_00% ";
#		$tst_cmd[1] = "ncks -C -H -s '%f' -v weepy %tmp_fl_00%"; # smaller test file
		$tst_cmd[2] = "0.000000";
		$tst_cmd[3] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng]\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### begin ncea benchmark
	$opr_nm='ncea';
	$dsc_sng = 'ncea averaging 2^5 files';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
		if ($nco_dbg_lvl > 0) {print "\nBenchmark: \$fl_fmt = [$fl_fmt], \$nco_D_flg = $nco_D_flg, \$omp_flg = [$omp_flg], \$dsc_sng = $dsc_sng, \$fl_cnt = [$fl_cnt], \n";}
		$tst_cmd[0] = "ncea -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg stl_5km_00.nc %tmp_fl_00%";
		if($nco_dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y sqrt -a lat,lon %tmp_fl_00% %tmp_fl_01%";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v d2_00 %tmp_fl_01%";
		$tst_cmd[3] = "1.604304";
		$tst_cmd[4] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### begin ncecat benchmark
	$opr_nm='ncecat';
	$dsc_sng = 'ncecat joining 2^5 files'; # tms_lng.nc * 32 = 1.51GB
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng, files=$fl_cnt\n";}
		$tst_cmd[0] = "ncecat -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg tms_lng_00.nc %tmp_fl_00%";
		$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg %tmp_fl_00% %tmp_fl_01%";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v PO2 %tmp_fl_01%";
		# following required due to shortened length of test under dap.
		if ($dodap eq "FALSE") { $tst_cmd[3] = "12.759310";}
		else                   { $tst_cmd[3] = "18.106375";}
		$tst_cmd[4] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### begin ncflint benchmark  - needs to be verified and md5/wc sums created.
	$opr_nm='ncflint';
	$dsc_sng = 'ncflint weight-averaging 2 files';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncflint -h -O $fl_fmt $nco_D_flg   -w '0.5' $in_pth_arg gcm_T85_00.nc  gcm_T85_01.nc %tmp_fl_00%";
		if($nco_dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y sqrt -a lat,lon %tmp_fl_00% %tmp_fl_01%";
		$tst_cmd[2] = "ncks -C -H -s '%f ' -v d1_00 %tmp_fl_01%";
		$tst_cmd[3] = "1.800000 1.800000 1.800000 1.800000 1.800000 1.800000 1.800000 1.800000";
		$tst_cmd[4] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### begin ncpdq benchmark - reversal
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order reversal';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		#!!WARN - change back to testing the ipcc file after verify
		# !! this one is buggered by the current ssdwrap
		$tst_cmd[0] = "ncpdq -h -O $fl_fmt $nco_D_flg $omp_flg -a '-time,-lev,-lat,-lon' $in_pth_arg  gcm_T85.nc %tmp_fl_00%";
		# ~2m on sand for gcm_T85.nc
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey %tmp_fl_00%";  #ipcc
		$tst_cmd[2] = "0.800000";
		$tst_cmd[3] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### next ncpdq benchmark - re-ordering
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order re-ordering';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $fl_fmt $nco_D_flg $omp_flg -a 'lon,time,lev,lat' $in_pth_arg  gcm_T85.nc %tmp_fl_00%";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey %tmp_fl_00%";  #ipcc
		$tst_cmd[2] = "0.800000";
		$tst_cmd[3] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### next ncpdq benchmark - re-ordering & reversing
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order re-ordering & reversing';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $fl_fmt $nco_D_flg $omp_flg -a '-lon,-time,-lev,-lat' $in_pth_arg  gcm_T85.nc %tmp_fl_00%";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey %tmp_fl_00%";  #ipcc
		$tst_cmd[2] = "0.800000";
		$tst_cmd[3] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### next ncpdq benchmark
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq packing a file';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $fl_fmt $nco_D_flg $omp_flg -P all_new  $in_pth_arg  gcm_T85.nc %tmp_fl_00%";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey %tmp_fl_00%";
		$tst_cmd[2] = "0.000000";
		$tst_cmd[3] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### begin cz benchmark list #2
	$opr_nm='ncra';
	$dsc_sng = 'ncra time-averaging 2^5 (i.e. one month) ipcc files';
	####################
	if ($notbodi) { # too big for bodi
		if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#			if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
			$tst_cmd[0] = "ncra -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg gcm_T85_00.nc %tmp_fl_00%";
			# ~4m on sand.
			$tst_cmd[1] =  "ncks -C -H -s '%f' -v d1_03   %tmp_fl_00% ";
			$tst_cmd[2] = "1.800001";
			$tst_cmd[3] = "NO_SS_OK";
			tst_run(\@tst_cmd);
			$#tst_cmd=0;  # reset the array
			if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
		} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}
	}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}



# the 1st and last lines of this stanza are commented for testing the benchmarks on the Gb net
# but it's incredibly slow - order of several hours even if done on the same machine
 	if ($dodap eq "FALSE") { # only if not being done by remote
		#################### begin ncrcat benchmark
		$opr_nm='ncrcat';
		$dsc_sng = 'ncrcat joining 2^5 files'; # tms_lng.nc * 32 = 1.51GB
		####################
		if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
	#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
			$tst_cmd[0] = "ncrcat -h -O $fl_fmt $nco_D_flg $omp_flg -n 22,2,1 $in_pth_arg tms_lng_00.nc %tmp_fl_00%";
#			$tst_cmd[0] = "ncrcat -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg tms_lng_00.nc %tmp_fl_00%";
			$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg %tmp_fl_00% %tmp_fl_01%";
			$tst_cmd[2] = "ncks -C -H -s '%f' -v PO2 %tmp_fl_01%";
			$tst_cmd[3] = "12.759310";
			$tst_cmd[4] = "SS_OK";
			tst_run(\@tst_cmd);
			$#tst_cmd=0;  # reset the array
			if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
		} else {	print "Skipping Benchmark [$opr_nm] - not MPI-ready\n"; }
 	} else { print "\nNB: ncrcat benchmark skipped for OpenDAP test - takes too long.\n\n"; }

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	#################### begin ncwa benchmark list #1a
	$opr_nm='ncwa';
	$dsc_sng = 'ncwa averaging all variables to scalars - stl_5km.nc & sqrt';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -w lat -y sqrt -a lat,lon $in_pth_arg stl_5km.nc %tmp_fl_00%";
		$tst_cmd[1] = "ncks -C -H -s '%f' -v d2_02  %tmp_fl_00%";
		$tst_cmd[2] = "1.673425";
		$tst_cmd[3] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}
printf("paused @ [%s:%d]  - hit return to continue\n", __FILE__, __LINE__); my $wait = <STDIN>;

	# following fails on numeric cmp but why should the result the same?
	#################### begin ncwa benchmark list #1b
	$opr_nm='ncwa';
	$dsc_sng = 'ncwa averaging all variables to scalars - stl_5km.nc & rms';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y rms -w lat -a lat,lon $in_pth_arg stl_5km.nc %tmp_fl_00%";
		$tst_cmd[1] = "ncks -C -H -s '%f' -v d2_02  %tmp_fl_00%";
		$tst_cmd[2] = "2.800084";
		$tst_cmd[3] = "SS_OK";
		tst_run(\@tst_cmd);
		$#tst_cmd=0;  # reset the array
		if($nco_dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}

	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
		if ($notbodi) { #  ipcc too big for bodi
			#################### begin ncwa benchmark list #1c
			$opr_nm = 'ncwa';
			$dsc_sng = 'ncwa averaging all variables to scalars - gcm_T85.nc & sqt';
			####################
#			if ($nco_dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
			$tst_cmd[0] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y sqrt  -w lat -a lat,lon $in_pth_arg gcm_T85.nc %tmp_fl_00%";
			$tst_cmd[1] = "ncks -C -H -s '%f' -v skanky  %tmp_fl_00%";
			$tst_cmd[2] = "0.800000";
			$tst_cmd[3] = "SS_OK";
			tst_run(\@tst_cmd);
			$#tst_cmd=0;  # reset the array
		}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

if ($nco_dbg_lvl >= 1) {print "paused - hit return to continue"; $wait = <STDIN>;}


	if ($ncwa_scl_tst) {
		if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
			if ($notbodi) { #  ipcc too big for bodi
				#my @ipcc_dm_sz = (8, 8, 16, 64, 32); # dims for d0-d4
				# 3 -v descriptors that should reduce the total size to 1/2, 1/4, 1/8.
				my @var_sz =
				(" -v d1_0[0-3] -v d2_0[0-7] -v d3_[0-2][0-9] -v -d3_3[01]  -v d4_0[0-9] -v d4_1[0-5]",
				"  -v d1_0[01]  -v d2_0[0-3] -v d3_0[0-9]      -v d3_1[0-5] -v d4_0[0-3]]",
				"  -v d1_00     -v d2_0[01]  -v d3_0[0-7]                   -v d4_0[01]");

				#################### begin ncwa benchmark list #1c
				$opr_nm='ncwa';
				$dsc_sng = 'ncwa averaging at dif var sizes - gcm_T85.nc & sqt';
				####################
				# make vars in size of 1/2, 1/4, 1/8 of each one.
# 				for (my $r=0; $r<5; $r++) {
# 					my $var_pfx = sprintf("d%d_", $r);
# 					for (my $nd=2; $nd<=8; $nd*2){
# 						my $var_sfx = sprintf("%2d", $ipcc_dm_sz[$r]);
# 						my $var_sng =
# 					}
# 				}
				for (my $r=0; $r<2; $r++) {
					$tst_cmd[0] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg $var_sz[$r] -y sqrt  -w lat -a lat,lon $in_pth_arg gcm_T85.nc %tmp_fl_00%";
					$tst_cmd[1] = "ncks -C -H -s '%f' -v skanky  %tmp_fl_00%";
					$tst_cmd[2] = "0.800000";
					$tst_cmd[3] = "SS_OK";
					tst_run(\@tst_cmd);
					$#tst_cmd=0;  # reset the array
				}
			} # not bodi
		} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}
	}

	# and summarize the benchmarks results
	NCO_bm::rsl_smr_rgr();
}
1;
__END__
