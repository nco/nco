/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_pck.c,v 1.10 2002-08-29 05:16:02 zender Exp $ */

/* Purpose: NCO utilities for packing and unpacking variables */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_pck.h" /* Packing and unpacking variables */

/* Notes on packing/unpacking:
   Routines in this file must be used in correct order:
   pck_dsk_inq(): called first, e.g., in nco_var_fll(), before var_prc copied to var_prc_out
   nco_var_upk(): called in data retrieval routine, e.g., in nco_var_get()
   nco_var_pck(): called just before writing output file, e.g., in main()
   Bookkeeping hassle is keeping flags in var_prc synchronized with flags in var_prc_out

   From netCDF User's Guide:
   scale_factor: If present for a variable, the data are to be multiplied by this factor after the data are read by the application that accesses the data
   add_offset: If present for a variable, this number is added to the data after the data are read by the application. If both scale_factor and add_offset attributes are present, the data are first scaled before the offset is added. 
   When scale_factor and add_offset are used for packing, the associated variable (containing the packed data) is typically of type byte or short, whereas the unpacked values are intended to be of type float or double. Attribute's scale_factor and add_offset should both be of type intended for the unpacked data, e.g., float or double. */     

int /* O [enm] Packing type */
nco_pck_typ_get /* [fnc] Convert user-specified packing type to key */
(const char *nco_pck_sng) /* [sng] User-specified packing type */
{
  /* Purpose: Process '-P' command line argument
     Convert user-specified string to packing operation type 
     Return nco_pck_nil by default */
  /* fxm: add the rest of types */
  if(!strcmp(nco_pck_sng,"all")) return nco_pck_all_xst_att;

  return nco_pck_nil;
} /* end nco_pck_typ_get() */

bool /* O [flg] Variable is packed on disk */
pck_dsk_inq /* [fnc] Check whether variable is packed on disk */
(const int nc_id, /* I [idx] netCDF file ID */
 var_sct * const var) /* I/O [sct] Variable */
{
  /* Purpose: Check whether variable is packed on disk and set variable members 
     pck_dsk, has_scl_fct, has_add_fst, and typ_upk accordingly
     pck_dsk_inq() should be called early in application, e.g., in nco_var_fll() 
     Call pck_dsk_inq() before copying input list to output list 
     Multi-file operators which handle packing must call this routine prior
     to each read of a variable, in case that variable has been unpacked. */
  /* ncea -O -D 3 -v pck ~/nco/data/in.nc ~/nco/data/foo.nc */

  char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  
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
      (void)fprintf(stderr,"%s: WARNING pck_dsk_inq() reports scale_factor for %s is not NC_FLOAT or NC_DOUBLE. Will not attempt to unpack using scale_factor.\n",prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(scl_fct_lng != 1){
      (void)fprintf(stderr,"%s: WARNING pck_dsk_inq() reports %s has scale_factor of length %li. Will not attempt to unpack using scale_factor\n",prg_nm_get(),var->nm,scl_fct_lng); 
      return False;
    } /* endif */
    var->has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
    var->typ_upk=scl_fct_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
  } /* endif */

  /* Vet add_offset */
  rcd=nco_inq_att_flg(nc_id,var->id,add_fst_sng,&add_fst_typ,&add_fst_lng);
  if(rcd != NC_ENOTATT){
    if(add_fst_typ != NC_FLOAT && add_fst_typ != NC_DOUBLE){
      (void)fprintf(stderr,"%s: WARNING pck_dsk_inq() reports add_offset for %s is not NC_FLOAT or NC_DOUBLE. Will not attempt to unpack.\n",prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(add_fst_lng != 1){
      (void)fprintf(stderr,"%s: WARNING pck_dsk_inq() reports %s has add_offset of length %li. Will not attempt to unpack.\n",prg_nm_get(),var->nm,add_fst_lng); 
      return False;
    } /* endif */
    var->has_add_fst=True; /* [flg] Valid add_offset attribute exists */
    var->typ_upk=add_fst_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
  } /* endif */

  if(var->has_scl_fct && var->has_add_fst){
    if(scl_fct_typ != add_fst_typ){
      (void)fprintf(stderr,"%s: WARNING pck_dsk_inq() reports type of scale_factor does not equal type of add_offset. Will not attempt to unpack.\n",prg_nm_get());
      return False;
    } /* endif */
  } /* endif */

  if(var->has_scl_fct || var->has_add_fst){
    /* Variable is considered packed iff either or both valid scale_factor or add_offset exist */
    var->pck_dsk=True; /* [flg] Variable is packed on disk */
    /* If variable is packed on disk and is in memory then variable is packed in memory */
    var->pck_ram=True; /* [flg] Variable is packed in memory */
    var->typ_upk=scl_fct_typ; /* [enm] Type of variable when unpacked (expanded) (in memory) */
    if(is_rth_opr(prg_get()) && dbg_lvl_get() > 2){
      (void)fprintf(stderr,"%s: PACKING Variable %s is type %s packed into type %s\n",prg_nm_get(),var->nm,nco_typ_sng(var->typ_upk),nco_typ_sng(var->typ_dsk));
      (void)fprintf(stderr,"%s: INFO Packed variables processed by all arithmetic operators are unpacked automatically. This is a relatively new feature and is still in beta-testing. Be careful with results. If it breaks you get to keep both parts.\n",prg_nm_get());
    } /* endif print packing information */
  }else{
    /* Variable is not packed since neither scale factor nor add_offset exist
       Insert hooks which depend on variable not being packed here
       Currently this is no-op */
    ;
  } /* end else */

  return var->pck_dsk; /* [flg] Variable is packed on disk (valid scale_factor, add_offset, or both attributes exist) */
  
} /* end pck_dsk_inq() */

var_sct * /* O [sct] Unpacked variable */
nco_var_upk /* [fnc] Unpack variable in memory */
(var_sct *var) /* I/O [sct] Variable to be unpacked */
{
  /* Threads: Routine is thread-unsafe */
  /* Purpose: Unpack variable
     Routine is inverse of nco_var_pck(): nco_var_upk[nco_var_pck(var)]=var */

  char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */

  /* Return if variable in memory is not currently packed */
  if(!var->pck_ram) return var;

  /* Routine should be called with variable already in memory */
  if(var->val.vp == NULL) (void)fprintf(stdout,"%s: ERROR nco_var_upk() called with empty var->val.vp\n",prg_nm_get());

  /* Packed variables are guaranteed to have both scale_factor and add_offset
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

  if(dbg_lvl_get() >= 2){
    (void)fprintf(stderr,"%s: PACKING Unpacked %s\n",prg_nm_get(),var->nm);
    (void)fprintf(stderr,"%s: WARNING Writing unpacked data to disk, or repacking and writing packed data, is not yet fully supported, output disk values of %s may be incorrect.\n",prg_nm_get(),var->nm);
  } /* endif dbg */

  return var;
  
} /* end nco_var_upk() */

var_sct * /* O [sct] Packed variable */
nco_var_pck /* [fnc] Pack variable in memory */
(var_sct *var, /* I/O [sct] Variable to be packed */
 const nc_type typ_pck, /* I [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
 const bool USE_EXISTING_PCK) /* I [flg] Use existing packing scale_factor and add_offset */
{
  /* Purpose: Pack variable 
     Routine is inverse of nco_var_upk(): nco_var_pck[nco_var_upk(var)]=var 
     Currently routine outputs same variable structure as given on input
     In other words, output structure may be neglected as all changes are made 
     to input structure */

  double scl_fct_dbl=double_CEWI; /* [sct] Double precision value of scale_factor */
  double add_fst_dbl=double_CEWI; /* [sct] Double precision value of add_offset */

  /* Return if variable in memory is currently packed */
  if(var->pck_ram) return var;

  /* Routine should be called with variable already in memory */
  if(var->val.vp == NULL) (void)fprintf(stdout,"%s: ERROR nco_var_pck() called with empty var->val.vp\n",prg_nm_get());
  
  /* Packed type must be NC_CHAR or NC_SHORT */
  if(typ_pck != NC_CHAR && typ_pck != NC_SHORT) (void)fprintf(stdout,"%s: ERROR nco_var_pck() called with invalid packed type typ_pck = %s, \n",prg_nm_get(),nco_typ_sng(typ_pck));

  /* Source type must be NC_INT, NC_FLOAT, or NC_DOUBLE */
  if(var->type == NC_SHORT || var->type == NC_CHAR || var->type == NC_BYTE) (void)fprintf(stdout,"%s: ERROR nco_var_pck() called with invalid source type var->type = %s, \n",prg_nm_get(),nco_typ_sng(var->type));

  if(USE_EXISTING_PCK){
    /* Assume var->scl_fct.vp and var->add_fst.vp are already in memory from pck_dsk_inq() */
  }else{
    /* Compute packing parameters to apply to var

       Linear packing in a nutshell:
       scale_factor = (max-min)/ndrv <--> (max-min)/scale_factor = ndrv <--> scale_factor*ndrv = max-min
       add_offset = 0.5*(min+max)
       pck = (upk-add_offset)/scale_factor = (upk-0.5*(min+max))*ndrv/(max-min)
       upk = scale_factor*pck + add_offset = (max-min)*pck/ndrv + 0.5*(min+max) 

       where 

       ndrv = number of discrete representable values for given type of packed variable and
       ndrv = 256 iff var->typ_pck == NC_CHAR
       ndrv = 256*256 iff var->typ_pck == NC_SHORT */

    double ndrv_dbl=double_CEWI; /* [frc] Double precision value of number of discrete representable values */
    double max_mns_min_dbl; /* [frc] Maximum value minus minimum value */

    ptr_unn ptr_unn_min; /* [ptr] Pointer union to minimum value of variable */
    ptr_unn ptr_unn_max; /* [ptr] Pointer union to maximum value of variable */
    
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
    if(var->tally == NULL) (void)fprintf(stdout,"%s: ERROR var->tally==NULL in nco_var_pck(), no room for incrementing tally while in nco_var_add()\n",prg_nm_get());
    /* max_var->val is overridden with add_offset answers, no longer valid as max_var */
    (void)nco_var_add((nc_type)NC_DOUBLE,1L,var->has_mss_val,var->mss_val,var->tally,min_var->val,max_var->val);
    (void)nco_var_mlt((nc_type)NC_DOUBLE,1L,var->has_mss_val,var->mss_val,hlf_var->val,max_var->val);
    /* Reset tally buffer to zero for any subsequent arithmetic */
    (void)nco_zero_long(var->sz,var->tally);
    /* Contents of max_var are actually add_offset */
    (void)val_conform_type((nc_type)NC_DOUBLE,max_var->val,var->type,var->add_fst);

    /* ndrv is 2^{bits per packed value} where bppv = 8 for NC_CHAR and bppv = 16 for NC_SHORT
       Subtract one to leave slop for rounding errors */
    if(typ_pck == NC_CHAR){
      ndrv_dbl=256.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    }else if(typ_pck == NC_SHORT){
      ndrv_dbl=65536.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    }else if(typ_pck == NC_INT){
      ndrv_dbl=4294967295.0-1.0; /* [sct] Double precision value of number of discrete representable values */
    } /* end else */
    ndrv_unn.d=ndrv_dbl; /* Generic container for number of discrete representable values */
    ndrv_var=scl_mk_var(ndrv_unn,NC_DOUBLE); /* [sct] Variable structure for number of discrete representable values */

    /* scale_factor is (max-min)/ndrv
       If max-min = 0 then variable is constant value so scale_factor=0.0 and add_offset=var
       If max-min > ndrv then precision is worse than 1.0
       If max-min < ndrv then precision is better than 1.0 */
    (void)nco_var_sbt((nc_type)NC_DOUBLE,1L,var->has_mss_val,var->mss_val,min_var->val,max_var_dpl->val);
    /* max-min is currently stored in max_var_dpl */
    max_mns_min_dbl=ptr_unn_2_scl_dbl(max_var_dpl->val,max_var_dpl->type); 

    if(max_mns_min_dbl != 0.0){
      (void)nco_var_dvd((nc_type)NC_DOUBLE,1L,var->has_mss_val,var->mss_val,ndrv_var->val,max_var_dpl->val);
      /* Contents of max_var_dpl are actually scale_factor */
      (void)val_conform_type((nc_type)NC_DOUBLE,max_var_dpl->val,var->type,var->scl_fct);
    }else{
      /* Variable is a constant */
      zero_var=scl_mk_var(zero_unn,var->type); /* [sct] NCO variable for value 0.0 */
      /* Set scale_factor to 0.0 */
      (void)memcpy(var->scl_fct.vp,zero_var->val.vp,nco_typ_lng(var->type));
      if(zero_var != NULL) zero_var=nco_var_free(zero_var);
      /* Set add_offset to variable value */
      (void)memcpy(var->add_fst.vp,var->val.vp,nco_typ_lng(var->type));
    } /* end else */

    /* Free minimum and maximum values */
    ptr_unn_min.vp=nco_free(ptr_unn_min.vp);
    ptr_unn_max.vp=nco_free(ptr_unn_max.vp);

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

  } /* not USE_EXISTING_PCK */

  /* Create double precision value of scale_factor for diagnostics */
  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    scl_fct_dbl=ptr_unn_2_scl_dbl(var->scl_fct,var->type); 
    if(scl_fct_dbl == 0.0) (void)fprintf(stdout,"%s: WARNING nco_var_pck() reports scl_fct_dbl = 0.0\n",prg_nm_get());
  } /* endif */
  
  /* Create double precision value of add_offset for diagnostics */
  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    add_fst_dbl=ptr_unn_2_scl_dbl(var->add_fst,var->type);
  } /* endif */
  
  if(dbg_lvl_get() == 3) (void)fprintf(stdout,"%s: %s: scl_fct_dbl = %g, add_fst_dbl = %g\n",prg_nm_get(),var->nm,scl_fct_dbl,add_fst_dbl);
  
  /* Packing factors now exist and are guaranteed to be same type as variable in memory */

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
    (void)scv_conform_type(var->type,&add_fst_scv);
    (void)var_scv_sub(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&add_fst_scv);
  } /* endif */

  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    /* Divide var by scale_factor */
    scv_sct scl_fct_scv;
    scl_fct_scv.type=NC_DOUBLE;
    scl_fct_scv.val.d=scl_fct_dbl;
    (void)scv_conform_type(var->type,&scl_fct_scv);
    if(scl_fct_dbl != 0.0) (void)var_scv_dvd(var->type,var->sz,var->has_mss_val,var->mss_val,var->val,&scl_fct_scv);
  } /* endif */

  /* Tell the world */
  var->pck_ram=True; /* [flg] Variable is packed in memory */
  var->typ_pck=typ_pck; /* [enm] Type of variable when packed (on disk). This should be same as typ_dsk except in cases where variable is packed in input file and unpacked in output file. */
  var->typ_upk=var->type; /* [enm] Type of variable when unpacked (expanded) (in memory) */

  /* Convert variable to user-specified packed type
     This is where var->type is changed from original to packed type */
  var=nco_var_cnf_typ(typ_pck,var);

  if(dbg_lvl_get() >=3){
    (void)fprintf(stderr,"%s: PACKING Packed %s\n",prg_nm_get(),var->nm);
    (void)fprintf(stderr,"%s: WARNING Writing packed data to disk, or repacking and writing packed data, is not yet fully supported, output disk values of %s may be incorrect.\n",prg_nm_get(),var->nm);
  } /* endif dbg */

  return var;
  
} /* end nco_var_pck() */

var_sct * /* O [sct] Packed variable */
nco_put_var_pck /* [fnc] Pack variable in memory and write packing attributes to disk */
(const int out_id, /* I [id] netCDF output file ID */
 var_sct *var, /* I/O [sct] Variable to be packed */
 const int nco_pck_typ) /* [enm] Packing operation type */
{
  /* Purpose: Pack variable in memory and write packing attributes to disk
     NB: Routine is not complete and not currently used */
  
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
  default:
    break;
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
