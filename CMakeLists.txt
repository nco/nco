#CMake build, Pedro Vicente 
#command line options for library dependencies
#-DFIND_SZIP=BOOL try to locate SZIP library (default ON)
#mininum library dependencies
#-DNETCDF_INCLUDE=PATH
#-DNETCDF_LIBRARY=FILE
#-DHDF5_LIBRARY=FILE
#-DHDF5_HL_LIBRARY=FILE
#-DZLIB_LIBRARY=FILE
#-DSZIP_LIBRARY=FILE
#-DCURL_LIBRARY=FILE
#/////////////////////////////////////////////////////////////////////////////////////
#ncap2 
#/////////////////////////////////////////////////////////////////////////////////////
#options
#command line options for library dependencies
#-DANTLR_INCLUDE=PATH
#-DANTLR_LIBRARY=FILE

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
option(FIND_SZIP "Try to link with SZIP library" ON)

#//////////////////////////
#preprocessor definitions
#a netCDF4 library version is assumed
#//////////////////////////

if (MSVC)

  #//////////////////////////
  #visual studio defines math symbols in math.h, avoid duplicate definition
  #//////////////////////////
  
  add_definitions(-D_MATH_DEFINES_DEFINED)
endif(MSVC)

#//////////////////////////
#detect minimum dependencies
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

#//////////////////////////
#detect optional dependencies
#//////////////////////////

#//////////////////////////
# UDUNITS2 (and EXPAT)
#//////////////////////////

find_path(UDUNITS2_INCLUDE_FOUND udunits2.h HINTS ${UDUNITS2_INCLUDE})
if(NOT UDUNITS2_INCLUDE_FOUND)
  message("-- Unidata units library header files not found")
else()
  message("-- Found Unidata units library header files at: " ${UDUNITS2_INCLUDE_FOUND})
endif()

find_library(UDUNITS2_LIBRARY NAMES udunits2)
if(NOT UDUNITS2_LIBRARY)
  message("-- Unidata units library not found")
else()
  message("-- Found Unidata units library at: " ${UDUNITS2_LIBRARY})
endif()

if (UDUNITS2_LIBRARY AND UDUNITS2_INCLUDE_FOUND)
  # UDUnits depends on expat
  find_library(EXPAT_LIBRARY NAMES libexpatMT expat)
  if(NOT EXPAT_LIBRARY)
    message("-- Expat library not found")
  else()
    message("-- Found Expat library at: " ${EXPAT_LIBRARY})
    message("-- Building with UDUNITS support")
  endif()
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
#link with libraries
#//////////////////////////

set(LIB_DEP ${LIB_DEP})
set(LIB_DEP ${LIB_DEP} ${NETCDF_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${HDF5_HL_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${HDF5_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${CURL_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${ZLIB_LIBRARY})
set(LIB_DEP ${LIB_DEP} ${SZIP_LIBRARY})

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

if (MSVC)
  set(LIB_DEP ${LIB_DEP} ${WINSOCK_LIBRARY})
else (MSVC)
  set(LIB_DEP ${LIB_DEP} m)
endif(MSVC)

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

if (UNIX)
  find_package(BISON)
  find_package(FLEX) 
  if(BISON_FOUND AND FLEX_FOUND)
    message("-- Bison found at ${BISON_EXECUTABLE}")
    message("-- Flex found at ${FLEX_EXECUTABLE}")
    BISON_TARGET(nco_parser src/nco/ncap_yacc.y ${CMAKE_CURRENT_BINARY_DIR}/ncap_yacc.c COMPILE_FLAGS "--name-prefix=nco_yy")
    FLEX_TARGET(nco_scanner src/nco/ncap_lex.l ${CMAKE_CURRENT_BINARY_DIR}/ncap_lex.c COMPILE_FLAGS "-Pnco_yy")
    ADD_FLEX_BISON_DEPENDENCY(nco_scanner nco_parser)
    include_directories(${CMAKE_CURRENT_BINARY_DIR})
    include_directories(src/nco)
    message("-- Bison/Flex will generate ${BISON_nco_parser_OUTPUTS} ${FLEX_nco_scanner_OUTPUTS}")
    add_executable(ncwa src/nco/ncwa.c ${BISON_nco_parser_OUTPUTS} ${FLEX_nco_scanner_OUTPUTS} src/nco/ncap_utl.c)
  endif()  
endif(UNIX)

if (MSVC)
add_executable(ncwa src/nco/ncwa.c)
endif(MSVC)

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
  message("-- ANTLR header files not found")
else()
  message("-- Found ANTLR header files at: " ${ANTLR_INCLUDE_FOUND})
endif()

find_library(ANTLR_LIBRARY NAMES antlr)
if(NOT ANTLR_LIBRARY)
  message("-- ANTLR library not found")
else()
  message("-- Found ANTLR library at: " ${ANTLR_LIBRARY})
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
#//////////////////////////

if(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)
  find_path(GSL_INCLUDE_FOUND gsl/gsl_errno.h HINTS ${GSL_INCLUDE})
  if(NOT GSL_INCLUDE_FOUND)
    message("-- GSL header files not found")
  else()
    message("-- Found GSL header files at: " ${GSL_INCLUDE_FOUND})
  endif()
  find_library(GSL_LIBRARY NAMES gsl HINTS libgsl_d.lib)
  if(NOT GSL_LIBRARY)
    message("-- GSL library not found")
  else()
    message("-- Found GSL library at: " ${GSL_LIBRARY})
  endif()
  if(GSL_INCLUDE_FOUND AND GSL_LIBRARY)
  add_definitions(-DENABLE_GSL)   
  include_directories(${GSL_INCLUDE_FOUND})  
  set(LIB_DEP ${LIB_DEP} ${GSL_LIBRARY}) 
  message("-- Building with GSL support")
  endif (GSL_INCLUDE_FOUND AND GSL_LIBRARY)
endif(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)

#//////////////////////////
# ncap2
#//////////////////////////

if(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)
  add_executable(ncap2 ${ncap2_SOURCES})
  target_include_directories(ncap2 PRIVATE src/nco ${ANTLR_INCLUDE_FOUND})
  target_link_libraries (ncap2 ${nco_LIB} ${LIB_DEP} ${ANTLR_LIBRARY}) 
endif()

# add the install targets
if (UNIX)
set(NCO_TARGETS ${NCO_TARGETS})
set(NCO_TARGETS ${NCO_TARGETS} ncks ncbo ncecat ncflint ncpdq ncra ncrename ncatted ncwa)
if(ANTLR_INCLUDE_FOUND AND ANTLR_LIBRARY)
  set(NCO_TARGETS ${NCO_TARGETS} ncap2)
endif()
message("-- Installing ${NCO_TARGETS} to: " ${CMAKE_INSTALL_PREFIX}/bin)
install (TARGETS ${NCO_TARGETS} RUNTIME DESTINATION bin)
endif()



