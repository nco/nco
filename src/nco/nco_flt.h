/* $Header$ */

/* Purpose: Description (definition) of compression filter functions */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_flt.h" *//* Compression filters */

#ifndef NCO_FLT_H
#define NCO_FLT_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#ifdef NC_HAVE_META_H
# include <netcdf_meta.h> /* NC_VERSION_..., HAVE_NC_RENAME_GRP */	 
#endif /* !NC_HAVE_META_H */
#ifndef NC_LIB_VERSION
# define NC_LIB_VERSION ( NC_VERSION_MAJOR * 100 + NC_VERSION_MINOR * 10 + NC_VERSION_PATCH )
#endif /* !NC_LIB_VERSION */
#if NC_LIB_VERSION >= 474
# include <netcdf_filter.h> /* netCDF filter definitions */
#endif /* !4.7.4 */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */

/* Filters types that NCO knows internally: 
   Convert string to filter enum in nco_flt_typ_set() */
typedef enum nco_flt_typ_enm{ /* [enm] Chunking policy */
  nco_flt_nil=0, /* 0 [enm] Filter type is unset */
  nco_flt_dfl=1, /* 1 [enm] DEFLATE */
  nco_flt_bzp=2, /* 2 [enm] Bzip2 */
  nco_flt_lz4=3, /* 3 [enm] LZ4 */
  nco_flt_bgr=4, /* 4 [enm] Bit Grooming */
  nco_flt_dgr=5, /* 5 [enm] Digit Rounding */
  nco_flt_btr=6, /* 6 [enm] Bit Rounding */
  nco_flt_zst=7, /* 7 [enm] Zstandard */
} nco_flt_typ_enm; /* end nco_flt_typ_enm */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
nco_flt_prs /* [fnc] Parse user-provided filter string */
(char * const flt_sng); /* I [sng] User-provided filter string */
 // const unsigned int *flt_id, /* O [enm] Compression filter ID */
 // const nco_flt_typ_enm *nco_flt_typ); /* O [enm] Compression filter type */
  
const char * /* O [sng] Filter string */
nco_flt_enm2sng /* [fnc] Convert compression filter enum to string */
(const nco_flt_typ_enm nco_flt_typ); /* I [enm] Compression filter type */

const int /* O [enm] Filter enum */
nco_flt_sng2enm /* [fnc] Convert user-specified filter string to NCO enum */
(const char *nco_flt_sng); /* [sng] User-specified filter string */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_FLT_H */
