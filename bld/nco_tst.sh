#!/bin/sh

# $Header: /data/zender/nco_20150216/nco/bld/nco_tst.sh,v 1.33 2000-11-03 19:11:21 zender Exp $

# Purpose: NCO test battery

# Usage: 

usage () {
    echo >&2 "Usage: ${basename $0} ncra | ncea | ncwa | ncflint | ncdiff | net" ;
}

START=0
NCWA=0
NCRA=0
NCEA=0
NCFLINT=0
NCDIFF=0
NET=0

if [ $# -eq 0 ]
then
    START=1
    NCWA=1
    NCRA=1
    NCEA=1
    NCFLINT=1
    NCDIFF=1
    NET=1
else 
    
    while [ $# -gt 0 ]
    do
	case $1 in
	ncwa)
	    NCWA=1
	    shift
	    ;;
	ncra)
	    NCRA=1
	    shift
	    ;;
	ncea)
	    NCEA=1
	    shift
	    ;;
	ncflint)
	    NCFLINT=1
	    shift
	    ;;
	ncdiff)
	    NCDIFF=1
	    shift
	    ;;
	net)
	    NET=1
	    shift
	    ;;
	    
	  *)
	    usage 
	    exit 0
	    ;;
       esac
    done 
    
fi    



# T42-size test field named one, which is identically 1.0 in foo.nc
cd ../data 2> foo.tst
printf "NCO Test Suite:\n"


if [ "$START" = 1 ]
then

# ncks -O -v PS,gw /fs/cgd/csm/input/atm/SEP1.T42.0596.nc ~/nco/data/nco_tst.nc
# Subtract PS from itself gives zero valued array
ncdiff -O -v PS nco_tst.nc nco_tst.nc foo.nc 2>> foo.tst
# Rename zero-valued PS array to array named negative_one (which will be renamed zero below)
ncrename -O -v PS,negative_one foo.nc 2>> foo.tst
# Zero-valued array minus negative one scalar value gives one-valued array
ncdiff -O -C -v negative_one foo.nc in.nc foo2.nc 2>> foo.tst
# Rename one-valued array from negative_one to one
ncrename -O -v negative_one,one foo2.nc 2>> foo.tst
# Append one-valued array to foo.nc
ncks -A -C -v one foo2.nc foo.nc 2>> foo.tst
# Append Gaussian weight array to foo.nc
ncks -A -C -v gw nco_tst.nc foo.nc 2>> foo.tst
# Rename zero-valued array named negative_one to array named zero
ncrename -O -v negative_one,zero foo.nc 2>> foo.tst
# Get rid of working file
/bin/rm -f foo2.nc 2>> foo.tst
fi # end start


# Average test field
if [ "$NCWA" = 1 ]
then
ncwa -O -a lat,lon -w gw foo.nc foo2.nc
avg=`ncks -C -H -s "%f" -v one foo2.nc`
echo "ncwa 1: normalize by denominator: 1.0 =?= $avg" 

#ncwa -n -O -a lat,lon -w gw foo.nc foo2.nc
#avg=`ncks -C -H -s "%f" -v one foo2.nc`
#echo "ncwa 2: normalize by tally but not weight: 0.0312495 =?= $avg" 

#ncwa -W -O -a lat,lon -w gw foo.nc foo2.nc
#avg=`ncks -C -H -s "%f" -v one foo2.nc`
#echo "ncwa 3: normalize by weight but not tally: 8192 =?= $avg" 

ncwa -N -O -a lat,lon -w gw foo.nc foo2.nc
avg=`ncks -C -H -s "%f" -v one foo2.nc`
echo "ncwa 4: do not normalize by denominator: 256 =?= $avg" 

ncwa -O -a lon -v mss_val in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v mss_val foo.nc`
echo "ncwa 5: average with missing value attribute: 73 =?= $avg" 

ncwa -O -a lon -v no_mss_val in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v no_mss_val foo.nc`
echo "ncwa 6: average without missing value attribute: 5.0e35 =?= $avg" 

ncwa -O -v lat -m lat -M 90.0 -o eq -a lat in.nc foo.nc 2>>foo.tst 
avg=`ncks -C -H -s "%e" -v lat foo.nc`
echo "ncwa 7: average masked coordinate: 90.0 =?= $avg" 

ncwa -O -v lat_var -m lat -M 90.0 -o eq -a lat in.nc foo.nc 2>>foo.tst 
avg=`ncks -C -H -s "%e" -v lat_var foo.nc`
echo "ncwa 8: average masked variable: 2.0 =?= $avg" 

ncwa -O -v lev -m lev -M 100.0 -o eq -a lev -w lev_wgt in.nc foo.nc 2>>foo.tst 
avg=`ncks -C -H -s "%e" -v lev foo.nc`
echo "ncwa 9: average masked, weighted coordinate: 100.0 =?= $avg" 

ncwa -O -v lev_var -m lev -M 100.0 -o gt -a lev -w lev_wgt in.nc foo.nc 2>>foo.tst 
avg=`ncks -C -H -s "%e" -v lev_var foo.nc`
echo "ncwa 10: average masked, weighted variable: 666.6667 =?= $avg" 

ncwa -O -v lat -a lat -w gw -d lat,0 in.nc foo.nc 2>>foo.tst 
avg=`ncks -C -H -s "%e" -v lat foo.nc`
echo "ncwa 11: weight conforms to var first time: -90.0 =?= $avg" 

ncwa -O -v mss_val_all -a lon -w lon in.nc foo.nc 2>>foo.tst 
avg=`ncks -C -H -s "%e" -v mss_val_all foo.nc`
echo "ncwa 12: average all missing values with weights: 1.0e36 =?= $avg" 

ncwa -O -v val_one_mss -a lat -w wgt_one in.nc foo.nc 2>>foo.tst 
avg=`ncks -C -H -s "%e" -v val_one_mss foo.nc`
echo "ncwa 13: average some missing values with unity weights: 1.0 =?= $avg" 

ncwa -O -v msk_prt_mss_prt -m msk_prt_mss_prt -M 1.0 -o lt -a lon in.nc foo.nc 2>>foo.tst 
avg=`ncks -C -H -s "%e" -v msk_prt_mss_prt foo.nc`
echo "ncwa 14: average masked variable with some missing values: 0.5 =?= $avg" 

ncwa -O -y min -v rec_var_flt_mss_val_dbl in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%e" -v rec_var_flt_mss_val_dbl foo.nc`
echo "ncwa 15: min swich on type double, some missing values: 2 =?= $avg" 

ncwa  -O -y min -v three_dmn_var_dbl -a lon in.nc foo.nc 2>>foo.tst
ncks -C -H -s "%f," -v three_dmn_var_dbl foo.nc >foo$$
avg=`cut -d, -f 7 foo$$`
echo "ncwa 16: Dimension reduction with min switch and missing values: -99 =?= $avg"
avg=`cut -d, -f 20 foo$$`
echo "ncwa 17: Dimension reduction with min switch : 77 =?= $avg"

ncwa -O -y min -v three_dmn_var_lng -a lon in.nc foo.nc 2>>foo.tst
ncks -C -H -s "%d," -v three_dmn_var_lng foo.nc>foo$$
avg=`cut -d, -f 5 foo$$`
echo "ncwa 18: Dimension reduction on type long  with min switch and missing values: -99 =?= $avg"
avg=`cut -d, -f 7 foo$$`
echo "ncwa 19: Dimension reduction on type long variable: 25 =?= $avg"

ncwa -O -y min -v three_dmn_var_sht -a lon in.nc foo.nc 2>>foo.tst
ncks -C -H -s "%d," -v three_dmn_var_sht foo.nc>foo$$
avg=`cut -d, -f 20 foo$$`
echo "ncwa 20: Dimension reduction on type short variable with min switch and missing values: -99 =?= $avg"
avg=`cut -d, -f 8 foo$$`
echo "ncwa 21: Dimension reduction on type short variable: 29 =?= $avg"

ncwa -O -y min -v three_dmn_rec_var  in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%f" -v three_dmn_rec_var foo.nc`
echo "ncwa 22: Dimension reduction with min flag on type float variable: 1 =?= $avg"

ncwa -O -y max  -v four_dmn_rec_var  in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%f" -v four_dmn_rec_var foo.nc`
echo "ncwa 23: Max flag on type float variable: 240 =?= $avg"

ncwa -O -y max  -v three_dmn_var_dbl  -a lat,lon in.nc foo.nc 2>>foo.tst
ncks -C -H -s "%f," -v three_dmn_var_dbl foo.nc>foo$$
avg=`cut -d, -f 4 foo$$`
echo "ncwa 24: Dimension reduction on type double  variable with max switch and missing values: -99 =?= $avg"
avg=`cut -d, -f 5 foo$$`
echo "ncwa 25: Dimension reduction on type double variable: 40 =?= $avg"

ncwa -O -y max -v three_dmn_var_lng -a lat in.nc foo.nc 2>>foo.tst
ncks -C -H -s "%d," -v three_dmn_var_lng foo.nc>foo$$
avg=`cut -d, -f 9 foo$$`
echo "ncwa 26: Dimension reduction on type long variable with min switch and missing values: -99 =?= $avg"
avg=`cut -d, -f 13 foo$$`
echo "ncwa 27: Dimension reduction on type long variable: 29 =?= $avg"

ncwa -O -y max -v three_dmn_var_sht -a lat in.nc foo.nc 2>>foo.tst
ncks -C -H -s "%d," -v three_dmn_var_sht foo.nc>foo$$
avg=`cut -d, -f 37 foo$$`
echo "ncwa 28: Dimension reduction on type short variable with max switch and missing values: -99 =?= $avg"
avg=`cut -d, -f 33 foo$$`
echo "ncwa 29: Dimension reduction on type short, max switch variable: 69 =?= $avg"

ncwa -O -y rms -w lat_wgt -v lat in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%f" -v lat foo.nc`
echo "ncwa 30: rms with weights: 90 =?= $avg" 

ncwa -O -w val_half_half -v val_one_one_lng in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%ld" -v val_one_one_lng foo.nc`
echo "ncwa 31: weights would cause SIGFPE without dbl_prc patch: 1L =?= $avg" 

ncwa -O -y avg -v val_max_max_sht in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%d" -v val_max_max_sht foo.nc`
echo "ncwa 32: avg would overflow without dbl_prc patch: 17000S =?= $avg" 

ncwa -O -y ttl -v val_max_max_sht in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%d" -v val_max_max_sht foo.nc`
echo "ncwa 33: ttl would overflow without dbl_prc patch, wraps anyway: -31536S =?= $avg" 
ncwa -O -y min -a lat -v lat -w gw in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%g" -v lat foo.nc`
echo "ncwa 34: min with weights: -900 =?= $avg" 

ncwa -O -y max -a lat -v lat -w gw in.nc foo.nc 2>>foo.tst
avg=`ncks -C -H -s "%g" -v lat foo.nc`
echo "ncwa 35: max with weights: 900 =?= $avg" 
fi  # end ncwa

if [ "$NCRA" = 1 ]
then
ncra -O -v one_dmn_rec_var in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%d" -v one_dmn_rec_var foo.nc`
echo "ncra 1: record mean of long across two files: 5 =?= $avg" 

ncra -O -v rec_var_flt_mss_val_dbl in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc`
echo "ncra 2: record mean of float with double missing values: 5 =?= $avg" 

ncra -O -y avg -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc`
echo "ncra 3: record mean of float with double missing values across two files: 5 =?= $avg" 

ncra -O -y min -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc`
echo "ncra 4: record min of float with double missing values across two files: 2 =?= $avg" 

ncra -O -y max -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc`
echo "ncra 5: record max of float with double missing values across two files: 8 =?= $avg" 

ncra -O -y ttl -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc`
echo "ncra 6: record ttl of float with double missing values across two files: 70 =?= $avg"

ncra -O -y rms -v rec_var_flt_mss_val_dbl in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo.nc`
echo "ncra 7: record rms of float with double missing values across two files: 5.385164807 =?= $avg"



ncrcat -O -v rec_var_flt_mss_val_dbl in.nc in.nc foo1.nc 2>> foo.tst
ncra -O -y avg -v rec_var_flt_mss_val_dbl in.nc in.nc foo2.nc 2>> foo.tst
ncwa -O -a time foo2.nc foo2.nc 2>> foo.tst
ncdiff -O -v rec_var_flt_mss_val_dbl foo1.nc foo2.nc foo2.nc 2>> foo.tst
ncra -O -y rms -v rec_var_flt_mss_val_dbl foo2.nc foo2.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v rec_var_flt_mss_val_dbl foo2.nc`
echo "ncra 8: record sdn of float with double missing values across two files: 2 =?= $avg"
fi #end ncra


if [ "$NCEA" = 1 ]
then
ncea -O -v one_dmn_rec_var -d time,4 in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%d" -v one_dmn_rec_var foo.nc`
echo "ncea 1: ensemble mean of long across two files: 5 =?= $avg" 

ncea -O -v rec_var_flt_mss_val_flt -d time,0 in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v rec_var_flt_mss_val_flt foo.nc`
echo "ncea 2: ensemble mean with missing values across two files: 1.0e36 =?= $avg" 

ncea -O -y min -v rec_var_flt_mss_val_dbl -d time,1 in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v rec_var_flt_mss_val_dbl foo.nc`
echo "ncea 3: ensemble min of float across two files: 2 =?= $avg" 

ncea -O -C -v pck in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v pck foo.nc`
echo "ncea 4: scale factor + add_offset packing/unpacking: 3 =?= $avg" 
fi # end ncea

if [ "$NCDIFF" = 1 ]
then
ncdiff -O -d lon,1 -v mss_val in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v mss_val foo.nc`
echo "ncdiff 1: difference with missing value attribute: 1.0e36 =?= $avg" 

ncdiff -O -d lon,0 -v no_mss_val in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v no_mss_val foo.nc`
echo "ncdiff 2: difference without missing value attribute: 0 =?= $avg" 
fi # end ncdiff

if [ "$NCFLINT" = 1 ]
then
ncflint -O -w 3,-2 -v one in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v one foo.nc`
echo "ncflint 1: identity weighting: 1.0 =?= $avg" 

ncrename -O -v zero,foo in.nc foo1.nc 2>> foo.tst
ncrename -O -v one,foo in.nc foo2.nc 2>> foo.tst
ncflint -O -i foo,0.5 -v two foo1.nc foo2.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v two foo.nc`
echo "ncflint 2: identity interpolation: 2.0 =?= $avg" 
fi #end ncflint

if [ "$NET" = 1 ]
then
/bin/rm -f foo.nc;mv in.nc in_tmp.nc;
ncks -O -v one -p ftp://ftp.cgd.ucar.edu/pub/zender/nco -l ./ in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v one foo.nc 2>> foo.tst`
echo "nco 1: FTP protocol: 1 =?= $avg (Will fail if unable to anonymous FTP to ftp.cgd.ucar.edu)" 
mv in_tmp.nc in.nc

/bin/rm -f foo.nc;mv in.nc in_tmp.nc;
ncks -O -v one -p goldhill.cgd.ucar.edu:/home/zender/nco/data -l ./ in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v one foo.nc 2>> foo.tst`
echo "nco 2: scp/rcp protocol: 1 =?= $avg (Will fail if no SSH/RSH access to goldhill.cgd.ucar.edu)" 
mv in_tmp.nc in.nc

/bin/rm -f foo.nc;mv in.nc in_tmp.nc;
ncks -O -v one -p mss:/ZENDER/nc -l ./ in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v one foo.nc 2>> foo.tst`
echo "nco 3: msrcp protocol: 1 =?= $avg (Will fail if not at NCAR)" 
mv in_tmp.nc in.nc

/bin/rm -f foo.nc;mv in.nc in_tmp.nc;
ncks -O -v one -p http://dust.ps.uci.edu/pub/zender/nco -l ./ in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v one foo.nc 2>> foo.tst`
echo "nco 4: HTTP protocol: 1 =?= $avg (Will always fail until HTTP implemented in NCO)" 
mv in_tmp.nc in.nc

ncks -C -d lon,0 -v lon -l ./ -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v lon foo.nc 2>> foo.tst`
echo "nco 5: HTTP/DODS protocol: 0 =?= $avg (Will fail if not compiled on Linux with 'make DODS=Y')" 
fi #end net


