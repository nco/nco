/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rth_utl.h,v 1.2 2002-05-07 08:34:15 zender Exp $ */

/* Purpose: Arithmetic controls and utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_rth_utl.h" *//* Arithmetic controls and utilities */

#ifndef NCO_RTH_UTL_H
#define NCO_RTH_UTL_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */
#include <unistd.h> /* POSIX stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_var_utl.h" /* Variable utilities */
#include "nco_var_rth.h" /* Variable arithmetic */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int /* O [enm] Arithmetic operation */
nco_op_typ_get /* [fnc] Convert user-specified operation into operation key */
(const char * const nco_op_sng); /* I [sng] User-specified operation */

void 
nco_opr_drv /* [fnc] Intermediate control of arithmetic operations for ncra/ncea */
(const int idx_rec, /* I [idx] Index of current record */
 const int nco_op_typ, /* I [enm] Operation type */
 var_sct * const var_prc_out, /* I/O [sct] Variable in output file */
 const var_sct * const var_prc); /* I [sct] Variable in input file */

int /* O [enm] Relational operation */
op_prs_rlt /* [fnc] Convert Fortran abbreviation for relational operator into NCO operation key */
(const char * const op_sng); /* I [sng] Fortran representation of relational operator */

void
vec_set /* [fnc] Fill every value of first operand with value of second operand */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] size (in elements) of operand */
 ptr_unn op1, /* I [sct] Values of first operand */
 const double op2); /* I [frc] Value to fill vector with */

void
zero_long /* [fnc] Zero all values of long array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 long * const op1); /* I/O [nbr] Array to be zeroed */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_RTH_UTL_H */
