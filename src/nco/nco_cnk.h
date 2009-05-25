/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnk.h,v 1.3 2009-05-25 18:12:45 zender Exp $ */

/* Purpose: Description (definition) of chunking functions */

/* Copyright (C) 1995--2009 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cnk.h" *//* Chunking */

#ifndef NCO_CNK_H
#define NCO_CNK_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_lst_utl.h" /* List utilities */
#include "nco_mmr.h" /* Memory management */

/* Chunking policies: 
   Handle each chunking policy in nco_cnk_mtd(), nco_cnk_val(), nco_var_dfn(), and end of ncpdq */
enum nco_cnk_plc{ /* [enm] Chunking policy */
  nco_cnk_plc_nil, /* 0 [enm] Do not think about chunking */
  nco_cnk_plc_all, /* 1 [enm] Chunk all variables */
  nco_cnk_plc_g2d, /* 2 [enm] Chunk variables >= two dimensions */
  nco_cnk_plc_g3d, /* 3 [enm] Chunk variables >= three dimensions */
  nco_cnk_plc_uck /* 4 [enm] Unchunk all chunked variables */
}; /* end nco_cnk_plc enum */

/* Chunking type maps: */
enum nco_cnk_map{ /* [enm] Chunking conversion map */
  nco_cnk_map_nil, /* 0 [enm] Do not chunk anything, storage is unchanged */
  nco_cnk_map_dmn, /* 1 [enm] Chunksize equals dimension size */
  nco_cnk_map_rcd_one, /* 2 [enm] Chunksize equals dimension size except record dimension has size one */
  nco_cnk_map_scl /* 3 [enm] Chunksize equals scalar size specified */
}; /* end nco_cnk_map enum */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 
nco_dfl_case_cnk_map_err(void); /* [fnc] Print error and exit for illegal switch(cnk_map) case */

void 
nco_dfl_case_cnk_plc_err(void); /* [fnc] Print error and exit for illegal switch(nco_cnk_plc) case */

const char * /* O [sng] Chunking map string */
nco_cnk_map_sng_get /* [fnc] Convert chunking map enum to string */
(const int nco_cnk_map); /* I [enm] Chunking map */

const char * /* O [sng] Chunking policy string */
nco_cnk_plc_sng_get /* [fnc] Convert chunking policy enum to string */
(const int nco_cnk_plc); /* I [enm] Chunking policy */

cnk_sct ** /* O [sct] Structure list with user-specified chunking information */
nco_cnk_prs /* [fnc] Create chunking structures with name and chunksize elements */
(const int cnk_nbr, /* I [nbr] Number of chunksizes specified */
 CST_X_PTR_CST_PTR_CST_Y(char,cnk_arg)); /* I [sng] List of user-specified chunksizes */

cnk_sct ** /* O [sct] Pointer to free'd structure list */
nco_cnk_lst_free /* [fnc] Free memory associated with chunking structure list */
(cnk_sct **cnk_lst, /* I/O [sct] Chunking structure list to free */
 const int cnk_nbr); /* I [nbr] Number of chunking structures in list */

cnk_sct * /* O [sct] Pointer to free'd chunking structure */
nco_cnk_free /* [fnc] Free all memory associated with chunking structure */
(cnk_sct *cnk); /* I/O [sct] Chunking structure to free */

size_t * /* O [nbr] Chunksize array for variable */
nco_cnk_sz_get /* [fnc] Determine chunksize array */
(const int out_id, /* I [id] netCDF output file ID */
 const int rec_dmn_id, /* I [id] Input file record dimension ID  */
 const char * const var_nm, /* I [sng] Variable name */
 const int cnk_map, /* I [enm] Chunking map */
 const int cnk_plc, /* I [enm] Chunking policy */
 const size_t cnk_sz_scl, /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr); /* I [nbr] Number of dimensions with user-specified chunking */

int /* O [enm] Chunking map */
nco_cnk_map_get /* [fnc] Convert user-specified chunking map to key */
(const char *nco_cnk_map_sng); /* [sng] User-specified chunking map */

int /* O [enm] Chunking policy */
nco_cnk_plc_get /* [fnc] Convert user-specified chunking policy to key */
(const char *nco_cnk_plc_sng); /* [sng] User-specified chunking policy */

#if 0 
nco_bool /* O [flg] NCO will attempt to chunk variable */
nco_is_chunkable /* [fnc] Will NCO attempt to chunk variable? */
(const nc_type nc_typ_in); /* I [enm] Type of input variable */

nco_bool /* O [flg] Chunking policy allows chunking nc_typ_in */
nco_cnk_plc_typ_get /* [fnc] Determine type, if any, to chunk input type to */
(const int nco_cnk_map, /* I [enm] Chunking map */
 const nc_type nc_typ_in, /* I [enm] Type of input variable */
 nc_type *nc_typ_cnk_out); /* O [enm] Type to chunk variable to */

void
nco_cnk_mtd /* [fnc] Alter metadata according to chunking specification */
(const var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * const var_out, /* I/O [ptr] Variable whose metadata will be altered */
 const int nco_cnk_map, /* I [enm] Chunking map */
 const int nco_cnk_plc); /* I [enm] Chunking policy */

void
nco_cnk_val /* [fnc] Chunk variable according to chunking specification */
(var_sct * const var_in, /* I [ptr] Variable in original disk state */
 var_sct * var_out, /* I/O [ptr] Variable after chunking/unchunking operation */
 const int nco_cnk_map, /* I [enm] Chunking map */
 const int nco_cnk_plc, /* I [enm] Chunking policy */
 aed_sct * const aed_lst_add_fst, /* O [enm] Attribute edit structure, add_offset */
 aed_sct * const aed_lst_scl_fct); /* O [enm] Attribute edit structure, scale_factor */

int /* O [enm] Chunking map */
nco_cnk_map_get /* [fnc] Convert user-specified chunking map to key */
(const char *nco_cnk_map_sng); /* [sng] User-specified chunking map */

int /* O [enm] Chunking policy */
nco_cnk_plc_get /* [fnc] Convert user-specified chunking policy to key */
(const char *nco_cnk_plc_sng); /* [sng] User-specified chunking policy */
  
nco_bool /* O [flg] Variable is chunked on disk */
nco_cnk_dsk_inq /* [fnc] Check whether variable is chunked on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 var_sct *var); /* I/O [sct] Variable */
  
var_sct * /* O [sct] Chunked variable */
nco_put_var_cnk /* [fnc] Chunk variable in memory and write chunking attributes to disk */
(const int out_id, /* I [id] netCDF output file ID */
 var_sct *var, /* I/O [sct] Variable to be chunked */
 const int nco_cnk_plc); /* [enm] Chunking operation type */

var_sct * /* O [sct] Chunked variable */
nco_var_cnk /* [fnc] Chunk variable in memory */
(var_sct *var, /* I/O [sct] Variable to be chunked */
 const nc_type typ_cnk, /* I [enm] Type of variable when chunked (on disk). This should be same as typ_dsk except in cases where variable is chunked in input file and unchunked in output file. */
 nco_bool *CNK_VAR_WITH_NEW_CNK_ATT); /* O [flg] Routine generated new scale_factor/add_offset */

var_sct * /* O [sct] Unchunked variable */
nco_var_uck /* [fnc] Unchunk variable in memory */
(var_sct * const var); /* I/O [sct] Variable to be unchunked */

void
nco_var_uck_swp /* [fnc] Unchunk var_in into var_out */
(var_sct * const var_in, /* I/O [sct] Variable to unchunk */
 var_sct * const var_out); /* I/O [sct] Variable to unchunk into */
#endif /* endif 0 */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CNK_H */
