/* $Header$ */

/* Purpose: Conform variable types */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_cnf_typ.h" /* Conform variable types */

void
cast_void_nctype /* [fnc] Cast generic pointer to netCDF type */
(const nc_type type, /* I [enm] netCDF type to cast void pointer to */
 ptr_unn * const ptr) /* I/O [ptr] Pointer to pointer union whose vp element will be cast to type type */
{
  /* Purpose: Cast generic pointer in ptr_unn structure from type void to output netCDF type */
  switch(type){
  case NC_FLOAT:
    ptr->fp=(float *)ptr->vp;
    break;
  case NC_DOUBLE:
    ptr->dp=(double *)ptr->vp;
    break;
  case NC_INT:
    ptr->ip=(nco_int *)ptr->vp;
    break;
  case NC_SHORT:
    ptr->sp=(short *)ptr->vp;
    break;
  case NC_CHAR:
    ptr->cp=(nco_char *)ptr->vp;
    break;
  case NC_BYTE:
    ptr->bp=(nco_byte *)ptr->vp;
    break;
  case NC_UBYTE:
    ptr->ubp=(nco_ubyte *)ptr->vp;
    break;
  case NC_USHORT:
    ptr->usp=(nco_ushort *)ptr->vp;
    break;
  case NC_UINT:
    ptr->uip=(nco_uint *)ptr->vp;
    break;
  case NC_INT64:
    ptr->i64p=(nco_int64 *)ptr->vp;
    break;
  case NC_UINT64:
    ptr->ui64p=(nco_uint64 *)ptr->vp;
    break;
  case NC_STRING:
    ptr->sngp=(nco_string *)ptr->vp;
    break;
  case NC_VLEN:
    ptr->vlnp=(nco_vlen *)ptr->vp;
    break;
  case NC_OPAQUE:
    ptr->opqp=(nco_opaque *)ptr->vp;
    break;
  case NC_ENUM:
    ptr->enmp=(nco_enum *)ptr->vp;
    break;
  case NC_COMPOUND:
    ptr->cpdp=(nco_compound *)ptr->vp;
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
} /* end cast_void_nctype() */

void
cast_nctype_void /* [fnc] Cast generic pointer in ptr_unn structure from type type to type void */
(const nc_type type, /* I [enm] netCDF type of pointer */
 ptr_unn * const ptr) /* I/O pointer to pointer union which to cast from type type to type void */
{
  /* Cast generic pointer in ptr_unn structure from type type to type void */
  switch(type){
  case NC_FLOAT:
    ptr->vp=(void *)ptr->fp;
    break;
  case NC_DOUBLE:
    ptr->vp=(void *)ptr->dp;
    break;
  case NC_INT:
    ptr->vp=(void *)ptr->ip;
    break;
  case NC_SHORT:
    ptr->vp=(void *)ptr->sp;
    break;
  case NC_CHAR:
    ptr->vp=(void *)ptr->cp;
    break;
  case NC_BYTE:
    ptr->vp=(void *)ptr->bp;
    break;
  case NC_UBYTE:
    ptr->vp=(void *)ptr->ubp;
    break;
  case NC_USHORT:
    ptr->vp=(void *)ptr->usp;
    break;
  case NC_UINT:
    ptr->vp=(void *)ptr->uip;
    break;
  case NC_INT64:
    ptr->vp=(void *)ptr->i64p;
    break;
  case NC_UINT64:
    ptr->vp=(void *)ptr->ui64p;
    break;
  case NC_STRING:
    ptr->vp=(void *)ptr->sngp;
    break;
  case NC_VLEN:
    ptr->vp=(void *)ptr->vlnp;
    break;
  case NC_OPAQUE:
    ptr->vp=(void *)ptr->opqp;
    break;
  case NC_ENUM:
    ptr->vp=(void *)ptr->enmp;
    break;
  case NC_COMPOUND:
    ptr->vp=(void *)ptr->cpdp;
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
} /* end cast_nctype_void() */

var_sct * /* O [var] Variable after (possible) conversion */
nco_typ_cnv_rth /* [fnc] Convert char, short, long, int, and float types to doubles before arithmetic */
(var_sct *var, /* I/O [var] Variable to be considered for conversion */
 const int nco_op_typ) /* I [enm] Operation type */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Convert char, short, long, int types to doubles for arithmetic
     Conversions are performed unless arithmetic operation type is min or max
     Floats (and doubles, of course) are not converted for performance reasons
     Remember to convert back after weighting and arithmetic are complete! */

  /* Variables which are unpacked into NC_FLOAT should remain NC_FLOAT here
     Unpacking happens 'transparently' when original data are read by nco_var_get() 
     Output structures (i.e., var_prc_out) often correspond to original input type
     Thus var structure may have typ_upk=NC_FLOAT and type=NC_SHORT
     In that case, promote based on typ_upk rather than on type
     Otherwise most var's that had been unpacked would be converted to NC_DOUBLE here
     That would put them in conflict with corresponding var_out, which is usually
     based on typ_upk
     Check this first, then proceed with normal non-float->double conversion */

  /* 20130906:
     Users have long been uncomfortable with not implicitly converting floats to doubles
     A new section of the manual that describes the advantages and disadvantages:
     http://nco.sf.net/nco.html#dbl
     Implementing --dbl switch on ncwa, ncra, nces (ncap2?) to force implicit conversion */
  if(nco_rth_cnv_get() == nco_rth_flt_flt){

    /* NCO default until 201309: promote, where necessary, anything but floats and doubles
       Now non-default and activated only by --flt switch */
    if(var->typ_upk == NC_FLOAT){
      var=nco_var_cnf_typ((nc_type)NC_FLOAT,var);
    }else{ /* Conversion only for appropriate operation types */ 
      if(var->type != NC_FLOAT && var->type != NC_DOUBLE && nco_op_typ != nco_op_min && nco_op_typ != nco_op_max && nco_op_typ != nco_op_mabs && nco_op_typ != nco_op_mebs && nco_op_typ != nco_op_mibs) var=nco_var_cnf_typ((nc_type)NC_DOUBLE,var);
    } /* end if */

  }else{ /* !nco_rth_flt_flt */

    /* NCO default after 201309: promote, where necessary, anything but doubles
       Now default and activated redundantly --dbl switch */
    /* Conversion only for appropriate operation types */ 
    if(var->type != NC_DOUBLE && nco_op_typ != nco_op_min && nco_op_typ != nco_op_max && nco_op_typ != nco_op_mabs && nco_op_typ != nco_op_mebs && nco_op_typ != nco_op_mibs) var=nco_var_cnf_typ((nc_type)NC_DOUBLE,var);
    
  } /* !nco_rth_flt_flt */
  
  return var;
} /* nco_typ_cnv_rth() */

var_sct * /* O [sct] Variable reverted to on-disk type */
nco_cnv_var_typ_dsk  /* [fnc] Revert variable to on-disk type */
(var_sct *var) /* I [sct] Variable to be reverted */
{
  /* Purpose: Revert variable to on-disk type */
  
  if(var->type != var->typ_dsk) var=nco_var_cnf_typ(var->typ_dsk,var);
  
  return var;
} /* nco_cnv_var_typ_dsk() */

var_sct * /* O [sct] Variable with mss_val converted to typ_upk */
nco_cnv_mss_val_typ  /* [fnc] Convert missing_value, if any, to mss_val_out_typ */
(var_sct *var, /* I [sct] Variable with missing_value to convert */
 const nc_type mss_val_out_typ) /* I [enm] Type of mss_val on output */
{
  /* Purpose: Convert variable missing_value field, if any, to mss_val_out_typ
     Routine is currently called only by ncra and by nco_var_get(), for following reason:
     Most applications should call nco_var_cnf_typ() without calling nco_cnv_mss_val_typ()
     since nco_var_cnf_typ() converts misssing_value type along with variable type.
     The important exception to this is ncra
     ncra refreshes variable metadata (including missing_value, if any) 
     once per file (naturally), but refreshes variable values once per record.
     Current type of missing_value is not stored separately in variable structure
     (this is probably a mistake), so type of missing value may remain as promoted
     type for arithmetic.
     When next record is read and nco_typ_cnf_rth() promotion of new input 
     to arithmetic type (double, if necessary) will fail when it tries to
     convert the missing_value, if any, which _was already promoted_.
     Performing type conversion on memory already converted is a no-no!
     And it results in unpredictable and incorrect answers
     Thus it is very important to synchronize type of variable and missing_value 
     In the case described above, ncra simply calls this routine to convert 
     missing_value to typ_upk at the end of each record.
     Routine is dangerous because it _allows_ mss_val and variable to be different types
     Make sure you have a (very) good reason to do this!
     Better permanent solution is to add missing_value type to variable structure */

  nc_type var_in_typ; /* [enm] Type of variable and mss_val on input */

  ptr_unn mss_val_in;
  ptr_unn mss_val_out;

  var_in_typ=var->type; /* [enm] Type of variable and mss_val on input */

  /* Skip if no missing_value or if missing_value is already typ_upk */ 
  if(!var->has_mss_val || var_in_typ == mss_val_out_typ) return var; 

  /* Simple error-checking and diagnostics */
  if(nco_dbg_lvl_get() >= nco_dbg_scl && nco_dbg_lvl_get() != nco_dbg_dev){
    (void)fprintf(stdout,"%s: %s NCO_MSS_VAL_SNG attribute of variable %s from type %s to type %s\n",nco_prg_nm_get(),mss_val_out_typ > var_in_typ ? "Promoting" : "Demoting",var->nm,nco_typ_sng(var_in_typ),nco_typ_sng(mss_val_out_typ));
  } /* end if */
  
  /* Sequence of following commands is important (copy before overwriting!) */
  mss_val_in=var->mss_val;
  mss_val_out.vp=(void *)nco_malloc(nco_typ_lng(mss_val_out_typ));
  (void)nco_val_cnf_typ(var_in_typ,mss_val_in,mss_val_out_typ,mss_val_out);
  var->mss_val=mss_val_out;
  /* Free original 
     Of course this only changes var_in->mss_val
     Calling routine must update var_out->mss_val if var_out->val points to var_in->val
     This dangling pointer was a problem in ncpdq */
  mss_val_in.vp=nco_free(mss_val_in.vp);
  
  return var; /* O [sct] Variable with mss_val converted to typ_upk */
} /* nco_cnv_mss_val_typ() */

var_sct * /* O [sct] Pointer to variable structure of type var_out_typ */
nco_var_cnf_typ /* [fnc] Return copy of input variable typecast to desired type */
(const nc_type var_out_typ, /* I [enm] Type to convert variable structure to */
 var_sct * const var_in) /* I/O [enm] Pointer to variable structure (may be destroyed) */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Return copy of input variable typecast to desired type
     Routine converts missing_value, if any, to output type
     Routine saves time by returning original variable structure
     with val and type members changed as necessary
     Routine assumes variable and missing_value, if any, are same type in memory
     This is currently always true except briefly in ncra (and possibly ncpdq)
     This condition is unsafe and is described more fully in nco_cnv_mss_val_typ() */
  long idx;
  long sz;
  long sz_msk=long_CEWI; /* Holds value when called with var_in->val.vp==NULL */
  
  nc_type var_in_typ;
  
  ptr_unn val_in;
  ptr_unn val_out;

  var_sct *var_out;

  /* Do types of variable AND its missing value already match?
     This routine assumes missing_value, if any, to be same type as variable */
  if(var_in->type == var_out_typ) return var_in;

  if(var_in->val.vp == NULL){
    /* Variable has no data when var_in.val.vp==NULL
       In this case function should only convert missing values 
       Accomplish this by temporarily masking off val_in by setting var_in->sz=0
       Restore correct size at function end
       fxm: 20050521 Which operators take advantage of this behavior? */
    sz_msk=var_in->sz;
    var_in->sz=0L;
  } /* endif NULL */

  /* Setting output pointer equal to input pointer is confusing
     Theoretical advantage is that it speeds up routine 
     Nevertheless, be careful... */
  var_out=var_in;
  
  var_in_typ=var_in->type;
  
  /* Simple error-checking and diagnostics */
  if(nco_dbg_lvl_get() >= nco_dbg_scl && nco_dbg_lvl_get() != nco_dbg_dev){
    (void)fprintf(stdout,"%s: %s variable %s from type %s to type %s\n",nco_prg_nm_get(),var_out_typ > var_in_typ ? "Promoting" : "Demoting",var_in->nm,nco_typ_sng(var_in_typ),nco_typ_sng(var_out_typ));
  } /* end if */
  
  /* Move current variable values to swap location */
  val_in=var_in->val;
  
  /* Allocate space for type-conforming values */
  var_out->type=var_out_typ;
  var_out->val.vp=(void *)nco_malloc(var_out->sz*nco_typ_lng(var_out->type));
  
  /* Define convenience variables to avoid repetitive indirect addressing */
  sz=var_out->sz;
  val_out=var_out->val;
  
  /* Copy and typecast missing_value attribute, if any */
  /* Calling routine must avoid re-promoting missing values already promoted during arithmetic */
  if(var_out->has_mss_val){
    ptr_unn var_in_mss_val;

    /* Sequence of following commands is important (copy before overwriting!) */
    var_in_mss_val=var_out->mss_val;
    var_out->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_out->type));
    (void)nco_val_cnf_typ(var_in_typ,var_in_mss_val,var_out_typ,var_out->mss_val);
    /* Free original */
    var_in_mss_val.vp=nco_free(var_in_mss_val.vp);
  } /* end if */

  /* Typecast pointer to values before access 
     There is only one var structure so use shortcut, de-referenced types */
  (void)cast_void_nctype(var_in_typ,&val_in);
  (void)cast_void_nctype(var_out_typ,&val_out);
  
  /* Copy and typecast entire array of values, using C implicit coercion */
  switch(var_out_typ){
  case NC_FLOAT:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.dp[idx];} break; 
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_DOUBLE:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.dp[idx];} break; 
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.sp[idx];} break;
      /* valgrind detects uninitialized write errors in following line with GCC 3.4 */
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_INT:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=(nco_int)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=(nco_int)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_SHORT:
    switch(var_in_typ){
      /* NCO began rounding floating point numbers while converting them to integers on 20111020 
	 Recommendation and patch from Neil Davis to fix packing bias and align with netCDF Best Practices
	 Neil recommended using lround() family and I knew no better so did that from 20111020-20130327
	 On 20130327 replaced lround() with lrint() family in internal NCO rounding
	 Halfway cases that were rounded away from zero now round to nearest even integer
	 NB: Rounding with lround(), lroundf(), llround(), and llroundf() imposes dependency on <math.h>
	 NB: Rounding with lrint(), lrintf(), llrint(), and llrintf() imposes dependency on <math.h> */
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)lrintf(val_in.fp[idx]);} break;  /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
      /* case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)lroundf(val_in.fp[idx]);} break; *//* Coerce to avoid C++ compiler assignment warning */
      /* case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)lround(val_in.dp[idx]);} break; *//* Coerce to avoid C++ compiler assignment warning */
      /* Prior to 20111020 NCO did not round() floating point numbers while converting them to integers
	 Instead it used explicit coercion, which is the same as using truncate(), as the following: */
      /* case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)val_in.fp[idx];} break; *//* Coerce to avoid C++ compiler assignment warning */
      /* case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)val_in.dp[idx];} break; *//* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_CHAR:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=(nco_char)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=(nco_char)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_BYTE:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=(nco_byte)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=(nco_byte)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UBYTE:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=(nco_ubyte)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=(nco_ubyte)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_USHORT:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=(nco_ushort)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=(nco_ushort)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UINT:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=(nco_uint)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=(nco_uint)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_INT64:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=(nco_int64)llrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=(nco_int64)llrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UINT64:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=(nco_uint64)llrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=(nco_uint64)llrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_STRING:
    switch(var_in_typ){
    case NC_FLOAT: break;
    case NC_DOUBLE: break;
    case NC_INT: break;
    case NC_SHORT: break;
    case NC_CHAR: break;
    case NC_BYTE: break;
    case NC_UBYTE: break;
    case NC_USHORT: break;
    case NC_UINT: break;
    case NC_INT64: break;
    case NC_UINT64: break;
    case NC_STRING: for(idx=0L;idx<sz;idx++) {val_out.sngp[idx]=val_in.sngp[idx];} break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: we operated on local copies of val_in and val_out
     It is only neccessary to un-typecast pointer to val_in because we access it one more time
     Un-typecast pointer to val_out for symmetry */
  (void)cast_nctype_void(var_in_typ,&val_in);
  (void)cast_nctype_void(var_out_typ,&val_out);

  /* If var_in.vp empty then unmask sz */
  if(val_in.vp == NULL) var_out->sz=sz_msk;

  /* Free input variable data */
  val_in.vp=nco_free(val_in.vp);
  
  return var_out;
  
} /* end nco_var_cnf_typ() */

var_sct * /* O [sct] Pointer to variable structure of type var_out_typ */
nco_var_cnf_typ_tst /* [fnc] Return copy of input variable typecast to desired type */
(const nc_type var_out_typ, /* I [enm] Type to convert variable structure to */
 var_sct * const var_in) /* I/O [enm] Pointer to variable structure (may be destroyed) */
{
  /* 20120302: Same as nco_var_cnf_typ() but keeps missing values in range on down-promotions
     Similar changes were made to nco_val_cnf_typ_tst() which is based on nco_val_cnf_typ()
     Intended to address TODO nco1035 */
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Return copy of input variable typecast to desired type
     Routine converts missing_value, if any, to output type
     Routine saves time by returning original variable structure
     with val and type members changed as necessary
     Routine assumes variable and missing_value, if any, are same type in memory
     This is currently always true except briefly in ncra (and possibly ncpdq)
     This condition is unsafe and is described more fully in nco_cnv_mss_val_typ() */
  long idx;
  long sz;
  long sz_msk=long_CEWI; /* Holds value when called with var_in->val.vp==NULL */
  
  nc_type var_in_typ;
  
  ptr_unn val_in;
  ptr_unn val_out;

  var_sct *var_out;

  /* Do types of variable AND its missing value already match?
     This routine assumes missing_value, if any, to be same type as variable */
  if(var_in->type == var_out_typ) return var_in;

  if(var_in->val.vp == NULL){
    /* Variable has no data when var_in.val.vp==NULL
       In this case function should only convert missing values 
       Accomplish this by temporarily masking off val_in by setting var_in->sz=0
       Restore correct size at function end
       fxm: 20050521 Which operators take advantage of this behavior? */
    sz_msk=var_in->sz;
    var_in->sz=0L;
  } /* endif NULL */

  /* Setting output pointer equal to input pointer is confusing
     Theoretical advantage is that it speeds up routine 
     Nevertheless, be careful... */
  var_out=var_in;
  
  var_in_typ=var_in->type;
  
  /* Simple error-checking and diagnostics */
  if(nco_dbg_lvl_get() >= nco_dbg_scl && nco_dbg_lvl_get() != nco_dbg_dev){
    (void)fprintf(stdout,"%s: %s variable %s from type %s to type %s\n",nco_prg_nm_get(),var_out_typ > var_in_typ ? "Promoting" : "Demoting",var_in->nm,nco_typ_sng(var_in_typ),nco_typ_sng(var_out_typ));
  } /* end if */
  
  /* Move current variable values to swap location */
  val_in=var_in->val;
  
  /* Allocate space for type-conforming values */
  var_out->type=var_out_typ;
  var_out->val.vp=(void *)nco_malloc(var_out->sz*nco_typ_lng(var_out->type));
  
  /* Define convenience variables to avoid repetitive indirect addressing */
  sz=var_out->sz;
  val_out=var_out->val;
  
  /* Copy and typecast missing_value attribute, if any */
  /* Calling routine must avoid re-promoting missing values already promoted during arithmetic */
  if(var_out->has_mss_val){
    ptr_unn var_in_mss_val;

    /* Sequence of following commands is important (copy before overwriting!) */
    var_in_mss_val=var_out->mss_val;
    var_out->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_out->type));
    (void)nco_val_cnf_typ(var_in_typ,var_in_mss_val,var_out_typ,var_out->mss_val);
    /* Free original */
    var_in_mss_val.vp=nco_free(var_in_mss_val.vp);
  } /* end if */

  /* Typecast pointer to values before access 
     There is only one var structure so use shortcut, de-referenced types */
  (void)cast_void_nctype(var_in_typ,&val_in);
  (void)cast_void_nctype(var_out_typ,&val_out);
  
  /* Copy and typecast entire array of values, using C implicit coercion */
  switch(var_out_typ){
  case NC_FLOAT:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.dp[idx];} break; 
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_DOUBLE:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.dp[idx];} break; 
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.sp[idx];} break;
      /* valgrind detects uninitialized write errors in following line with GCC 3.4 */
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_INT:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=(nco_int)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=(nco_int)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.ip[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_SHORT:
    switch(var_in_typ){
      /* NCO began rounding floating point numbers while converting them to integers on 20111020 
	 Recommendation and patch from Neil Davis to fix packing bias and align with netCDF Best Practices
	 Neil recommended using lround() family and I knew no better so did that from 20111020-20130327
	 On 20130327 replaced lround() with lrint() family in internal NCO rounding
	 Halfway cases that were rounded away from zero now round to nearest even integer
	 NB: Rounding with lround(), lroundf(), llround(), and llroundf() imposes dependency on <math.h>
	 NB: Rounding with lrint(), lrintf(), llrint(), and llrintf() imposes dependency on <math.h> */
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
      /* case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)lroundf(val_in.fp[idx]);} break; *//* Coerce to avoid C++ compiler assignment warning */
      /* case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)lround(val_in.dp[idx]);} break; *//* Coerce to avoid C++ compiler assignment warning */
      /* Prior to 20111020 NCO did not round() floating point numbers while converting them to integers
	 Instead it used explicit coercion, which is the same as using truncate(), as the following: */
      /* case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)val_in.fp[idx];} break; *//* Coerce to avoid C++ compiler assignment warning */
      /* case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short int)val_in.dp[idx];} break; *//* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_CHAR:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=(nco_char)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=(nco_char)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_BYTE:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=(nco_byte)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=(nco_byte)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UBYTE:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=(nco_ubyte)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=(nco_ubyte)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.ubp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_USHORT:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=(nco_ushort)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=(nco_ushort)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.usp[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UINT:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=(nco_uint)lrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=(nco_uint)lrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.uip[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_INT64:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=(nco_int64)llrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=(nco_int64)llrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.i64p[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UINT64:
    switch(var_in_typ){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=(nco_uint64)llrintf(val_in.fp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=(nco_uint64)llrint(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.ip[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.bp[idx];} break;
    case NC_UBYTE: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.ubp[idx];} break;
    case NC_USHORT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.usp[idx];} break;
    case NC_UINT: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.uip[idx];} break;
    case NC_INT64: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.i64p[idx];} break;
    case NC_UINT64: for(idx=0L;idx<sz;idx++) {val_out.ui64p[idx]=val_in.ui64p[idx];} break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_STRING:
    switch(var_in_typ){
    case NC_FLOAT: break;
    case NC_DOUBLE: break;
    case NC_INT: break;
    case NC_SHORT: break;
    case NC_CHAR: break;
    case NC_BYTE: break;
    case NC_UBYTE: break;
    case NC_USHORT: break;
    case NC_UINT: break;
    case NC_INT64: break;
    case NC_UINT64: break;
    case NC_STRING: for(idx=0L;idx<sz;idx++) {val_out.sngp[idx]=val_in.sngp[idx];} break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: we operated on local copies of val_in and val_out
     It is only neccessary to un-typecast pointer to val_in because we access it one more time
     Un-typecast pointer to val_out for symmetry */
  (void)cast_nctype_void(var_in_typ,&val_in);
  (void)cast_nctype_void(var_out_typ,&val_out);

  /* If var_in.vp empty then unmask sz */
  if(val_in.vp == NULL) var_out->sz=sz_msk;

  /* Free input variable data */
  val_in.vp=nco_free(val_in.vp);
  
  return var_out;
  
} /* end nco_var_cnf_typ_tst() */

void
nco_val_cnf_typ /* [fnc] Copy val_in and typecast from typ_in to typ_out */
(const nc_type typ_in, /* I [enm] Type of input value */
 ptr_unn val_in, /* I [ptr] Pointer to input value */
 const nc_type typ_out, /* I [enm] Type of output value */
 ptr_unn val_out) /* I [ptr] Pointer to output value */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Fill val_out with copy of val_in that has been typecast from typ_in to typ_out
     Last-referenced state of both value pointers is assumed to be .vp, and val_out union is returned in that state */

  /* val_in and val_out should not be same pointer union since
     val_out must hold enough space (one element of type typ_out) to hold output
     and output type may be larger than input type */

  /* 20180509: Unsure why it is unsafe to return immediately when typ_in == typ_out
     However, doing so causes dozens of regression failures */
  //  if(typ_in == typ_out) return;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(typ_in,&val_in);
  (void)cast_void_nctype(typ_out,&val_out);
  
  /* Copy and typecast single value using implicit coercion rules of C */
  switch(typ_out){
  case NC_FLOAT:
    switch(typ_in){
    case NC_FLOAT: *val_out.fp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.fp=*val_in.dp; break; 
    case NC_INT: *val_out.fp=*val_in.ip; break;
    case NC_SHORT: *val_out.fp=*val_in.sp; break;
    case NC_CHAR: *val_out.fp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.fp=*val_in.bp; break;
    case NC_UBYTE: *val_out.fp=*val_in.ubp; break;
    case NC_USHORT: *val_out.fp=*val_in.usp; break;
    case NC_UINT: *val_out.fp=*val_in.uip; break;
    case NC_INT64: *val_out.fp=*val_in.i64p; break;
    case NC_UINT64: *val_out.fp=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_DOUBLE:
    switch(typ_in){
      /* 20160114: Unclear whether implicit coercion to promote NaNf to NaN works (needed for chlorophyll variable in MODIS_L2N_20140304T1120.nc) */
      //    case NC_FLOAT: if(isfinite(*val_in.fp)) *val_out.dp=*val_in.fp; else *val_out.dp=NC_MIN_DOUBLE; break; 
    case NC_FLOAT: *val_out.dp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.dp=*val_in.dp; break; 
    case NC_INT: *val_out.dp=*val_in.ip; break;
    case NC_SHORT: *val_out.dp=*val_in.sp; break;
    case NC_CHAR: *val_out.dp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.dp=*val_in.bp; break;
    case NC_UBYTE: *val_out.dp=*val_in.ubp; break;
    case NC_USHORT: *val_out.dp=*val_in.usp; break;
    case NC_UINT: *val_out.dp=*val_in.uip; break;
    case NC_INT64: *val_out.dp=*val_in.i64p; break;
    case NC_UINT64: *val_out.dp=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_INT:
    switch(typ_in){
    case NC_FLOAT: *val_out.ip=(nco_int)lrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.ip=(nco_int)lrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.ip=*val_in.ip; break;
    case NC_SHORT: *val_out.ip=*val_in.sp; break;
    case NC_CHAR: *val_out.ip=(nco_int)strtod((const char *)val_in.cp,(char **)NULL); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_BYTE: *val_out.ip=*val_in.bp; break;
    case NC_UBYTE: *val_out.ip=*val_in.ubp; break;
    case NC_USHORT: *val_out.ip=*val_in.usp; break;
    case NC_UINT: *val_out.ip=*val_in.uip; break;
    case NC_INT64: *val_out.ip=*val_in.i64p; break;
    case NC_UINT64: *val_out.ip=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_SHORT:
    switch(typ_in){
    case NC_FLOAT: *val_out.sp=(short int)lrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.sp=(short int)lrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.sp=*val_in.ip; break;
    case NC_SHORT: *val_out.sp=*val_in.sp; break;
    case NC_CHAR: *val_out.sp=(short int)strtod((const char *)val_in.cp,(char **)NULL); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_BYTE: *val_out.sp=*val_in.bp; break;
    case NC_UBYTE: *val_out.sp=*val_in.ubp; break;
    case NC_USHORT: *val_out.sp=*val_in.usp; break;
    case NC_UINT: *val_out.sp=*val_in.uip; break;
    case NC_INT64: *val_out.sp=*val_in.i64p; break;
    case NC_UINT64: *val_out.sp=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_CHAR:
    switch(typ_in){
    case NC_FLOAT: *val_out.cp=(nco_char)lrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.cp=(nco_char)lrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.cp=*val_in.ip; break;
    case NC_SHORT: *val_out.cp=*val_in.sp; break;
    case NC_CHAR: *val_out.cp=*val_in.cp; break;
    case NC_BYTE: *val_out.cp=*val_in.bp; break;
    case NC_UBYTE: *val_out.cp=*val_in.ubp; break;
    case NC_USHORT: *val_out.cp=*val_in.usp; break;
    case NC_UINT: *val_out.cp=*val_in.uip; break;
    case NC_INT64: *val_out.cp=*val_in.i64p; break;
    case NC_UINT64: *val_out.cp=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_BYTE:
    switch(typ_in){
    case NC_FLOAT: *val_out.bp=(nco_byte)lrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.bp=(nco_byte)lrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.bp=*val_in.ip; break;
    case NC_SHORT: *val_out.bp=*val_in.sp; break;
    case NC_CHAR: *val_out.bp=*val_in.cp; break;
    case NC_BYTE: *val_out.bp=*val_in.bp; break;
    case NC_UBYTE: *val_out.bp=*val_in.ubp; break;
    case NC_USHORT: *val_out.bp=*val_in.usp; break;
    case NC_UINT: *val_out.bp=*val_in.uip; break;
    case NC_INT64: *val_out.bp=*val_in.i64p; break;
    case NC_UINT64: *val_out.bp=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UBYTE:
    switch(typ_in){
    case NC_FLOAT: *val_out.ubp=(nco_ubyte)lrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.ubp=(nco_ubyte)lrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.ubp=*val_in.ip; break;
    case NC_SHORT: *val_out.ubp=*val_in.sp; break;
    case NC_CHAR: *val_out.ubp=*val_in.cp; break;
    case NC_BYTE: *val_out.ubp=*val_in.bp; break;
    case NC_UBYTE: *val_out.ubp=*val_in.ubp; break;
    case NC_USHORT: *val_out.ubp=*val_in.usp; break;
    case NC_UINT: *val_out.ubp=*val_in.uip; break;
    case NC_INT64: *val_out.ubp=*val_in.i64p; break;
    case NC_UINT64: *val_out.ubp=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_USHORT:
    switch(typ_in){
    case NC_FLOAT: *val_out.usp=(nco_ushort)lrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.usp=(nco_ushort)lrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.usp=*val_in.ip; break;
    case NC_SHORT: *val_out.usp=*val_in.sp; break;
    case NC_CHAR: *val_out.usp=*val_in.cp; break;
    case NC_BYTE: *val_out.usp=*val_in.bp; break;
    case NC_UBYTE: *val_out.usp=*val_in.ubp; break;
    case NC_USHORT: *val_out.usp=*val_in.usp; break;
    case NC_UINT: *val_out.usp=*val_in.uip; break;
    case NC_INT64: *val_out.usp=*val_in.i64p; break;
    case NC_UINT64: *val_out.usp=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UINT:
    switch(typ_in){
    case NC_FLOAT: *val_out.uip=(nco_uint)lrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.uip=(nco_uint)lrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.uip=*val_in.ip; break;
    case NC_SHORT: *val_out.uip=*val_in.sp; break;
    case NC_CHAR: *val_out.uip=*val_in.cp; break;
    case NC_BYTE: *val_out.uip=*val_in.bp; break;
    case NC_UBYTE: *val_out.uip=*val_in.ubp; break;
    case NC_USHORT: *val_out.uip=*val_in.usp; break;
    case NC_UINT: *val_out.uip=*val_in.uip; break;
    case NC_INT64: *val_out.uip=*val_in.i64p; break;
    case NC_UINT64: *val_out.uip=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_INT64:
    switch(typ_in){
    case NC_FLOAT: *val_out.i64p=(nco_int64)llrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.i64p=(nco_int64)llrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.i64p=*val_in.ip; break;
    case NC_SHORT: *val_out.i64p=*val_in.sp; break;
    case NC_CHAR: *val_out.i64p=*val_in.cp; break;
    case NC_BYTE: *val_out.i64p=*val_in.bp; break;
    case NC_UBYTE: *val_out.i64p=*val_in.ubp; break;
    case NC_USHORT: *val_out.i64p=*val_in.usp; break;
    case NC_UINT: *val_out.i64p=*val_in.uip; break;
    case NC_INT64: *val_out.i64p=*val_in.i64p; break;
    case NC_UINT64: *val_out.i64p=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_UINT64:
    switch(typ_in){
    case NC_FLOAT: *val_out.ui64p=(nco_uint64)llrintf(*val_in.fp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.ui64p=(nco_uint64)llrint(*val_in.dp); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.ui64p=*val_in.ip; break;
    case NC_SHORT: *val_out.ui64p=*val_in.sp; break;
    case NC_CHAR: *val_out.ui64p=*val_in.cp; break;
    case NC_BYTE: *val_out.ui64p=*val_in.bp; break;
    case NC_UBYTE: *val_out.ui64p=*val_in.ubp; break;
    case NC_USHORT: *val_out.ui64p=*val_in.usp; break;
    case NC_UINT: *val_out.ui64p=*val_in.uip; break;
    case NC_INT64: *val_out.ui64p=*val_in.i64p; break;
    case NC_UINT64: *val_out.ui64p=*val_in.ui64p; break;
    case NC_STRING: break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  case NC_STRING:
    switch(typ_in){
    case NC_FLOAT: break;
    case NC_DOUBLE: break;
    case NC_INT: break;
    case NC_SHORT: break;
    case NC_CHAR: break;
    case NC_BYTE: break;
    case NC_UBYTE: break;
    case NC_USHORT: break;
    case NC_UINT: break;
    case NC_INT64: break;
    case NC_UINT64: break;
    case NC_STRING: *val_out.sngp=*val_in.sngp; break;
    default: nco_dfl_case_nc_type_err(); break;
    } break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: There is no need to un-typecast input pointers because they were passed by
     value and are thus purely local to this routine. The only thing changed by this
     routine is the contents of the location pointed to by the pointer to the output value. */
  
} /* end nco_val_cnf_typ() */

int /* O [enm] Dummy return */
nco_scv_cnf_typ /* [fnc] Convert scalar attribute to typ_new using C implicit coercion */
(const nc_type typ_new, /* I [enm] Type to convert scv_old to */
 scv_sct * const scv_old) /* I/O [sct] Scalar value to convert */
{
  /* Purpose: Convert scalar attribute to typ_new using C implicit coercion */
  nc_type typ_old=scv_old->type;
  
  scv_sct scv_new;

  switch (typ_new){ 
  case NC_FLOAT:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.f=(scv_old->val).f; break; 
    case NC_DOUBLE: scv_new.val.f=(scv_old->val).d; break; 
    case NC_INT: scv_new.val.f=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.f=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.f=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.f=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.f=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.f=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.f=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.f=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;    
    } break;
  case NC_DOUBLE:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.d=(scv_old->val).f; break; 
    case NC_DOUBLE: scv_new.val.d =(scv_old->val).d; break; 
    case NC_INT: scv_new.val.d=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.d=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.d=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.d=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.d=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.d=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.d=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.d=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;    
    } break;
  case NC_INT:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.i=(nco_int)lrintf((scv_old->val).f); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: scv_new.val.i=(nco_int)lrint((scv_old->val).d); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: scv_new.val.i =scv_old->val.i; break;
    case NC_SHORT: scv_new.val.i=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.i=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.i=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.i=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.i=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.i=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.i=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_SHORT:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.s=(short int)lrintf((scv_old->val).f); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: scv_new.val.s=(short int)lrint((scv_old->val).d); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: scv_new.val.s=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.s=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.s=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.s=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.s=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.s=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.s=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.s=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;    
    } break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.b=(nco_byte)lrintf((scv_old->val).f); break;
    case NC_DOUBLE: scv_new.val.b=(nco_byte)lrint((scv_old->val).d); break;
    case NC_INT: scv_new.val.b=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.b=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.b=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.b=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.b=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.b=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.b=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.b=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_UBYTE:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.ub=(nco_ubyte)lrintf((scv_old->val).f); break;
    case NC_DOUBLE: scv_new.val.ub=(nco_ubyte)lrint((scv_old->val).d); break;
    case NC_INT: scv_new.val.ub=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.ub=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.ub=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.ub=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.ub=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.ub=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.ub=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.ub=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_USHORT:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.us=(nco_ushort)lrintf((scv_old->val).f); break;
    case NC_DOUBLE: scv_new.val.us=(nco_ushort)lrint((scv_old->val).d); break;
    case NC_INT: scv_new.val.us=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.us=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.us=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.us=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.us=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.us=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.us=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.us=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_UINT:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.ui=(nco_uint)lrintf((scv_old->val).f); break;
    case NC_DOUBLE: scv_new.val.ui=(nco_uint)lrint((scv_old->val).d); break;
    case NC_INT: scv_new.val.ui=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.ui=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.ui=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.ui=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.ui=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.ui=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.ui=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.ui=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_INT64:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.i64=(nco_int64)llrintf((scv_old->val).f); break;
    case NC_DOUBLE: scv_new.val.i64=(nco_int64)llrint((scv_old->val).d); break;
    case NC_INT: scv_new.val.i64=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.i64=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.i64=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.i64=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.i64=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.i64=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.i64=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.i64=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_UINT64:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.ui64=(nco_uint64)llrintf((scv_old->val).f); break;
    case NC_DOUBLE: scv_new.val.ui64=(nco_uint64)llrint((scv_old->val).d); break;
    case NC_INT: scv_new.val.ui64=(scv_old->val).i; break;
    case NC_SHORT: scv_new.val.ui64=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.ui64=(scv_old->val).b; break;
    case NC_UBYTE: scv_new.val.ui64=(scv_old->val).ub; break;
    case NC_USHORT: scv_new.val.ui64=(scv_old->val).us; break;
    case NC_UINT: scv_new.val.ui64=(scv_old->val).ui; break;
    case NC_INT64: scv_new.val.ui64=(scv_old->val).i64; break;
    case NC_UINT64: scv_new.val.ui64=(scv_old->val).ui64; break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_STRING:
    switch(typ_old){
    case NC_FLOAT: break;
    case NC_DOUBLE: break;
    case NC_INT: break;
    case NC_SHORT: break;
    case NC_BYTE: break;
    case NC_UBYTE: break;
    case NC_USHORT: break;
    case NC_UINT: break;
    case NC_INT64: break;
    case NC_UINT64: break;
    case NC_STRING: break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  scv_new.type=typ_new;
  *scv_old=scv_new;
  return True;
} /* end nco_scv_cnf_typ() */

nco_bool /* O [flg] Input is integer type */
nco_typ_ntg /* [fnc] Identify integer types */
(const nc_type typ_in) /* I [enm] Type to check for integer-ness */
{
  nco_bool flg_ntg=True; /* CEWI */

  switch(typ_in){
  case NC_INT: 
  case NC_SHORT: 
  case NC_INT64: 
  case NC_BYTE: 
  case NC_UINT:
  case NC_USHORT:
  case NC_UINT64:
  case NC_UBYTE: 
    flg_ntg=True;
    break;
  case NC_FLOAT: 
  case NC_DOUBLE: 
  case NC_CHAR: 
  case NC_NAT: 
  case NC_STRING: 
    flg_ntg=False;
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  return flg_ntg;
} /* end nco_typ_ntg() */

nco_bool /* O [flg] Input is signed type */
nco_typ_sgn /* [fnc] Identify signed types */
(const nc_type typ_in) /* I [enm] Type to check for signedness */
{
  nco_bool flg_sgn=True; /* CEWI */

  switch(typ_in){
  case NC_FLOAT: 
  case NC_DOUBLE: 
  case NC_INT64: 
  case NC_INT: 
  case NC_SHORT: 
  case NC_BYTE: 
  case NC_CHAR: 
  case NC_NAT: 
  case NC_STRING: 
    flg_sgn=True;
    break;       
  case NC_UBYTE: 
  case NC_USHORT:
  case NC_UINT:
  case NC_UINT64:
    flg_sgn=False;
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  return flg_sgn;
} /* end nco_typ_sgn() */

nco_bool /* O [flg] Input is netCDF3 atomic type */
nco_typ_nc3 /* [fnc] Identify netCDF3 atomic types */
(nc_type typ_in) /* I [enm] Type to check netCDF3 compliance */
{
  nco_bool flg_nc3=True; /* CEWI */

  switch(typ_in){
  case NC_FLOAT: 
  case NC_DOUBLE: 
  case NC_INT: 
  case NC_SHORT: 
  case NC_BYTE: 
  case NC_CHAR: 
    flg_nc3=True;
    break;       
  case NC_STRING: 
  case NC_INT64: 
  case NC_UBYTE: 
  case NC_USHORT:
  case NC_UINT:
  case NC_UINT64:
    flg_nc3=False;
    break;
  case NC_NAT: 
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  return flg_nc3;
} /* end nco_typ_nc3() */

nco_bool /* O [flg] Input is CDF5 atomic type */
nco_typ_nc5 /* [fnc] Identify CDF5 atomic types */
(nc_type typ_in) /* I [enm] Type to check for CDF5 compliance */
{
  nco_bool flg_nc5=True; /* CEWI */

  switch(typ_in){
  case NC_FLOAT: 
  case NC_DOUBLE: 
  case NC_INT: 
  case NC_SHORT: 
  case NC_BYTE: 
  case NC_CHAR: 
  case NC_INT64: 
  case NC_UBYTE: 
  case NC_USHORT:
  case NC_UINT:
  case NC_UINT64:
    flg_nc5=True;
    break;       
  case NC_STRING: 
    flg_nc5=False;
    break;
  case NC_NAT: 
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  return flg_nc5;
} /* end nco_typ_nc5() */

nc_type /* O [enm] CDF5 atomic type */
nco_typ_nc4_nc5 /* [fnc] Convert netCDF4 to CDF5 atomic type */
(const nc_type typ_nc4) /* I [enm] netCDF4 type */
{
  /* Purpose: Perform intelligent type conversion from netCDF4->5 type */

  /* Already CDF5 type */
  if(nco_typ_nc5(typ_nc4)) return typ_nc4;

  switch(typ_nc4){
  case NC_STRING: 
    return NC_CHAR;
    break;
  case NC_NAT:
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  return typ_nc4;
} /* end nco_typ_nc4_nc5() */

nc_type /* O [enm] netCDF3 type */
nco_typ_nc4_nc3 /* [fnc] Convert netCDF4 to netCDF3 atomic type */
(const nc_type typ_nc4) /* I [enm] netCDF4 type */
{
  /* Purpose: Perform intelligent type conversion from netCDF4->3 type */

  /* Already netCDF3 type */
  if(nco_typ_nc3(typ_nc4)) return typ_nc4;

  switch(typ_nc4){
  case NC_UBYTE:
    return NC_SHORT;
    break;
  case NC_USHORT:
  case NC_UINT:
  case NC_INT64:
  case NC_UINT64:
    return NC_INT;
    break;
  case NC_STRING: 
    return NC_CHAR;
    break;
  case NC_NAT:
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  return typ_nc4;
} /* end nco_typ_nc4_nc3() */

nc_type /* O [enm] Return Highest type */
ncap_typ_hgh /* [fnc] Return Highest type */
(nc_type typ_1, /* I [enm] type */
 nc_type typ_2) /* I [enm] type */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Perform intelligent type conversion with the netCDF3/4 types 
     The logic is as follows
     1) If one type is double highest type double .. end
     2) if one type float then highest type float .. end
     3) if both are signed type then highest type returned
        
     4) if both are unsigned type then highest type returned
        
     5) First var signed second var unsigned
        if the signed type can represent all values af the unsigned type 
        Then the signed type returned

        If the types are the same then unsigned type returned

        If the signed type is less than the unsigned type then the unsigned 
         type is returned    

        Examples:

        NC_CHAR,NC_UBYTE -> result NC_CHAR
        NC_INT, NC_USHORT -> result NC_INT
        NC_SHORT, NC_UINT -> result NC_UINT  
        NC_INT64, NC_UINT -> result NC_INT64
        NC_INT64,NC_UINT64  -> result NC_UINT64 */
 
  nco_bool v1s;
  nco_bool v2s;
 
  /* Already identical */
  if(typ_1 == typ_2) return typ_1;

  /* Deal with NC_DOUBLE */
  if(typ_1 == NC_DOUBLE || typ_2 == NC_DOUBLE) return NC_DOUBLE;

  /* Deal with NC_FLOAT */
  if(typ_1 == NC_FLOAT || typ_2 == NC_FLOAT) return NC_FLOAT;

  v1s=nco_typ_sgn(typ_1);
  v2s=nco_typ_sgn(typ_2);

  /* Both signed or both unsigned */
  if(v1s == v2s) return (typ_1 > typ_2 ? typ_1 : typ_2);  

  /* From here on one is signed the other unsigned */
  
  /* Swap vars about so var_1 is signed, var_2 unsigned */
  if(v1s == False && v2s == True){
    nc_type typ_tmp;
    typ_tmp=typ_1;typ_1=typ_2;typ_2=typ_tmp; 
  }

  switch(typ_1){
    case NC_NAT:
      return typ_2; 
      break;
    case NC_BYTE: 
      return typ_2; 
      break;
    case NC_CHAR:
      return typ_2; 
      break;
    case NC_SHORT: 
      return (typ_2 < NC_USHORT ? typ_1 : typ_2);
      break;
    case NC_INT:
      return (typ_2 < NC_UINT ? typ_1 : typ_2); 
      break;
    case NC_INT64:
      return (typ_2 < NC_UINT64 ? typ_1 : typ_2); 
      break; 
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  return typ_1;
} /* end ncap_typ_hgh() */

nc_type /* O [enm] Higher precision of input variables */
ncap_var_retype /* [fnc] Promote variable to higher common precision */
(var_sct *var_1, /* I/O [sct] Variable */
 var_sct *var_2) /* I/O [sct] Variable */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */

  /* Purpose: Perform intelligent type conversion with netCDF3/4 types */
  nc_type typ_hgh;

  typ_hgh=ncap_typ_hgh(var_1->type,var_2->type);
  if(var_1->type != typ_hgh) var_1=nco_var_cnf_typ(typ_hgh,var_1);
  if(var_2->type != typ_hgh) var_2=nco_var_cnf_typ(typ_hgh,var_2);

  return typ_hgh;
} /* end ncap_var_retype() */

nc_type /* O [enm] Highest precision of arguments */
ncap_scv_scv_cnf_typ_hgh_prc /* [fnc] Promote arguments to higher precision if necessary */
(scv_sct * const scv_1, /* I/O [sct] Scalar value */
 scv_sct * const scv_2) /* I/O [sct] Scalar value */
{
  /* fxm: TODO nco616: netCDF4 breaks assumption that range/precision increases with nc_type enum */
  /* Purpose: Promote scalar values to higher of two precisions, if necessary */
  if(scv_1->type == scv_2->type){
    return scv_1->type;
  }else if(scv_1->type > scv_2->type){
    (void)nco_scv_cnf_typ(scv_1->type,scv_2);
    return scv_1->type;
  }else{
    (void)nco_scv_cnf_typ(scv_2->type,scv_1);
    return scv_2->type;
  } /* endif */
} /* end ncap_scv_scv_cnf_typ_hgh_prc() */

nc_type /* O [enm] Highest precision of arguments */
ncap_var_scv_cnf_typ_hgh_prc /* [fnc] Promote arguments to higher precision if necessary */
(var_sct ** const var, /* I/O [sct] Variable */
 scv_sct * const scv) /* I/O [sct] Scalar value */
{
  /* fxm: TODO nco616: netCDF4 breaks assumption that range/precision increases with nc_type enum */
  /* Purpose: If types of variable and scalar value differ, convert argument with 
     lower precision to higher precision type.
     Otherwise do nothing. 
     fxm: Assumes nc_type increases monotonically with precision */

  /* Do nothing if types match */
  if((*var)->type == scv->type){
    return (*var)->type;
  }else if((*var)->type > scv->type){
    (void)nco_scv_cnf_typ((*var)->type,scv); 
    return (*var)->type;
  }else{
    *var=nco_var_cnf_typ(scv->type,*var);
    return scv->type;
  } /* endif */

} /* end ncap_var_scv_cnf_typ_hgh_prc() */
