#!/bin/bash

# Minimal standalone reproducer for ncremap:
# /home/ac.zender/bin_chrysalis/ncremap: line 3831: ppp_pid[${fl_idx}]: unbound variable

# Usage:
# sbatch --output=${HOME}/e3smu_dbg.o%j --mail-type=fail,end --mail-user=zender@uci.edu ~/nco/data/e3smu_dbg.sh
# m ~/e3smu_dbg.o*

#SBATCH --job-name=e3smu_dbg.sh
#SBATCH --account=e3sm
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=00:10:00
#SBATCH --partition=debug

source /home/ac.forsyth2/miniforge3/etc/profile.d/conda.sh
conda activate test-e3sm-to-cmip-master-20260928_run2

NCREMAP=/home/ac.zender/bin_chrysalis/ncremap
NCCLIMO=/home/ac.zender/bin_chrysalis/ncclimo
VRT_MAP=/lcrc/group/e3sm/diagnostics/e3sm_to_cmip_data/maps/vrt_remap_plev19.nc
SRC_FILE=/lcrc/group/e3sm/ac.forsyth2/zppy_weekly_comprehensive_v3_output/zppy_main_branch_test_20260928_run2/v3.LR.historical_0051/post/atm/180x360_aave/ts/monthly/2yr/U_198501_198612.nc

WORKDIR=$(mktemp -d)
cd "${WORKDIR}" || exit 1
cp -s "${SRC_FILE}" ./test.nc

echo "Running: ${NCREMAP} --npo -p mpi --vrt_ntp=log --vrt_xtr=mss_val --vrt_out=${VRT_MAP} test.nc test.nc.plev"
"${NCREMAP}" --npo -p mpi --vrt_ntp=log --vrt_xtr=mss_val --vrt_out="${VRT_MAP}" test.nc test.nc.plev
echo "Exit code: $?"

if [[ -z ${HOSTNAME:-} ]]; then
    if [[ -f /bin/hostname ]] && [[ -x /bin/hostname ]]; then
	export HOSTNAME=$(/bin/hostname)
    elif [[ -f /usr/bin/hostname ]] && [[ -x /usr/bin/hostname ]]; then
	export HOSTNAME=$(/usr/bin/hostname)
    fi # !hostname
fi # HOSTNAME
# Default input and output directory is ${DATA}
if [[ -z ${DATA:-} ]]; then
    case "${HOSTNAME:-}" in 
	andes* ) DATA="/gpfs/alpine/world-shared/cli115/${USER}" ; ;; # OLCF andes compute nodes named andesNNN, 256 GB/node
	bebop* | blogin* | b[0123456789][0123456789][0123456789] ) DATA="/lcrc/group/e3sm/${USER}" ; ;; # ANL/LCRC bebop compute nodes named bNNN, 36|64 cores|GB/node 
	chrysalis* | chrlogin* | chr-[0123456789][0123456789][0123456789][0123456789] ) DATA="/lcrc/group/e3sm/${USER}" ; ;; # ANL/LCRC chrysalis compute nodes named chr-NNNN, 64|256 cores|GB/node 
	compy* | n[0123456789][0123456789][0123456789][0123456789] ) DATA="/qfs/people/${USER}/data" ; ;; # PNNL compy compute nodes all named nNNNN, 40|192 cores|GB/node (compy login nodes also 192 GB)
	constance* | node* ) DATA='/scratch' ; ;; # PNNL
	derecho* ) DATA="/glade/p/work/${USER}" ; ;; # NCAR derecho compute nodes named, e.g., r8i0n8, r5i3n16, r12i5n29 ... 18|(64/256) cores|GB/node (derecho login nodes 512 GB)
	frontier* ) DATA="/lustre/orion/cli115/world-shared/${USER}" ; ;; # OLCF frontier compute nodes named frontier01276 64|512 cores|GB/node
	ilogin* | i[0123456789][0123456789][0123456789] ) DATA="/lcrc/group/e3sm/${USER}" ; ;; # ANL/LCRC improv compute nodes named iNNN, 128|256 cores|GB/node 
	login[0123456789][0123456789] ) # 20230831 Frontier and Perlmutter login nodes share this name :(
	    if [[ ${LMOD_SYSTEM_NAME:-} == 'frontier' ]]; then
		DATA="/lustre/orion/cli115/world-shared/${USER}"
	    elif [[ ${LMOD_SYSTEM_NAME:-} == 'perlmutter' ]]; then
		DATA="${SCRATCH}"
	    fi # !LMOD_SYSTEM_NAME
	    ;; # !login	
	perlmutter* | nid[0123456789][0123456789][0123456789][0123456789][0123456789][0123456789] ) DATA="${SCRATCH}" ; ;; # NERSC perlmutter compute nodes named nidNNNNNN (CPU) with (128)|(512) cores|GB/node (cpu) (login nodes 512 GB)
	* ) DATA='/tmp' ; ;; # Other
    esac # !HOSTNAME
fi # DATA

# Climos, compression, and regridding
ncclimo -7 --bfr=134217728 --blk=4098 --cmp='gbr|shf|zst' -P eam -v FSNT,AODVIS,TREFHT -c v2.LR.historical_0101 -s 2013 -e 2014 -i ${DATA}/ne30/raw -o ${DATA}/ne30/clm -O ${DATA}/ne30/rgr -r ${DATA}/maps/map_ne30pg2_to_cmip6_180x360_nco.20200901.nc

# Timeseries and vertical interpolation
cd ${DATA}/ne30/raw;ls v3.LR.piControl.eam*046[01]-??*.nc | ncclimo --split --dbg=1 -s 1 -e 2 --var=T --vrt_out=${DATA}/grids/vrt_prs_ncep_L17.nc --vrt_xtr=mss_val --drc_out=${DATA}/ne30/clm # Missing value interpolation

# Simultaneous horizontal/vertical regridding L72->L30
ncremap -v lat,lon,FSNT,AODVIS,T,Q,U,V,Z3 --map=${DATA}/maps/map_ne30pg2_to_cmip6_180x360_aave.20200201.nc --vrt_out=${DATA}/grids/vrt_hyb_L30.nc ${DATA}/bm/eamv3_ne30pg2l80.nc ~/foo.nc
