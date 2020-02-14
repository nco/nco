/* $Header$ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_var_rth.h" *//* Variable arithmetic */

#ifndef NCO_VAR_RTH_H
#define NCO_VAR_RTH_H

/* Standard header files */
/* C++ compilers have math.h but do not define constants like M_LN10, M_LN2 unless _USE_MATH_DEFINES is first defined */
#ifdef _MSC_VER
# define _USE_MATH_DEFINES
#endif /* !_MSC_VER */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_rth_flt.h" /* Float-precision arithmetic, MSVC macros */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  typedef union{ /* f32_u32_unn */
    float f32;
    unsigned int u32;
  } f32_u32_unn;
  typedef union{ /* f64_u64_unn */
    double f64;
    unsigned long u64;
  } f64_u64_unn;
  
  void
  nco_var_abs /* [fnc] Replace op1 values by their absolute values */
  (const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1); /* I/O [val] Values of first operand */
  
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
   const double wgt_crr, /* I [frc] Weight of current record (ncra/ncea only) */
   double * restrict const wgt_sum, /* I/O [frc] Running sum of per-file weights (ncra/ncea only) */
   ptr_unn op1, /* I [val] Values of first operand */
   ptr_unn op2); /* I/O [val] Values of second operand (running sum) on input, values of new sum on output */
  
  void 
  nco_var_copy_tll /* [fnc] Copy hyperslab variables of type var_typ from op1 to op2, accounting for missing values in tally */
  (const nc_type type, /* I [enm] netCDF type */
   const long sz, /* I [nbr] Number of elements to copy */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   long * restrict const tally, /* O [nbr] Counter space */
   const ptr_unn op1, /* I [sct] Values to copy */
   ptr_unn op2); /* O [sct] Destination to copy values to */

  void
  nco_var_tll_zro_mss_val /* [fnc] Write missing value into elements with zero tally */
  (const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   const long * const tally, /* I [nbr] Counter to normalize by */
   ptr_unn op1); /* I/O [val] Values of first operand on input, possibly missing values on output */

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
  nco_var_nrm_wgt /* [fnc] Normalize value of first operand by weight array */
  (const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   const long * const tally, /* I [nbr] Counter to normalize by */
   const double * const wgt, /* I [nbr] Weight to normalize by */
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
