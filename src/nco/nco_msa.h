/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_msa.h,v 1.1 2002-12-19 20:48:25 zender Exp $ */

/* Purpose: Multi-slabbing algorithm */

/* Copyright (C) 1995--2002 Charlie Zender and Henry Butowsky
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_var_utl.h" *//* Multi-slabbing algorithm */

#ifndef NCO_VAR_UTL_H
#define NCO_VAR_UTL_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <limits.h> /* need LONG_MAX */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_bnr.h" /* Binary write utilities */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

 void
  nco_cpy_var_val_multi_lmt /* [fnc] Copy variable data from input to output file */
  (const int in_id, /* I [id] netCDF input file ID */
   const int out_id, /* I [id] netCDF output file ID */
   FILE * const fp_bnr, /* I [fl] Unformatted binary output file handle */
   const bool NCO_BNR_WRT, /* I [flg] Write binary file */
   char *var_nm, /* I [sng] Variable name */
   const lmt_all * const lmt_lst, /* I multi-hyperslab limits */
   const int nbr_dmn_fl); /* I [nbr] Number of multi-hyperslab limits */


  void *
  nco_msa_merge_slabs(
  int i,               /* current depth, we start at 0 */
  int imax,            /* maximium depth (i.e the number of dims in variable (does not change)*/	
  lmt_all **lmt_lst,   /* list of limits in each dimension (this remains STATIC as we recurse) */
  void **vp_wrap,     /* pointers to hyperslabs */ 
  long *vp_size,      /* size of each hyperslab */
  long *var_sz,       /* Var size of merged hyperslab */ 
  var_sct *vara);      /* hold data about current variable */

  
  bool 
  nco_msa_calc_indices(   /* if false then this is the last limit out */
  bool NORMALIZE,         /* Return indices of slab within the slab */
  lmt_all *lmt_a,         /* I list of lmts for each dimension  */
  long *indices,          /* I/O so routine can keep track of where its at */
  lmt_sct  *lmt_out,      /* O  output hyperslab */
  int *slb );             /* slab which the above limit refers to */ 

  
  void 
  nco_msa_print_indices    /* Calculate indices for multiple hyperslabbing */
  (lmt_all * lmt_lst); 

  void 
  nco_msa_calc_cnt    /* Calculate indices for multiple hyperslabbing */
  (lmt_all * lmt_lst); 

  void *
  nco_msa_rec_calc(  /* Multi slab algorithm (recursive routine, returns a single slab pointer */
  int i,             /* current depth, we start at 0 */
  int imax,          /* maximium depth (i.e the number of dims in variable (does not change)*/		 
  lmt_sct **lmt,    /* limits of the current hyperslab these change as we recurse */
  lmt_all **lmt_lst, /* list of limits in each dimension (this remains STATIC as we recurse */
  var_sct *var1);    /* Infor for routine to read var (should not change */

  long
  nco_msa_min_indices( /* find min values in current and return the min value*/
  long *current,   /* current indices */
  bool *min,       /* element true if a minimum */
  int size);       /* size of current and min */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_VAR_UTL_H */
