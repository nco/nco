#CMake build, Pedro Vicente 
#assumptions:
#netCDF version 4 (based on HDF5) library version 
#that's all

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

#logic for finding header files and library dependencies:
#find_path()
#find_library()
#are used (with optional location hints); the first argument contains the found file if found.
#and the message "-- Found file at location" is made
#if not found the first argument variable is the same as a user option that contains the file location full path.
#the same message "-- Found file at location" is made
#///////////////////////////////////////////////////////

cmake_minimum_required(VERSION 2.8)
project (nco)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckLibraryExists)

#/////////////////////////////////////////////////////////////////////////////////////
#general C/C++ build flags
#/////////////////////////////////////////////////////////////////////////////////////

set(CMAKE_BUILD_TYPE Debug)
set(BUILD_STATIC_LIBS ON)
set(BUILD_SHARED_LIBS OFF)
add_definitions(-DENABLE_STATIC)

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

#//////////////////////////
# detect system specific functions
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

find_library(MATH_LIBRARY NAMES m)
if(MATH_LIBRARY)
  message("-- Math library found: " ${MATH_LIBRARY})
  set(CMAKE_REQUIRED_LIBRARIES m)

  check_function_exists(acosf HAVE_acosf)
  if (HAVE_acosf)
  else()
    message("${color_blue}-- Using NCO defined version of acos${color_reset}")
    add_definitions(-DNEED_ACOSF)
  endif()

endif(MATH_LIBRARY)

#//////////////////////////
# detect system/libraries specific header files
#//////////////////////////

check_include_file(strings.h HAVE_STRINGS_H)
if (HAVE_STRINGS_H)
  message("-- Found header strings.h")
  add_definitions(-DHAVE_STRINGS_H)
endif()

check_include_file(netcdf_meta.h NC_HAVE_META_H)
if (NC_HAVE_META_H)
  message("-- Found header netcdf_meta.h")
  add_definitions(-DNC_HAVE_META_H)
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
  message("-- Enabling Regex functionality...")
  add_definitions(-DNCO_HAVE_REGEX_FUNCTIONALITY)
endif()

#//////////////////////////
#detect minimum dependencies
#find_library() list a static name first to avoid link errors in systems with multiple versions of the same libraries in PATH
#used for ZLIB_LIBRARY: libz.a
#//////////////////////////

find_path(NETCDF_INCLUDE_FOUND netcdf.h HINTS ${NETCDF_INCLUDE} "/usr/include")
if(NOT NETCDF_INCLUDE_FOUND)
  message(FATAL_ERROR "${color_red}netcdf.h header file not found${color_reset}")
else()
  message("-- Found netcdf.h header file at: " ${NETCDF_INCLUDE_FOUND})
endif()

find_library(NETCDF_LIBRARY NAMES netcdf)
if(NOT NETCDF_LIBRARY)
  message(FATAL_ERROR "${color_red}netcdf library not found${color_reset}")
else()
  message("-- Found netcdf library at: " ${NETCDF_LIBRARY})
endif()

find_library(HDF5_LIBRARY NAMES hdf5)
if(NOT HDF5_LIBRARY)
  message(FATAL_ERROR "${color_red}hdf5 library not found${color_reset}")
else()
  message("-- Found hdf5 library at: " ${HDF5_LIBRARY})
endif()

find_library(HDF5_HL_LIBRARY NAMES hdf5_hl)
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

set(CMAKE_REQUIRED_LIBRARIES ${HDF5_LIBRARY})

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
  find_library(SZIP_LIBRARY NAMES sz)
  if(NOT SZIP_LIBRARY)
    message(FATAL_ERROR "${color_red}szip library not found${color_reset}")
  else()
    message("-- Found szip library at: " ${SZIP_LIBRARY})
  endif()
endif()

if (NEED_ZLIB)
  message("-- Trying to find the ZLIB library...")
  find_library(ZLIB_LIBRARY NAMES libz.a z)
  if(NOT ZLIB_LIBRARY)
    message(FATAL_ERROR "${color_red}zlib library not found${color_reset}")
  else()
    message("-- Found zlib library at: " ${ZLIB_LIBRARY})
  endif()
endif()

find_library(CURL_LIBRARY NAMES curl)
if(NOT CURL_LIBRARY)
  message(FATAL_ERROR "${color_red}curl library not found${color_reset}")
else()
  message("-- Found curl library at: " ${CURL_LIBRARY})
endif()

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

find_path(UDUNITS2_INCLUDE_FOUND udunits2.h HINTS ${UDUNITS2_INCLUDE})
if(NOT UDUNITS2_INCLUDE_FOUND)
  message("${color_magenta}-- Unidata units library header files not found${color_reset}")
else()
  message("-- Found Unidata units library header files at: " ${UDUNITS2_INCLUDE_FOUND})
endif()

find_library(UDUNITS2_LIBRARY NAMES udunits2)
if(NOT UDUNITS2_LIBRARY)
  message("${color_magenta}-- Unidata units library not found${color_reset}")
else()
  message("-- Found Unidata units library at: " ${UDUNITS2_LIBRARY})
endif()

if (UDUNITS2_LIBRARY AND UDUNITS2_INCLUDE_FOUND)
  # UDUnits depends on expat
  find_library(EXPAT_LIBRARY NAMES libexpatMT expat)
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
  add_definitions( "/D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_DEPRECATE" )
  message("-- Building with static runtime library")
  set(CMAKE_CXX_FLAGS_DEBUG "/D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_MINSIZEREL     "/MT /O1 /Ob1 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE        "/MT /O2 /Ob2 /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")
  
  #//////////////////////////
  #compile as C++ Code (/TP)
  #//////////////////////////
  
  set(CMAKE_C_FLAGS_DEBUG "/TP /D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_C_FLAGS_MINSIZEREL     "/TP /MT /O1 /Ob1 /D NDEBUG")
  set(CMAKE_C_FLAGS_RELEASE        "/TP /MT /O2 /Ob2 /D NDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/TP /MT /Zi /O2 /Ob1 /D NDEBUG")
  set(WINSOCK_LIBRARY ws2_32.lib winmm.lib wldap32.lib)
  
  #//////////////////////////
  #visual studio defines math symbols in math.h, avoid duplicate definition
  #//////////////////////////

  add_definitions(-D_MATH_DEFINES_DEFINED)

else (MSVC)  

#//////////////////////////
#compile as C99
#//////////////////////////

if (CMAKE_VERSION VERSION_LESS "3.1")  
  if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
  endif ()
else ()
  set (CMAKE_C_STANDARD 99)
endif ()

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

if(NETCDF_INCLUDE_FOUND AND NETCDF_LIBRARY)
add_definitions(-DHAVE_NETCDF4_H)
add_definitions(-DENABLE_NETCDF4)
include_directories(${NETCDF_INCLUDE_FOUND})
endif()

if(UDUNITS2_INCLUDE_FOUND AND UDUNITS2_LIBRARY AND EXPAT_LIBRARY)
add_definitions(-DENABLE_UDUNITS)  
add_definitions(-DHAVE_UDUNITS2_H) 
include_directories(${UDUNITS2_INCLUDE_FOUND})  
set(LIB_DEP ${LIB_DEP} ${UDUNITS2_LIBRARY} ${EXPAT_LIBRARY}) 
endif()

#//////////////////////////
# system specific libraries needed to link (assume existing)
#//////////////////////////

if(APPLE)
  set(LIB_DEP ${LIB_DEP} resolv)
endif()

if(MSVC)
  set(LIB_DEP ${LIB_DEP} ${WINSOCK_LIBRARY})
endif()

if(UNIX)
  set(LIB_DEP ${LIB_DEP} m)
endif()

#//////////////////////////
#nco lib sources 
#//////////////////////////

set(nco_SOURCES ${nco_SOURCES} src/nco/libnco.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_att_utl.c src/nco/nco_att_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_aux.c src/nco/nco_aux.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_bnr.c src/nco/nco_bnr.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cln_utl.c src/nco/nco_cln_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnf_dmn.c src/nco/nco_cnf_dmn.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnf_typ.c src/nco/nco_cnf_typ.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnk.c src/nco/nco_cnk.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnv_arm.c src/nco/nco_cnv_arm.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_cnv_csm.c src/nco/nco_cnv_csm.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_ctl.c src/nco/nco_ctl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_dbg.c src/nco/nco_dbg.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_dmn_utl.c src/nco/nco_dmn_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_fl_utl.c src/nco/nco_fl_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_getopt.c src/nco/nco_getopt.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_grp_trv.c src/nco/nco_grp_trv.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_grp_utl.c src/nco/nco_grp_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_lmt.c src/nco/nco_lmt.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_lst_utl.c src/nco/nco_lst_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_md5.c src/nco/nco_md5.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_mmr.c src/nco/nco_mmr.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_mpi.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_msa.c src/nco/nco_msa.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_mss_val.c src/nco/nco_mss_val.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_mta.c src/nco/nco_mta.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_netcdf.c src/nco/nco_netcdf.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_omp.c src/nco/nco_omp.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_pck.c src/nco/nco_pck.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_ppc.c src/nco/nco_ppc.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_prn.c src/nco/nco_prn.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_rec_var.c src/nco/nco_rec_var.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_rgr.c src/nco/nco_rgr.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_rth_flt.c src/nco/nco_rth_flt.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_rth_utl.c src/nco/nco_rth_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_scl_utl.c src/nco/nco_scl_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_scm.c src/nco/nco_scm.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_sld.c src/nco/nco_sld.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_sng_utl.c src/nco/nco_sng_utl.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_srm.c src/nco/nco_srm.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_typ.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_uthash.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_avg.c src/nco/nco_var_avg.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_lst.c src/nco/nco_var_lst.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_rth.c src/nco/nco_var_rth.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_scv.c src/nco/nco_var_scv.h)
set(nco_SOURCES ${nco_SOURCES} src/nco/nco_var_utl.c src/nco/nco_var_utl.h)

add_library(nco STATIC ${nco_SOURCES})
add_executable(ncks src/nco/ncks.c)
add_executable(ncbo src/nco/ncbo.c)
add_executable(ncecat src/nco/ncecat.c)
add_executable(ncflint src/nco/ncflint.c)
add_executable(ncpdq src/nco/ncpdq.c)
add_executable(ncra src/nco/ncra.c)
add_executable(ncrename src/nco/ncrename.c)
add_executable(ncatted src/nco/ncatted.c)

#//////////////////////////
#ncwa, flex, bison on UNIX
#flex -Pnco_yy ncap_lex.l
#mv lex.nco_yy.c ncap_lex.c
#bison -d --name-prefix=nco_yy --output=ncap_yacc.c -d ncap_yacc.y
#//////////////////////////

set(ncwa_SOURCES ${ncwa_SOURCES})
set(ncwa_SOURCES ${ncwa_SOURCES} src/nco/ncwa.c)
find_package(BISON QUIET)
find_package(FLEX  QUIET) 
if(BISON_FOUND AND FLEX_FOUND)
 add_definitions(-DHAVE_BISON_FLEX)
 message("-- Bison found at ${BISON_EXECUTABLE}")
 message("-- Flex found at ${FLEX_EXECUTABLE}")
 BISON_TARGET(nco_parser src/nco/ncap_yacc.y ${CMAKE_CURRENT_BINARY_DIR}/ncap_yacc.c COMPILE_FLAGS "--name-prefix=nco_yy")
 FLEX_TARGET(nco_scanner src/nco/ncap_lex.l ${CMAKE_CURRENT_BINARY_DIR}/ncap_lex.c COMPILE_FLAGS "-Pnco_yy")
 ADD_FLEX_BISON_DEPENDENCY(nco_scanner nco_parser)
 include_directories(${CMAKE_CURRENT_BINARY_DIR})
 include_directories(src/nco)
 message("-- Bison/Flex will generate ${BISON_nco_parser_OUTPUTS} ${FLEX_nco_scanner_OUTPUTS}")
 set(ncwa_SOURCES ${ncwa_SOURCES} ${BISON_nco_parser_OUTPUTS} ${FLEX_nco_scanner_OUTPUTS} src/nco/ncap_utl.c)
else()
  message("${color_magenta}-- Bison/Flex not found${color_reset}")
endif()
add_executable(ncwa ${ncwa_SOURCES})


set(nco_LIB nco)

target_link_libraries (ncks ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncbo ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncecat ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncflint ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncpdq ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncra ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncrename ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncatted ${nco_LIB} ${LIB_DEP})
target_link_libraries (ncwa ${nco_LIB} ${LIB_DEP})


#//////////////////////////
#detect dependencies
#note: cmake can detect relative paths, e.g, antlr/AST.hpp
#//////////////////////////

find_path(ANTLR_INCLUDE_FOUND antlr/AST.hpp HINTS ${ANTLR_INCLUDE})
if(NOT ANTLR_INCLUDE_FOUND)
  message("${color_magenta}-- ANTLR header files not found${color_reset}")
else()
  message("-- Found ANTLR header files at: " ${ANTLR_INCLUDE_FOUND})
endif()

find_library(ANTLR_LIBRARY NAMES antlr)
if(NOT ANTLR_LIBRARY)
  message("${color_magenta}-- ANTLR library not found${color_reset}")
else()
  message("-- Found ANTLR library at: " ${ANTLR_LIBRARY})
endif()

if(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)
  message("${color_green}-- Building ncap2, netCDF Arithmetic Processor${color_reset}")
else()
  message("${color_magenta}-- Disabling ncap2, netCDF Arithmetic Processor${color_reset}")
endif()

set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/Invoke.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2_utl.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2_att.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/sdo_utl.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/sym_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/fmc_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/fmc_all_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/fmc_gsl_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/prs_cls.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/NcapVar.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/NcapVarVector.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncoLexer.cpp)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncoParser.cpp)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncoTree.cpp)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/Invoke.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/nco_gsl.c)

#//////////////////////////
# GSL
# GSL CBLAS needed for Linux Fedora; assuming here all gsl-dev packages have it
#//////////////////////////

if(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)
  find_path(GSL_INCLUDE_FOUND gsl/gsl_errno.h HINTS ${GSL_INCLUDE})
  if(NOT GSL_INCLUDE_FOUND)
    message("${color_magenta}-- GSL header files not found${color_reset}")
  else()
    message("-- Found GSL header files at: " ${GSL_INCLUDE_FOUND})
  endif()
  find_library(GSL_LIBRARY NAMES gsl HINTS libgsl_d.lib)
  if(NOT GSL_LIBRARY)
    message("${color_magenta}-- GSL library not found${color_reset}")
  else()
    message("-- Found GSL library at: " ${GSL_LIBRARY})
  endif()
  find_library(GSL_CBLAS_LIBRARY NAMES gslcblas HINTS libgslcblas_d.lib)
  if(NOT GSL_CBLAS_LIBRARY)
    message("${color_magenta}-- GSL CBLAS library not found${color_reset}")
  else()
    message("-- Found GSL CBLAS library at: " ${GSL_CBLAS_LIBRARY})
  endif()
  if(GSL_INCLUDE_FOUND AND GSL_LIBRARY AND GSL_CBLAS_LIBRARY)
    add_definitions(-DENABLE_GSL)
    include_directories(${GSL_INCLUDE_FOUND})
    set(LIB_DEP ${LIB_DEP} ${GSL_LIBRARY} ${GSL_CBLAS_LIBRARY})
    message("${color_green}-- Building with GSL support${color_reset}")
  else()
    message("${color_magenta}-- Disabling GSL support${color_reset}")
  endif ()
endif(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)

#//////////////////////////
# ncap2
#//////////////////////////

if(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)
  add_executable(ncap2 ${ncap2_SOURCES})
  target_include_directories(ncap2 PRIVATE src/nco ${ANTLR_INCLUDE_FOUND})
  target_link_libraries (ncap2 ${nco_LIB} ${LIB_DEP} ${ANTLR_LIBRARY}) 
endif()

#//////////////////////////
#additional informational definitions used by ncks -r
#//////////////////////////

if (CURL_LIBRARY)
  add_definitions(-DENABLE_DAP)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DENABLE_DEBUG_SYMBOLS)
  message("-- Using debug symbols in build")
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
    message("-- Building with OpenMP support")
  else()
    message("-- Building without OpenMP support")
  endif()
endif()

#//////////////////////////
# install target
#//////////////////////////

set(nco_targets ${nco_targets})
set(nco_targets ${nco_targets} ncks ncbo ncecat ncflint ncpdq ncra ncrename ncatted ncwa)
if(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)
  
  #//////////////////////////
  #make ncap2 build last 
  #//////////////////////////
  
  add_dependencies(ncap2 ${nco_targets})
  set(nco_targets ${nco_targets} ncap2)
endif()

message("-- Building ${nco_targets}")

if (UNIX)
  install(TARGETS ${nco_targets} RUNTIME DESTINATION bin)
  install(FILES data/ncremap data/ncclimo DESTINATION bin)
  message("-- Installing data/ncremap;data/ncclimo to: " ${CMAKE_INSTALL_PREFIX}/bin)
endif()

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

if (UNIX)
  set(src ncra)
  set(dest ncea)
  add_custom_command(TARGET ncra POST_BUILD COMMAND ${CMAKE_COMMAND} -E create_symlink ${src} ${dest} DEPENDS ${src})
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${dest} DESTINATION bin)
  message("-- Installing ${dest} to: " ${CMAKE_INSTALL_PREFIX}/bin)
endif()
