#!/usr/bin/perl

# $Header$

# Purpose: Clear machine cache by loading memory with useless data
# Once cache is clear, benchmarking may proceed without risk that
# previous input files would already be in cache

# Usage:
# ~/nco/bm/cch_clr.pl

# Copy code to test machines:
# scp ~/nco/bm/cch_clr.pl clay.ess.uci.edu:nco/bm

my $mmr_sz=80000000; # 80.0e06 elements = 320.0e06 Bytes or 720.0e06 Bytes
my @big_arr;
print "Starting to load big_arr of size $mmr_sz\n";
for (my $mmr_idx=0;$mmr_idx<$mmr_sz;$mmr_idx++){
  $big_arr[$mmr_idx] = $mmr_idx;
} # end loop over mmr_sz
print "Finished loading big_arr...File cache should be clear!\n";
