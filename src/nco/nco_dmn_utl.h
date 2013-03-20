/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_dmn_utl.h,v 1.42 2013-03-20 14:24:08 pvicente Exp $ */

/* Purpose: Dimension utilities */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_dmn_utl.h" *//* Dimension utilities */

#ifndef NCO_DMN_UTL_H
#define NCO_DMN_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_grp_utl.h"  /* Group utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
nco_dmn_dfn /* [fnc] Define dimensions in output file */
(const char * const fl_nm, /* I [sng] Name of output file */
 const int nc_id, /* I [id] netCDF output file ID */
 dmn_sct ** const dmn, /* I [sct] Dimension structures to define in output file */
 const int nbr_dmn); /* I [nbr] Number of dimension structures in structure list */

dmn_sct * /* O [sct] copy of input dimension structure */
nco_dmn_dpl /* [fnc] Duplicate input dimension structure */
(const dmn_sct * const dmn); /* I [sct] Dimension structure to duplicate */


void 
nco_dmn_cpy /* [fnc] Copy dim elements from dim from source to target */
(dmn_sct *const dmn_t, /* O [sct] Dimension structure to copy into */
 const dmn_sct * const dmn_s); 


dmn_sct * /* O [sct] Output dimension structure */
nco_dmn_fll /* [fnc] Create and return completed dmn_sct */
(const int nc_id, /* I [id] netCDF input file ID*/
 const int dmn_id, /* I [id] Dimension ID */
 const char * const dmn_nm); /* I [sng] Dimension name */

dmn_sct * /* O [sct] Pointer to free'd dimension */
nco_dmn_free /* [fnc] Free all memory associated with dimension structure */
(dmn_sct *dmn); /* I/O [sct] Dimension to free */

void
nco_dmn_cnk_mrg /* [fnc] Merge chunking information into dimension structures */
(dmn_sct ** const dmn, /* I [sct] Dimension structures to modify */
 const int nbr_dmn, /* I [nbr] Number of dimension structures in structure list */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr); /* I [nbr] Number of dimensions with user-specified chunking */

void
nco_dmn_lmt_mrg /* [fnc] Merge limit structure information into dimension structures */
(dmn_sct ** const dmn, /* I [sct] Dimension structures to modify */
 const int nbr_dmn, /* I [nbr] Number of dimension structures in structure list */
 CST_X_PTR_CST_PTR_CST_Y(lmt_sct,lmt), /* I [sct] Dimension limit information */
 const int lmt_nbr); /* I [nbr] Number of dimensions with user-specified limits */


void
nco_dmn_lmt_all_mrg /* [fnc] Merge limit structure information into dimension structures */
(dmn_sct ** const dmn, /* I [sct] Dimension structures to modify */
 const int nbr_dmn, /* I [nbr] Number of dimension structures in structure list */
 CST_X_PTR_CST_PTR_CST_Y(lmt_msa_sct,lmt_all_lst), /* I [sct] Dimension limit information */
 const int lmt_nbr); /* I [nbr] Number of dimensions with user-specified limits */


dmn_sct ** /* O [sct] Pointer to free'd structure list */
nco_dmn_lst_free /* [fnc] Free memory associated with dimension structure list */
(dmn_sct **dmn_lst, /* I/O [sct] Dimension structure list to free */
 const int dmn_nbr); /* I [nbr] Number of dimension structures in list */

nm_id_sct * /* O [sct] Dimension list */
nco_dmn_lst_mk /* [fnc] Attach dimension IDs to dimension list */
(const int nc_id, /* I [id] netCDF file ID */
 CST_X_PTR_CST_PTR_CST_Y(char,dmn_lst_in), /* I [sng] User-specified list of dimension names */
 const int nbr_dmn); /* I [nbr] Total number of dimensions in list */

void
nco_dmn_xrf  /* [fnc] Crossreference xrf elements of dimension structures */
(dmn_sct * const dmn_1, /* I/O [sct] Dimension structure */
 dmn_sct * const dmn_2); /* I/O [sct] Dimension structure */

int /* O [flg] Dimension exists in scope of group (if rcd != NC_NOERR) */
nco_inq_dmn_grp_id /* [fnc] Return location and ID of named dimension in specified group */
(const int nc_id, /* I [id] netCDF group ID */
 const char * const dmn_nm, /* I [sng] Dimension name */
 int * const dmn_id, /* O [id] Dimension ID in specified group */
 int * const grp_id_dmn); /* O [id] Group ID where dimension visible to specified group is defined */

nm_id_sct * /* O [sct] List of dimensions associated with input variable list */
nco_dmn_lst_ass_var /* [fnc] Create list of all dimensions associated with input variable list */
(const int nc_id, /* I [id] netCDF input-file ID */
 const nm_id_sct * const var, /* I [sct] Variable list */
 const int nbr_var, /* I [nbr] Number of variables in list */
 int * const nbr_dmn); /* O [nbr] Number of dimensions associated with input variable list */

void 
nco_dmn_sct_cmp /* [fnc] Check that dims in list 2 are a subset of list 1 and that they are the same size */
(dmn_sct ** const dim_1n, /* I [sct] Dimension list 1 */
 const int nbr_dmn_1, /* I [nbr] Number of dimension structures in structure list */
 dmn_sct **const dim_2, /* I [sct] Dimension list 1 */
 const int nbr_dmn_2, /* I [nbr] Number of dimension structures in structure list */
 const char *const fl_sng_1, /* I [sng] Name of first file */
 const char *fl_sng_2); /* I [sng] Name of second file */

void
nco_dmn_ass_var_prt                  /* [fnc] Print list of all dimensions associated with input variable list */
(trv_tbl_sct * const trv_tbl);       /* I/O [sct] GTT (Group Traversal Table) */

void
nco_dmn_ass_var_trv                   /* [fnc] Create list of all dimensions associated with input variable list */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

void 
nco_dmn_sct_cmp_trv                    /* [fnc] Check that dims in list 2 are a subset of list 1 and that they are the same size */
(const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2,  /* I [sct] GTT (Group Traversal Table) */
 const char *const fl_sng_1,           /* I [sng] Name of first file */
 const char *const fl_sng_2);          /* I [sng] Name of second file */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_DMN_UTL_H */
