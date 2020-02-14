/* $Header$ */

/* Purpose: Arithmetic between variables and scalar values */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_var_scv.h" /* Arithmetic between variables and scalar values */

void
nco_var_scv_add /* [fnc] Add scalar to variable */
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
    const nco_int scv_ntg=scv->val.i;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]+=scv_ntg;
    }else{
      const nco_int mss_val_ntg=*mss_val.ip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg) op1.ip[idx]+=scv_ntg; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const nco_short scv_short=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]+=scv_short;
    }else{
      const nco_short mss_val_short=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short) op1.sp[idx]+=scv_short;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_SHORT */
  case NC_USHORT:{
    const nco_ushort scv_ushort=scv->val.us; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]+=scv_ushort;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort) op1.usp[idx]+=scv_ushort;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_USHORT */
  case NC_UINT:{
    const nco_uint scv_uint=scv->val.ui; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]+=scv_uint;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint) op1.uip[idx]+=scv_uint;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT */
  case NC_INT64:{
    const nco_int64 scv_int64=scv->val.i64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]+=scv_int64;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]+=scv_int64;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT64 */
  case NC_UINT64:{
    const nco_uint64 scv_uint64=scv->val.ui64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]+=scv_uint64;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64) op1.ui64p[idx]+=scv_uint64;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT64 */
  case NC_BYTE:{
    const nco_byte scv_byte=scv->val.b; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.bp[idx]+=scv_byte;
    }else{
      const nco_byte mss_val_byte=*mss_val.bp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.bp[idx] != mss_val_byte) op1.bp[idx]+=scv_byte;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_BYTE */
  case NC_UBYTE:{
    const nco_ubyte scv_ubyte=scv->val.ub; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ubp[idx]+=scv_ubyte;
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ubp[idx] != mss_val_ubyte) op1.ubp[idx]+=scv_ubyte;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UBYTE */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_scv_add() */

void
nco_var_scv_sub /* [fnc] Subtract scalar from variable */
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
    const nco_int scv_ntg=scv->val.i;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]-=scv_ntg;
    }else{
      const nco_int mss_val_ntg=*mss_val.ip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg) op1.ip[idx]-=scv_ntg; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    const nco_short scv_short=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]-=scv_short;
    }else{
      const nco_short mss_val_short=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short) op1.sp[idx]-=scv_short;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_USHORT:{
    const nco_ushort scv_ushort=scv->val.us; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]-=scv_ushort;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort) op1.usp[idx]-=scv_ushort;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_UINT:{
    const nco_uint scv_uint=scv->val.ui; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]-=scv_uint;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint) op1.uip[idx]-=scv_uint;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT64:{
    const nco_int64 scv_int64=scv->val.i64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]-=scv_int64;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]-=scv_int64;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_UINT64:{
    const nco_uint64 scv_uint64=scv->val.ui64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]-=scv_uint64;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64) op1.ui64p[idx]-=scv_uint64;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_BYTE:{
    const nco_byte scv_byte=scv->val.b; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.bp[idx]-=scv_byte;
    }else{
      const nco_byte mss_val_byte=*mss_val.bp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.bp[idx] != mss_val_byte) op1.bp[idx]-=scv_byte;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_UBYTE:{
    const nco_ubyte scv_ubyte=scv->val.ub; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ubp[idx]-=scv_ubyte;
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ubp[idx] != mss_val_ubyte) op1.ubp[idx]-=scv_ubyte;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_scv_sub() */

void
nco_var_scv_mlt /* [fnc] Multiply variable by scalar */
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
    const nco_int scv_ntg=scv->val.i;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]*=scv_ntg;
    }else{
      const nco_int mss_val_ntg=*mss_val.ip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg) op1.ip[idx]*=scv_ntg; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    const nco_short scv_short=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]*=scv_short;
    }else{
      const nco_short mss_val_short=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short) op1.sp[idx]*=scv_short;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_USHORT:{
    const nco_ushort scv_ushort=scv->val.us; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]*=scv_ushort;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort) op1.usp[idx]*=scv_ushort;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_UINT:{
    const nco_uint scv_uint=scv->val.ui; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]*=scv_uint;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint) op1.uip[idx]*=scv_uint;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT64:{
    const nco_int64 scv_int64=scv->val.i64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]*=scv_int64;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]*=scv_int64;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_UINT64:{
    const nco_uint64 scv_uint64=scv->val.ui64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]*=scv_uint64;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64) op1.ui64p[idx]*=scv_uint64;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_BYTE:{
    const nco_byte scv_byte=scv->val.b; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.bp[idx]*=scv_byte;
    }else{
      const nco_byte mss_val_byte=*mss_val.bp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.bp[idx] != mss_val_byte) op1.bp[idx]*=scv_byte;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_UBYTE:{
    const nco_ubyte scv_ubyte=scv->val.ub; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ubp[idx]*=scv_ubyte;
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ubp[idx] != mss_val_ubyte) op1.ubp[idx]*=scv_ubyte;
      } /* end for */
    } /* end else */
    break;
  }
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_scv_mlt() */

void
nco_var_scv_dvd /* [fnc] Divide variable by scalar */
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
    const nco_int scv_ntg=scv->val.i;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]/=scv_ntg;
    }else{
      const nco_int mss_val_ntg=*mss_val.ip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg) op1.ip[idx]/=scv_ntg; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const nco_short scv_short=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=scv_short;
    }else{
      const nco_short mss_val_short=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short) op1.sp[idx]/=scv_short;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT64 */
  case NC_USHORT:{
    const nco_ushort scv_ushort=scv->val.us; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]/=scv_ushort;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort) op1.usp[idx]/=scv_ushort;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_USHORT */
  case NC_UINT:{
    const nco_uint scv_uint=scv->val.ui; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]/=scv_uint;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint) op1.uip[idx]/=scv_uint;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT */
  case NC_INT64:{
    const nco_int64 scv_int64=scv->val.i64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]/=scv_int64;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]/=scv_int64;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT64 */
  case NC_UINT64:{
    const nco_uint64 scv_uint64=scv->val.ui64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]/=scv_uint64;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64) op1.ui64p[idx]/=scv_uint64;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT64 */
  case NC_BYTE:{
    const nco_byte scv_byte=scv->val.b; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.bp[idx]/=scv_byte;
    }else{
      const nco_byte mss_val_byte=*mss_val.bp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.bp[idx] != mss_val_byte) op1.bp[idx]/=scv_byte;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_BYTE */
  case NC_UBYTE:{
    const nco_ubyte scv_ubyte=scv->val.ub; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ubp[idx]/=scv_ubyte;
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ubp[idx] != mss_val_ubyte) op1.ubp[idx]/=scv_ubyte;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UBYTE */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_scv_dvd() */

void
nco_scv_var_dvd /* [fnc] Divide scalar by variable */
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
    const nco_int scv_ntg=scv->val.i;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]=scv_ntg/op2.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.ip[idx] != mss_val_ntg) op2.ip[idx]=scv_ntg/op2.ip[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const nco_short scv_short=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]=scv_short/op2.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] != mss_val_short) op2.sp[idx]=scv_short/op2.sp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_USHORT */
  case NC_USHORT:{
    const nco_ushort scv_ushort=scv->val.us; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]=scv_ushort/op2.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.usp[idx] != mss_val_ushort) op2.usp[idx]=scv_ushort/op2.usp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_USHORT */
  case NC_UINT:{
    const nco_uint scv_uint=scv->val.ui; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]=scv_uint/op2.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.uip[idx] != mss_val_uint) op2.uip[idx]=scv_uint/op2.uip[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT */
  case NC_INT64:{
    const nco_int64 scv_int64=scv->val.i64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]=scv_int64/op2.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.i64p[idx] != mss_val_int64) op2.i64p[idx]=scv_int64/op2.i64p[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT64 */
  case NC_UINT64:{
    const nco_uint64 scv_uint64=scv->val.ui64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]=scv_uint64/op2.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.ui64p[idx] != mss_val_uint64) op2.ui64p[idx]=scv_uint64/op2.ui64p[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT64 */
  case NC_BYTE:{
    const nco_byte scv_byte=scv->val.b; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.bp[idx]=scv_byte/op2.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.bp[idx] != mss_val_byte) op2.bp[idx]=scv_byte/op2.bp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_BYTE */
  case NC_UBYTE:{
    const nco_ubyte scv_ubyte=scv->val.ub; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ubp[idx]=scv_ubyte/op2.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.ubp[idx] != mss_val_ubyte) op2.ubp[idx]=scv_ubyte/op2.ubp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UBYTE */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_scv_var_dvd() */

void
nco_var_scv_mod /* [fnc] Modulo variable by scalar */
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
    const nco_int scv_ntg=scv->val.i;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]%=scv_ntg;
    }else{
      const nco_int mss_val_ntg=*mss_val.ip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg) op1.ip[idx]%=scv_ntg; 
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const nco_short scv_short=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]%=scv_short;
    }else{
      const nco_short mss_val_short=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short) op1.sp[idx]%=scv_short;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_SHORT */
  case NC_USHORT:{
    const nco_ushort scv_ushort=scv->val.us; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]%=scv_ushort;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort) op1.usp[idx]%=scv_ushort;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_USHORT */
  case NC_UINT:{
    const nco_uint scv_uint=scv->val.ui; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]%=scv_uint;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint) op1.uip[idx]%=scv_uint;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT */
  case NC_INT64:{
    const nco_int64 scv_int64=scv->val.i64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]%=scv_int64;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]%=scv_int64;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT64 */
  case NC_UINT64:{
    const nco_uint64 scv_uint64=scv->val.ui64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]%=scv_uint64;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64) op1.ui64p[idx]%=scv_uint64;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT64 */
  case NC_BYTE:{
    const nco_byte scv_byte=scv->val.b; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.bp[idx]%=scv_byte;
    }else{
      const nco_byte mss_val_byte=*mss_val.bp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.bp[idx] != mss_val_byte) op1.bp[idx]%=scv_byte;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_BYTE */
  case NC_UBYTE:{
    const nco_ubyte scv_ubyte=scv->val.ub; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ubp[idx]%=scv_ubyte;
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op1.ubp[idx] != mss_val_ubyte) op1.ubp[idx]%=scv_ubyte;
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UBYTE */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
} /* end nco_var_scv_mod */

void
nco_scv_var_mod /* [fnc] Modulo scalar by variable */
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
    const nco_int scv_ntg=scv->val.i;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]=scv_ntg%op2.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.ip[idx] != mss_val_ntg) op2.ip[idx]=scv_ntg%op2.ip[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT */
  case NC_SHORT:{
    const nco_short scv_short=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]=scv_short%op2.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] != mss_val_short) op2.sp[idx]=scv_short%op2.sp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_SHORT */
  case NC_USHORT:{
    const nco_ushort scv_ushort=scv->val.us; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]=scv_ushort%op2.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.usp[idx] != mss_val_ushort) op2.usp[idx]=scv_ushort%op2.usp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_USHORT */
  case NC_UINT:{
    const nco_uint scv_uint=scv->val.ui; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]=scv_uint%op2.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.uip[idx] != mss_val_uint) op2.uip[idx]=scv_uint%op2.uip[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT */
  case NC_INT64:{
    const nco_int64 scv_int64=scv->val.i64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]=scv_int64%op2.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.i64p[idx] != mss_val_int64) op2.i64p[idx]=scv_int64%op2.i64p[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_INT64 */
  case NC_UINT64:{
    const nco_uint64 scv_uint64=scv->val.ui64; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]=scv_uint64%op2.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.ui64p[idx] != mss_val_uint64) op2.ui64p[idx]=scv_uint64%op2.ui64p[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UINT64 */
  case NC_BYTE:{
    const nco_byte scv_byte=scv->val.b; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.bp[idx]=scv_byte%op2.bp[idx];
    }else{
      const nco_byte mss_val_byte=*mss_val.bp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.bp[idx] != mss_val_byte) op2.bp[idx]=scv_byte%op2.bp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_BYTE */
  case NC_UBYTE:{
    const nco_ubyte scv_ubyte=scv->val.ub; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ubp[idx]=scv_ubyte%op2.ubp[idx];
    }else{
      const nco_ubyte mss_val_ubyte=*mss_val.ubp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
	if(op2.ubp[idx] != mss_val_ubyte) op2.ubp[idx]=scv_ubyte%op2.ubp[idx];
      } /* end for */
    } /* end else */
    break;
  } /* endif NC_UBYTE */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
} /* end nco_scv_var_mod */

void
nco_var_scv_pwr /* [fnc] Empower variable by scalar */
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
    /* fxm: nco322 Implement integer empowerment? GSL? */
  case NC_INT: break; /* Do nothing */
  case NC_SHORT: break; /* Do nothing */
  case NC_USHORT: break; /* Do nothing */
  case NC_UINT: break; /* Do nothing */
  case NC_INT64: break; /* Do nothing */
  case NC_UINT64: break; /* Do nothing */
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_scv_pwr */

void
nco_scv_var_pwr /* [fnc] Empower scalar by variable */
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
    /* fxm: nco322 Implement integer empowerment? GSL? */
  case NC_INT: break; /* Do nothing */
  case NC_SHORT: break; /* Do nothing */
  case NC_USHORT: break; /* Do nothing */
  case NC_UINT: break; /* Do nothing */
  case NC_INT64: break; /* Do nothing */
  case NC_UINT64: break; /* Do nothing */
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_scv_pwr */
