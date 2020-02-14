/* $Header$ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_msa.h" *//* Multi-slabbing algorithm */

#ifndef NCO_MSA_H
#define NCO_MSA_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, qsort */
#include <limits.h> /* need LONG_MAX */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_bnr.h" /* Binary file utilities */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_grp_utl.h" /* Group utilities */
#include "nco_lmt.h" /* Hyperslab limits */
#include "nco_mmr.h" /* Memory management */
#include "nco_prn.h" /* Print variables, attributes, metadata */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_var_utl.h" /* Variable utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
nco_bool /* if false then we are at the end of the slab */
nco_msa_clc_idx
(nco_bool NORMALIZE,         /* Return indices of slab within the slab */
 lmt_msa_sct *lmt_a,         /* I list of lmts for each dimension  */
 long *indices,          /* I/O so routine can keep track of where its at */
 lmt_sct* lmt_out,      /* O  output hyperslab */
 int *slb);             /* slab which the above limit refers to */ 

void 
nco_msa_prn_idx    /* [fnc] Print multiple hyperslab indices  */
(lmt_msa_sct * lmt_lst); 
  
void 
nco_msa_clc_cnt    /* [fnc] Calculate size of  multiple hyperslab */ 
(lmt_msa_sct *lmt_lst); 

void
nco_msa_wrp_splt /* [fnc] Split wrapped dimensions */
(lmt_msa_sct *lmt_lst);
  
nco_bool           /* O return true if limits overlap */
nco_msa_ovl        /* [fnc] see if limits overlap */ 
(lmt_msa_sct *lmt_lst);

int               /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */ 
nco_cmp_lmt_srt   /* [fnc]  Sort comparison operator */
(const void *vp1,
 const void* vp2);

void             
nco_msa_qsort_srt  /* [fnc] sort limits by srt values  */
(lmt_msa_sct *lmt_lst);

void      
nco_msa_lmt_all_ntl     /* [fnc] Initilaize lmt_msa_sct's */ 
(int in_id,
 nco_bool MSA_USR_RDR,
 lmt_msa_sct **lmt_all_lst,
 int nbr_dmn_fl,
 lmt_sct** lmt,
 int lmt_nbr);

void *          /* O pointer to malloced slab */
nco_msa_rcr_clc /* Multi slab algorithm (recursive routine) */
(int i,             /* current depth, we start at 0 */
 int imax,          /* maximum depth (i.e the number of dims in variable (does not change)*/		 
 lmt_sct **lmt,    /* limits of the current hyperslab these change as we recurse */
 lmt_msa_sct **lmt_lst, /* list of limits in each dimension (this remains STATIC as we recurse */
 var_sct *var1);    /* Infor for routine to read var (should not change */
  
long /* O [idx] Minimum value */
nco_msa_min_idx /* [fnc] Find minimum values in current and return minimum value */
(const long * const current, /* I [idx] Current indices */
 nco_bool * const mnm, /* O [flg] Minimum */
 const int size); /* I [nbr] Size of current and min */

void            
nco_msa_ram_2_dsk   /* [fnc] Convert hyperlsab indices into indices relative to disk */ 
(long *dmn_sbs_ram,   /* Input indices */
 lmt_msa_sct** lmt_msa,   /* input hyperlab limits     */
 int nbr_dmn,         /* number of dimensions */    
 long *dmn_sbs_dsk,  /* Output - indices relative to disk */
 nco_bool flg_free);        /* Free static space on last call */

void
nco_msa_var_get  /* [fnc] Get var data from disk taking accound of multihyperslabs */
(const int in_id,  /* I [id] netCDF input file ID */
 var_sct *var_in, 
 lmt_msa_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl); /* I [nbr] Number of multi-hyperslab limits */

void
nco_msa_var_val_cpy /* [fnc] Copy variables data from input to output file */
(const int in_id, /* I [enm] netCDF file ID */
 const int out_id, /* I [enm] netCDF output file ID */
 var_sct ** const var, /* I/O [sct] Variables to copy to output file */
 const int nbr_var,  /* I [nbr] Number of variables */
 lmt_msa_sct * const * lmt_lst, /* I multi-hyperslab limits */
 int nbr_dmn_fl); /* I [nbr] Number of multi-hyperslab limits */

void
nco_msa_wrp_splt_trv    /* [fnc] Split wrapped dimensions (traversal table version) */
(dmn_trv_sct *dmn_trv); /* [sct] Dimension structure from traversal table */

void 
nco_msa_clc_cnt_trv     /* [fnc] Calculate size of  multiple hyperslab (traversal table version) */ 
(dmn_trv_sct *dmn_trv); /* [sct] Dimension structure from traversal table */

nco_bool                /* O [flg] return true if limits overlap (traversal table version) */
nco_msa_ovl_trv         /* [fnc] See if limits overlap */ 
(dmn_trv_sct *dmn_trv); /* [sct] Dimension structure from traversal table */

void             
nco_msa_qsort_srt_trv   /* [fnc] Sort limits by srt values (traversal table version) */
(dmn_trv_sct *dmn_trv); /* [sct] Dimension structure from traversal table */

void
nco_msa_wrp_splt_cpy    /* [fnc] Split wrapped dimensions (make deep copy of new wrapped limits) */
(lmt_msa_sct *lmt_lst); /* [sct] MSA */

void
nco_cpy_var_val_mlt_lmt_trv         /* [fnc] Copy variable data from input to output file */
(const int in_id,                   /* I [id] netCDF input file ID */
 const int out_id,                  /* I [id] netCDF output file ID */
 FILE * const fp_bnr,               /* I [flg] Unformatted binary output file handle */
 const md5_sct * const md5,         /* I [flg] MD5 Configuration */
 const trv_sct * const var_trv);    /* I [sct] Object to write (variable) */

void
nco_cpy_msa_lmt                     /* [fnc] Copy MSA struct from table to local function (print or write) */
(const trv_sct * const var_trv,     /* I [sct] Object to write (variable) */
 lmt_msa_sct ***lmt_msa);           /* O [sct] MSA array for dimensions */

void
nco_msa_var_get_trv                 /* [fnc] Define a 'var_sct' hyperslab fields from a GTT variable */
(const int nc_id,                   /* I [ID] netCDF file ID */
 var_sct *var_in,                   /* I/O [sct] Variable */
 const trv_tbl_sct * const trv_tbl);/* I [sct] GTT (Group Traversal Table) */

void
nco_msa_var_get_sct                 /* [fnc] Define a 'var_sct' hyperslab fields from a GTT variable ('trv_sct')*/
(const int nc_id,                   /* I [ID] netCDF file ID */
  var_sct *var_in,                  /* I/O [sct] Variable */
  const trv_sct * const var_trv);   /* I [sct] GTT variable */

void
nco_lmt_msa_free                    /* [fnc] Free MSA */
(const int nbr_dmn,                 /* I [nbr] Number of dimensions */
 lmt_msa_sct **lmt_msa);            /* I [sct] MSA */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MSA_H */
