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
    /bin/rm ~/nco_e3smu.o*; ~/nco/data/nco_e3smu.sh > ~/nco_e3smu.o$! 2>&1 # Default
    /bin/rm ~/nco_e3smu.o*; ~/nco/data/nco_e3smu.sh --no_e3smu --no_npo > ~/nco_e3smu.o$! 2>&1 # 
    # Perlmutter:
    /bin/rm ~/nco_e3smu.o*; sbatch --output=${HOME}/nco_e3smu.o%j --mail-type=fail,end --mail-user=zender@uci.edu --constraint=cpu ~/nco/data/nco_e3smu.sh
    m ~/nco_e3smu.o*
fi # !false

# Safety: Exit immediately when any command returns non-zero exit status, uninitialized variables are used, or pipes fail
set -u # Exit on unbound variable reference

function exit_gracefully {
    local exit_code=${1:-0} # Default to exit code 0 if not specified
    
    if [[ "${BASH_SOURCE[0]}" != "${0}" ]]; then
	# If script was invoked via "source ${spt_nm}" then return() so as not to exit() current shell
        return "$exit_code"
    else
	# If script was invoked via "${spt_nm}" then exit() current sub-shell to parent shell
        exit "$exit_code"
    fi
} # !exit_gracefully()

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
    andes* | frontier* ) DATA="/lustre/orion/cli115/world-shared/zender" ; MY_BIN_DIR="~zender/bin_andes" ; ;;
    bebop* ) DATA="/lcrc/group/e3sm/ac.zender/data" ; MY_BIN_DIR="/home/ac.zender/bin" ; ;;
    chrysalis* ) DATA="/home/ac.zender/data" ; MY_BIN_DIR="/home/ac.zender/bin_chrysalis" ; ;;
    compy* ) DATA="/qfs/people/zender/data" ; MY_BIN_DIR="/qfs/people/zender/bin" ; ;;
    derecho* ) DATA="/glade/work/zender" ; MY_BIN_DIR="/glade/home/zender/bin" ; ;;
    e3sm* ) DATA="/home/zender/data" ; MY_BIN_DIR="/home/zender/bin" ; ;;
    frontier* ) DATA="/lustre/orion/cli115/world-shared/zender" ; MY_BIN_DIR="~zender/bin_frontier" ; ;;
    ilogin* ) DATA="/home/ac.zender/data" ; MY_BIN_DIR="/home/ac.zender/bin_chrysalis" ; ;;
    imua* ) DATA="/home/zender/data" ; MY_BIN_DIR="/Users/home/zender/bin" ; ;;
    maluhia* ) DATA="/Users/zender/data" ; MY_BIN_DIR="/Users/zender/bin" ; ;;
    spectral* ) DATA="/Users/zender/data" ; MY_BIN_DIR="/Users/zender/bin" ; ;;
    perlmutter* ) DATA="/global/cfs/cdirs/e3sm/zender" ; MY_BIN_DIR="/global/cfs/cdirs/e3sm/zender/bin_perlmutter" ; ;;
    * ) DATA="/home/zender/data" ; MY_BIN_DIR="/home/zender/bin" ; ;; # default
esac # !${HOST_FFC}

# Defaults for command-line options and some derived variables
# Modify these defaults to save typing later
dbg_lvl=0 # [nbr] Debugging level
e3smu_flg='Yes' # [sng] Load E3SM-Unified
fl_fmt='' # [enm] Output file format
hyd_flg='No' # [sng] Hide CSZ executables
npo_flg='Yes' # [sng] Set --npo (NCO-Path-Override) option
npo_opt='' # [sng] Set --npo (NCO-Path-Override) option

while getopts :34567D:n:-: OPT; do
    case ${OPT} in
	3) fl_fmt='3' ;; # File format
	4) fl_fmt='4' ;; # File format
	5) fl_fmt='5' ;; # File format
	6) fl_fmt='6' ;; # File format
	7) fl_fmt='7' ;; # File format
	D) dbg_lvl=$((10#${OPTARG})) ;; # Debugging level
	e) e3smu_flg='Yes' ;; # [sng] Load E3SM-Unified
	h) hyd_flg='Yes' ;; # [sng] Hide CSZ executable
	n) npo_flg='Yes' ;; # [sng] Set --npo (NCO-Path-Override) option
	-) LONG_OPTARG="${OPTARG#*=}"
	   case ${OPTARG} in
	       # Hereafter ${OPTARG} is long argument key, and ${LONG_OPTARG}, if any, is long argument value
	       # Long options with no argument, no short option counterpart
	       # Long options with argument, no short option counterpart
	       # Long options with short counterparts, ordered by short option key
	       dbg_lvl=?* | dbg=?* | debug=?* | debug_level=?* ) dbg_lvl=$((10#${LONG_OPTARG})) ;; # -d # Debugging level
	       fl_fmt=?* | fmt_out=?* | file_format=?* | format_out=?* ) fl_fmt="${LONG_OPTARG}" ;; # # Output file format
	       hyd | hide | hide_csz_exe | hyd_flg ) hyd_flg='Yes' ;; # -h # Hide CSZ executable directories
	       hyd=?* | hide | hide_csz_exe | hyd_flg=?* ) echo "No argument allowed for --${OPTARG switch}" >&2; exit_gracefully 1 ;; # -e # Hide CSZ executable directories
	       no_hyd | no_hide | no_hide_csz_exe | no_hyd_flg ) hyd_flg='No' ;; # -h # Do not hide CSZ executables
	       no_hyd=?* | no_hide | no_hide_csz_exe | hyd_flg=?* ) echo "No argument allowed for --${OPTARG switch}" >&2; exit_gracefully 1 ;; # # Do not hide CSZ executables
	       e3smu | e3smu_flg ) e3smu_flg='Yes' ;; # -e # Load E3SM-Unified
	       e3smu=?* | e3smu_flg=?* ) echo "No argument allowed for --${OPTARG switch}" >&2; exit_gracefully 1 ;; # -e # Load E3SM-Unified
	       no_e3smu | no_e3smu_flg ) e3smu_flg='No' ;; # # No not load E3SM-Unified
	       no_e3smu=?* | no_e3smu_flg=?* ) echo "No argument allowed for --${OPTARG switch}" >&2; exit_gracefully 1 ;; # # Do not load E3SM-Unified
	       no_npo | no_npo_flg ) npo_flg='No' ;; # -n # Do not set --npo (NCO-Path-Override) option
	       no_npo=?* | no_npo_flg=?* ) echo "No argument allowed for --${OPTARG switch}" >&2; exit_gracefully 1 ;; # -n # Do not set --npo (NCO-Path-Override) option
	       npo | npo_flg ) npo_flg='Yes' ;; # -n # Set --npo (NCO-Path-Override) option
	       npo=?* | npo_flg=?* ) echo "No argument allowed for --${OPTARG switch}" >&2; exit_gracefully 1 ;; # -n # Set --npo (NCO-Path-Override) option
               '' ) break ;; # "--" terminates argument processing
               * ) printf "\nERROR: Unrecognized option ${fnt_bld}--${OPTARG}${fnt_nrm}\n" >&2; fnc_usg_prn ;;
	   esac ;; # !OPTARG
	\?) # Unrecognized option
	    printf "\nERROR: Option ${fnt_bld}-${OPTARG}${fnt_nrm} not recognized\n" >&2
	    fnc_usg_prn ;;
    esac # !OPT
done # !getopts
shift $((OPTIND-1)) # Advance one argument
psn_nbr=$#

printf "nco_e3smu.sh config:\n"

if [[ ${e3smu_flg} == 'Yes' ]]; then
    # Set Conda environment
    case "${HOST_FFC:-}" in 
	andes* ) source /ccs/proj/cli115/software/e3sm-unified/load_latest_e3sm_unified_andes.sh ; ;;
	bebop* ) source /lcrc/soft/climate/e3sm-unified/load_latest_e3sm_unified_chrysalis.sh ; ;;
	#    chrysalis* ) source /lcrc/soft/climate/e3sm-unified/load_latest_e3sm_unified_chrysalis.sh ; ;;
	#    chrysalis* ) source /home/ac.forsyth2/miniforge3/etc/profile.d/conda.sh ; conda activate test-e3sm-to-cmip-master-20260928_run2 ; ;; # Ryan's development path
	chrysalis* ) source /lcrc/soft/climate/e3sm-unified/test_e3sm_unified_1.14.0rc3_chrysalis.sh; ;;
	compy* ) source /share/apps/E3SM/conda_envs/load_latest_e3sm_unified_compy.sh ; ;;
	derecho* ) "E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	e3sm* ) echo "E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	frontier* ) source /load_latest_e3sm_unified_frontier.sh ; ;;
	ilogin* ) source fxm/load_latest_e3sm_unified_chrysalis.sh ; ;;
	imua* ) echo "E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	maluhia* ) echo "E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	spectral* ) echo "E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	#    perlmutter* ) source /global/common/software/e3sm/anaconda_envs/load_latest_e3sm_unified_pm-cpu.sh ; ;;
	perlmutter* ) source /global/common/software/e3sm/anaconda_envs/test_e3sm_unified_1.14.0rc3_pm-cpu.sh ; ;;
	* ) echo "E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;; # default
    esac # !${HOST_FFC}
else # !e3smu_flg
    echo "E3SM-Unified environment not requested"
fi # !e3smu_flg

if [[ ${hyd_flg} == 'Yes' ]]; then
    echo "Removing CSZ binaries..."
    /bin/rm -r -f ${MY_BIN_DIR}_tmp ${MY_LIB_DIR}_tmp;mv ${MY_BIN_DIR} ${MY_BIN_DIR}_tmp;mv ${MY_LIB_DIR} ${MY_LIB_DIR}_tmp # Remove my NCO from PATH
    /bin/rm -r -f anaconda_tmp;mv ~/anaconda ~/anaconda_tmp # Remove my Anaconda from PATH
else # !hyd_flg
    echo "Not hiding CSZ binaries"
fi # !hyd_flg

if [[ ${npo_flg} == 'Yes' ]]; then
    npo_opt='--npo'
    echo "Setting NPO option with ${npo_opt}"
else # !npo_flg
    echo "Not setting NPO option"
fi # !npo_flg

printf "\n"

# Use scripts from my latest snapshot (not from Conda-Forge)
# Invoke scripts with --npo to access my latest binaries as well
NCREMAP="${MY_BIN_DIR}/ncremap ${npo_opt}"
NCCLIMO="${MY_BIN_DIR}/ncclimo ${npo_opt}"

# Work in temporary directory to reduce unecessary bloat
WORKDIR=$(mktemp -d)
cd "${WORKDIR}" || exit 1

${NCREMAP} --config
printf "\nExit code: $?\n"

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
printf "\nExit code: $?\n"

printf "\nTest parallel ESMF map generation...\n"
${NCREMAP} --wgt_opt=' ' --mpi_nbr=4 --alg_typ=esmfaave --grd_src=${DATA}/grids/ne30pg2.nc --grd_dst=${DATA}/grids/cmip6_180x360_scrip.20181001.nc --map=${HOME}/map.nc
printf "\nExit code: $?\n"

printf "\nTest parallel MOAB map generation...\n"
${NCREMAP} --mpi_nbr=4 --alg_typ=traave --grd_src=${DATA}/grids/ne30pg2.nc --grd_dst=${DATA}/grids/cmip6_180x360_scrip.20181001.nc --map=${HOME}/map.nc
printf "\nExit code: $?\n"

printf "\nTest parallel ESMF map generation and regridding...\n"
${NCREMAP} --wgt_opt=' ' --mpi_nbr=4 -v FSNT,AODVIS --dbg=1 --vrb=3 --devnull=No --alg_typ=esmfaave --grd_src=${DATA}/grids/ne30pg2.nc --grd_dst=${DATA}/grids/cmip6_180x360_scrip.20181001.nc --map=${HOME}/map.nc ${DATA}/bm/eamv2_ne30pg2l72.nc ~/foo.nc
printf "\nExit code: $?\n"

printf "\nTest parallel MOAB map generation and regridding...\n"
${NCREMAP} --mpi_nbr=4 -v FSNT,AODVIS --dbg=1 --vrb=3 --devnull=No --alg_typ=traave --grd_src=${DATA}/grids/ne30pg2.nc --grd_dst=${DATA}/grids/cmip6_180x360_scrip.20181001.nc --map=${HOME}/map.nc ${DATA}/bm/eamv2_ne30pg2l72.nc ~/foo.nc
printf "\nExit code: $?\n"

if [[ ${hyd_flg} == 'Yes' ]]; then
    echo "Restoring CSZ binaries..."
    /bin/rm -r -f ${MY_BIN_DIR} ${MY_LIB_DIR};mv ${MY_BIN_DIR}_tmp ${MY_BIN_DIR};mv ${MY_LIB_DIR}_tmp ${MY_LIB_DIR} # Restore my NCO to PATH
    /bin/rm -r -f anaconda;mv ~/anaconda_tmp ~/anaconda # Restore my Anaconda to PATH
fi # !hyd_flg

printf "\nCleaning up...\n"
/bin/rm -r ${WORKDIR}

exit_gracefully 0
