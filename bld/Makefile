# $Header$ -*-makefile-*-

# Purpose: GNU Makefile for NCO module nco
# Requires GNU Make---AT&T Make chokes on GNU syntax

# Copyright (C) 1994--present Charlie Zender
# License: 3-Clause BSD License
# See https://opensource.org/licenses/BSD-3-Clause for full license text

# Quickie test copies:
# scp ~/nco/bld/Makefile dust.ess.uci.edu:nco/bld
# scp ~/nco/bld/Makefile grele.ess.uci.edu:nco/bld
# scp ~/nco/bld/Makefile gplogin2.ps.uci.edu:nco/bld
# scp ~/nco/bld/Makefile skyglow.ess.uci.edu:nco/bld

# Machine build parameters
#                         Source     Method   Packages Compiler
# aerosol.ess.uci.edu: NCO          Configure All else GCC
# andes.olcf.ornl.gov: NCO          Makefile  All else GCC
# AppVeyor CI        : NCO          CMake     All else MSVC
# cheyenne.ucar.edu  : NCO          Makefile  All else Intel
# cooley.alcf.anl.g  : NCO          Configure All else GCC
# edison.nersc.gov   : NCO          Configure All else Intel
# firn.ess.uci.edu   : NCO          Configure All else GCC
# frazil.ess.uci.edu : NCO          Configure All else clang
# glace.ess.uci.edu  : NCO          CMake     All else GCC
# grele.ess.uci.edu  : NCO + netCDF Makefile  All else GCC
# katabatic.ess.uci.e: NCO          CMake     All else clang
# rhea.ccs.ornl.gov  : NCO          Makefile  All else GCC
# skyglow.ess.uci.edu: NCO + netCDF Configure All else GCC
# Travis CI          : NCO          Configure All else gcc,clang,g++

# Usage (testing):
# make tst                           # Vanilla regression test
# make FL_FMT=netcdf4 tst            # netCDF4 regression test
# make MPI_PRC=3 tst                 # MPI regression test
# make MPI_PRC=3 bm                  # MPI benchmarks
# make THR_NBR=2 tst                 # OpenMP regression test
# make THR_NBR=2 bm                  # OpenMP benchmarks
# make MPI=Y                         # MPI features (HDF5 and PnetCDF I/O)
# make NETCDF4=Y                     # netCDF4 features
# make PNETCDF=Y                     # PnetCDF features

# Usage (Compilation):
# cd ~/nco/qt;qmake;make;cd -        # Qt
# cd ~/nco/bld;make;cd -             # Default build
# cd ~/nco/bld;make USR_TKN='-DHAVE_NC_RENAME_GRP';cd - # New API
# cd ~/nco/bld;make UDUNITS_INC='/usr/include/udunits2';cd - # Fedora build
# cd ~/nco/bld;make cln bin_cln;cd - # Clean all dependencies for fresh build
# cd ~/nco/bld;make dir all; cd -    # Create target directories then build
# cd ~/nco/bld;make dbg;cd -         # Print make diagnostics
# cd ~/nco/bld;make DPKG=Y;cd -      # Debian hardening
# cd ~/nco/bld;make ESMF=Y;cd -      # ESMF regridding
# cd ~/nco/bld;make GSL=Y;cd -	     # GSL support
# cd ~/nco/bld;make I18N=Y;cd -      # Internationalization
# cd ~/nco/bld;make lib_cln;cd -     # Clean libraries
# cd ~/nco/bld;make MPI=Y;cd -       # MPI support
# cd ~/nco/bld;make MPI_FAKE=Y;cd -  # Spoof MPI support
# cd ~/nco/bld;make OMP=Y;cd -       # OpenMP support
# cd ~/nco/bld;make OPTS=D;cd -      # "Debugging": Enough symbols for debugging (default)
# cd ~/nco/bld;make OPTS=O;cd -      # "Optimize": For production (includes symbols)
# cd ~/nco/bld;make OPTS=R;cd -      # "Regular": No optimization or debugging
# cd ~/nco/bld;make OPTS=X;cd -      # "eXtreme": All debugging switches available
# cd ~/nco/bld;make sys;cd -         # Install in /usr/local (must sudo)
# cd ~/nco/bld;make SZ=Y;cd -	     # Szip support
# cd ~/nco/bld;make UDUNITS=Y;cd -   # UDUnits support
# cd ~/nco/bld;make --jobs=4;cd -    # Parallel make
# Normal (Linux) developer systems:
# cd ~/nco/bld;make cln all ncap2;cd - # New default for most machines
# cd ~/nco/bld;make OMP=Y CUDA=Y OPTS=D NETCDF4=Y UDUNITS=Y allinone;cd - # givre
# cd ~/nco/bld;make OMP=Y OPTS=D NETCDF4=Y UDUNITS=Y allinone;cd - # givre, neige
# cd ~/nco/bld;make OMP=Y OPTS=D NETCDF4=N UDUNITS=Y allinone;cd - # glace, virga
# mpi-selector --set openmpi_gcc-1.3.3 # Then use /sopt/netcdf/netcdf3-gcc-serial serial libraries
# mpi-selector --set mpich_pgi_1.2.7p1 # After compiling
# cd ~/nco/bld;ANTLR_ROOT=/sopt/gfortran_g++ NETCDF_ROOT=/sopt/gfortran_g++ SZ_LIB=/sopt/lib UDUNITS_INC=/sopt/include UDUNITS_LIB=/sopt/lib make OMP=Y OPTS=D NETCDF4=Y SZ=Y UDUNITS=N allinone;cd - # greenplanet gcc
# cd ~/nco/bld;ANTLR=/home/pvicente/install/antlr-2.7.7/bin/antlr ANTLR_ROOT=/home/pvicente/install/antlr-2.7.7 NETCDF_ROOT=/home/pvicente/install/netcdf-4.3.0 UDUNITS_INC=/home/pvicente/install/udunits-2.1.24/include UDUNITS_LIB=/home/pvicente/install/udunits-2.1.24/lib make OPTS=D allinone;cd - # greenplanet gcc pedro
# cd ~/nco/bld;ANTLR=/sopt/ifort_icpc/bin/antlr ANTLR_ROOT=/sopt/ifort_icpc SZ_LIB=/sopt/lib UDUNITS_INC=/sopt/include UDUNITS_LIB=/sopt/lib make OMP=Y OPTS=D NETCDF4=Y SZ=Y UDUNITS=N allinone;cd - # greenplanet intel
# cd ~/nco/bld;ANTLR=/sopt/pgf90_pgcc/bin/antlr ANTLR_ROOT=/sopt/pgf90_pgcc SZ_LIB=/sopt/lib UDUNITS_INC=/sopt/include UDUNITS_LIB=/sopt/lib make OMP=Y OPTS=D NETCDF4=N SZ=Y UDUNITS=N allinone;cd - # greenplanet pgi
# cd ~/nco/bld;make OMP=Y OPTS=D NETCDF4=N UDUNITS=Y allinone;cd - # pbs
# cd ~/nco/bld;make OMP=Y OPTS=D UDUNITS=Y UBUNTU_440=Y allinone # grele
# cd ~/nco/bld;make NETCDF4=N all ncap2;cd - # tephra
# cd ~/nco/bld;make OMP=Y OPTS=D NETCDF4=N UDUNITS=N allinone;cd - # silt, clay
# cd ~/nco/bld;NETCDF_INC='/usr/include/netcdf-3' NETCDF_LIB='/usr/lib64' make GSL=N OMP=N OPTS=D NETCDF4=N UDUNITS=N allinone;cd - # snow
# cd ~/nco/bld;make OMP=N OPTS=D NETCDF4=N UDUNITS=N USR_TKN='-DNEED_NC_INQ_FORMAT' allinone;cd - # snow w/OPeNDAP
# cd ~/nco/bld;make OPTS=D USR_TKN='-DNEED_NC_INQ_FORMAT' allinone;cd -
# 64-bit ABI on UCI MPC:
# cd ~/nco/bld;env ANTLR='/software/antlr/bin/antlr' ANTLR_ROOT='/software/antlr' UDUNITS_INC='/software/udunits/include' UDUNITS_LIB='/software/udunits/lib' make --jobs=1 ABI=64 allinone;cd -
# 64-bit ABI on UCI IPCC:
# cd ~/nco/bld;env ANTLR='/usr/local/pgi/bin/antlr' ANTLR_ROOT='/usr/local/pgi' make --jobs=1 ABI=64 allinone;cd -
# 64-bit ABI netCDF3 on NCAR AIX systems (bluefire):
# cd ~/nco/bld;ANTLR='/contrib/antlr-2.7.7/bin/antlr' ANTLR_ROOT='/contrib/antlr-2.7.7' GSL_INC='/contrib/gsl-1.12/include' GSL_LIB='/contrib/gsl-1.12/lib' NETCDF_LIB='/usr/local/lib64/r4i4' UDUNITS_INC='/contrib/udunits-1.12.9/include' UDUNITS_LIB='/contrib/udunits-1.12.9/lib' make --jobs=1 OPTS=D NETCDF4=N UDUNITS=N allinone ABI=64;cd -
# 64-bit ABI netCDF4 on NCAR AIX systems (bluefire):
# cd ~/nco/bld;ANTLR='/contrib/antlr-2.7.7/bin/antlr' ANTLR_ROOT='/contrib/antlr-2.7.7' CURL_LIB='/contrib/curl/7.21.2/lib' GSL_INC='/contrib/gsl-1.12/include' GSL_LIB='/contrib/gsl-1.12/lib' HDF5_ROOT='/contrib/hdf5-1.8.7_seq' LDFLAGS='-lnetcdf -lhdf5_hl -lhdf5 -lz' NETCDF_ROOT='/contrib/netcdf/4.1.3_seq' SZ_LIB='/contrib/szip/lib' UDUNITS_LIB='/contrib/zlib/lib' make --jobs=1 OPTS=D SZ=Y allinone ABI=64;cd -
# 64-bit ABI netCDF4 on NCAR AIX systems (bluefire):
# cd ~/bin/AIX;/usr/local/bin/tar cvzf ~/nco-4.4.6.aix53.tar.gz nc*;scp ~/nco-4.4.6.aix53.tar.gz zender,nco@web.sf.net:/home/project-web/nco/htdocs/src
# netCDF4 on NCAR Linux cluster systems (mirage):
# cd ~/nco/bld;make OPTS=D allinone;cd -
# netCDF4 on ORNL Linux cluster systems (titan, rhea.ccs.ornl.gov):
# module avail
# . ~/.bashrc
# module add intel gsl
# export LD_LIBRARY_PATH='/sw/redhat6/netcdf/4.1.3/rhel6.4_intel13.1.3/lib:/sw/redhat6/szip/2.1/rhel6.6_gnu4.8.2/lib':${LD_LIBRARY_PATH}
# export NETCDF_ROOT='/sw/redhat6/netcdf/4.1.3/rhel6.4_intel13.1.3' 
# export PATH='/sw/redhat6/netcdf/4.1.3/rhel6.4_intel13.1.3/bin':${PATH}
# cd ~/nco/bld;make ANTLR_ROOT=${HOME} NETCDF_ROOT='/sw/redhat6/netcdf/4.1.3/rhel6.4_intel13.1.3' SZ=Y SZ_LIB='/sw/redhat6/szip/2.1/rhel6.6_gnu4.8.2/lib' UDUNITS_INC='/sw/redhat6/udunits/2.1.24/rhel6.4_intel13.1.3/include' UDUNITS_LIB='/sw/redhat6/udunits/2.1.24/rhel6.4_intel13.1.3/lib' OPTS=D allinone;cd -
# ncap2:
# netCDF4 on NCAR Linux cluster systems (yellowstone):
# /ncar/opt/hpss/hsi # Starts HPSS session
# export PATH=${PATH}\:/ncar/opt/hpss
# GNU (works):
# module swap intel gnu;module add gsl;module add netcdf/4.3.0;
# export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}\:/glade/apps/opt/udunits/2.1.24/gnu/default/lib
# cd ~/nco/bld;make NETCDF_ROOT='/glade/apps/opt/netcdf/4.3.0/gnu/default' UDUNITS_INC='/glade/apps/opt/udunits/2.1.24/gnu/default/include' UDUNITS_LIB='/glade/apps/opt/udunits/2.1.24/gnu/default/lib' OPTS=D allinone;cd -
# Intel (works as of 20140129):
# module add intel;module add gsl;module add netcdf/4.3.0;
# https://www2.cisl.ucar.edu/sites/default/files/Yellowstone_Started_Nov30-2.pdf
# module add pnetcdf/1.4.1
# export PATH=${PATH}:/glade/apps/opt/netcdf/4.3.0/intel/12.1.5/bin # needed for ncgen
# export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:/glade/apps/opt/netcdf/4.3.0/intel/12.1.5/lib # needed to run NCO
# cd ~/nco/bld;make NETCDF_ROOT='/glade/apps/opt/netcdf/4.3.0/intel/default' UDUNITS_INC='/glade/apps/opt/udunits/2.1.24/intel/12.1.4/include' UDUNITS_LIB='/glade/apps/opt/udunits/2.1.24/intel/12.1.4/lib' OPTS=D allinone;cd -
# Cygwin on Windows Vista systems:
# cd ~/nco/bld;ANTLR='antlr' make --jobs=1 GSL=Y OPTS=D NETCDF4=Y UDUNITS=Y allinone;cd -
# cd ~/bin/WIN32;tar cvzf ~/nco-4.5.2.win32.cygwin.tar.gz nc* ; scp ~/nco-4.5.2.win32.cygwin.tar.gz zender,nco@web.sf.net:/home/project-web/nco/htdocs/src
# Data files to dust
# cd ~/nco/data;scp cmip5.nc obs.nc mdl_1.nc mdl_2.nc in_grp*.nc in.nc in_4c.nc dust.ess.uci.edu:/var/www/html/nco

# Top-level tokens defining directory structure
# These tokens may be over-ridden by environment variables or when invoking make, e.g., make OMP=Y
MY_BLD_DIR := ../bld
ifndef PVM_ARCH
 PVM_ARCH := $(shell ${MY_BLD_DIR}/pvmgetarch)
endif
ifndef MY_BIN_DIR
 MY_BIN_DIR := ../bin
endif
ifndef MY_LIB_DIR
 MY_LIB_DIR := ../lib
endif
ifndef MY_MAN_DIR
 MY_MAN_DIR := ../man
endif
ifndef MY_OBJ_DIR
 MY_OBJ_DIR := ../obj
endif
MY_BM_DIR := ../bm
MY_DAT_DIR := ../data
MY_DBN_DIR := ../debian
MY_DOC_DIR := ../doc
MY_DPN_DIR := ${MY_OBJ_DIR}
MY_SRC_DIR := ../src/nco
MY_INC_DIR := ${MY_SRC_DIR}
NCO_CXX_SRC_DIR := ${MY_SRC_DIR}/../nco_c++
NCOXX_SRC_DIR := ${MY_SRC_DIR}/../nco++

# Primary tokens which determine build options
# Specify non-default when invoking make, e.g. make OMP=Y
ifndef ABI
# 32- vs. 64-bit ABI: 32=32-bit mode, 64=64-bit mode (default) if available
 ABI := 64
endif # endif ABI
ifndef CCACHE
 CCACHE := N
endif # CCACHE
ifndef CNK
# Use newer netCDF4.1 chunking API
 CNK := Y
endif # endif CNK
ifndef CUDA
 CUDA := N
endif # CUDA
ifndef DAP
 DAP := Y
endif # DAP
ifndef DBG
# Debugging token N=No (default) Y=Yes
 DBG := N
endif # endif DBG
ifndef DPKG
# Debian hardening tokens
 DPKG := N
endif # endif DPKG
ifdef FL_FMT
# Pass this netCDF4 argument to nco_bm.pl
 FL_FMT_SNG := "--fl_fmt=${FL_FMT}"
else # endif FL_FMT
 FL_FMT_SNG :=
endif # endif FL_FMT
ifndef GCC_RCH_ARG
 GCC_RCH_ARG :=
endif # endif GCC_RCH_ARG
ifndef GSL
# Use GSL functionality
 GSL := Y
endif # endif GSL
ifndef ICC_RCH_ARG
 ICC_RCH_ARG :=
endif # endif ICC_RCH_ARG
ifndef I18N
 I18N := N
endif
ifndef NCO_YY_PFX
 NCO_YY_PFX := nco_yy
endif
ifndef MK_DPN
 MK_DPN = ${CPP} -M # NB: Recursive expansion required
 MK_DPN_CXX = ${CXX} -M # NB: Recursive expansion required
endif # endif MK_DPN
ifndef MPI # MPI
 MPI := N
endif # endif MPI
ifndef MPI_FAKE # MPI
 MPI_FAKE := N
endif # endif MPI_FAKE
ifndef NCO_BUILDENGINE
 NCO_BUILDENGINE := Traditional \(nco/bld/Makefile\)
endif # NCO_BUILDENGINE
ifndef NCO_VRS
# Used for RPM building
 NCO_VRS := $(shell cat ${MY_DOC_DIR}/VERSION)
endif
ifndef NETCDF4 # netCDF4 support
 NETCDF4 := Y
endif # endif NETCDF4
ifndef NETCDF_ROOT
 NETCDF_ROOT := /usr/local
endif
ifndef NETCDF4_ROOT
 NETCDF4_ROOT := ${NETCDF_ROOT}
endif
ifndef NETCDF_INC
ifdef INC_NCAR
 NETCDF_INC := ${INC_NCAR} # NCAR module path
else # endelse INC_NCAR
 NETCDF_INC := ${NETCDF_ROOT}/include # Directory containing netcdf.h
endif # endif INC_NCAR
endif # endif NETCDF_INC
ifndef NETCDF_LIB
ifdef LIB_NCAR
 NETCDF_LIB := ${LIB_NCAR} # NCAR module path
else # endelse LIB_NCAR
 NETCDF_LIB := ${NETCDF_ROOT}/lib # Directory containing libnetcdf.a
endif # endif LIB_NCAR
endif # endif NETCDF_LIB
ifndef ${OMP} # OpenMP
 OMP := N
endif # endif OMP
ifndef OPTS
 OPTS := D
endif
ifndef PGI_RCH_ARG
 PGI_RCH_ARG :=
endif # endif PGI_RCH_ARG
ifndef PNETCDF # pnetCDF support
 PNETCDF := N
endif # endif PNETCDF
ifndef PSC_RCH_ARG
 PSC_RCH_ARG :=
endif # endif PSC_RCH_ARG
ifndef RPM
 RPM := N
endif # endif RPM
ifndef STC
# Created statically linked executable
 STC := N
endif
ifndef SZ
# Link to Szip library
 SZ := N
endif
ifndef UDUNITS
# Use UDUnits functionality
 UDUNITS := Y
endif
ifndef UNAMES
 UNAMES := $(shell uname -s)
endif
ifndef USR_TKN
 USR_TKN := 
endif # endif USR_TKN
ifndef VRS_SNG
# VRS_SNG := $(shell date +%Y%m%d)
# 20150622
# VRS_SNG := $(shell git describe --abbrev=7 --dirty --always --tags)
 VRS_SNG := $(shell cat ${MY_DOC_DIR}/VERSION)
endif # endif VRS_SNG
ifndef ZNETCDF # znetcdf support
 ZNETCDF := N
endif # endif ZNETCDF
# Derived-tokens based on primary tokens
# These tokens should not be altered by hand

# NB: CPP_TKN is recursively expanded variable, define only when components are ready
CPP_TKN = ${USR_TKN} -DNCO_BUILDENGINE='${NCO_BUILDENGINE}' -D${PVM_ARCH} -DNO_NETCDF_2 -DNCO_VERSION='${VRS_SNG}' -DHOSTNAME='${HOST}' -DUSER='${USER}' -DNCO_ABORT_ON_ERROR
ifndef LFLAGS # Flags for Flex (Lex)
 LFLAGS := -P${NCO_YY_PFX}
endif
ifndef YFLAGS # Flags for Bison (Yacc)
 YFLAGS := -d --name-prefix=${NCO_YY_PFX}
endif

# Internationalize NCO with i18n features
ifeq (${I18N},Y)
 MY_SHR_DIR := ${HOME}/share
 MY_ES_DIR := ${MY_SHR_DIR}/locale/es/LC_MESSAGES
 MY_FR_DIR := ${MY_SHR_DIR}/locale/fr/LC_MESSAGES
endif

# Message Passing Interface (MPI)
ifeq (${MPI_FAKE},Y)
# MPI_FAKE instructs make to compile mpnc*.c operators without defining ENABLE_MPI
# Resulting executables use UP or SMP code not MPI code
# This tests compile, link, execution of MPI mpnc*.c code without using any MPI calls
 MPI := Y
endif # !MPI_FAKE
ifdef MPI_PRC
# MPI_PRC tells test scripts how many MPI processes to spawn
# Pass this MPI argument to nco_bm.pl
 MPI_PRC_SNG := "--mpi_prc=${MPI_PRC}"
# MPI_PRC implies MPI
 MPI := Y
else # !MPI_PRC
 MPI_PRC_SNG :=
endif # !MPI_PRC

# PnetCDF implies MPI
ifeq (${PNETCDF},Y)
 MPI := Y
endif # !PNETCDF

LINUX_OR_MACOSX := N
ifneq (${null},$(findstring LINUX,${PVM_ARCH}))
 LINUX_OR_MACOSX := Y
endif
ifneq (${null},$(findstring MACOSX,${PVM_ARCH}))
 LINUX_OR_MACOSX := Y
endif
# Decide among the plethora of compilers. Use names like LINUX_CC for historical reasons.
ifeq (${LINUX_OR_MACOSX},Y)
ifndef LINUX_CXX
# C++ compiler for Linux
 LINUX_CXX := g++
 #LINUX_CXX := clang
 #LINUX_CXX := como
 #LINUX_CXX := icpc
 #LINUX_CXX := insure
 #LINUX_CXX := pathCC
 #LINUX_CXX := pgCC
endif # endif LINUX_CXX
ifndef LINUX_CC
# C compiler for Linux
 LINUX_CC := gcc -std=c99 -pedantic -D_DEFAULT_SOURCE
 #LINUX_CC := clang
 #LINUX_CC := como --c99
 #LINUX_CC := icc -std=c99 -D_DEFAULT_SOURCE
 #LINUX_CC := insure
 #LINUX_CC := nvcc
 #LINUX_CC := pathcc -std=c99
 #LINUX_CC := pgcc -c9x
endif # endif LINUX_CC
ifndef LINUX_FC
# Fortran compiler for Linux
 #LINUX_FC := g95
 LINUX_FC := gfortran
 #LINUX_FC := ifort
 #LINUX_FC := lf95
 #LINUX_FC := pathf95
 #LINUX_FC := pgf90
endif # endif LINUX_CC
endif # endif LINUX_OR_MACOSX

# OpenMP
ifeq (${OMP},Y)
ifdef THR_NBR
# Pass this OpenMP argument to nco_bm.pl
 THR_NBR_SNG := "--thr_nbr=${THR_NBR}"
else # endif THR_NBR
 THR_NBR_SNG :=
endif # endif THR_NBR
endif # endif OMP

ifeq (${RPM},Y)
# rpm command, and thus RPM variables only guaranteed in RedHat Linux
# Use recursive expansion so rpm command is not executed on non-RPM systems
 MDL_RPM_NST_NM = $(shell rpm -qa | grep nco-) # Name of installed package
# MDL_RPM_PRV_NM = $(shell rpm -qp foo) # Name of package provided by specified RPM
endif # endif RPM

ifeq (${PVM_ARCH},WIN32)
 BNR_SFX := .exe
else
 BNR_SFX := ${null}
endif 

ifeq (${ESMF},Y)
# Build ESMF-enabled NCO 
# Place ESMF block after DAP blocks for both to work together
ifdef ESMF_INC
 ESMF_INC_FLG := -I${ESMF_INC}
else
ifdef ESMF_ROOT
ifeq (${PVM_ARCH},MACOSX)
   ESMF_INC_FLG := -I${ESMF_ROOT}/include/esmf
else
   ESMF_INC_FLG := -I${ESMF_ROOT}/include
endif # endif MACOSX
else
  ESMF_INC_FLG := -I/usr/local/include
endif # endif ESMF_ROOT
endif # endif ESMF_INC
# 20150319: For some reason, ESMC.h #includes ESMC_VM.h which #includes mpi.h
ifeq (${PVM_ARCH},MACOSX)
# Use stub header for uniprocessor MPI emulation provided by ESMF
 ESMF_INC_FLG += -I/Users/zender/data/esmf/src/Infrastructure/stubs/mpiuni
 ESMF_LIB_RT := ${null}
 ESMF_LIB_LAPACK := -llapack
 ESMF_LIB_XERCES := -lxerces-c
else # endif MACOSX
# Use real MPI
 ESMF_INC_FLG += -I/usr/include/openmpi-x86_64
 ESMF_LIB_RT := -lrt
 ESMF_LIB_LAPACK := ${null}
 ESMF_LIB_XERCES := ${null}
endif # endif MACOSX
ifdef ESMF_LIB
 ESMF_LIB_FLG := -L${ESMF_LIB} -lesmf ${ESMF_LIB_LAPACK} ${ESMF_LIB_RT} ${ESMF_LIB_XERCES} -lgfortran -ldl -lnetcdff -lnetcdf_c++
else
ifdef ESMF_ROOT
  ESMF_LIB_FLG := -L${ESMF_ROOT}/lib -lesmf ${ESMF_LIB_LAPACK} ${ESMF_LIB_RT} ${ESMF_LIB_XERCES} -lgfortran -ldl -lnetcdff -lnetcdf_c++
else
  ESMF_LIB_FLG := -L/usr/local/lib -lesmf ${ESMF_LIB_LAPACK} ${ESMF_LIB_RT} ${ESMF_LIB_XERCES} -lgfortran -ldl -lnetcdff -lnetcdf_c++
endif # endif ESMF_ROOT
endif # endif ESMF_LIB
endif # end if ESMF

ifeq (${GSL},Y)
# Build GSL-enabled NCO 
# Place GSL block after DAP blocks for both to work together
ifdef GSL_ROOT
 GSL_CONFIG := ${GSL_ROOT}/bin/gsl-config
else
 GSL_CONFIG := gsl-config
endif
ifdef GSL_INC
 GSL_INC_FLG := -I${GSL_INC}
else
 GSL_INC_FLG := $(shell ${GSL_CONFIG} --cflags)
endif # endif GSL_INC
ifdef GSL_LIB
 GSL_LIB_FLG := -L${GSL_LIB} -lgsl
else
 GSL_LIB_FLG := $(shell ${GSL_CONFIG} --libs)
endif # endif GSL_LIB
ifndef GSL_MAJOR_VERSION
# 20161118: Following line is always accurate
  GSL_MAJOR_VERSION := $(shell ${GSL_CONFIG} --version | cut -c 1 )
endif # endif GSL_MAJOR_VERSION
ifndef GSL_MINOR_VERSION
# gsl-config --version: 1.16 (old GSL versioning)
# gsl-config --version: 2.2.1 (new GSL versioning)
# GSL_MINOR_VERSION := $(shell ${GSL_CONFIG} --version )
  ifeq (${GSL_MAJOR_VERSION},2)
   GSL_MINOR_VERSION := $(shell ${GSL_CONFIG} --version | cut -c 3 )
   GSL_PATCH_VERSION := $(shell ${GSL_CONFIG} --version | cut -c 5 )
  else # !GSL_MAJOR_VERSION
   GSL_MINOR_VERSION := $(shell ${GSL_CONFIG} --version | sed s/^[1-9]\.// )
   GSL_PATCH_VERSION := 0
  endif # !GSL_MAJOR_VERSION
  ifeq (${GSL_MINOR_VERSION},${null})
   GSL_MINOR_VERSION := 4
   GSL_PATCH_VERSION := 0
  endif # end if GSL_MINOR_VERSION
 endif # endif GSL_PATCH_VERSION
 ifndef GSL_PATCH_VERSION
   GSL_PATCH_VERSION := 0
 endif # endif GSL_PATCH_VERSION
endif # end if GSL

ifeq (${SZ},Y)
# 20150515: -lsz needed (at least by icc) on rhea
ifdef SZ_LIB
 SZ_LIB_FLG := -L${SZ_LIB} -lsz
else
 SZ_LIB_FLG := -lsz
endif # end if SZ_LIB
endif # end if SZ

ifeq (${UDUNITS},Y)
# 20130607: -lexpat needed on .deb systems, not on RPM systems
ifdef UDUNITS_INC
 UDUNITS_INC_FLG := -I${UDUNITS_INC}
else
 UDUNITS_INC_FLG := -I/usr/include/udunits2
endif # endif UDUNITS_INC
ifdef UDUNITS_LIB
 UDUNITS_LIB_FLG := -L${UDUNITS_LIB} -ludunits2
else
 UDUNITS_LIB_FLG := -ludunits2
endif # end if UDUNITS_LIB
endif # end if UDUNITS

# At this point we change NETCDF_INC from a directory to a flag
NC_CFLAGS := $(shell ${NETCDF_ROOT}/bin/nc-config --cflags)
NC_LDFLAGS := $(shell ${NETCDF_ROOT}/bin/nc-config --libs)
NCO_LDFLAGS := -L${MY_LIB_DIR} -lnco

ifeq (${MPI},Y)
# Enable MPI functionality
 NC_CFLAGS := $(shell /usr/local/parallel/bin/nc-config --cflags)
 NC_LDFLAGS := $(shell /usr/local/parallel/bin/nc-config --libs)
 MPI_CFLAGS := -I/usr/include/openmpi-x86_64
 MPI_LDFLAGS := -L/usr/lib64/openmpi/lib -lmpi
endif # endif MPI

ifdef LIB_NCAR
 NC_LDFLAGS := ${LIB_NCAR} ${NC_LDFLAGS}
endif

OTHER_CFLAGS :=  ${ESMF_INC_FLG} ${NC_CFLAGS} ${MPI_CFLAGS} ${GSL_INC_FLG} ${UDUNITS_INC_FLG}
OTHER_LDFLAGS := ${NCO_LDFLAGS} ${ESMF_LIB_FLG} ${NC_LDFLAGS} ${MPI_LDFLAGS} ${GSL_LIB_FLG} ${SZ_LIB_FLG} ${UDUNITS_LIB_FLG}

# NB: Do NOT add comment lines, e.g., # This is a comma, to character definitions
null :=
space := ${null} ${null}
comma := ,
newline := \n
# '/' and '+' appear in filenames ('/' is directory separator)
# Operating on these with Perl is problematic since they are special Rx characters
# We replace `/' and '+' by non-special Rx's, call perl, then demangle
# Unique character(s) to substitute for '/' and '+' before passing to perl Rx
slash_rx := cszzsc
plus_rx := xdikmj
# Unique character(s) to replace by ${slash_rx} before passing to perl regex
slash := /
plus := +
MY_OBJ_DIR_RX := $(subst ${slash},${slash_rx},${MY_OBJ_DIR})
MY_DPN_DIR_RX := $(subst ${slash},${slash_rx},${MY_DPN_DIR})

# Directories to search for source files
MDL_PTH := ./ ${MY_SRC_DIR}
# Find all C, C++, CUDA files in given directory
FIND_FNC = $(wildcard ${dir}/*.cc ${dir}/*.c ${dir}/*.cu)
# Assemble source files from all directories
SRC_LST = $(foreach dir, ${MDL_PTH},$(FIND_FNC))
# Source file names with directories removed
MDL_SRC := $(notdir $(SRC_LST))
# Dependency list for executable
MDL_OBJ := $(addprefix ${MY_OBJ_DIR}/,$(addsuffix .o, $(basename ${MDL_SRC}))) 
# Dependency (make) file for each object file
MDL_DPN := $(addprefix ${MY_DPN_DIR}/,$(addsuffix .d, $(basename ${MDL_SRC}))) 
# VPATH helps make find dependencies (which are not pathname qualified) in *.d file
VPATH := $(subst ${space},:,${MDL_PTH})
# Prepend -I to use for compiler argument
CPP_PTH := $(foreach dir,${MDL_PTH},-I${dir})

# Variables having to do with binary executables created by module
MDL_BIN_TRG := ncap ncatted ncbo ncecat ncflint ncks ncpdq ncra ncrename ncwa # NCO binary targets
MDL_BIN_SYM_LNK := ncdiff ncea nces ncrcat # Symbolic links
MDL_BIN_SPT := ncclimo ncremap # Scripts
ifeq (${MPI},Y)
# MDL_MPI_TRG := mpncbo mpncecat mpncflint mpncpdq mpncra mpncwa # MPI binary targets
 MDL_MPI_TRG := # MPI binary targets
 MDL_MPI_TRG_SMP := ncbo ncecat ncflint ncpdq ncra ncwa # MPI binary targets
# MDL_MPI_SYM_LNK := mpncdiff mpncea mpnces mpncrcat # MPI Symbolic links
 MDL_MPI_SYM_LNK := # MPI Symbolic links
 MDL_MPI_STB := ${MDL_MPI_TRG} ${MDL_MPI_SYM_LNK} # All MPI files in MY_BIN_DIR
 MDL_MPI_BIN := $(addprefix ${MY_BIN_DIR}/,${MDL_MPI_TRG}) # mpi_cln removes these files
 MDL_MPI_OBJ := $(addsuffix .o,$(addprefix ${MY_OBJ_DIR}/,${MDL_MPI_TRG})) # mpi_cln removes these files
 MDL_BIN_TRG += ${MDL_MPI_TRG} # NCO binary targets
 MDL_BIN_SYM_LNK += ${MDL_MPI_SYM_LNK} # Symbolic links
endif # endif MPI
MDL_BIN_STB := ${MDL_BIN_TRG} ${MDL_BIN_SPT} ${MDL_BIN_SYM_LNK} # All NCO files in MY_BIN_DIR
MDL_BIN := $(addprefix ${MY_BIN_DIR}/,${MDL_BIN_STB}) # distclean removes these files
MDL_SPT_SRC := $(addprefix ${MY_DAT_DIR}/,${MDL_BIN_SPT})

# Variables having to do with header files created by module
# List header targets alphabetically by "category":
MDL_INC_TRG := # Raw (no functions)
MDL_INC_TRG += libnco.h # libnco
MDL_INC_SYM_LNK := # Symbolic links
MDL_INC_STB = ${MDL_INC_TRG} ${MDL_INC_SYM_LNK} # All header files in ${MY_INC_DIR}
MDL_INC = $(addprefix ${MY_INC_DIR}/,${MDL_INC_STB}) # dst_cln removes these files

# Variables having to do with NCO data
MDL_DAT_STB := 85 86 87 88 89 h0001 h0002 h0003 # Symbolic links to in.nc
MDL_DAT_STB := $(addsuffix .nc,${MDL_DAT_STB}) # `make data' creates these files
MDL_DAT := $(addprefix ${MY_DAT_DIR}/,${MDL_DAT_STB}) # `make distclean' removes these files

# Variables having to do with NCO documentation
MDL_DOC_SRC := $(addprefix ../,acinclude.m4 CMakeLists.txt configure.ac configure.eg Makefile.am) $(addprefix ${MY_DOC_DIR}/,nco.texi ANNOUNCE MANIFEST NEWS README TODO VERSION beta.txt debian.txt dods.sh index.shtml xmp_cesm.html man_end.txt man_srt.txt ncap.txt nco_news.shtml nco_src_frg.txt opendap.sh) $(addprefix ${MY_DBN_DIR}/,changelog compat control copyright files info rules doc-base) # `make tags' includes these files
MDL_DOC_TRG := nco.dvi nco.html nco.info nco.pdf nco.ps nco.txt nco.xml # `make doc' creates these files
MDL_DOC := $(addprefix ${MY_DOC_DIR}/,${MDL_DOC_TRG}) # `make distclean' removes these files

MDL_MAN := $(wildcard ${MY_MAN_DIR}/*.1)
MDL_MAN := $(notdir ${MDL_MAN})
MDL_MAN := $(addprefix ${MY_MAN_DIR}/,${MDL_MAN}) # distclean removes these files

# Variables having to do with NCO build
MDL_BLD_SRC := $(addprefix ${MY_BLD_DIR}/,libnco_tst.c libnco_c++_tst.cc Makefile nco.spec nco_dst.pl) # `make tags' includes these files
MDL_BLD_SRC += $(addprefix ${MY_BM_DIR}/,NCO_bm.pm NCO_benchmarks.pm NCO_rgr.pm nco_bm.pl nco_bm.sh mk_bm_plots.pl) # `make tags' includes these files
MDL_BLD_SRC += $(addprefix ${MY_DAT_DIR}/,Makefile.am) $(addprefix ${MY_SRC_DIR}/,Makefile.am) $(addprefix ${MY_DOC_DIR}/,Makefile.am) $(addprefix ${NCO_CXX_SRC_DIR}/,Makefile.am)  $(addprefix ${NCOXX_SRC_DIR}/,Makefile.am) # `make tags' includes these files
# Files, if any, to exclude from tags
#TAGS_FILTER_FILES := .//libnco_tst.c .//libnco_c++_tst.cc ../src/nco/lex.${NCO_YY_PFX}.c
TAGS_FILTER_FILES := .//libnco_tst.c .//libnco_c++_tst.cc ../bld/lex.${NCO_YY_PFX}.c
# TAGS_FILTER_FILES := .//libnco_tst.c .//libnco_c++_tst.cc

# Variables having to do with ncap
MDL_NCAP_SRC := $(addprefix ${MY_SRC_DIR}/,ncap_lex.l ncap_utl.h) # `make tags' includes these files
MDL_NCAP_TRG := ncap_lex.c # `make ncap' creates these files
MDL_NCAP := $(addprefix ${MY_SRC_DIR}/,${MDL_NCAP_TRG}) # `make distclean' removes these files

# Variables having to do with C++ source and documentation
# fxm: auto-generate source code components of C++ lists
MDL_CXX_SRC := $(addprefix ${NCOXX_SRC_DIR}/,libnco++.hh Makefile.am Makefile.old ncap2.cc ncap2.hh ncap2_utl.cc NcapVar.cc NcapVar.hh NcapVarVector.cc NcapVarVector.hh NcapVector.hh ncoGrammer.g nco_antlr_pst_prc.pl fmc_cls.cc fmc_cls.hh fmc_all_cls.cc fmc_all_cls.hh fmc_gsl_cls.cc fmc_gsl_cls.hh Invoke.cc Invoke.hh map_srt_tmp.hh nco_gsl.cc nco_gsl.hh prs_cls.cc prs_cls.hh sdo_utl.cc sdo_utl.hh sym_cls.cc sym_cls.hh VarOp.hh vtl_cls.hh) $(addprefix ${NCO_CXX_SRC_DIR}/,INSTALL libnco_c++.hh Makefile.am Makefile.old nco_att.cc nco_att.hh nco_dmn.cc nco_dmn.hh nco_fl.cc nco_fl.hh nco_hgh.cc nco_hgh.hh nco_utl.cc nco_utl.hh nco_var.cc nco_var.hh README TODO tst.cc) # `make tags' includes these files

TAGS_FILES := ${MY_SRC_DIR}/*.h $(filter-out ${TAGS_FILTER_FILES},${SRC_LST}) ${MDL_DOC_SRC} ${MDL_MAN} ${MDL_BLD_SRC} ${MDL_CXX_SRC} ${MDL_NCAP_SRC} ${MDL_SPT_SRC}

# Redefine default C and C++ pattern rules
${MY_OBJ_DIR}/%.o : %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o ${MY_OBJ_DIR}/$(notdir $@)
${MY_OBJ_DIR}/%.o : %.cu
	nvcc -deviceemu ${CPPFLAGS} ${CFLAGS} -c $< -o ${MY_OBJ_DIR}/$(notdir $@)
${MY_OBJ_DIR}/%.o : %.ccxb
	${CXX} ${CPPFLAGS} ${CXXFLAGS} -c $< -o ${MY_OBJ_DIR}/$(notdir $@)

# Default Fortran pattern rules: CRAY and RS6K must override these rules
${MY_OBJ_DIR}/%.o : %.F
	${FC} ${CPPFLAGS} -c ${FFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
${MY_OBJ_DIR}/%.o : %.f
	${FC} -c ${FFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<

# Rules for installing header files
#${MY_INC_DIR}/%.h : %.h
#	cp -f -p $(notdir $@) $@
${MY_INC_DIR}/%.hh : %.hh
	cp -f -p $(notdir $@) $@

# Rules for installing i18n files
# -k_ : Treat underscore as keyword (so "_" is equivalent to "gettext")
# --default-domain : Name of program or library
# --join-existing : Join messages with existing file
%.po : %.c
	xgettext --default-domain=$* -k_ --join-existing $<
${MY_ES_DIR}/%.mo : %.po
#	Linux version accepts more arguments than Solaris version
#	msgfmt --output-file=$@ --statistics $<
	msgfmt -o $@ $<

# Automatically generate a dependency file for each source file
# $* is the stem, e.g., f
# $@ is the filename of the target, e.g., f.d
# Linux gcc may return an extra `.F' on Fortran names, e.g., `hello.F.o: hello.F'
# (.F)? gets rid of this extra `.F'
${MY_DPN_DIR}/%.d : %.F
# Following command makes, e.g., f.d begin "f.o f.d : f.F ..."
# Since f.o is not preceded by ${MY_OBJ_DIR}, objects are not recompiled when sources are touched.
#	${MK_DPN} ${CPPFLAGS} $< | perl -p -e 's/$*\.F\.o/$*.o $@/g;' > $@
# Following command makes, e.g., f.d begin "/home/zender/obj/LINUX/f.o f.d : f.F ..."
# This works fairly well, but is a hack
# First pattern substitutes MY_OBJ_DIR_RX, which has placeholders for slashes
# Second pattern substitutes slashes for the placeholders
	${MK_DPN} ${CPPFLAGS} $< | perl -p -e 's/$*(\.F)?\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' > $@
# Following command makes, e.g., f.d begin "${MY_OBJ_DIR}/f.o f.d : f.F ..."
# This would be the ideal command but I can't get the dollar sign to show up
#	${MK_DPN} ${CPPFLAGS} $< | perl -p -e 's/$*\.F\.o/\${dollar}MY_OBJ_DIR\/$*.o $@/g;' > $@

${MY_DPN_DIR}/%.d : %.c
#	${MK_DPN} ${CPPFLAGS} $< | perl -p -e 's/$*\.o/$*.o $@/g;' > $@
	${MK_DPN} ${CPPFLAGS} $< | perl -p -e 's/$*\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' > $@

${MY_DPN_DIR}/%.d : %.cu
	nvcc -M ${CPPFLAGS} $< | perl -p -e 's/$*\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' > $@

${MY_DPN_DIR}/%.d : %.cc
# NB: Use ${CXX} rather than ${CPP} on C++ files for now because, e.g., SUNMP cpp does not pre-process .cc files quite correctly
# Extra hack to allow C++ filenames to contain '+' character
# $(subst ${plus},${plus_rx},${*}) is filename stub with an Rx in place of '+'
	${MK_DPN_CXX} ${CXXCPPFLAGS} $< | perl -p -e 's/\${plus}/${plus_rx}/g;s/$(subst ${plus},${plus_rx},${*})\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g;s/${plus_rx}/\${plus}/g' > $@

# First LDFLAGS is for typical C programs with netCDF, math, and networking
# Second LDFLAGS, when present, enables C/Fortran linking

# Manually define autotools tokens normally defined with HAVE_CONFIG_H in config.h 
# Initialize OS-specific tokens to empty
CPP_TKN_OS := -DHAVE_REGEX_H -DNCO_HAVE_REGEX_FUNCTIONALITY -DHAVE_GETPAGESIZE -DHAVE_GETRUSAGE -DNEED_STRCASESTR
# Assume netCDF >= 3.6.1
CPP_TKN_OS += -DHAVE_NC_INQ_FORMAT
# Assume netCDF >= 4.3.1
CPP_TKN_OS += -DHAVE_NC_RENAME_GRP
# Assume netCDF >= 4.3.2
CPP_TKN_OS += -DHAVE_NC_INQ_PATH
# Assume netCDF >= 4.6.1
CPP_TKN_OS += -DHAVE_NC_SET_LOG_LEVEL
# Assume netCDF >= 4.4.0, but Ubuntu 4.4.0 (e.g., on grele) fumbled netcdf_mem.h header
ifndef UBUNTU_440
 CPP_TKN_OS += -DHAVE_NETCDF_MEM_H
endif # !grele
# gcc 4.7.3 finally includes strcasestr() in string.h iff _GNU_SOURCE token is defined
# NB: C++ (or at least g++ 4.7.3) always includes strcasestr()---work around this with tokens in nco_sng_utl.[ch]
ifneq (${PVM_ARCH},CRAY)
 CPP_TKN_OS += -DHAVE_MKSTEMP
endif # CRAY
ifneq (${null},$(findstring ${PVM_ARCH},FREEBSDLINUXALPHALINUXAMD64LINUXARMMACOSXWIN32))
 CPP_TKN_OS += -DHAVE_GETOPT_H -DHAVE_GETOPT_LONG
endif # !LINUX
ifneq (${null},$(findstring ${PVM_ARCH},AIXSGIMP64))
 CPP_TKN_OS += -DNEED_GETOPT_LONG
endif # !(AIX || SGI)
ifeq (${CNK},Y)
 CPP_TKN_OS += -DHAVE_NEW_CHUNKING_API
endif # !CNK
ifeq (${DAP},Y)
 CPP_TKN_OS += -DENABLE_DAP
endif # !DAP
ifeq (${ESMF},Y)
 CPP_TKN_OS += -DENABLE_ESMF
endif # !ESMF
ifeq (${GSL},Y)
 CPP_TKN_OS += -DENABLE_GSL -DHAVE_GSL_H
endif # !GSL
ifeq (${MPI},Y)
ifneq (${MPI_FAKE},Y)
 CPP_TKN_OS += -DENABLE_MPI
endif # MPI_FAKE
endif # !MPI
ifeq (${UDUNITS},Y)
 CPP_TKN_OS += -DENABLE_UDUNITS -DHAVE_UDUNITS_H -DHAVE_UDUNITS2_H
endif # !UDUNITS
ifeq (${NETCDF4},Y)
 CPP_TKN_OS += -DENABLE_NETCDF4 -DHAVE_NETCDF4_H
endif # endif NETCDF4
ifeq (${PNETCDF},Y)
 CPP_TKN_OS += -DENABLE_PNETCDF
endif # !PNETCDF
# endif PNETCDF
# Assume strcasecmp() and strdup() routines are present (Comeau, Pathscale are exceptions)
CPP_TKN_OS += 
# fxm: Define HAVE_LIBINTL, HAVE_LOCALE_H, HAVE_GETTEXT, HAVE_OMP_H

# Works on AIX and AIX46K
ifneq (${null},$(findstring AIX,${PVM_ARCH}))
# 20030804: Always use re-entrant (_r) compilers---Jim Edwards NCAR/IBM 
 CC := xlc_r -qlanglvl=extc99
#CC := gcc -std=c99 -pedantic -D_DEFAULT_SOURCE
 CXX := xlC_r
# CXX := g++
 CPP := xlc -c -qlanglvl=extc99 -qsuppress=1501-245 -I/usr/lpp/ppe.poe/include
 FC := xlf95_r
ifneq (${null},$(findstring xl,${CC}))
# /usr/include headers must occur before Visual Age headers to prevent xlC pragma warnings
# Visual Age compiler headers must occur before g++ headers
 CPP_TKN_OS += -I/usr/include -I/usr/vacpp/include
endif # xlC compilers
# Add /usr/local/include for libintl.h explicitly until netCDF is moved there
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS} -DNEED_LOGF
LD := ld
# 20020422: -lC links to AIX C++ library which contains float intrinsics cosf()...
# -bh:5 suppresses annoying messages from xlC linker WARNING: Duplicate symbol: ...
LDFLAGS += -bh:5 ${OTHER_LDFLAGS} -lm -lC
LEX := flex
LINT := lint
YACC := bison
# AIX VA Compiler Collection
ifneq (${null},$(findstring xl,${CC}))
# Visual Age compiler defaults specified in /etc/vac.cfg
# Additional switch to fix compiler warnings on csz.c
# -qarch=auto : Automatically detect architecture of compiling machine and assume execution on same machine
# -qlonglong allow long long integers (and strtoll(), strtoull()) (default on in C not C++). Redundant with -qlanglvl=extc99
# -qmaxmem=num Limit memory used by space intensive optimizations to <num> kilobytes
# -qspill=size Size in B of register allocation spill area, mie needs > 1040 B
# -qsrcmsg prints transgressing source line with finger
# -qsuppress=1501-245 : Suppress RLIM_INFINITY memory message due to ulimits
# -qtune=auto : Optimize executable for architecture detected during compilation
 CFLAGS := -qmaxmem=8192 -qspill=2048 -qsrcmsg -qsuppress=1501-245
 FFLAGS := -NS2000 -qfixed=132 -qsrcmsg
# -bh:5 suppresses annoying messages from xlC linker WARNING: Duplicate symbol: ...
 LDFLAGS += -bh:5 -qsuppress=1501-245
#LDFLAGS += -lxlf90 # Required for linking Fortran objects
ifeq (${OMP},Y)
 FC := xlf95_r
# -qsmp=noauto : Turn on SMP/OMP code generation but do no automatic parallelization
# -qsmp=omp : Use industry standard OMP without IBM extensions
 OMP_FLG := -qsmp=omp
else
 CPP_DFN += -U_OPENMP
 FC := xlf95
endif # endif OMP
ifeq (${OPTS},O)
# -O : -O3 is safe, -O5 is dangerous
# -qstrict: Ensure that -O3 optimizations do not alter program semantics
# -Q : Inline all appropriate subprograms
 CFLAGS += -O3 -g -qstrict -Q
 FFLAGS += -O3 -g -qstrict -Q
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS += -g
endif
ifeq (${OPTS},X)
# -qflttrap generates instructions for floating point exceptions
# -qidirfirst uses headers found in -I directories first
# -qmakedep creates .d file
# -qwarn64 check for possible long-to-integer or pointer-to-integer truncation
# -qhalt=e stop compiler if error severity equals or exceeds i, w, e, s, u
 CFLAGS += -g -qflttrap -qidirfirst -qwarn64 -qcheck=all -qhalt=s
 FFLAGS += -g
endif
ifeq (${ABI},64)
 AR := ar -X 64
 CFLAGS += -q64
 FFLAGS += -q64
 LDFLAGS += -q64 
else
 CPPFLAGS += -D_LARGE_FILES
endif # endif ABI
# Additional flags for AIX:
# -M Generate information to be included in a "make" description file; output goes to .u file
# -c Do not send object files to the linkage editor
# -P Preprocess but do not compile; output goes to .i file
# Using -P causes additional warning messages about lm 
# Not using -P causes *.o files to be created twice
${MY_DPN_DIR}/%.d : %.c
	${MK_DPN} ${CPPFLAGS} $< ;perl -p -e 's/$*\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' $*.u > $@ ; \
	rm -f $*.i $*.o $*.u;
${MY_DPN_DIR}/%.d : %.cc
	${MK_DPN_CXX} ${CPPFLAGS} $< ;perl -p -e 's/$*\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' $*.u > $@ ; \
	rm -f $*.i $*.o $*.u;
endif # endif AIX VA Compiler Collection
# GNU Compiler Collection
ifneq (${null},$(findstring gcc,${CC}))
 CFLAGS := -Wall -Werror=format-security -Wunused
ifeq (${OPTS},O)
 CFLAGS += -O -g ${GCC_RCH_ARG}
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},R)
 CFLAGS += 
endif
ifeq (${OPTS},X)
 CFLAGS += -g -O
 LDFLAGS += /usr/local/lib/ccmalloc-g++.o -L/usr/local/lib -lccmalloc -ldl
endif
ifeq (${ABI},64)
 CC += -maix64 
 CXX += -maix64 
endif # endif ABI
 CXXFLAGS := ${CFLAGS}
endif # endif GNU Compiler Collection
# -q64: Select 64-bit compiler mode (required for accessing large files)
# -qwarn64: Warn on possible long-to-integer or pointer-to-integer truncation
CXXFLAGS := ${CFLAGS}
ifeq (${OMP},Y)
 CFLAGS += ${OMP_FLG}
 CXXFLAGS += ${OMP_FLG}
 FFLAGS += ${OMP_FLG}
 LDFLAGS := ${OMP_FLG} ${LDFLAGS}
endif # endif OMP
endif
# endif AIX

ifeq (${PVM_ARCH},ALPHA)
ifeq (${OMP},Y)
 OMP_FLG := -omp
endif # endif OMP
CXX := cxx
CC := cc
CFLAGS := ${OMP_FLG}
CPP := cpp
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := f90
FFLAGS := -r8 -i4 -c ${OMP_FLG} -automatic
FIXEDFLAGS := -extend_source ${OMP_FLG} -automatic
FREEFLAGS := -DHIDE_SHR_MSG -free
LD := ld
LDFLAGS += ${OMP_FLG} ${OTHER_LDFLAGS} -lm
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -O2 -g -ansi_alias
 FFLAGS += -O3 -g -inline speed
endif
ifeq (${OPTS},D)
 CFLAGS += -g -check_bounds -check -check_omp
 FFLAGS += -g3 -C
endif
ifeq (${OPTS},X)
 CFLAGS := -g -N 132
 FFLAGS := -g -check bounds -check omp_bindings -check overflow -check underflow
endif
CXXFLAGS := ${CFLAGS}
${MY_OBJ_DIR}/%.o : %.F90
	${FC} -c ${FREEFLAGS} ${FFLAGS} ${CPPFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
${MY_OBJ_DIR}/%.o : %.F
	${FC} -c ${FIXEDFLAGS} ${FFLAGS} ${CPPFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
endif
# endif ALPHA

ifeq (${PVM_ARCH},CRAY)
CXX := CC
CC := cc
CPP := cpp
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := f90
# -F enables macro substitution
# -dp enables DOUBLEPRECISION/double
FFLAGS := -N 132 -F -dp
LD := ld
LDFLAGS += ${OTHER_LDFLAGS} -lm
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -h rounddiv -h nofastmd -h nofastmodulus
 FFLAGS += -O2 -g
endif
ifeq (${OPTS},D)
 CFLAGS += -g -h indef -h rounddiv -h nofastmd -h nofastmodulus
 FFLAGS += -g -ei
endif
ifeq (${OPTS},X)
 CFLAGS += -g -h rounddiv -h indef -h bounds -h nofastmd -h nofastmodulus
 FFLAGS += -g -ei -Rabc
endif
# 19971021 Added -P to suppress #line # directives on Fortran files
${MY_OBJ_DIR}/%.o : %.F
	${CPP} -P ${CPPFLAGS} $< > $(patsubst %.F,%.f,$(notdir $<))
	${FC} -c ${FFLAGS} $(patsubst %.F,%.f,$(notdir $<)) 
	-mv -f $(notdir $@) ${MY_OBJ_DIR}
	rm -f $(patsubst %.F,%.f,$(notdir $<)) 
${MY_OBJ_DIR}/%.o : %.f
	${FC} -c ${FFLAGS} $<
	mv -f $(notdir $@) ${MY_OBJ_DIR}
endif
# endif CRAY

ifeq (${PVM_ARCH},HPPA)
CXX := g++
CC := gcc -std=c99 -pedantic -D_DEFAULT_SOURCE
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := f77
LD := ld
LDFLAGS += ${OTHER_LDFLAGS} -lnsl -lm
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -O2 -g
 FFLAGS := -fast -eendif
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS := -g -e
endif
ifeq (${OPTS},X)
 CFLAGS += -g 
 FFLAGS := -g -e
endif
endif
# endif HPPA

# Works on LINUX, LINUXALPHA, LINUXAMD64, LINUXARM, and FREEBSD and MACOSX
ifneq (${null},$(findstring ${PVM_ARCH},LINUXALPHALINUXAMD64LINUXARMFREEBSDMACOSX))
# 20161001 Linux ar with Ubuntu Xenial began using deterministic mode. -U undoes that.
# 20161001 GNU ar -s is equivalent to ranlib
# ar -D: Operate in deterministic mode (breaks NCO build on Ubuntu)
# ar -r: replace existing or insert new file(s) into the archive
# ar -s: equivalent to ranlib
# ar -U: Do not operate in deterministic mode.  This is the inverse of the D modifier, above: added files and the archive index will get their actual UID, GID, timestamp, and file mode values.
# ar -U: Unavailable option in RHEL 6 (2010) used on Rhea and Titan
# ar -v: be verbose
ARFLAGS := rsUv
ifneq (${null},$(findstring rhea,${HOSTNAME}))
 ARFLAGS := rsv
else ifneq (${null},$(findstring titan,${HOSTNAME}))
 ARFLAGS := rsv
else ifneq (${null},$(findstring ys,${HOSTNAME}))
 ARFLAGS := rsv
endif # !rhea,titan,yellowstone
ifeq (${PVM_ARCH},MACOSX)
 ARFLAGS := rsv
endif # !aerosol,firn
CXX := ${LINUX_CXX}
CC := ${LINUX_CC}
CPP := ${CC}
# NB: nameser.h needs -Di386, but gcc sends -Di586 (on pentiums)
CPP_TKN_OS += -Di386
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := ${LINUX_FC}
LD := ld
LDFLAGS += ${OTHER_LDFLAGS} -lm
ifeq (${PVM_ARCH},MACOSX)
 LDFLAGS += -lresolv
endif # endif MACOSX
LEX := flex
LINT := lint
YACC := bison
# Comeau C Compiler
ifeq (${CXX},como)
 CFLAGS := 
 CPPFLAGS += -DNEED_STRCASECMP
 CPPFLAGS += -DNEED_STRDUP
 LDFLAGS := ${COMOROOT}/libcomo/libcomo.a ${LDFLAGS}
ifeq (${OPTS},O)
 CFLAGS += -O -g
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},R)
 CFLAGS +=
endif
ifeq (${OPTS},X)
 CFLAGS += -g
endif
 CXXFLAGS := ${CFLAGS}
endif # endif Comeau C Compiler
# GNU Compiler Collection or LLVM
# 20140204: gcc and clang should receive identical options
ifeq (gcc,$(firstword ${CC}))
 GCC_OR_CLANG := Y
endif
ifeq (clang,$(firstword ${CC}))
 GCC_OR_CLANG := Y
endif
ifeq (${GCC_OR_CLANG},Y)
 CPPFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
 CFLAGS := -Wall -Wuninitialized
# Compilation flags for numerical routines recommended by GSL 1.3 manual, p. 397
# Greenplanet has old compiler that lacks -Werror=format-security
# CFLAGS += -W -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fno-common -g
 CFLAGS += -Werror=format-security -W -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fno-common -g
# Compilation flags recommended by GSL and others that I like and use:
# -D_BSD_SOURCE: Support 4.3 BSD Unix extensions to ANSI C (prevents nameser.h warnings)
# -D_POSIX_SOURCE: Support POSIX.1 standard additions to ANSI C (prevents fileno warnings)
# -pedantic: Disallow non-ISO constructs (including type long long) (sometimes useful)
# -W: Extra warnings, including missing return values, comparison of signed with unsigned
# -Wall: Warn about common programming problems
# -Wcast-align: Warn if casting pointer to type of different size
# -Wcast-qual: Warn if const qualifier removed from pointer
# -Werror: Consider warnings as errors
# -Werror=format-security: Consider this specific warning as errors (requires gcc > 4.1.2)
# -Wmaybe-uninitialized: Warn on uninitialized variables. GCC has many false negatives, clang is better? Available on GCC 4.8.2+. Not available on GCC 4.6.3-.
# -Wmissing-prototypes: Warn if missing prototypes
# -Wpointer-arith: Warn if pointer arithmetic on types without size, e.g., void
# -Wshadow: Warn if local variable has same name as other local variable
# -Wsometimes-uninitialized: LLVM/clang uses this but GCC does not
# -Wswitch: Warn if switch statement has enumerated index and case label outside enumeration range
# -Wuninitialized: Warn on uninitialized variables. GCC has many false negatives, clang is better? 
# -Wunused: Warn on unused functions, labels, parameters, values, and variables
# -Wwrite-strings: Apply const-qualifier to string constants, die if overwritten
# -fno-common: Prevent global variables from being simultaneously defined in different files
# -g: Put debugging symbols in executable
# -pg: Enable profiling, generate gmon.out output files (also needed by linker)
# -O3: clang -O4 DNE, highest clang optimization is -O3 
# -O4: Turn on optimization so uninitialized variables are flagged. Downside: optimizes-out many debugging symbols
# Compilation flags recommended by GSL that I do not like and do not use:
# -ansi: Support only strict ANSI C. Equivalent to -std=c89, conflicts with -std=c99
# --no-alias? -fstrict-aliasing
# -Waggregate-return: Warn if functions return aggregates like structures or unions
# -Wconversion: Warn if converting signed to unsigned. Intended for obsolete, non-prototyped code. Triggers fabsf(), sqrtf(), warnings.
# -Wnested-externs: Warn if extern is encountered within function. C only?
# -Wstrict-prototypes: Warn if inconsistent prototypes. C only?
# -Wtraditional: Warn if constructs differ between traditional and ANSI C. C only?
# -Dinline=: inline is not an ANSI keyword, must undefine inline to work with -ansi
# -fshort-enums: Make enums as short as possible, ususally non-int. Do not ever invoke this! This breaks ABI and causes subtle problems
ifeq (${OMP},Y)
ifneq (clang,$(firstword ${CC}))
# 20140526 clang does not recognize/utilize -fopenmp
 OMP_FLG_C := -fopenmp
 OMP_FLG_F := -fopenmp
 LDFLAGS += -lgomp -lpthread
endif # endif clang
endif # endif OMP
ifeq (${OPTS},O)
ifeq (clang,$(firstword ${CC}))
 CFLAGS += -O3 -g ${GCC_RCH_ARG}
else # endif clang
 CFLAGS += -O4 -g ${GCC_RCH_ARG}
endif # endif clang
endif
ifeq (${OPTS},D)
 CFLAGS += -g -Wno-switch
endif
ifeq (${OPTS},R)
 CFLAGS += 
endif
ifeq (${OPTS},X)
# 20090715: https://wiki.ubuntu.com/CompilerFlags
# -D_FORTIFY_SOURCE=2 : Compile-time libc checks, run-time buffer/memory checks
#                     : NB: _FORTIFY_SOURCE macro requires -O switch
# -fstack-protector : Enable run-time stack overflow verification
# -Wformat-security : Warn about misuse of format strings
# -Wl,-z,relro : Read-only relocation table area in final ELF
 CPPFLAGS += -D_FORTIFY_SOURCE=2
 CFLAGS += -g -O -fstack-protector -Wformat-security
ifneq (clang,$(firstword ${CC}))
# 20150106 clang does not recognize/utilize -z
 CFLAGS += -Wl,-z,relro
endif # endif clang
# 20090715: -Werror triggers known nc_put_var?_string() and nco_def_var_chunking() errors
# CFLAGS += -g -pg -fno-inline -Werror
# CFLAGS += -g -O -pg -fno-inline
 CFLAGS += -g -pg -fno-inline
 LDFLAGS += -pg
# LDFLAGS += /usr/local/lib/ccmalloc-gcc.o -L/usr/local/lib -lccmalloc -ldl
endif
ifneq (${null},$(findstring AMD64,${PVM_ARCH}))
ifeq (${ABI},64)
 CFLAGS += -m64
 FFLAGS += -m64
 LDFLAGS += -m64
endif # endif ABI
endif # endif LINUXAMD64
 CXXFLAGS := ${CFLAGS}
endif # endif GNU Compiler Collection or LLVM
# Intel (Kai) C Compiler
ifeq (icc,$(firstword ${CC}))
# -fast: enable -xP -O3 -ipo -static
# -ipo[n]: enable multi-file IP optimizations (between files)
# -no-gcc: do not define  __GNUC__, __GNUC_MINOR__, and __GNUC_PATCHLEVEL__ macros
# -static: prevents linking with shared libraries
# -std=c99: Enable C99 support for C programs
# -xB: specialize code to run exclusively on Intel Pentium M and compatible Intel processors
# -xK: specialize code to run exclusively on Intel Pentium III and compatible Intel processors
# -xN: specialize code to run exclusively on Intel Pentium 4 and compatible Intel processors
# -xP: specialize code to run exclusively on Intel Pentium 4 processors with SSE3 extensions
# -Wall: enable all warnings
# -Werror:force warnings to be reported as errors
# -w0: display errors (same as -w)
# -w1: display warnings and errors (DEFAULT)
# -w2: display remarks, warnings, and errors
# -wd<L1>[,<L2>,...] disable diagnostics L1 through LN
# warning #274: declaration is not visible outside of function
# remark #981: operands are evaluated in unspecified order
# remark #810: conversion from "double" to "float" may lose significant bits
# remark #1572: floating-point equality and inequality comparisons are unreliable
 CFLAGS := -w1 -wd274
# Rhea icc 13.1.3 (gcc version 4.4.7 compatibility) requires -D_BSD_SOURCE on files that #include nameser.h, recent transition of gcc 4.9 to -D_DEFAULT_SOURCE does not cut the mustard
 CPPFLAGS += -no-gcc -D_BSD_SOURCE
 LDFLAGS += -lsvml
 OMP_FLG_C := -openmp -openmp_report0
ifeq (${OPTS},O)
 CFLAGS += -O3 -g ${ICC_RCH_ARG}
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},R)
 CFLAGS += 
endif
ifeq (${OPTS},X)
 CFLAGS += -g -Wall -wd810,981,1572 -inline_debug_info
endif
 CXXFLAGS := ${CFLAGS}
endif # endif Intel (Kai) C Compiler
# Intel (Kai) Fortran Compiler
ifeq (${FC},ifc)
# -e95 issues warnings for non-standard fortran
# -fpp2 necessary, but not sufficient, for  OpenMP
 FFLAGS := -extend_source -implicitnone -vms -e95 -fpp2
# -lVaxlib needed for iargc_, getarg_
 LDFLAGS += -lVaxlib
 OMP_FLG_C := -openmp
ifeq (${PRC},D)
 FFLAGS += -i4 -r8 -doubletemps
else
 FFLAGS += -i4
endif
ifeq (${OPTS},O)
 FFLAGS += -O2 -g
endif
ifeq (${OPTS},D)
 FFLAGS += -g
endif
ifeq (${OPTS},R)
 FFLAGS += 
endif
ifeq (${OPTS},X)
 FFLAGS += -g -C -e95
endif
endif # endif Intel (Kai) Fortran Compiler
# cd ~;nvcc -o libcuda_add.a -deviceemu -lib simpleTemplates.cu
# cd ~;g++ -I/usr/local/cuda/include -c -o test.o test.c
# cd ~;g++ -o ~/test test.o libcuda_add.a
# NVidia C Compiler
ifeq (nvcc,$(firstword ${CC}))
 CPPFLAGS += 
# -deviceemu: Emulate CUDA CPU
# -lib: build static libraries
 CFLAGS := -deviceemu -lib
ifeq (${OMP},Y)
 OMP_FLG_C := 
 OMP_FLG_F := 
 LDFLAGS += 
endif # endif OMP
ifeq (${OPTS},O)
 CFLAGS += -O -g
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},R)
 CFLAGS += 
endif
ifeq (${OPTS},X)
 CFLAGS += -g
 LDFLAGS += 
endif
ifneq (${null},$(findstring AMD64,${PVM_ARCH}))
ifeq (${ABI},64)
 CFLAGS += 
 FFLAGS += 
 LDFLAGS += 
endif # endif ABI
endif # endif LINUXAMD64
 CXXFLAGS := ${CFLAGS}
endif # endif NVidia C Compiler
# Pathscale (QLogic) C Compiler
ifeq (pathcc,$(firstword ${CC}))
# pathcc -show-defaults
# shows that pathcc automatically sets many hardware-specific options
# man -k pathscale for full listing
# -O2 = -O: Default optimization
# -Ofast = -O3:
 CFLAGS := 
 CPPFLAGS += -DNEED_STRCASECMP
 LDFLAGS += 
 OMP_FLG_C := -apo -mp
ifeq (${OPTS},O)
 CFLAGS += -O3 -g ${PSC_RCH_ARG}
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},R)
 CFLAGS += -O2
endif
ifeq (${OPTS},X)
 CFLAGS += -g 
endif
 CXXFLAGS := ${CFLAGS}
endif # end Pathscale (QLogic) C++ Compiler
# Portland Group C++ Compiler
ifeq (pgcc,$(firstword ${CC}))
# Enable Large File Support (LFS) by default
 CFLAGS := -Mlfs
# Pass kludgy PGI identifier to flag for broken C99 designated initializers etc.
 CPPFLAGS := $(filter-out -DHAVE_C99,${CPPFLAGS})
 CPPFLAGS := -DPGI_CC ${CPPFLAGS}
 LDFLAGS += -Mlfs
 OMP_FLG_C := -mp
ifeq (${OPTS},O)
 CFLAGS += -fast ${PGI_RCH_ARG}
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},R)
 CFLAGS += 
endif
ifeq (${OPTS},X)
 CFLAGS += -g -Mbounds
endif
 CXXFLAGS := ${CFLAGS}
endif # endif Portland Group C++ Compiler
# Portland Group Fortran Compiler
ifeq (${FC},pgf90)
 FFLAGS := -Mextend -Mnosecond_underscore -byteswapio -Mrecursive -Mdalign -Ktrap=fp -Mlfs
 OMP_FLG_F := -mp
ifeq (${PRC},D)
 FFLAGS += -Mr8 -Mi4
endif
ifeq (${OPTS},O)
 FFLAGS += -fast
endif
ifeq (${OPTS},D)
 FFLAGS += -g
endif
ifeq (${OPTS},R)
 FFLAGS += 
endif
ifeq (${OPTS},X)
 FFLAGS += -g -Mbounds
endif
endif # endif Portland Group Fortran Compiler
# G77 Fortran compiler
ifeq (${FC},g77)
 FFLAGS := -ffixed-line-length-132 -fno-second-underscore
ifeq (${OPTS},O)
 FFLAGS += -O -g
endif
ifeq (${OPTS},D)
 FFLAGS += -g -fdebug-kludge
endif
ifeq (${OPTS},R)
 FFLAGS += -fdebug-kludge
endif
ifeq (${OPTS},X)
 FFLAGS := -g -O -fdebug-kludge -fbounds-check
endif
endif # endif G77 Fortran compiler
ifeq (${DPKG},Y)
 CPPFLAGS+=$(shell dpkg-buildflags --get CPPFLAGS)
 CFLAGS+=$(shell dpkg-buildflags --get CFLAGS)
 CXXFLAGS+=$(shell dpkg-buildflags --get CXXFLAGS)
 LDFLAGS+=$(shell dpkg-buildflags --get LDFLAGS)
endif # endif OMP
ifeq (${OMP},Y)
 CFLAGS += ${OMP_FLG_C}
 CXXFLAGS += ${OMP_FLG_C}
 FFLAGS += ${OMP_FLG_C}
# LD behavior assumes C source code
 LDFLAGS := ${OMP_FLG_C} ${LDFLAGS}
endif # endif OMP
endif
# endif LINUX, LINUXALPHA, LINUXAMD64, LINUXARM, FREEBSD, MACOSX

# NB: Deprecated as of 201403. Now LINUX* subsumes MACOSX
ifeq (${PVM_ARCH},MACOSXOLD)
CXX := ${LINUX_CXX}
# NB: -D_POSIX_SOURCE breaks MACOSX build in nco_fl_utl.c, nco_mmr.c
#CC := cc -std=c99 -pedantic -D_BSD_SOURCE
CC := ${LINUX_CC} -std=c99 -pedantic -D_BSD_SOURCE
CPP := ${CC}
# -fno-common: Allows shared libraries to build
CFLAGS := -Wall -Werror=format-security -fno-common
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := f90
LD := ld
LDFLAGS += ${OTHER_LDFLAGS} -lresolv -lm
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -O
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},R)
 CFLAGS += 
endif
ifeq (${OPTS},X)
 CFLAGS += -g -O
 LDFLAGS += /usr/local/lib/ccmalloc-gcc.o -L/usr/local/lib -lccmalloc -ldl
endif
CXXFLAGS := ${CFLAGS}
ifeq (${OMP},Y)
 CFLAGS += ${OMP_FLG}
 FFLAGS += ${OMP_FLG}
 LDFLAGS := ${OMP_FLG} ${LDFLAGS}
endif # endif OMP
endif
# endif MACOSXOLD

ifeq (${PVM_ARCH},NECSX)
ifeq (${OMP},Y)
 OMP_FLG := -Popenmp
endif # endif OMP
CXX := c++
#CC := c++ -Xa
CC := cc
CPP := c++ -E
#CPP := /usr/lib/cpp
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := f90
LD := ld
LDFLAGS += ${OTHER_LDFLAGS} -lm
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -h2 -hmath vector -hxint
# CFLAGS += -Cvopt -math vector -xint
 FFLAGS = -Cvopt -f3
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS = -g -f3
endif
ifeq (${OPTS},X)
 CFLAGS += -g -h0 -hstack=nan
# CFLAGS += -Cdebug -init stack=nan
 FFLAGS = -Cdebug -eR -f3 -Wf"-init stack=nan heap=nan"
endif
MK_DPN = /usr/local/bin/mkdep.perl /usr/lib/cpp # NECSX try this
${MY_DPN_DIR}/%.d : %.c
	${MK_DPN} ${CPPFLAGS} $< | perl -p -e 's/$*\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' > $@
endif
# endif NECSX

ifeq (${PVM_ARCH},RS6K)
CXX := g++
CC := gcc -std=c99 -pedantic -D_DEFAULT_SOURCE
CPP := /lib/cpp -P
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := xlf
LD := ld
LDFLAGS += ${OTHER_LDFLAGS} -lm
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -O2
 CPP := ${CPP} ${CPPFLAGS}
 PREPROCESS.F := ${CPP} ${CPPFLAGS}
 FFLAGS := -O -g -NS2000 -qfixed=132
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 CPP := ${CPP} ${CPPFLAGS}
 PREPROCESS.F := ${CPP} ${CPPFLAGS}
 FFLAGS := -g -NS2000 -qfixed=132
endif
${MY_OBJ_DIR}/%.o : %.F
	${CPP} ${CPPFLAGS} $< ${MY_OBJ_DIR}/$(basename $<).f 
	${FC} -c ${FFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) ${MY_OBJ_DIR}/$(basename $<).f
${MY_OBJ_DIR}/%.o : %.f
	${FC} -c ${FFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
endif
# endif RS6K

# SGI6, SGI64, SGIMP64
ifneq (${null},$(findstring SGI,${PVM_ARCH}))
ifeq (${OMP},Y)
 OMP_FLG := -mp -mpio
endif # endif OMP
CXX := CC -LANG:std
CC := cc -c99
# 20000302: -w suppresses warnings which will swamp linker
#CXX := g++ -w
#CC := gcc -std=c99 -pedantic -D_DEFAULT_SOURCE
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
ifdef $(MIPSPRO_SGI)
# SGIs like dataproc keep omp.h in special location determined by module MIPSpro
 CPPFLAGS := -I$(MIPSPRO_SGI)/usr/include ${CPPFLAGS}
endif
FC := f90 -cpp
LD := ld
LEX := flex
LINT := lint
YACC := bison
ifeq (${PVM_ARCH},SGI6)
 GCC_ABI_FLG := -mabi=32
 GCC_LDFLAGS_SZ_SPC := ${GCC_ABI_FLG} -mips3 
 SGI_ABI_FLG := -n32 -mips3 ${OMP_FLG}
else # SGI64, SGIMP64
ifeq (${ABI},64)
 GCC_ABI_FLG := -mabi=64
 GCC_LDFLAGS_SZ_SPC := ${GCC_ABI_FLG} -mips4 -L/usr/local/lib/mabi=64
 SGI_ABI_FLG := -64 -mips4 ${OMP_FLG}
else # ABI=32
 GCC_ABI_FLG := -mabi=32
 GCC_LDFLAGS_SZ_SPC := ${GCC_ABI_FLG} -mips4
 SGI_ABI_FLG := -n32 -mips4 ${OMP_FLG}
endif # endif ABI
endif # endif SGI64, SGIMP64
ifeq (gcc,$(firstword ${CC}))
 LDFLAGS += ${GCC_LDFLAGS_SZ_SPC} ${OTHER_LDFLAGS} -lm
 CFLAGS := ${GCC_ABI_FLG} -Wall -Werror=format-security
ifeq (${OPTS},O)
 CFLAGS += -O2
endif
ifeq (${OPTS},R)
 CFLAGS += 
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},X)
 CFLAGS += -g -O
endif
 CXXFLAGS := ${CFLAGS}
endif
# endif CC=gcc
ifeq (cc,$(firstword ${CC}))
 LDFLAGS += ${SGI_ABI_FLG} ${OTHER_LDFLAGS} -lm
 CFLAGS := ${SGI_ABI_FLG}
ifeq (${OPTS},O)
 CFLAGS += -O2
endif
ifeq (${OPTS},R)
 CFLAGS += 
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},X)
 CFLAGS += -g -trapuv
endif
endif
# endif CC=cc
# Fortran flags
FFLAGS := ${SGI_ABI_FLG} -extend_source
ifeq (${OPTS},O)
 FFLAGS := -O2 -g
endif
ifeq (${OPTS},R)
 FFLAGS := 
endif
ifeq (${OPTS},D)
 FFLAGS := -g
endif
ifeq (${OPTS},X)
 FFLAGS := -g -check_bounds -trapuv
endif
# end fortran flags
endif
# endif SGI6, SGI64, SGIMP64

ifeq (${UNAMES},SunOS) 
CXX := CC
CC:= c99 -D_BSD_SOURCE
CFLAGS := 
CPP := cc -E
#CXX := g++
#CC := gcc -std=c99 -pedantic -D_DEFAULT_SOURCE
#CFLAGS := -Wall
#CPP := cpp
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := f90 -DHIDE_SHR_MSG
#FFLAGS := -xs -stackvar -e -Qoption f90comp -r8const
FFLAGS := -xs -stackvar -e 
LD := ld
LDFLAGS += ${OTHER_LDFLAGS} -lsunmath -lresolv -lsocket -lnsl -lm
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -O2 -g
 FFLAGS += -fast
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS += -g
endif
ifeq (${OPTS},X)
 CFLAGS += -g 
 FFLAGS += -g
# NB: 19980601 -C (range-checking) is not supported by Sun f90
ifeq (${FC},f77)
 FFLAGS += -C
endif
endif
 CXXFLAGS := ${CFLAGS}
endif
# endif SunOS=SUN4SOL2,SUNMP

ifeq (${PVM_ARCH},WIN32)
CXX := g++
CC := gcc -std=c99 -pedantic -D_DEFAULT_SOURCE
# NB: nameser.h needs -Di386, but gcc sends -Di586 (on pentiums)
#CPP_TKN_OS += -Di386 -DNEED_STRCASECMP -DNEED_STRDUP -I/usr/include
CPP_TKN_OS += -Di386 -DNEED_STRDUP -I/usr/include
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} ${OTHER_CFLAGS}
FC := g77
LD := ld
LDFLAGS += ${OTHER_LDFLAGS} -lm
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -O -g
endif
ifeq (${OPTS},D)
 CFLAGS += -g
endif
${MY_OBJ_DIR}/%.o : %.F
	${FC} -c ${FFLAGS} ${CPPFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
${MY_OBJ_DIR}/%.o : %.f
	${FC} -c ${FFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
endif
# endif WIN32

# Resolve nvcc CUDA run-time library functions
ifeq (${CUDA},Y)
 CUDA_FLG_C := 
 CUDA_FLG_F := 
 LDFLAGS += -L/usr/local/cuda/lib -lcudart
endif # endif CUDA

# Internationalize NCO with i18n features
ifeq (${I18N},Y)
 CPPFLAGS += -DI18N
ifneq (${null},$(findstring MACOSX,${PVM_ARCH}))
 LDFLAGS += -lintl
endif
endif
# endif I18N

# Default to MPICC, MPICXX to CC, CXX and cross your fingers
MPICC := ${CC}
MPICXX := ${CXX}
# Manipulate CC, CPP, CXX as appropriate for MPI-enabled operators
ifeq (${MPI},Y)
ifneq (${null},$(findstring xl,${CC}))
# NB: AIX is not debugged yet
 MPICC := $(subst xlc,mpcc,${CC})
 MPICXX := $(subst xlC,mpCC,${CXX})
endif # endif AIX VA Compiler Collection
ifneq (${null},$(findstring gcc,${CC}))
 MPICC := $(subst gcc,mpicc,${CC})
 MPICXX := $(subst g++,mpicxx,${CXX})
endif # endif GNU Compiler Collection
ifeq (icpc,$(firstword ${CXX}))
 MPICC := $(subst icc,mpicc,${CC})
 MPICXX := $(subst icpc,mpicxx,${CXX})
endif # endif Intel (Kai) C++ Compiler
ifneq (${null},$(findstring pathcc,${CC}))
 MPICC := $(subst pathcc,mpicc,${CC})
 MPICXX := $(subst pathCC,mpicxx,${CXX})
endif # endif Pathscale (QLogic) Compilers
 CPP := ${MPICC}
endif # endif MPI
# Use MPI modifications, if any, to build all objects
CC := ${MPICC}
CXX := ${MPICXX}

# Disable OpenMP on platforms which automatically support it
ifneq (${OMP},Y)
ifneq (${null},$(findstring SGI,${PVM_ARCH}))
 CFLAGS := $(filter-out -mp -mpio,${CFLAGS})
 LDFLAGS := $(filter-out -mp -mpio,${LDFLAGS})
endif # endif SGI
 CPPFLAGS += -U_OPENMP
endif # endif OMP

ifneq (${null},$(findstring LINUX,${PVM_ARCH}))
ifeq (${CCACHE},Y)
# Prefix CC and CXX with ccache
 CC := ccache ${CC}
 CXX := ccache ${CXX}
endif # !CCACHE
endif # !LINUX

ifeq (${STC},Y)
# Created statically linked executable
 LDFLAGS := -static ${LDFLAGS}
endif # endif STC

# Define CPPCXXFLAGS after making all possible modifications to CPPFLAGS
# Add nco_c++ to directory search path
CXXCPPFLAGS := ${CPPFLAGS} -I${NCO_CXX_SRC_DIR}

# Define any remaining variables
libnco := ${MY_LIB_DIR}/libnco

# Default targets
all: dir lib ${MDL_BIN_TRG} ${MDL_BIN_SPT} data
non_ncap: dir lib $(filter-out ncap,${MDL_BIN_TRG}) ${MDL_BIN_SPT} data
# .PHONY tells make to remake the following non-file targets 
.PHONY: all cln dst_cln dbg ${MDL_BIN_TRG} ${MDL_BIN_SPT}
# Delete default suffixes---this should increase speed
.SUFFIXES: 
# Define suffixes that matter
.SUFFIXES: .cc .c .cu .o .F .d
# Delete targets that were not successfully made
.DELETE_ON_ERROR:
# Target directories which may not exist
dir: bin_dir obj_dir lib_dir
bin_dir:
# Compaq ALPHA complains about -install
#	-install -d ${MY_BIN_DIR}
	-mkdir -p ${MY_BIN_DIR}
lib_dir:
#	-install -d ${MY_BIN_DIR}
	-mkdir -p ${MY_LIB_DIR}
obj_dir:
#	-install -d ${MY_BIN_DIR}
	-mkdir -p ${MY_OBJ_DIR}
# The whole shebang
allinone: all nco_c++ nco++
nco_c++: 
	-cd ../src/nco_c++; ${MAKE} -f Makefile.old DBG=${DBG} NETCDF4=${NETCDF4} OMP=${OMP} OPTS=${OPTS} SZ=${SZ} lib dat all
nco++: 
	-cd ../src/nco++; ${MAKE} -f Makefile.old CNK=${CNK} DAP=${DAP} DBG=${DBG} GSL=${GSL} MPI=${MPI} NETCDF4=${NETCDF4} OMP=${OMP} OPTS=${OPTS} PNETCDF=${PNETCDF} SZ=${SZ} UDUNITS=${UDUNITS} lib all
# Targets in bin
mpi:   mpi_nco
mpinco:   mpi_nco
mpnco:   mpi_nco
mpi_nco:   ${MDL_MPI_TRG}
	-rm -f ${MY_BIN_DIR}/mpirun && ln -f -s `which mpirun` ${MY_BIN_DIR}/mpirun
mpncbo:   ${MY_BIN_DIR}/mpncbo
${MY_BIN_DIR}/mpncbo:	${MY_OBJ_DIR}/mpncbo.o lib
	${MPICC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f mpncdiff; ln -s -f mpncbo mpncdiff
# 20050710: icc 8.1 with -axW flag dies compiling ncecat
# 20050915: Same problem occurs with MPI_FAKE version of mpncecat
ifeq (icc,$(firstword ${LINUX_CC}))
ncecat : CFLAGS := $(filter-out -axW,${CFLAGS})
ifeq (${MPI_FAKE},Y)
mpncecat : CFLAGS := $(filter-out -axW,${CFLAGS})
endif # !MPI_FAKE
endif # endif Intel (Kai) C Compiler
mpncecat:   ${MY_BIN_DIR}/mpncecat
${MY_BIN_DIR}/mpncecat:	${MY_OBJ_DIR}/mpncecat.o lib
	${MPICC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
mpncflint:   ${MY_BIN_DIR}/mpncflint
${MY_BIN_DIR}/mpncflint:	${MY_OBJ_DIR}/mpncflint.o lib
	${MPICC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
mpncpdq:   ${MY_BIN_DIR}/mpncpdq
${MY_BIN_DIR}/mpncpdq:	${MY_OBJ_DIR}/mpncpdq.o lib
	${MPICC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
#	cd ${MY_BIN_DIR}; rm -f mpncunpack; ln -s -f mpncpdq mpncunpack
#	cd ${MY_BIN_DIR}; rm -f mpncpack; ln -s -f mpncpdq mpncpack
mpncra:   ${MY_BIN_DIR}/mpncra
${MY_BIN_DIR}/mpncra:	${MY_OBJ_DIR}/mpncra.o lib
	${MPICC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f mpncea; ln -s -f mpncra mpncea
	cd ${MY_BIN_DIR}; rm -f mpnces; ln -s -f mpncra mpnces
	cd ${MY_BIN_DIR}; rm -f mpncrcat; ln -s -f mpncra mpncrcat
mpncwa:   ${MY_BIN_DIR}/mpncwa
${MY_BIN_DIR}/mpncwa:	${MY_OBJ_DIR}/mpncwa.o ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o lib
	${MPICC} -o $@${BNR_SFX} $< ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncatted:   ${MY_BIN_DIR}/ncatted
${MY_BIN_DIR}/ncatted:	${MY_OBJ_DIR}/ncatted.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncbo:   ${MY_BIN_DIR}/ncbo
${MY_BIN_DIR}/ncbo:	${MY_OBJ_DIR}/ncbo.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f ncdiff; ln -s -f ncbo ncdiff
ncclimo:   ${MY_BIN_DIR}/ncclimo
${MY_BIN_DIR}/ncclimo:	${MY_DAT_DIR}/ncclimo
	/bin/cp -f ${MY_DAT_DIR}/ncclimo ${MY_BIN_DIR}
ncecat:   ${MY_BIN_DIR}/ncecat
${MY_BIN_DIR}/ncecat:	${MY_OBJ_DIR}/ncecat.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncflint:   ${MY_BIN_DIR}/ncflint
${MY_BIN_DIR}/ncflint:	${MY_OBJ_DIR}/ncflint.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncks:   ${MY_BIN_DIR}/ncks
${MY_BIN_DIR}/ncks:	${MY_OBJ_DIR}/ncks.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncpdq:   ${MY_BIN_DIR}/ncpdq
${MY_BIN_DIR}/ncpdq:	${MY_OBJ_DIR}/ncpdq.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f ncunpack; ln -s -f ncpdq ncunpack
	cd ${MY_BIN_DIR}; rm -f ncpack; ln -s -f ncpdq ncpack
ncra:   ${MY_BIN_DIR}/ncra
${MY_BIN_DIR}/ncra:	${MY_OBJ_DIR}/ncra.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f ncea; ln -s -f ncra ncea
	cd ${MY_BIN_DIR}; rm -f nces; ln -s -f ncra nces
	cd ${MY_BIN_DIR}; rm -f ncrcat; ln -s -f ncra ncrcat
ncremap:   ${MY_BIN_DIR}/ncremap
${MY_BIN_DIR}/ncremap:	${MY_DAT_DIR}/ncremap
	/bin/cp -f ${MY_DAT_DIR}/ncremap ${MY_BIN_DIR}
ncrename:   ${MY_BIN_DIR}/ncrename
${MY_BIN_DIR}/ncrename:	${MY_OBJ_DIR}/ncrename.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncwa:   ${MY_BIN_DIR}/ncwa
${MY_BIN_DIR}/ncwa:	${MY_OBJ_DIR}/ncwa.o ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o lib
ifeq (${MSVC},TRUE)
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
else
	${CC} -o $@${BNR_SFX} $< ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o ${LDFLAGS}
endif
	chmod 755 $@${BNR_SFX}
bin: ${MDL_BIN_TRG} ${MDL_BIN_SPT}
binclean: bin_cln
bin_cln: 
	rm -f ${MDL_BIN}
mpiclean: mpi_cln
mpi_cln: 
	rm -f ${MDL_MPI_BIN} ${MDL_MPI_OBJ}
mpi_fake_cp: ${MDL_MPI_TRG}
	- for fl in ${MDL_MPI_TRG_SMP}; do /bin/cp -f ${MY_BIN_DIR}/$$fl ${MY_BIN_DIR}/mp$$fl; done
#	20050916 fxm: not sure why simpler substitution method does not work
#	- for fl in ${MDL_MPI_TRG}; do /bin/cp -f ${MY_BIN_DIR}/$$fl ${MY_BIN_DIR}/$(subst mp,,$$fl); done
strip: 
	- printf "Before stripping...\n";ls -l ${MDL_BIN};strip ${MDL_BIN};printf "After stripping...\n";ls -l ${MDL_BIN}
# Targets in bld
buildclean: bld_cln
bld_cln:
	cd ${MY_BLD_DIR}; rm -f TAGS
libtest:	libtst
libtst:	libnco_tst libnco_c++_tst
libnco_tst:   ${MY_BLD_DIR}/libnco_tst
${MY_BLD_DIR}/libnco_tst:	${MY_BLD_DIR}/libnco_tst.o lib
	${CC} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
libnco_c++_tst:   ${MY_BLD_DIR}/libnco_c++_tst
${MY_BLD_DIR}/libnco_c++_tst:	${MY_BLD_DIR}/libnco_c++_tst.o lib
	${CXX} -o $@${BNR_SFX} $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
rpm:	${MY_BLD_DIR}/nco.spec # Building rpm requires root priveleges, e.g., sudo make NCO_VRS=4.5.2 rpm
# 	Building NCO RPM requires following packages
#       rpm -q bison flex netcdf-devel libtool automake autoconf udunits udunits-devel curl-devel libxml2 libxml2-devel librx-devel 
#       sudo yum install bison flex netcdf-devel libtool automake autoconf udunits udunits-devel curl-devel libxml2 libxml2-devel librx-devel
	cd ${DATA}/tmp; \
	/bin/rm -fr ${DATA}/tmp/nco*; \
	cp -r ${HOME}/nco nco-${NCO_VRS}; \
	tar cvzf nco-${NCO_VRS}.tar.gz ./nco-${NCO_VRS}; \
	mv ${DATA}/tmp/nco-${NCO_VRS}.tar.gz /usr/src/redhat/SOURCES; \
	/bin/rm -f /usr/src/redhat/SPECS/nco-${NCO_VRS}.spec; \
	ln -s ${HOME}/nco/bld/nco.spec /usr/src/redhat/SPECS/nco-${NCO_VRS}.spec; \
	cd /usr/src/redhat/SPECS; \
	rpmbuild -ba --sign nco-${NCO_VRS}.spec;
# Crypographically sign RPM packages with GPG: http://fedoranews.org/tchung/gpg
# gpg --export -a 'Charlie Zender' > ~/GPG-zender # Export public key from key ring to text file
# sudo rpm --import ~/GPG-zender # Import public key to RPM database
# rpm -q gpg-pubkey --qf '%{name}-%{version}-%{release} --> %{summary}\n' # Verify list of GPG public keys in RPM database
# Verify ~/.rpmmacros file specifies _signature and gpg_name
# sudo rpm --addsign /usr/src/redhat/SRPMS/nco-${NCO_VRS}-?.src.rpm /usr/src/redhat/RPMS/i386/nco-${NCO_VRS}-?.i386.rpm # Sign RPM packages
# rpm --checksig /usr/src/redhat/SRPMS/nco-${NCO_VRS}-?.src.rpm /usr/src/redhat/RPMS/i386/nco-${NCO_VRS}-?.i386.rpm # Check signatures
rpmnet:	${MY_BLD_DIR}/nco.spec # Building rpm requires root priveleges, e.g., sudo make NCO_VRS=3.1.8 rpmnet
	- if test -f /usr/src/redhat/SOURCES/nco-${NCO_VRS}.tar.gz; then printf "Using existing nco-${NCO_VRS}.tar.gz\n" ; else ${MY_BIN_DIR}/ncks -R -p ftp://dust.ess.uci.edu/pub/zender/nco -l /usr/src/redhat/SOURCES nco-${NCO_VRS}.tar.gz; fi
	/bin/rm -f /usr/src/redhat/SPECS/nco-${NCO_VRS}.spec; \
	cd /usr/src/redhat/SOURCES; \
	tar xvzf nco-${NCO_VRS}.tar.gz ./nco-${NCO_VRS}/bld/nco.spec; \
	mv ./nco-${NCO_VRS}/bld/nco.spec /usr/src/redhat/SPECS/nco.spec; \
	cd /usr/src/redhat/SPECS; \
	rpmbuild -ba --sign nco.spec; \
	scp /usr/src/redhat/SRPMS/nco-${NCO_VRS}-?.src.rpm /usr/src/redhat/RPMS/i386/nco-${NCO_VRS}-?.i386.rpm zender@dust.ess.uci.edu:/var/www/html/nco/src
rpm_cln:
	rpm --erase ${MDL_RPM_NST_NM}
# Targets in bm
bench:	bm
bnch:	bm
bm:	
	cd ${MY_BM_DIR}; \
	env MY_BIN_DIR=${MY_BIN_DIR} ../bm/nco_bm.pl --dbg=0 --bench --udpreport ${FL_FMT_SNG} ${MPI_PRC_SNG} ${THR_NBR_SNG}
file:	fl
fl:	
	cd ${MY_BM_DIR}; \
	env MY_BIN_DIR=${MY_BIN_DIR} ../bm/nco_bm.pl --dbg=0 --test_files=A --udpreport ${FL_FMT_SNG} ${MPI_PRC_SNG} ${THR_NBR_SNG}
regression:	tst
rgr:	tst
test:	tst
tst:	dat
	cd ${MY_BM_DIR}; \
	env MY_BIN_DIR=${MY_BIN_DIR} ../bm/nco_bm.pl --dbg=0 --regress --udpreport ${FL_FMT_SNG} ${MPI_PRC_SNG} ${THR_NBR_SNG}
# Targets in dat
# ncgen rules to build netCDF4 files must be error-tolerant to build on netCDF3-only installations
bug:	${MY_DAT_DIR}/buggy.nc
${MY_DAT_DIR}/buggy.nc: ${MY_DAT_DIR}/buggy.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
dap:	data
	-cd ~/nco/data; scp in.nc in.cdl dust.ess.uci.edu:/var/www/html/dodsdata; scp in.nc in.cdl esmf.ess.uci.edu:/var/www/html/dodsdata; scp in.nc in.cdl esmf.ess.uci.edu:/data/dodsdata
data:	dat
dat:	${MY_DAT_DIR}/cmip5.nc ${MY_DAT_DIR}/obs.nc ${MY_DAT_DIR}/dsm.nc ${MY_DAT_DIR}/hdn.nc ${MY_DAT_DIR}/hdf.hdf ${MY_DAT_DIR}/mdl_1.nc ${MY_DAT_DIR}/mdl_2.nc ${MY_DAT_DIR}/mdl_3.nc ${MY_DAT_DIR}/mrd.nc ${MY_DAT_DIR}/in.nc ${MY_DAT_DIR}/in_4c.nc ${MY_DAT_DIR}/in_grp.nc ${MY_DAT_DIR}/in_4.nc ${MY_DAT_DIR}/in_grp_1.nc ${MY_DAT_DIR}/in_grp_2.nc ${MY_DAT_DIR}/in_grp_3.nc ${MY_DAT_DIR}/in_grp_4.nc ${MY_DAT_DIR}/in_grp_5.nc ${MY_DAT_DIR}/in_grp_6.nc ${MY_DAT_DIR}/in_grp_7.nc ${MY_DAT_DIR}/in_grp_8.nc ${MY_DAT_DIR}/in_rec_zero.nc
	-for fl in ${MDL_DAT_STB}; do cd ${MY_DAT_DIR}; ln -s -f in.nc $$fl; done
${MY_DAT_DIR}/in.nc: ${MY_DAT_DIR}/in.cdl
	ncgen -b -o $@ $<
${MY_DAT_DIR}/in_4c.nc: ${MY_DAT_DIR}/in.nc
	-ncks -O -7 --cnk_dmn time,10 $< $@
# ncgen default chunksize for time = 4 MB produces ~200 MB in_4c.nc file. Avoid that!
#	-ncgen -k hdf5-nc3 -b -o $@ $<
${MY_DAT_DIR}/in_rec_zero.nc: ${MY_DAT_DIR}/in_rec_zero.cdl
	ncgen -b -o $@ $<
${MY_DAT_DIR}/in_grp.nc: ${MY_DAT_DIR}/in_grp.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_4.nc: ${MY_DAT_DIR}/in_4.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_1.nc: ${MY_DAT_DIR}/in_1.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_2.nc: ${MY_DAT_DIR}/in_2.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_grp_1.nc: ${MY_DAT_DIR}/in_grp_1.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_grp_2.nc: ${MY_DAT_DIR}/in_grp_2.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_grp_3.nc: ${MY_DAT_DIR}/in_grp_3.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_grp_4.nc: ${MY_DAT_DIR}/in_grp_4.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_grp_5.nc: ${MY_DAT_DIR}/in_grp_5.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_grp_6.nc: ${MY_DAT_DIR}/in_grp_6.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_grp_7.nc: ${MY_DAT_DIR}/in_grp_7.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/in_grp_8.nc: ${MY_DAT_DIR}/in_grp_8.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/cmip5.nc: ${MY_DAT_DIR}/cmip5.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/hdn.nc: ${MY_DAT_DIR}/hdn.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/mrd.nc: ${MY_DAT_DIR}/mrd.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/dsm.nc: ${MY_DAT_DIR}/dsm.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/obs.nc: ${MY_DAT_DIR}/obs.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/hdf.hdf: ${MY_DAT_DIR}/hdf.cdl
	-hncgen -b -o $@ $<
${MY_DAT_DIR}/mdl_1.nc: ${MY_DAT_DIR}/mdl_1.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/mdl_2.nc: ${MY_DAT_DIR}/mdl_2.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/mdl_3.nc: ${MY_DAT_DIR}/mdl_3.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/snd_ncwa.nc: ${MY_DAT_DIR}/snd_ncwa.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/snc_ncwa.nc: ${MY_DAT_DIR}/snc_ncwa.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/snd_grp.nc: ${MY_DAT_DIR}/snd_grp.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/snc_grp.nc: ${MY_DAT_DIR}/snc_grp.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/snc.nc: ${MY_DAT_DIR}/snc.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/snd.nc: ${MY_DAT_DIR}/snd.cdl
	-ncgen -k netCDF-4 -b -o $@ $<
${MY_DAT_DIR}/split.nc: ${MY_DAT_DIR}/split.cdl
	-ncgen -b -o $@ $<
dataclean: dat_cln
dat_cln:
	rm -f ${MDL_DAT}
	cd ${MY_DAT_DIR}; rm -f foo*
tst_ftp:	dat
	- if test -f ${MY_DAT_DIR}/nco_tst.nc; then printf "" ; else ${MY_BIN_DIR}/ncks -q -R -p ftp://dust.ess.uci.edu/pub/zender/nco -l ${MY_DAT_DIR} nco_tst.nc; if [ $? != 0 ]; then printf "WARNING: Unable to retrieve ftp://dust.ess.uci.edu/pub/zender/nco/nco_tst.nc required for self-test, possible problem with getting through your firewall? Manually download and install in directory ../data to continue self test...\n"; fi; fi
	cd ${MY_BLD_DIR}; \
	env MY_BIN_DIR=${MY_BIN_DIR} MY_DAT_DIR=${MY_DAT_DIR} ./nco_tst.pl
testclean: tst_cln
tst_cln:
	cd ${MY_DAT_DIR}; rm -f foo* nco_tst.nc
# Targets in doc
# Each make directive line spawns a separate shell so must use `cd dir;cmd' format
doc:	${MY_DOC_DIR}/nco.dvi ${MY_DOC_DIR}/nco.html ${MY_DOC_DIR}/nco.info ${MY_DOC_DIR}/nco.ps ${MY_DOC_DIR}/nco.pdf ${MY_DOC_DIR}/nco.txt ${MY_DOC_DIR}/nco.xml
${MY_DOC_DIR}/nco.dvi: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; texi2dvi --output=$@ $<
${MY_DOC_DIR}/nco.info: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; makeinfo --no-split --output=$@ $<
${MY_DOC_DIR}/nco.html: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; texi2html -monolithic -verbose $<
#	cd ${MY_DOC_DIR}; makeinfo --html --no-split --output=$@ $<
${MY_DOC_DIR}/nco.ps: ${MY_DOC_DIR}/nco.dvi
	cd ${MY_DOC_DIR}; dvips -o $@ nco.dvi
${MY_DOC_DIR}/nco.pdf: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; texi2dvi --pdf --output=$@ $<
#	cd ${MY_DOC_DIR}; ps2pdf -dMaxSubsetPct=100 -dCompatibilityLevel=1.2 -dSubsetFonts=true -dEmbedAllFonts=true nco.ps $@
${MY_DOC_DIR}/nco.txt: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; makeinfo --no-headers --no-split --output=$@ $<
# Neither of these xml formats seems to be viewable
${MY_DOC_DIR}/nco.xml: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; makeinfo --xml --no-split --output=$@ $<
#	cd ${MY_DOC_DIR}; makeinfo --docbook --no-split --output=$@ $<
docclean: doc_cln
doc_cln:
	cd ${MY_DOC_DIR}; rm -f nco.info* nco.dvi nco.html* nco.ps nco.pdf *~
# Targets in dpn
depend: dpn
dpn: ${MDL_DPN}
dpn_cln:
	rm -f ${MDL_DPN}
# Targets in inc
include: inc
inc: ${MDL_INC}
inc_cln:
	rm -f ${MDL_INC}
love: 
	echo "Not war?"
# Targets in lib
library: lib
lib : inc ${libnco}.a
ifeq (${PVM_ARCH},MACOSX)
	ranlib ${libnco}.a
endif # endif LINUX
# If not using glibc, build Sittler's getopt() and getopt_long() functions
ifeq (${null},$(findstring ${PVM_ARCH},FREEBSDLINUXALPHALINUXAMD64LINUXARMMACOSXWIN32))
${libnco}.a: ${libnco}.a(${MY_OBJ_DIR}/nco_getopt.o)
endif # endif not glibc
${libnco}.a: ${libnco}.a(${MY_OBJ_DIR}/nco_att_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_aux.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_bnr.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cln_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnf_dmn.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnf_typ.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnk.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnv_arm.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnv_csm.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_crt.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_ctl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_dbg.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_dmn_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_fl_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_flt.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_grp_trv.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_grp_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_kd.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_lmt.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_lst_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_map.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_md5.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_mmr.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_msa.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_mss_val.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_mta.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_netcdf.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_omp.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_pck.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_ply.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_ply_lst.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_ppc.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_prn.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_rec_var.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_rgr.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_rth_flt.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_rth_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_s1d.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_scl_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_scm.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_sld.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_sng_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_sph.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_srm.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_avg.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_lst.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_rth.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_scv.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_utl.o)
	$(AR) rsv $@ $^
libclean: lib_cln
lib_cln:
	rm -f ${libnco}.* ${libnco}*.* ${libnco}++.a ${libnco}_c++.a
# Targets in man
man: ${MDL_MAN}
	cd ../man;sudo cp -f *.1 ${MY_MAN_DIR}/man1
man_cln:
	rm -f ${MDL_MAN}
# Targets in obj
object: obj
obj: ${MDL_OBJ}
objclean: obj_cln
obj_cln:
	rm -f ${MDL_OBJ}
# Targets in root
autotools: dst_cln
	- cd ..; aclocal;autoheader;automake --foreign;autoconf ; ./configure --enable-optimize-custom --prefix=${HOME} --bindir=${MY_BIN_DIR} --datadir=${HOME}/nco/data --libdir=${MY_LIB_DIR} --mandir=${HOME}/nco/man ; ${MAKE} install
rootclean: root_cln
root_cln:
	- cd ..; rm -f config.h config.guess config.log config.status config.sub libtool Makefile stamp-h1; rm -r -f autom4te.cache conftest
# Targets in src
src: ${MDL_NCAP}
src_cln:
	rm -f ${MDL_NCAP}
# Targets in /usr/local
sys: ${MDL_BIN_TRG} ${MDL_BIN_SPT}
	- cd ${MY_BIN_DIR};sudo /bin/cp -f ${MDL_BIN_TRG} ${MDL_BIN_SPT} ncap2 /usr/local/bin; cd /usr/local/bin; sudo rm -f ncdiff ncea nces ncrcat ncunpack; sudo ln -s -f ncbo ncdiff; sudo ln -s -f ncra ncea; sudo ln -s -f ncra nces; sudo ln -s -f ncra ncrcat; sudo ln -s -f ncpdq ncunpack; sudo ln -s -f ncpdq ncpack; cd ${HOME}/nco/src/nco++; ${MAKE} -f Makefile.old OPTS=${OPTS} $@
sys_cln:
	- cd /usr/local/bin;rm -f ${MDL_BIN_TRG} ${MDL_BIN_SPT} ${MDL_BIN_SYM_LNK}

# Housekeeping
clean: cln
cln: lib_cln dpn_cln obj_cln src_cln tst_cln
	cd ${MY_DOC_DIR}; rm -f nco.aux nco.cp nco.cps nco.fn nco.ky nco.log nco.pg nco.toc nco.tp nco.vr 
debug: dbg
dbg:
	@printf "ABI = ${ABI}\n"
	@printf "AR = ${AR}\n"
	@printf "ARFLAGS = ${ARFLAGS}\n"
	@printf "BNR_SFX = ${BNR_SFX}\n"
	@printf "CC = ${CC}\n"
	@printf "CCACHE = ${CCACHE}\n"
	@printf "CFLAGS = ${CFLAGS}\n"
	@printf "CNK = ${CNK}\n"
	@printf "CPP = ${CPP}\n"
	@printf "CPPFLAGS = ${CPPFLAGS}\n"
	@printf "CPP_PTH = ${CPP_PTH}\n"
	@printf "CPP_TKN = ${CPP_TKN}\n"
	@printf "CURL_LIB = ${CURL_LIB}\n"
	@printf "CXX = ${CXX}\n"
	@printf "CXXFLAGS = ${CXXFLAGS}\n"
	@printf "DAP = ${DAP}\n"
	@printf "DAP_ROOT = ${DAP_ROOT}\n"
	@printf "DBG = ${DBG}\n"
	@printf "FC = ${FC}\n"
	@printf "FFLAGS = ${FFLAGS}\n"
	@printf "FL_FMT = ${FL_FMT}\n"
	@printf "GCC_RCH_ARG = ${GCC_RCH_ARG}\n"
	@printf "GSL = ${GSL}\n"
	@printf "GSL_INC_FLG = ${GSL_INC_FLG}\n"
	@printf "GSL_LIB_FLG = ${GSL_LIB_FLG}\n"
	@printf "HDF5_ROOT = ${HDF5_ROOT}\n"
	@printf "HDF5_INC = ${HDF5_INC}\n"
	@printf "HDF5_LIB = ${HDF5_LIB}\n"
	@printf "HOST = ${HOST}\n"
	@printf "HOSTNAME = ${HOSTNAME}\n"
	@printf "ICC_RCH_ARG = ${ICC_RCH_ARG}\n"
	@printf "INC_NCAR = ${INC_NCAR}\n"
	@printf "LAMMPICC = ${LAMMPICC}\n"
	@printf "LAMMPICXX = ${LAMMPICXX}\n"
	@printf "LDFLAGS = ${LDFLAGS}\n"
	@printf "LEX = ${LEX}\n"
	@printf "LFLAGS = ${LFLAGS}\n"
	@printf "LIB_NCAR = ${LIB_NCAR}\n"
	@printf "LINUX_CC = ${LINUX_CC}\n"
	@printf "LINUX_CXX = ${LINUX_CXX}\n"
	@printf "MDL_BIN = ${MDL_BIN}\n"
	@printf "MDL_BIN_SPT = ${MDL_BIN_SPT}\n"
	@printf "MDL_BIN_SYM_LNK = ${MDL_BIN_SYM_LNK}\n"
	@printf "MDL_BIN_TRG = ${MDL_BIN_TRG}\n"
	@printf "MDL_DPN = ${MDL_DPN}\n"
	@printf "MDL_MAN = ${MDL_MAN}\n"
	@printf "MDL_MPI_TRG = ${MDL_MPI_TRG}\n"
	@printf "MDL_OBJ = ${MDL_OBJ}\n"
	@printf "MDL_PTH = ${MDL_PTH}\n"
	@printf "MDL_RPM_NST_NM = ${MDL_RPM_NST_NM}\n"
	@printf "MDL_SRC = ${MDL_SRC}\n"
	@printf "MPI = ${MPI}\n"
	@printf "MPICC = ${MPICC}\n"
	@printf "MPICH_CC = ${MPICH_CC}\n"
	@printf "MPICH_CXX = ${MPICH_CXX}\n"
	@printf "MPICXX = ${MPICXX}\n"
	@printf "MPI_PRC = ${MPI_PRC}\n"
	@printf "MPI_PRC_SNG = ${MPI_PRC_SNG}\n"
	@printf "MPI_ROOT = ${MPI_ROOT}\n"
	@printf "MY_BIN_DIR = ${MY_BIN_DIR}\n"
	@printf "MY_BLD_DIR = ${MY_BLD_DIR}\n"
	@printf "MY_BLD_DIR = ${MY_BLD_DIR}\n"
	@printf "MY_DAT_DIR = ${MY_DAT_DIR}\n"
	@printf "MY_DOC_DIR = ${MY_DOC_DIR}\n"
	@printf "MY_DPN_DIR = ${MY_DPN_DIR}\n"
	@printf "MY_INC_DIR = ${MY_INC_DIR}\n"
	@printf "MY_LIB_DIR = ${MY_LIB_DIR}\n"
	@printf "MY_MAN_DIR = ${MY_MAN_DIR}\n"
	@printf "MY_OBJ_DIR = ${MY_OBJ_DIR}\n"
	@printf "NCO_LDFLAGS = ${NCO_LDFLAGS}\n"
	@printf "NCO_VRS = ${NCO_VRS}\n"
	@printf "NC_CFLAGS = ${NC_CFLAGS}\n"
	@printf "NC_LDFLAGS = ${NC_LDFLAGS}\n"
	@printf "NETCDF4 = ${NETCDF4}\n"
	@printf "NETCDF4_ROOT = ${NETCDF4_ROOT}\n"
	@printf "NETCDF_INC = ${NETCDF_INC}\n"
	@printf "NETCDF_LIB = ${NETCDF_LIB}\n"
	@printf "NETCDF_ROOT = ${NETCDF_ROOT}\n"
	@printf "OMP = ${OMP}\n"
	@printf "OPTS = ${OPTS}\n"
	@printf "OTHER_CFLAGS = ${OTHER_CFLAGS}\n"
	@printf "OTHER_LDFLAGS = ${OTHER_LDFLAGS}\n"
	@printf "PGI_RCH_ARG = ${PGI_RCH_ARG}\n"
	@printf "PNETCDF = ${PNETCDF}\n"
	@printf "PSC_RCH_ARG = ${PSC_RCH_ARG}\n"
	@printf "PVM_ARCH = ${PVM_ARCH}\n"
	@printf "RPM = ${RPM}\n"
	@printf "SRC_LST = ${SRC_LST}\n"
	@printf "STC = ${STC}\n"
	@printf "SZ = ${SZ}\n"
	@printf "SZ_LIB = ${SZ_LIB}\n"
	@printf "TAGS_FILES = ${TAGS_FILES}\n"
	@printf "TAGS_FILTER_FILES = ${TAGS_FILTER_FILES}\n"
	@printf "THR_NBR = ${THR_NBR}\n"
	@printf "THR_NBR_SNG = ${THR_NBR_SNG}\n"
	@printf "UDUNITS = ${UDUNITS}\n"
	@printf "UDUNITS_INC = ${UDUNITS_INC}\n"
	@printf "UDUNITS_LIB = ${UDUNITS_LIB}\n"
	@printf "UDUNITS_INC_FLG = ${UDUNITS_INC_FLG}\n"
	@printf "UDUNITS_LIB_FLG = ${UDUNITS_LIB_FLG}\n"
	@printf "VPATH = ${VPATH}\n"
	@printf "VRS_SNG = ${VRS_SNG}\n"
	@printf "YACC = ${YACC}\n"
	@printf "YFLAGS = ${YFLAGS}\n"
distclean: dst_cln
dst_cln: cln bin_cln dat_cln doc_cln root_cln
	cd ${MY_BLD_DIR}; rm -f *~
	cd ${MY_DAT_DIR}; rm -f buggy.nc cmip5.nc dsm.nc obs.nc hdf.hdf mdl_1.nc mdl_2.nc mdl_3.nc in.nc in_4c.nc in_4.nc in_grp.nc in_grp_1.nc in_grp_2.nc in_grp_3.nc in_grp_4.nc in_grp_5.nc in_grp_6.nc in_grp_7.nc in_grp_8.nc in_rec_zero.nc *~
tags: 
	etags ${TAGS_FILES}
#	etags ${MY_SRC_DIR}/*.h $(filter-out ${TAGS_FILTER_FILES},${SRC_LST}) ${MDL_DOC_SRC} ${MDL_MAN} ${MDL_BLD_SRC} ${MDL_CXX_SRC} ${MDL_NCAP_SRC}

# Reset internal YACC and LEX patterns
%.c : %.y
%.c : %.l

# It is safest to do both YACC and LEX after either file changes
# Otherwise only changing one and then switching, e.g., from bison to yacc, can cause problems
# NB: Bison has problem when bison.simple declares yyparse() as int yyparse (void);
# Solution is to comment out that definition in bison.simple
ifeq (${YACC},bison)
${MY_SRC_DIR}/%_yacc.c ${MY_SRC_DIR}/%_yacc.h : ${MY_SRC_DIR}/%_yacc.y
	${YACC} ${YFLAGS} --output=$(basename $<).c -d $<
#${MY_SRC_DIR}/%_yacc.c ${MY_SRC_DIR}/%_yacc.h : ${MY_SRC_DIR}/%_yacc.y
#	${YACC} ${YFLAGS} --file-prefix=$(notdir $($(basename $<))) -d $<
#%_yacc.c %_yacc.h : %_yacc.y
#	${YACC} ${YFLAGS} --file-prefix=$(notdir $($(basename $<))) -d $<
endif
ifeq (${YACC},yacc)
%.tab.c %.tab.h : %.y
	${YACC} ${YFLAGS} $<
	mv y.tab.c $(basename $<).c
	mv y.tab.h $(basename $<).h
endif

%_lex.c : %_lex.l
	${LEX} ${LFLAGS} $<
	mv lex.${NCO_YY_PFX}.c $(basename $<).c

# Target-specific variable values syntax TARGET ... : VARIABLE-ASSIGNMENT
# Rules begin in leftmost column else interpreted as commands
ifneq (${null},$(findstring ${PVM_ARCH},LINUXALPHALINUXAMD64LINUXARMFREEBSD))
ifeq (gcc,$(firstword ${CC}))
${MY_OBJ_DIR}/nco_grp_trv.o : CFLAGS := $(filter-out -Wcast-qual,${CFLAGS})
${MY_OBJ_DIR}/nco_grp_trv.o : CXXFLAGS := $(filter-out -Wcast-qual,${CXXFLAGS})
endif # endif GCC
endif # endif LINUX

# ncap, and only ncap, requires AIX system C++ library
ifneq (${null},$(findstring AIX,${PVM_ARCH}))
ifneq (${null},$(findstring xl,${CC}))
# Target-specific variable values syntax TARGET ... : VARIABLE-ASSIGNMENT
# Rules begin in leftmost column else interpreted as commands
ncap : LDFLAGS += -lC
endif # endif AIX VA Compiler Collection
endif # endif AIX
# For some reason, ncap_lex.c is not remade when I expect it to be, so I explicitly remove the object file every time
ncap:	${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o ${libnco}.a

ncap2 :
	-cd ../src/nco++; ${MAKE} -f Makefile.old DAP=${DAP} DBG=${DBG} GSL=${GSL} MPI=${MPI} NETCDF4=${NETCDF4} OMP=${OMP} OPTS=${OPTS} PNETCDF=${PNETCDF} SZ=${SZ} UDUNITS=${UDUNITS} all

#	/bin/rm -f ${MDL_NCAP} ${MY_OBJ_DIR}/$@_lex.o

# Create dependency files only if they will not be immediately deleted
INCLUDE_DPN := TRUE
GOALS_WHICH_DELETE_DEPENDENCY_FILES := cln clean dir distclean dst_cln dpn_cln tags uninstall
ifeq (${null},$(findstring $(MAKECMDGOALS),${GOALS_WHICH_DELETE_DEPENDENCY_FILES}))
 INCLUDE_DPN := TRUE
else
 INCLUDE_DPN := FALSE
endif
ifeq (${INCLUDE_DPN},TRUE)
-include ${MDL_DPN}
endif
