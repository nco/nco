/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_lmt.h,v 1.19 2005-01-07 23:54:57 zender Exp $ */

/* Purpose: Hyperslab limits */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_lmt.h" *//* Hyperslab limits */

#ifndef NCO_LMT_H
#define NCO_LMT_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */
#ifdef ENABLE_UDUNITS
#include <udunits.h> /* Unidata units library */
#endif /* !ENABLE_UDUNITS */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */

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
 bool FORTRAN_IDX_CNV); /* I [flg] Hyperslab indices obey Fortran convention */

lmt_sct * /* O [sct] Structure with user-specified strings for min and max limits */
nco_lmt_prs /* [fnc] Create limit structures with name, min_sng, max_sng elements */
(const int lmt_nbr, /* I [nbr] number of dimensions with limits */
 char * const * const lmt_arg); /* I [sng] list of user-specified dimension limits */

lmt_sct /* [sct] Limit structure for dimension */
nco_lmt_sct_mk /* [fnc] Create stand-alone limit structure for given dimension */
(const int nc_id, /* I [idx] netCDF file ID */
 const int dmn_id, /* I [idx] ID of dimension for this limit structure */
 const lmt_sct * const lmt, /* I [sct] Array of limit structures from nco_lmt_evl() */
 int lmt_nbr, /* I [nbr] Number of limit structures */
 const bool FORTRAN_IDX_CNV); /* I [flg] Hyperslab indices obey Fortran convention */

int /* [rcd] Successful conversion returns 0 */
nco_lmt_udu_cnv /* [fnc] Convert from Unidata units to coordinate value */
(const int nc_id, /* I [idx] netCDF file ID */
 const int dmn_id, /* I [idx] netCDF dimension ID */
 char *lmt_sng, /* I [ptr] Limit string */
 double *lmt_val); /* O [val] Limit coordinate value */ 

int /* 0 [enum] returns the limit type */
nco_lmt_typ /* [fnc] determine limit type */
(char* sng);/* I [ptr] pointer to the limit string */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_LMT_H */
