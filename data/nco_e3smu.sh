#!/bin/bash

# E3SM-Unified standalone tests for ncremap and ncclimo
# Works on any machine with Zender's default testing files

#SBATCH --job-name=nco_e3smu
#SBATCH --account=e3sm
#SBATCH --nodes=1
#SBATCH --exclusive
#SBATCH --time=00:10:00
#SBATCH --partition=debug

# Usage:
# Invoke machine-specific SBATCH parameters on command line, and the rest as shell comments
if false; then
    # Chrysalis:
    /bin/rm ~/nco_e3smu.o*; sbatch --output=${HOME}/nco_e3smu.o%j --mail-type=fail,end --mail-user=zender@uci.edu ~/nco/data/nco_e3smu.sh
    # Maluhia
    /bin/rm ~/nco_e3smu.o*; ~/nco/data/nco_e3smu.sh > ~/nco_e3smu.o$! 2>&1
    # Perlmutter:
    /bin/rm ~/nco_e3smu.o*; sbatch --output=${HOME}/nco_e3smu.o%j --mail-type=fail,end --mail-user=zender@uci.edu --constraint=cpu ~/nco/data/nco_e3smu.sh
    m ~/nco_e3smu.o*
fi # !false

if [[ -z ${HOSTNAME:-} ]]; then
    if [[ -f /bin/hostname ]] && [[ -x /bin/hostname ]]; then
	export HOSTNAME=$(/bin/hostname)
    elif [[ -f /usr/bin/hostname ]] && [[ -x /usr/bin/hostname ]]; then
	export HOSTNAME=$(/usr/bin/hostname)
    fi # !hostname
fi # HOSTNAME

# Disambiguate to find effective host name = HOST_FFC
# HOST_FFC is simply the common machine name instead of login or compute node name
HOST_FFC="${HOSTNAME}"
case "${HOST_FFC}" in
    andes* ) HOST_FFC='andes' ; ;; # OLCF andes compute nodes named andesNNN, 256 GB/node
    bebop* | blogin* | b[0123456789][0123456789][0123456789] ) HOST_FFC='bebop' ; ;; # ALCF LCRC bebop compute nodes named bNNN, 16|64 cores|GB/node 
    chrlogin* | chr-[0123456789][0123456789][0123456789][0123456789] | ilogin* | i[0123456789][0123456789][0123456789] ) HOST_FFC='chrysalis' ; ;; # ANL LCRC
    compy* | n[0123456789][0123456789][0123456789][0123456789] ) HOST_FFC='compy' ; ;; # PNNL compy compute nodes all named nNNNN, 40|192 cores|GB/node (compy login nodes also 192 GB)
    derecho* ) HOST_FFC='derecho' ; ;; # NCAR derecho compute nodes named, e.g., r8i0n8, r5i3n16, r12i5n29 ... 18|(64/256) cores|GB/node (derecho login nodes 512 GB)
    frontier* ) HOST_FFC='frontier' ; ;; # OLCF frontier compute nodes named frontier01276 64|512 cores|GB/node
    login-* ) HOST_FFC='hpc' ; ;; # UCI RCIC HPC3
    login[0123456789][0123456789] ) # 20230831 Frontier and Perlmutter login nodes share this name :(
	if [ "${LMOD_SYSTEM_NAME}" = 'frontier' ]; then
	    HOST_FFC='frontier'
	elif [ "${LMOD_SYSTEM_NAME}" = 'perlmutter' ]; then
	    HOST_FFC='perlmutter'
	fi # !LMOD_SYSTEM_NAME
	;; # !login	
    perlmutter* | nid[0123456789][0123456789][0123456789][0123456789][0123456789][0123456789] ) HOST_FFC='perlmutter' ; ;; # NERSC
esac # !${HOST_FFC}
export HOST_FFC

# Default input and output directory root is ${DATA}
case "${HOST_FFC:-}" in 
    andes* | frontier* ) DATA="/lustre/orion/cli115/world-shared/zender" ; CSZ_BIN_DIR="~zender/bin_andes" ; ;;
    bebop* ) DATA="/lcrc/group/e3sm/ac.zender/data" ; CSZ_BIN_DIR="/home/ac.zender/bin" ; ;;
    chrysalis* ) DATA="/home/ac.zender/data" ; CSZ_BIN_DIR="/home/ac.zender/bin_chrysalis" ; ;;
    compy* ) DATA="/qfs/people/zender/data" ; CSZ_BIN_DIR="/qfs/people/zender/bin" ; ;;
    derecho* ) DATA="/glade/work/zender" ; CSZ_BIN_DIR="/glade/home/zender/bin" ; ;;
    e3sm* ) DATA="/home/zender/data" ; CSZ_BIN_DIR="/home/zender/bin" ; ;;
    frontier* ) DATA="/lustre/orion/cli115/world-shared/zender" ; CSZ_BIN_DIR="~zender/bin_frontier" ; ;;
    ilogin* ) DATA="/home/ac.zender/data" ; CSZ_BIN_DIR="/home/ac.zender/bin_chrysalis" ; ;;
    imua* ) DATA="/home/zender/data" ; CSZ_BIN_DIR="/Users/home/zender/bin" ; ;;
    maluhia* ) DATA="/Users/zender/data" ; CSZ_BIN_DIR="/Users/zender/bin" ; ;;
    spectral* ) DATA="/Users/zender/data" ; CSZ_BIN_DIR="/Users/zender/bin" ; ;;
    perlmutter* ) DATA="/global/cfs/cdirs/e3sm/zender" ; CSZ_BIN_DIR="/global/cfs/cdirs/e3sm/zender/bin_perlmutter" ; ;;
    * ) DATA="/home/zender/data" ; CSZ_BIN_DIR="/home/zender/bin" ; ;; # default
esac # !${HOST_FFC}

# Set Conda environment
case "${HOST_FFC:-}" in 
    andes* | frontier* ) source /ccs/proj/cli115/software/e3sm-unified/load_latest_e3sm_unified.sh ; ;;
    bebop* ) source /lcrc/soft/climate/e3sm-unified/load_latest_e3sm_unified.sh ; ;;
#    chrysalis* ) source /home/ac.forsyth2/miniforge3/etc/profile.d/conda.sh ; conda activate test-e3sm-to-cmip-master-20260928_run2 ; ;; # Ryan's development path
    chrysalis* ) source /lcrc/soft/climate/e3sm-unified/load_latest_e3sm_unified.sh ; ;;
    compy* ) source /share/apps/E3SM/conda_envs/load_latest_e3sm_unified.sh ; ;;
    derecho* ) source fxm/load_latest_e3sm_unified.sh ; ;;
    e3sm* ) echo "No E3SM-Unified environment specified for ${HOST_FFC}" ; ;;
    frontier* ) source /load_latest_e3sm_unified.sh ; ;;
    ilogin* ) source fxm/load_latest_e3sm_unified.sh ; ;;
    imua* ) echo "No E3SM-Unified environment specified for ${HOST_FFC}" ; ;;
    maluhia* ) echo "No E3SM-Unified environment specified for ${HOST_FFC}" ; ;;
    spectral* ) echo "No E3SM-Unified environment specified for ${HOST_FFC}" ; ;;
    perlmutter* ) source /global/common/software/e3sm/anaconda_envs/load_latest_e3sm_unified.sh ; ;;
    * ) echo "No E3SM-Unified environment specified for ${HOST_FFC}" ; ;; # default
esac # !${HOST_FFC}

# Use scripts from my latest snapshot (not from Conda-Forge)
# Invoke scripts with --npo to access my latest binaries as well
NCREMAP="${CSZ_BIN_DIR}/ncremap --npo"
NCCLIMO="${CSZ_BIN_DIR}/ncclimo --npo"

# Work in temporary directory to reduce unecessary bloat
WORKDIR=$(mktemp -d)
cd "${WORKDIR}" || exit 1

printf "\nTest climos and regridding...\n"
${NCCLIMO} -P eam -v FSNT,AODVIS,TREFHT -c v3.LR.piControl -s 460 -e 461 -i ${DATA}/ne30/raw -o ${WORKDIR}/ne30/clm -O ${WORKDIR}/ne30/rgr -r ${DATA}/maps/map_ne30pg2_to_cmip6_180x360_traave.20231201.nc
printf "\nExit code: $?\n"

printf "\nTest timeseries and vertical interpolation...\n"
cd ${DATA}/ne30/raw;ls v3.LR.piControl.eam*046[01]-??*.nc | ${NCCLIMO} --split --dbg=1 -s 460 -e 461 --var=T --vrt_out=${DATA}/grids/vrt_prs_ncep_L17.nc --vrt_xtr=mss_val --drc_out=${WORKDIR}/ne30/clm # Missing value interpolation
printf "\nExit code: $?\n"

printf "\nTest simultaneous horizontal/vertical regridding L72->L30...\n"
${NCREMAP} --vrb=3 -v lat,lon,FSNT,AODVIS,T,Q,U,V,Z3 --map=${DATA}/maps/map_ne30pg2_to_cmip6_180x360_aave.20200201.nc --vrt_out=${DATA}/grids/vrt_hyb_L30.nc ${DATA}/bm/eamv3_ne30pg2l80.nc ${WORKDIR}/foo.nc
printf "\nExit code: $?\n"

printf "\nTest flexible months...\n"
${NCCLIMO} -v FSNT -c v3.LR.piControl -s 460 -e 461 --mth_srt=10 -i ${DATA}/ne30/raw -o ${WORKDIR}/ne30/clm

printf "\nCleaning up...\n"
/bin/rm -r ${WORKDIR}

