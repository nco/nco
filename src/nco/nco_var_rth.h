/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.h,v 1.5 2002-06-07 15:10:25 zender Exp $ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
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

  void var_abs(const nc_type,const long,const int,ptr_unn,ptr_unn);
  void var_add(const nc_type,const long,const int,ptr_unn,long *,ptr_unn,ptr_unn);
  void var_add_no_tally(const nc_type type,const long sz,const int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2);
  void var_divide(const nc_type,const long,const int,ptr_unn,ptr_unn,ptr_unn);
  void var_mask(const nc_type,const long,const int,ptr_unn,const double,const int,ptr_unn,ptr_unn);
  void var_max_bnr(const nc_type,const long,const int,ptr_unn,ptr_unn,ptr_unn);
  void var_min_bnr(const nc_type,const long,const int,ptr_unn,ptr_unn,ptr_unn);
  void var_multiply(const nc_type,const long,const int,ptr_unn,ptr_unn,ptr_unn);
  void var_normalize(const nc_type,const long,const int,ptr_unn,long *,ptr_unn);
  void var_normalize_sdn(const nc_type,const long,const int,ptr_unn,long *,ptr_unn);
  void var_sqrt(const nc_type type,const long sz,const int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2);
  void var_subtract(const nc_type,const long,const int,ptr_unn,ptr_unn,ptr_unn);
  void var_zero(const nc_type,const long,ptr_unn);
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_RTH_H */
