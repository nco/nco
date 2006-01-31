CC        = g++
LD        = g++
DEBUG     = -Wall

# Compile try/except

CFLAGS    = -g -O2  -Wall -D_BSD_SOURCE -D_POSIX_SOURCE 
OBJ       = o
EXE       = .exe
INCLUDE   = -I/usr/local/include/antlr -I/opt/SOURCES/netcdf-3.6.0-p1/include -I../nco -I. -I/opt/SOURCES/nco

LD_DEBUG  = /debug
ANTLR_LIB = -lantlr 
LDFLAGS   = -L/opt/SOURCES/nco/lib -L/opt/SOURCES/netcdf-3.6.0-p1/lib

LD_LIBRARY_PATH=/opt/SOURCES/nco/lib 
LD_RUN_PATH=/opt/SOURCES/nco/lib 

NCO_LIB   = -lnco
LINK_FLAGS = $(ANTLR_LIB) -lnco -lnetcdf -lm 

FLAGS = $(INCLUDE) $(DEBUG) $(CFLAGS) 

#
# To customize, fill in EXENAME, GRAMMAR, PARSER and LEXER
#
EXENAME = ncap

PARSE_OBJ = ncoLexer.$(OBJ) ncoParser.$(OBJ) ncoTree.$(OBJ) ncap.$(OBJ) ncap_utl.$(OBJ) Invoke.$(OBJ) NcapVarVector.$(OBJ)

all: $(EXENAME)$(EXE)

$(EXENAME)$(EXE): $(PARSE_OBJ)
	$(LD) $(PARSE_OBJ) $(INCLUDE) $(LDFLAGS) $(LINK_FLAGS) -static -o $(EXENAME)


#dummy.o:ncoGrammer.g
#	antlr ncoGrammer.g


ncoParser.$(OBJ): ncoParser.cpp ncoParser.hpp
	$(CC) -c $(CFLAGS) $(INCLUDE) ncoParser.cpp 

ncoLexer.$(OBJ): ncoLexer.cpp ncoLexer.hpp
	$(CC) -c $(CFLAGS) $(INCLUDE) ncoLexer.cpp 

ncoTree.$(OBJ): ncoTree.cpp ncoTree.hpp
	$(CC) -c $(CFLAGS) $(INCLUDE) ncoTree.cpp 

Invoke.$(OBJ): Invoke.cc
	$(CC) -c $(CFLAGS) $(INCLUDE) Invoke.cc  

NcapVarVector.$(OBJ): NcapVarVector.cc NcapVarVector.hh
	$(CC) -c $(CFLAGS) $(INCLUDE) NcapVarVector.cc

ncap_utl.$(OBJ): ncap_utl.cc 
	$(CC)  -c $(CFLAGS) $(INCLUDE) ncap_utl.cc 


ncap.$(OBJ): ncap.cc ncoParser.hpp ncoLexer.hpp ncoTree.hpp Invoke.cc 
	$(CC) -c $(CFLAGS) $(INCLUDE) ncap.cc

clean:
	rm -f Calc*.hpp Calc*.cpp *.o calc *.txt

