/* $Header$ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* This file contains the API for low level group data structures:
   Group Traversal Table (GTT): functions prefixed with "trv_tbl_"
   It does not include any netCDF API calls */

#include "nco_grp_trv.h" /* Group traversal */

void                          
trv_tbl_init                           /* [fnc] GTT initialize */
(trv_tbl_sct **tbl)                    /* I/O [sct] Traversal table */
{
  trv_tbl_sct *tb=(trv_tbl_sct *)nco_malloc(sizeof(trv_tbl_sct));
  
  /* Object (group/variable) list */
  tb->nbr=0;
  tb->lst=NULL; 
  
  /* Dimension list */
  tb->nbr_dmn=0;
  tb->lst_dmn=NULL;
  
  /* Degenerate dimensions used by ncwa */
  tb->nbr_dmn_dgn=0;
  tb->dmn_dgn=NULL;
  
  /* Ensembles */
  tb->nsm_nbr=0;
  tb->nsm=NULL;
  tb->nsm_sfx=NULL;
  
  *tbl=tb;
} /* trv_tbl_init() */

void 
trv_tbl_free                           /* [fnc] GTT free memory */
(trv_tbl_sct *tbl)                     /* I [sct] Traversal table */
{
  const char fnc_nm[]="trv_tbl_free()"; /* [sng] Function name  */
  
#ifdef DEBUG_LEAKS
  int crt_counter=0;
#endif
  
  /* Hash Table */
  nco_trv_hsh_del(tbl);
  
  /* Object (group/variable) list */
  for(unsigned idx=0;idx<tbl->nbr;idx++){
    tbl->lst[idx].nm_fll=(char *)nco_free(tbl->lst[idx].nm_fll);
    tbl->lst[idx].nm=(char *)nco_free(tbl->lst[idx].nm);
    tbl->lst[idx].grp_nm=(char *)nco_free(tbl->lst[idx].grp_nm);
    tbl->lst[idx].grp_nm_fll=(char *)nco_free(tbl->lst[idx].grp_nm_fll);
    tbl->lst[idx].grp_nm_fll_prn=(char *)nco_free(tbl->lst[idx].grp_nm_fll_prn);
    tbl->lst[idx].nsm_nm=(char *)nco_free(tbl->lst[idx].nsm_nm);
    tbl->lst[idx].rec_dmn_nm_out=(char *)nco_free(tbl->lst[idx].rec_dmn_nm_out);
    tbl->lst[idx].hsh_key=(char *)nco_free(tbl->lst[idx].hsh_key);
    
    /* Dimensions */
    for(int dmn_idx=0;dmn_idx<tbl->lst[idx].nbr_dmn;dmn_idx++){
      
      /* If dimensions exist (only for variables ) */
      if(tbl->lst[idx].var_dmn){
        tbl->lst[idx].var_dmn[dmn_idx].dmn_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].dmn_nm_fll);
        tbl->lst[idx].var_dmn[dmn_idx].dmn_nm=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].dmn_nm);
        tbl->lst[idx].var_dmn[dmn_idx].grp_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].grp_nm_fll);
	
        int nbr_lat_crd=tbl->lst[idx].var_dmn[dmn_idx].nbr_lat_crd;
        for(int idx_crd=0;idx_crd<nbr_lat_crd;idx_crd++) tbl->lst[idx].var_dmn[dmn_idx].lat_crd[idx_crd].nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].lat_crd[idx_crd].nm_fll);

        int nbr_lon_crd=tbl->lst[idx].var_dmn[dmn_idx].nbr_lon_crd;
        for(int idx_crd=0;idx_crd<nbr_lon_crd;idx_crd++) tbl->lst[idx].var_dmn[dmn_idx].lon_crd[idx_crd].nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].lon_crd[idx_crd].nm_fll);

        /* Coordinate structure */ 
        if(tbl->lst[idx].var_dmn[dmn_idx].crd){
          tbl->lst[idx].var_dmn[dmn_idx].crd->crd_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].crd->crd_nm_fll);
          tbl->lst[idx].var_dmn[dmn_idx].crd->dmn_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].crd->dmn_nm_fll);
          tbl->lst[idx].var_dmn[dmn_idx].crd->crd_grp_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].crd->crd_grp_nm_fll);
          tbl->lst[idx].var_dmn[dmn_idx].crd->dmn_grp_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].crd->dmn_grp_nm_fll);
          tbl->lst[idx].var_dmn[dmn_idx].crd->nm=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].crd->nm);
          tbl->lst[idx].var_dmn[dmn_idx].crd->lmt_msa.dmn_nm=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].crd->lmt_msa.dmn_nm);
          for(int lmt_idx=0;lmt_idx<tbl->lst[idx].var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn_nbr;lmt_idx++)
            tbl->lst[idx].var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn[lmt_idx]=nco_lmt_free(tbl->lst[idx].var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn[lmt_idx]);
          tbl->lst[idx].var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn=(lmt_sct **)nco_free(tbl->lst[idx].var_dmn[dmn_idx].crd->lmt_msa.lmt_dmn);
          tbl->lst[idx].var_dmn[dmn_idx].crd=(crd_sct *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].crd);
#ifdef DEBUG_LEAKS
          crt_counter++;
#endif
        }else if(tbl->lst[idx].var_dmn[dmn_idx].ncd){
          tbl->lst[idx].var_dmn[dmn_idx].ncd->grp_nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].ncd->grp_nm_fll);
          tbl->lst[idx].var_dmn[dmn_idx].ncd->nm_fll=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].ncd->nm_fll);
          tbl->lst[idx].var_dmn[dmn_idx].ncd->nm=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].ncd->nm);
          tbl->lst[idx].var_dmn[dmn_idx].ncd->lmt_msa.dmn_nm=(char *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].ncd->lmt_msa.dmn_nm);
          for(int lmt_idx=0;lmt_idx<tbl->lst[idx].var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn_nbr;lmt_idx++)
            tbl->lst[idx].var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn[lmt_idx]=nco_lmt_free(tbl->lst[idx].var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn[lmt_idx]);
          tbl->lst[idx].var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn=(lmt_sct **)nco_free(tbl->lst[idx].var_dmn[dmn_idx].ncd->lmt_msa.lmt_dmn);
          tbl->lst[idx].var_dmn[dmn_idx].ncd=(dmn_trv_sct *)nco_free(tbl->lst[idx].var_dmn[dmn_idx].ncd);
        }
      } /* Dimensions */
      
      tbl->lst[idx].var_dmn=(var_dmn_sct *)nco_free(tbl->lst[idx].var_dmn);
      tbl->lst[idx].dmn_idx_out_in=(int *)nco_free(tbl->lst[idx].dmn_idx_out_in);
      tbl->lst[idx].dmn_rvr_in=(nco_bool *)nco_free(tbl->lst[idx].dmn_rvr_in);
    } /* If dimensions exist (only for variables) */
  } /* Object (group/variable) list */ 

  tbl->lst=(trv_sct *)nco_free(tbl->lst);

  /* Dimension list */
  for(unsigned int dmn_idx=0;dmn_idx<tbl->nbr_dmn;dmn_idx++){
    tbl->lst_dmn[dmn_idx].nm=(char *)nco_free(tbl->lst_dmn[dmn_idx].nm);
    tbl->lst_dmn[dmn_idx].nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].nm_fll);
    tbl->lst_dmn[dmn_idx].grp_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].grp_nm_fll);
    tbl->lst_dmn[dmn_idx].lmt_msa.dmn_nm=(char *)nco_free(tbl->lst_dmn[dmn_idx].lmt_msa.dmn_nm);
    for(int lmt_idx=0;lmt_idx<tbl->lst_dmn[dmn_idx].lmt_msa.lmt_dmn_nbr;lmt_idx++)
      tbl->lst_dmn[dmn_idx].lmt_msa.lmt_dmn[lmt_idx]=nco_lmt_free(tbl->lst_dmn[dmn_idx].lmt_msa.lmt_dmn[lmt_idx]);

    /* Total number of coordinate variables for this dimension */
    int crd_nbr=tbl->lst_dmn[dmn_idx].crd_nbr;

    /* Coordinate structures */ 
    for(int crd_idx=0;crd_idx<crd_nbr;crd_idx++){
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->crd_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->crd_nm_fll);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_nm_fll);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->crd_grp_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->crd_grp_nm_fll);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_grp_nm_fll=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->dmn_grp_nm_fll);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->nm=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->nm);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]->lmt_msa.dmn_nm=(char *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->lmt_msa.dmn_nm);
      for(int lmt_idx=0;lmt_idx<tbl->lst_dmn[dmn_idx].crd[crd_idx]->lmt_msa.lmt_dmn_nbr;lmt_idx++)
        tbl->lst_dmn[dmn_idx].crd[crd_idx]->lmt_msa.lmt_dmn[lmt_idx]=nco_lmt_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]->lmt_msa.lmt_dmn[lmt_idx]);
      tbl->lst_dmn[dmn_idx].crd[crd_idx]=(crd_sct *)nco_free(tbl->lst_dmn[dmn_idx].crd[crd_idx]);
    }  /* Coordinate structures */

    tbl->lst_dmn[dmn_idx].crd=(crd_sct **)nco_free(tbl->lst_dmn[dmn_idx].crd);
  } /* Dimension list */
  tbl->lst_dmn=(dmn_trv_sct *)nco_free(tbl->lst_dmn);

  /* Members used only by transformation operators (non-ncks) */
  /* (ncwa) Degenerate dimensions */
  tbl->dmn_dgn=(dmn_sct *)nco_free(tbl->dmn_dgn); 

  /* Ensembles */
  for(int idx_nsm=0;idx_nsm<tbl->nsm_nbr;idx_nsm++){
    tbl->nsm[idx_nsm].grp_nm_fll_prn=(char *)nco_free(tbl->nsm[idx_nsm].grp_nm_fll_prn);
    for(int idx=0;idx<tbl->nsm[idx_nsm].tpl_nbr;idx++) tbl->nsm[idx_nsm].tpl_mbr_nm[idx]=(char *)nco_free(tbl->nsm[idx_nsm].tpl_mbr_nm[idx]);
    for(int idx=0;idx<tbl->nsm[idx_nsm].skp_nbr;idx++) tbl->nsm[idx_nsm].skp_nm_fll[idx]=(char *)nco_free(tbl->nsm[idx_nsm].skp_nm_fll[idx]);
    tbl->nsm[idx_nsm].mbr=(nsm_grp_sct*)nco_free(tbl->nsm[idx_nsm].mbr);
  } /* Ensembles */
  tbl->nsm_sfx=(char *)nco_free(tbl->nsm_sfx);  
  tbl=(trv_tbl_sct *)nco_free(tbl);
#ifdef DEBUG_LEAKS
  if(nco_dbg_lvl_get() >= nco_dbg_sup && nco_dbg_lvl_get() <= nco_dbg_nbr) (void)fprintf(stdout,"%s: DEBUG %s %d crd\n",nco_prg_nm_get(),fnc_nm,crt_counter);
#endif
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
 int * const var_udt_all,            /* O [nbr] Number of non-atomic variables in file */
 int * const var_tmc_all,            /* O [nbr] Number of atomic-type variables in file */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  /* [fnc] Find and return global file summaries like # of dimensions, variables, attributes */
  //const char fnc_nm[]="trv_tbl_inq()"; /* [sng] Function name  */

  int att_glb_lcl; /* [nbr] Number of global attributes in file */
  int att_grp_lcl; /* [nbr] Number of group attributes in file */
  int att_var_lcl; /* [nbr] Number of variable attributes in file */
  int dmn_rec_lcl; /* [nbr] Number of record dimensions in file */
  int grp_dpt_lcl; /* [nbr] Maximum group depth (root = 0) */
  int grp_nbr_lcl; /* [nbr] Number of groups in file */
  int typ_nbr_lcl; /* [nbr] Number of user-defined types in file */
  int var_udt_lcl; /* [nbr] Number of non-atomic variables in file */
  int var_tmc_lcl; /* [nbr] Number of atomic-type variables in file */

  /* Initialize */
  att_glb_lcl=0;
  att_grp_lcl=0;
  att_var_lcl=0;
  dmn_rec_lcl=0;
  grp_dpt_lcl=0;
  grp_nbr_lcl=0;
  typ_nbr_lcl=0;
  var_udt_lcl=0;
  var_tmc_lcl=0;
    
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl]; 
    if(var_trv.nco_typ == nco_obj_typ_var) att_var_lcl+=var_trv.nbr_att;
    if(var_trv.nco_typ == nco_obj_typ_nonatomic_var) var_udt_lcl++;
    if(var_trv.nco_typ == nco_obj_typ_grp){ 
      typ_nbr_lcl+=var_trv.nbr_typ;
      grp_nbr_lcl+=var_trv.nbr_grp;
      var_tmc_lcl+=var_trv.nbr_var;
      if(grp_dpt_lcl < var_trv.grp_dpt) grp_dpt_lcl=var_trv.grp_dpt;
      if(!strcmp(var_trv.nm_fll,"/")) att_glb_lcl=var_trv.nbr_att; else att_grp_lcl+=var_trv.nbr_att; 
    } /* end nco_obj_typ_grp */
  } /* end idx_tbl */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr_dmn;idx_tbl++)
    if(trv_tbl->lst_dmn[idx_tbl].is_rec_dmn) dmn_rec_lcl++;

  if(typ_nbr_lcl > 0 || var_udt_lcl > 0){
    (void)fprintf(stderr,"%s: WARNING File contains %d user-defined types (i.e., compound, enum, opaque, or vlen) used to define %d non-atomic variables and their attributes. NCO currently ignores variables and attributes with user-defined types by default, although some preliminary features can be accessed with the --udt flag. Nevertheless, most %s features will only work for atomic variables.\n",nco_prg_nm_get(),typ_nbr_lcl,var_udt_lcl,nco_prg_nm_get());
  } /* !var_udt_lcl */

  if(att_glb_all) *att_glb_all=att_glb_lcl;
  if(att_grp_all) *att_grp_all=att_grp_lcl;
  if(att_var_all) *att_var_all=att_var_lcl;
  if(dmn_nbr_all) *dmn_nbr_all=trv_tbl->nbr_dmn;
  if(dmn_rec_all) *dmn_rec_all=dmn_rec_lcl;
  if(grp_dpt_all) *grp_dpt_all=grp_dpt_lcl;
  if(grp_nbr_all) *grp_nbr_all=grp_nbr_lcl;
  if(var_udt_all) *var_udt_all=var_udt_lcl;
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
} /* end trv_tbl_prn_flg_mch() */

void 
trv_tbl_prn_flg_xtr                  /* [fnc] Print table items that have .flg_xtr  */
(const char * const fnc_nm,          /* I [sng] Function name  */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  (void)fprintf(stdout,"%s: INFO %s reports extracted objects:\n",nco_prg_nm_get(),fnc_nm);
 /* Print all matching objects from traversal table */
  trv_sct trv_obj;
  for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){
    trv_obj=trv_tbl->lst[tbl_idx];
    if(trv_obj.flg_xtr) (void)fprintf(stdout,"%s\n",trv_obj.nm_fll);
  } /* end loop over trv_tbl */
} /* end trv_tbl_prn_flg_xtr() */

void
trv_tbl_prn_dbg                      /* [fnc] Print several table members fields (debug only) */
(const char * const fnc_nm,          /* I [sng] Function name  */
const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  (void)fprintf(stdout, "%s: INFO %s reports extracted objects:\n", nco_prg_nm_get(), fnc_nm);
  /* Print all matching objects from traversal table */
  trv_sct trv_obj;
  for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){
    trv_obj=trv_tbl->lst[tbl_idx];
    if(trv_obj.flg_xtr && trv_obj.nco_typ == nco_obj_typ_var){
      (void)fprintf(stdout,"%s\n",trv_obj.nm_fll);
      (void)fprintf(stdout,"   %d dimensions: ",trv_obj.nbr_dmn);
      for(int idx_dmn=0; idx_dmn<trv_obj.nbr_dmn;idx_dmn++)
        (void)fprintf(stdout, " %s ",trv_obj.var_dmn[idx_dmn].dmn_nm);
      (void)fprintf(stdout,"\n");
      (void)fprintf(stdout,"   record dimension name: ");
      if(trv_obj.rec_dmn_nm_out) (void)fprintf(stdout, "%s\n ", trv_obj.rec_dmn_nm_out); else (void)fprintf(stdout, "NULL\n");
    }
  } /* end loop over trv_tbl */
} /* end trv_tbl_prn_flg_xtr() */

void                          
trv_tbl_prn                          /* [fnc] Print table with -z */
(const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */  
{
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_grp) (void)fprintf(stdout,"grp: "); else (void)fprintf(stdout,"var: ");
    (void)fprintf(stdout,"%s\n",trv_tbl->lst[idx_tbl].nm_fll); 
  } /* end idx_tbl */
} /* end trv_tbl_prn() */

char *                                /* O [flg] Full variable name */
nco_gid_var_nm_2_var_nm_fll           /* [fnc] Construct full variable name from group ID and variable name */
(const int grp_id,                    /* I [ID] netCDF input group ID */
 const char * const var_nm)           /* I [sng] Variable name */
{
  char *var_nm_fll;
  size_t grp_nm_lng;
  size_t var_nm_fll_lng;
  (void)nco_inq_grpname_full(grp_id,&grp_nm_lng,NULL);
  /* Groups except root need a slash character to separate their name from variable name */
  var_nm_fll_lng=grp_nm_lng+strlen(var_nm);
  if(grp_nm_lng != 1L) var_nm_fll_lng++;
  /* One extra space for NUL-terminator */
  var_nm_fll=(char *)nco_malloc(var_nm_fll_lng+1L);
  (void)nco_inq_grpname_full(grp_id,NULL,var_nm_fll);
  if(grp_nm_lng != 1L) strcat(var_nm_fll,"/");
  strcat(var_nm_fll,var_nm);
  return var_nm_fll;
} /* end nco_gid_var_nm_2_var_nm_fll() */

nco_bool                              /* O [flg] Item found or not */
trv_tbl_fnd_var_nm_fll                /* [fnc] Check if "var_nm_fll" is in table */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !strcmp(var_nm_fll,trv_tbl->lst[idx_tbl].nm_fll))
      return True;

  return False;
} /* end trv_tbl_fnd_var_nm_fll() */

trv_sct *                             /* O [sct] Table object */
trv_tbl_var_nm_fll                    /* [fnc] Return variable object from full name key */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Purpose: Return variable object with given full name */

#ifdef NCO_HSH_TRV_OBJ
  trv_sct *trv_obj; /* [sct] GTT object structure */
  HASH_FIND_STR(trv_tbl->hsh,var_nm_fll,trv_obj);
  if(trv_obj && trv_obj->nco_typ == nco_obj_typ_var) return trv_obj; else return NULL;
#else /* !NCO_HSH_TRV_OBJ */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var)
      if(!strcmp(var_nm_fll,trv_tbl->lst[idx_tbl].nm_fll))
	return &trv_tbl->lst[idx_tbl];

  return NULL;
#endif /* !NCO_HSH_TRV_OBJ */

} /* trv_tbl_var_nm_fll() */

trv_sct *                             /* O [sct] Table object */
trv_tbl_grp_nm_fll                    /* [fnc] Return group object from full name key */
(const char * const grp_nm_fll,       /* I [sng] Group name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Purpose: Return group object with given full name */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_grp && !strcmp(grp_nm_fll,trv_tbl->lst[idx_tbl].nm_fll))
      return &trv_tbl->lst[idx_tbl];

  return NULL;
} /* trv_tbl_grp_nm_fll() */

void
trv_tbl_mrk_xtr                       /* [fnc] Mark extraction flag in table for "var_nm_fll" */
(const char * const var_nm_fll,       /* I [sng] Variable name to find */
 const nco_bool flg_xtr,              /* I [flg] Flag (True or False) */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] Traversal table */
{

#ifdef NCO_HSH_TRV_OBJ
  trv_sct *trv_obj; /* [sct] GTT object structure */
  HASH_FIND_STR(trv_tbl->hsh,var_nm_fll,trv_obj);
  if(trv_obj) trv_obj->flg_xtr=flg_xtr;
#else /* !NCO_HSH_TRV_OBJ */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(!strcmp(var_nm_fll,trv_tbl->lst[idx_tbl].nm_fll)) trv_tbl->lst[idx_tbl].flg_xtr=flg_xtr;
#endif /* !NCO_HSH_TRV_OBJ */

  return;
} /* end trv_tbl_mrk_xtr() */

void
trv_tbl_mrk_grp_xtr                   /* [fnc] Mark extraction flag in table for "grp_nm_fll" */
(const char * const grp_nm_fll,       /* I [sng] Group name to find */
 const nco_bool flg_xtr,              /* I [flg] Flag (True or False) */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] Traversal table */
{
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(!strcmp(grp_nm_fll,trv_tbl->lst[idx_tbl].nm_fll))
      trv_tbl->lst[idx_tbl].flg_xtr=flg_xtr;

  return;
} /* end trv_tbl_mrk_grp_xtr() */

void
trv_tbl_mrk_prc_fix                    /* [fnc] Mark fixed/processed flag in table for "var_nm_fll" */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 prc_typ_enm typ_prc,                  /* I [enm] Processing type */
 trv_tbl_sct * const trv_tbl)          /* I/O [sct] Traversal table */
{
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    if(!strcmp(var_nm_fll,trv_tbl->lst[idx_tbl].nm_fll)){
      trv_tbl->lst[idx_tbl].enm_prc_typ=typ_prc;
      return;
    }
  }
  assert(0);
} /* end trv_tbl_mrk_prc_fix() */

void 
trv_tbl_prn_xtr                        /* [fnc] Print extraction flag of traversal table */
(const trv_tbl_sct * const trv_tbl,    /* I [sct] Traversal table */
 const char * const fnc_nm)            /* I [sng] Function name of the calling function */
{
  int idx=0;
  int nbr_flg=0;

  /* Loop table */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].flg_xtr) nbr_flg++;

  (void)fprintf(stdout,"%s: INFO %s reports %d objects with extraction flag (flg_xtr) set:\n",nco_prg_nm_get(),fnc_nm,nbr_flg); 
  
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].flg_xtr) (void)fprintf(stdout,"%d %s\n",idx++,trv_tbl->lst[idx_tbl].nm_fll); 

} /* end trv_tbl_prn_xtr() */

int                                    /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
trv_tbl_cmp_asc_nm_fll                 /* [fnc] Compare two trv_sct's by full name member, return ascending order */
(const void *val_1,                    /* I [sct] trv_sct to compare */
 const void *val_2)                    /* I [sct] trv_sct to compare */
{
  /* Purpose: Compare two trv_sct's by name structure member
     Comparison results, when interpreted by qsort(), sort in ascending (alphabetical) order
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     Code based on responses to my comp.lang.c thread 20040101 */
  return strcmp((*(trv_sct const *)val_1).nm_fll,(*(trv_sct const *)val_2).nm_fll);
} /* end nco_cmp_trv_tbl_nm() */

int                                    /* O [enm] Comparison result [<,=,>] 0 iff val_1 [>,==,<] val_2 */
trv_tbl_cmp_dsc_nm_fll                 /* [fnc] Compare two trv_sct's by full name member, return descending order */
(const void *val_1,                    /* I [sct] trv_sct to compare */
 const void *val_2)                    /* I [sct] trv_sct to compare */
{
  /* Purpose: Compare two trv_sct's by name structure member
     Comparison results, when interpreted by qsort(), sort in descending (reverse alphabetical) order
     Function is suitable for argument to ANSI C qsort() routine in stdlib.h
     Code based on responses to my comp.lang.c thread 20040101 */
  return -1*strcmp((*(trv_sct const *)val_1).nm_fll,(*(trv_sct const *)val_2).nm_fll);
} /* end nco_cmp_trv_tbl_nm() */

void 
trv_tbl_srt                            /* [fnc] Sort traversal table */
(const int srt_mth,                    /* [enm] Sort method */
 trv_tbl_sct * const trv_tbl)          /* I/O [sct] Traversal table */
{
  /* Purpose: Alphabetize list by object full name
     This produces easy-to-search variable name screen output with ncks */
  if(srt_mth == 0) qsort(trv_tbl->lst,(size_t)trv_tbl->nbr,sizeof(trv_sct),trv_tbl_cmp_asc_nm_fll); else if(srt_mth == 1) qsort(trv_tbl->lst,(size_t)trv_tbl->nbr,sizeof(trv_sct),trv_tbl_cmp_dsc_nm_fll);
} /* end trv_tbl_srt() */

void                          
trv_tbl_mch                            /* [fnc] Match 2 tables (find common objects) and export common objects  */
(const trv_tbl_sct * const trv_tbl_1,  /* I [sct] GTT (Group Traversal Table) */
 const trv_tbl_sct * const trv_tbl_2,  /* I [sct] GTT (Group Traversal Table) */
 nco_cmn_t **cmn_lst,                  /* I/O [sct] List of common names */
 int * nbr_cmn_nm)                     /* I/O [nbr] Number of common names */
{
  /* Purpose: Find common objects. 
     Use cosequential match algorithm described in
     Folk, Michael; Zoellick, Bill. (1992). File Structures. Addison-Wesley.
     
     Compare 2 ordered lists of names:
     if Name(1) is less than Name(2), read next name from List 1; this is done by incrementing current index
     if Name(1) is greater than Name(2), read next name from List 2
     if names are identical, read next names from both lists 
     
     Used in ncbo; ncbo performs binary operations on variables in file 1 and matching variables
     (those with the same name) in file 2 and stores results in file 3.
     This function detects common absolute names in tables 1 and 2 and does ncbo binary operation 
     
     NB: cosequential match algorithm requires alphabetical sorted full names. The table is sorted in nco_bld_trv_tbl() only for ncbo
     */
  
  const char fnc_nm[]="trv_tbl_mch()"; /* [sng] Function name */
  
  int idx_lst;                   /* [idx] Current position in common List */ 
  int idx_tbl_1;                 /* [idx] Current position in List 1 */ 
  int idx_tbl_2;                 /* [idx] Current position in List 2 */ 
  int nbr_tbl_1;                 /* [nbr] Number of items in list 1 */
  int nbr_tbl_2;                 /* [nbr] Number of items in list 2 */
  int nco_cmp;                   /* [nbr] Return value of strcmp() */ 

  nco_bool flg_more_names_exist; /* [flg] Are there more names to process? */

  if(nco_dbg_lvl_get() == nco_dbg_old){
    (void)fprintf(stdout,"%s: INFO %s reports Sorted table 1\n",nco_prg_nm_get(),fnc_nm);
    (void)trv_tbl_prn(trv_tbl_1);
    (void)fprintf(stdout,"%s: INFO %s reports Sorted table 2\n",nco_prg_nm_get(),fnc_nm);
    (void)trv_tbl_prn(trv_tbl_2);
  } /* endif dbg */

  /* Get number of objects in each table */
  nbr_tbl_1=trv_tbl_1->nbr;
  nbr_tbl_2=trv_tbl_2->nbr;

  /* If both lists have names, then there are names to process */
  flg_more_names_exist = (nbr_tbl_1 > 0 && nbr_tbl_2 > 0) ? True : False;

  /* Initialize counters */
  idx_tbl_1=0;
  idx_tbl_2=0;
  idx_lst=0;

  /* Store list of common objects */
  (*cmn_lst)=(nco_cmn_t *)nco_malloc((nbr_tbl_1+nbr_tbl_2)*sizeof(nco_cmn_t));

  /* Iterate lists */
  while(flg_more_names_exist){
    trv_sct trv_1=trv_tbl_1->lst[idx_tbl_1];
    trv_sct trv_2=trv_tbl_2->lst[idx_tbl_2];

    /* Criteria is string compare */
    nco_cmp=strcmp(trv_1.nm_fll,trv_2.nm_fll);

    /* Names match: store flag, define or write in output file, then read next name from lists */
    if(!nco_cmp){
      (*cmn_lst)[idx_lst].flg_in_fl[0]=True;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=True;
      (*cmn_lst)[idx_lst].nm=strdup(trv_1.nm_fll);
      idx_lst++;

      idx_tbl_1++;
      idx_tbl_2++;
    }else if(nco_cmp < 0){
      /* Name(1) is less than Name(2), read next name from List 1  */

      (*cmn_lst)[idx_lst].flg_in_fl[0]=True;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=False;
      (*cmn_lst)[idx_lst].nm=strdup(trv_1.nm_fll);
      idx_lst++;

      if(nco_dbg_lvl_get() == nco_dbg_old)(void)fprintf(stdout,"%s: INFO %s reports tbl_1[%d]:%s\n",nco_prg_nm_get(),fnc_nm,idx_tbl_1,trv_1.nm_fll);
      idx_tbl_1++;
    }else{
      /* Name(1) is greater than Name(2), read next name from List 2 */
      (*cmn_lst)[idx_lst].flg_in_fl[0]=False;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=True;
      (*cmn_lst)[idx_lst].nm=strdup(trv_2.nm_fll);
      idx_lst++;

      if(nco_dbg_lvl_get() == nco_dbg_old)(void)fprintf(stdout,"%s: INFO %s reports tbl_2[%d]:%s\n",nco_prg_nm_get(),fnc_nm,idx_tbl_2,trv_2.nm_fll);
      idx_tbl_2++;
    } /* end nco_cmp */

    flg_more_names_exist = (idx_tbl_1 < nbr_tbl_1 && idx_tbl_2 < nbr_tbl_2) ? True : False;

  } /* end while */

  /* List1 did not end */
  if(idx_tbl_1 < nbr_tbl_1){

    while(idx_tbl_1 < nbr_tbl_1){
      (*cmn_lst)[idx_lst].flg_in_fl[0]=True;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=False;
      (*cmn_lst)[idx_lst].nm=strdup(trv_tbl_1->lst[idx_tbl_1].nm_fll);
      idx_lst++;

      if(nco_dbg_lvl_get() == nco_dbg_old) (void)fprintf(stdout,"%s: INFO %s reports tbl_1[%d]:%s\n",nco_prg_nm_get(),fnc_nm,idx_tbl_1,trv_tbl_1->lst[idx_tbl_1].nm_fll);
      idx_tbl_1++;
    } /* end while */
  } /* end if */

  /* List2 did not end */
  if(idx_tbl_2 < nbr_tbl_2){
    while(idx_tbl_2 < nbr_tbl_2){
      (*cmn_lst)[idx_lst].flg_in_fl[0]=False;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=True;
      (*cmn_lst)[idx_lst].nm=strdup(trv_tbl_2->lst[idx_tbl_2].nm_fll);
      idx_lst++;

      if(nco_dbg_lvl_get() == nco_dbg_old)(void)fprintf(stdout,"%s: INFO %s reports tbl_2[%d]:%s\n",nco_prg_nm_get(),fnc_nm,idx_tbl_2,trv_tbl_2->lst[idx_tbl_2].nm_fll);
      idx_tbl_2++;
    } /* end while */
  } /* end if */

  /* Print list */
  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)trv_tbl_cmn_nm_prt(*cmn_lst,idx_lst);

  /* Export number of entries */
  *nbr_cmn_nm=idx_lst;
} /* trv_tbl_mch() */

void                          
trv_tbl_cmn_nm_prt                         /* [fnc] Print list of common objects (same absolute path) */
(const nco_cmn_t * const cmn_lst,      /* I [sct] List of common names */
 const int nbr_cmn_nm)                 /* I [nbr] Number of common names entries */
{
  (void)fprintf(stdout,"%s: INFO reports common objects for both files (same absolute path)\n",nco_prg_nm_get());
  (void)fprintf(stdout,"file1     file2\n");
  (void)fprintf(stdout,"---------------------------------------\n");
  for(int idx=0;idx<nbr_cmn_nm;idx++){
    char c1, c2;
    c1 = (cmn_lst[idx].flg_in_fl[0]) ? 'x' : ' ';
    c2 = (cmn_lst[idx].flg_in_fl[1]) ? 'x' : ' ';
    (void)fprintf(stdout,"%5c %6c    %-15s\n", c1, c2, cmn_lst[idx].nm);
  } /* end loop over idx */
  (void)fprintf(stdout,"\n");

} /* trv_tbl_cmn_nm_prt() */

int                                    /* O [nbr] Number of depth 1 groups (root = 0) */             
trv_tbl_inq_dpt                        /* [fnc] Return number of depth 1 groups */
(const trv_tbl_sct * const trv_tbl)    /* I [sct] GTT (Group Traversal Table) */           
{
  int nbr_grp_dpt=0; /* [nbr] Number of depth 1 groups (root = 0) */       

  /* Loop table */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_grp && trv_tbl->lst[idx_tbl].grp_dpt == 1)
      nbr_grp_dpt++;

  return nbr_grp_dpt;
} /* trv_tbl_inq_dpt() */

dmn_trv_sct *                         /* O [sct] GTT dimension structure (stored in *groups*) */
nco_dmn_trv_sct                       /* [fnc] Return unique dimension object from unique ID */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] GTT (Group Traversal Table) */
{
  /* Search table dimension list and compare IDs */
  for(unsigned int idx_dmn=0;idx_dmn<trv_tbl->nbr_dmn;idx_dmn++)
    if(dmn_id == trv_tbl->lst_dmn[idx_dmn].dmn_id)
      return &trv_tbl->lst_dmn[idx_dmn];

  assert(0);
  return NULL;
} /* nco_dmn_trv_sct() */

void                                  
nco_dmn_set_msa                       /* [fnc] Update dimension with hyperslabbed size */
(const int dmn_id,                    /* I [id] Unique dimension ID */
 const long dmn_cnt,                  /* I [nbr] New dimension size */
 trv_tbl_sct * const trv_tbl)         /* I/O [sct] GTT (Group Traversal Table) */
{
  /* Search table dimension list and compared IDs */
  for(unsigned int dmn_idx=0;dmn_idx<trv_tbl->nbr_dmn;dmn_idx++)
    if(dmn_id == trv_tbl->lst_dmn[dmn_idx].dmn_id)
      trv_tbl->lst_dmn[dmn_idx].lmt_msa.dmn_cnt=dmn_cnt;

  return;
} /* nco_dmn_set_msa() */

void
nco_trv_hsh_del /* Delete hash table */
(trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  /* Purpose: Delete hash table
     http://troydhanson.github.io/uthash/userguide.html#_delete_item */
  trv_sct *trv_obj; /* [sct] GTT object structure */
  trv_sct *hsh_tmp; /* [sct] GTT object structure */

  /* Iterate over hash table */
  HASH_ITER(hh,trv_tbl->hsh,trv_obj,hsh_tmp){
    /* Delete current object */
    HASH_DEL(trv_tbl->hsh,trv_obj);
  } /* end iteration over hash table */  

} /* nco_trv_hsh_del() */

void
nco_trv_hsh_bld /* Hash traversal table for fastest access */
(trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  /* Purpose: Hash all objects in traversal table and store resultant hash table */

  /* Notes on Hash vs. Other methods:
     Usually we wish to find and return object with same full name
     Full names are guaranteed to unique in netCDF4/HDF5 files
     The two practical methods are:
     
     1. Bruit-force search with loop/strcmp(): (old method)
     No initial setup necessary
     Loop through traversal table, use strcmp() to find matching name
     This method works if traversal table is re-ordered or names change after keys are generated
     Time-expense of bruit-force searches is O(n)
     This method is therefore more robust yet much slower than hashes
     2. Hash table: (new method)
     Add each object to hash table on creation by calling nco_trv_hsh_bld() from nco_bld_trv_tbl()
     This generates hash table as string-keyed hash with object's full-name as key
     Then, as needed, pass key to hash table to retrieve desired object
     This method does not depend on traversal table remaining in same order as when keys were generated
     But it does require that no names change after keys are generated
     More specifically, it requires old hash entries be deleted and new ones added when keys change
     Time-expense of hashes is O(1+n/k)
     Hashes are therefore more fragile yet much quicker than bruit-force searches
     Care must be used to destroy/replace/re-create hash table entries if when keys change (or table is re-ordered)

     Hash table lookups:
     trv_sct *trv_obj;
     HASH_FIND_STR(trv_tbl->hsh,nm_fll,trv_obj);
     return trv_obj;
     fxm: this does not work yet!!! must check that trv_obj->obj_typ=nco_obj_typ_var!!! why???
     Input key argument (nm_fll field) is unchanged
     However, HASH_FIND_STR() macro treats key as (i.e., casts it to) unsigned bytes internally
     Hence  -Wcast_qual causes compiler complaint if key argument is const char * const nm_fll
     Potential workarounds:
     1. Eliminate -Wcast_qual from compiler settings
        Verified that this does work
	Unpalatable because it weakens static checking on rest of code
	Use GNU Make Target-specific variables to restrict lax flags to files with hash prototypes, e.g.,
	# Target-specific variable values syntax TARGET ... : VARIABLE-ASSIGNMENT
	# Rules begin in leftmost column else interpreted as commands
	ifneq (${null},$(findstring ${PVM_ARCH},LINUXALPHALINUXAMD64LINUXARMFREEBSD))
	ifeq (gcc,$(firstword ${CC}))
	${MY_OBJ_DIR}/nco_grp_trv.o : CFLAGS := $(filter-out -Wcast-qual,${CFLAGS})
	${MY_OBJ_DIR}/nco_grp_trv.o : CXXFLAGS := $(filter-out -Wcast-qual,${CXXFLAGS})
	endif # endif GCC
	endif # endif LINUX
     2. Change function prototype to non-const key
        This eliminates compiler warnings at cost of introducing non-typesafe code paths
	Move all hash-lookup functions into single file that does not get -Wcast_qual?
     3. Pass key as void pointer?
        Not sure this is doable/would work
     4. Duplicate then free() key for each hash-lookup
        Verified this works yet it seems too expensive, e.g., 
        char *hsh_key=(char *)strdup(nm_fll);
        HASH_FIND_STR(trv_tbl->hsh,hsh_key,trv_obj);
        if(hsh_key) hsh_key=(char *)nco_free(hsh_key); */

  /* NB: Hash table MUST be NULL-initialized */
  trv_tbl->hsh=NULL;

  for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){
    /* Key is full object name but do not use nm_fll itself as key
       That would be unsafe because of dangling pointer to nm_fll
       Instead duplicate nm_fll into its own hsh_key field
       Release this hsh_key memory in trv_tbl_free() */
    trv_tbl->lst[tbl_idx].hsh_key=strdup(trv_tbl->lst[tbl_idx].nm_fll);
    /* General macro faster than convenience macro since can use GTT-supplied string length */
    HASH_ADD_KEYPTR(hh,trv_tbl->hsh,trv_tbl->lst[tbl_idx].hsh_key,trv_tbl->lst[tbl_idx].nm_fll_lng,trv_tbl->lst+tbl_idx);
    /* Convenience macro (more expensive because UTHASH must compute key-length itself?) */
    //    HASH_ADD_PTR(trv_tbl->hsh,trv_tbl->lst[tbl_idx].hsh_key,trv_tbl->lst+tbl_idx);
  } /* end loop over trv_tbl */

} /* end trv_tbl_hsh() */

void 
nco_nm_srt                             /* [fnc] Sort traversal table */
(char **nm_lst,                        /* I [sng] List of names */
 const int nm_lst_nbr)                 /* I [nbr] Number of items in list */
{
  /* Purpose: Sort list of strings  */
  qsort(nm_lst,(size_t)nm_lst_nbr,sizeof(char **),nco_cmp_sng);
} /* end nco_nm_srt() */

void                          
nco_nm_mch                             /* [fnc] Match 2 lists of strings and mark common strings  */
(char **nm_lst_1,                      /* I [sng] List of names */
 const int nm_lst_1_nbr,               /* I [nbr] Number of items in list */
 char **nm_lst_2,                      /* I [sng] List of names */
 const int nm_lst_2_nbr,               /* I [nbr] Number of items in list */
 nco_cmn_t **cmn_lst,                  /* I/O [sct] List of all names */
 int * nbr_nm,                         /* I/O [nbr] Number of all names (size of above array) */
 int * nbr_cmn_nm)                     /* I/O [nbr] Number of common names */
{
  /* Purpose: Match 2 lists of strings and export common strings. 
     Use cosequential match algorithm described in
     Folk, Michael; Zoellick, Bill. (1992). File Structures. Addison-Wesley.

  Compare 2 ordered lists of names:
  If Name(1) is less than Name(2), read next name from List 1; this is done by incrementing current index
  If Name(1) is greater than Name(2), read next name from List 2
  If names are identical, read next names from both lists */

  int idx_lst;                   /* [idx] Current position in common List */ 
  int idx_tbl_1;                 /* [idx] Current position in List 1 */ 
  int idx_tbl_2;                 /* [idx] Current position in List 2 */ 
  int nbr_tbl_1;                 /* [nbr] Number of items in list 1 */
  int nbr_tbl_2;                 /* [nbr] Number of items in list 2 */
  int nco_cmp;                   /* [nbr] Return value of strcmp() */            

  nco_bool flg_more_names_exist; /* [flg] Are there more names to process? */

  /* Names must be sorted */
  (void)nco_nm_srt(nm_lst_1,nm_lst_1_nbr);
  (void)nco_nm_srt(nm_lst_2,nm_lst_2_nbr);

  /* Get number of objects in each table */
  nbr_tbl_1=nm_lst_1_nbr;
  nbr_tbl_2=nm_lst_2_nbr;

  /* If both lists have names, then there are names to process */
  flg_more_names_exist = (nbr_tbl_1 > 0 && nbr_tbl_2 > 0) ? True : False;

  /* Store list of common objects */
  (*cmn_lst)=(nco_cmn_t *)nco_malloc((nbr_tbl_1+nbr_tbl_2)*sizeof(nco_cmn_t));

  /* Initialize counters and output */
  *nbr_cmn_nm=0;
  *nbr_nm=0;
  idx_tbl_1=0;
  idx_tbl_2=0;
  idx_lst=0;

  /* Iterate lists */
  while(flg_more_names_exist){
    char *nm_1=nm_lst_1[idx_tbl_1];
    char *nm_2=nm_lst_2[idx_tbl_2];

    /* Criteria is string compare */
    nco_cmp=strcmp(nm_1,nm_2);

    /* Names match: store flag, define or write in output file, then read next name from lists */
    if(!nco_cmp){
      (*cmn_lst)[idx_lst].flg_in_fl[0]=True;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=True;
      (*cmn_lst)[idx_lst].nm=strdup(nm_1);
      idx_lst++;
      idx_tbl_1++;
      idx_tbl_2++;
      /* Export commnon names */
      *nbr_cmn_nm=idx_lst;
    }else if(nco_cmp < 0){
      /* Name(1) is less than Name(2), read next name from List 1  */
      (*cmn_lst)[idx_lst].flg_in_fl[0]=True;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=False;
      (*cmn_lst)[idx_lst].nm=strdup(nm_1);
      idx_lst++; 
      idx_tbl_1++;
    }else{
      /* Name(1) is greater than Name(2), read next name from List 2 */
      (*cmn_lst)[idx_lst].flg_in_fl[0]=False;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=True;
      (*cmn_lst)[idx_lst].nm=strdup(nm_2);
      idx_lst++;   
      idx_tbl_2++;
    } /* end nco_cmp */

    flg_more_names_exist = (idx_tbl_1 < nbr_tbl_1 && idx_tbl_2 < nbr_tbl_2) ? True : False;

  } /* end while */

  /* List1 did not end */
  if(idx_tbl_1 < nbr_tbl_1){

    while(idx_tbl_1 < nbr_tbl_1){
      (*cmn_lst)[idx_lst].flg_in_fl[0]=True;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=False;
      (*cmn_lst)[idx_lst].nm=strdup(nm_lst_1[idx_tbl_1]);
      idx_lst++;
      idx_tbl_1++;
    } /* end while */
  } /* end if */

  /* List2 did not end */
  if(idx_tbl_2 < nbr_tbl_2){
    while(idx_tbl_2 < nbr_tbl_2){
      (*cmn_lst)[idx_lst].flg_in_fl[0]=False;
      (*cmn_lst)[idx_lst].flg_in_fl[1]=True;
      (*cmn_lst)[idx_lst].nm=strdup(nm_lst_2[idx_tbl_2]);
      idx_lst++;
      idx_tbl_2++;
    } /* end while */
  } /* end if */

  /* Export number of entries */
  *nbr_nm=idx_lst;

} /* nco_nm_mch() */

void
trv_tbl_mrk_nsm_mbr                    /* [fnc] Mark ensemble member flag in table for "var_nm_fll" */
(const char * const var_nm_fll,        /* I [sng] Variable name to find */
 const nco_bool flg_nsm_tpl,           /* I [flg] Variable is template member */
 const char * const grp_nm_fll_prn,    /* I [sng] Parent group full name (key for ensemble) */
 trv_tbl_sct * const trv_tbl)          /* I/O [sct] Traversal table */
{
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !strcmp(var_nm_fll,trv_tbl->lst[idx_tbl].nm_fll)){
      trv_tbl->lst[idx_tbl].flg_nsm_mbr=True;
      trv_tbl->lst[idx_tbl].nsm_nm=strdup(grp_nm_fll_prn);
      if(flg_nsm_tpl) trv_tbl->lst[idx_tbl].flg_nsm_tpl=True;
    }
  }

  return;
} /* end trv_tbl_mrk_nsm_mbr() */

char *                                 /* O [sng] Full path  */
nco_bld_nm_fll                         /* [fnc] Utility function to build a full path */
(const char * const grp_nm_fll,        /* I [sng] Group full name */
 const char * const var_nm)            /* I [sng] Variable name */
{
  /* Allocate path buffer and include space for trailing NUL */ 
  char *var_nm_fll=(char *)nco_malloc(strlen(grp_nm_fll)+strlen(var_nm)+2L);
  /* Initialize path with current absolute group path */
  strcpy(var_nm_fll,grp_nm_fll);
  /* If not root group, concatenate separator */
  if(strcmp(grp_nm_fll,"/")) strcat(var_nm_fll,"/");
  /* Concatenate variable to absolute group path */
  strcat(var_nm_fll,var_nm);

  return var_nm_fll;
} /* nco_bld_nm_fll() */

nco_bool                              
nco_lst_ins                            /* [fnc] Utility function to detect inserted names in a name list */
(const char * const nm,                /* I [sng] A name to detect */
 const nco_cmn_t *cmn_lst,             /* I [sct] List of names   */
 const int nbr_nm)                     /* I [nbr] Number of names (size of above array) */
{
  /* If match then mark as inserted in array */
  for(int idx_nm=0;idx_nm<nbr_nm;idx_nm++)
    if(!strcmp(cmn_lst[idx_nm].nm,nm)) return True;

  return False;
} /* nco_lst_ins() */

char *                                 /* O [sng] Full path with suffix */
nco_bld_nsm_sfx                        /* [fnc] Build ensemble suffix */
(const char * const grp_nm_fll_prn,    /* I [sng] Absolute group name of ensemble root */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] Traversal table */
{
  assert(trv_tbl->nsm_sfx);

  /* Loop table */
  for(unsigned tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){
    if(trv_tbl->lst[tbl_idx].nco_typ == nco_obj_typ_grp && !strcmp(grp_nm_fll_prn,trv_tbl->lst[tbl_idx].nm_fll)){

      /* Define (append) a new name */
      char *nm_fll_sfx=(char*)nco_malloc(strlen(grp_nm_fll_prn)+strlen(trv_tbl->lst[tbl_idx].nm)+strlen(trv_tbl->nsm_sfx)+2L);
      strcpy(nm_fll_sfx,grp_nm_fll_prn);
      strcat(nm_fll_sfx,"/");
      strcat(nm_fll_sfx,trv_tbl->lst[tbl_idx].nm);
      strcat(nm_fll_sfx,trv_tbl->nsm_sfx);
      return nm_fll_sfx;

    } /* Match */
  } /* Loop table*/

  assert(0);
  return NULL;
} /* nco_bld_nsm_sfx() */

nco_bool                              
nco_is_fll_pth                         /* [fnc] Utility function to inquire if a string is a full path */
(const char * const str)               /* I [sng] A string to inquire */
{
  const char *sbs_srt; /* [sng] Location of string match start in parameter string */

  /* If parameter string contains any slash character '/' , assume it is a full path */
  sbs_srt=strstr(str,"/");
  if(sbs_srt) return True;
  return False;
} /* nco_is_fll_pth() */

trv_sct *                             /* O [sct] Table object */
trv_tbl_var_nm                        /* [fnc] Return variable object (relative name) */
(const char * const var_nm,           /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Purpose: Return variable object with given relative name (returns 1st name found) 
     NB: Calling this function is almost certainly a mistake */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !strcmp(var_nm,trv_tbl->lst[idx_tbl].nm))
      return &trv_tbl->lst[idx_tbl];

  return NULL;
} /* trv_tbl_var_nm() */

trv_sct *                              /* O [sct] Table object */
trv_tbl_nsm_nm                         /* [fnc] Return variable object  */
(const char * const var_nm,            /* I [sng] Variable name (relative) to find */
 const char * const grp_nm_fll_prn,    /* I [sng] Ensemble parent group */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] Traversal table */
{
  /* Purpose: Return variable object that matches criteria of ensemble parent group and variable relative name */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl];  
    if(var_trv.nco_typ == nco_obj_typ_var && !strcmp(var_nm,var_trv.nm))
      if(!strcmp(grp_nm_fll_prn,var_trv.grp_nm_fll_prn))
	return &trv_tbl->lst[idx_tbl];
  }

  return NULL;
} /* trv_tbl_nsm_nm() */

trv_sct *                              /* O [sct] Table object */
trv_tbl_nsm_nm_att                     /* [fnc] Return variable object  */
(const char * const var_nm,            /* I [sng] Variable name (relative) to find */
 const char * const grp_nm_fll_prn,    /* I [sng] Ensemble parent group */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] Traversal table */
{
  /* Purpose: Return variable object that matches criteria of ensemble parent group and variable relative name
  NOTE: using "var_trv.grp_nm_fll" for match */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    trv_sct var_trv=trv_tbl->lst[idx_tbl];  
    if(var_trv.nco_typ == nco_obj_typ_var && !strcmp(var_nm,var_trv.nm))
      if(!strcmp(grp_nm_fll_prn,var_trv.grp_nm_fll))
        return &trv_tbl->lst[idx_tbl];
  }

  return NULL;
} /* trv_tbl_nsm_nm_att() */
