/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_pck.h,v 1.24 2004-09-06 06:26:28 zender Exp $ */

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

/* Packing types: 
   Handle each packing type in nco_pck_mtd(), nco_pck_val() */
enum nco_pck_typ{ /* [enm] Packing type */
  nco_pck_nil, /* 0 [enm] Do not think about packing */
  nco_pck_all_xst_att, /* 1 [enm] Pack all variables, keep existing packing attributes if any */
  nco_pck_all_new_att, /* 2 [enm] Pack all variables, always generate new packing attributes */
  nco_pck_xst_new_att, /* 3 [enm] Pack existing packed variables, always generate new packing attributes */
  nco_pck_upk /* 4 [enm] Unpack all packed variables */
}; /* end nco_pck_typ enum */

/* Packing conversions: */
enum nco_pck_map{ /* [enm] Packing conversion map */
  nco_pck_map_nil, /* 0 [enm] Do not convert anything, i.e., all types remain unchanged */
  nco_pck_map_hgh_sht, /* 1 [enm] Pack higher precision types to NC_SHORT, pack nothing else
			  [NC_DOUBLE,NC_FLOAT,NC_INT]->NC_SHORT, [NC_SHORT,NC_CHAR,NC_BYTE]->remain */
  nco_pck_map_hgh_chr, /* 2 [enm] Pack higher precision types to NC_CHAR, pack nothing else
			  [NC_DOUBLE,NC_FLOAT,NC_INT,NC_SHORT]->NC_CHAR, [NC_CHAR,NC_BYTE]->remain */
  nco_pck_map_dwn_one, /* 3 [enm] Convert each type of each size to type of next size down
			  NC_DOUBLE->NC_INT, [NC_FLOAT,NC_INT]->NC_SHORT, [NC_CHAR,NC_BYTE]->remain */
  nco_pck_map_flt_sht, /* 4 [enm] Pack floating precision types to NC_SHORT, pack nothing else
			  [NC_DOUBLE,NC_FLOAT]->NC_SHORT, [NC_INT,NC_SHORT,NC_CHAR,NC_BYTE]->remain */
  nco_pck_map_flt_chr /* 5 [enm] Pack floating precision types to NC_CHAR, pack nothing else
			  [NC_DOUBLE,NC_FLOAT]->NC_CHAR, [NC_INT,NC_SHORT,NC_CHAR,NC_BYTE]->remain */
}; /* end nco_pck_map enum */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 
nco_dfl_case_pck_typ_err /* [fnc] Print error and exit for illegal switch(nco_pck_typ) case */
(void);

bool /* O [flg] NCO will attempt to pack variable */
nco_is_packable /* [fnc] Will NCO attempt to pack variable? */
(const nc_type nc_typ_in); /* I [enm] Type of input variable */

nc_type /* O [enm] Type to pack variable to */
nco_typ_pck_get /* [fnc] Determine best type to pack input variable to */
(const nc_type nc_typ_in); /* I [enm] Type of input variable */

void
nco_pck_mtd /* [fnc] Alter metadata according to packing specification */
(const var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * const var_out, /* I/O [ptr] Variable whose metadata will be altered */
 const int nco_pck_typ); /* I [enm] Packing type */

void
nco_pck_val /* [fnc] Pack variable according to packing specification */
(var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * var_out, /* I/O [ptr] Variable after packing/unpacking operation */
 const int nco_pck_typ, /* I [enm] Packing type */
 aed_sct * const aed_lst_add_fst, /* O [enm] Attribute edit structure, add_offset */
 aed_sct * const aed_lst_scl_fct); /* O [enm] Attribute edit structure, scale_factor */

int /* O [enm] Packing map */
nco_pck_map_get /* [fnc] Convert user-specified packing map to key */
(const char *nco_pck_map_sng); /* [sng] User-specified packing map */

int /* O [enm] Packing type */
nco_pck_typ_get /* [fnc] Convert user-specified packing type to key */
(const char *nco_pck_typ_sng); /* [sng] User-specified packing type */
  
bool /* O [flg] Variable is packed on disk */
nco_pck_dsk_inq /* [fnc] Check whether variable is packed on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 var_sct *var); /* I/O [sct] Variable */
  
var_sct * /* O [sct] Packed variable */
nco_put_var_pck /* [fnc] Pack variable in memory and write packing attributes to disk */
(const int out_id, /* I [id] netCDF output file ID */
 var_sct *var, /* I/O [sct] Variable to be packed */
 const int nco_pck_typ); /* [enm] Packing operation type */

var_sct * /* O [sct] Packed variable */
nco_var_pck /* [fnc] Pack variable in memory */
(var_sct *var, /* I/O [sct] Variable to be packed */
 const nc_type typ_pck, /* I [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
 bool *PCK_VAR_WITH_NEW_PCK_ATT); /* O [flg] Routine generated new scale_factor/add_offset */

var_sct * /* O [sct] Unpacked variable */
nco_var_upk /* [fnc] Unpack variable in memory */
(var_sct * const var); /* I/O [sct] Variable to be unpacked */

void
nco_var_upk_swp /* [fnc] Unpack var_in into var_out */
(const var_sct * const var_in, /* I [sct] Variable to unpack */
 var_sct * const var_out); /* I/O [sct] Variable to unpack into */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_PCK_H */
