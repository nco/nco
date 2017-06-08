/* $Header$ */

/* Purpose: Calendar utilities */

/* Copyright (C) 1995--2017 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_cln_utl.h" /* Calendar utilities */
#include "libnco.h"
#include "nco.h"

/* Arrays to hold calendar type units */
/* Format: year, month, day, hour, minute, second, origin, offset */
/* origin for all calendars is 2001-01-01 (seconds)  (same as origin for udunits xalendar) */
static  double DATA_360[8]={31104000.0,2592000.0,86400.0,3600.0,60.0,1.0,31104000.0*2001.0,0.0};
static double DATA_365[8]={31536000.0,2628000.0,86400.0,3600.0,60.0,1.0,31536000.0*2001.0,0.0};
static double DATA_366[8]={31622400.0,2635200.0,86400.0,3600.0,60.0,1.0,63276422400.0,0.0};

/* Days in months */
static int DAYS_PER_MONTH_360[12]={30,30,30,30,30,30,30,30,30,30,30,30};
static int DAYS_PER_MONTH_365[12]={31,28,31,30,31,30,31,31,30,31,30,31};
static int DAYS_PER_MONTH_366[12]={31,29,31,30,31,30,31,31,30,31,30,31};

/* Size of temporary buffer used in parsing calendar dates */
#define NCO_MAX_LEN_TMP_SNG 200

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
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
     31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  
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

tm_typ /* O [enm] Units type */
nco_cln_get_tm_typ /* Returns time unit type or tm_void if not found */
(const char *ud_sng){ /* I [ptr] Units string  */
  size_t len; 
  char *lcl_sng;  
  tm_typ rcd_typ;
  
  lcl_sng=strdup(ud_sng);
  
  /* Set initial return type to void and overwrite */
  rcd_typ=tm_void;   
  
  /* Convert to lower case */
  len=strlen(lcl_sng);
  for(size_t idx=0;idx<len;idx++) lcl_sng[idx]=tolower(lcl_sng[idx]);
  
  if(!strcmp(lcl_sng,"year") || !strcmp(lcl_sng,"years")) rcd_typ=tm_year;
  else if(!strcmp(lcl_sng,"month") || !strcmp(lcl_sng,"months")) rcd_typ=tm_month;
  else if(!strcmp(lcl_sng,"day") || !strcmp(lcl_sng,"days")) rcd_typ=tm_day;
  else if(!strcmp(lcl_sng,"hour") || !strcmp(lcl_sng,"hours")) rcd_typ=tm_hour;
  else if(!strcmp(lcl_sng,"min") || !strcmp(lcl_sng,"mins") || !strcmp(lcl_sng,"minute") || !strcmp(lcl_sng,"minutes")) rcd_typ=tm_min;
  else if(!strcmp(lcl_sng,"sec") || !strcmp(lcl_sng,"secs") || !strcmp(lcl_sng,"second") || !strcmp(lcl_sng,"seconds")) rcd_typ=tm_sec;
  
  if(lcl_sng) lcl_sng=(char *)nco_free(lcl_sng);
  return rcd_typ;
} /* end nco_cln_get_tm_typ() */


char *                 /* O [sng] contains newly malloced output string */
nco_cln_fmt_dt         /*   [fnc] format an output string */
(tm_cln_sct *ttx,      /* I [ptr] Calendar structure */
int ifmt)              /* I [enm] nco_fmt_dt */
{
 char bdate[200]={0};
 char btime[200]={0};
 char *buff;

 buff=(char*)nco_malloc( sizeof(char) * NCO_MAX_LEN_FMT_SNG );

 switch(ifmt)
 {
    /* plain format all out */
    case fmt_dt_rgl:
     sprintf(buff,"%04d-%02d-%02d %02d:%02d:%f", ttx->year,ttx->month, ttx->day,ttx->hour,ttx->min,ttx->sec  );
     break;


     /* plain format all out with 'T' char as spacer*/
   case fmt_dt_iso8601:
     sprintf(buff,"%04d-%02d-%02dT%02d:%02d:%f", ttx->year,ttx->month, ttx->day,ttx->hour,ttx->min,ttx->sec  );
     break;

     /* do date and time if time not all zero */
    case fmt_dt_sht:
    case fmt_dt_nil:
      sprintf(bdate,"%04d-%02d-%02d", ttx->year,ttx->month, ttx->day);
      if( ttx->hour !=0 || ttx->min!=0 || ttx->sec !=0.0 )
      {
        int isec;
        double m_sec, frac_sec;

        frac_sec=modf(ttx->sec, &m_sec);
        isec=(int)m_sec;

        if( frac_sec==0.0)
          sprintf(btime, " %02d:%02d:%02d", ttx->hour,ttx->min, isec );
        else
          sprintf(btime, " %02d:%02d:%02.7f", ttx->hour,ttx->min, ttx->sec );
      }
      sprintf(buff,"%s%s", bdate,btime);
      break;
 }

   return buff;
}


nco_cln_typ /* [enm] Calendar type */
nco_cln_get_cln_typ /* [fnc] Determine calendar type or cln_nil if not found */
(const char *ud_sng) /* I [ptr] Units string */
{
  int idx;
  int len; 
  char *lcl_sng;  
  nco_cln_typ rcd_typ;
  
  if(!ud_sng) return cln_nil;
  
  lcl_sng=strdup(ud_sng);
  
  /* Set initial return type to void then overwrite */
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
nco_cln_pop_tm         /* [fnc] Calculate other members  in cln_sct from value*/
(tm_cln_sct *cln_sct) /* I/O [ptr] Calendar structure */
{
  int idx;
  int *days_per_month=NULL_CEWI;
  long ivalue;
  long days;
  double fr_value;
  double m_value;
  double *data=NULL_CEWI;

  switch(cln_sct->sc_cln)
  {
    case cln_360:
      data=DATA_360;
      days_per_month=DAYS_PER_MONTH_360;
      break;

    case cln_365:
      data=DATA_365;
      days_per_month=DAYS_PER_MONTH_365;
      break;

    case cln_366:
       data=DATA_366;
       days_per_month=DAYS_PER_MONTH_366;
       break;

    case cln_std:
    case cln_grg:
    case cln_jul:
    case cln_nil:
      break;
  } /* end switch */


  /* take account of origin */


  /* get integer value and fraction part - nb add origin */
  fr_value=modf(cln_sct->value+(double)data[6], &m_value);

  ivalue=(long)m_value;

  /* integer arithmetic kind of */
  cln_sct->sec= (ivalue % 60);
  cln_sct->sec+=fr_value;

  cln_sct->min =   (ivalue % (long)data[3]) /  (long)data[4];
  cln_sct->hour =  (ivalue % (long)data[2]) / (long)data[3];
  cln_sct->year=  ivalue / (long)data[0];

  /* days from start of year remember  zero based */
  days= ivalue % (long)data[0] / (long)data[2];

  /* remember a calendar is one based NOT zero based */
  days+=1;
  cln_sct->month=1;

  for(idx=0 ; idx<12 ;idx++  )
  {
    if( days - days_per_month[idx] <=0 )
      break;

    days -= days_per_month[idx];
      ++cln_sct->month;
  }
  cln_sct->day=days;


  return;
} /* end nco_cln_pop_tm() */


void
nco_cln_pop_val /* [fnc] Calculate value in cln_sct */ 
(tm_cln_sct *cln_sct) /* I/O [ptr] structure */
{
  /* Purpose: Populate values in calendar structure */
  double *data;
  
  switch(cln_sct->sc_cln){
  case cln_360:
    data=DATA_360;
    cln_sct->value=data[0]*(cln_sct->year-1)+
      data[1]*(cln_sct->month-1)+
      data[2]*(cln_sct->day-1)+
      data[3]*cln_sct->hour+
      data[4]*cln_sct->min+
      data[5]*cln_sct->sec;
      /* subtract origin */
      cln_sct->value-=data[6];
      break;
  case cln_365:  
    data=DATA_365;    
    cln_sct->value=data[0]*(cln_sct->year-1)+
      data[2]*nco_cln_days_in_year_prior_to_given_month(cln_365,cln_sct->month)+
      data[2]*(cln_sct->day-1)+
      data[3]*cln_sct->hour+
      data[4]*cln_sct->min+
      data[5]*cln_sct->sec;
      /* subtract origin */
      cln_sct->value-=data[6];
      break;
  case cln_366:
    data=DATA_366;    
    cln_sct->value=data[0]*(cln_sct->year-1)+
      data[2]*nco_cln_days_in_year_prior_to_given_month(cln_366,cln_sct->month)+
      data[2]*(cln_sct->day-1)+
      data[3]*cln_sct->hour+
      data[4]*cln_sct->min+
      data[5]*cln_sct->sec;
      /* subtract origin */
      cln_sct->value-=data[6];
      break;
  case cln_std:
  case cln_grg:
  case cln_jul:
  case cln_nil:
      break;
  } /* end switch */
  
  return;
} /* end nco_cln_pop_val() */




double /* O [dbl] time in (base) seconds of tm_typ */
nco_cln_val_tm_typ
( nco_cln_typ lmt_cln, /* I [enm] Calendar type */ 
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
    data=DATA_366;    
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
  
  return scl;
} /* end nco_cln_typ_val() */


void
nco_cln_prn_tm         /* [fnc] print tm sct*/
(tm_cln_sct *cln_sct) /* I [ptr] Calendar structure */
{

  (void)fprintf(stderr ,"%s: tm_sct cln_type=%d date=\"%d-%d-%d %d:%d:%g\" value=%g\n", nco_prg_nm_get(),cln_sct->sc_cln,
              cln_sct->year, cln_sct->month, cln_sct->day,cln_sct->hour,cln_sct->min, cln_sct->sec,cln_sct->value );

   return;
}

int /* O [flg] String is calendar date */
nco_cln_chk_tm /* [fnc] Is string a UDUnits-compatible calendar format, e.g., "PERIOD since REFERENCE_DATE" */
(const char *unit_sng) /* I [sng] Units string */
{
  /* Purpose:
     Determine whether the string is a UDUnits-compatible calendar format, e.g., "PERIOD since REFERENCE_DATE" */

  /* Does string contain date keyword? */
  if(strcasestr(unit_sng," from ") || strcasestr(unit_sng," since ") || strcasestr(unit_sng," after ")) return True; else return False;

} /* end nco_cln_chk_tm() */

#ifndef ENABLE_UDUNITS
/* Stub functions to compile without UDUNITS2 */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_var_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ lmt_cln,    /* I [enum] Calendar type of coordinate var */ 
double *val_dbl,           /* I/O [dbl] var values modified */
var_sct *var)           /* I/O [var_sct] var values modified */
{
  (void)fprintf(stderr,"%s: WARNING NCO was built without UDUnits. NCO is therefore unable to convert values from \"%s\" to \"%s\". See http://nco.sf.net/nco.html#rbs for more information.\n%s: HINT Re-build or re-install NCO enabled with UDUnits.\n", nco_prg_nm_get(),fl_unt_sng,fl_bs_sng);

  return NCO_NOERR;
} /* end nco_cln_clc_dbl_var_dff() */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_org(   /* [fnc] difference between two co-ordinate units */
const char *val_unt_sng, /* I [ptr] input value and  units in the same string */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ lmt_cln,    /* I [enum] Calendar type of coordinate var */ 
double *og_val)         /* O [dbl] output value */
{
  *og_val=0.0; /* CEWI */
  (void)fprintf(stderr,"%s: WARNING NCO was built without UDUnits. NCO is therefore unable to interpret the string \"%s\" limit argument. HINT Re-build or re-install NCO enabled with UDUnits.\n", nco_prg_nm_get(),val_unt_sng);
  return NCO_NOERR; 
} /* !nco_cln_clc_dbl_org() */

int /* [rcd] Return code */
nco_cln_sng_rbs /* [fnc] Rebase calendar string for legibility */
(const ptr_unn val, /* I [sct] Value to rebase */
 const long val_idx, /* I [idx] Index into 1-D array of values */
 const nc_type val_typ, /* I [enm] Value type */
 const char *unit_sng, /* I [sng] Units string */
 char *lgb_sng) /* O [sng] Legible version of input string */
{
  lgb_sng[0]='\0'; /* CEWI */
  return NCO_NOERR;
} /* end nco_cln_sng_rbs() */

int
nco_cln_var_prs
(const char *fl_unt_sng,
 nco_cln_typ lmt_cln,
 int ifmt,
 var_sct *var,
 var_sct *var_ret)
{
  return NCO_ERR;
} /*  !nco_cln_var_prs() */

#endif /* !ENABLE_UDUNITS */

#ifdef ENABLE_UDUNITS
# ifdef HAVE_UDUNITS2_H

/* UDUnits2 routines */
cv_converter*   /* UDUnits converter */
nco_cln_cnv_mk  /* [fnc] UDUnits2 create a custom converter  */
(const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
 const char *fl_bs_sng) /* I [ptr] units attribute string from disk */
{
  const char fnc_nm[]="nco_cln_cnv_mk()"; /* [sng] Function name */
  
  cv_converter *ut_cnv; /* UDUnits converter */

  int ut_rcd; /* [enm] UDUnits2 status */
  
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* [sct] UDUnits structure, input units */
  ut_unit *ut_sct_out; /* [sct] UDUnits structure, output units */
  
  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: %s() failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return (cv_converter*)NULL; /* Failure */
  } /* end if err */ 
  
  /* Units string to convert from */
  ut_sct_in=ut_parse(ut_sys,fl_unt_sng,UT_ASCII); 
  if(!ut_sct_in){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute \"%s\" has a syntax error\n",fl_unt_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_unt_sng);
    return (cv_converter*)NULL; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Units string to convert to */
  ut_sct_out=ut_parse(ut_sys,fl_bs_sng,UT_ASCII); 
  if(!ut_sct_out){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: Empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute  \"%s\" has a syntax error\n",fl_bs_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_bs_sng);
    return (cv_converter*)NULL; /* Failure */
  } /* endif */

  /* Create converter */
  ut_cnv=ut_get_converter(ut_sct_in,ut_sct_out); /* UDUnits converter */
  if(!ut_cnv){
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"WARNING: One of units, %s or %s, is NULL\n",fl_bs_sng,fl_unt_sng);
    if(ut_rcd == UT_NOT_SAME_SYSTEM) (void)fprintf(stderr,"WARNING: Units %s and %s belong to different unit systems\n",fl_bs_sng,fl_unt_sng);
    if(ut_rcd == UT_MEANINGLESS) (void)fprintf(stderr,"WARNING: Conversion between user-specified unit \"%s\" and file units \"%s\" is meaningless\n",fl_bs_sng,fl_unt_sng);
    return (cv_converter*)NULL; /* Failure */
  } /* endif */

  ut_free(ut_sct_in);
  ut_free(ut_sct_out);
  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */
  // cv_free(ut_cnv);

  return ut_cnv;
}  /* end UDUnits2 nco_cln_cnv_mk() */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
double *og_val)
{ /* I/O [dbl] var values modified */
  cv_converter *ut_cnv=NULL; 

  /* Do nothing if units identical */
  if(strcasecmp(fl_unt_sng,fl_bs_sng)==0) return NCO_NOERR;

  /* Convert */
  ut_cnv=nco_cln_cnv_mk(fl_unt_sng, fl_bs_sng);

  if(ut_cnv != NULL) og_val[0]=cv_convert_double(ut_cnv,og_val[0]); else return NCO_ERR;     

  cv_free(ut_cnv);  

  return NCO_NOERR;          
} /* end UDUnits2 nco_cln_clc_dbl_dff() */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_var_dff /* [fnc] difference between two co-ordinate units */
(const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
 const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
 var_sct *var) /* I/O [dbl] var values modified */
{
  cv_converter *ut_cnv=NULL; 

  ptr_unn op1;    

  size_t idx;    

  size_t sz;
  
  /* Do nothing if units identical */
  if(!strcasecmp(fl_unt_sng,fl_bs_sng)) return NCO_NOERR;

  /* Convert */
  ut_cnv=nco_cln_cnv_mk(fl_unt_sng,fl_bs_sng);

  if(ut_cnv == NULL) return NCO_ERR;     

  sz=var->sz;  

  (void)cast_void_nctype(var->type,&op1);                
  op1=var->val;
 
  if(var->type == NC_DOUBLE){
    double *dp;
    dp=op1.dp;     

    if(var->has_mss_val){
      double mss_dbl=var->mss_val.dp[0]; 
      for(idx=0; idx<sz; idx++)
	if(dp[idx] != mss_dbl)
	  dp[idx]=cv_convert_double(ut_cnv,dp[idx]);                      
    }else{
     (void)cv_convert_doubles(ut_cnv,dp,sz,dp);                      
    } /* !has_mss_val */
  }else if(var->type==NC_FLOAT){
    float*fp;
    fp=op1.fp;     
    if(var->has_mss_val){
      float mss_fp=var->mss_val.fp[0]; 
      for(idx=0;idx<sz;idx++)
	if(fp[idx] != mss_fp)   
	  fp[idx]=cv_convert_float(ut_cnv,fp[idx]);                      
    }else{
      (void)cv_convert_floats(ut_cnv,fp,sz,fp);                      
    } /* !has_mss_val */
  } /* !var->type */

 cv_free(ut_cnv);  
 (void)cast_nctype_void(var->type,&op1);

 return NCO_NOERR;
} /* !nco_cln_clc_var_dff() */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_var_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ lmt_cln,    /* I [enum] Calendar type of coordinate var */ 
double *og_val,           /* I/O [dbl] var values modified -can be NULL */
var_sct *var) /* I/O [var_sct] var values modified - can be NULL  */
{ /* Purpose: */
  
  const char fnc_nm[]="nco_cln_clc_dbl_var_dff()"; /* [sng] Function name */

  int is_date;
  int rcd;

  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: %s reports unt_sng=%s bs_sng=%s calendar=%d\n",nco_prg_nm_get(),fnc_nm,fl_unt_sng,fl_bs_sng,lmt_cln);

  /* Do nothing if units identical */
  if(!strcasecmp(fl_unt_sng,fl_bs_sng)) return NCO_NOERR;

  /* Are target units of the form  "units since date-string" */
  is_date=nco_cln_chk_tm(fl_bs_sng);

  /* Use custom time functions if irregular calendar */
  if(is_date && (lmt_cln == cln_360 || lmt_cln == cln_365 || lmt_cln == cln_366))
    rcd=nco_cln_clc_tm(fl_unt_sng,fl_bs_sng,lmt_cln,og_val,var);  
  else if(og_val != (double *)NULL) 
    rcd=nco_cln_clc_dbl_dff(fl_unt_sng,fl_bs_sng,og_val);
  else if(var != (var_sct *)NULL)
    rcd=nco_cln_clc_var_dff(fl_unt_sng,fl_bs_sng,var);
  else
    rcd=NCO_NOERR;
    
  return rcd;
} /* end UDUnits2 nco_cln_clc_dbl_var_dff() */

int   /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_org(   /* [fnc] difference between two co-ordinate units */
const char *val_unt_sng, /* I [ptr] input value and  units in the same string */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ lmt_cln,    /* I [enum] Calendar type of coordinate var */ 
double *og_val)         /* O [dbl] output value */
{
  const char fnc_nm[]="nco_cln_clc_dbl_org()"; /* [sng] Function name */

  int is_date=0;  /* set to true if date/time unit */
  int rcd=0;
  int month=0;
  int year=0;
  char lcl_unt_sng[200]={0};
  double val_dbl=0.0;

  rcd=0;  

  val_dbl=0.0;

  is_date=nco_cln_chk_tm(fl_bs_sng);

  lcl_unt_sng[0]='\0';
  
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) (void)fprintf(stderr,"%s: INFO %s: reports unt_sng=%s bs_sng=%s calendar=%d\n",nco_prg_nm_get(),fnc_nm,val_unt_sng,fl_bs_sng,lmt_cln);

  /* Does fl_unt_sng look like a regular timestamp? */ 
  if(is_date && sscanf(val_unt_sng,"%d-%d",&year,&month) == 2){
    /* UDUnits requires this prefix to recognize a raw-datetime string */  
    strcpy(lcl_unt_sng,"s@");
    strcat(lcl_unt_sng,val_unt_sng);
    val_dbl=0.0;
  }else{
    /* Regular conversion of fl_unt_sng of form <double_value units>, e.g., '10 inches', '100 ft'  '10 days since 1970-01-01' */
    char *ptr=NULL;
        
    val_dbl=strtod(val_unt_sng,&ptr);  
    if(ptr == val_unt_sng || strlen(++ptr) < 1L){
      (void)fprintf(stderr, "%s: INFO %s() reports input string must be of the form \"value unit\" got the string \"%s\"\n",nco_prg_nm_get(),fnc_nm,val_unt_sng);
      nco_exit(EXIT_FAILURE);           
    } /* !ptr */
    strcpy(lcl_unt_sng,ptr); 
  } /* !is_date */

  /* Use custom time functions if irregular calendar */
  if(is_date && (lmt_cln == cln_360 || lmt_cln == cln_365 || lmt_cln == cln_366)){
    rcd=nco_cln_clc_tm(lcl_unt_sng,fl_bs_sng,lmt_cln,&val_dbl,(var_sct *)NULL);
  }else rcd=nco_cln_clc_dbl_dff(lcl_unt_sng,fl_bs_sng,&val_dbl);

  /* Copy over iff successful */ 
  if(rcd==NCO_NOERR) *og_val=val_dbl; else (void)fprintf(stderr,"%s: ERROR %s: report unt_sng=%s bs_sng=%s calendar=%d og_val=%f\n",nco_prg_nm_get(),fnc_nm,val_unt_sng,fl_bs_sng,lmt_cln, val_dbl);  
 
  return rcd;        
} /* !nco_cln_clc_dbl_org() */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_clc_tm /* [fnc] Difference between two coordinate units */
(const char *fl_unt_sng, /* I [ptr] Units attribute string from disk */
 const char *fl_bs_sng, /* I [ptr] Units attribute string from disk */
 nco_cln_typ lmt_cln, /* [enum] Calendar type of coordinate variable */ 
 double *og_val, /* I/O [ptr] */
 var_sct *var) /* I/O [ptr] */  
{
  /* Called for target units in fl_bs_sng of form "value unit since date-stamp" for cln_360 or cln_365
     Either "var" is NULL and there is a single value to process *og_val or var is initialized and og_val is NULL */
  const char fnc_nm[]="nco_cln_clc_tm()"; /* [sng] Function name */

  char *tmp_sng;

  double crr_val=0.0;
  double scl_val=1.0;

  tm_typ unt_tm_typ; /* enum for units type in fl_unt_sng */  
  tm_typ bs_tm_typ; /* enum for units type in fl_bs_sng */

  tm_cln_sct unt_cln_sct;
  tm_cln_sct bs_cln_sct;
  
  /* Die if unsupported calendar type */
  if(lmt_cln != cln_360 && lmt_cln != cln_365 && lmt_cln != cln_366){
    (void)fprintf(stderr,"%s: %s reports invalid calendar type lmt_cln=%d. Only cln_365,cln_360 cln_366 allowed.\n",nco_prg_nm_get(),fnc_nm,lmt_cln);
    nco_exit(EXIT_FAILURE);
  } /* !lmt_cln */

  /* Obtain units type from fl_bs_sng */
  tmp_sng=(char *)nco_calloc(NCO_MAX_LEN_TMP_SNG,sizeof(char));
  if(sscanf(fl_bs_sng,"%s",tmp_sng) != 1) return NCO_ERR;
  bs_tm_typ=nco_cln_get_tm_typ(tmp_sng);  
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: %s reports unt_sng=\"%s\", bs_sng=\"%s\", tmp_sng=\"%s\"\n",nco_prg_nm_get(),fnc_nm,fl_unt_sng,fl_bs_sng,tmp_sng);
  if(tmp_sng) tmp_sng=(char *)nco_free(tmp_sng);

  /* Is unit string a bare date string? */ 
  tmp_sng=(char *)nco_calloc(NCO_MAX_LEN_TMP_SNG,sizeof(char));
  if(!strncmp("s@",fl_unt_sng,2)) unt_tm_typ=bs_tm_typ;
  else if(sscanf(fl_unt_sng,"%s",tmp_sng) == 1) unt_tm_typ=nco_cln_get_tm_typ(tmp_sng);  
  else return NCO_ERR;
  if(tmp_sng) tmp_sng=(char *)nco_free(tmp_sng);
  
  /* Assume non-standard calendar */ 
  if(nco_cln_prs_tm(fl_unt_sng,&unt_cln_sct) == NCO_ERR) return NCO_ERR;
  if(nco_cln_prs_tm(fl_bs_sng,&bs_cln_sct) == NCO_ERR) return NCO_ERR;
  
  unt_cln_sct.sc_typ=bs_tm_typ;
  bs_cln_sct.sc_typ=bs_tm_typ;
  
  unt_cln_sct.sc_cln=lmt_cln;
  bs_cln_sct.sc_cln=lmt_cln; 
  (void)nco_cln_pop_val(&unt_cln_sct);
  (void)nco_cln_pop_val(&bs_cln_sct);
  
  /* Get offset */
  crr_val=(unt_cln_sct.value-bs_cln_sct.value)/nco_cln_val_tm_typ(lmt_cln,bs_tm_typ);                 

  /* Scale factor */
  if(unt_tm_typ == bs_tm_typ) scl_val=1.0; else scl_val=nco_cln_val_tm_typ(lmt_cln,unt_tm_typ)/nco_cln_val_tm_typ(lmt_cln,bs_tm_typ);
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    nco_cln_prn_tm(&unt_cln_sct);
    nco_cln_prn_tm(&bs_cln_sct);
    (void)fprintf(stderr,"%s: %s reports offset=%g, scale factor=%g unt_val=%f bs_val=%f\n",nco_prg_nm_get(),fnc_nm,crr_val,scl_val, unt_cln_sct.value, bs_cln_sct.value);
    if(og_val) (void)fprintf(stderr,", *og_val=%g",*og_val);
    (void)fprintf(stderr,"\n");
  } /* !dbg */

  if(og_val){
    *og_val=(*og_val)*scl_val+crr_val;
  }else if(var){
    size_t sz;
    size_t idx;
    ptr_unn op1;
    
    sz=var->sz;  
    op1=var->val;
    (void)cast_void_nctype(var->type,&op1);

    if(var->type == NC_DOUBLE){
      double *dp;
      dp=op1.dp;
      if(var->has_mss_val){  
	double mss_dbl=var->mss_val.dp[0]; 
	for(idx=0;idx<sz;idx++)
	  if(dp[idx] != mss_dbl) dp[idx]=dp[idx]*scl_val+crr_val; 
      }else{
	for(idx=0;idx<sz;idx++) dp[idx]=dp[idx]*scl_val+crr_val; 
      } /* !var->has_mss_val */
    } /* !NC_DOUBLE */

    if(var->type == NC_FLOAT){
      float *fp;
      fp=op1.fp;
      if(var->has_mss_val){  
	float mss_flt=var->mss_val.fp[0]; 
	for(idx=0;idx<sz;idx++)
	  if(fp[idx] != mss_flt) fp[idx]=fp[idx]*(float)scl_val+(float)crr_val;                      
      }else{
	for(idx=0;idx<sz;idx++) fp[idx]=fp[idx]*(float)scl_val+(float)crr_val;                      
      } /* !has_mss_val */
    } /* !NC_FLOAT */
   (void)cast_nctype_void(var->type,&op1);
  } /* !var */
  
  return NCO_NOERR;

} /* end nco_cln_clc_tm() */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_prs_tm /* UDUnits2 Extract time stamp from parsed UDUnits string */
(const char *unt_sng, /* I [ptr] units attribute string */
 tm_cln_sct *tm_in) /* O [sct] Time structure to be populated */
{
  const char fnc_nm[]="nco_cln_prs_tm()"; /* [sng] Function name */

  char *bfr;
  char *dt_sng=NULL;
  int cnv_nbr;
  int ut_rcd; /* [enm] UDUnits2 status */
  int year;
  int month;
  int day;
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* UDUnits structure, input units */

  bfr=(char *)nco_calloc(NCO_MAX_LEN_TMP_SNG,sizeof(char));




  /* There is a problem letting udunits do the parsing for the other calendars - that is when parsing non regular dates
     eg for "all_leap" (cln_366) we have dates like 2001-02-29  which cannot be meaningfully parsed by udunits
     e.g for 360_days (cln_360)  we have dates like 1903-2-29, 1903-2-30  so we need to parse the date portion of the string here*/


   strcpy(bfr,unt_sng); 
    
   if( (dt_sng=strstr(bfr,"since")))
    dt_sng+=5;
   else if ( (dt_sng=strstr(bfr,"from")))
    dt_sng+=4;
   else if ( (dt_sng=strstr(bfr,"after")))
    dt_sng+=5;
   else if (( dt_sng=strstr(bfr,"s@")))
    dt_sng+=2;

   if(!dt_sng) return NCO_ERR;

   cnv_nbr=sscanf(dt_sng,"%d-%d-%d",&tm_in->year,&tm_in->month,&tm_in->day);
   if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s reports sscanf() converted %d values and it should have converted 3 values, format string=\"%s\"\n",nco_prg_nm_get(),fnc_nm,cnv_nbr,dt_sng);

  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: %s failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return NCO_ERR; /* Failure */
  } /* end if err */ 

  /* Units string to convert from */
  ut_sct_in=ut_parse(ut_sys,unt_sng,UT_ASCII); 
  if(ut_sct_in == NULL){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"%s: ERROR %s reports empty units attribute string\n",nco_prg_nm_get(),fnc_nm);
    else if(ut_rcd == UT_SYNTAX)  (void)fprintf(stderr,"%s: ERROR %s reports units attribute \"%s\" has a syntax error\n",nco_prg_nm_get(),fnc_nm,unt_sng);
    else if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"%s: ERROR %s reports units attribute \"%s\" is not listed in UDUnits2 SI system database\n",nco_prg_nm_get(),fnc_nm,unt_sng);
    else (void)fprintf(stderr,"%s: ERROR %s reports ut_rcd = %d\n",nco_prg_nm_get(),fnc_nm,ut_rcd);

    return NCO_ERR; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Print timestamp to buffer in standard, dependable format */
  ut_format(ut_sct_in,bfr,NCO_MAX_LEN_TMP_SNG,UT_ASCII|UT_NAMES);

  /* ut_format string normally ends in the string UTC we wish to terminate the string before this so that sscanf doesnt get confused */
  dt_sng=strstr(bfr,"UTC");
  if(dt_sng) *dt_sng='\0';

  dt_sng=strstr(bfr,"since");
  dt_sng+=(size_t)6;
  /* nb we dont need yar,month,day as that have been parsed earlier in THIS function */
  cnv_nbr=sscanf(dt_sng,"%d-%d-%d %d:%d:%lf",&year,&month,&day,&tm_in->hour,&tm_in->min,&tm_in->sec);
  
  /* Set defaults */ 
  if(cnv_nbr < 6) tm_in->sec=0.0;
  if(cnv_nbr < 5) tm_in->min=0;   
  if(cnv_nbr < 4) tm_in->hour=0;   

  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s reports sscanf() converted %d values and it should have converted 6 values, format string=\"%s\"\n",nco_prg_nm_get(),fnc_nm,cnv_nbr,bfr);

  bfr=(char *)nco_free(bfr);  
  ut_free(ut_sct_in);
  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */

  /* Return if sscanf() messed up */
  if(cnv_nbr < 3) return NCO_ERR;

  return NCO_NOERR;
} /* end UDUnits2 nco_cln_prs_tm() */

int /* [rcd] Return code */
nco_cln_sng_rbs /* [fnc] Rebase calendar string for legibility */
(const ptr_unn val, /* I [sct] Value to rebase */
 const long val_idx, /* I [idx] Index into 1-D array of values */
 const nc_type val_typ, /* I [enm] Value type */
 const char *unit_sng, /* I [sng] Units string */
 char *lgb_sng) /* O [sng] Legible version of input string */
{
  /* Purpose: Rebase calendar string for legibility
     Assumptions: Input units string unit_sng is a calendar date, i.e., contains "from", "since", or "after"

     ncdump handles this in nctime0.c
     dumplib.c/nctime_val_tostring() by Dave Allured, NOAA
     cdRel2Iso() from CDMS by Bob Drach, LLNL
     cdParseRelunits() from CDMS by Bob Drach, LLNL */

  const char fnc_nm[]="nco_cln_sng_rbs()"; /* [sng] Function name */
  
  double val_dbl; /* [day] Calendar offset converted to double */

  int ut_rcd; /* [enm] UDUnits2 status */
  
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* [sct] UDUnits structure, input units */
  ut_unit *ut_sct_out; /* [sct] UDUnits structure, output units */

  /* Quick return if units DNE */
  if(!unit_sng) return NCO_NOERR;
  
  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(!ut_sys){
    (void)fprintf(stdout,"%s: %s() failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    return NCO_ERR; /* Failure */
  } /* end if err */ 

  /* Units string containing calendar origin converted to UDUnit structure */
  ut_sct_in=ut_parse(ut_sys,unit_sng,UT_ASCII); 
  if(!ut_sct_in){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute \"%s\" has a syntax error\n",unit_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",unit_sng);
    return NCO_ERR; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Convert time since calendar origin to double */
  val_dbl=ptr_unn_2_scl_dbl(val,val_typ); 
  
  /* Units string to convert to */
  ut_sct_out=ut_offset(ut_sct_in,val_dbl);
  if(!ut_sct_out){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: Empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute  \"%s\" has a syntax error\n",unit_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",unit_sng);
    return NCO_ERR; /* Failure */
  } /* endif */

  val_dbl+=0*val_idx; /* CEWI */

  ut_free(ut_sct_in);
  ut_free(ut_sct_out);
  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */

  lgb_sng[0]='\0'; /* CEWI */

  return NCO_NOERR;

} /* end nco_cln_sng_rbs() */


int
nco_cln_var_prs
(const char *fl_unt_sng,
 nco_cln_typ lmt_cln,
 int ifmt,
 var_sct *var,
 var_sct *var_ret
)
{
  size_t sz;
  size_t idx;
  char empty_sng[1];

  double resolution;
  tm_cln_sct tm;



  /* base units for udunits */
  const char *bs_sng="seconds since 2001-01-01";
  const char *fnc_nm="nco_cln_var_prs";
  empty_sng[0]='\0';

  // if( lmt_cln != cln_std )
  //   return NCO_ERR;
  if(var->type !=NC_DOUBLE && var->type!=NC_FLOAT)
    nco_var_cnf_typ(NC_DOUBLE,var);

  if(nco_dbg_lvl_get() >= nco_dbg_crr)
     (void)fprintf(stderr,"%s: %s reports unt_sng=%s bs_sng=%s calendar=%d\n",nco_prg_nm_get(),fnc_nm,fl_unt_sng,bs_sng,lmt_cln);

  /* rebase to seconds since blah-blah */
  if(nco_cln_clc_dbl_var_dff(fl_unt_sng,bs_sng,lmt_cln, (double*)NULL, var ) != NCO_NOERR )
     return NCO_ERR;



  cast_void_nctype(var->type,&var->val);

  if(var_ret->type !=NC_STRING)
      nco_var_cnf_typ(NC_STRING, var_ret);

  if( var_ret->val.vp)
      var_ret->val.vp=(void*)nco_free(var_ret->val.vp);

  var_ret->val.vp=nco_malloc( sizeof(nco_string) *var_ret->sz);


  var_ret->has_mss_val=True;
  var_ret->mss_val.vp=nco_malloc(sizeof(nco_string*));

  cast_void_nctype(var_ret->type,&var_ret->val);

  var_ret->mss_val.sngp[0]=strdup(empty_sng);


  sz=var->sz;

  tm.sc_cln=lmt_cln;


  // (void)fprintf(stderr,"%s: %s reports var \"%s\" has missing value %d\n",nco_prg_nm_get(),fnc_nm,var->nm,var->has_mss_val);



  if(var->type == NC_DOUBLE) {
    double mss_val_dbl;
    if(var->has_mss_val)
       mss_val_dbl=var->mss_val.dp[0];

    for (idx = 0; idx < sz; idx++) {


      if(var->has_mss_val && var->val.dp[idx]==mss_val_dbl) {
        var_ret->val.sngp[idx] = strdup(empty_sng);
        continue;
      }

      tm.value = var->val.dp[idx];

      if (lmt_cln == cln_360 || lmt_cln == cln_365 || lmt_cln == cln_366)
        nco_cln_pop_tm(&tm);
      else
        (void) ut_decode_time(tm.value, &tm.year, &tm.month, &tm.day, &tm.hour, &tm.min, &tm.sec, &resolution);

      var_ret->val.sngp[idx] = nco_cln_fmt_dt(&tm, ifmt);

    }
  }
  else if(var->type==NC_FLOAT){

    float mss_val_flt;
    if(var->has_mss_val)
       mss_val_flt=var->mss_val.fp[0];

    for (idx = 0; idx < sz; idx++) {

      if(var->has_mss_val && var->val.fp[idx]==mss_val_flt ){
        var_ret->val.sngp[idx] = strdup(empty_sng);
        continue;
      }


      tm.value = (double) (var->val.fp[idx]);

      if (lmt_cln == cln_360 || lmt_cln == cln_365 || lmt_cln == cln_366)
        nco_cln_pop_tm(&tm);
      else
        (void) ut_decode_time(tm.value, &tm.year, &tm.month, &tm.day, &tm.hour, &tm.min, &tm.sec, &resolution);

      var_ret->val.sngp[idx] = nco_cln_fmt_dt(&tm, ifmt);

    }

  }


  cast_nctype_void(var->type,&var->val);
  cast_nctype_void(var_ret->type,&var->val);

    return NCO_NOERR;

} /* end nco_cln_var_prs() */





# endif /* HAVE_UDUNITS2_H */
#endif /* ENABLE_UDUNITS */
/* End UDUnits-related routines*/
 
