lib_LTLIBRARIES = libnco_c++.la

HEADERS_SRC = libnco_c++.hh nco_dmn.hh nco_hgh.hh nco_var.hh \
nco_att.hh nco_fl.hh nco_utl.hh

libnco_c___la_SOURCES = nco_att.cc nco_dmn.cc nco_fl.cc nco_hgh.cc nco_utl.cc nco_var.cc $(HEADERS_SRC)

check_PROGRAMS = tst

tst_SOURCES = tst.cc

tst_LDADD = -lnco_c++

TESTS = tst
