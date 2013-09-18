#!/bin/bash

#============================================================
# After cmb_fl.sh
# Example: Monthly cycle of each model in Greenland
#
# Input files:
# /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_185001-200512.nc
#
# Output files:
# /data/cmip5/snc/snc_LImon__all-mdl_historical_all-nsm_GN_mthly-anm.nc
#
# Online: http://nco.sourceforge.net/nco.html#Monthly-Cycle
#
# Execute this script: bash mcc.sh
#============================================================

#------------------------------------------------------------
# Parameters
drc_in='/home/wenshanw/data/cmip5/'		# Directory of input files
drc_out='/home/wenshanw/data/cmip5/output/'	# Directory of output files

var=( 'snc' 'snd' )		# Variables
rlm='LImon'			# Realm
xpt=( 'historical' )		# Experiment ( could be more )

fld_out=( 'snc/' 'snd/' )		# Folders of output files
#------------------------------------------------------------

for var_id in {0..1}; do		# Loop over two variables
  # names of all models 
  #  (ls [get file names]; cut [get the part for model names]; 
  #  sort; uniq [remove duplicates]; awk [print])
  mdl_set=$( ls ${drc_in}${var[var_id]}_${rlm}_*_${xpt[0]}_*.nc | \
    cut -d '_' -f 3 | sort | uniq -c | awk '{print $2}' )

  for mdl in ${mdl_set}; do		## Loop over models
    # Average all the ensemble members of each model
    ncea -O -4 -d time,"1956-01-01 00:00:0.0","2005-12-31 23:59:9.9" \
      ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*.nc \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm.nc
    
    # Greenland
    # Geographical weight
    ncap2 -O -s \
      'gw = cos(lat*3.1415926/180.); \
      gw@long_name="geographical weight";gw@units="ratio"' \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm.nc \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm.nc
    ncwa -O -w gw -d lat,60.0,75.0 -d lon,300.0,340.0 -a lat,lon \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm.nc \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN.nc
    
    # Anomaly----------------------------------------
    for moy in {1..12}; do		# Loop over months
      mm=$( printf "%02d" ${moy} )	# Change to 2-digit format
      
      for yr in {1956..2005}; do		# Loop over years
        # If January, calculate the annual average
        if [ ${moy} -eq 1 ]; then	 
        	ncra -O -d time,"${yr}-01-01 00:00:0.0","${yr}-12-31 23:59:9.9" \
            ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_\
            ${xpt[0]}_all-nsm_GN.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_\
            ${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_${yr}.nc
        fi
        
        # The specific month
        ncks -O -d time,"${yr}-${mm}-01 00:00:0.0","${yr}-${mm}-31 23:59:9.9" \
          ${drc_out}${fld_out[var_id]}${var[var_id]}_\
          ${rlm}_${mdl}_${xpt[0]}_all-nsm_GN.nc \
          ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_\
          all-nsm_GN_${yr}${mm}.nc
        # Subtract the annual average from the monthly data
        ncbo -O --op_typ=- ${drc_out}${fld_out[var_id]}${var[var_id]}_\
          ${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_${yr}${mm}.nc \
          ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_\
          all-nsm_GN_${yr}.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_\
          ${mdl}_${xpt[0]}_all-nsm_GN_${yr}${mm}_anm.nc
      done
      
      # Average over years
      ncra -O ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_\
        ${xpt[0]}_all-nsm_GN_????${mm}_anm.nc \
        ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_\
        ${xpt[0]}_all-nsm_GN_${mm}_anm.nc
    done
    #--------------------------------------------------
    
    # Concatenate months together
    ncrcat -O ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_\
      ${xpt[0]}_all-nsm_GN_??_anm.nc \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${mdl}.nc
    
    echo Model ${mdl} done!
  done
  
  rm -f ${drc_out}${fld_out[var_id]}${var[var_id]}*historical*
  
  # Store models as groups in the output file
  ncecat -O --gag -v ${var[var_id]} \
    ${drc_out}${fld_out[var_id]}${var[var_id]}_*.nc \
    ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_all-mdl_\
    ${xpt[0]}_all-nsm_GN_mthly-anm.nc
  
  echo Var ${var[var_id]} done!
done
