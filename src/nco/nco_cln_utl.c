/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cln_utl.c,v 1.1 2002-05-02 06:10:30 zender Exp $ */

/* Purpose: Calendar utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_cln_utl.h" /* Calendar utilities */

int /* O [nbr] Number of days to end of month */
nd2endm /* [fnc] Compute number of days to end of month */
(const int mth, /* I [mth] Month */
 const int day) /* I [day] Current day */
{
  /* Purpose: Returns number of days to end of month  
     This number added to the input argument day gives last day of month mth
     Original fortran: Brian Eaton cal_util.F:nd2endm()
     C version: Charlie Zender */
  int nbr_day_2_mth_end;
  int mdays[]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  if(mth < 1 || mth > 12 || day < 0){
    (void)fprintf(stdout,"%s: ERROR nd2endm() reports mth = %d, day = %d\n",prg_nm_get(),mth,day);
    exit(EXIT_FAILURE);
  } /* end if */

  nbr_day_2_mth_end=mdays[mth-1]-day;

  return nbr_day_2_mth_end;
} /* nd2endm */

nco_long /* O [YYMMDD] Date a specified number of days from input date */
newdate /* [fnc] Compute date a specified number of days from input date */
(const nco_long date, /* I [YYMMDD] Date */
 const int day_srt) /* I [day] Days ahead of input date */
{
  /* Purpose: Find date a specified number of days (possibly negative) from given date 
     Original fortran: Brian Eaton cal_util.F:newdate()
     C version: Charlie Zender */

  /* Local */
  int date_srt; /* Initial value of date (may change sign) */
  int day_nbr_2_eom; /* Days to end of month */
  int day_crr; /* Day of date */
  int day_ncr; /* Running count of days to increment date by */
  int mth_crr; /* Month of date */
  int mth_idx; /* Index */
  int mth_srt; /* Save the initial value of month */
  int mth_tmp; /* Current month as we increment date */
  int yr_crr; /* Year of date */
  int mth_day_nbr[]= /* Number of days in each month */
  { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31,
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  nco_long newdate_YYMMDD; /* New date in YYMMDD format */

  if(day_srt == 0) return date;

  date_srt=date;
  yr_crr=date_srt/10000;
  if(date_srt < 0) date_srt=-date_srt;
  mth_crr=(date_srt%10000)/100;
  mth_srt=mth_crr;
  day_crr=date_srt%100;
      
  if(day_srt > 0){
    day_ncr=day_srt;
    yr_crr+=day_ncr/365;
    day_ncr=day_ncr%365;
    for(mth_idx=mth_srt;mth_idx<=mth_srt+12;mth_idx++){
      mth_tmp=mth_idx;
      if(mth_idx > 12) mth_tmp=mth_idx-12;
      day_nbr_2_eom=nd2endm(mth_tmp,day_crr);
      if(day_ncr > day_nbr_2_eom){
	mth_crr++;
	if(mth_crr > 12){
	  mth_crr=1;
	  yr_crr++;
	} /* end if */
	day_crr=1;
	day_ncr-=day_nbr_2_eom+1;
	if(day_ncr == 0) break;
      }else{
	day_crr=day_crr+day_ncr;
	break;
      } /* end if */
    } /* end loop over mth */
    /* Previous two breaks continue execution here */
  }else if(day_srt < 0){
    day_ncr=-day_srt;
    yr_crr=yr_crr-day_ncr/365;
    day_ncr=day_ncr%365;
    mth_srt=mth_crr;
    for(mth_idx=mth_srt+12;mth_idx>=mth_srt;mth_idx--){
      if(day_ncr >= day_crr){
	mth_crr--;
	if(mth_crr < 1){
	  mth_crr=12;
	  yr_crr--;
	} /* end if */
	day_ncr-=day_crr;
	day_crr=mth_day_nbr[mth_crr-1];
	if(day_ncr == 0) break;
      }else{
	day_crr-=day_ncr;
	break;
      } /* end if */
    } /* end loop over mth */
    /* Previous two breaks continue execution here */
  } /* end if */

  if(yr_crr >= 0){
    newdate_YYMMDD=yr_crr*10000+mth_crr*100+day_crr;
  }else{
    newdate_YYMMDD=-yr_crr*10000+mth_crr*100+day_crr;
    newdate_YYMMDD=-newdate_YYMMDD;
  } /* end if */

  return newdate_YYMMDD;
} /* end newdate() */

