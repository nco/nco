/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.c,v 1.325 2012-12-16 19:50:54 pvicente Exp $ */

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

#include "nco_grp_utl.h"  /* Group utilities */

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

int /* O [rcd] Return code */
nco_def_grp_full /* [fnc] Ensure all components of group path are defined */
(const int nc_id, /* I [ID] netCDF output-file ID */
 const char * const grp_nm_fll, /* I [sng] Full group name */
 int * const grp_out_id) /* O [ID] Deepest group ID */
{
  /* Purpose: Ensure all components of full path name exist and return ID of deepest group */
  char *grp_pth=NULL; /* [sng] Full group path */
  char *grp_pth_dpl=NULL; /* [sng] Full group path memory duplicate */
  char *sls_ptr; /* [sng] Pointer to slash */

  int grp_id_prn; /* [ID] Parent group ID */
  int rcd=NC_NOERR;

  /* Initialize defaults */
  *grp_out_id=nc_id;

  grp_pth_dpl=(char *)strdup(grp_nm_fll);
  grp_pth=grp_pth_dpl;

  /* No need to try to define root group */
  if(grp_pth[0] == '/') grp_pth++;

  /* Define everything necessary beneath root group */
  while(strlen(grp_pth)){
    /* Terminate path at next slash, if any */
    sls_ptr=strchr(grp_pth,'/');

    /* Replace slash by NUL */
    if(sls_ptr) *sls_ptr='\0';
    
    /* Identify parent group */
    grp_id_prn=*grp_out_id;
    
    /* If current group is not defined, define it */
    if(nco_inq_ncid_flg(grp_id_prn,grp_pth,grp_out_id)) nco_def_grp(grp_id_prn,grp_pth,grp_out_id);

    /* Point to next group, if any */
    if(sls_ptr) grp_pth=sls_ptr+1; else break;
  } /* end while */

  grp_pth_dpl=(char *)nco_free(grp_pth_dpl);
  return rcd;
}  /* end nco_def_grp_full() */

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

nm_id_sct *                               /* O [sct] Variable extraction list */
nco_var_lst_mk_trv                        /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id,                         /* I [ID] Apex group ID */
 char * const * const grp_lst_in,         /* I [sng] User-specified list of groups names to extract (specified with -g) */
 const int grp_xtr_nbr,                   /* I [nbr] Number of groups in current extraction list (specified with -g) */
 char * const * const var_lst_in,         /* I [sng] User-specified list of variable names and rx's */
 const trv_tbl_sct * const trv_tbl,       /* I [sct] Group traversal table */
 const nco_bool EXTRACT_ALL_COORDINATES,  /* I [flg] Process all coordinates */
 int * const var_xtr_nbr)                 /* I/O [nbr] Number of variables in current extraction list */
{
  /* Purpose: Create variable extraction list with or without regular expressions */

  char *var_sng;           /* User-specified variable name or regular expression */
  char *grp_nm_fll;        /* [sng] Fully qualified group name */
  char *var_nm_fll;        /* [sng] Fully qualified variable name */
  char *grp_nm_fll_sls;    /* [sng] Fully qualified group name plus terminating '/' */
  char *var_nm_fll_sls_ptr;/* Pointer to first character following last slash */
  char grp_nm[NC_MAX_NAME];/* [sng] Relative group name */
  char var_nm[NC_MAX_NAME];/* [sng] Relative variable name */
  int *grp_ids;            /* [ID] Group IDs of children */
  int *var_ids;            /* [ID] Variable IDs */
  int grp_id;              /* [ID] Group ID */
  int grp_nbr;             /* [nbr] Number of groups in input file */
  int rcd=NC_NOERR;        /* [rcd] Return code */
  int idx_var;             /* [idx] Var index */
  int idx_grp;             /* [idx] Group index */
  int idx_var_crr;         /* [idx] Total variable index */
  int var_nbr;             /* [nbr] Number of variables in current group */
  int var_nbr_all;         /* [nbr] Number of variables in input file */
  int var_nbr_tbl;         /* Number of variables in table list (table list stores all paths, groups and variables ) */
  int var_nbr_tmp;
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
  nco_bool *var_xtr_rqs=NULL; /* [flg] Variable specified in extraction list */
  nco_bool FLG_ROOT_GRP=True; /* [flg] Current group is root group */
  nm_id_sct *var_lst_all=NULL;/* [sct] All variables in input file */
  nm_id_sct *xtr_lst=NULL;    /* xtr_lst may be alloc()'d from NULL with -c option */
  size_t grp_nm_lng;
  size_t grp_nm_sls_lng;
  int fl_fmt;                 /* [enm] netCDF file format */

  /* Get file format */
  (void)nco_inq_format(nc_id,&fl_fmt);

  /* Discover and return number of apex and all sub-groups */
  rcd+=nco_inq_grps_full(nc_id,&grp_nbr,(int *)NULL);

  grp_ids=(int *)nco_malloc(grp_nbr*sizeof(int)); /* [ID] Group IDs of children */

  /* Discover and return IDs of apex and all sub-groups */
  rcd+=nco_inq_grps_full(nc_id,&grp_nbr,grp_ids);

  /* Initialize variables that accumulate */
  idx_var_crr=0; /* [idx] Index into full variable list */
  var_nbr_all=0; /* [nbr] Total number of variables in file */

  /* Create list of all variables in input file */
  for(idx_grp=0;idx_grp<grp_nbr;idx_grp++){
    grp_id=grp_ids[idx_grp]; /* [ID] Group ID */

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

      /* Add trailing slash to group name except this would cause full name of root group to be "//" */
      if(!FLG_ROOT_GRP) grp_nm_fll_sls=strcat(grp_nm_fll_sls,"/");

      var_nm_fll=(char *)nco_malloc((grp_nm_sls_lng+NC_MAX_NAME+1L)*sizeof(char)); /* [sng] Fully qualified variable name */
      var_nm_fll=strcpy(var_nm_fll,grp_nm_fll_sls);
      var_nm_fll_sls_ptr=var_nm_fll+grp_nm_sls_lng; /* [ptr] Pointer to first character following last slash */

      /* Append all variables in current group to variable list */
      for(idx_var=0;idx_var<var_nbr;idx_var++){
        var_lst_all=(nm_id_sct *)nco_realloc(var_lst_all,var_nbr_all*sizeof(nm_id_sct));

        /* Get name current variable in current group */
        (void)nco_inq_varname(grp_id,idx_var,var_nm);

        /* Tack variable name onto slash following group name */
        var_nm_fll_sls_ptr=(char *)strcat(var_nm_fll_sls_ptr,var_nm);

        /* Create full name of each variable */
        var_lst_all[idx_var_crr].var_nm_fll=(char *)strdup(var_nm_fll);
        var_lst_all[idx_var_crr].nm=(char *)strdup(var_nm);
        var_lst_all[idx_var_crr].id=var_ids[idx_var];
        var_lst_all[idx_var_crr].grp_nm_fll=(char *)strdup(grp_nm_fll);

        /* Increment number of variables */
        idx_var_crr++;

        /* Full variable name has been duplicated, re-terminate with NUL for next variable */
        *var_nm_fll_sls_ptr='\0'; /* [ptr] Pointer to first character following last slash */
      } /* end loop over idx_var */

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);
      grp_nm_fll=(char *)nco_free(grp_nm_fll);
      var_nm_fll=(char *)nco_free(var_nm_fll);

    } /* endif current group has variables */

  } /* end loop over grp */

  /* Clean up memory */
  grp_ids=(int *)nco_free(grp_ids);

#ifdef NCO_SANITY_CHECK
  var_nbr_tbl=0; /* Number of variables in table list (table list stores all paths, groups and variables) */
  for(unsigned int uidx=0;uidx<trv_tbl->nbr;uidx++){
    if(trv_tbl->lst[uidx].typ == nco_obj_typ_var) var_nbr_tbl++; 
  }
  assert(var_nbr_tbl == var_nbr_all);
  idx_var_crr=0;
  for(unsigned int uidx=0;uidx<trv_tbl->nbr;uidx++){
    if(idx_var_crr == var_nbr_all) break;
    trv_sct trv=trv_tbl->lst[uidx];
    nm_id_sct nm_id=var_lst_all[idx_var_crr]; 
    /* Increment var_lst_all index only when table object is a variable; this keeps two lists in sync */
    if (trv_tbl->lst[uidx].typ == nco_obj_typ_var){
      /* Match both lists */
      assert(!strcmp(nm_id.nm,trv.nm));
      assert(!strcmp(nm_id.var_nm_fll,trv.nm_fll));
      idx_var_crr++; 
    } /* end nco_obj_typ_var */
  } /* end uidx */
#endif /* NCO_SANITY_CHECK */

  /* Return all variables if none were specified and not -c ... */
  if(*var_xtr_nbr == 0 && grp_xtr_nbr == 0 && !EXTRACT_ALL_COORDINATES){
    *var_xtr_nbr=var_nbr_all;
    return var_lst_all;
  } /* end if */

  /* Initialize and allocate extraction flag array to all False */
  var_xtr_rqs=(nco_bool *)nco_calloc((size_t)var_nbr_all,sizeof(nco_bool));

  /* Case 1: -v was specified (regardles of whether -g was specified)
  Method: Outer loop over all objects in file contains inner loop over user-supplied variable list
  Add variable to extraction list if it matches user-supplied name
  Regular expressions are allowed */
  if(*var_xtr_nbr){
    idx_var_crr=0;
    /* Loop over all objects in file */
    for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
      /* No need to look through more objects if extraction list already contains all variables in file */
      if(idx_var_crr == var_nbr_all) break;    

      /* Loop through user-specified variable list */
      for(int idx=0;idx<*var_xtr_nbr;idx++){
        var_sng=var_lst_in[idx];
        /* Convert pound signs (back) to commas */
        nco_hash2comma(var_sng);

        /* If var_sng is regular expression ... */
        if(strpbrk(var_sng,".*^$\\[]()<>+?|{}")){
          /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
          rx_mch_nbr=nco_lst_rx_search(var_nbr_all,var_lst_all,var_sng,var_xtr_rqs);
          if(!rx_mch_nbr) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any variables\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),var_sng); 
          continue;
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
          (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),var_sng);
          nco_exit(EXIT_FAILURE);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
        } /* end if regular expression */

        /* Compare var_nm from main iteration with var_sng found and, if equal, add to extraction list */
        if(!strcmp(var_sng,var_lst_all[idx_var_crr].nm)){
          if(!grp_xtr_nbr){
            /* No groups specified with -g, so add variable to extraction list */
            var_xtr_rqs[idx_var_crr]=True;
          }else{ /* grp_xtr_nbr */
            /* Groups specified with -g, so add variable to extraction list only if in matching group */
            for(idx_grp=0;idx_grp<grp_xtr_nbr;idx_grp++){
              if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
                (void)nco_inq_grp_full_ncid(nc_id,var_lst_all[idx_var_crr].grp_nm_fll,&grp_id);
              }else{ 
                grp_id=nc_id;
              } 
              (void)nco_inq_grpname(grp_id,grp_nm);
              if(!strcmp(grp_nm,grp_lst_in[idx_grp])){
                var_xtr_rqs[idx_var_crr]=True;
              } /* end strcmp() */
            } /* end idx_grp */
          } /* end grp_xtr_nbr */
        }  /* end strcmp() */
      } /* end loop over var_lst_in */ 

      /* Increment variable index for var_lst_all if table object is a variable; this keeps two lists in sync */
      if(trv_tbl->lst[uidx].typ == nco_obj_typ_var){
#ifdef NCO_SANITY_CHECK
        /* Full variable names in both lists must agree */
        assert(!strcmp(var_lst_all[idx_var_crr].var_nm_fll,trv_tbl->lst[uidx].nm_fll));
#endif /* !NCO_SANITY_CHECK */
        idx_var_crr++; 
      } /* end nco_obj_typ_var */
    } /* end loop over trv_tbl uidx */

  }else if(grp_xtr_nbr && *var_xtr_nbr == 0){ 

    /* Case 2: -v was not specified and -g was
    Regular expressions are not yet allowed in -g arguments */

#ifdef NCO_SANITY_CHECK
    /* idx_var_crr was used to traverse all groups and variables to make var_lst_all */
    assert(var_nbr_all == idx_var_crr);
#endif /* !NCO_SANITY_CHECK */

    for(idx_grp=0;idx_grp<grp_xtr_nbr;idx_grp++){ /* Outer loop over user-specified group list */
      for(idx_var_crr=0;idx_var_crr<var_nbr_all;idx_var_crr++){ /* Inner loop over variables */
        if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
          (void)nco_inq_grp_full_ncid(nc_id,var_lst_all[idx_var_crr].grp_nm_fll,&grp_id);
        }else{ 
          grp_id=nc_id;
        } 
        (void)nco_inq_grpname(grp_id,grp_nm);
        if(!strcmp(grp_nm,grp_lst_in[idx_grp])){
          var_xtr_rqs[idx_var_crr]=True;
        } /* end strcmp */
      } /* end idx_var_crr */
    } /* end idx_grp */
  } /* end Case 2 */

  /* Create final variable list using boolean flag array */

  /* malloc() xtr_lst to maximium size(var_nbr_all) */
  xtr_lst=(nm_id_sct *)nco_malloc(var_nbr_all*sizeof(nm_id_sct));
  var_nbr_tmp=0; /* var_nbr_tmp is incremented */
  for(int idx=0;idx<var_nbr_all;idx++){
    /* Copy variable to extraction list */
    if(var_xtr_rqs[idx]){
      xtr_lst[var_nbr_tmp].var_nm_fll=(char *)strdup(var_lst_all[idx].var_nm_fll);
      xtr_lst[var_nbr_tmp].nm=(char *)strdup(var_lst_all[idx].nm);
      xtr_lst[var_nbr_tmp].id=var_lst_all[idx].id;
      xtr_lst[var_nbr_tmp].grp_nm_fll=(char *)strdup(var_lst_all[idx].grp_nm_fll);
      var_nbr_tmp++;
    } /* end if */
  } /* end loop over var */

  /* realloc() list to actual size */  
  xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,var_nbr_tmp*sizeof(nm_id_sct));

  var_lst_all=(nm_id_sct *)nco_nm_id_lst_free(var_lst_all,var_nbr_all);
  var_xtr_rqs=(nco_bool *)nco_free(var_xtr_rqs);

  /* Store values for return */
  *var_xtr_nbr=var_nbr_tmp;  
  return xtr_lst;
} /* end nco_var_lst_mk_trv() */

int /* [rcd] Return code */
nco_grp_dfn /* [fnc] Define groups in output file */
(const int out_id, /* I [ID] netCDF output-file ID */
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
(const int in_id, /* I [ID] netCDF input-file ID */
 const int out_id, /* I [ID] netCDF output-file ID */
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
(const int nc_id, /* I [ID] netCDF file ID */
 char * const * const grp_lst_in, /* I [sng] User-specified list of group names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 int * const grp_xtr_nbr) /* I/O [nbr] Number of groups in current extraction list */
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
  if(*grp_xtr_nbr == 0){
    *grp_xtr_nbr=grp_nbr_top;
    return grp_lst_all;
  } /* end if */
  
  /* Initialize and allocate extraction flag array to all False */
  grp_xtr_rqs=(nco_bool *)nco_calloc((size_t)grp_nbr_top,sizeof(nco_bool));
  
  /* Loop through user-specified group list */
  for(idx=0;idx<*grp_xtr_nbr;idx++){
    grp_sng=grp_lst_in[idx];
    /* Convert pound signs (back) to commas */
    nco_hash2comma(grp_sng);

    /* If grp_sng is regular expression ... */
    if(strpbrk(grp_sng,".*^$\\[]()<>+?|{}")){
      /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
      rx_mch_nbr=nco_lst_rx_search(grp_nbr_top,grp_lst_all,grp_sng,grp_xtr_rqs);
      if(!rx_mch_nbr) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any groups\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),grp_sng); 
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

  *grp_xtr_nbr=grp_nbr_tmp;    
  return grp_lst;
} /* end nco_grp_lst_mk() */

nm_id_sct *                              /* O [sct] Extraction list */
nco_var_lst_xcl_trv                      /* [fnc] Convert exclusion list to extraction list */
(const int nc_id,                        /* I [ID] netCDF file ID */
 nm_id_sct *xtr_lst,                     /* I/O [sct] Current exclusion list (destroyed) */
 int * const xtr_nbr,                    /* I/O [nbr] Number of variables in exclusion/extraction list */
 const trv_tbl_sct * const trv_tbl)      /* I [sct] Traversal table */
{
  /* Purpose: Convert exclusion list to extraction list
     User wants to extract all variables except those currently in list
     It is hard to edit existing list so copy existing extraction list into 
     exclusion list, then construct new extraction list from scratch. */

  int nbr_var_xtr;   /* Number of variables to extract */
  int nbr_var;       /* Number of variables in the table/file */      
  int  grp_id;       /* Group ID */
  int  var_id;       /* Variable ID */
  int fl_fmt;        /* [enm] netCDF file format */
  int idx;
  int nbr_xcl;
  unsigned int uidx;
 
  /* Traverse the full list trv_tbl; if a name in xtr_lst (input extraction list) is found, mark it as flagged;
  A second traversal extracts all variables that are not marked (this reverses the list);
  The second traversal is needed because we need to find nbr_xcl, the number of variables to exclude, first
  */
  nbr_var_xtr=0;
  nbr_var=0;
  for(uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_var){ /* trv_tbl lists non-variables also; filter just variables */
      nbr_var++;
      for(idx=0;idx<*xtr_nbr;idx++){
        /* Compare variable name between full list and input extraction list */
        if(strcmp(xtr_lst[idx].var_nm_fll,trv.nm_fll) == 0){
          trv_tbl->lst[uidx].xcl_flg=True;
          nbr_var_xtr++;
          break;
        } /* endif strcmp */
      } /* end idx */
    } /* end nco_obj_typ_var */
  } /* end loop over uidx */

#ifdef NCO_SANITY_CHECK
  assert(nbr_var_xtr == *xtr_nbr);
#endif

  /* Variables to exclude = Total variables - Variables to extract */
  nbr_xcl=nbr_var - *xtr_nbr;

  /* Second traversal: extracts all variables that are not marked (this reverses the list); the xtr_lst must be reconstructed */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,*xtr_nbr);
  xtr_lst=(nm_id_sct *)nco_malloc(nbr_xcl*sizeof(nm_id_sct));

  /* Initialize index of extracted variables */
  int idx_xtr=0;

  for(uidx=0,idx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_var && trv.xcl_flg != True ){ 
      /* Extract the full group name from 'trv', that contains the full variable name, to xtr_lst */

      (void)nco_inq_format(nc_id,&fl_fmt);
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.grp_nm_fll,&grp_id);
      }else{ 
        grp_id=nc_id;
      } 

      /* Obtain variable ID from netCDF API using group ID */
      (void)nco_inq_varid(grp_id,trv.nm,&var_id);

      /* ncks needs only:
      1) xtr_lst.grp_nm_fll (full group name where variable resides, to get group ID) 
      2) xtr_lst.var_nm_fll
      3) xtr_lst.id
      4) xtr_lst.nm (relative variable name) 
      */
      xtr_lst[idx_xtr].nm=(char*)strdup(trv.nm);
      xtr_lst[idx_xtr].grp_nm_fll=(char*)strdup(trv.grp_nm_fll);
      xtr_lst[idx_xtr].var_nm_fll=(char*)strdup(trv.nm_fll);
      xtr_lst[idx_xtr].id=var_id;
      /* Increment index of extracted variables */
      idx_xtr++;
    }
  } /* end loop over uidx */

  /* Export */
  *xtr_nbr=nbr_xcl;

  /* Reset mark field */
  for(uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_tbl->lst[uidx].xcl_flg=nco_obj_typ_err;
  }

  return xtr_lst;
} /* end nco_var_lst_xcl_trv() */

void
nco_grp_var_mk_trv                     /* [fnc] Create groups/write variables in output file */
(const int nc_id,                      /* I [ID] netCDF input file ID  */
 const int nc_out_id,                  /* I [ID] netCDF output file ID  */
 const gpe_sct * const gpe,            /* I [sng] GPE structure */
 nm_id_sct * const xtr_lst,            /* I [sct] Extraction list  */
 const int xtr_nbr,                    /* I [nbr] Number of members in list */
 const int lmt_nbr,                    /* I [nbr] Number of dimensions with limits */
 lmt_all_sct * const * lmt_all_lst,    /* I multi-hyperslab limits */
 const int lmt_all_lst_nbr,            /* I [nbr] Number of hyperslab limits */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 nco_bool PRN_VAR_METADATA,            /* I [flg] Copy variable metadata (attributes) */
 int * const cnk_map_ptr,              /* I [enm] Chunking map */
 int * const cnk_plc_ptr,              /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 FILE * const fp_bnr,                  /* I [fl] Unformatted binary output file handle */
 const nco_bool MD5_DIGEST,            /* I [flg] Perform MD5 digests */
 const nco_bool NCO_BNR_WRT,           /* I [flg] Write binary file */
 const nco_bool DEF_MODE,              /* I [flg] netCDF define mode is true */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] Traversal table */
{
  /* Purpose: */

  char *rec_dmn_nm;              /* [sng] Record dimension name */
  char *var_nm_fll;              /* [sng] Full path of variable */
  char *grp_out_fll;             /* [sng] Group name */
  char dmn_ult_nm[NC_MAX_NAME+1];/* [sng] Unlimited dimension name */ 
  char var_nm[NC_MAX_NAME+1];    /* [sng] Variable name */ 

  gpe_nm_sct *gpe_nm;            /* [sct] GPE name duplicate check array  */

  int *var_ids;                  /* [ID]  Variable IDs array */

  int dmn_ids_ult[NC_MAX_DIMS];  /* [ID]  Unlimited dimensions IDs array */

  int fl_fmt;                    /* [enm] netCDF file format */
  int grp_id;                    /* [ID]  Group ID in input file */
  int grp_out_id;                /* [ID]  Group ID in output file */ 
  int nbr_att;                   /* [nbr] Number of attributes for group */
  int nbr_dmn;                   /* [nbr] Number of dimensions for group */
  int nbr_dmn_ult;               /* [nbr] Number of unlimited dimensions */
  int nbr_gpe_nm;                /* [nbr] Number of GPE entries */
  int nbr_grp;                   /* [nbr] Number of groups for group */
  int nbr_var;                   /* [nbr] Number of variables for group */
  int var_out_id;                /* [ID]  Variable ID in output file */

  nbr_gpe_nm=0;
  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_grp ){

      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=nc_id;
      }

      /* Edit group name */
      if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv.nm_fll); else grp_out_fll=(char *)strdup(trv.nm_fll);

      /* Obtain info for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
      (void)nco_inq_grps(grp_id,&nbr_grp,(int *)NULL);
#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att && nbr_grp == trv.nbr_grp);
#endif

      /* Allocate space for and obtain variable IDs in current group */
      var_ids=(int *)nco_malloc(nbr_var*sizeof(int));
      (void)nco_inq_varids(grp_id,&nbr_var,var_ids);

      /* Get number of unlimited dimensions */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      /* Get variables for this group */
      for(int idx_var=0;idx_var<nbr_var;idx_var++){
        var_nm_fll=NULL;

        /* Get name of current variable in current group NOTE: using obtained IDs array */
        (void)nco_inq_varname(grp_id,var_ids[idx_var],var_nm);

        /* Allocate path buffer; add space for a trailing NUL */ 
        var_nm_fll=(char*)nco_malloc(strlen(trv.nm_fll)+strlen(var_nm)+2L);

        /* Initialize path with the current absolute group path */
        strcpy(var_nm_fll,trv.nm_fll);

        /* If not root group, concatenate separator */
        if(strcmp(trv.nm_fll,"/")) strcat(var_nm_fll,"/");

        /* Concatenate variable to absolute group path */
        strcat(var_nm_fll,var_nm);

        /* Check if input variable is on extraction list; if yes, define it in the output file */
        for(int idx_xtr=0;idx_xtr<xtr_nbr;idx_xtr++){
          nm_id_sct xtr=xtr_lst[idx_xtr];

          /* If current variable is in extraction list, define it */
          if(!strcmp(var_nm_fll,xtr.var_nm_fll)){
            rec_dmn_nm=NULL;

            /* Search for record dimension name */
            for(int idx_dmn=0;idx_dmn<nbr_dmn_ult;idx_dmn++){
              (void)nco_inq_dimname(grp_id,dmn_ids_ult[idx_dmn],dmn_ult_nm);
              if(!strcmp(var_nm,dmn_ult_nm)){
                rec_dmn_nm=(char *)nco_malloc((NC_MAX_NAME+1L)*sizeof(char));
                strcpy(rec_dmn_nm,dmn_ult_nm);  
              } /* strcmp */
            } /* idx_dmn */     

            /* If output group does not exist, create it */
            if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

            /* Detect GPE duplicate names */
            char *gpe_var_nm_fll=NULL;  
            if(gpe && DEF_MODE){
              /* Construct the absolute GPE variable path */
              gpe_var_nm_fll=(char*)nco_malloc(strlen(grp_out_fll)+strlen(xtr.nm)+2L);
              strcpy(gpe_var_nm_fll,grp_out_fll);
              /* If not root group, concatenate separator */
              if(strcmp(grp_out_fll,"/")) strcat(gpe_var_nm_fll,"/");
              strcat(gpe_var_nm_fll,xtr.nm);

              /* GPE name is not already on the list, put it there */
              if(nbr_gpe_nm == 0){
                gpe_nm=(gpe_nm_sct *)nco_malloc((nbr_gpe_nm+1)*sizeof(gpe_nm_sct)); 
                gpe_nm[nbr_gpe_nm].var_nm_fll=strdup(gpe_var_nm_fll);
                nbr_gpe_nm++;
              }else{
                /* GPE might be already on the list, put it there only if not found */
                for(int idx_gpe=0;idx_gpe<nbr_gpe_nm;idx_gpe++){
                  if(!strcmp(gpe_var_nm_fll,gpe_nm[idx_gpe].var_nm_fll)){
                    (void)fprintf(stdout,"%s: ERROR nco_grp_var_mk_trv() reports variable %s already defined. HINT: Moving groups of flattening files can lead to over-determined situations where a single object name (e.g., a variable name) must refer to multiple objects in the same output group. The user's intent is ambiguous so instead of arbitrarily picking which (e.g., the last) variable of that name to place in the output file, NCO simply fails. User should re-try command after ensuring multiple objects of the same name will not be placed in the same group.\n",prg_nm_get(),gpe_var_nm_fll);
                    for(int idx=0;idx<nbr_gpe_nm;idx++)
                      gpe_nm[idx].var_nm_fll=(char *)nco_free(gpe_nm[idx].var_nm_fll);
                    nco_exit(EXIT_FAILURE);
                  } /* End string comparison */
                } /* End search in array */
                gpe_nm=(gpe_nm_sct *)nco_realloc((void *)gpe_nm,(nbr_gpe_nm+1)*sizeof(gpe_nm_sct));
                gpe_nm[nbr_gpe_nm].var_nm_fll=strdup(gpe_var_nm_fll);
                nbr_gpe_nm++;
              } /* End might be on list */
            }/* End GPE */

            /* Free full path name */
            if(gpe_var_nm_fll) gpe_var_nm_fll=(char *)nco_free(gpe_var_nm_fll);

            /* Define mode: create variables */
            if(DEF_MODE){

              /* Define variable */
              if(lmt_nbr > 0) var_out_id=nco_cpy_var_dfn_lmt(grp_id,grp_out_id,rec_dmn_nm,xtr.nm,lmt_all_lst,lmt_all_lst_nbr,dfl_lvl); 
              else var_out_id=nco_cpy_var_dfn(grp_id,grp_out_id,rec_dmn_nm,xtr.nm,dfl_lvl);

              /* Set chunksize parameters */
              if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC) (void)nco_cnk_sz_set(grp_out_id,lmt_all_lst,lmt_all_lst_nbr,cnk_map_ptr,cnk_plc_ptr,cnk_sz_scl,cnk,cnk_nbr);

              /* Copy variable's attributes */
              if(PRN_VAR_METADATA) (void)nco_att_cpy(grp_id,grp_out_id,xtr.id,var_out_id,(nco_bool)True);

            }else{ /* Write mode */

              /* Write output variable */
              if(lmt_nbr > 0) (void)nco_cpy_var_val_mlt_lmt(grp_id,grp_out_id,fp_bnr,MD5_DIGEST,NCO_BNR_WRT,xtr.nm,lmt_all_lst,lmt_all_lst_nbr); 
              else (void)nco_cpy_var_val(grp_id,grp_out_id,fp_bnr,MD5_DIGEST,NCO_BNR_WRT,xtr.nm);

            } /* Define mode */

            /* Memory management */
            if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm);

            /* Variable was found, exit */
            break;

          } /* end if on extraction list */

        } /* end extraction list loop */

        /* Memory management after current variable */
        var_nm_fll=(char *)nco_free(var_nm_fll);

      } /* end get variables for this group */  

      /* If there are group attributes, write them, avoid root case, these are always copied elsewhere */
      if(nbr_att && strcmp("/",trv.nm_fll) != 0){
        /* Check if group is on extraction list */
        for(int idx_lst=0;idx_lst<xtr_nbr;idx_lst++){
          nm_id_sct xtr=xtr_lst[idx_lst];

          /* Compare item on list with current group name (NOTE: using original xtr.grp_nm_fll full name to compare ) */
          if(strcmp(xtr.grp_nm_fll,trv.nm_fll) == 0){

            /* Obtain group ID from netCDF API using full group name (NOTE: using GPE grp_out_fll name to write ) */
            nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_out_id);
            /* Copy global attributes */
            (void)nco_att_cpy(grp_id,grp_out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
          } /* End compare item on list with current group name */

        } /* End check if group is on extraction list */
      } /* End nbr_att */

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);
      if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

    } /* end nco_obj_typ_grp */
  } /* end uidx  */

  /* Memory management for GPE names */
  for(int idx=0;idx<nbr_gpe_nm;idx++)
    gpe_nm[idx].var_nm_fll=(char *)nco_free(gpe_nm[idx].var_nm_fll);

} /* end nco_grp_var_mk_trv() */

nm_id_sct *                /* O [sct] Extraction list */
nco_xtr_lst_add            /* [fnc] Auxiliary function; add an entry to xtr_lst */
(char * const var_nm,      /* I [sng] Variable name */
 char * const var_nm_fll,  /* I [sng] Full variable name */
 char * const grp_nm_fll,  /* I [sng] Full group name */
 int const var_id,         /* I [ID] Variable ID */
 nm_id_sct *xtr_lst,       /* I/O [sct] Current list */
 int * xtr_nbr)            /* I/O [nbr] Current index in exclusion/extraction list */
{

  /* Check if variable is on extraction list */
  for(int idx_lst=0;idx_lst<*xtr_nbr;idx_lst++)
    if(!strcmp(xtr_lst[idx_lst].var_nm_fll,var_nm_fll)) return xtr_lst;

  /* Coordinate is not already on the list, put it there */
  if(*xtr_nbr == 0) xtr_lst=(nm_id_sct *)nco_malloc((*xtr_nbr+1)*sizeof(nm_id_sct)); else xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));

  /* ncks needs only:
  1) xtr_lst.grp_nm_fll (full group name where variable resides, to get group ID) 
  2) xtr_lst.var_nm_fll
  3) xtr_lst.id
  4) xtr_lst.nm (relative variable name) 
  */
  xtr_lst[*xtr_nbr].nm=(char*)strdup(var_nm);
  xtr_lst[*xtr_nbr].grp_nm_fll=(char*)strdup(grp_nm_fll);
  xtr_lst[*xtr_nbr].var_nm_fll=(char*)strdup(var_nm_fll);
  xtr_lst[*xtr_nbr].id=var_id;
  (*xtr_nbr)++;

  return xtr_lst;
} /* end nco_xtr_lst_add() */

int                            /* [rcd] Return code */
nco_grp_itr
(const int grp_id,             /* I [ID] Group ID */
 char * const grp_nm_fll,      /* I [sng] Absolute group name (path) */
 trv_tbl_sct *trv_tbl)         /* I/O [sct] Traversal table */
{
  /* Purpose: Recursively iterate grp_id */

  char grp_nm[NC_MAX_NAME+1];  /* O [sng] Group name */
  char var_nm[NC_MAX_NAME+1];  /* O [sng] Variable name */ 
  trv_sct obj;             /* O [obj] netCDF4 object, as having a path and a type */
  int *dmn_ids;                /* O [ID]  Dimension IDs */ 
  int *grp_ids;                /* O [ID]  Sub-group IDs */ 
  int idx;                     /* I [idx] Index */             
  int nbr_att;                 /* O [nbr] Number of attributes */
  int nbr_dmn;                 /* O [nbr] Number of dimensions */
  int nbr_dmn_prn;             /* O [nbr] Number of dimensions including parent groups */
  int nbr_grp;                 /* O [nbr] Number of sub-groups in this group */
  int nbr_var;                 /* O [nbr] Number of variables */
  int rcd=NC_NOERR;            /* O [rcd] Return code */
  int rec_dmn_id;              /* O [ID] Record dimension ID */
  int var_id;                  /* O [ID] Variable ID */ 
  const int flg_prn=0;         /* I [flg] All the dimensions in all parent groups will also be retrieved */        
  nc_type var_typ;             /* O [enm] Variable type */

  /* Get all information for this group */
  rcd+=nco_inq_nvars(grp_id,&nbr_var);
  rcd+=nco_inq_grpname(grp_id,grp_nm);
  rcd+=nco_inq_ndims(grp_id,&nbr_dmn);
  rcd+=nco_inq_natts(grp_id,&nbr_att);
  rcd+=nco_inq_grps(grp_id,&nbr_grp,NULL);
  rcd+=nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,&rec_dmn_id);
  dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
  rcd+=nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,flg_prn);
  rcd+=nco_inq_dimids(grp_id,&nbr_dmn_prn,NULL,1);

  /* Add group to table */
  obj.typ=nco_obj_typ_grp;
  obj.nm_fll=grp_nm_fll;
  obj.nm_fll_lng=strlen(obj.nm_fll);
  obj.grp_nm_fll=grp_nm_fll;
  obj.nm_lng=strlen(grp_nm);
  strcpy(obj.nm,grp_nm);
  obj.flg_mch=False;
  obj.flg_rcr=False;
  obj.nbr_att=nbr_att;
  obj.nbr_var=nbr_var;
  obj.nbr_dmn=nbr_dmn;
  obj.nbr_grp=nbr_grp;
  obj.xcl_flg=nco_obj_typ_err;
  obj.flg=nco_obj_typ_err;
  trv_tbl_add(obj,trv_tbl);

  /* Iterate variables for this group */
  for(var_id=0;var_id<nbr_var;var_id++){
    char *var_nm_fll=NULL; /* Full path of variable */
    rcd+=nco_inq_var(grp_id,var_id,var_nm,&var_typ,&nbr_dmn,NULL,&nbr_att);

    /* Allocate path buffer; add space for a trailing NUL */ 
    var_nm_fll=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(var_nm)+2L);

    /* Initialize path with current absolute group path */
    strcpy(var_nm_fll,grp_nm_fll);

    /* If not root group, concatenate separator */
    if(strcmp(grp_nm_fll,"/")) strcat(var_nm_fll,"/");

    /* Concatenate variable to absolute group path */
    strcat(var_nm_fll,var_nm);

    /* Add variable to table NB: nbr_var, nbr_grp, flg_rcr not valid here */
    obj.typ=nco_obj_typ_var;
    obj.flg_mch=False;
    obj.flg_rcr=False;
    obj.grp_nm_fll=grp_nm_fll;
    obj.nbr_att=nbr_att;
    obj.nbr_dmn=nbr_dmn;
    obj.nbr_grp=-1;
    obj.nbr_var=-1;
    obj.nm_fll=var_nm_fll;
    obj.nm_fll_lng=strlen(var_nm_fll);
    obj.nm_lng=strlen(var_nm);
    obj.xcl_flg=nco_obj_typ_err;
    obj.flg=nco_obj_typ_err;
    strcpy(obj.nm,var_nm);
    trv_tbl_add(obj,trv_tbl);
    var_nm_fll=(char*)nco_free(var_nm_fll);
  } /* end loop over variables */

  /* Go to sub-groups */ 
  grp_ids=(int*)nco_malloc((nbr_grp)*sizeof(int));
  rcd+=nco_inq_grps(grp_id,&nbr_grp,grp_ids);

  for(idx=0;idx<nbr_grp;idx++){
    char *pth=NULL;  /* Full group path */
    int gid=grp_ids[idx];
    rcd+=nco_inq_grpname(gid,grp_nm);

    /* Allocate path buffer; add space for a trailing NUL */ 
    pth=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(grp_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(pth,grp_nm_fll);

    /* If not root group, concatenate separator */
    if(strcmp(grp_nm_fll,"/")) strcat(pth,"/");

    (void)strcat(pth,grp_nm); /* Concatenate current group to absolute group path */

    /* Recursively go to sub-groups; NB: pass new absolute group name */
    rcd+=nco_grp_itr(gid,pth,trv_tbl);

    pth=(char*)nco_free(pth);
  } /* end loop over groups */

  (void)nco_free(grp_ids);
  (void)nco_free(dmn_ids);

  return rcd;
}/* end nco_grp_itr() */

int                       /* [rcd] Return code */
nco4_inq_vars             /* [fnc] Find and return total of variables */
(const int nc_id,         /* I [ID] Apex group */
 int * const var_nbr_all) /* O [nbr] Number of variables in file */
{
  /* [fnc] Find and return total variables */

  int rcd=NC_NOERR;
  int *grp_ids; /* [ID] Group IDs of children */
  int grp_id;   /* [ID] Group ID */
  int grp_nbr;  /* [nbr] Number of groups */
  int var_nbr;  /* [nbr] Number of variables */
  int idx_grp;

  /* Discover and return number of apex and all sub-groups */
  rcd+=nco_inq_grps_full(nc_id,&grp_nbr,(int *)NULL);

  grp_ids=(int *)nco_malloc(grp_nbr*sizeof(int)); /* [ID] Group IDs of children */

  /* Discover and return IDs of apex and all sub-groups */
  rcd+=nco_inq_grps_full(nc_id,&grp_nbr,grp_ids);

  /* Initialize variables that accumulate */
  *var_nbr_all=0; /* [nbr] Total number of variables in file */

  /* Create list of all variables in input file */
  for(idx_grp=0;idx_grp<grp_nbr;idx_grp++){
    grp_id=grp_ids[idx_grp]; /* [ID] Group ID */

    /* How many variables in current group? */
    rcd+=nco_inq_varids(grp_id,&var_nbr,(int *)NULL);

    /* Augment total number of variables in file */
    *var_nbr_all+=var_nbr;
  } /* end loop over grp */
 
  return rcd;
} /* end nco4_inq() */

void                          
nco_prt_grp_trv         /* [fnc] Print table  */
(const int nc_id,       /* I [ID] File ID */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  int fl_fmt; /* [enm] netCDF file format */
  (void)nco_inq_format(nc_id,&fl_fmt);

  (void)fprintf(stderr,"%s: INFO reports group information\n",prg_nm_get());
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->lst[uidx].typ == nco_obj_typ_grp ) {
      trv_sct trv=trv_tbl->lst[uidx];            
      (void)fprintf(stdout,"%s: %d subgroups, %d dimensions, %d attributes, %d variables\n",trv.nm_fll,trv.nbr_grp,trv.nbr_dmn,trv.nbr_att,trv.nbr_var); 
      int grp_id;                    /* [ID]  Group ID */
      int nbr_att;                   /* [nbr] Number of attributes */
      int nbr_var;                   /* [nbr] Number of variables */
      int nbr_dmn;                   /* [nbr] Number of dimensions */
      int nbr_dmn_ult;               /* [nbr] Number of unlimited dimensions */
      int dmn_ids[NC_MAX_DIMS];      /* [nbr] Dimensions IDs array */
      int dmn_ids_ult[NC_MAX_DIMS];  /* [nbr] Unlimited dimensions IDs array */
      char dmn_nm[NC_MAX_NAME];      /* [sng] Dimension name */ 
      long dmn_sz;                   /* [nbr] Dimension size */
      const int flg_prn=0;           /* [flg] All the dimensions in all parent groups will also be retrieved */        

      /* For classic files, the above is printed, and then return */
      if(fl_fmt == NC_FORMAT_CLASSIC || fl_fmt == NC_FORMAT_64BIT) return;

      /* Obtain group ID from netCDF API using full group name */
      (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);

      /* Obtain unlimited dimensions for group */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      /* Obtain number of dimensions for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);

      /* Obtain dimensions IDs for group */
      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,flg_prn);

#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif

      /* List dimensions using obtained group ID */
      for(int idx=0;idx<nbr_dmn;idx++){
        nco_bool is_rec_dim=False;
        (void)nco_inq_dim(grp_id,dmn_ids[idx],dmn_nm,&dmn_sz);

        /* Check if dimension is unlimited (record dimension) */
        for(int kdx=0;kdx<nbr_dmn_ult;kdx++){ 
          if (dmn_ids[idx] == dmn_ids_ult[kdx]){ 
            is_rec_dim=True;
            (void)fprintf(stdout," record dimension: %s (%ld)\n",dmn_nm,dmn_sz);
          } /* end if */
        } /* end kdx dimensions */

        /* An unlimited ID was not matched, so dimension is a plain vanilla dimension */
        if(is_rec_dim == False){
          (void)fprintf(stdout," dimension: %s (%ld)\n",dmn_nm,dmn_sz);
        } /* is_rec_dim */

      } /* end idx dimensions */
    } /* end nco_obj_typ_grp */
  } /* end uidx  */
} /* end nco_prt_grp_trv() */

void
nco_lmt_evl_trv            /* [fnc] Parse user-specified limits into hyperslab specifications */
(int nc_id,                /* I [idx] netCDF file ID */
 int lmt_nbr,              /* [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt,            /* I/O [sct] Structure from nco_lmt_prs() or from nco_lmt_sct_mk() to hold dimension limit information */
 nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  long dmn_sz;                 /* [nbr] Dimension size */ 
  int *dmn_ids;                /* [ID]  Dimension IDs */ 
  int grp_id;                  /* [ID]  Group ID */
  int nbr_att;                 /* [nbr] Number of attributes */
  int nbr_var;                 /* [nbr] Number of variables */
  int nbr_dmn;                 /* [nbr] Number of dimensions */
  const int flg_prn=0;         /* [flg] All the dimensions in all parent groups will also be retrieved */  
  int fl_fmt;                  /* [enm] netCDF file format */
  
  (void)nco_inq_format(nc_id,&fl_fmt);

  if(lmt_nbr){
    for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
      if (trv_tbl->lst[uidx].typ == nco_obj_typ_grp ) {
        trv_sct trv=trv_tbl->lst[uidx];           

        /* Obtain group ID from netCDF API using full group name */
        if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
          (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);
        }else{ /* netCDF3 case */
          grp_id=nc_id;
        }

        /* Obtain number of dimensions in group */
        (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
#ifdef NCO_SANITY_CHECK
        assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif
        dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
        (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,flg_prn);

        /* List dimensions using obtained group ID */
        for(int idx=0;idx<trv.nbr_dmn;idx++){
          (void)nco_inq_dim(grp_id,dmn_ids[idx],dmn_nm,&dmn_sz);

          /* NOTE: using obtained group ID */
          for(int kdx=0;kdx<lmt_nbr;kdx++) {
            if(strcmp(lmt[kdx]->nm,dmn_nm) == 0 ){
              (void)nco_lmt_evl(grp_id,lmt[kdx],0L,FORTRAN_IDX_CNV);
             
#ifdef NCO_SANITY_CHECK
              assert(lmt[kdx]->id == dmn_ids[idx]);
#endif
            } /* end if */
          } /* end kdx */
        } /* end idx dimensions */

        (void)nco_free(dmn_ids);
      } /* end nco_obj_typ_grp */
    } /* end uidx  */
  } /* end lmt_nbr */
} /* end nco_lmt_evl_trv() */

void 
nco_prn_att_trv               /* [fnc] Print all attributes of single variable */
(const int nc_id,             /* I [id] netCDF file ID */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  int grp_id;                 /* [ID]  Group ID */
  int nbr_att;                /* [nbr] Number of attributes */
  int nbr_var;                /* [nbr] Number of variables */
  int nbr_dmn;                /* [nbr] Number of dimensions */
  int fl_fmt;                 /* [enm] netCDF file format */

  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_grp ){

      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=nc_id;
      }

      /* Obtain info for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif

      /* List attributes using obtained group ID */
      if(nbr_att){
        (void)fprintf(stdout,"%s attributes for: %s\n",(strlen(trv.nm_fll) == 1L) ? "Global" : "Group",trv.nm_fll); 
        (void)nco_prn_att(nc_id,grp_id,NC_GLOBAL); 
      } /*nbr_att */
    } /* end nco_obj_typ_grp */
  } /* end uidx */
} /* end nco_prn_att_trv() */

nm_id_sct *                      /* O [sct] Extraction list */
nco_var_lst_crd_add_trv          /* [fnc] Add all coordinates to extraction list */
(const int nc_id,                /* I [ID] netCDF file ID */
 nm_id_sct *xtr_lst,             /* I/O [sct] Current extraction list  */
 int * xtr_nbr,                  /* I/O [nbr] Number of variables in current extraction list */
 int * const grp_xtr_nbr,        /* I [nbr] Number of groups in current extraction list (specified with -g ) */
 char * const * const grp_lst_in,/* I [sng] User-specified list of groups names to extract (specified with -g ) */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Purpose: Add all coordinates to extraction list
  Find all coordinates (dimensions which are also variables) and
  add them to the list if they are not already there. */

  int grp_id;                    /* [ID]  Group ID in input file */
  int nbr_att;                   /* [nbr] Number of attributes for group */
  int nbr_var;                   /* [nbr] Number of variables for group */
  int nbr_dmn;                   /* [nbr] Number of dimensions for group */
  int nbr_grp;                   /* [nbr] Number of groups for group */
  int fl_fmt;                    /* [enm] netCDF file format */
  char *var_nm_fll;              /* [sng] Full path of variable */
  int *var_ids;                  /* [ID]  Variable IDs array */
  char var_nm[NC_MAX_NAME+1];    /* [sng] Variable name */ 
  int nbr_dmn_ult;               /* [nbr] Number of unlimited dimensions */
  int dmn_ids_ult[NC_MAX_DIMS];  /* [ID]  Unlimited dimensions IDs array */
  int dmn_ids[NC_MAX_DIMS];      /* [nbr] Dimensions IDs array */
  char dmn_nm[NC_MAX_NAME];      /* [sng] Dimension name */ 
  long dmn_sz;                   /* [nbr] Dimension size */ 
  const int flg_prn=0;           /* [flg] All the dimensions in all parent groups will also be retrieved */        

  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_grp ){

      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=nc_id;
      }

      /* Obtain info for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
      (void)nco_inq_grps(grp_id,&nbr_grp,(int *)NULL);

      /* Get dimension IDs for group */
      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,flg_prn);
#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att && nbr_grp == trv.nbr_grp);
#endif

      /* Allocate space for and obtain variable IDs in current group */
      var_ids=(int *)nco_malloc(nbr_var*sizeof(int));
      (void)nco_inq_varids(grp_id,&nbr_var,var_ids);

      /* Get number of unlimited dimensions */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      /* Get variables for this group */
      for(int idx_var=0;idx_var<nbr_var;idx_var++){
        var_nm_fll=NULL;

        /* Get name of current variable in current group NOTE: using obtained IDs array */
        (void)nco_inq_varname(grp_id,var_ids[idx_var],var_nm);

        /* Allocate path buffer; add space for a trailing NUL */ 
        var_nm_fll=(char*)nco_malloc(strlen(trv.nm_fll)+strlen(var_nm)+2);

        /* Initialize path with the current absolute group path */
        strcpy(var_nm_fll,trv.nm_fll);
        if(strcmp(trv.nm_fll,"/")!=0) /* If not root group, concatenate separator */
          strcat(var_nm_fll,"/");
        strcat(var_nm_fll,var_nm); /* Concatenate variable to absolute group path */

        /* List dimensions */
        for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++){

          /* Get dimension info */
          (void)nco_inq_dim(grp_id,dmn_ids[idx_dmn],dmn_nm,&dmn_sz);

          /* Compare variable name with dimension name */
          if(strcmp(dmn_nm,var_nm) == 0){
            /* No groups case, just add  */
            if (*grp_xtr_nbr == 0 ){
              xtr_lst=nco_xtr_lst_add(var_nm,var_nm_fll,trv.nm_fll,var_ids[idx_var],xtr_lst,xtr_nbr);
            }
            /* Groups -g case, add only if current group name GRP_NM matches any of the supplied GRP_LST_IN names */
            else{  
              /* Loop through user-specified group list */
              for(int idx_grp=0;idx_grp<*grp_xtr_nbr;idx_grp++){
                /* Locate group name from -g in traversal name */
                char* pch=strstr(trv.nm,grp_lst_in[idx_grp]);
                /* strstr returns the first occurrence of 'grp_lst_in' in 'trv.nm', the higher level group( closer to root) */
                if(pch != NULL){
                  xtr_lst=nco_xtr_lst_add(var_nm,var_nm_fll,trv.nm_fll,var_ids[idx_var],xtr_lst,xtr_nbr);
                }
              } /* end idx_grp */       
            } /* end groups case */
          } /* end strcmp coordinate name */
        } /* end jdx dimensions */

        /* Memory management after current variable */
        var_nm_fll=(char *)nco_free(var_nm_fll);

      } /* end get variables for this group */  

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);

    } /* end nco_obj_typ_grp */
  } /* end uidx  */

  return xtr_lst;
} /* end nco_var_lst_crd_add_trv() */

void                          
nco_chk_var                         /* [fnc] Check if input names of -v or -g are in file */
(const int nc_id,                   /* I [ID] Apex group ID */
 char * const * const var_lst_in,   /* I [sng] User-specified list of variable names and rx's */
 const int var_xtr_nbr,             /* I [nbr] Number of variables in current extraction list */
 const nco_bool EXCLUDE_INPUT_LIST) /* I [flg] Exclude rather than extract */
{
  char *var_sng; /* User-specified variable name or regular expression */
  char *grp_nm_fll; /* [sng] Fully qualified group name */
  char *var_nm_fll; /* [sng] Fully qualified variable name */
  char *grp_nm_fll_sls; /* [sng] Fully qualified group name plus terminating '/' */
  char *var_nm_fll_sls_ptr; /* Pointer to first character following last slash */
  char grp_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  int *grp_ids; /* [ID] Group IDs of children */
  int *var_ids;
  int grp_id; /* [ID] Group ID */
  int idx_grp;
  int grp_nbr; /* [nbr] Number of groups in input file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int xtr_idx;
  int idx_var;
  int idx_var_crr; /* [idx] Total variable index */
  int var_nbr; /* [nbr] Number of variables in current group */
  int var_nbr_all; /* [nbr] Number of variables in input file */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */

  idx_var_crr=0; /* Incremented at cycle end */

  nco_bool FLG_ROOT_GRP=True; /* [flg] Current group is root group */
  nco_bool *var_xtr_rqs=NULL; /* [flg] Variable specified in extraction list */

  nm_id_sct *var_lst_all=NULL; /* [sct] All variables in input file */

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
  for(idx_grp=0;idx_grp<grp_nbr;idx_grp++){
    grp_id=grp_ids[idx_grp]; /* [ID] Group ID */

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

      /* Append all variables in current group to variable list */
      for(idx_var=0;idx_var<var_nbr;idx_var++){

        var_lst_all=(nm_id_sct *)nco_realloc(var_lst_all,var_nbr_all*sizeof(nm_id_sct));

        /* Get name current variable in current group */
        (void)nco_inq_varname(grp_id,idx_var,var_nm);

        /* Tack variable name onto slash following group name */
        var_nm_fll_sls_ptr=(char *)strcat(var_nm_fll_sls_ptr,var_nm);

        /* Create full name of each variable */
        var_lst_all[idx_var_crr].var_nm_fll=(char *)strdup(var_nm_fll);
        var_lst_all[idx_var_crr].nm=(char *)strdup(var_nm);
        var_lst_all[idx_var_crr].id=var_ids[idx_var];
        var_lst_all[idx_var_crr].grp_nm_fll=(char *)strdup(grp_nm_fll);

        /* Increment number of variables */
        idx_var_crr++;

        /* Full variable name has been duplicated, re-terminate with NUL for next variable */
        *var_nm_fll_sls_ptr='\0'; /* [ptr] Pointer to first character following last slash */
      } /* end loop over idx_var */

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);
      grp_nm_fll=(char *)nco_free(grp_nm_fll);
      var_nm_fll=(char *)nco_free(var_nm_fll);

    } /* endif current group has variables */

  } /* end loop over grp */

  /* Initialize and allocate extraction flag array to all False */
  var_xtr_rqs=(nco_bool *)nco_calloc((size_t)var_nbr_all,sizeof(nco_bool));

  /* Loop through user-specified variable list */
  for(xtr_idx=0;xtr_idx<var_xtr_nbr;xtr_idx++){
    var_sng=var_lst_in[xtr_idx];
    /* Convert pound signs (back) to commas */
    nco_hash2comma(var_sng);

    /* If var_sng is regular expression ... */
    if(strpbrk(var_sng,".*^$\\[]()<>+?|{}")){
      /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
      rx_mch_nbr=nco_lst_rx_search(var_nbr_all,var_lst_all,var_sng,var_xtr_rqs);
      if(!rx_mch_nbr) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any variables\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),var_sng); 
      continue;
#else
      (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),var_sng);
      nco_exit(EXIT_FAILURE);
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
    } /* end if regular expression */

    /* Normal variable so search through variable array */
    for(idx_var=0;idx_var<var_nbr_all;idx_var++)
      if(!strcmp(var_sng,var_lst_all[idx_var].nm)) break;

    /* Mark any match as requested for inclusion by user */
    if(idx_var != var_nbr_all){
      var_xtr_rqs[idx_var]=True;
    }else{
      if(EXCLUDE_INPUT_LIST){ 
        /* Variable need not be present if list will be excluded later ... */
        if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_chk_var() reports explicitly excluded variable \"%s\" is not in input file anyway\n",prg_nm_get(),var_sng); 
      }else{ /* !EXCLUDE_INPUT_LIST */
        /* Variable should be included but no matches found so die */
        (void)fprintf(stdout,"%s: ERROR nco_chk_var() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),var_sng); 
        nco_exit(EXIT_FAILURE);
      } /* !EXCLUDE_INPUT_LIST */
    } /* end else */

  } /* end loop over var_lst_in xtr_idx */ 

  var_lst_all=(nm_id_sct *)nco_nm_id_lst_free(var_lst_all,var_nbr_all);
  var_xtr_rqs=(nco_bool *)nco_free(var_xtr_rqs);

} /* end nco_chk_var_var() */

nm_id_sct *                       /* O [sct] Extraction list */
nco_var_lst_crd_ass_add_trv       /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                 /* I netCDF file ID */
 nm_id_sct *xtr_lst,              /* I/O current extraction list (changed) */
 int * const xtr_nbr,             /* I/O number of variables in current extraction list */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  int rcd=NC_NOERR;            /* [rcd] Return code */
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  char var_nm[NC_MAX_NAME];    /* [sng] Variable name */ 
  long dmn_sz;                 /* [nbr] Dimension size */  
  int grp_id;                  /* [ID] Group ID */
  int nbr_att;                 /* [nbr] Number of attributes */
  int nbr_var;                 /* [nbr] Number of variables */
  int nbr_dmn;                 /* [nbr] number of dimensions */
  int dmn_id[NC_MAX_DIMS];     /* [ID] Dimensions IDs array for group */
  int dmn_id_var[NC_MAX_DIMS]; /* [ID] Dimensions IDs array for variable */
  int *var_ids;                /* [ID] Variable IDs array */
  int nbr_var_dim;             /* [nbr] Number of dimensions associated with current matched variable */
  const int flg_prn=0;         /* [flg] All the dimensions in all parent groups will also be retrieved */ 
  int fl_fmt;                  /* [enm] netCDF file format */
  
  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_grp ) {

      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=nc_id;
      }

      /* Obtain number of dimensions for group: NOTE using group ID */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);

      /* Obtain dimension IDs */
      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_id,flg_prn);

      /* Allocate space for and obtain variable IDs in current group */
      var_ids=(int *)nco_malloc(nbr_var*sizeof(int));
      rcd+=nco_inq_varids(grp_id,&nbr_var,var_ids);

#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif

      /* Construct the full variable name for all variables in group */
      for(int idx_var_grp=0;idx_var_grp<nbr_var;idx_var_grp++){
        char *var_nm_fll=NULL; /* Full path of variable */

        /* Get name of current variable in current group NOTE: using obtained IDs array */
        (void)nco_inq_varname(grp_id,var_ids[idx_var_grp],var_nm);

        /* Allocate path buffer; add space for a trailing NUL */ 
        var_nm_fll=(char*)nco_malloc(strlen(trv.nm_fll)+strlen(var_nm)+2);

        /* Initialize path with the current absolute group path */
        strcpy(var_nm_fll,trv.nm_fll);
        if(strcmp(trv.nm_fll,"/")!=0) /* If not root group, concatenate separator */
          strcat(var_nm_fll,"/");
        strcat(var_nm_fll,var_nm); /* Concatenate variable to absolute group path */

        /* Check if variable is on extraction list */
        for(int idx_lst_var=0;idx_lst_var<*xtr_nbr;idx_lst_var++){
          nm_id_sct xtr=xtr_lst[idx_lst_var];

          /* Compare item on list with current variable name (NOTE: using full name to compare ) */
          if(strcmp(xtr.var_nm_fll,var_nm_fll) == 0){

            /* Get number of dimensions for variable */
            (void)nco_inq_varndims(grp_id,var_ids[idx_var_grp],&nbr_var_dim);

            /* Get dimension IDs for variable */
            (void)nco_inq_vardimid(grp_id,var_ids[idx_var_grp],dmn_id_var);

            /* List dimensions for variable */
            for(int idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){

              /* Get dimension name */
              (void)nco_inq_dim(grp_id,dmn_id_var[idx_var_dim],dmn_nm,&dmn_sz);

              /* Add all possible coordinate variables for current variable */
              xtr_lst=nco_aux_add_dmn_trv(nc_id,dmn_nm,xtr_lst,xtr_nbr,trv_tbl);

            } /* End loop over idx_var_dim: list dimensions for variable */
          } /* End strcmp: match in compare item on list with current item */
        } /* End idx_lst_var: check if variable is on extraction list */

        /* Memory management after current variable */
        var_nm_fll=(char*)nco_free(var_nm_fll);

      } /* end idx_var_grp: full variable name for all variables in group */ 

      /* Memory management after current group for variables */
      var_ids=(int *)nco_free(var_ids);

    } /* end nco_obj_typ_grp */
  } /* end uidx  */

  return xtr_lst;
} /* end nco_var_lst_crd_ass_add_trv */

void 
xtr_lst_prn                            /* [fnc] Validated name-ID structure list */
(nm_id_sct * const nm_id_lst,          /* I [sct] Name-ID structure list */
 const int nm_id_nbr)                  /* I [nbr] Number of name-ID structures in list */
{
  (void)fprintf(stdout,"%s: INFO List: %d extraction variables\n",prg_nm_get(),nm_id_nbr); 
  for(int idx=0;idx<nm_id_nbr;idx++){
    nm_id_sct nm_id=nm_id_lst[idx];
    (void)fprintf(stdout,"[%d] %s\n",idx,nm_id.var_nm_fll); 
  } 
}/* end xtr_lst_prn() */


nco_bool                        /* O [flg] Name is in extraction list */
xtr_lst_fnd                     /* [fnc] Check if "var_nm_fll" is in extraction list */
(const char * const var_nm_fll, /* I [sng] Full variable name to find */
 nm_id_sct *xtr_lst,            /* I [sct] Name ID structure list */
 const int xtr_nbr)             /* I [nbr] Name ID structure list size */
{
  for(int idx=0;idx<xtr_nbr;idx++){
    nm_id_sct nm_id=xtr_lst[idx];
    if(!strcmp(var_nm_fll,nm_id.var_nm_fll)) return True;
  } /* end loop */
  return False;
} /* end xtr_lst_fnd */ 

int                             /* O [id] Group ID */
nco_aux_grp_id                  /* [fnc] Return the group ID from the variable full name */
(const int nc_id,               /* I [id] netCDF file ID */
 const char * const var_nm_fll) /* I [sng] Full variable name to find */
{
  char *pch;        /* Pointer to character in string */
  int   pos;        /* Position of character */
  char *grp_nm_fll; /* Group path */
  int  grp_id;      /* Group ID */
  int  len_fll;     /* Lenght of fully qualified group where variable resides */

  /* Get group full name */
  len_fll=strlen(var_nm_fll);
  grp_nm_fll=(char *)nco_malloc((len_fll+1L)*sizeof(char));
  strcpy(grp_nm_fll,var_nm_fll);
  /* Find last occurence of '/' */
  pch=strrchr(grp_nm_fll,'/');
  /* Trim variable name */
  pos=pch-grp_nm_fll;
  grp_nm_fll[pos]='\0';

  /* Obtain netCDF file format */
  int fl_fmt;
  (void)nco_inq_format(nc_id,&fl_fmt);
  /* Obtain group ID from netCDF API using full group name */
  if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    (void)nco_inq_grp_full_ncid(nc_id,grp_nm_fll,&grp_id);
  }else{ /* netCDF3 case */
    grp_id=nc_id;
  }

  /* Free allocated memory */
  grp_nm_fll=(char *)nco_free(grp_nm_fll);

  return grp_id;
} /* end nco_aux_grp_id() */


int                            /* O [nbr] Item found or not */
nco_fnd_var_trv                /* [fnc] Find a variable that matches parameter "var_nm" and export to "nm_id" */
(const int nc_id,              /* I [id] netCDF file ID */
 const char * const var_nm,    /* I [sng] Variable name to find */
 const trv_tbl_sct * const trv_tbl,   /* I [sct] Traversal table */
 nm_id_sct *nm_id)             /* O [sct] Entry to add to list */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if(trv.typ == nco_obj_typ_var){ 

      /* Compare name with current relative name */
      if(strcmp(trv.nm,var_nm) == 0){
        int var_id;
        int grp_id;      
        int fl_fmt;
        (void)nco_inq_format(nc_id,&fl_fmt);
        /* Obtain group ID from netCDF API using full group name */
        if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
          (void)nco_inq_grp_full_ncid(nc_id,trv.grp_nm_fll,&grp_id);
        }else{ /* netCDF3 case */
          grp_id=nc_id;
        }
        /* Obtain variable ID from netCDF API using group ID */
        (void)nco_inq_varid(grp_id,trv.nm,&var_id);
        /* Define new nm_id_sct */
        nm_id->grp_nm_fll=strdup(trv.grp_nm_fll);    
        nm_id->var_nm_fll=strdup(trv.nm_fll);
        nm_id->nm=strdup(var_nm);
        nm_id->id=var_id; 
        return 1;
      } /* end strcmp */
    } /* end nco_obj_typ_var */
  } /* end uidx  */

  return 0;
} /* end nco_fnd_var_trv */ 

nm_id_sct *                       /* O [sct] Extraction list */
nco_var_lst_crd_ass_add_cf_trv    /* [fnc] Add to extraction list all coordinates associated with CF convention */
(const int nc_id,                 /* I netCDF file ID */
 const char * const cf_nm,        /* I [sng] CF name to find ( "coordinates" or "bounds" ) */
 nm_id_sct *xtr_lst,              /* I/O current extraction list (modified) */
 int * const xtr_nbr,             /* I/O number of variables in current extraction list */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Detect associated coordinates specified by CF "coordinates" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.5/cf-conventions.html#coordinate-system */

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_var){

      /* Check if current variable is in extraction list */
      if(xtr_lst_fnd(trv.nm_fll,xtr_lst,*xtr_nbr)){

        /* Try to add to extraction list */
        xtr_lst=nco_aux_add_cf(nc_id,trv.nm_fll,trv.nm,cf_nm,xtr_lst,xtr_nbr,trv_tbl);

      } /* end check if current variable is in extraction list */
    } /* end nco_obj_typ_var */
  } /* end uidx  */

  return xtr_lst;
} /* nco_var_lst_crd_ass_add_cf_trv() */



nm_id_sct *                      /* O [sct] Extraction list */
nco_aux_add_cf                   /* [fnc] Add to extraction list all coordinates associated with CF convention (associated with "var_nm_fll")*/
(const int nc_id,                /* I netCDF file ID */
 const char * const var_nm_fll,  /* I [sng] Full variable name */
 const char * const var_nm,      /* I [sng] Variable relative name */
 const char * const cf_nm,       /* I [sng] CF name to find ( "coordinates" or "bounds" */
 nm_id_sct *xtr_lst,             /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,            /* I/O number of variables in current extraction list */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Detect associated coordinates specified by CF "coordinates" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.1/cf-conventions.html#coordinate-system 
  NB: Only difference between this algorithm and CF algorithm in 
  nco_var_lst_crd_ass_add() is that this algorithm loops over 
  all variables in file, not just over current extraction list. */ 

  int rcd=NC_NOERR;         /* [rcd] Return code */
  char att_nm[NC_MAX_NAME]; /* [sng] Attribute name */
  int nbr_att;              /* [nbr] Number of attributes */
  int var_id;               /* [id]  Variable ID */
  int grp_id;               /* [id]  Group ID */
  char **bnd_lst;           /* [sng] 1D array of list elements */
  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  int nbr_bnd;              /* [nbr] Number of coordinates specified in "bounds" (or "coordinates") attribute */

  /* Obtain group ID using nco_aux_grp_id (get ID from full variable name ) */
  grp_id=nco_aux_grp_id(nc_id,var_nm_fll);

  /* Obtain variable ID from netCDF API using group ID. NOTE: using relative var name  */
  (void)nco_inq_varid(grp_id,var_nm,&var_id);

  /* Find number of attributes */
  (void)nco_inq_varnatts(grp_id,var_id,&nbr_att);
  for(int idx_att=0;idx_att<nbr_att;idx_att++){
    (void)nco_inq_attname(grp_id,var_id,idx_att,att_nm);

    /* Is attribute part of CF convention? */
    if(strcmp(att_nm,cf_nm) == 0){
      char *att_val;
      long att_sz;
      nc_type att_typ;
      int bnd_id;

      /* Yes, get list of specified attributes */
      (void)nco_inq_att(grp_id,var_id,att_nm,&att_typ,&att_sz);
      if(att_typ != NC_CHAR){
        (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore will skip this attribute.\n",prg_nm_get(),att_nm,var_nm_fll,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR));
        return xtr_lst;
      } /* end if */
      att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
      if(att_sz > 0) (void)nco_get_att(grp_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
      /* NUL-terminate attribute */
      att_val[att_sz]='\0';

      /* Split list into separate coordinate names
      Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
      bnd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_bnd);
      /* ...for each coordinate in "bounds" attribute... */
      for(int idx_bnd=0;idx_bnd<nbr_bnd;idx_bnd++){
        char* bnd_lst_var=bnd_lst[idx_bnd];
        if(bnd_lst_var==NULL)
          continue;
        /* Verify "bounds" exists in input file. NOTE: using group ID */
        rcd=nco_inq_varid_flg(grp_id,bnd_lst_var,&bnd_id);
        /* NB: Coordinates of rank N have bounds of rank N+1 */
        if(rcd == NC_NOERR){

          /* Try to find the variable */
          nm_id_sct nm_id;
          if (nco_fnd_var_trv(nc_id,bnd_lst_var,trv_tbl,&nm_id) == 1 )
          {
            char *pch;        /* Pointer to the last occurrence of character */
            int   pos;        /* Position of character */
            char *grp_nm_fll; /* Fully qualified group where variable resides */
            int  len;         /* Lenght of fully qualified group where variable resides */

            len=strlen(var_nm_fll);
            grp_nm_fll=(char*)nco_malloc((len+1L)*sizeof(char));
            strcpy(grp_nm_fll,var_nm_fll);
            pch=strrchr(grp_nm_fll,'/');
            pos=pch-grp_nm_fll;
            grp_nm_fll[pos]='\0';

            /* Construct the full variable name */
            char* cf_nm_fll=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(bnd_lst_var)+2);
            strcpy(cf_nm_fll,grp_nm_fll);
            if(strcmp(grp_nm_fll,"/")!=0) strcat(cf_nm_fll,"/");
            strcat(cf_nm_fll,bnd_lst_var); 

            /* Free allocated memory */
            grp_nm_fll=(char *)nco_free(grp_nm_fll);

            /* Check if the  variable is already in the  extraction list: NOTE using full name "cf_nm_fll" */
            if(!xtr_lst_fnd(cf_nm_fll,xtr_lst,*xtr_nbr)){

              /* Add variable to list
              NOTE: Needed members for traversal code:
              1) "grp_nm_fll": needed to "nco_inq_grp_full_ncid": obtain group ID from group path and netCDF file ID
              2) "nm": needed to "nco_prn_var_dfn" to print variable's definition 
              3) "grp_id": needed to "nco_prn_var_dfn" to print variable's definition 
              4) "id": needed for "nco_prn_att"  to print variable's attributes
              5) "var_nm_fll": using full name to compare criteria */

              /* Out with it */
              xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));
              xtr_lst[*xtr_nbr].grp_nm_fll=nm_id.grp_nm_fll;    
              xtr_lst[*xtr_nbr].var_nm_fll=nm_id.var_nm_fll;
              xtr_lst[*xtr_nbr].nm=nm_id.nm;
              xtr_lst[*xtr_nbr].id=nm_id.id;                
              (*xtr_nbr)++; 
            }

          } /* end nco_fnd_var_trv() */   

          /* Continue to next coordinate in loop */
          continue;


        }else{ /* end Verify "bounds" exists in input file */
          if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO Variable %s, specified in the \"bounds\" attribute of variable %s, is not present in the input file\n",prg_nm_get(),bnd_lst[idx_bnd],var_nm_fll);
        } /* end else named coordinate exists in input file */
      } /* end loop over idx_bnd */

      /* Free allocated memory */
      att_val=(char *)nco_free(att_val);
      bnd_lst=nco_sng_lst_free(bnd_lst,nbr_bnd);

    } /* end strcmp Is attribute part of CF convention? */
  } /* end find number of attributes */

  return xtr_lst;
} /* nco_aux_add_cf() */

nm_id_sct *                      /* O [sct] Extraction list */
nco_var_lst_crd_add_cf_trv       /* [fnc] Add to extraction list all coordinates associated with CF convention */
(const int nc_id,                /* I netCDF file ID */
 const char * const cf_nm,       /* I [sng] CF name to find ( "coordinates" or "bounds" */
 nm_id_sct *xtr_lst,             /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,            /* I/O number of variables in current extraction list */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Detect associated coordinates specified by CF "coordinates" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.1/cf-conventions.html#coordinate-system 
  NB: Only difference between this algorithm and CF algorithm in 
  nco_var_lst_crd_ass_add() is that this algorithm loops over 
  all variables in file, not just over current extraction list. */ 

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_var){

      /* Try to add to extraction list */
      xtr_lst=nco_aux_add_cf(nc_id,trv.nm_fll,trv.nm,cf_nm,xtr_lst,xtr_nbr,trv_tbl);

    } /* end nco_obj_typ_var */
  } /* end uidx  */

  return xtr_lst;
} /* nco_var_lst_crd_add_cf_trv() */

nco_bool /* O [flg] All names are in file */
nco_chk_trv /* [fnc] Check if input names of -v or -g are in file */
(char * const * const obj_lst_in, /* I [sng] User-specified list of object names */
 const int obj_nbr, /* I [nbr] Number of items in list */
 const nco_obj_typ obj_typ, /* I [enm] Object type (group or variable) */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  /* Purpose: Verify all user-specified objects exist in file
  Verify variables or groups independently of the other
  Handles regular expressions 
  Sets flags in traversal table to help generate extraction list

  Tests:
  ncks -O -D 5 -g / ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -g '' ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -g g1 ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -g /g1 ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -g /g1/g1 ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -g g1.+ ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -v v1 ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -g g1.+ -v v1,sc. ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -v scl,/g1/g1g1/v1 ~/nco/data/in_grp.nc ~/foo.nc
  ncks -O -D 5 -g g3g.+,g9/ -v scl,/g1/g1g1/v1 ~/nco/data/in_grp.nc ~/foo.nc */

  char *sbs_srt; /* [sng] Location of user-string match start in object path */
  char *sbs_end; /* [sng] Location of user-string match end   in object path */
  char *usr_sng; /* [sng] User-supplied object name */
  char *var_mch_srt; /* [sng] Location of variable short name in user-string */

  const char sls_chr='/'; /* [chr] Slash character */
  const char fnc_nm[]="nco_chk_trv()"; /* [sng] Function name */

  trv_sct trv_obj; /* [sct] Traversal table object */

#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */

  nco_bool flg_ncr_mch_crr; /* [flg] Current group meets anchoring properties of this user-supplied string */
  nco_bool flg_ncr_mch_grp; /* [flg] User-supplied string anchors at root */
  nco_bool flg_pth_end_bnd; /* [flg] String ends   at path component boundary */
  nco_bool flg_pth_srt_bnd; /* [flg] String begins at path component boundary */
  nco_bool flg_rcr_mch_crr; /* [flg] Current group meets recursion criteria of this user-supplied string */
  nco_bool flg_rcr_mch_grp; /* [flg] User-supplied string will match groups recursively */
  nco_bool flg_usr_mch_obj; /* [flg] One or more objects match each user-supplied string */
  nco_bool flg_var_cnd; /* [flg] Match meets addition condition(s) for variable */

  size_t usr_sng_lng; /* [nbr] Length of user-supplied string */

  for(int obj_idx=0;obj_idx<obj_nbr;obj_idx++){

    /* Initialize state for current user-specified string */
    flg_ncr_mch_grp=False;
    flg_rcr_mch_grp=True;
    flg_usr_mch_obj=False;

    if(!obj_lst_in[obj_idx]){
      (void)fprintf(stderr,"%s: ERROR %s reports user-supplied %s name is empty\n",prg_nm_get(),fnc_nm,(obj_typ == nco_obj_typ_grp) ? "group" : "variable");
      nco_exit(EXIT_FAILURE);
    } /* end else */

    usr_sng=strdup(obj_lst_in[obj_idx]); 
    usr_sng_lng=strlen(usr_sng);

    /* Turn-off recursion for groups? */
    if(obj_typ == nco_obj_typ_grp)
      if(usr_sng_lng > 1L && usr_sng[usr_sng_lng-1L] == sls_chr){
        /* Remove trailing slash for subsequent searches since canonical group names do not end with slash */
        flg_rcr_mch_grp=False;
        usr_sng[usr_sng_lng-1L]='\0';
        usr_sng_lng--;
      } /* flg_rcr_mch_grp */

      /* Turn-on root-anchoring for groups? */
      if(obj_typ == nco_obj_typ_grp)
        if(usr_sng[0L] == sls_chr)
          flg_ncr_mch_grp=True;

      /* Convert pound signs (back) to commas */
      nco_hash2comma(usr_sng);

      /* If usr_sng is regular expression ... */
      if(strpbrk(usr_sng,".*^$\\[]()<>+?|{}")){
        /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
        if((rx_mch_nbr=nco_trv_rx_search(usr_sng,obj_typ,trv_tbl))) flg_usr_mch_obj=True;
        if(!rx_mch_nbr) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any %s\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),usr_sng,(obj_typ == nco_obj_typ_grp) ? "group" : "variable"); 
        continue;
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
        (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),usr_sng);
        nco_exit(EXIT_FAILURE);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
      } /* end if regular expression */

      /* usr_sng is not rx, so manually search for multicomponent matches */
      for(unsigned int tbl_idx=0;tbl_idx<trv_tbl->nbr;tbl_idx++){

        /* Create shallow copy to avoid indirection */
        trv_obj=trv_tbl->lst[tbl_idx];

        if(trv_obj.typ == obj_typ){

          /* Initialize defaults for current candidate path to match */
          flg_pth_srt_bnd=False;
          flg_pth_end_bnd=False;
          flg_var_cnd=False;
          flg_rcr_mch_crr=True;
          flg_ncr_mch_crr=True;

          /* Look for partial match, not necessarily on path boundaries */
          if((sbs_srt=strstr(trv_obj.nm_fll,usr_sng))){

            /* Ensure match spans (begins and ends on) whole path-component boundaries */

            /* Does match begin at path component boundary ... directly on a slash? */
            if(*sbs_srt == sls_chr) flg_pth_srt_bnd=True;

            /* ...or one after a component boundary? */
            if((sbs_srt > trv_obj.nm_fll) && (*(sbs_srt-1L) == sls_chr)) flg_pth_srt_bnd=True;

            /* Does match end at path component boundary ... directly on a slash? */
            sbs_end=sbs_srt+usr_sng_lng-1L;

            if(*sbs_end == sls_chr) flg_pth_end_bnd=True;

            /* ...or one before a component boundary? */
            if(sbs_end <= trv_obj.nm_fll+trv_obj.nm_fll_lng-1L)
              if((*(sbs_end+1L) == sls_chr) || (*(sbs_end+1L) == '\0'))
                flg_pth_end_bnd=True;

            /* Additional condition for variables is user-supplied string must end with short form of variable name */
            if(obj_typ == nco_obj_typ_var){
              var_mch_srt=usr_sng+usr_sng_lng-trv_obj.nm_lng;
              if(!strcmp(var_mch_srt,trv_obj.nm)) flg_var_cnd=True; else flg_var_cnd=False;
              if(dbg_lvl_get() == nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s reports variable %s %s additional conditions for variable match with %s.\n",prg_nm_get(),fnc_nm,usr_sng,(flg_var_cnd) ? "meets" : "fails",trv_obj.nm_fll);
            } /* endif var */

            /* If anchoring, match must begin at root */
            if(flg_ncr_mch_grp && *sbs_srt != sls_chr) flg_ncr_mch_crr=False;

            /* If no recursion, match must terminate user-supplied string */
            if(!flg_rcr_mch_grp && *(sbs_end+1L)) flg_rcr_mch_crr=False;

            /* Set traversal table flags */
            if(obj_typ == nco_obj_typ_var){
              /* Variables must meet necessary flags for variables */
              if(flg_pth_srt_bnd && flg_pth_end_bnd && flg_var_cnd){
                trv_tbl->lst[tbl_idx].flg_mch=True;
                trv_tbl->lst[tbl_idx].flg_rcr=False;
              } /* end flags */
            }else{ /* !nco_obj_typ_var */
              /* Groups must meet necessary flags for groups */
              if(flg_pth_srt_bnd && flg_pth_end_bnd && flg_ncr_mch_crr && flg_rcr_mch_crr){
                trv_tbl->lst[tbl_idx].flg_mch=True;
                trv_tbl->lst[tbl_idx].flg_rcr=flg_rcr_mch_grp;
              } /* end flags */
            }  /* !nco_obj_typ_var */

            /* Set function return condition */
            if(trv_tbl->lst[tbl_idx].flg_mch) flg_usr_mch_obj=True;

            if(dbg_lvl_get() == nco_dbg_crr){
              (void)fprintf(stderr,"%s: INFO %s reports %s %s matches filepath %s. Begins on boundary? %s. Ends on boundary? %s. Extract? %s.",prg_nm_get(),fnc_nm,(obj_typ == nco_obj_typ_grp) ? "group" : "variable",usr_sng,trv_obj.nm_fll,(flg_pth_srt_bnd) ? "Yes" : "No",(flg_pth_end_bnd) ? "Yes" : "No",(trv_tbl->lst[tbl_idx].flg_mch) ?  "Yes" : "No");
              if(obj_typ == nco_obj_typ_grp) (void)fprintf(stderr," Anchored? %s.",(flg_ncr_mch_grp) ? "Yes" : "No");
              if(obj_typ == nco_obj_typ_grp) (void)fprintf(stderr," Recursive? %s.",(trv_tbl->lst[tbl_idx].flg_rcr) ? "Yes" : "No");
              (void)fprintf(stderr,"\n");
            } /* end if */

          } /* endif strstr() */
        } /* endif nco_obj_typ */
      } /* end loop over tbl_idx */

      if(!flg_usr_mch_obj){
        (void)fprintf(stderr,"%s: ERROR %s reports user-supplied %s name or regular expression %s is not in and/or does not match contents of input file\n",prg_nm_get(),fnc_nm,(obj_typ == nco_obj_typ_grp) ? "group" : "variable",usr_sng);
        nco_exit(EXIT_FAILURE);
      } /* flg_usr_mch_obj */
      /* Free dynamic memory */
      if(usr_sng) usr_sng=(char *)nco_free(usr_sng);

  } /* obj_idx */

  if(dbg_lvl_get() == nco_dbg_crr){
    (void)fprintf(stdout,"%s: INFO nco_chk_trv() reports following %s match sub-setting and regular expressions:\n",prg_nm_get(),(obj_typ == nco_obj_typ_grp) ? "groups" : "variables");
    trv_tbl_prn_flg_mch(trv_tbl,obj_typ);
  } /* endif dbg */

  return (nco_bool)True;

} /* end nco_chk_trv() */

void 
nco_msa_lmt_all_int_trv                /* [fnc] Initilaize lmt_all_sct's; recursive version */ 
(int in_id,                            /* [ID]  netCDF file ID */
 nco_bool MSA_USR_RDR,                 /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 lmt_all_sct **lmt_all_lst,            /* [sct] List of *lmt_all_sct structures */
 int nbr_dmn_fl,                       /* I [nbr] Number of dimensions in file */
 lmt_sct **lmt,                        /* [sct] Limits of the current hyperslab */
 int lmt_nbr,                          /* I [nbr] Number of limit structures in list */
 const trv_tbl_sct * const trv_tbl)    /* I [sct] Traversal table */
{
  lmt_sct *lmt_rgl;
  lmt_all_sct * lmt_all_crr;
  int nbr_dmn_all;             /* [nbr] Total number of dimensions in file */
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  long dmn_sz;                 /* [nbr] Dimension size */ 
  int *dmn_ids;                /* [ID]  Dimension IDs */ 
  int grp_id;                  /* [ID]  Group ID */
  int nbr_att;                 /* [nbr] Number of attributes */
  int nbr_var;                 /* [nbr] Number of variables */
  int nbr_dmn;                 /* [nbr] number of dimensions */
  int nbr_dmn_ult;             /* [nbr] Number of unlimited dimensions */
  int dmn_ids_ult[NC_MAX_DIMS];/* [nbr] Unlimited dimensions IDs array */
  int idx;                     /* [idx] Global index for lmt_all_lst */
  int jdx;
  int fl_fmt;                  /* [enm] netCDF file format */
  const int flg_prn=0;         /* [flg] All the dimensions in all parent groups will also be retrieved */        

  (void)nco_inq_format(in_id,&fl_fmt);
  
  /* Initialize counters/indices */
  nbr_dmn_all=0;
  idx=0;
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if(trv.typ == nco_obj_typ_grp){

      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(in_id,trv.nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=in_id;
      }

      /* Obtain unlimited dimensions for group: NOTE using group ID */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      /* Obtain dimensions for group: NOTE using group ID */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif

      dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,flg_prn);

      /* List dimensions using obtained group ID */
      for(jdx=0;jdx<nbr_dmn;jdx++){
        (void)nco_inq_dim(grp_id,dmn_ids[jdx],dmn_nm,&dmn_sz);

        /*NB: increment lmt_all_lst with global idx */
        lmt_all_crr=lmt_all_lst[idx]=(lmt_all_sct *)nco_malloc(sizeof(lmt_all_sct));
        lmt_all_crr->lmt_dmn=(lmt_sct **)nco_malloc(sizeof(lmt_sct *));
        lmt_all_crr->dmn_nm=strdup(dmn_nm);
        lmt_all_crr->lmt_dmn_nbr=1;
        lmt_all_crr->dmn_sz_org=dmn_sz;
        lmt_all_crr->WRP=False;
        lmt_all_crr->BASIC_DMN=True;
        lmt_all_crr->MSA_USR_RDR=False;

        lmt_all_crr->lmt_dmn[0]=(lmt_sct *)nco_malloc(sizeof(lmt_sct)); 
        /* Dereference */
        lmt_rgl=lmt_all_crr->lmt_dmn[0]; 
        lmt_rgl->nm=strdup(lmt_all_crr->dmn_nm);
        /* NB: ID is dmn_ids[jdx] */
        lmt_rgl->id=dmn_ids[jdx];

        /* NOTE: for a group case, to find out if a dimension is a record dimension, compare the 
        dimension ID with the unlimited dimension ID */
        lmt_rgl->is_rec_dmn=False;
        for(int kdx=0;kdx<nbr_dmn_ult;kdx++){
          if (dmn_ids[jdx] == dmn_ids_ult[kdx]){
            lmt_rgl->is_rec_dmn=True;
          }
        } /* end kdx */

        lmt_rgl->srt=0L;
        lmt_rgl->end=dmn_sz-1L;
        lmt_rgl->cnt=dmn_sz;
        lmt_rgl->srd=1L;
        lmt_rgl->drn=1L;
        lmt_rgl->flg_mro=False;
        lmt_rgl->min_sng=NULL;
        lmt_rgl->max_sng=NULL;
        lmt_rgl->srd_sng=NULL;
        lmt_rgl->drn_sng=NULL;
        lmt_rgl->mro_sng=NULL;
        lmt_rgl->rbs_sng=NULL;
        lmt_rgl->origin=0.0;

        /* A hack so we know structure has been initialized */
        lmt_rgl->lmt_typ=-1;

        /*NB: increment lmt_all_lst with global idx */
        idx++;

      } /* end jdx dimensions */
      (void)nco_free(dmn_ids);

      /* Increment total number of dimensions in file */
      nbr_dmn_all+=trv.nbr_dmn;

    } /* end nco_obj_typ_grp */
  } /* end uidx  */

  /* fxm: subroutine-ize this MSA code block for portability TODO nco926 */
  /* Add user specified limits lmt_all_lst */
  for(idx=0;idx<lmt_nbr;idx++){
    for(jdx=0;jdx<nbr_dmn_fl;jdx++){
      if(!strcmp(lmt[idx]->nm,lmt_all_lst[jdx]->dmn_nm)){   
        lmt_all_crr=lmt_all_lst[jdx];
        lmt_all_crr->BASIC_DMN=False;
        if(lmt_all_crr->lmt_dmn[0]->lmt_typ == -1) { 
          /* Free default limit set above structure first */
          lmt_all_crr->lmt_dmn[0]=(lmt_sct*)nco_lmt_free(lmt_all_crr->lmt_dmn[0]);
          lmt_all_crr->lmt_dmn[0]=lmt[idx]; 
        }else{ 
          lmt_all_crr->lmt_dmn=(lmt_sct**)nco_realloc(lmt_all_crr->lmt_dmn,((lmt_all_crr->lmt_dmn_nbr)+1)*sizeof(lmt_sct *));
          lmt_all_crr->lmt_dmn[(lmt_all_crr->lmt_dmn_nbr)++]=lmt[idx];
        } /* endif */
        break;
      } /* end if */
    } /* end loop over dimensions */
    /* Dimension in limit not found */
    if(jdx == nbr_dmn_fl){
      (void)fprintf(stderr,"Unable to find limit dimension %s in list\n",lmt[idx]->nm);
      nco_exit(EXIT_FAILURE);
    } /* end if err */
  } /* end loop over idx */       

  /* fxm: subroutine-ize this MSA code block for portability TODO nco926 */
  for(idx=0;idx<nbr_dmn_fl;idx++){
    nco_bool flg_ovl;

    /* NB: ncra/ncrcat have only one limit for record dimension so skip 
    evaluation otherwise this messes up multi-file operation */
    if(lmt_all_lst[idx]->lmt_dmn[0]->is_rec_dmn && (prg_get() == ncra || prg_get() == ncrcat)) continue;

    /* Split-up wrapped limits */   
    (void)nco_msa_wrp_splt(lmt_all_lst[idx]);

    /* NB: Wrapped hyperslabs are dimensions broken into the "wrong" order,
    e.g., from -d time,8,2 broken into -d time,8,9 -d time,0,2
    WRP flag set only when list contains dimensions split as above */
    if(lmt_all_lst[idx]->WRP == True){
      /* Find and store size of output dim */  
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);       
      continue;
    } /* endif */

    /* Single slab---no analysis needed */  
    if(lmt_all_lst[idx]->lmt_dmn_nbr == 1){
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);       
      continue;    
    } /* endif */

    if(MSA_USR_RDR){
      lmt_all_lst[idx]->MSA_USR_RDR=True;
      /* Find and store size of output dimension */  
      (void)nco_msa_clc_cnt(lmt_all_lst[idx]);       
      continue;
    } /* endif */

    /* Sort limits */
    (void)nco_msa_qsort_srt(lmt_all_lst[idx]);
    /* Check for overlap */
    flg_ovl=nco_msa_ovl(lmt_all_lst[idx]);  
    if(flg_ovl==False) lmt_all_lst[idx]->MSA_USR_RDR=True;

    /* Find and store size of output dimension */  
    (void)nco_msa_clc_cnt(lmt_all_lst[idx]);       
    if(dbg_lvl_get() > 1){
      if(flg_ovl) (void)fprintf(stdout,"%s: dimension \"%s\" has overlapping hyperslabs\n",prg_nm_get(),lmt_all_lst[idx]->dmn_nm); else (void)fprintf(stdout,"%s: dimension \"%s\" has distinct hyperslabs\n",prg_nm_get(),lmt_all_lst[idx]->dmn_nm); 
    } /* endif */

  } /* end idx */    

#ifdef NCO_SANITY_CHECK
  assert(nbr_dmn_fl == nbr_dmn_all);
#endif
} /* end nco4_msa_lmt_all_int() */

nm_id_sct *                         /* O [sct] Extraction list */                         
nco_aux_add_dmn_trv                 /* [fnc] Add a coordinate variable that matches parameter "var_nm" */
(const int nc_id,                   /* I [id] netCDF file ID */
 const char * const var_nm,         /* I [sng] Variable name to find */
 nm_id_sct *xtr_lst,                /* I/O [sct] Current extraction list  */
 int * const xtr_nbr,               /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl)/* I [sct] Traversal table */
{
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  long dmn_sz;                 /* [nbr] Dimension size */  
  int nbr_att;                 /* [nbr] Number of attributes */
  int nbr_var;                 /* [nbr] Number of variables */
  int nbr_dmn;                 /* [nbr] number of dimensions */
  int dmn_id[NC_MAX_DIMS];     /* [id] Dimensions IDs array for group */
  const int flg_prn=0;         /* [flg] All the dimensions in all parent groups will also be retrieved */        

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_var){

      /* Check if current variable matches requested variable */ 
      if(!strcmp(trv.nm,var_nm)){
        int  grp_id;     
        int fl_fmt;
        (void)nco_inq_format(nc_id,&fl_fmt);
        /* Obtain group ID from netCDF API using full group name */
        if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
          (void)nco_inq_grp_full_ncid(nc_id,trv.grp_nm_fll,&grp_id);
        }else{ /* netCDF3 case */
          grp_id=nc_id;
        }

        /* Obtain number of dimensions for group: NOTE using group ID */
        (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);

        /* Obtain dimension IDs */
        (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_id,flg_prn);

        /* List dimensions in group */ 
        for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++){ 
          (void)nco_inq_dim(grp_id,dmn_id[idx_dmn],dmn_nm,&dmn_sz); 

          /* Check if dimension matches the requested variable (it is a coordinate variable) */ 
          if(!strcmp(dmn_nm,var_nm)){

            /* Construct the full (dimension/variable) name to avoid duplicate insertions */
            char* dm_nm_fll=(char*)nco_malloc(strlen(trv.grp_nm_fll)+strlen(dmn_nm)+2);
            strcpy(dm_nm_fll,trv.grp_nm_fll);
            if(strcmp(trv.grp_nm_fll,"/") !=0 ) strcat(dm_nm_fll,"/");
            strcat(dm_nm_fll,dmn_nm); 

            /* Check if requested coordinate variable is already on extraction list */
            if(!xtr_lst_fnd(dm_nm_fll,xtr_lst,*xtr_nbr)){
              /* Obtain variable ID from netCDF API using group ID */
              int var_id;
              (void)nco_inq_varid(grp_id,trv.nm,&var_id);

              /* Add coordinate to list
              NOTE: Needed members for traversal code:
              1) "grp_nm_fll": needed to "nco_inq_grp_full_ncid": obtain group ID from group path and netCDF file ID
              2) "nm": needed to "nco_prn_var_dfn" to print variable's definition 
              3) "grp_id": needed to "nco_prn_var_dfn" to print variable's definition 
              4) "id": needed for "nco_prn_att"  to print variable's attributes
              5) "var_nm_fll": using full name to compare criteria */

              /* Out with it */
              xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));
              xtr_lst[*xtr_nbr].grp_nm_fll=strdup(trv.grp_nm_fll);     
              xtr_lst[*xtr_nbr].var_nm_fll=strdup(trv.nm_fll);
              xtr_lst[*xtr_nbr].nm=strdup(var_nm);
              xtr_lst[*xtr_nbr].id=var_id;                  
              (*xtr_nbr)++; /* NB: Changes size  */
            } /* End check if requested coordinate variable is already on extraction list */
            
            /* Free allocated */
            dm_nm_fll=(char *)nco_free(dm_nm_fll);
          }/* end check if dimension matches the requested variable */
        } /* end idx_dmn dimensions */ 
      } /* end check if current variable matches requested variable */
    } /* end nco_obj_typ_var */
  } /* end uidx  */

  return xtr_lst;
} /* end nco_aux_add_dmn_trv() */ 

nco_bool                    /* O [flg] Dimension was found */
nco_fnd_dmn                 /* [fnc] Find a dimension that matches dmn_nm in group grp_id and its parents */
(const int grp_id,          /* I [id] Group ID */
 const char * const dmn_nm, /* I [sng] Dimension name to find */
 const long dmn_sz)         /* I [nbr] Dimension size to find */
{
  typedef struct{			
    char nm[NC_MAX_NAME+1];
    size_t sz;
  } nco_dmn_t;

  nco_dmn_t *dmn_lst=NULL;
  const int flg_prn=1;         /* [flg] All the dimensions in all parent groups will also be retrieved */        
  int *dmn_ids=NULL;
  int dmn_idx;
  int nbr_dmn;

  (void)nco_inq_dimids(grp_id,&nbr_dmn,NULL,flg_prn);
  dmn_lst=(nco_dmn_t *)nco_malloc(nbr_dmn*sizeof(nco_dmn_t));
  dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
  (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,flg_prn);

  for(dmn_idx=0;dmn_idx<nbr_dmn;dmn_idx++) nc_inq_dim(grp_id,dmn_ids[dmn_idx],dmn_lst[dmn_idx].nm,&dmn_lst[dmn_idx].sz);

  for(dmn_idx=0;dmn_idx<nbr_dmn;dmn_idx++){
    if(!strcmp(dmn_nm,dmn_lst[dmn_idx].nm) && ((size_t)dmn_sz == dmn_lst[dmn_idx].sz)){
      dmn_ids=(int *)nco_free(dmn_ids);
      dmn_lst=(nco_dmn_t *)nco_free(dmn_lst);
      return True;
    } /* endif */
  } /* end loop */

  dmn_ids=(int *)nco_free(dmn_ids);
  dmn_lst=(nco_dmn_t *)nco_free(dmn_lst);

  return False;
} /* end nco_fnd_dmn() */ 


void 
nco_nm_id_val                 /* [fnc] Validated name-ID structure list */
(nm_id_sct * const nm_id_lst, /* I [sct] Name-ID structure list */
 const int nm_id_nbr)         /* I [nbr] Number of name-ID structures in list */
{
  if(dbg_lvl_get() < nco_dbg_dev) return;
  (void)fprintf(stdout,"%d items\n",nm_id_nbr);
  for(int idx=0;idx<nm_id_nbr;idx++){
    nm_id_sct nm_id=nm_id_lst[idx];
    (void)fprintf(stdout,"[%d] %s %s %s (%d)\n",idx,nm_id.grp_nm_fll,nm_id.var_nm_fll,nm_id.nm,nm_id.id); 
  } 

  for(int idx=0;idx<nm_id_nbr;idx++){
    nm_id_sct nm_id=nm_id_lst[idx];
    assert(nm_id.nm);
    assert(nm_id.grp_nm_fll); 
    assert(nm_id.var_nm_fll);      
  } 
} /* end nco_nm_id_chk() */

void 
nco_nm_id_cmp                         /* [fnc] Compare 2 name-ID structure lists */
(nm_id_sct * const nm_id_lst1,        /* I [sct] Name-ID structure list */
 const int nm_id_nbr1,                /* I [nbr] Number of name-ID structures in list */
 nm_id_sct * const nm_id_lst2,        /* I [sct] Name-ID structure list */
 const int nm_id_nbr2,                /* I [nbr] Number of name-ID structures in list */
 const nco_bool SAME_ORDER)           /* I [flg] Both lists have the same order */
{
  int idx,jdx;
  assert(nm_id_nbr1 == nm_id_nbr2);
  if(SAME_ORDER){
    for(idx=0;idx<nm_id_nbr1;idx++){
      assert(strcmp(nm_id_lst1[idx].nm,nm_id_lst2[idx].nm) == 0);
      assert(strcmp(nm_id_lst1[idx].grp_nm_fll,nm_id_lst2[idx].grp_nm_fll) == 0);
      assert(strcmp(nm_id_lst1[idx].var_nm_fll,nm_id_lst2[idx].var_nm_fll) == 0);
    }
  }else{ /* SAME_ORDER */

    int nm_id_nbr=0;
    for(idx=0;idx<nm_id_nbr1;idx++){
      nm_id_sct nm_id_1=nm_id_lst1[idx];
      for(jdx=0;jdx<nm_id_nbr2;jdx++){
        nm_id_sct nm_id_2=nm_id_lst2[jdx];
        if(strcmp(nm_id_1.var_nm_fll,nm_id_2.var_nm_fll) == 0){
          nm_id_nbr++;
          assert(strcmp(nm_id_lst1[idx].nm,nm_id_lst2[jdx].nm) == 0);
          assert(strcmp(nm_id_lst1[idx].grp_nm_fll,nm_id_lst2[jdx].grp_nm_fll) == 0);
          assert(strcmp(nm_id_lst1[idx].var_nm_fll,nm_id_lst2[jdx].var_nm_fll) == 0);
        }
      }/* jdx */
    }/* idx */
    assert(nm_id_nbr == nm_id_nbr1);
  } /* SAME_ORDER */
} /* end nco_nm_id_cmp() */

void
nco_var_lst_mk_trv2                   /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id,                     /* I [ID] Apex group ID */
 char * const * const grp_lst_in,     /* I [sng] User-specified list of groups names to extract (specified with -g) */
 const int grp_xtr_nbr,               /* I [nbr] Number of groups in current extraction list (specified with -g) */
 char * const * const var_lst_in,     /* I [sng] User-specified list of variable names and rx's */
 const int var_xtr_nbr,               /* I [nbr] User-specified list of variables (specified with -v) */
 const nco_bool EXTRACT_ALL_COORDINATES,  /* I [flg] Process all coordinates */ 
 trv_tbl_sct * trv_tbl)               /* I/O [sct] Traversal table */
{
  /* Purpose: Create variable extraction list with or without regular expressions */

  char *var_sng;              /* [sng] User-specified variable name or regular expression */
  char grp_nm[NC_MAX_NAME];   /* [sng] Relative group name */
  int grp_id;                 /* [ID]  Group ID */
  int idx_grp;                /* [idx] Group index */
  int idx_var_crr;            /* [idx] Total variable index */
  int var_nbr_all;            /* [nbr] Number of variables in input file */
  nco_bool *var_xtr_rqs=NULL; /* [flg] Variable specified in extraction list */
  nm_id_sct *var_lst_all=NULL;/* [sct] All variables in input file */
  int fl_fmt;

  /* CASE 1: both -v and -g were not specified: return all variables if none were specified and not -c ... */
  if(var_xtr_nbr == 0 && grp_xtr_nbr == 0 && !EXTRACT_ALL_COORDINATES){
    for(unsigned int uidx=0;uidx<trv_tbl->nbr;uidx++){
      if (trv_tbl->lst[uidx].typ == nco_obj_typ_var){   
        trv_tbl->lst[uidx].flg=True;
      }
    } /* end uidx */
    return;
  } /* end if */

  /* Get number of variables in file */ 
  (void)trv_tbl_inq((int*)NULL,(int*)NULL,&var_nbr_all,(int*)NULL,trv_tbl); 

  /* Allocate */ 
  var_lst_all=(nm_id_sct *)nco_malloc(var_nbr_all*sizeof(nm_id_sct)); 

  /* Build nm_id_sct needed for nco_lst_rx_search */ 
  idx_var_crr=0;
  for(unsigned int uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->lst[uidx].typ == nco_obj_typ_var){
      var_lst_all[idx_var_crr].var_nm_fll=(char *)strdup(trv_tbl->lst[uidx].nm_fll);
      var_lst_all[idx_var_crr].nm=(char *)strdup(trv_tbl->lst[uidx].nm);
      var_lst_all[idx_var_crr].grp_nm_fll=(char *)strdup(trv_tbl->lst[uidx].grp_nm_fll);
      idx_var_crr++; 
    } /* end nco_obj_typ_var */
  } /* end uidx */

#ifdef NCO_SANITY_CHECK
  assert(var_nbr_all == idx_var_crr);
#endif /* !NCO_SANITY_CHECK */

  /* Get file format */
  (void)nco_inq_format(nc_id,&fl_fmt);

  /* Initialize and allocate extraction flag array to all False */
  var_xtr_rqs=(nco_bool *)nco_calloc((size_t)var_nbr_all,sizeof(nco_bool));

  /* CASE 2: -v was specified (regardles of whether -g was specified)
  Method: Outer loop over all objects in file contains inner loop over user-supplied variable list
  Add variable to extraction list if it matches user-supplied name
  Regular expressions are allowed */
  if(var_xtr_nbr){
    idx_var_crr=0;
    /* Loop over all objects in file */
    for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
      /* Loop through user-specified variable list */
      for(int idx=0;idx<var_xtr_nbr;idx++){
        var_sng=var_lst_in[idx];
        /* Convert pound signs (back) to commas */
        nco_hash2comma(var_sng);

        /* If var_sng is regular expression ... */
        if(strpbrk(var_sng,".*^$\\[]()<>+?|{}")){
          /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
          int rx_mch_nbr=nco_lst_rx_search(var_nbr_all,var_lst_all,var_sng,var_xtr_rqs);
          if(!rx_mch_nbr) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any variables\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),var_sng); 
          continue;
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
          (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),var_sng);
          nco_exit(EXIT_FAILURE);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
        } /* end if regular expression */

        /* Compare var_nm from main iteration with var_sng found and, if equal, add to extraction list */
        if(!strcmp(var_sng,var_lst_all[idx_var_crr].nm)){
          if(!grp_xtr_nbr){
            /* No groups specified with -g, so add variable to extraction list */
            var_xtr_rqs[idx_var_crr]=True;
          }else{ /* grp_xtr_nbr */
            /* Groups specified with -g, so add variable to extraction list only if in matching group */
            for(idx_grp=0;idx_grp<grp_xtr_nbr;idx_grp++){
              if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
                (void)nco_inq_grp_full_ncid(nc_id,var_lst_all[idx_var_crr].grp_nm_fll,&grp_id);
              }else{ 
                grp_id=nc_id;
              } 
              (void)nco_inq_grpname(grp_id,grp_nm);
              if(!strcmp(grp_nm,grp_lst_in[idx_grp])){
                var_xtr_rqs[idx_var_crr]=True;
              } /* end strcmp() */
            } /* end idx_grp */
          } /* end grp_xtr_nbr */
        }  /* end strcmp() */
      } /* end loop over var_lst_in */ 

      /* Increment variable index for var_lst_all if table object is a variable; this keeps two lists in sync */
      if(trv_tbl->lst[uidx].typ == nco_obj_typ_var){
#ifdef NCO_SANITY_CHECK
        /* Full variable names in both lists must agree */
        assert(!strcmp(var_lst_all[idx_var_crr].var_nm_fll,trv_tbl->lst[uidx].nm_fll));
#endif /* !NCO_SANITY_CHECK */
        idx_var_crr++; 
      } /* end nco_obj_typ_var */
    } /* end loop over trv_tbl uidx */

  }else if(grp_xtr_nbr && var_xtr_nbr == 0){ 

    /* CASE 3: -v was not specified and -g was
    Regular expressions are not yet allowed in -g arguments */

#ifdef NCO_SANITY_CHECK
    assert(var_nbr_all == idx_var_crr);
#endif /* !NCO_SANITY_CHECK */

    for(idx_grp=0;idx_grp<grp_xtr_nbr;idx_grp++){ /* Outer loop over user-specified group list */
      for(idx_var_crr=0;idx_var_crr<var_nbr_all;idx_var_crr++){ /* Inner loop over variables */
        if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
          (void)nco_inq_grp_full_ncid(nc_id,var_lst_all[idx_var_crr].grp_nm_fll,&grp_id);
        }else{ 
          grp_id=nc_id;
        } 
        (void)nco_inq_grpname(grp_id,grp_nm);
        if(!strcmp(grp_nm,grp_lst_in[idx_grp])){
          var_xtr_rqs[idx_var_crr]=True;
        } /* end strcmp */
      } /* end idx_var_crr */
    } /* end idx_grp */
  } /* end Case 3 */

  /* Create final variable list using boolean flag array */
  idx_var_crr=0;
  for(unsigned int uidx=0;uidx<trv_tbl->nbr;uidx++){
    /* Increment idx_var_crr index only when table object is a variable; this keeps two lists in sync */
    if (trv_tbl->lst[uidx].typ == nco_obj_typ_var){ 
      /* NOTE: True/False must be set, initial value is -1; groups always stay -1 */ 
      if(var_xtr_rqs[idx_var_crr]){
        trv_tbl->lst[uidx].flg=True;
      }else {
        trv_tbl->lst[uidx].flg=False;
      }
      idx_var_crr++; 
    } /* end nco_obj_typ_var */
  } /* end uidx */

  /* Clean up memory */
  var_lst_all=(nm_id_sct *)nco_nm_id_lst_free(var_lst_all,var_nbr_all);
  var_xtr_rqs=(nco_bool *)nco_free(var_xtr_rqs);

} /* end nco_var_lst_mk_trv2() */


void
nco_var_lst_xcl_trv2                  /* [fnc] Convert exclusion list to extraction list */
(trv_tbl_sct * trv_tbl)               /* I/O [sct] Traversal table */
{
  /* Purpose: Convert exclusion list to extraction list */
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->lst[uidx].flg != nco_obj_typ_err){
      assert(trv_tbl->lst[uidx].typ == nco_obj_typ_var);
      trv_tbl->lst[uidx].flg=!trv_tbl->lst[uidx].flg;
    } /* end nco_obj_typ_var */
  } /* end loop over uidx */
  return;
} /* end nco_var_lst_xcl_trv2() */

void
nco_var_lst_crd_add_trv2              /* [fnc] Add all coordinates to extraction list */
(const int nc_id,                     /* I [ID] netCDF file ID */
 const int grp_xtr_nbr,               /* I [nbr] Number of groups in current extraction list (specified with -g) */
 char * const * const grp_lst_in,     /* I [sng] User-specified list of groups names to extract (specified with -g ) */
 trv_tbl_sct *trv_tbl)                /* I/O [sct] Traversal table */
{
  /* Purpose: Add all coordinates to extraction list
  Find all coordinates (dimensions which are also variables) and
  add them to the list if they are not already there. */

  int grp_id;                    /* [ID]  Group ID in input file */
  int nbr_att;                   /* [nbr] Number of attributes for group */
  int nbr_var;                   /* [nbr] Number of variables for group */
  int nbr_dmn;                   /* [nbr] Number of dimensions for group */
  int nbr_grp;                   /* [nbr] Number of groups for group */
  int fl_fmt;                    /* [enm] netCDF file format */
  char *var_nm_fll;              /* [sng] Full path of variable */
  int *var_ids;                  /* [ID]  Variable IDs array */
  char var_nm[NC_MAX_NAME+1];    /* [sng] Variable name */ 
  int nbr_dmn_ult;               /* [nbr] Number of unlimited dimensions */
  int dmn_ids_ult[NC_MAX_DIMS];  /* [ID]  Unlimited dimensions IDs array */
  int dmn_ids[NC_MAX_DIMS];      /* [nbr] Dimensions IDs array */
  char dmn_nm[NC_MAX_NAME];      /* [sng] Dimension name */ 
  long dmn_sz;                   /* [nbr] Dimension size */ 
  const int flg_prn=0;           /* [flg] All the dimensions in all parent groups will also be retrieved */        

  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_grp ){

      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=nc_id;
      }

      /* Obtain info for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
      (void)nco_inq_grps(grp_id,&nbr_grp,(int *)NULL);

      /* Get dimension IDs for group */
      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,flg_prn);
#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att && nbr_grp == trv.nbr_grp);
#endif

      /* Allocate space for and obtain variable IDs in current group */
      var_ids=(int *)nco_malloc(nbr_var*sizeof(int));
      (void)nco_inq_varids(grp_id,&nbr_var,var_ids);

      /* Get number of unlimited dimensions */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      /* Get variables for this group */
      for(int idx_var=0;idx_var<nbr_var;idx_var++){
        var_nm_fll=NULL;

        /* Get name of current variable in current group NOTE: using obtained IDs array */
        (void)nco_inq_varname(grp_id,var_ids[idx_var],var_nm);

        /* Allocate path buffer; add space for a trailing NUL */ 
        var_nm_fll=(char*)nco_malloc(strlen(trv.nm_fll)+strlen(var_nm)+2);

        /* Initialize path with the current absolute group path */
        strcpy(var_nm_fll,trv.nm_fll);
        if(strcmp(trv.nm_fll,"/")!=0) /* If not root group, concatenate separator */
          strcat(var_nm_fll,"/");
        strcat(var_nm_fll,var_nm); /* Concatenate variable to absolute group path */

        /* List dimensions */
        for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++){

          /* Get dimension info */
          (void)nco_inq_dim(grp_id,dmn_ids[idx_dmn],dmn_nm,&dmn_sz);

          /* Compare variable name with dimension name */
          if(strcmp(dmn_nm,var_nm) == 0){
            /* No groups case, just add  */
            if (grp_xtr_nbr == 0 ){
              (void)trv_tbl_mrk(var_nm_fll,trv_tbl);
            }
            /* Groups -g case, add only if current group name GRP_NM matches any of the supplied GRP_LST_IN names */
            else{  
              /* Loop through user-specified group list */
              for(int idx_grp=0;idx_grp<grp_xtr_nbr;idx_grp++){
                /* Locate group name from -g in traversal name */
                char* pch=strstr(trv.nm,grp_lst_in[idx_grp]);
                /* strstr returns the first occurrence of 'grp_lst_in' in 'trv.nm', the higher level group( closer to root) */
                if(pch != NULL){
                  (void)trv_tbl_mrk(var_nm_fll,trv_tbl);
                }
              } /* end idx_grp */       
            } /* end groups case */
          } /* end strcmp coordinate name */
        } /* end jdx dimensions */

        /* Memory management after current variable */
        var_nm_fll=(char *)nco_free(var_nm_fll);

      } /* end get variables for this group */  

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);

    } /* end nco_obj_typ_grp */
  } /* end uidx  */

  return;
} /* end nco_var_lst_crd_add_trv2() */


void
nco_var_lst_crd_add_cf_trv2           /* [fnc] Add to extraction list all coordinates associated with CF convention */
(const int nc_id,                     /* I netCDF file ID */
 const char * const cf_nm,            /* I [sng] CF name to find ( "coordinates" or "bounds" */
 trv_tbl_sct *trv_tbl)                /* I/O [sct] Traversal table */
{
  /* Detect associated coordinates specified by CF "coordinates" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.1/cf-conventions.html#coordinate-system */ 

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_var){

      /* Try to add to extraction list */
      (void)nco_aux_add_cf2(nc_id,trv.nm_fll,trv.nm,cf_nm,trv_tbl);

    } /* end nco_obj_typ_var */
  } /* end uidx  */

  return;
} /* nco_var_lst_crd_add_cf_trv2() */

void
nco_aux_add_cf2                       /* [fnc] Add to extraction list all coordinates associated with CF convention (associated with "var_nm_fll")*/
(const int nc_id,                     /* I netCDF file ID */
 const char * const var_nm_fll,       /* I [sng] Full variable name */
 const char * const var_nm,           /* I [sng] Variable relative name */
 const char * const cf_nm,            /* I [sng] CF name to find ( "coordinates" or "bounds" */
 trv_tbl_sct *trv_tbl)                /* I/O [sct] Traversal table */
{
  /* Detect associated coordinates specified by CF "coordinates" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.1/cf-conventions.html#coordinate-system */ 

  int rcd=NC_NOERR;         /* [rcd] Return code */
  char att_nm[NC_MAX_NAME]; /* [sng] Attribute name */
  int nbr_att;              /* [nbr] Number of attributes */
  int var_id;               /* [id]  Variable ID */
  int grp_id;               /* [id]  Group ID */
  char **bnd_lst;           /* [sng] 1D array of list elements */
  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  int nbr_bnd;              /* [nbr] Number of coordinates specified in "bounds" (or "coordinates") attribute */

  /* Obtain group ID using nco_aux_grp_id (get ID from full variable name ) */
  grp_id=nco_aux_grp_id(nc_id,var_nm_fll);

  /* Obtain variable ID from netCDF API using group ID. NOTE: using relative var name  */
  (void)nco_inq_varid(grp_id,var_nm,&var_id);

  /* Find number of attributes */
  (void)nco_inq_varnatts(grp_id,var_id,&nbr_att);
  for(int idx_att=0;idx_att<nbr_att;idx_att++){
    (void)nco_inq_attname(grp_id,var_id,idx_att,att_nm);

    /* Is attribute part of CF convention? */
    if(strcmp(att_nm,cf_nm) == 0){
      char *att_val;
      long att_sz;
      nc_type att_typ;
      int bnd_id;

      /* Yes, get list of specified attributes */
      (void)nco_inq_att(grp_id,var_id,att_nm,&att_typ,&att_sz);
      if(att_typ != NC_CHAR){
        (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore will skip this attribute.\n",prg_nm_get(),att_nm,var_nm_fll,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR));
        return;
      } /* end if */
      att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
      if(att_sz > 0) (void)nco_get_att(grp_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
      /* NUL-terminate attribute */
      att_val[att_sz]='\0';

      /* Split list into separate coordinate names
      Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
      bnd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_bnd);
      /* ...for each coordinate in "bounds" attribute... */
      for(int idx_bnd=0;idx_bnd<nbr_bnd;idx_bnd++){
        char* bnd_lst_var=bnd_lst[idx_bnd];
        if(bnd_lst_var==NULL)
          continue;
        /* Verify "bounds" exists in input file. NOTE: using group ID */
        rcd=nco_inq_varid_flg(grp_id,bnd_lst_var,&bnd_id);
        /* NB: Coordinates of rank N have bounds of rank N+1 */
        if(rcd == NC_NOERR){

          /* Try to find the variable */
          nm_id_sct nm_id;
          if (nco_fnd_var_trv(nc_id,bnd_lst_var,trv_tbl,&nm_id) == 1 )
          {
            char *pch;        /* Pointer to the last occurrence of character */
            int   pos;        /* Position of character */
            char *grp_nm_fll; /* Fully qualified group where variable resides */
            int  len;         /* Lenght of fully qualified group where variable resides */

            len=strlen(var_nm_fll);
            grp_nm_fll=(char*)nco_malloc((len+1L)*sizeof(char));
            strcpy(grp_nm_fll,var_nm_fll);
            pch=strrchr(grp_nm_fll,'/');
            pos=pch-grp_nm_fll;
            grp_nm_fll[pos]='\0';

            /* Construct the full variable name */
            char* cf_nm_fll=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(bnd_lst_var)+2);
            strcpy(cf_nm_fll,grp_nm_fll);
            if(strcmp(grp_nm_fll,"/")!=0) strcat(cf_nm_fll,"/");
            strcat(cf_nm_fll,bnd_lst_var); 

            /* Free allocated memory */
            grp_nm_fll=(char *)nco_free(grp_nm_fll);

            /* Check if the  variable is already in the  extraction list: NOTE using full name "cf_nm_fll" */
            if(trv_tbl_fnd_var_nm_fll(cf_nm_fll,trv_tbl)){

              /* Add variable to list
              NOTE: Needed members for traversal code:
              1) "grp_nm_fll": needed to "nco_inq_grp_full_ncid": obtain group ID from group path and netCDF file ID
              2) "nm": needed to "nco_prn_var_dfn" to print variable's definition 
              3) "grp_id": needed to "nco_prn_var_dfn" to print variable's definition 
              4) "id": needed for "nco_prn_att"  to print variable's attributes
              5) "var_nm_fll": using full name to compare criteria */

              (void)trv_tbl_mrk(cf_nm_fll,trv_tbl);
            }

          } /* end nco_fnd_var_trv() */   

          /* Continue to next coordinate in loop */
          continue;


        }else{ /* end Verify "bounds" exists in input file */
          if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO Variable %s, specified in the \"bounds\" attribute of variable %s, is not present in the input file\n",prg_nm_get(),bnd_lst[idx_bnd],var_nm_fll);
        } /* end else named coordinate exists in input file */
      } /* end loop over idx_bnd */

      /* Free allocated memory */
      att_val=(char *)nco_free(att_val);
      bnd_lst=nco_sng_lst_free(bnd_lst,nbr_bnd);

    } /* end strcmp Is attribute part of CF convention? */
  } /* end find number of attributes */

  return;
} /* nco_aux_add_cf2() */

void                               
nco_aux_add_dmn_trv2                   /* [fnc] Add a coordinate variable that matches parameter "var_nm" */
(const int nc_id,                      /* I [id] netCDF file ID */
 const char * const var_nm,            /* I [sng] Variable name to find */
 trv_tbl_sct *trv_tbl)                 /* I/O [sct] Traversal table */
{
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  long dmn_sz;                 /* [nbr] Dimension size */  
  int nbr_att;                 /* [nbr] Number of attributes */
  int nbr_var;                 /* [nbr] Number of variables */
  int nbr_dmn;                 /* [nbr] number of dimensions */
  int dmn_id[NC_MAX_DIMS];     /* [id] Dimensions IDs array for group */
  const int flg_prn=0;         /* [flg] All the dimensions in all parent groups will also be retrieved */ 
  int fl_fmt;                  /* [enm] netCDF file format */
  
  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.typ == nco_obj_typ_var){

      /* Check if current variable matches requested variable */ 
      if(!strcmp(trv.nm,var_nm)){
        int  grp_id;     
        
        /* Obtain group ID from netCDF API using full group name */
        if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
          (void)nco_inq_grp_full_ncid(nc_id,trv.grp_nm_fll,&grp_id);
        }else{ /* netCDF3 case */
          grp_id=nc_id;
        }

        /* Obtain number of dimensions for group: NOTE using group ID */
        (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);

        /* Obtain dimension IDs */
        (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_id,flg_prn);

        /* List dimensions in group */ 
        for(int idx_dmn=0;idx_dmn<nbr_dmn;idx_dmn++){ 
          (void)nco_inq_dim(grp_id,dmn_id[idx_dmn],dmn_nm,&dmn_sz); 

          /* Check if dimension matches the requested variable (it is a coordinate variable) */ 
          if(!strcmp(dmn_nm,var_nm)){

            /* Construct the full (dimension/variable) name to avoid duplicate insertions */
            char* dm_nm_fll=(char*)nco_malloc(strlen(trv.grp_nm_fll)+strlen(dmn_nm)+2);
            strcpy(dm_nm_fll,trv.grp_nm_fll);
            if(strcmp(trv.grp_nm_fll,"/") !=0 ) strcat(dm_nm_fll,"/");
            strcat(dm_nm_fll,dmn_nm); 

            /* Check if the  variable is already in the  extraction list: NOTE using full name "cf_nm_fll" */
            if(trv_tbl_fnd_var_nm_fll(dm_nm_fll,trv_tbl)){

              /* Add variable to list
              NOTE: Needed members for traversal code:
              1) "grp_nm_fll": needed to "nco_inq_grp_full_ncid": obtain group ID from group path and netCDF file ID
              2) "nm": needed to "nco_prn_var_dfn" to print variable's definition 
              3) "grp_id": needed to "nco_prn_var_dfn" to print variable's definition 
              4) "id": needed for "nco_prn_att"  to print variable's attributes
              5) "var_nm_fll": using full name to compare criteria */

              (void)trv_tbl_mrk(dm_nm_fll,trv_tbl);

            } /* End check if requested coordinate variable is already on extraction list */

            /* Free allocated */
            dm_nm_fll=(char *)nco_free(dm_nm_fll);

          }/* end check if dimension matches the requested variable */
        } /* end idx_dmn dimensions */ 
      } /* end check if current variable matches requested variable */
    } /* end nco_obj_typ_var */
  } /* end uidx  */

  return;
} /* end nco_aux_add_dmn_trv2() */ 


void
nco_var_lst_crd_ass_add_cf_trv2       /* [fnc] Add to extraction list all coordinates associated with CF convention */
(const int nc_id,                     /* I netCDF file ID */
 const char * const cf_nm,            /* I [sng] CF name to find ( "coordinates" or "bounds" ) */
 trv_tbl_sct *trv_tbl)                /* I/O [sct] Traversal table */
{
  /* Detect associated coordinates specified by CF "coordinates" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.5/cf-conventions.html#coordinate-system */

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.flg == True){
#ifdef NCO_SANITY_CHECK
      assert(trv.typ == nco_obj_typ_var);
#endif
      /* Try to add to extraction list */
      (void)nco_aux_add_cf2(nc_id,trv.nm_fll,trv.nm,cf_nm,trv_tbl);

    } /* end nco_obj_typ_var */
  } /* end uidx  */

  return;
} /* nco_var_lst_crd_ass_add_cf_trv2() */



nm_id_sct *                           /* O [sct] Extraction list */  
nco_trv_tbl_nm_id                     /* [fnc] Convert a trv_tbl_sct to a nm_id_sct */
(const int nc_id,                     /* I [id] netCDF file ID */
 nm_id_sct *xtr_lst,                  /* I/O [sct] Current extraction list  */
 int * const xtr_nbr,                 /* I/O [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Purpose: Define a nm_id_sct* from a trv_tbl_sct* */
  int fl_fmt; /* [enm] netCDF file format */
  
  (void)nco_inq_format(nc_id,&fl_fmt);

  int nbr_tbl=0; /* [nbr] Number of marked .flg items in table */
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->lst[uidx].flg == True){
#ifdef NCO_SANITY_CHECK
      assert(trv_tbl->lst[uidx].typ == nco_obj_typ_var);
#endif
      nbr_tbl++;
    } /* end flg == True */
  } /* end loop over uidx */

  xtr_lst=(nm_id_sct *)nco_malloc(nbr_tbl*sizeof(nm_id_sct));

  nbr_tbl=0;
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->lst[uidx].flg == True){
      xtr_lst[nbr_tbl].var_nm_fll=(char *)strdup(trv_tbl->lst[uidx].nm_fll);
      xtr_lst[nbr_tbl].nm=(char *)strdup(trv_tbl->lst[uidx].nm);
      xtr_lst[nbr_tbl].grp_nm_fll=(char *)strdup(trv_tbl->lst[uidx].grp_nm_fll);
      /* To deprecate: generate ID needed only to test netCDf3 library and netCDf3 only functions to deprecate */
      int var_id;
      int grp_id;
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv_tbl->lst[uidx].grp_nm_fll,&grp_id);
      }else{ 
        grp_id=nc_id;
      }
      (void)nco_inq_varid(grp_id,trv_tbl->lst[uidx].nm,&var_id);
      xtr_lst[nbr_tbl].id=var_id;

      nbr_tbl++;
    } /* end flg == True */
  } /* end loop over uidx */

  *xtr_nbr=nbr_tbl;
  return xtr_lst;
} /* end nco_nm_id_trv_tbl() */


void
nco_trv_tbl_chk                       /* [fnc] Validate trv_tbl_sct from a nm_id_sct input */
(const int nc_id,                     /* I netCDF file ID */
 nm_id_sct * const xtr_lst,           /* I [sct] Extraction list  */
 const int xtr_nbr,                   /* I [nbr] Number of variables in extraction list */
 const trv_tbl_sct * const trv_tbl,   /* I [sct] Traversal table */
 const nco_bool NM_ID_SAME_ORDER)     /* I [flg] Both nm_id_sct have the same order */
{
  nm_id_sct *xtr_lst_chk=NULL;
  int xtr_nbr_chk;

  if(dbg_lvl_get() >= nco_dbg_dev){
    (void)xtr_lst_prn(xtr_lst,xtr_nbr);
    (void)trv_tbl_prn_xtr(trv_tbl);
  }
  xtr_lst_chk=nco_trv_tbl_nm_id(nc_id,xtr_lst_chk,&xtr_nbr_chk,trv_tbl);
  (void)nco_nm_id_cmp(xtr_lst_chk,xtr_nbr_chk,xtr_lst,xtr_nbr,NM_ID_SAME_ORDER);
  if(xtr_lst_chk != NULL)xtr_lst_chk=nco_nm_id_lst_free(xtr_lst_chk,xtr_nbr_chk);
  return;
} /* end nco_trv_tbl_chk() */

void
nco_var_lst_crd_ass_add_trv2          /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                     /* I netCDF file ID */
 trv_tbl_sct *trv_tbl)                /* I/O [sct] Traversal table */
{
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  long dmn_sz;                 /* [nbr] Dimension size */  
  int grp_id;                  /* [ID] Group ID */
  int var_id;                  /* [ID] Variable ID */
  int nbr_var_dim;             /* [nbr] Number of dimensions associated with current matched variable */
  int dmn_id_var[NC_MAX_DIMS]; /* [ID] Dimensions IDs array for variable */
  int fl_fmt;                  /* [nbr] File format */

  /* Get file format */
  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    if (trv.flg == True){
#ifdef NCO_SANITY_CHECK
      assert(trv.typ == nco_obj_typ_var);
#endif
      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.grp_nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=nc_id;
      }
      /* Obtain variable ID from netCDF API using group ID */
      (void)nco_inq_varid(grp_id,trv.nm,&var_id);

      /* Get number of dimensions for variable */
      (void)nco_inq_varndims(grp_id,var_id,&nbr_var_dim);

      /* Get dimension IDs for variable */
      (void)nco_inq_vardimid(grp_id,var_id,dmn_id_var);

      /* List dimensions for variable */
      for(int idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){

        /* Get dimension name */
        (void)nco_inq_dim(grp_id,dmn_id_var[idx_var_dim],dmn_nm,&dmn_sz);

        if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
          /* Add all possible coordinate variables traversing file */
          (void)nco_aux_add_dmn_trv2(nc_id,dmn_nm,trv_tbl);
        }else{
          /* Construct the full (dimension/variable) name */
          char* dm_nm_fll=(char*)nco_malloc(strlen(trv.grp_nm_fll)+strlen(dmn_nm)+2);
          strcpy(dm_nm_fll,trv.grp_nm_fll);
          if(strcmp(trv.grp_nm_fll,"/") !=0 ) strcat(dm_nm_fll,"/");
          strcat(dm_nm_fll,dmn_nm); 
          (void)trv_tbl_mrk(dm_nm_fll,trv_tbl);
          /* Free allocated */
          dm_nm_fll=(char *)nco_free(dm_nm_fll);
        }

      } /* End loop over idx_var_dim: list dimensions for variable */
    } /* end nco_obj_typ_var */
  } /* end uidx  */

  return;
} /* end nco_var_lst_crd_ass_add_trv2 */


void
nco_get_prg_info()                   /* [fnc] Get program info */
{
  int ret=10;
#ifndef HAVE_NETCDF4_H 
  ret=20;
#else /* HAVE_NETCDF4_H */
#ifdef ENABLE_NETCDF4 
  ret=30;
#else /* HAVE_NETCDF4_H */
  ret=40;
#endif /* ENABLE_NETCDF4 */
#endif /* HAVE_NETCDF4_H */

  (void)fprintf(stdout,"%s: INFO: %d\n",prg_nm_get(),ret);
  exit(ret);
} /* end nco_get_prg_info() */

void
nco_grp_var_mk_trv2                    /* [fnc] Define OR write groups/variables and group attributes in output file */
(const int nc_id,                      /* I [ID] netCDF input file ID  */
 const int nc_out_id,                  /* I [ID] netCDF output file ID  */
 const gpe_sct * const gpe,            /* I [sng] GPE structure */
 const int lmt_nbr,                    /* I [nbr] Number of dimensions with limits */
 lmt_all_sct * const * lmt_all_lst,    /* I multi-hyperslab limits */
 const int lmt_all_lst_nbr,            /* I [nbr] Number of hyperslab limits */
 const int dfl_lvl,                    /* I [enm] Deflate level [0..9] */
 nco_bool PRN_VAR_METADATA,            /* I [flg] Copy variable metadata (attributes) */
 int * const cnk_map_ptr,              /* I [enm] Chunking map */
 int * const cnk_plc_ptr,              /* I [enm] Chunking policy */
 const size_t cnk_sz_scl,              /* I [nbr] Chunk size scalar */
 CST_X_PTR_CST_PTR_CST_Y(cnk_sct,cnk), /* I [sct] Chunking information */
 const int cnk_nbr,                    /* I [nbr] Number of dimensions with user-specified chunking */
 FILE * const fp_bnr,                  /* I [fl] Unformatted binary output file handle */
 const nco_bool MD5_DIGEST,            /* I [flg] Perform MD5 digests */
 const nco_bool NCO_BNR_WRT,           /* I [flg] Write binary file */
 const nco_bool DEF_MODE,              /* I [flg] netCDF define mode is true */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{
  /* Purpose: Define OR write groups/variables and group attributes in output file */

  char *rec_dmn_nm;              /* [sng] Record dimension name */
  char *var_nm_fll;              /* [sng] Full path of variable */
  char *grp_out_fll;             /* [sng] Group name */
  char dmn_ult_nm[NC_MAX_NAME+1];/* [sng] Unlimited dimension name */ 
  char var_nm[NC_MAX_NAME+1];    /* [sng] Variable name */ 

  gpe_nm_sct *gpe_nm;            /* [sct] GPE name duplicate check array  */

  int *var_ids;                  /* [ID]  Variable IDs array */

  int dmn_ids_ult[NC_MAX_DIMS];  /* [ID]  Unlimited dimensions IDs array */

  int fl_fmt;                    /* [enm] netCDF file format */
  int grp_id;                    /* [ID]  Group ID in input file */
  int grp_out_id;                /* [ID]  Group ID in output file */ 
  int nbr_att;                   /* [nbr] Number of attributes for group */
  int nbr_dmn;                   /* [nbr] Number of dimensions for group */
  int nbr_dmn_ult;               /* [nbr] Number of unlimited dimensions */
  int nbr_gpe_nm;                /* [nbr] Number of GPE entries */
  int nbr_grp;                   /* [nbr] Number of groups for group */
  int nbr_var;                   /* [nbr] Number of variables for group */
  int var_out_id;                /* [ID]  Variable ID in output file */

  nbr_gpe_nm=0;
  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    /* Object is marked to export */
    if(trv_tbl->lst[uidx].flg == True){

#ifdef NCO_SANITY_CHECK
      assert(trv.typ == nco_obj_typ_var);
#endif

      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.grp_nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=nc_id;
      }

      /* Edit group name */
      if(gpe) grp_out_fll=nco_gpe_evl(gpe,trv.grp_nm_fll); else grp_out_fll=(char *)strdup(trv.grp_nm_fll);

      /* Obtain info for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
      (void)nco_inq_grps(grp_id,&nbr_grp,(int *)NULL);

      /* Allocate space for and obtain variable IDs in current group */
      var_ids=(int *)nco_malloc(nbr_var*sizeof(int));
      (void)nco_inq_varids(grp_id,&nbr_var,var_ids);

      /* Get number of unlimited dimensions */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      rec_dmn_nm=NULL;

      /* Get variables for this group */
      for(int idx_var=0;idx_var<nbr_var;idx_var++){
        var_nm_fll=NULL;

        /* Get name of current variable in current group NOTE: using obtained IDs array */
        (void)nco_inq_varname(grp_id,var_ids[idx_var],var_nm);

        /* Allocate path buffer; add space for a trailing NUL */ 
        var_nm_fll=(char*)nco_malloc(strlen(trv.grp_nm_fll)+strlen(var_nm)+2L);

        /* Initialize path with the current absolute group path */
        strcpy(var_nm_fll,trv.grp_nm_fll);

        /* If not root group, concatenate separator */
        if(strcmp(trv.grp_nm_fll,"/")) strcat(var_nm_fll,"/");

        /* Concatenate variable to absolute group path */
        strcat(var_nm_fll,var_nm);

        /* Search for record dimension name */
        for(int idx_dmn=0;idx_dmn<nbr_dmn_ult;idx_dmn++){
          (void)nco_inq_dimname(grp_id,dmn_ids_ult[idx_dmn],dmn_ult_nm);
          if(!strcmp(var_nm,dmn_ult_nm)){
            rec_dmn_nm=(char *)nco_malloc((NC_MAX_NAME+1L)*sizeof(char));
            strcpy(rec_dmn_nm,dmn_ult_nm);  
          } /* strcmp */
        } /* idx_dmn */  

        /* Memory management after current variable */
        var_nm_fll=(char *)nco_free(var_nm_fll);

      } /* end get variables for this group */  

      /* If output group does not exist, create it */
      if(nco_inq_grp_full_ncid_flg(nc_out_id,grp_out_fll,&grp_out_id)) nco_def_grp_full(nc_out_id,grp_out_fll,&grp_out_id);

      /* Detect GPE duplicate names */
      char *gpe_var_nm_fll=NULL;  
      if(gpe && DEF_MODE){
        /* Construct the absolute GPE variable path */
        gpe_var_nm_fll=(char*)nco_malloc(strlen(grp_out_fll)+strlen(trv.nm)+2L);
        strcpy(gpe_var_nm_fll,grp_out_fll);
        /* If not root group, concatenate separator */
        if(strcmp(grp_out_fll,"/")) strcat(gpe_var_nm_fll,"/");
        strcat(gpe_var_nm_fll,trv.nm);

        /* GPE name is not already on the list, put it there */
        if(nbr_gpe_nm == 0){
          gpe_nm=(gpe_nm_sct *)nco_malloc((nbr_gpe_nm+1)*sizeof(gpe_nm_sct)); 
          gpe_nm[nbr_gpe_nm].var_nm_fll=strdup(gpe_var_nm_fll);
          nbr_gpe_nm++;
        }else{
          /* GPE might be already on the list, put it there only if not found */
          for(int idx_gpe=0;idx_gpe<nbr_gpe_nm;idx_gpe++){
            if(!strcmp(gpe_var_nm_fll,gpe_nm[idx_gpe].var_nm_fll)){
              (void)fprintf(stdout,"%s: ERROR nco_grp_var_mk_trv() reports variable %s already defined. HINT: Moving groups of flattening files can lead to over-determined situations where a single object name (e.g., a variable name) must refer to multiple objects in the same output group. The user's intent is ambiguous so instead of arbitrarily picking which (e.g., the last) variable of that name to place in the output file, NCO simply fails. User should re-try command after ensuring multiple objects of the same name will not be placed in the same group.\n",prg_nm_get(),gpe_var_nm_fll);
              for(int idx=0;idx<nbr_gpe_nm;idx++)
                gpe_nm[idx].var_nm_fll=(char *)nco_free(gpe_nm[idx].var_nm_fll);
              nco_exit(EXIT_FAILURE);
            } /* End string comparison */
          } /* End search in array */
          gpe_nm=(gpe_nm_sct *)nco_realloc((void *)gpe_nm,(nbr_gpe_nm+1)*sizeof(gpe_nm_sct));
          gpe_nm[nbr_gpe_nm].var_nm_fll=strdup(gpe_var_nm_fll);
          nbr_gpe_nm++;
        } /* End might be on list */
      }/* End GPE */

      /* Free full path name */
      if(gpe_var_nm_fll) gpe_var_nm_fll=(char *)nco_free(gpe_var_nm_fll);

      /* Define mode: create variables */
      if(DEF_MODE){

        /* Define variable */
        if(lmt_nbr > 0) var_out_id=nco_cpy_var_dfn_lmt(grp_id,grp_out_id,rec_dmn_nm,trv.nm,lmt_all_lst,lmt_all_lst_nbr,dfl_lvl); 
        else var_out_id=nco_cpy_var_dfn(grp_id,grp_out_id,rec_dmn_nm,trv.nm,dfl_lvl);

        /* Set chunksize parameters */
        if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC) {
          (void)nco_cnk_sz_set(grp_out_id,lmt_all_lst,lmt_all_lst_nbr,cnk_map_ptr,cnk_plc_ptr,cnk_sz_scl,cnk,cnk_nbr);
        }

        /* Copy variable's attributes */
        if(PRN_VAR_METADATA){
          int var_id;
          (void)nco_inq_varid(grp_id,trv_tbl->lst[uidx].nm,&var_id);
          (void)nco_att_cpy(grp_id,grp_out_id,var_id,var_out_id,(nco_bool)True);
        }

      }else{ /* Write mode */

        /* Write output variable */
        if(lmt_nbr > 0) (void)nco_cpy_var_val_mlt_lmt(grp_id,grp_out_id,fp_bnr,MD5_DIGEST,NCO_BNR_WRT,trv.nm,lmt_all_lst,lmt_all_lst_nbr); 
        else (void)nco_cpy_var_val(grp_id,grp_out_id,fp_bnr,MD5_DIGEST,NCO_BNR_WRT,trv.nm);

      } /* Define mode */

      /* Memory management */
      if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm);

      /* If there are group attributes, write them, avoid root case, these are always copied elsewhere */
      if(nbr_att && strcmp("/",trv.grp_nm_fll) != 0){

        /* Obtain group ID from netCDF API using full group name (NOTE: using GPE grp_out_fll name to write ) */
        nco_inq_grp_full_ncid(nc_out_id,grp_out_fll,&grp_out_id);
        /* Copy global attributes */
        (void)nco_att_cpy(grp_id,grp_out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
      } /* End nbr_att */

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);
      if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

    } /* end marked to export */
  } /* end uidx  */

  /* Memory management for GPE names */
  for(int idx=0;idx<nbr_gpe_nm;idx++)
    gpe_nm[idx].var_nm_fll=(char *)nco_free(gpe_nm[idx].var_nm_fll);

} /* end nco_grp_var_mk_trv2() */


void
nco_prn_var_def_trv2                  /* [fnc] Print variable metadata (called with PRN_VAR_METADATA) */
(const int nc_id,                     /* I netCDF file ID */
 const trv_tbl_sct * const trv_tbl)   /* I [sct] Traversal table */
{ 
  int grp_id;                  /* [ID] Group ID */
  int var_id;                  /* [ID] Variable ID */
  int fl_fmt;                  /* [nbr] File format */

  /* Get file format */
  (void)nco_inq_format(nc_id,&fl_fmt);

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_sct trv=trv_tbl->lst[uidx];
    /* Object is marked to export */
    if(trv_tbl->lst[uidx].flg == True){

#ifdef NCO_SANITY_CHECK
      assert(trv.typ == nco_obj_typ_var);
#endif

      /* Obtain group ID from netCDF API using full group name */
      if(fl_fmt == NC_FORMAT_NETCDF4 || fl_fmt == NC_FORMAT_NETCDF4_CLASSIC){
        (void)nco_inq_grp_full_ncid(nc_id,trv.grp_nm_fll,&grp_id);
      }else{ /* netCDF3 case */
        grp_id=nc_id;
      }
      /* Obtain variable ID from netCDF API using group ID */
      (void)nco_inq_varid(grp_id,trv.nm,&var_id);

      /* Print full name of variable */
      (void)fprintf(stdout,"%s\n",trv.nm_fll);
      /* Print variable's definition using the obtained grp_id instead of the netCDF file ID; Voila  */
      (void)nco_prn_var_dfn(grp_id,trv.nm); 
      /* Print variable's attributes */
      (void)nco_prn_att(nc_id,grp_id,var_id);
    } /* end  marked to export */
  } /* end uidx  */

  return;
} /* end nco_prn_var_def_trv2() */