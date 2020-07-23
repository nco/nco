/* $Header$ */

/* Purpose: GSL functions that handle missing values */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the
   3-Clause BSD License with exceptions described in the LICENSE file */

/* nco_gsl.[ch] re-implement GSL functions without arithmetic changes _except_
   that the NCO version (prefixed with nco_) accepts a missing value argument which,
   if non-NULL, indicates data to be "skipped", i.e., treated as missing.
   20200403: Above description was true until today, when we removed the GSL-derived code */

/* Usage:
   #include "nco_gsl.hh" *//* Missing value-aware GSL functions */

#ifndef NCO_GSL_HH
#define NCO_GSL_HH

#define ENABLE_NCO_GSL /* If disabled results should be the same as the GSL equivalent function; validation only */ 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Standard header files */
#include <stddef.h> /* size_t */

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* 3rd party vendors */
#ifdef ENABLE_GSL
  #include <gsl/gsl_statistics.h>
  #include <gsl/gsl_fit.h>
#endif

/* Personal headers */
#include "nco.h"

#define NCO_GSL_SUCCESS 0

  /* Forward declarations */
int
nco_gsl_fit_linear
(const double *x, 
 const size_t xstride,
 const double *y, 
 const size_t ystride,
 const size_t n,
 double *c0, 
 double *c1,
 double *cov_00, 
 double *cov_01, 
 double *cov_11, 
 double *sumsq,
 const double *mss_val);

double
nco_gsl_stats_mean
(const double data[],
 const size_t stride, 
 const size_t size,
 const double *mss_val); /* fill value */

double 
nco_gsl_stats_covariance
(const double data1[],
 const size_t stride1,
 const double data2[],
 const size_t stride2,
 const size_t n,
 const double *mss_val); /* fill value */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GSL_HH */
