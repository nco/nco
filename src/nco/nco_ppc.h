/* $Header$ */

/* Purpose: Description (definition) of Precision-Preserving Compression (PPC) functions */

/* Copyright (C) 2015--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_ppc.h" *//* Precision-Preserving Compression */

#ifndef NCO_PPC_H
#define NCO_PPC_H

/* Standard header files */
#include <ctype.h> /* isalnum(), isdigit(), tolower() */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */
#include "nco_mta.h" /* Multi-argument parsing */
#include "nco_sng_utl.h" /* String utilities */

/* Minimum number of explicit significand bits to preserve when zeroing/bit-masking floating point values
   Codes will preserve at least two explicit bits, IEEE significand representation contains one implicit bit
   Thus preserve a least three bits which is approximately one sigificant decimal digit
   Used in nco_ppc_bitmask() and nco_ppc_bitmask_scl() */
#define NCO_PPC_BIT_XPL_NBR_MIN 2

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  void 
  nco_ppc_ini /* [fnc] Set PPC based on user specifications */
  (const int nc_id, /* I [id] netCDF input file ID */
   int *dfl_lvl, /* O [enm] Deflate level */
   const int fl_out_fmt, /* I [enm] Output file format */
   char *const ppc_arg[], /* I [sng] List of user-specified ppc */
   const int ppc_nbr, /* I [nbr] Number of ppc specified */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */
  
  void
  nco_ppc_att_prc /* [fnc] create ppc att from trv_tbl */
  (const int nc_id, /* I [id] Input netCDF file ID */
   const trv_tbl_sct * const trv_tbl); /* I [sct] GTT (Group Traversal Table) */
  
  void
  nco_ppc_set_dflt /* Set the ppc value for all non-coordinate vars */
  (const int nc_id, /* I [id] netCDF input file ID */
   const char * const ppc_arg, /* I [sng] user input for precision-preserving compression */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */
  
  void
  nco_ppc_set_var
  (const char * const var_nm_fll, /* I [sng] Variable name to find */
   const char * const ppc_arg, /* I [sng] user input for precision-preserving compression */
   trv_tbl_sct * const trv_tbl); /* I/O [sct] Traversal table */

  void
  nco_ppc_around /* [fnc] Replace op1 values by their values rounded to decimal precision prc */
  (const int prc, /* I [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
   const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1); /* I/O [val] Values of first operand */

  void
  nco_ppc_bitmask /* [fnc] Mask-out insignificant bits of significand */
  (const int nsd, /* I [nbr] Number of significant digits, i.e., arithmetic precision */
   const nc_type type, /* I [enm] netCDF type of operand */
   const long sz, /* I [nbr] Size (in elements) of operand */
   const int has_mss_val, /* I [flg] Flag for missing values */
   ptr_unn mss_val, /* I [val] Value of missing value */
   ptr_unn op1); /* I/O [val] Values of first operand */
  
  double /* [frc] Rounded value */
  nco_ppc_bitmask_scl /* [fnc] Round input value significand by specified number of bits */
  (const double val_xct, /* I [frc] Exact value to round */
   const int bit_xpl_nbr_zro); /* I [nbr] Bits to round */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_PPC_H */
