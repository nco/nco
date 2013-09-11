#!/bin/bash

##===========================================================================
## Example for
##	- permute coordinates (using inverse-lat.nco): the grid of MODIS is 
##		from (-180 degE, 90 degN), the left-up corner, to
##		(180 degE, -90 degN), the right-low corner. However, CMIP5 is
##		from (0 degE, -90 degN) to (360 degE, 90 degN). The script
##		here changes the MODIS grid to CMIP5 grid.
##
## Input files:
## /modis/mcd43c3/cesm-grid/MCD43C3.2000049.regrid.nc
##
## Output files:
## /modis/mcd43c3/cesm-grid/MCD43C3.2000049.regrid.nc
##
## Online: http://nco.sourceforge.net/nco.html#Permute-MODIS-Coordinates
##
## Execute this script: bash permute_coor.sh
##===========================================================================

##---------------------------------------------------------------------------
## permute coordinates
##	- inverse lat from (90,-90) to (-90,90)
##	- permute lon from (-180,180) to (0,360)
for fn in $( ls MCD43C3.*.nc ); do		## loop over files
  sfx=$( echo ${fn} | cut -d '.' -f 1-3 )						## part of file names
  echo ${sfx}
  
  ## lat
  ncap2 -O -S inverse-lat.nco ${fn} ${fn}						## inverse latitude
  
  ## lon
  ncks -O --msa -d lon,0.0,180.0 -d lon,-180.0,-1.25 ${fn} ${fn}

  ## add new longitude coordinates
  ncap2 -O -s 'lon=array(0.0,1.25,$lon)' ${fn} ${fn}
done
