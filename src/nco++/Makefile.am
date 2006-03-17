## Process this file with automake to produce Makefile.in
# $Header: /data/zender/nco_20150216/nco/src/nco++/Makefile.am,v 1.2 2006-03-17 21:59:23 mangalam Exp $

# the main program in this dir is ncap2
bin_PROGRAMS = ncap2

ANTLR = runantlr

#ncolib_SOURCES includes almost all the ../nco C sources (except for the individual operator source files)
#but these shoud all be built before nco++ gets built.

ncap2_SOURCES = Invoke.cc  ncap2.cc  ncap2_utl.cc  NcapVarVector.cc  ncoLexer.cpp  ncoParser.cpp  ncoTree.cpp

# need to invoke 'runantlr' on ncoGrammer.g to generate the required hpp files.  A single .cc file will do it.
Invoke.cc : ncoGrammer.g
	${ANTLR} ncoGrammer.g

#either of the following should work if the ..nco dir is built first as it's supposed to be.
ncap2_LDFLAGS = -lantlr -lnco
INCLUDES = -I./ -I../nco

#The following libs have to be included based on configure options.  Down here as a reminder.
# -lnco++ -lnco -lnco_c++ -lantlr
# -lxlf90 # Required for linking Fortran objects
# # if dods
# -lnc-dods -ldap++ -lnc-dods -ldap++ -lxml2 -lcurl -lpthread -ldl -lz

# if netcdf4
# -lnetcdf -lhdf5 -lhdf5_hl
# if udunits
# -ludunits
