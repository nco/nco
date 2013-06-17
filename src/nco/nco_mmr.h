/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mmr.h,v 1.33 2013-06-17 23:48:27 zender Exp $ */

/* Purpose: Memory management */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_mmr.h" *//* Memory management */

#ifndef NCO_MMR_H
#define NCO_MMR_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <errno.h> /* system/library error diagnostics, errno */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */
#ifdef MACOSX
# include <sys/time.h> /* machine time (needed by Mac OS X for struct rusage) */ 
#endif /* !MACOSX */
#ifdef HAVE_GETRUSAGE
# include <sys/resource.h> /* Resource usage and limits */
#endif /* !HAVE_GETRUSAGE */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
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
(const nco_mmr_typ_enm nco_mmr_typ, /* I [enm] Memory allocation type */
 const size_t sz); /* I [B] Bytes allocated, deallocated, or reallocated */

const char * /* O [sng] String describing memory type */
nco_mmr_typ_sng /* [fnc] Convert NCO memory management type enum to string */
(const nco_mmr_typ_enm nco_mmr_typ); /* I [enm] NCO memory management type */

void * /* O [ptr] Pointer to re-allocated memory */
nco_realloc /* [fnc] Wrapper for realloc() */
(void *ptr, /* I/O [ptr] Buffer to reallocate */
 const size_t size); /* I [B] Bytes required */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MMR_H */
