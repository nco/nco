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
##============================================================

# Input Directory
drc_in='/media/grele_data/wenshan/cesm/historical-exp/nco_grp/'
drc_out='/media/grele_data/wenshan/cesm/historical-exp/nco_grp/grp/'

# Constants
rlm='LImon'    # Realm: LandIce; Time frequency: monthly
tms='199001-200512'     # Timeseris
flt='nc'     # File Type

# Geographical weights
# Can be skipped when ncap2 works on group data
for fn in $( ls ${drc_in}snc_${rlm}_*_${tms}.${flt} ); do     # Loop over all snc files
  ncap2 -O -s \
    'gw = float(cos(lat*3.1415926/180.)); gw@long_name="geographical weight";\
    gw@units="ratio"; gw@standard_name="Geographical Weight"' ${fn} ${fn}
done

var=( 'snc' 'snd' )
xpt=( 'esmHistorical' 'historical' )
mdl=( 'CCSM4' 'CESM1-BGC' 'CESM1-CAM5' )

for i in {0..1}; do     # Loop over variables
  for j in {0..1}; do     # Loop over experiments
    for k in {0..2}; do     # Loop over models
      ncecat -O --glb_mtd_spr -G ${xpt[j]}/${mdl[k]}/ \
        ${drc_in}${var[i]}_${rlm}_${mdl[k]}_${xpt[j]}_*_${tms}.${flt} \
        ${drc_out}${var[i]}_${rlm}_${mdl[k]}_${xpt[j]}_all-nsm_${tms}.${flt}
      ncks -A \
        ${drc_out}${var[i]}_${rlm}_${mdl[k]}_${xpt[j]}_all-nsm_${tms}.${flt} \
        ${drc_out}${var[i]}_${rlm}_${mdl[0]}_${xpt[j]}_all-nsm_${tms}.${flt}
    done
    ncks -A \
      ${drc_out}${var[i]}_${rlm}_${mdl[0]}_${xpt[j]}_all-nsm_${tms}.${flt} \
      ${drc_out}${var[i]}_${rlm}_${mdl[0]}_${xpt[0]}_all-nsm_${tms}.${flt}
  done
  ncks -A \
    ${drc_out}${var[i]}_${rlm}_${mdl[0]}_${xpt[0]}_all-nsm_${tms}.${flt} \
    ${drc_out}${var[0]}_${rlm}_${mdl[0]}_${xpt[0]}_all-nsm_${tms}.${flt}
done

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
#     00 
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
#      00
#      {
#       snc(time, lat, lon)
#       snd(time, lat, lon)
#      }
#      01
#      {
#       snc(time, lat, lon)
#       snd(time, lat, lon)
#      }
#      02 { ... }
#      03 { ... }
#      04 { ... }
#      05 { ... }
#    }
#    CESM1-BGC
#    {
#      00 { ... }
#    }
#    CESM1-CAM5
#    {
#      00 { ... }
#      01 { ... }
#      02 { ... }
#    }
# }
