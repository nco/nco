/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_gmm.h,v 1.1 2008-09-25 15:49:56 zender Exp $ */

/* Purpose: Calculate incomplete gamma function */

/* Copyright (C) 1995--2008 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_gmm.h" *//* Incomplete Gamma function */

#ifndef NCO_GMM_H /* Contents have not yet been inserted in current source file */
#define NCO_GMM_H 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  double r8_abs ( double x );
  double alngam ( double xvalue, int *ifault );
  float r8_abs_f ( float x );
  float alngam_f ( float xvalue, int *ifault );

double gamain    /* O [dbl] result */
(double x,       /* I  [x] */  
double p,        /* I  [y] */  
int *ifault );   /* O  [flg] error */

float gamain_f    /* O [flt] result */
(float x,         /* I  [x] */  
float p,          /* I  [y] */  
int *ifault );    /* O  [flg] error */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GMM_H */
