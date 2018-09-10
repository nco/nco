mkdir %SRC_DIR%\build
cd %SRC_DIR%\build

set "CFLAGS=%CFLAGS% -DWIN32 -DGSL_DLL"
set "CXXFLAGS=%CXXFLAGS% -DWIN32 -DGSL_DLL"

:: We need to define these b/c we are not using conda-build.
set LIBRARY_PREFIX=%CONDA_PREFIX%\Library
set LIBRARY_INC=%CONDA_PREFIX%\Library\include
set LIBRARY_LIB=%CONDA_PREFIX%\Library\lib
set LIBRARY_BIN=%CONDA_PREFIX%\Library\bin

cmake -G "NMake Makefiles" ^
      -D CMAKE_INSTALL_PREFIX=%LIBRARY_PREFIX% ^
      -D CMAKE_BUILD_TYPE=Release ^
      -D MSVC_USE_STATIC_CRT=OFF ^
      -D CMAKE_PREFIX_PATH=%LIBRARY_PREFIX% ^
      -D NETCDF_INCLUDE=%LIBRARY_INC% ^
      -D NETCDF_LIBRARY=%LIBRARY_LIB%\netcdf.lib ^
      -D HDF5_LIBRARY=%LIBRARY_LIB%\libhdf5.lib ^
      -D HDF5_HL_LIBRARY=%LIBRARY_LIB%\libhdf5_hl.lib ^
      -D GSL_INCLUDE=%LIBRARY_INC% ^
      -D GSL_LIBRARY=%LIBRARY_LIB%\gsl.lib ^
      -D GSL_CBLAS_LIBRARY=%LIBRARY_LIB%\gslcblas.lib ^
      -D UDUNITS2_INCLUDE=%LIBRARY_LIB% ^
      -D UDUNITS2_LIBRARY=%LIBRARY_LIB%\udunits2.lib ^
      -D EXPAT_LIBRARY=%LIBRARY_LIB%\expat.lib ^
      -D CURL_LIBRARY=%LIBRARY_LIB%\libcurl.lib ^
      -D ANTLR_INCLUDE:PATH=%LIBRARY_INC%\antlr ^
      %SRC_DIR%
if errorlevel 1 exit 1


nmake || exit 1
nmake install || exit 1

move %LIBRARY_PREFIX%\*.exe %LIBRARY_BIN% || exit 1
