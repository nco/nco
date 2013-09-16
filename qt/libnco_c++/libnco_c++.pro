# template type is library
TEMPLATE = lib
# name of library
TARGET = libnco_c++
# the library is a static library
CONFIG += staticlib

CONFIG -= qt
CONFIG += debug_and_release

INCLUDEPATH += ../../src/nco_c++


#nco_c++ library
unix:CONFIG( debug, debug|release ) {
    # debug
        DESTDIR = ./debug
} else {
    # release
        DESTDIR = ./release
}

unix {
 INCLUDEPATH += /usr/local/include
 INCLUDEPATH += /usr/local
 #RHEL
 INCLUDEPATH += /SNS/users/pvicente/libs/install/netcdf-4.3.1-rc2/include
 INCLUDEPATH += /SNS/users/pvicente/libs/install/udunits-2.1.24/include
}
win32 {
 INCLUDEPATH += $(HEADER_NETCDF) 
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 
 # netCDF assumed in Windows build
 DEFINES += ENABLE_NETCDF4
 DEFINES += HAVE_NETCDF4_H
 DEFINES += NEED_STRCASECMP
 DEFINES += NEED_STRCASESTR
 # gsl
 INCLUDEPATH += $(HEADER_GSL)
 DEFINES += ENABLE_GSL
}

# header files
HEADERS += ../../src/nco_c++/libnco_c++.hh \
../../src/nco_c++/nco_dmn.hh \
../../src/nco_c++/nco_hgh.hh \
../../src/nco_c++/nco_var.hh \
../../src/nco_c++/nco_att.hh \
../../src/nco_c++/nco_fl.hh \
../../src/nco_c++/nco_utl.hh 

# source files
SOURCES += ../../src/nco_c++/nco_att.cc \
../../src/nco_c++/nco_dmn.cc \
../../src/nco_c++/nco_fl.cc \
../../src/nco_c++/nco_hgh.cc \
../../src/nco_c++/nco_utl.cc \
../../src/nco_c++/nco_var.cc
