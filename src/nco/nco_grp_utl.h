/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.h,v 1.181 2013-01-19 05:58:46 zender Exp $ */

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

nm_id_sct *                           /* O [sct] Extraction list */  
nco_trv_tbl_nm_id_old                 /* [fnc] Create extraction list of nm_id_sct from traversal table */
(const int nc_id,                     /* I [id] netCDF file ID */
 nm_id_sct *xtr_lst,                  /* I/O [sct] Extraction list  */
 int * const xtr_nbr,                 /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void
nco_flg_set_grp_var_ass /* [fnc] Set flags for groups and variables associated with matched object */
(const char * const grp_nm_fll, /* I [sng] Full name of group */
 const nco_obj_typ obj_typ, /* I [enm] Object type (group or variable) */
 trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */
  /* end nco_flg_set_grp_var_ass() */

int /* O [rcd] Return code */
nco_def_grp_full /* [fnc] Ensure all components of group path are defined */
(const int nc_id, /* I [ID] netCDF output-file ID */
 const char * const grp_nm_fll, /* I [sng] Full group name */
 int * const grp_out_id); /* O [ID] Deepest group ID */
  /* end nco_def_grp_full() */

int /* [rcd] Return code */
nco_inq_grps_full /* [fnc] Discover and return IDs of apex and all sub-groups */
(const int grp_id, /* I [ID] Apex group */
 int * const grp_nbr, /* O [nbr] Number of groups */
 int * const grp_ids); /* O [ID] Group IDs of children */
  /* end nco_inq_grps_full() */

void
nco_grp_itr_free /* [fnc] Free group iterator */
(grp_stk_sct * const grp_stk); /* O [sct] Group stack pointer */

int /* [rcd] Return code */
nco_grp_stk_get /* [fnc] Initialize and obtain group iterator */
(const int grp_id, /* I [ID] Apex group */
 grp_stk_sct ** const grp_stk); /* O [sct] Group stack pointer */
  /* end nco_grp_stk_get() */

int /* [rcd] Return code */
nco_grp_stk_nxt /* [fnc] Find and return next group ID */
(grp_stk_sct * const grp_stk, /* O [sct] Group stack pointer */
 int * const grp_id); /* O [ID] Group ID */
  /* end nco_grp_stk_nxt() */

grp_stk_sct * /* O [sct] Group stack pointer */
nco_grp_stk_ntl /* [fnc] Initialize group stack */
(void); 
  /* end nco_grp_stk_ntl() */

void
nco_grp_stk_psh /* [fnc] Push group ID onto stack */
(grp_stk_sct * const grp_stk, /* I/O [sct] Group stack pointer */
 const int grp_id); /* I [ID] Group ID to push */
  /* end nco_grp_stk_psh() */

int /* O [ID] Group ID that was popped */
nco_grp_stk_pop /* [fnc] Remove and return group ID from stack */
(grp_stk_sct * const grp_stk); /* I/O [sct] Group stack pointer */
  /* end nco_grp_stk_pop() */

void
nco_grp_stk_free /* [fnc] Free group stack */
(grp_stk_sct * const grp_stk); /* O [sct] Group stack pointer */
/* end nco_grp_stk_free() */

nm_id_sct *                               /* O [sct] Variable extraction list */
nco_var_lst_mk_trv                        /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id,                         /* I [ID] Apex group ID */
 char * const * const grp_lst_in,         /* I [sng] User-specified list of groups names to extract (specified with -g) */
 const int grp_xtr_nbr,                   /* I [nbr] Number of groups in current extraction list (specified with -g) */
 char * const * const var_lst_in,         /* I [sng] User-specified list of variable names and rx's */
 const trv_tbl_sct * const trv_tbl,       /* I [sct] Group traversal table */
 const nco_bool EXTRACT_ALL_COORDINATES,  /* I [flg] Process all coordinates */
 int * const var_xtr_nbr);                 /* I/O [nbr] Number of variables in current extraction list */
/* end nco_var_lst_mk_trv() */

nm_id_sct * /* O [sct] Group extraction list */
nco_grp_lst_mk /* [fnc] Create group extraction list using regular expressions */
(const int nc_id, /* I [ID] netCDF file ID */
 char * const * const grp_lst_in, /* I [sng] User-specified list of group names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 int * const grp_nbr); /* I/O [nbr] Number of groups in current extraction list */

int /* [rcd] Return code */
nco_grp_dfn /* [fnc] Define groups in output file */
(const int out_id, /* I [ID] netCDF output-file ID */
 nm_id_sct *grp_xtr_lst, /* [grp] Number of groups to be defined */
 const int grp_nbr); /* I [nbr] Number of groups to be defined */

int /* [rcd] Return code */
nco_def_grp_rcr
(const int in_id, /* I [ID] netCDF input-file ID */
 const int out_id, /* I [ID] netCDF output-file ID */
 const char * const prn_nm, /* I [sng] Parent group name */
 const int rcr_lvl); /* I [nbr] Recursion level */

int                                      /* O [rcd] Return code */
nco_grp_itr
(const int grp_id,                       /* I [id] Group ID */
 char * const grp_pth,                   /* I [sng] Absolute group path */
 trv_tbl_sct *trv_tbl);                  /* I/O [sct] Group traversal table  */
/* end nco_grp_itr() */

nm_id_sct *                              /* O [sct] Extraction list */
nco_var_lst_xcl_trv                      /* [fnc] Convert exclusion list to extraction list */
(const int nc_id,                        /* I [ID] netCDF file ID */
 nm_id_sct *xtr_lst,                     /* I/O [sct] Current exclusion list (destroyed) */
 int * const xtr_nbr,                    /* I/O [nbr] Number of variables in exclusion/extraction list */
 const trv_tbl_sct * const trv_tbl);     /* I [sct] Traversal table */

nm_id_sct *                /* O [sct] Extraction list */
nco_xtr_lst_add            /* [fnc] Auxiliary function; add an entry to xtr_lst */
(char * const var_nm,      /* I [sng] Variable name */
 char * const var_nm_fll,  /* I [sng] Full variable name */
 char * const grp_nm_fll,  /* I [sng] Full group name */
 int const var_id,         /* I [ID] Variable ID */
 nm_id_sct *xtr_lst,       /* I/O [sct] Current list */
 int * xtr_nbr);           /* I/O [nbr] Current index in exclusion/extraction list */

int                       /* [rcd] Return code */
nco4_inq_vars             /* [fnc] Find and return total of variables */
(const int nc_id,         /* I [ID] Apex group */
 int * const var_nbr_all);/* O [nbr] Number of variables in file */

void                          
nco_prt_grp_trv                   /* [fnc] Print table with -G */
(const int nc_id,                 /* I [ID] File ID */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void
nco_lmt_evl_trv            /* [fnc] Parse user-specified limits into hyperslab specifications */
(int nc_id,                /* I [idx] netCDF file ID */
 int lmt_nbr,              /* [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt,            /* I/O [sct] Structure from nco_lmt_prs() or from nco_lmt_sct_mk() to hold dimension limit information */
 nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void 
nco_prn_att_trv               /* [fnc] Print all attributes of single variable */
(const int in_id,             /* I [id] netCDF input file ID */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                      /* O [sct] Extraction list */
nco_var_lst_crd_add_trv          /* [fnc] Add all coordinates to extraction list */
(const int nc_id,                /* I [ID] netCDF file ID */
 nm_id_sct *xtr_lst,             /* I/O [sct] Current extraction list  */
 int * xtr_nbr,                  /* I/O [nbr] Number of variables in current extraction list */
 int * const grp_xtr_nbr,        /* I [nbr] Number of groups in current extraction list (specified with -g ) */
 char * const * const grp_lst_in,/* I [sng] User-specified list of groups names to extract (specified with -g ) */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                       /* O [sct] Extraction list */
nco_var_lst_crd_ass_add_trv       /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                 /* I netCDF file ID */
 nm_id_sct *xtr_lst,              /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,             /* I/O number of variables in current extraction list */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void                          
nco_chk_var                         /* [fnc] Check if input names of -v or -g are in file */
(const int nc_id,                   /* I [ID] Apex group ID */
 char * const * const var_lst_in,   /* I [sng] User-specified list of variable names and rx's */
 const int var_xtr_nbr,             /* I [nbr] Number of variables in current extraction list */
 const nco_bool EXCLUDE_INPUT_LIST); /* I [flg] Exclude rather than extract */
 
nco_bool                        /* O [flg] Name is in extraction list */
xtr_lst_fnd                     /* [fnc] Check if "var_nm_fll" is in extraction list */
(const char * const var_nm_fll, /* I [sng] Full variable name to find */
 nm_id_sct *xtr_lst,            /* I [sct] Name ID structure list */
 const int xtr_nbr);            /* I [nbr] Name ID structure list size */

int                             /* O [id] Group ID */
nco_aux_grp_id                  /* [fnc] Return the group ID from the variable full name */
(const int nc_id,               /* I [id] netCDF file ID */
 const char * const var_nm_fll);/* I [sng] Full variable name to find */

nm_id_sct *                      /* O [sct] Extraction list */
nco_aux_add_cf                   /* [fnc] Add to extraction list all coordinates associated with CF convention (associated with "var_nm_fll")*/
(const int nc_id,                /* I netCDF file ID */
 const char * const var_nm_fll,  /* I [sng] Full variable name */
 const char * const var_nm,      /* I [sng] Variable relative name */
 const char * const cf_nm,       /* I [sng] CF name to find ( "coordinates" or "bounds" */
 nm_id_sct *xtr_lst,             /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,            /* I/O number of variables in current extraction list */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                      /* O [sct] Extraction list */
nco_var_lst_crd_add_cf_trv       /* [fnc] Add to extraction list all coordinates associated with CF convention */
(const int nc_id,                /* I netCDF file ID */
 const char * const cf_nm,       /* I [sng] CF name to find ( "coordinates" or "bounds" */
 nm_id_sct *xtr_lst,             /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,            /* I/O number of variables in current extraction list */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

int                              /* O [nbr] Item found or not */
nco_fnd_var_trv                  /* [fnc] Find a variable that matches parameter "var_nm" and export to "nm_id" */
(const int nc_id,                /* I [id] netCDF file ID */
 const char * const var_nm,      /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl,   /* I [sct] Traversal table */
 nm_id_sct *nm_id);              /* O [sct] Entry to add to list */

nco_bool /* O [flg] All names are in file */
nco_chk_trv /* [fnc] Check if input names of -v or -g are in file */
(char * const * const obj_lst_in, /* I [sng] User-specified list of object names */
 const int obj_xtr_nbr, /* I [nbr] Number of items in list */
 const nco_obj_typ obj_typ, /* I [enm] Object type (group or variable) */
 trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

nco_bool /* O [flg] All user-specified names are in file */
nco_mk_xtr /* [fnc] Check -v and -g input names and create extraction list */
(char **grp_lst_in, /* I [sng] User-specified list of groups */
 const int grp_xtr_nbr, /* I [nbr] Number of groups in list */
 char **var_lst_in, /* I [sng] User-specified list of variables */
 const int var_xtr_nbr, /* I [nbr] Number of variables in list */
 const nco_bool EXTRACT_ALL_COORDINATES,  /* I [flg] Process all coordinates */ 
 const nco_bool flg_unn, /* I [flg] Select union of specified groups and variables */
 trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

void
nco_xtr_crd_add /* [fnc] Add all coordinates to extraction list */
(const int nc_id, /* I [ID] netCDF file ID */
 trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

void
nco_xtr_cf_trv /* [fnc] Add to extraction list variable associated with CF convention */
(const int nc_id, /* I [ID] netCDF file ID */
 const char * const cf_nm, /* I [sng] CF convention ("coordinates" or "bounds") */
 trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

void
nco_xtr_cf_add /* [fnc] Add to extraction list all CF-compliant coordinates */
(const int nc_id, /* I [ID] netCDF file ID */
 const char * const var_nm_fll, /* I [sng] Full variable name */
 const char * const var_nm, /* I [sng] Variable relative name */
 const char * const cf_nm, /* I [sng] CF convention ( "coordinates" or "bounds") */
 trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

void
nco_grp_var_mk_trv                     /* [fnc] Create groups/write variables in output file */
(const int nc_id,                      /* I [ID] netCDF input file ID  */
 const int nc_out_id,                  /* I [ID] netCDF output file ID  */
 const gpe_sct * const gpe,            /* I [sng] GPE structure */
 nm_id_sct * const xtr_lst,            /* I [sct] Extraction list  */
 const int xtr_nbr,                    /* I [nbr] Number of members in list */
 const int lmt_nbr,                    /* I [nbr] Number of dimensions with limits */
 lmt_all_sct * const * lmt_all_lst,    /* I multi-hyperslab limits */
 const int lmt_all_lst_nbr,            /* I [nbr] Number of hyperslab limits */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 nco_bool PRN_VAR_METADATA,            /* I [flg] Copy variable metadata (attributes) */
 int * const cnk_map_ptr,              /* I [enm] Chunking map */
 int * const cnk_plc_ptr,              /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 FILE * const fp_bnr,                  /* I [fl] Unformatted binary output file handle */
 const nco_bool MD5_DIGEST,            /* I [flg] Perform MD5 digests */
 const nco_bool MD_DFN,              /* I [flg] netCDF define mode is true */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void 
nco_msa_lmt_all_int_trv                /* [fnc] Initilaize lmt_all_sct's; recursive version */ 
(int in_id,                            /* [ID]  netCDF file ID */
 nco_bool MSA_USR_RDR,                 /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 lmt_all_sct **lmt_all_lst,            /* [sct] List of *lmt_all_sct structures */
 int nbr_dmn_fl,                       /* I [nbr] Number of dimensions in file */
 lmt_sct **lmt,                        /* [sct] Limits of the current hyperslab */
 int lmt_nbr,                          /* I [nbr] Number of limit structures in list */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                            /* O [sct] Extraction list */                                
nco_aux_add_dmn_trv                    /* [fnc] Add a coordinate variable that matches parameter "var_nm" */
(const int nc_id,                      /* I [id] netCDF file ID */
 const char * const var_nm,            /* I [sng] Variable name to find */
 nm_id_sct *xtr_lst,                   /* I/O [sct] Current extraction list  */
 int * const xtr_nbr,                  /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nco_bool                               /* O [flg] Dimension was found */
nco_fnd_dmn                            /* [fnc] Find a dimension that matches dm_nm in group grp_id and its parents */
(const int grp_id,                     /* I [id] Group ID */
 const char * const dmn_nm,            /* I [sng] Dimension name to find */
 const long dmn_sz);                   /* I [nbr] Dimension size to find */

void 
xtr_lst_prn                            /* [fnc] Validated name-ID structure list */
(nm_id_sct * const nm_id_lst,          /* I [sct] Name-ID structure list */
 const int nm_id_nbr);                 /* I [nbr] Number of name-ID structures in list */

void 
nco_nm_id_val                          /* [fnc] Validated name-ID structure list */
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
nco_xtr_xcl /* [fnc] Convert extraction list to exclusion list */
(trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

void                               
nco_xtr_crd_ass_add2                  /* [fnc] Add a coordinate variable that matches parameter "dmn_var_nm" */
(const int nc_id,                     /* I [id] netCDF file ID */
 const char * const dmn_var_nm,       /* I [sng] Coordinate variable name to find */
 const char * const var_nm,           /* I [sng] Variable name  */
 const char * const grp_nm_fll,       /* I [sng] Full group name for "var_nm" */
 trv_tbl_sct *trv_tbl);               /* I/O [sct] Traversal table */

void
nco_trv_tbl_chk                       /* [fnc] Validate trv_tbl_sct from a nm_id_sct input */
(const int nc_id,                     /* I netCDF file ID */
 nm_id_sct * const xtr_lst,           /* I [sct] Extraction list  */
 const int xtr_nbr,                   /* I [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl,   /* I [sct] Traversal table */
 const nco_bool NM_ID_SAME_ORDER);    /* I [flg] Both nm_id_sct have the same order */

void
nco_xtr_crd_ass_add_trv               /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                     /* I netCDF file ID */
 trv_tbl_sct *trv_tbl);               /* I/O [sct] Traversal table */

void
nco_get_prg_info(void);               /* [fnc] Get program info */

void
nco_xtr_wrt /* [fnc] Write extracted data to output file */
(const int nc_id, /* I [ID] netCDF input file ID */
 const int nc_out_id, /* I [ID] netCDF output file ID */
 const int lmt_nbr, /* I [nbr] Number of dimensions with limits */
 lmt_all_sct * const * lmt_all_lst, /* I [sct] Multi-hyperslab limits */
 const int lmt_all_lst_nbr, /* I [nbr] Number of hyperslab limits */
 FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
 const nco_bool MD5_DIGEST, /* I [flg] Perform MD5 digests */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

void
nco_xtr_dfn /* [fnc] Define extracted groups, variables, and attributes in output file */
(const int nc_id, /* I [ID] netCDF input file ID */
 const int nc_out_id, /* I [ID] netCDF output file ID */
 int * const cnk_map_ptr, /* I [enm] Chunking map */
 int * const cnk_plc_ptr, /* I [enm] Chunking policy */
 const size_t cnk_sz_scl, /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr, /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl, /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe, /* I [sng] GPE structure */
 const int lmt_nbr, /* I [nbr] Number of dimensions with limits */
 lmt_all_sct * const * lmt_all_lst, /* I [sct] Multi-hyperslab limits */
 const int lmt_all_lst_nbr, /* I [nbr] Number of hyperslab limits */
 const nco_bool PRN_VAR_METADATA, /* I [flg] Copy variable metadata (attributes) */
 trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

void
nco_prn_xtr_dfn                  /* [fnc] Print variable metadata (called with PRN_VAR_METADATA) */
(const int nc_id,                     /* I netCDF file ID */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

void
nco_prn_var_val                  /* [fnc] Print variable data (called with PRN_VAR_DATA) */
(const int nc_id,                     /* I netCDF file ID */
 lmt_all_sct *  const * lmt_lst,      /* I [sct] Dimension limits */
 const int lmt_nbr,                   /* I [nbr] Number of dimensions with user-specified limits */
 char * const dlm_sng,                /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV,      /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool MD5_DIGEST,           /* I [flg] Perform MD5 digests */
 const nco_bool PRN_DMN_UNITS,        /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL,  /* I [flg] Print dimension/coordinate indices/values */
 const nco_bool PRN_DMN_VAR_NM,       /* I [flg] Print dimension/variable names */
 const nco_bool PRN_MSS_VAL_BLANK,    /* I [flg] Print missing values as blanks */
 const trv_tbl_sct * const trv_tbl);  /* I [sct] Traversal table */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GRP_UTL_H */
