# template type is application
TEMPLATE = app
# name
TARGET = tst_nco_c++

CONFIG -= qt
CONFIG += debug_and_release

#nco++ library
CONFIG( debug, debug|release ) {
	# debug
	win32:LIBS += ../libnco_c++/debug/libnco_c++.lib
	unix:LIBS  += ../libnco_c++/debug/liblibnco_c++.a
} else {
	# release
	win32:LIBS += ../libnco_c++/release/libnco_c++.lib
	unix:LIBS  += ../libnco_c++/release/liblibnco_c++.a
}

INCLUDEPATH += ../../src/nco_c++
HEADERS   = 
SOURCES   = ../../src/nco_c++/tst.cc

#netCDF library
unix {
 INCLUDEPATH += /usr/local/include
 INCLUDEPATH += /usr/local
 LIBS += -L/usr/lib/ -lnetcdf -lhdf5_hl -lhdf5
 LIBS += -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib/i386-linux-gnu/ -lz -ludunits2 -lexpat 
 #RHEL
 INCLUDEPATH += /SNS/users/pvicente/libs/install/netcdf-4.3.1-rc2/include
 INCLUDEPATH += /SNS/users/pvicente/libs/install/udunits-2.1.24/include
 LIBS += -L/SNS/users/pvicente/libs/install/netcdf-4.3.1-rc2/lib
 LIBS += -L/SNS/users/pvicente/libs/install/udunits-2.1.24/lib
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
 LIBS += $(LIB_CURL)
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 CONFIG  += console
 
 # netCDF assumed in Windows build
 DEFINES += ENABLE_NETCDF4
 DEFINES += HAVE_NETCDF4_H
 DEFINES += NEED_STRCASECMP
 DEFINES += NEED_STRCASESTR
}
