/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mmr.h,v 1.15 2005-03-23 03:02:00 zender Exp $ */

/* Purpose: Memory management */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_mmr.h" *//* Memory management */

#ifndef NCO_MMR_H
#define NCO_MMR_H

/* Standard header files */
#include <errno.h> /* system/library error diagnostics, errno */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */
#include <sys/resource.h> /* Resource usage and limits */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum nco_mmr_typ{ /* [enm] Memory allocation type */
  nco_mmr_calloc, /* [enm] nco_calloc() */
  nco_mmr_free, /* [enm] nco_free() */
  nco_mmr_malloc, /* [enm] nco_malloc() */
  nco_mmr_realloc /* [enm] nco_realloc() */
}; /* end nco_mmr_typ enum */

void * /* O [ptr] Pointer to calloc'd memory */
nco_calloc /* [fnc] Wrapper for calloc() */
(const size_t lmn_nbr, /* I [nbr] Number of elements to allocate */
 const size_t lmn_sz); /* I [nbr] Size of each element */

void * /* O [ptr] Buffer after free'ing */
nco_free /* [fnc] Wrapper for free() */
(void *vp); /* I/O [ptr] Buffer to free() */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc /* [fnc] Wrapper for malloc() */
(const size_t size); /* I [B] Bytes to allocate */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc_flg /* [fnc] Wrapper for malloc(), forgives ENOMEM errors */
(const size_t size); /* I [B] Bytes to allocate */

void * /* O [ptr] Pointer to allocated memory */
nco_malloc_dbg /* [fnc] Wrapper for malloc(), receives and prints more diagnostics */
(const size_t sz, /* I [B] Bytes to allocate */
 const char *fnc_nm, /* I [sng] Function name */
 const char *msg); /* I [sng] Supplemental error message */

void
nco_malloc_err_hnt_prn /* [fnc] Explain meaning and workarounds for malloc() failures */
(void);

long /* O [B] Maximum resident set size */
nco_mmr_rusage_prn /* [fnc] Print rusage memory usage statistics */
(const int rusage_who); /* [enm] RUSAGE_SELF, RUSAGE_CHILDREN, RUSAGE_LWP */

long /* O [nbr] Net memory currently allocated */
nco_mmr_stt /* [fnc] Track memory statistics */
(const int nco_mmr_typ, /* I [enm] Memory allocation type */
 const size_t sz); /* I [B] Bytes allocated, deallocated, or reallocated */

const char * /* O [sng] String describing type */
nco_mmr_typ_sng /* [fnc] Convert netCDF type enum to string */
(nc_type type); /* I [enm] netCDF type */

void * /* O [ptr] Pointer to re-allocated memory */
nco_realloc /* [fnc] Wrapper for realloc() */
(void *ptr, /* I/O [ptr] Buffer to reallocate */
 const size_t size); /* I [B] Bytes required */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MMR_H */
