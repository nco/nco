/* $Header$ */

/* Purpose: Average variables */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_var_avg.h" /* Average variables */

var_sct * /* O [sct] Partially (non-normalized) reduced variable */
nco_var_avg /* [fnc] Reduce given variable over specified dimensions */
(var_sct *var, /* I/O [sct] Variable to reduce (e.g., average) (destroyed) */
 dmn_sct * const * const dim, /* I [sct] Dimensions over which to reduce variable */
 const int nbr_dim, /* I [sct] Number of dimensions to reduce variable over */
 const int nco_op_typ, /* I [enm] Operation type, default is average */
 const nco_bool flg_rdd, /* I [flg] Retain degenerate dimensions */
 ddra_info_sct * const ddra_info) /* O [sct] DDRA information */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Reduce given variable over specified dimensions 
     "Reduce" means to rank-reduce variable by performing arithmetic operation
     Output variable is duplicate of input variable, except for averaging dimensions
     Default operation is averaging, but nco_op_typ can also be min, max, etc.
     nco_var_avg() overwrites contents, if any, of tally array with number of valid input elements contributing to each valid output element

     Input variable structure is destroyed and routine returns resized, partially reduced variable
     For some operations, such as min, max, ttl, variable returned by nco_var_avg() is complete and need not be further processed
     For averaging operation, output variable must be normalized by its tally array
     In other words, nco_var_nrm() should be called subsequently if normalization is desired
     Normalization is not done internally to nco_var_avg() to allow user more flexibility */ 
  
  /* Routine keeps track of three variables whose abbreviations are:
     avg: Array of averaging blocks, each a contiguous arrangement of all 
     elements of var which contribute to a single element of fix.
     fix: Output (averaged) variable
     rdd: Output (averaged) variable which retains degenerate dimensions
     var: Input variable (already hyperslabbed)
     
     It is easier to implement averaging as if all averaged dimensions are eliminated
     This presumption allows us to ignore degenerate dimension indices
     Including degenerate dimensions in fix would also complicate MRV-detection code 
     The core averaging algorithm treats input data as single 1-D array
     To retain degenerate dimensions, fxm */
  
  nco_bool AVG_DMN_ARE_MRV=False; /* [flg] Avergaging dimensions are MRV dimensions */

  dmn_sct **dmn_avg;
  dmn_sct **dmn_fix;

  int idx_avg_var[NC_MAX_VAR_DIMS];
  /*  int idx_var_avg[NC_MAX_VAR_DIMS];*/ /* Variable is unused but instructive anyway */
  int idx_fix_var[NC_MAX_VAR_DIMS];
  /*  int idx_var_fix[NC_MAX_VAR_DIMS];*/ /* Variable is unused but instructive anyway */
  int idx;
  int idx_dmn;
  int dmn_avg_nbr;
  int dmn_fix_nbr;
  int dmn_var_nbr;
  int dmn_rdd_nbr; /* flg_rdd ? dmn_var_nbr : dmn_fix_nbr */
  int nco_op_typ_lcl; /* [enm] Operation type, default is average */

  long avg_sz=int_CEWI;
  long fix_sz;
  long var_sz;

  var_sct *fix;

  /* Copy basic attributes of input variable into output (averaged) variable */
  fix=nco_var_dpl(var);

  /* Create lists of averaging and fixed dimensions (in order of their appearance 
     in the variable). We do not know a priori how many dimensions remain in the 
     output (averaged) variable, but dmn_var_nbr is an upper bound. Similarly, we do
     not know a priori how many of the dimensions in the input list of averaging 
     dimensions (dim) actually occur in the current variable, so we do not know
     dmn_avg_nbr, but nbr_dim is an upper bound on it. */
  dmn_var_nbr=var->nbr_dim;
  dmn_fix_nbr=0;
  dmn_avg_nbr=0;
  dmn_avg=(dmn_sct **)nco_malloc(nbr_dim*sizeof(dmn_sct *));
  dmn_fix=(dmn_sct **)nco_malloc(dmn_var_nbr*sizeof(dmn_sct *));
  for(idx=0;idx<dmn_var_nbr;idx++){
    for(idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
      /* Comparing dimension IDs is faster than comparing dimension names 
	 but requires assumption that all dimensions are from same file */
      if(var->dmn_id[idx] == dim[idx_dmn]->id){
        /* Although structures in dim are never altered, linking them into
	   dmn_avg list makes them vulnerable to manipulation and forces 
	   dim to lose const protection in prototype */
        dmn_avg[dmn_avg_nbr]=dim[idx_dmn];
        /* idx_avg_var[i]=j means that ith averaging dimension is jth dimension of var */
        idx_avg_var[dmn_avg_nbr]=idx;
        /* idx_var_avg[j]=i means that jth dimension of var is ith averaging dimension */
        /*	idx_var_avg[idx]=dmn_avg_nbr;*/ /* Variable is unused but instructive anyway */
        dmn_avg_nbr++;
        break;
      } /* end if */
    } /* end loop over idx_dmn */
    if(idx_dmn == nbr_dim){
      /* Dimension not in averaging list */
      dmn_fix[dmn_fix_nbr]=var->dim[idx];
      /* idx_fix_var[i]=j means that ith fixed dimension is jth dimension of var */
      idx_fix_var[dmn_fix_nbr]=idx;
      /* idx_var_fix[j]=i means that jth dimension of var is ith fixed dimension */
      /*      idx_var_fix[idx]=dmn_fix_nbr;*/ /* Variable is unused but instructive anyway */
      dmn_fix_nbr++;
    } /* end if */
  } /* end loop over idx */

  if(dmn_avg_nbr == 0){
    /* 20050517: ncwa only calls nco_var_avg() with variables containing averaging dimensions
       Variables without averaging dimensions are in the var_fix list 
       Preserve nco_var_avg() capability to work on var_fix variables for future flexibility */
    (void)fprintf(stderr,"%s: WARNING %s does not contain any averaging dimensions\n",nco_prg_nm_get(),fix->nm);
    /* Variable does not contain any averaging dimensions so we are done
       For consistency, return copy of variable held in fix and free() original
       Hence, nco_var_avg() always destroys original input and returns valid output */
    goto cln_and_xit;
  } /* end if */

  /* Use dmn_rdd_nbr rather than dmn_fix_nbr for dmn_fix memory management
     When flg_rdd is true, than malloc() and reallloc() will create/leave enough
     room in dmn_fix arrays to ultimately hold all dmn_rdd data
     However, dmn_fix_nbr remains same in flg_rdd and !flg_rdd cases
     Hence loops and averaging algorithm itself need not be re-coded
     Reconcile contents of dmn_fix structures with dengenerate dimensions at end */
  dmn_rdd_nbr=flg_rdd ? dmn_var_nbr : dmn_fix_nbr;
  
  /* Free extra list space */
  dmn_fix=(dmn_sct **)nco_realloc(dmn_fix,dmn_rdd_nbr*sizeof(dmn_sct *));
  dmn_avg=(dmn_sct **)nco_realloc(dmn_avg,dmn_avg_nbr*sizeof(dmn_sct *));

  /* Eliminate averaged dimensions */
  fix->nbr_dim=dmn_rdd_nbr;

  avg_sz=1L;
  for(idx=0;idx<dmn_avg_nbr;idx++){
    avg_sz*=dmn_avg[idx]->cnt;
    fix->sz/=dmn_avg[idx]->cnt;
    if(!dmn_avg[idx]->is_rec_dmn) fix->sz_rec/=dmn_avg[idx]->cnt;
  } /* end loop over idx */
  /* Convenience variable to avoid repetitive indirect addressing */
  fix_sz=fix->sz;

  fix->is_rec_var=False;
  for(idx=0;idx<dmn_fix_nbr;idx++){
    if(dmn_fix[idx]->is_rec_dmn) fix->is_rec_var=True;
    fix->dim[idx]=dmn_fix[idx];
    fix->dmn_id[idx]=dmn_fix[idx]->id;
    fix->srt[idx]=var->srt[idx_fix_var[idx]];
    fix->cnt[idx]=var->cnt[idx_fix_var[idx]];
    fix->end[idx]=var->end[idx_fix_var[idx]];
  } /* end loop over idx */

  fix->is_crd_var=False;
  if(dmn_fix_nbr == 1)
    if(dmn_fix[0]->is_crd_dmn) 
      fix->is_crd_var=True;

  /* Trim dimension arrays to their new sizes */
  fix->dim=(dmn_sct **)nco_realloc(fix->dim,dmn_rdd_nbr*sizeof(dmn_sct *));
  fix->dmn_id=(int *)nco_realloc(fix->dmn_id,dmn_rdd_nbr*sizeof(int));
  fix->srt=(long *)nco_realloc(fix->srt,dmn_rdd_nbr*sizeof(long));
  fix->cnt=(long *)nco_realloc(fix->cnt,dmn_rdd_nbr*sizeof(long));
  fix->end=(long *)nco_realloc(fix->end,dmn_rdd_nbr*sizeof(long));

  /* Resize (or just plain allocate) tally array */
  fix->tally=(long *)nco_realloc(fix->tally,fix_sz*sizeof(long));

  if(avg_sz == 1L){
    /* If averaging block size is 1L, input and output value arrays are identical 
       var->val was copied to fix->val by nco_var_dpl() at beginning of routine
       Only one task remains: to set fix->tally appropriately */
    long *fix_tally;

    fix_tally=fix->tally;

    /* Set tally field to 1 */
    for(idx=0;idx<fix_sz;idx++) fix_tally[idx]=1L;

    /* Overwrite any missing value locations with zero */
    if(fix->has_mss_val){
      int val_sz_byt;

      char *val;
      char *mss_val;

      /* NB: Use char * rather than void * because some compilers (acc, MSVC) will not do pointer arithmetic on void * */
      mss_val=(char *)fix->mss_val.vp;
      val_sz_byt=nco_typ_lng(fix->type);
      val=(char *)fix->val.vp;
      for(idx=0;idx<fix_sz;idx++,val+=val_sz_byt)
        if(!memcmp(val,mss_val,(size_t)val_sz_byt)) fix_tally[idx]=0L;
    } /* fix->has_mss_val */
  } /* end if avg_sz == 1L */

  /* Distribute all elements of input hyperslab into averaging block in avg_val
     Each block contains avg_sz elements in contiguous buffer 
     Reduction step then "reduces" each block into single output element */
  if(avg_sz != 1L){
    ptr_unn avg_val;

    /* Initialize data needed by reduction step independent of collection algorithm */
    var_sz=var->sz;

    /* Value buffer of size var_sz is currently duplicate of input values
       MRV algorithm uses these values without re-arranging
       General collection algorithm overwrites avg_val with averaging blocks */
    avg_val=fix->val;
    /* Create new value buffer for output (averaged) size */
    fix->val.vp=(void *)nco_malloc(fix_sz*nco_typ_lng(fix->type));

    /* Initialize value and tally arrays */
    (void)nco_zero_long(fix_sz,fix->tally);
    (void)nco_var_zero(fix->type,fix_sz,fix->val);

    /* Complex expensive collection step for creating averaging blocks works 
       in all cases though is unnecessary in one important case.
       No re-arrangement is necessary when averaging dimensions are most rapidly varying 
       (MRV) dimensions because original variable is stored in averaging block order.
       Averaging dimensions are MRV dimensions iff dmn_fix_nbr fixed dimensions are 
       one-to-one with first dmn_fix_nbr input dimensions. 
       Alternatively, could compare dmn_avg_nbr averaging dimensions to last 
       dmn_avg_nbr dimensions of input variable.
       However, averaging dimensions may appear in any order so it is more
       straightforward to compare fixed dimensions to LRV input dimensions. */
    for(idx=0;idx<dmn_fix_nbr;idx++) 
      if(idx_fix_var[idx] != idx) break;
    if(idx == dmn_fix_nbr){
      if(nco_dbg_lvl_get() >= nco_dbg_scl && nco_dbg_lvl_get() < 10) (void)fprintf(stderr,"%s: INFO Reduction dimensions are %d most-rapidly-varying (MRV) dimensions of %s. Will skip collection step and proceed straight to reduction step.\n",nco_prg_nm_get(),dmn_avg_nbr,fix->nm);
      AVG_DMN_ARE_MRV=True; /* [flg] Avergaging dimensions are MRV dimensions */
    } /* idx != dmn_fix_nbr */

    /* MRV algorithm simply skips this collection step 
       Some DDRA benchmarks need to know cost of collection
       Always invoke collection step by uncommenting following line: */
    /* AVG_DMN_ARE_MRV=False;*/
    if(!AVG_DMN_ARE_MRV){
      /* Dreaded, expensive collection algorithm sorts input into averaging blocks */
      char *avg_cp;
      char *var_cp;

      int typ_sz;
      int dmn_var_nbr_m1;

      long *var_cnt;
      long avg_lmn;
      long fix_lmn;
      long var_lmn;
      long dmn_ss[NC_MAX_VAR_DIMS];
      long dmn_var_map[NC_MAX_VAR_DIMS];
      long dmn_avg_map[NC_MAX_VAR_DIMS];
      long dmn_fix_map[NC_MAX_VAR_DIMS];

      dmn_var_nbr_m1=dmn_var_nbr-1;
      typ_sz=nco_typ_lng(fix->type);
      var_cnt=var->cnt;
      var_cp=(char *)var->val.vp;
      avg_cp=(char *)avg_val.vp;

      /* Compute map for each dimension of input variable */
      for(idx=0;idx<dmn_var_nbr;idx++) dmn_var_map[idx]=1L;
      for(idx=0;idx<dmn_var_nbr-1;idx++)
        for(idx_dmn=idx+1;idx_dmn<dmn_var_nbr;idx_dmn++)
          dmn_var_map[idx]*=var->cnt[idx_dmn];

      /* Compute map for each dimension of output variable */
      for(idx=0;idx<dmn_fix_nbr;idx++) dmn_fix_map[idx]=1L;
      for(idx=0;idx<dmn_fix_nbr-1;idx++)
        for(idx_dmn=idx+1;idx_dmn<dmn_fix_nbr;idx_dmn++)
          dmn_fix_map[idx]*=fix->cnt[idx_dmn];

      /* Compute map for each dimension of averaging buffer */
      for(idx=0;idx<dmn_avg_nbr;idx++) dmn_avg_map[idx]=1L;
      for(idx=0;idx<dmn_avg_nbr-1;idx++)
        for(idx_dmn=idx+1;idx_dmn<dmn_avg_nbr;idx_dmn++)
          dmn_avg_map[idx]*=dmn_avg[idx_dmn]->cnt;

      /* var_lmn is offset into 1-D array */
      for(var_lmn=0;var_lmn<var_sz;var_lmn++){
        /* dmn_ss are corresponding indices (subscripts) into N-D array */
        /* Operations: 1 modulo, 1 pointer offset, 1 user memory fetch
	   Repetitions: \lmnnbr
	   Total Counts: \rthnbr=2\lmnnbr, \mmrusrnbr=\lmnnbr
	   NB: LHS assumed compact and cached, counted RHS offsets and fetches only */
        dmn_ss[dmn_var_nbr_m1]=var_lmn%var_cnt[dmn_var_nbr_m1];
        for(idx=0;idx<dmn_var_nbr_m1;idx++){
          /* Operations: 1 divide, 1 modulo, 2 pointer offset, 2 user memory fetch
	     Repetitions: \lmnnbr(\dmnnbr-1)
	     Counts: \rthnbr=4\lmnnbr(\dmnnbr-1), \mmrusrnbr=2\lmnnbr(\dmnnbr-1)
	     NB: LHS assumed compact and cached, counted RHS offsets and fetches only
	     NB: Neglected loop arithmetic/compare */
          dmn_ss[idx]=(long)(var_lmn/dmn_var_map[idx]);
          dmn_ss[idx]%=var_cnt[idx];
        } /* end loop over dimensions */

        /* Map variable's N-D array indices into a 1-D index into averaged data */
        fix_lmn=0L;
        /* Operations: 1 add, 1 multiply, 3 pointer offset, 3 user memory fetch
	   Repetitions: \lmnnbr(\dmnnbr-\avgnbr)
	   Counts: \rthnbr=5\lmnnbr(\dmnnbr-\avgnbr), \mmrusrnbr=3\lmnnbr(\dmnnbr-\avgnbr) */
        for(idx=0;idx<dmn_fix_nbr;idx++) fix_lmn+=dmn_ss[idx_fix_var[idx]]*dmn_fix_map[idx];

        /* Map N-D array indices into 1-D offset from group offset */
        avg_lmn=0L;
        /* Operations: 1 add, 1 multiply, 3 pointer offset, 3 user memory fetch
	   Repetitions: \lmnnbr\avgnbr
	   Counts: \rthnbr=5\lmnnbr\avgnbr, \mmrusrnbr=3\lmnnbr\avgnbr */
        for(idx=0;idx<dmn_avg_nbr;idx++) avg_lmn+=dmn_ss[idx_avg_var[idx]]*dmn_avg_map[idx];

        /* Copy current element in input array into its slot in sorted avg_val */
        /* Operations: 3 add, 3 multiply, 0 pointer offset, 1 system memory copy
	   Repetitions: \lmnnbr
	   Counts: \rthnbr=6\lmnnbr, \mmrusrnbr=0, \mmrsysnbr=1 */
        (void)memcpy(avg_cp+(fix_lmn*avg_sz+avg_lmn)*typ_sz,var_cp+var_lmn*typ_sz,(size_t)typ_sz);
      } /* end loop over var_lmn */
    } /* AVG_DMN_ARE_MRV */

    /* Input data are now sorted and stored (in avg_val) in blocks (of length avg_sz)
       in same order as blocks' average values will appear in output buffer. 
       Averaging routines can take advantage of this by casting avg_val to 
       two dimensional variable and averaging over inner dimension. 
       nco_var_avg_rdc_*() sets tally array
       NCO operations on coordinate variables are restricted to averaging
       ncap2 is an exception because presumably the user knows what he/she is doing */
    nco_op_typ_lcl=nco_op_typ;
    if(nco_prg_id_get() != ncap && var->is_crd_var) nco_op_typ_lcl=nco_op_avg;
    switch(nco_op_typ_lcl){
    case nco_op_mabs:
    case nco_op_max:
      (void)nco_var_avg_rdc_max(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,avg_val,fix->val);
      break;
    case nco_op_mibs:
    case nco_op_min:
      (void)nco_var_avg_rdc_min(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,avg_val,fix->val);
      break;
    case nco_op_mebs: /* Operations: Previous=none, Current=sum, Next=normalize */
    case nco_op_avg: /* Operations: Previous=none, Current=sum, Next=normalize */
    case nco_op_sqravg: /* Operations: Previous=none, Current=sum, Next=normalize and square */
    case nco_op_avgsqr: /* Operations: Previous=square, Current=sum, Next=normalize */
    case nco_op_rms: /* Operations: Previous=square, Current=sum, Next=normalize and root */
    case nco_op_rmssdn: /* Operations: Previous=square, Current=sum, Next=normalize and root */
    case nco_op_ttl: /* Operations: Previous=none, Current=sum, Next=none */
    default:
      (void)nco_var_avg_rdc_ttl(fix->type,var_sz,fix_sz,fix->has_mss_val,fix->mss_val,fix->tally,avg_val,fix->val);
      break;
    } /* end case */

    /* Free dynamic memory that held rearranged input variable values */
    avg_val.vp=nco_free(avg_val.vp);
  } /* end if avg_sz != 1 */

  /* Retain degenerate dimensions? */
  if(flg_rdd){
    /* Simplest way to retain degenerate dimensions is average over them,
       then insert degenerate dimensions back into list at end */

    /* Eliminate averaged dimensions */
    fix->nbr_dim=dmn_rdd_nbr;
    if(var->is_rec_var) fix->is_rec_var=True;
    fix->is_crd_var=var->is_crd_var;

    /* Copy information from input variable.... */
    for(idx=0;idx<dmn_rdd_nbr;idx++){
      fix->dim[idx]=var->dim[idx];
      fix->dmn_id[idx]=var->dmn_id[idx];
      fix->srt[idx]=var->srt[idx];
      fix->cnt[idx]=var->cnt[idx];
      fix->end[idx]=var->end[idx];
    } /* end loop over idx */
    /* ...then overwrite specfific information for degenerate dimensions... */
    for(idx=0;idx<dmn_avg_nbr;idx++){
      fix->cnt[idx_avg_var[idx]]=1L;
      fix->srt[idx_avg_var[idx]]=fix->end[idx_avg_var[idx]]=0L;
    } /* end loop over idx */

  } /* !flg_rdd */

  /* Jump here when variable is not to be reduced. This occurs when
     1. Variable contains no averaging dimensions
     2. Averaging block size is 1 */
cln_and_xit:

  /* Free input variable */
  var=nco_var_free(var);
  dmn_avg=(dmn_sct **)nco_free(dmn_avg);
  dmn_fix=(dmn_sct **)nco_free(dmn_fix);

  /* Set diagnostic DDRA information DDRA */
  ddra_info->lmn_nbr_avg=avg_sz; /* [nbr] Averaging block size */
  ddra_info->rnk_avg=dmn_avg_nbr; /* [nbr] Rank of averaging space */
  ddra_info->MRV_flg=AVG_DMN_ARE_MRV; /* [flg] Avergaging dimensions are MRV dimensions */

  /* Return averaged variable */
  return fix;
} /* end nco_var_avg() */

void
nco_var_avg_rdc_ttl /* [fnc] Sum blocks of op1 into each element of op2 */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz_op1, /* I [nbr] Size (in elements) of op1 */
 const long sz_op2, /* I [nbr] Size (in elements) of op2 */
 const int has_mss_val, /* I [flg] Operand has missing values */
 ptr_unn mss_val, /* I [sct] Missing value */
 long * const tally, /* I/O [nbr] Tally buffer */
 ptr_unn op1, /* I [sct] Operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2)) */
 ptr_unn op2) /* O [sct] Sum of each block of op1 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Sum values in each contiguous block of first operand and place
     result in corresponding element in second operand. 
     Currently arithmetic operation performed is summation of elements in op1
     Input operands are assumed to have conforming types, but not dimensions or sizes
     nco_var_avg_rdc_ttl() knows nothing about dimensions
     Routine is one dimensional array operator acting serially on each element of input buffer op1
     Calling rouine knows exactly how rank of output, op2, is reduced from rank of input
     Routine only does summation rather than averaging in order to remain flexible
     Operations which require normalization, e.g., averaging, must call nco_var_nrm() 
     or nco_var_dvd() to divide sum set in this routine by tally set in this routine. */
  
  /* Each operation has GNUC and non-GNUC blocks:
     GNUC: Utilize (non-ANSI-compliant) compiler support for local automatic arrays
     This results in more elegent loop structure and, theoretically, in faster performance
     20040225: In reality, the GNUC non-ANSI blocks fail on some large files
     This may be because they allocate significant local storage on the stack
     
     non-GNUC: Fully ANSI-compliant structure
     Fortran: Support deprecated */
  
#define FXM_NCO315 1
#ifdef FXM_NCO315
  long idx_op1;
#endif /* __GNUC__ */
  const long sz_blk=sz_op1/sz_op2;
  long idx_op2;
  long idx_blk;

  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_byte mss_val_byte=nco_byte_CEWI;
  nco_int mss_val_ntg=nco_int_CEWI;
  nco_int64 mss_val_int64=nco_int64_CEWI;
  nco_short mss_val_short=nco_short_CEWI;
  nco_ubyte mss_val_ubyte=nco_ubyte_CEWI;
  nco_uint mss_val_uint=nco_uint_CEWI;
  nco_uint64 mss_val_uint64=nco_uint64_CEWI;
  nco_ushort mss_val_ushort=nco_ushort_CEWI;
  nco_char mss_val_char=nco_char_CEWI;
  nco_string mss_val_string=nco_string_CEWI;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_short=*mss_val.sp; break;
    case NC_INT: mss_val_ntg=*mss_val.ip; break;
    case NC_BYTE: mss_val_byte=*mss_val.bp; break;
    case NC_UBYTE: mss_val_ubyte=*mss_val.ubp; break;
    case NC_USHORT: mss_val_ushort=*mss_val.usp; break;
    case NC_UINT: mss_val_uint=*mss_val.uip; break;
    case NC_INT64: mss_val_int64=*mss_val.i64p; break;
    case NC_UINT64: mss_val_uint64=*mss_val.ui64p; break;
    case NC_CHAR: mss_val_char=*mss_val.cp; break;
    case NC_STRING: mss_val_string=*mss_val.sngp; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  } /* endif */
  
  switch(type){
  case NC_FLOAT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        /* Operations: 1 multiply 
	   Repetitions: \dmnszavg^(\dmnnbr-\avgnbr)
	   Total Counts: \rthnbr=\dmnszavg^(\dmnnbr-\avgnbr) */
        const long blk_off=idx_op2*sz_blk;
        /* Operations: 1 fp add, 3 pointer offsets, 3 user memory fetch
	   Repetitions: \lmnnbr
	   Total Counts: \flpnbr=\lmnnbr, \rthnbr=3\lmnnbr, \mmrusrnbr=3\lmnnbr,
	   NB: Counted LHS+RHS+tally offsets and fetches */
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.fp[idx_op2]+=op1.fp[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      if(isfinite(mss_val_flt)){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          const long blk_off=idx_op2*sz_blk;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            idx_op1=blk_off+idx_blk;
            if(op1.fp[idx_op1] != mss_val_flt){
              op2.fp[idx_op2]+=op1.fp[idx_op1];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.fp[idx_op2]=mss_val_flt;
        } /* end loop over idx_op2 */
      }else{ /* Missing value exists and is NaN-like */
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          const long blk_off=idx_op2*sz_blk;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            idx_op1=blk_off+idx_blk;
            /* Missing value is NaN-like so comparisons to it are always false---must use macros */
            if(isfinite(op1.fp[idx_op1])){
              op2.fp[idx_op2]+=op1.fp[idx_op1];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.fp[idx_op2]=mss_val_flt;
        } /* end loop over idx_op2 */
      } /* !Missing value exists and is NaN-like */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        if(isfinite(mss_val_flt)){
          for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
            for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
              if(op1_2D[idx_op2][idx_blk] != mss_val_flt){
                op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
                tally[idx_op2]++;
              } /* end if */
            } /* end loop over idx_blk */
            if(tally[idx_op2] == 0L) op2.fp[idx_op2]=mss_val_flt;
          } /* end loop over idx_op2 */
        }else{ /* Missing value exists and is NaN-like */
          for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
            for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
              if(isfinite(op1_2D[idx_op2][idx_blk])){
                op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
                tally[idx_op2]++;
              } /* end if */
            } /* end loop over idx_blk */
            if(tally[idx_op2] == 0L) op2.fp[idx_op2]=mss_val_flt;
          } /* end loop over idx_op2 */
        } /* !Missing value exists and is NaN-like */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_DOUBLE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.dp[idx_op2]+=op1.dp[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.dp[idx_op1] != mss_val_dbl){
            op2.dp[idx_op2]+=op1.dp[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.dp[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
              op2.dp[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.dp[idx_op2]=mss_val_dbl;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.ip[idx_op2]+=op1.ip[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ip[idx_op1] != mss_val_ntg){
            op2.ip[idx_op2]+=op1.ip[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.ip[idx_op2]=mss_val_ntg;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ip),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.ip[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ntg){
              op2.ip[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.ip[idx_op2]=mss_val_ntg;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.sp[idx_op2]+=op1.sp[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.sp[idx_op1] != mss_val_short){
            op2.sp[idx_op2]+=op1.sp[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.sp[idx_op2]=mss_val_short;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_short op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.sp[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_short){
              op2.sp[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.sp[idx_op2]=mss_val_short;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_USHORT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.usp[idx_op2]+=op1.usp[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.usp[idx_op1] != mss_val_ushort){
            op2.usp[idx_op2]+=op1.usp[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.usp[idx_op2]=mss_val_ushort;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_ushort op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.usp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.usp[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ushort){
              op2.usp[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.usp[idx_op2]=mss_val_ushort;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UINT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.uip[idx_op2]+=op1.uip[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.uip[idx_op1] != mss_val_uint){
            op2.uip[idx_op2]+=op1.uip[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.uip[idx_op2]=mss_val_uint;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_uint op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.uip),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.uip[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_uint){
              op2.uip[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.uip[idx_op2]=mss_val_uint;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT64:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.i64p[idx_op2]+=op1.i64p[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.i64p[idx_op1] != mss_val_int64){
            op2.i64p[idx_op2]+=op1.i64p[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.i64p[idx_op2]=mss_val_int64;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_int64 op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.i64p),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.i64p[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_int64){
              op2.i64p[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.i64p[idx_op2]=mss_val_int64;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UINT64:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.ui64p[idx_op2]+=op1.ui64p[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ui64p[idx_op1] != mss_val_uint64){
            op2.ui64p[idx_op2]+=op1.ui64p[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.ui64p[idx_op2]=mss_val_uint64;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_uint64 op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ui64p),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.ui64p[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_uint64){
              op2.ui64p[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.ui64p[idx_op2]=mss_val_uint64;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_BYTE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.bp[idx_op2]+=op1.bp[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.bp[idx_op1] != mss_val_byte){
            op2.bp[idx_op2]+=op1.bp[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.bp[idx_op2]=mss_val_byte;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_byte op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.bp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.bp[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_byte){
              op2.bp[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.bp[idx_op2]=mss_val_byte;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UBYTE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.ubp[idx_op2]+=op1.ubp[blk_off+idx_blk];
        tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ubp[idx_op1] != mss_val_ubyte){
            op2.ubp[idx_op2]+=op1.ubp[idx_op1];
            tally[idx_op2]++;
          } /* end if */
        } /* end loop over idx_blk */
        if(tally[idx_op2] == 0L) op2.ubp[idx_op2]=mss_val_ubyte;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_ubyte op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ubp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.ubp[idx_op2]+=op1_2D[idx_op2][idx_blk];
          tally[idx_op2]=sz_blk;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ubyte){
              op2.ubp[idx_op2]+=op1_2D[idx_op2][idx_blk];
              tally[idx_op2]++;
            } /* end if */
          } /* end loop over idx_blk */
          if(tally[idx_op2] == 0L) op2.ubp[idx_op2]=mss_val_ubyte;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Only allow ANSI-compliant branch. "total" is assigned value of first element of block */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.cp[idx_op2]=op1.cp[blk_off];
        tally[idx_op2]=1L;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
	idx_op1=blk_off;
	if(op1.cp[idx_op1] != mss_val_char){
	  op2.cp[idx_op2]=op1.cp[idx_op1];
	  tally[idx_op2]=1L;
	} /* end if */
        if(tally[idx_op2] == 0L) op2.cp[idx_op2]=mss_val_char;
      } /* end loop over idx_op2 */
    } /* end else */
    break;
  case NC_STRING: 
    /* Only allow ANSI-compliant branch. "total" is assigned value of first element of block */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.sngp[idx_op2]=op1.sngp[blk_off];
        tally[idx_op2]=1L;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
	idx_op1=blk_off;
	if(op1.sngp[idx_op1] != mss_val_string){
	  op2.sngp[idx_op2]=op1.sngp[idx_op1];
	  tally[idx_op2]=1L;
	} /* end if */
        if(tally[idx_op2] == 0L) op2.sngp[idx_op2]=mss_val_string;
      } /* end loop over idx_op2 */
    } /* end else */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_avg_rdc_ttl() */

void
nco_var_avg_rdc_min /* [fnc] Place minimum of op1 blocks into each element of op2 */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz_op1, /* I [nbr] Size (in elements) of op1 */
 const long sz_op2, /* I [nbr] Size (in elements) of op2 */
 const int has_mss_val, /* I [flg] Operand has missing values */
 ptr_unn mss_val, /* I [sct] Missing value */
 ptr_unn op1, /* I [sct] Operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2)) */
 ptr_unn op2) /* O [sct] Minimum of each block of op1 */
{
  /* Purpose: Find minimum value of each contiguous block of first operand and place
     result in corresponding element in second operand. Operands are assumed to have
     conforming types, but not dimensions or sizes. */
  
  /* nco_var_avg_rdc_min() is derived from nco_var_avg_rdc_ttl()
     Routines are very similar but tallies are not incremented
     See nco_var_avg_rdc_ttl() for more algorithmic documentation
     nco_var_avg_rdc_max() is derived from nco_var_avg_rdc_min() */
  
#define FXM_NCO315 1
#ifdef FXM_NCO315
  long idx_op1;
#endif /* __GNUC__ */
  const long sz_blk=sz_op1/sz_op2;
  long idx_op2;
  long idx_blk;
  
  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_byte mss_val_byte=nco_byte_CEWI;
  nco_int mss_val_ntg=nco_int_CEWI;
  nco_int64 mss_val_int64=nco_int64_CEWI;
  nco_short mss_val_short=nco_short_CEWI;
  nco_ubyte mss_val_ubyte=nco_ubyte_CEWI;
  nco_uint mss_val_uint=nco_uint_CEWI;
  nco_uint64 mss_val_uint64=nco_uint64_CEWI;
  nco_ushort mss_val_ushort=nco_ushort_CEWI;
  nco_char mss_val_char=nco_char_CEWI;
  nco_string mss_val_string=nco_string_CEWI;
  
  nco_bool flg_mss=False; /* [flg] Block has valid (non-missing) values */
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_short=*mss_val.sp; break;
    case NC_INT: mss_val_ntg=*mss_val.ip; break;
    case NC_BYTE: mss_val_byte=*mss_val.bp; break;
    case NC_UBYTE: mss_val_ubyte=*mss_val.ubp; break;
    case NC_USHORT: mss_val_ushort=*mss_val.usp; break;
    case NC_UINT: mss_val_uint=*mss_val.uip; break;
    case NC_INT64: mss_val_int64=*mss_val.i64p; break;
    case NC_UINT64: mss_val_uint64=*mss_val.ui64p; break;
    case NC_CHAR: mss_val_char=*mss_val.cp; break;
    case NC_STRING: mss_val_string=*mss_val.sngp; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  } /* endif */
  
  switch(type){
  case NC_FLOAT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.fp[idx_op2]=op1.fp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.fp[idx_op2] > op1.fp[blk_off+idx_blk]) op2.fp[idx_op2]=op1.fp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.fp[idx_op1] != mss_val_flt) {
            if(!flg_mss || op2.fp[idx_op2] > op1.fp[idx_op1]) op2.fp[idx_op2]=op1.fp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.fp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.fp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_flt) {
              if(!flg_mss || op2.fp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_DOUBLE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.dp[idx_op2]=op1.dp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.dp[idx_op2] > op1.dp[blk_off+idx_blk]) op2.dp[idx_op2]=op1.dp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.dp[idx_op1] != mss_val_dbl) {
            if(!flg_mss || (op2.dp[idx_op2] > op1.dp[idx_op1])) op2.dp[idx_op2]=op1.dp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.dp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.dp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk] ;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
              if(!flg_mss || (op2.dp[idx_op2] > op1_2D[idx_op2][idx_blk])) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk];	    
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.ip[idx_op2]=op1.ip[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.ip[idx_op2] > op1.ip[blk_off+idx_blk]) op2.ip[idx_op2]=op1.ip[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ip[idx_op1] != mss_val_ntg){
            if(!flg_mss || op2.ip[idx_op2] > op1.ip[idx_op1]) op2.ip[idx_op2]=op1.ip[idx_op1];
            flg_mss= True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.ip[idx_op2]=mss_val_ntg;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ip),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.ip[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.ip[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.ip[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ntg){
              if(!flg_mss || op2.ip[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.ip[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.ip[idx_op2]=mss_val_ntg;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.sp[idx_op2]=op1.sp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.sp[idx_op2] > op1.sp[blk_off+idx_blk]) op2.sp[idx_op2]=op1.sp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.sp[idx_op1] != mss_val_short){
            if(!flg_mss || op2.sp[idx_op2] > op1.sp[idx_op1]) op2.sp[idx_op2]=op1.sp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.sp[idx_op2]=mss_val_short;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_short op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.sp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.sp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_short){
              if(!flg_mss  || op2.sp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.sp[idx_op2]=mss_val_short;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_USHORT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.usp[idx_op2]=op1.usp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.usp[idx_op2] > op1.usp[blk_off+idx_blk]) op2.usp[idx_op2]=op1.usp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.usp[idx_op1] != mss_val_ushort){
            if(!flg_mss || op2.usp[idx_op2] > op1.usp[idx_op1]) op2.usp[idx_op2]=op1.usp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.usp[idx_op2]=mss_val_ushort;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_ushort op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.usp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.usp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.usp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.usp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ushort){
              if(!flg_mss  || op2.usp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.usp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.usp[idx_op2]=mss_val_ushort;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UINT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.uip[idx_op2]=op1.uip[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.uip[idx_op2] > op1.uip[blk_off+idx_blk]) op2.uip[idx_op2]=op1.uip[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.uip[idx_op1] != mss_val_uint){
            if(!flg_mss || op2.uip[idx_op2] > op1.uip[idx_op1]) op2.uip[idx_op2]=op1.uip[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.uip[idx_op2]=mss_val_uint;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_uint op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.uip),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.uip[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.uip[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.uip[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_uint){
              if(!flg_mss  || op2.uip[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.uip[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.uip[idx_op2]=mss_val_uint;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT64:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.i64p[idx_op2]=op1.i64p[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.i64p[idx_op2] > op1.i64p[blk_off+idx_blk]) op2.i64p[idx_op2]=op1.i64p[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.i64p[idx_op1] != mss_val_int64){
            if(!flg_mss || op2.i64p[idx_op2] > op1.i64p[idx_op1]) op2.i64p[idx_op2]=op1.i64p[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.i64p[idx_op2]=mss_val_int64;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_int64 op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.i64p),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.i64p[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.i64p[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.i64p[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_int64){
              if(!flg_mss  || op2.i64p[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.i64p[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.i64p[idx_op2]=mss_val_int64;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UINT64:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.ui64p[idx_op2]=op1.ui64p[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.ui64p[idx_op2] > op1.ui64p[blk_off+idx_blk]) op2.ui64p[idx_op2]=op1.ui64p[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ui64p[idx_op1] != mss_val_uint64){
            if(!flg_mss || op2.ui64p[idx_op2] > op1.ui64p[idx_op1]) op2.ui64p[idx_op2]=op1.ui64p[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.ui64p[idx_op2]=mss_val_uint64;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_uint64 op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ui64p),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.ui64p[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.ui64p[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.ui64p[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_uint64){
              if(!flg_mss  || op2.ui64p[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.ui64p[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.ui64p[idx_op2]=mss_val_uint64;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_BYTE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.bp[idx_op2]=op1.bp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.bp[idx_op2] > op1.bp[blk_off+idx_blk]) op2.bp[idx_op2]=op1.bp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.bp[idx_op1] != mss_val_byte){
            if(!flg_mss || op2.bp[idx_op2] > op1.bp[idx_op1]) op2.bp[idx_op2]=op1.bp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.bp[idx_op2]=mss_val_byte;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_byte op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.bp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.bp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.bp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.bp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_byte){
              if(!flg_mss  || op2.bp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.bp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.bp[idx_op2]=mss_val_byte;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UBYTE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.ubp[idx_op2]=op1.ubp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.ubp[idx_op2] > op1.ubp[blk_off+idx_blk]) op2.ubp[idx_op2]=op1.ubp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ubp[idx_op1] != mss_val_ubyte){
            if(!flg_mss || op2.ubp[idx_op2] > op1.ubp[idx_op1]) op2.ubp[idx_op2]=op1.ubp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.ubp[idx_op2]=mss_val_ubyte;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_ubyte op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ubp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.ubp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.ubp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.ubp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ubyte){
              if(!flg_mss  || op2.ubp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.ubp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.ubp[idx_op2]=mss_val_ubyte;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Only allow ANSI-compliant branch. "min" is assigned value of first element of block */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.cp[idx_op2]=op1.cp[blk_off];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
	idx_op1=blk_off;
	if(op1.cp[idx_op1] != mss_val_char){
	  if(!flg_mss) op2.cp[idx_op2]=op1.cp[idx_op1];
	  flg_mss=True;
	} /* end if */
        if(!flg_mss) op2.cp[idx_op2]=mss_val_char;
      } /* end loop over idx_op2 */
    } /* end else */
    break;
  case NC_STRING: 
    /* Only allow ANSI-compliant branch. "min" is assigned value of first element of block */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.sngp[idx_op2]=op1.sngp[blk_off];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
	idx_op1=blk_off;
	if(op1.sngp[idx_op1] != mss_val_string){
	  if(!flg_mss) op2.sngp[idx_op2]=op1.sngp[idx_op1];
	  flg_mss=True;
	} /* end if */
        if(!flg_mss) op2.sngp[idx_op2]=mss_val_string;
      } /* end loop over idx_op2 */
    } /* end else */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end  switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end nco_var_avg_rdc_min() */

void
nco_var_avg_rdc_max /* [fnc] Place maximum of op1 blocks into each element of op2 */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz_op1, /* I [nbr] Size (in elements) of op1 */
 const long sz_op2, /* I [nbr] Size (in elements) of op2 */
 const int has_mss_val, /* I [flg] Operand has missing values */
 ptr_unn mss_val, /* I [sct] Missing value */
 ptr_unn op1, /* I [sct] Operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2)) */
 ptr_unn op2) /* O [sct] Maximum of each block of op1 */
{
  /* Purpose: Find maximum value of each contiguous block of first operand and place
     result in corresponding element in second operand. Operands are assumed to have
     conforming types, but not dimensions or sizes. */
  
  /* nco_var_avg_rdc_min() is derived from nco_var_avg_rdc_ttl()
     Routines are very similar but tallies are not incremented
     See nco_var_avg_rdc_ttl() for more algorithmic documentation
     nco_var_avg_rdc_max() is derived from nco_var_avg_rdc_min() */
  
#define FXM_NCO315 1
#ifdef FXM_NCO315
  long idx_op1;
#endif /* __GNUC__ */
  const long sz_blk=sz_op1/sz_op2;
  long idx_op2;
  long idx_blk;

  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_byte mss_val_byte=nco_byte_CEWI;
  nco_int mss_val_ntg=nco_int_CEWI;
  nco_int64 mss_val_int64=nco_int64_CEWI;
  nco_short mss_val_short=nco_short_CEWI;
  nco_ubyte mss_val_ubyte=nco_ubyte_CEWI;
  nco_uint mss_val_uint=nco_uint_CEWI;
  nco_uint64 mss_val_uint64=nco_uint64_CEWI;
  nco_ushort mss_val_ushort=nco_ushort_CEWI;
  nco_char mss_val_char=nco_char_CEWI;
  nco_string mss_val_string=nco_string_CEWI;
  
  nco_bool flg_mss=False;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_short=*mss_val.sp; break;
    case NC_INT: mss_val_ntg=*mss_val.ip; break;
    case NC_BYTE: mss_val_byte=*mss_val.bp; break;
    case NC_UBYTE: mss_val_ubyte=*mss_val.ubp; break;
    case NC_USHORT: mss_val_ushort=*mss_val.usp; break;
    case NC_UINT: mss_val_uint=*mss_val.uip; break;
    case NC_INT64: mss_val_int64=*mss_val.i64p; break;
    case NC_UINT64: mss_val_uint64=*mss_val.ui64p; break;
    case NC_CHAR: mss_val_char=*mss_val.cp; break;
    case NC_STRING: mss_val_string=*mss_val.sngp; break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  } /* endif */

  switch(type){
  case NC_FLOAT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.fp[idx_op2]=op1.fp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.fp[idx_op2] < op1.fp[blk_off+idx_blk]) op2.fp[idx_op2]=op1.fp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.fp[idx_op1] != mss_val_flt) {
            if(!flg_mss || op2.fp[idx_op2] < op1.fp[idx_op1]) op2.fp[idx_op2]=op1.fp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.fp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.fp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_flt) {
              if(!flg_mss || op2.fp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_DOUBLE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.dp[idx_op2]=op1.dp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.dp[idx_op2] < op1.dp[blk_off+idx_blk]) op2.dp[idx_op2]=op1.dp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.dp[idx_op1] != mss_val_dbl) {
            if(!flg_mss || (op2.dp[idx_op2] < op1.dp[idx_op1])) op2.dp[idx_op2]=op1.dp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.dp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.dp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk] ;
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
              if(!flg_mss || (op2.dp[idx_op2] < op1_2D[idx_op2][idx_blk])) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk];	    
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.ip[idx_op2]=op1.ip[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.ip[idx_op2] < op1.ip[blk_off+idx_blk]) op2.ip[idx_op2]=op1.ip[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ip[idx_op1] != mss_val_ntg){
            if(!flg_mss || op2.ip[idx_op2] < op1.ip[idx_op1]) op2.ip[idx_op2]=op1.ip[idx_op1];
            flg_mss= True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.ip[idx_op2]=mss_val_ntg;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ip),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.ip[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.ip[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.ip[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ntg){
              if(!flg_mss || op2.ip[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.ip[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.ip[idx_op2]=mss_val_ntg;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.sp[idx_op2]=op1.sp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.sp[idx_op2] < op1.sp[blk_off+idx_blk]) op2.sp[idx_op2]=op1.sp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.sp[idx_op1] != mss_val_short){
            if(!flg_mss || op2.sp[idx_op2] < op1.sp[idx_op1]) op2.sp[idx_op2]=op1.sp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.sp[idx_op2]=mss_val_short;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_short op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.sp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.sp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_short){
              if(!flg_mss  || op2.sp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.sp[idx_op2]=mss_val_short;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_USHORT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.usp[idx_op2]=op1.usp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.usp[idx_op2] < op1.usp[blk_off+idx_blk]) op2.usp[idx_op2]=op1.usp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.usp[idx_op1] != mss_val_ushort){
            if(!flg_mss || op2.usp[idx_op2] < op1.usp[idx_op1]) op2.usp[idx_op2]=op1.usp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.usp[idx_op2]=mss_val_ushort;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_ushort op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.usp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.usp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.usp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.usp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ushort){
              if(!flg_mss  || op2.usp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.usp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.usp[idx_op2]=mss_val_ushort;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UINT:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.uip[idx_op2]=op1.uip[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.uip[idx_op2] < op1.uip[blk_off+idx_blk]) op2.uip[idx_op2]=op1.uip[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.uip[idx_op1] != mss_val_uint){
            if(!flg_mss || op2.uip[idx_op2] < op1.uip[idx_op1]) op2.uip[idx_op2]=op1.uip[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.uip[idx_op2]=mss_val_uint;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_uint op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.uip),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.uip[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.uip[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.uip[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_uint){
              if(!flg_mss  || op2.uip[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.uip[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.uip[idx_op2]=mss_val_uint;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT64:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.i64p[idx_op2]=op1.i64p[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.i64p[idx_op2] < op1.i64p[blk_off+idx_blk]) op2.i64p[idx_op2]=op1.i64p[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.i64p[idx_op1] != mss_val_int64){
            if(!flg_mss || op2.i64p[idx_op2] < op1.i64p[idx_op1]) op2.i64p[idx_op2]=op1.i64p[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.i64p[idx_op2]=mss_val_int64;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_int64 op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.i64p),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.i64p[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.i64p[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.i64p[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_int64){
              if(!flg_mss  || op2.i64p[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.i64p[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.i64p[idx_op2]=mss_val_int64;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UINT64:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.ui64p[idx_op2]=op1.ui64p[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.ui64p[idx_op2] < op1.ui64p[blk_off+idx_blk]) op2.ui64p[idx_op2]=op1.ui64p[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ui64p[idx_op1] != mss_val_uint64){
            if(!flg_mss || op2.ui64p[idx_op2] < op1.ui64p[idx_op1]) op2.ui64p[idx_op2]=op1.ui64p[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.ui64p[idx_op2]=mss_val_uint64;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_uint64 op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ui64p),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.ui64p[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.ui64p[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.ui64p[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_uint64){
              if(!flg_mss  || op2.ui64p[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.ui64p[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.ui64p[idx_op2]=mss_val_uint64;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_BYTE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.bp[idx_op2]=op1.bp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.bp[idx_op2] < op1.bp[blk_off+idx_blk]) op2.bp[idx_op2]=op1.bp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.bp[idx_op1] != mss_val_byte){
            if(!flg_mss || op2.bp[idx_op2] < op1.bp[idx_op1]) op2.bp[idx_op2]=op1.bp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.bp[idx_op2]=mss_val_byte;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_byte op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.bp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.bp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.bp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.bp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_byte){
              if(!flg_mss  || op2.bp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.bp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.bp[idx_op2]=mss_val_byte;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_UBYTE:
#define FXM_NCO315 1
#ifdef FXM_NCO315
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.ubp[idx_op2]=op1.ubp[blk_off];
        for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
          if(op2.ubp[idx_op2] < op1.ubp[blk_off+idx_blk]) op2.ubp[idx_op2]=op1.ubp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
        for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
          idx_op1=blk_off+idx_blk;
          if(op1.ubp[idx_op1] != mss_val_ubyte){
            if(!flg_mss || op2.ubp[idx_op2] < op1.ubp[idx_op1]) op2.ubp[idx_op2]=op1.ubp[idx_op1];
            flg_mss=True;
          } /* end if */
        } /* end loop over idx_blk */
        if(!flg_mss) op2.ubp[idx_op2]=mss_val_ubyte;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      nco_ubyte op1_2D[sz_op2][sz_blk];
      (void)memcpy((void *)op1_2D,(void *)(op1.ubp),sz_op1*nco_typ_lng(type));
      if(!has_mss_val){
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          op2.ubp[idx_op2]=op1_2D[idx_op2][0];
          for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
            if(op2.ubp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.ubp[idx_op2]=op1_2D[idx_op2][idx_blk];
        } /* end loop over idx_op2 */
      }else{
        for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
          flg_mss=False;
          for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
            if(op1_2D[idx_op2][idx_blk] != mss_val_ubyte){
              if(!flg_mss  || op2.ubp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.ubp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
              flg_mss=True;
            } /* end if */
          } /* end loop over idx_blk */
          if(!flg_mss) op2.ubp[idx_op2]=mss_val_ubyte;
        } /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Only allow ANSI-compliant branch. "max" is assigned value of first element of block */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.cp[idx_op2]=op1.cp[blk_off];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
	idx_op1=blk_off;
	if(op1.cp[idx_op1] != mss_val_char){
	  if(!flg_mss) op2.cp[idx_op2]=op1.cp[idx_op1];
	  flg_mss=True;
	} /* end if */
        if(!flg_mss) op2.cp[idx_op2]=mss_val_char;
      } /* end loop over idx_op2 */
    } /* end else */
    break;
  case NC_STRING: 
    /* Only allow ANSI-compliant branch. "max" is assigned value of first element of block */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        op2.sngp[idx_op2]=op1.sngp[blk_off];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
        const long blk_off=idx_op2*sz_blk;
        flg_mss=False;
	idx_op1=blk_off;
	if(op1.sngp[idx_op1] != mss_val_string){
	  if(!flg_mss) op2.sngp[idx_op2]=op1.sngp[idx_op1];
	  flg_mss=True;
	} /* end if */
        if(!flg_mss) op2.sngp[idx_op2]=mss_val_string;
      } /* end loop over idx_op2 */
    } /* end else */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end  switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end nco_var_avg_rdc_max() */
