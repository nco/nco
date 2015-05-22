/* $Header$ */

/* Purpose: Description (definition) of regridding functions */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_rgr.h" *//* Regridding */

#ifndef NCO_RGR_H
#define NCO_RGR_H

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

  typedef enum nco_rgr_cmd_typ_enm{ /* [enm] Tempest remap type enum */
    nco_rgr_AAA_nil=0,
    nco_rgr_ApplyOfflineMap,
    nco_rgr_CalculateDiffNorms,
    nco_rgr_GenerateCSMesh,
    nco_rgr_GenerateGLLMetaData,
    nco_rgr_GenerateICOMesh,
    nco_rgr_GenerateLambertConfConicMesh,
    nco_rgr_GenerateOfflineMap,
    nco_rgr_GenerateOverlapMesh,
    nco_rgr_GenerateRLLMesh,
    nco_rgr_GenerateTestData,
    nco_rgr_MeshToTxt,
    nco_rgr_ZZZ_last
  } nco_rgr_cmd_typ;

  typedef enum nco_rgr_mpf_typ_enm{ /* [enm] Mapfile type enum */
    nco_rgr_mpf_nil=0,
    nco_rgr_mpf_SCRIP,
    nco_rgr_mpf_ESMF
  } nco_rgr_mpf_typ_enm;

  /* ESMF: The normalization attribute describes how the conservative weights are calculated, currently this is always set to "destarea" because this is the only option which we currently support. The setting "destarea" means that the conservative weights are calculated by dividing the area of the intersection of the source and destination cells by the area of the destination cell. This is set even when the weights are not conservative in which case it can be ignored. */
  typedef enum nco_rgr_nrm_typ_enm{ /* [enm] Normalization type enum */
    nco_rgr_nrm_nil=0,
    nco_rgr_nrm_fracarea, /* [] Allowed and explained by SCRIP */
    nco_rgr_nrm_destarea, /* [] Only option supported by ESMF, ignored when non-conservative methods used */
    nco_rgr_nrm_none, /* [] Allowed and explained by SCRIP */
  } nco_rgr_nrm_typ_enm;

  /* ESMF: The map_method attribute indicates the interpolation type. The format of the interpolation weight file was developed by a group outside of ESMF, because of its use by utilities outside of ESMF control, the range of some of the meta data is constrained. The map_method is one of these. Because of this constraint, there is no map method corresponding to patch interpolation. A weight file generated with the "patch" interpolation method will have map_method set to "Bilinear remapping".  */
  typedef enum nco_rgr_mth_typ_enm{ /* [enm] Mapfile type enum */
    nco_rgr_mth_nil=0,
    nco_rgr_mth_conservative,
    nco_rgr_mth_bilinear,
    nco_rgr_mth_none, /* []  */
  } nco_rgr_mth_typ_enm;

  typedef struct{ /* nco_map_sct */
    /* Contents of SCRIP-generated or -compliant remapping file */
    long int src_grid_size; /* [nbr] Source grid size (src_grid_size or n_a) */
    long int dst_grid_size; /* [nbr] Destination grid size (dst_grid_size or n_b) */
    long int src_grid_corners; /* [nbr] Source grid corners/vertices per gridcell (src_grid_corners or nv_a) */
    long int dst_grid_corners; /* [nbr] Destination grid corners/vertices per gridcell (dst_grid_corners or nv_b) */
    long int src_grid_rank; /* [nbr] Source grid rank */
    long int dst_grid_rank; /* [nbr] Destination grid rank */
    long int num_links; /* [nbr] Number of links (num_links or n_s) */
    long int num_wgts; /* [nbr] Number of weights */
    char *normalization; /* [sng] Normalization type */
    char *map_method; /* [sng] Remapping method */
    char *source_grid; /* [sng] Source grid */
    char *dest_grid; /* [sng] Destination grid */
  } nco_map_sct;
  
  int /* O [enm] Return code */
  nco_rgr_map /* [fnc] Regrid using external weights */
  (rgr_sct * const rgr_nfo, /* I/O [sct] Regridding structure */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal Table */

  int /* O [enm] Return code */
  nco_rgr_ctl /* [fnc] Control regridding logic */
  (rgr_sct * const rgr_nfo, /* I/O [sct] Regridding structure */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal Table */

  int /* O [enm] Return code */
  nco_rgr_ini /* [fnc] Initialize regridding structure */
  (const int in_id, /* I [id] Input netCDF file ID */
   char **rgr_arg, /* [sng] Regridding arguments */
   const int rgr_nbr, /* [nbr] Number of regridding arguments */
   char * const rgr_in, /* I [sng] File containing fields to be regridded */
   char * const rgr_out, /* I [sng] File containing regridded fields */
   char * const rgr_grd_src, /* I [sng] File containing input grid */
   char * const rgr_grd_dst, /* I [sng] File containing destination grid */
   char * const rgr_map, /* I [sng] File containing mapping weights from source to destination grid */
   char * const rgr_var, /* I [sng] Variable for special regridding treatment */
   rgr_sct * const rgr_nfo); /* O [sct] Regridding structure */
    
  void
  nco_rgr_free /* [fnc] Deallocate regridding structure */
  (rgr_sct * const rgr_nfo); /* I/O [sct] Regridding structure */

  int /* O [enm] Return code */
  nco_rgr_tps /* [fnc] Regrid using Tempest library */
  (rgr_sct * const rgr_nfo); /* I/O [sct] Regridding structure */

  const char * /* O [sng] String describing regridding method */
  nco_rgr_mth_sng /* [fnc] Convert regridding method enum to string */
  (const nco_rgr_mth_typ_enm nco_rgr_mth_typ); /* I [enm] Regridding method enum */

  const char * /* O [sng] String describing regridding normalization */
  nco_rgr_nrm_sng /* [fnc] Convert regridding normalization enum to string */
  (const nco_rgr_nrm_typ_enm nco_rgr_nrm_typ); /* I [enm] Regridding normalization enum */

  const char * /* O [sng] String containing regridding command and format */
  nco_tps_cmd_fmt_sng /* [fnc] Convert Tempest remap command enum to command string */
  (const nco_rgr_cmd_typ nco_rgr_cmd); /* I [enm] Tempest remap command enum */

  const char * /* O [sng] String containing regridding command name */
  nco_tps_cmd_sng /* [fnc] Convert Tempest remap command enum to command name */
  (const nco_rgr_cmd_typ nco_rgr_cmd); /* I [enm] Tempest remap command enum */

#ifdef ENABLE_ESMF
  int /* O [enm] Return code */
  nco_rgr_esmf /* [fnc] Regrid using ESMF library */
  (rgr_sct * const rgr_nfo); /* I/O [sct] Regridding structure */

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

#endif /* NCO_RGR_H */
