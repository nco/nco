/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_omp.h,v 1.27 2012-06-05 21:49:03 pvicente Exp $ */

/* Purpose: OpenMP utilities */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_omp.h" *//* OpenMP utilities */

#ifndef NCO_OMP_H
#define NCO_OMP_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, getenv, exit */
#include <string.h> /* strcmp. . . */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
#endif

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* !_OPENMP */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifndef _OPENMP
/* OpenMP is not available with this compiler
   Prototype harmless stub routines for Uni-Processor (UP) code
   These stubs reduce pre-processor proliferation */
  int omp_get_dynamic(void);
  int omp_get_max_threads(void);
  int omp_get_nested(void);
  int omp_get_num_procs(void);
  int omp_get_num_threads(void);
  int omp_get_thread_num(void);
  int omp_in_parallel(void);
  void omp_set_dynamic(int dynamic_threads);
  void omp_set_nested(int nested);
  void omp_set_num_threads(int num_threads);
#endif /* _OPENMP */

int /* O [nbr] Thread number */
nco_openmp_ini /* [fnc] Initialize OpenMP threading environment */
(int thr_nbr); /* I [nbr] Thread number */

int /* O [enm] Return code */
nco_var_prc_crr_prn /* [fnc] Print name of current variable */
(const int idx, /* I [idx] Index of current variable */
 const char * const var_nm); /* I [sng] Variable name */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_OMP_H */
