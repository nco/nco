cd data

:: ncgen -o in.nc in.cdl || exit 1
:: if errorlevel 1 exit 1

ncks -H --trd -v one in.nc
if errorlevel 1 exit 1

ncap2 -O -v -s "erf_one=float(gsl_sf_erf(1.0f));" in.nc foo.nc
if errorlevel 1 exit 1

ncks -v erf_one foo.nc
if errorlevel 1 exit 1