/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.h,v 1.88 2013-07-24 04:21:30 pvicente Exp $ */

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
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_mmr.h" /* Memory management */

void                         
trv_tbl_init                          /* [fnc] GTT initialize */
(trv_tbl_sct **tbl);                  /* I/O [sct] Traversal table */
/* end trv_tbl_init */

void 
trv_tbl_free                          /* [fnc] GTT free memory */
(trv_tbl_sct *tbl);                   /* I/O [sct] Traversal table */
/* end trv_tbl_free */

void                       
trv_tbl_inq                            /* [fnc] Find and return global totals of dimensions, variables, attributes */
(int * const att_glb_all,              /* O [nbr] Number of global attributes in file */
 int * const att_grp_all,              /* O [nbr] Number of group attributes in file */
 int * const att_var_all,              /* O [nbr] Number of variable attributes in file */
 int * const dmn_nbr_all,              /* O [nbr] Number of dimensions in file */
 int * const dmn_rec_all,              /* O [nbr] Number of record dimensions in file */
 int * const grp_dpt_all,              /* O [nbr] Maximum group depth (root = 0) */
 int * const grp_nbr_all,              /* O [nbr] Number of groups in file */
 int * const var_ntm_all,              /* O [nbr] Number of non-atomic variables in file */
 int * const var_tmc_all,              /* O [nbr] Number of atomic-type variables in file */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void 
trv_tbl_prn_flg_mch                    /* [fnc] Print table items that have .flg_mch  */
(const trv_tbl_sct * const trv_tbl,    /* I [sct] Traversal table */
 const nco_obj_typ obj_typ);           /* I [enm] Object type (group or variable) */

void                          
trv_tbl_prn                            /* [fnc] Print table with -z */
(const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */  

nco_bool
trv_tbl_fnd_var_nm_fll                 /* [fnc] Check if "var_nm_fll" is in table */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

trv_sct *                              /* O [sct] Table object */
trv_tbl_var_nm_fll                     /* [fnc] Return object from full name key */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void
trv_tbl_mrk_xtr                        /* [fnc] Mark extraction flag in table for "var_nm_fll" */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void
trv_tbl_mrk_prc_fix                    /* [fnc] Mark fixed/processed flag in table for "var_nm_fll" */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 prc_typ_enm typ_prc,                  /* I [enm] Processing type */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */


void 
trv_tbl_prn_xtr                        /* [fnc] Print extraction flag of traversal table */
(const trv_tbl_sct * const trv_tbl,    /* I [sct] Traversal table */
 const char * const fnc_nm);           /* I [sng] Function name of the calling function */

void 
trv_tbl_srt                            /* [fnc] Sort traversal table */
(trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

int                                    /* O [nbr] Number of depth 1 groups (root = 0) */
trv_tbl_inq_dpt                        /* [fnc] Return number of depth 1 groups */
(const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

void                          
trv_tbl_mch                            /* [fnc] Match 2 tables (find common objects) and export common objects  */
(trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 nco_cmn_t **cmn_lst,                  /* I/O [sct] List of common names */
 int * nbr_cmn_nm);                    /* I/O [nbr] Number of common names entries */

void                          
trv_tbl_cmn_nm_prt                     /* [fnc] Print list of common objects (same absolute path) */
(const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm);                /* I [nbr] Number of common names entries */


dmn_trv_sct *                         /* O [sct] GTT dimension structure (stored in *groups*) */
nco_dmn_trv_sct                       /* [fnc] Return unique dimension object from unique ID */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

char *                                /* O [id] Unique dimension full name */
nco_dmn_fll_nm_id                     /* [fnc] Return unique dimension full name from unique ID  */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */


void                                  
nco_dmn_set_msa                       /* [fnc] Update dimension with hyperslabed size */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const long dmn_cnt,                  /* I [nbr] New dimension size */
 const trv_tbl_sct *trv_tbl);         /* I/O [sct] GTT (Group Traversal Table) */

long                                  /* O hyperslabed size */
nco_dmn_get_msa                       /* [fnc] Update dimension with hyperslabed size */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const trv_tbl_sct *trv_tbl);         /* I/O [sct] GTT (Group Traversal Table) */


#endif /* NCO_GRP_TRV_H */
