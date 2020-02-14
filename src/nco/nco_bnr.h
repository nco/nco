/* $Header$ */

/* Purpose: Binary file utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_bnr.h" *//* Binary file utilities */

#ifndef NCO_BNR_H
#define NCO_BNR_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#ifdef _MSC_VER
# include <stdlib.h> /* _byteswap_ulong() */
#else /* !_MSC_VER */
# include <inttypes.h> /* __builtin_bswap32() */
#endif /* !_MSC_VER */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

FILE * /* O [fl] Unformatted binary file handle */
nco_bnr_open /* [fnc] Open unformatted binary data file */
(const char * const fl_bnr, /* [sng] Unformatted binary file */
 const char * const fl_mode); /* [sng] Open-mode ("r", "w", ...) */

int /* [rcd] Return code */
nco_bnr_close /* [fnc] Close unformatted binary data file for writing */
(FILE *fp_bnr, /* I [fl] Unformatted binary output file handle */
 const char * const fl_bnr); /* [sng] Unformatted binary output file */

size_t /* O [nbr] Number of elements successfully written */
nco_bnr_wrt /* [fnc] Write unformatted binary data */
(FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const char * const var_nm, /* I [sng] Variable name */
 const long var_sz, /* I [nbr] Variable size */
 const nc_type var_typ, /* I [enm] Variable type */
 const void * const vp); /* I [ptr] Data to write */

size_t /* O [nbr] Number of elements successfully read */
nco_bnr_rd /* [fnc] Read unformatted binary data */
(FILE * const fp_bnr, /* I [fl] Unformatted binary input file handle */
 const char * const var_nm, /* I [sng] Variable name */
 const long var_sz, /* I [nbr] Variable size */
 const nc_type var_typ, /* I [enm] Variable type */
 void * const void_ptr); /* O [ptr] Data to read */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_BNR_H */
