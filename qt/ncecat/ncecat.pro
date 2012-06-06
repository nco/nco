# template type is application
TEMPLATE = app
# name
TARGET = ncecat

CONFIG -= qt
CONFIG += debug

DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

HEADERS   = 
SOURCES   = ../../src/nco/ncecat.c

#NCO library
LIBS += ../libnco/debug/libnco.lib

# netCDF library
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
 
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 CONFIG += console

}
