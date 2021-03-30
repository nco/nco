/* $Header$ */

/* Purpose: netCDF Operator (NCO) definitions */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco.h" *//* netCDF Operator (NCO) definitions */

#ifndef NCO_H /* Contents have not yet been inserted in current source file */
#define NCO_H

/* Token added by Pedro to help debugging in nco_grp_*.c */
#define DEBUG_LEAKS

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#ifdef NC_HAVE_META_H
# include <netcdf_meta.h> /* NC_VERSION_..., NC_HAVE_RENAME_GRP */
#endif /* !NC_HAVE_META_H */

/* Personal headers */
#include "nco_typ.h" /* Type definitions, opaque types */
#include "nco_uthash.h" /* Hash table functionality */

  /* C pre-processor macros for instantiating variable values with string tokens
     Macros for token pasting described at http://www.parashift.com/c++-faq-lite
     Layer of indirection is required, use public macro to call private macro */
#define TKN2SNG_PRV(x) #x
#define TKN2SNG(x) TKN2SNG_PRV(x)

/* Encapsulate C++ const usage in C99-safe macro 
   C++ compilers will use type-safe version
   C89 and C99 compilers use less type-safe version that is standards-compliant */
#ifdef __cplusplus
# define CST_X_PTR_CST_PTR_CST_Y(x,y) const x * const * const y
# define X_CST_PTR_CST_PTR_Y(x,y) x const * const * y
#else /* !__cplusplus */
# define CST_X_PTR_CST_PTR_CST_Y(x,y) x * const * const y
# define X_CST_PTR_CST_PTR_Y(x,y) x * const * y
#endif /* !__cplusplus */

#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* !__cplusplus */

/* Replace restrict by __restrict in g++ compiler
   Substitute whitespace for restrict in all other C++ compilers */
#ifdef __cplusplus
# ifdef __GNUG__
#  define restrict __restrict
# else /* !__GNUG__ */
#  define restrict
# endif /* !__GNUG__ */
#endif /* !__cplusplus */
#ifdef _MSC_VER
# define restrict
#endif /* !_MSC_VER */

/* Boolean values */
/* From Wikipedia:
   "On a recent C compiler (supporting the C99 standard), there is a _Bool type, which is used to define bool by the stdbool.h header: 
   #include <stdbool.h>
   bool b = false;
   ...
   b = true;
   During its standardization process, the C++ programming language introduced the bool, true and false keywords, adding a native datatype to support boolean data.
   Preprocessor macros may be used to turn bool into _Bool, false into 0 and true into 1, allowing compatibility with the aforementioned C99 use of the stdbool.h header." */
#define nco_bool int
#ifndef __cplusplus
# ifndef bool
#  define bool int
# endif /* bool */
#endif /* __cplusplus */
#ifndef True
# define True 1
#endif /* True */
#ifndef False
# define False 0
#endif /* False */
  
/* 20151207: Reports that Cygwin math.h does not define M_LN10 or M_LN2
   20160209: More reports that Cygwin math.h does not define M_PI
   math.h does not necessarily define these constants in C99 mode
   They are usually defined on Linux and MacOSX but not Cygwin
   Linux math.h says these constants are POSIX/UNIX extensions to the C standard,
   and only defines them when __DARWIN_C_LEVEL >= 199506L
   NCO uses these math constants in nco_aux.c (M_PI), nco_ppc.c (M_LN2,M_LN10), and nco_rgr.c (M_PI)
   Rather than have backup-definitions in the three corresponding .h files, 
   we #ifndef all necessary math constants once in common header nco.h */
#ifndef M_LN10
# define M_LN10      2.30258509299404568401799145468436421   /* loge(10)       */
#endif /* M_LN10 */
#ifndef M_LN2
# define M_LN2       0.693147180559945309417232121458176568  /* loge(2)        */
#endif /* M_LN2 */
#ifndef M_PI
# define M_PI        3.14159265358979323846264338327950288   /* pi             */
#endif /* M_PI */
#ifndef M_PI_2
# define M_PI_2      1.57079632679489661923132169163975144   /* pi/2           */
#endif /* M_PI_2 */

  /* Variables marked CEWI "Compiler Error Warning Initialization" are initialized
     to prevent spurious "warning: `float foo' might be used uninitialized in 
     this function" warnings when, e.g., GCC -Wuninitialized is turned on.
     Note that these warning messages are compiler- and OS-dependent
     GCC warning on Alpha, e.g., cannot be removed by this trick */
#define NULL_CEWI NULL
#define char_CEWI '\0'
#define double_CEWI 0.0
#define float_CEWI 0.0
#define int_CEWI 0
#define long_CEWI 0L
#define long_long_CEWI ((long long int)0LL)
#define nco_bool_CEWI 0
#define nco_byte_CEWI ((signed char)0)
#define nco_char_CEWI '\0'
#define nco_int_CEWI (0L)
#define nco_short_CEWI ((short int)0)
#define nco_ubyte_CEWI ((unsigned char)0)
#define nco_ushort_CEWI ((unsigned short int)0)
#define nco_uint_CEWI (0U)
#define nco_int64_CEWI ((long long int)0LL)
#define nco_uint64_CEWI ((unsigned long long int)0ULL)
#define nco_string_CEWI NULL
#define short_CEWI 0
#define size_t_CEWI 0UL

  /* 20150617: Define macro to indicate variables are unused by design
     http://stackoverflow.com/questions/777261/avoiding-unused-variables-warnings-when-using-assert-in-a-release-build 
     Especially useful to keep variables used only in assert() macros from triggering unused variable warnings
     Eventually, much CEWI code could be replaced by this macro */
#define CEWI_unused(x)   ((void)x)
  
  /* Numeric constants to simplify arithmetic */
#define NCO_BYT_PER_KiB 1024UL
#define NCO_BYT_PER_MiB 1048576UL
#define NCO_BYT_PER_GiB 1073741824UL
#define NCO_BYT_PER_TiB 1099511627776UL

#define NCO_BYT_PER_KB 1000UL
#define NCO_BYT_PER_MB 1000000UL
#define NCO_BYT_PER_GB 1000000000UL
#define NCO_BYT_PER_TB 1000000000000UL

  /* netcdf.h NC_GLOBAL is, strictly, the variable ID for global attributes
     NCO_REC_DMN_UNDEFINED is dimension ID of record dimension iff record dimension is undefined
     Normally using -1 for this ID is fine, but token makes meaning clearer
     NB: nc_inq() family returns -1 for missing record dimensions */
#define NCO_REC_DMN_UNDEFINED -1

  /* Valid netCDF4 deflation levels (dfl_lvl) range from 0..9 inclusive 
     Variables with dfl_lvl == 0 return true for compression yet are not deflated
     Calling deflation routine with dfl_lvl == 0 wastes time _unless_ user wishes 
     to uncompress variable that is currently deflated with dfl_lvl != 0.
     Undefined dfl_lvl indicates user has not yet specified and intended dfl_lvl
     This undefined value must be less than zero (and not equal to zero) so that
     if(dfl_lvl >= 0) deflate(); only calls deflate() when user requests it. 
     Setting dfl_lvl == 0 decompresses already compressed variables */
#define NCO_DFL_LVL_UNDEFINED -1

  /* netcdf.h defines four NC_FORMAT tokens: NC_FORMAT_CLASSIC, ...
     The values are (currently) enumerated from one to four
     Operators need to check if fl_out_fmt has been user-specified
     Safest way is to compare current value of fl_out_fmt to initial value 
     Initial value should be a number that will never be a true netCDF format */
#define NCO_FORMAT_UNDEFINED 0
  
  /* Argument to strtol() and strtoul() indicating base-10 conversions */
#define NCO_SNG_CNV_BASE10 10

  /* 20161121 Chunk cache size default
     http://www.unidata.ucar.edu/software/netcdf/docs/netcdf_perf_chunking.html
     netCDF cache size default settable at netCDF build time with --with-chunk-cache-size option
     If NCO default == 0, then NCO will use whatever default built-into netCDF library
     If NCO default  > 0, then NCO will override netCDF default */
#define NCO_CNK_CSH_BYT_DFL 0

  /* netCDF 4.3.2 (201404) implements a configure-time constant called DEFAULT_CHUNK_SIZE = 4194304 = 4 MB
     This is a good size for HPC systems with MB-scale blocksizes
     Token is not in netcdf.h, and NCO's equivalent need not match netCDF's
     NCO uses NCO token    NCO_CNK_SZ_BYT_DFL and default algorithm when user specifies any chunking option
     NCO uses netCDF token DEFAULT_CHUNK_SIZE and default algorithm when user specifies no  chunking option */
#define NCO_CNK_SZ_BYT_DFL 4194304

  /* 20141105 Minimum size of a variable to chunk */
#define NCO_CNK_SZ_MIN_BYT_DFL 8192

  /* Linux default blocksize is 4096 B---a good chunk size for 1-D record dimension variables */
#define NCO_CNK_SZ_BYT_R1D_DFL 4096

  /* Maximum number of names to examine in CF "coordinates" attribute */
#define NCO_MAX_CRD_PER_VAR 6

  /* netCDF provides no guidance on maximum nesting of groups */
#define NCO_MAX_GRP_DEPTH 10

  /* Signifier that iterator found no more groups in container */
#define NCO_LST_GRP 0

  /* Debugging level that quiets all non-requested informational messages
     This value is compared against user-selected nco_dbg_lvl 
     Running operators with --quiet automatically sets nco_dbg_lvl=NCO_DBG_QUIET */
#define NCO_DBG_QUIET 0

  /* Find traversal table objects via hash table rather than brute-force strcmp() search */
  /*#define NCO_HSH_TRV_OBJ */
#undef NCO_HSH_TRV_OBJ

  /* NCO_MSS_VAL_SNG names attribute whose value is "skipped" by arithmetic, aka the missing value attribute
     Attribute name should be either "missing_value" or "_FillValue" */
#ifndef NCO_MSS_VAL_SNG
  /* 20070831: TKN2SNG() technique inserts quotes into string though same test code in c.c does not produce extra quotes. TODO nco905. */
  /*# define NCO_MSS_VAL_SNG missing_value */
  /*# define NCO_MSS_VAL_SNG _FillValue*/
  /*char nco_mss_val_sng[]=TKN2SNG(NCO_MSS_VAL_SNG);*/ /* [sng] Missing value attribute name */
  /* Arcane workaround using NCO_USE_FILL_VALUE necessary because TKN2SNG() macro above is broken. TODO nco905 */
# define NCO_USE_FILL_VALUE
# ifdef NCO_USE_FILL_VALUE
  /* NCO_NETCDF4_AND_FILLVALUE tells whether netCDF4 restrictions on 
     _FillValue operations (must be defined before variable written,
     cannot be changed after variable written) can affect output file */ 
#  ifdef ENABLE_NETCDF4
#   define NCO_NETCDF4_AND_FILLVALUE
#  endif /* !ENABLE_NETCDF4 */
# endif /* !NCO_USE_FILL_VALUE */
#endif /* NCO_MSS_VAL_SNG */

  /* Prototype global functions before defining them in next block */
  char *nco_mss_val_sng_get(void); /* [sng] Missing value attribute name */
  char *nco_mta_dlm_get(void); /* [sng] Multi-argument delimiter */
  char *nco_not_mss_val_sng_get(void); /* [sng] Not missing value attribute name */
  char *nco_prg_nm_get(void);
  int nco_prg_id_get(void);
  int nco_flt_glb_get(void);
  unsigned short nco_baa_cnv_get(void);
  unsigned short nco_bnr_cnv_get(void);
  unsigned short nco_dbg_lvl_get(void);
  unsigned short nco_fmt_xtn_get(void);
  unsigned short nco_mrd_cnv_get(void);
  unsigned short nco_rth_cnv_get(void);
  unsigned short nco_upk_cnv_get(void);
  void nco_fmt_xtn_set(unsigned short nco_fmt_xtn_arg);
  void nco_mta_dlm_set(char *nco_mta_dlm_arg);

#ifdef MAIN_PROGRAM_FILE /* Current file contains main() */
  
  /* Tokens and variables with scope limited to main.c, and global variables allocated here */
  
  int nco_prg_id; /* [enm] Program ID */
  int nco_prg_id_get(void){return nco_prg_id;} /* [enm] Program ID */
  
  char *nco_prg_nm; /* [sng] Program name */
  char *nco_prg_nm_get(void){return nco_prg_nm;} /* [sng] Program name */
  
  char *nco_mta_dlm=NULL; /* [sng] Multi-argument delimiter */
  char *nco_mta_dlm_get(void){if(!nco_mta_dlm){nco_mta_dlm=(char *)strdup("#");}
    return nco_mta_dlm;} /* [sng] Multi-argument delimiter */
  void nco_mta_dlm_set(char *nco_mta_dlm_arg){nco_mta_dlm=nco_mta_dlm_arg;} /* [sng] Multi-argument delimiter */
  
  //unsigned short nco_baa_cnv=0; /* [enm] Bit-Adjustment Algorithm defaults to Bit Grooming (until 20200715) */
  unsigned short nco_baa_cnv=5; /* [enm] Bit-Adjustment Algorithm defaults to Bit Rounding (after 20200715) */
  unsigned short nco_baa_cnv_get(void){return nco_baa_cnv;} /* [enm] Bit-Adjustment Algorithm */

  unsigned short nco_bnr_cnv=0; /* [enm] Binary byte-ordering convention to produce (native or byte-swapped) */
  unsigned short nco_bnr_cnv_get(void){return nco_bnr_cnv;} /* [enm] Binary byte-ordering convention to employ */

  unsigned short nco_dbg_lvl=0; /* [enm] Debugging level */
  unsigned short nco_dbg_lvl_get(void){return nco_dbg_lvl;} /* [enm] Debugging level */

  int nco_flt_glb=0; /* [enm] Filter enum */
  int nco_flt_glb_get(void){return nco_flt_glb;} /* [enm] Filter enum */

  unsigned short nco_fmt_xtn=0; /* [enm] Extended file format */
  unsigned short nco_fmt_xtn_get(void){return nco_fmt_xtn;} /* [enm] Extended file format */
  void nco_fmt_xtn_set(unsigned short nco_fmt_xtn_arg){nco_fmt_xtn=nco_fmt_xtn_arg;} /* [enm] Extended file format */

  unsigned short nco_mrd_cnv=0; /* [enm] Multiple Record Dimension convention */
  unsigned short nco_mrd_cnv_get(void){return nco_mrd_cnv;} /* [enm] Multiple Record Dimension convention */

  unsigned short nco_rth_cnv=1; /* [enm] Arithmetic convention */
  unsigned short nco_rth_cnv_get(void){return nco_rth_cnv;} /* [enm] Arithmetic convention */

  unsigned short nco_upk_cnv=0; /* [enm] Unpacking convention */
  unsigned short nco_upk_cnv_get(void){return nco_upk_cnv;} /* [enm] Unpacking convention */

# ifdef NCO_USE_FILL_VALUE
  /* This arcane get()/set() usage necessary because TKN2SNG() macro above is broken. TODO nco905 */
  char nco_mss_val_sng[]="_FillValue"; /* [sng] Missing value attribute name */
  char nco_not_mss_val_sng[]="missing_value"; /* [sng] Not missing value attribute name */
# else /* !NCO_USE_FILL_VALUE */
  char nco_mss_val_sng[]="missing_value"; /* [sng] Missing value attribute name */
  char nco_not_mss_val_sng[]="_FillValue"; /* [sng] Not missing value attribute name */
# endif /* !NCO_USE_FILL_VALUE */
  char *nco_mss_val_sng_get(void){return nco_mss_val_sng;} /* [sng] Missing value attribute name */
  char *nco_not_mss_val_sng_get(void){return nco_not_mss_val_sng;} /* [sng] Not missing value attribute name */

#ifdef ENABLE_ESMF
  /* csz 20150320
     Define these only in main() files not in original location in ESMC_CoordSys.h  
     Otherwise any file #including ESMC.h inadvertently includes these constants multiple times 
     Should be defined as externals that resolve to single definition in main()
     sudo cp ~/nco/src/nco/ESMC_CoordSys.h /usr/local/include # Linux
     sudo cp ~/nco/src/nco/ESMC_CoordSys.h /opt/local/include/esmf # MacOS MacPorts */
  const double ESMC_CoordSys_Deg2Rad= 0.01745329251994329547437;
  const double ESMC_CoordSys_Rad2Deg=57.29577951308232286464772;
#endif /* !ENABLE_ESMF */

#else /* MAIN_PROGRAM_FILE is NOT defined, i.e., current file does not contain main() */
  
  /* External references to global variables are declared as extern here
     Variables with local file scope in all files except main.c are allocated here */
  
#endif /* MAIN_PROGRAM_FILE is NOT defined, i.e., the current file does not contain main() */

/* Compatibility tokens for when NCO compiled with older netcdf.h
   It is hard to track where/when many tokens defined
   Easiest to individually check for pre-definition of each */

  /* 20140924: netCDF meta-information available in library versions 4.3.3-rc2 
     Define compatibility tokens for earlier libraries equivalent to 3.6.0 */
#ifndef NC_VERSION_MAJOR
# define NC_VERSION_MAJOR 3
#endif /* !NC_VERSION_MAJOR */
#ifndef NC_VERSION_MINOR
# define NC_VERSION_MINOR 6
#endif /* !NC_VERSION_MINOR */
#ifndef NC_VERSION_PATCH
# define NC_VERSION_PATCH 0
#endif /* !NC_VERSION_PATCH */
#ifndef NC_VERSION_NOTE
# define NC_VERSION_NOTE  "" /* May be blank */
#endif /* !NC_VERSION_NOTE */
#ifndef NC_VERSION
# define NC_VERSION "3.6.0"
#endif /* !NC_VERSION */
  /* Define NC_LIB_VERSION as three-digit number for arithmetic comparisons by CPP */
#ifndef NC_LIB_VERSION
# define NC_LIB_VERSION ( NC_VERSION_MAJOR * 100 + NC_VERSION_MINOR * 10 + NC_VERSION_PATCH )
#endif /* !NC_LIB_VERSION */

  /* NCO meta-information available in NCO_VERSION token since forever
     20141008: Define NCO tokens consistent with new netcdf_meta.h tokens */
#ifndef NCO_VERSION_MAJOR
# define NCO_VERSION_MAJOR 4
#endif /* !NCO_VERSION_MAJOR */
#ifndef NCO_VERSION_MINOR
# define NCO_VERSION_MINOR 9
#endif /* !NCO_VERSION_MINOR */
#ifndef NCO_VERSION_PATCH
# define NCO_VERSION_PATCH 9
#endif /* !NCO_VERSION_PATCH */
#ifndef NCO_VERSION_NOTE
# define NCO_VERSION_NOTE "-alpha02" /* Blank for final versions, non-blank (e.g., "-beta37") for pre-release versions */
#endif /* !NCO_VERSION_NOTE */
#ifndef NCO_LIB_VERSION
  /* Define NC_LIB_VERSION as three-digit number for arithmetic comparisons by CPP */
# define NCO_LIB_VERSION ( NCO_VERSION_MAJOR * 100 + NCO_VERSION_MINOR * 10 + NCO_VERSION_PATCH )
#endif /* !NCO_LIB_VERSION */
#ifndef NCO_VERSION
# define NCO_VERSION "4.9.9-alpha02"
#endif /* !NCO_VERSION */

/* Compatibility tokens new to netCDF4 netcdf.h: */
#ifndef NC_NETCDF4
# define NC_NETCDF4  0x1000 /* Use netCDF-4/HDF5 format. Mode flag for nc_create(). */
#endif
#ifndef NC_MPIIO
# define NC_MPIIO    0x2000 /* Turn on MPI I/O. Mode flag for both nc_create() and nc_open(). */
#endif
#ifndef NC_MPIPOSIX
# define NC_MPIPOSIX 0x4000 /* Turn on MPI POSIX I/O. Mode flag for both nc_create() and nc_open(). Deprecated as of libhdf5 1.8.13. */
#endif
#ifndef NC_INMEMORY
# define NC_INMEMORY  0x8000 /* Read from memory. Mode flag for nc_open() or nc_create() */
#endif
#ifndef NC_PNETCDF
# define NC_PNETCDF  (NC_MPIIO) /* Use parallel-netcdf library. Alias for NC_MPIIO */
#endif
/* Use these with nc_var_par_access(). */
#ifndef NC_INDEPENDENT
# define NC_INDEPENDENT 0
#endif
#ifndef NC_COLLECTIVE
# define NC_COLLECTIVE 1
#endif
#ifndef NC_ENDIAN_NATIVE
# define NC_ENDIAN_NATIVE 0
#endif
#ifndef NC_ENDIAN_LITTLE
# define NC_ENDIAN_LITTLE 1
#endif
#ifndef NC_ENDIAN_BIG
# define NC_ENDIAN_BIG    2
#endif
#ifndef NC_CHUNKED
# define NC_CHUNKED (0)
#endif
#ifndef NC_CONTIGUOUS
# define NC_CONTIGUOUS (1)
#endif
#ifndef NC_NOCHECKSUM
# define NC_NOCHECKSUM 0
#endif
#ifndef NC_FLETCHER32
# define NC_FLETCHER32 1
#endif
#ifndef NC_NOSHUFFLE
# define NC_NOSHUFFLE 0
#endif
#ifndef NC_SHUFFLE
# define NC_SHUFFLE   1
#endif

/* Six compatibility tokens not all available until netCDF 3.6.1 netcdf.h
   NC_64BIT_OFFSET is used (so far) only in nco_fl_utl.c
   20151222: Introduction of CDF5 in netCDF 4.4.0-RC4 netcdf.h makes original NC_FORMAT_64BIT token ambiguous
   Type introduced as NC_FORMAT_64BIT in netCDF 3.X is now properly called NC_FORMAT_64BIT_OFFSET (64-bit offsets/pointers NOT data)
   Type introduced as CDF5 (from pnetCDF) in netCDF 4.4.0 is properly called NC_FORMAT_64BIT_DATA (64-bit offsets/pointers AND data) */
#ifndef NC_CLASSIC_MODEL
# define NC_CLASSIC_MODEL 0x0100 /**< Enforce classic model. Mode flag for nc_create(). */
#endif
#ifndef NC_64BIT_OFFSET
# define NC_64BIT_OFFSET 0x0200 /* Use large (64-bit) file offsets */
#endif
#ifndef NC_DISKLESS
# define NC_DISKLESS 0x0008 /* Use diskless file. Mode flag for nc_open() or nc_create(). */
#endif
#ifndef NC_FORMAT_CLASSIC
# define NC_FORMAT_CLASSIC (1)
#endif
#ifndef NC_FORMAT_64BIT_OFFSET
# define NC_FORMAT_64BIT_OFFSET   (2)
#endif
#ifndef NC_FORMAT_64BIT
# define NC_FORMAT_64BIT (NC_FORMAT_64BIT_OFFSET)
#endif
#ifndef NC_FORMAT_NETCDF4
# define NC_FORMAT_NETCDF4 (3)
#endif
#ifndef NC_FORMAT_NETCDF4_CLASSIC
# define NC_FORMAT_NETCDF4_CLASSIC  (4) /* create netcdf-4 files, with NC_CLASSIC_MODEL. */
#endif
#ifndef NC_FORMAT_64BIT_DATA
# define NC_FORMAT_64BIT_DATA   (5)
#endif

  /* 20131222: Seven compatibility tokens introduced in netCDF 4.3.1-rc7 netcdf.h
     20151222: Tokens are superseded in netCDF 4.4.0-RC4 netcdf.h by same tokens with "FORMATX" instead of "FORMAT"
     This disambiguates extended format (FORMATX) flags returned by nc_inq_format_extended() from format flags (e.g., NC_FORMAT_CLASSIC) returned by nco_inq_format()
     Also added NC_FORMAT_NC4 as alias to NC_FORMAT_NC_HDF5 */
#ifndef NC_FORMAT_UNDEFINED
# define NC_FORMAT_UNDEFINED (0)
#else
# define NC_HAVE_INQ_FORMAT_EXTENDED
#endif
#ifndef NC_FORMAT_NC3
# define NC_FORMAT_NC3     (1)
#endif
#ifndef NC_FORMAT_NC_HDF5
# define NC_FORMAT_NC_HDF5 (2) /* netCDF-4 subset of HDF5 */
#endif
#ifndef NC_FORMAT_NC4
# define NC_FORMAT_NC4 NC_FORMAT_NC_HDF5 /* alias */
#endif
#ifndef NC_FORMAT_NC_HDF4
# define NC_FORMAT_NC_HDF4 (3) /* netCDF-4 subset of HDF4 */
#endif
#ifndef NC_FORMAT_PNETCDF
# define NC_FORMAT_PNETCDF (4)
#endif
#ifndef NC_FORMAT_DAP2
# define NC_FORMAT_DAP2    (5)
#endif
#ifndef NC_FORMAT_DAP4
# define NC_FORMAT_DAP4    (6)
#endif

#ifndef NC_FORMATX_UNDEFINED
# define NC_FORMATX_UNDEFINED (0)
#else
# define NC_HAVE_INQ_FORMATX_EXTENDED
#endif
#ifndef NC_FORMATX_NC3
# define NC_FORMATX_NC3     (1)
#endif
#ifndef NC_FORMATX_NC_HDF5
# define NC_FORMATX_NC_HDF5 (2) /* netCDF4 subset of HDF5 */
#endif
#ifndef NC_FORMATX_NC4
# define NC_FORMATX_NC4 NC_FORMATX_NC_HDF5 /* alias */
#endif
#ifndef NC_FORMATX_NC_HDF4
# define NC_FORMATX_NC_HDF4 (3) /* netcdf4 subset of HDF4 */
#endif
#ifndef NC_FORMATX_PNETCDF
# define NC_FORMATX_PNETCDF (4)
#endif
#ifndef NC_FORMATX_DAP2
# define NC_FORMATX_DAP2    (5)
#endif
#ifndef NC_FORMATX_DAP4
# define NC_FORMATX_DAP4    (6)
#endif

  /* Three compatibility tokens from pnetcdf.h introduced to NCO 20140604 
     These were first fully supported in 201708 (NCO 4.6.9) */
#ifndef NC_64BIT_DATA
# define NC_64BIT_DATA	0x0010 /* CDF-5 format, (64-bit) supported */
#endif
#ifndef NC_FORMAT_CDF5
# define NC_FORMAT_CDF5  5 /* CDF-5 format, with NC_64BIT_DATA. */
#endif
#ifndef NC_FORMAT_64BIT_DATA
# define NC_FORMAT_64BIT_DATA	NC_FORMAT_CDF5
#endif

  /* Backwards-compatibility error codes for netCDF4
     Currently used only in nco_grp_utl.c and nco_netcdf.c */
#ifndef NC_EBADGRPID
# define NC_EBADGRPID (-116)    /**< Bad group ID. */
#endif
#ifndef NC_ENOGRP
# define NC_ENOGRP        (-125)    /**< No group found. */
#endif

  /* Two backwards-compatibility error codes implemented in 201106 to diagnose problems with DAP
     Currently used only in nco_fl_utl.c */
#ifndef NC_ECANTREAD
# define NC_ECANTREAD (-102)    /**< Can't read. */
#endif
#ifndef NC_EDAPSVC
# define NC_EDAPSVC (-70)      /**< DAP server error */
#endif

  /* Backwards-compatibility error codes implemented in 202004 to workaround netCDF 4.7.4 backwards incompatibility with nc_inq_var_filter()
     Currently used only in nco_prn.c */
#ifndef NC_ENOFILTER
# define NC_ENOFILTER (-136)      /**< Filter not defined on variable. */
#endif

  /* NCO began using MIN/MAX tokens in nco_pck.c on 20101130
     Not sure when these tokens were introduced to netcdf.h */
#ifndef NC_MAX_BYTE
# define NC_MAX_BYTE 127
#endif
#ifndef NC_MIN_BYTE
# define NC_MIN_BYTE (-NC_MAX_BYTE-1)
#endif
#ifndef NC_MAX_CHAR
# define NC_MAX_CHAR 255
#endif
#ifndef NC_MAX_SHORT
# define NC_MAX_SHORT 32767
#endif
#ifndef NC_MIN_SHORT
# define NC_MIN_SHORT (-NC_MAX_SHORT - 1)
#endif
#ifndef NC_MAX_INT
# define NC_MAX_INT 2147483647
#endif
#ifndef NC_MIN_INT
# define NC_MIN_INT (-NC_MAX_INT - 1)
#endif
#ifndef NC_MAX_FLOAT
# define NC_MAX_FLOAT 3.402823466e+38f
#endif
#ifndef NC_MIN_FLOAT
# define NC_MIN_FLOAT (-NC_MAX_FLOAT)
#endif
#ifndef NC_MAX_DOUBLE
# define NC_MAX_DOUBLE 1.7976931348623157e+308 
#endif
#ifndef NC_MIN_DOUBLE
# define NC_MIN_DOUBLE (-NC_MAX_DOUBLE)
#endif
#ifndef NC_MAX_UBYTE
# define NC_MAX_UBYTE NC_MAX_CHAR
#endif
#ifndef NC_MAX_USHORT
# define NC_MAX_USHORT 65535U
#endif
#ifndef NC_MAX_UINT
# define NC_MAX_UINT 4294967295U
#endif
#ifndef NC_MAX_INT64
# define NC_MAX_INT64 (9223372036854775807LL)
#endif
#ifndef NC_MIN_INT64
# define NC_MIN_INT64 (-9223372036854775807LL-1)
#endif
#ifndef NC_MAX_UINT64
# define NC_MAX_UINT64 (18446744073709551615ULL)
#endif
/* Endif older netcdf.h */

  /* Define compatibility tokens when user does not have znetCDF */
#ifndef ENABLE_ZNETCDF
# ifndef NC_COMPRESS
#  define NC_COMPRESS 0x200 /* bn file is compressed */
# endif
#endif /* !ENABLE_ZNETCDF */

  /* NB: Use NCO_NOERR and NCO_ERR as return codes to other functions, not to shell (e.g., Bash, Csh)
     Shell exit codes (where 0 indicates success) are traditionally opposite C exit codes (where 0 indicates failure) */
/* Internal NCO function return code indicating success */
#define NCO_NOERR 1
/* Internal NCO function return code indicating failure */
#define NCO_ERR 0

/* UDUnits return code indicating success */
#define UDUNITS_NOERR 0

  /* NB: Use EXIT_SUCCESS and EXIT_FAILURE as return codes to shell (e.g., Bash, Csh), not to other functions
     Shell exit codes (where 0 indicates success) are traditionally opposite C-function (not C-program) exit codes (where 0 indicates failure)
     20130711: FC19 x86_64 Linux defines EXIT_FAILURE == 134 */
#ifndef EXIT_SUCCESS /* Most likely this is a SUN4 machine */
# define EXIT_SUCCESS 0
#endif /* SUN4 */
#ifndef EXIT_FAILURE /* Most likely this is a SUN4 machine */
# define EXIT_FAILURE 1
#endif /* SUN4 */

#define TRV_MAP_SIZE NC_MAX_DIMS

  enum nco_prg_id{ /* [enm] Key value for all netCDF operators */
    ncap, /* 0 [enm] */
    ncatted, /* 1 [enm] */
    ncbo, /* 2 [enm] */
    ncfe, /* 3 [enm] */
    ncecat, /* 4 [enm] */
    ncflint, /* 5 [enm] */
    ncks, /* 6 [enm] */
    ncpdq, /* 7 [enm] */
    ncra, /* 8 [enm] */
    ncrcat, /* 9 [enm] */
    ncrename, /* 10 [enm] */
    ncwa, /* 11 [enm] */
    ncge, /* 12 [enm] */ /* 20131018: New operator */
    ncmv /* 13 [enm] */ /* 20131018: Potential new operator */
  }; /* end nco_prg_id enum */
  
  enum nco_dbg_typ_enm{ /* [enm] Debugging levels */
    /* List in increasing levels of verbosity */
    nco_dbg_quiet, /* 0 [enm] Quiet all non-error messages */
    nco_dbg_std,   /* 1 [enm] Standard mode. Minimal, but some, messages */
    nco_dbg_fl,    /* 2 [enm] Filenames */
    nco_dbg_scl,   /* 3 [enm] Scalars, other per-file information */
    nco_dbg_grp,   /* 4 [enm] Groups, highest level per-file loop information */
    nco_dbg_var,   /* 5 [enm] Variables, highest level per-group loop information */
    nco_dbg_crr,   /* 6 [enm] Current task */
    nco_dbg_sbr,   /* 7 [enm] Subroutine names on entry and exit */
    nco_dbg_io,    /* 8 [enm] Subroutine I/O */
    nco_dbg_vec,   /* 9 [enm] Entire vectors */
    nco_dbg_vrb,   /* 10 [enm] Verbose, print everything possible */
    nco_dbg_old,   /* 11 [enm] Old debugging blocks not used anymore */
    nco_dbg_dev,   /* 12 [enm] NCO developer information; only useful for debugging */
    nco_dbg_sup,   /* 13 [enm] NCO extra developer information not usually used (debug memory leaks) */
    nco_dbg_nbr    /* 14 [enm] Number of debugging types (equals last enumerated value) */
  }; /* end nco_dbg_typ_enm */

  enum nco_op_typ{ /* [enm] Operation type */
    /* Types used in ncbo(): */
    nco_op_add, /* [enm] Add file_1 to file_2 */
    nco_op_dvd, /* [enm] Divide file_1 by file_2 */
    nco_op_mlt, /* [enm] Multiply file_1 by file_2 */
    nco_op_sbt, /* [enm] Subtract file_2 from file_1 */
    /* Types used in ncra(), ncrcat(), ncwa(): */
    nco_op_avg, /* [enm] Average */
    nco_op_min, /* [enm] Minimum value */
    nco_op_max, /* [enm] Maximum value */
    nco_op_ttl, /* [enm] Linear sum */
    nco_op_sqravg, /* [enm] Square of mean */
    nco_op_avgsqr, /* [enm] Mean of sum of squares */
    nco_op_sqrt, /* [enm] Square root of mean */
    nco_op_rms, /* [enm] Root-mean-square (normalized by N) */
    nco_op_rmssdn, /* [enm] Root-mean square normalized by N-1 */
    nco_op_mabs, /* [enm] Maximum absolute value */
    nco_op_mebs, /* [enm] Mean absolute value */
    nco_op_mibs, /* [enm] Minimum absolute value */
    nco_op_tabs, /* [enm] Total absolute value */
    nco_op_nil /* [enm] Nil or undefined operation type */
  }; /* end nco_op_typ enum */

  enum nco_rlt_opr{ /* [enm] Arithmetic relations (comparisons) for masking */
    nco_op_eq, /* Equality */
    nco_op_ne, /* Inequality */
    nco_op_lt, /* Less than */
    nco_op_gt, /* Greater than */
    nco_op_le, /* Less than or equal to */
    nco_op_ge /* Greater than or equal to */
  }; /* end nco_rlt_opr enum */
  
  typedef enum nco_obj_typ_enm{ /* [enm] netCDF4 object type: group, variable */
    nco_obj_typ_err=-1, /* -1 Invalid type for initialization */
    nco_obj_typ_grp,    /*  0, Group */
    nco_obj_typ_var,    /*  1, Variable of atomic type */
    nco_obj_typ_nonatomic_var /*  2, Variable of non-atomic type (vlen, opaque, enum, compound, user-defined) */
  } nco_obj_typ;
  
  enum nco_fmt_xtn{ /* [enm] Extended or underlying filetype */
    /* 20131222: Tokens defined as of netCDF 4.3.1-rc7 */
    nco_fmt_xtn_nil=NC_FORMAT_UNDEFINED, /* 0 Undefined (more precisely, not yet defined) */
    nco_fmt_xtn_nc3=NC_FORMAT_NC3, /* 1 netCDF3 */
    nco_fmt_xtn_hdf5=NC_FORMAT_NC_HDF5, /* 2 HDF5 */
    nco_fmt_xtn_hdf4=NC_FORMAT_NC_HDF4, /* 3 HDF4 */
    nco_fmt_xtn_pnetcdf=NC_FORMAT_PNETCDF, /* 4 PnetCDF */
    nco_fmt_xtn_dap2=NC_FORMAT_DAP2, /* 5 DAP2 */
    nco_fmt_xtn_dap4=NC_FORMAT_DAP4, /* 6 DAP4 */
  }; /* end nco_fmt_xtn */

  enum nco_mrd_cnv{ /* [enm] Multiple Record Dimension convention: for ncecat and ncpdq */
    /* This currently could be implemented as a flag rather than an enum
       General case may need more than binary option so use enum */
    nco_mrd_restrict, /* 0 Fix some dimensions so as to avoid producing additional record dimensions */
    nco_mrd_allow  /* 1 Allow multiple record dimensions when operator produces them naturally */
  }; /* end nco_mrd_cnv */

  enum nco_rth_cnv{ /* [enm] Arithmetic convention to assume */
    nco_rth_flt_flt, /* 0 Keep single-precision floating point (NCO default through version 4.3.5 20130927) */
    nco_rth_flt_dbl  /* 1 Promote single-precision floating point to double before arithmetic (NCO default since version 4.3.6 20130927) */
  }; /* end nco_rth_cnv */

  enum nco_baa_cnv{ /* [enm] Bit-Adjustment Algorithm to use */
    nco_baa_grm, /* 0 Bit Groom (NCO default since inception) */
    nco_baa_shv, /* 1 Bit Shave (option since 20160117) */
    nco_baa_set, /* 2 Bit Set (option since 20160117) */
    nco_baa_dgr, /* 3 Digit Rounding (GCD19 option since 20190121) */
    nco_baa_bg2, /* 4 Bit Groom version 2 dynamic masks (option since 20190203) */
    nco_baa_rnd, /* 5 Bit rounding */
    nco_baa_sh2, /* 6 Bit Half-shave  */
    nco_baa_gbg, /* 7 Brute force granular bit grooming */
  }; /* end nco_baa_cnv */

  enum nco_bnr_cnv{ /* [enm] Binary byte-ordering convention to employ (native or byte-swapped) */
    nco_bnr_ntv, /* 0 Write native byte-order of host machine (write big- and little-endian on big- and little-endian machines, respectively) */
    nco_bnr_bsa, /* 1 Use byte-swap algorithm and write non-native order (write little- and big-endian on big- and little-endian machines, respectively) */
  }; /* end nco_bnr_cnv */

  enum nco_upk_cnv{ /* [enm] Unpacking convention to utilize */
    /* netCDF convention  : http://www.unidata.ucar.edu/software/netcdf/docs/netcdf/Attribute-Conventions.html
       HDF/NASA convention: http://modis-atmos.gsfc.nasa.gov/MOD08_D3/faq.html */
    nco_upk_netCDF, /* 0 netCDF unpack convention: unpacked=(scale_factor*packed)+add_offset */
    nco_upk_HDF_MOD10, /* 1 HDF MODIS MOD10 unpack convention: unpacked=scale_factor*(packed-add_offset) */
    nco_upk_HDF_MOD13, /* 2 HDF MODIS MOD13 unpack convention: unpacked=(packed-add_offset)/scale_factor */
  }; /* end nco_upk_cnv */

  typedef enum aed{ /* [enm] Attribute editor mode */
    aed_append,
    aed_create,
    aed_delete,
    aed_modify,
    aed_nappend,
    aed_overwrite
  } aed_enm; /* end aed enum */
  
  typedef enum gpe{ /* [enm] Group Path Editing mode */
    gpe_append,
    gpe_delete,
    gpe_flatten,
    gpe_backspace
  } gpe_enm; /* end gpe enum */

  typedef enum { /* [enm] Memory allocation type */
    nco_mmr_calloc, /* [enm] nco_calloc() */
    nco_mmr_free, /* [enm] nco_free() */
    nco_mmr_malloc, /* [enm] nco_malloc() */
    nco_mmr_realloc /* [enm] nco_realloc() */
  } nco_mmr_typ_enm; /* end nco_mmr_typ enum */

  typedef enum { /* [enm] Arithmetic precision rank */
    /* Ranked ordering of "arithmetic precision" from lowest to highest */
    nco_rth_prc_rnk_string, /* [enm] */ /* Least precise */
    nco_rth_prc_rnk_char, /* [enm] */
    nco_rth_prc_rnk_ubyte, /* [enm] */
    nco_rth_prc_rnk_byte, /* [enm] */
    nco_rth_prc_rnk_ushort, /* [enm] */
    nco_rth_prc_rnk_short, /* [enm] */
    nco_rth_prc_rnk_uint, /* [enm] */
    nco_rth_prc_rnk_int, /* [enm] */
    nco_rth_prc_rnk_uint64, /* [enm] */
    nco_rth_prc_rnk_int64, /* [enm] */
    nco_rth_prc_rnk_float, /* [enm] */
    nco_rth_prc_rnk_double /* [enm] */ /* Most precise */
  } nco_rth_prc_rnk_enm; /* end nco_rth_prc_rnk enum */

  typedef enum { /* [enm] Timer flag */
    nco_tmr_srt, /* [enm] Initialize timer (first timer call) */
    nco_tmr_mtd, /* [enm] Metadata timer (second timer call) */
    nco_tmr_rgl, /* [enm] Regular timer call (main loop timer call) */
    nco_tmr_end /* [enm] Close timer (last timer call) */
  } nco_tmr_flg; /* [enm] Timer flag */

  typedef enum { /* [enm] Node enumerator Nie02 nodeEnum */
    typ_scv, /* [enm] Scalar value */
    typ_sym, /* [enm] Symbol identifier */
    typ_opr /* [enm] Operator */
  } nod_typ_enm; /* end Node enumerator */
  /* end enumeration section */

  typedef enum { /* [enm] Calendar type */
    cln_std=1, /* Standard mixed Julian/Gregorian */
    cln_grg, /* Gregorian Calendar */ 
    cln_jul, /* Julian Calendar */ 
    cln_360, /* 30-day month Calendar */ 
    cln_365, /* No Leap year Calendar */ 
    cln_366, /* Leap year Calendar */ 
    cln_nil /* No calendar found */
  } nco_cln_typ; /* [enm] Calendar type */

  typedef enum { /* [enm] Date format */
    fmt_dt_nil=0, /* None specified */
    fmt_dt_sht, /* Shortest string possible */
    fmt_dt_rgl, /* All date and time fields */
    fmt_dt_iso8601, /* Include ISO 8601 'T' separator between date and time */
  } nco_fmt_dt;

  /* Limit structure */
  typedef struct { /* lmt_sct */
    char *nm; /* [sng] Dimension name */
    char *nm_fll; /* [sng] Full dimension name */
    char *grp_nm_fll; /* [sng] Full group where dimension is defined. Added for group support from original netCDF3 */

    char *max_sng; /* User-specified string for dimension maximum */
    char *min_sng; /* User-specified string for dimension minimum */
    char *ilv_sng; /* User-specified string for interleave stride */
    char *ssc_sng; /* User-specified string for dimension subcycle */
    
    char *rbs_sng; /* Used by ncra, ncrcat to re-base record coordinate (holds unit attribute from first file) */
    char *srd_sng; /* User-specified string for dimension stride */

    double max_val; /* Double precision representation of maximum value of coordinate requested or implied */
    double min_val; /* Double precision representation of minimum value of coordinate requested or implied */
    double origin; /* Used by ncra, ncrcat to re-base record coordinate */

    int id; /* Dimension ID */
    int lmt_typ; /* crd_val or dmn_idx */

    long cnt; /* # of valid elements in this dimension (including effects of stride and wrapping) */
    long end; /* Index to end of hyperslab */
    long ilv; /* Interleave stride */
    long max_idx; /* Index of maximum requested value in dimension */
    long min_idx; /* Index of minimum requested value in dimension */
    long srd; /* Stride of hyperslab */
    long srt; /* Index to start of hyperslab */
    long ssc; /* Subcycle of hyperslab */

    /* Following six counters are used only by multi-file operators ncra and ncrcat: */
    long idx_end_max_abs; /* [idx] Maximum allowed index in record dimension (multi-file record dimension only) */
    long rec_dmn_sz; /* [nbr] Number of records in this file (multi-file record dimension only) */
    long rec_in_cml; /* [nbr] Cumulative number of records in all files opened so far (multi-file record dimension only) */
    long rec_rmn_prv_ssc; /* [nbr] Records remaining-to-be-read to complete subcycle group from previous file (multi-file record dimension only) */
    long rec_skp_ntl_spf; /* [nbr] Records skipped in initial superfluous files (multi-file record dimension only) */
    long rec_skp_vld_prv; /* [nbr] Records skipped since previous good one (multi-file record dimension only) */

    nco_bool flg_ilv; /* True for interleaved output (used by ncra and ncrcat only) */
    nco_bool flg_input_complete; /* True for multi-file operators when no more files need be opened */
    nco_bool flg_mro; /* True for multi-record output (used by ncra only) */
    nco_bool flg_mso; /* True for multi-subcycle output (used by ncra only) */

    nco_bool is_rec_dmn; /* True if record dimension, else False */
    nco_bool is_usr_spc_lmt; /* True if any part of limit is user-specified, else False */
    nco_bool is_usr_spc_max; /* True if user-specified, else False */
    nco_bool is_usr_spc_min; /* True if user-specified, else False */

    nco_cln_typ cln_typ; /* Used by ncra, ncrcat to store enum of calendar-type attribute */

  } lmt_sct;
  
  /* Convention structure */
  typedef struct{ /* cnv_sct */
    nco_bool CCM_CCSM_CF; /* "CF" in "Conventions" attribute */
    nco_bool MPAS; /* "MPAS" in "Conventions" attribute */
    nco_bool Group; /* "Groups" in "Conventions" attribute */
    float cf_vrs; /* [enm] CF-Version */
  } cnv_sct;
  
  /* Name ID structure */
  typedef struct{ /* nm_id_sct */
    char *nm; /* Variable (stub name) */
    int id; /* [id] Variable ID */
    int grp_id_in; /* [id] Group ID in input file */
    int grp_id_out; /* [id] Group ID in output file */
  } nm_id_sct;
  
  /* Rename structure */
  typedef struct{ /* rnm_sct */
    char *old_nm;
    char *new_nm;
    int id;
  } rnm_sct;

  /* DDRA information structure */
  typedef struct{ /* ddra_info_sct */
    int nco_op_typ; /* [enm] Operation type */
    int rnk_avg; /* [nbr] Rank of averaging space */
    int rnk_var; /* [nbr] Variable rank (in input file) */
    int rnk_wgt; /* [nbr] Rank of weight */
    int var_idx; /* [enm] Index */
    int wrd_sz; /* [B] Bytes per element */
    long long lmn_nbr; /* [nbr] Variable size */
    long long lmn_nbr_avg; /* [nbr] Averaging block size */
    long long lmn_nbr_wgt; /* [nbr] Weight size */
    nco_bool flg_ddra; /* [flg] Run DDRA diagnostics */
    nco_bool MRV_flg; /* [flg] Avergaging dimensions are MRV dimensions */
    nco_bool wgt_brd_flg; /* [flg] Broadcast weight for this variable */
    nco_tmr_flg tmr_flg; /* [enm] Timer flag */
  } ddra_info_sct;
  
  /* Pointer union */
  typedef union{ /* ptr_unn */
    float * restrict fp;
    double * restrict dp;
    nco_int * restrict ip;
    short * restrict sp;
    nco_char * restrict cp;
    nco_byte * restrict bp;
    nco_ubyte * restrict ubp;
    nco_ushort * restrict usp;
    nco_uint * restrict uip;
    nco_int64 * restrict i64p;
    nco_uint64 * restrict ui64p;
    nco_string * restrict sngp;
    nco_vlen * restrict vlnp;
    nco_opaque * restrict opqp;
    nco_enum * restrict enmp;
    nco_compound * restrict cpdp;
    void * restrict vp;
  } ptr_unn;

  /* Value union */
  typedef union{ /* val_unn */
    float f;
    double d;
    nco_int i;
    short s;
    nco_char c;
    nco_byte b;
    nco_ubyte ub;
    nco_ushort us;
    nco_uint ui;
    nco_int64 i64;
    nco_uint64 ui64;
    nco_string sng;
  } val_unn;

  /* Scalar value structure */
  typedef struct{ /* scv_sct */
    val_unn val; /* [sct] Value */
    nc_type type; /* [enm] netCDF type */
    nod_typ_enm nod_typ; /* [enm] Node type */
  } scv_sct;      

  /* Group Path Editing (GPE) structure */
  typedef struct{ /* gpe_sct */
    char *arg; /* [sng] Full GPE specification (for debugging) */
    char *edt; /* [sng] Editing component of full GPE specification */
    char *nm; /* [sng] Group name component of full GPE specification */
    char *nm_cnn; /* [sng] Canonicalized (i.e., slash-terminated) group name */
    gpe_enm md; /* [enm] Editing mode to perform */
    short lvl_nbr; /* [nbr] Number of levels to shift */
    size_t lng; /* [nbr] Length of user-specified group path */
    size_t lng_cnn; /* [nbr] Length of canonicalized user-specified group path */
    size_t lng_edt; /* [nbr] Length of editing component of full GPE specification */
  } gpe_sct;

  /* Attribute editing structure */
  typedef struct{ /* aed_sct */
    char *att_nm; /* Name of attribute */
    char *var_nm; /* Name of variable, or NULL for global attribute */
    int id; /* Variable ID or NC_GLOBAL (= -1) for global attribute */
    long sz; /* Number of elements in attribute */
    nc_type type; /* Type of attribute */
    ptr_unn val; /* Pointer to attribute value */
    aed_enm mode; /* Action to perform with attribute */
  } aed_sct;
  
  /* MD5 flags structure */
  typedef struct{ /* md5_sct */
    char *att_nm; /* [sng] MD5 attribute name */
    nco_bool dgs; /* [flg] Perform MD5 digests */
    nco_bool wrt; /* [flg] Write MD5 digests as attributes */
  } md5_sct;

  /* Print flags structure */
  typedef struct{ /* prn_fmt_sct */
    char *dlm_sng; /* [sng] User specified delimiter string for printed output */
    char *fl_in; /* [sng] Input filename */
    char *fl_stb; /* [sng] Input filename stub */
    char *fmt_val; /* [sng] Format string for variable values */
    char *smr_sng; /* [sng] Summary string */
    char *smr_fl_sz_sng; /* [sng] String describing estimated file size */
    char *spr_chr; /* [sng] Separator string for character types */
    char *spr_nmr; /* [sng] Separator string for numeric types */
    FILE *fp_out; /* [fl] Formatted text output file handle */
    gpe_sct *gpe; /* I [sng] GPE structure */
    md5_sct *md5; /* [flg] MD5 configuration */
    nco_bool cdl; /* [flg] Print CDL */
    nco_bool srm; /* [flg] Print ncStream */
    nco_bool trd; /* [flg] Print traditional NCO format */
    nco_bool jsn; /* [flg] Print JSON */
    nco_bool xml; /* [flg] Print XML (NcML) */
    nco_bool hdn; /* [flg] Print hidden attributes */
    nco_bool udt; /* [flg] Print non-atomic variables */
    nco_bool rad; /* [flg] Retain all dimensions */
    nco_bool xml_lcn; /* [flg] Print XML location tag */
    nco_bool nfo_xtr; /* [flg] Print extra information in CDL/XML mode */
    nco_bool new_fmt; /* [flg] Print in new format */
    nco_bool nwl_pst_val; /* [flg] Print newline after variable values */
    int cdl_fmt_dt; /* [enm] CDL date-stamp format specifier */ 
    int fl_out_fmt; /* [enm] Output file format */
    int fll_pth; /* [nbr] Print full paths */
    int jsn_att_fmt; /* [enm] JSON format for netCDF attributes: 0 (no object, only data), 1 (data only for string, char, int, and floating-point types, otherwise object), 2 (always object) */
    int jsn_data_brk; /* [flg] JSON format for netCDF variables: 0 (no bracketing of var data ), 1 ( bracketing of var data ) */
    int jsn_var_fmt; /* [flg] JSON format for netCDF variables: 0 (no type except for user-defined types), 1 (type for non-default types), 2 (always type) */
    int nbr_zro; /* [nbr] Trailing zeros allowed after decimal point */
    int ndn; /* [nbr] Indentation */
    int spc_per_lvl; /* [nbr] Indentation spaces per group level */
    int sxn_fst; /* [nbr] Offset of section from group name */
    int tab; /* [nbr] Number of spaces in tab */
    int var_fst; /* [nbr] Offset of variable from section name */
    nco_bool ALPHA_BY_FULL_GROUP; /* [flg] Print alphabetically by full group */
    nco_bool ALPHA_BY_FULL_OBJECT; /* [flg] Print alphabetically by full object */
    nco_bool ALPHA_BY_STUB_GROUP; /* [flg] Print alphabetically by stub group */
    nco_bool ALPHA_BY_STUB_OBJECT; /* [flg] Print alphabetically by stub object */
    nco_bool FORTRAN_IDX_CNV; /* Option F */
    nco_bool PRN_DMN_IDX_CRD_VAL; /* [flg] Print leading dimension/coordinate indices/values Option Q */
    nco_bool PRN_DMN_UNITS; /* [flg] Print dimensional units Option u */
    nco_bool PRN_DMN_VAR_NM; /* [flg] Print dimension/variable names */
    nco_bool PRN_GLB_METADATA; /* [flg] Print global metadata */
    nco_bool PRN_MSS_VAL_BLANK; /* [flg] Print missing values as blanks */
    nco_bool PRN_VAR_DATA; /* [flg] Print variable data */
    nco_bool PRN_VAR_METADATA; /* [flg] Print variable metadata */
    nco_bool PRN_CLN_LGB; /* [flg] Print UDUnits-formatted calendar dates/times human-legibly */
  } prn_fmt_sct;
  
  /* Types used in regrid structure */
  typedef enum nco_grd_2D_typ_enm{ /* [enm] Two-dimensional grid-type enum */
    nco_grd_2D_nil=0,
    nco_grd_2D_gss, /* Gaussian latitudes used by global spectral models: CCM 1-3, CAM 1-3, LSM, MATCH, UCICTM */
    nco_grd_2D_fv, /* FV scalar grid (lat[0]=-90): CAM FV, GEOS-CHEM, UCICTM, UKMO */
    nco_grd_2D_eqa, /* Equi-angle offset grid, FV staggered velocity grid (lat[0]=-89.X)): CIESIN/SEDAC, IGBP-DIS, TOMS AAI */
    nco_grd_2D_unk, /* Unknown or unclassified, POP displaced-pole */
  } nco_grd_2D_typ_enm;

  typedef enum nco_grd_lat_drc_enm{ /* [enm] Latitude direction enum */
    nco_grd_lat_drc_nil=0,
    nco_grd_lat_drc_s2n, /* South-to-North: Everything but ERAI, ERA5 */ 
    nco_grd_lat_drc_n2s, /* North-to-South: Used by ERAI, ERA5 */
  } nco_grd_lat_drc_enm;

  typedef enum nco_grd_lat_typ_enm{ /* [enm] Latitude grid-type enum */
    nco_grd_lat_nil=0,
    nco_grd_lat_unk, /* Unknown or unclassified latitude grid type (e.g., curvilinear) */ 
    nco_grd_lat_gss, /* Gaussian latitude grid used by global spectral models: CCM 1-3, CAM 1-3, LSM, MATCH, UCICTM */
    nco_grd_lat_fv, /* FV latitude grid. Equi-angle (except at poles) latitude grid with odd number of latitudes so poles are considered at (and labeled as) centers of first and last gridcells (i.e., lat_ctr[0]=-90), and those polar gridcells span half the equi-angular latitude increment, aka FV scalar grid: CAM FV, GEOS-CHEM, UCICTM, UKMO */
    nco_grd_lat_eqa, /* Equi-Angular latitude grid. Equi-angle (everywhere) latitude grid. Poles are at edges of first and last gridcells (i.e., lat_ctr[0]=-89.xxx). AKA FV-staggered velocity grid. Used by CIESIN/SEDAC, IGBP-DIS, TOMS AAI */
  } nco_grd_lat_typ_enm;

  typedef enum nco_grd_lon_typ_enm{ /* [enm] Longitude grid-type enum */
    nco_grd_lon_nil=0,
    nco_grd_lon_unk, /* Unknown or unclassified longitude grid type (e.g., curvilinear) */
    nco_grd_lon_180_wst, /* Date line at west edge of first longitude cell */
    nco_grd_lon_180_ctr, /* Date line at center of first longitude cell */
    nco_grd_lon_Grn_wst, /* Greenwich at west edge of first longitude cell */
    nco_grd_lon_Grn_ctr, /* Greenwich at center of first longitude cell */
    nco_grd_lon_bb, /* Longitude grid determined by bounding box (lon_wst/lon_est) and gridcell number (lon_nbr) */
  } nco_grd_lon_typ_enm;

  typedef enum nco_ntp_typ_enm 
    { /* [enm] Interpolation type enum */
     nco_ntp_lnr=0, // Perform linear interpolation in vertical coordinate (whether pressure or height)
     nco_ntp_log // Perform logarithmic interpolation in vertical coordinate (whether pressure or height)
    } nco_ntp_typ_enm;

  typedef enum nco_ply_tri_mth_typ_enm{ /* [enm] Polygon-to-triangle decomposition method enum */
    nco_ply_tri_mth_nil=0,
    nco_ply_tri_mth_csz, /* CSZ's advancing polygon bisector method developed at UCI */
    nco_ply_tri_mth_ctr, /* Centroid-based method used by MAT at SNL */
  } nco_ply_tri_mth_typ_enm;

  typedef enum nco_edg_typ_enm{ /* [enm] Edge-type/arc-type that connects grid vertices */
    nco_edg_nil=0, /* [enm] Edges/arcs are of unspecified type */
    nco_edg_gtc, /* [enm] Edges/arcs are great circle segments */
    nco_edg_smc, /* [enm] Edges/arcs of constant latitude are small circle segments, all others are great circle segments */
    nco_edg_crt, /* [enm] Edges/arcs are Cartesian/planar lines and neglect curvature */
  } nco_edg_typ_enm;

  typedef enum nco_wgt_typ_enm{
    nco_wgt_nil=0,
    nco_wgt_con, /* Conservative algorithm */
    nco_wgt_dwe, /* Distance-weighted extrapolation */
    nco_wgt_bln  /* Bilinear interpolation */
  } nco_wgt_typ_enm;

typedef enum nco_xtr_typ_enm
    { /* [enm] Extrapolation type enum */
     nco_xtr_fll_lnr=0, // Perform linear extrapolation using two nearest valid neighbors
     nco_xtr_fll_ngh, // Set extrapolated value to value of nearest valid neighbor
     nco_xtr_fll_nil, // Set extrapolated value to 0.0
     nco_xtr_fll_tpt, // Temperature extrapolation assuming moist adiabatic lapse rate dT/dp=constant=(6.5 K)/(10000 Pa)
     nco_xtr_fll_gph, // Geopotential height extrapolation assuming hydrostatic equation dZ/dp=-RT/pg
     nco_xtr_fll_msv, // Set extrapolated value to missing value
     nco_xtr_prt_frc, // Use average value of overlap region
     nco_xtr_prt_lnr, // Perform linear extrapolation using two nearest valid neighbors
     nco_xtr_prt_ngh, // Set extrapolated value to value of nearest valid neighbor
     nco_xtr_prt_nil, // Set extrapolated value to 0.0
     nco_xtr_prt_wgt // Set extrapolated value to average value of overlap region weighted by size of overlap region plus 0.0 weighted by size of non overlap region (implies xtr_frc)
    } nco_xtr_typ_enm;

  /* CF Coordinates structure (20160503: Used only in ncks.c for to infer grids from CF coordinates convention) */
  typedef struct{ /* cr_crd_sct */
    char *crd_nm[2]; /* [sng] Coordinate names */
    char *crd_sng; /* [sng] Coordinates attribute value */
    char *dmn_nm[2]; /* [sng] Dimension names */
    char *unt_sng[2]; /* [sng] Units strings */
    char *var_nm; /* [sng] Coordinates variable name */
    int crd_id[2]; /* [id] Coordinate IDs */
    int dmn_id[2]; /* [id] Dimension IDs */
    int var_id; /* [id] Coordinate variable ID */
    nc_type var_type; /* [enm] Coordinates variable type */
    nco_bool crd; /* [flg] CF coordinates information is complete */
  } cf_crd_sct; /* end CF coordinates structure */

  /* Regrid structure */
  typedef struct{ /* rgr_sct */
    // File names specifiable with individual command line switches
    char *drc_tps; /* [sng] Data directory for Tempest grids, meshes, and weights */
    char *fl_grd_src; /* [sng] File containing input grid */
    char *fl_grd_dst; /* [sng] File containing destination grid */
    char *fl_hrz; /* [sng] File containing horizontal coordinate grid */
    char *fl_in; /* [sng] File containing fields to be regridded */
    char *fl_out; /* [sng] File containing regridded fields */
    char *fl_out_tmp; /* [sng] Temporary file containing regridded fields */
    char *fl_map; /* [sng] File containing mapping weights from source to destination grid */
    char *fl_vrt; /* [sng] File containing vertical coordinate grid */
    char *fl_msh;  /* [sng] File that contains SCRIP formatted overlap mesh */
    char *var_nm; /* [sng] Variable for special regridding treatment */
    // Grid names specifiable with key-value syntax
    char **rgr_arg; /* [sng] Regridding arguments */
    char *area_nm; /* [sng] Name of variable containing gridcell area */
    char *bnd_nm; /* [sng] Name of dimension to employ for spatial bounds */
    char *bnd_tm_nm; /* [sng] Name of dimension to employ for temporal bounds */
    char *col_nm_in; /* [sng] Name to recognize as input horizontal spatial dimension on unstructured grid */
    char *col_nm_out; /* [sng] Name of horizontal spatial output dimension on unstructured grid */
    char *fl_hnt_dst; /* [sng] ERWG hint destination */
    char *fl_hnt_src; /* [sng] ERWG hint source */
    char *frc_nm; /* [sng] Name of variable containing gridcell fraction */
    char *ilev_nm_in; /* [sng] Name of input dimension to recognize as vertical dimension at layer interfaces */
    char *ilev_nm_out; /* [sng] Name of output vertical dimension at layer interfaces */
    char *lat_bnd_nm; /* [sng] Name of rectangular boundary variable for latitude */
    char *lat_dmn_nm; /* [sng] Name of latitude dimension in inferred grid */
    char *lat_nm_in; /* [sng] Name of input dimension to recognize as latitude */
    char *lat_nm_out; /* [sng] Name of output dimension for latitude */
    char *lat_vrt_nm; /* [sng] Name of non-rectangular boundary variable for latitude */
    char *lat_wgt_nm; /* [sng] Name of variable containing latitude weights */
    char *lev_nm_in; /* [sng] Name of input dimension to recognize as vertical dimension at layer midpoints */
    char *lev_nm_out; /* [sng] Name of output vertical dimension at layer midpoints */
    char *lon_bnd_nm; /* [sng] Name of rectangular boundary variable for longitude */
    char *lon_dmn_nm; /* [sng] Name of longitude dimension in inferred grid */
    char *lon_nm_in; /* [sng] Name of dimension to recognize as longitude */
    char *lon_nm_out; /* [sng] Name of output dimension for longitude */
    char *lon_vrt_nm; /* [sng] Name of non-rectangular boundary variable for longitude */
    char *msk_nm; /* [sng] Name of variable containing destination mask */
    char *plev_nm_in; /* [sng] Name of variable to recognize as vertical coordinate for pure pressure grids */
    char *sgs_frc_nm; /* [sng] Name of variable containing sub-gridscale fraction */
    char *sgs_msk_nm; /* [sng] Name of variable containing sub-gridscale mask */
    char *vrt_nm; /* [sng] Name of dimension to employ for vertices */
    // User-specified grid properties
    char *fl_grd; /* [sng] Name of SCRIP grid file to create */
    char *fl_ugrid; /* [sng] Name of UGRID grid file to create */
    char *fl_skl; /* [sng] Name of skeleton data file to create */
    char *grd_ttl; /* [sng] Grid title */
    char *msk_var; /* [sng] Mask-template variable */
    double lat_crv; /* [dgr] Latitudinal  curvilinearity */
    double lon_crv; /* [dgr] Longitudinal curvilinearity */
    double lat_sth; /* [dgr] Latitude of southern edge of grid */
    double lon_wst; /* [dgr] Longitude of western edge of grid */
    double lat_nrt; /* [dgr] Latitude of northern edge of grid */
    double lon_est; /* [dgr] Longitude of eastern edge of grid */
    double sgs_nrm; /* [frc] Sub-gridscale normalization */
    double xtr_xpn; /* [frc] Exponent of distance in extrapolation (absolute value) */
    long lat_nbr; /* [nbr] Number of latitudes in destination grid */
    long lon_nbr; /* [nbr] Number of longitudes in destination grid */
    long xtr_nsp; /* [nbr] Extrapolation number of source points */
    nco_grd_2D_typ_enm grd_typ; /* [enm] Destination grid-type enum */
    nco_grd_lat_drc_enm lat_drc; /* [enm] Latitude grid-direction enum */
    nco_grd_lat_typ_enm lat_typ; /* [enm] Latitude grid-type enum */
    nco_grd_lon_typ_enm lon_typ; /* [enm] Longitude grid-type enum */
    nco_ntp_typ_enm ntp_mth; /* [enm] Interpolation method */
    nco_ply_tri_mth_typ_enm ply_tri_mth; /* [enm] Polygon-to-triangle decomposition method */ 
    nco_edg_typ_enm edg_typ; /* [enm] Arc-type for triangle edges */
    nco_xtr_typ_enm xtr_mth; /* [enm] Extrapolation method */
    nco_wgt_typ_enm wgt_typ; /* [enm] Weight generation type */
    // Other internal data and metadata 
    char **xtn_var; /* [sng] Extensive variables */
    char *cmd_ln; /* [sng] Command-line */
    double wgt_vld_thr; /* [frc] Weight threshold for valid destination value */
    int dfl_lvl; /* [enm] Deflate level */
    int fl_out_fmt; /* [enm] Output file format */
    int in_id; /* [id] Input netCDF file ID */
    int out_id; /* [id] Output netCDF file ID */
    int rgr_nbr; /* [nbr] Number of regridding arguments */
    int thr_nbr; /* [nbr] Thread number */
    int xtn_nbr; /* [nbr] Number of extensive variables */
    int area_mth; /* [enm]  area method used calculating grid cell area */
    long idx_dbg; /* [idx] Index of gridcell for debugging */
    long tst; /* [enm] Generic key for testing (undocumented) */
    nco_bool flg_area_out; /* [flg] Add area to output */
    nco_bool flg_cll_msr; /* [flg] Add cell_measures attribute */
    nco_bool flg_crv; /* [flg] Use curvilinear coordinates */
    nco_bool flg_dgn_area; /* [flg] Diagnose rather than copy inferred area */
    nco_bool flg_dgn_bnd; /* [flg] Diagnose rather than copy inferred bounds */
    nco_bool flg_cf_units; /* [flg] Generate CF-compliant (breaks ERWG 7.1.0r-) units fields in SCRIP-format grid files */
    nco_bool flg_erwg_units; /* [flg] Generate ERWG 7.1.0r-compliant SCRIP-format grid files */
    nco_bool flg_grd; /* [flg] Create SCRIP-format grid file */
    nco_bool flg_grd_dst; /* [flg] User-specified destination grid */
    nco_bool flg_grd_src; /* [flg] User-specified input grid */
    nco_bool flg_msk_out; /* [flg] Add mask to output */
    nco_bool flg_nfr; /* [flg] Infer SCRIP-format grid file */
    nco_bool flg_rnr; /* [flg] Renormalize destination values by valid area */
    nco_bool flg_s1d; /* [flg] Unpack sparse-1D CLM/ELM variables */
    nco_bool flg_stg; /* [flg] Write staggered grid with FV output */
    nco_bool flg_uio; /* [flg] Unbuffered I/O (NC_SHARE) for netCDF3 files */
    nco_bool flg_usr_rqs; /* [flg] User-requested regridding */
    nco_bool flg_wgt; /* [flg] User-specified mapping weights */
    size_t hdr_pad; /* [B] Pad at end of header section (for map-file only) */
  } rgr_sct; /* end Regrid structure */

  /* Key-value structure */
  typedef struct{
    char *key;
    char *val;
  } kvm_sct;
 
  /* Attribute structure */
  typedef struct{ /* att_sct */
    char *nm;
    nc_type type;
    long sz;
    char fmt[5];
    ptr_unn val;
  } att_sct;

  /* Per-dimension chunking specification (free with nco_cnk_dmn_free()) */
  typedef struct{ /* cnk_dmn_sct */
    char *nm; /* [sng] Dimension name */
    char *nm_fll; /* [sng] Full dimension name */
    size_t sz; /* [nbr] Chunk size */
    nco_bool is_usr_spc_cnk; /* [flg] Chunk size was user-specified */
  } cnk_dmn_sct;
  
  /* Chunking structure */
  typedef struct{ /* cnk_sct */
    nco_bool flg_usr_rqs; /* [flg] User requested chunking */
    int cnk_nbr; /* [nbr] Number of user-specified chunk sizes */
    cnk_dmn_sct **cnk_dmn; /* [sct] User-specified per-dimension chunking information */
    int cnk_map; /* [enm] Chunking map */
    int cnk_plc; /* [enm] Chunking policy */
    size_t cnk_csh_byt; /* [B] Chunk cache size */
    size_t cnk_min_byt; /* [B] Minimize size of variable to chunk */
    size_t cnk_sz_byt; /* [B] Chunk size in Bytes */
    size_t cnk_sz_scl; /* [nbr] Chunk size scalar */
  } cnk_sct;
  
  /* GTT structure to break full path name into components */
  typedef struct{ 
    char *nm;       /* [sng] Path component */
    int psn;        /* [nbr] Position of path component */
  } sng_pth_sct; 

  /* MSA Limits structure:
     GTT has a member for every unique dimension and for every coordinate variable */
  typedef struct { 
    char *dmn_nm;         /* [sng] Dimension name */
    long dmn_sz_org;      /* [nbr] Original size of dimension */
    long dmn_cnt;         /* [nbr] Hyperslabbed size of dimension */  
    nco_bool NON_HYP_DMN; /* [flg] Limit is same as dimension in input file */
    nco_bool WRP;         /* [flg] Limit is wrapped (true if wrapping, lmt_dmn_nbr==2) */ 
    nco_bool MSA_USR_RDR; /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
    int lmt_dmn_nbr;      /* [nbr] Number of lmt arguments */
    lmt_sct **lmt_dmn;    /* [sct] List of limit structures associated with each dimension */
    int lmt_crr;          /* [nbr] Index of current limit structure being initialized (helper to initialize lmt_sct *) */
  } lmt_msa_sct;

  /* GTT coordinate variable structure; it contains netCDF model fields and an MSA field */
  typedef struct{ 
    char *crd_nm_fll;       /* [sng] Full coordinate name */
    char *dmn_nm_fll;       /* [sng] Full name of dimension for *this* coordinate  */  
    char *crd_grp_nm_fll;   /* [sng] Full group name where coordinate is located */
    char *dmn_grp_nm_fll;   /* [sng] Full group name where dimension of *this* coordinate is located */
    char *nm;               /* [sng] Name of dimension and coordinate */
    nco_bool is_rec_dmn;    /* [flg] Is a record dimension? */
    size_t sz;              /* [nbr] Size of coordinate */
    nc_type var_typ;        /* [enm] NetCDF type  */  
    lmt_msa_sct lmt_msa;    /* [sct] MSA Limits structure for every coordinate */
    int dmn_id;             /* [ID] Unique ID for dimension (duplicate of dmn_trv_sct.dim_id, since all coordinates share that ID) */
    int grp_dpt;            /* [nbr] Depth of group (root = 0); needed to get in scope of variable match */
  } crd_sct; 

  /* GTT dimension structure (stored in *groups*); it contains netCDF model fields and an MSA field */
  typedef struct{ 
    char *grp_nm_fll;        /* [sng] Full group name where dimension was defined */
    char *nm_fll;            /* [sng] Dimension fully qualified name (path) */
    char *nm;                /* [sng] Dimension name */
    nco_bool is_rec_dmn;     /* [flg] Is a record dimension? */
    size_t sz;               /* [nbr] Size of dimension */   
    int crd_nbr;             /* [nbr] Number of coordinate structures */
    crd_sct **crd;           /* [sct] List of coordinate structures */
    lmt_msa_sct lmt_msa;     /* [sct] MSA Limits structure (implicit that is for non-coordinate case) */
    int dmn_id;              /* [ID] Dimension ID; same as "var_dmn_sct.id" from nc_inq_dimid() */
    nco_bool has_crd_scp;    /* [flg] Does variable with same name exist in dimension's scope? */
    nco_bool flg_xtr;        /* [flg] Extract dimension */
  } dmn_trv_sct; 

  /* Build a list of 'standard_name' 'latitude' and 'longitude' coordinates (Auxiliary Coordinates) */
  typedef struct{
    char *nm_fll;              /* [sng] Coordinate full name ('latitude' or 'longitude') */ 
    int dmn_id;                /* [id] Dimension ID of dimension of 'latitude' and 'longitude' coordinate variables, e.g., lat_gds(gds_crd) */
    char units[NC_MAX_NAME+1]; /* [sng] Units of 'latitude' and 'longitude' */ 
    nc_type crd_typ;           /* [enm] netCDF type of both "latitude" and "longitude" */
    int grp_dpt;               /* [nbr] Depth of group (root = 0) */
  } aux_crd_sct; 

  /* GTT Variable dimensions:
     Dimensions have name and size, and maybe an associated (coordinate) variable */
  typedef struct{ 
    char *dmn_nm_fll;        /* [sng] Full dimension name */
    char *dmn_nm;            /* [sng] Dimension name */
    char *grp_nm_fll;        /* [sng] Full group where dimension is defined */
    nco_bool is_rec_dmn;     /* [flg] Is a record dimension? */
    nco_bool is_crd_var;     /* [flg] Dimension has an associated coordinate variable */
    crd_sct *crd;            /* [sct] Pointer to coordinate variable if any */
    dmn_trv_sct *ncd;        /* [sct] Pointer to non-coordinate dimension if any */
    int dmn_id;              /* [ID] Dimension ID; same as dmn_trv_sct.id from nc_inq_vardimid() */

    aux_crd_sct *lat_crd;    /* [lst] Array of 'latitude' coordinates */
    aux_crd_sct *lon_crd;    /* [lst] Array of 'longitude' coordinates */
    int nbr_lat_crd;         /* [nbr] Number of items in 'latitude' coordinates array */
    int nbr_lon_crd;         /* [nbr] Number of items in 'longitude' coordinates array */

    /* Following are members only used by transformation operators (non-ncks) */
    nco_bool flg_dmn_avg;    /* [flg] Diferentiate between dimensions to average or keep for this variable (ncwa) */  
    nco_bool flg_rdd;        /* [flg] Retain dimension as degenerate (size 1) (ncwa) */  
  } var_dmn_sct; 

  /* Processing type enumerator */
  typedef enum {
    err_typ=-1,             /* -1 [enm] Invalid type for initialization */
    fix_typ,                /*  0 [enm] Fixed variable (operator alters neither data nor metadata) */
    prc_typ                 /*  1 [enm] Process variable (operator may alter data and metadata) */
  } prc_typ_enm; 

  /* GTT Object structure 
     Information for each object/node in traversal tree
     Contains basic information about this object/node needed by traversal algorithm
     Node/object is either group or variable like in HDF5
     Initialize trv_sct structure to defaults in trv_tbl_init()
     Populate trv_sct structure with correct values in nco_grp_itr()
     Deep-copy each pointer member of trv_sct structure in nco_grp_itr()
     free() each pointer member of trv_sct structure in trv_tbl_free() */
  typedef struct{ 
    nco_obj_typ nco_typ;              /* [enm] netCDF4 object type: group or variable */
    char *nm_fll;                     /* [sng] Fully qualified name (path) */
    var_dmn_sct *var_dmn;             /* [sct] Dimensions for variable object */
    nco_bool is_crd_lk_var;           /* [flg] Is a coordinate-like variable (same as var_sct is_crd_var: crd, 2D, bounds...) */
    nco_bool is_rec_lk_var;           /* [flg] Is a record variable of any dimension (same as var_sct is_rec_var) */
    nco_bool is_1D_crd;               /* [flg] Is a 1D coordinate variable? (unique dimension exists in scope) WARNING: Not yet defined */
    nco_bool is_1D_rec_crd;           /* [flg] Is a 1D record coordinate (e.g., time)? WARNING: Not yet defined */
    nco_bool is_crd_var;              /* [flg] Is a coordinate variable? (OLD incompatible definition meant is_1D_crd, new definition identical with is_crd_var in var_sct, i.e., coordinate-like variables, 2D, bounds...) */
    nco_bool is_rec_var;              /* [flg] Is a record variable? (OLD incorrect definition meant is_1D_rec_crd, new definition identical with is_rec_var in var_sct, i.e., any variable with a record dimension) */
    nc_type var_typ;                  /* [enm] netCDF type */
    size_t nm_fll_lng;                /* [sng] Length of full name */
    char *grp_nm_fll;                 /* [sng] Full group name (for groups, same as nm_fll) */
    char *grp_nm;                     /* [sng] Group name (for groups, same as nm) */
    char *nm;                         /* [sng] Relative name (i.e., variable name or last component of path name for groups) */
    size_t nm_lng;                    /* [sng] Length of short name */
    int grp_dpt;                      /* [nbr] Depth of group (root = 0) */
    int nbr_att;                      /* [nbr] Number of attributes */
    int nbr_dmn;                      /* [nbr] Number of dimensions */
    int nbr_rec;                      /* [nbr] Number of record dimensions */
    int nbr_grp;                      /* [nbr] (For groups only) Number of sub-groups */
    int nbr_typ;                      /* [nbr] (For groups only) Number of user-defined types defined in group */
    int nbr_var;                      /* [nbr] (For groups only) Number of variables */
    int ppc;                          /* [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
    nco_bool flg_nsd;                 /* [flg] PPC is NSD */

    /* Next five members introduced 20180415 for non-atomic variables */
    char *typ_nm; /* [sng] Type name used in CDL "types" declaration (e.g., "vlen_t") */
    int cls_typ; /* [enm] netCDF class type, same as var_typ except contiguous from 0..16 */
    nc_type bs_typ; /* [enm] netCDF atomic type underlying vlen and enum types */
    size_t fld_nbr; /* [nbr] Number of fields in enum and compound types */
    size_t typ_sz; /* [B] Size of user-defined type */

    nco_bool flg_cf;                  /* [flg] Object matches CF-metadata extraction criteria */
    nco_bool flg_crd;                 /* [flg] Object matches coordinate extraction criteria */
    nco_bool flg_dfl;                 /* [flg] Object meets default subsetting criteria */
    nco_bool flg_gcv;                 /* [flg] Group contains matched variable */
    nco_bool flg_mch;                 /* [flg] Object matches user-specified strings */
    nco_bool flg_mtd;                 /* [flg] Group contains only metadata */
    nco_bool flg_ncs;                 /* [flg] Group is ancestor of specified group or variable */
    nco_bool flg_nsx;                 /* [flg] Object matches intersection criteria */
    nco_bool flg_udt;                 /* [flg] Variable is non-atomic */
    nco_bool flg_rcr;                 /* [flg] Extract group recursively */
    nco_bool flg_rgr;                 /* [flg] Regrid variable */ 
    nco_bool flg_xtn;                 /* [flg] Extensive variable */ 
    nco_bool flg_mrv;                 /* [flg] Most-Rapidly-Varying horizontal dimensions variable */ 
    nco_bool flg_unn;                 /* [flg] Object matches union criteria */
    nco_bool flg_vfp;                 /* [flg] Variable matches full path specification */
    nco_bool flg_vsg;                 /* [flg] Variable selected because group matches */
    nco_bool flg_xcl;                 /* [flg] Object matches exclusion criteria */
    nco_bool flg_xtr;                 /* [flg] Extract object */ 
    nco_bool flg_aux;                 /* [flg] Variable contains auxiliary coordinates */ 
    nco_bool flg_std_att_lat;         /* [flg] Variable contains 'standard_name' attribute "latitude" */ 
    nco_bool flg_std_att_lon;         /* [flg] Variable contains 'standard_name' attribute "longitude" */ 

    /* Following are members only used by transformation operators (non-ncks) */
    prc_typ_enm enm_prc_typ;          /* [enm] Processing type enumerator */
    nc_type var_typ_out;              /* [enm] NetCDF type in output file (ncflint) (ncpdq) */  
    int *dmn_idx_out_in;              /* [nbr] Dimension correspondence, output->input (ncpdq); output of nco_var_dmn_rdr_mtd() */
    nco_bool *dmn_rvr_in;             /* [flg] Reverse dimensions (ncpdq) */
    nco_bool flg_rdr;                 /* [flg] Variable has dimensions to re-order (ncpdq) */ 
    char *rec_dmn_nm_out;             /* [sng] Record dimension name, re-ordered (ncpdq) (used as flag also for re-defined record dimension)*/
    char *grp_nm_fll_prn;             /* [sng] (ncge) Parent group full name */
    /* Good hash, dude */
    UT_hash_handle hh;                /* [sct] Handle for hash table */
    char *hsh_key;                    /* [sng] Hash key (must be unique!) */
    nco_bool flg_nsm_prn;             /* [flg] (ncge) Group is ensemble parent group */
    nco_bool flg_nsm_mbr;             /* [flg] (ncge) Variable or group is ensemble member */
    nco_bool flg_nsm_tpl;             /* [flg] (ncge) Variable is template */
    char *nsm_nm;                     /* [sng] (ncge) Ensemble parent group name i.e., full path to ensemble parent */ 
  } trv_sct;

  /* Fill actual value of dmn_sct structure in nco_dmn_fll()
     free() each pointer member of dmn_sct structure in nco_dmn_free()
     deep-copy each pointer member of dmn_sct structure in nco_dmn_dpl() */
  /* Dimension structure */
  typedef struct dmn_sct_tag{ /* dmn_sct */
    char *nm; /* [sng] Dimension name */
    char *nm_fll; /* [sng] Dimension full name */
    char fmt[5]; /* [sng] Hint for printf()-style formatting */
    int cid; /* [id] Variable ID of associated coordinate, if any */
    int id; /* [id] Dimension ID */
    int nc_id; /* [id] File ID */
    long cnt; /* [nbr] Number of valid elements in this dimension (including effects of stride and wrapping) */
    long end; /* [idx] Index to end of hyperslab */
    long srd; /* [nbr] Stride of hyperslab */
    long srt; /* [idx] Index to start of hyperslab */
    long sz; /* [nbr] Full size of dimension in file (NOT the hyperslabbed size) */
    nc_type type; /* [enm] Type of coordinate, if applicable */
    ptr_unn val; /* [sct] Buffer to hold hyperslab (used by nco_prn to hold coordinate values) */
    short is_crd_dmn; /* [flg] Is this a coordinate dimension? */
    short is_rec_dmn; /* [flg] Is this the record dimension? */
    size_t cnk_sz; /* [nbr] Chunk size */
    struct dmn_sct_tag *xrf; /* [sct] Cross-reference to associated dimension structure (usually the structure for dimension on output) */
  } dmn_sct; /* end dmn_sct_tag */

  /* Ensemble group structure (ncge). It contains a name for the group (e.g., /cesm/cesm_01) and a list of variables for the group */
  typedef struct{
    char *mbr_nm_fll;       /* [sng] Goup full name */ 
    char **var_nm_fll;      /* [sng] List of full variable names for this group (e.g., /cesm/cesm_01/tas) */
    int var_nbr;            /* [nbr] Number of variable for this group (size of above array) */ 
  } nsm_grp_sct; 

  /* Ensemble (ncge) */
  typedef struct{
    char *grp_nm_fll_prn;   /* [sng] Parent group full name (key for ensemble) (e.g., /cesm) */
    nsm_grp_sct *mbr;       /* [sng] List of ensemble group member structs (size is mbr_nbr) */
    int mbr_nbr;            /* [nbr] Number of members (groups) of ensemble (i.e., number in this ensemble in this file) */
    char **tpl_mbr_nm;      /* [sng] List of variable that act as templates (first, store relative name) (e.g., /cesm/cesm_01/tas) */
    int tpl_nbr;            /* [nbr] Number of variables that are templates in group */  
    char **skp_nm_fll;      /* [sng] Skip list (fixed variables to define at ensemble parent group, store full name) (e.g., /cesm/cesm_01/time)  */
    int skp_nbr;            /* [nbr] Number of skip list variables */ 
    int mbr_srt;            /* [nbr] Member offsets, start (multi files, keep track of new added members) */ 
    int mbr_end;            /* [nbr] Member offsets, end (multi files, keep track of new added members) */ 
  } nsm_sct; 
  
  /* Name structure */
  typedef struct{ /* nm_sct */
    char *nm;     /* [sng] Name */
  } nm_sct;

  /* Name list structure */
  typedef struct{ 
    nm_sct *lst;  /* [sct] List of nm_sct (char * currently) */
    int nbr;      /* [nbr] Number of items in "lst" array */
  } nm_lst_sct;

  /* GTT (Group Traversal Table) structure contains two lists
     1) lst: All objects (variables and groups) in file tree (HDF5 model)
     2) lst_dmn: All unique dimensions (in groups) in file tree (netCDF addition to HDF5) */
  typedef struct{
    trv_sct *lst;           /* [sct] Array of trv_sct */
    unsigned int nbr;       /* [nbr] Number of trv_sct elements */   
    dmn_trv_sct *lst_dmn;   /* [sct] Array of dmn_trv_sct */
    unsigned int nbr_dmn;   /* [nbr] Number of dmn_trv_sct elements */
    trv_sct *hsh;           /* [hsh] Hash table of all trv_sct objects */
    dmn_sct *dmn_dgn;       /* [sct] (ncwa) Degenerate dimensions */
    int nbr_dmn_dgn;        /* [sct] (ncwa) Number of degenerate dimensions (size of above array) */
    int nsm_nbr;            /* [nbr] (ncge) Number of ensembles (i.e., number in first file) */ 
    nsm_sct *nsm;           /* [lst] (ncge) List of ensembles (size is nsm_nbr) */ 
    char *nsm_sfx;          /* [sng] (ncge) Ensemble suffix (e.g., /cesm + _avg). Store here instead of passing as function parameters (ncge only) */
    int thr_nbr; /* [nbr] Thread number Option t */
    int *in_id_arr; /* [id] netCDF file IDs used by OpenMP code */
  } trv_tbl_sct;
 
  /* GPE duplicate name check structure */
  typedef struct{ /* gpe_nm_sct */
    char *var_nm_fll; /* [sng] Fully qualified variable name */
  } gpe_nm_sct;

  /* Common variable names; common defined as same absolute path in 2 files */
  typedef struct{		
    char *nm;                    /* [sng] A name */
    nco_bool flg_in_fl[2];       /* [flg] Is this name in each file?; files are [0] and [1] */
  } nco_cmn_t;

  /* cell_methods structure */
  typedef struct{ 
    char *dmn_nm;   /* [sng] Dimension name */
    int op_type;    /* [enm] Operation type */
  } cell_methods_sct; 

  /* Limit "lmt_sct" monotonic direction */
  typedef enum {
    decreasing, /* 0 */
    increasing, /* 1 */
    not_checked /* 2 */
  } monotonic_direction_enm;

   /* Structure to check for valid input dimension  */
  typedef struct {	
    nco_bool flg_dne;           /* [flg] Flag to check if input dimension -d "does not exist" */
    char *dim_nm;               /* [sng] Dimension name */    
  } nco_dmn_dne_t;

  /* Initialize default value of each member of var_sct structure in var_dfl_set()
     Fill actual value of var_sct structure in nco_var_fll()
     free() each pointer member of var_sct structure in nco_var_free()
     deep-copy each pointer member of var_sct structure in nco_var_dpl() */
  /* Variable structure */
  typedef struct var_sct_tag{ /* var_sct */
    char *nm; /* [sng] Variable name */
    char *nm_fll; /* [sng] Absolute variable name (needed for GTT search for object by full name) */
    char fmt[5]; /* [sng] Hint for printf()-style formatting */
    double *wgt_sum; /* [frc] Running sum (over records) of per-record or per-file weights (ncra/ncea only) */
    double wgt_crr; /* [frc] Weight of current record (ncra/ncea only) */
    dmn_sct **dim; /* [sct] Pointers to full dimension structures */
    int *dmn_id; /* [id] Contiguous vector of dimension IDs */
    int cid; /* [id] Dimension ID of associated coordinate, if any */
    int dfl_lvl; /* [enm] Deflate level [0..9] */
    int has_add_fst; /* [flg] Valid add_offset attribute exists */
    int has_dpl_dmn; /* [flg] Variable has duplicate copies of same dimension */
    int has_mss_val; /* [flg] Is there a missing_value attribute? */
    int has_scl_fct; /* [flg] Valid scale_factor attribute exists */
    int id; /* [id] Variable ID */
    int nbr_att; /* [nbr] Number of attributes */
    int nbr_dim; /* [nbr] Number of dimensions of variable in input file */
    int nc_id; /* [id] File ID */
    int pck_dsk; /* [flg] Variable is packed on disk (valid scale_factor, add_offset, or both attributes exist) */
    int pck_ram; /* [flg] Variable is packed in memory (valid scale_factor, add_offset, or both attributes exist) */
    int shuffle; /* [flg] Turn-on shuffle filter */
    int undefined; /* [flg] Variable is still undefined (in first parser pass) */
    long *cnt; /* [nbr] Contiguous vector of lengths of hyperslab */
    long *end; /* [idx] Contiguous vector of indices to end of hyperslab */
    long *srd; /* [nbr] Contiguous vector of stride of hyperslab */
    long *srt; /* [idx] Contiguous vector of indices to start of hyperslab */
    long *tally; /* [nbr] Number of valid operations performed so far */
    long sz; /* [nbr] Number of elements (NOT bytes) in hyperslab (NOT full size of variable in input file!) */
    long sz_rec; /* [nbr] Number of elements in one record of hyperslab */
    nc_type typ_dsk; /* [enm] Type of variable on disk (never changes) */
    nc_type typ_pck; /* [enm] Type of variable when packed (on disk). typ_pck = typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
    nc_type typ_upk; /* [enm] Type of variable when unpacked (expanded) (in memory) */
    nc_type type; /* [enm] Type of variable in RAM */
    ptr_unn add_fst; /* [frc] Value of add_offset attribute of type typ_upk */
    ptr_unn mss_val; /* [frc] Value of missing_value attribute, if any (mss_val stored in this structure must be same type as variable) */
    ptr_unn scl_fct; /* [frc] Value of scale_factor attribute of type typ_upk */
    ptr_unn val; /* [bfr] Buffer to hold hyperslab */
    short is_crd_var; /* [flg] Is this a coordinate variable? */
    short is_fix_var; /* [flg] Is this a fixed (non-processed) variable? */
    short is_rec_var; /* [flg] Is this a record variable? */
    size_t *cnk_sz; /* [id] Contiguous vector of chunk sizes */
    struct var_sct_tag *xrf; /* [sct] Cross-reference to associated variable structure (usually structure for variable on output) fxm: deprecate! TODO nco226 */
  } var_sct; /* end var_sct_tag */

  /* Utility structure to share dimension common fields; used in nco_cnk_sz_set_trv() */
  typedef struct{
    char *nm_fll; /* [sng] Full dimension name */
    char nm[NC_MAX_NAME+1L];/* [sng] Name of dimension/coordinate */
    int id; /* [id] Dimension ID */
    nco_bool NON_HYP_DMN; /* [flg] Limit is same as dimension in input file */
    nco_bool is_rec_dmn; /* [flg] Dimension is unlimited/record dimension */
    size_t dmn_cnt; /* [nbr] Hyperslabbed size of dimension (= sz iff NON_HYP_DMN) */
    size_t sz; /* [nbr] Size (non-hyperslabbed) of dimension */
  } dmn_cmn_sct;

  typedef enum {
     poly_none=0,
     poly_sph=1,
     poly_crt=2,
     poly_rll=3
  } poly_typ_enm;

  typedef enum {
      poly_vrl_pin,
      poly_vrl_qin,
      poly_vrl_unk
  } poly_vrl_flg_enm;

  typedef struct{
    poly_typ_enm pl_typ;

    nco_bool bwrp;   /* if true then whole polygon is wrapped - at Greenwich or Dateline */
    nco_bool bwrp_y; /* true if polygon is a polar cap - (ie it contains north or south pole)   */
    nco_bool bmsk;   /* polygon not used or has frac==0.0 */

    int crn_nbr;           /* number of vertices */
    int src_id;      /* used in map file as  "row" or "col" id of polygon   */
    int dst_id;      /* used in map file - used only in overlap polygon */
    int  stat;
    int mem_flg;     /* [flg]    */

    double *dp_x;          /* x  vertices */
    double *dp_y;          /* y vertices */
    double dp_x_minmax[2]; /* x/lon minmax */
    double dp_y_minmax[2]; /* y/lat minmax */
    double dp_x_ctr;       /* x/lon center */
    double dp_y_ctr;       /* x/lat center */
    double area;
    double wgt;      /* fraction of dst  area cell in overlap polygon */

    double **shp;    /* array of points size [crn_nbr][NBR] */
    double *dp_xyz;  /* maybe useful for 3D stuff */
  } poly_sct;

  typedef struct{
    int src_id;
    int dst_id;
    double area;
    double wgt;
    double dist;
  } wgt_sct;


#ifdef __cplusplus
} /* end extern "C" */
#endif /* !__cplusplus */

#endif /* NCO_H */
