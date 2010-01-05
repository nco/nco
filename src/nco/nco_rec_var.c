/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rec_var.c,v 1.18 2010-01-05 20:02:18 zender Exp $ */

/* Purpose: Record variable utilities */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_rec_var.h" /* Record variable utilities */

void
rec_var_dbg /* [fnc] Aid in debugging problems with record dimension */
(const int nc_id, /* I [id] netCDF file ID */
 const char * const dbg_sng) /* I [sng] Debugging message to print */
{
  /* Purpose: Aid in debugging problems with record dimension */
  /* Usage: if(dbg_lvl == 73) rec_var_dbg(out_id,"After ncvarput()"); */
  int nbr_dmn_fl;
  int nbr_var_fl;
  int rec_dmn_id=NCO_REC_DMN_UNDEFINED;
  long dmn_sz;

  (void)fprintf(stderr,"%s: DBG %s\n",prg_nm_get(),dbg_sng);
  (void)nco_inq(nc_id,&nbr_dmn_fl,&nbr_var_fl,(int *)NULL,&rec_dmn_id);
  if(rec_dmn_id == NCO_REC_DMN_UNDEFINED){
    (void)fprintf(stderr,"%s: DBG %d dimensions, %d variables, no record dimension\n",prg_nm_get(),nbr_dmn_fl,nbr_var_fl);
  }else{
    (void)nco_inq_dimlen(nc_id,rec_dmn_id,&dmn_sz);
    (void)fprintf(stderr,"%s: DBG %d dimensions, %d variables, record dimension size is %li\n",prg_nm_get(),nbr_dmn_fl,nbr_var_fl,dmn_sz);
  } /* end else */
  (void)fflush(stderr);
} /* end rec_var_dbg() */

void
rec_crd_chk /* Check for monotonicity of coordinate values */
(const var_sct * const var, /* I [sct] Coordinate to check for monotonicity */
 const char * const fl_in, /* I [sng] Input filename */
 const char * const fl_out, /* I [sng] Output filename */
 const long idx_rec, /* I [idx] Index of record coordinate in input file */
 const long idx_rec_out) /* I [idx] Index of record coordinate in output file */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Check for monotonicity of coordinate values */

  enum monotonic_direction{
    decreasing, /* 0 */
    increasing}; /* 1 */

  static double rec_crd_val_lst;
  static double rec_crd_val_crr;

  static int monotonic_direction;

  /* Use implicit type conversion */
  switch(var->type){
  case NC_FLOAT: rec_crd_val_crr=var->val.fp[0]; break; 
  case NC_DOUBLE: rec_crd_val_crr=var->val.dp[0]; break; 
  case NC_INT: rec_crd_val_crr=var->val.lp[0]; break;
  case NC_SHORT: rec_crd_val_crr=var->val.sp[0]; break;
  case NC_USHORT: rec_crd_val_crr=var->val.usp[0]; break;
  case NC_UINT: rec_crd_val_crr=var->val.uip[0]; break;
  case NC_INT64: rec_crd_val_crr=var->val.i64p[0]; break;
  case NC_UINT64: rec_crd_val_crr=var->val.ui64p[0]; break;
  case NC_BYTE: rec_crd_val_crr=var->val.bp[0]; break;
  case NC_UBYTE: rec_crd_val_crr=var->val.ubp[0]; break;
  case NC_CHAR: rec_crd_val_crr=var->val.cp[0]; break;
  case NC_STRING: break; /* Do nothing */
    default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  if(idx_rec_out > 1){
    if(((rec_crd_val_crr > rec_crd_val_lst) && monotonic_direction == decreasing) ||
       ((rec_crd_val_crr < rec_crd_val_lst) && monotonic_direction == increasing)){
      if(idx_rec-1 == -1){
	/* Inter-file non-monotonicity */
	if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: INFO/WARNING Inter-file non-monotonicity. Record coordinate \"%s\" does not monotonically %s between last specified record of previous input file (whose name is not cached locally and thus currently unavailable for printing) and first specified record (i.e., record index = %ld) of current input file (%s). This message is often informational only and may usually be safely ignored. It is quite common when joining files with \"wrapped\" record coordinates, e.g., joining a January file to a December file when the time coordinate is enumerated as day of year. It is also common when joining files which employ a \"time=base_time+time_offset\" convention. Sometimes, however, this message is a warning which signals that the user has joined files together in a different order than intended and that corrective action should be taken to re-order the input files. Output file %s will contain these non-monotonic record coordinate values (%f, %f) at record indices %ld, %ld.\n",prg_nm_get(),var->nm,(monotonic_direction == decreasing ? "decrease" : "increase"),idx_rec,fl_in,fl_out,rec_crd_val_lst,rec_crd_val_crr,idx_rec_out-1,idx_rec_out);
      }else{
	/* Intra-file non-monotonicity */
	(void)fprintf(stderr,"%s: WARNING Intra-file non-monotonicity. Record coordinate \"%s\" does not monotonically %s between (input file %s record indices: %ld, %ld) (output file %s record indices %ld, %ld) record coordinate values %f, %f\n",prg_nm_get(),var->nm,(monotonic_direction == decreasing ? "decrease" : "increase"),fl_in,idx_rec-1,idx_rec,fl_out,idx_rec_out-1,idx_rec_out,rec_crd_val_lst,rec_crd_val_crr);
      } /* end if Intra-file non-monotonicity */
    } /* end if not monotonic */
  }else if(idx_rec_out == 1){
    if(rec_crd_val_crr > rec_crd_val_lst) monotonic_direction=increasing; else monotonic_direction=decreasing;
  } /* end if */
    
  rec_crd_val_lst=rec_crd_val_crr;

} /* end rec_crd_chk() */

