:: bld.bat
:: Windows script to build NCO on the command line, using Visual Studio 
:: 1) clones all NCO dependecies 
:: 2) cmake builds all NCO dependencies obtained with 'git clone'
:: 3) cmake builds NCO
:: 4) tests the netcdf build by reading a remote file with the built ncdump
:: 5) tests the NCO build by using the built ncks to read 2 files (in.nc, in_grp.nc)
:: and dumping content to a text file; this text file is compared using the FC tool with pre-existing output (generated in Linux)
:: Notes:
:: 1) must be run from ~/nco/cmake
:: 2) requires manual editing of the 'hdf5' and 'netcdf' generated Visual Studio projects,
:: so that dependencies ZLIB and SZIP are detected. This is explained in ~/nco/cmake/README.md
:: 3) the existence of a NCO build is tested with the existence of a built ncks with
:: if exist %build%\Debug\ncks.exe
:: to force another cmake build, '%build%\Debug\ncks.exe' can be deleted and retyping
:: $bld.bat
:: Pedro Vicente

@echo off
if not defined DevEnvDir (
 call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
 if errorlevel 1 goto :eof
)
set MSVC_VERSION="Visual Studio 16 2019"
echo using %MSVC_VERSION%

if "%~1" == "crt" (
  set STATIC_CRT=ON
) else (
  set STATIC_CRT=OFF
)
echo using static crt %STATIC_CRT%

:: 'git clone' all the dependencies
:: the build folder is 'build'
:: git clone https://github.com/Unidata/netcdf-c
:: git clone https://github.com/curl/curl
:: git clone https://github.com/madler/zlib
:: git clone https://github.com/nextgis-borsch/lib_szip szip
:: git clone https://github.com/HDFGroup/hdf5
:: git clone https://github.com/Unidata/UDUNITS-2
:: git clone https://github.com/libexpat/libexpat
:: git clone https://github.com/ampl/gsl
:: git clone https://github.com/nco/antlr2

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

if not exist %build%\netcdf-c (
 git clone https://github.com/nco/netcdf-c
 pushd netcdf-c
 git checkout Branch_v4.6.0
 popd
) else (
 echo skipping netcdf git clone
)

if not exist %build%\curl (
 git clone https://github.com/curl/curl
) else (
 echo skipping curl git clone
)

if not exist %build%\zlib (
 git clone https://github.com/madler/zlib
) else (
 echo skipping zlib git clone
)

if not exist %build%\szip (
 git clone https://github.com/nextgis-borsch/lib_szip szip
) else (
 echo skipping szip git clone
)

if not exist %build%\hdf5 (
 git clone https://github.com/HDFGroup/hdf5
) else (
 echo skipping hdf5 git clone
)

:: use tag version that builds
if not exist %build%\UDUNITS-2 (
 git clone https://github.com/Unidata/UDUNITS-2
 pushd UDUNITS-2
 git checkout tags/v2.2.27.6
 popd
) else (
 echo skipping UDUNITS-2 git clone
)

if not exist %build%\libexpat (
 git clone https://github.com/libexpat/libexpat
) else (
 echo skipping libexpat git clone
)

if not exist %build%\gsl (
 git clone https://github.com/ampl/gsl
) else (
 echo skipping gsl git clone
)

if not exist %build%\antlr2 (
 git clone https://github.com/nco/antlr2
) else (
 echo skipping antlr2 git clone
)

:: //////////////////////////////////////////////////////////
:: zlib
:: //////////////////////////////////////////////////////////

:build_zlib
if exist %build%\zlib\build\zlib.sln (
 echo skipping zlib build
 goto build_szip
) else (
  echo building zlib
  pushd zlib
  mkdir build
  pushd build
  cmake .. -G %MSVC_VERSION% ^
           -A x64 ^
           -DMSVC_USE_STATIC_CRT=%STATIC_CRT% ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DBUILD_SHARED_LIBS=OFF
  msbuild zlib.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  cp %build%\zlib\build\zconf.h %build%\zlib
  popd
  popd
  if errorlevel 1 goto :eof
)

:: //////////////////////////////////////////////////////////
:: szip
:: //////////////////////////////////////////////////////////

:build_szip
if exist %build%\szip\build\SZIP.sln (
 echo skipping szip build
 goto build_hdf5
) else (
  echo building szip
  pushd szip
  mkdir build
  pushd build
  cmake .. -G %MSVC_VERSION% ^
           -A x64 ^
           -DMSVC_USE_STATIC_CRT=%STATIC_CRT% ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DBUILD_TESTING=OFF
  msbuild SZIP.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  popd
  popd
  if errorlevel 1 goto :eof
)

:: //////////////////////////////////////////////////////////
:: hdf5
:: //////////////////////////////////////////////////////////

:build_hdf5
if exist %build%\hdf5\build\bin\Debug\h5dump.exe (
 echo skipping hdf5 build
 goto build_curl
) else (
  echo building hdf5
  pushd hdf5
  mkdir build
  pushd build
  cmake .. -G %MSVC_VERSION% ^
           -A x64 ^
           -DBUILD_STATIC_CRT_LIBS=%STATIC_CRT% ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DBUILD_STATIC_EXECS=ON ^
           -DBUILD_TESTING=OFF ^
           -DHDF5_BUILD_EXAMPLES=OFF ^
           -DHDF5_BUILD_CPP_LIB=OFF ^
           -DHDF5_ENABLE_Z_LIB_SUPPORT=ON ^
           -DH5_ZLIB_HEADER=%root%/zlib/zlib.h ^
           -DZLIB_STATIC_LIBRARY:FILEPATH=%root%/zlib/build/Debug/zlibstaticd.lib ^
           -DZLIB_INCLUDE_DIRS:PATH=%root%/zlib ^
           -DHDF5_ENABLE_SZIP_SUPPORT=ON ^
           -DSZIP_USE_EXTERNAL=ON ^
           -DSZIP_FOUND=ON ^
           -DSZIP_STATIC_LIBRARY:FILEPATH=%root%/szip/build/bin/Debug/szip.lib ^
           -DSZIP_INCLUDE_DIRS:PATH=%root%/szip/src
  msbuild HDF5.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  popd
  popd
  if errorlevel 1 goto :eof
)

:: //////////////////////////////////////////////////////////
:: curl
:: //////////////////////////////////////////////////////////

:build_curl
if exist %build%\curl\builds\libcurl-vc14-x64-debug-static-ipv6-sspi-winssl\lib\libcurl_a_debug.lib (
 echo skipping curl build
 goto build_expat
) else (
  echo building curl
  pushd curl
  call buildconf.bat
  pushd winbuild
  @echo on
  if %STATIC_CRT% == ON (
   nmake /f Makefile.vc mode=static vc=14 debug=yes gen_pdb=yes MACHINE=x64 ENABLE_WINSSL=yes RTLIBCFG=static
  ) else (
   nmake /f Makefile.vc mode=static vc=14 debug=yes gen_pdb=yes MACHINE=x64 ENABLE_WINSSL=yes
  )
  @echo off
  popd
  popd
  if errorlevel 1 goto :eof
)

:: //////////////////////////////////////////////////////////
:: expat
:: //////////////////////////////////////////////////////////

:build_expat
if exist %build%\libexpat\expat\build\expat.sln (
 echo skipping expat build
 goto build_udunits
) else (
  echo building expat
  pushd libexpat
  pushd expat
  mkdir build
  pushd build
  cmake .. -G %MSVC_VERSION% ^
           -A x64 ^
           -DMSVC_USE_STATIC_CRT=%STATIC_CRT% ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DBUILD_shared=OFF
  msbuild expat.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  popd
  popd
  popd
  if errorlevel 1 goto :eof
)

:: //////////////////////////////////////////////////////////
:: udunits
:: //////////////////////////////////////////////////////////

:build_udunits
if exist %build%\UDUNITS-2\build\lib\Debug\udunits2.lib (
 echo skipping udunits build
 goto build_gsl
) else (
  echo building udunits
  pushd UDUNITS-2
  mkdir build
  pushd build
  cmake .. -G %MSVC_VERSION% ^
           -A x64 ^
           -DMSVC_USE_STATIC_CRT=%STATIC_CRT% ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DEXPAT_INCLUDE_DIR=%root%/libexpat/expat/lib ^
           -DEXPAT_LIBRARY=%root%/libexpat/expat/build/Debug/expatd.lib
  msbuild udunits.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  popd
  popd
  if errorlevel 1 goto :eof
)

:: //////////////////////////////////////////////////////////
:: gsl
:: //////////////////////////////////////////////////////////

:build_gsl
if exist %build%\GSL\build\GSL.sln (
 echo skipping gsl build
 goto build_antlr
) else (
  echo building gsl
  pushd GSL
  mkdir build
  pushd build
  if %STATIC_CRT%==ON (
  set DYNAMIC_CRT=OFF
  ) else (
  set DYNAMIC_CRT=ON
  )
  echo using dynamic CRT %DYNAMIC_CRT%
  cmake .. -G %MSVC_VERSION% ^
           -A x64 ^
           -DMSVC_RUNTIME_DYNAMIC=%DYNAMIC_CRT% ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DGSL_DISABLE_TESTS=ON ^
           -DNO_AMPL_BINDINGS=ON
  msbuild GSL.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  popd
  popd
  if errorlevel 1 goto :eof
)

:: //////////////////////////////////////////////////////////
:: antlr
:: //////////////////////////////////////////////////////////

:build_antlr
if exist %build%\antlr2\lib\cpp\build\antlr.sln (
 echo skipping antlr build
 goto build_netcdf
) else (
  echo building antlr
  pushd antlr2
  pushd lib
  pushd cpp
  mkdir build
  pushd build
  cmake .. -G %MSVC_VERSION% ^
           -A x64 ^
           -DMSVC_USE_STATIC_CRT=%STATIC_CRT% ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DBUILD_SHARED_LIBS=OFF
  msbuild antlr.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  popd
  popd
  popd
  popd
  if errorlevel 1 goto :eof
)

:: //////////////////////////////////////////////////////////
:: netcdf
:: //////////////////////////////////////////////////////////

:build_netcdf
if exist %build%\netcdf-c\build\ncdump\ncdump.exe (
 echo skipping netcdf build
 goto test_netcdf
) else (
  echo building netcdf
  pushd netcdf-c
  mkdir build
  pushd build
  cmake .. -G %MSVC_VERSION% ^
           -A x64 ^
           -DNC_USE_STATIC_CRT=%STATIC_CRT% ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DENABLE_TESTS=OFF ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DHDF5_HL_LIBRARY=%root%/hdf5/build/bin/Debug/libhdf5_hl_D.lib ^
           -DHDF5_C_LIBRARY=%root%/hdf5/build/bin/Debug/libhdf5_D.lib ^
           -DHDF5_INCLUDE_DIR=%root%/hdf5/src ^
           -DZLIB_LIBRARY:FILE=%root%/zlib/build/Debug/zlibstaticd.lib ^
           -DSZIP_LIBRARY:FILE=%root%/szip/build/bin/Debug/szip.lib ^
           -DZLIB_INCLUDE_DIR:PATH=%root%/zlib ^
           -DHAVE_HDF5_H=%root%/hdf5/build ^
           -DHDF5_HL_INCLUDE_DIR=%root%/hdf5/hl/src ^
           -DCURL_LIBRARY=%root%/curl/builds/libcurl-vc14-x64-debug-static-ipv6-sspi-winssl/lib/libcurl_a_debug.lib ^
           -DCURL_INCLUDE_DIR=%root%/curl/include
  msbuild netcdf.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  popd
  popd
  if errorlevel 1 goto :eof
)

:test_netcdf
if exist %build%\netcdf-c\build\ncdump\ncdump.exe (
 echo testing netcdf build
 @echo on
 %build%\netcdf-c\build\ncdump\ncdump.exe -h http://www.esrl.noaa.gov/psd/thredds/dodsC/Datasets/cmap/enh/precip.mon.mean.nc
 @echo off
 @echo.
 goto build_nco
)

:: //////////////////////////////////////////////////////////
:: NCO
:: use undocumented option -H (location of CMakeLists.txt)
:: //////////////////////////////////////////////////////////


:build_nco
if exist %build%\Debug\ncks.exe (
 echo skipping nco build
 goto test_nco
) else (
  echo building NCO
  rm -rf CMakeCache.txt CMakeFiles
  cmake .. -H..\.. ^
   -G %MSVC_VERSION% ^
  -A x64 ^
  -DMSVC_DEVELOPER=ON ^
  -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
  -DMSVC_USE_STATIC_CRT=%STATIC_CRT% ^
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
  msbuild nco.sln /target:build /property:configuration=debug /nologo /verbosity:minimal
  if errorlevel 1 goto :eof
)

:test_nco
set data=%root_win%\..\data
@echo on
%build%\Debug\ncks.exe -v lat http://www.esrl.noaa.gov/psd/thredds/dodsC/Datasets/cmap/enh/precip.mon.mean.nc
%build%\netcdf-c\build\ncgen\ncgen.exe -k netCDF-4 -b -o %data%\in_grp.nc %data%\in_grp.cdl
%build%\netcdf-c\build\ncgen\ncgen.exe -k netCDF-4 -b -o %data%\in.nc %data%\in.cdl
%build%\Debug\ncks.exe --jsn_fmt 2 -C -g g10 -v two_dmn_rec_var %data%\in_grp.nc

:: generation of comparison files in UNIX done with
:: pvicente@glace:~/nco/data$ ../cmake/ncks in_grp.nc > in_grp.nc.txt
:: pvicente@glace:~/nco/data$ ../cmake/ncks in.nc > in.nc.txt
:: generate text files in_grp.nc.gen.txt from in_grp.nc and in.nc and use tool FC to compare contents 
:: with pre-existing in_grp.nc.txt, in.nc.txt, generated in Linux
:: generation of in_grp.nc.gen.txt must be done in 'data' folder so that paths match

@pushd %data%
%build%\Debug\ncks.exe in_grp.nc > %build%\in_grp.nc.gen.txt 
%build%\Debug\ncks.exe in.nc > %build%\in.nc.gen.txt 
@popd
@popd
fc %build%\in_grp.nc.gen.txt in_grp.nc.txt
fc %build%\in.nc.gen.txt in.nc.txt
echo done