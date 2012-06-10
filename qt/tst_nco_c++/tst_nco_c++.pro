# template type is application
TEMPLATE = app
# name
TARGET = tst

CONFIG -= qt
CONFIG += debug_and_release

#nco++ library
CONFIG( debug, debug|release ) {
    # debug
	LIBS += ../nco_c++/debug/libnco_c++.lib
} else {
    # release
	LIBS += ../nco_c++/release/libnco_c++.lib
}


DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

INCLUDEPATH += ../../src/nco_c++

HEADERS   = 
SOURCES   = ../../src/nco_c++/tst.cc


#netCDF library
unix {
 INCLUDEPATH += 
 LIBS += 
}
win32 {
 INCLUDEPATH += $(HEADER_NETCDF)
 LIBS += $(LIB_NETCDF)
 LIBS += $(LIB_DISPATCH)
 LIBS += $(LIB_NETCDF4)
 LIBS += $(LIB_HDF5)
 LIBS += $(LIB_HDF5_HL)
 LIBS += $(LIB_ZLIB)
 LIBS += $(LIB_SZIP)
 
 
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 CONFIG  += console
}
