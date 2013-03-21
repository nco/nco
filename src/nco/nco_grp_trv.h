/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.h,v 1.39 2013-03-21 22:45:34 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_grp_trv.h" *//* Group traversal */

#ifndef NCO_GRP_TRV_H
#define NCO_GRP_TRV_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */

void                         
trv_tbl_init
(trv_tbl_sct **tbl);                 /* I/O [sct] Traversal table */
/* end trv_tbl_init */

void 
trv_tbl_free
(trv_tbl_sct *tbl);                  /* I/O [sct] Traversal table */
/* end trv_tbl_free */


void                       
trv_tbl_inq                          /* [fnc] Find and return global totals of dimensions, variables, attributes */
(int * const att_glb_all,            /* O [nbr] Number of global attributes in file */
 int * const att_grp_all,            /* O [nbr] Number of group attributes in file */
 int * const att_var_all,            /* O [nbr] Number of variable attributes in file */
 int * const dmn_nbr_all,            /* O [nbr] Number of dimensions in file */
 int * const dmn_rec_all,            /* O [nbr] Number of record dimensions in file */
 int * const grp_dpt_all,            /* O [nbr] Maximum group depth (root = 0) */
 int * const grp_nbr_all,            /* O [nbr] Number of groups in file */
 int * const var_ntm_all,            /* O [nbr] Number of non-atomic variables in file */
 int * const var_tmc_all,            /* O [nbr] Number of atomic-type variables in file */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

void 
trv_tbl_prn_flg_mch                  /* [fnc] Print table items that have .flg_mch  */
(const trv_tbl_sct * const trv_tbl,  /* I [sct] Traversal table */
 const nco_obj_typ obj_typ);         /* I [enm] Object type (group or variable) */

void                          
trv_tbl_prn                          /* [fnc] Print table with -z */
(const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */  

nco_bool
trv_tbl_fnd_var_nm_fll                /* [fnc] Check if "var_nm_fll" is in table */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */


void
trv_tbl_mrk_xtr                       /* [fnc] Mark extraction flag in table for "var_nm_fll" */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void 
trv_tbl_prn_xtr                      /* [fnc] Print extraction flag of traversal table */
(const trv_tbl_sct * const trv_tbl,  /* I [sct] Traversal table */
 const char * const fnc_nm);         /* I [sng] Function name of the calling function */

void 
trv_tbl_srt /* [fnc] Sort traversal table */
(trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_trv_tbl_nm_fll  /* [fnc] Compare two trv_sct's by full name member */
(const void *val_1, /* I [sct] trv_sct to compare */
 const void *val_2); /* I [sct] trv_sct to compare */

void                          
trv_tbl_mch                          /* [fnc] Match 2 tables (find common objects) and export common objects */
(trv_tbl_sct * const trv_tbl_1,      /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,      /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl);       /* O [sct] GTT (Group Traversal Table) */

void                          
trv_sct_new                          /* [fnc] Enter a new object to table */
(trv_sct *trv,                       /* I [sct] Object structure to enter in new table */
 trv_tbl_sct * const trv_tbl);       /* O [sct] GTT (Group Traversal Table) */


#endif /* NCO_GRP_TRV_H */
