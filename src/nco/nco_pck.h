/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_pck.h,v 1.12 2004-01-12 18:11:07 zender Exp $ */

/* Purpose: Description (definition) of packing/unpacking functions */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_pck.h" *//* Packing and unpacking variables */

#ifndef NCO_PCK_H
#define NCO_PCK_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_scl_utl.h" /* Scalar utilities */
#include "nco_rth_utl.h" /* Arithmetic controls and utilities */
#include "nco_var_avg.h" /* Average variables */
#include "nco_var_rth.h" /* Variable arithmetic */
#include "nco_var_scv.h" /* Arithmetic between variables and scalar values */
#include "nco_var_utl.h" /* Variable utilities */

enum nco_pck_typ{ /* [enm] Packing type */
  nco_pck_all_xst_att, /* 0 [enm] Pack all variables, keeping existing packing attributes if any */
  nco_pck_all_new_att, /* 1 [enm] Pack all variables, always generate new packing attributes */
  nco_pck_xst_xst_att, /* 2 [enm] Pack existing packed variables, keep existing packing attributes if any */
  nco_pck_xst_new_att, /* 3 [enm] Pack existing packed variables, always generat new packing attributes */
  nco_pck_upk, /* 4 [enm] Unpack all packed variables */
  nco_pck_nil /* 5 [enm] Do not think about packing */
}; /* end nco_pck_typ enum */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int /* O [enm] Packing type */
nco_pck_typ_get /* [fnc] Convert user-specified packing type to key */
(const char *nco_pck_sng); /* [sng] User-specified packing type */

bool /* O [flg] Variable is packed on disk */
pck_dsk_inq /* [fnc] Check whether variable is packed on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 var_sct *var); /* I/O [sct] Variable */
  
var_sct * /* O [sct] Packed variable */
nco_var_pck /* [fnc] Pack variable in memory */
(var_sct *var, /* I/O [sct] Variable to be packed */
 const nc_type typ_pck, /* I [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
 const bool USE_EXISTING_PCK); /* I [flg] Use existing packing scale_factor and add_offset */

var_sct * /* O [sct] Unpacked variable */
nco_var_upk /* [fnc] Unpack variable in memory */
(var_sct * const var); /* I/O [sct] Variable to be unpacked */

var_sct * /* O [sct] Packed variable */
nco_put_var_pck /* [fnc] Pack variable in memory and write packing attributes to disk */
(const int out_id, /* I [id] netCDF output file ID */
 var_sct *var, /* I/O [sct] Variable to be packed */
 const int nco_pck_typ); /* [enm] Packing operation type */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_PCK_H */
