/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.c,v 1.138 2013-04-05 21:19:29 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Testing:
   ncks -D 1 ~/nco/data/in_grp.nc */

#include "nco_grp_trv.h" /* Group traversal */
#include "nco_lmt.h" /* Hyperslab limits */
#include "nco_cnf_dmn.h" /* Conform dimensions */

void                          
trv_tbl_init
(trv_tbl_sct **tbl) /* I/O [sct] Traversal table */
{
  trv_tbl_sct *tb=(trv_tbl_sct *)nco_malloc(sizeof(trv_tbl_sct));

  /* Object (group/variable) list */

  tb->nbr=0;
  tb->lst=NULL; 

  /* Dimension list */

  tb->nbr_dmn=0;
  tb->lst_dmn=NULL;

  *tbl=tb;
} /* trv_tbl_init() */

 
void 
trv_tbl_free
(trv_tbl_sct *tbl) /* I [sct] Traversal table */
{
  unsigned int idx;

  /* Object (group/variable) list */

  for(idx=0;idx<tbl->nbr;idx++){
    tbl->lst[idx].nm_fll=(char *)nco_free(tbl->lst[idx].nm_fll);
    tbl->lst[idx].grp_nm_fll=(char *)nco_free(tbl->lst[idx].grp_nm_fll);

    if (tbl->lst[idx].nco_typ == nco_obj_typ_var){
      int nbr_dmn=tbl->lst[idx].nbr_dmn;
      for(int dmn_idx=0;dmn_idx<nbr_dmn;dmn_idx++){
        tbl->lst[idx].var_dmn[dmn_idx].dmn_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].dmn_nm_fll);
        tbl->lst[idx].var_dmn[dmn_idx].dmn_nm=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].dmn_nm);
        tbl->lst[idx].var_dmn[dmn_idx].grp_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].grp_nm_fll);
      }
    }
  } 

  tbl->lst=(trv_sct *)nco_free(tbl->lst);

  /* Dimension list */

  for(unsigned int dmn_idx=0;dmn_idx<tbl->nbr_dmn;dmn_idx++){
    tbl->lst_dmn[dmn_idx].nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].nm_fll);
    tbl->lst_dmn[dmn_idx].grp_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].grp_nm_fll);
    tbl->lst_dmn[dmn_idx].lmt_msa.dmn_nm=(char *)nco_free(tbl->lst_dmn[dmn_idx].lmt_msa.dmn_nm);

    for(int lmt_idx=0;lmt_idx<tbl->lst_dmn[dmn_idx].lmt_msa.lmt_dmn_nbr;lmt_idx++){
      tbl->lst_dmn[dmn_idx].lmt_msa.lmt_dmn[lmt_idx]=nco_lmt_free(tbl->lst_dmn[dmn_idx].lmt_msa.lmt_dmn[lmt_idx]);
    }

    for(int crd_idx=0;crd_idx<tbl->lst_dmn[dmn_idx].crd_nbr;crd_idx++){
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->crd_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->crd_nm_fll);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_nm_fll);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->crd_grp_nm_fll);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_grp_nm_fll);

      for(int lmt_idx=0;lmt_idx<tbl->lst_dmn[dmn_idx].crd[crd_idx]->lmt_msa.lmt_dmn_nbr;lmt_idx++){
        tbl->lst_dmn[dmn_idx].crd[crd_idx]->lmt_msa.lmt_dmn[lmt_idx]=nco_lmt_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->lmt_msa.lmt_dmn[lmt_idx]);
      }  
    } 
  } 

  tbl->lst_dmn=(dmn_trv_sct *)nco_free(tbl->lst_dmn);

  tbl=(trv_tbl_sct *)nco_free(tbl);
} /* end trv_tbl_free() */



void                       
trv_tbl_inq                          /* [fnc] Find and return global totals of dimensions, variables, attributes */
(int * const att_glb_all,            /* O [nbr] Number of global attributes in file */
 int * const att_grp_all,            /* O [nbr] Number of group attributes in file */
 int * const att_var_all,            /* O [nbr] Number of variable attributes in file */
 int * const dmn_nbr_all,            /* O [nbr] Number of dimensions in file */
 int * const dmn_rec_all,            /* O [nbr] Number of record dimensions in file */
 int * const grp_dpt_all,            /* O [nbr] Maximum group depth (root = 0) */
 int * const grp_nbr_all,            /* O [nbr] Number of groups in file */
 int * const var_ntm_all,            /* O [nbr] Number of non-atomic variables in file */
 int * const var_tmc_all,            /* O [nbr] Number of atomic-type variables in file */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  /* [fnc] Find and return global file summaries like # of dimensions, variables, attributes */

  int att_glb_lcl; /* [nbr] Number of global attributes in file */
  int att_grp_lcl; /* [nbr] Number of group attributes in file */
  int att_var_lcl; /* [nbr] Number of variable attributes in file */
  int dmn_rec_lcl; /* [nbr] Number of record dimensions in file */
  int grp_dpt_lcl; /* [nbr] Maximum group depth (root = 0) */
  int grp_nbr_lcl; /* [nbr] Number of groups in file */
  int var_ntm_lcl; /* [nbr] Number of non-atomic variables in file */
  int var_tmc_lcl; /* [nbr] Number of atomic-type variables in file */
 
  /* Initialize */
  att_glb_lcl=0;
  att_grp_lcl=0;
  att_var_lcl=0;
  dmn_rec_lcl=0;
  grp_dpt_lcl=0;
  grp_nbr_lcl=0;
  var_ntm_lcl=0;
  var_tmc_lcl=0;

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx]; 
    if(trv.nco_typ == nco_obj_typ_var) att_var_lcl+=trv.nbr_att;
    if(trv.nco_typ == nco_obj_typ_nonatomic_var) var_ntm_lcl++;
    if(trv.nco_typ == nco_obj_typ_grp){ 
      grp_nbr_lcl+=trv.nbr_grp;
      var_tmc_lcl+=trv.nbr_var;
      if(grp_dpt_lcl < trv.grp_dpt) grp_dpt_lcl=trv.grp_dpt;
      if(!strcmp(trv.nm_fll,"/")) att_glb_lcl=trv.nbr_att; else att_grp_lcl+=trv.nbr_att; 
    } /* end nco_obj_typ_grp */
  } /* end uidx */

  for(unsigned uidx=0;uidx<trv_tbl->nbr_dmn;uidx++)
    if(trv_tbl->lst_dmn[uidx].is_rec_dmn) dmn_rec_lcl++;

  if(att_glb_all) *att_glb_all=att_glb_lcl;
  if(att_grp_all) *att_grp_all=att_grp_lcl;
  if(att_var_all) *att_var_all=att_var_lcl;
  if(dmn_nbr_all) *dmn_nbr_all=trv_tbl->nbr_dmn;
  if(dmn_rec_all) *dmn_rec_all=dmn_rec_lcl;
  if(grp_dpt_all) *grp_dpt_all=grp_dpt_lcl;
  if(grp_nbr_all) *grp_nbr_all=grp_nbr_lcl;
  if(var_ntm_all) *var_ntm_all=var_ntm_lcl;
  if(var_tmc_all) *var_tmc_all=var_tmc_lcl;

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
    if((trv_obj.nco_typ == obj_typ) && trv_obj.flg_mch) (void)fprintf(stdout,"nm_fll=%s\n",trv_obj.nm_fll);
  } /* end loop over trv_tbl */
}/* end trv_tbl_prn_flg_mch() */

void                          
trv_tbl_prn                          /* [fnc] Print table with -z */
(const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */  
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if(trv.nco_typ == nco_obj_typ_grp) (void)fprintf(stdout,"grp: "); else (void)fprintf(stdout,"var: ");
    (void)fprintf(stdout,"%s\n",trv_tbl->lst[uidx].nm_fll); 
  } /* end uidx */
} /* end trv_tbl_prn() */

nco_bool                              /* O [nbr] Item found or not */
trv_tbl_fnd_var_nm_fll                /* [fnc] Check if "var_nm_fll" is in table */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++)
    if(trv_tbl->lst[uidx].nco_typ == nco_obj_typ_var && !strcmp(var_nm_fll,trv_tbl->lst[uidx].nm_fll))
      return True;

  return False;
} /* end trv_tbl_fnd_var_nm_fll() */

trv_sct *                               /* O [sct] Table object */
trv_tbl_var_nm_fll                    /* [fnc] Check if "var_nm_fll" is in table */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++)
    if(trv_tbl->lst[uidx].nco_typ == nco_obj_typ_var && !strcmp(var_nm_fll,trv_tbl->lst[uidx].nm_fll))
      return &trv_tbl->lst[uidx];

  return NULL;

} /* trv_tbl_var_nm_fll() */


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
(const trv_tbl_sct * const trv_tbl,  /* I [sct] Traversal table */
 const char * const fnc_nm)          /* I [sng] Function name of the calling function */
{
  int idx=0;
  int nbr_flg=0;

  /* Loop table */
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if(trv.flg_xtr) nbr_flg++;
  } /* Loop table */

  (void)fprintf(stdout,"%s: INFO %s reports <%d> objects with extraction flag (flg_xtr) set:\n",prg_nm_get(),fnc_nm,nbr_flg); 
  
  /* Loop table */
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if(trv.flg_xtr) (void)fprintf(stdout,"[%d] %s\n",idx++,trv.nm_fll); 
  }/* Loop table */

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

void                          
trv_tbl_mch                            /* [fnc] Match 2 tables (find common objects) and process common objects  */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 int cnk_map,                          /* I [enm] Chunking map */
 int cnk_plc,                          /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 const nco_bool CNV_CCM_CCSM_CF,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl), /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_def)               /* I [flg] Action type (True for define variables, False when write variables ) */
{
  /* Purpose: Find common objects; the algorithm used for this search is the
  *  cosequential match algorithm and is described in
  *  Folk, Michael; Zoellick, Bill. (1992). File Structures. Addison-Wesley.
  *
  * Compare 2 ordered lists of names:
  *  if Name(1) is less than Name(2), read the next name from List 1; this is done by incrementing a current index
  *  if Name(1) is greater than Name(2), read the next name from List 2
  *  if the names are the same, read the next names from the two lists 
  * 
  * Used in ncbo; ncbo performs binary operations on variables in file 1 and the corresponding variables
  * (those with the same name) in file 2 and stores the results in file 3.
  * This function detects common absolute names in tables 1 and 2 and does the ncbo binary operation on it
  *
  */

  const char fnc_nm[]="trv_tbl_mch()"; /* [sng] Function name */

  typedef struct{		
    char *var_nm_fll;            /* [sng] Full path of variable */
    nco_bool flg_in_fl[2];       /* [flg] Is this name in each file?; files are [0] and [1] */
  } nco_cmn_t;

  int idx_lst;                   /* [idx] Current position in common List */ 
  int idx_tbl_1;                 /* [idx] Current position in List 1 */ 
  int idx_tbl_2;                 /* [idx] Current position in List 2 */ 
  int nbr_tbl_1;                 /* [nbr] Number of items in list 1 */
  int nbr_tbl_2;                 /* [nbr] Number of items in list 2 */
  int nco_cmp;                   /* [nbr] Return value of strcmp() */ 

  nco_bool flg_more_names_exist; /* [flg] Are there more names to process? */
  nco_bool flg_in_fl[2];         /* [flg] Is this name if each file?; files are [0] and [1] */

  nco_cmn_t *cmn_lst=NULL;       /* [sct] A list of common names */ 

  /* Tables *must* be sorted */
  (void)trv_tbl_srt(trv_tbl_1);
  (void)trv_tbl_srt(trv_tbl_2);

  if(dbg_lvl_get() >= nco_dbg_vrb && flg_def == True){
    (void)fprintf(stdout,"%s: INFO %s reports Sorted table 1\n",prg_nm_get(),fnc_nm);
    (void)trv_tbl_prn(trv_tbl_1);
    (void)fprintf(stdout,"%s: INFO %s reports Sorted table 2\n",prg_nm_get(),fnc_nm);
    (void)trv_tbl_prn(trv_tbl_2);
    (void)fprintf(stdout,"%s: INFO %s reports Common objects\n",prg_nm_get(),fnc_nm);
  }

  /* Get number of objects in each table */
  nbr_tbl_1=trv_tbl_1->nbr;
  nbr_tbl_2=trv_tbl_2->nbr;

  /* If both lists have names, then there are names to process */
  flg_more_names_exist = (nbr_tbl_1>0 && nbr_tbl_2>0) ? 1 : 0;

  /* Put counters at start */
  idx_tbl_1=0;
  idx_tbl_2=0;
  idx_lst=0;

  /* Store list of common objects */
  cmn_lst=(nco_cmn_t *)nco_malloc((nbr_tbl_1+nbr_tbl_2)*sizeof(nco_cmn_t));

  /* Copy global attributes in define mode */
  if(flg_def) (void)nco_att_cpy(nc_id_1,nc_out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);

  /* Iterate the 2 lists */
  while (flg_more_names_exist)
  {
    trv_sct trv_1=trv_tbl_1->lst[idx_tbl_1];
    trv_sct trv_2=trv_tbl_2->lst[idx_tbl_2];

    /* Criteria is string compare */
    nco_cmp = strcmp(trv_1.nm_fll,trv_2.nm_fll);

    /* Names match: store flag, define or write in output file, then read next names from lists */
    if(nco_cmp == 0)
    {
      flg_in_fl[0]=True; 
      flg_in_fl[1]=True;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_1.nm_fll);
      idx_lst++;

      idx_tbl_1++;
      idx_tbl_2++;

    }else if(nco_cmp < 0){
      /* Name(1) is less than Name(2), read the next name from List 1  */

      flg_in_fl[0]=True; 
      flg_in_fl[1]=False;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_1.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() >= nco_dbg_dev)(void)fprintf(stdout,"%s: INFO %s reports tbl_1[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_1,trv_1.nm_fll);

      idx_tbl_1++;
    }else{
      /* Name(1) is greater than Name(2), read the next name from List 2 */
      flg_in_fl[0]=False; 
      flg_in_fl[1]=True;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_2.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() >= nco_dbg_dev)(void)fprintf(stdout,"%s: INFO %s reports tbl_2[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_2,trv_2.nm_fll);

      idx_tbl_2++;
    } /* end nco_cmp */

    flg_more_names_exist = (idx_tbl_1<nbr_tbl_1 && idx_tbl_2<nbr_tbl_2) ? 1 : 0;

  } /* end while */

  /* List1 did not end */

  if(idx_tbl_1<nbr_tbl_1){

    while(idx_tbl_1<nbr_tbl_1){
      trv_sct trv_1=trv_tbl_1->lst[idx_tbl_1];

      flg_in_fl[0]=True; 
      flg_in_fl[1]=False;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_1.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() >= nco_dbg_dev)(void)fprintf(stdout,"%s: INFO %s reports tbl_1[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_1,trv_1.nm_fll);

      idx_tbl_1++;
    } /* end while */
  } /* end if */

  /* List2 did not end */
  if(idx_tbl_2<nbr_tbl_2){
    while(idx_tbl_2<nbr_tbl_2){
      trv_sct trv_2=trv_tbl_2->lst[idx_tbl_2];

      flg_in_fl[0]=False; 
      flg_in_fl[1]=True;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_2.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() >= nco_dbg_dev)(void)fprintf(stdout,"%s: INFO %s reports tbl_2[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_2,trv_2.nm_fll);

      idx_tbl_2++;
    } /* end while */
  } /* end if */

  /* Print list */
  if(dbg_lvl_get() >= nco_dbg_var && flg_def == True){
    (void)fprintf(stdout,"%s: INFO %s reports Common objects\n",prg_nm_get(),fnc_nm);
    (void)fprintf(stdout,"file1     file2\n");
    (void)fprintf(stdout,"---------------------------------------\n");
    for(int idx=0;idx<idx_lst;idx++){
      char c1, c2;
      c1 = (cmn_lst[idx].flg_in_fl[0]) ? 'x' : ' ';
      c2 = (cmn_lst[idx].flg_in_fl[1]) ? 'x' : ' ';
      (void)fprintf(stdout,"%5c %6c    %-15s\n", c1, c2, cmn_lst[idx].var_nm_fll);
    } /* end loop over idx */
    (void)fprintf(stdout,"\n");
  } /* endif dbg */


  /* Process objects in list */
  for(int idx=0;idx<idx_lst;idx++){

    trv_sct *trv_1=trv_tbl_var_nm_fll(cmn_lst[idx].var_nm_fll,trv_tbl_1);
    trv_sct *trv_2=trv_tbl_var_nm_fll(cmn_lst[idx].var_nm_fll,trv_tbl_2);

    /* Both objects exist in the 2 files, both objects are to extract */
    if (trv_1 && trv_2 && cmn_lst[idx].flg_in_fl[0] == True && cmn_lst[idx].flg_in_fl[1] == True && trv_1->flg_xtr && trv_2->flg_xtr){

      if(dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: INFO %s reports common element to output:%s\n",prg_nm_get(),fnc_nm,trv_1->nm_fll); 
      }

      /* Process common object */
      (void)trv_tbl_prc(nc_id_1,nc_id_2,nc_out_id,cnk_map,cnk_plc,cnk_sz_scl,cnk,cnk_nbr,dfl_lvl,gpe,CNV_CCM_CCSM_CF,(nco_bool)False,(dmn_sct **)NULL,(int)0,nco_op_typ,trv_1,trv_2,trv_tbl_1,trv_tbl_2,flg_def);

    } /* Both objects exist in the 2 files, both objects are to extract */

    /* Object exists only in file 1 and is to extract */
    else if (trv_1 && cmn_lst[idx].flg_in_fl[0] == True && cmn_lst[idx].flg_in_fl[1] == False && trv_1->flg_xtr){

      if(dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: INFO %s reports element in file 1 to output:%s\n",prg_nm_get(),fnc_nm,trv_1->nm_fll); 
      }

      /* Copy processint type fixed object from file 1 */
      (void)trv_tbl_fix(nc_id_1,nc_out_id,cnk_map,cnk_plc,cnk_sz_scl,cnk,cnk_nbr,dfl_lvl,gpe,CNV_CCM_CCSM_CF,(nco_bool)False,(dmn_sct **)NULL,(int)0,trv_1,trv_tbl_1,flg_def);

    }/* Object exists only in file 1 and is to extract */

    /* Object exists only in file 2 and is to extract */
    else if (trv_2 && cmn_lst[idx].flg_in_fl[0] == False && cmn_lst[idx].flg_in_fl[1] == True && trv_2->flg_xtr){

      if(dbg_lvl_get() >= nco_dbg_dev){
        (void)fprintf(stdout,"%s: INFO %s reports element in file 2 to output:%s\n",prg_nm_get(),fnc_nm,trv_2->nm_fll); 
      }

      /* Copy processint type fixed object from file 2 */
      (void)trv_tbl_fix(nc_id_2,nc_out_id,cnk_map,cnk_plc,cnk_sz_scl,cnk,cnk_nbr,dfl_lvl,gpe,CNV_CCM_CCSM_CF,(nco_bool)False,(dmn_sct **)NULL,(int)0,trv_2,trv_tbl_2,flg_def);

    } /* Object exists only in file 2 and is to extract */    
  } /* Process objects in list */

  for(int idx=0;idx<idx_lst;idx++) cmn_lst[idx].var_nm_fll=(char *)nco_free(cmn_lst[idx].var_nm_fll);
  cmn_lst=(nco_cmn_t *)nco_free(cmn_lst);

} /* trv_tbl_mch() */


void                          
trv_tbl_prc                            /* [fnc] Process objects  */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_id_2,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 int cnk_map,                          /* I [enm] Chunking map */
 int cnk_plc,                          /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 const nco_bool CNV_CCM_CCSM_CF,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_sct *trv_1,                       /* I [sct] Table object */
 trv_sct *trv_2,                       /* I [sct] Table object */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_def)               /* I [flg] Action type (True for define variables, False when write variables ) */
{
  const char fnc_nm[]="trv_tbl_prc()"; /* [sng] Function name */
  const char sls_sng[]="/";      /* [sng] Slash string */

  char *grp_out_fll;             /* [sng] Group name */

  gpe_nm_sct *gpe_nm;            /* [sct] GPE name duplicate check array */

  int fl_fmt;                    /* [enm] netCDF file format */
  int grp_id_1;                  /* [id] Group ID in input file */
  int grp_id_2;                  /* [id] Group ID in input file */
  int grp_out_id;                /* [id] Group ID in output file */ 
  int nbr_gpe_nm;                /* [nbr] Number of GPE entries */  
  int prg_id;                    /* [enm] Program ID */
  int var_id_1;                  /* [id] Variable ID in input file */
  int var_id_2;                  /* [id] Variable ID in input file */
  int var_out_id;                /* [id] Variable ID in output file */

  var_sct *var_prc_1;            /* [sct] Variable to process in file 1 */
  var_sct *var_prc_2;            /* [sct] Variable to process in file 2 */
  var_sct *var_prc_out;          /* [sct] Variable to process in output */
  var_sct *var_prc_gtr;          /* [sct] Greater rank variable to process */
  var_sct *var_prc_lsr;          /* [sct] Lesser  rank variable to process */

  nco_bool RNK_1_GTR;            /* [flg] Rank of variable in file 1 variable greater than or equal to file 2 */

  prc_typ_enm prc_typ_1;         /* [enm] Processing type */
  prc_typ_enm prc_typ_2;         /* [enm] Processing type */

  trv_sct *rnk_gtr;              /* [sct] Object of greater or equal rank */

  assert(trv_1->nco_typ == nco_obj_typ_var);
  assert(trv_1->flg_xtr == True);

  assert(trv_2->nco_typ == nco_obj_typ_var);
  assert(trv_2->flg_xtr == True);

  /* Get Program ID */
  prg_id=prg_get(); 

  /* Get output file format */
  nbr_gpe_nm=0;
  (void)nco_inq_format(nc_out_id,&fl_fmt);

  /* Edit group name for output */
  if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv_1->grp_nm_fll); else grp_out_fll=(char *)strdup(trv_1->grp_nm_fll);

  /* Obtain group ID using full group name */
  (void)nco_inq_grp_full_ncid(nc_id_1,trv_1->grp_nm_fll,&grp_id_1);
  (void)nco_inq_grp_full_ncid(nc_id_2,trv_2->grp_nm_fll,&grp_id_2);

  /* Get variable ID */
  (void)nco_inq_varid(grp_id_1,trv_1->nm,&var_id_1);
  (void)nco_inq_varid(grp_id_2,trv_2->nm,&var_id_2);

  /* Allocate variable structure and fill with metadata */
  var_prc_1=nco_var_fll_trv(grp_id_1,var_id_1,trv_1,trv_tbl_1);     
  var_prc_2=nco_var_fll_trv(grp_id_2,var_id_2,trv_2,trv_tbl_2);

  if(var_prc_1->nbr_dim >= var_prc_2->nbr_dim) RNK_1_GTR=True; else RNK_1_GTR=False;
  rnk_gtr = (RNK_1_GTR) ? trv_1 : trv_2;
  var_prc_gtr= (RNK_1_GTR) ? var_prc_1 : var_prc_2;
  var_prc_lsr= (RNK_1_GTR) ? var_prc_2 : var_prc_1;
  var_prc_out= (RNK_1_GTR) ? nco_var_dpl(var_prc_1) : nco_var_dpl(var_prc_2);

  /* Get processing type */
  (void)nco_var_lst_dvd_trv(var_prc_1,var_prc_out,CNV_CCM_CCSM_CF,FIX_REC_CRD,cnk_map,cnk_plc,dmn_xcl,nbr_dmn_xcl,&prc_typ_1); 
  (void)nco_var_lst_dvd_trv(var_prc_2,var_prc_out,CNV_CCM_CCSM_CF,FIX_REC_CRD,cnk_map,cnk_plc,dmn_xcl,nbr_dmn_xcl,&prc_typ_2); 

  /* Define mode */
  if(flg_def){  

    nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */

    const int nco_pck_plc=nco_pck_map_nil; /* I [enm] Packing policy */

    /* Copy all attributes except in cases where packing/unpacking is involved
    0. Variable is unpacked on input, unpacked on output
    --> Copy all attributes
    1. Variable is packed on input, is not altered, and remains packed on output
    --> Copy all attributes
    2. Variable is packed on input, is unpacked for some reason, and will be unpacked on output
    --> Copy all attributes except scale_factor and add_offset
    3. Variable is packed on input, is unpacked for some reason, and will be packed on output (possibly with new packing attributes)
    --> Copy all attributes, but scale_factor and add_offset must be overwritten later with new values
    4. Variable is not packed on input, packing is performed, and output is packed
    --> Copy all attributes, define dummy values for scale_factor and add_offset now, and write those values later, when they are known */

    /* Do not copy packing attributes "scale_factor" and "add_offset" 
    if variable is packed in input file and unpacked in output file 
    Arithmetic operators calling nco_var_dfn() with fixed variables should leave them fixed
    Currently ncap calls nco_var_dfn() only for fixed variables, so handle exception with ncap-specific condition */
    /* Copy exising packing attributes, if any, unless... */
    if(nco_is_rth_opr(prg_id) && /* ...operator is arithmetic... */
      prg_id != ncap && /* ...and is not ncap (hence it must be, e.g., ncra, ncbo)... */
      !var_prc_1->is_fix_var && /* ...and variable is processed (not fixed)... */
      var_prc_1->pck_dsk) /* ...and variable is packed in input file... */
      PCK_ATT_CPY=False;

    /* Do not copy packing attributes when unpacking variables 
    ncpdq is currently only operator that passes values other than nco_pck_plc_nil */
    if(nco_pck_plc == nco_pck_plc_upk) /* ...and variable will be _unpacked_ ... */
      PCK_ATT_CPY=False;        

    /* If output group does not exist, create it */
    if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

    /* Detect duplicate GPE names in advance, then exit with helpful error */
    if(gpe){
      char *gpe_var_nm_fll=NULL; 

      /* Construct absolute GPE variable path */
      gpe_var_nm_fll=(char*)nco_malloc(strlen(grp_out_fll)+strlen(trv_1->nm)+2L);
      strcpy(gpe_var_nm_fll,grp_out_fll);
      /* If not root group, concatenate separator */
      if(strcmp(grp_out_fll,sls_sng)) strcat(gpe_var_nm_fll,sls_sng);
      strcat(gpe_var_nm_fll,trv_1->nm);

      /* GPE name is not already on list, put it there */
      if(nbr_gpe_nm == 0){
        gpe_nm=(gpe_nm_sct *)nco_malloc((nbr_gpe_nm+1)*sizeof(gpe_nm_sct)); 
        gpe_nm[nbr_gpe_nm].var_nm_fll=strdup(gpe_var_nm_fll);
        nbr_gpe_nm++;
      }else{
        /* Put GPE on list only if not already there */
        for(int idx_gpe=0;idx_gpe<nbr_gpe_nm;idx_gpe++){
          if(!strcmp(gpe_var_nm_fll,gpe_nm[idx_gpe].var_nm_fll)){
            (void)fprintf(stdout,"%s: ERROR %s reports variable %s already defined. HINT: Removing groups to flatten files can lead to over-determined situations where a single object name (e.g., a variable name) must refer to multiple objects in the same output group. The user's intent is ambiguous so instead of arbitrarily picking which (e.g., the last) variable of that name to place in the output file, NCO simply fails. User should re-try command after ensuring multiple objects of the same name will not be placed in the same group.\n",prg_nm_get(),fnc_nm,gpe_var_nm_fll);
            for(int idx=0;idx<nbr_gpe_nm;idx++) gpe_nm[idx].var_nm_fll=(char *)nco_free(gpe_nm[idx].var_nm_fll);
            nco_exit(EXIT_FAILURE);
          } /* strcmp() */
        } /* end loop over gpe_nm */
        gpe_nm=(gpe_nm_sct *)nco_realloc((void *)gpe_nm,(nbr_gpe_nm+1)*sizeof(gpe_nm_sct));
        gpe_nm[nbr_gpe_nm].var_nm_fll=strdup(gpe_var_nm_fll);
        nbr_gpe_nm++;
      } /* nbr_gpe_nm */

      /* Free full path name */
      if(gpe_var_nm_fll) gpe_var_nm_fll=(char *)nco_free(gpe_var_nm_fll);
    } /* !GPE */

    /* Define variable in output file. NB: Use file/variable of greater rank as template */
    var_out_id= (RNK_1_GTR) ? nco_cpy_var_dfn(nc_id_1,nc_out_id,grp_id_1,grp_out_id,dfl_lvl,gpe,(char *)NULL,trv_1,trv_tbl_1) : nco_cpy_var_dfn(nc_id_2,nc_out_id,grp_id_2,grp_out_id,dfl_lvl,gpe,(char *)NULL,trv_2,trv_tbl_2);

    /* Set chunksize parameters */
    if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set_trv(grp_out_id,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr,rnk_gtr);

    /* Copy variable's attributes */
    if(RNK_1_GTR) (void)nco_att_cpy(grp_id_1,grp_out_id,var_id_1,var_out_id,PCK_ATT_CPY); else (void)nco_att_cpy(grp_id_2,grp_out_id,var_id_2,var_out_id,PCK_ATT_CPY);

  }else{ /* Write mode */

    int dmn_idx_gtr;
    int dmn_idx_lsr;
    int has_mss_val;      /* [flg] Variable has missing value */

    ptr_unn mss_val;      /* [sct] Missing value */

    /* Get group ID */
    (void)nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_out_id);

    /* Get variable ID */
    (void)nco_inq_varid(grp_out_id,trv_1->nm,&var_out_id);         

    if(dbg_lvl_get() >= nco_dbg_vrb) (void)fprintf(stdout,"%s: INFO %s reports operation type <%d> for <%s>\n",prg_nm_get(),fnc_nm,prc_typ_1,trv_1->nm_fll);

    /* Non-processed variable */
    if(prc_typ_1 == fix_typ || prc_typ_2 == fix_typ){
      if(RNK_1_GTR)(void)nco_cpy_var_val_mlt_lmt_trv(grp_id_1,grp_out_id,(FILE *)NULL,(nco_bool)False,trv_1); else (void)nco_cpy_var_val_mlt_lmt_trv(grp_id_2,grp_out_id,(FILE *)NULL,(nco_bool)False,trv_2);
    } /* endif fix */

    /* Processed variable */
    if(prc_typ_1 == prc_typ && prc_typ_2 == prc_typ){

      var_prc_out->id=var_out_id;

      /* fxm: gtr or lsr? */
      var_prc_out->srt=var_prc_gtr->srt;
      var_prc_out->cnt=var_prc_gtr->cnt;

      /* Find and set variable dmn_nbr, ID, mss_val, type in first file */
      (void)nco_var_mtd_refresh(grp_id_1,var_prc_1);

      /* Set missing value */
      has_mss_val=var_prc_gtr->has_mss_val;

      /* Read hyperslab from first file */
      (void)nco_msa_var_get_trv(grp_id_1,var_prc_1,trv_1);

      /* Find and set variable dmn_nbr, ID, mss_val, type in second file */
      (void)nco_var_mtd_refresh(grp_id_2,var_prc_2);

      /* Read hyperslab from second file */
      (void)nco_msa_var_get_trv(grp_id_2,var_prc_2,trv_2);

      /* Check that all dims in var_prc_lsr are in var_prc_gtr */
      for(dmn_idx_lsr=0;dmn_idx_lsr<var_prc_lsr->nbr_dim;dmn_idx_lsr++){
        for(dmn_idx_gtr=0;dmn_idx_gtr<var_prc_gtr->nbr_dim;dmn_idx_gtr++)  
          if(!strcmp(var_prc_lsr->dim[dmn_idx_lsr]->nm,var_prc_gtr->dim[dmn_idx_gtr]->nm)){
            break;
          }
          if(dmn_idx_gtr == var_prc_gtr->nbr_dim){
            (void)fprintf(stdout,"%s: ERROR Variables do not conform: variable %s has dimension %s not present in variable %s\n",prg_nm_get(),var_prc_lsr->nm,var_prc_lsr->dim[dmn_idx_lsr]->nm,var_prc_gtr->nm);
            nco_exit(EXIT_FAILURE);
          } /* endif error */
      } /* end loop over idx */

      /* Make sure variables conform in type */
      if(var_prc_1->type != var_prc_2->type){
        if(dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Input variables do not conform in type: file 1 variable %s has type %s, file 2 variable %s has type %s, output variable %s will have type %s\n",prg_nm_get(),var_prc_1->nm,nco_typ_sng(var_prc_1->type),var_prc_2->nm,nco_typ_sng(var_prc_2->type),var_prc_gtr->nm,nco_typ_sng(var_prc_gtr->type));
      }  

      /* Broadcast lesser to greater variable. NB: Pointers may change so _gtr, _lsr not valid */
      if(var_prc_1->nbr_dim != var_prc_2->nbr_dim) (void)ncap_var_cnf_dmn(&var_prc_1,&var_prc_2);

      if(RNK_1_GTR) var_prc_2=nco_var_cnf_typ(var_prc_1->type,var_prc_2); else var_prc_1=nco_var_cnf_typ(var_prc_2->type,var_prc_1);

      /* var1 and var2 now conform in size and type to eachother and are in memory */

      /* Change missing_value, if any, of lesser rank to missing_value, if any, of greater rank */
      if(RNK_1_GTR) has_mss_val=nco_mss_val_cnf(var_prc_1,var_prc_2); else has_mss_val=nco_mss_val_cnf(var_prc_2,var_prc_1);

      /* mss_val of larger rank, if any, overrides mss_val of smaller rank */
      if(has_mss_val) mss_val= (RNK_1_GTR) ? var_prc_1->mss_val : var_prc_2->mss_val;

      /* Perform specified binary operation */
      switch(nco_op_typ){
      case nco_op_add: /* [enm] Add file_1 to file_2 */
        (void)nco_var_add(var_prc_1->type,var_prc_1->sz,has_mss_val,mss_val,var_prc_2->val,var_prc_1->val); break;
      case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
        (void)nco_var_mlt(var_prc_1->type,var_prc_1->sz,has_mss_val,mss_val,var_prc_2->val,var_prc_1->val); break;
      case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
        (void)nco_var_dvd(var_prc_1->type,var_prc_1->sz,has_mss_val,mss_val,var_prc_2->val,var_prc_1->val); break;
      case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
        (void)nco_var_sbt(var_prc_1->type,var_prc_1->sz,has_mss_val,mss_val,var_prc_2->val,var_prc_1->val); break;
      default: /* Other defined nco_op_typ values are valid for ncra(), ncrcat(), ncwa(), not ncbo() */
        (void)fprintf(stdout,"%s: ERROR Illegal nco_op_typ in binary operation\n",prg_nm_get());
        nco_exit(EXIT_FAILURE);
        break;
      } /* end case */

      /* Copy result to output file and free workspace buffer. NB. use grp_out_id */
      if(var_prc_1->nbr_dim == 0){
        (void)nco_put_var1(grp_out_id,var_prc_out->id,var_prc_out->srt,var_prc_1->val.vp,var_prc_1->type);
      }else{ /* end if variable is scalar */
        (void)nco_put_vara(grp_out_id,var_prc_out->id,var_prc_out->srt,var_prc_out->cnt,var_prc_1->val.vp,var_prc_1->type);
      } /* end else */

    } /* Processed variable */
  } /* Write mode */

  /* Free allocated variable structures */
  var_prc_1->val.vp=nco_free(var_prc_1->val.vp);
  var_prc_2->val.vp=nco_free(var_prc_2->val.vp);
  var_prc_1=(var_sct *)nco_free(var_prc_1);
  var_prc_2=(var_sct *)nco_free(var_prc_2);
  var_prc_out=(var_sct *)nco_free(var_prc_out);

  /* Free output path name */
  grp_out_fll=(char *)nco_free(grp_out_fll);


} /* trv_tbl_prc() */

void                          
trv_tbl_fix                            /* [fnc] Copy processing type fixed object  */
(const int nc_id_1,                    /* I [id] netCDF input-file ID */
 const int nc_out_id,                  /* I [id] netCDF output-file ID */
 int cnk_map,                          /* I [enm] Chunking map */
 int cnk_plc,                          /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 const gpe_sct * const gpe,            /* I [sct] GPE structure */
 const nco_bool CNV_CCM_CCSM_CF,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 trv_sct *trv_1,                       /* I [sct] Table object */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 const nco_bool flg_def)               /* I [flg] Action type (True for define variables, False when write variables ) */
{
  const char fnc_nm[]="trv_tbl_fix()"; /* [sng] Function name */
  const char sls_sng[]="/";      /* [sng] Slash string */

  char *grp_out_fll;             /* [sng] Group name */

  gpe_nm_sct *gpe_nm;            /* [sct] GPE name duplicate check array */

  int fl_fmt;                    /* [enm] netCDF file format */
  int grp_id_1;                  /* [id] Group ID in input file */
  int grp_out_id;                /* [id] Group ID in output file */ 
  int nbr_gpe_nm;                /* [nbr] Number of GPE entries */  
  int prg_id;                    /* [enm] Program ID */
  int var_id_1;                  /* [id] Variable ID in input file */
  int var_out_id;                /* [id] Variable ID in output file */

  var_sct *var_prc_1;            /* [sct] Variable to process in file 1 */
  var_sct *var_prc_out;          /* [sct] Variable to process in output */

  prc_typ_enm prc_typ_1; /* [enm] Processing type */

  assert(trv_1->nco_typ == nco_obj_typ_var);
  assert(trv_1->flg_xtr == True);

  /* Get Program ID */
  prg_id=prg_get(); 

  /* Get output file format */
  nbr_gpe_nm=0;
  (void)nco_inq_format(nc_out_id,&fl_fmt);

  /* Edit group name for output */
  if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv_1->grp_nm_fll); else grp_out_fll=(char *)strdup(trv_1->grp_nm_fll);

  /* Obtain group ID using full group name */
  (void)nco_inq_grp_full_ncid(nc_id_1,trv_1->grp_nm_fll,&grp_id_1);

  /* Get variable ID */
  (void)nco_inq_varid(grp_id_1,trv_1->nm,&var_id_1);

  /* Allocate variable structure and fill with metadata */
  var_prc_1=nco_var_fll_trv(grp_id_1,var_id_1,trv_1,trv_tbl_1);     

  var_prc_out= nco_var_dpl(var_prc_1);
  (void)nco_var_lst_dvd_trv(var_prc_1,var_prc_out,CNV_CCM_CCSM_CF,FIX_REC_CRD,cnk_map,cnk_plc,dmn_xcl,nbr_dmn_xcl,&prc_typ_1); 

  if(prc_typ_1 != fix_typ){
    var_prc_1->val.vp=nco_free(var_prc_1->val.vp);
    var_prc_1=(var_sct *)nco_free(var_prc_1);
    var_prc_out=(var_sct *)nco_free(var_prc_out);
    grp_out_fll=(char *)nco_free(grp_out_fll);
    return;
  }

  /* Define mode */
  if(flg_def){  

    nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */

    const int nco_pck_plc=nco_pck_map_nil; /* I [enm] Packing policy */

    /* Copy all attributes except in cases where packing/unpacking is involved
    0. Variable is unpacked on input, unpacked on output
    --> Copy all attributes
    1. Variable is packed on input, is not altered, and remains packed on output
    --> Copy all attributes
    2. Variable is packed on input, is unpacked for some reason, and will be unpacked on output
    --> Copy all attributes except scale_factor and add_offset
    3. Variable is packed on input, is unpacked for some reason, and will be packed on output (possibly with new packing attributes)
    --> Copy all attributes, but scale_factor and add_offset must be overwritten later with new values
    4. Variable is not packed on input, packing is performed, and output is packed
    --> Copy all attributes, define dummy values for scale_factor and add_offset now, and write those values later, when they are known */

    /* Do not copy packing attributes "scale_factor" and "add_offset" 
    if variable is packed in input file and unpacked in output file 
    Arithmetic operators calling nco_var_dfn() with fixed variables should leave them fixed
    Currently ncap calls nco_var_dfn() only for fixed variables, so handle exception with ncap-specific condition */
    /* Copy exising packing attributes, if any, unless... */
    if(nco_is_rth_opr(prg_id) && /* ...operator is arithmetic... */
      prg_id != ncap && /* ...and is not ncap (hence it must be, e.g., ncra, ncbo)... */
      !var_prc_1->is_fix_var && /* ...and variable is processed (not fixed)... */
      var_prc_1->pck_dsk) /* ...and variable is packed in input file... */
      PCK_ATT_CPY=False;

    /* Do not copy packing attributes when unpacking variables 
    ncpdq is currently only operator that passes values other than nco_pck_plc_nil */
    if(nco_pck_plc == nco_pck_plc_upk) /* ...and variable will be _unpacked_ ... */
      PCK_ATT_CPY=False;        

    /* If output group does not exist, create it */
    if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

    /* Detect duplicate GPE names in advance, then exit with helpful error */
    if(gpe){
      char *gpe_var_nm_fll=NULL; 

      /* Construct absolute GPE variable path */
      gpe_var_nm_fll=(char*)nco_malloc(strlen(grp_out_fll)+strlen(trv_1->nm)+2L);
      strcpy(gpe_var_nm_fll,grp_out_fll);
      /* If not root group, concatenate separator */
      if(strcmp(grp_out_fll,sls_sng)) strcat(gpe_var_nm_fll,sls_sng);
      strcat(gpe_var_nm_fll,trv_1->nm);

      /* GPE name is not already on list, put it there */
      if(nbr_gpe_nm == 0){
        gpe_nm=(gpe_nm_sct *)nco_malloc((nbr_gpe_nm+1)*sizeof(gpe_nm_sct)); 
        gpe_nm[nbr_gpe_nm].var_nm_fll=strdup(gpe_var_nm_fll);
        nbr_gpe_nm++;
      }else{
        /* Put GPE on list only if not already there */
        for(int idx_gpe=0;idx_gpe<nbr_gpe_nm;idx_gpe++){
          if(!strcmp(gpe_var_nm_fll,gpe_nm[idx_gpe].var_nm_fll)){
            (void)fprintf(stdout,"%s: ERROR %s reports variable %s already defined. HINT: Removing groups to flatten files can lead to over-determined situations where a single object name (e.g., a variable name) must refer to multiple objects in the same output group. The user's intent is ambiguous so instead of arbitrarily picking which (e.g., the last) variable of that name to place in the output file, NCO simply fails. User should re-try command after ensuring multiple objects of the same name will not be placed in the same group.\n",prg_nm_get(),fnc_nm,gpe_var_nm_fll);
            for(int idx=0;idx<nbr_gpe_nm;idx++) gpe_nm[idx].var_nm_fll=(char *)nco_free(gpe_nm[idx].var_nm_fll);
            nco_exit(EXIT_FAILURE);
          } /* strcmp() */
        } /* end loop over gpe_nm */
        gpe_nm=(gpe_nm_sct *)nco_realloc((void *)gpe_nm,(nbr_gpe_nm+1)*sizeof(gpe_nm_sct));
        gpe_nm[nbr_gpe_nm].var_nm_fll=strdup(gpe_var_nm_fll);
        nbr_gpe_nm++;
      } /* nbr_gpe_nm */

      /* Free full path name */
      if(gpe_var_nm_fll) gpe_var_nm_fll=(char *)nco_free(gpe_var_nm_fll);
    } /* !GPE */

    /* Define variable in output file. */
    var_out_id= nco_cpy_var_dfn(nc_id_1,nc_out_id,grp_id_1,grp_out_id,dfl_lvl,gpe,(char *)NULL,trv_1,trv_tbl_1);

    /* Set chunksize parameters */
    if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set_trv(grp_out_id,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr,trv_1);

    /* Copy variable's attributes */
    (void)nco_att_cpy(grp_id_1,grp_out_id,var_id_1,var_out_id,PCK_ATT_CPY); 

  }else{ /* Write mode */

    /* Get group ID */
    (void)nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_out_id);

    /* Get variable ID */
    (void)nco_inq_varid(grp_out_id,trv_1->nm,&var_out_id);         

    /* Copy non-processed variable */
    (void)nco_cpy_var_val_mlt_lmt_trv(grp_id_1,grp_out_id,(FILE *)NULL,(nco_bool)False,trv_1); 
  
  } /* Write mode */

  /* Free allocated variable structures */
  var_prc_1->val.vp=nco_free(var_prc_1->val.vp);
  var_prc_1=(var_sct *)nco_free(var_prc_1);
  var_prc_out=(var_sct *)nco_free(var_prc_out);

  /* Free output path name */
  grp_out_fll=(char *)nco_free(grp_out_fll);


} /* trv_tbl_prc() */

    