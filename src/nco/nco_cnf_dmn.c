/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnf_dmn.c,v 1.14 2004-07-22 15:26:34 zender Exp $ */

/* Purpose: Conform dimensions between variables */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_cnf_dmn.h" /* Conform dimensions */

var_sct * /* O [sct] Pointer to conforming variable structure */
nco_var_cnf_dmn /* [fnc] Stretch second variable to match dimensions of first variable */
(const var_sct * const var, /* I [ptr] Pointer to variable structure to serve as template */
 var_sct * const wgt, /* I [ptr] Pointer to variable structure to make conform to var */
 var_sct *wgt_crr, /* I/O [ptr] Pointer to existing conforming variable structure, if any (destroyed when does not conform to var) */
 const bool MUST_CONFORM, /* I [flg] Must wgt and var conform? */
 bool *DO_CONFORM) /* O [flg] Do wgt and var conform? */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* fxm: TODO 226. Is xrf in nco_var_cnf_dmn() really necessary? If not, remove it and make wgt arg const var_sct * const */

  /* Purpose: Stretch second variable to match dimensions of first variable
     Dimensions in var which are not in wgt will be present in wgt_out, with values
     replicated from existing dimensions in wgt.
     By default, wgt's dimensions must be subset of var's dimensions (MUST_CONFORM=true)
     If it is permissible for wgt not to conform to var then set MUST_CONFORM=false before calling this routine
     In this case when wgt and var do not conform then then nco_var_cnf_dmn sets *DO_CONFORM=False and returns a copy of var with all values set to 1.0
     The calling procedure can then decide what to do with the output
     MUST_CONFORM is True for ncbo: Variables of like name to be, e.g., differenced, must be same rank
     MUST_CONFORM is False false for ncap, ncflint, ncwa: Some variables to be averaged may not conform to the specified weight, e.g., lon will not conform to gw. This is fine and the returned wgt_out may not be used. */

  /* There are many inelegant ways to accomplish this (without using C++): */  

  /* Perhaps most efficient method to accomplish this in general case is to expand 
     weight array until it is same size as variable array, and then multiply these
     together element-by-element in highly vectorized loop, preferably in Fortran. 
     To enhance speed, (enlarged) weight-values array could be static, only remade
     when dimensions of incoming variables change. */

  /* Another method for the general case, though an expensive one, is to use C to 
     figure out the multidimensional indices into the one dimensional hyperslab, 
     a la ncks. Knowing these indices, one can loop over the one-dimensional array
     element by element, choosing the appropriate index into the weight array from 
     those same multidimensional indices. This method can also create a static weight-value
     array that is only destroyed when an incoming variable changes dimensions from the
     previous variable. */

  /* Yet another method, which is not completely general, but which may be good enough for
     governement work, is to create fortran subroutines which expect variables of a given
     number of dimensions as input. Creating these functions for up to five dimensions would
     satisfy all foreseeable situations. The branch as to which function to call would be
     done based on number of dimensions, here in the C code. C++ or Fortran9x overloading
     could accomplish some of this interface more elegantly. */

  /* An (untested) simplification to some of these methods is to copy the 1-D array
     value pointer of variable and cast it to an N-D array pointer
     Then C could handle indexing 
     This method easily produce working, but non-general code
     Implementation would require ugly branches or hard-to-understand recursive function calls */
  
  /* Routine assumes weight will never have more dimensions than variable
     (otherwise which hyperslab of weight to use would be ill-defined). 
     However, weight may (and often will) have fewer dimensions than variable */

  bool CONFORMABLE=False; /* [flg] wgt can be made to conform to var */
  bool USE_DUMMY_WGT=False; /* [flg] Fool NCO into thinking wgt conforms to var */

  int idx; /* [idx] Counting index */
  int idx_dmn; /* [idx] Dimension index */
  int wgt_var_dmn_shr_nbr=0; /* [nbr] Number of dimensions wgt and var share */

  var_sct *wgt_out=NULL;

  /* Initialize flag to false. Overwrite by true after successful conformance */
  *DO_CONFORM=False;
  
  /* Does current weight (wgt_crr) conform to variable's dimensions? */
  if(wgt_crr != NULL){
    /* Test rank first because wgt_crr because of 19960218 bug (invalid dmn_id in old wgt_crr leads to match) */
    if(var->nbr_dim == wgt_crr->nbr_dim){
      /* Test whether all wgt and var dimensions match in sequence */
      for(idx=0;idx<var->nbr_dim;idx++){
	/*	if(wgt_crr->dmn_id[idx] != var->dmn_id[idx]) break;*/
	if(!strstr(wgt_crr->dim[idx]->nm,var->dim[idx]->nm)) break;
      } /* end loop over dimensions */
      if(idx == var->nbr_dim) *DO_CONFORM=True;
    } /* end if */
    if(*DO_CONFORM){
      wgt_out=wgt_crr;
    }else{
      wgt_crr=nco_var_free(wgt_crr);
      wgt_out=NULL;
    } /* end if */
  } /* end if */

  /* Does original weight (wgt) conform to variable's dimensions? */
  if(wgt_out == NULL){
    if(var->nbr_dim > 0){
      /* Test that all dimensions in wgt appear in var */
      for(idx=0;idx<wgt->nbr_dim;idx++){
        for(idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(strstr(wgt->dim[idx]->nm,var->dim[idx_dmn]->nm)){
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
	  (void)fprintf(stdout,"%s: ERROR %s and template %s share no dimensions\n",prg_nm_get(),wgt->nm,var->nm);
	  nco_exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"\n%s: DEBUG %s and template %s share no dimensions: Not broadcasting %s to %s\n",prg_nm_get(),wgt->nm,var->nm,wgt->nm,var->nm);
	  USE_DUMMY_WGT=True;
	} /* endif */
      }else if(wgt->nbr_dim > var->nbr_dim){
	/* wgt is larger rank than var---no possibility of conforming */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %s is rank %d but template %s is rank %d: Impossible to broadcast\n",prg_nm_get(),wgt->nm,wgt->nbr_dim,var->nm,var->nbr_dim);
	  nco_exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"\n%s: DEBUG %s is rank %d but template %s is rank %d: Not broadcasting %s to %s\n",prg_nm_get(),wgt->nm,wgt->nbr_dim,var->nm,var->nbr_dim,wgt->nm,var->nm);
	  USE_DUMMY_WGT=True;
	} /* endif */
      }else if(wgt_var_dmn_shr_nbr > 0 && wgt_var_dmn_shr_nbr < wgt->nbr_dim){
	/* Some, but not all, of wgt dimensions are in var */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",prg_nm_get(),wgt_var_dmn_shr_nbr,wgt->nm,var->nm,wgt->nbr_dim-wgt_var_dmn_shr_nbr);
	  nco_exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() > 2) (void)fprintf(stdout,"\n%s: DEBUG %d dimensions of %s belong to template %s but %d dimensions do not: Not broadcasting %s to %s\n",prg_nm_get(),wgt_var_dmn_shr_nbr,wgt->nm,var->nm,wgt->nbr_dim-wgt_var_dmn_shr_nbr,wgt->nm,var->nm);
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
	    if(!strstr(wgt->dim[idx]->nm,var->dim[idx]->nm)) break;
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

  if(wgt_out == NULL){
    /* Expand original weight (wgt) to match size of current variable */
    char * restrict wgt_cp;
    char * restrict wgt_out_cp;

    int idx_wgt_var[NC_MAX_DIMS];
    /*    int idx_var_wgt[NC_MAX_DIMS];*/
    int wgt_nbr_dim;
    int var_nbr_dmn_m1;

    long * restrict var_cnt;
    long dmn_ss[NC_MAX_DIMS];
    long dmn_var_map[NC_MAX_DIMS];
    long dmn_wgt_map[NC_MAX_DIMS];
    long var_lmn;
    long wgt_lmn;
    long var_sz;

    size_t wgt_typ_sz;

    /* Copy main attributes of variable into current weight */
    wgt_out=nco_var_dpl(var);
    (void)nco_xrf_var(wgt,wgt_out);

    /* Modify a few elements of weight array */
    wgt_out->nm=wgt->nm;
    wgt_out->id=wgt->id;
    wgt_out->type=wgt->type;
    wgt_out->val.vp=(void *)nco_malloc(wgt_out->sz*nco_typ_lng(wgt_out->type));
    wgt_cp=(char *)wgt->val.vp;
    wgt_out_cp=(char *)wgt_out->val.vp;
    wgt_typ_sz=nco_typ_lng(wgt_out->type);

    if(wgt_out->nbr_dim == 0){
      /* Variable (and weight) are scalars, not arrays */

      (void)memcpy(wgt_out_cp,wgt_cp,wgt_typ_sz);

    }else{
      /* Variable (and weight) are arrays, not scalars */
      
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
	 request imap vector to specify offset (imap) vector in bytes.
      */

      for(idx=0;idx<wgt->nbr_dim;idx++){
	for(idx_dmn=0;idx_dmn<var->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(strstr(var->dim[idx_dmn]->nm,wgt->dim[idx]->nm)){
	    idx_wgt_var[idx]=idx_dmn;
	    /*	    idx_var_wgt[idx_dmn]=idx;*/
	    break;
	  } /* end if */
	  /* Sanity check */
	  if(idx_dmn == var->nbr_dim-1){
	    (void)fprintf(stdout,"%s: ERROR wgt %s has dimension %s but var %s does not deep in nco_var_cnf_dmn()\n",prg_nm_get(),wgt->nm,wgt->dim[idx]->nm,var->nm);
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
	dmn_ss[var_nbr_dmn_m1]=var_lmn%var_cnt[var_nbr_dmn_m1];
	for(idx=0;idx<var_nbr_dmn_m1;idx++){
	  dmn_ss[idx]=(long)(var_lmn/dmn_var_map[idx]);
	  dmn_ss[idx]%=var_cnt[idx];
	} /* end loop over dimensions */
	
	/* Map (shared) N-D array indices into 1-D index into original weight data */
	wgt_lmn=0L;
	for(idx=0;idx<wgt_nbr_dim;idx++) wgt_lmn+=dmn_ss[idx_wgt_var[idx]]*dmn_wgt_map[idx];
	
	(void)memcpy(wgt_out_cp+var_lmn*wgt_typ_sz,wgt_cp+wgt_lmn*wgt_typ_sz,wgt_typ_sz);
	
      } /* end loop over var_lmn */
      
    } /* end if variable (and weight) are arrays, not scalars */
    
    *DO_CONFORM=True;
  } /* end if we had to stretch weight to fit variable */
  
  if(*DO_CONFORM == -1){
    (void)fprintf(stdout,"%s: ERROR *DO_CONFORM == -1 on exit from nco_var_cnf_dmn()\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  
  /* Current weight (wgt_out) now conforms to current variable */
  return wgt_out;
  
} /* end nco_var_cnf_dmn() */

bool /* [flg] var_1 and var_2 conform after processing */
ncap_var_cnf_dmn /* [fnc] Broadcast smaller variable into larger */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2) /* I/O [ptr] Second variable */
{
  /* Purpose: Return conforming variables. If this is not possible then die. 
     Routine is a wrapper for nco_var_cnf_dmn() which does the hard work */

  bool DO_CONFORM; /* [flg] Do var_1 and var_2 conform after processing? */
  bool MUST_CONFORM=True; /* [flg] Must var_1 and var_2 conform? */
  var_sct *var_1_org; /* [ptr] Original location of var_1 */
  var_sct *var_2_org; /* [ptr] Original location of var_2 */

  var_1_org=*var_1; /* [ptr] Original location of var_1 */
  var_2_org=*var_2; /* [ptr] Original location of var_2 */

  if(var_1_org->nbr_dim > var_2_org->nbr_dim) *var_2=nco_var_cnf_dmn(var_1_org,var_2_org,NULL,MUST_CONFORM,&DO_CONFORM); else *var_1=nco_var_cnf_dmn(var_2_org,var_1_org,NULL,MUST_CONFORM,&DO_CONFORM);
  
  /* fxm: Memory leak?
     nco_var_cnf_dmn() does not do its own memory handling
     If original var_1 or var_2 was overwritten (replaced by conforming variable),
     then original must be free()'d now before its location is lost.
     Test for equality between pointers on entry and exit, and
     nco_var_free() calling variable if it changed 
     20020114: Freeing variables here causes core dump, not sure why */
  /*  if(*var_1 != var_1_org) var_1_org=nco_var_free(var_1_org);*/
  /*  if(*var_2 != var_2_org) var_2_org=nco_var_free(var_2_org);*/

  if(!DO_CONFORM){
    (void)fprintf(stderr,"%s: Variables do not have have conforming dimensions. Cannot proceed with operation\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */

  return DO_CONFORM; /* [flg] Do var_1 and var_2 conform after processing? */
} /* end ncap_var_cnf_dmn() */

var_sct * /* O [sct] Pointer to variable with re-ordered dimensions */
nco_var_dmn_rdr /* [fnc] Change dimension ordering */
(const var_sct * const var_in, /* I [ptr] Variable whose dimensions to re-order */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_out), /* I [sct] List of dimension structures in new order */
 const int nbr_dmn_out) /* I [nbr] Number of dimension structures in structure list */
{
  /* Purpose: Re-order dimensions in a given variable
     dmn_out contains is the new order for dimensions
     NB: free() var_in in calling routine if it is no longer needed */
  int dmn_in_idx; /* [idx] Counting index for dmn_in */
  int dmn_out_idx; /* [idx] Counting index for dmn_out */
  int dmn_in_nbr; /* [nbr] Number of dimensions in input file */
  int dmn_in_dmn_out_shr_nbr=0; /* [nbr] Number of dimensions shared by dmn_in and dmn_out */

  var_sct *var_out=NULL;
  
  /* Initialize variables to reduce indirection */
  dmn_in_nbr=var_in->nbr_dim; /* [nbr] Number of dimensions in input file */
  var_out=nco_var_dpl(var_in);

  /* Create complete 1-to-1 ordered list of dimensions in new output variable */

  if(dmn_in_nbr > 0){
    /* For each dimension in the re-ordered dimension list... */
    for(dmn_out_idx=0;dmn_out_idx<nbr_dmn_out;dmn_out_idx++){
      /* Test whether dimension exists in dmn_in dimension list... */
      for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++){
	/* Compare names, not dimension IDs */
	if(strstr(var_in->dim[dmn_in_idx]->nm,dmn_out[dmn_out_idx]->nm)){
	  dmn_in_dmn_out_shr_nbr++; /* dmn_in and dmn_out share this dimension */
	  break;
	} /* endif */
      } /* end loop over dmn_in */
    } /* end loop over dmn_out */
  } /* endif dmn_in_nbr > 0 */

  return var_out;
} /* end nco_var_dmn_rdr() */ 

