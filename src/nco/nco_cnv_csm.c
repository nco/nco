/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.c,v 1.100 2014-02-13 21:19:01 pvicente Exp $ */

/* Purpose: CCM/CCSM/CF conventions */

/* Copyright (C) 1995--2014 Charlie Zender
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
      if(nco_dbg_lvl_get() >= nco_dbg_fl && nco_is_rth_opr(nco_prg_id_get())) (void)fprintf(stderr,"%s: INFO NCO has a unified (though incomplete) treatment of many related (official and unoffical) conventions including the older CCM and CCSM and newer CF conventions. To adhere to these conventions, NCO implements variable-specific exceptions in certain operators, e.g., ncbo will not subtract variables named \"date\" or \"gw\", and many operators will always leave coordinate variables unchanged. The full list of exceptions is in the manual http://nco.sf.net/nco.html#CF\n",nco_prg_nm_get());
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
 const int nbr_var,                  /* I [nbr] Number of variables to be defined */
 dmn_sct * const * const dim,        /* I [sct] Dimensions over which to reduce variable */
 const int nbr_dim,                  /* I [sct] Number of dimensions to reduce variable over */
 const int nco_op_typ,               /* I [enm] Operation type, default is average */
 gpe_sct *gpe,                       /* [sng] Group Path Editing (GPE) structure */
 const trv_tbl_sct * const trv_tbl)  /* I [sct] Traversal table */
{
  /* Purpose: Add/modify cell_methods attribute according to CF convention
  http://cf-pcmdi.llnl.gov/documents/cf-conventions/1.7-draft1/cf-conventions.html#cell-methods */

  const char fnc_nm[]="nco_cnv_cf_cll_mth_add()"; /* [sng] Function name */

  aed_sct aed;                  /* [sct] Structure containing information necessary to edit */

  char att_val[NC_MAX_NAME];    /* [sng] Final value of attribute (e.g., "time: mean") */
  char att_op_sng[NC_MAX_NAME]; /* [sng] Operation type (e.g. nco_op_avg translates to "mean") */
  char val1[NC_MAX_NAME];       /* [sng] Value of attribute */
  char val2[NC_MAX_NAME];       /* [sng] Value of attribute */

  int grp_out_id;               /* [ID] Group ID (output) */
  int var_out_id;               /* [ID] Variable ID (output) */
  int rcd=NC_NOERR;             /* [rcd] Return code */
  int nco_op_typ_lcl;           /* [enm] Operation type, default is average */

  long att_sz;                  /* [nbr] Attribute size */

  nc_type att_typ;              /* [nbr] Attribute type */

  nco_bool att_xst;             /* [nbr] Attribute "exists" */

  /* cell_methods attribute values and description

  point	: The data values are representative of points in space or time (instantaneous). 
  sum	  : The data values are representative of a sum or accumulation over the cell. 
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

  /* Initialize common members */
  aed.att_nm=strdup("cell_methods");
  aed.var_nm=NULL;
  aed.type=NC_CHAR;

  /* Process all variables */
  for(int idx_var=0;idx_var<nbr_var;idx_var++){ 
    char *grp_out_fll=NULL; /* [sng] Group name */
    trv_sct *var_trv=NULL;  /* [sct] Variable GTT object */

    /* Obtain variable GTT object using full variable name */
    var_trv=trv_tbl_var_nm_fll(var[idx_var]->nm_fll,trv_tbl);

    /* Edit group name for output */
    if(gpe) grp_out_fll=nco_gpe_evl(gpe,var_trv->grp_nm_fll); else grp_out_fll=(char *)strdup(var_trv->grp_nm_fll);

    /* Obtain output group ID using full group name */
    (void)nco_inq_grp_full_ncid(out_id,grp_out_fll,&grp_out_id);

    /* Memory management after current extracted group */
    if(grp_out_fll) grp_out_fll=(char *)nco_free(grp_out_fll);

    /* Get variable ID */
    (void)nco_inq_varid(grp_out_id,var_trv->nm,&var_out_id);

    /* Build operation type string (e.g. nco_op_avg translates to "mean") */

    /* Initialize values */
    att_op_sng[0]='\0';
    val1[0]='\0';

    /* Preserve rule to always return averages (never extrema or other statistics) of coordinates */
    if(var[idx_var]->is_crd_var) nco_op_typ_lcl=nco_op_avg; else nco_op_typ_lcl=nco_op_typ;
    switch(nco_op_typ_lcl){
      /* Next four operations are defined in CF Conventions */
    case nco_op_avg:               /* nco_op_avg,  Average */
      strcpy(att_op_sng,"mean");  
      break;
    case nco_op_min:               /* nco_op_min,  Minimum value */
      strcpy(att_op_sng,"minimum"); 
      break;
    case nco_op_max:               /* nco_op_max, Maximum value */
      strcpy(att_op_sng,"maximum"); 
      break;
    case nco_op_ttl:               /* nco_op_ttl,  Linear sum */
      strcpy(att_op_sng,"sum"); 
      break;
      /* Remaining operations are supported by NCO but are not in CF Conventions */
    case nco_op_sqravg:            /* nco_op_sqravg,  Square of mean */          
      strcpy(att_op_sng,"sqravg"); 
      break;
    case nco_op_avgsqr:            /* nco_op_avgsqr, Mean of sum of squares */      
      strcpy(att_op_sng,"avgsqr"); 
      break;
    case nco_op_sqrt:              /* nco_op_sqrt,  Square root of mean  */      
      strcpy(att_op_sng,"sqrt"); 
      break;
    case nco_op_rms:               /* nco_op_rms,  Root-mean-square (normalized by N) */     
      strcpy(att_op_sng,"rms"); 
      break;
    case nco_op_rmssdn:            /* nco_op_rmssdn, Root-mean square normalized by N-1 */
      strcpy(att_op_sng,"rmssdn"); 
      break;
    case nco_op_nil:               /* nco_op_nil  Nil or undefined operation type */    
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s variable <%s> Cell method not implemented for operation %d\n",nco_prg_nm_get(),fnc_nm,var_trv->nm_fll,nco_op_typ);
      continue;
    } /* End switch */
    
    char *sng;             /* [sng] String */
    char *ptr_chr;         /* [sng] Pointer to character in string */

    int nbr_dmn_chr;       /* [nbr] Number of dimension separator ',' characters in string */
    int nbr_op_chr;        /* [nbr] Number of operation separator ':' characters in string */
    int len_dmn=0;         /* [nbr] Lenght of possible dimension names string (e.g 'time, lon' ) */
    int nbr_dmn_add=0;     /* [nbr] Number of possible dimension names (e.g 'time, lon' ) that were added  */

    sng_pth_sct** sng_lst; /* [sct] Parse dimensions */
   
    cell_methods_sct *cm;  /* [sct] Cell methods  */
    
    int idx_add=0;         /* [nbr] Index to cell methods array */

    /* Arrays (nco_realloc) must be initialized to NULL */
    cm=NULL;

    /* Inquire if "cell_methods" attribute exists */
    rcd=nco_inq_att_flg(grp_out_id,var_out_id,"cell_methods",&att_typ,&att_sz);

    /* Set "exists" flag */
    if(rcd == NC_NOERR) att_xst=True; else att_xst=False;

    /* Attribute mode is always create. If existing attribute, it has to be parsed for dimensions and op types and re-created */
    aed.mode=aed_create;

    /* Get attribute if it exists */
    if(att_xst == True){

      (void)nco_get_att(grp_out_id,var_out_id,"cell_methods",(void *)val1,NC_CHAR);  

      /* netCDF requires to manually terminate string */ 
      val1[att_sz]='\0';

      /* Parse attribute */

      /* Get number of operation separators ':' in string */
      nbr_op_chr=nco_get_sng_chr_cnt(val1,':'); 

      /* Separator ':' found */
      if (nbr_op_chr == 1){

        ptr_chr=strchr(val1,':');

        size_t len=ptr_chr-val1;
        sng=(char *)nco_malloc(len+1);
        memcpy(sng,val1,len);
        sng[len]='\0';

        /* Get number of dimension separators ',' in string */
        nbr_dmn_chr=nco_get_sng_chr_cnt(sng,','); 

        /* More than 1 dimension found (separated by ',') */
        if(nbr_dmn_chr){

          /* Alloc */
          sng_lst=(sng_pth_sct **)nco_malloc(nbr_dmn_chr*sizeof(sng_pth_sct *)); 

          /* Get tokens */
          (void)nco_get_sng_pth_sct(sng,&sng_lst); 

          /* Transfer to cell_methods array */
          for(int idx_dmn=0;idx_dmn<nbr_dmn_chr;idx_dmn++){

          } /* Transfer to cell_methods arrays */

        } else { /* Just 1 dimension found  */




        } /* Just 1 dimension found  */



        sng=(char *)nco_free(sng);
      } /* Separator ':' found */

    } /* Get attribute if it exists */


    

    /* Initialize values */
    aed.val.cp=NULL;
    val2[0]='\0';
    att_val[0]='\0';
    aed.sz=-1L;
    aed.id=-1;

    /* Build attribute and write */

    /* Cell methods format: string attribute comprising a list of blank-separated words of the form "name: method" */

    /* STEP 1: build list of dimensions string, by looping and matching dimensions  */

    /* Loop variable dimensions */
    for(int idx_dmn_var=0;idx_dmn_var<var_trv->nbr_dmn;idx_dmn_var++){
      /* Loop input dimensions (ncwa -a or ncra records) */
      for(int idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
        /* Match name (variable dimension with input dimension) */
        if(!strcmp(var_trv->var_dmn[idx_dmn_var].dmn_nm,dim[idx_dmn]->nm)){ 

          /* Add space for name */
          len_dmn+=strlen(var_trv->var_dmn[idx_dmn_var].dmn_nm);
          nbr_dmn_add++;

        }  /* Match name (variable dimension with input dimension) */
      } /* Loop input dimensions (ncwa -a or ncra records) */
    }/* Loop variable dimensions */

    /* Add space for ", ", 2 characters times number of names found less 1 name */ 
    if(nbr_dmn_add>1){
      len_dmn+=2*(nbr_dmn_add-1);
    }

    /* Resize cm with the number of dimensions found */

    cm=(cell_methods_sct *)nco_realloc(cm,nbr_dmn_add*sizeof(cell_methods_sct));
  

    /* Loop variable dimensions */
    for(int idx_dmn_var=0;idx_dmn_var<var_trv->nbr_dmn;idx_dmn_var++){
      /* Loop input dimensions (ncwa -a or ncra records) */
      for(int idx_dmn=0;idx_dmn<nbr_dim;idx_dmn++){
        /* Match name (variable dimension with input dimension) */
        if(!strcmp(var_trv->var_dmn[idx_dmn_var].dmn_nm,dim[idx_dmn]->nm)){ 

          /* Concatenate name */ 
          strcat(att_val,var_trv->var_dmn[idx_dmn_var].dmn_nm);

          /* Concatenate ', ' only ( only up to last name ) */ 
          if(nbr_dmn_add>1){
            if (idx_dmn_var<nbr_dmn_add){
              strcat(att_val,", ");
            }
          }

          cm[idx_add].dmn_nm=strdup(var_trv->var_dmn[idx_dmn_var].dmn_nm);
          cm[idx_add].op_type=nco_op_typ_lcl;
          idx_add++;

        } /*  Match name */
      } /* Loop dimensions */
    } /* Loop variable dimensions */

    /* STEP 2: Add operation type to string  */

    /* Add operation type to string */
    strcat(att_val,": ");
    strcat(att_val,att_op_sng);




    /* Type is NC_CHAR */
    aed.val.cp=(char *)strdup(att_val);
    aed.sz=strlen(att_val);

    /* Compare current attribute with new value */
    int cmp;
    cmp=strcmp(val1,att_val);

    /* Edit attribute  */
    (void)nco_aed_prc(grp_out_id,var_out_id,aed);

    /* Get attribute */
    (void)nco_get_att(grp_out_id,var_out_id,"cell_methods",(void *)val2,NC_CHAR);
    val2[aed.sz]='\0';

    if(nco_dbg_lvl_get() >= nco_dbg_dev){
      (void)fprintf(stdout,"%s: DEBUG %s <%s> att_val = '%s'\n",nco_prg_nm_get(),fnc_nm,
        var_trv->nm_fll,att_val);
    }

    /* Delete current value */
    if(aed.val.cp) aed.val.cp=(char *)nco_free(aed.val.cp);
    aed.sz=-1L;


    for(int idx=0;idx<nbr_dmn_add;idx++) cm[idx].dmn_nm=(char *)nco_free(cm[idx].dmn_nm);

  } /* Process all variables */

  aed.att_nm=(char *)nco_free(aed.att_nm);

  return 0;

} /* end nco_cnv_cf_cll_mth_add() */


int
nco_get_sng_chr_cnt                   /* [fnc] Get number of specified characters in a string  */
(char * const sng,                    /* I [sng] String  */
 const char chr)                      /* I [sng] Character */
{
  char *ptr_chr;   /* [sng] Pointer to character in string */
  int nbr_chr=0;   /* [nbr] Number of characterrs in string  */
  int psn_chr;     /* [nbr] Position of character in string */

  ptr_chr=strchr(sng,chr);
  while(ptr_chr){
    psn_chr=ptr_chr-sng;

    if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout," ::found at %d",psn_chr);

    ptr_chr=strchr(ptr_chr+1,chr);

    nbr_chr++;
  } /* end while */

  if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"\n");
  return nbr_chr;

} /* nco_get_sng_chr_cnt() */


int
nco_get_sng_sct                       /* [fnc] Get token structure  */
(char * const sng,                    /* I [sng] String  */ 
 sng_pth_sct ***str_pth_lst)          /* I/O [sct] List of components  */    
{
  /* Purpose: Break string into components separated by ',' character 

  strtok()
  A sequence of calls to this function split str into tokens, which are sequences of contiguous characters 
  separated by any of the characters that are part of delimiters.

  strchr() is used to get position of separator that corresponsds to each token

  Usage:

  Get number of tokens in string
  nbr_chr=nco_get_sng_chr_cnt(sng); 

  Alloc
  sng_lst=(sng_pth_sct **)nco_malloc(nbr_chr*sizeof(sng_pth_sct *)); 

  Get tokens 
  (void)nco_get_sng_pth_sct(sng,&sng_lst); */

  char *ptr_chr;      /* [sng] Pointer to character '/' in full name */
  char *ptr_chr_tok;  /* [sng] Pointer to character */
  int nbr_sls_chr=0;  /* [nbr] Number of of slash characterrs in  string path */
  int psn_chr;        /* [nbr] Position of character '/' in in full name */

  /* Duplicate original, since strtok() changes it */
  char *str=strdup(sng);

  if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"Splitting \"%s\" into tokens:\n",str);

  /* Get first token */
  ptr_chr_tok=strtok (str,",");

  ptr_chr=strchr(sng,',');

  while(ptr_chr){
    if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stdout,"#%s ",ptr_chr_tok);

    psn_chr=ptr_chr-sng;

    /* Store token and position */
    (*str_pth_lst)[nbr_sls_chr]=(sng_pth_sct *)nco_malloc(1*sizeof(sng_pth_sct));

    (*str_pth_lst)[nbr_sls_chr]->nm=strdup(ptr_chr_tok);
    (*str_pth_lst)[nbr_sls_chr]->psn=psn_chr;

    /* Point where last token was found is kept internally by function */
    ptr_chr_tok=strtok(NULL,",");

    ptr_chr=strchr(ptr_chr+1,',');

    nbr_sls_chr++;   
  } /* end while */

  if(nco_dbg_lvl_get() >= nco_dbg_dev)(void)fprintf(stdout,"\n");

  str=(char *)nco_free(str);

  return nbr_sls_chr;

} /* nco_get_sng_sct() */


nco_bool                              
nco_nm_lst_flg                         /* [fnc] Utility function to detect inserted names in a name list */
(const char * const nm,                /* I [sng] A name to detect */
 const nm_tbl_sct *nm_lst)             /* I [sct] List of names   */
{
  /* Loop constructed array to see if already inserted */
  for(int idx=0;idx<nm_lst->nbr;idx++){
    /* Match */
    if(strcmp(nm_lst->lst[idx].nm,nm) == 0){
      /* Mark as inserted in array */
      return True;
    }  /* Match */
  } /* Loop constructed array to see if already inserted  */

  return False;

} /* nco_nm_lst_flg() */



void                          
nco_nm_lst_ins                         /* [fnc] Check if name is on a list of names  */
(const char * const nm,                /* I [sng] Name to find */
 nm_tbl_sct **nm_lst)                  /* I/O [sct] List of names   */
{

  nco_bool flg_ins_lst=True;      /* [flg] Insert new name in array */  

  int nbr_nm=(*nm_lst)->nbr;

  /* Loop input names */
  for(int idx=0;idx<nbr_nm;idx++){

    nco_bool flg_ins;      /* [flg] Is name already inserted? */

    flg_ins=nco_nm_lst_flg(nm,*nm_lst);
    /* Insert in list */
    if (flg_ins == False){

      (*nm_lst)->lst=(nm_sct *)nco_realloc((*nm_lst)->lst,(nbr_nm+1)*sizeof(nm_sct));
      (*nm_lst)->nbr++;
      (*nm_lst)->lst[nbr_nm].nm=strdup(nm);
      return;
    } /* Insert in list */

    /* Do not insert new name */
    flg_ins_lst=False;
  } /* Loop input names */

  if (flg_ins_lst){
    (*nm_lst)->lst=(nm_sct *)nco_realloc((*nm_lst)->lst,(nbr_nm+1)*sizeof(nm_sct));
    (*nm_lst)->nbr++;
    (*nm_lst)->lst[nbr_nm].nm=strdup(nm);
  }

} /* nco_nm_lst_ins() */

