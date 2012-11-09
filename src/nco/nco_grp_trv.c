/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.c,v 1.12 2012-11-09 23:21:12 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */


/* Testing:
   ncks -D 1 ~/nco/data/in_grp.nc
 */

#include "nco_grp_trv.h" /* Group traversal storage */

void                          
trv_tbl_init
(grp_tbl_sct **tbl)       /* I/O [sct] Traversal table */
{
  unsigned int idx;
  grp_tbl_sct* tb=(grp_tbl_sct*)nco_malloc(sizeof(grp_tbl_sct));
  tb->sz=100;
  tb->nbr=0;
  tb->grp_lst=(grp_trv_sct*)nco_malloc(tb->sz*sizeof(grp_trv_sct));

  for(idx=0;idx<tb->sz;idx++){
    tb->grp_lst[idx].nm_fll  = NULL;
    tb->grp_lst[idx].grp_nm_fll  = NULL;
    tb->grp_lst[idx].typ     = nc_typ_err;
    tb->grp_lst[idx].nm[0]   = '\0';
    tb->grp_lst[idx].flg     = -1;
    tb->grp_lst[idx].nbr_att = -1;
    tb->grp_lst[idx].nbr_var = -1;
    tb->grp_lst[idx].nbr_dmn = -1;
    tb->grp_lst[idx].nbr_grp = -1;
  }

  *tbl = tb;
}
/* trv_tbl_init() */

void 
trv_tbl_free
(grp_tbl_sct *tbl)   /* I [sct] Traversal table */
{
  unsigned int idx;

  for(idx=0;idx<tbl->sz;idx++){
    nco_free(tbl->grp_lst[idx].nm_fll);
    nco_free(tbl->grp_lst[idx].grp_nm_fll);
  }
  nco_free(tbl->grp_lst);
  nco_free(tbl);
}
/* trv_tbl_free() */



void 
trv_tbl_add
(grp_trv_sct obj,          /* I   [sct] Object to store */
 grp_tbl_sct *tbl)         /* I/O [sct] Traversal table */
{
  unsigned int idx;

  if(tbl->nbr == tbl->sz){
    tbl->sz*=2;
    tbl->grp_lst=(grp_trv_sct*)nco_realloc(tbl->grp_lst,tbl->sz*sizeof(grp_trv_sct));

    for(idx=tbl->nbr;idx<tbl->sz;idx++) {
      tbl->grp_lst[idx].nm_fll = NULL;
      tbl->grp_lst[idx].grp_nm_fll = NULL;
    } /* idx */
  } /* tbl->sz */
  idx=tbl->nbr++;
  tbl->grp_lst[idx].nm_fll=(char*)strdup(obj.nm_fll);
  tbl->grp_lst[idx].grp_nm_fll=(char*)strdup(obj.grp_nm_fll);
  strcpy(tbl->grp_lst[idx].nm,obj.nm);
  tbl->grp_lst[idx].typ=obj.typ;
  tbl->grp_lst[idx].nbr_att=obj.nbr_att;
  tbl->grp_lst[idx].nbr_var=obj.nbr_var;
  tbl->grp_lst[idx].nbr_dmn=obj.nbr_dmn;
  tbl->grp_lst[idx].nbr_grp=obj.nbr_grp;
}
/* trv_tbl_add() */

