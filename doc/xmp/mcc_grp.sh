#!/bin/bash
# Complied by Wang Wenshan 2013-09-22 Sunday 21:37:11
#============================================================
# Monthly cycle of each ensemble member in Greenland
#
# Input file from cmb_fl_grpsh
#   sn_LImon_all-mdl_all-xpt_all-nsm_199001-200512.nc
# Online: http://nco.sourceforge.net/nco.html#Monthly-Cycle
#
# Execute this script in command line: bash mcc_grp.sh
#============================================================
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
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
# time is no longer the record dimension in the outputs
ncks -O --mk_rec_dmn time \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld.${flt}
#!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

# Anomaly from annual average of each year 
for yyyy in {1990..2005}; do
  # Annual average
  ncwa -O -d time,"${yyyy}-01-01","${yyyy}-12-31" \
    ${drc}${var}_${rlm}_${lbl}_${tms}_gld.${flt} \
    ${drc}${var}_${rlm}_${lbl}_${tms}_gld_${yyyy}.${flt}

  # Anomaly
  ncbo -O --op_typ=- -d time,"${yyyy}-01-01","${yyyy}-12-31" \
    ${drc}${var}_${rlm}_${lbl}_${tms}_gld.${flt} \
    ${drc}${var}_${rlm}_${lbl}_${tms}_gld_${yyyy}.${flt} \
    ${drc}${var}_${rlm}_${lbl}_${tms}_gld_${yyyy}_anm.${flt}
done

# Monthly cycle
for moy in {1..12}; do
  mm=$( printf "%02d" ${moy} )      # Change to 2-digit format
  ncra -O -d time,"1990-${mm}-01",,12 \
    ${drc}${var}_${rlm}_${lbl}_${tms}_gld_????_anm.${flt} \
    ${drc}${var}_${rlm}_${lbl}_${tms}_gld_${mm}_anm.${flt}
done
# Concatenate 12 months together
ncrcat -O ${drc}${var}_${rlm}_${lbl}_${tms}_gld_??_anm.${flt} \
  ${drc}${var}_${rlm}_${lbl}_${tms}_gld_mth_anm.${flt}
