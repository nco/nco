/* $Header$ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   ncks -4 -O -C -v ppc_dbl --ppc /ppc_dbl=3 ~/nco/data/in.nc ~/foo.nc */

#include "nco_sld.h" /* Swath-Like Data */

int /* O [rcd] Return code */
nco_scrip_read /* [fnc] Read, parse, and print contents of SCRIP file */
(char *fl_scrip, /* SCRIP file name with proper path */
 kvm_sct *kvm_scrip)/* structure to hold contents of SCRIP file */ 
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
    kvm_scrip[idx]=nco_sng2kvm(sng_line);
    if(!kvm_scrip[idx].key){
      fclose(fp_scrip);
      return NCO_ERR;
    }else{
      idx++;
    } /* end else */
  } /* finish parsing SCRIP file */
  fclose(fp_scrip);
  if(sng_line) sng_line=(char *)nco_free(sng_line);

  for(icnt=0;icnt<idx;icnt++) nco_kvm_prn(kvm_scrip[icnt]);

  return NCO_NOERR;
} /* end nco_scrip_read */
