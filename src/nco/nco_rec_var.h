/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rec_var.h,v 1.3 2004-01-01 20:41:43 zender Exp $ */

/* Purpose: Record variable utilities */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL)
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_rec_var.h" *//* Record variable utilities */

#ifndef NCO_REC_VAR_H
#define NCO_REC_VAR_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
rec_var_dbg /* [fnc] Aid in debugging problems with record dimension */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const dbg_sng); /* I [sng] Debugging message to print */

void
rec_crd_chk /* Check for monotonicity of coordinate values */
(const var_sct * const var, /* I [sct] Coordinate to check for monotonicity */
 const char * const fl_in, /* I [sng] Input filename */
 const char * const fl_out, /* I [sng] Output filename */
 const long idx_rec, /* I [idx] Index of record coordinate in input file */
 const long idx_rec_out); /* I [idx] Index of record coordinate in output file */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_REC_VAR_H */
