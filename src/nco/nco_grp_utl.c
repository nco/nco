/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.c,v 1.2 2011-07-25 03:38:42 zender Exp $ */

/* Purpose: Group utilities */

/* Copyright (C) 2011--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_grp_utl.h" /* Group utilities */

nm_id_sct * /* O [sct] Group extraction list */
nco_grp_lst_mk /* [fnc] Create group extraction list using regular expressions */
(const int nc_id, /* I [enm] netCDF file ID */
 char * const * const grp_lst_in, /* I [sng] User-specified list of group names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 int * const grp_nbr) /* I/O [nbr] Number of groups in current extraction list */
{
  /* Purpose: Create group extraction list with or without regular expressions */
  
  char *grp_sng;
  char grp_nm[NC_MAX_NAME];
  
  int idx;
  int jdx;
  int nbr_tmp;
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
  
  nm_id_sct *grp_lst=NULL; /* grp_lst may be alloc()'d from NULL with -c option */
  nm_id_sct *fl_in_grp_lst=NULL; /* [sct] All groups in input file */
  nco_bool *grp_xtr_rqs=NULL; /* [flg] Group specified in extraction list */
  
  /* Create list of all groups in input file */
  fl_in_grp_lst=(nm_id_sct *)nco_malloc(nbr_var*sizeof(nm_id_sct));
  for(idx=0;idx<nbr_var;idx++){
    /* Get name of each group */
    (void)nco_inq_varname(nc_id,idx,grp_nm);
    fl_in_grp_lst[idx].nm=(char *)strdup(grp_nm);
    fl_in_grp_lst[idx].id=idx;
  } /* end loop over idx */
  
  /* Return all groups if .. */
  if(*nbr_xtr == 0 && !EXTRACT_ALL_COORDINATES){
    *nbr_xtr=nbr_var;
    return fl_in_grp_lst;
  } /* end if */
  
  /* Initialize and allocacte nco_bool array to all False */
  grp_xtr_rqs=(nco_bool *)nco_calloc((size_t)nbr_var,sizeof(nco_bool));
  
  /* Loop through grp_lst_in */
  for(idx=0;idx<*nbr_xtr;idx++){
    grp_sng=grp_lst_in[idx];
    
    /* Convert pound signs (back) to commas */
    while(*grp_sng){
      if(*grp_sng == '#') *grp_sng=',';
      grp_sng++;
    } /* end while */
    grp_sng=grp_lst_in[idx];
    
    /* If grp_sng is regular expression... */
    if(strpbrk(grp_sng,".*^$\\[]()<>+?|{}")){
      /* ...and regular expression library is present */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
      rx_mch_nbr=nco_var_meta_search(nbr_var,fl_in_grp_lst,grp_sng,grp_xtr_rqs);
      if(rx_mch_nbr == 0) (void)fprintf(stdout,"%s: WARNING: Regular expression \"%s\" does not match any groups\nHINT: See regular expression syntax examples at http://nco.sf.net/nco.html#rx\n",prg_nm_get(),grp_sng); 
      continue;
#else
      (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to groups) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",prg_nm_get(),grp_sng);
      nco_exit(EXIT_FAILURE);
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
    } /* end if regular expression */
    
    /* Normal group so search through group array */
    for(jdx=0;jdx<nbr_var;jdx++)
      if(!strcmp(grp_sng,fl_in_grp_lst[jdx].nm)) break;

    if(jdx != nbr_var){
      /* Mark this group as requested for inclusion by user */
      grp_xtr_rqs[jdx]=True;
    }else{
      if(EXCLUDE_INPUT_LIST){ 
	/* Group need not be present if list will be excluded later... */
	if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_grp_lst_mk() reports explicitly excluded group \"%s\" is not in input file anyway\n",prg_nm_get(),grp_sng); 
      }else{ /* !EXCLUDE_INPUT_LIST */
	/* Group should be included but no matches found so die */
	(void)fprintf(stdout,"%s: ERROR nco_grp_lst_mk() reports user-specified group \"%s\" is not in input file\n",prg_nm_get(),grp_sng); 
	nco_exit(EXIT_FAILURE);
      } /* !EXCLUDE_INPUT_LIST */
    } /* end else */

  } /* end loop over grp_lst_in */
  
  /* Create final group list using bool array */
  
  /* malloc() grp_lst to maximium size(nbr_var) */
  grp_lst=(nm_id_sct *)nco_malloc(nbr_var*sizeof(nm_id_sct));
  nbr_tmp=0; /* nbr_tmp is incremented */
  for(idx=0;idx<nbr_var;idx++){
    /* Copy var to output array */
    if(grp_xtr_rqs[idx]){
      grp_lst[nbr_tmp].nm=(char *)strdup(fl_in_grp_lst[idx].nm);
      grp_lst[nbr_tmp].id=fl_in_grp_lst[idx].id;
      nbr_tmp++;
    } /* end if */
    (void)nco_free(fl_in_grp_lst[idx].nm);
  } /* end loop over var */
  
  /* realloc() list to actual size */  
  grp_lst=(nm_id_sct *)nco_realloc(grp_lst,nbr_tmp*sizeof(nm_id_sct));

  (void)nco_free(fl_in_grp_lst);
  (void)nco_free(grp_xtr_rqs);

  *nbr_xtr=nbr_tmp;    
  return grp_lst;
} /* end nco_grp_lst_mk() */

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
  int rcr_lvl=0; /* [nbr] Recursion level */

  /* For each (possibly user-specified) top-level group ... */
  for(idx=0;idx<grp_nbr;idx++){
    int grp_out_id;
    /* Define group and all subgroups */
    rcd+=nco_def_grp_rcr(in_id,out_id,prn_id,rcr_lvl)
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
  char *grp_nm[NC_MAX_NAME];

  int idx;
  int grp_nbr; /* I [nbr] Number of sub-groups */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int *grp_in_id[NC_MAX_DIMS]; /* [ID] Sub-group IDs in input file */ /* fxm: NC_MAX_GRPS? */
  int *grp_out_id[NC_MAX_DIMS]; /* [ID] Sub-group IDs in output file */

  /* How many and which sub-groups are in this group? */
  rcd+=nco_inq_grps(in_id,grp_nbr,grp_in_id);

  if(dbg_lvl >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO nco_def_grp_rcr() reports recursion level = %d parent group = %s has %d sub-groups\n",prg_nm_get(),rcr_lvl,prn_nm,grp_nbr);

  /* Define each group, recursively, in output file */
  for(idx=0;idx<grp_nbr;idx++){

    /* Obtain name of current group in input file */
    rcd+=nco_inq_grpname(grp_in_id[idx],grp_nm);

    /* Define group of same name in output file */
    rcd+=nco_def_grp(out_id,grp_nm,grp_out_id+idx);

    /* Define group and all sub-groups */
    rcd+=nco_def_grp_rcr(grp_in_id[idx],grp_out_id[idx],grp_nm,rcr_lvl+1);
  } /* end loop over sub-groups groups */

  return rcd;
} /* end nco_grp_dfn_rcr() */

