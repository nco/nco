/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sld.h,v 1.6 2015-01-30 04:16:48 zender Exp $ */

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
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  char *key;
  char *value;
} kvmap_sct;
 
int hdlscrip(char *fl_nm_scrip, kvmap_sct *smps); 
  
kvmap_sct nco_sng2map(char *str,  kvmap_sct sm); /* parse a line return a name-value pair kvmap */
  
int nco_sng2array(const char *delim, const char *str, char **sarray); /* split str by delim to sarray returns size of sarray */

char * nco_sng_strip(char *str); /* remove heading and trailing blanks */

void nco_kvmaps_free(kvmap_sct *kvmaps); /* release memory */

void nco_kvmap_prn(kvmap_sct sm);  /* print kvmap contents */

void nco_ppc_set(/* set ppc based user specifications */
char *const ppc_arg[], /* I [sng] List of user-specified ppc */
const int cnk_nbr, /* I [nbr] Number of ppc specified */
trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

void
nco_ppc_att_prc /* [fnc] create ppc att from trv_tbl */
(const int nc_id,                     /* I [id] Input netCDF file ID */
 const trv_tbl_sct * const trv_tbl);   /* I [sct] GTT (Group Traversal Table) */

void
trv_tbl_ppc_set_dflt /* Set the ppc value for all non-coordinate vars */
(const char * const sppc, /* I [sng] user input for least significant digit */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

void
trv_tbl_ppc_set_var
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 const char * const sppc, /* I [sng] user input for least significant digit */
 trv_tbl_sct * const trv_tbl);         /* I/O [sct] Traversal table */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SLD_H */
