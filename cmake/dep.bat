:: cmake build all NCO dependencies obtained with 'git clone' using nmake
:: Pedro Vicente

@echo off
@call "%VS140COMNTOOLS%VsDevCmd.bat" amd64

:: replace the character string '\' with '/' needed for cmake
set temp=%cd%
set root=%temp:\=/%
echo root is %root%

pushd zlib
if exist build\Makefile (
 echo skipping zlib build
 popd
 goto build_hdf5
) else (
  echo building zlib
  mkdir build
  pushd build
  cmake .. -G "NMake Makefiles" ^
           -DCMAKE_BUILD_TYPE=Debug ^
           -DBUILD_SHARED_LIBS=OFF
  nmake
  popd
  popd
)

:build_hdf5
pushd hdf5
if exist build\Makefile (
 echo skipping hdf5 build
 popd
 goto build_curl
) else (
  echo building hdf5
  mkdir build
  pushd build
  cmake .. -G "NMake Makefiles" ^
           -DCMAKE_BUILD_TYPE=Debug ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DBUILD_STATIC_EXECS=ON ^
           -DBUILD_TESTING=OFF ^
           -DHDF5_BUILD_EXAMPLES=OFF ^
           -DHDF5_BUILD_CPP_LIB=OFF
  nmake
  popd
  popd
)

:build_curl
pushd curl
if exist build\Makefile (
 echo skipping curl build
 popd
 goto build_netcdf
) else (
  echo building curl
  mkdir build
  pushd build
  cmake .. -G "NMake Makefiles" ^
           -DCMAKE_BUILD_TYPE=Debug ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DBUILD_TESTING=OFF
  nmake
  popd
  popd
)

:build_netcdf
pushd netcdf-c
if exist build\ncdump\ncdump.exe (
 echo skipping netcdf build
 popd
 goto test_netcdf
) else (
  echo building netcdf
  mkdir build
  pushd build
  cmake .. -G "NMake Makefiles" ^
           -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
           -DENABLE_TESTS=OFF ^
           -DCMAKE_BUILD_TYPE=Debug ^
           -DBUILD_SHARED_LIBS=OFF ^
           -DHDF5_HL_LIBRARY=%root%/hdf5/build/bin/libhdf5_hl_D.lib ^
           -DHDF5_C_LIBRARY=%root%/hdf5/build/bin/libhdf5_D.lib ^
           -DHDF5_INCLUDE_DIR=%root%/hdf5/src ^
           -DZLIB_LIBRARY:FILE=%root%/zlib/build/zlibstaticd.lib ^
           -DZLIB_INCLUDE_DIR:PATH=%root%/zlib ^
           -DHAVE_HDF5_H=%root%/hdf5/build ^
           -DHDF5_HL_INCLUDE_DIR=%root%/hdf5/hl/src ^
           -DCURL_LIBRARY=%root%/curl/build/lib/libcurl-d_imp.lib ^
           -DCURL_INCLUDE_DIR=%root%/curl/include
  nmake
  popd
  popd
)

:test_netcdf
if exist %root%\netcdf-c\build\ncdump\ncdump.exe (
 echo testing netcdf build
 pushd netcdf-c
 pushd build
 pushd ncdump
 cp %root%\curl\build\lib\libcurl-d.dll %root%\netcdf-c\build\ncdump
 ncdump http://www.esrl.noaa.gov/psd/thredds/dodsC/Datasets/cmap/enh/precip.mon.mean.nc
 popd
 popd
 popd
 goto build_nco
)

:build_nco
cmake .. ^
-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON ^
-DMSVC_USE_STATIC_CRT=OFF ^
-DNETCDF_INCLUDE:PATH=%root%/netcdf-c/include ^
-DNETCDF_LIBRARY:FILE=%root%/netcdf-c/build/liblib/netcdf.lib ^
-DHDF5_LIBRARY:FILE=%root%/hdf5/build/bin/libhdf5_D.lib ^
-DHDF5_HL_LIBRARY:FILE=%root%/hdf5/build/bin/libhdf5_hl_D.lib ^
-DZLIB_LIBRARY:FILE=%root%/zlib/build/zlibstaticd.lib ^
-DCURL_LIBRARY:FILE=%root%/curl/build/lib/libcurl-d_imp.lib