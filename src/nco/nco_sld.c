/* $Header$ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   ncks -4 -O -C -v ppc_dbl --ppc /ppc_dbl=3 ~/nco/data/in.nc ~/foo.nc */

#include "nco_sld.h" /* Swath-Like Data */

int /* O [enm] Return code */
nco_rgr_wgt /* [fnc] Regrid using external weights */
(rgr_sct * const rgr_nfo) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields using external weights

     Examine template SCRIP remap file:
     ncks --cdl -m ${DATA}/scrip/rmp_T42_to_POP43_conserv.nc | m

     Conventions:
     grid_size: Number of grid cells (product of lat*lon)
     address: Source and destination index for each link pair
     num_links: Number of unique address pairs in remapping, i.e., size of sparse matrix
     num_wgts: Number of weights for given remapping
     = 1 for bilinear- and distance-based, 
     = 3 for second-order conservative
     = 4 for bicubic (gradients in each direction plus cross-gradient term)
     
     wgt: 
     Maximum number of source cells contributing to destination cell is not a dimension
     in SCRIP remapping files because SCRIP stores everying in 1-D sparse matrix arrays
     Sparse matrix formulations:

     for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
       // Normalization: fractional area
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0];
       // Normalization: destination area
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0]/dst_area[ddr_dst[lnk_idx]];
       // Normalization: none
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0]/(dst_area[ddr_dst[lnk_idx]]*dst_frc[ddr_dst[lnk_idx]);
  */

  const char fnc_nm[]="nco_rgr_wgt()"; /* [sng] Function name */
  const char wgt_nm[]="remap_matrix"; /* [sng] Name of weighting variable */

  int rcd=NC_NOERR;

  size_t lnk_nbr; /* [nbr] Number of links */
  size_t lnk_idx; 
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO Obtaining mapping weights from %s...\n",nco_prg_nm_get(),rgr_nfo->fl_map);

  var_sct *wgt=NULL;
  var_sct *wgt_out=NULL;
  
  return rcd;
} /* nco_rgr_wgt() */

kvmap_sct nco_sng2map /* [fnc] Parse string into key-value pair */
(char *sng, /* I [sng] String to parse, including "=" */
 kvmap_sct kvm) /* O [sct] Key-value pair */
{
  /* Purpose: Convert string separated by single delimiter into two strings
     Routine converts argument "--ppc key1,key2,...,keyN=val" into kvm.key="key1,key2,...keyN" and kvm.val=val
     e.g., routine converts argument "--ppc one,two=3" into kvm.key="one,two" and kvm.val=3 */
  char *tkn_sng;
  const char dlm[]="="; /* [sng] Delimiter */
  
  int arg_idx=0; /* [nbr] */

  /* NB: Replace strtok() by strsep()? strtok() does not handle consecutive delimiters well */
  tkn_sng=strtok(sng,dlm);
  while(tkn_sng){
    arg_idx++;
    /* fxm: Whitespace-stripping may be unnecessary */
    nco_sng_strip(tkn_sng);
    switch(arg_idx){
    case 1:
      kvm.key=strdup(tkn_sng);
      break;
    case 2:
      kvm.val=strdup(tkn_sng);
      break;
    default:
      (void)fprintf(stderr,"nco_sng2map() cannot get key-value pair from input: %s\n",sng);
      break;
    }/* end switch */
    tkn_sng=strtok(NULL,dlm);
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
    /* nco_sng2map() converts argument "--ppc one,two=3" into kvm.key="one,two" and kvm.val=3
       Then nco_lst_prs_2D() converts kvm.key into two items, "one" and "two", with the same value, 3 */
    if(kvm.key){
      int var_idx; /* [idx] Index over variables in current PPC argument */
      int var_nbr; /* [nbr] Number of variables in current PPC argument */
      char **var_lst;
      var_lst=nco_lst_prs_2D(kvm.key,",",&var_nbr);
      for(var_idx=0;var_idx<var_nbr;var_idx++){ /* Expand multi-variable specification */
        ppc_lst[ppc_var_nbr].key=strdup(var_lst[var_idx]);
        ppc_lst[ppc_var_nbr].val=strdup(kvm.val);
        ppc_var_nbr++;
      } /* end for */
      var_lst=nco_sng_lst_free(var_lst,var_nbr);
    } /* end if */
  } /* end for */

  /* PPC default exists, set all non-coordinate variables to default first */
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

char * /* O [sng] Stripped-string */
nco_sng_strip /* [fnc] Strip leading and trailing white space */
(char *sng) /* I/O [sng] String to strip */
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
    kvmaps[idx].val=(char *)nco_free(kvmaps[idx].val);
    idx++;
  } /* end while */
  kvmaps=(kvmap_sct *)nco_free(kvmaps);
} /* end nco_kvmaps_free */

void nco_kvmap_prn(kvmap_sct kvm)
{
  if(kvm.key) (void)fprintf(stdout,"%s = %s\n",kvm.key,kvm.val); else return;
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
 char **rgr_arg, /* [sng] Regridding arguments */
 const int rgr_nbr, /* [nbr] Number of regridding arguments */
 char * const rgr_in, /* I [sng] File containing fields to be regridded */
 char * const rgr_out, /* I [sng] File containing regridded fields */
 char * const rgr_grd_src, /* I [sng] File containing input grid */
 char * const rgr_grd_dst, /* I [sng] File containing destination grid */
 char * const rgr_map, /* I [sng] File containing mapping weights from source to destination grid */
 char * const rgr_var, /* I [sng] Variable for special regridding treatment */
 rgr_sct * const rgr_nfo) /* O [sct] Regridding structure */
{
  /* Purpose: Initialize regridding structure */
     
  /* Sample calls:
     Debugging:
     ncks -O -D 6 --rgr=Y ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc

     T62->T42 from scratch, minimal arguments:
     ncks -O -D 6 --rgr=Y ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T62->T42 from scratch, more arguments:
     ncks -O -D 6 --rgr=Y --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T62->T42 from scratch, explicit arguments:
     ncks -O --rgr=Y --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T42->T42 from scratch:
     ncks -O --rgr=Y --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     T42->POP43 from existing weights:
     ncks -O --rgr=Y --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_POP43.nc --rgr_map=${DATA}/scrip/rmp_T42_to_POP43_conserv.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc */

  const char fnc_nm[]="nco_rgr_ini()";
  
  int rcd=NC_NOERR;
  
  /* Initialize */
  rgr_nfo->flg_usr_rqs=False; /* [flg] User requested regridding */
  rgr_nfo->out_id=int_CEWI; /* [id] Output netCDF file ID */

  rgr_nfo->in_id=in_id; /* [id] Input netCDF file ID */
  rgr_nfo->rgr_arg=rgr_arg; /* [sng] Regridding arguments */
  rgr_nfo->rgr_nbr=rgr_nbr; /* [nbr] Number of regridding arguments */

  rgr_nfo->fl_grd_src=rgr_grd_src; /* [sng] File containing input grid */
  rgr_nfo->fl_grd_dst=rgr_grd_dst; /* [sng] File containing destination grid */
  rgr_nfo->fl_in=rgr_in; /* [sng] File containing fields to be regridded */
  rgr_nfo->fl_out=rgr_out; /* [sng] File containing regridded fields */
  rgr_nfo->fl_out_tmp=NULL_CEWI; /* [sng] Temporary file containing regridded fields */
  rgr_nfo->fl_map=rgr_map; /* [sng] File containing mapping weights from source to destination grid */
  rgr_nfo->var_nm=rgr_var; /* [sng] Variable for special regridding treatment */
  
  /* Did user explicitly request regridding? */
  if(rgr_nbr > 0 || rgr_grd_src != NULL || rgr_grd_dst != NULL || rgr_out != NULL || rgr_map != NULL) rgr_nfo->flg_usr_rqs=True;

  /* Initialize arguments after copying */
  if(!rgr_nfo->fl_out) rgr_nfo->fl_out=(char *)strdup("/data/zender/rgr/rgr_out.nc");
  if(!rgr_nfo->fl_grd_dst) rgr_nfo->fl_grd_dst=(char *)strdup("/data/zender/scrip/grids/remap_grid_T42.nc");
  if(!rgr_nfo->var_nm) rgr_nfo->var_nm=(char *)strdup("ORO");
  
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
  if(rgr_nfo->var_nm) rgr_nfo->var_nm=(char *)nco_free(rgr_nfo->var_nm);

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
  default: nco_dfl_case_generic_err(); break;
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
  default: nco_dfl_case_generic_err(); break;
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
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_fl_fmt_sng() */

const char * /* O [sng] String version of ESMC_StaggerLoc enum */
nco_esmf_stg_lcn_sng /* [fnc] Convert ESMF stagger location enum to string */
(const int nco_esmf_stg_lcn) /* I [enm] ESMF stagger location enum ESMC_StaggerLoc */
{
  /* Purpose: Convert ESMF stagger location enum ESMC_StaggerLoc to string */
  switch(nco_esmf_stg_lcn){
  case ESMC_STAGGERLOC_INVALID: return "ESMC_STAGGERLOC_INVALID";
  case ESMC_STAGGERLOC_UNINIT: return "ESMC_STAGGERLOC_UNINIT";
  case ESMC_STAGGERLOC_CENTER: return "ESMC_STAGGERLOC_CENTER";
  case ESMC_STAGGERLOC_EDGE1: return "ESMC_STAGGERLOC_EDGE1";
  case ESMC_STAGGERLOC_EDGE2: return "ESMC_STAGGERLOC_EDGE2";
  case ESMC_STAGGERLOC_CORNER: return "ESMC_STAGGERLOC_CORNER";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_stg_lcn_sng() */

const char * /* O [sng] String version of ESMC_GridItem_Flag enum */
nco_esmf_grd_itm_sng /* [fnc] Convert ESMF grid item enum to string */
(const int nco_esmf_grd_itm) /* I [enm] ESMF grid item enum ESMC_GridItem_Flag */
{
  /* Purpose: Convert ESMF grid item enum ESMC_GridItem_Flag to string */
  switch(nco_esmf_grd_itm){
  case ESMC_GRIDITEM_INVALID: return "ESMC_GRIDITEM_INVALID";
  case ESMC_GRIDITEM_UNINIT: return "ESMC_GRIDITEM_UNINIT";
  case ESMC_GRIDITEM_MASK: return "ESMC_GRIDITEM_MASK";
  case ESMC_GRIDITEM_AREA: return "ESMC_GRIDITEM_AREA";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_grd_itm_sng() */

const char * /* O [sng] String version of ESMC_CoordSys_Flag enum */
nco_esmf_crd_sys_sng /* [fnc] Convert ESMF coordinate system enum to string */
(const int nco_esmf_crd_sys) /* I [enm] ESMF coordinate system enum ESMC_CoordSys_Flag */
{
  /* Purpose: Convert ESMF coordinate system enum ESMC_CoordSys_Flag to string */
  switch(nco_esmf_crd_sys){
  case ESMC_COORDSYS_INVALID: return "ESMC_COORDSYS_INVALID";
  case ESMC_COORDSYS_UNINIT: return "ESMC_COORDSYS_UNINIT";
  case ESMC_COORDSYS_CART: return "ESMC_COORDSYS_CART";
  case ESMC_COORDSYS_SPH_DEG: return "ESMC_COORDSYS_SPH_DEG";
  case ESMC_COORDSYS_SPH_RAD: return "ESMC_COORDSYS_SPH_RAD";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_crd_sys_sng() */

const char * /* O [sng] String version of ESMC_TypeKind_Flag enum */
nco_esmf_typ_knd_sng /* [fnc] Convert ESMF type kind enum to string */
(const int nco_esmf_typ_knd) /* I [enm] ESMF type kind enum ESMC_TypeKind_Flag */
{
  /* Purpose: Convert ESMF type kind enum ESMC_TypeKind_Flag to string */
  switch(nco_esmf_typ_knd){
  case ESMC_TYPEKIND_I1: return "ESMC_TYPEKIND_I1";
  case ESMC_TYPEKIND_I2: return "ESMC_TYPEKIND_I2";
  case ESMC_TYPEKIND_I4: return "ESMC_TYPEKIND_I4";
  case ESMC_TYPEKIND_I8: return "ESMC_TYPEKIND_I8";
  case ESMC_TYPEKIND_R4: return "ESMC_TYPEKIND_R4";
  case ESMC_TYPEKIND_R8: return "ESMC_TYPEKIND_R8";
  case ESMC_TYPEKIND_LOGICAL: return "ESMC_TYPEKIND_LOGICAL";
  case ESMC_TYPEKIND_CHARACTER: return "ESMC_TYPEKIND_CHARACTER";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_typ_knd_sng() */

const char * /* O [sng] String version of ESMC_LogMsgType_Flag enum */
nco_esmf_log_msg_sng /* [fnc] Convert ESMF log message type enum to string */
(const int nco_esmf_log_msg) /* I [enm] ESMF log message type enum ESMC_LogMsgType_Flag */
{
  /* Purpose: Convert ESMF coordinate system enum ESMC_LogMsgType_Flag to string */
  switch(nco_esmf_log_msg){
  case ESMC_LOGMSG_INFO: return "ESMC_LOGMSG_INFO";
  case ESMC_LOGMSG_WARN: return "ESMC_LOGMSG_WARN";
  case ESMC_LOGMSG_ERROR: return "ESMC_LOGMSG_ERROR";
  case ESMC_LOGMSG_TRACE: return "ESMC_LOGMSG_TRACE";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_log_msg_sng() */

const char * /* O [sng] String version of ESMC_RegridMethod_Flag enum */
nco_esmf_rgr_mth_sng /* [fnc] Convert ESMF regrid method type enum to string */
(const int nco_esmf_rgr_mth) /* I [enm] ESMF regrid method type enum ESMC_RegridMethod_Flag */
{
  /* Purpose: Convert ESMF regrid method enum ESMC_RegridMethod_Flag to string */
  switch(nco_esmf_rgr_mth){
  case ESMC_REGRIDMETHOD_BILINEAR: return "ESMC_REGRIDMETHOD_BILINEAR";
  case ESMC_REGRIDMETHOD_PATCH: return "ESMC_REGRIDMETHOD_PATCH";
  case ESMC_REGRIDMETHOD_CONSERVE: return "ESMC_REGRIDMETHOD_CONSERVE";
  case ESMC_REGRIDMETHOD_NEAREST_STOD: return "ESMC_REGRIDMETHOD_NEAREST_STOD";
  case ESMC_REGRIDMETHOD_NEAREST_DTOS: return "ESMC_REGRIDMETHOD_NEAREST_DTOS";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_rgr_mth_sng() */

const char * /* O [sng] String version of ESMC_PoleMethod_Flag enum */
nco_esmf_pll_mth_sng /* [fnc] Convert ESMF pole method type enum to string */
(const int nco_esmf_pll_mth) /* I [enm] ESMF pole method type enum ESMC_PoleMethod_Flag */
{
  /* Purpose: Convert ESMF pole method enum ESMC_PoleMethod_Flag to string */
  switch(nco_esmf_pll_mth){
  case ESMC_POLEMETHOD_NONE: return "ESMC_POLEMETHOD_NONE";
  case ESMC_POLEMETHOD_ALLAVG: return "ESMC_POLEMETHOD_ALLAVG";
  case ESMC_POLEMETHOD_NPNTAVG: return "ESMC_POLEMETHOD_NPNTAVG";
  case ESMC_POLEMETHOD_TEETH: return "ESMC_POLEMETHOD_TEETH";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_pll_mth_sng() */

const char * /* O [sng] String version of ESMC_UnmappedAction_Flag enum */
nco_esmf_unm_act_sng /* [fnc] Convert ESMF unmapped action type enum to string */
(const int nco_esmf_unm_act) /* I [enm] ESMF unmapped action type enum ESMC_UnmappedAction_Flag */
{
  /* Purpose: Convert ESMF unmapped action enum ESMC_UnmappedAction_Flag to string */
  switch(nco_esmf_unm_act){
  case ESMC_UNMAPPEDACTION_ERROR: return "ESMC_UNMAPPEDACTION_ERROR";
  case ESMC_UNMAPPEDACTION_IGNORE: return "ESMC_UNMAPPEDACTION_IGNORE";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_unm_act_sng() */

const char * /* O [sng] String version of ESMC_Region_Flag enum */
nco_esmf_rgn_flg_sng /* [fnc] Convert ESMF region flag enum to string */
(const int nco_esmf_rgn_flg) /* I [enm] ESMF region flag enum ESMC_Region_Flag */
{
  /* Purpose: Convert ESMF region flag enum ESMC_Region_Flag to string */
  switch(nco_esmf_rgn_flg){
  case ESMC_REGION_TOTAL: return "ESMC_REGION_TOTAL";
  case ESMC_REGION_SELECT: return "ESMC_REGION_SELECT";
  case ESMC_REGION_EMPTY: return "ESMC_REGION_EMPTY";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_rgn_flg_sng() */

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
  
  enum ESMC_StaggerLoc stg_lcn_src=ESMC_STAGGERLOC_CENTER; /* [enm] Stagger location of source grid */
  enum ESMC_StaggerLoc stg_lcn_dst=ESMC_STAGGERLOC_CENTER; /* [enm] Stagger location of destination grid */

  int flg_openMPEnabledFlag; /* [flg] ESMF library was compiled with OpenMP enabled */
  int flg_pthreadsEnabledFlag; /* [flg] ESMF library was compiled with Pthreads enabled */
  int localPet; /* [id] ID of PET that issued call */
  int peCount; /* [nbr] Number of PEs referenced by ESMF_VM */
  int petCount; /* [nbr] Number of PETs referenced by ESMF_VM */
  int rcd_esmf; /* [enm] Return codes from ESMF functions */
  
  int dmn_nbr_grd=2;
  int rcd=ESMF_SUCCESS;
  
  int *dmn_id;
  
  int dmn_nbr;
  int idx;
  int var_in_id;
  
  long *dmn_cnt;
  long *dmn_srt;
  
  nc_type var_typ_in;
  nc_type var_typ_out=NC_DOUBLE;
  
  double *lon_in;
  double *lat_in;
  void *void_ptr_var;
  
  int in_id; /* I [id] Input netCDF file ID */
  int out_id; /* I [id] Output netCDF file ID */

  /* SCRIP rank-2 grids are almost always lat,lon (C) and lon,lat (Fortran)
     Indexing is confusing because ESMF always uses Fortran-ordering convention with mixture of 0- and 1-based indices
     netCDF always uses C-ordering convention with 0-based indices
     No data transposition is necessary because variable RAM is always in C-order (same as netCDF)
     However, Fortran and C (i.e., ESMF and netCDF) access that same RAM using different conventions
     2-D data are always C[lat,lon] or F(lon,lat) in practice
     2-D stored in netCDF or RAM as C[lon,lat] and F(lat,lon) are possible, though not seen in practice
     NCO regridding below assumes data are in C[lat,lon] order
     NCO hardcoded subscript conventions follow this pattern:
     crd_idx_[axs]_[ibs]_[dmn]_[grd] where
     axs = access convention = C or Fortran = _c_ or _f_ 
     ibs = index base = 0 or 1 = _0bs_ or _1bs_ for zero-based or one-based indices
     NB: axs is independent from ibs! 
     dmn = dimension = lon or lat
     grd = grid = source or destination */
  const int crd_idx_c_0bs_lat_dst=0; /* [dgr] 0-based index of latitude  in C       representation of rank-2 destination grids */
  const int crd_idx_c_0bs_lon_dst=1; /* [dgr] 0-based index of longitude in C       representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lat_dst=1; /* [dgr] 0-based index of latitude  in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lat_src=1; /* [dgr] 0-based index of latitude  in Fortran representation of rank-2 source grids */
  const int crd_idx_f_0bs_lon_dst=0; /* [dgr] 0-based index of longitude in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lon_src=0; /* [dgr] 0-based index of longitude in Fortran representation of rank-2 source grids */
  const int crd_idx_f_1bs_lat_dst=2; /* [dgr] 1-based index of latitude  in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_1bs_lat_src=2; /* [dgr] 1-based index of latitude  in Fortran representation of rank-2 source grids */
  const int crd_idx_f_1bs_lon_dst=1; /* [dgr] 1-based index of longitude in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_1bs_lon_src=1; /* [dgr] 1-based index of longitude in Fortran representation of rank-2 source grids */

  /* Initialize */
  in_id=rgr_nfo->in_id;
  out_id=rgr_nfo->out_id;
  
  /* Obtain input longitude type and length */
  char lon_nm_in[]="lon";
  (void)nco_inq_varid(in_id,lon_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr_nfo->fl_in);
  } /* !var_typ_in */

  /* Allocate space to hold dimension metadata */
  dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
  dmn_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  /* Get input dimension sizes */
  long dmn_sz=1L; /* [nbr] Number of elements in dimension (will be self-multiplied) */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    dmn_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  
  int *max_idx; /* [nbr] Zero-based array containing dimension sizes in order? */
  max_idx=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  max_idx[crd_idx_f_0bs_lon_src]=dmn_sz; /* [nbr] Number of elements in dimensions */
  
  /* Allocate space for and obtain longitude */
  lon_in=(double *)nco_malloc(dmn_sz*nco_typ_lng(var_typ_out));
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,lon_in,var_typ_out);

  /* Obtain input latitude type and length */
  char lat_nm_in[]="lat";
  (void)nco_inq_varid(in_id,lat_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr_nfo->fl_in);
  } /* !var_typ_in */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  dmn_sz=1L;
  /* Get dimension sizes from input file */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    dmn_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  max_idx[crd_idx_f_0bs_lat_src]=dmn_sz; /* [nbr] Number of elements in dimensions */

  /* Allocate space for and obtain latitude */
  lat_in=(double *)nco_malloc(dmn_sz*nco_typ_lng(var_typ_out));
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,lat_in,var_typ_out);

  /* Initialize before any other ESMC API calls!
     ESMC_ArgLast is ALWAYS at the end to indicate the end of opt args */
  ESMC_Initialize(&rcd_esmf,
		  ESMC_InitArgDefaultCalKind(ESMC_CALKIND_GREGORIAN),
		  ESMC_InitArgLogFilename(fl_nm_esmf_log),
		  ESMC_InitArgLogKindFlag(ESMC_LOGKIND_MULTI),
		  ESMC_ArgLast);
  enum ESMC_LogMsgType_Flag log_msg_typ=ESMC_LOGMSG_TRACE;
  if(rcd_esmf != ESMF_SUCCESS){
    ESMC_LogWrite("ESMC_Initialize() failed",log_msg_typ);
    goto rgr_cln;
  } /* endif */
  
  /* Set log to flush after every message */
  rcd_esmf=ESMC_LogSet(ESMF_TRUE);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Obtain VM */
  ESMC_VM vm;
  vm=ESMC_VMGetGlobal(&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Set-up local PET info */
  /* Source: ${DATA}/esmf/src/Infrastructure/VM/interface/ESMC_VM.C */
  rcd_esmf=ESMC_VMGet(vm,&localPet,&petCount,&peCount,(MPI_Comm *)NULL,&flg_pthreadsEnabledFlag,&flg_openMPEnabledFlag);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Create destination grid from SCRIP file set as rgr_grd_dst */
  int flg_isSphere=1; /* [flg] Set to 1 for a spherical grid, or 0 for regional. Defaults to 1. */
  int flg_addCornerStagger=0; /* [flg] Add corner stagger to grid. Defaults to 0. */
  int flg_addUserArea=0; /* [flg] Read cell area from Grid file (instead of calculate it). Defaults to 0. */
  int flg_addMask=0; /* [flg] Generate mask using missing value attribute in var_nm (iff GRIDSPEC) */
  /* 20150424: ESMF library bug at ESMCI_Grid.C line 365 means var_nm must non-NULL so set to blank name */
  char var_nm[]=""; /* [sng] Iff addMask == 1 use this variable's missing value attribute */
  char **crd_nm=NULL; /* [sng] Iff GRIDSPEC use these lon/lat variable coordinates */
  enum ESMC_FileFormat_Flag grd_fl_typ=ESMC_FILEFORMAT_SCRIP;
  ESMC_Grid grd_dst;
  grd_dst=ESMC_GridCreateFromFile(rgr_nfo->fl_grd_dst,grd_fl_typ,&flg_isSphere,&flg_addCornerStagger,&flg_addUserArea,&flg_addMask,var_nm,crd_nm,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  int *msk_dst; /* [enm] Destination grid mask */
  enum ESMC_GridItem_Flag grd_itm=ESMC_GRIDITEM_MASK;
  msk_dst=ESMC_GridGetItem(grd_dst,grd_itm,stg_lcn_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  int *bnd_lwr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_dst; /* [dgr] Destination grid longitude */
  double *lat_dst; /* [dgr] Destination grid latitude */
  /* 20150427: Written documentation is somewhat inadequate or misleading to normal C-programmers
     Some ESMC_Grid***() functions, like this one, return allocated void pointers that must be cast to desired numeric type
     Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  lon_dst=ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lon_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  lat_dst=ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lat_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  double *lon_out; /* [dgr] Longitude dimension for output file */
  double *lat_out; /* [dgr] Latitude  dimension for output file */
  lon_out=(double *)nco_malloc(bnd_upr_dst[crd_idx_f_0bs_lon_dst]*sizeof(double));
  lat_out=(double *)nco_malloc(bnd_upr_dst[crd_idx_f_0bs_lat_dst]*sizeof(double));
  for(idx=0;idx<bnd_upr_dst[crd_idx_f_0bs_lon_dst];idx++) lon_out[idx]=lon_dst[idx];
  for(idx=0;idx<bnd_upr_dst[crd_idx_f_0bs_lat_dst];idx++) lat_out[idx]=lat_dst[idx];
  const long var_sz_dst=bnd_upr_dst[crd_idx_f_0bs_lon_dst]*bnd_upr_dst[crd_idx_f_0bs_lat_dst]; /* [nbr] Number of elements in variable */
  for(idx=0;idx<var_sz_dst;idx++) msk_dst[idx]=0;

  /* Create source grid with same sizes as those in input data file */
  ESMC_InterfaceInt max_idx_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Util/interface/ESMC_Interface.C
     NB: ESMF is fragile in that dynamic memory provided as input to grids cannot be immediately freed
     In other words, ESMF copies the values of pointers but not the contents of pointers to provided arrays
     To be concrete, the max_idx array provided below cannot be freed until after the ESMC_Finalize() is called */
  max_idx_src=ESMC_InterfaceIntCreate(max_idx,dmn_nbr_grd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  enum ESMC_CoordSys_Flag crd_sys=ESMC_COORDSYS_SPH_DEG; /* NB: dyw sez ESMF supports ESMC_COORDSYS_SPH_DEG only */
  enum ESMC_TypeKind_Flag typ_knd=ESMC_TYPEKIND_R8; /* NB: NCO default is to use double precision for coordinates */
  ESMC_Grid grd_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  grd_src=ESMC_GridCreateNoPeriDim(max_idx_src,&crd_sys,&typ_knd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* fxm: why destroy this now? */
  ESMC_InterfaceIntDestroy(&max_idx_src);
  /* Define stagger locations on source grid. Necessary for remapping later? */
  rcd_esmf=ESMC_GridAddCoord(grd_src,stg_lcn_src);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  
  /* Allocate */
  int *bnd_lwr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_src; /* [dgr] Source grid longitude */
  double *lat_src; /* [dgr] Source grid latitude  */
  lon_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lon_src,stg_lcn_src,bnd_lwr_src,bnd_upr_src,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  lat_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lat_src,stg_lcn_src,NULL,NULL,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"ESMC_VMGet(): localPet = %d, petCount = %d, peCount = %d, pthreadsEnabledFlag = %s, openMPEnabledFlag = %s\n",localPet,petCount,peCount,flg_pthreadsEnabledFlag ? "Enabled" : "Disabled",flg_openMPEnabledFlag ? "Enabled" : "Disabled");
    (void)fprintf(stderr,"ESMC_GridCreateFromFile(): filename = %s, fileTypeFlag = %s, isSphere = %s, addCornerStagger = %s, addUserArea = %s, addMask = %s, var_nm = %s, crd_nm = %s\n",rgr_nfo->fl_grd_dst,nco_esmf_fl_fmt_sng(grd_fl_typ),flg_isSphere ? "Enabled" : "Disabled",flg_addCornerStagger ? "Enabled" : "Disabled",flg_addUserArea ? "Enabled" : "Disabled",flg_addMask ? "Enabled" : "Disabled",var_nm,crd_nm ? "non-NULL" : "NULL");
    (void)fprintf(stderr,"ESMC_GridGetCoord(): bnd_lwr_dst[0..1] = %d, %d; bnd_upr_dst[0..1] = %d, %d;\n",bnd_lwr_dst[0],bnd_lwr_dst[1],bnd_upr_dst[0],bnd_upr_dst[1]);
    (void)fprintf(stderr,"ESMC_InterfaceIntCreate(): No printable information\n");
    (void)fprintf(stderr,"ESMC_GridCreateNoPeriDim(): crd_sys = %s, typ_knd = %s\n",nco_esmf_crd_sys_sng(crd_sys),nco_esmf_typ_knd_sng(typ_knd));
    (void)fprintf(stderr,"ESMC_GridAddCoord(): stg_lcn_src = %s,  stg_lcn_dst = %s\n",nco_esmf_stg_lcn_sng(stg_lcn_src),nco_esmf_stg_lcn_sng(stg_lcn_dst));
    (void)fprintf(stderr,"ESMC_GridGetCoord(): bnd_lwr_src[0..1] = %d, %d; bnd_upr_src[0..1] = %d, %d;\n",bnd_lwr_src[0],bnd_lwr_src[1],bnd_upr_src[0],bnd_upr_src[1]);
    //    (void)fprintf(stderr,"ESMC_InterfaceIntCreate(): max_idx_src[0..1] = %d, %d\n",max_idx_src[0],max_idx_src[1]);
  } /* endif dbg */

  /* Type-conversion and cell-center coordinates */
  for(idx=0;idx<bnd_upr_src[crd_idx_f_0bs_lat_src];idx++) lat_src[idx]=lat_in[idx];
  for(idx=0;idx<bnd_upr_src[crd_idx_f_0bs_lon_src];idx++) lon_src[idx]=lon_in[idx];
  idx=0;
  for(int idx_lat=0;idx_lat<bnd_upr_src[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=0;idx_lon<bnd_upr_src[crd_idx_f_0bs_lon_src];idx_lon++){
      lon_src[idx]=lon_in[idx_lon];
      lat_src[idx]=lat_in[idx_lat];
      idx++;
    } /* endfor */
  } /* endfor */
  
  /* Create source field from source grid */
  ESMC_Field fld_src;
  ESMC_InterfaceInt *gridToFieldMap=NULL; /* [idx] Map of all grid dimensions to field dimensions */
  ESMC_InterfaceInt *ungriddedLBound=NULL; /* [idx] Lower bounds of ungridded dimensions */
  ESMC_InterfaceInt *ungriddedUBound=NULL; /* [idx] Upper bounds of ungridded dimensions */
  fld_src=ESMC_FieldCreateGridTypeKind(grd_src,typ_knd,stg_lcn_src,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_src",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Create destination field from destination grid */
  ESMC_Field fld_dst;
  fld_dst=ESMC_FieldCreateGridTypeKind(grd_dst,typ_knd,stg_lcn_dst,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_dst",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Get field pointers */
  double *fld_src_ptr;
  int localDe=0; /* [idx] Local DE for which information is requested [0..localDeCount-1] */
  fld_src_ptr=(double *)ESMC_FieldGetPtr(fld_src,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  double *fld_dst_ptr;
  fld_dst_ptr=(double *)ESMC_FieldGetPtr(fld_dst,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Get variables from input file */
  (void)nco_inq_varid(in_id,rgr_nfo->var_nm,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  long var_sz_src=1L; /* [nbr] Number of elements in variable (will be self-multiplied) */
  /* Get input dimension sizes */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    var_sz_src*=dmn_cnt[idx];
  } /* end loop over dim */

  /* Allocate space for and obtain latitude */
  void_ptr_var=(void *)nco_malloc_dbg(var_sz_src*nco_typ_lng(var_typ_in),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr_var,var_typ_in);
  float *var_fld=(float *)void_ptr_var;

  /* Type-conversion and ensure every cell has data */
  idx=0;
  for(int idx_lat=bnd_lwr_src[crd_idx_f_0bs_lat_src];idx_lat<=bnd_upr_src[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=bnd_lwr_src[crd_idx_f_0bs_lon_src];idx_lon<=bnd_upr_src[crd_idx_f_0bs_lon_src];idx_lon++){
      fld_src_ptr[idx]=(double)var_fld[idx];
      idx++;
    } /* idx_lon */
  } /* idx_lat */

  /* Initialize dst data ptr */
  idx=0;
  for(int idx_lat=bnd_lwr_dst[crd_idx_f_0bs_lat_src];idx_lat<=bnd_upr_dst[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=bnd_lwr_dst[crd_idx_f_0bs_lon_src];idx_lon<=bnd_upr_dst[crd_idx_f_0bs_lon_src];idx_lon++){
      fld_dst_ptr[idx]=0.0;
      idx++;
    } /* idx_lon */
  } /* idx_lat */

  ESMC_LogWrite("nco_rgr_esmf() invoking ESMC to start regridstore actions",log_msg_typ);
  /* int *msk_val=(int *)nco_malloc(sizeof(int));
     msk_val[0]=1;
     ESMC_InterfaceInt i_msk_val=ESMC_InterfaceIntCreate(msk_val,1,&rcd_esmf);
     rcd_esmf = ESMC_FieldRegridStore(fld_src,fld_dst,&i_msk_val,&i_msk_val,&rte_hnd,NULL,NULL,NULL,&unmap_act,NULL,NULL);
     rcd_esmf=ESMC_FieldRegridStore(fld_src,fld_dst,NULL,NULL,&rte_hnd,NULL,NULL,NULL,&unmap_act,NULL,NULL); */

  ESMC_Field *cll_frc_dst=NULL; /* [frc] Fraction of each cell participating in regridding, destination grid */
  ESMC_Field *cll_frc_src=NULL; /* [frc] Fraction of each cell participating in regridding, source grid */
  ESMC_InterfaceInt *msk_src_rgr=NULL; /* [idx] Points to mask while regridding, source grid */
  /* fxm: unsure whether/why need both msk_dst (above) and msk_dst_rgr (below) */
  ESMC_InterfaceInt *msk_dst_rgr=NULL; /* [idx] Points to mask while regridding, destination grid */
  ESMC_RouteHandle rte_hnd;
  enum ESMC_RegridMethod_Flag rgr_mth=ESMC_REGRIDMETHOD_BILINEAR; /* [flg] Regrid method (default bilinear) */
  enum ESMC_PoleMethod_Flag pll_mth=ESMC_POLEMETHOD_ALLAVG; /* [flg] Regrid method (default ESMC_POLEMETHOD_ALLAVG) */
  enum ESMC_UnmappedAction_Flag unm_act=ESMC_UNMAPPEDACTION_ERROR; /* [flg] Unmapped action (default ESMC_UNMAPPEDACTION_ERROR) */
  int pll_nbr=int_CEWI; /* [nbr] Number of points to average (iff ESMC_POLEMETHOD_NPNTAVG) */
  /* Source: ${DATA}/esmf/src/Infrastructure/Field/interface/ESMC_Field.C */
  rcd_esmf=ESMC_FieldRegridStore(fld_src,fld_dst,msk_src_rgr,msk_dst_rgr,&rte_hnd,&rgr_mth,&pll_mth,&pll_nbr,&unm_act,cll_frc_src,cll_frc_dst);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  enum ESMC_Region_Flag rgn_flg=ESMC_REGION_TOTAL; /* [flg] Whether/how to zero input fields before regridding (default ESMC_REGION_TOTAL) */
  rcd_esmf=ESMC_FieldRegrid(fld_src,fld_dst,rte_hnd,&rgn_flg);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Call once on each PET */
  rcd_esmf=ESMC_Finalize();
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Write regridded data to netCDF file */
  int var_out_id; /* [id] Variable ID */
  int lon_out_id; /* [id] Variable ID for longitude */
  int lat_out_id; /* [id] Variable ID for latitude  */
  int dmn_id_lat; /* [id] Dimension ID */
  int dmn_id_lon; /* [id] Dimension ID */
  (void)nco_def_dim(out_id,lat_nm_in,bnd_upr_dst[crd_idx_f_0bs_lat_dst],&dmn_id_lat);
  (void)nco_def_dim(out_id,lon_nm_in,bnd_upr_dst[crd_idx_f_0bs_lon_dst],&dmn_id_lon);
  int dmn_ids_out[2]; /* [id] Dimension IDs array for output variable */
  long dmn_srt_out[2];
  long dmn_cnt_out[2];
  dmn_ids_out[crd_idx_c_0bs_lat_dst]=dmn_id_lat;
  dmn_ids_out[crd_idx_c_0bs_lon_dst]=dmn_id_lon;
  (void)nco_def_var(out_id,lon_nm_in,var_typ_out,1,&dmn_id_lon,&lon_out_id);
  (void)nco_def_var(out_id,lat_nm_in,var_typ_out,1,&dmn_id_lat,&lat_out_id);
  (void)nco_def_var(out_id,rgr_nfo->var_nm,var_typ_out,2,dmn_ids_out,&var_out_id);
  (void)nco_enddef(out_id);
  dmn_srt_out[crd_idx_c_0bs_lat_dst]=0L;
  dmn_srt_out[crd_idx_c_0bs_lon_dst]=0L;
  dmn_cnt_out[crd_idx_c_0bs_lat_dst]=bnd_upr_dst[crd_idx_f_0bs_lat_dst];
  dmn_cnt_out[crd_idx_c_0bs_lon_dst]=bnd_upr_dst[crd_idx_f_0bs_lon_dst];
  (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_out,lat_out,var_typ_out);
  (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_out,lon_out,var_typ_out);
  (void)nco_put_vara(out_id,var_out_id,dmn_srt_out,dmn_cnt_out,fld_dst_ptr,var_typ_out);

rgr_cln:
  if(rcd_esmf != ESMF_SUCCESS){
    /* ESMF return codes are enumerated in ESMC_ReturnCodes.h and ESMC_LogErr.h
       However, there seems to be no translator function for these codes */
    (void)fprintf(stderr,"%s: ERROR %s received ESMF return code %d\nSee ESMC_ReturnCodes.h or ESMC_LogErr.h for more information, e.g.,\n/bin/more /usr/local/include/ESMC_ReturnCodes.h | grep %d\n",nco_prg_nm_get(),fnc_nm,rcd_esmf,rcd_esmf);
  } /* rcd_esmf */
  if(max_idx) max_idx=(int *)nco_free(max_idx);
  if(bnd_lwr_src) bnd_lwr_src=(int *)nco_free(bnd_lwr_src);
  if(bnd_upr_src) bnd_upr_src=(int *)nco_free(bnd_upr_src);
  if(bnd_lwr_dst) bnd_lwr_dst=(int *)nco_free(bnd_lwr_dst);
  if(bnd_upr_dst) bnd_upr_dst=(int *)nco_free(bnd_upr_dst);
  if(void_ptr_var) void_ptr_var=(void *)nco_free(void_ptr_var);
  if(lat_out) lat_out=(double *)nco_free(lat_out);
  if(lon_out) lon_out=(double *)nco_free(lon_out);
  if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
  if(dmn_id) dmn_id=(int *)nco_free(dmn_id);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);

  rcd_esmf=ESMC_FieldRegridRelease(&rte_hnd);
  rcd_esmf=ESMC_FieldDestroy(&fld_src);
  rcd_esmf=ESMC_FieldDestroy(&fld_dst);
  rcd_esmf=ESMC_GridDestroy(&grd_src);
  rcd_esmf=ESMC_GridDestroy(&grd_dst);
  ESMC_Finalize();

  return rcd;
} /* nco_rgr_esmf */
#endif /* !ENABLE_ESMF */

