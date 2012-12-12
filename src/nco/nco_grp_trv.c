/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.c,v 1.22 2012-12-12 02:49:47 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Testing:
   ncks -D 1 ~/nco/data/in_grp.nc
 */

#include "nco_grp_trv.h" /* Group traversal */

void                          
trv_tbl_init
(trv_tbl_sct **tbl) /* I/O [sct] Traversal table */
{
  unsigned int idx;
  trv_tbl_sct *tb=(trv_tbl_sct *)nco_malloc(sizeof(trv_tbl_sct));
  tb->sz=100;
  tb->nbr=0;
  tb->lst=(trv_sct *)nco_malloc(tb->sz*sizeof(trv_sct));

  for(idx=0;idx<tb->sz;idx++){
    tb->lst[idx].nm_fll  = NULL;
    tb->lst[idx].nm_fll_lng  = 0L;
    tb->lst[idx].grp_nm_fll  = NULL;
    tb->lst[idx].typ     = nco_obj_typ_err;
    tb->lst[idx].nm[0]   = '\0';
    tb->lst[idx].nm_lng  = 0L;
    tb->lst[idx].flg     = nco_obj_typ_err;
    tb->lst[idx].xcl_flg = nco_obj_typ_err;
    tb->lst[idx].flg_mch = False;
    tb->lst[idx].flg_rcr = False;
    tb->lst[idx].nbr_att = nco_obj_typ_err;
    tb->lst[idx].nbr_var = nco_obj_typ_err;
    tb->lst[idx].nbr_dmn = nco_obj_typ_err;
    tb->lst[idx].nbr_grp = nco_obj_typ_err;
  } /* end loop over objects */

  *tbl = tb;
} /* trv_tbl_init() */

void 
trv_tbl_free
(trv_tbl_sct *tbl)   /* I [sct] Traversal table */
{
  unsigned int idx;

  for(idx=0;idx<tbl->sz;idx++){
    nco_free(tbl->lst[idx].nm_fll);
    nco_free(tbl->lst[idx].grp_nm_fll);
  } /* end loop */
  nco_free(tbl->lst);
  nco_free(tbl);
} /* end trv_tbl_free() */

void 
trv_tbl_add
(trv_sct obj,              /* I   [sct] Object to store */
 trv_tbl_sct *tbl)         /* I/O [sct] Traversal table */
{
  unsigned int idx;

  if(tbl->nbr == tbl->sz){
    tbl->sz*=2;
    tbl->lst=(trv_sct*)nco_realloc(tbl->lst,tbl->sz*sizeof(trv_sct));

    for(idx=tbl->nbr;idx<tbl->sz;idx++){
      tbl->lst[idx].nm_fll=NULL;
      tbl->lst[idx].grp_nm_fll=NULL;
    } /* idx */
  } /* tbl->sz */
  idx=tbl->nbr++;
  tbl->lst[idx].nm_fll=(char *)strdup(obj.nm_fll);
  tbl->lst[idx].nm_fll_lng=obj.nm_fll_lng;
  tbl->lst[idx].grp_nm_fll=(char *)strdup(obj.grp_nm_fll);
  strcpy(tbl->lst[idx].nm,obj.nm);
  tbl->lst[idx].nm_lng=obj.nm_lng;
  tbl->lst[idx].typ=obj.typ;
  tbl->lst[idx].flg=obj.flg;
  tbl->lst[idx].xcl_flg=obj.xcl_flg;
  tbl->lst[idx].flg_mch=obj.flg_mch;
  tbl->lst[idx].flg_rcr=obj.flg_rcr;
  tbl->lst[idx].nbr_att=obj.nbr_att;
  tbl->lst[idx].nbr_var=obj.nbr_var;
  tbl->lst[idx].nbr_dmn=obj.nbr_dmn;
  tbl->lst[idx].nbr_grp=obj.nbr_grp;
} /* end trv_tbl_add() */
