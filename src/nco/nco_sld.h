/* $Header$ */

/* Purpose: Description (definition) of Swath-Like Data (SLD) functions */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_sld.h" *//* Swath-Like Data */

#ifndef NCO_SLD_H
#define NCO_SLD_H

/* Standard header files */
#include <ctype.h> /* isalnum(), isdigit(), tolower() */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp() */

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

 typedef struct {
   char *key;
   char *value;
 } kvmap_sct;
 
  int
  nco_scrip_read /* [fnc] Handle SCRIP file */
  (char *fl_scrip,
   kvmap_sct *kvm_scrip);
  
  kvmap_sct
  nco_sng2map /* [fnc] Parse line return key-value structure */
  (char *str,
   kvmap_sct kvm);
  
  int nco_sng2array /* [fnc] Split string by delimite to subarray and return size of subarray */
  (const char *delim,
   const char *str,
   char **sarray);

  char *
  nco_sng_strip /* [fnc] Remove leading and trailing blanks */
  (char *str);

  void
  nco_kvmaps_free /* [fnc] Release memory */
  (kvmap_sct *kvmaps);

  void
  nco_kvmap_prn /* [fnc] Print kvmap contents */
  (kvmap_sct kvm);

  void 
  nco_ppc_ini /* [fnc] Set PPC based on user specifications */
  (const int nc_id, /* I [id] netCDF input file ID */
   int *dfl_lvl, /* O [enm] Deflate level */
   const int fl_out_fmt, /* I [enm] Output file format */
   char *const ppc_arg[], /* I [sng] List of user-specified ppc */
   const int ppc_nbr, /* I [nbr] Number of ppc specified */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */
  
  void
  nco_ppc_att_prc /* [fnc] create ppc att from trv_tbl */
  (const int nc_id, /* I [id] Input netCDF file ID */
   const trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */
  
  void
  nco_ppc_set_dflt /* Set the ppc value for all non-coordinate vars */
  (const int nc_id, /* I [id] netCDF input file ID */
   const char * const ppc_arg, /* I [sng] user input for precision-preserving compression */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */
  
  void
  nco_ppc_set_var
  (const char * const var_nm_fll, /* I [sng] Variable name to find */
   const char * const ppc_arg, /* I [sng] user input for precision-preserving compression */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

#ifdef ENABLE_ESMF
  int /* O [enm] Return code */
  nco_rgr_ini /* [fnc] Initialize regridding structure */
  (const int in_id, /* I [id] Input netCDF file ID */
   char **rgr_arg, /* [sng] Regriding arguments */
   const int rgr_nbr, /* [nbr] Number of regriding arguments */
   char * const rgr_in, /* I [sng] File containing fields to be regridded */
   char * const rgr_out, /* I [sng] File containing regridded fields */
   char * const rgr_grd_src, /* I [sng] File containing input grid */
   char * const rgr_grd_dst, /* I [sng] File containing destination grid */
   char * const rgr_map, /* I [sng] File containing mapping weights from source to destination grid */
   rgr_sct * const rgr_nfo); /* O [sct] Regridding structure */
    
  void
  nco_rgr_free /* [fnc] Deallocate regridding structure */
  (rgr_sct * const rgr_nfo); /* I/O [sct] Regridding structure */

  int /* O [enm] Return code */
  nco_rgr_esmf /* [fnc] Regrid using ESMF library */
  (rgr_sct * const rgr_nfo); /* I/O [sct] Regridding structure */
#endif /* !ENABLE_ESMF */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SLD_H */
