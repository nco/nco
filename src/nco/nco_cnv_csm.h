/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.h,v 1.36 2014-01-31 03:57:19 pvicente Exp $ */

/* Purpose: CCM/CCSM/CF conventions */

/* Copyright (C) 1995--2014 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cnv_csm.h" *//* CCM/CCSM/CF conventions */

#ifndef NCO_CNV_CCSM_H
#define NCO_CNV_CCSM_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cln_utl.h" /* Calendar utilities */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

nco_bool /* O [flg] File obeys CCM/CCSM/CF conventions */
nco_cnv_ccm_ccsm_cf_inq /* O [fnc] Check if file obeys CCM/CCSM/CF conventions */
(const int nc_id); /* I [id] netCDF file ID */

void
nco_cnv_ccm_ccsm_cf_date /* [fnc] Fix date variable in averaged CCSM files */
(const int nc_id, /* I [id] netCDF file ID */
 X_CST_PTR_CST_PTR_Y(var_sct,var), /* I/O [sct] Variables in output file */
 const int nbr_var); /* I [nbr] Number of variables in list */

nm_id_sct * /* O [sct] Extraction list */
nco_cnv_cf_crd_add /* [fnc] Add coordinates defined by CF convention */
(const int nc_id, /* I netCDF file ID */
 nm_id_sct *xtr_lst, /* I/O current extraction list (destroyed) */
 int * const xtr_nbr); /* I/O number of variables in current extraction list */

int                                  /* [rcd] Return code */
nco_cnv_cf_cll_mth_add               /* [fnc] Add cell_methods attributes */
(const int out_id,                   /* I [id] netCDF file ID */
 var_sct * const * const var,        /* I [sct] Variable to reduce (e.g., average) (destroyed) */
 const int nbr_var,                  /* I [nbr] Number of variables to be defined */
 dmn_sct * const * const dim,        /* I [sct] Dimensions over which to reduce variable */
 const int nbr_dim,                  /* I [sct] Number of dimensions to reduce variable over */
 const int nco_op_typ,               /* I [enm] Operation type, default is average */
 gpe_sct *gpe,                       /* [sng] Group Path Editing (GPE) structure */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */


#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNV_CCSM_H */
