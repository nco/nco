/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.c,v 1.100 2013-03-23 17:25:30 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Testing:
   ncks -D 1 ~/nco/data/in_grp.nc */

#include "nco_grp_trv.h" /* Group traversal */
#include "nco_lmt.h" /* Hyperslab limits */

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
 const nco_bool CNV_CCM_CCSM_CF,       /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const nco_bool FIX_REC_CRD,           /* I [flg] Do not interpolate/multiply record coordinate variables (ncflint only) */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl),   /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl,                /* I [nbr] Number of altered dimensions */
 const int nco_op_typ,                 /* I [enm] Operation type (command line -y) */
 trv_tbl_sct * const trv_tbl_1,        /* I/O [sct] GTT (Group Traversal Table) */
 trv_tbl_sct * const trv_tbl_2,        /* I/O [sct] GTT (Group Traversal Table) */
 nco_bool flg_def)                     /* I [flg] Action type (True for define variables, False when write variables ) */
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
    char *var_nm_fll;       /* [sng] Full path of variable */
    nco_bool flg_in_fl[2];  /* [flg] Is this name if each file?; files are [0] and [1] */
  } nco_cmn_t;

  nco_cmn_t *cmn_lst=NULL;       /* [sct] A list of common names */ 

  int nbr_tbl_1;                 /* [nbr] Number of items in list 1 */
  int nbr_tbl_2;                 /* [nbr] Number of items in list 2 */
  int nco_cmp;                   /* [nbr] Return value of strcmp() */ 
  int idx_tbl_1;                 /* [idx] Current position in List 1 */ 
  int idx_tbl_2;                 /* [idx] Current position in List 2 */ 
  int idx_lst;                   /* [idx] Current position in common List */ 
  int fl_fmt;                    /* [enm] netCDF file format */
  int grp_id_1;                  /* [id] Group ID in input file */
  int var_id_1;                  /* [id] Variable ID in input file */
  int grp_id_2;                  /* [id] Group ID in input file */
  int var_id_2;                  /* [id] Variable ID in input file */
  int grp_out_id;                /* [id] Group ID in output file */ 
  int var_out_id;                /* [id] Variable ID in output file */

  nco_bool flg_more_names_exist; /* [flg] Are there more names to process? */
  nco_bool flg_in_fl[2];         /* [flg] Is this name if each file?; files are [0] and [1] */

  /* Get output file format */
  (void)nco_inq_format(nc_out_id,&fl_fmt);

  /* Tables *must* be sorted */
  (void)trv_tbl_srt(trv_tbl_1);
  (void)trv_tbl_srt(trv_tbl_2);

  if(dbg_lvl_get() == 15){
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

  /* Put counters ar start */
  idx_tbl_1=0;
  idx_tbl_2=0;
  idx_lst=0;

  /* Store a list of common objects */
  cmn_lst=(nco_cmn_t *)nco_malloc((nbr_tbl_1+nbr_tbl_2)*sizeof(nco_cmn_t));

  /* Define mode */
  if(flg_def){

    /* Copy global attributes */
    (void)nco_att_cpy(nc_id_1,nc_out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
  }

  /* Iterate the 2 lists */
  while (flg_more_names_exist)
  {
    trv_sct trv_1=trv_tbl_1->lst[idx_tbl_1];
    trv_sct trv_2=trv_tbl_2->lst[idx_tbl_2];

    /* Criteria is string compare */
    nco_cmp = strcmp(trv_1.nm_fll,trv_2.nm_fll);

    /* Names are the same: store flag True for both items and read the next names from the two lists */
    if (nco_cmp == 0)
    {
      flg_in_fl[0]=True; 
      flg_in_fl[1]=True;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_1.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() == 15)(void)fprintf(stdout,"%s: INFO %s reports tbl_1[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_1,trv_1.nm_fll);

      /* If object is an extracted variable... */
      if(trv_1.nco_typ == nco_obj_typ_var && trv_2.nco_typ == nco_obj_typ_var && trv_1.flg_xtr && trv_2.flg_xtr){

        if(dbg_lvl_get() >= 16){ 
          (void)fprintf(stdout,"%s: INFO %s reports new element in output:%s\n",prg_nm_get(),fnc_nm,trv_1.nm_fll); 
        } 


        /* Obtain group ID using full group name */
        (void)nco_inq_grp_full_ncid(nc_id_1,trv_1.grp_nm_fll,&grp_id_1);
        (void)nco_inq_grp_full_ncid(nc_id_2,trv_2.grp_nm_fll,&grp_id_2);

        /* Get variable ID */
        (void)nco_inq_varid(grp_id_1,trv_1.nm,&var_id_1);
        (void)nco_inq_varid(grp_id_2,trv_2.nm,&var_id_2);


        /* Define mode */
        if(flg_def){     

          /* If output group does not exist, create it */
          if(nco_inq_grp_full_ncid_flg(nc_out_id,trv_1.grp_nm_fll,&grp_out_id)){
            nco_def_grp_full(nc_out_id,trv_1.grp_nm_fll,&grp_out_id);
          } /* Create group */

          /* Define variable in output file. NB. using table 1 as parameter */
          var_out_id=nco_cpy_var_dfn(nc_id_1,nc_out_id,grp_id_1,grp_out_id,dfl_lvl,NULL,NULL,&trv_1,trv_tbl_1);

          /* Set chunksize parameters */
          if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set_trv(grp_out_id,&cnk_map,&cnk_plc,cnk_sz_scl,cnk,cnk_nbr,&trv_1);

          /* Copy variable's attributes */
          (void)nco_att_cpy(grp_id_1,grp_out_id,var_id_1,var_out_id,(nco_bool)True);

        }

        /* Write mode */
        else {

          op_typ_enm op_typ_1; /* [enm] Operation type */
          op_typ_enm op_typ_2; /* [enm] Operation type */

          (void)nco_var_op_typ(&trv_1,CNV_CCM_CCSM_CF,FIX_REC_CRD,cnk_map,cnk_plc,dmn_xcl,nbr_dmn_xcl,&op_typ_1);                    
          (void)nco_var_op_typ(&trv_2,CNV_CCM_CCSM_CF,FIX_REC_CRD,cnk_map,cnk_plc,dmn_xcl,nbr_dmn_xcl,&op_typ_2);

          /* Get group ID */
          (void)nco_inq_grp_full_ncid(nc_out_id,trv_1.grp_nm_fll,&grp_out_id);

          /* Get variable ID */
          (void)nco_inq_varid(grp_out_id,trv_1.nm,&var_out_id);

          /* Non-processed variable */
          if (op_typ_1 == fix){

            /* Copy variable data for non-processed variables */
            (void)nco_cpy_var_val_mlt_lmt_trv(nc_id_1,nc_out_id,(FILE *)NULL,False,&trv_1);

          } /* Non-processed variable */

          /* Processed variable */
          if (op_typ_1 == prc){

            var_sct var_prc_1;
            var_sct var_prc_2;
            var_sct var_prc_out;
            int has_mss_val=False;
            ptr_unn mss_val;

            var_prc_1.nbr_dim=trv_1.nbr_dmn;
            var_prc_2.nbr_dim=trv_2.nbr_dmn;
            var_prc_1.id=var_id_1;
            var_prc_2.id=var_id_2;
            var_prc_1.typ_dsk=trv_1.var_typ;
            var_prc_2.typ_dsk=trv_2.var_typ;
            var_prc_1.type=trv_1.var_typ;
            var_prc_2.type=trv_2.var_typ;
            var_prc_1.typ_upk=trv_1.var_typ;
            var_prc_2.typ_upk=trv_1.var_typ;
            var_prc_1.has_scl_fct=False;
            var_prc_2.has_scl_fct=False;
            var_prc_1.has_add_fst=False;
            var_prc_2.has_add_fst=False;
            var_prc_1.pck_dsk=False;
            var_prc_2.pck_dsk=False;

            var_prc_out.id=var_out_id;
     

            /* Read hyperslab from first file */
            (void)nco_msa_var_get_trv(grp_id_1,&var_prc_1,&trv_1);

            /* Read hyperslab from second file */
            (void)nco_msa_var_get_trv(grp_id_2,&var_prc_2,&trv_2);

            /* Perform specified binary operation */
            switch(nco_op_typ){
            case nco_op_add: /* [enm] Add file_1 to file_2 */
              (void)nco_var_add(var_prc_1.type,var_prc_1.sz,has_mss_val,mss_val,var_prc_2.val,var_prc_1.val); break;
            case nco_op_mlt: /* [enm] Multiply file_1 by file_2 */
              (void)nco_var_mlt(var_prc_1.type,var_prc_1.sz,has_mss_val,mss_val,var_prc_2.val,var_prc_1.val); break;
            case nco_op_dvd: /* [enm] Divide file_1 by file_2 */
              (void)nco_var_dvd(var_prc_1.type,var_prc_1.sz,has_mss_val,mss_val,var_prc_2.val,var_prc_1.val); break;
            case nco_op_sbt: /* [enm] Subtract file_2 from file_1 */
              (void)nco_var_sbt(var_prc_1.type,var_prc_1.sz,has_mss_val,mss_val,var_prc_2.val,var_prc_1.val); break;
            default: /* Other defined nco_op_typ values are valid for ncra(), ncrcat(), ncwa(), not ncbo() */
              (void)fprintf(stdout,"%s: ERROR Illegal nco_op_typ in binary operation\n",prg_nm_get);
              nco_exit(EXIT_FAILURE);
              break;
            } /* end case */

            var_prc_2.val.vp=nco_free(var_prc_2.val.vp);

            /* Copy result to output file and free workspace buffer */
            if(var_prc_1.nbr_dim == 0){
              (void)nco_put_var1(nc_out_id,var_prc_out.id,var_prc_out.srt,var_prc_1.val.vp,var_prc_1.type);
            }else{ /* end if variable is scalar */
              (void)nco_put_vara(nc_out_id,var_prc_out.id,var_prc_out.srt,var_prc_out.cnt,var_prc_1.val.vp,var_prc_1.type);
            } /* end else */

            var_prc_1.val.vp=nco_free(var_prc_1.val.vp);

          } /* Processed variable */
        } /* Write mode */
      }  /* If object is an extracted variable... */ 

      idx_tbl_1++;
      idx_tbl_2++;
    }
    /* Name(1) is less than Name(2), read the next name from List 1  */
    else if (nco_cmp < 0)
    {
      flg_in_fl[0]=True; 
      flg_in_fl[1]=False;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_1.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() == 15)(void)fprintf(stdout,"%s: INFO %s reports tbl_1[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_1,trv_1.nm_fll);

      idx_tbl_1++;
    }
    /* Name(1) is greater than Name(2), read the next name from List 2 */
    else
    {
      flg_in_fl[0]=False; 
      flg_in_fl[1]=True;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_2.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() == 15)(void)fprintf(stdout,"%s: INFO %s reports tbl_2[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_2,trv_2.nm_fll);

      idx_tbl_2++;
    }

    flg_more_names_exist = (idx_tbl_1<nbr_tbl_1 && idx_tbl_2<nbr_tbl_2) ? 1 : 0;
  } /* end while */


  /* List1 did not end */
  if (idx_tbl_1<nbr_tbl_1)
  {
    while (idx_tbl_1<nbr_tbl_1)
    {
      trv_sct trv_1=trv_tbl_1->lst[idx_tbl_1];

      flg_in_fl[0]=True; 
      flg_in_fl[1]=False;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_1.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() == 15)(void)fprintf(stdout,"%s: INFO %s reports tbl_1[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_1,trv_1.nm_fll);

      idx_tbl_1++;
    }
  }

  /* List2 did not end */
  if (idx_tbl_2<nbr_tbl_2)
  {
    while (idx_tbl_2<nbr_tbl_2)
    {
      trv_sct trv_2=trv_tbl_2->lst[idx_tbl_2];

      flg_in_fl[0]=False; 
      flg_in_fl[1]=True;
      cmn_lst[idx_lst].flg_in_fl[0]=flg_in_fl[0]; 
      cmn_lst[idx_lst].flg_in_fl[1]=flg_in_fl[1];
      cmn_lst[idx_lst].var_nm_fll=strdup(trv_2.nm_fll);
      idx_lst++;

      if(dbg_lvl_get() == 15)(void)fprintf(stdout,"%s: INFO %s reports tbl_2[%d]:%s\n",prg_nm_get(),fnc_nm,idx_tbl_2,trv_2.nm_fll);

      idx_tbl_2++;
    }
  }

  /* Print the list */
  if(dbg_lvl_get() >= nco_dbg_vrb){
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
  }


  for(int idx=0;idx<idx_lst;idx++){
    cmn_lst[idx].var_nm_fll=(char *)nco_free(cmn_lst[idx].var_nm_fll);
  } 
  cmn_lst=(nco_cmn_t *)nco_free(cmn_lst);

} /* trv_tbl_mch() */




