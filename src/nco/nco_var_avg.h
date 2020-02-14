/* $Header$ */

/* Purpose: Average variables */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_var_avg.h" *//* Average variables */

#ifndef NCO_VAR_AVG_H
#define NCO_VAR_AVG_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 isfinite isinf isnan */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_mmr.h" /* Memory management */
#ifdef _MSC_VER
# include "nco_rth_flt.h" /* Float-precision arithmetic, MSVC macros */
#endif /* !_MSC_VER */
#include "nco_rth_utl.h" /* Arithmetic controls and utilities */
#include "nco_var_rth.h" /* Variable arithmetic */
#include "nco_var_utl.h" /* Variable utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

var_sct * /* O [sct] Partially (non-normalized) reduced variable */
nco_var_avg /* [fnc] Reduce given variable over specified dimensions */
(var_sct *var, /* I/O [sct] Variable to reduce (e.g., average) (destroyed) */
 dmn_sct * const * const dim, /* I [sct] Dimensions over which to reduce variable */
 const int nbr_dim, /* I [sct] Number of dimensions to reduce variable over */
 const int nco_op_typ, /* I [enm] Operation type, default is average */
 const nco_bool flg_rdd, /* I [flg] Retain degenerate dimensions */
 ddra_info_sct * const ddra_info); /* O [sct] DDRA information */

void
nco_var_avg_rdc_ttl /* [fnc] Sum blocks of op1 into each element of op2 */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz_op1, /* I [nbr] Size (in elements) of op1 */
 const long sz_op2, /* I [nbr] Size (in elements) of op2 */
 const int has_mss_val, /* I [flg] Operand has missing values */
 ptr_unn mss_val, /* I [sct] Missing value */
 long * const tally, /* I/O [nbr] Tally buffer */
 ptr_unn op1, /* I [sct] Operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2)) */
 ptr_unn op2); /* O [sct] Sum of each block of op1 */

void
nco_var_avg_rdc_min /* [fnc] Place minimum of op1 blocks into each element of op2 */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz_op1, /* I [nbr] Size (in elements) of op1 */
 const long sz_op2, /* I [nbr] Size (in elements) of op2 */
 const int has_mss_val, /* I [flg] Operand has missing values */
 ptr_unn mss_val, /* I [sct] Missing value */
 ptr_unn op1, /* I [sct] Operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2)) */
 ptr_unn op2); /* O [sct] Minimum of each block of op1 */

void
nco_var_avg_rdc_max /* [fnc] Place maximum of op1 blocks into each element of op2 */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz_op1, /* I [nbr] Size (in elements) of op1 */
 const long sz_op2, /* I [nbr] Size (in elements) of op2 */
 const int has_mss_val, /* I [flg] Operand has missing values */
 ptr_unn mss_val, /* I [sct] Missing value */
 ptr_unn op1, /* I [sct] Operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2)) */
 ptr_unn op2); /* O [sct] Maximum of each block of op1 */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_AVG_H */
