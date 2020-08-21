/* $Header$ */

/* Purpose: Description (definition) of map-generation functions */

/* Copyright (C) 2015--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_map.h" *//* Map generation */

#ifndef NCO_MAP_H
#define NCO_MAP_H

/* Standard header files */
#include <limits.h> /* INT_MAX, LONG_MAX */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* atof, atoi, malloc, getopt */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */
#include "nco_omp.h" /* OpenMP utilities */
#include "nco_rgr.h" /* Regridding */
#include "nco_sld.h" /* Swath-Like Data */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_ply_lst.h"  /* polygon list functions */
#include "nco_ply.h"     /* polygon functions */
#include "nco_kd.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  int /* O [enm] Return code */
  nco_map_mk /* [fnc] Create ESMF-format map file */
  (rgr_sct * const rgr); /* I/O [sct] Regridding structure */
  
  int /* O [enm] Return code */
  nco_msh_mk /* [fnc] Compute overlap mesh and weights */
  (rgr_sct * const rgr, /* I [sct] Regridding structure */
   nco_mpf_sct *mpf,
   double *area_in, /* I [sr] Area of source grid */
   int *msk_in, /* I [flg] Mask on source grid */
   double *lat_ctr_in, /* I [dgr] Latitude  centers of source grid */
   double *lon_ctr_in, /* I [dgr] Longitude centers of source grid */
   double *lat_crn_in, /* I [dgr] Latitude  corners of source grid */
   double *lon_crn_in, /* I [dgr] Longitude corners of source grid */
   
   double *area_out, /* I [sr] Area of destination grid */
   int *msk_out, /* I [flg] Mask on destination grid */
   double *lat_ctr_out, /* I [dgr] Latitude  centers of destination grid */
   double *lon_ctr_out, /* I [dgr] Longitude centers of destination grid */
   double *lat_crn_out, /* I [dgr] Latitude  corners of destination grid */
   double *lon_crn_out, /* I [dgr] Longitude corners of destination grid */
   
   double *frc_in, /* O [frc] Fraction of source grid */
   double *frc_out, /* O [frc] Fraction of destination grid */
   int **col_src_adr_ptr, /* O [idx] Source address (col) */
   int **row_dst_adr_ptr, /* O [idx] Destination address (row) */
   double **wgt_raw_ptr, /* O [frc] Remapping weights */ 
   size_t *lnk_nbr_ptr); /* O [nbr] Number of links */
  
  poly_sct *           /* return a ply_sct with lat/lon minmax and total area */
  nco_msh_stats
  (double *area,       /* I [sr] Area of  grid */
   int *msk,           /* I [flg] Mask on  grid */
   double *lat_ctr,    /* I [dgr] Latitude  centers of  grid */
   double *lon_ctr,    /* I [dgr] Longitude centers of  grid */
   double *lat_crn,    /* I [dgr] Latitude  corners of  grid */
   double *lon_crn,    /* I [dgr] Longitude corners of  grid */
   size_t grd_sz,      /* I [nbr] Number of elements in single layer of  grid */
   long grd_crn_nbr); /* I [nbr] Maximum number of corners in  gridcell */
  
  void
  nco_msh_lon_crr
  (double *lon_crn,      /* I/O longitude to be corrected */
   size_t grd_sz_in,     /* I [nbr] Number of elements in single layer of source grid */
   long grd_crn_nbr_in,  /* I [nbr] Maximum number of corners in source gridcell */
   nco_grd_lon_typ_enm typ_in,
   nco_grd_lon_typ_enm typ_out);
  
  void
  nco_msh_poly_lst_wrt
  (const char *fl_out,
   poly_sct ** pl_lst,
   int pl_nbr,
   nco_grd_lon_typ_enm grd_lon_typ,
   int fl_out_fmt
   );
  
  int
  nco_msh_att_char
  (int out_id,
   int var_id,
   const char *var_nm,
   const char *att_nm,
   const char *att_val
   );

  nco_bool
  nco_map_hst_mk
  (var_sct* var_row,
   int row_max,
   int hst_ar[],
   int hst_sz);
  
  void
  nco_map_var_min_max_ttl
  (var_sct *var,
   double *area,
   nco_bool flg_area_wgt,
   double *min,
   size_t *idx_min,
   double *max,
   size_t *idx_max,
   double *ttl,
   double *avg,
   double *mebs,
   double *rms,
   double *sdn);
  
  nco_bool
  nco_map_chk
  (const char *fl_in,
   nco_bool flg_frac_b_nrm,
   nco_bool flg_area_wgt);
  
  nco_bool
  nco_map_frac_b_clc   /* calculate frac from mapping weights */
  (var_sct *var_S,
   var_sct *var_row,
   var_sct *var_frac_b);
  
  nco_bool
  nco_map_frac_a_clc
  (var_sct *var_S,
   var_sct *var_row,
   var_sct *var_col,
   var_sct *var_area_a,
   var_sct *var_area_b,
   var_sct *var_frac_a);

  /* create tree */
  KDTree**
  nco_map_kd(
  poly_sct **pl_lst,
  int pl_cnt,
  nco_grd_lon_typ_enm grd_lon_typ,
  int *nbr_tr);

  KDTree *
  nco_map_kd_init
  (poly_sct **pl_lst,
   int pl_cnt,
   nco_grd_lon_typ_enm grd_lon_typ);

  void
  nco_poly_area_add
  (poly_sct *pl);


#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MAP_H */
