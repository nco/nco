/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_att_utl.h,v 1.10 2002-10-25 20:04:45 zender Exp $ */

/* Purpose: Attribute utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_att_utl.h" *//* Attribute utilities */

#ifndef NCO_ATT_UTL_H
#define NCO_ATT_UTL_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */
#include <time.h> /* machine time */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_mss_val.h" /* Missing value utilities */
#if (!defined HAVE_STRCASECMP) || (!defined HAVE_STRDUP)
#include "nco_sng_utl.h" /* String utilities */
#endif /* HAVE_STRCASECMP || HAVE_STRDUP */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
nco_aed_prc /* [fnc] Process single attribute edit for single variable */
(const int nc_id, /* I [id] Input netCDF file ID */
 const int var_id, /* I [id] ID of variable on which to perform attribute editing */
 const aed_sct aed); /* I [id] Structure containing information necessary to edit */

void 
nco_att_cpy  /* [fnc] Copy attributes from input netCDF file to output netCDF file */
(const int in_id, /* I [id] netCDF input-file ID */
 const int out_id, /* I [id] netCDF output-file ID */
 const int var_in_id, /* I [id] netCDF input-variable ID */
 const int var_out_id, /* I [id] netCDF output-variable ID */
 const bool PCK_ATT_CPY); /* I [flg] Copy attributes "scale_factor", "add_offset" */

void 
nco_hst_att_cat /* [fnc] Add command line, date stamp to history attribute */
(const int out_id, /* I [id] netCDF output-file ID */
 const char * const hst_sng); /* I [sng] String to add to history attribute */

int /* [flg] Variable and attribute names are conjoined */
nco_prs_att /* [fnc] Parse conjoined variable and attribute names */
(rnm_sct * const rnm_att, /* I/O [sct] Structure [Variable:]Attribute name on input, Attribute name on output */
 char * const var_nm); /* O [sng] Variable name, if any */

rnm_sct * /* O [sng] Structured list of old, new names */
nco_prs_rnm_lst /* [fnc] Set old_nm, new_nm elements of rename structure */
(const int nbr_rnm, /* I [nbr] Number of elements in rename list */
 CST_X_PTR_CST_PTR_CST_Y(char,rnm_arg)); /* I [sng] Unstructured list of old, new names */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_ATT_UTL_H */
