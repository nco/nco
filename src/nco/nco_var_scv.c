/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_scv.c,v 1.11 2002-12-30 02:56:15 zender Exp $ */

/* Purpose: Arithmetic between variables and scalar values */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_var_scv.h" /* Arithmetic between variables and scalar values */

void
var_scv_add(nc_type type,const long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /*	nc_type type: I netCDF type of operands
	const long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        scv_sct scv: I pointer to scalar value (second operand) */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Add scv to all values in op1
     Store result in first operand */    
  
  /* Addition is currently defined as op1:=op1+scv */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    const float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]+=scv_flt;
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]+=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_FLOAT */
  case NC_DOUBLE:{
    const double scv_dpl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]+=scv_dpl;
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]+=scv_dpl;  
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_DOUBLE */
  case NC_INT:{
    const nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]+=scv_lng;
    }else{
      const long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]+=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]+=scv_sht;
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht) op1.sp[idx]+=scv_sht;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_SHORT */
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_add() */

void
var_scv_sub(nc_type type,const long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /*	nc_type type: I netCDF type of operands
	const long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        scv_sct scv: I pointer to scalar value (second operand) */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Subtract scv from all values in op1
     Store result in first operand */    
  
  /* Subtraction is currently defined as op1:=op1-scv */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    const float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]-=scv_flt;
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]-=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    const double scv_dpl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]-=scv_dpl;
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]-=scv_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    const nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]-=scv_lng;
    }else{
      const long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]-=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]-=scv_sht;
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht) op1.sp[idx]-=scv_sht;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_sub() */

void
var_scv_mlt(nc_type type,const long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /* 
	nc_type type: I netCDF type of operands
	const long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        scv_sct scv: I pointer to scalar value (second operand) */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Multiply all values in op1 by scv
     Store result in first operand */    
  
  /* Multiplication is currently defined as op1:=op1*scv */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    const float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]*=scv_flt;
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]*=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    const double scv_dpl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]*=scv_dpl;
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]*=scv_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    const nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]*=scv_lng;
    }else{
      const long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]*=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]*=scv_sht;
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht) op1.sp[idx]*=scv_sht;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_mlt() */

void
var_scv_dvd(nc_type type,const long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /* 
	nc_type type: I netCDF type of operands
	const long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        scv_sct scv: I pointer to scalar value (second operand) */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide all values in op1 by scv
     Store result in first operand */    
  
  /* Division is currently defined as op1:=op1/scv */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    const float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=scv_flt;
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]/=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    const double scv_dpl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=scv_dpl;
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]/=scv_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    const nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=scv_lng;
    }else{
      const long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]/=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=scv_sht;
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht) op1.sp[idx]/=scv_sht;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_dvd() */

void 
var_scv_mod(nc_type type,const long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /* 
	nc_type type: I netCDF type of operands
	const long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        scv_sct scv: I pointer to scalar value (second operand) */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Take modulus of  all values in op1 by value in attrib
     Store result in op1  */    
  
  /* Modulus is currently defined as op1:=op1%scv */  
  
#ifndef __GNUG__
  float fmodf(float,float); /* Cannot insert fmodf in ncap_sym_init() because it takes two arguments TODO #20 */
  float fabsf(float); /* Sun math.h does not include fabsf() prototype */
#endif /* __GNUG__ */

  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{ 
    const float scv_flt=fabsf(scv->val.f);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=fmodf(op1.fp[idx],scv_flt);
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=fmodf(op1.fp[idx],scv_flt);
      } /* end for */
    } /* end else */
  }
  break; 
  case NC_DOUBLE:{
    const double scv_dpl=fabs(scv->val.d);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=fmod(op1.dp[idx],scv_dpl);
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fmod(op1.dp[idx],scv_dpl);  
      } /* end for */
    } /* end else */
    break;
  }
  
  case NC_INT:{
    const nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]%=scv_lng;
    }else{
      const long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]%=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]%=scv_sht;
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht) op1.sp[idx]%=scv_sht;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
} /* end var_scv_mod */
