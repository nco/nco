#!/usr/bin/env perl

# Usage:  (see usage() below for more info)
# <BUILD_ROOT>/nco/bld/nco_tst.pl # Tests all operators
# <BUILD_ROOT>/nco/bld/nco_tst.pl ncra # Test one operator

# NB: When adding tests, _be sure to use -O to overwrite files_
# Otherwise, script hangs waiting for interactive response to overwrite queries
require 5.6.1 or die "this script requires Perl version >= 5.6.1";
use Cwd 'abs_path';
#use IO::Socket;
use Getopt::Long; #qw(:config no_ignore_case bundling);
use strict;

#test nonfatally for useful modules
my $hiresfound;
BEGIN {eval "use Time::HiRes qw(usleep ualarm gettimeofday tv_interval)"; $hiresfound = $@ ? 0 : 1}
#$hiresfound = 0;  # uncomment to simulate not found
if ($hiresfound == 0) {
    print "\nOoops! Time::HiRes (needed for accurate timing) not found\nContinuing without timing.";
} else {
    print "Time::HiRes ... found!\n";
}
my $iosockfound;
BEGIN {eval "use IO::Socket"; $iosockfound = $@ ? 0 : 1}
#$iosockfound = 0;  # uncomment to simulate not found
if ($iosockfound == 0) {
    print "\nOoops! IO::Socket module not found - continuing with no udp logging.\n";
} else {
    print "IO::Socket ... found!\n\n";
}

my %subbenchmarks;
my %totbenchmarks;

#declare vars for strict
use vars qw($dbg_lvl $wantlog $usage $operator @test $description $expected
@all_operators @operators $MY_BIN_DIR %sym_link %testnum %success %failure
%testnum $result $server_name $server_ip $server_port $sock $udp_report
);

$server_name = "ibonk";
#$server_ip = "128.200.14.132";
$server_ip = "68.5.247.102";
$server_port = 29659;
$udp_report = 0;

$dbg_lvl = 0; # [enm] Print tests during execution for debugging
$wantlog = 0;
$usage   = 0;

&GetOptions("debug=i"    => \$dbg_lvl,    # debug level
				"log"        => \$wantlog,    # set if want output logged
				"udpreport"  => \$udp_report,
				"usage"      => \$usage,      # explains how to use this thang
				"help"       => \$usage,      # explains how to use this thang
				"h"          => \$usage,      # explains how to use this thang
);

if ($usage) { usage()};

if ($iosockfound) {
	   $sock = IO::Socket::INET->new (
		Proto    => 'udp',
		PeerAddr => $server_ip,
		PeerPort => $server_port
		) or die "\nCan't get the socket!\n\n";
} else {$udp_report = 0;}

if ($wantlog) { 
	open(LOG, ">nctest.log") or die "\nCan't open the log file 'nctest.log' - check permissions on it \nor the directory you're in.\n\n";
}

&initialize();
&perform_tests();
&summarize_results();

sub perform_tests
{
# Tests are in alphabetical order by operator name

# The following tests are organized and laid out as follows:
# - $test[] holds the commandlines for each of the nco's being tested
# - $description  holds the description line for the test
# - $expected is the string or expression that is the result of
#     executing the nco
# - go() is the function that actually executes each test



####################
#### ncap tests ####
####################
$operator="ncap";
####################
$test[0]='ncap -O -v -S ncap.in in.nc foo.nc';
$description=" running ncap.in script into nco_tst.pl";
$expected ="ncap: WARNING Replacing missing value data in variable val_half_half";
&go();

####################
#### ncatted tests #
####################
$operator="ncatted";
####################
$test[0]='ncatted -O -a units,,m,c,"meter second-1" in.nc foo.nc';
$test[1]='ncks -C -H -s "%s" -v lev foo.nc | grep units | cut -d " " -f 11-12';
$description=" Modify all existing units attributes to meter second-1";
$expected="meter second-1";
&go();

####################
#### ncbo tests ####
####################
$operator="ncbo";
####################
$test[0]='ncbo -O --op_typ="-" -v mss_val_scl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v mss_val_scl foo.nc';
$description=" difference scalar missing value";
$expected= 1.0e36 ; 
&go();

$test[0]='ncbo -O --op_typ="-" -d lon,1 -v mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v mss_val foo.nc';
$description=" difference with missing value attribute";
$expected= 1.0e36 ; 
&go();

$test[0]='ncbo -O --op_typ="-" -d lon,0 -v no_mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v no_mss_val foo.nc';
$description=" difference without missing value attribute";
$expected= 0 ; 
&go();

$test[0]='ncks -O -v mss_val_fst in.nc foo.nc';
$test[1]='ncrename -O -v mss_val_fst,mss_val foo.nc';
$test[2]='ncbo -O -y '-' -v mss_val foo.nc in.nc foo.nc';
$test[3]='ncks -C -H -s "%f," -v mss_val foo.nc';
$description=" missing_values differ between files";
$expected= "-999,-999,-999,-999" ; 
&go();

$test[0]='ncdiff -O -d lon,1 -v mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v mss_val foo.nc';
$description=" ncdiff symbolically linked to ncbo";
$expected= 1.0e36 ; 
&go();

$test[0]='ncdiff -O -d lon,1 -v mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v mss_val foo.nc';
$description=" difference with missing value attribute";
$expected= 1.0e36 ; 
&go();

$test[0]='ncdiff -O -d lon,0 -v no_mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v no_mss_val foo.nc';
$description=" difference without missing value attribute";
$expected= 0 ; 
&go();

####################
#### ncea tests ####
####################
$operator="ncea";
####################
$test[0]='ncra -Y ncea -O -v one_dmn_rec_var -d time,4 in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -v one_dmn_rec_var foo.nc';
$description=" ensemble mean of int across two files";
$expected= 5 ; 
&go();

$test[0]='ncra -Y ncea -O -v rec_var_flt_mss_val_flt -d time,0 in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v rec_var_flt_mss_val_flt foo.nc';
$description=" ensemble mean with missing values across two files";
$expected= 1.0e36 ; 
&go();

$test[0]='ncra -Y ncea -O -y min -v rec_var_flt_mss_val_dbl -d time,1 in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" ensemble min of float across two files";
$expected= 2 ; 
&go();

$test[0]='ncra -Y ncea -O -C -v pck in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v pck foo.nc';
$description=" scale factor + add_offset packing/unpacking";
$expected= 3 ; 
&go();

####################
## ncecat tests ####
####################
$operator="ncecat";
####################
$test[0]='ncks -O -v one in.nc foo1.nc';
$test[1]='ncks -O -v one in.nc foo2.nc';
$test[2]='ncecat -O foo1.nc foo2.nc foo.nc';
$test[3]='ncks -C -H -s "%f, " -v one foo.nc';
$description=" concatenate two files containing only scalar variables";
$expected= "1, 1" ; 
&go();

####################
## ncflint tests ###
####################
$operator="ncflint";
####################
$test[0]='ncflint -O -w 3,-2 -v one in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v one foo.nc';
$description=" identity weighting";
$expected= 1.0 ; 
&go();

$test[0]='ncrename -O -v zero,foo in.nc foo1.nc';
$test[1]='ncrename -O -v one,foo in.nc foo.nc';
$test[2]='ncflint -O -i foo,0.5 -v two foo1.nc foo.nc foo.nc';
$test[3]='ncks -C -H -s "%e" -v two foo.nc';
$description=" identity interpolation";
$expected= 2.0 ; 
&go();

$test[0]='ncks -O -C -d lon,1 -v mss_val in.nc foo_x.nc';
$test[1]='ncks -O -C -d lon,0 -v mss_val in.nc foo_y.nc';
$test[2]='ncflint -O -w 0.5,0.5 foo_x.nc foo_y.nc foo_xy.nc';
$test[3]='ncflint -O -w 0.5,0.5 foo_y.nc foo_x.nc foo_yx.nc';
$test[4]='ncdiff -O foo_xy.nc foo_yx.nc foo_xymyx.nc';
$test[5]='ncks -C -H -s "%g" -v mss_val foo_xymyx.nc';
$description=" switch order of occurrence to test for commutivity";
$expected= 1e+36 ; 
&go();

####################
#### ncks tests ####
####################
$operator="ncks";
####################
$test[0]='ncks -O -v lat_T42,lon_T42,gw_T42 in.nc foo_T42.nc';
$test[1]='ncrename -O -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon foo_T42.nc';
$test[2]='ncap -O -s "one[lat,lon]=lat*lon*0.0+1.0" -s "zero[lat,lon]=lat*lon*0.0" foo_T42.nc foo_T42.nc';
$test[3]='ncks -C -H -s "%g" -v one -F -d lon,128 -d lat,64 foo_T42.nc';
$expected="1";
$description="Create T42 variable named one, uniformly 1.0 over globe in foo_T42.nc";
&go();
$test[0]='ncks -C -H -s "%c" -v fl_nm in.nc';
$description=" extract filename string";
$expected= "/home/zender/nco/data/in.cdl" ;
&go();

$test[0]='ncks -O -v lev in.nc foo.nc';
$test[1]='ncks -C -H -s "%f," -v lev foo.nc';
$description=" extract a dimension";
$expected= "100.000000,500.000000,1000.000000" ; 
&go();

$test[0]='ncks -O -v three_dmn_var in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,3,3 foo.nc';
$description=" extract a variable with limits";
$expected= 23;
&go();

$test[0]='ncks -O -v int_var in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -v int_var foo.nc';
$description=" extract variable of type NC_INT";
$expected= "10" ;
&go();

$test[0]='ncks -O -C -v three_dmn_var -d lat,1,1 -d lev,0,0 -d lev,2,2 -d lon,0,,2 -d lon,1,,2 in.nc foo.nc';
$test[1]='ncks -C -H -s "%4.1f," -v three_dmn_var foo.nc';
$description=" Multi-slab lat and lon with srd";
$expected= "12.0,13.0,14.0,15.0,20.0,21.0,22.0,23.0";
&go();

$test[0]='ncks -O -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0,3 -d lon,1,3 in.nc foo.nc';
$test[1]='ncks -C -H -s "%4.1f," -v three_dmn_var foo.nc';
$description=" Multi-slab with redundant hyperslabs";
$expected= "20.0,21.0,22.0,23.0";
&go();

$test[0]='ncks -O -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0.,,2 -d lon,90.,,2 in.nc foo.nc';
$test[1]='ncks -C -H -s "%4.1f," -v three_dmn_var foo.nc';
$description=" Multi-slab with coordinates";
$expected= "20.0,21.0,22.0,23.0";
&go();

$test[0]='ncks -O -C -v three_dmn_var -d lat,1,1 -d lev,800.,200. -d lon,270.,0. in.nc foo.nc';
$test[1]='ncks -C -H -s "%4.1f," -v three_dmn_var foo.nc';
$description=" Double-wrapped hyperslab";
$expected= "23.0,20.0,15.0,12.0";
&go();

$test[0]='ncks -O -C -v three_double_dmn -d lon,2,2 -d time,8,8  in.nc foo.nc';
$test[1]='ncks -C -H -s "%f," -v three_double_dmn foo.nc';
$description=" Hyperslab of a variable that has two identical dims";
$expected= 59.5;
&go();

$test[0]='ncks -O -C -d time_udunits,"1999-12-08 12:00:0.0","1999-12-09 00:00:0.0" in.nc foo.nc';
$test[1]='ncks -C -H -s "%6.0f" -d time_udunits,"1999-12-08 18:00:0.0","1999-12-09 12:00:0.0",2 -v time_udunits in.nc';
$description=" dimension slice using UDUnits library (fails without UDUnits library support)";
$expected= 876018;
&go();

$test[0]='ncks -O -C -d wvl,"0.1 micron","1 micron" in.nc foo.nc';
$test[1]='ncks -C -H -d wvl,"0.6 micron","1 micron" -s "%3.1e" -v wvl in.nc';
$description=" dimension slice using UDUnit conversion (fails without UDUnits library support)";
$expected= 1.0e-06;
&go();

$test[0]='ncks -O -C -v "^three_*" in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -C -v three foo.nc';
$description=" variable wildcards (fails without regex library)";
$expected= 3 ;
&go();

$test[0]='ncks -O -C -v "^[a-z]{3}_[a-z]{3}_[a-z]{3,}$" in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -C -v val_one_int foo.nc';
$description=" variable wildcards (fails without regex library)";
$expected= 1;
&go();

$test[0]='ncks -O -C -d time,0,1 -v time in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -C -d time,2, foo.nc';
$description=" Offset past end of file";
$expected='ncks: ERROR User-specified dimension index range 2 <= time <=  does not fall within valid dimension index range 0 <= time <= 1';
&go();

$test[0]='ncks -C -H -s "%d" -v byte_var in.nc';
$description=" Print byte value";
$expected= 122 ;
&go();

####################
#### ncpdq tests ####
####################
$operator="ncpdq";
####################
$test[0]='ncpdq -O -a -lat -v lat in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v lat -d lat,0 foo.nc';
$description=" reverse coordinate";
$expected= 90 ; 
&go();

$test[0]='ncpdq -O -a -lat,-lev,-lon -v three_dmn_var in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v three_dmn_var -d lat,0 -d lev,0 -d lon,0 foo.nc';
$description=" reverse three dimensional variable";
$expected= 23 ; 
&go();

$test[0]='ncpdq -O -a lon,lat -v three_dmn_var in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v three_dmn_var -d lat,0 -d lev,2 -d lon,3 foo.nc';
$description=" re-order three dimensional variable";
$expected= 11 ; 
&go();

$test[0]='ncpdq -O -P all_new -v upk in.nc foo.nc';
$test[1]='ncpdq -O -P upk -v upk foo.nc foo.nc';
$test[2]='ncks -C -H -s "%g" -v upk foo.nc';
$description=" Pack and then unpack scalar (uses only add_offset)";
$expected= 3 ; 
&go();

####################
#### ncra tests ####
####################
$operator="ncra";
####################
$test[0]='ncra -O -v one_dmn_rec_var in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -v one_dmn_rec_var foo.nc';
$description=" record mean of int across two files";
$expected= 5 ; 
&go();

$test[0]='ncra -O -v rec_var_flt_mss_val_dbl in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record mean of float with double missing values";
$expected= 5 ; 
&go();

$test[0]='ncra -O -v rec_var_flt_mss_val_int in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_int foo.nc';
$description=" record mean of float with integer missing values";
$expected= 5 ; 
&go();

$test[0]='ncra -O -v rec_var_int_mss_val_int in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -v rec_var_int_mss_val_int foo.nc';
$description=" record mean of integer with integer missing values";
$expected= 5 ; 
&go();

$test[0]='ncra -O -v rec_var_int_mss_val_flt in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -v rec_var_int_mss_val_flt foo.nc';
$description=" record mean of integer with float missing values";
$expected= 5 ; 
&go();

$test[0]='ncra -O -v rec_var_dbl_mss_val_dbl_pck in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_dbl_mss_val_dbl_pck foo.nc';
$description=" record mean of packed double with double missing values";
$expected= 5 ;
&go();

$test[0]='ncra -O -v rec_var_dbl_pck in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_dbl_pck foo.nc';
$description=" record mean of packed double to test precision";
$expected= 100.55 ;
&go();

$test[0]='ncra -O -v rec_var_flt_pck in.nc foo.nc';
$test[1]='ncks -C -H -s "%3.2f" -v rec_var_flt_pck foo.nc';
$description=" record mean of packed float to test precision";
$expected= 100.55 ;
&go();

$test[0]='ncra -O -y avg -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record mean of float with double missing values across two files";
$expected= 5 ; 
&go();

$test[0]='ncra -O -y min -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record min of float with double missing values across two files";
$expected= 2 ; 
&go();

$test[0]='ncra -O -y max -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record max of float with double missing values across two files";
$expected= 8 ; 
&go();

$test[0]='ncra -O -y ttl -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record ttl of float with double missing values across two files";
$expected= 70 ;
&go();

$test[0]='ncra -O -y rms -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%1.5f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record rms of float with double missing values across two files";
$expected= 5.38516 ;
&go();

$test[0]='ncra -Y ncrcat -O -v rec_var_flt_mss_val_dbl in.nc in.nc foo1.nc 2>foo.tst';
$test[1]='ncra -O -y avg -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[2]='ncwa -O -a time foo.nc foo.nc';
$test[3]='ncdiff -O -v rec_var_flt_mss_val_dbl foo1.nc foo.nc foo.nc';
$test[4]='ncra -O -y rms -v rec_var_flt_mss_val_dbl foo.nc foo.nc';
$test[5]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record sdn of float with double missing values across two files";
$expected= 2 ;
&go();

####################
#### ncwa tests ####
####################
$operator="ncwa";
####################
$test[0]='ncwa -O -a lat,lon -w gw -d lat,0.0,90.0 foo_T42.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v one foo.nc';
$description=" normalize by denominator upper hemisphere";
$expected= 1;
&go();

#${MY_BIN_DIR}/ncwa -n -O -a lat,lon -w gw foo_T42.nc foo.nc';
#$test[1]='ncks -C -H -s "%f" -v one foo.nc';
#$description=" normalize by tally but not weight";
#$expected= 0.0312495 ; 
#&go();
#${MY_BIN_DIR}/ncwa -W -O -a lat,lon -w gw foo_T42.nc foo.nc';
#$test[1]='ncks -C -H -s "%f" -v one foo.nc';
#$description=" normalize by weight but not tally";
#$expected= 8192 ; 
#&go();
$test[0]='ncwa -N -O -a lat,lon -w gw in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v mask foo.nc';
$description=" do not normalize by denominator";
$expected= 50 ; 
&go();

$test[0]='ncwa -O -a lon -v mss_val in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v mss_val foo.nc';
$description=" average with missing value attribute";
$expected= 73 ; 
&go();

$test[0]='ncwa -O -a lon -v no_mss_val in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v no_mss_val foo.nc';
$description=" average without missing value attribute";
$expected= 5.0e35 ; 
&go();

$test[0]='ncwa -O -v lat -m lat -M 90.0 -T eq -a lat in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v lat foo.nc';
$description=" average masked coordinate";
$expected= 90.0 ; 
&go();

$test[0]='ncwa -O -v lat_var -m lat -M 90.0 -T eq -a lat in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v lat_var foo.nc';
$description=" average masked variable";
$expected= 2.0 ; 
&go();

$test[0]='ncwa -O -v lev -m lev -M 100.0 -T eq -a lev -w lev_wgt in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v lev foo.nc';
$description=" average masked, weighted coordinate";
$expected= 100.0 ; 
&go();

$test[0]='ncwa -O -v lev_var -m lev -M 100.0 -T gt -a lev -w lev_wgt in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v lev_var foo.nc';
$description=" average masked, weighted variable";
$expected= 666.6667 ; 
&go();

$test[0]='ncwa -O -v lat -a lat -w gw -d lat,0 in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v lat foo.nc';
$description=" weight conforms to var first time";
$expected= -90.0 ; 
&go();

$test[0]='ncwa -O -v mss_val_all -a lon -w lon in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%g" -v mss_val_all foo.nc';
$description=" average all missing values with weights";
$expected= 1.0e36 ; 
&go();

$test[0]='ncwa -O -v val_one_mss -a lat -w wgt_one in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v val_one_mss foo.nc';
$description=" average some missing values with unity weights";
$expected= 1.0 ; 
&go();

$test[0]='ncwa -O -v msk_prt_mss_prt -m msk_prt_mss_prt -M 1.0 -T lt -a lon in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v msk_prt_mss_prt foo.nc';
$description=" average masked variable with some missing values";
$expected= 0.5 ; 
&go();

$test[0]='ncwa -O -y min -v rec_var_flt_mss_val_dbl in.nc foo.nc 2>foo.tst';
$test[1]='ncks -C -H -s "%e" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" min switch on type double, some missing values";
$expected= 2 ; 
&go();

$test[0]='ncwa  -O -y min -v three_dmn_var_dbl -a lon in.nc foo.nc';
$test[1]='ncks -C -H -s "%f," -v three_dmn_var_dbl foo.nc >foo';
$test[2]='cut -d, -f 7 foo';
$description=" Dimension reduction with min switch and missing values";
$expected= -99 ;
&go();
$test[0]='cut -d, -f 20 foo';
$description=" Dimension reduction with min switch ";
$expected= 77 ;
&go();

$test[0]='ncwa -O -y min -v three_dmn_var_int -a lon in.nc foo.nc';
$test[1]='ncks -C -H -s "%d," -v three_dmn_var_int foo.nc >foo';
$test[2]='cut -d, -f 5 foo';
$description=" Dimension reduction on type int with min switch and missing values";
$expected= -99 ;
&go();
$test[0]='cut -d, -f 7 foo';
$description=" Dimension reduction on type int variable";
$expected= 25 ;
&go();

$test[0]='ncwa -O -y min -v three_dmn_var_sht -a lon in.nc foo.nc';
$test[1]='ncks -C -H -s "%d," -v three_dmn_var_sht foo.nc >foo';
$test[2]='cut -d, -f 20 foo';
$description=" Dimension reduction on type short variable with min switch and missing values";
$expected= -99 ;
&go();
$test[0]='cut -d, -f 8 foo';
$description=" Dimension reduction on type short variable";
$expected= 29 ;
&go();

$test[0]='ncwa -O -y min -v three_dmn_rec_var in.nc foo.nc 2>foo.tst';
$test[1]='ncks -C -H -s "%f" -v three_dmn_rec_var foo.nc';
$description=" Dimension reduction with min flag on type float variable";
$expected= 1 ;
&go();

$test[0]='ncwa -O -y max -v four_dmn_rec_var in.nc foo.nc 2> foo.tst';
$test[1]='ncks -C -H -s "%f" -v four_dmn_rec_var foo.nc';
$description=" Max flag on type float variable";
$expected= 240 ;
&go();

$test[0]='ncwa -O -y max -v three_dmn_var_dbl -a lat,lon in.nc foo.nc';
$test[1]='ncks -C -H -s "%f," -v three_dmn_var_dbl foo.nc >foo';
$test[2]='cut -d, -f 4 foo';
$description=" Dimension reduction on type double variable with max switch and missing values";
$expected= -99 ;
&go();
$test[0]='cut -d, -f 5 foo';
$description=" Dimension reduction on type double variable";
$expected= 40 ;
&go();

$test[0]='ncwa -O -y max -v three_dmn_var_int -a lat in.nc foo.nc';
$test[1]='ncks -C -H -s "%d," -v three_dmn_var_int foo.nc >foo';
$test[2]='cut -d, -f 9 foo';
$description=" Dimension reduction on type int variable with min switch and missing values";
$expected= -99 ;
&go();
$test[0]='cut -d, -f 13 foo';
$description=" Dimension reduction on type int variable";
$expected= 29 ;
&go();

$test[0]='ncwa -O -y max -v three_dmn_var_sht -a lat in.nc foo.nc';
$test[1]='ncks -C -H -s "%d," -v three_dmn_var_sht foo.nc >foo';
$test[2]='cut -d, -f 37 foo';
$description=" Dimension reduction on type short variable with max switch and missing values";
$expected= -99 ;
&go();
$test[0]='cut -d, -f 33 foo';
$description=" Dimension reduction on type short, max switch variable";
$expected= 69 ;
&go();

$test[0]='ncwa -O -y rms -w lat_wgt -v lat in.nc foo.nc 2>foo.tst';
$test[1]='ncks -C -H -s "%f" -v lat foo.nc';
$description=" rms with weights";
$expected= 90 ; 
&go();

$test[0]='ncwa -O -w val_half_half -v val_one_one_int in.nc foo.nc 2> foo.tst';
$test[1]='ncks -C -H -s "%ld" -v val_one_one_int foo.nc';
$description=" weights would cause SIGFPE without dbl_prc patch";
$expected= 1 ; 
&go();

$test[0]='ncwa -O -y avg -v val_max_max_sht in.nc foo.nc 2> foo.tst';
$test[1]='ncks -C -H -s "%d" -v val_max_max_sht foo.nc';
$description=" avg would overflow without dbl_prc patch";
$expected= 17000 ; 
&go();

$test[0]='ncwa -O -y ttl -v val_max_max_sht in.nc foo.nc 2> foo.tst';
$test[1]='ncks -C -H -s "%d" -v val_max_max_sht foo.nc';
$description=" ttl would overflow without dbl_prc patch, wraps anyway so exact value not important (failure on AIX, LINUX, SUNMP expected/OK because of different wrap behavior)";
$expected= -32768 ; 
&go();
$test[0]='ncwa -O -y min -a lat -v lat -w gw in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v lat foo.nc';
$description=" min with weights";
$expected= -900 ; 
&go();

$test[0]='ncwa -O -y max -a lat -v lat -w gw in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v lat foo.nc';
$description=" max with weights";
$expected= 900 ; 
&go();

####################
##### net tests ####
####################
$operator="net";
####################
$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[1]='ncks -O -v one -p ftp://dust.ess.uci.edu/pub/zender/nco -l ./ in.nc foo.nc';
$test[2]='ncks -C -H -s "%e" -v one foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 1: FTP protocol (fails if unable to anonymous FTP to dust.ess.uci.edu)";
$expected= 1;
&go();
$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[1]='ncks -O -v one -p goldhill.cgd.ucar.edu:/home/zender/nco/data -l ./ in.nc foo.nc';
$test[2]='ncks -C -H -s "%e" -v one foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 2: scp/rcp protocol(fails if no SSH/RSH access to goldhill.cgd.ucar.edu)";
$expected= 1;
&go();

$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[0]='ncks -O -v one -p mss:/ZENDER/nc -l ./ in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v one foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 3: msrcp protocol(fails if not at NCAR)";
$expected= 1; 
&go();
$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[0]='ncks -O -v one -p http://dust.ess.uci.edu/pub/zender/nco -l ./ in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v one foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 4: HTTP protocol (Will always fail until HTTP implemented in NCO) ";
$expected= 1; 
&go();

$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[0]='ncks -C -d lon,0 -v lon -l ./ -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v lon foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 5: HTTP/DODS protocol (fails if not compiled on Linux with make DODS=Y)";
$expected= 0;
&go();
} # end of perform_test()

# usage - infomational blurb for script

sub usage {
	print << 'USAGE';
	
Usage:
nco_test.pl (options) [list of operators to test from the following list]

ncap ncatted ncbo ncflint ncea ncecat
ncks ncpdq ncra ncrcat ncrename ncwa          (default tests all)

where (options) are:
  --usage || -h ...dumps this help
  --debug {1-3) ...puts the script into debug mode; emits more and (hopefully)
                     more useful info.
  --log ...........requests that the debug info is logged to 'nctest.log'
                     as well as spat to STDOUT.
  --udpreport......requests that the test results are communicated back to
                     NCO Central to add your test, timing, and build results.
							NB: This option uses udp port 29659 and may set off
							firewall alarms if used unless that port is open.
                     
nco_test.pl is a semi-automated script for testing the accuracy and
robustness of the nco (netCDF Operators), typically after they are
built, using the 'make test' command.  In this mode, a user should
never have to see this message, so this is all I'll say about it.

In nco debug/testing  mode, it tries to validate the nco's for both
accuracy and robustness.  It also can collect benchmark statistics via
sending test resuilts to a 

NB: When adding tests, be sure to use -O to overwrite files.
Otherwise, script hangs waiting for interactive response to
overwrite queries.

This script is part of the netCDF Operators package:
  http://nco.sourceforge.net

Copyright © 1994-2005 Charlie Zender zender@uci.edu

USAGE
	exit(0);
}  # end of usage()


####################
sub initialize
{
  # list below enumerates the nco's to be tested; does not set up the tests for the
  # operators.
  @all_operators = qw( ncap ncatted ncbo ncflint ncea ncecat ncks ncpdq ncra ncrcat ncrename ncwa);
if (scalar @ARGV > 0) 
{
  @operators=@ARGV;
} else {
  @operators=@all_operators;
}
  if (defined $ENV{MY_BIN_DIR})
  {
    $MY_BIN_DIR=$ENV{MY_BIN_DIR};
  } else {
  # set and verify MY_BIN_DIR
		$MY_BIN_DIR = abs_path("../src/nco");
    print "MY_BIN_DIR not specified, use $MY_BIN_DIR? ('y' or specify) ";
    my $ans = <STDIN>;
    chomp $ans;
    $MY_BIN_DIR = $ans unless (lc($ans) eq "y");
  }
  # Die if this path still does not work
  die "$MY_BIN_DIR/$operators[0] doesn't exist\n" unless (-e "$MY_BIN_DIR/$operators[0]");
  
  # create symbolic links for testing
	# if shared libraries were created, then the real executables are
	# in src/nco/.libs, so point to them instead
	my $dotlib = "";
	$dotlib = ".libs/lt-" if `head -1 $MY_BIN_DIR/ncatted` =~ m/sh/;
        $sym_link{ncdiff}=$dotlib . "ncbo";
	$sym_link{ncea}=$dotlib . "ncra";
	$sym_link{ncrcat}=$dotlib . "ncra";
	foreach(keys %sym_link) {
	  system("cd $MY_BIN_DIR && ln -s -f $sym_link{$_} $_ || (rm -f $_ && ln -s -f $sym_link{$_} $_)");
	}

# Go to data directory where tests are actually run
  my $data_dir = "../data";
  
  chdir $data_dir or die "$!\n";
  
  # Make sure in.nc exists, make it if possible, or die
  unless (-e "in.nc") {
    system("ncgen -o in.nc in.cdl") if (`which ncgen` and -e "in.cdl");
    }
  
  die "The netCDF file \"in.nc\" is necessary for testing NCO, however, it could not be found in \"$data_dir\".  Also, it could not be generated because \"ncgen\" could not be found in your path and/or the file \"$data_dir/in.cdl\" does not exist.\n"
  unless (-e "in.nc");
  
  # Initialize some hashes for each operator that will be tested
  foreach(@operators) 
  {
    $testnum{$_}=0;
    $success{$_}=0;
    $failure{$_}=0;
    }
  print "***NCO Test Suite***\n";
} # end of initialize()

####################
sub verbosity {
	my $ts = shift;
	if($dbg_lvl > 0){printf ("$ts");}
	if ($wantlog) {printf (LOG "$ts");}
} # end of verbosity()


####################
sub go {
  &verbosity("\n\n=====\nResult Stanza for [$operator] ($description)\nsubtest $testnum{$operator} :\n===\n");
  # Only perform tests of requested operator; default is all
  if (!defined $testnum{$operator}) { 
    # Clear test array
    @test=();
    return; 
  }
  print STDERR "Test: [$operator], subtest [$testnum{$operator}]\n";
  $subbenchmarks{$operator} = 0;

  $testnum{$operator}++;
  my $testcnt = 0;
  my $t = 0;
  foreach  (@test) {
    &verbosity("## test cycle $testcnt ## \n");
    my $opcnt = 0;
    # Add MY_BIN_DIR to NCO operator commands only, not things like 'cut'
    foreach my $op (@all_operators) {
      if ($_ =~ m/^$op/) {
        $_ = "$MY_BIN_DIR/$_" ;
      }
    }
    # Perform an individual test
	 &verbosity("Commandline for operator [$operator]:\n$_\n\n");
	 # timing code using Time::HiRes
	 my $t0;
	 if ($hiresfound) {$t0 = [gettimeofday];}
	 else {$t0 = time;}
	
    $result=`$_` ;
	 
    my $elapsed;
	 if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
	 else {$elapsed = time - $t0;}

#	 print "inter benchmark for $operator = $subbenchmarks{$operator} \n";
	 $subbenchmarks{$operator} += $elapsed;
	 $t = $testnum{$operator} - 1;
    print "$operator [$t] took $elapsed seconds\n";
	 &verbosity($result);
	 $testcnt++;
  }
  print STDERR "total time for $operator [$t] =  $subbenchmarks{$operator} s \n\n";
  $totbenchmarks{$operator} += $subbenchmarks{$operator};
  
  # Remove trailing newline for easier regex comparison
  chomp $result;
  
  # Compare numeric results
  if ($expected =~/\d/)
  {
    if ($result == $expected)
    {
      $success{$operator}++;
	 	&verbosity("\nPASSED - Numeric output: [$operator]:\n");
    } else {
      &failed($expected);
		&verbosity("\nFAILED - Numeric output: [$operator]:\n");
    }
  }
  # Compare non-numeric tests
  elsif ($expected =~/\D/)
  {
    # Compare $result with $expected
    if (substr($result,0,length($expected)) eq $expected)
    {
      $success{$operator}++;
		&verbosity("\nPASSED Alphabetic output: [$operator]:\n");
    } else {
      &failed($expected);
		&verbosity("\nFAILED Alphabetic output: [$operator]\n");
    }
 }   
 # No result at all?
 else {
    &failed();
	 &verbosity("\nFAILED - No result from [$operator]\n");
 }
 # Clear test
 @test=();
 print Total $totbenchmarks{$operator}
} # end &go()

####################
sub failed {
  my $expected = shift;

  $failure{$operator}++;
  print "FAILED($operator): $description\n";
  foreach(@test) {
    print "$_\n";
  }
  
  if ($expected) {
    print "$result != $expected\n" ;
  } else {
    print "produced no results\n";
  }
  print "-----------\n";
  return;
}

####################
sub summarize_results 
{ my $CC = `../src/nco/ncks --compiler`;
  my $CCinfo = "";
  if ($CC =~ /gcc/) {$CCinfo = `gcc --version |grep -i gcc`;}
  elsif ($CC =~ /xlc/) {$CCinfo = "xlc version ??";}
  elsif ($CC =~ /icc/) {$CCinfo = "Intel C Compiler version ??";}
  
  my $reportstr = "";
  my $idstring = `uname -a` . "using: " . $CCinfo;
  $reportstr .= "\n\nNCO Test Result Summary for:\n$idstring\n";
  $reportstr .=  "      Test                            Total Time (s) \n";
  
  foreach(@operators) 
  {
    my $total = $success{$_}+$failure{$_};
    #printf "$_:\tsuccess: $success{$_} of $total\n";
	 $reportstr .= sprintf "%10s:  success: %3d of %3d      %6.4f\n", $_, $success{$_}, $total, $totbenchmarks{$_};
  }
  chdir "../bld";
  if ($dbg_lvl == 0) {print $reportstr;}
  else { &verbosity($reportstr); }
  if ($udp_report) {$sock->send($reportstr);}
}
