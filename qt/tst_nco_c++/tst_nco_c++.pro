# template type is application
TEMPLATE = app
# name
TARGET = tst

CONFIG -= qt
CONFIG += debug

DEFINES += ENABLE_NETCDF4

INCLUDEPATH += ../../src/nco_c++

HEADERS   = 
SOURCES   = ../../src/nco_c++/tst.cc

#libnco_c++ library
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
 LIBS += $(LIB_SZIP)
 LIBS += $(LIB_ZLIB)
 LIBS += ../nco_c++/debug/libnco_c++.lib
 
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 CONFIG  += console
}
