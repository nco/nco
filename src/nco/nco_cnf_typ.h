/* $Header$ */

/* Purpose: Conform variable types */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_cnf_typ.h" *//* Conform variable types */

#ifndef NCO_CNF_TYP_H
#define NCO_CNF_TYP_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */
#ifdef _MSC_VER
# include "nco_rth_flt.h" /* Float-precision arithmetic, MSVC macros */
#endif 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
cast_void_nctype /* [fnc] Cast generic pointer to netCDF type */
(const nc_type type, /* I [enm] netCDF type to cast void pointer to */
 ptr_unn * const ptr); /* I/O [ptr] Pointer to pointer union whose vp element will be cast to type type*/

void
cast_nctype_void /* [fnc] Cast generic pointer in ptr_unn structure from type type to type void */
(const nc_type type, /* I [enm] netCDF type of pointer */
 ptr_unn * const ptr); /* I/O pointer to pointer union which to cast from type type to type void */

var_sct * /* O [var] Variable after (possible) conversion */
nco_typ_cnv_rth  /* [fnc] Convert char, short, long, int, and float types to doubles before arithmetic */
(var_sct *var, /* I/O [var] Variable to be considered for conversion */
 const int nco_op_typ); /* I [enm] Operation type */

var_sct * /* O [sct] Variable reverted to on-disk type */
nco_cnv_var_typ_dsk  /* [fnc] Revert variable to on-disk type */
(var_sct *var); /* I [sct] Variable to be reverted */

var_sct * /* O [sct] Pointer to variable structure of type var_out_typ */
nco_var_cnf_typ /* [fnc] Return copy of input variable typecast to desired type */
(const nc_type var_out_typ, /* I [enm] Type to convert variable structure to */
 var_sct * const var_in); /* I/O [enm] Pointer to variable structure (may be destroyed) */

var_sct * /* O [sct] Pointer to variable structure of type var_out_typ */
nco_var_cnf_typ_tst /* [fnc] Return copy of input variable typecast to desired type */
(const nc_type var_out_typ, /* I [enm] Type to convert variable structure to */
 var_sct * const var_in); /* I/O [enm] Pointer to variable structure (may be destroyed) */

var_sct * /* O [sct] Variable with mss_val converted to typ_upk */
nco_cnv_mss_val_typ  /* [fnc] Convert missing_value, if any, to mss_val_out_typ */
(var_sct *var, /* I [sct] Variable with missing_value to convert */
 const nc_type mss_val_out_typ); /* I [enm] Type of mss_val on output */

void
nco_val_cnf_typ /* [fnc] Copy val_in and typecast from typ_in to typ_out */
(const nc_type typ_in, /* I [enm] Type of input value */
 ptr_unn val_in, /* I [ptr] Pointer to input value */
 const nc_type typ_out, /* I [enm] Type of output value */
 ptr_unn val_out); /* I [ptr] Pointer to output value */

int /* O [enm] Dummy return */
nco_scv_cnf_typ /* [fnc] Convert scalar attribute to typ_new using C implicit coercion */
(const nc_type typ_new, /* I [enm] Type to convert scv_old to */
 scv_sct * const scv_old); /* I/O [sct] Scalar value to convert */

nc_type /* O [enm] Return Highest type */
ncap_typ_hgh /* [fnc] Promote variable to higher common precision */
(nc_type typ_1,  /* I [enm] type */
 nc_type typ_2); /* I [enm] type */

nc_type /* O [enm] Highest precision of input variables */
ncap_var_retype /* [fnc] Promote variable to higher common precision */
(var_sct *var_1, /* I/O [sct] Variable */
 var_sct *var_2); /* I/O [sct] Variable */

nc_type /* O [enm] Highest precision of arguments */
ncap_scv_scv_cnf_typ_hgh_prc /* [fnc] Promote arguments to higher precision if necessary */
(scv_sct * const scv_1, /* I/O [sct] Scalar value */
 scv_sct * const scv_2); /* I/O [sct] Scalar value */

nc_type /* O [enm] Highest precision of arguments */
ncap_var_scv_cnf_typ_hgh_prc /* [fnc] Promote arguments to higher precision if necessary */
(var_sct ** const var, /* I/O [sct] Variable */
 scv_sct * const scv); /* I/O [sct] Scalar value */

nco_bool /* O [flg] Input is integer type */
nco_typ_ntg /* [fnc] Identify integer types */
(const nc_type typ_in); /* I [enm] Type to check for integer-ness */

nco_bool /* O [flg] Input is signed type */
nco_typ_sgn /* [fnc] Identify signed types */
(const nc_type typ_in); /* I [enm] Type to check for signedness */

nco_bool /* O [flg] Input is netCDF3 atomic type */
nco_typ_nc3 /* [fnc] Identify netCDF3 atomic types */
(const nc_type typ_in); /* I [enm] Type to check netCDF3 compliance */

nco_bool /* O [flg] Input is CDF5 atomic type */
nco_typ_nc5 /* [fnc] Identify CDF5 atomic types */
(nc_type typ_in); /* I [enm] Type to check for CDF5 compliance */

nc_type /* O [enm] netCDF3 type */
nco_typ_nc4_nc3 /* [fnc] Convert netCDF4 to netCDF3 atomic type */
(const nc_type typ_nc4); /* I [enm] netCDF4 type */

nc_type /* O [enm] CDF5 atomic type */
nco_typ_nc4_nc5 /* [fnc] Convert netCDF4 to CDF5 atomic type */
(const nc_type typ_nc4); /* I [enm] netCDF4 type */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNF_TYP_H */
