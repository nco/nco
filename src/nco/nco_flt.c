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

/* Filter variables are file scope for now, could shift to global scope later if necessary */
static nco_flt_typ_enm nco_flt_glb_lsl_alg=nco_flt_dfl; /* [enm] Lossless compression algorithm */
static nco_flt_typ_enm nco_flt_glb_lsy_alg=nco_flt_nil; /* [enm] Lossy compression algorithm */
static int nco_flt_glb_lsl_lvl=NC_MIN_INT; /* [nbr] Lossless compression level */
static int nco_flt_glb_lsy_lvl=NC_MIN_INT; /* [nbr] Lossy compression level */

static int nco_flt_glb_nbr=0; /* [nbr] Number of codecs specified by user */
static nco_flt_typ_enm *nco_flt_glb_alg=NULL; /* [nbr] List of filters specified by user */
static int *nco_flt_glb_lvl=NULL; /* [nbr] List of compression levels for each filter */
static int *nco_flt_glb_prm_nbr=NULL; /* [nbr] List of parameter numbers for each filter */
static int **nco_flt_glb_prm=NULL; /* [nbr] List of lists of parameters for each filter */

/* Manipulate private compression algorithms through public interfaces */
nco_flt_typ_enm nco_flt_glb_lsl_alg_get(void){return nco_flt_glb_lsl_alg;} /* [enm] Lossless enum */
nco_flt_typ_enm nco_flt_glb_lsy_alg_get(void){return nco_flt_glb_lsy_alg;} /* [enm] Lossy enum */
int nco_flt_glb_lsl_lvl_get(void){return nco_flt_glb_lsl_lvl;} /* [enm] Lossless level */
int nco_flt_glb_lsy_lvl_get(void){return nco_flt_glb_lsy_lvl;} /* [enm] Lossy level */
void nco_flt_glb_lsl_alg_set(nco_flt_typ_enm nco_flt_lsl_alg){nco_flt_glb_lsl_alg=nco_flt_lsl_alg;} 
void nco_flt_glb_lsy_alg_set(nco_flt_typ_enm nco_flt_lsy_alg){nco_flt_glb_lsy_alg=nco_flt_lsy_alg;} 
void nco_flt_glb_lsl_lvl_set(int nco_flt_lsl_lvl){nco_flt_glb_lsl_lvl=nco_flt_lsl_lvl;} 
void nco_flt_glb_lsy_lvl_set(int nco_flt_lsy_lvl){nco_flt_glb_lsy_lvl=nco_flt_lsy_lvl;} 

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
(char * const cmp_sng, /* I [sng] Compression specification */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* Purpose: Parse and set global lossy/lossless compression settings

     Algorithm: 
     NCO accepts compression strings as comma-separated lists of couplets
     Couplet first value identifies compression algorithm, e.g., "Zstd","Deflate","BitGroom"
     Couplet second value, if present, is integer compression level
     Specify codecs in the desired order of their application

     Test:
     ncks --dbg=2 --cdc=gbr,3,zstd,1 in.nc out.nc */

  const char fnc_nm[]="nco_cmp_prs()";

  char **prm_lst; /* [sng] List of user-supplied filter parameters as strings */
  char **flt_lst; /* [sng] List of user-supplied filters as pipe-separated lists of comma-separated strings */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  int rcd=NCO_NOERR; /* [rcd] Return code */

  int prm_idx; /* [idx] Parameter index */
  int flt_idx; /* [idx] Filter index */
  int flt_nbr; /* [nbr] Filter number */
  int prm_nbr=0L; /* [nbr] Number of parameters */

  if(cmp_sng){

    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s reports requested codec string = %s\n",nco_prg_nm_get(),fnc_nm,cmp_sng);

    flt_lst=nco_lst_prs_1D(cmp_sng,"|",&flt_nbr);

    nco_flt_glb_nbr=flt_nbr;
    nco_flt_glb_alg=(nco_flt_typ_enm *)nco_malloc(nco_flt_glb_nbr*sizeof(nco_flt_typ_enm));
    nco_flt_glb_lvl=(int *)nco_malloc(nco_flt_glb_nbr*sizeof(int));
    nco_flt_glb_prm_nbr=(int *)nco_malloc(nco_flt_glb_nbr*sizeof(int));
    nco_flt_glb_prm=(int **)nco_malloc(nco_flt_glb_nbr*sizeof(int *));
      
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG %s reports codec string contains flt_nbr=%d codecs separated by \"|\"\n",nco_prg_nm_get(),fnc_nm,nco_flt_glb_nbr);

    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
      
      prm_lst=nco_lst_prs_1D(flt_lst[flt_idx],",",&prm_nbr);

      /* First element in list is always name/ID */
      nco_flt_glb_alg[flt_idx]=nco_flt_sng2enm(prm_lst[0]);

      /* Any remaining elements are filter parameters in the HDF5 sense */
      nco_flt_glb_prm_nbr[flt_idx]=prm_nbr-1;
      nco_flt_glb_prm[flt_idx]= (nco_flt_glb_prm_nbr[flt_idx] > 0) ? (int *)nco_malloc(nco_flt_glb_prm_nbr[flt_idx]*sizeof(int)) : NULL;

      if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s reports requested codec is \"%s\"\n",nco_prg_nm_get(),fnc_nm,nco_flt_enm2sng(nco_flt_glb_alg[flt_idx]));

      for(prm_idx=1;prm_idx<prm_nbr;prm_idx++){
	(void)fprintf(stdout,"%s: DEBUG flt_idx=%d prm_nbr=%d prm_idx=%d prm_val=%s\n",nco_prg_nm_get(),flt_idx,prm_nbr,prm_idx,prm_lst[prm_idx]);
	nco_flt_glb_prm[flt_idx][prm_idx-1]=(int)strtol(prm_lst[prm_idx],&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
	if(*sng_cnv_rcd) nco_sng_cnv_err(prm_lst[prm_idx],"strtol",sng_cnv_rcd);
	(void)fprintf(stdout,"%s: DEBUG %s post store \n",nco_prg_nm_get(),fnc_nm);
      } /* !prm_idx */
      (void)fprintf(stdout,"%s: DEBUG %s after prm_idx loop \n",nco_prg_nm_get(),fnc_nm);
    } /* !flt_idx */
    (void)fprintf(stdout,"%s: DEBUG %s after flt_idx loop \n",nco_prg_nm_get(),fnc_nm);
  } /* !cmp_sng */

  (void)fprintf(stdout,"%s: DEBUG %s why doesnt macos reach this spot?\n",nco_prg_nm_get(),fnc_nm);

  /* Allow use of traditional --dfl_lvl to set DEFLATE filter */
  if(nco_flt_glb_nbr == 0 && dfl_lvl != NCO_DFL_LVL_UNDEFINED){
    nco_flt_glb_nbr=1;
    nco_flt_glb_alg=(nco_flt_typ_enm *)nco_malloc(nco_flt_glb_nbr*sizeof(nco_flt_typ_enm));
    nco_flt_glb_lvl=(int *)nco_malloc(nco_flt_glb_nbr*sizeof(int));
    nco_flt_glb_prm_nbr=(int *)nco_malloc(nco_flt_glb_nbr*sizeof(int));
    nco_flt_glb_prm=(int **)nco_malloc(nco_flt_glb_nbr*sizeof(int *));

    nco_flt_glb_alg[0]=nco_flt_dfl;
    nco_flt_glb_prm_nbr[0]=1;
    nco_flt_glb_prm[0]=(int *)nco_malloc(nco_flt_glb_prm_nbr[0]*sizeof(int));
    nco_flt_glb_prm[0][0]=dfl_lvl;
    nco_flt_glb_lvl[0]=nco_flt_glb_prm[0][0];
  } /* !flt_nbr, !dfl_lvl */
    
  /* Allow user to mix --dfl_lvl and --cdc
     For example, --dfl_lvl=3 --cdc=zst means use Zstandard with compression level 3 */
  if(nco_flt_glb_nbr == 1 && nco_flt_glb_prm_nbr[0] == 0 && dfl_lvl != NCO_DFL_LVL_UNDEFINED){
    nco_flt_glb_prm_nbr[0]=1;
    nco_flt_glb_prm[0]=(int *)nco_malloc(nco_flt_glb_prm_nbr[0]*sizeof(int));
    nco_flt_glb_prm[0][0]=dfl_lvl;
    nco_flt_glb_lvl[0]=nco_flt_glb_prm[0][0];
  } /* !flt_nbr, !dfl_lvl */
  
  (void)fprintf(stdout,"%s: DEBUG exiting %s\n",nco_prg_nm_get(),fnc_nm);

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
nco_flt_id2sng /* [fnc] Convert compression filter ID to string */
(const unsigned int nco_flt_id) /* I [id] Compression filter ID */
{
  /* Purpose: Convert compression filter ID to descriptive string */
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
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG default case is in nco_flt_id2sng()\n",nco_prg_nm_get());
    nco_dfl_case_generic_err(); break;
  } /* !nco_flt_id */

  return (char *)NULL;
  
} /* !nco_flt_id2sng() */

nco_flt_typ_enm /* O [enm] Filter enum */
nco_flt_sng2enm /* [fnc] Convert user-specified filter string to NCO enum */
(const char *nco_flt_sng) /* [sng] User-specified filter string */
{
  /* Purpose: Convert user-specified string to enumerated filter type
     Return nco_flt_nil by default */

  const char fnc_nm[]="nco_flt_get()"; /* [sng] Function name */

  char *nco_prg_nm; /* [sng] Program name */

  nco_prg_nm=nco_prg_nm_get(); /* [sng] Program name */

  if(nco_flt_sng == NULL){
    /* 20140815: Change default from g2d to xst */
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s reports %s invoked without explicit filter string. Defaulting to \"nil\".\n",nco_prg_nm,fnc_nm,nco_prg_nm);
    return nco_flt_nil;
  } /* !nco_flt_sng */

  if(!strcasecmp(nco_flt_sng,"nil")) return nco_flt_nil;
  if(!strcasecmp(nco_flt_sng,"none")) return nco_flt_nil;
  if(!strcasecmp(nco_flt_sng,"default")) return nco_flt_nil;
  if(!strcasecmp(nco_flt_sng,"deflate")) return nco_flt_dfl;
  if(!strcasecmp(nco_flt_sng,"dfl")) return nco_flt_dfl;
  if(!strcasecmp(nco_flt_sng,"zlib")) return nco_flt_dfl;
  if(!strcasecmp(nco_flt_sng,"bzp")) return nco_flt_bz2;
  if(!strcasecmp(nco_flt_sng,"bzip")) return nco_flt_bz2;
  if(!strcasecmp(nco_flt_sng,"bzip2")) return nco_flt_bz2;
  if(!strcasecmp(nco_flt_sng,"lz4")) return nco_flt_lz4;
  if(!strcasecmp(nco_flt_sng,"bgr")) return nco_flt_bgr;
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

    /* fxm: Generalize to inquire about all compression options */
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
  nco_bool COPY_COMPRESSION_FROM_INPUT=False; /* [flg] Copy compression setting from input to output */
  char var_nm[NC_MAX_NAME+1L]; /* [sng] Variable name */
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

    /* fxm: Generalize to inquire about all compression options */
    rcd=nco_inq_var_deflate(nc_in_id,var_in_id_cpy,&shuffle,&deflate,&dfl_lvl_in);

    char *flt_sng=NULL;
    char sng_foo[12]; /* nbr] Maximum printed size of unsigned integer (4294967295) + 1 (for comma) + 1 (for trailing NUL) */
    char spr_sng[]="|"; /* [sng] Separator string between information for different filters */
    char var_nm[NC_MAX_NAME+1L]; /* [sng] Variable name */

    size_t flt_idx; /* [idx] Filter index */
    size_t flt_nbr; /* [nbr] Filter number */
    size_t prm_idx; /* [idx] Parameter index */
    size_t prm_nbr; /* [nbr] Parameter number */
    unsigned int *prm_lst=NULL;
    unsigned int *flt_lst=NULL; /* [nbr] Filter IDs */

    (void)fprintf(stdout,"DEBUG quark nco_flt.c 1\n");

    rcd=nco_inq_var_filter_ids(nc_in_id,var_in_id_cpy,&flt_nbr,NULL);
    flt_lst=(unsigned int *)nco_malloc(flt_nbr*sizeof(unsigned int));
    rcd=nco_inq_var_filter_ids(nc_in_id,var_in_id_cpy,(size_t *)NULL,flt_lst);
    flt_sng=(char *)nco_malloc(100L*sizeof(char));
    for(flt_idx=0;flt_idx<flt_nbr;flt_idx++){
      rcd=nco_inq_var_filter_info(nc_in_id,var_in_id_cpy,flt_lst[flt_idx],&prm_nbr,NULL);
      prm_lst=(unsigned int *)nco_malloc(prm_nbr*sizeof(unsigned int));
      rcd=nco_inq_var_filter_info(nc_in_id,var_in_id_cpy,flt_lst[flt_idx],NULL,prm_lst);
      (void)sprintf(sng_foo,"%u,",flt_lst[flt_idx]);
      strcat(flt_sng,sng_foo);
      for(prm_idx=0;prm_idx<prm_nbr;prm_idx++){
	strcat(flt_sng,sng_foo);
      } /* !prm_idx */
      if(flt_idx < flt_nbr-1) strcat(flt_sng,spr_sng);
      if(prm_lst) prm_lst=(unsigned int *)nco_free(prm_lst);
    } /* !flt_idx */
    
    if(nco_dbg_lvl_get() >= nco_dbg_fl){
      rcd=nco_inq_varname(nc_in_id,var_in_id_cpy,var_nm);
      (void)fprintf(stdout,"%s: DEBUG %s reports variable %s, flt_sng = %s\n",nco_prg_nm_get(),fnc_nm,var_nm,flt_sng);
      (void)fprintf(stdout,"%s: DEBUG %s reports variable %s, dfl_lvl = %d, dfl_lvl_in = %d\n",nco_prg_nm_get(),fnc_nm,var_nm,dfl_lvl,dfl_lvl_in);
    } /* !dbg */
    if(flt_sng) flt_sng=(char *)nco_free(flt_sng);
    
    (void)fprintf(stdout,"DEBUG quark nco_flt.c 2\n");

    /* Copy original filters if user did not explicity set dfl_lvl for output */ 
    if((deflate || shuffle) && dfl_lvl == NCO_DFL_LVL_UNDEFINED){
      /* Before netCDF 4.8.0, nco_def_var_deflate() could be called multiple times 
	 Properties of final invocation before nc_enddef() would take effect
	 After netCDF 4.8.0 first instance of nco_def_var_deflate() takes effect
	 It is therefore crucial not to call nco_def_var_deflate() more than once */
      rcd=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,dfl_lvl_in);
      if(rcd == NC_NOERR) COPY_COMPRESSION_FROM_INPUT=True;
    } /* !dfl_lvl */

  } /* !VARIABLE_EXISTS_IN_INPUT */

  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    rcd=nco_inq_varname(nc_out_id,var_out_id,var_nm);
    (void)fprintf(stdout,"%s: DEBUG %s reports variable %s, dfl_lvl = %d\n",nco_prg_nm_get(),fnc_nm,var_nm,dfl_lvl);
  } /* !dbg */

  /* If exotic codec option invoked, set dfl_lvl_cpy to spoof that dfl_lvl was user-modified 
     This enables the extended codec branch in nco_tst_def_out() */
  int nco_flt_lsl_lvl; /* [nbr] Lossless compression level */
  nco_flt_lsl_lvl=nco_flt_glb_lsl_lvl_get();
  if(nco_flt_lsl_lvl != NC_MIN_INT) dfl_lvl_cpy=nco_flt_lsl_lvl;
  
  /* Protect against calling nco_def_var_deflate() more than once */
  if(dfl_lvl_cpy != NCO_DFL_LVL_UNDEFINED && !COPY_COMPRESSION_FROM_INPUT)
    rcd=nco_tst_def_out(nc_out_id,var_out_id,dfl_lvl_cpy);

  return rcd;
  
} /* !nco_tst_def_wrp() */
  
int /* O [enm] Return code */
nco_tst_def_out /* [fnc]  */
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

  const char fnc_nm[]="nco_tst_def_out()"; /* [sng] Function name */

  int rcd=NC_NOERR; /* [rcd] Return code */

  /* Deflation */
  int deflate; /* [flg] Turn-on deflate filter */
  int shuffle; /* [flg] Turn-on shuffle filter */

  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    char var_nm[NC_MAX_NAME+1L];
    rcd=nco_inq_varname(nc_out_id,var_out_id,var_nm);
    (void)fprintf(stdout,"%s: DEBUG %s reports variable %s, dfl_lvl = %d\n",nco_prg_nm_get(),fnc_nm,var_nm,dfl_lvl);
  } /* !dbg */

  if(dfl_lvl != NCO_DFL_LVL_UNDEFINED){
    /* Overwrite HDF Lempel-Ziv compression level, if requested */
    deflate=(int)True;
    /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
    if(dfl_lvl <= 0) shuffle=NC_NOSHUFFLE;
    /* Shuffle never, to my knowledge, increases filesize, so shuffle by default when manually deflating (and do not shuffle when uncompressing) */
    if(dfl_lvl > 0) shuffle=NC_SHUFFLE;
#ifndef ENABLE_CCR
    /* If CCR is not enabled, perform normal deflation */
    rcd=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,dfl_lvl);
    return rcd;
#endif /* ENABLE_CCR */
  } /* !dfl_lvl */

  /* Build list of available filters */
  char cdc_lst[200]; /* [sng] List of available filters */
  nco_bool cdc_has_flt=True; /* [flg] Available filters include requested filter */
  cdc_lst[0]='\0';
  strcat(cdc_lst,"DEFLATE");
  /* Tokens like CCR_HAS_BITGROOM are defined by CCR in ccr_meta.h */
# if CCR_HAS_BZIP2 || NC_LIB_VER >= 490
  strcat(cdc_lst,", Bzip2");
# endif /* !CCR_HAS_BZIP2 */
# if CCR_HAS_LZ4
  strcat(cdc_lst,", LZ4");
# endif /* !CCR_HAS_LZ4 */
# if CCR_HAS_BITGROOM || NC_LIB_VER >= 490 
  strcat(cdc_lst,", BitGroom");
# endif /* !CCR_HAS_BITGROOM */
# if CCR_HAS_BITROUND || NC_LIB_VER >= 490 
  strcat(cdc_lst,", BitRound");
# endif /* !CCR_HAS_BITROUND */
# if CCR_HAS_GRANULARBR || NC_LIB_VER >= 490
  strcat(cdc_lst,", GranularBR");
# endif /* !CCR_HAS_GRANULARBR */
# if CCR_HAS_ZSTD || NC_LIB_VER >= 490
  strcat(cdc_lst,", Zstd");
# endif /* !CCR_HAS_ZSTD */
# if CCR_HAS_BLOSC_LZ || NC_LIB_VER >= 490
  strcat(cdc_lst,", BLOSC_LZ");
# endif /* !CCR_HAS_BLOSC_LZ */
# if CCR_HAS_BLOSC_LZ4 || NC_LIB_VER >= 490
  strcat(cdc_lst,", BLOSC LZ4");
# endif /* !CCR_HAS_BLOSC_LZ4 */
# if CCR_HAS_BLOSC_LZ4_HC || NC_LIB_VER >= 490
  strcat(cdc_lst,", BLOSC LZ4 HC");
# endif /* !CCR_HAS_BLOSC_LZ4_HC */
# if CCR_HAS_SNAPPY || NC_LIB_VER >= 490
  strcat(cdc_lst,", Snappy");
# endif /* !CCR_HAS_SNAPPY */
# if CCR_HAS_BLOSC_DEFLATE || NC_LIB_VER >= 490
  strcat(cdc_lst,", BLOSC DEFLATE");
# endif /* !CCR_HAS_BLOSC_DEFLATE */
# if CCR_HAS_BLOSC_ZSTANDARD || NC_LIB_VER >= 490
  strcat(cdc_lst,", BLOSC Zstandard");
# endif /* !CCR_HAS_BLOSC_ZSTANDARD */
  //(void)fprintf(stdout,"DEBUG quark: nco_flt_alg=%d, dfl_lvl=%d\n",(int)nco_flt_alg,dfl_lvl);

  int nco_flt_lsl_lvl; /* [nbr] Lossless compression level */
  int nco_flt_lsy_lvl; /* [nbr] Lossy compression level */
  nco_flt_typ_enm nco_flt_lsl_alg; /* [enm] Lossless compression algorithm */
  nco_flt_typ_enm nco_flt_lsy_alg; /* [enm] Lossy compression algorithm */
  nco_bool nco_flt_lsl_flg=False; /* [flg] Lossless filter is defined */
  nco_bool nco_flt_lsy_flg=False; /* [flg] Lossy filter is defined */

  nco_flt_lsl_alg=nco_flt_glb_lsl_alg_get();
  nco_flt_lsl_lvl=nco_flt_glb_lsl_lvl_get();
  nco_flt_lsy_alg=nco_flt_glb_lsy_alg_get();
  nco_flt_lsy_lvl=nco_flt_glb_lsy_lvl_get();

  if(nco_flt_lsl_alg != nco_flt_nil) nco_flt_lsl_flg=True;
  if(nco_flt_lsy_alg != nco_flt_nil) nco_flt_lsy_flg=True;

  int flt_idx; /* [idx] Filter index */
  int flt_nbr=0; /* [nbr] Number of filters to apply */
  if(nco_flt_lsl_flg) flt_nbr++;
  if(nco_flt_lsy_flg) flt_nbr++;

  /* Invoke applicable codec(s) 
     Ensure lossy filter, if any, is applied before lossless filter */
  //  const nco_flt_typ_enm nco_flt_enm=(nco_flt_typ_enm)nco_flt_glb_get();
  const int flt_nbr_max=2; /* [nbr] Maximum number of filters supported */
  nco_flt_typ_enm nco_flt_alg[flt_nbr_max]; /* [enm] Filter list */
  int nco_flt_lvl[flt_nbr_max]; /* [enm] Filter-level list */
  int flt_idx_srt; /* [idx] Index of first filter to apply */
  const int idx_lsy=0; /* [idx] index of lossy compressor in array */
  const int idx_lsl=1; /* [idx] index of lossless compressor in array */
  nco_flt_alg[idx_lsy]=nco_flt_lsy_alg;
  nco_flt_lvl[idx_lsy]=nco_flt_lsy_lvl;
  nco_flt_alg[idx_lsl]=nco_flt_lsl_alg;
  nco_flt_lvl[idx_lsl]=nco_flt_lsl_lvl;
  
  //(void)fprintf(stdout,"DEBUG quark1: nbr=%d, cdc_lst=%s\n",flt_nbr,cdc_lst);

  if(nco_flt_lsy_flg) flt_idx_srt=0; else flt_idx_srt=1;
  for(flt_idx=flt_idx_srt;flt_idx<flt_nbr_max;flt_idx++){ 
    //(void)fprintf(stdout,"DEBUG quark2: nbr=%d, idx=%d, flt_alg=%s, flt_lvl=%d\n",flt_nbr,flt_idx,nco_flt_enm2sng(nco_flt_alg[flt_idx]),nco_flt_lvl[flt_idx]);
    switch(nco_flt_alg[flt_idx]){
    case nco_flt_nil: /* If user did not select a filter then exit */
      break;

    case nco_flt_dfl: /* DEFLATE */
      rcd+=nco_def_var_deflate(nc_out_id,var_out_id,shuffle,deflate,nco_flt_lvl[flt_idx]);
      break;

    case nco_flt_bz2: /* Bzip2 */
# if CCR_HAS_BZIP2 || NC_LIB_VER >= 490 
      if(nco_flt_lvl[flt_idx] > 0) rcd+=nc_def_var_bzip2(nc_out_id,var_out_id,nco_flt_lvl[flt_idx]);
# else /* !CCR_HAS_BZIP2 || NC_LIB_VER >= 490 */
      cdc_has_flt=False;
# endif /* !CCR_HAS_BZIP2 || NC_LIB_VER >= 490 */
      break;

    case nco_flt_lz4: /* LZ4 */ 
# if CCR_HAS_LZ4
      if(nco_flt_lvl[flt_idx] > 0) rcd+=nc_def_var_lz4(nc_out_id,var_out_id,nco_flt_lvl[flt_idx]);
# else /* !CCR_HAS_LZ4 */
      cdc_has_flt=False;
# endif /* !CCR_HAS_LZ4 */
      break;

    case nco_flt_bgr: /* BitGroom */
# if NC_LIB_VER >= 490 
      if(nco_flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_BITGROOM,nco_flt_lvl[flt_idx]);
# elif CCR_HAS_BITGROOM
      if(nco_flt_lvl[flt_idx] > 0) rcd+=nc_def_var_bitgroom(nc_out_id,var_out_id,nco_flt_lvl[flt_idx]);
# else /* !CCR_HAS_BITGROOM */
      cdc_has_flt=False;
# endif /* !CCR_HAS_BITGROOM */
      break;

    case nco_flt_btr: /* BitRound */
# if NC_LIB_VER >= 490 
      if(nco_flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_BITROUND,nco_flt_lvl[flt_idx]);
# elif CCR_HAS_BITROUND
      if(nco_flt_lvl[flt_idx] > 0) rcd+=nc_def_var_bitround(nc_out_id,var_out_id,nco_flt_lvl[flt_idx]);
# else /* !CCR_HAS_BITROUND */
      cdc_has_flt=False;
# endif /* !CCR_HAS_BITROUND */
      break;

    case nco_flt_gbr: /* Granular BitRound */
# if NC_LIB_VER >= 490 
      if(nco_flt_lvl[flt_idx] > 0) rcd+=nc_def_var_quantize(nc_out_id,var_out_id,NC_QUANTIZE_GRANULARBR,nco_flt_lvl[flt_idx]);
# elif CCR_HAS_GRANULARBR
      if(nco_flt_lvl[flt_idx] > 0) rcd+=nc_def_var_granularbr(nc_out_id,var_out_id,nco_flt_lvl[flt_idx]);
# else /* !CCR_HAS_GRANULARBR */
      cdc_has_flt=False;
# endif /* !CCR_HAS_GRANULARBR */
      break;

    case nco_flt_zst: /* Zstandard */
# if CCR_HAS_ZSTD || NC_LIB_VER >= 490 
      /* NB: Zstandard accepts negative compression levels */
      rcd+=nc_def_var_zstandard(nc_out_id,var_out_id,nco_flt_lvl[flt_idx]);
# else /* !NC_LIB_VER >= 490 */
      cdc_has_flt=False;
# endif /* !CCR_HAS_ZSTD || NC_LIB_VER >= 490  */
      break;

    case nco_flt_dgr: /* DigitRound */
      cdc_has_flt=False;
      if(nco_flt_lvl[flt_idx] <= 0) break;

    default:
      nco_dfl_case_flt_err();
      break;
    } /* !nco_flt_alg */
  } /* !flt_idx */
  
  (void)fprintf(stdout,"%s: DEBUG %s quark8\n",nco_prg_nm_get(),fnc_nm);

  if(!cdc_has_flt){
    (void)fprintf(stdout,"%s: ERROR %s reports neither netCDF nor CCR library appears to define an API for requested filter \"%s\". If this filter name was not a typo, then probably this filter was not built and/or not installed in netCDF or in CCR. If the filter is newish and is supposed to be in CCR, update the installed CCR then recompile NCO. Otherwise, re-try this command and specify only filters included in this list of available filters: %s\n",nco_prg_nm_get(),fnc_nm,nco_flt_enm2sng(nco_flt_alg[flt_idx]),cdc_lst);
    nco_exit(EXIT_FAILURE);
  } /* !cdc_has_flt */
    
  (void)fprintf(stdout,"%s: DEBUG %s quark9\n",nco_prg_nm_get(),fnc_nm);

  return rcd;
  
} /* !nco_tst_def_out() */

