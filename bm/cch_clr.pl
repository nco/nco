#!/usr/bin/perl
my $memsize =   80000000; # 200 els
my @bigarray;
print "Starting to load bigarray of size $memsize\n";
for (my $r=0; $r<$memsize; $r++) {
  $bigarray[$r] = $r;
}
print "Have we blown up yet?  Finished loading bigarray\n\n";
print "File cache should be clear!\n";
