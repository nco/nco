#!/usr/bin/perl
use strict;
# Declare vars for strict

# This script is used to gnerate run times based on Zender's DDRA theory paper of 2006
# It currently takes no options - you have to change the variables in the script to change its behavior.
# the options are described inline below.

use vars qw(
$ncbo
$ncwa_atomic_INS $bytes_in $bytes_out $bytes_per_val $ncwa_compute_sec_nowt
$dim_sz $e $ncwa_Icyc_per_el $r $timestamp
$ncwa_compute_sec_wt $cpu_clock_freq $disk_bw_read $disk_bw_write $disk_time_in
$disk_time_out $disk_tot_IO_time $int_coeff $ncwa_est_flops $ncwa_int_INS_nowt $ncwa_int_INS_wt
$ncwa_memcpy_INS_nowt $ncwa_memcpy_INS_wt $ncwa_memcpy_per_el_nowt $ncwa_memcpy_per_el_wt $N
$nbr_files_in $nbr_files_out $nbr_vars $ncwa_NOWT_INS_multiplier $rank $rank @dim_vars
$ncwa_tot_cyc_mlt_nowt $ncwa_tot_cyc_mlt_wt $ncwa_TOT_CYC_nowt $ncwa_TOT_CYC_wt $ncwa_TOT_INS_multiplier $ncbo_Icyc_per_ins $ncbo_Int_INS_per_el $stl_5km $out_fl_nme
$V_flops $V_int_INS $ncwa_wallclock_sec_nowt $ncwa_wallclock_sec_wt $weighting
$ncwa_weighting_correction $ncwa_weighting_multiplier $ncwa_WT_INS_multiplier $ncwa_wt_var_sz
);

# initialization - vars that user must set/check to generate a valid run
$dim_sz                     = 1; # simple initializ'n for the dimension size
$ncbo                       = 1; # 1 for doing the ncbo routines, 0 for ncwa
$weighting                  = 1; # 1 for weighting, 0 for NOT weighting
$stl_5km                    = 1; # 1 if the file is the stl_5km file, 0 for the ipcc file

# Vars to set relating to the size and layout of the test files.
# These should be set correctly unless you want to test files other than the std files
# ie stl_km or ipcc
if ($stl_5km) {
	$rank = @dim_vars         = (2160, 4320);
   $ncwa_wt_var_sz           = 2160; # stl_5km - scalar sz of the weighting var (ncwa only)
	$nbr_vars                 = 8;   # nbr of variables (assuming all same size)
	$out_fl_nme               = "ddra.stl_5km.";
} else { # assume ipcc file
	$rank = @dim_vars         = (8, 32, 128, 256); # rank of vars that are >94% of the file
	$ncwa_wt_var_sz           = 128; # ipcc set to scalar sz of weighting var (ncwa only)
	$nbr_vars                 = 32; # for most significant vars in ipcc - 94% of file
	$out_fl_nme               = "ddra.ipcc.";
}

# Nmbr of files in and out - these vars should not have to be changed unless you want
# to change the basis for the tests.
if ($ncbo) {
	$nbr_files_in   = 2;  # number of files in - 2 for ncbo, 1 for ncwa
	$out_fl_nme .= "ncbo";
} else {
	$nbr_files_in         = 1;
	$out_fl_nme .= "ncwa";
}
$nbr_files_out              = 1; # number of files out


# following vars are set for CLAY and should be relatively stable unless changing machines

# following disk i/o values are from raw disk i/o writing to disk from memory (/dev/zero)
# and reading from disk and sending the data to /dev/null.  While not as 'realistic' as bonnie++,
# these numbers fit better for the kind of disk i/o that the nco's use, whih is tyically
# very large reads and writes
$disk_bw_read               = 100e6; # was 87 fr disk read bandwidth in bytes/s (bonnie++)
$disk_bw_write              = 60e6; # was 43 fr disk write bandwidth in bytes/s (bonnie++)

$bytes_per_val              = 4;   #change if using doubles or other non-NC_FLOAT sizes
$cpu_clock_freq             = 2e9; # CPU clock in Hz
$V_flops                    = 153e6; # floating point speed - Xpt-derived form PAPI
$V_int_INS                  = 562e6; # Xpt-derived - speed of INS NOT used by FP ops - includes both memory and array offset calculations.

# following multipliers are constants derived to allow conversion from measurements PAPI can supply
# (INStructions, CYCles, and FP (floating point operations)) into the values that the DDRA requires.
# These are multiplied by the INS count to derive INTEGER operations
# $ncwa_TOT_INS_multiplier    = 100; # Xpt-derived by elimination - used anymore??
$ncwa_WT_INS_multiplier     = 0.89; #Xpt-derived by elimination
$ncwa_NOWT_INS_multiplier   = 0.9285; # Xpt-derived
$ncwa_tot_cyc_mlt_wt        = 90; # Xpt-derived
$ncwa_tot_cyc_mlt_nowt      = 45; # Xpt-derived (could also be ~$ncwa_tot_cyc_mlt_wt/2)
$ncwa_memcpy_per_el_wt      = 54; # Xpt-derived
$ncwa_memcpy_per_el_nowt    = 24.4; # Xpt-derived (could also be ~$memcpy_mlt_wt/2)
$ncbo_Int_INS_per_el        = 43.9; # nbr of integer CYC per el for ncbo (adjusted for the INS
												# assigned to FP operations)
$ncbo_Icyc_per_ins           = 0.693 ; # 2.01851e+09 / 2.91069e+09; derived from Xpt - shows pipeline effect for CPU in which more than one instruction gets executed per cycle.

# Calculated Vars direct from above
for (my $r=0; $r<$rank; $r++){$dim_sz *= $dim_vars[$r];} # total scalar size of all dimensions
# $N = total nbr of els - only for those calcs that do not run thru the
# N calculations in the loop below
$N = $nbr_vars * $dim_sz;
$ncwa_weighting_multiplier = $weighting * 2 * (($N + $ncwa_wt_var_sz)/$N);
$ncwa_weighting_correction = 1 + $ncwa_weighting_multiplier;

$bytes_in  = $N * $bytes_per_val * $nbr_files_in;
$bytes_out = $N * $bytes_per_val * $nbr_files_out;

$disk_time_in = $bytes_in / $disk_bw_read;
$disk_time_out = $bytes_out / $disk_bw_write;
$disk_tot_IO_time = $disk_time_in + $disk_time_out;

$ncwa_atomic_INS = $N * $ncwa_weighting_multiplier * $ncwa_TOT_INS_multiplier; # <- used anymore?
$ncwa_int_INS_wt = $ncwa_atomic_INS * $ncwa_WT_INS_multiplier;
$ncwa_int_INS_nowt = $ncwa_atomic_INS * $ncwa_NOWT_INS_multiplier;
$ncwa_est_flops = $N * $ncwa_weighting_correction;
#$ncwa_TOT_CYC_wt = $N * $ncwa_tot_cyc_mlt_wt; # <- used anymore?
$ncwa_TOT_CYC_nowt = $N * $ncwa_tot_cyc_mlt_nowt;
$ncwa_memcpy_INS_wt = $N * $ncwa_memcpy_per_el_wt;
$ncwa_memcpy_INS_nowt = $N * $ncwa_memcpy_per_el_nowt;
$ncwa_compute_sec_wt = $ncwa_TOT_CYC_wt / $cpu_clock_freq;
$ncwa_compute_sec_nowt = $ncwa_TOT_CYC_nowt / $cpu_clock_freq;
$ncwa_wallclock_sec_wt = $disk_tot_IO_time + $ncwa_compute_sec_wt;
$ncwa_wallclock_sec_nowt = $disk_tot_IO_time + $ncwa_compute_sec_nowt;

if ($weighting == 0) {$int_coeff = 4.221;} # no_weighting coefficient is smaller due to less mem ops
else { $int_coeff = 8.56;} # weighting coefficient (also ~2x no_weighting)


my $time;
$timestamp = `date "+%a%d%b%g-%R"`; chomp $timestamp;
# output Gnuplot data (GPD) autonamed and timestamped
open(GPD, ">$out_fl_nme.$timestamp.gpdata") or die "\nUnable to open GPD file\n";

# this stanza does ncbo for an initial data size of 10MB up to 80.(steps of 8)
if (!$ncbo) {
	my $WGT = "weighted";
	if ($weighting == 0) {$WGT = "unweighted";}
	# weighted
	print GPD "#ncwa $WGT - 8 data cycles\nCycle   DataSize  ncwa_est_flops    Time\n";
	print "#ncwa $WGT - 8 data cycles\nCycle   DataSize  ncwa_est_flops    Time\n";
#   print "\$e    \$dsz      \$time        \$ncwa_est_flops    \$ncwa_int_count\n";
	for (my $e=1; $e<=$nbr_vars; $e++) {
		my $dsz = $e * $dim_sz;
		$bytes_in  = $dsz * $bytes_per_val * $nbr_files_in;
		$bytes_out = $dsz * $bytes_per_val * $nbr_files_out;
		$disk_time_in = $bytes_in / $disk_bw_read;
		$disk_time_out = $bytes_out / $disk_bw_write;
		$disk_tot_IO_time = $disk_time_in + $disk_time_out;
		$ncwa_weighting_multiplier = $weighting * 2 * (($dsz + $ncwa_wt_var_sz) / $dsz);
		$ncwa_weighting_correction = 1 + $ncwa_weighting_multiplier;
		$ncwa_est_flops =  $dsz * $ncwa_weighting_correction;
		$ncwa_TOT_CYC_wt = $e *  $ncwa_tot_cyc_mlt_wt;
		my $fp_time = ($ncwa_est_flops/$V_flops);
		my $ncwa_int_time; my $ncwa_int_count;
		if ($weighting == 0) {
		# next line is for unweighted averaging
			# the final '*2' is the conversion factor from operations to cycles
			$ncwa_int_count = ((14 * $rank * $dsz) + (4 * $dsz)) * 2 ;
			$ncwa_int_time = ($ncwa_int_count * $int_coeff)/ $cpu_clock_freq;
		} else {
		# next line is for weighted averaging
			$ncwa_int_count = (((14 * $rank * $dsz) + (10 * $dsz)) + 5) * 3;
			$ncwa_int_time = ($ncwa_int_count * $int_coeff)/  $cpu_clock_freq;
		}
		#my $ncwa_int_term = ($dsz * $ncwa_TOT_CYC_wt / $cpu_clock_freq); #weighting
		#         IO                IO             FP term                 Int term
		$time = $disk_tot_IO_time + $fp_time  + $ncwa_int_time;
		#print "$e  $bytes_in  $disk_tot_IO_time   $time   $fp_time   $ncwa_int_term \n";
		printf     "%d       %1.3e   %1.3e   %1.3e \n", $e, $dsz, $ncwa_est_flops, $time ;
#		print "$e     $dsz    $time   $ncwa_est_flops     $ncwa_int_count \n";
		printf GPD "%d       %1.3e   %1.3e   %1.3e \n", $e, $dsz, $ncwa_est_flops, $time ;
	}
}


if ($ncbo) {
	print GPD "#ncbo - 8 data cycles\nCycle   DataSize  time_p    time_m   Time_m_sem\n";
	print "#ncbo - 8 data cycles\nCycle  DataSize     T_fp     T_int     T_Disk    T_tot     Int_counts\n";
	for (my $e=1; $e<=$nbr_vars; $e++) {
		my $dsz = $e * $dim_sz;
		$bytes_in  = $dsz * $bytes_per_val * $nbr_files_in;
		$bytes_out = $dsz * $bytes_per_val * $nbr_files_out;
		$disk_time_in = $bytes_in / $disk_bw_read;
		$disk_time_out = $bytes_out / $disk_bw_write;
		$disk_tot_IO_time = $disk_time_in + $disk_time_out;
		my $fp_time = ($dsz/$V_flops);
		my $ncbo_int_counts = $dsz * $ncbo_Int_INS_per_el;
		my $ncbo_int_time = $ncbo_int_counts * $ncbo_Icyc_per_ins /$cpu_clock_freq;

		#         IO                 FP term                 Int term
		$time = $disk_tot_IO_time + $fp_time + $ncbo_int_time;
		#print "$e $bytes_in $bytes_out  $disk_tot_IO_time $fp_time  $ncbo_int_time $ncbo_Icyc_per_el\n";
		printf     " %2d    %1.3e   %6.3f    %6.3f    %6.3f    %6.3f    %1.3e\n",  $e, $dsz, $fp_time, $ncbo_int_time, $disk_tot_IO_time, $time, $ncbo_int_counts;
		printf GPD " %2d    %1.3e   %6.3f    %6.3f    %6.3f    %6.3f    %1.3e\n",  $e, $dsz, $fp_time, $ncbo_int_time, $disk_tot_IO_time, $time, $ncbo_int_counts;
		# "%d  %e  %6.3f \n", $e, $dsz, $time ;
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


