# template type is application
TEMPLATE = app
# name
TARGET = ncap2

CONFIG -= qt
CONFIG += debug_and_release

# NCO library
CONFIG( debug, debug|release ) {
    # debug
        win32:LIBS += ../libnco/debug/libnco.lib
        unix:LIBS  += ../libnco/debug/liblibnco.a
        win32:LIBS += $(LIB_ANTLR) 
} else {
    # release
        win32:LIBS += ../libnco/release/libnco.lib
        unix:LIBS  += ../libnco/release/liblibnco.a
        win32:LIBS += $(LIB_ANTLR_REL)
}

# netCDF
DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

# library dependencies
unix {
 INCLUDEPATH += /usr/local/include 
 INCLUDEPATH += /usr/local 	 	  
# LIBS += /usr/local/lib/libnetcdf.a 
# LIBS += /usr/local/lib/libhdf5_hl.a 
# LIBS += /usr/local/lib/libhdf5.a 
# LIBS += /usr/lib/x86_64-linux-gnu/libz.a 
# LIBS += /usr/lib/x86_64-linux-gnu/libudunits2.a 
# LIBS += /usr/lib/x86_64-linux-gnu/libexpat.a 
# LIBS += /usr/lib/x86_64-linux-gnu/libcurl.a 
# LIBS += /usr/lib/libantlr.a 

  LIBS += -L/usr/lib/ -lnetcdf -lhdf5_hl -lhdf5
  LIBS += -L/usr/lib/x86_64-linux-gnu/ -lz -ludunits2 -lexpat -lcurl
  LIBS += -lantlr

}
win32 {
 INCLUDEPATH += $(HEADER_NETCDF)
 INCLUDEPATH += $(HEADER_ANTLR)
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

# SOURCES
# HEADERS

INCLUDEPATH += ../../src/nco
INCLUDEPATH += ../../src/nco++


HEADERS   = ../../src/nco++/fmc_all_cls.hh \
../../src/nco++/fmc_cls.hh \
../../src/nco++/Invoke.hh \
../../src/nco++/fmc_cls.hh \
../../src/nco++/libnco++.hh \
../../src/nco++/fmc_cls.hh \
../../src/nco++/map_srt_tmp.hh \
../../src/nco++/ncap2.hh \
../../src/nco++/ncap2_utl.hh \
../../src/nco++/ncap2_utl.hh \
../../src/nco++/NcapVar.hh \
../../src/nco++/NcapVarVector.hh  \
../../src/nco++/NcapVector.hh \
../../src/nco++/ncoEnumTokenTypes.hpp \
../../src/nco++/ncoTree.hpp \
../../src/nco++/prs_cls.hh \
../../src/nco++/sdo_utl.hh \
../../src/nco++/sym_cls.hh \
../../src/nco++/sdo_utl.hh \
../../src/nco++/VarOp.hh \
../../src/nco++/vtl_cls.hh


SOURCES   = ../../src/nco++/Invoke.cc \
../../src/nco++/ncap2.cc \
../../src/nco++/ncap2_utl.cc \
../../src/nco++/sdo_utl.cc \
../../src/nco++/sym_cls.cc \
../../src/nco++/fmc_cls.cc \
../../src/nco++/fmc_all_cls.cc \
../../src/nco++/fmc_gsl_cls.cc \
../../src/nco++/NcapVar.cc \
../../src/nco++/NcapVarVector.cc \
../../src/nco++/ncoLexer.cpp \
../../src/nco++/ncoParser.cpp \
../../src/nco++/ncoTree.cpp \
../../src/nco++/prs_cls.cc

