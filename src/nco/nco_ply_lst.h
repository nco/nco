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
#include "nco_ply.h"    /* poly sct stuff */



#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* !__cplusplus */



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
poly_typ_enm pl_typ,
int *pl_nbr);    /* O [nbr] size  poly_sct */




poly_sct **
nco_poly_lst_free(
poly_sct **pl_lst,
int arr_nbr);


poly_sct **
nco_poly_lst_mk_vrl(   /* create overlap mesh  for crt */
poly_sct **pl_lst_in,
int pl_cnt_in,
poly_sct **pl_lst_out,
int pl_cnt_out,
int *pl_cnt_vrl_ret);

poly_sct **
nco_poly_lst_mk_vrl_sph(   /* create overlap mesh  for sph */
poly_sct **pl_lst_in,
int pl_cnt_in,
poly_sct **pl_lst_out,
int pl_cnt_out,
nco_grd_lon_typ_enm grd_lon_typ,
int *pl_cnt_vrl_ret);




#ifdef __cplusplus
} /* end extern "C" */
#endif /* !__cplusplus */

#endif /* NCO_PLY_LST_H */

