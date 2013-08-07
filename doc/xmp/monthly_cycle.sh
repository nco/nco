#!/bin/bash

##============================================================
## After one-ensemble-one-file.sh
## Example: Monthly cycle of each model in Greenland
##
## Input files:
## /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_185001-200512.nc
##
## Output files:
## /data/cmip5/snc/snc_LImon__all-mdl_historical_all-nsm_GN_mthly-anm.nc
##
## Online: http://nco.sourceforge.net/nco.html#Monthly-Cycle
##
## Execute this script: bash monthly_cycle.sh
##============================================================

##------------------------------------------------------------
## parameters
drc_in='/home/wenshanw/data/cmip5/'		## directory of input files
drc_out='/home/wenshanw/data/cmip5/output/'	## directory of output files

var=( 'snc' 'snd' )		## variables
rlm='LImon'			## realm
xpt=( 'historical' )		## experiment ( could be more )

fld_out=( 'snc/' 'snd/' )		## folders of output files
##------------------------------------------------------------

for var_id in {0..1}; do		## loop over two variables
  ## names of all models (ls [get file names]; cut [get the part for model names]; sort; uniq [remove duplicates]; awk [print])
  mdl_set=$( ls ${drc_in}${var[var_id]}_${rlm}_*_${xpt[0]}_*.nc | cut -d '_' -f 3 | sort | uniq -c | awk '{print $2}' )

  for mdl in ${mdl_set}; do		## loop over models
    ## average all the ensemble members in each model
    ncea -O -4 -d time,"1956-01-01 00:00:0.0","2005-12-31 23:59:9.9" ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm.nc
    
    ## retrieve Greenland
    ## calculate the geographical weight first
    ncap2 -O -s 'gw = cos(lat*3.1415926/180.); gw@long_name="geographical weight";gw@units="ratio"' ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm.nc
    ## Greenland
    ncwa -O -w gw -d lat,60.0,75.0 -d lon,300.0,340.0 -a lat,lon ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN.nc
    
    ## anomaly----------------------------------------
    for moy in {1..12}; do		## loop over months
      mm=$( printf "%02d" ${moy} )	## change the format of month into 2-digit
      
      for yr in {1956..2005}; do		## loop over years
        if [ ${moy} -eq 1 ]; then	## if January, calculate the annual average of this year 
        	ncra -O -d time,"${yr}-01-01 00:00:0.0","${yr}-12-31 23:59:9.9" ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_${yr}.nc
        fi
        
        ## retrieve this month
        ncks -O -d time,"${yr}-${mm}-01 00:00:0.0","${yr}-${mm}-31 23:59:9.9" ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_${yr}${mm}.nc
        ## subtract the annual average from the month data
        ncbo -O --op_typ=- ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_${yr}${mm}.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_${yr}.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_${yr}${mm}_anm.nc
      done
      
      ## average over years
      ncra -O ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_????${mm}_anm.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_${mm}_anm.nc
    done
    ##--------------------------------------------------
    
    ## concatenate the months data together
    ncrcat -O ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_GN_??_anm.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${mdl}.nc
    
    echo Model ${mdl} done!
  done
  
  rm -f ${drc_out}${fld_out[var_id]}${var[var_id]}*historical*
  
  ## Store models as groups in the output file
  ncecat -O --gag -v ${var[var_id]} ${drc_out}${fld_out[var_id]}${var[var_id]}_*.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_all-mdl_${xpt[0]}_all-nsm_GN_mthly-anm.nc
  
  echo Var ${var[var_id]} done!
done
