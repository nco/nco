/* $Header$ */

/* Purpose: NCO utilities for compression filters */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   ncks -O -4 -D 1 --flt='*,32004,1' ~/nco/data/in.nc ~/foo.nc
   ncks -O -4 -D 1 --flt='32004,1U,1,-1,1.0f,-1.0d,1UL,-1L,1US,-1S' ~/nco/data/in.nc # Test all input types
   ncks -C -m --hdn --cdl ~/foo.nc

   Unidata _Filter docs:
   https://www.unidata.ucar.edu/software/netcdf/docs/md__Users_wfisher_Desktop_docs_netcdf-c_docs_filters.html
   Use NC_parsefilterspec(), NC_filterfix8() demonstrated in nc_test4/tst_filterparser.c, to parse options */

#include "nco_flt.h" /* Compression filters */

/* Filter variables are file scope for now, could shift to global scope later if necessary
   20220609: fxm free() these two global variables */
static char *nco_cdc_lst_glb=NULL; /* [sng] List of available filters */
static char *nco_cmp_sng_glb=NULL; /* [sng] Global compression specification in NCO-standard format */

void
nco_dfl_case_flt_enm_err /* [fnc] Print error and exit for illegal switch(nco_flt_enm) case */
(nco_flt_typ_enm nco_flt_enm, /* [enm] Unrecognized enum */
 char *fnc_err) /* [sng] Function where error occurred */
{
  /* Purpose: Print error and exit when switch statement reaches illegal default case
     Routine reduces bloat because many switch() statements invoke this functionality */
  const char fnc_nm[]="nco_dfl_case_flt_enm_err()";
  (void)fprintf(stdout,"%s: ERROR nco_flt_enm=%d is unrecognized in switch(nco_flt_enm) statement in function %s. This specific error handler ensures all switch(nco_flt_enm) statements are fully enumerated. Exiting...\n",fnc_nm,(int)nco_flt_enm,fnc_err);
  nco_err_exit(0,fnc_nm);
} /* !nco_dfl_case_flt_enm_err() */

int /* O [enm] Return code */
nco_cmp_prs /* [fnc] Parse user-provided compression specification */
(char * const cmp_sng, /* I/O [sng] Compression specification */
 int * const dfl_lvlp, /* I [enm] Deflate level [0..9] */
 int *flt_glb_nbr, /* [nbr] Number of codecs specified */
 nco_flt_typ_enm **flt_glb_alg, /* [nbr] List of filters specified */
 int **flt_glb_lvl, /* [nbr] List of compression levels for each filter */
 int **flt_glb_prm_nbr, /* [nbr] List of parameter numbers for each filter */
 int ***flt_glb_prm) /* [nbr] List of lists of parameters for each filter */
{
  /* Purpose: Parse and set global lossy/lossless compression settings

     Algorithm: 
     NCO accepts compression strings as comma-separated lists of couplets
     Couplet first value identifies compression algorithm, e.g., "Zstd","Deflate","BitGroom"
     Couplet second value, if present, is integer compression level
     Specify codecs in the desired order of their application

     Test:
     ncks --dbg=2 --cdc=bgr,3|zstd,1 in.nc out.nc */

  const char fnc_nm[]="nco_cmp_prs()";

  char **prm_lst; /* [sng] List of user-supplied filter parameters as strings */
  char **flt_lst; /* [sng] List of user-supplied filters as pipe-separated lists of comma-separated strings */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char spr_sng[]="|"; /* [sng] Separator string between information for different filters */

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level [0..9] */
  int rcd=NCO_NOERR; /* [rcd] Return code */

  int prm_idx; /* [idx] Parameter index */
  int flt_idx; /* [idx] Filter index */
  int prm_nbr=0L; /* [nbr] Number of parameters */

  /* Varibles to copy back to calling routine if requested */
  int flt_nbr=0; /* [nbr] Number of codecs specified */
  nco_flt_typ_enm *flt_alg=NULL; /* [nbr] List of filters specified */
  int *flt_lvl=NULL; /* [nbr] List of compression levels for each filter */
  int *flt_prm_nbr=NULL; /* [nbr] List of parameter numbers for each filter */
  int **flt_prm=NULL; /* [nbr] List of lists of parameters for each filter */

  if(dfl_lvlp) dfl_lvl=*dfl_lvlp;

  if(cmp_sng){

    if(nco_dbg_lvl_get() >= nco_dbg_std && !nco_cmp_sng_glb) (void)fprintf(stderr,"%s: INFO %s reports requested codec string = %s\n",nco_prg_nm_get(),fnc_nm,cmp_sng);

    flt_lst=nco_lst_prs_1D(cmp_sng,spr_sng,&flt_nbr);

    flt_alg=(nco_flt_typ_enm *)nco_malloc(flt_nbr*sizeof(nco_flt_typ_enm));
    flt_lvl=(int *)nco_malloc(flt_nbr*sizeof(int));
    flt_prm_nbr=(int *)nco_malloc(flt_nbr*sizeof(int));
    flt_prm=(int **)nco_malloc(flt_nbr*sizeof(int *));
      
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s reports codec string contains flt_nbr=%d codecs separated by \"%s\"\n",nco_prg_nm_get(),fnc_nm,flt_nbr,spr_sng);

    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
      
      prm_lst=nco_lst_prs_1D(flt_lst[flt_idx],",",&prm_nbr);

      /* First element in list is always name/ID */
      //(void)fprintf(stdout,"DEBUG quark3 prm_nbr=%d, prm_lst=%s\n",(int)prm_nbr,prm_lst[0]);
      flt_alg[flt_idx]=nco_flt_sng2enm(prm_lst[0]);

      /* Remaining elements, if any, are filter parameters in the HDF5 sense */
      flt_prm_nbr[flt_idx]=prm_nbr-1;
      int prm_nbr_min; /* [nbr] Minimum number of parameters to allocate space for */
      prm_nbr_min=(flt_prm_nbr[flt_idx] > 1) ? flt_prm_nbr[flt_idx] : 1;
      flt_prm[flt_idx]=(int *)nco_malloc(prm_nbr_min*sizeof(int));

      for(prm_idx=1;prm_idx<prm_nbr;prm_idx++){ /* NB: prm_nbr is one greater than # of HDF5 parameters */
	if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG flt_idx=%d prm_nbr=%d prm_idx=%d prm_val=%s\n",nco_prg_nm_get(),flt_idx,prm_nbr,prm_idx,prm_lst[prm_idx]);
	flt_prm[flt_idx][prm_idx-1]=(int)strtol(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtol",sng_cnv_rcd);
      } /* !prm_idx */
      /* Allocate room for at least one parameter so default values can be imposed */
      if(flt_prm_nbr[flt_idx] == 0) flt_prm[flt_idx][0]=NC_MIN_INT;
      flt_lvl[flt_idx]=flt_prm[flt_idx][0];
    } /* !flt_idx */
  } /* !cmp_sng */

  /* Allow use of traditional --dfl_lvl to set DEFLATE filter in global string */
  if(flt_nbr == 0 && dfl_lvl != NCO_DFL_LVL_UNDEFINED){
    flt_nbr=1;
    flt_alg=(nco_flt_typ_enm *)nco_malloc(flt_nbr*sizeof(nco_flt_typ_enm));
    flt_lvl=(int *)nco_malloc(flt_nbr*sizeof(int));
    flt_prm_nbr=(int *)nco_malloc(flt_nbr*sizeof(int));
    flt_prm=(int **)nco_malloc(flt_nbr*sizeof(int *));

    flt_alg[0]=nco_flt_dfl;
    flt_prm_nbr[0]=1;
    flt_prm[0]=(int *)nco_malloc(flt_prm_nbr[0]*sizeof(int));
    flt_prm[0][0]=dfl_lvl;
    flt_lvl[0]=flt_prm[0][0];
  } /* !flt_nbr, !dfl_lvl */
    
  /* Allow user to rely on default filter parameters
     For example, --cdc=zst means use Zstandard with compression level 3 */
  if(flt_nbr == 1 && flt_alg[0] == nco_flt_zst && flt_prm_nbr[0] == 0 && flt_prm[0][0] == NC_MIN_INT){
    flt_prm_nbr[0]=1;
    flt_prm[0][0]=3;
    flt_lvl[0]=flt_prm[0][0];
  } /* !flt_nbr, !dfl_lvl */
  /* -L dfl_lvl with unspecified codec causes NCO to use default DEFLATE compression level dfl_lvl */
  if(flt_nbr == 1 && flt_alg[0] == nco_flt_dfl && flt_prm_nbr[0] == 0 && flt_prm[0][0] == NC_MIN_INT){
    flt_prm_nbr[0]=1;
    flt_prm[0][0]=1;
    flt_lvl[0]=flt_prm[0][0];
  } /* !flt_nbr, !dfl_lvl */
  /* --cdc=dfl with unspecified compression level parameter means causes NCO to use default DEFLATE compression level 1 */
  if(flt_nbr == 1 && flt_alg[0] == nco_flt_dfl && flt_prm_nbr[0] == 0 && flt_prm[0][0] == NC_MIN_INT){
    flt_prm_nbr[0]=1;
    flt_prm[0][0]=1;
    flt_lvl[0]=flt_prm[0][0];
  } /* !flt_nbr, !dfl_lvl */

  /* Allow user to mix --dfl_lvl and --cdc
     For example, --dfl_lvl=3 --cdc=zst means use Zstandard with compression level 3 */
  if(flt_nbr == 1 && flt_prm_nbr[0] == 0 && dfl_lvl != NCO_DFL_LVL_UNDEFINED){
    flt_prm_nbr[0]=1;
    flt_prm[0][0]=dfl_lvl;
    flt_lvl[0]=flt_prm[0][0];
  } /* !flt_nbr, !dfl_lvl */

  /* Global user-specified specification has been merged with dfl_lvl 
     Generate final global compression specification in NCO standard format */
  char *cmp_sng_std=NULL; /* [sng] Compression specification in NCO-standard format */
  char int_sng[100]; /* [sng] Buffer to hold printed integers */
  cmp_sng_std=(char *)nco_malloc(100*sizeof(char)); /* [sng] Compression specification in NCO-standard format */
  /* NUL-terminate string */
  cmp_sng_std[0]='\0';
  if(flt_nbr > 0){
    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
      (void)strcat(cmp_sng_std,nco_flt_enm2sng(flt_alg[flt_idx]));
      if(flt_prm_nbr[flt_idx] > 0) (void)strcat(cmp_sng_std,",");
      int_sng[0]='\0';
      for(prm_idx=0;prm_idx<flt_prm_nbr[flt_idx];prm_idx++){
	(void)sprintf(int_sng,"%d%s",flt_prm[flt_idx][prm_idx], prm_idx < flt_prm_nbr[flt_idx]-1 ? "," : "");
      } /* !prm_idx */
      (void)strcat(cmp_sng_std,int_sng);
      if(flt_idx < flt_nbr-1) (void)strcat(cmp_sng_std,spr_sng);
    } /* !flt_idx */
  } /* !flt_nbr */

  /* If global user-requested compression specification string has not yet been saved, copy and save it */
  if(!nco_cmp_sng_glb) nco_cmp_sng_glb=(char *)strdup(cmp_sng_std);

  /* Variables to copy back to calling routine (or global variable) if requested */
  if(flt_glb_nbr) *flt_glb_nbr=flt_nbr;
  if(flt_glb_alg) *flt_glb_alg=flt_alg; else flt_alg=(nco_flt_typ_enm *)nco_free(flt_alg);
  if(flt_glb_lvl) *flt_glb_lvl=flt_lvl; else flt_lvl=(int *)nco_free(flt_lvl);
  if(flt_glb_prm_nbr) *flt_glb_prm_nbr=flt_prm_nbr; else flt_prm_nbr=(int *)nco_free(flt_prm_nbr);
  if(flt_glb_prm){
    *flt_glb_prm=flt_prm;
  }else{
    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++) flt_prm[flt_idx]=(int *)nco_free(flt_prm[flt_idx]);
    flt_prm=(int **)nco_free(flt_prm);
  } /* !flt_idx */

  /* Build list of available filters once */
  if(!nco_cdc_lst_glb){
    nco_cdc_lst_glb=(char *)nco_malloc(200*sizeof(char));
    nco_cdc_lst_glb[0]='\0';
    strcat(nco_cdc_lst_glb,"DEFLATE");
    /* Tokens like CCR_HAS_BITGROOM are defined by CCR in ccr_meta.h */
# if CCR_HAS_BZIP2 || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", Bzip2");
# endif /* !CCR_HAS_BZIP2 */
# if CCR_HAS_LZ4
    strcat(nco_cdc_lst_glb,", LZ4");
# endif /* !CCR_HAS_LZ4 */
# if CCR_HAS_BITGROOM || NC_LIB_VER >= 490 
    strcat(nco_cdc_lst_glb,", BitGroom");
# endif /* !CCR_HAS_BITGROOM */
# if CCR_HAS_BITROUND || NC_LIB_VER >= 490 
    strcat(nco_cdc_lst_glb,", BitRound");
# endif /* !CCR_HAS_BITROUND */
# if CCR_HAS_GRANULARBR || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", GranularBR");
# endif /* !CCR_HAS_GRANULARBR */
# if CCR_HAS_ZSTD || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", Zstandard");
# endif /* !CCR_HAS_ZSTD */
# if CCR_HAS_BLOSC_LZ || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", BLOSC_LZ");
# endif /* !CCR_HAS_BLOSC_LZ */
# if CCR_HAS_BLOSC_LZ4 || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", BLOSC LZ4");
# endif /* !CCR_HAS_BLOSC_LZ4 */
# if CCR_HAS_BLOSC_LZ4_HC || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", BLOSC LZ4 HC");
# endif /* !CCR_HAS_BLOSC_LZ4_HC */
# if CCR_HAS_SNAPPY || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", Snappy");
# endif /* !CCR_HAS_SNAPPY */
# if CCR_HAS_BLOSC_DEFLATE || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", BLOSC DEFLATE");
# endif /* !CCR_HAS_BLOSC_DEFLATE */
# if CCR_HAS_BLOSC_ZSTANDARD || NC_LIB_VER >= 490
    strcat(nco_cdc_lst_glb,", BLOSC Zstandard");
# endif /* !CCR_HAS_BLOSC_ZSTANDARD */
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s reports available codec list is nco_cdc_lst_glb=%s\n",nco_prg_nm_get(),fnc_nm,nco_cdc_lst_glb);
  } /* !nco_cdc_lst_glb */
  
  if(nco_dbg_lvl_get() >= nco_dbg_grp){
    (void)fprintf(stdout,"%s: DEBUG cmp_sng_std = %s\n",nco_prg_nm_get(),cmp_sng_std);
    (void)fprintf(stdout,"%s: DEBUG exiting %s\n",nco_prg_nm_get(),fnc_nm);
  } /* !dbg */
  
  return rcd;
  
} /* !nco_cmp_prs() */

void
nco_flt_hdf5_prs /* [fnc] Parse user-legible string into HDF5 filter parameter list */
(char * const flt_sng) /* I [sng] Filter string */
{
  /* Purpose: 
     HDF5 filters require specifications in (bizarre looking) uint32 values
     Function prints/returns input list parsed into HDF5-required list of u32 arguments
     Example filter strings: "32022,3", "32004,1U,1,-1,1.0f,-1.0d,1UL,-1L,1US,-1S" 

     Test:
     ncks --dbg=2 --filter="32022,3" */

  const char fnc_nm[]="nco_flt_hdf5_prs()";

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

  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s reports requested filter string = %s\n",nco_prg_nm_get(),fnc_nm,flt_sng);

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
  char *sfx_lcn=NULL; /* [sng] Location of type-suffix first character, if any */
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
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'s')) || (sfx_lcn=strchr(prm_lst[prm_idx],'S'))){
      *sfx_lcn='\0';
      if((sfx_lcn=strchr(prm_lst[prm_idx],'u')) || (sfx_lcn=strchr(prm_lst[prm_idx],'U'))){
	*sfx_lcn='\0';
	unsigned short usht_foo=(unsigned short)strtoul(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoul",sng_cnv_rcd);
	ui32p=(unsigned int *)&usht_foo;
	u32_lst[u32_nbr++]=*ui32p;
	/* !unsigned short */
      }else{ 
	short sht_foo=(short)strtol(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtol",sng_cnv_rcd);
	ui32p=(unsigned int *)&sht_foo;
	u32_lst[u32_nbr++]=*ui32p;
	/* !short */
      } /* !sfx_lcn */
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'l')) || (sfx_lcn=strchr(prm_lst[prm_idx],'L'))){
      *sfx_lcn='\0';
      if((sfx_lcn=strchr(prm_lst[prm_idx],'u')) || (sfx_lcn=strchr(prm_lst[prm_idx],'U'))){
	*sfx_lcn='\0';
	unsigned long long ulng_lng_foo=(unsigned long long)strtoull(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoull",sng_cnv_rcd);
	ui32p=(unsigned int *)&ulng_lng_foo;
	u32_lst[u32_nbr++]=*ui32p;
	u32_lst[u32_nbr++]=*(ui32p+1);
	/* !unsigned long long */
      }else{
	long long lng_lng_foo=(long long)strtoll(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoll",sng_cnv_rcd);
	ui32p=(unsigned int *)&lng_lng_foo;
	u32_lst[u32_nbr++]=*ui32p;
	u32_lst[u32_nbr++]=*(ui32p+1);
	/* !long long */
      } /* !sfx_lcn */
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'u')) || (sfx_lcn=strchr(prm_lst[prm_idx],'U'))){
      /* Explicit unsigned int */
      *sfx_lcn='\0';
      unsigned int uint_foo=(unsigned int)strtoul(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtoul",sng_cnv_rcd);
      ui32p=(unsigned int *)&uint_foo;
      u32_lst[u32_nbr++]=*ui32p;
      /* !unsigned int */
    }else if((sfx_lcn=strchr(prm_lst[prm_idx],'-'))){
      /* Implicit signed int */
      int int_foo=(int)strtol(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtol",sng_cnv_rcd);
      ui32p=(unsigned int *)&int_foo;
      u32_lst[u32_nbr++]=*ui32p;
      /* !int */
    }else{
      /* Implicit unsigned int */
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

} /* !nco_flt_hdf5_prs() */

char * /* O [sng] String describing compression filter */
nco_flt_enm2sng /* [fnc] Convert compression filter enum to string */
(const nco_flt_typ_enm nco_flt_enm) /* I [enm] Compression filter type */
{
  /* Purpose: Convert compression filter-type enum to descriptive string */
  switch(nco_flt_enm){
  case nco_flt_nil: return "Filter type is unset"; break;
  case nco_flt_dfl: return "DEFLATE"; break;
  case nco_flt_bz2: return "Bzip2"; break;
  case nco_flt_lz4: return "LZ4"; break;
  case nco_flt_bgr: return "BitGroom"; break;
  case nco_flt_gbr: return "GranularBR"; break;
  case nco_flt_dgr: return "DigitRound"; break;
  case nco_flt_btr: return "BitRound"; break;
  case nco_flt_zst: return "Zstandard"; break;
  case nco_flt_bls: return "BLOSC (unspecified)"; break;
  case nco_flt_bls_lz: return "BLOSC LZ"; break;
  case nco_flt_bls_lz4: return "BLOSC LZ4"; break;
  case nco_flt_bls_lzh: return "BLOSC LZ4 HC"; break;
  case nco_flt_bls_snp: return "BLOSC Snappy"; break;
  case nco_flt_bls_dfl: return "BLOSC DEFLATE"; break;
  case nco_flt_bls_zst: return "BLOSC Zstandard"; break;
  default:
    nco_dfl_case_flt_enm_err(nco_flt_enm,"nco_flt_enm2sng()"); break;
  } /* !nco_flt_enm */

  return (char *)NULL;
  
} /* !nco_flt_enm2sng() */

char * /* O [sng] String describing compression filter */
nco_flt_id2sng /* [fnc] Convert compression filter HDF5 ID to string */
(const unsigned int nco_flt_id) /* I [id] Compression filter HDF5 ID */
{
  /* Purpose: Convert compression filter HDF5 ID to descriptive string */
  switch(nco_flt_id){
  case nco_flt_nil: return "Filter type is unset"; break;
  case 1 : return "DEFLATE"; break;
  case 307 : return "Bzip2"; break;
  case 32004 : return "LZ4"; break;
  case 32022 : return "BitGroom"; break;
  case 32023 : return "Granular BitRound"; break;
    //  case : return "DigitRound"; break;
  case 37373: return "BitRound"; break;
  case 32015 : return "Zstandard"; break;
  case 32001 : return "BLOSC"; break;
  default:
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG Filter ID = %u is unknown. Default case reached in nco_flt_id2sng()\n",nco_prg_nm_get(),nco_flt_id);
    nco_dfl_case_generic_err(); break;
  } /* !nco_flt_id */

  return (char *)NULL;
  
} /* !nco_flt_id2sng() */

nco_flt_typ_enm /* O [enm] Filter enum */
nco_flt_id2enm /* [fnc] Convert compression filter ID to enum */
(const unsigned int nco_flt_id) /* I [id] Compression filter ID */
{
  /* Purpose: Convert compression filter ID to descriptive string */
  switch(nco_flt_id){
  case 0: return nco_flt_nil; break;
  case 1 : return nco_flt_dfl; break;
  case 307 : return nco_flt_bz2; break;
  case 32004 : return nco_flt_lz4; break;
  case 32022 : return nco_flt_bgr; break;
  case 32023 : return nco_flt_gbr; break;
    //  case : return nco_flt_dgr; break;
  case 37373: return nco_flt_btr; break;
  case 32015 : return nco_flt_zst; break;
  case 32001 : return nco_flt_bls; break;
  default:
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG Filter ID = %u is unknown by NCO. Default case reached in nco_flt_id2enm()\n",nco_prg_nm_get(),nco_flt_id);
    nco_dfl_case_generic_err(); break;
  } /* !nco_flt_id */

  return nco_flt_nil;
  
} /* !nco_flt_id2enm() */

nco_flt_typ_enm /* O [enm] Filter enum */
nco_flt_sng2enm /* [fnc] Convert user-specified filter string to NCO enum */
(const char *nco_flt_sng) /* [sng] User-specified filter string */
{
  /* Purpose: Convert user-specified string to enumerated filter type
     Return nco_flt_nil by default */

  const char fnc_nm[]="nco_flt_sng2enm()"; /* [sng] Function name */

  char *nco_prg_nm; /* [sng] Program name */

  nco_prg_nm=nco_prg_nm_get(); /* [sng] Program name */

  if(nco_flt_sng == NULL){
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s reports %s invoked without explicit filter string. Defaulting to \"nil\".\n",nco_prg_nm,fnc_nm,nco_prg_nm);
    return nco_flt_nil;
  } /* !nco_flt_sng */

  /* Filter HDF5 IDs gleaned from nc_inq_var_filter_info() or copied from ncks/ncdump output will be unsigned ints
     Filter strings with an NCO-standardized key will be ... strings
     First check if filter appears to be specified by HDF5 ID */
  unsigned int flt_id; /* [ID] HDF5 Filter ID */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  flt_id=strtoul(nco_flt_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
  if(!*sng_cnv_rcd){
    nco_flt_typ_enm nco_flt_enm; /* [enm] Compression filter type */
    /* Filter appears to be specified by HDF5 ID */
    nco_flt_enm=nco_flt_id2enm(flt_id);
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s reports filter string %s interpreted as HDF5 ID for filter \"%s\" with NCO enum %d\n",nco_prg_nm,fnc_nm,nco_flt_sng,nco_flt_id2sng(flt_id),(int)nco_flt_enm);
    return nco_flt_enm;
  } /* !sng_cnv_rcd */

  if(!strcasecmp(nco_flt_sng,"nil")) return nco_flt_nil;
  if(!strcasecmp(nco_flt_sng,"none")) return nco_flt_nil;
  if(!strcasecmp(nco_flt_sng,"default")) return nco_flt_nil;
  if(!strcasecmp(nco_flt_sng,"deflate")) return nco_flt_dfl;
  if(!strcasecmp(nco_flt_sng,"dfl")) return nco_flt_dfl;
  if(!strcasecmp(nco_flt_sng,"gzp")) return nco_flt_dfl;
  if(!strcasecmp(nco_flt_sng,"gz")) return nco_flt_dfl;
  if(!strcasecmp(nco_flt_sng,"zlib")) return nco_flt_dfl;
  if(!strcasecmp(nco_flt_sng,"bz2")) return nco_flt_bz2;
  if(!strcasecmp(nco_flt_sng,"bzp")) return nco_flt_bz2;
  if(!strcasecmp(nco_flt_sng,"bz")) return nco_flt_bz2;
  if(!strcasecmp(nco_flt_sng,"bzip")) return nco_flt_bz2;
  if(!strcasecmp(nco_flt_sng,"bzip2")) return nco_flt_bz2;
  if(!strcasecmp(nco_flt_sng,"lz4")) return nco_flt_lz4;
  if(!strcasecmp(nco_flt_sng,"bgr")) return nco_flt_bgr;
  if(!strcasecmp(nco_flt_sng,"btg")) return nco_flt_bgr;
  if(!strcasecmp(nco_flt_sng,"bitgroom")) return nco_flt_bgr;
  if(!strcasecmp(nco_flt_sng,"Zen16")) return nco_flt_bgr;
  if(!strcasecmp(nco_flt_sng,"gbr")) return nco_flt_gbr;
  if(!strcasecmp(nco_flt_sng,"granularbr")) return nco_flt_gbr;
  if(!strcasecmp(nco_flt_sng,"granular")) return nco_flt_gbr;
  if(!strcasecmp(nco_flt_sng,"dgr")) return nco_flt_dgr;
  if(!strcasecmp(nco_flt_sng,"digitround")) return nco_flt_dgr;
  if(!strcasecmp(nco_flt_sng,"DCG19")) return nco_flt_dgr;
  if(!strcasecmp(nco_flt_sng,"btr")) return nco_flt_btr;
  if(!strcasecmp(nco_flt_sng,"bitround")) return nco_flt_btr;
  if(!strcasecmp(nco_flt_sng,"Kou20")) return nco_flt_btr;
  if(!strcasecmp(nco_flt_sng,"zst")) return nco_flt_zst;
  if(!strcasecmp(nco_flt_sng,"zstd")) return nco_flt_zst;
  if(!strcasecmp(nco_flt_sng,"zstandard")) return nco_flt_zst;

  if(!strcasecmp(nco_flt_sng,"blosc lz4 hc")) return nco_flt_bls_lzh;
  if(!strcasecmp(nco_flt_sng,"blosc_lz4_hc")) return nco_flt_bls_lzh;
  if(!strcasecmp(nco_flt_sng,"blosclz4hc")) return nco_flt_bls_lzh;
  if(!strcasecmp(nco_flt_sng,"bls_lzh")) return nco_flt_bls_lzh;
  if(!strcasecmp(nco_flt_sng,"bls_lz4hc")) return nco_flt_bls_lzh;
  if(!strcasecmp(nco_flt_sng,"blosc_lzh")) return nco_flt_bls_lzh;
  if(!strcasecmp(nco_flt_sng,"blosc_lz4hc")) return nco_flt_bls_lzh;

  if(!strcasecmp(nco_flt_sng,"blosc lz4")) return nco_flt_bls_lz4;
  if(!strcasecmp(nco_flt_sng,"blosc_lz4")) return nco_flt_bls_lz4;
  if(!strcasecmp(nco_flt_sng,"bls_lz4")) return nco_flt_bls_lz4;
  if(!strcasecmp(nco_flt_sng,"blslz4")) return nco_flt_bls_lz4;
  if(!strcasecmp(nco_flt_sng,"blosclz4")) return nco_flt_bls_lz4;

  if(!strcasecmp(nco_flt_sng,"blosc lz")) return nco_flt_bls_lz;
  if(!strcasecmp(nco_flt_sng,"blosc_lz")) return nco_flt_bls_lz;
  if(!strcasecmp(nco_flt_sng,"bls_lz")) return nco_flt_bls_lz;
  if(!strcasecmp(nco_flt_sng,"blslz")) return nco_flt_bls_lz;

  if(!strcasecmp(nco_flt_sng,"blosc snappy")) return nco_flt_bls_snp;
  if(!strcasecmp(nco_flt_sng,"bloscsnappy")) return nco_flt_bls_snp;
  if(!strcasecmp(nco_flt_sng,"blosc_snappy")) return nco_flt_bls_snp;
  if(!strcasecmp(nco_flt_sng,"bls_snp")) return nco_flt_bls_snp;
  if(!strcasecmp(nco_flt_sng,"blssnp")) return nco_flt_bls_snp;
  if(!strcasecmp(nco_flt_sng,"bls snp")) return nco_flt_bls_snp;

  if(!strcasecmp(nco_flt_sng,"blosc deflate")) return nco_flt_bls_dfl;
  if(!strcasecmp(nco_flt_sng,"bloscdeflate")) return nco_flt_bls_dfl;
  if(!strcasecmp(nco_flt_sng,"blosc_deflate")) return nco_flt_bls_dfl;
  if(!strcasecmp(nco_flt_sng,"bls_dfl")) return nco_flt_bls_dfl;
  if(!strcasecmp(nco_flt_sng,"blsdfl")) return nco_flt_bls_dfl;
  if(!strcasecmp(nco_flt_sng,"bls dfl")) return nco_flt_bls_dfl;

  if(!strcasecmp(nco_flt_sng,"blosc zstandard")) return nco_flt_bls_zst;
  if(!strcasecmp(nco_flt_sng,"blosczstandard")) return nco_flt_bls_zst;
  if(!strcasecmp(nco_flt_sng,"blosc_zstandard")) return nco_flt_bls_zst;
  if(!strcasecmp(nco_flt_sng,"bls_zst")) return nco_flt_bls_zst;
  if(!strcasecmp(nco_flt_sng,"blszst")) return nco_flt_bls_zst;
  if(!strcasecmp(nco_flt_sng,"bls zst")) return nco_flt_bls_zst;
  if(!strcasecmp(nco_flt_sng,"bls_zstd")) return nco_flt_bls_zst;
  if(!strcasecmp(nco_flt_sng,"blszstd")) return nco_flt_bls_zst;
  if(!strcasecmp(nco_flt_sng,"bls zstd")) return nco_flt_bls_zst;
  
  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified filter \"%s\"\n",nco_prg_nm_get(),fnc_nm,nco_flt_sng);
  nco_exit(EXIT_FAILURE);
  return nco_flt_nil; /* Statement should not be reached */
} /* !nco_flt_sng2enm() */

int /* O [enm] Return code */
nco_flt_def_wrp /* [fnc] Call filters immediately after variable definition */
(const int nc_in_id, /* I [id] netCDF input file/group ID */
 const int var_in_id, /* I [id] Variable ID */
 const char * const var_nm_in, /* I [nm] Variable name [optional] */
 const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Copy compression settings (if any) from same variable in input to output file

     Usage: 
     Until 20220501 the NCO code to define per-variable filters was scattered in ~four places 
     Introduction of new filters in netCDF 4.9.0 makes this untenable 
     Routine functionalizes invocation of original netCDF4 filters DEFLATE and Shuffle

     Algorithm:
     If var_nm_in is supplied (i.e., is not NULL) then determine var_in_id from nco_inq_varid_flg()
     If supplied (or determined from var_nm_in) var_in_id is valid then check input compression settings
     If supplied dfl_lvl is < 0 (i.e., unset) then copy input compression settings (if available)
     If supplied dfl_lvl is >= 0 (i.e., set) then set compression to dfl_lvl */

  //const char fnc_nm[]="nco_flt_def_wrp()"; /* [sng] Function name */

  nco_bool VARIABLE_EXISTS_IN_INPUT=False; /* [flg] Variable exists in input file */
  nco_bool COPY_COMPRESSION_FROM_INPUT=False; /* [flg] Copy compression setting from input to output */
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Deflation */
  int deflate; /* [flg] Turn-on deflate filter */
  int dfl_lvl_in; /* [enm] Deflate level [0..9] */
  int shuffle; /* [flg] Turn-on shuffle filter */
  int var_in_id_cpy=-1; /* [id] Writable copy of input variable ID */

  /* Use copy so var_in_id can remain const in prototype */
  var_in_id_cpy=var_in_id;

  /* Write (or overwrite) var_in_id when var_nm_in is supplied */
  if(var_nm_in && nc_in_id >= 0){
    /* Output variable may not exist in input file (e.g., when ncap2 defines new variable) */
    rcd=nco_inq_varid_flg(nc_in_id,var_nm_in,&var_in_id_cpy);
    if(rcd == NC_NOERR) VARIABLE_EXISTS_IN_INPUT=True;
  } /* !var_nm_in */

  if(nc_in_id >= 0 && var_in_id_cpy >= 0) VARIABLE_EXISTS_IN_INPUT=True;

  /* If variable exists in input file, copy its compression then return
     Otherwise call compression based solely on settings requested for output file */
  if(VARIABLE_EXISTS_IN_INPUT){

    /* fxm: Generalize to inquire about all compression options
       NB: dfl_lvl_in will be zero if deflate == 0 */
    rcd=nco_inq_var_deflate(nc_in_id,var_in_id_cpy,&shuffle,&deflate,&dfl_lvl_in);

    /* Copy original filters if user did not explicity set dfl_lvl for output */ 
    if((deflate || shuffle) && dfl_lvl < 0){
      /* Before netCDF 4.8.0, nco_def_var_deflate() could be called multiple times 
	 Properties of final invocation before nc_enddef() would take effect
	 After netCDF 4.8.0 first instance of nco_def_var_deflate() takes effect
	 It is therefore crucial not to call nco_def_var_deflate() more than once */
      rcd=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,dfl_lvl_in);
      if(rcd == NC_NOERR) COPY_COMPRESSION_FROM_INPUT=True;
    } /* !dfl_lvl */

  } /* !VARIABLE_EXISTS_IN_INPUT */

  /* Protect against calling nco_def_var_deflate() more than once */
  if(dfl_lvl != NCO_DFL_LVL_UNDEFINED && !COPY_COMPRESSION_FROM_INPUT)
    rcd=nco_flt_def_out(nc_out_id,var_out_id,dfl_lvl);

  return rcd;
  
} /* !nco_flt_def_wrp() */
  
int /* O [enm] Return code */
nco_flt_def_out /* [fnc]  */
(const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Set compression filters in newly defined variable in output file

     Usage: 
     Until 20220501 the NCO code to define per-variable filters was scattered in ~four places 
     Introduction of new filters in netCDF 4.9.0 makes this untenable 
     Here were functionalize the invocation of original netCDF4 filters DEFLATE and Shuffle

     Algorithm:
     If supplied dfl_lvl is >= 0 (i.e., set) then set compression to dfl_lvl */

  //  const char fnc_nm[]="nco_flt_def_out()"; /* [sng] Function name */

  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Deflation */
  int deflate; /* [flg] Turn-on deflate filter */
  int shuffle; /* [flg] Turn-on shuffle filter */

  if(dfl_lvl != NCO_DFL_LVL_UNDEFINED){
    /* Overwrite HDF Lempel-Ziv compression level, if requested */
    deflate=(int)True;
    /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
    if(dfl_lvl <= 0) shuffle=NC_NOSHUFFLE;
    /* Shuffle never, to my knowledge, increases filesize, so shuffle by default when manually deflating (and do not shuffle when uncompressing) */
    if(dfl_lvl > 0) shuffle=NC_SHUFFLE;
    rcd=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,dfl_lvl);
  } /* !dfl_lvl */

  return rcd;

} /* !nco_flt_def_out() */
  
int /* O [enm] Return code */
nco_tst_def_wrp /* [fnc] Call filters immediately after variable definition */
(const int nc_in_id, /* I [id] netCDF input file/group ID */
 const int var_in_id, /* I [id] Variable ID */
 const char * const var_nm_in, /* I [nm] Variable name [optional] */
 const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Copy compression settings (if any) from same variable in input to output file

     Usage: 
     Until 20220501 the NCO code to define per-variable filters was scattered in ~four places 
     Introduction of new filters in netCDF 4.9.0 makes this untenable 
     Routine functionalizes invocation of original netCDF4 filters DEFLATE and Shuffle

     Algorithm:
     If var_nm_in is supplied (i.e., is not NULL) then determine var_in_id from nco_inq_varid_flg()
     If supplied (or determined from var_nm_in) var_in_id is valid then check input compression settings
     If supplied dfl_lvl is < 0 (i.e., unset) then copy input compression settings (if available)
     If supplied dfl_lvl is >= 0 (i.e., set) then set compression to dfl_lvl */

  const char fnc_nm[]="nco_tst_def_wrp()"; /* [sng] Function name */

  nco_bool VARIABLE_EXISTS_IN_INPUT=False; /* [flg] Variable exists in input file */
  //nco_bool COPY_COMPRESSION_FROM_INPUT=False; /* [flg] Copy compression setting from input to output */
  char var_nm[NC_MAX_NAME+1L]; /* [sng] Variable name */
  char *flt_sng=NULL; /* [sng] Compression specification, if any, of variable as stored in input file */
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Deflation */
  int deflate; /* [flg] Turn-on deflate filter */
  int dfl_lvl_in; /* [enm] Deflate level [0..9] in input file */
  int dfl_lvl_cpy; /* [enm] Copy of requested deflate level, if any */
  int shuffle; /* [flg] Turn-on shuffle filter */
  int var_in_id_cpy=-1; /* [id] Writable copy of input variable ID */

  /* Use copies so var_in_id, dfl_lvl can remain const in prototype */
  var_in_id_cpy=var_in_id;
  dfl_lvl_cpy=dfl_lvl;

  /* Write (or overwrite) var_in_id when var_nm_in is supplied */
  if(var_nm_in && nc_in_id >= 0){
    /* Output variable may not exist in input file (e.g., when ncap2 defines new variable) */
    rcd=nco_inq_varid_flg(nc_in_id,var_nm_in,&var_in_id_cpy);
    if(rcd == NC_NOERR) VARIABLE_EXISTS_IN_INPUT=True;
  } /* !var_nm_in */

  if(nc_in_id >= 0 && var_in_id_cpy >= 0) VARIABLE_EXISTS_IN_INPUT=True;

  /* If variable exists in input file, copy its compression then return
     Otherwise call compression based solely on settings requested for output file */
  if(VARIABLE_EXISTS_IN_INPUT){

    /* NB: dfl_lvl_in will be zero if deflate == 0 */
    rcd=nco_inq_var_deflate(nc_in_id,var_in_id_cpy,&shuffle,&deflate,&dfl_lvl_in);

    char sng_foo[12]; /* nbr] Maximum printed size of unsigned integer (4294967295) + 1 (for comma) + 1 (for trailing NUL) */
    char spr_sng[]="|"; /* [sng] Separator string between information for different filters */

    size_t flt_idx; /* [idx] Filter index */
    size_t flt_nbr; /* [nbr] Filter number */
    size_t prm_idx; /* [idx] Parameter index */
    size_t prm_nbr; /* [nbr] Parameter number */
    unsigned int *prm_lst=NULL;
    unsigned int *flt_lst=NULL; /* [nbr] Filter IDs */

    rcd=nco_inq_var_filter_ids(nc_in_id,var_in_id_cpy,&flt_nbr,NULL);
    flt_lst=(unsigned int *)nco_malloc(flt_nbr*sizeof(unsigned int));
    if(flt_nbr > 0){
      rcd=nco_inq_var_filter_ids(nc_in_id,var_in_id_cpy,(size_t *)NULL,flt_lst);
      flt_sng=(char *)nco_malloc(100L*sizeof(char));
      for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
	rcd=nco_inq_var_filter_info(nc_in_id,var_in_id_cpy,flt_lst[flt_idx],&prm_nbr,NULL);
	prm_lst=(unsigned int *)nco_malloc(prm_nbr*sizeof(unsigned int));
	rcd=nco_inq_var_filter_info(nc_in_id,var_in_id_cpy,flt_lst[flt_idx],NULL,prm_lst);
	(void)sprintf(sng_foo,"%u,",flt_lst[flt_idx]);
	strcat(flt_sng,sng_foo);
	/* Overwrite on-disk DEFLATE level, if any, with user-specified level, if any */ 
	if(dfl_lvl != NCO_DFL_LVL_UNDEFINED)
	  if(flt_lst[flt_idx] == H5Z_FILTER_DEFLATE)
	    prm_lst[0]=dfl_lvl_cpy;
	for(prm_idx=0;prm_idx<prm_nbr;prm_idx++){
	  (void)sprintf(sng_foo,"%u",prm_lst[prm_idx]);
	  strcat(flt_sng,sng_foo);
	  if(prm_idx < prm_nbr-1) strcat(flt_sng,",");
	} /* !prm_idx */
	if(flt_idx < flt_nbr-1) (void)strcat(flt_sng,spr_sng);
	if(prm_lst) prm_lst=(unsigned int *)nco_free(prm_lst);
      } /* !flt_idx */
      if(flt_lst) flt_lst=(unsigned int *)nco_free(flt_lst);
      
    } /* !flt_nbr */

  } /* !VARIABLE_EXISTS_IN_INPUT */
  
  if(nco_dbg_lvl_get() >= nco_dbg_grp){
    rcd=nco_inq_varname(nc_out_id,var_out_id,var_nm);
    (void)fprintf(stdout,"%s: DEBUG %s reports variable %s, cmp_sng_glb=\"%s\", flt_sng=\"%s\"\n",nco_prg_nm_get(),fnc_nm,var_nm,nco_cmp_sng_glb ? nco_cmp_sng_glb : "none",flt_sng ? flt_sng : "none");
  } /* !dbg */

  /* User-specified global compression options override on-disk input compression settings */
  char *cmp_sng=NULL; /* [sng] Compression specification */
  /* Use global options, if any were specified, otherwise use on-disk settings */
  if(nco_cmp_sng_glb) cmp_sng=nco_cmp_sng_glb;
  else if(flt_sng) cmp_sng=flt_sng;
  
  /* Call single routine that executes all requested filters */
  if(cmp_sng) rcd=nco_tst_def_out(nc_out_id,var_out_id,cmp_sng,(nco_flt_flg_enm *)NULL);

  /* Free memory, if any, that holds input file on-disk filter string for this variable */
  if(flt_sng) flt_sng=(char *)nco_free(flt_sng);
    
  return rcd;
  
} /* !nco_tst_def_wrp() */
  
int /* O [enm] Return code */
nco_tst_def_out /* [fnc]  */
(const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const char * const cmp_sng, /* I [sng] Compression specification */
 const nco_flt_flg_enm * const flt_flgp) /* I [enm] Enumerated flags for fine-grained compression control */
{
  /* Purpose: Set compression filters in newly defined variable in output file

     Usage: 
     Until 20220501 the NCO code to define per-variable filters was scattered in ~four places 
     Introduction of new filters in netCDF 4.9.0 makes this untenable 
     Here were functionalize the invocation of original netCDF4 filters DEFLATE and Shuffle */

  const char fnc_nm[]="nco_tst_def_out()"; /* [sng] Function name */

  int rcd=NC_NOERR; /* [rcd] Return code */

  char *cmp_sng_cpy=NULL; /* [sng] Compression specification copy */

  /* Deflation */
  int deflate; /* [flg] Turn-on deflate filter */
  int shuffle; /* [flg] Turn-on shuffle filter */

  int flt_idx; /* [idx] Filter index */

  nco_bool cdc_has_flt=True; /* [flg] Available filters include requested filter */
  nco_bool lsy_flt_ok=True; /* [flg] Lossy filters are authorized (i.e., not specifically disallowed) for this variable */
  nco_flt_flg_enm flt_flg=nco_flg_nil; /* [enm] Enumerated flags for fine-grained compression control */

  /* Varibles to obtain by parsing compression specification */
  int flt_nbr=0; /* [nbr] Number of codecs specified */
  nco_flt_typ_enm *flt_alg=NULL; /* [nbr] List of filters specified */
  int *flt_lvl=NULL; /* [nbr] List of compression levels for each filter */
  int *flt_prm_nbr=NULL; /* [nbr] List of parameter numbers for each filter */
  int **flt_prm=NULL; /* [nbr] List of lists of parameters for each filter */

  if(flt_flgp) flt_flg=*flt_flgp;
  if(flt_flg == nco_flg_lsy_no) lsy_flt_ok=False;
  
  if(cmp_sng || nco_cmp_sng_glb){
    if(nco_dbg_lvl_get() >= nco_dbg_std && !nco_cmp_sng_glb) (void)fprintf(stderr,"%s: INFO %s reports requested codec string = %s\n",nco_prg_nm_get(),fnc_nm,cmp_sng);
    /* If parent routine passed a valid compression specification then use that
       This often occurs when copying variable's specifications from input files
       In this case, nco_tst_def_wrp() send the on-disk specification already merged with any global options
       Otherwise, if no specification is passed and a global specification exists, then use that 
       This often occurs when creating variables from scratch */
    if(cmp_sng) cmp_sng_cpy=(char *)strdup(cmp_sng);
    else if(nco_cmp_sng_glb) cmp_sng_cpy=(char *)strdup(nco_cmp_sng_glb);

    /* Avoid mutililating global specification by passing copy to be parsed
       This also works when invoking nco_tst_def_out() with static cmp_sng */
    (void)nco_cmp_prs(cmp_sng_cpy,(int *)NULL,&flt_nbr,&flt_alg,&flt_lvl,&flt_prm_nbr,&flt_prm);
  } /* !cmp_sng */
    
  /* Invoke applicable codec(s) */
  for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){ 
    if(nco_dbg_lvl_get() >= nco_dbg_grp) (void)fprintf(stdout,"%s: DEBUG %s executing filter: flt_nbr=%d, flt_idx=%d, flt_enm=%d flt_sng=%s, flt_lvl=%d\n",nco_prg_nm_get(),fnc_nm,flt_nbr,flt_idx,flt_alg[flt_idx],nco_flt_enm2sng(flt_alg[flt_idx]),flt_lvl[flt_idx]);
    switch(flt_alg[flt_idx]){
    case nco_flt_nil: /* If user did not select a filter then exit */
      cdc_has_flt=False;
      break;

    case nco_flt_dfl: /* DEFLATE */
      /* Overwrite HDF Lempel-Ziv compression level, if requested */
      deflate=(int)True;
      /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
      if(flt_lvl[flt_idx] <= 0) shuffle=NC_NOSHUFFLE;
      /* Shuffle never, to my knowledge, increases filesize, so shuffle by default when manually deflating (and do not shuffle when uncompressing) */
      if(flt_lvl[flt_idx] > 0) shuffle=NC_SHUFFLE;
      rcd+=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,flt_lvl[flt_idx]);
      break;

    case nco_flt_bz2: /* Bzip2 */
# if CCR_HAS_BZIP2 || NC_LIB_VER >= 490 
      if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_bzip2(nc_out_id,var_out_id,flt_lvl[flt_idx]);
# else /* !CCR_HAS_BZIP2 || NC_LIB_VER >= 490 */
      cdc_has_flt=False;
# endif /* !CCR_HAS_BZIP2 || NC_LIB_VER >= 490 */
      break;

    case nco_flt_lz4: /* LZ4 */ 
# if CCR_HAS_LZ4
      if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_lz4(nc_out_id,var_out_id,flt_lvl[flt_idx]);
# else /* !CCR_HAS_LZ4 */
      cdc_has_flt=False;
# endif /* !CCR_HAS_LZ4 */
      break;

    case nco_flt_bgr: /* BitGroom */
      if(lsy_flt_ok){
# if NC_LIB_VER >= 490 
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_BITGROOM,flt_lvl[flt_idx]);
# elif CCR_HAS_BITGROOM
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_bitgroom(nc_out_id,var_out_id,flt_lvl[flt_idx]);
# else /* !CCR_HAS_BITGROOM */
	cdc_has_flt=False;
# endif /* !CCR_HAS_BITGROOM */
      } /* !lsy_flt_ok */
      break;

    case nco_flt_btr: /* BitRound */
      if(lsy_flt_ok){
# if NC_LIB_VER >= 490 
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_BITROUND,flt_lvl[flt_idx]);
# elif CCR_HAS_BITROUND
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_bitround(nc_out_id,var_out_id,flt_lvl[flt_idx]);
# else /* !CCR_HAS_BITROUND */
	cdc_has_flt=False;
# endif /* !CCR_HAS_BITROUND */
      } /* !lsy_flt_ok */
      break;

    case nco_flt_gbr: /* Granular BitRound */
      if(lsy_flt_ok){
# if NC_LIB_VER >= 490 
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_GRANULARBR,flt_lvl[flt_idx]);
# elif CCR_HAS_GRANULARBR
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_granularbr(nc_out_id,var_out_id,flt_lvl[flt_idx]);
# else /* !CCR_HAS_GRANULARBR */
	cdc_has_flt=False;
# endif /* !CCR_HAS_GRANULARBR */
      } /* !lsy_flt_ok */
      break;

    case nco_flt_zst: /* Zstandard */
# if CCR_HAS_ZSTD || NC_LIB_VER >= 490 
      /* NB: Zstandard accepts negative compression levels */
      rcd+=nc_def_var_zstandard(nc_out_id,var_out_id,flt_lvl[flt_idx]);
# else /* !NC_LIB_VER >= 490 */
      cdc_has_flt=False;
# endif /* !CCR_HAS_ZSTD || NC_LIB_VER >= 490  */
      break;

    case nco_flt_dgr: /* DigitRound */
      cdc_has_flt=False;
      break;

    default:
      nco_dfl_case_flt_err();
      break;
    } /* !flt_alg */

    if(cdc_has_flt == False){
      (void)fprintf(stdout,"%s: ERROR %s reports neither netCDF nor CCR library appears to define an API for requested filter \"%s\". If this filter name was not a typo, then probably this filter was not built and/or not installed in netCDF or in CCR. If the filter is newish and is supposed to be in CCR, update the installed CCR then recompile NCO. Otherwise, re-try this command and specify only filters included in this list of available filters: %s\n",nco_prg_nm_get(),fnc_nm,nco_flt_enm2sng(flt_alg[flt_idx]),nco_cdc_lst_glb);
      nco_exit(EXIT_FAILURE);
    } /* !cdc_has_flt */

  } /* !flt_idx */
  
  /* Free compression string */
  if(cmp_sng_cpy) cmp_sng_cpy=(char *)nco_free(cmp_sng_cpy);

  /* Free filter parameters */
  if(flt_alg) flt_alg=(nco_flt_typ_enm *)nco_free(flt_alg);
  if(flt_lvl) flt_lvl=(int *)nco_free(flt_lvl);
  if(flt_prm_nbr) flt_prm_nbr=(int *)nco_free(flt_prm_nbr);
  if(flt_prm){
    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++) flt_prm[flt_idx]=(int *)nco_free(flt_prm[flt_idx]);
    flt_prm=(int **)nco_free(flt_prm);
  } /* !flt_prm */

  return rcd;
  
} /* !nco_tst_def_out() */

