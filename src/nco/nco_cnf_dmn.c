/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnf_dmn.c,v 1.17 2004-07-27 19:47:31 zender Exp $ */

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

dmn_sct ** /* O [sct] Dimension structures to be re-ordered */
nco_prs_rdr_lst /* [fnc] Convert re-order string list into dimension structure list */
(dmn_sct ** const dmn_in, /* I [sct] Dimension list for input file */
 char **dmn_rdr_lst, /* I [sng] Names of dimensions to be re-ordered */
 const int dmn_rdr_nbr) /* I [nbr] Number of dimension structures in re-order list */
{
  /* Purpose: Convert re-order string list into dimension structure list
     Check validity of requested dimension and print warnings/errors as necessary */

  dmn_sct **dmn_rdr; /* [sct] Dimension structures to be re-ordered */
  int foo;
  /* fxm: TODO nco329 */
  dmn_rdr=dmn_in;
  dmn_rdr_lst=dmn_rdr_lst;
  foo=dmn_rdr_nbr;

  return dmn_rdr;
} /* end nco_prs_rdr_lst() */

var_sct * /* O [sct] Pointer to variable with re-ordered dimensions */
nco_var_dmn_rdr /* [fnc] Change dimension ordering */
(var_sct * const var_in, /* I [ptr] Variable whose dimensions to re-order */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_rdr), /* I [sct] List of dimension structures in new order */
 const int dmn_rdr_nbr, /* I [nbr] Number of dimension structures in structure list */
 bool DO_DIMENSIONALITY_ONLY, /* I [flg] Determine and set new dimensionality then return */
 bool DO_REORDER_ONLY, /* I [flg] Re-order data (dimensionality already set) */
 bool DO_WHOLE_SHEBANG) /* I [flg] Determine and set new dimensionality then re-order data */
{
  /* Purpose: Re-order dimensions in a given variable
     dmn_rdr contains is the new order for dimensions
     NB: free() var_in in calling routine if it is no longer needed

     Routine keeps track of two variables var_* whose abbreviations are:
     in: Input variable (already hyperslabbed) with old dimension ordering
     rdr: User-specified re-ordered dimension list. Possibly subset of dmn_in
     out: Output (re-ordered) dimensionality specific to each variable

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
     
     fxm: Add dimension reversal capability to this routine
     Dimension reversal:
     Users specify dimension reversal by prefixing dimension name with negative sign
     Dimensions may be re-ordered, reversed, or both
     Dimension reversal algorithm could be implemented by fiddling with dimension maps */
  
  char *val_in_cp; /* [ptr] Input data location as char pointer */
  char *val_out_cp; /* [ptr] Output data location as char pointer */
  
  const char fnc_nm[]="nco_var_dmn_rdr()"; /* [sng] Function name */
  const int dmn_rdr_nil=-1; /* [enm] Input dimension is not in re-order list */
  
  dmn_sct **dmn_in=NULL; /* [sct] List of dimension structures in input order */
  dmn_sct **dmn_out; /* [sct] List of dimension structures in output order */
  
  int *dmn_id_out; /* [id] Contiguous vector of dimension IDs */
  int dmn_idx; /* [idx] Index over dimensions */
  int dmn_idx_in_out[NC_MAX_DIMS]; /* [idx] Dimension correspondence, input->output */
  int dmn_idx_out_in[NC_MAX_DIMS]; /* [idx] Dimension correspondence, output->input */
  int dmn_idx_in_rdr[NC_MAX_DIMS]; /* [idx] Dimension correspondence, input->re-order */
  int dmn_idx_rdr_in[NC_MAX_DIMS]; /* [idx] Dimension correspondence, re-order->input */
  int dmn_in_dmn_rdr_shr_nbr=0; /* [nbr] Number of dimensions dmn_in and dmn_rdr share */
  int dmn_in_idx; /* [idx] Counting index for dmn_in */
  int dmn_in_nbr; /* [nbr] Number of dimensions in input variable */
  int dmn_in_nbr_m1; /* [nbr] Number of dimensions in input variable, less one */
  int dmn_nbr_ass_crr=0; /* [nbr] Number of dimensions currently assigned */
  int dmn_out_idx; /* [idx] Counting index for dmn_out */
  int dmn_out_nbr; /* [nbr] Number of dimensions in output variable */
  int dmn_rdr_idx; /* [idx] Counting index for dmn_rdr */
  int typ_sz; /* [B] Size of data element in memory */
  int dmn_in_idx_srt; /* [idx] Starting index of current un-ordered dimension group */
  
  long dmn_in_map[NC_MAX_DIMS]; /* [idx] Map for each dimension of input variable */
  long dmn_out_map[NC_MAX_DIMS]; /* [idx] Map for each dimension of output variable */
  long dmn_in_ss[NC_MAX_DIMS]; /* [idx] Dimension subscripts into N-D input array */
  long var_in_lmn; /* [idx] Offset into 1-D input array */
  long var_out_lmn; /* [idx] Offset into 1-D output array */
  long *var_in_cnt; /* [nbr] Number of valid elements in this dimension (including effects of stride and wrapping) */
  long var_sz; /* [nbr] Number of elements (NOT bytes) in hyperslab (NOT full size of variable in input file!) */
  
  var_sct *var_out=NULL; /* O [sct] Pointer to variable with re-ordered dimensions */
  
  DO_DIMENSIONALITY_ONLY=DO_DIMENSIONALITY_ONLY; /* [flg] CEWI */
  DO_REORDER_ONLY=DO_REORDER_ONLY; /* [flg] CEWI */
  DO_WHOLE_SHEBANG=DO_WHOLE_SHEBANG; /* [flg] CEWI */

  /* Initialize variables to reduce indirection */
  /* Number of input and output dimensions are equal for any re-ordering */
  dmn_out_nbr=dmn_in_nbr=var_in->nbr_dim; /* [nbr] Number of dimensions in input variable */
  /* Scalars and 1-D variables are never altered by dimension re-ordering */
  if(dmn_in_nbr <= 1) return var_in;
  
  /* Initialize correspondence of input-to-reorder dimensions to "none" */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++) dmn_idx_in_rdr[dmn_in_idx]=dmn_rdr_nil;
  /* Create complete 1-to-1 ordered list of dimensions in new output variable */
  /* For each dimension in the re-ordered dimension list... */
  for(dmn_rdr_idx=0;dmn_rdr_idx<dmn_rdr_nbr;dmn_rdr_idx++){
    /* Test whether dimension exists in dmn_in dimension list... */
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++){
      /* Compare names, not dimension IDs */
      if(strstr(var_in->dim[dmn_in_idx]->nm,dmn_rdr[dmn_rdr_idx]->nm)){
	dmn_idx_in_rdr[dmn_in_idx]=dmn_rdr_idx; /* [idx] Dimension correspondence, input->re-order */	  
	dmn_idx_rdr_in[dmn_rdr_idx]=dmn_in_idx; /* [idx] Dimension correspondence, re-order->input */	  
	dmn_in_dmn_rdr_shr_nbr++; /* dmn_in and dmn_rdr share this dimension */
	break;
      } /* endif */
    } /* end loop over dmn_in */
  } /* end loop over dmn_rdr */
  
  /* No re-ordering necessary if dmn_in and dmn_rdr share fewer than two dimensions */
  /* fxm: this will change to 1 dimension when reversing is implemented */
  if(dmn_in_dmn_rdr_shr_nbr <= 1) return var_in;
  
  /* Initialize output order to input order */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
    dmn_idx_in_out[dmn_in_idx]=dmn_in_idx;

  /* For each dimension in re-order dimension list... */
  for(dmn_rdr_idx=0;dmn_rdr_idx<dmn_rdr_nbr;dmn_rdr_idx++){
    /* Lower index bound for current group of un-assigned, un-ordered dimensions */
    if(dmn_rdr_idx == 0) dmn_in_idx_srt=0; else dmn_in_idx_srt=dmn_idx_rdr_in[dmn_rdr_idx-1]+1;
    /* Assign preceding un-assigned, un-ordered input dimensions to output list */
    for(dmn_in_idx=dmn_in_idx_srt;dmn_in_idx<dmn_idx_rdr_in[dmn_rdr_idx]-2;dmn_in_idx++)
      dmn_idx_in_out[dmn_in_idx]=dmn_nbr_ass_crr++;
    
    /* Splice shared dimension in after last group of non-shared dimensions */
    dmn_idx_in_out[dmn_nbr_ass_crr++]=dmn_idx_rdr_in[dmn_rdr_idx];
  } /* end loop over dmn_rdr */
  
  /* Create reverse correspondence */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
    dmn_idx_out_in[dmn_idx_in_out[dmn_in_idx]]=dmn_in_idx;

  /* Create full dmn_out list */
  dmn_out=(dmn_sct **)nco_malloc(dmn_out_nbr*sizeof(dmn_sct *));

  /* Assign dimension structures to new dimension list in correct order */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++)
    dmn_out[dmn_in_idx]=dmn_in[dmn_idx_in_out[dmn_in_idx]];

  if(dbg_lvl_get() == 3){
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++){
      (void)fprintf(stdout,"%s: DEBUG %s maps dimension %s from %d to %d\n",prg_nm_get(),fnc_nm,var_in->dim[dmn_in_idx]->nm,dmn_in_idx,dmn_idx_in_out[dmn_in_idx]);
    } /* end loop over dmn_in */
  } /* endif dbg */
  
  /* Create output variable structure */
  var_out=nco_var_dpl(var_in); /* O [sct] Pointer to variable with re-ordered dimensions */
  var_sz=var_out->sz;

  /* Replace old with new dimension list then free old list */
  dmn_in=var_in->dim;
  var_out->dim=dmn_out;
  dmn_in=(dmn_sct **)nco_free(dmn_in);

  /* Initialize value and tally arrays */
  (void)nco_zero_long(var_sz,var_out->tally);
  (void)nco_var_zero(var_out->type,var_sz,var_out->val);

  /* Get ready to re-order */
  dmn_id_out=var_out->dmn_id;
  dmn_in_nbr_m1=dmn_in_nbr-1;
  dmn_out=var_out->dim;
  typ_sz=nco_typ_lng(var_out->type);
  val_in_cp=(char *)var_in->val.vp;
  val_out_cp=(char *)var_out->val.vp;
  var_in_cnt=var_in->cnt;

  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++){
    /* fxm: Changed dmn_id,cnt,srt,end,srd...anything else need changing? */
    var_out->dmn_id[dmn_out_idx]=var_in->dmn_id[dmn_idx_out_in[dmn_out_idx]];
    var_out->cnt[dmn_out_idx]=var_in->cnt[dmn_idx_out_in[dmn_out_idx]];
    var_out->srt[dmn_out_idx]=var_in->srt[dmn_idx_out_in[dmn_out_idx]];
    var_out->end[dmn_out_idx]=var_in->end[dmn_idx_out_in[dmn_out_idx]];
    var_out->srd[dmn_out_idx]=var_in->srd[dmn_idx_out_in[dmn_out_idx]];
  } /* end loop over dmn_out */

  /* Compute map for each dimension of input variable */
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr;dmn_in_idx++) dmn_in_map[dmn_in_idx]=1L;
  for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr-1;dmn_in_idx++)
    for(dmn_idx=dmn_in_idx+1;dmn_idx<dmn_in_nbr;dmn_idx++)
      dmn_in_map[dmn_in_idx]*=var_in->cnt[dmn_idx];

  /* Compute map for each dimension of output variable */
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++) dmn_out_map[dmn_out_idx]=1L;
  for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr-1;dmn_out_idx++)
    for(dmn_idx=dmn_out_idx+1;dmn_idx<dmn_out_nbr;dmn_idx++)
      /* fxm: make sure var_out->cnt exists already */
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

    /* dmn_in_ss are corresponding indices (subscripts) into N-D array */
    dmn_in_ss[dmn_in_nbr_m1]=var_in_lmn%var_in_cnt[dmn_in_nbr_m1];
    for(dmn_in_idx=0;dmn_in_idx<dmn_in_nbr_m1;dmn_in_idx++){
      dmn_in_ss[dmn_in_idx]=(long)(var_in_lmn/dmn_in_map[dmn_in_idx]);
      dmn_in_ss[dmn_in_idx]%=var_in_cnt[dmn_in_idx];
    } /* end loop over dimensions */

    /* Map variable's N-D array indices to get 1-D index into output data */
    var_out_lmn=0L;
    for(dmn_out_idx=0;dmn_out_idx<dmn_out_nbr;dmn_out_idx++) var_out_lmn+=dmn_in_ss[dmn_idx_out_in[dmn_out_idx]]*dmn_out_map[dmn_out_idx];

    /* Copy current input element into its slot in output array */
    (void)memcpy(val_out_cp+var_out_lmn*typ_sz,val_in_cp+var_in_lmn*typ_sz,(size_t)typ_sz);
  } /* end loop over var_in_lmn */
  /* End Method 1: Loop over input elements */

  /* Begin Method 2: Loop over input dimensions */
  /* End Method 2: Loop over input dimensions */

  return var_out;
} /* end nco_var_dmn_rdr() */ 

