/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.h,v 1.9 2004-01-05 17:29:05 zender Exp $ */

/* Purpose: CSM conventions */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cnv_csm.h" *//* CSM conventions */

#ifndef NCO_CNV_CSM_H
#define NCO_CNV_CSM_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_cln_utl.h" /* Calendar utilities */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool /* O [flg] File obeys CSM conventions */
nco_ncar_csm_inq /* O [fnc] Check if file obeys CSM conventions */
(const int nc_id); /* I [id] netCDF file ID */

void
nco_ncar_csm_date /* [fnc] Fix date variable in averaged CSM files */
(const int nc_id, /* I [id] netCDF file ID */
 X_CST_PTR_CST_PTR_Y(var_sct,var), /* I/O [sct] Variables in output file */
 const int nbr_var); /* I [nbr] Number of variables in list */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNV_CSM_H */
