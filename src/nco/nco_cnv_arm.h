/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_arm.h,v 1.4 2002-12-30 02:56:14 zender Exp $ */

/* Purpose: ARM conventions */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cnv_arm.h" *//* ARM conventions */

#ifndef NCO_CNV_ARM_H
#define NCO_CNV_ARM_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_att_utl.h" /* Attribute utilities */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

bool /* O [flg] File obeys ARM conventions */
arm_inq /* O [fnc] Check if file obeys ARM conventions */
(const int nc_id); /* I [id] netCDF file ID */

double /* O [s] base_time + current time_offset */ 
arm_time_mk /* [fnc] Return time corresponding to current time offset */
(const int nc_id, /* I [id] netCDF file ID */
 const double time_offset); /* I [s] Current time offset */

void
nco_arm_time_install /* [fnc] Add time variable to concatenated ARM files */
(const int nc_id, /* I [id] netCDF file ID */
 const nco_long base_time_srt); /* I [s] base_time of first input file */

nco_long /* O [s] Value of base_time variable */
arm_base_time_get /* [fnc] Get base_time variable from ARM file */
(const int nc_id); /* I [id] netCDF file ID */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNV_ARM_H */
