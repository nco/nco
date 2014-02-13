#!/bin/bash      # shell type
shopt -s extglob # enable extended globbing

#===========================================================================
# Some of the models cut one ensemble member into several files, 
#  which include data of different time periods.
# We'd better concatenate them into one at the beginning so that 
#  we won't have to think about which files we need if we want 
#  to retrieve a specific time period later.
#
# Method:
#	- Make sure 'time' is the record dimension (i.e., left-most)
#	- ncrcat
#
# Input files like:
# /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_185001-190012.nc
# /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_190101-200512.nc
# 
# Output files like:
# /data/cmip5/snc_LImon_bcc-csm1-1_historical_r1i1p1_185001-200512.nc
#
# Online: http://nco.sourceforge.net/nco.html#Combine-Files
#
# Execute this script: bash cmb_fl.sh
#===========================================================================

drc_in='/home/wenshanw/data/cmip5/' # Directory of input files

var=( 'snc' 'snd' )                 # Variables
rlm='LImon'                         # Realm
xpt=( 'historical' )                # Experiment ( could be more )

for var_id in {0..1}; do            # Loop over two variables
  # Names of all the models (ls [get file names]; 
  #  cut [get model names]; 
  #  sort; uniq [remove duplicates]; awk [print])
  mdl_set=$( ls ${drc_in}${var[var_id]}_${rlm}_*_${xpt[0]}_*.nc | \
    cut -d '_' -f 3 | sort | uniq -c | awk '{print $2}' )
  # Number of models (echo [print contents]; wc [count])
  mdl_nbr=$( echo ${mdl_set} | wc -w )
  echo "=============================="
  echo "There are" ${mdl_nbr} "models for" ${var[var_id]}.
  
  for mdl in ${mdl_set}; do	        # Loop over models
    # Names of all the ensemble members
    nsm_set=$( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_*.nc | \
      cut -d '_' -f 5 | sort | uniq -c | awk '{print $2}' )
    # Number of ensemble members in each model
    nsm_nbr=$( echo ${nsm_set} | wc -w )		
    echo "------------------------------"
    echo "Model" ${mdl} "includes" ${nsm_nbr} "ensemble member(s):"
    echo ${nsm_set}"."
    
    for nsm in ${nsm_set}; do	      # Loop over ensemble members
      # Number of files in this ensemble member
      fl_nbr=$( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_*.nc \
        | wc -w ) 
      
      # If there is only 1 file, continue to next loop
      if [ ${fl_nbr} -le 1 ]			
      then
      	echo "There is only 1 file in" ${nsm}.
      	continue
      fi
      
      echo "There are" ${fl_nbr} "files in" ${nsm}.
      
      # Starting date of data 
      #   (sed [the name of the first file includes the starting date])
      yyyymm_str=$( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_*.nc\
        | sed -n '1p' | cut -d '_' -f 6 | cut -d '-' -f 1 )
      # Ending date of data 
      #   (sed [the name of the last file includes the ending date])
      yyyymm_end=$( ls ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_*.nc\
        | sed -n "${fl_nbr}p" | cut -d '_' -f 6 | cut -d '-' -f 2 )
      
      # Concatenate one ensemble member files 
      #   into one along the record dimension (now is time)
      ncrcat -O ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_*.nc \
        ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_\
        ${nsm}_${yyyymm_str}-${yyyymm_end}
      
      # Remove useless files
      rm ${drc_in}${var[var_id]}_${rlm}_${mdl}_${xpt[0]}_${nsm}_\
        !(${yyyymm_str}-${yyyymm_end})
    done
  done
done
