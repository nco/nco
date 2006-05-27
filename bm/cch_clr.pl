#!/usr/bin/perl

# $Header: /data/zender/nco_20150216/nco/bm/cch_clr.pl,v 1.2 2006-05-27 20:39:49 zender Exp $

# Purpose: Clear machine cache by loading memory with useless data
# Once cache is clear, benchmarking may proceed without risk that
# previous input files would already be in cache

my $mmr_sz=80000000; # 200 els
my @big_arr;
print "Starting to load big_arr of size $mmr_sz\n";
for (my $mmr_idx=0;$mmr_idx<$mmr_sz;$mmr_idx++){
  $big_arr[$mmr_idx] = $mmr_idx;
} # end loop over mmr_sz
print "Finished loading big_arr...File cache should be clear!\n";
