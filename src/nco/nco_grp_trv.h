/* $Header$ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_grp_trv.h" *//* Group traversal */

#ifndef NCO_GRP_TRV_H
#define NCO_GRP_TRV_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_lst_utl.h" /* List utilities */
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
 int * const var_udt_all,              /* O [nbr] Number of non-atomic variables in file */
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
trv_tbl_var_nm                         /* [fnc] Return variable object (relative name) */
(const char * const var_nm,            /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

trv_sct *                              /* O [sct] Table object */
trv_tbl_var_nm_fll                     /* [fnc] Return object from full name key */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

trv_sct *                              /* O [sct] Table object */
trv_tbl_grp_nm_fll                     /* [fnc] Return group object from full name key */
(const char * const grp_nm_fll,        /* I [sng] Group name to find */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void
trv_tbl_mrk_xtr                        /* [fnc] Mark extraction flag in table for "var_nm_fll" */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 const nco_bool flg_xtr,               /* I [flg] Flag (True or False) */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

void
trv_tbl_mrk_grp_xtr                   /* [fnc] Mark extraction flag in table for "grp_nm_fll" */
(const char * const grp_nm_fll,       /* I [sng] Group name to find */
 const nco_bool flg_xtr,              /* I [flg] Flag (True or False) */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
trv_tbl_mrk_prc_fix                    /* [fnc] Mark fixed/processed flag in table for "var_nm_fll" */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 prc_typ_enm typ_prc,                  /* I [enm] Processing type */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

void 
trv_tbl_prn_xtr                        /* [fnc] Print extraction flag of traversal table */
(const trv_tbl_sct * const trv_tbl,    /* I [sct] Traversal table */
 const char * const fnc_nm);           /* I [sng] Function name of the calling function */

int                                    /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
trv_tbl_cmp_asc_nm_fll                 /* [fnc] Compare two trv_sct's by full name member, return ascending order */
(const void *val_1,                    /* I [sct] trv_sct to compare */
 const void *val_2);                   /* I [sct] trv_sct to compare */

int                                    /* O [enm] Comparison result [<,=,>] 0 iff val_1 [>,==,<] val_2 */
trv_tbl_cmp_dsc_nm_fll                 /* [fnc] Compare two trv_sct's by full name member, return descending order */
(const void *val_1,                    /* I [sct] trv_sct to compare */
 const void *val_2);                   /* I [sct] trv_sct to compare */

void 
trv_tbl_srt                            /* [fnc] Sort traversal table */
(const int srt_mth,                    /* [enm] Sort method */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

int                                    /* O [nbr] Number of depth 1 groups (root = 0) */
trv_tbl_inq_dpt                        /* [fnc] Return number of depth 1 groups */
(const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

void                          
trv_tbl_mch                            /* [fnc] Match 2 tables (find common objects) and export common objects  */
(const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2,  /* I [sct] GTT (Group Traversal Table) */
 nco_cmn_t **cmn_lst,                  /* I/O [sct] List of common names */
 int * nbr_cmn_nm);                    /* I/O [nbr] Number of common names entries */

void 
trv_tbl_prn_flg_xtr                   /* [fnc] Print table items that have .flg_xtr  */
(const char * const fnc_nm,           /* I [sng] Function name  */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void
trv_tbl_prn_dbg                       /* [fnc] Print several table members fields (debug only) */
(const char * const fnc_nm,           /* I [sng] Function name  */
const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void                          
trv_tbl_cmn_nm_prt                     /* [fnc] Print list of common objects (same absolute path) */
(const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm);                /* I [nbr] Number of common names entries */

dmn_trv_sct *                         /* O [sct] GTT dimension structure (stored in *groups*) */
nco_dmn_trv_sct                       /* [fnc] Return unique dimension object from unique ID */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void                                  
nco_dmn_set_msa                       /* [fnc] Update dimension with hyperslabbed size */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const long dmn_cnt,                  /* I [nbr] New dimension size */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

void 
nco_trv_hsh_bld                       /* Hash traversal table for fastest access */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_trv_hsh_del                       /* Delete hash table */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void 
nco_nm_srt                             /* [fnc] Sort traversal table */
(char **nm_lst,                        /* I [sng] List of names */
 const int nm_lst_nbr);                /* I [nbr] Number of items in list */

void                          
nco_nm_mch                             /* [fnc] Match 2 lists of strings and mark common strings  */
(char **nm_lst_1,                      /* I [sng] List of names */
 const int nm_lst_1_nbr,               /* I [nbr] Number of items in list */
 char **nm_lst_2,                      /* I [sng] List of names */
 const int nm_lst_2_nbr,               /* I [nbr] Number of items in list */
 nco_cmn_t **cmn_lst,                  /* I/O [sct] List of all names */
 int * nbr_nm,                         /* I/O [nbr] Number of all names (size of above array) */
 int * nbr_cmn_nm);                    /* I/O [nbr] Number of common names */

void
trv_tbl_mrk_nsm_mbr                    /* [fnc] Mark ensemble member flag in table for "var_nm_fll" */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 const nco_bool flg_nsm_tpl,           /* I [flg] Variable is template member */
 const char * const grp_nm_fll_prn,    /* I [sng] Parent group full name (key for ensemble) */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

char *                                 /* O [sng] Full path  */
nco_bld_nm_fll                         /* [fnc] Utility function to build a full path */
(const char * const grp_nm_fll,        /* I [sng] Group full name */
 const char * const var_nm);           /* I [sng] Variable name */

nco_bool                              
nco_lst_ins                            /* [fnc] Utility function to detect inserted names in a name list */
(const char * const nm,                /* I [sng] A name to detect */
 const nco_cmn_t *cmn_lst,             /* I [sct] List of names   */
 const int nbr_nm);                    /* I [nbr] Number of names (size of above array) */

char *                                 /* O [sng] Full path with suffix */
nco_bld_nsm_sfx                        /* [fnc] Build ensemble suffix */
(const char * const grp_nm_fll_prn,    /* I [sng] Absolute group name of ensemble root */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

char *                                /* O [flg] Full variable name */
nco_gid_var_nm_2_var_nm_fll           /* [fnc] Construct full variable name from group ID and variable name */
(const int grp_id,                    /* I [ID] netCDF input group ID */
 const char * const var_nm);          /* I [sng] Variable name */

nco_bool                              
nco_is_fll_pth                         /* [fnc] Utility function to inquire if a string is a full path */
(const char * const str);              /* I [sng] A string to inquire */

trv_sct *                              /* O [sct] Table object */
trv_tbl_nsm_nm                         /* [fnc] Return variable object  */
(const char * const var_nm,            /* I [sng] Variable name (relative) to find */
 const char * const grp_nm_fll_prn,    /* I [sng] Ensemble parent group */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

trv_sct *                              /* O [sct] Table object */
trv_tbl_nsm_nm_att                     /* [fnc] Return variable object  */
(const char * const var_nm,            /* I [sng] Variable name (relative) to find */
 const char * const grp_nm_fll_prn,    /* I [sng] Ensemble parent group */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

#endif /* NCO_GRP_TRV_H */
