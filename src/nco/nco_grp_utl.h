/* $Header$ */

/* Purpose: Group utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

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

void
nco_chk_nan                           /* [fnc] Check file for NaNs */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

  nm_id_sct *                           /* O [sct] Extraction list */  
nco_trv_tbl_nm_id                     /* [fnc] Create extraction list of nm_id_sct from traversal table */
(const int nc_id_in,                  /* I [ID] netCDF input file ID */
 const int nc_id_out,                 /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,           /* I [sct] GPE structure */
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
(grp_stk_sct * const grp_stk,         /* I/O [sct] Group stack pointer */
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
nco_prn_grp_nm_fll                   /* [fnc] Debug function to print group full name from ID */
(const int grp_id);                  /* I [ID] Group ID */

nco_bool                             /* O [flg] All user-specified names are in file */
nco_xtr_mk                           /* [fnc] Check -v and -g input names and create extraction list */
(char **grp_lst_in,                  /* I [sng] User-specified list of groups */
 const int grp_xtr_nbr,              /* I [nbr] Number of groups in list */
 char **var_lst_in,                  /* I [sng] User-specified list of variables */
 const int var_xtr_nbr,              /* I [nbr] Number of variables in list */
 const nco_bool EXCLUDE_INPUT_LIST,   /* I [flg] Exclude rather than extract groups and variables */
 const nco_bool EXTRACT_ALL_COORDINATES,  /* I [flg] Process all coordinates */ 
 const nco_bool flg_unn,             /* I [flg] Select union of specified groups and variables */
 trv_tbl_sct * const trv_tbl);       /* I/O [sct] Traversal table */

void
nco_xtr_xcl                          /* [fnc] Convert extraction list to exclusion list */
(nco_bool EXTRACT_ASSOCIATED_COORDINATES, /* I [flg] Extract all coordinates associated with extracted variables? */
 nco_bool GRP_XTR_VAR_XCL, /* [flg] Extract matching groups, exclude matching variables */
 trv_tbl_sct * const trv_tbl); /* I/O [sct] GTT (Group Traversal Table) */

void
nco_xtr_xcl_chk /* [fnc] Convert extraction list to exclusion list */
(char ** var_lst_in, /* I [sng] User-specified list of variables */
 const int var_xtr_nbr, /* I [nbr] Number of variables in list */
 trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */

void
nco_xtr_crd_add                       /* [fnc] Add all coordinates to extraction list */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_xtr_lst /* [fnc] Print extraction list and exit */
(trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */

void
nco_xtr_ND_lst /* [fnc] Print extraction list of N>=D variables and exit */
(trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */

void
nco_xtr_ilev_add                      /* [fnc] Add ilev coordinate to extraction list */
(trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

void
nco_xtr_cf_add                        /* [fnc] Add to extraction list variable associated with CF convention */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const char * const cf_nm,            /* I [sng] CF convention ("coordinates" or "bounds") */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_xtr_cf_var_add                    /* [fnc] Add specified CF-compliant coordinates of specified variable to extraction list */
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
nco_prn_xtr_mtd                       /* [fnc] Print variable metadata (called with PRN_VAR_METADATA) */
(const int nc_id,                     /* I netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void 
nco_xtr_lst_prn                        /* [fnc] Print name-ID structure list */
(nm_id_sct * const nm_id_lst,          /* I [sct] Name-ID structure list */
 const int nm_id_nbr);                 /* I [nbr] Number of name-ID structures in list */

void                          
nco_prn_dmn_grp                       /* [fnc] Print dimensions for a group  */
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

nco_bool                                                         
nco_pth_mch                            /* [fnc] Name component in full path matches user string  */
(char * const nm_fll,                  /* I [sng] Full name (path) */
 char * const nm,                      /* I [sng] Name (relative) */
 char * const usr_sng);                /* [sng] User-supplied object name */

void
nco_xtr_grp_mrk                      /* [fnc] Mark extracted groups */
(trv_tbl_sct * const trv_tbl);       /* I/O [sct] GTT (Group Traversal Table) */

void
nco_xtr_dfn                           /* [fnc] Define extracted groups, variables, and attributes in output file */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int nc_out_id,                 /* I [ID] netCDF output file ID */
 const cnk_sct * const cnk,           /* I [sct] Chunking structure */
 const int dfl_lvl,                   /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,           /* I [sng] GPE structure */
 const md5_sct * const md5,           /* I [sct] MD5 configuration */
 const nco_bool CPY_GRP_METADATA,     /* I [flg] Copy group metadata (attributes) */
 const nco_bool CPY_VAR_METADATA,     /* I [flg] Copy variable metadata (attributes) */
 const nco_bool RETAIN_ALL_DIMS,      /* I [flg] Retain all dimensions */
 const int nco_pck_plc,               /* I [enm] Packing policy */
 const char * const rec_dmn_nm,       /* I [sng] Record dimension name */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_xtr_wrt                           /* [fnc] Write extracted data to output file */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int nc_out_id,                 /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,           /* I [sct] GPE structure */
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
 char * const grp_nm_fll_prn,          /* I [sng] Absolute group name of parent (path) */
 char * const grp_nm_fll,              /* I [sng] Absolute group name (path) */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] GTT (Group Traversal Table) */

void                      
nco_bld_crd_rec_var_trv               /* [fnc] Build dimension information for all variables */
(const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void                      
nco_bld_crd_var_trv                   /* [fnc] Build GTT "crd_sct" coordinate variable structure */
(trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

void                          
nco_prn_trv_tbl                      /* [fnc] Print GTT (Group Traversal Table) for debugging  with --get_grp_info  */
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
nco_scp_var_crd                       /* [fnc] Return in scope coordinate for variable  */
(const trv_sct * const var_trv,       /* I [sct] Variable object */
 dmn_trv_sct * const dmn_trv);         /* I [sct] Dimension object */

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
 nm_lst_sct ** rec_dmn_nm);            /* I/O [sct] Array of record names */
 
void
nco_prs_aux_crd                       /* [fnc] Parse auxiliary coordinates */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const int aux_nbr,                   /* I [nbr] Number of auxiliary coordinates */
 char *aux_arg[],                     /* I [sng] Auxiliary coordinates */
 const nco_bool FORTRAN_IDX_CNV,      /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool MSA_USR_RDR,          /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 const nco_bool EXTRACT_ASSOCIATED_COORDINATES,  /* I [flg] Extract all coordinates associated with extracted variables? */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

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
nco_cpy_fix_var_trv                   /* [fnc] Copy fixed variables from input to output file */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int out_id,                    /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,           /* I [sng] GPE structure */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] GTT (Group Traversal Table) */

void                          
nco_prc_cmn                            /* [fnc] Process objects (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_sct * trv_1,                      /* I [sct] Table object */
 trv_sct * trv_2,                      /* I [sct] Table object */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

void                          
nco_cpy_fix                            /* [fnc] Copy fixed object (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 trv_sct *trv_1,                       /* I/O [sct] Table object */
 trv_tbl_sct * const trv_tbl_1,        /* I [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

nco_bool                               /* O [flg] Copy packing attributes */
nco_pck_cpy_att                        /* [fnc] Inquire about copying packing attributes  */
(const int nco_prg_id,                     /* I [enm] Program ID */
 const int nco_pck_plc,                /* I [enm] Packing policy */
 const var_sct * const var_prc);       /* I [sct] Variable */

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
 CST_X_PTR_CST_PTR_CST_Y(var_sct,var), /* I [sct] Array of extracted variables */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

void
nco_set_prm_typ_out /* [fnc] Set GTT variable output type to unpacked, arithmetically promoted type for integers */
(nco_bool const PROMOTE_INTS, /* I/O [flg] Promote integers to floating point in output */
 const int prc_nbr, /* I [nbr] Number of processed variables */
 CST_X_PTR_CST_PTR_CST_Y(var_sct,var), /* I [sct] Array of extracted variables */
 trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

var_sct *                              /* O [sct] Variable structure */
nco_var_fll_trv                        /* [fnc] Allocate variable structure and fill with metadata */
(const int nc_id,                      /* I [id] netCDF file ID */
 const int var_id,                     /* I [id] Variable ID */
 const trv_sct * const var_trv,        /* I [sct] Object to write (variable) */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

int                                    /* O [id] Output file variable ID */
nco_cpy_var_dfn_trv                    /* [fnc] Define specified variable in output file */
(const int nc_in_id,                   /* I [ID] netCDF input file ID */
 const int nc_out_id,                  /* I [ID] netCDF output file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const char * const grp_out_fll,       /* I [sng] Output group name */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 const char * const rec_dmn_nm_cst,    /* I [sng] User-specified record dimension, if any, to create or fix in output file */
 trv_sct *var_trv,                     /* I/O [sct] Object to write (variable) */
 dmn_cmn_sct **dmn_cmn_out,            /* I/O [sct] List of all dimensions in output file (used for RETAIN_ALL_DIMS) */
 int *nbr_dmn_cmn_out,                 /* I/O [sct] Number of all dimensions in output file (used for RETAIN_ALL_DIMS) */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] GTT (Group Traversal Table) */

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
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

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
 const nco_bool flg_dmn_prc_usr_spc,   /* I [flg] Processed dimensions specified on command line */
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
 lmt_sct ***lmt_rec,                  /* I/O [lst] (ncra) Record dimensions */
 int *nbr_rec,                        /* I/O [nbr] (ncra) Number of record dimensions (size of above array) */
 trv_tbl_sct * trv_tbl);              /* I/O [sct] GTT (Group Traversal Table) */

void
nco_prn_tbl_lmt                       /* [fnc] Print table limits */
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
 const nco_bool GRP_XTR_VAR_XCL,      /* I [flg] Extract matching groups, exclude matching variables */
 const nco_bool EXCLUDE_INPUT_LIST,   /* I [flg] Exclude rather than extract groups and variables specified with -v */ 
 const nco_bool EXTRACT_ASSOCIATED_COORDINATES,  /* I [flg] Extract all coordinates associated with extracted variables? */ 
 const nco_bool EXTRACT_CLL_MSR, /* I [flg] Extract cell_measures variables */
 const nco_bool EXTRACT_FRM_TRM, /* I [flg] Extract formula_terms variables */
 const int nco_pck_plc,               /* I [enm] Packing policy */
 nco_dmn_dne_t **flg_dne,             /* I/O [lst] Flag to check if input dimension -d "does not exist" */
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
nco_bld_lmt_var                       /* [fnc] Assign user specified dimension limits to one GTT variable */
(const int nc_id,                     /* I [ID] netCDF file ID */
  nco_bool MSA_USR_RDR,               /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
  int lmt_nbr,                        /* I [nbr] Number of user-specified dimension limits */
  lmt_sct **lmt,                      /* I [sct] Structure comming from nco_lmt_prs() */
  nco_bool FORTRAN_IDX_CNV,           /* I [flg] Hyperslab indices obey Fortran convention */
  trv_sct *wgt_trv);                  /* I/O [sct] GTT variable (used for weight/mask) */
 

void 
nco_msa_var_get_rec_trv             /* [fnc] Read one record of a variable */
(const int nc_id,                   /* I [ID] netCDF file ID */
 var_sct *var_prc,                  /* I/O [sct] Variable */
 const char * const rec_nm_fll,     /* I [sng] Record dimension name */
 const long idx_rec_crr_in,         /* [idx] Index of current record in current input file */
 const trv_tbl_sct * const trv_tbl);/* I [sct] GTT (Group Traversal Table) */

nco_bool                             /* O [flg] Skip variable  */
nco_skp_var                          /* [fnc] Skip variable while doing record   */
(const var_sct * const var_prc,      /* I [sct] Processed variable */
 const char * const rec_nm_fll,      /* I [sng] Full name of record being done in loop (trv_tbl->lmt_rec[idx_rec]->nm_fll ) */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

var_sct *                             /* O [sct] Variable (weight or mask) */  
nco_var_get_wgt_trv                   /* [fnc] Retrieve weighting or mask variable */
(const int nc_id,                     /* I [id] netCDF file ID */
 const int lmt_nbr,                   /* I [nbr] number of dimensions with limits */
 CST_X_PTR_CST_PTR_CST_Y(char, lmt_arg), /* I [sng] List of user-specified dimension limits */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 const char * const wgt_nm,           /* I [sng] Weight or mask variable name (relative or absolute) */
 const var_sct * const var,           /* I [sct] Variable that needs the weight/mask variable */
 const trv_tbl_sct * const trv_tbl);  /* I [lst] Traversal table */

void                                    
nco_grp_var_lst                        /* [fnc] Export list of variable names for group */
(const int nc_id,                      /* I [id] netCDF file ID */
 const char * const grp_nm_fll,        /* I [sng] Absolute group name */
 char ***nm_lst,                       /* I/O [sng] List of names */
 int *nm_lst_nbr);                     /* I/O [nbr] Number of items in list */

char *                                /* O [sng] Name of variable   */
nco_var_has_cf                        /* [fnc] Variable has CF-compliant information ("coordinates" or "bounds") */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const trv_sct * const var_trv,       /* I [sct] Variable (object) */
 const char * const cf_nm,            /* I [sng] CF convention ( "coordinates" or "bounds") */
 nco_bool *flg_cf_fnd);               /* I/O [flg] CF variable was found */

void
nco_chk_dmn                           /* [fnc] Check valid dimension names */
(const int lmt_nbr,                   /* I [nbr] number of dimensions with limits */
 nco_dmn_dne_t * flg_dne);            /* I [lst] Flag to check if input dimension -d "does not exist" */

void
nco_chk_dmn_in                        /* [fnc] Check input dimensions */
(const int lmt_nbr,                   /* I [nbr] Number of user-specified dimension limits */
 CST_X_PTR_CST_PTR_CST_Y(lmt_sct,lmt), /* I [sct] List of user-specified dimension limits */
 nco_dmn_dne_t ** const dne_lst,      /* I/O [lst] Input dimension does not exist */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

nco_bool                               /* O [flg] True if variable 1 is in scope of variable 2 */
nco_var_scp                            /* [fnc] Is variable 1 is in scope of variable 2 */
(const trv_sct * const var_trv_1,      /* I [sct] Variable 1 */
 const trv_sct * const var_trv_2,      /* I [sct] Variable 2 */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void
nco_dmn_swap                           /* [fnc] Swap dimensions */
(const char * const dmn_nm_1,          /* I [sng] Name of dimension 1 */
 const char * const dmn_nm_2,          /* I [sng] Name of dimension 2 */
 dmn_cmn_sct *dmn_cmn,                 /* I/O [sct] Dimension structure array */
 const int nbr_dmn);                   /* I [nbr] Number of dimensions (size of above array) */

void
nco_dfn_dmn                            /* [fnc] Define dimension size and ID in array */
(const char * const dmn_nm_fll_out,    /* I [sng] Full name of dimension in output */
 const long dmn_sz,                    /* I [nbr] Size of dimension */
 const int dmn_id_out,                 /* I [id] ID of dimension in output */
 dmn_cmn_sct *dmn_cmn,                 /* I/O [sct] Dimension structure array */
 const int nbr_dmn);                   /* I [nbr] Number of dimensions (size of above array) */

int                                    /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_aux_crd_dpt                    /* [fnc] Compare two aux_crd_sct's by group depth */
(const void *val_1,                    /* I [sct] aux_crd_sct * to compare */
 const void *val_2);                   /* I [sct] aux_crd_sct * to compare */

int                                    /* [flg] True if at least one standard_name="latitude/longitude" pair have been found  */      
nco_bld_crd_aux                        /* [fnc] Build auxiliary coordinates information into table */
(const int nc_id,                      /* I [ID] netCDF file ID */
 trv_tbl_sct *trv_tbl);                /* I [sct] GTT (Group Traversal Table) */

int                                    /* [flg] True if at least one nm_lat,nm_lon pair have been found  */            
nco_bld_crd_nm_aux                     /* [fnc] Build auxiliary coordinates information into table  using named latitude and longitude*/
(const int nc_id,                      /* I [ID] netCDF file ID */
 const char * const nm_lat,            /* I [sng] name of "latitude" variable to find  */
 const char * const nm_lon,            /* I [sng] name of "latitude" variable to find  */
 trv_tbl_sct *trv_tbl);                /* I [sct] GTT (Group Traversal Table) */


  
void
nco_lmt_aux_tbl                       /* [fnc] Apply limits to variable in table */
(const int nc_id,                     /* I [ID] netCDF file ID */
 lmt_sct **lmt,                       /* I [sct] Limits */
 const int nbr_lmt,                   /* I [nbr] Number of limits */
 const char * const var_nm_fll,       /* I [sng] Variable full name */
 const int dmn_id,                    /* I [id] ID of dimension to apply the limits */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

int *
nco_dmn_malloc                         /* [fnc] Inquire about number of dimensions in group and return dynamic array of dimension IDs */
(const int nc_id,                      /* I [ID] netCDF file ID */
 const char * const grp_nm_fll,        /* I [sng] Group full name */
 int *nbr_dmn);                        /* I/O [nbr] Number of dimensions in group */

void
nco_lmt_std_att_lat_lon               /* [fnc] Apply limits to variable in table that contains 'standard_name' attribute "latitude" */
(const int nc_id,                     /* I [ID] netCDF file ID */
 lmt_sct **lmt,                       /* I [sct] Limits */
 const int nbr_nbr,                   /* I [nbr] Number of limits */
 const int dmn_id,                    /* I [id] ID of dimension to apply the limits */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

void
nco_lmt_aux                           /* [fnc] Apply auxiliary -X limits (Auxiliary function called by different functions ) */
(const int nc_id,                     /* I [ID] netCDF file ID */
 lmt_sct **lmt,                       /* I [sct] Limits */
 const int nbr_nbr,                   /* I [nbr] Number of limits */
 nco_bool FORTRAN_IDX_CNV,            /* I [flg] Hyperslab indices obey Fortran convention */
 nco_bool MSA_USR_RDR,                /* I [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 const int idx_tbl,                   /* I [nbr] Table index */
 const int idx_dmn,                   /* I [nbr] Dimension index */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] GTT (Group Traversal Table) */

void
nco_dmn_lmt                            /* [fnc] Convert a lmt_sct array to dmn_sct (name only) */
(lmt_sct **lmt,                        /* I [lst] lmt_sct array  */
 const int nbr_lmt,                    /* I [nbr] Size of lmt_sct array  */
 dmn_sct ***dmn);                      /* O [sct] dmn_sct array  */

void
nco_prn_nsm                           /* [fnc] Print ensembles  */
(const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void
nco_bld_nsm                           /* [fnc] Build ensembles */
(const int nc_id,                     /* I [id] netCDF file ID */
 const nco_bool flg_fix_xtr,          /* I [flg] Mark fized variables as extracted  */
 const cnv_sct * const cnv,           /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_pck_plc,               /* I [enm] Packing policy */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_nsm_dfn_wrt                       /* [fnc] Define OR write ensemble fixed variables */
(const int nc_id,                     /* I [ID] netCDF input file ID */
 const int nc_out_id,                 /* I [ID] netCDF output file ID */
 const cnk_sct * const cnk,           /* I [sct] Chunking structure */
 const int dfl_lvl,                   /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,           /* I [sct] GPE structure */
 const nco_bool flg_def,              /* I [flg] Define OR write */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

void
nco_nsm_ncr                           /* [fnc] Increase ensembles (more than 1 file cases) */
(const int nc_id,                     /* I [id] netCDF file ID */
 trv_tbl_sct * const trv_tbl);        /* I/O [sct] Traversal table */

nco_bool                               /* O [flg] True for match found */
nco_prc_rel_mch                        /* [fnc] Relative match of object in table 1 to table 2  */
(const int nc_id_1,                    /* I [id] netCDF input-file ID from file 1 */
 const int nc_id_2,                    /* I [id] netCDF input-file ID from file 2 */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_sct * var_trv,                    /* I [sct] Table variable object (can be from table 1 or 2) */
 const nco_bool flg_tbl_1,             /* I [flg] Table variable object is from table1 for True, otherwise is from table 2 */
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

void                          
nco_cmn_var                            /* [fnc] Does the list of common names contain absolute variables match? */
(const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2,  /* I [sct] GTT (Group Traversal Table) */
 const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm,                 /* I [nbr] Number of common names entries */
 nco_bool *flg_cmn_abs,                /* I/O [flg] Does the list of common names contain absolute variables match? */
 nco_bool *flg_cmn_rel);               /* I/O [flg] Does the list of common names contain relative variables match? */

void                                               
nco_prc_cmn_var_nm_fll                 /* [fnc] Process (define, write) absolute variables in both files (same path) (ncbo) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm,                 /* I [nbr] Number of common names entries */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False for write variables ) */

void                          
nco_grp_brd2                           /* [fnc] Group broadcasting (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False for write variables ) */

void                          
nco_grp_brd                            /* [fnc] Group broadcasting (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False for write variables ) */

void                          
nco_cmn_nsm_var                        /* [fnc] Common variable exist in ensembles (ncbo only) */
(nco_bool *flg_var_cmn,                /* I/O [flg] Common variable exists */
 nco_bool *flg_var_cmn_rth,            /* I/O [flg] Common variable exists at root */
 nm_lst_sct **var_nm,                  /* I/O [sct] Array of common variable names */ 
 nm_lst_sct **var_nm_rth,              /* I/O [sct] Array of common variable names at root */ 
 const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2); /* I [sct] GTT (Group Traversal Table) */

void                                               
nco_prc_cmn_nsm                        /* [fnc] Process (define, write) variables belonging to ensembles in both files (ncbo) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False for write variables ) */

void                                               
nco_prc_nsm                            /* [fnc] Process (define, write) variables belonging to ensembles only in 1 file (ncbo) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nm_lst_sct * const var_lst,     /* I [sct] Array of common variable names from file not having ensembles */ 
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False for write variables ) */

void                          
nco_fix_dfn_wrt                        /* [fnc] Define/write fixed variables (ncbo) */
(const int nc_id,                      /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 trv_sct * trv,                        /* I [sct] Table object */
 trv_tbl_sct * const trv_tbl,          /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

nco_bool                               /* O [flg] True for match found */
nco_rel_mch                            /* [fnc] Relative match of object in table 1 to table 2  */
(trv_sct * var_trv,                    /* I [sct] Table variable object (can be from table 1 or 2) */
 const nco_bool flg_tbl_1,             /* I [flg] Table variable object is from table1 for True, otherwise is from table 2 */
 const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2); /* I [sct] GTT (Group Traversal Table) */

void                          
nco_prc_rel_cmn_nm                     /* [fnc] Process common relative objects from a common mames list (ncbo only) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm,                 /* I [nbr] Number of common names entries */
 const nco_bool flg_dfn);              /* I [flg] Action type (True for define variables, False when write variables ) */

void
nco_chk_nsm                            /* [fnc] Check if ensembles are valid  */                                
(const int in_id,                      /* I [id] netCDF input-file ID of current file, starting with first  */
 const int fl_idx,                     /* I [nbr] Index of file loop  */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) of *first* file */

void                          
nco_nsm_att                            /* [fnc] Inquire if ensemble parent group has "ensemble" attribute (ncbo only) */
(const int nc_id,                      /* I [id] netCDF file ID  */
 const trv_tbl_sct * const trv_tbl,    /* I [sct] GTT (Group Traversal Table) */
 nco_bool *flg_nsm_att,                /* I/O [flg] "ensemble" attribute exists */
 nm_lst_sct **nsm_grp_nm_fll_prn);     /* I/O [sct] Array of ensemble paths read in the attributes */ 

void                      
nco_rad                                /* [fnc] Retain all dimensions */
(const int nc_out_id,                  /* I [ID] netCDF output file ID */
 const int nbr_dmn_var_out,            /* I [nbr] Number of dimensions for variable on output  */
 const dmn_cmn_sct * const dmn_cmn,    /* I [sct] Dimension structure in output file */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

void                                               
nco_prc_cmn_nsm_att                    /* [fnc] Process (define, write) variables belonging to ensembles in both files (ncbo) */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 const cnk_sct * const cnk,            /* I [sct] Chunking structure */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 gpe_nm_sct *gpe_nm,                   /* I/O [sct] GPE name duplicate check array */
 int nbr_gpe_nm,                       /* I/O [nbr] Number of GPE entries */  
 const cnv_sct * const cnv,            /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_grp_1,             /* I [flg] Use table 1 as template for group creation on True, otherwise use table 2 */
 const nco_bool flg_dfn,               /* I [flg] Action type (True for define variables, False for write variables ) */
 nm_lst_sct *nsm_grp_nm_fll_prn);      /* I/O [sct] Array of ensemble paths read in the attributes */ 

void
nco_prn_dmn                            /* [fnc] Print dimensions (debug) */
(const int nc_id,                      /* I [ID] netCDF file ID */
 const char * const grp_nm_fll,        /* I [sng] Group name full */
 const char * const var_nm,            /* I [sng] Variable name relative */
 const char * const var_nm_fll,        /* I [sng] Variable name full */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] GTT (Group Traversal Table) */

void
nco_nsm_wrt_att                      /* [fnc] Save ncge metadata attribute */
(const int nc_id,                    /* I [ID] netCDF input file ID */
 const int out_id,                   /* I [ID] netCDF output file ID */
 const gpe_sct * const gpe,          /* I [sct] GPE structure */
 trv_tbl_sct * const trv_tbl);       /* I/O [sct] GTT (Group Traversal Table) */

void
nco_wrt_atr
(const int nc_id,                    /* I [id] netCDF input file ID */
 const int grp_out_id,               /* I [id] netCDF output group ID */
 const int var_out_id,               /* I [id] netCDF output variable ID */
 const trv_sct *var_trv);            /* I [sct] traversal variable */

const char *
nco_get_dmn_nm_fll                     /* [fnc] Return name corresponding to input dimension ID (debug) */
(const int dmn_id,                     /* I [id] ID of dimension */
 const dmn_cmn_sct * const dmn_cmn,    /* I [sct] Dimension structure array */
 const int nbr_dmn);                   /* I [nbr] Number of dimensions (size of above array) */

void
nco_var_xtr_trv                       /* [fnc] Print all variables to extract (debug) */
(const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

crd_sct*
nco_get_crd_sct                       /* [fnc] Return a coordinate variable crd_sct for a given table variable var_trv */
(trv_sct * const var_trv,             /* I [sct] GTT Variable */
 int lmt_nbr,                         /* I [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt);                      /* I [sct] Limit array. Structure comming from nco_lmt_prs() */

void
nco_srt_aux                           /* [fnc] sort auxiliary coordinates */  
(const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GRP_UTL_H */
