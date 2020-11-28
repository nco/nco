/* $Header$ */

/* Purpose: Description (definition) of Swath-Like Data (SLD) functions */

/* Copyright (C) 2015--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_sld.h" *//* Swath-Like Data */

#ifndef NCO_SLD_H
#define NCO_SLD_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* atof, atoi, malloc, getopt */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */
#ifdef ENABLE_ESMF
# include <ESMC.h> /* ESMF definitions */
#endif /* !ENABLE_ESMF */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /* Types used in Sparse structure */
  typedef enum nco_srs_ntl_typ_enm{ /* [enm] Interleave-type enum */
    nco_srs_ntl_nil=0,
    nco_srs_ntl_unk, /* Unknown or unclassified sparse-type */
    nco_srs_ntl_pft, /* Sparse PFT dimension (pfts1d format) */
    nco_srs_ntl_clm, /* Sparse Column dimension (cols1d format) */
  } nco_srs_ntl_typ_enm;

  /* Types used in Terraref structure */
  typedef enum nco_trr_ntl_typ_enm{ /* [enm] Interleave-type enum */
    nco_trr_ntl_nil=0,
    nco_trr_ntl_unk, /* Unknown or unclassified interleave-type */ 
    nco_trr_ntl_bsq, /* Band Sequential */
    nco_trr_ntl_bip, /* Band-interleaved-by-pixel */
    nco_trr_ntl_bil, /* Band-interleaved-by-line */
  } nco_trr_ntl_typ_enm;

  /* Terraref structure */
  typedef struct{ /* trr_sct */
    // File names specifiable with individual command line switches
    char *fl_in; /* [sng] File containing raw imagery */
    char *fl_out; /* [sng] File containing netCDF imagery */
    char *fl_out_tmp; /* [sng] Temporary file containing netCDF imagery */
    // Metadata specifiable with key-value syntax
    char **trr_arg; /* [sng] Terraref arguments */
    char *wvl_nm; /* [sng] Name of wavelength dimension */
    char *xdm_nm; /* [sng] Name of x-coordinate dimension */
    char *ydm_nm; /* [sng] Name of y-coordinate dimension */
    char *var_nm; /* [sng] Variable containing imagery */
    char *wvl_bnd_nm; /* [sng] Name of dimension to employ for wavelength bounds */
    char *xdm_bnd_nm; /* [sng] Name of dimension to employ for x-coordinate bounds */
    char *ydm_bnd_nm; /* [sng] Name of dimension to employ for y-coordinate bounds */
    long wvl_nbr; /* [nbr] Number of wavelengths */
    long xdm_nbr; /* [nbr] Number of pixels in x-dimension */
    long ydm_nbr; /* [nbr] Number of pixels in y-dimension */
    nc_type var_typ_in; /* [enm] NetCDF type */
    nc_type var_typ_out; /* [enm] NetCDF type */
    // Other internal data and metadata 
    char *cmd_ln; /* [sng] Command-line */
    char *ttl; /* [sng] Title */
    int dfl_lvl; /* [enm] Deflate level [0..9] */
    int trr_nbr; /* [nbr] Number of Terraref arguments */
    nco_trr_ntl_typ_enm ntl_typ_in; /* [enm] Interleave-type of raw data */
    nco_trr_ntl_typ_enm ntl_typ_out; /* [enm] Interleave-type or output */
  } trr_sct; /* !Terraref structure */

  const char * /* O [sng] String describing sparse-type */
  nco_srs_ntl_sng /* [fnc] Convert sparse-type enum to string */
  (const nco_srs_ntl_typ_enm nco_srs_ntl_typ); /* I [enm] Sparse-type enum */

  int /* O [rcd] Return code */
  nco_srs_rewrite /* [fnc] Rewrite contents of sparse CLM/ELM file into unrolled file */
  (rgr_sct * const rgr, /* I/O [sct] Regridding structure */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal Table */

  int /* O [rcd] Return code */
  nco_trr_read /* [fnc] Read, parse, and print contents of TERRAREF file */
  (trr_sct *trr_nfo); /* I/O [sct] Terraref information */
  
  trr_sct * /* O [sct] Terraref structure */
  nco_trr_ini /* [fnc] Initialize Terraref structure */
  (const char * const cmd_ln, /* I [sng] Command-line */
   const int dfl_lvl, /* I [enm] Deflate level [0..9] */
   char **trr_arg, /* I [sng] Terraref arguments */
   const int trr_arg_nbr, /* I [nbr] Number of Terraref arguments */
   char * const trr_in, /* I [sng] File containing raw Terraref imagery */
   char * const trr_out, /* I [sng] File containing netCDF Terraref imagery */
   char * const trr_wxy); /* I [sng] Terraref dimension sizes */
  
  trr_sct * /* O [sct] Pointer to free'd Terraref structure */
  nco_trr_free /* [fnc] Deallocate Terraref structure */
  (trr_sct *trr); /* I/O [sct] Terraref structure */

  nco_trr_ntl_typ_enm /* O [enm] Interleave-type */
  nco_trr_sng_ntl /* [fnc] Convert user-supplied string to interleave-type enum */
  (const char * const typ_sng); /* I [sng] String indicating interleave-type */

  const char * /* O [sng] String describing interleave-type */
  nco_trr_ntl_sng /* [fnc] Convert interleave-type enum to string */
  (const nco_trr_ntl_typ_enm nco_trr_ntl_typ); /* I [enm] Interleave-type enum */

#ifdef ENABLE_ESMF
  int /* O [enm] Return code */
  nco_rgr_esmf /* [fnc] Regrid using ESMF library */
  (rgr_sct * const rgr_nfo); /* I/O [sct] Regridding structure */

  int /* O [enm] Return code */
  nco_rgr_esmf2 /* [fnc] Regrid using ESMF library */
  (rgr_sct * const rgr); /* I/O [sct] Regridding structure */

  const char * /* O [sng] String version of ESMC_StaggerLoc enum */
  nco_esmf_stg_lcn_sng /* [fnc] Convert ESMF stagger location enum to string */
  (const int nco_esmf_stg_lcn); /* I [enm] ESMF stagger location enum ESMC_StaggerLoc */
  
  const char * /* O [sng] String version of ESMC_CoordSys_Flag enum */
  nco_esmf_crd_sys_sng /* [fnc] Convert ESMF coordinate system enum to string */
  (const int nco_esmf_crd_sys); /* I [enm] ESMF coordinate system enum ESMC_CoordSys_Flag */
  
  const char * /* O [sng] String version of ESMC_FileFormat_Flag */
  nco_esmf_fl_fmt_sng /* [fnc] Convert ESMF file format to string */
  (const int nco_esmf_fl_fmt); /* I [enm] ESMF file format enum ESMC_FileFormat_Flag */

  const char * /* O [sng] String version of ESMC_GridItem_Flag enum */
  nco_esmf_grd_itm_sng /* [fnc] Convert ESMF grid item enum to string */
  (const int nco_esmf_grd_itm); /* I [enm] ESMF grid item enum ESMC_GridItem_Flag */
  
  const char * /* O [sng] String version of ESMC_LogMsgType_Flag enum */
  nco_esmf_log_msg_sng /* [fnc] Convert ESMF log message type enum to string */
  (const int nco_esmf_log_msg); /* I [enm] ESMF log message type enum ESMC_LogMsgType_Flag */
  
  const char * /* O [sng] String version of ESMC_PoleMethod_Flag enum */
  nco_esmf_pll_mth_sng /* [fnc] Convert ESMF pole method type enum to string */
  (const int nco_esmf_pll_mth); /* I [enm] ESMF pole method type enum ESMC_PoleMethod_Flag */

  const char * /* O [sng] String version of ESMC_PoleKind_Flag enum */
  nco_esmf_pll_knd_sng /* [fnc] Convert ESMF pole type enum to string */
  (const int nco_esmf_pll_knd); /* I [enm] ESMF pole type enum ESMC_PoleKind_Flag */

  const char * /* O [sng] String version of ESMC_Region_Flag enum */
  nco_esmf_rgn_flg_sng /* [fnc] Convert ESMF region flag enum to string */
  (const int nco_esmf_rgn_flg); /* I [enm] ESMF region flag enum ESMC_Region_Flag */

  const char * /* O [sng] String version of ESMC_RegridMethod_Flag enum */
  nco_esmf_rgr_mth_sng /* [fnc] Convert ESMF regrid method type enum to string */
  (const int nco_esmf_rgr_mth); /* I [enm] ESMF regrid method type enum ESMC_RegridMethod_Flag */

  const char * /* O [sng] String version of ESMC_TypeKind_Flag enum */
  nco_esmf_typ_knd_sng /* [fnc] Convert ESMF type kind enum to string */
  (const int nco_esmf_typ_knd); /* I [enm] ESMF type kind enum ESMC_TypeKind_Flag */
  
  const char * /* O [sng] String version of ESMC_UnmappedAction_Flag enum */
  nco_esmf_unm_act_sng /* [fnc] Convert ESMF unmapped action type enum to string */
  (const int nco_esmf_unm_act); /* I [enm] ESMF unmapped action type enum ESMC_UnmappedAction_Flag */
#endif /* !ENABLE_ESMF */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SLD_H */
