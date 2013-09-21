/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.h,v 1.381 2013-09-21 05:20:32 pvicente Exp $ */

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
#include "nco_att_utl.h" /* Attribute utilities */
#include "nco_aux.h" /* Auxiliary coordinates */
#include "nco_cnf_dmn.h" /* Conform dimensions */
#include "nco_cnk.h" /* Chunking */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_grp_trv.h" /* Group traversal */
#include "nco_mmr.h" /* Memory management */
#include "nco_msa.h" /* Multi-slabbing algorithm */
#include "nco_prn.h" /* Print variables, attributes, metadata */
#include "nco_var_lst.h" /* Variable list utilities */
#include "nco_cnv_csm.h" /* CCM/CCSM/CF conventions */

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
nco_get_sng_pth_sct                   /* [fnc] Get string path structure  */
(char * const nm_fll,                 /* I [sng] Full name  */ 
 sng_pth_sct ***str_pth_lst);         /* I/O [sct] List of path components  */  

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
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void
nco_xtr_crd_ass_add                   /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                     /* I netCDF file ID */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_get_prg_info(void);               /* [fnc] Get program info */

void
nco_prn_xtr_mtd                       /* [fnc] Print variable metadata (called with PRN_VAR_METADATA) */
(const int nc_id,                     /* I netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void 
nco_xtr_lst_prn                        /* [fnc] Print name-ID structure list */
(nm_id_sct * const nm_id_lst,          /* I [sct] Name-ID structure list */
 const int nm_id_nbr);                 /* I [nbr] Number of name-ID structures in list */

void                          
nco_prt_dmn                           /* [fnc] Print dimensions for a group  */
(const int nc_id,                     /* I [ID] File ID */
 const char * const grp_nm_fll);      /* I [sng] Full name of group */

void
nco_prn_xtr_val                       /* [fnc] Print variable data (called with PRN_VAR_DATA) */
(const int nc_id,                     /* I netCDF file ID */
 prn_fmt_sct * const prn_flg,         /* I/O [sct] Print formatting flags */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

int                                   /* O [nbr] Number of matches to current rx */
nco_trv_rx_search                     /* [fnc] Search for pattern matches in traversal table */
(const char * const rx_sng,           /* I [sng] Regular expression pattern */
 const nco_obj_typ obj_typ,           /* I [enm] Object type (group or variable) */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_xtr_dmn_mrk                      /* [fnc] Mark extracted dimensions */
(trv_tbl_sct * const trv_tbl);       /* I/O [sct] GTT (Group Traversal Table) */

void
nco_xtr_grp_mrk                      /* [fnc] Mark extracted groups */
(trv_tbl_sct * const trv_tbl);       /* I/O [sct] GTT (Group Traversal Table) */

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
 const md5_sct * const md5,           /* I [sct] MD5 configuration */
 const nco_bool CPY_GRP_METADATA,     /* I [flg] Copy group metadata (attributes) */
 const nco_bool CPY_VAR_METADATA,     /* I [flg] Copy variable metadata (attributes) */
 const int nco_pck_plc,               /* I [enm] Packing policy */
 const char * const rec_dmn_nm,       /* I [sng] Record dimension name */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_xtr_wrt                           /* [fnc] Write extracted data to output file */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int nc_out_id,                 /* I [ID] netCDF output file ID */
 FILE * const fp_bnr,                 /* I [fl] Unformatted binary output file handle */
 const md5_sct * const md5,           /* I [flg] MD5 Configuration */
 const nco_bool HAVE_LIMITS,          /* I [flg] Dimension limits exist */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

nco_bool                              /* O [flg] True if in scope */ 
nco_crd_var_dmn_scp                   /* [fnc] Is coordinate variable in dimension scope */
(const trv_sct * const var_trv,       /* I [sct] GTT Object Variable */
 const dmn_trv_sct * const dmn_trv,   /* I [sct] GTT unique dimension */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

int                                    /* [rcd] Return code */
nco_grp_itr                            /* [fnc] Populate traversal table by examining, recursively, subgroups of parent */
(const int grp_id,                     /* I [ID] Group ID */
 char * const grp_nm_fll,              /* I [sng] Absolute group name (path) */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] GTT (Group Traversal Table) */

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
nco_bld_dmn_ids_trv                   /* [fnc] Build dimension info for all variables */
(const int nc_id,                     /* I [ID] netCDF file ID */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

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
 
void
nco_bld_aux_crd                       /* [fnc] Parse auxiliary coordinates */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const int aux_nbr,                   /* I [nbr] Number of auxiliary coordinates */
 char *aux_arg[],                     /* I [sng] Auxiliary coordinates */
 int *lmt_nbr,                        /* I/O [nbr] Number of user-specified dimension limits */
 lmt_sct ***lmt,                      /* I/O [sct] Limit structure  */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

var_sct **                            /* O [sct] Variable list */  
nco_fll_var_trv                       /* [fnc] Fill-in variable structure list for all extracted variables */
(const int nc_id,                     /* I [id] netCDF file ID */
 int * const xtr_nbr,                 /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

var_sct **                            /* O [sct] Variable list */  
nco_var_trv                           /* [fnc] Fill-in variable structure list for all variables named "var_nm" */
(const int nc_id,                     /* I [id] netCDF file ID */
 const char * const var_nm,           /* I [sng] Variable name (relative) */
 int * const xtr_nbr,                 /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void
nco_cpy_fix_var_trv                   /* [fnc] Copy processing type fixed variables from input to output file */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int out_id,                    /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,           /* I [sng] GPE structure */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void                          
nco_prc_cmn                            /* [fnc] Process objects (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 int cnk_map,                          /* I [enm] Chunking map */
 int cnk_plc,                          /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const nco_bool CNV_CCM_CCSM_CF,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_sct * trv_1,                      /* I [sct] Table object */
 trv_sct * trv_2,                      /* I [sct] Table object */
 const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2,  /* I [sct] GTT (Group Traversal Table) */
 nco_bool flg_grp_1,                   /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

void                          
nco_cpy_fix                            /* [fnc] Copy processing type fixed object (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 int cnk_map,                          /* I [enm] Chunking map */
 int cnk_plc,                          /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const nco_bool CNV_CCM_CCSM_CF,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 trv_sct *trv_1,                       /* I/O [sct] Table object */
 const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

nco_bool                               /* O [flg] Copy packing attributes */
nco_pck_cpy_att                        /* [fnc] Inquire about copying packing attributes  */
(const int prg_id,                     /* I [enm] Program ID */
 const int nco_pck_plc,                /* I [enm] Packing policy */
 const var_sct * const var_prc);       /* I [sct] Variable */

nco_bool                               /* O [flg] True for match found */
nco_rel_mch                            /* [fnc] Relative match of object in table 1 to table 2  */
(const int nc_id_1,                    /* I [id] netCDF input-file ID from file 1 */
 const int nc_id_2,                    /* I [id] netCDF input-file ID from file 2 */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 int cnk_map,                          /* I [enm] Chunking map */
 int cnk_plc,                          /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const nco_bool CNV_CCM_CCSM_CF,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_sct * var_trv,                    /* I [sct] Table variable object (can be from table 1 or 2) */
 nco_bool flg_tbl_1,                   /* I [flg] Table variable object is from table1 for True, otherwise is from table 2 */
 nco_bool flg_grp_1,                   /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2,  /* I [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

void                          
nco_prc_cmn_nm                         /* [fnc] Process common objects from a common mames list  */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 int cnk_map,                          /* I [enm] Chunking map */
 int cnk_plc,                          /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const nco_bool CNV_CCM_CCSM_CF,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm,                 /* I [nbr] Number of common names entries */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

void
nco_var_prc_fix_trv                    /* [fnc] Store processed and fixed variables info into GTT */
(const int nbr_var_prc,                /* I [nbr] Number of processed variables */
 var_sct **var_prc,                    /* I [sct] Array of processed variables */
 const int nbr_var_fix,                /* I [nbr] Number of fixed variables */
 var_sct **var_fix,                    /* I [sct] Array of fixed variables */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

void
nco_var_typ_trv                        /* [fnc] Transfer variable type into GTT */
(const int xtr_nbr,                    /* I [nbr] Number of extracted variables */
 var_sct **var,                        /* I [sct] Array of extracted variables */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

var_sct *                              /* O [sct] Variable structure */
nco_var_fll_trv                        /* [fnc] Allocate variable structure and fill with metadata */
(const int nc_id,                      /* I [id] netCDF file ID */
 const int var_id,                     /* I [id] Variable ID */
 const trv_sct * const var_trv,        /* I [sct] Object to write (variable) */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

int                                    /* O [id] Output file variable ID */
nco_cpy_var_dfn_trv                    /* [fnc] Define specified variable in output file */
(const int nc_out_id,                  /* I [ID] netCDF output file ID */ 
 const int grp_in_id,                  /* I [id] netCDF input group ID */
 const int grp_out_id,                 /* I [id] netCDF output group ID */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 const char * const rec_dmn_nm_cst,    /* I [sng] User-specified record dimension, if any, to create or fix in output file */
 trv_sct *var_trv,                     /* I/O [sct] Object to write (variable) */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

void
nco_dmn_rdr_trv                        /* [fnc] Transfer dimension structures to be re-ordered (ncpdq) into GTT */
(int **dmn_idx_out_in,                 /* I [idx] Dimension correspondence, output->input, output of nco_var_dmn_rdr_mtd() */
 const int nbr_var_prc,                /* I [nbr] Size of above array (number of processed variables) */
 var_sct **var_prc_out,                /* I [sct] Processed variables */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

void
nco_var_dmn_rdr_mtd_trv               /* [fnc] Determine and set new dimensionality in metadata of each re-ordered variable */
(trv_tbl_sct * const trv_tbl,         /* I/O [sct] GTT (Group Traversal Table) */
 const int nbr_var_prc,               /* I [nbr] Number of processed variables */
 var_sct **var_prc,                   /* I/O [sct] Processed variables */
 var_sct **var_prc_out,               /* I/O [sct] Processed variables */
 const int nbr_var_fix,               /* I [nbr] Number of processed variables */
 var_sct **var_fix,                   /* I/O [sct] Fixed variables */
 dmn_sct **dmn_rdr,                   /* I [sct] Dimension structures to be re-ordered */
 const int dmn_rdr_nbr,               /* I [nbr] Number of dimension to re-order */
 const nco_bool *dmn_rvr_rdr);        /* I [flg] Reverse dimension */


void
nco_var_dmn_rdr_val_trv               /* [fnc] Change dimension ordering of variable values */
(const var_sct * const var_in,        /* I [ptr] Variable with metadata and data in original order */
 var_sct * const var_out,             /* I/O [ptr] Variable whose data will be re-ordered */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

nco_bool                              /* O [flg] Re-define dimension ordering */
nco_rdf_dmn_trv                       /* [fnc] Re-define dimension ordering */
(trv_sct var_trv,                     /* I [sct] varible with record dimension name, re-ordered */
 const trv_tbl_sct * const trv_tbl,   /* I [sct] GTT (Group Traversal Table) */
 int * idx_var_mrk_out);              /* O [nbr] Index in GTT where name was found */

nco_bool                              /* [flg] Name was found */
nco_var_prc_idx_trv                   /* [fnc] Find index of processed variable that matches full name */
(const char * const var_nm_fll,       /* I [nbr] Full name of variable */
 var_sct **var_prc_out,               /* I [sct] Processed variables */
 const int nbr_var_prc,               /* I [nbr] Number of processed variables */
 int * var_prc_idx_out);              /* O [nbr] Number of dimension to re-order */

nm_id_sct *                           /* O [sct] Dimension list */
nco_dmn_lst_mk_trv                    /* [fnc] Attach dimension IDs to dimension list */
(char **dmn_lst_in,                   /* I [sng] User-specified list of dimension names */
 const int nbr_dmn,                   /* I [nbr] Total number of dimensions in list */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void
nco_aed_prc_trv                       /* [fnc] Process single attribute edit for single variable (GTT) */
(const int nc_id,                     /* I [id] Input netCDF file ID */
 const aed_sct *aed,                  /* I [sct] Structure containing information necessary to edit */
 const int nbr_aed,                   /* I [nbr] Number of attribute structures */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

var_sct *                             /* O [sct] Variable */  
nco_var_get_trv                       /* [fnc] Fill-in variable structure for a variable named "var_nm" */
(const int nc_id,                     /* I [id] netCDF file ID */
 const char * const var_nm,           /* I [sng] Variable name (relative) */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void
nco_dmn_trv_msa_tbl                   /* [fnc] Update all GTT dimensions with hyperslabbed size */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const char * const rec_dmn_nm,       /* I [sng] Record dimension name */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

void                                  /* [fnc] Update all GTT dimensions with hyperslabbed size */
nco_dmn_msa_tbl                       /* [fnc] Define specified variable in output file */
(const int grp_in_id,                 /* I [id] netCDF input group ID */
 const char * const rec_dmn_nm_cst,   /* I [sng] User-specified record dimension, if any, to create or fix in output file */
 trv_sct *var_trv,                    /* I/O [sct] Object to write (variable) trv_map_dmn_set() is O */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void                          
nco_dmn_dgn_tbl                       /* [fnc] Transfer degenerated dimensions information into GTT  */
(dmn_sct **dmn_dgn,                   /* [sct] Degenerate (size 1) dimensions used by ncwa */
 const int nbr_dmn_dgn,               /* I [nbr] Total number of dimensions in list */
 trv_tbl_sct *trv_tbl);               /* I/O [sct] GTT (Group Traversal Table) */

void
nco_dmn_lst_ass_var_trv                /* [fnc] Create list of all dimensions associated with input variable list  (ncpdq only) */
(const int nc_id,                      /* I [id] netCDF file ID */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 int *nbr_dmn_xtr,                     /* O [nbr] Number of dimensions associated with variables to be extracted  */
 dmn_sct ***dim);                      /* O [sct] Array of dimensions associated with variables to be extracted  */

void
nco_dmn_avg_mk                         /* [fnc] Build dimensions to average(ncwa)/re-order(ncpdq) array from input dimension names */
(const int nc_id,                      /* I [id] netCDF file ID */
 char **obj_lst_in,                    /* I [sng] User-specified list of dimension names (-a names without the - ) */
 const int nbr_dmn_in,                 /* I [nbr] Total number of dimensions in input list (size of above array) */
 const nco_bool flg_rdd,               /* I [flg] Retain degenerate dimensions */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 dmn_sct ***dmn_avg,                   /* O [sct] Array of dimensions to average */
 int *nbr_dmn_avg);                    /* O [nbr] Number of dimensions to average (size of above array) */

void
nco_dmn_out_mk                         /* [fnc] Build dimensions array to keep on output */
(dmn_sct **dmn_xtr,                    /* I [sct] Array of dimensions associated with variables to be extracted  */
 const int nbr_dmn_xtr,                /* I [nbr] Number of dimensions associated with variables to be extracted (size of above array) */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 dmn_sct ***dmn_out,                   /* O [sct] Array of dimensions on ouput */
 int *nbr_dmn_out);                    /* O [nbr] Number of dimensions on output (size of above array) */

void
nco_dmn_id_mk                          /* [fnc] Mark flag average, optionally flag degenerate for all dimensions that have the input ID */
(const int dmn_id,                     /* I [nbr] Number of dimensions associated with variables to be extracted (size of above array) */
 const nco_bool flg_rdd,               /* I [flg] Mark flag retain degenerate dimension */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

void                          
nco_bld_rec_dmn                       /* [fnc] Build record dimensions array */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const nco_bool FORTRAN_IDX_CNV,      /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool flg_rec_all,          /* I [flg] Retrieve all records */
 trv_tbl_sct * trv_tbl);              /* I/O [sct] GTT (Group Traversal Table) */

void
nco_prt_tbl_lmt                       /* [fnc] Print table limits */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_bld_trv_tbl                       /* [fnc] Construct GTT, Group Traversal Table (groups,variables,dimensions, limits)   */
(const int nc_id,                     /* I [ID] netCDF file ID */
 char * const grp_pth,                /* I [sng] Absolute group path where to start build (root typically) */
 int lmt_nbr,                         /* I [nbr] number of dimensions with limits */
 CST_X_PTR_CST_PTR_CST_Y(char,lmt_arg), /* I [sng] List of user-specified dimension limits */
 const int aux_nbr,                   /* I [nbr] Number of auxiliary coordinates */
 char *aux_arg[],                     /* I [sng] Auxiliary coordinates */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 char **grp_lst_in,                   /* I [sng] User-specified list of groups */
 const int grp_lst_in_nbr,            /* I [nbr] Number of groups in list */
 char **var_lst_in,                   /* I [sng] User-specified list of variables */
 const int var_xtr_nbr,               /* I [nbr] Number of variables in list */
 const nco_bool EXTRACT_ALL_COORDINATES,  /* I [flg] Process all coordinates */ 
 const nco_bool flg_unn,              /* I [flg] Select union of specified groups and variables */
 const nco_bool EXCLUDE_INPUT_LIST,   /* I [flg] Exclude rather than extract groups and variables specified with -v */ 
 const nco_bool EXTRACT_ASSOCIATED_COORDINATES,  /* I [flg] Extract all coordinates associated with extracted variables? */ 
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
nco_msa_var_get_elm_trv             /* [fnc] Define a limit of size 1 to be used by ncra */
(const int nc_id,                   /* I [ID] netCDF file ID */
 var_sct *var_prc,                  /* I/O [sct] Variable */
 const char * const rec_nm_fll,     /* I [sng] Full name of record being done in loop (trv_tbl->lmt_rec[idx_rec]->nm_fll ) */
 const long idx_rec_crr_in,         /* [idx] Index of current record in current input file */
 const trv_tbl_sct * const trv_tbl);/* I [sct] GTT (Group Traversal Table) */


nco_bool                             /* O [flg] Skip record */
nco_skp_rec                          /* [fnc] Skip record  */
(const int idx_rec,                  /* I [nbr] Current record */
 const var_sct * const var,          /* I [sct] Variable */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

nco_bool                             /* O [flg] Skip variable  */
nco_skp_var                          /* [fnc] Skip variable while doing record   */
(const var_sct * const var_prc,      /* I [sct] Processed variable */
 const char * const rec_nm_fll,      /* I [sng] Full name of record being done in loop (trv_tbl->lmt_rec[idx_rec]->nm_fll ) */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GRP_UTL_H */
