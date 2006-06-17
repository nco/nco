#!/usr/bin/perl

# Purpose:
# Generate run times based on Zender and Mangalam (2006) DDRA paper
# Currently no options---all variables hardcoded in script

# Usage:
# ~/nco/bm/ddra.theory_model.pl

use strict;

# Declare vars for strict
use vars qw(
	    $ncbo
	    $ncwa_atomic_INS $byt_in $byt_out $byt_per_val $ncwa_compute_sec_nowt
	    $var_sz $var_idx $ncwa_Icyc_per_el $rnk_idx $timestamp $INS_per_Int_op $INS_per_FP_op
	    $ncwa_compute_sec_wt $cpu_clock_freq $spd_rd $spd_wrt $dsk_tm_in
	    $dsk_tm_out $dsk_ttl_IO_tm $int_coeff $ncwa_est_flops $ncwa_int_INS_nowt $ncwa_int_INS_wt
	    $ncwa_memcpy_INS_nowt  $ncwa_memcpy_per_el_nowt $ncwa_memcpy_per_el_wt $lmn_nbr_ttl
	    $fl_nbr_in $fl_nbr_out $var_nbr $ncwa_NOWT_INS_multiplier $rnk_var @dim_vars
	    $ncwa_ttl_cyc_mlt_nowt $ncwa_ttl_cyc_mlt_wt $ncwa_TOT_CYC_nowt $ncwa_TOT_CYC_wt $ncwa_TOT_INS_multiplier $ncbo_Icyc_per_ins $ncbo_Int_INS_per_el $stl_5km $out_fl_nm $sz_avg_blk
	    $spd_flp $spd_ntg $V_int_INS $ncwa_wallclock_sec_nowt $ncwa_wallclock_sec_wt $wgt_flg $wru_flg $V_intops
	    $ncwa_weighting_correction $ncwa_weighting_multiplier $ncwa_WT_INS_multiplier $rnk_wgt
	    
	    $K_Ia  $K_Mu  $F_cnt  $Ia_cnt  $Mu_cnt  $Ms_cnt  $fp_tm  $Ittl_cnt $sz_wgt_blk
	    $Int_tm $ncwa_int_tm $ncwa_int_count
	    $MRV_flg $MRV_mlt_fct
	    );

# Initialization: Variables that user must set/check to generate valid run
$ncbo      = 0; # 1 for ncbo routines, 0 for ncwa
$wgt_flg   = 1; # 1 for weighting, 0 for NOT weighting (no effect on ncbo)
$wru_flg   = 0; # 1 for weight re-use, 0 no re-use
$stl_5km   = 0; # 1 if file is stl_5km file, 0 for gcm_T85 file
$var_sz    = 1; # simple initialization for dimension size
$MRV_flg   = 0; # Use MRV optimization 

# Variables relating to size and layout of test files
# These should be set correctly using other than standard files (stl_5km and gcm_T85)
if ($stl_5km) {               # lat   lon
    $rnk_var = @dim_vars         = (2160, 4320);
    $sz_wgt_blk              = 2160; # size of weight variable (lat)
    $rnk_wgt                 = 1; # Rank of weight
    $sz_avg_blk              = 2160 * 4320; # N sub A in ppr
    $var_nbr                 = 8; # nbr of variables (assuming all same size)
    $out_fl_nm               = "ddra.stl_5km.";
} else { # assume gcm_T85 file            lat  lon
    $rnk_var = @dim_vars         = (8, 32, 128, 256); # Rank vars that are >94% of file
    $sz_wgt_blk              = 128; # Size of weight variable (lat)
    $rnk_wgt                 = 1; # Rank of weight
    $sz_avg_blk              = 8 * 32 * 128 * 256; # N sub A in ppr
    $var_nbr                 = 32; # for most significant vars in gcm_T85 - 94% of file
    $out_fl_nm               = "ddra.gcm_T85.";
}

# Number of files in and out:
if ($ncbo) {
    $fl_nbr_in             = 2;  # number of files in - 2 for ncbo, 1 for ncwa
    $out_fl_nm .= "ncbo";
#	$INS_per_Int_op              = 2.0;   #  NCBO - calculated from tacg utility
} else {
    $fl_nbr_in             = 1;
    $out_fl_nm              .= "ncwa";
    # Following calculations imply integer operations are 18--28 times faster than FP
}
$fl_nbr_out              = 1; # number of files out

# Value of $INS_per_Int_op is not used in this script.
# Contrary to what I thought before, it seems to be constant across both ncbo and ncwa
# $INS_per_Int_op              = 2.0;   # ncbo - calculated from tacg utility
# Similarly, this should not change - we can measure it directly..
# $INS_per_FP_op               = 7.0;   # direct measure from PAPI for both ncbo AND ncwa

# Following vars are set for clay and are relatively stable unless changing machines
# I retract my previous estimate--- 
# I think bonnie estimates work better across range of tests we're doing
$spd_rd               = 63.375e6; # fr disk read bandwidth in byt/s (bonnie++)
$spd_wrt              = 57.865e6; # fr disk write bandwidth in byt/s (bonnie++)
#$spd_rd               = 100e6; # value from to or from memory - theoretical max
#$spd_wrt              = 60e6;  # value from to or from memory - theoretical max

$byt_per_val              = 4;   #change if using doubles or other non-NC_FLOAT sizes
$cpu_clock_freq             = 2e9; # CPU clock in Hz

$spd_flp                    = 153e6; # floating point speed derived from direct PAPI measures
$spd_ntg                   = 200e6; # int speed from direct measure, using a large data set

# the following estimates are from local variables and as such are highly optimized and therefore
# inapplicable to the nco benchmarks.
#$spd_flp                    = 353e6; # floating point speed derived from tacg measures
#$spd_ntg                   = 1386e6; # integer speed derived from tacg measures
#$spd_flp                    = 153e6; # floating point speed derived directly from PAPI
#$spd_flp                    = 1017e6; # floating point speed derived from BOINC measures
#$spd_ntg                   = 2147e6; # integer speed derived from BOINC measures

for (my $rnk_idx=0; $rnk_idx<$rnk_var; $rnk_idx++){$var_sz *= $dim_vars[$rnk_idx];} # total scalar size of all dimensions
# $lmn_nbr_ttl = total nbr of elelments - only for those calcs that do not run thru the
# Number of elements in all variables counted in loop below
$lmn_nbr_ttl = $var_nbr * $var_sz;

my $time;
$timestamp = `date "+%a%d%b%g-%R"`; chomp $timestamp;
# output Gnuplot data (GPD) autonamed and timestamped
open(GPD, ">$out_fl_nm.$timestamp.gpdata") or die "\nUnable to open GPD file\n";

# this stanza does ncwa for an initial data size of 10MB up to 80.(steps of 8)
if (!$ncbo) {
    my $wgt_sng = "weighted";
    if ($wgt_flg == 0) {$wgt_sng = "unweighted";}
    
    print GPD  "#ncwa $wgt_sng \n#         bytes        ops         ops         ops         ops        ops           sec        sec\nCycle   DataSize     Floating    Int_math    Mem_user     Mem_sys   Total_Int   FP_tm      Int_tm       I/O       Tot_tm\n";
    
    print "#ncwa $wgt_sng \n#         bytes        ops         ops         ops         ops        ops           sec        sec\nCycle   DataSize     Floating    Int_math    Mem_user     Mem_sys   Total_Int   FP_tm    Int_tm       I/O         Tot_tm\n";
    
    for (my $var_idx=1; $var_idx<=$var_nbr; $var_idx++) {
	my $lmn_nbr = $var_idx * $var_sz;
	# Re-arranged to better match ZeM06 equations
	if($wgt_flg == 1){ # Weighting
	    $K_Ia = 1; # fudge for Ia
	    $K_Mu = 1; # fudge for Mu
	    $F_cnt = $lmn_nbr * (3 + (2/$sz_avg_blk)); # 20a
	    $Ia_cnt = $K_Ia * ($lmn_nbr * ((22*$rnk_var) + (5*$rnk_wgt) + 18 + (7/$sz_avg_blk)) + (2*$sz_wgt_blk)); # 20b
	    $Mu_cnt = $K_Mu * ($lmn_nbr * ((12*$rnk_var) + (3*$rnk_wgt) + 4 + ($byt_per_val + 4) / $sz_avg_blk) + ($byt_per_val*$sz_wgt_blk)); # 20c
	    $Ms_cnt = 3*$lmn_nbr; # 20d
#	    $brd_cnt=$ntg_nbr_brd_fdg_fct*($lmn_nbr*(6*$rnk_var+8*$rnk_wgt+2));
	}else{ # !wgt_flg
	    $K_Ia = 1; # fudge for Ia
	    $K_Mu = 1; # fudge for Mu
	    $F_cnt = $lmn_nbr * (1 + (1/$sz_avg_blk)); # 19a
	    $Ia_cnt = $K_Ia * ($lmn_nbr * ((9 * $rnk_var) + 9 + (4 / $sz_avg_blk))); # 19b
	    $Mu_cnt = $K_Mu * ($lmn_nbr * ((5 * $rnk_var) + 3 + ($byt_per_val + 2) / $sz_avg_blk)); # 19c
	    $Ms_cnt = $lmn_nbr; # 19d
	} # !wgt_flg
	if($wgt_flg == 1 && $wru_flg == 1){ # With weight re-use
	    # Subtract cost of broadcasting weights
	    $Ia_cnt-= $K_Ia * ($lmn_nbr * ((4*$rnk_var) + (5*$rnk_wgt) + 2)); # 12a
	    $Mu_cnt-= $K_Mu * ($lmn_nbr * ((2*$rnk_var) + (3*$rnk_wgt) - 1)); # 12b
	    $Ms_cnt-= $lmn_nbr; # 12c
	} # end $wru_flg
	if($MRV_flg == 1){
	    # Subtract collection cost for unweighted averaging
	    # Subtract twice collection cost for weighted averaging
	    if($wgt_flg = 1){$MRV_mlt_fct=2;}else{$MRV_mlt_fct=1;}
	    $Ia_cnt-=$MRV_mlt_fct*$K_Ia * ($lmn_nbr * (9*$rnk_var + 4)); # 13a
	    $Mu_cnt-=$MRV_mlt_fct* $K_Mu * ($lmn_nbr * (5*$rnk_var - 1)); # 13b
	    $Ms_cnt-=$MRV_mlt_fct* $lmn_nbr; # 13c
	} # !MRV_flg

	$byt_in  = $lmn_nbr * $byt_per_val * $fl_nbr_in;
	
	# Averaging (rank reduction) file out size is less than input file size by size of averaging variable - $sz_wgt_blk
	$byt_out = ($lmn_nbr/$sz_avg_blk) * $byt_per_val * $fl_nbr_out;
	$dsk_tm_in = $byt_in / $spd_rd;
	$dsk_tm_out = $byt_out / $spd_wrt;
	$dsk_ttl_IO_tm = $dsk_tm_in + $dsk_tm_out;
#		printf "byt in=  %1.4e \$dsk_tm_in = %1.4e \nbyt out= %1.4e \$dsk_tm_out = %1.4e \n\$dsk_ttl_IO_tm =  %1.4e\n", $byt_in, $dsk_tm_in, $byt_out, $dsk_tm_out, $dsk_ttl_IO_tm;
	$fp_tm = $F_cnt / $spd_flp;
	$Ittl_cnt = $Ia_cnt + $Mu_cnt + $Ms_cnt;
	$Int_tm = $Ittl_cnt / $spd_ntg; # remember that $spd_ntg is under some suspicion
	$time = $dsk_ttl_IO_tm + $fp_tm  + $Int_tm;
	
	#         IO                IO             FP term                 Int term
	printf     "%d       %1.3e   %1.3e   %1.3e   %1.3e   %1.3e  %1.3e   %1.3e  %1.3e   %1.3e   %1.3e\n", $var_idx,      $lmn_nbr,   $F_cnt,  $Ia_cnt,   $Mu_cnt,  $Ms_cnt,  $Ittl_cnt, $fp_tm, $Int_tm, $dsk_ttl_IO_tm,  $time ;
	printf GPD "%d       %1.3e   %1.3e   %1.3e   %1.3e   %1.3e  %1.3e   %1.3e  %1.3e   %1.3e   %1.3e\n", $var_idx,      $lmn_nbr,   $F_cnt,  $Ia_cnt,   $Mu_cnt,  $Ms_cnt,  $Ittl_cnt, $fp_tm, $Int_tm, $dsk_ttl_IO_tm,  $time ;
    }
}

if ($ncbo) {
    print GPD "#ncbo - NB: FP op counts = DataSize\n
Cycle   DataSize     T_fp     T_int    T_I/O      T_tot      Int_ops\n";
    
    print "#ncbo - NB: FP op counts = DataSize\n
Cycle   DataSize     T_fp     T_int    T_I/O      T_tot      Int_ops\n";
    
    for (my $var_idx=1; $var_idx<=$var_nbr; $var_idx++) {
	my $lmn_nbr = $var_idx * $var_sz;
	$byt_in  = $lmn_nbr * $byt_per_val * $fl_nbr_in;
	$byt_out = $lmn_nbr * $byt_per_val * $fl_nbr_out; # no rank reduction
	
	$dsk_tm_in = $byt_in / $spd_rd;
	$dsk_tm_out = $byt_out / $spd_wrt;
	$dsk_ttl_IO_tm = $dsk_tm_in + $dsk_tm_out;
	
	# match ZeM06 equations, unweighted
	my $fp_cnts = $lmn_nbr;
	my $fp_tm = ($fp_cnts/$spd_flp); # NB: $spd_flp is dodgy
	
	my $K_Ia = 1; # fudge for Ia
	my $K_Mu = 1; # fudge for Mu (Mu may have to change relative to
	my $Int_cnt =  $K_Ia *   6 * $lmn_nbr;
	my $Mu_cnt =   $K_Mu *   3 * $byt_per_val * $lmn_nbr;
	my $Ms_cnt = 0;
	my $ncbo_int_op_cnts =  $Int_cnt + $Mu_cnt +  $Ms_cnt;
	my $ncbo_int_tm = $ncbo_int_op_cnts / $spd_ntg; # $spd_ntg is dodgy
	
	#         IO                 FP term                 Int term
	$time = $dsk_ttl_IO_tm + $fp_tm + $ncbo_int_tm;
	#print "$var_idx $byt_in $byt_out  $dsk_ttl_IO_tm $fp_tm  $ncbo_int_tm $ncbo_Icyc_per_el\n";
	printf     " %2d    %1.3e   %6.3f    %6.3f    %6.3f    %6.3f    %1.3e\n",  $var_idx, $lmn_nbr, $fp_tm, $ncbo_int_tm, $dsk_ttl_IO_tm, $time, $ncbo_int_op_cnts;
	printf GPD " %2d    %1.3e   %6.3f    %6.3f    %6.3f    %6.3f    %1.3e\n",  $var_idx, $lmn_nbr, $fp_tm, $ncbo_int_tm, $dsk_ttl_IO_tm, $time, $ncbo_int_op_cnts;
    }
}

# output gnuplot commands (CMDS) to file as well as prepend to gnuplot data behind '#'
# open(CMDS, ">ddra.$timestamp.gnuplot") or die "\nUnable to open CMD file\n";
# print CMDS << "GNUPLOT_CMDS";
# set title "Wallclock seconds to # Variables - stl_5km"
# set key top left
# set xlabel "Number of same-sized variables"
# set ylabel "Execution time in wallclock seconds"
# set xrange [0:9]
# set key autotitle columnheader
# plot '/home/hjm/z/change-dims.wt+nowt.dat' using 1:2
# replot '/home/hjm/z/change-dims.wt+nowt.dat' using 1:3
# replot '/home/hjm/z/change-dims.wt+nowt.dat' using 1:4
# replot '/home/hjm/z/change-dims.wt+nowt.dat' using 1:5
# replot '/home/hjm/z/change-dims.wt+nowt.dat' using 1:6
# replot '/home/hjm/z/change-dims.wt+nowt.dat' using 1:7
# set terminal postscript color
# replot
# quit
#
# GNUPLOT_CMDS


