nm_id_sct *
dmn_lst_ass_var(int nc_id,nm_id_sct *var,int nbr_var,int *nbr_dim)
/* 
   int nc_id: I netCDF input-file ID
   nm_id_sct *var: I variable list
   int nbr_var: I number of variables in list
   int *nbr_dim: O number of dimensions associated with input variable list
   nm_id_sct *dmn_lst_ass_var(): O list of dimensions associated with input variable list
 */
{
  /* Purpose: Create a list of all dimensions associated with input variable list */

  bool dmn_has_been_placed_on_list;

  char dmn_nm[NC_MAX_NAME];

  int dmn_id[NC_MAX_DIMS];
  int idx_dmn_in;
  int idx_var;
  int idx_var_dim;
  int idx_dmn_lst;
  int nbr_dmn_in;
  int nbr_var_dim;
  
  nm_id_sct *dim;

  *nbr_dim=0;

  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dmn_in,(int *)NULL,(int *)NULL,(int *)NULL);

  /* Number of input dimensions is upper bound on number of output dimensions */
  dim=(nm_id_sct *)nco_malloc(nbr_dmn_in*sizeof(nm_id_sct));
  
  /* ...For each dimension in file... */
  for(idx_dmn_in=0;idx_dmn_in<nbr_dmn_in;idx_dmn_in++){
    /* ...begin search for dimension in dimension list by... */
    dmn_has_been_placed_on_list=False;
    /* ...looking through the set of output variables... */
    for(idx_var=0;idx_var<nbr_var;idx_var++){
      /* ...and searching each dimension of each output variable... */
      (void)nco_inq_var(nc_id,var[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dim,dmn_id,(int *)NULL);
      for(idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){
	/* ...until output variable is found which contains input dimension... */
	if(idx_dmn_in == dmn_id[idx_var_dim]){
	  /* ...then search each member of output dimension list... */
	  for(idx_dmn_lst=0;idx_dmn_lst<*nbr_dim;idx_dmn_lst++){
	    /* ...until input dimension is found... */
	    if(idx_dmn_in == dim[idx_dmn_lst].id) break; /* ...then search no further... */
	  } /* end loop over idx_dmn_lst */
	  /* ...and if dimension was not found on output dimension list... */
	  if(idx_dmn_lst == *nbr_dim){
	    /* ...then add dimension to output dimension list... */
	    (void)nco_inq_dimname(nc_id,idx_dmn_in,dmn_nm);
	    dim[*nbr_dim].id=idx_dmn_in;
	    dim[*nbr_dim].nm=(char *)strdup(dmn_nm);
	    (*nbr_dim)++;
	  } /* end if dimension was not found in current output dimension list */
	  /* ...call off the dogs for this input dimension... */
	  dmn_has_been_placed_on_list=True;
	} /* end if input dimension belongs to this output variable */
	if(dmn_has_been_placed_on_list) break; /* break out of idx_var_dim to idx_var */
      } /* end loop over idx_var_dim */
      if(dmn_has_been_placed_on_list) break; /* break out of idx_var to idx_dmn_in */
    } /* end loop over idx_var */
  } /* end loop over idx_dmn_in */
  
  /* We now have final list of dimensions to extract. Phew. */
  
  /* Free unused space in output dimension list */
  dim=(nm_id_sct *)nco_realloc((void *)dim,*nbr_dim*sizeof(nm_id_sct));
  
  return dim;

} /* end dmn_lst_ass_var() */

dmn_sct *
dmn_dpl(dmn_sct *dim)
/* 
   dmn_sct *dim: I dimension structure to duplicate
   dmn_sct *dmn_dpl(): O copy of input dimension structure
 */
{
  /* Purpose: nco_malloc() and return duplicate of input dmn_sct */

  dmn_sct *dmn_dpl;

  dmn_dpl=(dmn_sct *)nco_malloc(sizeof(dmn_sct));

  (void)memcpy((void *)dmn_dpl,(void *)dim,sizeof(dmn_sct));

  return dmn_dpl;

} /* end dmn_dpl() */

void
dmn_xrf(dmn_sct *dim,dmn_sct *dmn_dpl)
/*  
   dmn_sct *dim: I/O pointer to dimension structure
   dmn_sct *dim: I/O pointer to dimension structure
*/
{
  /* Make xrf elements of dimension structures point to eachother */

  dim->xrf=dmn_dpl;
  dmn_dpl->xrf=dim;

} /* end dmn_xrf() */

void
dmn_dfn(char *fl_nm,int nc_id,dmn_sct **dim,int nbr_dim)
/* 
   char *fl_nm: I name of output file
   int nc_id: I netCDF output-file ID
   dmn_sct **dim: I list of pointers to dimension structures to be defined in output file
   int nbr_dim: I number of dimension structures in structure list
*/
{
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */

  for(idx=0;idx<nbr_dim;idx++){

    /* Has dimension already been defined? */
    rcd=nco_inq_dimid_flg(nc_id,dim[idx]->nm,&dim[idx]->id);

    /* If dimension has not been defined yet, define it */
    if(rcd != NC_NOERR){
      if(dim[idx]->is_rec_dmn){
	(void)nco_def_dim(nc_id,dim[idx]->nm,NC_UNLIMITED,&(dim[idx]->id));
      }else{
	(void)nco_def_dim(nc_id,dim[idx]->nm,dim[idx]->cnt,&(dim[idx]->id));
      } /* end else */
    }else{
      (void)fprintf(stderr,"%s: WARNING dimension \"%s\" is already defined in %s\n",prg_nm_get(),dim[idx]->nm,fl_nm);
    } /* end if */
  } /* end loop over idx */
  
} /* end dmn_dfn() */

dmn_sct *
dmn_fll(int nc_id,int dmn_id,char *dmn_nm)
/* 
   int nc_id: I netCDF input-file ID
   int dmn_id: I dimension ID
   char *dmn_nm: I dimension name
   dmn_sct *dmn_fll(): pointer to output dimension structure
 */
{
  /* Routine to nco_malloc() and return a completed dmn_sct */

  dmn_sct *dim;
  
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rec_dmn_id;
  
  dim=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
  
  dim->nm=dmn_nm;
  dim->id=dmn_id;
  dim->nc_id=nc_id;
  dim->xrf=NULL;
  dim->val.vp=NULL;

  dim->is_crd_dmn=False;
  (void)nco_inq_dimlen(dim->nc_id,dmn_id,&dim->sz);
  
  /* Get the record dimension ID */
  (void)nco_inq(dim->nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  if(dim->id == rec_dmn_id){
    dim->is_rec_dmn=True;
  }else{
    dim->is_rec_dmn=False;
  } /* end if */
   
  rcd=nco_inq_varid_flg(dim->nc_id,dmn_nm,&dim->cid);
  if(rcd == NC_NOERR){
    dim->is_crd_dmn=True;
    /* What type is coordinate? */
    (void)nco_inq_vartype(dim->nc_id,dim->cid,&dim->type);
  } /* end if */
  
  dim->cnt=dim->sz;
  dim->srt=0L;
  dim->end=dim->sz-1L;
  dim->srd=1L;
  
  return dim;
} /* end dmn_fll() */

void
dmn_lmt_mrg(dmn_sct **dim,int nbr_dim,lmt_sct *lmt,int lmt_nbr)
/* 
   dmn_sct **dim: I list of pointers to dimension structures
   int nbr_dim: I number of dimension structures in structure list
   lmt_sct *lmt: I structure from lmt_evl() holding dimension limit info.
   int lmt_nbr: I number of dimensions with user-specified limits
 */
{
  /* Routine to merge the limit structure information into dimension structures */

  int idx;
  int lmt_idx;

  for(idx=0;idx<nbr_dim;idx++){

    /* Decide whether this dimension has any user-specified limits */
    for(lmt_idx=0;lmt_idx<lmt_nbr;lmt_idx++){
      if(lmt[lmt_idx].id == dim[idx]->id){
	dim[idx]->cnt=lmt[lmt_idx].cnt;
	dim[idx]->srt=lmt[lmt_idx].srt;
	dim[idx]->end=lmt[lmt_idx].end;
	dim[idx]->srd=lmt[lmt_idx].srd;
	break;
      } /* end if */
    } /* end loop over lmt_idx */
  } /* end loop over dim */
} /* end dmn_lmt_mrg() */

nm_id_sct *
dmn_lst_mk(int nc_id,char **dmn_lst_in,int nbr_dim)
/* 
   int nc_id: I netCDF file ID
   char **dmn_lst_in: user specified list of dimension names
   int nbr_dim: I total number of dimensions in lst
   nm_id_sct dmn_lst_mk(): O dimension list
 */
{
  /* Purpose:  */
  int idx;

  nm_id_sct *dmn_lst;
  
  dmn_lst=(nm_id_sct *)nco_malloc(nbr_dim*sizeof(nm_id_sct));
  for(idx=0;idx<nbr_dim;idx++){
    /* See if requested dimension is in input file */
    dmn_lst[idx].nm=dmn_lst_in[idx];
    (void)nco_inq_dimid(nc_id,dmn_lst[idx].nm,&dmn_lst[idx].id);
  } /* end loop over idx */
  
  return dmn_lst;
} /* end dmn_lst_mk() */

