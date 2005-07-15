#!/usr/bin/env perl

# $Header: /data/zender/nco_20150216/nco/bm/mk_bm_plots.pl,v 1.2 2005-07-15 18:37:00 mangalam Exp $

# use this script to plot the data from the daemon-recorded benchmark info
# located at: sand:/var/log/nco_benchmark.log
# That log needs to be filtered from the commandline via the parameters you want,
# typically by host, 'bench', possibly specific operator or thread number.
# The filtered data can be piped into this script which currently expects a data format like:
# commandline|timing data (the last 2 fields of the nco_benchmark.log) like so
# grep sand nco_benchmark.log |grep bench|cut -d'|' -f'2,3' | ./mk_bm_plots.pl

# NB: tho note that the format of the UDP records is changing so there will probably be failures if 
# the UDP format and this file are not kep in sync.

# Currently runs gnuplot on the datafiles it generates to produce a postscript file 
# that is date-stamped in the filename and noted at the end of the run eg:
# nco.benchmarks_2005-07-14+22:37.ps
# It also produces a number of operator-specific data files on each run so beware that 
# they'll add up quickly if you run it multiple times:

# nco_bm.2005-07-14+22:37.gnuplot (the gnuplot instructions)
# ncap.2005-07-14+22:37.gnuplot
# ncbo.2005-07-14+22:37.gnuplot
# ncea.2005-07-14+22:37.gnuplot
# ncecat.2005-07-14+22:37.gnuplot
# ncflint.2005-07-14+22:37.gnuplot
# ncpdq.2005-07-14+22:37.gnuplot
# ncra.2005-07-14+22:37.gnuplot
# ncrcat.2005-07-14+22:37.gnuplot
# ncwa.2005-07-14+22:37.gnuplot

require 5.6.1 or die "This script requires Perl version >= 5.6.1, stopped";
#use Getopt::Long; # GNU-style getopt #qw(:config no_ignore_case bundling);
use strict; # Protect all namespaces

# Declare vars for strict
use vars qw( @cmdline @nco_tim_info $thr_num %nc %tim_dta $num_nco_stz @nco_stz @clin_bits
$num_bits @nco_stz $num_nco_stz $nco_name @nco_tim_dta $gnuplot_data_file @nco_name_array
$tim_dta_end $cmdfile $ps_file $filetimestamp
);
$thr_num = 0;   
$tim_dta_end = 5; # number of variables to be plotted (to expand if start adding more rusage() vars)
$filetimestamp = `date +%F_%T`; chomp $filetimestamp;
$filetimestamp =~ s/://g; # ':' interfere with scp

#grep bench nco_benchmark_log_file |grep AIX (or whatever OS you want) | scut --c1="2 3" --id1='\|' --od='\' > output_file
# will yeild : commandline  ]  nco timing stuff
# where ' ] ' is the output delimiter (could be anything)
my $linect = 0;
while (<>) {
	if ($_ =~ '^#') {
#		print "skipping line $linect: $_\n";
	} else { # split the line on the '|'s into 
#		print "\n\nworking on: $_\n";
		($cmdline[$linect],$nco_tim_info[$linect]) = split(/\|/,$_,2);
		#my $splitcnt = split(/]/,$_);
		#print "splitcnt = $splitcnt\n";
#		print "cmd: $cmdline[$linect]\n";
#		print "timing data: $nco_tim_info[$linect]\n";
	}
	#print "should wait here\n";
	#my $wait = <STDIN>;
	$linect++;
	
}
print "processed $linect lines\n";

for (my $i=0; $i<$linect;$i++) {
	# process the commandline to see how many threads were requested
	if ($cmdline[$i] =~ "thr") {
#		print "working on $cmdline[$i]\n";
		$num_bits = @clin_bits = split /\s/,$cmdline[$i]; # split the cmdline into ws-delim bits
#		# then look for the thread number
#		print "numbits = $num_bits\n";
		my $notfound = 1;
		my $bit_cnt = 0;
		while ($bit_cnt < $num_bits && $notfound) {
			if ($clin_bits[$bit_cnt] =~ "thr") {
				my @tn = split(/[= ]/,$clin_bits[$bit_cnt]);
				$thr_num = $tn[1];
#				print "threads = $thr_num \n";
				$notfound = 0;
			}
			$bit_cnt++;
		}
	} else {
#		print "line $i [$cmdline[$i]] has no thread spec - treat as special case of 0\n";
		$thr_num = 0;
		# yadda yada yadda
	}
	# now process the timing info into %tim_dta{nco_name}{time_type}{bm_run_date?}
	# split the line into nco stanzas
#	print "timing info: $nco_tim_info[$i]\n";
	chomp $nco_tim_info[$i]; chop $nco_tim_info[$i];
#	print "post-chop, timing info: $nco_tim_info[$i]^\n";
	
	$num_nco_stz = @nco_stz = split(/:/, $nco_tim_info[$i]);
	my $cnt = 0;
	foreach my $chunk (@nco_stz) {
#		$chunk =~ s/,/ /g;
		$chunk =~ s/\s+/ /g;
#		print "processed chunk = $chunk\n";
		   @nco_tim_dta = split(/\s+/,$chunk); 
		   $nco_name_array[$cnt] = $nco_name = $nco_tim_dta[0];
		my $walltime = $nco_tim_dta[3]; # wall = [0]
		my $realtime = $nco_tim_dta[4]; # real = [1]
		my $usertime = $nco_tim_dta[5]; # user = [2]
		my $systime  = $nco_tim_dta[6]; # sys  = [3]
		
		# @nco_tim_dta should now have [name][#success][#fail][wall][real][user][sys]
		# so copy the tasty bits into the big array
#		print "i = $i\n";
		$tim_dta{$nco_name}[0][$i] = $thr_num; 
		$tim_dta{$nco_name}[1][$i] = $walltime; 
		$tim_dta{$nco_name}[2][$i] = $realtime; 
		$tim_dta{$nco_name}[3][$i] = $usertime; 
		$tim_dta{$nco_name}[4][$i] = $systime; 
		#print "for: $nco_tim_dta[0]\nwall = $tim_dta{$nco_name}[0][$linect]\nreal = $tim_dta{$nco_name}[1][$linect]\nuser = $tim_dta{$nco_name}[2][$linect]\nsyst = $tim_dta{$nco_name}[3][$linect]\n\n";
		$cnt++;
	}
}

write_gnuplot_cmds();
write_nco_data();
print "\nexecuting the gnuplot on $cmdfile\n";
system "gnuplot $cmdfile";
print "\n\nThe benchmark scripts should be in $ps_file\n\n";

sub write_nco_data {
# need to write one datafile for each nco of the form:
# thread   wall  real  user  sys   and   other   rusage   params    spread    across    the    top
# and then 1 gnuplot command file to read them all in and plot them
	#open the file
	for (my $r=0; $r<$num_nco_stz;$r++) {
		$nco_name = $nco_name_array[$r];
		my $datafile = "$nco_name.$filetimestamp.gnuplot";
		print "datafile name: $datafile\n";
		open(DAT, ">$datafile") or die "\nUnable to open command file '$datafile' in current dir - check permissions on it\nor the directory you are in.\n stopped";

		# now plot the header line:
		printf DAT "    threads       wall       real       user    system\n";
		for (my $i=0; $i<$linect; $i++) {
			printf DAT "%10d ",$tim_dta{$nco_name}[0][$i];
			for (my $e=1; $e<$tim_dta_end; $e++) { printf DAT "%10.3f ",$tim_dta{$nco_name}[$e][$i]; }
			print DAT "\n";
		}
	}
}


# now munge into appro formats
# 1st write the gnuplot commands to read in and plot the file - should be a separate fn().
sub write_gnuplot_cmds {
	# need to write 1 command file that plots all the files to a single postscript file
	#open file
	$cmdfile = "nco_bm.$filetimestamp.gnuplot";
	$ps_file = "nco.benchmarks_$filetimestamp.ps";
	print "cmdfile name: $cmdfile\n\n";
	open(CMD, ">$cmdfile") or die "\nUnable to open command file '$cmdfile' in current dir - check permissions on it\nor the directory you are in.\n stopped";
	print CMD << "HEADER";
# auto-generated data for nco benchmark plots to postscript 
#  created $filetimestamp
# called from this perl script as: system ("gnuplot $cmdfile");
# input file has the format:<nco>.datestamp.gnuplot
set xrange [-1:9]
set xlabel "Number of threads or processes"
set ylabel "Execution time (s)"
set key box
set terminal postscript landscape color
set output '$ps_file'
HEADER

	# now write the plot cmds to plot each column
	for (my $r=0; $r< $num_nco_stz; $r++){
		my $tail = ", \\\n";
		#print "set output 'bench.$nco_name_array[$r].ps'\n"
#		print "\n\n";
		print CMD "set title 'Benchmark for $nco_name_array[$r] using multiple threads'\n";
		my $datafile = "$nco_name_array[$r].$filetimestamp.gnuplot";
		for (my $e=0; $e<($tim_dta_end-1);$e++) {
			my $plot_str = "plot";
			if ($e > 0) {$plot_str = "";}
			if ($e > ($tim_dta_end-3)) { $tail = "\n"; }
			my $col = $e + 2;
			print CMD "$plot_str '$datafile' using 1:$col title $col$tail";
		}
	}
	close (CMD);
}





