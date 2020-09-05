/* $Header$ */

/* Purpose: Arithmetic controls and utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_rth_utl.h" *//* Arithmetic controls and utilities */

#ifndef NCO_RTH_UTL_H
#define NCO_RTH_UTL_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
#endif /* !_MSC_VER */
#ifdef _OPENMP
# include <omp.h> /* OpenMP pragmas */
#endif /* !_OPENMP */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#ifdef _MSC_VER
# include "nco_rth_flt.h" /* Float-precision arithmetic, MSVC macros */
#endif /* !_MSC_VER */
#include "nco_var_utl.h" /* Variable utilities */
#include "nco_var_rth.h" /* Variable arithmetic */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int /* O [enm] Arithmetic operation */
nco_op_typ_get /* [fnc] Convert user-specified operation into operation key */
(const char * const nco_op_sng); /* I [sng] User-specified operation */

const char * /* O [enm] Arithmetic operation */
nco_op_typ_cf_sng /* [fnc] Convert arithmetic operation type enum to string */
(const int nco_op_typ); /* I [enm] Arithmetic operation type */

void 
nco_opr_nrm /* [fnc] Normalization of arithmetic operations for ncra/nces */
(const int nco_op_typ, /* I [enm] Operation type */
 const int nbr_var_prc, /* I [nbr] Number of processed variables */
 X_CST_PTR_CST_PTR_Y(var_sct,var_prc), /* I [sct] Variables in input file */
 X_CST_PTR_CST_PTR_Y(var_sct,var_prc_out), /* I/O [sct] Variables in output file */
 const char * const rec_nm_fll, /* I [sng] Full name of record dimension */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

void 
nco_opr_drv /* [fnc] Intermediate control of arithmetic operations for ncra/nces */
(const long idx_rec, /* I [idx] Index of record (ncra), file (ncfe), or group (ncge) in current operation */
 const int nco_op_typ, /* I [enm] Operation type */
 const var_sct * const var_prc, /* I [sct] Variable in input file */
 var_sct * const var_prc_out); /* I/O [sct] Variable in output file */

int /* O [enm] Relational operation */
nco_op_prs_rlt /* [fnc] Convert Fortran abbreviation for relational operator into NCO operation key */
(const char * const op_sng); /* I [sng] Fortran representation of relational operator */

nco_rth_prc_rnk_enm /* [enm] Ranked precision of arithmetic type */
nco_rth_prc_rnk /* [fnc] Rank precision of arithmetic type */
(const nc_type type); /* I [enm] netCDF type of operand */

void
vec_set /* [fnc] Fill every value of first operand with value of second operand */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] size (in elements) of operand */
 ptr_unn op1, /* I [sct] Values of first operand */
 const double op2); /* I [frc] Value to fill vector with */

void
nco_zero_double /* [fnc] Zero all values of double array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 double * restrict const op1); /* I/O [nbr] Array to be zeroed */

void
nco_zero_long /* [fnc] Zero all values of long array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 long * restrict const op1); /* I/O [nbr] Array to be zeroed */

void
nco_set_long /* [fnc] Set all values of long array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 const long val, /* I [] Number to set array to */
 long * restrict const op1); /* I/O [nbr] Array to be set */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_RTH_UTL_H */
