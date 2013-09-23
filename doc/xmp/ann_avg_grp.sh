#!/bin/bash
#
#============================================================
# Group data output by cmb_fl_grp.sh
# Annual trend of each model over Greenland and Tibet 
# Time- and spatial-average, standard deviation and anomaly
# No regression yet (needs the help of ncap2)
# No standard deviation yet (ncap2 or ncea)
# No ensemble member average (ncea)
#
# Input files:
# sn_LImon_all-mdl_all-xpt_all-nsm_199001-200512.nc
#
# Online: http://nco.sourceforge.net/nco.html#Annual-Average-over-Regions
#
# Execute this script: bash ann_avg_grp.sh
#===========================================================================

# Input and output directory
drc='/media/grele_data/wenshan/cesm/historical-exp/nco_grp/grp/'

# Constants
rlm='LImon'    # Realm: LandIce; Time frequency: monthly
tms='199001-200512'     # Timeseris
flt='nc'     # File Type
var='sn'      # Variable
lbl='all-mdl_all-xpt_all-nsm'     # Label

# Greenland
ncwa -O -w gw -d lat,60.0,75.0 -d lon,300.0,340.0 -a lat,lon \
  ${drc}${var}_${rlm}_${lbl}_${tms}.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld.${flt}
# Tibet
ncwa -O -w gw -d lat,30.0,40.0 -d lon,80.0,100.0 -a lat,lon \
  ${drc}${var}_${rlm}_${lbl}_${tms}.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_tbt.${flt}
# time is no longer the record dimension in the outputs
ncks -O --mk_rec_dmn time \
  ${drc}${var}_${rlm}_${lbl}_${tms}_tbt.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_tbt.${flt}
ncks -O --mk_rec_dmn time \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld.${flt}

# Concatenate 2 regions together
ncecat -O -u rgn ${drc}${var}_${rlm}_${lbl}_${tms}_???.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_rgn2.${flt}
# Change dimensions order
ncpdq -O -a time,rgn ${drc}${var}_${rlm}_${lbl}_${tms}_rgn2.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_rgn2.${flt}

# Remove temporary files (optional)
#rm ${drc}${var}_${rlm}_${lbl}_${tms}_???.${flt}

#Annual average
#ncra -O --mro -d time,,,12,12 ${drc}${var}_${rlm}_${lbl}_${tms}_rgn2.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_rgn2_ann.${flt}
ncra -O --mro -d time,,,12,12 ${drc}${var}_${rlm}_${lbl}_${tms}_gld.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld_ann.${flt}

# Anomaly
#------------------------------------------------------------
# Long-term average
#ncwa -O -a time ${drc}${var}_${rlm}_${lbl}_${tms}_rgn2_ann.${flt} \
#  ${drc}${var}_${rlm}_${lbl}_${tms}_rgn2_clm.${flt}
ncwa -O -a time ${drc}${var}_${rlm}_${lbl}_${tms}_gld_ann.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld_clm.${flt}
# Subtract
ncbo -O --op_typ=- ${drc}${var}_${rlm}_${lbl}_${tms}_gld_ann.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld_clm.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld_anm.${flt}
#------------------------------------------------------------

# Standard Deviation
# Needs to confirm
#ncea -O -y rmssdn ${drc}${var}_${rlm}_${lbl}_${tms}_gld_anm.${flt} ${drc}${var}_${rlm}_${lbl}_${tms}_gld_anm-sdv.${flt}
