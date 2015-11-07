TEMPLATE = app
TARGET = ncap2
CONFIG -= qt
CONFIG += debug_and_release

CONFIG( debug, debug|release ) {
        win32:LIBS += ../libnco/debug/libnco.lib
        unix:LIBS  += ../libnco/debug/liblibnco.a
        win32:LIBS += $(LIB_ANTLR) 
} else {
        win32:LIBS += ../libnco/release/libnco.lib
        unix:LIBS  += ../libnco/release/liblibnco.a
        win32:LIBS += $(LIB_ANTLR_REL)
}

unix {
 LIBS += -lhdf5_hl -lhdf5 -lgsl -lgslcblas -lnetcdf -lm
 LIBS += -L/usr/lib/x86_64-linux-gnu/ -L/usr/lib/i386-linux-gnu/ -lz -ludunits2 -lexpat 
 LIBS += -lantlr
}

include (../nco.pri)

win32 {
 INCLUDEPATH += $(HEADER_ANTLR)
}

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
../../src/nco++/vtl_cls.hh \
../../src/nco++/fmc_gsl_cls.hh \
../../src/nco++/nco_gsl.h

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
../../src/nco++/prs_cls.cc \
../../src/nco++/nco_gsl.c

