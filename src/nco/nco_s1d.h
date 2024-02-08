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

  /* Types used in column landunit structure */
  typedef enum nco_clm_typ_enm{ /* [enm] Landunit type enum */
    nco_clm_icol_nil=0, /* Unknown, unset, or unclassified landunit type */
    nco_clm_icol_vegetated_or_bare_soil=1, /* Vegetated or bare soil */
    nco_clm_icol_crop=2, /* Crop icol_crop_noncompete: 2*100+m, m=cft_lb,cft_ub */
    nco_clm_icol_crop_noncompete_01=201, /* Crop icol_crop_noncompete: 2*100+m, m=cft_lb,cft_ub */
    nco_clm_icol_crop_noncompete_02=202, /* Crop icol_crop_noncompete: 2*100+m, m=cft_lb,cft_ub */
    nco_clm_icol_landice=3, /* Landice (plain, no MEC) */
    nco_clm_icol_landice_multiple_elevation_classes=4, /* Landice multiple elevation classes: 4*100+m, m=1,glcnec */
    nco_clm_icol_landice_multiple_elevation_class_01=401, /* Landice multiple elevation class 01 */
    nco_clm_icol_landice_multiple_elevation_class_02=402, /* Landice multiple elevation class 02 */
    nco_clm_icol_landice_multiple_elevation_class_03=403, /* Landice multiple elevation class 03 */
    nco_clm_icol_landice_multiple_elevation_class_04=404, /* Landice multiple elevation class 04 */
    nco_clm_icol_landice_multiple_elevation_class_05=405, /* Landice multiple elevation class 05 */
    nco_clm_icol_landice_multiple_elevation_class_06=406, /* Landice multiple elevation class 06 */
    nco_clm_icol_landice_multiple_elevation_class_07=407, /* Landice multiple elevation class 07 */
    nco_clm_icol_landice_multiple_elevation_class_08=408, /* Landice multiple elevation class 08 */
    nco_clm_icol_landice_multiple_elevation_class_09=409, /* Landice multiple elevation class 09 */
    nco_clm_icol_landice_multiple_elevation_class_10=410, /* Landice multiple elevation class 10 */
    nco_clm_icol_deep_lake=5, /* Deep lake */
    nco_clm_icol_wetland=6, /* Wetland */
    nco_clm_icol_urban_roof=71, /* Urban roof */
    nco_clm_icol_urban_sunwall=72, /* Urban sunwall */
    nco_clm_icol_urban_shadewall=73, /* Urban shadewall */
    nco_clm_icol_urban_impervious_road=74, /* Urban impervious road */
    nco_clm_icol_urban_pervious_road=75, /* Urban pervious road */
  } nco_clm_typ_enm;

  /* Types used in landunit structure */
  typedef enum nco_lnd_typ_enm{ /* [enm] Landunit type enum */
    nco_lnd_ilun_nil=0, /* Unknown, unset, or unclassified landunit type */
    nco_lnd_ilun_vegetated_or_bare_soil=1, /* Vegetated or bare soil */
    nco_lnd_ilun_crop=2, /* Crop */
    nco_lnd_ilun_landice=3, /* Landice (plain, no MEC) */
    nco_lnd_ilun_landice_multiple_elevation_classes=4, /* Landice multiple elevation classes */
    nco_lnd_ilun_deep_lake=5, /* Deep lake */
    nco_lnd_ilun_wetland=6, /* Wetland */
    nco_lnd_ilun_urban_tbd=7, /* Urban tall building district */
    nco_lnd_ilun_urban_hd=8, /* Urban high density */
    nco_lnd_ilun_urban_md=9, /* Urban medium density */
  } nco_lnd_typ_enm;

  /* Types used in landunit structure */
  typedef enum nco_pft_typ_enm{ /* [enm] Landunit type enum */
    nco_pft_ipft_not_vegetated=0, /* Not vegetated */
    nco_pft_ipft_needleleaf_evergreen_temperate_tree=1, /* Needleleaf evergreen temperate tree */
    nco_pft_ipft_needleleaf_evergreen_boreal_tree=2, /* Needleleaf evergreen boreal tree */
    nco_pft_ipft_needleleaf_deciduous_boreal_tree=3, /* Needleleaf deciduous boreal tree */
    nco_pft_ipft_broadleaf_evergreen_tropical_tree=4, /* Broadleaf evergreen tropical tree */
    nco_pft_ipft_broadleaf_evergreen_temperate_tree=5, /* Broadleaf evergreen temperate tree */
    nco_pft_ipft_broadleaf_deciduous_tropical_tree=6, /* Broadleaf deciduous tropical tree */
    nco_pft_ipft_broadleaf_deciduous_temperate_tree=7, /* Broadleaf deciduous temperate tree */
    nco_pft_ipft_broadleaf_deciduous_boreal_tree=8, /* Broadleaf deciduous boreal tree */
    nco_pft_ipft_broadleaf_evergreen_shrub=9, /* Broadleaf evergreen shrub */
    nco_pft_ipft_broadleaf_deciduous_temperate_shrub=10, /* Broadleaf deciduous temperate shrub */
    nco_pft_ipft_broadleaf_deciduous_boreal_shrub=11, /* Broadleaf deciduous boreal shrub */
    nco_pft_ipft_c3_arctic_grass=12, /* C3 Arctic grass */
    nco_pft_ipft_c3_non_arctic_grass=13, /* C3 non-Arctic grass */
    nco_pft_ipft_c4_grass=14, /* C4 grass */
    nco_pft_ipft_c3_crop=15, /* C3 crop */
    nco_pft_ipft_c3_irrigated=16, /* C3 irrigated */
    nco_pft_ipft_corn=17, /* Corn */
    nco_pft_ipft_irrigated_corn=18, /* Irrigated corn */
    nco_pft_ipft_spring_temperate_cereal=19, /* Spring temperate cereal */
    nco_pft_ipft_irrigated_spring_temperate_cereal=20, /* Irrigated spring temperate cereal */
    nco_pft_ipft_winter_temperate_cereal=21, /* Winter temperate cereal */
    nco_pft_ipft_irrigated_winter_temperate_cereal=22, /* Irrigated winter temperate cereal */
    nco_pft_ipft_soybean=23, /* Soybean */
    nco_pft_ipft_irrigated_soybean=24, /* Irrigated soybean */
  } nco_pft_typ_enm;

  /* Types used in Sparse-1D structure */
  typedef enum nco_s1d_typ_enm{ /* [enm] Sparse-1D type enum */
    nco_s1d_nil=0,
    nco_s1d_unk, /* Unknown or unclassified sparse-type */
    nco_s1d_clm, /* Sparse-1D Column (cols1d) format */
    nco_s1d_grd, /* Sparse-1D Gridcell (grid1d) format */
    nco_s1d_lnd, /* Sparse-1D Landunit (land1d) format */
    nco_s1d_pft, /* Sparse-1D PFT (pfts1d) format */
  } nco_s1d_typ_enm;

  const char * /* O [sng] String describing column type */
  nco_clm_typ_sng /* [fnc] Convert column-type enum to string */
  (const int nco_clm_typ); /* I [enm] Column type enum */

  const char * /* O [sng] String describing landunit type */
  nco_lnd_typ_sng /* [fnc] Convert landunit type enum to string */
  (const int nco_lnd_typ); /* I [enm] Landunit type enum */

  const char * /* O [sng] String describing PFT type */
  nco_pft_typ_sng /* [fnc] Convert PFT-type enum to string */
  (const int nco_pft_typ); /* I [enm] PFT type enum */

  const char * /* O [sng] String describing sparse-type */
  nco_s1d_sng /* [fnc] Convert sparse-type enum to string */
  (const int nco_s1d_typ); /* I [enm] Sparse-type enum */

  int /* O [rcd] Return code */
  nco_s1d_unpack /* [fnc] Unpack contents of sparse-1D CLM/ELM file into full file */
  (rgr_sct * const rgr, /* I/O [sct] Regridding structure */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal Table */

#ifdef __cplusplus
} /* !extern "C" */
#endif /* __cplusplus */

#endif /* NCO_S1D_H */
