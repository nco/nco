void
var_srt_zero(var_sct **var,int nbr_var)
/* 
   var_sct **var: I list of pointers to variable structures whose srt elements will be zeroed
   int nbr_var: I number of structures in variable structure list
 */
{
  /* Purpose: Point srt element of variable structure to array of zeroes */

  int idx;
  int idx_dmn;

  for(idx=0;idx<nbr_var;idx++)
    for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++)
      var[idx]->srt[idx_dmn]=0L;

} /* end var_srt_zero() */

var_sct *
var_dpl(var_sct *var)
/* 
   var_sct *var: I variable structure to duplicate
   var_sct *var_dpl(): O copy of input variable structure
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: nco_malloc() and return duplicate of input var_sct
     Duplicate is deep copy of original so original may always be free()'d */

  var_sct *var_dpl;

  var_dpl=(var_sct *)nco_malloc(sizeof(var_sct));

  /* Shallow copy structure */
  (void)memcpy((void *)var_dpl,(void *)var,sizeof(var_sct));

  /* Deep copy dyamically allocated arrays currently defined in original */
  if(var->val.vp != NULL){
    if((var_dpl->val.vp=(void *)malloc(var_dpl->sz*nco_typ_lng(var_dpl->type))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes for value buffer for variable %s in var_dpl()\n",prg_nm_get(),var_dpl->sz,nco_typ_lng(var_dpl->type),var_dpl->nm);
      exit(EXIT_FAILURE); 
    } /* end if */
    (void)memcpy((void *)(var_dpl->val.vp),(void *)(var->val.vp),var_dpl->sz*nco_typ_lng(var_dpl->type));
  } /* end if */
  if(var->mss_val.vp != NULL){
    var_dpl->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_dpl->type));
    (void)memcpy((void *)(var_dpl->mss_val.vp),(void *)(var->mss_val.vp),nco_typ_lng(var_dpl->type));
  } /* end if */
  if(var->tally != NULL){
    if((var_dpl->tally=(long *)malloc(var_dpl->sz*sizeof(long))) == NULL){
      (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%ld bytes for tally buffer for variable %s in var_dpl()\n",prg_nm_get(),var_dpl->sz,(long)sizeof(long),var_dpl->nm);
      exit(EXIT_FAILURE); 
    } /* end if */
    (void)memcpy((void *)(var_dpl->tally),(void *)(var->tally),var_dpl->sz*sizeof(long));
  } /* end if */
  if(var->dim != NULL){
    var_dpl->dim=(dmn_sct **)nco_malloc(var_dpl->nbr_dim*sizeof(dmn_sct *));
    (void)memcpy((void *)(var_dpl->dim),(void *)(var->dim),var_dpl->nbr_dim*sizeof(var->dim[0]));
  } /* end if */
  if(var->dmn_id != NULL){
    var_dpl->dmn_id=(int *)nco_malloc(var_dpl->nbr_dim*sizeof(int));
    (void)memcpy((void *)(var_dpl->dmn_id),(void *)(var->dmn_id),var_dpl->nbr_dim*sizeof(var->dmn_id[0]));
  } /* end if */
  if(var->cnt != NULL){
    var_dpl->cnt=(long *)nco_malloc(var_dpl->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dpl->cnt),(void *)(var->cnt),var_dpl->nbr_dim*sizeof(var->cnt[0]));
  } /* end if */
  if(var->srt != NULL){
    var_dpl->srt=(long *)nco_malloc(var_dpl->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dpl->srt),(void *)(var->srt),var_dpl->nbr_dim*sizeof(var->srt[0]));
  } /* end if */
  if(var->end != NULL){
    var_dpl->end=(long *)nco_malloc(var_dpl->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dpl->end),(void *)(var->end),var_dpl->nbr_dim*sizeof(var->end[0]));
  } /* end if */
  if(var->srd != NULL){
    var_dpl->srd=(long *)nco_malloc(var_dpl->nbr_dim*sizeof(long));
    (void)memcpy((void *)(var_dpl->srd),(void *)(var->srd),var_dpl->nbr_dim*sizeof(var->srd[0]));
  } /* end if */
  if(var->scl_fct.vp != NULL){
    var_dpl->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var_dpl->typ_upk));
    (void)memcpy((void *)(var_dpl->scl_fct.vp),(void *)(var->scl_fct.vp),nco_typ_lng(var_dpl->typ_upk));
  } /* end if */
  if(var->add_fst.vp != NULL){
    var_dpl->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var_dpl->typ_upk));
    (void)memcpy((void *)(var_dpl->add_fst.vp),(void *)(var->add_fst.vp),nco_typ_lng(var_dpl->typ_upk));
  } /* end if */

  return var_dpl;

} /* end var_dpl() */

void
var_get(int nc_id,var_sct *var)
/* 
   int nc_id: I netCDF file ID
   var_sct *var: I pointer to variable structure
 */
{
  /* Threads: Routine contains thread-unsafe calls protected by critical regions */
  /* Purpose: Allocate and retrieve given variable hyperslab from disk into memory
     If variable is packed on disk then inquire about scale_factor and add_offset */

  if((var->val.vp=(void *)malloc(var->sz*nco_typ_lng(var->typ_dsk))) == NULL){
    (void)fprintf(stdout,"%s: ERROR Unable to malloc() %ld*%d bytes in var_get()\n",prg_nm_get(),var->sz,nco_typ_lng(var->type));
    exit(EXIT_FAILURE); 
  } /* end if */

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
  { /* begin OpenMP critical */
    if(var->sz > 1){
      (void)nco_get_vara(nc_id,var->id,var->srt,var->cnt,var->val.vp,var->typ_dsk);
    }else{
      (void)nco_get_var1(nc_id,var->id,var->srt,var->val.vp,var->typ_dsk);
    } /* end else */
  } /* end OpenMP critical */

  /* Packing properties of variable obtained from pck_dsk_inq() call in var_fll() */

  /* Type in memory is now same as type on disk */
  var->type=var->typ_dsk; /* Type of variable in RAM */
  
  /* Packing/Unpacking */
  if(is_arithmetic_operator(prg_get())){
    /* fxm: Automatic unpacking is not debugged, just do it with ncap for now */
    if(prg_get() == ncap && dbg_lvl_get() != 3){
#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
      if(var->pck_dsk) var=var_upk(var);
    } /* endif debug */
  } /* endif arithmetic operator */

} /* end var_get() */

void
var_dmn_xrf(var_sct *var)
/*  
   var_sct *var: I pointer to variable structure
*/
{
  /* Purpose: Switch pointers to dimension structures so var->dim points to var->dim->xrf.
     Routine makes dim element of variable structure from var_dpl() refer to counterparts
     of dimensions directly associated with variable it was duplicated from */
  
  int idx;
  
  for(idx=0;idx<var->nbr_dim;idx++) var->dim[idx]=var->dim[idx]->xrf;
  
} /* end var_dmn_xrf() */

void
var_xrf(var_sct *var,var_sct *var_dpl)
/*  
   var_sct *var: I/O pointer to variable structure
   var_sct *var_dpl: I/O pointer to variable structure
*/
{
  /* Make xrf elements of variable structures point to eachother */

  var->xrf=var_dpl;
  var_dpl->xrf=var;

} /* end var_xrf() */

var_sct *
var_free(var_sct *var)
/*  
   var_sct *var: I pointer to variable structure
*/
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Free all memory associated with a dynamically allocated variable structure */
  
  /* NB: var->nm is not freed because I decided to let names be static memory, and refer to
     the optarg list if available. This assumption needs to be changed before freeing 
     the name pointer. */
  
  var->val.vp=nco_free(var->val.vp);
  var->mss_val.vp=nco_free(var->mss_val.vp);
  var->tally=nco_free(var->tally);
  var->dmn_id=nco_free(var->dmn_id);
  var->dim=nco_free(var->dim);
  var->srt=nco_free(var->srt);
  var->end=nco_free(var->end);
  var->cnt=nco_free(var->cnt);
  var->srd=nco_free(var->srd);
  var->scl_fct.vp=nco_free(var->scl_fct.vp);
  var->add_fst.vp=nco_free(var->add_fst.vp);

  /* Free structure pointer only after all dynamic elements of structure have been freed */
  var=nco_free(var);

  return NULL;

} /* end var_free */

int /* [enm] Return code */
var_dfl_set /* [fnc] Set defaults for each member of variable structure */
(var_sct *var){ /* [sct] Pointer to variable strucutre to initialize to defaults */
  /* Purpose: Set defaults for each member of variable structure
     var_dfl_set() should be called by any routine that creates a variable structure */

  int rcd=0; /* [enm] Return code */

  /* Set defaults to be overridden by available information */
  var->nm=NULL;
  var->id=-1;
  var->nc_id=-1;
  var->type=NC_NAT; /* Type of variable in RAM */ /* fxm: should use nc_type enum */
  var->typ_dsk=NC_NAT; /* Type of variable on disk */ /* fxm: should use nc_type enum */
  var->typ_pck=NC_NAT; /* Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
  var->typ_upk=NC_NAT; /* Type of variable when unpacked (expanded) (in memory) */
  var->is_rec_var=False;
  var->is_crd_var=False;
  /* Size of 1 is assumed in var_fll() */
  var->sz=1L;
  var->sz_rec=1L;
  var->cid=-1;
  var->has_mss_val=False;
  var->mss_val.vp=NULL;
  var->val.vp=NULL;
  var->tally=NULL;
  var->xrf=NULL;
  var->nbr_dim=-1;
  var->nbr_att=-1;
  var->dim=(dmn_sct **)NULL;
  var->dmn_id=(int *)NULL;
  var->cnt=(long *)NULL;
  var->srt=(long *)NULL;
  var->end=(long *)NULL;
  var->srd=(long *)NULL;

  /* Members related to packing */
  var->has_scl_fct=False; /* [flg] Valid scale_factor attribute exists */
  var->has_add_fst=False; /* [flg] Valid add_offset attribute exists */
  var->pck_dsk=False; /* [flg] Variable is packed on disk */
  var->pck_ram=False; /* [flg] Variable is packed in memory */
  var->scl_fct.vp=NULL; /* [ptr] Value of scale_factor attribute, if any */
  var->add_fst.vp=NULL; /* [ptr] Value of add_offset attribute, if any */

  return rcd; /* [enm] Return code */
} /* end var_dfl_set() */

void 
var_copy(nc_type var_typ,long sz,ptr_unn op1,ptr_unn op2)
{
  /* Purpose: Copy hyperslab variables of type var_typ from op1 to op2
     Assumes memory area in op2 has already been malloc()'d */
  (void)memcpy((void *)(op2.vp),(void *)(op1.vp),sz*nco_typ_lng(var_typ));
} /* end var_copy() */

void
var_dfn(int in_id,char *fl_out,int out_id,var_sct **var,int nbr_var,dmn_sct **dmn_ncl,int nbr_dmn_ncl)
/* 
   int in_id: I netCDF input-file ID
   char *fl_out: I name of output file
   int out_id: I netCDF output-file ID
   var_sct **var: I list of pointers to variable structures to be defined in output file
   int nbr_var: I number of variable structures in structure list
   dmn_sct **dmn_ncl: I list of pointers to dimension structures allowed in output file
   int nbr_dmn_ncl: I number of dimension structures in structure list
*/
{
  /* Define variables in output file, and copy their attributes */

  /* This function is unusual (for me) in that dimension arguments are only intended
     to be used by certain programs, those that alter the rank of input variables. If a
     program does not alter the rank (dimensionality) of input variables then it should
     call this function with a NULL dimension list. Otherwise, this routine attempts
     to define variable correctly in output file (allowing variable to be
     defined with only those dimensions that are in dimension inclusion list) 
     without altering variable structures. 

     The other unusual thing about this function is that it is intended to be called with var_prc_out
     So the local variable var usually refers to var_prc_out in the calling function 
     That is why many of the names look reversed in this function, and why xrf is frequently used */

  int idx_dmn;
  int dmn_id_vec[NC_MAX_DIMS];
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  for(idx=0;idx<nbr_var;idx++){

    /* Is requested variable already in output file? */
    rcd=nco_inq_varid_flg(out_id,var[idx]->nm,&var[idx]->id);

    /* If variable has not been defined, define it */
    if(rcd != NC_NOERR){
      
      /* TODO #116: There is a problem here in that var_out[idx]->nbr_dim is never explicitly set to the actual number of output dimensions, rather, it is simply copied from var[idx]. When var_out[idx] actually has 0 dimensions, the loop executes once anyway, and an erroneous index into the dmn_out[idx] array is attempted. Fix is to explicitly define var_out[idx]->nbr_dim. Until this is done, anything in ncwa that explicitly depends on var_out[idx]->nbr_dim is suspect. The real problem is that, in ncwa, var_avg() expects var_out[idx]->nbr_dim to contain the input, rather than output, number of dimensions. The routine, var_dfn() was designed to call the simple branch when dmn_ncl == 0, i.e., for operators besides ncwa. However, when ncwa averages all dimensions in output file, nbr_dmn_ncl == 0 so the wrong branch would get called unless we specifically use this branch whenever ncwa is calling. */
      if(dmn_ncl != NULL || prg_get() == ncwa){
	int nbr_var_dim=0;
	int idx_ncl;

	/* Rank of output variable may have to be reduced */
	for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	  /* Is dimension allowed in output file? */
	  for(idx_ncl=0;idx_ncl<nbr_dmn_ncl;idx_ncl++){
	    if(var[idx]->xrf->dim[idx_dmn]->id == dmn_ncl[idx_ncl]->xrf->id) break;
	  } /* end loop over idx_ncl */
	  if(idx_ncl != nbr_dmn_ncl) dmn_id_vec[nbr_var_dim++]=var[idx]->dim[idx_dmn]->id;
	} /* end loop over idx_dmn */
	(void)nco_def_var(out_id,var[idx]->nm,var[idx]->type,nbr_var_dim,dmn_id_vec,&var[idx]->id);

      }else{ /* Straightforward definition */
	for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	  dmn_id_vec[idx_dmn]=var[idx]->dim[idx_dmn]->id;
	} /* end loop over idx_dmn */
	(void)nco_def_var(out_id,var[idx]->nm,var[idx]->type,var[idx]->nbr_dim,dmn_id_vec,&var[idx]->id);
      } /* end else */
      
      /* endif if variable had not yet been defined in output file */
    }else{
      /* Variable is already in output file---use existing definition
	 This branch is executed, e.g., by operators in append mode */
      (void)fprintf(stderr,"%s: WARNING Using existing definition of variable \"%s\" in %s\n",prg_nm_get(),var[idx]->nm,fl_out);
    } /* end if */

    /* Always copy all attributes of a variable except in cases where packing/unpacking is involved

       0. Variable is unpacked on input, unpacked on output
       --> Copy all attributes
       1. Variable is packed on input, is not altered, and remains packed on output
       --> Copy all attributes
       2. Variable is packed on input, is unpacked for some reason, and will be unpacked on output
       --> Copy all attributes except scale_factor and add_offset
       3. Variable is packed on input, is unpacked for some reason, and will be packed on output (possibly with new packing attributes)
       --> Copy all attributes, but scale_factor and add_offset must be overwritten later with new values
       4. Variable is not packed on input, packing is performed, and output is packed
       --> Copy all attributes, define scale_factor and add_offset now, write their values later
    */

#undef FALSE
#ifdef FALSE
    /* Set nco_pck_typ based on program */
    switch(prg){
    case ncea:
      nco_pck_typ=nco_pck_all_new_att;
      break;
    case ncap:
    case ncra:
    case ncdiff:
    case ncflint:
    case ncwa:
    case ncrcat:
    case ncecat:
    default:
      nco_pck_typ=nco_pck_nil;
      break;
    } /* end switch */

    switch(nco_pck_typ){
    case nco_pck_all_xst_att:
    case nco_pck_all_new_att:
      break;
    case nco_pck_xst_xst_att:
    case nco_pck_xst_new_att:
      break;
    case nco_pck_upk:
      break;
    default:
      break;
    } /* end switch */
#endif /* not FALSE */

    /* var refers to output variable structure, var->xrf refers to input variable structure */
    (void)att_cpy(in_id,out_id,var[idx]->xrf->id,var[idx]->id);
  } /* end loop over idx */
  
} /* end var_dfn() */

void
var_val_cpy(int in_id,int out_id,var_sct **var,int nbr_var)
/* 
   int in_id: I netCDF file ID
   int out_id: I netCDF output-file ID
   var_sct **var: I list of pointers to variable structures
   int nbr_var: I number of structures in variable structure list
   var_val_cpy():
*/
{
  /* Copy variable data for every variable in the input variable structure list
     from input file to output file */

  int idx;

  for(idx=0;idx<nbr_var;idx++){
    var[idx]->xrf->val.vp=var[idx]->val.vp=(void *)nco_malloc(var[idx]->sz*nco_typ_lng(var[idx]->type));
    if(var[idx]->nbr_dim==0){
      nco_get_var1(in_id,var[idx]->id,var[idx]->srt,var[idx]->val.vp,var[idx]->type);
      nco_put_var1(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->val.vp,var[idx]->type);
    }else{ /* end if variable is a scalar */
      nco_get_vara(in_id,var[idx]->id,var[idx]->srt,var[idx]->cnt,var[idx]->val.vp,var[idx]->type);
      nco_put_vara(out_id,var[idx]->xrf->id,var[idx]->xrf->srt,var[idx]->xrf->cnt,var[idx]->xrf->val.vp,var[idx]->type);
    } /* end if variable is an array */
    var[idx]->val.vp=nco_free(var[idx]->val.vp); var[idx]->xrf->val.vp=var[idx]->val.vp=NULL;
  } /* end loop over idx */

} /* end var_val_cpy() */

nm_id_sct *
var_lst_crd_xcl(int nc_id,int dmn_id,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   int dmn_id: I dimension ID of the coordinate to eliminate from extraction list
   nm_id_sct *xtr_lst: current extraction list (destroyed)
   int *nbr_xtr: I/O number of variables in current extraction list
   nm_id_sct var_lst_crd_xcl(): O extraction list
 */
{
  /* The following code modifies extraction list to exclude the coordinate, 
     if any, associated with the given dimension ID */
  
  char crd_nm[NC_MAX_NAME];

  int idx;
  int crd_id=-1;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  /* What is variable ID of record coordinate, if any? */
  (void)nco_inq_dimname(nc_id,dmn_id,crd_nm);
   
  rcd=nco_inq_varid_flg(nc_id,crd_nm,&crd_id);
  if(rcd == NC_NOERR){
    /* Is coordinate on extraction list? */
    for(idx=0;idx<*nbr_xtr;idx++){
      if(xtr_lst[idx].id == crd_id) break;
    } /* end loop over idx */
    if(idx != *nbr_xtr){
      nm_id_sct *var_lst_tmp;
      
      var_lst_tmp=(nm_id_sct *)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
      /* Copy the extract list to the temporary extract list and reallocate the extract list */
      (void)memcpy((void *)var_lst_tmp,(void *)xtr_lst,*nbr_xtr*sizeof(nm_id_sct));
      (*nbr_xtr)--;
      xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,*nbr_xtr*sizeof(nm_id_sct));
      /* Collapse the temporary extract list into the permanent list by copying 
	 all but the coordinate. NB: the ordering of the list is conserved. */
      (void)memcpy((void *)xtr_lst,(void *)var_lst_tmp,idx*sizeof(nm_id_sct));
      (void)memcpy((void *)(xtr_lst+idx),(void *)(var_lst_tmp+idx+1),(*nbr_xtr-idx)*sizeof(nm_id_sct));
      /* Free the memory for coordinate name in the extract list before losing the pointer */
      var_lst_tmp[idx].nm=nco_free(var_lst_tmp[idx].nm);
      var_lst_tmp=nco_free(var_lst_tmp);
    } /* end if */
  } /* end if */
  
  return xtr_lst;
  
} /* end var_lst_crd_xcl() */

nm_id_sct *
var_lst_ass_crd_add(int nc_id,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   nm_id_sct *xtr_lst: I/O current extraction list (destroyed)
   int *nbr_xtr: I/O number of variables in current extraction list
   nm_id_sct var_lst_ass_crd_add(): O extraction list
 */
{
  /* Makes sure all coordinates associated with each of variables
     to be extracted is also on the list. This helps with making concise
     nco_malloc() calls down the road. */

  char dmn_nm[NC_MAX_NAME];

  int crd_id;
  int dmn_id[NC_MAX_DIMS];
  int idx_dmn;
  int idx_var_dim;
  int idx_var;
  int nbr_dim;
  int nbr_var_dim;
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dim,(int *)NULL,(int *)NULL,(int *)NULL);

  /* ...for each dimension in input file... */
  for(idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
    /* ...see if it is a coordinate dimension... */
    (void)nco_inq_dimname(nc_id,idx_dmn,dmn_nm);
     
    rcd=nco_inq_varid_flg(nc_id,dmn_nm,&crd_id);
    if(rcd == NC_NOERR){
      /* Is this coordinate already on extraction list? */
      for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	if(crd_id == xtr_lst[idx_var].id) break;
      } /* end loop over idx_var */
      if(idx_var == *nbr_xtr){
	/* ...the coordinate is not on the list, is it associated with any variables?... */
	for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	  /* Get number of dimensions and dimension IDs for variable. */
	  (void)nco_inq_var(nc_id,xtr_lst[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dim,dmn_id,(int *)NULL);
	  for(idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){
	    if(idx_dmn == dmn_id[idx_var_dim]) break;
	  } /* end loop over idx_var_dim */
	  if(idx_var_dim != nbr_var_dim){
	    /* Add the coordinate to the list */
	    xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	    xtr_lst[*nbr_xtr].nm=(char *)strdup(dmn_nm);
	    xtr_lst[*nbr_xtr].id=crd_id;
	    (*nbr_xtr)++;
	    break;
	  } /* end if */
	} /* end loop over idx_var */
      } /* end if coordinate was not already on the list */
    } /* end if dimension is a coordinate */
  } /* end loop over idx_dmn */
  
  return xtr_lst;
  
} /* end var_lst_ass_crd_add() */

var_sct *
var_fll(int nc_id,int var_id,char *var_nm,dmn_sct **dim,int nbr_dim)
/* 
   int nc_id: I netCDF file ID
   int var_id: I variable ID
   char *var_nm: I variable name
   dmn_sct **dim: I list of pointers to dimension structures
   int nbr_dim: I number of dimensions in list
   var_sct *var_fll(): O variable structure
 */
{
  /* Purpose: nco_malloc() and return a completed var_sct */
  char dmn_nm[NC_MAX_NAME];

  int dmn_idx;
  int idx;
  int rec_dmn_id;

  var_sct *var;

  /* Get record dimension ID */
  (void)nco_inq(nc_id,(int *)NULL,(int *)NULL,(int *)NULL,&rec_dmn_id);
  
  /* Allocate space for variable structure */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */

  /* Fill in known fields */
  var->nm=var_nm;
  var->id=var_id;
  var->nc_id=nc_id;

  /* Get type and number of dimensions and attributes for variable */
  (void)nco_inq_var(var->nc_id,var->id,(char *)NULL,&var->typ_dsk,&var->nbr_dim,(int *)NULL,&var->nbr_att);

  /* Allocate space for dimension information */
  if(var->nbr_dim > 0) var->dim=(dmn_sct **)nco_malloc(var->nbr_dim*sizeof(dmn_sct *)); else var->dim=(dmn_sct **)NULL;
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;

  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(var->nc_id,var->id,var->dmn_id);
  
  /* Type in memory begins as same type as on disk */
  var->type=var->typ_dsk; /* Type of variable in RAM */
  /* Type of packed data on disk */
  var->typ_pck=var->type;  /* Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */

  /* Refresh number of attributes and missing value attribute, if any */
  var->has_mss_val=mss_val_get(var->nc_id,var);

  /* Size defaults to 1 in var_dfl_set(), but set to 1 here for safety */
  var->sz=1L; 
  for(idx=0;idx<var->nbr_dim;idx++){
    (void)nco_inq_dimname(nc_id,var->dmn_id[idx],dmn_nm);
    /* Search input dimension list for matching name */
    for(dmn_idx=0;dmn_idx<nbr_dim;dmn_idx++){
      if(!strcmp(dmn_nm,dim[dmn_idx]->nm)) break;
    } /* end for */
    if(dmn_idx == nbr_dim){
      (void)fprintf(stdout,"%s: ERROR dimension %s is not in input dimension list\n",prg_nm_get(),dmn_nm);
      exit(EXIT_FAILURE);
    } /* end if */

    /* fxm: hmb, what is this for? */
    /* re-define dim_id so that if dim is the dimension list from the output file
       then we get the correct dim_id. Should not affect normal running of the
       routine as most the time, dim is the dimension from the input file */
    var->dmn_id[idx]=dim[dmn_idx]->id;

    var->dim[idx]=dim[dmn_idx];
    var->cnt[idx]=dim[dmn_idx]->cnt;
    var->srt[idx]=dim[dmn_idx]->srt;
    var->end[idx]=dim[dmn_idx]->end;
    var->srd[idx]=dim[dmn_idx]->srd;

    if(var->dmn_id[idx] == rec_dmn_id) var->is_rec_var=True; else var->sz_rec*=var->cnt[idx];

    if(var->dim[idx]->is_crd_dmn && var->id == var->dim[idx]->cid){
      var->is_crd_var=True;
      var->cid=var->dmn_id[idx];
    } /* end if */

    /* NB: This assumes default var->sz begins as 1 */
    var->sz*=var->cnt[idx];
  } /* end loop over dim */

  /* Portions of variable structure depend on packing propterties
     pck_dsk_inq() fills in these portions harmlessly */
  (void)pck_dsk_inq(nc_id,var);

  return var;
} /* end var_fll() */

void
var_refresh(int nc_id,var_sct *var)
/* 
   int nc_id: I netCDF input-file ID
   var_sct *var: I/O variable structure
 */
{
  /* Threads: Routine contains thread-unsafe calls protected by critical regions */
  /* Purpose: Update variable ID, number of dimensions, and missing_value attribute for given variable
     var_refresh() is called in file loop in multi-file operators because each new file may have 
     different variable ID and missing_value for same variable.
     This is necessary, for example, if a computer model runs for awhile on one machine, e.g., SGI,
     and then the run is restarted on another, e.g., Cray. 
     Since internal floating point representations differ betwee these architectures, the missing_value
     representation may differ. 
     Variable IDs may changes whenever someone fiddles with original model output in some files, 
     but not others, and then processes all files in a batch.
     NCO is one of the only tool I know of which makes all of this transparent to the user
     Thus this capability is very important to maintain
   */

  /* Refresh variable ID */
  var->nc_id=nc_id;

#ifdef _OPENMP
#pragma omp critical
#endif /* _OPENMP */
  { /* begin OpenMP critical */
    (void)nco_inq_varid(var->nc_id,var->nm,&var->id);
    
    /* fxm: Not sure if/why it is necessary refresh number of dimensions...but it should not hurt */
    /* Refresh number of dimensions in variable */
    (void)nco_inq_varndims(var->nc_id,var->id,&var->nbr_dim);
    
    /* Refresh number of attributes and missing value attribute, if any */
    var->has_mss_val=mss_val_get(var->nc_id,var);
  } /* end OpenMP critical */

} /* end var_refresh() */

