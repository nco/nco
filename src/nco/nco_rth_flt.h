/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rth_flt.h,v 1.5 2002-09-14 17:25:20 zender Exp $ */

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

  /* fxm: Eventually make this MACOSX kludge go away */
#ifndef MACOSX_FLT_FNC
#define MACOSX_FLT_FNC

#ifdef MACOSX
#define NEED_ACOSF 1
#define NEED_ACOSHF 1
#define NEED_ASINF 1
#define NEED_ASINHF 1
#define NEED_ATANF 1
#define NEED_ATANHF 1
#define NEED_CEILF 1
#define NEED_COSF 1
#define NEED_COSHF 1
#define NEED_ERFCF 1
#define NEED_ERFF 1
#define NEED_EXPF 1
#define NEED_FLOORF 1
#define NEED_FMODF 1
#define NEED_GAMMAF 1
#define NEED_LOG10F 1
#define NEED_LOGF 1
#define NEED_POWF 1
#define NEED_SINF 1
#define NEED_SINHF 1
#define NEED_SQRTF 1
#define NEED_TANF 1
#define NEED_TANHF 1
#endif /* !MACOSX */
#ifdef NEED_FMODF
float fmodf(float x,float y){return (float)(fmod((double)x,(double)y));}
#endif /* !NEED_FMODF */ 
#ifdef NEED_POWF
float powf(float x,float y){return (float)(pow((double)x,(double)y));}
#endif /* !NEED_POWF */ 
#ifdef NEED_ACOSF
float acosf(float x){return (float)(acos((double)x));}
#endif /* !NEED_ACOSF */ 
#ifdef NEED_ACOSHF
float acoshf(float x){return (float)(acosh((double)x));}
#endif /* !NEED_ACOSHF */ 
#ifdef NEED_ASINF
float asinf(float x){return (float)(asin((double)x));}
#endif /* !NEED_ASINF */ 
#ifdef NEED_ASINHF
float asinhf(float x){return (float)(asinh((double)x));}
#endif /* !NEED_ASINHF */ 
#ifdef NEED_ATANF
float atanf(float x){return (float)(atan((double)x));}
#endif /* !NEED_ATANF */ 
#ifdef NEED_ATANHF
float atanhf(float x){return (float)(atanh((double)x));}
#endif /* !NEED_ATANHF */ 
#ifdef NEED_CEILF
float ceilf(float x){return (float)(ceil((double)x));}
#endif /* !NEED_CEILF */ 
#ifdef NEED_COSF
float cosf(float x){return (float)(cos((double)x));}
#endif /* !NEED_COSF */ 
#ifdef NEED_COSHF
float coshf(float x){return (float)(cosh((double)x));}
#endif /* !NEED_COSHF */ 
#ifdef NEED_ERFCF
float erfcf(float x){return (float)(erfc((double)x));}
#endif /* !NEED_ERFCF */ 
#ifdef NEED_ERFF
float erff(float x){return (float)(erf((double)x));}
#endif /* !NEED_ERFF */ 
#ifdef NEED_EXPF
float expf(float x){return (float)(exp((double)x));}
#endif /* !NEED_EXPF */ 
#ifdef NEED_FLOORF
float floorf(float x){return (float)(floor((double)x));}
#endif /* !NEED_FLOORF */ 
#ifdef NEED_GAMMAF
float gammaf(float x){return (float)(gamma((double)x));}
#endif /* !NEED_GAMMAF */ 
#ifdef NEED_LOG10F
float log10f(float x){return (float)(log10((double)x));}
#endif /* !NEED_LOG10F */ 
#ifdef NEED_LOGF
float logf(float x){return (float)(log((double)x));}
#endif /* !NEED_LOGF */ 
#ifdef NEED_SINF
float sinf(float x){return (float)(sin((double)x));}
#endif /* !NEED_SINF */ 
#ifdef NEED_SINHF
float sinhf(float x){return (float)(sinh((double)x));}
#endif /* !NEED_SINHF */ 
#ifdef NEED_SQRTF
float sqrtf(float x){return (float)(sqrt((double)x));}
#endif /* !NEED_SQRTF */ 
#ifdef NEED_TANF
float tanf(float x){return (float)(tan((double)x));}
#endif /* !NEED_TANF */ 
#ifdef NEED_TANHF
float tanhf(float x){return (float)(tanh((double)x));}
#endif /* !NEED_TANHF */ 

#endif /* MACOSX_FLT_FNC */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_RTH_FLT_H */
