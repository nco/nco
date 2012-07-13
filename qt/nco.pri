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
# LIBS += /usr/local/lib/libnetcdf.a
# LIBS += /usr/local/lib/libhdf5_hl.a
# LIBS += /usr/local/lib/libhdf5.a
# LIBS += /usr/lib/x86_64-linux-gnu/libz.a
# LIBS += /usr/lib/x86_64-linux-gnu/libudunits2.a
# LIBS += /usr/lib/x86_64-linux-gnu/libexpat.a
# LIBS += /usr/lib/x86_64-linux-gnu/libcurl.a
  LIBS += -L/usr/local/lib -L/usr/lib/ -lnetcdf -lhdf5_hl -lhdf5 
  LIBS += -L/usr/lib/x86_64-linux-gnu/ -lz -ludunits2 -lexpat -lcurl

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

