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
  sng_fnl=nco_join_sng(ppc_arg,ppc_arg_nbr);
  ppc_lst=nco_arg_mlt_prs(sng_fnl);

  if(sng_fnl) sng_fnl=(char *)nco_free(sng_fnl);

  /* Compute ppc_var_nbr */
  for(int idx=0;(ppc_lst+idx)->key;idx++,ppc_var_nbr++);

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
	if(nco_baa_cnv_get() != nco_baa_btr && nco_baa_cnv_get() != nco_baa_sh2){
	  if(trv_tbl->lst[idx_tbl].ppc > nco_max_ppc){
	    if(trv_tbl->lst[idx_tbl].flg_nsd) (void)fprintf(stdout,"%s: INFO Number of Significant Digits (NSD) requested = %d too high for variable %s which is of type %s. No quantization or rounding will be performed for this variable. HINT: Maximum precisions for NC_FLOAT and NC_DOUBLE are %d and %d, respectively.\n",nco_prg_nm_get(),trv_tbl->lst[idx_tbl].ppc,trv_tbl->lst[idx_tbl].nm,nco_typ_sng(trv_tbl->lst[idx_tbl].var_typ),nco_max_ppc_flt,nco_max_ppc_dbl);
	    trv_tbl->lst[idx_tbl].ppc=NC_MAX_INT;
	  } /* !nco_max_ppc */
	} /* !nco_baa_btr, nco_baa_sh2 */
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
} /* !nco_ppc_set_dflt() */

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
    } /* !ppc_val */    
  } /* !NSD */

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
} /* !nco_ppc_set_var() */

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
  const double bit_per_dgt=M_LN10/M_LN2; /* 3.32 [frc] Bits per decimal digit of precision */

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
    bit_nbr=(int)ceil(ppc_abs*bit_per_dgt);
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
      } /* !nco_rth_cnv_get() */
    } /* !has_mss_val */
    break;
  case NC_DOUBLE: 
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.dp[idx]=rint(scale*op1.dp[idx])/scale;
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++)
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=rint(scale*op1.dp[idx])/scale;
    } /* !has_mss_val */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.sp[idx]=(short int)lrint(scale*op1.sp[idx])/scale;
    }else{
      const nco_int mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++)
	if(op1.sp[idx] != mss_val_short) op1.sp[idx]=(short int)lrint(scale*op1.sp[idx])/scale;
    } /* !has_mss_val */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.usp[idx]=(unsigned short int)lrint(scale*op1.usp[idx])/scale;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++)
	if(op1.usp[idx] != mss_val_ushort) op1.usp[idx]=(unsigned short int)lrint(scale*op1.usp[idx])/scale;
    } /* !has_mss_val */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.ip[idx]=lrint(scale*op1.ip[idx])/scale;
    }else{
      const nco_int mss_val_int=*mss_val.ip;
      for(idx=0;idx<sz;idx++)
	if(op1.ip[idx] != mss_val_int) op1.ip[idx]=lrint(scale*op1.ip[idx])/scale;
    } /* !has_mss_val */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.uip[idx]=(unsigned int)lrint(scale*op1.uip[idx])/scale;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++)
	if(op1.uip[idx] != mss_val_uint) op1.uip[idx]=(unsigned int)lrint(scale*op1.uip[idx])/scale;
    } /* !has_mss_val */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.i64p[idx]=lrint(scale*op1.i64p[idx])/scale;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++)
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]=lrint(scale*op1.i64p[idx])/scale;
    } /* !has_mss_val */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0L;idx<sz;idx++) op1.ui64p[idx]=(unsigned long long)lrint(scale*op1.ui64p[idx])/scale;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++)
	if(op1.ui64p[idx] != mss_val_uint64) op1.ui64p[idx]=(unsigned long long)lrint(scale*op1.ui64p[idx])/scale;
    } /* !has_mss_val */
    break;
  case NC_CHAR: /* Do nothing for non-numeric types ...*/
  case NC_BYTE:
  case NC_UBYTE:
  case NC_STRING: break;
  default: 
    nco_dfl_case_nc_type_err();
    break;
  } /* !nc_type */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* !nco_ppc_around() */

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
  
  /* This routine implements the BitGrooming Number of Significant Digits (NSD) algorithm
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
  
  /* IEEE single- and double-precision significands have 24 and 53 bits of total precision (prc_bnr)
     Decimal digits of precision (prc_dcm) obtained via prc_dcm=prc_bnr*ln(2)/ln(10) = 7.22 and 15.95, respectively
     Binary "digits" (i.e., bits) of precision (prc_bnr) obtained via prc_bnr=prc_dcm*ln(10)/ln(2) */
  
  const char fnc_nm[]="nco_ppc_bitmask()"; /* [sng] Function name  */

  /* Use constants defined in math.h */
  const double bit_per_dgt=M_LN10/M_LN2; /* 3.32 [frc] Bits per decimal digit of precision = log2(10) */
  const double dgt_per_bit=M_LN2/M_LN10; /* 0.301 [frc] Decimal digits per bit of precision = log10(2) */
  const double dgt_per_bit_dgr=0.301029996; /* 0.301 [frc] Decimal digits per bit of precision = log10(2) used in DigitRound algorithm and code */
  
  const int bit_xpl_nbr_sgn_flt=23; /* [nbr] Bits 0-22 of SP significands are explicit. Bit 23 is implicitly 1. */
  const int bit_xpl_nbr_sgn_dbl=52; /* [nbr] Bits 0-51 of DP significands are explicit. Bit 52 is implicitly 1. */
  //const int ieee_xpn_fst_flt=127; /* [nbr] IEEE "exponent bias" = actual exponent minus stored exponent */
  //const int ieee_xpn_fst_dbl=1023; /* [nbr] IEEE "exponent bias" = actual exponent minus stored exponent */
  
  /* Begin Digit Rounding (DCG19) declarations:
     Table contains approximate value v for log10(mnt) for five "tenths" ranges of mantissa
     These are pre-tabulated rather than computed in-line to save time 
     It is guaranteed that v < log10(mnt) for mnt in each range
     Approximation is "conservative" (preserves slightly more precision than necessary)
     20210928: Promote table from float->double to quiet MSVC warnings */
  const double mnt_log10_tbl_dgr[5][2]=
    { /* NB: Digit Round paper and code uses 9-digits (not 15-digits) of precision
	 NCO does the same to faithfully implement/represent their algorithm */
      {0.6,-0.301029996}, /* Approximate log10(mnt) for mantissas in [0.5,0.6) as log10(0.5) = log10(2^(-1)) = -log10(2) = -dgt_per_bit = -0.301 */
      {0.7,-0.221848749}, /* Approximate log10(mnt) for mantissas in [0.6,0.7) as log10(0.6) = -0.222 */
      {0.8,-0.154901959}, /* Approximate log10(mnt) for mantissas in [0.7,0.8) as log10(0.7) = -0.155 */
      {0.9,-0.096910013}, /* Approximate log10(mnt) for mantissas in [0.8,0.9) as log10(0.8) = -0.0969 */
      {1.0,-0.045757490}, /* Approximate log10(mnt) for mantissas in [0.9,1.0) as log10(0.9) = -0.0458 */
    }; /* !mnt_log10_tbl_dgr */
#if 0 /* Not currently used */
  const double mnt_log10_tbl_gbr[5][2]=
    { /* NB: Granular BitGroom extends Digit Round tabular precision from 9 to 15 digits */
     {0.6,-dgt_per_bit}, /* Approximate log10(mnt) for mantissas in [0.5,0.6) as log10(0.5) = log10(2^(-1)) = -log10(2) = -dgt_per_bit = -0.301 */
     {0.7,-0.221848749616356}, /* Approximate log10(mnt) for mantissas in [0.6,0.7) as log10(0.6) = -0.222 */
     {0.8,-0.154901959985743}, /* Approximate log10(mnt) for mantissas in [0.7,0.8) as log10(0.7) = -0.155 */
     {0.9,-0.096910013008056}, /* Approximate log10(mnt) for mantissas in [0.8,0.9) as log10(0.8) = -0.0969 */
     {1.0,-0.045757490560675}, /* Approximate log10(mnt) for mantissas in [0.9,1.0) as log10(0.9) = -0.0458 */
    }; /* !mnt_log10_tbl_gbr */
#endif /* !0 */
  double mnt; /* [frc] Mantissa, 0.5 <= mnt < 1.0 */
  double mnt_fabs; /* [frc] fabs(mantissa) */
  double mnt_log10_fabs; /* [frc] log10(fabs(mantissa))) */
  double mnt_log10_prx; /* [frc] Table-based approximation to log10(mnt) */
  double qnt_fct; /* [frc] Greatest power of two bitmask for quantization */
  double qnt_grr; /* [frc] Guaranteed maximum allowed quantization error */ 
  double qnt_prx; /* [frc] Quantization approximation */
  double qnt_val; /* [frc] Quantized value */
  double val; /* [frc] Copy of input value to avoid indirection */
  
  int dgt_nbr; /* [nbr] Number of digits before decimal point */
  int qnt_pwr; /* [nbr] Power of two in quantization mask: qnt_msk = 2^qnt_pwr */
  int tbl_idx; /* [idx] Index into pre-computed lookup table for log10(mnt) or log2(mnt) */
  int xpn_bs2; /* [nbr] Binary exponent xpn_bs2 in val = sign(val) * 2^xpn_bs2 * mnt, 0.5 < mnt <= 1.0 */
  /* End Digit Rounding (DCG19) declarations */

  /* Begin Brute Force declarations */
  unsigned int msk_rst32; /* Set mask to original BG mask */
  nco_bool flg_qnt_nxt_bit; /* [flg] Quantize (set or shave) next bit */
  nco_bool x; /* [flg] */
  float err_max32;  /* Maximum quantization error allowed by user-specified NSD */
  float raw32; /* Unquantized value of current element */
  float tmp32; /* Provisional quantized value of current element */
  float xpn32; /* Base 10 exponent of current element: log10(abs(raw32))) */
  float flr_xpn32; /* Floor of base 10 exponent of current element: floor(xpn32) */

  double raw64;
  double temp64;
  unsigned long long int msk_rst64; // fxm: 20211215 should be ull not ul?
  double xpn64;
  int flr_xpn64;
  double err_max64;
  nco_bool c;
  /* End Brute Force declarations */

  double prc_bnr_xct; /* [nbr] Binary digits of precision, exact */
  double mss_val_cmp_dbl; /* Missing value for comparison to double precision values */

  float mss_val_cmp_flt; /* Missing value for comparison to single precision values */

  int bit_xpl_nbr_sgn=int_CEWI; /* [nbr] Number of explicit bits in significand */
  int bit_xpl_nbr_zro; /* [nbr] Number of explicit bits to zero */
  int prc_bnr_ceil; /* [nbr] Exact binary digits of precision rounded-up */
  int prc_bnr_xpl_rqr=int_CEWI; /* [nbr] Explicitly represented binary digits (i.e., keepbits) required to retain */
  int nsb; /* I [nbr] Number of significant bits, i.e., "keepbits" */

  long idx;

  int nco_baa_cnv_typ; /* [enm] Bit adjustment algorithm type */
  
  unsigned int *u32_ptr;
  unsigned int msk_f32_u32_zro=NC_MAX_UINT; /* CEWI */
  unsigned int msk_f32_u32_one=NC_MAX_UINT; /* CEWI */
  unsigned int msk_f32_u32_hshv=NC_MAX_UINT; /* CEWI */
  unsigned long long int *u64_ptr;
  unsigned long long int msk_f64_u64_zro=NC_MAX_UINT64; /* CEWI */
  unsigned long long int msk_f64_u64_one=NC_MAX_UINT64; /* CEWI */
  unsigned long long int msk_f64_u64_hshv=NC_MAX_UINT64; /* CEWI */
  
  /* Only floating point types can be quantized */
  if(type != NC_FLOAT && type != NC_DOUBLE) return;

  nco_baa_cnv_typ=nco_baa_cnv_get(); /* [enm] Bit-adjustment algorithm type */

  /* Step 1: Determine # keepbits for algorithms that use a uniform number 
     For NSD algorithms this involves using log2() math and some fine-tuning
     For NSB algorithms this simply means sanity checking user input */
  switch(nco_baa_cnv_typ){
    /* Methods that do granular pre-processing of NSD or masks */
  case nco_baa_dgr:
  case nco_baa_gbr:
    /* Provide a dummy value of keepbits to satisfy sanity checks
       Granular algorithms determine keepbits/masks on a per-value basis
       They do not actually use this dummy value */
    prc_bnr_xpl_rqr=5;
    break;
    /* Methods that pre-process NSD to obtain keepbits, masks */
  case nco_baa_bgr:
  case nco_baa_shv:
  case nco_baa_set:
  case nco_baa_bgr_btr:
  case nco_baa_brt:
    /* Most NCO lossy routines expect user to provide NSD */
    /* Disallow unreasonable quantization */
    assert(nsd > 0);
    assert(nsd <= 16);
    /* How many bits to preserve? */
    prc_bnr_xct=nsd*bit_per_dgt;
    /* Be conservative, round upwards */
    prc_bnr_ceil=(int)ceil(prc_bnr_xct);
    /* First bit is implicit not explicit but corner cases prevent our taking advantage of this */
    //prc_bnr_xpl_rqr=prc_bnr_ceil-1; /* 20201223 CSZ verified this fails for small integers with NSD=1 */
    //prc_bnr_xpl_rqr=prc_bnr_ceil;
    prc_bnr_xpl_rqr=prc_bnr_ceil+1;

    /* 20220219: Bugfix
       Set bit_xpl_nbr_sgn_dbl=52 not 53 since limits.h/climit DBL_MANT_DIG=53 (which includes implicit bit)
       No longer need to add an additional bit for NC_DOUBLE quantization:
       if(type == NC_DOUBLE) prc_bnr_xpl_rqr++; // Seems necessary for double-precision ppc=array(1.234567,1.0e-6,$dmn) 
    */
    
    /* 20150128: Hand-tuning shows we can sacrifice one or two more bits for almost all cases
       20150205: However, small integers are an exception. In fact they require two more bits, at least for NSD=1.
       Thus minimum threshold to preserve half of least significant digit (LSD) is prc_bnr_xpl_rqr=prc_bnr_ceil
       Decrementing prc_bnr_xpl_rqr by one or two more bits produces maximum errors that exceed half the LSD
       ncra -4 -O -C --ppc default=1 --ppc one=1 -p ~/nco/data in.nc in.nc ~/foo.nc 
       ncks -H -v Q.. --cdl ~/foo.nc | m */
    break;
    /* Methods that expect user to provide NSB not NSD */
  case nco_baa_sh2:
  case nco_baa_btr:
    nsb=nsd;
    /* Disallow unreasonable quantization */
    assert(nsb > 0);
    prc_bnr_xpl_rqr=nsb;
    break;
  default: 
    nco_dfl_case_generic_err();
    break;
  } /* !nco_baa_cnv_typ */

  if(type == NC_FLOAT  && prc_bnr_xpl_rqr >= bit_xpl_nbr_sgn_flt) return;
  if(type == NC_DOUBLE && prc_bnr_xpl_rqr >= bit_xpl_nbr_sgn_dbl) return;

  /* Step 2: Set type-dependent _FillValue, # significand bits, initial unsigned pointer for all algorithms
     Missing value for comparison is _FillValue (if any) otherwise default NC_FILL_FLOAT/DOUBLE */
  if(type == NC_FLOAT){
    if(has_mss_val) mss_val_cmp_flt=*mss_val.fp; else mss_val_cmp_flt=NC_FILL_FLOAT;
    bit_xpl_nbr_sgn=bit_xpl_nbr_sgn_flt;
    u32_ptr=op1.uip;
  }else if(type == NC_DOUBLE){
    if(has_mss_val) mss_val_cmp_dbl=*mss_val.dp; else mss_val_cmp_dbl=NC_FILL_DOUBLE;
    bit_xpl_nbr_sgn=bit_xpl_nbr_sgn_dbl;
    u64_ptr=op1.ui64p;
  } /* !NC_FLOAT */

  /* Diagnostics for bits preserved by all algorithms */
  bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: %s reports pbxr = %d, bxnz = %d\n",nco_prg_nm_get(),fnc_nm,prc_bnr_xpl_rqr,bit_xpl_nbr_zro);

  /* 20150126: fxm casting pointers is tricky with this routine. Avoid for now. */
  /* Typecast pointer to values before access */
  //(void)cast_void_nctype(type,&op1);
  //if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  /* Step 3: Initialize masks for algorithms that make use of uniform masks */
  switch(nco_baa_cnv_typ){
    /* Methods that determine granular (per-value) NSD or masks */
  case nco_baa_dgr:
  case nco_baa_gbr:
    /* These methods create masks within the loop over values */
    break;
    /* Methods that use uniform NSD or NSB and masks */
  case nco_baa_bgr:
  case nco_baa_shv:
  case nco_baa_set:
  case nco_baa_bgr_btr:
  case nco_baa_brt:
  case nco_baa_sh2:
  case nco_baa_btr:
    bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
    assert(bit_xpl_nbr_zro <= bit_xpl_nbr_sgn-NCO_PPC_BIT_XPL_NBR_MIN);
    /* Create masks */
    if(type == NC_FLOAT){
      msk_f32_u32_zro=0u; /* Zero all bits */
      msk_f32_u32_zro=~msk_f32_u32_zro; /* Turn all bits to ones */
      /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
      msk_f32_u32_zro <<= bit_xpl_nbr_zro;
      /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
      msk_f32_u32_one=~msk_f32_u32_zro;
      msk_f32_u32_hshv=msk_f32_u32_one & (msk_f32_u32_zro >> 1); /* Set one bit: the MSB of LSBs */
    }else if(type == NC_DOUBLE){
      msk_f64_u64_zro=0ull; /* Zero all bits */
      msk_f64_u64_zro=~msk_f64_u64_zro; /* Turn all bits to ones */
      /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
      msk_f64_u64_zro <<= bit_xpl_nbr_zro;
      /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
      msk_f64_u64_one=~msk_f64_u64_zro;
      msk_f64_u64_hshv=msk_f64_u64_one & (msk_f64_u64_zro >> 1); /* Set one bit: the MSB of LSBs */
    } /* !NC_FLOAT */
    break;
  default: 
    nco_dfl_case_generic_err();
    break;
  } /* !nco_baa_cnv_typ */

  switch(type){
  case NC_FLOAT:
    switch(nco_baa_cnv_typ){
    case nco_baa_bgr:
      /* Bit-Groom: alternately shave and set LSBs */
      for(idx=0L;idx<sz;idx+=2L)
	if(op1.fp[idx] != mss_val_cmp_flt) u32_ptr[idx]&=msk_f32_u32_zro;
      for(idx=1L;idx<sz;idx+=2L)
	if(op1.fp[idx] != mss_val_cmp_flt && u32_ptr[idx] != 0U) /* Never quantize upwards floating point values of zero */
	  u32_ptr[idx]|=msk_f32_u32_one;
      break;
      /* !BitGroom = BGR */
    case nco_baa_shv:
      /* Bit-Shave: always shave LSBs */
      for(idx=0L;idx<sz;idx++)
	if(op1.fp[idx] != mss_val_cmp_flt) u32_ptr[idx]&=msk_f32_u32_zro;
      break;
    case nco_baa_set:
      /* Bit-Set: always set LSBs */
      for(idx=0L;idx<sz;idx++)
	if(op1.fp[idx] != mss_val_cmp_flt && u32_ptr[idx] != 0U) /* Never quantize upwards floating point values of zero */
	  u32_ptr[idx]|=msk_f32_u32_one;
      break;
    case nco_baa_dgr:
      /* Digit Rounding (DCG19)
	 Test DGR:
	 ccc --tst=bnr --flt_foo=8 2> /dev/null | grep "Binary of float"
	 ncks -O -C -D 1 --baa=3 -v ppc_bgr --ppc default=3 ~/nco/data/in.nc ~/foo.nc
	 ncks -O -C -D 1 --baa=3 -v one_dmn_rec_var_flt --ppc default=3 ~/nco/data/in.nc ~/foo.nc */

      /* 20210927:
         NCO uses a modified (for clarity) implementation of Digit Rounding 
	 from the DCG19 HDF5 codec code which is under an MIT-style license.
	 Citation:
	 Delaunay, X., A. Courtois, and F. Gouillon (2019), 
	 Evaluation of lossless and lossy algorithms for the compression of scientific datasets in netCDF-4 or HDF5 files, 
	 Geosci. Model Dev., 12(9), 4099-4113, doi:10.5194/gmd-2018-250.
	 The original code carries the following copyright notice: */
      /*
       * Copyright (c) 2019, CNES.
       *
       * This source code is licensed under MIT-style license (found in the
       * COPYING file in the root directory of this source tree).
       */
      /* Equivalent symbols between this code and DCG19:
	 DCG19 Code
	 e     xpn_bs2
	 s     val
	 s~    qnt_val
	 m     mnt
	 d     dgt_nbr
	 p     qnt_pwr
	 q     qnt_fct
	 i     tbl_idx
	 nsd   nsd
	 2     FLT_RADIX
         v     mnt_log10_prx
         TABLE mnt_log10_tbl_dgr
	 LOG2_10 bit_per_dgt
	 LOG10_2 dgt_per_bit_dgr */
      for(idx=0L;idx<sz;idx++){
	if((val=op1.fp[idx]) != mss_val_cmp_flt && u32_ptr[idx] != 0U){
	  /* Algorithm flow chart in DCG19 has equation numbers one less than actual
	     Equations indicated below are the actual equation numbers in DCG19 */

	  /* Decompose value into mantissa and exponent
	     Value val = 10^d + eps = sign(val) * 2^xpn_bs2 * mnt, 0.5 <= mnt < 1.0 <--Correct 
	     NB: DCG19 filter code is incorrectly commented with:
	     Value val = 10^d + eps = sign(val) * 2^xpn_bs2 + mnt, 0 <= mnt < 0.5 <--Incorrect
	     double frexp(double x, int *y) returns double mnt=mantissa, exponent y (no FP math) */
	  mnt=frexp(val,&xpn_bs2); /* DGG19 p. 4102 (8) */
	  /* Initialize bin index */
	  tbl_idx=0;
	  /* Search upper bounds to identify appropriate mantissa bin */
	  while(mnt_log10_tbl_dgr[tbl_idx][0] < mnt) tbl_idx++;
	  /* Approximate log10(actual mantissa) as log10(lower bound of mantissa bin) */
	  mnt_log10_prx=mnt_log10_tbl_dgr[tbl_idx][1];
	  /* Convert binary exponent to number of digits dgt_nbr before decimal separator
	     This is # of digits before decimal when writing number longhand, no base-10 exponent
	     dgt_nbr is positive/negative if base-10 exponent is positive/negative
	     Sign of value has no effect on dgt_nbr
	     dgt_nbr must be correct otherwise quantization guarantee check will be incorrect
	     Examples:
	     ncks -O -C -D 1 --baa=4 -v ppc_tst --ppc default=3 ~/nco/data/in.nc ~/foo.nc
	     0.123457    =  0.987654 * 2^-3,  dgt_nbr = 0,   qnt_pwr = -10, qnt_val = 0.123535
	     -0.123457   = -0.987654 * 2^-3,  dgt_nbr = -1,  qnt_pwr = -14, qnt_val = -0.123444
	     64          =  0.5      * 2^7,   dgt_nbr = 2,   qnt_pwr = -4,  qnt_val = 64.0312
	     -64         = -0.5      * 2^7,   dgt_nbr = 2,   qnt_pwr = -4,  qnt_val = -64.0312
	     1.23457e+33 =  0.951077 * 2^110, dgt_nbr = 34,  qnt_pwr = 102, qnt_val = 1.23469e+33
	     1.23457e-17 =  0.8896   * 2^-56, dgt_nbr = -16, qnt_pwr = -64, qnt_val = 1.23328e-17
	     3.14159     =  0.785398 * 2^2,   dgt_nbr = 1,   qnt_pwr = -7,  qnt_val = 3.14453
	     -3.14159    = -0.785398 * 2^2,   dgt_nbr = 1,   qnt_pwr = -7,  qnt_val = -3.14453 */
	  dgt_nbr=(int)floor(xpn_bs2*dgt_per_bit_dgr+mnt_log10_prx)+1; /* DGG19 p. 4102 (9) */
	  /* 20211003: Exact logarithm improves CR by ~10% (EAM BM ~ 116635739 B), slows algorithm ~5% */
	  //dgt_nbr=(int)floor(xpn_bs2*dgt_per_bit-log10(fabs(mnt)))+1; /* DGG19 p. 4102 (8.67) */
	  /* Compute power of quantization mask: qnt_msk = 2^qnt_pwr
	     Spread = dgt_nbr-NSD is how many digits will be quantized by floor() below
	     Smaller NSD yields larger quantization power, masks more bits, reduces precision */
	  qnt_pwr=(int)floor(bit_per_dgt*(dgt_nbr-nsd)); /* DGG19 p. 4101 (7) */
	  /* Compute quantization factor: qnt_fct = 2^qnt_pwr
	     IEEE754 represents qnt_fct exactly using only exponent (no mantissa) bits
	     double ldexp(double x,int y) returns double z=x*2^y without using FP math */
	  qnt_fct=ldexp(1,qnt_pwr); /* DGG19 p. 4101 (5) */
	  /* Quantize:
	     Uniform scalar quantization with reconstruction at bin center
	     Guarantees maximum quantization error = |val-qnt_val| <= qnt_fct/2.0 
	     floor() performs actual quantization
	     double floor(x) returns largest integer value not greater than x
	     Add 0.5 prior to floor to ensure IEEE-rounded value emerges
	     NB: Adding 0.5 produces poorer-than necessary accuracy for some numbers
	     However, it is guaranteed to be bias-free for random distributions */
	  qnt_val=SIGN(val)*(floor(fabs(val)/qnt_fct)+0.5)*qnt_fct; /* DGG19 p. 4101 (1) */
	  /* Implicit conversion casts double to float */
	  op1.fp[idx]=qnt_val;
	  if(nco_dbg_lvl_get() >= nco_dbg_var){
	    /* Verify NSD precision guarantee NB: this check increases time ~30% */
	    qnt_prx=fabs(val-qnt_val);
	    qnt_grr=0.5*pow(10.0,dgt_nbr-nsd);
	    if(nco_dbg_lvl_get() >= nco_dbg_crr || qnt_prx > qnt_grr) (void)fprintf(stdout,"%s: %g = %g * %d^%d, dgt_nbr = %d, qnt_pwr = %d, qnt_val = %g, qnt_prx = %g\n",nco_prg_nm_get(),val,mnt,FLT_RADIX,xpn_bs2,dgt_nbr,qnt_pwr,qnt_val,qnt_prx);
	    assert(qnt_prx <= qnt_grr);
	  } /* !dbg */
	} /* !mss_val_cmp_flt */
      } /* !idx */
      break;
      /* !DigitRound = DGR */
    case nco_baa_gbr:
      /* Granular BitGroom
	 Test GBG:
	 ccc --tst=bnr --flt_foo=8 2> /dev/null | grep "Binary of float"
	 ncks -O -7 -C -D 1 --baa=4 -v ppc_bgr --ppc default=3 ~/nco/data/in.nc ~/foo.nc
	 ncks -O -7 -C -D 1 --baa=4 -v one_dmn_rec_var_flt --ppc default=3 ~/nco/data/in.nc ~/foo.nc */
      for(idx=0L;idx<sz;idx++){
	if((val=op1.fp[idx]) != mss_val_cmp_flt && u32_ptr[idx] != 0U){
	  mnt=frexp(val,&xpn_bs2); /* DGG19 p. 4102 (8) */
	  //tbl_idx=0;
	  //while(mnt_log10_tbl_gbr[tbl_idx][0] < mnt) tbl_idx++;
	  //mnt_log10_prx=mnt_log10_tbl_gbr[tbl_idx][1];
	  //dgt_nbr=(int)floor(xpn_bs2*dgt_per_bit+mnt_log10_prx)+1; /* DGG19 p. 4102 (9) */
#if 1
	  /* Default GBG path */
	  mnt_fabs=fabs(mnt);
	  mnt_log10_fabs=log10(mnt_fabs);
	  /* 20211003 Continuous determination of dgt_nbr improves CR by ~10% */
	  dgt_nbr=(int)floor(xpn_bs2*dgt_per_bit+mnt_log10_fabs)+1; /* DGG19 p. 4102 (8.67) */
	  qnt_pwr=(int)floor(bit_per_dgt*(dgt_nbr-nsd)); /* DGG19 p. 4101 (7) */
	  prc_bnr_xpl_rqr= mnt_fabs == 0.0 ? 0 : abs((int)floor(xpn_bs2-bit_per_dgt*mnt_log10_fabs)-qnt_pwr); /* Protect against mnt = -0.0 */
	  prc_bnr_xpl_rqr--; /* 20211003 Reduce formula result by 1 bit: Passes all tests, improves CR by ~10% */
#else /* !1 */ 
	  /* Development GBG path */
	  int bit_nbr; /* [nbr] Number of bits before decimal point */
	  bit_nbr=(int)floor(xpn_bs2-log2(fabs(mnt))); /* DGG19 p. 4102 (8.67) */
	  qnt_pwr=(int)floor(bit_nbr-bit_per_dgt*nsd); /* DGG19 p. 4101 (7) */
	  prc_bnr_xpl_rqr=abs((int)floor(xpn_bs2-log2(fabs(mnt)))+1-qnt_pwr); /* Protect against mnt = -0.0 */
	  //	  prc_bnr_xpl_rqr= fabs(mnt) == 0.0 ? 0 : abs((int)floor(xpn_bs2-log2(fabs(mnt)))-qnt_pwr); /* Protect against mnt = -0.0 */
	  //prc_bnr_xpl_rqr= fabs(mnt) == 0.0 ? 0 : abs((int)floor(xpn_bs2-log10(fabs(mnt))*bit_per_dgt)-qnt_pwr); /* Protect against mnt = -0.0 */
	  //prc_bnr_xpl_rqr=abs((int)floor(xpn_bs2-log10(fabs(mnt))*bit_per_dgt)+1-qnt_pwr); /* Fails if mnt = -0.0 */
	  //prc_bnr_xpl_rqr=abs((int)floor(xpn_bs2+mnt_log10_prx*bit_per_dgt)+1-qnt_pwr);
#endif /* !1 */ 
	  bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
	  msk_f32_u32_zro=0u; /* Zero all bits */
	  msk_f32_u32_zro=~msk_f32_u32_zro; /* Turn all bits to ones */
	  /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
	  msk_f32_u32_zro <<= bit_xpl_nbr_zro;
	  /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
	  msk_f32_u32_one=~msk_f32_u32_zro;
	  msk_f32_u32_hshv=msk_f32_u32_one & (msk_f32_u32_zro >> 1); /* Set one bit: the MSB of LSBs */
	  u32_ptr[idx]+=msk_f32_u32_hshv; /* Add 1 to the MSB of LSBs, carry 1 to mantissa or even exponent */
	  u32_ptr[idx]&=msk_f32_u32_zro; /* Shave it */
	  qnt_val=op1.fp[idx];
	  if(nco_dbg_lvl_get() >= nco_dbg_var){
	    /* Verify NSD precision guarantee NB: this assert() increases time ~30% */
	    qnt_prx=fabs(val-qnt_val);
	    qnt_grr=0.5*pow(10.0,dgt_nbr-nsd);
	    if(nco_dbg_lvl_get() >= nco_dbg_crr || qnt_prx > qnt_grr) (void)fprintf(stdout,"%s: %g = %g * %d^%d, dgt_nbr = %d, qnt_pwr = %d, pbxr = %d, bxnz = %d, qnt_val = %g, qnt_prx = %g\n",nco_prg_nm_get(),val,mnt,FLT_RADIX,xpn_bs2,dgt_nbr,qnt_pwr,prc_bnr_xpl_rqr,bit_xpl_nbr_zro,qnt_val,qnt_prx);
	    assert(qnt_prx <= qnt_grr);
	    assert(bit_xpl_nbr_zro >= -1); /* NB: 0.0 might require -1 bits, i.e., "setting" the implicit first bit to zero */
	  } /* !dbg */
	} /* !mss_val_cmp_flt */
      } /* !idx */
      break;
      /* !GranularBitRound = GBG */
    case nco_baa_bgr_btr:
    case nco_baa_btr:
      /* BitRound and
	 BitGroom with BitRound (instead of shave/set):
	 Round mantissa, LSBs to zero contributed by Rostislav Kouznetsov 20200711
	 Round mantissa using IEEE floating-point arithmetic, shave LSB using bit-mask */
      for(idx=0L;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_cmp_flt){
	  u32_ptr[idx]+=msk_f32_u32_hshv; /* Add 1 to the MSB of LSBs, carry 1 to mantissa or even exponent */
	  u32_ptr[idx]&=msk_f32_u32_zro; /* Shave it */
	} /* !mss_val_cmp_flt */
      } /* !idx */
      break;
      /* !BitRound = BTR */
      /* !BitGroomRound = RND */
    case nco_baa_sh2:
      /* Bit Half-Shave contributed by Rostislav Kouznetsov 20200715
	 Shave LSBs and set MSB of them
	 See figures at https://github.com/nco/nco/pull/200 */
      for(idx=0L;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_cmp_flt){
	  u32_ptr[idx]&=msk_f32_u32_zro; /* Shave as normal */
	  u32_ptr[idx]|=msk_f32_u32_hshv; /* Set MSB of LSBs */
	} /* !mss_val_cmp_flt */
      } /* !idx */
      break;
      /* !Halfshave */
    case nco_baa_brt: /* JPT 20210102: baa_brt Brute force masking of each individual data point */
      msk_rst32=msk_f32_u32_zro; /* Set mask to original BG mask */
      for(idx=0L;idx<sz;idx+=2L){ // Shave loop
	if(op1.fp[idx] != 0.0f && op1.fp[idx] != 1.0f && op1.fp[idx] != mss_val_cmp_flt){
	  raw32=op1.fp[idx];
	  msk_f32_u32_zro=msk_rst32;
	  op1.uip[idx]&=msk_f32_u32_zro;
	  xpn32=log10f(fabsf(raw32));
	  flr_xpn32=floorf(xpn32);
	  err_max32=0.5f*powf(10.0f,flr_xpn32-nsd+1.0f);
	  flg_qnt_nxt_bit=True;
	  if(op1.fp[idx] != 0.0f && op1.fp[idx] != 1.0f){
	    while(flg_qnt_nxt_bit){
	      tmp32=op1.fp[idx];
	      msk_f32_u32_zro<<=1;
	      op1.uip[idx]&=msk_f32_u32_zro;
	      if((fabsf(raw32)-fabsf(op1.fp[idx])) >= err_max32) flg_qnt_nxt_bit=False;
	      if(op1.fp[idx] == 1.0f || op1.fp[idx] == 0.0f) flg_qnt_nxt_bit=False;
	      fflush(stdout);
	    } // !flg_qnt_nxt_bit
	    op1.fp[idx]=tmp32;
	  } // !0.0 || 1.0
	} // !0.0 || 1.0
      } // !idx
      for(idx=1L;idx<sz;idx+=2L){ // Set loop
	raw32=op1.fp[idx];
	msk_f32_u32_zro=msk_rst32;
	msk_f32_u32_one=~msk_f32_u32_zro; 
	if(op1.fp[idx] != mss_val_cmp_flt && op1.fp[idx] != 0.0f){ /* Never quantize upwards floating point values of zero */
	  op1.uip[idx]|=msk_f32_u32_one;
	  xpn32=log10f(fabsf(raw32));
	  flr_xpn32=floorf(xpn32);
	  err_max32=0.5f*powf(10.0f,flr_xpn32-nsd+1.0f);
	  x=False;
	  flg_qnt_nxt_bit=True;
	  while(flg_qnt_nxt_bit){
	    x=True;
	    tmp32=op1.fp[idx];
	    msk_f32_u32_zro<<=1;
	    msk_f32_u32_one=~msk_f32_u32_zro;
	    op1.uip[idx]|=msk_f32_u32_one;
	    if(fabsf((fabsf(raw32)-fabsf(op1.fp[idx]))) >= err_max32) flg_qnt_nxt_bit=False;
	    if(op1.fp[idx] == 1.0f) flg_qnt_nxt_bit=False;
	    fflush(stdout);
	  } // !flg_qnt_nxt_bit
	  if(x) op1.fp[idx]=tmp32;
	} // !0.0
      } // !idx
      break;
      /* !BruteForce = BRT */
    default:
      abort();
      break;
    } /* !nco_baa_typ_get() */
    break; /* !NC_FLOAT */
  case NC_DOUBLE:
    switch(nco_baa_cnv_typ){
    case nco_baa_bgr:
      /* Bit-Groom: alternately shave and set LSBs */
      for(idx=0L;idx<sz;idx+=2L)
	if(op1.dp[idx] != mss_val_cmp_dbl)
	  u64_ptr[idx]&=msk_f64_u64_zro;
      for(idx=1L;idx<sz;idx+=2L)
	if(op1.dp[idx] != mss_val_cmp_dbl && u64_ptr[idx] != 0ULL) /* Never quantize upwards floating point values of zero */
	  u64_ptr[idx]|=msk_f64_u64_one;
      break;
    case nco_baa_shv:
      /* Bit-Shave: always shave LSBs */
      for(idx=0L;idx<sz;idx++)
	if(op1.dp[idx] != mss_val_cmp_dbl) u64_ptr[idx]&=msk_f64_u64_zro;
      break;
    case nco_baa_set:
      /* Bit-Set: always set LSBs */
      for(idx=0L;idx<sz;idx++)
	if(op1.dp[idx] != mss_val_cmp_dbl && u64_ptr[idx] != 0ULL) /* Never quantize upwards floating point values of zero */
	  u64_ptr[idx]|=msk_f64_u64_one;
      break;
    case nco_baa_dgr:
      /* Digit Rounding (DCG19), copy of NC_FLOAT version except for double precision changes:
	 1. Change fp to dp
	 2. Reference mss_val_cmp_dbl not mss_val_cmp_flt
	 3. Compare u64_ptr (not u32_ptr) to zero
	 DO NOT EDIT the NC_DOUBLE code except for dp-specific changes, use fp code as template */
      for(idx=0L;idx<sz;idx++){
	if((val=op1.dp[idx]) != mss_val_cmp_dbl && u64_ptr[idx] != 0ULL){
	  /* Compute number of digits before decimal point of input floating-point value val
	     Value val = 10^d + eps = sign(val) * 2^xpn_bs2 * mnt, 0.5 <= mnt < 1.0 <--Correct 
	     Note that DCG19 filter code is incorrectly commented with:
	     Value val = 10^d + eps = sign(val) * 2^xpn_bs2 + mnt, 0 <= mnt < 0.5 <--Incorrect
	     Note that algorithm flow chart in DCG19 has equation numbers one less than actual
	     Equations indicated below are the actual equations in DCG19 */
	  mnt=frexp(val,&xpn_bs2); /* DGG19 p. 4102 (8) */
	  tbl_idx=0;
	  while(mnt_log10_tbl_dgr[tbl_idx][0] < mnt) tbl_idx++;
	  mnt_log10_prx=mnt_log10_tbl_dgr[tbl_idx][1];
	  dgt_nbr=(int)floor(xpn_bs2*dgt_per_bit_dgr+mnt_log10_prx)+1; /* DGG19 p. 4102 (9) */
	  qnt_pwr=(int)floor(bit_per_dgt*(dgt_nbr-nsd)); /* DGG19 p. 4101 (7) */
	  qnt_fct=ldexp(1.0,qnt_pwr); /* DGG19 p. 4101 (5) */
	  qnt_val=SIGN(val)*(floor(fabs(val)/qnt_fct)+0.5)*qnt_fct; /* DGG19 p. 4101 (1) */
	  op1.dp[idx]=qnt_val;
	  if(nco_dbg_lvl_get() >= nco_dbg_var){
	    /* Verify NSD precision guarantee NB: this check increases time ~30% */
	    qnt_prx=fabs(val-qnt_val);
	    qnt_grr=0.5*pow(10.0,dgt_nbr-nsd);
	    if(nco_dbg_lvl_get() >= nco_dbg_crr || qnt_prx > qnt_grr) (void)fprintf(stdout,"%s: %g = %g * %d^%d, dgt_nbr = %d, qnt_pwr = %d, qnt_val = %g, qnt_prx = %g\n",nco_prg_nm_get(),val,mnt,FLT_RADIX,xpn_bs2,dgt_nbr,qnt_pwr,qnt_val,qnt_prx);
	    assert(qnt_prx <= qnt_grr);
	  } /* !dbg */
	} /* !mss_val_cmp_dbl */
      } /* !idx */
      break;
      /* !DigitRound = DGR */
    case nco_baa_gbr:
      for(idx=0L;idx<sz;idx++){
	if((val=op1.dp[idx]) != mss_val_cmp_dbl && u64_ptr[idx] != 0ULL){
	  mnt=frexp(val,&xpn_bs2); /* DGG19 p. 4102 (8) */
	  mnt_fabs=fabs(mnt);
	  mnt_log10_fabs=log10(mnt_fabs);
	  /* 20211003 Continuous determination of dgt_nbr improves CR by ~10% */
	  dgt_nbr=(int)floor(xpn_bs2*dgt_per_bit+mnt_log10_fabs)+1; /* DGG19 p. 4102 (8.67) */
	  qnt_pwr=(int)floor(bit_per_dgt*(dgt_nbr-nsd)); /* DGG19 p. 4101 (7) */
	  prc_bnr_xpl_rqr= mnt_fabs == 0.0 ? 0 : abs((int)floor(xpn_bs2-bit_per_dgt*mnt_log10_fabs)-qnt_pwr); /* Protect against mnt = -0.0 */
	  prc_bnr_xpl_rqr--; /* 20211003 Reduce formula result by 1 bit: Passes all tests, improves CR by ~10% */
	  bit_xpl_nbr_zro=bit_xpl_nbr_sgn-prc_bnr_xpl_rqr;
	  msk_f64_u64_zro=0ull; /* Zero all bits */
	  msk_f64_u64_zro=~msk_f64_u64_zro; /* Turn all bits to ones */
	  /* Bit Shave mask for AND: Left shift zeros into bits to be rounded, leave ones in untouched bits */
	  msk_f64_u64_zro <<= bit_xpl_nbr_zro;
	  /* Bit Set   mask for OR:  Put ones into bits to be set, zeros in untouched bits */
	  msk_f64_u64_one=~msk_f64_u64_zro;
	  msk_f64_u64_hshv=msk_f64_u64_one & (msk_f64_u64_zro >> 1); /* Set one bit: the MSB of LSBs */
	  u64_ptr[idx]+=msk_f64_u64_hshv; /* Add 1 to the MSB of LSBs, carry 1 to mantissa or even exponent */
	  u64_ptr[idx]&=msk_f64_u64_zro; /* Shave it */
	  qnt_val=op1.dp[idx];
	  if(nco_dbg_lvl_get() >= nco_dbg_var){
	    /* Verify NSD precision guarantee NB: this assert() increases time ~30% */
	    qnt_prx=fabs(val-qnt_val);
	    qnt_grr=0.5*pow(10.0,dgt_nbr-nsd);
	    if(nco_dbg_lvl_get() >= nco_dbg_crr || qnt_prx > qnt_grr) (void)fprintf(stdout,"%s: %g = %g * %d^%d, dgt_nbr = %d, qnt_pwr = %d, pbxr = %d, bxnz = %d, qnt_val = %g, qnt_prx = %g\n",nco_prg_nm_get(),val,mnt,FLT_RADIX,xpn_bs2,dgt_nbr,qnt_pwr,prc_bnr_xpl_rqr,bit_xpl_nbr_zro,qnt_val,qnt_prx);
	    assert(qnt_prx <= qnt_grr);
	    assert(bit_xpl_nbr_zro >= -1); /* NB: 0.0 might require -1 bits, i.e., "setting" the implicit first bit to zero */
	  } /* !dbg */
	} /* !mss_val_cmp_dbl */
      } /* !idx */
      break;
      /* !GranularBitRound = GBG */
    case nco_baa_bgr_btr:
    case nco_baa_btr:
      /* Round mantissa, LSBs to zero contributed by Rostislav Kouznetsov 20200711
	 Round mantissa using software emulation of IEEE arithmetic, shave LSB using bit-mask
	 See figures at https://github.com/nco/nco/pull/199 */
      for(idx=0L;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_cmp_dbl){
	  u64_ptr[idx]+=msk_f64_u64_hshv; /* Add 1 at MSB of LSBs */
	  u64_ptr[idx]&=msk_f64_u64_zro; /* Shave it */
	} /* !mss_val_cmp_dbl */
      } /* !idx */
      break;
      /* !BitRound = BTR */
      /* !BitGroomRound = RND */
    case nco_baa_sh2:
      /* Bit Half-Shave contributed by Rostislav Kouznetsov 20200715
	 Shave LSBs and set MSB of them
	 See figures at https://github.com/nco/nco/pull/200 */
      for(idx=0L;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_cmp_dbl){
	  u64_ptr[idx]&=msk_f64_u64_zro; /* Shave as normal */
	  u64_ptr[idx]|=msk_f64_u64_hshv; /* Set MSB of LSBs */
	} /* !mss_val_cmp_dbl */
      } /* !idx */
      break;
      /* !Halfshave */
    case nco_baa_brt: /* JPT 20210102: baa_brt Brute force masking of each individual data point */ 
      msk_rst64=msk_f64_u64_zro;
      for(idx=0L;idx<sz;idx+=2L){ // shave loop
	if(op1.dp[idx] != 0.0 && op1.dp[idx] != 1.0 && op1.dp[idx] != mss_val_cmp_dbl){
	  raw64 = op1.dp[idx];
	  msk_f64_u64_zro=msk_rst64;
	  u64_ptr[idx]&=msk_f64_u64_zro;
	  xpn64=log10(fabs(raw64));
	  flr_xpn64=floor(xpn64);
	  err_max64=0.5*pow(10.0,flr_xpn64-nsd+1);
	  c=True;
	  if(op1.dp[idx] != 0.0 && op1.dp[idx] != 1.0){
	    while(c){
	      temp64=op1.dp[idx];
	      msk_f64_u64_zro<<=1;
	      u64_ptr[idx]&=msk_f64_u64_zro;
	      if((fabs(raw64)-fabs(op1.dp[idx])) >= err_max64) c=False;
	      if(op1.dp[idx] == 1.0) c=False;
	      if(op1.dp[idx] == 0.0) c=False;
	      if(op1.dp[idx] != op1.dp[idx]) c=False;
	      fflush(stdout);
	    } // close while loop
	    op1.dp[idx] = temp64;
	  } // close if before while
	} // close if != 0
      } // close shave loop
      
      for(idx=1L;idx<sz;idx+=2L){ // Set loop
	raw64 = op1.dp[idx];
	msk_f64_u64_zro=msk_rst64;
	msk_f64_u64_one=~msk_f64_u64_zro;
	if(op1.dp[idx] != 0ULL && op1.dp[idx] != mss_val_cmp_dbl){ /* Never quantize upwards floating point values of zero */
	  u64_ptr[idx]|=msk_f64_u64_one;
	  xpn64=log10(fabs(raw64));
	  flr_xpn64=floor(xpn64);
	  err_max64=0.5*pow(10.0,flr_xpn64-nsd+1);
	  x=False;
	  c=True;
	  while(c){
	    x=True;
	    temp64 = op1.dp[idx];
	    msk_f64_u64_zro <<= 1;
	    msk_f64_u64_one =~ msk_f64_u64_zro;
	    u64_ptr[idx]|= msk_f64_u64_one;
	    if(fabs((fabs(raw64)-fabs(op1.dp[idx]))) >= err_max64) c=False;
	    if(op1.dp[idx] == 1) c=False;
	    if(op1.dp[idx] != op1.dp[idx]) c=False;
	    fflush(stdout);
	  } // close while
	  if(x) op1.dp[idx]=temp64;
	} // close if 0
      } // close set loop
      break; // closes baa_brt
    default:
      abort();
      break;
    } /* !nco_baa_typ_get() */
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
  } /* !type */
  
  /* 20150126: fxm casting pointers is tricky with this routine. Avoid for now. */
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* !nco_ppc_bitmask() */

double /* [frc] Rounded value */
nco_ppc_bitmask_scl /* [fnc] Round input value significand by specified number of bits */
(const double val_xct, /* I [frc] Exact value to round */
 const int bit_xpl_nbr_zro) /* I [nbr] Bits to round */
{
  /* Purpose: Mask-out bit_xpl_nbr_zro least most significant bits of a scalar double precision value
     Code originally from nco_ppc_bitmask() (bitmasking is my signature move)
     Code used in nco_rgr_wgt() when diagnosing whether quadrature weights properly normalized */

  const int bit_xpl_nbr_sgn_dbl=52; /* [nbr] Bits 0-51 of DP significands are explicit. Bit 52 is implicit. */
  double val_rnd; /* [frc] Rounded version of exact value */
  unsigned long int *u64_ptr;
  unsigned long int msk_f64_u64_zro;
  assert(bit_xpl_nbr_zro >= 0 && bit_xpl_nbr_zro <= bit_xpl_nbr_sgn_dbl-NCO_PPC_BIT_XPL_NBR_MIN);
  CEWI_unused(bit_xpl_nbr_sgn_dbl);
  
  val_rnd=val_xct;
  msk_f64_u64_zro=0ull; /* Zero all bits */
  msk_f64_u64_zro=~msk_f64_u64_zro; /* Turn all bits to ones */
  msk_f64_u64_zro <<= bit_xpl_nbr_zro;
  u64_ptr=(unsigned long int *)&val_rnd;
  *u64_ptr&=msk_f64_u64_zro;

  return val_rnd;
} /* !nco_ppc_bitmask_scl() */
