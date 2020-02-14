/* $Header$ */

/* Purpose: Conform dimensions between variables */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_cnf_dmn.h" /* Conform dimensions */

var_sct * /* O [sct] Pointer to conforming variable structure */
nco_var_cnf_dmn /* [fnc] Stretch second variable to match dimensions of first variable */
(const var_sct * const var, /* I [ptr] Pointer to variable structure to serve as template */
 var_sct * const wgt, /* I [ptr] Pointer to variable structure to make conform to var */
 var_sct *wgt_crr, /* I/O [ptr] Pointer to existing conforming variable structure, if any (destroyed when does not conform to var) */
 const nco_bool MUST_CONFORM, /* I [flg] Must wgt and var conform? */
 nco_bool *DO_CONFORM) /* O [flg] Do wgt and var conform? */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* fxm: TODO 226. Is xrf in nco_var_cnf_dmn() really necessary? If not, remove it and make wgt arg const var_sct * const */

  /* Purpose: Stretch second variable to match dimensions of first variable
     Dimensions in var which are not in wgt will be present in wgt_out, with values
     replicated from existing dimensions in wgt.
     By default, wgt's dimensions must be subset of var's dimensions (MUST_CONFORM=true)
     Calling routine should set MUST_CONFORM=false if wgt and var need not conform
     When wgt and var do not conform then then nco_var_cnf_dmn sets *DO_CONFORM=False and returns copy of var with all values set to 1.0
     Calling procedure then decides what to do with unity output
     MUST_CONFORM is True for ncbo: Variables of like name to be, e.g., differenced, must conform
     MUST_CONFORM is False false for ncap, ncflint, ncwa: Some variables to be averaged may not conform to specified weight, e.g., lon will not conform to gw. This is fine and returned wgt_out may be discarded. */

  /* There are many inelegant ways to accomplish this (without using C++): */  

  /* Perhaps most efficient method in general case is to expand weight array until
     it is same size as variable array, and then multiply these arrays together 
     element-by-element in highly vectorized loop (possibly in Fortran or BLAS). 
     To enhance speed, (enlarged) weight-values array could be static, only re-made
     when dimensions of incoming variables change. */

  /* Another general method, though more expensive, is to use C to figure out the 
     multidimensional indices into the one dimensional hyperslab, a la ncks. 
     Knowing these indices, routine could loop over the one-dimensional array
     element by element, choosing the appropriate index into the weight array from 
     those same multidimensional indices. 
     This method can also create a static weight-value array that is only destroyed 
     when an incoming variable changes dimensions from the previous variable. */

  /* Another method, which is not completely general, but which may be good enough for
     governement work, is to create Fortran subroutines which expect variables of 
     a given number of dimensions as input. 
     Creating these functions for up to five dimensions would satisfy most situations
     C code would determine which branch to call based on number of dimensions
     C++ or Fortran9x overloading could construct this interface more elegantly */

  /* An (untested) simplification to some of these methods is to copy the 1-D array
     value pointer of variable and cast it to an N-D array pointer
     Then C could handle indexing 
     This method easily produce working, but non-general code
     Implementation would require ugly branches or hard-to-understand recursive function calls */
  
  /* Routine assumes weight will never have more dimensions than variable
     (otherwise which hyperslab of weight to use would be ill-defined). 
     However, weight may (and often will) have fewer dimensions than variable */

  nco_bool CONFORMABLE=False; /* [flg] wgt can be made to conform to var */
  nco_bool USE_DUMMY_WGT=False; /* [flg] Fool NCO into thinking wgt conforms to var */

  int idx; /* [idx] Counting index */
  int idx_dmn; /* [idx] Dimension index */
  int wgt_var_dmn_shr_nbr=0; /* [nbr] Number of dimensions wgt and var share */

  var_sct *wgt_out=NULL;

  /* Initialize flag to false. Overwrite by true after successful conformance */
  *DO_CONFORM=False;

  /* Does current weight (wgt_crr) conform to variable's dimensions? */
  if(wgt_crr){
    /* Test rank first because wgt_crr because of 19960218 bug (invalid dmn_id in old wgt_crr leads to match) */
    if(var->nbr_dim == wgt_crr->nbr_dim){
      /* Test whether all wgt and var dimensions match in sequence */
      for(idx=0;idx<var->nbr_dim;idx++){
	/* 20131002: nco_var_cnf_dmn() borken for groups as shown by dimension short-name strcmp() comparison here */
        if(strcmp(wgt_crr->dim[idx]->nm,var->dim[idx]->nm)) break;
      } /* end loop over dimensions */
      if(idx == var->nbr_dim) *DO_CONFORM=True;
    } /* end if ranks are equal */

    /* 20060425: Weight re-use will not occur if wgt_crr is free()'d here
       Some DDRA benchmarks need to know cost of broadcasting weights
       To turn off weight re-use and cause broadcasting, execute "else" block below
       by (temporarily) using 
              if(*DO_CONFORM && False){ ....instead of.... if(*DO_CONFORM){ 
       in following condition */
    if(*DO_CONFORM){
      wgt_out=wgt_crr;
    }else{
      wgt_crr=nco_var_free(wgt_crr);
      wgt_out=NULL;
    } /* !*DO_CONFORM */
  } /* wgt_crr == NULL */

  /* Does original weight (wgt) conform to variable's dimensions? */
  if(wgt_out == NULL){
    if(var->nbr_dim > 0){
      /* Test that all dimensions in wgt appear in var */
      for(idx=0;idx<wgt->nbr_dim;idx++){
        for(idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){
          /* Compare names, not dimension IDs */
	  /* 20131002: nco_var_cnf_dmn() borken for groups as shown by dimension short-name strcmp() comparison here */
          if(!strcmp(wgt->dim[idx]->nm,var->dim[idx_dmn]->nm)){
            wgt_var_dmn_shr_nbr++; /* wgt and var share this dimension */
            break;
          } /* endif */
        } /* end loop over var dimensions */
      } /* end loop over wgt dimensions */
      /* Decide whether wgt and var dimensions conform, are mutually exclusive, or are partially exclusive (an error) */ 
      if(wgt_var_dmn_shr_nbr == wgt->nbr_dim){
        /* wgt and var conform */
        CONFORMABLE=True;
      }else if(wgt_var_dmn_shr_nbr == 0){
        /* Dimensions in wgt and var are mutually exclusive */
        CONFORMABLE=False;
        if(MUST_CONFORM){
          (void)fprintf(stdout,"%s: ERROR %s and template %s share no dimensions\n",nco_prg_nm_get(),wgt->nm,var->nm);
          nco_exit(EXIT_FAILURE);
        }else{
          if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"\n%s: DEBUG %s and template %s share no dimensions: Not broadcasting %s to %s\n",nco_prg_nm_get(),wgt->nm,var->nm,wgt->nm,var->nm);
          USE_DUMMY_WGT=True;
        } /* endif */
      }else if(wgt->nbr_dim > var->nbr_dim){
        /* wgt is larger rank than var---no possibility of conforming */
        CONFORMABLE=False;
        if(MUST_CONFORM){
          (void)fprintf(stdout,"%s: ERROR %s is rank %d but template %s is rank %d: Impossible to broadcast\n",nco_prg_nm_get(),wgt->nm,wgt->nbr_dim,var->nm,var->nbr_dim);
          nco_exit(EXIT_FAILURE);
        }else{
          if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"\n%s: DEBUG %s is rank %d but template %s is rank %d: Not broadcasting %s to %s\n",nco_prg_nm_get(),wgt->nm,wgt->nbr_dim,var->nm,var->nbr_dim,wgt->nm,var->nm);
          USE_DUMMY_WGT=True;
        } /* endif */
      }else if(wgt_var_dmn_shr_nbr > 0 && wgt_var_dmn_shr_nbr < wgt->nbr_dim){
        /* Some, but not all, of wgt dimensions are in var */
        CONFORMABLE=False;
        if(MUST_CONFORM){
          (void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",nco_prg_nm_get(),wgt_var_dmn_shr_nbr,wgt->nm,var->nm,wgt->nbr_dim-wgt_var_dmn_shr_nbr);
          nco_exit(EXIT_FAILURE);
        }else{
          if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"\n%s: DEBUG %d dimensions of %s belong to template %s but %d dimensions do not: Not broadcasting %s to %s\n",nco_prg_nm_get(),wgt_var_dmn_shr_nbr,wgt->nm,var->nm,wgt->nbr_dim-wgt_var_dmn_shr_nbr,wgt->nm,var->nm);
          USE_DUMMY_WGT=True;
        } /* endif */
      } /* end if */
      if(USE_DUMMY_WGT){
        /* Variables do not truly conform, but this might be OK, depending on the application, so set DO_CONFORM flag to false and ... */
        *DO_CONFORM=False;
        /* ... return a dummy weight of 1.0, which allows program logic to pretend variable is weighted, but does not change answers */ 
        wgt_out=nco_var_dpl(var);
        (void)vec_set(wgt_out->type,wgt_out->sz,wgt_out->val,1.0);
      } /* endif */
      if(CONFORMABLE){
        if(var->nbr_dim == wgt->nbr_dim){
          /* var and wgt conform and are same rank */
          /* Test whether all wgt and var dimensions match in sequence */
          for(idx=0;idx<var->nbr_dim;idx++){
	    /* 20131002: nco_var_cnf_dmn() borken for groups as shown by dimension short-name strcmp() comparison here */
            if(strcmp(wgt->dim[idx]->nm,var->dim[idx]->nm)) break;
            /*	    if(wgt->dmn_id[idx] != var->dmn_id[idx]) break;*/
          } /* end loop over dimensions */
          /* If so, take shortcut and copy wgt to wgt_out */
          if(idx == var->nbr_dim) *DO_CONFORM=True;
        }else{
          /* var and wgt conform but are not same rank, set flag to proceed to generic conform routine */
          *DO_CONFORM=False;
        } /* end else */
      } /* endif CONFORMABLE */
    }else{
      /* var is scalar, if wgt is also then set flag to copy wgt to wgt_out else proceed to generic conform routine */
      if(wgt->nbr_dim == 0) *DO_CONFORM=True; else *DO_CONFORM=False;
    } /* end else */
    if(CONFORMABLE && *DO_CONFORM){
      wgt_out=nco_var_dpl(wgt);
      (void)nco_xrf_var(wgt,wgt_out);
    } /* end if */
  } /* end if */

  /* Set diagnostic DDRA information DDRA */
  /* ddra_info->wgt_brd_flg=(wgt_out == NULL) ? True : False; *//* [flg] Broadcast weight for this variable */

  if(wgt_out == NULL){
    /* Expand original weight (wgt) to match size of current variable */
    char * restrict wgt_cp;
    char * restrict wgt_out_cp;

    int idx_wgt_var[NC_MAX_VAR_DIMS];
    int wgt_nbr_dim;
    int var_nbr_dmn_m1;

    long * restrict var_cnt;
    long dmn_ss[NC_MAX_VAR_DIMS];
    long dmn_var_map[NC_MAX_VAR_DIMS];
    long dmn_wgt_map[NC_MAX_VAR_DIMS];
    long var_lmn;
    long wgt_lmn;
    long var_sz;

    size_t wgt_typ_sz;

    /* Copy main attributes of variable into output weight */
    wgt_out=nco_var_dpl(var);
    (void)nco_xrf_var(wgt,wgt_out);

    /* wgt_out variable was copied from template var
       Modify key fields so its name and type are based on wgt, not var
       wgt_out will then be hybrid between wgt and var 
       Remainder of routine fills wgt_out's var-dimensionality with wgt-values */
    wgt_out->nm=(char *)nco_free(wgt_out->nm);
    wgt_out->nm=(char *)strdup(wgt->nm);
    wgt_out->id=wgt->id;
    wgt_out->type=wgt->type;
    wgt_out->val.vp=(void *)nco_free(wgt_out->val.vp);
    wgt_out->val.vp=(void *)nco_malloc(wgt_out->sz*nco_typ_lng(wgt_out->type));
    wgt_cp=(char *)wgt->val.vp;
    wgt_out_cp=(char *)wgt_out->val.vp;
    wgt_typ_sz=nco_typ_lng(wgt_out->type);

    if(wgt_out->nbr_dim == 0){
      /* Variable (and weight) are scalars, not arrays */
      (void)memcpy(wgt_out_cp,wgt_cp,wgt_typ_sz);
    }else if(wgt->nbr_dim == 0){
      /* Lesser-ranked input variable is scalar 
	 Expansion in this degenerate case needs no index juggling (reverse-mapping)
	 Code as special case to speed-up important applications of ncap
	 for synthetic file creation */
      var_sz=var->sz;
      for(var_lmn=0;var_lmn<var_sz;var_lmn++){
        (void)memcpy(wgt_out_cp+var_lmn*wgt_typ_sz,wgt_cp,wgt_typ_sz);      
      } /* end loop over var_lmn */
    }else{
      /* Variable (and therefore wgt_out) are arrays, not scalars */
      
      /* Create forward and reverse mappings from variable's dimensions to weight's dimensions:
	 
	 dmn_var_map[i] is number of elements between one value of i_th 
	 dimension of variable and next value of i_th dimension, i.e., 
	 number of elements in memory between indicial increments in i_th dimension. 
	 This is computed as product of one (1) times size of all dimensions (if any) after i_th 
	 dimension in variable.
	 
	 dmn_wgt_map[i] contains analogous information, except for original weight variable
	 
	 idx_wgt_var[i] contains index into variable's dimensions of i_th dimension of original weight
	 idx_var_wgt[i] contains index into original weight's dimensions of i_th dimension of variable 
	 
	 Since weight is a subset of variable, some elements of idx_var_wgt may be "empty", or unused
	 
	 Since mapping arrays (dmn_var_map and dmn_wgt_map) are ultimately used for a
	 memcpy() operation, they could (read: should) be computed as byte offsets, not type offsets. 
	 This is why netCDF generic hyperslab routines (ncvarputg(), ncvargetg())
	 request imap vector to specify offset (imap) vector in bytes. */

      for(idx=0;idx<wgt->nbr_dim;idx++){
        for(idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){
          /* Compare names, not dimension IDs */
	  /* 20131002: nco_var_cnf_dmn() borken for groups as shown by dimension short-name strcmp() comparison here */
          if(!strcmp(var->dim[idx_dmn]->nm,wgt->dim[idx]->nm)){
            idx_wgt_var[idx]=idx_dmn;
            break;
          } /* end if */
          /* Sanity check */
          if(idx_dmn == var->nbr_dim-1){
            (void)fprintf(stdout,"%s: ERROR wgt %s has dimension %s but var %s does not deep in nco_var_cnf_dmn()\n",nco_prg_nm_get(),wgt->nm,wgt->dim[idx]->nm,var->nm);
            nco_exit(EXIT_FAILURE);
          } /* end if err */
        } /* end loop over variable dimensions */
      } /* end loop over weight dimensions */

      /* Figure out map for each dimension of variable */
      for(idx=0;idx<var->nbr_dim;idx++)	dmn_var_map[idx]=1L;
      for(idx=0;idx<var->nbr_dim-1;idx++)
        for(idx_dmn=idx+1;idx_dmn<var->nbr_dim;idx_dmn++)
          dmn_var_map[idx]*=var->cnt[idx_dmn];

      /* Figure out map for each dimension of weight */
      for(idx=0;idx<wgt->nbr_dim;idx++)	dmn_wgt_map[idx]=1L;
      for(idx=0;idx<wgt->nbr_dim-1;idx++)
        for(idx_dmn=idx+1;idx_dmn<wgt->nbr_dim;idx_dmn++)
          dmn_wgt_map[idx]*=wgt->cnt[idx_dmn];

      /* Define convenience variables to avoid repetitive indirect addressing */
      wgt_nbr_dim=wgt->nbr_dim;
      var_sz=var->sz;
      var_cnt=var->cnt;
      var_nbr_dmn_m1=var->nbr_dim-1;

      /* var_lmn is offset into 1-D array corresponding to N-D indices dmn_ss */
      for(var_lmn=0;var_lmn<var_sz;var_lmn++){
        /* dmn_ss are corresponding indices (subscripts) into N-D array */
        /* Operations: 1 modulo, 1 pointer offset, 1 user memory fetch
	   Repetitions: \lmnnbr
	   Total Counts: \rthnbr=2\lmnnbr, \mmrusrnbr=\lmnnbr
	   NB: LHS assumed compact and cached, counted RHS offsets and fetches only */
        dmn_ss[var_nbr_dmn_m1]=var_lmn%var_cnt[var_nbr_dmn_m1];
        for(idx=0;idx<var_nbr_dmn_m1;idx++){
          /* Operations: 1 divide, 1 modulo, 2 pointer offset, 2 user memory fetch
	     Repetitions: \lmnnbr(\dmnnbr-1)
	     Counts: \rthnbr=4\lmnnbr(\dmnnbr-1), \mmrusrnbr=2\lmnnbr(\dmnnbr-1)
	     NB: LHS assumed compact and cached, counted RHS offsets and fetches only
	     NB: Neglected loop arithmetic/compare */
          dmn_ss[idx]=(long int)(var_lmn/dmn_var_map[idx]);
          dmn_ss[idx]%=var_cnt[idx];
        } /* end loop over dimensions */
	
        /* Map (shared) N-D array indices into 1-D index into original weight data */
        wgt_lmn=0L;
        /* Operations: 1 add, 1 multiply, 3 pointer offset, 3 user memory fetch
	   Repetitions: \lmnnbr\rnkwgt
	   Counts: \rthnbr=5\lmnnbr\rnkwgt, \mmrusrnbr=3\lmnnbr\rnkwgt */
        for(idx=0;idx<wgt_nbr_dim;idx++) wgt_lmn+=dmn_ss[idx_wgt_var[idx]]*dmn_wgt_map[idx];
	
        /* Operations: 2 add, 2 multiply, 0 pointer offset, 1 system memory copy
	   Repetitions: \lmnnbr
	   Counts: \rthnbr=4\lmnnbr, \mmrusrnbr=0, \mmrsysnbr=1 */
        (void)memcpy(wgt_out_cp+var_lmn*wgt_typ_sz,wgt_cp+wgt_lmn*wgt_typ_sz,wgt_typ_sz);
	
      } /* end loop over var_lmn */

    } /* end if variable (and weight) are arrays, not scalars */

    *DO_CONFORM=True;
  } /* end if we had to stretch weight to fit variable */

  if(MUST_CONFORM && !(*DO_CONFORM)){
    (void)fprintf(stdout,"%s: ERROR Variables which MUST_CONFORM do not on exit from nco_var_cnf_dmn()\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Current weight (wgt_out) now conforms to current variable */
  return wgt_out;
  
} /* end nco_var_cnf_dmn() */

nco_bool /* [flg] var_1 and var_2 conform after processing */
ncap_var_cnf_dmn /* [fnc] Broadcast smaller variable into larger */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2) /* I/O [ptr] Second variable */
{
  /* Purpose: Conform lesser ranked to greater ranked variable, so that
     both variables are equal size on return.
     If this is possible then return true, otherwise die.
     Routine wraps nco_var_cnf_dmn(), which does the hard work
     If an input variable is replaced by a broadcast version of itself,
     then calling routine must free original version or it will leak. */

  nco_bool DO_CONFORM; /* [flg] Do var_1 and var_2 conform after processing? */
  nco_bool MUST_CONFORM=True; /* [flg] Must var_1 and var_2 conform? */
  var_sct *var_1_org; /* [ptr] Original location of var_1 */
  var_sct *var_2_org; /* [ptr] Original location of var_2 */
  var_sct *var_tmp=NULL;

  var_1_org=*var_1; /* [ptr] Original location of var_1 */
  var_2_org=*var_2; /* [ptr] Original location of var_2 */
  
  if(var_1_org->nbr_dim > var_2_org->nbr_dim){
    var_tmp=nco_var_cnf_dmn(var_1_org,var_2_org,var_tmp,MUST_CONFORM,&DO_CONFORM);
    if(var_2_org != var_tmp){
      var_2_org=nco_var_free(var_2_org);
      *var_2=var_tmp;
    } /* endif replace var_2 */
  }else{ 
    var_tmp=nco_var_cnf_dmn(var_2_org,var_1_org,var_tmp,MUST_CONFORM,&DO_CONFORM);
    if(var_1_org != var_tmp){
      var_1_org=nco_var_free(var_1_org);
      *var_1=var_tmp;
    } /* endif replace var_1 */
  } /* endif var_1 > var_2 */

  if(!DO_CONFORM){
    (void)fprintf(stderr,"%s: ncap_var_cnf_dmn() reports that variables %s and %s do not have have conforming dimensions. Cannot proceed with operation\n",nco_prg_nm_get(),(*var_1)->nm,(*var_2)->nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  return DO_CONFORM; /* [flg] Do var_1 and var_2 conform after processing? */
} /* end ncap_var_cnf_dmn() */

char * /* [sng] Name of record dimension, if any, required by re-order */
nco_var_dmn_rdr_mtd /* [fnc] Change dimension ordering of variable metadata */
(const var_sct * const var_in, /* I [ptr] Variable with metadata and data in original order */
 var_sct * const var_out, /* I/O [ptr] Variable whose metadata will be re-ordered */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_rdr), /* I [sct] List of dimension structures in new order */
 const int dmn_rdr_nbr, /* I [nbr] Number of dimension structures in structure list */
 int * const dmn_idx_out_in, /* O [idx] Dimension correspondence, output->input */
 const nco_bool * const dmn_rvr_rdr, /* I [idx] Reverse dimension */
 nco_bool * const dmn_rvr_in) /* O [idx] Reverse dimension */
{
  /* Purpose: Re-order dimensions in a given variable
     dmn_rdr contains new dimension order for dimensions
     Currently routine allows only dimension permutations, i.e., 
     re-arranging dimensions without changing their number (variable rank).
     
     Routine keeps track of two variables var_* whose abbreviations are:
     in: Input variable (already hyperslabbed) with old dimension ordering
     rdr: User-specified re-ordered dimension list. Possibly subset of dmn_in
     out: Output (re-ordered) dimensionality specific to each variable
     
     At first it seemed this routine could re-order input variable in place without copying it
     Multiple constraints keep this from being practical
     Constraints are dictated by the architectural decision to call nco_var_dmn_rdr_mtd() twice
     Decision to call nco_var_dmn_rdr_mtd() twice is based on:
     1. Want to parallelize loop over variables to increase throughput
     Parallel writes to output file only possible if output file is defined in shape, order
     Output file only definable once variable shapes, i.e., re-ordered dimensions known
     Alternatives to calling nco_var_dmn_rdr_mtd() twice:
     A. Each thread enters redefine() mode and adds its variable to output file
     Internal data re-copying would be expensive and unnecessary
     Hence Alternative A is not viable
     B. Perform output file definition and all writes after all variable re-ordering
     Memory consumption would increase to O(fl_in_sz) to keep all re-ordered data in memory
     Hence Alternative B is not viable
     2. The two calls to nco_var_dmn_rdr_mtd() accomplish the following
     A. First call: Create var_out->dim for call to nco_var_dfn() 
     Main thread makes first call in serial mode just prior to nco_var_dfn() 
     No input data (AOT metadata) have been allocated or read in at this point
     Routine exits after modifying var_out metadata for new dimension geometry
     B. Second call: Re-order var_in->val data and place in var_out
     Although var_out->dmn is retained between calls, intermediate information such as
     in_out dimension mapping arrays are lost and must be re-created
     Hence second call must re-do most of first call, then begin re-ordering
     Routine must access un-touched var_in->dim input structure during both parts of second call
     Hence var_in must be unmodified between first and second call
     
     dmn_rdr is user-specified list of dimensions to be re-arranged 
     User specifies all or only a subset of all dimensions in input file
     For example, say user specifies -d lat,lon
     This ensures lat precedes lon in all variables in output file
     In this case dmn_rdr is (user-specified) list [lat,lon]
     Input 0-D variables dimensioned [] output with dmn_out=[] (unaltered)
     Input 1-D variables dimensioned [lat] output with dmn_out=[lat] (unaltered)
     Input 2-D variables dimensioned [lat,lon] output with dmn_out=[lat,lon] (unaltered)
     Input 2-D variables dimensioned [time,lev] output with dmn_out=[time,lev] (unaltered)
     Input 2-D variables dimensioned [lon,lat] output with dmn_out=[lon,lat] (transposed)
     Input 3-D variables dimensioned [lon,lat,time] output with dmn_out=[lat,lon,time]
     Input 3-D variables dimensioned [time,lon,lat] output with dmn_out=[time,lat,lon]
     Input 3-D variables dimensioned [lon,lev,lat] output with dmn_out=[lat,lev,lon]
     Input 4-D variables dimensioned [lon,lev,lat,time] output with dmn_out=[lat,lev,lon,time]
     Hence output dimension dmn_out list depends on each particular variable 
     Some, or even all, dimensions in dmn_rdr may not be in dmn_in
     Re-ordering is only necessary for variables where dmn_in and dmn_rdr share at least two dimensions
     
     Dimension reversal:
     Users specify dimension reversal by prefixing dimension name with negative sign
     Host routine passes dimension reversing flags in dmn_rvr_rdr
     Dimensions may be re-ordered, reversed, or both */
  
  /* 20070509 CEWI RUVICFFU: dmn_idx_rdr_in */
  
  const char fnc_nm[]="nco_var_dmn_rdr_mtd()"; /* [sng] Function name */
  
  char *rec_dmn_nm_out=NULL; /* [sng] Name of record dimension, if any, required by re-order */
  
  dmn_sct **dmn_in=NULL; /* [sct] List of dimension structures in input order */
  dmn_sct **dmn_out; /* [sct] List of dimension structures in output order */
  
  int dmn_idx_in_shr[NC_MAX_VAR_DIMS]; /* [idx] Dimension correspondence, input->share Purely diagnostic */
  int dmn_idx_in_out[NC_MAX_VAR_DIMS]; /* [idx] Dimension correspondence, input->output */
  int dmn_idx_in_rdr[NC_MAX_VAR_DIMS]; /* [idx] Dimension correspondence, input->re-order NB: Purely diagnostic */
  int dmn_idx_shr_rdr[NC_MAX_VAR_DIMS]; /* [idx] Dimension correspondence, share->re-order */	  
  int dmn_idx_shr_in[NC_MAX_VAR_DIMS]; /* [idx] Dimension correspondence, share->input */	  
  int dmn_idx_shr_out[NC_MAX_VAR_DIMS]; /* [idx] Dimension correspondence, share->output */	  
  int dmn_idx_rec_out=NCO_REC_DMN_UNDEFINED; /* [idx] Record dimension index in output variable */
  int dmn_shr_nbr=0; /* [nbr] Number of dimensions dmn_in and dmn_rdr share */
  int dmn_in_idx; /* [idx] Counting index for dmn_in */
  int dmn_in_nbr; /* [nbr] Number of dimensions in input variable */
  int dmn_out_idx; /* [idx] Counting index for dmn_out */
  int dmn_out_nbr; /* [nbr] Number of dimensions in output variable */
  int dmn_rdr_idx; /* [idx] Counting index for dmn_rdr */
  int dmn_shr_idx; /* [idx] Counting index for dmn_shr */
  int idx_err=-99999; /* [idx] Invalid index for debugging */
  
  /* Initialize variables to reduce indirection */
  /* NB: Number of input and output dimensions are equal for pure re-orders
     However, keep dimension numbers in separate variables to ease relax this rule in future */
  dmn_in_nbr=var_in->nbr_dim;
  dmn_out_nbr=var_out->nbr_dim;
  
  /* Initialize dimension maps to missing_value to aid debugging */
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++)
    dmn_idx_out_in[dmn_out_idx]=idx_err;
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++){
    dmn_idx_in_shr[dmn_in_idx]=idx_err;
    dmn_idx_in_rdr[dmn_in_idx]=idx_err;
    dmn_idx_shr_rdr[dmn_in_idx]=idx_err; /* fxm: initialize up to dmn_shr_nbr which is currently unknown */
    dmn_idx_shr_in[dmn_in_idx]=idx_err; /* fxm: initialize up to dmn_shr_nbr which is currently unknown */
    dmn_idx_shr_out[dmn_in_idx]=idx_err; /* fxm: initialize up to dmn_shr_nbr which is currently unknown */
  } /* end loop over dmn_in */
  
  /* Initialize default correspondence and record dimension in case early return desired */
  if(var_out->is_rec_var) rec_dmn_nm_out=var_in->dim[0]->nm; /* 20130613: if netCDF3 _only_! */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++){
    dmn_idx_out_in[dmn_in_idx]=dmn_in_idx;
    dmn_rvr_in[dmn_in_idx]=False;
  } /* end if */
  
  /* Scalars are never altered by dimension re-ordering or reversal */
  if(dmn_in_nbr < 1) return rec_dmn_nm_out;
  
  /* On entry to this section of code, we assume:
     1. var_out duplicates var_in */
 
  /* Create complete 1-to-1 ordered list of dimensions in new output variable */
  /* For each dimension in re-ordered dimension list... */
  for(dmn_rdr_idx=0;dmn_rdr_idx<dmn_rdr_nbr;dmn_rdr_idx++){
    /* ...see if re-order dimension exists in dmn_in dimension list... */
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++){
      /* ...by comparing names, not dimension IDs... */
      if(!strcmp(var_in->dim[dmn_in_idx]->nm,dmn_rdr[dmn_rdr_idx]->nm)){
        dmn_idx_in_rdr[dmn_in_idx]=dmn_rdr_idx;
        dmn_idx_shr_rdr[dmn_shr_nbr]=dmn_rdr_idx;
        dmn_idx_shr_in[dmn_shr_nbr]=dmn_in_idx;
        dmn_idx_in_shr[dmn_in_idx]=dmn_shr_nbr;
        dmn_shr_nbr++; /* dmn_in and dmn_rdr share this dimension */
        break;
      } /* endif */
    } /* end loop over dmn_in */
  } /* end loop over dmn_rdr */
  
  /* Map permanent list of reversed dimensions to input variable */
  for(dmn_shr_idx=0;dmn_shr_idx<dmn_shr_nbr;dmn_shr_idx++)
    dmn_rvr_in[dmn_idx_shr_in[dmn_shr_idx]]=dmn_rvr_rdr[dmn_idx_shr_rdr[dmn_shr_idx]];
  
  /* No dimension re-ordering is necessary if dmn_in and dmn_rdr share fewer than two dimensions
     Dimension reversal must be done with even one shared dimension
     Single dimension reversal, however, uses default dimension maps and return values */
  if(dmn_shr_nbr < 2) return rec_dmn_nm_out;
  
  /* dmn_idx_shr_out is sorted version of dmn_idx_shr_in */
  (void)memcpy((void *)(dmn_idx_shr_out),(void *)(dmn_idx_shr_in),dmn_shr_nbr*sizeof(dmn_idx_shr_in[0]));
  qsort(dmn_idx_shr_out,(size_t)dmn_shr_nbr,sizeof(dmn_idx_shr_out[0]),nco_cmp_int);
  
  /* Initialize final map to no re-ordering */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++) 
    dmn_idx_in_out[dmn_in_idx]=dmn_in_idx;
  
  /* Splice-in re-ordered dimension location for each shared dimension */
  for(dmn_shr_idx=0;dmn_shr_idx<dmn_shr_nbr;dmn_shr_idx++)
    dmn_idx_in_out[dmn_idx_shr_in[dmn_shr_idx]]=dmn_idx_shr_out[dmn_shr_idx];
  
  if(nco_dbg_lvl_get() > nco_dbg_scl){
    (void)fprintf(stdout,"%s: DEBUG %s variable %s shares %d of its %d dimensions with the %d dimensions in the re-order list\n",nco_prg_nm_get(),fnc_nm,var_in->nm,dmn_shr_nbr,var_in->nbr_dim,dmn_rdr_nbr);
    (void)fprintf(stdout,"shr_idx\tshr_rdr\tshr_in\tshr_out\n");
    for(dmn_shr_idx=0;dmn_shr_idx<dmn_shr_nbr;dmn_shr_idx++)
      (void)fprintf(stdout,"%d\t%d\t%d\t%d\n",dmn_shr_idx,dmn_idx_shr_rdr[dmn_shr_idx],dmn_idx_shr_in[dmn_shr_idx],dmn_idx_shr_out[dmn_shr_idx]);
    (void)fprintf(stdout,"in_idx\tin_shr\tin_rdr\tin_out\trvr_flg\n");
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
      (void)fprintf(stdout,"%d\t%d\t%d\t%d\t%s\n",dmn_in_idx,dmn_idx_in_shr[dmn_in_idx],dmn_idx_in_rdr[dmn_in_idx],dmn_idx_in_out[dmn_in_idx],(dmn_rvr_in[dmn_in_idx]) ? "true" : "false");
  } /* end if dbg */

  /* Create reverse correspondence */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
    dmn_idx_out_in[dmn_idx_in_out[dmn_in_idx]]=dmn_in_idx;

  /* Create full dmn_out list */
  dmn_in=var_in->dim;
  dmn_out=(dmn_sct **)nco_malloc(dmn_out_nbr*sizeof(dmn_sct *));
  
  /* Assign dimension structures to new dimension list in correct order
     Remember: dmn_in has dimension IDs relative to input file 
     Copy dmn_in->xrf to get dimension IDs relative to output file (once they are defined) 
     Oh come on, it only seems like cheating! */
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++)
    dmn_out[dmn_out_idx]=dmn_in[dmn_idx_out_in[dmn_out_idx]]->xrf;
  
  /* Re-ordered output dimension list dmn_out now comprises correctly ordered but 
     otherwise verbatim copies of dmn_out structures in calling routine */
  
  /* Free var_out's old dimension list */
  var_out->dim=(dmn_sct **)nco_free(var_out->dim);
  /* Replace old with new dimension list */
  var_out->dim=dmn_out;
  
  /* NB: var_out is now in an inconsistent state 
     var_out->dim refers to re-ordered dimensions 
     However, var_out->dmn_id,cnt,srt,end,srd refer still duplicate var_in members
     They refer to old dimension ordering in input file
     nco_cnf_dmn_rdr_mtd() implicitly assumes that only nco_cnf_dmn_rdr_mtd() modifies var_out 
     Call to nco_cnf_dmn_rdr_val() for this variable performs actual re-ordering
     The interim inconsistent state is required for dimension IDs because 
     output dimension IDs are not known until nco_dmn_dfn() which cannot 
     (or, at least, should not) occur until output record dimension is known.
     Interim modifications of var_out by any other routine are dangerous! */
  
  /* This is clear at date written (20040727), but memories are short
     Hence we modify var_out->dmn_id,cnt,srt,end,srd to contain re-ordered values now
     This makes it safer to var_out->dmn_id,cnt,srt,end,srd before second call to nco_cnf_dmn_rdr()
     If dmn_out->id does depend on record dimension identity, then this update will do no good
     Hence, we must re-update dmn_out->id after nco_dmn_dfn() in nco_cnf_dmn_rdr_val()
     Structures should be completely consistent at that point
     Not updating these structures (at least dmn_out->id) is equivalent to assuming that
     dmn_out->id does not depend on record dimension identity, which is an ASSUMPTION
     that may currently be true, but netCDF API does not guarantee as always true. */
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++){
    /* NB: Change dmn_id,cnt,srt,end,srd together to minimize chances of forgetting one */
    var_out->dmn_id[dmn_out_idx]=dmn_out[dmn_out_idx]->id;
    var_out->cnt[dmn_out_idx]=dmn_out[dmn_out_idx]->cnt;
    var_out->srt[dmn_out_idx]=dmn_out[dmn_out_idx]->srt;
    var_out->end[dmn_out_idx]=dmn_out[dmn_out_idx]->end;
    var_out->srd[dmn_out_idx]=dmn_out[dmn_out_idx]->srd;
  } /* end loop over dmn_out */
  
  if(var_out->is_rec_var){
    /* Which dimension in output dimension list is scheduled to be record dimension? */
    for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++)
      if(dmn_out[dmn_out_idx]->is_rec_dmn) break;

    if(dmn_out_idx != dmn_out_nbr){
      dmn_idx_rec_out=dmn_out_idx;
      /* Request that first dimension be record dimension */
      rec_dmn_nm_out=dmn_out[0]->nm;
      if(nco_dbg_lvl_get() >= nco_dbg_scl && dmn_idx_rec_out != 0) (void)fprintf(stdout,"%s: INFO %s for variable %s reports old input record dimension %s is now ordinal dimension %d, new record dimension must be %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm,dmn_out[dmn_idx_rec_out]->nm,dmn_idx_rec_out,dmn_out[0]->nm);
    }else{
      /* 20121009: 
	 This block only reached by variables that will change from record in input file to fixed in output file
	 Leave is_rec_var as True here for those variables
	 Change is_rec_var to false in "if(REDEFINED_RECORD_DIMENSION)" block of ncpdq.c instead
	 Yes, this leaves the metadata in an inconsistent state 
	 However, changing all these flags in one place in ncpdq.c main() is clearer */
      ;
    } /* end else */
  } /* endif record variable */
  
  if(nco_dbg_lvl_get() > nco_dbg_var){
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
      (void)fprintf(stdout,"%s: DEBUG %s variable %s re-order maps dimension %s from (ordinal,ID)=(%d,%d) to (%d,unknown)\n",nco_prg_nm_get(),fnc_nm,var_in->nm,var_in->dim[dmn_in_idx]->nm,dmn_in_idx,var_in->dmn_id[dmn_in_idx],dmn_idx_in_out[dmn_in_idx]);
  } /* endif dbg */

  return rec_dmn_nm_out;
} /* end nco_var_dmn_rdr_mtd() */ 

int /* O [enm] Return success code */
nco_var_dmn_rdr_val /* [fnc] Change dimension ordering of variable values */
(const var_sct * const var_in, /* I [ptr] Variable with metadata and data in original order */
 var_sct * const var_out, /* I/O [ptr] Variable whose data will be re-ordered */
 const int * const dmn_idx_out_in, /* I [idx] Dimension correspondence, output->input */
 const nco_bool * const dmn_rvr_in) /* I [idx] Reverse dimension */
{
  /* Purpose: Re-order values in given variable according to supplied dimension map
     Description of re-ordering concepts is in nco_var_dmn_rdr_mtd()
     Description of actual re-ordering algorithm is in nco_var_dmn_rdr_val() */

  /* 20070509 CEWI RUVICFFU: dmn_in, dmn_id_out */

  nco_bool IDENTITY_REORDER=False; /* [flg] User requested identity re-ordering */

  char *val_in_cp; /* [ptr] Input data location as char pointer */
  char *val_out_cp; /* [ptr] Output data location as char pointer */
  
  const char fnc_nm[]="nco_var_dmn_rdr_val()"; /* [sng] Function name */

  /* dmn_sct **dmn_in=NULL; */ /* [sct] List of dimension structures in input order */
  dmn_sct **dmn_out; /* [sct] List of dimension structures in output order */

  /* int *dmn_id_out; */ /* [id] Contiguous vector of dimension IDs */
  int dmn_idx; /* [idx] Index over dimensions */
  int dmn_in_idx; /* [idx] Counting index for dmn_in */
  int dmn_in_nbr; /* [nbr] Number of dimensions in input variable */
  int dmn_in_nbr_m1; /* [nbr] Number of dimensions in input variable, less one */
  int dmn_out_idx; /* [idx] Counting index for dmn_out */
  int dmn_out_nbr; /* [nbr] Number of dimensions in output variable */
  int rcd=0; /* [rcd] Return code */
  int typ_sz; /* [B] Size of data element in memory */
  
  long dmn_in_map[NC_MAX_VAR_DIMS]; /* [idx] Map for each dimension of input variable */
  long dmn_out_map[NC_MAX_VAR_DIMS]; /* [idx] Map for each dimension of output variable */
  long dmn_in_sbs[NC_MAX_VAR_DIMS]; /* [idx] Dimension subscripts into N-D input array */
  long var_in_lmn; /* [idx] Offset into 1-D input array */
  long var_out_lmn; /* [idx] Offset into 1-D output array */
  long *var_in_cnt; /* [nbr] Number of valid elements in this dimension (including effects of stride and wrapping) */
  long var_sz; /* [nbr] Number of elements (NOT bytes) in hyperslab (NOT full size of variable in input file!) */
  
  /* Initialize variables to reduce indirection */
  /* NB: Number of input and output dimensions are equal for pure re-orders
     However, keep dimension numbers in separate variables to ease relax this rule in future */
  dmn_in_nbr=var_in->nbr_dim;
  dmn_out_nbr=var_out->nbr_dim;
  
  /* On entry to this section of code, we assume:
     1. var_out metadata are re-ordered
     2. var_out->val buffer has been allocated (calling routine must do this) */

  /* Get ready to re-order */
  /* dmn_id_out=var_out->dmn_id; */
  /* dmn_in=var_in->dim; */
  dmn_in_nbr_m1=dmn_in_nbr-1;
  dmn_out=var_out->dim;
  typ_sz=nco_typ_lng(var_out->type);
  val_in_cp=(char *)var_in->val.vp;
  val_out_cp=(char *)var_out->val.vp;
  var_in_cnt=var_in->cnt;
  var_sz=var_in->sz;
  
  /* As explained in nco_var_dmn_rdr_mtd(),
     "Hence, we must re-update dmn_out->id after nco_dmn_dfn() in nco_cnf_dmn_rdr_val()
     Structures should be completely consistent at that point
     Not updating these structures (at least dmn_out->id) is equivalent to assuming that
     dmn_out->id does not depend on record dimension identity, which is an ASSUMPTION
     that may currently be true, but is not guaranteed by the netCDF API to always be true." */
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++){
    /* NB: Change dmn_id,cnt,srt,end,srd together to minimize chances of forgetting one */
    var_out->dmn_id[dmn_out_idx]=dmn_out[dmn_out_idx]->id;
    var_out->cnt[dmn_out_idx]=dmn_out[dmn_out_idx]->cnt;
    var_out->srt[dmn_out_idx]=dmn_out[dmn_out_idx]->srt;
    var_out->end[dmn_out_idx]=dmn_out[dmn_out_idx]->end;
    var_out->srd[dmn_out_idx]=dmn_out[dmn_out_idx]->srd;
  } /* end loop over dmn_out */
  
  /* Report full metadata re-order, if requested */
  if(nco_dbg_lvl_get() > 3){
    int dmn_idx_in_out[NC_MAX_VAR_DIMS]; /* [idx] Dimension correspondence, input->output */
    /* Create reverse correspondence */
    for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++)
      dmn_idx_in_out[dmn_idx_out_in[dmn_out_idx]]=dmn_out_idx;
  
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
      (void)fprintf(stdout,"%s: DEBUG %s variable %s re-order maps dimension %s from (ordinal,ID)=(%d,%d) to (%d,%d)\n",nco_prg_nm_get(),fnc_nm,var_in->nm,var_in->dim[dmn_in_idx]->nm,dmn_in_idx,var_in->dmn_id[dmn_in_idx],dmn_idx_in_out[dmn_in_idx],var_out->dmn_id[dmn_idx_in_out[dmn_in_idx]]);
  } /* endif dbg */
  
  /* Is identity re-ordering requested? */
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++)
    if(dmn_out_idx != dmn_idx_out_in[dmn_out_idx]) break;
  if(dmn_out_idx == dmn_out_nbr) IDENTITY_REORDER=True;

  /* Dimension reversal breaks identity re-ordering */
  if(IDENTITY_REORDER){
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
      if(dmn_rvr_in[dmn_in_idx]) break;
    if(dmn_in_idx != dmn_in_nbr) IDENTITY_REORDER=False;
  } /* !IDENTITY_REORDER */

  if(IDENTITY_REORDER){
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s reports re-order is identity transformation for variable %s\n",nco_prg_nm_get(),fnc_nm,var_in->nm);
    /* Copy in one fell swoop then return */
    (void)memcpy((void *)(var_out->val.vp),(void *)(var_in->val.vp),var_out->sz*nco_typ_lng(var_out->type));
    return rcd;
  } /* !IDENTITY_REORDER */

  if(var_in->has_dpl_dmn) (void)fprintf(stdout,"%s: WARNING %s reports non-identity re-order for variable with duplicate dimensions %s.\n%s does not support non-identity re-orders of variables with duplicate dimensions\n",nco_prg_nm_get(),fnc_nm,var_in->nm,nco_prg_nm_get());

  /* Compute map for each dimension of input variable */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++) dmn_in_map[dmn_in_idx]=1L;
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr-1;dmn_in_idx++)
    for(dmn_idx=dmn_in_idx+1;dmn_idx<dmn_in_nbr;dmn_idx++)
      dmn_in_map[dmn_in_idx]*=var_in->cnt[dmn_idx];
  
  /* Compute map for each dimension of output variable */
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++) dmn_out_map[dmn_out_idx]=1L;
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr-1;dmn_out_idx++)
    for(dmn_idx=dmn_out_idx+1;dmn_idx<dmn_out_nbr;dmn_idx++)
      dmn_out_map[dmn_out_idx]*=var_out->cnt[dmn_idx];
  
  /* There is more than one method to re-order dimensions
     Output dimensionality is known in advance, unlike nco_var_avg()
     Hence outer loop may be over dimensions or over elements
     Method 1: Loop over input elements 
     1a. Loop over 1-D input array offsets
     1b. Invert 1-D input array offset to get N-D input subscripts
     1c. Turn N-D input subscripts into N-D output subscripts
     1d. Map N-D output subscripts to get 1-D output element
     1e. Copy input element to output element
     This method is simplified from method used in nco_var_avg()
     Method 2: Loop over input dimensions
     1a. Loop over input dimensions, from slowest to fastest varying
     1b. 
   */

  /* Begin Method 1: Loop over input elements */
  /* var_in_lmn is offset into 1-D array */
  for(var_in_lmn=0;var_in_lmn<var_sz;var_in_lmn++){

    /* dmn_in_sbs are corresponding indices (subscripts) into N-D array */
    dmn_in_sbs[dmn_in_nbr_m1]=var_in_lmn%var_in_cnt[dmn_in_nbr_m1];
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr_m1;dmn_in_idx++){
      dmn_in_sbs[dmn_in_idx]=(long int)(var_in_lmn/dmn_in_map[dmn_in_idx]);
      dmn_in_sbs[dmn_in_idx]%=var_in_cnt[dmn_in_idx];
    } /* end loop over dimensions */

    /* Dimension reversal:
       Reversing a dimension changes subscripts along that dimension
       Consider dimension of size N indexed by [0,1,2,...k-1,k,k+1,...,N-2,N-1] 
       Reversal maps element k to element N-1-k=N-k-1 
       Enhance speed by using that all elements along dimension share reversal */
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
      if(dmn_rvr_in[dmn_in_idx]) dmn_in_sbs[dmn_in_idx]=var_in_cnt[dmn_in_idx]-dmn_in_sbs[dmn_in_idx]-1L;

    /* Map variable's N-D array indices to get 1-D index into output data */
    var_out_lmn=0L;
    for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++) 
      var_out_lmn+=dmn_in_sbs[dmn_idx_out_in[dmn_out_idx]]*dmn_out_map[dmn_out_idx];

    /* Copy current input element into its slot in output array */
    (void)memcpy(val_out_cp+var_out_lmn*typ_sz,val_in_cp+var_in_lmn*typ_sz,(size_t)typ_sz);
  } /* end loop over var_in_lmn */
  /* End Method 1: Loop over input elements */

  /* Begin Method 2: Loop over input dimensions */
  /* End Method 2: Loop over input dimensions */

  return rcd;
} /* end nco_var_dmn_rdr_val() */ 
