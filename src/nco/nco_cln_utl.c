/* $Header$ */

/* Purpose: Calendar utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_cln_utl.h" /* Calendar utilities */

/* Arrays to hold calendar type units
   Format: year, month, day, hour, minute, second, origin, offset
   Origin for all calendars is 2001-01-01 (seconds) (same as origin for UDUnits xalendar) */
static double DATA_360[8]={31104000.0,2592000.0,86400.0,3600.0,60.0,1.0,31104000.0*2001.0,0.0};
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
  const int mdays[]={31,28,31,30,31,30,31,31,30,31,30,31};
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
    {31,28,31,30,31,30,31,31,30,31,30,31,
     31,28,31,30,31,30,31,31,30,31,30,31};
  
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
} /* !nco_newdate() */

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
} /* !nco_cln_get_tm_typ() */

char * /* O [sng] Newly malloc'd output string */
nco_cln_fmt_dt /* [fnc] Format date-string for printable output */
(tm_cln_sct *ttx, /* I [ptr] Calendar structure */
 int dt_fmt_enm) /* I [enm] nco_fmt_dt */
{
  char bdate[200]={0};
  char btime[200]={0};

  char *bfr;
  
  bfr=(char *)nco_malloc(sizeof(char)*NCO_MAX_LEN_FMT_SNG);
  
  switch(dt_fmt_enm){
  case fmt_dt_nil:
    bfr[0]='\0';
    break;
  case fmt_dt_rgl:
    /* Plain format */
    sprintf(bfr,"%04d-%02d-%02d %02d:%02d:%09.6f",ttx->year,ttx->month,ttx->day,ttx->hour,ttx->min,ttx->sec);
    break;
  case fmt_dt_iso8601:
    /* Same as plain format except with 'T' character as date-time spacer
       ISO8601 format tester:
       https://dencode.com/en/date
       20201026T182352,215Z */
    sprintf(bfr,"%04d-%02d-%02dT%02d:%02d:%09.6f",ttx->year,ttx->month,ttx->day,ttx->hour,ttx->min,ttx->sec);
    break;
  case fmt_dt_sht:
    /* Print date and print time if time not all zeros */
    sprintf(bdate,"%04d-%02d-%02d",ttx->year,ttx->month,ttx->day);
    if(ttx->hour != 0 || ttx->min != 0 || ttx->sec != 0.0){
      int isec;
      double m_sec,frc_sec;
      
      frc_sec=modf(ttx->sec,&m_sec);
      isec=(int)m_sec;
      
      if(frc_sec == 0.0) sprintf(btime," %02d:%02d:%02d",ttx->hour,ttx->min,isec); else sprintf(btime," %02d:%02d:%02.7f",ttx->hour,ttx->min,ttx->sec);
    } /* !0 */
    sprintf(bfr,"%s%s",bdate,btime);
    break;
  } /* !dt_fmt_enm */
 
  return bfr;
} /* !nco_cln_fmt_dt() */

nco_cln_typ /* O [enm] Calendar type */
nco_cln_get_cln_typ /* [fnc] Determine calendar type or cln_nil if not found */
(const char *ud_sng) /* I [ptr] Units string */
{
  int idx;
  int len; 
  char *lcl_sng;  
  nco_cln_typ cln_typ;
  
  if(!ud_sng) return cln_nil;
  
  lcl_sng=strdup(ud_sng);
  
  /* Set initial return type to void then overwrite */
  cln_typ=cln_nil;
  
  /* Convert to lower case */
  len=strlen(lcl_sng);
  for(idx=0;idx<len;idx++) lcl_sng[idx]=tolower(lcl_sng[idx]);
  
  if(strstr(lcl_sng,"standard")) cln_typ=cln_std;
  else if(strstr(lcl_sng,"gregorian") || strstr(lcl_sng,"proleptic_gregorian")) cln_typ=cln_grg;
  else if(strstr(lcl_sng,"julian")) cln_typ=cln_jul;
  else if(strstr(lcl_sng,"360_day")) cln_typ=cln_360;
  else if(strstr(lcl_sng,"noleap") || strstr(lcl_sng,"365_day")) cln_typ=cln_365;
  else if(strstr(lcl_sng,"all_leap") || strstr(lcl_sng,"366_day")) cln_typ=cln_366;
  
  lcl_sng=(char *)nco_free(lcl_sng);
  
  return cln_typ;
} /* !nco_cln_get_cln_typ() */

int /* O [nbr] Number of days */
nco_cln_days_in_year_prior_to_given_month /* [fnc] Number of days in year prior to month */
(nco_cln_typ cln_typ, /* [enm] Calendar type */
 int mth_idx) /* I [idx] Month (1-based counting, December == 12) */
{ 
  int *days=NULL_CEWI;
  int idx;
  int idays=0;
  
  switch(cln_typ){
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
} /* !nco_cln_days_in_year_prior_to_given_month() */

void
nco_cln_pop_tm /* [fnc] Calculate other members  in cln_sct from value*/
(tm_cln_sct *cln_sct) /* I/O [ptr] Calendar structure */
{
  int idx;
  int *days_per_month=NULL_CEWI;
  long ivalue;
  long days;
  double fr_value;
  double m_value;
  double *data=NULL_CEWI;

  switch(cln_sct->cln_typ){
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
  fr_value=modf(cln_sct->value+(double)data[6],&m_value);

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

  for(idx=0;idx<12;idx++){
    if(days - days_per_month[idx] <= 0) break;

    days-=days_per_month[idx];
    ++cln_sct->month;
  }
  cln_sct->day=days;
  return;
} /* !nco_cln_pop_tm() */

void
nco_cln_pop_val /* [fnc] Calculate value in cln_sct */ 
(tm_cln_sct *cln_sct) /* I/O [ptr] structure */
{
  /* Purpose: Populate values in calendar structure */
  double *data;
  
  switch(cln_sct->cln_typ){
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
} /* !nco_cln_pop_val() */

double /* O [dbl] time in (base) seconds of tm_typ */
nco_cln_val_tm_typ
( nco_cln_typ cln_typ, /* I [enm] Calendar type */ 
 tm_typ bs_tm_typ) /* I [enm] Time units */
{
  double *data=NULL_CEWI;
  double scl=double_CEWI;
  
  switch(cln_typ){
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
} /* !nco_cln_typ_val() */

void
nco_cln_prn_tm         /* [fnc] print tm sct*/
(tm_cln_sct *cln_sct) /* I [ptr] Calendar structure */
{

  (void)fprintf(stderr ,"%s: tm_sct cln_type=%d date=\"%d-%d-%d %d:%d:%g\" value=%g\n",nco_prg_nm_get(),cln_sct->cln_typ,
              cln_sct->year,cln_sct->month,cln_sct->day,cln_sct->hour,cln_sct->min,cln_sct->sec,cln_sct->value );

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

} /* !nco_cln_chk_tm() */

int /* O [rcd] Return code */
nco_clm_nfo_to_tm_bnds /* [fnc] Compute and return climatological time and bounds arrays */
(int yr_srt, /* I [yr] Year at climo start */
 int yr_end, /* I [yr] Year at climo start */
 int mth_srt, /* I [mth] Month at climo start [1..12] format */
 int mth_end, /* I [mth] Month at climo end [1..12] format */
 int tpd, /* I [nbr] Timesteps per day [0=monthly, 1=daily, 2, 3, 4, 6, 8, 12, 24=hourly, ...] */
 const char *unt_sng, /* I [sng] Units of time coordinate (UDUnits format) */
 const char *cln_sng, /* I [sng] Calendar string of time coordinate (UDUnits format, NULL=none) */
 double *bnd_val, /* O [frc] Climatology bounds variable values */
 double *tm_val) /* O [frc] Time coordinate values */
{
  const char fnc_nm[]="nco_clm_nfo_to_tm_bnds()"; /* [sng] Function name */

  char srt_sng[200];
  char end_sng[200];
  char md_sng[200];
  
  int idx;
  int day_mid=15;

  double step;
  
  nco_cln_typ cln_typ;
  var_sct *var_tmp=NULL_CEWI;
  
  step=24.0/tpd;

  if(!unt_sng) (void)fprintf(stdout,"%s: WARNING %s called with empty units string\n",nco_prg_nm_get(),fnc_nm);
  if(!cln_sng){
    (void)fprintf(stdout,"%s: WARNING %s called with empty calendar string, will adopt default calendar \"noleap\"\n",nco_prg_nm_get(),fnc_nm);
    /* Default to noleap (365-day) calendar used by most ESMs */
    if(!cln_sng) cln_sng=(char *)strdup("noleap");
  } /* !cln_sng */
  cln_typ=nco_cln_get_cln_typ(cln_sng);
  
  if(cln_typ == cln_nil) return NCO_ERR;
  
  sprintf(srt_sng,"seconds since %d-%d-01",yr_srt,mth_srt);
  
  /* Move end forward one month */
  if(++mth_end == 13){
    mth_end=1;
   yr_end++;
  } /* !mth_end */

  sprintf(end_sng,"seconds since %d-%d-01",yr_end,mth_end);
  
  if(tpd == 0){

    if(tm_val){
      tm_val[0]=0.0;
      /* find day in middle of month - see day_mid  */
      sprintf(md_sng,"seconds since %d-%d-%d",yr_srt,mth_srt,day_mid);
      if(nco_cln_clc_dbl_var_dff(md_sng,unt_sng,cln_typ,&tm_val[0],(var_sct *)NULL) != NCO_NOERR)
        return NCO_ERR;
    }

    if(bnd_val){
      bnd_val[0]=0.0;
      bnd_val[1]=0.0;
      if(nco_cln_clc_dbl_var_dff(srt_sng,unt_sng,cln_typ,&bnd_val[0],(var_sct *)NULL) != NCO_NOERR)
        return NCO_ERR;
      if(nco_cln_clc_dbl_var_dff(end_sng,unt_sng,cln_typ,&bnd_val[1],(var_sct *)NULL) != NCO_NOERR)
        return NCO_ERR;
    } /* !bnd_val */

    return NCO_NOERR;
  } /* tpd != 0 */

  /* tpd >= 1 so times are daily or diurnal */
  var_tmp=(var_sct *)nco_malloc(sizeof(var_sct));
  var_dfl_set(var_tmp);
  var_tmp->type=NC_DOUBLE;

  if(tm_val){
     var_tmp->sz=tpd;
     for(idx=0;idx<tpd;idx++)
       tm_val[idx]=(step/2 + step*idx)*3600;

     cast_void_nctype(NC_DOUBLE,&var_tmp->val);
     var_tmp->val.dp=tm_val;
     cast_nctype_void(NC_DOUBLE,&var_tmp->val);

     if(nco_cln_clc_dbl_var_dff(srt_sng,unt_sng,cln_typ,(double *)NULL,var_tmp) != NCO_NOERR)
       return NCO_ERR;

     var_tmp->val.vp=NULL;
  } /* !tm_val */

  if(bnd_val){
    /* Seconds difference between srt_sng and end_sng */
    double srt_end_dff;

    if(nco_cln_clc_dbl_var_dff(end_sng,srt_sng,cln_typ,&srt_end_dff,(var_sct *)NULL) != NCO_NOERR)
      return NCO_ERR;

    /* Start at midnight of previous day (last day of previous month) */
    srt_end_dff=srt_end_dff-(24.0-step)*3600.0;

    var_tmp->sz=tpd*2;

    for(idx=0;idx<tpd;idx++){
      bnd_val[2*idx]=(step*idx)*3600;
      bnd_val[2*idx+1]=bnd_val[2*idx]+srt_end_dff;
    }

    cast_void_nctype(NC_DOUBLE,&var_tmp->val);
    var_tmp->val.dp=bnd_val;
    cast_nctype_void(NC_DOUBLE,&var_tmp->val);

    if(nco_cln_clc_dbl_var_dff(srt_sng,unt_sng,cln_typ,(double *)NULL,var_tmp) != NCO_NOERR) return NCO_ERR;

    var_tmp->val.vp=NULL;
  } /* !bnd_val */

  if(var_tmp) var_tmp=nco_var_free(var_tmp);

  return NCO_NOERR;
} /* !nco_clm_nfo_to_tm_bnds() */

#ifndef ENABLE_UDUNITS
/* Stub functions to compile without UDUNITS2 */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_var_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ cln_typ,    /* I [enum] Calendar type of coordinate var */ 
double *val_dbl,        /* I/O [dbl] var values modified */
var_sct *var)           /* I/O [var_sct] var values modified */
{
  (void)fprintf(stderr,"%s: WARNING NCO was built without UDUnits. NCO is therefore unable to convert values from \"%s\" to \"%s\". See http://nco.sf.net/nco.html#rbs for more information.\n%s: HINT Re-build or re-install NCO enabled with UDUnits.\n",nco_prg_nm_get(),fl_unt_sng,fl_bs_sng,nco_prg_nm_get());

  return NCO_NOERR;
} /* !nco_cln_clc_dbl_var_dff() */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_org(   /* [fnc] difference between two co-ordinate units */
const char *val_unt_sng, /* I [ptr] input value and units in the same string */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ cln_typ,    /* I [enum] Calendar type of coordinate var */ 
double *og_val)         /* O [dbl] output value */
{
  *og_val=0.0; /* CEWI */
  (void)fprintf(stderr,"%s: WARNING NCO was built without UDUnits. NCO is therefore unable to interpret the string \"%s\" limit argument.\n%s: HINT Re-build or re-install NCO enabled with UDUnits.\n",nco_prg_nm_get(),val_unt_sng,nco_prg_nm_get());
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
} /* !nco_cln_sng_rbs() */

int
nco_cln_var_prs
(const char *fl_unt_sng,
 nco_cln_typ cln_typ,
 int dt_fmt_enm,
 var_sct *var,
 var_sct *var_ret)
{
  return NCO_ERR;
} /*  !nco_cln_var_prs() */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_prs_tm /* UDUnits2 Extract time stamp from parsed UDUnits string */
(const char *unt_sng, /* I [ptr] units attribute string */
 tm_cln_sct *tm_in) /* O [sct] Time structure to be populated */
{
  (void)fprintf(stderr,"%s: WARNING NCO was built without UDUnits. NCO is therefore unable to interpret the string \"%s\".\n:  HINT Re-build or re-install NCO enabled with UDUnits.\n",nco_prg_nm_get(),unt_sng);

  return NCO_ERR;
}

#endif /* !ENABLE_UDUNITS */

#ifdef ENABLE_UDUNITS
# ifdef HAVE_UDUNITS2_H

/* Make a UDUnits2 converter:
   Used internally by nco_cln_clc_dbl_dff() and nco_cln_clc_var_dff()
   NB: the calling function must free() the converter after use */
cv_converter * /* O [sct] UDUnits converter */
nco_cln_cnv_mk /* [fnc] UDUnits2 create custom converter */
(const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
 const char *fl_bs_sng) /* I [ptr] units attribute string from disk */
{
  const char fnc_nm[]="nco_cln_cnv_mk()"; /* [sng] Function name */
  
  cv_converter *ut_cnv; /* UDUnits converter */

  int ut_rcd; /* [enm] UDUnits2 status */
  
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* [sct] UDUnits structure, input units */
  ut_unit *ut_sct_out; /* [sct] UDUnits structure, output units */
  
  /* UDUnits2 (specifically, the function ut_read_xml()) uses the environment variable UDUNITS2_XML_PATH, if any, to find its all-important XML database named udunits2.xml by default.
     If UDUNITS2_XML_PATH is undefined, UDUnits2 looks in the fall-back default initial location that was hardcoded when the UDUnits2 library was built.
     This location varies depending upon your operating system and UDUnits2 compilation settings.
     If UDUnits2 is correctly linked yet cannot find the XML database in either of these locations,
     then NCO will report that the UDUnits2 library has failed to initialize.
     To fix this, export the location of the UDUnits2 XML database file udunits2.xml to the shell:
     export UDUNITS2_XML_PATH='/opt/local/share/udunits/udunits2.xml'
     One can then invoke (without recompilation) NCO again, and UDUNITS2 should work. */
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: WARNING %s failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stdout,"%s: HINT UDUnits2 (specifically, the function ut_read_xml()) uses the environment variable UDUNITS2_XML_PATH, if any, to find its all-important XML database named by default udunits2.xml. If UDUNITS2_XML_PATH is undefined, UDUnits2 looks in the fall-back default initial location that was hardcoded when the UDUnits2 library was built. This location varies depending upon your operating system and UDUnits2 compilation settings. If UDUnits2 is correctly linked yet cannot find the XML database in either of these locations, then NCO warns that the UDUnits2 library has failed to initialize and prints this message. To fix this, export the full location (path+name) of the UDUnits2 XML database file udunits2.xml to the shell with, e.g.,\n\texport UDUNITS2_XML_PATH='/opt/local/share/udunits/udunits2.xml'\nOne can then invoke (without recompilation) NCO again, and UDUNITS2 should work.\n",nco_prg_nm_get());
    return (cv_converter *)NULL; /* Failure */
  } /* !ut_sys */ 
  
  /* Units string to convert from */
  ut_sct_in=ut_parse(ut_sys,fl_unt_sng,UT_ASCII); 
  if(!ut_sct_in){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute \"%s\" has a syntax error\n",fl_unt_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_unt_sng);
    return (cv_converter *)NULL; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* Units string to convert to */
  ut_sct_out=ut_parse(ut_sys,fl_bs_sng,UT_ASCII); 
  if(!ut_sct_out){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: Empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute  \"%s\" has a syntax error\n",fl_bs_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_bs_sng);
    return (cv_converter *)NULL; /* Failure */
  } /* endif */

  /* Create converter */
  ut_cnv=ut_get_converter(ut_sct_in,ut_sct_out); /* UDUnits converter */
  if(!ut_cnv){
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"WARNING: One of units, %s or %s, is NULL\n",fl_bs_sng,fl_unt_sng);
    if(ut_rcd == UT_NOT_SAME_SYSTEM) (void)fprintf(stderr,"WARNING: Units %s and %s belong to different unit systems\n",fl_bs_sng,fl_unt_sng);
    if(ut_rcd == UT_MEANINGLESS) (void)fprintf(stderr,"WARNING: Conversion between user-specified unit \"%s\" and file units \"%s\" is meaningless\n",fl_bs_sng,fl_unt_sng);
    return (cv_converter *)NULL; /* Failure */
  } /* endif */

  ut_free(ut_sct_in);
  ut_free(ut_sct_out);
  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */
  // cv_free(ut_cnv);

  return ut_cnv;
}  /* UDUnits2 !nco_cln_cnv_mk() */

/* fl_unt_sng - source units
 * fl_bs_sng  - target units
 * *og_val    -  single value of type NC_DOUBLE
 *
 * Conversion done by udunits library.
 * Input and Output value og_val
 */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
double *og_val)
{ /* I/O [dbl] var values modified */
  cv_converter *ut_cnv=NULL; 

  /* Do nothing if units identical */
  if(strcasecmp(fl_unt_sng,fl_bs_sng) == 0) return NCO_NOERR;

  /* Convert */
  ut_cnv=nco_cln_cnv_mk(fl_unt_sng,fl_bs_sng);

  if(ut_cnv != NULL) og_val[0]=cv_convert_double(ut_cnv,og_val[0]); else return NCO_ERR;     

  cv_free(ut_cnv);

  return NCO_NOERR;          
} /* UDUnits2 !nco_cln_clc_dbl_dff() */

/* fl_unt_sng - source units
 * fl_bs_sng  - target units
 * var        -  var_sct array of values.
 *
 * Conversion done by udunits library
 * if var->type less than NC_FLOAT then var is promoted to NC_DOUBLE for conversion then
 * demoted back afterwards. */

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

  nc_type var_typ_org;
  
  /* Do nothing if units identical */
  if(!strcasecmp(fl_unt_sng,fl_bs_sng)) return NCO_NOERR;

  /* Convert */
  ut_cnv=nco_cln_cnv_mk(fl_unt_sng,fl_bs_sng);

  if(ut_cnv == NULL) return NCO_ERR;     

  sz=var->sz;

  var_typ_org=var->type;
  if(!(var->type==NC_FLOAT && var->type==NC_DOUBLE))
    var=nco_var_cnf_typ(NC_DOUBLE,var);

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

 /* convert back to original type */
 var=nco_var_cnf_typ(var_typ_org,var);

 return NCO_NOERR;
} /* !nco_cln_clc_var_dff() */

/* This function is similar to nco_cln_clc_dbl_org() except the value(s) to be converted
   is in og_val or an array of values in var.

   fl_unt_sng - is the source units string
   fl_bs_sng  - is the targets unit string
   cln_typ    - is the calendar type or lmt_nil (as approriate)
   *og_val    - input value or (double*)NULL if var is used
   *var       - input array of values or (var_sct*)NULL if og_val used.
   *
   * if a non standard cln_typ is used.then the nco_cln_utl is used (transparently) to do the conversion
   * otherwise the udunits2 library is used.
   *
   * If var type is less than NC_FLOAT or NC_DOUBLE. Then var is promoted to NC_DOUBLE for the conversion;
   * then demoted back to original type after.
   *
   * Examples of usage:
   * nco_cln_clc_dbl_var_dff("seconds since 2001-01-01","days since 1978-02-04",cln_365,og_val,(var_sct*)NULL);
   *
   * nco_cln_clc_dbl_var_dff("days since 1900-01-01","days since 1950-01-31",cln_std,(double*)NULL,var);
   */

int /* [flg] NCO_NOERR or NCO_ERR */ 
nco_cln_clc_dbl_var_dff( /* [fnc] difference between two co-ordinate units */
const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
nco_cln_typ cln_typ, /* I [enum] Calendar type of coordinate var */ 
double *og_val, /* I/O [dbl] var values modified -can be NULL */
var_sct *var) /* I/O [var_sct] var values modified - can be NULL */
{
  /* Purpose: */
  
  const char fnc_nm[]="nco_cln_clc_dbl_var_dff()"; /* [sng] Function name */

  int is_date;
  int rcd;

  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: %s reports unt_sng=%s, bs_sng=%s, calendar=%d\n",nco_prg_nm_get(),fnc_nm,fl_unt_sng,fl_bs_sng,cln_typ);

  /* Do nothing if units identical */
  if(!strcasecmp(fl_unt_sng,fl_bs_sng)) return NCO_NOERR;

  /* Are target units of the form  "units since date-string" */
  is_date=nco_cln_chk_tm(fl_bs_sng);

  /* Use custom time functions if irregular calendar */
  if(is_date && (cln_typ == cln_360 || cln_typ == cln_365 || cln_typ == cln_366))
    rcd=nco_cln_clc_tm(fl_unt_sng,fl_bs_sng,cln_typ,og_val,var);  
  else if(og_val != (double *)NULL) 
    rcd=nco_cln_clc_dbl_dff(fl_unt_sng,fl_bs_sng,og_val);
  else if(var != (var_sct *)NULL)
    rcd=nco_cln_clc_var_dff(fl_unt_sng,fl_bs_sng,var);
  else
    rcd=NCO_NOERR;
    
  return rcd;
} /* end UDUnits2 nco_cln_clc_dbl_var_dff() */

/* This should be your first port of call to replicate the UDUnits2 command line application functionality
 * val_unt_sng takes the form  "value  units" where val is interpreted as a double
 * fl_bs_sng is of the form "units" and is the target units to be converted to
 *
 * Examples of usage:
 * "10 days since 2001-01-01" "hours since 2001-01-09" - 48 (hours)
 * "5 minutes since 1992-02-29T12:00:00" "seconds since 1992-01-31" 2.5491e+06 (seconds)
 * "1 mile" "m" 1609.34 (m)
 * "12 inches" "cm" 30.48 cm
 *
 * If cln_typ is empty or of type "standard" (cln_std) or "julian" (cln_jul) or "gregorian" (cln_grg) then
 * the conversion is done by the UDUnits2 library.
 *
 * If cln_typ is of type "360_day" (cln_360) or "365_day" (cln_365) or "366_day" (cln_366) then
 * the conversion is done "transparently" by the nco_cln_utl library.
 */

int /* [flg] NCO_NOERR or NCO_ERR */
nco_cln_clc_dbl_org(   /* [fnc] difference between two co-ordinate units */
const char *val_unt_sng, /* I [ptr] input value and units in the same string */
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
nco_cln_typ cln_typ,    /* I [enum] Calendar type of coordinate var */ 
double *og_val)         /* O [dbl] output value */
{
  const char fnc_nm[]="nco_cln_clc_dbl_org()"; /* [sng] Function name */

  int is_date=0;  /* True if unit is date/time */
  int rcd=0;
  int month=0;
  int year=0;
  char lcl_unt_sng[200]={0};
  double val_dbl=0.0;

  rcd=0;  
  val_dbl=0.0;
  is_date=nco_cln_chk_tm(fl_bs_sng);
  lcl_unt_sng[0]='\0';
  
  if(nco_dbg_lvl_get() >= nco_dbg_vrb) (void)fprintf(stderr,"%s: INFO %s reports unt_sng=%s, bs_sng=%s, calendar=%d\n",nco_prg_nm_get(),fnc_nm,val_unt_sng,fl_bs_sng,cln_typ);

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
      (void)fprintf(stderr,"%s: INFO %s reports input string must be of the form \"value unit\" got the string \"%s\"\n",nco_prg_nm_get(),fnc_nm,val_unt_sng);
      nco_exit(EXIT_FAILURE);           
    } /* !ptr */
    strcpy(lcl_unt_sng,ptr); 
  } /* !is_date */

  /* Use custom time functions if irregular calendar */
  if(is_date && (cln_typ == cln_360 || cln_typ == cln_365 || cln_typ == cln_366)){
    rcd=nco_cln_clc_tm(lcl_unt_sng,fl_bs_sng,cln_typ,&val_dbl,(var_sct *)NULL);
  }else rcd=nco_cln_clc_dbl_dff(lcl_unt_sng,fl_bs_sng,&val_dbl);

  /* Copy iff successful */ 
  if(rcd == NCO_NOERR) *og_val=val_dbl; else (void)fprintf(stderr,"%s: ERROR %s reports unt_sng=%s, bs_sng=%s, calendar=%d, og_val=%f\n",nco_prg_nm_get(),fnc_nm,val_unt_sng,fl_bs_sng,cln_typ,val_dbl);  
 
  return rcd;        
} /* !nco_cln_clc_dbl_org() */

int /* [rcd] Successful conversion returns NCO_NOERR */
nco_cln_clc_tm /* [fnc] Difference between two coordinate units */
(const char *fl_unt_sng, /* I [ptr] Units attribute string from disk */
 const char *fl_bs_sng, /* I [ptr] Units attribute string from disk */
 nco_cln_typ cln_typ, /* I [enum] Calendar type of coordinate variable */
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
  if(cln_typ != cln_360 && cln_typ != cln_365 && cln_typ != cln_366){
    (void)fprintf(stderr,"%s: %s reports invalid calendar type cln_typ=%d. Only cln_365, cln_360, and cln_366 allowed.\n",nco_prg_nm_get(),fnc_nm,cln_typ);
    nco_exit(EXIT_FAILURE);
  } /* !cln_typ */

  /* Obtain units type from fl_bs_sng */
  tmp_sng=(char *)nco_calloc(NCO_MAX_LEN_TMP_SNG,sizeof(char));
  if(sscanf(fl_bs_sng,"%s",tmp_sng) != 1) return NCO_ERR;
  bs_tm_typ=nco_cln_get_tm_typ(tmp_sng);
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: DEBUG %s reports unt_sng=\"%s\", bs_sng=\"%s\", tmp_sng=\"%s\"\n",nco_prg_nm_get(),fnc_nm,fl_unt_sng,fl_bs_sng,tmp_sng);
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

  unt_cln_sct.cln_typ=cln_typ;
  bs_cln_sct.cln_typ=cln_typ;
  (void)nco_cln_pop_val(&unt_cln_sct);
  (void)nco_cln_pop_val(&bs_cln_sct);

  /* Get offset */
  crr_val=(unt_cln_sct.value-bs_cln_sct.value)/nco_cln_val_tm_typ(cln_typ,bs_tm_typ);

  /* Scale factor */
  if(unt_tm_typ == bs_tm_typ) scl_val=1.0; else scl_val=nco_cln_val_tm_typ(cln_typ,unt_tm_typ)/nco_cln_val_tm_typ(cln_typ,bs_tm_typ);

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    nco_cln_prn_tm(&unt_cln_sct);
    nco_cln_prn_tm(&bs_cln_sct);
    (void)fprintf(stderr,"%s: %s reports offset=%g, scale factor=%g unt_val=%f bs_val=%f\n",nco_prg_nm_get(),fnc_nm,crr_val,scl_val,unt_cln_sct.value,bs_cln_sct.value);
    if(og_val) (void)fprintf(stderr,", *og_val=%g",*og_val);
    (void)fprintf(stderr,"\n");
  } /* !dbg */

  if(og_val){
    *og_val=(*og_val)*scl_val+crr_val;
  }else if(var){
    size_t sz;
    size_t idx;
    ptr_unn op1;
    /* save  original type if have to promote type to do conversion */
    nc_type var_typ_org;

    var_typ_org=var->type;

    /* promote type to do conversion */
    if( var->type!=NC_FLOAT ||  var->type !=NC_DOUBLE)
      var=nco_var_cnf_typ(NC_DOUBLE,var);


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

    var=nco_var_cnf_typ(var_typ_org,var);
  } /* !var */

  return NCO_NOERR;
} /* !nco_cln_clc_tm() */

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

  /* UDUnits cannot parse non-regular dates for other calendars 
     e.g., "all_leap" (cln_366) has dates like 2001-02-29 that choke UDUnits
     e.g., "360_days" (cln_360) has dates like 1903-2-29, 1903-2-30
     We manually parse the date portion of those strings here */

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
    (void)fprintf(stdout,"%s: ERROR %s failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stdout,"%s: HINT UDUnits2 (specifically, the function ut_read_xml()) uses the environment variable UDUNITS2_XML_PATH, if any, to find its all-important XML database named by default udunits2.xml. If UDUNITS2_XML_PATH is undefined, UDUnits2 looks in the fall-back default initial location that was hardcoded when the UDUnits2 library was built. This location varies depending upon your operating system and UDUnits2 compilation settings. If UDUnits2 is correctly linked yet cannot find the XML database in either of these locations, then NCO warns that the UDUnits2 library has failed to initialize and prints this message. To fix this, export the full location (path+name) of the UDUnits2 XML database file udunits2.xml to the shell with, e.g.,\n\texport UDUNITS2_XML_PATH='/opt/local/share/udunits/udunits2.xml'\nOne can then invoke (without recompilation) NCO again, and UDUNITS2 should work.\n",nco_prg_nm_get());
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
    (void)fprintf(stdout,"%s: ERROR %s failed to initialize UDUnits2 library\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stdout,"%s: HINT UDUnits2 (specifically, the function ut_read_xml()) uses the environment variable UDUNITS2_XML_PATH, if any, to find its all-important XML database named by default udunits2.xml. If UDUNITS2_XML_PATH is undefined, UDUnits2 looks in the fall-back default initial location that was hardcoded when the UDUnits2 library was built. This location varies depending upon your operating system and UDUnits2 compilation settings. If UDUnits2 is correctly linked yet cannot find the XML database in either of these locations, then NCO warns that the UDUnits2 library has failed to initialize and prints this message. To fix this, export the full location (path+name) of the UDUnits2 XML database file udunits2.xml to the shell with, e.g.,\n\texport UDUNITS2_XML_PATH='/opt/local/share/udunits/udunits2.xml'\nOne can then invoke (without recompilation) NCO again, and UDUNITS2 should work.\n",nco_prg_nm_get());
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
} /* !nco_cln_sng_rbs() */

int
nco_cln_var_prs
(const char *fl_unt_sng,
 nco_cln_typ cln_typ,
 int dt_fmt_enm,
 var_sct *var,
 var_sct *var_ret)
{
  const char *fnc_nm="nco_cln_var_prs()";
  const char *bs_sng="seconds since 2001-01-01"; /* [sng] Base units for UDUnits */

  char empty_sng[1];

  double resolution;

  size_t sz;
  size_t idx;

  tm_cln_sct tm;

  empty_sng[0]='\0';

  // if(cln_typ != cln_std) return NCO_ERR;
  if(var->type !=NC_DOUBLE && var->type != NC_FLOAT) nco_var_cnf_typ(NC_DOUBLE,var);

  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: DEBUG %s reports unt_sng=%s, bs_sng=%s, calendar=%d\n",nco_prg_nm_get(),fnc_nm,fl_unt_sng,bs_sng,cln_typ);

  /* Rebase to seconds since blah-blah */
  if(nco_cln_clc_dbl_var_dff(fl_unt_sng,bs_sng,cln_typ,(double *)NULL,var) != NCO_NOERR) return NCO_ERR;

  cast_void_nctype(var->type,&var->val);

  if(var_ret->type != NC_STRING) nco_var_cnf_typ(NC_STRING,var_ret);

  if(var_ret->val.vp) var_ret->val.vp=(void *)nco_free(var_ret->val.vp);

  var_ret->val.vp=nco_malloc(sizeof(nco_string)*var_ret->sz);

  var_ret->has_mss_val=True;
  var_ret->mss_val.vp=nco_malloc(sizeof(nco_string *));

  cast_void_nctype(var_ret->type,&var_ret->val);

  var_ret->mss_val.sngp[0]=strdup(empty_sng);
  sz=var->sz;
  tm.cln_typ=cln_typ;

  // (void)fprintf(stderr,"%s: %s reports var \"%s\" has missing value %d\n",nco_prg_nm_get(),fnc_nm,var->nm,var->has_mss_val);

  if(var->type == NC_DOUBLE){

    double mss_val_dbl;
    if(var->has_mss_val) mss_val_dbl=var->mss_val.dp[0];

    for(idx=0;idx < sz;idx++){
      if(var->has_mss_val && var->val.dp[idx] == mss_val_dbl){
        var_ret->val.sngp[idx]=strdup(empty_sng);
        continue;
      } /* !has_mss_val */

      tm.value=var->val.dp[idx];
      if(cln_typ == cln_360 || cln_typ == cln_365 || cln_typ == cln_366)
        nco_cln_pop_tm(&tm);
      else
        (void)ut_decode_time(tm.value,&tm.year,&tm.month,&tm.day,&tm.hour,&tm.min,&tm.sec,&resolution);

      var_ret->val.sngp[idx]=nco_cln_fmt_dt(&tm,dt_fmt_enm);
    } /* !idx */
  }else if(var->type == NC_FLOAT){

    float mss_val_flt;
    if(var->has_mss_val) mss_val_flt=var->mss_val.fp[0];

    for (idx=0;idx < sz;idx++) {
      if(var->has_mss_val && var->val.fp[idx] == mss_val_flt){
        var_ret->val.sngp[idx]=strdup(empty_sng);
        continue;
      } /* !has_mss_val */

      tm.value=(double)(var->val.fp[idx]);
      if(cln_typ == cln_360 || cln_typ == cln_365 || cln_typ == cln_366)
        nco_cln_pop_tm(&tm);
      else
        (void)ut_decode_time(tm.value,&tm.year,&tm.month,&tm.day,&tm.hour,&tm.min,&tm.sec,&resolution);

      var_ret->val.sngp[idx]=nco_cln_fmt_dt(&tm,dt_fmt_enm);
    } /* !idx */
  } /* !var->type */

  cast_nctype_void(var->type,&var->val);
  cast_nctype_void(var_ret->type,&var->val);

  return NCO_NOERR;
} /* !nco_cln_var_prs() */

# endif /* HAVE_UDUNITS2_H */
#endif /* ENABLE_UDUNITS */

/* !UDUnits-related routines */
