#!/bin/sh
test="test2.sh"
program="./ncks"
datfile="dods_test.nc"
options=" -C -d lon,0 -v lon -l ./ -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc $datfile"

eval $program $options
if (test $? != 0); then
  echo $program $options
  printf "Failed to retrieve remote file using DODS protocol\n"
  exit 1
fi
rm -rf $datfile
exit 0
