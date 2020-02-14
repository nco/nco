/* $Header$ */

/* Purpose: Variable list utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_var_lst.h" *//* Variable list utilities */

#ifndef NCO_VAR_LST_H
#define NCO_VAR_LST_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <assert.h> /* assert() */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */

#ifdef HAVE_REGEX_H 
/* 20120213: Linux GCC 4.6 man page says regex.h depends on (non-present) sys/types.h */
# ifdef MACOSX
#  include <sys/types.h> /*19950312: _res, 20040822: Mac OS X off_t required by regex.h */
# endif /* !MACOSX */
# include <regex.h> /* POSIX regular expressions library */
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
 const int var_nbr_all, /* I [nbr] Number of variables in input file */
 char * const * const var_lst_in, /* I [sng] User-specified list of variable names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 const nco_bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const var_xtr_nbr); /* I/O [nbr] Number of variables in current extraction list */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_xcl /* [fnc] Convert exclusion list to extraction list */
(const int nc_id, /* I netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current exclusion list (destroyed) */
 int * const xtr_nbr); /* I/O [nbr] Number of variables in exclusion/extraction list */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_crd_add /* [fnc] Add all coordinates to extraction list */
(const int nc_id, /* I [id] netCDF file ID */
 const int nbr_dim, /* I [nbr] Number of dimensions in input file */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
 int * const xtr_nbr, /* I/O [nbr] Number of variables in current extraction list */
 const cnv_sct * const cnv); /* I [flg] file obeys CCM/CCSM/CF conventions */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_crd_ass_add /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id, /* I netCDF file ID */
 nm_id_sct *xtr_lst, /* I/O current extraction list (destroyed) */
 int * const xtr_nbr, /* I/O number of variables in current extraction list */
 const cnv_sct * const cnv); /* I [flg] file obeys CCM/CCSM/CF conventions */

nm_id_sct * /* O [sct] List with coordinate excluded */
nco_var_lst_crd_xcl /* [fnc] Exclude given coordinates from extraction list */
(const int nc_id, /* I [id] netCDF file ID */
 const int dmn_id, /* I [id] Dimension ID of coordinate to remove from extraction list */
 nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
 int * const xtr_nbr); /* I/O [nbr] Number of variables in extraction list */

void
nco_var_lst_fix_rec_dvd /* [fnc] Divide extraction list into fixed and record data */
(const int nc_id, /* I [id] netCDF file ID */
 nm_id_sct *xtr_lst, /* I/O [sct] Extraction list (pointers to it are constructed) */
 const int xtr_nbr, /* I [nbr] Number of variables in extraction list */
 nm_id_sct ***fix_lst, /* O [sct] Fixed-length variables */
 int * const fix_nbr, /* O [nbr] Number of fixed-length variables */
 nm_id_sct ***rec_lst, /* O [sct] Record variables */
 int * const rec_nbr); /* O [nbr] Number of record variables */

void
nco_var_lst_convert /* [fnc] Make variable structure list from variable name ID list */
(const int nc_id, /* I [enm] netCDF file ID */
 nm_id_sct *xtr_lst, /* I [sct] Current extraction list (destroyed) */
 const int xtr_nbr, /* I [nbr] Number of variables in input file */
 dmn_sct * const * const dim, /* I [sct] Dimensions associated with input variable list */
 const int nbr_dmn_xtr, /* I [nbr] Number of dimensions in list  */
 var_sct *** const var_ptr, /* O [sct] Variable list (for input file) */
 var_sct *** const var_out_ptr); /* O [sct] Duplicate variable list (for output file) */

void
nco_var_lst_dvd /* [fnc] Divide input lists into output lists */
(var_sct * const * const var, /* I [sct] Variable list (input file) */
 var_sct * const * const var_out, /* I [sct] Variable list (output file) */
 const int nbr_var, /* I [nbr] Number of variables */
 const cnv_sct * const cnv, /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD, /* [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 const int nco_pck_map, /* I [enm] Packing map */
 const int nco_pck_plc, /* I [enm] Packing policy */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl), /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl, /* I [nbr] Number of altered dimensions */
 var_sct *** const var_fix_ptr, /* O [sct] Fixed-variables (input file) */
 var_sct *** const var_fix_out_ptr, /* O [sct] Fixed-variables (output file) */
 int * const nbr_var_fix, /* O [nbr] Number of fixed variables */
 var_sct *** const var_prc_ptr, /* O [sct] Processed-variables (input file) */
 var_sct *** const var_prc_out_ptr, /* O [sct] Processed-variables (output file) */
 int * const nbr_var_prc, /* O [nbr] Number of processed variables */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

int /* O [enm] Return code */
nco_var_lst_mrg /* [fnc] Merge two variable lists into same order */
(var_sct *** var_1_ptr, /* I/O [sct] Variable list 1 */
 var_sct *** var_2_ptr, /* I/O [sct] Variable list 2 */
 int * const var_nbr_1, /* I/O [nbr] Number of variables in list 1 */
 int * const var_nbr_2); /* I/O [nbr] Number of variables in list 2 */

void
nco_var_lst_dvd_ncbo                         /* [fnc] Divide input lists into output lists (ncbo only) */
(var_sct * const var,                        /* I [sct] Variable list (input file) */
 var_sct * const var_out,                    /* I [sct] Variable list (output file) */
 const cnv_sct * const cnv,                  /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,                 /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 const int nco_pck_map,                      /* I [enm] Packing map */
 const int nco_pck_plc,                      /* I [enm] Packing policy */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                      /* I [nbr] Number of altered dimensions */
 prc_typ_enm *prc);                          /* O [enm] Processing type */

nco_bool
nco_var_is_fix /* [fnc] Variable should be treated as a fixed variable */
(const char * const var_nm, /* I [sng] Variable name */
 const int nco_prg_id, /* I [enm] Program key */
 const int nco_pck_plc, /* I [enm] Packing policy */
 const cnv_sct * const cnv); /* I [sct] Convention structure */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_LST_H */
