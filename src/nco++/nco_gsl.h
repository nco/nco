/* $Header: /data/zender/nco_20150216/nco/src/nco++/nco_gsl.h,v 1.5 2013-01-13 19:19:21 zender Exp $ */

/* Purpose: GSL functions that handle missing values */

/* nco_gsl.[ch] re-implement GSL functions without arithmetic changes _except_
   that the NCO version (prefixed with nco_) accepts a missing value argument which,
   if non-NULL, indicates data to be "skipped", i.e., treated as missing. 

   GSL and NCO are both distributed under the GPL3 license.
   The GSL code is copyright by its respective authors.
   The NCO modifications that provide missing value support are, in addition, 
   Copyright (C) 2013--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text

   Original GSL files, copyright holders, and authors of functions below are:
   gsl/fit/linear.c: Copyright (C) 2000, 2007 Brian Gough */

/* Usage:
   #include "nco_gsl.h" *//* Missing value-aware GSL functions */

#ifndef NCO_GSL_H
#define NCO_GSL_H

#define ENABLE_NCO_GSL

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Standard header files */
#include <stddef.h> /* size_t */
 
/* GSL macros */
#define CONCAT2x(a,b) a ## _ ## b 
#define CONCAT2(a,b) CONCAT2x(a,b)
#define FUNCTION(drc,nm) CONCAT2(drc,nm)
#define BASE double
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
FUNCTION(nco_gsl_stats,mean) 
(const BASE data[], 
 const size_t stride, 
 const size_t size,
 const double *mss_val); /* fill value */

double 
FUNCTION(nco_gsl_stats,covariance) 
(const BASE data1[], 
 const size_t stride1,
 const BASE data2[], 
 const size_t stride2,
 const size_t n,
 const double *mss_val); /* fill value */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GSL_H */
