#!/usr/bin/perl

# Usage: 
# ~/nco/bld/nco_tst.pl

# NB: When adding tests, _be sure to use -O to overwrite files_
# Otherwise, script hangs waiting for interactive response to overwrite queries

use File::Basename;

my $dbg_lvl=0; # [enm] Print tests during execution for debugging

&initialize();
&perform_tests();
&summarize_results();

sub perform_tests
{
# Tests are in alphabetical order by operator name

####################
#### ncap tests ####
####################
$operator="ncap";
####################
$test[0]='ncap -O -v -S ncap.in in.nc foo.nc';
$description=" running ncap.in script into nco_tst.pl";
$expected="ncap: WARNING Replacing missing value data in variable val_half_half";
&go();

####################
#### ncbo tests ####
####################
$operator="ncbo";
####################
$test[0]='ncbo -O --op_typ="-" -d lon,1 -v mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v mss_val foo.nc';
$description=" difference with missing value attribute";
$expected= 1.0e36 ; 
&go();
####################

$test[0]='ncbo -O --op_typ="-" -d lon,0 -v no_mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v no_mss_val foo.nc';
$description=" difference without missing value attribute";
$expected= 0 ; 
&go();
####################

$test[0]='ncks -O -v mss_val_fst in.nc foo.nc';
$test[1]='ncrename -O -v mss_val_fst,mss_val foo.nc';
$test[2]='ncbo -O -y '-' -v mss_val foo.nc in.nc foo.nc';
$test[3]='ncks -C -H -s "%f," -v mss_val foo.nc';
$description=" missing_values differ between files";
$expected= "-999,-999,-999,-999" ; 
&go();
####################

$test[0]='ncdiff -O -d lon,1 -v mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v mss_val foo.nc';
$description=" ncdiff symbolically linked to ncbo";
$expected= 1.0e36 ; 
&go();
####################

$test[0]='ncdiff -O -d lon,1 -v mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v mss_val foo.nc';
$description=" difference with missing value attribute";
$expected= 1.0e36 ; 
&go();
####################

$test[0]='ncdiff -O -d lon,0 -v no_mss_val in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v no_mss_val foo.nc';
$description=" difference without missing value attribute";
$expected= 0 ; 
&go();
####################

####################
#### ncea tests ####
####################
$operator="ncea";
####################
$test[0]='ncea -O -v one_dmn_rec_var -d time,4 in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -v one_dmn_rec_var foo.nc';
$description=" ensemble mean of int across two files";
$expected= 5 ; 
&go();
####################

$test[0]='ncea -O -v rec_var_flt_mss_val_flt -d time,0 in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v rec_var_flt_mss_val_flt foo.nc';
$description=" ensemble mean with missing values across two files";
$expected= 1.0e36 ; 
&go();
####################

$test[0]='ncea -O -y min -v rec_var_flt_mss_val_dbl -d time,1 in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" ensemble min of float across two files";
$expected= 2 ; 
&go();
####################

$test[0]='ncea -O -C -v pck in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v pck foo.nc';
$description=" scale factor + add_offset packing/unpacking";
$expected= 3 ; 
&go();
####################

####################
#### ncecat tests ####
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
####################

$test[0]='ncrename -O -v zero,foo in.nc foo1.nc';
$test[1]='ncrename -O -v one,foo in.nc foo.nc';
$test[2]='ncflint -O -i foo,0.5 -v two foo1.nc foo.nc foo.nc';
$test[3]='ncks -C -H -s "%e" -v two foo.nc';
$description=" identity interpolation";
$expected= 2.0 ; 
&go();
####################

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

####################
#### ncks tests ####
####################
$operator="ncks";
####################
$test[0]='ncks -O -v lat_T42,lon_T42,gw_T42 in.nc foo_T42.nc';
$test[1]='ncrename -O -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon foo_T42.nc';
$test[2]='ncap -O -s "one[lat,lon]=lat*lon*0.0+1.0" -s "zero[lat,lon]=lat*lon*0.0" foo_T42.nc foo_T42.nc';
$test[3]='ncks -H -C -s "%g" -v one -F -d lon,128 -d lat,64 foo_T42.nc';
$expected="1";
$description="Create T42 variable named one, uniformly 1.0 over globe in foo_T42.nc";
&go();
####################
$test[0]='ncks -C -H -s "%c" -v fl_nm in.nc';
$description=" extract filename string";
$expected= "/home/zender/nco/data/in.cdl" ;
&go();
####################

$test[0]='ncks -O -v lev in.nc foo.nc';
$test[1]='ncks -H -C -s "%f," -v lev foo.nc';
$description=" extract a dimension";
$expected= "100.000000,500.000000,1000.000000" ; 
&go();
####################

$test[0]='ncks -O -v three_dmn_var in.nc foo.nc';
$test[1]='ncks -H -C -s "%f" -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,3,3 foo.nc';
$description="ncks 3: extract a variable with limits";
$expected= 23;
&go();
####################

$test[0]='ncks -O -v int_var in.nc foo.nc';
$test[1]='ncks -H -C -s "%d" -v int_var foo.nc';
$description="ncks 4: extract variable of type NC_INT";
$expected= "10" ;
&go();
####################

$test[0]='ncks -O -C -v three_dmn_var -d lat,1,1 -d lev,0,0 -d lev,2,2 -d lon,0,,2 -d lon,1,,2 in.nc foo.nc';
$test[1]='ncks -H -C -s "%4.1f," -v three_dmn_var foo.nc';
$description="ncks 5: Multi-slab lat and lon with srd";
$expected= "12.0,13.0,14.0,15.0,20.0,21.0,22.0,23.0";
&go();
####################

$test[0]='ncks -O -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0,3 -d lon,1,3 in.nc foo.nc';
$test[1]='ncks -H -C -s "%4.1f," -v three_dmn_var foo.nc';
$description="ncks 6: Multi-slab with redundant hyperslabs";
$expected= "20.0,21.0,22.0,23.0";
&go();
####################

$test[0]='ncks -O -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0.,,2 -d lon,90.,,2 in.nc foo.nc';
$test[1]='ncks -H -C -s "%4.1f," -v three_dmn_var foo.nc';
$description="ncks 7: Multi-slab with coordinates";
$expected= "20.0,21.0,22.0,23.0";
&go();
####################

$test[0]='ncks -O -C -v three_dmn_var -d lat,1,1 -d lev,800.,200. -d lon,270.,0. in.nc foo.nc';
$test[1]='ncks -H -C -s "%4.1f," -v three_dmn_var foo.nc';
$description="ncks 8: Double-wrapped hyperslab";
$expected= "23.0,20.0,15.0,12.0";
&go();
####################

$test[0]='ncks -O -C -v three_double_dmn -d lon,2,2 -d time,8,8  in.nc foo.nc';
$test[1]='ncks -H -C -s "%f," -v three_double_dmn foo.nc';
$description="ncks 9: Hyperslab of a variable that has two identical dims";
$expected= 59.5;
&go();
####################

$test[0]='ncks -O -C -d time_udunits,"1999-12-08 12:00:0.0","1999-12-09 00:00:0.0" in.nc foo.nc';
$test[1]='ncks -H -C -s "%6.0f" -d time_udunits,"1999-12-08 18:00:0.0","1999-12-09 12:00:0.0",2 -v time_udunits in.nc';
$description="ncks 10: dimension slice using UDUnits library (fails without UDUnits library support)";
$expected= 876018;
&go();
####################

$test[0]='ncks -O -C -d wvl,"0.1 micron","1 micron" in.nc foo.nc';
$test[1]='ncks -H -C -d wvl,"0.6 micron","1 micron" -s "%3.1e" -v wvl in.nc';
$description="ncks 11: dimension slice using UDUnit conversion (fails without UDUnits library support)";
$expected= 1.0e-06;
&go();
####################

$test[0]='ncks -O -C -v "^three_*" in.nc foo.nc';
$test[1]='ncks -H -s "%f" -C -v three foo.nc';
$description="ncks 12: variable wildcards (fails without regex library)";
$expected= 3 ;
&go();
####################

$test[0]='ncks -O -C -v "^[a-z]{3}_[a-z]{3}_[a-z]{3,}$" in.nc foo.nc';
$test[1]='ncks -H -s "%d" -C -v val_one_int foo.nc';
$description="ncks 13: variable wildcards (fails without regex library)";
$expected= 1;
&go();
####################

$test[0]='ncks -O -C -d time,0,1 -v time in.nc foo.nc';
$test[1]='ncks -H -s "%g" -C -d time,2, foo.nc';
$description="ncks 14: Offset past end of file";
$expected='ncks: ERROR User-specified dimension index range 2 <= time <=  does not fall within valid dimension index range 0 <= time <= 1';
&go();
####################

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
####################

$test[0]='ncra -O -v rec_var_flt_mss_val_dbl in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record mean of float with double missing values";
$expected= 5 ; 
&go();
####################

$test[0]='ncra -O -v rec_var_flt_mss_val_int in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_int foo.nc';
$description=" record mean of float with integer missing values";
$expected= 5 ; 
&go();
####################

$test[0]='ncra -O -v rec_var_int_mss_val_int in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -v rec_var_int_mss_val_int foo.nc';
$description=" record mean of integer with integer missing values";
$expected= 5 ; 
&go();
####################

$test[0]='ncra -O -v rec_var_int_mss_val_flt in.nc foo.nc';
$test[1]='ncks -C -H -s "%d" -v rec_var_int_mss_val_flt foo.nc';
$description=" record mean of integer with float missing values";
$expected= 5 ; 
&go();
####################

$test[0]='ncra -O -v rec_var_dbl_mss_val_dbl_pck in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_dbl_mss_val_dbl_pck foo.nc';
$description=" record mean of packed double with double missing values";
$expected= 5 ;
&go();
####################

$test[0]='ncra -O -v rec_var_dbl_pck in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_dbl_pck foo.nc';
$description=" record mean of packed double to test precision";
$expected= 100.55 ;
&go();
####################

$test[0]='ncra -O -v rec_var_flt_pck in.nc foo.nc';
$test[1]='ncks -C -H -s "%3.2f" -v rec_var_flt_pck foo.nc';
$description=" record mean of packed float to test precision";
$expected= 100.55 ;
&go();
####################

$test[0]='ncra -O -y avg -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record mean of float with double missing values across two files";
$expected= 5 ; 
&go();
####################

$test[0]='ncra -O -y min -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record min of float with double missing values across two files";
$expected= 2 ; 
&go();
####################

$test[0]='ncra -O -y max -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record max of float with double missing values across two files";
$expected= 8 ; 
&go();
####################

$test[0]='ncra -O -y ttl -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record ttl of float with double missing values across two files";
$expected= 70 ;
&go();
####################

$test[0]='ncra -O -y rms -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[1]='ncks -C -H -s "%1.5f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record rms of float with double missing values across two files";
$expected= 5.38516 ;
&go();
####################

$test[0]='ncrcat -O -v rec_var_flt_mss_val_dbl in.nc in.nc foo1.nc 2>foo.tst';
$test[1]='ncra -O -y avg -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc';
$test[2]='ncwa -O -a time foo.nc foo.nc';
$test[3]='ncdiff -O -v rec_var_flt_mss_val_dbl foo1.nc foo.nc foo.nc';
$test[4]='ncra -O -y rms -v rec_var_flt_mss_val_dbl foo.nc foo.nc';
$test[5]='ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" record sdn of float with double missing values across two files";
$expected= 2 ;
&go();
####################

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
####################

#${MY_BIN_DIR}/ncwa -n -O -a lat,lon -w gw foo_T42.nc foo.nc';
#$test[1]='ncks -C -H -s "%f" -v one foo.nc';
#$description=" normalize by tally but not weight";
#$expected= 0.0312495 ; 
#&go();
####################
#${MY_BIN_DIR}/ncwa -W -O -a lat,lon -w gw foo_T42.nc foo.nc';
#$test[1]='ncks -C -H -s "%f" -v one foo.nc';
#$description=" normalize by weight but not tally";
#$expected= 8192 ; 
#&go();
####################
$test[0]='ncwa -N -O -a lat,lon -w gw in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v mask foo.nc';
$description=" do not normalize by denominator";
$expected= 50 ; 
&go();
####################

$test[0]='ncwa -O -a lon -v mss_val in.nc foo.nc';
$test[1]='ncks -C -H -s "%f" -v mss_val foo.nc';
$description=" average with missing value attribute";
$expected= 73 ; 
&go();
####################

$test[0]='ncwa -O -a lon -v no_mss_val in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v no_mss_val foo.nc';
$description=" average without missing value attribute";
$expected= 5.0e35 ; 
&go();
####################

$test[0]='ncwa -O -v lat -m lat -M 90.0 -T eq -a lat in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v lat foo.nc';
$description=" average masked coordinate";
$expected= 90.0 ; 
&go();
####################

$test[0]='ncwa -O -v lat_var -m lat -M 90.0 -T eq -a lat in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v lat_var foo.nc';
$description=" average masked variable";
$expected= 2.0 ; 
&go();
####################

$test[0]='ncwa -O -v lev -m lev -M 100.0 -T eq -a lev -w lev_wgt in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v lev foo.nc';
$description=" average masked, weighted coordinate";
$expected= 100.0 ; 
&go();
####################

$test[0]='ncwa -O -v lev_var -m lev -M 100.0 -T gt -a lev -w lev_wgt in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v lev_var foo.nc';
$description=" average masked, weighted variable";
$expected= 666.6667 ; 
&go();
####################

$test[0]='ncwa -O -v lat -a lat -w gw -d lat,0 in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v lat foo.nc';
$description=" weight conforms to var first time";
$expected= -90.0 ; 
&go();
####################

$test[0]='ncwa -O -v mss_val_all -a lon -w lon in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v mss_val_all foo.nc';
$description=" average all missing values with weights";
$expected= 1.0e36 ; 
&go();
####################

$test[0]='ncwa -O -v val_one_mss -a lat -w wgt_one in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v val_one_mss foo.nc';
$description=" average some missing values with unity weights";
$expected= 1.0 ; 
&go();
####################

$test[0]='ncwa -O -v msk_prt_mss_prt -m msk_prt_mss_prt -M 1.0 -T lt -a lon in.nc foo.nc'; 
$test[1]='ncks -C -H -s "%e" -v msk_prt_mss_prt foo.nc';
$description=" average masked variable with some missing values";
$expected= 0.5 ; 
&go();
####################

$test[0]='ncwa -O -y min -v rec_var_flt_mss_val_dbl in.nc foo.nc 2>foo.tst';
$test[1]='ncks -C -H -s "%e" -v rec_var_flt_mss_val_dbl foo.nc';
$description=" min switch on type double, some missing values";
$expected= 2 ; 
&go();
####################

$test[0]='ncwa  -O -y min -v three_dmn_var_dbl -a lon in.nc foo.nc';
$test[1]='ncks -C -H -s "%f," -v three_dmn_var_dbl foo.nc >foo';
$test[2]='cut -d, -f 7 foo';
$description=" Dimension reduction with min switch and missing values";
$expected= -99 ;
&go();
####################
$test[0]='cut -d, -f 20 foo';
$description=" Dimension reduction with min switch ";
$expected= 77 ;
&go();
####################

$test[0]='ncwa -O -y min -v three_dmn_var_int -a lon in.nc foo.nc';
$test[1]='ncks -C -H -s "%d," -v three_dmn_var_int foo.nc >foo';
$test[2]='cut -d, -f 5 foo';
$description=" Dimension reduction on type int with min switch and missing values";
$expected= -99 ;
&go();
####################
$test[0]='cut -d, -f 7 foo';
$description=" Dimension reduction on type int variable";
$expected= 25 ;
&go();
####################

$test[0]='ncwa -O -y min -v three_dmn_var_sht -a lon in.nc foo.nc';
$test[1]='ncks -C -H -s "%d," -v three_dmn_var_sht foo.nc >foo';
$test[2]='cut -d, -f 20 foo';
$description=" Dimension reduction on type short variable with min switch and missing values";
$expected= -99 ;
&go();
####################
$test[0]='cut -d, -f 8 foo';
$description=" Dimension reduction on type short variable";
$expected= 29 ;
&go();
####################

$test[0]='ncwa -O -y min -v three_dmn_rec_var in.nc foo.nc 2>foo.tst';
$test[1]='ncks -C -H -s "%f" -v three_dmn_rec_var foo.nc';
$description=" Dimension reduction with min flag on type float variable";
$expected= 1 ;
&go();
####################

$test[0]='ncwa -O -y max -v four_dmn_rec_var in.nc foo.nc 2> foo.tst';
$test[1]='ncks -C -H -s "%f" -v four_dmn_rec_var foo.nc';
$description=" Max flag on type float variable";
$expected= 240 ;
&go();
####################

$test[0]='ncwa -O -y max -v three_dmn_var_dbl -a lat,lon in.nc foo.nc';
$test[1]='ncks -C -H -s "%f," -v three_dmn_var_dbl foo.nc >foo';
$test[2]='cut -d, -f 4 foo';
$description=" Dimension reduction on type double variable with max switch and missing values";
$expected= -99 ;
&go();
####################
$test[0]='cut -d, -f 5 foo';
$description=" Dimension reduction on type double variable";
$expected= 40 ;
&go();
####################

$test[0]='ncwa -O -y max -v three_dmn_var_int -a lat in.nc foo.nc';
$test[1]='ncks -C -H -s "%d," -v three_dmn_var_int foo.nc >foo';
$test[2]='cut -d, -f 9 foo';
$description=" Dimension reduction on type int variable with min switch and missing values";
$expected= -99 ;
&go();
####################
$test[0]='cut -d, -f 13 foo';
$description=" Dimension reduction on type int variable";
$expected= 29 ;
&go();
####################

$test[0]='ncwa -O -y max -v three_dmn_var_sht -a lat in.nc foo.nc';
$test[1]='ncks -C -H -s "%d," -v three_dmn_var_sht foo.nc >foo';
$test[2]='cut -d, -f 37 foo';
$description=" Dimension reduction on type short variable with max switch and missing values";
$expected= -99 ;
&go();
####################
$test[0]='cut -d, -f 33 foo';
$description=" Dimension reduction on type short, max switch variable";
$expected= 69 ;
&go();
####################

$test[0]='ncwa -O -y rms -w lat_wgt -v lat in.nc foo.nc 2>foo.tst';
$test[1]='ncks -C -H -s "%f" -v lat foo.nc';
$description=" rms with weights";
$expected= 90 ; 
&go();
####################

$test[0]='ncwa -O -w val_half_half -v val_one_one_int in.nc foo.nc 2> foo.tst';
$test[1]='ncks -C -H -s "%ld" -v val_one_one_int foo.nc';
$description=" weights would cause SIGFPE without dbl_prc patch";
$expected= 1 ; 
&go();
####################

$test[0]='ncwa -O -y avg -v val_max_max_sht in.nc foo.nc 2> foo.tst';
$test[1]='ncks -C -H -s "%d" -v val_max_max_sht foo.nc';
$description=" avg would overflow without dbl_prc patch";
$expected= 17000 ; 
&go();
####################

$test[0]='ncwa -O -y ttl -v val_max_max_sht in.nc foo.nc 2> foo.tst';
$test[1]='ncks -C -H -s "%d" -v val_max_max_sht foo.nc';
$description=" ttl would overflow without dbl_prc patch, wraps anyway so exact value not important";
$expected= -32768 ; 
&go();
####################
$test[0]='ncwa -O -y min -a lat -v lat -w gw in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v lat foo.nc';
$description=" min with weights";
$expected= -900 ; 
&go();
####################

$test[0]='ncwa -O -y max -a lat -v lat -w gw in.nc foo.nc';
$test[1]='ncks -C -H -s "%g" -v lat foo.nc';
$description=" max with weights";
$expected= 900 ; 
&go();
####################

####################
##### net tests ####
####################
$operator="net";
####################
$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[1]='ncks -O -v one -p ftp://dust.ps.uci.edu/pub/zender/nco -l ./ in.nc foo.nc';
$test[2]='ncks -C -H -s "%e" -v one foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 1: FTP protocol (fails if unable to anonymous FTP to dust.ess.uci.edu)";
$expected= 1;
&go();
####################
$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[1]='ncks -O -v one -p goldhill.cgd.ucar.edu:/home/zender/nco/data -l ./ in.nc foo.nc';
$test[2]='ncks -C -H -s "%e" -v one foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 2: scp/rcp protocol(fails if no SSH/RSH access to goldhill.cgd.ucar.edu)";
$expected= 1;

&go();
####################

$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[0]='ncks -O -v one -p mss:/ZENDER/nc -l ./ in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v one foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 3: msrcp protocol(fails if not at NCAR)";
$expected= 1; 
&go();
####################
$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[0]='ncks -O -v one -p http://dust.ps.uci.edu/pub/zender/nco -l ./ in.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v one foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 4: HTTP protocol (Will always fail until HTTP implemented in NCO) ";
$expected= 1; 
&go();
####################

$test[0]='/bin/rm -f foo.nc;mv in.nc in_tmp.nc';
$test[0]='ncks -C -d lon,0 -v lon -l ./ -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc foo.nc';
$test[1]='ncks -C -H -s "%e" -v lon foo.nc';
$test[3]='mv in_tmp.nc in.nc';
$description="nco 5: HTTP/DODS protocol (fails if not compiled on Linux with make DODS=Y)";
$expected= 0;
&go();
####################
} # end of perform_test()

####################
sub initialize
{
  @all_operators = qw(ncap ncbo ncflint ncea ncecat ncks ncra ncrename ncwa);
#if (scalar @ARGV > 0) 
if ($#ARGV > 0) 
{
  @operators=@ARGV;
} else {
  @operators=@all_operators;
}
  if (defined $ENV{MY_BIN_DIR})
  {
    $MY_BIN_DIR=$ENV{MY_BIN_DIR};
  } else {
  # find available operators
    if ($op= `which $operators[0]`) {
      $MY_BIN_DIR = dirname($op);
      print "MY_BIN_DIR not specified, use $MY_BIN_DIR? ('y' or specify) ";
      my $ans = <STDIN>;
      chomp $ans;
      $MY_BIN_DIR = $ans unless (lc($ans) eq "y");
    } else {
      print "MY_BIN_DIR not specified, please specify. ";
      $MY_BIN_DIR=<STDIN>;
      chomp $MY_BIN_DIR;
    }
  }
  # Die if this path still does not work
  die "$MY_BIN_DIR/$operators[0] doesn't exist\n" unless (-e "$MY_BIN_DIR/$operators[0]");
  
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
}

####################
sub go {
  # Only perform tests of requested operator; default is all
  if (!defined $testnum{$operator}) { 
    # Clear test array
    @test=();
    return; 
    }
  $testnum{$operator}++;
  foreach  (@test) {
    # Add MY_BIN_DIR to NCO operator commands only, not things like 'cut'
    foreach $op (@all_operators) {
      if ($_ =~ m/^$op/) {
        $_ = "$MY_BIN_DIR/$_" ;
      }
    }
    # Perform an individual test
    if($dbg_lvl > 0){printf ("$0: Executing following test operator $operator:\n$_\n");}
    $result=`$_` ;
  }
  
  # Remove trailing newline for easier regex comparison
  chomp $result;
  
  # Compare numeric results
  if ($expected =~/\d/)
  {
    if ($result == $expected)
    {
      $success{$operator}++;
    } else { 
      &failed($expected);
    } 
  }
  # Compare non-numeric tests
  elsif ($expected =~/\D/)
  {
    # Compare $result with $expected
    if (substr($result,0,length($expected)) eq $expected)
    {
      $success{$operator}++;
    } else {
      &failed($expected);
    }
 }   
 # No result at all?
 else {
    &failed();
 }
 # Clear test
 @test=();
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
{
  print "NCO Test Results:\n";
  
  foreach(@operators) 
  {
    my $total = $success{$_}+$failure{$_};
    print "$_:\tsuccess: $success{$_} of $total\n";
  }
  chdir "../bld";
}
