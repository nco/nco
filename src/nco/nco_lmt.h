/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_lmt.h,v 1.3 2002-05-07 08:34:15 zender Exp $ */

/* Purpose: Hyperslab limits */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_lmt.h" *//* Hyperslab limits */

#ifndef NCO_LMT_H
#define NCO_LMT_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
lmt_evl /* [fnc] Parse user-specified limits into hyperslab specifications */
(int nc_id, /* I [idx] netCDF file ID */
 lmt_sct *lmt_ptr, /* I/O [sct] Structure from lmt_prs() or from lmt_sct_mk() to hold dimension limit information */
 long cnt_crr, /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 bool FORTRAN_STYLE); /* I [flg] Hyperslab indices obey Fortran convention */

lmt_sct * /* O [sct] Structure with user-specified strings for min and max limits */
lmt_prs /* [fnc] Create limit structures with name, min_sng, max_sng elements */
(const int lmt_nbr, /* I [nbr] number of dimensions with limits */
 char * const * const lmt_arg); /* I [sng] list of user-specified dimension limits */

lmt_sct /* [sct] Limit structure for dimension */
lmt_sct_mk /* [fnc] Create stand-alone limit structure for given dimension */
(const int nc_id, /* I [idx] netCDF file ID */
 const int dmn_id, /* I [idx] ID of dimension for this limit structure */
 const lmt_sct * const lmt, /* I [sct] Array of limit structures from lmt_evl() */
 int lmt_nbr, /* I [nbr] Number of limit structures */
 const bool FORTRAN_STYLE); /* I [flg] Hyperslab indices obey Fortran convention */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_LMT_H */
