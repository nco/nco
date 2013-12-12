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
 # Mac Ports netCDF locations
 LIBS += -L/opt/local/lib
 INCLUDEPATH += /opt/local/include 
 # Mac Ports udunits locations
 INCLUDEPATH += /opt/local/include/udunits2
 #RHEL
 INCLUDEPATH += /SNS/users/pvicente/libs/install/netcdf-4.3.1-rc2/include
 INCLUDEPATH += /SNS/users/pvicente/libs/install/udunits-2.1.24/include
 LIBS += -L/SNS/users/pvicente/libs/install/netcdf-4.3.1-rc2/lib
 LIBS += -L/SNS/users/pvicente/libs/install/udunits-2.1.24/lib
}

CONFIG -= qt
#CONFIG += netcdf3

win32{
 CONFIG += console 
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 DEFINES += NEED_STRCASECMP
 # gsl
 INCLUDEPATH += $(HEADER_GSL)
 DEFINES += ENABLE_GSL
 DEFINES += ENABLE_DAP
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

 LIBS += $(LIB_GSL)
 LIBS += $(LIB_CURL)
}

