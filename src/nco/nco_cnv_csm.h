/* $Header$ */

/* Purpose: CCM/CCSM/CF conventions */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_cnv_csm.h" *//* CCM/CCSM/CF conventions */

#ifndef NCO_CNV_CSM_H
#define NCO_CNV_CSM_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cln_utl.h" /* Calendar utilities */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* Climatology bounds structure (20160503: Used only in ncra.c for climos) */
  typedef struct{ /* clm_bnd_sct */
    char *bnd_dmn_nm; /* [sng] Bounds dimension name name */
    char *clm_bnd_nm; /* [sng] Climatology bounds variable name (to create) */
    char *tm_bnd_nm; /* [sng] Time bounds variable name (to delete) */
    char *tm_crd_nm; /* [sng] Name of time coordinate variable */
    char *cln_val; /* [sng] Bounds calendar value */
    char *unt_val; /* [sng] Bounds units value */
    double *tm_val; /* [frc] Time coordinate variable values */
    double *bnd_val; /* [frc] Time (or climatology) bounds variable values */
    int clm_bnd_id_in; /* [id] Variable ID for clm_bnds in input */
    int clm_bnd_id_out; /* [id] Variable ID for clm_bnds in output */
    int dmn_ids[2]; /* [idx] Dimension IDs for new bounds variable */
    int mth_end; /* [mth] Month at climo end [1..12] format */
    int mth_srt; /* [mth] Month at climo start [1..12] format */
    int tm_bnd_id_in; /* [id] Variable ID for tm_bnds in input */
    int tm_bnd_id_out; /* [id] Variable ID for tm_bnds in output */
    int tm_crd_id_in; /* [id] Variable ID for tm_crd in input */
    int tm_crd_id_out; /* [id] Variable ID for tm_crd in output */
    int tpd; /* [nbr] Timesteps per day [0=none, 1, 2, 3, 4, 6, 8,  12, 24, ...] */
    int yr_end; /* [yr] Year at climo start */
    int yr_srt; /* [yr] Year at climo start */
    long dmn_srt_srt[2]; /* [idx] Start indices for retrieving start bounds */
    long dmn_srt_end[2]; /* [idx] Start indices for retrieving end bounds */
    nc_type type; /* [enm] Type of (time and) climatology bounds variable(s) */
    nco_bool bnd2clm; /* [flg] Convert time bounds to climatology bounds */
    nco_bool clm2bnd; /* [flg] Convert climatology bounds to time bounds */
    nco_bool clm2clm; /* [flg] Convert climatology bounds to climatology bounds */
    nco_bool clm_bnd_in; /* [flg] Climatology bounds appear in input */
    nco_bool tm_bnd_in; /* [flg] Time bounds appear in input */
  } clm_bnd_sct; /* end climatology bounds structure */

  int /* O [rcd] Return code */
  nco_clm_nfo_get /* [fnc] Parse clm_nfo arguments and merge into structure */
  (const char *clm_nfo_sng, /* I [sng] Climatology information string */
   clm_bnd_sct *cb); /* I/O [sct] Climatology bounds structure */
  
  cnv_sct * /* O [sct] Convention structure */
  nco_cnv_ini /* [fnc] Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
  (const int nc_id); /* I [id] netCDF file ID */
  
  nco_bool /* O [flg] File obeys CCM/CCSM/CF conventions */
  nco_cnv_ccm_ccsm_cf_inq /* [fnc] Check if file obeys CCM/CCSM/CF conventions */
  (const int nc_id); /* I [id] netCDF file ID */
  
  void
  nco_cnv_ccm_ccsm_cf_date /* [fnc] Fix date variable in averaged CCSM files */
  (const int nc_id, /* I [id] netCDF file ID */
   X_CST_PTR_CST_PTR_Y(var_sct,var), /* I/O [sct] Variables in output file */
   const int nbr_var); /* I [nbr] Number of variables in list */
  
  nm_id_sct * /* O [sct] Extraction list */
  nco_cnv_cf_crd_add /* [fnc] Add coordinates defined by CF convention */
  (const int nc_id, /* I netCDF file ID */
   nm_id_sct *xtr_lst, /* I/O current extraction list (destroyed) */
   int * const xtr_nbr); /* I/O number of variables in current extraction list */
  
  int                                  /* [rcd] Return code */
  nco_cnv_cf_cll_mth_add               /* [fnc] Add cell_methods attributes */
  (const int out_id,                   /* I [id] netCDF file ID */
   var_sct * const * const var,        /* I [sct] Variable to reduce (e.g., average) (destroyed) */
   const int var_nbr,                  /* I [nbr] Number of variables to be defined */
   dmn_sct * const * const dmn,        /* I [sct] Dimensions over which to reduce variable */
   const int dmn_nbr,                  /* I [sct] Number of dimensions to reduce variable over */
   const int nco_op_typ,               /* I [enm] Operation type, default is average */
   gpe_sct *gpe,                       /* I [sng] Group Path Editing (GPE) structure */
   const clm_bnd_sct * const cb,       /* I [sct] Climatology bounds structure */
   const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */
  
  int /* I [enm] Operation type */
  nco_rdc_sng_to_op_typ /* [fnc] Convert operation string to integer */
  (const char * const att_op_sng); /* I [sng] Operation string */
  
  const char * /* O [sng] String describing operation type */
  nco_op_typ_to_rdc_sng /* [fnc] Convert operation type to string */
  (const int nco_op_typ); /* I [enm] Operation type */
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNV_CSM_H */
