/* $Header$ */

/* Purpose: ARM conventions, e.g., http://www.arm.gov/data/time.stm */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_cnv_arm.h" /* ARM conventions */

nco_bool /* O [flg] File obeys ARM conventions */
nco_cnv_arm_inq /* O [fnc] Check if file obeys ARM conventions */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* Purpose: Check whether file adheres to ARM time format defined
     http://www.arm.gov/data/time.stm */
  nco_bool CNV_ARM;

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
    CNV_ARM=False;
  }else{
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: CONVENTION File convention is DOE ARM\n",nco_prg_nm_get()); 
    CNV_ARM=True;
  } /* end else */

  return CNV_ARM;
} /* end nco_cnv_arm_inq */

double /* O [s] base_time + current time_offset */ 
nco_cnv_arm_time_mk /* [fnc] Return time corresponding to current time offset */
(const int nc_id, /* I [id] netCDF file ID */
 const double time_offset) /* I [s] Current time offset */
{
  /* NB: nco_cnv_arm_time_mk() with same nc_id contains OpenMP critical region */
  /* Purpose: Return time corresponding to current time offset */
  double arm_time;

  int base_time_id;
  int rcd=NC_NOERR; /* [rcd] Return code */

  nco_int base_time;

  /* Find base_time variable (NC_INT: base UNIX time of file) */
  rcd=nco_inq_varid_flg(nc_id,"base_time",&base_time_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have variable \"base_time\", exiting nco_cnv_arm_time_mk()...\n",nco_prg_nm_get());
    return -1;
  } /* end if */

  { /* begin potential OpenMP critical */
    /* Block is critical/thread-safe for identical/distinct in_id's */
    (void)nco_get_var1(nc_id,base_time_id,0L,&base_time,NC_INT);
  } /* end potential OpenMP critical */
  arm_time=base_time+time_offset;

  return arm_time;
} /* end nco_cnv_arm_time_mk() */

void
nco_cnv_arm_time_install /* [fnc] Add time variable to concatenated ARM files */
(const int nc_id, /* I [id] netCDF file ID */
 const nco_int base_time_srt, /* I [s] base_time of first input file */
 const int dfl_lvl) /* I [enm] Deflate level [0..9] */
{
  /* NB: nco_cnv_arm_time_install() contains OpenMP critical region */
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
    (void)fprintf(stderr,"%s: WARNING ARM file does not have variable \"time_offset\", exiting nco_cnv_arm_time_install()...\n",nco_prg_nm_get());
    return;
  } /* endif */

  /* See if time variable already exists */
  rcd=nco_inq_varid_flg(nc_id,time_sng,&time_id);
  if(rcd == NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file already has variable \"time\"\n",nco_prg_nm_get());
    return;
  } /* endif */

  /* See if time dimension exists */
  rcd=nco_inq_dimid_flg(nc_id,time_sng,&time_dmn_id);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: WARNING ARM file does not have dimension \"time\"\n",nco_prg_nm_get());
    return;
  } /* endif */
  /* Get dimension size */
  (void)nco_inq_dimlen(nc_id,time_dmn_id,&cnt);

  /* If the time coordinate does not already exist, create it */
  time_offset=(double *)nco_malloc(cnt*nco_typ_lng(NC_DOUBLE));

  { /* begin potential OpenMP critical */
    /* Block is critical/thread-safe for identical/distinct in_id's */
    (void)nco_get_vara(nc_id,time_offset_id,&srt,&cnt,(void *)time_offset,NC_DOUBLE);
  } /* end potential OpenMP critical */

  for(idx=0L;idx<cnt;idx++) time_offset[idx]+=base_time_srt;

  /* File must be in define mode */
  (void)nco_redef(nc_id);
  (void)nco_def_var(nc_id,time_sng,NC_DOUBLE,1,&time_dmn_id,&time_id);

  /* Set HDF Lempel-Ziv compression level, if requested */
  if(dfl_lvl >= 0) (void)nco_def_var_deflate(nc_id,time_id,(int)NC_SHUFFLE,(int)True,dfl_lvl);

  /* Add attributes for time variable */
  (void)nco_put_att(nc_id,time_id,units_sng,NC_CHAR,(long int)(strlen(att_units)+1UL),(const void *)att_units);
  (void)nco_put_att(nc_id,time_id,long_name_sng,NC_CHAR,(long int)(strlen(att_long_name)+1UL),(const void *)att_long_name);

  /* Catenate time-stamped reminder onto "history" global attribute */
  (void)nco_hst_att_cat(nc_id,"ncrcat added variable time=base_time+time_offset");

  /* Take file out of define mode */
  (void)nco_enddef(nc_id);

  /* Block is always critical */
  { /* begin OpenMP critical */
    /* Write time variable */
    (void)nco_put_vara(nc_id,time_id,&srt,&cnt,(void *)time_offset,NC_DOUBLE);
  } /* end OpenMP critical */

  /* Free time_offset buffer */
  time_offset=(double *)nco_free(time_offset);

  return; /* 20050109: fxm added return to void function to squelch erroneous gcc-3.4.2 warning */ 
} /* end nco_cnv_arm_time_install */

nco_int /* O [s] Value of base_time variable */
nco_cnv_arm_base_time_get /* [fnc] Get base_time variable from ARM file */
(const int nc_id) /* I [id] netCDF file ID */
{
  /* NB: nco_cnv_arm_base_time_get() with same nc_id contains OpenMP critical region */
  /* Purpose: Get base_time variable from ARM file */
  int base_time_id;

  nco_int base_time;

  (void)nco_inq_varid(nc_id,"base_time",&base_time_id);

  /* Block is critical/thread-safe for identical/distinct in_id's */
  { /* begin potential OpenMP critical */
    (void)nco_get_var1(nc_id,base_time_id,0L,&base_time,NC_INT);
  } /* end potential OpenMP critical */

  return base_time;
} /* end nco_cnv_arm_base_time_get */

