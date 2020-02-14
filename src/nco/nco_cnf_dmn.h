/* $Header$ */

/* Purpose: Conform dimensions */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_cnf_dmn.h" *//* Conform dimensions */

#ifndef NCO_CNF_DMN_H
#define NCO_CNF_DMN_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, qsort */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */
#include "nco_rth_utl.h" /* Arithmetic controls and utilities */
#include "nco_var_utl.h" /* Variable utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

var_sct * /* O [sct] Pointer to conforming variable structure */
nco_var_cnf_dmn /* [fnc] Stretch second variable to match dimensions of first variable */
(const var_sct * const var, /* I [ptr] Pointer to variable structure to serve as template */
 var_sct * const wgt, /* I [ptr] Pointer to variable structure to make conform to var */
 var_sct *wgt_crr, /* I/O [ptr] pointer to existing conforming variable structure, if any (destroyed when does not conform to var) */
 const nco_bool MUST_CONFORM, /* I [flg] Must wgt and var conform? */
 nco_bool *DO_CONFORM); /* O [flg] Do wgt and var conform? */

nco_bool /* [flg] var_1 and var_2 conform after processing */
ncap_var_cnf_dmn /* [fnc] Broadcast smaller variable into larger */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2); /* I/O [ptr] Second variable */

dmn_sct ** /* O [sct] Dimension structures to be re-ordered */
nco_dmn_avg_rdr_prp /* [fnc] Process dimension string list into dimension structure list */
(dmn_sct ** const dmn_in, /* I [sct] Dimension list for input file */
 char **dmn_rdr_lst, /* I [sng] Names of dimensions to be re-ordered */
 const int dmn_rdr_nbr); /* I [nbr] Number of dimension structures in re-order list */

char * /* [sng] Name of record dimension, if any, required by re-order */
nco_var_dmn_rdr_mtd /* [fnc] Change dimension ordering of variable metadata */
(const var_sct * const var_in, /* I [ptr] Variable with metadata and data in original order */
 var_sct * const var_out, /* I/O [ptr] Variable whose metadata will be re-ordered */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_rdr), /* I [sct] List of dimension structures in new order */
 const int dmn_rdr_nbr, /* I [nbr] Number of dimension structures in structure list */
 int * const dmn_idx_out_in, /* O [idx] Dimension correspondence, output->input */
 const nco_bool * const dmn_rvr_rdr, /* I [idx] Reverse dimension */
 nco_bool * const dmn_rvr_in); /* O [idx] Reverse dimension */

int /* O [enm] Return success code */
nco_var_dmn_rdr_val /* [fnc] Change dimension ordering of variable values */
(const var_sct * const var_in, /* I [ptr] Variable with metadata and data in original order */
 var_sct * const var_out, /* I/O [ptr] Variable whose data will be re-ordered */
 const int * const dmn_idx_out_in, /* I [idx] Dimension correspondence, output->input */
 const nco_bool * const dmn_rvr_in); /* I [idx] Reverse dimension */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNF_DMN_H */
