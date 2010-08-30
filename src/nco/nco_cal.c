/* Purpose: Multi-calendar functions */

/* Copyright (C) 2009--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Array to hold calendar type units */
/* Format year,month,day,hour,min,sec,origin,offset */   
double DATA_360[8]={31104000,2592000,86400,3600,60,1,0.0,0.0};
double DATA_365[8]={31536000,2628000,86400,3600.0,60.0,1,0.0,0.0};

/* Days in months */
int DAYS360[12]={30,30,30,30,30,30,30,30,30,30,30,30};
int DAYS365[12]={31,28,31,30,31,30,31,31,30,31,30,31};
int DAYS366[12]={31,29,31,30,31,30,31,31,30,31,30,31};

#ifdef ENABLE_UDUNITS
# ifdef HAVE_UDUNITS2_H

/****************************************************************************************************/
/******************** UDUNITS 2 *********************************************************************/
/****************************************************************************************************/

int   /* O  difference between two co-ordinate units */      
nco_cal_clc_dff(
		const char* fl_unt_sng, /* I [ptr] units attribute string from disk  */     
		const char* fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
		double crr_val,
		double *og_val)         /* O difference between two units string */
{
  int ut_rcd; /* [enm] UDUnits2 status */
  double in_val=0.0;
  
  cv_converter *ut_cnv; /* UDUnits converter */
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* UDUnits structure, input units */
  ut_unit *ut_sct_out; /* UDUnits structure, output units */
  
  /* quick return if units identical */
  if(!strcmp(fl_unt_sng,fl_bs_sng) ){
    *og_val=crr_val;  
    return EXIT_SUCCESS;
  }
  
  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: nco_udu_lmt_cnv() failed to initialize UDUnits2 library\n",prg_nm_get());
    return EXIT_FAILURE; /* Failure */
  } /* end if err */ 
  
  /* units string to convert from */
  ut_sct_in=ut_parse(ut_sys,fl_unt_sng,UT_ASCII); 
  if(ut_sct_in == NULL){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR:  units attribute \"%s\" has a syntax error\n",fl_unt_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_unt_sng);

    return EXIT_FAILURE; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* units string to convert to */
  ut_sct_out=ut_parse(ut_sys,fl_bs_sng,UT_ASCII); 
  if(ut_sct_out == NULL){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: Empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR: units attribute  \"%s\" has a syntax error\n",fl_bs_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",fl_bs_sng);

      return EXIT_FAILURE; /* Failure */
  } /* endif */

  /* Create a converter */
  ut_cnv=ut_get_converter(ut_sct_in,ut_sct_out); /* UDUnits converter */
  if(ut_cnv == NULL){
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: One of units is NULL\n");
    if(ut_rcd == UT_NOT_SAME_SYSTEM) (void)fprintf(stderr,"ERROR: Units belong to different unit systems\n");
    if(ut_rcd == UT_MEANINGLESS) (void)fprintf(stderr,"ERROR: Conversion between user specified unit \"%s\" and file units \"%s\" is  is meaningless\n",fl_bs_sng,fl_unt_sng);
    return EXIT_FAILURE; /* Failure */
  } /* endif */

  /* Finally do the conversion  */
  *og_val=cv_convert_double(ut_cnv,crr_val);
  
  if(dbg_lvl_get() >nco_dbg_std) {
    fprintf(stderr, "%s : nco_cal_clc_dff: difference between systems \"%s\" and \"%s\" is %f\n",prg_nm_get(),fl_unt_sng,fl_bs_sng, *og_val);
  }

  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */
  ut_free(ut_sct_in);
  ut_free(ut_sct_out);
  cv_free(ut_cnv);

  return EXIT_SUCCESS;

}  /* end nco_cal_clc_dff() */

int                  /* [rcd] Successful conversion returns 0 */     
nco_cal_prs_tm(      /* Extract time stamp from a parsed udunits string */
const char *unt_sng, /* I [ptr] units attribute string   */            
tm_cal_sct *tm_in){ /*  O [sct] struct to be populated   */             
  int ut_rcd; /* [enm] UDUnits2 status */
  char buf[200];
  char *bptr;
    
  cv_converter *ut_cnv; /* UDUnits converter */
  ut_system *ut_sys;
  ut_unit *ut_sct_in; /* UDUnits structure, input units */

  /* When empty, ut_read_xml() uses environment variable UDUNITS2_XML_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  if(dbg_lvl_get() >= nco_dbg_vrb) ut_set_error_message_handler(ut_write_to_stderr); else ut_set_error_message_handler(ut_ignore);
  ut_sys=ut_read_xml(NULL);
  if(ut_sys == NULL){
    (void)fprintf(stdout,"%s: nco_cal_prs_tm() failed to initialize UDUnits2 library\n",prg_nm_get());
    return EXIT_FAILURE; /* Failure */
  } /* end if err */ 

  /* units string to convert from */
  ut_sct_in=ut_parse(ut_sys,unt_sng,UT_ASCII); 
  if(ut_sct_in == NULL){ /* Problem with 'units' attribute */
    ut_rcd=ut_get_status(); /* [enm] UDUnits2 status */
    if(ut_rcd == UT_BAD_ARG) (void)fprintf(stderr,"ERROR: empty units attribute string\n");
    if(ut_rcd == UT_SYNTAX) (void)fprintf(stderr,"ERROR:  units attribute \"%s\" has a syntax error\n",unt_sng);
    if(ut_rcd == UT_UNKNOWN) (void)fprintf(stderr,"ERROR: units attribute \"%s\" is not listed in UDUnits2 SI system database\n",unt_sng);

    return EXIT_FAILURE; /* Failure */
  } /* endif coordinate on disk has no units attribute */

  /* this prints out the timestamp to buf in a standard dependable format */
  ut_format(ut_sct_in,buf,sizeof(buf), UT_ASCII|UT_NAMES);

  /*its a bit sloppy but this is the only way to extract the parsed units */
  /* extract time info from print string */
  bptr=strstr(buf,"since");  
  sscanf( bptr,"%*s %d-%d-%d %d:%d:%f",&tm_in->year,&tm_in->month,&tm_in->day,&tm_in->hour,&tm_in->min,&tm_in->sec);

  ut_free_system(ut_sys); /* Free memory taken by UDUnits library */
  ut_free(ut_sct_in);

return EXIT_SUCCESS;

} /* end nco_cal_prs_tm */

# else /* !HAVE_UDUNITS2_H */

/****************************************************************************************************/
/******************** UDUNITS 1 *********************************************************************/
/****************************************************************************************************/

int                 /* [rcd] Successful conversion returns 0 */
nco_cal_clc_dff(    /* [fnc] Difference between two co-ordinate units */      
const char* fl_unt_sng, /* I [ptr] units attribute string from disk  */     
const char* fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
double crr_val,
double *og_val){        /* O [ptr]                                   */

  int rcd;

  double slp;
  double incpt;

  utUnit udu_sct_in; /* UDUnits structure, input units */
  utUnit udu_sct_out; /* UDUnits structure, output units */


  /* quick return if units identical */
  if(!strcmp(fl_unt_sng,fl_bs_sng) ){
    *og_val=crr_val;  
    return EXIT_SUCCESS;
  }

#ifdef UDUNITS_PATH
  /* UDUNITS_PATH macro expands to where autoconf found database file */
  rcd=utInit(UDUNITS_PATH);
# else /* !UDUNITS_PATH */
  /* When empty, utInit() uses environment variable UDUNITS_PATH, if any
     Otherwise it uses default initial location hardcoded when library was built */
  rcd=utInit("");
# endif /* !UDUNITS_PATH */

  if(rcd != 0){
    (void)fprintf(stdout,"%s: nco_lmt_cls_dff() failed to initialize UDUnits library\n",prg_nm_get());

    return EXIT_FAILURE;

  } /* end if err */ 

  /* units string to convert from */
  rcd=utScan(fl_unt_sng,&udu_sct_in); 
  if(rcd !=0){
    if(rcd == UT_EINVALID)
      (void)fprintf(stderr,"ERROR: units attribute \"%s\" is invalid \n",fl_unt_sng);

    if(rcd == UT_ESYNTAX)
      (void)fprintf(stderr,"ERROR units attribute \"%s\" contains a syntax error",fl_unt_sng);

    if(rcd == UT_EUNKNOWN)
      (void)fprintf(stderr,"ERROR units attribute \"%s\" is not in udunits database",fl_unt_sng);

    (void)utTerm(); /* Free memory taken by UDUnits library */
    return EXIT_FAILURE;
  } /* endif unkown type */

  /* units string to convert to */
  rcd=utScan(fl_bs_sng,&udu_sct_out); 
  if(rcd !=0){
    if(rcd == UT_EINVALID)
      (void)fprintf(stderr,"ERROR: units attribute \"%s\" is invalid \n",fl_bs_sng);

    if(rcd == UT_ESYNTAX)
      (void)fprintf(stderr,"ERROR units attribute \"%s\" contains a syntax error",fl_bs_sng);

    if(rcd == UT_EUNKNOWN)
      (void)fprintf(stderr,"ERROR units attribute \"%s\" is not in udunits database",fl_bs_sng);

    (void)utTerm(); /* Free memory taken by UDUnits library */
    return EXIT_FAILURE;
  } /* endif unkown type */

  rcd=utConvert(&udu_sct_in,&udu_sct_out,&slp,&incpt);
  if(rcd == UT_ECONVERT){
    (void)fprintf(stderr,"ERROR: user specified unit \"%s\" cannot be converted to units \"%s\"\n",fl_unt_sng,fl_bs_sng);
    (void)utTerm();
    return EXIT_FAILURE;
  } /* endif */

  *og_val=crr_val*slp+incpt;

  /* debug stuff */
  if(dbg_lvl_get() >nco_dbg_std) 
    fprintf(stderr, "%s : nco_cal_clc_org: difference between systems \"%s\" and \"%s\" is %f\n",prg_nm_get(),fl_unt_sng,fl_bs_sng, *og_val);

  (void)utTerm();

  return EXIT_SUCCESS;   
}

int                  /* [rcd] Successful conversion returns 0 */     
nco_cal_prs_tm(      /* Extract time stamp from a parsed udunits string */
const char *unt_sng, /* I [ptr] units attribute string   */            
tm_cal_sct *tm_in){ /*  O [sct] struct to be populated   */             

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

  if(rcd != 0){
    (void)fprintf(stdout,"%s: nco_cal_prs_tm() failed to initialize UDUnits library\n",prg_nm_get());
    return EXIT_FAILURE;
  } /* end if err */ 

  /* units string to convert from */
  rcd=utScan(unt_sng,&udu_sct_in); 
  if(rcd !=0){
    if(rcd == UT_EINVALID)
      (void)fprintf(stderr,"ERROR: units attribute \"%s\" is invalid \n",unt_sng);

    if(rcd == UT_ESYNTAX)
      (void)fprintf(stderr,"ERROR units attribute \"%s\" contains a syntax error",unt_sng);

    if(rcd == UT_EUNKNOWN)
      (void)fprintf(stderr,"ERROR units attribute \"%s\" is not in udunits database",unt_sng);

    (void)utTerm(); /* Free memory taken by UDUnits library */
    return EXIT_FAILURE;
  } /* endif unkown type */


    /* Extract time origin */
  if(utIsTime(&udu_sct_in)){
    utCalendar(0.0,&udu_sct_in,&tm_in->year,&tm_in->month,&tm_in->day,&tm_in->hour,&tm_in->min,&tm_in->sec);
    rcd=EXIT_SUCCESS;  
  }else{
    rcd=EXIT_FAILURE;
  }

 (void)utTerm(); /* Free memory taken by UDUnits library */
 return rcd;
} /* end nco_cal_prs_tm */

# endif /*!HAVE_UDUNITS2 */
#else /* !ENABLE_UDUNITS */

/****************************************************************************************************/
/******************** NO UDUNITS  *******************************************************************/
/****************************************************************************************************/

/* Generate dummy (stub) functions so that compilation works without UDUnits 1/2 */

int                 /* [rcd] Successful conversion returns 0 */
nco_cal_clc_dff(    /* [fnc] Difference between two co-ordinate units */      
const char* fl_unt_sng, /* I [ptr] units attribute string from disk  */     
const char* fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
double crr_val,
double *og_val){        /* O [ptr]                                   */

return EXIT_SUCCESS;
}

int                  /* [rcd] Successful conversion returns 0 */     
nco_cal_prs_tm(      /* Extract time stamp from a parsed udunits string */
const char *unt_sng, /* I [ptr] units attribute string   */            
tm_cal_sct *tm_in){ /*  O [sct] struct to be populated   */             
return EXIT_SUCCESS;
}

#endif /* !ENABLE_UDUNITS */

/****************************************************************************************************/
/******************** END ALL UDUNITS  ************************* ************************************/
/****************************************************************************************************/
 
tm_typ              /* [enum] Units type */    
nco_cal_get_tm_typ /* returns unit type or tm_void if not found */
(const char* ud_sng){ /* I [ptr] units string  */      
int idx;
int len; 
char *lcl_sng;  
tm_typ ret_typ;
 
  lcl_sng=strdup(ud_sng);
  
  /* initially set ret type to void */
  ret_typ=tm_void;   

  /* convert to lower case */
  len=strlen(lcl_sng);
  for(idx=0 ; idx<len ; idx++)
    lcl_sng[idx]=tolower(lcl_sng[idx]);
  
  if(!strcmp(lcl_sng,"year") || !strcmp(lcl_sng,"years") ) 
    ret_typ=tm_year;
  else if(!strcmp(lcl_sng,"month") || !strcmp(lcl_sng,"months") ) 
    ret_typ=tm_month;
  else if(!strcmp(lcl_sng,"day") || !strcmp(lcl_sng,"days") ) 
    ret_typ=tm_day;
  else if(!strcmp(lcl_sng,"hour") || !strcmp(lcl_sng,"hours") ) 
    ret_typ=tm_hour;
  else if(!strcmp(lcl_sng,"min") || !strcmp(lcl_sng,"mins")|| !strcmp(lcl_sng,"minute") || !strcmp(lcl_sng,"minutes") ) 
    ret_typ=tm_min;
  else if(!strcmp(lcl_sng,"sec") || !strcmp(lcl_sng,"secs")|| !strcmp(lcl_sng,"second") || !strcmp(lcl_sng,"seconds") ) 
    ret_typ=tm_sec;

  return ret_typ;
}

tm_typ               /* [enum] Calendar type */    
nco_cal_get_cal_typ /* returns Calendar type or cal_void if not found */
(const char* ud_sng) /* I [ptr] units string  */      
{
int idx;
int len; 
char *lcl_sng;  
cal_typ ret_typ;

 if(!ud_sng)
   return (int)cal_void;
 
  lcl_sng=strdup(ud_sng);
  
  /* initially set ret type to void */
  ret_typ=cal_void;   

  /* convert to lower case */
  len=strlen(lcl_sng);
  for(idx=0 ; idx<len ; idx++)
    lcl_sng[idx]=tolower(lcl_sng[idx]);


  if(!strcmp(lcl_sng, "standard") )
    ret_typ=cal_std;
  else if(!strcmp(lcl_sng, "gregorian") || !strcmp(lcl_sng, "proleptic_gregorian") )
    ret_typ=cal_greg;
  else if(!strcmp(lcl_sng, "julian") )
    ret_typ=cal_jul;
  else if(!strcmp(lcl_sng, "360_day"))
    ret_typ=cal_360;
  else if(!strcmp(lcl_sng, "no_leap") || !strcmp(lcl_sng,"365_day" ))
    ret_typ=cal_365;
  else if(!strcmp(lcl_sng, "all_leap") || !strcmp(lcl_sng,"366_day" ))
    ret_typ=cal_366;

  return ret_typ;
}

int
nco_cal_mths2days(
cal_typ lmt_cal,   
int months){
int *days;
int idx;
int idays=0;

 switch(lmt_cal){
   case cal_360:
     days=DAYS360;
     break; 
   case cal_365:
     days=DAYS365;
     break; 
   case cal_366: 
     days=DAYS366;
     break;  
 }

 months--;

 for(idx=0 ;idx<months ;idx++)
   idays+=days[idx];
  
 return idays;
}

void
nco_cal_pop_val(
tm_cal_sct *cal_sct){
double *data;

 switch(cal_sct->sc_cal) {
   case cal_360:
     data=DATA_360;
     cal_sct->value=data[0]*(cal_sct->year-1)+
                    data[1]*(cal_sct->month-1)+
                    data[2]*(cal_sct->day-1)+
                    data[3]*cal_sct->hour+
                    data[4]*cal_sct->min+
                    data[5]*(double)cal_sct->sec;
     break; 
   case cal_365:  
     data=DATA_365;    
     cal_sct->value=data[0]*(cal_sct->year-1)+
                    data[2]*nco_cal_mths2days(cal_365,cal_sct->month)+
                    data[2]*(cal_sct->day-1)+
                    data[3]*cal_sct->hour+
                    data[4]*cal_sct->min+
                    data[5]*(double)cal_sct->sec;
     break;
   case cal_366:
     break;
 }

 return;
}

double nco_cal_rel_val(
double offset, 
cal_typ lmt_cal,
tm_typ bs_tm_typ){

double *data;
double scl;

 switch(lmt_cal) {
   case cal_360:
     data=DATA_360;    
     break; 
   case cal_365:  
     data=DATA_365;    
     break;
   case cal_366:
     break;
 }

 /* switch for type */
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
}


int                 /* [rcd] Successful conversion returns 0 */
nco_cal_clc_tm(    /* [fnc] Difference between two co-ordinate units */      
const char* fl_unt_sng, /* I [ptr] units attribute string from disk  */     
const char* fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
cal_typ lmt_cal,        /* [enum] Calendar type of coordinate var */ 
double *og_val){        /* O [ptr]                                   */

int rcd;
int year;
int month;
char *lcl_unt_sng;
char tmp_sng[100]; 
double crr_val;

tm_typ bs_tm_typ;     /* enum for the units type in fl_bs_sng */
tm_cal_sct unt_cal_sct;
tm_cal_sct bs_cal_sct;
  
if(dbg_lvl_get() >nco_dbg_std){fprintf(stderr, "%s : nco_cal_clc_tm: unt_sng=%s bs_sng=%s\n",prg_nm_get(),fl_unt_sng,fl_bs_sng);}

/* see if fl_unt_sng  looks like a regular timestamp */ 
    if(sscanf(fl_unt_sng,"%d-%d",&year,&month)==2){
      lcl_unt_sng=(char*)nco_malloc( (strlen(fl_unt_sng)+3) *sizeof(char) );
      strcpy(lcl_unt_sng,"s@");
      strcat(lcl_unt_sng,fl_unt_sng);

  }else{
    lcl_unt_sng=strdup(fl_unt_sng);
  }

 /* temporary untill we deal with more calendar types */
 if(lmt_cal != cal_360 && lmt_cal != cal_365 ) {
   rcd= nco_cal_clc_dff(lcl_unt_sng,fl_bs_sng,0.0,og_val);
   lcl_unt_sng=(char*)nco_free(lcl_unt_sng);
   return rcd;
 }

 /* get the units type from fl_bs_sng */
 if( sscanf(fl_bs_sng,"%s",tmp_sng) !=1 )
   return EXIT_FAILURE;

 bs_tm_typ=nco_cal_get_tm_typ(tmp_sng);  

 /* assume from here on dealing with non-standard calendar */ 
 if( nco_cal_prs_tm(lcl_unt_sng, &unt_cal_sct) )
   return EXIT_FAILURE;

 if( nco_cal_prs_tm(fl_bs_sng, &bs_cal_sct) )
   return EXIT_FAILURE;

 unt_cal_sct.sc_typ=bs_tm_typ;
 bs_cal_sct.sc_typ=bs_tm_typ;
 
 unt_cal_sct.sc_cal=lmt_cal;
 bs_cal_sct.sc_cal=lmt_cal;


 (void)nco_cal_pop_val(&unt_cal_sct);
 (void)nco_cal_pop_val(&bs_cal_sct);

 crr_val=nco_cal_rel_val( unt_cal_sct.value-bs_cal_sct.value,lmt_cal,bs_tm_typ);                 

 *og_val=crr_val;

return EXIT_SUCCESS;
}

int                 /* [rcd] Successful conversion returns 0 */
nco_cal_clc_org(    /* [fnc] Difference between two co-ordinate units */      
const char* fl_unt_sng, /* I [ptr] units attribute string from disk  */     
const char* fl_bs_sng,  /* I [ptr] units attribute string from disk  */     
cal_typ lmt_cal,        /* [enum] Calendar type of coordinate var */ 
double *og_val){        /* O [ptr]                                   */

  int rcd;
  
  char *usr_unt_sng;   
  double crr_val;
  double incpt;

  /* check if units output  is a timestamp if so call special time routine */
  if( strstr(fl_bs_sng," from ") || strstr(fl_bs_sng," since ") || strstr(fl_bs_sng," after ") ){
    rcd=nco_cal_clc_tm(fl_unt_sng,fl_bs_sng,lmt_cal,og_val);   
    return rcd;
  }  
  
  /* From here on assume a regular conversion */
  /* assume fl_unt_sng is of the form 'double value units */
  /* e.g '10 inches' , '100 foot' */

   usr_unt_sng=(char*)nco_calloc( strlen(fl_unt_sng)+1, sizeof(char));

  sscanf(fl_unt_sng,"%lg %s", &crr_val,usr_unt_sng);

  rcd=nco_cal_clc_dff(usr_unt_sng,fl_bs_sng,crr_val,og_val);
  
  usr_unt_sng=(char*)nco_free(usr_unt_sng);
  return rcd;  
} /* end nco_cal_clc_org */
