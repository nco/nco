/* $Header$ */

/* Purpose: CCM/CCSM/CF conventions */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_cnv_csm.h" /* CCM/CCSM/CF conventions */

int /* O [rcd] Return code */
nco_clm_nfo_get /* [fnc] Parse clm_nfo arguments and merge into structure */
(const char *clm_nfo_sng, /* I [sng] Climatology information string */
 clm_bnd_sct *cb) /* I/O [sct] Climatology bounds structure */
{
  const char fnc_nm[]="nco_clm_nfo_get()"; /* [sng] Function name */

  const char dlm_sng[]=",";

  char **arg_lst;

  char *msg_sng=NULL_CEWI; /* [sng] Error message */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  int arg_nbr;

  nco_bool NCO_SYNTAX_ERROR=False; /* [flg] Syntax error in hyperslab specification */

  /* Purpose: Parse user-supplied climatology bounds arguments from clm_nfo_sng and 
     merge results into climatolgoy bounds (cb) structure */
  arg_lst=nco_lst_prs_2D(clm_nfo_sng,dlm_sng,&arg_nbr);
  
  /* Check syntax */
  if(arg_nbr < 2){ /* Need more than just dimension name */
    msg_sng=strdup("Climatology information must specify at least six arguments (the first argument is the start year, the second is the end year, etc.)");
    NCO_SYNTAX_ERROR=True;
  }else if(arg_nbr > 7){ /* Too much information */
    msg_sng=strdup("Too many (more than 7) arguments in climatology information string");
    NCO_SYNTAX_ERROR=True;
  }else if(arg_lst[0] == NULL){ /* Start year not specified */
    msg_sng=strdup("Start year not specified");
    NCO_SYNTAX_ERROR=True;
  }else if(arg_nbr == 2 && arg_lst[1] == NULL){ /* End year not specified */
    msg_sng=strdup("End year not specified");
    NCO_SYNTAX_ERROR=True;
  }else if(arg_nbr == 3 && arg_lst[2] == NULL){ /* Start month not specified */
    msg_sng=strdup("Start month not specified");
    NCO_SYNTAX_ERROR=True;
  }else if(arg_nbr == 4 && arg_lst[3] == NULL){ /* End month not specified */
    msg_sng=strdup("End month not specified");
    NCO_SYNTAX_ERROR=True;
  }else if(arg_nbr == 5 && arg_lst[4] == NULL){ /* Timesteps per day not specified */
    msg_sng=strdup("Timesteps per day not specified");
    NCO_SYNTAX_ERROR=True;
  }else if(arg_nbr == 6 && arg_lst[5] == NULL){ /* Units string not specified */
    msg_sng=strdup("Units string not specified");
    NCO_SYNTAX_ERROR=True;
  }else if(arg_nbr == 7 && arg_lst[6] == NULL){ /* Calendar string not specified */
    msg_sng=strdup("Calendar string not specified");
    NCO_SYNTAX_ERROR=True;
  } /* end else */
  
  if(NCO_SYNTAX_ERROR){
    (void)fprintf(stdout,"%s: ERROR parsing climatolgy bounds information from \"%s\": %s\n%s: HINT Conform request to hyperslab documentation at http://nco.sf.net/nco.html#hyp\n",nco_prg_nm_get(),clm_nfo_sng,msg_sng,nco_prg_nm_get());
    msg_sng=(char *)nco_free(msg_sng);
    nco_exit(EXIT_FAILURE);
  } /* !NCO_SYNTAX_ERROR */

  if(arg_lst[0]){
    cb->yr_srt=strtol(arg_lst[0],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[0],"strtol",sng_cnv_rcd);
  } /* !arg_lst[0] */    
  if(arg_lst[1]){
    cb->yr_end=strtol(arg_lst[1],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[1],"strtol",sng_cnv_rcd);
  } /* !arg_lst[1] */    
  if(arg_lst[2]){
    cb->mth_srt=strtol(arg_lst[2],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[2],"strtol",sng_cnv_rcd);
  } /* !arg_lst[2] */    
  if(arg_lst[3]){
    cb->mth_end=strtol(arg_lst[3],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[3],"strtol",sng_cnv_rcd);
  } /* !arg_lst[3] */    
  if(arg_lst[4]){
    cb->tpd=strtol(arg_lst[4],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(arg_lst[4],"strtol",sng_cnv_rcd);
  } /* !arg_lst[4] */    
  //if(arg_lst[5]) cb->unt_val=arg_lst[5];
  //if(arg_lst[6]) cb->cln_val=arg_lst[6];
    
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG %s reports variable climatology bounds structure elements yr_srt=%d, yr_end=%d, mth_srt=%d, mth_end=%d, tpd=%d, unt_val=%s, cln_val=%s\n",nco_prg_nm_get(),fnc_nm,cb->yr_srt,cb->yr_end,cb->mth_srt,cb->mth_end,cb->tpd,cb->unt_val,cb->cln_val);

  return NCO_NOERR;
} /* !nco_clm_nfo_get() */

cnv_sct * /* O [sct] Convention structure */
nco_cnv_ini /* O [fnc] Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* Purpose: Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file
     Based-on and supercedes nco_cnv_ccm_ccsm_cf_inq() */

  // const char fnc_nm[]="nco_cnv_ini()"; /* [sng] Function name */

  char *att_val;
  char *cnv_sng=NULL_CEWI;

  /* netCDF standard is uppercase Conventions, though some models user lowercase */
  char cnv_sng_UC[]="Conventions"; /* Unidata standard     string (uppercase) */
  char cnv_sng_LC[]="conventions"; /* Unidata non-standard string (lowercase) */
  
  cnv_sct *cnv; /* [sct] Conventions structure */

  int rcd; /* [rcd] Return code */
  
  long att_sz;

  nc_type att_typ;

  /* Allocate */
  cnv=(cnv_sct *)nco_malloc(sizeof(cnv_sct));
    
  /* Look for signature of a CCM/CCSM/CF-format file */
  cnv_sng=cnv_sng_UC;
  rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd != NC_NOERR){
    /* Re-try with lowercase string because some models, e.g., CLM, user lowercase "conventions" */
    cnv_sng=cnv_sng_LC;
    rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  } /* endif lowercase */
  
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1L);
    (void)nco_get_att(nc_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    /* CCM3, CCSM1 conventions */
    if(strstr(att_val,"NCAR-CSM")) cnv->CCM_CCSM_CF=True; /* Backwards compatibility */
    /* Climate-Forecast conventions */
    if(strstr(att_val,"CF-1.")) cnv->CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    /* As of 20060514, CLM 3.0 uses CF1.0 not CF-1.0 (CAM gets it right) */
    if(strstr(att_val,"CF1.")) cnv->CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    if(strstr(att_val,"MPAS")) cnv->MPAS=True; /* This works for all known MPAS versions */
    if(strstr(att_val,"Group")) cnv->Group=True; /* Untested */
    cnv->cf_vrs=1.0; /* Untested */
    if(nco_dbg_lvl_get() >= nco_dbg_scl && (cnv->CCM_CCSM_CF || cnv->MPAS)){
      (void)fprintf(stderr,"%s: CONVENTION File \"%s\" attribute is \"%s\"\n",nco_prg_nm_get(),cnv_sng,att_val);
      if(cnv_sng == cnv_sng_LC) (void)fprintf(stderr,"%s: WARNING: This file uses a non-standard attribute (\"%s\") to indicate the netCDF convention. The correct attribute is \"%s\".\n",nco_prg_nm_get(),cnv_sng_LC,cnv_sng_UC);
      /* Only warn in arithmetic operators where conventions change behavior */
      if(nco_dbg_lvl_get() >= nco_dbg_fl && nco_dbg_lvl_get() != nco_dbg_dev && nco_is_rth_opr(nco_prg_id_get())) (void)fprintf(stderr,"%s: INFO NCO attempts to abide by many official and unofficial metadata conventions including ARM, CCM, CCSM, CF, and MPAS. To adhere to these conventions, NCO implements variable-specific exceptions in certain operators, e.g., ncbo will not subtract variables named \"date\" or \"gw\" (for CCM/CCSM files) or \"areaCell\" or \"edgesOnCell\" (for MPAS files), and many operators will always leave coordinate variables unchanged. The full list of exceptions is in the manual http://nco.sf.net/nco.html#CF\n",nco_prg_nm_get());
    } /* endif dbg */
    att_val=(char *)nco_free(att_val);
  } /* endif */

  return cnv;
  
} /* end nco_cnv_ini() */

nco_bool /* O [flg] File obeys CCM/CCSM/CF conventions */
nco_cnv_ccm_ccsm_cf_inq /* O [fnc] Check if file obeys CCM/CCSM/CF conventions */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* Purpose: Check if file adheres to CCM/CCSM/CF history tape format */

  nco_bool CNV_CCM_CCSM_CF=False;

  char *att_val;
  char *cnv_sng=NULL_CEWI;

  /* netCDF standard is uppercase Conventions, though some models user lowercase */
  char cnv_sng_UC[]="Conventions"; /* Unidata standard     string (uppercase) */
  char cnv_sng_LC[]="conventions"; /* Unidata non-standard string (lowercase) */
  
  int rcd; /* [rcd] Return code */
  
  long att_sz;

  nc_type att_typ;

  /* Look for signature of a CCM/CCSM/CF-format file */
  cnv_sng=cnv_sng_UC;
  rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd != NC_NOERR){
    /* Re-try with lowercase string because some models, e.g., CLM, user lowercase "conventions" */
    cnv_sng=cnv_sng_LC;
    rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  } /* endif lowercase */
  
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1L);
    (void)nco_get_att(nc_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    /* CCM3, CCSM1 conventions */
    if(strstr(att_val,"NCAR-CSM")) CNV_CCM_CCSM_CF=True; /* Backwards compatibility */
    /* Climate-Forecast conventions */
    if(strstr(att_val,"CF-1.")) CNV_CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    /* As of 20060514, CLM 3.0 uses CF1.0 not CF-1.0 (CAM gets it right) */
    if(strstr(att_val,"CF1.")) CNV_CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    if(CNV_CCM_CCSM_CF && nco_dbg_lvl_get() >= nco_dbg_scl){
      (void)fprintf(stderr,"%s: CONVENTION File \"%s\" attribute is \"%s\"\n",nco_prg_nm_get(),cnv_sng,att_val);
      if(cnv_sng == cnv_sng_LC) (void)fprintf(stderr,"%s: WARNING: This file uses a non-standard attribute (\"%s\") to indicate the netCDF convention. The correct attribute is \"%s\".\n",nco_prg_nm_get(),cnv_sng_LC,cnv_sng_UC);
      /* Only warn in arithmetic operators where conventions change behavior */
      if(nco_dbg_lvl_get() >= nco_dbg_fl && nco_dbg_lvl_get() != nco_dbg_dev && nco_is_rth_opr(nco_prg_id_get())) (void)fprintf(stderr,"%s: INFO NCO attempts to abide by many official and unofficial metadata conventions including ARM, CCM, CCSM, and CF. To adhere to these conventions, NCO implements variable-specific exceptions in certain operators, e.g., ncbo will not subtract variables named \"date\" or \"gw\", and many operators will always leave coordinate variables unchanged. The full list of exceptions is in the manual http://nco.sf.net/nco.html#CF\n",nco_prg_nm_get());
    } /* endif dbg */
    att_val=(char *)nco_free(att_val);
  } /* endif */

  return CNV_CCM_CCSM_CF;
  
} /* end nco_cnv_ccm_ccsm_cf_inq() */

void
nco_cnv_ccm_ccsm_cf_date /* [fnc] Fix date variable in averaged CCM/CCSM/CF files */
(const int nc_id, /* I [id] netCDF file ID */
 X_CST_PTR_CST_PTR_Y(var_sct,var), /* I/O [sct] Variables in output file */
 const int nbr_var) /* I [nbr] Number of variables in list */
{
  /* Purpose: Fix date variable in averaged CCM/CCSM/CF files */
  char wrn_sng[1000];

  int date_idx;
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int time_idx;
  
  int nbdate_id;
  
  nco_int day;
  nco_int date;
  nco_int nbdate;
  
  (void)sprintf(wrn_sng,"Most, but not all, CCM/CCSM/CF files which are in CCM format contain the fields \"nbdate\", \"time\", and \"date\". When the \"date\" field is present but either \"nbdate\" or \"time\" is missing, then %s is unable to construct a meaningful average \"date\" to store in the output file. Therefore the \"date\" variable in your output file may be meaningless.\n",nco_prg_nm_get());

  /* Find date variable (NC_INT: current date as 6 digit integer (YYMMDD)) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"date")) break;
  } /* end loop over idx */
  if(idx == nbr_var) return; else date_idx=idx;
  // if(var[date_idx]->type != NC_INT) return; /* 20101026 TODO nco998 problem is that "date" type is NC_DOUBLE here */
  
  /* Find scalar nbdate variable (NC_INT: base date date as 6 digit integer (YYMMDD)) */
  rcd=nco_inq_varid_flg(nc_id,"nbdate",&nbdate_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING CCM/CCSM/CF convention file output variable list contains \"date\" but not \"nbdate\"\n",nco_prg_nm_get());
    (void)fprintf(stderr,"%s: %s",nco_prg_nm_get(),wrn_sng);
    return;
  } /* endif */

  { /* begin potential OpenMP critical */
    /* Block is critical/thread-safe for identical/distinct in_id's */
    (void)nco_get_var1(nc_id,nbdate_id,0L,&nbdate,NC_INT);
  } /* end potential OpenMP critical */
  
  /* Find time variable (NC_DOUBLE: current day since nbdate) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"time")) break;
  } /* end loop over idx */
  if(idx == nbr_var){
    (void)fprintf(stderr,"%s: WARNING CCM/CCSM/CF convention file output variable list contains \"date\" and \"nbdate\" yet lacks \"time\"\n",nco_prg_nm_get());
    (void)fprintf(stderr,"%s: %s",nco_prg_nm_get(),wrn_sng);
    return;
  }else{
    time_idx=idx;
  } /* endif */
  
  /* Assign current day to averaged day number */
  day=(nco_int)(var[time_idx]->val.dp[0]);
  
  /* Recompute date variable based on new (averaged) day number */
  date=nco_newdate(nbdate,day);
  if(var[date_idx]->type == NC_INT){
    if(!var[date_idx]->val.ip) return; else var[date_idx]->val.ip[0]=date;
  }else if(var[date_idx]->type == NC_DOUBLE){
    if(!var[date_idx]->val.dp) return; else var[date_idx]->val.dp[0]=date;
  }else{
    (void)fprintf(stderr,"%s: WARNING CCM/CCSM/CF convention file output variable \"date\" is not NC_INT or NC_DOUBLE\n",nco_prg_nm_get());
  } /* end else */
  
  return; /* 20050109: fxm added return to void function to squelch erroneous gcc-3.4.2 warning */ 
} /* end nco_cnv_ccm_ccsm_cf_date */

nm_id_sct * /* O [sct] Extraction list */
nco_cnv_cf_crd_add /* [fnc] Add coordinates defined by CF convention */
(const int nc_id, /* I netCDF file ID */
 nm_id_sct *xtr_lst, /* I/O current extraction list (destroyed) */
 int * const xtr_nbr) /* I/O number of variables in current extraction list */
{
  /* Purpose: Detect coordinates specified by CF convention and add them to extraction list
     http://www.cgd.ucar.edu/cms/eaton/cf-metadata/CF-1.0.html#grid_ex2 */

  const char dlm_sng[]=" "; /* [sng] Delimiter string */
  const char fnc_nm[]="nco_cnv_cf_crd_add()"; /* [sng] Function name */

  char **crd_lst; /* [sng] 1D array of list elements */
  char *att_val;
  char att_nm[NC_MAX_NAME];

  int crd_id;
  int idx_att;
  int idx_crd;
  int idx_var;
  int idx_var2;
  int nbr_att;
  int nbr_crd; /* [nbr] Number of coordinates specified in "coordinates" attribute */
  int rcd=NC_NOERR; /* [rcd] Return code */
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
	  (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for variable %s is type %s, not %s. This violates the CF convention for specifying additional attributes. Therefore %s will skip this attribute.\n",nco_prg_nm_get(),att_nm,xtr_lst[idx_var].nm,nco_typ_sng(att_typ),nco_typ_sng(NC_CHAR),fnc_nm);
	  return xtr_lst;
	} /* end if */
	att_val=(char *)nco_malloc((att_sz+1L)*sizeof(char));
	if(att_sz > 0) (void)nco_get_att(nc_id,var_id,att_nm,(void *)att_val,NC_CHAR);	  
	/* NUL-terminate attribute */
	att_val[att_sz]='\0';
	/* Split list into separate coordinate names */
        /* using nco_lst_prs_sgl_2D() and not nco_lst_prs_2D */
        /* see TODO 944   */
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
	  } /* end if named coordinate exists in input file */
	} /* end loop over idx_crd */
	  /* Free allocated memory */
	att_val=(char *)nco_free(att_val);
	crd_lst=nco_sng_lst_free(crd_lst,nbr_crd);
      } /* !coordinates */
    } /* end loop over attributes */
  } /* end loop over idx_var */
  
  return xtr_lst;
  
} /* end nco_cnv_cf_crd_add() */

int                                  /* O [rcd] Return code */
nco_cnv_cf_cll_mth_add               /* [fnc] Add cell_methods attributes */
(const int out_id,                   /* I [id] netCDF file ID */
 var_sct * const * const var,        /* I [sct] Variable to reduce (e.g., average) (destroyed) */
 const int var_nbr,                  /* I [nbr] Number of variables to be defined */
 dmn_sct * const * const dmn_rdc,    /* I [sct] Dimensions over which to reduce variable */
 const int dmn_nbr_rdc,              /* I [sct] Number of dimensions to reduce variable over */
 const int nco_op_typ,               /* I [enm] Operation type, default is average */
 gpe_sct *gpe,                       /* I [sng] Group Path Editing (GPE) structure */
 const clm_bnd_sct * const cb,       /* I [sct] Climatology bounds structure */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  /* Purpose: Add/modify CF cell_methods attribute
     http://cfconventions.org/Data/cf-conventions/cf-conventions-1.7/build/cf-conventions.html#cell-methods
     
     cell_methods values and description:
     point	: Data values are representative of points in space or time (instantaneous)
     sum	: Data values are representative of a sum or accumulation over the cell
     maximum_absolute_value	: Maximum absolute value
     maximum	: Maximum
     median	: Median
     mid_range	: Average of maximum and minimum
     minimum	: Minimum
     mean	: Mean (average value)
     mode	: Mode (most common value)
     range	: Absolute difference between maximum and minimum
     standard_deviation : Standard deviation
     variance	: Variance
     
     NCO operation types:
     avg Mean value
     avgsqr Mean of sum of squares
     mabs Maximum absolute value
     max Maximum value
     mebs Mean absolute value
     mibs Minimum absolute value
     min Minimum value
     rms Root-mean-square (normalized by N)
     rmssdn Root-mean square (normalized by N-1)
     sqravg Square of the mean
     sqrt Square root of the mean
     ttl Sum of values */

  const char fnc_nm[]="nco_cnv_cf_cll_mth_add()"; /* [sng] Function name */

  aed_sct aed; /* [sct] Attribute-edit information */

  char att_op_sng[23]; /* [sng] Operation type (longest is nco_op_mabs which translates to "maximum_absolute_value") */

  char *att_val=NULL; /* [sng] Coordinates attribute */
  char *att_val_cpy; /* [sng] Copy of attribute */
  char *grp_out_fll=NULL; /* [sng] Group name */
  char *sbs_ptr; /* [sng] Pointer to substring */
  char *cll_mth_clm; /* [sng] Cell methods for climatology */
  
  int *dmn_mch; /* [idx] Indices of dimensions reduced in this variable */

  int dmn_idx_rdc;
  int dmn_idx_var;
  int dmn_nbr_mch; /* [nbr] Number of dimension names to add to cell_methods */
  int grp_out_id; /* [ID] Group ID (output) */
  int nco_op_typ_lcl; /* [enm] Operation type, default is average */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_idx;
  int var_out_id; /* [ID] Variable ID (output) */

  long int att_lng; /* [nbr] Length of attribute string */
  
  nc_type att_typ; /* [nbr] Attribute type */

  nco_bool FIRST_WARNING=True;
  nco_bool flg_dpl; /* [flg] New cell_methods attribute duplicates old */
  nco_bool mlt_dmn_rdc; /* [flg] Multiple dimension reduction flag */

  size_t dmn_sng_lng; /* [nbr] Length of dimension string */
  size_t sbs_sng_lng; /* [nbr] Length of substring */

  trv_sct *var_trv=NULL;  /* [sct] Variable GTT object */

  /* Initialize unchanging structure members */
  aed.att_nm=strdup("cell_methods");
  aed.type=NC_CHAR;

  /* Allocate space for maximum number of matching dimensions */
  dmn_mch=(int *)nco_calloc(dmn_nbr_rdc,sizeof(int));

  if(cb){
    if(cb->tpd > 1) cll_mth_clm=strdup("time: mean within days time: mean within years time: mean over years");
    else if(cb->bnd2clm || cb->clm2clm) cll_mth_clm=strdup("time: mean within years time: mean over years");
    else if(cb->clm2bnd) cll_mth_clm=strdup("time: mean");
    else{
      (void)fprintf(stdout,"%s: ERROR %s reports climatology bounds error with variable %s\n",nco_prg_nm_get(),fnc_nm,var_trv->nm);
      nco_exit(EXIT_FAILURE);
    } /* !cb->tpd */
  } /* !cb */

  /* Process all variables */
  for(var_idx=0;var_idx<var_nbr;var_idx++){ 

    /* Obtain variable GTT object using full variable name */
    var_trv=trv_tbl_var_nm_fll(var[var_idx]->nm_fll,trv_tbl);

    /* Edit group name for output */
    if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);

    /* Obtain output group ID */
    (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);

    /* Memory management after current extracted group */
    if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

    /* Get variable ID */
    (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);

    /* Initialize attribute-edit structure for this variable */
    aed.var_nm=var_trv->nm;
    aed.id=var_out_id;
    aed.sz=0L;
    dmn_nbr_mch=0;
    flg_dpl=False;

    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s reports variable %s has cll_mth_clm = %s\n",nco_prg_nm_get(),fnc_nm,var_trv->nm,cll_mth_clm);

    if(cb){
      /* Does variable use time coordinate? */
      for(dmn_idx_var=0;dmn_idx_var<var_trv->nbr_dmn;dmn_idx_var++)
	if(!strcmp(var_trv->var_dmn[dmn_idx_var].dmn_nm,cb->tm_crd_nm)) break;
      if(dmn_idx_var < var_trv->nbr_dmn){
	/* Stamp with appropriate cell_methods temporal attribute */
	att_val=strdup(cll_mth_clm);
	aed.sz=strlen(att_val);
	aed.type=NC_CHAR;
	aed.val.cp=att_val;
	aed.mode=aed_overwrite;
	/* Do not add cell_methods to time bounds variables, and delete them if they exist */
	if(var_out_id == cb->clm_bnd_id_out || var_out_id == cb->tm_bnd_id_out) aed.mode=aed_delete;
	(void)nco_aed_prc(grp_out_id,var_out_id,aed);
	if(att_val) att_val=(char *)nco_free(att_val);
	continue;
      } /* !dmn_idx_var */
    } /* !cb */

    /* cell_methods format: blank-separated phrases of form "dmn1[, dmn2[...]]: op_typ", e.g., "lat, lon: mean" */ 
    for(dmn_idx_var=0;dmn_idx_var<var_trv->nbr_dmn;dmn_idx_var++){
      for(dmn_idx_rdc=0;dmn_idx_rdc<dmn_nbr_rdc;dmn_idx_rdc++){
        assert(dmn_rdc[dmn_idx_rdc]->nm_fll);
        /* Compare full names */
        if(!strcmp(var_trv->var_dmn[dmn_idx_var].dmn_nm_fll,dmn_rdc[dmn_idx_rdc]->nm_fll)){
          /* Add length of each matching dimension to accumulating attribute size */
          aed.sz+=strlen(dmn_rdc[dmn_idx_rdc]->nm);
          dmn_mch[dmn_nbr_mch++]=dmn_idx_rdc;
        } /* !match */
      } /* dmn_idx_rdc */
    } /* dmn_idx_var */

    assert(dmn_nbr_mch > 0);

    /* Preserve rule to always return averages (never extrema or other statistics) of coordinates */
    if(var[var_idx]->is_crd_var) nco_op_typ_lcl=nco_op_avg; else nco_op_typ_lcl=nco_op_typ;

    /* NUL-terminate before concatenation */
    att_op_sng[0]='\0';
    switch(nco_op_typ_lcl){
      /* Next five operations are defined in CF Conventions */
    case nco_op_avg: strcpy(att_op_sng,"mean"); break;
    case nco_op_min: strcpy(att_op_sng,"minimum"); break;
    case nco_op_max: strcpy(att_op_sng,"maximum"); break;
    case nco_op_ttl: strcpy(att_op_sng,"sum"); break;
    case nco_op_avgsqr: strcpy(att_op_sng,"variance"); break; /* Mean of sum of squares */
      /* Remaining operations are supported by NCO yet are not in CF Conventions */
    case nco_op_mabs: strcpy(att_op_sng,"maximum_absolute_value"); break; /* Maximum absolute value */
    case nco_op_mebs: strcpy(att_op_sng,"mean_absolute_value"); break; /* Mean absolute value */
    case nco_op_mibs: strcpy(att_op_sng,"minimum_absolute_value"); break; /* Minimum absolute value */
    case nco_op_tabs: strcpy(att_op_sng,"sum absolute_value"); break; /* sum  absolute value */
    case nco_op_sqravg: strcpy(att_op_sng,"square_of_mean"); break; /* Square of mean */
    case nco_op_sqrt: strcpy(att_op_sng,"square_root_of_mean"); break; /* Square root of mean */ 
    case nco_op_rms: strcpy(att_op_sng,"root_mean_square"); break; /* Root-mean-square (normalized by N) */
    case nco_op_rmssdn: strcpy(att_op_sng,"root_mean_square_nm1"); break; /* Root-mean square normalized by N-1 */
    case nco_op_nil: /* nco_op_nil, Undefined operation type */
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s reports variable %s cell_method not implemented for operation %d\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,nco_op_typ);
      continue;
    } /* End switch */

    /* Initialize to size of ": " plus length of operation string */
    aed.sz+=2L+strlen(att_op_sng);
    /* Add room for commas and spaces, i.e., "dmn1, dmn2, dmn3" */
    if(dmn_nbr_mch > 1) aed.sz+=2*(dmn_nbr_mch-1);
    /* Add room for NUL-terminator */
    aed.val.cp=(char *)nco_malloc((aed.sz+1L)*sizeof(char));
    aed.val.cp[0]='\0';

    /* Build single string by concatenating known matches */
    for(int dmn_idx_mch=0;dmn_idx_mch<dmn_nbr_mch;dmn_idx_mch++){
      (void)strcat(aed.val.cp,dmn_rdc[dmn_mch[dmn_idx_mch]]->nm);
      if(dmn_idx_mch<dmn_nbr_mch-1) (void)strcat(aed.val.cp,", ");
    } /* dmn_idx_mch>=dmn_nbr_mch */
    (void)strcat(aed.val.cp,": ");
    (void)strcat(aed.val.cp,att_op_sng);

    /* 20150625: Older versions of CAM, e.g., CAM3 and CLM3, used "cell_method" instead of "cell_methods" 
       If old attribute is not deleted then the output file will contain both attributes
       Does variable already have "cell_method" attribute? */
    strcpy(aed.att_nm,"cell_method");
    rcd=nco_inq_att_flg(grp_out_id,var_out_id,aed.att_nm,&att_typ,&att_lng);
    if(rcd == NC_NOERR){
      if(FIRST_WARNING) (void)fprintf(stderr,"%s: WARNING: Variable \"%s\" uses the non-standard attribute name \"cell_method\" instead of \"cell_methods\", the correct attribute name. The CAM3 and CLM3 models (and others?) have this problem. Expect \"double attributes\" in output. This message is printed only once per invocation, although the problem likely occurs in multiple variables.\n",nco_prg_nm_get(),aed.var_nm);
      FIRST_WARNING=False;
    } /* endif attribute exists */

    /* Does variable already have "cell_methods" attribute? */
    strcpy(aed.att_nm,"cell_methods");
    rcd=nco_inq_att_flg(grp_out_id,var_out_id,aed.att_nm,&att_typ,&att_lng);
    if(rcd == NC_NOERR){
      if(att_typ == NC_STRING) (void)fprintf(stderr,"%s: WARNING %s reports existing cell_methods attribute for variable %s is type NC_STRING. Unpredictable results...\n",nco_prg_nm_get(),fnc_nm,aed.var_nm);
      if(att_typ != NC_STRING && att_typ != NC_CHAR) (void)fprintf(stderr,"%s: WARNING %s reports existing cell_methods attribute for variable %s is type %s. Unpredictable results...\n",nco_prg_nm_get(),fnc_nm,aed.var_nm,nco_typ_sng(att_typ));

      /* Often climatologies are multiply-averaged over time
	 NCO's treatment of this has changed with time
	 pre-20140131: 
	 NCO has no special treatment of cell_methods
	 20140131: 
	 First NCO implementation (ncra, ncea, ncwa) of cell_methods with 
	 20150625: 
	 For example, climate model output is often archived as monthly means in each gridcell
	 cell_methods attributes of these monthly data begin as "time: mean" (i.e., monthly mean).
	 We then create a climatology by a sequence of one or two more temporal-averaging steps
	 The one-step method puts all the months in the hopper and averages those
	 Variables in the resultiing file may have cell_methods = "time: mean time: mean"
	 The two-step method first averages the months into four climatological seasons
	 Then it averages those four seasons into the climatological annual mean
	 Variables in the resultiing file may have cell_methods = "time: mean time: mean time: mean"
	 To avoid this redundancy, we check that the new cell_method does not duplicate the old 
	 If it would, then skip adding the new
	 20160418: 
	 Treatment of multiply-time-averaged quantities requires climatology bounds attribute
	 One-step methods (e.g., monthly mean) should have time-bounds attribute
	 cell_methods = "time: mean"
	 Two-step methods (e.g., climatological March) should have climatology-bounds attribute
	 cell_methods = "time: mean within years time: mean over years"
	 Three-step methods (e.g., climatological MAM) should have climatology-bounds attribute
	 cell_methods = "time: mean within years time: mean over years"
	 Four-step methods (e.g., climatological ANN) with one timestep should have time-bounds attribute
	 cell_methods = "time: mean"
	 20200809:
	 http://cfconventions.org/Data/cf-conventions/cf-conventions-1.8/cf-conventions.html#climatological-statistics Example 7.13
	 Two-step methods with multiple timesteps (e.g., climatological diurnal cycle for March) should have climatology-bounds attribute
	 cell_methods = "time: cell_methods="time: mean within days time: mean over days time: mean over years";
	 Three-step methods with multiple timesteps (e.g., climatological diurnal cycle for MAM) should have climatology-bounds attribute
	 cell_methods = "time: cell_methods="time: mean within days time: mean over days time: mean over years";
	 Four-step methods with multiple timesteps (e.g., climatological diurnal cycle for ANN) should have climatology-bounds attribute
	 cell_methods = "time: cell_methods="time: mean within days time: mean over years"; */
      ptr_unn val_old; /* [sng] Old cell_methods attribute */
      val_old.vp=(void *)nco_malloc((att_lng+1L)*sizeof(char));
      (void)nco_get_att(grp_out_id,var_out_id,aed.att_nm,val_old.vp,NC_CHAR);
      val_old.cp[att_lng]='\0';
      if(strstr(val_old.cp,aed.val.cp)) flg_dpl=True;
      if(val_old.vp) val_old.vp=(void *)nco_free(val_old.vp);
      
      aed.mode=aed_append;
      /* Insert space between existing attribute and appended attribute */
      att_val_cpy=(char *)strdup(aed.val.cp);
      /* Add one for space character */
      aed.sz++;
      /* Add one for NUL-terminator */
      aed.val.cp=(char *)nco_realloc(aed.val.cp,(aed.sz+1L)*sizeof(char));
      aed.val.cp[0]=' ';
      aed.val.cp[1]='\0';
      (void)strncat(aed.val.cp,att_val_cpy,aed.sz-1L);
      if(att_val_cpy) att_val_cpy=(char *)nco_free(att_val_cpy);
    }else{ /* !cell_methods attribute already exists */
      aed.mode=aed_create;
    } /* !cell_methods attribute already exists */

    /* Edit attribute */
    if(!flg_dpl) (void)nco_aed_prc(grp_out_id,var_out_id,aed);

    /* 20150308 */
    /* Does variable already have "coordinates" attribute?
       NB: This re-uses att_nm which has only enough space to hold "cell_methods" */
    strcpy(aed.att_nm,"coordinates");
    rcd=nco_inq_att_flg(grp_out_id,var_out_id,aed.att_nm,&att_typ,&att_lng);
    if(rcd == NC_NOERR && att_typ == NC_CHAR){
      /* Remove reduced dimensions from coordinates string
	 coordinates format: blank-separated names of form "dmn1 [dmn2 [...]] dmnN", e.g., "time lat lon" */ 
      /* Add room for NUL-terminator */
      att_val=(char *)nco_malloc((att_lng+1L)*sizeof(char));
      rcd=nco_get_att(grp_out_id,var_out_id,aed.att_nm,att_val,att_typ);
      /* Reset value from previous use */
      aed.val.cp[0]='\0';
      att_val[att_lng]='\0';
      mlt_dmn_rdc=False;
      assert(rcd == NC_NOERR);
      for(dmn_idx_var=0;dmn_idx_var<var_trv->nbr_dmn;dmn_idx_var++){
	for(dmn_idx_rdc=0;dmn_idx_rdc<dmn_nbr_rdc;dmn_idx_rdc++){
	  /* Is reduced dimension in variable? */
	  if(!strcmp(var_trv->var_dmn[dmn_idx_var].dmn_nm_fll,dmn_rdc[dmn_idx_rdc]->nm_fll)){
	    if(mlt_dmn_rdc){
	      /* At least one other dimension has already been reduced/excised
		 Hence multiple dimensions of this variable may be reduced
		 Start next excision from ending point of last excision, not from disk-values */
	      strcpy(att_val,aed.val.cp);
	      att_lng=strlen(aed.val.cp);
	    } /* endif */
	    /* Is dimension in current (possibly locally-modified) "coordinates" attribute? NB: Assume short name not full name */
	    if((sbs_ptr=strstr(att_val,dmn_rdc[dmn_idx_rdc]->nm))){
	      /* Is this the only dimension in "coordinates" attribute? */
	      if(!strcmp(dmn_rdc[dmn_idx_rdc]->nm,att_val)){
		/* Variable will become scalar so delete "coordinates" attribute */
		aed.mode=aed_delete;
	      }else{ /* endif scalar */
		/* Excise dimension from "coordinates" attribute */
		dmn_sng_lng=strlen(dmn_rdc[dmn_idx_rdc]->nm);
		sbs_sng_lng=(size_t)(sbs_ptr-att_val);
		aed.mode=aed_overwrite;
		/* Remove whitespace immediately following excised dimension, i.e., count it as part of dimension string
		   True for all dimensions except final dimension (trailed by a NUL, not a space) */
		if(sbs_ptr[dmn_sng_lng] == ' ') dmn_sng_lng++;
		aed.sz=att_lng-dmn_sng_lng;
		/* Add one for NUL-terminator */
		aed.val.cp=(char *)nco_realloc(aed.val.cp,(aed.sz+1L)*sizeof(char));
		strncpy(aed.val.cp,att_val,sbs_sng_lng);
		aed.val.cp[sbs_sng_lng]='\0';
		strcat(aed.val.cp,sbs_ptr+dmn_sng_lng);
	      } /* endelse scalar */
	      /* Edit attribute */
	      (void)nco_aed_prc(grp_out_id,var_out_id,aed);
	      mlt_dmn_rdc=True;
	    } /* !match attribute */
	  } /* !match variable */
	} /* dmn_idx_rdc */
      } /* dmn_idx_var */
    } /* endif attribute exists */

    if(att_val) att_val=(char *)nco_free(att_val);
    if(aed.val.cp) aed.val.cp=(char *)nco_free(aed.val.cp);

  } /* var_idx >= var_nbr */

  if(aed.att_nm) aed.att_nm=(char *)nco_free(aed.att_nm);
  if(dmn_mch) dmn_mch=(int *)nco_free(dmn_mch);

  return NC_NOERR;
} /* end nco_cnv_cf_cll_mth_add() */

int
nco_rdc_sng_to_op_typ /* [fnc] Convert operation string to integer */
(const char * const att_op_sng) /* [fnc] Operation string */
{           
  if(!strcmp(att_op_sng,"mabs")) return nco_op_mabs;
  if(!strcmp(att_op_sng,"mebs")) return nco_op_mebs;
  if(!strcmp(att_op_sng,"mibs")) return nco_op_mibs;
  if(!strcmp(att_op_sng,"tabs")) return nco_op_tabs;
  if(!strcmp(att_op_sng,"mean")) return nco_op_avg;
  if(!strcmp(att_op_sng,"minimum")) return nco_op_min;
  if(!strcmp(att_op_sng,"maximum")) return nco_op_max;
  if(!strcmp(att_op_sng,"sum")) return nco_op_ttl;
  if(!strcmp(att_op_sng,"sqravg")) return nco_op_sqravg;
  if(!strcmp(att_op_sng,"avgsqr")) return nco_op_avgsqr;
  if(!strcmp(att_op_sng,"sqrt")) return nco_op_sqrt;
  if(!strcmp(att_op_sng,"rms")) return nco_op_rms;
  if(!strcmp(att_op_sng,"rmssdn")) return nco_op_rmssdn;

  return False; /* Statement should not be reached */
} /* nco_rdc_sng_to_op_typ() */

const char * /* O [sng] String describing operation type */
nco_op_typ_to_rdc_sng /* [fnc] Convert operation type to string */
(const int nco_op_typ) /* I [enm] Operation type */
{           
  switch(nco_op_typ){
  case nco_op_avg: return "mean";
  case nco_op_mabs: return "mabs";
  case nco_op_mebs: return "mebs";
  case nco_op_mibs: return "mibs";
  case nco_op_tabs: return "tabs";
  case nco_op_min: return "minimum";
  case nco_op_max: return "maximum";
  case nco_op_ttl: return "sum";
  case nco_op_sqravg: return "sqravg";
  case nco_op_avgsqr: return "avgsqr";
  case nco_op_sqrt: return "sqrt";
  case nco_op_rms: return "rms";
  case nco_op_rmssdn: return "rmssdn";
  default: 
    nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  return False; /* Statement should not be reached */
} /* nco_op_typ_to_rdc_sng() */
