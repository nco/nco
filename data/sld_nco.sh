#!/bin/bash

# Purpose: Regridding script
# This script regrids all input files (possibly on different grids) to a single specified output grid
# Script runs in one of two modes, free-will or predestination:
# A. Free-will mode infers source grid from input file, creates mapfile using that source plus supplied destination grid, applies mapfile, (by default) deletes intermediate source grid and mapfile, proceeds to next input file
# B. Predestination mode applies supplied mapfile to all input files
# Use free-will mode to process Swath-Like-Data (SLD) where each input may be a granule on a new grid, yet all inputs are to be regridded to the same output grid
# Use predestination mode to post-process models or analyses where all files are converted from the same source grid to the same destination grid

# Author: C. Zender
# Created: 20150909

# Source: https://github.com/nco/nco/tree/master/data/sld_nco.sh

# Prerequisites: Bash, NCO, ESMF_RegridWeightGen
# Script could use other shells, e.g., dash (Debian default) after rewriting function definition and looping constructs

# Additional Documentation:

# Configure paths at High-Performance Computer Centers (HPCCs) based on ${HOSTNAME}
if [ -z "${HOSTNAME}" ]; then
    if [ -f /bin/hostname ] && [ -x /bin/hostname ] ; then
	export HOSTNAME=`/bin/hostname`
    elif [ -f /usr/bin/hostname ] && [ -x /usr/bin/hostname ] ; then
	export HOSTNAME=`/usr/bin/hostname`
    fi # !hostname
fi # HOSTNAME
# Default input and output directory is ${DATA}
if [ -z "${DATA}" ]; then
    case "${HOSTNAME}" in 
	cooley* | cc* ) DATA="/projects/HiRes_EarthSys/${USER}" ; ;; # ALCF cooley compute nodes named ccNNN
	edison* | hopper* | nid* ) DATA="${SCRATCH}" ; ;; # NERSC edison compute nodes named nidNNNNN
	pileus* ) DATA="/lustre/atlas/proj-shared/cli115/${USER}" ; ;; # OLCF CADES
	rhea* ) DATA="/lustre/atlas/proj-shared/cli115/${USER}" ; ;; # OLCF rhea compute nodes named rheaNNN
	* ) DATA='/tmp' ; ;; # Other
    esac # !HOSTNAME
fi # DATA

# Production usage:
# chmod a+x ~/sld_nco.sh
# sld_nco.sh -s AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc -o ${DATA}/sld/rgr
# sld_nco.sh -s AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc -i ${DATA}/sld/raw -o ${DATA}/sld/rgr
# sld_nco.sh -s ${DATA}/sld/raw/AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc -o ${DATA}/sld/rgr
# sld_nco.sh -x TSurfStd_ct -s ${DATA}/sld/raw/AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc -o ${DATA}/sld/rgr

# Debugging and Benchmarking:
# sld_nco.sh > ~/sld_nco.txt 2>&1 &
# sld_nco.sh -s AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc -i ${DATA}/sld/raw -o ${DATA}/sld/rgr > ~/sld_nco.txt 2>&1 &

# Set script name and run directory
drc_pwd=${PWD}
spt_nm=$(basename ${0}) # [sng] Script name
nco_version=$(ncks --version 2>&1 >/dev/null | grep NCO | awk '{print $5}')

# Set fonts for legibility
fnt_nrm=`tput sgr0` # Normal
fnt_bld=`tput bold` # Bold
fnt_rvr=`tput smso` # Reverse

# Defaults for command-line options and some derived variables
# Modify these defaults to save typing later
#caseid='AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc' # [sng] Case ID
alg_typ='bilinear' # [nbr] Algorithm for interpolation (bilinear|patch|neareststod|nearestdtos|conserve)
dbg_lvl=0 # [nbr] Debugging level
drc_in="${drc_pwd}" # [sng] Input file directory
#drc_in='' # [sng] Input file directory
drc_out="${DATA}/sld/rgr" # [sng] Output file directory
esmf_opt='> /dev/null' # [sng] ESMF_RegridWeightGen options
fml_nm='' # [sng] Family name (e.g., 'amip', 'control', 'experiment')
gaa_sng="--gaa rgr_script=${spt_nm} --gaa rgr_hostname=${HOSTNAME} --gaa rgr_version=${nco_version}" # [sng] Global attributes to add
grd_fl='' # [sng] Grid-file
grd_dst_glb="${DATA}/grids/180x360_SCRIP.20150901.nc" # [sng] Grid-file (destination) global
grd_sng='' # [sng] Grid string
hdr_pad='1000' # [B] Pad at end of header section
map_fl='' # [sng] Map-file
mpi_flg='No' # [sng] Parallelize over nodes
nco_opt='-O -t 1 --no_tmp_fl' # [sng] NCO defaults (e.g., '-O -6 -t 1')
nco_usr='' # [sng] NCO user-configurable options (e.g., '-D 1')
par_typ='bck' # [sng] Parallelism type
rgr_fl='' # [sng] Regridded file
rgr_opt='--rgr lat_nm_out=lat --rgr lon_nm_out=lon' # [sng] Regridding options (e.g., '--rgr col_nm=lndgrid')
#sld_fl='' # [sng] SLD file
sld_fl='AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc' # [sng] SLD file
thr_nbr=2 # [nbr] Thread number for regridder
#var_lst='FSNT,AODVIS' # [sng] Variables to process (empty means all)
var_lst='' # [sng] Variables to process (empty means all)
xtn_var='' # [sng] Extensive variables (e.g., 'TSurfStd_ct')
yyyy_srt='1980' # [yyyy] Start year
yyyy_end='1983' # [yyyy] End year

# Derived defaults (to print working examples)
grd_dst_dfl="${drc_out}/grd_dst.nc" # [sng] Grid-file (destination) default
grd_src="${drc_out}/grd_src.nc" # [sng] Grid-file (source) 

function fnc_usg_prn { # NB: dash supports fnc_nm (){} syntax, not function fnc_nm{} syntax
    # Print usage
    printf "\nQuick documentation for ${fnt_bld}${spt_nm}${fnt_nrm} (read script for more thorough explanations)\n\n"
    printf "${fnt_rvr}Basic usage:${fnt_nrm} ${fnt_bld}$spt_nm -s sld_fl -g grd_fl -i drc_in -o drc_out${fnt_nrm}\n\n"
    echo "Command-line options:"
    echo "${fnt_rvr}-a${fnt_nrm} ${fnt_bld}alg_typ${fnt_nrm}  Algorithm for weight generation (default ${fnt_bld}${alg_typ}${fnt_nrm})"
#    echo "${fnt_rvr}-c${fnt_nrm} ${fnt_bld}caseid${fnt_nrm}   Case ID string (default ${fnt_bld}${caseid}${fnt_nrm})"
    echo "${fnt_rvr}-d${fnt_nrm} ${fnt_bld}dbg_lvl${fnt_nrm}  Debugging level (default ${fnt_bld}${dbg_lvl}${fnt_nrm})"
#    echo "${fnt_rvr}-e${fnt_nrm} ${fnt_bld}yyyy_end${fnt_nrm} Ending year in YYYY format (default ${fnt_bld}${yyyy_end}${fnt_nrm})"
    echo "${fnt_rvr}-f${fnt_nrm} ${fnt_bld}fml_nm${fnt_nrm}   Family name (empty means none) (default ${fnt_bld}${fml_nm}${fnt_nrm})"
    echo "${fnt_rvr}-g${fnt_nrm} ${fnt_bld}grd_fl${fnt_nrm}   Grid-file (destination) (empty means generate internally) (default ${fnt_bld}${grd_fl}${fnt_nrm})"
    echo "${fnt_rvr}-G${fnt_nrm} ${fnt_bld}grd_sng${fnt_nrm}  Grid generation string (empty means none) (default ${fnt_bld}${grd_sng}${fnt_nrm})"
    echo "${fnt_rvr}-i${fnt_nrm} ${fnt_bld}drc_in${fnt_nrm}   Input directory (empty means look in current directory) (default ${fnt_bld}${drc_in}${fnt_nrm})"
    echo "${fnt_rvr}-m${fnt_nrm} ${fnt_bld}map_fl${fnt_nrm}   Map-file (empty means generate internally) (default ${fnt_bld}${map_fl}${fnt_nrm})"
    echo "${fnt_rvr}-n${fnt_nrm} ${fnt_bld}nco_opt${fnt_nrm}  NCO options (empty means none) (default ${fnt_bld}${nco_opt}${fnt_nrm})"
    echo "${fnt_rvr}-o${fnt_nrm} ${fnt_bld}drc_out${fnt_nrm}  Output directory (default ${fnt_bld}${drc_out}${fnt_nrm})"
    echo "${fnt_rvr}-p${fnt_nrm} ${fnt_bld}par_typ${fnt_nrm}  Parallelism type (default ${fnt_bld}${par_typ}${fnt_nrm})"
    echo "${fnt_rvr}-r${fnt_nrm} ${fnt_bld}rgr_fl${fnt_nrm}   Regridded-file (empty copies SLD filename) (default ${fnt_bld}${rgr_fl}${fnt_nrm})"
    echo "${fnt_rvr}-R${fnt_nrm} ${fnt_bld}rgr_opt${fnt_nrm}  Regridding options (empty means none) (default ${fnt_bld}${rgr_opt}${fnt_nrm})"
    echo "${fnt_rvr}-s${fnt_nrm} ${fnt_bld}sld_fl${fnt_nrm}   SLD file (mandatory) (default ${fnt_bld}${sld_fl}${fnt_nrm})"
    echo "${fnt_rvr}-v${fnt_nrm} ${fnt_bld}var_lst${fnt_nrm}  Variable list (empty means all) (default ${fnt_bld}${var_lst}${fnt_nrm})"
    echo "${fnt_rvr}-x${fnt_nrm} ${fnt_bld}xtn_var${fnt_nrm}  Extensive variables (empty means none) (default ${fnt_bld}${xtn_var}${fnt_nrm})"
    printf "\n"
    printf "Examples: ${fnt_bld}$spt_nm -s ${sld_fl} -g ${grd_dst_dfl} -i ${DATA}/sld/raw -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -a bilinear -s ${sld_fl} -g ${grd_dst_dfl} -i ${DATA}/sld/raw -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -a conserve -s ${sld_fl} -g ${grd_dst_dfl} -i ${DATA}/sld/raw -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -g ${grd_dst_dfl} -o ${drc_out} < ls ${drc_in}/*.1980*nc ${fnt_nrm}\n"
    printf "          ${fnt_bld}ls ${drc_in}/*.1980*nc | $spt_nm -g ${grd_dst_dfl} -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -x TSurfStd_ct -s ${DATA}/sld/raw/${sld_fl} -g ${grd_dst_dfl} -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -v TSurfAir -s ${DATA}/hdf/AIRS.2015.01.15.001.L2.RetStd.v6.0.11.0.G15015142014.hdf -g ${grd_dst_glb} -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -v CloudFrc_A -s ${DATA}/hdf/AIRS.2002.08.01.L3.RetStd_H031.v4.0.21.0.G06104133732.hdf -g ${grd_dst_glb} -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -s ${DATA}/hdf/MOD04_L2.A2000055.0005.006.2014307165927.hdf -g ${grd_dst_glb} -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -s ${DATA}/hdf/OMI-Aura_L2-OMIAuraSO2_2012m1222-o44888_v01-00-2014m0107t114720.h5 -g ${grd_dst_glb} -o ${drc_out} ${fnt_nrm}\n"
    printf "          ${fnt_bld}$spt_nm -v T -s ${DATA}/hdf/wrfout_v2_Lambert_notime.nc -g ${grd_dst_glb} -o ${drc_out} ${fnt_nrm}\n"
    exit 1
} # end fnc_usg_prn()

# Check argument number and complain accordingly
arg_nbr=$#
#printf "\ndbg: Number of arguments: ${arg_nbr}"
if [ ${arg_nbr} -eq 0 ]; then
  fnc_usg_prn
fi # !arg_nbr

# Parse command-line options:
# http://stackoverflow.com/questions/402377/using-getopts-in-bash-shell-script-to-get-long-and-short-command-line-options
# http://tuxtweaks.com/2014/05/bash-getopts
cmd_ln="${@}"
while getopts :a:d:f:g:G:h:i:m:n:o:p:R:r:s:v:x: OPT; do
    case ${OPT} in
	a) alg_typ=${OPTARG} ;; # Algorithm
#	c) caseid=${OPTARG} ;; # CASEID
	d) dbg_lvl=${OPTARG} ;; # Debugging level
#	e) yyyy_end=${OPTARG} ;; # End year
	f) fml_nm=${OPTARG} ;; # Family name
	g) grd_fl=${OPTARG} ;; # Grid-file
	G) grd_sng=${OPTARG} ;; # Grid generation string
	i) drc_in=${OPTARG} ;; # Input directory
	m) map_fl=${OPTARG} ;; # Map-file
	n) nco_usr=${OPTARG} ;; # NCO options
	o) drc_out=${OPTARG} ;; # Output directory
	p) par_typ=${OPTARG} ;; # Parallelism type
	r) rgr_fl=${OPTARG} ;; # Regridded file
	R) rgr_opt=${OPTARG} ;; # Regridding options
	s) sld_fl=${OPTARG} ;; # SLD file
	v) var_lst=${OPTARG} ;; # Variables
	x) xtn_var=${OPTARG} ;; # Extensive variables
	\?) # Unrecognized option
	    printf "\nERROR: Option ${fnt_bld}-$OPTARG${fnt_nrm} not allowed"
	    fnc_usg_prn ;;
    esac
done
shift $((OPTIND-1)) # Advance one argument

# Derived variables
grd_dst_dfl="${drc_out}/grd_dst.nc" # [sng] Grid-file (destination) default
grd_src="${drc_out}/grd_src.nc" # [sng] Grid-file (source) 
if [ ${alg_typ} = 'bilinear' ] || [ ${alg_typ} = 'bln' ] ; then 
    # ESMF options are bilinear|patch|neareststod|nearestdtos|conserve
    alg_opt='bilinear'
elif [ ${alg_typ} = 'conserve' ] || [ ${alg_typ} = 'conservative' ] || [ ${alg_typ} = 'cns' ] ; then 
    alg_opt='conserve'
elif [ ${alg_typ} = 'nearestdtos' ] || [ ${alg_typ} = 'nds' ] || [ ${alg_typ} = 'dtos' ] ; then 
    alg_opt='nearestdtos'
elif [ ${alg_typ} = 'neareststod' ] || [ ${alg_typ} = 'nsd' ] || [ ${alg_typ} = 'stod' ] ; then 
    alg_opt='nearestdtos'
elif [ ${alg_typ} = 'patch' ] || [ ${alg_typ} = 'pch' ] || [ ${alg_typ} = 'ptc' ] ; then 
    alg_opt='patch'
fi # !alg_typ
if [ -z "${drc_in}" ]; then
    drc_in="${drc_pwd}"
fi # !drc_in
if [ -n "${nco_usr}" ]; then 
    nco_opt="${nco_usr} ${nco_opt}"
fi # !var_lst
if [ -n "${gaa_sng}" ]; then 
    nco_opt="${nco_opt} ${gaa_sng}"
fi # !var_lst
if [ -n "${hdr_pad}" ]; then 
    nco_opt="${nco_opt} --hdr_pad=${hdr_pad}"
fi # !hdr_pad
if [ -n "${var_lst}" ]; then 
    nco_var_lst="-v ${var_lst}"
fi # !var_lst
if [ -n "${xtn_var}" ]; then 
    rgr_opt="${rgr_opt} --xtn=${xtn_var}"
fi # !var_lst
if [ ${par_typ} = 'bck' ]; then 
    par_opt=' &'
    par_opt_cf=''
elif [ ${par_typ} = 'mpi' ]; then 
    mpi_flg='Yes'
    par_opt=' &'
    par_opt_cf=''
fi # !par_typ

if [ ! -n "${sld_fl}" ]; then
    # http://stackoverflow.com/questions/2456750/detect-presence-of-stdin-contents-in-shell-script
    if [ -t 0 ]; then 
	# Input awaits on unit 0, i.e., on stdin
	fl_nbr=0
	while read -r line; do # NeR05 p. 179
	    fl_in[${fl_nbr}]=${line}
	    echo "Asked to regrid file ${fl_nbr}: ${fl_in[${fl_nbr}]}"
	    let fl_nbr=${fl_nbr}+1
	done < /dev/stdin
	sld_fl=${fl_in[0]}
    else
	echo "ERROR: Must specify input file with -s or with stdin"
	echo "HINT: Send file list to script with, e.g., 'ls *.nc | ${spt_nm}'"
	exit 1
    fi # stdin
fi # !grd_fl
if [ -n "${grd_fl}" ]; then 
    if [ ! -e "${grd_fl}" ]; then
	echo "ERROR: Unable to find specified grid-file ${grd_fl}"
	echo "HINT: Supply the full path-name for the destination grid, or generate one automatically with -G"
	exit 1
    fi # ! -e
    grd_dst=${grd_fl}
    grd_usr_flg='Yes'
else
    grd_dst=${grd_dst_dfl} # [sng] Grid-file default
fi # !grd_fl
if [ -z "${grd_sng}" ]; then 
    grd_sng_dfl="--rgr grd_ttl='Default internally-generated grid' --rgr grid=${grd_dst_dfl} --rgr lat_nbr=100 --rgr lon_nbr=100 --rgr snwe=30.0,70.0,-130.0,-90.0" # [sng] Grid string default
    grd_sng="${grd_sng_dfl}"
fi # !grd_sng
if [ -n "${map_fl}" ]; then 
    if [ ! -e "${map_fl}" ]; then
	echo "ERROR: Unable to find specified regrid map ${map_fl}"
	echo "HINT: Supply the full path-name for the regridding map"
	exit 1
    fi # ! -e
    map_usr_flg='Yes'
else
    map_fl_dfl="${drc_out}/map_src_to_dst_${alg_opt}.nc" # [sng] Map-file default
    map_fl=${map_fl_dfl}
fi # !map_fl
if [ -n "${rgr_fl}" ]; then 
    rgr_usr_flg='Yes'
else
    rgr_fl="${drc_out}/$(basename ${sld_fl})" # [sng] Map-file default
fi # !rgr_fl

# Doubly-derived fields
if [ "$(basename ${sld_fl})" = "${sld_fl}" ]; then
    sld_fl="${drc_in}/${sld_fl}"
fi # !basename
if [ "${sld_fl}" = "${rgr_fl}" ]; then
    echo "ERROR: SLD file = Regridded file = ${sld_fl}"
    echo "HINT: To prevent inadvertent data loss, ${spt_nm} insists that SLD file and regridded file be different"
    exit 1
fi # !basename

# Print initial state
if [ ${dbg_lvl} -ge 1 ]; then
    printf "dbg: alg_opt  = ${alg_opt}\n"
#    printf "dbg: caseid   = ${caseid}\n"
    printf "dbg: dbg_lvl  = ${dbg_lvl}\n"
    printf "dbg: drc_in   = ${drc_in}\n"
    printf "dbg: drc_out  = ${drc_out}\n"
    printf "dbg: fml_nm   = ${fml_nm}\n"
    printf "dbg: gaa_sng  = ${gaa_sng}\n"
    printf "dbg: grd_dst  = ${grd_dst}\n"
    printf "dbg: grd_sng  = ${grd_sng}\n"
    printf "dbg: grd_src  = ${grd_src}\n"
    printf "dbg: hdr_pad  = ${hdr_pad}\n"
    printf "dbg: map_fl   = ${map_fl}\n"
    printf "dbg: mpi_flg  = ${mpi_flg}\n"
    printf "dbg: nco_opt  = ${nco_opt}\n"
    printf "dbg: nd_nbr   = ${nd_nbr}\n"
    printf "dbg: par_typ  = ${par_typ}\n"
    printf "dbg: rgr_fl   = ${rgr_fl}\n"
    printf "dbg: sld_fl   = ${sld_fl}\n"
    printf "dbg: thr_nbr  = ${thr_nbr}\n"
    printf "dbg: var_lst  = ${var_lst}\n"
#    printf "dbg: yyyy_end = ${yyyy_end}\n"
fi # !dbg
if [ ${dbg_lvl} -ge 2 ]; then
    if [ ${mpi_flg} = 'Yes' ]; then
	for ((nd_idx=0;nd_idx<${nd_nbr};nd_idx++)); do
	    printf "dbg: nd_nm[${nd_idx}] = ${nd_nm[${nd_idx}]}\n"
	done # !nd
    fi # !mpi
fi # !dbg

# Create output directory
mkdir -p ${drc_out}

# Human-readable summary
if [ ${dbg_lvl} -ge 1 ]; then
    printf "Swath-Like Data (SLD) processor invoked with command:\n"
    echo "${spt_nm} ${cmd_ln}"
fi # !dbg
date_srt=$(date +"%s")
printf "Started SLD processing for file pattern ${sld_fl} at `date`.\n"
printf "Source grid will be inferred from SLD file and stored as ${grd_src}\n"
if [ "${grd_usr_flg}" = 'Yes' ]; then 
    printf "Destination grid-file supplied by user as ${grd_dst}\n"
else
    printf "Destination grid-file will be generated internally and stored as ${grd_dst}\n"
    if [ ${dbg_lvl} -ge 0 ]; then
	printf "Destination grid characteristics: ${grd_sng}\n"
    fi # !dbg
fi # !grd_usr_flg
if [ "${map_usr_flg}" = 'Yes' ]; then 
    printf "Map-file supplied as ${map_fl}\n"
else
    printf "Map-file will be generated internally and stored as ${map_fl}\n"
    printf "Algorithm used to generate weights is: ${alg_opt}\n"
fi # !map_usr_flg
printf "Regridded file will be stored as ${rgr_fl}\n"
printf "NCO version is ${nco_version}\n"

# Block 1: Destination grid
if [ "${grd_usr_flg}" != 'Yes' ]; then 
    printf "Generate destination grid...\n"
    # Block 1 Loop 1: Generate, check, and store (but do not yet execute) commands
    rgr_idx=1
    cmd_rgr[${rgr_idx}]="ncks ${nco_opt} ${grd_sng} ~/nco/data/in.nc ~/foo.nc"

    # Block 1 Loop 2: Execute and/or echo commands
    for ((rgr_idx=1;rgr_idx<=1;rgr_idx++)); do
	if [ ${dbg_lvl} -ge 1 ]; then
	    echo ${cmd_rgr[${rgr_idx}]}
	fi # !dbg
	if [ ${dbg_lvl} -le 1 ]; then
	    eval ${cmd_rgr[${rgr_idx}]}
	    if [ $? -ne 0 ]; then
		printf "${spt_nm}: ERROR Failed to generate destination grid\n"
		exit 1
	    fi # !err
	fi # !dbg
    done # !rgr_idx
fi # !grd_usr_flg
wait

# Block 2: Source grid
# Block 2 Loop 1: Source gridfile command
printf "Generate source grid...\n"
rgr_idx=2
if [ ! -e "${sld_fl}" ]; then
    echo "${spt_nm}: ERROR Unable to find SLD file ${sld_fl}"
    echo "HINT: All files implied to exist must be in the directory specified by their filename or in ${drc_in} before ${spt_nm} will proceed"
    exit 1
fi # ! -e
cmd_rgr[${rgr_idx}]="ncks ${nco_opt} --rgr nfr=y --rgr grid=${grd_src} ${sld_fl} ~/foo.nc"

# Block 2 Loop 2: Execute and/or echo commands
for ((rgr_idx=2;rgr_idx<=2;rgr_idx++)); do
    if [ ${dbg_lvl} -ge 1 ]; then
	echo ${cmd_rgr[${rgr_idx}]}
    fi # !dbg
    if [ ${dbg_lvl} -le 1 ]; then
	eval ${cmd_rgr[${rgr_idx}]}
	if [ $? -ne 0 ]; then
	    printf "${spt_nm}: ERROR Failed to generate source grid\n"
	    exit 1
	fi # !err
    fi # !dbg
done # !rgr_idx
wait

# Block 3: Source->destination maps
if [ "${map_usr_flg}" != 'Yes' ]; then 
    # Block 3 Loop 1: Mapfile commands
    printf "Generate source->destination mapping weights...\n"
    rgr_idx=3
    cmd_rgr[${rgr_idx}]="ESMF_RegridWeightGen -s ${grd_src} -d ${grd_dst} -w ${map_fl} --method ${alg_opt} --src_regional --dst_regional --ignore_unmapped ${esmf_opt}"

    # Block 3 Loop 2: Execute and/or echo commands
    for ((rgr_idx=3;rgr_idx<=3;rgr_idx++)); do
	if [ ${dbg_lvl} -ge 1 ]; then
	    echo ${cmd_rgr[${rgr_idx}]}
	fi # !dbg
	if [ ${dbg_lvl} -le 1 ]; then
	    eval ${cmd_rgr[${rgr_idx}]}
	    if [ $? -ne 0 ]; then
		printf "${spt_nm}: ERROR Failed to generate mapfile\n"
		printf "${spt_nm}: HINT When ESMF fails to generate mapfiles, it often puts additional debugging information in the file named PET0.RegridWeightGen.Log in the invocation directory\n"
		exit 1
	    fi # !err
	fi # !dbg
    done # !rgr_idx
fi # !map_usr_flg
wait

# Block 4: Regrid
printf "Regridding...\n"
rgr_idx=4
cmd_rgr[${rgr_idx}]="ncks ${nco_opt} ${nco_var_lst} ${rgr_opt} --map=${map_fl} ${sld_fl} ${rgr_fl}"

# Block 4 Loop 2: Execute and/or echo commands
for ((rgr_idx=4;rgr_idx<=4;rgr_idx++)); do
    if [ ${dbg_lvl} -ge 1 ]; then
	echo ${cmd_rgr[${rgr_idx}]}
    fi # !dbg
    if [ ${dbg_lvl} -le 1 ]; then
	eval ${cmd_rgr[${rgr_idx}]}
	if [ $? -ne 0 ]; then
	    printf "${spt_nm}: ERROR Failed to regrid\n"
	    exit 1
	fi # !err
    fi # !dbg
done # !rgr_idx
wait

date_end=$(date +"%s")
printf "Completed processing for SLD file ${sld_fl} at `date`.\n"
date_dff=$((date_end-date_srt))
echo "Quick plots of results: ncview ${rgr_fl} &"
echo "Elapsed time $((date_dff/60))m$((date_dff % 60))s"

exit 0
