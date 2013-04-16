/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.h,v 1.269 2013-04-16 20:57:49 pvicente Exp $ */

/* Purpose: Group utilities */

/* Copyright (C) 2011--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_var_utl.h" *//* Group utilities */

#ifndef NCO_GRP_UTL_H
#define NCO_GRP_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <assert.h> /* assert() */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_cnk.h" /* Chunking */
#include "nco_grp_trv.h" /* Group traversal */
#include "nco_mmr.h" /* Memory management */
#include "nco_msa.h" /* Multi-slabbing algorithm */
#include "nco_var_lst.h" /* Variable list utilities */

/* Dynamic array implementation of group stack */
typedef struct {
  int grp_nbr; /* [nbr] Number of items in stack = number of elements in grp_id array */
  int *grp_id; /* [ID] Group ID */
} grp_stk_sct; 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

nm_id_sct *                           /* O [sct] Extraction list */  
nco_trv_tbl_nm_id                     /* [fnc] Create extraction list of nm_id_sct from traversal table */
(const int nc_id,                     /* I [id] netCDF file ID */
 int * const xtr_nbr,                 /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void
nco_flg_set_grp_var_ass               /* [fnc] Set flags for groups and variables associated with matched object */
(const char * const grp_nm_fll,       /* I [sng] Full name of group */
 const nco_obj_typ obj_typ,           /* I [enm] Object type (group or variable) */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */
 
int                                   /* O [rcd] Return code */
nco_def_grp_full                      /* [fnc] Ensure all components of group path are defined */
(const int nc_id,                     /* I [ID] netCDF output-file ID */
 const char * const grp_nm_fll,       /* I [sng] Full group name */
 int * const grp_out_id);             /* O [ID] Deepest group ID */

int                                   /* [rcd] Return code */
nco_inq_grps_full                     /* [fnc] Discover and return IDs of apex and all sub-groups */
(const int grp_id,                    /* I [ID] Apex group */
 int * const grp_nbr,                 /* O [nbr] Number of groups */
 int * const grp_ids);                /* O [ID] Group IDs of children */

void
nco_grp_itr_free                      /* [fnc] Free group iterator */
(grp_stk_sct * const grp_stk);        /* O [sct] Group stack pointer */

int                                   /* [rcd] Return code */
nco_grp_stk_get                       /* [fnc] Initialize and obtain group iterator */
(const int grp_id,                    /* I [ID] Apex group */
 grp_stk_sct ** const grp_stk);       /* O [sct] Group stack pointer */

int                                   /* [rcd] Return code */
nco_grp_stk_nxt                       /* [fnc] Find and return next group ID */
(grp_stk_sct * const grp_stk,         /* O [sct] Group stack pointer */
 int * const grp_id);                 /* O [ID] Group ID */
   
grp_stk_sct *                        /* O [sct] Group stack pointer */
nco_grp_stk_ntl                      /* [fnc] Initialize group stack */
(void); 

void
nco_grp_stk_psh                      /* [fnc] Push group ID onto stack */
(grp_stk_sct * const grp_stk,        /* I/O [sct] Group stack pointer */
 const int grp_id);                  /* I [ID] Group ID to push */

int                                  /* O [ID] Group ID that was popped */
nco_grp_stk_pop                      /* [fnc] Remove and return group ID from stack */
(grp_stk_sct * const grp_stk);       /* I/O [sct] Group stack pointer */
   
void
nco_grp_stk_free                     /* [fnc] Free group stack */
(grp_stk_sct * const grp_stk);       /* O [sct] Group stack pointer */
 
int                                  /* [rcd] Return code */
nco_grp_dfn                          /* [fnc] Define groups in output file */
(const int out_id,                   /* I [ID] netCDF output-file ID */
 nm_id_sct *grp_xtr_lst,             /* [grp] Number of groups to be defined */
 const int grp_nbr);                 /* I [nbr] Number of groups to be defined */

int                                  /* [rcd] Return code */
nco_def_grp_rcr                      /* [fnc] Define groups */
(const int in_id,                    /* I [ID] netCDF input-file ID */
 const int out_id,                   /* I [ID] netCDF output-file ID */
 const char * const prn_nm,          /* I [sng] Parent group name */
 const int rcr_lvl);                 /* I [nbr] Recursion level */

int
nco_get_sls_chr_cnt                   /* [fnc] Get number of slash characterrs in a string path  */
(char * const nm_fll);                /* I [sct] Full name  */

int
nco_get_str_pth_sct                   /* [fnc] Get string path structure  */
(char * const nm_fll,                 /* I [sng] Full name  */ 
 str_pth_sct ***str_pth_lst);         /* I/O [sct] List of path components  */  

void
nco_prt_grp_nm_fll                   /* [fnc] Debug function to print group full name from ID */
(const int grp_id);                  /* I [ID] Group ID */

nco_bool                             /* O [flg] All user-specified names are in file */
nco_xtr_mk                           /* [fnc] Check -v and -g input names and create extraction list */
(char **grp_lst_in,                  /* I [sng] User-specified list of groups */
 const int grp_xtr_nbr,              /* I [nbr] Number of groups in list */
 char **var_lst_in,                  /* I [sng] User-specified list of variables */
 const int var_xtr_nbr,              /* I [nbr] Number of variables in list */
 const nco_bool EXTRACT_ALL_COORDINATES,  /* I [flg] Process all coordinates */ 
 const nco_bool flg_unn,             /* I [flg] Select union of specified groups and variables */
 trv_tbl_sct * const trv_tbl);       /* I/O [sct] Traversal table */

void
nco_xtr_xcl                          /* [fnc] Convert extraction list to exclusion list */
(trv_tbl_sct * const trv_tbl);       /* I/O [sct] Traversal table */


void
nco_xtr_crd_add                       /* [fnc] Add all coordinates to extraction list */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_xtr_cf_add                        /* [fnc] Add to extraction list variable associated with CF convention */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const char * const cf_nm,            /* I [sng] CF convention ("coordinates" or "bounds") */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_xtr_cf_prv_add                    /* [fnc] Add specified CF-compliant coordinates of specified variable to extraction list */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const trv_sct * const var_trv,       /* I [sct] Variable (object) */
 const char * const cf_nm,            /* I [sng] CF convention ( "coordinates" or "bounds") */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */


void 
nco_prn_att_trv                       /* [fnc] Print all attributes of single variable */
(const int in_id,                     /* I [id] netCDF input file ID */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void
nco_xtr_crd_ass_add                   /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                     /* I netCDF file ID */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_get_prg_info(void);               /* [fnc] Get program info */

void
nco_prn_xtr_dfn                       /* [fnc] Print variable metadata (called with PRN_VAR_METADATA) */
(const int nc_id,                     /* I netCDF file ID */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */


void 
xtr_lst_prn                            /* [fnc] Print name-ID structure list */
(nm_id_sct * const nm_id_lst,          /* I [sct] Name-ID structure list */
 const int nm_id_nbr);                 /* I [nbr] Number of name-ID structures in list */

void 
nco_nm_id_cmp                         /* [fnc] Compare 2 name-ID structure lists */
(nm_id_sct * const nm_id_lst1,        /* I [sct] Name-ID structure list */
 const int nm_id_nbr1,                /* I [nbr] Number of name-ID structures in list */
 nm_id_sct * const nm_id_lst2,        /* I [sct] Name-ID structure list */
 const int nm_id_nbr2,                /* I [nbr] Number of name-ID structures in list */
 const nco_bool SAME_ORDER);          /* I [flg] Both lists have the same order */

void
nco_trv_tbl_chk                       /* [fnc] Validate trv_tbl_sct from a nm_id_sct input */
(const int nc_id,                     /* I [id] netCDF file ID */
 nm_id_sct * const xtr_lst,           /* I [sct] Extraction list  */
 const int xtr_nbr,                   /* I [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl,   /* I [sct] Traversal table */
 const nco_bool NM_ID_SAME_ORDER);    /* I [flg] Both nm_id_sct have the same order */

void                          
nco_prt_dmn                           /* [fnc] Print dimensions for a group  */
(const int nc_id,                     /* I [ID] File ID */
 const char * const grp_nm_fll);      /* I [sng] Full name of group */

void
nco_prn_var_val                       /* [fnc] Print variable data (called with PRN_VAR_DATA) */
(const int nc_id,                     /* I netCDF file ID */
 char * const dlm_sng,                /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV,      /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool MD5_DIGEST,           /* I [flg] Perform MD5 digests */
 const nco_bool PRN_DMN_UNITS,        /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL,  /* I [flg] Print dimension/coordinate indices/values */
 const nco_bool PRN_DMN_VAR_NM,       /* I [flg] Print dimension/variable names */
 const nco_bool PRN_MSS_VAL_BLANK,    /* I [flg] Print missing values as blanks */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */


void
nco_xtr_dfn                           /* [fnc] Define extracted groups, variables, and attributes in output file */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int nc_out_id,                 /* I [ID] netCDF output file ID */
 int * const cnk_map_ptr,             /* I [enm] Chunking map */
 int * const cnk_plc_ptr,             /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,             /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                   /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl,                   /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,           /* I [sng] GPE structure */
 const nco_bool CPY_GRP_METADATA,     /* I [flg] Copy group metadata (attributes) */
 const nco_bool CPY_VAR_METADATA,     /* I [flg] Copy variable metadata (attributes) */
 const char * const rec_dmn_nm,       /* I [sng] Record dimension name */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_xtr_wrt                           /* [fnc] Write extracted data to output file */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int nc_out_id,                 /* I [ID] netCDF output file ID */
 FILE * const fp_bnr,                 /* I [fl] Unformatted binary output file handle */
 const nco_bool MD5_DIGEST,           /* I [flg] Perform MD5 digests */
 const nco_bool HAVE_LIMITS,          /* I [flg] Dimension limits exist */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */


nco_bool                              /* O [flg] True if in scope */ 
nco_var_dmn_scp                       /* [fnc] Is variable in dimension scope */
(const trv_sct * const var_trv,       /* I [sct] GTT Object Variable */
 const dmn_trv_sct * const dmn_trv,   /* I [sct] GTT unique dimension */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */


int                                   /* O [rcd] Return code */
nco_grp_itr                           /* [fnc] Populate traversal table by examining, recursively, subgroups of parent  */
(const int grp_id,                    /* I [id] Group ID */
 char * const grp_pth,                /* I [sng] Absolute group path */
 trv_tbl_sct *trv_tbl);               /* I/O [sct] Group traversal table  */


void
nco_bld_trv_tbl                       /* [fnc] Construct GTT, Group Traversal Table (groups,variables,dimensions, limits)   */
(const int nc_id,                     /* I [ID] netCDF file ID */
 char * const grp_pth,                /* I [sng] Absolute group path where to start build (root typically) */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 int lmt_nbr,                         /* I [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt,                       /* I [sct] Structure comming from nco_lmt_prs() */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void                      
nco_bld_crd_rec_var_trv               /* [fnc] Build dimension information for all variables */
(const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void                      
nco_bld_crd_var_trv                   /* [fnc] Build GTT "crd_sct" coordinate variable structure */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

void                          
nco_prt_trv_tbl                      /* [fnc] Print GTT (Group Traversal Table) for debugging  with --get_grp_info  */
(const int nc_id,                    /* I [ID] File ID */
 const trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */

void
nco_bld_lmt_trv                       /* [fnc] Assign user specified dimension limits to traversal table dimensions   */
(const int nc_id,                     /* I [ID] netCDF file ID */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 int lmt_nbr,                         /* I [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt,                       /* I [sct] Structure comming from nco_lmt_prs() */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_bld_lmt                           /* [fnc] Assign user specified dimension limits to traversal table */
(const int nc_id,                     /* I [ID] netCDF file ID */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 int lmt_nbr,                         /* I [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt,                       /* I [sct] Structure comming from nco_lmt_prs() */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void                          
nco_bld_dmn_ids_trv                   /* [fnc] Build dimension info for all variables */
(const int nc_id,                     /* I [ID] netCDF file ID */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

dmn_trv_sct *                         /* O [sct] GTT dimension structure (stored in *groups*) */
nco_dmn_trv_sct                       /* [fnc] Return unique dimension object from unique ID */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

char *                                /* O [id] Unique dimension full name */
nco_dmn_fll_nm_id                     /* [fnc] Return unique dimension full name from unique ID  */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */


void                          
nco_has_crd_dmn_scp                  /* [fnc] Is there a variable with same name in dimension's scope?   */
(const trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */

void
nco_bld_var_dmn                       /* [fnc] Assign variables dimensions to either coordinates or dimension structs */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

crd_sct *                             /* O [sct] Coordinate object */
nco_scp_var_crd                       /* [fnc] Is coordinate in scope of variable?  */
(trv_sct *var_trv,                    /* I [sct] Variable object */
 dmn_trv_sct *dmn_trv);               /* I [sct] Dimension object */

int                                  /* O [nbr] Comparison result */
nco_cmp_crd_dpt                      /* [fnc] Compare two crd_sct's by group depth */
(const void *p1,                     /* I [sct] crd_sct* to compare */
 const void *p2);                    /* I [sct] crd_sct* to compare */


void                          
nco_wrt_trv_tbl                      /* [fnc] Obtain file information from GTT (Group Traversal Table) for debugging  */
(const int nc_id,                    /* I [ID] File ID */
 const trv_tbl_sct * const trv_tbl,  /* I [sct] GTT (Group Traversal Table) */
 nco_bool use_flg_xtr);              /* I [flg] Use flg_xtr in selection */

void                          
nco_gpe_chk                            /* [fnc] Check valid GPE new name  */
(const char * const grp_out_fll,       /* I [sng] Group name */
 const char * const var_nm,            /* I [sng] Variable name */
 gpe_nm_sct ** gpe_nm,                 /* I/O [sct] GPE name duplicate check array */
 int * nbr_gpe_nm);                    /* I/O [nbr] Number of GPE entries */ 

void
nco_get_rec_dmn_nm                     /* [fnc] Return array of record names  */
(const trv_sct * const var_trv,        /* I [sct] Variable object */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 nm_tbl_sct ** rec_dmn_nm);            /* I/O [sct] Array of record names */
 




#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GRP_UTL_H */
