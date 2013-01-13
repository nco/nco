/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_att_utl.h,v 1.45 2013-01-13 06:07:47 zender Exp $ */

/* Purpose: Attribute utilities */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_att_utl.h" *//* Attribute utilities */

#ifndef NCO_ATT_UTL_H
#define NCO_ATT_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
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
#include "nco_mss_val.h" /* Missing value utilities */
#include "nco_scm.h" /* Software configuration management */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
nco_aed_prc /* [fnc] Process single attribute edit for single variable */
(const int nc_id, /* I [id] Input netCDF file ID */
 const int var_id, /* I [id] ID of variable on which to perform attribute editing */
 const aed_sct aed); /* I [sct] Structure containing information necessary to edit */

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
nco_mpi_att_cat /* [fnc] Add MPI tasks global attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const int mpi_nbr); /* I [nbr] MPI nodes/tasks number */

aed_sct * /* O [sct] List of attribute edit structures */
nco_prs_aed_lst /* [fnc] Parse user-specified attribute edits into structure list */
(const int nbr_aed, /* I [nbr] Number of attributes in list */
 X_CST_PTR_CST_PTR_Y(char,aed_arg)); /* I/O [sng] List of user-specified attribute edits (delimiters are changed to NULL on output */

int /* [flg] Variable and attribute names are conjoined */
nco_prs_att /* [fnc] Parse conjoined variable and attribute names */
(rnm_sct * const rnm_att, /* I/O [sct] Structure [Variable:]Attribute name on input, Attribute name on output */
 char * const var_nm, /* O [sng] Variable name, if any */
 nco_bool * const IS_GLB_GRP_ATT); /* O [flg] Attribute is Global or Group attribute */

gpe_sct * /* O [sng] GPE structure */
nco_gpe_prs_arg /* [fnc] Parse Group Path Editing (GPE) argument */
(const char * const gpe_arg); /* I [sng] User-specified GPE specification */

char * /* O [sng] Result of applying GPE to input path */
nco_gpe_evl /* [fnc] Apply Group Path Editing (GPE) to argument */
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
nco_thr_att_cat /* [fnc] Add threading global attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const int thr_nbr); /* I [nbr] Thread number */

void 
nco_vrs_att_cat /* [fnc] Add NCO version global attribute */
(const int out_id); /* I [id] netCDF output-file ID */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_ATT_UTL_H */
