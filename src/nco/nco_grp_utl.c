/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.c,v 1.22 2012-01-01 20:51:53 zender Exp $ */

/* Purpose: Group utilities */

/* Copyright (C) 2011--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Testing:
   eos52nc4 ~/nco/data/in.he5 ~/in.nc4
   export HDF5_DISABLE_VERSION_CHECK=1
   ncdump -h ~/in.nc4
   ncks -D 1 -m ~/in.nc4
   ncks -O -D 3 -g HIRDLS -m ~/in.nc4 ~/foo.nc
   ncks -O -D 3 -m ~/in_grp.nc ~/foo.nc
   ncks -O -D 3 -v 'Q.?' ~/nco/data/in.nc ~/foo.nc
 */

#include "nco_grp_utl.h" /* Group utilities */

int /* [rcd] Return code */
nco_inq_grps_full /* [fnc] Discover and return IDs of apex and all sub-groups */
(const int grp_id, /* I [ID] Apex group */
 int * const grp_nbr, /* O [nbr] Number of groups */
 int * const grp_ids) /* O [ID] Group IDs of children */
{
  /* Purpose: Discover and return IDs of apex and all sub-groups
     If grp_nbr is NULL, it is ignored 
     If grp_ids is NULL, it is ignored
     grp_ids must contain enough space to hold grp_nbr IDs */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int grp_nbr_crr; /* [nbr] Number of groups counted so far */
  int grp_id_crr; /* [ID] Current group ID */

  grp_stk_sct *grp_stk; /* [sct] Group stack pointer */
  
  /* Initialize variables that are incremented */
  grp_nbr_crr=0; /* [nbr] Number of groups counted (i.e., stored in grp_ids array) so far */

  /* Initialize and obtain group iterator */
  rcd+=nco_grp_stk_get(grp_id,&grp_stk);

  /* Find and return next group ID */
  rcd+=nco_grp_stk_nxt(grp_stk,&grp_id_crr);
  while(grp_id_crr != NCO_LST_GRP){
    /* Store last popped value into group ID array */
    if(grp_ids) grp_ids[grp_nbr_crr]=grp_id_crr; /* [ID] Group IDs of children */

    /* Increment counter */
    grp_nbr_crr++;

    /* Find and return next group ID */
    rcd+=nco_grp_stk_nxt(grp_stk,&grp_id_crr);
  } /* end while */
  if(grp_nbr) *grp_nbr=grp_nbr_crr; /* O [nbr] Number of groups */

  /* Free group iterator */
  nco_grp_itr_free(grp_stk);

  return rcd; /* [rcd] Return code */
} /* end nco_inq_grps_full() */

void
nco_grp_itr_free /* [fnc] Free group iterator */
(grp_stk_sct * const grp_stk) /* O [sct] Group stack pointer */
{
  /* Purpose: Free group iterator
     Call a function that hides the iterator implementation behind the API */
  nco_grp_stk_free(grp_stk);
} /* end nco_grp_itr_free() */

int /* [rcd] Return code */
nco_grp_stk_get /* [fnc] Initialize and obtain group iterator */
(const int grp_id, /* I [ID] Apex group */
 grp_stk_sct ** const grp_stk) /* O [sct] Group stack pointer */
{
  /* Purpose: Initialize and obtain group iterator
     Routine pushes apex group onto stack
     The "Apex group" is normally (though not necessarily) the netCDF file ID, 
     aka, root ID, and is the top group in the hierarchy.
     Returned iterator contains one valid group, the apex group,
     and the stack counter equals one */
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  rcd+=nco_inq_grps(grp_id,(int *)NULL,(int *)NULL);

  /* These error codes would cause an abort in the netCDF wrapper layer anyway,
     so this condition is purely for defensive programming. */
  if(rcd != NC_EBADID && rcd != NC_EBADGRPID){
    *grp_stk=nco_grp_stk_ntl();

    /* [fnc] Push group ID onto stack */
    (void)nco_grp_stk_psh(*grp_stk,grp_id);
  } /* endif */

  return rcd; /* [rcd] Return code */
} /* end nco_grp_stk_get() */

int /* [rcd] Return code */
nco_grp_stk_nxt /* [fnc] Find and return next group ID */
(grp_stk_sct * const grp_stk, /* O [sct] Group stack pointer */
 int * const grp_id) /* O [ID] Group ID */
{
  /* Purpose: Find and return next group ID
     Next group ID is popped off stack 
     If this group contains sub-groups, these sub-groups are pushed onto stack before return
     Hence routine ensures every group is eventually examined for sub-groups */
  int *grp_ids; /* [ID] Group IDs of children */
  int idx;
  int grp_nbr; /* I [nbr] Number of sub-groups */
  int rcd=NC_NOERR; /* [rcd] Return code */
 
  /* Is stack empty? */
  if(grp_stk->grp_nbr == 0){
    /* Return flag showing iterator has reached the end, i.e., no more groups */
    *grp_id=NCO_LST_GRP;
  }else{
    /* Return current stack top */
    *grp_id=nco_grp_stk_pop(grp_stk);
    /* Replenish stack with sub-group IDs, if available */
    rcd+=nco_inq_grps(*grp_id,&grp_nbr,(int *)NULL);
    if(grp_nbr > 0){
      /* Add sub-groups of current stack top */
      grp_ids=(int *)nco_malloc(grp_nbr*sizeof(int));
      rcd+=nco_inq_grps(*grp_id,(int *)NULL,grp_ids);
      /* Push sub-group IDs in reverse order so when popped will come out in original order */
      for(idx=grp_nbr-1;idx>=0;idx--) (void)nco_grp_stk_psh(grp_stk,grp_ids[idx]);
      /* Clean up memory */
      grp_ids=(int *)nco_free(grp_ids);
    } /* endif sub-groups exist */
  } /* endelse */

  return rcd; /* [rcd] Return code */
} /* end nco_grp_stk_nxt() */

grp_stk_sct * /* O [sct] Group stack pointer */
nco_grp_stk_ntl /* [fnc] Initialize group stack */
(void)
{
  /* Purpose: Initialize dynamic array implementation of group stack */
  grp_stk_sct *grp_stk; /* O [sct] Group stack pointer */
  grp_stk=(grp_stk_sct *)nco_malloc(sizeof(grp_stk)); /* O [sct] Group stack pointer */
  grp_stk->grp_nbr=0; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=NULL; /* [ID] Group ID */
  return grp_stk;/* O [sct] Group stack pointer */
} /* end nco_grp_stk_ntl() */

void
nco_grp_stk_psh /* [fnc] Push group ID onto stack */
(grp_stk_sct * const grp_stk, /* I/O [sct] Group stack pointer */
 const int grp_id) /* I [ID] Group ID to push */
{
  /* Purpose: Push group ID onto dynamic array implementation of stack */
  grp_stk->grp_nbr++; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=(int *)nco_realloc(grp_stk->grp_id,(grp_stk->grp_nbr)*sizeof(int)); /* O [sct] Pointer to group IDs */
  grp_stk->grp_id[grp_stk->grp_nbr-1]=grp_id; /* [ID] Group ID */
} /* end nco_grp_stk_psh() */

int /* O [ID] Group ID that was popped */
nco_grp_stk_pop /* [fnc] Remove and return group ID from stack */
(grp_stk_sct * const grp_stk) /* I/O [sct] Pointer to top of stack */
{
  /* Purpose: Remove and return group ID from dynamic array implementation of stack */
  int grp_id; /* [ID] Group ID that was popped */
  grp_id=grp_stk->grp_id[grp_stk->grp_nbr-1]; /* [ID] Group ID that was popped */

  if(grp_stk->grp_nbr == 0){
    (void)fprintf(stderr,"%s: ERROR nco_grp_stk_pop() asked to pop empty stack\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  grp_stk->grp_nbr--; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=(int *)nco_realloc(grp_stk->grp_id,(grp_stk->grp_nbr)*sizeof(int)); /* O [sct] Pointer to group IDs */

  return grp_id; /* [ID] Group ID that was popped */
} /* end nco_grp_stk_pop() */

void
nco_grp_stk_free /* [fnc] Free group stack */
(grp_stk_sct * const grp_stk) /* O [sct] Group stack pointer */
{
  /* Purpose: Free dynamic array implementation of stack */
  grp_stk->grp_id=(int *)nco_free(grp_stk->grp_id);
} /* end nco_grp_stk_free() */

int /* [rcd] Return code */
nco4_inq /* [fnc] Find and return global totals of dimensions, variables, attributes */
(const int nc_id, /* I [ID] Apex group */
 int * const att_nbr_glb, /* O [nbr] Number of global attributes in file */
 int * const dmn_nbr_all, /* O [nbr] Number of dimensions in file */
 int * const var_nbr_all, /* O [nbr] Number of variables in file */
 int * const rec_dmn_nbr, /* O [nbr] Number of record dimensions in file */
 int * const rec_dmn_ids) /* O [ID] Record dimension IDs in file */
{
  /* [fnc] Find and return global totals of dimensions, variables, attributes
     nco_inq() only applies to a single group
     Statistics for recursively nested netCDF4 files require more care */
  int rcd=NC_NOERR;
  int *grp_ids; /* [ID] Group IDs of children */
  int grp_id; /* [ID] Group ID */
  int grp_idx;
  int grp_nbr; /* [nbr] Number of groups */
  int var_nbr; /* [nbr] Number of variables */

  /* Discover and return number of apex and all sub-groups */
  rcd+=nco_inq_grps_full(nc_id,&grp_nbr,(int *)NULL);

  grp_ids=(int *)nco_malloc(grp_nbr*sizeof(int)); /* [ID] Group IDs of children */

  /* Discover and return IDs of apex and all sub-groups */
  rcd+=nco_inq_grps_full(nc_id,&grp_nbr,grp_ids);

  /* Initialize variables that accumulate */
  *var_nbr_all=0; /* [nbr] Total number of variables in file */

  /* Create list of all variables in input file */
  for(grp_idx=0;grp_idx<grp_nbr;grp_idx++){
    grp_id=grp_ids[grp_idx]; /* [ID] Group ID */

    /* How many variables in current group? */
    rcd+=nco_inq_varids(grp_id,&var_nbr,(int *)NULL);

    /* Augment total number of variables in file */
    *var_nbr_all+=var_nbr;
  } /* end loop over grp */

  /* Compare to results of nco_inq() */
  {
    int att_nbr; /* [nbr] Number of attributes */
    int dmn_nbr; /* [nbr] Number of dimensions */
    int rec_dmn_id=NCO_REC_DMN_UNDEFINED; /* [ID] Record dimension ID */

    rcd+=nco_inq(nc_id,&dmn_nbr,&var_nbr,&att_nbr,&rec_dmn_id);
    if(dbg_lvl_get() >= 2) (void)fprintf(stdout,"%s: INFO nco_inq() reports file contains %d variable%s, %d dimension%s, and %d global attribute%s\n",prg_nm_get(),var_nbr,(var_nbr > 1) ? "s" : "",dmn_nbr,(dmn_nbr > 1) ? "s" : "",att_nbr,(att_nbr > 1) ? "s" : "");

    /* fxm: Backward compatibility */
    *rec_dmn_nbr=1;
    if(rec_dmn_ids) *rec_dmn_ids=rec_dmn_id;
    *att_nbr_glb=att_nbr;
    *dmn_nbr_all=dmn_nbr;
  }

  if(dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO nco4_inq() reports file contains %d group%s comprising %d variable%s, %d dimension%s, and %d global attribute%s\n",prg_nm_get(),grp_nbr,(grp_nbr > 1) ? "s" : "",*var_nbr_all,(*var_nbr_all > 1) ? "s" : "",*dmn_nbr_all,(*dmn_nbr_all > 1) ? "s" : "",*att_nbr_glb,(*att_nbr_glb > 1) ? "s" : "");

  return rcd;
} /* end nco4_inq() */

nm_id_sct * /* O [sct] Variable extraction list */
nco4_var_lst_mk /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id, /* I [enm] Apex group ID */
 int * const nbr_var_fl, /* O [nbr] Number of variables in input file */
 char * const * const var_lst_in, /* I [sng] User-specified list of variable names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 const nco_bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const var_nbr_xtr) /* I/O [nbr] Number of variables in current extraction list */
{
  /* Purpose: Create variable extraction list with or without regular expressions */
  
  char *var_sng; /* User-specified variable name or regular expression */
  char *grp_nm_fll; /* [sng] Fully qualified group name */
  char *var_nm_fll; /* [sng] Fully qualified variable name */
  char *grp_nm_fll_sls; /* [sng] Fully qualified group name plus terminating '/' */
  char *grp_nm_fll_sls_ptr; /* [sng] Pointer to first character following last slash */
  char *var_nm_fll_sls_ptr; /* Pointer to first character following last slash */
  char grp_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  
  int *grp_ids; /* [ID] Group IDs of children */
  int *var_ids;
  int grp_id; /* [ID] Group ID */
  int grp_idx;
  int grp_nbr; /* [nbr] Number of groups in input file */
  int idx;
  int jdx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_idx;
  int var_idx_crr; /* [idx] Variable index accounting for previous groups */
  int var_nbr; /* [nbr] Number of variables in current group */
  int var_nbr_all; /* [nbr] Number of variables in input file */
  int var_nbr_fst=0; /* [nbr] Number of variables before current group */
  int var_nbr_tmp;
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
  
  nco_bool FLG_ROOT_GRP=True; /* [flg] Current group is root group */
  nco_bool *var_xtr_rqs=NULL; /* [flg] Variable specified in extraction list */

  nm_id_sct *var_lst_all=NULL; /* [sct] All variables in input file */
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */

  size_t grp_nm_lng;
  size_t grp_nm_sls_lng;

  /* Discover and return number of apex and all sub-groups */
  rcd+=nco_inq_grps_full(nc_id,&grp_nbr,(int *)NULL);

  grp_ids=(int *)nco_malloc(grp_nbr*sizeof(int)); /* [ID] Group IDs of children */

  /* Discover and return IDs of apex and all sub-groups */
  rcd+=nco_inq_grps_full(nc_id,&grp_nbr,grp_ids);

  /* Initialize variables that accumulate */
  var_nbr_all=0; /* [nbr] Total number of variables in file */

  /* Create list of all variables in input file */
  for(grp_idx=0;grp_idx<grp_nbr;grp_idx++){
    grp_id=grp_ids[grp_idx]; /* [ID] Group ID */

    /* Re-set Root group flag */
    FLG_ROOT_GRP=False; 

    /* How many variables in current group? */
    rcd+=nco_inq_varids(grp_id,&var_nbr,(int *)NULL);

    if(var_nbr > 0){
      /* Augment total number of variables in file */
      var_nbr_all+=var_nbr;

      /* Allocate space for and obtain variable IDs in current group */
      var_ids=(int *)nco_malloc(var_nbr*sizeof(int));
      rcd+=nco_inq_varids(grp_id,&var_nbr,var_ids);

      /* Allocate space for and obtain full name of current group */
      rcd+=nco_inq_grpname(grp_id,grp_nm);
      rcd+=nco_inq_grpname_len(grp_id,&grp_nm_lng);
      grp_nm_fll=(char *)nco_malloc((grp_nm_lng+1L)*sizeof(char));
      rcd+=nco_inq_grpname_full(grp_id,&grp_nm_lng,grp_nm_fll);

      /* Allocate space for full group name */
      if(!strcmp("/",grp_nm_fll)) FLG_ROOT_GRP=True;
      /* Root group does not need space for additional */
      if(FLG_ROOT_GRP) grp_nm_sls_lng=grp_nm_lng; else grp_nm_sls_lng=grp_nm_lng+1L;
      grp_nm_fll_sls=(char *)nco_malloc((grp_nm_lng+2L)*sizeof(char)); /* Add space for a trailing NUL */

      /* Copy canonical name into new space for full name with slash */
      grp_nm_fll_sls=strcpy(grp_nm_fll_sls,grp_nm_fll);

      /* Add trailing slash to group name 
	 Except this would cause full name of root group to be "//" */
      if(!FLG_ROOT_GRP) grp_nm_fll_sls=strcat(grp_nm_fll_sls,"/");

      var_nm_fll=(char *)nco_malloc((grp_nm_sls_lng+NC_MAX_NAME+1L)*sizeof(char)); /* [sng] Fully qualified variable name */
      var_nm_fll=strcpy(var_nm_fll,grp_nm_fll_sls);
      var_nm_fll_sls_ptr=var_nm_fll+grp_nm_sls_lng; /* [ptr] Pointer to first character following last slash */
      grp_nm_fll_sls_ptr=grp_nm_fll+grp_nm_sls_lng; /* [ptr] Pointer to first character following last slash */
    
      if(dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: INFO nco4_var_lst_mk() reports group %s, %s has %d variable%s:\n",prg_nm_get(),grp_nm,grp_nm_fll,var_nbr,(var_nbr > 1) ? "s" : "");

      /* Append all variables in current group to variable list */
      for(var_idx=0;var_idx<var_nbr;var_idx++){
	var_idx_crr=var_nbr_fst+var_idx; /* [idx] Variable index accounting for previous groups */
	var_lst_all=(nm_id_sct *)nco_realloc(var_lst_all,var_nbr_all*sizeof(nm_id_sct));

	/* Get name current variable in current group */
	(void)nco_inq_varname(grp_id,var_idx,var_nm);
	
	/* Tack variable name onto slash following group name */
	var_nm_fll_sls_ptr=(char *)strcat(var_nm_fll_sls_ptr,var_nm);
	
	/* Create full name of each variable */
	var_lst_all[var_idx_crr].grp_nm=(char *)strdup(grp_nm);
	var_lst_all[var_idx_crr].var_nm_fll=(char *)strdup(var_nm_fll);
	var_lst_all[var_idx_crr].nm=(char *)strdup(var_nm);
	var_lst_all[var_idx_crr].id=var_ids[var_idx];
	var_lst_all[var_idx_crr].grp_id=grp_id;
	
	if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"var_nm=%s, var_nm_fll=%s\n",var_nm,var_nm_fll);

	/* Full variable name has been duplicated, re-terminate with NUL for next variable */
	*var_nm_fll_sls_ptr='\0'; /* [ptr] Pointer to first character following last slash */
      } /* end loop over var_idx */

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);
      grp_nm_fll=(char *)nco_free(grp_nm_fll);
      var_nm_fll=(char *)nco_free(var_nm_fll);

      /* Offset for variables in next group by number of variables in this group */
      var_nbr_fst=var_nbr; /* [nbr] Number of variables before current group */

    } /* endif current group has variables */

  } /* end loop over grp */
  
  if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco4_var_lst_mk() reports file contains %d group%s comprising %d total variable%s\n",prg_nm_get(),grp_nbr,(grp_nbr > 1) ? "s" : "",var_nbr_all,(var_nbr_all > 1) ? "s" : "");

  /* Store results prior to first return */
  *nbr_var_fl=var_nbr_all; /* O [nbr] Number of variables in input file */

  /* Return all variables if none were specified and not -c ... */
  if(*var_nbr_xtr == 0 && !EXTRACT_ALL_COORDINATES){
    *var_nbr_xtr=var_nbr_all;
    return var_lst_all;
  } /* end if */
  
  /* Initialize and allocate extraction flag array to all False */
  var_xtr_rqs=(nco_bool *)nco_calloc((size_t)var_nbr_all,sizeof(nco_bool));
  
  /* Loop through user-specified variable list */
  for(idx=0;idx<*var_nbr_xtr;idx++){
    var_sng=var_lst_in[idx];
    
    /* Convert pound signs (back) to commas */
    while(*var_sng){
      if(*var_sng == '#') *var_sng=',';
      var_sng++;
    } /* end while */
    var_sng=var_lst_in[idx];
    
    /* If var_sng is regular expression ... */
    if(strpbrk(var_sng,".*^$\\[]()<>+?|{}")){
      /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
      rx_mch_nbr=nco_lst_meta_search(var_nbr_all,var_lst_all,var_sng,var_xtr_rqs);
      if(rx_mch_nbr == 0) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any variables\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),var_sng); 
      continue;
#else
      (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),var_sng);
      nco_exit(EXIT_FAILURE);
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
    } /* end if regular expression */
    
    /* Normal variable so search through variable array */
    for(jdx=0;jdx<var_nbr_all;jdx++)
      if(!strcmp(var_sng,var_lst_all[jdx].nm)) break;

    /* Mark any match as requested for inclusion by user */
    if(jdx != var_nbr_all){
      var_xtr_rqs[jdx]=True;
    }else{
      if(EXCLUDE_INPUT_LIST){ 
	/* Variable need not be present if list will be excluded later ... */
	if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco4_var_lst_mk() reports explicitly excluded variable \"%s\" is not in input file anyway\n",prg_nm_get(),var_sng); 
      }else{ /* !EXCLUDE_INPUT_LIST */
	/* Variable should be included but no matches found so die */
	(void)fprintf(stdout,"%s: ERROR nco4_var_lst_mk() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),var_sng); 
	nco_exit(EXIT_FAILURE);
      } /* !EXCLUDE_INPUT_LIST */
    } /* end else */

  } /* end loop over var_lst_in */
  
  /* Create final variable list using boolean flag array */
  
  /* malloc() xtr_lst to maximium size(var_nbr_all) */
  xtr_lst=(nm_id_sct *)nco_malloc(var_nbr_all*sizeof(nm_id_sct));
  var_nbr_tmp=0; /* var_nbr_tmp is incremented */
  for(idx=0;idx<var_nbr_all;idx++){
    /* Copy variable to extraction list */
    if(var_xtr_rqs[idx]){
      xtr_lst[var_nbr_tmp].grp_nm=(char *)strdup(var_lst_all[idx].grp_nm);
      xtr_lst[var_nbr_tmp].var_nm_fll=(char *)strdup(var_lst_all[idx].var_nm_fll);
      xtr_lst[var_nbr_tmp].nm=(char *)strdup(var_lst_all[idx].nm);
      xtr_lst[var_nbr_tmp].id=var_lst_all[idx].id;
      xtr_lst[var_nbr_tmp].grp_id=var_lst_all[idx].grp_id;
      var_nbr_tmp++;
    } /* end if */
  } /* end loop over var */
  
  /* re-alloc() list to actual size */  
  xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,var_nbr_tmp*sizeof(nm_id_sct));

  var_lst_all=(nm_id_sct *)nco_nm_id_lst_free(var_lst_all,var_nbr_all);
  var_xtr_rqs=(nco_bool *)nco_free(var_xtr_rqs);

  /* Store values for return */
  *var_nbr_xtr=var_nbr_tmp;    

  if(dbg_lvl_get() >= nco_dbg_var){
    (void)fprintf(stdout,"%s: INFO nco4_var_lst_mk() reports following %d variable%s matched sub-setting and regular expressions:\n",prg_nm_get(),*var_nbr_xtr,(*var_nbr_xtr > 1) ? "s" : "");
    for(idx=0;idx<*var_nbr_xtr;idx++) (void)fprintf(stdout,"var_nm = %s, var_nm_fll = %s\n",xtr_lst[idx].nm,xtr_lst[idx].var_nm_fll);
  } /* endif dbg */

  return xtr_lst;
} /* end nco4_var_lst_mk() */

int /* [rcd] Return code */
nco_grp_dfn /* [fnc] Define groups in output file */
(const int out_id, /* I [enm] netCDF output-file ID */
 nm_id_sct *grp_xtr_lst, /* [grp] Groups to be defined */
 const int grp_nbr) /* I [nbr] Number of groups to be defined */
{
  /* Purpose: Define groups in output file */
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int rcr_lvl=1; /* [nbr] Recursion level */

  if(dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO nco_grp_dfn() reports file level = 0 parent group = / (root group) will have %d sub-group%s\n",prg_nm_get(),grp_nbr,(grp_nbr == 1) ? "" : "s");

  /* For each (possibly user-specified) top-level group ... */
  for(idx=0;idx<grp_nbr;idx++){
    /* Define group and all subgroups */
    rcd+=nco_def_grp_rcr(grp_xtr_lst[idx].id,out_id,grp_xtr_lst[idx].nm,rcr_lvl);
  } /* end loop over top-level groups */

  return rcd;
} /* end nco_grp_dfn() */

int /* [rcd] Return code */
nco_def_grp_rcr
(const int in_id, /* I [enm] netCDF input-file ID */
 const int out_id, /* I [enm] netCDF output-file ID */
 const char * const prn_nm, /* I [sng] Parent group name */
 const int rcr_lvl) /* I [nbr] Recursion level */
{
  /* Purpose: Recursively define parent group and all subgroups in output file */
  char grp_nm[NC_MAX_NAME];

  int idx;
  int grp_nbr; /* I [nbr] Number of sub-groups */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int grp_in_ids[NC_MAX_DIMS]; /* [ID] Sub-group IDs in input file */ /* fxm: NC_MAX_GRPS? */
  int grp_out_ids[NC_MAX_DIMS]; /* [ID] Sub-group IDs in output file */

  /* How many and which sub-groups are in this group? */
  rcd+=nco_inq_grps(in_id,&grp_nbr,grp_in_ids);

  if(dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO nco_def_grp_rcr() reports file level = %d parent group = %s will have %d sub-group%s\n",prg_nm_get(),rcr_lvl,prn_nm,grp_nbr,(grp_nbr == 1) ? "" : "s");

  /* Define each group, recursively, in output file */
  for(idx=0;idx<grp_nbr;idx++){
    /* Obtain name of current group in input file */
    rcd+=nco_inq_grpname(grp_in_ids[idx],grp_nm);

    /* Define group of same name in output file */
    rcd+=nco_def_grp(out_id,grp_nm,grp_out_ids+idx);

    /* Define group and all sub-groups */
    rcd+=nco_def_grp_rcr(grp_in_ids[idx],grp_out_ids[idx],grp_nm,rcr_lvl+1);
  } /* end loop over sub-groups groups */

  return rcd;
} /* end nco_grp_dfn_rcr() */

nm_id_sct * /* O [sct] Group extraction list */
nco_grp_lst_mk /* [fnc] Create group extraction list using regular expressions */
(const int nc_id, /* I [enm] netCDF file ID */
 char * const * const grp_lst_in, /* I [sng] User-specified list of group names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 int * const grp_nbr_xtr) /* I/O [nbr] Number of groups in current extraction list */
{
  /* Purpose: Create group extraction list with or without regular expressions
     Code adapted from nco_var_lst_mk() and nearly identical in all respects
     Routine is currently (20110775) intended to be called either with 
     1. no user-specified groups (same as specifying all top-level groups), or,
     2. a list of user-specified (possibly regular expressions) of top-level groups
     Routine returns list of structures of all top-level groups matching input expression(s) */
  
  char *grp_sng; /* User-specified group name or regular expression */
  char grp_nm[NC_MAX_NAME];
  
  int grp_in_ids[NC_MAX_DIMS]; /* [ID] Sub-group IDs in input file */ /* fxm: NC_MAX_GRPS? */
  int grp_nbr_top; /* [nbr] Number of top-level groups */
  int idx;
  int jdx;
  int grp_nbr_tmp;
  int rcd=NC_NOERR; /* [rcd] Return code */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
  
  nm_id_sct *grp_lst=NULL; /* grp_lst may be alloc()'d from NULL with -c option */
  nm_id_sct *grp_lst_all=NULL; /* [sct] All groups in input file */
  nco_bool *grp_xtr_rqs=NULL; /* [flg] Group specified in extraction list */

  /* Find number of top-level groups */
  rcd+=nco_inq_grps(nc_id,&grp_nbr_top,grp_in_ids);

  /* Create list of all groups in input file */
  grp_lst_all=(nm_id_sct *)nco_malloc(grp_nbr_top*sizeof(nm_id_sct));
  for(idx=0;idx<grp_nbr_top;idx++){
    /* Get name of each group */
    (void)nco_inq_grpname(grp_in_ids[idx],grp_nm);
    grp_lst_all[idx].nm=(char *)strdup(grp_nm);
    grp_lst_all[idx].id=grp_in_ids[idx];
  } /* end loop over idx */
  
  /* Return all top-level groups if none were specified ... */
  if(*grp_nbr_xtr == 0){
    *grp_nbr_xtr=grp_nbr_top;
    return grp_lst_all;
  } /* end if */
  
  /* Initialize and allocate extraction flag array to all False */
  grp_xtr_rqs=(nco_bool *)nco_calloc((size_t)grp_nbr_top,sizeof(nco_bool));
  
  /* Loop through user-specified group list */
  for(idx=0;idx<*grp_nbr_xtr;idx++){
    grp_sng=grp_lst_in[idx];
    
    /* Convert pound signs (back) to commas */
    while(*grp_sng){
      if(*grp_sng == '#') *grp_sng=',';
      grp_sng++;
    } /* end while */
    grp_sng=grp_lst_in[idx];
    
    /* If grp_sng is regular expression ... */
    if(strpbrk(grp_sng,".*^$\\[]()<>+?|{}")){
      /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
      rx_mch_nbr=nco_lst_meta_search(grp_nbr_top,grp_lst_all,grp_sng,grp_xtr_rqs);
      if(rx_mch_nbr == 0) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any groups\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),grp_sng); 
      continue;
#else
      (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to groups) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),grp_sng);
      nco_exit(EXIT_FAILURE);
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
    } /* end if regular expression */
    
    /* Normal group so search through group array */
    for(jdx=0;jdx<grp_nbr_top;jdx++)
      if(!strcmp(grp_sng,grp_lst_all[jdx].nm)) break;

    if(jdx != grp_nbr_top){
      /* Mark this group as requested for inclusion by user */
      grp_xtr_rqs[jdx]=True;
    }else{
      if(EXCLUDE_INPUT_LIST){ 
	/* Group need not be present if list will be excluded later ... */
	if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_grp_lst_mk() reports explicitly excluded group \"%s\" is not in input file anyway\n",prg_nm_get(),grp_sng); 
      }else{ /* !EXCLUDE_INPUT_LIST */
	/* Group should be included but no matches found so die */
	(void)fprintf(stdout,"%s: ERROR nco_grp_lst_mk() reports user-specified top-level group \"%s\" is not in input file\n",prg_nm_get(),grp_sng); 
	nco_exit(EXIT_FAILURE);
      } /* !EXCLUDE_INPUT_LIST */
    } /* end else */

  } /* end loop over grp_lst_in */
  
  /* Create final group list using bool array */
  
  /* malloc() grp_lst to maximium size(grp_nbr_top) */
  grp_lst=(nm_id_sct *)nco_malloc(grp_nbr_top*sizeof(nm_id_sct));
  grp_nbr_tmp=0; /* grp_nbr_tmp is incremented */
  for(idx=0;idx<grp_nbr_top;idx++){
    /* Copy group to extraction list */
    if(grp_xtr_rqs[idx]){
      grp_lst[grp_nbr_tmp].nm=(char *)strdup(grp_lst_all[idx].nm);
      grp_lst[grp_nbr_tmp].id=grp_lst_all[idx].id;
      grp_nbr_tmp++;
    } /* end if */
    (void)nco_free(grp_lst_all[idx].nm);
  } /* end loop over var */
  
  /* realloc() list to actual size */  
  grp_lst=(nm_id_sct *)nco_realloc(grp_lst,grp_nbr_tmp*sizeof(nm_id_sct));

  grp_lst_all=(nm_id_sct *)nco_free(grp_lst_all);
  grp_xtr_rqs=(nco_bool *)nco_free(grp_xtr_rqs);

  *grp_nbr_xtr=grp_nbr_tmp;    
  return grp_lst;
} /* end nco_grp_lst_mk() */
