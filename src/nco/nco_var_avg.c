var_sct *
var_avg(var_sct *var,dmn_sct **dim,int nbr_dim,int nco_op_typ)
/*  
   var_sct *var: I/O pointer to variable structure (destroyed)
   dmn_sct **dim: I pointer to list of dimension structures
   int nbr_dim: I number of structures in list
   var_sct *var_avg(): O pointer to PARTIALLY (non-normalized) averaged variable
   nco_op_typ : average,min,max,ttl operation to perform, default is average
*/
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Routine to reduce given variable over specified dimensions. 
     "Reduce" means to reduce the rank of variable by performing an arithmetic operation
     The default operation is to average, but nco_op_typ can also be min, max, etc.
     The input variable structure is destroyed and the routine returns the resized, partially reduced variable
     For some operations, such as min, max, ttl, the variable returned by var_avg() is complete and need not be further processed
     But to complete the averaging operation, the output variable must be normalized by its tally array
     In other words, var_normalize() should be called subsequently if normalization is desired
     Normalization is not done internally to var_avg() in order to allow the user more flexibility
  */ 

  /* Create output variable as a duplicate of the input variable, except for dimensions which are to be averaged over */

  /* var_avg() overwrites contents, if any, of tally array with number of valid reduction operations */

  /* There are three variables to keep track of in this routine, their abbreviations are:
     var: Input variable (already hyperslabbed)
     avg: A contiguous arrangement of all elements of var that contribute to a single element of fix (a quasi-hyperslab)
     fix: Output (averaged) variable
   */

  dmn_sct **dmn_avg;
  dmn_sct **dmn_fix;

  int idx_avg_var[NC_MAX_DIMS];
  /*  int idx_var_avg[NC_MAX_DIMS];*/
  int idx_fix_var[NC_MAX_DIMS];
  /*  int idx_var_fix[NC_MAX_DIMS];*/
  int idx;
  int idx_dmn;
  int nbr_dmn_avg;
  int nbr_dmn_fix;
  int nbr_dmn_var;

  long avg_sz;
  long fix_sz;
  long var_sz;

  var_sct *fix;

  /* Copy basic attributes of input variable into output (averaged) variable */
  fix=var_dpl(var);
  (void)nco_xrf_var(fix,var->xrf);

  /* Create lists of averaging and fixed dimensions (in order of their appearance 
     in the variable). We do not know a priori how many dimensions remain in the 
     output (averaged) variable, but nbr_dmn_var is an upper bound. Similarly, we do
     not know a priori how many of the dimensions in the input list of averaging 
     dimensions (dim) actually occur in the current variable, so we do not know
     nbr_dmn_avg, but nbr_dim is an upper bound on it. */
  nbr_dmn_var=var->nbr_dim;
  nbr_dmn_fix=0;
  nbr_dmn_avg=0;
  dmn_avg=(dmn_sct **)nco_malloc(nbr_dim*sizeof(dmn_sct *));
  dmn_fix=(dmn_sct **)nco_malloc(nbr_dmn_var*sizeof(dmn_sct *));
  for(idx=0;idx<nbr_dmn_var;idx++){
    for(idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
      if(var->dmn_id[idx] == dim[idx_dmn]->id){
	dmn_avg[nbr_dmn_avg]=dim[idx_dmn];
	idx_avg_var[nbr_dmn_avg]=idx;
	/*	idx_var_avg[idx]=nbr_dmn_avg;*/ /* Variable is unused but instructive anyway */
	nbr_dmn_avg++;
	break;
      } /* end if */
    } /* end loop over idx_dmn */
    if(idx_dmn == nbr_dim){
      dmn_fix[nbr_dmn_fix]=var->dim[idx];
      idx_fix_var[nbr_dmn_fix]=idx;
      /*      idx_var_fix[idx]=nbr_dmn_fix;*/ /* Variable is unused but instructive anyway */
      nbr_dmn_fix++;
    } /* end if */
  } /* end loop over idx */

  /* Free extra list space */
  if(nbr_dmn_fix > 0) dmn_fix=(dmn_sct **)nco_realloc(dmn_fix,nbr_dmn_fix*sizeof(dmn_sct *)); else dmn_fix=(dmn_sct **)NULL;
  if(nbr_dmn_avg > 0) dmn_avg=(dmn_sct **)nco_realloc(dmn_avg,nbr_dmn_avg*sizeof(dmn_sct *)); else dmn_avg=(dmn_sct **)NULL;

  if(nbr_dmn_avg == 0){
    (void)fprintf(stderr,"%s: WARNING %s does not contain any averaging dimensions\n",prg_nm_get(),fix->nm);
    return (var_sct *)NULL;
  } /* end if */

  /* Get rid of averaged dimensions */
  fix->nbr_dim=nbr_dmn_fix;

  avg_sz=1L;
  for(idx=0;idx<nbr_dmn_avg;idx++){
    avg_sz*=dmn_avg[idx]->cnt;
    fix->sz/=dmn_avg[idx]->cnt;
    if(!dmn_avg[idx]->is_rec_dmn) fix->sz_rec/=dmn_avg[idx]->cnt;
  } /* end loop over idx */
  /* Convenience variable to avoid repetitive indirect addressing */
  fix_sz=fix->sz;

  fix->is_rec_var=False;
  for(idx=0;idx<nbr_dmn_fix;idx++){
    if(dmn_fix[idx]->is_rec_dmn) fix->is_rec_var=True;
    fix->dim[idx]=dmn_fix[idx];
    fix->dmn_id[idx]=dmn_fix[idx]->id;
    fix->srt[idx]=var->srt[idx_fix_var[idx]];
    fix->cnt[idx]=var->cnt[idx_fix_var[idx]];
    fix->end[idx]=var->end[idx_fix_var[idx]];
  } /* end loop over idx */
  
  fix->is_crd_var=False;
  if(nbr_dmn_fix == 1)
    if(dmn_fix[0]->is_crd_dmn) 
      fix->is_crd_var=True;

  /* Trim dimension arrays to their new sizes */
  if(nbr_dmn_fix > 0) fix->dim=(dmn_sct **)nco_realloc(fix->dim,nbr_dmn_fix*sizeof(dmn_sct *)); else fix->dim=NULL;
  if(nbr_dmn_fix > 0) fix->dmn_id=(int *)nco_realloc(fix->dmn_id,nbr_dmn_fix*sizeof(int)); else fix->dmn_id=NULL;
  if(nbr_dmn_fix > 0) fix->srt=(long *)nco_realloc(fix->srt,nbr_dmn_fix*sizeof(long)); else fix->srt=NULL;
  if(nbr_dmn_fix > 0) fix->cnt=(long *)nco_realloc(fix->cnt,nbr_dmn_fix*sizeof(long)); else fix->cnt=NULL;
  if(nbr_dmn_fix > 0) fix->end=(long *)nco_realloc(fix->end,nbr_dmn_fix*sizeof(long)); else fix->end=NULL;
  
  /* If product of sizes of all averaging dimensions is 1, input and output value arrays should be identical 
     Since var->val was already copied to fix->val by var_dpl() at the beginning
     of this routine, only one task remains, to set fix->tally appropriately. */
  if(avg_sz == 1L){
    long *fix_tally;

    fix_tally=fix->tally;

    /* First set tally field to 1 */
    for(idx=0;idx<fix_sz;idx++) fix_tally[idx]=1L;
    /* Next overwrite any missing value locations with zero */
    if(fix->has_mss_val){
      int val_sz_byte;

      char *val;
      char *mss_val;

      /* NB: Use char * rather than void * because some compilers (acc) will not do pointer
	 arithmetic on void * */
      mss_val=(char *)fix->mss_val.vp;
      val_sz_byte=nco_typ_lng(fix->type);
      val=(char *)fix->val.vp;
      for(idx=0;idx<fix_sz;idx++,val+=val_sz_byte)
	if(!memcmp(val,mss_val,val_sz_byte)) fix_tally[idx]=0L;
    } /* fix->has_mss_val */
  } /* end if avg_sz == 1L */

  /* Starting at first element of input hyperslab, add up next stride elements
     and place result in first element of output hyperslab. */
  if(avg_sz != 1L){
    char *avg_cp;
    char *var_cp;
    
    int type_sz;
    int nbr_dmn_var_m1;

    long *var_cnt;
    long avg_lmn;
    long fix_lmn;
    long var_lmn;
    long dmn_ss[NC_MAX_DIMS];
    long dmn_var_map[NC_MAX_DIMS];
    long dmn_avg_map[NC_MAX_DIMS];
    long dmn_fix_map[NC_MAX_DIMS];

    ptr_unn avg_val;

    nbr_dmn_var_m1=nbr_dmn_var-1;
    type_sz=nco_typ_lng(fix->type);
    var_cnt=var->cnt;
    var_cp=(char *)var->val.vp;
    var_sz=var->sz;
    
    /* Reuse the existing value buffer (it is of size var_sz, created by var_dpl())*/
    avg_val=fix->val;
    avg_cp=(char *)avg_val.vp;
    /* Create a new value buffer for output (averaged) size */
    fix->val.vp=(void *)nco_malloc(fix_sz*nco_typ_lng(fix->type));
    /* Resize (or just plain allocate) the tally array */
    fix->tally=(long *)nco_realloc(fix->tally,fix_sz*sizeof(long));

    /* Re-initialize value and tally arrays */
    (void)zero_long(fix_sz,fix->tally);
    (void)var_zero(fix->type,fix_sz,fix->val);
  
    /* Compute map for each dimension of variable */
    for(idx=0;idx<nbr_dmn_var;idx++) dmn_var_map[idx]=1L;
    for(idx=0;idx<nbr_dmn_var-1;idx++)
      for(idx_dmn=idx+1;idx_dmn<nbr_dmn_var;idx_dmn++)
	dmn_var_map[idx]*=var->cnt[idx_dmn];
    
    /* Compute map for each dimension of output variable */
    for(idx=0;idx<nbr_dmn_fix;idx++) dmn_fix_map[idx]=1L;
    for(idx=0;idx<nbr_dmn_fix-1;idx++)
      for(idx_dmn=idx+1;idx_dmn<nbr_dmn_fix;idx_dmn++)
	dmn_fix_map[idx]*=fix->cnt[idx_dmn];
    
    /* Compute map for each dimension of averaging buffer */
    for(idx=0;idx<nbr_dmn_avg;idx++) dmn_avg_map[idx]=1L;
    for(idx=0;idx<nbr_dmn_avg-1;idx++)
      for(idx_dmn=idx+1;idx_dmn<nbr_dmn_avg;idx_dmn++)
	dmn_avg_map[idx]*=dmn_avg[idx_dmn]->cnt;
    
    /* var_lmn is the offset into 1-D array */
    for(var_lmn=0;var_lmn<var_sz;var_lmn++){

      /* dmn_ss are corresponding indices (subscripts) into N-D array */
      dmn_ss[nbr_dmn_var_m1]=var_lmn%var_cnt[nbr_dmn_var_m1];
      for(idx=0;idx<nbr_dmn_var_m1;idx++){
	dmn_ss[idx]=(long)(var_lmn/dmn_var_map[idx]);
	dmn_ss[idx]%=var_cnt[idx];
      } /* end loop over dimensions */

      /* Map variable's N-D array indices into a 1-D index into averaged data */
      fix_lmn=0L;
      for(idx=0;idx<nbr_dmn_fix;idx++) fix_lmn+=dmn_ss[idx_fix_var[idx]]*dmn_fix_map[idx];
      
      /* Map N-D array indices into a 1-D offset from offset of its group */
      avg_lmn=0L;
      for(idx=0;idx<nbr_dmn_avg;idx++) avg_lmn+=dmn_ss[idx_avg_var[idx]]*dmn_avg_map[idx];
      
      /* Copy current element in input array into its slot in sorted avg_val */
      (void)memcpy(avg_cp+(fix_lmn*avg_sz+avg_lmn)*type_sz,var_cp+var_lmn*type_sz,type_sz);
      
    } /* end loop over var_lmn */
    
    /* Input data are now sorted and stored (in avg_val) in blocks (of length avg_sz)
       in same order as blocks' average values will appear in output buffer. 
       An averaging routine can take advantage of this by casting avg_val to a two dimensional
       variable and averaging over inner dimension. 
       This is where tally array is actually set */
    switch(nco_op_typ){
    case nco_op_max:
      (void)var_avg_reduce_max(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,avg_val,fix->val);
      break;
    case nco_op_min:
      (void)var_avg_reduce_min(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,avg_val,fix->val);
      break;
    case nco_op_avg: /* Operations: Previous=none, Current=sum, Next=normalize and root */
    case nco_op_sqravg: /* Operations: Previous=none, Current=sum, Next=normalize and square */
    case nco_op_avgsqr: /* Operations: Previous=square, Current=sum, Next=normalize */
    case nco_op_rms: /* Operations: Previous=square, Current=sum, Next=normalize and root */
    case nco_op_rmssdn: /* Operations: Previous=square, Current=sum, Next=normalize and root */
    case nco_op_ttl: /* Operations: Previous=none, Current=sum, Next=none */
    default:
      (void)var_avg_reduce_ttl(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,fix->tally,avg_val,fix->val);	  		
      break;
    } /* end case */

    /* Free dynamic memory that held rearranged input variable values */
    avg_val.vp=nco_free(avg_val.vp);
  } /* end if avg_sz != 1 */
  
  /* Free input variable */
  var=var_free(var);
  dmn_avg=nco_free(dmn_avg);
  dmn_fix=nco_free(dmn_fix);

  /* Return averaged variable */
  return fix;
} /* end var_avg() */
