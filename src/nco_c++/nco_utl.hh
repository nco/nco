// $Header$ 

// Purpose: Description (definition) of C++ interface utilities for netCDF routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

// Usage:
// #include <nco_utl.hh> // C++ interface utilities for netCDF routines

#ifndef NCO_UTL_HH // Contents have not yet been inserted in current source file
#define NCO_UTL_HH

#ifdef HAVE_CONFIG_H
# include <config.h> // Autotools tokens
#endif // !HAVE_CONFIG_H

// C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <string> // Standard C++ string class

// Standard C headers
#ifdef HAVE_CSTDLIB
#include <cstdlib> // abort, exit, getopt, malloc, strtod, strtol
#else // The only C++ compiler known to lack <cstdlib> is SGI's CC
#include <stdlib.h> // abort, exit, getopt, malloc, strtod, strtol
#endif // !HAVE_CSTDLIB

// 3rd party vendors
#include <netcdf.h> // netCDF C interface

// Library-wide headers
/* All headers in libnco_c++ include nco_utl.hh
   Hence nco_utl.hh is the appropriate place to define library-wide variables
   fxm: Make this extern and instantiate in .cc file? */
const int NCO_NOERR=NC_NOERR; // [enm] Variable'ize CPP macro for use in function parameter initialization

#ifndef NCO_FORMAT_UNDEFINED
  /* netcdf.h defines four NC_FORMAT tokens: NC_FORMAT_CLASSIC, ...
     The values are (currently) enumerated from one to four
     Operators need to check if fl_out_fmt has been user-specified
     Saftest way is to compare current value of fl_out_fmt to initial value 
     Initial value should be a number that will never be a true netCDF format */
# define NCO_FORMAT_UNDEFINED 0
#endif // NCO_FORMAT_UNDEFINED

/* Define compatibility tokens when user does not have netCDF4 */
/* Single compatibility token new to netCDF4 netcdf.h */
#ifndef NC_NETCDF4
# define NC_NETCDF4     (0x1000) /* Use netCDF-4/HDF5 format */
#endif
/* Six compatibility tokens not all available until netCDF 3.6.1 netcdf.h
   NC_64BIT_OFFSET is used (so far) only in nco_fl_utl.c */
#ifndef NC_64BIT_OFFSET
# define NC_64BIT_OFFSET (0x0200) /* Use large (64-bit) file offsets */
#endif
#ifndef NC_CLASSIC_MODEL
# define NC_CLASSIC_MODEL (0x0008) /* Enforce strict netcdf-3 rules. */
#endif
#ifndef NC_FORMAT_CLASSIC
# define NC_FORMAT_CLASSIC (1)
#endif
#ifndef NC_FORMAT_64BIT
# define NC_FORMAT_64BIT   (2)
#endif
#ifndef NC_FORMAT_NETCDF4
# define NC_FORMAT_NETCDF4 (3)
#endif
#ifndef NC_FORMAT_NETCDF4_CLASSIC
# define NC_FORMAT_NETCDF4_CLASSIC  (4) /* create netcdf-4 files, with NC_CLASSIC_MODEL. */
#endif

/* Datatypes referenced in nco_typ.h, nco_netcdf.c: */
#ifndef NC_UBYTE
# define	NC_UBYTE 	7	/* unsigned 1 byte int */
#endif
#ifndef NC_USHORT
# define	NC_USHORT 	8	/* unsigned 2-byte int */
#endif
#ifndef NC_UINT
# define	NC_UINT 	9	/* unsigned 4-byte int */
#endif
#ifndef NC_INT64
# define	NC_INT64 	10	/* signed 8-byte int */
#endif
#ifndef NC_UINT64
# define	NC_UINT64 	11	/* unsigned 8-byte int */
#endif
#ifndef NC_STRING
# define	NC_STRING 	12	/* string */
#endif
/* Fill values for netCDF4 datatypes. Referenced in nco_mss_val.c: */
#ifndef NC_FILL_UBYTE
# define NC_FILL_UBYTE   (255)
#endif
#ifndef NC_FILL_USHORT
# define NC_FILL_USHORT  (65535)
#endif
#ifndef NC_FILL_UINT
# define NC_FILL_UINT    (4294967295U)
#endif
#ifndef NC_FILL_INT64
# define NC_FILL_INT64   ((long long int)-9223372036854775808)
#endif
#ifndef NC_FILL_UINT64
# define NC_FILL_UINT64  ((unsigned long long int)18446744073709551615)
#endif
#ifndef NC_FILL_STRING
# define NC_FILL_STRING  ""
#endif

/* C pre-processor compares integers not strings
   Perform comparisons on enumerated integer values corresponding to each type */
#define NCO_TYP_CHAR 0
#define NCO_TYP_SCHAR 1
#define NCO_TYP_UCHAR 2
#define NCO_TYP_INT 3
#define NCO_TYP_LONG 4
#define NCO_TYP_UBYTE 5
#define NCO_TYP_USHORT 6
#define NCO_TYP_UINT 7
#define NCO_TYP_INT64 8
#define NCO_TYP_UINT64 9
#define NCO_TYP_STRING 10

/* Tokens to give semantic compatibility between easy- and hard-to-handle types
   i.e., nco_short is always short and easy-to-handle */
typedef short nco_short; /* [typ] NC_SHORT */

/* NC_BYTE handling */
#ifndef NCO_BYTE
/* Valid options are NCO_TYP_CHAR, NCO_TYP_SCHAR, NCO_TYP_UCHAR 
   Default is NCO_TYP_SCHAR, which treats NC_BYTE as C-type signed char */
# define NCO_BYTE NCO_TYP_SCHAR
#endif /* NCO_BYTE */
#if NCO_BYTE == NCO_TYP_CHAR
/* Treat NC_BYTE as C-type char */
typedef char nco_byte; /* [typ] NC_BYTE */
# define NCO_BYTE_SNG "char"
#elif NCO_BYTE == NCO_TYP_SCHAR
/* Treat NC_BYTE as C-type signed char */
typedef signed char nco_byte; /* [typ] NC_BYTE */
# define NCO_BYTE_SNG "signed char"
#elif NCO_BYTE == NCO_TYP_UCHAR
/* Treat NC_BYTE as C-type unsigned char */
typedef unsigned char nco_byte; /* [typ] NC_BYTE */
# define NCO_BYTE_SNG "unsigned char"
#else
# error "ERROR: Unrecognized NCO_BYTE token"
#endif /* NCO_BYTE */

/* NC_UBYTE handling */
#ifndef NCO_UBYTE
/* Only valid option is NCO_TYP_UCHAR 
   Default is NCO_TYP_UCHAR, which treats NC_UBYTE as C-type unsigned char */
# define NCO_UBYTE NCO_TYP_UCHAR
#endif /* NCO_UBYTE */
#if NCO_UBYTE == NCO_TYP_UCHAR
/* Treat NC_UBYTE as C-type unsigned char */
typedef unsigned char nco_ubyte; /* [typ] NC_UBYTE */
# define NCO_UBYTE_SNG "unsigned char"
#else
# error "ERROR: Unrecognized NCO_UBYTE token"
#endif /* NCO_UBYTE */

/* NC_CHAR handling */
#ifndef NCO_CHAR
/* Valid options are NCO_TYP_CHAR, NCO_TYP_SCHAR, NCO_TYP_UCHAR 
   Default is NCO_TYP_CHAR, which treats NC_CHAR as C-type char */
# define NCO_CHAR NCO_TYP_CHAR
#endif /* NCO_CHAR */
#if NCO_CHAR == NCO_TYP_CHAR
/* Treat NC_CHAR as C-type char */
typedef char nco_char; /* [typ] NC_CHAR */
# define NCO_CHAR_SNG "char"
#elif NCO_CHAR == NCO_TYP_SCHAR
/* Treat NC_CHAR as C-type signed char */
typedef signed char nco_char; /* [typ] NC_CHAR */
# define NCO_CHAR_SNG "signed char"
#elif NCO_CHAR == NCO_TYP_UCHAR
/* Treat NC_CHAR as C-type unsigned char */
typedef unsigned char nco_char; /* [typ] NC_CHAR */
# define NCO_CHAR_SNG "unsigned char"
#else
# error "ERROR: Unrecognized NCO_CHAR token"
#endif /* NCO_CHAR */

/* NC_USHORT handling */
#ifndef NCO_USHORT
/* Only valid option is NCO_TYP_USHORT 
   Default is NCO_TYP_USHORT, which treats NC_USHORT as C-type unsigned short */
# define NCO_USHORT NCO_TYP_USHORT
#endif /* NCO_USHORT */
#if NCO_USHORT == NCO_TYP_USHORT
/* Treat NC_USHORT as C-type unsigned short */
typedef unsigned short nco_ushort; /* [typ] NC_USHORT */
# define NCO_USHORT_SNG "unsigned short"
#else
# error "ERROR: Unrecognized NCO_USHORT token"
#endif /* NCO_USHORT */

/* NC_INT handling */
#ifndef NCO_INT
/* Valid options are NCO_TYP_INT, NCO_TYP_LONG
   Default is NCO_TYP_LONG, which treats NC_INT as C-type long */
# define NCO_INT NCO_TYP_LONG
#endif /* NCO_INT */
#if NCO_INT == NCO_TYP_INT
/* Treat NC_INT as C-type int */
typedef int nco_int; /* [typ] NC_INT */
# define NCO_INT_SNG "int"
#elif NCO_INT == NCO_TYP_LONG
/* Treat NC_INT as C-type long */
typedef long nco_int; /* [typ] NC_INT */
# define NCO_INT_SNG "long"
#else
# error "ERROR: Unrecognized NCO_INT token"
#endif /* NCO_INT */

/* NC_UINT handling */
#ifndef NCO_UINT
/* Only valid option is NCO_TYP_UINT
   Default is NCO_TYP_UINT, which treats NC_UINT as C-type unsigned int */
# define NCO_UINT NCO_TYP_UINT
#endif /* NCO_UINT */
#if NCO_UINT == NCO_TYP_UINT
/* Treat NC_UINT as C-type unsigned int */
typedef unsigned int nco_uint; /* [typ] NC_UINT */
# define NCO_UINT_SNG "unsigned int"
#else
# error "ERROR: Unrecognized NCO_UINT token"
#endif /* NCO_UINT */

/* NC_INT64 handling */
#ifndef NCO_INT64
/* Only valid option is NCO_TYP_INT64
   Default is NCO_TYP_INT64, which treats NC_INT64 as C-type long long */
# define NCO_INT64 NCO_TYP_INT64
#endif /* NCO_INT64 */
#if NCO_INT64 == NCO_TYP_INT64
/* Treat NC_INT64 as C-type long long */
typedef long long nco_int64; /* [typ] NC_INT64 */
# define NCO_INT64_SNG "long long"
#else
# error "ERROR: Unrecognized NCO_INT64 token"
#endif /* NCO_INT64 */

/* NC_UINT64 handling */
#ifndef NCO_UINT64
/* Only valid option is NCO_TYP_UINT64
   Default is NCO_TYP_UINT64, which treats NC_UINT64 as C-type unsigned long long */
# define NCO_UINT64 NCO_TYP_UINT64
#endif /* NCO_UINT64 */
#if NCO_UINT64 == NCO_TYP_UINT64
/* Treat NC_UINT64 as C-type unsigned long long */
typedef unsigned long long nco_uint64; /* [typ] NC_UINT64 */
# define NCO_UINT64_SNG "unsigned long long"
#else
# error "ERROR: Unrecognized NCO_UINT64 token"
#endif /* NCO_UINT64 */

/* NC_STRING handling */
#ifndef NCO_STRING
/* 20070514: netcdf4-beta1 only supports nc_put_var_string() and nc_get_var_string() */
/* Only valid option is NCO_TYP_STRING
   Default is NCO_TYP_STRING, which treats NC_STRING as C-type char */
# define NCO_STRING NCO_TYP_STRING
#endif /* NCO_STRING */
#if NCO_STRING == NCO_TYP_STRING
/* Treat NC_STRING as C-type char * */
typedef char * nco_string; /* [typ] NC_STRING */
# define NCO_STRING_SNG "char *"
#else
# error "ERROR: Unrecognized NCO_STRING token"
#endif /* NCO_STRING */

// Same debugging types as in nco.h
enum nco_dbg_typ_enm{ /* [enm] Debugging levels */
  /* List in increasing levels of verbosity */
  nco_dbg_quiet, /* Quiet all non-error messages. */
  nco_dbg_std, /* Standard mode. Minimal, but some, messages. */
  nco_dbg_fl, /* Filenames */
  nco_dbg_scl, /* Scalars, other per-file information  */
  nco_dbg_var, /* Variables, highest level per-file loop information */
  nco_dbg_crr, /* Current task */
  nco_dbg_sbr, /* Subroutine names on entry and exit */
  nco_dbg_io, /* Subroutine I/O */
  nco_dbg_vec, /* Entire vectors */
  nco_dbg_vrb, /* Verbose, print everything possible */
  nco_dbg_old, /* Old debugging blocks not used anymore */
  nco_dbg_nbr /* Number of debugging types. Should be last enumerated value. */
  }; /* end nco_dbg_typ_enm */

// Personal headers
#include <nco_att.hh> // C++ interface to netCDF attribute routines
#include <nco_dmn.hh> // C++ interface to netCDF dimension routines
#include <nco_var.hh> // C++ interface to netCDF variable routines

// Typedefs

// Define nco_cls class

// Prototype global functions with C++ linkages

int // [rcd] Return code
nco_create_mode_prs // [fnc] Parse user-specified file format
(const std::string fl_fmt_sng, // I [sng] User-specified file format string
 int &fl_fmt_enm); // O [enm] Output file format
// end nco_create_mode_prs() prototype

void 
nco_err_exit // [fnc] Lookup, print netCDF error message, exit
(const int &rcd, // I [enm] netCDF error code
 const std::string &msg, // I [sng] supplemental error message
 const std::string &msg_opt=""); // I [sng] Optional supplemental error message
// end nco_err_exit() prototype

void 
nco_err_exit // [fnc] Print error message, exit
(const std::string &sbr_nm, // I [sng] Subroutine name
 const std::string &msg); // I [sng] Error message
// end nco_err_exit() prototype

void 
nco_wrn_prn // [fnc] Print NCO warning message and return
(const std::string &msg, // I [sng] Supplemental warning message
 const std::string &msg_opt=""); // I [sng] Optional supplemental warning message

// Begin nco_inq_varsz() overloads

int // O [enm] Return success code
nco_inq_varsz // [fnc] Compute size of variable
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 size_t &var_sz); // O [nbr] Variable size
// end nco_inq_varsz() prototype

size_t // O [nbr] Variable size
nco_inq_varsz // [fnc] Compute size of variable
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id); // I [id] Variable ID
// end nco_inq_varsz() prototype

// End nco_inq_varsz() overloads

int // O [enm] Return success code
nco_inq_varsrt // [fnc] Inquire variable dimension IDS
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 std::valarray<size_t> &srt, // O [idx] Starting indices
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_varsrt() prototype

int // [nbr] Internal (native) size (Bytes) of netCDF external type
nco_typ_lng // [fnc] Internal (native) size (Bytes) of netCDF external type
(const nc_type &nco_typ); // I [enm] netCDF external type
// end nco_typ_lng() prototype

std::string // O [sng] String version of netCDF external type enum
nco_typ_sng // [fnc] String version of netCDF external type enum
(const nc_type &nco_typ); // I [enm] netCDF external type
// end nco_typ_sng() prototype

std::string // O [sng] String version of C++ internal type for storing netCDF external type enum
nco_c_typ_sng // [fnc] String version of C++ internal type for storing netCDF external type enum
(const nc_type &nco_typ); // I [enm] netCDF external type
// end nco_c_typ_sng() prototype

std::string // O [sng] String version of C++ internal type for storing netCDF external type enum
nco_ftn_typ_sng // [fnc] String version of C++ internal type for storing netCDF external type enum
(const nc_type &nco_typ); // I [enm] netCDF external type
// end nco_ftn_typ_sng() prototype

void 
nco_dfl_case_nctype_err(void); // [fnc] Handle illegal nc_type references
// end nco_dfl_case_nctype_err() prototype

// Begin nco_get_xtype() overloads

nc_type // O [enm] External netCDF type
nco_get_xtype // [fnc] Determine external netCDF type
(const float &var_val); // I [frc] Variable value
// end nco_get_xtype<float>() prototype

nc_type // O [enm] External netCDF type
nco_get_xtype // [fnc] Determine external netCDF type
(const double &var_val); // I [frc] Variable value
// end nco_get_xtype<double>() prototype

// End nco_get_xtype() overloads

// Define inline'd functions in header so source is visible to calling files

#endif // NCO_UTL_HH  






