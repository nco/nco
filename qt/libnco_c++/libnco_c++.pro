TEMPLATE = lib
TARGET = libnco_c++
CONFIG += console 
CONFIG += static
CONFIG -= qt
CONFIG += debug_and_release

INCLUDEPATH += ../../src/nco_c++

unix:CONFIG( debug, debug|release ) {
        DESTDIR = ./debug
} else {
        DESTDIR = ./release
}

unix {
 INCLUDEPATH += /usr/local/include
 INCLUDEPATH += /usr/local
}
win32 {
 QMAKE_CFLAGS_RELEASE += /MT
 QMAKE_CXXFLAGS_RELEASE += /MT
 QMAKE_CFLAGS_DEBUG += /MTd
 QMAKE_CXXFLAGS_DEBUG += /MTd
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 DEFINES += ENABLE_NETCDF4
 DEFINES += HAVE_NETCDF4_H
 DEFINES += ENABLE_GSL
 DEFINES += ENABLE_UDUNITS
 DEFINES += HAVE_UDUNITS2_H
 INCLUDEPATH += $(HEADER_NETCDF) 
 INCLUDEPATH += $(HEADER_GSL)
}

HEADERS += ../../src/nco_c++/libnco_c++.hh \
../../src/nco_c++/nco_dmn.hh \
../../src/nco_c++/nco_hgh.hh \
../../src/nco_c++/nco_var.hh \
../../src/nco_c++/nco_att.hh \
../../src/nco_c++/nco_fl.hh \
../../src/nco_c++/nco_utl.hh 

SOURCES += ../../src/nco_c++/nco_att.cc \
../../src/nco_c++/nco_dmn.cc \
../../src/nco_c++/nco_fl.cc \
../../src/nco_c++/nco_hgh.cc \
../../src/nco_c++/nco_utl.cc \
../../src/nco_c++/nco_var.cc
