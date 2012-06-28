# template type is application
TEMPLATE = app
# name
TARGET = ncwa

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

unix:HEADERS = ../../src/nco/ncap_yacc.h
unix:SOURCES   = ../../src/nco/ncwa.c \
../../src/nco/ncap_utl.c 

unix {
FLEXSOURCES = ../../src/nco/ncap_lex.l
BISONSOURCES = ../../src/nco/ncap_yacc.y	 

flex.name = flex ${QMAKE_FILE_IN}
flex.input = FLEXSOURCES
flex.output = ../../src/nco/ncap_lex.c
flex.commands = flex -o ../../src/nco/ncap_lex.c ${QMAKE_FILE_IN}
flex.variable_out = GENERATED_SOURCES
silent:flex.commands = @echo Lex ${QMAKE_FILE_IN} && $$flex.commands
QMAKE_EXTRA_COMPILERS += flex

bison.name = bison ${QMAKE_FILE_IN}
bison.input = BISONSOURCES
bison.commands = bison -d -o ../../src/nco/ncap_yacc.c ${QMAKE_FILE_IN}
bison.output = ../../src/nco/ncap_yacc.c
bison.variable_out = GENERATED_SOURCES
bison.CONFIG += target_predeps
silent:bison.commands = @echo Bison ${QMAKE_FILE_IN} && $$bison.commands
QMAKE_EXTRA_COMPILERS += bison

bisonheader.commands = bison -d -o ../../src/nco/ncap_yacc.c ${QMAKE_FILE_IN}
bisonheader.input = BISONSOURCES
bisonheader.output = ../../src/nco/ncap_yacc.c
bisonheader.variable_out = HEADERS
bisonheader.name = bison header
silent:bisonheader.commands = @echo Bison ${QMAKE_FILE_IN} && $$bison.commands
QMAKE_EXTRA_COMPILERS += bisonheader
}




