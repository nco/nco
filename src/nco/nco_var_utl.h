/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_utl.h,v 1.59 2010-03-22 15:19:59 hmb Exp $ */

/* Purpose: Variable utilities */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_var_utl.h" *//* Variable utilities */

#ifndef NCO_VAR_UTL_H
#define NCO_VAR_UTL_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */
#include <limits.h> /* need LONG_MAX */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* !_OPENMP */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_att_utl.h" /* Attribute utilities */
#include "nco_bnr.h" /* Binary write utilities */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_mss_val.h" /* Missing value utilities */
#include "nco_pck.h" /* Packing and unpacking variables */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  int /* O [id] Output file variable ID */
  nco_cpy_var_dfn /* [fnc] Copy variable metadata from input to output file */
  (const int in_id, /* I [id] netCDF input file ID */
   const int out_id, /* I [id] netCDF output file ID */
   const char *rec_dmn_nm, /* I [sng] Output file record dimension name  */
   const char * const var_nm, /* I [sng] Input variable name */
   const int dfl_lvl); /* I [enm] Deflate level [0..9] */

  int /* O [id] Output file variable ID */
  nco_cpy_var_dfn_lmt /* Copy variable metadata from input to output file */
  (const int in_id, /* I [id] netCDF input file ID */
   const int out_id, /* I [id] netCDF output file ID */
   const char *rec_dmn_nm, /* I [sng] Output file record dimension name  */
   const char * const var_nm, /* I [sng] Input variable name */
   CST_X_PTR_CST_PTR_CST_Y(lmt_all_sct,lmt_all_lst), /* I [sct] Hyperslab limits */
   const int lmt_all_lst_nbr, /* I [nbr] Number of hyperslab limits */
   const int dfl_lvl); /* I [enm] Deflate level [0..9] */

  void
  nco_cpy_var_val /* [fnc] Copy variable data from input to output file, no limits */
  (const int in_id, /* I [id] netCDF input file ID */
   const int out_id, /* I [id] netCDF output file ID */
   FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
   const nco_bool NCO_BNR_WRT, /* I [flg] Write binary file */
   const char *var_nm); /* I [sng] Variable name */

  void
  nco_cpy_var_val_lmt /* [fnc] Copy variable data from input to output file, simple hyperslabs */
  (const int in_id, /* I [id] netCDF input file ID */
   const int out_id, /* I [id] netCDF output file ID */
   FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
   const nco_bool NCO_BNR_WRT, /* I [flg] Write binary file */
   char *var_nm, /* I [sng] Variable name */
   const lmt_sct * const lmt, /* I [sct] Hyperslab limits */
   const int lmt_nbr); /* I [nbr] Number of hyperslab limits */
 
  void 
  var_copy /* [fnc] Copy hyperslab variables of type var_typ from op1 to op2 */
  (const nc_type var_typ, /* I [enm] netCDF type */
   const long sz, /* I [nbr] Number of elements to copy */
   const ptr_unn op1, /* I [sct] Values to copy */
   ptr_unn op2); /* O [sct] Destination to copy values to */
  
  int /* O [enm] Return code */
  var_dfl_set /* [fnc] Set defaults for each member of variable structure */
  (var_sct * const var); /* I [sct] Variable strucutre to initialize to defaults */

  void
  nco_var_dfn /* [fnc] Define variables and write their attributes to output file */
  (const int in_id, /* I [enm] netCDF input-file ID */
   const char * const fl_out, /* I [sng] Name of output file */
   const int out_id, /* I [enm] netCDF output-file ID */
   var_sct * const * const var, /* I/O [sct] Variables to be defined in output file */
   const int nbr_var, /* I [nbr] Number of variables to be defined */
   CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_ncl), /* I [sct] Dimensions included in output file */
   const int nbr_dmn_ncl, /* I [nbr] Number of dimensions in list */
   const int nco_pck_map, /* I [enm] Packing map */
   const int nco_pck_plc, /* I [enm] Packing policy */
   const int dfl_lvl); /* I [enm] Deflate level [0..9] */
  
  var_sct * /* O [sct] Copy of input variable */
  nco_var_dpl /* [fnc] Duplicate input variable */
  (const var_sct * const var); /* I [sct] Variable to duplicate */
  
  var_sct * /* O [sct] Variable structure */
  nco_var_fll /* [fnc] Allocate variable structure and fill with metadata */
  (const int nc_id, /* I [id] netCDF file ID */
   const int var_id, /* I [id] variable ID */
   const char * const var_nm, /* I [sng] Variable name */
   dmn_sct * const * const dim, /* I [sct] Dimensions available to variable */
   const int nbr_dim); /* I [nbr] Number of dimensions in list */
  
  var_sct * /* O [sct] Pointer to free'd variable */
  nco_var_free /* [fnc] Free all memory associated with variable structure */
  (var_sct *var); /* I/O [sct] Variable to free */
  
  void
  nco_var_get /* [fnc] Allocate, retrieve variable hyperslab from disk to memory */
  (const int nc_id, /* I [id] netCDF file ID */
   var_sct *var); /* I [sct] Variable to get */
  
  var_sct ** /* O [sct] Pointer to free'd structure list */
  nco_var_lst_free /* [fnc] Free memory associated with variable structure list */
  (var_sct **var_lst, /* I/O [sct] Variable structure list to free */
   const int var_nbr); /* I [nbr] Number of variable structures in list */

  void
  nco_var_mtd_refresh /* [fnc] Update variable metadata (dmn_nbr, ID, mss_val, type) */
  (const int nc_id, /* I [id] netCDF input-file ID */
   var_sct * const var); /* I/O [sct] Variable to update */
  
  void
  nco_var_srd_srt_set /* [fnc] Assign zero-start and unity-stride vectors to variables */
  (var_sct ** const var, /* I [sct] Variables whose start and stride arrays to set */
   const int nbr_var); /* I [nbr] Number of structures in variable structure list */

  
  void
  nco_var_dmn_refresh /* [fnc] Refresh var hyperslab info with var->dim[] info */
  (var_sct ** const var, /* I [sct] Variables to refresh */
   const int nbr_var); /* I [nbr] Number of structures in variable structure list */
  
  void
  nco_var_val_cpy /* [fnc] Copy variables data from input to output file */
  (const int in_id, /* I [enm] netCDF file ID */
   const int out_id, /* I [enm] netCDF output file ID */
   var_sct ** const var, /* I/O [sct] Variables to copy to output file */
   const int nbr_var); /* I [nbr] Number of variables */
  
  void
  nco_xrf_dmn /* [fnc] Switch pointers to dimension structures so var->dim points to var->dim->xrf */
  (var_sct * const var); /* I [sct] Variable to manipulate */
  
  void
  nco_xrf_var /* [fnc] Make xrf elements of variable structures point to eachother */
  (var_sct * const var_1, /* I/O [sct] Variable */
   var_sct * const var_2); /* I/O [sct] Related variable */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_UTL_H */
