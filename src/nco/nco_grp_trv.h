/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.h,v 1.10 2012-09-18 18:37:06 pvicente Exp $ */

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
 nc_typ_err=-1,                /* -1 Invalid type for initialization */
 nc_typ_grp,                   /*  0, Group */
 nc_typ_var                    /*  1, variable */
}nc_typ; 
/* end nc_typ */

/* struct to store basic info needed for the traversal algorythm: 
   this identifies an "object", as having a path and a type, like in HDF5 
*/
typedef struct{ 
  char     *nm_fll;         /* [sng] Absolute name (path) */
  nc_typ   typ;             /* [enm] netCDF4 object type: group, variable */
  char     nm[NC_MAX_NAME]; /* [sng] Relative name */
  int      flg;             /* [flg] mark flag (several actions) */
} grp_trv_sct;
/* end nc_typ */

/* struct that stores all objects */
typedef struct{
  unsigned sz;            /* [nbr] Allocated size */
  unsigned nbr;           /* [nbr] Number of current elements */
  grp_trv_sct *grp_lst;   /*       Array of grp_trv_sct */
} grp_tbl_sct;
/* end grp_tbl_sct */

void                         
trv_tbl_init
(grp_tbl_sct **tbl);        /* I/O [sct] Table */
/* end trv_tbl_init */

void 
trv_tbl_free
(grp_tbl_sct *tbl);        /* I   [sct] Table */
/* end trv_tbl_free */

void 
trv_tbl_add
(grp_trv_sct obj,          /* I   [sct] Object to store */
 grp_tbl_sct *tbl);        /* I/O [sct] Table */
/* end trv_tbl_add */


#endif /* NCO_GRP_TRV_H */
