/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_aux.h,v 1.15 2013-01-13 06:07:47 zender Exp $ */

/* Purpose: Sub-set cell-based grids using auxiliary coordinate variable */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_aux.h" *//* Auxiliary coordinates */

#ifndef NCO_AUX_H
#define NCO_AUX_H

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_lmt.h" /* Hyperslab limits */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

lmt_sct **
nco_aux_evl
(int in_id, 
 int aux_nbr, 
 char *aux_arg[],
 int *lmt_nbr);

int
nco_find_lat_lon
(int ncid, 
 char var_nm_lat[], 
 char var_nm_lon[], 
 char **units,
 int *lat_id,
 int *lon_id,
 nc_type *crd_typ);

int
nco_get_dmn_info
(int ncid,
 int varid,
 char dimname[],
 int *dimid,
 long *dmn_sz);

void 
nco_aux_prs
(const char *bnd_bx_sng, 
 const char *units, 
 float *lon_min, 
 float *lon_max, 
 float *lat_min, 
 float *lat_max);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_AUX_H */
