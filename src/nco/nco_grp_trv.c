/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.c,v 1.33 2013-01-21 06:00:12 zender Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Testing:
   ncks -D 1 ~/nco/data/in_grp.nc */

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
    tb->lst[idx].nm_fll=NULL;
    tb->lst[idx].nm_fll_lng=0L;
    tb->lst[idx].grp_nm_fll=NULL;
    tb->lst[idx].typ=nco_obj_typ_err;
    tb->lst[idx].nm[0]='\0';
    tb->lst[idx].nm_lng=0L;

    tb->lst[idx].flg_cf=False; /* [flg] Object matches CF-metadata extraction criteria */
    tb->lst[idx].flg_crd=False; /* [flg] Object matches coordinate extraction criteria */
    tb->lst[idx].flg_dfl=False; /* [flg] Object meets default subsetting criteria */
    tb->lst[idx].flg_gcv=False; /* [flg] Group contains matched variable */
    tb->lst[idx].flg_mch=False; /* [flg] Object matches user-specified strings */
    tb->lst[idx].flg_ncs=False; /* [flg] Group is ancestor of specified group or variable */
    tb->lst[idx].flg_nsx=False; /* [flg] Object matches intersection criteria */
    tb->lst[idx].flg_rcr=False; /* [flg] Extract group recursively */
    tb->lst[idx].flg_unn=False; /* [flg] Object matches union criteria */
    tb->lst[idx].flg_vfp=False; /* [flg] Variable matches full path specification */
    tb->lst[idx].flg_vsg=False; /* [flg] Variable selected because group matches */
    tb->lst[idx].flg_xcl=False; /* [flg] Object matches exclusion criteria */
    tb->lst[idx].flg_xtr=False; /* [flg] Extract object */

    tb->lst[idx].grp_dpt=nco_obj_typ_err; /* [nbr] Depth of group (root = 0) */
    tb->lst[idx].grp_id_in=nco_obj_typ_err; /* [id] Group ID in input file */
    tb->lst[idx].grp_id_out=nco_obj_typ_err; /* [id] Group ID in output file */

    tb->lst[idx].nbr_att=nco_obj_typ_err;
    tb->lst[idx].nbr_var=nco_obj_typ_err;
    tb->lst[idx].nbr_dmn=nco_obj_typ_err;
    tb->lst[idx].nbr_grp=nco_obj_typ_err;
  } /* end loop over objects */

  *tbl=tb;
} /* trv_tbl_init() */

void 
trv_tbl_free
(trv_tbl_sct *tbl) /* I [sct] Traversal table */
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
(trv_sct const obj, /* I [sct] Object to store */
 trv_tbl_sct * const tbl) /* I/O [sct] Traversal table */
{
  unsigned int idx;

  if(tbl->nbr == tbl->sz){
    tbl->sz*=2;
    tbl->lst=(trv_sct *)nco_realloc(tbl->lst,tbl->sz*sizeof(trv_sct));

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

  tbl->lst[idx].flg_cf=obj.flg_cf; /* [flg] Object matches CF-metadata extraction criteria */
  tbl->lst[idx].flg_crd=obj.flg_crd; /* [flg] Object matches coordinate extraction criteria */
  tbl->lst[idx].flg_dfl=obj.flg_dfl; /* [flg] Object meets default subsetting criteria */
  tbl->lst[idx].flg_gcv=obj.flg_gcv; /* [flg] Group contains matched variable */
  tbl->lst[idx].flg_mch=obj.flg_mch; /* [flg] Object matches user-specified strings */
  tbl->lst[idx].flg_ncs=obj.flg_ncs; /* [flg] Group is ancestor of specified group or variable */
  tbl->lst[idx].flg_nsx=obj.flg_nsx; /* [flg] Object matches intersection criteria */
  tbl->lst[idx].flg_rcr=obj.flg_rcr; /* [flg] Extract group recursively */
  tbl->lst[idx].flg_unn=obj.flg_unn; /* [flg] Object matches union criteria */
  tbl->lst[idx].flg_vfp=obj.flg_vfp; /* [flg] Variable matches full path specification */
  tbl->lst[idx].flg_vsg=obj.flg_vsg; /* [flg] Variable selected because group matches */
  tbl->lst[idx].flg_xcl=obj.flg_xcl; /* [flg] Object matches exclusion criteria */
  tbl->lst[idx].flg_xtr=obj.flg_xtr; /* [flg] Extract object */

  tbl->lst[idx].grp_dpt=obj.grp_dpt; /* [nbr] Depth of group (root = 0) */
  tbl->lst[idx].grp_id_in=obj.grp_id_in; /* [id] Group ID in input file */
  tbl->lst[idx].grp_id_out=obj.grp_id_out; /* [id] Group ID in output file */

  tbl->lst[idx].nbr_att=obj.nbr_att;
  tbl->lst[idx].nbr_dmn=obj.nbr_dmn;
  tbl->lst[idx].nbr_grp=obj.nbr_grp;
  tbl->lst[idx].nbr_var=obj.nbr_var;
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
    if(trv.typ == nco_obj_typ_grp){ 
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
    if((trv_obj.typ == obj_typ) && trv_obj.flg_mch) (void)fprintf(stdout,"nm_fll=%s\n",trv_obj.nm_fll);
  } /* end loop over trv_tbl */
}/* end trv_tbl_prn_flg_mch() */

void                          
trv_tbl_prn                          /* [fnc] Print table with -z */
(const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */  
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if(trv.typ == nco_obj_typ_grp) (void)fprintf(stdout,"grp: "); else (void)fprintf(stdout,"var: ");
    (void)fprintf(stdout,"%s\n",trv_tbl->lst[uidx].nm_fll); 
  } /* end uidx */
} /* end trv_tbl_prn() */

nco_bool                              /* O [nbr] Item found or not */
trv_tbl_fnd_var_nm_fll                /* [fnc] Check if "var_nm_fll" is in table */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++)
    if(trv_tbl->lst[uidx].typ == nco_obj_typ_var && !strcmp(var_nm_fll,trv_tbl->lst[uidx].nm_fll))
      return True;

  return False;
} /* end trv_tbl_fnd_var_nm_fll() */

nco_bool                              /* O [flg] Item found */
trv_tbl_fnd_var_nm                    /* [fnc] Find a variable that matches parameter "var_nm" */
(const char * const var_nm,           /* I [sng] Variable (relative) name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if(trv.typ == nco_obj_typ_var && !strcmp(trv.nm,var_nm)) return True;
  } /* end loop over uidx */

  return False;
} /* end trv_tbl_fnd_var_nm() */ 

void
trv_tbl_mrk_xtr                       /* [fnc] Mark extraction flag in table for "var_nm_fll" */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++)
    if(!strcmp(var_nm_fll,trv_tbl->lst[uidx].nm_fll)) trv_tbl->lst[uidx].flg_xtr=True;

  return;
} /* end trv_tbl_mrk_xtr() */

void 
trv_tbl_prn_xtr                      /* [fnc] Print extraction flag of traversal table */
(const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  int idx=0;
  int nbr_flg=0; 

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++)
    if(trv_tbl->lst[uidx].flg_xtr){
      (void)fprintf(stdout,"[%d] %s\n",idx++,trv_tbl->lst[uidx].nm_fll); 
      nbr_flg++;
    } /* endif */

  (void)fprintf(stdout,"%s: INFO trv_tbl_prn_xtr() reports %d objects with extraction flag (flg_xtr) set\n",prg_nm_get(),nbr_flg); 
  
} /* end trv_tbl_prn_xtr() */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_trv_tbl_nm_fll  /* [fnc] Compare two trv_sct's by full name member */
(const void *val_1, /* I [sct] trv_sct to compare */
 const void *val_2) /* I [sct] trv_sct to compare */
{
  /* Purpose: Compare two trv_sct's by name structure member
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     Code based on responses to my comp.lang.c thread 20040101 */
  return strcmp((*(trv_sct const *)val_1).nm_fll,(*(trv_sct const *)val_2).nm_fll);
} /* end nco_cmp_trv_tbl_nm() */

void 
trv_tbl_srt /* [fnc] Sort traversal table */
(trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  /* Purpose: Alphabetize list by object full name
     This produces easy-to-search variable name screen output with ncks */
  qsort(trv_tbl->lst,(size_t)trv_tbl->nbr,sizeof(trv_sct),nco_cmp_trv_tbl_nm_fll);
} /* end trv_tbl_srt() */
