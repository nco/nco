#!/bin/bash
# includes gsl_rgr.nco

#===========================================================================
# After cmb_fl.sh
# Example: Annual trend of each model over Greenland and Tibet 
#   ( time- and spatial-average, standard deviation, 
#   anomaly and linear regression)
#
# Input files:
# /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_185001-200512.nc
#
# Output files:
# /data/cmip5/outout/snc/snc_LImon_all-mdl_historical_all-nsm_annual.nc
#
# Online: http://nco.sourceforge.net/nco.html#Annual-Average-over-Regions
#
# Execute this script: bash ann_avg.sh
#===========================================================================

#---------------------------------------------------------------------------
# Parameters
drc_in='/home/wenshanw/data/cmip5/'		# Directory of input files
drc_out='/home/wenshanw/data/cmip5/output/'	# Directory of output files

var=( 'snc' 'snd' )		# Variables
rlm='LImon'			# Realm
xpt=( 'historical' )		# Experiment ( could be more )

fld_out=( 'snc/' 'snd/' )		# Folders of output files
#------------------------------------------------------------

for var_id in {0..1}; do		# Loop over two variables
  # Names of all models 
  #   (ls [get file names]; cut [get the part for model names]; 
  #   sort; uniq [remove duplicates]; awk [print])
  mdl_set=$( ls ${drc_in}${var[var_id]}_${rlm}_*_${xpt[0]}_*.nc | \
    cut -d '_' -f 3 | sort | uniq -c | awk '{print $2}' )
  
  for mdl in ${mdl_set}; do		# Loop over models
  	# Loop over ensemble members
    for fn in $( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*.nc ); do
      pfx=$( echo ${fn} | cut -d'/' -f6 | cut -d'_' -f1-5 )
    
      # Two regions
      # Geographical weight
      ncap2 -O -s 'gw = cos(lat*3.1415926/180.); gw@long_name="geographical weight"\
        ;gw@units="ratio"' ${fn} ${drc_out}${fld_out[var_id]}${pfx}_gw.nc
      # Greenland
      ncwa -O -w gw -d lat,60.0,75.0 -d lon,300.0,340.0 -a lat,lon \
        ${drc_out}${fld_out[var_id]}${pfx}_gw.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_gw_1.nc
      # Tibet
      ncwa -O -w gw -d lat,30.0,40.0 -d lon,80.0,100.0 -a lat,lon \
        ${drc_out}${fld_out[var_id]}${pfx}_gw.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_gw_2.nc
    
      # Concatenate 2 regions together
      ncecat -O -u rgn ${drc_out}${fld_out[var_id]}${pfx}_gw_?.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_gw_rgn4.nc

      # Change the order of the dimensions
      ncpdq -O -a time,rgn ${drc_out}${fld_out[var_id]}${pfx}_gw_rgn4.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_gw_rgn4.nc

      # Remove the temporary files (optional)
      rm ${drc_out}${fld_out[var_id]}${pfx}_gw_?.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_gw.nc
    
      # Annual average (use the feature of 'Duration')
      ncra -O --mro -d time,"1956-01-01 00:00:0.0","2005-12-31 23:59:9.9",12,12 \
        ${drc_out}${fld_out[var_id]}${pfx}_gw_rgn4.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_yrly.nc
    
      # Anomaly
      # Long-term average
      ncwa -O -a time ${drc_out}${fld_out[var_id]}${pfx}_yrly.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_clm.nc
      # Subtract long-term average
      ncbo -O --op_typ=- ${drc_out}${fld_out[var_id]}${pfx}_yrly.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_clm.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_anm.nc
    done
    
    rm ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*_yrly.nc
    
    # Average over all the ensemble members
    ncea -O -4 ${drc_out}${fld_out[var_id]}${var[var_id]}_\
      ${rlm}_${mdl}_${xpt[0]}_*_anm.nc ${drc_out}${fld_out[var_id]}\
      ${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_anm.nc
    
    # Standard deviation ------------------------------
    for fn in $( ls ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_\
      ${xpt[0]}_*_anm.nc ); do
      pfx=$( echo ${fn} | cut -d'/' -f8 | cut -d'_' -f1-5 )
    
      # Difference between each ensemble member and the average of all members
      ncbo -O --op_typ=- ${fn} \
        ${drc_out}${fld_out[var_id]}${var[var_id]}_\
        ${rlm}_${mdl}_${xpt[0]}_all-nsm_anm.nc \
        ${drc_out}${fld_out[var_id]}${pfx}_dlt.nc
    done
    
    # RMS
    ncea -O -y rmssdn ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_\
      ${mdl}_${xpt[0]}_*_dlt.nc \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_\
      ${mdl}_${xpt[0]}_all-nsm_sdv.nc
    # Rename variables
    ncrename -v ${var[var_id]},sdv \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_\
      ${mdl}_${xpt[0]}_all-nsm_sdv.nc
    # Edit attributions
    ncatted -a standard_name,sdv,a,c,"_standard_deviation_over_ensemble" \
      -a long_name,sdv,a,c," Standard Deviation over Ensemble" \
      -a original_name,sdv,a,c," sdv" \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_\
      ${mdl}_${xpt[0]}_all-nsm_sdv.nc
    #------------------------------------------------------------
  
    # Linear regression -----------------------------------------
    #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    # Have to change the name of variable in the commands file 
    #   of gsl_rgr.nco manually (gsl_rgr.nco is listed below)
    ncap2 -O -S gsl_rgr.nco \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_\
      ${mdl}_${xpt[0]}_all-nsm_anm.nc ${drc_out}${fld_out[var_id]}${var[var_id]}\
      _${rlm}_${mdl}_${xpt[0]}_all-nsm_anm_rgr.nc
    #!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    # Get rid of temporary variables
    ncks -O -v c0,c1,pval,${var[var_id]},gw \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_\
      ${xpt[0]}_all-nsm_anm_rgr.nc \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${mdl}.nc
    #------------------------------------------------------------
    
    # Move the variable 'sdv' into the anomaly files (i.e., *anm.nc files)
    ncks -A -v sdv \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_\
      ${mdl}_${xpt[0]}_all-nsm_sdv.nc \
      ${drc_out}${fld_out[var_id]}${var[var_id]}_${mdl}.nc
    rm ${drc_out}${fld_out[var_id]}${var[var_id]}_*historical*
    
    echo Model ${mdl} done!
  done
  
  # Store models as groups in the output file
  ncecat -O --gag ${drc_out}${fld_out[var_id]}${var[var_id]}_*.nc 
  ${drc_out}${fld_out[var_id]}${var[var_id]}_\
    ${rlm}_all-mdl_${xpt[0]}_all-nsm_annual.nc

  echo Var ${var[var_id]} done!
done
