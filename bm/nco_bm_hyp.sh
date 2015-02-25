# $Header$

# Purpose: Perform NCO benchmarks while subsetting 

# Viable approaches to scale datasets include:
# Approach 1: Scale by sub-setting
# Approach 1: Process 1/8, 2/8, ... 8/8 of the variables ("subsetting")
# Approach 1 keeps rank and shape of variables constant, and extends number processed
# Approach 2: Scale by hyperslabbing
# Approach 2: Process 1/8, 2/8, ... 8/8 of the data in each variable ("hyperslabbing")
# Approach 2 keeps rank and number of variables constant, and fattens processed shape

# Usage:
# ~/nco/bm/nco_bm_hyp.sh

# Copy code to test machines:
# scp ~/nco/bm/nco_bm_hyp.sh clay.ess.uci.edu:nco/bm
# scp ~/nco/src/nco/ncwa.c ~/nco/src/nco/nco_ctl.c ~/nco/src/nco/nco_var_avg.c ~/nco/src/nco/nco_cnf_dmn.c clay.ess.uci.edu:nco/src/nco
# cd ~/nco/bld;make allinone;cd ~

CCH_CLR_FLG='1' # [flg] Clear cache before each command
FGR_03_FLG='0' # Figure 3 is the ncbo Satellite timing figure
FGR_04_FLG='1' # Figure 4 is the ncwa IPCC timing figure
FGR_05_OLD_FLG='0' # FGR_05_OLD is the bar chart for ncwa
SCL_HYP_SLB_FLG='0' # [flg] Use approach 2: Scale by hyperslabbing
SCL_SUB_SET_FLG='1' # [flg] Use approach 1: Scale by sub-setting
WGT_FLG='1' # [flg] Perform weighted averages

if [ ${WGT_FLG} = '1' ]; then
    WGT_CMD='-w lat'
else
    WGT_CMD=''
fi # !WGT_FLG

if [ ${CCH_CLR_FLG} = '1' ]; then
    CCH_CMD="${HOME}/nco/bm/cch_clr.pl"
else
    CCH_CMD=''
fi # !CCH_CLR_FLG

# Figure 3 is the ncbo timing figure
if [ ${FGR_03_FLG} = '1' -a ${SCL_SUB_SET_FLG} = '1' ]; then
# Approach 1: Scale by sub-setting
    for var_nbr in {0..7}; do
	var_nbr_sng=`printf "%02d" ${var_nbr}`
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -v "d2_0[0-${var_nbr}]" -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_${var_nbr_sng}.out
    done # end loop over var_nbr
fi # !${SCL_HYP_SLB_FLG}

if [ ${FGR_03_FLG} = '1' -a ${SCL_HYP_SLB_FLG} = '1' ]; then
# Approach 2: Scale by hyperslabbing
# 1/8
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -d lat,0,0800 -d lon,0,1166 -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_01.out
# 2/8
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -d lat,0,1000 -d lon,0,2332 -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_02.out
# 3/8
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -d lat,0,1200 -d lon,0,2916 -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_03.out
# 4/8
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -d lat,0,1400 -d lon,0,3333 -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_04.out
# 5/8
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -d lat,0,1600 -d lon,0,3645 -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_05.out
# 6/8
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -d lat,0,1800 -d lon,0,3888 -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_06.out
# 7/8
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -d lat,0,2000 -d lon,0,4082 -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_07.out
# 8/8
	${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncbo.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncbo -O --mdl -p ${DATA}/nco_bm stl_5km.nc stl_5km.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_08.out
fi # !${SCL_HYP_SLB_FLG}

if [ ${FGR_04_FLG} = '1' -a ${SCL_SUB_SET_FLG} = '1' ]; then
# Approach 1: Scale by sub-setting
# 1/8
    ${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	${MY_BIN_DIR}/ncwa -O --mdl ${WGT_CMD} -v 'd4_0[0-3],d3_0[0-7],d2_0[0-1],d1_0[0-0]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_01.out
# 2/8
    ${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	${MY_BIN_DIR}/ncwa -O --mdl ${WGT_CMD} -v 'd4_0[0-7],d3_0[0-9],d3_1[0-5],d2_0[0-3],d1_0[0-1]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_02.out
# 3/8
    ${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	${MY_BIN_DIR}/ncwa -O --mdl ${WGT_CMD} -v 'd4_0[0-9],d4_1[0-1],d3_[0-1][0-9],d3_2[0-3],d2_0[0-5],d1_0[0-2]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_03.out
# 4/8
    ${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	${MY_BIN_DIR}/ncwa -O --mdl ${WGT_CMD} -v 'd4_0[0-9],d4_1[0-5],d3_[0-2][0-9],d3_3[0-1],d2_0[0-7],d1_0[0-3]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_04.out
# 5/8
    ${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	${MY_BIN_DIR}/ncwa -O --mdl ${WGT_CMD} -v 'd4_[0-1][0-9],d3_[0-3][0-9],d2_0[0-9],d1_0[0-4]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_05.out
# 6/8
    ${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	${MY_BIN_DIR}/ncwa -O --mdl ${WGT_CMD} -v 'd4_[0-1][0-9],d4_2[0-3],d3_[0-3][0-9],d3_4[0-7],d2_0[0-9],d2_1[0-1],d1_0[0-5]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_06.out
# 7/8
    ${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	${MY_BIN_DIR}/ncwa -O --mdl ${WGT_CMD} -v 'd4_[0-1][0-9],d4_2[0-7],d3_[0-4][0-9],d3_5[0-5],d2_0[0-9],d2_1[0-3],d1_0[0-6]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_07.out
# 8/8
    ${CCH_CMD}; /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	${MY_BIN_DIR}/ncwa -O --mdl ${WGT_CMD} -v 'd4_[0-2][0-9],d4_3[0-1],d3_[0-5][0-9],d3_6[0-3],d2_0[0-9],d2_1[0-5],d1_0[0-7]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_08.out
fi # !${SCL_SUB_SET_FLG}

if [ ${FGR_04_FLG} = '1' -a ${SCL_HYP_SLB_FLG} = '1' ]; then
# Approach 2: Scale by hyperslabbing
# Two-dimensional and scalar variables do not have time dimension
# Error incurred by including small non-record variables is minimal
# Exclude 2D variables with -x -v d2_[0-9][0-9]
# NB: Scaling by hyperslab uses different disk geometry than sub-setting
    for fl_nbr in {01..08}; do
	fl_nbr_sng=`printf "%02d" ${fl_nbr}`
	${CCH_CMD}
	/usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
	    ${MY_BIN_DIR}/ncwa -O -F --mdl ${WGT_CMD} -d time,1,${fl_nbr} -x -v 'd2_[0-9][0-9]' -p ${DATA}/nco_bm ipcc_dly_T85.nc ${DATA}/nco_bm/foo.nc > ${DATA}/nco_bm/foo_${fl_nbr_sng}.out
	${CCH_CMD}
    done # end loop over fl_nbr
fi # !${SCL_HYP_SLB_FLG}

# This FGR_05_OLD refers to the old Figure 5 which had the bar chart for ncwa
if [ ${FGR_05_OLD_FLG} = '1' ]; then
    /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
    ${MY_BIN_DIR}/ncwa -O -a lat --mdl -w lat -p ${DATA}/nco_bm stl_5km.nc ${DATA}/nco_bm/foo_stl_lat.nc > ${DATA}/nco_bm/foo_stl_lat.out
    /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
    ${MY_BIN_DIR}/ncwa -O -a lon --mdl -w lat -p ${DATA}/nco_bm stl_5km.nc ${DATA}/nco_bm/foo_stl_lon.nc > ${DATA}/nco_bm/foo_stl_lon.out
    /usr/bin/time -a -o ~/nco/bm/ncwa.timing -f "%e wall %s system %U user " \
    ${MY_BIN_DIR}/ncwa -O -a lat,lon --mdl -w lat -p ${DATA}/nco_bm stl_5km.nc ${DATA}/nco_bm/foo_stl_all.nc > ${DATA}/nco_bm/foo_stl_all.out
fi # !${FGR_05_OLD_FLG}
