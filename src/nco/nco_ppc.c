/* $Header$ */

/* Purpose: NCO utilities for Precision-Preserving Compression (PPC) */

/* Copyright (C) 2015--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   ncks -4 -O -C -v ppc_dbl --ppc /ppc_dbl=3 ~/nco/data/in.nc ~/foo.nc */

#include "nco_ppc.h" /* Precision-Preserving Compression */

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
 char * const ppc_arg[], /* I [sng] List of user-specified PPCs */
 const int ppc_arg_nbr, /* I [nbr] Number of PPC specified */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  int ppc_var_idx; /* [idx] Index over ppc_lst (i.e., all names explicitly specified in all "--ppc var1[,var2]=val" options) */
  int ppc_var_nbr=0;
  kvm_sct *ppc_lst; /* [sct] List of all PPC specifications */

  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    /* If user did not explicitly set deflate level for this file ... */
    if(*dfl_lvl == NCO_DFL_LVL_UNDEFINED){
      *dfl_lvl=1;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Precision-Preserving Compression (PPC) automatically activating file-wide deflation level = %d\n",nco_prg_nm_get(),*dfl_lvl);
    } /* endif */
  }else{
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Requested Precision-Preserving Compression (PPC) on netCDF3 output dataset. Unlike netCDF4, netCDF3 does not support internal compression. To take full advantage of PPC consider writing file as netCDF4 enhanced (e.g., %s -4 ...) or classic (e.g., %s -7 ...). Or consider compressing the netCDF3 file afterwards with, e.g., gzip or bzip2. File must then be uncompressed with, e.g., gunzip or bunzip2 before netCDF readers will recognize it. See http://nco.sf.net/nco.html#ppc for more information on PPC strategies.\n",nco_prg_nm_get(),nco_prg_nm_get(),nco_prg_nm_get());
  } /* endelse */

  char *sng_fnl=NULL;

  /* Join arguments together */
  sng_fnl=nco_join_sng(ppc_arg, ppc_arg_nbr);
  ppc_lst=nco_arg_mlt_prs(sng_fnl);

  if(sng_fnl) sng_fnl=(char *)nco_free(sng_fnl);

  /* jm fxm use more descriptive name than i---what does i count? */
  for(int index=0;(ppc_lst+index)->key;index++, ppc_var_nbr++); /* end loop over i */

  // ppc_lst=(kvm_sct *)nco_malloc(NC_MAX_VARS*sizeof(kvm_sct));

  // /* Parse PPCs */
  // for(ppc_arg_idx=0;ppc_arg_idx<ppc_arg_nbr;ppc_arg_idx++){
  //   if(!strstr(ppc_arg[ppc_arg_idx],"=")){
  //     (void)fprintf(stdout,"%s: Invalid --ppc specification: %s. Must contain \"=\" sign.\n",nco_prg_nm_get(),ppc_arg[ppc_arg_idx]);
  //     if(ppc_lst) ppc_lst=(kvm_sct *)nco_free(ppc_lst);
  //     nco_exit(EXIT_FAILURE);
  //   } /* endif */
  //   kvm=nco_sng2kvm(ppc_arg[ppc_arg_idx]);
  //   /* nco_sng2kvm() converts argument "--ppc one,two=3" into kvm.key="one,two" and kvm.val=3
  //      Then nco_lst_prs_2D() converts kvm.key into two items, "one" and "two", with the same value, 3 */
  //   if(kvm.key){
  //     int var_idx; /* [idx] Index over variables in current PPC argument */
  //     int var_nbr; /* [nbr] Number of variables in current PPC argument */
  //     char **var_lst;
  //     var_lst=nco_lst_prs_2D(kvm.key,",",&var_nbr);
  //     for(var_idx=0;var_idx<var_nbr;var_idx++){ /* Expand multi-variable specification */
  //       ppc_lst[ppc_var_nbr].key=strdup(var_lst[var_idx]);
  //       ppc_lst[ppc_var_nbr].val=strdup(kvm.val);
  //       ppc_var_nbr++;
  //     } /* end for */
  //     var_lst=nco_sng_lst_free(var_lst,var_nbr);
  //   } /* end if */
  // } /* end for */

  /* PPC "default" specified, set all non-coordinate variables to default first */
  for(ppc_var_idx=0;ppc_var_idx<ppc_var_nbr;ppc_var_idx++){
    if(!strcasecmp(ppc_lst[ppc_var_idx].key,"default")){
      nco_ppc_set_dflt(nc_id,ppc_lst[ppc_var_idx].val,trv_tbl);
      break; /* Only one default is needed */
    } /* endif */
  } /* end for */

  /* Set explicit, non-default PPCs that can overwrite default */
  for(ppc_var_idx=0;ppc_var_idx<ppc_var_nbr;ppc_var_idx++){
    if(!strcasecmp(ppc_lst[ppc_var_idx].key,"default")) continue;
    nco_ppc_set_var(ppc_lst[ppc_var_idx].key,ppc_lst[ppc_var_idx].val,trv_tbl);
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

  if(ppc_lst) ppc_lst=nco_kvm_lst_free(ppc_lst,ppc_var_nbr);
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
      (void)fprintf(stdout,"%s ERROR Number of Significant Digits (NSD) must be positive. Default is specified as %d. HINT: Decimal Significant Digit (DSD) rounding does accept negative arguments (number of digits in front of the decimal point). However, the DSD argument must be prefixed by a period or \"dot\", e.g., \"--ppc foo=.-2\", to distinguish it from NSD quantization.\n",nco_prg_nm_get(),ppc_val);
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
	if(!nco_is_spc_in_cf_att(grp_id, "bounds", var_id, NULL) && !nco_is_spc_in_cf_att(grp_id, "climatology", var_id,
                                                                                      NULL) && !nco_is_spc_in_cf_att(
    grp_id, "coordinates", var_id, NULL)){
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
      (void)fprintf(stdout,"%s ERROR Number of Significant Digits (NSD) must be positive. Specified value for %s is %d. HINT: Decimal Significant Digit (DSD) rounding does accept negative arguments (number of digits in front of the decimal point). However, the DSD argument must be prefixed by a period or \"dot\", e.g., \"--ppc foo=.-2\", to distinguish it from NSD quantization.\n",nco_prg_nm_get(),var_nm,ppc_val);
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

void
nco_ppc_around /* [fnc] Replace op1 values by their values rounded to decimal precision prc */
(const int ppc, /* I [nbr] Precision-preserving compression, i.e., number of total or decimal significant digits */
 const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1) /* I/O [val] Values of first operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */

  /* Purpose: Implement NCO DSD PPC algorithm from Zen16
     Replace op1 values by their values rounded to decimal precision ppc
     Similar to numpy.around() function, hence the name around()
     Based on implementation by Jeff Whitaker for netcdf4-python described here:
     http://netcdf4-python.googlecode.com/svn/trunk/docs/netCDF4-module.html
     which invokes the numpy.around() function documented here:
     http://docs.scipy.org/doc/numpy/reference/generated/numpy.around.html#numpy.around
     A practical discussion of rounding is at
     http://stackoverflow.com/questions/20388071/what-are-the-under-the-hood-differences-between-round-and-numpy-round
     This mentions the () NumPy source code:
     https://github.com/numpy/numpy/blob/7b2f20b406d27364c812f7a81a9c901afbd3600c/numpy/core/src/multiarray/calculation.c#L588

     Manually determine scale:
     ncap2 -O -v -s 'ppc=2;ppc_abs=abs(ppc);bit_nbr_xct=ppc_abs*ln(10.)/ln(2.);bit_nbr_int=ceil(bit_nbr_xct);scale=pow(2.0,bit_nbr_int);' ~/nco/data/in.nc ~/foo.nc 
     ncks -H ~/foo.nc

     Test full algorithm:
     ncks -4 -O -C -v ppc_dbl,ppc_big --ppc ppc_dbl=3 --ppc ppc_big=-2 ~/nco/data/in.nc ~/foo.nc

     Compare to Jeff Whitaker's nc3tonc4 results:
     nc3tonc4 -o --quantize=ppc_dbl=3,ppc_big=-2 ~/nco/data/in.nc ~/foo.nc
     ncks -H -C -v ppc_dbl,ppc_big ~/foo.nc */
  
  /* Rounding is currently defined as op1:=around(op1,ppc) */  
  
  /* Use constants defined in math.h */
  const double bit_per_dcm_dgt_prc=M_LN10/M_LN2; /* 3.32 [frc] Bits per decimal digit of precision */

  double scale; /* [frc] Number by which to scale data to achieve rounding */
  float scalef; /* [frc] Number by which to scale data to achieve rounding */

  int bit_nbr; /* [nbr] Number of bits required to exceed pow(10,-ppc) */
  int ppc_abs; /* [nbr] Absolute value of precision */

  long idx;
  
  /* Only numeric types can be quantized */
  if(type == NC_CHAR || type == NC_BYTE || type == NC_UBYTE || type == NC_STRING) return;

  ppc_abs=abs(ppc);
  assert(ppc_abs <= 16);
  switch(ppc_abs){
  case 0:
    bit_nbr=0;
    scale=1.0;
    break;
  case 1:
    bit_nbr=4;
    scale=16.0;
    break;
  case 2:
    bit_nbr=7;
    scale=128.0;
    break;
  case 3:
    bit_nbr=10;
    scale=1024.0;
    break;
  case 4:
    bit_nbr=14;
    scale=16384.0;
    break;
  case 5:
    bit_nbr=17;
    scale=131072.0;
    break;
  case 6:
    bit_nbr=20;
    scale=1048576.0;
    break;
  default:
    bit_nbr=(int)ceil(ppc_abs*bit_per_dcm_dgt_prc);
    scale=pow(2.0,bit_nbr);
    break;
  } /* end switch */   
  if(ppc < 0) scale=1.0/scale;

  if(nco_dbg_lvl_get() == nco_dbg_sbr) (void)fprintf(stdout,"%s: INFO nco_ppc_around() reports ppc = %d, bit_nbr= %d, scale = %g\n",nco_prg_nm_get(),ppc,bit_nbr,scale);

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  scalef=(float)scale;
  switch(type){
  case NC_FLOAT: 
    /* By default do float arithmetic in double precision before converting back to float
       Allow --flt to override
       NB: Use rint() not lrint()
       If ignoring this advice, be sure to bound calls to lrint(), e.g., 
       rint_arg=scale*op1.fp[idx];
       if(rint_arg > LONG_MIN && rint_arg < LONG_MAX) op1.fp[idx]=(float)lrint(scale*op1.fp[idx])/scale; */
    if(!has_mss_val){
      if(nco_rth_cnv_get() == nco_rth_flt_flt)
	for(idx=0L;idx<sz;idx++) op1.fp[idx]=rintf(scalef*op1.fp[idx])/scalef;
      else
	for(idx=0L;idx<sz;idx++) op1.fp[idx]=(float)(rint(scale*op1.fp[idx])/scale); /* Coerce to avoid implicit conversions warning */
    }else{
      const float mss_val_flt=*mss_val.fp;
      if(nco_rth_cnv_get() == nco_rth_flt_flt){
	for(idx=0;idx<sz;idx++)
	  if(op1.fp[idx] != mss_val_flt)
	    op1.fp[idx]=rintf(scalef*op1.fp[idx])/scalef;
      }else{
	for(idx=0;idx<sz;idx++)
	  if(op1.fp[idx] != mss_val_flt)
	    op1.fp[idx]=(float)(rint(scale*op1.fp[idx])/scale); /* Coerce to avoid implicit conversions warning */
      } /* end else */
    } /* end else */
    break;
  case NC_DOUBLE: 
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.dp[idx]=rint(scale*op1.dp[idx])/scale;
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++)
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=rint(scale*op1.dp[idx])/scale;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.sp[idx]=(short int)lrint(scale*op1.sp[idx])/scale;
    }else{
      const nco_int mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++)
	if(op1.sp[idx] != mss_val_short) op1.sp[idx]=(short int)lrint(scale*op1.sp[idx])/scale;
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.usp[idx]=(unsigned short int)lrint(scale*op1.usp[idx])/scale;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++)
	if(op1.usp[idx] != mss_val_ushort) op1.usp[idx]=(unsigned short int)lrint(scale*op1.usp[idx])/scale;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.ip[idx]=lrint(scale*op1.ip[idx])/scale;
    }else{
      const nco_int mss_val_int=*mss_val.ip;
      for(idx=0;idx<sz;idx++)
	if(op1.ip[idx] != mss_val_int) op1.ip[idx]=lrint(scale*op1.ip[idx])/scale;
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.uip[idx]=(unsigned int)lrint(scale*op1.uip[idx])/scale;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++)
	if(op1.uip[idx] != mss_val_uint) op1.uip[idx]=(unsigned int)lrint(scale*op1.uip[idx])/scale;
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.i64p[idx]=lrint(scale*op1.i64p[idx])/scale;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++)
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]=lrint(scale*op1.i64p[idx])/scale;
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.ui64p[idx]=(unsigned long)lrint(scale*op1.ui64p[idx])/scale;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++)
	if(op1.ui64p[idx] != mss_val_uint64) op1.ui64p[idx]=(unsigned long)lrint(scale*op1.ui64p[idx])/scale;
    } /* end else */
    break;
  case NC_CHAR: /* Do nothing for non-numeric types ...*/
  case NC_BYTE:
  case NC_UBYTE:
  case NC_STRING: break;
  default: 
    nco_dfl_case_nc_type_err();
    break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_ppc_around() */

void
nco_ppc_bitmask /* [fnc] Mask-out insignificant bits of significand */
(const int nsd, /* I [nbr] Number of significant digits, i.e., arithmetic precision */
 const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1) /* I/O [val] Values of first operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */

  /* Purpose: Implement NCO NSD PPC algorithm from Zen16 by masking-out insignificant bits of op1 values */
  
  /* Rounding is currently defined as op1:=bitmask(op1,ppc) */  
  
  /* This routine implements the Bit Grooming Number of Significant Digits (NSD) algorithm
     NSD based on absolute precision, i.e., number of digits in significand and in decimal scientific notation
     DSD based on precision relative to decimal point, i.e., number of digits before/after decimal point
     DSD is more often used colloquially, e.g., "thermometers measure temperature accurate to 1 degree C" 
     NSD is more often used scientifically, e.g., "thermometers measure temperature to three significant digits"
     These statements are both equivalent and describe the same instrument and data
     If data are stored in C or K then optimal specifications for each algorithm would be DSD=0 and NSD=3
     However, if data are stored in mK (milli-Kelvin) then optimal specifications would be DSD=-3 and NSD=3
     In other words, the number of significant digits (NSD) does not depend on the units of storage, but DSD does
     Hence NSD is more instrinsic and portable than DSD
     NSD requires only bit-shifting and bit-masking, no floating point math
     DSD is implemented with rounding techniques that rely on floating point math
     This makes DSD subject to accompanying overflow and underflow problems when exponent near MAX_EXP/2
     Thus NSD is faster, more accurate, and less ambiguous than DSD
     Nevertheless many users think in terms of DSD not NSD
     
     Terminology: 
     Decimal Precision is number of significant digits following decimal point (DSD)
     Arithmetic Precision is number of significant digits (NSD)
     "Arithmetic precision can also be defined with reference to a fixed number of decimal places (the number of digits following the decimal point). This second definition is useful in applications where the number of digits in the fractional part has particular importance, but it does not follow the rules of significance arithmetic." -- Wikipedia
     "A common convention in science and engineering is to express accuracy and/or precision implicitly by means of significant figures. Here, when not explicitly stated, the margin of error is understood to be one-half the value of the last significant place. For instance, a recording of 843.6 m, or 843.0 m, or 800.0 m would imply a margin of 0.05 m (the last significant place is the tenths place), while a recording of 8,436 m would imply a margin of error of 0.5 m (the last significant digits are the units)." -- Wikipedia
     
     Test NSD:
     nc3tonc4 -o --quantize=ppc_big=3,ppc_bgr=3,ppc_flt=3 --quiet=1 ~/nco/data/in.nc ~/foo_n34.nc
     ncks -D 1 -4 -O -C -v ppc_big,ppc_bgr,ppc_flt --ppc .?=3 ~/nco/data/in.nc ~/foo.nc
     ncks -C -v ppc_big,ppc_bgr ~/foo.nc
     ncks -s '%16.12e\n' -C -H -v ppc_big,ppc_bgr ~/foo_n34.nc */
  
  /* IEEE single- and double-precision significands have 24 and 53 bits of precision (prc_bnr)
     Decimal digits of precision (prc_dcm) obtained via prc_dcm=prc_bnr*ln(2)/ln(10) = 7.22 and 15.95, respectively
     Binary digits of precision (prc_bnr) obtained via prc_bnr=prc_dcm*ln(10)/ln(2) */
  
  /* Use constants defined in math.h */
  const double bit_per_dcm_dgt_prc=M_LN10/M_LN2; /* 3.32 [frc] Bits per decimal digit of precision */
  const double dcm_per_bit_dgt_prc=M_LN2/M_LN10; /* 0.301 [frc] Bits per decimal digit of precision */
  
  const int bit_xpl_nbr_sgn_flt=23; /* [nbr] Bits 0-22 of SP significands are explicit. Bit 23 is implicitly 1. */
  const int bit_xpl_nbr_sgn_dbl=53; /* [nbr] Bits 0-52 of DP significands are explicit. Bit 53 is implicitly 1. */
  const int ieee_xpn_fst_flt=127; /* [nbr] IEEE "exponent bias" = actual exponent minus stored exponent */
  //const int ieee_xpn_fst_dbl=1023; /* [nbr] IEEE "exponent bias" = actual exponent minus stored exponent */
  
  double prc_bnr_xct; /* [nbr] Binary digits of precision, exact */
  
  int bit_xpl_nbr_sgn=int_CEWI; /* [nbr] Number of explicit bits in significand */
  int bit_xpl_nbr_zro; /* [nbr] Number of explicit bits to zero */

  long idx;

  unsigned int *u32_ptr;
  unsigned int msk_f32_u32_zro;
  unsigned int msk_f32_u32_one;
  unsigned int msk_f32_u32_hshv;
  unsigned long long int *u64_ptr;
  unsigned long long int msk_f64_u64_zro;
  unsigned long long int msk_f64_u64_one;
  unsigned long long int msk_f64_u64_hshv;
  unsigned short prc_bnr_ceil; /* [nbr] Exact binary digits of precision rounded-up */
  unsigned short prc_bnr_xpl_rqr; /* [nbr] Explicitly represented binary digits required to retain */
  
  /* Only floating point types can be quantized */
  if(type != NC_FLOAT && type != NC_DOUBLE) return;

  /* Disallow unreasonable quantization */
  assert(nsd > 0);
  assert(nsd <= 16);

  /* How many bits to preserve? */
  prc_bnr_xct=nsd*bit_per_dcm_dgt_prc;
  /* Be conservative, round upwards */
  prc_bnr_ceil=(unsigned short)ceil(prc_bnr_xct);
  /* First bit is implicit not explicit but corner cases prevent our taking advantage of this */
  //prc_bnr_xpl_rqr=prc_bnr_ceil-1; /* 20201223 CSZ verified this fails for small integers with NSD=1 */
  //prc_bnr_xpl_rqr=prc_bnr_ceil;
  prc_bnr_xpl_rqr=prc_bnr_ceil+1;
  if(type == NC_DOUBLE) prc_bnr_xpl_rqr++; /* Seems necessary for double-precision ppc=array(1.234567,1.0e-6,$dmn) */
  /* 20150128: Hand-tuning shows we can sacrifice one or two more bits for almost all cases
     20150205: However, small integers are an exception. In fact they require two more bits, at least for NSD=1.
     Thus minimum threshold to preserve half of least significant digit (LSD) is prc_bnr_xpl_rqr=prc_bnr_ceil
     Decrementing prc_bnr_xpl_rqr by one or two more bits produces maximum errors that exceed half the LSD
     ncra -4 -O -C --ppc default=1 --ppc one=1 -p ~/nco/data in.nc in.nc ~/foo.nc 
     ncks -H -v Q.. --cdl ~/foo.nc | m */

  /* 20150126: fxm casting pointers is tricky with this routine. Avoid for now. */
  /* Typecast pointer to values before access */
  //(void)cast_void_nctype(type,&op1);
  //if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  if(type == NC_FLOAT  && prc_bnr_xpl_rqr >= bit_xpl_nbr_sgn_flt) return;
  if(type == NC_DOUBLE && prc_bnr_xpl_rqr >= bit_xpl_nbr_sgn_dbl) return;

  switch(type){
  case NC_FLOAT:
    bit_xpl_nbr_sgn=bit_xpl_nbr_sgn_flt;
    bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
    assert(bit_xpl_nbr_zro <= bit_xpl_nbr_sgn-NCO_PPC_BIT_XPL_NBR_MIN);
    u32_ptr=op1.uip;
    /* Create mask */
    msk_f32_u32_zro=0u; /* Zero all bits */
    msk_f32_u32_zro=~msk_f32_u32_zro; /* Turn all bits to ones */
    /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
    msk_f32_u32_zro <<= bit_xpl_nbr_zro;
    /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
    msk_f32_u32_one=~msk_f32_u32_zro;
    msk_f32_u32_hshv=msk_f32_u32_one & (msk_f32_u32_zro >> 1); /* Set one bit: the MSB of LSBs */
    if(nco_baa_cnv_get() == nco_baa_grm){
      /* Bit-Groom: alternately shave and set LSBs */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx+=2L) u32_ptr[idx]&=msk_f32_u32_zro;
	for(idx=1L;idx<sz;idx+=2L)
	  if(u32_ptr[idx] != 0U) /* Never quantize upwards floating point values of zero */
	    u32_ptr[idx]|=msk_f32_u32_one;
      }else{
	const float mss_val_flt=*mss_val.fp;
	for(idx=0L;idx<sz;idx+=2L)
	  if(op1.fp[idx] != mss_val_flt) u32_ptr[idx]&=msk_f32_u32_zro;
	for(idx=1L;idx<sz;idx+=2L)
	  if(op1.fp[idx] != mss_val_flt && u32_ptr[idx] != 0U) u32_ptr[idx]|=msk_f32_u32_one;
      } /* end else */
    }else if(nco_baa_cnv_get() == nco_baa_shv){
      /* Bit-Shave: always shave LSBs */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx++) u32_ptr[idx]&=msk_f32_u32_zro;
      }else{
	const float mss_val_flt=*mss_val.fp;
	for(idx=0L;idx<sz;idx++)
	  if(op1.fp[idx] != mss_val_flt) u32_ptr[idx]&=msk_f32_u32_zro;
      } /* end else */
    }else if(nco_baa_cnv_get() == nco_baa_set){
      /* Bit-Set: always set LSBs */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx++)
	  if(u32_ptr[idx] != 0U) /* Never quantize upwards floating point values of zero */
	    u32_ptr[idx]|=msk_f32_u32_one;
      }else{
	const float mss_val_flt=*mss_val.fp;
	for(idx=0L;idx<sz;idx++)
	  if(op1.fp[idx] != mss_val_flt) u32_ptr[idx]|=msk_f32_u32_one;
      } /* end else */
    }else if(nco_baa_cnv_get() == nco_baa_dgr){
      /* Digit Rounding (GCD19), an unfinished work in progress...
	 Test DGR:
	 ccc --tst=bnr --flt_foo=8 2> /dev/null | grep "Binary of float"
	 ncks -O -C -D 1 --baa=3 -v ppc_bgr --ppc default=3 ~/nco/data/in.nc ~/foo.nc
	 ncks -O -C -D 1 --baa=3 -v one_dmn_rec_var_flt --ppc default=3 ~/nco/data/in.nc ~/foo.nc */
      unsigned char u8_xpn; /* dgr */
      unsigned int u32_xpn; /* dgr */
      unsigned int u32_dpl; /* dgr */
      unsigned int msk_u32_xpn;
      int i32_xpn; /* dgr */
      int dgt_nbr_pre_dcm; /* Number of decimal digits before decimal point d_i in DCG19 (7) */
      int qnt_pwr_xpn; /* Quantization power exponent p_i in DCG19 (6) */
      float qnt_fct_flt; /* Quantization factor q_i in DCG19 (5) */
      float val_qnt; /* Quantized value DCG19 (1) */
      //int sgn_val; /* Sign of  DCG19 (5) */
      /* Create mask */
      msk_u32_xpn=0u; /* Zero all bits */
      msk_u32_xpn=~msk_u32_xpn; /* Turn all bits to ones */
      msk_u32_xpn>>=24; /* Right-shift zeros into bits to be zerod, leave ones in exponent bits */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx+=2L){
	  u32_xpn=u32_ptr[idx]; // Copy memory so bitshifting exponent to byte boundary does not corrupt original
	  u32_xpn>>=bit_xpl_nbr_sgn_flt; // Bit-shift exponent to end of word
	  u32_xpn&=msk_u32_xpn; // Mask to elimnate sign bit
	  i32_xpn=(int)u32_xpn-ieee_xpn_fst_flt; // Subtract 127 to compensate for IEEE SP exponent bias
	  /* Number of decimal digits before decimal point */
	  //	  dgt_nbr_pre_dcm=(i32_xpn > 0) ? (int)((i32_xpn-1)*dcm_per_bit_dgt_prc)+1 : 0; /* d_i DCG19 (7) */
	  dgt_nbr_pre_dcm=(int)((i32_xpn-1)*dcm_per_bit_dgt_prc)+1; /* d_i DCG19 (7) */
	  qnt_pwr_xpn=(dgt_nbr_pre_dcm > nsd) ? (int)((dgt_nbr_pre_dcm-nsd)*bit_per_dcm_dgt_prc) : 0; /* p_i DCG19 (6) */
	  qnt_fct_flt=pow(2,qnt_pwr_xpn); /* Quantization factor q_i DCG19 (5) (as floating point) */
	  /* Quantize value */
	  val_qnt=op1.fp[idx];
	  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: DEBUG nco_ppc_bitmask() reports val_qnt1 = %g\n",nco_prg_nm_get(),val_qnt);
	  val_qnt=(int)(fabs(val_qnt)/qnt_fct_flt);
	  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: DEBUG nco_ppc_bitmask() reports val_qnt2 = %g\n",nco_prg_nm_get(),val_qnt);
	  val_qnt+=0.5f;
	  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: DEBUG nco_ppc_bitmask() reports val_qnt3 = %g\n",nco_prg_nm_get(),val_qnt);
	  val_qnt*=qnt_fct_flt;
	  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: DEBUG nco_ppc_bitmask() reports val_qnt4 = %g\n",nco_prg_nm_get(),val_qnt);
	  val_qnt*=(op1.fp[idx] > 0) ? 1 : -1;
	  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: DEBUG nco_ppc_bitmask() reports val_qnt5 = %g\n",nco_prg_nm_get(),val_qnt);
	  u32_dpl=u32_ptr[idx];
	  // u32_dpl= /* Take absolute value by placing 0 in sign bit */
	  u32_dpl>>=qnt_pwr_xpn; /* Divide by q_i same as right-shifting by p_i */
	  	  
	  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: DEBUG nco_ppc_bitmask() reports val = %g, u32_xpn = %u, i32_xpn=%d, dgt_nbr_pre_dcm = d_i = %d, qnt_pwr_xpn = p_i = %d, qnt_fct_flt = q_i = %g\n",nco_prg_nm_get(),op1.fp[idx],u32_xpn,i32_xpn,dgt_nbr_pre_dcm,qnt_pwr_xpn,qnt_fct_flt);
	} /* !idx */
	for(idx=1L;idx<sz;idx+=2L){
	  u8_xpn='0'; /* CEWI */
	  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: DEBUG nco_ppc_bitmask() reports val = %g, u8_xpn_nbr = %hhu\n",nco_prg_nm_get(),op1.fp[idx],u8_xpn);
	  if(u32_ptr[idx] != 0U) /* Never quantize upwards floating point values of zero */
	    u32_ptr[idx]|=msk_f32_u32_one;
	} /* !idx */
      }else{
	const float mss_val_flt=*mss_val.fp;
	for(idx=0L;idx<sz;idx+=2L)
	  if(op1.fp[idx] != mss_val_flt) u32_ptr[idx]&=msk_f32_u32_zro;
	for(idx=1L;idx<sz;idx+=2L)
	  if(op1.fp[idx] != mss_val_flt && u32_ptr[idx] != 0U) u32_ptr[idx]|=msk_f32_u32_one;
      } /* end else */
    }else if(nco_baa_cnv_get() == nco_baa_bg2){
      /* Bit-Groom2: alternately shave and set LSBs with dynamic masks
	 Test BG2:
	 ccc --tst=bnr --flt_foo=8 2> /dev/null | grep "Binary of float"
	 ncks -O -C -D 1 --baa=4 -v ppc_bgr --ppc default=3 ~/nco/data/in.nc ~/foo.nc
	 ncks -O -C -D 1 --baa=4 -v one_dmn_rec_var_flt --ppc default=3 ~/nco/data/in.nc ~/foo.nc */
      idx=0L;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: DEBUG nco_ppc_bitmask() reports val = %g\n",nco_prg_nm_get(),op1.fp[idx]);
    }else if(nco_baa_cnv_get() == nco_baa_rnd){
      /* Round mantissa, LSBs to zero contributed by Rostislav Kouznetsov 20200711
	 Round mantissa using floating-point arithmetic, shave LSB using bit-mask */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx++){
          u32_ptr[idx]+=msk_f32_u32_hshv; /* Add 1 at MSB of LSBs, carry 1 to mantissa or even exponent*/
          u32_ptr[idx]&=msk_f32_u32_zro; /* Shave it */
        } /* !idx */
      }else{ /* !has_mss_val */
	const float mss_val_flt=*mss_val.fp;
	for(idx=0L;idx<sz;idx++){
	  if(op1.fp[idx] != mss_val_flt){ 
            u32_ptr[idx]+=msk_f32_u32_hshv;/* add 1 to the MSB of LSBs*/
            u32_ptr[idx]&=msk_f32_u32_zro; /* Shave it */
	  } /* !mss_val_flt */
	} /* !idx */
      } /* !has_mss_val */
    }else if(nco_baa_cnv_get() == nco_baa_sh2){
      /* Bit Half-Shave contributed by Rostislav Kouznetsov 20200715
	 Shave LSBs and set MSB of them
	 See figures at https://github.com/nco/nco/pull/200 */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx++){
	  u32_ptr[idx]&=msk_f32_u32_zro; /* Shave as normal */
	  u32_ptr[idx]|=msk_f32_u32_hshv; /* Set MSB of LSBs */
        } /* !idx */
      }else{
	const float mss_val_flt=*mss_val.fp;
	for(idx=0L;idx<sz;idx++){
	  if(op1.fp[idx] != mss_val_flt){
	    u32_ptr[idx]&=msk_f32_u32_zro; /* Shave as normal */
	    u32_ptr[idx]|=msk_f32_u32_hshv; /* Set MSB of LSBs */
	  } /* !mss_val_flt */
	} /* !idx */
      } /* !has_mss_val */
      
    }else if(nco_baa_cnv_get() == nco_baa_gbg){ /* JPT 20210102: baa_gbg (granualar bit grooming), brute force compression of each individual data point */
      bool x;
      const unsigned int msk_rst32 = msk_f32_u32_zro; /* resets the mask to orginal garuntee mask */
      float err_max32;
      float raw32;
      float temp32;
      float xpn32;
      int xpn_flr32;

      if(!has_mss_val){
        for(idx=0L;idx<sz;idx+=2L){ // shave loop
	  if(op1.fp[idx] != 0.0 && op1.fp[idx] != 1.0){
	    raw32 = op1.fp[idx];
	    msk_f32_u32_zro = msk_rst32;
	    op1.uip[idx]&=msk_f32_u32_zro;
	    xpn32=log10f(raw32);
	    xpn_flr32=floor(xpn32);
	    err_max32=0.5*pow(10.0,xpn_flr32-nsd+1);
	    if(op1.fp[idx] != 0.0 && op1.fp[idx] != 1.0){
	      while(fabs(raw32 - op1.fp[idx]) <= err_max32 && op1.fp[idx] != 1.0){
		temp32=op1.fp[idx];
		msk_f32_u32_zro<<=1;
		op1.uip[idx]&=msk_f32_u32_zro;
		//		fflush(stdout);
	      } // close while loop
	      op1.fp[idx] = temp32;
	    } // close if before while
	    //	    if(fabs(raw32 - op1.fp[idx]) >= err_max32) printf("Failed\n"); //test
	  } //close if !=0	  
	} // close shave loop
	
	for(idx=1L;idx<sz;idx+=2L){ // set loop
	  raw32 = op1.fp[idx];
	  msk_f32_u32_zro = msk_rst32;
	  msk_f32_u32_one=~msk_f32_u32_zro; 
          if(op1.fp[idx] != 0U){ /* Never quantize upwards floating point values of zero */
	    op1.uip[idx]|=msk_f32_u32_one;
	    xpn32=log10f(raw32);
	    xpn_flr32=floor(xpn32);
	    err_max32=0.5*pow(10.0,xpn_flr32-nsd+1);
	    x = False;
	    while(fabs(raw32 - op1.fp[idx]) <= err_max32){
	      x = True;
	      temp32 = op1.fp[idx];
	      msk_f32_u32_zro <<= 1;
	      msk_f32_u32_one =~ msk_f32_u32_zro;
	      op1.uip[idx] |= msk_f32_u32_one;
	      fflush(stdout);
	    } //close while
	    if(x) op1.fp[idx] = temp32;
	  } // close if > 0
	  //	  if(fabs(raw32 - op1.fp[idx]) >= err_max32) printf("Failed\n"); // test
	} // close set loop
      } // close if(!has_mss_val)
      
      else{
        const float mss_val_flt=*mss_val.fp;
        for(idx=0L;idx<sz;idx+=2L){
          if(op1.fp[idx] != mss_val_flt && op1.fp[idx] != 0.0){
	    raw32 = op1.fp[idx];
	    msk_f32_u32_zro = msk_rst32;
	    op1.uip[idx]&=msk_f32_u32_zro;
	    xpn32=log10f(raw32);
	    xpn_flr32=floor(xpn32);
	    err_max32=0.5*pow(10.0,xpn_flr32-nsd+1);
	    if(op1.fp[idx] != 0.0 && op1.fp[idx] != 1.0){
	      while(fabs(raw32 - op1.fp[idx]) <= err_max32 && op1.fp[idx] != 1.0){
		temp32=op1.fp[idx];
		msk_f32_u32_zro<<=1;
		op1.uip[idx]&=msk_f32_u32_zro;
		fflush(stdout);
	      } // close while loop
	      op1.fp[idx] = temp32;
	    } // close if before while
	  } // close if 
	} // close shave loop
        for(idx=1L;idx<sz;idx+=2L){
          if(op1.fp[idx] != mss_val_flt && op1.fp[idx] != 0U){
	    raw32 = op1.fp[idx];
	    msk_f32_u32_zro = msk_rst32;
	    msk_f32_u32_one=~msk_f32_u32_zro;
	    op1.uip[idx]|=msk_f32_u32_one;
	    xpn32=log10f(raw32);
	    xpn_flr32=floor(xpn32);
	    err_max32=0.5*pow(10.0,xpn_flr32-nsd+1);
	    x = False;
	    while(fabs(raw32 - op1.fp[idx]) <= err_max32){
	      x = True;
	      temp32 = op1.fp[idx];
	      msk_f32_u32_zro <<= 1;
	      msk_f32_u32_one =~ msk_f32_u32_zro;
	      op1.uip[idx] |= msk_f32_u32_one;
	      fflush(stdout);
	    } //close while
	    if(x) op1.fp[idx] = temp32;
	  } // close if
	} // close set loop 
      } /* end else */
    }else abort();
    break; /* !NC_FLOAT */
  case NC_DOUBLE:
    bit_xpl_nbr_sgn=bit_xpl_nbr_sgn_dbl;
    bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
    assert(bit_xpl_nbr_zro <= bit_xpl_nbr_sgn-NCO_PPC_BIT_XPL_NBR_MIN);
    u64_ptr=(unsigned long long int *)op1.ui64p;
    /* Create mask */
    msk_f64_u64_zro=0ull; /* Zero all bits */
    msk_f64_u64_zro=~msk_f64_u64_zro; /* Turn all bits to ones */
    /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
    msk_f64_u64_zro <<= bit_xpl_nbr_zro;
    /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
    msk_f64_u64_one=~msk_f64_u64_zro;
    msk_f64_u64_hshv=msk_f64_u64_one & (msk_f64_u64_zro >> 1); /* Set one bit: the MSB of LSBs */
    if(nco_baa_cnv_get() == nco_baa_grm){
      /* Bit-Groom: alternately shave and set LSBs */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx+=2L) u64_ptr[idx]&=msk_f64_u64_zro;
	for(idx=1L;idx<sz;idx+=2L)
	  if(u64_ptr[idx] != 0ULL) /* Never quantize upwards floating point values of zero */
	    u64_ptr[idx]|=msk_f64_u64_one;
      }else{
	const double mss_val_dbl=*mss_val.dp;
	for(idx=0L;idx<sz;idx+=2L)
	  if(op1.dp[idx] != mss_val_dbl) u64_ptr[idx]&=msk_f64_u64_zro;
	for(idx=1L;idx<sz;idx+=2L)
	  if(op1.dp[idx] != mss_val_dbl && u64_ptr[idx] != 0ULL) u64_ptr[idx]|=msk_f64_u64_one;
      } /* end else */
    }else if(nco_baa_cnv_get() == nco_baa_shv){
      /* Bit-Shave: always shave LSBs */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx++) u64_ptr[idx]&=msk_f64_u64_zro;
      }else{
	const double mss_val_dbl=*mss_val.dp;
	for(idx=0L;idx<sz;idx++)
	  if(op1.dp[idx] != mss_val_dbl) u64_ptr[idx]&=msk_f64_u64_zro;
      } /* end else */
    }else if(nco_baa_cnv_get() == nco_baa_set){
      /* Bit-Set: always set LSBs */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx++)
	  if(u64_ptr[idx] != 0UL) /* Never quantize upwards floating point values of zero */
	    u64_ptr[idx]|=msk_f64_u64_one;
      }else{
	const double mss_val_dbl=*mss_val.dp;
	for(idx=0L;idx<sz;idx++)
	  if(op1.dp[idx] != mss_val_dbl && u64_ptr[idx] != 0UL) u64_ptr[idx]|=msk_f64_u64_one;
      } /* end else */
    }else if(nco_baa_cnv_get() == nco_baa_bg2){
      /* Bit-Grooming Version 2: alternately shave and set LSBs with dynamic masks, an unfinished work in progress... */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx+=2L) u64_ptr[idx]&=msk_f64_u64_zro;
	for(idx=1L;idx<sz;idx+=2L)
	  if(u64_ptr[idx] != 0UL) /* Never quantize upwards floating point values of zero */
	    u64_ptr[idx]|=msk_f64_u64_one;
      }else{
	const double mss_val_dbl=*mss_val.dp;
	for(idx=0L;idx<sz;idx+=2L)
	  if(op1.dp[idx] != mss_val_dbl) u64_ptr[idx]&=msk_f64_u64_zro;
	for(idx=1L;idx<sz;idx+=2L)
	  if(op1.dp[idx] != mss_val_dbl && u64_ptr[idx] != 0UL) u64_ptr[idx]|=msk_f64_u64_one;
      } /* end else */
    }else if(nco_baa_cnv_get() == nco_baa_rnd){
      /* Round mantissa, LSBs to zero contributed by Rostislav Kouznetsov 20200711
	 Round mantissa using floating-point arithmetic, shave LSB using bit-mask
	 See figures at https://github.com/nco/nco/pull/199 */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx++){
          u64_ptr[idx]+=msk_f64_u64_hshv; /* Add 1 at MSB of LSBs */
          u64_ptr[idx]&=msk_f64_u64_zro; /* Shave it */
        } /* !idx */
      }else{ /* !has_mss_val */
	const double mss_val_dbl=*mss_val.dp;
	for(idx=0L;idx<sz;idx++){
	  if(op1.dp[idx] != mss_val_dbl){ 
            u64_ptr[idx]+=msk_f64_u64_hshv; /* Add 1 at MSB of LSBs */
            u64_ptr[idx]&=msk_f64_u64_zro; /* Shave it */
	  } /* !mss_val_dbl */
	} /* !idx */
      } /* !has_mss_val */
    }else if(nco_baa_cnv_get() == nco_baa_sh2){
      /* Bit Half-Shave contributed by Rostislav Kouznetsov 20200715
	 Shave LSBs and set MSB of them
	 See figures at https://github.com/nco/nco/pull/200 */
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx++){
	  u64_ptr[idx]&=msk_f64_u64_zro; /* Shave as normal */
	  u64_ptr[idx]|=msk_f64_u64_hshv; /* Set MSB of LSBs */
        } /* !idx */
      }else{
	const double mss_val_dbl=*mss_val.dp;
	for(idx=0L;idx<sz;idx++){
	  if(op1.dp[idx] != mss_val_dbl){
	    u64_ptr[idx]&=msk_f64_u64_zro; /* Shave as normal */
	    u64_ptr[idx]|=msk_f64_u64_hshv; /* Set MSB of LSBs */
          } /* !mss_val_dbl */
	} /* !idx */
      } /* !has_mss_val */
      
    }else if(nco_baa_cnv_get() == nco_baa_gbg){ /* JPT 20210102: baa_gbg (granualar bit grooming), brute force compression of each individual data point */ 
      double raw64;
      double temp64;
      const unsigned long int msk_rst64 = msk_f64_u64_zro;
      double xpn64;
      int xpn_flr64;
      double err_max64;
      bool x;
      
      if(!has_mss_val){
	for(idx=0L;idx<sz;idx+=2L){ // shave loop
	  if(op1.dp[idx] != 0.0 && op1.dp[idx] != 1.0){
	    raw64 = op1.dp[idx];
	    msk_f64_u64_zro = msk_rst64;
	    u64_ptr[idx]&=msk_f64_u64_zro;
	    xpn64=log10(raw64);
	    xpn_flr64=floor(xpn64);
	    err_max64=0.5*pow(10.0,xpn_flr64-nsd+1);
	    if(op1.dp[idx] != 0.0 && op1.dp[idx] != 1.0){
	      while(fabs(raw64 - op1.dp[idx]) <= err_max64 && op1.dp[idx] != 1.0){
		temp64  = op1.dp[idx];
		msk_f64_u64_zro<<=1;
		u64_ptr[idx]&=msk_f64_u64_zro;
		fflush(stdout);
	      } // close while loop
	      op1.dp[idx] = temp64;
	    } // close if before while
	    //if(fabs(raw64 - op1.dp[idx]) >= err_max64) printf("Failed\n"); //test
	  } // close if != 0
	} // close shave loop
	
	for(idx=1L;idx<sz;idx+=2L){ // set loop
	  raw64 = op1.dp[idx];
	  msk_f64_u64_zro = msk_rst64;
	  msk_f64_u64_one=~msk_f64_u64_zro;
          if(op1.dp[idx] != 0U){ /* Never quantize upwards floating point values of zero */
            u64_ptr[idx]|=msk_f64_u64_one;
	    xpn64=log10(raw64);
	    xpn_flr64=floor(xpn64);
	    err_max64=0.5*pow(10.0,xpn_flr64-nsd+1);
	    x = False;
	    while(fabs(raw64 - op1.dp[idx]) <= err_max64){
	      x = True;
	      temp64 = op1.dp[idx];
	      msk_f64_u64_zro <<= 1;
	      msk_f64_u64_one =~ msk_f64_u64_zro;
	      u64_ptr[idx]|= msk_f64_u64_one;
	      fflush(stdout);
	    } //close while
	    if(x) op1.dp[idx] = temp64;
	  } // close if 0
	} // close set loop
      }else{
	const double mss_val_dbl=*mss_val.dp;
	for(idx=0L;idx<sz;idx+=2L){
	  if(op1.dp[idx] != mss_val_dbl && op1.dp[idx] != 0.0){
	    raw64 = op1.dp[idx];
	    msk_f64_u64_zro = msk_rst64;
	    u64_ptr[idx]&=msk_f64_u64_zro;
	    xpn64=log10(raw64);
	    xpn_flr64=floor(xpn64);
	    err_max64=0.5*pow(10.0,xpn_flr64-nsd+1);
	    if(op1.dp[idx] != 0.0 && op1.dp[idx] != 1.0){
	      while(fabs(raw64 - op1.dp[idx]) <= err_max64 && op1.dp[idx] != 1.0){
		temp64  = op1.dp[idx];
		msk_f64_u64_zro<<=1;
		u64_ptr[idx]&=msk_f64_u64_zro;
		fflush(stdout);
	      } // close while loop
	      op1.dp[idx] = temp64;
	    } // close if before while
	  } // close if     
	} // close shave loop 
	for(idx=1L;idx<sz;idx+=2L){
	  if(op1.dp[idx] != mss_val_dbl && op1.dp[idx] != 0ULL){
	    raw64 = op1.dp[idx];
	    msk_f64_u64_zro = msk_rst64;
	    msk_f64_u64_one=~msk_f64_u64_zro;
	    u64_ptr[idx]|=msk_f64_u64_one;
	    xpn64=log10(raw64);
	    xpn_flr64=floor(xpn64);
	    err_max64=0.5*pow(10.0,xpn_flr64-nsd+1);
	    x = False;
	    while(fabs(raw64 - op1.dp[idx]) <= err_max64){
	      x = True;
	      temp64 = op1.dp[idx];
	      msk_f64_u64_zro <<= 1;
	      msk_f64_u64_one =~ msk_f64_u64_zro;
	      u64_ptr[idx]|= msk_f64_u64_one;
	      fflush(stdout);
	    } //close while
	    if(x) op1.dp[idx] = temp64;
	  } // close if != 
	} //close set loop
      }//close else
    } else // closes baa_gbg
      abort();
    break; /* !NC_DOUBLE */
  case NC_INT: /* Do nothing for non-floating point types ...*/
  case NC_SHORT:
  case NC_CHAR:
  case NC_BYTE:
  case NC_UBYTE:
  case NC_USHORT:
  case NC_UINT:
  case NC_INT64:
  case NC_UINT64:
  case NC_STRING: break;
  default: 
    nco_dfl_case_nc_type_err();
    break;
  } /* end switch */
  
  /* 20150126: fxm casting pointers is tricky with this routine. Avoid for now. */
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end nco_ppc_bitmask() */

double /* [frc] Rounded value */
nco_ppc_bitmask_scl /* [fnc] Round input value significand by specified number of bits */
(const double val_xct, /* I [frc] Exact value to round */
 const int bit_xpl_nbr_zro) /* I [nbr] Bits to round */
{
  /* Purpose: Mask-out bit_xpl_nbr_zro least most significant bits of a scalar double precision value
     Code originally from nco_ppc_bitmask() (bitmasking is my signature move)
     Code used in nco_rgr_wgt() when diagnosing whether quadrature weights properly normalized */

  const int bit_xpl_nbr_sgn_dbl=53; /* [nbr] Bits 0-52 of DP significands are explicit. Bit 53 is implicit. */
  double val_rnd; /* [frc] Rounded version of exact value */
  unsigned long int *u64_ptr;
  unsigned long int msk_f64_u64_zro;
  assert(bit_xpl_nbr_zro >= 0 && bit_xpl_nbr_zro <= bit_xpl_nbr_sgn_dbl-NCO_PPC_BIT_XPL_NBR_MIN);
  CEWI_unused(bit_xpl_nbr_sgn_dbl);
  
  val_rnd=val_xct;
  msk_f64_u64_zro=0ul; /* Zero all bits */
  msk_f64_u64_zro=~msk_f64_u64_zro; /* Turn all bits to ones */
  msk_f64_u64_zro <<= bit_xpl_nbr_zro;
  u64_ptr=(unsigned long int *)&val_rnd;
  *u64_ptr&=msk_f64_u64_zro;

  return val_rnd;
} /* end nco_ppc_bitmask_scl() */
