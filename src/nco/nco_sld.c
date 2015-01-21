/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sld.c,v 1.2 2015-01-21 20:06:40 dywei2 Exp $ */

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
  int icnt=0;
  char * prt;
  prt=strtok(str, "=");
  while(prt != NULL){
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
    prt=strtok(NULL, "=");
  }/* end while */
  return sm;
}/* end nco_sng2map */

void
nco_lsd_set(/* set lsd based user specifications */
 char *const lsd_arg[], /* I [sng] List of user-specified lsd */
 const int lsd_nbr, /* I [nbr] Number of lsd specified */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  int idx;
  int ilsd=0;
  char *arg;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  kvmap_sct *lsds;  /* container to hold lsd info */
  kvmap_sct sm;

  lsds=(kvmap_sct *)nco_malloc(NC_MAX_VARS*sizeof(kvmap_sct));
  /* parsing lsds */
  for(idx=0;idx<lsd_nbr;idx++){
    arg=(char *)strdup(lsd_arg[idx]);
    sm=nco_sng2map(arg,sm);
    if(!strstr(arg,"=")){
      (void)fprintf(stdout,"%s: invalid --lsd specification: %s\n",nco_prg_nm_get(),arg);
      if(lsds) nco_kvmaps_free(lsds);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    if(sm.key){
      char *items[BUFSIZ];
      int idxi;
      int item_nbr=nco_sng2array(",", sm.key, items); /* multi-var specification */
      for(idxi=0;idxi<item_nbr;idxi++){ /* expand multi-var specification */
        lsds[ilsd].key=strdup(items[idxi]);
        lsds[ilsd].value=strdup(sm.value);
        ilsd++;
      } /* end for */
    } /* end if */
  } /* end for */

  /* setting lsds */
  for(idx=0;idx<ilsd;idx++){ /* if lsd default exists, set all non-coordinate vars first */
    if(!strcasecmp(lsds[idx].key, "default")){
      trv_tbl_lsd_set_dflt((int)strtol(lsds[idx].value,&sng_cnv_rcd,NCO_SNG_CNV_BASE10),trv_tbl);
      if(*sng_cnv_rcd) nco_sng_cnv_err(lsds[idx].value,"strtol",sng_cnv_rcd);
      break;
    }
  } /* end for */
  for(idx=0;idx<ilsd;idx++){ /* set non-default lsds that can overwrite dflt */
    if(!strcasecmp(lsds[idx].key, "default")) continue;
    trv_tbl_lsd_set_var(lsds[idx].key,(int)strtol(lsds[idx].value,&sng_cnv_rcd,NCO_SNG_CNV_BASE10),trv_tbl);
    if(*sng_cnv_rcd) nco_sng_cnv_err(lsds[idx].value,"strtol",sng_cnv_rcd);
  } /* end for */

  if(lsds) nco_kvmaps_free(lsds);
} /* end nco_lsd_set() */

void
trv_tbl_lsd_set_dflt /* Set the lsd value for all non-coordinate vars */
(const int lsd, /* I [nbr] Least significant digit */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  /* Purpose: Initialize LSD member to default value for no compression
     Function is currently obsolete becasuse LSD member is initialize in nco_grp_itr() */
  for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++)
    if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var && !trv_tbl->lst[idx_tbl].is_crd_var){
      trv_tbl->lst[idx_tbl].lsd=lsd;
    }
} /* end trv_tbl_lsd_set_dflt() */


void
trv_tbl_lsd_set_var
(const char * const var_nm, /* I [sng] Variable name to find */
 const int lsd, /* I [nbr] Least significant digit */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal table */
{
  const char sls_chr='/'; /* [chr] Slash character */
  int mch_nbr=0;

  if(strpbrk(var_nm,".*^$\\[]()<>+?|{}")){ /* regular expression ... */
#ifdef NCO_HAVE_REGEX_FUNCTIONALITY
    regmatch_t *result;
    regex_t *rx;
    size_t rx_prn_sub_xpr_nbr;
    rx=(regex_t *)nco_malloc(sizeof(regex_t));
    if(strchr(var_nm,sls_chr)){ /* full name is used */
      char sng2mch[BUFSIZ]="^";
      strcat(sng2mch,var_nm);
      if(regcomp(rx,sng2mch,(REG_EXTENDED | REG_NEWLINE))){ /* Compile regular expression */
        (void)fprintf(stdout,"%s: ERROR trv_tbl_set_lsd() error in regular expression \"%s\"\n",nco_prg_nm_get(),var_nm);
        nco_exit(EXIT_FAILURE);
      }
      rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */
      result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);
      for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
        if(!regexec(rx,trv_tbl->lst[idx_tbl].nm_fll,rx_prn_sub_xpr_nbr,result,0)&&(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var)){
          trv_tbl->lst[idx_tbl].lsd=lsd;
          mch_nbr++;
        } /* endif */
      } /* endfor */
    }else{ /* relative name is used */
      if(regcomp(rx,var_nm,(REG_EXTENDED | REG_NEWLINE))){ /* Compile regular expression */
        (void)fprintf(stdout,"%s: ERROR trv_tbl_set_lsd() error in regular expression \"%s\"\n",nco_prg_nm_get(),var_nm);
        nco_exit(EXIT_FAILURE);
      }
      rx_prn_sub_xpr_nbr=rx->re_nsub+1L; /* Number of parenthesized sub-expressions */
      result=(regmatch_t *)nco_malloc(sizeof(regmatch_t)*rx_prn_sub_xpr_nbr);
      for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
        if(!regexec(rx,trv_tbl->lst[idx_tbl].nm,rx_prn_sub_xpr_nbr,result,0)&&(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var)){
          trv_tbl->lst[idx_tbl].lsd=lsd;
          mch_nbr++;
        } /* endif */
      } /* endfor */
    }
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
          mch_nbr++;
          return; /* Only one match with full name */
        } /* endif */
      } /* endif */
    } /* endfor */
  }else{ /* not full name then set all matching vars */
    for(unsigned idx_tbl=0;idx_tbl<trv_tbl->nbr;idx_tbl++){
      if(trv_tbl->lst[idx_tbl].nco_typ == nco_obj_typ_var){
        if(!strcmp(var_nm,trv_tbl->lst[idx_tbl].nm)){
          trv_tbl->lst[idx_tbl].lsd=lsd;
          mch_nbr++;
        } /* endif */
      } /* endif */
    } /* endfor */
  }
} /* end trv_tbl_lsd_set_var() */
/* DYW end*/


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
  if(vm.key == NULL) return;
  (void)fprintf(stdout,"%s=", vm.key);
  (void)fprintf(stdout,"%s\n", vm.value);
} /* end nco_kvmap_prn */

int 
hdlscrip( /* return 0 invalid scrip file or rcd, 1 success */ 
char *scripflnm, /* scrip file name with proper path */
kvmap_sct *smps)/* structure to hold contents of scrip file */ 
{
  char line[BUFSIZ];
  FILE *sfile=fopen(scripflnm, "r");
  if (!sfile) {
    fprintf(stderr,"Cannot open scrip file %s\n", scripflnm);
    return NCO_ERR;
  }
  int icnt, idx=0;
  while (fgets(line, sizeof(line), sfile)){
    if(strstr(line, "=") == NULL){
      fprintf(stderr,"invalid line in scrip file: %s\n", line);
      fclose(sfile);
      return NCO_ERR;
    }
    smps[idx]=nco_sng2map(line, smps[idx]);
    if(smps[idx].key == NULL){
      fclose(sfile);
      return NCO_ERR;
    }
    else{
      idx++;
    }
  } /* finish parsing SCRIP file */
  fclose(sfile);
  for(icnt=0; icnt<idx; icnt++){
    nco_kvmap_prn(smps[icnt]);
  }
  return NCO_NOERR;
} /* end hdlscrip */
