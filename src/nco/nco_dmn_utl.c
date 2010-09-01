/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_dmn_utl.c,v 1.39 2010-09-01 11:15:25 hmb Exp $ */

/* Purpose: Dimension utilities */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_dmn_utl.h" /* Dimension utilities */

void
nco_dmn_dfn /* [fnc] Define dimensions in output file */
(const char * const fl_nm, /* I [sng] Name of output file */
 const int nc_id, /* I [id] netCDF output file ID */
 dmn_sct ** const dmn, /* I [sct] Dimension structures to define in output file */
 const int nbr_dmn) /* I [nbr] Number of dimension structures in structure list */
{
  /* Purpose: Define dimensions in output file */

  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */

  for(idx=0;idx<nbr_dmn;idx++){

    /* Has dimension already been defined? */
    rcd=nco_inq_dimid_flg(nc_id,dmn[idx]->nm,&dmn[idx]->id);

    /* If dimension has not been defined yet, define it */
    if(rcd != NC_NOERR){
      if(dmn[idx]->is_rec_dmn){
	(void)nco_def_dim(nc_id,dmn[idx]->nm,NC_UNLIMITED,&(dmn[idx]->id));
      }else{
	(void)nco_def_dim(nc_id,dmn[idx]->nm,dmn[idx]->cnt,&(dmn[idx]->id));
      } /* end else */
    }else{
      (void)fprintf(stderr,"%s: WARNING dimension \"%s\" is already defined in %s\n",prg_nm_get(),dmn[idx]->nm,fl_nm);
    } /* end if */
  } /* end loop over idx */
  
} /* end nco_dmn_dfn() */

dmn_sct * /* O [sct] Copy of input dimension structure */
nco_dmn_dpl /* [fnc] Duplicate input dimension structure */
(const dmn_sct * const dmn) /* I [sct] Dimension structure to duplicate */
{
  /* Purpose: nco_malloc() and return duplicate of input dmn_sct */
  const void *dmn_vp=dmn;

  dmn_sct *dmn_cpy;

  dmn_cpy=(dmn_sct *)nco_malloc(sizeof(dmn_sct));

  /* Shallow copy structure */
  (void)memcpy((void *)dmn_cpy,dmn_vp,sizeof(dmn_sct));

  /* Make sure dmn_free() frees names when dimension is destructed */
  if(dmn->nm) dmn_cpy->nm=(char *)strdup(dmn->nm);

  return dmn_cpy;
} /* end nco_dmn_dpl() */

void 
nco_dmn_cpy /* [fnc] Copy dim elements from dim from source to target */
(dmn_sct * const dmn_t, /* O [sct] Dimension structure to copy into */
 const dmn_sct * const dmn_s) /* I [sct] Dimension structure to copy from */
{
  if(dmn_t->nm)
    dmn_t->nm=(char*)nco_free(dmn_t->nm);

  /* shallow copy elements */
  *dmn_t=*dmn_s;   
  
  /* re-copy name */
  dmn_t->nm=strdup(dmn_s->nm);
  return;
} /* end nco_dmn_cpy() */
  

dmn_sct * /* O [sct] Output dimension structure */
nco_dmn_fll /* [fnc] Create and return completed dmn_sct */
(const int nc_id, /* I [id] netCDF input file ID */
 const int dmn_id, /* I [id] Dimension ID */
 const char * const dmn_nm) /* I [sng] Dimension name */
{
  /* Purpose: nco_malloc() and return a completed dmn_sct */

  dmn_sct *dmn;
  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id;
  
  dmn=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
  
  dmn->nm=(char *)strdup(dmn_nm);
  dmn->id=dmn_id;
  dmn->nc_id=nc_id;
  dmn->xrf=NULL;
  dmn->val.vp=NULL;

  dmn->is_crd_dmn=False;
  (void)nco_inq_dimlen(dmn->nc_id,dmn_id,&dmn->sz);
  
  /* Get record dimension ID */
  (void)nco_inq(dmn->nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  if(dmn->id == rec_dmn_id){
    dmn->is_rec_dmn=True;
  }else{
    dmn->is_rec_dmn=False;
  } /* end if */
   
  rcd=nco_inq_varid_flg(dmn->nc_id,dmn_nm,&dmn->cid);
  if(rcd == NC_NOERR){
    dmn->is_crd_dmn=True;
    /* What type is coordinate? */
    (void)nco_inq_vartype(dmn->nc_id,dmn->cid,&dmn->type);
  } /* end if */
  
  dmn->cnk_sz=0L;
  dmn->cnt=dmn->sz;
  dmn->srt=0L;
  dmn->end=dmn->sz-1L;
  dmn->srd=1L;
  
  return dmn;
} /* end nco_dmn_fll() */

dmn_sct * /* O [sct] Pointer to free'd dimension */
nco_dmn_free /* [fnc] Free all memory associated with dimension structure */
(dmn_sct *dmn) /* I/O [sct] Dimension to free */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with a dynamically allocated dimension structure */
  dmn->nm=(char *)nco_free(dmn->nm);
  /* Free structure pointer last */
  dmn=(dmn_sct *)nco_free(dmn);

  return NULL;
} /* end nco_dmn_free() */

void
nco_dmn_cnk_mrg /* [fnc] Merge chunking information into dimension structures */
(dmn_sct ** const dmn, /* I [sct] Dimension structures to modify */
 const int nbr_dmn, /* I [nbr] Number of dimension structures in structure list */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr) /* I [nbr] Number of dimensions with user-specified chunking */
{
  /* Purpose: Merge chunking information into dimension structures */
  int dmn_idx;
  int cnk_idx;

  for(dmn_idx=0;dmn_idx<nbr_dmn;dmn_idx++){
    /* Does this dimension have user-specified chunking? */
    for(cnk_idx=0;cnk_idx<cnk_nbr;cnk_idx++){
      /* Match on name not ID so nco_dmn_cnk_mrg() works on any input file */
      if(!strcmp(cnk[cnk_idx]->nm,dmn[dmn_idx]->nm)){
	dmn[dmn_idx]->cnk_sz=cnk[cnk_idx]->sz;
	break;
      } /* end if */
    } /* end loop over cnk_idx */
  } /* end loop over dmn */
} /* end nco_dmn_cnk_mrg() */

void
nco_dmn_lmt_mrg /* [fnc] Merge limit structure information into dimension structures */
(dmn_sct ** const dmn, /* I [sct] Dimension structures to modify */
 const int nbr_dmn, /* I [nbr] Number of dimension structures in structure list */
 CST_X_PTR_CST_PTR_CST_Y(lmt_sct,lmt), /* I [sct] Dimension limit information */
 const int lmt_nbr) /* I [nbr] Number of dimensions with user-specified limits */
{
  /* Purpose: Merge limit structure information into dimension structures */

  int dmn_idx;
  int lmt_idx;

  for(dmn_idx=0;dmn_idx<nbr_dmn;dmn_idx++){
    /* Does this dimension have user-specified limits? */
    for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
      /* 20050707: Match on name not ID so nco_dmn_lmt_mrg() works with single
	 limit list applied to any input file */
      if(!strcmp(lmt[lmt_idx]->nm,dmn[dmn_idx]->nm)){
	dmn[dmn_idx]->cnt=lmt[lmt_idx]->cnt;
	dmn[dmn_idx]->srt=lmt[lmt_idx]->srt;
	dmn[dmn_idx]->end=lmt[lmt_idx]->end;
	dmn[dmn_idx]->srd=lmt[lmt_idx]->srd;
	break;
      } /* end if */
    } /* end loop over lmt_idx */
  } /* end loop over dmn */
} /* end nco_dmn_lmt_mrg() */


void
nco_dmn_lmt_all_mrg /* [fnc] Merge limit structure information into dimension structures */
(dmn_sct ** const dmn, /* I [sct] Dimension structures to modify */
 const int nbr_dmn, /* I [nbr] Number of dimension structures in structure list */
 CST_X_PTR_CST_PTR_CST_Y(lmt_all_sct,lmt_all_lst), /* I [sct] Dimension limit information */
 const int lmt_nbr) /* I [nbr] Number of dimensions with user-specified limits */
{
  /* Purpose: Merge limit structure information into dimension structures */

  int dmn_idx;
  int lmt_idx;

  for(dmn_idx=0;dmn_idx<nbr_dmn;dmn_idx++){
    /* Does this dimension have user-specified limits? */
    for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
      /* 20050707: Match on name not ID so nco_dmn_lmt_mrg() works with single
	 limit list applied to any input file */
      if(!strcmp(lmt_all_lst[lmt_idx]->dmn_nm,dmn[dmn_idx]->nm)){
	dmn[dmn_idx]->cnt=lmt_all_lst[lmt_idx]->dmn_cnt;
	dmn[dmn_idx]->srt=0;
	dmn[dmn_idx]->end=lmt_all_lst[lmt_idx]->dmn_cnt-1;
	dmn[dmn_idx]->srd=1L;
	break;
      } /* end if */
    } /* end loop over lmt_idx */
  } /* end loop over dmn */
} /* end nco_dmn_lmt_all_mrg() */



nm_id_sct * /* O [sct] List of dimensions associated with input variable list */
nco_dmn_lst_ass_var /* [fnc] Create list of all dimensions associated with input variable list */
(const int nc_id, /* I [id] netCDF input-file ID */
 const nm_id_sct * const var, /* I [sct] Variable list */
 const int nbr_var, /* I [nbr] Number of variables in list */
 int * const nbr_dmn) /* O [nbr] Number of dimensions associated with input variable list */
{
  /* Purpose: Create list of all dimensions associated with input variable list */

  nco_bool dmn_has_been_placed_on_list;

  char dmn_nm[NC_MAX_NAME];

  int dmn_id[NC_MAX_DIMS];
  int idx_dmn_in;
  int idx_var;
  int idx_var_dmn;
  int idx_dmn_lst;
  int nbr_dmn_in;
  int nbr_var_dmn;
  
  nm_id_sct *dmn;

  *nbr_dmn=0;

  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dmn_in,(int *)NULL,(int *)NULL,(int *)NULL);

  /* Number of input dimensions is upper bound on number of output dimensions */
  dmn=(nm_id_sct *)nco_malloc(nbr_dmn_in*sizeof(nm_id_sct));
  
  /* ...For each dimension in file... */
  for(idx_dmn_in=0;idx_dmn_in<nbr_dmn_in;idx_dmn_in++){
    /* ...begin search for dimension in dimension list by... */
    dmn_has_been_placed_on_list=False;
    /* ...looking through the set of output variables... */
    for(idx_var=0;idx_var<nbr_var;idx_var++){
      /* ...and searching each dimension of each output variable... */
      (void)nco_inq_var(nc_id,var[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dmn,dmn_id,(int *)NULL);
      for(idx_var_dmn=0;idx_var_dmn<nbr_var_dmn;idx_var_dmn++){
	/* ...until output variable is found which contains input dimension... */
	if(idx_dmn_in == dmn_id[idx_var_dmn]){
	  /* ...then search each member of output dimension list... */
	  for(idx_dmn_lst=0;idx_dmn_lst<*nbr_dmn;idx_dmn_lst++){
	    /* ...until input dimension is found... */
	    if(idx_dmn_in == dmn[idx_dmn_lst].id) break; /* ...then search no further... */
	  } /* end loop over idx_dmn_lst */
	  /* ...and if dimension was not found on output dimension list... */
	  if(idx_dmn_lst == *nbr_dmn){
	    /* ...then add dimension to output dimension list... */
	    (void)nco_inq_dimname(nc_id,idx_dmn_in,dmn_nm);
	    dmn[*nbr_dmn].id=idx_dmn_in;
	    dmn[*nbr_dmn].nm=(char *)strdup(dmn_nm);
	    (*nbr_dmn)++;
	  } /* end if dimension was not found in current output dimension list */
	  /* ...call off the dogs for this input dimension... */
	  dmn_has_been_placed_on_list=True;
	} /* end if input dimension belongs to this output variable */
	if(dmn_has_been_placed_on_list) break; /* break out of idx_var_dmn to idx_var */
      } /* end loop over idx_var_dmn */
      if(dmn_has_been_placed_on_list) break; /* break out of idx_var to idx_dmn_in */
    } /* end loop over idx_var */
  } /* end loop over idx_dmn_in */
  
  /* We now have final list of dimensions to extract. Phew. */
  
  /* Free unused space in output dimension list */
  dmn=(nm_id_sct *)nco_realloc((void *)dmn,*nbr_dmn*sizeof(nm_id_sct));
  
  return dmn;
} /* end nco_dmn_lst_ass_var() */

dmn_sct ** /* O [sct] Pointer to free'd structure list */
nco_dmn_lst_free /* [fnc] Free memory associated with dimension structure list */
(dmn_sct **dmn_lst, /* I/O [sct] Dimension structure list to free */
 const int dmn_nbr) /* I [nbr] Number of dimension structures in list */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with dynamically allocated dimension structure list */
  int idx;

  for(idx=0;idx<dmn_nbr;idx++){
    dmn_lst[idx]=nco_dmn_free(dmn_lst[idx]);
  } /* end loop over idx */

  /* Free structure pointer last */
  dmn_lst=(dmn_sct **)nco_free(dmn_lst);

  return dmn_lst;
} /* end nco_dmn_lst_free() */

nm_id_sct * /* O [sct] Dimension list */
nco_dmn_lst_mk /* [fnc] Attach dimension IDs to dimension list */
(const int nc_id, /* I [id] netCDF file ID */
 CST_X_PTR_CST_PTR_CST_Y(char,dmn_lst_in), /* I [sng] User-specified list of dimension names */
 const int nbr_dmn) /* I [nbr] Total number of dimensions in list */
{
  /* Purpose: Create list of dimension name-ID structures from list of dimension name strings */
  int idx;

  nm_id_sct *dmn_lst;
  
  dmn_lst=(nm_id_sct *)nco_malloc(nbr_dmn*sizeof(nm_id_sct));
  for(idx=0;idx<nbr_dmn;idx++){
    /* Copy name and then get requested dimension ID from input file */
    dmn_lst[idx].nm=(char *)strdup(dmn_lst_in[idx]);
    (void)nco_inq_dimid(nc_id,dmn_lst[idx].nm,&dmn_lst[idx].id);
  } /* end loop over idx */
  
  return dmn_lst;
} /* end nco_dmn_lst_mk() */

void
nco_dmn_xrf  /* [fnc] Crossreference xrf elements of dimension structures */
(dmn_sct * const dmn_1, /* I/O [sct] Dimension structure */
 dmn_sct * const dmn_2) /* I/O [sct] Dimension structure */
{
  /* Purpose: Make xrf elements of dimension structures point to eachother */
  dmn_1->xrf=dmn_2;
  dmn_2->xrf=dmn_1;
} /* end nco_dmn_xrf() */



void 
nco_dmn_sct_cmp   /* [fnc] Check that dims in list 2 are a subset of list 1 and that they are the same size */
(dmn_sct ** const dim_1, /* I [sct] Dimension list 1 */
 const int nbr_dmn_1,  /* I [nbr] Number of dimension structures in structure list */
 dmn_sct **const dim_2,  /* I [sct] Dimension list 1 */
 const int nbr_dmn_2,  /* I [nbr] Number of dimension structures in structure list */
 const char* const fl_sng_1, /* I [sng] Name of first file */
 const char *fl_sng_2)       /* I [sng] Name of second file */
{

  int idx;
  int jdx;

  for(idx=0 ; idx<nbr_dmn_2 ; idx++ ){
    for(jdx=0 ; jdx<nbr_dmn_1 ; jdx++) 
       if(!strcmp(dim_2[idx]->nm,dim_1[jdx]->nm))
	     break;
		 		
	if(jdx==nbr_dmn_1) {	 	   
     (void)fprintf(stderr,"%s: ERROR dim \"%s\" in second file %s - not present in first file %s\n",prg_nm_get(),dim_2[idx]->nm,fl_sng_2,fl_sng_1);
       nco_exit(EXIT_FAILURE);
    }  
	
	if(dim_2[idx]->cnt != dim_1[jdx]->cnt){
	   if(dim_1[jdx]->is_rec_dmn)    
	     (void)fprintf(stderr,"%s: ERROR record dimension mismatch : record dim file in %s is %li : record dim file in %s is %li\n",prg_nm_get(),fl_sng_1,dim_1[jdx]->cnt, fl_sng_2,dim_2[idx]->cnt);     
	   else	      
		 (void)fprintf(stderr,"%s: ERROR dimension mismatch :  dim \"%s\" in file %s is %li : in file  %s it is %li\n",prg_nm_get(),dim_1[jdx]->nm, fl_sng_1,dim_1[jdx]->cnt, fl_sng_2,dim_2[idx]->cnt);
		nco_exit(EXIT_FAILURE); 
	}	
				

  } 
 

} /* end nco_dmn_sct_cmp */



