#!/bin/bash

##============================================================
## Example for
##	- regrid (using bi_interp.nco): the spatial resolution of MODIS data 
##		is much finer than those of CMIP5 models. In order to compare
##		the two, we can regrid MODIS data to comform to CMIP5.
##	- add coordinates (using coor.nco): there is no coordinate information
##		in MODIS data. We have to add it manually now.
##
## Input files:
## /modis/mcd43c3/cesm-grid/MCD43C3.2000049.regrid.nc
##
## Output files:
## /modis/mcd43c3/cesm-grid/MCD43C3.2000049.regrid.nc
##
## Online: http://nco.sourceforge.net/nco.html#Add-Coordinates-to-MODIS-Data
##
## Execute this script: bash add_coor.sh
##============================================================

var=( 'MOD10CM' )							## variable
fld_in=( 'snc/nc/' )					## folder of input files
drc_in='/media/grele_data/wenshan/modis/'				## directory of input files

for fn in $( ls ${drc_in}${fld_in}${var}*.nc ); do		## loop over files
  sfx=$( echo ${fn} | cut -d '/' -f 8 | cut -d '.' -f 2-4 )				## part of file names
  echo ${sfx} 
  
  ## rename dimension names since they are too long
  ncrename -d YDim_MOD_CMG_Snow_5km,lat -d XDim_MOD_CMG_Snow_5km,lon -O ${drc_in}${fld_in}${var}.${sfx}.nc ${drc_in}${fld_in}${var}.${sfx}.nc
  ## add coordinates
  ncap2 -O -S coor.nco ${drc_in}${fld_in}${var}.${sfx}.nc ${drc_in}${fld_in}${var}.${sfx}.nc
done
