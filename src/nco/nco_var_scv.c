/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_scv.c,v 1.22 2005-11-26 05:43:26 zender Exp $ */

/* Purpose: Arithmetic between variables and scalar values */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_var_scv.h" /* Arithmetic between variables and scalar values */

void
var_scv_add /* [fnc] Add scalar to variable */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz,/* I [nbr] Size (in elements) of array operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1, /* I/O [val] Values of first operand */
 scv_sct *scv) /* I [val] Pointer to scalar value (second operand) */
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
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]+=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_FLOAT */
  case NC_DOUBLE:{
    const double scv_dbl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]+=scv_dbl;
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]+=scv_dbl;  
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_DOUBLE */
  case NC_INT:{
    const nco_int scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]+=scv_lng;
    }else{
      const nco_int mss_val_lng=*mss_val.lp; /* Temporary variable reduces de-referencing */
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
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces de-referencing */
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
var_scv_sub /* [fnc] Subtract scalar from variable */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of array operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1, /* I/O [val] Values of first operand */
 scv_sct *scv) /* I [val] Pointer to scalar value (second operand) */
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
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]-=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    const double scv_dbl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]-=scv_dbl;
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]-=scv_dbl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    const nco_int scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]-=scv_lng;
    }else{
      const nco_int mss_val_lng=*mss_val.lp; /* Temporary variable reduces de-referencing */
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
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces de-referencing */
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
var_scv_mlt /* [fnc] Multiply variable by scalar */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of array operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1, /* I/O [val] Values of first operand */
 scv_sct *scv) /* I [val] Pointer to scalar value (second operand) */
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
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]*=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    const double scv_dbl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]*=scv_dbl;
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]*=scv_dbl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    const nco_int scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]*=scv_lng;
    }else{
      const nco_int mss_val_lng=*mss_val.lp; /* Temporary variable reduces de-referencing */
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
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces de-referencing */
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
var_scv_dvd /* [fnc] Divide variable by scalar */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of array operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I/O [val] Values of first operand */
 scv_sct *scv) /* I [val] Pointer to scalar value (second operand) */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide all values in op1 by scv
     Store result in first operand */    
  
  /* Variable-scalar division is currently defined as op1:=op1/scv */  
  
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
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]/=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_FLOAT */
  case NC_DOUBLE:{
    const double scv_dbl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=scv_dbl;
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]/=scv_dbl;  
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_DOUBLE */
  case NC_INT:{
    const nco_int scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=scv_lng;
    }else{
      const nco_int mss_val_lng=*mss_val.lp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]/=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=scv_sht;
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht) op1.sp[idx]/=scv_sht;
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
  
} /* end var_scv_dvd() */

void
scv_var_dvd /* [fnc] Divide scalar by variable */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of array operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 scv_sct *scv, /* I [val] Pointer to scalar value (first operand) */
 ptr_unn op2) /* I/O [val] Values of second operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide all values in scv by op2
     Store result in second operand */
  
  /* Scalar-variable division is currently defined as op2:=scv/op2 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    const float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]=scv_flt/op2.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.fp[idx] != mss_val_flt) op2.fp[idx]=scv_flt/op2.fp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_FLOAT */
  case NC_DOUBLE:{
    const double scv_dbl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]=scv_dbl/op2.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.dp[idx] != mss_val_dbl) op2.dp[idx]=scv_dbl/op2.dp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_DOUBLE */
  case NC_INT:{
    const nco_int scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]=scv_lng/op2.lp[idx];
    }else{
      const nco_int mss_val_lng=*mss_val.lp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.lp[idx] != mss_val_lng) op2.lp[idx]=scv_lng/op2.lp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]=scv_sht/op2.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] != mss_val_sht) op2.sp[idx]=scv_sht/op2.sp[idx];
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
  
} /* end scv_var_dvd() */

void
var_scv_mod /* [fnc] Modulo variable by scalar */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of array operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I/O [val] Values of first operand */
 scv_sct *scv) /* I [val] Pointer to scalar value (second operand) */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Take modulus of all values in op1 by scv
     Store result in op1 */    
  
  /* Variable-scalar modulus is currently defined as op1:=op1%scv */  
  
  /* http://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
     __GNUC__ : Defined by gcc 
     __GNUG__ : Defined by g++, equivalent to (__GNUC__ && __cplusplus) */

#ifndef __GNUG__
  float fmodf(float,float); /* Cannot insert fmodf() in ncap_sym_init() because it takes two arguments TODO #20 */
  /* Sun cc math.h does not include fabsf() prototype 
     AIX xlc work fine with this prototype
     HP-UX cc fails when fabsf() is prototyped */
#ifndef HPUX
  float fabsf(float);
#endif /* HPUX */
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
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=fmodf(op1.fp[idx],scv_flt);
      } /* end for */
    } /* end else */
    break; 
  } /* endif NC_FLOAT */
  case NC_DOUBLE:{
    const double scv_dbl=fabs(scv->val.d);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=fmod(op1.dp[idx],scv_dbl);
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fmod(op1.dp[idx],scv_dbl);  
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_DOUBLE */
  case NC_INT:{
    const nco_int scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]%=scv_lng;
    }else{
      const nco_int mss_val_lng=*mss_val.lp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]%=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]%=scv_sht;
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht) op1.sp[idx]%=scv_sht;
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
  
} /* end var_scv_mod */

void
scv_var_mod /* [fnc] Modulo scalar by variable */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of array operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 scv_sct *scv, /* I [val] Pointer to scalar value (first operand) */
 ptr_unn op2) /* I/O [val] Values of second operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Take modulus of all values in scv by op2
     Store result in op2 */    
  
  /* Scalar-variable modulus is currently defined as op2:=scv%op2 */  
  
  /* http://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
     __GNUC__ : Defined by gcc 
     __GNUG__ : Defined by g++, equivalent to (__GNUC__ && __cplusplus) */

#ifndef __GNUG__
  float fmodf(float,float); /* Cannot insert fmodf() in ncap_sym_init() because it takes two arguments TODO #20 */
  /* Sun cc math.h does not include fabsf() prototype 
     AIX xlc work fine with this prototype
     HP-UX cc fails when fabsf() is prototyped */
#ifndef HPUX
  float fabsf(float);
#endif /* HPUX */
#endif /* __GNUG__ */

  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{ 
    const float scv_flt=fabsf(scv->val.f);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]=fmodf(scv_flt,op2.fp[idx]);
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.fp[idx] != mss_val_flt) op2.fp[idx]=fmodf(scv_flt,op2.fp[idx]);
      } /* end for */
    } /* end else */
    break; 
  } /* endif NC_FLOAT */
  case NC_DOUBLE:{
    const double scv_dbl=fabs(scv->val.d);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]=fmod(scv_dbl,op2.dp[idx]);
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.dp[idx] != mss_val_dbl) op2.dp[idx]=fmod(scv_dbl,op2.dp[idx]);  
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_DOUBLE */
  case NC_INT:{
    const nco_int scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]=scv_lng%op2.lp[idx];
    }else{
      const nco_int mss_val_lng=*mss_val.lp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.lp[idx] != mss_val_lng) op2.lp[idx]=scv_lng%op2.lp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const short scv_sht=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]=scv_sht%op2.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] != mss_val_sht) op2.sp[idx]=scv_sht%op2.sp[idx];
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
  
} /* end scv_var_mod */

void
var_scv_pwr /* [fnc] Empower variable by scalar */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of array operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I/O [val] Values of first operand */
 scv_sct *scv) /* I [val] Pointer to scalar value (second operand) */
{
  /* Purpose: Raise var to power in scv */

  /* Variable-scalar empowerment is currently defined as op1:=op1^scv */  

  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){ 
  case NC_FLOAT:{
    const float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=powf(op1.fp[idx],scv_flt);
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=powf(op1.fp[idx],scv_flt);
      } /* end for */
    } /* end else */
    break;
  } /* end NC_FLOAT */
  case NC_DOUBLE:{
    const double scv_dbl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=pow(op1.dp[idx],scv_dbl);
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=pow(op1.dp[idx],scv_dbl);
      } /* end for */
    } /* end else */
    break;
  } /* end NC_DOUBLE */
  case NC_INT:
    /* Do nothing */
    /* fxm: nco322 Implement integer empowerment? GSL? */
    break;
  case NC_SHORT:
    /* Do nothing */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_pwr */

void
scv_var_pwr /* [fnc] Empower scalar by variable */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of array operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 scv_sct *scv, /* I [val] Pointer to scalar value (first operand) */
 ptr_unn op2) /* I/O [val] Values of second operand */
{
  /* Purpose: Raise scv to power in var */

  /* Scalar-variable empowerment is currently defined as op2:=scv^op2 */  

  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){ 
  case NC_FLOAT:{
    const float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]=powf(scv_flt,op2.fp[idx]);
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op2.fp[idx] != mss_val_flt) op2.fp[idx]=powf(scv_flt,op2.fp[idx]);
      } /* end for */
    } /* end else */
    break;
  } /* end NC_FLOAT */
  case NC_DOUBLE:{
    const double scv_dbl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]=pow(scv_dbl,op2.dp[idx]);
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op2.dp[idx] != mss_val_dbl) op2.dp[idx]=pow(scv_dbl,op2.dp[idx]);
      } /* end for */
    } /* end else */
    break;
  } /* end NC_DOUBLE */
  case NC_INT:
    /* Do nothing */
    /* fxm: nco322 Implement integer empowerment? GSL? */
    break;
  case NC_SHORT:
    /* Do nothing */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_pwr */
