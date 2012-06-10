# template type is library
TEMPLATE = lib
# name of library
TARGET = libnco_c++
# the library is a static library
CONFIG += staticlib

CONFIG -= qt
CONFIG += debug_and_release

#nco library
CONFIG( debug, debug|release ) {
    # debug
	LIBS += ../libnco/debug/libnco_c++.lib
} else {
    # release
	LIBS += ../libnco/release/libnco_c++.lib
}


# netCDF library
unix {
 INCLUDEPATH += 
 LIBS += 
}
win32 {
 INCLUDEPATH += $(HEADER_NETCDF) 
 INCLUDEPATH += ../../src/nco_c++
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
}

#libnco_c++.hh nco_dmn.hh nco_hgh.hh nco_var.hh nco_att.hh nco_fl.hh nco_utl.hh
# header files
HEADERS += ../../src/nco_c++/libnco_c++.hh \
../../src/src/nco_c++/nco_dmn.hh \
../../src/nco_c++/nco_hgh.hh \
../../src/nco_c++/nco_var.hh \
../../src/nco_c++/nco_att.hh \
../../src/nco_c++/nco_fl.hh \
../../src/nco_c++/nco_utl.hh 



#libnco_c___la_SOURCES = nco_att.cc nco_dmn.cc nco_fl.cc nco_hgh.cc nco_utl.cc nco_var.cc
# source files
SOURCES += ../../src/nco_c++/nco_att.cc \
../../src/nco_c++/nco_dmn.cc \
../../src/nco_c++/nco_fl.cc \
../../src/nco_c++/nco_hgh.cc \
../../src/nco_c++/nco_utl.cc \
../../src/nco_c++/nco_var.cc