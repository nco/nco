#!/bin/sh

# $Header: /data/zender/nco_20150216/nco/data/nco_bnch.sh,v 1.1 2004-07-19 23:50:33 zender Exp $

# Purpose: Benchmark NCO performance

# Usage:

ncap -D 3 -O -s "wvl_1e8[wvl_1e8]=1.0f" ${DATA}/mie/big.nc ${DATA}/mie/big.nc

# Benchmark arithmetic operations on large files
timex ncwa -O -a wvl_1e8 -v wvl_1e8 ${DATA}/mie/big.nc ${DATA}/mie/foo.nc;ncks -H ${DATA}/mie/foo.nc
date;ncwa -O -a wvl_1e8 -v wvl_1e8 ${DATA}/mie/big.nc ${DATA}/mie/foo.nc;date;ncks -H ${DATA}/mie/foo.nc
# Results: 
# elnino gcc 3.3.4 with restrict keyword     20040719: 11m31s
# elnino gcc 3.3.4 without restrict keyword  20040719: 10m20s
# esmf04m xlc 6.x with restrict keyword      20040719: 22s
# esmf04m xlc 6.x without restrict keyword   20040719: 23s
# 2^32=4294967296
# 2^31=2147483648
# 10^9=1000000000
# 10^8= 100000000

