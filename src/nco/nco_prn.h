/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_prn.h,v 1.1 2002-05-06 02:17:56 zender Exp $ */

/* Purpose: Printing variables, attributes, metadata */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_prn.h" *//* Printing variables, attributes, metadata */

#ifndef NCO_PRN_H
#define NCO_PRN_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */
#include <unistd.h> /* POSIX stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 
prn_att /* [fnc] Print all attributes of single variable */
(const int in_id, /* I [id] netCDF input file ID */
 const int var_id); /* I [id] netCDF input variable ID */

char * /* O [sng] sprintf() format string for type type */
type_fmt_sng /* [fnc] Provide sprintf() format string for specified type */
(const nc_type type); /* I [enm] netCDF type to provide format string for */

void
prn_var_dfn /* [fnc] Print variable metadata */
(int in_id, /* I [id] netCDF input file ID */
 char *var_nm); /* I [sng] Variable to pring */

void
prn_var_val_lmt /* [fnc] Print variable data */
(const int in_id, /* I [id] netCDF input file ID */
 const char * const var_nm, /* I [sng] Variable name */
 const lmt_sct * const lmt, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] number of dimensions with user-specified limits */
 const char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const bool FORTRAN_STYLE, /* I [flg] Hyperslab indices obey Fortran convention */
 const bool PRINT_DIMENSIONAL_UNITS, /* I [flg] Print units attribute, if any */
 const bool PRN_DMN_IDX_CRD_VAL); /* I [flg] Print dimension/coordinate indices/values */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_PRN_H */
