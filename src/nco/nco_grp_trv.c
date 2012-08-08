/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.c,v 1.1 2012-08-08 22:40:54 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */


/* Testing:
   ncks -D 1 ~/nco/data/in_grp.nc
 */

#include <assert.h>
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include "nco_grp_trv.h" /* Group traversal storage */
#include "nco_mmr.h" /* Memory management */


void                          
trv_tbl_init
(grp_tbl_t **tbl)       /* I/O [sct] Table */
{
  unsigned int idx;
  grp_tbl_t* tb=(grp_tbl_t*)nco_malloc(sizeof(grp_tbl_t));
  tb->size=100;
  tb->nobjs=0;
  tb->objs=(grp_trv_t*)nco_malloc(tb->size*sizeof(grp_trv_t));

  for(idx=0;idx<tb->size;idx++){
    tb->objs[idx].nm = NULL;
  }

  *tbl = tb;
}
/* trv_tbl_init() */

void 
trv_tbl_free
(grp_tbl_t *tbl)   /* I [sct] Table */
{
  unsigned int idx;

  for(idx=0;idx<tbl->size;idx++){
    nco_free(tbl->objs[idx].nm);
  }
  nco_free(tbl->objs);
  nco_free(tbl);
}
/* trv_tbl_free() */



void 
trv_tbl_add
(const char *name,       /* I   [sng] Path name */
 grp_tbl_t *tbl)    /* I/O [sct] Table */
{
  unsigned int idx;

  if(tbl->nobjs == tbl->size){
    tbl->size*=2;
    tbl->objs=(grp_trv_t*)nco_realloc(tbl->objs,tbl->size*sizeof(grp_trv_t));

    for(idx=tbl->nobjs;idx<tbl->size;idx++) {
      tbl->objs[idx].nm = NULL;
    } /* idx */
  } /* tbl->size */
  idx=tbl->nobjs++;
  tbl->objs[idx].nm=(char*)strdup(name);
}
/* trv_tbl_add() */

