#!/bin/sh

# Purpose: Regridding script tailored for Swath-Like-Data (SLD)
# This script regrids all input SLD files to specified output grid

# Author: C. Zender
# Created: 20150909

# Source: fxm

# Additional Documentation:

# Production usage:
# chmod a+x ~/sld_nco.sh
# sld_nco.sh -c AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc -i ${DATA}/sld/raw -o ${DATA}/sld/rgr

# Debugging and Benchmarking:
# sld_nco.sh > ~/sld_nco.txt 2>&1 &
# sld_nco.sh -c AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc -i ${DATA}/sld/raw -o ${DATA}/sld/rgr > ~/sld_nco.txt 2>&1 &

# Set script name and run directory
spt_nm=`basename ${0}` # [sng] Script name
nco_version=$(ncks --version 2>&1 >/dev/null | grep NCO | awk '{print $5}')

# Set fonts for legibility
fnt_nrm=`tput sgr0` # Normal
fnt_bld=`tput bold` # Bold
fnt_rvr=`tput smso` # Reverse

# Defaults for command-line options and some derived variables
# Modify these defaults to save typing later
caseid='AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc' # [sng] Case ID
dbg_lvl=0 # [nbr] Debugging level
drc_in="${DATA}/sld/raw" # [sng] Input file directory
drc_out="${DATA}/sld/rgr" # [sng] Output file directory
fml_nm='' # [sng] Family name (e.g., 'amip', 'control', 'experiment')
gaa_sng="--gaa sld_script=${spt_nm} --gaa sld_hostname=${HOSTNAME} --gaa sld_version=${nco_version}" # [sng] Global attributes to add
grd_fl="${DATA}/sld/rgr/grd_dst_40x30.nc" # [sng] Grid-file
grd_fl_dfl="${DATA}/sld/rgr/grd_dst.nc" # [sng] Grid-file default
grd_sng='' # [sng] Grid string
grd_sng_dfl='--rgr grd_ttl='Equi-angular grid 40x30' --rgr grid=${DATA}/sld/rgr/grd_dst_40x30.nc --rgr lat_nbr=100 --rgr lon_nbr=100 --rgr lat_sth=30.0 --rgr lat_nrt=70.0 --rgr lon_wst=-120.0 --rgr lon_est=-90.0' # [sng] Grid string default
hdr_pad='1000' # [B] Pad at end of header section
map_fl='' # [sng] Map name
#map_fl="${DATA}/sld/rgr/map_airs_to_dst_bilin.nc"
nco_opt='--no_tmp_fl' # [sng] NCO options (e.g., '-7 -D 1 -L 1')
par_typ='bck' # [sng] Parallelism type
rgr_opt='' # [sng] Regridding options (e.g., '--rgr col_nm=lndgrid')
thr_nbr=2 # [nbr] Thread number for regridder
#var_lst='FSNT,AODVIS' # [sng] Variables to process (empty means all)
var_lst='' # [sng] Variables to process (empty means all)
yyyy_srt='1980' # [yyyy] Start year
yyyy_end='1983' # [yyyy] End year

function fnc_usg_prn {
    # Print usage
    printf "\nQuick documentation for ${fnt_bld}${spt_nm}${fnt_nrm} (read script for more thorough explanations)\n\n"
    printf "${fnt_rvr}Basic usage:${fnt_nrm} ${fnt_bld}$spt_nm -c caseid -g grd_fl -i drc_in -o drc_out${fnt_nrm}\n\n"
    echo "Command-line options:"
    echo "${fnt_rvr}-c${fnt_nrm} ${fnt_bld}caseid${fnt_nrm}   Case ID string (default ${fnt_bld}${caseid}${fnt_nrm})"
    echo "${fnt_rvr}-d${fnt_nrm} ${fnt_bld}dbg_lvl${fnt_nrm}  Debugging level (default ${fnt_bld}${dbg_lvl}${fnt_nrm})"
#    echo "${fnt_rvr}-e${fnt_nrm} ${fnt_bld}yyyy_end${fnt_nrm} Ending year in YYYY format (default ${fnt_bld}${yyyy_end}${fnt_nrm})"
    echo "${fnt_rvr}-f${fnt_nrm} ${fnt_bld}fml_nm${fnt_nrm}   Family name (empty means none) (default ${fnt_bld}${fml_nm}${fnt_nrm})"
    echo "${fnt_rvr}-g${fnt_nrm} ${fnt_bld}grd_fl${fnt_nrm}   Grid name (empty means none) (default ${fnt_bld}${grd_fl}${fnt_nrm})"
    echo "${fnt_rvr}-G${fnt_nrm} ${fnt_bld}grd_sng${fnt_nrm}  Grid generation string (empty means none) (default ${fnt_bld}${grd_sng}${fnt_nrm})"
    echo "${fnt_rvr}-i${fnt_nrm} ${fnt_bld}drc_in${fnt_nrm}   Input directory ${fnt_bld}drc_in${fnt_nrm} (default ${fnt_bld}${drc_in}${fnt_nrm})"
    echo "${fnt_rvr}-m${fnt_nrm} ${fnt_bld}map_fl${fnt_nrm}   Map-file (empty means none) (default ${fnt_bld}${map_fl}${fnt_nrm})"
    echo "${fnt_rvr}-n${fnt_nrm} ${fnt_bld}nco_opt${fnt_nrm}  NCO options (empty means none) (default ${fnt_bld}${nco_opt}${fnt_nrm})"
    echo "${fnt_rvr}-o${fnt_nrm} ${fnt_bld}drc_out${fnt_nrm}  Output directory (default ${fnt_bld}${drc_out}${fnt_nrm})"
    echo "${fnt_rvr}-p${fnt_nrm} ${fnt_bld}par_typ${fnt_nrm}  Parallelism type (default ${fnt_bld}${par_typ}${fnt_nrm})"
    echo "${fnt_rvr}-R${fnt_nrm} ${fnt_bld}rgr_opt${fnt_nrm}  Regridding options (empty means none) (default ${fnt_bld}${rgr_opt}${fnt_nrm})"
#    echo "${fnt_rvr}-s${fnt_nrm} ${fnt_bld}yyyy_srt${fnt_nrm} Starting year in YYYY format (default ${fnt_bld}${yyyy_srt}${fnt_nrm})"
    echo "${fnt_rvr}-v${fnt_nrm} ${fnt_bld}var_lst${fnt_nrm}  Variable list (empty means all) (default ${fnt_bld}${var_lst}${fnt_nrm})"
    echo "${fnt_rvr}-x${fnt_nrm} ${fnt_bld}xpt_flg${fnt_nrm}  Xperimental switch (for developers) (default ${fnt_bld}${xpt_flg}${fnt_nrm})"
    printf "\n"
    printf "Examples: ${fnt_bld}$spt_nm -c ${caseid} -g ${grd_fl} -i ${drc_in} -o ${drc_out} ${fnt_nrm}\n"
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
while getopts :a:b:c:d:e:f:g:G:h:i:m:n:o:p:R:s:v:x: OPT; do
    case ${OPT} in
	c) caseid=${OPTARG} ;; # CASEID
	d) dbg_lvl=${OPTARG} ;; # Debugging level
	e) yyyy_end=${OPTARG} ;; # End year
	f) fml_nm=${OPTARG} ;; # Family name
	g) grd_fl=${OPTARG} ;; # Grid name
	G) grd_sng=${OPTARG} ;; # Grid generation string
	h) hst_nm=${OPTARG} ;; # History tape name
	i) drc_in=${OPTARG} ;; # Input directory
	m) map_fl=${OPTARG} ;; # Map-file
	n) nco_opt=${OPTARG} ;; # NCO options
	o) drc_out=${OPTARG} ;; # Output directory
	p) par_typ=${OPTARG} ;; # Parallelism type
	R) rgr_opt=${OPTARG} ;; # Regridding options
	s) yyyy_srt=${OPTARG} ;; # Start year
	v) var_lst=${OPTARG} ;; # Variables
	x) xpt_flg=${OPTARG} ;; # Experiment flag
	h) fnc_usg_prn ;; # Help
	\?) # Unrecognized option
	    printf "\nERROR: Option ${fnt_bld}-$OPTARG${fnt_nrm} not allowed"
	    fnc_usg_prn ;;
    esac
done
shift $((OPTIND-1)) # Advance one argument

# Derived variables
out_nm=${caseid}
if [ -n "${fml_nm}" ]; then 
    out_nm="${fml_nm}"
fi # !fml_nm
if [ -n "${gaa_sng}" ]; then 
    nco_opt="${nco_opt} ${gaa_sng}"
fi # !var_lst
if [ -n "${var_lst}" ]; then 
    nco_opt="${nco_opt} -v ${var_lst}"
fi # !var_lst
if [ -n "${hdr_pad}" ]; then 
    nco_opt="${nco_opt} --hdr_pad=${hdr_pad}"
fi # !hdr_pad
if [ ${par_typ} = 'bck' ]; then 
    par_opt=' &'
    par_opt_cf=''
elif [ ${par_typ} = 'mpi' ]; then 
    mpi_flg='Yes'
    par_opt=' &'
    par_opt_cf=''
fi # !par_typ
if [ -n "${grd_fl}" ]; then 
    if [ ! -e "${grd_fl}" ]; then
	echo "ERROR: Unable to find specified grid-file ${grd_fl}"
	echo "HINT: Supply the full path-name for the destination grid, or generate one automatically with -G"
	exit 1
    fi # ! -e
fi # !grd_fl
if [ -z "${grd_sng}" ]; then 
    grd_sng="${grd_sng_dfl}"
if [ -n "${map_fl}" ]; then 
    if [ ! -e "${map_fl}" ]; then
	echo "ERROR: Unable to find specified regrid map ${map_fl}"
	echo "HINT: Supply the full path-name for the regridding map"
	exit 1
    fi # ! -e
    rgr_opt="${rgr_opt} --map ${map_fl}"
fi # !map_fl

# Print initial state
if [ ${dbg_lvl} -ge 1 ]; then
    printf "dbg: caseid   = ${caseid}\n"
    printf "dbg: dbg_lvl  = ${dbg_lvl}\n"
    printf "dbg: drc_in   = ${drc_in}\n"
    printf "dbg: drc_out  = ${drc_out}\n"
    printf "dbg: fml_nm   = ${fml_nm}\n"
    printf "dbg: gaa_sng  = ${gaa_sng}\n"
    printf "dbg: grd_fl   = ${grd_fl}\n"
    printf "dbg: grd_sng  = ${grd_sng}\n"
    printf "dbg: hdr_pad  = ${hdr_pad}\n"
    printf "dbg: map_fl   = ${map_fl}\n"
    printf "dbg: mpi_flg  = ${mpi_flg}\n"
    printf "dbg: nco_opt  = ${nco_opt}\n"
    printf "dbg: nd_nbr   = ${nd_nbr}\n"
    printf "dbg: par_typ  = ${par_typ}\n"
    printf "dbg: thr_nbr  = ${thr_nbr}\n"
    printf "dbg: var_lst  = ${var_lst}\n"
    printf "dbg: yyyy_end = ${yyyy_end}\n"
    printf "dbg: yyyy_srt = ${yyyy_srt}\n"
fi # !dbg
if [ ${dbg_lvl} -ge 2 ]; then
    if [ ${mpi_flg} = 'Yes' ]; then
	for ((nd_idx=0;nd_idx<${nd_nbr};nd_idx++)); do
	    printf "dbg: nd_nm[${nd_idx}] = ${nd_nm[${nd_idx}]}\n"
	done # !nd
    fi # !mpi
fi # !dbg

# Create output directory, go to working directory
mkdir -p ${drc_out}
cd ${drc_out}

# Human-readable summary
if [ ${dbg_lvl} -ge 1 ]; then
    printf "Swath-Like Data (SLD) processor invoked with command:\n"
    echo "${spt_nm} ${cmd_ln}"
fi # !dbg
date_srt=$(date +"%s")
printf "Started SLD processing for file pattern ${caseid} at `date`.\n"
if [ -n "${map_fl}" ]; then 
    printf "Map-file supplied as ${map_fl}\n"
else
    printf "Map-file will be internally generated.\n"
fi # !map
printf "NCO version is ${nco_version}\n"

# Block 1: Generate destination grid
printf "Generating destination grid...\n"
# Block 1 Loop 1: Generate, check, and store (but do not yet execute) commands
if [ -z "${grd_fl}" ]; then 
    clm_idx=1
    grd_fl=${grd_fl_dfl}
    cmd_clm[${clm_idx}]="ncks -O -D 1 ${grd_sng} ~/nco/data/in.nc ~/foo.nc"

    # Block 1 Loop 2: Execute and/or echo commands
    for ((clm_idx=1;clm_idx<=1;clm_idx++)); do
	printf "Generate destination grid #${clm_idx} ...\n"
	if [ ${dbg_lvl} -ge 1 ]; then
	    echo ${cmd_clm[${clm_idx}]}
	fi # !dbg
	if [ ${dbg_lvl} -le 1 ]; then
	    eval ${cmd_clm[${clm_idx}]}
	fi # !dbg
    done # !clm_idx
done # !grd_fl
wait

# Block 2: Generate source grids
# Block 2 Loop 1: Source gridfile commands
printf "Generating source grids...\n"
clm_idx=2
cmd_clm[${clm_idx}]="ncks -O -D 5 --rgr nfr=y --rgr grid=${DATA}/sld/rgr/grd_airs.nc ${DATA}/sld/raw/AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.nc ~/foo.nc"

# Block 2 Loop 2: Execute and/or echo commands
for ((clm_idx=2;clm_idx<=2;clm_idx++)); do
    printf "Generate source grid #${clm_idx} ...\n"
    if [ ${dbg_lvl} -ge 1 ]; then
	echo ${cmd_clm[${clm_idx}]}
    fi # !dbg
    if [ ${dbg_lvl} -le 1 ]; then
	eval ${cmd_clm[${clm_idx}]}
    fi # !dbg
done # !clm_idx
wait

# Block 3: Generate source->destination maps
# Block 3 Loop 1: Mapfile commands
printf "Generating source->destination maps...\n"
clm_idx=3
cmd_clm[${clm_idx}]="ESMF_RegridWeightGen -s ${DATA}/sld/rgr/grd_airs.nc -d ${grd_fl} -w ${DATA}/sld/rgr/map_airs_to_dst_bilin.nc --method bilinear --src_regional --dst_regional --ignore_unmapped"

# Block 3 Loop 2: Execute and/or echo commands
for ((clm_idx=3;clm_idx<=3;clm_idx++)); do
    printf "Generate source->destination map #${clm_idx} ...\n"
    if [ ${dbg_lvl} -ge 1 ]; then
	echo ${cmd_clm[${clm_idx}]}
    fi # !dbg
    if [ ${dbg_lvl} -le 1 ]; then
	eval ${cmd_clm[${clm_idx}]}
    fi # !dbg
done # !clm_idx
wait

# Block 4: Regrid
printf "Regridding...\n"
clm_idx=4
cmd_clm[${clm_idx}]="ncks -D 6 -O --rnr=0.5 --map=${DATA}/sld/rgr/map_airs_to_dst_bilin.nc ${DATA}/sld/raw/AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.nc ${DATA}/sld/rgr/dogfood.nc"

# Block 4 Loop 2: Execute and/or echo commands
for ((clm_idx=4;clm_idx<=4;clm_idx++)); do
    printf "Regrid file #${clm_idx} ...\n"
    if [ ${dbg_lvl} -ge 1 ]; then
	echo ${cmd_clm[${clm_idx}]}
    fi # !dbg
    if [ ${dbg_lvl} -le 1 ]; then
	eval ${cmd_clm[${clm_idx}]}
    fi # !dbg
done # !clm_idx
wait
