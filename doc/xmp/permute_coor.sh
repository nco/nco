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
  ## break into east and west hemispheres in order to switch the two
  ncks -O -d lon,0.0,180.0 ${fn} ${sfx}.part1.nc
  ncks -O -d lon,-180.0,-1.25 ${fn} ${sfx}.part2.nc
  ## make longitude the record dimension
  ncpdq -O -a lon,lat,time ${sfx}.part1.nc ${sfx}.part1.nc
  ncpdq -O -a lon,lat,time ${sfx}.part2.nc ${sfx}.part2.nc
  ## concatenate the two hemispheres along longitude
  ncrcat -O ${sfx}.part?.nc ${fn}
  ## reorder dimensions
  ncpdq -O -a time,lat,lon ${fn} ${fn}
  ## add new longitude coordinates
  ncap2 -O -s 'lon=array(0.0,1.25,$lon)' ${fn} ${fn}
done
