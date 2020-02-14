/* $Header$ */

/* Purpose: Type definitions, opaque types */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_typ.h" *//* Type definitions, opaque types */

#ifndef NCO_TYP_H
#define NCO_TYP_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */

/* 3rd party vendors */

/* Personal headers */

/* Opaque types:
   NCO defines opaque types to abstract treatment of certain netCDF types
   NCO uses types nco_byte, nco_char, and nco_int to handle netCDF internal
   types NC_BYTE, NC_CHAR, and NC_INT, respectively
   Most routines need access to opaque type definitions, so nco.h is appropriate 
   However, isolating opaque types definitions in nco_typ.h ensures that 
   wrappers (e.g., nco_netcdf.[ch]) need not depend on nco.h. */

/* byte, char, and int/long types are the most confusing part of netCDF:
   
netCDF manual p. 20:
char: 8-bit characters intended for representing text
byte: 8-bit signed or unsigned integers
"It is possible to interpret byte data as either signed (-128 to 127) or 
unsigned (0 to 255). However, when reading byte data to be converted into other 
numeric types, it is interpreted as signed."

By default (and subject to modification by manipulating the tokens below), 
  NCO treats NC_BYTE as C-type "signed char".
  NCO treats NC_CHAR as C-type "char".
  C-Type "char" equals C-type "unsigned char" on most compilers/OSs
  NCO uses an opaque type nco_char to ease code portability 

Therefore, by default,
  NCO reads/writes NC_BYTE using nc_put/get_var*_schar() functions
  NCO reads/writes NC_CHAR using nc_put/get_var*_text() functions
  NCO does not use nc_put/get_var*_uchar() functions for anything

netCDF manual p. 102:
"The byte type differs from the char type in that it is intended for eight-bit data 
and the zero byte has no special significance, as it may for character data.
The ncgen utility converts byte declarations to char declarations in the output 
C code." */

/* NCO_TYP_IO_FNC_MRG(x,y) function generates appropriate netCDF-layer I/O function 
   call for given I/O operation (x) and (possibly opaque) type (y)
   fxm TODO nco549: Automagically generate function names when called with, e g., 
   NCO_TYP_IO_FNC_MRG(nc_get_var1,NCO_TYP_BYTE_IO_SFX)
   Unfortunately this pre-precessor macro has never worked... */
#define NCO_TYP_IO_FNC_MRG(x,y) x##y

/* Define compatibility tokens when user does not have netCDF4 netcdf.h
   Compatibility tokens for when NCO compiled with older netcdf.h
   It is hard to track where/when many tokens defined
   Easiest to individually check for pre-definition of each */
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
#ifndef NC_VLEN
# define	NC_VLEN 	13	/* vlen */
#endif
#ifndef NC_OPAQUE
# define	NC_OPAQUE 	14	/* opaque */
#endif
#ifndef NC_ENUM
# define	NC_ENUM 	15	/* enum */
#endif
#ifndef NC_COMPOUND
# define	NC_COMPOUND 	16	/* compound */
#endif
#ifndef NC_MAX_ATOMIC_TYPE
#define NC_MAX_ATOMIC_TYPE NC_STRING
#endif
#ifndef NC_FIRSTUSERTYPEID
#define NC_FIRSTUSERTYPEID 32
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
/* NB: These fill values are one shy of min(int64) and max(uint64) */
#ifndef NC_FILL_INT64
# define NC_FILL_INT64   ((long long int)-9223372036854775806LL)
#endif
#ifndef NC_FILL_UINT64
# define NC_FILL_UINT64  ((unsigned long long int)18446744073709551614ULL)
#endif
#ifndef NC_FILL_STRING
/* 20140526: Changed from "" to (char *)"" */
# define NC_FILL_STRING  (char *)""
#endif
/* end define compatibility tokens when user does not have netCDF4 netcdf.h */

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
typedef nc_vlen_t nco_vlen; /* [typ] NC_VLEN */
typedef void nco_opaque; /* [typ] NC_OPAQUE */
typedef void nco_enum; /* [typ] NC_ENUM */
typedef void nco_compound; /* [typ] NC_COMPOUND */

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
# define NCO_BYTE_IO_SFX text
# define NCO_GET_ATT_BYTE nc_get_att_text
# define NCO_GET_VAR_BYTE nc_get_var_text
# define NCO_GET_VAR1_BYTE nc_get_var1_text
# define NCO_GET_VARA_BYTE nc_get_vara_text
# define NCO_GET_VARS_BYTE nc_get_vars_text
# define NCO_GET_VARM_BYTE nc_get_varm_text
# define NCO_PUT_ATT_BYTE nc_put_att_text
# define NCO_PUT_VAR_BYTE nc_put_var_text
# define NCO_PUT_VAR1_BYTE nc_put_var1_text
# define NCO_PUT_VARA_BYTE nc_put_vara_text
# define NCO_PUT_VARS_BYTE nc_put_vars_text
# define NCO_PUT_VARM_BYTE nc_put_varm_text
#elif NCO_BYTE == NCO_TYP_SCHAR
/* Treat NC_BYTE as C-type signed char */
typedef signed char nco_byte; /* [typ] NC_BYTE */
# define NCO_BYTE_SNG "signed char"
# define NCO_BYTE_IO_SFX schar
# define NCO_GET_ATT_BYTE nc_get_att_schar
# define NCO_GET_VAR_BYTE nc_get_var_schar
# define NCO_GET_VAR1_BYTE nc_get_var1_schar
# define NCO_GET_VARA_BYTE nc_get_vara_schar
# define NCO_GET_VARS_BYTE nc_get_vars_schar
# define NCO_GET_VARM_BYTE nc_get_varm_schar
# define NCO_PUT_ATT_BYTE nc_put_att_schar
# define NCO_PUT_VAR_BYTE nc_put_var_schar
# define NCO_PUT_VAR1_BYTE nc_put_var1_schar
# define NCO_PUT_VARA_BYTE nc_put_vara_schar
# define NCO_PUT_VARS_BYTE nc_put_vars_schar
# define NCO_PUT_VARM_BYTE nc_put_varm_schar
#elif NCO_BYTE == NCO_TYP_UCHAR
/* Treat NC_BYTE as C-type unsigned char */
typedef unsigned char nco_byte; /* [typ] NC_BYTE */
# define NCO_BYTE_SNG "unsigned char"
# define NCO_BYTE_IO_SFX uchar
# define NCO_GET_ATT_BYTE nc_get_att_uchar
# define NCO_GET_VAR_BYTE nc_get_var_uchar
# define NCO_GET_VAR1_BYTE nc_get_var1_uchar
# define NCO_GET_VARA_BYTE nc_get_vara_uchar
# define NCO_GET_VARS_BYTE nc_get_vars_uchar
# define NCO_GET_VARM_BYTE nc_get_varm_uchar
# define NCO_PUT_ATT_BYTE nc_put_att_uchar
# define NCO_PUT_VAR_BYTE nc_put_var_uchar
# define NCO_PUT_VAR1_BYTE nc_put_var1_uchar
# define NCO_PUT_VARA_BYTE nc_put_vara_uchar
# define NCO_PUT_VARS_BYTE nc_put_vars_uchar
# define NCO_PUT_VARM_BYTE nc_put_varm_uchar
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
# define NCO_UBYTE_IO_SFX ubyte
# define NCO_GET_ATT_UBYTE nc_get_att_ubyte
# define NCO_GET_VAR_UBYTE nc_get_var_ubyte
# define NCO_GET_VAR1_UBYTE nc_get_var1_ubyte
# define NCO_GET_VARA_UBYTE nc_get_vara_ubyte
# define NCO_GET_VARS_UBYTE nc_get_vars_ubyte
# define NCO_GET_VARM_UBYTE nc_get_varm_ubyte
# define NCO_PUT_ATT_UBYTE nc_put_att_ubyte
# define NCO_PUT_VAR_UBYTE nc_put_var_ubyte
# define NCO_PUT_VAR1_UBYTE nc_put_var1_ubyte
# define NCO_PUT_VARA_UBYTE nc_put_vara_ubyte
# define NCO_PUT_VARS_UBYTE nc_put_vars_ubyte
# define NCO_PUT_VARM_UBYTE nc_put_varm_ubyte
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
# define NCO_CHAR_IO_SFX text
# define NCO_GET_ATT_CHAR nc_get_att_text
# define NCO_GET_VAR_CHAR nc_get_var_text
# define NCO_GET_VAR1_CHAR nc_get_var1_text
# define NCO_GET_VARA_CHAR nc_get_vara_text
# define NCO_GET_VARS_CHAR nc_get_vars_text
# define NCO_GET_VARM_CHAR nc_get_varm_text
/* nc_put_att_text() is unique---it uses strlen() to determine argument length */
# define NCO_PUT_ATT_CHAR(a,b,c,d,e,f) nc_put_att_text(a,b,c,e,f)
# define NCO_PUT_VAR_CHAR nc_put_var_text
# define NCO_PUT_VAR1_CHAR nc_put_var1_text
# define NCO_PUT_VARA_CHAR nc_put_vara_text
# define NCO_PUT_VARS_CHAR nc_put_vars_text
# define NCO_PUT_VARM_CHAR nc_put_varm_text
#elif NCO_CHAR == NCO_TYP_SCHAR
/* Treat NC_CHAR as C-type signed char */
typedef signed char nco_char; /* [typ] NC_CHAR */
# define NCO_CHAR_SNG "signed char"
# define NCO_CHAR_IO_SFX schar
# define NCO_GET_ATT_CHAR nc_get_att_schar
# define NCO_GET_VAR_CHAR nc_get_var_schar
# define NCO_GET_VAR1_CHAR nc_get_var1_schar
# define NCO_GET_VARA_CHAR nc_get_vara_schar
# define NCO_GET_VARS_CHAR nc_get_vars_schar
# define NCO_GET_VARM_CHAR nc_get_varm_schar
# define NCO_PUT_ATT_CHAR nc_put_att_schar
# define NCO_PUT_VAR_CHAR nc_put_var_schar
# define NCO_PUT_VAR1_CHAR nc_put_var1_schar
# define NCO_PUT_VARA_CHAR nc_put_vara_schar
# define NCO_PUT_VARS_CHAR nc_put_vars_schar
# define NCO_PUT_VARM_CHAR nc_put_varm_schar
#elif NCO_CHAR == NCO_TYP_UCHAR
/* Treat NC_CHAR as C-type unsigned char */
typedef unsigned char nco_char; /* [typ] NC_CHAR */
# define NCO_CHAR_SNG "unsigned char"
# define NCO_CHAR_IO_SFX uchar
# define NCO_GET_ATT_CHAR nc_get_att_uchar
# define NCO_GET_VAR_CHAR nc_get_var_uchar
# define NCO_GET_VAR1_CHAR nc_get_var1_uchar
# define NCO_GET_VARA_CHAR nc_get_vara_uchar
# define NCO_GET_VARS_CHAR nc_get_vars_uchar
# define NCO_GET_VARM_CHAR nc_get_varm_uchar
# define NCO_PUT_ATT_CHAR nc_put_att_uchar
# define NCO_PUT_VAR_CHAR nc_put_var_uchar
# define NCO_PUT_VAR1_CHAR nc_put_var1_uchar
# define NCO_PUT_VARA_CHAR nc_put_vara_uchar
# define NCO_PUT_VARS_CHAR nc_put_vars_uchar
# define NCO_PUT_VARM_CHAR nc_put_varm_uchar
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
# define NCO_USHORT_IO_SFX ushort
# define NCO_GET_ATT_USHORT nc_get_att_ushort
# define NCO_GET_VAR_USHORT nc_get_var_ushort
# define NCO_GET_VAR1_USHORT nc_get_var1_ushort
# define NCO_GET_VARA_USHORT nc_get_vara_ushort
# define NCO_GET_VARS_USHORT nc_get_vars_ushort
# define NCO_GET_VARM_USHORT nc_get_varm_ushort
# define NCO_PUT_ATT_USHORT nc_put_att_ushort
# define NCO_PUT_VAR_USHORT nc_put_var_ushort
# define NCO_PUT_VAR1_USHORT nc_put_var1_ushort
# define NCO_PUT_VARA_USHORT nc_put_vara_ushort
# define NCO_PUT_VARS_USHORT nc_put_vars_ushort
# define NCO_PUT_VARM_USHORT nc_put_varm_ushort
#else
# error "ERROR: Unrecognized NCO_USHORT token"
#endif /* NCO_USHORT */

/* NC_INT handling */
#ifndef NCO_INT
/* Valid options are NCO_TYP_INT, NCO_TYP_LONG
   Before 20091030, default was NCO_TYP_LONG, which treats NC_INT as C-type long
   Since  20091030, default is  NCO_TYP_INT,  which treats NC_INT as C-type int */
# define NCO_INT NCO_TYP_INT
#endif /* NCO_INT */
#if NCO_INT == NCO_TYP_INT
/* Treat NC_INT as C-type int */
typedef int nco_int; /* [typ] NC_INT */
# define NCO_INT_SNG "int"
# define NCO_INT_IO_SFX int
# define NCO_GET_ATT_INT nc_get_att_int
# define NCO_GET_VAR_INT nc_get_var_int
# define NCO_GET_VAR1_INT nc_get_var1_int
# define NCO_GET_VARA_INT nc_get_vara_int
# define NCO_GET_VARS_INT nc_get_vars_int
# define NCO_GET_VARM_INT nc_get_varm_int
# define NCO_PUT_ATT_INT nc_put_att_int
# define NCO_PUT_VAR_INT nc_put_var_int
# define NCO_PUT_VAR1_INT nc_put_var1_int
# define NCO_PUT_VARA_INT nc_put_vara_int
# define NCO_PUT_VARS_INT nc_put_vars_int
# define NCO_PUT_VARM_INT nc_put_varm_int
#elif NCO_INT == NCO_TYP_LONG
/* Treat NC_INT as C-type long */
typedef long nco_int; /* [typ] NC_INT */
# define NCO_INT_SNG "long"
# define NCO_INT_IO_SFX long
# define NCO_GET_ATT_INT nc_get_att_long
# define NCO_GET_VAR_INT nc_get_var_long
# define NCO_GET_VAR1_INT nc_get_var1_long
# define NCO_GET_VARA_INT nc_get_vara_long
# define NCO_GET_VARS_INT nc_get_vars_long
# define NCO_GET_VARM_INT nc_get_varm_long
# define NCO_PUT_ATT_INT nc_put_att_long
# define NCO_PUT_VAR_INT nc_put_var_long
# define NCO_PUT_VAR1_INT nc_put_var1_long
# define NCO_PUT_VARA_INT nc_put_vara_long
# define NCO_PUT_VARS_INT nc_put_vars_long
# define NCO_PUT_VARM_INT nc_put_varm_long
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
# define NCO_UINT_IO_SFX uint
# define NCO_GET_ATT_UINT nc_get_att_uint
# define NCO_GET_VAR_UINT nc_get_var_uint
# define NCO_GET_VAR1_UINT nc_get_var1_uint
# define NCO_GET_VARA_UINT nc_get_vara_uint
# define NCO_GET_VARS_UINT nc_get_vars_uint
# define NCO_GET_VARM_UINT nc_get_varm_uint
# define NCO_PUT_ATT_UINT nc_put_att_uint
# define NCO_PUT_VAR_UINT nc_put_var_uint
# define NCO_PUT_VAR1_UINT nc_put_var1_uint
# define NCO_PUT_VARA_UINT nc_put_vara_uint
# define NCO_PUT_VARS_UINT nc_put_vars_uint
# define NCO_PUT_VARM_UINT nc_put_varm_uint
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
# define NCO_INT64_IO_SFX longlong
# define NCO_GET_ATT_INT64 nc_get_att_longlong
# define NCO_GET_VAR_INT64 nc_get_var_longlong
# define NCO_GET_VAR1_INT64 nc_get_var1_longlong
# define NCO_GET_VARA_INT64 nc_get_vara_longlong
# define NCO_GET_VARS_INT64 nc_get_vars_longlong
# define NCO_GET_VARM_INT64 nc_get_varm_longlong
# define NCO_PUT_ATT_INT64 nc_put_att_longlong
# define NCO_PUT_VAR_INT64 nc_put_var_longlong
# define NCO_PUT_VAR1_INT64 nc_put_var1_longlong
# define NCO_PUT_VARA_INT64 nc_put_vara_longlong
# define NCO_PUT_VARS_INT64 nc_put_vars_longlong
# define NCO_PUT_VARM_INT64 nc_put_varm_longlong
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
# define NCO_UINT64_IO_SFX ulonglong
# define NCO_GET_ATT_UINT64 nc_get_att_ulonglong
# define NCO_GET_VAR_UINT64 nc_get_var_ulonglong
# define NCO_GET_VAR1_UINT64 nc_get_var1_ulonglong
# define NCO_GET_VARA_UINT64 nc_get_vara_ulonglong
# define NCO_GET_VARS_UINT64 nc_get_vars_ulonglong
# define NCO_GET_VARM_UINT64 nc_get_varm_ulonglong
# define NCO_PUT_ATT_UINT64 nc_put_att_ulonglong
# define NCO_PUT_VAR_UINT64 nc_put_var_ulonglong
# define NCO_PUT_VAR1_UINT64 nc_put_var1_ulonglong
# define NCO_PUT_VARA_UINT64 nc_put_vara_ulonglong
# define NCO_PUT_VARS_UINT64 nc_put_vars_ulonglong
# define NCO_PUT_VARM_UINT64 nc_put_varm_ulonglong
#else
# error "ERROR: Unrecognized NCO_UINT64 token"
#endif /* NCO_UINT64 */

/* NC_STRING handling */
#ifndef NCO_STRING
/* 20070514: netcdf4-alpha9 only supports nc_put_var_string() and nc_get_var_string() */
/* Only valid option is NCO_TYP_STRING
   Default is NCO_TYP_STRING, which treats NC_STRING as C-type char */
# define NCO_STRING NCO_TYP_STRING
#endif /* NCO_STRING */
#if NCO_STRING == NCO_TYP_STRING
/* Treat NC_STRING as C-type char * */
typedef char * nco_string; /* [typ] NC_STRING */
# define NCO_STRING_SNG "char *"
# define NCO_STRING_IO_SFX string
# define NCO_GET_ATT_STRING nc_get_att_string
# define NCO_GET_VAR_STRING nc_get_var_string
# define NCO_GET_VAR1_STRING nc_get_var1_string
# define NCO_GET_VARA_STRING nc_get_vara_string
# define NCO_GET_VARS_STRING nc_get_vars_string
# define NCO_GET_VARM_STRING nc_get_varm_string
/* nc_put_att_string() is unique---it uses strlen() to determine argument length */
# define NCO_PUT_ATT_STRING(a,b,c,d,e,f) nc_put_att_string(a,b,c,e,f)
# define NCO_PUT_VAR_STRING nc_put_var_string
# define NCO_PUT_VAR1_STRING nc_put_var1_string
# define NCO_PUT_VARA_STRING nc_put_vara_string
# define NCO_PUT_VARS_STRING nc_put_vars_string
# define NCO_PUT_VARM_STRING nc_put_varm_string
#else
# error "ERROR: Unrecognized NCO_STRING token"
#endif /* NCO_STRING */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* Hi */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_TYP_H */
