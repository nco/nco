/* $Header$ */

/* Purpose: GSL functions that handle missing values - stubs only 
   Extra NCO code derived from GSL removed 2020-04-02 to avoid license conflict between GSL GPL and NCO BSD code */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_gsl.hh"

static double
compute_covariance
(const double data1[],
 const size_t stride1,
 const double data2[],
 const size_t stride2,
 const size_t n,
 const double mean1,
 const double mean2,
 const double *mss_val) /* Missing value */
{
  /* takes a dataset and finds the covariance */

  long double covariance = 0 ;

  size_t i;

  /* find the sum of the squares */
  if (mss_val==NULL)
  {
    for (i = 0; i < n; i++)
    {
      const long double delta1 = (data1[i * stride1] - mean1);
      const long double delta2 = (data2[i * stride2] - mean2);
      covariance += (delta1 * delta2 - covariance) / (i + 1);
    }

  }
  else
  {
    for (i = 0; i < n; i++)
    {
      if (data2[i * stride2]!=*mss_val)
      {
        const long double delta1 = (data1[i * stride1] - mean1);
        const long double delta2 = (data2[i * stride2] - mean2);
        covariance += (delta1 * delta2 - covariance) / (i + 1);
      }
    }
  }

  return covariance ;
}

#ifdef ENABLE_GSL

/* Fit the data (x_i, y_i) to the linear relationship

   Y = c0 + c1 x

   returning,

   c0, c1  --  coefficients
   cov00, cov01, cov11  --  variance-covariance matrix of c0 and c1,
   sumsq   --   sum of squares of residuals

   This fit can be used in the case where the errors for the data are
   uknown, but assumed equal for all points. The resulting
   variance-covariance matrix estimates the error in the coefficients
   from the observed variance of the points around the best fit line.
*/

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
 const double *mss_val)
{
  return gsl_fit_linear(x, xstride, y, ystride, n, c0, c1, cov_00, cov_01, cov_11, sumsq);
}

double 
nco_gsl_stats_covariance_m
(const double data1[],
 const size_t stride1, 
 const double data2[],
 const size_t stride2, 
 const size_t n, 
 const double mean1, 
 const double mean2,
 const double *mss_val) /* Missing value */
{
  return gsl_stats_covariance_m(data1, stride1, data2,stride2,n,mean1,mean2);
}

double 
nco_gsl_stats_covariance
(const double data1[],
 const size_t stride1,
 const double data2[],
 const size_t stride2,
 const size_t n,
 const double *mss_val) /* Missing value */
{
  return gsl_stats_covariance(data1, stride1, data2,stride2,n);
}

double
nco_gsl_stats_mean
(const double data[],
 const size_t stride, 
 const size_t size,
 const double *mss_val) /* Missing value */
{
  return gsl_stats_mean(data, stride, size);
}

#else /* !ENABLE_GSL */

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
 const double *mss_val)
{
  return EXIT_FAILURE;
}

double
nco_gsl_stats_covariance_m
(const double data1[],
 const size_t stride1,
 const double data2[],
 const size_t stride2,
 const size_t n,
 const double mean1,
 const double mean2,
 const double *mss_val) /* Missing value */
{
  return *mss_val;
}

double
nco_gsl_stats_covariance
(const double data1[],
 const size_t stride1,
 const double data2[],
 const size_t stride2,
 const size_t n,
 const double *mss_val) /* Missing value */
{
  return *mss_val;
}

double
nco_gsl_stats_mean
(const double data[],
 const size_t stride,
 const size_t size,
 const double *mss_val) /* Missing value */
{
  return *mss_val;
}
#endif /* !ENABLE_GSL */
