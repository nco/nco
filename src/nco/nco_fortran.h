/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_fortran.h,v 1.2 2002-06-17 00:06:02 zender Exp $ */

/* Purpose: Fortran utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_fortran.h" *//* Fortran utilities */

#ifndef NCO_FORTRAN_H
#define NCO_FORTRAN_H

/* Standard header files */

/* 3rd party vendors */

/* Personal headers */

  /* Fortran functions are deprecated as of NCO 1.2, will be removed unless volunteer takes over their maintenance */
#ifdef USE_FORTRAN_ARITHMETIC
#ifdef CRAY
#define FORTRAN_add_real ADD_REAL
#define FORTRAN_add_double_precision ADD_DOUBLE_PRECISION
#define FORTRAN_avg_reduce_real AVG_REDUCE_REAL
#define FORTRAN_avg_reduce_double_precision AVG_REDUCE_DOUBLE_PRECISION
#define FORTRAN_normalize_real NORMALIZE_REAL
#define FORTRAN_normalize_double_precision NORMALIZE_DOUBLE_PRECISION
#define FORTRAN_newdate NEWDATE
#define FORTRAN_subtract_real SUBTRACT_REAL
#define FORTRAN_subtract_double_precision SUBTRACT_DOUBLE_PRECISION
#define FORTRAN_mlt_real MULTIPLY_REAL
#define FORTRAN_mlt_double_precision MULTIPLY_DOUBLE_PRECISION
#define FORTRAN_dvd_real DIVIDE_REAL
#define FORTRAN_dvd_double_precision DIVIDE_DOUBLE_PRECISION
#endif /* CRAY */
#if ( defined RS6K ) || ( defined AIX )
#define FORTRAN_add_real add_real
#define FORTRAN_add_double_precision add_double_precision
#define FORTRAN_avg_reduce_real avg_reduce_real
#define FORTRAN_avg_reduce_double_precision avg_reduce_double_precision
#define FORTRAN_normalize_real normalize_real
#define FORTRAN_normalize_double_precision normalize_double_precision
#define FORTRAN_newdate newdate
#define FORTRAN_subtract_real subtract_real
#define FORTRAN_subtract_double_precision subtract_double_precision
#define FORTRAN_mlt_real multiply_real
#define FORTRAN_mlt_double_precision multiply_double_precision
#define FORTRAN_dvd_real divide_real
#define FORTRAN_dvd_double_precision divide_double_precision
#endif /* RS6K || AIX */
  /* pgf90 subroutines have one underscore by default
     pgf90 underscore behavior is altered by -Mnosecond_underscore
     g77 subroutines have two underscores by default
     g77 functions (e.g., newdate()) have one underscore by default 
     g77 underscore behavior is altered by -fno-second-underscore */
#ifdef LINUX 
#define FORTRAN_add_real add_real_
#define FORTRAN_add_double_precision add_double_precision_
#define FORTRAN_avg_reduce_real avg_reduce_real_
#define FORTRAN_avg_reduce_double_precision avg_reduce_double_precision_
#define FORTRAN_normalize_real normalize_real_
#define FORTRAN_normalize_double_precision normalize_double_precision_
#define FORTRAN_newdate newdate_
#define FORTRAN_subtract_real subtract_real_
#define FORTRAN_subtract_double_precision subtract_double_precision_
#define FORTRAN_mlt_real multiply_real_
#define FORTRAN_mlt_double_precision multiply_double_precision_
#define FORTRAN_dvd_real divide_real_
#define FORTRAN_dvd_double_precision divide_double_precision_
#endif /* LINUX */
#if ( defined ALPHA ) || ( defined SUN4 ) || ( defined SUN4SOL2 ) || ( defined SUNMP ) || ( defined SGI5 ) || ( defined SGI64 ) || ( defined SGIMP64 )
#define FORTRAN_add_real add_real_
#define FORTRAN_add_double_precision add_double_precision_
#define FORTRAN_avg_reduce_real avg_reduce_real_
#define FORTRAN_avg_reduce_double_precision avg_reduce_double_precision_
#define FORTRAN_normalize_real normalize_real_
#define FORTRAN_normalize_double_precision normalize_double_precision_
#define FORTRAN_newdate newdate_
#define FORTRAN_subtract_real subtract_real_
#define FORTRAN_subtract_double_precision subtract_double_precision_
#define FORTRAN_mlt_real multiply_real_
#define FORTRAN_mlt_double_precision multiply_double_precision_
#define FORTRAN_dvd_real divide_real_
#define FORTRAN_dvd_double_precision divide_double_precision_
#endif /* SUN-style */
#endif /* USE_FORTRAN_ARITHMETIC */
  
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  nco_long FORTRAN_newdate(nco_long *,int *);
  void FORTRAN_add_double_precision(long *,int *,double *,long *,double *,double *);
  void FORTRAN_add_real(long *,int *,float *,long *,float *,float *);
  void FORTRAN_avg_reduce_double_precision(long *,long *,int *,double *,long *,double *,double *);
  void FORTRAN_avg_reduce_real(long *,long *,int *,float *,long *,float *,float *);
  void FORTRAN_dvd_double_precision(long *,int *,double *,double *,double *);
  void FORTRAN_dvd_real(long *,int *,float *,float *,float *);
  void FORTRAN_mlt_double_precision(long *,int *,double *,double *,double *);
  void FORTRAN_mlt_real(long *,int *,float *,float *,float *);
  void FORTRAN_normalize_double_precision(long *,int *,double *,long *,double *);
  void FORTRAN_normalize_real(long *,int *,float *,long *,float *);
  void FORTRAN_subtract_double_precision(long *,int *,double *,double *,double *);
  void FORTRAN_subtract_real(long *,int *,float *,float *,float *);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_FORTRAN_H */
