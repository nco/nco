# template type is application
TEMPLATE = app
# name
TARGET = ncpdq



CONFIG -= qt
CONFIG += debug_and_release

#nco library
CONFIG( debug, debug|release ) {
    # debug
        win32:LIBS += ../libnco/debug/libnco.lib
        unix:LIBS  += ../libnco/debug/liblibnco.a
} else {
    # release
        win32:LIBS += ../libnco/release/libnco.lib
        unix:LIBS  += ../libnco/release/liblibnco.a
}

#nco avoid writing temporary file
DEFINES += WRT_TMP_FL

#netCDF
DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

HEADERS   = 
SOURCES   = ../../src/nco/ncpdq.c



# netCDF library
# gcc settings to use C99
# HDF5 and netCDF LIB order is important
# _BSD_SOURCE and _POSIX_SOURCE needed
unix {
 DEFINES += HAVE_CONFIG_H
 INCLUDEPATH += ../../
 DEFINES += _BSD_SOURCE
 DEFINES += _POSIX_SOURCE
 QMAKE_CFLAGS += -std=c99
 INCLUDEPATH += ~/libs/install/netcdf-4.2/include

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
 CONFIG += console


}
