/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_csm.c,v 1.1 2002-05-02 06:10:30 zender Exp $ */

/* Purpose: CSM conventions */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_cnv_arm.h" /* CSM conventions */

bool /* O [flg] File obeys CSM conventions */
ncar_csm_inq /* O [fnc] Check if file obeys CSM conventions */
(const int nc_id); /* I [id] netCDF file ID */
{
  /* Purpose: Check if file adheres to NCAR CSM history tape format */

  bool NCAR_CSM=False;

  char *att_val;

  char cnv_sng[]="Conventions"; /* Unidata standard string */
  
  int rcd; /* [rcd] Return code */
  
  long att_sz;

  nc_type att_typ;

  /* Look for signature of an NCAR CSM format file */
  rcd=nco_inq_att_flg(nc_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);

  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NULL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1);
    (void)nco_get_att(nc_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    if(strstr(att_val,"NCAR-CSM") != NULL) NCAR_CSM=True;
    if(NCAR_CSM && dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: CONVENTION File convention is %s\n",prg_nm_get(),att_val);
    att_val=nco_free(att_val);
  } /* endif */

  return NCAR_CSM;
  
} /* end ncar_csm_inq */

void
ncar_csm_date /* [fnc] Fix date variable in averaged CSM files */
(const int nc_id, /* I [id] netCDF file ID */
 const var_sct **var, /* I [sct] List of pointers to variable structures */
 const int nbr_var) /* I [nbr] Number of structures in variable structure list */
{
  /* Purpose: Fix date variable in averaged CSM files */
  char wrn_sng[1000];

  int day;
  int date_idx;
  int idx;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int time_idx;
  
  int nbdate_id;
  
  nco_long nbdate;
  nco_long date;
  
  (void)sprintf(wrn_sng,"Most, but not all, CSM files which are in CCM format contain the fields \"nbdate\", \"time\", and \"date\". When the \"date\" field is present but either \"nbdate\" or \"time\" is missing, then %s is unable to construct a meaningful average \"date\" to store in the output file. Therefore the \"date\" variable in your output file may be meaningless.\n",prg_nm_get());

  /* Find the date variable (NC_INT: current date as 6 digit integer (YYMMDD)) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"date")) break;
  } /* end loop over idx */
  if(idx == nbr_var) return; else date_idx=idx;
  if(var[date_idx]->type != NC_INT) return;
  
  /* Find scalar nbdate variable (NC_INT: base date date as 6 digit integer (YYMMDD)) */
  rcd=nco_inq_varid_flg(nc_id,"nbdate",&nbdate_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING NCAR CSM convention file output variable list contains \"date\" but not \"nbdate\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  } /* endif */
  (void)nco_get_var1(nc_id,nbdate_id,0L,&nbdate,NC_INT);
  
  /* Find time variable (NC_DOUBLE: current day) */
  for(idx=0;idx<nbr_var;idx++){
    if(!strcmp(var[idx]->nm,"time")) break;
  } /* end loop over idx */
  if(idx == nbr_var){
    (void)fprintf(stderr,"%s: WARNING NCAR CSM convention file output variable list contains \"date\" but not \"time\"\n",prg_nm_get());
    (void)fprintf(stderr,"%s: %s",prg_nm_get(),wrn_sng);
    return;
  }else{
    time_idx=idx;
  } /* endif */
  
  /* Assign the current day to the averaged day number */
  day=(int)(var[time_idx]->val.dp[0]);
  
  /* Recompute the date variable based on the new (averaged) day number */
#ifdef USE_FORTRAN_ARITHMETIC
  date=FORTRAN_newdate(&nbdate,&day);
#else /* !USE_FORTRAN_ARITHMETIC */
  date=newdate(nbdate,day);
#endif /* !USE_FORTRAN_ARITHMETIC */
  if(var[date_idx]->val.lp != NULL) return; else var[date_idx]->val.lp[0]=date;
  
} /* end ncar_csm_date */

