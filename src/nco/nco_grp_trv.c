/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.c,v 1.25 2012-12-14 03:40:31 pvicente Exp $ */

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


void                       
trv_tbl_inq                          /* [fnc] Find and return global totals of dimensions, variables, attributes */
(int * const att_nbr_glb,            /* O [nbr] Number of global attributes in file */
 int * const dmn_nbr_all,            /* O [nbr] Number of dimensions in file */
 int * const var_nbr_all,            /* O [nbr] Number of variables in file  */
 int * const grp_nbr_all,            /* O [nbr] Number of groups in file */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  /* [fnc] Find and return global totals of dimensions, variables, attributes */
  int att_nbr_lcl; /* [nbr] Number of global attributes in file */
  int dmn_nbr_lcl; /* [nbr] Number of dimensions in file */
  int var_nbr_lcl; /* [nbr] Number of variables in file */
  int grp_nbr_lcl; /* [nbr] Number of groups in file */

  /* Initialize */
  att_nbr_lcl=0;
  dmn_nbr_lcl=0;
  var_nbr_lcl=0;
  grp_nbr_lcl=0;

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx]; 
    if (trv.typ == nco_obj_typ_grp ) { 

      /* Increment/Export */
      att_nbr_lcl+=trv.nbr_att; 
      dmn_nbr_lcl+=trv.nbr_dmn;
      var_nbr_lcl+=trv.nbr_var;
      grp_nbr_lcl+=trv.nbr_grp;
    } /* end nco_obj_typ_grp */
  } /* end uidx  */

  if(att_nbr_glb) *att_nbr_glb=att_nbr_lcl;
  if(dmn_nbr_all) *dmn_nbr_all=dmn_nbr_lcl;
  if(var_nbr_all) *var_nbr_all=var_nbr_lcl;
  if(grp_nbr_all) *grp_nbr_all=grp_nbr_lcl;

  return;
} /* end trv_tbl_inq() */


void 
trv_tbl_prn_flg_mch                  /* [fnc] Print table items that have .flg_mch  */
(const trv_tbl_sct * const trv_tbl,  /* I [sct] Traversal table */
 const nco_obj_typ obj_typ)          /* I [enm] Object type (group or variable) */
{
  /* Print all matching objects of given type from traversal table */
  trv_sct trv_obj;
  for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){
    trv_obj=trv_tbl->lst[tbl_idx];
    if((trv_obj.typ == obj_typ) && (trv_obj.flg_mch == True)) 
      (void)fprintf(stdout,"nm_fll=%s\n",trv_obj.nm_fll);
  } /* end loop over trv_tbl */
}/* end trv_tbl_prn_flg_mch() */


void                          
trv_tbl_prn                          /* [fnc] Print table with -z */
(const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */  
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_grp ) {
      (void)fprintf(stdout,"grp: ");
    } else if (trv.typ == nco_obj_typ_var ) {
      (void)fprintf(stdout,"var: ");
    }
    (void)fprintf(stdout,"%s\n",trv_tbl->lst[uidx].nm_fll); 
  } /* end uidx */
} /* end trv_tbl_prn() */

nco_bool                              /* O [nbr] Item found or not */
trv_tbl_fnd_var_nm_fll                /* [fnc] Check if "var_nm_fll" is in table */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->lst[uidx].typ == nco_obj_typ_var && strcmp(var_nm_fll,trv_tbl->lst[uidx].nm_fll) == 0){
      return True;
    } /* end nco_obj_typ_var */
  } /* end loop over uidx */
  return False;
} /* end trv_tbl_fnd_var_nm_fll() */

nco_bool                              /* O [nbr] Item found or not */
trv_tbl_fnd_var_nm                    /* [fnc] Find a variable that matches parameter "var_nm" */
(const char * const var_nm,           /* I [sng] Variable (relative) name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if(trv.typ == nco_obj_typ_var && strcmp(trv.nm,var_nm) == 0){ 
      return True;
    } /* end nco_obj_typ_var */
  } /* end uidx  */
  return False;
} /* end trv_tbl_fnd_var_nm() */ 

void
trv_tbl_mrk                           /* [fnc] Mark "var_nm_fll" with .flg flag in table */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (strcmp(var_nm_fll,trv_tbl->lst[uidx].nm_fll) == 0){
#ifdef NCO_SANITY_CHECK
      assert(trv_tbl->lst[uidx].typ == nco_obj_typ_var);
#endif
      trv_tbl->lst[uidx].flg=True;
    } /* end strcmp */
  } /* end loop over uidx */
  return;
} /* end trv_tbl_mrk() */

void 
trv_tbl_prn_xtr                      /* [fnc] Print extraction .flg members of traversal table */
(const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  int nbr_flg=0; /* [nbr] Number of marked .flg items in table */
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->lst[uidx].flg == True){
#ifdef NCO_SANITY_CHECK
      assert(trv_tbl->lst[uidx].typ == nco_obj_typ_var);
#endif
      nbr_flg++;
    } /* end flg == True */
  } /* end loop over uidx */

  (void)fprintf(stdout,"%s: INFO Table: %d extraction variables\n",prg_nm_get(),nbr_flg); 
  int idx=0;
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    /* Object is marked to export */
    if(trv_tbl->lst[uidx].flg == True){
      (void)fprintf(stdout,"[%d] %s\n",idx,trv_tbl->lst[uidx].nm_fll); 
      assert(trv_tbl->lst[uidx].typ == nco_obj_typ_var);
      idx++;
    } /* end flg */
  } /* end uidx */
} /* end trv_tbl_prn_xtr() */
