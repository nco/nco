/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_gmm.h,v 1.6 2010-01-05 20:02:17 zender Exp $ */

/* Purpose: Calculate incomplete gamma function */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_gmm.h" *//* Incomplete Gamma function */

#ifndef NCO_GMM_H /* Contents have not yet been inserted in current source file */
#define NCO_GMM_H 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


double              /* O [dbl] absolute value */ 
r8_abs                    
(double x);         /* I [dbl] */ 

double alngam       /* log of gamma function */ 
(double xvalue,     /* I [dbl] */ 
int *ifault );      /* O [flg]  error flag */  

double             /* O [dbl] result */
nco_gamain         /* incomplete gamma function */
(double p,         /* I  [p] */  
double x,          /* I  [x] */  
int *ifault );     /* O  [flg] error flag*/


float              /* O [flt] absolute value */ 
r8_abs_f                    
(float x);         /* I [flt] */ 

float alngam_f     /* log of gamma function */ 
(float xvalue,     /* I [flt] */ 
int *ifault);      /* O [flg]  error flag */  



float nco_gamain_f    /* O [flt] result */
(float p,         /* I  [p] */  
float x,          /* I  [x] */  
int *ifault );    /* O  [flg] error flag */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GMM_H */
