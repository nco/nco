/* $Header$ */

/* Purpose: Software configuration management */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_scm.h" /* Software configuration management */

char * /* O [sng] CVS version string */
cvs_vrs_prs(void) /* [fnc] Return CVS version string */
{
  /* Purpose: Return CVS version string */
  nco_bool dly_snp;

  char *cvs_mjr_vrs_sng=NULL;
  char *cvs_mnr_vrs_sng=NULL;
  char *cvs_nm_ptr=NULL;
  char *cvs_nm_sng=NULL;
  char *cvs_pch_vrs_sng=NULL;
  char *cvs_vrs_sng=NULL;
  char *dsh_ptr=NULL;
  char *dlr_ptr=NULL;
  char *nco_sng_ptr=NULL;
  char *usc_1_ptr=NULL;
  char *usc_2_ptr=NULL;
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */

  /* Unexpanded cvs keywords in cvs_Name trigger GCC 4.0+ warning
     "./src/nco/nco_scm.c:81: warning: offset outside bounds of constant string"
     because routine looks for cvs_Name+7 later on */
  char cvs_Name[]="$Name$";
  const char dlr_nm_cln_spc[]="$Name: "; /* [sng] Dollar name colon space */
  const char nco_sng[]="nco"; 
  const char spc_dlr[]=" $"; /* [sng] Space dollar */

  int cvs_nm_sng_len;
  int cvs_vrs_sng_len;
  int cvs_mjr_vrs_len;
  int cvs_mnr_vrs_len;
  int cvs_pch_vrs_len;
  int nco_sng_len;
  
  long cvs_mjr_vrs=-1L;
  long cvs_mnr_vrs=-1L;
  long cvs_pch_vrs=-1L;

  /* Is cvs_Name keyword expanded? */
  dlr_ptr=(char *)strstr(cvs_Name,spc_dlr);
  if(dlr_ptr == NULL && nco_dbg_lvl_get() >= nco_dbg_io)(void)fprintf(stderr,"%s: INFO cvs_vrs_prs() reports dlr_ptr == NULL\n%s: HINT Make sure CVS export uses -kkv\n",nco_prg_nm_get(),nco_prg_nm_get());
  cvs_nm_ptr=(char *)strstr(cvs_Name,dlr_nm_cln_spc);
  if(cvs_nm_ptr == NULL && nco_dbg_lvl_get() > nco_dbg_io)(void)fprintf(stderr,"%s: INFO cvs_vrs_prs() reports cvs_nm_ptr == NULL\n%s: HINT Make sure CVS export uses -kkv\n",nco_prg_nm_get(),nco_prg_nm_get());
  cvs_nm_sng_len=(int)(dlr_ptr-cvs_nm_ptr-strlen(dlr_nm_cln_spc)); /* 7 is strlen("$Name: ") */
  if(cvs_nm_sng_len > 0) dly_snp=False; else dly_snp=True;

  if(dly_snp){
    /* Presume this a daily snapshot, use YYYYMMDD date for version string */
    int mth;
    int day;
    int yr;
    struct tm *gmt_tm;
    time_t time_crr_time_t;

    time_crr_time_t=time((time_t *)NULL);
    gmt_tm=gmtime(&time_crr_time_t); 
    /* localtime() gives YYYYMMDD in e.g., PDT/MDT, but this conflicts with CVS, which uses GMT */
    /*    gmt_tm=localtime(&time_crr_time_t); */

    mth=gmt_tm->tm_mon+1;
    day=gmt_tm->tm_mday;
    yr=gmt_tm->tm_year+1900;

    cvs_vrs_sng_len=4+2+2;
    cvs_vrs_sng=(char *)nco_malloc((size_t)cvs_vrs_sng_len+1);
    (void)sprintf(cvs_vrs_sng,"%04i%02i%02i",yr,mth,day);
    return cvs_vrs_sng;
  } /* endif dly_snp */

  /* cvs_nm_sng is, e.g., "nco1_1" */
  cvs_nm_sng=(char *)nco_malloc((size_t)cvs_nm_sng_len+1);
  cvs_nm_sng=strncpy(cvs_nm_sng,cvs_Name+strlen(dlr_nm_cln_spc),(size_t)cvs_nm_sng_len); /* strlen("$Name: ") = 7 */
  cvs_nm_sng[cvs_nm_sng_len]='\0';

  /* cvs_vrs_sng is, e.g., "1.1" */
  nco_sng_len=strlen(nco_sng);
  nco_sng_ptr=strstr(cvs_nm_sng,nco_sng);
  if(nco_sng_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports nco_sng_ptr == NULL\n",nco_prg_nm_get());
  dsh_ptr=strstr(cvs_nm_sng,"-");
  if(dsh_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports dsh_ptr == NULL\n",nco_prg_nm_get());
  usc_1_ptr=strstr(cvs_nm_sng,"_");
  if(usc_1_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports usc_1_ptr == NULL\n",nco_prg_nm_get());
  cvs_mjr_vrs_len=(int)(usc_1_ptr-dsh_ptr)-1; /* NB: cast pointer to int before subtracting */
  usc_2_ptr=strstr(usc_1_ptr+1,"_");
  cvs_mjr_vrs_sng=(char *)nco_malloc((size_t)cvs_mjr_vrs_len+1);
  cvs_mjr_vrs_sng=strncpy(cvs_mjr_vrs_sng,cvs_nm_sng+nco_sng_len+1,(size_t)cvs_mjr_vrs_len);
  cvs_mjr_vrs_sng[cvs_mjr_vrs_len]='\0';
  cvs_mjr_vrs=strtol(cvs_mjr_vrs_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
  if(*sng_cnv_rcd) nco_sng_cnv_err(cvs_mjr_vrs_sng,"strtol",sng_cnv_rcd);
  if(usc_2_ptr == NULL){
    cvs_mnr_vrs_len=cvs_nm_sng_len-cvs_mjr_vrs_len-1;
    cvs_pch_vrs_len=0;
    cvs_vrs_sng_len=cvs_mjr_vrs_len+1+cvs_mnr_vrs_len;
  }else{
    cvs_mnr_vrs_len=usc_2_ptr-usc_1_ptr-1;
    cvs_pch_vrs_len=cvs_nm_sng_len-cvs_mjr_vrs_len-1-cvs_mnr_vrs_len-1;
    cvs_vrs_sng_len=cvs_mjr_vrs_len+1+cvs_mnr_vrs_len+1+cvs_pch_vrs_len;
  } /* end else */
  cvs_mnr_vrs_sng=(char *)nco_malloc((size_t)cvs_mnr_vrs_len+1);
  cvs_mnr_vrs_sng=strncpy(cvs_mnr_vrs_sng,usc_1_ptr+1,(size_t)cvs_mnr_vrs_len);
  cvs_mnr_vrs_sng[cvs_mnr_vrs_len]='\0';
  cvs_mnr_vrs=strtol(cvs_mnr_vrs_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
  if(*sng_cnv_rcd) nco_sng_cnv_err(cvs_mnr_vrs_sng,"strtol",sng_cnv_rcd);

  cvs_pch_vrs_sng=(char *)nco_malloc((size_t)cvs_pch_vrs_len+1);
  cvs_pch_vrs_sng[cvs_pch_vrs_len]='\0';
  cvs_vrs_sng=(char *)nco_malloc((size_t)cvs_vrs_sng_len+1);
  if(usc_2_ptr){
    cvs_pch_vrs_sng=strncpy(cvs_pch_vrs_sng,usc_2_ptr+1,(size_t)cvs_pch_vrs_len);
    cvs_pch_vrs=strtol(cvs_pch_vrs_sng,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
    if(*sng_cnv_rcd) nco_sng_cnv_err(cvs_pch_vrs_sng,"strtol",sng_cnv_rcd);
    (void)sprintf(cvs_vrs_sng,"%li.%li.%li",cvs_mjr_vrs,cvs_mnr_vrs,cvs_pch_vrs);
  }else{
    (void)sprintf(cvs_vrs_sng,"%li.%li",cvs_mjr_vrs,cvs_mnr_vrs);
  }/* end else */

  if(nco_dbg_lvl_get() >= nco_dbg_vrb){
    (void)fprintf(stderr,"NCO version %s\n",cvs_vrs_sng);
    (void)fprintf(stderr,"cvs_nm_sng %s\n",cvs_nm_sng);
    (void)fprintf(stderr,"cvs_mjr_vrs_sng %s\n",cvs_mjr_vrs_sng);
    (void)fprintf(stderr,"cvs_mnr_vrs_sng %s\n",cvs_mnr_vrs_sng);
    (void)fprintf(stderr,"cvs_pch_vrs_sng %s\n",cvs_pch_vrs_sng);
    (void)fprintf(stderr,"cvs_mjr_vrs %li\n",cvs_mjr_vrs);
    (void)fprintf(stderr,"cvs_mnr_vrs %li\n",cvs_mnr_vrs);
    (void)fprintf(stderr,"cvs_pch_vrs %li\n",cvs_pch_vrs);
  } /* endif dbg */

  cvs_mjr_vrs_sng=(char *)nco_free(cvs_mjr_vrs_sng);
  cvs_mnr_vrs_sng=(char *)nco_free(cvs_mnr_vrs_sng);
  cvs_pch_vrs_sng=(char *)nco_free(cvs_pch_vrs_sng);
  cvs_nm_sng=(char *)nco_free(cvs_nm_sng);

  return cvs_vrs_sng;
} /* end cvs_vrs_prs() */

void
nco_cpy_prn(void) /* [fnc] Print copyright notice */
{
  /* Purpose: Print copyright notice */
  (void)fprintf(stderr,"Copyright (C) 1995--2021 Charlie Zender\n");
  (void)fprintf(stdout,"This program is part of NCO, the netCDF Operators.\nNCO is free software and comes with a BIG FAT KISS and ABSOLUTELY NO WARRANTY\nYou may redistribute and/or modify NCO under the terms of the\n3-Clause BSD License with exceptions described in the LICENSE file\nBSD: https://opensource.org/licenses/BSD-3-Clause\nLICENSE: https://github.com/nco/nco/tree/master/LICENSE\n");
} /* end copyright_prn() */

void
nco_lbr_vrs_prn(void) /* [fnc] Print netCDF library version */
{
  /* Purpose: Print netCDF library version */

  char *cmp_dat_sng;
  char *dlr_ptr;
  char *lbr_sng;
  char *lbr_vrs_sng;
  char *of_ptr;

  size_t cmp_dat_sng_lng;
  size_t lbr_vrs_sng_lng;

  /* Behavior of nc_inq_libvers() depends on library versions. Return values are:
     netCDF 3.4--3.6.x: "3.4 of May 16 1998 14:06:16 $"
     netCDF 4.0-alpha1--4.0-alpha10: NULL
     netCDF 4.0-alpha11--4.0-alpha16: "4.0-alpha11"
     netCDF 4.0-beta1--4.4: "4.0-beta1"
     netCDF 4.4--present: "4.5.0-development" */  
  lbr_sng=(char *)strdup(nc_inq_libvers());
  /* (void)fprintf(stderr,"%s: nco_lbr_vrs_prn() returns %s\n",nco_prg_nm_get(),lbr_sng);*/
  of_ptr=strstr(lbr_sng," of ");
  if(of_ptr == NULL){
    (void)fprintf(stderr,"%s: WARNING nco_lbr_vrs_prn() reports of_ptr == NULL\n",nco_prg_nm_get());
    lbr_vrs_sng_lng=(size_t)strlen(lbr_sng);
  }else{
    lbr_vrs_sng_lng=(size_t)(of_ptr-lbr_sng);
  } /* endif */
  lbr_vrs_sng=(char *)nco_malloc(lbr_vrs_sng_lng+1ul);
  strncpy(lbr_vrs_sng,lbr_sng,lbr_vrs_sng_lng);
  lbr_vrs_sng[lbr_vrs_sng_lng]='\0'; /* NUL-terminate */

  dlr_ptr=strstr(lbr_sng," $");
  if(of_ptr && dlr_ptr){
    cmp_dat_sng_lng=(size_t)(dlr_ptr-of_ptr-4ul); /* 4 is the length of " of " */
    cmp_dat_sng=(char *)nco_malloc(cmp_dat_sng_lng+1ul);
    strncpy(cmp_dat_sng,of_ptr+4ul,cmp_dat_sng_lng); /* 4 is the length of " of " */
    cmp_dat_sng[cmp_dat_sng_lng]='\0'; /* NUL-terminate */
  }else{
    cmp_dat_sng=(char *)strdup("Unknown");
  } /* endif */

  (void)fprintf(stderr,"Linked to netCDF library version %s compiled %s\n",lbr_vrs_sng,cmp_dat_sng);

  cmp_dat_sng=(char *)nco_free(cmp_dat_sng);
  lbr_vrs_sng=(char *)nco_free(lbr_vrs_sng);
  lbr_sng=(char *)nco_free(lbr_sng);
} /* end nco_lbr_vrs_prn() */

const char * /* O [sng] Mnemonic that describes current NCO version */
nco_nmn_get(void) /* [fnc] Return mnemonic that describes current NCO version */
{ 
  /* Purpose: Return mnemonic describing current NCO version 
     20191221: ncremap/ncclimo print left quote and first word of this string, so one-word strings look best 
     20200117: fixed this limitation, multi-word versions work fine */
  return "Pt. Concepcion IPA";
} /* !nco_nmn_get() */

void
nco_vrs_prn /* [fnc] Print NCO version */
(const char * const CVS_Id, /* I [sng] CVS identification string */
 const char * const CVS_Revision) /* I [sng] CVS revision string */
{
  /* Purpose: Print NCO version */
  char *date_cvs=NULL; /* Date this file was last modified */
  char *vrs_rcs=NULL; /* Version of this file, e.g., 1.213 */
  char *vrs_cvs=NULL; /* Version according to CVS release tag */

  int date_cvs_lng;
  int vrs_cvs_lng;
  
  const char date_cpp[]=__DATE__; /* [sng] Date from C pre-processor */
  const char time_cpp[]=__TIME__; /* [sng] Time from C pre-processor */
  /*  const char time_cpp[]=__TIME__; *//* [sng] Time from C pre-processor */
  const char hst_cpp[]=TKN2SNG(HOSTNAME); /* [sng] Hostname from C pre-processor */
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */

  char vrs_cpp[]=TKN2SNG(NCO_VERSION); /* [sng] Version from C pre-processor */

  /* 20170417: vrs_cpp is typically something like "4.6.6-alpha09" (quotes included) 
     The quotation marks annoy me yet are necessary to protect the string in nco.h 
     Here we remove the quotation marks by pointing past the first and putting NUL in the last */
  char *vrs_sng; /* [sng] NCO version */
  vrs_sng=vrs_cpp;
  if(vrs_cpp[0L] == '"'){
    vrs_cpp[strlen(vrs_cpp)-1L]='\0';
    vrs_sng=vrs_cpp+1L;
  } /* endif */

  if(strlen(CVS_Id) > strlen("*Id*")){
    /* CVS_Id is defined */
    date_cvs_lng=10;
    date_cvs=(char *)nco_malloc((date_cvs_lng+1)*sizeof(char));
    (void)strncpy(date_cvs,strchr(CVS_Id,'/')-4,(size_t)date_cvs_lng);
    date_cvs[date_cvs_lng]='\0';
  }else{
    /* CVS_Id is undefined */
    date_cvs=(char *)strdup("Current");
  } /* endif */

  if(strlen(CVS_Revision) != strlen("*Revision*")){
    /* CVS_Revision is defined */
    vrs_cvs_lng=strrchr(CVS_Revision,'$')-strchr(CVS_Revision,':')-3L;
    vrs_rcs=(char *)nco_malloc((vrs_cvs_lng+1L)*sizeof(char));
    (void)strncpy(vrs_rcs,strchr(CVS_Revision,':')+2L,(size_t)vrs_cvs_lng);
    vrs_rcs[vrs_cvs_lng]='\0';
  }else{
    /* CVS_Revision is undefined */
    vrs_rcs=(char *)strdup("Current");
  } /* endif */

  if(strlen(CVS_Id) > strlen("*Id*")){
    (void)fprintf(stderr,"NCO netCDF Operators version %s last modified %s built %s on %s by %s\n",vrs_sng,date_cvs,date_cpp,hst_cpp,usr_cpp);
  }else{
    /* 20141008: Try new nco.h-based versioning */
    /*    (void)fprintf(stderr,"NCO netCDF Operators version %s built %s on %s by %s\n",vrs_sng,date_cpp,hst_cpp,usr_cpp);*/
    (void)fprintf(stderr,"NCO netCDF Operators version %s \"%s\" built by %s on %s at %s %s\n",vrs_sng,nco_nmn_get(),usr_cpp,hst_cpp,date_cpp,time_cpp);
  } /* endif */
  if(strlen(CVS_Id) > strlen("*Id*")){
    vrs_cvs=cvs_vrs_prs();
    (void)fprintf(stderr,"%s version %s\n",nco_prg_nm_get(),vrs_cvs);
  }else{
    (void)fprintf(stderr,"%s version %s\n",nco_prg_nm_get(),vrs_sng);
  } /* endif */

  if(date_cvs) date_cvs=(char *)nco_free(date_cvs);
  if(vrs_rcs) vrs_rcs=(char *)nco_free(vrs_rcs);
  if(vrs_cvs) vrs_cvs=(char *)nco_free(vrs_cvs);
} /* end nco_vrs_prn() */

