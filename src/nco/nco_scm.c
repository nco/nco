/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_scm.c,v 1.5 2002-08-21 11:47:42 zender Exp $ */

/* Purpose: Software configuration management */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_scm.h" /* Software configuration management */

char * /* O [sng] CVS version string */
cvs_vrs_prs(void) /* [fnc] Return CVS version string */
{
  /* Purpose: Return CVS version string */
  bool dly_snp;

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
  char cvs_Name[]="$Name: not supported by cvs2svn $";
  char nco_sng[]="nco"; 

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
  dlr_ptr=strstr(cvs_Name," $");
  if(dlr_ptr == NULL && dbg_lvl_get() > 3)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports dlr_ptr == NULL\n%s: HINT Make sure CVS export uses -kkv\n",prg_nm_get(),prg_nm_get());
  cvs_nm_ptr=strstr(cvs_Name,"$Name: ");
  if(cvs_nm_ptr == NULL && dbg_lvl_get() > 3)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports cvs_nm_ptr == NULL\n%s: HINT Make sure CVS export uses -kkv\n",prg_nm_get(),prg_nm_get());
  cvs_nm_sng_len=(int)(dlr_ptr-cvs_nm_ptr-7); /* 7 is strlen("$Name: ") */
  if(cvs_nm_sng_len > 0) dly_snp=False; else dly_snp=True;

  /* If not, this is daily snapshot so use YYYYMMDD date for version string */
  if(dly_snp){
    int mth;
    int day;
    int yr;
    struct tm *gmt_tm;
    time_t clock;

    clock=time((time_t *)NULL);
    gmt_tm=gmtime(&clock); 
    /* localtime() gives YYYYMMDD in MDT, but this conflicts with CVS, which uses GMT */
    /*    gmt_tm=localtime(&clock); */

    mth=gmt_tm->tm_mon+1;
    day=gmt_tm->tm_mday;
    yr=gmt_tm->tm_year+1900;

    cvs_vrs_sng_len=4+2+2;
    cvs_vrs_sng=(char *)nco_malloc(cvs_vrs_sng_len+1);
    (void)sprintf(cvs_vrs_sng,"%04i%02i%02i",yr,mth,day);
    return cvs_vrs_sng;
  } /* endif dly_snp */

  /* cvs_nm_sng is, e.g., "nco1_1" */
  cvs_nm_sng=(char *)nco_malloc(cvs_nm_sng_len+1);
  strncpy(cvs_nm_sng,cvs_Name+7,cvs_nm_sng_len); /* 7 is strlen("$Name: ") */
  cvs_nm_sng[cvs_nm_sng_len]='\0';

  /* cvs_vrs_sng is, e.g., "1.1" */
  nco_sng_len=strlen(nco_sng);
  nco_sng_ptr=strstr(cvs_nm_sng,nco_sng);
  if(nco_sng_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports nco_sng_ptr == NULL\n",prg_nm_get());
  dsh_ptr=strstr(cvs_nm_sng,"-");
  if(dsh_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports dsh_ptr == NULL\n",prg_nm_get());
  usc_1_ptr=strstr(cvs_nm_sng,"_");
  if(usc_1_ptr == NULL)(void)fprintf(stderr,"%s: WARNING cvs_vrs_prs() reports usc_1_ptr == NULL\n",prg_nm_get());
  cvs_mjr_vrs_len=(int)(usc_1_ptr-dsh_ptr)-1; /* NB: cast pointer to int before subtracting */
  usc_2_ptr=strstr(usc_1_ptr+1,"_");
  cvs_mjr_vrs_sng=(char *)nco_malloc(cvs_mjr_vrs_len+1);
  cvs_mjr_vrs_sng=strncpy(cvs_mjr_vrs_sng,cvs_nm_sng+nco_sng_len+1,cvs_mjr_vrs_len);
  cvs_mjr_vrs_sng[cvs_mjr_vrs_len]='\0';
  cvs_mjr_vrs=strtol(cvs_mjr_vrs_sng,(char **)NULL,10);
  if(usc_2_ptr == NULL){
    cvs_mnr_vrs_len=cvs_nm_sng_len-cvs_mjr_vrs_len-1;
    cvs_pch_vrs_len=0;
    cvs_vrs_sng_len=cvs_mjr_vrs_len+1+cvs_mnr_vrs_len;
  }else{
    cvs_mnr_vrs_len=usc_2_ptr-usc_1_ptr-1;
    cvs_pch_vrs_len=cvs_nm_sng_len-cvs_mjr_vrs_len-1-cvs_mnr_vrs_len-1;
    cvs_vrs_sng_len=cvs_mjr_vrs_len+1+cvs_mnr_vrs_len+1+cvs_pch_vrs_len;
  } /* end else */
  cvs_mnr_vrs_sng=(char *)nco_malloc(cvs_mnr_vrs_len+1);
  cvs_mnr_vrs_sng=strncpy(cvs_mnr_vrs_sng,usc_1_ptr+1,cvs_mnr_vrs_len);
  cvs_mnr_vrs_sng[cvs_mnr_vrs_len]='\0';
  cvs_mnr_vrs=strtol(cvs_mnr_vrs_sng,(char **)NULL,10);

  cvs_pch_vrs_sng=(char *)nco_malloc(cvs_pch_vrs_len+1);
  cvs_pch_vrs_sng[cvs_pch_vrs_len]='\0';
  cvs_vrs_sng=(char *)nco_malloc(cvs_vrs_sng_len+1);
  if(usc_2_ptr != NULL){
    cvs_pch_vrs_sng=strncpy(cvs_pch_vrs_sng,usc_2_ptr+1,cvs_pch_vrs_len);
    cvs_pch_vrs=strtol(cvs_pch_vrs_sng,(char **)NULL,10);
    (void)sprintf(cvs_vrs_sng,"%li.%li.%li",cvs_mjr_vrs,cvs_mnr_vrs,cvs_pch_vrs);
  }else{
    (void)sprintf(cvs_vrs_sng,"%li.%li",cvs_mjr_vrs,cvs_mnr_vrs);
  }/* end else */

  if(dbg_lvl_get() == 4){
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
copyright_prn /* [fnc] Print copyright notice */
(const char * const CVS_Id, /* I [sng] CVS identification string */
 const char * const CVS_Revision) /* I [sng] CVS revision string */
{
  /* Purpose: Print copyright notice */
  char *date_cvs; /* Date this file was last modified */
  char *vrs_rcs; /* Version of this file, e.g., 1.213 */
  char *vrs_cvs; /* Version according to CVS release tag */

  int date_cvs_lng;
  int vrs_cvs_lng;
  
  /* C pre-processor macros for instantiating variable values with string tokens */
#define XTKN2SNG(x) #x
#define TKN2SNG(x) XTKN2SNG(x)
  const char date_cpp[]=__DATE__; /* [sng] Date from C pre-processor */
  /*  const char time_cpp[]=__TIME__; *//* [sng] Time from C pre-processor */
  const char vrs_cpp[]=TKN2SNG(VERSION); /* [sng] Version from C pre-processor */
  const char hst_cpp[]=TKN2SNG(HOSTNAME); /* [sng] Hostname from C pre-processor */
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */

  if(strlen(CVS_Id) > strlen("*Id*")){
    /* CVS_Id is defined */
    date_cvs_lng=10;
    date_cvs=(char *)nco_malloc((date_cvs_lng+1)*sizeof(char));
    (void)strncpy(date_cvs,strchr(CVS_Id,'/')-4,date_cvs_lng);
    date_cvs[date_cvs_lng]='\0';
  }else{
    /* CVS_Id is undefined */
    date_cvs=(char *)strdup("Current");
  } /* endif */

  if(strlen(CVS_Revision) > strlen("*Revision*") || strlen(CVS_Revision) < strlen("*Revision*")){
    /* CVS_Revision is defined */
    vrs_cvs_lng=strrchr(CVS_Revision,'$')-strchr(CVS_Revision,':')-3;
    vrs_rcs=(char *)nco_malloc((vrs_cvs_lng+1)*sizeof(char));
    (void)strncpy(vrs_rcs,strchr(CVS_Revision,':')+2,vrs_cvs_lng);
    vrs_rcs[vrs_cvs_lng]='\0';
  }else{
    /* CVS_Revision is undefined */
    vrs_rcs=(char *)strdup("Current");
  } /* endif */

  vrs_cvs=cvs_vrs_prs();

  if(strlen(CVS_Id) > strlen("*Id*")){
    (void)fprintf(stderr,"%s version %s last modified %s built %s on %s by %s\n",prg_nm_get(),vrs_cvs,date_cvs,date_cpp,hst_cpp,usr_cpp);
  }else{
    (void)fprintf(stderr,"%s version %s built %s on %s by %s\n",prg_nm_get(),vrs_cpp,date_cpp,hst_cpp,usr_cpp);
  } /* endif */

  /*  (void)fprintf(stderr,"NCO netCDF Operators version %s by Charlie Zender\n",vrs_cvs);
  (void)fprintf(stderr,"%s version %s (%s) \"%s\"\n",prg_nm_get(),vrs_rcs,date_cvs,nmn_get());
  (void)fprintf(stderr,"Copyright 1995--1999 University Corporation for Atmospheric Research\n");
  (void)fprintf(stderr,"Portions copyright 1999--2000 Regents of the University of California\n"); */

  (void)fprintf(stderr,"NCO netCDF Operators version %s\n",vrs_cvs);
  (void)fprintf(stderr,"Copyright (C) 1995--2002 Charlie Zender\n");
  (void)fprintf(stderr,"%s version %s (%s) \"%s\"\n",prg_nm_get(),vrs_rcs,date_cvs,nmn_get());
  (void)fprintf(stdout,"NCO is free software and comes with ABSOLUTELY NO WARRANTY\nNCO is distributed under the terms of the GNU General Public License\n");

  vrs_rcs=(char *)nco_free(vrs_rcs);
  vrs_cvs=(char *)nco_free(vrs_cvs);
} /* end copyright_prn() */

char * /* O [sng] Mnemonic that describes current NCO version */
nmn_get(void) /* [fnc] Return mnemonic that describes current NCO version */
{ 
  /* Purpose: Return mnemonic that describes current NCO version */
  return "Autobahn";
} /* end nmn_get() */

