/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnf_dmn.h,v 1.9 2004-01-01 20:41:43 zender Exp $ */

/* Purpose: Conform dimensions */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL)
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cnf_dmn.h" *//* Conform dimensions */

#ifndef NCO_CNF_DMN_H
#define NCO_CNF_DMN_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_ctl.h" /* Program flow control functions */
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
 const bool MUST_CONFORM, /* I [flg] Must wgt and var conform? */
 bool *DO_CONFORM); /* O [flg] Do wgt and var conform? */

bool /* [flg] Do var_1 and var_2 conform after processing? */
ncap_var_cnf_dmn /* [fnc] Broadcast smaller variable into larger */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2); /* I/O [ptr] Second variable */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNF_DMN_H */
