/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.c,v 1.24 2006-01-31 06:42:11 zender Exp $ */

/* Purpose: CCM/CCSM/CF conventions */

/* Copyright (C) 1995--2006 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_cnv_csm.h" /* CCM/CCSM/CF conventions */

bool /* O [flg] File obeys CCM/CCSM/CF conventions */
nco_cnv_ccm_ccsm_cf_inq /* O [fnc] Check if file obeys CCM/CCSM/CF conventions */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* Purpose: Check if file adheres to CCM/CCSM/CF history tape format */

  bool CNV_CCM_CCSM_CF=False;

  char *att_val;

  char cnv_sng[]="Conventions"; /* Unidata standard string */
  
  int rcd; /* [rcd] Return code */
  
  long att_sz;

  nc_type att_typ;

  /* Look for signature of an CCM/CCSM/CF format file */
  rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);

  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1);
    (void)nco_get_att(nc_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    /* CCM3, CCSM1 conventions */
    if(strstr(att_val,"NCAR-CSM") != NULL) CNV_CCM_CCSM_CF=True; /* Backwards compatibility */
    /* Climate-Forecast conventions */
    if(strstr(att_val,"CF-1.0") != NULL) CNV_CCM_CCSM_CF=True; /* NB: Not fully implemented TODO nco145 */
    if(CNV_CCM_CCSM_CF && dbg_lvl_get() > 0){
      (void)fprintf(stderr,"%s: CONVENTION File Convention attribute is \"%s\".",prg_nm_get(),att_val);
      if(dbg_lvl_get() > 1) (void)fprintf(stderr," NCO has a unified (but incomplete) treatment of many related (official and unoffical) conventions such as CCM, CCSM, and CF. As part of adhering to this convention, NCO implements variable-specific exceptions in certain operators, e.g., ncbo will not subtract variables named \"date\" or \"gw\". For a full list of exceptions, see the manual http://nco.sf.net/nco.html#CF");
      (void)fprintf(stderr,"\n");
    } /* endif dbg */
    att_val=(char *)nco_free(att_val);
  } /* endif */

  return CNV_CCM_CCSM_CF;
  
} /* end nco_cnv_ccm_ccsm_cf_inq */

void
nco_cnv_ccm_ccsm_cf_date /* [fnc] Fix date variable in averaged CCM/CCSM/CF files */
(const int nc_id, /* I [id] netCDF file ID */
 X_CST_PTR_CST_PTR_Y(var_sct,var), /* I/O [sct] Variables in output file */
 const int nbr_var) /* I [nbr] Number of variables in list */
{
  /* Purpose: Fix date variable in averaged CCM/CCSM/CF files */
  char wrn_sng[1000];

  int date_idx;
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int time_idx;
  
  long day;

  int nbdate_id;
  
  nco_int nbdate;
  nco_int date;
  
  (void)sprintf(wrn_sng,"Most, but not all, CCM/CCSM/CF files which are in CCM format contain the fields \"nbdate\", \"time\", and \"date\". When the \"date\" field is present but either \"nbdate\" or \"time\" is missing, then %s is unable to construct a meaningful average \"date\" to store in the output file. Therefore the \"date\" variable in your output file may be meaningless.\n",prg_nm_get());

  /* Find date variable (NC_INT: current date as 6 digit integer (YYMMDD)) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"date")) break;
  } /* end loop over idx */
  if(idx == nbr_var) return; else date_idx=idx;
  if(var[date_idx]->type != NC_INT) return;
  
  /* Find scalar nbdate variable (NC_INT: base date date as 6 digit integer (YYMMDD)) */
  rcd=nco_inq_varid_flg(nc_id,"nbdate",&nbdate_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING CCM/CCSM/CF convention file output variable list contains \"date\" but not \"nbdate\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  } /* endif */

  { /* begin potential OpenMP critical */
    /* Block is critical/thread-safe for identical/distinct in_id's */
    (void)nco_get_var1(nc_id,nbdate_id,0L,&nbdate,NC_INT);
  } /* end potential OpenMP critical */
  
  /* Find time variable (NC_DOUBLE: current day) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"time")) break;
  } /* end loop over idx */
  if(idx == nbr_var){
    (void)fprintf(stderr,"%s: WARNING CCM/CCSM/CF convention file output variable list contains \"date\" but not \"time\"\n",prg_nm_get());
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
} /* end nco_cnv_ccm_ccsm_cf_date */

