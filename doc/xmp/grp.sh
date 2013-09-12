#!/bin/bash
##
##============================================================
## Process netCDF-4 files with nested groups
##  - Create groups
##    - Input files as top level groups
##    - And/Or append variables or groups from other files
##  - Hyperslab
##  - Spatial average
##  - Time average
##  - Anomaly
##============================================================

drc_in='/media/grele_data/wenshan/cesm/historical-exp/nco_grp/'   #- directory of input files

#------------------------------------------------------------ 
# Create groups
#------------------------------------------------------------
# Input files as top level groups; group name = file name
#   - The input files are:
#       CCSM4_his.nc
#       CESM1-BGC_esmHis.nc
#       CESM1-BGC_his.nc
#       CESM1-CAM5_his.nc
#       CESM1-FASTCHEM_his.nc
#       CESM1-WACCM_his.nc
#   - One file looks like:
#       dimensions:
#         lat = 192
#         lon = 288
#         time = 192  // unlimited
#       variables:
#         double lat ( lat )
#           units :	degrees_north
#           axis :	Y
#           long_name :	latitude
#         double lon ( lon )
#           units : degrees_east
#           axis :  X
#           long_name : longitude
#         float snc ( time, lat, lon )
#           long_name : Snow Area Cover
#           units : %
#           _FillValue : 1e+20
#       ...
#------------------------------------------------------------         
ncecat --gag ${drc_in}C*.nc ${drc_in}snc_LImon_r1i1p1_199001-200512.nc
# Same for snow depth data (ie, snd) 
# Pay attention to the files you list using *; here I changed the files manually
ncecat --gag ${drc_in}C*.nc ${drc_in}snd_LImon_r1i1p1_199001-200512.nc

# Append variables
# - Append variable snc in each group in file snc_LImon_r1i1p1_199001-200512.nc to the coresponding group in file snd_LImon_r1i1p1_199001-200512.nc
sfx='_LImon_r1i1p1_199001-200512'
ncks -A -v snc ${drc_in}snc${sfx}.nc ${drc_in}snd${sfx}.nc
# - Rename the output file
mv ${drc_in}snd${sfx}.nc ${drc_in}snow${sfx}.nc

# Append groups
# - Multiple group levels
#   - Input files as top level  
ncecat --gag ${drc_in}r?i1p1.nc ${drc_in} snc_LImon_CCSM4_historical_all-nsm_199001-200512.nc
#   - /r?i1p1 --> /CCSM4_his/r?i1p1
ncks -G CCSM4_his -O ${drc_in}snc_LImon_CCSM4_historical_all-nsm_199001-200512.nc ${drc_in}snc_LImon_CCSM4_historical_all-nsm_199001-200512.nc
# - Append group
ncks -A -g CCSM4_his ${drc_in}snc_LImon_CCSM4_historical_all-nsm_199001-200512.nc ${drc_in}snc_LImon_CESM1-CAM5_historical_all-nsm_199001-200512.nc
# Rename the output file
mv ${drc_in}snc_LImon_CESM1-CAM5_historical_all-nsm_199001-200512.nc ${drc_in}snc_LImon_all-mdl_historical_all-nsm_199001-200512.nc

#------------------------------------------------------------
# Hyperslab
#------------------------------------------------------------
# Greenland
ncks -O -d lat,60.0,75.0 -d lon,300.0,340.0 ${drc_in}snow${sfx}.nc ${drc_in}snow${sfx}_GrIS.nc

#------------------------------------------------------------
# Spatial Average
#------------------------------------------------------------
ncwa -O -a lat,lon ${drc_in}snow${sfx}_GrIS.nc ${drc_in}snow${sfx}_GrIS-avg.nc
# hyperslab and spatial average in one step
ncwa -O -d lat,60.0,75.0 -d lon,300.0,340.0 -a lat,lon ${drc_in}snow${sfx}.nc ${drc_in}${sfx}_GrIS-avg.nc

#------------------------------------------------------------
# Time Average (eg, monthly)
#------------------------------------------------------------
for moy in {1..12}; do  # loop over months
  mm=$( printf "%02d" ${moy} ) ## change the format of month into 2-digit

  ncwa -O -d time,"1990-${mm}-01 00:00:0.0","2005-${mm}-21 23:59:9.9",12 -a time ${drc_in}snow${sfx}_GrIS.nc ${drc_in}snow${sfx}_GrIS_${mm}.nc
done
ncecat -O -u mon ${drc_in}snow${sfx}_GrIS_??.nc ${drc_in}snow${sfx}_GrIS_mon-avg.nc

#------------------------------------------------------------
# Anomaly
#------------------------------------------------------------
ncbo -O --op_typ=- ${drc_in}snow${sfx}_GrIS.nc ${drc_in}snow${sfx}_GrIS-avg.nc ${drc_in}snow${sfx}_GrIS-anomaly.nc
