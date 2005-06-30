/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.c,v 1.17 2005-06-30 18:34:02 zender Exp $ */

/* Purpose: CCSM conventions */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_cnv_csm.h" /* CCSM conventions */

bool /* O [flg] File obeys CCSM conventions */
nco_ncar_csm_inq /* O [fnc] Check if file obeys CCSM conventions */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* Purpose: Check if file adheres to NCAR CCSM history tape format */

  bool NCAR_CCSM=False;

  char *att_val;

  char cnv_sng[]="Conventions"; /* Unidata standard string */
  
  int rcd; /* [rcd] Return code */
  
  long att_sz;

  nc_type att_typ;

  /* Look for signature of an NCAR CCSM format file */
  rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);

  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NULL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1);
    (void)nco_get_att(nc_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    /* CCM3, CCSM1 conventions */
    if(strstr(att_val,"NCAR-CSM") != NULL) NCAR_CCSM=True; /* Backwards compatibility */
    /* Climate-Forecast conventions */
    if(strstr(att_val,"CF-1.0") != NULL) NCAR_CCSM=True; /* NB: Not fully implemented TODO nco145 */
    if(NCAR_CCSM && dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: CONVENTION File convention is %s\n",prg_nm_get(),att_val);
    att_val=(char *)nco_free(att_val);
  } /* endif */

  return NCAR_CCSM;
  
} /* end nco_ncar_csm_inq */

void
nco_ncar_csm_date /* [fnc] Fix date variable in averaged CCSM files */
(const int nc_id, /* I [id] netCDF file ID */
 X_CST_PTR_CST_PTR_Y(var_sct,var), /* I/O [sct] Variables in output file */
 const int nbr_var) /* I [nbr] Number of variables in list */
{
  /* Purpose: Fix date variable in averaged CCSM files */
  char wrn_sng[1000];

  int date_idx;
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int time_idx;
  
  long day;

  int nbdate_id;
  
  nco_int nbdate;
  nco_int date;
  
  (void)sprintf(wrn_sng,"Most, but not all, CCSM files which are in CCM format contain the fields \"nbdate\", \"time\", and \"date\". When the \"date\" field is present but either \"nbdate\" or \"time\" is missing, then %s is unable to construct a meaningful average \"date\" to store in the output file. Therefore the \"date\" variable in your output file may be meaningless.\n",prg_nm_get());

  /* Find date variable (NC_INT: current date as 6 digit integer (YYMMDD)) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"date")) break;
  } /* end loop over idx */
  if(idx == nbr_var) return; else date_idx=idx;
  if(var[date_idx]->type != NC_INT) return;
  
  /* Find scalar nbdate variable (NC_INT: base date date as 6 digit integer (YYMMDD)) */
  rcd=nco_inq_varid_flg(nc_id,"nbdate",&nbdate_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING NCAR CCSM convention file output variable list contains \"date\" but not \"nbdate\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  } /* endif */
  (void)nco_get_var1(nc_id,nbdate_id,0L,&nbdate,NC_INT);
  
  /* Find time variable (NC_DOUBLE: current day) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"time")) break;
  } /* end loop over idx */
  if(idx == nbr_var){
    (void)fprintf(stderr,"%s: WARNING NCAR CCSM convention file output variable list contains \"date\" but not \"time\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  }else{
    time_idx=idx;
  } /* endif */
  
  /* Assign current day to averaged day number */
  day=(long)(var[time_idx]->val.dp[0]);
  
  /* Recompute date variable based on new (averaged) day number */
#ifdef USE_FORTRAN_ARITHMETIC
  date=FORTRAN_newdate(&nbdate,&day);
#else /* !USE_FORTRAN_ARITHMETIC */
  date=nco_newdate(nbdate,day);
#endif /* !USE_FORTRAN_ARITHMETIC */
  if(var[date_idx]->val.lp != NULL) return; else var[date_idx]->val.lp[0]=date;
  
  return; /* 20050109: fxm added return to void function to squelch erroneous gcc-3.4.2 warning */ 
} /* end nco_ncar_csm_date */

