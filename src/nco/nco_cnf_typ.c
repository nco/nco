/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_cnf_typ.c,v 1.8 2002-08-14 19:45:01 zender Exp $ */

/* Purpose: Conform variable types */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_cnf_typ.h" /* Conform variable types */

void
cast_void_nctype /* [fnc] Cast generic pointer to netCDF type */
(const nc_type type, /* I [enm] netCDF type to cast void pointer to*/
 ptr_unn * const ptr) /* I/O [ptr] Pointer to pointer union whose vp element will be cast to type type*/
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
    ptr->lp=(nco_long *)ptr->vp;
    break;
  case NC_SHORT:
    ptr->sp=(short *)ptr->vp;
    break;
  case NC_CHAR:
    ptr->cp=(unsigned char *)ptr->vp;
    break;
  case NC_BYTE:
    ptr->bp=(signed char *)ptr->vp;
    break;
  default: nco_dfl_case_nctype_err(); break;
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
    ptr->vp=(void *)ptr->lp;
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end cast_nctype_void() */

var_sct * /* O [var] Variable after (possible) conversion */
nco_typ_cnv_rth  /* [fnc] Convert char, short, long, int types to doubles before arithmetic */
(var_sct *var, /* I/O [var] Variable to be considered for conversion */
 const int nco_op_typ) /* I [enm] Operation type */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Convert char, short, long, int types to doubles for arithmetic
     Conversions are performed unless arithmetic operation type is min or max
     Floats (and doubles, of course) are not converted for performance reason 
     This routine is usually called 
     Remember to convert back after weighting and arithmetic are complete! */

  if(var->type != NC_FLOAT && var->type != NC_DOUBLE && nco_op_typ != nco_op_min && nco_op_typ != nco_op_max) var=nco_var_cnf_typ((nc_type)NC_DOUBLE,var);
  
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

var_sct * /* O [sct] Pointer to variable structure of type var_out_typ */
nco_var_cnf_typ /* [fnc] Return copy of input variable typecast to desired type */
(const nc_type var_out_typ, /* I [enm] Type to convert variable structure to */
 var_sct *var_in) /* I/O [enm] Pointer to variable structure (may be destroyed) */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Return copy of input variable typecast to desired type */

  long idx;
  long sz;
  
  nc_type var_in_type;
  
  ptr_unn val_in;
  ptr_unn val_out;

  var_sct *var_out;

  /* Do types of variable AND its missing value already match? */
  if(var_in->type == var_out_typ) return var_in;

  var_out=var_in;
  
  var_in_type=var_in->type;
  
  /* Simple error-checking and diagnostics */
  if(dbg_lvl_get() > 2){
    (void)fprintf(stderr,"%s: DEBUG %s variable %s from type %s to type %s\n",prg_nm_get(),var_out_typ > var_in_type ? "Promoting" : "Demoting",var_in->nm,nco_typ_sng(var_in_type),nco_typ_sng(var_out_typ));
  } /* end if */
  
  /* Move the current var values to swap location */
  val_in=var_in->val;
  
  /* Allocate space for type-conforming values */
  var_out->type=var_out_typ;
  var_out->val.vp=(void *)nco_malloc(var_out->sz*nco_typ_lng(var_out->type));
  
  /* Define convenience variables to avoid repetitive indirect addressing */
  sz=var_out->sz;
  val_out=var_out->val;
  
  /* Copy and typecast missing_value attribute, if any */
  /* fxm: 20020814: ncra bug here since this tries to re-convert missing values that have already been promoted to double during arithmetic */
  if(var_out->has_mss_val){
    ptr_unn var_in_mss_val;

    /* Sequence of following commands is important (copy before overwriting!) */
    var_in_mss_val=var_out->mss_val;
    var_out->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var_out->type));
    (void)val_conform_type(var_in_type,var_in_mss_val,var_out_typ,var_out->mss_val);
    /* Free original */
    var_in_mss_val.vp=nco_free(var_in_mss_val.vp);
  } /* end if */

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(var_in->type,&val_in);
  (void)cast_void_nctype(var_out->type,&var_out->val);
  
  /* Copy and typecast entire array of values, using C implicit coercion */
  switch(var_out_typ){
  case NC_FLOAT:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.dp[idx];} break; 
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.fp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_DOUBLE:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.fp[idx];} break; 
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.dp[idx];} break; 
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.dp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_INT:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=(long)val_in.fp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=(long)val_in.dp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.lp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_SHORT:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short)val_in.fp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=(short)(val_in.dp[idx]);} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.sp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_CHAR:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=(unsigned char)val_in.fp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=(unsigned char)val_in.dp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.cp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_BYTE:
    switch(var_in_type){
    case NC_FLOAT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=(signed char)val_in.fp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=(signed char)val_in.dp[idx];} break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.lp[idx];} break;
    case NC_SHORT: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.sp[idx];} break;
    case NC_CHAR: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.cp[idx];} break;
    case NC_BYTE: for(idx=0L;idx<sz;idx++) {val_out.bp[idx]=val_in.bp[idx];} break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* Un-typecast pointer to values after access */
  (void)cast_nctype_void(var_in->type,&val_in);
  (void)cast_nctype_void(var_out->type,&var_out->val);
  
  /* Free input variable data */
  val_in.vp=nco_free(val_in.vp);
  
  return var_out;
  
} /* end nco_var_cnf_typ() */

void
val_conform_type /* [fnc] Copy val_in and typecast from typ_in to typ_out */
(const nc_type typ_in, /* I [enm] Type of input value */
 ptr_unn val_in, /* I [ptr] Pointer to input value */
 const nc_type typ_out, /* I [enm] Type of output value */
 ptr_unn val_out) /* I [ptr] Pointer to output value */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Fill val_out with copy of val_in that has been typecast from typ_in to typ_out
     Last-referenced state of both value pointers is assumed to be .vp, and the val_out union is returned in that state */

  /* val_out must hold enough space (one element of type typ_out) to hold output */

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(typ_in,&val_in);
  (void)cast_void_nctype(typ_out,&val_out);
  
  /* Copy and typecast single value using implicit coercion rules of C */
  switch(typ_out){
  case NC_FLOAT:
    switch(typ_in){
    case NC_FLOAT: *val_out.fp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.fp=*val_in.dp; break; 
    case NC_INT: *val_out.fp=*val_in.lp; break;
    case NC_SHORT: *val_out.fp=*val_in.sp; break;
    case NC_CHAR: *val_out.fp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.fp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_DOUBLE:
    switch(typ_in){
    case NC_FLOAT: *val_out.dp=*val_in.fp; break; 
    case NC_DOUBLE: *val_out.dp=*val_in.dp; break; 
    case NC_INT: *val_out.dp=*val_in.lp; break;
    case NC_SHORT: *val_out.dp=*val_in.sp; break;
    case NC_CHAR: *val_out.dp=strtod((const char *)val_in.cp,(char **)NULL); break;
    case NC_BYTE: *val_out.dp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_INT:
    switch(typ_in){
    case NC_FLOAT: *val_out.lp=(long)*val_in.fp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.lp=(long)*val_in.dp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.lp=*val_in.lp; break;
    case NC_SHORT: *val_out.lp=*val_in.sp; break;
    case NC_CHAR: *val_out.lp=(long)strtod((const char *)val_in.cp,(char **)NULL); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_BYTE: *val_out.lp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_SHORT:
    switch(typ_in){
    case NC_FLOAT: *val_out.sp=(short)*val_in.fp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.sp=(short)*val_in.dp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.sp=*val_in.lp; break;
    case NC_SHORT: *val_out.sp=*val_in.sp; break;
    case NC_CHAR: *val_out.sp=(short)strtod((const char *)val_in.cp,(char **)NULL); break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_BYTE: *val_out.sp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_CHAR:
    switch(typ_in){
    case NC_FLOAT: *val_out.cp=(unsigned char)*val_in.fp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.cp=(unsigned char)*val_in.dp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.cp=*val_in.lp; break;
    case NC_SHORT: *val_out.cp=*val_in.sp; break;
    case NC_CHAR: *val_out.cp=*val_in.cp; break;
    case NC_BYTE: *val_out.cp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  case NC_BYTE:
    switch(typ_in){
    case NC_FLOAT: *val_out.bp=(signed char)*val_in.fp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: *val_out.bp=(signed char)*val_in.dp; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: *val_out.bp=*val_in.lp; break;
    case NC_SHORT: *val_out.bp=*val_in.sp; break;
    case NC_CHAR: *val_out.bp=*val_in.cp; break;
    case NC_BYTE: *val_out.bp=*val_in.bp; break;
    default: nco_dfl_case_nctype_err(); break;
    } break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: There is no need to un-typecast input pointers because they were passed by
     value and are thus purely local to this routine. The only thing changed by this
     routine is the contents of the location pointed to by the pointer to the output value. */
  
} /* end val_conform_type */

int /* O [enm] Dummy return */
scv_conform_type /* [fnc] Convert scalar attribute to typ_new using C implicit coercion */
(const nc_type typ_new, /* I [enm] Type to convert scv_old to */
 scv_sct * const scv_old) /* I/O [sct] Scalar value to convert */
{
  /* Purpose: Convert scalar attribute to typ_new using C implicit coercion */
  nc_type typ_old=scv_old->type;
  
  scv_sct scv_new;

  switch (typ_new){ 
  case NC_BYTE:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.b=(signed char)(scv_old->val).f; break;
    case NC_DOUBLE: scv_new.val.b=(signed char)(scv_old->val).d; break;
    case NC_INT: scv_new.val.b=(scv_old->val).l; break;
    case NC_SHORT: scv_new.val.b=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.b=(scv_old->val).b; break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_SHORT:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.s=(short)(scv_old->val).f; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: scv_new.val.s=(short)(scv_old->val).d; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: scv_new.val.s=(scv_old->val).l; break;
    case NC_SHORT: scv_new.val.s=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.s=(scv_old->val).b; break;
    case NC_CHAR: break;
    case NC_NAT: break;    
    } break;
  case NC_INT:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.l=(long)(scv_old->val).f; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_DOUBLE: scv_new.val.l=(long)(scv_old->val).d; break; /* Coerce to avoid C++ compiler assignment warning */
    case NC_INT: scv_new.val.l =scv_old->val.l; break;
    case NC_SHORT: scv_new.val.l=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.l=(scv_old->val).b; break;
    case NC_CHAR: break;
    case NC_NAT: break;
    } break;
  case NC_FLOAT:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.f=(scv_old->val).f; break; 
    case NC_DOUBLE: scv_new.val.f=(scv_old->val).d; break; 
    case NC_INT: scv_new.val.f=(scv_old->val).l; break;
    case NC_SHORT: scv_new.val.f=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.f=(scv_old->val).b; break;
    case NC_CHAR: break;
    case NC_NAT: break;    
    } break;
  case NC_DOUBLE:
    switch(typ_old){
    case NC_FLOAT: scv_new.val.d=(scv_old->val).f; break; 
    case NC_DOUBLE: scv_new.val.d =(scv_old->val).d; break; 
    case NC_INT: scv_new.val.d=(scv_old->val).l; break;
    case NC_SHORT: scv_new.val.d=(scv_old->val).s; break;
    case NC_BYTE: scv_new.val.d=(scv_old->val).b; break;
    case NC_CHAR: break;
    case NC_NAT: break;    
    } break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  scv_new.type=typ_new;
  *scv_old=scv_new;
  return 1;      
} /* end scv_conform_type */

nc_type /* O [enm] Highest precision of input variables */
ncap_var_retype /* [fnc] Promote variable to higher common precision */
(var_sct *var_1, /* I/O [sct] Variable */
 var_sct *var_2) /* I/O [sct] Variable */
{
  /* Purpose: Convert variable, if necessary, so variables are of same type */
  if(var_1->type == var_2->type) return var_1->type;
  if(var_1->type > var_2->type){
    var_2=nco_var_cnf_typ(var_1->type,var_2);
    return var_1->type;
  }else{
    var_1=nco_var_cnf_typ(var_2->type,var_1);
    return var_2->type;
  } /* endif */
} /* end ncap_var_retype */

nc_type /* O [enm] Highest precision of arguments */
ncap_scv_scv_cnf_typ_hgh_prc /* [fnc] Promote arguments to higher precision if necessary */
(scv_sct * const scv_1, /* I/O [sct] Scalar value */
 scv_sct * const scv_2) /* I/O [sct] Scalar value */
{
  /* Purpose: Promote scalar values to higher of two precisions, if necessary */
  if(scv_1->type == scv_2->type){
    return scv_1->type;
  }else if(scv_1->type > scv_2->type){
    (void)scv_conform_type(scv_1->type,scv_2);
    return scv_1->type;
  }else{
    (void)scv_conform_type(scv_2->type,scv_1);
    return scv_2->type;
  } /* endif */
} /* end ncap_scv_scv_cnf_typ_hgh_prc */

nc_type /* O [enm] Highest precision of arguments */
ncap_var_scv_cnf_typ_hgh_prc /* [fnc] Promote arguments to higher precision if necessary */
(var_sct ** const var, /* I/O [sct] Variable */
 scv_sct * const scv) /* I/O [sct] Scalar value */
{
  /* Purpose: If types of variable and scalar value differ, convert argument with 
     lower precision to type of argument with higher precision.
     Otherwise do nothing. 
     fxm: Assumes nc_type increases monotonically with precision */

  /* Do nothing if types match */
  if((*var)->type == scv->type){
    return (*var)->type;
  }else if((*var)->type > scv->type){
    (void)scv_conform_type((*var)->type,scv); 
    return (*var)->type;
  }else{
    *var=nco_var_cnf_typ(scv->type,*var);
    return scv->type;
  } /* endif */

} /* end ncap_var_scv_cnf_typ_hgh_prc() */


