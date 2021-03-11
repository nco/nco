#CMake build, Pedro Vicente

#/////////////////////////////////////////////////////////////////////////////////////
#mininum library dependencies: netCDF, HDF5, HDF5_HL, ZLIB (SZIP)
#/////////////////////////////////////////////////////////////////////////////////////

#-DNETCDF_INCLUDE:PATH=<some_path>
#-DNETCDF_LIBRARY:FILE=<some_file>
#-DHDF5_LIBRARY:FILE=<some_file>
#-DHDF5_HL_LIBRARY:FILE=<some_file>
#-DZLIB_LIBRARY:FILE=<some_file>
#-DSZIP_LIBRARY:FILE=<some_file>
#-DCURL_LIBRARY:FILE=<some_file>

#/////////////////////////////////////////////////////////////////////////////////////
#optional library dependencies (Antlr, UDUNITS, EXPAT, GSL)
#/////////////////////////////////////////////////////////////////////////////////////

#-DANTLR_INCLUDE:PATH=<some_path>
#-DANTLR_LIBRARY:FILE=<some_file>
#-DUDUNITS2_INCLUDE:PATH=<some_path>
#-DUDUNITS2_LIBRARY:FILE=<some_file>
#-DEXPAT_LIBRARY:FILE=<some_file>
#-DGSL_INCLUDE:PATH=<some_path>
#-DGSL_LIBRARY:FILE=<some_file>
#-DGSL_CBLAS_LIBRARY:FILE=<some_file>

cmake_minimum_required(VERSION 3.12)
project (nco)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckLibraryExists)

#//////////////////////////
# colorized output
# red, fatal error, cannot build (missing netCDF, HDF5, CURL libraries)
# magenta, "negative" warning (missing optional libraries, GSL, ANTLR, UDUNITS)
# green, "positive" warning (opposite of magenta, optional libraries found)
# blue, general information to take notice (SZIP/ZLIB were detected as needed, special NCO functions are defined)
#//////////////////////////

if(NOT WIN32)
  string(ASCII 27 Esc)
  set(color_reset   "${Esc}[m")
  set(color_magenta "${Esc}[35m")
  set(color_red     "${Esc}[31m")
  set(color_green   "${Esc}[32m")
  set(color_blue    "${Esc}[34m")
endif()

#/////////////////////////////////////////////////////////////////////////////////////
#general C/C++ build flags
#on single-configuration generators (Unix Makefiles, NMake Makefiles) use Debug as default
#on multi-configuration generators (Visual Studio, Xcode) define only 2 types: Debug;Release
#/////////////////////////////////////////////////////////////////////////////////////

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build.")
if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libs.")
if (NOT BUILD_SHARED_LIBS)
  add_definitions(-DENABLE_STATIC)
endif()

#//////////////////////////
#detect minimum dependencies using
#find_path()
#find_library()
#Notes:
#1) list a static name first to avoid link errors in systems with multiple versions of the same libraries in PATH, used for libz.a
#2) use option NO_SYSTEM_ENVIRONMENT_PATH
#//////////////////////////

set(find_opt NO_SYSTEM_ENVIRONMENT_PATH)

#/////////////////////////////////////////////////////////////////////////////////////
#netCDF
#detect a user defined system environment variable NETCDF_ROOT
#this sets the include path to NETCDF_INCLUDE directly
#and a PATHS hint for NETCDF_LIBRARY (because the library name can be any legal combination of "netcdf" )
#For this case find_library(NETCDF_LIBRARY) MUST be called 2 times
#First time it looks in NETCDF_LIBRARY_PATH if set, avoiding all other PATHS with NO_DEFAULT_PATH
#If not found, business as usual
#//////////////////////////

# NETCDF_ROOT, if found in environment, will be searched after specific command line locations for NETCDF_INCLUDE
# and NETCDF_LIBRARY
set(netcdf_root "$ENV{NETCDF_ROOT}")
if(netcdf_root)
  message("${color_blue}-- Using user-defined netCDF root location: ${netcdf_root}${color_reset}")
endif()

#//////////////////////////
#a user defined -DNETCDF_INCLUDE was specified; check if it really exists :-)
#//////////////////////////

if(NETCDF_INCLUDE)
  if(EXISTS "${NETCDF_INCLUDE}/netcdf.h")
    message("-- Using ${NETCDF_INCLUDE}/netcdf.h")
  else()
    if(netcdf_root)
        set(NETCDF_INCLUDE "${netcdf_root}/include")
    endif()
    if(EXISTS "${NETCDF_INCLUDE}/netcdf.h")
      message("-- Using ${NETCDF_INCLUDE}/netcdf.h")
    else()
      message(FATAL_ERROR "${color_red}netcdf.h not found at ${NETCDF_INCLUDE}${color_reset}")
    endif()
  endif()
endif()

find_path(NETCDF_INCLUDE netcdf.h ${find_opt})
if(NOT NETCDF_INCLUDE)
  message(FATAL_ERROR "${color_red}netcdf.h header file not found${color_reset}")
else()
  message("-- Found netcdf.h header file at: " ${NETCDF_INCLUDE})
endif()

#//////////////////////////
#a user defined -DNETCDF_LIBRARY was specified; check if it really exists :-)
#//////////////////////////

if(NETCDF_LIBRARY)
  if(EXISTS "${NETCDF_LIBRARY}")
    message("-- Using ${NETCDF_LIBRARY}")
  else()
    if(netcdf_root)
        set(NETCDF_LIBRARY_PATH "${netcdf_root}/lib")
    else()
        message(FATAL_ERROR "${color_red}Specified ${NETCDF_LIBRARY} does not exist${color_reset}")
    endif()
  endif()
endif()

find_library(NETCDF_LIBRARY NAMES netcdf PATHS ${NETCDF_LIBRARY_PATH} NO_DEFAULT_PATH)
find_library(NETCDF_LIBRARY NAMES netcdf ${find_opt})
if(NOT NETCDF_LIBRARY)
  message(FATAL_ERROR "${color_red}netcdf library not found${color_reset}")
else()
  message("-- Found netcdf library at: " ${NETCDF_LIBRARY})
endif()

#/////////////////////////////////////////////////////////////////////////////////////
#netCDF
#Check for several functions in the netCDF library; define C macro accordingly
#CMAKE_REQUIRED_LIBRARIES needs NETCDF_LIBRARY
#check for header file netcdf_mem.h existence
#/////////////////////////////////////////////////////////////////////////////////////

check_library_exists(${NETCDF_LIBRARY} nc_set_log_level "" has_nc_set_log_level)
if (has_nc_set_log_level)
  message("-- Found nc_set_log_level in: " ${NETCDF_LIBRARY})
  add_definitions(-DHAVE_NC_SET_LOG_LEVEL)
endif()

check_library_exists(${NETCDF_LIBRARY} nc_rename_grp "" has_nc_rename_grp)
if (has_nc_rename_grp)
  message("-- Found nc_rename_grp in: " ${NETCDF_LIBRARY})
  add_definitions(-DHAVE_NC_RENAME_GRP)
endif()

check_library_exists(${NETCDF_LIBRARY} nc_inq_path "" has_nc_inq_path)
if (has_nc_inq_path)
  message("-- Found nc_inq_path in: " ${NETCDF_LIBRARY})
  add_definitions(-DHAVE_NC_INQ_PATH)
endif()

check_library_exists(${NETCDF_LIBRARY} nc_inq_format "" has_nc_inq_format)
if (has_nc_inq_format)
  message("-- Found nc_inq_format in: " ${NETCDF_LIBRARY})
  add_definitions(-DHAVE_NC_INQ_FORMAT)
endif()

#/////////////////////////////////////////////////////////////////////////////////////
#netcdf_meta.h defines NC_VERSION_MAJOR NC_VERSION_MINOR NC_VERSION_PATCH 
#NCO defines NC_LIB_VERSION ( NC_VERSION_MAJOR * 100 + NC_VERSION_MINOR * 10 + NC_VERSION_PATCH ) 
#/////////////////////////////////////////////////////////////////////////////////////

find_path(path_netcdf_meta_h netcdf_meta.h PATHS ${NETCDF_INCLUDE})
if(path_netcdf_meta_h)
  message("-- Found netcdf_meta.h in " ${path_netcdf_meta_h})
  add_definitions(-DNC_HAVE_META_H)
else()
  message("-- netcdf_meta.h was not found; defining NC_LIB_VERSION=360")
  add_definitions(-DNC_LIB_VERSION=360)
endif()

find_path(path_netcdf_mem_h netcdf_mem.h PATHS ${NETCDF_INCLUDE})
if (path_netcdf_mem_h)
  message("-- Found netcdf_mem.h in: " ${path_netcdf_mem_h})
endif()

check_library_exists(${NETCDF_LIBRARY} nc_open_mem "" has_nc_open_mem)
if (has_nc_open_mem)
  message("-- Found nc_open_mem in: " ${NETCDF_LIBRARY})
endif()

if (has_nc_open_mem AND path_netcdf_mem_h)
  add_definitions(-DHAVE_NETCDF_MEM_H)
  message("-- NetCDF diskless functionaliy enabled")
else()
  message("-- NetCDF diskless functionaliy not available, netcdf_mem.h or nc_open_mem() not found")
endif()

#if nc_def_var_filter() is available, nc_inq_path(), nc_inq_format() and has_nc_open_mem() are too
if (has_nc_def_var_filter)
  message("-- Defining HAVE_NC_INQ_PATH, HAVE_NC_INQ_FORMAT, HAVE_NETCDF_MEM_H")
  add_definitions(-DHAVE_NC_INQ_PATH)
  add_definitions(-DHAVE_NC_INQ_FORMAT)
  add_definitions(-DHAVE_NETCDF_MEM_H)
endif()

set(NETCDF_REQUIRED_LIBRARIES ${NETCDF_LIBRARY} ${HDF5_HL_LIBRARY} ${HDF5_LIBRARY} ${ZLIB_LIBRARY} ${SZIP_LIBRARY})
# Replace backslashes with forward slashes because it can cause problems with Windows' paths
string(REPLACE "\\" "/" NETCDF_REQUIRED_LIBRARIES ${NETCDF_REQUIRED_LIBRARIES})
set(CMAKE_REQUIRED_LIBRARIES ${NETCDF_REQUIRED_LIBRARIES})

#/////////////////////////////////////////////////////////////////////////////////////
#HDF5
#/////////////////////////////////////////////////////////////////////////////////////


find_library(HDF5_LIBRARY NAMES hdf5 PATHS "/usr/lib/x86_64-linux-gnu/hdf5/serial")
if(NOT HDF5_LIBRARY)
  message(FATAL_ERROR "${color_red}hdf5 library not found${color_reset}")
else()
  message("-- Found hdf5 library at: " ${HDF5_LIBRARY})
endif()

find_library(HDF5_HL_LIBRARY NAMES hdf5_hl PATHS "/usr/lib/x86_64-linux-gnu/hdf5/serial")
if(NOT HDF5_HL_LIBRARY)
  message(FATAL_ERROR "${color_red}hdf5 high level library not found${color_reset}")
else()
  message("-- Found hdf5 high level library at: " ${HDF5_HL_LIBRARY})
endif()

#//////////////////////////
#HDF5 can be optionally linked with the SZIP library (Science Data Lossless Compression Program) and ZLIB
#Symbol to detect in ZLIB can be only H5Z_DEFLATE, a structure only defined in H5Zdeflate.c if the filter is enabled
#For SZIP the structure can be only H5Z_SZIP, defined in H5Zszip.c if the filter is enabled
#check_library_exists() tries to link a temporary program with these symbols
#On MSVC for this detection to work, the HDF5 library MUST HAVE as additional dependencies the ZLIB and SZIP libraries,
#which is not a requirement for the library to build successfully
#//////////////////////////

set(HDF5_REQUIRED_LIBRARIES ${HDF5_HL_LIBRARY} ${HDF5_LIBRARY} ${ZLIB_LIBRARY} ${SZIP_LIBRARY})
# Replace backslashes with forward slashes because it can cause	problems with Windows' paths
string(REPLACE "\\" "/" HDF5_REQUIRED_LIBRARIES ${HDF5_REQUIRED_LIBRARIES})
set(CMAKE_REQUIRED_LIBRARIES ${HDF5_REQUIRED_LIBRARIES})

message("-- Detecting if HDF5 library ${HDF5_LIBRARY} needs the ZLIB library...")
check_library_exists(${HDF5_LIBRARY} H5Z_DEFLATE "" NEED_ZLIB)
if (NEED_ZLIB)
  message("${color_blue}-- ZLIB library is needed...${color_reset}")
else()
  message("${color_blue}-- ZLIB library is not needed...${color_reset}")
endif()

message("-- Detecting if HDF5 library ${HDF5_LIBRARY} needs the SZIP library...")
check_library_exists(${HDF5_LIBRARY} H5Z_SZIP "" NEED_SZIP)
if (NEED_SZIP)
  message("${color_blue}-- SZIP library is needed...${color_reset}")
else()
  message("${color_blue}-- SZIP library is not needed...${color_reset}")
endif()

if (NEED_SZIP)
  message("-- Trying to find the SZIP library...")
  find_library(SZIP_LIBRARY NAMES sz ${find_opt})
  if(NOT SZIP_LIBRARY)
    message(FATAL_ERROR "${color_red}szip library not found${color_reset}")
  else()
    message("-- Found szip library at: " ${SZIP_LIBRARY})
  endif()
endif()

if (NEED_ZLIB)
  message("-- Trying to find the ZLIB library...")
  find_library(ZLIB_LIBRARY NAMES libz.a z ${find_opt})
  if(NOT ZLIB_LIBRARY)
    message(FATAL_ERROR "${color_red}zlib library not found${color_reset}")
  else()
    message("-- Found zlib library at: " ${ZLIB_LIBRARY})
  endif()
endif()

find_library(CURL_LIBRARY NAMES curl ${find_opt})
if(NOT CURL_LIBRARY)
  message(FATAL_ERROR "${color_red}curl library not found${color_reset}")
else()
  message("-- Found curl library at: " ${CURL_LIBRARY})
endif()


#//////////////////////////
#GSL_ROOT
#detect a user defined system environment variable GSL_ROOT
#this sets the include path to GSL_INCLUDE directly
#and a PATHS hint for GSL_LIBRARY and GSL_CBLAS_LIBRARY
#For this case find_library(GSL_LIBRARY/GSL_CBLAS_LIBRARY) MUST be called 2 times
#First time it looks in GSL_LIBRARY_PATH if set, avoiding all other PATHS with NO_DEFAULT_PATH
#//////////////////////////

set(gsl_root "$ENV{GSL_ROOT}")
if(gsl_root)
  set(GSL_INCLUDE "${gsl_root}/include")
  set(GSL_LIBRARY_PATH "${gsl_root}/lib")
  set(GSL_CBLAS_LIBRARY_PATH "${gsl_root}/lib")
  message("${color_blue}-- Using user defined GSL location: ${gsl_root}${color_reset}")
  set(GSL_CONFIG_PATH ${gsl_root}/bin/gsl-config)
endif(gsl_root)
  
#//////////////////////////
#find gsl-config
#//////////////////////////  
  
find_program(gsl_config NAMES gsl-config PATHS ${GSL_CONFIG_PATH} NO_DEFAULT_PATH)
find_program(gsl_config NAMES gsl-config ${find_opt})

if (gsl_config)
  execute_process(COMMAND ${gsl_config} --prefix OUTPUT_VARIABLE gsl_prefix)
  if (gsl_prefix)
    string(REGEX REPLACE "\n$" "" gsl_prefix "${gsl_prefix}")
    message("${color_blue}-- Found GSL installation prefix at ${gsl_prefix}${color_reset}")
  endif(gsl_prefix)
  execute_process(COMMAND ${gsl_config} --version OUTPUT_VARIABLE gsl_version)
  if (gsl_version)
    string(REGEX REPLACE "\n$" "" gsl_version "${gsl_version}")
    message("${color_blue}-- Found GSL version ${gsl_version}${color_reset}")
    string(REPLACE "." ";" version_list ${gsl_version})
    list(GET version_list 0 GSL_MAJOR)
    list(GET version_list 1 GSL_MINOR)
    if (GSL_MAJOR)
      add_definitions(-DNCO_GSL_MAJOR_VERSION=${GSL_MAJOR})
    endif()
    if (GSL_MINOR)
      add_definitions(-DNCO_GSL_MINOR_VERSION=${GSL_MINOR})
    endif()
    math(EXPR GSL_VER "${GSL_MAJOR} * 100 + ${GSL_MINOR}")
    if (GSL_VER)
      add_definitions(-DGSL_VER=${GSL_VER})
    endif()
  endif(gsl_version)
endif(gsl_config)

find_path(GSL_INCLUDE gsl/gsl_sf_gamma.h ${find_opt})
if(NOT GSL_INCLUDE)
  message("${color_magenta}-- GSL header files not found${color_reset}")
else()
  message("-- Found GSL header files at: " ${GSL_INCLUDE})
endif()

find_library(GSL_LIBRARY NAMES gsl libgsl_d.lib PATHS ${GSL_LIBRARY_PATH} NO_DEFAULT_PATH)
find_library(GSL_LIBRARY NAMES gsl libgsl_d.lib ${find_opt})
if(NOT GSL_LIBRARY)
  message("${color_magenta}-- GSL library not found${color_reset}")
else()
  message("-- Found GSL library at: " ${GSL_LIBRARY})
endif()

find_library(GSL_CBLAS_LIBRARY NAMES gslcblas libgslcblas_d.lib PATHS ${GSL_CBLAS_LIBRARY_PATH} NO_DEFAULT_PATH)
find_library(GSL_CBLAS_LIBRARY NAMES gslcblas libgslcblas_d.lib ${find_opt})
if(NOT GSL_CBLAS_LIBRARY)
  message("${color_magenta}-- GSL CBLAS library not found${color_reset}")
else()
  message("-- Found GSL CBLAS library at: " ${GSL_CBLAS_LIBRARY})
endif()

if(GSL_INCLUDE AND GSL_LIBRARY AND GSL_CBLAS_LIBRARY)
  add_definitions(-DENABLE_GSL)
  include_directories(${GSL_INCLUDE})
  set(LIB_DEP ${LIB_DEP} ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY})
  message("${color_green}-- Building with GSL support${color_reset}")
else()
  message("${color_magenta}-- Disabling GSL support${color_reset}")
endif ()

#//////////////////////////
#detect optional dependencies
#//////////////////////////

#//////////////////////////
# UDUNITS2 (and EXPAT)
# Add non standard UDUNITS2 header paths (Mac ports, Linux Fedora)
#//////////////////////////

set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH})
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/opt/local/include/udunits2")
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/usr/include/udunits2")

find_path(UDUNITS2_INCLUDE udunits2.h ${find_opt})
if(NOT UDUNITS2_INCLUDE)
  message("${color_magenta}-- Unidata units library header files not found${color_reset}")
else()
  message("-- Found Unidata units library header files at: " ${UDUNITS2_INCLUDE})
endif()

find_library(UDUNITS2_LIBRARY NAMES udunits2 ${find_opt})
if(NOT UDUNITS2_LIBRARY)
  message("${color_magenta}-- Unidata units library not found${color_reset}")
else()
  message("-- Found Unidata units library at: " ${UDUNITS2_LIBRARY})
endif()

#//////////////////////////
# UDUnits depends on expat
#//////////////////////////

if (UDUNITS2_LIBRARY AND UDUNITS2_INCLUDE)
  find_library(EXPAT_LIBRARY NAMES libexpatMT expat ${find_opt})
  if(NOT EXPAT_LIBRARY)
    message("${color_magenta}-- Expat library not found${color_reset}")
    message("-- Disabling UDUNITS support")
  else()
    message("-- Found Expat library at: " ${EXPAT_LIBRARY})
    message("${color_green}-- Building with UDUNITS support${color_reset}")
  endif()
endif()

#//////////////////////////
#compile as C++/static CRT on Visual Studio and as C99 on UNIX
#//////////////////////////

if (MSVC)
  set(MSVC_USE_STATIC_CRT ON CACHE BOOL "Use /MT flag (static CRT) when compiling in MSVC")
  if (MSVC_USE_STATIC_CRT)
    message("-- Using static CRT ${MSVC_USE_STATIC_CRT}")
    foreach(flag_var CMAKE_CXX_FLAGS_DEBUG 
      CMAKE_CXX_FLAGS_RELEASE
      CMAKE_C_FLAGS_DEBUG 
      CMAKE_C_FLAGS_RELEASE)
    string(REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
    endforeach()
  endif(MSVC_USE_STATIC_CRT)

  #//////////////////////////
  #compile as C++ Code (/TP)
  #//////////////////////////
  
  foreach(flag_var CMAKE_C_FLAGS_DEBUG 
    CMAKE_C_FLAGS_RELEASE)
  set(${flag_var} "/TP ${${flag_var}}")
  endforeach()

  #//////////////////////////
  #visual studio defines math symbols in math.h, avoid duplicate definition
  #//////////////////////////

  add_definitions(-D_MATH_DEFINES_DEFINED)

else (MSVC)

  #//////////////////////////
  #compile as C99
  #//////////////////////////

  if(CMAKE_VERSION VERSION_LESS "3.1")
    if(CMAKE_COMPILER_IS_GNUCC)
      set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif()
  else()
    set(CMAKE_C_STANDARD 99)
  endif()

  #//////////////////////////
  # other gcc flags
  #//////////////////////////

  if(CMAKE_COMPILER_IS_GNUCC)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 4.9)
     set(CMAKE_C_FLAGS "-D_BSD_SOURCE -D_POSIX_SOURCE ${CMAKE_C_FLAGS}")
    else()
     set(CMAKE_C_FLAGS "-D_DEFAULT_SOURCE ${CMAKE_C_FLAGS}")
    endif()
  endif()

endif(MSVC)


#//////////////////////////
#link with libraries
#LIB_DEP contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(LIB_DEP ${LIB_DEP})
set(LIB_DEP ${LIB_DEP} ${NETCDF_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${HDF5_HL_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${HDF5_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${CURL_LIBRARY})
if (NEED_ZLIB)
  set(LIB_DEP ${LIB_DEP} ${ZLIB_LIBRARY})
endif()
if (NEED_SZIP)
  set(LIB_DEP ${LIB_DEP} ${SZIP_LIBRARY})
endif()

#//////////////////////////
#add definitions based on libraries found
#//////////////////////////

if(NETCDF_INCLUDE AND NETCDF_LIBRARY)
add_definitions(-DHAVE_NETCDF4_H)
add_definitions(-DENABLE_NETCDF4)
include_directories(${NETCDF_INCLUDE})
endif()

if(UDUNITS2_INCLUDE AND UDUNITS2_LIBRARY AND EXPAT_LIBRARY)
add_definitions(-DENABLE_UDUNITS)  
add_definitions(-DHAVE_UDUNITS2_H) 
include_directories(${UDUNITS2_INCLUDE})  
set(LIB_DEP ${LIB_DEP} ${UDUNITS2_LIBRARY} ${EXPAT_LIBRARY}) 
endif()

#//////////////////////////
# system specific libraries needed to link (assume existing)
#//////////////////////////

if(APPLE)
  set(LIB_DEP ${LIB_DEP} resolv)
endif()

if(MSVC)
  set(LIB_DEP ${LIB_DEP} ws2_32.lib winmm.lib wldap32.lib)
endif()

if(UNIX)
  set(LIB_DEP ${LIB_DEP} m)
endif()

#//////////////////////////
#nco lib sources 
#//////////////////////////

set(nco_SOURCES ${nco_SOURCES} src/nco/libnco.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_att_utl.c src/nco/nco_att_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_aux.c src/nco/nco_aux.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_bnr.c src/nco/nco_bnr.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cln_utl.c src/nco/nco_cln_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnf_dmn.c src/nco/nco_cnf_dmn.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnf_typ.c src/nco/nco_cnf_typ.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnk.c src/nco/nco_cnk.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnv_arm.c src/nco/nco_cnv_arm.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnv_csm.c src/nco/nco_cnv_csm.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_crt.c src/nco/nco_crt.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_ctl.c src/nco/nco_ctl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_dbg.c src/nco/nco_dbg.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_dmn_utl.c src/nco/nco_dmn_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_fl_utl.c src/nco/nco_fl_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_flt.c src/nco/nco_flt.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_getopt.c src/nco/nco_getopt.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_grp_trv.c src/nco/nco_grp_trv.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_grp_utl.c src/nco/nco_grp_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_kd.c src/nco/nco_kd.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_lmt.c src/nco/nco_lmt.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_lst_utl.c src/nco/nco_lst_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_map.c src/nco/nco_map.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_map.c src/nco/nco_map.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_md5.c src/nco/nco_md5.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_mmr.c src/nco/nco_mmr.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_mpi.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_msa.c src/nco/nco_msa.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_mss_val.c src/nco/nco_mss_val.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_mta.c src/nco/nco_mta.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_netcdf.c src/nco/nco_netcdf.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_omp.c src/nco/nco_omp.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_pck.c src/nco/nco_pck.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_ply.c src/nco/nco_ply.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_ply_lst.c src/nco/nco_ply_lst.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_ppc.c src/nco/nco_ppc.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_prn.c src/nco/nco_prn.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_rec_var.c src/nco/nco_rec_var.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_rgr.c src/nco/nco_rgr.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_rth_flt.c src/nco/nco_rth_flt.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_rth_utl.c src/nco/nco_rth_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_s1d.c src/nco/nco_s1d.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_scl_utl.c src/nco/nco_scl_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_scm.c src/nco/nco_scm.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_sld.c src/nco/nco_sld.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_sng_utl.c src/nco/nco_sng_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_sph.c src/nco/nco_sph.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_srm.c src/nco/nco_srm.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_typ.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_uthash.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_avg.c src/nco/nco_var_avg.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_lst.c src/nco/nco_var_lst.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_rth.c src/nco/nco_var_rth.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_scv.c src/nco/nco_var_scv.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_utl.c src/nco/nco_var_utl.h)

# Type of library (SHARED, STATIC) is configured using BUILD_SHARED_LIBS
add_library(nco ${nco_SOURCES})
add_executable(ncatted src/nco/ncatted.c)
add_executable(ncbo src/nco/ncbo.c)
add_executable(ncecat src/nco/ncecat.c)
add_executable(ncflint src/nco/ncflint.c)
add_executable(ncks src/nco/ncks.c)
add_executable(ncpdq src/nco/ncpdq.c)
add_executable(ncra src/nco/ncra.c)
add_executable(ncrename src/nco/ncrename.c)

#//////////////////////////
#ncwa, flex, bison on UNIX
#flex -Pnco_yy ncap_lex.l
#mv lex.nco_yy.c ncap_lex.c
#bison -d --name-prefix=nco_yy --output=ncap_yacc.c -d ncap_yacc.y
#//////////////////////////

set(ncwa_SOURCES ${ncwa_SOURCES})
set(ncwa_SOURCES ${ncwa_SOURCES} src/nco/ncwa.c)
find_package(FLEX QUIET) 
if(FLEX_FOUND)
 add_definitions(-DHAVE_BISON_FLEX)
 message("-- Flex found at ${FLEX_EXECUTABLE}")
 FLEX_TARGET(nco_scanner src/nco/ncap_lex.l ${CMAKE_CURRENT_BINARY_DIR}/ncap_lex.c COMPILE_FLAGS "-Pnco_yy")
 include_directories(${CMAKE_CURRENT_BINARY_DIR})
 include_directories(src/nco)
 message("-- Flex will generate ${FLEX_nco_scanner_OUTPUTS}")
 set(ncwa_SOURCES ${ncwa_SOURCES}  ${FLEX_nco_scanner_OUTPUTS} src/nco/ncap_utl.c)
else()
  message("${color_magenta}-- Flex not found${color_reset}")
endif()
add_executable(ncwa ${ncwa_SOURCES})

set(nco_LIB nco)

target_link_libraries (ncatted ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncbo ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncecat ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncflint ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncks ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncpdq ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncra ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncrename ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncwa ${nco_LIB} ${LIB_DEP})

#//////////////////////////
#ANTLR
#cmake can detect relative paths, e.g, antlr/AST.hpp
#detect a user defined system environment variable ANTLR_ROOT
#this sets the include path to ANTLR_INCLUDE directly
#and a PATHS hint for ANTLR_LIBRARY 
#For this case find_library(ANTLR_LIBRARY) MUST be called 2 times
#First time it looks in ANTLR_LIBRARY_PATH if set, avoiding all other PATHS with NO_DEFAULT_PATH
#//////////////////////////

set(antlr_root "$ENV{ANTLR_ROOT}")
if(antlr_root)
  set(ANTLR_INCLUDE "${antlr_root}/include")
  set(ANTLR_LIBRARY_PATH "${antlr_root}/lib")
  set(ANTLR_BIN "${antlr_root}/bin")
  set(ENV{PATH} "${ANTLR_BIN}:$ENV{PATH}")
  message("${color_blue}-- Using user defined antlr location: ${antlr_root}${color_reset}")
endif()

find_path(ANTLR_INCLUDE antlr/AST.hpp ${find_opt})
if(NOT ANTLR_INCLUDE)
  message("${color_magenta}-- ANTLR header files not found${color_reset}")
else()
  message("-- Found ANTLR header files at: " ${ANTLR_INCLUDE})
endif()

find_library(ANTLR_LIBRARY NAMES antlr PATHS ${ANTLR_LIBRARY_PATH} NO_DEFAULT_PATH)
find_library(ANTLR_LIBRARY NAMES antlr ${find_opt})
if(NOT ANTLR_LIBRARY)
  message("${color_magenta}-- ANTLR library not found${color_reset}")
else()
  message("-- Found ANTLR library at: " ${ANTLR_LIBRARY})
endif()

if(ANTLR_INCLUDE AND ANTLR_LIBRARY)
  message("${color_green}-- Building ncap2, netCDF Arithmetic Processor${color_reset}")
else()
  message("${color_magenta}-- Disabling ncap2, netCDF Arithmetic Processor${color_reset}")
endif()

set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/Invoke.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/Invoke.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/NcapVar.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/NcapVarVector.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/fmc_all_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/fmc_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/fmc_gsl_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2_att.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2_utl.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncoLexer.cpp)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncoParser.cpp)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncoTree.cpp)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/nco_gsl.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/prs_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/sdo_utl.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/sym_cls.cc)

#//////////////////////////
#GSL
#GSL CBLAS needed for Linux Fedora; assuming here all gsl-dev packages have it
#//////////////////////////


#//////////////////////////
# ncap2
#//////////////////////////

if(ANTLR_INCLUDE AND ANTLR_LIBRARY)
  add_executable(ncap2 ${ncap2_SOURCES})
  target_include_directories(ncap2 PRIVATE src/nco ${ANTLR_INCLUDE})
  target_link_libraries (ncap2 ${nco_LIB} ${LIB_DEP} ${ANTLR_LIBRARY}) 
  
  #//////////////////////////
  # macros used in ncap2 to use advanced math: erf, erfc, gamma
  # LINUX LINUXAMD64 MACOSX
  #//////////////////////////
  
  if(APPLE)
    add_definitions(-DMACOSX)
  endif() 
  if(UNIX AND NOT APPLE)
   add_definitions(-DLINUX)
  endif() 
endif()

#//////////////////////////
# detect system specific functions used by NCO
#//////////////////////////

check_function_exists(mkstemp HAVE_mkstemp)
if (HAVE_mkstemp)
  message("-- Found function mkstemp")
  add_definitions(-DHAVE_MKSTEMP)
endif()

#//////////////////////////
#math functions, including float versions
#NCO_CHECK_FUNCS([atan2])
#NCO_CHECK_FUNCS([acosf asinf atanf atan2f cosf expf fabsf fmodf log10f logf powf sinf sqrtf tanf])
#NCO_CHECK_FUNCS([erff erfcf gammaf])
#NCO_CHECK_FUNCS([acoshf asinhf atanhf coshf sinhf tanhf])
#NCO_CHECK_FUNCS([ceilf floorf])
#NCO_CHECK_FUNCS([nearbyint rint round trunc])
#NCO_CHECK_FUNCS([nearbyintf rintf roundf truncf])
#//////////////////////////

find_library(MATH_LIBRARY NAMES m ${find_opt})
if(MATH_LIBRARY)
  message("-- Checking for math library functions at: " ${MATH_LIBRARY})
  set(CMAKE_REQUIRED_LIBRARIES m)
  set(CMAKE_REQUIRED_QUIET 0)
  macro(nco_check_funcs func)
  unset(have_result CACHE)
  check_function_exists(${ARGV0} have_result)
  if (NOT have_result)
    message("${color_blue}-- Using NCO defined version of ${ARGV0}${color_reset}")
    string(TOUPPER ${ARGV0} def_fnc )
    add_definitions(-DNEED_${def_fnc})
  endif()
  endmacro(nco_check_funcs)
  nco_check_funcs(atan2)
  nco_check_funcs(acosf)
  nco_check_funcs(asinf)
  nco_check_funcs(atanf)
  nco_check_funcs(cosf)
  nco_check_funcs(expf)
  nco_check_funcs(fabsf)
  nco_check_funcs(fmodf)
  nco_check_funcs(log10f)
  nco_check_funcs(logf)
  nco_check_funcs(powf)
  nco_check_funcs(sinf)
  nco_check_funcs(sqrtf)
  nco_check_funcs(tanf)
  nco_check_funcs(erff)
  nco_check_funcs(erfcf)
  nco_check_funcs(gammaf)
  nco_check_funcs(acoshf)
  nco_check_funcs(asinhf)
  nco_check_funcs(atanhf)
  nco_check_funcs(coshf)
  nco_check_funcs(sinhf)
  nco_check_funcs(tanhf)
  nco_check_funcs(ceilf)
  nco_check_funcs(floorf)
  nco_check_funcs(nearbyint)
  nco_check_funcs(rint)
  nco_check_funcs(round)
  nco_check_funcs(trunc)
  nco_check_funcs(nearbyintf)
  nco_check_funcs(rintf)
  nco_check_funcs(roundf)
  nco_check_funcs(truncf)
endif(MATH_LIBRARY)

check_include_file(strings.h HAVE_STRINGS_H)
if (HAVE_STRINGS_H)
  message("-- Found header strings.h")
  add_definitions(-DHAVE_STRINGS_H)
endif()

#//////////////////////////
# regex
#//////////////////////////

check_include_file(regex.h HAVE_REGEX_H)
check_function_exists(regexec HAVE_REGEXEC)
check_function_exists(regcomp HAVE_REGCOMP)
check_function_exists(regfree HAVE_REGFREE)
if (HAVE_REGEX_H)
  message("-- Found header regex.h")
  add_definitions(-DHAVE_REGEX_H)
endif()

if (HAVE_REGEX_H AND HAVE_REGEXEC AND HAVE_REGCOMP AND HAVE_REGFREE)
  message("${color_green}-- Enabling Regex functionality...${color_reset}")
  add_definitions(-DNCO_HAVE_REGEX_FUNCTIONALITY)
endif()


#//////////////////////////
#additional informational definitions used by ncks -r
#//////////////////////////

if (CURL_LIBRARY)
  add_definitions(-DENABLE_DAP)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DENABLE_DEBUG_SYMBOLS)
endif()


#//////////////////////////
#USER, HOSTNAME, NCO_BUILDENGINE
#HOSTNAME is a variable that is set by some shells like bash only, 
#but cmake for speed and consistency runs commands using the most basic unix shell (ie. sh), 
#which does not set that variable by default.
#//////////////////////////

set(nco_user "$ENV{USER}")
if(NOT nco_user)
 set(nco_user "$ENV{USERNAME}")
endif()
if(nco_user)
 message("-- USER is set to: " ${nco_user})
 add_definitions(-DUSER=${nco_user})
endif()

cmake_host_system_information(RESULT nco_hostname QUERY HOSTNAME)
if(nco_hostname)
 message("-- HOSTNAME is set to: " ${nco_hostname})
 add_definitions(-DHOSTNAME=${nco_hostname})
endif()

set(NCO_BUILDENGINE "CMake")
if(NCO_BUILDENGINE)
 message("-- NCO_BUILDENGINE is set to: " ${NCO_BUILDENGINE})
 add_definitions(-DNCO_BUILDENGINE=${NCO_BUILDENGINE})
endif()

#//////////////////////////
# OpenMP
#//////////////////////////

if(NOT MSVC)
  find_package(OpenMP QUIET)
  if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    message("${color_green}-- Building with OpenMP support${color_reset}")
  else()
    message("-- Building without OpenMP support")
  endif()
endif()

#/////////////////////////////////////////////////////////////////////////////////////
#install target
#install prefix: detect test suite expected location; if present use it instead
#/////////////////////////////////////////////////////////////////////////////////////

set(test_dir "$ENV{MY_BIN_DIR}")
if(test_dir)
  if(test_dir STREQUAL ${CMAKE_INSTALL_PREFIX})
  else()
    message("${color_magenta}-- Modifying installation prefix to location expected by test suite : ${test_dir}${color_reset}")
    set(CMAKE_INSTALL_PREFIX ${test_dir})
  endif()
endif()

set(nco_targets ${nco_targets})
set(nco_targets ${nco_targets} ncks ncbo ncecat ncflint ncpdq ncra ncrename ncatted ncwa)

#//////////////////////////
#make ncap2 build last 
#//////////////////////////

if(ANTLR_INCLUDE AND ANTLR_LIBRARY)
  add_dependencies(ncap2 ${nco_targets})
  set(nco_targets ${nco_targets} ncap2)
endif()

set(PERMISSIONS_DEFAULT OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
message("-- Building ${nco_targets}")
install(TARGETS ${nco_targets} RUNTIME DESTINATION . PERMISSIONS ${PERMISSIONS_DEFAULT})
install(FILES data/ncremap data/ncclimo DESTINATION . PERMISSIONS ${PERMISSIONS_DEFAULT})
message("-- Installing data/ncremap;data/ncclimo to: " ${CMAKE_INSTALL_PREFIX})

#//////////////////////////
# ncea->ncra
# nces->ncra
# ncrcat->ncra
# ncdiff->ncbo
# ncunpack->ncpdq
# the following cmake -E commands are available only on UNIX:
# create_symlink <old> <new> 
# create a symbolic link <new> naming <old>. 
#//////////////////////////

macro(install_links target src dest)
add_custom_command(TARGET ${target} POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest} 
COMMENT "Making symbolic link ${src} -> ${dest}")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${dest} DESTINATION . PERMISSIONS ${PERMISSIONS_DEFAULT})
endmacro(install_links)
if(UNIX)
  install_links(ncra ${CMAKE_INSTALL_PREFIX}/ncra ncea)
  install_links(ncra ${CMAKE_INSTALL_PREFIX}/ncra nces)
  install_links(ncra ${CMAKE_INSTALL_PREFIX}/ncra ncrcat)
  install_links(ncbo ${CMAKE_INSTALL_PREFIX}/ncbo ncdiff)
  install_links(ncpdq ${CMAKE_INSTALL_PREFIX}/ncpdq ncunpack)
endif()
message("-- Installing links ncea;nces;ncrcat;ncdiff;ncunpack to: " ${CMAKE_INSTALL_PREFIX})

#//////////////////////////
# uninstall target
#//////////////////////////

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

#//////////////////////////
# 'data' target , 'make data', generate .nc files from .cdl
# find ncgen (any ncgen will do)
#//////////////////////////

find_program(PATH_TO_NCGEN NAMES ncgen)
if (PATH_TO_NCGEN)
  set(PATH_DATA ${CMAKE_CURRENT_SOURCE_DIR}/data)
  message("${color_blue}-- Generating data target, using the ncgen tool at ${PATH_TO_NCGEN}${color_reset}")
  add_custom_target(data
  COMMENT "${color_green}Creating sample netCDF data files for self-tests in ${PATH_DATA}${color_reset}"
  COMMAND ${PATH_TO_NCGEN} -b -o ${PATH_DATA}/in.nc ${PATH_DATA}/in.cdl
  COMMAND ${PATH_TO_NCGEN} -b -o ${PATH_DATA}/split.nc ${PATH_DATA}/split.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/cmip5.nc ${PATH_DATA}/cmip5.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_1.nc ${PATH_DATA}/in_1.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_2.nc ${PATH_DATA}/in_2.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_grp.nc ${PATH_DATA}/in_grp.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_grp_1.nc ${PATH_DATA}/in_grp_1.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_grp_2.nc ${PATH_DATA}/in_grp_2.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_grp_3.nc ${PATH_DATA}/in_grp_3.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_grp_4.nc ${PATH_DATA}/in_grp_4.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_grp_5.nc ${PATH_DATA}/in_grp_5.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_grp_6.nc ${PATH_DATA}/in_grp_6.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/in_grp_7.nc ${PATH_DATA}/in_grp_7.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/mdl_1.nc ${PATH_DATA}/mdl_1.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/mdl_2.nc ${PATH_DATA}/mdl_2.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/mdl_3.nc ${PATH_DATA}/mdl_3.cdl
  COMMAND ${PATH_TO_NCGEN} -k netCDF-4 -b -o ${PATH_DATA}/obs.nc ${PATH_DATA}/obs.cdl)

#//////////////////////////
#make data build last 
#//////////////////////////

  add_dependencies(data ${nco_targets})
endif()

#//////////////////////////
# check target , 'make check'
# it depends on the NCO executables AND on the 'data' target
# in NCO 'make check' depends on 'make install', so tests can only be done
# if we can install to a writable directory, test -w /path; echo $? returns 0 if writable
# the default install prefix (e.g /usr) requires 'sudo' previleges 
# and the target would FAIL for this case, on the 'make install' part
# if CMAKE_INSTALL_PREFIX does not exist, it is created
#//////////////////////////

execute_process(COMMAND test -d ${CMAKE_INSTALL_PREFIX} RESULT_VARIABLE test_d)
if (test_d)
 message("${color_blue}-- Creating directory: ${CMAKE_INSTALL_PREFIX}${color_reset}")
 execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX} ERROR_QUIET)
endif()
execute_process(COMMAND test -w ${CMAKE_INSTALL_PREFIX} RESULT_VARIABLE test_w)
if (NOT test_w)
 set(can_write 1)
endif()

if(can_write AND UNIX)
  message("${color_blue}-- Generating check/install target with executables at: ${CMAKE_INSTALL_PREFIX}${color_reset}")
  add_custom_target(check
  COMMENT "${color_green}Running self-tests with NCO executables in ${CMAKE_INSTALL_PREFIX}${color_reset}"
  COMMAND cmake --build . --target install
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/bm/nco_bm.pl --regress)
  add_dependencies(check ${nco_targets} data)
endif()

#//////////////////////////
# install prefix
#//////////////////////////

message("${color_blue}-- Using installation path: ${CMAKE_INSTALL_PREFIX}${color_reset}")
if (NOT can_write)
 message("${color_magenta}-- Installing NCO requires write permission to: ${CMAKE_INSTALL_PREFIX}; check target is disabled${color_reset}")
endif()

#//////////////////////////
# generate 'path.sh' on build path, run with 'source path.sh'
# sets $PATH to locate the binaries we just built
#//////////////////////////

if (UNIX)
 set(file_sh ${CMAKE_CURRENT_BINARY_DIR}/path.sh)
 set(path "${CMAKE_INSTALL_PREFIX}:$ENV{PATH}")
 file(WRITE ${file_sh} "#!/usr/bin/env bash\n")
 file(APPEND ${file_sh} "export PATH=\"${path}\"")
 execute_process(COMMAND chmod a+x ${file_sh} RESULT_VARIABLE res)
endif()

#//////////////////////////
# compiler/linker warnings
#//////////////////////////

if(MSVC)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
set(MSVC_DEVELOPER ON CACHE BOOL "Disable MSVC warnings /wd4267 /wd4244 /wd4477")
if (MSVC_DEVELOPER)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /wd4267 /wd4244 /wd4477")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /wd4267 /wd4244 /wd4477")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /wd4267 /wd4244 /wd4477")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /wd4267 /wd4244 /wd4477")
endif()
endif()

#//////////////////////////
# disable .pdb not found warnings
#//////////////////////////


if (MSVC)
 set_target_properties(ncks PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncbo PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncecat PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncflint PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncpdq PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncra PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncrename PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncatted PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncwa PROPERTIES LINK_FLAGS "/ignore:4099")
 set_target_properties(ncap2 PROPERTIES LINK_FLAGS "/ignore:4099")
endif (MSVC)

#//////////////////////////
# use of strcasecmp and strncasecmp
# UDUNITS2 defines strcasecmp
#//////////////////////////

if (UDUNITS2_LIBRARY AND UDUNITS2_INCLUDE)
 set(have_udunits)
endif()

if (MSVC)
 if (NOT ${have_udunits})
   add_definitions(-DNEED_STRCASECMP)
 endif()
 add_definitions(-DNEED_STRNCASECMP)
endif(MSVC)

#ncra.exe to nces.exe and ncrcat.exe
if (MSVC)
 set (out ${CMAKE_CURRENT_BINARY_DIR})
 if (MSVC_IDE)
  set (out ${CMAKE_CURRENT_BINARY_DIR}/Debug)
 endif (MSVC_IDE)
 message("-- Renaming ncra.exe to nces.exe and ncrcat.exe in " ${output})
 add_custom_command(TARGET ncra POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${out}/ncra.exe ${out}/nces.exe)
 add_custom_command(TARGET ncra POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${out}/ncra.exe ${out}/ncrcat.exe)
endif (MSVC)
