/* $Header$ */

/* Purpose: Description (definition) of Sparse-1D (S1D) Dataset functions */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_s1d.h" *//* Sparse-1D CLM/ELM datasets */

#ifndef NCO_S1D_H
#define NCO_S1D_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* atof, atoi, malloc, getopt */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */
#include "nco_omp.h" /* OpenMP utilities */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* Types used in Sparse-1D structure */
  typedef enum nco_s1d_typ_enm{ /* [enm] Sparse-1D type enum */
    nco_s1d_nil=0,
    nco_s1d_unk, /* Unknown or unclassified sparse-type */
    nco_s1d_clm, /* Sparse-1D Column (cols1d) format */
    nco_s1d_grd, /* Sparse-1D Gridcell (grid1d) format */
    nco_s1d_lnd, /* Sparse-1D Landunit (land1d) format */
    nco_s1d_pft, /* Sparse-1D PFT (pfts1d) format */
  } nco_s1d_typ_enm;

  const char * /* O [sng] String describing sparse-type */
  nco_s1d_sng /* [fnc] Convert sparse-type enum to string */
  (const nco_s1d_typ_enm nco_s1d_typ); /* I [enm] Sparse-type enum */

  int /* O [rcd] Return code */
  nco_s1d_unpack /* [fnc] Unpack contents of sparse-1D CLM/ELM file into full file */
  (rgr_sct * const rgr, /* I/O [sct] Regridding structure */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal Table */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_S1D_H */
