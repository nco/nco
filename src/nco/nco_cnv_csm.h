/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.h,v 1.2 2002-05-06 06:37:14 zender Exp $ */

/* Purpose: CSM conventions */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
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
ncar_csm_inq /* O [fnc] Check if file obeys CSM conventions */
(const int nc_id); /* I [id] netCDF file ID */

void
ncar_csm_date /* [fnc] Fix date variable in averaged CSM files */
(const int nc_id, /* I [id] netCDF file ID */
 const var_sct **var, /* I [sct] List of pointers to variable structures */
 const int nbr_var); /* I [nbr] Number of structures in variable structure list */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNV_CSM_H */
