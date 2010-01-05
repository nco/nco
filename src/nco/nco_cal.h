
/* Purpose: Calendar Functions */

/* Copyright (C) 2009--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cal.h" *//* Calendar Functions */

#include <ctype.h>

#ifndef NCO_CAL_H
#define NCO_CAL_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* calendar types */
typedef enum {
  cal_std=1,   /* Standard mixed Julian/Gregorian */
  cal_greg,  /* Gregorian Calendar */ 
  cal_jul,   /* Julian Calendar */ 
  cal_360 ,  /* 30 days a month Calendar */ 
  cal_365,   /* No Leap year Calendar */ 
  cal_366,   /*  Leap year Calendar */ 
  cal_void   /* no calendar found */
} cal_typ;

typedef enum {
  tm_year=1,    
  tm_month,
  tm_day,
  tm_hour,
  tm_min,
  tm_sec,
  tm_void  /* no time units matched */  
} tm_typ;

typedef struct {
  tm_typ sc_typ;
  cal_typ sc_cal;
  int year;
  int month;
  int day;
  int hour;
  int min;
  float sec;
  double value;
} tm_cal_sct;
 
tm_typ              /* [enum] Units type */    
nco_cal_get_tm_typ( /* returns unit type or tm_void if not found */
const char* ud_sng); /* I [ptr] units string  */      

tm_typ               /* [enum] Calendar type */    
nco_cal_get_cal_typ( /* returns Calendar type or cal_void if not found */
const char* ud_sng); /* I [ptr] units string  */      

int                 /* [rcd] Successful conversion returns 0 */
nco_cal_clc_org(    /* [fnc] Difference between two co-ordinate units */      
const char* fl_unt_sng, /* I [ptr] units attribute string from disk  */     
const char* fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
cal_typ lmt_cal,        /* I [enum] Calendar type of coordinate var */ 
double *og_val);        /* O [ptr]                                   */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CAL_H */




