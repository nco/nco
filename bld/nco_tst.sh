#!/bin/sh

# $Header: /data/zender/nco_20150216/nco/bld/nco_tst.sh,v 1.9 1999-10-04 06:04:07 zender Exp $

# Purpose: NCO test battery

# Create T42-size test field named one, which is identically 1.0 in foo.nc
cd ../data 2> foo.tst
printf "NCO Test Suite:\n"
# ncks -O -v PS,gw /fs/cgd/csm/input/atm/SEP1.T42.0596.nc ~/nc/nco/data/nco_tst.nc
ncdiff -O -v PS nco_tst.nc nco_tst.nc foo.nc 2>> foo.tst
ncrename -O -v PS,negative_one foo.nc 2>> foo.tst
ncdiff -O -C -v negative_one foo.nc in.nc foo2.nc 2>> foo.tst
ncrename -O -v negative_one,one foo2.nc 2>> foo.tst
ncks -A -C -v one foo2.nc foo.nc 2>> foo.tst
ncks -A -C -v gw nco_tst.nc foo.nc 2>> foo.tst
ncrename -O -v negative_one,zero foo.nc 2>> foo.tst
/bin/rm -f foo2.nc 2>> foo.tst

# Average test field
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
echo "ncwa 12: average of missing values with weights: 1.0e36 =?= $avg" 

ncdiff -O -d lon,1 -v mss_val in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%e" -v mss_val foo.nc`
echo "ncdiff 1: difference with missing value attribute: 1.0e36 =?= $avg" 

ncdiff -O -d lon,0 -v no_mss_val in.nc in.nc foo.nc 2>> foo.tst
avg=`ncks -C -H -s "%f" -v no_mss_val foo.nc`
echo "ncdiff 2: difference without missing value attribute: 0 =?= $avg" 
