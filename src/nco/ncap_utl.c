/* $Header$ */

/* Purpose: netCDF arithmetic processor */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include <assert.h>
#include "ncap_utl.h" /* netCDF arithmetic processor */
extern char ncap_err_sng[200]; /* [sng] Buffer for error string (declared in ncap_lex.l) */
/* Global variables */
extern size_t ncap_ncl_dpt_crr; /* [nbr] Depth of current #include file (declared in ncap.c) */
extern size_t *ncap_ln_nbr_crr; /* [cnt] Line number (declared in ncap.c) */
extern char **ncap_fl_spt_glb; /* [fl] Script file (declared in ncap.c) */



var_sct *
/* fxm: which prototype to use? */
/* ncap_var_init(const char * const var_nm,prs_sct *prs_arg) */
ncap_var_init(char *var_nm,prs_sct *prs_arg)
{
  /* Purpose: Initialize variable structure, retrieve variable values from disk
     Parser calls ncap_var_init() when it encounters a new RHS variable */
  /* const char fnc_nm[]="ncap_var_init()"; *//* [sng] Function name */
  
  int idx;
  int jdx;
  int dmn_var_nbr;
  int *dim_id=NULL;
  int var_id;
  int rcd;
  int fl_id;
  
  nco_bool DEF_VAR=False;
  
  dmn_sct *dmn_in; 
  dmn_sct **dim_new=NULL_CEWI;
  
  var_sct var_lkp;
  var_sct *var;
  var_sct *var_lst;  
  
  /* Several possiblilties here:
     var NOT in I or O
     var in I
     var in O (defined and filled)
     var in 0 (defined but empty)
     
     Must handle case var in I, var in O (defined & empty) with care
     This occurs when var is on LHS and RHS, e.g., two=two^6
     
     INITIAL SCAN
     check var list for var -- 
     if present fill with nulls and return
     Not present check output then input
     
     FINAL SCAN
     check var list for var -- 
     if present & defined + filled return var from output
     if present & defined + unfilled reurn var from input if defined
     Not present check output then input */     
  
  var_lkp.nm=strdup(var_nm);
  var_lst=ncap_var_lookup(&var_lkp,((prs_sct*)prs_arg),False);
  (void)nco_free(var_lkp.nm);
  
  if(prs_arg->ntl_scn && var_lst){
    var=nco_var_dpl(var_lst);
    var->val.vp=NULL;
    return var;	
  } /* endif */
  
  /* Check if var in list has been defined but NOT filled */
  if(!prs_arg->ntl_scn && var_lst && var_lst->sz >0 ) DEF_VAR=True; 
  
  /* Check output file for var */  
  rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
  if(rcd == NC_NOERR && !DEF_VAR){
    fl_id=prs_arg->out_id;
  }else{
    /* Check input file for ID */
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
    if(rcd != NC_NOERR){
      /* Return NULL if variable not in input or output file */
      (void)fprintf(stderr,"WARNING unable to find %s in %s or %s\n",var_nm,prs_arg->fl_in,prs_arg->fl_out);    
      return (var_sct *)NULL;
    } /* end if */
    
    /* Find dimensions used in var
       Learn which are not already in output list prs_arg->dmn_out and output file
       Add these to output list and output file */
    (void)nco_redef(prs_arg->out_id);
    fl_id=prs_arg->in_id;
    
    (void)nco_inq_varndims(fl_id,var_id,&dmn_var_nbr);
    if(dmn_var_nbr>0){
      dim_id=(int *)nco_malloc(dmn_var_nbr*sizeof(int));
      
      (void)nco_inq_vardimid(fl_id,var_id,dim_id);
      for(idx=0;idx<dmn_var_nbr;idx++) 
	for(jdx=0;jdx<prs_arg->nbr_dmn_in;jdx++){
	  
	  /* De-reference */
	  dmn_in=prs_arg->dmn_in[jdx];
	  if(dim_id[idx] != dmn_in->id || dmn_in->xrf) continue;
	  
	  /* Define new dimension in (prs_arg->dmn_out) */ 
	  dim_new=nco_dmn_out_grow(prs_arg);
	  *dim_new=nco_dmn_dpl(dmn_in);
	  (void)nco_dmn_xrf(*dim_new,dmn_in);
	  /* Write new dimension to output file */
	  (void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,dim_new,1);
	  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: DEBUG Found new dimension %s in input variable %s in file %s. Defining dimension %s in output file %s\n",nco_prg_nm_get(),(*dim_new)->nm,var_nm,prs_arg->fl_in,(*dim_new)->nm,prs_arg->fl_out);
	  break;
	} /* end loop over dimensions in current output dimension list */
      (void)nco_free(dim_id);
    } /* end loop over dimension in current input variable */
    (void)nco_enddef(prs_arg->out_id); 
    
  } /* end else */
  
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: parser VAR action called ncap_var_init() to retrieve %s from disk\n",nco_prg_nm_get(),var_nm);
  var=nco_var_fll(fl_id,var_id,var_nm,*(prs_arg->dmn_out),*(prs_arg->nbr_dmn_out));
  /*  var->nm=(char *)nco_malloc((strlen(var_nm)+1UL)*sizeof(char));
      (void)strcpy(var->nm,var_nm); */
  
  /* Tally is not required yet since ncap does not perform cross-file operations (yet) */
  /* var->tally=(long *)nco_malloc_dbg(var->sz*sizeof(long int),"Unable to malloc() tally buffer in variable initialization",fnc_nm);
     (void)nco_zero_long(var->sz,var->tally); */
  var->tally=(long *)NULL;
  
  /* Retrieve variable values from disk into memory */
  if(prs_arg->ntl_scn) {
    var->val.vp=(void*)NULL;
  }else{
    (void)nco_var_get(fl_id,var);
  } /* end else */
  /* (void)nco_var_free(var_nm);*/
  /* (void)nco_free(var_nm->nm);*/
  /* var=nco_var_upk(var); */
  return var;
} /* end ncap_var_init() */

dmn_sct **
nco_dmn_out_grow
(prs_sct * prs_arg)
{
  /* Purpose: Expand dimension list by one and return pointer to newly created member */
  
  int *sz;
  sz=prs_arg->nbr_dmn_out;
  
  *(prs_arg->dmn_out)=(dmn_sct **)nco_realloc(*(prs_arg->dmn_out),(++*sz)*sizeof(dmn_sct *));
  
  return (*(prs_arg->dmn_out)+(*sz-1));
} /* end nco_dmn_out_grow() */

int 
ncap_var_write
(var_sct *var,
 prs_sct *prs_arg)
{
  /* Purpose: Define variable in output file and write variable */
  
  /*  const char mss_val_sng[]="missing_value"; *//* [sng] Unidata standard string for missing value */
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  
  int rcd; /* [rcd] Return code */
  int var_out_id;
  
  var_sct *ptr_var;
  var_sct *var_dpl;
  nco_bool DEF_VAR; /* True if var has been defined in O in initial scan */
  
#ifdef NCO_RUSAGE_DBG
  long maxrss; /* [B] Maximum resident set size */
#endif /* !NCO_RUSAGE_DBG */
  
  /* If inital scan duplicate then save in list, free val.vp */
  if(prs_arg->ntl_scn){
    var->val.vp=nco_free(var->val.vp);
    var_dpl=nco_var_dpl(var);
    assert(var_dpl->nm);
    if(ncap_var_lookup(var_dpl,((prs_sct*)prs_arg),True))
      (void)fprintf(stdout,"%s: variable %s defined\n",nco_prg_nm_get(),var->nm);
    (void)nco_var_free(var);
    return True;
  } /* endif ntl_scn */
  
  /* Check if var is in table AND has been defined
     NB: var and ptr_var are different */
  ptr_var=ncap_var_lookup(var,((prs_sct*)prs_arg),False);
  DEF_VAR=(ptr_var && ptr_var->sz > 0 ? True : False);
  
  rcd=nco_inq_varid_flg(((prs_sct *)prs_arg)->out_id,var->nm,&var_out_id);
  
  if(!DEF_VAR){ 
    /* Check to see if variable has already been defined and written */
    if(rcd == NC_NOERR){
      (void)sprintf(ncap_err_sng,"Warning: Variable %s has aleady been saved in %s",var->nm,((prs_sct *)prs_arg)->fl_out);
      (void)nco_yyerror(prs_arg,ncap_err_sng);
      var = nco_var_free(var);
      return False;
    }
  } /* DEF_VAR */
  
  if(DEF_VAR && (var->pck_ram || var->has_mss_val)){
    /* Put file in define mode to allow metadata writing */
    (void)nco_redef(prs_arg->out_id);
    
    /* Put missing value */  
    if(var->has_mss_val) (void)nco_put_att(prs_arg->out_id,var_out_id,nco_mss_val_sng_get(),var->type,1,var->mss_val.vp);
    
    /* Write/overwrite scale_factor and add_offset attributes */
    if(var->pck_ram){ /* Variable is packed in memory */
      if(var->has_scl_fct) (void)nco_put_att(prs_arg->out_id,var_out_id,scl_fct_sng,var->typ_upk,1,var->scl_fct.vp);
      if(var->has_add_fst) (void)nco_put_att(prs_arg->out_id,var_out_id,add_fst_sng,var->typ_upk,1,var->add_fst.vp);
    } /* endif pck_ram */
    
    /* Take output file out of define mode */
    (void)nco_enddef(prs_arg->out_id);
  } /* endif DEF_VAR... */
  
  if(!DEF_VAR){
    /* Put file in define mode to allow metadata writing */
    (void)nco_redef(prs_arg->out_id);
    
    /* Define variable */   
    (void)nco_def_var(prs_arg->out_id,var->nm,var->type,var->nbr_dim,var->dmn_id,&var_out_id);
    /* Set HDF Lempel-Ziv compression level, if requested */
    if(prs_arg->dfl_lvl >= 0 && var->nbr_dim > 0) (void)nco_def_var_deflate(prs_arg->out_id,var_out_id,(int)NC_SHUFFLE,(int)True,prs_arg->dfl_lvl);    
    /* Set chunk sizes, if requested */
    if(prs_arg->cnk_sz && var->nbr_dim > 0) (void)nco_def_var_chunking(prs_arg->out_id,var_out_id,(int)NC_CHUNKED,prs_arg->cnk_sz);
    
    /* Put missing value */  
    if(var->has_mss_val) (void)nco_put_att(prs_arg->out_id,var_out_id,nco_mss_val_sng_get(),var->type,1,var->mss_val.vp);
    
    /* Write/overwrite scale_factor and add_offset attributes */
    if(var->pck_ram){ /* Variable is packed in memory */
      if(var->has_scl_fct) (void)nco_put_att(prs_arg->out_id,var_out_id,scl_fct_sng,var->typ_upk,1,var->scl_fct.vp);
      if(var->has_add_fst) (void)nco_put_att(prs_arg->out_id,var_out_id,add_fst_sng,var->typ_upk,1,var->add_fst.vp);
    } /* endif pck_ram */
    
      /* Take output file out of define mode */
    (void)nco_enddef(prs_arg->out_id);
  } /* end if */
  
  /* Write variable */ 
  if(var->nbr_dim == 0){
    (void)nco_put_var1(prs_arg->out_id,var_out_id,0L,var->val.vp,var->type);
  }else{
    (void)nco_put_vara(prs_arg->out_id,var_out_id,var->srt,var->cnt,var->val.vp,var->type);
  } /* end else */
  
#ifdef NCO_RUSAGE_DBG
  /* Compile: cd ~/nco/bld;make 'USR_TKN=-DNCO_RUSAGE_DBG';cd - */
  /* Print rusage memory usage statistics */
  if(nco_dbg_lvl_get() >= 0) (void)fprintf(stdout,"%s: INFO ncap_var_write() writing variable %s\n",nco_prg_nm_get(),var->nm);
  maxrss=nco_mmr_usg_prn((int)0);
#endif /* !NCO_RUSAGE_DBG */
  
  /* Free varible */
  var=nco_var_free(var);
  
  /* Use sz to keep track of defined and written variables */
  if(DEF_VAR) ptr_var->sz=-1;
  
  return rcd;
} /* end ncap_var_write() */

sym_sct *
ncap_sym_init
(const char * const sym_nm,
 double (*fnc_dbl)(double),
 float (*fnc_flt)(float))
{ 
  /* Purpose: Allocate space for sym_sct then initialize */
  sym_sct *symbol;
  symbol=(sym_sct *)nco_malloc(sizeof(sym_sct));
  symbol->nm=(char *)strdup(sym_nm);
  symbol->fnc_dbl=fnc_dbl;
  symbol->fnc_flt=fnc_flt;
  return symbol;
} /* end ncap_sym_init() */

ptr_unn
ncap_scv_2_ptr_unn
(scv_sct scv)
{
  /* Purpose: Convert scv_sct to ptr_unn
     malloc() appropriate space for single type
     NB: Does not work on strings */
  ptr_unn val;
  nc_type type=scv.type;
  val.vp=(void *)nco_malloc(nco_typ_lng(type));
  (void)cast_void_nctype(type,&val);
  
  switch(type){
  case NC_FLOAT: *val.fp=scv.val.f; break;
  case NC_DOUBLE: *val.dp=scv.val.d; break;
  case NC_INT: *val.ip=scv.val.i; break;
  case NC_SHORT: *val.sp=scv.val.s; break;
  case NC_USHORT: *val.usp=scv.val.us; break;
  case NC_UINT: *val.uip=scv.val.ui; break;
  case NC_INT64: *val.i64p=scv.val.i64; break;
  case NC_UINT64: *val.ui64p=scv.val.ui64; break;
  case NC_BYTE: *val.bp=scv.val.b; break;
  case NC_UBYTE: *val.ubp=scv.val.ub; break;
  case NC_CHAR: break; /* do nothing */
  case NC_STRING: break; /* do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  (void)cast_nctype_void(type,&val);
  return val;
} /* end ncap_scv_2_ptr_unn() */


scv_sct
ncap_scv_abs(scv_sct scv)
{
  /* Purpose: Find the absolute value of a scalar value */
  
#ifndef __GNUG__ 
  extern float fabsf(float); /* Sun math.h does not include fabsf() prototype */
#endif   
  
  scv_sct scv_out;
  scv_out.type=scv.type;
  
  switch(scv.type){ 
  case NC_FLOAT: scv_out.val.f=fabsf(scv.val.f); break;
  case NC_DOUBLE: scv_out.val.d=fabs(scv.val.d); break;
  case NC_INT: scv_out.val.i=labs(scv.val.i); break; /* int abs(int), long labs(long int) */ break;            
  case NC_SHORT: scv_out.val.s=((scv.val.s >= 0) ? scv.val.s : -scv.val.s); break;
  case NC_USHORT: scv_out.val.us=scv.val.us; break;
  case NC_UINT: scv_out.val.ui=scv.val.ui; break;
  case NC_INT64: scv_out.val.i64=llabs(scv.val.i64); break;
  case NC_UINT64: scv_out.val.ui64=scv.val.ui64; break;
  case NC_BYTE: scv_out.val.b=((scv.val.b >= 0) ? scv.val.b : -scv.val.b); break;
  case NC_UBYTE: scv_out.val.ub=scv.val.ub; break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;    
  } /* end switch */
  return scv_out;
} /* end ncap_scv_abs */

int 
ncap_scv_minus(scv_sct *scv)
{
  /* Purpose: Return negative of input */
  switch(scv->type){ 
  case NC_FLOAT: scv->val.f=-scv->val.f; break;
  case NC_DOUBLE: scv->val.d=-scv->val.d; break;
  case NC_INT: scv->val.i=-scv->val.i; break;            
  case NC_SHORT: scv->val.s=-scv->val.s; break;
  case NC_INT64: scv->val.i64=-scv->val.i64; break;
  case NC_BYTE: scv->val.b=-scv->val.b; break;
  case NC_USHORT: /* NB: Unsigned */
  case NC_UINT: /* NB: Unsigned */
  case NC_UINT64: /* NB: Unsigned */
  case NC_UBYTE: /* NB: Unsigned */
    (void)fprintf(stdout,"%s: ERROR ncap_scr_minus() reports attempt to convert unsigned integer type to a negative number\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;  
  } /* end switch */    
  return scv->type;
} /* end ncap_scv_minus() */



nco_bool /* O [flg] Variables now conform */
ncap_var_stretch /* [fnc] Stretch variables */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2) /* I/O [ptr] Second variable */
{
  /* Purpose: Make input variables conform or die
     var_1 and var_2 are considered completely symmetrically
     No assumption is made about var_1 relative to var_2
     Main difference betwee ncap_var_stretch() and nco_var_cnf_dmn() is
     If variables conform, then ncap_var_stretch() will broadcast
     If variables share no dimensions, then ncap_var_stretch() will convolve
     
     Terminology--- 
     Broadcast: Inflate smaller conforming variable to larger variable
     Conform: Dimensions of one variable are subset of other variable
     Convolve: Construct array whose rank is sum of non-duplicated dimensions
     Stretch: Union of broadcast and convolve
     
     Logic is pared down version of nco_var_cnf_dmn()
     1. USE_DUMMY_WGT has been eliminated: 
     ncap has no reason not to stretch input variables because grammar
     ensures only arithmetic variables will be stretched.
     
     2. wgt_crr has been eliminated:
     ncap never does anything multiple times so no equivalent to wgt_crr exists
     
     3. ncap_var_stretch(), unlike nco_var_cnf_dmn(), performs memory management
     Variables are var_free'd if they are superceded (replaced)
     
     4. Conformance logic is duplicated from nco_var_cnf_dmn()
     var_gtr plays role of var
     var_lsr plays role of wgt
     var_lsr_out plays role of wgt_out
     var_lsr_out=var_lsr only if variables already conform
     var_gtr_out is required since both variables may change
     var_gtr_out=var_gtr unless convolution is required */
  
  nco_bool CONFORMABLE=False; /* [flg] Whether var_lsr can be made to conform to var_gtr */
  nco_bool CONVOLVE=False; /* [flg] var_1 and var_2 had to be convolved */
  nco_bool DO_CONFORM; /* [flg] Did var_1 and var_2 conform? */
  nco_bool MUST_CONFORM=False; /* [flg] Must var_1 and var_2 conform? */
  
  int idx;
  int idx_dmn;
  int var_lsr_var_gtr_dmn_shr_nbr=0; /* [nbr] Number of dimensions shared by var_lsr and var_gtr */
  
  var_sct *var_gtr=NULL; /* [ptr] Pointer to variable structure of greater rank */
  var_sct *var_lsr=NULL; /* [ptr] Pointer to variable structure to lesser rank */
  var_sct *var_gtr_out=NULL; /* [ptr] Pointer to stretched version of greater rank variable */
  var_sct *var_lsr_out=NULL; /* [ptr] Pointer to stretched version of lesser rank variable */
  
  /* Initialize flag to false. Overwrite by true after successful conformance */
  DO_CONFORM=False;
  
  /* Determine which variable is greater and which lesser rank */
  if((*var_1)->nbr_dim >= (*var_2)->nbr_dim){
    var_gtr=*var_1;
    var_lsr=*var_2;
  }else{
    var_gtr=*var_2;
    var_lsr=*var_1;
  } /* endif */
  
  /* var_gtr_out=var_gtr unless convolution is required */
  var_gtr_out=var_gtr;
  
  /* Does lesser variable (var_lsr) conform to greater variable's dimensions? */
  if(var_lsr_out == NULL){
    if(var_gtr->nbr_dim > 0){
      /* Test that all dimensions in var_lsr appear in var_gtr */
      for(idx=0;idx<var_lsr->nbr_dim;idx++){
        for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(!strcmp(var_lsr->dim[idx]->nm,var_gtr->dim[idx_dmn]->nm)){
	    var_lsr_var_gtr_dmn_shr_nbr++; /* var_lsr and var_gtr share this dimension */
	    break;
	  } /* endif */
        } /* end loop over var_gtr dimensions */
      } /* end loop over var_lsr dimensions */
      /* Decide whether var_lsr and var_gtr dimensions conform, are mutually exclusive, or are partially exclusive */ 
      if(var_lsr_var_gtr_dmn_shr_nbr == var_lsr->nbr_dim){
	/* var_lsr and var_gtr conform */
	/* fxm: Variables do not conform when dimension list of one is subset of other if order of dimensions differs, i.e., a(lat,lev,lon) !~ b(lon,lev) */
	CONFORMABLE=True;
      }else if(var_lsr_var_gtr_dmn_shr_nbr == 0){
	/* Dimensions in var_lsr and var_gtr are mutually exclusive */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %s and template %s share no dimensions\n",nco_prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  nco_exit(EXIT_FAILURE);
	}else{
	  if(nco_dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: DEBUG %s and %s share no dimensions: Attempting to convolve...\n",nco_prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      }else if(var_lsr_var_gtr_dmn_shr_nbr > 0 && var_lsr_var_gtr_dmn_shr_nbr < var_lsr->nbr_dim){
	/* Some, but not all, of var_lsr dimensions are in var_gtr */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",nco_prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr);
	  nco_exit(EXIT_FAILURE);
	}else{
	  if(nco_dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: DEBUG %d dimensions of %s belong to template %s but %d dimensions do not: Not broadcasting %s to %s, could attempt stretching???\n",nco_prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      } /* end if */
      if(CONFORMABLE){
	if(var_gtr->nbr_dim == var_lsr->nbr_dim){
	  /* var_gtr and var_lsr conform and are same rank */
	  /* Test whether all var_lsr and var_gtr dimensions match in sequence */
	  for(idx=0;idx<var_gtr->nbr_dim;idx++){
	    if(strcmp(var_lsr->dim[idx]->nm,var_gtr->dim[idx]->nm)) break;
	  } /* end loop over dimensions */
	  /* If so, take shortcut and copy var_lsr to var_lsr_out */
	  if(idx == var_gtr->nbr_dim) DO_CONFORM=True;
	}else{
	  /* var_gtr and var_lsr conform but are not same rank, set flag to proceed to generic conform routine */
	  DO_CONFORM=False;
	} /* end else */
      } /* endif CONFORMABLE */
    }else{ /* nbr_dmn == 0 */
      /* var_gtr is scalar, if var_lsr is also then set flag to copy var_lsr to var_lsr_out else proceed to generic conform routine */
      if(var_lsr->nbr_dim == 0) DO_CONFORM=True; else DO_CONFORM=False;
    } /* end else nbr_dmn == 0 */
    if(CONFORMABLE && DO_CONFORM){
      var_lsr_out=nco_var_dpl(var_lsr);
      (void)nco_xrf_var(var_lsr,var_lsr_out);
    } /* end if */
  } /* endif var_lsr_out == NULL */
  
  if(var_lsr_out == NULL && CONVOLVE){
    /* Convolve variables by returned stretched variables with minimum possible number of dimensions */
    int dmn_nbr; /* Number of dimensions in convolution */
    if(nco_dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: WARNING Convolution not yet implemented, results of operation between %s and %s are unpredictable\n",nco_prg_nm_get(),var_lsr->nm,var_gtr->nm);
    /* Dimensions in convolution are union of dimensions in variables */
    dmn_nbr=var_lsr->nbr_dim+var_gtr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr; /* Number of dimensions in convolution */
    dmn_nbr=dmn_nbr+0; /* CEWI: Avert compiler warning that variable is set but never used */
    /* fxm: these should go away soon */
    var_lsr_out=nco_var_dpl(var_lsr);
    var_gtr_out=nco_var_dpl(var_gtr);
    
    /* for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){;}
       if(var_lsr_var_gtr_dmn_shr_nbr == 0); else; */
    
    /* Free calling variables */
    var_lsr=nco_var_free(var_lsr);
    var_gtr=nco_var_free(var_gtr);
  } /* endif STRETCH */
  
  if(var_lsr_out == NULL){
    /* Expand lesser variable (var_lsr) to match size of greater variable */
    const char fnc_nm[]="ncap_var_stretch()"; /* [sng] Function name */
    char *var_lsr_cp;
    char *var_lsr_out_cp;
    
    int idx_var_lsr_var_gtr[NC_MAX_VAR_DIMS];
    int var_lsr_nbr_dim;
    int var_gtr_nbr_dmn_m1;
    
    long *var_gtr_cnt;
    long dmn_ss[NC_MAX_VAR_DIMS];
    long dmn_var_gtr_map[NC_MAX_VAR_DIMS];
    long dmn_var_lsr_map[NC_MAX_VAR_DIMS];
    long var_gtr_lmn;
    long var_lsr_lmn;
    long var_gtr_sz;
    
    size_t var_lsr_typ_sz;
    
    /* Copy main attributes of greater variable into lesser variable */
    var_lsr_out=nco_var_dpl(var_gtr);
    (void)nco_xrf_var(var_lsr,var_lsr_out);
    
    /* Modify elements of lesser variable array */
    var_lsr_out->nm=(char *)nco_free(var_lsr_out->nm);
    var_lsr_out->nm=(char *)strdup(var_lsr->nm);
    var_lsr_out->id=var_lsr->id;
    var_lsr_out->type=var_lsr->type;
    /* Added 20050323: 
       Not quite sure why, but var->val.vp may already have values here when LHS-casting
       Perform safety free to guard against memory leaks */
    var_lsr_out->val.vp=nco_free(var_lsr_out->val.vp);
    var_lsr_out->val.vp=(void *)nco_malloc_dbg(var_lsr_out->sz*nco_typ_lng(var_lsr_out->type),"Unable to malloc() value buffer in variable stretching",fnc_nm);
    var_lsr_cp=(char *)var_lsr->val.vp;
    var_lsr_out_cp=(char *)var_lsr_out->val.vp;
    var_lsr_typ_sz=nco_typ_lng(var_lsr_out->type);
    
    if(var_lsr_out->nbr_dim == 0){
      /* Variables are scalars, not arrays */
      (void)memcpy(var_lsr_out_cp,var_lsr_cp,var_lsr_typ_sz);
    }else if(var_lsr->nbr_dim == 0){
      /* Lesser-ranked input variable is scalar 
	 Expansion in this degenerate case needs no index juggling (reverse-mapping)
	 Code as special case to speed-up important applications of ncap
	 for synthetic file creation */
      var_gtr_sz=var_gtr->sz;
      for(var_gtr_lmn=0;var_gtr_lmn<var_gtr_sz;var_gtr_lmn++){
	(void)memcpy(var_lsr_out_cp+var_gtr_lmn*var_lsr_typ_sz,var_lsr_cp,var_lsr_typ_sz);
      } /* end loop over var_gtr_lmn */
    }else{
      /* Variables are arrays, not scalars */
      
      /* Create forward and reverse mappings from greater variable's dimensions to lesser variable's dimensions:
	 
      dmn_var_gtr_map[i] is number of elements between one value of i_th 
      dimension of greater variable and next value of i_th dimension, i.e., 
      number of elements in memory between indicial increments in i_th dimension. 
      This is computed as product of one (1) times size of all dimensions (if any) after i_th 
      dimension in greater variable.
      
      dmn_var_lsr_map[i] contains analogous information, except for lesser variable
      
      idx_var_lsr_var_gtr[i] contains index into greater variable's dimensions of i_th dimension of lesser variable
      idx_var_gtr_var_lsr[i] contains index into lesser variable's dimensions of i_th dimension of greater variable 
      
      Since lesser variable is a subset of greater variable, some elements of idx_var_gtr_var_lsr may be "empty", or unused
      
      Since mapping arrays (dmn_var_gtr_map and dmn_var_lsr_map) are ultimately used for a
      memcpy() operation, they could (read: should) be computed as byte offsets, not type offsets.
      This is why netCDF generic hyperslab routines (ncvarputg(), ncvargetg())
      request imap vector to specify offset (imap) vector in bytes. */
      for(idx=0;idx<var_lsr->nbr_dim;idx++){
	for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(!strcmp(var_gtr->dim[idx_dmn]->nm,var_lsr->dim[idx]->nm)){
	    idx_var_lsr_var_gtr[idx]=idx_dmn;
	    /*	    idx_var_gtr_var_lsr[idx_dmn]=idx;*/
	    break;
	  } /* end if */
	  /* Sanity check */
	  if(idx_dmn == var_gtr->nbr_dim-1){
	    (void)fprintf(stdout,"%s: ERROR var_lsr %s has dimension %s but var_gtr %s does not deep in ncap_var_stretch()\n",nco_prg_nm_get(),var_lsr->nm,var_lsr->dim[idx]->nm,var_gtr->nm);
	    nco_exit(EXIT_FAILURE);
	  } /* end if err */
	} /* end loop over greater variable dimensions */
      } /* end loop over lesser variable dimensions */
      
      /* Figure out map for each dimension of greater variable */
      for(idx=0;idx<var_gtr->nbr_dim;idx++) dmn_var_gtr_map[idx]=1L;
      for(idx=0;idx<var_gtr->nbr_dim-1;idx++)
	for(idx_dmn=idx+1;idx_dmn<var_gtr->nbr_dim;idx_dmn++)
	  dmn_var_gtr_map[idx]*=var_gtr->cnt[idx_dmn];
      
      /* Figure out map for each dimension of lesser variable */
      for(idx=0;idx<var_lsr->nbr_dim;idx++) dmn_var_lsr_map[idx]=1L;
      for(idx=0;idx<var_lsr->nbr_dim-1;idx++)
	for(idx_dmn=idx+1;idx_dmn<var_lsr->nbr_dim;idx_dmn++)
	  dmn_var_lsr_map[idx]*=var_lsr->cnt[idx_dmn];
      
      /* Define convenience variables to avoid repetitive indirect addressing */
      var_lsr_nbr_dim=var_lsr->nbr_dim;
      var_gtr_sz=var_gtr->sz;
      var_gtr_cnt=var_gtr->cnt;
      var_gtr_nbr_dmn_m1=var_gtr->nbr_dim-1;
      
      /* var_gtr_lmn is offset into 1-D array corresponding to N-D indices dmn_ss */
      for(var_gtr_lmn=0;var_gtr_lmn<var_gtr_sz;var_gtr_lmn++){
	dmn_ss[var_gtr_nbr_dmn_m1]=var_gtr_lmn%var_gtr_cnt[var_gtr_nbr_dmn_m1];
	for(idx=0;idx<var_gtr_nbr_dmn_m1;idx++){
	  dmn_ss[idx]=(long int)(var_gtr_lmn/dmn_var_gtr_map[idx]);
	  dmn_ss[idx]%=var_gtr_cnt[idx];
	} /* end loop over dimensions */
	
	/* Map (shared) N-D array indices into 1-D index into original lesser variable data */
	var_lsr_lmn=0L;
	for(idx=0;idx<var_lsr_nbr_dim;idx++) var_lsr_lmn+=dmn_ss[idx_var_lsr_var_gtr[idx]]*dmn_var_lsr_map[idx];
	
	(void)memcpy(var_lsr_out_cp+var_gtr_lmn*var_lsr_typ_sz,var_lsr_cp+var_lsr_lmn*var_lsr_typ_sz,var_lsr_typ_sz);
	
      } /* end loop over var_gtr_lmn */
      
    } /* end if greater variable (and lesser variable) are arrays, not scalars */
    
    DO_CONFORM=True;
  } /* end if we had to broadcast lesser variable to fit greater variable */
  
  /* Place variables in original order
     Not necessary if variables are used in binary operations that are associative
     But do not want to require that assumption of calling routines */
  if((*var_1)->nbr_dim >= (*var_2)->nbr_dim){
    *var_1=var_gtr_out; /* [ptr] First variable */
    *var_2=var_lsr_out; /* [ptr] Second variable */
  }else{
    *var_1=var_lsr_out; /* [ptr] First variable */
    *var_2=var_gtr_out; /* [ptr] Second variable */
  } /* endif */
  
  /* Variables now conform */
  return DO_CONFORM;
} /* end ncap_var_stretch() */



int /* [rcd] Return code */
nco_yyerror /* [fnc] Print error/warning/info messages generated by parser */
(prs_sct *prs_arg, const char * const err_sng_lcl) /* [sng] Message to print */
{
  /* Purpose: Print error/warning/info messages generated by parser
     Use eprokoke_skip to skip error message after sending error message from yylex()
     Stop provoked error message from yyparse being printed */
  
  static nco_bool eprovoke_skip;
  
  prs_arg=prs_arg+0; /* CEWI otherwise unused parameter error */

  /* if(eprovoke_skip){eprovoke_skip=False ; return 0;} */
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: %s line %lu",nco_prg_nm_get(),ncap_fl_spt_glb[ncap_ncl_dpt_crr],(unsigned long)ncap_ln_nbr_crr[ncap_ncl_dpt_crr]);
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr," %s",err_sng_lcl);
    (void)fprintf(stderr,"\n");
    (void)fflush(stderr);
  } /* endif dbg */
  
  if(err_sng_lcl[0] == '#') eprovoke_skip=True;
  eprovoke_skip=eprovoke_skip+0; /* CEWI Do nothing except avoid compiler warnings */
  return 0;
} /* end yyerror() */

aed_sct *  /* O [idx] Location of attribute in list */
ncap_aed_lookup /* [fnc] Find location of existing attribute or add new attribute */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const att_nm, /* I [sng] Attribute name */
 prs_sct *  prs_arg,   /* contains attribute list */       
 const nco_bool update) /* I [flg] Delete existing value or add new attribute to list */
{
  int idx;
  int size;
  aed_sct *ptr_aed;
  
  size=*(prs_arg->nbr_att);
  
  for(idx=0;idx<size;idx++){
    ptr_aed=(*(prs_arg->att_lst))[idx];
    
    if(strcmp(ptr_aed->att_nm,att_nm) || strcmp(ptr_aed->var_nm,var_nm)) 
      continue; 
    
    if(update) ptr_aed->val.vp=nco_free(ptr_aed->val.vp);   
    /* Return pointer to list element */
    return ptr_aed;
    
  } /* end for */
  
  if(!update) return (aed_sct *)NULL;
  
  *(prs_arg->att_lst)=(aed_sct **)nco_realloc(*(prs_arg->att_lst),(size+1)*sizeof(aed_sct*));
  ++*(prs_arg->nbr_att);
  (*(prs_arg->att_lst))[size]=(aed_sct *)nco_malloc(sizeof(aed_sct));
  (*(prs_arg->att_lst))[size]->var_nm=strdup(var_nm);
  (*(prs_arg->att_lst))[size]->att_nm=strdup(att_nm);
  
  return (*(prs_arg->att_lst))[size];
} /* end ncap_aed_lookup() */

var_sct * /*I [sct] varibale in list */
ncap_var_lookup
(var_sct *var, /* I  [sct] variable  */
 prs_sct *prs_arg, /* I/O [sct] contains var list */
 const nco_bool add) /* I if not in list then add to list */          
{
  int idx;
  int size;
  
  var_sct *ptr_var; 
  
  size = *(prs_arg->nbr_var);
  
  for(idx=0;idx<size;idx++){
    
    ptr_var=(*(prs_arg->var_lst))[idx];
    /*
      assert(var->nm);
      assert(ptr_var->nm);
      if(!strcmp(var->nm,ptr_var->nm)) return ptr_var;    
    */
    if(ptr_var==NULL || strcmp(var->nm,ptr_var->nm) ) continue;
    
    return ptr_var;
  } /* end loop over idx */
  
  if(!add) return (var_sct *)NULL;
  
  *(prs_arg->var_lst)=(var_sct **)nco_realloc(*(prs_arg->var_lst),(size+1)*sizeof(var_sct*));
  ++*(prs_arg->nbr_var);
  (*(prs_arg->var_lst))[size]=var;
  
  return (var_sct *)NULL;
} /* end ncap_var_lookup() */


