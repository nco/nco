/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mmr.h,v 1.3 2002-05-07 08:00:08 zender Exp $ */

/* Purpose: Memory management */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_mmr.h" *//* Memory management */

#ifndef NCO_MMR_H
#define NCO_MMR_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
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
(const size_t size); /* I [nbr] Number of bytes to allocate */

void * /* O [ptr] Pointer to re-allocated memory */
nco_realloc /* [fnc] Wrapper for realloc() */
(const void *ptr, /* I/O [ptr] Buffer to reallocate */
 const size_t size); /* I [nbr] Number of bytes required */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MMR_H */
