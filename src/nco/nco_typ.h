/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_typ.h,v 1.2 2005-06-30 21:27:59 zender Exp $ */

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

/* NC_BYTE handling */
#ifndef NCO_TYP_BYTE
#define NCO_TYP_BYTE_SCHAR
#endif /* NCO_TYP_BYTE */
#ifdef NCO_TYP_BYTE_CHAR
/* Treat NC_BYTE as C-type char */
typedef char nco_byte; /* [typ] NC_BYTE */
#define NCO_TYP_BYTE_SNG "char"
#endif /* !NCO_TYP_BYTE_CHAR */
#ifdef NCO_TYP_BYTE_SCHAR
/* Treat NC_BYTE as C-type signed char */
typedef signed char nco_byte; /* [typ] NC_BYTE */
#define NCO_TYP_BYTE_SNG "signed char"
#endif /* !NCO_TYP_BYTE_SCHAR */
#ifdef NCO_TYP_BYTE_UCHAR
/* Treat NC_BYTE as C-type unsigned char */
typedef unsigned char nco_byte; /* [typ] NC_BYTE */
#define NCO_TYP_BYTE_SNG "unsigned char"
#endif /* !NCO_TYP_BYTE_UCHAR */

/* NC_CHAR handling */
#ifndef NCO_TYP_CHAR
#define NCO_TYP_CHAR_CHAR
#endif /* NCO_TYP_CHAR */
#ifdef NCO_TYP_CHAR_CHAR
/* Treat NC_CHAR as C-type char */
typedef char nco_char; /* [typ] NC_CHAR */
#define NCO_TYP_CHAR_SNG "char"
#endif /* !NCO_TYP_CHAR_CHAR */
#ifdef NCO_TYP_CHAR_SCHAR
/* Treat NC_CHAR as C-type signed char */
typedef signed char nco_char; /* [typ] NC_CHAR */
#define NCO_TYP_CHAR_SNG "signed char"
#endif /* !NCO_TYP_CHAR_SCHAR */
#ifdef NCO_TYP_CHAR_UCHAR
/* Treat NC_CHAR as C-type unsigned char */
typedef unsigned char nco_char; /* [typ] NC_CHAR */
#define NCO_TYP_CHAR_SNG "unsigned char"
#endif /* !NCO_TYP_CHAR_UCHAR */

/* NC_INT handling */
#ifndef NCO_TYP_INT
#define NCO_TYP_INT_LONG
#endif /* NCO_TYP_INT */
#ifdef NCO_TYP_INT_LONG
/* Treat NC_INT as C-type long */
typedef long nco_int; /* [typ] NC_INT */
#define NCO_TYP_INT_SNG "long"
#endif /* !NCO_TYP_INT_LONG */
#ifdef NCO_TYP_INT_INT
/* Treat NC_INT as C-type int */
typedef int nco_int; /* [typ] NC_INT */
#define NCO_TYP_INT_SNG "int"
#endif /* !NCO_TYP_INT_INT */
  
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* Hi */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_TYP_H */
