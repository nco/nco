/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_typ.h,v 1.4 2005-10-24 22:30:26 zender Exp $ */

/* Purpose: Type definitions, opaque types */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_typ.h" *//* Type definitions, opaque types */

#ifndef NCO_TYP_H
#define NCO_TYP_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */

/* 3rd party vendors */

/* Personal headers */

/* Opaque types:
   NCO defines opaque types to abstract treatment of certain netCDF types
   NCO uses types nco_byte, nco_char, and nco_int to handle netCDF internal
   types NC_BYTE, NC_CHAR, and NC_INT, respectively
   Most routines need access to opaque type definitions, so nco.h is appropriate 
   However, in future it may be wiser to put opaque types in, e.g., nco_typ.h 
   so that certain files, e.g., nco_netcdf.c, do not depend on rest of nco.h */

/* byte, char, and int/long types are the most confusing part of netCDF:
   
netCDF manual p. 20:
char: 8-bit characters intended for representing text
byte: 8-bit signed or unsigned integers
"It is possible to interpret byte data as either signed (-128 to 127) or 
unsigned (0 to 255). However, when reading byte data to be converted into other 
numeric types, it is interpreted as signed."

NCO treats NC_BYTE as C-type "signed char".
NCO treats NC_CHAR as C-type "char".
C-Type "char" equals C-type "unsigned char" on most compilers/OSs
NCO uses an opaque type nco_char to ease code portability 

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
   NCO_TYP_IO_FNC_MRG(nc_get_var1,NCO_TYP_BYTE_IO_SFX) */
#define NCO_TYP_IO_FNC_MRG(x,y) x##y

/* C pre-processor can compare integers not strings
   Enumerate integer values corresponding to each type and compare those instead */
#define NCO_TYP_CHAR 0
#define NCO_TYP_SCHAR 1
#define NCO_TYP_UCHAR 2
#define NCO_TYP_INT 3
#define NCO_TYP_LONG 4

/* TODO nco632: new netCDF4 types */
typedef unsigned char nco_ubyte; /* [typ] NC_UBYTE */
typedef unsigned short nco_ushort; /* [typ] NC_USHORT */
typedef unsigned long nco_uint; /* [typ] NC_UINT */
typedef long long nco_int64; /* [typ] NC_INT64 */
typedef unsigned long long nco_uint64; /* [typ] NC_UINT64 */

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
# define NCO_GET_VAR1_BYTE nc_get_var1_text
# define NCO_GET_VARA_BYTE nc_get_vara_text
# define NCO_GET_VARM_BYTE nc_get_varm_text
# define NCO_PUT_ATT_BYTE nc_put_att_text
# define NCO_PUT_VAR1_BYTE nc_put_var1_text
# define NCO_PUT_VARA_BYTE nc_put_vara_text
# define NCO_PUT_VARM_BYTE nc_put_varm_text
#elif NCO_BYTE == NCO_TYP_SCHAR
/* Treat NC_BYTE as C-type signed char */
typedef signed char nco_byte; /* [typ] NC_BYTE */
# define NCO_BYTE_SNG "signed char"
# define NCO_BYTE_IO_SFX schar
# define NCO_GET_ATT_BYTE nc_get_att_schar
# define NCO_GET_VAR1_BYTE nc_get_var1_schar
# define NCO_GET_VARA_BYTE nc_get_vara_schar
# define NCO_GET_VARM_BYTE nc_get_varm_schar
# define NCO_PUT_ATT_BYTE nc_put_att_schar
# define NCO_PUT_VAR1_BYTE nc_put_var1_schar
# define NCO_PUT_VARA_BYTE nc_put_vara_schar
# define NCO_PUT_VARM_BYTE nc_put_varm_schar
#elif NCO_BYTE == NCO_TYP_UCHAR
/* Treat NC_BYTE as C-type unsigned char */
typedef unsigned char nco_byte; /* [typ] NC_BYTE */
# define NCO_BYTE_SNG "unsigned char"
# define NCO_BYTE_IO_SFX uchar
# define NCO_GET_ATT_BYTE nc_get_att_uchar
# define NCO_GET_VAR1_BYTE nc_get_var1_uchar
# define NCO_GET_VARA_BYTE nc_get_vara_uchar
# define NCO_GET_VARM_BYTE nc_get_varm_uchar
# define NCO_PUT_ATT_BYTE nc_put_att_uchar
# define NCO_PUT_VAR1_BYTE nc_put_var1_uchar
# define NCO_PUT_VARA_BYTE nc_put_vara_uchar
# define NCO_PUT_VARM_BYTE nc_put_varm_uchar
#else
#error "ERROR: Unrecognized NCO_BYTE token"
#endif /* NCO_BYTE */

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
# define NCO_GET_VAR1_CHAR nc_get_var1_text
# define NCO_GET_VARA_CHAR nc_get_vara_text
# define NCO_GET_VARM_CHAR nc_get_varm_text
/* nc_put_att_text() is unique---it uses strlen() to determine argument length */
# define NCO_PUT_ATT_CHAR(a,b,c,d,e,f) nc_put_att_text(a,b,c,e,f)
# define NCO_PUT_VAR1_CHAR nc_put_var1_text
# define NCO_PUT_VARA_CHAR nc_put_vara_text
# define NCO_PUT_VARM_CHAR nc_put_varm_text
#elif NCO_CHAR == NCO_TYP_SCHAR
/* Treat NC_CHAR as C-type signed char */
typedef signed char nco_char; /* [typ] NC_CHAR */
# define NCO_CHAR_SNG "signed char"
# define NCO_CHAR_IO_SFX schar
# define NCO_GET_ATT_CHAR nc_get_att_schar
# define NCO_GET_VAR1_CHAR nc_get_var1_schar
# define NCO_GET_VARA_CHAR nc_get_vara_schar
# define NCO_GET_VARM_CHAR nc_get_varm_schar
# define NCO_PUT_ATT_CHAR nc_put_att_schar
# define NCO_PUT_VAR1_CHAR nc_put_var1_schar
# define NCO_PUT_VARA_CHAR nc_put_vara_schar
# define NCO_PUT_VARM_CHAR nc_put_varm_schar
#elif NCO_CHAR == NCO_TYP_UCHAR
/* Treat NC_CHAR as C-type unsigned char */
typedef unsigned char nco_char; /* [typ] NC_CHAR */
# define NCO_CHAR_SNG "unsigned char"
# define NCO_CHAR_IO_SFX uchar
# define NCO_GET_ATT_CHAR nc_get_att_uchar
# define NCO_GET_VAR1_CHAR nc_get_var1_uchar
# define NCO_GET_VARA_CHAR nc_get_vara_uchar
# define NCO_GET_VARM_CHAR nc_get_varm_uchar
# define NCO_PUT_ATT_CHAR nc_put_att_uchar
# define NCO_PUT_VAR1_CHAR nc_put_var1_uchar
# define NCO_PUT_VARA_CHAR nc_put_vara_uchar
# define NCO_PUT_VARM_CHAR nc_put_varm_uchar
#else
#error "ERROR: Unrecognized NCO_CHAR token"
#endif /* NCO_CHAR */

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
# define NCO_INT_IO_SFX int
# define NCO_GET_ATT_INT nc_get_att_int
# define NCO_GET_VAR1_INT nc_get_var1_int
# define NCO_GET_VARA_INT nc_get_vara_int
# define NCO_GET_VARM_INT nc_get_varm_int
# define NCO_PUT_ATT_INT nc_put_att_int
# define NCO_PUT_VAR1_INT nc_put_var1_int
# define NCO_PUT_VARA_INT nc_put_vara_int
# define NCO_PUT_VARM_INT nc_put_varm_int
#elif NCO_INT == NCO_TYP_LONG
/* Treat NC_INT as C-type long */
typedef long nco_int; /* [typ] NC_INT */
# define NCO_INT_SNG "long"
# define NCO_INT_IO_SFX long
# define NCO_GET_ATT_INT nc_get_att_long
# define NCO_GET_VAR1_INT nc_get_var1_long
# define NCO_GET_VARA_INT nc_get_vara_long
# define NCO_GET_VARM_INT nc_get_varm_long
# define NCO_PUT_ATT_INT nc_put_att_long
# define NCO_PUT_VAR1_INT nc_put_var1_long
# define NCO_PUT_VARA_INT nc_put_vara_long
# define NCO_PUT_VARM_INT nc_put_varm_long
#else
#error "ERROR: Unrecognized NCO_INT token"
#endif /* NCO_INT */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* Hi */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_TYP_H */
