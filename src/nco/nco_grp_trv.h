/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.h,v 1.18 2012-11-16 18:13:00 zender Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_grp_trv.h" *//* Group traversal */

#ifndef NCO_GRP_TRV_H
#define NCO_GRP_TRV_H

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */

/* Personal headers */
#include "nco_mmr.h" /* Memory management */

typedef enum{               /* [enm] netCDF4 object type: group, variable */
 nco_obj_typ_err=-1,        /* -1 Invalid type for initialization */
 nco_obj_typ_grp,           /*  0, Group */
 nco_obj_typ_var            /*  1, variable */
} nco_obj_typ; 
/* end nco_obj_typ */

/* Structure to store basic info needed for the traversal algorithm: 
   this identifies an "object", as having a path and a type, like in HDF5 
*/
typedef struct{ 
  char    *nm_fll;         /* [sng] Absolute name (path): this can be a variable or group */
  size_t  nm_fll_lng;      /* [sng] Length of full name */
  char    *grp_nm_fll;     /* [sng] Full group name (for variables, for convenience to avoid to obtain several times) */
  nco_obj_typ typ;         /* [enm] netCDF4 object type: group or variable */
  char    nm[NC_MAX_NAME]; /* [sng] Relative name */
  int     flg;             /* [flg] mark flag (several actions) */
  int     nbr_att;         /* [nbr] Number of attributes */
  int     nbr_var;         /* [nbr] Number of variables ( for group ) */
  int     nbr_dmn;         /* [nbr] Number of dimensions */
  int     nbr_grp;         /* [nbr] Number of sub-groups ( for group ) */
} grp_trv_sct;
/* end grp_trv_sct */

/* Structure that stores all objects */
typedef struct{
  unsigned int sz;        /* [nbr] Allocated size */
  unsigned int nbr;       /* [nbr] Number of current elements */
  grp_trv_sct *grp_lst;   /* [sct] Array of grp_trv_sct */
} grp_tbl_sct;
/* end grp_tbl_sct */

void                         
trv_tbl_init
(grp_tbl_sct **tbl);        /* I/O [sct] Traversal table */
/* end trv_tbl_init */

void 
trv_tbl_free
(grp_tbl_sct *tbl);        /* I   [sct] Traversal table */
/* end trv_tbl_free */

void 
trv_tbl_add
(grp_trv_sct obj,          /* I   [sct] Object to store */
 grp_tbl_sct *tbl);        /* I/O [sct] Traversal table */
/* end trv_tbl_add */

#endif /* NCO_GRP_TRV_H */
