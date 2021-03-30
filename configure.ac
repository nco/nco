# $Header$ -*-Shell-script-*-

# Purpose: Generate NCO configure script using autoconf build-engine
# Newer autoconf convention names this file configure.ac, formerly configure.in

# Copyright (C) 2003--present Charlie Zender
# License: 3-Clause BSD License
# See https://opensource.org/licenses/BSD-3-Clause for full license text

# Usage:
# autoconf
# cd ~/nco;aclocal;autoheader;automake --foreign;autoconf
# cd ~/nco;make distclean;aclocal;autoheader;automake --foreign;autoconf;configure;make install
# cd ~/nco;autoreconf --force
# cd ~/nco;for fl in NEWS README AUTHORS ChangeLog ; do ln -s doc/${fl} ${fl};done;autoreconf --force

# Distribution:
# scp ~/nco/configure givre.ess.uci.edu:nco
# scp ~/nco/configure gplogin2.ess.uci.edu:nco

# NB: Different GNU toolchain versions produce different (though equivalent)
# configure scripts from the same autotools inputs.
# To keep proliferation of different but equivalent versions of automatically
# generated scripts (e.g., configure, Makefile.in) from cluttering repository,
# please try to generate scripts only from current autotools distributions
# on machines running an up-to-date version of a Debian-flavor of Linux, 
# preferably Sid or Ubuntu

# GNU autotools rely on ~/nco/autobld/config.[guess,sub]
# Auto-configuration fails if these routines cannot detect your machine type
# This happens periodically as newer machines are introduced
# Notify the NCO project if configuration fails in either file
# and we will update the config files from the upstream source,
# ftp://ftp.gnu.org/pub/gnu/config/config.[guess,sub]

# Autotools glossary:
# AC_ARG_ENABLE(foo,...) Create configure option --enable-foo
# AC_ARG_VAR Add token TKN to influential environment variables ingested by configure, reported by configure --help
# AC_CHECK_DECL Find token TKN defined in header, define ${flg} accordingly
# AC_CHECK_FUNC Find fnc_nm() in libXXX.a, define ${flg} accordingly
# AC_CHECK_HEADER Find XXX.h and define HAVE_XXX_H
# AC_CHECK_LIB Find libXXX.a, prepend -lXXX to LIBS, define HAVE_LIBXXX
# AC_CHECK_PROG Find whether prg_nm is on ${PATH}, define ${flg} accordingly
# AC_DEFINE Define token TKN to value VAL with comment CMNT in config.h
# AC_DEFINE_UNQUOTED Define token TKN to value VAL with comment CMNT in config.h. Use when TKN or VAL are shell variables.
# AC_EGREP_HEADER Search CPP output of header.h (NB: CPP outputs C-code only), define ${flg} accordingly
# AC_MSG_NOTICE Print MSG to output
# AC_MSG_WARN Print MSG to output
# AC_SUBST fxm
# NCO_CHECK_FUNC Sets HAVE_XXX = 0 _and_ NEED_XXX = 1 when xxx() not found

# Process configure input with autoconf to produce configure script
# (package name,version,bug-report-address,tarball name)
# 20170808: Choose between traditional version and Git-based version
# If Git, consider that Linux dynamic libraries will have full messy name, MacOSX will not
# https://stackoverflow.com/questions/43526939/how-to-insert-git-based-version-in-autoconf-managed-project
AC_INIT([NCO netCDF Operators],[4.9.9-alpha02],[nco-bugs@lists.sourceforge.net],[nco])

# Print GNU copyright in configure script
AC_COPYRIGHT

# Safety check that correct --srcdir was specified
AC_CONFIG_SRCDIR([src/nco/ncks.c])
# Put helper scripts here to reduce top level clutter
AC_CONFIG_AUX_DIR(autobld)
# Tell system about local m4 macros to keep libtool in sync
AC_CONFIG_MACRO_DIR([m4])

# Get build system parameters, need config.guess and config.sub to use these
# Set $build
AC_CANONICAL_BUILD
# Set $host = GNU canonical "triplet" =  GNU_TRP
AC_CANONICAL_HOST
# Set $target
AC_CANONICAL_TARGET

# Get NCO version
if test -r doc/VERSION; then
#    NCO_VERSION=`cat doc/VERSION`
    NCO_VERSION=`git describe --abbrev=7 --dirty --always --tags`
    AC_SUBST(NCO_VERSION)
fi

# Set build-engine
AC_DEFINE(NCO_BUILDENGINE,Autoconf,System used to build/install NCO)

# Set egrep
AC_PROG_EGREP

# Get hostname and user
if test "${USER}"; then
    nco_user=${USER};
elif test "${LOGNAME}"; then
    nco_user="${LOGNAME}";
else
    nco_user="nobody";
fi
AC_DEFINE_UNQUOTED(USER,${nco_user},User)
if test "${HOSTNAME}"; then
    nco_hostname="${HOSTNAME}"
# following line $HOST will not be set (from AC_CANONICAL_HOST - will set $host
elif test "${HOST}"; then
    nco_hostname="${HOST}"
else
    nco_hostname="nowhere";
fi

AC_DEFINE_UNQUOTED(HOST,${nco_hostname},Hostname)
AC_DEFINE_UNQUOTED(HOSTNAME,${nco_hostname},Hostname alias)
AC_DEFINE_UNQUOTED(GNU_TRP,${build},Autoconf-generated architecture \"triplet\")

# Begin pvmgetarch Compatibility
# Compatibility map between pvmgetarch and GNU triplets
# Maintained as master list of architectures on which NCO is known to work
# Define boolean variables for some architecture-specific clauses in configure.in
case ${host} in
    alpha*-dec-osf* ) AC_DEFINE(LINUXALPHA,1,Compatibility pvmgetarch token for OSF on DEC Alpha)
	;;
    alpha*-*-linux* ) AC_DEFINE(LINUXALPHA,1,Compatibility pvmgetarch token for Linux on DEC Alpha)
	;;
# e.g., alphaev5-cray-unicosmk2.0.5.X at NIWA
    alpha*-cray-unicos* ) AC_DEFINE(CRAY,1,Compatibility pvmgetarch token for UNICOS on DEC Alpha)
	;;
    hppa*-hp-hpux* ) AC_DEFINE(HPUX,1,Compatibility pvmgetarch token for HPUX on PA RISC)
	;;
    i*86-pc-freebsd* ) AC_DEFINE(FREEBSD,1,Compatibility pvmgetarch token for FreeBSD on Intel x86)
	;;
    i*86-pc-linux* ) AC_DEFINE(LINUX,1,Compatibility pvmgetarch token for Linux on Intel x86) LINUX=1 ;
	;;
    i*86-pc-cygwin* ) AC_DEFINE(WIN32,1,Compatibility pvmgetarch token for Windows on Intel x86)
	;;
    mips*-sgi-irix* ) AC_DEFINE(SGIMP64,1,Compatibility pvmgetarch token for IRIX on SGI MIPS)
	;;
    *-apple-darwin* ) AC_DEFINE(MACOSX,1,Compatibility pvmgetarch token for Apple Mac OSX) MACOSX=1 ;
	;;
    powerpc-ibm-aix* ) AC_DEFINE(AIX,1,Compatibility pvmgetarch token for AIX on IBM PowerPC)
	;;
    powerpc-*-linux* ) AC_DEFINE(LINUXPPC,1,Compatibility pvmgetarch token for Linux on PowerPC)
	;;
    rs6000-ibm-aix* ) AC_DEFINE(AIX,1,Compatibility pvmgetarch token for AIX on IBM RS6000)
	;;
    sparc64-*-linux* ) AC_DEFINE(LINUXSPARC64,1,Compatibility pvmgetarch token for Linux on Sun Sparc)
	;;
    sparc-sun-solaris2* ) AC_DEFINE(SUNMP,1,Compatibility pvmgetarch token for Solaris 2.x on Sun Sparc)
	;;
    sx*-nec-superux* ) AC_DEFINE(NECSX,1,Compatibility pvmgetarch token for SuperUX on NEC SX)
	;;
    x86_64-unknown-cygwin* ) AC_DEFINE(WIN32,1,Compatibility pvmgetarch token for Windows on Intel-compatible x86_64)
	;;
    x86_64*-linux* ) AC_DEFINE(LINUXAMD64,1,Compatibility pvmgetarch token for Linux on AMD x86_64)
	;;
esac
# 20131120: Workaround for Cygwin64
if test "x${host}" = 'xx86_64-unknown-cygwin'; then
    AC_DEFINE(NEED_STRDUP,1,Use NCO strdup on Cygwin)
fi

# End pvmgetarch Compatibility

# Use automake to create Makefiles
AM_INIT_AUTOMAKE
#AM_INIT_AUTOMAKE(nco,"${nco_version}")

# Put preprocessor symbols in config.h
AC_CONFIG_HEADERS(config.h)

# Check for build chain
# CC and CXX have preferred lists
# AC_PROG_CC sets GCC to 'yes' when GCC is detected
# AC_PROG_CC sets CFLAGS = -g [-O2 iff GCC]
# 20061120: Require alternative compilers be passed to configure as environment variables
#AC_PROG_CC( xlc_r xlc acc cc     pathcc pgcc icc  gcc     como c89)
#AC_PROG_CXX(xlC_r xlC aCC CC cxx pathCC pgCC icpc g++ c++ como)
# 20150107: Autotools on FC21 complain that next command requires autobld/compile
AC_PROG_CC
AC_PROG_CXX
AC_PROG_CPP
AC_PROG_CXXCPP
# 20210321 autoupdate suggests but then autoconf fails with this:
#AC_REQUIRE(AC_PROG_CC)
# 20210321 autoconf after autoupdate say AC_PROG_LEX without either yywrap or noyywrap argument is obsolete
AC_PROG_LEX(yywrap)
# AC_PROG_YACC
AC_PROG_INSTALL
AC_PROG_LN_S
LT_INIT

# Until fix for libtool error is found
LIBTOOL="${LIBTOOL} --tag=CXX"
#LIBTOOL="${LIBTOOL} --tag=ignore_tag_err"

# AM_CONDITIONAL(BUILD_YACC_SOURCE,[test "${YACC}" != "yacc" || test -f "src/nco/ncap_yacc.c"])

# Disable maintainer-only options in generated Makefiles unless flag
# --enable-maintainer-mode is given at configure time
AM_MAINTAINER_MODE

# Check for specific library functions and take corrective action when buggy versions found
AC_FUNC_MEMCMP
AC_FUNC_STAT
AC_FUNC_STRTOD

# Check for library functions, if found then #define HAVE_FUNCTION in config.h
AC_CHECK_FUNCS([canonicalize_file_name floor gethostname getpagesize getrusage memchr memmove memset mkstemp pow readlink realpath regcomp setlocale sqrt strchr strerror strpbrk strrchr strstr strtol])

# Check for header files
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_CHECK_HEADERS([getopt.h libintl.h limits.h locale.h netinet/in.h stdlib.h string.h strings.h unistd.h])
AC_LANG_PUSH([C++])
AC_CHECK_HEADERS(cstdlib)
AC_LANG_POP([C++])

# Check for typedefs, structures, and compiler characteristics
# Allow wanton use of 'const' despite compiler
AC_C_CONST
# Allow inline, use __inline__ or __inline, or remove inline as appropriate
AC_C_INLINE
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_TYPE_UID_T
AC_CHECK_TYPES([ptrdiff_t])
AC_CHECK_SIZEOF(int*)
# 20210321 autoconf after autoupdate complains AC_LANG([C++]) is obsolete here, so I delete it
# AC_CXX_HAVE_VALARRAY
# Arrange for large file support (LFS)
AC_SYS_LARGEFILE

echo "#################################"
echo "#  Test for netCDF             #"
echo "################################"

#Mac OSX MacPorts packages
# sudo port install netcdf
# installs to /opt/local/include, /opt/local/lib
# Add this path to CPPFLAGS if it exists, otherwise do nothing
#AC_CHECK_FILE([/opt/local/include/netcdf.h], [CPPFLAGS="$CPPFLAGS -I/opt/local/include"], [])
#AC_CHECK_FILE([/opt/local/lib/libnetcdf.a], [LDFLAGS="$LDFLAGS -L/opt/local/lib -lnetcdf"], [])

# Begin mandatory link test with libnetcdf.a
# Make precious variables for ./configure --help
AC_ARG_VAR([NETCDF_INC],[Directory with netCDF headers (e.g., netcdf.h)])
AC_ARG_VAR([NETCDF_LIB],[Directory with netCDF libraries (e.g., libnetcdf.a)])

# User-specified netCDF locations, if any, take precedence
if test "${NETCDF_ROOT}"; then
    if test -d "${NETCDF_ROOT}"; then
	CPPFLAGS="-I${NETCDF_ROOT}/include ${CPPFLAGS}";
	LDFLAGS="-L${NETCDF_ROOT}/lib ${LDFLAGS}";
    else
	echo "WARNING: NETCDF_ROOT location \"${NETCDF_ROOT}\" does not exist!"
    fi
fi # !NETCDF_ROOT
if test "${NETCDF_LIB}"; then
    if test -d "${NETCDF_LIB}"; then
	LDFLAGS="-L${NETCDF_LIB} ${LDFLAGS} "
    else
	echo "WARNING: NETCDF_LIB location \"${NETCDF_LIB}\" does not exist!"
    fi
fi # !NETCDF_LIB
if test "${NETCDF_INC}"; then
    if test -d "${NETCDF_INC}"; then
	CPPFLAGS="-I${NETCDF_INC} ${CPPFLAGS} "
    else
	echo "WARNING: NETCDF_INC location \"${NETCDF_INC}\" does not exist!"
    fi
fi # !NETCDF_INC

# NCO no longer supports netCDF2
AC_CHECK_HEADERS([netcdf.h],[],[AC_MSG_FAILURE([cannot find netCDF header])])
AC_DEFINE(NO_NETCDF_2,1,No netCDF version 2.x API)
AC_CHECK_LIB([netcdf],[nc_open],[LIBS="-lnetcdf ${LIBS}"],[AC_MSG_FAILURE([cannot find netCDF library])])

echo "#################################"
echo "#  Test for netCDF4 support     #"
echo "#################################"

AC_ARG_ENABLE(netcdf4,AS_HELP_STRING([--enable-netcdf4],[Enable netCDF Version 4 features (same as enable-netcdf-4) [default=yes]]),enable_netcdf4=${enableval},enable_netcdf4=yes)
# 20100105: make --enable-netcdf-4 synonym for --enable-netcdf4
if test "x${enable_netcdf4}" = 'xno' ; then
    AC_ARG_ENABLE(netcdf-4,AS_HELP_STRING([--enable-netcdf-4],[Enable netCDF Version 4 features (same as enable-netcdf4) [default=yes]]),enable_netcdf4=${enableval},enable_netcdf4=yes)  
fi # !netCDF4
AC_ARG_VAR([NETCDF_ROOT],[Root directory of netCDF4 installation (contains bin, include, lib)])

# Test if user set NETCDF_ROOT and it exists
if test "x${enable_netcdf4}" = 'xyes'; then
    if test -z "${NETCDF_ROOT}"; then
	# 20160901: Branch fails on Fedora Core 23+ (not MacOSX, not Ubuntu) due to mysterious emission of "Using <pathname>/nf-config" text as if warning user about security hole
	NETCDF_CONFIG='nc-config'
	NETCDF_ROOT="`nc-config --prefix`"
    else
	NETCDF_CONFIG="${NETCDF_ROOT}/bin/nc-config"
    fi
    LDFLAGS="`${NETCDF_CONFIG} --libs` ${LDFLAGS}";
    CPPFLAGS="`${NETCDF_CONFIG} --cflags` ${CPPFLAGS}";
fi

# ENABLE_NETCDF4 refers to presence of netCDF4-enabled library
# ENABLE_NETCDF4 implies HAVE_NETCDF4_H
# The converse is not true! Some distributions (e.g., Debian stable)
# build _only_ netCDF3 library from netCDF4 source code
AC_CHECK_DECL([NC_CHUNKED],[header_defines_nc_chunked='yes'],[header_defines_nc_chunked='no'],[[#include <netcdf.h>]])
if test "x${header_defines_nc_chunked}" = 'xyes' ; then
    AC_DEFINE([HAVE_NETCDF4_H],[1],[Define to 1 if netcdf.h contains netCDF4 definitions])
    AC_MSG_NOTICE([netcdf.h contains netCDF4 definitions])
fi

if (test "x${enable_netcdf4}" = 'xyes' && test "x${header_defines_nc_chunked}" = 'xyes') ; then
    AC_CHECK_LIB([netcdf],[nc_def_var_chunking],[LIBS="-lnetcdf ${LIBS}"
	    AC_DEFINE([ENABLE_NETCDF4],[1],[Define to 1 if libnetcdf.a contains netCDF4 functions])
	    AC_MSG_NOTICE([Good news: Simple program compiles and links to netCDF4 library])
	    ],[
	    AC_MSG_NOTICE([Bad news: Simple program does not compile and link with netCDF4 library])
	    ])
fi

# 20171218: nc_open_mem() is defined in netCDF >= 4.4.0, however ...
# Ubuntu (Xenial at least) used broken netCDF CMake (not autoconf) to package 4.4.0 (it does not install netcdf_mem.h):
# https://github.com/nco/nco/issues/44
# Symptom of "missing netcdf_mem.h" and/or "unresolved nc_open_mem()" occurs with NCO 4.6.2+
# Until 20171112 we used (Option 1): 
# #if NC_LIB_VERSION >= 440 
# which forces Ubuntu netCDF 4.4.0 users to build netCDF with autoconf and install, e.g., into /usr/local, or
# to manually copy netcdf_mem.h into /usr/include (has anyone tested whether that really solves the problem?)
# Option 2 is to add test/symbol in build-engine, e.g., 
#  #if defined(HAVE_NETCDF_MEM_H)
# which requires additional build tests in Autoconf/CMake/Makefile */
# 20191212 Dave Allured suggests correct procedure is to always include netcdf.h during autoconf header checks. This ensures tokens like EXTERNL (from netcdf.h) are defined in netcdf_mem.h etc.
AC_CHECK_HEADER([netcdf_mem.h],AC_DEFINE([HAVE_NETCDF_MEM_H],1,[Define to 1 if <netcdf_mem.h> is present]),nco_have_netcdf_mem_h=no,
[#ifdef HAVE_NETCDF_H
# include <netcdf.h>
#endif
])

# Begin OpenMP
AC_ARG_ENABLE(openmp,AS_HELP_STRING([--enable-openmp],[Build NCO with OpenMP [default=yes]]),enable_openmp=${enableval},enable_openmp=yes)
# Enable OpenMP unless told not to
if ((test "x${enable_openmp}" != 'xno') && (test "x${enable_netcdf4}" = 'xyes')); then
    AC_OPENMP
	#skatreka++
	# GPU offloading depends upon OpenMP presence
    AC_ARG_ENABLE(gpu,AS_HELP_STRING([--enable-gpu],[Enable OpenMP GPU offloading for Nvidia GPUs [default=no]]),enable_gpu=${enableval},enable_gpu=no)

    if (test "x$enable_gpu" != 'xno'); then
	AC_DEFINE([ENABLE_GPU],[1],[Enable GPU offloading support])
	case ${CC} in
	    gcc* | g\+\+* ) OPENMP_CFLAGS="${OPENMP_CFLAGS} -foffload=nvptx-none" ;;
	    clang* ) OPENMP_CFLAGS="${OPENMP_CFLAGS} -fopenmp-targets=nvptx64-nvidia-cuda" ;;
	    # Intel compilers do not yet support Nvidia GPU offloading
	esac
    fi
    #skatreka--
    AC_SUBST(OPENMP_CFLAGS)
    AC_DEFINE([ENABLE_OPENMP],[1],[Compile operators with OpenMP support])
    AC_MSG_NOTICE([OpenMP support enabled])
    CFLAGS="${CFLAGS} ${OPENMP_CFLAGS}"
    # Oddly, linker needs same OpenMP flag(s) as compiler
    # 20170909 Linux clang on travis does not resolve __kmpc* symbols, apparently needs -lgomp or -lomp
    # LDFLAGS="${LDFLAGS} ${OPENMP_CFLAGS}"
fi
# End OpenMP

# Begin DAP support
echo "Beginning generic tests for DAP support"
# Store pre-DAP LIBS value to restore from later
nco_LIBS_no_DAP="${LIBS}"
AC_ARG_ENABLE(dap,AS_HELP_STRING([--enable-dap],[Build DAP-enabled NCO with netCDF-provided DAP [default=yes]]),enable_dap=${enableval},enable_dap=yes)
# Check for DAP in netCDF library unless told not to
if test "${enable_dap}" != 'no'; then
    echo "Testing for DAP-netCDF support (is DAP in libnetcdf.a?)..."
    AC_CHECK_LIB([netcdf],[daperror],,enable_dap=no,[-lcurl])
    if test "${enable_dap}" = 'no'; then
	AC_MSG_WARN([DAP-netCDF support through libnetcdf.a disabled (missing DAP functions)])
    fi
    if (test "x${enable_dap}" = 'xyes'); then
	AC_DEFINE([ENABLE_DAP],[1],[Compile operators with DAP support from libnetcdf])
	AC_MSG_NOTICE([DAP support enabled])
	LIBS="${LIBS} -lcurl"
# This would activate conditional DAP tests, if there were any
#    AM_CONDITIONAL(TEST_DAP,[test "${enable_dap}" = 'yes'])
    fi
fi
# End DAP

##################################################
# Check libraries, add to link path if found
# fxm: move some checks to OS-specific blocks
##################################################

#AC_CHECK_LIB([C],[cosf])
AC_CHECK_FUNC(getopt_long,,[nco_need_getopt_long='yes'])
AC_CHECK_LIB([f95],[_g95_ac_array],,,[-lm])
AC_CHECK_LIB([m],[sin])
AC_CHECK_LIB([sunmath],[sinf])
AC_CHECK_LIB([intl],[gettext])
AC_CHECK_LIB([resolv],[res_init])
AC_CHECK_LIB([socket],[main])
AC_CHECK_LIB([thread],[main])
AC_CHECK_LIB([nco],[nco_open],[enable_libnco=yes],,)
AM_CONDITIONAL(NCO_NEED_GETOPT_LONG,test "${nco_need_getopt_long}" = 'yes')
# Architecture-dependent library checks
case ${host} in
    hppa*-hp-hpux* | sparc-sun-solaris2* ) 
	AC_CHECK_LIB([nsl],[main]) ;;
esac
# End OS-specific libraries

# NCO_CHECK_FUNC, defined in acinclude.m4, does 
# "#define HAVE_XXX 1" and "#undef NEED_XXX" when function xxx() is found and
# "#undef HAVE_XXX" and "#define NEED_XXX 1" when function xxx() is not found
# Purpose: CPP macros like "if(NEED_XXX)" are simpler to read than "if(!HAVE_xxx)"
NCO_CHECK_FUNCS([nc_inq_format nc_inq_path nc_set_log_level nc_rename_grp])
NCO_CHECK_FUNCS([getopt getopt_long])
NCO_CHECK_FUNCS([atan2])
NCO_CHECK_FUNCS([acosf asinf atanf atan2f cosf expf fabsf fmodf log10f logf powf sinf sqrtf tanf])
NCO_CHECK_FUNCS([erff erfcf gammaf])
NCO_CHECK_FUNCS([acoshf asinhf atanhf coshf sinhf tanhf])
NCO_CHECK_FUNCS([ceilf floorf])
NCO_CHECK_FUNCS([nearbyint rint round trunc])
NCO_CHECK_FUNCS([nearbyintf rintf roundf truncf])
NCO_CHECK_FUNCS([getopt_long])
NCO_CHECK_FUNCS([strdup strcasecmp strsep strcasestr])
NCO_CHECK_FUNCS([htonl htons ntohl ntohs __builtin_bswap16 __builtin_bswap32 __builtin_bswap64])

echo "##########################################################"
echo "#     Sorting out MacOSX build parameters                #"
echo "##########################################################"
#echo "DEBUG: enable_libnco = ${enable_libnco} and MACOSX = ${MACOSX}"
if ((test "${enable_libnco}" = 'yes') && (test "${MACOSX}" != 1)); then
    echo "Phew, NOT MacOSX, adding -lnco to LIBS"
    LIBS="-lnco ${LIBS}"
fi
#echo "DEBUG: After library checks LIBS=$LIBS"
# End Library checks

# Begin ANTLR
echo "#################################"
echo "#  Testing for ANTLR support    #"
echo "#################################"

AC_ARG_VAR(ANTLR_ROOT,Location of ANTLR version 2.7.x installation (compile-time))
AC_ARG_ENABLE(ncoxx,AS_HELP_STRING([--enable-ncoxx],[Build libnco++ and ncap2 (same as enable-ncap2) [default=yes]]),enable_ncoxx=${enableval},[enable_ncoxx=yes])
AC_ARG_ENABLE(ncap2,AS_HELP_STRING([--enable-ncap2],[Build ncap2 and libnco++ (same as enable-ncoxx) [default=yes]]),enable_ncoxx=${enableval},[enable_ncoxx=yes])
if test "${C_INC}"; then
# Prepend ${C_INC} to search path if present
# C_INC refers to a directory that is guaranteed to be searched first
# C_INC is useful on AIX platforms that have alternative C-compilers (e.g., gcc) installed in a directory (e.g., /usr/local) searched for other software (e.g., antlr)
# Specifying both C_INC and ANTLR_INC would cause the pre-processor to grab 
# C-headers from C_INC and antlr headers from ANTLR_INC
# Using C_INC was required on UCI's old ESMF
    CPPFLAGS="-I${C_INC} ${CPPFLAGS}"
fi
AC_LANG_PUSH([C++])
if test "${enable_ncoxx}" != 'no'; then
    if test "${ANTLR_ROOT}"; then
	if test -z "${ANTLR_BIN}"; then
	    ANTLR_BIN="${ANTLR_ROOT}/bin"
	fi
	if test -z "${ANTLR_INC}"; then
	    ANTLR_INC="${ANTLR_ROOT}/include"
	fi
	if test -z "${ANTLR_LIB}"; then
	    ANTLR_LIB="${ANTLR_ROOT}/lib"
	fi
    fi
    if test -d "${ANTLR_BIN}"; then
# Add ${ANTLR_BIN}, if present, to binary search path
	PATH="${PATH}:${ANTLR_BIN}"
    fi
    if test -d "${ANTLR_INC}"; then
# Add ${ANTLR_INC}, if present, to include search path
	CPPFLAGS="${CPPFLAGS} -I${ANTLR_INC}"
    fi
    if test -d "${ANTLR_LIB}"; then
# Add ${ANTLR_LIB}, if present, to library search path
	LDFLAGS="${LDFLAGS} -L${ANTLR_LIB}"
    fi
fi

# AC_CHECK_PROGS (variable, progs-to-check-for, [value-if-not-found], [path]
# Check for antlr executable installed as runantlr (runantlr ~= java antlr.Tool)
AC_CHECK_PROGS(HAVE_ANTLR,[runantlr antlr],no,${PATH},[]) 

#echo "DEBUG: HAVE_ANTLR  = ${HAVE_ANTLR}"

# Issue message if Antlr library was found
if test "${HAVE_ANTLR}" != 'no' ; then
    AC_MSG_NOTICE([antlr library was found])
else
    AC_MSG_NOTICE([antlr library was not found, disabling ncap2 build])
fi

if test "x${HAVE_ANTLR}" = 'xyes'; then
# 20070228: Avoid CHECK_LIB on C++ libraries (requires too-complex link-test scripts)
# AC_CHECK_LIB([antlr],[ASTFactory],enable_ncoxx=yes,enable_ncoxx=no,[-lstdc++])
	AC_CHECK_HEADER([antlr/CommonToken.hpp],enable_ncoxx=yes,enable_ncoxx=no)
	if test "${enable_ncoxx}" = 'yes'; then
	    echo "INFO: ANTLR development support found---will build nco++ directory and ncap2"
	fi
fi

if test "${enable_ncoxx}" != 'yes'; then
    echo "INFO: ANTLR support not found. To build ncap2, please install ANTLR and its C++ development package, e.g., 'sudo aptitude install antlr libantlr-dev' or 'sudo yum install antlr antlr-c++-devel'"
fi
AC_LANG_POP([C++])
# end ANTLR

# Begin MPI (deprecated)
AC_ARG_ENABLE(mpi,AS_HELP_STRING([--enable-mpi],[Build NCO for Message Passing Interface (NB: Do not use, this is not yet supported) [default=no]]),enable_mpi=${enableval},enable_mpi=no)
AM_CONDITIONAL(ENABLE_MPI,[test "${enable_mpi}" = 'yes'])
# End MPI

# Begin Fortran (obsolete)
# If --enable-fortran define USE_FORTRAN_ARITHMETIC
AC_ARG_ENABLE(fortran,AS_HELP_STRING([--enable-fortran],[Use Fortran arithmetic (deprecated) [default=no]]),enable_fortran=${enableval},enable_fortran=no)
if test "${enable_fortran}" = 'yes'; then
    AC_DEFINE(USE_FORTRAN_ARITHMETIC,1,Use Fortran arithmetic)
fi
# End Fortran

# Begin i18n
# i18n support (experimental, for future use)
AC_ARG_VAR([I18N_SHARE],[Root of internationalization (i18n) locale directories (used at run-time)])
# Add user-specified netCDF locations, if any, else add /usr/local/ locations if they exist
if test "${I18N_SHARE}"; then
    if test -d "${I18N_SHARE}"; then
	echo "I18N_SHARE directory exists"
    else
	echo "WARNING: I18N_SHARE location \"${I18N_SHARE}\" does not exist!"
    fi
else
    if test -d '/usr/share/locale'; then
	I18N_SHARE='/usr/share/locale'
    fi
fi
AC_ARG_ENABLE(i18n,AS_HELP_STRING([--enable-i18n],[Internationalization (i18n) support (WARNING: Experimental, for future use) [default=no]]),enable_i18n=${enableval},enable_i18n=no)

# Check for i18n libraries unless told otherwise
if test "${enable_i18n}" != 'no'; then
# These must all be present for i18n to work. If all present, enable_i18n=yes
    AC_CHECK_LIB([intl],[main],nco_have_libintl=yes,)
    if test "${enable_i18n}" = 'no'; then
	AC_MSG_WARN([i18n support disabled (missing libraries)])
    elif test "${enable_i18n}" = 'yes'; then
	AC_DEFINE(I18N,1,i18n support requested)
	AC_MSG_NOTICE([i18n support enabled (WARNING: Experimental, for future use)])
	LIBS="-lmfhdf -li18n ${LIBS}"
    fi
fi
# End i18n

# Begin largefile
# If --enable-largefile define ENABLE_LARGEFILE
# fxm: TODO bld45 Obsolete! Replace with test for 64-bit offset support (i.e., netCDF 3.6)
if test "${enable_largefile}" = 'yes'; then
    echo "This approach to large file support (LFS) is deprecated; use only if netCDF 3.6 is unavailable..."
    AC_DEFINE(ENABLE_LARGEFILE,1,Enable Large File Support (LFS))
fi
# End largefile

# Begin nco_c++
# If --enable-nco_cplusplus, compile NCO C++ interface library
AC_ARG_ENABLE(nco_cplusplus,AS_HELP_STRING([--enable-nco_cplusplus],[Build NCO C++ interface library [default=yes]]),enable_nco_cxx=${enableval},enable_nco_cxx=yes)
# End nco_c++

echo "########################################"
echo "#     Testing for GSL support          #"
echo "########################################"
# Begin GSL
# NB: Desirable for GSL flags to follow netCDF flags so configure does not look in GSL location (often /usr) for netCDF
# Store pre-GSL LIBS value to restore from later
nco_LIBS_no_GSL="${LIBS}"
nco_CPPFLAGS_no_GSL="${CPPFLAGS}"
AC_ARG_ENABLE(gsl,AS_HELP_STRING([--enable-gsl],[Build-in GSL support if possible [default=yes]]),enable_gsl=${enableval},enable_gsl=yes)
if test "${enable_gsl}" = 'yes'; then

    if test "${GSL_ROOT}"; then
	GSL_CONFIG="${GSL_ROOT}/bin/gsl-config"
	else
	GSL_CONFIG='gsl-config'
	fi

    if which ${GSL_CONFIG} > /dev/null; then
	enable_gsl='yes'
    else
	AC_MSG_NOTICE([WARNING: Did not find 'gsl-config' command in your default path])
	enable_gsl='no'
    fi
fi
# Check for GSL libraries unless told not to
if test "${enable_gsl}" != 'no'; then
# fxm: make non-fatal code path for when cannot find gsl-config
    if test -z "${GSL_ROOT}"; then
	GSL_CONFIG='gsl-config'
	GSL_ROOT="`gsl-config --prefix`"
    else
	GSL_CONFIG="${GSL_ROOT}/bin/gsl-config"
    fi
    if test "${GSL_INC}"; then
	CPPFLAGS="${CPPFLAGS} -I${GSL_INC}"
    else
	GSL_INC="${GSL_ROOT}/include"
	GSL_INC_ARG="`${GSL_CONFIG} --cflags`"
	CPPFLAGS="${CPPFLAGS} ${GSL_INC_ARG}"
    fi
    if test "${GSL_LIB}"; then
	LIBS="${LIBS} -L${GSL_LIB}"
    fi
	LIBS="${LIBS} `${GSL_CONFIG} --libs`"
    AC_MSG_NOTICE([These GSL library and header tests must succeed for GSL support:])
# fxm: default action of check_lib adds a superfluous -lgsl to $LIBS
    AC_CHECK_LIB([gsl],[gsl_sf_gamma_inc],,enable_gsl=no)
    if test "${GSL_ROOT}"; then
	AC_CHECK_FILE([${GSL_INC}/gsl/gsl_sf_gamma.h],AC_DEFINE([HAVE_GSL_H],1,[Define to 1 if <gsl/gsl_sf_gamma.h> is present]),enable_gsl=no)
    else
	AC_CHECK_HEADER([gsl/gsl_sf_gamma.h],AC_DEFINE([HAVE_GSL_H],1,[Define to 1 if <gsl_sf_gamma.h> is present]),enable_gsl=no)
    fi
    if test "${enable_gsl}" = 'no'; then
	AC_MSG_WARN([GSL support disabled (missing/unusable library or header file)])
	LIBS="${nco_LIBS_no_GSL}"  
	CPPFLAGS="${nco_CPPFLAGS_no_GSL}"
    elif test "${enable_gsl}" = 'yes'; then
# Assumes GSL version in form 1.[0-9]+  extract minor version only   
	GSL_VER="`${GSL_CONFIG} --version`" 
	GSL_MAJOR="`echo "${GSL_VER}" | cut -d '.' -f 1`"
	GSL_MINOR="`echo "${GSL_VER}" | cut -d '.' -f 2`"
        GSL_VER=`expr $GSL_MAJOR \* 100 + $GSL_MINOR`  
	if test "x${GSL_VER}" = 'x'; then
	    # 20100115: Fix for machines like bluefire that lack gsl-config
	    GSL_VER='104' 
            GSL_MAJOR='1'
            GSL_MINOR='4'
	fi
	AC_DEFINE(ENABLE_GSL,1,Compile operators with GSL support)
	AC_DEFINE_UNQUOTED(NCO_GSL_MINOR_VERSION,${GSL_MINOR},GSL minor version number)
	AC_DEFINE_UNQUOTED(NCO_GSL_MAJOR_VERSION,${GSL_MAJOR},GSL major version number)
	AC_DEFINE_UNQUOTED(NCO_GSL_VERSION,${GSL_VER},GSL version number)
	AC_MSG_NOTICE([GSL support enabled])
    fi
fi
AM_CONDITIONAL(ENABLE_GSL,[test "${enable_gsl}" = 'yes'])
#echo "DEBUG: After GSL tests LIBS = ${LIBS}"
# This would activate conditional GSL tests, if there were any
# AM_CONDITIONAL(TEST_GSL,[test "${enable_gsl}" = 'yes'])
# End GSL

echo "########################################"
echo "#     Testing for CCR support          #"
echo "########################################"
# Begin CCR
# If the env. variable HDF5_PLUGIN_PATH is set, or if
# --with-hdf5-plugin-dir=<directory>, use it as the plugin
# directory. This is necessary at the top level to provide the help
# message and --with option. If used, the option will be passed to the
# subdirs, and also handled by the configure in each filter
# subdirectory.
AC_MSG_CHECKING([where to put HDF5 plugins])
AC_ARG_WITH([hdf5-plugin-path],
            [AS_HELP_STRING([--with-hdf5-plugin-path=<directory>],
                            [specify HDF5 plugin path (defaults to /usr/local/hdf5/lib/plugin, or value of HDF5_PLUGIN_PATH, if set)])],
            [HDF5_PLUGIN_PATH=$with_hdf5_plugin_path])
HDF5_PLUGIN_PATH=${HDF5_PLUGIN_PATH-.}
AC_MSG_RESULT($HDF5_PLUGIN_PATH)

AC_MSG_CHECKING([whether Community Codec Repository library should be used])
AC_ARG_ENABLE([ccr],AS_HELP_STRING([--enable-ccr],[Build-in CCR support if possible [default=no]]),enable_ccr=${enableval},enable_ccr=no)
test "x$enable_ccr" = xyes || enable_ccr=no
AC_MSG_RESULT([$enable_ccr])

# Check for CCR library
if test "x$enable_ccr" = xyes; then
    CPPFLAGS_backup="${CPPFLAGS}"
    LIBS_backup="${LIBS}"
    LDFLAGS_backup="${LDFLAGS}"
    LDFLAGS="-L${HDF5_PLUGIN_PATH} ${LDFLAGS}"
    AC_CHECK_HEADER([ccr.h],AC_DEFINE([HAVE_CCR_H],1,[Define to 1 if <ccr.h> is present]),nco_have_ccr=no)
    AC_SEARCH_LIBS([nc_def_var_bitgroom], [ccr], [enable_ccr=yes], [AC_MSG_NOTICE([Cannot find or link to the CCR library, set CPPFLAGS/LDFLAGS])])
    test "x$enable_ccr" = xyes || enable_ccr=no
    if test "x$enable_ccr" = xno; then
	AC_MSG_WARN([CCR support disabled (missing/unusable library or header file)])
	CPPFLAGS="${CPPFLAGS_backup}"
	LIBS="${LIBS_backup}"  
	LDFLAGS="${LDFLAGS_backup}"  
    fi
    if test "x$enable_ccr" = xyes; then
	AC_DEFINE(ENABLE_CCR,1,Compile operators with CCR support)
    fi
fi
AM_CONDITIONAL(ENABLE_CCR, [test "x$enable_ccr" = xyes])
# End CCR

# Begin rx
# Check to enable regular expression stuff, allow user to override
AC_ARG_ENABLE(regex,AS_HELP_STRING([--enable-regex],[Allow extended regular expressions [default=yes]]),nco_have_regex=${enableval},nco_have_regex=yes)
if test "x${nco_have_regex}" = 'xyes'; then
    AC_CHECK_HEADER([regex.h],AC_DEFINE([HAVE_REGEX_H],1,[Define to 1 if <regex.h> is present]),nco_have_regex=no)
    AC_CHECK_HEADER([sys/types.h],AC_DEFINE([HAVE_SYS_TYPES_H],1,[Define to 1 if <sys/types.h> is present]),nco_have_regex=no)
    AC_CHECK_FUNC([regexec],AC_DEFINE([HAVE_REGEXEC],1,[Define to 1 if 'regexec()' is present]),nco_have_regex=no)
    AC_CHECK_FUNC([regcomp],AC_DEFINE([HAVE_REGCOMP],1,[Define to 1 if 'regcomp()' is present]),nco_have_regex=no)
    AC_CHECK_FUNC([regfree],AC_DEFINE([HAVE_REGFREE],1,[Define to 1 if 'regfree()' is present]),nco_have_regex=no)
fi
if test "x${nco_have_regex}" = 'xyes'; then
    AC_DEFINE(NCO_HAVE_REGEX_FUNCTIONALITY,1,POSIX extended regular expressions available)
fi
# End rx

# - Variable: CPPFLAGS
# Header file search directory (`-IDIR') and any other miscellaneous
# options for the C and C++ preprocessors and compilers.  If it is
# not set in the environment when `configure' runs, the default
# value is empty.  `configure' uses this variable when compiling or
# preprocessing programs to test for C and C++ features.

echo "########################################"
echo "#     Testing for UDUNITS support      #"
echo "########################################"

AC_ARG_ENABLE(udunits,AS_HELP_STRING([--enable-udunits],[Build-in UDUnits support if possible [default=no]]),enable_udunits=${enableval},enable_udunits=yes)
AC_ARG_ENABLE(udunits2,AS_HELP_STRING([--enable-udunits2],[Build-in UDUnits2 support if possible [default=yes]]),enable_udunits2=${enableval},enable_udunits2=yes)
# Are both UDUnits and UDUnits2 requested?
if (test "${enable_udunits}" != 'no' && test "${enable_udunits2}" != 'no'); then
# Override UDUnits request
    echo "WARNING: Requested both UDUnits and UDUnits2 support---overriding UDUnits"
    echo "Will attempt to build NCO with UDUnits support from libudunits2, not from libudunits"
    enable_udunits='no'
fi # End test for UDUnits duplication

# Begin UDUnits2
# Check for UDUnits2 libraries unless told not to
if test "${enable_udunits2}" != 'no'; then
    AC_CHECK_LIB([expat],[XML_ParserCreate],,)
    if test "${UDUNITS2_PATH}" != ''; then
	if test -d "${UDUNITS2_PATH}"; then
	    # NB: Use ${UDUNITS2_PATH} not ${UDUNITS2_ROOT} because UDUnits software looks for ${UDUNITS2_PATH}
	    # Add ${UDUNITS2_PATH}/lib to search path if present
	    # Some systems install 64bit libraries into $prefix/lib64 instead of $prefix/lib
	    # If NCO libraries were installed in $prefix/lib64, then UDUNITS presumably was installed in $UDUNITS2_PATH/lib64
	    # Therefore, we take `basename $libdir` (libdir=$prefix/lib|lib64), which is 'lib' or 'lib64', and append it to UDUNITS2_PATH
	    # If this directory does not exist, we append 'lib' to UDUNITS2_PATH
	    # (Daniel Neumann, 3rd June 2017)
	    nco_udunits2_xml=${UDUNITS2_PATH}/share/udunits/udunits2.xml
	    if test -d "${UDUNITS2_PATH}/`basename $libdir`"; then
		LDFLAGS="${LDFLAGS} -L${UDUNITS2_PATH}/`basename $libdir`"
	    else
		LDFLAGS="${LDFLAGS} -L${UDUNITS2_PATH}/lib"
	    fi
	    CPPFLAGS="${CPPFLAGS} -I${UDUNITS2_PATH}/include"
	else
	    echo "WARNING: UDUNITS2_PATH location \"${UDUNITS2_PATH}\" does not exist!"
	fi # !UDUNITS2_PATH
    elif test "${NETCDF_INC}"; then
	nco_udunits2_xml="${NETCDF_INC}/../share/udunits/udunits2.xml"
    fi # !UDUNITS2_PATH
    if test "${UDUNITS2_PATH}" = ''; then
	# NO UDUNITS2_PATH section
	# Fedora 1.7 has /usr/include/udunits2/udunits2.h
	# 20160418 FC23 has /usr/include/udunits2/udunits2.h
	# Add this path to CPPFLAGS if it exists, otherwise do nothing
	AC_CHECK_FILE([/usr/include/udunits2/udunits2.h], [CPPFLAGS="$CPPFLAGS -I/usr/include/udunits2"], [])
	# 20170113 FC25 has /usr/include/udunits2/udunits2.h
	# Ubuntu
	# /usr/lib/x86_64-linux-gnu/libudunits2.a
	# /usr/lib/i386-linux-gnu/libudunits2.a
	# /usr/include/udunits2.h
	AC_CHECK_FILE([/usr/include/udunits2.h], [CPPFLAGS="$CPPFLAGS -I/usr/include"], [])
	AC_CHECK_FILE([/usr/lib/x86_64-linux-gnu/libudunits2.a], [LDFLAGS="$LDFLAGS -L/usr/lib/x86_64-linux-gnu"], [])
	AC_CHECK_FILE([/usr/lib/i386-linux-gnu/libudunits2.a], [LDFLAGS="$LDFLAGS -L/usr/lib/i386-linux-gnu"], [])
	# Mac OSX MacPorts packages
	# sudo port install libdap
	# sudo port install gsl
	# sudo port install antlr
	# sudo port install udunits2
	# sudo port install netcdf
	# installs to /opt/local/include/udunits2/udunits2.h
	# /opt/local/share/udunits/udunits2.xml
	# Add this path to CPPFLAGS if it exists, otherwise do nothing
	AC_CHECK_FILE([/opt/local/include/udunits2/udunits2.h], [CPPFLAGS="$CPPFLAGS -I/opt/local/include/udunits2"], [])
	#/opt/local/lib/libudunits2.a
	AC_CHECK_FILE([/opt/local/lib/libudunits2.a], [LDFLAGS="$LDFLAGS -L/opt/local/lib"], [])
	AC_CHECK_FILE([/opt/local/lib/libudunits2.dylib], [LDFLAGS="$LDFLAGS -L/opt/local/lib"], [])
	# 20201001 MacPorts installs /opt/local/lib/libudunits2.dylib not /opt/local/lib/libudunits2.a 
	# 20101011 Debian installs udunits2.xml in /usr/share/xml/udunits/udunits2
	# NB: 20100729 udunits2.xml location seems to have changed from 
	# /usr/local/share/udunits2.xml (in Fedora Core 6) to /usr/local/share/udunits/udunits2.xml
	# Not sure of location in more recent Fedora systems...
	# Updating patch from Fedora Core 6 nco.spec:
	for udunits2_data_file in /usr/share/xml/udunits/udunits2/udunits2.xml /usr/local/share/udunits/udunits2.xml /usr/local/share/udunits2.xml /share/udunits2.xml /usr/share/udunits/udunits2.xml; do
	    if test -e "$udunits2_data_file" ; then
		nco_udunits2_xml="$udunits2_data_file"
	    fi
	done
    fi # end NO UDUNITS2_PATH section

# Warn if udunits2.xml is not found, but continue anyway since builder may be savvy enough to add it when building packages (e.g., .debs)
    AC_CHECK_FILE([$nco_udunits2_xml],,AC_MSG_WARN(file UDUNITS2 XML database file $nco_udunits2_xml was not found but will still attempt to build NCO with UDUnits2 support.))
	# Use DEFINE_UNQUOTED for quoted string so quotes get correctly merged into source file
    AC_DEFINE_UNQUOTED([UDUNITS2_PATH],["$nco_udunits2_xml"],[Location of UDUnits2 data file])

    AC_MSG_NOTICE([NB: Following tests of UDUnits2 library and header must succeed for UDUnits2 support:])
    AC_CHECK_LIB([udunits2],[ut_read_xml],,enable_udunits2=no)
    if test "${UDUNITS2_PATH}"; then
	AC_CHECK_FILE([${UDUNITS2_PATH}/include/udunits2.h],AC_DEFINE([HAVE_UDUNITS2_H],1,[Define to 1 if <udunits2.h> is present]),enable_udunits2=no)
    else
	AC_CHECK_HEADER([udunits2.h],AC_DEFINE([HAVE_UDUNITS2_H],1,[Define to 1 if <udunits2.h> is present]),enable_udunits2=no)
    fi

    if test "${enable_udunits2}" = 'no'; then
	AC_MSG_WARN([UDUnits2 support disabled (missing/unusable library or header file)])
    elif test "${enable_udunits2}" = 'yes'; then
	LIBS="${LIBS} -ludunits2"
	AC_DEFINE(ENABLE_UDUNITS,1,Compile operators with UDUnits2 support)
    AC_DEFINE(HAVE_UDUNITS2_H,1,[])
	AC_MSG_NOTICE([UDUnits2 support enabled])
    fi

# When empty, utInit() uses environment variable UDUNITS2_PATH, if any (see nco_lmt.c)
# Thus UDUNITS2_PATH need not be known at compile time
    AC_ARG_VAR([UDUNITS2_PATH],[Root directory of UDUnits2 (normally contains bin, include, lib, share subdirectories)])
fi

# This would activate conditional UDUnits2 tests, if there were any
# AM_CONDITIONAL(TEST_UDUNITS2,[test "${enable_udunits2}" = 'yes'])
# End UDUnits2

# Begin Default OS-specific Compiler Arguments
# Perform standard additions to compilers and preprocessor flags before testing anything
# NB: These flags take effect regardless of enable-[debug/optimize]-custom setting
case ${host} in
    i*86-*-linux-gnu | x86_64*-linux* )
	case ${CC} in
	    como* ) CFLAGS="${CFLAGS} --c99" ; ;;
# 20150525: fxm Remove -no-gcc from icc flags as per ocehugo
#	    icc* ) CFLAGS="${CFLAGS} -std=c99" ; CPPFLAGS="${CPPFLAGS} -D_BSD_SOURCE -D_POSIX_SOURCE -no-gcc" ; ;;
	    icc* ) CFLAGS="${CFLAGS} -std=c99" ; CPPFLAGS="${CPPFLAGS} -D_BSD_SOURCE -D_POSIX_SOURCE" ; ;;
	    pathcc* ) CFLAGS="${CFLAGS} -std=c99" ; ;;
	    pgcc* ) CFLAGS="${CFLAGS} -c9x" ; CPPFLAGS="${CPPFLAGS} -DPGI_CC" ; ;;
	esac
	case ${CXX} in
#	    icpc* ) CXXFLAGS="${CXXFLAGS} -cxxlib-icc" ; ;;
	    #icpc* ) CXXFLAGS="${CXXFLAGS} -cxxlib-gcc" ; ;;
	esac
	;;
    mips*-sgi-irix* )
	case ${CC} in
	    cc* ) CFLAGS="${CFLAGS} -c99" ; ;;
	esac
	;;
    powerpc-ibm-aix* )
	case ${CC} in
	    gcc* ) CFLAGS="${CFLAGS} -maix64" ; ;;
# -qmaxmem=num Limit memory used by space intensive optimizations to <num> kilobytes
# -qspill=size Size in B of register allocation spill area, mie needs > 1040 B
# -qlanglvl=extc99: Include orthogonal extensions to C99 standard
 		xlc* ) CFLAGS="${CFLAGS} -O3 -g -qstrict -Q -qsmp=omp -qlanglvl=extc99 -qmaxmem=8192 -qspill=2048 -q64" ; LDFLAGS="${LDFLAGS} -lC" ; ;;
#    CPPFLAGS="${CPPFLAGS} -qlanglvl=extended"
	esac
	case ${CXX} in
# -bh:5 suppresses annoying messages from xlC linker WARNING: Duplicate symbol: ...
	    xlC* ) LDFLAGS="${LDFLAGS} -bh:5" ; ;;
	esac
esac
# End Default OS-specific Compiler Arguments

# Begin custom GCC switches
# Perform elaborate "or" test since autoconf dislikes [ -o ] syntax
GCC_OR_GXX='no'
if (test "x${GCC}" = 'xyes'); then
    GCC_OR_GXX='yes'
fi
if (test "x${GXX}" = 'xyes'); then
    GCC_OR_GXX='yes'
fi
if test "x${GCC_OR_GXX}" = 'xyes' ; then
# Explain and set common custom GCC flags once, here, and modify later to suit
# since GCC has same base optimization and debugging flags on all architectures
    
# Compilation flags for numerical routines recommended by GSL 1.3 manual, p. 397
# CFLAGS += -Werror -Wall -W -Wmissing-prototypes -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fno-common -g -O4
# 20070310: Change to no-shadow so ncoLexer.cpp compiles
# 20150115: Deprecate GCC_CMD_ARGS='-std=c99 -pedantic -D_BSD_SOURCE -D_POSIX_SOURCE'

    GCC_CMD_ARGS='-std=c99 -pedantic -D_DEFAULT_SOURCE'
    GCC_BASE_FLAGS='-Wall -Wuninitialized -Wunused -W -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fno-common -g -O4'
    GCC_BASE_FLAGS="${GCC_BASE_FLAGS} ${OPENMP_CFLAGS}"
    GCC_CFLAGS="${GCC_BASE_FLAGS} -Wmissing-prototypes"
    GCC_CXXFLAGS="${GCC_BASE_FLAGS}"

# Compilation flags that I like and use:
# See also GSL optimization guide, and Gentoo https://wiki.gentoo.org/wiki/GCC_optimization
# -D_BSD_SOURCE: Support 4.3 BSD Unix extensions to ANSI C (prevents nameser.h warnings)
# -D_POSIX_SOURCE: Support POSIX.1 standard additions to ANSI C (prevents fileno warnings)
# -pedantic: Disallow non-ISO constructs (including type long long) (sometimes useful)
# -Werror: Consider warnings as errors
# -W: Extra warnings, including missing return values, comparison of signed with unsigned
# -Wall: Warn about common programming problems
# -Wcast-align: Warn if casting pointer to type of different size
# -Wcast-qual: Warn if const qualifier removed from pointer
# -Wmaybe-uninitialized: Warn on uninitialized variables. GCC has many false negatives, clang is better? Available on GCC 4.8.2+. Not available on GCC 4.6.3-.
# -Wmissing-prototypes: Warn if missing prototypes (C only, not C++)
# -Wpointer-arith: Warn if pointer arithmetic on types without size, e.g., void
# -Wshadow: Warn if local variable has same name as other local variable
# -Wsometimes-uninitialized: LLVM/clang uses this but GCC does not
# -Wuninitialized: Warn on uninitialized variables. GCC has many false negatives, clang is better? 
# -Wwrite-strings: Apply const-qualifier to string constants, die if overwritten
# -fno-common: Prevent global variables from being simultaneously defined in different files
# -g: Put debugging symbols in binary executable
# -pg: Enable profiling, generate gmon.out output files (also needed by linker)
# -Og: Optimize while retaining complete compatibility with -g
# -O2: Recommended optimization level for general case, uses SSE and AVX but no YMM
# -O3: clang -O4 DNE, highest clang optimization is -O3 
# -O4: Turn on optimization so uninitialized variables are flagged
# Compilation flags recommended by GSL that I do not like and do not use:
# -ansi: Support only strict ANSI C. Equivalent to -std=c89, conflicts with -std=c99
# --no-alias? -fstrict-aliasing
# -Waggregate-return: Warn if functions return aggregates like structures or unions
# -Wconversion: Warn if converting signed to unsigned. Intended for obsolete, non-prototyped code. Triggers fabsf(), sqrtf(), warnings.
# -Wnested-externs: Warn if extern is encountered within function. C only?
# -Wstrict-prototypes: Warn if inconsistent prototypes. C only?
# -Wtraditional: Warn if constructs differ between traditional and ANSI C. C only?
# -Dinline=: inline is not an ANSI keyword, must undefine inline to work with -ansi
# -fshort-enums: Make enums as short as possible, ususally non-int. Do not ever invoke this! This breaks ABI and causes subtle problems
fi
# End custom GCC switches

# Begin enable_debug_custom
# Custom debug: Activate all known, helpful compile-time and run-time debugging checks
AC_ARG_ENABLE(debug-custom,AS_HELP_STRING([--enable-debug-custom],[Activate all known, helpful compile-time and run-time debugging checks such as pedantic warnings, bounds checking (slowest execution). Automatically activates --enable-debug-symbols. NB: This option may interact unfavorably with some versions of Flex. De-activate this if building fails with with errors involving "yyget_leng()". [default=no]]),enable_debug_custom=${enableval},enable_debug_custom=no)
if test "${enable_debug_custom}" = 'yes'; then
    AC_DEFINE(ENABLE_DEBUG_CUSTOM,1,[Custom debugging: Pedantic, bounds checking (slowest execution)])
    if (test "x${GCC}" = 'xyes'); then
	CC="${CC} ${GCC_CMD_ARGS}"
        GCC_CUSTOM_FLAGS='-g -Wall -Wuninitialized -Wunused -W -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -Wmissing-prototypes -fno-common'
	CFLAGS="${GCC_CUSTOM_FLAGS} ${OPENMP_CFLAGS}"
	if (test "x${CXX}" = 'xg++' && test "x${GXX}" = 'xyes'); then
	    CXXFLAGS="${GCC_CUSTOM_FLAGS}"
	fi
    fi
# Other switches are compiler-specific
    case ${host} in
	alpha*-dec-osf* )
	    case ${CC} in
		cc* ) CFLAGS="${CFLAGS} -check_bounds -check -check_omp" ; ;;
	    esac
	    ;;
	alpha*-cray-unicos* )
	    case ${CC} in
		cc* ) CFLAGS="${CFLAGS} -h rounddiv -h indef -h bounds -h nofastmd -h nofastmodulus" ; ;;
	    esac
	    ;;
	i*86-pc-linux* | x86_64*-linux* )
	    case ${CC} in
		icc* ) CFLAGS="${CFLAGS} -Wall -wd810,981,1572 -inline_debug_info"
		    case ${CXX} in
			icpc* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
		pathcc* ) CFLAGS="${CFLAGS}"
		    case ${CXX} in
			pathCC* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
		pgcc* ) CFLAGS="${CFLAGS} -Mbounds -Minfo=all"
		    case ${CXX} in
			pgCC* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
	    esac
	    ;;
	mips*-sgi-irix* )
	    if test "x${CC}" = xcc; then
		CFLAGS="${CFLAGS} -trapuv"
	    fi
	    ;;
	*-apple-darwin* )
	    ;;
	powerpc-ibm-aix* )
	    case ${CC} in
		xlc* ) CFLAGS="${CFLAGS} -qflttrap -qidirfirst -qlonglong -qwarn64 -qcheck=all -qhalt=s"
		    case ${CXX} in
			xlC* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
	    esac
	    ;;
	rs6000-ibm-aix* )
	    ;;
	sparc-sun-solaris2* )
	    ;;
	sx*-nec-superux* )
	    case ${CC} in
		cc* ) CFLAGS="${CFLAGS} -h0 -hstack=nan" ; ;;
	    esac
	    ;;
    esac
    
# Check for debugging libraries
    AC_CHECK_LIB([ccmalloc],[main],,AC_MSG_WARN([Unable to find ccmalloc library]) )
    AC_CHECK_LIB([dl],[main],,AC_MSG_WARN([Unable to find dl library]) )
    
fi
# End enable_debug_custom

# Begin enable_debug_symbols
# If --enable-debug-symbols, add these compiler flags
AC_ARG_ENABLE(debug-symbols,AS_HELP_STRING([--enable-debug-symbols],[Debugging symbols: Produce symbols for debuggers (e.g., dbx, gdb) [default=no]]),enable_debug_symbols=${enableval},enable_debug_symbols=no)
# Custom debug automatically invokes debugging symbols
if test "${enable_debug_custom}" = 'yes'; then
    enable_debug_symbols='yes';
fi
if test "${enable_debug_symbols}" = 'yes'; then
    AC_DEFINE(ENABLE_DEBUG_SYMBOLS,1,Debugging symbols: Produce symbols for debuggers (e.g., dbx, gdb))
# All known architectures use -g to turn on debugging symbols
    CFLAGS="${CFLAGS} -g"
fi
# End enable_debug_symbols

# Begin enable_optimize_custom
# Activate all known, helpful switches for fastest possible run-time performance
# These switches are highly compiler and architecture dependent
# Settings should improve performance relative to default ./configure setttings
AC_ARG_ENABLE(optimize-custom,AS_HELP_STRING([--enable-optimize-custom],[Activate compiler switches for potentially faster run-time performance (slower compilation). NB: This option may interact unfavorably with some versions of Flex. De-activate this if building fails with with errors involving "yyget_leng()". [default=no]]),enable_optimize_custom=${enableval},enable_optimize_custom=no)
if test "${enable_optimize_custom}" = 'yes'; then
    AC_DEFINE(ENABLE_OPTIMIZE_CUSTOM,1,Fastest possible execution (slowest compilation))
    #echo "DEBUG: x\$CC=x${CC},  x\$GCC=x${GCC}"
    if (test "x${GCC}" = 'xyes'); then
# fxm: TODO #303 go through GCC manual and add more speedy flags
# Until then, GCC has interesting property that --enable-optimize-custom flags equal --enable-optimize-debug flags!
	CC="${CC} ${GCC_CMD_ARGS}"
	CFLAGS="${CFLAGS} ${GCC_CFLAGS}"
	if (test "x${GXX}" = 'xyes'); then
	    CXXFLAGS="${GCC_CXXFLAGS}"
	fi
    fi
    case ${host} in
	alpha*-dec-osf* )
	    case ${CC} in
		cc* ) CFLAGS="${CFLAGS} -O2 -ansi-alias" ; ;;
	    esac
	    ;;
	alpha*-cray-unicos* )
	    case ${CC} in
		cc* ) CFLAGS="${CFLAGS} -h rounddiv -h nofastmd -h nofastmodulus" ; ;;
	    esac
	    ;;
	i*86-pc-linux* )
	    case ${CC} in
		icc* ) CFLAGS="${CFLAGS} -O3 -axW -mcpu=pentium4"
		    case ${CXX} in
			icpc* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
		pathcc* ) CFLAGS="${CFLAGS} -O3 -mmmx -msse -msse2 -m3dnow -pipe -fexpensive-optimizations"
		    case ${CXX} in
			pathCC* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
		pgcc* ) CFLAGS="${CFLAGS} -fast -fastsse -Minfo=all"
		    case ${CXX} in
			pgCC* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
	    esac
	    ;;
	mips*-sgi-irix* )
	    if test "x${CC}" = xcc; then
		CFLAGS="${CFLAGS} -O2"
	    fi
	    ;;
	powerpc-ibm-aix* )
	    case ${CC} in
# -qstrict: Ensure that -O3 optimizations do not alter program semantics
# -Q : Inline all appropriate subprograms
		xlc* ) CFLAGS="${CFLAGS} -O3 -qstrict -Q"
 	            case ${CXX} in
			xlC* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
	    esac
	    ;;
	rs6000-ibm-aix* )
	    case ${CC} in
		cc* ) CFLAGS="${CFLAGS} -O" ; ;;
	    esac
	    ;;
	sparc-sun-solaris2* )
	    case ${CC} in
		cc* ) CFLAGS="${CFLAGS} -O2" ; ;;
	    esac
	    ;;
	sx*-nec-superux* )
	    case ${CC} in
		cc* ) CFLAGS="${CFLAGS} -h2 -hmath vector -hxint" ; ;;
	    esac
	    ;;
	x86_64*-linux* )
	    case ${CC} in
		#skatreka++
		clang* ) CFLAGS="${CFLAGS} -march=native"
		    case ${CXX} in
			clang* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
		gcc* | g\+\+* ) CFLAGS="${CFLAGS} -march=native"
		    case ${CXX} in
			g\+\+* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
	    	    esac
	    	    ;;
		#icc* ) CFLAGS="${CFLAGS} -O3 -axW -mcpu=pentium4"
		icc* ) CFLAGS="${CFLAGS} -xHost -axHASWELL,KNL"
		#skatreka--
		    case ${CXX} in
			icpc* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
		pathcc* ) CFLAGS="${CFLAGS} -O3 -march=opteron -mmmx -msse -msse2 -m3dnow -pipe -fexpensive-optimizations"
		    case ${CXX} in
			pathCC* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
		pgcc* ) CFLAGS="${CFLAGS} -tp=k8-64 -fast -fastsse -Minfo=all"
		    case ${CXX} in
			pgCC* ) CXXFLAGS="${CXXFLAGS} ${CFLAGS}"
		    esac
		    ;;
	    esac
	    ;;
	esac
fi
# End enable_optimize_custom

######################################################
# Previous Location of PVM_ARCH stuff
######################################################
# Begin netCDF test data
AC_PATH_PROG(PATH_TO_NCGEN,ncgen,,)
if test "${PATH_TO_NCGEN}"; then
    AC_MSG_NOTICE(Creating sample netCDF data files for self-tests)
    ${PATH_TO_NCGEN} -b -o data/in.nc data/in.cdl
    if test "x${enable_netcdf4}" = 'xyes'; then
	${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_grp.nc data/in_grp.cdl
        ${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_grp_1.nc data/in_grp_1.cdl
        ${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_grp_2.nc data/in_grp_2.cdl
	    ${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_grp_3.nc data/in_grp_3.cdl
		${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_grp_4.nc data/in_grp_4.cdl
		${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_grp_5.nc data/in_grp_5.cdl
		${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_grp_6.nc data/in_grp_6.cdl
		${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_grp_7.nc data/in_grp_7.cdl
        ${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_1.nc data/in_1.cdl
        ${PATH_TO_NCGEN} -k netCDF-4 -b -o data/in_2.nc data/in_2.cdl
		${PATH_TO_NCGEN} -k netCDF-4 -b -o data/mdl_1.nc data/mdl_1.cdl
		${PATH_TO_NCGEN} -k netCDF-4 -b -o data/mdl_2.nc data/mdl_2.cdl
		${PATH_TO_NCGEN} -k netCDF-4 -b -o data/mdl_3.nc data/mdl_3.cdl
        ${PATH_TO_NCGEN} -k netCDF-4 -b -o data/cmip5.nc data/cmip5.cdl
        ${PATH_TO_NCGEN} -k netCDF-4 -b -o data/obs.nc data/obs.cdl
    fi
    AC_SUBST(PATH_TO_NCGEN)
    for fl in 85.nc 86.nc 87.nc 88.nc 89.nc h0001.nc h0002.nc h0003.nc; do
	if test -f data/${fl}; then
	    printf ""
	else
	    ${LN_S} in.nc data/${fl};
	fi
    done
    if test -f src/nco_c++/in.nc; then
	printf ""
    else
	${LN_S} ../../data/in.nc src/nco_c++/in.nc
    fi
else
    echo "ERROR: Unable to find 'ncgen' (part of the netCDF distribution)"
    echo "Hence unable to create netCDF data files for testing"
    echo "Do this manually in data/ subdirectory before running \"make check\""
fi
# End netCDF test data

# Tell GNU C compiler to use C99 standard (older gcc versions default to C89)
# fxm: only do this for gcc, not g++
if test $ac_compiler_gnu = 'yes'; then
    # 20150109: fxm Introduce -D_DEFAULT_SOURCE in place of deprecated (as of gcc 4.9.2) -D_BSD_SOURCE -D_POSIX_SOURCE 
    # _BSD_SOURCE needed for backward compatibility with old GCC
    # 20160418: default cori gcc is 4.3.4 from 2008!
    CFLAGS="${CFLAGS} -std=c99 -D_BSD_SOURCE -D_POSIX_SOURCE"
    case ${host} in
# MacOSX does not understand shared libraries
# clang -O4 DNE, highest clang optimization is -O3 
	*-apple-darwin* ) CFLAGS=`echo "${CFLAGS}" | sed -e 's/-D_POSIX_SOURCE//'` ; CFLAGS=`echo "${CFLAGS}" | sed -e 's/-O4/-O3/'` ; CFLAGS="-fno-common ${CFLAGS}" ; enable_shared='no' ;;
# Solaris has problems with time.h
	sparc-sun-solaris2* ) CFLAGS=`echo "${CFLAGS}" | sed -e 's/-D_POSIX_SOURCE//'` ; ;;
    esac
fi

# Drop C99 switches when compiling NCO with C++ compiler, e.g., CC=g++
case ${CC} in
    g\+\+* | icpc* ) CC=`echo "${CC}" | sed -e 's/ -std=c99//'` ; CC=`echo "${CC}" | sed -e 's/ -pedantic//'` ; CFLAGS=`echo "${CFLAGS}" | sed -e 's/ -std=c99//'` ; CFLAGS=`echo "${CFLAGS}" | sed -e 's/ -Wmissing-prototypes//'` ; ;;
    xlC* ) CC=`echo "${CC}" | sed -e 's/ -qlanglvl=extc99//'` ; CFLAGS=`echo "${CFLAGS}" | sed -e 's/ -qlanglvl=extc99//'` ; CFLAGS="${CFLAGS} -qlonglong" ; ;;
esac

# Drop C99 switches accidentally incorporated into CXXFLAGS
# (for when laziness led me to copy CXXFLAGS from CFLAGS)
case ${CXX} in
    g\+\+* | icpc* | pathCC* ) CXX=`echo "${CXX}" | sed -e 's/ -std=c99//'` ; CXXFLAGS=`echo "${CXXFLAGS}" | sed -e 's/ -std=c99//'` ; ;;
    pgCC* ) CXX=`echo "${CXX}" | sed -e 's/ -c9x//'` ; CXXFLAGS=`echo "${CXXFLAGS}" | sed -e 's/ -c9x//'` ; ;;
esac

# xlC must use math.h from /usr/vacpp/include or else atan2() will be missing
case ${CXX} in
    xlC* ) CPPFLAGS="-I/usr/vacpp/include ${CPPFLAGS}" ; ;;
esac

echo "###############################"
echo "#    Require Shared Libs?     #"
echo "###############################"
# If --enable-shared define ENABLE_SHARED
#echo "DEBUG: enable_shared: ${enable_shared} "
if (test "${enable_shared}" = 'yes'); then
    AC_DEFINE(ENABLE_SHARED,1,Enable shared libraries)
fi
# End Shared

# Begin Static
# If --enable-static define ENABLE_STATIC
if test "${enable_static}" = 'yes'; then
    AC_DEFINE(ENABLE_STATIC,1,Enable static libraries)
fi
# End Static

# Begin config
# Treat essential and man files now, defer doc files (that rely on makeinfo) to below
AC_CONFIG_FILES([Makefile data/Makefile src/Makefile src/nco/Makefile man/Makefile])
if test "${ac_cv_cxx_have_valarray}" = 'yes' && test "${enable_nco_cxx}" = 'yes' ; then
    AC_CONFIG_FILES(src/nco_c++/Makefile)
    NCO_CXX="nco_c++"
else
    NCO_CXX=
fi
AC_SUBST(NCO_CXX)

# Build ncap2 only if ANTLR library was found
if test "${enable_ncoxx}" = 'yes'  && test "${HAVE_ANTLR}" != 'no' ; then
    AC_CONFIG_FILES(src/nco++/Makefile)
    NCOXX="nco++"
else
    NCOXX=""
fi
AC_SUBST(NCOXX)

# Begin Doc
# Mac OS X has outdated TeXInfo, and fails build docs (unless MacPorts is installed)
# Enable doc-building by default (subject to later checks) unless told otherwise
AC_ARG_ENABLE(doc,AS_HELP_STRING([--enable-doc],[Build/install NCO TexInfo-based documentation (info hierarchy and PDF/HTML Users Guide) [default=yes]]),enable_doc=${enableval},enable_doc=yes)
# End Doc

# Conditional build of doc/Makefile
# Build docs iff makeinfo exists and is recent enough
# This is complex on MacOS:
# nco.texi uses @textdegree since 20140130
# @textdegree appeared by TeXInfo/makeinfo 4.12 (released ~20080420)
# Mac OS X 10.8, 10.9, 10.10 still ship with TeXInfo/makeinfo 4.8 (released ~20050103)
# MacPorts installs modern TeXInfo/makeinfo 5.2 and 6.0 (released ~20130926, ~2015, respectively)

# 20120705 Test for makeinfo; if not present, do not build doc/Makefile
# This prevents failed builds on many server distributions that by default not to install makeinfo, e.g., RHEL and CentOS
AC_CHECK_PROG(HAVE_MAKEINFO,makeinfo,yes,no,${PATH}) 
DOC_FOLDER=""
if test "x${enable_doc}" = 'xyes' && test "x${HAVE_MAKEINFO}" = 'xyes' ; then
    # Ensure makeinfo is recent enough, i.e., version 4.12 or later
    MAKEINFO_VERSION="`makeinfo --version | grep texinfo | cut -d ' ' -f 4`"
    echo "MAKEINFO_VERSION=${MAKEINFO_VERSION}"
    AC_DEFINE_UNQUOTED(MAKEINFO_VERSION,${MAKEINFO_VERSION},Building TeXInfo documentation requires version greater than 4.8)
    #    test_result=m4_version_compare([${MAKEINFO_VERSION}],[4.8])
    test_result='1'
    if test "${test_result}" = '1'; then
	AC_DEFINE([ENABLE_DOC],[1],[Build/install NCO TexInfo-based documentation (info hierarchy and PDF/HTML Users Guide)])
	AC_CONFIG_FILES(doc/Makefile)
	DOC_FOLDER="doc"
	AC_MSG_NOTICE([Documentation support enabled])
    fi
fi
AC_SUBST(DOC_FOLDER)

# End config

# Prefix Flex output symbols with NCO string to avoid namespace conflict with flex-generated symbols from other programs/libraries
NCO_YY_PFX='nco_yy'
LEX_OUTPUT_ROOT="lex.${NCO_YY_PFX}"
LFLAGS="-P${NCO_YY_PFX}"
AC_SUBST(LEX_OUTPUT_ROOT)
AC_SUBST(LFLAGS)

# Every autoconf script finishes by calling AC_OUTPUT to generate and run config.status
AC_OUTPUT

# Echo build parameters for debugging
echo ""
echo "Configuration Parameters:"
echo "AR_FLAGS............. ${AR_FLAGS}"
echo "CC................... ${CC}"
echo "CFLAGS............... ${CFLAGS}"
echo "CPP.................. ${CPP}"
echo "CPPFLAGS............. ${CPPFLAGS}"
echo "CXX.................. ${CXX}"
echo "CXXFLAGS............. ${CXXFLAGS}"
echo "ENABLE_CCR........... ${enable_ccr}"
echo "ENABLE_DAP........... ${enable_dap}"
echo "ENABLE_GSL........... ${enable_gsl}"
echo "ENABLE_NETCDF4....... ${enable_netcdf4}"
echo "ENABLE_OPENMP........ ${enable_openmp}"
echo "ENABLE_UDUNITS....... ${enable_udunits}"
echo "ENABLE_UDUNITS2...... ${enable_udunits2}"
echo "GSL_ROOT............. ${GSL_ROOT}"
echo "HAVE_ANTLR........... ${HAVE_ANTLR}"  
echo "HAVE_MAKEINFO........ ${HAVE_MAKEINFO}"  
echo "HAVE_NETCDF4_H....... ${header_defines_nc_chunked}"
echo "HDF5_PLUGIN_PATH..... ${HDF5_PLUGIN_PATH}"  
echo "HOST................. ${HOST}"
echo "HOSTNAME............. ${HOSTNAME}"
echo "LDFLAGS.............. ${LDFLAGS}"
echo "LIBS................. ${LIBS}"
echo "NETCDF_ROOT.......... ${NETCDF_ROOT}"
echo "OPENMP_CFLAGS........ ${OPENMP_CFLAGS}"
echo "host................. ${host}"
echo "install prefix ...... ${prefix}"
# Inform users of the very useful configure.eg
echo "*******************************************************************"
echo "Configuration complete. You are now ready to run 'make'."
echo "Ensure 'make' points to GNU Make (AT&T Make chokes on GNU syntax)."
echo "If the build fails, please examine the file 'configure.eg'"
echo "in the top-level NCO directory.  It contains examples known to"
echo "build NCO for various platforms and compilers."
echo "*******************************************************************"

