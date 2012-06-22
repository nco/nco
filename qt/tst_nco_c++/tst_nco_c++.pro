# template type is application
TEMPLATE = app
# name
TARGET = tst

CONFIG -= qt
CONFIG += debug_and_release

#nco++ library
CONFIG( debug, debug|release ) {
    # debug
	win32:LIBS += ../nco_c++/debug/libnco_c++.lib
        unix:LIBS  += ../nco_c++/debug/liblibnco_c++.a
} else {
    # release
	win32:LIBS += ../nco_c++/release/libnco_c++.lib
        unix:LIBS  += ../nco_c++/release/liblibnco_c++.a
}


DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

INCLUDEPATH += ../../src/nco_c++

HEADERS   = 
SOURCES   = ../../src/nco_c++/tst.cc


#netCDF library
unix {
 INCLUDEPATH += /usr/local/include
 INCLUDEPATH += /usr/local
 
 LIBS += ~/libs/install/netcdf-4.2/lib/libnetcdf.a
 LIBS += ~/libs/install/hdf5-1.8.5-patch1/lib/libhdf5_hl.a
 LIBS += ~/libs/install/hdf5-1.8.5-patch1/lib/libhdf5.a
 LIBS += ~/libs/install/zlib-1.2.7/lib/libz.a

 LIBS += ~/libs/install/udunits-2.1.24/lib/libudunits2.a
 LIBS += ~/libs/install/expat-2.1.0/lib/libexpat.a

 
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
