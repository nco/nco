# template type is application
TEMPLATE = app
# name
TARGET = ncrcat

CONFIG -= qt
CONFIG += debug_and_release

# NCO library
CONFIG( debug, debug|release ) {
    # debug
        win32:LIBS += ../libnco/debug/libnco.lib
        unix:LIBS  += ../libnco/debug/liblibnco.a
} else {
    # release
        win32:LIBS += ../libnco/release/libnco.lib
        unix:LIBS  += ../libnco/release/liblibnco.a
}

# netCDF
DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

# common NCO settings
include (../nco.pri)

# SOURCES
# HEADERS

HEADERS   = 
SOURCES   = ../../src/nco/ncra.c