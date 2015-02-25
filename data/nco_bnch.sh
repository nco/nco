#!/bin/sh

# $Header$

# Purpose: Benchmark NCO performance

# Usage:

# Create file with dimension sizes
ncgen -b -o ~/nco/data/big.nc ~/nco/data/big.cdl

# Create file with decadal differences in dimension sizes
ncap -D 3 -O \
-s "wvl_1e0[wvl_1e0]=1.0f" \
-s "wvl_1e1[wvl_1e1]=1.0f" \
-s "wvl_1e2[wvl_1e2]=1.0f" \
-s "wvl_1e3[wvl_1e3]=1.0f" \
-s "wvl_1e4[wvl_1e4]=1.0f" \
-s "wvl_1e5[wvl_1e5]=1.0f" \
-s "wvl_1e6[wvl_1e6]=1.0f" \
-s "wvl_1e7[wvl_1e7]=1.0f" \
-s "wvl_1e8[wvl_1e8]=1.0f" \
~/nco/data/big.nc ${DATA}/tmp/big.nc

# Verify big file dimensionality
ls -l ${DATA}/tmp/big.nc
ncks -M -m ${DATA}/tmp/big.nc

# Benchmarks for ncwa 
# ncwa_b1:
# Average 1-D arrays of multiple sizes
timex ncwa -O -a wvl_1e8 -v wvl_1e8 ${DATA}/tmp/big.nc ${DATA}/tmp/foo.nc;ncks -H ${DATA}/tmp/foo.nc
date;ncwa -O ${DATA}/tmp/big.nc ${DATA}/tmp/foo.nc;date;ncks -H ${DATA}/tmp/foo.nc
date;ncwa -O -a wvl_1e8 -v wvl_1e8 ${DATA}/tmp/big.nc ${DATA}/tmp/foo.nc;date;ncks -H ${DATA}/tmp/foo.nc

# Results: 
# elnino gcc 3.3.4 with restrict keyword     20040719: 11m31s
# elnino gcc 3.3.4 without restrict keyword  20040719: 10m20s
# esmf04m xlc 6.x with restrict keyword      20040719: 25s
# esmf04m xlc 6.x without restrict keyword   20040719: 25s
# 2^32=4294967296
# 2^31=2147483648
# 10^9=1000000000
# 10^8= 100000000

# Benchmarks for ncra
# Create file with multiple long record arrays
ncap -D 3 -O \
-s "one[wvl_1e7]=1.0f" \
-s "two[wvl_1e7]=2.0f" \
-s "three[wvl_1e7]=3.0f" \
-s "four[wvl_1e7]=4.0f" \
~/nco/data/big.nc ${DATA}/tmp/big.nc

# ncra_b1:
# Running average with one thread
date;ncra -O -t 1 -p ${DATA}/tmp big.nc ${DATA}/tmp/foo.nc;date
# Running average with multiple threads
date;ncra -O -t 4 -p ${DATA}/tmp big.nc ${DATA}/tmp/foo.nc;date

# ncea_b1:
# Running average with one thread
date;ncea -O -t 1 -p ${DATA}/tmp big.nc big.nc big.nc big.nc ${DATA}/tmp/foo.nc;date
# Running average with multiple threads
date;ncea -O -t 4 -p ${DATA}/tmp big.nc big.nc big.nc big.nc ${DATA}/tmp/foo.nc;date
