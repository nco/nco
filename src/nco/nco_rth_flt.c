/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rth_flt.c,v 1.2 2002-09-10 04:54:10 zender Exp $ */

/* Purpose: Float-precision arithmetic */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_rth_flt.h" /* Float-precision arithmetic */

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
inline float fmodf(float x,float y){return (float)(fmod((double)x,(double)y));}
#endif /* !NEED_FMODF */ 
#ifdef NEED_POWF
inline float powf(float x,float y){return (float)(pow((double)x,(double)y));}
#endif /* !NEED_POWF */ 
#ifdef NEED_GAMMAF
#ifdef NEED_ACOSF
inline float acosf(float x){return (float)(acos((double)x));}
#endif /* !NEED_ACOSF */ 
#ifdef NEED_ACOSHF
inline float acoshf(float x){return (float)(acosh((double)x));}
#endif /* !NEED_ACOSHF */ 
#ifdef NEED_ASINF
inline float asinf(float x){return (float)(asin((double)x));}
#endif /* !NEED_ASINF */ 
#ifdef NEED_ASINHF
inline float asinhf(float x){return (float)(asinh((double)x));}
#endif /* !NEED_ASINHF */ 
#ifdef NEED_ATANF
inline float atanf(float x){return (float)(atan((double)x));}
#endif /* !NEED_ATANF */ 
#ifdef NEED_ATANHF
inline float atanhf(float x){return (float)(atanh((double)x));}
#endif /* !NEED_ATANHF */ 
#ifdef NEED_CEILF
inline float ceilf(float x){return (float)(ceil((double)x));}
#endif /* !NEED_CEILF */ 
#ifdef NEED_COSF
inline float cosf(float x){return (float)(cos((double)x));}
#endif /* !NEED_COSF */ 
#ifdef NEED_COSHF
inline float coshf(float x){return (float)(cosh((double)x));}
#endif /* !NEED_COSHF */ 
#ifdef NEED_ERFCF
inline float erfcf(float x){return (float)(erfc((double)x));}
#endif /* !NEED_ERFCF */ 
#ifdef NEED_ERFF
inline float erff(float x){return (float)(erf((double)x));}
#endif /* !NEED_ERFF */ 
#ifdef NEED_EXPF
inline float expf(float x){return (float)(exp((double)x));}
#endif /* !NEED_EXPF */ 
#ifdef NEED_FLOORF
inline float floorf(float x){return (float)(floor((double)x));}
#endif /* !NEED_FLOORF */ 
inline float gammaf(float x){return (float)(gamma((double)x));}
#endif /* !NEED_GAMMAF */ 
#ifdef NEED_LOG10F
inline float log10f(float x){return (float)(log10((double)x));}
#endif /* !NEED_LOG10F */ 
#ifdef NEED_LOGF
inline float logf(float x){return (float)(log((double)x));}
#endif /* !NEED_LOGF */ 
#ifdef NEED_SINF
inline float sinf(float x){return (float)(sin((double)x));}
#endif /* !NEED_SINF */ 
#ifdef NEED_SINHF
inline float sinhf(float x){return (float)(sinh((double)x));}
#endif /* !NEED_SINHF */ 
#ifdef NEED_SQRTF
inline float sqrtf(float x){return (float)(sqrt((double)x));}
#endif /* !NEED_SQRTF */ 
#ifdef NEED_TANF
inline float tanf(float x){return (float)(tan((double)x));}
#endif /* !NEED_TANF */ 
#ifdef NEED_TANHF
inline float tanhf(float x){return (float)(tanh((double)x));}
#endif /* !NEED_TANHF */ 
#endif /* MACOSX_FLT_FNC */
