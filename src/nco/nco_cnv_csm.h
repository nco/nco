/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.h,v 1.16 2005-10-19 23:32:35 zender Exp $ */

/* Purpose: CCM/CCSM/CF conventions */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cnv_csm.h" *//* CCM/CCSM/CF conventions */

#ifndef NCO_CNV_CCSM_H
#define NCO_CNV_CCSM_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* !_OPENMP */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cln_utl.h" /* Calendar utilities */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool /* O [flg] File obeys CCM/CCSM/CF conventions */
nco_cnv_ccm_ccsm_cf_inq /* O [fnc] Check if file obeys CCM/CCSM/CF conventions */
(const int nc_id); /* I [id] netCDF file ID */

void
nco_cnv_ccm_ccsm_cf_date /* [fnc] Fix date variable in averaged CCSM files */
(const int nc_id, /* I [id] netCDF file ID */
 X_CST_PTR_CST_PTR_Y(var_sct,var), /* I/O [sct] Variables in output file */
 const int nbr_var); /* I [nbr] Number of variables in list */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNV_CCSM_H */
