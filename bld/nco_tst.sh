#!/bin/sh

# $Header: /data/zender/nco_20150216/nco/bld/nco_tst.sh,v 1.1.1.1 1998-08-18 05:35:00 zender Exp $

# Purpose: a battery of tests for the nc operators

# Create a T42 size test field named one, which is identically 1.
ncdiff -O -v ORO,PS,gw /data2/zender/ccm/SEP1.nc /data2/zender/ccm/SEP1.nc /data2/zender/ccm/foo.nc 2> /dev/null
ncrename -O -v PS,negative_one /data2/zender/ccm/foo.nc 2> /dev/null
ncdiff -O -v ORO,negative_one,gw /data2/zender/ccm/foo.nc /home/zender/nc/in.nc /data2/zender/ccm/foo.nc 2> /dev/null
ncrename -O -v negative_one,one /data2/zender/ccm/foo.nc 2> /dev/null

# Average the test field
ncwa -O -a lat,lon -w gw /data2/zender/ccm/foo.nc /data2/zender/ccm/foo2.nc 2> /dev/null
avg=`ncks -C -H -s "" -v one /data2/zender/ccm/foo2.nc`
echo "ncwa 1: normalize by tally and weight: 1 =?= $avg" 

ncwa -n -O -a lat,lon -w gw /data2/zender/ccm/foo.nc /data2/zender/ccm/foo2.nc 2> /dev/null
avg=`ncks -C -H -s "" -v one /data2/zender/ccm/foo2.nc`
echo "ncwa 2: normalize by tally but not weight: .0312495 =?= $avg" 

ncwa -W -O -a lat,lon -w gw /data2/zender/ccm/foo.nc /data2/zender/ccm/foo2.nc 2> /dev/null
avg=`ncks -C -H -s "" -v one /data2/zender/ccm/foo2.nc`
echo "ncwa 3: normalize by weight but not tally: 8192 =?= $avg" 

ncwa -N -O -a lat,lon -w gw /data2/zender/ccm/foo.nc /data2/zender/ccm/foo2.nc 2> /dev/null
avg=`ncks -C -H -s "" -v one /data2/zender/ccm/foo2.nc`
echo "ncwa 4: no normalization by tally or weight: 256 =?= $avg" 

ncwa -O -a lon -v mss_val /home/zender/nc/in.nc /home/zender/nc/foo.nc 2> /dev/null
avg=`ncks -C -H -s "" -v mss_val /home/zender/nc/foo.nc`
echo "ncwa 5: average with missing value attribute: 73 =?= $avg" 

ncwa -O -a lon -v no_mss_val /home/zender/nc/in.nc /home/zender/nc/foo.nc 2> /dev/null
avg=`ncks -C -H -s "" -v no_mss_val /home/zender/nc/foo.nc`
echo "ncwa 6: average without missing value attribute: 5e35 =?= $avg" 

ncdiff -O -d lon,1 -v mss_val /home/zender/nc/in.nc /home/zender/nc/in.nc /home/zender/nc/foo.nc 2> /dev/null
avg=`ncks -C -H -s "" -v mss_val /home/zender/nc/foo.nc`
echo "ncwa 7: difference with missing value attribute: 1.e36 =?= $avg" 

ncdiff -O -d lon,0 -v no_mss_val /home/zender/nc/in.nc /home/zender/nc/in.nc /home/zender/nc/foo.nc 2> /dev/null
avg=`ncks -C -H -s "" -v no_mss_val /home/zender/nc/foo.nc`
echo "ncwa 8: difference without missing value attribute: 0 =?= $avg" 
