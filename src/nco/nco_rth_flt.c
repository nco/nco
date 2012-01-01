/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rth_flt.c,v 1.27 2012-01-01 20:51:53 zender Exp $ */

/* Purpose: Float-precision arithmetic */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_rth_flt.h" /* Float-precision arithmetic */

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
float gammaf(float x){return (float)(gamma((double)x));}
#endif /* !NEED_GAMMAF */ 
#ifdef NEED_LOG10F
float log10f(float x){return (float)(log10((double)x));}
#endif /* !NEED_LOG10F */ 
#ifdef NEED_LOGF
float logf(float x){
  /* fxm TODO ncap57: Eventually remove this debugging statement and the include stdio that it requires */
  /* (void)fprintf(stderr,"%s: DEBUG Using NCO-supplied function logf() from nco_rth_flt.c\n",prg_nm_get()); */
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
     Output does not depend on input value of x */
  long rnd_nbr_lng;
  double rnd_nbr_dbl_frc;
  x=x+0.0; /* CEWI */
  rnd_nbr_lng=random();
  rnd_nbr_dbl_frc=rnd_nbr_lng*1.0/RAND_MAX;
  return rnd_nbr_dbl_frc;
} /* end rnd_nbr() */

#define NEED_RND_NBRF
#ifdef NEED_RND_NBRF
float rnd_nbrf(float x){return (float)(rnd_nbr((double)x));}
#endif /* !NEED_RND_NBRF */ 
