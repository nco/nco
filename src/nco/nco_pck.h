/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_pck.h,v 1.1 2002-05-02 06:10:30 zender Exp $ */

/* Purpose: Description (definition) of packing/unpacking functions */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "pck.h" *//* Packing and unpacking variables */

#ifndef PCK_H
#define PCK_H

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
#include "nco_netcdf.h" /* netCDF 3.0 wrapper functions */

enum nco_pck_typ{ /* [enm] Packing type */
  nco_pck_all_xst_att, /* 0, Pack all variables, keeping existing packing attributes if any */
  nco_pck_all_new_att, /* 1, Pack all variables, always generating new packing attributes */
  nco_pck_xst_xst_att, /* 2, Pack existing packed variables, keeping existing packing attributes if any */
  nco_pck_xst_new_att, /* 3, Pack existing packed variables, always generating new packing attributes */
  nco_pck_upk, /* 4, Unpack all packed variables */
  nco_pck_nil /* 5, Do not think about packing */
}; /* end nco_pck_typ enum */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  extern int nco_pck_typ_get(char *);

  extern bool /* O [flg] Variable is packed */
  pck_dsk_inq /* [fnc] Check whether variable is packed */
  (int nc_id, /* I [idx] netCDF file ID */
   var_sct *var); /* I/O [sct] Variable */
  
  extern var_sct * /* O [sct] Unpacked variable */
  var_upk /* [fnc] Unpack variable in memory */
  (var_sct *var); /* I/O [sct] Variable to be unpacked */

  extern var_sct * /* O [sct] Packed variable */
  var_pck /* [fnc] Pack variable in memory */
  (var_sct *var, /* I/O [sct] Variable to be packed */
   nc_type typ_pck, /* I [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
   bool USE_EXISTING_PCK); /* I [flg] Use existing packing scale_factor and add_offset */

  extern var_sct * /* O [sct] Packed variable */
  nco_put_var_pck /* [fnc] Pack variable in memory and write packing attributes to disk */
  (var_sct *var, /* I/O [sct] Variable to be packed */
   int nco_pck_typ); /* [enm] Packing operation type */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* PCK_H */
