#CMake build, Pedro Vicente 
#options
#FIND_SZIP try to locate SZIP library (default ON)
#library dependencies
#NETCDF_INCLUDE
#NETCDF_LIBRARY
#HDF5_LIBRARY
#HDF5_LIBRARY
#HDF5_HL_LIBRARY
#ZLIB_LIBRARY
#SZIP_LIBRARY
#CURL_LIBRARY

#///////////////////////////////////////////////////////
#logic for finding header files and library dependencies:
#find_path
#find_library
#are used (with optional location hints); the first argument contains the found file if found.
#and the message "-- Found file at location" is made
#if not found the first argument variable is the same as a user option that contains the file location full path.
#the same message "-- Found file at location" is made
#but there is not an attempt at this time to validate the user input file (.h or .a)
#///////////////////////////////////////////////////////

cmake_minimum_required(VERSION 2.8)
project (nco)
set(BUILD_SHARED_LIBRARIES OFF)

#//////////////////////////
#preprocessor definitions
#//////////////////////////

add_definitions(-DHAVE_NETCDF4_H)
if (MSVC)

  #//////////////////////////
  #visual studio defines math symbols in math.h, avoid duplicate definition
  #//////////////////////////
  
  add_definitions(-D_MATH_DEFINES_DEFINED)
endif(MSVC)

#//////////////////////////
#detect dependencies
#//////////////////////////

find_path(NETCDF_INCLUDE_FOUND netcdf.h HINTS ${NETCDF_INCLUDE} "/usr/include")
if(NOT NETCDF_INCLUDE_FOUND)
  message(FATAL_ERROR "netcdf.h header file not found")
else()
  message("-- Found netcdf.h header file at: " ${NETCDF_INCLUDE_FOUND})
endif()

find_library(NETCDF_LIBRARY NAMES netcdf)
if(NOT NETCDF_LIBRARY)
  message(FATAL_ERROR "netcdf library not found")
else()
  message("-- Found netcdf library at: " ${NETCDF_LIBRARY})
endif()

find_library(HDF5_LIBRARY NAMES hdf5)
if(NOT HDF5_LIBRARY)
  message(FATAL_ERROR "hdf5 library not found")
else()
  message("-- Found hdf5 library at: " ${HDF5_LIBRARY})
endif()

find_library(HDF5_HL_LIBRARY NAMES hdf5_hl)
if(NOT HDF5_HL_LIBRARY)
  message(FATAL_ERROR "hdf5 high level library not found")
else()
  message("-- Found hdf5 high level library at: " ${HDF5_HL_LIBRARY})
endif()

find_library(ZLIB_LIBRARY NAMES z)
if(NOT ZLIB_LIBRARY)
  message(FATAL_ERROR "zlib library not found")
else()
  message("-- Found zlib library at: " ${ZLIB_LIBRARY})
endif()

option(FIND_SZIP "Try to link with SZIP libarry" ON)

if (FIND_SZIP)
message("-- Trying to find SZIP library...")
find_library(SZIP_LIBRARY NAMES sz)
if(NOT SZIP_LIBRARY)
  message(FATAL_ERROR "szip library not found")
else()
  message("-- Found szip library at: " ${SZIP_LIBRARY})
endif()
endif(FIND_SZIP)

find_library(CURL_LIBRARY NAMES curl)
if(NOT CURL_LIBRARY)
  message(FATAL_ERROR "curl library not found")
else()
  message("-- Found curl library at: " ${CURL_LIBRARY})
endif()

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
#compiler includes
#//////////////////////////

include_directories(${NETCDF_INCLUDE_FOUND})

#//////////////////////////
#link with libraries
#//////////////////////////

set(LIB_DEP ${LIB_DEP} ${NETCDF_LIBRARY} ${HDF5_HL_LIBRARY} ${HDF5_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${CURL_LIBRARY} ${ZLIB_LIBRARY} ${SZIP_LIBRARY})

if (MSVC)
  set(LIB_DEP ${LIB_DEP} ${WINSOCK_LIBRARY})
else (MSVC)
  set(LIB_DEP ${LIB_DEP} m)
endif(MSVC)

#//////////////////////////
#nco lib sources 
#//////////////////////////

set(SRC ${SRC} src/nco/libnco.h)
set(SRC ${SRC} src/nco/nco_att_utl.c src/nco/nco_att_utl.h)
set(SRC ${SRC} src/nco/nco_aux.c src/nco/nco_aux.h)
set(SRC ${SRC} src/nco/nco_bnr.c src/nco/nco_bnr.h)
set(SRC ${SRC} src/nco/nco_cln_utl.c src/nco/nco_cln_utl.h)
set(SRC ${SRC} src/nco/nco_cnf_dmn.c src/nco/nco_cnf_dmn.h)
set(SRC ${SRC} src/nco/nco_cnf_typ.c src/nco/nco_cnf_typ.h)
set(SRC ${SRC} src/nco/nco_cnk.c src/nco/nco_cnk.h)
set(SRC ${SRC} src/nco/nco_cnv_arm.c src/nco/nco_cnv_arm.h)
set(SRC ${SRC} src/nco/nco_cnv_csm.c src/nco/nco_cnv_csm.h)
set(SRC ${SRC} src/nco/nco_ctl.c src/nco/nco_ctl.h)
set(SRC ${SRC} src/nco/nco_dbg.c src/nco/nco_dbg.h)
set(SRC ${SRC} src/nco/nco_dmn_utl.c src/nco/nco_dmn_utl.h)
set(SRC ${SRC} src/nco/nco_fl_utl.c src/nco/nco_fl_utl.h)
set(SRC ${SRC} src/nco/nco_getopt.c src/nco/nco_getopt.h)
set(SRC ${SRC} src/nco/nco_grp_trv.c src/nco/nco_grp_trv.h)
set(SRC ${SRC} src/nco/nco_grp_utl.c src/nco/nco_grp_utl.h)
set(SRC ${SRC} src/nco/nco.h)
set(SRC ${SRC} src/nco/nco_lmt.c src/nco/nco_lmt.h)
set(SRC ${SRC} src/nco/nco_lst_utl.c src/nco/nco_lst_utl.h)
set(SRC ${SRC} src/nco/nco_md5.c src/nco/nco_md5.h)
set(SRC ${SRC} src/nco/nco_mmr.c src/nco/nco_mmr.h)
set(SRC ${SRC} src/nco/nco_mpi.h)
set(SRC ${SRC} src/nco/nco_msa.c src/nco/nco_msa.h)
set(SRC ${SRC} src/nco/nco_mss_val.c src/nco/nco_mss_val.h)
set(SRC ${SRC} src/nco/nco_mta.c src/nco/nco_mta.h)
set(SRC ${SRC} src/nco/nco_netcdf.c src/nco/nco_netcdf.h)
set(SRC ${SRC} src/nco/nco_omp.c src/nco/nco_omp.h)
set(SRC ${SRC} src/nco/nco_pck.c src/nco/nco_pck.h)
set(SRC ${SRC} src/nco/nco_ppc.c src/nco/nco_ppc.h)
set(SRC ${SRC} src/nco/nco_prn.c src/nco/nco_prn.h)
set(SRC ${SRC} src/nco/nco_rec_var.c src/nco/nco_rec_var.h)
set(SRC ${SRC} src/nco/nco_rgr.c src/nco/nco_rgr.h)
set(SRC ${SRC} src/nco/nco_rth_flt.c src/nco/nco_rth_flt.h)
set(SRC ${SRC} src/nco/nco_rth_utl.c src/nco/nco_rth_utl.h)
set(SRC ${SRC} src/nco/nco_scl_utl.c src/nco/nco_scl_utl.h)
set(SRC ${SRC} src/nco/nco_scm.c src/nco/nco_scm.h)
set(SRC ${SRC} src/nco/nco_sld.c src/nco/nco_sld.h)
set(SRC ${SRC} src/nco/nco_sng_utl.c src/nco/nco_sng_utl.h)
set(SRC ${SRC} src/nco/nco_srm.c src/nco/nco_srm.h)
set(SRC ${SRC} src/nco/nco_typ.h)
set(SRC ${SRC} src/nco/nco_uthash.h)
set(SRC ${SRC} src/nco/nco_var_avg.c src/nco/nco_var_avg.h)
set(SRC ${SRC} src/nco/nco_var_lst.c src/nco/nco_var_lst.h)
set(SRC ${SRC} src/nco/nco_var_rth.c src/nco/nco_var_rth.h)
set(SRC ${SRC} src/nco/nco_var_scv.c src/nco/nco_var_scv.h)
set(SRC ${SRC} src/nco/nco_var_utl.c src/nco/nco_var_utl.h)

add_executable(ncks src/nco/ncks.c ${SRC})
add_executable(ncbo src/nco/ncbo.c ${SRC})
add_executable(ncecat src/nco/ncecat.c ${SRC})
add_executable(ncflint src/nco/ncflint.c ${SRC})
add_executable(ncpdq src/nco/ncpdq.c ${SRC})
add_executable(ncra src/nco/ncra.c ${SRC})
add_executable(ncrename src/nco/ncrename.c ${SRC})

target_link_libraries (ncks ${LIB_DEP})
target_link_libraries (ncbo ${LIB_DEP})
target_link_libraries (ncecat ${LIB_DEP})
target_link_libraries (ncflint ${LIB_DEP})
target_link_libraries (ncpdq ${LIB_DEP})
target_link_libraries (ncra ${LIB_DEP})
target_link_libraries (ncrename ${LIB_DEP})

#//////////////////////////
#ncap2 
#//////////////////////////

set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/Invoke.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2_utl.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/ncap2_att.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/sdo_utl.cc)
set(ncap2_SOURCES ${ncap2_SOURCES} src/nco++/sym_cls.cc)
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

add_executable(ncap2 ${ncap2_SOURCES})
target_include_directories(ncap2 PRIVATE src/nco)
target_link_libraries (ncap2 ${LIB_DEP})



