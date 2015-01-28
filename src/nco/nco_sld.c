/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sld.c,v 1.9 2015-01-28 23:03:53 dywei2 Exp $ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   ncks -4 -O -C -v lsd_dbl --lsd /lsd_dbl=3 ~/nco/data/in.nc ~/foo.nc */

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
nco_lsd_att_prc /* [fnc] Create LSD attribute */
(const int nc_id, /* I [id] Input netCDF file ID */
 const trv_tbl_sct * const trv_tbl) /* I [sct] GTT (Group Traversal Table) */
{
  /* NB: Can fail when output file has fewer variables than input file (i.e., was subsetted)
     20150126: functionality moved to nco_xtr_dfn() */
  aed_sct aed;
  char att_nm[]="least_significant_digit";
  int grp_id; /* [id] Group ID */
  int var_id; /* [id] Variable ID */
  int lsd;
  int rcd=NC_NOERR;
  long att_sz;
  nc_type att_typ;
  ptr_unn att_val;
  int lsd_xst;

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
    lsd=trv_tbl->lst[idx_tbl].lsd;
    if(lsd == NC_MAX_INT) continue;
    trv_sct var_trv=trv_tbl->lst[idx_tbl];
    aed.var_nm=strdup(var_trv.nm);
    (void)nco_inq_grp_full_ncid(nc_id,var_trv.grp_nm_fll,&grp_id); /* Obtain group ID */
    (void)nco_inq_varid(grp_id,var_trv.nm,&var_id); /* Obtain variable ID */
    att_val.ip=&lsd;
    aed.id=var_id;
    aed.val=att_val;
    if(var_trv.flg_nsd) aed.att_nm="number_of_significant_digits";
    else aed.att_nm="least_significant_digit";
    aed.type=NC_INT; /* NB: value changes if it is assigned outside for loop */
    aed.att_nm=att_nm;
    aed.sz=1L;
    aed.mode=aed_create; 
    rcd=nco_inq_att_flg(nc_id,var_id,aed.att_nm,&att_typ,&att_sz);
    if(rcd == NC_NOERR && aed.sz == att_sz && aed.type == att_typ){
      (void)nco_get_att(nc_id,var_id,aed.att_nm,&lsd_xst,att_typ);
      if(lsd < lsd_xst) aed.mode=aed_overwrite;
      else continue; /* no changes needed */
    } /* endif */
    (void)nco_aed_prc(nc_id,var_id,aed);
  } /* end loop */
} /* end nco_lsd_att_prc() */

void
nco_lsd_set( /* Set LSD based on user specifications */
 char *const lsd_arg[], /* I [sng] List of user-specified LSD */
 const int lsd_nbr, /* I [nbr] Number of LSD specified */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  char *arg;
  int idx;
  int ilsd=0;
  kvmap_sct *lsds;  /* [sct] LSD container */
  kvmap_sct sm;

  lsds=(kvmap_sct *)nco_malloc(NC_MAX_VARS*sizeof(kvmap_sct));

  /* Parse lsds */
  for(idx=0;idx<lsd_nbr;idx++){
    arg=(char *)strdup(lsd_arg[idx]);
    if(!strstr(arg,"=")){
      (void)fprintf(stdout,"%s: Invalid --lsd specification: %s\n",nco_prg_nm_get(),arg);
      if(lsds) nco_kvmaps_free(lsds);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    sm=nco_sng2map(arg,sm);
    if(sm.key){
      char *items[BUFSIZ];
      int idxi;
      int item_nbr=nco_sng2array(",",sm.key,items); /* multi-var specification */
      for(idxi=0;idxi<item_nbr;idxi++){ /* expand multi-var specification */
        lsds[ilsd].key=strdup(items[idxi]);
        lsds[ilsd].value=strdup(sm.value);
        ilsd++;
      } /* end for */
    } /* end if */
  } /* end for */

  /* Set lsds */
  for(idx=0;idx<ilsd;idx++){ /* if LSD default exists, set all non-coordinate vars first */
    if(!strcasecmp(lsds[idx].key, "default")){
      trv_tbl_lsd_set_dflt(lsds[idx].value,trv_tbl);
      break; /* only one default is needed */
    }
  } /* end for */
  for(idx=0;idx<ilsd;idx++){ /* set non-default LSDs that can overwrite dflt */
    if(!strcasecmp(lsds[idx].key, "default")) continue;
    trv_tbl_lsd_set_var(lsds[idx].key,lsds[idx].value,trv_tbl);
  } /* end for */

  if(lsds) nco_kvmaps_free(lsds);
} /* end nco_lsd_set() */

void
trv_tbl_lsd_set_dflt /* Set the lsd value for all non-coordinate vars for --lsd default  */
(const char * const slsd, /* I [sng] user input for least significant digit */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  int lsd;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  nco_bool flg_nsd=True;                 /* [flg] LSD is NSD when it's true */

  if(slsd[0]=='.'){
    flg_nsd=False; /* DSD */
    lsd=(int)strtol(slsd+1,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(slsd+1,"strtol",sng_cnv_rcd);
  }else{ /* NSD */
    lsd=(int)strtol(slsd,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(slsd,"strtol",sng_cnv_rcd);
  } /* end if */

  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !trv_tbl->lst[idx_tbl].is_crd_var){
      trv_tbl->lst[idx_tbl].lsd=lsd;
      trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
    }
} /* end trv_tbl_lsd_set_dflt() */

void
trv_tbl_lsd_set_var
(const char * const var_nm, /* I [sng] Variable name to find */
 const char * const slsd, /* I [sng] user input for least significant digit */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  const char sls_chr='/'; /* [chr] Slash character */
  int mch_nbr=0;
  int lsd;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  nco_bool flg_nsd=True;                 /* [flg] LSD is NSD when it's true */

  if(slsd[0]=='.'){ /* DSD */
    flg_nsd=False;
    lsd=(int)strtol(slsd+1,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(slsd+1,"strtol",sng_cnv_rcd);
  }else{ /* NSD */
    lsd=(int)strtol(slsd,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(slsd,"strtol",sng_cnv_rcd);
  }

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
        (void)fprintf(stdout,"%s: ERROR trv_tbl_set_lsd() error in regular expression \"%s\"\n",nco_prg_nm_get(),var_nm);
        nco_exit(EXIT_FAILURE);
      } /* endif */
      rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */
      result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);
      for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
        if(!regexec(rx,trv_tbl->lst[idx_tbl].nm_fll,rx_prn_sub_xpr_nbr,result,0)&&(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var)){
          trv_tbl->lst[idx_tbl].lsd=lsd;
          trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
          mch_nbr++;
        } /* endif */
      } /* endfor */
    }else{ /* Relative name is used */
      if(regcomp(rx,var_nm,(REG_EXTENDED | REG_NEWLINE))){ /* Compile regular expression */
        (void)fprintf(stdout,"%s: ERROR trv_tbl_set_lsd() error in regular expression \"%s\"\n",nco_prg_nm_get(),var_nm);
        nco_exit(EXIT_FAILURE);
      }
      rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */
      result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);
      for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
        if(!regexec(rx,trv_tbl->lst[idx_tbl].nm,rx_prn_sub_xpr_nbr,result,0)&&(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var)){
          trv_tbl->lst[idx_tbl].lsd=lsd;
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
          trv_tbl->lst[idx_tbl].lsd=lsd;
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
          trv_tbl->lst[idx_tbl].lsd=lsd;
          trv_tbl->lst[idx_tbl].flg_nsd=flg_nsd;
          mch_nbr++;
        } /* endif */
      } /* endif */
    } /* endfor */
  } /* end Full name */
} /* end trv_tbl_lsd_set_var() */

char * nco_sng_strip(/* strip off heading and tailing white spaces.  seems not working for \n??? */
char *str) 
{
  char *start = str;
  while(isspace(*start)) start++;
  int end = strlen(start);
  if(start != str) {
    memmove(str, start, end);
    str[end] = '\0';
  }
  while(isblank(*(str+end-1))) end--;
  str[end] = '\0';
  return str;
}/* end nco_sng_strip */

int nco_sng2array(const char *delim, const char *str, char **sarray)
{
  int idx=0;
  char *tstr;
  tstr=strdup(str);
  sarray[idx]=strtok(tstr, delim);
  while(sarray[idx]!=NULL)
  {
    sarray[++idx] = strtok(NULL, delim);
  }
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
