/* $Header: /data/zender/nco_20150216/nco/src/nco++/nco_gsl.c,v 1.9 2014-12-31 01:50:08 zender Exp $ */

/* Purpose: GSL functions that handle missing values */

/* nco_gsl.[ch] re-implement GSL functions without arithmetic changes _except_
   that the NCO version (prefixed with nco_) accepts a missing value argument which,
   if non-NULL, indicates data to be "skipped", i.e., treated as missing. 

   GSL and NCO are both distributed under the GPL3 license.
   The GSL code is copyright by its respective authors.
   The NCO modifications that provide missing value support are, in addition, 
   Copyright (C) 2013--2015 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text

   Original GSL files, copyright holders, and authors of functions below are:
   gsl/fit/linear.c: Copyright (C) 2000 Brian Gough
   statistics/covar_source.c: Copyright (C) 1996, 1997, 1998, 1999, 2000 Jim Davies, Brian Gough */

#include "nco_gsl.h" /* Missing value-aware GSL functions */

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
  double m_x = 0, m_y = 0, m_dx2 = 0, m_dxdy = 0;
  size_t i;

#ifdef ENABLE_NCO_GSL
  /* NCO changes: Skip missing values, consider number of valid points */
  size_t nbr_val; /* [nbr] Number of sample points that have data */
  size_t idx_val; /* [nbr] Current valid data index */

  if (mss_val!=NULL)
  {
    nbr_val=0;
    for (i = 0; i < n; i++)
    {
      if (y[i * ystride]!=*mss_val)
      {
        nbr_val++;
      }
    }
  }
#endif /* ENABLE_NCO_GSL */

  if (mss_val==NULL)
  {
    for (i = 0; i < n; i++)
    {
      m_x += (x[i * xstride] - m_x) / (i + 1.0);
      m_y += (y[i * ystride] - m_y) / (i + 1.0);
    }
  }
#ifdef ENABLE_NCO_GSL
  else
  {
    idx_val=0;
    for (i = 0; i < n; i++)
    {
      if (y[i * ystride]!=*mss_val)
      {
        m_x += (x[i * xstride] - m_x) / (idx_val + 1.0);
        m_y += (y[i * ystride] - m_y) / (idx_val + 1.0);
        idx_val++;
      }
    }
  }
#endif /* ENABLE_NCO_GSL */

  if (mss_val==NULL)
  {
    for (i = 0; i < n; i++)
    {
      const double dx = x[i * xstride] - m_x;
      const double dy = y[i * ystride] - m_y;
      m_dx2 += (dx * dx - m_dx2) / (i + 1.0);
      m_dxdy += (dx * dy - m_dxdy) / (i + 1.0);
    }
  }
#ifdef ENABLE_NCO_GSL
  else
  {
    idx_val=0;
    for (i = 0; i < n; i++)
    {
      if (y[i * ystride]!=*mss_val)
      {
        const double dx = x[i * xstride] - m_x;
        const double dy = y[i * ystride] - m_y;
        m_dx2 += (dx * dx - m_dx2) / (idx_val + 1.0);
        m_dxdy += (dx * dy - m_dxdy) / (idx_val + 1.0);
        idx_val++;
      }
    }
  }
#endif /* ENABLE_NCO_GSL */

  /* In terms of y = a + b x */

  {
    double s2 = 0, d2 = 0;
    double b = m_dxdy / m_dx2;
    double a = m_y - m_x * b;

    *c0 = a;
    *c1 = b;

    /* Compute chi^2 = \sum (y_i - (a + b * x_i))^2 */

    if (mss_val==NULL)
    {
      for (i = 0; i < n; i++)
      {
        const double dx = x[i * xstride] - m_x;
        const double dy = y[i * ystride] - m_y;
        const double d = dy - b * dx;
        d2 += d * d;
      }
    }
    else
    {
      for (i = 0; i < n; i++)
      {
        if (y[i * ystride]!=*mss_val)
        {
          const double dx = x[i * xstride] - m_x;
          const double dy = y[i * ystride] - m_y;
          const double d = dy - b * dx;
          d2 += d * d;
        }
      }
    }

    if (mss_val==NULL)
    {
      s2 = d2 / (n - 2.0);        /* chisq per degree of freedom */

      *cov_00 = s2 * (1.0 / n) * (1 + m_x * m_x / m_dx2);
      *cov_11 = s2 * 1.0 / (n * m_dx2);

      *cov_01 = s2 * (-m_x) / (n * m_dx2);

      *sumsq = d2;
    }
#ifdef ENABLE_NCO_GSL /* Use number of sample points that have data */
    else
    {
      s2 = d2 / (nbr_val - 2.0);        /* chisq per degree of freedom */

      *cov_00 = s2 * (1.0 / nbr_val) * (1 + m_x * m_x / m_dx2);
      *cov_11 = s2 * 1.0 / (nbr_val * m_dx2);

      *cov_01 = s2 * (-m_x) / (nbr_val * m_dx2);

      *sumsq = d2;
    }  
  }
#endif /* ENABLE_NCO_GSL */

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

double 
FUNCTION(nco_gsl_stats,covariance_m) 
(const BASE data1[], 
 const size_t stride1, 
 const BASE data2[], 
 const size_t stride2, 
 const size_t n, 
 const double mean1, 
 const double mean2,
 const double *mss_val) /* Missing value */
{
  const double covariance = FUNCTION(compute,covariance) (data1, stride1, data2, stride2, n, mean1, mean2, mss_val);
  return covariance * ((double)n / (double)(n - 1));
}

double 
FUNCTION(nco_gsl_stats,covariance) 
(const BASE data1[], 
 const size_t stride1,
 const BASE data2[], 
 const size_t stride2,
 const size_t n,
 const double *mss_val) /* Missing value */
{
  const double mean1 = FUNCTION(nco_gsl_stats,mean) (data1, stride1, n, mss_val);
  const double mean2 = FUNCTION(nco_gsl_stats,mean) (data2, stride2, n, mss_val);
  return FUNCTION(nco_gsl_stats,covariance_m)(data1, stride1, data2, stride2, n, mean1, mean2, mss_val);
}


double
FUNCTION (nco_gsl_stats, mean) 
(const BASE data[], 
 const size_t stride, 
 const size_t size,
 const double *mss_val) /* Missing value */
{
  /* Compute the arithmetic mean of a dataset using the recurrence relation 
  mean_(n) = mean(n-1) + (data[n] - mean(n-1))/(n+1)   */

  long double mean = 0;
  size_t i;

  if (mss_val==NULL)
  {
    for (i = 0; i < size; i++)
    {
      mean += (data[i * stride] - mean) / (i + 1);
    }
  }
  else
  {
    for (i = 0; i < size; i++)
    {
      if (data[i * stride]!=*mss_val)
      {
        mean += (data[i * stride] - mean) / (i + 1);
      }
    }
  }

  return mean;
}


