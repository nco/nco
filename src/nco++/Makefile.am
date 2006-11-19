# $Header: /data/zender/nco_20150216/nco/src/nco++/Makefile.am,v 1.12 2006-11-19 23:58:30 zender Exp $ -*-makefile-*-

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

# fxm: Split off all sources except front-end into nco++ library
ncap2_SOURCES = Invoke.cc ncap2.cc ncap2_utl.cc sdo_utl.cc NcapVarVector.cc ncoLexer.cpp ncoParser.cpp ncoTree.cpp

ncap2_LDADD = -lnco -lantlr
ncap2_LDFLAGS = -L../nco
INCLUDES = -I./ -I../nco

# 20060824: *.hpp and *.cpp files are now part of source tree
# DISTCLEANFILES=ncoLexer.hpp ncoParser.hpp ncoParserTokenTypes.hpp ncoParserTokenTypes.txt ncoTree.hpp
# CLEANFILES=ncap2 ncoLexer.hpp ncoParser.hpp ncoParserTokenTypes.hpp ncoParserTokenTypes.txt ncoTree.hpp
DISTCLEANFILES=ncoParserTokenTypes.txt
CLEANFILES=ncap2 ncoParserTokenTypes.txt

# Following libraries are included based on configure options
# Down here as a reminder.
# -lnco++ -lnco -lnco_c++ -lantlr
# -lxlf90 # Required for linking Fortran objects
# OPeNDAP
# -lnc-dods -ldap++ -lnc-dods -ldap++ -lxml2 -lcurl -lpthread -ldl -lz
# netcdf4
# -lnetcdf -lhdf5 -lhdf5_hl
# Udunits
# -ludunits
