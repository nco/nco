#!/bin/bash

# Purpose: Regrid regional output from SE grid to any other grid
# Only use script on regional output on an SE (spectral element) grid
# For regional output on rectangular, curvilinear, or swath grids,
# simply create/find the destination SCRIP file and use ncremap directly
# This script is necessary for SE regional output because no known SE grid
# generator creates regional SE grids with full vertice or weight information.

# Usage:
# rgn_rgr.sh lon_min lon_max lat_min lat_max dat_glb.nc grd_glb.nc grd_rgn.nc dat_rgn.nc dat_rgr.nc
# where
# dat_glb.nc is global output file from EAM containing at least one 2D variable (read-only)
# grd_glb.nc is gridfile in SCRIP format of dat_glb.nc (i.e., the global dual-grid) (read-only)
# grd_rgn.nc is destination gridfile in SCRIP format for regional output (can be regional) (read-only)
# dat_rgn.nc is regional output file from EAM containing at least one 2D variable (read-only)
# dat_rgr.nc is regional output file regridded to destination grid (i.e., the answers)

# Example:
# 1. Generate regional destination grid SCRIP file as per http://nco.sf.net/nco.html#scrip
# ncks --rgr grd_ttl='Test grid'#latlon=24,32#snwe=-16.0,-9.0,128.0,134.0#lat_typ=uni#lon_typ=grn_ctr --rgr scrip=${HOME}/grd_rgn.nc ~zender/nco/data/in.nc ~/foo.nc
# 2. Find or create data file with any 2D spatial variable on source grid on which (global) EAM ran
# File dat_glb.nc will not be altered, and smaller is better/faster since it will be regridded:
# ncks -O -v FSNT ${DATA}/ne30/raw/famipc5_ne30_v0.3_00003.cam.h0.1979-01.nc ~/dat_glb.nc
# 3. Rename EAM output regional variables with rgn_rnm.sh:
# ~/rgn_rnm.sh _128e_to_134e_9s_to_16s ~/rgn_in.nc ~/rgn_in_rnm.nc
# 4. Regrid file containing renamed variables with rgn_rgr.sh:
# ~/rgn_rgr.sh 128.0 134.0 -16.0 -9.0 ~/dat_glb.nc ${DATA}/grids/ne30np4_pentagons.091226.nc ~/grd_rgn.nc ~/rgn_in_rnm.nc ~/dat_rgr.nc

# Notes:
# 1. lat/lon_min/max must be same format (degrees or radians, 0->360 or -180->+180) as in dat_rgn.nc
# 2. Order of input bounding-box is WESN not WENS (may differ from EAM namelist-order)
# 3. Script runs faster if dat_glb.nc contains only one 2D/3D variable (e.g., FSNT)
# 4. dat_glb.nc need not have any data variable(s) in common with dat_rgn.nc
# 5. grd_glb.nc is a dual-grid computed offline (it cannot be inferred) for all SE maps

alg_opt='conserve' # [sng] Weight-generation option
dbg_lvl=0 # [enm] Debugging level
fl_idx=0 # [idx] File index
vrb_lvl=0 # [enm] Verbosity level

lon_min=${1}
lon_max=${2}
lat_min=${3}
lat_max=${4}
dat_glb=${5}
grd_glb=${6}
grd_rgn=${7}
dat_rgn=${8}
dat_rgr=${9}

# Regional regridding requires regions regional gridcell vertices to compute regional gridcell weights
# CAM SE outputs weights (i.e., area) not vertices to dat_glb (CAM SE does not know of SE dual-grid)
# Dual-grid (in grd_glb) contains vertice information necessary for regional regridding
# Identity-map dat_glb with grd_glb to annotate (nnt) output with necessary metadata (i.e., vertices)
# ncremap puts vertice information (and other CF metadata) into "annotated" output file dat_glb_nnt
# Later we cull vertice information from dat_glb_nnt, to infer dual-grid of regional data in dat_rgn
# ERWG cannot detect that this is an identity remapping and, sadly, recomputes area/weight information
# Grid inferral only needs vertice (not area) information so this regrid is wasteful
# Tempest intelligently identity-remaps SE grid (in .g format), yet emits no vertice information
# Presumably this is because Tempest SE->SE algorithm is high-order, does not use vertices
# Does Tempest output vertices when fed with dual-grid in SCRIP format? fxm: Probably. fxm: Test this
# Tempest (1.x or 2.x) outputs neither vertices nor regions for (non-dual) SE grids
# This Tempest feature would eliminate need for dual-grid gymnastics
dat_glb_nnt="${dat_glb}.nnt.tmp"
cmd_nnt[${fl_idx}]="ncremap --dbg=${dbg_lvl} --vrb=${vrb_lvl} -a bilin -s ${grd_glb} -g ${grd_glb} ${dat_glb} ${dat_glb_nnt}"
if [ ${dbg_lvl} -ne 2 ]; then
    eval ${cmd_nnt[${fl_idx}]}
    if [ $? -ne 0 ]; then
	printf "${spt_nm}: ERROR Failed to annotate global data file. Debug this:\n${cmd_nnt[${fl_idx}]}\n"
	exit 1
    fi # !err
fi # !dbg

# Subset coordinates only to rectangular regional bounding box ("coordinate subset region")
# NCO auxiliary hyperslabs grab all cells whose center lies in bounding-box
# The vertices of these cells define the unstructured region to regrid
# The specified bounding-box for cell centers must currently be lat/lon rectangular
# The dual-grid of the regridded region will look "bumpy" at fine scales, due to non-rectangular SE weights
# NCO <= 4.7.5 requires auxiliary coordinates (with -X) to have lat/lon standard_name attributes
# NCO 4.7.6 may allow auxiliary coordinate hyperslabs without lat/lon standard_name attributes
# For now, auxiliary coordinate hyperslabs only work on annotated data in dat_glb_nnt in NCO <= 4.7.5
aux_sng="-X ${lon_min},${lon_max},${lat_min},${lat_max}"
crd_sbs_rgn="${dat_glb}.sbs.tmp"
cmd_sbs[${fl_idx}]="ncks -O --dbg=${dbg_lvl} -v lat,lon ${aux_sng} ${dat_glb_nnt} ${crd_sbs_rgn}"
if [ ${dbg_lvl} -ne 2 ]; then
    eval ${cmd_sbs[${fl_idx}]}
    if [ $? -ne 0 ]; then
	printf "${spt_nm}: ERROR Failed to subset and hyperslab coordinates into regional file. Debug this:\n${cmd_sbs[${fl_idx}]}\n"
	exit 1
    fi # !err
fi # !dbg

# Rename fields in regional input file
# CAM SE namelist specifications/format for regional output are not crystal clear
# Multiple regions requested with namelist entries like (per Wuyin Lin)
# fincl3 = 'T','PRECT','CLDTOT','LWCF'
# fincl3lonlat = '120e_2n','262e_35n','120e:130e_2n:5n'
# Above example outputs variables for two single columns and one region
# Regional output file contains variables with CDL definition like
# lat_128e_to_134e_9s_to_16s(ncol_128e_to_134e_9s_to_16s),
# lon_128e_to_134e_9s_to_16s(ncol_128e_to_134e_9s_to_16s), 
# PRECL_128e_to_134e_9s_to_16s(time,ncol_128e_to_134e_9s_to_16s)
# NB: Same format applies to FV files
# However, FV files may be directly regridded by using vertices on ncks-generated regional grid (http://nco.sf.net/nco.html#scrip)
# FV output must be renamed as below, though does not require dual-grid gymnastics to obtain vertices
if [ 0 -eq 1 ] ; then
    dat_rgn_rnm="${dat_rgn}.rnm.tmp"
    cmd_rnm[${fl_idx}]="~/rgn_rnm.sh ${rgn_sng} ${dat_rgn} ${dat_rgn_rnm}"
    if [ ${dbg_lvl} -ne 2 ]; then
	eval ${cmd_rnm[${fl_idx}]}
	if [ $? -ne 0 ]; then
	    printf "${spt_nm}: ERROR Failed to rename regional input file. Debug this:\n${cmd_rnm[${fl_idx}]}\n"
	    exit 1
	fi # !err
    fi # !dbg
else # !0
    dat_rgn_rnm=${dat_rgn}
fi # !0

# Append regional input data to annotated regional coordinate file
cmd_apn[${fl_idx}]="ncks -A --dbg=${dbg_lvl} ${dat_rgn_rnm} ${crd_sbs_rgn}"
if [ ${dbg_lvl} -ne 2 ]; then
    eval ${cmd_apn[${fl_idx}]}
    if [ $? -ne 0 ]; then
	printf "${spt_nm}: ERROR Failed to append regional data to annotated regional coordinates. Debug this:\n${cmd_apn[${fl_idx}]}\n"
	exit 1
    fi # !err
fi # !dbg

# Use source grid inferral to regrid input regional data to desired output grid
cmd_rgn[${fl_idx}]="ncremap --dbg=${dbg_lvl} --vrb=${vrb_lvl} -a ${alg_opt} -g ${grd_rgn} ${crd_sbs_rgn} ${dat_rgr}"
if [ ${dbg_lvl} -ne 2 ]; then
    eval ${cmd_rgn[${fl_idx}]}
    if [ $? -ne 0 ]; then
	printf "${spt_nm}: ERROR Failed to regrid regional data. Debug this:\n${cmd_rgn[${fl_idx}]}\n"
	exit 1
    fi # !err
fi # !dbg

# Clean-up
/bin/rm -f ${crd_sbs_rgn}
/bin/rm -f ${dat_glb_nnt}

exit 0
