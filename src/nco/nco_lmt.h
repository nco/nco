/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_lmt.h,v 1.49 2012-08-10 19:08:18 zender Exp $ */

/* Purpose: Hyperslab limits */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_lmt.h" *//* Hyperslab limits */

#ifndef NCO_LMT_H
#define NCO_LMT_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */
#ifdef HAVE_STRINGS_H
# include <strings.h> /* strcasecmp() */
#endif /* !HAVE_STRINGS_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_cln_utl.h" /* Multi-Calendar functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum lmt_typ{ /* [enm] Limit type */
 lmt_crd_val, /* 0, Coordinate value limit */
 lmt_dmn_idx, /* 1, Dimension index limit */
 lmt_udu_sng /* 2, UDUnits string */
}; /* end lmt_typ enum */
  
void
nco_lmt_evl /* [fnc] Parse user-specified limits into hyperslab specifications */
(int nc_id, /* I [idx] netCDF file ID */
 lmt_sct *lmt_ptr, /* I/O [sct] Structure from nco_lmt_prs() or from nco_lmt_sct_mk() to hold dimension limit information */
 long cnt_crr, /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 nco_bool FORTRAN_IDX_CNV); /* I [flg] Hyperslab indices obey Fortran convention */

lmt_sct * /* O [sct] Pointer to free'd structure */
nco_lmt_free /* [fnc] Free memory associated with limit structure */
(lmt_sct *lmt); /* I/O [sct] Limit structure to free */

char * /* O [sng] Units string */
nco_lmt_get_udu_att /* Successful conversion returns units attribute otherwise null */
(const int nc_id, /* I [idx] netCDF file ID */
 const int var_id,
 const char *att_nm); /* I [id] Variable ID whose attribute to read */

lmt_sct ** /* O [sct] Pointer to free'd structure list */
nco_lmt_lst_free /* [fnc] Free memory associated with limit structure list */
(lmt_sct **lmt_lst, /* I/O [sct] Limit structure list to free */
 const int lmt_nbr); /* I [nbr] Number of limit structures in list */

lmt_all_sct * /* O [sct] Pointer to free'd structure */
nco_lmt_all_free /* [fnc] Free memory associated with lmt_all structure */
(lmt_all_sct *lmt_all); /* I/O [sct] Limit structure to free */

lmt_all_sct ** /* O [sct] Pointer to free'd structure list */
nco_lmt_all_lst_free /* [fnc] Free memory associated with lmt_all structure list */
(lmt_all_sct **lmt_all_lst, /* I/O [sct] Limit structure list to free */
 const int lmt_all_nbr); /* I [nbr] Number of limit structures in list */

lmt_sct ** /* O [sct] Structure list with user-specified strings for min and max limits */
nco_lmt_prs /* [fnc] Create limit structures with name, min_sng, max_sng elements */
(const int lmt_nbr, /* I [nbr] number of dimensions with limits */
 CST_X_PTR_CST_PTR_CST_Y(char,lmt_arg)); /* I [sng] List of user-specified dimension limits */

lmt_sct * /* [sct] Limit structure for dimension */
nco_lmt_sct_mk /* [fnc] Create stand-alone limit structure for given dimension */
(const int nc_id, /* I [idx] netCDF file ID */
 const int dmn_id, /* I [idx] ID of dimension for this limit structure */
 CST_X_PTR_CST_PTR_CST_Y(lmt_sct,lmt), /* I [sct] Array of limit structures from nco_lmt_evl() */ 
 int lmt_nbr, /* I [nbr] Number of limit structures */
 const nco_bool FORTRAN_IDX_CNV); /* I [flg] Hyperslab indices obey Fortran convention */

int /* O [enm] Limit type */
nco_lmt_typ /* [fnc] Determine limit type */
(char *sng);/* I [ptr] Pointer to limit string */


#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_LMT_H */
