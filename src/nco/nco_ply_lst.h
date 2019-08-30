/* $Header$ */

/* Purpose: Description (definition) of polygon list manipulation functions */

/* Copyright (C) 2018--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_ply_lst.h" *//* Polygon list manipulation */

#ifndef NCO_PLY_LST_H /* Contents have not yet been inserted in current source file */
#define NCO_PLY_LST_H

#include  <stdlib.h>
#include  <stdio.h>
#include  <math.h>

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h"     /* Memory management */
#include "nco_omp.h"     /* OpenMP utilities */
#include "nco_rgr.h"     /* Regridding */
#include "nco_sld.h"     /* Swath-Like Data */
#include "nco_sng_utl.h" /* String utilities */

#include "nco_crt.h" /* Cartesian geometry intersections */
#include "nco_kd.h" /* K-d tree geometric data structure */
#include "nco_ply.h" /* Polygon structure & utilities */

#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* !__cplusplus */

/* used in nco_ply_lst_vrl - for realloc to expand */
#define NCO_VRL_BLOCKSIZE 4000

typedef struct {
    poly_sct **pl_lst;
    size_t pl_cnt;
    size_t blk_nbr;
    KDPriority *kd_list;
} omp_mem_sct;

/************************ functions that manipulate lists of polygons ****************************************************/

void
nco_poly_re_org_lst(  /* for each poly_sct*  in list re-order points so that first point is the leftermost point */
poly_sct **pl_lst,
int arr_nbr);

poly_sct**             /* [O] [nbr] Array of poly_sct */
nco_poly_lst_mk(
double *area, /* I [sr] Area of source grid */
int *msk, /* I [flg] Mask on source grid */
double *lat_ctr, /* I [dgr] Latitude  centers of source grid */
double *lon_ctr, /* I [dgr] Longitude centers of source grid */
double *lat_crn, /* I [dgr] Latitude  corners of source grid */
double *lon_crn, /* I [dgr] Longitude corners of source grid */
size_t grd_sz, /* I [nbr] Number of elements in single layer of source grid */
long grd_crn_nbr, /* I [nbr] Maximum number of corners in source gridcell */
nco_grd_lon_typ_enm grd_lon_typ, /* I [num] if not nil then split cells that straddle Greenwich or Dateline  */
poly_typ_enm pl_typ,
int *pl_nbr);    /* O [nbr] size  poly_sct */

poly_sct**             /* [O] [nbr] Array of poly_sct */
nco_poly_lst_mk_sph(
double *area, /* I [sr] Area of source grid */
int *msk, /* I [flg] Mask on source grid */
double *lat_ctr, /* I [dgr] Latitude  centers of source grid */
double *lon_ctr, /* I [dgr] Longitude centers of source grid */
double *lat_crn, /* I [dgr] Latitude  corners of source grid */
double *lon_crn, /* I [dgr] Longitude corners of source grid */
size_t grd_sz, /* I [nbr] Number of elements in single layer of source grid */
long grd_crn_nbr, /* I [nbr] Maximum number of corners in source gridcell */
nco_grd_lon_typ_enm grd_lon_typ, /* I [num] if not nil then split cells that straddle Greenwich or Dateline  */
poly_typ_enm pl_typ);    /* O [nbr] size  poly_sct */




poly_sct **
nco_poly_lst_free(
poly_sct **pl_lst,
int arr_nbr);


poly_sct **
nco_poly_lst_mk_vrl(   /* create overlap mesh  for crt */
poly_sct **pl_lst_in,
int pl_cnt_in,
KDTree *rtree,
int *pl_cnt_vrl_ret);


poly_sct **
nco_poly_lst_mk_vrl_sph(  /* create overlap mesh  for sph polygons */
poly_sct **pl_lst_in,
int pl_cnt_in,
nco_grd_lon_typ_enm grd_lon_typ,
KDTree *rtree,
int *pl_cnt_vrl_ret);


void
nco_poly_set_priority(
int nbr_lst,
KDPriority *list);


/* create list for debugging */
poly_sct **
nco_poly_lst_chk_dbg(
poly_sct **pl_lst,
int pl_cnt,
poly_sct **pl_lst_vrl,
int pl_cnt_vrl,
int io_flg,  /* [flg] 0 - use src_id from vrl, 1 - use dst_id from vrl */
int *pl_cnt_dbg); /* size of output dbg grid */


/* check areas */
void nco_poly_lst_chk(
poly_sct **pl_lst_in,
int pl_cnt_in,
poly_sct **pl_lst_out,
int pl_cnt_out,
poly_sct **pl_lst_vrl,
int pl_cnt_vrl);



#ifdef __cplusplus
} /* end extern "C" */
#endif /* !__cplusplus */

#endif /* NCO_PLY_LST_H */

