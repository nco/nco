/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_lst.h,v 1.1 2002-05-05 21:21:14 zender Exp $ */

/* Purpose: Variable list utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_var_lst.h" *//* Variable list utilities */

#ifndef NCO_VAR_LST_H
#define NCO_VAR_LST_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

nm_id_sct * /* O [sct] Variable extraction list */
var_lst_mk /* [fnc] Create variable extraction list */
(const int nc_id, /* I [enm] netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 const char ** const var_lst_in, /* I [sng] User-specified list of variable names */
 const bool PROCESS_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const nbr_xtr); /* I/O [nbr] Number of variables in current extraction list */

nm_id_sct * /* O [sct] Extraction list */
var_lst_xcl /* [fnc] Convert exclusion list to extraction list */
(const int nc_id, /* I netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current exclusion list (destroyed) */
 int * const nbr_xtr); /* I/O [nbr] Number of variables in exclusion/extraction list */

nm_id_sct * /* O [sct] Extraction list */
var_lst_add_crd /* [fnc] Add all coordinates to extraction list */
(const int nc_id, /* I [id] netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 const int nbr_dim, /* I [nbr] Number of dimensions in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
 int * const nbr_xtr); /* I/O [nbr] Number of variables in current extraction list */

void
var_lst_convert /* [fnc] Make variable structure list from variable name ID list */
(const int nc_id, /* I [enm] netCDF file ID */
 nm_id_sct *xtr_lst, /* I [sct] Current extraction list (destroyed) */
 const int nbr_xtr, /* I [nbr] Number of variables in input file */
 const dmn_sct ** const dim, /* I [sct] Dimensions associated with input variable list */
 const int nbr_dmn_xtr, /* I [nbr] Number of dimensions in list  */
 var_sct *** const var_ptr, /* O [sct] Variable list (for input file) */
 var_sct *** const var_out_ptr); /* O [sct] Duplicate variable list (for output file) */

void
var_lst_divide /* [fnc] Divide input lists into output lists */
(const var_sct ** const var, /* I [sct] Variable list (input file) */
 const var_sct ** const var_out, /* I [sct] Variable list (output file) */
 const int nbr_var, /* I [nbr] Number of variables */
 const bool NCAR_CSM_FORMAT, /* I [flg] File adheres to NCAR CSM conventions */
 const dmn_sct ** const dmn_xcl, /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl, /* I [nbr] Number of excluded dimensions */
 const var_sct ***var_fix_ptr, /* O [sct] Fixed-variables (input file) */
 const var_sct ***var_fix_out_ptr, /* O [sct] Fixed-variables (output file) */
 int * const nbr_var_fix, /* O [nbr] Number of fixed variables */
 const var_sct ***var_prc_ptr, /* O [sct] Processed-variables (input file) */
 const var_sct ***var_prc_out_ptr, /* O [sct] Processed-variables (output file) */
 int * const nbr_var_prc); /* O [nbr] Number of processed variables */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_LST_H */
