/* $Header$ */

/* Purpose: NCO utilities for compression filters */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   ncks -O -4 -D 1 --flt='*,32004,1' ~/nco/data/in.nc ~/foo.nc
   ncks -O -4 -D 1 --flt='32004,1,6,-4,2.5f,-7.3d' ~/nco/data/in.nc ~/foo.nc
   ncks -C -m --hdn --cdl ~/foo.nc

   Unidata _Filter docs:
   https://www.unidata.ucar.edu/software/netcdf/docs/md__Users_wfisher_Desktop_docs_netcdf-c_docs_filters.html
   Use NC_parsefilterspec(), NC_filterfix8() demonstrated in nc_test4/tst_filterparser.c, to parse options */

#include "nco_flt.h" /* Compression filters */

void
nco_flt_prs /* [fnc] Parse user-provided filter string */
(char * const flt_sng) /* I [sng] Filter string */
{
  /* Purpose: Parse user-provided filter string */
  const char fnc_nm[]="nco_flt_prs()";

  char **prm_lst; /* [sng] List of user-supplied filter parameters as strings */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  size_t prm_idx; /* [idx] Parameter index in user-supplied list */
  size_t prm_nbr=0L; /* [nbr] Number of parameters in user-supplied list */

  size_t u32_idx; /* [idx] Parameter index in u32 format list */
  size_t u32_nbr=0L; /* [nbr] Number of array elements required to pass list in u32 format */

  unsigned int flt_id=0; /* [id] Filter ID */
  unsigned int *u32_lst=NULL; /* [sct] List of filter parameters */
  unsigned int *ui32p; /* [ptr] Pointer to numeric parameter */

  if(!flt_sng){
    (void)fprintf(stderr,"%s: ERROR %s reports supplied filter string is empty\n",nco_prg_nm_get(),flt_sng);
    nco_exit(EXIT_FAILURE);
  } /* !flt_sng */

  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Requested filter string = %s\n",nco_prg_nm_get(),flt_sng);

  prm_lst=nco_lst_prs_1D(flt_sng,",",(int *)&prm_nbr);
  flt_id=strtoul(prm_lst[0],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
  if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[0],"strtoul",sng_cnv_rcd);

  /* Decrement so prm_nbr counts filter parameters not including filter ID */
  prm_nbr--;

  /* Allocate array space for worst case scenario, i.e., every parameter requires 64 bits */
  u32_lst=(unsigned int *)nco_malloc(sizeof(unsigned int)*2*prm_nbr);
  
  /* Use netCDF-provided helper function to parse user-provided filter request 
     EXTERNL int NC_parsefilterspec(const char* txt, int format, NC_Filterspec** specp);
     int NC_parsefilterspec(const char* spec, unsigned int* idp, size_t* nparamsp, unsigned int** paramsp);
     idp will contain the filter id value from the spec.
     nparamsp will contain the number of 4-byte parameters
     paramsp will contain a pointer to the parsed parameters – the caller must free. This function can parse filter spec strings as defined in the section on Filter Specification Syntax. This function parses the first argument and returns several values. */
  // Apparently four-argument NC_parsefilterspec() documented but not supported in 4.7.4
  //NC_parsefilterspec(flt_sng,&flt_id,&prm_nbr,&prm_lst);
  // Old NC_parsefilterspec() supported in 4.7.4 passes structures so, just forget it
  //NC_parsefilterspec(flt_sng,int format, NC_Filterspec** specp);

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s reports parsed filter ID = %u, prm_nbr = %lu: ",nco_prg_nm_get(),fnc_nm,flt_id,prm_nbr);
    for(prm_idx=1;prm_idx<=prm_nbr;prm_idx++) (void)fprintf(stdout,"%s%s",prm_lst[prm_idx],prm_idx == prm_nbr ? "" : ", ");
    (void)fprintf(stdout,"\n");
  } /* !dbg */

  /* Initialize u32_nbr before incrementing */
  u32_nbr=0L;
  char *sfx_lcn=NULL; /* [sng] Location of character type suffix, if any */
  for(prm_idx=1;prm_idx<=prm_nbr;prm_idx++){ 
    if((sfx_lcn=strchr(prm_lst[prm_idx],'d')) || (sfx_lcn=strchr(prm_lst[prm_idx],'D'))){
      *sfx_lcn='\0';
      double dbl_foo=strtod(prm_lst[prm_idx],&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtod",sng_cnv_rcd);
      ui32p=(unsigned int *)&dbl_foo;
      u32_lst[u32_nbr++]=*ui32p;
      u32_lst[u32_nbr++]=*(ui32p+1);
      /* !double */
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'f')) || (sfx_lcn=strchr(prm_lst[prm_idx],'F'))){
      *sfx_lcn='\0';
      float flt_foo=(float)strtod(prm_lst[prm_idx],&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtod",sng_cnv_rcd);
      ui32p=(unsigned int *)&flt_foo;
      u32_lst[u32_nbr++]=*ui32p;
      /* !float */
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'u')) || (sfx_lcn=strchr(prm_lst[prm_idx],'U'))){
      *sfx_lcn='\0';
      if((sfx_lcn=strchr(prm_lst[prm_idx],'s')) || (sfx_lcn=strchr(prm_lst[prm_idx],'S'))){
	*sfx_lcn='\0';
	unsigned short usht_foo=(unsigned short)strtoul(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoul",sng_cnv_rcd);
	ui32p=(unsigned int *)&usht_foo;
	u32_lst[u32_nbr++]=*ui32p;
	/* !unsigned short */
      }else if((sfx_lcn=strchr(prm_lst[prm_idx],'l')) || (sfx_lcn=strchr(prm_lst[prm_idx],'L'))){
	*sfx_lcn='\0';
	unsigned long long ulng_lng_foo=(unsigned long long)strtoull(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoull",sng_cnv_rcd);
	ui32p=(unsigned int *)&ulng_lng_foo;
	u32_lst[u32_nbr++]=*ui32p;
	u32_lst[u32_nbr++]=*(ui32p+1);
	/* !unsigned long long */
      }else{
	/* unsigned int */
	*sfx_lcn='\0';
	unsigned int uint_foo=(unsigned int)strtoul(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoul",sng_cnv_rcd);
	ui32p=(unsigned int *)&uint_foo;
	u32_lst[u32_nbr++]=*ui32p;
	/* !unsigned int */
      } /* !unsigned */
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'s')) || (sfx_lcn=strchr(prm_lst[prm_idx],'S'))){
      *sfx_lcn='\0';
      short sht_foo=(short)strtol(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtol",sng_cnv_rcd);
      ui32p=(unsigned int *)&sht_foo;
      u32_lst[u32_nbr++]=*ui32p;
      /* !short */
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'l')) || (sfx_lcn=strchr(prm_lst[prm_idx],'L'))){
      *sfx_lcn='\0';
      long long lng_lng_foo=(long long)strtoll(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoll",sng_cnv_rcd);
      ui32p=(unsigned int *)&lng_lng_foo;
      u32_lst[u32_nbr++]=*ui32p;
      u32_lst[u32_nbr++]=*(ui32p+1);
      /* !long long */
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'-'))){
      int int_foo=(int)strtol(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtol",sng_cnv_rcd);
      ui32p=(unsigned int *)&int_foo;
      u32_lst[u32_nbr++]=*ui32p;
      /* !int */
    }else{
      unsigned int uint_foo=(unsigned int)strtoul(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoul",sng_cnv_rcd);
      ui32p=(unsigned int *)&uint_foo;
      u32_lst[u32_nbr++]=*ui32p;
      /* !unsigned int */
    } /* !type */
    
  } /* !prm_idx */

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s reports parsed filter ID = %u, u32_nbr = %lu: ",nco_prg_nm_get(),fnc_nm,flt_id,u32_nbr);
    for(u32_idx=0;u32_idx<u32_nbr;u32_idx++) (void)fprintf(stdout,"%u%s",u32_lst[u32_idx],u32_idx == u32_nbr-1 ? "u" : "u, ");
    (void)fprintf(stdout,"\n");
  } /* !dbg */

  nco_exit(EXIT_SUCCESS);

} /* !nco_flt_prs() */

const char * /* O [sng] String describing compression filter */
nco_flt_sng_get /* [fnc] Convert compression filter enum to string */
(const nco_flt_typ_enm nco_flt_typ) /* I [enm] Compression filter type */
{
  /* Purpose: Convert compression grid-type enum to descriptive string */
  switch(nco_flt_typ){
  case nco_flt_nil: return "Filter type is unset"; break;
  case nco_flt_dfl: return "DEFLATE"; break;
  case nco_flt_bzp: return "Bzip2"; break;
  case nco_flt_lz4: return "LZ4"; break;
  case nco_flt_bgr: return "Bit Grooming"; break;
  case nco_flt_dgr: return "Digit Rounding"; break;
  case nco_flt_btr: return "Bit Rounding"; break;
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  return (char *)NULL;
  
} /* !nco_flt_sng_get() */
