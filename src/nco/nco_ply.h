/* $Header$ */

/* Purpose: Description (definition) of polygon functions */

/* Copyright (C) 2018--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_ply.h" *//* Polygon structure & utilities */

#ifndef NCO_PLY_H
#define NCO_PLY_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* atof, atoi, malloc, getopt */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */
#include "nco_omp.h" /* OpenMP utilities */
#include "nco_rgr.h" /* Regridding */
#include "nco_sld.h" /* Swath-Like Data */
#include "nco_sng_utl.h" /* String utilities */

#include "nco_crt.h" /* Cartesian geometry intersections */
#include "nco_kd.h" /* K-d tree geometric data structure */
#include "nco_sph.h" /* Spherical geometry intersections */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define CORNER_MAX 100

/* Filter-out cells larger than below */
#define CELL_LATITUDE_MAX  30.0
#define CELL_LONGITUDE_MAX 180.0

#define DEBUG_VRL (0)

  poly_sct *
  nco_poly_free
  (poly_sct *pl);

  poly_sct *   
  nco_poly_init
  (void);

  poly_sct *
  nco_poly_init_lst
  (poly_typ_enm pl_typ,
   int arr_nbr,
   int mem_flg,
   int src_id,
   double *dp_x_in,
   double *dp_y_in);

  poly_sct *
  nco_poly_init_crn
  (poly_typ_enm pl_typ,
  int crn_nbr_in,
  int src_id);

  void
  nco_poly_init_crn_re  /* re malloc crn_nbr to a new size */
  (poly_sct *pl,
   int new_crn_nbr);

  poly_sct*
  nco_poly_dpl
  (poly_sct *pl);

  void
  nco_poly_minmax_add
  (poly_sct *pl,
   nco_grd_lon_typ_enm grd_lon_typ,
   nco_bool bchk_caps);

  void
  nco_poly_minmax_use_crn /* use the values of minmax box as dp_x, dp_y  */
  (poly_sct *pl);
  
  void
  nco_poly_prn
  (poly_sct *pl, int style);
  
  poly_sct*
  nco_poly_vrl_do(
  poly_sct *pl_in,
  poly_sct *pl_out,
  int flg_snp_to,
  char *sp_sng);

  nco_bool                  /* [flg] return True if pl_out inside pl_in */
  nco_poly_in_poly_minmax(  /* uses minmax limits only no serious polygon stuff */
  poly_sct *pl_in,
  poly_sct *pl_out
  );
  
  nco_bool
  nco_poly_wrp_splt( 
  poly_sct  *pl,
  nco_grd_lon_typ_enm grd_lon_typ,
  poly_sct **pl_wrp_left,
  poly_sct ** pl_wrp_right);

  nco_bool
  nco_poly_wrp_splt180(
  poly_sct  *pl,
  poly_sct **pl_wrp_left,
  poly_sct ** pl_wrp_right);

  nco_bool
  nco_poly_wrp_splt360(
  poly_sct  *pl,
  poly_sct **pl_wrp_left,
  poly_sct ** pl_wrp_right);


  void
  nco_poly_re_org(
  poly_sct *pl, 		  
  double *lcl_dp_x,
  double *lcl_dp_y);

  nco_bool  /* 0 [flg] True if polygon is convex */
  nco_poly_is_convex(
  poly_sct *pl);

  void
  nco_poly_shp_init(  /* allocate shp to sph[crn_nbr][2] or shp[crn_nbr][5] */
  poly_sct *pl);

  void
  nco_poly_shp_free(  /* allocate shp to sph[crn_nbr][2] or shp[crn_nbr][5] */
  poly_sct *pl);

  void
  nco_poly_shp_pop(  /* fill out sph with values from dp_x, and dp_y */
  poly_sct *pl);

  void
  nco_poly_dp_pop_shp(  /* fill out dp_x, dp_y with values from shp */
  poly_sct *pl
  );

  size_t
  nco_poly_sizeof(
  poly_sct *pl);

  nco_bool
  nco_poly_minmax_split(
  poly_sct *pl,
  nco_grd_lon_typ_enm nco_grd_typ,
  kd_box size1,
  kd_box size2);

  nco_grd_lon_typ_enm
  nco_poly_minmax_2_lon_typ
  (poly_sct *pl);

  void
  nco_poly_ctr_add
  (poly_sct *pl,
   nco_grd_lon_typ_enm lon_typ);

  int
  nco_poly_intersect(
   poly_sct *pl_in,
   poly_sct *pl_out,
   poly_sct *pl_vrl,
   int *r);

  const char *
  nco_poly_typ_sng_get(
  poly_typ_enm pl_typ
  );

int
nco_poly_typ_sz(poly_typ_enm pl_typ);
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_PLY_H  */
