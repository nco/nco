/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rth_flt.h,v 1.1 2002-09-09 07:12:56 zender Exp $ */

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

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* Math float prototypes required by AIX, Solaris, but not by Linux, IRIX */
  /* Basic math: acos, asin, atan, cos, exp, log, log10, sin, sqrt, tan */
  extern float acosf(float);
  extern float asinf(float);
  extern float atanf(float);
  extern float cosf(float);
  extern float erff(float);
  extern float erfcf(float);
  extern float expf(float);
  extern float gammaf(float);
  extern float logf(float);
  extern float log10f(float);
  extern float sinf(float);
  extern float sqrtf(float);
  extern float tanf(float);

  /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
  extern float acoshf(float);
  extern float asinhf(float);
  extern float atanhf(float);
  extern float coshf(float);
  extern float sinhf(float);
  extern float tanhf(float);

  /* Basic Rounding: ceil, floor */
  extern float ceilf(float);
  extern float floorf(float);

  /* Advanced Rounding: nearbyint, rint, round, trunc */
  extern float nearbyintf(float);
  extern float rintf(float);
  extern float roundf(float);
  extern float truncf(float);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_LST_H */
