# $Header: /data/zender/nco_20150216/nco/src/nco_c++/Makefile.am,v 1.9 2006-12-02 21:35:18 zender Exp $ -*-makefile-*-

lib_LTLIBRARIES = libnco_c++.la

include_HEADERS = libnco_c++.hh nco_dmn.hh nco_hgh.hh nco_var.hh \
nco_att.hh nco_fl.hh nco_utl.hh

libnco_c___la_SOURCES = nco_att.cc nco_dmn.cc nco_fl.cc nco_hgh.cc nco_utl.cc nco_var.cc

libnco_c___la_CPPFLAGS = -DABORT_ON_ERROR

libnco_c___la_LDFLAGS = -release @VERSION@
check_PROGRAMS = tst

tst_SOURCES = tst.cc

tst_LDADD = -lnco_c++

TESTS = tst
	ln -f -s ../../data/in.nc in.nc

