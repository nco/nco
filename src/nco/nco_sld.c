/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sld.c,v 1.12 2015-01-30 04:16:48 zender Exp $ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   ncks -4 -O -C -v ppc_dbl --ppc /ppc_dbl=3 ~/nco/data/in.nc ~/foo.nc */

#include "nco_sld.h" /* Swath-Like Data */

kvmap_sct nco_sng2map /* [fnc] parsing string to key-value pair */
(char *str, /* I [sng] string to parse with a = */
kvmap_sct sm) /* O [sct] key-value pair */
{
  char *prt;

  int icnt=0;

  prt=strtok(str,"=");
  while(prt){
    icnt++;
    nco_sng_strip(prt);
    switch(icnt){
    case 1:
      sm.key=strdup(prt);
      break;
    case 2:
      sm.value=strdup(prt);
      break;
    default:
      fprintf(stderr,"cannot get key-value pair from this imput: %s\n", str);
      break;
    }/* end switch */
    prt=strtok(NULL,"=");
  }/* end while */
  return sm;
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
nco_ppc_set( /* Set PPC based on user specifications */
 char *const ppc_arg[], /* I [sng] List of user-specified PPC */
 const int ppc_nbr, /* I [nbr] Number of PPC specified */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  char *arg;
  int idx;
  int ippc=0;
  kvmap_sct *ppcs;  /* [sct] PPC container */
  kvmap_sct sm;

  ppcs=(kvmap_sct *)nco_malloc(NC_MAX_VARS*sizeof(kvmap_sct));

  /* Parse PPCs */
  for(idx=0;idx<ppc_nbr;idx++){
    arg=(char *)strdup(ppc_arg[idx]);
    if(!strstr(arg,"=")){
      (void)fprintf(stdout,"%s: Invalid --ppc specification: %s\n",nco_prg_nm_get(),arg);
      if(ppcs) nco_kvmaps_free(ppcs);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    sm=nco_sng2map(arg,sm);
    if(sm.key){
      char *items[BUFSIZ];
      int idxi;
      int item_nbr=nco_sng2array(",",sm.key,items); /* multi-var specification */
      for(idxi=0;idxi<item_nbr;idxi++){ /* expand multi-var specification */
        ppcs[ippc].key=strdup(items[idxi]);
        ppcs[ippc].value=strdup(sm.value);
        ippc++;
      } /* end for */
    } /* end if */
  } /* end for */

  /* PPC default exists, set all non-coordinate variables to default first */
  for(idx=0;idx<ippc;idx++){
    if(!strcasecmp(ppcs[idx].key,"default")){
      trv_tbl_ppc_set_dflt(ppcs[idx].value,trv_tbl);
      break; /* only one default is needed */
    } /* endif */
  } /* end for */

  /* Set explicit, non-default PPCs that can overwrite default */
  for(idx=0;idx<ippc;idx++){
    if(!strcasecmp(ppcs[idx].key,"default")) continue;
    trv_tbl_ppc_set_var(ppcs[idx].key,ppcs[idx].value,trv_tbl);
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

  if(ppcs) nco_kvmaps_free(ppcs);
} /* end nco_ppc_set() */

void
trv_tbl_ppc_set_dflt /* Set PPC value for all non-coordinate variables for --ppc default  */
(const char * const sppc, /* I [sng] User input for least significant digit */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  int ppc;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  nco_bool flg_nsd=True; /* [flg] PPC is NSD */

  if(sppc[0] == '.'){
    flg_nsd=False; /* DSD */
    ppc=(int)strtol(sppc+1L,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(sppc+1L,"strtol",sng_cnv_rcd);
  }else{ /* NSD */
    ppc=(int)strtol(sppc,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(sppc,"strtol",sng_cnv_rcd);
  } /* end if */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !trv_tbl->lst[idx_tbl].is_crd_var){
      trv_tbl->lst[idx_tbl].ppc=ppc;
      trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
    } /* endif */
} /* end trv_tbl_ppc_set_dflt() */

void
trv_tbl_ppc_set_var
(const char * const var_nm, /* I [sng] Variable name to find */
 const char * const sppc, /* I [sng] User input for least significant digit */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  const char sls_chr='/'; /* [chr] Slash character */
  int mch_nbr=0;
  int ppc;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  nco_bool flg_nsd=True; /* [flg] PPC is NSD */

  if(sppc[0] == '.'){ /* DSD */
    flg_nsd=False;
    ppc=(int)strtol(sppc+1L,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(sppc+1L,"strtol",sng_cnv_rcd);
  }else{ /* NSD */
    ppc=(int)strtol(sppc,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(sppc,"strtol",sng_cnv_rcd);
  } /* end else */

  if(strpbrk(var_nm,".*^$\\[]()<>+?|{}")){ /* regular expression ... */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
    regmatch_t *result;
    regex_t *rx;
    size_t rx_prn_sub_xpr_nbr;
    rx=(regex_t *)nco_malloc(sizeof(regex_t));
    if(strchr(var_nm,sls_chr)){ /* Full name is used */
      char sng2mch[BUFSIZ]="^";
      strcat(sng2mch,var_nm);
      if(regcomp(rx,sng2mch,(REG_EXTENDED | REG_NEWLINE))){ /* Compile regular expression */
        (void)fprintf(stdout,"%s: ERROR trv_tbl_set_ppc() error in regular expression \"%s\"\n",nco_prg_nm_get(),var_nm);
        nco_exit(EXIT_FAILURE);
      } /* endif */
      rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */
      result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);
      for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
        if(!regexec(rx,trv_tbl->lst[idx_tbl].nm_fll,rx_prn_sub_xpr_nbr,result,0)&&(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var)){
          trv_tbl->lst[idx_tbl].ppc=ppc;
          trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
          mch_nbr++;
        } /* endif */
      } /* endfor */
    }else{ /* Relative name is used */
      if(regcomp(rx,var_nm,(REG_EXTENDED | REG_NEWLINE))){ /* Compile regular expression */
        (void)fprintf(stdout,"%s: ERROR trv_tbl_set_ppc() error in regular expression \"%s\"\n",nco_prg_nm_get(),var_nm);
        nco_exit(EXIT_FAILURE);
      }
      rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */
      result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);
      for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
        if(!regexec(rx,trv_tbl->lst[idx_tbl].nm,rx_prn_sub_xpr_nbr,result,0)&&(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var)){
          trv_tbl->lst[idx_tbl].ppc=ppc;
          trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
          mch_nbr++;
        } /* endif */
      } /* endfor */
    } /* end Full name */
    regfree(rx); /* Free regular expression data structure */
    rx=(regex_t *)nco_free(rx);
    result=(regmatch_t *)nco_free(result);
#else /* !NCO_HAVE_REGEX_FUNCTIONALITY */
    (void)fprintf(stdout,"%s: ERROR: Sorry, wildcarding (extended regular expression matches to variables) was not built into this NCO executable, so unable to compile regular expression \"%s\".\nHINT: Make sure libregex.a is on path and re-build NCO.\n",nco_prg_nm_get(),usr_sng);
    nco_exit(EXIT_FAILURE);
#endif /* !NCO_HAVE_REGEX_FUNCTIONALITY */
  }else if(strchr(var_nm,sls_chr)){ /* Full name */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var){
        if(!strcmp(var_nm,trv_tbl->lst[idx_tbl].nm_fll)){
          trv_tbl->lst[idx_tbl].ppc=ppc;
          trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
          mch_nbr++;
          return; /* Only one match with full name */
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
} /* end trv_tbl_ppc_set_var() */

char *
nco_sng_strip( /* [fnc] Strip leading and trailing white space */
char *sng)
{
  /* fxm: seems not working for \n??? */
  char *start=sng;
  while(isspace(*start)) start++;
  int end=strlen(start);
  if(start != sng){
    memmove(sng, start, end);
    sng[end]='\0';
  }
  while(isblank(*(sng+end-1))) end--;
  sng[end]='\0';
  return sng;
}/* end nco_sng_strip */

int nco_sng2array(const char *delim, const char *str, char **sarray)
{
  int idx=0;
  char *tstr;
  tstr=strdup(str);
  sarray[idx]=strtok(tstr,delim);
  while(sarray[idx]!=NULL) sarray[++idx]=strtok(NULL,delim);
  return idx;
}/* end nco_sng2array */

void nco_kvmaps_free(kvmap_sct *kvmaps)
{
  int idx=0;
  while(kvmaps[idx].key){
    kvmaps[idx].key=nco_free(kvmaps[idx].key);
    kvmaps[idx].value=nco_free(kvmaps[idx].value);
  }
  kvmaps=nco_free(kvmaps);
}/* end nco_kvmaps_free */

// nco_kvmap_prn
void nco_kvmap_prn(kvmap_sct vm)
{
  if(!vm.key) return;
  (void)fprintf(stdout,"%s=",vm.key);
  (void)fprintf(stdout,"%s\n",vm.value);
} /* end nco_kvmap_prn */

int 
hdlscrip( /* return 0 invalid SCRIP file or rcd, 1 success */ 
char *fl_nm_scrip, /* SCRIP file name with proper path */
kvmap_sct *smps)/* structure to hold contents of SCRIP file */ 
{
  char line[BUFSIZ];

  int icnt;
  int idx=0;

  FILE *fl_scrip;

  fl_scrip=fopen(fl_nm_scrip,"r");

  if(!fl_scrip){
    fprintf(stderr,"Cannot open SCRIP file %s\n",fl_nm_scrip);
    return NCO_ERR;
  }

  while(fgets(line,sizeof(line),fl_scrip)){
    if(!strstr(line,"=")){
      fprintf(stderr,"invalid line in SCRIP file: %s\n", line);
      fclose(fl_scrip);
      return NCO_ERR;
    }
    smps[idx]=nco_sng2map(line,smps[idx]);
    if(!smps[idx].key){
      fclose(fl_scrip);
      return NCO_ERR;
    }else{
      idx++;
    }
  } /* finish parsing SCRIP file */
  fclose(fl_scrip);

  for(icnt=0;icnt<idx;icnt++) nco_kvmap_prn(smps[icnt]);

  return NCO_NOERR;
} /* end hdlscrip */
