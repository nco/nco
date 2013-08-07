#!/bin/bash         ## shell type
shopt -s extglob		## enable extended globbing

##===========================================================================
## Some of the models cut one ensemble member into several files, which include data of different time periods.
## We'd better concatenate them into one at the beginning so that we won't have to think about which files we need if we want to retrieve a specific time period later.
##
## Method:
##	- Make sure 'time' is the record dimension (i.e., left-most)
##	- ncrcat
##
## Input files like:
## /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_185001-190012.nc
## /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_190101-200512.nc
## 
## Output files like:
## /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_185001-200512.nc
##
## Online: http://nco.sourceforge.net/nco.html#Combine-Files
##
## Execute this script: bash combine_file.sh
##===========================================================================

drc_in='/home/wenshanw/data/cmip5/'		## directory of input files

var=( 'snc' 'snd' )										## variables
rlm='LImon'														## realm
xpt=( 'historical' )									## experiment ( could be more )

for var_id in {0..1}; do	## loop over two variables
  ## names of all the models (ls [get file names]; cut [get the part for model names]; sort; uniq [remove duplicates]; awk [print])
  mdl_set=$( ls ${drc_in}${var[var_id]}_${rlm}_*_${xpt[0]}_*.nc | cut -d '_' -f 3 | sort | uniq -c | awk '{print $2}' )
  ## number of models (echo [print contents]; wc [count])
  mdl_nbr=$( echo ${mdl_set} | wc -w )
  echo "=============================="
  echo "There are" ${mdl_nbr} "models for" ${var[var_id]}.
  
  for mdl in ${mdl_set}; do	## loop over models
    ## names of all the ensemble members
    nsm_set=$( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*.nc | cut -d '_' -f 5 | sort | uniq -c | awk '{print $2}' )
    nsm_nbr=$( echo ${nsm_set} | wc -w )		## number of ensemble members in each model
    echo "------------------------------"
    echo "Model" ${mdl} "includes" ${nsm_nbr} "ensemble member(s):"
    echo ${nsm_set}"."
    
    for nsm in ${nsm_set}; do	## loop over ensemble members
      fl_nbr=$( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_*.nc | wc -w ) ## number of files in this ensemble member
      
      if [ ${fl_nbr} -le 1 ]			## if there is only 1 file, continue to next loop
      then
      	echo "There is only 1 file in" ${nsm}.
      	continue
      fi
      
      echo "There are" ${fl_nbr} "files in" ${nsm}.
      
      ## starting date of data (sed [the name of the first file includes the starting date])
      yyyymm_str=$( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_*.nc | sed -n '1p' | cut -d '_' -f 6 | cut -d '-' -f 1 )
      ## ending date of data (sed [the name of the last file includes the ending date])
      yyyymm_end=$( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_*.nc | sed -n "${fl_nbr}p" | cut -d '_' -f 6 | cut -d '-' -f 2 )
      
      ## concatenate the files of one ensemble member into one along the record dimension (now is time)
      ncrcat -O ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_*.nc ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_${yyyymm_str}-${yyyymm_end}
      
      ## remove useless files
      rm ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_!(${yyyymm_str}-${yyyymm_end})
    done
  done
done
