# $Header: /data/zender/nco_20150216/nco/bm/nco_bm_hyp.sh,v 1.6 2006-05-26 20:15:46 zender Exp $

# Purpose: Perform NCO benchmarks while subsetting 
# Approach 1: Process 1/8, 2/8, ... 8/8 of the variables ("subsetting")
# Approach 1 keeps rank and shape of variables constant, and extends number processed
# Approach 2: Process 1/8, 2/8, ... 8/8 of the data in each variable ("hyperslabbing")
# Approach 2 keeps rank and number of variables constant, and fattens processed shape

# Usage:
# ~/nco/bm/nco_bm_hyp.sh

# Implement Approach 1:
# 1/8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-03],d3_[00-07],d2_[00-01] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 2/8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-07],d3_[00-15],d2_[00-03] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 3/8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-11],d3_[00-23],d2_[00-05] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 4/8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-15],d3_[00-31],d2_[00-07] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 5/8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-19],d3_[00-39],d2_[00-09] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 6/8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-23],d3_[00-47],d2_[00-11] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 7/8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-27],d3_[00-55],d2_[00-13] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
# 8/8
~/nco/bm/cch_clr.pl; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
${MY_BIN_DIR}/ncwa --mdl -h -O -w lat -v d4_[00-33],d3_[00-63],d2_[00-15] -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc
