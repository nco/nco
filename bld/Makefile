# $Header: /data/zender/nco_20150216/nco/bld/Makefile,v 1.1.1.1 1998-08-18 05:35:00 zender Exp $

MY_BIN_DIR := ../bin
MY_BLD_DIR := ../bld
MY_DAT_DIR := ../data
MY_DOC_DIR := ../doc
MY_INC_DIR := 
MY_LIB_DIR := ../lib
MY_OBJ_DIR := ../obj
ifndef OPTS
 OPTS := O
endif
PVM_ARCH := $(shell $(MY_BLD_DIR)/pvmgetarch)
ifndef C_ONLY
 C_ONLY := N
endif

# NB: Do NOT add comment lines, e.g., # This is a comma, to character definitions
null :=
space := $(null) $(null)
comma := ,
newline := \n

dirs := $(shell cat Filepath) # Load dependency search path
VPATH := $(subst $(space),:,$(dirs)) # Change spaces to colons. VPATH tells make where to find dependencies (which are not pathname qualified) in Depends file
cpp_dirs := $(dirs) # Additional directories, e.g., ./, could be added here
cpp_path := $(foreach dir,$(cpp_dirs),-I$(dir)) # -I format for compiler argument
MY_INC_DIR += $(cpp_path)
FIND_FNC = $(wildcard $(dir)/*.c $(dir)/*.F) # Find all C and Fortran source files in a given directory
SRC_LST = $(strip $(foreach dir, $(dirs),$(FIND_FNC))) # Assemble source files from all directories
SRC_STB_LST := $(strip $(sort $(notdir $(SRC_LST)))) # Source file names with directories removed
SRC_STB_FL := $(strip $(subst $(space),$(newline),$(SRC_STB_LST))) # Source files names sorted alphabetically, one per line, suitable for input to makdep
CRM_OBJ := $(addprefix $(MY_OBJ_DIR)/,$(addsuffix .o, $(basename $(SRC_STB_LST)))) # Dependency list for executable

# The first LDFLAGS is for typical C programs with netCDF, math, and networking
# The second LDFLAGS enables C/Fortran linking
# Redefine default patterns for C compiling
$(MY_OBJ_DIR)/%.o : %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $(MY_OBJ_DIR)/$(notdir $@)

ifeq ($(PVM_ARCH),CRAY)
NETCDF_INC := /usr/local/include
NETCDF_LIB := /usr/local/lib
C++ := g++
CC := cc
CPP := cpp
CPPFLAGS := -D$(PVM_ARCH) $(MY_INC_DIR) -I$(NETCDF_INC)
FC := f90
LD := ld
LDFLAGS := -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lnetcdf -lm
LDFLAGS += -L/lib -lf
LEX := lex
LINT := lint
YACC := yacc
ifeq ($(OPTS),O)
 CFLAGS := -O
endif
ifeq ($(OPTS),D)
 CFLAGS := -g
endif
# 97/10/21 Added -P to suppress #line # directives on Fortran files
$(MY_OBJ_DIR)/%.o : %.F
	$(CPP) -P $(CPPFLAGS) $< > $(patsubst %.F,%.f,$(notdir $<))
	$(FC) -c $(FFLAGS) $(patsubst %.F,%.f,$(notdir $<)) 
	-mv -f $(notdir $@) $(MY_OBJ_DIR)
	rm -f $(patsubst %.F,%.f,$(notdir $<)) 
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) $<
	mv -f $(notdir $@) $(MY_OBJ_DIR)
endif

ifeq ($(PVM_ARCH),LINUX)
NETCDF_INC := /usr/local/include
NETCDF_LIB := /usr/local/lib
C++ := g++
# 98/05/17 As of gcc-2.8.0 using -ansi switch breaks because resolv.h structure have changed
#CC := gcc -ansi
CC := gcc
# NB: nameser.h needs -Di386, but gcc is sending -Di586 (on pentiums)
CPPFLAGS := -D$(PVM_ARCH) -Di386 $(MY_INC_DIR) -I$(NETCDF_INC)
FC := g77
LD := ld
LDFLAGS := -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lnetcdf -lm
LDFLAGS += 
LEX := flex
LINT := lint
YACC := bison
ifeq ($(OPTS),O)
 CFLAGS := -O
endif
ifeq ($(OPTS),D)
 CFLAGS := -g
endif
$(MY_OBJ_DIR)/%.o : %.F
	$(FC) -c $(FFLAGS) $(CPPFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
endif

ifeq ($(PVM_ARCH),RS6K)
NETCDF_INC := /usr/local/include
NETCDF_LIB := /usr/local/lib
C++ := g++
CC := gcc -ansi
CPP := /lib/cpp -P
CPPFLAGS := -D$(PVM_ARCH) $(MY_INC_DIR) -I$(NETCDF_INC)
FC := xlf
LD := ld
LDFLAGS := -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lncaru -lnetcdf -lm
LDFLAGS += -lxlf90 -lxlf
LEX := lex
LINT := lint
YACC := yacc
ifeq ($(OPTS),O)
 CFLAGS := -O2
 CPP := $(CPP) $(CPPFLAGS)
 PREPROCESS.F := $(CPP) $(CPPFLAGS)
 FFLAGS := -O -NS2000 -qfixed=132
endif
ifeq ($(OPTS),D)
 CFLAGS := -g
 CPP := $(CPP) $(CPPFLAGS)
 PREPROCESS.F := $(CPP) $(CPPFLAGS)
 FFLAGS := -g -NS2000 -qfixed=132
endif
$(MY_OBJ_DIR)/%.o : %.F
	$(CPP) $(CPPFLAGS) $< $(MY_OBJ_DIR)/$(basename $<).f 
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $(MY_OBJ_DIR)/$(basename $<).f
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
endif

ifeq ($(PVM_ARCH),SGI5)
NETCDF_INC := /opt/netcdf-2.3.2/include
NETCDF_LIB := /opt/netcdf-2.3.2/lib
C++ := g++
CC := gcc -ansi
CPPFLAGS := -D$(PVM_ARCH) $(MY_INC_DIR) -I$(NETCDF_INC)
FC := f77
LD := ld
LDFLAGS := -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lnetcdf -lm
LDFLAGS += -lF77 -lI77 -lU77 -lftn
LEX := lex
LINT := lint
YACC := yacc
ifeq ($(OPTS),O)
 CFLAGS := -O2
 FFLAGS := -O2 -e -Nl99 -extend_source
endif
ifeq ($(OPTS),D)
 CFLAGS := -g
 FFLAGS := -g -e -Nl99 -extend_source
endif
$(MY_OBJ_DIR)/%.o : %.F
	$(FC) -c $(FFLAGS) $(CPPFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
endif

ifeq ($(PVM_ARCH),SGI64)
NETCDF_INC := /fs/local/include64
NETCDF_LIB := /fs/local/lib64
C++ := c++
CC := cc
CPPFLAGS := -D$(PVM_ARCH) $(MY_INC_DIR) -I/usr/local/lib -I$(NETCDF_INC)
FC := f77
LD := ld
LDFLAGS := -64 -mips4 -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lnetcdf
LEX := flex
LINT := lint
YACC := bison
ifeq ($(OPTS),O)
 CFLAGS := -64 -mips4 -O2
 FFLAGS := -64 -mips4 -O2 -extend_source
endif
ifeq ($(OPTS),)
 CFLAGS := -64 -mips4 
 FFLAGS := -64 -mips4 -extend_source
endif
ifeq ($(OPTS),D)
 CFLAGS := -64 -mips4 -g
 FFLAGS := -64 -mips4 -g -extend_source
endif
$(MY_OBJ_DIR)/%.o : %.F
	$(FC) -c $(FFLAGS) $(CPPFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
endif

ifeq ($(PVM_ARCH),SUN4)
NETCDF_INC := /contrib/include
NETCDF_LIB := /contrib/lib
C++ := g++
CC := acc
CPPFLAGS := -D$(PVM_ARCH) $(MY_INC_DIR) -I$(NETCDF_INC)
FC := f77
LD := ld
LDFLAGS := -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lnetcdf -lm
LDFLAGS += -cg92 -L/opt/SUNWspro/SC3.0/lib/cg92 -lF77 -lM77 -lresolv
LEX := lex
LINT := lint
YACC := yacc
ifeq ($(OPTS),O)
 CFLAGS := -O2
 FFLAGS := -fast -e -Nl99
endif
ifeq ($(OPTS),D)
 CFLAGS := -g
 FFLAGS := -g -e -Nl99
endif
$(MY_OBJ_DIR)/%.o : %.F
	$(FC) -c $(FFLAGS) $(CPPFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
endif

ifeq ($(PVM_ARCH),SUN4SOL2)
NETCDF_INC := /contrib/include
NETCDF_LIB := /contrib/lib
C++ := g++
CC := gcc -ansi
CPPFLAGS := -D$(PVM_ARCH) $(MY_INC_DIR) -I$(NETCDF_INC)
FC := f77
LD := ld
LDFLAGS := -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lnetcdf -lsunmath -lsocket -lnsl -lm
LDFLAGS += -lF77 -lM77 -lresolv
LEX := lex
LINT := lint
YACC := yacc
ifeq ($(OPTS),O)
 CFLAGS := -O2
 FFLAGS := -fast -e -Nl99
endif
ifeq ($(OPTS),D)
 CFLAGS := -g
 FFLAGS := -g -e -Nl99
endif
$(MY_OBJ_DIR)/%.o : %.F
	$(FC) -c $(FFLAGS) $(CPPFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
endif

ifeq ($(PVM_ARCH),SUNMP)
NETCDF_INC := /contrib/include
NETCDF_LIB := /contrib/lib
C++ := g++
#CC := gcc -ansi -pedantic
CC := gcc 
CPPFLAGS := -D$(PVM_ARCH) $(MY_INC_DIR) -I$(NETCDF_INC)
FC := f77
LD := ld
LDFLAGS := -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lnetcdf -lsunmath -lthread -lsocket -lnsl -lm
LDFLAGS += -lF77 -lM77 -lresolv
LEX := flex
LINT := lint
YACC := bison
ifeq ($(OPTS),O)
 CFLAGS := -O2
 FFLAGS := -fast -e -Nl99
endif
ifeq ($(OPTS),D)
 CFLAGS := -g
 FFLAGS := -g -e -Nl99
endif
$(MY_OBJ_DIR)/%.o : %.F
	$(FC) -c $(FFLAGS) $(CPPFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
endif

ifeq ($(PVM_ARCH),WIN32)
NETCDF_INC := /usr/local/include
NETCDF_LIB := /usr/local/lib
C_ONLY := Y
C++ := g++
CC := gcc -ansi
# NB: nameser.h needs -Di386, but gcc is sending -Di586 (on pentiums)
CPPFLAGS := -D$(PVM_ARCH) $(MY_INC_DIR) -Di386 -I$(NETCDF_INC)
FC := g77
LD := ld
LDFLAGS := -L$(MY_LIB_DIR) -lnco -L$(NETCDF_LIB) -lnetcdf -lm
LDFLAGS += 
LEX := flex
LINT := lint
YACC := bison
ifeq ($(OPTS),O)
 CFLAGS := -O
endif
ifeq ($(OPTS),D)
 CFLAGS := -g
endif
$(MY_OBJ_DIR)/%.o : %.F
	$(FC) -c $(FFLAGS) $(CPPFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
$(MY_OBJ_DIR)/%.o : %.f
	$(FC) -c $(FFLAGS) -o $(MY_OBJ_DIR)/$(notdir $@) $<
endif

ifeq ($(C_ONLY),Y)
 CPPFLAGS += -DC_ONLY
endif

all: obj_dir lib_dir libnco ncks ncrename ncra ncdiff ncwa ncecat ncflint ncatted
# .PHONY tells make to remake the following non-file targets 
.PHONY: all clean distclean debug 

obj_dir :
	- if test -d $(MY_OBJ_DIR); then echo ; else mkdir $(MY_OBJ_DIR); fi

lib_dir :
	- if test -d $(MY_LIB_DIR); then echo ; else mkdir $(MY_LIB_DIR); fi

libnco := $(MY_LIB_DIR)/libnco
libnco : $(libnco).a
ifeq ($(C_ONLY),Y)
$(libnco).a : $(libnco).a($(MY_OBJ_DIR)/nc_utl.o) \
	$(libnco).a($(MY_OBJ_DIR)/csz.o)
else
$(libnco).a : $(libnco).a($(MY_OBJ_DIR)/nc_utl.o) \
	$(libnco).a($(MY_OBJ_DIR)/nc_fortran.o) \
	$(libnco).a($(MY_OBJ_DIR)/cal_util.o) \
	$(libnco).a($(MY_OBJ_DIR)/csz.o)
endif

ncra:	$(MY_OBJ_DIR)/ncra.o $(libnco).a
	$(CC) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(LDFLAGS)
	chmod 755 $(MY_BIN_DIR)/ncra
	cd $(MY_BIN_DIR)
	rm -f $(MY_BIN_DIR)/ncea
	ln -s $(MY_BIN_DIR)/ncra $(MY_BIN_DIR)/ncea
	rm -f $(MY_BIN_DIR)/ncrcat
	ln -s $(MY_BIN_DIR)/ncra $(MY_BIN_DIR)/ncrcat

ncatted:	$(MY_OBJ_DIR)/ncatted.o $(libnco).a
	$(CC) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/$@

ncks:	$(MY_OBJ_DIR)/ncks.o $(libnco).a
	$(CC) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/$@

ncwa:	$(MY_OBJ_DIR)/ncwa.o $(libnco).a
	$(CC) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/$@

ncecat:	$(MY_OBJ_DIR)/ncecat.o $(libnco).a
	$(CC) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/$@

ncdiff:	$(MY_OBJ_DIR)/ncdiff.o $(libnco).a
	$(CC) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/$@

ncflint:	$(MY_OBJ_DIR)/ncflint.o $(libnco).a
	$(CC) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/$@

ncrename:	$(MY_OBJ_DIR)/ncrename.o $(libnco).a
	$(CC) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/$@

Srcfiles: 
	rm -f $(MY_BLD_DIR)/Srcfiles
	printf "$(SRC_STB_FL)" > $(MY_BLD_DIR)/Srcfiles

Depends: Filepath Srcfiles 
	rm -f $(MY_BLD_DIR)/Depends
	cd $(MY_BLD_DIR); makdep -t $(MY_OBJ_DIR) Filepath Srcfiles > Depends

data:	$(MY_DAT_DIR)/in.nc 
$(MY_DAT_DIR)/in.nc : $(MY_DAT_DIR)/in.cdl
	cd $(MY_DAT_DIR); ncgen -b -o in.nc in.cdl
	cd $(MY_DAT_DIR); ln -s in.nc 85.nc
	cd $(MY_DAT_DIR); ln -s in.nc 86.nc
	cd $(MY_DAT_DIR); ln -s in.nc 87.nc
	cd $(MY_DAT_DIR); ln -s in.nc 88.nc
	cd $(MY_DAT_DIR); ln -s in.nc 89.nc
	cd $(MY_DAT_DIR); ln -s in.nc h0001.nc
	cd $(MY_DAT_DIR); ln -s in.nc h0002.nc
	cd $(MY_DAT_DIR); ln -s in.nc h0003.nc

doc:	$(MY_DOC_DIR)/nco.dvi $(MY_DOC_DIR)/nco.ps $(MY_DOC_DIR)/nco.html $(MY_DOC_DIR)/nco.info   
# Each make directive line spawns a separate shell so must use `cd dir;cmd' format
$(MY_DOC_DIR)/nco.ps : $(MY_DOC_DIR)/nco.texi
	cd $(MY_DOC_DIR); dvips -o nco.ps nco.dvi
$(MY_DOC_DIR)/nco.dvi : $(MY_DOC_DIR)/nco.texi
	cd $(MY_DOC_DIR); texi2dvi nco.texi
$(MY_DOC_DIR)/nco.info : $(MY_DOC_DIR)/nco.texi
	cd $(MY_DOC_DIR); makeinfo nco.texi
$(MY_DOC_DIR)/nco.html : $(MY_DOC_DIR)/nco.texi
	cd $(MY_DOC_DIR); texi2html -monolithic -verbose nco.texi

clean:
	rm -f $(MY_OBJ_DIR)/*.o 
	cd $(MY_DOC_DIR); rm -f nco.aux nco.cp nco.cps nco.fn nco.ky nco.log nco.pg nco.toc nco.tp nco.vr 

distclean: clean
	rm -f $(MY_BIN_DIR)/nc*
	rm -f $(MY_LIB_DIR)/libnco*
	cd $(MY_DOC_DIR); rm -f nco.info* nco.dvi nco.html* nco.ps *~
	cd $(MY_DAT_DIR); rm -f in.nc h000?.nc 8?.nc *~
	cd $(MY_BLD_DIR); rm -f *~

debug:
	printf "VPATH = $(VPATH)\n"
	printf "SRC_LST = $(SRC_LST)\n"
	printf "SRC_STB_LST = $(SRC_STB_LST)\n"
	printf "SRC_STB_FL = $(SRC_STB_FL)\n"
	printf "CRM_OBJ = $(CRM_OBJ)\n"

# Reset internal YACC and LEX patterns
%.c : %.y
%.c : %.l

# It is safest to do both YACC and LEX after either file changes. 
# Otherwise only changing one and then switching, e.g., from bison to yacc, can cause problems.
# NB: Bison has a problem when bison.simple declares yyparse() as int yyparse (void);
# The solution is to comment out that definition in bison.simple
ifeq ($(YACC),bison)
%.tab.c %.tab.h : %.y
	$(YACC) -d $<
endif
ifeq ($(YACC),yacc)
%.tab.c %.tab.h : %.y
	$(YACC) -d $<
	mv y.tab.c $(basename $<).tab.c
	mv y.tab.h $(basename $<).tab.h
endif

%_lex.c : %.l
	$(LEX) $<
	mv lex.yy.c $(basename $<)_lex.c

# For some reason, ncap_lex.c is not remade when I expect it to be, so I explicitly remove the object file every time
ncap:	$(MY_OBJ_DIR)/ncap.tab.o ncap.tab.h $(MY_OBJ_DIR)/ncap_lex.o $(MY_OBJ_DIR)/ncap.o $(MY_OBJ_DIR)/ncap_utl.o $(MY_OBJ_DIR)/getopt.o $(MY_OBJ_DIR)/getopt1.o $(libnco).a
#        $(YACC) --name-prefix=nc_
ifeq ($(PVM_ARCH),LINUX)
	$(CC) $(CFLAGS) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(MY_OBJ_DIR)/ncap_utl.o $(MY_OBJ_DIR)/getopt.o $(MY_OBJ_DIR)/getopt1.o $(MY_OBJ_DIR)/$@.tab.o $(MY_OBJ_DIR)/$@_lex.o $(LDFLAGS)
else	
	$(CC) $(CFLAGS) -o $(MY_BIN_DIR)/$@ $(MY_OBJ_DIR)/$@.o $(MY_OBJ_DIR)/ncap_utl.o $(MY_OBJ_DIR)/getopt.o $(MY_OBJ_DIR)/getopt1.o $(MY_OBJ_DIR)/$@.tab.o $(MY_OBJ_DIR)/$@_lex.o -ll -ly $(LDFLAGS)
endif
	chmod 755 $(MY_BIN_DIR)/$@
	/bin/rm -f $@.tab.c $@.tab.h $@_lex.c $(MY_OBJ_DIR)/$@_lex.o

GENNET_SRC := gennet.f
GENNET_OBJ := $(MY_OBJ_DIR)/gennet.o
gennet:	$(GENNET_OBJ)
	$(F77) $(CPP_FLAGS) -o $(MY_BIN_DIR)/gennet $(GENNET_OBJ) $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/gennet

READNET_SRC := readnet.f
READNET_OBJ := $(MY_OBJ_DIR)/readnet.o
readnet:	$(READNET_OBJ)
	$(F77) $(CPP_FLAGS) -o $(MY_BIN_DIR)/readnet $(READNET_OBJ) $(LDFLAGS) 
	chmod 755 $(MY_BIN_DIR)/readnet

include Depends






