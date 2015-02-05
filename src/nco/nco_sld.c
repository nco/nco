/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sld.c,v 1.20 2015-02-05 23:08:39 zender Exp $ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   ncks -4 -O -C -v ppc_dbl --ppc /ppc_dbl=3 ~/nco/data/in.nc ~/foo.nc */

#include "nco_sld.h" /* Swath-Like Data */

kvmap_sct nco_sng2map /* [fnc] parsing string to key-value pair */
(char *sng, /* I [sng] string to parse with a = */
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
      (void)fprintf(stderr,"Cannot get key-value pair from this input: %s\n",sng);
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
  /* NB: Can fail when output file has fewer variables than input file (i.e., was subsetted)
     20150126: functionality moved to nco_xtr_dfn() */
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
    aed.type=NC_INT; /* NB: value changes if it is assigned outside for loop */
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
(int *dfl_lvl, /* O [enm] Deflate level */
 const int fl_out_fmt,  /* I [enm] Output file format */
 char * const ppc_arg[], /* I [sng] List of user-specified PPC */
 const int ppc_nbr, /* I [nbr] Number of PPC specified */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  int idx;
  int ippc=0;
  kvmap_sct *ppc_lst;  /* [sct] PPC container */
  kvmap_sct kvm;

  if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    /* If user did not explicitly set deflate level for this file .. */
    if(*dfl_lvl == NCO_DFL_LVL_UNDEFINED){
      *dfl_lvl=1;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Precision-Preserving Compression (PPC) automatically activating file-wide deflation level = %d\n",nco_prg_nm_get(),*dfl_lvl);
    } /* endif */
  } /* endif */

  if(fl_out_fmt != NC_FORMAT_NETCDF4 && fl_out_fmt != NC_FORMAT_NETCDF4_CLASSIC){
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Requested Precision-Preserving Compression (PPC) on netCDF3 output dataset. Unlike netCDF4, netCDF3 does not support internal compression. To take full advantage of PPC consider writing file as netCDF4 enhanced (e.g., %s -4 ...) or classic (e.g., %s -7 ...). Or consider compressing the netCDF3 file afterwards with, e.g., gzip or bzip2. File must then be uncompressed with, e.g., gunzip or bunzip2 before netCDF readers will recognize it. See http://nco.sf.net/nco.html#ppc for more information on PPC strategies.\n",nco_prg_nm_get(),nco_prg_nm_get(),nco_prg_nm_get());
  } /* endif */

  ppc_lst=(kvmap_sct *)nco_malloc(NC_MAX_VARS*sizeof(kvmap_sct));

  /* Parse PPCs */
  for(idx=0;idx<ppc_nbr;idx++){
    if(!strstr(ppc_arg[idx],"=")){
      (void)fprintf(stdout,"%s: Invalid --ppc specification: %s\n",nco_prg_nm_get(),ppc_arg[idx]);
      if(ppc_lst) nco_kvmaps_free(ppc_lst);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    kvm=nco_sng2map(ppc_arg[idx],kvm);
    if(kvm.key){
      char *items[BUFSIZ];
      int idxi;
      int item_nbr=nco_sng2array(",",kvm.key,items); /* multi-var specification */
      for(idxi=0;idxi<item_nbr;idxi++){ /* expand multi-var specification */
        ppc_lst[ippc].key=strdup(items[idxi]);
        ppc_lst[ippc].value=strdup(kvm.value);
        ippc++;
      } /* end for */
    } /* end if */
  } /* end for */

  /* PPC default exists, set all non-coordinate variables to default first */
  for(idx=0;idx<ippc;idx++){
    if(!strcasecmp(ppc_lst[idx].key,"default")){
      nco_ppc_set_dflt(ppc_lst[idx].value,trv_tbl);
      break; /* only one default is needed */
    } /* endif */
  } /* end for */

  /* Set explicit, non-default PPCs that can overwrite default */
  for(idx=0;idx<ippc;idx++){
    if(!strcasecmp(ppc_lst[idx].key,"default")) continue;
    nco_ppc_set_var(ppc_lst[idx].key,ppc_lst[idx].value,trv_tbl);
  } /* end for */

  /* Unset PPC and flag for all variables with excessive PPC
     Operational definition of maximum PPC is maximum decimal precision of double = DBL_DIG = 15 */
  const int nco_max_ppc=15;
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].ppc != NC_MAX_INT)
      if(trv_tbl->lst[idx_tbl].ppc >= nco_max_ppc){
	trv_tbl->lst[idx_tbl].ppc=NC_MAX_INT;
	trv_tbl->lst[idx_tbl].flg_nsd=True;
      } /* endif */

  if(ppc_lst) nco_kvmaps_free(ppc_lst);
} /* end nco_ppc_ini() */

void
nco_ppc_set_dflt /* Set PPC value for all non-coordinate variables for --ppc default  */
(const char * const ppc_arg, /* I [sng] User input for precision-preserving compression */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  int ppc;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  nco_bool flg_nsd=True; /* [flg] PPC is NSD */

  if(ppc_arg[0] == '.'){
    flg_nsd=False; /* DSD */
    ppc=(int)strtol(ppc_arg+1L,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(ppc_arg+1L,"strtol",sng_cnv_rcd);
  }else{ /* NSD */
    ppc=(int)strtol(ppc_arg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(ppc_arg,"strtol",sng_cnv_rcd);
    if(ppc <= 0){
      (void)fprintf(stdout,"%s ERROR Number of Significant Digits (NSD) must be postive. Default was specified as %d.\n",nco_prg_nm_get(),ppc);
      nco_exit(EXIT_FAILURE);
    } /* endif */    
  } /* end if */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !trv_tbl->lst[idx_tbl].is_crd_var){
      trv_tbl->lst[idx_tbl].ppc=ppc;
      trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
    } /* endif */
} /* end nco_ppc_set_dflt() */

void
nco_ppc_set_var
(const char * const var_nm, /* I [sng] Variable name to find */
 const char * const ppc_arg, /* I [sng] User input for precision-preserving compression */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  const char sls_chr='/'; /* [chr] Slash character */
  int mch_nbr=0;
  int ppc;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  nco_bool flg_nsd=True; /* [flg] PPC is NSD */

  if(ppc_arg[0] == '.'){ /* DSD */
    flg_nsd=False;
    ppc=(int)strtol(ppc_arg+1L,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(ppc_arg+1L,"strtol",sng_cnv_rcd);
  }else{ /* NSD */
    ppc=(int)strtol(ppc_arg,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(ppc_arg,"strtol",sng_cnv_rcd);
    if(ppc <= 0){
      (void)fprintf(stdout,"%s ERROR Number of Significant Digits (NSD) must be positive. Specified value for %s was %d.\n",nco_prg_nm_get(),var_nm,ppc);
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
      char sng2mch[BUFSIZ]="^";
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
	    trv_tbl->lst[idx_tbl].ppc=ppc;
	    trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
	    mch_nbr++;
	  } /* endif */
        } /* endif */
      } /* endfor */
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
	    trv_tbl->lst[idx_tbl].ppc=ppc;
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
	  trv_tbl->lst[idx_tbl].ppc=ppc;
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
	  trv_tbl->lst[idx_tbl].ppc=ppc;
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
  int end=strlen(srt);
  if(srt != sng){
    memmove(sng,srt,end);
    sng[end]='\0';
  } /* endif */
  while(isblank(*(sng+end-1))) end--;
  sng[end]='\0';
  return sng;
}/* end nco_sng_strip */

int nco_sng2array(const char *dlm, const char *str, char **sarray)
{
  int idx=0;
  char *tstr;
  tstr=strdup(str);
  sarray[idx]=strtok(tstr,dlm);
  while(sarray[idx]) sarray[++idx]=strtok(NULL,dlm);
  return idx;
} /* end nco_sng2array */

void nco_kvmaps_free(kvmap_sct *kvmaps)
{
  int idx=0;
  while(kvmaps[idx].key){
    kvmaps[idx].key=(char *)nco_free(kvmaps[idx].key);
    kvmaps[idx].value=(char *)nco_free(kvmaps[idx].value);
  } /* end while */
  kvmaps=(kvmap_sct *)nco_free(kvmaps);
} /* end nco_kvmaps_free */

void nco_kvmap_prn(kvmap_sct vm)
{
  if(!vm.key) return;
  (void)fprintf(stdout,"%s=",vm.key);
  (void)fprintf(stdout,"%s\n",vm.value);
} /* end nco_kvmap_prn */

int 
hdlscrip( /* return 0 invalid SCRIP file or rcd, 1 success */ 
char *fl_nm_scrip, /* SCRIP file name with proper path */
kvmap_sct *kvm_scrip)/* structure to hold contents of SCRIP file */ 
{
  char line[BUFSIZ];

  int icnt;
  int idx=0;

  FILE *fl_scrip;

  fl_scrip=fopen(fl_nm_scrip,"r");

  if(!fl_scrip){
    fprintf(stderr,"Cannot open SCRIP file %s\n",fl_nm_scrip);
    return NCO_ERR;
  } /* endif */

  while(fgets(line,sizeof(line),fl_scrip)){
    if(!strstr(line,"=")){
      fprintf(stderr,"invalid line in SCRIP file: %s\n", line);
      fclose(fl_scrip);
      return NCO_ERR;
    } /* endif */
    kvm_scrip[idx]=nco_sng2map(line,kvm_scrip[idx]);
    if(!kvm_scrip[idx].key){
      fclose(fl_scrip);
      return NCO_ERR;
    }else{
      idx++;
    } /* end else */
  } /* finish parsing SCRIP file */
  fclose(fl_scrip);

  for(icnt=0;icnt<idx;icnt++) nco_kvmap_prn(kvm_scrip[icnt]);

  return NCO_NOERR;
} /* end hdlscrip */

#if 0
int
nco_ESMF_Regrid(
wgt,
data_in,
&data_out
){
// Given weights and data, do regridding
ESMF_Regrid(wgt,data_in,data_);
// write data_out
}

int
nco_rgr_wgt_grd_mk();

typedef struct{
char *fl_grd_in;
char *fl_grd_out;
flg_1...;
} rgr_wgt_sct;

int
nco_rgr_wgt_grd_mk(
rgr_wgt_sct *rgr_wgt;
){
// Given grids, compute weights

// NCO reads and checks inputs
open(fl_grd_in);
read lat_in,lon_in,lat_in_crn,... // nco_var_get

// populate rgr_wgt

nco_ESMF_RegridWeightGen(lat_in_crn,lon_in_crn,lat_out_crn,lon_out_crn,&wgt);

// NCO writes output
write wgt // nco_var_get

}

int
nco_ESMF_RegridWeightGen(
lat_in_crn,
lon_in_crn,
...
){
// Given grids, compute weights

const char fnc_nm[]="nco_ESMF_RegridWeightGen()";
int rcd=NC_NOERR;
rcd=ESMF_RegridWeightGen(lat_in_crn,lon_in_crn,lat_out_crn,lon_out_crn,&wgt);
if(rcd != NC_NOERR) nco_err_exit(rcd,fnc_nm);
return rcd;

// NCO writes output
}
#endif /* endif False */
