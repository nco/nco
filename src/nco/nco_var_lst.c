/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_lst.c,v 1.118 2012-07-12 04:56:17 pvicente Exp $ */

/* Purpose: Variable list utilities */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_var_lst.h" /* Variable list utilities */

nm_id_sct * /* O [sct] Variable extraction list */
nco_var_lst_mk /* [fnc] Create variable extraction list using regular expressions */
(const int nc_id, /* I [enm] netCDF file ID */
 const int var_nbr_all, /* I [nbr] Number of variables in input file */
 char * const * const var_lst_in, /* I [sng] User-specified list of variable names and rx's */
 const nco_bool EXCLUDE_INPUT_LIST, /* I [flg] Exclude rather than extract */
 const nco_bool EXTRACT_ALL_COORDINATES, /* I [flg] Process all coordinates */
 int * const var_xtr_nbr) /* I/O [nbr] Number of variables in current extraction list */
{
  /* Purpose: Create variable extraction list with or without regular expressions */
  
  char *var_sng; /* User-specified variable name or regular expression */
  char var_nm[NC_MAX_NAME];
  
  int idx;
  int jdx;
  int var_nbr_tmp;
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
  int rx_mch_nbr;
#endif /* NCO_HAVE_REGEX_FUNCTIONALITY */
  
  nm_id_sct *xtr_lst=NULL; /* xtr_lst may be alloc()'d from NULL with -c option */
  nm_id_sct *var_lst_all=NULL; /* [sct] All variables in input file */
  nco_bool *var_xtr_rqs=NULL; /* [flg] Variable specified in extraction list */
  
  /* Create list of all variables in input file */
  var_lst_all=(nm_id_sct *)nco_malloc(var_nbr_all*sizeof(nm_id_sct));
  for(idx=0;idx<var_nbr_all;idx++){
    /* Get name of each variable */
    (void)nco_inq_varname(nc_id,idx,var_nm);
    var_lst_all[idx].nm=(char *)strdup(var_nm);
    var_lst_all[idx].id=idx;
  } /* end loop over idx */
  
  /* Return all variables if none were specified and not -c ... */
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
	if(dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO nco_var_lst_mk() reports explicitly excluded variable \"%s\" is not in input file anyway\n",prg_nm_get(),var_sng); 
      }else{ /* !EXCLUDE_INPUT_LIST */
	/* Variable should be included but no matches found so die */
	(void)fprintf(stdout,"%s: ERROR nco_var_lst_mk() reports user-specified variable \"%s\" is not in input file\n",prg_nm_get(),var_sng); 
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
  } /* end loop over var */
  
  /* realloc() list to actual size */  
  xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,var_nbr_tmp*sizeof(nm_id_sct));

  var_lst_all=(nm_id_sct *)nco_nm_id_lst_free(var_lst_all,var_nbr_all);
  var_xtr_rqs=(nco_bool *)nco_free(var_xtr_rqs);

  *var_xtr_nbr=var_nbr_tmp;    
  return xtr_lst;
} /* end nco_var_lst_mk() */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_xcl /* [fnc] Convert exclusion list to extraction list */
(const int nc_id, /* I netCDF file ID */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current exclusion list (destroyed) */
 int * const xtr_nbr) /* I/O [nbr] Number of variables in exclusion/extraction list */
{
  /* Purpose: Convert exclusion list to extraction list
     User wants to extract all variables except those currently in list
     It is hard to edit existing list so copy existing extraction list into 
     exclusion list, then construct new extraction list from scratch. */

  char var_nm[NC_MAX_NAME];

  int idx;
  int lst_idx;
  int nbr_xcl;

  nm_id_sct *xcl_lst;
  
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
  
  return xtr_lst;
} /* end nco_var_lst_xcl() */

nco_bool /* [flg] Variable is listed in a "coordinates" attribute */
nco_is_spc_in_crd_att /* [fnc] Variable is listed in a "coordinates" attribute */
(const int nc_id, /* I [id] netCDF file ID */
 const int var_trg_id) /* I [id] Variable ID */
{
  /* Purpose: Is variable specified in a "coordinates" attribute?
     Typical variables that appear in a "coordinates" attribute include ...
     If so, it may be a "multi-dimensional coordinate" that should
     undergo special treatment by arithmetic operators. */
  nco_bool IS_SPC_IN_CRD_ATT=False; /* [flg] Variable is listed in a "coordinates" attribute  */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_is_spc_in_crd_att()"; /* [sng] Function name */
  char **crd_lst; /* [sng] 1D array of list elements */
  char *att_val;
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  char var_trg_nm[NC_MAX_NAME];
  int idx_att;
  int idx_crd;
  int idx_var;
  int nbr_att;
  int nbr_crd; /* [nbr] Number of coordinates specified in "coordinates" attribute */
  int nbr_var; /* [nbr] Number of variables in file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id; /* [id] Variable ID */
  long att_sz;
  nc_type att_typ;
  
  /* May need variable name for later comparison to "coordinates" attribute */
  rcd+=nco_inq_varname(nc_id,var_trg_id,var_trg_nm);
  rcd+=nco_inq_nvars(nc_id,&nbr_var);

  for(idx_var=0;idx_var<nbr_var;idx_var++){
    /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
    var_id=idx_var;
    
    /* Find number of attributes */
    rcd+=nco_inq_varnatts(nc_id,var_id,&nbr_att);
    for(idx_att=0;idx_att<nbr_att;idx_att++){
      rcd+=nco_inq_attname(nc_id,var_id,idx_att,att_nm);
      /* Is attribute part of CF convention? */
      if(!strcmp(att_nm,"coordinates")){
	/* Yes, get list of specified attributes */
	rcd+=nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
	if(att_typ != NC_CHAR){
	  rcd=nco_inq_varname(nc_id,var_id,var_nm);
	  (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore %s will skip this attribute.\n",prg_nm_get(),att_nm,var_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
	  return IS_SPC_IN_CRD_ATT;
	} /* end if */
	att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
	if(att_sz > 0) rcd=nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
	/* NUL-terminate attribute */
	att_val[att_sz]='\0';
	/* Split list into separate coordinate names
	   Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
	crd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_crd);
	/* ...for each coordinate in "coordinates" attribute... */
	for(idx_crd=0;idx_crd<nbr_crd;idx_crd++){
	  /* Does variable match name specified in coordinate list? */
	  if(!strcmp(var_trg_nm,crd_lst[idx_crd])) break;
	} /* end loop over coordinates in list */
	if(idx_crd!=nbr_crd) IS_SPC_IN_CRD_ATT=True;
	/* Free allocated memory */
	att_val=(char *)nco_free(att_val);
	crd_lst=nco_sng_lst_free(crd_lst,nbr_crd);
      } /* !coordinates */
    } /* end loop over attributes */
  } /* end loop over idx_var */
  
  return IS_SPC_IN_CRD_ATT; /* [flg] Variable is listed in a "coordinates" attribute  */
} /* end nco_is_spc_in_crd_att() */

nco_bool /* [flg] Variable is listed in a "bounds" attribute */
nco_is_spc_in_bnd_att /* [fnc] Variable is listed in a "bounds" attribute */
(const int nc_id, /* I [id] netCDF file ID */
 const int var_trg_id) /* I [id] Variable ID */
{
  /* Purpose: Is variable specified in a "bounds" attribute?
     Typical variables that appear in a "bounds" attribute include "lat_bnds", "lon_bnds", etc.
     Such variables may be "multi-dimensional coordinates" that should
     undergo special treatment by arithmetic operators.
     Routine based on nco_is_spc_in_crd_att() */
  nco_bool IS_SPC_IN_BND_ATT=False; /* [flg] Variable is listed in a "bounds" attribute  */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_is_spc_in_bnd_att()"; /* [sng] Function name */
  char **bnd_lst; /* [sng] 1D array of list elements */
  char *att_val;
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  char var_trg_nm[NC_MAX_NAME];
  int idx_att;
  int idx_bnd;
  int idx_var;
  int nbr_att;
  int nbr_bnd; /* [nbr] Number of coordinates specified in "bounds" attribute */
  int nbr_var; /* [nbr] Number of variables in file */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_id; /* [id] Variable ID */
  long att_sz;
  nc_type att_typ;
  
  /* May need variable name for later comparison to "bounds" attribute */
  rcd+=nco_inq_varname(nc_id,var_trg_id,var_trg_nm);
  rcd+=nco_inq_nvars(nc_id,&nbr_var);

  for(idx_var=0;idx_var<nbr_var;idx_var++){
    /* This assumption, praise the Lord, is valid in netCDF2, netCDF3, and netCDF4 */
    var_id=idx_var;
    
    /* Find number of attributes */
    rcd+=nco_inq_varnatts(nc_id,var_id,&nbr_att);
    for(idx_att=0;idx_att<nbr_att;idx_att++){
      rcd+=nco_inq_attname(nc_id,var_id,idx_att,att_nm);
      /* Is attribute part of CF convention? */
      if(!strcmp(att_nm,"bounds")){
	/* Yes, get list of specified attributes */
	rcd+=nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
	if(att_typ != NC_CHAR){
	  rcd=nco_inq_varname(nc_id,var_id,var_nm);
	  (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore %s will skip this attribute.\n",prg_nm_get(),att_nm,var_nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
	  return IS_SPC_IN_BND_ATT;
	} /* end if */
	att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
	if(att_sz > 0) rcd=nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
	/* NUL-terminate attribute */
	att_val[att_sz]='\0';
	/* Split list into separate coordinate names
	   Use nco_lst_prs_sgl_2D() not nco_lst_prs_2D() to avert TODO nco944 */
	bnd_lst=nco_lst_prs_sgl_2D(att_val,dlm_sng,&nbr_bnd);
	/* ...for each coordinate in "bounds" attribute... */
	for(idx_bnd=0;idx_bnd<nbr_bnd;idx_bnd++){
	  /* Does variable match name specified in coordinate list? */
	  if(!strcmp(var_trg_nm,bnd_lst[idx_bnd])) break;
	} /* end loop over coordinates in list */
	if(idx_bnd!=nbr_bnd) IS_SPC_IN_BND_ATT=True;
	/* Free allocated memory */
	att_val=(char *)nco_free(att_val);
	bnd_lst=nco_sng_lst_free(bnd_lst,nbr_bnd);
      } /* !coordinates */
    } /* end loop over attributes */
  } /* end loop over idx_var */
  
  return IS_SPC_IN_BND_ATT; /* [flg] Variable is listed in a "bounds" attribute  */
} /* end nco_is_spc_in_bnd_att() */

void
nco_var_lst_fix_rec_dvd /* [fnc] Divide extraction list into fixed and record data */
(const int nc_id, /* I [id] netCDF file ID */
 nm_id_sct *xtr_lst, /* I/O [sct] Extraction list (pointers to it are constructed) */
 const int xtr_nbr, /* I [nbr] Number of variables in extraction list */
 nm_id_sct ***fix_lst, /* O [sct] Fixed-length variables */
 int * const fix_nbr, /* O [nbr] Number of fixed-length variables */
 nm_id_sct ***rec_lst, /* O [sct] Record variables */
 int * const rec_nbr) /* O [nbr] Number of record variables */
{
  /* Purpose: Divide extraction list into fixed-length and record variables
     These lists will be used by netCDF3 Multi-record Multi-variable netCDF3 file (MM3) workaround
     Routine assumes file is in netCDF3 format */
  int dmn_nbr;
  int idx;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  int rcd=NC_NOERR; /* [rcd] Return code */

  int *dmn_id;

  nco_bool flg_crr_var_rec; /* [flg] Current variable is record variable */

  /* Initialize variables */
  *fix_nbr=0;
  *rec_nbr=0;

  /* Allocate too much space */
  *fix_lst=(nm_id_sct **)nco_malloc(xtr_nbr*sizeof(nm_id_sct *));
  *rec_lst=(nm_id_sct **)nco_malloc(xtr_nbr*sizeof(nm_id_sct *));

  /* Assume file contains record dimension (and netCDF3 files can have only one record dimension) */
  rcd+=nco_inq_unlimdim(nc_id,&rec_dmn_id);
  for(idx=0;idx<xtr_nbr;idx++){
    /* Assume current variable is fixed */
    flg_crr_var_rec=False;
    rcd+=nco_inq_varndims(nc_id,xtr_lst[idx].id,&dmn_nbr);
    if(dmn_nbr > 0){
      dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
      rcd+=nco_inq_vardimid(nc_id,xtr_lst[idx].id,dmn_id);
      /* netCDF3 requires record dimension to be first dimension */
      if(dmn_id[0] == rec_dmn_id) flg_crr_var_rec=True;
      if(dmn_id) dmn_id=(int*)nco_free(dmn_id);
    } /* endif dmn_nbr > 0 */
    if(flg_crr_var_rec){
      /* Current variable is record variable */
      (*rec_lst)[*rec_nbr]=xtr_lst+idx;
      ++*rec_nbr;
    }else{
      /* Current variable is fixed-length */
      (*fix_lst)[*fix_nbr]=xtr_lst+idx;
      ++*fix_nbr;
    } /* endif no record variable in file */
  } /* end loop over variables */
  *fix_lst=(nm_id_sct **)nco_realloc((void *)*fix_lst,*fix_nbr*sizeof(nm_id_sct *));
  *rec_lst=(nm_id_sct **)nco_realloc((void *)*rec_lst,*rec_nbr*sizeof(nm_id_sct *));

} /* end nco_var_lst_fix_rec_dvd() */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_crd_add /* [fnc] Add all coordinates to extraction list */
(const int nc_id, /* I [id] netCDF file ID */
 const int nbr_dim, /* I [nbr] Number of dimensions in input file */
 const int nbr_var, /* I [nbr] Number of variables in input file */
 nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
 int * const xtr_nbr, /* I/O [nbr] Number of variables in current extraction list */
 const nco_bool CNV_CCM_CCSM_CF) /* I [flg] file obeys CCM/CCSM/CF conventions */
{
  /* Purpose: Add all coordinates to extraction list
     Find all coordinates (dimensions which are also variables) and
     add them to the list if they are not already there. */
  
  char crd_nm[NC_MAX_NAME];

  int crd_id;
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */

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

  return xtr_lst;
} /* end nco_var_lst_crd_add() */

nm_id_sct * /* O [sct] Extraction list */
nco_var_lst_crd_ass_add /* [fnc] Add to extraction list all coordinates associated with extracted variables */
(const int nc_id, /* I netCDF file ID */
 nm_id_sct *xtr_lst, /* I/O current extraction list (destroyed) */
 int * const xtr_nbr, /* I/O number of variables in current extraction list */
 const nco_bool CNV_CCM_CCSM_CF) /* I [flg] file obeys CCM/CCSM/CF conventions */
{
  /* Purpose: Add coordinates associated with variables to extraction list */

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

  return xtr_lst;
  
} /* end nco_var_lst_crd_ass_add() */

nm_id_sct * /* O [sct] List with coordinate excluded */
nco_var_lst_crd_xcl /* [fnc] Exclude given coordinates from extraction list */
(const int nc_id, /* I [id] netCDF file ID */
 const int dmn_id, /* I [id] Dimension ID of coordinate to remove from extraction list */
 nm_id_sct *xtr_lst, /* I/O [sct] Current extraction list (destroyed) */
 int * const xtr_nbr) /* I/O [nbr] Number of variables in extraction list */
{
  /* Purpose: Modify extraction list to exclude coordinate, if any, associated with given dimension ID */
  
  char crd_nm[NC_MAX_NAME];

  int idx;
  int crd_id=-1;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  /* What is variable ID of record coordinate, if any? */
  (void)nco_inq_dimname(nc_id,dmn_id,crd_nm);
   
  rcd=nco_inq_varid_flg(nc_id,crd_nm,&crd_id);
  if(rcd == NC_NOERR){
    /* Is coordinate on extraction list? */
    for(idx=0;idx<*xtr_nbr;idx++){
      if(xtr_lst[idx].id == crd_id) break;
    } /* end loop over idx */
    if(idx != *xtr_nbr){
      nm_id_sct *var_lst_tmp;
      
      var_lst_tmp=(nm_id_sct *)nco_malloc(*xtr_nbr*sizeof(nm_id_sct));
      /* Copy the extract list to the temporary extract list and reallocate the extract list */
      (void)memcpy((void *)var_lst_tmp,(void *)xtr_lst,*xtr_nbr*sizeof(nm_id_sct));
      (*xtr_nbr)--;
      xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,*xtr_nbr*sizeof(nm_id_sct));
      /* Collapse the temporary extract list into the permanent list by copying 
	 all but the coordinate. NB: the ordering of the list is conserved. */
      (void)memcpy((void *)xtr_lst,(void *)var_lst_tmp,idx*sizeof(nm_id_sct));
      (void)memcpy((void *)(xtr_lst+idx),(void *)(var_lst_tmp+idx+1),(*xtr_nbr-idx)*sizeof(nm_id_sct));
      /* Free the memory for coordinate name in the extract list before losing the pointer */
      var_lst_tmp[idx].nm=(char *)nco_free(var_lst_tmp[idx].nm);
      var_lst_tmp=(nm_id_sct *)nco_free(var_lst_tmp);
    } /* end if */
  } /* end if */
  
  return xtr_lst;
  
} /* end nco_var_lst_crd_xcl() */

void
nco_var_lst_convert /* [fnc] Make variable structure list from variable name ID list */
(const int nc_id, /* I [enm] netCDF file ID */
 nm_id_sct *xtr_lst, /* I [sct] Current extraction list (destroyed) */
 const int xtr_nbr, /* I [nbr] Number of variables in input file */
 dmn_sct * const * const dim, /* I [sct] Dimensions associated with input variable list */
 const int nbr_dmn_xtr, /* I [nbr] Number of dimensions in list  */
 var_sct *** const var_ptr, /* O [sct] Variable list (for input file) */
 var_sct *** const var_out_ptr) /* O [sct] Duplicate variable list (for output file) */
{
  /* Purpose: Make var_sct list from nm_id list 
     The var_sct lst is duplicated to be used for output list */

  int idx;

  var_sct **var;
  var_sct **var_out;

  var=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));
  var_out=(var_sct **)nco_malloc(xtr_nbr*sizeof(var_sct *));

  /* Fill-in variable structure list for all extracted variables */
  for(idx=0;idx<xtr_nbr;idx++){
    var[idx]=nco_var_fll(nc_id,xtr_lst[idx].id,xtr_lst[idx].nm,dim,nbr_dmn_xtr);
    var_out[idx]=nco_var_dpl(var[idx]);
    (void)nco_xrf_var(var[idx],var_out[idx]);
    (void)nco_xrf_dmn(var_out[idx]);
  } /* end loop over idx */
  
  *var_ptr=var;
  *var_out_ptr=var_out;

} /* end nco_var_lst_convert() */

void
nco_var_lst_dvd /* [fnc] Divide input lists into output lists */
(var_sct * const * const var, /* I [sct] Variable list (input file) */
 var_sct * const * const var_out, /* I [sct] Variable list (output file) */
 const int nbr_var, /* I [nbr] Number of variables */
 const nco_bool CNV_CCM_CCSM_CF, /* I [flg] File adheres to NCAR CCM/CCSM/CF conventions */
 const int nco_pck_map, /* I [enm] Packing map */
 const int nco_pck_plc, /* I [enm] Packing policy */
 CST_X_PTR_CST_PTR_CST_Y(dmn_sct,dmn_xcl), /* I [sct] Dimensions not allowed in fixed variables */
 const int nbr_dmn_xcl, /* I [nbr] Number of altered dimensions */
 var_sct *** const var_fix_ptr, /* O [sct] Fixed-variables (input file) */
 var_sct *** const var_fix_out_ptr, /* O [sct] Fixed-variables (output file) */
 int * const nbr_var_fix, /* O [nbr] Number of fixed variables */
 var_sct *** const var_prc_ptr, /* O [sct] Processed-variables (input file) */
 var_sct *** const var_prc_out_ptr, /* O [sct] Processed-variables (output file) */
 int * const nbr_var_prc) /* O [nbr] Number of processed variables */
{
  /* Purpose: Divide two input lists into output lists based on program type */

  char *var_nm=NULL_CEWI;

  int idx;
  int prg_id; /* Program key */

  enum op_typ{
    fix, /* 0 [enm] Fix variable (operator alters neither data nor metadata) */
    prc /* 1 [enm] Process variable (operator may alter data or metadata) */
  }; /* end op_typ */

  int idx_dmn;
  int idx_xcl;
  int var_op_typ[NC_MAX_VARS];

  nco_bool is_spc_in_bnd_att; /* [flg] Is specified in a "bounds"      attribute */
  nco_bool is_spc_in_crd_att; /* [flg] Is specified in a "coordinates" attribute */
  nco_bool is_sz_rnk_prv_rth_opr; /* [flg] Size- and rank-preserving operator */
  nco_bool var_typ_fnk=False; /* [flg] Variable type is too funky for arithmetic */ /* CEWI */

  nc_type var_typ=NC_NAT; /* NC_NAT present in netcdf.h version netCDF 3.5+ */

  var_sct **var_fix;
  var_sct **var_fix_out;
  var_sct **var_prc;
  var_sct **var_prc_out;

  prg_id=prg_get(); /* Program key */

  /* Allocate space for too many structures first then realloc() appropriately
     It is calling function's responsibility to free() this memory */
  var_fix=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_fix_out=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_prc=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));
  var_prc_out=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));

  is_sz_rnk_prv_rth_opr=nco_is_sz_rnk_prv_rth_opr(prg_id,nco_pck_plc);

  /* Find operation type for each variable: for now this is either fix or prc */
  for(idx=0;idx<nbr_var;idx++){
    
    /* Initialize operation type to processed. Change to fixed where warranted later. */
    var_op_typ[idx]=prc;
    var_nm=var[idx]->nm;
    var_typ=var[idx]->type;
    if((var_typ == NC_BYTE) || (var_typ == NC_UBYTE) || (var_typ == NC_CHAR) || (var_typ == NC_STRING)) var_typ_fnk=True; else var_typ_fnk=False;

    /* Many operators should not process coordinate variables, or auxiliary coordinate variables (lat, lon, time, latixy, longxy, ...) and bounds (lat_bnds, lon_bnds, ...) */
    is_spc_in_crd_att=nco_is_spc_in_crd_att(var[idx]->nc_id,var[idx]->id);
    is_spc_in_bnd_att=nco_is_spc_in_bnd_att(var[idx]->nc_id,var[idx]->id);

    /* Override operation type based depending on variable properties and program */
    switch(prg_id){
    case ncap:
      var_op_typ[idx]=fix;
      break;
    case ncatted:
      /* Do nothing */
      break;
    case ncbo:
      if(var[idx]->is_crd_var || is_spc_in_bnd_att || is_spc_in_crd_att || var_typ_fnk) var_op_typ[idx]=fix;
      break;
    case ncea:
      if(var[idx]->is_crd_var || is_spc_in_bnd_att || is_spc_in_crd_att || var_typ_fnk) var_op_typ[idx]=fix;
      break;
    case ncecat:
      /* Allow ncecat to concatenate funky variables */
      if(var[idx]->is_crd_var || is_spc_in_bnd_att || is_spc_in_crd_att) var_op_typ[idx]=fix;
      break;
    case ncflint:
      /* Allow ncflint to interpolate record variables, not fixed coordinates */
      if((var[idx]->is_crd_var || is_spc_in_bnd_att || is_spc_in_crd_att || var_typ_fnk) && !var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncks:
      /* Do nothing */
      break;
    case ncra:
      if(!var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncrcat:
      if(!var[idx]->is_rec_var) var_op_typ[idx]=fix;
      break;
    case ncpdq:
    case ncwa:
      if(nco_pck_plc != nco_pck_plc_nil){
	/* Packing operation requested
	   Variables are processed for packing/unpacking operator unless... */
	if(
	   /* ...packing coordinate variables has few benefits... */
	   (var[idx]->is_crd_var && !(nco_pck_plc == nco_pck_plc_upk) ) ||
     /* unless if it's NOT a record variable and the policy is unpack 
        20120711. nco: ncpdq unpack coordinate variables */     
	   /* ...likewise for variables listed in "coordinates" or "bounds" attributes... */
	   (is_spc_in_crd_att || is_spc_in_bnd_att) ||
	   /* ...unpacking requested for unpacked variable... */
	   (nco_pck_plc == nco_pck_plc_upk && !var[idx]->pck_ram) ||
	   /* ...or packing unpacked requested and variable is already packed... */
	   (nco_pck_plc == nco_pck_plc_all_xst_att && var[idx]->pck_ram) ||
	   /* ...or re-packing packed requested and variable is unpacked... */
	   (nco_pck_plc == nco_pck_plc_xst_new_att && !var[idx]->pck_ram) ||
	   /* ...or... */
	   (
	    /* ...any type of packing requested... */
	    (nco_pck_plc == nco_pck_plc_all_new_att || 
	    nco_pck_plc == nco_pck_plc_all_xst_att || 
	    nco_pck_plc == nco_pck_plc_xst_new_att) &&
	    /* ...yet map does not allow (re-)packing... */
	    !nco_pck_plc_typ_get(nco_pck_map,var[idx]->typ_upk,(nc_type *)NULL)
	    )
	   )
	  var_op_typ[idx]=fix;
      }else{ /* endif packing operation requested */
	/* Process every variable containing an altered (averaged, re-ordered, reversed) dimension */
	for(idx_dmn=0;idx_dmn<var[idx]->nbr_dim;idx_dmn++){
	  for(idx_xcl=0;idx_xcl<nbr_dmn_xcl;idx_xcl++){
	    if(var[idx]->dim[idx_dmn]->id == dmn_xcl[idx_xcl]->id) break;
	  } /* end loop over idx_xcl */
	  if(idx_xcl != nbr_dmn_xcl){
	    var_op_typ[idx]=prc;
	    break;
	  } /* end if */
	} /* end loop over idx_dmn */
	/* Fix variables with no altered (averaged, re-ordered, reversed) dimensions */
	if(idx_dmn == var[idx]->nbr_dim) var_op_typ[idx]=fix;
      } /* endif averaging or re-ordering */
      break;
    default: nco_dfl_case_prg_id_err(); break;
    } /* end switch */
    
    /* Previous case-statement does not account for variables with no data */
    if(nco_is_rth_opr(prg_id))
      if(var[idx]->sz == 0L)
	var_op_typ[idx]=fix;

    if(CNV_CCM_CCSM_CF){
      if(!strcmp(var_nm,"ntrm") || !strcmp(var_nm,"ntrn") || !strcmp(var_nm,"ntrk") || !strcmp(var_nm,"ndbase") || !strcmp(var_nm,"nsbase") || !strcmp(var_nm,"nbdate") || !strcmp(var_nm,"nbsec") || !strcmp(var_nm,"mdt") || !strcmp(var_nm,"mhisf")) var_op_typ[idx]=fix;
      /* NB: all !strcmp()'s except "msk_" which uses strstr() */
      if(is_sz_rnk_prv_rth_opr && (!strcmp(var_nm,"hyam") || !strcmp(var_nm,"hybm") || !strcmp(var_nm,"hyai") || !strcmp(var_nm,"hybi") || !strcmp(var_nm,"gw") || !strcmp(var_nm,"lon_bnds") || !strcmp(var_nm,"lat_bnds") || !strcmp(var_nm,"area") || !strcmp(var_nm,"ORO") || !strcmp(var_nm,"date") || !strcmp(var_nm,"datesec") || (strstr(var_nm,"msk_") == var_nm))) var_op_typ[idx]=fix;
      /* Known "multi-dimensional coordinates" in CCSM-like model output:
	 lat, lon, lev are normally 1-D coordinates
	 Known exceptions:
	 lat and lon are "2-D coordinates" in NARCCAP output
	 NARCCAP specifies lat and lon in "coordinates" attribute of 2-D fields
	 latixy and longxy are "2-D coordinates" in CLM output
	 CLM does not specify latixy and longxy in "coordinates" attribute of any fields
	 NARCCAP output gives all "coordinate-like" fields an "axis" attribute
	 This includes the record coordinate (i.e., "time") which both ncra and ncwa _should_ process
	 CLM does not give an "axis" attribute to any fields
	 One method of chasing down all "coordinate-like" fields is to look
	 for the field name in the "coordinates" attribute of any variable.
	 However, this will miss (false-negative) the case when no variables 
	 use an N-D coordinate-like variable as a coordinate. 
	 And this may hit (false-positive) the record coordinate (often "time")
	 which should be averaged by ncra, though perhaps not by ncea.
	 "coordinate-like" variables that should be "fixed", and not
	 differenced, interpolated, or ensemble-averaged, include those 
	 satisfying these conditions:
	 0. Traditional coordinate (1-D variable same name as its dimension)
	 1. Present in a "coordinates" attribute (except "time" for ncra)
	 2. Present in a "bounds" attribute (except "time_bnds" for ncra)
	 3. Contain an "axis" attribute (except "time") fxm not done yet
	 4. Found in empirical list of variables
	 NB: In the above algorithm discussion, "time" is my shorthand 
	 for "the record variable, if any" */

      /* Conditions #1 and #2 are already implemented above in the case() statement */
      /* Check condition #4 above: */
      if(is_sz_rnk_prv_rth_opr && (!strcmp(var_nm,"lat") || !strcmp(var_nm,"lon") || !strcmp(var_nm,"lev") || !strcmp(var_nm,"longxy") || !strcmp(var_nm,"latixy") )) var_op_typ[idx]=fix;
    } /* end if CNV_CCM_CCSM_CF */

    /* Warn about any expected weird behavior */
    if(var_op_typ[idx] == prc){
      if(var_typ_fnk && ((prg_id != ncecat) && (prg_id != ncpdq) && (prg_id != ncrcat))){
	if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: INFO Variable %s is of type %s, for which requested processing (i.e., averaging, differencing) is ill-defined\n",prg_nm_get(),var[idx]->nm,nco_typ_sng(var[idx]->type));
      } /* end if */
    } /* end if prc */

  } /* end loop over var */

  /* Assign list pointers based on operation type for each variable */
  *nbr_var_prc=*nbr_var_fix=0;
  for(idx=0;idx<nbr_var;idx++){
    if(var_op_typ[idx] == fix){
      var[idx]->is_fix_var=var_out[idx]->is_fix_var=True;
      var_fix[*nbr_var_fix]=var[idx];
      var_fix_out[*nbr_var_fix]=var_out[idx];
      ++*nbr_var_fix;
    }else{
      var[idx]->is_fix_var=var_out[idx]->is_fix_var=False;
      var_prc[*nbr_var_prc]=var[idx];
      var_prc_out[*nbr_var_prc]=var_out[idx];
      ++*nbr_var_prc;
    } /* end else */
  } /* end loop over var */
  
  /* Sanity check */
  if(*nbr_var_prc+*nbr_var_fix != nbr_var){
    (void)fprintf(stdout,"%s: ERROR nbr_var_prc+nbr_var_fix != nbr_var\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* fxm: Remove ncap exception when finished with ncap list processing */
  /* fxm: ncpdq processes all variables when packing requested */
  if(*nbr_var_prc == 0 && prg_id != ncap && prg_id != ncpdq){
    (void)fprintf(stdout,"%s: ERROR no variables fit criteria for processing\n",prg_nm_get());
    switch(prg_id){
    case ncap:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain at least one derived field\n",prg_nm_get());
    case ncatted:
      /* Do nothing */
      break;
    case ncbo:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a non-coordinate variable that is not NC_BYTE, NC_UBYTE, NC_CHAR, or NC_STRING in order to perform a binary operation (e.g., subtraction)\n",prg_nm_get());
      break;
    case ncea:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a non-coordinate variable that is not NC_BYTE, NC_UBYTE, NC_CHAR, or NC_STRING\n",prg_nm_get());
      break;
    case ncecat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a non-coordinate variable\n",prg_nm_get());
      break;
    case ncflint:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a variable that is not NC_BYTE, NC_UBYTE, NC_CHAR, or NC_STRING\n",prg_nm_get());
      break;
    case ncks:
      /* Do nothing */
      break;
    case ncpdq:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a variable that shares at least one dimension with the re-order list\n",prg_nm_get());
      break;
    case ncra:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a record variable that is not NC_BYTE, NC_UBYTE, NC_CHAR, or NC_STRING\n",prg_nm_get());
      break;
    case ncrcat:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a record variable which to concatenate\n",prg_nm_get());
      break;
    case ncwa:
      (void)fprintf(stdout,"%s: HINT Extraction list must contain a variable that contains an averaging dimension\n",prg_nm_get());
      break;
    default: nco_dfl_case_prg_id_err(); break;
    } /* end switch */
    nco_exit(EXIT_FAILURE);
  } /* end if */

  /* Free unused space and save pointers in output variables */
  *var_fix_ptr=(var_sct **)nco_realloc(var_fix,*nbr_var_fix*sizeof(var_sct *));
  *var_fix_out_ptr=(var_sct **)nco_realloc(var_fix_out,*nbr_var_fix*sizeof(var_sct *));
  *var_prc_ptr=(var_sct **)nco_realloc(var_prc,*nbr_var_prc*sizeof(var_sct *));
  *var_prc_out_ptr=(var_sct **)nco_realloc(var_prc_out,*nbr_var_prc*sizeof(var_sct *));

} /* end nco_var_lst_dvd */

int /* O [enm] Return code */
nco_var_lst_mrg /* [fnc] Merge two variable lists into same order */
(var_sct *** var_1_ptr, /* I/O [sct] Variable list 1 */
 var_sct *** var_2_ptr, /* I/O [sct] Variable list 2 */
 int * const var_nbr_1, /* I/O [nbr] Number of variables in list 1 */
 int * const var_nbr_2) /* I/O [nbr] Number of variables in list 2 */
{
  /* Purpose: Merge two variable lists into same order

     Routine design is open-ended with maximum flexibility
     Initial functionality will simply sort list two into list one order and
     destroy original (un-merged) list two on output
     Refinements could include changing number of variables in each list
     This would allow asymmetric list merges

     Routine is only used by ncbo to synchronize processed variable list
     Until 20070628, routine did not change var_nbr_2
     As of 20070628 (NCO 3.9.1) routine shrinks var_nbr_2 to var_nbr_1
     In effect this sets nbr_var_prc_2:=nbr_var_prc_1 in ncbo
     This allows file_2 to contain variables not in file_1
     Routine now warns about processed variable list "truncation" 
     "asymmetric" list processing should now work iff lst_2 is superset of lst_1

     Until 20070707 lst_2 had to be superset of lst_1
     Next desired functionality is to copy variables only in lst_1 or lst_2 as
     fixed variables to output file, i.e., do not subtract them.
     May only be possible with lst_1 variables
     At minimum, want to print which variables are _only_ in which file
     This would make it easier for user to give -x -v var_lst argument which
     would make files subtractable */

  const char fnc_nm[]="nco_var_lst_mrg()"; /* [sng] Function name */

  int idx_1;
  int idx_2;
  int rcd=0; /* [rcd] Return code */

  var_sct **var_1;
  var_sct **var_2;
  var_sct **var_out;

  var_1=*var_1_ptr;
  var_2=*var_2_ptr;

  var_out=(var_sct **)nco_malloc(NC_MAX_VARS*sizeof(var_sct *));

  /* ...For each variable in first list... */
  for(idx_1=0;idx_1<*var_nbr_1;idx_1++){
    /* ...search through second list... */
    for(idx_2=0;idx_2<*var_nbr_2;idx_2++){
      /* ...until variable with same name is found... */
      if(!strcmp(var_1[idx_1]->nm,var_2[idx_2]->nm)) break; /* ...then search no further... */
    } /* end loop over idx_2 */
    /* ...and if variable was not found in second list... */
    if(idx_2 == *var_nbr_2){
      (void)fprintf(stderr,"%s: ERROR %s variable \"%s\" is in file one and not in file two, i.e., the user is attempting to difference incommensurate sets of variables. %s allows the second file to have more process-able (e.g., differencable) variables than the first file, but disallows the reverse. All process-able variables in the first file must be in the second file (or manually excluded from the operation with the '-x' switch).\n",prg_nm_get(),fnc_nm,var_1[idx_1]->nm,prg_nm_get());
      nco_exit(EXIT_FAILURE);
    } /* end if variable was not found in second list */
    /* ...otherwise assign variable to correct slot in output list */
    var_out[idx_1]=var_2[idx_2];
  } /* end loop over idx_1 */

  /* Asymmetric lists */
  if(*var_nbr_2 > *var_nbr_1){
    if(dbg_lvl_get() > nco_dbg_quiet){
      const int orphan_nbr=*var_nbr_2-*var_nbr_1;
      int orphan_idx=0;
      (void)fprintf(stderr,"%s: INFO %s detects that file two contains %d more \"process-able\" (e.g., difference-able) variable%s than file one. Processable variables exclude those (often coordinates) that are intended to pass through an operator unchanged. The following variable%s present and/or process-able only in file two: ",prg_nm_get(),fnc_nm,orphan_nbr,(orphan_nbr > 1) ? "s" : "",(orphan_nbr > 1) ? "s are" : " is");
      for(idx_2=0;idx_2<*var_nbr_2;idx_2++){ 
	for(idx_1=0;idx_1<*var_nbr_1;idx_1++)
	  if(!strcmp(var_out[idx_1]->nm,var_2[idx_2]->nm)) break;
	/* Print name of variable in list two and not in var_out */  
	if(idx_1 == *var_nbr_1){
	  orphan_idx++;
	  (void)fprintf(stderr,"%s%s",var_2[idx_2]->nm,(orphan_idx < orphan_nbr) ? ", " : ".");
	} /* end if orphan */
      } /* end loop over idx_2 */ 
      (void)fprintf(stderr," If %s in file one then this notice may be safely ignored. Otherwise, %s will do no harm and will not appear in the output file.\n",(orphan_nbr > 1) ? "these variables are all scalar averages of the coordinate variables with the same names" : "this variable is a scalar-average of the coordinate variable with the same name",(orphan_nbr > 1) ? "these variables appear to be orphans. They" : "this variable appears to be an orphan. It");
    } /* endif dbg */
    *var_nbr_2=*var_nbr_1;
  } /* end if asymmetric and debug */

  /* Free un-merged list before overwriting with merged list */
  var_2=(var_sct **)nco_free(var_2);
  *var_2_ptr=(var_sct **)nco_realloc(var_out,*var_nbr_2*sizeof(var_sct *));

  return rcd;
} /* end nco_var_lst_mrg() */

