/* $Header$ */

/* Purpose: Attribute utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_att_utl.h" *//* Attribute utilities */

#ifndef NCO_ATT_UTL_H
#define NCO_ATT_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 isfinite isinf isnan */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */
#ifdef HAVE_STRINGS_H
# include <strings.h> /* strcasecmp() */
#endif /* !HAVE_STRINGS_H */
#include <time.h> /* machine time */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */
#include "nco_mta.h" /* Multi-argument parsing */
#include "nco_mss_val.h" /* Missing value utilities */
#ifdef _MSC_VER
# include "nco_rth_flt.h" /* Float-precision arithmetic, MSVC macros */
#endif /* !_MSC_VER */
#include "nco_scm.h" /* Software configuration management */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

nco_bool /* [flg] Attribute was changed */
nco_aed_prc_wrp /* [fnc] Expand regular expressions then pass attribute edits to nco_aed_prc() */
(const int nc_id, /* I [id] Input netCDF file ID */
 const int var_id, /* I [id] ID of variable on which to perform attribute editing */
 const aed_sct aed); /* I [sct] Attribute-edit information */

nco_bool /* [flg] Attribute was altered */
nco_aed_prc /* [fnc] Process single attribute edit for single variable */
(const int nc_id, /* I [id] Input netCDF file ID */
 const int var_id, /* I [id] ID of variable on which to perform attribute editing */
 const aed_sct aed); /* I [sct] Attribute-edit information */

nco_bool /* [flg] Attribute was changed */
nco_aed_prc_glb /* [fnc] Process attributes in root group */
(const int nc_id, /* I [id] netCDF file ID */
 const aed_sct aed, /* I [sct] Attribute-edit information */
 const trv_tbl_sct * const trv_tbl); /* I [lst] Traversal table */ 

nco_bool /* [flg] Attribute was changed */
nco_aed_prc_grp /* [fnc] Process attributes in groups */
(const int nc_id, /* I [id] netCDF file ID */
 const aed_sct aed, /* I [sct] Attribute-edit information */
 const trv_tbl_sct * const trv_tbl); /* I [lst] Traversal table */ 

nco_bool /* [flg] Attribute was changed */
nco_aed_prc_var_all /* [fnc] Process attributes in all variables */
(const int nc_id, /* I [id] netCDF file ID */
 const aed_sct aed, /* I [sct] Attribute-edit information */
 const nco_bool flg_typ_mch, /* I [flg] Type-match attribute edits */
 const trv_tbl_sct * const trv_tbl); /* I [lst] Traversal table */ 

nco_bool /* [flg] Attribute was changed */
nco_aed_prc_var_xtr /* [fnc] Process attributes in variables that match table extraction flag */
(const int nc_id, /* I [id] netCDF file ID */
 const aed_sct aed, /* I [sct] Attribute-edit information */
 const trv_tbl_sct * const trv_tbl); /* I [lst] Traversal table */ 

nco_bool /* [flg] Attribute was changed */
nco_aed_prc_var_nm /* [fnc] Process attributes in variables that match input name */
(const int nc_id, /* I [id] netCDF file ID */
 const aed_sct aed, /* I [sct] Attribute-edit information */
 const trv_tbl_sct * const trv_tbl); /* I [lst] Traversal table */ 

void 
nco_att_cpy  /* [fnc] Copy attributes from input netCDF file to output netCDF file */
(const int in_id, /* I [id] netCDF input-file ID */
 const int out_id, /* I [id] netCDF output-file ID */
 const int var_in_id, /* I [id] netCDF input-variable ID */
 const int var_out_id, /* I [id] netCDF output-variable ID */
 const nco_bool PCK_ATT_CPY); /* I [flg] Copy attributes "scale_factor", "add_offset" */

void 
nco_fl_lst_att_cat /* [fnc] Add input file list global attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 CST_X_PTR_CST_PTR_CST_Y(char,fl_lst_in), /* I [sng] Input file list */
 const int fl_nbr); /* I [nbr] Number of files in input file list */

void 
nco_hst_att_cat /* [fnc] Add command line, date stamp to history attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const char * const hst_sng); /* I [sng] String to add to history attribute */

void 
nco_prv_att_cat /* [fnc] Add provenance (history contents) of appended file to provenance attribute */
(const char * const fl_in, /* I [sng] Name of input-file */
 const int in_id, /* I [id] netCDF input-file ID */
 const int out_id); /* I [id] netCDF output-file ID */

void 
nco_mpi_att_cat /* [fnc] Add MPI tasks global attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const int mpi_nbr); /* I [nbr] MPI nodes/tasks number */

aed_sct * /* O [sct] List of attribute edit structures */
nco_prs_aed_lst /* [fnc] Parse user-specified attribute edits into structure list */
(const int nbr_aed, /* I [nbr] Number of attributes in list */
 X_CST_PTR_CST_PTR_Y(char,aed_arg)); /* I/O [sng] List of user-specified attribute edits (delimiters are changed to NULL on output */

int /* [flg] Error code */
nco_prs_att /* [fnc] Parse conjoined object and attribute names */
(rnm_sct * const rnm_att, /* I/O [sct] Structure [Object@]Attribute name on input, Attribute name on output */
 char * const obj_nm, /* O [sng] Object name, if any */
 nco_bool *mch_grp_all, /* O [flg] Rename all group attributes */
 nco_bool *mch_grp_glb, /* O [flg] Rename only global attributes */
 nco_bool *mch_obj_all); /* O [flg] Rename all group and variable attributes */

gpe_sct * /* O [sng] GPE structure */
nco_gpe_prs_arg /* [fnc] Parse Group Path Editing (GPE) argument */
(const char * const gpe_arg); /* I [sng] User-specified GPE specification */

char * /* O [sng] Result of applying GPE to input path */
nco_gpe_evl /* [fnc] Apply Group Path Editing (GPE) to argument */
(const gpe_sct * const gpe, /* I [sng] GPE structure */
 const char * const grp_nm_fll_in); /* I [sng] Full group name */

char * /* O [sng] Stub of GPE applied to input path */
nco_gpe_evl_stb /* [fnc] Find stub of Group Path Editing (GPE) applied to argument */
(const gpe_sct * const gpe, /* I [sng] GPE structure */
 const char * const grp_nm_fll_in); /* I [sng] Full group name */

gpe_sct * /* O [sct] Structure with dynamic memory free()'d */
nco_gpe_free /* [fnc] Free dynamic memory of GPE structure */
(gpe_sct * const gpe); /* O [sct] GPE structure */

const char * /* O [sng] String describing GPE */
nco_gpe_sng /* [fnc] Convert GPE enum to string */
(const gpe_enm gpe_md); /* I [enm] GPE mode */

rnm_sct * /* O [sng] Structured list of old, new names */
nco_prs_rnm_lst /* [fnc] Set old_nm, new_nm elements of rename structure */
(const int nbr_rnm, /* I [nbr] Number of elements in rename list */
 char * const * const rnm_arg); /* I [sng] Unstructured list of old, new names */

void 
nco_glb_att_add /* [fnc] Add global attributes */
(const int out_id, /* I [id] netCDF output-file ID */
 char **gaa_arg,  /* [sng] Global attribute arguments */
 const int gaa_arg_nbr);  /* [nbr] Number of global attribute arguments */

void 
nco_thr_att_cat /* [fnc] Add threading global attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const int thr_nbr); /* I [nbr] Thread number */

void 
nco_vrs_att_cat /* [fnc] Add NCO version global attribute */
(const int out_id); /* I [id] netCDF output-file ID */

nco_bool /* [O] Perform exception processing on this variable */
nco_is_xcp /* [fnc] Determine whether to perform exception processing */
(const char * const var_nm); /* [sng] Variable name */

void
nco_xcp_prc /* [fnc] Perform exception processing on this variable */
(const char * const var_nm, /* [sng] Variable name */
 const nc_type var_typ, /* I [enm] netCDF type of operand */
 const long var_sz, /* I [nbr] Size (in elements) of operand */
 char * const var_val); /* I/O [sng] Values of operand */

char * /* O [sng] Attribute value */
nco_char_att_get /* [fnc] Get a character string attribute from an open file */
(const int in_id, /* I [id] netCDF input-file ID */
 const int var_id, /* I [id] netCDF variable ID */
 const char * const att_nm); /* [sng] Attribute name */

int /* O [rcd] Return code */
nco_char_att_put /* [fnc] Get a character string attribute from an open file */
(const int out_id, /* I [id] netCDF output-file ID */
 const char * const var_nm_sng, /* [sng] Variable name */
 const char * const att_nm_sng, /* [sng] Attribute name */
 const char * const att_val_sng); /* [sng] Attribute value */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_ATT_UTL_H */
