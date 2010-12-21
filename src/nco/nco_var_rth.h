/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.h,v 1.31 2010-12-21 20:12:07 zender Exp $ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_var_rth.h" *//* Variable arithmetic */

#ifndef NCO_VAR_RTH_H
#define NCO_VAR_RTH_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_rth_flt.h" /* Float-precision arithmetic */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  void
  nco_var_abs /* [fnc] Replace op1 values by their absolute values */
  (const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1); /* I [val] Values of first operand */
  
  void
  nco_var_add /* [fnc] Add first operand to second operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of sum on output */
  
  void
  nco_var_add_tll_ncflint /* [fnc] Add first operand to second operand, increment tally */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   long * restrict const tally, /* I/O [nbr] Counter space */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of sum on output */
  
  void
  nco_var_add_tll_ncra /* [fnc] Add first operand to second operand, increment tally */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   long * restrict const tally, /* I/O [nbr] Counter space */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand (running sum) on input, values of new sum on output */
  
  void
  nco_var_dvd /* [fnc] Divide second operand by first operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I [val] Values of numerator */
   ptr_unn op2); /* I/O [val] Values of denominator on input, values of quotient on output */
  
  void
  nco_var_max_bnr /* [fnc] Maximize two operands */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of maximum on output */
  
  void
  nco_var_min_bnr /* [fnc] Minimize two operands */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of minimum on output */
  
  void
  nco_var_msk /* [fnc] Mask third operand where first and second operands fail comparison */
  (const nc_type type, /* I [enm] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operand op3 */
   const int has_mss_val, /* I [flg] Flag for missing values (basically assumed to be true) */
   ptr_unn mss_val, /* I [val] Value of missing value */
   const double op1, /* I [val] Target value to compare against mask field (i.e., argument of -M) */
   const int op_typ_rlt, /* I [enm] Comparison type test for op2 and op1 */
   ptr_unn op2, /* I [val] Value of mask field */
   ptr_unn op3); /* I/O [val] Values of second operand on input, masked values on output */
  
  void
  nco_var_mlt /* [fnc] Multiply first operand by second operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of product on output */
  
  void
  nco_var_mod /* [fnc] Remainder (modulo) operation of two variables */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I [val] Values of field */
   ptr_unn op2); /* I/O [val] Values of divisor on input, values of remainder on output */
  
  void
  nco_var_nrm /* [fnc] Normalize value of first operand by count in tally array */
  (const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   const long * const tally, /* I [nbr] Counter to normalize by */
   ptr_unn op1); /* I/O [val] Values of first operand on input, normalized result on output */
  
  void
  nco_var_nrm_sdn /* [fnc] Normalize value of first operand by count-1 in tally array */
  (const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   const long * const tally, /* I [nbr] Counter to normalize by */
   ptr_unn op1); /* I/O [val] Values of first operand on input, normalized result on output */
  
  void
  nco_var_pwr /* [fnc] Raise first operand to power of second operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I [val] Values of base */
   ptr_unn op2); /* I/O [val] Values of exponent on input, values of power on output */
  
  void
  nco_var_sbt /* [fnc] Subtract first operand from second operand */
  (const nc_type type, /* I [type] netCDF type of operands */
   const long sz, /* I [nbr] Size (in elements) of operands */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand on input, values of difference on output */

  void
  nco_var_sqrt /* [fnc] Place squareroot of first operand in value of second operand */
  (const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   long * restrict const tally, /* I/O [nbr] Counter space */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* O [val] Squareroot of first operand */
  
  void
  nco_var_zero /* [fnc] Zero value of first operand */
  (const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   ptr_unn op1); /* O [val] Values of first operand zeroed on output */
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_RTH_H */
