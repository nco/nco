@echo off
if not defined DevEnvDir (
 call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
 if errorlevel 1 goto :eof
)
set MSVC_VERSION="Visual Studio 16 2019"
echo using %MSVC_VERSION%

:: current place
set root_win=%cd%
set build=%root_win%\build

if not exist %build% (
 mkdir %build%
 echo cloning/building in %build%
) else (
 echo skipping mkdir %build%
)

:: change to build place 
:: replace the character string '\' with '/' needed for cmake
pushd %build%
set tmp=%cd%
set root=%tmp:\=/%
echo cmake root is %root%

echo building NCO
rm -rf CMakeCache.txt CMakeFiles
cmake .. -H..\.. ^
--debug-trycompile ^
-G %MSVC_VERSION% ^
-A x64 ^
-DMSVC_DEVELOPER=ON ^
-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
-DMSVC_USE_STATIC_CRT=ON ^
-DNETCDF_INCLUDE:PATH=%root%/netcdf-c/include ^
-DNETCDF_LIBRARY:FILE=%root%/netcdf-c/build/liblib/Debug/netcdf.lib ^
-DHDF5_LIBRARY:FILE=%root%/hdf5/build/bin/Debug/libhdf5_D.lib ^
-DHDF5_HL_LIBRARY:FILE=%root%/hdf5/build/bin/Debug/libhdf5_hl_D.lib ^
-DZLIB_LIBRARY:FILE=%root%/zlib/build/Debug/zlibstaticd.lib ^
-DSZIP_LIBRARY:FILE=%root%/szip/build/bin/Debug/szip.lib ^
-DCURL_LIBRARY:FILE=%root%/curl/builds/libcurl-vc14-x64-debug-static-ipv6-sspi-winssl/lib/libcurl_a_debug.lib ^
-DUDUNITS2_INCLUDE:PATH=%root%/UDUNITS-2/lib ^
-DUDUNITS2_LIBRARY:FILE=%root%/UDUNITS-2/build/lib/Debug/udunits2.lib ^
-DEXPAT_LIBRARY:FILE=%root%/libexpat/expat/build/Debug/expatd.lib ^
-DGSL_INCLUDE:PATH=%root%/gsl/build ^
-DGSL_LIBRARY:FILE=%root%/gsl/build/Debug/gsl.lib ^
-DGSL_CBLAS_LIBRARY:FILE=%root%/gsl/build/Debug/gslcblas.lib ^
-DANTLR_INCLUDE:PATH=%root%/antlr2/lib/cpp ^
-DANTLR_LIBRARY:FILE=%root%/antlr2/lib/cpp/build/Debug/antlr.lib 