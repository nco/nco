/* Purpose: gsl functions that handle netCDF fill value */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#ifndef NCO_GSL_H
#define NCO_GSL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Standard header files */
#include <stddef.h> /* size_t */
 
//gsl macros
#define CONCAT2x(a,b) a ## _ ## b 
#define CONCAT2(a,b) CONCAT2x(a,b)
#define FUNCTION(dir,name) CONCAT2(dir,name)
#define BASE double
#define GSL_SUCCESS 0

//nco_gsl forward declarations
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
 const double *flv);

double
FUNCTION(nco_gsl_stats,mean) 
(const BASE data[], 
 const size_t stride, 
 const size_t size);

double 
FUNCTION(nco_gsl_stats,covariance) 
(const BASE data1[], 
 const size_t stride1,
 const BASE data2[], 
 const size_t stride2,
 const size_t n);


#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_AUX_H */