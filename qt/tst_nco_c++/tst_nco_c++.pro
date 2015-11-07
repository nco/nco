TEMPLATE = app
TARGET = tst_nco_c++
CONFIG -= qt
CONFIG += console
CONFIG += debug_and_release

CONFIG( debug, debug|release ) {
	win32:LIBS += ../libnco_c++/debug/libnco_c++.lib
	unix:LIBS  += ../libnco_c++/debug/liblibnco_c++.a
} else {
	win32:LIBS += ../libnco_c++/release/libnco_c++.lib
	unix:LIBS  += ../libnco_c++/release/liblibnco_c++.a
}

INCLUDEPATH += ../../src/nco_c++
HEADERS   = 
SOURCES   = ../../src/nco_c++/tst.cc

unix {
 INCLUDEPATH += /usr/local/include
 INCLUDEPATH += /usr/local
 LIBS += -L/usr/lib/ -lnetcdf -lhdf5_hl -lhdf5
 LIBS += -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib/i386-linux-gnu/ -lz -ludunits2 -lexpat 
}
win32 {
 QMAKE_CFLAGS_RELEASE += /MT
 QMAKE_CXXFLAGS_RELEASE += /MT
 QMAKE_CFLAGS_DEBUG += /MTd
 QMAKE_CXXFLAGS_DEBUG += /MTd
 INCLUDEPATH += $(HEADER_NETCDF)
 LIBS += $(LIB_NETCDF)
 LIBS += $(LIB_DISPATCH)
 LIBS += $(LIB_NETCDF4)
 LIBS += $(LIB_HDF5)
 LIBS += $(LIB_HDF5_HL)
 LIBS += $(LIB_ZLIB)
 LIBS += $(LIB_SZIP)
 LIBS += $(LIB_CURL)
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 DEFINES += ENABLE_NETCDF4
 DEFINES += HAVE_NETCDF4_H
 DEFINES += NEED_STRCASECMP
 DEFINES += NEED_STRCASESTR
}
