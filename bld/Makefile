# $Header: /data/zender/nco_20150216/nco/bld/Makefile,v 1.103 2001-10-26 07:02:58 zender Exp $

# Purpose: GNUMakefile for NCO module nco
# Requires GNU make. AT&T make will not process this file.

# Usage: 
# make
# If this fails, try
# make dir all
# Clean all dependencies for fresh build
# cd ~/nco/bld;make cln bin_cln;cd -
# Print make diagnostics
# cd ~/nco/bld;make dbg;cd -
# Enable symbols for debugging
# cd ~/nco/bld;make OPTS=D;cd -
# DODS support:
# cd ~/nco/bld;make DODS=Y;cd -
# HDF5 version of netCDF libraries:
# cd ~/nco/bld;make cln bin_cln all HDF5=Y;cd -

# Top-level tokens defining directory structure
# These tokens may be over-ridden by environment variables or when invoking make, e.g. make DODS=Y
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
ifndef MY_OBJ_DIR
 MY_OBJ_DIR := ../obj
endif
MY_DAT_DIR := ../data
MY_DOC_DIR := ../doc
MY_INC_DIR := 
MY_DPN_DIR := ${MY_OBJ_DIR}
MY_SRC_DIR := ../src/nco

# Primary tokens which determine build options
# Specify non-default when invoking make, e.g. make DODS=Y
ifndef DODS
 DODS := N
endif
ifndef HDF5
 HDF5 := N
endif # endif HDF5
ifndef I18N
 I18N := N
endif
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
ifndef OPTS
 OPTS := O
endif
ifndef USE_FORTRAN_ARITHMETIC
# Whether or not to use Fortran arithmetic routines
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
CPP_TKN = ${USR_TKN} -D${PVM_ARCH} -DNO_NETCDF_2 -DVERSION='${VRS_SNG}' -DHOSTNAME='${HOST}' -DUSER='${USER}'

ifeq ($(DODS),Y)
# Build NCO as DODS-enabled clients
ifndef DODS_ROOT
# Directory containing libdap++.a, libnc-dods.a
 DODS_ROOT := /usr/local/DODS
endif
 NC_LDFLAGS := -L$(DODS_ROOT)/lib -L$(DODS_ROOT)/packages-3.1.1/lib
 NC_LIBS := -lnc-dods -ldap++ -lnc-dods -ldap++ -lwww -lz -lrx
# Get netCDF from DODS version
 NETCDF_INC=$(DODS_ROOT)/include
 NETCDF_LIB=$(DODS_ROOT)/lib
else
# Build NCO straight, without DODS
 NC_LDFLAGS := -L${NETCDF_LIB}
 NC_LIBS := -lnetcdf
endif # end DODS

ifeq ($(HDF5),Y)
# Build NCO using HDF5 netCDF interface
 HDF5_ROOT := /usr/local/hdf5
 NETCDF_INC := $(HDF5_ROOT)/include # Directory containing HDF5 version of netcdf.h
 NETCDF_LIB := $(HDF5_ROOT)/lib -lmfhdf -ldf -ljpeg -lz # Directory containing HDF5 version of libnetcdf.a routines, order of library linkage is significant
endif # endif HDF5

# Internationalize NCO with i18n features
ifeq ($(I18N),Y)
 MY_SHR_DIR := ${HOME}/share
 MY_ES_DIR := ${MY_SHR_DIR}/locale/es/LC_MESSAGES
 MY_FR_DIR := ${MY_SHR_DIR}/locale/fr/LC_MESSAGES
endif

ifeq (${PVM_ARCH},LINUX)
# rpm command, and thus RPM variables, may only be assumed to be available in Linux
 MDL_RPM_NST_NM := $(shell rpm -qa | grep nco-) # Name of installed package
# MDL_RPM_PRV_NM := $(shell rpm -qp foo) # Name of package provided by specified RPM
endif
# endif LINUX

# TMP_* and NCO_* are required to play nicely with DODS flags
NCO_LDFLAGS := -L${MY_LIB_DIR}
NCO_LIBS := -lnco
TMP_LDFLAGS := $(NCO_LDFLAGS) $(NC_LDFLAGS)
TMP_LIBS := $(NCO_LIBS) $(NC_LIBS)

# NB: Do NOT add comment lines, e.g., # This is a comma, to character definitions
null :=
space := ${null} ${null}
comma := ,
newline := \n
# We replace `/' by `cszzsc', call perl, then replace `cszzxc' by `/'
# Unique character(s) to substitute for ${slash} before passing to perl regex
slash_rx := cszzsc
# Unique character(s) to replace by ${slash_rx} before passing to perl regex
slash := /
MY_OBJ_DIR_RX := $(subst ${slash},${slash_rx},${MY_OBJ_DIR})
MY_DPN_DIR_RX := $(subst ${slash},${slash_rx},${MY_DPN_DIR})

# Directories to search for source files
MDL_PTH := ${MY_SRC_DIR}
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
MDL_BIN_TRG := ncks ncrename ncra ncdiff ncwa ncecat ncflint ncatted # NCO binary targets
MDL_BIN_SYM_LNK := ncea ncrcat # Symbolic links
MDL_BIN_STB := ${MDL_BIN_TRG} ${MDL_BIN_SYM_LNK} # All NCO files in MY_BIN_DIR
MDL_BIN := $(addprefix ${MY_BIN_DIR}/,${MDL_BIN_STB}) # distclean removes these files

# Variables having to do with NCO data
MDL_DAT_STB := 85 86 87 88 89 h0001 h0002 h0003 # Symbolic links to in.nc
MDL_DAT_STB := $(addsuffix .nc,$(MDL_DAT_STB)) # `make data' creates these files
MDL_DAT := $(addprefix ${MY_DAT_DIR}/,$(MDL_DAT_STB)) # `make distclean' removes these files

# Variables having to do with NCO documentation
MDL_DOC_SRC := $(addprefix ${MY_DOC_DIR}/,nco.texi MANIFEST README NEWS TODO index.shtml ncap.txt nco_news.shtml) # `make tags' includes these files
MDL_DOC_TRG := nco.dvi nco.ps nco.pdf nco.html nco.info # `make doc' creates these files
MDL_DOC := $(addprefix ${MY_DOC_DIR}/,$(MDL_DOC_TRG)) # `make distclean' removes these files

# Variables having to do with NCO build
MDL_BLD_SRC := $(addprefix ${MY_BLD_DIR}/,Makefile nco_dst.pl nco_tst.sh) # `make tags' includes these files

# Variables having to do with ncap
MDL_NCAP_SRC := $(addprefix ${MY_SRC_DIR}/,ncap.y ncap.l ncap.h) # `make tags' includes these files
MDL_NCAP_TRG := ncap.tab.c ncap.tab.h ncap_lex.c # `make ncap' creates these files
MDL_NCAP := $(addprefix ${MY_SRC_DIR}/,$(MDL_NCAP_TRG)) # `make distclean' removes these files

# Redefine default C and C++ pattern rules
${MY_OBJ_DIR}/%.o : %.c
	${CC} ${CPPFLAGS} ${CFLAGS} -c $< -o ${MY_OBJ_DIR}/$(notdir $@)
${MY_OBJ_DIR}/%.o : %.cc
	${C++} ${CPPFLAGS} ${C++FLAGS} -c $< -o ${MY_OBJ_DIR}/$(notdir $@)

# Default Fortran pattern rules: CRAY and RS6K must override these rules
${MY_OBJ_DIR}/%.o : %.F
	${FC} ${CPPFLAGS} -c ${FFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
${MY_OBJ_DIR}/%.o : %.f
	${FC} -c ${FFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<

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
#	${CPP} -M ${CPPFLAGS} $< | perl -p -e 's/$*\.F\.o/$*.o $@/g;' > $@
# Following command makes, e.g., f.d begin "/home/zender/obj/LINUX/f.o f.d : f.F ..."
# This works fairly well, but is a hack
# First pattern substitutes MY_OBJ_DIR_RX, which has placeholders for slashes
# Second pattern substitutes slashes for the placeholders
	${CPP} -M ${CPPFLAGS} $< | perl -p -e 's/$*(\.F)?\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' > $@
# Following command makes, e.g., f.d begin "${MY_OBJ_DIR}/f.o f.d : f.F ..."
# This would be the ideal command but I can't get the dollar sign to show up
#	${CPP} -M ${CPPFLAGS} $< | perl -p -e 's/$*\.F\.o/\${dollar}MY_OBJ_DIR\/$*.o $@/g;' > $@

${MY_DPN_DIR}/%.d : %.c
#	${CPP} -M ${CPPFLAGS} $< | perl -p -e 's/$*\.o/$*.o $@/g;' > $@
	${CPP} -M ${CPPFLAGS} $< | perl -p -e 's/$*\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' > $@

${MY_DPN_DIR}/%.d : %.cc
#	${CPP} -M ${CPPFLAGS} $< | perl -p -e 's/$*\.o/$*.o $@/g;' > $@
	${CPP} -M ${CPPFLAGS} $< | perl -p -e 's/$*\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' > $@

# The first LDFLAGS is for typical C programs with netCDF, math, and networking
# The second LDFLAGS enables C/Fortran linking

# Works on AIX and AIX46K
ifneq (,$(findstring AIX,${PVM_ARCH}))
C++ := xlC
CC := cc
# Additional switch to fix compiler warnings on csz.c
# -qmaxmem=num Limit the amount of memory used by space intensive optimizations to <num> kilobytes
CFLAGS := -qmaxmem=8192
CPP := cc -c
CPPFLAGS := ${CPP_TKN} ${CPP_PTH} -I${NETCDF_INC}
FC := xlf
FFLAGS := -qfixed=132
LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
#LDFLAGS += -lxlf90 -lxlf
endif
LEX := lex
LINT := lint
YACC := yacc
ifeq (${OPTS},O)
 CFLAGS += -O
# CPP := ${CPP} ${CPPFLAGS}
# PREPROCESS.F := ${CPP} ${CPPFLAGS}
 FFLAGS += -O -NS2000 -qfixed=132
endif
ifeq (${OPTS},D)
 CFLAGS += -g
# CPP := ${CPP} ${CPPFLAGS}
# PREPROCESS.F := ${CPP} ${CPPFLAGS}
 FFLAGS := -g -NS2000 -qfixed=132
endif
# Additional flags for AIX:
# -M Generate information to be included in a "make" description file; output goes to .u file
# -c Do not send object files to the linkage editor
# -P Preprocess but do not compile; output goes to .i file
# Using -P causes additional warning messages about lm 
# Not using -P causes *.o files to be created twice
${MY_DPN_DIR}/%.d : %.c
	${CPP} -M ${CPPFLAGS} $< ;perl -p -e 's/$*\.o/${MY_OBJ_DIR_RX}\/$*.o ${MY_DPN_DIR_RX}\/$(notdir $@)/g;s/${slash_rx}/\${slash}/g' $*.u > $@ ; \
	rm -f $*.i $*.o $*.u;
endif
# endif AIX

ifeq (${PVM_ARCH},ALPHA)
C++ := cxx
CC := cc
CPP := cpp
CPPFLAGS := ${CPP_PTH} -I${NETCDF_INC}
FC := f90
FFLAGS := -r8 -i4 -c -omp -automatic
FIXEDFLAGS := -extend_source -omp -automatic
FREEFLAGS := -DHIDE_SHR_MSG -free
LD := ld
LDFLAGS += -omp $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS +=
endif
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS := -O2
 FFLAGS += -O3 -strict
endif
ifeq (${OPTS},D)
 CFLAGS := -g
 FFLAGS += -g
endif
${MY_OBJ_DIR}/%.o : %.F90
	${FC} -c ${FREEFLAGS} ${FFLAGS} ${CPPFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
${MY_OBJ_DIR}/%.o : %.F
	${FC} -c ${FIXEDFLAGS} ${FFLAGS} ${CPPFLAGS} -o ${MY_OBJ_DIR}/$(notdir $@) $<
endif
# endif ALPHA

ifeq (${PVM_ARCH},CRAY)
C++ := g++
CC := cc
CPP := cpp
CPPFLAGS := ${CPP_TKN} -I${NETCDF_INC}
FC := f90
LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -L/lib -lf
endif
LEX := lex
LINT := lint
YACC := yacc
ifeq (${OPTS},O)
 CFLAGS += -O
 FFLAGS = -N 132
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS = -g -N 132
endif
ifeq (${OPTS},X)
 CFLAGS += -g -N 132
 FFLAGS = -g -N 132 -e i
endif
# 97/10/21 Added -P to suppress #line # directives on Fortran files
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

# Works on LINUX and LINUXALPHA
ifneq (,$(findstring LINUX,${PVM_ARCH}))
C++ := g++
# 19980517: As of gcc-2.8.0 using -ansi switch breaks because resolv.h structure have changed
#CC := gcc -ansi
CC := gcc
# NB: nameser.h needs -Di386, but gcc is sending -Di586 (on pentiums)
CPPFLAGS := ${CPP_TKN} -Di386 -I${NETCDF_INC}
FC := g77
 LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS +=
endif
LEX := flex
LINT := lint
YACC := bison
ifeq (${CC},gcc)
 CFLAGS := -Wall
# Compilation flags for numerical routines recommended by GSL 0.9 manual, p. 370
# CFLAGS += -ansi -Werror -W -Wmissing-prototypes -Wconversion -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fshort-enums -fno-common -Dinline= -g -O4
# Compilation flags recommended by GSL that I do not like
# -Waggregate-return: Warn if functions return aggregates like structures or unions
# -pedantic: Disallow non ISO constructs, including type long long
# -Wstrict-prototypes: C only?
# -Wtraditional: C only?
# -Wnested-externs: C only?
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
endif
 C++FLAGS := ${CFLAGS}
endif
ifeq (${FC},pgf90)
 FFLAGS := -Mextend -Mnosecond_underscore -mp -byteswapio -Mrecursive -Mdalign -Ktrap=fp
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
endif
ifeq (${FC},g77)
 FFLAGS := -ffixed-line-length-132 -fno-second-underscore
ifeq (${OPTS},O)
 FFLAGS += -O
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
endif
endif
# endif LINUX

ifeq (${PVM_ARCH},RS6K)
C++ := g++
CC := gcc -ansi
CPP := /lib/cpp -P
CPPFLAGS := ${CPP_TKN} -I${NETCDF_INC}
FC := xlf
LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -lxlf90 -lxlf
endif
LEX := lex
LINT := lint
YACC := yacc
ifeq (${OPTS},O)
 CFLAGS += -O2
 CPP := ${CPP} ${CPPFLAGS}
 PREPROCESS.F := ${CPP} ${CPPFLAGS}
 FFLAGS := -O -NS2000 -qfixed=132
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
ifneq (,$(findstring SGI,${PVM_ARCH}))
C++ := CC -LANG:std
CC := cc
# 20000302: -w suppresses warnings which will swamp linker
#C++ := g++ -w
#CC := gcc
CPPFLAGS := ${CPP_TKN} -I${NETCDF_INC}
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
SGI_ABI_FLG := -n32 -mips3
# endif SGI6
else
# SGI64, SGIMP64
GCC_ABI_FLG := -mabi=64
GCC_LDFLAGS_SZ_SPC := ${GCC_ABI_FLG} -mips4 -L/usr/local/lib/mabi=64
SGI_ABI_FLG := -64 -mips4 -mp -mpio
# endif SGI64, SGIMP64
endif
ifeq (${CC},gcc)
 LDFLAGS += $(GCC_LDFLAGS_SZ_SPC) $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq (${OPTS},O)
 CFLAGS += ${GCC_ABI_FLG} -O2 -Wall
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
 C++FLAGS := ${CFLAGS}
endif
# endif CC=gcc
ifeq (${CC},cc)
 LDFLAGS += ${SGI_ABI_FLG} $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq (${OPTS},O)
 CFLAGS += ${SGI_ABI_FLG} -O2
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
 FFLAGS := ${SGI_ABI_FLG} -O2 -extend_source
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

ifeq (${PVM_ARCH},SUN4)
C++ := g++
CC := acc
CPPFLAGS := ${CPP_TKN} -I${NETCDF_INC}
FC := f77
LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -cg92 -L/opt/SUNWspro/SC3.0/lib/cg92 -lF77 -lM77
endif
LDFLAGS += -lresolv
LEX := lex
LINT := lint
YACC := yacc
ifeq (${OPTS},O)
 CFLAGS += -O2
 FFLAGS := -fast -e -Nl99
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS := -g -e -Nl99
endif
endif
# endif SUN4

ifeq (${PVM_ARCH},SUN4SOL2)
C++ := g++
CC := gcc -ansi
CPPFLAGS := ${CPP_TKN} -I${NETCDF_INC}
FC := f77
LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lsocket -lnsl -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -lF77 -lM77
endif
LDFLAGS += -lresolv
LEX := lex
LINT := lint
YACC := yacc
ifeq (${OPTS},O)
 CFLAGS += -O2
 FFLAGS := -O -e
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS := -g -e
endif
ifeq (${OPTS},X)
 CFLAGS += -g 
 FFLAGS := -g -e
# NB: 1998/06/01 -C (range-checking) is not supported by Sun f90
ifeq (${FC},f77)
 FFLAGS += ${FFLAGS} -C
endif
endif
endif
# endif SUN4SOL2

ifeq (${PVM_ARCH},SUNMP)
C++ := g++
#CC := gcc -ansi -pedantic
CC := gcc 
CPPFLAGS := ${CPP_TKN} -I${NETCDF_INC}
FC := f77
LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lsunmath -lthread -lsocket -lnsl -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS += -lF77 -lM77
endif
LDFLAGS += -lresolv
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -O2
 FFLAGS := -fast -e
endif
ifeq (${OPTS},D)
 CFLAGS += -g
 FFLAGS := -g -e
endif
ifeq (${OPTS},X)
 CFLAGS += -g 
 FFLAGS := -g -e
# NB: 1998/06/01 -C (range-checking) is not supported by Sun f90
ifeq (${FC},f77)
 FFLAGS += ${FFLAGS} -C
endif
endif
endif
# endif SUNMP

ifeq (${PVM_ARCH},WIN32)
C++ := g++
CC := gcc -ansi
# NB: nameser.h needs -Di386, but gcc is sending -Di586 (on pentiums)
CPPFLAGS := ${CPP_TKN} -Di386 -I${NETCDF_INC}
FC := g77
LD := ld
LDFLAGS += $(TMP_LDFLAGS) $(TMP_LIBS) -lm
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
 LDFLAGS +=
endif
LEX := flex
LINT := lint
YACC := bison
ifeq (${OPTS},O)
 CFLAGS += -O
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

# Link to DODS libraries first, see DODS User's Guide
# Currently this is invoked by using 'make DODS=Y' on the command
# NB: Yes, nc-dods, dap++ must be linked twice!
ifeq ($(DODS),Y)
ifneq (${CC},g++)
# -lstdc++ automatically linked by g++, must add by hand for others
 LDFLAGS := ${LDFLAGS} -lstdc++
endif
endif
# endif DODS

# Internationalize NCO with i18n features
ifeq ($(I18N),Y)
 CPPFLAGS += -DI18N
ifneq (${null},$(findstring SGI,${PVM_ARCH}))
 LDFLAGS += -lintl
endif
endif
# endif I18N

ifeq ($(HDF5),Y)
 CPPFLAGS += -DHDF5
# No need to link to libnetcdf.a since libmfhdf.a has all netCDF3 calls
 LDFLAGS := $(filter-out -lnetcdf,${LDFLAGS})
endif
# endif HDF5

# Link to Electric Fence library, see man efence
ifeq (${PVM_ARCH},LINUX)
ifneq (,$(findstring ${OPTS},DX))
 LDFLAGS += -lefence
endif
endif
# endif LINUX

# Disable OpenMP on platforms that automatically support it
ifeq (${OMP},N)
ifneq (,$(findstring SGI,${PVM_ARCH}))
 CFLAGS := $(filter-out -mp -mpio,${CFLAGS})
 LDFLAGS := $(filter-out -mp -mpio,${LDFLAGS})
endif # endif SGI
 CPPFLAGS += -U_OPENMP
endif # endif OMP

# Define any remaining variables
libnco := ${MY_LIB_DIR}/libnco

# Default targets
all: dir lib ${MDL_BIN_TRG} data
# .PHONY tells make to remake the following non-file targets 
.PHONY: all cln dst_cln dbg 
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
.PHONY:  ncra ncatted ncks ncwa ncecat ncdiff ncflint ncrename
ncra:   ${MY_BIN_DIR}/ncra
${MY_BIN_DIR}/ncra:	${MY_OBJ_DIR}/ncra.o $(libnco).a
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@
	cd ${MY_BIN_DIR}; rm -f ncea; ln -f -s ncra ncea
	cd ${MY_BIN_DIR}; rm -f ncrcat; ln -f -s ncra ncrcat
ncatted:   ${MY_BIN_DIR}/ncatted
${MY_BIN_DIR}/ncatted:	${MY_OBJ_DIR}/ncatted.o $(libnco).a
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@
ncks:   ${MY_BIN_DIR}/ncks
${MY_BIN_DIR}/ncks:	${MY_OBJ_DIR}/ncks.o $(libnco).a
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@
ncwa:   ${MY_BIN_DIR}/ncwa
${MY_BIN_DIR}/ncwa:	${MY_OBJ_DIR}/ncwa.o $(libnco).a
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@
ncecat:   ${MY_BIN_DIR}/ncecat
${MY_BIN_DIR}/ncecat:	${MY_OBJ_DIR}/ncecat.o $(libnco).a
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@
ncdiff:   ${MY_BIN_DIR}/ncdiff
${MY_BIN_DIR}/ncdiff:	${MY_OBJ_DIR}/ncdiff.o $(libnco).a
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@
ncflint:   ${MY_BIN_DIR}/ncflint
${MY_BIN_DIR}/ncflint:	${MY_OBJ_DIR}/ncflint.o $(libnco).a
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@
ncrename:   ${MY_BIN_DIR}/ncrename
${MY_BIN_DIR}/ncrename:	${MY_OBJ_DIR}/ncrename.o $(libnco).a
	${CC} -o $@ $< ${LDFLAGS}
	chmod 755 $@
bin: ${MDL_BIN_TRG}
binclean: bin_cln
bin_cln: 
	rm -f ${MDL_BIN}
# Targets in bld
buildclean: bld_cln
bld_cln:
	cd ${MY_BLD_DIR}; rm -f TAGS
rpm:	${MY_BLD_DIR}/nco.spec # Building rpm requires root priveleges, e.g., sudo make NCO_VRS=2.0.0 rpm
	cd ${DATA}/tmp; \
	/bin/rm -fr ${DATA}/tmp/nco*; \
	cp -r ${HOME}/nco nco-$(NCO_VRS); \
	tar cvzf nco-$(NCO_VRS).tar.gz ./nco-$(NCO_VRS); \
	mv ${DATA}/tmp/nco-$(NCO_VRS).tar.gz /usr/src/redhat/SOURCES; \
	/bin/rm -f /usr/src/redhat/SPECS/nco-$(NCO_VRS).spec; \
	ln -s ${HOME}/nco/bld/nco.spec /usr/src/redhat/SPECS/nco-$(NCO_VRS).spec; \
	cd /usr/src/redhat/SPECS; \
	rpm -ba nco-$(NCO_VRS).spec;
rpmnet:	${MY_BLD_DIR}/nco.spec # Building rpm requires root priveleges, e.g., sudo make NCO_VRS=2.0.0 rpmnet
	- if test -f /usr/src/redhat/SOURCES/nco-$(NCO_VRS).tar.gz; then printf "Using existing nco-$(NCO_VRS).tar.gz\n" ; else ${MY_BIN_DIR}/ncks -R -p ftp://ftp.cgd.ucar.edu/pub/zender/nco -l /usr/src/redhat/SOURCES nco-$(NCO_VRS).tar.gz; fi
	/bin/rm -f /usr/src/redhat/SPECS/nco-$(NCO_VRS).spec; \
	cd /usr/src/redhat/SOURCES; \
	tar xvzf nco-$(NCO_VRS).tar.gz ./nco-$(NCO_VRS)/bld/nco.spec; \
	mv ./nco-$(NCO_VRS)/bld/nco.spec /usr/src/redhat/SPECS/nco.spec; \
	cd /usr/src/redhat/SPECS; \
	rpm -ba nco.spec; \
	scp /usr/src/redhat/SRPMS/nco-$(NCO_VRS)-*.src.rpm /usr/src/redhat/RPMS/i386/nco-$(NCO_VRS)-*.i386.rpm zender@ftp.cgd.ucar.edu:/ftp/pub/zender/nco
	scp /usr/src/redhat/SRPMS/nco-$(NCO_VRS)-*.src.rpm /usr/src/redhat/RPMS/i386/nco-$(NCO_VRS)-*.i386.rpm zender@nco.sourceforge.net:/home/groups/ftp/pub/nco
	ncftpput upload.sourceforge.net /incoming /usr/src/redhat/SRPMS/nco-2.0.0-*.src.rpm /usr/src/redhat/RPMS/i386/nco-2.0.0-*.i386.rpm /usr/src/redhat/SOURCES/nco-2.0.0.tar.gz
rpm_cln:
	rpm --erase $(MDL_RPM_NST_NM)
# Targets in dat
data:	dat
dat:	${MY_DAT_DIR}/in.nc 
	-for fl in $(MDL_DAT_STB); do cd ${MY_DAT_DIR}; ln -f -s in.nc $$fl; done
${MY_DAT_DIR}/in.nc: ${MY_DAT_DIR}/in.cdl
	cd ${MY_DAT_DIR}; ncgen -b -o in.nc in.cdl
dataclean: dat_cln
dat_cln:
	rm -f $(MDL_DAT)
	cd ${MY_DAT_DIR}; rm -f foo*
test:	tst
tst:	${MY_DAT_DIR}/in.nc
	- if test -f ${MY_DAT_DIR}/nco_tst.nc; then printf "" ; else ${MY_BIN_DIR}/ncks -R -p ftp://ftp.cgd.ucar.edu/pub/zender/nco -l ${MY_DAT_DIR} nco_tst.nc; fi
	cd ${MY_BLD_DIR}; nco_tst.sh
testclean: tst_cln
tst_cln:
	cd ${MY_DAT_DIR}; rm -f foo* nco_tst.nc
# Targets in doc
# Each make directive line spawns a separate shell so must use `cd dir;cmd' format
doc:	${MY_DOC_DIR}/nco.dvi ${MY_DOC_DIR}/nco.ps ${MY_DOC_DIR}/nco.pdf ${MY_DOC_DIR}/nco.html ${MY_DOC_DIR}/nco.info   
${MY_DOC_DIR}/nco.ps: ${MY_DOC_DIR}/nco.dvi
	cd ${MY_DOC_DIR}; dvips -o nco.ps nco.dvi
${MY_DOC_DIR}/nco.dvi: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; texi2dvi nco.texi
${MY_DOC_DIR}/nco.info: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; makeinfo nco.texi
${MY_DOC_DIR}/nco.html: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; texi2html -monolithic -verbose nco.texi
${MY_DOC_DIR}/nco.pdf: ${MY_DOC_DIR}/nco.texi
	cd ${MY_DOC_DIR}; texi2dvi --pdf nco.texi
#	cd ${MY_DOC_DIR}; ps2pdf -dMaxSubsetPct=100 -dCompatibilityLevel=1.2 -dSubsetFonts=true -dEmbedAllFonts=true nco.ps nco.pdf
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
lib: inc $(libnco).a
ifeq ($(USE_FORTRAN_ARITHMETIC),Y)
$(libnco).a: $(libnco).a(${MY_OBJ_DIR}/nc_utl.o) \
	$(libnco).a(${MY_OBJ_DIR}/nc_fortran.o) \
	$(libnco).a(${MY_OBJ_DIR}/cal_util.o) \
	$(libnco).a(${MY_OBJ_DIR}/csz.o)
else
$(libnco).a: $(libnco).a(${MY_OBJ_DIR}/nc_utl.o) \
	$(libnco).a(${MY_OBJ_DIR}/nco_netcdf.o) \
	$(libnco).a(${MY_OBJ_DIR}/csz.o) \
	$(libnco).a(${MY_OBJ_DIR}/pck.o)
endif
libclean: lib_cln
lib_cln:
	rm -f $(libnco).a
# Targets in obj
object: obj
obj: ${MDL_OBJ}
objclean: obj_cln
obj_cln:
	rm -f ${MDL_OBJ}
# Targets in src
src: $(MDL_NCAP)
src_cln:
	rm -f $(MDL_NCAP)

# Housekeeping
clean: cln
cln: lib_cln dpn_cln obj_cln tst_cln
	cd ${MY_DOC_DIR}; rm -f nco.aux nco.cp nco.cps nco.fn nco.ky nco.log nco.pg nco.toc nco.tp nco.vr 
debug: dbg
dbg:
	@printf "C++ = ${C++}\n"
	@printf "C++FLAGS = ${C++FLAGS}\n"
	@printf "CC = ${CC}\n"
	@printf "CFLAGS = ${CFLAGS}\n"
	@printf "CPP = ${CPP}\n"
	@printf "CPPFLAGS = ${CPPFLAGS}\n"
	@printf "CPP_PTH = ${CPP_PTH}\n"
	@printf "CPP_TKN = ${CPP_TKN}\n"
	@printf "DODS = $(DODS)\n"
	@printf "DODS_ROOT = $(DODS_ROOT)\n"
	@printf "FC = ${FC}\n"
	@printf "FFLAGS = ${FFLAGS}\n"
	@printf "HDF5 = $(HDF5)\n"
	@printf "LDFLAGS = ${LDFLAGS}\n"
	@printf "LEX = $(LEX)\n"
	@printf "MDL_DPN = ${MDL_DPN}\n"
	@printf "MDL_OBJ = ${MDL_OBJ}\n"
	@printf "MDL_PTH = ${MDL_PTH}\n"
	@printf "MDL_RPM_NST_NM = $(MDL_RPM_NST_NM)\n"
	@printf "MDL_SRC = ${MDL_SRC}\n"
	@printf "NCO_VRS = $(NCO_VRS)\n"
	@printf "NETCDF_INC = ${NETCDF_INC}\n"
	@printf "NETCDF_LIB = ${NETCDF_LIB}\n"
	@printf "OMP = ${OMP}\n"
	@printf "PVM_ARCH = ${PVM_ARCH}\n"
	@printf "SRC_LST = $(SRC_LST)\n"
	@printf "USE_FORTRAN_ARITHMETIC = $(USE_FORTRAN_ARITHMETIC)\n"
	@printf "VPATH = ${VPATH}\n"
	@printf "VRS_SNG = ${VRS_SNG}\n"
	@printf "YACC = $(YACC)\n"
distclean: dst_cln
dst_cln: cln bin_cln doc_cln dat_cln
	cd ${MY_BLD_DIR}; rm -f *~
	cd ${MY_DAT_DIR}; rm -f in.nc *~
tags: 
	etags ${MY_SRC_DIR}/*.h $(SRC_LST) $(MDL_DOC_SRC) $(MDL_BLD_SRC) $(MDL_NCAP_SRC)

# Reset internal YACC and LEX patterns
%.c : %.y
%.c : %.l

# It is safest to do both YACC and LEX after either file changes
# Otherwise only changing one and then switching, e.g., from bison to yacc, can cause problems
# NB: Bison has a problem when bison.simple declares yyparse() as int yyparse (void);
# The solution is to comment out that definition in bison.simple
ifeq ($(YACC),bison)
${MY_SRC_DIR}/%.tab.c ${MY_SRC_DIR}/%.tab.h : ${MY_SRC_DIR}/%.y
	$(YACC) -d $<
endif
ifeq ($(YACC),yacc)
%.tab.c %.tab.h : %.y
	$(YACC) -d $<
	mv y.tab.c $(basename $<).tab.c
	mv y.tab.h $(basename $<).tab.h
endif

%_lex.c : %.l
	$(LEX) $<
	mv lex.yy.c $(basename $<)_lex.c

# For some reason, ncap_lex.c is not remade when I expect it to be, so I explicitly remove the object file every time
ncap:	${MY_OBJ_DIR}/ncap.tab.o ncap.tab.h ${MY_OBJ_DIR}/ncap_lex.o ${MY_OBJ_DIR}/ncap.o ${MY_OBJ_DIR}/ncap_utl.o $(libnco).a
#        $(YACC) --name-prefix=nc_
ifeq (${PVM_ARCH},LINUX)
	${CC} ${CFLAGS} -o ${MY_BIN_DIR}/$@ ${MY_OBJ_DIR}/$@.o ${MY_OBJ_DIR}/ncap_utl.o ${MY_OBJ_DIR}/$@.tab.o ${MY_OBJ_DIR}/$@_lex.o ${LDFLAGS}
else	
	${CC} ${CFLAGS} -o ${MY_BIN_DIR}/$@ ${MY_OBJ_DIR}/$@.o ${MY_OBJ_DIR}/ncap_utl.o ${MY_OBJ_DIR}/getopt.o ${MY_OBJ_DIR}/getopt1.o ${MY_OBJ_DIR}/$@.tab.o ${MY_OBJ_DIR}/$@_lex.o -ll -ly ${LDFLAGS}
endif
	chmod 755 ${MY_BIN_DIR}/$@
	/bin/rm -f $(MDL_NCAP) ${MY_OBJ_DIR}/$@_lex.o

# Create dependency files only if they will not be immediately deleted
INCLUDE_DPN := TRUE
GOALS_WHICH_DELETE_DEPENDENCY_FILES := cln clean dir distclean dst_cln dpn_cln tags uninstall
ifeq (,$(findstring $(MAKECMDGOALS),${GOALS_WHICH_DELETE_DEPENDENCY_FILES}))
 INCLUDE_DPN := TRUE
else
 INCLUDE_DPN := FALSE
endif
ifeq (${INCLUDE_DPN},TRUE)
-include ${MDL_DPN}
endif
