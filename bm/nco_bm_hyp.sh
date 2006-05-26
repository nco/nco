# $Header: /data/zender/nco_20150216/nco/bm/nco_bm_hyp.sh,v 1.5 2006-05-26 04:44:16 zender Exp $

# Purpose: Perform NCO benchmarks while hyperslabbing 

# Usage:
# ~/nco/bm/nco_bm_hyp.sh

# 1
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-03],d3_[00-07],d2_[00-01] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 2
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-07],d3_[00-15],d2_[00-03] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 3
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-11],d3_[00-23],d2_[00-05] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 4
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-15],d3_[00-31],d2_[00-07] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 5
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-19],d3_[00-39],d2_[00-09] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 6
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-23],d3_[00-47],d2_[00-11] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 7
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-27],d3_[00-55],d2_[00-13] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-33],d3_[00-63],d2_[00-15] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
