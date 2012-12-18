# common NCO Qt dependency library settings for all NCO operators
# gcc settings to use C99
# HDF5 and netCDF LIB order is important
# _BSD_SOURCE and _POSIX_SOURCE needed
unix {
 DEFINES += HAVE_CONFIG_H
 INCLUDEPATH += ../../
 DEFINES += _BSD_SOURCE
 DEFINES += _POSIX_SOURCE
 QMAKE_CFLAGS += -std=c99
 INCLUDEPATH += /usr/local/include
 INCLUDEPATH += /usr/local
 LIBS += -L/usr/local/lib -L/usr/lib/ -lnetcdf -lhdf5_hl -lhdf5 -lgsl -lgslcblas -lm
 LIBS += -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib/i386-linux-gnu/ -lz -ludunits2 -lexpat 
}

CONFIG -= qt
#CONFIG += netcdf3

win32{
 CONFIG += console 
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 DEFINES += NEED_STRCASECMP
 DEFINES += NEED_STRCASESTR
}

win32{
 netcdf3 {
 INCLUDEPATH += $(HEADER_NETCDF3)
 LIBS += $(LIB_NETCDF3)
 } else {
 INCLUDEPATH += $(HEADER_NETCDF)
 LIBS += $(LIB_NETCDF)
 LIBS += $(LIB_DISPATCH)
 LIBS += $(LIB_NETCDF4)
 LIBS += $(LIB_HDF5)
 LIBS += $(LIB_HDF5_HL)
 LIBS += $(LIB_ZLIB)
 LIBS += $(LIB_SZIP)
 DEFINES += ENABLE_NETCDF4
 DEFINES += HAVE_NETCDF4_H
}
}

