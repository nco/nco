/* $Header$ */

/* Purpose: Description (definition) of filter functions */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_cnk.h" *//* Compression filters */

#ifndef NCO_FLT_H
#define NCO_FLT_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#if NC_LIB_VERSION >= 474
# include <netcdf_filter.h> /* netCDF filter definitions */
#endif /* !4.7.4 */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char * /* O [sng] Parsed Filter string */
nco_flt_prs /* [fnc] Parse user-provided filter string */
(const char * flt_sng); /* I [sng] Filter string */
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_FLT_H */
