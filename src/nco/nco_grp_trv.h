/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.h,v 1.3 2012-08-09 02:01:21 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_grp_trv.h" *//* Group traversal */

#ifndef NCO_GRP_TRV_H
#define NCO_GRP_TRV_H

typedef enum{               /* [enm] netCDF4 object type: group, variable */
 nc_typ_grp,                   /* 0, Group */
 nc_typ_var                    /* 1, variable */
}nc_typ; 
/* end nc_typ */

/* struct to store basic info needed for the traversal algorythm: 
   this identifies an "object", as having a path and a type, like in HDF5 
*/
typedef struct{ 
  char     *nm;           /* [sng] Path name */
  nc_typ   typ;           /* [enm] netCDF4 object type: group, variable */
} grp_trv_t;
/* end nc_typ */

/* struct that stores all objects */
typedef struct{
  unsigned sz;            /* [nbr] Allocated size */
  unsigned nbr;           /* [nbr] Number of current elements */
  grp_trv_t *grp_lst;     /*       Array of grp_trv_t */
} grp_tbl_t;
/* end grp_tbl_t */

void                         
trv_tbl_init
(grp_tbl_t **tbl);        /* I/O [sct] Table */
/* end trv_tbl_init */

void 
trv_tbl_free
(grp_tbl_t *tbl);        /* I   [sct] Table */
/* end trv_tbl_free */

void 
trv_tbl_add
(grp_trv_t obj,          /* I   [sct] Object to store */
 grp_tbl_t *tbl);        /* I/O [sct] Table */
/* end trv_tbl_add */


#endif /* NCO_GRP_TRV_H */