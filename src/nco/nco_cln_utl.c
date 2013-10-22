/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cln_utl.c,v 1.46 2013-10-22 03:03:45 zender Exp $ */

/* Purpose: Calendar utilities */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_cln_utl.h" /* Calendar utilities */

/* Arrays to hold calendar type units */
/* Format: year,month,day,hour,min,sec,origin,offset */   
double DATA_360[8]={31104000.0,2592000.0,86400.0,3600.0,60.0,1,0.0,0.0};
double DATA_365[8]={31536000.0,2628000.0,86400.0,3600.0,60.0,1,0.0,0.0};

/* Days in months */
int DAYS_PER_MONTH_360[12]={30,30,30,30,30,30,30,30,30,30,30,30};
int DAYS_PER_MONTH_365[12]={31,28,31,30,31,30,31,31,30,31,30,31};
int DAYS_PER_MONTH_366[12]={31,29,31,30,31,30,31,31,30,31,30,31};

int /* O [nbr] Number of days to end of month */
nco_nd2endm /* [fnc] Compute number of days to end of month */
(const int mth, /* I [mth] Month */
 const int day) /* I [day] Current day */
{
  /* Purpose: Returns number of days to end of month  
     This number added to the input argument day gives last day of month mth
     Original fortran: Brian Eaton cal_util.F:nd2endm()
     C version: Charlie Zender */
  const int mdays[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int nbr_day_2_mth_end;
  
  if(mth < 1 || mth > 12 || day < 0){
    (void)fprintf(stdout,"%s: ERROR nco_nd2endm() reports mth = %d, day = %d\n",nco_prg_nm_get(),mth,day);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  
  nbr_day_2_mth_end=mdays[mth-1]-day;
  
  return nbr_day_2_mth_end;
} /* nco_nd2endm */

nco_int /* O [YYMMDD] Date a specified number of days from input date */
nco_newdate /* [fnc] Compute date a specified number of days from input date */
(const nco_int date, /* I [YYMMDD] Date */
 const nco_int day_srt) /* I [day] Days ahead of input date */
{
  /* Purpose: Find date a specified number of days (possibly negative) from given date 
     Original fortran: Brian Eaton cal_util.F:newdate()
     C version: Charlie Zender */
  
  /* Local */
  const long mth_day_nbr[]= /* Number of days in each month */
    { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
      31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
  
  long day_nbr_2_eom; /* Days to end of month */
  long day_crr; /* Day of date */
  long day_ncr; /* Running count of days to increment date by */
  long mth_crr; /* Month of date */
  long mth_idx; /* Index */
  long mth_srt; /* Save the initial value of month */
  long mth_tmp; /* Current month as we increment date */
  long yr_crr; /* Year of date */
  
  nco_int date_srt; /* Initial value of date (may change sign) */
  nco_int newdate_YYMMDD; /* New date in YYMMDD format */
  
  if(day_srt == 0L) return date;
  
  date_srt=date;
  yr_crr=date_srt/10000L;
  if(date_srt < 0L) date_srt=-date_srt;
  mth_crr=(date_srt%10000L)/100L;
  mth_srt=mth_crr;
  day_crr=date_srt%100L;
  
  if(day_srt > 0){
    day_ncr=day_srt;
    yr_crr+=day_ncr/365L;
    day_ncr=day_ncr%365L;
    for(mth_idx=mth_srt;mth_idx<=mth_srt+12L;mth_idx++){
      mth_tmp=mth_idx;
      if(mth_idx > 12L) mth_tmp=mth_idx-12L;
      day_nbr_2_eom=(long int)nco_nd2endm(mth_tmp,day_crr);
      if(day_ncr > day_nbr_2_eom){
	mth_crr++;
	if(mth_crr > 12L){
	  mth_crr=1L;
	  yr_crr++;
	} /* end if */
	day_crr=1L;
	day_ncr-=day_nbr_2_eom+1L;
	if(day_ncr == 0L) break;
      }else{
	day_crr=day_crr+day_ncr;
	break;
      } /* end if */
    } /* end loop over mth */
    /* Previous two breaks continue execution here */
  }else if(day_srt < 0L){
    day_ncr=-day_srt;
    yr_crr=yr_crr-day_ncr/365L;
    day_ncr=day_ncr%365L;
    mth_srt=mth_crr;
    for(mth_idx=mth_srt+12L;mth_idx>=mth_srt;mth_idx--){
      if(day_ncr >= day_crr){
	mth_crr--;
	if(mth_crr < 1L){
	  mth_crr=12L;
	  yr_crr--;
	} /* end if */
	day_ncr-=day_crr;
	day_crr=mth_day_nbr[mth_crr-1L];
	if(day_ncr == 0L) break;
      }else{
	day_crr-=day_ncr;
	break;
      } /* end if */
    } /* end loop over mth */
    /* Previous two breaks continue execution here */
  } /* end if */
  
  if(yr_crr >= 0){
    newdate_YYMMDD=yr_crr*10000L+mth_crr*100L+day_crr;
  }else{
    newdate_YYMMDD=-yr_crr*10000L+mth_crr*100L+day_crr;
    newdate_YYMMDD=-newdate_YYMMDD;
  } /* end if */
  
  return newdate_YYMMDD;
} /* end nco_newdate() */

#ifdef ENABLE_UDUNITS
# ifdef HAVE_UDUNITS2_H

/* UDUnits2 routines */

int /* [rcd] Return code */
nco_cln_clc_dff /* [fnc] UDUnits2 Compute difference between two coordinate units */
(const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
 const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
 double crr_val,
 double *og_val) /* O [] Difference between two units strings */
{
  const char fnc_nm[]="nco_cln_clc_dff()"; /* [sng] Function name */
  
  cv_converter *ut_cnv; /* UDUnits converter */

  int ut_rcd; /* [enm] UDUnits2 status */
  
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* UDUnits structure, input units */
  ut_unit *ut_sct_out; /* UDUnits structure, output units */
  
  /* Quick return if units identical */
  if(!strcmp(fl_unt_sng,fl_bs_sng)){
    *og_val=crr_val;  
    return NCO_NOERR;
  } /* end if */
  
  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: %s() failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return NCO_ERR; /* Failure */
  } /* end if err */ 
  
  /* Units string to convert from */
  ut_sct_in=ut_parse(ut_sys,fl_unt_sng,UT_ASCII); 
  if(!ut_sct_in){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute \"%s\" has a syntax error\n",fl_unt_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_unt_sng);
    return NCO_ERR; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Units string to convert to */
  ut_sct_out=ut_parse(ut_sys,fl_bs_sng,UT_ASCII); 
  if(!ut_sct_out){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: Empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute  \"%s\" has a syntax error\n",fl_bs_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_bs_sng);
    return NCO_ERR; /* Failure */
  } /* endif */

  /* Create converter */
  ut_cnv=ut_get_converter(ut_sct_in,ut_sct_out); /* UDUnits converter */
  if(!ut_cnv){
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"WARNING: One of units, %s or %s, is NULL\n",fl_bs_sng,fl_unt_sng);
    if(ut_rcd == UT_NOT_SAME_SYSTEM) (void)fprintf(stderr,"WARNING: Units %s and %s belong to different unit systems\n",fl_bs_sng,fl_unt_sng);
    if(ut_rcd == UT_MEANINGLESS) (void)fprintf(stderr,"WARNING: Conversion between user-specified unit \"%s\" and file units \"%s\" is meaningless\n",fl_bs_sng,fl_unt_sng);
    return NCO_ERR; /* Failure */
  } /* endif */

  /* Convert */
  *og_val=cv_convert_double(ut_cnv,crr_val);
  
  if(nco_dbg_lvl_get() >= nco_dbg_var) fprintf(stderr, "%s: INFO %s() reports conversion between systems \"%s\" and \"%s\" is %f\n",nco_prg_nm_get(),fnc_nm,fl_unt_sng,fl_bs_sng,*og_val);

  ut_free(ut_sct_in);
  ut_free(ut_sct_out);
  cv_free(ut_cnv);
  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */

  return NCO_NOERR;
}  /* end UDUnits2 nco_cln_clc_dff() */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_prs_tm /* UDUnits2 Extract time stamp from parsed UDUnits string */
(const char *unt_sng, /* I [ptr] units attribute string */
 tm_cln_sct *tm_in) /*  O [sct] struct to be populated */             
{
  const char fnc_nm[]="nco_cln_prs_tm()"; /* [sng] Function name */

  char bfr[200];

  char *dt_sng;

  int ut_rcd; /* [enm] UDUnits2 status */

  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* UDUnits structure, input units */

  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: %s failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return NCO_ERR; /* Failure */
  } /* end if err */ 

  /* units string to convert from */
  ut_sct_in=ut_parse(ut_sys,unt_sng,UT_ASCII); 
  if(ut_sct_in == NULL){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR:  units attribute \"%s\" has a syntax error\n",unt_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",unt_sng);

    return NCO_ERR; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Print timestamp to buffer in standard, dependable format */
  ut_format(ut_sct_in,bfr,sizeof(bfr), UT_ASCII|UT_NAMES);

  /* Extract parsed time units from print string (kludgy) */
  dt_sng=strstr(bfr,"since");  
  sscanf(dt_sng,"%*s %d-%d-%d %d:%d:%f",&tm_in->year,&tm_in->month,&tm_in->day,&tm_in->hour,&tm_in->min,&tm_in->sec);

  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */
  ut_free(ut_sct_in);

  return NCO_NOERR;
} /* end UDUnits2 nco_cln_prs_tm() */

# else /* !HAVE_UDUNITS2_H */

/* UDUnits1 routines */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_clc_dff /* [fnc] UDUnits1 Difference between two co-ordinate units */
(const char *fl_unt_sng, /* I [ptr] units attribute string from disk  */     
 const char *fl_bs_sng, /* I [ptr] units attribute string from disk  */     
 double crr_val,
 double *og_val) /* O [ptr] */
{
  const char fnc_nm[]="nco_cln_clc_dff()"; /* [sng] Function name */
    
  double slp;
  double incpt;

  int rcd;

  utUnit udu_sct_in; /* UDUnits structure, input units */
  utUnit udu_sct_out; /* UDUnits structure, output units */

  /* Quick return if units identical */
  if(!strcmp(fl_unt_sng,fl_bs_sng) ){
    *og_val=crr_val;  
    return NCO_NOERR;
  } /* endif */

#ifdef UDUNITS_PATH
  /* UDUNITS_PATH macro expands to where autoconf found database file */
  rcd=utInit(UDUNITS_PATH);
#else /* !UDUNITS_PATH */
  /* When empty, utInit() uses environment variable UDUNITS_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  rcd=utInit("");
#endif /* !UDUNITS_PATH */

  if(rcd != UDUNITS_NOERR){
    (void)fprintf(stdout,"%s: %s failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return NCO_ERR;
  } /* end if err */ 

  /* units string to convert from */
  rcd=utScan(fl_unt_sng,&udu_sct_in); 
  if(rcd != UDUNITS_NOERR){
    if(rcd == UT_EINVALID) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is invalid \n",fl_unt_sng);
    if(rcd == UT_ESYNTAX) (void)fprintf(stderr,"ERROR units attribute \"%s\" contains a syntax error",fl_unt_sng);
    if(rcd == UT_EUNKNOWN) (void)fprintf(stderr,"ERROR units attribute \"%s\" is not in udunits database",fl_unt_sng);
    (void)utTerm(); /* Free memory taken by UDUnits library */
    return NCO_ERR;
  } /* endif unkown type */

  /* units string to convert to */
  rcd=utScan(fl_bs_sng,&udu_sct_out); 
  if(rcd != UDUNITS_NOERR){
    if(rcd == UT_EINVALID) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is invalid \n",fl_bs_sng);
    if(rcd == UT_ESYNTAX) (void)fprintf(stderr,"ERROR units attribute \"%s\" contains a syntax error",fl_bs_sng);
    if(rcd == UT_EUNKNOWN) (void)fprintf(stderr,"ERROR units attribute \"%s\" is not in udunits database",fl_bs_sng);
    (void)utTerm(); /* Free memory taken by UDUnits library */
    return NCO_ERR;
  } /* endif unkown type */

  rcd=utConvert(&udu_sct_in,&udu_sct_out,&slp,&incpt);
  if(rcd == UT_ECONVERT){
    (void)fprintf(stderr,"ERROR: user specified unit \"%s\" cannot be converted to units \"%s\"\n",fl_unt_sng,fl_bs_sng);
    (void)utTerm();
    return NCO_ERR;
  } /* endif */

  *og_val=crr_val*slp+incpt;

  /* debug stuff */
  if(nco_dbg_lvl_get() > nco_dbg_std) (void)fprintf(stderr,"%s: %s reports difference between systems \"%s\" and \"%s\" is %f\n",nco_prg_nm_get(),fnc_nm,fl_unt_sng,fl_bs_sng,*og_val);

  (void)utTerm();

  return NCO_NOERR;   
}  /* end UDUnits1 nco_cln_clc_dff() */

int /* [rcd] Successful conversion returns NCO_NOERR */     
nco_cln_prs_tm /* UDUnits1 Extract time stamp from a parsed udunits string */
(const char *unt_sng, /* I [ptr] units attribute string   */            
tm_cln_sct *tm_in) /*  O [sct] struct to be populated   */             
{
  const char fnc_nm[]="nco_cln_prs_tm()"; /* [sng] Function name */

  int rcd;

  utUnit udu_sct_in; /* UDUnits structure, input units */

#ifdef UDUNITS_PATH
  /* UDUNITS_PATH macro expands to where autoconf found database file */
  rcd=utInit(UDUNITS_PATH);
#else /* !UDUNITS_PATH */
  /* When empty, utInit() uses environment variable UDUNITS_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  rcd=utInit("");
#endif /* !UDUNITS_PATH */

  if(rcd != UDUNITS_NOERR){
    (void)fprintf(stdout,"%s: %s failed to initialize UDUnits library\n",nco_prg_nm_get(),fnc_nm);
    return NCO_ERR;
  } /* end if err */ 

  /* units string to convert from */
  rcd=utScan(unt_sng,&udu_sct_in); 
  if(rcd != UDUNITS_NOERR){
    if(rcd == UT_EINVALID) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is invalid \n",unt_sng);
    if(rcd == UT_ESYNTAX) (void)fprintf(stderr,"ERROR units attribute \"%s\" contains a syntax error",unt_sng);
    if(rcd == UT_EUNKNOWN) (void)fprintf(stderr,"ERROR units attribute \"%s\" is not in udunits database",unt_sng);
    (void)utTerm(); /* Free memory taken by UDUnits library */
    return NCO_ERR;
  } /* endif unkown type */

  /* Extract time origin */
  if(utIsTime(&udu_sct_in)){
    utCalendar(0.0,&udu_sct_in,&tm_in->year,&tm_in->month,&tm_in->day,&tm_in->hour,&tm_in->min,&tm_in->sec);
    rcd=NCO_NOERR;  
  }else{
    rcd=NCO_ERR;
  } /* endelse */

 (void)utTerm(); /* Free memory taken by UDUnits library */
 return rcd;

} /* end UDUnits1 nco_cln_prs_tm() */

# endif /*!HAVE_UDUNITS2 */

#else /* !ENABLE_UDUNITS */

/* No UDUnits implementation available */

/* Stubs to enable compilation without UDUnits */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_clc_dff( /* [fnc] Difference between two co-ordinate units */      
const char *fl_unt_sng, /* I [ptr] units attribute string from disk  */     
const char *fl_bs_sng, /* I [ptr] units attribute string from disk  */     
double crr_val,
double *og_val){ /* O [ptr] */
return NCO_NOERR;
}

int /* [rcd] Successful conversion returns NCO_NOERR */     
nco_cln_prs_tm( /* Extract time stamp from a parsed UDUnits string */
const char *unt_sng, /* I [ptr] units attribute string */
tm_cln_sct *tm_in){ /* O [sct] struct to be populated */             
return NCO_NOERR;
}

#endif /* !ENABLE_UDUNITS */

/* End UDUnits-related routines*/
 
tm_typ /* [enum] Units type */    
nco_cln_get_tm_typ /* Returns time unit type or tm_void if not found */
(const char *ud_sng){ /* I [ptr] units string  */      
  int idx;
  int len; 
  char *lcl_sng;  
  tm_typ rcd_typ;
  
  lcl_sng=strdup(ud_sng);
  
  /* Initially set ret type to void */
  rcd_typ=tm_void;   
  
  /* Convert to lower case */
  len=strlen(lcl_sng);
  for(idx=0;idx<len;idx++) lcl_sng[idx]=tolower(lcl_sng[idx]);
  
  if(!strcmp(lcl_sng,"year") || !strcmp(lcl_sng,"years")) rcd_typ=tm_year;
  else if(!strcmp(lcl_sng,"month") || !strcmp(lcl_sng,"months")) rcd_typ=tm_month;
  else if(!strcmp(lcl_sng,"day") || !strcmp(lcl_sng,"days")) rcd_typ=tm_day;
  else if(!strcmp(lcl_sng,"hour") || !strcmp(lcl_sng,"hours")) rcd_typ=tm_hour;
  else if(!strcmp(lcl_sng,"min") || !strcmp(lcl_sng,"mins") || !strcmp(lcl_sng,"minute") || !strcmp(lcl_sng,"minutes")) rcd_typ=tm_min;
  else if(!strcmp(lcl_sng,"sec") || !strcmp(lcl_sng,"secs") || !strcmp(lcl_sng,"second") || !strcmp(lcl_sng,"seconds")) rcd_typ=tm_sec;
  
  if(lcl_sng) lcl_sng=(char *)nco_free(lcl_sng);
  return rcd_typ;
} /* end nco_cln_get_tm_typ() */

nco_cln_typ /* [enum] Calendar type */    
nco_cln_get_cln_typ /*  [fnc]  Calendar type or cln_nil if not found */
(const char *ud_sng) /* I [ptr] units string  */      
{
  int idx;
  int len; 
  char *lcl_sng;  
  nco_cln_typ rcd_typ;
  
  if(!ud_sng) return cln_nil;
  
  lcl_sng=strdup(ud_sng);
  
  /* Initially set return type to void */
  rcd_typ=cln_nil;
  
  /* Convert to lower case */
  len=strlen(lcl_sng);
  for(idx=0;idx<len;idx++) lcl_sng[idx]=tolower(lcl_sng[idx]);
  
  if(strstr(lcl_sng,"standard")) rcd_typ=cln_std;
  else if(strstr(lcl_sng,"gregorian") || strstr(lcl_sng,"proleptic_gregorian")) rcd_typ=cln_grg;
  else if(strstr(lcl_sng,"julian")) rcd_typ=cln_jul;
  else if(strstr(lcl_sng,"360_day")) rcd_typ=cln_360;
  else if(strstr(lcl_sng,"noleap") || strstr(lcl_sng,"365_day")) rcd_typ=cln_365;
  else if(strstr(lcl_sng,"all_leap") || strstr(lcl_sng,"366_day")) rcd_typ=cln_366;

  lcl_sng=(char *)nco_free(lcl_sng);
  
  return rcd_typ;
} /* end nco_cln_get_cln_typ() */

int /* O [nbr] Number of days */
nco_cln_days_in_year_prior_to_given_month /* [fnc] Number of days in year prior to month */
(nco_cln_typ lmt_cln, /* [enm] Calendar type */
 int mth_idx) /* I [idx] Month (1-based counting, December == 12) */
{ 
  int *days=NULL_CEWI;
  int idx;
  int idays=0;
  
  switch(lmt_cln){
  case cln_360:
    days=DAYS_PER_MONTH_360;
    break; 
  case cln_365:
    days=DAYS_PER_MONTH_365;
    break; 
  case cln_366: 
    days=DAYS_PER_MONTH_366;
    break;  
  case cln_std:
  case cln_grg:
  case cln_jul:
  case cln_nil:
    break;
  } /* end switch */
  
  mth_idx--;
  
  for(idx=0;idx<mth_idx;idx++) idays+=days[idx];
  
  return idays;
} /* end nco_cln_days_in_year_prior_to_given_month() */

void
nco_cln_pop_val /* [fnc] Calculate value in cln_sct */ 
(tm_cln_sct *cln_sct) /* I/O [ptr] structure */
{
  double *data;
  
  switch(cln_sct->sc_cln){
  case cln_360:
    data=DATA_360;
    cln_sct->value=data[0]*(cln_sct->year-1)+
      data[1]*(cln_sct->month-1)+
      data[2]*(cln_sct->day-1)+
      data[3]*cln_sct->hour+
      data[4]*cln_sct->min+
      data[5]*(double)cln_sct->sec;
    break; 
  case cln_365:  
    data=DATA_365;    
    cln_sct->value=data[0]*(cln_sct->year-1)+
      data[2]*nco_cln_days_in_year_prior_to_given_month(cln_365,cln_sct->month)+
      data[2]*(cln_sct->day-1)+
      data[3]*cln_sct->hour+
      data[4]*cln_sct->min+
      data[5]*(double)cln_sct->sec;
    break;
  case cln_366:
    break;
  case cln_std:
  case cln_grg:
  case cln_jul:
  case cln_nil:
    break;
  }
  
  return;
} /* end nco_cln_pop_val() */

double /* O [dbl] relative time */
nco_cln_rel_val
(double offset, /* I [dbl] time in base units */
 nco_cln_typ lmt_cln, /* I [enm] Calendar type */ 
 tm_typ bs_tm_typ) /* I [enm] Time units */
{
  double *data=NULL_CEWI;
  double scl=double_CEWI;
  
  switch(lmt_cln) {
  case cln_360:
    data=DATA_360;    
    break; 
  case cln_365:  
    data=DATA_365;    
    break;
  case cln_366:
    break;
  case cln_std:
  case cln_grg:
  case cln_jul:
  case cln_nil:
    break;
  }
  
  /* Switch for type */
  switch(bs_tm_typ){ 
  case tm_year:    
    scl=data[0];
    break;   
  case tm_month:
    scl=data[1];
    break;   
  case tm_day:
    scl=data[2];
    break;   
  case tm_hour:
    scl=data[3];
    break;   
  case tm_min:
    scl=data[4];
    break;   
  case tm_sec:
  case tm_void: 
    scl=data[5];
    break;     
  } /* end switch */ 
  
  return offset/scl;
} /* end nco_cln_rel_val() */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_clc_tm /* [fnc] Difference between two coordinate units */
(const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
 const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
 nco_cln_typ lmt_cln, /* [enum] Calendar type of coordinate var */ 
 double *og_val){ /* O [ptr] */
  
  int rcd;
  int year;
  int month;
  char *lcl_unt_sng;
  char tmp_sng[100];
  double crr_val;
  
  tm_typ bs_tm_typ; /* enum for units type in fl_bs_sng */
  tm_cln_sct unt_cln_sct;
  tm_cln_sct bs_cln_sct;
  
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: nco_cln_clc_tm() reports unt_sng=%s bs_sng=%s\n",nco_prg_nm_get(),fl_unt_sng,fl_bs_sng);
  
  /* Does fl_unt_sng look like a regular timestamp? */ 
  if(sscanf(fl_unt_sng,"%d-%d",&year,&month) == 2){
    lcl_unt_sng=(char *)nco_malloc((strlen(fl_unt_sng)+3L)*sizeof(char));
    strcpy(lcl_unt_sng,"s@");
    strcat(lcl_unt_sng,fl_unt_sng);
  }else{
    lcl_unt_sng=strdup(fl_unt_sng);
  } /* endelse */
  
  /* Temporary until we handle more calendar types */
  if(lmt_cln != cln_360 && lmt_cln != cln_365){
    rcd=nco_cln_clc_dff(lcl_unt_sng,fl_bs_sng,0.0,og_val);
    lcl_unt_sng=(char *)nco_free(lcl_unt_sng);
    return rcd;
  } /* endif */

  /* Obtain units type from fl_bs_sng */
  if(sscanf(fl_bs_sng,"%s",tmp_sng) != 1) return NCO_ERR;
  
  bs_tm_typ=nco_cln_get_tm_typ(tmp_sng);  
  
  /* Assume non-standard calendar */ 
  if(nco_cln_prs_tm(lcl_unt_sng,&unt_cln_sct) == NCO_ERR) return NCO_ERR;
  
  if(nco_cln_prs_tm(fl_bs_sng,&bs_cln_sct) == NCO_ERR) return NCO_ERR;
  
  unt_cln_sct.sc_typ=bs_tm_typ;
  bs_cln_sct.sc_typ=bs_tm_typ;
  
  unt_cln_sct.sc_cln=lmt_cln;
  bs_cln_sct.sc_cln=lmt_cln;
  
  (void)nco_cln_pop_val(&unt_cln_sct);
  (void)nco_cln_pop_val(&bs_cln_sct);
  
  crr_val=nco_cln_rel_val(unt_cln_sct.value-bs_cln_sct.value,lmt_cln,bs_tm_typ);                 
  
  *og_val=crr_val;
  
  lcl_unt_sng=(char *)nco_free(lcl_unt_sng);
  
  return NCO_NOERR;
} /* end nco_cln_clc_tm() */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_clc_org /* [fnc] Difference between two generic co-ordinate units */
(const char *fl_unt_sng, /* I [ptr] Source value (optional) and source units */
 const char *fl_bs_sng, /* I [ptr] Target units */
 nco_cln_typ lmt_cln, /* [enm] Calendar type, if any, of coordinate variable */ 
 double *og_val) /* O [ptr] Target value in units stored on disk */
{
  /* Purpose:
     Given a value expressed source units (fl_unit_sng) and target units to switch to, 
     determine and return the value expressed in the target units. */

  int rcd;
  
  char *usr_unt_sng;   
  double crr_val;
  
  /* If units contain date or timestamp call special time-conversion routine */
  if(strstr(fl_bs_sng," from ") || strstr(fl_bs_sng," since ") || strstr(fl_bs_sng," after ")){
    rcd=nco_cln_clc_tm(fl_unt_sng,fl_bs_sng,lmt_cln,og_val);   
    return rcd;
  } /* endif */
  
  /* Regular conversion of fl_unt_sng of form <double_value units>, e.g., '10 inches', '100 ft' */
  usr_unt_sng=(char *)nco_calloc(strlen(fl_unt_sng)+1L, sizeof(char));
  sscanf(fl_unt_sng,"%lg %s",&crr_val,usr_unt_sng);
  rcd=nco_cln_clc_dff(usr_unt_sng,fl_bs_sng,crr_val,og_val);
  usr_unt_sng=(char *)nco_free(usr_unt_sng);
  return rcd;  
} /* end nco_cln_clc_org() */
