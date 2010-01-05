/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_aux.h,v 1.8 2010-01-05 20:02:17 zender Exp $ */

/* Purpose: Sub-set cell-based grids using auxillary coordinate variable */

/* Copyright (C) 1995--2010 Charlie Zender
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
 int *lmt_nbr
 );


int 
nco_find_lat_lon
(int ncid, 
 char latvar[], 
 char lonvar[], 
 char **units,
 int *latid,
 int *lonid,
 nc_type *coordtype
 );


int
nco_getdmninfo
(int ncid,
 int varid,
 char dimname[],
 int *dimid,
 long *dmnsz
);

void 
nco_aux_prs
(const char *args, 
 const char *units, 
 float *lllon, 
 float *lllat, 
 float *urlon, 
 float *urlat
 );

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_AUX_H */
