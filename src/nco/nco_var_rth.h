/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.h,v 1.2 2002-05-06 03:31:01 zender Exp $ */

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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  void var_add(nc_type,long,int,ptr_unn,long *,ptr_unn,ptr_unn);
  void var_add_no_tally(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2);
  void var_avg_reduce_ttl(nc_type,long,long,int,ptr_unn,long *,ptr_unn,ptr_unn);
  void var_avg_reduce_min(nc_type,long,long,int,ptr_unn,ptr_unn,ptr_unn);
  void var_avg_reduce_max(nc_type,long,long,int,ptr_unn,ptr_unn,ptr_unn);
  void var_divide(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
  void var_mask(nc_type,long,int,ptr_unn,double,int,ptr_unn,ptr_unn);
  void var_max_bnr(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
  void var_min_bnr(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
  void var_multiply(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
  void var_normalize(nc_type,long,int,ptr_unn,long *,ptr_unn);
  void var_normalize_sdn(nc_type,long,int,ptr_unn,long *,ptr_unn);
  void var_abs(nc_type,const long,int,ptr_unn,ptr_unn);
  void var_sqrt(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2);
  void var_subtract(nc_type,long,int,ptr_unn,ptr_unn,ptr_unn);
  void var_zero(nc_type,long,ptr_unn);
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_RTH_H */
