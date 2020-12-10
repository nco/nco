/* $Header$ */

/* Purpose: Calendar utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_cln_utl.h" *//* Calendar utilities */

#ifndef NCO_CLN_UTL_H
#define NCO_CLN_UTL_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <ctype.h> /* isalnum(), isdigit(), tolower() */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */
#ifdef HAVE_STRINGS_H
# include <strings.h> /* strcasecmp() */
#endif /* !HAVE_STRINGS_H */

/* 3rd party vendors */
#ifdef ENABLE_UDUNITS
# ifdef HAVE_UDUNITS2_H
#  include <udunits2.h> /* Unidata units library */
# else
#  include <udunits.h> /* Unidata units library */
# endif /* !HAVE_UDUNITS2_H */
#endif /* !ENABLE_UDUNITS */

#ifdef _MSC_VER
#undef snprintf
/* udunits2 defines snprintf as c99_snprintf, so the above is required  */
#define snprintf _snprintf
#endif

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_sng_utl.h" /* String utilities */

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
    nco_cln_typ cln_typ;
    int year;
    int month;
    int day;
    int hour;
    int min;
    double sec;
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
  
  tm_typ /* [enm] Units type */
  nco_cln_get_tm_typ /* returns time unit type or tm_void if not found */
  (const char *ud_sng); /* I [ptr] units string */

  nco_cln_typ /* O [enm] Calendar type */    
  nco_cln_get_cln_typ /* [fnc] Calendar type or cln_nil if not found */
  (const char *ud_sng); /* I [ptr] units string */
  
  int /* O [int] number of days */
  nco_cln_days_in_year_prior_to_given_month /* [fnc] Number of days in year prior to month */
  (nco_cln_typ cln_typ, /* [enm] calendar type */
   int mth_idx); /* I [idx] Month (1-based counting, December == 12) */
  
  void
  nco_cln_pop_val /* [fnc] Calculate value in cln_sct */ 
  (tm_cln_sct *cln_sct);/* I/O [ptr] Calendar structure */

  void
  nco_cln_pop_tm         /* [fnc] Calculate other members  in cln_sct from value*/
  (tm_cln_sct *cln_sct); /* I/O [ptr] Calendar structure */

  void
  nco_cln_prn_tm         /* [fnc] print tm sct*/
  (tm_cln_sct *cln_sct); /* I [ptr] Calendar structure */

  char*                    /* O [sng] contains newly malloced output string */
  nco_cln_fmt_dt           /*   [fnc] format an output string */
  (tm_cln_sct *cln_sct,   /* I [ptr] Calendar structure */
   int fmt);              /* I [int] format type */

  double /* O [dbl] time in (base) seconds of tm_typ */
  nco_cln_val_tm_typ /* [fnc] */
  (nco_cln_typ cln_typ, /* I [enm] Calendar type */
   tm_typ bs_tm_typ); /* I [enm] Time units */
  
  int /* O [flg] String is calendar date */
  nco_cln_chk_tm /* [fnc] Is string a UDUnits-compatible calendar format, e.g., "PERIOD since REFERENCE_DATE" */
  (const char *unit_sng); /* I [sng] Units string */

  int /* [flg] NCO_NOERR or NCO_ERR */ 
  nco_cln_clc_var_dff /* [fnc] difference between two co-ordinate units */
  (const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
   const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
   var_sct *var); /* I/O [dbl] var values modified */
  
  int /* [flg] NCO_NOERR or NCO_ERR */ 
  nco_cln_clc_dbl_var_dff /* [fnc] difference between two co-ordinate units */
  (const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
   const char *fl_bs_sng,  /* I [ptr] units attribute string from disk */
   nco_cln_typ cln_typ,    /* I [enm] Calendar type of coordinate var */ 
   double *val_dbl,           /* I/O [dbl] var values modified */
   var_sct *var);           /* I/O [var_sct] var values modified */

  int
  nco_cln_var_prs
  (const char *fl_unt_sng,
   nco_cln_typ cln_typ,
   int dt_fmt_enm,
   var_sct *var,
   var_sct *var_out
  );

  int /* [flg] NCO_NOERR or NCO_ERR */ 
  nco_cln_clc_dbl_org /* [fnc] difference between two co-ordinate units */
  (const char *val_unt_sng, /* I [ptr] input value and  units in the same string */
   const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
   nco_cln_typ cln_typ, /* I [enm] Calendar type of coordinate var */ 
   double *og_val); /* O [dbl] output value */
  
  int /* [flg] NCO_NOERR or NCO_ERR */ 
  nco_cln_clc_dbl_var_dff /* [fnc] difference between two co-ordinate units */
  (const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
   const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
   nco_cln_typ cln_typ, /* I [enm] Calendar type of coordinate var */ 
   double *val_dbl, /* I/O [dbl] var values modified */
   var_sct *var); /* I/O [var_sct] var values modified */

  int /* [rcd] Return code */
  nco_cln_sng_rbs /* [fnc] Rebase calendar string for legibility */
  (const ptr_unn val, /* I [sct] Value to rebase */
   const long val_idx, /* I [idx] Index into 1-D array of values */
   const nc_type val_typ, /* I [enm] Value type */
   const char *unit_sng, /* I [sng] Units string */
   char *lgb_sng); /* O [sng] Legible version of input string */

  int /* [flg] NCO_NOERR or NCO_ERR */
  nco_cln_prs_tm /* Extract time stamp from a parsed udunits string */
  (const char *unt_sng, /* I [ptr] units attribute string */  
   tm_cln_sct *tm_in); /*  O [sct] struct to be populated */

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
   double *tm_val); /* O [frc] Time coordinate values */

#ifdef ENABLE_UDUNITS
# ifdef HAVE_UDUNITS2_H

  cv_converter * /* UDUnits converter */
  nco_cln_cnv_mk /* [fnc] UDUnits2 create a custom converter  */
  (const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
   const char *fl_bs_sng);/* I [ptr] units attribute string from disk */

  int /* [flg] NCO_NOERR or NCO_ERR */ 
  nco_cln_clc_dbl_dff /* [fnc] difference between two co-ordinate units */
  (const char *fl_unt_sng, /* I [ptr] units attribute string from disk */
   const char *fl_bs_sng, /* I [ptr] units attribute string from disk */
   double *og_val);

  int /* O [flg] NCO_NOERR or NCO_ERR */ 
  nco_cln_clc_tm /* [fnc] Difference between two time coordinate units */
  (const char *fl_unt_sng, /* I [ptr] user units attribute string */
   const char *fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
   nco_cln_typ cln_typ,  /* I [enm] Calendar type of coordinate var */ 
   double *rgn_val, /* I/O [ptr] time diff in units based on fl_bs_sng */ 
   var_sct *var); /* I/O [ptr] */ 

# endif /* !HAVE_UDUNITS2_H */
#endif /* !ENABLE_UDUNITS */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CLN_UTL_H */
