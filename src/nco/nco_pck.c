/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_pck.c,v 1.31 2004-09-03 23:59:09 zender Exp $ */

/* Purpose: NCO utilities for packing and unpacking variables */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_pck.h" /* Packing and unpacking variables */

/* Notes on packing/unpacking:
   Routines in this file must be used in correct order:
   nco_pck_dsk_inq(): called first, e.g., in nco_var_fll(), before var_prc copied to var_prc_out
   nco_var_upk(): called in data retrieval routine, e.g., in nco_var_get()
   nco_var_pck(): called just before writing output file, e.g., in main()
   Bookkeeping hassle is keeping flags in var_prc synchronized with flags in var_prc_out

   From netCDF User's Guide:
   scale_factor: If present for a variable, the data are to be multiplied by this factor after the data are read by the application that accesses the data
   add_offset: If present for a variable, this number is added to the data after the data are read by the application. If both scale_factor and add_offset attributes are present, the data are first scaled before the offset is added. 
   When scale_factor and add_offset are used for packing, the associated variable (containing the packed data) is typically of type byte or short, whereas the unpacked values are intended to be of type float or double. Attribute's scale_factor and add_offset should both be of type intended for the unpacked data, e.g., float or double. */

void 
nco_dfl_case_pck_typ_err(void) /* [fnc] Print error and exit for illegal switch(pck_typ) case */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(pck_typ) statement receives an illegal default case

     Placing this in its own routine also has the virtue of saving many lines 
     of code since this function is used in many many switch() statements. */
  const char fnc_nm[]="nco_dfl_case_pck_typ_err()";
  (void)fprintf(stdout,"%s: ERROR switch(pck_typ) statement fell through to default case, which is unsafe. This catch-all error handler ensures all switch(pck_typ) statements are fully enumerated. Exiting...\n",fnc_nm);
  nco_err_exit(0,fnc_nm);
} /* end nco_dfl_case_pck_typ_err() */

bool /* O [flg] NCO will attempt to pack variable */
nco_is_packable /* [fnc] Will NCO attempt to pack variable? */
(const nc_type nc_typ_in) /* I [enm] Type of input variable */
{
  /* Purpose: Determine whether NCO should attempt to pack input variable
     Packing certain variable types is not recommended, e.g., packing NC_CHAR
     and NC_BYTE makes no sense, because precision would needlessly be lost.
     Whether to attempt to pack NC_SHORT is arguable and may change in future
     This routine decides whether NCO will attempt to pack a given type */
  switch(nc_typ_in){ 
  case NC_FLOAT: 
  case NC_DOUBLE: 
  case NC_INT: 
    return True;
    break;
  case NC_SHORT: 
  case NC_CHAR: 
  case NC_BYTE: 
    return False;
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */ 

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return False;
} /* end nco_is_packable() */

int /* O [enm] Packing type */
nco_pck_typ_get /* [fnc] Convert user-specified packing type to key */
(const char *nco_pck_sng) /* [sng] User-specified packing type */
{
  /* Purpose: Process '-P' command line argument
     Convert user-specified string to packing operation type 
     Return nco_pck_nil by default */
  /* fxm: add the rest of types */
  if(!strcmp(nco_pck_sng,"all_xst")) return nco_pck_all_xst_att;
  if(!strcmp(nco_pck_sng,"nco_pck_all_xst_att")) return nco_pck_all_xst_att;
  if(!strcmp(nco_pck_sng,"all_new")) return nco_pck_all_new_att;
  if(!strcmp(nco_pck_sng,"nco_pck_all_new_att")) return nco_pck_all_new_att;
  if(!strcmp(nco_pck_sng,"xst_xst")) return nco_pck_xst_xst_att;
  if(!strcmp(nco_pck_sng,"nco_pck_xst_xst_att")) return nco_pck_xst_xst_att;
  if(!strcmp(nco_pck_sng,"xst_new")) return nco_pck_xst_new_att;
  if(!strcmp(nco_pck_sng,"nco_pck_xst_new_att")) return nco_pck_xst_new_att;
  if(!strcmp(nco_pck_sng,"upk")) return nco_pck_upk;
  if(!strcmp(nco_pck_sng,"nco_pck_upk")) return nco_pck_upk;

  (void)fprintf(stderr,"%s: ERROR nco_pck_typ_get() reports unknown user-specified packing type %s\n",prg_nm_get(),nco_pck_sng);
  nco_exit(EXIT_FAILURE);
  return nco_pck_nil; /* Statement should not be reached */
} /* end nco_pck_typ_get() */

nc_type /* O [enm] Type to pack variable to */
nco_typ_pck_get /* [fnc] Determine best type to pack input variable to */
(const nc_type nc_typ_in) /* I [enm] Type of input variable */
{
  /* Purpose: Determine best type to pack input variable to */
  /* fxm: devise better system to allow user to specify output type for packed variable */
  nc_type nc_typ_pck_out=NC_NAT; /* [enm] Type to pack to */

  switch(nc_typ_in){ 
  case NC_FLOAT: 
  case NC_DOUBLE: 
  case NC_INT: 
    nc_typ_pck_out=NC_SHORT;
    break;
  case NC_SHORT: 
    nc_typ_pck_out=NC_SHORT;
    break;
  case NC_CHAR: 
    nc_typ_pck_out=NC_CHAR;
    break;
  case NC_BYTE: 
    nc_typ_pck_out=NC_BYTE;
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */ 

  return nc_typ_pck_out;
} /* end nco_typ_pck_get() */

bool /* O [flg] Variable is packed on disk */
nco_pck_dsk_inq /* [fnc] Check whether variable is packed on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 var_sct * const var) /* I/O [sct] Variable */
{
  /* Purpose: Check whether variable is packed on disk and set variable members 
     pck_dsk, has_scl_fct, has_add_fst, and typ_upk accordingly
     nco_pck_dsk_inq() should be called early in application, e.g., in nco_var_fll() 
     Call nco_pck_dsk_inq() before copying input list to output list 
     Multi-file operators which handle packing must call this routine prior
     to each read of a variable, in case that variable has been unpacked. */
  /* ncea -O -D 3 -v pck ~/nco/data/in.nc ~/nco/data/foo.nc */

  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  
  int rcd; /* [rcd] Return success code */

  long add_fst_lng; /* [idx] Number of elements in add_offset attribute */
  long scl_fct_lng; /* [idx] Number of elements in scale_factor attribute */

  nc_type add_fst_typ; /* [idx] Type of add_offset attribute */
  nc_type scl_fct_typ; /* [idx] Type of scale_factor attribute */

  /* Set some defaults in variable structure for safety in case of early return
     Flags for variables without valid scaling information should appear 
     same as flags for variables with _no_ scaling information
     Set has_scl_fct, has_add_fst in var_dfl_set()
     typ_upk:
     1. is required by ncra nco_cnv_mss_val_typ() 
     2. depends on var->type and so should not be set in var_dfl_set()
     3. is therefore set to default here */
  var->typ_upk=var->type; /* [enm] Type of variable when unpacked (expanded) (in memory) */

  /* Vet scale_factor */
  rcd=nco_inq_att_flg(nc_id,var->id,scl_fct_sng,&scl_fct_typ,&scl_fct_lng);
  if(rcd != NC_ENOTATT){
    if(scl_fct_typ != NC_FLOAT && scl_fct_typ != NC_DOUBLE){
      (void)fprintf(stderr,"%s: WARNING nco_pck_dsk_inq() reports scale_factor for %s is not NC_FLOAT or NC_DOUBLE. Will not attempt to unpack using scale_factor.\n",prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(scl_fct_lng != 1){
      (void)fprintf(stderr,"%s: WARNING nco_pck_dsk_inq() reports %s has scale_factor of length %li. Will not attempt to unpack using scale_factor\n",prg_nm_get(),var->nm,scl_fct_lng); 
      return False;
    } /* endif */
    var->has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
    var->typ_upk=scl_fct_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
  } /* endif */

  /* Vet add_offset */
  rcd=nco_inq_att_flg(nc_id,var->id,add_fst_sng,&add_fst_typ,&add_fst_lng);
  if(rcd != NC_ENOTATT){
    if(add_fst_typ != NC_FLOAT && add_fst_typ != NC_DOUBLE){
      (void)fprintf(stderr,"%s: WARNING nco_pck_dsk_inq() reports add_offset for %s is not NC_FLOAT or NC_DOUBLE. Will not attempt to unpack.\n",prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(add_fst_lng != 1){
      (void)fprintf(stderr,"%s: WARNING nco_pck_dsk_inq() reports %s has add_offset of length %li. Will not attempt to unpack.\n",prg_nm_get(),var->nm,add_fst_lng); 
      return False;
    } /* endif */
    var->has_add_fst=True; /* [flg] Valid add_offset attribute exists */
    var->typ_upk=add_fst_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
  } /* endif */

  if(var->has_scl_fct && var->has_add_fst){
    if(scl_fct_typ != add_fst_typ){
      (void)fprintf(stderr,"%s: WARNING nco_pck_dsk_inq() reports type of scale_factor does not equal type of add_offset. Will not attempt to unpack.\n",prg_nm_get());
      return False;
    } /* endif */
  } /* endif */

  if(var->has_scl_fct || var->has_add_fst){
    /* Variable is considered packed iff either or both valid scale_factor or add_offset exist */
    var->pck_dsk=True; /* [flg] Variable is packed on disk */
    /* If variable is packed on disk and is in memory then variable is packed in memory */
    var->pck_ram=True; /* [flg] Variable is packed in memory */
    var->typ_upk=scl_fct_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
    if(nco_is_rth_opr(prg_get()) && dbg_lvl_get() > 2){
      (void)fprintf(stderr,"%s: PACKING Variable %s is type %s packed into type %s\n",prg_nm_get(),var->nm,nco_typ_sng(var->typ_upk),nco_typ_sng(var->typ_dsk));
      (void)fprintf(stderr,"%s: DEBUG Packed variables processed by all arithmetic operators are unpacked automatically, and then stored unpacked in the output file. If you wish to repack them in the output file, use, e.g., ncap -O -s \"foo=pack(foo);\" out.nc out.nc. If you wish to pack all the variables in a file, use ncpdq fxm.\n",prg_nm_get());
    } /* endif print packing information */
  }else{
    /* Variable is not packed since neither scale factor nor add_offset exist
       Insert hooks which depend on variable not being packed here
       Currently this is no-op */
    ;
  } /* end else */

  return var->pck_dsk; /* [flg] Variable is packed on disk (valid scale_factor, add_offset, or both attributes exist) */
  
} /* end nco_pck_dsk_inq() */

var_sct * /* O [sct] Packed variable */
nco_put_var_pck /* [fnc] Pack variable in memory and write packing attributes to disk */
(const int out_id, /* I [id] netCDF output file ID */
 var_sct *var, /* I/O [sct] Variable to be packed */
 const int nco_pck_typ) /* [enm] Packing operation type */
{
  /* Purpose: Pack variable in memory and write packing attributes to disk
     NB: Routine is not complete, debugged, or currently used 
     ncpdq breaks up writing packed variables into multiple tasks, i.e.,
     ncpdq separates variable value writes from packing attribute value writes.
     This routine is intended to write a packed variable in one routine */
  
  bool USE_EXISTING_PCK=False; /* I [flg] Use existing packing scale_factor and add_offset */

  switch(nco_pck_typ){
  case nco_pck_xst_xst_att:
  case nco_pck_all_xst_att:
    USE_EXISTING_PCK=True; /* I [flg] Use existing packing scale_factor and add_offset */
    break;
  case nco_pck_xst_new_att:
  case nco_pck_all_new_att:
    USE_EXISTING_PCK=False; /* I [flg] Use existing packing scale_factor and add_offset */
    break;
  case nco_pck_upk:
    break;
  case nco_pck_nil:
  default: nco_dfl_case_pck_typ_err(); break;
  } /* end switch */

  /* Pack variable */
  if(var->xrf->pck_dsk && !var->xrf->pck_ram) var=nco_var_pck(var,var->typ_pck,USE_EXISTING_PCK);

  /* Write/overwrite scale_factor and add_offset attributes */
  if(var->pck_ram){ /* [flg] Variable is packed in memory */
    if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
      (void)nco_put_att(out_id,var->id,"scale_factor",var->typ_upk,1,var->scl_fct.vp);
    } /* endif has_scl_fct */
    if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
      (void)nco_put_att(out_id,var->id,"add_offset",var->typ_upk,1,var->add_fst.vp);
    } /* endif has_add_fst */
  } /* endif pck_ram */
  
  return var;
  
} /* end nco_put_var_pck() */

var_sct * /* O [sct] Packed variable */
nco_var_pck /* [fnc] Pack variable in memory */
(var_sct *var, /* I/O [sct] Variable to be packed */
 const nc_type nc_typ_pck, /* I [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
 const bool USE_EXISTING_PCK) /* I [flg] Use existing scale_factor and add_offset, if any */
{
  /* Purpose: Pack variable 
     Routine is inverse of nco_var_upk(): nco_var_pck[nco_var_upk(var)]=var 
     Currently routine outputs same variable structure as given on input
     In other words, output structure may be neglected as all changes are made 
     to input structure.
     NB: Value buffer var->val.vp is usually free()'d here
     Variables in calling routine which point to var->val.vp will be left dangling */

  const char fnc_nm[]="nco_var_pck()"; /* [sng] Function name */
  double scl_fct_dbl=double_CEWI; /* [sct] Double precision value of scale_factor */
  double add_fst_dbl=double_CEWI; /* [sct] Double precision value of add_offset */

  /* Return if variable in memory is currently packed and should not be re-packed */
  if(var->pck_ram) return var;

  /* Routine should be called with variable already in memory */
  if(var->val.vp == NULL) (void)fprintf(stdout,"%s: ERROR %s called with empty var->val.vp\n",prg_nm_get(),fnc_nm);
  
  /* Packed type must be NC_CHAR or NC_SHORT */
  /* fxm: Allow packing NC_DOUBLE to NC_INT */
  if(nc_typ_pck != NC_CHAR && nc_typ_pck != NC_SHORT){
    (void)fprintf(stdout,"%s: ERROR %s called with invalid packed type nc_typ_pck = %s\n",prg_nm_get(),fnc_nm,nco_typ_sng(nc_typ_pck));
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Variable must be packable (e.g., NC_INT, NC_FLOAT, or NC_DOUBLE) */
  if(!nco_is_packable(var->type)){
    (void)fprintf(stdout,"%s: ERROR %s is asked to pack variable %s of type %s\n",prg_nm_get(),fnc_nm,var->nm,nco_typ_sng(var->type));
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(True){
    /* Compute packing parameters to apply to var

       Linear packing in a nutshell:
       scale_factor = (max-min)/ndrv <--> (max-min)/scale_factor = ndrv <--> scale_factor*ndrv = max-min
       add_offset = 0.5*(min+max)
       pck = (upk-add_offset)/scale_factor = (upk-0.5*(min+max))*ndrv/(max-min)
       upk = scale_factor*pck + add_offset = (max-min)*pck/ndrv + 0.5*(min+max) 

       where 

       ndrv = number of discrete representable values for given type of packed variable and
       ndrv = 256 iff var->typ_pck == NC_CHAR
       ndrv = 256*256 iff var->typ_pck == NC_SHORT
       ndrv = 256*256*256*256 = 2^32 iff var->typ_pck == NC_INT */

    const double max_mns_min_dbl_wrn=1.0e10; /* [frc] Threshold value for warning */
    double ndrv_dbl=double_CEWI; /* [frc] Double precision value of number of discrete representable values */
    double max_mns_min_dbl; /* [frc] Maximum value minus minimum value */

    ptr_unn ptr_unn_min; /* [ptr] Pointer union to minimum value of variable */
    ptr_unn ptr_unn_max; /* [ptr] Pointer union to maximum value of variable */
    ptr_unn ptr_unn_mss_val_dbl; /* [ptr] Pointer union to missing value of variable */
    
    var_sct *min_var; /* [sct] Minimum value of variable */
    var_sct *max_var; /* [sct] Maximum value of variable */
    var_sct *max_var_dpl; /* [sct] Copy of Maximum value of variable */
    var_sct *hlf_var; /* [sct] NCO variable for value 0.5 */
    var_sct *zero_var; /* [sct] NCO variable for value 0.0 */
    var_sct *ndrv_var; /* [sct] NCO variable for number of discrete representable values */
    
    val_unn hlf_unn; /* [frc] Generic container for value 0.5 */
    val_unn zero_unn; /* [frc] Generic container for value 0.0 */
    val_unn ndrv_unn; /* [nbr] Generic container for number of discrete representable values */

    /* Initialize data */
    hlf_unn.d=0.5; /* Generic container for value 0.5 */
    zero_unn.d=0.0; /* Generic container for value 0.0 */

    /* Derive scalar values for scale_factor and add_offset */
    var->scl_fct.vp=nco_free(var->scl_fct.vp);
    var->add_fst.vp=nco_free(var->add_fst.vp);
    var->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    var->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    ptr_unn_min.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    ptr_unn_max.vp=(void *)nco_malloc(nco_typ_lng(var->type));

    /* Create double precision missing_value for use in min/max arithmetic */
    if(var->has_mss_val){
      ptr_unn_mss_val_dbl.vp=(void *)nco_malloc(nco_typ_lng((nc_type)NC_DOUBLE));
      (void)nco_val_cnf_typ(var->type,var->mss_val,(nc_type)NC_DOUBLE,ptr_unn_mss_val_dbl);
    } /* endif has_mss_val */

    /* Find minimum and maximum values in data */
    (void)nco_var_avg_reduce_max(var->type,var->sz,1L,var->has_mss_val,var->mss_val,var->val,ptr_unn_min);
    (void)nco_var_avg_reduce_min(var->type,var->sz,1L,var->has_mss_val,var->mss_val,var->val,ptr_unn_max);

    /* Convert to NC_DOUBLE before 0.5*(min+max) operation */
    min_var=scl_ptr_mk_var(ptr_unn_min,var->type);
    min_var=nco_var_cnf_typ((nc_type)NC_DOUBLE,min_var);
    max_var=scl_ptr_mk_var(ptr_unn_max,var->type);
    max_var=nco_var_cnf_typ((nc_type)NC_DOUBLE,max_var);
    /* Copy max_var for use in scale_factor computation */
    max_var_dpl=nco_var_dpl(max_var);
    hlf_var=scl_mk_var(hlf_unn,NC_DOUBLE); /* [sct] NCO variable for value one half */

    if(dbg_lvl_get() == 3) (void)fprintf(stdout,"%s: %s: min_var = %g, max_var = %g\n",prg_nm_get(),var->nm,min_var->val.dp[0],max_var->val.dp[0]);

    /* add_offset is 0.5*(min+max) */
    /* max_var->val is overridden with add_offset answers, no longer valid as max_var */
    (void)nco_var_add((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,min_var->val,max_var->val);
    (void)nco_var_mlt((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,hlf_var->val,max_var->val);
    /* Contents of max_var are actually add_offset */
    (void)nco_val_cnf_typ((nc_type)NC_DOUBLE,max_var->val,var->type,var->add_fst);

    /* ndrv is 2^{bits per packed value} where bppv = 8 for NC_CHAR and bppv = 16 for NC_SHORT
       Subtract one to leave slop for rounding errors */
    if(nc_typ_pck == NC_CHAR){
      ndrv_dbl=256.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    }else if(nc_typ_pck == NC_SHORT){
      ndrv_dbl=65536.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    }else if(nc_typ_pck == NC_INT){
      ndrv_dbl=4294967295.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    } /* end else */
    ndrv_unn.d=ndrv_dbl; /* Generic container for number of discrete representable values */
    ndrv_var=scl_mk_var(ndrv_unn,NC_DOUBLE); /* [sct] Variable structure for number of discrete representable values */

    /* scale_factor is (max-min)/ndrv
       If max-min = 0 then variable is constant value so scale_factor=0.0 and add_offset=var
       If max-min > ndrv then precision is worse than 1.0
       If max-min < ndrv then precision is better than 1.0 */
    (void)nco_var_sbt((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,min_var->val,max_var_dpl->val);
    /* max-min is currently stored in max_var_dpl */
    max_mns_min_dbl=ptr_unn_2_scl_dbl(max_var_dpl->val,max_var_dpl->type); 

    if(max_mns_min_dbl != 0.0){
      (void)nco_var_dvd((nc_type)NC_DOUBLE,1L,var->has_mss_val,ptr_unn_mss_val_dbl,ndrv_var->val,max_var_dpl->val);
      /* Contents of max_var_dpl are actually scale_factor */
      (void)nco_val_cnf_typ((nc_type)NC_DOUBLE,max_var_dpl->val,var->type,var->scl_fct);
    }else{
      /* Variable is constant, i.e., equal values everywhere */
      zero_var=scl_mk_var(zero_unn,var->type); /* [sct] NCO variable for value 0.0 */
      /* Set scale_factor to 0.0 */
      (void)memcpy(var->scl_fct.vp,zero_var->val.vp,nco_typ_lng(var->type));
      if(zero_var != NULL) zero_var=nco_var_free(zero_var);
      /* Set add_offset to first variable value 
	 Variable is constant everywhere so particular value copied is unimportant */
      (void)memcpy(var->add_fst.vp,var->val.vp,nco_typ_lng(var->type));
    } /* end else */

    if(max_mns_min_dbl > max_mns_min_dbl_wrn){
      (void)fprintf(stdout,"%s: WARNING %s reports data range of variable %s is = %g. The linear data packing technique defined by netCDF's packing convention and implemented by NCO result in significant precision loss over such a great range.\n",prg_nm_get(),fnc_nm,var->nm,max_mns_min_dbl);
      if(var->has_mss_val) (void)fprintf(stdout,"%s: HINT variable %s has missing_value = %g. Consider specifying new missing_value to reduce range of data needing packing. See http://nco.sf.net/nco.html#ncatted for examples of how to change the missing_value attribute.\n",prg_nm_get(),fnc_nm,ptr_unn_mss_val_dbl.dp[0]);
    } /* endif large data range */

    /* Free minimum and maximum values */
    ptr_unn_min.vp=nco_free(ptr_unn_min.vp);
    ptr_unn_max.vp=nco_free(ptr_unn_max.vp);

    /* Free temporary double missing_value */
    if(var->has_mss_val) ptr_unn_mss_val_dbl.vp=nco_free(ptr_unn_mss_val_dbl.vp);

    /* Free variables */
    if(min_var != NULL) min_var=nco_var_free(min_var);
    if(max_var != NULL) max_var=nco_var_free(max_var);
    if(max_var_dpl != NULL) max_var_dpl=nco_var_free(max_var_dpl);
    if(hlf_var != NULL) hlf_var=nco_var_free(hlf_var);
    if(ndrv_var != NULL) ndrv_var=nco_var_free(ndrv_var);

    /* Do not bother creating superfluous scale_factor (0.0 or 1.0) or add_offset (0.0) */
    scl_fct_dbl=ptr_unn_2_scl_dbl(var->scl_fct,var->type); 
    add_fst_dbl=ptr_unn_2_scl_dbl(var->add_fst,var->type);

    if(scl_fct_dbl != 0.0 && scl_fct_dbl != 1.0) var->has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
    if(add_fst_dbl != 0.0) var->has_add_fst=True; /* [flg] Valid add_offset attribute exists */

    /* However, must create either scale_factor or add_offset
       Otherwise, routine fails to pack field uniformly equal to zero (0.0)
       In zero corner case, create add_offset (avoids division by zero problems) */
    if(scl_fct_dbl == 0.0 && add_fst_dbl == 0.0) var->has_add_fst=True; 

  } /* endif True */

  /* Create double precision value of scale_factor for diagnostics */
  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    scl_fct_dbl=ptr_unn_2_scl_dbl(var->scl_fct,var->type); 
    if(scl_fct_dbl == 0.0) (void)fprintf(stdout,"%s: WARNING %s reports scl_fct_dbl = 0.0\n",prg_nm_get(),fnc_nm);
  } /* endif */
  
  /* Create double precision value of add_offset for diagnostics */
  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    add_fst_dbl=ptr_unn_2_scl_dbl(var->add_fst,var->type);
  } /* endif */
  
  if(dbg_lvl_get() == 3) (void)fprintf(stdout,"%s: %s reports variable %s has scl_fct_dbl = %g, add_fst_dbl = %g\n",prg_nm_get(),fnc_nm,var->nm,scl_fct_dbl,add_fst_dbl);
  
  /* Packing attributes now exist and are same type as variable in memory */

  /* Apply scale_factor and add_offset to reduce variable size
     add_offset and scale_factor are always scalars so use var_scv_* functions
     var_scv_[sub,multiply] functions avoid cost of broadcasting attributes and doing element-by-element operations 
     Using var_scv_[sub,multiply] instead of ncap_var_scv_[sub,multiply] avoids cost of deep copies
     Moreover, this keeps variable structure from changing (because ncap_var_scv_* functions all do deep copies before operations) and thus complicating memory management */
  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    /* Subtract add_offset from var */
    scv_sct add_fst_scv;
    add_fst_scv.type=NC_DOUBLE;
    add_fst_scv.val.d=add_fst_dbl;
    (void)nco_scv_cnf_typ(var->type,&add_fst_scv);
    (void)var_scv_sub(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&add_fst_scv);
  } /* endif */

  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    /* Divide var by scale_factor */
    scv_sct scl_fct_scv;
    scl_fct_scv.type=NC_DOUBLE;
    scl_fct_scv.val.d=scl_fct_dbl;
    (void)nco_scv_cnf_typ(var->type,&scl_fct_scv);
    if(scl_fct_dbl != 0.0) (void)var_scv_dvd(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
  } /* endif */

  if(!var->has_scl_fct && !var->has_add_fst){
    (void)fprintf(stderr,"%s: ERROR Reached end of %s without packing variable\n",prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  /* Tell the world we have packed the variable
     This is true if input variable satisfied nco_is_packable() criteria
     Variables that fail nco_is_packable() (e.g., type == NC_CHAR) are not packed 
     and should not have their packing attributes set */
  var->pck_ram=True; /* [flg] Variable is packed in memory */
  var->typ_pck=nc_typ_pck; /* [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
  var->typ_upk=var->type; /* [enm] Type of variable when unpacked (expanded) (in memory) */

  /* Convert variable to user-specified packed type
     This is where var->type is changed from original to packed type */
  var=nco_var_cnf_typ(nc_typ_pck,var);

  if(dbg_lvl_get() >= 3) (void)fprintf(stderr,"%s: PACKING Packed %s\n",prg_nm_get(),var->nm);

  return var;
} /* end nco_var_pck() */

var_sct * /* O [sct] Unpacked variable */
nco_var_upk /* [fnc] Unpack variable in memory */
(var_sct *var) /* I/O [sct] Variable to be unpacked */
{
  /* Threads: Routine is thread-unsafe */
  /* Purpose: Unpack variable
     Routine is inverse of nco_var_pck(): nco_var_upk[nco_var_pck(var)]=var */

  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */

  /* Return if variable in memory is not currently packed */
  if(!var->pck_ram) return var;

  /* Routine should be called with variable already in memory */
  if(var->val.vp == NULL) (void)fprintf(stdout,"%s: ERROR nco_var_upk() called with empty var->val.vp\n",prg_nm_get());

  /* Packed variables are not guaranteed to have both scale_factor and add_offset
     The scale factor is guaranteed to be of type NC_FLOAT or NC_DOUBLE and of size 1 (a scalar) */

  /* Create scalar value structures from values of scale_factor, add_offset */
  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    scv_sct scl_fct_scv;
    var->scl_fct.vp=(void *)nco_malloc(nco_typ_lng(var->typ_upk));
    (void)nco_get_att(var->nc_id,var->id,scl_fct_sng,var->scl_fct.vp,var->typ_upk);
    scl_fct_scv=ptr_unn_2_scv(var->typ_upk,var->scl_fct);
    /* Convert var to type of scale_factor for expansion */
    var=nco_var_cnf_typ(scl_fct_scv.type,var);
    /* Multiply var by scale_factor */
    (void)var_scv_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
  } /* endif has_scl_fct */

  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    scv_sct add_fst_scv;
    var->add_fst.vp=(void *)nco_malloc(nco_typ_lng(var->typ_upk));
    (void)nco_get_att(var->nc_id,var->id,add_fst_sng,var->add_fst.vp,var->typ_upk);
    add_fst_scv=ptr_unn_2_scv(var->typ_upk,var->add_fst);
    /* Convert var to type of scale_factor for expansion */
    var=nco_var_cnf_typ(add_fst_scv.type,var);
    /* Add add_offset to var */
    (void)var_scv_add(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&add_fst_scv);
  } /* endif has_add_fst */

  /* Tell the world */  
  var->pck_ram=False;

  /* Clean up tell-tale signs that variable was ever packed */
  var->has_scl_fct=False; /* Valid scale_factor attribute exists */
  var->has_add_fst=False; /* Valid add_offset attribute exists */
  var->scl_fct.vp=nco_free(var->scl_fct.vp);
  var->add_fst.vp=nco_free(var->add_fst.vp);

  if(dbg_lvl_get() >= 2) (void)fprintf(stderr,"%s: PACKING Unpacked %s\n",prg_nm_get(),var->nm);

  return var;
} /* end nco_var_upk() */

void
nco_var_upk_swp /* [fnc] Unpack var_in into var_out */
(const var_sct * const var_in, /* I [sct] Variable to unpack */
 var_sct * const var_out) /* I/O [sct] Variable to unpack into */
{
  /* Purpose: Unpack var_in into var_out
     Information flow in ncpdq prevents ncpdq from calling nco_var_upk()
     directly with either var_in or var_out.
     A combination of var_in (for the right file and variable IDs) 
     and var_out (so the unpacked variable ends up in the right place) are needed
     We accomplish this by unpacking into a temporary variable and copying what
     is needed from the temporary (swap) variable to var_out.
     This routine hides the gory details of the swapped unpacking

     nco_pck_val() uses this routine for two purposes:
     1. Unpack already packed variable prior to re-packing them 
     2. Unpack already packed variables permanently */
  const char fnc_nm[]="nco_var_upk_swp()";
  var_sct *var_tmp; /* [sct] Temporary variable to be unpacked */
  
  if(dbg_lvl_get() > 0) (void)fprintf(stderr,"%s: DEBUG %s unpacking variable %s values from %s to %s\n",prg_nm_get(),fnc_nm,var_in->nm,nco_typ_sng(var_out->typ_pck),nco_typ_sng(var_out->typ_upk));
  /* Output file does not contain packing attributes yet 
     Hence unpacking var_out directly is impossible 
     Instead, make var_tmp a copy of var_in and unpack var_tmp 
     Then copy needed elements of var_tmp to var_out 
     Then delete the rest of var_tmp 
     Fields modified in nco_var_upk() must be explicitly updated in var_out */
  var_tmp=nco_var_dpl(var_in);
  var_tmp=nco_var_upk(var_tmp); 
  var_out->type=var_tmp->type;
  var_out->val=var_tmp->val;
  var_out->pck_ram=var_tmp->pck_ram;
  var_out->has_scl_fct=var_tmp->has_scl_fct;
  var_out->has_add_fst=var_tmp->has_add_fst;
  var_out->scl_fct.vp=nco_free(var_out->scl_fct.vp);
  var_out->add_fst.vp=nco_free(var_out->add_fst.vp);
  /* var_tmp->val buffer now doubles as var_out->val buffer
     Must prevent nco_var_free() from free()'ing var_tmp->val
     Setting var_tmp->val.vp=NULL accomplishes this
     Use var_out->val.vp to free() this buffer later with nco_var_free() */
  var_tmp->val.vp=NULL;
  if(var_tmp != NULL) var_tmp=nco_var_free(var_tmp);
} /* end nco_var_upk_swp() */
