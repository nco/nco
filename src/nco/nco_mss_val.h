/* $Header$ */

/* Purpose: Missing value utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_mss_val.h" *//* Missing value utilities */

#ifndef NCO_MSS_VAL_H
#define NCO_MSS_VAL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */
#ifdef HAVE_STRINGS_H
# include <strings.h> /* strcasecmp() */
#endif /* !HAVE_STRINGS_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_mmr.h" /* Memory management */
#include "nco_prn.h" /* Print variables, attributes, metadata */
#if (defined NEED_STRCASECMP) || (defined NEED_STRNCASECMP) || (defined NEED_STRDUP)
# include "nco_sng_utl.h" /* String utilities */
#endif /* NEED_STRCASECMP || NEED_STRNCASECMP || NEED_STRDUP */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

ptr_unn /* O [sct] Default missing value for type type */
nco_mss_val_mk /* [fnc] Return default missing value for type type */
(const nc_type type); /* I [enm] netCDF type of operand */

nco_bool /* O [flg] One or both operands have missing value */
nco_mss_val_cnf /* [fnc] Change missing_value of var2 to missing_value of var1 */
(var_sct * const var1, /* I [sct] Variable with template missing value to copy */
 var_sct * const var2); /* I/O [sct] Variable with missing value to fill-in/overwrite */

void
nco_mss_val_cp /* [fnc] Copy missing value from var1 to var2 */
(const var_sct * const var1, /* I [sct] Variable with template missing value to copy */
 var_sct * const var2); /* I/O [sct] Variable with missing value to fill-in/overwrite */

int /* O [flg] Variable has missing value on output */
nco_mss_val_get /* [fnc] Update number of attributes, missing value of variable */
(const int nc_id, /* I [id] netCDF input-file ID */
 var_sct * const var); /* I/O [sct] Variable with missing_value to update */

nco_bool /* O [flg] Variable has missing value */
nco_mss_val_get_dbl /* [fnc] Return missing value of variable, if any, as double precision number */
(const int nc_id, /* I [id] netCDF input-file ID */
 const int var_id, /* I [id] netCDF variable ID */
 double *mss_val_dbl); /* O [frc] Missing value in double precision */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MSS_VAL_H */
