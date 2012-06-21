# template type is application
TEMPLATE = app
# name
TARGET = ncrename


CONFIG -= qt
CONFIG += debug_and_release

#nco library
CONFIG( debug, debug|release ) {
    # debug
	LIBS += ../libnco/debug/libnco.lib
} else {
    # release
	LIBS += ../libnco/release/libnco.lib
}

#nco avoid writing temporary file
DEFINES += WRT_TMP_FL

#netCDF
DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

HEADERS   = 
SOURCES   = ../../src/nco/ncrename.c



# netCDF library
# gcc settings to use C99
unix {
 INCLUDEPATH +=
 LIBS +=
 QMAKE_CFLAGS += -std=c99
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
