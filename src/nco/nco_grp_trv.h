/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.h,v 1.2 2012-08-08 23:11:17 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_grp_trv.h" *//* Group traversal */

#ifndef NCO_GRP_TRV_H
#define NCO_GRP_TRV_H

/* struct to store basic info needed for the traversal algorythm */
typedef struct{ 
  char *nm; /* [sng] Path name */
} grp_trv_t;

/* struct that stores all objects */
typedef struct{
  unsigned sz;         /* [nbr] Allocated size */
  unsigned nbr;        /* [nbr] Number of current elements */
  grp_trv_t *grp_lst;  /*       Array of grp_trv_t */
} grp_tbl_t;

void                         
trv_tbl_init
(grp_tbl_t **tbl);  /* I/O [sct] Table */

void 
trv_tbl_free
(grp_tbl_t *tbl);   /* I   [sct] Table */

void 
trv_tbl_add
(const char *nm,    /* I   [sng] Path name */
 grp_tbl_t *tbl);   /* I/O [sct] Table */


#endif /* NCO_GRP_TRV_H */