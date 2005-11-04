package NCO_rgr;
# This file is part of the NCO nco_bm.pl benchmarking and regression testing.
# This file contains all the REGRESSION tests for the NCO operators.
# The BENCHMARKS are coded in the file "nco_bm_benchmarks.pl" which is inline
# code.  This is a module, so it has different packaging semantics, but
# it must maintain Perl semantics

# $Header: /data/zender/nco_20150216/nco/bm/NCO_rgr.pm,v 1.17 2005-11-04 01:49:31 zender Exp $

require 5.6.1 or die "This script requires Perl version >= 5.6.1, stopped";
use English; # WCS96 p. 403 makes incomprehensible Perl errors sort of comprehensible
use Cwd 'abs_path';
use NCO_bm;
use strict;
require Exporter;
our @ISA = qw(Exporter);
#export functions (top) and variables (bottom)
our @EXPORT = qw (
	perform_tests

	$dust_usr $prefix $opr_nm $opr_sng_mpi $dodap $pth_rmt_scp_tst
	$nsr_xpc @tst_cmd %tst_nbr $dbg_lvl $localhostname $wnt_log $dsc_sng
	$outfile $orig_outfile  $foo_fl $foo_avg_fl $foo_tst $foo1_fl $foo2_fl $foo_x_fl
	$foo_y_fl $foo_xy_fl $foo_yx_fl $foo_xymyx_fl $foo_T42_fl $fl_fmt

);

use vars qw(
    $dodap $dsc_sng $dust_usr $fl_fmt $fl_pth $foo1_fl $foo2_fl $foo_avg_fl
    $foo_fl $foo_T42_fl $foo_tst $foo_x_fl $foo_xy_fl
    $foo_xymyx_fl $foo_y_fl $foo_yx_fl $mpi_prc $nco_D_flg $localhostname
    $nsr_xpc $omp_flg $opr_nm $opr_rgr_mpi $orig_outfile
    $outfile $pth_rmt_scp_tst @tst_cmd $USER
);

sub perform_tests
{
# Tests are in alphabetical order by operator name

# The following tests are organized and laid out as follows:
# - $tst_cmd[] holds command lines for each operator being tested
# - $dsc_sng holds test description line
# - $nsr_xpc is string or expression that is correct result of command
# - go() is function which executes each test

my $in_pth = "../data";
my $in_pth_arg = "-p $in_pth";

# csz++
# fxm: pass as arguments or use exporter/importer instead?
*omp_flg=*main::omp_flg;
*nco_D_flg=*main::nco_D_flg;
#*dodap=*main::dodap;
#*$fl_fmt=*main::fl_fmt;


dbg_msg(1,"in package NCO_rgr, \$dodap = $dodap");
dbg_msg(1,"in package NCO_rgr, \$omp_flg = $omp_flg");
# csz--


dbg_msg(1,"File format set to [$fl_fmt]");


if ($dodap ne "FALSE") {
	if ($dodap ne "" && $fl_pth =~ /http/ ) { $in_pth_arg = "-p $fl_pth"; }
	if ($dodap eq "") { $in_pth_arg = "-p http://sand.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata"; }
}
dbg_msg(1,"-------------  REGRESSION TESTS STARTED from perform_tests()  -------------");

#if (0) {
####################
#### ncap tests ####
####################
    $opr_nm='ncap';
####################
	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -v -S ncap.in $in_pth_arg in.nc $outfile";
	$dsc_sng="running ncap.in script into nco_tst.pl";
	$nsr_xpc ="ncap: WARNING Replacing missing value data in variable val_half_half";
	go();

	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -C -v -s 'tpt_mod=tpt%273.0f' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.1f' $outfile";
	$dsc_sng="Testing float modulo float";
	$nsr_xpc ="0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0";
	go();

	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=log(e_flt)^1' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.6f\n' $outfile";
	$dsc_sng="Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)";
	$nsr_xpc ="1.000000";
 go();

	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=log(e_dbl)^1' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.12f\n' $outfile";
	$dsc_sng="Testing foo=log(e_dbl)^1";
	$nsr_xpc ="1.000000000000";
 go();

	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=4*atan(1)' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.12f\n' $outfile";
	$dsc_sng="Testing foo=4*atan(1)";
	$nsr_xpc ="3.141592741013";
 go();

	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=erf(1)' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.12f\n' $outfile";
	$dsc_sng="Testing foo=erf(1)";
	$nsr_xpc ="0.842701";
 go();

	#fails - wrong result
	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=gamma(0.5)' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.12f\n' $outfile";
	$dsc_sng="Testing foo=gamma(0.5)";
	$nsr_xpc ="1.772453851";
 go();

	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -C -v -s 'pi=4*atan(1);foo=sin(pi/2)' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -v foo -s '%.12f\n' $outfile";
	$dsc_sng="Testing foo=sin(pi/2)";
	$nsr_xpc ="1.000000000000";
 go();

	$tst_cmd[0]="ncap -h -O $fl_fmt $nco_D_flg -C -v -s 'pi=4*atan(1);foo=cos(pi)' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -v foo -s '%.12f\n' $outfile";
	$dsc_sng="Testing foo=cos(pi)";
	$nsr_xpc ="-1.000000000000";
 go();

####################
#### ncatted tests #
####################
    $opr_nm="ncatted";
####################
	$tst_cmd[0]="ncatted -h -O $fl_fmt $nco_D_flg -a units,,m,c,'meter second-1' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%s' -v lev $outfile | grep units | cut -d' ' -f 11-12";
	$dsc_sng="Modify all existing units attributes to meter second-1";
	$nsr_xpc="meter second-1";
 go();

        $tst_cmd[0]="ncatted -h -O $fl_fmt $nco_D_flg -a missing_value,val_one_mss,m,f,0.0 $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -d lat,1 -v val_one_mss $outfile";
	$dsc_sng="Change missing_value attribute from 1.0e36 to 0.0";
	$nsr_xpc= 0 ;
 go();

	$tst_cmd[0]="ncatted -O --hdr_pad=1000 $nco_D_flg -a missing_value,val_one_mss,m,f,0.0 $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -M $outfile | grep hdr_pad | wc >$foo_fl";
	$tst_cmd[2]="cut -d' ' -f 13-14 -s $foo_fl";
	$dsc_sng="Pad header with 1000 extra bytes for future metadata";
	$nsr_xpc= 26 ;
 go();

####################
#### ncbo tests ####
####################
    $opr_nm="ncbo";
####################
#if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_rgr_mpi =~ /$opr_nm/)) {
	$tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -v mss_val_scl $in_pth_arg in.nc in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val_scl $outfile";
	$dsc_sng="difference scalar missing value";
	$nsr_xpc= 1.0e36 ;
 go();

	$tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -d lon,1 -v mss_val $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val $outfile";
	$dsc_sng="difference with missing value attribute";
	$nsr_xpc= 1.0e36 ;
 go();

	$tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -d lon,0 -v no_mss_val $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v no_mss_val $outfile";
	$dsc_sng="difference without missing value attribute";
	$nsr_xpc= 0 ;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v mss_val_fst $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncrename -h -O $fl_fmt $nco_D_flg -v mss_val_fst,mss_val $outfile";
	$tst_cmd[2]="ncbo $omp_flg  -h -O $fl_fmt $nco_D_flg -y '-' -v mss_val $outfile ../data/in.nc $outfile 2> $foo_tst";
	$tst_cmd[3]="ncks -C -H -s '%f,' -v mss_val $outfile";
	$dsc_sng="missing_values differ between files";
	$nsr_xpc= "-999,-999,-999,-999" ;
 go();

	$tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,1 -v mss_val $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val $outfile";
	$dsc_sng="ncdiff symbolically linked to ncbo";
	$nsr_xpc= 1.0e36 ;
 go();

	$tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,1 -v mss_val $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val $outfile";
	$dsc_sng="difference with missing value attribute";
	$nsr_xpc= 1.0e36 ;
 go();

	$tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,0 -v no_mss_val $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v no_mss_val $outfile";
	$dsc_sng="difference without missing value attribute";
	$nsr_xpc= 0 ;
 go();


	$tst_cmd[0]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc $foo_avg_fl";
	$tst_cmd[1]="ncbo $omp_flg -C -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth/in.nc $foo_avg_fl $outfile";
	$tst_cmd[2]="ncks -C -H -d time,3 -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="Difference which tests broadcasting and changing variable IDs";
	$nsr_xpc= -1.0 ;
 go();

#} # endif $mpi_prc == 0...

####################
#### ncea tests ####
####################
    $opr_nm='ncea';
####################
# this is a duplicate of test 04 - only here to run 1st under explict editing.
$tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -C -v pck $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v pck $outfile";
	$dsc_sng="scale factor + add_offset packing/unpacking";
	$nsr_xpc= 3 ;
# go(); # uncomment this line to fire this test

	$tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var -d time,4 $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var $outfile";
	$dsc_sng="ensemble mean of int across two files";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_flt -d time,0 $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v rec_var_flt_mss_val_flt $outfile";
	$dsc_sng="ensemble mean with missing values across two files";
	$nsr_xpc= 1.0e36 ;
 go();

        $tst_cmd[0]="/bin/rm -f $outfile";
	$tst_cmd[1]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl -d time,1 $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[2]="ncks -C -H -s '%e' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="ensemble min of float across two files";
	$nsr_xpc= 2 ;
 go();

	$tst_cmd[0]="/bin/rm -f $outfile";
	$tst_cmd[1]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -C -v pck $in_pth_arg in.nc $outfile";
	$tst_cmd[2]="ncks -C -H -s '%e' -v pck $outfile";
	$dsc_sng="scale factor + add_offset packing/unpacking";
	$nsr_xpc= 3 ;
 go();

	$tst_cmd[0]="/bin/rm -f $outfile";
	$tst_cmd[1]="ncra -Y ncea $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[2]="ncks -C -H -s '%d ' -v rec_var_int_mss_val_int $outfile";
	$dsc_sng="ensemble mean of integer with integer missing values across two files";
	$nsr_xpc= '-999 2 3 4 5 6 7 8 -999 -999' ;
 go();

####################
## ncecat tests ####
####################
    $opr_nm='ncecat';
####################
	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v one $in_pth_arg in.nc $foo1_fl";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -v one $in_pth_arg in.nc $foo2_fl";
	$tst_cmd[2]="ncecat $omp_flg -h -O $fl_fmt $nco_D_flg $foo1_fl $foo2_fl $outfile";
	$tst_cmd[3]="ncks -C -H -s '%f, ' -v one $outfile";
	$dsc_sng="concatenate two files containing only scalar variables";
	$nsr_xpc= "1, 1" ;
 go();

####################
## ncflint tests ###
####################
    $opr_nm='ncflint';
####################
	$tst_cmd[0]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 3,-2 -v one $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v one $outfile";
	$dsc_sng="identity weighting";
	$nsr_xpc= 1.0 ;
 go();

	$tst_cmd[0]="ncrename -h -O $fl_fmt $nco_D_flg -v zero,foo $in_pth_arg in.nc $foo1_fl";
	$tst_cmd[1]="ncrename -h -O $fl_fmt $nco_D_flg -v one,foo $in_pth_arg in.nc $outfile";
	$tst_cmd[2]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -i foo,0.5 -v two $foo1_fl $outfile $outfile";
	$tst_cmd[3]="ncks -C -H -s '%e' -v two $outfile";
	$dsc_sng="identity interpolation";
	$nsr_xpc= 2.0 ;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d lon,1 -v mss_val $in_pth_arg in.nc $foo_x_fl";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -d lon,0 -v mss_val $in_pth_arg in.nc $foo_y_fl";
	$tst_cmd[2]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 0.5,0.5 $foo_x_fl $foo_y_fl $foo_xy_fl";
	$tst_cmd[3]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 0.5,0.5 $foo_y_fl $foo_x_fl $foo_yx_fl";
	$tst_cmd[4]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg $foo_xy_fl $foo_yx_fl $foo_xymyx_fl";
	$tst_cmd[5]="ncks -C -H -s '%g' -v mss_val $foo_xymyx_fl";
	$dsc_sng="switch order of occurrence to test for commutivity";
	$nsr_xpc= 1e+36 ;
 go();

####################
#### ncks tests ####
####################
    $opr_nm='ncks';
####################
	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v lat_T42,lon_T42,gw_T42 $in_pth_arg in.nc $foo_T42_fl";
	$tst_cmd[1]="ncrename -h -O $fl_fmt $nco_D_flg -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon $foo_T42_fl";
	$tst_cmd[2]="ncap -h -O $fl_fmt $nco_D_flg -s 'one[lat,lon]=lat*lon*0.0+1.0' -s 'zero[lat,lon]=lat*lon*0.0' $foo_T42_fl $foo_T42_fl";
	$tst_cmd[3]="ncks -C -H -s '%g' -v one -F -d lon,128 -d lat,64 $foo_T42_fl";
	$nsr_xpc="1";
	$dsc_sng="Create T42 variable named one, uniformly 1.0 over globe in $foo_T42_fl ";
 go();

	#passes, but returned string includes tailing NULLS (<nul> in nedit)
	$tst_cmd[0]="ncks -C -H -s '%c' -v fl_nm $in_pth_arg in.nc";
	$dsc_sng="extract filename string";
	$nsr_xpc= "/home/zender/nco/data/in.cdl" ;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v lev $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f,' -v lev $outfile";
	$dsc_sng="extract a dimension";
	$nsr_xpc= "100.000000,500.000000,1000.000000" ;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v three_dmn_var $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,3,3 $outfile";
	$dsc_sng="extract a variable with limits";
	$nsr_xpc= 23;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v int_var $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v int_var $outfile";
	$dsc_sng="extract variable of type NC_INT";
	$nsr_xpc= "10" ;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,0,0 -d lev,2,2 -d lon,0,,2 -d lon,1,,2 $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var $outfile";
	$dsc_sng="Multi-slab lat and lon with srd";
	$nsr_xpc= "12.0,13.0,14.0,15.0,20.0,21.0,22.0,23.0";
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0,3 -d lon,1,3 $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var $outfile";
	$dsc_sng="Multi-slab with redundant hyperslabs";
	$nsr_xpc= "20.0,21.0,22.0,23.0";
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0.,,2 -d lon,90.,,2 $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var $outfile";
	$dsc_sng="Multi-slab with coordinates";
	$nsr_xpc= "20.0,21.0,22.0,23.0";
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,800.,200. -d lon,270.,0. $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var $outfile";
	$dsc_sng="Double-wrapped hyperslab";
	$nsr_xpc= "23.0,20.0,15.0,12.0";
 go();

# $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_double_dmn -d lon,2,2 -d time,8,8  in.nc $outfile";
# $tst_cmd[1]="ncks -C -H -s '%f,' -v three_double_dmn $outfile";
# $dsc_sng="Hyperslab of a variable that has two identical dims";
# $nsr_xpc= 59.5;
#go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d time_udunits,'1999-12-08 12:00:0.0','1999-12-09 00:00:0.0' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%6.0f' -d time_udunits,'1999-12-08 18:00:0.0','1999-12-09 12:00:0.0',2 -v time_udunits $in_pth_arg in.nc";
	$dsc_sng="dimension slice using UDUnits library (fails without UDUnits library support)";
	$nsr_xpc= 876018;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -H -v wvl -d wvl,'0.4 micron','0.7 micron' -s '%3.1e' $in_pth_arg in.nc";
	$dsc_sng="dimension slice using UDUnit conversion (fails without UDUnits library support)";
	$nsr_xpc= 1.0e-06;
 go();

	#fails
	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v '^three_*' $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f'-C -v three $outfile";
	$dsc_sng="variable wildcards A (fails without regex library)";
	$nsr_xpc= 3 ;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v '^[a-z]{3}_[a-z]{3}_[a-z]{3,}\$' $in_pth_arg in.nc $outfile";
	# for this test, the regex is mod'ed                       ^
	$tst_cmd[1]="ncks -C -H -s '%d' -C -v val_one_int $outfile";
	$dsc_sng="variable wildcards B (fails without regex library)";
	$nsr_xpc= 1;
 go();

	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d time,0,1 -v time $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -C -d time,2, $outfile";
	$dsc_sng="Offset past end of file";
	$nsr_xpc="ncks: ERROR User-specified dimension index range 2 <= time <=  does not fall within valid dimension index range 0 <= time <= 1";
 go();

	$tst_cmd[0]="ncks -C -H -s '%d' -v byte_var $in_pth_arg in.nc";
	$dsc_sng="Print byte value";
	$nsr_xpc= 122 ;
 go();

####################
#### ncpdq tests ####
####################
    $opr_nm='ncpdq';
####################

	$tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a -lat -v lat $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v lat -d lat,0 $outfile";
	$dsc_sng="reverse coordinate";
	$nsr_xpc= 90 ;
 go();

	$tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a -lat,-lev,-lon -v three_dmn_var $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,0 -d lon,0 $outfile";
	$dsc_sng="reverse three dimensional variable";
	$nsr_xpc= 23 ;
 go();

	$tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a lon,lat -v three_dmn_var $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,2 -d lon,3 $outfile";
	$dsc_sng="re-order three dimensional variable";
	$nsr_xpc= 11 ;
 go();

	$tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -P all_new -v upk $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -P upk -v upk $outfile $outfile";
	$tst_cmd[2]="ncks -C -H -s '%g' -v upk $outfile";
	$dsc_sng="Pack and then unpack scalar (uses only add_offset)";
	$nsr_xpc= 3 ;
 go();

####################
#### ncrcat tests ##
####################
    $opr_nm='ncrcat';
####################
#if ($mpi_prc == 0) { # fxm test hangs because of ncrcat TODO 593
	$tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc $outfile 2>$foo_tst";
	$tst_cmd[1]="ncks -C -H -d time,11 -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="Concatenate float with double missing values across two files";
	$nsr_xpc= 2 ;
 go();
#    } else { print "NB: Current mpncrcat test skipped because it hangs fxm TODO nco593.\n";}

####################
#### ncra tests ####
####################
    $opr_nm='ncra';
####################

#        if ($mpi_prc == 0 || ($mpi_prc > 0 && $localhostname !~ /sand/)) { # test hangs because of ncrcat TODO nco593
	$outfile =  $foo1_fl;
	$tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc $outfile 2>$foo_tst";
	$outfile =  $orig_outfile;
	$tst_cmd[1]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y avg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[2]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a time $outfile $outfile";
	$tst_cmd[3]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $foo1_fl $outfile $outfile";
	$tst_cmd[4]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -v rec_var_flt_mss_val_dbl $outfile $outfile";
	$tst_cmd[5]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record sdn of float with double missing values across two files";
	$nsr_xpc= 2 ;
 go();
#    } else { print "NB: Current mpncra test skipped on sand because mpncrcat step hangs fxm TODO nco593\n";}

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var $outfile";
	$dsc_sng="record mean of int across two files";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record mean of float with double missing values";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_int $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_int $outfile";
	$dsc_sng="record mean of float with integer missing values";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_int $outfile";
	$dsc_sng="record mean of integer with integer missing values";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_int $outfile";
	$dsc_sng="record mean of integer with integer missing values across two files (TODO nco543)";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_flt $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_flt $outfile";
	$dsc_sng="record mean of integer with float missing values";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_flt $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_flt $outfile";
	$dsc_sng="record mean of integer with float missing values across two files (TODO nco543)";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_dbl_mss_val_dbl_pck $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_mss_val_dbl_pck $outfile";
	$dsc_sng="record mean of packed double with double missing values";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_dbl_pck $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_pck $outfile";
	$dsc_sng="record mean of packed double to test precision";
	$nsr_xpc= 100.55 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_pck $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%3.2f' -v rec_var_flt_pck $outfile";
	$dsc_sng="record mean of packed float to test precision";
	$nsr_xpc= 100.55 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y avg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record mean of float with double missing values across two files";
	$nsr_xpc= 5 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record min of float with double missing values across two files";
	$nsr_xpc= 2 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record max of float with double missing values across two files";
	$nsr_xpc= 8 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record ttl of float with double missing values across two files";
	$nsr_xpc= 70 ;
 go();

	$tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%1.5f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record rms of float with double missing values across two files";
	$nsr_xpc= 5.38516 ;
 go();

####################
#### ncwa tests ####
####################
    $opr_nm='ncwa';
####################
	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon -w gw -d lat,0.0,90.0 $foo_T42_fl $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v one $outfile";
	$dsc_sng="normalize by denominator upper hemisphere";
	$nsr_xpc= 1;
 go();

#${MY_BIN_DIR}/ncwa -n $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon -w gw $foo_T42_fl$outfile";
#$tst_cmd[1]="ncks -C -H -s '%f' -v one $outfile";
#$dsc_sng="normalize by tally but not weight";
#$nsr_xpc= 0.0312495 ;
#go();
#${MY_BIN_DIR}/ncwa -W $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon -w gw $foo_T42_fl$outfile";
#$tst_cmd[1]="ncks -C -H -s '%f' -v one $outfile";
#$dsc_sng="normalize by weight but not tally";
#$nsr_xpc= 8192 ;
#go();

	$tst_cmd[0]="ncwa -N $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon -w gw $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v mask $outfile";
	$dsc_sng="do not normalize by denominator";
	$nsr_xpc= 50 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lon -v mss_val $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v mss_val $outfile";
	$dsc_sng="average with missing value attribute";
	$nsr_xpc= 73 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lon -v no_mss_val $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v no_mss_val $outfile";
	$dsc_sng="average without missing value attribute";
	$nsr_xpc= 5.0e35 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat -m lat -M 90.0 -T eq -a lat $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v lat $outfile";
	$dsc_sng="average masked coordinate";
	$nsr_xpc= 90.0 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat_var -m lat -M 90.0 -T eq -a lat $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v lat_var $outfile";
	$dsc_sng="average masked variable";
	$nsr_xpc= 2.0 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev -m lev -M 100.0 -T eq -a lev -w lev_wgt $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v lev $outfile";
	$dsc_sng="average masked, weighted coordinate";
	$nsr_xpc= 100.0 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev_var -m lev -M 100.0 -T gt -a lev -w lev_wgt $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v lev_var $outfile";
	$dsc_sng="average masked, weighted variable";
	$nsr_xpc= 666.6667 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat -a lat -w gw -d lat,0 $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v lat $outfile";
	$dsc_sng="weight conforms to var first time";
	$nsr_xpc= -90.0 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v mss_val_all -a lon -w lon $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val_all $outfile";
	$dsc_sng="average all missing values with weights";
	$nsr_xpc= 1.0e36 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v val_one_mss -a lat -w wgt_one $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v val_one_mss $outfile";
	$dsc_sng="average some missing values with unity weights";
	$nsr_xpc= 1.0 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v msk_prt_mss_prt -m msk_prt_mss_prt -M 1.0 -T lt -a lon $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v msk_prt_mss_prt $outfile";
	$dsc_sng="average masked variable with some missing values";
	$nsr_xpc= 0.5 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc $outfile 2>$foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%e' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="min switch on type double, some missing values";
	$nsr_xpc= 2 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg  -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_dbl -a lon $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f,' -v three_dmn_var_dbl $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 7 $foo_fl";
	$dsc_sng="Dimension reduction with min switch and missing values";
	$nsr_xpc= -99 ;
 go();
	$tst_cmd[0]="cut -d, -f 20 $foo_fl";
	$dsc_sng="Dimension reduction with min switch";
	$nsr_xpc= 77 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_int -a lon $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d,' -v three_dmn_var_int $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 5 $foo_fl";
	$dsc_sng="Dimension reduction on type int with min switch and missing values";
	$nsr_xpc= -99 ;
 go();
	$tst_cmd[0]="cut -d, -f 7 $foo_fl";
	$dsc_sng="Dimension reduction on type int variable";
	$nsr_xpc= 25 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_sht -a lon $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d,' -v three_dmn_var_sht $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 20 $foo_fl";
	$dsc_sng="Dimension reduction on type short variable with min switch and missing values";
	$nsr_xpc= -99 ;
 go();
	$tst_cmd[0]="cut -d, -f 8 $foo_fl";
	$dsc_sng="Dimension reduction on type short variable";
	$nsr_xpc= 29 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_rec_var $in_pth_arg in.nc $outfile 2>$foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_rec_var $outfile";
	$dsc_sng="Dimension reduction with min flag on type float variable";
	$nsr_xpc= 1 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v four_dmn_rec_var $in_pth_arg in.nc $outfile 2> $foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%f' -v four_dmn_rec_var $outfile";
	$dsc_sng="Max flag on type float variable";
	$nsr_xpc= 240 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_dbl -a lat,lon $in_pth_arg in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f,' -v three_dmn_var_dbl $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 4 $foo_fl";
	$dsc_sng="Dimension reduction on type double variable with max switch and missing values";
	$nsr_xpc= -99 ;
 go();
	$tst_cmd[0]="cut -d, -f 5 $foo_fl";
	$dsc_sng="Dimension reduction on type double variable";
	$nsr_xpc= 40 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_int -a lat $in_pth_arg in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%d,' -v three_dmn_var_int $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 9 $foo_fl";
	$dsc_sng="Dimension reduction on type int variable with min switch and missing values";
	$nsr_xpc= -99 ;
 go();
	$tst_cmd[0]="cut -d, -f 13 $foo_fl";
	$dsc_sng="Dimension reduction on type int variable";
	$nsr_xpc= 29 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_sht -a lat $in_pth_arg in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%d,' -v three_dmn_var_sht $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 37 $foo_fl";
	$dsc_sng="Dimension reduction on type short variable with max switch and missing values";
	$nsr_xpc= -99 ;
 go();
	$tst_cmd[0]="cut -d, -f 33 $foo_fl";
	$dsc_sng="Dimension reduction on type short, max switch variable";
	$nsr_xpc= 69 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -w lat_wgt -v lat $in_pth_arg in.nc $outfile 2>$foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%f' -v lat $outfile";;
	$dsc_sng="rms with weights";
	$nsr_xpc= 90 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -w val_half_half -v val_one_one_int $in_pth_arg in.nc $outfile 2> $foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%ld' -v val_one_one_int $outfile";;
	$dsc_sng="weights would cause SIGFPE without dbl_prc patch";
	$nsr_xpc= 1 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y avg -v val_max_max_sht $in_pth_arg in.nc $outfile 2> $foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%d' -v val_max_max_sht $outfile";;
	$dsc_sng="avg would overflow without dbl_prc patch";
	$nsr_xpc= 17000 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v val_max_max_sht $in_pth_arg in.nc $outfile 2> $foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%d' -v val_max_max_sht $outfile";
	$dsc_sng="ttl would overflow without dbl_prc patch, wraps anyway so exact value not important (failure expected/OK on Xeon chips because of different wrap behavior)";
	$nsr_xpc= -31536 ; # Expected on Pentium IV GCC Debian 3.4.3-13, PowerPC xlc
#    $nsr_xpc= -32768 ; # Expected on Xeon GCC Fedora 3.4.2-6.fc3
#    $nsr_xpc= -32768 ; # Expected on PentiumIII (Coppermine) gcc 3.4 MEPIS
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -a lat -v lat -w gw $in_pth_arg in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%g' -v lat $outfile";;
	$dsc_sng="min with weights";
	$nsr_xpc= -900 ;
 go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -a lat -v lat -w gw $in_pth_arg in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%g' -v lat $outfile";;
	$dsc_sng="max with weights";
	$nsr_xpc= 900 ;
 go();


####################
##### net tests ####
####################
    $opr_nm='net';
####################
	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -v one -p ftp://dust.ess.uci.edu/pub/zender/nco -l /tmp in.nc";
	$dsc_sng="Anonymous FTP protocol (requires anonymous FTP access to dust.ess.uci.edu)";
	$nsr_xpc= 1.000000e+00 ;
 go();

	my $sftp_url = "sftp://dust.ess.uci.edu:/home/ftp/pub/zender/nco";

	if ($dust_usr ne ""){ # if we need to connect as another user (hmangalm@esmf -> hjm@dust))
		 $sftp_url =~ s/dust/$dust_usr\@dust/;
	}

#sftp://dust.ess.uci.edu:/home/ftp/pub/zender/nco
	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -O $nco_D_flg -v one -p $sftp_url -l /tmp in.nc";
	$tst_cmd[2]="ncks -H $nco_D_flg -s '%e' -v one -l /tmp in.nc";
	$dsc_sng="Secure FTP (SFTP) protocol (requires SFTP access to dust.ess.uci.edu)";
	$nsr_xpc= 1.000000e+00 ;
 go();

if ($dust_usr ne ""){ # if we need to connect as another user (hmangalm@esmf -> hjm@dust))
	$pth_rmt_scp_tst = $dust_usr . '@' . $pth_rmt_scp_tst;
}

	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg  -s '%e' -v one -p $pth_rmt_scp_tst -l /tmp in.nc";
	$dsc_sng="SSH protocol (requires authorized SSH/scp access to dust.ess.uci.edu)";
	$nsr_xpc= 1 ;
 go();

	$tst_cmd[0]="ncks -C -O -d lon,0 -s '%e' -v lon -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc";
	$dsc_sng="OPeNDAP protocol (requires OPeNDAP/DODS-enabled NCO)";
	$nsr_xpc= 0 ;
 go();

	if($USER eq 'zender'){
	    $tst_cmd[0]="/bin/rm -f /tmp/etr_A4.SRESA1B_9.CCSM.atmd.2000_cat_2099.nc";
	    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -d time,0 -v time -p ftp://climate.llnl.gov//sresa1b/atm/yr/etr/ncar_ccsm3_0/run9 -l /tmp etr_A4.SRESA1B_9.CCSM.atmd.2000_cat_2099.nc";
	    $dsc_sng="Password-protected FTP protocol (requires .netrc-based FTP access to climate.llnl.gov)";
	    $nsr_xpc= 182.5;
        go();

	    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -v one -p mss:/ZENDER/nc -l /tmp in.nc";
	    $tst_cmd[2]="ncks -C -H -s '%e' -v one $outfile";
	    $dsc_sng="msrcp protocol (requires msrcp and authorized access to NCAR MSS)";
	    $nsr_xpc= 1;
        go();
	} else { print "WARN: Skipping net tests of mss: and password protected FTP protocol retrieval---user not zender\n";}

	if($USER eq 'zender' || $USER eq 'hjm'){
	    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -v one -p wget://dust.ess.uci.edu/nco -l /tmp in.nc";
	    $dsc_sng="HTTP protocol (requires developers to implement wget in NCO nudge nudge wink wink)";
	    $nsr_xpc= 1;
        go();
	} else { print "WARN: Skipping net test wget: protocol retrieval---not implemented yet\n";}

} # end of perform_test()
