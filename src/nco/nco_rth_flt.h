/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rth_flt.h,v 1.8 2002-12-15 06:49:43 zender Exp $ */

/* Purpose: Float-precision arithmetic */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_rth_flt.h" *//* Float-precision arithmetic */

#ifndef NCO_RTH_FLT_H
#define NCO_RTH_FLT_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* Math float prototypes required by AIX, Solaris, but not by Linux, IRIX */
  /* Basic math: acos, asin, atan, cos, exp, log, log10, sin, sqrt, tan */
  float acosf(float);
  float asinf(float);
  float atanf(float);
  float cosf(float);
  float erff(float);
  float erfcf(float);
  float expf(float);
  float gammaf(float);
  float logf(float);
  float log10f(float);
  float powf(float,float);
  float sinf(float);
  float sqrtf(float);
  float tanf(float);

  /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
  float acoshf(float);
  float asinhf(float);
  float atanhf(float);
  float coshf(float);
  float sinhf(float);
  float tanhf(float);

  /* Basic Rounding: ceil, floor */
  float ceilf(float);
  float floorf(float);

  /* Advanced Rounding: nearbyint, rint, round, trunc */
  float nearbyintf(float);
  float rintf(float);
  float roundf(float);
  float truncf(float);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_RTH_FLT_H */
