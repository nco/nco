/* $Header$ */

/* Purpose: OpenMP utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_omp.h" *//* OpenMP utilities */

#ifndef NCO_OMP_H
#define NCO_OMP_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, getenv, exit */
#include <string.h> /* strcmp() */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
#endif

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */
#ifdef _OPENMP
# include <omp.h> /* OpenMP pragmas */
#endif /* !_OPENMP */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */

#ifdef _OPENMP
# ifndef OPENMP_LEGACY
#  ifndef OPENMP_TARGET
/* Unless OPENMP_LEGACY is defined, assume OpenMP library supports target offloading... */
#   define OPENMP_TARGET
#  endif /* !OPENMP_TARGET */
# else /* !OPENMP_LEGACY */
/* ...otherwise assume OpenMP library supports only SMP on host... */
#  ifndef OPENMP_HOST
#   define OPENMP_HOST
#  endif /* !OPENMP_HOST */
# endif /* !OPENMP_LEGACY */
#endif /* !_OPENMP */

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

void
nco_omp_chk(const char *smsg);

void
nco_omp_for_chk(const char *smsg);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_OMP_H */
