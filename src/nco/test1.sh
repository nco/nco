#!/bin/sh
test="test1.sh"
program="ncks"
datfile="nco_tst.nc"
datdir="../../data"
options="-R -p ftp://dust.ess.uci.edu/pub/zender/nco -l $datdir $datfile"

if test -f $datdir/$datfile; then
   printf "$test: Data file \"$datfile\" already exists locally\n" ; 
else 
  exec $program $options
  if [ $? != 0 ]; then 
     printf "WARNING: Unable to retrieve ftp://dust.ess.uci.edu/pub/zender/nco/nco_tst.nc required for self-test, possible problem with getting through your firewall? Manually download and install in directory ../data to continue self test...\n"
  exit 1
  fi
fi
exit 0
