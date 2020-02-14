/* $Header$ */

/* Purpose: Float-precision arithmetic, MSVC macros */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_rth_flt.h" 

/* In ANSI C, <math.h> provides standard math intrinsics in double precision 
   On most architectures, single precision ("float") versions are also supplied 
   C++ compilers need float versions of these functions to be efficient anyway
   Names of these optional float functions end in "f", as specified by ANSI
   Create any needed float functions simply by coercing I/O of double versions
   MacOS X does not provide float versions of _any_ standard function! */
/* fxm: TODO #37 inline these definitions? */
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
#ifdef NEED_ATAN2F
float atan2f(float x,float y){return (float)(atan2((double)x,(double)y));}
#endif /* !NEED_ATAN2F */ 
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
#ifdef NEED_FABSF
float fabsf(float x){return (float)(fabs((double)x));}
#endif /* !NEED_FABSF */ 
#ifdef NEED_FLOORF
float floorf(float x){return (float)(floor((double)x));}
#endif /* !NEED_FLOORF */ 
#ifdef NEED_GAMMAF
float gammaf(float x){return (float)(tgamma((double)x));}
#endif /* !NEED_GAMMAF */ 
#ifdef NEED_LOG10F
float log10f(float x){return (float)(log10((double)x));}
#endif /* !NEED_LOG10F */ 
#ifdef NEED_LOGF
float logf(float x){
  /* fxm TODO ncap57: Eventually remove this debugging statement and the include stdio that it requires */
  /* (void)fprintf(stderr,"%s: DEBUG Using NCO-supplied function logf() from nco_rth_flt.c\n",nco_prg_nm_get()); */
  return (float)(log((double)x));}
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

/* fxm TODO nco652 */
double /* O [frc] Random fraction in [0,1] */
rnd_nbr /* [fnc] Generate random fraction in [0,1] */
(double x) /* I [frc] Immaterial */
{
  /* Purpose: Wrapper for system random number generator 
     Output does not depend on input value of x
     201207: Without srand() and srandom() seeds, same number would be generated each time */
  long rnd_nbr_lng;
  double rnd_nbr_dbl_frc;
  x=x+0.0; /* CEWI */
#ifdef _MSC_VER
  /* Seed random-number generator with current time */
  srand((unsigned)time(NULL));
  rnd_nbr_lng=rand();
#else /* !_MSC_VER */
  /* Seed random-number generator with current time */
  srandom((unsigned)time(NULL));
  rnd_nbr_lng=random();
#endif /* !_MSC_VER */
  rnd_nbr_dbl_frc=rnd_nbr_lng*1.0/RAND_MAX;
  return rnd_nbr_dbl_frc;
} /* end rnd_nbr() */

#define NEED_RND_NBRF
#ifdef NEED_RND_NBRF
float rnd_nbrf(float x){return (float)(rnd_nbr((double)x));}
#endif /* !NEED_RND_NBRF */ 

/* fxm TODO nco1092 */

/* C math library math.h guaranteed to include floor() and ceil()
   So floor() and ceil() are safe to use in definition of other would-be intrinsics
   Helpful summary of POSIX, ISO, and MSVC math intrinsics at
   http://www.johndcook.com/math_h.html */
#ifdef NEED_RINT
/* Lack of double-precision version implies lack of single-precision version */
# define NEED_RINTF
double /* O [frc] Rounded value of x */
rint /* [fnc] Round x to nearest even integer, raise exceptions */
(double x) /* I [frc] Value to round */
{
  /* Purpose: Rounding function for lame systems that lack the intrinsic rint() 
     rint() should round x to nearest integer, using current rounding direction (unlike round(), which always rounds away from zero). Halfway cases are rounded to nearest even integer (!). Yes, that appears to be an accurate summary of this highly technical IEEE floating point rounding algorithm.
     Only difference from nearbyint() is that rint() should raise inexact flag, nearbyint() should not
     NB: POSIX version of rint() sets IEEE inexact exceptions (unlike nearbyint())
     This hacked version does not */
  /* Source:  */
  return (x >= 0.0) ? floor(x+0.5) : ceil(x-0.5);
} /* end rint() */
#endif /* !NEED_RINT */ 

#ifdef NEED_NEARBYINT
/* Lack of double-precision version implies lack of single-precision version */
# define NEED_NEARBYINTF
double /* O [frc] Rounded value of x */
nearbyint /* [fnc] Round x to nearest even integer, do not raise exceptions */
(double x) /* I [frc] Value to round */
{
  /* Purpose: Rounding function for lame systems that lack the intrinsic nearbyint() 
     nearbyint() should round x to nearest integer, using current rounding direction (unlike round(), which always rounds away from zero). Halfway cases are rounded to nearest even integer (!). Yes, that appears to be an accurate summary of this highly technical IEEE floating point rounding algorithm.
     Only difference from rint() is that rint() should raise inexact flag, nearbyint() should not
     NB: POSIX version of nearbyint() does not set IEEE inexact exceptions (unlike rint()) */
  /* Source:  */
  return (x >= 0.0) ? floor(x+0.5) : ceil(x-0.5);
} /* end nearbyint() */
#endif /* !NEED_NEARBYINT */ 

#ifdef NEED_ROUND
/* Lack of double-precision version implies lack of single-precision version */
# define NEED_ROUNDF
double /* O [frc] Rounded value of x */
round /* [fnc] Round x to nearest integer, half-way cases round away from zero */
(double x) /* I [frc] Value to round */
{
  /* Purpose: Rounding function for lame systems that lack the intrinsic round() 
     round() should round x to nearest integer, halfway cases away from zero regardless of current rounding direction (unlike rint() and nearbyint())
     Source: http://www.codeproject.com/Articles/58289/C-Round-Function
     NB: POSIX version should set IEEE inexact exceptions 
     This hacked version does not */
  /* NB: function returns int and then implicit coercion turns receptor into floating point */
  return (x >= 0) ? (int)(x+0.5) : (int)(x-0.5);
} /* end round() */
#endif /* !NEED_ROUND */ 

#ifdef NEED_TRUNC
/* Lack of double-precision version implies lack of single-precision version */
# define NEED_TRUNCF
double /* O [frc] Truncated value of x */
trunc /* [fnc] Truncate x to nearest integer not larger in absolute value */
(double x) /* I [frc] Value to truncate */
{
  /* Purpose: Truncating function for lame systems that lack the intrinsic trunc() 
     trunc() should round x to nearest integer not larger in absolute value 
     Truncation is the same thing as implicit conversion to int */
  /* NB: function returns int and then implicit coercion turns receptor into floating point */
  return (int)x;
} /* end trunc() */
#endif /* !NEED_TRUNC */ 

#ifdef NEED_RINTF
float rintf(float x){return (float)(rint((double)x));}
#endif /* !NEED_RINTF */ 

#ifdef NEED_ROUNDF
float roundf(float x){return (float)(round((double)x));}
#endif /* !NEED_ROUNDF */ 

#ifdef NEED_NEARBYINTF
float nearbyintf(float x){return (float)(nearbyint((double)x));}
#endif /* !NEED_NEARBYINTF */ 

#ifdef NEED_TRUNCF
float truncf(float x){return (float)(trunc((double)x));}
#endif /* !NEED_TRUNCF */ 
