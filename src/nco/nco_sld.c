/* $Header$ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   ncks -4 -O -C -v ppc_dbl --ppc /ppc_dbl=3 ~/nco/data/in.nc ~/foo.nc */

#include "nco_sld.h" /* Swath-Like Data */

kvmap_sct nco_sng2map /* [fnc] Parse string into key-value pair */
(char *sng, /* I [sng] String to parse, including "=" */
 kvmap_sct kvm) /* O [sct] key-value pair */
{
  char *prt;

  int icnt=0;

  prt=strtok(sng,"=");
  while(prt){
    icnt++;
    nco_sng_strip(prt);
    switch(icnt){
    case 1:
      kvm.key=strdup(prt);
      break;
    case 2:
      kvm.value=strdup(prt);
      break;
    default:
      (void)fprintf(stderr,"nco_sng2map() cannot get key-value pair from input: %s\n",sng);
      break;
    }/* end switch */
    prt=strtok(NULL,"=");
  }/* end while */
  return kvm;
} /* end nco_sng2map() */

void
nco_ppc_att_prc /* [fnc] Create PPC attribute */
(const int nc_id, /* I [id] Input netCDF file ID */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* NB: Can fail when output file has fewer variables than input file (e.g., file was subsetted)
     20150126: Deprecated, debugged and functionality moved to nco_xtr_dfn() */
  aed_sct aed;
  char att_nm_dsd[]="least_significant_digit";
  char att_nm_nsd[]="number_of_significant_digits";
  int grp_id; /* [id] Group ID */
  int var_id; /* [id] Variable ID */
  int ppc;
  int rcd=NC_NOERR;
  long att_sz;
  nc_type att_typ;
  ptr_unn att_val;
  int ppc_xst;

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    ppc=trv_tbl->lst[idx_tbl].ppc;
    if(ppc == NC_MAX_INT) continue;
    trv_sct var_trv=trv_tbl->lst[idx_tbl];
    aed.var_nm=strdup(var_trv.nm);
    (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id); /* Obtain group ID */
    (void)nco_inq_varid(grp_id,var_trv.nm,&var_id); /* Obtain variable ID */
    att_val.ip=&ppc;
    aed.id=var_id;
    aed.val=att_val;
    if(var_trv.flg_nsd) aed.att_nm=att_nm_nsd; else aed.att_nm=att_nm_dsd;
    aed.type=NC_INT; /* NB: Buggy: value changes if it is assigned outside for loop */
    aed.sz=1L;
    aed.mode=aed_create; 
    rcd=nco_inq_att_flg(nc_id,var_id,aed.att_nm,&att_typ,&att_sz);
    if(rcd == NC_NOERR && aed.sz == att_sz && aed.type == att_typ){
      (void)nco_get_att(nc_id,var_id,aed.att_nm,&ppc_xst,att_typ);
      if(ppc < ppc_xst) aed.mode=aed_overwrite;
      else continue; /* no changes needed */
    } /* endif */
    (void)nco_aed_prc(nc_id,var_id,aed);
  } /* end loop */
} /* end nco_ppc_att_prc() */

void
nco_ppc_ini /* Set PPC based on user specifications */
(const int nc_id, /* I [id] netCDF input file ID */
 int *dfl_lvl, /* O [enm] Deflate level */
 const int fl_out_fmt,  /* I [enm] Output file format */
 char * const ppc_arg[], /* I [sng] List of user-specified PPC */
 const int ppc_arg_nbr, /* I [nbr] Number of PPC specified */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  int ppc_arg_idx; /* [idx] Index over ppr_arg (i.e., separate invocations of "--ppc var1[,var2]=val") */
  int ppc_var_idx; /* [idx] Index over ppr_lst (i.e., all names explicitly specified in all "--ppc var1[,var2]=val" options) */
  int ppc_var_nbr=0;
  kvmap_sct *ppc_lst;  /* [sct] PPC container */
  kvmap_sct kvm;

  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    /* If user did not explicitly set deflate level for this file ... */
    if(*dfl_lvl == NCO_DFL_LVL_UNDEFINED){
      *dfl_lvl=1;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Precision-Preserving Compression (PPC) automatically activating file-wide deflation level = %d\n",nco_prg_nm_get(),*dfl_lvl);
    } /* endif */
  }else{
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Requested Precision-Preserving Compression (PPC) on netCDF3 output dataset. Unlike netCDF4, netCDF3 does not support internal compression. To take full advantage of PPC consider writing file as netCDF4 enhanced (e.g., %s -4 ...) or classic (e.g., %s -7 ...). Or consider compressing the netCDF3 file afterwards with, e.g., gzip or bzip2. File must then be uncompressed with, e.g., gunzip or bunzip2 before netCDF readers will recognize it. See http://nco.sf.net/nco.html#ppc for more information on PPC strategies.\n",nco_prg_nm_get(),nco_prg_nm_get(),nco_prg_nm_get());
  } /* endelse */

  ppc_lst=(kvmap_sct *)nco_malloc(NC_MAX_VARS*sizeof(kvmap_sct));

  /* Parse PPCs */
  for(ppc_arg_idx=0;ppc_arg_idx<ppc_arg_nbr;ppc_arg_idx++){
    if(!strstr(ppc_arg[ppc_arg_idx],"=")){
      (void)fprintf(stdout,"%s: Invalid --ppc specification: %s. Must contain \"=\" sign.\n",nco_prg_nm_get(),ppc_arg[ppc_arg_idx]);
      if(ppc_lst) nco_kvmaps_free(ppc_lst);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    kvm=nco_sng2map(ppc_arg[ppc_arg_idx],kvm);
    /* nco_sng2map() converts argument "--ppc one,two=3" into kvm.key="one,two" and kvm.value=3
       Then nco_lst_prs_2D() converts kvm.key into two items, "one" and "two", with the same value, 3 */
    if(kvm.key){
      int var_idx; /* [idx] Index over variables in current PPC argument */
      int var_nbr; /* [nbr] Number of variables in current PPC argument */
      char **var_lst;
      var_lst=nco_lst_prs_2D(kvm.key,",",&var_nbr);
      for(var_idx=0;var_idx<var_nbr;var_idx++){ /* Expand multi-variable specification */
        ppc_lst[ppc_var_nbr].key=strdup(var_lst[var_idx]);
        ppc_lst[ppc_var_nbr].value=strdup(kvm.value);
        ppc_var_nbr++;
      } /* end for */
      var_lst=nco_sng_lst_free(var_lst,var_nbr);
    } /* end if */
  } /* end for */

  /* PPC default exists, set all non-coordinate variables to default first */
  for(ppc_var_idx=0;ppc_var_idx<ppc_var_nbr;ppc_var_idx++){
    if(!strcasecmp(ppc_lst[ppc_var_idx].key,"default")){
      nco_ppc_set_dflt(nc_id,ppc_lst[ppc_var_idx].value,trv_tbl);
      break; /* Only one default is needed */
    } /* endif */
  } /* end for */

  /* Set explicit, non-default PPCs that can overwrite default */
  for(ppc_var_idx=0;ppc_var_idx<ppc_var_nbr;ppc_var_idx++){
    if(!strcasecmp(ppc_lst[ppc_var_idx].key,"default")) continue;
    nco_ppc_set_var(ppc_lst[ppc_var_idx].key,ppc_lst[ppc_var_idx].value,trv_tbl);
  } /* end for */

  /* Unset PPC and flag for all variables with excessive PPC
     Operational definition of maximum PPC is maximum decimal precision of float/double = FLT_DIG/DBL_DIG = 7/15 */
  const int nco_max_ppc_dbl=15;
  const int nco_max_ppc_flt=7;
  /* Maximum digits for integers taken based on LONG_MAX ... from limits.h */
  const int nco_max_ppc_short=5;
  const int nco_max_ppc_ushort=5;
  const int nco_max_ppc_int=10;
  const int nco_max_ppc_uint=10;
  const int nco_max_ppc_int64=19;
  const int nco_max_ppc_uint64=20;
  int nco_max_ppc=int_CEWI;

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    if(trv_tbl->lst[idx_tbl].ppc != NC_MAX_INT){
      switch(trv_tbl->lst[idx_tbl].var_typ){
      case NC_FLOAT: nco_max_ppc=nco_max_ppc_flt; break;
      case NC_DOUBLE: nco_max_ppc=nco_max_ppc_dbl; break;
      case NC_SHORT: nco_max_ppc=nco_max_ppc_short; break;
      case NC_USHORT: nco_max_ppc=nco_max_ppc_ushort; break;
      case NC_INT: nco_max_ppc=nco_max_ppc_int; break;
      case NC_UINT: nco_max_ppc=nco_max_ppc_uint; break;
      case NC_INT64: nco_max_ppc=nco_max_ppc_int64; break;
      case NC_UINT64: nco_max_ppc=nco_max_ppc_uint64; break;
	/* Do nothing for non-numeric types ...*/
      case NC_CHAR:
      case NC_BYTE:
      case NC_UBYTE:
      case NC_STRING: break;
      default: 
	nco_dfl_case_nc_type_err();
	break;
      } /* end switch */

      switch(trv_tbl->lst[idx_tbl].var_typ){
	/* Floating point types */
      case NC_FLOAT: 
      case NC_DOUBLE: 
	if(trv_tbl->lst[idx_tbl].ppc > nco_max_ppc){
	  if(trv_tbl->lst[idx_tbl].flg_nsd) (void)fprintf(stdout,"%s: INFO Number of Significant Digits (NSD) requested = %d too high for variable %s which is of type %s. No quantization or rounding will be performed for this variable. HINT: Maximum precisions for NC_FLOAT and NC_DOUBLE are %d and %d, respectively.\n",nco_prg_nm_get(),trv_tbl->lst[idx_tbl].ppc,trv_tbl->lst[idx_tbl].nm,nco_typ_sng(trv_tbl->lst[idx_tbl].var_typ),nco_max_ppc_flt,nco_max_ppc_dbl);
	  trv_tbl->lst[idx_tbl].ppc=NC_MAX_INT;
	} /* endif */
	break;
	/* Integer types */
      case NC_SHORT:
      case NC_USHORT:
      case NC_INT:
      case NC_UINT:
      case NC_INT64:
      case NC_UINT64:
	if(
	   /* ...rounding requested with NSD or ... */
	   (trv_tbl->lst[idx_tbl].flg_nsd) ||
	   /* ...more rounding requested with DSD than available or ... */
	   (!trv_tbl->lst[idx_tbl].flg_nsd && (trv_tbl->lst[idx_tbl].ppc < -1*nco_max_ppc)) ||
	   /* ...more precision requested than integers have or ... */
	   (!trv_tbl->lst[idx_tbl].flg_nsd && (trv_tbl->lst[idx_tbl].ppc >= 0)) ||
	   False)
	trv_tbl->lst[idx_tbl].ppc=NC_MAX_INT;
	break;
      case NC_CHAR: /* Do nothing for non-numeric types ...*/
      case NC_BYTE:
      case NC_UBYTE:
      case NC_STRING:
	trv_tbl->lst[idx_tbl].ppc=NC_MAX_INT;
	break;
      default: 
	nco_dfl_case_nc_type_err();
	break;
      } /* end switch */
      /* For consistency reset flg_nsd as well */
      if(trv_tbl->lst[idx_tbl].ppc == NC_MAX_INT) trv_tbl->lst[idx_tbl].flg_nsd=True;
    } /* endif */
  } /* endfor */

  if(ppc_lst) nco_kvmaps_free(ppc_lst);
} /* end nco_ppc_ini() */

void
nco_ppc_set_dflt /* Set PPC value for all non-coordinate variables for --ppc default  */
(const int nc_id, /* I [id] netCDF input file ID */
 const char * const ppc_arg, /* I [sng] User input for precision-preserving compression */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  int ppc_val;
  nco_bool flg_nsd=True; /* [flg] PPC is NSD */

  if(ppc_arg[0] == '.'){
    flg_nsd=False; /* DSD */
    ppc_val=(int)strtol(ppc_arg+1L,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(ppc_arg+1L,"strtol",sng_cnv_rcd);
  }else{ /* NSD */
    ppc_val=(int)strtol(ppc_arg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(ppc_arg,"strtol",sng_cnv_rcd);
    if(ppc_val <= 0){
      (void)fprintf(stdout,"%s ERROR Number of Significant Digits (NSD) must be postive. Default is specified as %d. HINT: Decimal Significant Digit (DSD) rounding does accept negative arguments (number of digits in front of the decimal point). However, the DSD argument must be prefixed by a period or \"dot\", e.g., \"--ppc foo=.-2\", to distinguish it from NSD quantization.\n",nco_prg_nm_get(),ppc_val);
      nco_exit(EXIT_FAILURE);
    } /* endif */    
  } /* end if */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !trv_tbl->lst[idx_tbl].is_crd_var){
      /* Allow "default" to affect only floating point types */
      if(trv_tbl->lst[idx_tbl].var_typ == NC_FLOAT || trv_tbl->lst[idx_tbl].var_typ == NC_DOUBLE){
	/* Prevent "default" from applying to coordinate and bounds variables */
	int grp_id;
	int var_id;
	nco_inq_grp_full_ncid(nc_id,trv_tbl->lst[idx_tbl].grp_nm_fll,&grp_id);
	nco_inq_varid(grp_id,trv_tbl->lst[idx_tbl].nm,&var_id);
	if(!nco_is_spc_in_bnd_att(grp_id,var_id) && !nco_is_spc_in_crd_att(grp_id,var_id)){
	  trv_tbl->lst[idx_tbl].ppc=ppc_val;
	  trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
	} /* endif */
      } /* endif */
    } /* endif */
  } /* endfor */
} /* end nco_ppc_set_dflt() */

void
nco_ppc_set_var
(const char * const var_nm, /* I [sng] Variable name to find */
 const char * const ppc_arg, /* I [sng] User input for precision-preserving compression */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  const char sls_chr='/'; /* [chr] Slash character */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  int mch_nbr=0;
  int ppc_val;
  nco_bool flg_nsd=True; /* [flg] PPC is NSD */

  if(ppc_arg[0] == '.'){ /* DSD */
    flg_nsd=False;
    ppc_val=(int)strtol(ppc_arg+1L,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(ppc_arg+1L,"strtol",sng_cnv_rcd);
  }else{ /* NSD */
    ppc_val=(int)strtol(ppc_arg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(ppc_arg,"strtol",sng_cnv_rcd);
    if(ppc_val <= 0){
      (void)fprintf(stdout,"%s ERROR Number of Significant Digits (NSD) must be postive. Specified value for %s is %d. HINT: Decimal Significant Digit (DSD) rounding does accept negative arguments (number of digits in front of the decimal point). However, the DSD argument must be prefixed by a period or \"dot\", e.g., \"--ppc foo=.-2\", to distinguish it from NSD quantization.\n",nco_prg_nm_get(),var_nm,ppc_val);
      nco_exit(EXIT_FAILURE);
    } /* endif */    
  } /* end else */

  if(strpbrk(var_nm,".*^$\\[]()<>+?|{}")){ /* Regular expression ... */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
    regmatch_t *result;
    regex_t *rx;
    size_t rx_prn_sub_xpr_nbr;
    rx=(regex_t *)nco_malloc(sizeof(regex_t));
    if(strchr(var_nm,sls_chr)){ /* Full name is used */
      /* Important difference between full- and short-name matching: Prepend carat to RX so full name matches must start at beginning of variable name */
      char *sng2mch;
      sng2mch=(char *)nco_malloc(NC_MAX_VARS*sizeof(char *));
      sng2mch[0]='\0';
      strcat(sng2mch,"^");
      strcat(sng2mch,var_nm);
      if(regcomp(rx,sng2mch,(REG_EXTENDED | REG_NEWLINE))){ /* Compile regular expression */
        (void)fprintf(stdout,"%s: ERROR trv_tbl_set_ppc() error in regular expression \"%s\"\n",nco_prg_nm_get(),var_nm);
        nco_exit(EXIT_FAILURE);
      } /* endif */
      rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */
      result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);
      for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
        if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var){
	  if(!regexec(rx,trv_tbl->lst[idx_tbl].nm_fll,rx_prn_sub_xpr_nbr,result,0)){
	    trv_tbl->lst[idx_tbl].ppc=ppc_val;
	    trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
	    mch_nbr++;
	  } /* endif */
        } /* endif */
      } /* endfor */
      sng2mch=(char *)nco_free(sng2mch);
    }else{ /* Relative name is used */
      if(regcomp(rx,var_nm,(REG_EXTENDED | REG_NEWLINE))){ /* Compile regular expression */
        (void)fprintf(stdout,"%s: ERROR trv_tbl_set_ppc() error in regular expression \"%s\"\n",nco_prg_nm_get(),var_nm);
        nco_exit(EXIT_FAILURE);
      } /* endif */
      rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */
      result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);
      for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
        if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var){
	  if(!regexec(rx,trv_tbl->lst[idx_tbl].nm,rx_prn_sub_xpr_nbr,result,0)){
	    trv_tbl->lst[idx_tbl].ppc=ppc_val;
	    trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
	    mch_nbr++;
	  } /* endif */
	} /* endif */
      } /* endfor */
    } /* end Full name */
    regfree(rx); /* Free regular expression data structure */
    rx=(regex_t *)nco_free(rx);
    result=(regmatch_t *)nco_free(result);
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
    (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",nco_prg_nm_get(),var_nm);
      nco_exit(EXIT_FAILURE);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
  }else if(strchr(var_nm,sls_chr)){ /* Full name */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var){
	if(!strcmp(var_nm,trv_tbl->lst[idx_tbl].nm_fll)){
	  trv_tbl->lst[idx_tbl].ppc=ppc_val;
	  trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
	  mch_nbr++;
	  break; /* Only one match with full name */
	} /* endif */
      } /* endif */
    } /* endfor */
  }else{ /* Not full name so set all matching vars */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var){
	if(!strcmp(var_nm,trv_tbl->lst[idx_tbl].nm)){
	  trv_tbl->lst[idx_tbl].ppc=ppc_val;
	  trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
	  mch_nbr++;
	} /* endif */
      } /* endif */
    } /* endfor */
  } /* end Full name */
  
  if(mch_nbr == 0){
    (void)fprintf(stdout,"%s: ERROR nco_ppc_set_var() reports user specified variable (or, possibly, regular expression) = \"%s\" does not match any variables in input file\n",nco_prg_nm_get(),var_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */
    
  return;
} /* end nco_ppc_set_var() */

char *
nco_sng_strip( /* [fnc] Strip leading and trailing white space */
char *sng)
{
  /* fxm: seems not working for \n??? */
  char *srt=sng;
  while(isspace(*srt)) srt++;
  size_t end=strlen(srt);
  if(srt != sng){
    memmove(sng,srt,end);
    sng[end]='\0';
  } /* endif */
  while(isblank(*(sng+end-1L))) end--;
  sng[end]='\0';
  return sng;
} /* end nco_sng_strip() */

void nco_kvmaps_free(kvmap_sct *kvmaps)
{
  /* Purpose: Relinquish dynamic memory from list of kvmaps structures */
  int idx=0;
  while(kvmaps[idx].key){
    kvmaps[idx].key=(char *)nco_free(kvmaps[idx].key);
    kvmaps[idx].value=(char *)nco_free(kvmaps[idx].value);
    idx++;
  } /* end while */
  kvmaps=(kvmap_sct *)nco_free(kvmaps);
} /* end nco_kvmaps_free */

void nco_kvmap_prn(kvmap_sct vm)
{
  if(vm.key) (void)fprintf(stdout,"%s = %s\n",vm.key,vm.value); else return;
} /* end nco_kvmap_prn() */

int /* O [rcd] Return code */
nco_scrip_read /* [fnc] Read, parse, and print contents of SCRIP file */
(char *fl_scrip, /* SCRIP file name with proper path */
 kvmap_sct *kvm_scrip)/* structure to hold contents of SCRIP file */ 
{
  char *sng_line;
  
  int icnt;
  int idx=0;

  FILE *fp_scrip;

  fp_scrip=fopen(fl_scrip,"r");

  if(!fp_scrip){
    (void)fprintf(stderr,"%s: ERROR Cannot open SCRIP file %s\n",nco_prg_nm_get(),fl_scrip);
    return NCO_ERR;
  } /* endif */

  sng_line=(char *)nco_malloc(BUFSIZ*sizeof(char));
  while(fgets(sng_line,sizeof(sng_line),fp_scrip)){
    /* Each line must contain "=" */
    if(!strstr(sng_line,"=")){
      (void)fprintf(stderr,"%s: ERROR Invalid line in SCRIP file: %s\n",nco_prg_nm_get(),sng_line);
      fclose(fp_scrip);
      return NCO_ERR;
    } /* endif */
    kvm_scrip[idx]=nco_sng2map(sng_line,kvm_scrip[idx]);
    if(!kvm_scrip[idx].key){
      fclose(fp_scrip);
      return NCO_ERR;
    }else{
      idx++;
    } /* end else */
  } /* finish parsing SCRIP file */
  fclose(fp_scrip);
  if(sng_line) sng_line=(char *)nco_free(sng_line);

  for(icnt=0;icnt<idx;icnt++) nco_kvmap_prn(kvm_scrip[icnt]);

  return NCO_NOERR;
} /* end nco_scrip_read */

#ifdef ENABLE_ESMF
int /* O [enm] Return code */
nco_rgr_ini /* [fnc] Initialize regridding structure */
(const int in_id, /* I [id] Input netCDF file ID */
 char **rgr_arg, /* [sng] Regriding arguments */
 const int rgr_nbr, /* [nbr] Number of regriding arguments */
 char * const rgr_in, /* I [sng] File containing fields to be regridded */
 char * const rgr_out, /* I [sng] File containing regridded fields */
 char * const rgr_grd_src, /* I [sng] File containing input grid */
 char * const rgr_grd_dst, /* I [sng] File containing destination grid */
 char * const rgr_map, /* I [sng] File containing mapping weights from source to destination grid */
 rgr_sct * const rgr_nfo) /* O [sct] Regridding structure */
{
  /* Purpose: Initialize regriding structure */
     
  /* Sample calls:
     Debugging:
     ncks -O -D 6 --rgr=Y ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc

     T42->T42 from scratch, minimal arguments:
     ncks -O -D 6 --rgr=Y ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T42->T42 from scratch, more arguments:
     ncks -O -D 6 --rgr=Y --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T42->T42 from scratch, explicit arguments:
     ncks -O --rgr=Y --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T42->T42 from scratch:
     ncks -O --rgr=Y --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T42->POP43 from existing weights:
     ncks -O --rgr=Y --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_POP43.nc --rgr_map=${DATA}/scrip/rmp_T42_to_POP43_conserv.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc */

  const char fnc_nm[]="nco_rgr_ini()";
  
  int rcd=NC_NOERR;
  
  /* Initialize */
  rgr_nfo->flg_usr_rqs=False; /* [flg] User requested regridding */
  rgr_nfo->out_id=int_CEWI; /* [id] Output netCDF file ID */

  rgr_nfo->in_id=in_id; /* [id] Input netCDF file ID */
  rgr_nfo->rgr_arg=rgr_arg; /* [sng] Regriding arguments */
  rgr_nfo->rgr_nbr=rgr_nbr; /* [nbr] Number of regridding arguments */

  rgr_nfo->fl_grd_src=rgr_grd_src; /* [sng] File containing input grid */
  rgr_nfo->fl_grd_dst=rgr_grd_dst; /* [sng] File containing destination grid */
  rgr_nfo->fl_in=rgr_in; /* [sng] File containing fields to be regridded */
  rgr_nfo->fl_out=rgr_out; /* [sng] File containing regridded fields */
  rgr_nfo->fl_out_tmp=NULL_CEWI; /* [sng] Temporary file containing regridded fields */
  rgr_nfo->fl_map=rgr_map; /* [sng] File containing mapping weights from source to destination grid */
  
  /* Did user explicitly request regridding? */
  if(rgr_nbr > 0 || rgr_grd_src != NULL || rgr_grd_dst != NULL || rgr_out != NULL || rgr_map != NULL) rgr_nfo->flg_usr_rqs=True;

  /* Initialize arguments after copying */
  if(!rgr_nfo->fl_out) rgr_nfo->fl_out=(char *)strdup("/data/zender/rgr/rgr_out.nc");
  if(!rgr_nfo->fl_grd_dst) rgr_nfo->fl_grd_dst=(char *)strdup("/data/zender/scrip/grids/remap_grid_T42.nc");
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"flg_usr_rqs = %d, ",rgr_nfo->flg_usr_rqs);
    (void)fprintf(stderr,"rgr_nbr = %d, ",rgr_nfo->rgr_nbr);
    (void)fprintf(stderr,"fl_grd_src = %s, ",rgr_nfo->fl_grd_src ? rgr_nfo->fl_grd_src : "NULL");
    (void)fprintf(stderr,"fl_grd_dst = %s, ",rgr_nfo->fl_grd_dst ? rgr_nfo->fl_grd_dst : "NULL");
    (void)fprintf(stderr,"fl_in = %s, ",rgr_nfo->fl_in ? rgr_nfo->fl_in : "NULL");
    (void)fprintf(stderr,"fl_out = %s, ",rgr_nfo->fl_out ? rgr_nfo->fl_out : "NULL");
    (void)fprintf(stderr,"fl_out_tmp = %s, ",rgr_nfo->fl_out_tmp ? rgr_nfo->fl_out_tmp : "NULL");
    (void)fprintf(stderr,"fl_map = %s, ",rgr_nfo->fl_map ? rgr_nfo->fl_map : "NULL");
    (void)fprintf(stderr,"\n");
  } /* endif dbg */
  
  char *nvr_DATA_TEMPEST; /* [sng] Directory where Tempest grids, meshes, and weights are stored */
  nvr_DATA_TEMPEST=getenv("DATA_TEMPEST");
  rgr_nfo->drc_dat= (nvr_DATA_TEMPEST && strlen(nvr_DATA_TEMPEST) > 0) ? (char *)strdup(nvr_DATA_TEMPEST) : (char *)strdup("/tmp");
  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"nvr_DATA_TEMPEST = %s, ",nvr_DATA_TEMPEST ? nvr_DATA_TEMPEST : "NULL");
    (void)fprintf(stderr,"drc_dat = %s, ",rgr_nfo->drc_dat ? rgr_nfo->drc_dat : "NULL");
    (void)fprintf(stderr,"\n");
  } /* endif dbg */

  const int fmt_chr_nbr=6;
  char *cmd_rgr;
  char fl_grd_dst[]="/tmp/foo_outRLLMesh.g";
  char *fl_grd_dst_cdl;
  int rcd_sys;
  int lat_nbr_rqs=180;
  int lon_nbr_rqs=360;
  nco_rgr_cmd_typ nco_rgr_cmd; /* [enm] Tempest remap command enum */
  const char *cmd_rgr_fmt;

  /* Allow for whitespace characters in fl_grd_dst
     Assume CDL translation results in acceptable name for shell commands */
  fl_grd_dst_cdl=nm2sng_fl(fl_grd_dst);
  //drc_dat=strcat(drc_dat,"/");
  //drc_dat=strcat(drc_dat,fl_grd_dst);

  /* Construct and execute regridding command */
  nco_rgr_cmd=nco_rgr_GenerateRLLMesh;
  cmd_rgr_fmt=nco_rgr_cmd_fmt_sng(nco_rgr_cmd);
  cmd_rgr=(char *)nco_malloc((strlen(cmd_rgr_fmt)+strlen(fl_grd_dst_cdl)-fmt_chr_nbr+1UL)*sizeof(char));
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: Generating %d by %d RLL mesh in %s...\n",nco_prg_nm_get(),lat_nbr_rqs,lon_nbr_rqs,fl_grd_dst);
  (void)sprintf(cmd_rgr,cmd_rgr_fmt,lat_nbr_rqs,lon_nbr_rqs,fl_grd_dst_cdl);
  rcd_sys=system(cmd_rgr);
  if(rcd_sys == -1){
    (void)fprintf(stdout,"%s: ERROR %s is unable to complete regridding command \"%s\"\n",nco_prg_nm_get(),fnc_nm,cmd_rgr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"done\n");

  /* Clean-up memory */
  if(fl_grd_dst_cdl) fl_grd_dst_cdl=(char *)nco_free(fl_grd_dst_cdl);
  if(cmd_rgr) cmd_rgr=(char *)nco_free(cmd_rgr);
  
  if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* end nco_rgr_ini() */
  
void
nco_rgr_free /* [fnc] Deallocate regridding structure */
(rgr_sct * const rgr_nfo) /* I/O [sct] Regridding structure */
{
  /* [fnc] Free all dynamic memory in regridding structure */
  if(rgr_nfo->fl_grd_src) rgr_nfo->fl_grd_src=(char *)nco_free(rgr_nfo->fl_grd_src);
  if(rgr_nfo->fl_grd_dst) rgr_nfo->fl_grd_dst=(char *)nco_free(rgr_nfo->fl_grd_dst);
  if(rgr_nfo->fl_in) rgr_nfo->fl_in=(char *)nco_free(rgr_nfo->fl_in);
  if(rgr_nfo->fl_out) rgr_nfo->fl_out=(char *)nco_free(rgr_nfo->fl_out);
  if(rgr_nfo->fl_out_tmp) rgr_nfo->fl_out_tmp=(char *)nco_free(rgr_nfo->fl_out_tmp);
  if(rgr_nfo->fl_map) rgr_nfo->fl_map=(char *)nco_free(rgr_nfo->fl_map);

  /* Tempest */
  if(rgr_nfo->drc_dat) rgr_nfo->drc_dat=(char *)nco_free(rgr_nfo->drc_dat);
} /* end nco_rfr_free() */
  
const char * /* O [sng] String containing regridding command and format */
nco_rgr_cmd_fmt_sng /* [fnc] Convert Tempest remap command enum to command string */
(const nco_rgr_cmd_typ nco_rgr_cmd) /* I [enm] Tempest remap command enum */
{
  /* Purpose: Convert Tempest remap command enum to command string and format */
  switch(nco_rgr_cmd){
  case nco_rgr_ApplyOfflineMap:
    return "ApplyOfflineMap";
  case nco_rgr_CalculateDiffNorms:
    return "CalculateDiffNorms";
  case nco_rgr_GenerateCSMesh:
    return "GenerateCSMesh --res %d --file %s";
  case nco_rgr_GenerateGLLMetaData:
    return "GenerateGLLMetaData";
  case nco_rgr_GenerateICOMesh:
    return "GenerateICOMesh";
  case nco_rgr_GenerateLambertConfConicMesh:
    return "GenerateLambertConfConicMesh";
  case nco_rgr_GenerateOfflineMap:
    return "GenerateOfflineMap --in_mesh %s --out_mesh %s --ov_mesh %s --in_data %s --out_data %s";
  case nco_rgr_GenerateOverlapMesh:
    return "GenerateOverlapMesh --a %s --b %s --out %s";
  case nco_rgr_GenerateRLLMesh:
    return "GenerateRLLMesh --lat %d --lon %d --file %s";
  case nco_rgr_GenerateTestData:
    return "GenerateTestData --mesh %s --np %d --test %d --out %s";
  case nco_rgr_MeshToTxt:
    return "MeshToTxt";
  case nco_rgr_AAA_nil:
  case nco_rgr_ZZZ_last:
  default: nco_dfl_case_rgr_cmd_err(); break;
  } /* end switch */
  
  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_cmd_fmt_sng() */

const char * /* O [sng] String containing regridding command name */
nco_rgr_cmd_sng /* [fnc] Convert Tempest remap command enum to command name */
(const nco_rgr_cmd_typ nco_rgr_cmd) /* I [enm] Tempest remap command enum */
{
  /* Purpose: Convert Tempest remap command enum to command string */
  switch(nco_rgr_cmd){
  case nco_rgr_ApplyOfflineMap: return "ApplyOfflineMap";
  case nco_rgr_CalculateDiffNorms: return "CalculateDiffNorms";
  case nco_rgr_GenerateCSMesh: return "GenerateCSMesh";
  case nco_rgr_GenerateGLLMetaData: return "GenerateGLLMetaData";
  case nco_rgr_GenerateICOMesh: return "GenerateICOMesh";
  case nco_rgr_GenerateLambertConfConicMesh: return "GenerateLambertConfConicMesh";
  case nco_rgr_GenerateOfflineMap: return "GenerateOfflineMap";
  case nco_rgr_GenerateOverlapMesh: return "GenerateOverlapMesh";
  case nco_rgr_GenerateRLLMesh: return "GenerateRLLMesh";
  case nco_rgr_GenerateTestData: return "GenerateTestData";
  case nco_rgr_MeshToTxt: return "MeshToTxt";
  case nco_rgr_AAA_nil:
  case nco_rgr_ZZZ_last:
  default: nco_dfl_case_rgr_cmd_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_cmd_sng() */

const char * /* O [sng] String version of ESMC_FileFormat_Flag */
nco_esmf_fl_fmt_sng /* [fnc] Convert ESMF file format to string */
(const int nco_esmf_fl_fmt) /* I [enm] ESMF file format enum ESMC_FileFormat_Flag */
{
  /* Purpose: Convert ESMF fileTypeFlag enum to string */
  switch(nco_esmf_fl_fmt){
  case ESMC_FILEFORMAT_UNDEFINED: return "ESMC_FILEFORMAT_UNDEFINED";
  case ESMC_FILEFORMAT_VTK: return "ESMC_FILEFORMAT_VTK";
  case ESMC_FILEFORMAT_SCRIP: return "ESMC_FILEFORMAT_SCRIP";
  case ESMC_FILEFORMAT_ESMFMESH: return "ESMC_FILEFORMAT_ESMFMESH";
  case ESMC_FILEFORMAT_ESMCGRID: return "ESMC_FILEFORMAT_ESMCGRID";
  case ESMC_FILEFORMAT_UGRID: return "ESMC_FILEFORMAT_UGRID";
  case ESMC_FILEFORMAT_GRIDSPEC: return "ESMC_FILEFORMAT_GRIDSPEC";
  default: nco_dfl_case_esmf_fl_fmt_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_fl_fmt_sng() */

void
nco_dfl_case_rgr_cmd_err(void) /* [fnc] Print error and exit for illegal switch case */
{

/* Purpose: Print error and exit when switch statement reaches illegal default case
   Routine reduces bloat because many switch() statements invoke this functionality */
const char fnc_nm[]="nco_dfl_case_rgr_cmd_err()";
  (void)fprintf(stdout,"%s: ERROR switch statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_rgr_cmd_err() */

void
nco_dfl_case_esmf_fl_fmt_err(void) /* [fnc] Print error and exit for illegal switch case */
{

/* Purpose: Print error and exit when switch statement reaches illegal default case
   Routine reduces bloat because many switch() statements invoke this functionality */
const char fnc_nm[]="nco_dfl_case_esmf_fl_fmt_err()";
  (void)fprintf(stdout,"%s: ERROR switch statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_esmf_fl_fmt_err() */

int /* O [enm] Return code */
nco_rgr_esmf /* [fnc] Regrid using ESMF library */
(rgr_sct * const rgr_nfo) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields
     ESMC is C-interface to ESMF documented at
     http://www.earthsystemmodeling.org/esmf_releases/last_built/ESMC_crefdoc/ESMC_crefdoc.html
     ESMF Developer's Guide
     http://www.earthsystemmodeling.org/documents/dev_guide
     ESMF_RegridWeightGen
     http://www.earthsystemcog.org/projects/regridweightgen
     http://www.earthsystemmodeling.org/python_releases/last_esmpy/esmpy_doc/html/index.html */
  
  const char fnc_nm[]="nco_rgr_esmf()"; /* [sng] Function name */
  const char fl_nm_esmf_log[]="nco_rgr_log_foo.txt"; /* [sng] Log file for ESMF routines */
  
  int flg_isSphere=1; /* [enm] Set to 1 for a spherical grid, or 0 for regional. Defaults to 1. */
  int flg_addCornerStagger=0; /* [enm] Add corner stagger to grid. Defaults to 0. */
  int flg_addUserArea=0; /* [enm] Read cell area from Grid file (instead of calculate it). Defaults to 0. */
  int flg_addMask=0; /* [enm] Generate mask using missing value attribute in var_nm (iff GRIDSPEC) */
  char *var_nm=NULL; /* [sng] Iff addMask == 1 use this variable's missing value attribute */
  char **crd_nm=NULL; /* [sng] Iff GRIDSPEC use these lon/lat variable coordinates */
  int rcd_esmf; /* [enm] Return codes from ESMF functions */
  
  int *localPet=int_CEWI; 
  int *max_idx;
  int *petCount=int_CEWI;
  int dim_cnt=2;
  int rcd=ESMF_SUCCESS;
  
  ESMC_InterfaceInt src_max_idx;
  ESMC_Grid src_grd;
  ESMC_Grid dst_grd;
  ESMC_Field src_fld;
  ESMC_Field dst_fld;
  ESMC_RouteHandle route_hdl;
  ESMC_VM vm;
  
  //enum ESMC_RegridMethod_Flag rgr_mth=ESMC_REGRIDMETHOD_BILINEAR;
  enum ESMC_StaggerLoc stg_loc=ESMC_STAGGERLOC_CENTER;
  //enum ESMC_CoordSys_Flag crd_sys=ESMC_COORDSYS_CART;
  enum ESMC_CoordSys_Flag crd_sys=ESMC_COORDSYS_SPH_DEG;
  enum ESMC_TypeKind_Flag typ_knd=ESMC_TYPEKIND_R8;
  nc_type var_typ_out=NC_DOUBLE;
  enum ESMC_LogMsgType_Flag log_msg=ESMC_LOGMSG_INFO;
  //enum ESMC_UnmappedAction_Flag unmap_act=ESMC_UNMAPPEDACTION_IGNORE;
  enum ESMC_FileFormat_Flag grd_fl_typ=ESMC_FILEFORMAT_SCRIP;
  
  int *dmn_id;
  
  int idx;
  int dmn_nbr;
  int var_in_id;
  
  long *dmn_cnt;
  long *dmn_srt;
  
  long var_sz=1L;
  
  nc_type var_typ;
  
  void *void_ptr_lon;
  void *void_ptr_lat;
  void *void_ptr_var;
  
  int in_id; /* I [id] Input netCDF file ID */
  int out_id; /* I [id] Output netCDF file ID */

  /* Initialize */
  in_id=rgr_nfo->in_id;
  out_id=rgr_nfo->out_id;
  dmn_nbr=3;
  
  /* Allocate space to hold dimension IDs */
  dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
  dmn_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  
  /* Obtain longitude from input data file */
  (void)nco_inq_varid(in_id,"lon",&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ,&dmn_nbr,(int *)NULL,(int *)NULL);
  
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  
  /* Get dimension sizes from input file */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    var_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  
  max_idx=(int *)nco_malloc(dim_cnt*sizeof(int));
  max_idx[0]=var_sz; /* upbound idx of lon */
  
  /* Allocate enough space to hold variable */
  void_ptr_lon=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_typ),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr_lon,var_typ);
  float *lon=(float *)void_ptr_lon;

  /* Obtain lat from input data file */
  (void)nco_inq_varid(in_id,"lat",&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ,&dmn_nbr,(int *)NULL,(int *)NULL);
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  var_sz=1L;
  /* Get dimension sizes from input file */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    var_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  max_idx[1]=var_sz; /* upbound idx of lat */
  void_ptr_lat=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_typ),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr_lat,var_typ);
  float *lat=(float *)void_ptr_lat;

  /* Initialize before any other ESMC API calls!
     ESMC_ArgLast is ALWAYS at the end to indicate the end of opt args */
  ESMC_Initialize(&rcd_esmf,
		  ESMC_InitArgDefaultCalKind(ESMC_CALKIND_GREGORIAN),
		  ESMC_InitArgLogFilename(fl_nm_esmf_log),
		  ESMC_InitArgLogKindFlag(ESMC_LOGKIND_MULTI),
		  ESMC_ArgLast);
  if(rcd_esmf != ESMF_SUCCESS){
    ESMC_LogWrite("ESMC_Initialize() failed",log_msg);
    goto rgr_cln;
  } /* endif */
  
  /* Set log to flush after every message */
  rcd_esmf=ESMC_LogSet(ESMF_TRUE);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Get all VM information */
  vm=ESMC_VMGetGlobal(&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Set-up local pet info */
  rcd_esmf=ESMC_VMGet(vm,localPet,petCount,(int *)NULL,(MPI_Comm *)NULL,(int *)NULL,(int *)NULL);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"ESMC_GridCreateFromFile arguments: filename = %s, fileTypeFlag = %s",rgr_nfo->fl_grd_dst,nco_esmf_fl_fmt_sng(grd_fl_typ));
    (void)fprintf(stderr,"\n");
  } /* endif dbg */

  /* Create destination grid from SCRIP file */
  /* 20150423 fxm core dump in next line */
  dst_grd=ESMC_GridCreateFromFile(rgr_nfo->fl_grd_dst,grd_fl_typ,&flg_isSphere,&flg_addCornerStagger,&flg_addUserArea,&flg_addMask,var_nm,crd_nm,&rcd_esmf); /* NB: ESMC_COORDSYS_SPH_DEG only */

  int *src_bnd_l=(int *)nco_malloc(dim_cnt*sizeof(int));
  int *src_bnd_u=(int *)nco_malloc(dim_cnt*sizeof(int));
  int *dst_bnd_l=(int *)nco_malloc(dim_cnt*sizeof(int));
  int *dst_bnd_u=(int *)nco_malloc(dim_cnt*sizeof(int));

  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  int *dst_msk = (int *)ESMC_GridGetItem(dst_grd,ESMC_GRIDITEM_MASK,stg_loc,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  double *dst_lon=(double *)ESMC_GridGetCoord(dst_grd,1,stg_loc,dst_bnd_l,dst_bnd_u,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  double *dst_lat=(double *)ESMC_GridGetCoord(dst_grd,2,stg_loc,NULL,NULL,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  double *lon_ptr; /* dim lon for output file */
  double *lat_ptr; /* dim lat for output file */
  lon_ptr=(double *)nco_malloc(dst_bnd_u[0]*sizeof(double));
  lat_ptr=(double *)nco_malloc(dst_bnd_u[1]*sizeof(double));
  for(idx=0;idx<dst_bnd_u[0];idx++) lon_ptr[idx]=dst_lon[idx];
  for(idx=0;idx<dst_bnd_u[1];idx++) lat_ptr[idx]=dst_lat[idx*dst_bnd_u[0]];

  for(idx=0;idx<dst_bnd_u[0]*dst_bnd_u[1];idx++) dst_msk[idx]=0;
  /* Create src_grid from lon,lat data file */
  src_max_idx=ESMC_InterfaceIntCreate(max_idx,dim_cnt,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  src_grd=ESMC_GridCreateNoPeriDim(src_max_idx,&crd_sys,&typ_knd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Add crd to src grid */
  rcd_esmf=ESMC_GridAddCoord(src_grd, stg_loc);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  double *src_lon=(double *)ESMC_GridGetCoord(src_grd,1,stg_loc,src_bnd_l,src_bnd_u,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  double *src_lat=(double *)ESMC_GridGetCoord(src_grd,2,stg_loc,NULL,NULL,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* NB: work around for non-spherical coordinate 
     max_idx[0]=dst_bnd_u[0];
     max_idx[1]=dst_bnd_u[1];
     src_max_idx=ESMC_InterfaceIntCreate(max_idx,dim_cnt,&rcd_esmf);
     dst_grd2=ESMC_GridCreateNoPeriDim(src_max_idx,&crd_sys,&typ_knd,&rcd_esmf);
     rcd_esmf=ESMC_GridAddCoord(dst_grd2, stg_loc);
     double *dst_lon2=(double *)ESMC_GridGetCoord(dst_grd2,1,stg_loc,dst_bnd_l,dst_bnd_u,&rcd_esmf);
     double *dst_lat2=(double *)ESMC_GridGetCoord(dst_grd2,2,stg_loc,NULL,NULL,&rcd_esmf);
     dst_lon2=dst_lon;
     dst_lat2=dst_lat; */
  max_idx=nco_free(max_idx);
  ESMC_InterfaceIntDestroy(&src_max_idx);
  /* iff mask is used
     rcd_esmf=ESMC_GridAddItem(src_grd,ESMC_GRIDITEM_MASK,stg_loc);
     int *src_msk=(int *)ESMC_GridGetItem(src_grd,ESMC_GRIDITEM_MASK,stg_loc,&rcd_esmf); */
  /* Type-conversion and cell-center coordinates */
  idx=0;
  for(int idx_1=0;idx_1<src_bnd_u[1];idx_1++){
    for(int idx_0=0;idx_0<src_bnd_u[0];idx_0++){
      src_lon[idx]=(double)lon[idx_0];
      src_lat[idx]=(double)lat[idx_1];
      idx++;
    } /* endfor */
  } /* endfor */

  /* Create src field from src grid */
  src_fld=ESMC_FieldCreateGridTypeKind(src_grd,typ_knd,stg_loc,NULL,NULL,NULL,"src_fld",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Create dst field from dst grid */
  dst_fld=ESMC_FieldCreateGridTypeKind(dst_grd,typ_knd,stg_loc,NULL,NULL,NULL,"dst_fld",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Get field pointers */
  double *src_fld_ptr=(double *)ESMC_FieldGetPtr(src_fld,0,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  double *dst_fld_ptr=(double *)ESMC_FieldGetPtr(dst_fld,0,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Get variables from input file */
  (void)nco_inq_varid(in_id,"ORO",&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ,&dmn_nbr,(int *)NULL,(int *)NULL);
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  var_sz=1L;
  /* Get dimension sizes from input file */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    var_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  /* Allocate enough space to hold variable */
  void_ptr_var=(void *)nco_malloc_dbg(var_sz*nco_typ_lng(var_typ),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr_var,var_typ);
  float *var_fld=(float *)void_ptr_var;
  /* Type-conversion and ensure every cell has data */
  idx=0;
  for(int idx_1=src_bnd_l[1];idx_1<=src_bnd_u[1];idx_1++){
    for(int idx_0=src_bnd_l[0];idx_0<=src_bnd_u[0];idx_0++){
      src_fld_ptr[idx]=(double)var_fld[idx];
      idx++;
    } /* idx_0 */
  } /* idx_1 */
  /* Initialize dst data ptr */
  idx=0;
  for(int idx_1=dst_bnd_l[1];idx_1<=dst_bnd_u[1];idx_1++){
    for(int idx_0=dst_bnd_l[0];idx_0<=dst_bnd_u[0];idx_0++){
      dst_fld_ptr[idx]=0.0;
      idx++;
    } /* idx_0 */
  } /* idx_1 */

  ESMC_LogWrite("ESMC starting regridstore DYW",log_msg);
  /* int *msk_val=(int *)nco_malloc(sizeof(int));
     msk_val[0]=1;
     ESMC_InterfaceInt i_msk_val=ESMC_InterfaceIntCreate(msk_val,1,&rcd_esmf);
     rcd_esmf = ESMC_FieldRegridStore(src_fld,dst_fld,&i_msk_val,&i_msk_val,&route_hdl,NULL,NULL,NULL,&unmap_act,NULL,NULL);
     rcd_esmf=ESMC_FieldRegridStore(src_fld,dst_fld,NULL,NULL,&route_hdl,NULL,NULL,NULL,&unmap_act,NULL,NULL); */

  rcd_esmf=ESMC_FieldRegridStore(src_fld,dst_fld,NULL,NULL,&route_hdl,NULL,NULL,NULL,NULL,NULL,NULL);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  rcd_esmf=ESMC_FieldRegrid(src_fld,dst_fld,route_hdl,NULL);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Write dst_fld data to netCDF file */
  int var_out_id,var_lon_id,var_lat_id;    /* [id] Variable ID */
  int lat_id,lon_id; /* [id] Dimension ID */
  (void)nco_def_dim(out_id,"lat",dst_bnd_u[1],&lat_id);
  (void)nco_def_dim(out_id,"lon",dst_bnd_u[0],&lon_id);
  int dmn_ids_out[2]; /* [id] Dimension IDs array for output variable */
  long dmn_srt_out[2];
  long dmn_cnt_out[2];
  long cnt_out[1];
  long srt_out[1];
  dmn_ids_out[0]=lat_id;
  dmn_ids_out[1]=lon_id;
  (void)nco_def_var(out_id,"lon",var_typ_out,1,&lon_id,&var_lon_id);
  (void)nco_def_var(out_id,"lat",var_typ_out,1,&lat_id,&var_lat_id);
  (void)nco_def_var(out_id,"ORO",var_typ_out,2,dmn_ids_out,&var_out_id);
  /* char * att_val;
     att_val=strdup("degrees_north");
     nco_put_att(out_id,var_lon_id,"units",NC_STRING,strlen(att_val),att_val);
     att_val=strdup("degrees_east");
     nco_put_att(out_id,var_lat_id,"units",NC_STRING,strlen(att_val),att_val);
     att_val=(char *)nco_free(att_val); */
  (void)nco_enddef(out_id);
  cnt_out[0]=dst_bnd_u[1];
  srt_out[0]=0L;
  (void)nco_put_vara(out_id,var_lat_id,srt_out,cnt_out,lat_ptr,var_typ_out);
  cnt_out[0]=dst_bnd_u[0];
  (void)nco_put_vara(out_id,var_lon_id,srt_out,cnt_out,lon_ptr,var_typ_out);
  dmn_srt_out[0]=0L;
  dmn_srt_out[1]=0L;
  dmn_cnt_out[0]=dst_bnd_u[1];
  dmn_cnt_out[1]=dst_bnd_u[0];
  (void)nco_put_vara(out_id,var_out_id,dmn_srt_out,dmn_cnt_out,dst_fld_ptr,var_typ_out);

rgr_cln:
  if(src_bnd_l) src_bnd_l=(int *)nco_free(src_bnd_l);
  if(src_bnd_u) src_bnd_u=(int *)nco_free(src_bnd_u);
  if(dst_bnd_l) dst_bnd_l=(int *)nco_free(dst_bnd_l);
  if(dst_bnd_u) dst_bnd_u=(int *)nco_free(dst_bnd_u);
  if(void_ptr_lon) void_ptr_lon=(void *)nco_free(void_ptr_lon);
  if(void_ptr_lat) void_ptr_lat=(void *)nco_free(void_ptr_lat);
  if(void_ptr_var) void_ptr_var=(void *)nco_free(void_ptr_var);
  if(lat_ptr) lat_ptr=(double *)nco_free(lat_ptr);
  if(lon_ptr) lon_ptr=(double *)nco_free(lon_ptr);
  if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
  if(dmn_id) dmn_id=(int *)nco_free(dmn_id);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);

  rcd_esmf=ESMC_FieldRegridRelease(&route_hdl);
  rcd_esmf=ESMC_FieldDestroy(&src_fld);
  rcd_esmf=ESMC_FieldDestroy(&dst_fld);
  rcd_esmf=ESMC_GridDestroy(&src_grd);
  rcd_esmf=ESMC_GridDestroy(&dst_grd);
  ESMC_Finalize();

  return rcd;
} /* nco_rgr_esmf */
#endif /* !ENABLE_ESMF */

