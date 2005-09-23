/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_lst.h,v 1.30 2005-09-23 19:30:40 zender Exp $ */

/* Purpose: Variable list utilities */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_var_lst.h" *//* Variable list utilities */

#ifndef NCO_VAR_LST_H
#define NCO_VAR_LST_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */

#ifdef HAVE_REGEX_H
#ifdef MACOSX
#include <sys/types.h> /* 20040822: Provide off_t required by Mac OS X regex.h */
#endif /* !MACOSX */
#include <regex.h> /* POSIX regular expressions library */
#endif /* HAVE_REGEX_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */
#include "nco_var_utl.h" /* Variable utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

nm_id_sct * /* O [sct] Variable extraction list */
nco_var_lst_mk /* [fnc] Create variable extraction list */
(const int nc_id, /* I [enm] netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 CST_X_PTR_CST_PTR_CST_Y(char,var_lst_in), /* I [sng] User-specified list of variable names and rx's */
 const bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const nbr_xtr); /* I/O [nbr] Number of variables in current extraction list */

nm_id_sct * /* O [sct] Variable extraction list */
nco_var_lst_mk_old /* [fnc] Create variable extraction list */
(const int nc_id, /* I [enm] netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 CST_X_PTR_CST_PTR_CST_Y(char,var_lst_in), /* I [sng] User-specified list of variable names */
 const bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const nbr_xtr); /* I/O [nbr] Number of variables in current extraction list */

int /* O [nbr] Number of matches found */
nco_var_meta_search /* [fnc] Search for pattern matches in var string list */
(int nbr_var, /* I [nbr] number of vars in srch_sng and size of in_bool */
 nm_id_sct *in_lst, /* I [sct] List of all variables in input file (with IDs) */
 char *rexp, /* I [sng] Regular expression pattern */
 bool *in_bool); /* O [flg] Matched vars holder */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_xcl /* [fnc] Convert exclusion list to extraction list */
(const int nc_id, /* I netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current exclusion list (destroyed) */
 int * const nbr_xtr); /* I/O [nbr] Number of variables in exclusion/extraction list */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_add_crd /* [fnc] Add all coordinates to extraction list */
(const int nc_id, /* I [id] netCDF file ID */
 const int nbr_dim, /* I [nbr] Number of dimensions in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
 int * const nbr_xtr); /* I/O [nbr] Number of variables in current extraction list */

void
var_lst_convert /* [fnc] Make variable structure list from variable name ID list */
(const int nc_id, /* I [enm] netCDF file ID */
 nm_id_sct *xtr_lst, /* I [sct] Current extraction list (destroyed) */
 const int nbr_xtr, /* I [nbr] Number of variables in input file */
 dmn_sct * const * const dim, /* I [sct] Dimensions associated with input variable list */
 const int nbr_dmn_xtr, /* I [nbr] Number of dimensions in list  */
 var_sct *** const var_ptr, /* O [sct] Variable list (for input file) */
 var_sct *** const var_out_ptr); /* O [sct] Duplicate variable list (for output file) */

void
nco_var_lst_dvd /* [fnc] Divide input lists into output lists */
(var_sct * const * const var, /* I [sct] Variable list (input file) */
 var_sct * const * const var_out, /* I [sct] Variable list (output file) */
 const int nbr_var, /* I [nbr] Number of variables */
 const bool CNV_CCM_CCSM_CF, /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_pck_map, /* I [enm] Packing map */
 const int nco_pck_plc, /* I [enm] Packing policy */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl), /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl, /* I [nbr] Number of altered dimensions */
 var_sct *** const var_fix_ptr, /* O [sct] Fixed-variables (input file) */
 var_sct *** const var_fix_out_ptr, /* O [sct] Fixed-variables (output file) */
 int * const nbr_var_fix, /* O [nbr] Number of fixed variables */
 var_sct *** const var_prc_ptr, /* O [sct] Processed-variables (input file) */
 var_sct *** const var_prc_out_ptr, /* O [sct] Processed-variables (output file) */
 int * const nbr_var_prc); /* O [nbr] Number of processed variables */

int /* O [enm] Return code */
nco_var_lst_mrg /* [fnc] Merge two variable lists into same order */
(var_sct *** var_1_ptr, /* I/O [sct] Variable list 1 */
 var_sct *** var_2_ptr, /* I/O [sct] Variable list 2 */
 int * const var_nbr_1, /* I/O [nbr] Number of variables in list 1 */
 int * const var_nbr_2); /* I/O [nbr] Number of variables in list 2 */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_LST_H */
