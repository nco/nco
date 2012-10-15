/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.c,v 1.153 2012-10-15 21:17:19 pvicente Exp $ */

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
#include "nco_cnk.h"      /* Chunking; needed for nco4_grp_lst_mk */
#include "nco_msa.h"      /* Multi-slabbing algorithm: needed for nco_cpy_var_val_mlt_lmt */

#define GRP_DEV           /* Symbol that encapsulates group development code; just keeps old non-group code commented in #else */

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


nm_id_sct * /* O [sct] Variable extraction list */
nco4_var_lst_mk /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id, /* I [ID] Apex group ID */
 int * const nbr_var_fl, /* O [nbr] Number of variables in input file */
 char * const * const var_lst_in, /* I [sng] User-specified list of variable names and rx's */
 const nco_bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const var_xtr_nbr, /* I/O [nbr] Number of variables in current extraction list */
 int * const grp_xtr_nbr,  /* I/O [nbr] Number of groups in current extraction list (specified with -g ) */
 char * const * const grp_lst_in, /* I [sng] User-specified list of groups names to extract (specified with -g ) */
 grp_tbl_sct *trv_tbl)  /* I   [sct] Group traversal table  */
{
  /* Purpose: Create variable extraction list with or without regular expressions */

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
  int grp_idx;
  int grp_nbr; /* [nbr] Number of groups in input file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_idx;
  int var_idx_crr; /* [idx] Total variable index */
  int var_nbr; /* [nbr] Number of variables in current group */
  int var_nbr_all; /* [nbr] Number of variables in input file */
  int var_nbr_tbl;   /* Number of variables in table list (table list stores all paths, groups and variables ) */
  nco_bool var_prn;  /* Variable to print or not (variable exists only in var_lst_all )*/
  int var_nbr_tmp;
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */

  var_idx_crr=0; /* Incremented at cycle end */

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

      /* Append all variables in current group to variable list */
      for(var_idx=0;var_idx<var_nbr;var_idx++){

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
        var_lst_all[var_idx_crr].grp_nm_fll=grp_nm_fll_sls;

        /* Increment number of variables */
        var_idx_crr++;

        if(dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"var_nm=%s, var_nm_fll=%s\n",var_nm,var_nm_fll);

        /* Full variable name has been duplicated, re-terminate with NUL for next variable */
        *var_nm_fll_sls_ptr='\0'; /* [ptr] Pointer to first character following last slash */
      } /* end loop over var_idx */

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);
      grp_nm_fll=(char *)nco_free(grp_nm_fll);
      var_nm_fll=(char *)nco_free(var_nm_fll);

    } /* endif current group has variables */

  } /* end loop over grp */

  /* Store results prior to first return */
  *nbr_var_fl=var_nbr_all; /* O [nbr] Number of variables in input file */

#ifdef NCO_SANITY_CHECK
  var_nbr_tbl=0; /* Number of variables in table list (table list stores all paths, groups and variables ) */
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->grp_lst[uidx].typ == nc_typ_var) var_nbr_tbl++; 
  }
  assert(var_nbr_tbl == var_nbr_all);
  var_prn=True; /* Variable to print or not (variable exists only in var_lst_all )*/
  var_idx_crr=0;
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if(var_idx_crr == var_nbr_all) break;
    grp_trv_sct trv=trv_tbl->grp_lst[uidx];
    nm_id_sct nm_id=var_lst_all[var_idx_crr]; 
    if(var_prn == True){
      var_prn=False;
    } /* end var_prn */
    /* Increment var_lst_all index only when table object is a variable; this keeps the 2 lists in sync */
    if (trv_tbl->grp_lst[uidx].typ == nc_typ_var){
      /* Match 2 lists */
      assert(strcmp(nm_id.nm,trv.nm)==0);
      assert(strcmp(nm_id.var_nm_fll,trv.nm_fll)==0);
      var_idx_crr++; 
      var_prn=True;
    } /* end nc_typ_var */
  }/* end uidx */
#endif /* NCO_SANITY_CHECK */

  /* Return all variables if none were specified and not -c ... */
#ifdef GRP_DEV
  if(*var_xtr_nbr == 0 && *grp_xtr_nbr == 0 && !EXTRACT_ALL_COORDINATES){
    *var_xtr_nbr=var_nbr_all;
    return var_lst_all;
  } /* end if */

  /* Initialize and allocate extraction flag array to all False */
  var_xtr_rqs=(nco_bool *)nco_calloc((size_t)var_nbr_all,sizeof(nco_bool));

  /* Create variable extraction list using regular expressions:  
  2 cases to deal with extraction with supplied -v (variable) and -g (group) names: 

  Case 1) -v was specified:  
  Do a loop cycle to iterate all objects found in the file. 
  For each variable found, do a second loop through the user-specified variable list.
  Add the variable to the extraction list if a name found in the second loop matches.
  This achieves printing all the variables.
  NOTE: the -g case is handled here too, if present, add only the variable if present in the group.

  Case 2) -v was not specified but -g was: traverse the -g list first and add all variables from that group
  */
  grp_nm_fll=NULL;
  var_nm_fll=NULL;
  if (*var_xtr_nbr) {
    var_prn=True; /* Variable to print or not (variable exists only in var_lst_all )*/
    var_idx_crr=0;
    for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
      if(var_idx_crr == var_nbr_all) break;    
      if(var_prn == True) {
        int var_id;
        strcpy(grp_nm,var_lst_all[var_idx_crr].grp_nm);
        var_nm_fll=(char *)strdup(var_lst_all[var_idx_crr].var_nm_fll);
        strcpy(var_nm,var_lst_all[var_idx_crr].nm);
        var_id=var_lst_all[var_idx_crr].id;
        grp_id=var_lst_all[var_idx_crr].grp_id;
        grp_nm_fll=(char *)strdup(var_lst_all[var_idx_crr].grp_nm_fll);

        /* Loop through user-specified variable list */
        for(int idx=0;idx<*var_xtr_nbr;idx++){
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
            rx_mch_nbr=nco_lst_rx_search(var_nbr_all,var_lst_all,var_sng,var_xtr_rqs);
            if(rx_mch_nbr == 0) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any variables\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),var_sng); 
            continue;
#else
            (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),var_sng);
            nco_exit(EXIT_FAILURE);
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
          } /* end if regular expression */


          /* Compare var_nm from main iteration with var_sng found; if the same add to extraction list at index IDX  */
          if(strcmp(var_sng,var_nm) == 0 ){
            /* No groups case, just add  */
            if (*grp_xtr_nbr == 0 ){
              var_xtr_rqs[var_idx_crr]=True;
              /* Groups -g case, add only if current group name GRP_NM matches any of the supplied GRP_LST_IN names */
            }else{
              /* Loop through user-specified group list */
              for(grp_idx=0;grp_idx<*grp_xtr_nbr;grp_idx++){
                char grp_nm_lst[NC_MAX_NAME]; /* Group name from the supplied argument list */
                /* Get current group name */
                strcpy(grp_nm_lst,grp_lst_in[grp_idx]);
                /* Find group specified in -g and mark any match for inclusion to extract */
                if(strcmp(grp_nm,grp_nm_lst) == 0){
                  var_xtr_rqs[var_idx_crr]=True;
                } /* end strcmp */
              } /* end grp_idx */
            } /* end else */
            if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout," grp_nm_fll=%s\n var_nm_fll=%s\n grp_nm=%s grp_id=%d var_nm=%s var_id=%d\n",grp_nm_fll,var_nm_fll,grp_nm,grp_id,var_nm,var_id);
          }  /* end strcmp */

        } /* end loop over var_lst_in idx */ 

        var_prn=False;

      } /* end var_prn == True */

      /* Increment var index for var_lst_all only when table object is a variable; this keeps the 2 lists in sync */
      if (trv_tbl->grp_lst[uidx].typ == nc_typ_var){
        var_idx_crr++; 
        var_prn=True;
        /* Full variable names between the 2 lists must be the same */
#ifdef NCO_SANITY_CHECK
        assert(strcmp(var_nm_fll,trv_tbl->grp_lst[uidx].nm_fll) == 0);
#endif
        grp_nm_fll=(char *)nco_free(grp_nm_fll);
        var_nm_fll=(char *)nco_free(var_nm_fll);
      } /* end nc_typ_var */
    }/* end loop over trv_tbl uidx */
  } /* end *var_xtr_nbr>0 */

  /* Case 2) 
  -v was not specified but -g was: traverse the -g list first and add all variables from that group 
  */
  else if (*grp_xtr_nbr) {  
#ifdef NCO_SANITY_CHECK
    /* var_idx_crr was used to traverse all groups and variables to make var_lst_all */
    assert(var_nbr_all == var_idx_crr);
#endif
    /* Loop through user-specified group list */
    for(grp_idx=0;grp_idx<*grp_xtr_nbr;grp_idx++){
      /* Get current group name */
      strcpy(grp_nm,grp_lst_in[grp_idx]);
      /* Loop through found variable list */
      for(var_idx_crr=0;var_idx_crr<var_nbr_all;var_idx_crr++){
        nm_id_sct nm_id=var_lst_all[var_idx_crr];   
        /* Find group specified in -g and mark any match for inclusion to extract */
        if(strcmp(nm_id.grp_nm,grp_nm)==0){
          var_xtr_rqs[var_idx_crr]=True;
        } /* end strcmp */
      } /* end var_idx_crr */
    } /* end grp_idx */
  } /* end *grp_xtr_nbr */


#else /* GRP_DEV */ 

  int jdx;
  if(*var_xtr_nbr == 0 && !EXTRACT_ALL_COORDINATES){
    *var_xtr_nbr=var_nbr_all;
    return var_lst_all;
  } /* end if */

  /* Initialize and allocate extraction flag array to all False */
  var_xtr_rqs=(nco_bool *)nco_calloc((size_t)var_nbr_all,sizeof(nco_bool));

  /* Loop through user-specified variable list */
  for(idx=0;idx<*var_xtr_nbr;idx++){
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
      rx_mch_nbr=nco_lst_rx_search(var_nbr_all,var_lst_all,var_sng,var_xtr_rqs);
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

#endif /* GRP_DEV */

  /* Create final variable list using boolean flag array */

  /* malloc() xtr_lst to maximium size(var_nbr_all) */
  xtr_lst=(nm_id_sct *)nco_malloc(var_nbr_all*sizeof(nm_id_sct));
  var_nbr_tmp=0; /* var_nbr_tmp is incremented */
  for(int idx=0;idx<var_nbr_all;idx++){
    /* Copy variable to extraction list */
    if(var_xtr_rqs[idx]){
      xtr_lst[var_nbr_tmp].grp_nm=(char *)strdup(var_lst_all[idx].grp_nm);
      xtr_lst[var_nbr_tmp].var_nm_fll=(char *)strdup(var_lst_all[idx].var_nm_fll);
      xtr_lst[var_nbr_tmp].nm=(char *)strdup(var_lst_all[idx].nm);
      xtr_lst[var_nbr_tmp].id=var_lst_all[idx].id;
      xtr_lst[var_nbr_tmp].grp_id=var_lst_all[idx].grp_id;
      xtr_lst[var_nbr_tmp].grp_nm_fll=(char *)strdup(var_lst_all[idx].grp_nm_fll);
      var_nbr_tmp++;
    } /* end if */
  } /* end loop over var */

  /* re-alloc() list to actual size */  
  xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,var_nbr_tmp*sizeof(nm_id_sct));

  var_lst_all=(nm_id_sct *)nco_nm_id_lst_free(var_lst_all,var_nbr_all);
  var_xtr_rqs=(nco_bool *)nco_free(var_xtr_rqs);

  /* Store values for return */
  *var_xtr_nbr=var_nbr_tmp;  

  if(dbg_lvl_get() >= nco_dbg_var){
    (void)fprintf(stdout,"%s: INFO nco4_var_lst_mk() reports following %d variable%s matched sub-setting and regular expressions:\n",prg_nm_get(),*var_xtr_nbr,(*var_xtr_nbr > 1) ? "s" : "");
    prt_xtr_lst(xtr_lst,*var_xtr_nbr);
  } /* endif dbg */

  return xtr_lst;
} /* end nco4_var_lst_mk() */

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
    while(*grp_sng){
      if(*grp_sng == '#') *grp_sng=',';
      grp_sng++;
    } /* end while */
    grp_sng=grp_lst_in[idx];
    
    /* If grp_sng is regular expression ... */
    if(strpbrk(grp_sng,".*^$\\[]()<>+?|{}")){
      /* ... and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
      rx_mch_nbr=nco_lst_rx_search(grp_nbr_top,grp_lst_all,grp_sng,grp_xtr_rqs);
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

  *grp_xtr_nbr=grp_nbr_tmp;    
  return grp_lst;
} /* end nco_grp_lst_mk() */




int                            /* [rcd] Number of sub-groups */
nco_has_subgrps
(const int nc_id)              /* I [ID] NetCDF file ID */  
{
  /* Purpose: 
     Return a bool value telling if the netCDF file has groups other than root 
  */
  int ngrps;           
  (void)nco_inq_grps(nc_id,&ngrps,NULL);
  return ngrps;
} /* nco_has_subgrps() */

void 
prt_xtr_lst           /*   [fnc] Print Name ID structure list */
(nm_id_sct *xtr_lst,  /* I [sct] Name ID structure list */
 int xtr_nbr)         /* I [nbr] Name ID structure list size */
{
  for(int idx=0;idx<xtr_nbr;idx++){
    nm_id_sct nm_id=xtr_lst[idx];
    (void)fprintf(stdout,"nm=%s var_nm_fll=%s grp_nm_fll=%s grp_nm=%s grp_id=(%d) id=(%d)\n",
      nm_id.nm, nm_id.var_nm_fll, nm_id.grp_nm_fll, nm_id.grp_nm, nm_id.grp_id, nm_id.id);
  }
}/* end prt_xtr_lst() */

nm_id_sct *             /* O [sct] Extraction list */
nco4_var_lst_xcl        /* [fnc] Convert exclusion list to extraction list */
(const int nc_id,       /* I [ID] netCDF file ID */
 const int nbr_var,     /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst,    /* I/O [sct] Current exclusion list (destroyed) */
 int * const xtr_nbr,   /* I/O [nbr] Number of variables in exclusion/extraction list */
 grp_tbl_sct *trv_tbl)  /* I [sct] Group traversal table  */
{
  /* Purpose: Convert exclusion list to extraction list
     User wants to extract all variables except those currently in list
     It is hard to edit existing list so copy existing extraction list into 
     exclusion list, then construct new extraction list from scratch. */

  int nbr_var_xtr;   /* Number of variables to extract */
  int nbr_var_tbl;   /* Number of variables in the table */
  int idx;
  int nbr_xcl;
  unsigned int uidx;
#ifndef GRP_DEV
  int var_in_id;
  int lst_idx;
  char var_nm[NC_MAX_NAME];
  nm_id_sct *xcl_lst;
#endif

  if(dbg_lvl_get() >= nco_dbg_vrb){
    (void)fprintf(stdout,"%s: INFO nco4_var_lst_xcl() reports following %d variable%s to be excluded:\n",prg_nm_get(),*xtr_nbr,(*xtr_nbr > 1) ? "s" : "");
    prt_xtr_lst(xtr_lst,*xtr_nbr);
  } /* endif dbg */
 
#ifdef GRP_DEV
  /* Traverse the full list trv_tbl; if a name in xtr_lst (input extraction list) is found, mark it as flagged;
  A second traversal extracts all variables that are not marked (this reverses the list);
  The second traversal is needed because we need to find nbr_xcl, the number of variables to exclude, first
  */
  nbr_var_xtr=0;
  nbr_var_tbl=0;
  for(uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->grp_lst[uidx].typ == nc_typ_var){ /* trv_tbl lists non-variables also; filter just variables */
      nbr_var_tbl++;

      grp_trv_sct trv=trv_tbl->grp_lst[uidx];

      for(idx=0;idx<*xtr_nbr;idx++){

        /* Compare variable name between full list and input extraction list */
        if(strcmp(xtr_lst[idx].var_nm_fll,trv.nm_fll) == 0){

          trv_tbl->grp_lst[uidx].flg=1;
          nbr_var_xtr++;

          if(dbg_lvl_get() >= nco_dbg_vrb){
            (void)fprintf(stdout,"idx = %d, nm = %s, var_nm_fll = %s\n",idx,xtr_lst[idx].nm,xtr_lst[idx].var_nm_fll);
          } /* endif dbg */
        } /* endif strcmp */
      } /* end idx */
    } /* end nc_typ_var */
  } /* end loop over uidx */

#ifdef NCO_SANITY_CHECK
  assert(nbr_var_tbl == nbr_var);
#endif
  nbr_xcl=nbr_var-nbr_var_xtr;

  /* Second traversal: extracts all variables that are not marked (this reverses the list); the xtr_lst must be reconstructed for:
  1) grp_nm_fll (full group name)
  2) grp_id (group ID) 
  */

  xtr_lst=(nm_id_sct *)nco_free(xtr_lst);
  xtr_lst=(nm_id_sct *)nco_malloc(nbr_xcl*sizeof(nm_id_sct));

  /* Initialize index of extracted variables */
  *xtr_nbr=0;

  for(uidx=0,idx=0;uidx<trv_tbl->nbr;uidx++){
    grp_trv_sct trv=trv_tbl->grp_lst[uidx];
    if (trv.typ == nc_typ_var && trv.flg != 1 ){ /* trv_tbl lists non-variables also; filter just variables */

      /* Extract the full group name from 'trv', that contains the full variable name, to xtr_lst */
      (void)nco4_xtr_grp_nm_fll(nc_id,xtr_lst,xtr_nbr,trv);

      /* Increment index of extracted variables */
      ++*xtr_nbr;
    }
  } /* end loop over uidx */


#else /* GRP_DEV */
  /* Turn extract list into exclude list and reallocate extract list  */
  nbr_xcl=*xtr_nbr;
  *xtr_nbr=0;
  xcl_lst=(nm_id_sct *)nco_malloc(nbr_xcl*sizeof(nm_id_sct));
  (void)memcpy((void *)xcl_lst,(void *)xtr_lst,nbr_xcl*sizeof(nm_id_sct));
  xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(nbr_var-nbr_xcl)*sizeof(nm_id_sct));

  for(idx=0;idx<nbr_var;idx++){
    /* Get name and ID of variable */
    (void)nco_inq_varname(nc_id,idx,var_nm);
    for(lst_idx=0;lst_idx<nbr_xcl;lst_idx++){
      if(idx == xcl_lst[lst_idx].id) break;
    } /* end loop over lst_idx */
    /* If variable is not in exclusion list then add it to new list */
    if(lst_idx == nbr_xcl){
      xtr_lst[*xtr_nbr].nm=(char *)strdup(var_nm);
      xtr_lst[*xtr_nbr].id=idx;
      ++*xtr_nbr;
    } /* end if */
  } /* end loop over idx */

  /* Free memory for names in exclude list before losing pointers to names */
  /* NB: cannot free memory if list points to names in argv[] */
  /* for(idx=0;idx<nbr_xcl;idx++) xcl_lst[idx].nm=(char *)nco_free(xcl_lst[idx].nm);*/
  xcl_lst=(nm_id_sct *)nco_free(xcl_lst);
#endif /* GRP_DEV */

#ifdef NCO_SANITY_CHECK
  assert(*xtr_nbr == nbr_xcl);
#endif

  if(dbg_lvl_get() >= nco_dbg_vrb){
    (void)fprintf(stdout,"%s: INFO nco4_var_lst_xcl() reports following %d variable%s to be extracted:\n",prg_nm_get(),*xtr_nbr,(*xtr_nbr > 1) ? "s" : "");
    prt_xtr_lst(xtr_lst,*xtr_nbr);
  } /* endif dbg */

  /* Reset mark field */
  for(uidx=0;uidx<trv_tbl->nbr;uidx++){
    trv_tbl->grp_lst[uidx].flg=-1;
  }

  return xtr_lst;
} /* end nco4_var_lst_xcl() */



void
nco4_xtr_grp_nm_fll     /* [fnc] Auxiliary function; extract full group name from a grp_trv_sct to a nm_id_sct */
(const int nc_id,       /* I [ID] netCDF file ID */
 nm_id_sct *xtr_lst,    /* I/O [sct] Current exclusion list */
 int * const xtr_nbr,   /* I [nbr] Current index in exclusion/extraction list */
 grp_trv_sct trv)       /* I [sct] Group traversal table entry */
{
  /* Purpose: 
     Extract the full group name from a grp_trv_sct entry that contains the full variable name 
     to a nm_id_sct struct. 
  */
  char *pch;        /* Pointer to the last occurrence of character */
  int   pos;        /* Position of character */
  char *grp_nm_fll; /* Fully qualified group where variable resides */
  int  grp_id;      /* Group ID */
  int  var_id;      /* Variable ID */
  int  len;         /* Lenght of fully qualified group where variable resides */

  if (trv.typ == nc_typ_var){

    len=strlen(trv.nm_fll);
    grp_nm_fll=(char*)nco_malloc((len+1L)*sizeof(char));
    strcpy(grp_nm_fll,trv.nm_fll);

    /* Find last occurence of '/' to form group full name */
    pch=strrchr(grp_nm_fll,'/');

#ifdef NCO_SANITY_CHECK
    /* trv.nm_fll must have a '/'  */
    assert(pch != NULL);
#endif

    /* Trim the variable name */
    pos=pch-grp_nm_fll+1;
    grp_nm_fll[pos]='\0';

    /* Obtain group ID from netCDF API using full group name */
    (void)nco_inq_grp_full_ncid(nc_id,grp_nm_fll,&grp_id);

    /* Obtain variable ID from netCDF API using group ID */
    (void)nco_inq_varid(grp_id,trv.nm,&var_id);

    /* ncks needs only:
    1) xtr_lst.grp_nm_fll (full group name where variable resides, to get group ID) 
    done with nco_inq_grp_full_ncid(in_id,nm_id.grp_nm_fll,..)
    2) xtr_lst.var_nm_fll
    3) xtr_lst.id
    4) xtr_lst.nm (relative variable name) 
    NOTE: 
    1) xtr_lst.grp_id is stored for validation
    2) xtr_lst.grp_nm is not used (stored here as grp_nm_fll)
    */
    xtr_lst[*xtr_nbr].nm=(char*)strdup(trv.nm);
    xtr_lst[*xtr_nbr].grp_nm_fll=(char*)strdup(grp_nm_fll);
    xtr_lst[*xtr_nbr].var_nm_fll=(char*)strdup(trv.nm_fll);
    xtr_lst[*xtr_nbr].id=var_id;
    xtr_lst[*xtr_nbr].grp_nm=(char*)strdup(grp_nm_fll);
#ifdef NCO_SANITY_CHECK
    xtr_lst[*xtr_nbr].grp_id=grp_id;
#endif
    /* Free allocated memory */
    grp_nm_fll=(char*)nco_free(grp_nm_fll);
  }else if (trv.typ == nc_typ_grp){
#ifdef NCO_SANITY_CHECK
    /* No business being here  */
    assert(0);
#endif
  } /* nc_typ_var */

  return;
} /* end nco4_xtr_grp_nm_fll() */

char*           /* O [sng] Return substring */
get_lst_nm      /* [fnc] Strip last component of full name */
(char *nm_in)   /* I [sng] Full name; it contains '/' as last character */
{
  int len;  /* Input string lenght */
  char *cp; 

  len=strlen(nm_in);
  if(len == 0) return NULL; 
  cp=(char*)(nm_in+len);      /* Point to the NULL ending the string */
  cp--;                      /* Back up one character */
  cp--;                      /* Back up one character; avoid the last '/' */
  while(cp != nm_in)         
  { 
    if('/' == *cp){          /* Found path separator */
      break;                 /* End traversal */
    }
    cp--;                    /* Traverse from end */
  }

  return cp+1;               /* Return component without leading '/' */
}/* end get_lst_nm() */


int                              /* O [rcd] Return code, bool */
nco4_xtr_grp_nm                  /* [fnc] Auxiliary function; extract group name from a grp_trv_sct */
(const int nc_id,                /* I [ID] netCDF file ID */
 int * const grp_xtr_nbr,        /* I [nbr] Number of groups in current extraction list (specified with -g ) */
 char * const * const grp_lst_in,/* I [sng] User-specified list of groups names to extract (specified with -g ) */
 grp_trv_sct trv)                /* I [sct] Group traversal table entry */
{
  /* Purpose: 
     Extract the group name from a grp_trv_sct entry that contains the full path 
  */
  char *pch;                    /* Pointer to character in string */
  int   pos;                    /* Position of character */
  char *nm_fll;                 /* Path */
  int  grp_id;                  /* Group ID */
  int  len_fll;                 /* Lenght of fully qualified group where variable resides */
  char grp_nm_lst[NC_MAX_NAME]; /* Group name from the supplied argument list */
  int  fnd=0;                   /* Return value; name was found or not */

  len_fll=strlen(trv.nm_fll);
  nm_fll=(char *)nco_malloc((len_fll+1L)*sizeof(char));
  strcpy(nm_fll,trv.nm_fll);

  /* Find last occurence of '/' */
  pch=strrchr(nm_fll,'/');

#ifdef NCO_SANITY_CHECK
  /* trv.nm_fll must have a '/'  */
  assert(pch != NULL);
#endif

  /* Trim the variable name */
  pos=pch-nm_fll+1;
  nm_fll[pos]='\0';

  /* Obtain group ID from netCDF API using full group name */
#ifdef NCO_SANITY_CHECK
  (void)nco_inq_grp_full_ncid(nc_id,nm_fll,&grp_id);
#endif

  /* Loop through user-specified group list */
  for(int grp_idx=0;grp_idx<*grp_xtr_nbr;grp_idx++){

    /* Get group name from -g */
    strcpy(grp_nm_lst,grp_lst_in[grp_idx]);

    /* Locate group name from -g in traversal name */
    pch=strstr(nm_fll,grp_nm_lst);

    /* strstr returns the first occurrence of 'grp_nm_lst' in 'nm_fll', the higher level group( closer to root) */
    if(pch != NULL){    
      fnd=1;
    } /* end pch was found */
  } /* end loop user-specified group list */

  /* Free allocated memory */
  nm_fll=(char *)nco_free(nm_fll);
  return fnd;
} /* end nco4_xtr_grp_nm() */


int                            /* [rcd] Return code */
nco4_grp_lst_mk_itr            /* [fnc] Iterator function for nco4_grp_lst_mk */
(const int in_id,              /* I [ID] Group ID from netCDF intput file */
 const int out_id,             /* I [ID] Group ID from netCDF output file */
 char * const grp_pth,         /* I [sng] Group path */
 char * const grp_nm,          /* I [sng] Group name */
 nm_id_sct * const xtr_lst,    /* I [sct] Extraction list  */
 const int xtr_nbr,            /* I [nbr] Number of members in extraction list */
 const int lmt_nbr,            /* I [nbr] Number of dimensions with limits */
 CST_X_PTR_CST_PTR_CST_Y(lmt_all_sct,lmt_all_lst), /* I [sct] Hyperslab limits */
 const int lmt_all_lst_nbr,    /* I [nbr] Number of hyperslab limits */
 const int dfl_lvl,            /* I [enm] Deflate level [0..9] */
 nco_bool PRN_VAR_METADATA,    /* I [flg] Copy variable metadata (attributes) */
 nco_bool PRN_GLB_METADATA)    /* I [flg] Copy global variable metadata (attributes) */
{
  /* Purpose: Recursively iterate in_id, creating groups and defining variables in out_id */

  int rcd=NC_NOERR;            /* O [rcd] Return code */
  nc_type var_typ;             /* O [enm] Variable type */
  int nbr_att;                 /* O [nbr] Number of attributes */
  int nbr_var;                 /* O [nbr] Number of variables */
  int nbr_dmn;                 /* O [nbr] number of dimensions */
  int nbr_grp;                 /* O [nbr] Number of sub-groups in this group */
  int rec_dmn_id;              /* O [ID]  Record dimension ID */
  char gp_nm[NC_MAX_NAME+1];   /* O [sng] Group name */
  char var_nm[NC_MAX_NAME+1];  /* O [sng] Variable name */ 
  int *grp_ids;                /* O [ID]  Sub-group IDs */ 
  int *dmn_ids;                /* O [ID]  Dimension IDs */
  int grp_out_id;              /* O [ID]  Group ID */ 
  int idx;                     /* I [idx] Index */
  int nbr_dmn_ult;             /* O [nbr] Number of unlimited dimensions */
  int dmn_ids_ult[NC_MAX_DIMS];/* O [nbr] Unlimited dimensions IDs array */
  char dmn_ult_nm[NC_MAX_NAME+1];  /* O [sng] Unlimited dimension name */ 

  /* Get all information for this group */
  rcd+=nco_inq_nvars(in_id,&nbr_var);
  rcd+=nco_inq_grpname(in_id,gp_nm);
  rcd+=nco_inq_ndims(in_id,&nbr_dmn);
  rcd+=nco_inq_natts(in_id,&nbr_att);
  rcd+=nco_inq_grps(in_id,&nbr_grp,NULL);
  rcd+=nco_inq(in_id,&nbr_dmn,&nbr_var,&nbr_att,&rec_dmn_id);
  dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
  rcd+=nco_inq_dimids(in_id,&nbr_dmn,dmn_ids,0);
  rcd+=nco_inq_unlimdims(in_id,&nbr_dmn_ult,dmn_ids_ult);

  /* Avoid the root case */
  grp_out_id = out_id;

  /* No need to create root :) ; if NOT root, define the group in the ouput file */
  if (strcmp("/",grp_nm) != 0){

    if(nbr_var == 0 && nbr_dmn == 0 && nbr_att == 0 && nbr_grp == 0 ){
      if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_grp_lst_mk_itr() empty group: %s\n",prg_nm_get(),grp_nm);
    }
    else

    /* Define group of same name in output file */
    rcd+=nco_def_grp(out_id,grp_nm,&grp_out_id);
  }

  /* Copy global attributes (in group) NOTE: use grp_out_id obtained */
  if(PRN_GLB_METADATA && nbr_att){
    (void)nco_att_cpy(in_id,grp_out_id,NC_GLOBAL,NC_GLOBAL,(nco_bool)True);
  }

  /* Get variables for this group */
  for(int var_id=0;var_id<nbr_var;var_id++){
    char *var_pth=NULL; /* Full path of variable */
    rcd+=nco_inq_var(in_id,var_id,var_nm,&var_typ,NULL,NULL,&nbr_att);

    /* Allocate path buffer; add space for a trailing NULL */ 
    var_pth=(char*)nco_malloc(strlen(grp_pth)+strlen(var_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(var_pth,grp_pth);
    if(strcmp(grp_pth,"/")!=0) /* If not root group, concatenate separator */
      strcat(var_pth,"/");
    strcat(var_pth,var_nm); /* Concatenate variable to absolute group path */

    if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_grp_lst_mk_itr() variable: %s\n",prg_nm_get(),var_pth);

    /* Check if input variable is on extraction list; if yes, write it to output file */
    for(idx=0;idx<xtr_nbr;idx++){
      int var_out_id;

      /* If current variable is in extraction list, define it */
      if(strcmp(var_pth,xtr_lst[idx].var_nm_fll) == 0) { 
        char *rec_dmn_nm=NULL; /* [sng] Record dimension name */

        if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_grp_lst_mk_itr()  extract: %s\n",prg_nm_get(),var_pth);

        for(int ndx=0;ndx<nbr_dmn_ult;ndx++){
          rcd+=nco_inq_dimname(in_id,dmn_ids_ult[ndx],dmn_ult_nm);
          if(strcmp(var_nm,dmn_ult_nm) == 0 ){
            rec_dmn_nm=(char *)nco_malloc(NC_MAX_NAME*(sizeof(char)));
            strcpy(rec_dmn_nm,dmn_ult_nm);
            if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_grp_lst_mk_itr()  record dimension: %s\n",prg_nm_get(),rec_dmn_nm);
          }
        }       

        /* Define variable in output file: NOTE: use grp_out_id obtained */
        if(lmt_nbr > 0) var_out_id=nco_cpy_var_dfn_lmt(in_id,grp_out_id,rec_dmn_nm,xtr_lst[idx].nm,lmt_all_lst,lmt_all_lst_nbr,dfl_lvl); 
        else var_out_id=nco_cpy_var_dfn(in_id,grp_out_id,rec_dmn_nm,xtr_lst[idx].nm,dfl_lvl);

        /* Copy variable's attributes */
        if(PRN_VAR_METADATA) (void)nco_att_cpy(in_id,grp_out_id,xtr_lst[idx].id,var_out_id,(nco_bool)True);

        if(rec_dmn_nm) rec_dmn_nm=(char *)nco_free(rec_dmn_nm);

        /* Variable was found, exit idx loop, there can be one and only one variable */
        break;
      } /* end variable in extraction list */ 
    } /* end loop over idx */

    var_pth=(char*)nco_free(var_pth);
  }

  /* Go to sub-groups */ 
  grp_ids=(int*)nco_malloc((nbr_grp)*sizeof(int));
  rcd+=nco_inq_grps(in_id,&nbr_grp,grp_ids);

  for(idx=0;idx<nbr_grp;idx++){
    char *pth=NULL;  /* Full group path */
    int gid=grp_ids[idx];
    rcd+=nco_inq_grpname(gid,gp_nm);

    /* Allocate path buffer; add space for a trailing NUL */ 
    pth=(char*)nco_malloc(strlen(grp_pth)+strlen(gp_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(pth,grp_pth);
    if(strcmp(grp_pth,"/")!=0) /* If not root group, concatenate separator */
      strcat(pth,"/");
    strcat(pth,gp_nm); /* Concatenate current group to absolute group path */

    /* Recursively go to sub-groups */
    rcd+=nco4_grp_lst_mk_itr(gid,grp_out_id,pth,gp_nm,xtr_lst,xtr_nbr,lmt_nbr,lmt_all_lst,lmt_all_lst_nbr,dfl_lvl,PRN_VAR_METADATA,PRN_GLB_METADATA);

    pth=(char*)nco_free(pth);
  }

  (void)nco_free(grp_ids);
  (void)nco_free(dmn_ids);
  return rcd;
}/* end nco4_grp_lst_mk_itr() */

void
nco4_grp_lst_mk                  /* [fnc] Create groups/variables in output file */
(const int in_id,                /* I [ID] netCDF input file ID */
 const int out_id,               /* I [ID] netCDF output file ID */
 nm_id_sct * const xtr_lst,      /* I [sct] Extraction list  */
 const int xtr_nbr,              /* I [nbr] Number of members in list */
 const int lmt_nbr,              /* I [nbr] Number of dimensions with limits */
 CST_X_PTR_CST_PTR_CST_Y(lmt_all_sct,lmt_all_lst), /* I [sct] Hyperslab limits */
 const int lmt_all_lst_nbr,      /* I [nbr] Number of hyperslab limits */
 const int dfl_lvl,              /* I [enm] Deflate level [0..9] */
 nco_bool PRN_VAR_METADATA,      /* I [flg] Copy variable metadata (attributes) */
 nco_bool PRN_GLB_METADATA)      /* I [flg] Copy global variable metadata (attributes) */
{
  /* Purpose: 
     Recursively iterate input file (nc_id) and generate groups/define variables in output file (out_id) 
  */
  int rcd=NC_NOERR;              /* I [rcd] Return code */
  char rth[2];
  strcpy(rth,"/");

  if(dbg_lvl_get() >= nco_dbg_vrb){
    (void)fprintf(stdout,"%s: INFO nco4_grp_lst_mk() reports following %d variable%s to define:\n",prg_nm_get(),xtr_nbr,(xtr_nbr > 1) ? "s" : "");
    prt_xtr_lst(xtr_lst,xtr_nbr);
  } /* endif dbg */

  /* Recursively go to sub-groups, starting with netCDF file ID and root group name */
  rcd+=nco4_grp_lst_mk_itr(in_id,out_id,rth,rth,xtr_lst,xtr_nbr,lmt_nbr,lmt_all_lst,lmt_all_lst_nbr,dfl_lvl,PRN_VAR_METADATA,PRN_GLB_METADATA);

  return;
} /* end nco4_grp_lst_mk() */


int                              /* [rcd] Return code */
nco4_grp_var_cpy_itr             /* [fnc] Iterator function for nco4_grp_var_cpy */
(const int in_id,                /* I [ID] netCDF input file ID */
 const int out_id,               /* I [ID] netCDF output file ID */
 char * const grp_nm_fll,        /* I [sng] Group path */
 char * const grp_nm,            /* I [sng] Group name */
 nm_id_sct * const xtr_lst,      /* I [sct] Extraction list  */
 const int xtr_nbr,              /* I [nbr] Number of members in list */
 const int lmt_nbr,              /* I [nbr] Number of dimensions with limits */
 lmt_all_sct * const * lmt_all_lst,  /* I multi-hyperslab limits */
 const int lmt_all_lst_nbr,      /* I [nbr] Number of hyperslab limits */
 FILE * const fp_bnr,            /* I [fl] Unformatted binary output file handle */
 const nco_bool MD5_DIGEST,      /* I [flg] Perform MD5 digests */
 const nco_bool NCO_BNR_WRT)     /* I [flg] Write binary file */
{
  /* Purpose: Recursively iterate in_id, writing variables in out_id */

  int rcd=NC_NOERR;            /* O [rcd] Return code */
  nc_type var_typ;             /* O [enm] Variable type */
  int nbr_att;                 /* O [nbr] Number of attributes */
  int nbr_var;                 /* O [nbr] Number of variables */
  int nbr_dmn;                 /* O [nbr] number of dimensions */
  int nbr_grp;                 /* O [nbr] Number of sub-groups in this group */
  int rec_dmn_id;              /* O [ID] Record dimension ID */
  char gp_nm[NC_MAX_NAME+1];   /* O [sng] Group name */
  char var_nm[NC_MAX_NAME+1];  /* O [sng] Variable name */ 
  int *grp_ids;                /* O [ID]  Sub-group IDs */ 
  int *dmn_ids;                /* O [ID]  Dimension IDs */
  int grp_out_id;              /* O [ID]  Group ID */ 
  int idx;                     /* I [idx] Index */

  /* Get all information for this group */
  rcd+=nco_inq_nvars(in_id,&nbr_var);
  rcd+=nco_inq_grpname(in_id,gp_nm);
  rcd+=nco_inq_ndims(in_id,&nbr_dmn);
  rcd+=nco_inq_natts(in_id,&nbr_att);
  rcd+=nco_inq_grps(in_id,&nbr_grp,NULL);
  rcd+=nco_inq(in_id,&nbr_dmn,&nbr_var,&nbr_att,&rec_dmn_id);
  dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
  rcd+=nco_inq_dimids(in_id,&nbr_dmn,dmn_ids,0);

  /* Avoid the root case */
  grp_out_id = out_id;

  /* Avoid the root case */ 
  if (strcmp("/",grp_nm)) {

    if(nbr_var == 0 && nbr_dmn == 0 && nbr_att == 0 && nbr_grp == 0 ){
      if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_grp_var_cpy_itr() empty group: %s\n",prg_nm_get(),grp_nm);
    }
    else

      /* Obtain group ID from netCDF API using group name */
      nco_inq_grp_ncid(out_id,grp_nm,&grp_out_id);
  }

  /* Get variables for this group */
  for(int var_id=0;var_id<nbr_var;var_id++){
    char *var_pth=NULL; /* Full path of variable */
    rcd+=nco_inq_var(in_id,var_id,var_nm,&var_typ,NULL,NULL,&nbr_att);

    /* Allocate path buffer; add space for a trailing NULL */ 
    var_pth=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(var_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(var_pth,grp_nm_fll);
    if(strcmp(grp_nm_fll,"/")!=0) /* If not root group, concatenate separator */
      strcat(var_pth,"/");
    strcat(var_pth,var_nm); /* Concatenate variable to absolute group path */

    if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_grp_var_cpy_itr() variable: %s\n",prg_nm_get(),var_pth);

    /* Check if input variable is on extraction list; if yes, write it to output file */
    for(idx=0;idx<xtr_nbr;idx++){

      /* If current variable is in extraction list */
      if(strcmp(var_pth,xtr_lst[idx].var_nm_fll) == 0) { 

        if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_grp_var_cpy_itr()  extract: %s\n",prg_nm_get(),var_pth);

        /* Write output variable; NOTE: use grp_out_id obtained  */
        if(lmt_nbr > 0) (void)nco_cpy_var_val_mlt_lmt(in_id,grp_out_id,fp_bnr,MD5_DIGEST,NCO_BNR_WRT,xtr_lst[idx].nm,lmt_all_lst,lmt_all_lst_nbr); 
        else (void)nco_cpy_var_val(in_id,grp_out_id,fp_bnr,MD5_DIGEST,NCO_BNR_WRT,xtr_lst[idx].nm);

        /* Variable was found, exit idx loop, there can be one and only one variable */
        break;
      } /* end variable in extraction list */ 
    } /* end loop over idx */

    var_pth=(char*)nco_free(var_pth);
  }

  /* Go to sub-groups */ 
  grp_ids=(int*)nco_malloc((nbr_grp)*sizeof(int));
  rcd+=nco_inq_grps(in_id,&nbr_grp,grp_ids);

  for(idx=0;idx<nbr_grp;idx++){
    char *pth=NULL;  /* Full group path */
    int gid=grp_ids[idx];
    rcd+=nco_inq_grpname(gid,gp_nm);

    /* Allocate path buffer; add space for a trailing NUL */ 
    pth=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(gp_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(pth,grp_nm_fll);
    if(strcmp(grp_nm_fll,"/")!=0) /* If not root group, concatenate separator */
      strcat(pth,"/");
    strcat(pth,gp_nm); /* Concatenate current group to absolute group path */

    /* Recursively go to sub-groups */
    rcd+=nco4_grp_var_cpy_itr(gid,grp_out_id,pth,gp_nm,xtr_lst,xtr_nbr,lmt_nbr,lmt_all_lst,lmt_all_lst_nbr,fp_bnr,MD5_DIGEST,NCO_BNR_WRT);

    pth=(char*)nco_free(pth);
  }

  (void)nco_free(grp_ids);
  (void)nco_free(dmn_ids);
  return rcd;
}/* end nco4_grp_var_itr() */

void
nco4_grp_var_cpy                 /* [fnc] Write variables in output file (copy from input file)  */
(const int in_id,                /* I [ID] netCDF input file ID */
 const int out_id,               /* I [ID] netCDF output file ID */
 nm_id_sct * const xtr_lst,      /* I [sct] Extraction list  */
 const int xtr_nbr,              /* I [nbr] Number of members in list */
 const int lmt_nbr,              /* I [nbr] Number of dimensions with limits */
 lmt_all_sct * const * lmt_all_lst,  /* I multi-hyperslab limits */
 const int lmt_all_lst_nbr,      /* I [nbr] Number of hyperslab limits */
 FILE * const fp_bnr,            /* I [fl] Unformatted binary output file handle */
 const nco_bool MD5_DIGEST,      /* I [flg] Perform MD5 digests */
 const nco_bool NCO_BNR_WRT)     /* I [flg] Write binary file */
{
  /* Purpose: 
     Recursively iterate input file (nc_id) and write variables in output file (out_id) 
  */
  char rth[2];
  strcpy(rth,"/");

  if(dbg_lvl_get() >= nco_dbg_vrb){
    (void)fprintf(stdout,"%s: INFO nco4_grp_lst_mk() reports following %d variable%s to write:\n",prg_nm_get(),xtr_nbr,(xtr_nbr > 1) ? "s" : "");
    prt_xtr_lst(xtr_lst,xtr_nbr);
  } /* endif dbg */

  /* Recursively go to sub-groups, starting with netCDF file ID and root group name */
  (void)nco4_grp_var_cpy_itr(in_id,out_id,rth,rth,xtr_lst,xtr_nbr,lmt_nbr,lmt_all_lst,lmt_all_lst_nbr,fp_bnr,MD5_DIGEST,NCO_BNR_WRT);

  return;
} /* end nco4_grp_var_cpy() */



void
nco4_xtr_lst_add           /* [fnc] Auxiliary function; add an entry to xtr_lst */
(char * const var_nm,      /* I [sng] Variable name */
 char * const var_nm_fll,  /* I [sng] Full variable name */
 char * const grp_nm_fll,  /* I [sng] Full group name */
 char * const grp_nm,      /* I [sng] Group name */
 int const var_id,         /* I [ID] Variable ID */
 int const grp_id,         /* I [ID] Group ID */ 
 nm_id_sct *xtr_lst,       /* I/O [sct] Current list */
 int * xtr_nbr)            /* I/O [nbr] Current index in exclusion/extraction list */
{

  /* Count mode */
  if (xtr_lst == NULL){ 
    ++*xtr_nbr;
    return;
  }

  /* ncks needs only:
  1) xtr_lst.grp_nm_fll (full group name where variable resides, to get group ID) 
  done with nco_inq_grp_full_ncid(in_id,nm_id.grp_nm_fll,..)
  2) xtr_lst.var_nm_fll
  3) xtr_lst.id
  4) xtr_lst.nm (relative variable name) 
  NOTE: 
  1) xtr_lst.grp_id is stored for validation
  2) xtr_lst.grp_nm is not used 
  */
  xtr_lst[*xtr_nbr].nm=(char*)strdup(var_nm);
  xtr_lst[*xtr_nbr].grp_nm_fll=(char*)strdup(grp_nm_fll);
  xtr_lst[*xtr_nbr].var_nm_fll=(char*)strdup(var_nm_fll);
  xtr_lst[*xtr_nbr].id=var_id;
  xtr_lst[*xtr_nbr].grp_nm=(char*)strdup(grp_nm);
  xtr_lst[*xtr_nbr].grp_id=grp_id;

  ++*xtr_nbr;

  return;
} /* end nco4_xtr_lst_add() */


int                            /* [rcd] Return code */
nco_grp_itr
(const int grp_id,             /* I [ID] Group ID */
 char * grp_nm_fll,            /* I [sng] Absolute group name (path) */
 grp_tbl_sct *trv_tbl)         /* I/O [sct] Traversal table */
{
  /* Purpose: Recursively iterate grp_id */

  int rcd=NC_NOERR;            /* O [rcd] Return code */
  nc_type var_typ;             /* O [enm] Variable type */
  int nbr_att;                 /* O [nbr] Number of attributes */
  int nbr_var;                 /* O [nbr] Number of variables */
  int nbr_dmn;                 /* O [nbr] number of dimensions */
  int nbr_grp;                 /* O [nbr] Number of sub-groups in this group */
  int var_id;                  /* O [ID] Variable ID */ 
  int rec_dmn_id;              /* O [ID] Record dimension ID */
  char gp_nm[NC_MAX_NAME+1];   /* O [sng] Group name */
  char var_nm[NC_MAX_NAME+1];  /* O [sng] Variable name */ 
  int *grp_ids;                /* O [ID]  Sub-group IDs */ 
  int *dmn_ids;                /* O [ID]  Dimension IDs */ 
  int idx;                     /* I [idx] Index */             
  grp_trv_sct obj;             /* O [obj] netCDF4 object, as having a path and a type */

  /* Get all information for this group */
  rcd+=nco_inq_nvars(grp_id,&nbr_var);
  rcd+=nco_inq_grpname(grp_id,gp_nm);
  rcd+=nco_inq_ndims(grp_id,&nbr_dmn);
  rcd+=nco_inq_natts(grp_id,&nbr_att);
  rcd+=nco_inq_grps(grp_id,&nbr_grp,NULL);
  rcd+=nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,&rec_dmn_id);
  dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
  rcd+=nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,0);

  /* Add to table: this is a group */
  obj.nm_fll=grp_nm_fll;
  strcpy(obj.nm,gp_nm);
  obj.typ=nc_typ_grp;
  obj.nbr_att=nbr_att;
  obj.nbr_var=nbr_var;
  obj.nbr_dmn=nbr_dmn;
  obj.nbr_grp=nbr_grp;
  trv_tbl_add(obj,trv_tbl);

  /* Iterate variables for this group */
  for(var_id=0;var_id<nbr_var;var_id++){
    char *var_pth=NULL; /* Full path of variable */
    rcd+=nco_inq_var(grp_id,var_id,var_nm,&var_typ,&nbr_dmn,NULL,&nbr_att);

    /* Allocate path buffer; add space for a trailing NUL */ 
    var_pth=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(var_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(var_pth,grp_nm_fll);
    if(strcmp(grp_nm_fll,"/")!=0) /* If not root group, concatenate separator */
      strcat(var_pth,"/");
    strcat(var_pth,var_nm); /* Concatenate variable to absolute group path */

    /* Add to table: this is a variable NOTE: nbr_var, nbr_grp not valid here */
    obj.nm_fll=var_pth;
    obj.typ=nc_typ_var;
    strcpy(obj.nm,var_nm);
    obj.nbr_att=nbr_att;
    obj.nbr_dmn=nbr_dmn;
    obj.nbr_var=-1;
    obj.nbr_grp=-1;
    trv_tbl_add(obj,trv_tbl);
    var_pth=(char*)nco_free(var_pth);
  }

  /* Go to sub-groups */ 
  grp_ids=(int*)nco_malloc((nbr_grp)*sizeof(int));
  rcd+=nco_inq_grps(grp_id,&nbr_grp,grp_ids);

  for(idx=0;idx<nbr_grp;idx++){
    char *pth=NULL;  /* Full group path */
    int gid=grp_ids[idx];
    rcd+=nco_inq_grpname(gid,gp_nm);

    /* Allocate path buffer; add space for a trailing NUL */ 
    pth=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(gp_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(pth,grp_nm_fll);
    if(strcmp(grp_nm_fll,"/")!=0) /* If not root group, concatenate separator */
      strcat(pth,"/");
    strcat(pth,gp_nm); /* Concatenate current group to absolute group path */

    /* Recursively go to sub-groups; NOTE the new absolute group name is passed  */
    rcd+=nco_grp_itr(gid,pth,trv_tbl);

    pth=(char*)nco_free(pth);
  }

  (void)nco_free(grp_ids);
  (void)nco_free(dmn_ids);
  return rcd;
}/* end nco_grp_itr() */

void 
nco4_msa_lmt_all_int            /* [fnc] Initilaize lmt_all_sct's; netCDF4 group recursive version */ 
(int in_id,                     /* [ID]  netCDF file ID */
 nco_bool MSA_USR_RDR,          /* [flg] Multi-Slab Algorithm returns hyperslabs in user-specified order */
 lmt_all_sct **lmt_all_lst,     /* [sct] List of *lmt_all_sct structures */
 int nbr_dmn_fl,                /* I [nbr] Number of dimensions in file */
 lmt_sct **lmt,                 /* [sct] Limits of the current hyperslab */
 int lmt_nbr,                   /* I [nbr] Number of limit structures in list */
 grp_tbl_sct *trv_tbl)          /* I [sct] Traversal table */
{
  lmt_sct *lmt_rgl;
  lmt_all_sct * lmt_all_crr;
  int nbr_dmn_all;             /* [nbr] Total number of dimensions in file */
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  long dmn_sz;                 /* [nbr] Dimension size */ 
  int *dmn_ids;                /* [ID]  Dimension IDs */ 
  int grp_id;                  /* [ID] Group ID */
  int nbr_att;                 /* [nbr] Number of attributes */
  int nbr_var;                 /* [nbr] Number of variables */
  int nbr_dmn;                 /* [nbr] number of dimensions */
  int nbr_dmn_ult;             /* [nbr] Number of unlimited dimensions */
  int dmn_ids_ult[NC_MAX_DIMS];/* [nbr] Unlimited dimensions IDs array */
  int idx;                     /* [idx] Global index for lmt_all_lst */
  int jdx;
  
#ifdef GRP_DEV
  /* Initialize counters/indices */
  nbr_dmn_all=0;
  idx=0;
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    grp_trv_sct trv=trv_tbl->grp_lst[uidx];
    if (trv.typ == nc_typ_grp ) {

      /* Obtain group ID from netCDF API using full group name */
      (void)nco_inq_grp_full_ncid(in_id,trv.nm_fll,&grp_id);

      /* Obtain unlimited dimensions for group: NOTE using group ID */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      /* Obtain dimensions for group: NOTE using group ID */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif

      dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,0);

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

    } /* end nc_typ_grp */
  } /* end uidx  */

#else /* GRP_DEV */ 

  int idx;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  long dmn_sz;
  char dmn_nm[NC_MAX_NAME];
  

  (void)nco_inq(in_id,(int*)NULL,(int*)NULL,(int *)NULL,&rec_dmn_id);

  for(idx=0;idx<nbr_dmn_fl;idx++){
    (void)nco_inq_dim(in_id,idx,dmn_nm,&dmn_sz);
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
    lmt_rgl->id=idx;

    /* NB: nco_lmt_evl() may alter this */
    if(idx==rec_dmn_id) lmt_rgl->is_rec_dmn=True; else lmt_rgl->is_rec_dmn=False;

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
  } /* end loop over dimensions */

#endif /* GRP_DEV */ 

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


void                      
nco4_inq_dmn               /* [fnc] Find and return global totals of dimensions */
(int nc_id,                /* I [ID]  netCDF file ID */
 int * const dmn_nbr_all,  /* O [nbr] Number of dimensions in file */
 grp_tbl_sct *trv_tbl)     /* I [sct] Traversal table */
{
  int nbr_dmn_all=0;       /* [nbr] Total number of dimensions in file */
  char dmn_nm[NC_MAX_NAME];/* [sng] Dimension name */ 
  long dmn_sz;             /* [nbr] Dimension size */ 
  int *dmn_ids;            /* [ID]  Dimension IDs */ 
  int grp_id;              /* [ID]  Group ID */
  int nbr_att;             /* [nbr] Number of attributes */
  int nbr_var;             /* [nbr] Number of variables */
  int nbr_dmn;             /* [nbr] Number of dimensions */

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    grp_trv_sct trv=trv_tbl->grp_lst[uidx]; 
    if (trv.typ == nc_typ_grp ) {    
      
      if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"INFO nco4_inq_dmn() reports: %s: %d subgroups, %d dimensions, %d attributes, %d variables\n",trv.nm_fll,trv.nbr_grp,trv.nbr_dmn,trv.nbr_att,trv.nbr_var); 

      /* Obtain group ID from netCDF API using full group name */
      (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);

#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif

      dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,0);

      /* List dimensions using obtained group ID */
      for(int jdx=0;jdx<trv.nbr_dmn;jdx++){
        (void)nco_inq_dim(grp_id,dmn_ids[jdx],dmn_nm,&dmn_sz);
        if(dbg_lvl_get() >= nco_dbg_vrb) (void)fprintf(stdout,"dimension: %s (%ld)\n",dmn_nm,dmn_sz);
      } /* end jdx dimensions */
      (void)nco_free(dmn_ids);

      /* Increment total number of dimensions in file */
      nbr_dmn_all+=trv.nbr_dmn;

    } /* end nc_typ_grp */
  } /* end uidx  */

  /* Export */
  *dmn_nbr_all=nbr_dmn_all;

  if(dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO nco4_inq_dmn() reports file contains %d dimension%s\n",prg_nm_get(),*dmn_nbr_all,(*dmn_nbr_all > 1) ? "s" : "");

  return;
} /* end nco4_inq_dmn() */

void                       
nco4_inq_trv              /* [fnc] Find and return global totals of dimensions, variables, attributes */
(int * const att_nbr_glb, /* O [nbr] Number of global attributes in file */
 int * const dmn_nbr_all, /* O [nbr] Number of dimensions in file */
 int * const var_nbr_all, /* O [nbr] Number of variables in file  */
 int * const grp_nbr_all, /* O [nbr] Number of groups in file */
 grp_tbl_sct *trv_tbl)    /* I [sct] Traversal table */
{
  /* [fnc] Find and return global totals of dimensions, variables, attributes */

  /* Initialize */
  *att_nbr_glb=0;
  *dmn_nbr_all=0;
  *var_nbr_all=0;
  *grp_nbr_all=0;

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    grp_trv_sct trv=trv_tbl->grp_lst[uidx]; 
    if (trv.typ == nc_typ_grp ) { 

      /* Increment/Export */
      *att_nbr_glb+=trv.nbr_att; /* attributes in groups are global */
      *dmn_nbr_all+=trv.nbr_dmn;
      *var_nbr_all+=trv.nbr_var;
      *grp_nbr_all+=trv.nbr_grp;
    } /* end nc_typ_grp */
  } /* end uidx  */

  if(dbg_lvl_get() >= nco_dbg_fl){
    (void)fprintf(stdout,"%s: INFO nco4_inq_trv() reports file contains %d group%s comprising %d variable%s, %d dimension%s, and %d global attribute%s\n",
      prg_nm_get(),*grp_nbr_all,(*grp_nbr_all > 1) ? "s" : "",*var_nbr_all,(*var_nbr_all > 1) ? "s" : "",*dmn_nbr_all,(*dmn_nbr_all > 1) ? "s" : "",*att_nbr_glb,(*att_nbr_glb > 1) ? "s" : "");
  }

  return;
} /* end nco4_inq_trv() */


int                       /* [rcd] Return code */
nco4_inq_vars             /* [fnc] Find and return total of variables */
(const int nc_id,         /* I [ID] Apex group */
 int * const var_nbr_all) /* O [nbr] Number of variables in file */
{
  /* [fnc] Find and return total variables */

  int rcd=NC_NOERR;
  int *grp_ids; /* [ID] Group IDs of children */
  int grp_id;   /* [ID] Group ID */
  int grp_idx;
  int grp_nbr;  /* [nbr] Number of groups */
  int var_nbr;  /* [nbr] Number of variables */

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
 
  return rcd;
} /* end nco4_inq() */

void                          
nco_prt_trv             /* [fnc] Print table with -z */
(grp_tbl_sct *trv_tbl)  /* I [sct] Traversal table */
{
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    grp_trv_sct trv=trv_tbl->grp_lst[uidx];
    if (trv.typ == nc_typ_grp ) {
      (void)fprintf(stdout,"grp: ");
    } else if (trv.typ == nc_typ_var ) {
      (void)fprintf(stdout,"var: ");
    }
    (void)fprintf(stdout,"%s\n",trv_tbl->grp_lst[uidx].nm_fll); 
  } /* end uidx */
} /* end nco_prt_trv() */

void                          
nco_prt_grp_trv         /* [fnc] Print table with -G */
(const int nc_id,       /* I [ID] File ID */
 grp_tbl_sct *trv_tbl)  /* I [sct] Traversal table */
{
  (void)fprintf(stderr,"%s: INFO reports group information\n",prg_nm_get());
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    if (trv_tbl->grp_lst[uidx].typ == nc_typ_grp ) {
      grp_trv_sct trv=trv_tbl->grp_lst[uidx];            
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

      /* Obtain group ID from netCDF API using full group name */
      (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);

      /* Obtain unlimited dimensions for group */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      /* Obtain number of dimensions for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);

      /* Obtain dimensions IDs for group */
      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,0);

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
    } /* end nc_typ_grp */
  } /* end uidx  */
} /* end nco_prt_grp_trv() */


void
nco_lmt_evl_trv            /* [fnc] Parse user-specified limits into hyperslab specifications */
(int nc_id,                /* I [idx] netCDF file ID */
 int lmt_nbr,              /* [nbr] Number of user-specified dimension limits */
 lmt_sct **lmt,            /* I/O [sct] Structure from nco_lmt_prs() or from nco_lmt_sct_mk() to hold dimension limit information */
 nco_bool FORTRAN_IDX_CNV, /* I [flg] Hyperslab indices obey Fortran convention */
 grp_tbl_sct *trv_tbl)     /* I [sct] Traversal table */
{
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  long dmn_sz;                 /* [nbr] Dimension size */ 
  int *dmn_ids;                /* [ID]  Dimension IDs */ 
  int grp_id;                  /* [ID]  Group ID */
  int nbr_att;                 /* [nbr] Number of attributes */
  int nbr_var;                 /* [nbr] Number of variables */
  int nbr_dmn;                 /* [nbr] Number of dimensions */

  if(lmt_nbr){
    for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
      if (trv_tbl->grp_lst[uidx].typ == nc_typ_grp ) {
        grp_trv_sct trv=trv_tbl->grp_lst[uidx];           

        /* Obtain group ID from netCDF API using full group name */
        (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);         

        /* Obtain number of dimensions in group */
        (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
#ifdef NCO_SANITY_CHECK
        assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif
        dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
        (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,0);

        /* List dimensions using obtained group ID */
        for(int idx=0;idx<trv.nbr_dmn;idx++){
          (void)nco_inq_dim(grp_id,dmn_ids[idx],dmn_nm,&dmn_sz);

          if(dbg_lvl_get() >= nco_dbg_dev)(void)fprintf(stdout,"dimension: %s (%ld)\n",dmn_nm,dmn_sz);

          /* NOTE: using obtained group ID */
          for(int kdx=0;kdx<lmt_nbr;kdx++) {
            if(strcmp(lmt[kdx]->nm,dmn_nm) == 0 ){
              (void)nco_lmt_evl(grp_id,lmt[kdx],0L,FORTRAN_IDX_CNV);
              if(dbg_lvl_get() >= nco_dbg_dev)(void)fprintf(stdout,"dimension limit: %s (%ld)\n",lmt[kdx]->nm,lmt[kdx]->cnt);
#ifdef NCO_SANITY_CHECK
              assert(lmt[kdx]->id == dmn_ids[idx]);
#endif
            } /* end if */
          } /* end kdx */
        } /* end idx dimensions */

        (void)nco_free(dmn_ids);
      } /* end nc_typ_grp */
    } /* end uidx  */
  } /* end lmt_nbr */
} /* end nco_lmt_evl_trv() */



void                          
nco_chk_trv                         /* [fnc] Check if input names of -v or -g are in file */
(const int nc_id,                   /* I [ID] Apex group ID */
 int * const nbr_var_fl,            /* I [nbr] Number of variables in input file */
 char * const * const var_lst_in,   /* I [sng] User-specified list of variable names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 int * const var_xtr_nbr)           /* I [nbr] Number of variables in current extraction list */
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
  int grp_idx;
  int grp_nbr; /* [nbr] Number of groups in input file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_idx;
  int var_idx_crr; /* [idx] Total variable index */
  int var_nbr; /* [nbr] Number of variables in current group */
  int var_nbr_all; /* [nbr] Number of variables in input file */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */

  var_idx_crr=0; /* Incremented at cycle end */

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

      /* Append all variables in current group to variable list */
      for(var_idx=0;var_idx<var_nbr;var_idx++){

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
        var_lst_all[var_idx_crr].grp_nm_fll=grp_nm_fll_sls;

        /* Increment number of variables */
        var_idx_crr++;

        /* Full variable name has been duplicated, re-terminate with NUL for next variable */
        *var_nm_fll_sls_ptr='\0'; /* [ptr] Pointer to first character following last slash */
      } /* end loop over var_idx */

      /* Memory management after current group */
      var_ids=(int *)nco_free(var_ids);
      grp_nm_fll=(char *)nco_free(grp_nm_fll);
      var_nm_fll=(char *)nco_free(var_nm_fll);

    } /* endif current group has variables */

  } /* end loop over grp */

  /* Store results prior to first return */
  *nbr_var_fl=var_nbr_all; /* O [nbr] Number of variables in input file */

  /* Initialize and allocate extraction flag array to all False */
  var_xtr_rqs=(nco_bool *)nco_calloc((size_t)var_nbr_all,sizeof(nco_bool));

  /* Loop through user-specified variable list */
  for(int idx=0;idx<*var_xtr_nbr;idx++){
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
      rx_mch_nbr=nco_lst_rx_search(var_nbr_all,var_lst_all,var_sng,var_xtr_rqs);
      if(rx_mch_nbr == 0) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any variables\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),var_sng); 
      continue;
#else
      (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),var_sng);
      nco_exit(EXIT_FAILURE);
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
    } /* end if regular expression */


    /* Normal variable so search through variable array */
    int jdx;
    for(jdx=0;jdx<var_nbr_all;jdx++)
      if(!strcmp(var_sng,var_lst_all[jdx].nm)) {
        break;
      }

      /* Mark any match as requested for inclusion by user */
      if(jdx != var_nbr_all){
        var_xtr_rqs[jdx]=True;
      }else{
        if(EXCLUDE_INPUT_LIST){ 
          /* Variable need not be present if list will be excluded later ... */
          if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_chk_trv() reports explicitly excluded variable \"%s\" is not in input file anyway\n",prg_nm_get(),var_sng); 
        }else{ /* !EXCLUDE_INPUT_LIST */
          /* Variable should be included but no matches found so die */
          (void)fprintf(stdout,"%s: ERROR nco_chk_trv() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),var_sng); 
          nco_exit(EXIT_FAILURE);
        } /* !EXCLUDE_INPUT_LIST */
      } /* end else */

  } /* end loop over var_lst_in idx */ 

  var_lst_all=(nm_id_sct *)nco_nm_id_lst_free(var_lst_all,var_nbr_all);
  var_xtr_rqs=(nco_bool *)nco_free(var_xtr_rqs);

} /* end nco4_var_lst_mk() */


void 
nco_prn_att_trv               /* [fnc] Print all attributes of single variable */
(const int nc_id,             /* I [id] netCDF file ID */
 grp_tbl_sct *trv_tbl)        /* I [sct] Traversal table */
{
  int grp_id;                 /* [ID]  Group ID */
  int nbr_att;                /* [nbr] Number of attributes */
  int nbr_var;                /* [nbr] Number of variables */
  int nbr_dmn;                /* [nbr] Number of dimensions */

  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    grp_trv_sct trv=trv_tbl->grp_lst[uidx];
    if (trv.typ == nc_typ_grp ){

      /* Obtain group ID from netCDF API using full group name */
      (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);         

      /* Obtain info for group */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif

      /* List attributes using obtained group ID */
      if(nbr_att){
        (void)fprintf(stdout,"Global attributes for: %s\n",trv.nm_fll); 
        (void)nco_prn_att(grp_id,NC_GLOBAL); 
      } /*nbr_att */
    } /* end nc_typ_grp */
  } /* end uidx  */
} /* end nco_prn_att_trv() */



nm_id_sct *                      /* O [sct] Extraction list */
nco_var_lst_crd_add_trv          /* [fnc] Add all coordinates to extraction list */
(const int nc_id,                /* I [ID] netCDF file ID */
 nm_id_sct *xtr_lst,             /* I/O [sct] Current extraction list  */
 int * xtr_nbr,                  /* I/O [nbr] Number of variables in current extraction list */
 const nco_bool CNV_CCM_CCSM_CF, /* I [flg] file obeys CCM/CCSM/CF conventions */
 int * const grp_xtr_nbr,        /* I [nbr] Number of groups in current extraction list (specified with -g ) */
 char * const * const grp_lst_in)/* I [sng] User-specified list of groups names to extract (specified with -g ) */
{
  /* Purpose: Add all coordinates to extraction list
     Find all coordinates (dimensions which are also variables) and
     add them to the list if they are not already there. */

#ifdef GRP_DEV
  int nbr_var_xtr;   /* Number of variables to extract */
  char rth[2];
  strcpy(rth,"/");

  /* Initialize index of extracted variables */
  nbr_var_xtr=0;

  /* Get number of items to allocate (pass NULL to xtr_lst)  */
  (void)nco_var_lst_crd_add_itr(nc_id,rth,rth,NULL,&nbr_var_xtr,CNV_CCM_CCSM_CF,grp_xtr_nbr,grp_lst_in);

  xtr_lst=(nm_id_sct *)nco_malloc(nbr_var_xtr*sizeof(nm_id_sct));

  /* Initialize index of extracted variables */
  nbr_var_xtr=0;

  /* Recursively go to sub-groups, starting with netCDF file ID and root group name */
  (void)nco_var_lst_crd_add_itr(nc_id,rth,rth,xtr_lst,&nbr_var_xtr,CNV_CCM_CCSM_CF,grp_xtr_nbr,grp_lst_in);

  /* Export */
  *xtr_nbr=nbr_var_xtr;

#else /* GRP_DEV */

  for(idx=0;idx<nbr_dim;idx++){
    (void)nco_inq_dimname(nc_id,idx,crd_nm);

    /* Does variable of same name exist in input file? */
    rcd=nco_inq_varid_flg(nc_id,crd_nm,&crd_id);
    if(rcd == NC_NOERR){
      /* Dimension is coordinate. Is it already on list? */
      int lst_idx;

      for(lst_idx=0;lst_idx<*xtr_nbr;lst_idx++){
        if(crd_id == xtr_lst[lst_idx].id) break;
      } /* end loop over lst_idx */
      if(lst_idx == *xtr_nbr){
        /* Coordinate is not already on the list, put it there */
        if(*xtr_nbr == 0) xtr_lst=(nm_id_sct *)nco_malloc((*xtr_nbr+1)*sizeof(nm_id_sct)); else xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));
        /* According to man page for realloc(), this should work even when xtr_lst == NULL */
        /*	xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));*/
        xtr_lst[*xtr_nbr].nm=(char *)strdup(crd_nm);
        xtr_lst[*xtr_nbr].id=crd_id;
        (*xtr_nbr)++;
      } /* end if */
    } /* end if */
  } /* end loop over idx */

  /* Detect associated coordinates specified by CF "coordinates" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.1/cf-conventions.html#coordinate-system */
  if(CNV_CCM_CCSM_CF){
    const char dlm_sng[]=" "; /* [sng] Delimiter string */
    const char fnc_nm[]="nco_var_lst_crd_add()"; /* [sng] Function name */
    char **crd_lst; /* [sng] 1D array of list elements */
    char *att_val;
    char att_nm[NC_MAX_NAME];
    int idx_att;
    int idx_crd;
    int idx_var;
    int idx_var2;
    int nbr_att;
    int nbr_crd; /* [nbr] Number of coordinates specified in "coordinates" attribute */
    int var_id;
    long att_sz;
    nc_type att_typ;

    /* NB: Only difference between this algorithm and CF algorithm in 
    nco_var_lst_crd_ass_add() is that this algorithm loops over 
    all variables in file, not just over current extraction list. */ 
    /* ...for each variable in file... */
    for(idx_var=0;idx_var<nbr_var;idx_var++){
      /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
      var_id=idx_var;
      /* fxm: Functionalize remainder and call from both 
      nco_var_lst_crd_ass_add() and nco_var_lst_crd_add()? */
      /* Find number of attributes */
      (void)nco_inq_varnatts(nc_id,var_id,&nbr_att);
      for(idx_att=0;idx_att<nbr_att;idx_att++){
        (void)nco_inq_attname(nc_id,var_id,idx_att,att_nm);
        /* Is attribute part of CF convention? */
        if(!strcmp(att_nm,"coordinates")){
          /* Yes, get list of specified attributes */
          (void)nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
          if(att_typ != NC_CHAR){
            (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore %s will skip this attribute.\n",prg_nm_get(),att_nm,xtr_lst[idx_var].nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
            return xtr_lst;
          } /* end if */
          att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
          if(att_sz > 0) (void)nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
          /* NUL-terminate attribute */
          att_val[att_sz]='\0';
          /* Split list into separate coordinate names
          Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
          crd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_crd);
          /* ...for each coordinate in "coordinates" attribute... */
          for(idx_crd=0;idx_crd<nbr_crd;idx_crd++){
            /* Verify "coordinate" exists in input file */
            rcd=nco_inq_varid_flg(nc_id,crd_lst[idx_crd],&crd_id);
            /* NB: Do not check that dimension by this name exists
            CF files often use "coordinates" convention to identify
            two-dimensional (or greater) variables which serve as coordinates.
            In other words, we want to allow N-D variables to work as coordinates
            for the purpose of adding them to the extraction list only. */
            if(rcd == NC_NOERR){
              /* idx_var2 labels inner loop over variables */
              /* Is "coordinate" already on extraction list? */
              for(idx_var2=0;idx_var2<*xtr_nbr;idx_var2++){
                if(crd_id == xtr_lst[idx_var2].id) break;
              } /* end loop over idx_var2 */
              if(idx_var2 == *xtr_nbr){
                /* Add coordinate to list */
                xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));
                xtr_lst[*xtr_nbr].nm=(char *)strdup(crd_lst[idx_crd]);
                xtr_lst[*xtr_nbr].id=crd_id;
                (*xtr_nbr)++; /* NB: Changes size of current loop! */
                /* Continue to next coordinate in loop */
                continue;
              } /* end if coordinate was not already in list */
            }else{ /* end if named coordinate exists in input file */
              if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO Variable %s, specified in the \"coordinates\" attribute of variable %s, is not present in the input file\n",prg_nm_get(),crd_lst[idx_crd],xtr_lst[idx_var].nm);
            } /* end else named coordinate exists in input file */
          } /* end loop over idx_crd */
          /* Free allocated memory */
          att_val=(char *)nco_free(att_val);
          crd_lst=nco_sng_lst_free(crd_lst,nbr_crd);
        } /* !coordinates */
      } /* end loop over attributes */
    } /* end loop over idx_var */
  } /* !CNV_CCM_CCSM_CF for "coordinates" */

  /* Detect coordinate boundaries specified by CF "bounds" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.5/cf-conventions.html#cell-boundaries
  Algorithm copied with modification from "coordinates" algorithm above */
  if(CNV_CCM_CCSM_CF){
    const char dlm_sng[]=" "; /* [sng] Delimiter string */
    const char fnc_nm[]="nco_var_lst_crd_add()"; /* [sng] Function name */
    char **bnd_lst; /* [sng] 1D array of list elements */
    char *att_val;
    char att_nm[NC_MAX_NAME];
    int bnd_id;
    int idx_att;
    int idx_bnd;
    int idx_var;
    int idx_var2;
    int nbr_att;
    int nbr_bnd; /* [nbr] Number of coordinates specified in "bounds" attribute */
    int var_id;
    long att_sz;
    nc_type att_typ;

    /* NB: Only difference between this algorithm and CF algorithm in 
    nco_var_lst_crd_ass_add() is that this algorithm loops over 
    all variables in file, not just over current extraction list. */ 
    /* ...for each variable in file... */
    for(idx_var=0;idx_var<nbr_var;idx_var++){
      /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
      var_id=idx_var;
      /* fxm: Functionalize remainder and call from both 
      nco_var_lst_crd_ass_add() and nco_var_lst_crd_add()? */
      /* Find number of attributes */
      (void)nco_inq_varnatts(nc_id,var_id,&nbr_att);
      for(idx_att=0;idx_att<nbr_att;idx_att++){
        (void)nco_inq_attname(nc_id,var_id,idx_att,att_nm);
        /* Is attribute part of CF convention? */
        if(!strcmp(att_nm,"bounds")){
          /* Yes, get list of specified attributes */
          (void)nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
          if(att_typ != NC_CHAR){
            (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore %s will skip this attribute.\n",prg_nm_get(),att_nm,xtr_lst[idx_var].nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
            return xtr_lst;
          } /* end if */
          att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
          if(att_sz > 0) (void)nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
          /* NUL-terminate attribute */
          att_val[att_sz]='\0';
          /* Split list into separate coordinate names
          Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
          bnd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_bnd);
          /* ...for each coordinate in "bounds" attribute... */
          for(idx_bnd=0;idx_bnd<nbr_bnd;idx_bnd++){
            if(bnd_lst[idx_bnd]==NULL)
              continue;
            /* Verify "bounds" exists in input file */
            rcd=nco_inq_varid_flg(nc_id,bnd_lst[idx_bnd],&bnd_id);
            /* NB: Coordinates of rank N have bounds of rank N+1 */
            if(rcd == NC_NOERR){
              /* idx_var2 labels inner loop over variables */
              /* Is "bound" already on extraction list? */
              for(idx_var2=0;idx_var2<*xtr_nbr;idx_var2++){
                if(bnd_id == xtr_lst[idx_var2].id) break;
              } /* end loop over idx_var2 */
              if(idx_var2 == *xtr_nbr){
                /* Add coordinate to list */
                xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));
                xtr_lst[*xtr_nbr].nm=(char *)strdup(bnd_lst[idx_bnd]);
                xtr_lst[*xtr_nbr].id=bnd_id;
                (*xtr_nbr)++; /* NB: Changes size of current loop! */
                /* Continue to next coordinate in loop */
                continue;
              } /* end if coordinate was not already in list */
            }else{ /* end if named coordinate exists in input file */
              if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO Variable %s, specified in the \"bounds\" attribute of variable %s, is not present in the input file\n",prg_nm_get(),bnd_lst[idx_bnd],xtr_lst[idx_var].nm);
            } /* end else named coordinate exists in input file */
          } /* end loop over idx_bnd */
          /* Free allocated memory */
          att_val=(char *)nco_free(att_val);
          bnd_lst=nco_sng_lst_free(bnd_lst,nbr_bnd);
        } /* !coordinates */
      } /* end loop over attributes */
    } /* end loop over idx_var */
  } /* !CNV_CCM_CCSM_CF for "bounds" */

#endif /* GRP_DEV */

  if(dbg_lvl_get() >= nco_dbg_var){
    (void)fprintf(stdout,"%s: INFO nco4_var_lst_crd_add() reports following %d variable%s to be added with dimensions:\n",prg_nm_get(),*xtr_nbr,(*xtr_nbr > 1) ? "s" : "");
    prt_xtr_lst(xtr_lst,*xtr_nbr);
  } /* endif dbg */

  return xtr_lst;
} /* end nco4_var_lst_crd_add() */

void                  
nco_var_lst_crd_add_itr          /* [fnc] Iterator function for nco_var_lst_crd_add_trv */
(const int in_id,                /* I [ID] Group ID */
 char * const grp_nm_fll,        /* I [sng] Group path */
 char * const grp_nm,            /* I [sng] Group name */
 nm_id_sct *xtr_lst,             /* I/O [sct] Current extraction list  */
 int * xtr_nbr,                  /* I/O [nbr] Number of variables in current extraction list */
 const nco_bool CNV_CCM_CCSM_CF, /* I [flg] file obeys CCM/CCSM/CF conventions */
 int * const grp_xtr_nbr,        /* I [nbr] Number of groups in current extraction list (specified with -g ) */
 char * const * const grp_lst_in)/* I [sng] User-specified list of groups names to extract (specified with -g ) */
{
  /* Purpose: Add all coordinates to extraction list
     Find all coordinates (dimensions which are also variables) and
     add them to the list if they are not already there. */

  int rcd=NC_NOERR;            /* O [rcd] Return code */
  nc_type var_typ;             /* O [enm] Variable type */
  int nbr_att;                 /* O [nbr] Number of attributes */
  int nbr_var;                 /* O [nbr] Number of variables */
  int nbr_dmn;                 /* O [nbr] number of dimensions */
  int nbr_grp;                 /* O [nbr] Number of sub-groups in this group */
  char gp_nm[NC_MAX_NAME+1];   /* O [sng] Group name */
  char var_nm[NC_MAX_NAME+1];  /* O [sng] Variable name */ 
  int *grp_ids;                /* O [ID]  Sub-group IDs */ 
  int *dmn_ids;                /* O [ID]  Dimension IDs */
  char dmn_nm[NC_MAX_NAME];    /* O [sng] Dimension name */ 
  long dmn_sz;                 /* O [nbr] Dimension size */ 
  char *var_nm_fll;            /* Full path of variable */
  int idx;                     /* I [idx] Index */
  char *pch;                   /* Pointer to character in string */

  /* Get all information for this group */
  rcd+=nco_inq_nvars(in_id,&nbr_var);
  rcd+=nco_inq_grpname(in_id,gp_nm);
  rcd+=nco_inq_ndims(in_id,&nbr_dmn);
  rcd+=nco_inq_natts(in_id,&nbr_att);
  rcd+=nco_inq_grps(in_id,&nbr_grp,NULL);
  rcd+=nco_inq(in_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
  dmn_ids=(int *)nco_malloc(nbr_dmn*sizeof(int));
  rcd+=nco_inq_dimids(in_id,&nbr_dmn,dmn_ids,0);

  /* Get variables for this group */
  for(int var_id=0;var_id<nbr_var;var_id++){    
    rcd+=nco_inq_var(in_id,var_id,var_nm,&var_typ,NULL,NULL,&nbr_att);

    /* Allocate path buffer; add space for a trailing NULL */ 
    var_nm_fll=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(var_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(var_nm_fll,grp_nm_fll);
    if(strcmp(grp_nm_fll,"/")!=0) /* If not root group, concatenate separator */
      strcat(var_nm_fll,"/");
    strcat(var_nm_fll,var_nm); /* Concatenate variable to absolute group path */

    /* List dimensions */
    for(int jdx=0;jdx<nbr_dmn;jdx++){
      (void)nco_inq_dim(in_id,dmn_ids[jdx],dmn_nm,&dmn_sz);

      /* Compare variable name with dimension name */
      if(strcmp(dmn_nm,var_nm) == 0){
        /* No groups case, just add  */
        if (*grp_xtr_nbr == 0 ){
          (void)nco4_xtr_lst_add(var_nm,var_nm_fll,grp_nm_fll,grp_nm,var_id,in_id,xtr_lst,xtr_nbr);
          if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_var_lst_crd_add_itr() add coordinate variable: %s\n",prg_nm_get(),var_nm_fll);
        }
        /* Groups -g case, add only if current group name GRP_NM matches any of the supplied GRP_LST_IN names */
        else{  
          /* Loop through user-specified group list */
          for(int grp_idx=0;grp_idx<*grp_xtr_nbr;grp_idx++){
            /* Locate group name from -g in traversal name */
            pch=strstr(grp_nm,grp_lst_in[grp_idx]);
            /* strstr returns the first occurrence of 'grp_nm_lst' in 'nm_fll', the higher level group( closer to root) */
            if(pch != NULL){
              (void)nco4_xtr_lst_add(var_nm,var_nm_fll,grp_nm_fll,grp_nm,var_id,in_id,xtr_lst,xtr_nbr);
              if(dbg_lvl_get() >= nco_dbg_vrb)(void)fprintf(stdout,"%s: INFO nco4_var_lst_crd_add_itr() add coordinate variable: %s\n",prg_nm_get(),var_nm_fll);
            }
          } /* end grp_idx */       
        } /* end groups case */
      } /* end strcmp coordinate name */
    } /* end jdx dimensions */

    var_nm_fll=(char*)nco_free(var_nm_fll);
  } /* end var_id */

  /* Go to sub-groups */ 
  grp_ids=(int*)nco_malloc((nbr_grp)*sizeof(int));
  rcd+=nco_inq_grps(in_id,&nbr_grp,grp_ids);

  for(idx=0;idx<nbr_grp;idx++){
    char *pth=NULL;  /* Full group path */
    int gid=grp_ids[idx];
    rcd+=nco_inq_grpname(gid,gp_nm);

    /* Allocate path buffer; add space for a trailing NUL */ 
    pth=(char*)nco_malloc(strlen(grp_nm_fll)+strlen(gp_nm)+2);

    /* Initialize path with the current absolute group path */
    strcpy(pth,grp_nm_fll);
    if(strcmp(grp_nm_fll,"/")!=0) /* If not root group, concatenate separator */
      strcat(pth,"/");
    strcat(pth,gp_nm); /* Concatenate current group to absolute group path */

    /* Recursively go to sub-groups */
    (void)nco_var_lst_crd_add_itr(gid,pth,gp_nm,xtr_lst,xtr_nbr,CNV_CCM_CCSM_CF,grp_xtr_nbr,grp_lst_in);

    pth=(char*)nco_free(pth);
  }

  (void)nco_free(grp_ids);
  (void)nco_free(dmn_ids);

  return;
} /* end nco4_var_lst_crd_add() */


nm_id_sct *                       /* O [sct] Extraction list */
nco_var_lst_crd_ass_add_trv       /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id,                 /* I netCDF file ID */
 nm_id_sct *xtr_lst,              /* I/O current extraction list (destroyed) */
 int * const xtr_nbr,             /* I/O number of variables in current extraction list */
 const nco_bool CNV_CCM_CCSM_CF, /* I [flg] file obeys CCM/CCSM/CF conventions */
 grp_tbl_sct *trv_tbl)           /* I [sct] Traversal table */
{
  char dmn_nm[NC_MAX_NAME];    /* [sng] Dimension name */ 
  long dmn_sz;                 /* [nbr] Dimension size */  
  int grp_id;                  /* [ID] Group ID */
  int nbr_att;                 /* [nbr] Number of attributes */
  int nbr_var;                 /* [nbr] Number of variables */
  int nbr_dmn;                 /* [nbr] number of dimensions */
  int nbr_dmn_ult;             /* [nbr] Number of unlimited dimensions */
  int dmn_ids_ult[NC_MAX_DIMS];/* [ID] Unlimited dimensions IDs array */
  int dmn_ids[NC_MAX_DIMS];    /* [ID] Dimensions IDs array */

#ifdef GRP_DEV
  for(unsigned uidx=0;uidx<trv_tbl->nbr;uidx++){
    grp_trv_sct trv=trv_tbl->grp_lst[uidx];
    if (trv.typ == nc_typ_grp ) {

      /* Obtain group ID from netCDF API using full group name */
      (void)nco_inq_grp_full_ncid(nc_id,trv.nm_fll,&grp_id);

      /* Obtain unlimited dimensions for group: NOTE using group ID */
      (void)nco_inq_unlimdims(grp_id,&nbr_dmn_ult,dmn_ids_ult);

      /* Obtain dimensions for group: NOTE using group ID */
      (void)nco_inq(grp_id,&nbr_dmn,&nbr_var,&nbr_att,NULL);
#ifdef NCO_SANITY_CHECK
      assert(nbr_dmn == trv.nbr_dmn && nbr_var == trv.nbr_var && nbr_att == trv.nbr_att);
#endif

      (void)nco_inq_dimids(grp_id,&nbr_dmn,dmn_ids,0);

      if(nbr_dmn && dbg_lvl_get() >= nco_dbg_crr)(void)fprintf(stdout,"%s: DEBUG nco_var_lst_crd_ass_add_trv() grp=%s\n",prg_nm_get(),trv.nm_fll);

      /* List dimensions using obtained group ID */
      for(int jdx=0;jdx<nbr_dmn;jdx++){
        (void)nco_inq_dim(grp_id,dmn_ids[jdx],dmn_nm,&dmn_sz);

        if(dbg_lvl_get() >= nco_dbg_crr)(void)fprintf(stdout,"dimension: %s (%ld)\n",dmn_nm,dmn_sz);

      } /* end jdx dimensions */
    } /* end nc_typ_grp */
  } /* end uidx  */


#else
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
  rcd+=nco_inq(nc_id,&nbr_dim,(int *)NULL,(int *)NULL,(int *)NULL);

  /* 20101011: Dimension IDs in netCDF3 files will be 0..N-1 
  However, in netCDF4 files, dimension IDs may not enumerate consecutively
  Keep one code path and assume file may be netCDF4 in structure */
  /* Create space for dimension IDs */
  /*  int dmn_id_all[NC_MAX_DIMS];
  dmn_id_all=(int *)nco_malloc(nbr_dim*sizeof(int));
  (void)nco_inq_dimid(nc_id,dmn_id_all); */

  /* ...for each dimension in input file... */
  for(idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
    /* ...check name to see if it is a coordinate dimension... */
    (void)nco_inq_dimname(nc_id,idx_dmn,dmn_nm);
    rcd=nco_inq_varid_flg(nc_id,dmn_nm,&crd_id);
    if(rcd == NC_NOERR){ /* Valid coordinate (same name of dimension and variable) */
      /* Is coordinate already on extraction list? */
      for(idx_var=0;idx_var<*xtr_nbr;idx_var++){
        if(crd_id == xtr_lst[idx_var].id) break;
      } /* end loop over idx_var */
      if(idx_var == *xtr_nbr){
        /* ...coordinate is not on list, is it associated with any extracted variables?... */
        for(idx_var=0;idx_var<*xtr_nbr;idx_var++){
          /* Get number of dimensions and dimension IDs for variable */
          (void)nco_inq_var(nc_id,xtr_lst[idx_var].id,(char *)NULL,(nc_type *)NULL,&nbr_var_dim,dmn_id,(int *)NULL);
          for(idx_var_dim=0;idx_var_dim<nbr_var_dim;idx_var_dim++){
            if(idx_dmn == dmn_id[idx_var_dim]) break;
          } /* end loop over idx_var_dim */
          if(idx_var_dim != nbr_var_dim){
            /* Add coordinate to list */
            xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));
            xtr_lst[*xtr_nbr].nm=(char *)strdup(dmn_nm);
            xtr_lst[*xtr_nbr].id=crd_id;
            (*xtr_nbr)++; /* NB: Changes size of current loop! */
            break;
          } /* end if */
        } /* end loop over idx_var */
      } /* end if coordinate was not already in list */
    } /* end if dimension is coordinate */
  } /* end loop over idx_dmn */

  /* Detect associated coordinates specified by CF "coordinates" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.5/cf-conventions.html#coordinate-system */
  if(CNV_CCM_CCSM_CF){
    const char dlm_sng[]=" "; /* [sng] Delimiter string */
    const char fnc_nm[]="nco_var_lst_crd_ass_add()"; /* [sng] Function name */
    char **crd_lst; /* [sng] 1D array of list elements */
    char *att_val;
    char att_nm[NC_MAX_NAME];
    int idx_att;
    int idx_crd;
    int idx_var2;
    int nbr_att;
    int nbr_crd; /* [nbr] Number of coordinates specified in "coordinates" attribute */
    int var_id;
    long att_sz;
    nc_type att_typ;

    /* ...for each variable in extraction list... */
    for(idx_var=0;idx_var<*xtr_nbr;idx_var++){
      /* Eschew indirection */
      var_id=xtr_lst[idx_var].id;
      /* Find number of attributes */
      (void)nco_inq_varnatts(nc_id,var_id,&nbr_att);
      for(idx_att=0;idx_att<nbr_att;idx_att++){
        (void)nco_inq_attname(nc_id,var_id,idx_att,att_nm);
        /* Is attribute part of CF convention? */
        if(!strcmp(att_nm,"coordinates")){
          /* Yes, get list of specified attributes */
          (void)nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
          if(att_typ != NC_CHAR){
            (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore %s will skip this attribute.\n",prg_nm_get(),att_nm,xtr_lst[idx_var].nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
            return xtr_lst;
          } /* end if */
          att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
          if(att_sz > 0) (void)nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
          /* NUL-terminate attribute */
          att_val[att_sz]='\0';
          /* Split list into separate coordinate names
          Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
          crd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_crd);
          /* ...for each coordinate in "coordinates" attribute... */
          for(idx_crd=0;idx_crd<nbr_crd;idx_crd++){
            if(crd_lst[idx_crd]==NULL)
              continue;
            /* Verify "coordinate" exists in input file */
            rcd=nco_inq_varid_flg(nc_id,crd_lst[idx_crd],&crd_id);
            /* NB: Do not check that dimension by this name exists
            CF files often use "coordinates" convention to identify
            two-dimensional (or greater) variables which serve as coordinates.
            In other words, we want to allow N-D variables to work as coordinates
            for the purpose of adding them to the extraction list only. */
            if(rcd == NC_NOERR){
              /* idx_var2 labels inner loop over variables */
              /* Is "coordinate" already on extraction list? */
              for(idx_var2=0;idx_var2<*xtr_nbr;idx_var2++){
                if(crd_id == xtr_lst[idx_var2].id) break;
              } /* end loop over idx_var2 */
              if(idx_var2 == *xtr_nbr){
                /* Add coordinate to list */
                xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));
                xtr_lst[*xtr_nbr].nm=(char *)strdup(crd_lst[idx_crd]);
                xtr_lst[*xtr_nbr].id=crd_id;
                (*xtr_nbr)++; /* NB: Changes size of current loop! */
                /* Continue to next coordinate in loop */
                continue;
              } /* end if coordinate was not already in list */
            }else{ /* end if named coordinate exists in input file */
              if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO Variable %s, specified in the \"coordinates\" attribute of variable %s, is not present in the input file\n",prg_nm_get(),crd_lst[idx_crd],xtr_lst[idx_var].nm);
            } /* end else named coordinate exists in input file */
          } /* end loop over idx_crd */
          /* Free allocated memory */
          att_val=(char *)nco_free(att_val);
          crd_lst=nco_sng_lst_free(crd_lst,nbr_crd);
        } /* !coordinates */
      } /* end loop over attributes */
    } /* end loop over idx_var */
  } /* !CNV_CCM_CCSM_CF for "coordinates" */

  /* Detect coordinate boundaries specified by CF "bounds" convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.5/cf-conventions.html#cell-boundaries
  Algorithm copied with modification from "coordinates" algorithm above */
  if(CNV_CCM_CCSM_CF){
    const char dlm_sng[]=" "; /* [sng] Delimiter string */
    const char fnc_nm[]="nco_var_lst_crd_ass_add()"; /* [sng] Function name */
    char **bnd_lst; /* [sng] 1D array of list elements */
    char *att_val;
    char att_nm[NC_MAX_NAME];
    int bnd_id;
    int idx_att;
    int idx_bnd;
    int idx_var2;
    int nbr_att;
    int nbr_bnd; /* [nbr] Number of coordinates specified in "bounds" attribute */
    int var_id;
    long att_sz;
    nc_type att_typ;

    /* ...for each variable in extraction list... */
    for(idx_var=0;idx_var<*xtr_nbr;idx_var++){
      /* Eschew indirection */
      var_id=xtr_lst[idx_var].id;
      /* Find number of attributes */
      (void)nco_inq_varnatts(nc_id,var_id,&nbr_att);
      for(idx_att=0;idx_att<nbr_att;idx_att++){
        (void)nco_inq_attname(nc_id,var_id,idx_att,att_nm);
        /* Is attribute part of CF convention? */
        if(!strcmp(att_nm,"bounds")){
          /* Yes, get list of specified attributes */
          (void)nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
          if(att_typ != NC_CHAR){
            (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore %s will skip this attribute.\n",prg_nm_get(),att_nm,xtr_lst[idx_var].nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
            return xtr_lst;
          } /* end if */
          att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
          if(att_sz > 0) (void)nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
          /* NUL-terminate attribute */
          att_val[att_sz]='\0';
          /* Split list into separate coordinate names
          Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
          bnd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_bnd);
          /* ...for each coordinate in "bounds" attribute... */
          for(idx_bnd=0;idx_bnd<nbr_bnd;idx_bnd++){
            if(bnd_lst[idx_bnd]==NULL)
              continue;
            /* Verify "bounds" exists in input file */
            rcd=nco_inq_varid_flg(nc_id,bnd_lst[idx_bnd],&bnd_id);
            /* NB: Coordinates of rank N have bounds of rank N+1 */
            if(rcd == NC_NOERR){
              /* idx_var2 labels inner loop over variables */
              /* Is "bound" already on extraction list? */
              for(idx_var2=0;idx_var2<*xtr_nbr;idx_var2++){
                if(bnd_id == xtr_lst[idx_var2].id) break;
              } /* end loop over idx_var2 */
              if(idx_var2 == *xtr_nbr){
                /* Add coordinate to list */
                xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*xtr_nbr+1)*sizeof(nm_id_sct));
                xtr_lst[*xtr_nbr].nm=(char *)strdup(bnd_lst[idx_bnd]);
                xtr_lst[*xtr_nbr].id=bnd_id;
                (*xtr_nbr)++; /* NB: Changes size of current loop! */
                /* Continue to next coordinate in loop */
                continue;
              } /* end if coordinate was not already in list */
            }else{ /* end if named coordinate exists in input file */
              if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stderr,"%s: INFO Variable %s, specified in the \"bounds\" attribute of variable %s, is not present in the input file\n",prg_nm_get(),bnd_lst[idx_bnd],xtr_lst[idx_var].nm);
            } /* end else named coordinate exists in input file */
          } /* end loop over idx_bnd */
          /* Free allocated memory */
          att_val=(char *)nco_free(att_val);
          bnd_lst=nco_sng_lst_free(bnd_lst,nbr_bnd);
        } /* !coordinates */
      } /* end loop over attributes */
    } /* end loop over idx_var */
  } /* !CNV_CCM_CCSM_CF for "bounds" */

#endif

  return xtr_lst;
}