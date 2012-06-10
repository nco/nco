# template type is application
TEMPLATE = app
# name
TARGET = ncap2

CONFIG -= qt
CONFIG += debug_and_release

#nco library
CONFIG( debug, debug|release ) {
    # debug
	LIBS += ../libnco/debug/libnco.lib
    LIBS += $(LIB_ANTLR)
} else {
    # release
	LIBS += ../libnco/release/libnco.lib
    LIBS += $(LIB_ANTLR_REL)
}


DEFINES += ENABLE_NETCDF4
DEFINES += HAVE_NETCDF4_H

INCLUDEPATH += ../../src/nco
INCLUDEPATH += ../../src/nco++

#fmc_all_cls.hh fmc_cls.hh Invoke.hh libnco++.hh map_srt_tmp.hh ncap2.hh ncap2_utl.hh NcapVar.hh NcapVarVector.hh 
#NcapVector.hh ncoEnumTokenTypes.hpp ncoTree.hpp prs_cls.hh sdo_utl.hh sym_cls.hh VarOp.hh vtl_cls.hh

#gsl
#fmc_gsl_cls.hh

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

#ncap2_SOURCES = Invoke.cc ncap2.cc ncap2_utl.cc sdo_utl.cc sym_cls.cc fmc_cls.cc fmc_all_cls.cc fmc_gsl_cls.cc prs_cls.cc 
#NcapVar.cc NcapVarVector.cc ncoLexer.cpp ncoParser.cpp ncoTree.cpp

# prs_cls.cc

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


#netCDF library
unix {
 INCLUDEPATH += 
 LIBS += 
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
 INCLUDEPATH += $(HEADER_ANTLR)
 
 
 DEFINES += _CRT_SECURE_NO_WARNINGS
 DEFINES += _CRT_NONSTDC_NO_DEPRECATE
 CONFIG += console
 
}
