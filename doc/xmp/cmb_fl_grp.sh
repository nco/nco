#!/bin/bash
#
#============================================================
# Aggregate models to one group file
#
# Method:
# - Create files with groups by ncecat --gag
#	- Append groups level by level using ncks
#
# Input files like:
# snc_LImon_CCSM4_historical_r1i1p1_199001-200512.nc
# snd_LImon_CESM1-BGC_esmHistorical_r1i1p1_199001-200512.nc
# 
# Output files like:
# sn_LImon_199001-200512.nc
#
# Online: http://nco.sourceforge.net/nco.html#Combine-Files
#
# Execute this script: bash cmb_fl_grp.sh
#============================================================

# Directories
drc_in='../data/'
drc_out='../data/grp/'

# Constants
rlm='LImon'         # Realm: LandIce; Time frequency: monthly
tms='200001-200512' # Timeseris
flt='nc'            # File Type

# Geographical weights
# Can be skipped when ncap2 works on group data
# Loop over all snc files
for fn in $( ls ${drc_in}snc_${rlm}_*_${tms}.${flt} ); do
  ncap2 -O -s \
    'gw = float(cos(lat*3.1416/180.)); gw@long_name="geographical weight";'\
    ${fn} ${fn}
done

var=( 'snc' 'snd' )
xpt=( 'esmHistorical' 'historical' )
mdl=( 'CCSM4' 'CESM1-BGC' 'CESM1-CAM5' )

for i in {0..1}; do     # Loop over variables
  for j in {0..1}; do   # Loop over experiments
    for k in {0..2}; do # Loop over models
      ncecat -O --glb_mtd_spp -G ${xpt[j]}/${mdl[k]}/${mdl[k]}_ \
        ${drc_in}${var[i]}_${rlm}_${mdl[k]}_${xpt[j]}_*_${tms}.${flt} \
        ${drc_out}${var[i]}_${rlm}_${mdl[k]}_${xpt[j]}_all-nsm_${tms}.${flt}
      ncks -A \
        ${drc_out}${var[i]}_${rlm}_${mdl[k]}_${xpt[j]}_all-nsm_${tms}.${flt} \
        ${drc_out}${var[i]}_${rlm}_${mdl[0]}_${xpt[j]}_all-nsm_${tms}.${flt}
    done                # Loop done: models
    ncks -A \
      ${drc_out}${var[i]}_${rlm}_${mdl[0]}_${xpt[j]}_all-nsm_${tms}.${flt} \
      ${drc_out}${var[i]}_${rlm}_${mdl[0]}_${xpt[0]}_all-nsm_${tms}.${flt}
  done                  # Loop done: experiments
  ncks -A \
    ${drc_out}${var[i]}_${rlm}_${mdl[0]}_${xpt[0]}_all-nsm_${tms}.${flt} \
    ${drc_out}${var[0]}_${rlm}_${mdl[0]}_${xpt[0]}_all-nsm_${tms}.${flt}
done                    # Loop done: variables

# Rename output file
mv ${drc_out}${var[0]}_${rlm}_${mdl[0]}_${xpt[0]}_all-nsm_${tms}.${flt} \
  ${drc_out}sn_${rlm}_all-mdl_all-xpt_all-nsm_${tms}.${flt}
# Remove temporary files
rm ${drc_out}sn?_${rlm}*.nc

#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# Coming soon!
#- Group names: 
#   E.g., file snc_LImon_CESM1-CAM5_historical_r1i1p1_199001-200512.nc
#   will be group /historical/CESM1-CAM5/00
#- You can rename groups on the last level to be more meaningful by
#ncrename -g ${xpt}/${mdl}/02,${xpt}/${mdl}/r3i1p1 \
#  ${drc_out}${var}_${rlm}_${mdl}_all-nsm_${tms}.${flt}
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#------------------------------------------------------------
# Output file structure
#------------------------------------------------------------
# esmHistorical 
# {
#   CESM1-BGC 
#   {
#     CESM1-BGC_00 
#     {
#       snc(time, lat, lon)
#       snd(time, lat, lon)
#     }
#   }
# }
# historical
# {
#    CCSM4
#    {
#      CCSM4_00
#      {
#       snc(time, lat, lon)
#       snd(time, lat, lon)
#      }
#      CCSM4_01
#      {
#       snc(time, lat, lon)
#       snd(time, lat, lon)
#      }
#      CCSM4_02 { ... }
#      CCSM4_03 { ... }
#      CCSM4_04 { ... }
#    }
#    CESM1-BGC
#    {
#      CESM1-BGC_00 { ... }
#    }
#    CESM1-CAM5
#    {
#      CESM1-CAM5_00 { ... }
#      CESM1-CAM5_01 { ... }
#      CESM1-CAM5_02 { ... }
#    }
# }
