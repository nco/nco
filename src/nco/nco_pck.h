/* $Header$ */

/* Purpose: Description (definition) of packing/unpacking functions */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

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

/* Packing policies: 
   Handle each packing policy in nco_pck_mtd(), nco_pck_val(), nco_var_dfn(), and end of ncpdq */
enum nco_pck_plc_enm{ /* [enm] Packing policy */
  nco_pck_plc_nil, /* 0 [enm] Do not think about packing */
  nco_pck_plc_all_xst_att, /* 1 [enm] Pack all variables, keep existing packing attributes if any */
  nco_pck_plc_all_new_att, /* 2 [enm] Pack all variables, always generate new packing attributes */
  nco_pck_plc_xst_new_att, /* 3 [enm] Pack existing packed variables, always generate new packing attributes */
  nco_pck_plc_upk /* 4 [enm] Unpack all packed variables */
}; /* end nco_pck_plc enum */

/* Packing type maps: */
enum nco_pck_map{ /* [enm] Packing conversion map */
  /* NB: Packing to NC_CHAR is implemented but not advertised or supported because 
     (char)0 == NUL, a special value with difficult implicit conversion rules.
     We recommend always packing to type NC_BYTE rather than NC_CHAR */
  nco_pck_map_nil, /* 0 [enm] Do not convert anything, i.e., all types remain unchanged */
  nco_pck_map_hgh_sht, /* 1 [enm] Pack higher precision types to NC_SHORT, pack nothing else
			  [NC_DOUBLE,NC_FLOAT,NC_INT]->NC_SHORT, [NC_SHORT,NC_CHAR,NC_BYTE]->unaltered */
  nco_pck_map_hgh_chr, /* 2 [enm] Pack higher precision types to NC_CHAR, pack nothing else
			  [NC_DOUBLE,NC_FLOAT,NC_INT,NC_SHORT]->NC_CHAR, [NC_CHAR,NC_BYTE]->unaltered */
  nco_pck_map_hgh_byt, /* 3 [enm] Pack higher precision types to NC_BYTE, pack nothing else
			  [NC_DOUBLE,NC_FLOAT,NC_INT,NC_SHORT]->NC_BYTE, [NC_CHAR,NC_BYTE]->unaltered */
  nco_pck_map_nxt_lsr, /* 4 [enm] Convert each type of each size to type of next size down
			  NC_DOUBLE->NC_INT, [NC_FLOAT,NC_INT]->NC_SHORT, NC_SHORT->NC_BYTE, [NC_CHAR,NC_BYTE]->unaltered */
  nco_pck_map_flt_sht, /* 5 [enm] Pack floating precision types to NC_SHORT, pack nothing else
			  [NC_DOUBLE,NC_FLOAT]->NC_SHORT, [NC_INT,NC_SHORT,NC_CHAR,NC_BYTE]->unaltered */
  nco_pck_map_flt_chr, /* 6 [enm] Pack floating precision types to NC_CHAR, pack nothing else
			  [NC_DOUBLE,NC_FLOAT]->NC_CHAR, [NC_INT,NC_SHORT,NC_CHAR,NC_BYTE]->unaltered */
  nco_pck_map_flt_byt, /* 7 [enm] Pack floating precision types to NC_BYTE, pack nothing else
			  [NC_DOUBLE,NC_FLOAT]->NC_BYTE, [NC_INT,NC_SHORT,NC_CHAR,NC_BYTE]->unaltered */
  nco_pck_map_dbl_flt, /* 8 [enm] Convert (do not pack) floating precision type NC_DOUBLE to NC_FLOAT, pack nothing
			  [NC_DOUBLE]->NC_FLOAT, [NC_FLOAT,NC_INT,NC_SHORT,NC_CHAR,NC_BYTE]->unaltered */
  nco_pck_map_flt_dbl  /* 9 [enm] Convert (do not pack) floating precision type NC_FLOAT to NC_DOUBLE, pack nothing
			  [NC_FLOAT]->NC_DOUBLE, [NC_DOUBLE,NC_INT,NC_SHORT,NC_CHAR,NC_BYTE]->unaltered */
}; /* end nco_pck_map enum */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 
nco_dfl_case_pck_map_err(void); /* [fnc] Print error and exit for illegal switch(pck_map) case */

void 
nco_dfl_case_pck_plc_err(void); /* [fnc] Print error and exit for illegal switch(nco_pck_plc) case */

nco_bool /* O [flg] NCO will attempt to pack variable */
nco_is_packable /* [fnc] Will NCO attempt to pack variable? */
(const nc_type nc_typ_in); /* I [enm] Type of input variable */

const char * /* O [sng] Packing map string */
nco_pck_map_sng_get /* [fnc] Convert packing map enum to string */
(const int nco_pck_map); /* I [enm] Packing map */

const char * /* O [sng] Packing policy string */
nco_pck_plc_sng_get /* [fnc] Convert packing policy enum to string */
(const int nco_pck_plc); /* I [enm] Packing policy */

nco_bool /* O [flg] Packing policy allows packing nc_typ_in */
nco_pck_plc_typ_get /* [fnc] Determine type, if any, to pack input type to */
(const int nco_pck_map, /* I [enm] Packing map */
 const nc_type nc_typ_in, /* I [enm] Type of input variable */
 nc_type *nc_typ_pck_out); /* O [enm] Type to pack variable to */

void
nco_pck_mtd /* [fnc] Alter metadata according to packing specification */
(const var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * const var_out, /* I/O [ptr] Variable whose metadata will be altered */
 const int nco_pck_map, /* I [enm] Packing map */
 const int nco_pck_plc); /* I [enm] Packing policy */

void
nco_pck_val /* [fnc] Pack variable according to packing specification */
(var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * var_out, /* I/O [ptr] Variable after packing/unpacking operation */
 const int nco_pck_map, /* I [enm] Packing map */
 const int nco_pck_plc, /* I [enm] Packing policy */
 aed_sct * const aed_lst_add_fst, /* O [enm] Attribute edit structure, add_offset */
 aed_sct * const aed_lst_scl_fct); /* O [enm] Attribute edit structure, scale_factor */

int /* O [enm] Packing map */
nco_pck_map_get /* [fnc] Convert user-specified packing map to key */
(const char *nco_pck_map_sng); /* [sng] User-specified packing map */

int /* O [enm] Packing policy */
nco_pck_plc_get /* [fnc] Convert user-specified packing policy to key */
(const char *nco_pck_plc_sng); /* [sng] User-specified packing policy */
  
nco_bool /* O [flg] Variable is packed on disk */
nco_pck_dsk_inq /* [fnc] Check whether variable is packed on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 var_sct *var); /* I/O [sct] Variable */
  
var_sct * /* O [sct] Packed variable */
nco_put_var_pck /* [fnc] Pack variable in memory and write packing attributes to disk */
(const int out_id, /* I [id] netCDF output file ID */
 var_sct *var, /* I/O [sct] Variable to be packed */
 const int nco_pck_plc); /* [enm] Packing operation type */

var_sct * /* O [sct] Packed variable */
nco_var_pck /* [fnc] Pack variable in memory */
(var_sct *var, /* I/O [sct] Variable to be packed */
 const nc_type typ_pck, /* I [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
 nco_bool *PCK_VAR_WITH_NEW_PCK_ATT); /* O [flg] Routine generated new scale_factor/add_offset */

var_sct * /* O [sct] Unpacked variable */
nco_var_upk /* [fnc] Unpack variable in memory */
(var_sct * const var); /* I/O [sct] Variable to be unpacked */

void
nco_var_upk_swp /* [fnc] Unpack var_in into var_out */
(var_sct * const var_in, /* I/O [sct] Variable to unpack */
 var_sct * const var_out); /* I/O [sct] Variable to unpack into */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_PCK_H */
