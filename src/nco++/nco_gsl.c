/* $Header: /data/zender/nco_20150216/nco/src/nco++/nco_gsl.c,v 1.3 2013-01-13 07:23:06 pvicente Exp $ */

/* Purpose: gsl functions that handle netCDF fill value */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_gsl.h"

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
 const double *flv)
{
  double m_x = 0, m_y = 0, m_dx2 = 0, m_dxdy = 0;
  size_t i;

  for (i = 0; i < n; i++)
  {
    if (flv==NULL)
    {
      m_x += (x[i * xstride] - m_x) / (i + 1.0);
      m_y += (y[i * ystride] - m_y) / (i + 1.0);
    }
    else
    {
      if (y[i * ystride]!=*flv)
      {
        m_x += (x[i * xstride] - m_x) / (i + 1.0);
        m_y += (y[i * ystride] - m_y) / (i + 1.0);
      }
    }
  }

  for (i = 0; i < n; i++)
  {
    if (flv==NULL)
    {
      const double dx = x[i * xstride] - m_x;
      const double dy = y[i * ystride] - m_y;
      m_dx2 += (dx * dx - m_dx2) / (i + 1.0);
      m_dxdy += (dx * dy - m_dxdy) / (i + 1.0);
    }
    else
    {
      if (y[i * ystride]!=*flv)
      {
        const double dx = x[i * xstride] - m_x;
        const double dy = y[i * ystride] - m_y;
        m_dx2 += (dx * dx - m_dx2) / (i + 1.0);
        m_dxdy += (dx * dy - m_dxdy) / (i + 1.0);
      }
    }
  }

  /* In terms of y = a + b x */

  {
    double s2 = 0, d2 = 0;
    double b = m_dxdy / m_dx2;
    double a = m_y - m_x * b;

    *c0 = a;
    *c1 = b;

    /* Compute chi^2 = \sum (y_i - (a + b * x_i))^2 */

    for (i = 0; i < n; i++)
    {
      if (flv==NULL)
      {
        const double dx = x[i * xstride] - m_x;
        const double dy = y[i * ystride] - m_y;
        const double d = dy - b * dx;
        d2 += d * d;
      }
      else
      {
        if (y[i * ystride]!=*flv)
        {
          const double dx = x[i * xstride] - m_x;
          const double dy = y[i * ystride] - m_y;
          const double d = dy - b * dx;
          d2 += d * d;
        }
      }
    }

    s2 = d2 / (n - 2.0);        /* chisq per degree of freedom */

    *cov_00 = s2 * (1.0 / n) * (1 + m_x * m_x / m_dx2);
    *cov_11 = s2 * 1.0 / (n * m_dx2);

    *cov_01 = s2 * (-m_x) / (n * m_dx2);

    *sumsq = d2;
  }

  return NCO_GSL_SUCCESS;
}



static double
FUNCTION(compute,covariance) 
(const BASE data1[], 
 const size_t stride1,
 const BASE data2[], 
 const size_t stride2,
 const size_t n, 
 const double mean1, 
 const double mean2)
{
  /* takes a dataset and finds the covariance */

  long double covariance = 0 ;

  size_t i;

  /* find the sum of the squares */
  for (i = 0; i < n; i++)
  {
    const long double delta1 = (data1[i * stride1] - mean1);
    const long double delta2 = (data2[i * stride2] - mean2);
    covariance += (delta1 * delta2 - covariance) / (i + 1);
  }

  return covariance ;
}

double 
FUNCTION(nco_gsl_stats,covariance_m) 
(const BASE data1[], 
 const size_t stride1, 
 const BASE data2[], 
 const size_t stride2, 
 const size_t n, 
 const double mean1, 
 const double mean2)
{
  const double covariance = FUNCTION(compute,covariance) (data1, stride1, data2, stride2, n, mean1, mean2);
  return covariance * ((double)n / (double)(n - 1));
}

double 
FUNCTION(nco_gsl_stats,covariance) 
(const BASE data1[], 
 const size_t stride1,
 const BASE data2[], 
 const size_t stride2,
 const size_t n)
{
  const double mean1 = FUNCTION(nco_gsl_stats,mean) (data1, stride1, n);
  const double mean2 = FUNCTION(nco_gsl_stats,mean) (data2, stride2, n);
  return FUNCTION(nco_gsl_stats,covariance_m)(data1, stride1, data2, stride2, n, mean1, mean2);
}


double
FUNCTION (nco_gsl_stats, mean) 
(const BASE data[], 
 const size_t stride, 
 const size_t size)
{
  /* Compute the arithmetic mean of a dataset using the recurrence relation 
  mean_(n) = mean(n-1) + (data[n] - mean(n-1))/(n+1)   */

  long double mean = 0;
  size_t i;

  for (i = 0; i < size; i++)
  {
    mean += (data[i * stride] - mean) / (i + 1);
  }

  return mean;
}


