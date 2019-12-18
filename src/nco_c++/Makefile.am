# $Header$ -*-makefile-*-

test_data = in.nc

all: ${test_data}

in.nc:
	cd ../../data;make;cd -
	ln -f -s ../../data/in.nc in.nc

lib_LTLIBRARIES = libnco_c++.la

include_HEADERS = libnco_c++.hh nco_dmn.hh nco_hgh.hh nco_var.hh nco_att.hh nco_fl.hh nco_utl.hh

libnco_c___la_SOURCES = nco_att.cc nco_dmn.cc nco_fl.cc nco_hgh.cc nco_utl.cc nco_var.cc

libnco_c___la_CPPFLAGS = -DABORT_ON_ERROR

# NB: VERSION here refers to ... what? argument to AC_INIT() in configure.ac? No, that's package_version
libnco_c___la_LDFLAGS = -release @VERSION@

tst_SOURCES = tst.cc

tst_LDADD = libnco_c++.la

TESTPROGRAMS = tst
check_PROGRAMS = $(TESTPROGRAMS)
TESTS = $(TESTPROGRAMS)

# Test programs produce these files:
CLEANFILES = out.nc

MAINTAINERCLEANFILES = Makefile.in in.nc

test: check

