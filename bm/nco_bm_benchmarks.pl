
# This file contains the BENCHMARK code (as opposed to the REGRESSION tests,
# which are included in "nco_bm_perform_tests.pl)
# for the NCO benchmark script nco_bm.pl
# It must maintain Perl semantics for Perl code.

# $Header: /data/zender/nco_20150216/nco/bm/nco_bm_benchmarks.pl,v 1.9 2005-11-30 19:14:21 mangalam Exp $

	print "\nINFO: Starting Benchmarks now\n";
	if($dbg_lvl > 1){print "bm: prefix = $prefix\n";}

	my $in_pth = "$dta_dir";
	my $in_pth_arg = " -p $in_pth ";
#	if ($dodap eq "FALSE") {$in_pth_arg = " -p $in_pth "}
	if ($dodap ne "" && $fl_pth =~ /http/ ) { $in_pth_arg = " $in_pth_arg "; }
	if ($dodap eq "") { $in_pth_arg = " -p  http://sand.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata "; }


	################### Set up the symlinks ###################

	if ($bm && $dodap eq "FALSE") {
		if ($dbg_lvl > 0) {print "\nINFO: Setting up symlinks for test nc files\n";}
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
	}

	#################### begin ncap benchmark hjm - needs to be verified.
	$opr_nm='ncap';
	$dsc_sng = 'ncap long algebraic operation';
	###################
	dbg_msg(2,"mpi_prc = $mpi_prc\nopr_sng_mpi = $opr_sng_mpi");

	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
		$tst_cmd[0] = "ncap -h -O $fl_fmt $nco_D_flg -s  \"nu_var1[time,lat,lon,lev]=d4_01*d4_02*(d4_03^2)-(d4_05/d4_06)\" 	-s \"nu_var2[lat,time,lev,lon]=(d4_13/d4_02)*((d4_03^2)-(d4_05/d4_06))\" -s \"nu_var3[time,lat,lon]=(d3_08*d3_01)-(d3_05^3)-(d3_11*d3_16)\" -s \"nu_var4[lon,lat,time]=(d3_08+d3_01)-((d3_05*3)-d3_11-17.33)\" $in_pth_arg ipcc_dly_T85.nc $outfile";
		$tst_cmd[1] = "ncwa -O $omp_flg -y sqrt -a lat,lon $outfile $outfile";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v d2_00 $outfile";
		$nsr_xpc = "4.024271";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng]\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncbo benchmark
	$opr_nm='ncbo';
	$dsc_sng = 'ncbo differencing two files';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncbo -h -O $fl_fmt $nco_D_flg $omp_flg --op_typ='-' $in_pth_arg ipcc_dly_T85.nc ipcc_dly_T85_00.nc $outfile";
		if($dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncks -C -H -s '%f' -v sleepy $outfile";
		$nsr_xpc = "0.000000";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng]\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncea benchmark
	$opr_nm='ncea';
	$dsc_sng = 'ncea averaging 2^5 files';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng, files=$fl_cnt\n";}
		$tst_cmd[0] = "ncea -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg stl_5km_00.nc $outfile";
		if($dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y sqrt -a lat,lon $outfile $outfile";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v d2_00 $outfile";
		$nsr_xpc = "1.604304";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncecat benchmark
	$opr_nm='ncecat';
	$dsc_sng = 'ncecat joining 2^5 files'; # skn_lgs.nc * 32 = 1.51GB
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng, files=$fl_cnt\n";}
		$tst_cmd[0] = "ncecat -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg skn_lgs_00.nc $outfile";
		$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg $outfile $outfile";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v PO2 $outfile";
		# following required due to shortened length of test under dap.
		if ($dodap eq "FALSE") { $nsr_xpc = "12.759310";}
		else { $nsr_xpc = "18.106375";}
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncflint benchmark  - needs to be verified and md5/wc sums created.
	$opr_nm='ncflint';
	$dsc_sng = 'ncflint weight-averaging 2 files';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncflint -h -O $fl_fmt $nco_D_flg   -w '0.5' $in_pth_arg ipcc_dly_T85_00.nc  ipcc_dly_T85_01.nc $outfile";
		if($dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y sqrt -a lat,lon $outfile $outfile";
		$tst_cmd[2] = "ncks -C -H -s '%f ' -v d1_00 $outfile";
		$nsr_xpc = "1.800000 1.800000 1.800000 1.800000 1.800000 1.800000 1.800000 1.800000";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncpdq benchmark - reversal
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order reversal';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		#!!WARN - change back to testing the ipcc file after verify
		$tst_cmd[0] = "ncpdq -h -O $fl_fmt $nco_D_flg $omp_flg -a -time -lev -lat -lon $in_pth_arg  ipcc_dly_T85.nc $outfile";
		# ~2m on sand for ipcc_dly_T85.nc
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey $outfile";  #ipcc
		$nsr_xpc = "0.800000"; #ipcc
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### next ncpdq benchmark - re-ordering
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order re-ordering';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $fl_fmt $nco_D_flg $omp_flg -a 'lon,time,lev,lat' $in_pth_arg  ipcc_dly_T85.nc $outfile";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey $outfile";  #ipcc
		$nsr_xpc = "0.800000"; #ipcc
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### next ncpdq benchmark - re-ordering & reversing
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order re-ordering & reversing';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $fl_fmt $nco_D_flg $omp_flg -a '-lon,-time,-lev,-lat' $in_pth_arg  ipcc_dly_T85.nc $outfile";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey $outfile";  #ipcc
		$nsr_xpc = "0.800000"; #ipcc
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### next ncpdq benchmark
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq packing a file';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $fl_fmt $nco_D_flg $omp_flg -P all_new  $in_pth_arg  ipcc_dly_T85.nc $outfile";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey $outfile";
		$nsr_xpc = "0.000000";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

	#################### begin cz benchmark list #2
	$opr_nm='ncra';
	$dsc_sng = 'ncra time-averaging 2^5 (i.e. one month) ipcc files';
	####################
	if ($notbodi) { # too big for bodi
		if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#			if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
			$tst_cmd[0] = "ncra -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg ipcc_dly_T85_00.nc $outfile";
			# ~4m on sand.
			$tst_cmd[1] =  "ncks -C -H -s '%f' -v d1_03   $outfile ";
			$nsr_xpc = "1.800001";
			go();
			if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
		} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}
	}

# the 1st and last lines of this stanza are commented for testing the benchmarks on the Gb net
# but it's incredibly slow - order of several hours even if done on the same machine
 	if ($dodap eq "FALSE") { # only if not being done by remote
		#################### begin ncrcat benchmark
		$opr_nm='ncrcat';
		$dsc_sng = 'ncrcat joining 2^5 files'; # skn_lgs.nc * 32 = 1.51GB
		####################
		if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
	#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
			$tst_cmd[0] = "ncrcat -h -O $fl_fmt $nco_D_flg $omp_flg -n $fl_cnt,2,1 $in_pth_arg skn_lgs_00.nc $outfile";
			$tst_cmd[1] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg $outfile $outfile";
			$tst_cmd[2] = "ncks -C -H -s '%f' -v PO2 $outfile";
			$nsr_xpc = "12.759310";
			go();
			if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
		} else {	print "\nNB: ncrcat benchmark skipped for OpenDAP test - takes too long.\n\n"; }
 	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncwa benchmark list #1a
	$opr_nm='ncwa';
	$dsc_sng = 'ncwa averaging all variables to scalars - stl_5km.nc & sqrt';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -w lat -y sqrt -a lat,lon $in_pth_arg stl_5km.nc $outfile";
		$tst_cmd[1] = "ncks -C -H -s '%f' -v d2_02  $outfile";
		$nsr_xpc = "1.604304";  # was 1.974842
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	# following fails on numeric cmp but why should the result the same?
	#################### begin ncwa benchmark list #1b
	$opr_nm='ncwa';
	$dsc_sng = 'ncwa averaging all variables to scalars - stl_5km.nc & rms';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y rms -w lat -a lat,lon $in_pth_arg stl_5km.nc $outfile";
		$tst_cmd[1] = "ncks -C -H -s '%f' -v d2_02  $outfile";
		$nsr_xpc = "2.826392"; # past result = 3.939694
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
		if ($notbodi) { #  ipcc too big for bodi
			#################### begin ncwa benchmark list #1c
			$opr_nm='ncwa';
			$dsc_sng = 'ncwa averaging all variables to scalars - ipcc_dly_T85.nc & sqt';
			####################
#			if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
			$tst_cmd[0] = "ncwa -h -O $fl_fmt $nco_D_flg $omp_flg -y sqrt  -w lat -a lat,lon $in_pth_arg ipcc_dly_T85.nc $outfile";
			$tst_cmd[1] = "ncks -C -H -s '%f' -v skanky  $outfile";
			$nsr_xpc = "0.800000";
			go();
		}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

	# and summarize the benchmarks results
	smrz_rgr_rslt();
