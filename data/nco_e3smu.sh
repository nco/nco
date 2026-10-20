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
    /bin/rm ~/nco_e3smu.o*; ~/nco/data/nco_e3smu.sh --no_e3smu --no_npo > ~/nco_e3smu.o$! 2>&1 # Similar to Default
    # Perlmutter:
    /bin/rm ~/nco_e3smu.o*; sbatch --output=${HOME}/nco_e3smu.o%j --mail-type=fail,end --mail-user=zender@uci.edu --constraint=cpu ~/nco/data/nco_e3smu.sh
    m ~/nco_e3smu.o*
fi # !false

# Safety: Exit immediately when ...
#set -u # Exit on unbound variable reference

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

# Define error handlers
function hnd_err {
    # ERR Error handler. Called after Bash non-zero return code.
    local exit_code=$?
    local line_number=${BASH_LINENO[0]}
    local failed_command="$BASH_COMMAND"
    echo "❌ ABORT DUE TO NON-ZERO COMMAND EXIT CODE" >&2
    echo "Failed Command: '$failed_command'" >&2
    echo "Line Number   : $line_number" >&2
    echo "Exit Status   : $exit_code" >&2
} # !hnd_err()

function hnd_xit {
    # EXIT Error handler. Called after exit().
    # Return terminal to normal state after errors
    local exit_code=$?
    if (( exit_code == 3 )); then
	# exit_code 3 is only used by fnc_usg_prn()
	if [[ -t 1 ]]; then
	    printf "\e[0m" # Normal font
	fi # !TERM
    fi # !exit_code
} # !hnd_xit()

function hnd_int {
    # SIGINT Error handler. Trap set by interrupting with Ctrl-C.
    # Print line where it happened, exit with standard SIGINT code
    local line_number=${1}
    local exit_code=130
    echo "\nInterrupted by Ctrl-C at line ${line_number}" >&2
    echo "Exiting with SIGINT status ${exit_code}" >&2
    exit_gracefully ${exit_code}
} # !hnd_int()

# Register error handlers
trap 'printf "\nInterrupted by Ctrl-C\n"' INT # Call hnd_int() on SIGINT signal, i.e., Ctrl-C
trap hnd_err ERR # Call hnd_err() on Bash ERR pseudosignal, i.e., non-zero return code
trap hnd_xit EXIT # Call hnd_xit() on Bash EXIT pseudosignal, i.e., after exit()

# Set script name, directory, PID, run directory
# Set these before 'module' command which can overwrite ${BASH_SOURCE[0]}
# NB: dash supports $0 syntax, not ${BASH_SOURCE[0]} syntax
# http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
drc_pwd=${PWD} # [sng] Run directory
spt_src="${BASH_SOURCE[0]}"
[[ -z ${spt_src:-} ]] && spt_src="${0}" # Use ${0} when BASH_SOURCE is unavailable (e.g., dash)
while [[ -h ${spt_src:-} ]]; do # Recursively resolve ${spt_src} until file is no longer a symlink
  drc_spt="$( cd -P "$( dirname "${spt_src}" )" && pwd )"
  spt_src="$(readlink "${spt_src}")"
  [[ ${spt_src:-} != /* ]] && spt_src="${drc_spt}/${spt_src}" # If ${spt_src} was relative symlink, resolve it relative to path where symlink file was located
done
cmd_ln="${spt_src} $@"
drc_spt="$( cd -P "$( dirname "${spt_src}" )" && pwd )"
spt_nm=$(basename ${spt_src}) # [sng] Script name (unlike $0, ${BASH_SOURCE[0]} works well with 'source <script>')
spt_pid=$$ # [nbr] Script PID (process ID)
# Fetch parent process command line using cross-platform 'ps' target
prn_cmd=$(ps -p $PPID -o command= 2>/dev/null || ps -p $PPID -o args=)
# Extract target script path (skipping interpreter if present)
prn_pth=$(echo "$prn_cmd" | awk '{print ($1 ~ /sh$/) ? $2 : $1}')
# Isolate pure filename
prn_nm=$(basename -- "$prn_pth")

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

# When running in a terminal window (not in an non-interactive batch queue)...
if [[ ${TERM:-} ]]; then
    # If stdout is connected to a terminal
    if [[ -t 1 ]]; then
	# Set terminal fonts for legibility
	if [[ -x /usr/bin/tput ]]; then
	    fnt_bld=$(tput bold) # Bold
	    fnt_nrm=$(tput sgr0) # Normal
	    fnt_rvr=$(tput smso) # Reverse
	    fnt_tlc=$(tput sitm) # Italic
	else
	    fnt_bld="\e[1m" # Bold
	    fnt_nrm="\e[0m" # Normal
	    fnt_rvr="\e[07m" # Reverse
	    fnt_tlc="\e[3m" # Italic
	fi # !tput
    else # !t
	# Otherwise do not specially format text
	fnt_bld='' # Bold
	fnt_nrm='' # Normal
	fnt_rvr='' # Reverse
	fnt_tlc='' # Italic
    fi # !t
fi # !TERM

# Defaults for command-line options and some derived variables
# Modify these defaults to save typing later
dbg_lvl=0 # [nbr] Debugging level
e3smu_flg='Yes' # [sng] Load E3SM-Unified
fl_fmt='' # [enm] Output file format
hyd_flg='No' # [sng] Hide CSZ executables
npo_flg='Yes' # [sng] Set --npo (NCO-Path-Override) option
npo_opt='' # [sng] Set --npo (NCO-Path-Override) option
wgt_exe_nco='ncks' # [sng] NCO executable (both mesh- and weight-generation)
spt_ncremap='ncremap' # [sng] NCO ncremap script
spt_ncclimo='ncclimo' # [sng] NCO ncclimo script

function fnc_usg_prn { # NB: dash supports fnc_nm (){} syntax, not function fnc_nm{} syntax
    # Print usage
    printf "${fnt_rvr}Basic usage:\n"
    printf "${fnt_nrm} ${fnt_bld}${spt_nm} --map=map.nc in_fl out_fl${fnt_nrm} # Apply existing map from in->out\n"
    echo "Command-line options [long-option synonyms in ${fnt_tlc}italics${fnt_nrm}]:"
    echo "${fnt_rvr}-3${fnt_nrm}          Output file format ${fmt_sng_nc3} [${fnt_tlc}fl_fmt, file_format=classic${fnt_nrm}]"
    echo "${fnt_rvr}-4${fnt_nrm}          Output file format ${fmt_sng_nc4} [${fnt_tlc}fl_fmt, file_format=netcdf4${fnt_nrm}]"
    echo "${fnt_rvr}-5${fnt_nrm}          Output file format ${fmt_sng_nc5} [${fnt_tlc}fl_fmt, file_format=64bit_data${fnt_nrm}]"
    echo "${fnt_rvr}-6${fnt_nrm}          Output file format ${fmt_sng_nc6} [${fnt_tlc}fl_fmt, file_format=64bit_offset${fnt_nrm}]"
    echo "${fnt_rvr}-7${fnt_nrm}          Output file format ${fmt_sng_nc7} [${fnt_tlc}fl_fmt, file_format=netcdf4_classic${fnt_nrm}]"
    echo "${fnt_rvr}-D${fnt_nrm} ${fnt_bld}dbg_lvl${fnt_nrm}  Debug level (default ${fnt_bld}${dbg_lvl}${fnt_nrm}) [${fnt_tlc}dbg_lvl, dbg, debug, debug_level${fnt_nrm}]"
    echo " ${fnt_bld}--no_add_fll${fnt_nrm}  Do not add _FillValue to fields with empty destination cells [${fnt_tlc}no_add_fll, no_add_fill_value, no_fll_mpt, no_fill_empty${fnt_nrm}]"
    printf "\n"
    printf "Examples: ${fnt_bld}${spt_nm} -m ${map_xmp} ${in_xmp} ${out_xmp} ${fnt_nrm}\n"
    printf "\nComplete documentation at http://nco.sf.net/nco.html#${spt_nm}\n\n"
    # Terminal currently has boldface, italic, and reverse fonts
    # Resetting terminal would translate those back into escape code gibberish
    # Use exit code 3 (not reserved by system) so hnd_xit() does not reset screen
    exit_gracefully 3
} # !fnc_usg_prn()

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

printf "nco_e3smu.sh, the NCO regression tester for E3SM-Unified compatibility:\n"

if [[ ${e3smu_flg} == 'Yes' ]]; then
    # Set Conda environment
    case "${HOST_FFC:-}" in 
	andes* ) echo "Config: Loading E3SM-Unified"; source /ccs/proj/cli115/software/e3sm-unified/load_latest_e3sm_unified_andes.sh ; ;;
	bebop* ) echo "Config: Loading E3SM-Unified"; source /lcrc/soft/climate/e3sm-unified/load_latest_e3sm_unified_chrysalis.sh ; ;;
	#    chrysalis* ) source /lcrc/soft/climate/e3sm-unified/load_latest_e3sm_unified_chrysalis.sh ; ;;
	#    chrysalis* ) source /home/ac.forsyth2/miniforge3/etc/profile.d/conda.sh ; conda activate test-e3sm-to-cmip-master-20260928_run2 ; ;; # Ryan's development path
	chrysalis* ) echo "Config: Loading E3SM-Unified"; source /lcrc/soft/climate/e3sm-unified/test_e3sm_unified_1.14.0rc4_chrysalis.sh; ;;
	compy* ) echo "Config: Loading E3SM-Unified"; source /share/apps/E3SM/conda_envs/load_latest_e3sm_unified_compy.sh ; ;;
	derecho* ) "Config: E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	e3sm* ) echo "Config: E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	frontier* ) echo "Config: Loading E3SM-Unified"; source /load_latest_e3sm_unified_frontier.sh ; ;;
	ilogin* ) echo "Config: Loading E3SM-Unified"; source fxm/load_latest_e3sm_unified_chrysalis.sh ; ;;
	imua* ) echo "Config: E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	maluhia* ) echo "Config: E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	spectral* ) echo "Config: E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;;
	#    perlmutter* ) echo "Config: Loading E3SM-Unified"; source /global/common/software/e3sm/anaconda_envs/load_latest_e3sm_unified_pm-cpu.sh ; ;;
	perlmutter* ) echo "Config: Loading E3SM-Unified"; source /global/common/software/e3sm/anaconda_envs/test_e3sm_unified_1.14.0rc4_pm-cpu.sh ; ;;
	* ) echo "Config: E3SM-Unified environment requested but unavailable on ${HOST_FFC}" ; ;; # default
    esac # !${HOST_FFC}
else # !e3smu_flg
    echo "Config: E3SM-Unified environment not requested"
fi # !e3smu_flg

if [[ ${hyd_flg} == 'Yes' ]]; then
    echo "Config: Hiding CSZ binaries"
    /bin/rm -r -f ${MY_BIN_DIR}_tmp ${MY_LIB_DIR}_tmp;mv ${MY_BIN_DIR} ${MY_BIN_DIR}_tmp;mv ${MY_LIB_DIR} ${MY_LIB_DIR}_tmp # Remove my NCO from PATH
    /bin/rm -r -f anaconda_tmp;mv ~/anaconda ~/anaconda_tmp # Remove my Anaconda from PATH
else # !hyd_flg
    echo "Config: Not hiding CSZ binaries"
fi # !hyd_flg

if [[ ${npo_flg} == 'Yes' ]]; then
    npo_opt='--npo'
    echo "Config: Setting NPO option with ${npo_opt}"
else # !npo_flg
    echo "Config: Not setting NPO option"
fi # !npo_flg

cmd_nco_exe=$(command -v "${wgt_exe_nco}" 2>/dev/null || true)
cmd_ncremap=$(command -v "${spt_ncremap}" 2>/dev/null || true)
cmd_ncclimo=$(command -v "${spt_ncclimo}" 2>/dev/null || true)
printf "Config: Script & binary executables availability:\n"
if [[ ${cmd_nco_exe:-} ]]; then
    printf "Config: NCO binary executable command ${wgt_exe_nco} found as ${cmd_nco_exe}\n"
else
    printf "Config: NCO binary executable command ${wgt_exe_nco} not found\n"
fi # !err
if [[ ${cmd_ncclimo:-} ]]; then
    printf "Config: NCO script executable command ${spt_ncclimo} found as ${cmd_ncclimo}\n"
else
    printf "Config: NCO binary executable command ${spt_ncclimo} not found\n"
fi # !err
if [[ ${cmd_ncremap:-} ]]; then
    printf "Config: NCO script executable command ${spt_ncremap} found as ${cmd_ncremap}\n"
else
    printf "Config: NCO binary executable command ${spt_ncremap} not found\n"
fi # !err

printf "\n"

# Use scripts from my latest snapshot (not from Conda-Forge)
# Invoke scripts with --npo to access my latest binaries as well
NCCLIMO="${spt_ncclimo} ${npo_opt}"
NCREMAP="${spt_ncremap} ${npo_opt}"

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
