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
   20220609: fxm free() this/these global variables */
static char *nco_cdc_lst_glb=NULL; /* [sng] List of available filters */

#if !defined(CCR_HAS_BZIP2) && (NC_LIB_VERSION < 490)
int nc_def_var_bzip2
(const int nc_id, /* I [ID] netCDF ID */
 const int var_id, /* I [ID] Variable ID */
 const int cmp_lvl) /* I [enm] Compression level */
{
  /* Purpose: Pseudo-library stub filter for Bzip2
     This particular stub routine is only called by netCDF4-enabled code
     when built against a netCDF library too old to have the nc_def_var_bzip2() function. */
  int rcd;
  const char fnc_nm[]="nc_def_var_bzip2()";
  rcd=NC_NOERR+0*(nc_id+var_id+cmp_lvl);
  (void)fprintf(stdout,"ERROR: %s reports Bzip2 filter wrapper was foiled because neither libnetcdf.a nor CCR contain a real %s. To obtain this functionality, please rebuild NCO against netCDF library version 4.9.0 (released ~20220610) or later.\nExiting...\n",fnc_nm,fnc_nm);
  nco_err_exit(rcd,fnc_nm);
  return rcd;
} /* !nc_def_var_bzip2() */
#endif /* !490, !4.9.0 */

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
 int *flt_nbrp, /* [nbr] Number of codecs specified */
 nco_flt_typ_enm **flt_algp, /* [nbr] List of filters specified */
 unsigned int **flt_idp, /* [ID] List of HDF5 filter IDs */
 int **flt_lvlp, /* [nbr] List of compression levels for each filter */
 int **flt_prm_nbrp, /* [nbr] List of parameter numbers for each filter */
 int ***flt_prmp) /* [nbr] List of lists of parameters for each filter */
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

  nco_bool FLG_COMPRESS=True; /* [flg] String specifies compression (not decompression) */

  /* Varibles to copy back to calling routine if requested */
  int flt_nbr=0; /* [nbr] Number of codecs specified */
  nco_flt_typ_enm *flt_alg=NULL; /* [nbr] List of filters specified */
  unsigned int *flt_id=NULL; /* [ID] Filter HDF5 ID */
  int *flt_lvl=NULL; /* [nbr] List of compression levels for each filter */
  int *flt_prm_nbr=NULL; /* [nbr] List of parameter numbers for each filter */
  int **flt_prm=NULL; /* [nbr] List of lists of parameters for each filter */

  if(dfl_lvlp) dfl_lvl=*dfl_lvlp;

  /* Decide whether to decompress before chopping up compression specification */
  if(cmp_sng || nco_cmp_glb_get()){
    /* Interpret a local or global specification that contains any of the (case-insensitive) strings 
       "none" (or its synonyms) as a request to decompress all output variables */
    if(strcasestr(cmp_sng,"none") || strcasestr(cmp_sng,"uncompress") || strcasestr(cmp_sng,"decompress") || strcasestr(cmp_sng,"defilter")) FLG_COMPRESS=False;
  } /* !cmp_sng */
    
  if(cmp_sng && FLG_COMPRESS){

    if(nco_dbg_lvl_get() >= nco_dbg_std && !nco_cmp_glb_get()) (void)fprintf(stderr,"%s: INFO %s reports requested codec string = %s\n",nco_prg_nm_get(),fnc_nm,cmp_sng);

    flt_lst=nco_lst_prs_1D(cmp_sng,spr_sng,&flt_nbr);

    flt_alg=(nco_flt_typ_enm *)nco_malloc(flt_nbr*sizeof(nco_flt_typ_enm));
    flt_id=(unsigned int *)nco_malloc(flt_nbr*sizeof(unsigned int));
    flt_lvl=(int *)nco_malloc(flt_nbr*sizeof(int));
    flt_prm_nbr=(int *)nco_malloc(flt_nbr*sizeof(int));
    flt_prm=(int **)nco_malloc(flt_nbr*sizeof(int *));
      
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: DEBUG %s reports codec string contains flt_nbr=%d codecs separated by \"%s\"\n",nco_prg_nm_get(),fnc_nm,flt_nbr,spr_sng);

    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
      
      prm_lst=nco_lst_prs_1D(flt_lst[flt_idx],",",&prm_nbr);

      /* First element in list is either NCO-recognized filter name or HDF5 filter ID */
      //(void)fprintf(stdout,"%s: DEBUG quark3 cmp_sng=%s, flt_nbr=%d, flt_idx=%d, prm_nbr=%d, prm_lst[0]=%s\n",nco_prg_nm_get(),cmp_sng,flt_nbr,flt_idx,prm_nbr,prm_lst[0]);
      flt_alg[flt_idx]=nco_flt_nm2enmid(prm_lst[0],flt_id+flt_idx);

      /* Remaining elements, if any, are filter parameters in the HDF5 sense */
      flt_prm_nbr[flt_idx]=prm_nbr-1;
      /* Allocate space for at least one parameter so default parameters can be supplied
	 For example, user might specify "shuffle" with no parameter, and we can supply the default */
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
    flt_id=(unsigned int *)nco_malloc(flt_nbr*sizeof(unsigned int));
    flt_lvl=(int *)nco_malloc(flt_nbr*sizeof(int));
    flt_prm_nbr=(int *)nco_malloc(flt_nbr*sizeof(int));
    flt_prm=(int **)nco_malloc(flt_nbr*sizeof(int *));

    flt_alg[0]=nco_flt_dfl;
    flt_id[0]=H5Z_FILTER_DEFLATE;
    flt_prm_nbr[0]=1;
    flt_prm[0]=(int *)nco_malloc(flt_prm_nbr[0]*sizeof(int));
    flt_prm[0][0]=dfl_lvl;
    flt_lvl[0]=flt_prm[0][0];
  } /* !flt_nbr, !dfl_lvl */
    
  const int lvl_dfl_dfl=1; /* [enm] Default level for DEFLATE */
  const int lvl_dfl_shf=4; /* [enm] Default level for Shuffle */
  const int lvl_dfl_zst=3; /* [enm] Default level for Zstandard */
  const int lvl_dfl_bz2=1; /* [enm] Default level for Bzip2 */
    
  /* Supply default levels values for selected filters */
  for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
    /* '--cmp=dfl' with unspecified level causes NCO to use default DEFLATE level */
    if(flt_alg[flt_idx] == nco_flt_dfl && flt_prm_nbr[flt_idx] == 0 && flt_prm[flt_idx][0] == NC_MIN_INT){
      flt_prm_nbr[flt_idx]=1;
      flt_prm[flt_idx][0]=lvl_dfl_dfl;
      flt_lvl[flt_idx]=flt_prm[flt_idx][0];
    } /* !dfl */
    /* '--cmp=shf' with unspecified level causes NCO to use default Shuffle level */
    if(flt_alg[flt_idx] == nco_flt_shf && flt_prm_nbr[flt_idx] == 0 && flt_prm[flt_idx][0] == NC_MIN_INT){
      flt_prm_nbr[flt_idx]=1;
      flt_prm[flt_idx][0]=lvl_dfl_shf;
      flt_lvl[flt_idx]=flt_prm[flt_idx][0];
    } /* !shf */
    /* '--cmp=zst' with unspecified level causes NCO to use default Zstandard level */
    if(flt_alg[flt_idx] == nco_flt_zst && flt_prm_nbr[flt_idx] == 0 && flt_prm[flt_idx][0] == NC_MIN_INT){
      flt_prm_nbr[flt_idx]=1;
      flt_prm[flt_idx][0]=lvl_dfl_zst;
      flt_lvl[flt_idx]=flt_prm[flt_idx][0];
    } /* !zst */
    /* '--cmp=bz2' with unspecified level causes NCO to use default Bzip2 level */
    if(flt_alg[flt_idx] == nco_flt_bz2 && flt_prm_nbr[flt_idx] == 0 && flt_prm[flt_idx][0] == NC_MIN_INT){
      flt_prm_nbr[flt_idx]=1;
      flt_prm[flt_idx][0]=lvl_dfl_bz2;
      flt_lvl[flt_idx]=flt_prm[flt_idx][0];
    } /* !bz2 */
  } /* !flt_idx */

  /* Global user-specified specification has been merged with dfl_lvl 
     Generate final global compression specification in NCO standard format */
  char *cmp_sng_std=NULL; /* [sng] Compression specification in NCO-standard format */
  char int_sng[100]; /* [sng] Buffer to hold printed integer parameters for a single filter */
  if(flt_nbr > 0){
    cmp_sng_std=(char *)nco_malloc(NCO_FLT_SNG_LNG_MAX*sizeof(char)); /* [sng] Compression specification in NCO-standard format */
    /* NUL-terminate string */
    cmp_sng_std[0]='\0';
    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
      (void)strcat(cmp_sng_std,nco_flt_enm2nmid(flt_alg[flt_idx],NULL));
      if(flt_prm_nbr[flt_idx] > 0) (void)strcat(cmp_sng_std,",");
      int_sng[0]='\0';
      for(prm_idx=0;prm_idx<flt_prm_nbr[flt_idx];prm_idx++){
	(void)sprintf(int_sng,"%d%s",flt_prm[flt_idx][prm_idx], prm_idx < flt_prm_nbr[flt_idx]-1 ? "," : "");
      } /* !prm_idx */
      (void)strcat(cmp_sng_std,int_sng);
      if(flt_idx < flt_nbr-1) (void)strcat(cmp_sng_std,spr_sng);
    } /* !flt_idx */
  } /* !flt_nbr */

  /* If cmp_sng ~ "none" then flt_nbr == 0 and we must store cmp_sng in cmp_sng_std */
  if(!cmp_sng_std && cmp_sng) cmp_sng_std=(char *)strdup(cmp_sng);
  
  /* If global user-requested compression specification string has not yet been saved, copy and save it */
  if(!nco_cmp_glb_get()) nco_cmp_glb_set((char *)strdup(cmp_sng_std));

  // if(nco_dbg_lvl_get() >= nco_dbg_grp) (void)fprintf(stdout,"%s: DEBUG cmp_sng_std = %s\n",nco_prg_nm_get(),cmp_sng_std);
  
  /* Variables to copy back to calling routine (or global variable) if requested */
  if(flt_nbrp) *flt_nbrp=flt_nbr;
  if(flt_algp) *flt_algp=flt_alg; else flt_alg=(nco_flt_typ_enm *)nco_free(flt_alg);
  if(flt_idp) *flt_idp=flt_id; else flt_id=(unsigned int *)nco_free(flt_id);
  if(flt_lvlp) *flt_lvlp=flt_lvl; else flt_lvl=(int *)nco_free(flt_lvl);
  if(flt_prm_nbrp) *flt_prm_nbrp=flt_prm_nbr; else flt_prm_nbr=(int *)nco_free(flt_prm_nbr);
  if(flt_prmp){
    *flt_prmp=flt_prm;
  }else{
    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++) flt_prm[flt_idx]=(int *)nco_free(flt_prm[flt_idx]);
    flt_prm=(int **)nco_free(flt_prm);
  } /* !flt_idx */
  if(cmp_sng_std) cmp_sng_std=(char *)nco_free(cmp_sng_std);

  /* Build list of available filters once */
  if(!nco_cdc_lst_glb){
    nco_cdc_lst_glb=(char *)nco_malloc(NCO_FLT_SNG_LNG_MAX*sizeof(char));
    nco_cdc_lst_glb[0]='\0';
    strcat(nco_cdc_lst_glb,"DEFLATE, Shuffle, Fletcher32");
    /* CCR, netCDF define tokens like CCR_HAS_BITGROOM, NC_HAS_ZSTD in ccr_meta.h, netcdf_meta.h */

    /* netCDF 4.9.0 lacks NC_HAS_BZIP2 token */
#ifdef CCR_HAS_BZIP2
    strcat(nco_cdc_lst_glb,", Bzip2");
#endif /* !CCR_HAS_BZIP2 */

#ifdef CCR_HAS_LZ4
    strcat(nco_cdc_lst_glb,", LZ4");
#endif /* !CCR_HAS_LZ4 */
#if defined(CCR_HAS_BITGROOM) || defined(NC_HAS_QUANTIZE)
    strcat(nco_cdc_lst_glb,", BitGroom");
#endif /* !CCR_HAS_BITGROOM */
#if defined(CCR_HAS_BITROUND)|| defined(NC_HAS_QUANTIZE)
    strcat(nco_cdc_lst_glb,", BitRound");
#endif /* !CCR_HAS_BITROUND */
#if defined(CCR_HAS_GRANULARBR) || defined(NC_HAS_QUANTIZE)
    strcat(nco_cdc_lst_glb,", GranularBR");
#endif /* !CCR_HAS_GRANULARBR */
#if defined(CCR_HAS_ZSTD) || defined(NC_HAS_ZSTD)
    strcat(nco_cdc_lst_glb,", Zstandard");
#endif /* !CCR_HAS_ZSTD */
#if defined(CCR_HAS_BLOSC_LZ) || defined(NC_LIB_VERSION) >= 490
    strcat(nco_cdc_lst_glb,", BLOSC_LZ");
#endif /* !CCR_HAS_BLOSC_LZ */
#if defined(CCR_HAS_BLOSC_LZ4) || defined(NC_LIB_VERSION) >= 490
    strcat(nco_cdc_lst_glb,", BLOSC LZ4");
#endif /* !CCR_HAS_BLOSC_LZ4 */
#if defined(CCR_HAS_BLOSC_LZ4_HC) || defined(NC_LIB_VERSION) >= 490
    strcat(nco_cdc_lst_glb,", BLOSC LZ4 HC");
#endif /* !CCR_HAS_BLOSC_LZ4_HC */
#if defined(CCR_HAS_SNAPPY) || defined(NC_LIB_VERSION) >= 490
    strcat(nco_cdc_lst_glb,", BLOSC Snappy");
#endif /* !CCR_HAS_SNAPPY */
#if defined(CCR_HAS_BLOSC_DEFLATE) || defined(NC_LIB_VERSION) >= 490
    strcat(nco_cdc_lst_glb,", BLOSC DEFLATE");
#endif /* !CCR_HAS_BLOSC_DEFLATE */
#if defined(CCR_HAS_BLOSC_ZSTANDARD) || defined(NC_LIB_VERSION) >= 490
    strcat(nco_cdc_lst_glb,", BLOSC Zstandard");
#endif /* !CCR_HAS_BLOSC_ZSTANDARD */
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s reports available codec list is nco_cdc_lst_glb=%s\n",nco_prg_nm_get(),fnc_nm,nco_cdc_lst_glb);
  } /* !nco_cdc_lst_glb */
  
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
nco_flt_enm2nmid /* [fnc] Convert compression filter enum to string */
(const nco_flt_typ_enm nco_flt_enm, /* I [enm] Compression filter type */
 unsigned int * const flt_idp)  /* O [ID] HDF5 filter ID */
{
  /* Purpose: Convert compression filter-type enum to NCO-standardized name and HDF filter ID, if applicable */

  switch(nco_flt_enm){
  case nco_flt_nil: return "Filter type is unset"; break;
  case nco_flt_dfl: if(flt_idp) *flt_idp=H5Z_FILTER_DEFLATE; return "DEFLATE"; break; /* 1 */
  case nco_flt_shf: if(flt_idp) *flt_idp=H5Z_FILTER_SHUFFLE; return "Shuffle"; break; /* 2 */
  case nco_flt_f32: if(flt_idp) *flt_idp=H5Z_FILTER_FLETCHER32; return "Fletcher32"; break; /* 3 */
  case nco_flt_szp: if(flt_idp) *flt_idp=H5Z_FILTER_SZIP; return "Szip"; break; /* 4 */
  case nco_flt_bz2: if(flt_idp) *flt_idp=H5Z_FILTER_BZIP2; return "Bzip2"; break; /* 307 */
  case nco_flt_lz4: if(flt_idp) *flt_idp=32004U; return "LZ4"; break; /* 32004 */
  case nco_flt_bgr: if(flt_idp) *flt_idp=32022U; return "BitGroom"; break; /* 32022 */
  case nco_flt_gbr: if(flt_idp) *flt_idp=32023U; return "Granular BitRound"; break; /* 32023 */
  case nco_flt_dgr: return "DigitRound"; break;
  case nco_flt_btr: if(flt_idp) *flt_idp=37373U; return "BitRound"; break; /* 37373 */
  case nco_flt_zst: if(flt_idp) *flt_idp=32015U; return "Zstandard"; break; /* 32015 */
  case nco_flt_bls: if(flt_idp) *flt_idp=32001U; return "BLOSC (unspecified)"; break; /* 32001 */
  case nco_flt_bls_lz: if(flt_idp) *flt_idp=32001U; return "BLOSC LZ"; break; /* 32001 */
  case nco_flt_bls_lz4: if(flt_idp) *flt_idp=32001U; return "BLOSC LZ4"; break; /* 32001 */
  case nco_flt_bls_lzh: if(flt_idp) *flt_idp=32001U; return "BLOSC LZ4 HC"; break; /* 32001 */
  case nco_flt_bls_snp: if(flt_idp) *flt_idp=32001U; return "BLOSC Snappy"; break; /* 32001 */
  case nco_flt_bls_dfl: if(flt_idp) *flt_idp=32001U; return "BLOSC DEFLATE"; break; /* 32001 */
  case nco_flt_bls_zst: if(flt_idp) *flt_idp=32001U; return "BLOSC Zstandard"; break; /* 32001 */
  case nco_flt_unk: return "Unknown"; break;
  default:
    nco_dfl_case_flt_enm_err(nco_flt_enm,"nco_flt_enm2nmid()"); break;
  } /* !nco_flt_enm */

  return (char *)NULL;
  
} /* !nco_flt_enm2nmid() */

char * /* O [sng] String describing compression filter */
nco_flt_id2nm /* [fnc] Convert compression filter HDF5 ID to string */
(const unsigned int flt_id) /* I [id] Compression filter HDF5 ID */
{
  /* Purpose: Convert compression filter HDF5 ID to descriptive string if possible */
  switch(flt_id){
  case nco_flt_nil: return "Filter type is unset"; break;
  case H5Z_FILTER_DEFLATE : return "DEFLATE"; break; /* 1 */
  case H5Z_FILTER_SHUFFLE : return "Shuffle"; break; /* 2 */
  case H5Z_FILTER_FLETCHER32 : return "Fletcher32"; break; /* 3 */
  case H5Z_FILTER_SZIP : return "Szip"; break; /* 4 */
  case H5Z_FILTER_BZIP2 : return "Bzip2"; break; /* 307 */
  case 32004 : return "LZ4"; break; /* 32004 */
  case 32022 : return "BitGroom"; break; /* 32022 */
  case 32023 : return "Granular BitRound"; break; /* 32023 */
    //  case : return "DigitRound"; break;
  case 37373: return "BitRound"; break; /* 37373 */
  case H5Z_FILTER_ZSTD : return "Zstandard"; break; /* 32015 */
  case H5Z_FILTER_BLOSC : return "BLOSC"; break; /* 32001 */
  default:
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG HDF5 filter ID = %u is unknown. Default case reached in nco_flt_id2nm()\n",nco_prg_nm_get(),flt_id);
    nco_dfl_case_generic_err(); break;
  } /* !flt_id */

  return (char *)NULL;
  
} /* !nco_flt_id2nm() */

nco_flt_typ_enm /* O [enm] Filter enum */
nco_flt_id2enm /* [fnc] Convert HDF5 compression filter ID to enum */
(const unsigned int flt_id) /* I [id] Compression filter ID */
{
  /* Purpose: Convert HDF5 compression filter ID to NCO-recognized enum */
  nco_flt_typ_enm flt_enm=nco_flt_unk; /* [enm] Compression filter type */

  switch(flt_id){
  case 0 : flt_enm=nco_flt_nil; break;
  case 1 : flt_enm=nco_flt_dfl; break;
  case 2 : flt_enm=nco_flt_shf; break;
  case 3 : flt_enm=nco_flt_f32; break;
  case 4 : flt_enm=nco_flt_szp; break;
  case 307 : flt_enm=nco_flt_bz2; break;
  case 32004 : flt_enm=nco_flt_lz4; break;
  case 32022 : flt_enm=nco_flt_bgr; break;
  case 32023 : flt_enm=nco_flt_gbr; break;
    //  case : flt_enm=nco_flt_dgr; break;
  case 37373: flt_enm=nco_flt_btr; break;
  case 32015 : flt_enm=nco_flt_zst; break;
  case 32001 : flt_enm=nco_flt_bls; break;
  default: flt_enm=nco_flt_unk; break;
  } /* !flt_id */

  if(flt_enm == nco_flt_unk) (void)fprintf(stdout,"%s: DEBUG nco_flt_id2enm() reports filter ID = %u is unknown by NCO, though may be present in filter directory.\n",nco_prg_nm_get(),flt_id);

  return flt_enm;
} /* !nco_flt_id2enm() */

nco_flt_typ_enm /* O [enm] Filter enum */
nco_flt_nm2enmid /* [fnc] Convert user-specified filter name to NCO enum */
(const char * const flt_nm, /* I [sng] User-specified filter name */
 unsigned int * const flt_idp) /* O [ID] HDF5 filter ID */
{
  /* Purpose: Convert user-specified string to enumerated filter type
     Return nco_flt_nil by default */

  const char fnc_nm[]="nco_flt_nm2enmid()"; /* [sng] Function name */

  nco_bool FLT_NM_IS_ID=False; /* [flg] Filter name is HDF5 ID */

  nco_flt_typ_enm flt_enm=nco_flt_unk; /* [enm] Compression filter type */

  unsigned int flt_id=NC_MAX_UINT; /* [ID] HDF5 Filter ID */
  
  if(flt_nm == NULL) (void)fprintf(stdout,"%s: WARNING %s was invoked without explicit filter name.\n",nco_prg_nm_get(),fnc_nm);

  /* Filter HDF5 IDs gleaned from nc_inq_var_filter_info() or copied from ncks/ncdump output will be unsigned ints
     Filter names with an NCO-standardized key will be ... strings
     First check if filter name appears to be specified by HDF5 ID */
  if(flt_idp){
    char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
    flt_id=strtoul(flt_nm,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(!*sng_cnv_rcd){
      /* Filter appears to be specified by HDF5 ID */
      flt_enm=nco_flt_id2enm(flt_id);
      if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s reports filter string %s interpreted as HDF5 ID for filter \"%s\" with NCO enum %d\n",nco_prg_nm_get(),fnc_nm,flt_nm,nco_flt_id2nm(flt_id),(int)flt_enm);
      *flt_idp=flt_id;
      FLT_NM_IS_ID=True;
    } /* !sng_cnv_rcd */
  } /* !flt_idp */

  /* Parse descriptive name string to enum if possible, then fill-in ID if known */
  if(!FLT_NM_IS_ID){
    if(!strcasecmp(flt_nm,"nil")) flt_enm=nco_flt_nil;
    else if(!strcasecmp(flt_nm,"none")) flt_enm=nco_flt_nil;
    else if(!strcasecmp(flt_nm,"default")) flt_enm=nco_flt_nil;
    
    else if(!strcasecmp(flt_nm,"deflate")) flt_enm=nco_flt_dfl;
    else if(!strcasecmp(flt_nm,"dfl")) flt_enm=nco_flt_dfl;
    else if(!strcasecmp(flt_nm,"gzp")) flt_enm=nco_flt_dfl;
    else if(!strcasecmp(flt_nm,"gzip")) flt_enm=nco_flt_dfl;
    else if(!strcasecmp(flt_nm,"gz")) flt_enm=nco_flt_dfl;
    else if(!strcasecmp(flt_nm,"zlib")) flt_enm=nco_flt_dfl;
    else if(!strcasecmp(flt_nm,"zlb")) flt_enm=nco_flt_dfl;
    
    else if(!strcasecmp(flt_nm,"shf")) flt_enm=nco_flt_shf;
    else if(!strcasecmp(flt_nm,"shuffle")) flt_enm=nco_flt_shf;
    
    else if(!strcasecmp(flt_nm,"f32")) flt_enm=nco_flt_f32;
    else if(!strcasecmp(flt_nm,"fletcher")) flt_enm=nco_flt_f32;
    else if(!strcasecmp(flt_nm,"fletcher32")) flt_enm=nco_flt_f32;

    else if(!strcasecmp(flt_nm,"szp")) flt_enm=nco_flt_szp;
    else if(!strcasecmp(flt_nm,"szip")) flt_enm=nco_flt_szp;

    else if(!strcasecmp(flt_nm,"unk")) flt_enm=nco_flt_unk;
    else if(!strcasecmp(flt_nm,"unknown")) flt_enm=nco_flt_unk;
    
    else if(!strcasecmp(flt_nm,"bz2")) flt_enm=nco_flt_bz2;
    else if(!strcasecmp(flt_nm,"bzp")) flt_enm=nco_flt_bz2;
    else if(!strcasecmp(flt_nm,"bz")) flt_enm=nco_flt_bz2;
    else if(!strcasecmp(flt_nm,"bzip")) flt_enm=nco_flt_bz2;
    else if(!strcasecmp(flt_nm,"bzip2")) flt_enm=nco_flt_bz2;
    
    else if(!strcasecmp(flt_nm,"lz4")) flt_enm=nco_flt_lz4;
    
    else if(!strcasecmp(flt_nm,"bgr")) flt_enm=nco_flt_bgr;
    else if(!strcasecmp(flt_nm,"btg")) flt_enm=nco_flt_bgr;
    else if(!strcasecmp(flt_nm,"bitgroom")) flt_enm=nco_flt_bgr;
    else if(!strcasecmp(flt_nm,"bit-groom")) flt_enm=nco_flt_bgr;
    else if(!strcasecmp(flt_nm,"bit groom")) flt_enm=nco_flt_bgr;
    else if(!strcasecmp(flt_nm,"Zen16")) flt_enm=nco_flt_bgr;
    
    else if(!strcasecmp(flt_nm,"gbr")) flt_enm=nco_flt_gbr;
    else if(!strcasecmp(flt_nm,"granularbr")) flt_enm=nco_flt_gbr;
    else if(!strcasecmp(flt_nm,"granular")) flt_enm=nco_flt_gbr;
    else if(!strcasecmp(flt_nm,"granular bitround")) flt_enm=nco_flt_gbr;
    else if(!strcasecmp(flt_nm,"granular-bitround")) flt_enm=nco_flt_gbr;
    else if(!strcasecmp(flt_nm,"granularbitround")) flt_enm=nco_flt_gbr;
    
    else if(!strcasecmp(flt_nm,"dgr")) flt_enm=nco_flt_dgr;
    else if(!strcasecmp(flt_nm,"digitround")) flt_enm=nco_flt_dgr;
    else if(!strcasecmp(flt_nm,"digit round")) flt_enm=nco_flt_dgr;
    else if(!strcasecmp(flt_nm,"digit-round")) flt_enm=nco_flt_dgr;
    else if(!strcasecmp(flt_nm,"DCG19")) flt_enm=nco_flt_dgr;
    
    else if(!strcasecmp(flt_nm,"btr")) flt_enm=nco_flt_btr;
    else if(!strcasecmp(flt_nm,"bitround")) flt_enm=nco_flt_btr;
    else if(!strcasecmp(flt_nm,"bit round")) flt_enm=nco_flt_btr;
    else if(!strcasecmp(flt_nm,"bit-round")) flt_enm=nco_flt_btr;
    else if(!strcasecmp(flt_nm,"Kou20")) flt_enm=nco_flt_btr;
    
    else if(!strcasecmp(flt_nm,"zst")) flt_enm=nco_flt_zst;
    else if(!strcasecmp(flt_nm,"zstd")) flt_enm=nco_flt_zst;
    else if(!strcasecmp(flt_nm,"zstandard")) flt_enm=nco_flt_zst;
    
    else if(!strcasecmp(flt_nm,"blosc lz4 hc")) flt_enm=nco_flt_bls_lzh;
    else if(!strcasecmp(flt_nm,"blosc_lz4_hc")) flt_enm=nco_flt_bls_lzh;
    else if(!strcasecmp(flt_nm,"blosclz4hc")) flt_enm=nco_flt_bls_lzh;
    else if(!strcasecmp(flt_nm,"bls_lzh")) flt_enm=nco_flt_bls_lzh;
    else if(!strcasecmp(flt_nm,"bls_lz4hc")) flt_enm=nco_flt_bls_lzh;
    else if(!strcasecmp(flt_nm,"blosc_lzh")) flt_enm=nco_flt_bls_lzh;
    else if(!strcasecmp(flt_nm,"blosc_lz4hc")) flt_enm=nco_flt_bls_lzh;
    
    else if(!strcasecmp(flt_nm,"blosc lz4")) flt_enm=nco_flt_bls_lz4;
    else if(!strcasecmp(flt_nm,"blosc_lz4")) flt_enm=nco_flt_bls_lz4;
    else if(!strcasecmp(flt_nm,"bls_lz4")) flt_enm=nco_flt_bls_lz4;
    else if(!strcasecmp(flt_nm,"blslz4")) flt_enm=nco_flt_bls_lz4;
    else if(!strcasecmp(flt_nm,"blosclz4")) flt_enm=nco_flt_bls_lz4;
    
    else if(!strcasecmp(flt_nm,"blosc lz")) flt_enm=nco_flt_bls_lz;
    else if(!strcasecmp(flt_nm,"blosc_lz")) flt_enm=nco_flt_bls_lz;
    else if(!strcasecmp(flt_nm,"bls_lz")) flt_enm=nco_flt_bls_lz;
    else if(!strcasecmp(flt_nm,"blslz")) flt_enm=nco_flt_bls_lz;
    
    else if(!strcasecmp(flt_nm,"blosc snappy")) flt_enm=nco_flt_bls_snp;
    else if(!strcasecmp(flt_nm,"bloscsnappy")) flt_enm=nco_flt_bls_snp;
    else if(!strcasecmp(flt_nm,"blosc_snappy")) flt_enm=nco_flt_bls_snp;
    else if(!strcasecmp(flt_nm,"bls_snp")) flt_enm=nco_flt_bls_snp;
    else if(!strcasecmp(flt_nm,"blssnp")) flt_enm=nco_flt_bls_snp;
    else if(!strcasecmp(flt_nm,"bls snp")) flt_enm=nco_flt_bls_snp;
    
    else if(!strcasecmp(flt_nm,"blosc deflate")) flt_enm=nco_flt_bls_dfl;
    else if(!strcasecmp(flt_nm,"bloscdeflate")) flt_enm=nco_flt_bls_dfl;
    else if(!strcasecmp(flt_nm,"blosc_deflate")) flt_enm=nco_flt_bls_dfl;
    else if(!strcasecmp(flt_nm,"bls_dfl")) flt_enm=nco_flt_bls_dfl;
    else if(!strcasecmp(flt_nm,"blsdfl")) flt_enm=nco_flt_bls_dfl;
    else if(!strcasecmp(flt_nm,"bls dfl")) flt_enm=nco_flt_bls_dfl;
    
    else if(!strcasecmp(flt_nm,"blosc zstandard")) flt_enm=nco_flt_bls_zst;
    else if(!strcasecmp(flt_nm,"blosczstandard")) flt_enm=nco_flt_bls_zst;
    else if(!strcasecmp(flt_nm,"blosc_zstandard")) flt_enm=nco_flt_bls_zst;
    else if(!strcasecmp(flt_nm,"bls_zst")) flt_enm=nco_flt_bls_zst;
    else if(!strcasecmp(flt_nm,"blszst")) flt_enm=nco_flt_bls_zst;
    else if(!strcasecmp(flt_nm,"bls zst")) flt_enm=nco_flt_bls_zst;
    else if(!strcasecmp(flt_nm,"bls_zstd")) flt_enm=nco_flt_bls_zst;
    else if(!strcasecmp(flt_nm,"blszstd")) flt_enm=nco_flt_bls_zst;
    else if(!strcasecmp(flt_nm,"bls zstd")) flt_enm=nco_flt_bls_zst;

    if(flt_idp) (void)nco_flt_enm2nmid(flt_enm,flt_idp);
  } /* !FLT_NM_IS_ID */
  
  if(flt_enm == nco_flt_unk) (void)fprintf(stderr,"%s: WARNING %s reports unknown user-specified filter \"%s\"\n",nco_prg_nm_get(),fnc_nm,flt_nm);

  /* Unknown filters should work if they are nonetheless installed
     However, this requires that their HDF5 filter ID be parsed and stored */
  return flt_enm;

} /* !nco_flt_nm2enmid() */

int /* O [enm] Return code */
nco_flt_old_wrp /* [fnc] Call filters immediately after variable definition */
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

  //const char fnc_nm[]="nco_flt_old_wrp()"; /* [sng] Function name */

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
    rcd=nco_flt_old_out(nc_out_id,var_out_id,dfl_lvl);

  return rcd;
  
} /* !nco_flt_old_wrp() */
  
int /* O [enm] Return code */
nco_flt_old_out /* [fnc]  */
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

  //  const char fnc_nm[]="nco_flt_old_out()"; /* [sng] Function name */

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

} /* !nco_flt_old_out() */
  
int /* O [enm] Return code */
nco_flt_def_wrp /* [fnc] Call filters immediately after variable definition */
(const int nc_in_id, /* I [id] netCDF input file/group ID */
 const int var_in_id, /* I [id] Variable ID */
 const char * const var_nm_in, /* I [nm] Variable name [optional] */
 const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id) /* I [id] Variable ID */
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

  const char fnc_nm[]="nco_flt_def_wrp()"; /* [sng] Function name */

  nco_bool VARIABLE_EXISTS_IN_INPUT=False; /* [flg] Variable exists in input file */
  //nco_bool COPY_COMPRESSION_FROM_INPUT=False; /* [flg] Copy compression setting from input to output */

  char var_nm[NC_MAX_NAME+1L]; /* [sng] Variable name */
  char *flt_sng=NULL; /* [sng] Compression specification, if any, of variable as stored in input file */
  char *cmp_sng=NULL; /* [sng] Compression specification */
  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Deflation */
  int deflate; /* [flg] Turn-on deflate filter */
  int dfl_lvl_in; /* [enm] Deflate level [0..9] in input file */
  int shuffle; /* [flg] Turn-on shuffle filter */
  int var_in_id_cpy=-1; /* [id] Writable copy of input variable ID */

  nco_bool FLG_COMPRESS=True; /* [flg] String specifies compression (not decompression) */

  /* Use copies so var_in_id, dfl_lvl can remain const in prototype */
  var_in_id_cpy=var_in_id;

  /* Write (or overwrite) var_in_id when var_nm_in is supplied */
  if(var_nm_in && nc_in_id >= 0){
    /* Output variable may not exist in input file (e.g., when ncap2 defines new variable) */
    rcd=nco_inq_varid_flg(nc_in_id,var_nm_in,&var_in_id_cpy);
    if(rcd == NC_NOERR) VARIABLE_EXISTS_IN_INPUT=True;
  } /* !var_nm_in */

  if(nc_in_id >= 0 && var_in_id_cpy >= 0) VARIABLE_EXISTS_IN_INPUT=True;

  if((cmp_sng=nco_cmp_glb_get()) != NULL){
    if(strcasestr(cmp_sng,"none") || strcasestr(cmp_sng,"uncompress") || strcasestr(cmp_sng,"decompress") || strcasestr(cmp_sng,"defilter")) FLG_COMPRESS=False;
    cmp_sng=NULL;
  } /* !cmp_sng */

  /* If variable exists in input file, copy its compression then return
     Otherwise call compression based solely on settings requested for output file */
  if(VARIABLE_EXISTS_IN_INPUT && FLG_COMPRESS){

    /* NB: dfl_lvl_in will be zero if deflate == 0 */
    rcd=nco_inq_var_deflate(nc_in_id,var_in_id_cpy,&shuffle,&deflate,&dfl_lvl_in);

    /* Generic filter calls DNE prior to 4.7.4 so copy input settings and return
       20220621: fxm this routine would need to parse nco_cmp_sng_glb to determine user-specified dfl_lvl */
#if NC_LIB_VERSION < 474
    /* Copy original filters and return if newer filters unavailable */ 
    if(dfl_lvl_in != NCO_DFL_LVL_UNDEFINED){
      /* Overwrite HDF Lempel-Ziv compression level, if requested */
      deflate=(int)True;
      /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
      if(dfl_lvl_in <= 0) shuffle=NC_NOSHUFFLE;
      /* Shuffle never, to my knowledge, increases filesize, so shuffle by default when manually deflating (and do not shuffle when uncompressing) */
      if(dfl_lvl_in > 0) shuffle=NC_SHUFFLE;
      rcd=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,dfl_lvl_in);
      return rcd;
    } /* !dfl_lvl_in */
#endif /* !474, !4.7.4 */

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
      flt_sng=(char *)nco_malloc(NCO_FLT_SNG_LNG_MAX*sizeof(char));
      flt_sng[0]='\0';
      for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
	rcd=nco_inq_var_filter_info(nc_in_id,var_in_id_cpy,flt_lst[flt_idx],&prm_nbr,NULL);
	if(prm_nbr > 0){
	  prm_lst=(unsigned int *)nco_malloc(prm_nbr*sizeof(unsigned int));
	  rcd=nco_inq_var_filter_info(nc_in_id,var_in_id_cpy,flt_lst[flt_idx],NULL,prm_lst);
	} /* !prm_nbr */
	/* 20200624: prm_lst is NULL iff prm_nbr==0, e.g., for Fletcher32
	   Only append zero if parameters exist */
	(void)sprintf(sng_foo,"%u%s",flt_lst[flt_idx],(prm_nbr > 0) ? "," : "");
	strcat(flt_sng,sng_foo);
	for(prm_idx=0;prm_idx<prm_nbr;prm_idx++){
	  (void)sprintf(sng_foo,"%u",prm_lst[prm_idx]);
	  strcat(flt_sng,sng_foo);
	  if(prm_idx < prm_nbr-1) strcat(flt_sng,",");
	} /* !prm_idx */
	if(flt_idx < flt_nbr-1) (void)strcat(flt_sng,spr_sng);
	//(void)fprintf(stdout,"%s: DEBUG quark1 flt_sng=%s, flt_nbr=%d, flt_idx=%d, flt_id=%d, prm_nbr=%d\n",nco_prg_nm_get(),flt_sng,(int)flt_nbr,(int)flt_idx,(int)flt_lst[flt_idx],(int)prm_nbr);
	if(prm_lst) prm_lst=(unsigned int *)nco_free(prm_lst);
      } /* !flt_idx */
      if(flt_lst) flt_lst=(unsigned int *)nco_free(flt_lst);
    } /* !flt_nbr */
  } /* !VARIABLE_EXISTS_IN_INPUT */
  
  if(nco_dbg_lvl_get() >= nco_dbg_grp){
    rcd=nco_inq_varname(nc_out_id,var_out_id,var_nm);
    (void)fprintf(stdout,"%s: DEBUG %s reports variable %s, cmp_sng_glb=\"%s\", flt_sng=\"%s\"\n",nco_prg_nm_get(),fnc_nm,var_nm,nco_cmp_glb_get() ? nco_cmp_glb_get() : "no user-specified filters",flt_sng ? flt_sng : "no on-disk filters");
  } /* !dbg */

  /* Use global options, if any were specified, otherwise use on-disk settings */
  if(nco_cmp_glb_get()) cmp_sng=nco_cmp_glb_get(); else if(flt_sng) cmp_sng=flt_sng;

  /* Prevent quantization of non-floating-point variables */
  nco_flt_flg_enm flt_flg=nco_flt_flg_all_ok; /* [enm] Enumerated flag for fine-grained compression control */
  nc_type var_typ; /* [enm] netCDF type of output variable (usually same as input) */
  rcd=nco_inq_vartype(nc_out_id,var_out_id,&var_typ);
  if(var_typ != NC_FLOAT && var_typ != NC_DOUBLE) flt_flg=nco_flt_flg_qnt_no;
  if(nco_is_crd_var(nc_out_id,var_out_id) || nco_is_spc_in_cf_att(nc_out_id,"bounds",var_out_id,NULL) || nco_is_spc_in_cf_att(nc_out_id,"climatology",var_out_id,NULL) || nco_is_spc_in_cf_att(nc_out_id,"coordinates",var_out_id,NULL) || nco_is_spc_in_cf_att(nc_out_id,"grid_mapping",var_out_id,NULL)) flt_flg=nco_flt_flg_prc_fll;

  if(nco_dbg_lvl_get() >= nco_dbg_var){
    if(flt_flg != nco_flt_flg_all_ok) (void)fprintf(stdout,"%s: DEBUG %s reports variable type or NCO guidelines (based on coordinate-like variables and CF rules) prevent variable %s from using lossy compression\n",nco_prg_nm_get(),fnc_nm,var_nm);
  } /* !dbg */

  /* Call single routine that executes all requested filters */
  if(cmp_sng) rcd=nco_flt_def_out(nc_out_id,var_out_id,cmp_sng,flt_flg);

  /* Free memory, if any, that holds input file on-disk filter string for this variable */
  if(flt_sng) flt_sng=(char *)nco_free(flt_sng);
    
  return rcd;
} /* !nco_flt_def_wrp() */
  
int /* O [enm] Return code */
nco_flt_def_out /* [fnc]  */
(const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const char * const cmp_sng, /* I [sng] Compression specification */
 const nco_flt_flg_enm flt_flg) /* I [enm] Enumerated flags for fine-grained compression control */
{
  /* Purpose: Set compression filters in newly defined variable in output file

     Usage: 
     Until 20220501 the NCO code to define per-variable filters was scattered in ~four places 
     Introduction of new filters in netCDF 4.9.0 makes this untenable 
     Here were functionalize the invocation of original netCDF4 filters DEFLATE and Shuffle,
     and add invocation of other known filters */

  const char fnc_nm[]="nco_flt_def_out()"; /* [sng] Function name */

  int rcd=NC_NOERR; /* [rcd] Return code */

  char *cmp_sng_cpy=NULL; /* [sng] Compression specification copy */
  char var_nm[NC_MAX_NAME+1L]; /* [sng] Variable name */

  /* Deflation */
  int deflate; /* [flg] Turn-on deflate filter */
  int shuffle; /* [flg] Turn-on shuffle filter */
  int flt_idx; /* [idx] Filter index */
  int prm_idx; /* [idx] Parameter index */

  nco_bool cdc_has_flt=True; /* [flg] Available filters include requested filter */
  nco_bool lsy_flt_ok=True; /* [flg] Lossy filters are authorized (i.e., not specifically disallowed) for this variable */

  unsigned int add_shf=1; /* [flg] Add Shuffle to BLOSC filter */
  unsigned int bls_sbc=NC_MAX_UINT; /* [enm] BLOSC subcompressor */
  unsigned int blk_sz=0U; /* [nbr] Blocksize for BLOSC filter */
  unsigned int *flt_prm_uns=NULL; /* [enm] Filter parameters stored as unsigned ints */

  /* Varibles to obtain by parsing compression specification */
  int flt_nbr=0; /* [nbr] Number of codecs specified */
  nco_flt_typ_enm *flt_alg=NULL; /* [nbr] List of filters specified */
  unsigned int *flt_id=NULL; /* [ID] List of HDF5 filter IDs */
  int *flt_lvl=NULL; /* [nbr] List of compression levels for each filter */
  int *flt_prm_nbr=NULL; /* [nbr] List of parameter numbers for each filter */
  int **flt_prm=NULL; /* [nbr] List of lists of parameters for each filter */

  if(flt_flg == nco_flt_flg_prc_fll || flt_flg == nco_flt_flg_qnt_no) lsy_flt_ok=False;
  
  if(cmp_sng || nco_cmp_glb_get()){
    if(nco_dbg_lvl_get() >= nco_dbg_std && !nco_cmp_glb_get()) (void)fprintf(stderr,"%s: INFO %s reports requested codec string = %s\n",nco_prg_nm_get(),fnc_nm,cmp_sng);
    /* If parent routine passed a valid compression specification then use that
       This often occurs when copying variable's specifications from input files
       In this case, nco_flt_def_wrp() send the on-disk specification already merged with any global options
       Otherwise, if no specification is passed and a global specification exists, then use that 
       This often occurs when creating variables from scratch */
    if(cmp_sng) cmp_sng_cpy=(char *)strdup(cmp_sng);
    else if(nco_cmp_glb_get()) cmp_sng_cpy=(char *)strdup(nco_cmp_glb_get());

    /* Avoid mutililating global specification by passing copy to be parsed
       This also works when invoking nco_flt_def_out() with static cmp_sng */
    (void)nco_cmp_prs(cmp_sng_cpy,(int *)NULL,&flt_nbr,&flt_alg,&flt_id,&flt_lvl,&flt_prm_nbr,&flt_prm);
  } /* !cmp_sng */
    
  /* Get variable name for debugging output */
  rcd=nco_inq_varname(nc_out_id,var_out_id,var_nm);

  /* Set extra parameters needed by BLOSC filters */
  for(flt_idx=0;flt_idx<flt_nbr;flt_idx++)
    if(flt_id[flt_idx] == H5Z_FILTER_BLOSC)
      rcd+=nco_inq_var_blk_sz(nc_out_id,var_out_id,&blk_sz);
  
  /* Invoke applicable codec(s) */
  for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){ 
    if(nco_dbg_lvl_get() >= nco_dbg_grp) (void)fprintf(stdout,"%s: DEBUG %s executing filter for %s: flt_nbr=%d, flt_idx=%d, flt_enm=%d flt_nm=%s, flt_id=%u, flt_lvl=%d\n",nco_prg_nm_get(),fnc_nm,var_nm,flt_nbr,flt_idx,flt_alg[flt_idx],nco_flt_enm2nmid(flt_alg[flt_idx],NULL),flt_id[flt_idx],flt_lvl[flt_idx]);
    switch(flt_alg[flt_idx]){
    case nco_flt_nil: /* If user did not select a filter then exit */
      cdc_has_flt=False;
      break;

    case nco_flt_dfl: /* DEFLATE */
      /* Overwrite HDF Lempel-Ziv compression level, if requested */
      deflate=(int)True;
      /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
      if(flt_lvl[flt_idx] <= 0) shuffle=NC_NOSHUFFLE;
      /* Shuffle by default when manually deflating (and do not shuffle when uncompressing)
	 In rare cases Shuffle can increase compressed filesize */
      if(flt_lvl[flt_idx] > 0) shuffle=NC_SHUFFLE;
      rcd+=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,flt_lvl[flt_idx]);
      break;

    case nco_flt_shf: /* Shuffle */
      if(flt_lvl[flt_idx] <= 0){
	shuffle=NC_NOSHUFFLE;
	deflate=(int)False;
	rcd+=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,0);
      }else{
	shuffle=NC_SHUFFLE;
	deflate=(int)True;
	rcd+=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,0);
      } /* !flt_lvl */
      break;

    case nco_flt_f32: /* Fletcher32 */
      rcd+=nco_def_var_fletcher32(nc_out_id,var_out_id,NC_FLETCHER32);
      break;

    case nco_flt_bz2: /* Bzip2 */
      rcd+=nco_inq_filter_avail_flg(nc_out_id,flt_id[flt_idx]);
      /* netCDF 4.9.0 lacks NC_HAS_BZIP2 token */
      if(rcd == NC_NOERR){
	rcd+=nc_def_var_bzip2(nc_out_id,var_out_id,flt_lvl[flt_idx]);
      }else{ /* !rcd */
	/* Reset rcd */
	rcd=NC_NOERR;
	cdc_has_flt=False;
      } /* !rcd */
      break;

    case nco_flt_lz4: /* LZ4 */ 
#if CCR_HAS_LZ4
      if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_lz4(nc_out_id,var_out_id,flt_lvl[flt_idx]);
#else /* !CCR_HAS_LZ4 */
      cdc_has_flt=False;
#endif /* !CCR_HAS_LZ4 */
      break;

    case nco_flt_bgr: /* BitGroom */
      if(lsy_flt_ok){
#if NC_HAS_QUANTIZE 
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_BITGROOM,flt_lvl[flt_idx]);
#elif CCR_HAS_BITGROOM
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_bitgroom(nc_out_id,var_out_id,flt_lvl[flt_idx]);
# else /* !CCR_HAS_BITGROOM */
	cdc_has_flt=False;
#endif /* !CCR_HAS_BITGROOM */
      } /* !lsy_flt_ok */
      break;

    case nco_flt_btr: /* BitRound */
      if(lsy_flt_ok){
#if NC_HAS_QUANTIZE 
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_BITROUND,flt_lvl[flt_idx]);
#elif CCR_HAS_BITROUND
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_bitround(nc_out_id,var_out_id,flt_lvl[flt_idx]);
#else /* !CCR_HAS_BITROUND */
	cdc_has_flt=False;
#endif /* !CCR_HAS_BITROUND */
      } /* !lsy_flt_ok */
      break;

    case nco_flt_gbr: /* Granular BitRound */
      if(lsy_flt_ok){
#if NC_HAS_QUANTIZE 
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_GRANULARBR,flt_lvl[flt_idx]);
#elif CCR_HAS_GRANULARBR
	if(flt_lvl[flt_idx] > 0) rcd+=nc_def_var_granularbr(nc_out_id,var_out_id,flt_lvl[flt_idx]);
#else /* !CCR_HAS_GRANULARBR */
	cdc_has_flt=False;
#endif /* !CCR_HAS_GRANULARBR */
      } /* !lsy_flt_ok */
      break;

    case nco_flt_zst: /* Zstandard */
#if CCR_HAS_ZSTD || NC_HAS_ZSTD
      /* NB: Zstandard accepts negative compression levels */
      rcd+=nc_def_var_zstandard(nc_out_id,var_out_id,flt_lvl[flt_idx]);
      //(void)fprintf(stdout,"%s: DEBUG quark5 cmp_sng=%s, flt_nbr=%d, flt_idx=%d, flt_enm=%d, rcd=%d\n",nco_prg_nm_get(),cmp_sng,flt_nbr,flt_idx,(int)flt_alg[flt_idx],rcd);
#else /* !NC_HAS_ZSTD */
      cdc_has_flt=False;
#endif /* !CCR_HAS_ZSTD || NC_HAS_ZSTD */
      break;

    case nco_flt_bls_lz: bls_sbc=BLOSC_LZ; /* BLOSC LZ */
    case nco_flt_bls_lz4: bls_sbc=BLOSC_LZ4; /* BLOSC LZ4 */
    case nco_flt_bls_lzh: bls_sbc=BLOSC_LZ4HC; /* BLOSC LZ4 HC */
    case nco_flt_bls_snp: bls_sbc=BLOSC_SNAPPY; /* BLOSC Snappy */
    case nco_flt_bls_dfl: bls_sbc=BLOSC_ZLIB; /* BLOSC DEFLATE */
    case nco_flt_bls_zst: bls_sbc=BLOSC_ZSTD; /* BLOSC Zstandard */
#if NC_LIB_VERSION >= 490
      if(blk_sz > 0U) rcd+=nc_def_var_blosc(nc_out_id,var_out_id,bls_sbc,(unsigned int)flt_lvl[flt_idx],blk_sz,add_shf); else if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s reports variable %s is not chunked so will not attempt BLOSC compression\n",nco_prg_nm_get(),fnc_nm,var_nm);
#else /* !NC_LIB_VERSION >= 490 */
      add_shf+=0*add_shf;
      bls_sbc+=0*bls_sbc;
      cdc_has_flt=False;
#endif /* NC_LIB_VERSION >= 490  */
      break;

    case nco_flt_dgr: /* DigitRound */
      cdc_has_flt=False;
      break;

    case nco_flt_unk: /* Unknown filter referenced by ID not name */
      /* Unknown filters must call the filter handler with unsigned ints */
      rcd+=nco_inq_filter_avail(nc_out_id,flt_id[flt_idx]);
      flt_prm_uns=(unsigned int *)nco_malloc(flt_prm_nbr[flt_idx]*sizeof(unsigned int));
      for(prm_idx=0;prm_idx<flt_prm_nbr[flt_idx];prm_idx++)
	flt_prm_uns[prm_idx]=(unsigned int)flt_prm[flt_idx][prm_idx];
      rcd+=nco_def_var_filter(nc_out_id,var_out_id,flt_id[flt_idx],(unsigned int)flt_prm_nbr[flt_idx],flt_prm_uns);
      if(flt_prm_uns) flt_prm_uns=(unsigned int *)nco_free(flt_prm_uns);
      break;

    default:
      nco_dfl_case_flt_err();
      break;
    } /* !flt_alg */

    if(cdc_has_flt == False){
      (void)fprintf(stdout,"%s: ERROR %s reports neither netCDF nor CCR library appears to define an API for requested filter \"%s\". If this filter name was not a typo, then probably this filter was not built and/or not installed in netCDF or in CCR. If the filter is newish and is supposed to be in CCR, update the installed CCR then recompile NCO. Otherwise, re-try this command and specify only filters included in this list of available filters: %s\n",nco_prg_nm_get(),fnc_nm,nco_flt_enm2nmid(flt_alg[flt_idx],NULL),nco_cdc_lst_glb);
      nco_exit(EXIT_FAILURE);
    } /* !cdc_has_flt */

    if(rcd != NC_NOERR){
      (void)fprintf(stdout,"%s: WARNING %s returned from filter execution on variable %s with bad return code: cmp_sng=%s, flt_nbr=%d, flt_idx=%d, flt_enm=%d, flt_id=%u, rcd=%d \"%s\". Proceeding anyway, though do not expect this filter to have been applied in the output file.\n",nco_prg_nm_get(),fnc_nm,var_nm,cmp_sng,flt_nbr,flt_idx,(int)flt_alg[flt_idx],flt_id[flt_idx],rcd,nc_strerror(rcd));
      rcd=NC_NOERR;
    } /* !rcd */
      
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
  
} /* !nco_flt_def_out() */

int /* O [enm] Return code */
nco_inq_var_blk_sz
(const int nc_id, /* I [id] netCDF output file/group ID */
 const int var_id, /* I [id] Variable ID */
 unsigned int * const blk_szp) /* O [B] Block size in bytes */
{
  /* Purpose: Determine block size used for chunked storage */
  const char fnc_nm[]="nco_inq_blk_sz()"; /* [sng] Function name */
  char var_nm[NC_MAX_NAME+1L];

  int rcd=NC_NOERR; /* [rcd] Return code */

  int *dmn_id; /* [ID] Dimension IDs */
  int dmn_idx; /* [idx] Dimension index */
  int dmn_nbr; /* [nbr] Number of dimensions in variable */
  int srg_typ; /* [enm] Storage type */

  nc_type var_typ; /* [enm] Variable type */
  unsigned int blk_sz=0U; /* [nbr] Blocksize of each chunk */

  size_t *cnk_sz; /* [nbr] Chunksize list */

  rcd+=nco_inq_varname(nc_id,var_id,var_nm);

  rcd+=nco_inq_varndims(nc_id,var_id,&dmn_nbr);
  if(dmn_nbr == 0){
    (void)fprintf(stderr,"%s: ERROR %s reports variable %s is scalar not array. Unsuitable for BLOSC compression filters, bailing now...\n",nco_prg_nm_get(),fnc_nm,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* !srg_typ */

  rcd+=nco_inq_var_chunking(nc_id,var_id,&srg_typ,(size_t *)NULL);
  if(srg_typ != NC_CHUNKED){
    /* netCDF autormatically and silently chunks variables of storage type NC_CONTIGUOUS for DEFLATE, Zstandard filters
       Such variables are set to the netCDF4 default chunk sizes (thus ignoring user-specified sizes?) 
       However, netCDF does chunk NC_CONTIGUOUS variables sent to BLOSC filters 
       nco_inq_var_blk_sz() is only called in preparation for BLOSC filters 
       20220627: For now it's OK to set NC_CONTIGUOUS block size to 0U and return 
       Revisit this issue once we get BLOSC filters actually working
       May require put nco_cnk_sz_set_trv() before nco_flt_def_wrp() to utilize user-defined chunk sizes? */
    // (void)fprintf(stdout,"%s: INFO %s reports variable %s is not chunked so will not attempt BLOSC compression\n",nco_prg_nm_get(),fnc_nm,var_nm);
    *blk_szp=blk_sz;
    return rcd;
  } /* !srg_typ */

  /* Allocate space to hold dimension IDs */
  dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
  /* Allocate space to hold chunksizes */
  cnk_sz=(size_t *)nco_malloc(dmn_nbr*sizeof(size_t));    

  /* Get dimension IDs */
  rcd+=nco_inq_vardimid(nc_id,var_id,dmn_id);
  /* Get chunksizes */
  rcd+=nco_inq_var_chunking(nc_id,var_id,(int *)NULL,cnk_sz);
  /* Inquire variable type */
  rcd+=nco_inq_vartype(nc_id,var_id,&var_typ);

  blk_sz=nco_typ_lng(var_typ);
  for(dmn_idx=0;dmn_idx<dmn_nbr;dmn_idx++)
    blk_sz*=cnk_sz[dmn_idx];
  
  /* Free space holding dimension IDs and chunksizes */
  if(cnk_sz) cnk_sz=(size_t *)nco_free(cnk_sz);
  if(dmn_id) dmn_id=(int *)nco_free(dmn_id);

  *blk_szp=blk_sz;
  
  if(nco_dbg_lvl_get() >= nco_dbg_grp) (void)fprintf(stdout,"%s: DEBUG %s reports block size of variable %s is %u\n",nco_prg_nm_get(),fnc_nm,var_nm,blk_sz);

  return rcd;
} /* !nco_inq_var_blk_sz() */

