/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trav.h,v 1.2 2012-08-08 18:08:44 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender/Pedro Vicente
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_trav.h" *//* Group traversal */

#ifndef NCO_TRAV_H
#define NCO_TRAV_H

/* struct to store basic info needed for the traversal algorythm */
typedef struct{ 
  char *nm; /* [sng] Path name */
} trav_obj_t;

/* struct that stores all objects */
typedef struct{
  unsigned size;
  unsigned nobjs;
  trav_obj_t *objs;
} trav_table_t;

void                         
trav_table_init
(trav_table_t **table);  /* I/O [sct] table */

void 
trav_table_free
(trav_table_t *table);   /* I [sct] table */

void 
trav_table_add
(const char *name,       /* I   [sng] Path name */
 trav_table_t *table);   /* I/O [sct] Table */


#endif