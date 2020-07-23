# $Header$ -*-makefile-*-

# Purpose: Template builds for nco++ directory
# Process this file with automake to produce Makefile.in

# ncap2 is only executable in this directory
bin_PROGRAMS = ncap2

ANTLR = runantlr

# ncolib_SOURCES includes all ../nco C-sources except for operator main() files
# These are all built before nco++

# Invoke 'runantlr' on ncoGrammer.g to generate required hpp files
#Invoke.cc : ncoGrammer.g
#Invoke.o : ncoGrammer.g
#	${ANTLR} ncoGrammer.g

# 20160125: fxm postprocess tokens when symbol table changes
# cd ~/nco/src/nco++;antlr ncoGrammer.g;./nco_antlr_pst_prc.pl < ncoParserTokenTypes.hpp > ncoEnumTokenTypes.hpp

# fxm: Split off all sources except front-end into nco++ library
ncap2_SOURCES = Invoke.cc \
ncap2.cc \
ncap2_utl.cc \
ncap2_att.cc \
sdo_utl.cc \
sym_cls.cc \
fmc_cls.cc \
fmc_all_cls.cc \
fmc_gsl_cls.cc \
prs_cls.cc \
NcapVar.cc \
NcapVarVector.cc \
ncoLexer.cpp \
ncoParser.cpp \
ncoTree.cpp \
nco_gsl.cc

# 20190414 ncap2 on MacOS relink when libnco.a changes
#ncap2_LDFLAGS = ../nco/libnco.la

# 20150108 Be sure that netCDF in /usr/local takes precedence over GSL libraries in /usr
# AKA be sure GSL in /usr does not cause ncap2 linker to search /usr before /usr/local
NCO_AND_ANTLR_STUFF = ../nco/libnco.la -lantlr
#ncap2_LDFLAGS = ../nco/libnco.la -lantlr

# fxm: 20080515 Takeshi Enomoto reports that MACOSX needs
# ncap2_LDFLAGS = ../nco/.libs/libnco.dylib -lantlr

# fxm: 20140730 Ed Anderson reports that PGI- and GCC-compiled ncap2 need
# -Xcompiler -shared added to ncap2_LINK which, apparently, would be done with
# ncap2_LDFLAGS = -Xcompiler -shared

if ENABLE_GSL
GSL_LIB_ARG = $(shell gsl-config --libs)
GSL_INC_ARG = $(shell gsl-config --cflags)
#ncap2_LDFLAGS = ${GSL_LIB_ARG}
ncap2_LDFLAGS = ${NCO_AND_ANTLR_STUFF} ${GSL_LIB_ARG}
#AM_CPPFLAGS = -I./ -I../nco ${GSL_INC_ARG}
AM_CPPFLAGS = -I./ -I../nco
else
ncap2_LDFLAGS = ${NCO_AND_ANTLR_STUFF}
AM_CPPFLAGS = -I./ -I../nco
endif

# 20060824: *.hpp and *.cpp files are now part of source tree
# DISTCLEANFILES=ncoLexer.hpp ncoParser.hpp ncoParserTokenTypes.hpp ncoParserTokenTypes.txt ncoTree.hpp
# CLEANFILES=ncap2 ncoLexer.hpp ncoParser.hpp ncoParserTokenTypes.hpp ncoParserTokenTypes.txt ncoTree.hpp
DISTCLEANFILES=ncoParserTokenTypes.txt
CLEANFILES=ncap2 ncoParserTokenTypes.txt

# Configure automagically includes following libraries as requested:
# Keep list here as reminder:
# ncap2:
# -lnco++ -lnco -lnco_c++ -lantlr
# OPeNDAP:
# -lnc-dap -ldap -lxml2 -lcurl (Linux)
# -lnc-dap -ldap -lxml2 -lcurl -lcrypto -liconv -lssl -lz (AIX)
# netCDF4:
# -lnetcdf -lhdf5 -lhdf5_hl
# UDUnits:
# -ludunits
