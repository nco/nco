#!/usr/bin/perl
use strict;
# Declare vars for strict

# This script is used to gnerate run times based on Zender's DDRA theory paper of 2006
# It currently takes no options - you have to change the variables in the script to change its behavior.
# the options are described inline below.

use vars qw(
$ncbo
$ncwa_atomic_INS $bytes_in $bytes_out $bytes_per_val $ncwa_compute_sec_nowt
$dim_sz $e $ncwa_Icyc_per_el $r $timestamp $INS_per_Int_op $INS_per_FP_op
$ncwa_compute_sec_wt $cpu_clock_freq $disk_bw_read $disk_bw_write $disk_time_in
$disk_time_out $disk_tot_IO_time $int_coeff $ncwa_est_flops $ncwa_int_INS_nowt $ncwa_int_INS_wt
$ncwa_memcpy_INS_nowt  $ncwa_memcpy_per_el_nowt $ncwa_memcpy_per_el_wt $N
$nbr_files_in $nbr_files_out $nbr_vars $ncwa_NOWT_INS_multiplier $rank $rank @dim_vars
$ncwa_tot_cyc_mlt_nowt $ncwa_tot_cyc_mlt_wt $ncwa_TOT_CYC_nowt $ncwa_TOT_CYC_wt $ncwa_TOT_INS_multiplier $ncbo_Icyc_per_ins $ncbo_Int_INS_per_el $stl_5km $out_fl_nme $sz_av_bl
$V_flops $V_int_INS $ncwa_wallclock_sec_nowt $ncwa_wallclock_sec_wt $weighting $V_intops
$ncwa_weighting_correction $ncwa_weighting_multiplier $ncwa_WT_INS_multiplier $R_wght

 $K_Ia  $K_Mu  $F_cnt  $Ia_cnt  $Mu_cnt  $Ms_cnt  $fp_time  $Itot_cnt $sz_wght_bl
 $Int_time $ncwa_int_time $ncwa_int_count
);

# initialization - vars that user must set/check to generate a valid run
$ncbo                       = 1; # 1 for doing the ncbo routines, 0 for ncwa
$weighting                  = 0; # 1 for weighting, 0 for NOT weighting
$stl_5km                    = 1; # 1 if the file is the stl_5km file, 0 for the ipcc file

$dim_sz                     = 1; # simple initializ'n for the dimension size
$ncbo                       = 1; # 1 for doing the ncbo routines, 0 for ncwa
$weighting                  = 1; # 1 for weighting, 0 for NOT weighting
$stl_5km                    = 1; # 1 if the file is the stl_5km file, 0 for the ipcc file

# Vars to set relating to the size and layout of the test files.
# These should be set correctly unless you want to test files other than the std files
# ie stl_km or ipcc
if ($stl_5km) {               # lat   lon
	$rank = @dim_vars         = (2160, 4320);
   $sz_wght_bl               = 2160; # stl_5km - scalar sz of the weighting var (ncwa only)-assuming lat
	$R_wght                   = 1; # rank of the weighting block
   $sz_av_bl                 = 2160 * 4320; # N sub A in ppr
	$nbr_vars                 = 8;   # nbr of variables (assuming all same size)
	$out_fl_nme               = "ddra.stl_5km.";
} else { # assume ipcc file            lat  lon
	$rank = @dim_vars         = (8, 32, 128, 256); # rank of vars that are >94% of the file
	$sz_wght_bl               = 128; # ipcc - scalar sz of the weighting var (ncwa only)- assuming lat here
	$R_wght                   = 1; # rank of the weighting block
	$sz_av_bl                 = 8 * 32 * 128 * 256; # N sub A in ppr
	$nbr_vars                 = 32; # for most significant vars in ipcc - 94% of file
	$out_fl_nme               = "ddra.ipcc.";
}

# Nmbr of files in and out - these vars should not have to be changed unless you want
# to change the basis for the tests.
if ($ncbo) {
	$nbr_files_in             = 2;  # number of files in - 2 for ncbo, 1 for ncwa
	$out_fl_nme .= "ncbo";
#	$INS_per_Int_op              = 2.0;   #  NCBO - calculated from tacg utility
} else {
	$nbr_files_in             = 1;
	$out_fl_nme              .= "ncwa";
	# following calculations imply that int ops are 18-28 times faster than FP ops - could be.
}
$nbr_files_out              = 1; # number of files out

# the value $INS_per_Int_op is not used in this script but I've left it in as a reminder.  Contrary to
# what I thought before, it seems to be constant across both ncbo and ncwa.
#	$INS_per_Int_op              = 2.0;   #  NCBO - calculated from tacg utility
# similarly, this should not change - we can measure it directly..
#$INS_per_FP_op               = 7.0;   # direct measure from PAPI for both ncbo AND ncwa



# following vars are set for CLAY and should be relatively stable unless changing machines

# I'm retracting my previous estimate - I thin the bonnie estimates work better across the
# range of tests we're doing.
$disk_bw_read               = 63.375e6; # fr disk read bandwidth in bytes/s (bonnie++)
$disk_bw_write              = 57.865e6; # fr disk write bandwidth in bytes/s (bonnie++)
#$disk_bw_read               = 100e6; # value from to or from memory - theoretical max
#$disk_bw_write              = 60e6;  # value from to or from memory - theoretical max

$bytes_per_val              = 4;   #change if using doubles or other non-NC_FLOAT sizes
$cpu_clock_freq             = 2e9; # CPU clock in Hz

$V_flops                    = 153e6; # floating point speed derived from direct PAPI measures
$V_intops                   = 200e6; # int speed from direct measure, using a large data set

# the following estimates are from local variables and as such are highly optimized and therefore
# inapplicable to the nco benchmarks.
#$V_flops                    = 353e6; # floating point speed derived from tacg measures
#$V_intops                   = 1386e6; # integer speed derived from tacg measures
#$V_flops                    = 153e6; # floating point speed derived directly from PAPI
#$V_flops                    = 1017e6; # floating point speed derived from BOINC measures
#$V_intops                   = 2147e6; # integer speed derived from BOINC measures

for (my $r=0; $r<$rank; $r++){$dim_sz *= $dim_vars[$r];} # total scalar size of all dimensions
# $N = total nbr of els - only for those calcs that do not run thru the
# N calculations in the loop below
$N = $nbr_vars * $dim_sz;

my $time;
$timestamp = `date "+%a%d%b%g-%R"`; chomp $timestamp;
# output Gnuplot data (GPD) autonamed and timestamped
open(GPD, ">$out_fl_nme.$timestamp.gpdata") or die "\nUnable to open GPD file\n";

# this stanza does ncwa for an initial data size of 10MB up to 80.(steps of 8)
if (!$ncbo) {
	my $WGT = "weighted";
	if ($weighting == 0) {$WGT = "unweighted";}

		print GPD  "#ncwa $WGT \n#         bytes        ops         ops         ops         ops        ops           sec        sec\nCycle   DataSize     Floating    Int_math    Mem_user     Mem_sys   Total_Int   FP_time    Int_time     I/O       Tot_Time\n";

		print "#ncwa $WGT \n#         bytes        ops         ops         ops         ops        ops           sec        sec\nCycle   DataSize     Floating    Int_math    Mem_user     Mem_sys   Total_Int   FP_time    Int_time     I/O       Tot_Time\n";

	for (my $e=1; $e<=$nbr_vars; $e++) {
		my $dsz = $e * $dim_sz;
		# re-arranged to better match ZeM06 equations

		if ($weighting == 0) { # WITHOUT weighting
			$K_Ia = 1; # fudge for Ia
			$K_Mu = 1; # fudge for Mu
			$F_cnt = $dsz * (1 + (1/$sz_av_bl));                                             # 20a
			# BS now included in eq below
			$Ia_cnt = $K_Ia * ($dsz * ((9 * $rank) + 9 + (4 / $sz_av_bl)));                  # 20b
			$Mu_cnt = $K_Mu * ($dsz * ((5 * $rank) + 3 + ($bytes_per_val + 2) / $sz_av_bl)); # 20c
			$Ms_cnt = $dsz;                                                                  # 20d
		} else { # WITH weighting
			$K_Ia = 1; # fudge for Ia
			$K_Mu = 1; # fudge for Mu
			$F_cnt = $dsz * (3 + (2/$sz_av_bl));                                             # 20a
			# BS now included in eq below
			$Ia_cnt = $K_Ia * ($dsz * ((22*$rank) + (5*$R_wght) + 18 + (7/$sz_av_bl)) + (2*$sz_wght_bl)); # 20b
			$Mu_cnt = $K_Mu * ($dsz * ((12*$rank) + (3*$R_wght) + 4 + ($bytes_per_val + 4) / $sz_av_bl) + ($bytes_per_val*$sz_wght_bl)); # 20c
			$Ms_cnt = 3*$dsz;                                                                  # 20d
		}
		$bytes_in  = $dsz * $bytes_per_val * $nbr_files_in;

		# for averaging (rank reduction) file out will be less than input file size by the size of the
		# averaging variable - $sz_wght_bl
		$bytes_out = ($dsz/$sz_wght_bl) * $bytes_per_val * $nbr_files_out;
		$disk_time_in = $bytes_in / $disk_bw_read;
		$disk_time_out = $bytes_out / $disk_bw_write;
		$disk_tot_IO_time = $disk_time_in + $disk_time_out;
#		printf "bytes in=  %1.4e \$disk_time_in = %1.4e \nbytes out= %1.4e \$disk_time_out = %1.4e \n\$disk_tot_IO_time =  %1.4e\n", $bytes_in, $disk_time_in, $bytes_out, $disk_time_out, $disk_tot_IO_time;
		$fp_time = $F_cnt / $V_flops;
		$Itot_cnt = $Ia_cnt + $Mu_cnt + $Ms_cnt;
		$Int_time = $Itot_cnt / $V_intops; # remember that $V_intops is under some suspicion
		$time = $disk_tot_IO_time + $fp_time  + $Int_time;

		#         IO                IO             FP term                 Int term
		printf     "%d       %1.3e   %1.3e   %1.3e   %1.3e   %1.3e  %1.3e   %1.3e  %1.3e   %1.3e   %1.3e\n", $e,      $dsz,   $F_cnt,  $Ia_cnt,   $Mu_cnt,  $Ms_cnt,  $Itot_cnt, $fp_time, $Int_time, $disk_tot_IO_time,  $time ;
		printf GPD "%d       %1.3e   %1.3e   %1.3e   %1.3e   %1.3e  %1.3e   %1.3e  %1.3e   %1.3e   %1.3e\n", $e,      $dsz,   $F_cnt,  $Ia_cnt,   $Mu_cnt,  $Ms_cnt,  $Itot_cnt, $fp_time, $Int_time, $disk_tot_IO_time,  $time ;
	}
}


if ($ncbo) {
	print GPD "#ncbo - NB: FP op counts = DataSize\n
Cycle   DataSize     T_fp     T_int    T_I/O      T_tot      Int_ops\n";

	print "#ncbo - NB: FP op counts = DataSize\n
Cycle   DataSize     T_fp     T_int    T_I/O      T_tot      Int_ops\n";

	for (my $e=1; $e<=$nbr_vars; $e++) {
		my $dsz = $e * $dim_sz;
		$bytes_in  = $dsz * $bytes_per_val * $nbr_files_in;
		$bytes_out = $dsz * $bytes_per_val * $nbr_files_out; # don't have to consider rank reduction

		$disk_time_in = $bytes_in / $disk_bw_read;
		$disk_time_out = $bytes_out / $disk_bw_write;
		$disk_tot_IO_time = $disk_time_in + $disk_time_out;

		# match ZeM06 equations, unweighted
		my $fp_cnts = $dsz;
		my $fp_time = ($fp_cnts/$V_flops); # remember that $V_flops is under some suspicion

		my $K_Ia = 1; # fudge for Ia
		my $K_Mu = 1; # fudge for Mu (Mu may have to change relative to
		my $Int_cnt =  $K_Ia *   6 * $dsz;
		my $Mu_cnt =   $K_Mu *   3 * $bytes_per_val * $dsz;
		my $Ms_cnt = 0;
		my $ncbo_int_op_cnts =  $Int_cnt + $Mu_cnt +  $Ms_cnt;
		my $ncbo_int_time = $ncbo_int_op_cnts / $V_intops; # remember that $V_intops is under some suspicion

		#         IO                 FP term                 Int term
		$time = $disk_tot_IO_time + $fp_time + $ncbo_int_time;
		#print "$e $bytes_in $bytes_out  $disk_tot_IO_time $fp_time  $ncbo_int_time $ncbo_Icyc_per_el\n";
		printf     " %2d    %1.3e   %6.3f    %6.3f    %6.3f    %6.3f    %1.3e\n",  $e, $dsz, $fp_time, $ncbo_int_time, $disk_tot_IO_time, $time, $ncbo_int_op_cnts;
		printf GPD " %2d    %1.3e   %6.3f    %6.3f    %6.3f    %6.3f    %1.3e\n",  $e, $dsz, $fp_time, $ncbo_int_time, $disk_tot_IO_time, $time, $ncbo_int_op_cnts;
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


