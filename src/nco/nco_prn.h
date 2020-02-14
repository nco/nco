/* $Header$ */

/* Purpose: Print variables, attributes, metadata */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_prn.h" *//* Print variables, attributes, metadata */

#ifndef NCO_PRN_H
#define NCO_PRN_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 isfinite isinf isnan */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_grp_utl.h" /* Group utilities */
#include "nco_mmr.h" /* Memory management */
#ifdef _MSC_VER
# include "nco_rth_flt.h" /* Float-precision arithmetic, MSVC macros */
#endif /* !_MSC_VER */
#include "nco_sng_utl.h" /* String utilities */

/* fxm: strings statically allocated with NCO_MAX_LEN_FMT_SNG chars are susceptible to buffer overflow attacks */
/* Length should be computed at run-time but is a pain */
#define NCO_MAX_LEN_FMT_SNG 100ul

/* Maximum length of single formatted value of atomic value type */
#define NCO_ATM_SNG_LNG 25ul

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int 
nco_att_nbr        /* [fnc] return number of atts in var or global atts in group */ 
(const int grp_id, /* I [id] netCDF group ID */
 const int var_id); /* I [id] netCDF input variable ID */

void 
nco_prn_att /* [fnc] Print all attributes of single variable or group */
(const int grp_id, /* I [id] netCDF group ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const int var_id); /* I [id] netCDF input variable ID */

const char * /* O [sng] sprintf() format string for type typ */
nco_typ_fmt_sng /* [fnc] Provide sprintf() format string for specified type */
(const nc_type typ); /* I [enm] netCDF type to provide format string for */

const char * /* O [sng] sprintf() format string for CDL variable type typ */
nco_typ_fmt_sng_var_cdl /* [fnc] Provide sprintf() format string for specified variable type in CDL */
(const nc_type typ); /* I [enm] netCDF variable type to provide CDL format string for */

const char * /* O [sng] sprintf() format string for CDL attribute type typ */
nco_typ_fmt_sng_att_cdl /* [fnc] Provide sprintf() format string for specified attribute type in CDL */
(const nc_type typ); /* I [enm] netCDF attribute type to provide CDL format string for */

const char * /* O [sng] sprintf() format string for XML attribute type typ */
nco_typ_fmt_sng_att_xml /* [fnc] Provide sprintf() format string for specified attribute type in XML */
(const nc_type typ); /* I [enm] netCDF attribute type to provide XML format string for */

const char * /* O [sng] sprintf() format string for JSN attribute type typ */
nco_typ_fmt_sng_att_jsn /* [fnc] Provide sprintf() format string for specified attribute type in JSN */
(const nc_type typ); /* I [enm] netCDF attribute type to provide JSN format string for */

void
nco_prn_var_val_cmt     /* 0 print to stdout var values formatted  by prn_flg or dlm_sng_in  or */
(var_sct *var,          /* I [sct] variable to print */
const prn_fmt_sct * const prn_flg);  /* I [sct] Print-format information */

void
nco_prn_var_val_lmt /* [fnc] Print variable data */
(const int in_id, /* I [id] netCDF input file ID */
 const char * const var_nm, /* I [sng] Variable name */
 const lmt_sct * const lmt, /* I [sct] Dimension limits */
 const int lmt_nbr, /* I [nbr] number of dimensions with user-specified limits */
 char * const dlm_sng, /* I [sng] User-specified delimiter string, if any */
 const nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const nco_bool PRN_DMN_UNITS, /* I [flg] Print units attribute, if any */
 const nco_bool PRN_DMN_IDX_CRD_VAL); /* I [flg] Print dimension/coordinate indices/values */

void
nco_prn_var_val_trv /* [fnc] Print variable data */
(const int nc_id, /* I [ID] netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_sct * const var_trv, /* I [sct] Object to print (variable) */
 const trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */

void
nco_prn_var_dfn /* [fnc] Print variable metadata */
(const int nc_id, /* I [id] netCDF file ID */
 const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
 const trv_sct * const var_trv); /* I [sct] Object to print (variable) */

int /* [rcd] Return code */
nco_grp_prn /* [fnc] Recursively print group contents */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

int /* [rcd] Return code */
nco_prn_cdl_trd /* [fnc] Recursively print group contents in cdl or trd format */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

int /* [rcd] Return code */
nco_prn_jsn /* [fnc] Recursively print group contents in JSON format */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

int /* [rcd] Return code */
nco_prn_xml /* [fnc] Recursively print group contents in XML format */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const grp_nm_fll, /* I [sng] Absolute group name (path) */
 prn_fmt_sct * const prn_flg, /* I/O [sct] Print-format information */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

nco_bool /* O [flg] Variable is compound */
nco_prn_cpd_chk /* [fnc] Check whether variable is compound */
(const trv_sct * const var_trv, /* I [sct] Variable to check */
 const trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */ 

nco_bool /* O [flg] Type requires hidden _FillValue attribute string */
nco_xml_typ_rqr_flv_att /* [fnc] Does type require hidden _FillValue attribute for XML representation? */
(const nc_type nco_typ); /* I [enm] netCDF type */

nco_bool /* O [flg] Type requires hidden attribute string */
nco_xml_typ_rqr_nsg_att /* [fnc] Does type require hidden _Unsigned attribute for XML representation? */
(const nc_type nco_typ); /* I [enm] netCDF type */

void
nco_dfl_case_fmt_xtn_err /* [fnc] Print error and exit for illegal switch(nco_fmt_xtn) case */
(void);

const char * /* O [sng] JSON file type */
jsn_fmt_xtn_nm /* [fnc] Return string describing JSON filetype */
(const int fl_fmt_xtn); /* I [enm] Extended filetype */

void
nco_prn_nonfinite_flt(
char * const val_sng,		  
const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
float val_flt);

void
nco_prn_nonfinite_dbl(
char * const val_sng,		  
const prn_fmt_sct * const prn_flg, /* I [sct] Print-format information */
double val_dbl);
  

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_PRN_H */
