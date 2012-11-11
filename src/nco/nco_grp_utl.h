/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.h,v 1.126 2012-11-11 23:19:25 pvicente Exp $ */

/* Purpose: Group utilities */

/* Copyright (C) 2011--2012 Charlie Zender
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
#include "nco_grp_trv.h" /* Group traversal storage */
#include "nco_mmr.h" /* Memory management */

/* Dynamic array implementation of group stack */
typedef struct {
  int grp_nbr; /* [nbr] Number of items in stack = number of elements in grp_id array */
  int *grp_id; /* [ID] Group ID */
} grp_stk_sct; 

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int /* O [rcd] Return code */
nco_def_grp_full /* [fnc] Ensure all components of group path are defined */
(const int nc_id, /* I [ID] netCDF output-file ID */
 const char * const grp_nm_fll, /* I [sng] Full group name */
 int * const grp_out_id); /* O [ID] Deepest group ID */

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

nm_id_sct * /* O [sct] Variable extraction list */
nco4_var_lst_mk /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id,  /* I [ID] netCDF group ID (root ID of input file) */
 char * const * const grp_lst_in, /* I [sng] User-specified list of groups names to extract (specified with -g) */
 const int grp_xtr_nbr,  /* I [nbr] Number of groups in current extraction list (specified with -g) */
 char * const * const var_lst_in, /* I [sng] User-specified list of variable names and rx's */
 const grp_tbl_sct * const trv_tbl, /* I [sct] Group traversal table */
 const nco_bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const var_xtr_nbr, /* I/O [nbr] Number of variables in current extraction list */
 int * const nbr_var_fl); /* O [nbr] Number of variables in input file */
  /* nco4_var_lst_mk() */

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
 grp_tbl_sct *trv_tbl);                  /* I/O [sct] Group traversal table  */
/* end nco_grp_itr() */

int                                      /* O [rcd] Return code */
nco_has_subgrps
(const int nc_id);                       /* I [ID] NetCDF file ID */  
/* end nco_has_subgrps() */

nm_id_sct *                     /* O [sct] Extraction list */
nco4_var_lst_xcl                /* [fnc] Convert exclusion list to extraction list */
(const int nc_id,               /* I [ID] netCDF file ID */
 const int nbr_var,             /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst,            /* I/O [sct] Current exclusion list (destroyed) */
 int * const xtr_nbr,           /* I/O [nbr] Number of variables in exclusion/extraction list */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void
nco4_xtr_grp_nm_fll              /* [fnc] Auxiliary function; extract full group name from a grp_trv_sct to a nm_id_sct */
(const int nc_id,                /* I [ID] netCDF file ID */
 nm_id_sct *xtr_lst,             /* I/O [sct] Current exclusion list */
 int * const xtr_nbr,            /* I [nbr] Current index in exclusion/extraction list */
 grp_trv_sct trv);               /* I [sct] Group traversal table entry */

nm_id_sct *                /* O [sct] Extraction list */
nco_xtr_lst_add            /* [fnc] Auxiliary function; add an entry to xtr_lst */
(char * const var_nm,      /* I [sng] Variable name */
 char * const var_nm_fll,  /* I [sng] Full variable name */
 char * const grp_nm_fll,  /* I [sng] Full group name */
 char * const grp_nm,      /* I [sng] Group name */
 int const var_id,         /* I [ID] Variable ID */
 int const grp_id,         /* I [ID] Group ID */ 
 nm_id_sct *xtr_lst,       /* I/O [sct] Current list */
 int * xtr_nbr);           /* I/O [nbr] Current index in exclusion/extraction list */

void                       
nco4_inq_trv              /* [fnc] Find and return global totals of dimensions, variables, attributes */
(int * const att_nbr_glb, /* O [nbr] Number of global attributes in file */
 int * const dmn_nbr_all, /* O [nbr] Number of dimensions in file */
 int * const var_nbr_all, /* O [nbr] Number of variables in file  */
 int * const grp_nbr_all, /* O [nbr] Number of groups in file */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

int                       /* [rcd] Return code */
nco4_inq_vars             /* [fnc] Find and return total of variables */
(const int nc_id,         /* I [ID] Apex group */
 int * const var_nbr_all);/* O [nbr] Number of variables in file */

void                          
nco_prt_trv                       /* [fnc] Print table with -z */
(const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */        

void                          
nco_prt_grp_trv                   /* [fnc] Print table with -G */
(const int nc_id,                 /* I [ID] File ID */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void
nco_lmt_evl_trv            /* [fnc] Parse user-specified limits into hyperslab specifications */
(int nc_id,                /* I [idx] netCDF file ID */
 int lmt_nbr,              /* [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt,            /* I/O [sct] Structure from nco_lmt_prs() or from nco_lmt_sct_mk() to hold dimension limit information */
 nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void 
nco_prn_att_trv               /* [fnc] Print all attributes of single variable */
(const int in_id,             /* I [id] netCDF input file ID */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                      /* O [sct] Extraction list */
nco_var_lst_crd_add_trv          /* [fnc] Add all coordinates to extraction list */
(const int nc_id,                /* I [ID] netCDF file ID */
 nm_id_sct *xtr_lst,             /* I/O [sct] Current extraction list  */
 int * xtr_nbr,                  /* I/O [nbr] Number of variables in current extraction list */
 int * const grp_xtr_nbr,        /* I [nbr] Number of groups in current extraction list (specified with -g ) */
 char * const * const grp_lst_in,/* I [sng] User-specified list of groups names to extract (specified with -g ) */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                       /* O [sct] Extraction list */
nco_var_lst_crd_ass_add_trv       /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                 /* I netCDF file ID */
 nm_id_sct *xtr_lst,              /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,             /* I/O number of variables in current extraction list */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void                          
nco_chk_var                         /* [fnc] Check if input names of -v or -g are in file */
(const int nc_id,                   /* I [ID] Apex group ID */
 char * const * const var_lst_in,   /* I [sng] User-specified list of variable names and rx's */
 const int var_xtr_nbr,             /* I [nbr] Number of variables in current extraction list */
 const nco_bool EXCLUDE_INPUT_LIST); /* I [flg] Exclude rather than extract */

void 
xtr_lst_ptr                     /*   [fnc] Print Name ID structure list */
(nm_id_sct *xtr_lst,            /* I [sct] Name ID structure list */
 const int xtr_nbr);            /* I [nbr] Name ID structure list size */

int                             /* O [nbr] True/False */
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
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                      /* O [sct] Extraction list */
nco_var_lst_crd_ass_add_cf_trv   /* [fnc] Add to extraction list all coordinates associated with CF convention */
(const int nc_id,                /* I netCDF file ID */
 const char * const cf_nm,       /* I [sng] CF name to find ( "coordinates" or "bounds" */
 nm_id_sct *xtr_lst,             /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,            /* I/O number of variables in current extraction list */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                      /* O [sct] Extraction list */
nco_var_lst_crd_add_cf_trv       /* [fnc] Add to extraction list all coordinates associated with CF convention */
(const int nc_id,                /* I netCDF file ID */
 const char * const cf_nm,       /* I [sng] CF name to find ( "coordinates" or "bounds" */
 nm_id_sct *xtr_lst,             /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,            /* I/O number of variables in current extraction list */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

int                              /* O [nbr] Item found or not */
nco_fnd_var_trv                  /* [fnc] Find a variable that matches parameter "var_nm" and export to "nm_id" */
(const int nc_id,                /* I [id] netCDF file ID */
 const char * const var_nm,      /* I [sng] Variable name to find */
 const grp_tbl_sct * const trv_tbl,   /* I [sct] Traversal table */
 nm_id_sct *nm_id);              /* O [sct] Entry to add to list */

nco_bool                          /* O [flg] Is name in file */
nco_chk_trv                       /* [fnc] Check if input names of -v or -g are in file */
(char * const * const var_lst_in, /* I [sng] User-specified list of variable or group names ( -v or -g ) */
 int const var_xtr_nbr,           /* I [nbr] Number of items in the above list */
 nc_typ  typ,                     /* I [enm] netCDF4 object type: is list group or variable */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

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
 const nco_bool NCO_BNR_WRT,           /* I [flg] Write binary file */
 const nco_bool DEF_MODE,              /* I [flg] netCDF define mode is true */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

void 
nco_msa_lmt_all_int_trv                /* [fnc] Initilaize lmt_all_sct's; recursive version */ 
(int in_id,                            /* [ID]  netCDF file ID */
 nco_bool MSA_USR_RDR,                 /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 lmt_all_sct **lmt_all_lst,            /* [sct] List of *lmt_all_sct structures */
 int nbr_dmn_fl,                       /* I [nbr] Number of dimensions in file */
 lmt_sct **lmt,                        /* [sct] Limits of the current hyperslab */
 int lmt_nbr,                          /* I [nbr] Number of limit structures in list */
 const grp_tbl_sct * const trv_tbl);   /* I [sct] Traversal table */

nm_id_sct *                         /* O [sct] Extraction list */                                
nco_aux_add_dmn_trv                     /* [fnc] Add a coordinate variable that matches parameter "var_nm" */
(const int nc_id,                   /* I [id] netCDF file ID */
 const char * const var_nm,         /* I [sng] Variable name to find */
 nm_id_sct *xtr_lst,                /* I/O [sct] Current extraction list  */
 int * const xtr_nbr,               /* I/O [nbr] Number of variables in extraction list */
 const grp_tbl_sct * const trv_tbl);/* I [sct] Traversal table */

nco_bool                    /* O [flg] Dimension was found */
nco_fnd_dmn                 /* [fnc] Find a dimension that matches dm_nm in group grp_id and its parents */
(int grp_id,                /* I [id] Group ID */
 const char * const dmn_nm, /* I [sng] Dimension name to find */
 int const dmn_len);        /* I [nbr] Dimension size to find */


#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_GRP_UTL_H */


