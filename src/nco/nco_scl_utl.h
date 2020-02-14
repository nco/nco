/* $Header$ */

/* Purpose: Scalar utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_scl_utl.h" *//* Scalar utilities */

#ifndef NCO_SCL_UTL_H
#define NCO_SCL_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_var_utl.h" /* Variable utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

var_sct * /* O [sct] netCDF variable structure representing val */
scl_dbl_mk_var /* [fnc] Convert scalar double into netCDF variable */
(const double val); /* I [frc] Double precision value to turn into netCDF variable */

var_sct * /* O [sct] Output netCDF variable structure representing val */
scl_mk_var /* [fnc] Convert scalar value of any type into NCO variable */
(val_unn val, /* I [frc] Scalar value to turn into netCDF variable */
 const nc_type val_typ); /* I [enm] netCDF type of value */

var_sct * /* O [sct] Output NCO variable structure representing value */
scl_ptr_mk_var /* [fnc] Convert void pointer to scalar of any type into NCO variable */
(const ptr_unn val_ptr_unn, /* I [unn] Scalar value to turn into netCDF variable */
 const nc_type val_typ); /* I [enm] netCDF type of existing pointer/value */

double /* O [frc] Double precision representation of var->val.?p[0] */
ptr_unn_2_scl_dbl /* [fnc] Convert first element of NCO variable to a scalar double */
(const ptr_unn val, /* I [sct] Pointer union to convert to scalar double */
 const nc_type type); /* I [enm] Type of values pointed to by pointer union */

scv_sct  /* O [sct] Scalar value structure representing val */
ptr_unn_2_scv /* [fnc] Convert ptr_unn to scalar value structure */
(const nc_type type, /* I [enm] netCDF type of value */
 ptr_unn val); /* I [sct] Value to convert to scalar value structure */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SCL_UTL_H */
