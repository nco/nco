cmake .. ^
-DNETCDF_INCLUDE:PATH=I:/grafic_project/netcdf/include ^
-DNETCDF_LIBRARY:FILE=I:/grafic_project/Debug/netcdf.lib ^
-DHDF5_LIBRARY:FILE=I:/grafic_project/Debug/hdf5.1.8.5.patch1.lib ^
-DHDF5_HL_LIBRARY:FILE=I:/grafic_project/Debug/hdf5_hl.1.8.5.patch1.lib ^
-DZLIB_LIBRARY:FILE=I:/grafic_project/Debug/zlib.lib ^
-DSZIP_LIBRARY:FILE=I:/grafic_project/Debug/szlib.lib ^
-DCURL_LIBRARY:FILE=I:/grafic_project/Debug/libcurld.lib ^
-DANTLR_INCLUDE:PATH=I:/antlr-2.7.7/lib/cpp ^
-DANTLR_LIBRARY:FILE=I:/antlr-2.7.7/lib/cpp/debug/antlr.lib ^
-DUDUNITS2_INCLUDE:PATH=I:/udunits2/lib ^
-DUDUNITS2_LIBRARY:FILE=I:/udunits2/build/lib/Debug/udunits2.lib ^
-DEXPAT_LIBRARY:FILE=I:/expat/expat/win32/bin/Debug/libexpatMT.lib ^
-DGSL_INCLUDE:PATH=I:/gsl-1.8/src ^
-DGSL_LIBRARY:FILE=I:/gsl-1.8/src/gsl/1.8/gsl-1.8/VC8/libgsl/Debug-StaticLib/libgsl_d.lib ^
-DGSL_CBLAS_LIBRARY:FILE=I:/gsl-1.8/src/gsl/1.8/gsl-1.8/VC8/libgslcblas/Debug-StaticLib/libgslcblas_d.lib
msbuild nco.sln /target:build /property:configuration=debug


