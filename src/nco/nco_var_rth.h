/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.h,v 1.12 2004-01-05 17:29:05 zender Exp $ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_var_rth.h" *//* Variable arithmetic */

#ifndef NCO_VAR_RTH_H
#define NCO_VAR_RTH_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  void nco_var_abs(const nc_type,const long,const int,ptr_unn,ptr_unn);
  void nco_var_add_tll(const nc_type,const long,const int,ptr_unn,long *,ptr_unn,ptr_unn);
  void nco_var_mask(const nc_type,const long,const int,ptr_unn,const double,const int,ptr_unn,ptr_unn);
  void nco_var_nrm(const nc_type,const long,const int,ptr_unn,long *,ptr_unn);
  void nco_var_nrm_sdn(const nc_type,const long,const int,ptr_unn,long *,ptr_unn);
  void nco_var_sqrt(const nc_type type,const long sz,const int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2);
  void nco_var_zero(const nc_type,const long,ptr_unn);
  
  void
  nco_var_add /* [fnc] Add first operand to second operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of sum on output */

  void
  nco_var_dvd /* [fnc] Divide second operand by first operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of quotient on output */

  void
  nco_var_max_bnr /* [fnc] Maximize two operands */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of maximum on output */

void
  nco_var_min_bnr /* [fnc] Minimize two operands */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of minimum on output */

  void
  nco_var_mlt /* [fnc] Multiply first operand by second operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of product on output */

  void
  nco_var_sbt /* [fnc] Subtract first operand from second operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [flg] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of difference on output */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_RTH_H */
