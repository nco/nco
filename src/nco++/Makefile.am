## Process this file with automake to produce Makefile.in
# $Header: /data/zender/nco_20150216/nco/src/nco++/Makefile.am,v 1.1 2006-03-15 17:25:31 mangalam Exp $

# the main program in this dir is ncap2
bin_PROGRAMS = ncap2

ANTLR = runantlr

# also needs runantlr parsing assistance

#ncolib_SOURCES includes almost all the ../nco C sources (except for the individual operator source files)
#but these shoud all be built before nco++ gets built.
#ncolib_SOURCES = ../nco/*.c

ncap2_SOURCES = Invoke.cc  ncap2.cc  ncap2_utl.cc  NcapVarVector.cc  ncoLexer.cpp  ncoParser.cpp  ncoTree.cpp

ncoLexer.cpp : ncoGrammer.g
	${ANTLR} ncoGrammer.g
ncoParser.cpp : ncoGrammer.g
	${ANTLR} ncoGrammer.g
ncoTree.cpp : ncoGrammer.g
	${ANTLR} ncoGrammer.g


#either of the following should work if the ..nco dir is built first as it's supposed to be.
#ncap2_LDADD = ../nco/.libs/libnco.so
#ncap2_LDADD = ../nco/libnco.la
ncap2_LDFLAGS = -lantlr -lnco
INCLUDES = -I./ -I../nco

# the headers should include all the .hh files
# HEADER_SRC = \
# ../nco/nco_netcdf.h \
# ../nco/libnco.h \
# ../nco/nco.h \
# ../nco/nco_var_utl.h \
# libnco++.hh  \
# Ncap2.hh \
# NcapVarVector.hh \
# VarOp.hh \
# ncap2.hh \
# NcapVar.hh \
# NcapVector.hh


#The following libs have to be included based on configure options.  Down here as a reminder.
# -lnco++ -lnco -lnco_c++ -lantlr
# -lxlf90 # Required for linking Fortran objects
# # if dods
# -lnc-dods -ldap++ -lnc-dods -ldap++ -lxml2 -lcurl -lpthread -ldl -lz

# if netcdf4
# -lnetcdf -lhdf5 -lhdf5_hl
# if udunits
# -ludunits
