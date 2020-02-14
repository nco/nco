/* $Header$ */

/* Purpose: Sub-set cell-based grids using auxiliary coordinate variable */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

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
 int *lmt_nbr,
 char *nm_dmn);                 /* O [sng] Dimension name */ 

nco_bool
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
 double *lon_min, 
 double *lon_max, 
 double *lat_min, 
 double *lat_max);

lmt_sct **                           /* O [lst] Auxiliary coordinate limits */
nco_aux_evl_trv
(const int nc_id,                    /* I [ID] netCDF file ID */
 int aux_nbr,                        /* I [sng] Number of auxiliary coordinates */
 char *aux_arg[],                    /* I [sng] Auxiliary coordinates */
 trv_sct *lat_trv,                   /* I [sct] "latitude" variable */
 trv_sct *lon_trv,                   /* I [sct] "longitude" variable */
 const nc_type crd_typ,              /* I [nbr] netCDF type of both "latitude" and "longitude" */
 const char * const units,           /* I [sng] Units of both "latitude" and "longitude" */
 int *aux_lmt_nbr);                  /* I/O [nbr] Number of coordinate limits */

nco_bool 
nco_find_lat_lon_trv
(const int nc_id,                    /* I [ID] netCDF file ID */
 const trv_sct * const var_trv,      /* I [sct] Variable object that contains "standard_name" attribute */
 const char * const att_val_trg,     /* I [sng] Attribute value to find ( "latitude" or "longitude" ) */
 char **var_nm_fll,                  /* I/O [sng] Full name of variable that has "latitude" or "longitude" attributes */
 int *dmn_id,                        /* I/O [id] Dimension ID of the diension of "latitude" and "longitude" */
 nc_type *crd_typ,                   /* I/O [enm] netCDF type of both "latitude" and "longitude" */
 char units[]);                      /* I/O [sng] Units of both "latitude" and "longitude" */

nco_bool 
nco_check_nm_aux
(const int nc_id,                    /* I [ID] netCDF file ID */
 const trv_sct * const var_trv,      /* I [sct] Variable object that contains "standard_name" attribute */
 int *dmn_id,                        /* I/O [id] Dimension ID of the diension of "latitude" and "longitude" */
 nc_type *crd_typ,                   /* I/O [enm] netCDF type of both "latitude" and "longitude" */
 char units[]);                      /* I/O [sng] Units of both "latitude" and "longitude" */



  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_AUX_H */
