/* $Header$ */

/* Purpose: Description (definition) of map-generation functions */

/* Copyright (C) 2015--2017 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  int /* O [enm] Return code */
  nco_map_mk /* [fnc] Create ESMF-format map file */
  (rgr_sct * const rgr); /* I/O [sct] Regridding structure */

  int /* O [enm] Return code */
  nco_msh_mk /* [fnc] Compute overlap mesh and weights */
  (rgr_sct * const rgr, /* I [sct] Regridding structure */
   
   double *area_in, /* I [sr] Area of source grid */
   int *msk_in, /* I [flg] Mask on source grid */
   double *lat_ctr_in, /* I [dgr] Latitude  centers of source grid */
   double *lon_ctr_in, /* I [dgr] Longitude centers of source grid */
   double *lat_crn_in, /* I [dgr] Latitude  corners of source grid */
   double *lon_crn_in, /* I [dgr] Longitude corners of source grid */
   size_t grd_sz_in, /* I [nbr] Number of elements in single layer of source grid */
   long grd_crn_nbr_in, /* I [nbr] Maximum number of corners in source gridcell */
   
   double *area_out, /* I [sr] Area of destination grid */
   int *msk_out, /* I [flg] Mask on destination grid */
   double *lat_ctr_out, /* I [dgr] Latitude  centers of destination grid */
   double *lon_ctr_out, /* I [dgr] Longitude centers of destination grid */
   double *lat_crn_out, /* I [dgr] Latitude  corners of destination grid */
   double *lon_crn_out, /* I [dgr] Longitude corners of destination grid */
   size_t grd_sz_out, /* I [nbr] Number of elements in single layer of destination grid */
   long grd_crn_nbr_out, /* I [nbr] Maximum number of corners in destination gridcell */
   
   double *frc_in, /* O [frc] Fraction of source grid */
   double *frc_out, /* O [frc] Fraction of destination grid */
   int **col_src_adr_ptr, /* O [idx] Source address (col) */
   int **row_dst_adr_ptr, /* O [idx] Destination address (row) */
   double **wgt_raw_ptr, /* O [frc] Remapping weights */ 
   size_t *lnk_nbr_ptr); /* O [nbr] Number of links */
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MAP_H */
