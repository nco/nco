/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cln_utl.h,v 1.29 2011-03-21 22:53:48 zender Exp $ */

/* Purpose: Calendar utilities */

/* Copyright (C) 1995--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_cln_utl.h" *//* Calendar utilities */

#ifndef NCO_CLN_UTL_H
#define NCO_CLN_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <ctype.h>

/* 3rd party vendors */
#ifdef ENABLE_UDUNITS
# ifdef HAVE_UDUNITS2_H
#  include <udunits2.h> /* Unidata units library */
# else
#  include <udunits.h> /* Unidata units library */
# endif /* !HAVE_UDUNITS2_H */
#endif /* !ENABLE_UDUNITS */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Calendar types */
typedef enum {
  tm_year=1,    
  tm_month,
  tm_day,
  tm_hour,
  tm_min,
  tm_sec,
  tm_void  /* No time units matched */  
} tm_typ;

typedef struct {
  tm_typ sc_typ;
  cln_typ sc_cln;
  int year;
  int month;
  int day;
  int hour;
  int min;
  float sec;
  double value;
} tm_cln_sct;

int /* O [nbr] Number of days to end of month */
nco_nd2endm /* [fnc] Compute number of days to end of month */
(const int mth, /* I [mth] Month */
 const int day); /* I [day] Current day */

nco_int /* O [YYMMDD] Date a specified number of days from input date */
nco_newdate /* [fnc] Compute date a specified number of days from input date */
(const nco_int date, /* I [YYMMDD] Date */
 const nco_int day_srt); /* I [day] Days ahead of input date */

int               /* [flg] SUCCESS/FALURE */ 
nco_cln_clc_dff(  /* [fnc] difference between two co-ordinate units */      
const char *fl_unt_sng, /* I [ptr] units attribute string from disk  */     
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
double crr_val,         /* I [dbl] input units value */
double *og_val);         /* O difference between two units string */

int                  /* [flg] SUCCESS/FALURE */ 
nco_cln_prs_tm(      /* Extract time stamp from a parsed udunits string */
const char *unt_sng, /* I [ptr] units attribute string   */            
tm_cln_sct *tm_in); /*  O [sct] struct to be populated   */             

tm_typ              /* [enum] Units type */    
nco_cln_get_tm_typ( /* returns time unit type or tm_void if not found */
const char *ud_sng); /* I [ptr] units string  */      

cln_typ               /* [enum] Calendar type */    
nco_cln_get_cln_typ( /* [fnc] Calendar type or cln_nil if not found */
const char *ud_sng); /* I [ptr] units string  */      

int                /* O [int] number of days */
nco_cln_mth2day( /* [fnc] number of days in months */
cln_typ lmt_cln,   /* [enum] calendar type */
int months);       /* I [int] month */ 

void
nco_cln_pop_val(     /* [fnc] Calculate value in cln_sct */ 
tm_cln_sct *cln_sct);/* I/O [ptr] structure */

double               /* O [dbl] relative time */
nco_cln_rel_val(     /* [fnc] */   
double offset,          /* I [dbl] time in base units */
cln_typ lmt_cln,        /* I [enum] Calendar type */ 
tm_typ bs_tm_typ);      /* I [enum] Time units */

int                 /* O [flg] SUCCESS/FAILURE 0 */
nco_cln_clc_tm(    /* [fnc] Difference between two co-ordinate units */      
const char *fl_unt_sng, /* I [ptr] user units attribute string   */     
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
cln_typ lmt_cln,        /* [enum] Calendar type of coordinate var */ 
double *og_val);        /* O [ptr] time diff in units based on fl_bs_sng */ 

int                 /* [rcd] Successful conversion returns 0 */
nco_cln_clc_org(    /* [fnc] Difference between two co-ordinate units */      
const char *fl_unt_sng, /* I [ptr] units attribute string from disk  */     
const char *fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
cln_typ lmt_cln,        /* I [enum] Calendar type of coordinate var */ 
double *og_val);        /* O [ptr]                                   */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CLN_UTL_H */
