# $Header: /data/zender/nco_20150216/nco/bld/Makefile,v 1.326 2005-09-15 21:14:54 zender Exp $ -*-makefile-*-

# Purpose: GNU Makefile for NCO module nco
# Requires GNU Make---AT&T Make chokes on this make syntax

# Copyright (C) 1994--2005 Charlie Zender
# This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL)
# See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text

# Quickie test copies:
# scp ~/nco/bld/Makefile dust.ess.uci.edu:nco/bld
# scp ~/nco/bld/Makefile esmf.ess.uci.edu:nco/bld
# scp ~/nco/bld/Makefile goldhill.cgd.ucar.edu:nco/bld
# scp ~/nco/bld/Makefile soot.ess.uci.edu:nco/bld

# Usage (testing):
# make MPI_PRC=3 tst                 # MPI regression test
# make MPI_PRC=3 bm                  # MPI benchmarks
# make THR_NBR=2 tst                 # OpenMP regression test
# make THR_NBR=2 bm                  # OpenMP benchmarks
# Test netCDF4:
# sudo ln -s -f /usr/local/lib/libnetcdf4.a.gcc-g95 /usr/local/lib/libnetcdf.a
# make USR_TKN=-DNETCDF4             # Pass user-defined tokens to CC & CPP

# Usage (Compilation):
# cd ~/nco/bld;make;cd -             # Default build
# cd ~/nco/bld;make dir all; cd -    # Create target directories then build
# cd ~/nco/bld;make cln bin_cln;cd - # Clean all dependencies for fresh build
# cd ~/nco/bld;make DAP=Y;cd -       # DAP support
# cd ~/nco/bld;make dbg;cd -         # Print make diagnostics
# cd ~/nco/bld;make lib_cln;cd -     # Clean libraries
# cd ~/nco/bld;make OMP=Y;cd -       # OpenMP support
# cd ~/nco/bld;make OPTS=D;cd -      # "Debugging": Enable symbols for debugging
# cd ~/nco/bld;make OPTS=O;cd -      # "Optimize": For production (includes symbols)
# cd ~/nco/bld;make OPTS=R;cd -      # "Regular": No optimization or debugging
# cd ~/nco/bld;make OPTS=X;cd -      # "eXtreme": All debugging switches available
# cd ~/nco/bld;make MPI=Y;cd -       # MPI support
# cd ~/nco/bld;make MPI_FAKE=Y;cd -  # MPI spoof
# cd ~/nco/bld;make UDUNITS=Y;cd -   # UDUnits support
# cd ~/nco/bld;make --jobs=4;cd -    # Parallel make
# 32-bit ABI on NCAR AIX systems:
# cd ~/nco/bld;NETCDF_LIB=/usr/local/lib32/r4i4 make --jobs=1 lib_cln cln all ABI=32 UDUNITS=N;cd -
# 32-bit ABI on NCAR IRIX systems:
# cd ~/nco/bld;NETCDF_LIB=/usr/local/lib32/r4i4 UDUNITS_INC=/contrib/include UDUNITS_LIB=/contrib/lib make --jobs=1 lib_cln cln all ABI=32;cd -

# Top-level tokens defining directory structure
# These tokens may be over-ridden by environment variables or when invoking make, e.g., make DAP=Y
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
 MY_MAN_DIR := /usr/local/man
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

# Primary tokens which determine build options
# Specify non-default when invoking make, e.g. make DAP=Y
ifndef ${ABI}
# 32 vs. 64 bit ABI: 32=32 bit mode, 64=64 bit mode (default) if available
 ABI := 64
endif # endif ABI
ifndef ${DBG}
# Debugging token N=No (default) Y=Yes
 DBG := N
endif # endif DBG
ifndef DAP
 DAP := N
endif
ifndef GCC_RCH_ARG
 GCC_RCH_ARG :=
endif # endif GCC_RCH_ARG
ifndef HDF5
 HDF5 := N
endif # endif HDF5
ifndef ICC_RCH_ARG
 ICC_RCH_ARG :=
endif # endif ICC_RCH_ARG
ifndef I18N
 I18N := N
endif
ifndef MK_DPN
 MK_DPN = ${CPP} -M # NB: Recursive expansion required
 MK_DPN_CXX = ${CXX} -M # NB: Recursive expansion required
endif # endif MK_DPN
ifndef ${MPI} # MPI
 MPI := N
endif # endif MPI
ifndef ${MPI_FAKE} # MPI
 MPI_FAKE := N
endif # endif MPI_FAKE
ifndef NCO_VRS
# Used for RPM building
 NCO_VRS := $(shell cat ${MY_DOC_DIR}/VERSION)
endif
ifndef NETCDF_INC
 NETCDF_INC := /usr/local/include # Directory containing netcdf.h
endif
ifndef NETCDF_LIB
 NETCDF_LIB := /usr/local/lib # Directory containing libnetcdf.a
endif
ifndef ${OMP} # OpenMP
 OMP := Y
endif # endif OMP
ifndef OPTS
 OPTS := O
endif
ifndef RPM
 RPM := N
endif # endif RPM
ifndef UDUNITS
# Use UDUnits functionality
 UDUNITS := Y
endif
ifndef UNAMES
 UNAMES := $(shell uname -s)
endif
ifndef USE_FORTRAN_ARITHMETIC
# Whether or not to use Fortran arithmetic routines (feature deprecated in 2001)
 USE_FORTRAN_ARITHMETIC := N
endif
ifndef USR_TKN
 USR_TKN :=
endif # endif USR_TKN
ifndef VRS_SNG
 VRS_SNG := $(shell date +%Y%m%d)
endif # endif VRS_SNG
# Derived-tokens based on primary tokens
# These tokens should not be altered by hand

# NB: CPP_TKN is recursively expanded variable, define only when components are ready
CPP_TKN = ${USR_TKN} -D${PVM_ARCH} -DNO_NETCDF_2 -DVERSION='${VRS_SNG}' -DHOSTNAME='${HOST}' -DUSER='${USER}' -DNCO_ABORT_ON_ERROR

ifeq (${DAP},Y)
# Build NCO as DAP-enabled clients
# fxm: OPeNDAP bloats executables, should automagically enable stripping?
# http://www.opendap.org/user/guide-html/guide_28.html
ifndef DAP_ROOT
# Directory containing libdap++.a, libnc-dap.a
 DAP_ROOT := /usr/local
endif
 NC_LDFLAGS := -L${DAP_ROOT}/lib
# NB: nc-dods, dap++ prior to version 3.5.X must be linked twice!
# DODS 3.3-
# NC_LIBS := -lnc-dods -ldap++ -lnc-dods -ldap++ -lwww -lz -lrx
# DODS 3.4.X
# NC_LIBS := -lnc-dods -ldap++ -lnc-dods -ldap++ -lxml2 -lcurl -lpthread -ldl -lz
# DAP 3.5+
 NC_LIBS := -lnc-dap -ldap++ -lxml2 -lcurl
# Get netCDF from OPeNDAP version
 NETCDF_INC=${DAP_ROOT}/include/libnc-dap
 NETCDF_LIB=${DAP_ROOT}/lib
else
# Build NCO straight, without DAP
 NC_LDFLAGS := -L${NETCDF_LIB}
 NC_LIBS := -lnetcdf
endif # end DAP

ifeq (${HDF5},Y)
# Build NCO using HDF5 netCDF interface
 HDF5_ROOT := /usr/local/hdf5
 NETCDF_INC := $(HDF5_ROOT)/include # Directory containing HDF5 version of netcdf.h
 NETCDF_LIB := $(HDF5_ROOT)/lib -lmfhdf -ldf -ljpeg -lz # Directory containing HDF5 version of libnetcdf.a routines, order of library linkage is significant
endif # endif HDF5

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
endif # MPI_FAKE
ifdef MPI_PRC
# MPI_PRC tells test scripts how many MPI processes to spawn
# Pass this MPI argument to nco_bm.pl
 MPI_PRC_SNG := "--mpi_prc=${MPI_PRC}"
# MPI_PRC implies MPI
 MPI := Y
else # !MPI_PRC
 MPI_PRC_SNG :=
endif # !MPI_PRC
#ifeq (${MPI},Y)
#ifneq (${MPI_FAKE},Y)
# Prohibit hybrid code for now---turn off OpenMP
# OMP := N
#endif # MPI_FAKE
#endif # !MPI

ifneq (${null},$(findstring LINUX,${PVM_ARCH}))
# Decide among the plethora of Linux compilers
ifndef LINUX_CXX
# C++ compiler for Linux
 LINUX_CXX := g++
 #LINUX_CXX := como
 #LINUX_CXX := icpc
 #LINUX_CXX := insure
 #LINUX_CXX := pgCC
endif # endif LINUX_CXX
ifndef LINUX_CC
# C compiler for Linux
 LINUX_CC := gcc -std=c99 -pedantic -D_BSD_SOURCE -D_POSIX_SOURCE
 #LINUX_CC := como --c99
 #LINUX_CC := icc -std=c99 -D_BSD_SOURCE -D_POSIX_SOURCE
 #LINUX_CC := insure
 #LINUX_CC := pgcc -std=c99
endif # endif LINUX_CC
ifndef LINUX_FC
# Fortran compiler for Linux
 LINUX_FC := g95
 #LINUX_FC := gfortran
 #LINUX_FC := ifort
 #LINUX_FC := lf95
 #LINUX_FC := pgf90
endif # endif LINUX_CC
endif # endif LINUX

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

ifeq (${UDUNITS},Y)
# Build UDUnits-enabled NCO 
# Place UDUNITS block after DAP block for both to work together
ifndef UDUNITS_INC
 UDUNITS_INC := /usr/local/include # Directory containing udunits.h
endif
ifndef UDUNITS_LIB
 UDUNITS_LIB := /usr/local/lib # Directory containing libudunits.a
endif
ifneq (${UDUNITS_INC},${NETCDF_INC})
 NETCDF_INC += -I${UDUNITS_INC}
endif # end 
ifneq (${UDUNITS_LIB},${NETCDF_LIB})
 NC_LDFLAGS += -L${UDUNITS_LIB}
endif # end 
 NC_LIBS += -ludunits
endif # end if UDUNITS

# TMP_* and NCO_* are required to play nicely with DAP flags
NCO_LDFLAGS := -L${MY_LIB_DIR}
NCO_LIBS := -lnco
TMP_LDFLAGS := ${NCO_LDFLAGS} ${NC_LDFLAGS}
TMP_LIBS := ${NCO_LIBS} ${NC_LIBS}

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
# Find all C++, C, and Fortran files in a given directory
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 FIND_FNC = $(wildcard ${dir}/*.cc ${dir}/*.c ${dir}/*.F) # Compile Fortran only if necessary
else
 FIND_FNC = $(wildcard ${dir}/*.cc ${dir}/*.c)
endif
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
ifeq (${MPI},Y)
 MDL_MPI_TRG := mpncbo mpncecat mpncflint mpncpdq mpncra mpncwa # MPI binary targets
 MDL_BIN_TRG := ${MDL_BIN_TRG} ${MDL_MPI_TRG} # NCO binary targets
endif # endif MPI
MDL_BIN_SYM_LNK := ncdiff ncea ncrcat # Symbolic links
MDL_BIN_STB := ${MDL_BIN_TRG} ${MDL_BIN_SYM_LNK} # All NCO files in MY_BIN_DIR
MDL_BIN := $(addprefix ${MY_BIN_DIR}/,${MDL_BIN_STB}) # distclean removes these files

# Variables having to do with header files created by module
# List header targets alphabetically by "category":
MDL_INC_TRG := # Raw (no functions)
MDL_INC_TRG += libnco.h # libnco
MDL_INC_SYM_LNK := # Symbolic links
MDL_INC_STB = ${MDL_INC_TRG} ${MDL_INC_SYM_LNK} # All header files in ${MY_INC_DIR}
MDL_INC = $(addprefix ${MY_INC_DIR}/,${MDL_INC_STB}) # dst_cln removes these files

# Variables having to do with NCO data
MDL_DAT_STB := 85 86 87 88 89 h0001 h0002 h0003 # Symbolic links to in.nc
MDL_DAT_STB := $(addsuffix .nc,$(MDL_DAT_STB)) # `make data' creates these files
MDL_DAT := $(addprefix ${MY_DAT_DIR}/,$(MDL_DAT_STB)) # `make distclean' removes these files

# Variables having to do with NCO documentation
MDL_DOC_SRC := $(addprefix ../,acinclude.m4 configure.in configure.eg Makefile.am) $(addprefix ${MY_DOC_DIR}/,nco.texi ANNOUNCE MANIFEST NEWS README TODO VERSION debian.txt dods.sh opendap.sh index.shtml ncap.txt nco_news.shtml nco_src_frg.txt) $(addprefix ${MY_DBN_DIR}/,changelog compat control convert copyright docs files info postinst postrm prerm rules nco.doc-base) # `make tags' includes these files
MDL_DOC_TRG := nco.dvi nco.html nco.info nco.pdf nco.ps nco.txt nco.xml # `make doc' creates these files
MDL_DOC := $(addprefix ${MY_DOC_DIR}/,$(MDL_DOC_TRG)) # `make distclean' removes these files

MDL_MAN = $(wildcard ${MDL_MAN_DIR}/*.1)
MDL_MAN := $(notdir $(MDL_MAN))
MDL_MAN := $(addprefix ${MY_MAN_DIR}/,${MDL_MAN}) # distclean removes these files

# Variables having to do with NCO build
MDL_BLD_SRC := $(addprefix ${MY_BLD_DIR}/,libnco_tst.c libnco_c++_tst.cc Makefile nco.spec nco_dst.pl nco_tst.pl) # `make tags' includes these files

# Variables having to do with ncap
MDL_NCAP_SRC := $(addprefix ${MY_SRC_DIR}/,ncap_yacc.y ncap_lex.l ncap.h) # `make tags' includes these files
MDL_NCAP_TRG := ncap_yacc.c ncap_yacc.h ncap_lex.c # `make ncap' creates these files
MDL_NCAP := $(addprefix ${MY_SRC_DIR}/,$(MDL_NCAP_TRG)) # `make distclean' removes these files

# Redefine default C and C++ pattern rules
${MY_OBJ_DIR}/%.o : %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o ${MY_OBJ_DIR}/$(notdir $@)
${MY_OBJ_DIR}/%.o : %.cc
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
%.po : %.cc
	xgettext --default-domain=$* --join-existing $<
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

${MY_DPN_DIR}/%.d : %.cc
# NB: Use ${CXX} rather than ${CPP} on C++ files for now because, e.g., SUNMP cpp does not pre-process .cc files quite correctly
# Extra hack to allow C++ filenames to contain '+' character
# $(subst ${plus},${plus_rx},${*}) is filename stub with an Rx in place of '+'
	${MK_DPN_CXX} ${CXXCPPFLAGS} $< | perl -p -e 's/\${plus}/${plus_rx}/g;s/$(subst ${plus},${plus_rx},${*})\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g;s/${plus_rx}/\${plus}/g' > $@

# First LDFLAGS is for typical C programs with netCDF, math, and networking
# Second LDFLAGS, when present, enables C/Fortran linking

# Manually define autotools tokens normally defined in HAVE_CONFIG_H
# Initialize OS-specific tokens to empty
CPP_TKN_OS := -DHAVE_REGEX_H -DNCO_HAVE_REGEX_FUNCTIONALITY
ifneq (${PVM_ARCH},CRAY)
 CPP_TKN_OS += -DHAVE_MKSTEMP
endif # CRAY
ifneq (${null},$(findstring ${PVM_ARCH},LINUXALPHALINUXAMD64LINUXARMFREEBSDWIN32))
 CPP_TKN_OS += -DHAVE_GETOPT_H -DHAVE_GETOPT_LONG
# UDUNITS_DAT OK to be empty for now
endif # !LINUX
ifneq (${null},$(findstring ${PVM_ARCH},AIXSGIMP64))
 CPP_TKN_OS += -DNEED_GETOPT_LONG
endif # !(AIX || SGI)
ifeq (${DAP},Y)
 CPP_TKN_OS += -DENABLE_DAP
endif # !DAP
ifeq (${MPI},Y)
ifneq (${MPI_FAKE},Y)
 CPP_TKN_OS += -DENABLE_MPI
endif # MPI_FAKE
endif # !MPI
ifeq (${UDUNITS},Y)
 CPP_TKN_OS += -DENABLE_UDUNITS -DHAVE_UDUNITS_H
endif # !UDUNITS
# Assume strcasecmp() and strdup() routines are present (Comeau is exception)
CPP_TKN_OS += 
# fxm: Define HAVE_LIBINTL, HAVE_LOCALE_H, HAVE_GETTEXT, HAVE_OMP_H

# Works on AIX and AIX46K
ifneq (${null},$(findstring AIX,${PVM_ARCH}))
# 20030804: Always use re-entrant (_r) compilers---Jim Edwards NCAR/IBM 
 CC := xlc_r -qlanglvl=extc99
#CC := gcc -std=c99 -pedantic -D_BSD_SOURCE -D_POSIX_SOURCE
 CXX := xlC_r
# CXX := g++
 CPP := xlc -c -qlanglvl=extc99 -I/usr/lpp/ppe.poe/include
 FC := xlf95_r
ifneq (${null},$(findstring xl,${CC}))
# /usr/include headers must occur before Visual Age headers to prevent xlC pragma warnings
# Visual Age compiler headers must occur before g++ headers
 CPP_TKN_OS += -I/usr/include -I/usr/vacpp/include
endif # xlC compilers
# Add /usr/local/include for libintl.h explicitly until netCDF is moved there
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC} -I/usr/local/include -DNEED_LOGF
LD := ld
# 20020422: -lC links to AIX C++ library which contains float intrinsics cosf()...
# -bh:5 suppresses annoying messages from xlC linker WARNING: Duplicate symbol: ...
LDFLAGS += -bh:5 ${TMP_LDFLAGS} ${TMP_LIBS} -lm -lC
LEX := flex
LINT := lint
YACC := bison
# AIX VA Compiler Collection
ifneq (${null},$(findstring xl,${CC}))
# Additional switch to fix compiler warnings on csz.c
# -qmaxmem=num Limit memory used by space intensive optimizations to <num> kilobytes
# -qspill=size Size in B of register allocation spill area, mie needs > 1040 B
# -qsrcmsg prints transgressing source line with finger
 CFLAGS := -qmaxmem=8192 -qspill=2048 -qsrcmsg
 FFLAGS := -NS2000 -qfixed=132 -qsrcmsg
# -bh:5 suppresses annoying messages from xlC linker WARNING: Duplicate symbol: ...
 LDFLAGS += -bh:5
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
 CFLAGS := -O3 -g -qstrict -Q
 FFLAGS += -O3 -g -qstrict -Q
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS += -g
endif
ifeq (${OPTS},X)
# -qflttrap generates instructions for floating point exceptions
# -qidirfirst uses headers found in -I directories first
# -qlonglong allow long long integers
# -qmakedep creates .d file
# -qwarn64 check for possible long-to-integer or pointer-to-integer truncation
# -qhalt=e stop compiler if error severity equals or exceeds i, w, e, s, u
 CFLAGS += -g -qflttrap -qidirfirst -qlonglong -qwarn64 -qcheck=all -qhalt=s
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
 CFLAGS := -Wall -Wunused
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
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := f90
FFLAGS := -r8 -i4 -c ${OMP_FLG} -automatic
FIXEDFLAGS := -extend_source ${OMP_FLG} -automatic
FREEFLAGS := -DHIDE_SHR_MSG -free
LD := ld
LDFLAGS += ${OMP_FLG} ${TMP_LDFLAGS} ${TMP_LIBS} -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS +=
endif
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
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := f90
# -F enables macro substitution
# -dp enables DOUBLEPRECISION/double
FFLAGS := -N 132 -F -dp
LD := ld
LDFLAGS += ${TMP_LDFLAGS} ${TMP_LIBS} -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -L/lib -lf
endif
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
CC := gcc -std=c99 -pedantic -D_BSD_SOURCE -D_POSIX_SOURCE
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := f77
LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lnsl -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -lF77 -lM77
endif
LDFLAGS += 
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

# Works on LINUX, LINUXALPHA, LINUXAMD64, LINUXARM, and FREEBSD
ifneq (${null},$(findstring ${PVM_ARCH},LINUXALPHALINUXAMD64LINUXARMFREEBSD))
CXX := ${LINUX_CXX}
CC := ${LINUX_CC}
CPP := ${CC}
# NB: nameser.h needs -Di386, but gcc sends -Di586 (on pentiums)
CPP_TKN_OS += -Di386
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := ${LINUX_FC}
LD := ld
LDFLAGS += ${TMP_LDFLAGS} ${TMP_LIBS} -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS +=
endif
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
# GNU Compiler Collection
ifneq (${null},$(findstring gcc,${CC}))
 CPPFLAGS += -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE
 CFLAGS := -Wall
# Compilation flags for numerical routines recommended by GSL 1.3 manual, p. 397
# CFLAGS += -Werror -Wall -W -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fno-common -g -O4
# Compilation flags recommended by GSL that I like and use:
# -D_BSD_SOURCE: Support 4.3 BSD Unix extensions to ANSI C (prevents nameser.h warnings)
# -D_POSIX_SOURCE: Support POSIX.1 standard additions to ANSI C (prevents fileno warnings)
# -pedantic: Disallow non-ISO constructs (including type long long) (sometimes useful)
# -Werror: Consider warnings as errors
# -W: Extra warnings, including missing return values, comparison of signed with unsigned
# -Wall: Warn about common programming problems
# -Wmissing-prototypes: Warn if missing prototypes
# -Wshadow: Warn if local variable has same name as other local variable
# -Wpointer-arith: Warn if pointer arithmetic on types without size, e.g., void
# -Wcast-qual: Warn if const qualifier removed from pointer
# -Wcast-align: Warn if casting pointer to type of different size
# -Wwrite-strings: Apply const-qualifier to string constants, die if overwritten
# -fno-common: Prevent global variables from being simultaneously defined in different files
# -g: Put debugging symbols in executable
# -pg: Enable profiling, generate gmon.out output files (also needed by linker)
# -O4: Turn on optimization so unitialized variables are flagged
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
 CFLAGS += -g -O -pg -fno-inline
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
endif # endif GNU Compiler Collection
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
# remark #981: operands are evaluated in unspecified order
# remark #810: conversion from "double" to "float" may lose significant bits
# remark #1572: floating-point equality and inequality comparisons are unreliable
 CFLAGS := -w1
 CPPFLAGS += -no-gcc
 OMP_FLG := -openmp -openmp_report0
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
 OMP_FLG := -openmp
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
# Portland Group C++ Compiler
ifeq (${CC},pgcc)
# Enable Large File Support (LFS) by default
 CFLAGS := -Mlfs
 LDFLAGS += -Mlfs
 OMP_FLG := -mp
ifeq (${OPTS},D)
 CFLAGS += -g
endif
ifeq (${OPTS},O)
 CFLAGS += -fast
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
 OMP_FLG := -mp
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
ifeq (${OMP},Y)
 CFLAGS += ${OMP_FLG}
 FFLAGS += ${OMP_FLG}
 LDFLAGS := ${OMP_FLG} ${LDFLAGS}
endif # endif OMP
endif
# endif LINUX, LINUXALPHA, LINUXAMD64, LINUXARM, FREEBSD

ifeq (${PVM_ARCH},MACOSX)
CXX := c++
CC := cc
CFLAGS := -Wall
CPP := ${CC}
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := f90
LD := ld
LDFLAGS += ${TMP_LDFLAGS} ${TMP_LIBS} -lm
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
# endif MACOSX

ifeq (${PVM_ARCH},NECSX)
ifeq (${OMP},Y)
 OMP_FLG := -Popenmp
endif # endif OMP
CXX := c++
#CC := c++ -Xa
CC := cc
CPP := c++ -E
#CPP := /usr/lib/cpp
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := f90
LD := ld
LDFLAGS += ${TMP_LDFLAGS} ${TMP_LIBS} -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
# LDFLAGS += -L/lib -lf
endif
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
CC := gcc -std=c99 -pedantic -D_BSD_SOURCE -D_POSIX_SOURCE
CPP := /lib/cpp -P
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := xlf
LD := ld
LDFLAGS += ${TMP_LDFLAGS} ${TMP_LIBS} -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -lxlf90 -lxlf
endif
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
CC := cc
# 20000302: -w suppresses warnings which will swamp linker
#CXX := g++ -w
#CC := gcc -std=c99 -pedantic -D_BSD_SOURCE -D_POSIX_SOURCE
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
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
# SGI6
GCC_ABI_FLG := -mabi=32
GCC_LDFLAGS_SZ_SPC := ${GCC_ABI_FLG} -mips3 
SGI_ABI_FLG := -n32 -mips3 ${OMP_FLG}
# endif SGI6
else
# SGI64, SGIMP64
GCC_ABI_FLG := -mabi=64
GCC_LDFLAGS_SZ_SPC := ${GCC_ABI_FLG} -mips4 -L/usr/local/lib/mabi=64
SGI_ABI_FLG := -64 -mips4 ${OMP_FLG}
# endif SGI64, SGIMP64
endif
ifeq (${CC},gcc)
 LDFLAGS += $(GCC_LDFLAGS_SZ_SPC) ${TMP_LDFLAGS} ${TMP_LIBS} -lm
ifeq (${OPTS},O)
CFLAGS += ${GCC_ABI_FLG} -O2 -g -Wall
endif
ifeq (${OPTS},R)
 CFLAGS += ${GCC_ABI_FLG} -Wall
endif
ifeq (${OPTS},D)
 CFLAGS += ${GCC_ABI_FLG} -g -Wall
endif
ifeq (${OPTS},X)
 CFLAGS += ${GCC_ABI_FLG} -g -O -Wall
endif
 CXXFLAGS := ${CFLAGS}
endif
# endif CC=gcc
ifeq (${CC},cc)
 LDFLAGS += ${SGI_ABI_FLG} ${TMP_LDFLAGS} ${TMP_LIBS} -lm
ifeq (${OPTS},O)
 CFLAGS += ${SGI_ABI_FLG} -O2 -g
endif
ifeq (${OPTS},R)
 CFLAGS += ${SGI_ABI_FLG}
endif
ifeq (${OPTS},D)
 CFLAGS += ${SGI_ABI_FLG} -g
endif
ifeq (${OPTS},X)
 CFLAGS += ${SGI_ABI_FLG} -g -trapuv
endif
endif
# endif CC=cc
# Fortran flags
ifeq (${OPTS},O)
 FFLAGS := ${SGI_ABI_FLG} -O2 -g -extend_source
endif
ifeq (${OPTS},R)
 FFLAGS := ${SGI_ABI_FLG} -extend_source
endif
ifeq (${OPTS},D)
 FFLAGS := ${SGI_ABI_FLG} -g -extend_source
endif
ifeq (${OPTS},X)
 FFLAGS := ${SGI_ABI_FLG} -g -extend_source -check_bounds -trapuv
endif
# 1998/08/20: Using range checking results in unresolved symbol errors unless linking to this library
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -L/usr/lib64 -lftn
endif
# end fortran flags
endif
# endif SGI6, SGI64, SGIMP64

ifeq (${UNAMES},SunOS) 
CXX := g++
CC := gcc -std=c99 -pedantic -D_BSD_SOURCE -D_POSIX_SOURCE
CFLAGS := -Wall
CPP := cpp
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := f90 -DHIDE_SHR_MSG
#FFLAGS := -xs -stackvar -e -Qoption f90comp -r8const
FFLAGS := -xs -stackvar -e 
LD := ld
LDFLAGS += ${TMP_LDFLAGS} ${TMP_LIBS} -lsunmath -lsocket -lnsl -lm
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
CC := gcc -std=c99 -pedantic -D_BSD_SOURCE -D_POSIX_SOURCE
# NB: nameser.h needs -Di386, but gcc is sending -Di586 (on pentiums)
CPP_TKN_OS += -Di386 -I/usr/include
CPPFLAGS := ${CPP_TKN} ${CPP_TKN_OS} ${CPP_PTH} -I${NETCDF_INC}
FC := g77
LD := ld
LDFLAGS += ${TMP_LDFLAGS} ${TMP_LIBS} -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS +=
endif
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

ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 CPPFLAGS += -DUSE_FORTRAN_ARITHMETIC
endif

# Link to DAP libraries first, see OPeNDAP User's Guide
# Currently this is invoked by using 'make DAP=Y'
ifeq (${DAP},Y)
 CPPFLAGS += -DNC_64BIT_OFFSET=0
ifneq (${CC},g++)
# g++ automatically links to -lstdc++, other compilers may need help
 LDFLAGS := ${LDFLAGS} -lstdc++
endif
endif
# endif DAP

# Internationalize NCO with i18n features
ifeq (${I18N},Y)
 CPPFLAGS += -DI18N
ifneq (${null},$(findstring SGI,${PVM_ARCH}))
 LDFLAGS += -lintl
endif
endif
# endif I18N

ifeq (${HDF5},Y)
 CPPFLAGS += -DHDF5
# No need to link to libnetcdf.a since libmfhdf.a has all netCDF3 calls
 LDFLAGS := $(filter-out -lnetcdf,${LDFLAGS})
endif
# endif HDF5

# Link to Electric Fence library, see man efence
ifeq (${PVM_ARCH},LINUX)
ifneq (${null},$(findstring ${OPTS},X))
# LDFLAGS += -lefence
endif
endif
# endif LINUX

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

# Define CPPCXXFLAGS after making all possible modifications to CPPFLAGS
# Add nco_c++ to directory search path
CXXCPPFLAGS := ${CPPFLAGS} -I${NCO_CXX_SRC_DIR}

# Define any remaining variables
libnco := ${MY_LIB_DIR}/libnco

# Default targets
all: dir lib ${MDL_BIN_TRG} data
non_ncap: dir lib $(filter-out ncap,${MDL_BIN_TRG}) data
# .PHONY tells make to remake the following non-file targets 
.PHONY: all cln dst_cln dbg ${MDL_BIN_TRG}
# Delete default suffixes---this should increase speed
.SUFFIXES: 
# Define suffixes which matter
.SUFFIXES: .cc .c .o .F .d
# Delete targets which were not successfully made
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
# Targets in bin
mpncbo:   ${MY_BIN_DIR}/mpncbo
${MY_BIN_DIR}/mpncbo:	${MY_OBJ_DIR}/mpncbo.o lib
	${MPICC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f mpncdiff; ln -s -f mpncbo mpncdiff
mpncecat:   ${MY_BIN_DIR}/mpncecat
${MY_BIN_DIR}/mpncecat:	${MY_OBJ_DIR}/mpncecat.o lib
	${MPICC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
mpncflint:   ${MY_BIN_DIR}/mpncflint
${MY_BIN_DIR}/mpncflint:	${MY_OBJ_DIR}/mpncflint.o lib
	${MPICC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
mpncpdq:   ${MY_BIN_DIR}/mpncpdq
${MY_BIN_DIR}/mpncpdq:	${MY_OBJ_DIR}/mpncpdq.o lib
	${MPICC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
#	cd ${MY_BIN_DIR}; rm -f mpncunpack; ln -s -f mpncpdq mpncunpack
#	cd ${MY_BIN_DIR}; rm -f mpncpack; ln -s -f mpncpdq mpncpack
mpncra:   ${MY_BIN_DIR}/mpncra
${MY_BIN_DIR}/mpncra:	${MY_OBJ_DIR}/mpncra.o lib
	${MPICC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f mpncea; ln -s -f mpncra mpncea
	cd ${MY_BIN_DIR}; rm -f mpncrcat; ln -s -f mpncra mpncrcat
mpncwa:   ${MY_BIN_DIR}/mpncwa
${MY_BIN_DIR}/mpncwa:	${MY_OBJ_DIR}/mpncwa.o ${MY_OBJ_DIR}/ncap_yacc.o ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o lib
	${MPICC} -o $@ $<  ${MY_OBJ_DIR}/ncap_yacc.o ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncatted:   ${MY_BIN_DIR}/ncatted
${MY_BIN_DIR}/ncatted:	${MY_OBJ_DIR}/ncatted.o lib
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncbo:   ${MY_BIN_DIR}/ncbo
${MY_BIN_DIR}/ncbo:	${MY_OBJ_DIR}/ncbo.o lib
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f ncdiff; ln -s -f ncbo ncdiff
# 20050710: -axW flag causes icc to die compiling ncecat 
ifeq (icc,$(firstword ${CC}))
ncecat : CFLAGS := $(filter-out -axW,${CFLAGS})
endif # endif Intel (Kai) C Compiler
ncecat:   ${MY_BIN_DIR}/ncecat
${MY_BIN_DIR}/ncecat:	${MY_OBJ_DIR}/ncecat.o lib
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncflint:   ${MY_BIN_DIR}/ncflint
${MY_BIN_DIR}/ncflint:	${MY_OBJ_DIR}/ncflint.o lib
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncks:   ${MY_BIN_DIR}/ncks
${MY_BIN_DIR}/ncks:	${MY_OBJ_DIR}/ncks.o lib
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncpdq:   ${MY_BIN_DIR}/ncpdq
${MY_BIN_DIR}/ncpdq:	${MY_OBJ_DIR}/ncpdq.o lib
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f ncunpack; ln -s -f ncpdq ncunpack
	cd ${MY_BIN_DIR}; rm -f ncpack; ln -s -f ncpdq ncpack
ncra:   ${MY_BIN_DIR}/ncra
${MY_BIN_DIR}/ncra:	${MY_OBJ_DIR}/ncra.o lib
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
	cd ${MY_BIN_DIR}; rm -f ncea; ln -s -f ncra ncea
	cd ${MY_BIN_DIR}; rm -f ncrcat; ln -s -f ncra ncrcat
ncrename:   ${MY_BIN_DIR}/ncrename
${MY_BIN_DIR}/ncrename:	${MY_OBJ_DIR}/ncrename.o lib
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
ncwa:   ${MY_BIN_DIR}/ncwa
${MY_BIN_DIR}/ncwa:	${MY_OBJ_DIR}/ncwa.o ${MY_OBJ_DIR}/ncap_yacc.o ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o lib
	${CC} -o $@ $<  ${MY_OBJ_DIR}/ncap_yacc.o ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap_utl.o ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
bin: ${MDL_BIN_TRG}
binclean: bin_cln
bin_cln: 
	rm -f ${MDL_BIN}
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
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
libnco_c++_tst:   ${MY_BLD_DIR}/libnco_c++_tst
${MY_BLD_DIR}/libnco_c++_tst:	${MY_BLD_DIR}/libnco_c++_tst.o lib
	${CXX} -o $@ $< ${LDFLAGS}
	chmod 755 $@${BNR_SFX}
rpm:	${MY_BLD_DIR}/nco.spec # Building rpm requires root priveleges, e.g., sudo make NCO_VRS=2.5.1 rpm
	cd ${DATA}/tmp; \
	/bin/rm -fr ${DATA}/tmp/nco*; \
	cp -r ${HOME}/nco nco-${NCO_VRS}; \
	tar cvzf nco-${NCO_VRS}.tar.gz ./nco-${NCO_VRS}; \
	mv ${DATA}/tmp/nco-${NCO_VRS}.tar.gz /usr/src/redhat/SOURCES; \
	/bin/rm -f /usr/src/redhat/SPECS/nco-${NCO_VRS}.spec; \
	ln -s ${HOME}/nco/bld/nco.spec /usr/src/redhat/SPECS/nco-${NCO_VRS}.spec; \
	cd /usr/src/redhat/SPECS; \
	rpm -ba nco-${NCO_VRS}.spec;
rpmnet:	${MY_BLD_DIR}/nco.spec # Building rpm requires root priveleges, e.g., sudo make NCO_VRS=2.5.1 rpmnet
	- if test -f /usr/src/redhat/SOURCES/nco-${NCO_VRS}.tar.gz; then printf "Using existing nco-${NCO_VRS}.tar.gz\n" ; else ${MY_BIN_DIR}/ncks -R -p ftp://dust.ess.uci.edu/pub/zender/nco -l /usr/src/redhat/SOURCES nco-${NCO_VRS}.tar.gz; fi
	/bin/rm -f /usr/src/redhat/SPECS/nco-${NCO_VRS}.spec; \
	cd /usr/src/redhat/SOURCES; \
	tar xvzf nco-${NCO_VRS}.tar.gz ./nco-${NCO_VRS}/bld/nco.spec; \
	mv ./nco-${NCO_VRS}/bld/nco.spec /usr/src/redhat/SPECS/nco.spec; \
	cd /usr/src/redhat/SPECS; \
	rpm -ba nco.spec; \
	scp /usr/src/redhat/SRPMS/nco-${NCO_VRS}-*.src.rpm /usr/src/redhat/RPMS/i386/nco-${NCO_VRS}-*.i386.rpm zender@dust.ess.uci.edu:/var/www/html/nco
	scp /usr/src/redhat/SRPMS/nco-${NCO_VRS}-*.src.rpm /usr/src/redhat/RPMS/i386/nco-${NCO_VRS}-*.i386.rpm zender@nco.sourceforge.net:/home/groups/ftp/pub/nco
	ncftpput upload.sourceforge.net /incoming /usr/src/redhat/SRPMS/nco-2.5.1-*.src.rpm /usr/src/redhat/RPMS/i386/nco-2.5.1-*.i386.rpm /usr/src/redhat/SOURCES/nco-2.5.1.tar.gz
rpm_cln:
	rpm --erase $(MDL_RPM_NST_NM)
# Targets in bm
bench:	bm
bnch:	bm
bm:	
	cd ${MY_BM_DIR}; \
	env MY_BIN_DIR=${MY_BIN_DIR} ../bm/nco_bm.pl --dbg=0 --bench --udpreport ${THR_NBR_SNG} ${MPI_PRC_SNG}
file:	fl
fl:	
	cd ${MY_BM_DIR}; \
	env MY_BIN_DIR=${MY_BIN_DIR} ../bm/nco_bm.pl --dbg=0 --test_files=A --udpreport ${THR_NBR_SNG} ${MPI_PRC_SNG}
regression:	tst
rgr:	tst
test:	tst
tst:	dat
	cd ${MY_BM_DIR}; \
	env MY_BIN_DIR=${MY_BIN_DIR} ../bm/nco_bm.pl --dbg=0 --regress --udpreport ${THR_NBR_SNG} ${MPI_PRC_SNG}
# Targets in dat
data:	dat
dat:	${MY_DAT_DIR}/in.nc
	-for fl in $(MDL_DAT_STB); do cd ${MY_DAT_DIR}; ln -s -f in.nc $$fl; done
${MY_DAT_DIR}/in.nc: ${MY_DAT_DIR}/in.cdl
	cd ${MY_DAT_DIR}; ncgen -b -o in.nc in.cdl
dataclean: dat_cln
dat_cln:
	rm -f $(MDL_DAT)
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
# Targets in lib
library: lib
lib : inc ${libnco}.a
ifeq (${PVM_ARCH},MACOSX)
	ranlib ${libnco}.a
endif # endif LINUX
# ${libnco}.a: ${libnco}.a(${MY_OBJ_DIR}/ncap_utl.o)
# If not using glibc, build Sittler's getopt() and getopt_long() functions
ifeq (${null},$(findstring ${PVM_ARCH},LINUXALPHALINUXAMD64LINUXARMFREEBSDWIN32))
${libnco}.a: ${libnco}.a(${MY_OBJ_DIR}/nco_getopt.o)
endif # endif not glibc
${libnco}.a: ${libnco}.a(${MY_OBJ_DIR}/nco_att_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_bnr.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cln_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnf_dmn.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnf_typ.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnv_arm.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_cnv_csm.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_ctl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_dbg.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_dmn_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_fl_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_lmt.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_lst_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_mmr.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_msa.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_mss_val.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_netcdf.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_omp.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_pck.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_prn.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_rec_var.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_rth_flt.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_rth_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_scl_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_scm.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_sng_utl.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_avg.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_lst.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_rth.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_scv.o) \
	${libnco}.a(${MY_OBJ_DIR}/nco_var_utl.o)
libclean: lib_cln
lib_cln:
	rm -f ${libnco}.a ${libnco}.*
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
	- cd ..; rm -f config.h.in config.h config.guess config.log config.status config.sub libtool Makefile stamp-h1; rm -r -f autom4te.cache conftest
# Targets in src
src: ${MDL_NCAP}
src_cln:
	rm -f ${MDL_NCAP}
# Targets in /usr/local
sys: ${MDL_BIN_TRG}
	- cd ${MY_BIN_DIR}; sudo /bin/cp -f ${MDL_BIN_TRG} /usr/local/bin; cd /usr/local/bin; sudo rm -f ncdiff ncea ncrcat ncunpack; sudo ln -s -f ncbo ncdiff; sudo ln -s -f ncra ncea; sudo ln -s -f ncra ncrcat; sudo ln -s -f ncpdq ncunpack; sudo ln -s -f ncpdq ncpack
sys_cln:
	- cd /usr/local/bin;rm -f ${MDL_BIN_TRG}

# Housekeeping
clean: cln
cln: lib_cln dpn_cln obj_cln src_cln tst_cln
	cd ${MY_DOC_DIR}; rm -f nco.aux nco.cp nco.cps nco.fn nco.ky nco.log nco.pg nco.toc nco.tp nco.vr 
debug: dbg
dbg:
	@printf "ABI = ${ABI}\n"
	@printf "AR = ${AR}\n"
	@printf "BNR_SFX = ${BNR_SFX}\n"
	@printf "CC = ${CC}\n"
	@printf "CFLAGS = ${CFLAGS}\n"
	@printf "CPP = ${CPP}\n"
	@printf "CPPFLAGS = ${CPPFLAGS}\n"
	@printf "CPP_PTH = ${CPP_PTH}\n"
	@printf "CPP_TKN = ${CPP_TKN}\n"
	@printf "CXX = ${CXX}\n"
	@printf "CXXFLAGS = ${CXXFLAGS}\n"
	@printf "DAP = ${DAP}\n"
	@printf "DAP_ROOT = ${DAP_ROOT}\n"
	@printf "DBG = ${DBG}\n"
	@printf "FC = ${FC}\n"
	@printf "FFLAGS = ${FFLAGS}\n"
	@printf "GCC_RCH_ARG = ${GCC_RCH_ARG}\n"
	@printf "HDF5 = ${HDF5}\n"
	@printf "HOST = ${HOST}\n"
	@printf "ICC_RCH_ARG = ${ICC_RCH_ARG}\n"
	@printf "LAMMPICC = ${LAMMPICC}\n"
	@printf "LAMMPICXX = ${LAMMPICXX}\n"
	@printf "LDFLAGS = ${LDFLAGS}\n"
	@printf "LEX = $(LEX)\n"
	@printf "LINUX_CC = $(LINUX_CC)\n"
	@printf "LINUX_CXX = $(LINUX_CXX)\n"
	@printf "MDL_BIN_TRG = ${MDL_BIN_TRG}\n"
	@printf "MDL_DPN = ${MDL_DPN}\n"
	@printf "MDL_MPI_TRG = ${MDL_MPI_TRG}\n"
	@printf "MDL_OBJ = ${MDL_OBJ}\n"
	@printf "MDL_PTH = ${MDL_PTH}\n"
	@printf "MDL_RPM_NST_NM = $(MDL_RPM_NST_NM)\n"
	@printf "MDL_SRC = ${MDL_SRC}\n"
	@printf "MPICC = ${MPICC}\n"
	@printf "MPICXX = ${MPICXX}\n"
	@printf "MPICH_CC = ${MPICH_CC}\n"
	@printf "MPICH_CXX = ${MPICH_CXX}\n"
	@printf "MPI_PRC = ${MPI_PRC}\n"
	@printf "MPI_PRC_SNG = ${MPI_PRC_SNG}\n"
	@printf "NCO_VRS = ${NCO_VRS}\n"
	@printf "NC_LDFLAGS = $(NC_LDFLAGS)\n"
	@printf "NC_LIBS = $(NC_LIBS)\n"
	@printf "NETCDF_INC = ${NETCDF_INC}\n"
	@printf "NETCDF_LIB = ${NETCDF_LIB}\n"
	@printf "OMP = ${OMP}\n"
	@printf "PVM_ARCH = ${PVM_ARCH}\n"
	@printf "RPM = ${RPM}\n"
	@printf "SRC_LST = $(SRC_LST)\n"
	@printf "THR_NBR = ${THR_NBR}\n"
	@printf "THR_NBR_SNG = ${THR_NBR_SNG}\n"
	@printf "TMP_LDFLAGS = $(TMP_LDFLAGS)\n"
	@printf "TMP_LIBS = $(TMP_LIBS)\n"
	@printf "UDUNITS = $(UDUNITS)\n"
	@printf "UDUNITS_INC = $(UDUNITS_INC)\n"
	@printf "UDUNITS_LIB = $(UDUNITS_LIB)\n"
	@printf "USE_FORTRAN_ARITHMETIC = $(USE_FORTRAN_ARITHMETIC)\n"
	@printf "VPATH = ${VPATH}\n"
	@printf "VRS_SNG = ${VRS_SNG}\n"
	@printf "YACC = ${YACC}\n"
distclean: dst_cln
dst_cln: cln bin_cln dat_cln doc_cln root_cln
	cd ${MY_BLD_DIR}; rm -f *~
	cd ${MY_DAT_DIR}; rm -f in.nc *~
tags: 
	etags ${MY_SRC_DIR}/*.h ${SRC_LST} ${MDL_DOC_SRC} ${MDL_BLD_SRC} ${MDL_NCAP_SRC}

# Reset internal YACC and LEX patterns
%.c : %.y
%.c : %.l

# It is safest to do both YACC and LEX after either file changes
# Otherwise only changing one and then switching, e.g., from bison to yacc, can cause problems
# NB: Bison has a problem when bison.simple declares yyparse() as int yyparse (void);
# The solution is to comment out that definition in bison.simple
ifeq (${YACC},bison)
${MY_SRC_DIR}/%_yacc.c ${MY_SRC_DIR}/%_yacc.h : ${MY_SRC_DIR}/%_yacc.y
	${YACC} --output=$(basename $<).c -d $<
#${MY_SRC_DIR}/%_yacc.c ${MY_SRC_DIR}/%_yacc.h : ${MY_SRC_DIR}/%_yacc.y
#	${YACC} --file-prefix=$(notdir $($(basename $<))) -d $<
#%_yacc.c %_yacc.h : %_yacc.y
#	${YACC} --file-prefix=$(notdir $($(basename $<))) -d $<
endif
ifeq (${YACC},yacc)
%.tab.c %.tab.h : %.y
	${YACC} -d $<
	mv y.tab.c $(basename $<).c
	mv y.tab.h $(basename $<).h
endif

%_lex.c : %_lex.l
	$(LEX) $<
	mv lex.yy.c $(basename $<).c

# ncap, and only ncap, requires AIX system C++ library
ifneq (${null},$(findstring AIX,${PVM_ARCH}))
ifneq (${null},$(findstring xl,${CC}))
# Target-specific variable values syntax TARGET ... : VARIABLE-ASSIGNMENT
# Rules begin in leftmost column else interpreted as commands
ncap : LDFLAGS += -lC
endif # endif AIX VA Compiler Collection
endif # endif AIX
# For some reason, ncap_lex.c is not remade when I expect it to be, so I explicitly remove the object file every time
ncap:	${MY_OBJ_DIR}/ncap_yacc.o ${MY_OBJ_DIR}/ncap_lex.o ncap_yacc.h ${MY_OBJ_DIR}/ncap.o ${MY_OBJ_DIR}/ncap_utl.o ${libnco}.a
#        ${YACC} --name-prefix=nco_
ifneq (${null},$(findstring ${PVM_ARCH},LINUXALPHALINUXAMD64LINUXARMFREEBSDWIN32))
	${CC} ${CFLAGS} -o ${MY_BIN_DIR}/$@ ${MY_OBJ_DIR}/$@.o ${MY_OBJ_DIR}/ncap_utl.o ${MY_OBJ_DIR}/$@_yacc.o ${MY_OBJ_DIR}/$@_lex.o ${LDFLAGS}
else	
	${CC} ${CFLAGS} -o ${MY_BIN_DIR}/$@ ${MY_OBJ_DIR}/$@.o ${MY_OBJ_DIR}/ncap_utl.o ${MY_OBJ_DIR}/$@_yacc.o ${MY_OBJ_DIR}/$@_lex.o -ll -ly ${LDFLAGS}
endif
	chmod 755 ${MY_BIN_DIR}/$@${BNR_SFX}
#	/bin/rm -f ${MDL_NCAP} ${MY_OBJ_DIR}/$@_lex.o ${MY_OBJ_DIR}/ncap_yacc.h ${MY_OBJ_DIR}/ncap_yacc.c

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
