/* $Header$ */

/* Purpose: Hyperslab limits */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_lmt.h" *//* Hyperslab limits */

#ifndef NCO_LMT_H
#define NCO_LMT_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */
#ifdef HAVE_STRINGS_H
# include <strings.h> /* strcasecmp() */
#endif /* !HAVE_STRINGS_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_cln_utl.h" /* Multi-Calendar functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

enum lmt_typ{ /* [enm] Limit type */
 lmt_crd_val, /* 0, Coordinate value limit */
 lmt_dmn_idx, /* 1, Dimension index limit */
 lmt_udu_sng /* 2, UDUnits string */
}; /* end lmt_typ enum */

void
nco_lmt_init /* [fnc] Initialize limit to NULL/default values */
(lmt_sct *lmt); /* I/O [sct] Limit structure to initialize */

void
nco_lmt_prn /* [fnc] Print a Limit structure */
(lmt_sct *lmt); /* I/O [sct] Limit structure to print */

void
nco_lmt_cpy /* [fnc] Deep-copy a Limit structure */
(const lmt_sct * const lmt1, /* I [sct] Limit structure to copy */
 lmt_sct *lmt2);/* O [sct] New limit structure */

void
nco_lmt_evl /* [fnc] Parse user-specified limits into hyperslab specifications */
(const int grp_id, /* I [idx] netCDF group ID */
 lmt_sct *lmt_ptr, /* I/O [sct] Structure from nco_lmt_prs() or from nco_lmt_sct_mk() to hold dimension limit information */
 long cnt_crr, /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 nco_bool FORTRAN_IDX_CNV); /* I [flg] Hyperslab indices obey Fortran convention */

lmt_sct * /* O [sct] Pointer to free'd structure */
nco_lmt_free /* [fnc] Free memory associated with limit structure */
(lmt_sct *lmt); /* I/O [sct] Limit structure to free */

char * /* O [sng] Units string */
nco_lmt_get_udu_att /* Successful conversion returns units attribute otherwise null */
(const int nc_id, /* I [idx] netCDF file ID */
 const int var_id,
 const char *att_nm); /* I [id] Variable ID whose attribute to read */

lmt_sct ** /* O [sct] Pointer to free'd structure list */
nco_lmt_lst_free /* [fnc] Free memory associated with limit structure list */
(lmt_sct **lmt_lst, /* I/O [sct] Limit structure list to free */
 const int lmt_nbr); /* I [nbr] Number of limit structures in list */

lmt_msa_sct * /* O [sct] Pointer to free'd structure */
nco_lmt_all_free /* [fnc] Free memory associated with lmt_all structure */
(lmt_msa_sct *lmt_all); /* I/O [sct] Limit structure to free */

lmt_msa_sct ** /* O [sct] Pointer to free'd structure list */
nco_lmt_all_lst_free /* [fnc] Free memory associated with lmt_all structure list */
(lmt_msa_sct **lmt_all_lst, /* I/O [sct] Limit structure list to free */
 const int lmt_all_nbr); /* I [nbr] Number of limit structures in list */

lmt_sct ** /* O [sct] Structure list with user-specified strings for min and max limits */
nco_lmt_prs /* [fnc] Create limit structures with name, min_sng, max_sng elements */
(const int lmt_nbr, /* I [nbr] number of dimensions with limits */
 CST_X_PTR_CST_PTR_CST_Y(char,lmt_arg)); /* I [sng] List of user-specified dimension limits */

lmt_sct * /* [sct] Limit structure for dimension */
nco_lmt_sct_mk /* [fnc] Create stand-alone limit structure for given dimension */
(const int nc_id, /* I [idx] netCDF file ID */
 const int dmn_id, /* I [idx] ID of dimension for this limit structure */
 CST_X_PTR_CST_PTR_CST_Y(lmt_sct,lmt), /* I [sct] Array of limit structures from nco_lmt_evl() */ 
 int lmt_nbr, /* I [nbr] Number of limit structures */
 const nco_bool FORTRAN_IDX_CNV); /* I [flg] Hyperslab indices obey Fortran convention */

int /* O [enm] Limit type */
nco_lmt_typ /* [fnc] Determine limit type */
(char *sng);/* I [ptr] Pointer to limit string */

void
nco_prn_lmt                    /* [fnc] Print limit information */
(lmt_sct lmt,                  /* I [sct] Limit structure */
 int min_lmt_typ,              /* I [nbr] Limit type */
 nco_bool FORTRAN_IDX_CNV,     /* I [flg] Hyperslab indices obey Fortran convention */
 nco_bool flg_no_data_ok,      /* I [flg] True if file contains no data for hyperslab */
 long rec_usd_cml,             /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 monotonic_direction_enm monotonic_direction, /* I [enm] Monotonic_direction */
 nco_bool rec_dmn_and_mfo,     /* I [flg] True if record dimension in multi-file operator */
 long cnt_rmn_ttl,             /* I [nbr] Total records to be read from this and all remaining files */
 long cnt_rmn_crr,             /* I [nbr] Records to extract from current file */
 long rec_skp_vld_prv_dgn);    /* I [nbr] Records skipped at end of previous valid file, if any (diagnostic only) */

void                      
nco_lmt_evl_dmn_crd            /* [fnc] Parse user-specified limits into hyperslab specifications */
(const int nc_id,              /* I [ID] netCDF file ID */
 long rec_usd_cml,             /* I [nbr] Number of valid records already processed (only used for record dimensions in multi-file operators) */
 nco_bool FORTRAN_IDX_CNV,     /* I [flg] Hyperslab indices obey Fortran convention */
 const char * const grp_nm_fll,/* I [sng] Full group name (dimension or coordinate) */
 const char * const nm,        /* I [sng] Name (dimension or coordinate) */
 const size_t sz,              /* I [nbr] Size (dimension or coordinate) */
 const nco_bool is_rec,        /* I [flg] Is a record (dimension or coordinate) ? */
 const nco_bool is_crd,        /* I [flg] Is a coordinate variable ? */
 lmt_sct *lmt_ptr);            /* I/O [sct] Structure from nco_lmt_prs() in input, filled on output  */
 



#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_LMT_H */
