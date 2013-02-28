/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_prn.h,v 1.35 2013-02-28 11:49:01 pvicente Exp $ */

/* Purpose: Printing variables, attributes, metadata */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_prn.h" *//* Printing variables, attributes, metadata */

#ifndef NCO_PRN_H
#define NCO_PRN_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */

/* fxm: strings statically allocated with NCO_MAX_LEN_FMT_SNG chars are susceptible to buffer overflow attacks */
/* Length should be computed at run time but is a pain */
#define NCO_MAX_LEN_FMT_SNG 100

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 
nco_prn_att /* [fnc] Print all attributes of single variable or group */
(const int in_id, /* I [id] netCDF file ID */
 const int grp_id, /* I [id] netCDF group ID */
 const int var_id); /* I [id] netCDF input variable ID */

const char * /* O [sng] sprintf() format string for type typ */
nco_typ_fmt_sng /* [fnc] Provide sprintf() format string for specified type */
(const nc_type typ); /* I [enm] netCDF type to provide format string for */

void
nco_prn_var_val_lmt /* [fnc] Print variable data */
(const int in_id, /* I [id] netCDF input file ID */
 const char * const var_nm, /* I [sng] Variable name */
 const lmt_sct * const lmt, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] number of dimensions with user-specified limits */
 char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool PRN_DMN_UNITS, /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL); /* I [flg] Print dimension/coordinate indices/values */

void
nco_prn_var_dfn /* [fnc] Print variable metadata */
(int nc_id, /* I [id] netCDF file ID */
 const trv_sct * const var_trv); /* I [sct] Object to print (variable) */



#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_PRN_H */
