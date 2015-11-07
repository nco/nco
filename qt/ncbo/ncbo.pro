TARGET = ncbo
TEMPLATE = app
CONFIG -= qt
CONFIG += debug_and_release

CONFIG( debug, debug|release ) {
        win32:LIBS += ../libnco/debug/libnco.lib
        unix:LIBS  += ../libnco/debug/liblibnco.a
} else {
        win32:LIBS += ../libnco/release/libnco.lib
        unix:LIBS  += ../libnco/release/liblibnco.a
}

include (../nco.pri)

HEADERS   = 
SOURCES   = ../../src/nco/ncbo.c
