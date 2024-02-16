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
     ncks --dbg=2 --cdc=btg,3|zstd,1 in.nc out.nc */

  const char fnc_nm[]="nco_cmp_prs()";

  char **prm_lst; /* [sng] List of user-supplied filter parameters as strings */
  char **flt_lst; /* [sng] List of user-supplied filters as pipe-separated lists of comma-separated strings */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  char flt_nm_id[12]; /* [sng] Filter ID converted to name string */
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
    if(strcasestr(cmp_sng,"none") || strcasestr(cmp_sng,"uncompress") || strcasestr(cmp_sng,"decompress") || strcasestr(cmp_sng,"defilter") || strcasestr(cmp_sng,"unset")) FLG_COMPRESS=False;
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
  const int lvl_dfl_bz2=1; /* [enm] Default level for Bzip2 */
  const int lvl_dfl_zst=3; /* [enm] Default level for Zstandard */
  const int lvl_dfl_btg=3; /* [enm] Default level (NSD) for BitGroom */
  const int lvl_dfl_gbr=3; /* [enm] Default level (NSD) for GranularBR */
  const int lvl_dfl_btr=9; /* [enm] Default level (NSB) for BitRound */
  const int lvl_dfl_bls=1; /* [enm] Default level Blosc LZ and Snappy filters */
    
  /* Supply default levels values for selected filters */
  for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
    if(flt_prm_nbr[flt_idx] == 0 && flt_prm[flt_idx][0] == NC_MIN_INT){
      switch(flt_alg[flt_idx]){
      case nco_flt_dfl:
      case nco_flt_dns:
      case nco_flt_bls_dfl:
	/* '--cmp=dfl' or '--cmp=dns' or '--cmp=bls_dfl' with unspecified level causes NCO to use default DEFLATE level */
	flt_prm_nbr[flt_idx]=1;
	flt_prm[flt_idx][0]=lvl_dfl_dfl;
	flt_lvl[flt_idx]=flt_prm[flt_idx][0];
	break;

      case nco_flt_shf:
	/* '--cmp=shf' with unspecified level causes NCO to use default Shuffle level */
	flt_prm_nbr[flt_idx]=1;
	flt_prm[flt_idx][0]=lvl_dfl_shf;
	flt_lvl[flt_idx]=flt_prm[flt_idx][0];
	break;

      case nco_flt_bz2:
	/* '--cmp=bz2' with unspecified level causes NCO to use default Bzip2 level */
	flt_prm_nbr[flt_idx]=1;
	flt_prm[flt_idx][0]=lvl_dfl_bz2;
	flt_lvl[flt_idx]=flt_prm[flt_idx][0];
	break;

      case nco_flt_zst:
      case nco_flt_bls_zst:
	/* '--cmp=zst' with unspecified level causes NCO to use default Zstandard level */
	flt_prm_nbr[flt_idx]=1;
	flt_prm[flt_idx][0]=lvl_dfl_zst;
	flt_lvl[flt_idx]=flt_prm[flt_idx][0];
	break;

      case nco_flt_btg:
	flt_prm_nbr[flt_idx]=1;
	flt_prm[flt_idx][0]=lvl_dfl_btg;
	flt_lvl[flt_idx]=flt_prm[flt_idx][0];
	break;

      case nco_flt_gbr:
	/* '--cmp=gbr' with unspecified level causes NCO to use default GranularBR level */
	flt_prm_nbr[flt_idx]=1;
	flt_prm[flt_idx][0]=lvl_dfl_gbr;
	flt_lvl[flt_idx]=flt_prm[flt_idx][0];
	break;

      case nco_flt_btr:
	/* '--cmp=btr' with unspecified level causes NCO to use default BitRound level */
	flt_prm_nbr[flt_idx]=1;
	flt_prm[flt_idx][0]=lvl_dfl_btr;
	flt_lvl[flt_idx]=flt_prm[flt_idx][0];
	break;

      case nco_flt_bls:
      case nco_flt_bls_lz:
      case nco_flt_bls_lz4:
      case nco_flt_bls_lzh:
      case nco_flt_bls_snp:
	/* '--cmp=bls_???' with unspecified level causes NCO to use default Blosc level */
	flt_prm_nbr[flt_idx]=1;
	flt_prm[flt_idx][0]=lvl_dfl_bls;
	flt_lvl[flt_idx]=flt_prm[flt_idx][0];
	break;

      default:
	break;
      } /* !flt_alg[flt_idx] */
    } /* !flt_prm_nbr */
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
      if(flt_alg[flt_idx] != nco_flt_unk){
	(void)strcat(cmp_sng_std,nco_flt_enm2nmid(flt_alg[flt_idx],NULL));
      }else{
        flt_nm_id[0]='\0';
	(void)sprintf(flt_nm_id,"%u",flt_id[flt_idx]);
	(void)strcat(cmp_sng_std,flt_nm_id);
      } /* !flt_alg */
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
     ncks --dbg=2 --filter="32022,3"
     ncks --dbg=2 --filter="32004,1U,1,-1,1.0f,-1.0d,1UL,-1L,1US,-1S" */

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
  case nco_flt_dns: if(flt_idp) *flt_idp=H5Z_FILTER_DEFLATE; return "DEFLATE No Shuffle"; break; /* 1 */
  case nco_flt_shf: if(flt_idp) *flt_idp=H5Z_FILTER_SHUFFLE; return "Shuffle"; break; /* 2 */
  case nco_flt_f32: if(flt_idp) *flt_idp=H5Z_FILTER_FLETCHER32; return "Fletcher32"; break; /* 3 */
  case nco_flt_szp: if(flt_idp) *flt_idp=H5Z_FILTER_SZIP; return "Szip"; break; /* 4 */
  case nco_flt_bz2: if(flt_idp) *flt_idp=H5Z_FILTER_BZIP2; return "Bzip2"; break; /* 307 */
  case nco_flt_lz4: if(flt_idp) *flt_idp=32004U; return "LZ4"; break; /* 32004 */
  case nco_flt_btg: if(flt_idp) *flt_idp=32022U; return "BitGroom"; break; /* 32022 */
  case nco_flt_gbr: if(flt_idp) *flt_idp=32023U; return "Granular BitRound"; break; /* 32023 */
  case nco_flt_dgr: return "DigitRound"; break;
  case nco_flt_btr: if(flt_idp) *flt_idp=37373U; return "BitRound"; break; /* 37373 */
  case nco_flt_zst: if(flt_idp) *flt_idp=32015U; return "Zstandard"; break; /* 32015 */
  case nco_flt_bls: if(flt_idp) *flt_idp=32001U; return "Blosc"; break; /* 32001 */
  case nco_flt_bls_lz: if(flt_idp) *flt_idp=32001U; return "Blosc LZ"; break; /* 32001 */
  case nco_flt_bls_lz4: if(flt_idp) *flt_idp=32001U; return "Blosc LZ4"; break; /* 32001 */
  case nco_flt_bls_lzh: if(flt_idp) *flt_idp=32001U; return "Blosc LZ4 HC"; break; /* 32001 */
  case nco_flt_bls_snp: if(flt_idp) *flt_idp=32001U; return "Blosc Snappy"; break; /* 32001 */
  case nco_flt_bls_dfl: if(flt_idp) *flt_idp=32001U; return "Blosc DEFLATE"; break; /* 32001 */
  case nco_flt_bls_zst: if(flt_idp) *flt_idp=32001U; return "Blosc Zstandard"; break; /* 32001 */
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
  case H5Z_FILTER_BLOSC : return "Blosc"; break; /* 32001 */
  default:
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO HDF5 filter ID = %u is unknown in nco_flt_id2nm(). This is fine so long as the associated filter is in the plugin directory.\n",nco_prg_nm_get(),flt_id);
    break;
  } /* !flt_id */

  return (char *)NULL;
  
} /* !nco_flt_id2nm() */

char * /* O [sng] String describing Blosc sub-compressor */
nco_flt_sbc2nm /* [fnc] Convert Blosc sub-compressor ID to name */
(const unsigned int sbc_id) /* I [id] Blosc sub-compressor ID */
{
  /* Purpose: Convert Blosc sub-compressor ID to descriptive string if possible */
  switch(sbc_id){
  case BLOSC_LZ: return "LZ"; break; /* 0 */
  case BLOSC_LZ4: return "LZ4"; break; /* 1 */
  case BLOSC_LZ4HC: return "LZ4HC"; break; /* 2 */
  case BLOSC_SNAPPY: return "Snappy"; break; /* 3 */
  case BLOSC_ZLIB: return "DEFLATE"; break; /* 4 */
  case BLOSC_ZSTD: return "ZStandard"; break; /* 5 */
  default:
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG Blosc sub-compressor ID = %u is unknown. Default case reached in nco_flt_sbc2nm()\n",nco_prg_nm_get(),sbc_id);
    nco_dfl_case_generic_err((int)sbc_id); break;
  } /* !sbc_id */

  return (char *)NULL;
} /* !nco_flt_sbc2nm() */

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
  case 32022 : flt_enm=nco_flt_btg; break;
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
      if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s reports filter string %s interpreted as HDF5 ID for filter \"%s\" with NCO enum %d. Filter enum will be set to Unknown so that filter is invoked via generic nc_def_var_filter() rather than by any specific filter wrapper.\n",nco_prg_nm_get(),fnc_nm,flt_nm,nco_flt_id2nm(flt_id),(int)flt_enm);
      //flt_enm=nco_flt_unk;
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
    
    else if(!strcasecmp(flt_nm,"dns")) flt_enm=nco_flt_dns;
    else if(!strcasecmp(flt_nm,"dflnoshf")) flt_enm=nco_flt_dns;
    else if(!strcasecmp(flt_nm,"deflate no shuffle")) flt_enm=nco_flt_dns;
    else if(!strcasecmp(flt_nm,"deflatenoshuffle")) flt_enm=nco_flt_dns;

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
    
    else if(!strcasecmp(flt_nm,"btg")) flt_enm=nco_flt_btg;
    else if(!strcasecmp(flt_nm,"btg")) flt_enm=nco_flt_btg;
    else if(!strcasecmp(flt_nm,"bitgroom")) flt_enm=nco_flt_btg;
    else if(!strcasecmp(flt_nm,"bit-groom")) flt_enm=nco_flt_btg;
    else if(!strcasecmp(flt_nm,"bit groom")) flt_enm=nco_flt_btg;
    else if(!strcasecmp(flt_nm,"Zen16")) flt_enm=nco_flt_btg;
    
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
    
    else if(!strcasecmp(flt_nm,"blosc")) flt_enm=nco_flt_bls;
    else if(!strcasecmp(flt_nm,"bls")) flt_enm=nco_flt_bls;

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

    else if(!strcasecmp(flt_nm,"unknown")) flt_enm=nco_flt_unk;

    if(flt_idp && flt_enm != nco_flt_unk) (void)nco_flt_enm2nmid(flt_enm,flt_idp);

    if(flt_enm == nco_flt_unk){
      /* "Unknown" filters will be used as IDs, only exit() when name is truly _unknown_ (not "unknown") */
      if(strcasecmp(flt_nm,"unknown")){
	(void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified filter \"%s\". HINT: This filter was apparently specified by an unrecognized name. Therefore NCO has no way to assign it an HDF5 filter ID to invoke. If the filter does exist (and NCO just does not know it by name) then replace the name by the HDF5 filter ID in the invocation and the filter will be found and used if it is available in the plugin directory. Otherwise, eliminate this filter (or fix the spelling of its name) in the invocation and retry.\n",nco_prg_nm_get(),fnc_nm,flt_nm);
	nco_exit(EXIT_FAILURE);
      } /* !flt_nm */
    } /* !flt_enm */
  } /* !FLT_NM_IS_ID */

  if(flt_enm == nco_flt_unk && FLT_NM_IS_ID){
    if(flt_id != NC_MAX_UINT && !nco_flt_id2nm(flt_id)) (void)fprintf(stderr,"%s: INFO %s user-specified filter \"%s\" is not in the NCO database of recognized filters. However, it should work if it is a valid HDF5 filter ID with an associated filter in the plugin directory.\n",nco_prg_nm_get(),fnc_nm,flt_nm);
  } /* !flt_enm */

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

  /* 20221130: Explicitly disallow compression of variable-length types so as not to run afoul of
     https://github.com/Unidata/netcdf-c/pull/2231 */
  nc_type var_typ; /* [enm] netCDF type of output variable (usually same as input) */
  rcd=nco_inq_vartype(nc_out_id,var_out_id,&var_typ);
  if(var_typ == NC_STRING || var_typ == NC_VLEN){
    rcd=nco_inq_varname(nc_out_id,var_out_id,var_nm);
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s ignoring attempt to compress variable %s which is of variable-length type %s\n",nco_prg_nm_get(),fnc_nm,var_nm,nco_typ_sng(var_typ));
    return rcd;
  } /* !var_typ */

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
    }else{ /* !dfl_lvl_in */
      /* This branch handles copying compressor information with old libraries (Grele has netCDF 4.6.0!)
	 NCO can only handle DEFLATE (as in the above if-branch) with libraries this old
	 If we continue this function then library will need to execute newish API calls for filters
	 By definition the old library lacks those calls
	 Nothing else to do but return now and pray user is not trying to copy new file with old library */
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
	/* 20200624: prm_lst is NULL iff prm_nbr == 0, e.g., for Fletcher32
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
  if(var_typ != NC_FLOAT && var_typ != NC_DOUBLE) flt_flg=nco_flt_flg_qnt_no;
  if(nco_is_crd_var(nc_out_id,var_out_id) || nco_is_spc_in_cf_att(nc_out_id,"bounds",var_out_id,NULL) || nco_is_spc_in_cf_att(nc_out_id,"climatology",var_out_id,NULL) || nco_is_spc_in_cf_att(nc_out_id,"coordinates",var_out_id,NULL) || nco_is_spc_in_cf_att(nc_out_id,"grid_mapping",var_out_id,NULL) || nco_is_spc_in_cf_att(nc_out_id,"lossy_compression",var_out_id,NULL)) flt_flg=nco_flt_flg_prc_fll;

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

  unsigned int bls_shf=0; /* [flg] Blosc (not HDF5) will perform Shuffle filter */
  unsigned int bls_sbc=NC_MAX_UINT; /* [enm] Blosc subcompressor */
  unsigned int blk_sz=0U; /* [nbr] Blocksize for Blosc filter */
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

  /* Set extra parameters needed by Blosc filters */
  for(flt_idx=0;flt_idx<flt_nbr;flt_idx++)
    if(flt_id[flt_idx] == H5Z_FILTER_BLOSC)
      break;
  /* If a Blosc filter has been requested ... */
  if(flt_idx != flt_nbr){
    /* Determine block size for Blosc */
    rcd=nco_inq_var_blk_sz(nc_out_id,var_out_id,&blk_sz);
    /* If Shuffle has also been requested, then use Blosc shuffle not HDF5 Shuffle
       Blosc shuffle is faster since it is optimized to use AVX2 instructions etc etc */
    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++)
      if(flt_id[flt_idx] == H5Z_FILTER_SHUFFLE)
	bls_shf=1;
  } /* flt_idx */
  
  /* Invoke applicable codec(s) */
  for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){ 
    if(nco_dbg_lvl_get() >= nco_dbg_grp) (void)fprintf(stdout,"%s: DEBUG %s executing filter for %s: flt_nbr=%d, flt_idx=%d, flt_enm=%d, flt_nm=%s, flt_id=%u, flt_lvl=%d\n",nco_prg_nm_get(),fnc_nm,var_nm,flt_nbr,flt_idx,flt_alg[flt_idx],nco_flt_enm2nmid(flt_alg[flt_idx],NULL),flt_id[flt_idx],flt_lvl[flt_idx]);
    switch(flt_alg[flt_idx]){
    case nco_flt_nil: /* If user did not select a filter then exit */
      cdc_has_flt=False;
      break;

    case nco_flt_dfl: /* DEFLATE */
      // 20220703 Handle nco_flt_dns as specific type of nco_flt_unk below   
      //    case nco_flt_dns: /* DEFLATE No Shuffle */
      /* Overwrite HDF Lempel-Ziv compression level, if requested */
      deflate=(int)True;
      /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
      if(flt_lvl[flt_idx] <= 0) shuffle=NC_NOSHUFFLE;
      /* Shuffle by default when manually deflating (and do not shuffle when uncompressing)
	 In rare cases Shuffle can increase compressed filesize */
      if(flt_lvl[flt_idx] > 0) shuffle=NC_SHUFFLE;
      if(flt_alg[flt_idx] == nco_flt_dns) shuffle=NC_NOSHUFFLE;
      rcd+=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,flt_lvl[flt_idx]);
      break;

    case nco_flt_shf: /* Shuffle */
      /* Allow Blosc Shuffle to pre-empt HDF5 Shuffle */
      if(bls_shf) continue;
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

    case nco_flt_btg: /* BitGroom */
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

      /* Set Blosc sub-compressor here, then execute filter after switch statement (though still within filter loop) */
    case nco_flt_bls: /* Blosc default compressor is Blosc LZ */
    case nco_flt_bls_lz: /* Blosc LZ */
      bls_sbc=BLOSC_LZ; break;
    case nco_flt_bls_lz4: bls_sbc=BLOSC_LZ4; break; /* Blosc LZ4 */
    case nco_flt_bls_lzh: bls_sbc=BLOSC_LZ4HC; break; /* Blosc LZ4 HC */
    case nco_flt_bls_snp: bls_sbc=BLOSC_SNAPPY; break; /* Blosc Snappy */
    case nco_flt_bls_dfl: bls_sbc=BLOSC_ZLIB; break; /* Blosc DEFLATE */
    case nco_flt_bls_zst: bls_sbc=BLOSC_ZSTD; break; /* Blosc Zstandard */

    case nco_flt_dgr: /* DigitRound */
      cdc_has_flt=False;
      break;

      /* nco_flt_dns can be handled either by a variant of nco_flt_dfl or as a specific nco_flt_unk
	 We handle it here to exercise the nco_flt_unk mechanism */
    case nco_flt_dns: /* DEFLATE No Shuffle */
    case nco_flt_unk: /* Unknown filter referenced by ID not name */
      /* Unknown filters must call the filter handler with unsigned ints */
#if NC_LIB_VERSION >= 490
      rcd+=nco_inq_filter_avail(nc_out_id,flt_id[flt_idx]);
#endif /* NC_LIB_VERSION >= 490  */
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

    /* Execute Blosc filters here, outside switch statement (for conciseness) */
    if(bls_sbc != NC_MAX_UINT){
#if NC_LIB_VERSION >= 490
      rcd=nco_inq_filter_avail_flg(nc_out_id,flt_id[flt_idx]);
      /* netCDF 4.9.0 lacks NC_HAS_BLOSC token */
      if(rcd == NC_NOERR){
	/* Description of Blosc performance with small blocksizes:
	   https://github.com/Blosc/c-blosc/issues/307
	   https://github.com/Blosc/c-blosc/issues/337 */
#define NCO_MIN_CNK_BLOSC 4096U
	if(blk_sz >= NCO_MIN_CNK_BLOSC){
	  rcd+=nc_def_var_blosc(nc_out_id,var_out_id,bls_sbc,(unsigned int)flt_lvl[flt_idx],blk_sz,bls_shf);
	}else if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO %s reports variable %s is not chunked or has chunksize < %u B so will not attempt Blosc compression\n",nco_prg_nm_get(),fnc_nm,var_nm,NCO_MIN_CNK_BLOSC);
      }else{ /* !rcd */
	/* Reset rcd */
	(void)fprintf(stdout,"%s: WARNING %s reports nc_inq_filter_avail() did not find netCDF codec for requested filter \"%s\".\n",nco_prg_nm_get(),fnc_nm,nco_flt_enm2nmid(flt_alg[flt_idx],NULL));
	rcd=NC_NOERR;
	cdc_has_flt=False;
      } /* !rcd */
#else /* !NC_LIB_VERSION >= 490 */
      bls_shf+=0*bls_shf;
      bls_sbc+=0*bls_sbc;
      cdc_has_flt=False;
#endif /* NC_LIB_VERSION >= 490 */
      /* Reset bls_sbc to default */
      bls_sbc=NC_MAX_UINT; 
    } /* !bls_sbc */

    if(!cdc_has_flt){
      /* Build list of available filters the first time it might be useful in a debugging message */
      if(!nco_cdc_lst_glb) (void)nco_cdc_lst_bld(nc_out_id);

      char *nvr_HDF5_PLUGIN_PATH=NULL; /* [sng] Path where netCDF-HDF5 plugins are stored */
      nvr_HDF5_PLUGIN_PATH=getenv("HDF5_PLUGIN_PATH");
      (void)fprintf(stdout,"%s: ERROR %s reports the netCDF library does not appear to define an API for requested filter (aka codec) \"%s\". If this filter name was not a typo, then probably this filter was not built and/or not installed by netCDF (nor CCR).\nHINT: If the filter is supposed to be in netCDF (e.g., Zstandard), be sure that the external filter libraries (e.g., libzstd.a) is installed. Moreover, the netCDF-HDF5 \"glue\" library (e.g., lib__nch5zstd.so) for each HDF5-style filter must reside where libnetcdf looks for it. The location where it was built during netCDF installation can be determined by executing 'nc-config --plugindir'. Unless configured otherwise during installation, it will be /usr/local/hdf5/lib/plugin. However, the environment variable HDF5_PLUGIN_PATH (if it exists) will override this location. Currently, HDF5_PLUGIN_PATH = %s. If the preceding hints do not resolve the problem, re-try this command and specify only filters included in this list of available filters: %s\n",nco_prg_nm_get(),fnc_nm,nco_flt_enm2nmid(flt_alg[flt_idx],NULL),(nvr_HDF5_PLUGIN_PATH) ? nvr_HDF5_PLUGIN_PATH : "not set",nco_cdc_lst_glb);
      nco_exit(EXIT_FAILURE);
    } /* !cdc_has_flt */

    if(rcd != NC_NOERR){
      (void)fprintf(stdout,"%s: WARNING %s returned from filter execution on variable %s with bad return code: cmp_sng=%s, flt_nbr=%d, flt_idx=%d, flt_enm=%d, flt_id=%u, rcd=%d \"%s\". Proceeding anyway, though do not expect this filter to have been applied in the output file.\n",nco_prg_nm_get(),fnc_nm,var_nm,cmp_sng,flt_nbr,flt_idx,(int)flt_alg[flt_idx],flt_id[flt_idx],rcd,nc_strerror(rcd));
      rcd=NC_NOERR;
    } /* !rcd */
      
    /* 20240122 Add draft CF-Compliant lossy compression metadata */
    switch(flt_alg[flt_idx]){
    case nco_flt_btg: /* BitGroom */
    case nco_flt_btr: /* BitRound */
    case nco_flt_gbr: /* Granular BitRound */
      if(lsy_flt_ok && cdc_has_flt){
	(void)nco_qnt_mtd(nc_out_id,var_out_id,(nco_flt_typ_enm)0,flt_alg[flt_idx],flt_lvl[flt_idx]);
      } /* !lsy_flt_ok */
      break;
    default:
      break;
    } /* !flt_alg */

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
nco_cdc_lst_bld
(const int nc_out_id) /* I [id] netCDF output file/group ID */
{
  /* Purpose: Build list of available quantization algorithms and HDF5 Filters */
  /* Build list of available filters once */
  const char fnc_nm[]="nco_cdc_lst_bld()";

  char char_foo;

  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Build list of available filters once */
  if(!nco_cdc_lst_glb){
    /* Lame attempt to avoid races when multiple threads "simultaneously" check this pointer
       Idea is to instantly set the global value before waiting for the following system
       call to malloc() to complete */
    nco_cdc_lst_glb=&char_foo;
    nco_cdc_lst_glb=(char *)nco_malloc(NCO_FLT_SNG_LNG_MAX*sizeof(char));
    nco_cdc_lst_glb[0]='\0';

    strcat(nco_cdc_lst_glb,"DEFLATE, Shuffle, Fletcher32");

    /* CCR, netCDF define tokens like CCR_HAS_BITGROOM, NC_HAS_ZSTD in ccr_meta.h, netcdf_meta.h */
#if defined(CCR_HAS_BITGROOM) || defined(NC_HAS_QUANTIZE)
    strcat(nco_cdc_lst_glb,", BitGroom");
#endif /* !CCR_HAS_BITGROOM */
#if defined(CCR_HAS_BITROUND)|| defined(NC_HAS_QUANTIZE)
    strcat(nco_cdc_lst_glb,", BitRound");
#endif /* !CCR_HAS_BITROUND */
#if defined(CCR_HAS_GRANULARBR) || defined(NC_HAS_QUANTIZE)
    strcat(nco_cdc_lst_glb,", GranularBR");
#endif /* !CCR_HAS_GRANULARBR */

#if defined(_CCR_H) || (NC_LIB_VERSION >= 490)
  unsigned int flt_id; /* [ID] HDF5 filter ID */
  const char hlp_txt[]="This is probably fixable because this filter is supported by all default installations of netCDF version 4.9.0 or higher. HINT: If you build netCDF from source, please be sure it was configured with the following options: \"--enable-nczarr\" and \"--with-plugin-dir=${HDF5_PLUGIN_PATH}\". The latter is especially important in netCDF 4.9.0. Also, please be sure the library for the missing filter (e.g., libzstd.a, libblosc.a, libbz2.a) is in an automatically searched directory, e.g., $LD_LIBRARY_PATH or /usr/lib.";
#endif /* NC_LIB_VERSION >= 490 */
  
    /* Only call nc_inq_filter_avail() if it exists and might be successful */
#if defined(CCR_HAS_BZIP2) || (NC_LIB_VERSION >= 490)
    flt_id=H5Z_FILTER_BZIP2;
    rcd=nco_inq_filter_avail_flg(nc_out_id,flt_id);
    if(rcd == NC_NOERR) strcat(nco_cdc_lst_glb,", Bzip2"); else (void)fprintf(stdout,"%s: WARNING %s reports nco_inq_filter_avail() did not find %s filter (with HDF5 filter ID = %u) as an HDF5 shared library filter. %s\n",nco_prg_nm_get(),fnc_nm,nco_flt_id2nm(flt_id),flt_id,hlp_txt);
#endif /* !BZIP2 */
    
#if defined(CCR_HAS_ZSTD) || (NC_LIB_VERSION >= 490)
    flt_id=H5Z_FILTER_ZSTD;
    rcd=nco_inq_filter_avail_flg(nc_out_id,flt_id);
    if(rcd == NC_NOERR) strcat(nco_cdc_lst_glb,", Zstandard"); else (void)fprintf(stdout,"%s: WARNING %s reports nco_inq_filter_avail() did not find %s filter (with HDF5 filter ID = %u) as an HDF5 shared library filter. %s\n",nco_prg_nm_get(),fnc_nm,nco_flt_id2nm(flt_id),flt_id,hlp_txt);
#endif /* !ZSTD */

#if defined(CCR_HAS_BLOSC) || (NC_LIB_VERSION >= 490)
    flt_id=H5Z_FILTER_BLOSC;
    rcd=nco_inq_filter_avail_flg(nc_out_id,flt_id);
    if(rcd == NC_NOERR) strcat(nco_cdc_lst_glb,", Blosc (LZ = default, LZ4, LZ4 HC, DEFLATE, Snappy, Zstandard)"); else (void)fprintf(stdout,"%s: WARNING %s reports nco_inq_filter_avail() did not find %s filter (with HDF5 filter ID = %u) as an HDF5 shared library filter. %s\n",nco_prg_nm_get(),fnc_nm,nco_flt_id2nm(flt_id),flt_id,hlp_txt);
#endif /* !BLOSC */

    /* Reset return code */
    rcd=NC_NOERR;

    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s reports available codec list is nco_cdc_lst_glb=%s\n",nco_prg_nm_get(),fnc_nm,nco_cdc_lst_glb);
  } /* !nco_cdc_lst_glb */
  
  return rcd;
} /* !nco_cdc_lst_bld() */

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
    (void)fprintf(stderr,"%s: ERROR %s reports variable %s is scalar not array. Unsuitable for Blosc compression filters, bailing now...\n",nco_prg_nm_get(),fnc_nm,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* !srg_typ */

  rcd+=nco_inq_var_chunking(nc_id,var_id,&srg_typ,(size_t *)NULL);
  if(srg_typ != NC_CHUNKED){
    /* netCDF autormatically and silently chunks variables of storage type NC_CONTIGUOUS for DEFLATE, Zstandard filters
       Such variables are set to the netCDF4 default chunk sizes (thus ignoring user-specified sizes?) 
       However, netCDF does chunk NC_CONTIGUOUS variables sent to Blosc filters 
       nco_inq_var_blk_sz() is only called in preparation for Blosc filters 
       20220627: For now it's OK to set NC_CONTIGUOUS block size to 0U and return 
       Revisit this issue once we get Blosc filters actually working
       May require put nco_cnk_sz_set_trv() before nco_flt_def_wrp() to utilize user-defined chunk sizes? */
    // (void)fprintf(stdout,"%s: INFO %s reports variable %s is not chunked so will not attempt Blosc compression\n",nco_prg_nm_get(),fnc_nm,var_nm);
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

int /* O [enm] Return code */
nco_qnt_mtd /* [fnc] Define output filters based on input filters */
(const int nc_id, /* I [id] netCDF file/group ID */
 const int var_id, /* I [id] Variable ID */
 const nco_flt_typ_enm nco_flt_baa_enm, /* [nbr] NCO BAA filter enum */
 const nco_flt_typ_enm nco_flt_hdf_enm, /* [nbr] NCO HDF5 filter enum */
 const int qnt_lvl) /* I [enm] NSD/NSB level */
{
  /* Purpose: */
  
  /* Test CF algorithm:
     ncks -O -7 -v one_dmn_rec_var_flt --cmp='shf|zst,3' --baa=8 --ppc default=9 --ppc one_dmn_rec_var_flt=6 ~/nco/data/in.nc ~/foo.nc
     ncks -m --hdn ~/foo.nc
     Controlled Vocabularies (CVs): 
     Family (optional): quantize
     Algorithm: BitGroom, BitRound, Granular BitGroom
     Implementation: libnetcdf 4.9.2 */

  const char fnc_nm[]="nco_qnt_mtd()";

  aed_sct aed_ppc_alg;
  aed_sct aed_ppc_cnt;
  aed_sct aed_ppc_lvl;

  //  char alg_nm_dsd[]="least_significant_digit";
  char alg_nm_btg[]="BitGroom"; /* [sng] CV value per draft CF-Convention */
  char alg_nm_shv[]="BitShave";
  char alg_nm_set[]="BitSet";
  char alg_nm_dgr[]="DigitRound";
  char alg_nm_gbr[]="GranularBitRound"; /* [sng] CV value per draft CF-Convention */
  char alg_nm_bgr[]="BitGroomRound";
  char alg_nm_sh2[]="HalfShave";
  char alg_nm_brt[]="BruteForce";
  char alg_nm_btr[]="BitRound"; /* [sng] CV value per draft CF-Convention */
  char cnt_nm_all[]="lossy_compression"; /* [sng] Attribute name to hold lossy compression container name (per draft CF-Convention) */
  char fml_val[]="quantize"; /* [sng] Container variable family value (CV per draft CF-Convention) (Optional) */
  char mpl_val_libnetcdf[]="libnetcdf"; /* [sng] Container variable implementation attribute value (per draft CF-Convention) */
  char mpl_val_nco[]="NCO"; /* [sng] Container variable implementation attribute value (per draft CF-Convention) */
  char qnt_lvl_nsb_nm[]="lossy_compression_nsb"; /* [sng] Attribute name to hold NSB (CV value per draft CF-Convention) */
  char qnt_lvl_nsd_nm[]="lossy_compression_nsd"; /* [sng] Attribute name to hold NSD (CV value per draft CF-Convention) */
  char var_cnt_nm[]="compression_info"; /* [sng] Container variable name */
  char var_cnt_alg_nm[]="algorithm"; /* [sng] Container variable attribute name for algorithm type (per draft CF-Convention) */
  char var_cnt_fml_nm[]="family"; /* [sng] Container variable attribute name for algorithm family type (per draft CF-Convention) */
  char var_cnt_mpl_nm[]="implementation"; /* [sng] Container variable attribute name for algorithm implementation (per draft CF-Convention) */
  char var_nm[NC_MAX_NAME+1L]; /* [sng] Variable name */

  int qnt_lvl_dpl; /* [enm] Duplicate of const NSD/NSB level */
  int qnt_lvl_old; /* [enm] Existing NSD/NSB level, if any */
  int rcd=NC_NOERR;
  int var_cnt_id; /* [id] Container variable ID */

  nco_bool flg_baa=False; /* [flg] Write metadata for internal NCO-generated bit adjustment */
  nco_bool flg_hdf=False; /* [flg] Write metadata for external libnetCDF-generated quantization */

  /* Determine whether filter is applied by NCO or by libnetCDF */
  if(nco_flt_hdf_enm != nco_flt_nil) flg_hdf=True; else flg_baa=True;
  
  rcd=nco_inq_varname(nc_id,var_id,var_nm);

  if(flg_hdf){
    switch(nco_flt_hdf_enm){
    case nco_flt_btg: aed_ppc_alg.att_nm=alg_nm_btg; break; /* 8 [enm] BitGroom */
    case nco_flt_dgr: aed_ppc_alg.att_nm=alg_nm_dgr; break; /* 9 [enm] DigitRound */
    case nco_flt_gbr: aed_ppc_alg.att_nm=alg_nm_gbr; break; /* 10 [enm] Granular BitRound */
    case nco_flt_btr: aed_ppc_alg.att_nm=alg_nm_btr; break; /* 11 [enm] BitRound */
    default: 
      (void)fprintf(stdout,"%s: ERROR %s reports unknown libnetCDF quantization algorithm\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
      break;
    } /* !nco_flt_hdf_enm */
    switch(nco_flt_hdf_enm){
    case nco_flt_btg: 
    case nco_flt_dgr: 
    case nco_flt_gbr: 
      aed_ppc_lvl.att_nm=qnt_lvl_nsd_nm; break;
    case nco_flt_btr: 
      aed_ppc_lvl.att_nm=qnt_lvl_nsb_nm; break;
    default: 
      (void)fprintf(stdout,"%s: ERROR %s reports unknown libnetCDF quantization algorithm\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
      break;
    } /* !nco_flt_hdf_enm */
  } /* !flg_hdf */
      
  if(flg_baa){
    switch(nco_flt_baa_enm){
    case nco_baa_btg: aed_ppc_alg.att_nm=alg_nm_btg; break;
    case nco_baa_shv: aed_ppc_alg.att_nm=alg_nm_shv; break;
    case nco_baa_set: aed_ppc_alg.att_nm=alg_nm_set; break;
    case nco_baa_dgr: aed_ppc_alg.att_nm=alg_nm_dgr; break;
    case nco_baa_gbr: aed_ppc_alg.att_nm=alg_nm_gbr; break;
    case nco_baa_bgr: aed_ppc_alg.att_nm=alg_nm_bgr; break;
    case nco_baa_sh2: aed_ppc_alg.att_nm=alg_nm_sh2; break;
    case nco_baa_brt: aed_ppc_alg.att_nm=alg_nm_brt; break;
    case nco_baa_btr: aed_ppc_alg.att_nm=alg_nm_btr; break;
    default: 
      (void)fprintf(stdout,"%s: ERROR %s reports unknown NCO bit-adjustment algorithm\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
      break;
    } /* !nco_flt_baa_enm */
    switch(nco_flt_baa_enm){
    case nco_baa_btg: 
    case nco_baa_shv: 
    case nco_baa_set: 
    case nco_baa_dgr: 
    case nco_baa_gbr: 
    case nco_baa_bgr:
    case nco_baa_sh2: 
    case nco_baa_brt: 
      aed_ppc_lvl.att_nm=qnt_lvl_nsd_nm; break;
    case nco_baa_btr: 
      aed_ppc_lvl.att_nm=qnt_lvl_nsb_nm; break;
    default: 
      (void)fprintf(stdout,"%s: ERROR %s reports unknown NCO bit-adjustment algorithm\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
      break;
    } /* !nco_flt_baa_enm */
  } /* !flg_baa */

#define NCO_MAX_LEN_MPL_SNG 100
  char mpl_val_sng[NCO_MAX_LEN_MPL_SNG];
  if(flg_baa) (void)snprintf(mpl_val_sng,NCO_MAX_LEN_MPL_SNG,"%s version %s",mpl_val_nco,NCO_VERSION);
  if(flg_hdf) (void)snprintf(mpl_val_sng,NCO_MAX_LEN_MPL_SNG,"%s version %s",mpl_val_libnetcdf,NC_VERSION);

  qnt_lvl_dpl=qnt_lvl;
  aed_ppc_lvl.val.ip=&qnt_lvl_dpl;
  aed_ppc_cnt.var_nm=aed_ppc_lvl.var_nm=var_nm;
  aed_ppc_cnt.id=aed_ppc_lvl.id=var_id;
  aed_ppc_cnt.att_nm=cnt_nm_all;
  aed_ppc_cnt.val.cp=var_cnt_nm;
  rcd=nco_inq_att_flg(nc_id,aed_ppc_lvl.id,aed_ppc_lvl.att_nm,&aed_ppc_lvl.type,&aed_ppc_lvl.sz);
  if(rcd != NC_NOERR){
    /* No PPC attribute yet exists */
    aed_ppc_cnt.sz=strlen(aed_ppc_cnt.val.cp);
    aed_ppc_cnt.type=NC_CHAR;
    aed_ppc_cnt.mode=aed_create;
    aed_ppc_lvl.sz=1L;
    aed_ppc_lvl.type=NC_INT;
    aed_ppc_lvl.mode=aed_create;
    (void)nco_aed_prc(nc_id,var_id,aed_ppc_cnt);
    (void)nco_aed_prc(nc_id,var_id,aed_ppc_lvl);
  }else{
    if(aed_ppc_lvl.sz == 1L && aed_ppc_lvl.type == NC_INT){
      /* Conforming PPC attribute already exists, replace with new value only if rounder */
      (void)nco_get_att(nc_id,aed_ppc_lvl.id,aed_ppc_lvl.att_nm,&qnt_lvl_old,NC_INT);
      if(qnt_lvl < qnt_lvl_old){
	aed_ppc_lvl.mode=aed_modify;
	(void)nco_aed_prc(nc_id,var_id,aed_ppc_lvl);
      } /* !aed_ppc_lvl.sz, !aed_ppc_lvl.type */
    }else{ /* !conforming */
      (void)fprintf(stderr,"%s: WARNING Non-conforming %s attribute found in variable %s, skipping...\n",nco_prg_nm_get(),aed_ppc_lvl.att_nm,var_nm);
    } /* !conforming */
  } /* !rcd */
  /* Does container variable already exist? */
  rcd=nco_inq_varid_flg(nc_id,var_cnt_nm,&var_cnt_id);
  if(rcd != NC_NOERR){
    aed_sct aed_cnt_alg;
    aed_sct aed_cnt_fml;
    aed_sct aed_cnt_mpl;
    /* If not, create and populate it */
    rcd=nco_def_var(nc_id,var_cnt_nm,NC_CHAR,(int)0,(int *)NULL,&var_cnt_id);
    aed_cnt_alg.var_nm=aed_cnt_mpl.var_nm=var_cnt_nm;
    aed_cnt_alg.id=aed_cnt_fml.id=aed_cnt_mpl.id=var_cnt_id;
    aed_cnt_alg.att_nm=var_cnt_alg_nm;
    aed_cnt_alg.val.cp=aed_ppc_alg.att_nm;
    aed_cnt_alg.sz=strlen(aed_cnt_alg.val.cp);
    aed_cnt_alg.type=aed_cnt_fml.type=aed_cnt_mpl.type=NC_CHAR;
    aed_cnt_alg.mode=aed_cnt_fml.mode=aed_cnt_mpl.mode=aed_create;
    aed_cnt_fml.att_nm=var_cnt_fml_nm;
    aed_cnt_fml.val.cp=fml_val;
    aed_cnt_fml.sz=strlen(aed_cnt_fml.val.cp);
    aed_cnt_mpl.att_nm=var_cnt_mpl_nm;
    aed_cnt_mpl.val.cp=mpl_val_sng;
    aed_cnt_mpl.sz=strlen(aed_cnt_mpl.val.cp);
    (void)nco_aed_prc(nc_id,var_cnt_id,aed_cnt_fml);
    (void)nco_aed_prc(nc_id,var_cnt_id,aed_cnt_alg);
    (void)nco_aed_prc(nc_id,var_cnt_id,aed_cnt_mpl);
  } /* !rcd */

  return rcd;
  
} /* !nco_qnt_mtd() */
