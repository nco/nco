/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sld.c,v 1.1 2015-01-20 21:57:13 zender Exp $ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   ncks -4 -O -C -v lsd_dbl --lsd /lsd_dbl=3 ~/nco/data/in.nc ~/foo.nc */

#include "nco_sld.h" /* Swath-Like Data */

// nco_sng2map
kvmap sng2map(char *str, kvmap sm){
  int icnt=0;
  char * prt;
  prt=strtok(str, "=");
  while(prt != NULL){
    icnt++;
    strip(prt);
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
}/* end sng2map */

// nco_sng_strip
char * strip(char *str) /* strip off heading and tailing white spaces.  seems not working for \n??? */
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
}/* end strip */

int sng2array(const char *delim, const char *str, char **sarray)
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
}/* end sng2array */

// nco_kvmap_free
void freekvmaps(kvmap *kvmaps)
{
  int idx=0;
  while(kvmaps[idx].key){
    kvmaps[idx].key=nco_free(kvmaps[idx].key);
    kvmaps[idx].value=nco_free(kvmaps[idx].value);
  }
  kvmaps=nco_free(kvmaps);
}

// nco_kvmap_prn
void prtkvmap (kvmap vm)
{
  if(vm.key == NULL) return;
  // (void)fprintf(stdout,...)
  printf("%s=", vm.key);
  printf("%s\n", vm.value);
}

int 
hdlscrip( /* return 0 invalid scrip file or rcd, 1 success */ 
char *scripflnm, /* scrip file name with proper path */
kvmap *smps)/* structure to hold contents of scrip file */ 
{
  char line[BUFSIZ];
  FILE *sfile=fopen(scripflnm, "r");
  if (!sfile) {
    printf("Cannot open scrip file %s\n", scripflnm);
    // return NCO_ERR;
    return 0;
  }
  int icnt, idx=0;
  while (fgets(line, sizeof(line), sfile)){
    if(strstr(line, "=") == NULL){
      printf("invalid line in scrip file: %s\n", line);
      fclose(sfile);
      return 0;
    }
    smps[idx]=sng2map(line, smps[idx]);
    if(smps[idx].key == NULL){
      fclose(sfile);
      return 0;
    }
    else{
      idx++;
    }
  } /* finish parsing SCRIP file */
  fclose(sfile);
  for(icnt=0; icnt<idx; icnt++){
    prtkvmap(smps[icnt]);
  }
  // NCO_NOERR
  return 1;
}/* end hdlscrip */
