/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_dmn_utl.h,v 1.1 2002-05-05 01:27:33 zender Exp $ */

/* Purpose: Dimension utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_dmn_utl.h" *//* Dimension utilities */

#ifndef NCO_DMN_UTL_H
#define NCO_DMN_UTL_H

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

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
dmn_dfn /* [fnc] Define dimensions in output file */
(const char * const fl_nm, /* I [sng] Name of output file */
 const int nc_id, /* I [id] netCDF output file ID */
 dmn_sct ** const dim, /* I [sct] Dimension structures to be defined in output file */
 const int nbr_dim); /* I [nbr] Number of dimension structures in structure list */

dmn_sct * /* O [sct] copy of input dimension structure */
dmn_dpl /* [fnc] Duplicate input dimension structure */
(const dmn_sct * const dim); /* I [sct] Dimension structure to duplicate */

dmn_sct * /* O [sct] Output dimension structure */
dmn_fll /* [fnc] Create and return a completed dmn_sct */
(const int nc_id, /* I [id] netCDF input file ID*/
 const int dmn_id, /* I [id] Dimension ID */
 const char * const dmn_nm); /* I [sng] Dimension name */

void
dmn_lmt_mrg /* [fnc] Merge limit structure information into dimension structures */
(dmn_sct ** const dim, /* I [sct] Dimension structures to modify */
 const int nbr_dim, /* I [nbr] Number of dimension structures in structure list */
 const lmt_sct * const lmt, /* I [sct] Dimension limit information */
 const int lmt_nbr); /* I [nbr] Number of dimensions with user-specified limits */

nm_id_sct * /* O [sct] Dimension list */
dmn_lst_mk /* [fnc] Attach dimension IDs to dimension list */
(const int nc_id, /* I [id] netCDF file ID */
 const char ** const dmn_lst_in, /* I [sng] User-specified list of dimension names */
 const int nbr_dim); /* I [nbr] Total number of dimensions in list */

nm_id_sct * /* O [sct] List of dimensions associated with input variable list */
dmn_lst_ass_var /* [fnc] Create list of all dimensions associated with input variable list */
(const int nc_id, /* I [id] netCDF input-file ID */
 const nm_id_sct * const var, /* I [sct] Variable list */
 const int nbr_var, /* I [nbr] Number of variables in list */
 int * const nbr_dim); /* O [nbr] Number of dimensions associated with input variable list */

void
dmn_xrf  /* [fnc] Crossreference xrf elements of dimension structures */
(dmn_sct * const dim, /* I/O [sct] Dimension structure */
 dmn_sct * const dmn_dpl); /* I/O [sct] Dimension structure */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_DMN_UTL_H */
