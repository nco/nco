/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnv_arm.c,v 1.8 2005-01-10 02:22:47 zender Exp $ */

/* Purpose: ARM conventions */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_cnv_arm.h" /* ARM conventions */

bool /* O [flg] File obeys ARM conventions */
arm_inq /* O [fnc] Check if file obeys ARM conventions */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* Purpose: Check whether file adheres to ARM time format */
  bool ARM_FORMAT;

  const char time_sng[]="time"; /* CEWI */
  const char base_time_sng[]="base_time"; /* CEWI */
  const char time_offset_sng[]="time_offset"; /* CEWI */

  int time_dmn_id;
  int base_time_id;
  int time_offset_id;
  int rcd=NC_NOERR; /* [rcd] Return code */
  
  /* Look for ARM file signature */
  rcd+=nco_inq_dimid_flg(nc_id,time_sng,&time_dmn_id);
  rcd+=nco_inq_varid_flg(nc_id,base_time_sng,&base_time_id);
  rcd+=nco_inq_varid_flg(nc_id,time_offset_sng,&time_offset_id);
  
  /* All three IDs must be valid to handle ARM format */
  if(rcd != NC_NOERR){
    ARM_FORMAT=False;
  }else{
    if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: CONVENTION File convention is DOE ARM\n",prg_nm_get()); 
    ARM_FORMAT=True;
  } /* end else */

  return ARM_FORMAT;
} /* end arm_inq */

double /* O [s] base_time + current time_offset */ 
arm_time_mk /* [fnc] Return time corresponding to current time offset */
(const int nc_id, /* I [id] netCDF file ID */
 const double time_offset) /* I [s] Current time offset */
{
  /* Purpose: Return time corresponding to current time offset */
  double arm_time;

  int base_time_id;
  int rcd=NC_NOERR; /* [rcd] Return code */

  nco_long base_time;

  /* Find base_time variable (NC_INT: base UNIX time of file) */
  rcd=nco_inq_varid_flg(nc_id,"base_time",&base_time_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have variable \"base_time\", exiting arm_time_mk()...\n",prg_nm_get());
    return -1;
  } /* end if */
  (void)nco_get_var1(nc_id,base_time_id,0L,&base_time,NC_INT);
  arm_time=base_time+time_offset;

  return arm_time;
} /* end arm_time_mk() */

void
nco_arm_time_install /* [fnc] Add time variable to concatenated ARM files */
(const int nc_id, /* I [id] netCDF file ID */
 const nco_long base_time_srt) /* I [s] base_time of first input file */
{
  /* Purpose: Add time variable to concatenated ARM files */

  const char att_long_name[]="UNIX time";
  const char att_units[]="seconds since 1970/01/01 00:00:00.00";
  const char long_name_sng[]="long_name"; /* CEWI */
  const char time_sng[]="time"; /* CEWI */
  const char units_sng[]="units"; /* CEWI */

  double *time_offset;

  int rcd=NC_NOERR; /* [rcd] Return code */
  int time_id;
  int time_dmn_id;
  int time_offset_id;
  
  long idx;
  long srt=0L;
  long cnt;

  /* Synchronize output file */
  (void)nco_sync(nc_id);

  /* Find time_offset variable */
  rcd=nco_inq_varid_flg(nc_id,"time_offset",&time_offset_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have variable \"time_offset\", exiting nco_arm_time_install()...\n",prg_nm_get());
    return;
  } /* endif */

  /* See if time variable already exists */
  rcd=nco_inq_varid_flg(nc_id,time_sng,&time_id);
  if(rcd == NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file already has variable \"time\"\n",prg_nm_get());
    return;
  } /* endif */

  /* See if time dimension exists */
  rcd=nco_inq_dimid_flg(nc_id,time_sng,&time_dmn_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have dimension \"time\"\n",prg_nm_get());
    return;
  } /* endif */
  /* Get dimension size */
  (void)nco_inq_dimlen(nc_id,time_dmn_id,&cnt);

  /* If the time coordinate does not already exist, create it */
  time_offset=(double *)nco_malloc(cnt*nco_typ_lng(NC_DOUBLE));

  (void)nco_get_vara(nc_id,time_offset_id,&srt,&cnt,(void *)time_offset,NC_DOUBLE);
  for(idx=0L;idx<cnt;idx++) time_offset[idx]+=base_time_srt;

  /* File must be in define mode */
  (void)nco_redef(nc_id);
  (void)nco_def_var(nc_id,time_sng,NC_DOUBLE,1,&time_dmn_id,&time_id);

  /* Add attributes for time variable */
  (void)nco_put_att(nc_id,time_id,units_sng,NC_CHAR,(long)(strlen(att_units)+1UL),(const void *)att_units);
  (void)nco_put_att(nc_id,time_id,long_name_sng,NC_CHAR,(long)(strlen(att_long_name)+1UL),(const void *)att_long_name);

  /* Catenate time-stamped reminder onto "history" global attribute */
  (void)nco_hst_att_cat(nc_id,"ncrcat added variable time=base_time+time_offset");

  /* Take file out of define mode */
  (void)nco_enddef(nc_id);

  /* Write time variable */
  (void)nco_put_vara(nc_id,time_id,&srt,&cnt,(void *)time_offset,NC_DOUBLE);

  /* Free time_offset buffer */
  time_offset=(double *)nco_free(time_offset);

  return; /* 20050109: fxm added return to void function to squelch erroneous gcc-3.4.2 warning */ 
} /* end nco_arm_time_install */

nco_long /* O [s] Value of base_time variable */
arm_base_time_get /* [fnc] Get base_time variable from ARM file */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* Purpose: Get base_time variable from ARM file */
  int base_time_id;

  nco_long base_time;

  (void)nco_inq_varid(nc_id,"base_time",&base_time_id);
  (void)nco_get_var1(nc_id,base_time_id,0L,&base_time,NC_INT);

  return base_time;
} /* end arm_base_time_get */

