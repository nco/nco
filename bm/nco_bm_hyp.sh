# $Header: /data/zender/nco_20150216/nco/bm/nco_bm_hyp.sh,v 1.1 2006-05-04 00:45:36 zender Exp $

# Purpose: Perform some NCO benchmarks while hyperslabbing 

# 1
~/nco/bm/clearcache.pl; /usr/bin/time -a -o ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa -h -O -w lat -v d4_[00-03],d3_[00-07],d2_[00-01] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 2
~/nco/bm/clearcache.pl; /usr/bin/time -a -o ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa -h -O -w lat -v d4_[00-07],d3_[00-15],d2_[00-03] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 3
~/nco/bm/clearcache.pl; /usr/bin/time -a -o ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa -h -O -w lat -v d4_[00-11],d3_[00-23],d2_[00-05] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 4
~/nco/bm/clearcache.pl; /usr/bin/time -a -o ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa -h -O -w lat -v d4_[00-15],d3_[00-31],d2_[00-07] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 5
~/nco/bm/clearcache.pl; /usr/bin/time -a -o ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa -h -O -w lat -v d4_[00-19],d3_[00-39],d2_[00-09] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 6
~/nco/bm/clearcache.pl; /usr/bin/time -a -o ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa -h -O -w lat -v d4_[00-23],d3_[00-47],d2_[00-11] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 7
~/nco/bm/clearcache.pl; /usr/bin/time -a -o ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa -h -O -w lat -v d4_[00-27],d3_[00-55],d2_[00-13] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 8
~/nco/bm/clearcache.pl; /usr/bin/time -a -o ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa -h -O -w lat -v d4_[00-33],d3_[00-63],d2_[00-15] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
