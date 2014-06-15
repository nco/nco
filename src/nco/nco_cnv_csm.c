/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.c,v 1.114 2014-06-15 21:06:22 zender Exp $ */

/* Purpose: CCM/CCSM/CF conventions */

/* Copyright (C) 1995--2014 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You can redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_cnv_csm.h" /* CCM/CCSM/CF conventions */

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
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1);
    (void)nco_get_att(nc_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    /* CCM3, CCSM1 conventions */
    if(strstr(att_val,"NCAR-CSM")) CNV_CCM_CCSM_CF=True; /* Backwards compatibility */
    /* Climate-Forecast conventions */
    if(strstr(att_val,"CF-1.")) CNV_CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    /* As of 20060514, CLM 3.0 uses CF1.0 not CF-1.0 (CAM gets it right) */
    if(strstr(att_val,"CF1.")) CNV_CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    if(CNV_CCM_CCSM_CF && nco_dbg_lvl_get() >= nco_dbg_std){
      (void)fprintf(stderr,"%s: CONVENTION File \"%s\" attribute is \"%s\"\n",nco_prg_nm_get(),cnv_sng,att_val);
      if(cnv_sng == cnv_sng_LC) (void)fprintf(stderr,"%s: WARNING: This file uses a non-standard attribute (\"%s\") to indicate the netCDF convention. The correct attribute is \"%s\".\n",nco_prg_nm_get(),cnv_sng_LC,cnv_sng_UC);
      /* Only warn in arithmetic operators where conventions change behavior */
      if(nco_dbg_lvl_get() >= nco_dbg_fl && nco_dbg_lvl_get() != nco_dbg_dev && nco_is_rth_opr(nco_prg_id_get())) (void)fprintf(stderr,"%s: INFO NCO has a unified (though incomplete) treatment of many related (official and unoffical) conventions including the older CCM and CCSM and newer CF conventions. To adhere to these conventions, NCO implements variable-specific exceptions in certain operators, e.g., ncbo will not subtract variables named \"date\" or \"gw\", and many operators will always leave coordinate variables unchanged. The full list of exceptions is in the manual http://nco.sf.net/nco.html#CF\n",nco_prg_nm_get());
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

int                                  /* [rcd] Return code */
nco_cnv_cf_cll_mth_add               /* [fnc] Add cell_methods attributes */
(const int out_id,                   /* I [id] netCDF file ID */
 var_sct * const * const var,        /* I [sct] Variable to reduce (e.g., average) (destroyed) */
 const int var_nbr,                  /* I [nbr] Number of variables to be defined */
 dmn_sct * const * const dmn_rdc,        /* I [sct] Dimensions over which to reduce variable */
 const int dmn_nbr_rdc,                  /* I [sct] Number of dimensions to reduce variable over */
 const int nco_op_typ,               /* I [enm] Operation type, default is average */
 gpe_sct *gpe,                       /* [sng] Group Path Editing (GPE) structure */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  /* Purpose: Add/modify cell_methods attribute according to CF convention
     http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.7-draft1/cf-conventions.html#cell-methods
     
     cell_methods values and description:
     point	: Data values are representative of points in space or time (instantaneous)
     sum	: Data values are representative of a sum or accumulation over the cell
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
     sqravg Square of the mean
     avgsqr Mean of sum of squares
     max Maximium value
     min Minimium value
     rms Root-mean-square (normalized by N)
     rmssdn Root-mean square (normalized by N-1)
     sqrt Square root of the mean
     ttl Sum of values */

  const char fnc_nm[]="nco_cnv_cf_cll_mth_add()"; /* [sng] Function name */

  aed_sct aed; /* [sct] Structure containing information necessary to edit */

  char att_op_sng[8]; /* [sng] Operation type (longest is nco_op_max which translates to "maximum") */

  char *att_val_cpy; /* [sng] Copy of attribute */
  char *grp_out_fll=NULL; /* [sng] Group name */

  int *dmn_mch; /* [idx] Indices of dimensions reduced in this variable */

  int dmn_idx_rdc;
  int dmn_idx_var;
  int dmn_nbr_mch; /* [nbr] Number of dimension names to add to cell_methods */
  int grp_out_id; /* [ID] Group ID (output) */
  int nco_op_typ_lcl; /* [enm] Operation type, default is average */
  int rcd=NC_NOERR; /* [rcd] Return code */
  int var_idx;
  int var_out_id; /* [ID] Variable ID (output) */

  nc_type att_typ; /* [nbr] Attribute type */

  trv_sct *var_trv=NULL;  /* [sct] Variable GTT object */

  /* Initialize unchanging structure members */
  aed.att_nm=strdup("cell_methods");
  aed.type=NC_CHAR;

  /* Allocate space for maximum number of matching dimensions */
  dmn_mch=(int *)nco_calloc(dmn_nbr_rdc,sizeof(int));

  /* Process all variables */
  for(var_idx=0;var_idx<var_nbr;var_idx++){ 

    /* Obtain variable GTT object using full variable name */
    var_trv=trv_tbl_var_nm_fll(var[var_idx]->nm_fll,trv_tbl);

    /* Edit group name for output */
    if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);

    /* Obtain output group ID using full group name */
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

    /* Format: blank-separated phrases of form "dmn1[, dmn2[...]]: op_typ" */ 
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

    assert(dmn_nbr_mch != 0);

    /* Preserve rule to always return averages (never extrema or other statistics) of coordinates */
    if(var[var_idx]->is_crd_var) nco_op_typ_lcl=nco_op_avg; else nco_op_typ_lcl=nco_op_typ;
    /* NUL-terminate before concatenation */
    att_op_sng[0]='\0';
    switch(nco_op_typ_lcl){
      /* Next four operations are defined in CF Conventions */
    case nco_op_avg: strcpy(att_op_sng,"mean"); break;
    case nco_op_min: strcpy(att_op_sng,"minimum"); break;
    case nco_op_max: strcpy(att_op_sng,"maximum"); break;
    case nco_op_ttl: strcpy(att_op_sng,"sum"); break;
      /* Remaining operations are supported by NCO yet are not in CF Conventions */
    case nco_op_sqravg: strcpy(att_op_sng,"sqravg"); break; /* Square of mean */
    case nco_op_avgsqr: strcpy(att_op_sng,"avgsqr"); break; /* Mean of sum of squares */
    case nco_op_sqrt: strcpy(att_op_sng,"sqrt"); break; /* Square root of mean */ 
    case nco_op_rms: strcpy(att_op_sng,"rms"); break; /* Root-mean-square (normalized by N) */
    case nco_op_rmssdn: strcpy(att_op_sng,"rmssdn"); break; /* Root-mean square normalized by N-1 */
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

    /* Does variable already have "cell_methods" attribute? */
    rcd=nco_inq_att_flg(grp_out_id,var_out_id,"cell_methods",&att_typ,(long *)NULL);
    if(rcd == NC_NOERR){
      aed.mode=aed_append;
      if(att_typ == NC_STRING) (void)fprintf(stderr,"%s: WARNING %s reports existing cell_methods attribute for variable %s is type NC_STRING. Unpredictable results...\n",nco_prg_nm_get(),fnc_nm,aed.var_nm);
      if(att_typ != NC_STRING && att_typ != NC_CHAR) (void)fprintf(stderr,"%s: WARNING %s reports existing cell_methods attribute for variable %s is type %s. Unpredictable results...\n",nco_prg_nm_get(),fnc_nm,aed.var_nm,nco_typ_sng(att_typ));
      /* Insert space between existing attribute and appended attribute */
      att_val_cpy=(char *)strdup(aed.val.cp);
      aed.val.cp=(char *)nco_realloc(aed.val.cp,(++aed.sz)*sizeof(char));
      aed.val.cp[0]=' ';
      aed.val.cp[1]='\0';
      (void)strcat(aed.val.cp,att_val_cpy);
      if(att_val_cpy) att_val_cpy=(char *)nco_free(att_val_cpy);
    }else{
      aed.mode=aed_create;
    } /* endif attribute exists */

    /* Edit attribute */
    (void)nco_aed_prc(grp_out_id,var_out_id,aed);

    if(aed.val.cp) aed.val.cp=(char *)nco_free(aed.val.cp);

  } /* var_idx>=var_nbr */

  if(aed.att_nm) aed.att_nm=(char *)nco_free(aed.att_nm);
  if(dmn_mch) dmn_mch=(int *)nco_free(dmn_mch);

  return NC_NOERR;

} /* end nco_cnv_cf_cll_mth_add() */

int
nco_rdc_sng_to_op_typ /* [fnc] Convert operation string to integer */
(const char * const att_op_sng) /* [fnc] Operation string */
{           
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
