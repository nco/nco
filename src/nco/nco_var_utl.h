/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_utl.h,v 1.25 2004-09-07 04:16:36 zender Exp $ */

/* Purpose: Variable utilities */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

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
#endif /* not _OPENMP */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_att_utl.h" /* Attribute utilities */
#include "nco_bnr.h" /* Binary write utilities */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_pck.h" /* Packing and unpacking variables */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  int /* O [id] Output file variable ID */
  nco_cpy_var_dfn /* [fnc] Copy variable metadata from input to output file */
  (const int in_id, /* I [id] netCDF input file ID */
   const int out_id, /* I [id] netCDF output file ID */
   const int rec_dmn_id, /* I [id] Input file record dimension ID  */
   const char * const var_nm); /* I [sng] Input variable name */

  int /* O [id] Output file variable ID */
  nco_cpy_var_dfn_lmt /* Copy variable metadata from input to output file */
  (const int in_id, /* I [id] netCDF input file ID */
   const int out_id, /* I [id] netCDF output file ID */
   const int rec_dmn_id, /* I [id] Input file record dimension ID  */
   const char * const var_nm, /* I [sng] Input variable name */
   const lmt_all_sct * const lmt_lst, /* I [sct] Hyperslab limits */
   const int lmt_lst_nbr); /* I [nbr] Number of hyperslab limits */

  void
  nco_cpy_var_val /* [fnc] Copy variable data from input to output file, no limits */
  (int in_id, /* I [id] netCDF input file ID */
   int out_id, /* I [id] netCDF output file ID */
   FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
   const bool NCO_BNR_WRT, /* I [flg] Write binary file */
   char *var_nm); /* I [sng] Variable name */

  void
  nco_cpy_var_val_lmt /* [fnc] Copy variable data from input to output file, simple hyperslabs */
  (const int in_id, /* I [id] netCDF input file ID */
   const int out_id, /* I [id] netCDF output file ID */
   FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
   const bool NCO_BNR_WRT, /* I [flg] Write binary file */
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
   const int nco_pck_typ); /* I [enm] Packing type */
  
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
  (var_sct *var); /* I [sct] Variable to free */
  
  void
  nco_var_get /* [fnc] Allocate, retrieve variable hyperslab from disk to memory */
  (const int nc_id, /* I [id] netCDF file ID */
   var_sct *var); /* I [sct] Variable to get */
  
  nm_id_sct * /* O [sct] List with coordinate excluded */
  nco_var_lst_crd_xcl /* [fnc] Exclude given coordinates from extraction list */
  (const int nc_id, /* I [id] netCDF file ID */
   const int dmn_id, /* I [id] Dimension ID of coordinate to remove from extraction list */
   nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
   int * const nbr_xtr); /* I/O [nbr] Number of variables in extraction list */
  
  nm_id_sct * /* O [sct] Extraction list */
  nco_var_lst_ass_crd_add /* [fnc] Add coordinates associated extracted variables to extraction list */
  (const int nc_id, /* I netCDF file ID */
   nm_id_sct *xtr_lst, /* I/O current extraction list (destroyed) */
   int * const nbr_xtr); /* I/O number of variables in current extraction list */
  
  void
  nco_var_refresh /* [fnc] Update variable metadata (var ID, dmn_nbr, mss_val) */
  (const int nc_id, /* I [id] netCDF input-file ID */
   var_sct * const var); /* I/O [sct] Variable to update */
  
  void
  nco_var_srt_zero /* [fnc] Zero srt array of variable structure */
  (var_sct ** const var, /* I [sct] Variables whose srt arrays will be zeroed */
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
