/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnf_typ.h,v 1.1 2002-05-02 06:10:30 zender Exp $ */

/* Purpose: Conform variable types */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cnf_typ.h" *//*  */

#ifndef NCO_CNF_TYP_H
#define NCO_CNF_TYP_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

/* Personal headers */
#include "nco.h" /* NCO definitions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

var_sct * /* O [var] Variable after (possible) conversion */
nco_typ_cnv_rth  /* [fnc] Convert char, short, long, int types to doubles before arithmetic */
(var_sct *var, /* I/O [var] Variable to be considered for conversion */
 const int nco_op_typ); /* I [enm] Operation type */

var_sct * /* O [sct] Variable reverted to on-disk type */
nco_cnv_var_typ_dsk  /* [fnc] Revert variable to on-disk type */
(var_sct *var); /* I [sct] Variable to be reverted */

var_sct * /* O [sct] Pointer to variable structure of type var_out_type */
var_conform_type /* [fnc] Return copy of input variable typecast to desired type */
(const nc_type var_out_type, /* I [enm] Type to convert variable structure to */
 var_sct const *var_in); /* I/O [enm] Pointer to variable structure (may be destroyed) */

void
val_conform_type /* [fnc] Copy val_in and typecast from typ_in to typ_out */
(const nc_type typ_in, /* I [enm] Type of input value */
 ptr_unn val_in, /* I [ptr] Pointer to input value */
 const nc_type typ_out, /* I [enm] Type of output value */
 ptr_unn val_out); /* I [ptr] Pointer to output value */

int /* O [enm] Dummy return */
scv_conform_type /* [fnc] Convert scalar attribute to typ_new using C implicit coercion */
(const nc_type typ_new, /* I [enm] Type to convert scv_old to */
 scv_sct const *scv_old); /* I/O [sct] Scalar value to convert */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNF_TYP_H */
