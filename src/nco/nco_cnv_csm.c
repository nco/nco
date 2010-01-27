/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.c,v 1.47 2010-01-27 09:36:31 zender Exp $ */

/* Purpose: CCM/CCSM/CF conventions */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_cnv_csm.h" /* CCM/CCSM/CF conventions */

nco_bool /* O [flg] File obeys CCM/CCSM/CF conventions */
nco_cnv_ccm_ccsm_cf_inq /* O [fnc] Check if file obeys CCM/CCSM/CF conventions */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* Purpose: Check if file adheres to CCM/CCSM/CF history tape format */

  nco_bool CNV_CCM_CCSM_CF=False;

  char *att_val;
  char *cnv_sng=NULL_CEWI;

  /* netCDF standard is uppercase Conventions, but some models user lowercase */
  char cnv_sng_UC[]="Conventions"; /* Unidata standard string (uppercase) */
  char cnv_sng_LC[]="conventions"; /* Unidata non-standard string (lowercase) */
  
  int rcd; /* [rcd] Return code */
  
  long att_sz;

  nc_type att_typ;

  /* Look for signature of an CCM/CCSM/CF format file */
  cnv_sng=cnv_sng_UC;
  rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd != NC_NOERR){
    /* Some models, e.g., CLM user lowercase "conventions" */
    cnv_sng=cnv_sng_LC;
    rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);

  } /* Re-try with lowercase string */
  
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1);
    (void)nco_get_att(nc_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    /* CCM3, CCSM1 conventions */
    if(strstr(att_val,"NCAR-CSM")) CNV_CCM_CCSM_CF=True; /* Backwards compatibility */
    /* Climate-Forecast conventions */
    if(strstr(att_val,"CF-1.0")) CNV_CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    /* As of 20060514, CLM 3.0 uses CF1.0 not CF-1.0 (CAM gets it right) */
    if(strstr(att_val,"CF1.0")) CNV_CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    if(CNV_CCM_CCSM_CF && dbg_lvl_get() > 0){
      (void)fprintf(stderr,"%s: CONVENTION File \"%s\" attribute is \"%s\"\n",prg_nm_get(),cnv_sng,att_val);
      if(cnv_sng == cnv_sng_LC) (void)fprintf(stderr,"%s: WARNING: This file uses a non-standard attribute (\"%s\") to indicate the netCDF convention. The correct attribute is \"%s\".\n",prg_nm_get(),cnv_sng_LC,cnv_sng_UC);
      /* Only warn in arithmetic operators where conventions change behavior */
      if(dbg_lvl_get() > 1 && nco_is_rth_opr(prg_get())) (void)fprintf(stderr,"%s: INFO NCO has a unified (though incomplete) treatment of many related (official and unoffical) conventions including the older CCM and CCSM and newer CF conventions. To adhere to these conventions, NCO implements variable-specific exceptions in certain operators, e.g., ncbo will not subtract variables named \"date\" or \"gw\". The full list of exceptions is in the manual http://nco.sf.net/nco.html#CF\n",prg_nm_get());
    } /* endif dbg */
    att_val=(char *)nco_free(att_val);
  } /* endif */

  return CNV_CCM_CCSM_CF;
  
} /* end nco_cnv_ccm_ccsm_cf_inq */

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
  
  long day;

  int nbdate_id;
  
  nco_int nbdate;
  nco_int date;
  
  (void)sprintf(wrn_sng,"Most, but not all, CCM/CCSM/CF files which are in CCM format contain the fields \"nbdate\", \"time\", and \"date\". When the \"date\" field is present but either \"nbdate\" or \"time\" is missing, then %s is unable to construct a meaningful average \"date\" to store in the output file. Therefore the \"date\" variable in your output file may be meaningless.\n",prg_nm_get());

  /* Find date variable (NC_INT: current date as 6 digit integer (YYMMDD)) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"date")) break;
  } /* end loop over idx */
  if(idx == nbr_var) return; else date_idx=idx;
  if(var[date_idx]->type != NC_INT) return;
  
  /* Find scalar nbdate variable (NC_INT: base date date as 6 digit integer (YYMMDD)) */
  rcd=nco_inq_varid_flg(nc_id,"nbdate",&nbdate_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING CCM/CCSM/CF convention file output variable list contains \"date\" but not \"nbdate\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  } /* endif */

  { /* begin potential OpenMP critical */
    /* Block is critical/thread-safe for identical/distinct in_id's */
    (void)nco_get_var1(nc_id,nbdate_id,0L,&nbdate,NC_INT);
  } /* end potential OpenMP critical */
  
  /* Find time variable (NC_DOUBLE: current day) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"time")) break;
  } /* end loop over idx */
  if(idx == nbr_var){
    (void)fprintf(stderr,"%s: WARNING CCM/CCSM/CF convention file output variable list contains \"date\" but not \"time\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  }else{
    time_idx=idx;
  } /* endif */
  
  /* Assign current day to averaged day number */
  day=(long)(var[time_idx]->val.dp[0]);
  
  /* Recompute date variable based on new (averaged) day number */
#ifdef USE_FORTRAN_ARITHMETIC
  date=FORTRAN_newdate(&nbdate,&day);
#else /* !USE_FORTRAN_ARITHMETIC */
  date=nco_newdate(nbdate,day);
#endif /* !USE_FORTRAN_ARITHMETIC */
  if(var[date_idx]->val.ip) return; else var[date_idx]->val.ip[0]=date;
  
  return; /* 20050109: fxm added return to void function to squelch erroneous gcc-3.4.2 warning */ 
} /* end nco_cnv_ccm_ccsm_cf_date */

nm_id_sct * /* O [sct] Extraction list */
nco_cnv_cf_crd_add /* [fnc] Add coordinates defined by CF convention */
(const int nc_id, /* I netCDF file ID */
 nm_id_sct *xtr_lst, /* I/O current extraction list (destroyed) */
 int * const nbr_xtr) /* I/O number of variables in current extraction list */
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
  for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
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
	    for(idx_var2=0;idx_var2<*nbr_xtr;idx_var2++){
	      if(crd_id == xtr_lst[idx_var2].id) break;
	    } /* end loop over idx_var2 */
	    if(idx_var2 == *nbr_xtr){
	      /* Add coordinate to list */
	      xtr_lst=(nm_id_sct *)nco_realloc((void *)xtr_lst,(*nbr_xtr+1)*sizeof(nm_id_sct));
	      xtr_lst[*nbr_xtr].nm=(char *)strdup(crd_lst[idx_crd]);
	      xtr_lst[*nbr_xtr].id=crd_id;
	      (*nbr_xtr)++; /* NB: Changes size of current loop! */
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

int /* [rcd] Return code */
nco_cnv_cf_cll_mth_add /* [fnc] Add cell_methods attributes */
(const int nc_id, /* I netCDF file ID */
 var_sct * const * const var, /* I [sct] Variable to reduce (e.g., average) (destroyed) */
 const int nbr_var, /* I [nbr] Number of variables to be defined */
 dmn_sct * const * const dim, /* I [sct] Dimensions over which to reduce variable */
 const int nbr_dim, /* I [sct] Number of dimensions to reduce variable over */
 const int nco_op_typ) /* I [enm] Operation type, default is average */
{
  /* Purpose: Add/modify cell_methods attribute according to CF convention
     http://www.cgd.ucar.edu/cms/eaton/cf-metadata/CF-1.0.html#cell-methods
     http://www.cgd.ucar.edu/cms/eaton/cf-metadata/CF-1.0.html#sub_app */

  const char fnc_nm[]="nco_cnv_cf_cll_mth_add()"; /* [sng] Function name */

  int rcd=NC_NOERR; /* [rcd] Return code */

  rcd=0*(nbr_dim+(int)strlen(fnc_nm)+nc_id+sizeof(var)+nbr_var+sizeof(dim)+nco_op_typ);

  return rcd;
  
} /* end nco_cnv_cf_cll_mth_add() */
