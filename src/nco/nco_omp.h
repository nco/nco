/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_omp.h,v 1.4 2002-12-30 02:56:15 zender Exp $ */

/* Purpose: OpenMP utilities */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_omp.h" *//* OpenMP utilities */

#ifndef NCO_OMP_H
#define NCO_OMP_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */
#include <unistd.h> /* POSIX stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* not _OPENMP */

/* Personal headers */
#include "nco.h" /* NCO definitions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int /* O [enm] Return code */
nco_openmp_ini(void); /* [fnc] Set up OpenMP multi-threading environment */

int /* O [enm] Return code */
nco_var_prc_crr_prn /* [fnc] Print name of current variable */
(const int idx, /* I [idx] Index of current variable */
 const char * const var_nm); /* I [sng] Variable name */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_OMP_H */
