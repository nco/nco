#!/bin/bash
# includes gsl_rgr.nco

##===========================================================================
## After one-ensemble-one-file.sh
## Example: Annual trend of each model over Greenland and Tibet ( time- and spatial-average, standard deviation, anomaly and linear regression)
##
## Input files:
## /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_185001-200512.nc
##
## Output files:
## /data/cmip5/outout/snc/snc_LImon_all-mdl_historical_all-nsm_annual.nc
##
## Online: http://nco.sourceforge.net/nco.html#Annual-Average-over-Regions
##
## Execute this script: bash annual_avg.sh
##===========================================================================

##---------------------------------------------------------------------------
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
    for fn in $( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*.nc ); do	## loop over ensemble members
      pfx=$( echo ${fn} | cut -d'/' -f6 | cut -d'_' -f1-5 )		## part of the file name
    
      ## retrieve the 2 zones
      ## calculate the geographical weight first
      ncap2 -O -s 'gw = cos(lat*3.1415926/180.); gw@long_name="geographical weight";gw@units="ratio"' ${fn} ${drc_out}${fld_out[var_id]}${pfx}_gw.nc
      ## Greenland
      ncwa -O -w gw -d lat,60.0,75.0 -d lon,300.0,340.0 -a lat,lon ${drc_out}${fld_out[var_id]}${pfx}_gw.nc ${drc_out}${fld_out[var_id]}${pfx}_gw_1.nc
      ## Tibet
      ncwa -O -w gw -d lat,30.0,40.0 -d lon,80.0,100.0 -a lat,lon ${drc_out}${fld_out[var_id]}${pfx}_gw.nc ${drc_out}${fld_out[var_id]}${pfx}_gw_2.nc
    
      ## concatenate 2 zones together
      ncecat -O -u zone ${drc_out}${fld_out[var_id]}${pfx}_gw_?.nc ${drc_out}${fld_out[var_id]}${pfx}_gw_zone4.nc

      ## change the order of the dimension so that the record dimension is 'time'
      ncpdq -O -a time,zone ${drc_out}${fld_out[var_id]}${pfx}_gw_zone4.nc ${drc_out}${fld_out[var_id]}${pfx}_gw_zone4.nc

      ## remove the temporary files (optional)
      rm ${drc_out}${fld_out[var_id]}${pfx}_gw_?.nc ${drc_out}${fld_out[var_id]}${pfx}_gw.nc
    
      ## annual average (use the feature of 'Duration')
      ncra -O --mro -d time,"1956-01-01 00:00:0.0","2005-12-31 23:59:9.9",12,12 ${drc_out}${fld_out[var_id]}${pfx}_gw_zone4.nc ${drc_out}${fld_out[var_id]}${pfx}_yrly.nc
    
      ## anomaly
      ## long-term average
      ncwa -O -a time ${drc_out}${fld_out[var_id]}${pfx}_yrly.nc ${drc_out}${fld_out[var_id]}${pfx}_clm.nc
      ## subtract long-term average
      ncbo -O --op_typ=- ${drc_out}${fld_out[var_id]}${pfx}_yrly.nc ${drc_out}${fld_out[var_id]}${pfx}_clm.nc ${drc_out}${fld_out[var_id]}${pfx}_anm.nc
    done
    
    rm ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*_yrly.nc
    
    ## average over all the ensemble members
    ncea -O -4 ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*_anm.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_anm.nc
    
    ## standard deviation ------------------------------
    for fn in $( ls ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*_anm.nc ); do
      pfx=$( echo ${fn} | cut -d'/' -f8 | cut -d'_' -f1-5 )
    
      ## difference between each ensemble member and the average of all members
      ncbo -O --op_typ=- ${fn} ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_anm.nc ${drc_out}${fld_out[var_id]}${pfx}_dlt.nc
    done
    
    ## RMS
    ncea -O -y rmssdn ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*_dlt.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_sdv.nc
    ## rename variables
    ncrename -v ${var[var_id]},sdv ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_sdv.nc
    ## edit attributions
    ncatted -a standard_name,sdv,a,c,"_standard_deviation_over_ensemble" -a long_name,sdv,a,c," Standard Deviation over Ensemble" -a original_name,sdv,a,c," sdv" ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_sdv.nc
    ##------------------------------------------------------------
  
    ## linear regression -----------------------------------------
    ##!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    ## have to change the name of variable in the commands file of gsl_rgr.nco manually (gsl_rgr.nco is listed below)
    ncap2 -O -S gsl_rgr.nco ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_anm.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_anm_rgr.nc
    ##!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    ## get rid of temporary variables
    ncks -O -v c0,c1,pval,${var[var_id]},gw ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_anm_rgr.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${mdl}.nc
    ##------------------------------------------------------------
    
    ## move the variable 'sdv' into the anomaly files (i.e., *anm.nc files)
    ncks -A -v sdv ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_all-nsm_sdv.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${mdl}.nc
    rm ${drc_out}${fld_out[var_id]}${var[var_id]}_*historical*
    
    echo Model ${mdl} done!
  done
  
  ## Store models as groups in the output file
  ncecat -O --gag ${drc_out}${fld_out[var_id]}${var[var_id]}_*.nc ${drc_out}${fld_out[var_id]}${var[var_id]}_${rlm}_all-mdl_${xpt[0]}_all-nsm_annual.nc

  echo Var ${var[var_id]} done!
done
