/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trav.c,v 1.2 2012-08-08 18:08:44 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender/Pedro Vicente
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */


/* Testing:
   ncks -D 1 ~/nco/data/in_grp.nc
 */

#include <assert.h>
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include "nco_grp_trav.h" /* Group traversal storage */
#include "nco_mmr.h" /* Memory management */


void                          
trav_table_init
(trav_table_t **tbl)       /* I/O [sct] table */
{
  unsigned int idx;
  trav_table_t* table=(trav_table_t*)nco_malloc(sizeof(trav_table_t));
  table->size=20;
  table->nobjs=0;
  table->objs=(trav_obj_t*)nco_malloc(table->size*sizeof(trav_obj_t));

  for(idx=0;idx<table->size;idx++){
    table->objs[idx].nm = NULL;
  }

  *tbl = table;
}
/* trav_table_init() */

void 
trav_table_free
(trav_table_t *table)   /* I [sct] table */
{
  unsigned int idx;

  for(idx=0;idx<table->size;idx++){
    nco_free(table->objs[idx].nm);
  }
  nco_free(table->objs);
  nco_free(table);
}
/* trav_table_free() */



void 
trav_table_add
(const char *name,       /* I   [sng] Path name */
 trav_table_t *table)    /* I/O [sct] Table */
{
  unsigned int idx;

  if(table->nobjs == table->size){
    table->size*=2;
    table->objs=(trav_obj_t*)nco_realloc(table->objs,table->size*sizeof(trav_obj_t));

    for(idx=table->nobjs;idx<table->size;idx++) {
      table->objs[idx].nm = NULL;
    } /* idx */
  } /* table->size */
  idx=table->nobjs++;
  table->objs[idx].nm=(char*)strdup(name);
}
/* trav_table_add() */

