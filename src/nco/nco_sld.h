/* $Header$ */

/* Purpose: Description (definition) of Swath-Like Data (SLD) functions */

/* Copyright (C) 2015--2016 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

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

  int /* O [rcd] Return code */
  nco_scrip_read /* [fnc] Read, parse, and print contents of SCRIP file */
  (char *fl_scrip, /* I [sng] SCRIP file name with proper path */
   kvm_sct *kvm_scrip); /* I/O [sct] Structure to hold contents of SCRIP file */ 

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
