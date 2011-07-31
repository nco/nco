/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.c,v 1.11 2011-07-31 23:56:03 zender Exp $ */

/* Purpose: Group utilities */

/* Copyright (C) 2011--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Testing:
   eos52nc4 ~/nco/data/in.he5 ~/in.nc4
   export HDF5_DISABLE_VERSION_CHECK=1
   ncdump -h ~/in.nc4
   ncks -D 1 -m ~/in.nc4
   ncks -O -D 3 -g HIRDLS -m ~/in.nc4 ~/foo.nc
 */

#include "nco_grp_utl.h" /* Group utilities */

grp_stk_sct * /* O [sct] Pointer to stack */
nco_grp_stk_ntl /* [fnc] Initialize group stack */
(void)
{
  /* Purpose: Initialize dynamic array implementation of group stack */
  grp_stk_sct *grp_stk; /* O [sct] Pointer to stack */
  grp_stk=(grp_stk_sct *)nco_malloc(sizeof(grp_stk)); /* O [sct] Pointer to stack */
  grp_stk->grp_nbr=0; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=NULL; /* [ID] Group ID */
  return grp_stk;
} /* end nco_grp_stk_ntl() */

void
nco_grp_stk_psh /* [fnc] Push group ID onto stack */
(grp_stk_sct * const grp_stk, /* I/O [sct] Pointer to top of group stack */
 const int grp_id) /* I [ID] Group ID to push */
{
  /* Purpose: Push group ID onto dynamic array implementation of stack */
  grp_stk->grp_nbr++; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=(int *)nco_realloc(grp_stk,(grp_stk->grp_nbr)*sizeof(int)); /* O [sct] Pointer to group IDs */
  grp_stk->grp_id[grp_stk->grp_nbr-1]=grp_id; /* [ID] Group ID */
} /* end nco_grp_stk_psh() */

int /* O [ID] Group ID that was popped */
nco_grp_stk_pop /* [fnc] Remove and return group ID from stack */
(grp_stk_sct * const grp_stk) /* I/O [sct] Pointer to top of stack */
{
  /* Purpose: Remove and return group ID from dynamic array implementation of stack */
  int grp_id;
  grp_id=grp_stk->grp_id[grp_stk->grp_nbr]; /* [ID] Group ID that was popped */

  if(grp_stk->grp_nbr == 0){
    (void)fprintf(stderr,"%s: ERROR nco_grp_stk_pop() asked to pop empty stack\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  grp_stk->grp_nbr--; /* [nbr] Number of items in stack = number of elements in grp_id array */
  grp_stk->grp_id=(int *)nco_realloc(grp_stk,(grp_stk->grp_nbr)*sizeof(int)); /* O [sct] Pointer to group IDs */

  return grp_id;
} /* end nco_grp_stk_pop() */

nm_id_sct * /* O [sct] Variable extraction list */
nco_var4_lst_mk /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id, /* I [enm] netCDF group ID (root ID of input file) */
 char * const * const var_lst_in, /* I [sng] User-specified list of variable names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 const nco_bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const var_nbr_xtr) /* I/O [nbr] Number of variables in current extraction list */
{
  /* Purpose: Create variable extraction list with or without regular expressions */
  
  char *var_sng; /* User-specified variable name or regular expression */
  char *grp_nm_fll; /* [sng] Fully qualified group name */
  char *grp_nm_fll_sls; /* [sng] Fully qualified group name plus terminating '/' */
  char *grp_nm_fll_sls_ptr; /* [sng] Pointer to first character following last slash */
  char *var_nm_fll_sls_ptr; /* Pointer to first character following last slash */
  char grp_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  char var_nm_fll[NC_MAX_NAME*NCO_MAX_GROUP_DEPTH]; /* [sng] Fully qualified variable name */
  
  int *var_ids;
  int idx;
  int jdx;
  int var_idx;
  int grp_idx;
  int var_nbr_tmp;
  int grp_nbr_all; /* [nbr] Number of groups in input file */
  int var_nbr_all; /* [nbr] Number of variables in input file */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
  
  nco_bool *var_xtr_rqs=NULL; /* [flg] Variable specified in extraction list */

  nm_id_sct *var_lst_all=NULL; /* [sct] All variables in input file */
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */

  size_t grp_nm_lng;

  static short FIRST_INVOCATION=True;

  /* Free any old space */
  fl_nm=(char *)nco_free(fl_nm);

  /* Initialize accumulators for file contents */
  if(FIRST_INVOCATION){
    /* Top-level (root group) counts as group */
    grp_nbr_all=1; /* [nbr] Total number of groups in file */
    var_nbr_all=0; /* [nbr] Total number of variables in file */
    nc_id=grp_id_root; /* [enm] netCDF ID of current group */
    fl_lvl_crr=0; /* [nbr] Depth of current group */
  }else{ /* end if FIRST_INVOCATION */
    ;
  } /* end if not FIRST_INVOCATION */

  /* Create list of all variables in input file */

  while(grp_nbr_rmn > 0 && fl_lvl != 0){
  }

  /* Allocate space for and obtain full name of current group */
  rcd+=nco_inq_grpname(nc_id,grp_nm);
  rcd+=nco_inq_grpname_len(nc_id,&grp_nm_lng);
  grp_nm_fll=(char *)nco_malloc((grp_nm_lng+1L)*sizeof(char));
  rcd+=nco_inq_grpname_full(nc_id,&grp_nm_lng,grp_nm_fll);

  grp_nm_fll_sls=(char *)nco_malloc((grp_nm_lng+2L)*sizeof(char)); /* Add space for a '/' character */
  grp_nm_fll_sls=strcpy(grp_nm_fll_sls,grp_nm_fll);
  grp_nm_fll_sls=strcat(grp_nm_fll_sls,"/");
  var_nm_fll=strcpy(var_nm_fll,grp_nm_fll_sls);
  var_nm_fll_sls_ptr=var_nm_fll+grp_nm_lng+1; /* [ptr] Pointer to first character following last slash */
  grp_nm_fll_sls_ptr=grp_nm_fll+grp_nm_lng+1; /* [ptr] Pointer to first character following last slash */

  /* Allocate space for and obtain variable IDs in current group */
  rcd+=nco_inq_varids(nc_id,&var_nbr_crr,(int *)NULL);
  var_ids=(int *)nco_malloc(var_nbr_crr*sizeof(int));
  rcd+=nco_inq_varids(nc_id,&var_nbr_crr,var_ids);

  var_nbr_all+=var_nbr_crr; /* [nbr] Total number of variables in file */

  if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_var4_lst_mk() reports group %s, %s has %d variables:\n",prg_nm_get(),grp_nm,grp_nm_fll,var_nbr_crr);

  for(var_idx=0;var_idx<var_nbr_crr;var_idx++){
    /* Get name of each variable in current group */
    (void)nco_inq_varname(nc_id,var_idx,var_nm);

    /* Tack variable name onto slash following group name */
    var_nm_fll_sls_ptr=(char *)strcat(var_nm_fll_sls_ptr,var_nm);

    /* Create full name of each variable */
    var_lst_all[var_idx].grp_nm=(char *)strdup(grp_nm);
    var_lst_all[var_idx].var_nm_fll=(char *)strdup(var_nm_fll);
    var_lst_all[var_idx].nm=(char *)strdup(var_nm);
    var_lst_all[var_idx].id=var_ids[var_idx];

    if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_var4_lst_mk() reports var_nm=%s, var_nm_fll=%s\n",prg_nm_get(),var_nm,var_nm_fll);
  } /* end loop over var_idx */

  if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_var4_lst_mk() reports file has %d named groups and %d total variables\n",prg_nm_get(),grp_nm,grp_nm_fll,grp_nbr_all,var_nbr_all);

  /* Memory management after current group */
  var_ids=(int *)nco_free(var_ids);
  grp_nm_fll=(char *)nco_free(grp_nm_fll);

  /* Create list of all variables in input file */
  var_lst_all=(nm_id_sct *)nco_malloc(var_nbr_all*sizeof(nm_id_sct));
  for(idx=0;idx<var_nbr_all;idx++){
    /* Get name of each variable */
    (void)nco_inq_varname(nc_id,idx,var_nm);
    var_lst_all[idx].nm=(char *)strdup(var_nm);
    var_lst_all[idx].id=idx;
  } /* end loop over idx */
  
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

    if(jdx != var_nbr_all){
      /* Mark this variable as requested for inclusion by user */
      var_xtr_rqs[jdx]=True;
    }else{
      if(EXCLUDE_INPUT_LIST){ 
	/* Variable need not be present if list will be excluded later ... */
	if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_var4_lst_mk() reports explicitly excluded variable \"%s\" is not in input file anyway\n",prg_nm_get(),var_sng); 
      }else{ /* !EXCLUDE_INPUT_LIST */
	/* Variable should be included but no matches found so die */
	(void)fprintf(stdout,"%s: ERROR nco_var4_lst_mk() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),var_sng); 
	nco_exit(EXIT_FAILURE);
      } /* !EXCLUDE_INPUT_LIST */
    } /* end else */

  } /* end loop over var_lst_in */
  
  /* Create final variable list using bool array */
  
  /* malloc() xtr_lst to maximium size(var_nbr_all) */
  xtr_lst=(nm_id_sct *)nco_malloc(var_nbr_all*sizeof(nm_id_sct));
  var_nbr_tmp=0; /* var_nbr_tmp is incremented */
  for(idx=0;idx<var_nbr_all;idx++){
    /* Copy variable to extraction list */
    if(var_xtr_rqs[idx]){
      xtr_lst[var_nbr_tmp].nm=(char *)strdup(var_lst_all[idx].nm);
      xtr_lst[var_nbr_tmp].id=var_lst_all[idx].id;
      var_nbr_tmp++;
    } /* end if */
    (void)nco_free(var_lst_all[idx].nm);
  } /* end loop over var */
  
  /* realloc() list to actual size */  
  xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,var_nbr_tmp*sizeof(nm_id_sct));

  var_lst_all=(nm_id_sct *)nco_free(var_lst_all);
  var_xtr_rqs=(nco_bool *)nco_free(var_xtr_rqs);

  *var_nbr_xtr=var_nbr_tmp;    
  return xtr_lst;
} /* end nco_var4_lst_mk() */

int /* [rcd] Return code */
nco_grp_dfn /* [fnc] Define groups in output file */
(const int in_id, /* I [enm] netCDF input-file ID */
 const int out_id, /* I [enm] netCDF output-file ID */
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
