/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_utl.c,v 1.6 2011-07-26 00:46:17 zender Exp $ */

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
    /* Define group and all subgroups */
    rcd+=nco_def_grp_rcr(in_id,out_id,grp_xtr_lst[idx].nm,rcr_lvl);
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
  int grp_in_id[NC_MAX_DIMS]; /* [ID] Sub-group IDs in input file */ /* fxm: NC_MAX_GRPS? */
  int grp_out_id[NC_MAX_DIMS]; /* [ID] Sub-group IDs in output file */

  /* How many and which sub-groups are in this group? */
  rcd+=nco_inq_grps(in_id,&grp_nbr,grp_in_id);

  if(dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO nco_def_grp_rcr() reports recursion level = %d parent group = %s has %d sub-group%s\n",prg_nm_get(),rcr_lvl,prn_nm,grp_nbr,(grp_nbr == 1) ? "" : "s");

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

nm_id_sct * /* O [sct] Group extraction list */
nco_grp_lst_mk /* [fnc] Create group extraction list using regular expressions */
(const int nc_id, /* I [enm] netCDF file ID */
 char * const * const grp_lst_in, /* I [sng] User-specified list of group names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 int * const grp_nbr_xtr) /* I/O [nbr] Number of groups in current extraction list */
{
  /* Purpose: Create group extraction list with or without regular expressions
     Code adapted from nco_var_lst_mk() and nearly identical in all respects */
  
  char *grp_sng; /* User-specified group name or regular expression */
  char grp_nm[NC_MAX_NAME];
  
  int grp_in_id[NC_MAX_DIMS]; /* [ID] Sub-group IDs in input file */ /* fxm: NC_MAX_GRPS? */
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
  rcd+=nco_inq_grps(nc_id,&grp_nbr_top,grp_in_id);

  /* Create list of all groups in input file */
  grp_lst_all=(nm_id_sct *)nco_malloc(grp_nbr_top*sizeof(nm_id_sct));
  for(idx=0;idx<grp_nbr_top;idx++){
    /* Get name of each group */
    (void)nco_inq_grpname(grp_in_id[idx],grp_nm);
    grp_lst_all[idx].nm=(char *)strdup(grp_nm);
    grp_lst_all[idx].id=grp_in_id[idx];
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

  (void)nco_free(grp_lst_all);
  (void)nco_free(grp_xtr_rqs);

  *grp_nbr_xtr=grp_nbr_tmp;    
  return grp_lst;
} /* end nco_grp_lst_mk() */
