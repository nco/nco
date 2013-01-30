/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.h,v 1.29 2013-01-30 11:44:45 pvicente Exp $ */

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
trv_tbl_add
(trv_sct const obj, /* I [sct] Object to store */
 trv_tbl_sct * const tbl); /* I/O [sct] Traversal table */
/* end trv_tbl_add */

void                       
trv_tbl_inq                          /* [fnc] Find and return global totals of dimensions, variables, attributes */
(int * const att_nbr_glb,            /* O [nbr] Number of global attributes in file */
 int * const dmn_nbr_all,            /* O [nbr] Number of dimensions in file */
 int * const grp_nbr_all,            /* O [nbr] Number of groups in file */
 int * const rec_nbr_all,            /* O [nbr] Number of record dimensions in file */
 int * const var_nbr_all,            /* O [nbr] Number of variables in file  */
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

nco_bool                              /* O [nbr] Item found or not */
trv_tbl_fnd_var_nm                    /* [fnc] Find a variable that matches parameter "var_nm" */
(const char * const var_nm,           /* I [sng] Variable (relative) name to find */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void
trv_tbl_mrk_xtr                       /* [fnc] Mark extraction flag in table for "var_nm_fll" */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void 
trv_tbl_prn_xtr                      /* [fnc] Print extraction flag of traversal table */
(const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

void 
trv_tbl_srt /* [fnc] Sort traversal table */
(trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_trv_tbl_nm_fll  /* [fnc] Compare two trv_sct's by full name member */
(const void *val_1, /* I [sct] trv_sct to compare */
 const void *val_2); /* I [sct] trv_sct to compare */

void                          
trv_tbl_mch                           /* [fnc] Match 2 tables (find common objects) */
(trv_tbl_sct * const trv_tbl_1,       /* I/O [sct] Traversal table 1 */  
 trv_tbl_sct * const trv_tbl_2);      /* I/O [sct] Traversal table 2 */  

void 
trv_tbl_add_dmn                       /* [fnc] Add a dimension object to table  */
(dmn_fll_sct const obj,               /* I [sct] Object to store */
 trv_tbl_sct * const tbl);            /* I/O [sct] Traversal table */

#endif /* NCO_GRP_TRV_H */
