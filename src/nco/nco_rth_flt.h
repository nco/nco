/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rth_flt.h,v 1.53 2014-12-31 01:50:07 zender Exp $ */

/* Purpose: Float-precision arithmetic, MSVC macros */

/* Copyright (C) 1995--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_rth_flt.h" *//* Float-precision arithmetic, MSVC macros */

#ifndef NCO_RTH_FLT_H
#define NCO_RTH_FLT_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */

#ifdef _MSC_VER
# include <float.h> /* isfinite(), isinf(), isnan() */
#endif /* !_MSC_VER */
/* fxm stdio only needed for TODO ncap57 on UNIX */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <time.h> /* time() seed for random()/rand() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Home-brewed functions like rnd_nbr() need no pre-processor token protection since 
   they will never be defined in system-supplied libraries/headers */
double /* O [frc] Random fraction in [0,1] */
rnd_nbr /* [fnc] Generate random fraction in [0,1] */
(double x); /* I [frc] Immaterial */

float /* O [frc] Random fraction in [0,1] */
rnd_nbrf /* [fnc] Generate random fraction in [0,1] */
(float); /* I [frc] Immaterial */

/* Remaining definitions are system-dependent */

/* MSVC does not define isnormal(), isnan(), isinf(), isfinite()
   http://stackoverflow.com/questions/2249110/how-do-i-make-a-portable-isnan-isinf-function */
#ifdef _MSC_VER
  /* isnormal() may be in MSVC 2012 with header amp_math.h. fxm: verify and utilize if true. */
# define isnormal
  /* Following functions are in MSVC 2008+ and require float.h */
# define isnan(x) _isnan(x)
# define isfinite(x) _finite(x)
# define isinf(x) (!_finite(x))
#endif /* !_MSC_VER */

#ifdef _MSC_VER
  /* _snprintf() is in MSVC 2005+ and requires stdio.h */
# define snprintf _snprintf
#endif /* !_MSC_VER */

/* MSVC does not define lround(), lroundf(), lroundl(), llround(), llroundf(), llroundl(): Round to nearest integer, halfway cases round away from 0
   MSVC does not define lrint(), lrintf(), lrintl(), llrint(), llrintf(), llrintl(): Round to nearest even integer, raise exceptions
   Summary of POSIX, ISO, and MSVC math intrinsics at http://www.johndcook.com/math_h.html */
#ifdef _MSC_VER
  long long int llrint(double x);
  long long int llrintf(float x);
  long int lrint(double x);
  long int lrintf(float x);
  long long int llround(double x);
  long long int llroundf(float x);
  long int lround(double x);
  long int lroundf(float x);
#endif /* !_MSC_VER */ 

#ifdef _MSC_VER
  /* 20130227: Hard-code these because MSVC does not have access to config.h
     Lack of double-precision version implies lack of single-precision version
     Hence single-precision ("float") tokens are set in double-precision prototypes below */
# define NEED_RINT
# define NEED_NEARBYINT
# define NEED_ROUND
# define NEED_TRUNC
#endif /* !_MSC_VER */

#ifdef NEED_RINT
# define NEED_RINTF
double /* O [frc] Rounded value of x */
rint /* [fnc] Round x to nearest even integer, raise exceptions */
(double x); /* I [frc] Value to round */
#endif /* !NEED_RINT */ 

#ifdef NEED_NEARBYINT
# define NEED_NEARBYINTF
double /* O [frc] Rounded value of x */
nearbyint /* [fnc] Round x to nearest even integer, do not raise exceptions */
(double x); /* I [frc] Value to round */
#endif /* !NEED_NEARBYINT */ 

#ifdef NEED_ROUND
# define NEED_ROUNDF
double /* O [frc] Rounded value of x */
round /* [fnc] Round x to nearest integer, half-way cases round away from zero */
(double x); /* I [frc] Value to round */
#endif /* !NEED_ROUND */ 

#ifdef NEED_TRUNC
# define NEED_TRUNCF
double /* O [frc] Truncated value of x */
trunc /* [fnc] Truncate x to nearest integer not larger in absolute value */
(double x); /* I [frc] Value to truncate */
#endif /* !NEED_TRUNC */

#if !defined(HPUX) && !defined(__INTEL_COMPILER) && !defined(LINUXAMD64)
  /* Math float prototypes required by AIX, Solaris, but not by Linux, IRIX
     20040708: HP-UX does not like these 
     20090223: Intel compilers version 11.x complains about these
     20130724: Yellowstone chokes on these when compiling ncap2 with g++ */

  /* Basic math: acos, asin, atan, atan2, cos, exp, fabs, log, log10, pow, sin, sqrt, tan */
  float acosf(float);
  float asinf(float);
  float atanf(float);
  float atan2f(float,float);
  float cosf(float);
  float expf(float);
  float fabsf(float);
  float logf(float);
  float log10f(float);
  float powf(float,float);
  float sinf(float);
  float sqrtf(float);
  float tanf(float);

  /* Advanced math: erf, erfc, gamma, rnd_nbr */
  float erff(float);
  float erfcf(float);
  float gammaf(float);

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

#endif /* !defined(HPUX) && !defined(__INTEL_COMPILER) && !defined(LINUXAMD64) */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_RTH_FLT_H */
