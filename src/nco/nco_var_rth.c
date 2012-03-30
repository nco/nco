/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.c,v 1.60 2012-03-30 17:48:40 zender Exp $ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "nco_var_rth.h" /* Variable arithmetic */

void
nco_var_abs /* [fnc] Replace op1 values by their absolute values */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 ptr_unn op1) /* I [val] Values of first operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Replace op1 values by their absolute values */
  
  /* Absolute value is currently defined as op1:=abs(op1) */  
  
  /* NB: Many compilers need to #include "nco_rth_flt.h" for fabsf() prototype */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=fabsf(op1.fp[idx]);
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=fabsf(op1.fp[idx]); 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=fabs(op1.dp[idx]);
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fabs(op1.dp[idx]);
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]=labs(op1.ip[idx]); /* int abs(int), long labs(long) */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg) op1.ip[idx]=labs(op1.ip[idx]); /* int abs(int), long labs(long) */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) if(op1.sp[idx] < 0 ) op1.sp[idx]=-op1.sp[idx] ;
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short && op1.sp[idx] < 0 ) op1.sp[idx]=-op1.sp[idx];
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing fxm: do something? */
  case NC_UBYTE: break; /* Do nothing */
  case NC_USHORT: break; /* Do nothing */
  case NC_UINT: break; /* Do nothing */
  case NC_INT64: 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]=llabs(op1.i64p[idx]);
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64) op1.i64p[idx]=llabs(op1.i64p[idx]);
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_abs() */

void
nco_var_add /* [fnc] Add first operand to second operand */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of sum on output */
{
  /* Purpose: Add value of first operand to value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() does _not_ increment tally counter
     nco_var_add_tll_ncra() does increment tally counter */
  
  /* Addition is currently defined as op2:=op1+op2 where op1 != mss_val and op2 != mss_val */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]+=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]+=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]+=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]+=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]+=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]+=op1.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]+=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]+=op1.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]+=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]+=op1.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]+=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]+=op1.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]+=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]+=op1.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]+=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]+=op1.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_add() */

void
nco_var_add_tll_ncflint /* [fnc] Add first operand to second operand, increment tally */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 long * restrict const tally, /* I/O [nbr] Counter space */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of sum on output */
{
  /* Purpose: Add value of first operand to value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() does _not_ increment tally counter
     nco_var_add_tll_ncflint() does increment tally counter */
  
  /* Addition is currently defined as op2:=op1+op2 where op1 != mss_val and op2 != mss_val */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  /* Return missing_value where either or both input values are missing 
     Algorithm used since 20040603 
     NB: Tally is incremented but not used */
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]+=op1.fp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)){
	  op2.fp[idx]+=op1.fp[idx];
	  tally[idx]++;
	}else{
	  op2.fp[idx]=mss_val_flt;
	}/* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]+=op1.dp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)){
	  op2.dp[idx]+=op1.dp[idx];
	  tally[idx]++;
	}else{
	  op2.dp[idx]=mss_val_dbl;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ip[idx]+=op1.ip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)){
	  op2.ip[idx]+=op1.ip[idx];
	  tally[idx]++;
	}else{
	  op2.ip[idx]=mss_val_ntg;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]+=op1.sp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	}else{
	  op2.sp[idx]=mss_val_short;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.usp[idx]+=op1.usp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)){
	  op2.usp[idx]+=op1.usp[idx];
	  tally[idx]++;
	}else{
	  op2.usp[idx]=mss_val_ushort;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.uip[idx]+=op1.uip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)){
	  op2.uip[idx]+=op1.uip[idx];
	  tally[idx]++;
	}else{
	  op2.uip[idx]=mss_val_uint;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.i64p[idx]+=op1.i64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)){
	  op2.i64p[idx]+=op1.i64p[idx];
	  tally[idx]++;
	}else{
	  op2.i64p[idx]=mss_val_int64;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ui64p[idx]+=op1.ui64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)){
	  op2.ui64p[idx]+=op1.ui64p[idx];
	  tally[idx]++;
	}else{
	  op2.ui64p[idx]=mss_val_uint64;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* Used this block of code until 20040603. 
     It keeps track of tally but does not do anything with it later */
#if 0
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]+=op1.fp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)){
	  op2.fp[idx]+=op1.fp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]+=op1.dp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)){
	  op2.dp[idx]+=op1.dp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ip[idx]+=op1.ip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)){
	  op2.ip[idx]+=op1.ip[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]+=op1.sp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.usp[idx]+=op1.usp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)){
	  op2.usp[idx]+=op1.usp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.uip[idx]+=op1.uip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)){
	  op2.uip[idx]+=op1.uip[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.i64p[idx]+=op1.i64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)){
	  op2.i64p[idx]+=op1.i64p[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ui64p[idx]+=op1.ui64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)){
	  op2.ui64p[idx]+=op1.ui64p[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
#endif /* endif 0 */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_add_tll_ncflint() */

void
nco_var_add_tll_ncra /* [fnc] Add first operand to second operand, increment tally */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 long * restrict const tally, /* I/O [nbr] Counter space */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand (running sum) on input, values of new sum on output */
{
  /* Purpose: Add value of first operand to value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() does _not_ increment tally counter.
     nco_var_add_tll_ncflint() adds if neither operand equals missing value
     nco_var_add_tll_ncflint() does increment tally counter (unlike nco_var_add())
     nco_var_add_tll_ncra() adds if op1 does not equal missing value
     nco_var_add_tll_ncra() does increment tally counter (like nco_var_add_tll_ncflint())
     nco_var_add_tll_ncra() is designed to:
     1. Work for "running average" algorithms only
     2. Assume running sum is valid and is stored in op2
     3. Assume new record is stored in op1
     4. Check only if new record (not running sum) equals missing_value
     Note that missing_value is associated with op1, i.e., new record, not running sum
     5. Accumulate running sum only if new record is valid
     6. Increment tally
     Difference between nco_var_add_tll_ncra() and nco_var_add_tll_ncflint() is that
     nco_var_add_tll_ncflint() checks both operands against the missing_value, whereas 
     nco_var_add_tll_ncra() only checks first operand (new record) against missing_value
     nco_var_add_tll_ncflint() algorithm fails as running average algorithm when
     missing value is zero because running sum is bootstrapped to zero and this
     causes comparison to missing_value to always be true.
     nco_var_add_tll_ncflint() also fails as running average algorithm whenever
     running sum happens to equal missing_value (regardless if missing value is zero).
     NCO uses nco_var_add_tll_ncflint() only for ncflint
     NCO uses nco_var_add_tll_ncra() only for ncra/ncea */
  
  /* Addition is currently defined as op2:=op1+op2 where op1 != mss_val */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]+=op1.fp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt){
	  op2.fp[idx]+=op1.fp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]+=op1.dp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl){
	  op2.dp[idx]+=op1.dp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ip[idx]+=op1.ip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg){
	  op2.ip[idx]+=op1.ip[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]+=op1.sp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.usp[idx]+=op1.usp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort){
	  op2.usp[idx]+=op1.usp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.uip[idx]+=op1.uip[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint){
	  op2.uip[idx]+=op1.uip[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.i64p[idx]+=op1.i64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64){
	  op2.i64p[idx]+=op1.i64p[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ui64p[idx]+=op1.ui64p[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64){
	  op2.ui64p[idx]+=op1.ui64p[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_add_tll_ncra() */

void 
nco_var_copy_tll /* [fnc] Copy hyperslab variables of type var_typ from op1 to op2 */
(const nc_type type, /* I [enm] netCDF type */
 const long sz, /* I [nbr] Number of elements to copy */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 long * restrict const tally, /* O [nbr] Counter space */
 const ptr_unn op1, /* I [sct] Values to copy */
 ptr_unn op2) /* O [sct] Destination to copy values to */
{
  /* Purpose: Copy hyperslab variables of type var_typ from op1 to op2
     Assumes memory area in op2 has already been malloc()'d
     Where the value copied is not equal to the missing value, set the tally to one
     nco_var_copy(): Does nothing with missing values and tallies
     nco_var_copy_tll(): Accounts for missing values in tally */

  /* Algorithm is currently defined as: op2:=op1 */

  long idx;

  /* Use fast nco_var_copy() method to copy variable */
  (void)memcpy((void *)(op2.vp),(void *)(op1.vp),sz*nco_typ_lng(type));

  /* Tally is one if no missing value is defined */
  (void)nco_set_long(sz,1L,tally);

  if(!has_mss_val) return;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  /* Overwrite one's with zero's where value equals missing value */
  switch(type){
  case NC_FLOAT:
    {
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if(op2.fp[idx] == mss_val_flt) tally[idx]=0L;
    }
    break;
  case NC_DOUBLE:
    {
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if(op2.dp[idx] == mss_val_dbl) tally[idx]=0L;
    }
    break;
  case NC_INT:
    {
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++) if(op2.ip[idx] == mss_val_ntg) tally[idx]=0L;
    }
    break;
  case NC_SHORT:
    {
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(op2.sp[idx] == mss_val_short) tally[idx]=0L;
    }
    break;
  case NC_USHORT:
    {
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++) if(op2.usp[idx] == mss_val_ushort) tally[idx]=0L;
    }
    break;
  case NC_UINT:
    {
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++) if(op2.uip[idx] == mss_val_uint) tally[idx]=0L;
    }
    break;
  case NC_INT64:
    {
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++) if(op2.i64p[idx] == mss_val_int64) tally[idx]=0L;
    }
    break;
  case NC_UINT64:
    {
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] == mss_val_uint64) tally[idx]=0L;
    }
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end nco_var_copy_tll() */

void
nco_var_dvd /* [fnc] Divide second operand by first operand */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of denominator */
 ptr_unn op2) /* I/O [val] Values of numerator on input, values of quotient on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide value of first operand by value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Variable-variable division is currently defined as op2:=op2/op1 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]/=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]/=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break; /* end NC_FLOAT */
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]/=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]/=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break; /* end NC_DOUBLE */
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]/=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]/=op1.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break; /* end NC_INT */
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]/=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]/=op1.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break; /* end NC_SHORT */
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]/=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]/=op1.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break; /* end NC_USHORT */
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]/=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]/=op1.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break; /* end NC_UINT */
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]/=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]/=op1.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break; /* end NC_INT64 */
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]/=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]/=op1.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break; /* end NC_UINT64 */
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_dvd() */

void
nco_var_max_bnr /* [fnc] Maximize two operands */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of maximum on output */
{
  /* Purpose: Find minimium value(s) of two operands and store result in second operand 
     Operands are assumed to conform, be of same specified type, and have values in memory */
  long idx;
  
  /* Typecast pointer to values before access */
  /* It is not necessary to untype-cast pointer types after using them as we have 
     operated on local copies of them */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.fp[idx] < op1.fp[idx]) op2.fp[idx]=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op2.fp[idx] == mss_val_flt) 
	  op2.fp[idx]=op1.fp[idx];
	else if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] < op1.fp[idx]))
	  op2.fp[idx]=op1.fp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.dp[idx] < op1.dp[idx]) op2.dp[idx]=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op2.dp[idx] == mss_val_dbl) 
	  op2.dp[idx]=op1.dp[idx];
	else if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] < op1.dp[idx]))
	  op2.dp[idx]=op1.dp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ip[idx] < op1.ip[idx]) 
	  op2.ip[idx]=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op2.ip[idx] == mss_val_ntg) 
	  op2.ip[idx]=op1.ip[idx];
	else if((op1.ip[idx] != mss_val_ntg) && (op2.ip[idx] < op1.ip[idx]))
	  op2.ip[idx]=op1.ip[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.sp[idx] < op1.sp[idx])
	  op2.sp[idx]=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] == mss_val_short) 
	  op2.sp[idx]=op1.sp[idx];
	else if((op1.sp[idx] != mss_val_short) && (op2.sp[idx] < op1.sp[idx]))
	  op2.sp[idx]=op1.sp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.usp[idx] < op1.usp[idx])
	  op2.usp[idx]=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if(op2.usp[idx] == mss_val_ushort) 
	  op2.usp[idx]=op1.usp[idx];
	else if((op1.usp[idx] != mss_val_ushort) && (op2.usp[idx] < op1.usp[idx]))
	  op2.usp[idx]=op1.usp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.uip[idx] < op1.uip[idx])
	  op2.uip[idx]=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if(op2.uip[idx] == mss_val_uint) 
	  op2.uip[idx]=op1.uip[idx];
	else if((op1.uip[idx] != mss_val_uint) && (op2.uip[idx] < op1.uip[idx]))
	  op2.uip[idx]=op1.uip[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.i64p[idx] < op1.i64p[idx])
	  op2.i64p[idx]=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op2.i64p[idx] == mss_val_int64) 
	  op2.i64p[idx]=op1.i64p[idx];
	else if((op1.i64p[idx] != mss_val_int64) && (op2.i64p[idx] < op1.i64p[idx]))
	  op2.i64p[idx]=op1.i64p[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ui64p[idx] < op1.ui64p[idx])
	  op2.ui64p[idx]=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if(op2.ui64p[idx] == mss_val_uint64) 
	  op2.ui64p[idx]=op1.ui64p[idx];
	else if((op1.ui64p[idx] != mss_val_uint64) && (op2.ui64p[idx] < op1.ui64p[idx]))
	  op2.ui64p[idx]=op1.ui64p[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
} /* end nco_var_max_bnr() */

void
nco_var_min_bnr /* [fnc] Minimize two operands */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of minimum on output */
{
  /* Purpose: Find minimium value(s) of two operands and store result in second operand 
     Operands are assumed to conform, be of same specified type, and have values in memory */
  long idx;
  
  /* Typecast pointer to values before access */
  /* It is not necessary to uncast pointer types after using them as we have 
     operated on local copies of them */
  
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.fp[idx] > op1.fp[idx]) op2.fp[idx]=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op2.fp[idx] == mss_val_flt) 
	  op2.fp[idx]=op1.fp[idx];
	else if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] > op1.fp[idx]))
	  op2.fp[idx]=op1.fp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.dp[idx] > op1.dp[idx]) op2.dp[idx]=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op2.dp[idx] == mss_val_dbl) 
	  op2.dp[idx]=op1.dp[idx];
	else if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] > op1.dp[idx]))
	  op2.dp[idx]=op1.dp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ip[idx] > op1.ip[idx]) 
	  op2.ip[idx]=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op2.ip[idx] == mss_val_ntg) 
	  op2.ip[idx]=op1.ip[idx];
	else if((op1.ip[idx] != mss_val_ntg) && (op2.ip[idx] > op1.ip[idx]))
	  op2.ip[idx]=op1.ip[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.sp[idx] > op1.sp[idx])  op2.sp[idx]=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] == mss_val_short) 
	  op2.sp[idx]=op1.sp[idx];
	else if((op1.sp[idx] != mss_val_short) && (op2.sp[idx] > op1.sp[idx]))
	  op2.sp[idx]=op1.sp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.usp[idx] > op1.usp[idx])  op2.usp[idx]=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if(op2.usp[idx] == mss_val_ushort) 
	  op2.usp[idx]=op1.usp[idx];
	else if((op1.usp[idx] != mss_val_ushort) && (op2.usp[idx] > op1.usp[idx]))
	  op2.usp[idx]=op1.usp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.uip[idx] > op1.uip[idx])  op2.uip[idx]=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if(op2.uip[idx] == mss_val_uint) 
	  op2.uip[idx]=op1.uip[idx];
	else if((op1.uip[idx] != mss_val_uint) && (op2.uip[idx] > op1.uip[idx]))
	  op2.uip[idx]=op1.uip[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.i64p[idx] > op1.i64p[idx])  op2.i64p[idx]=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op2.i64p[idx] == mss_val_int64) 
	  op2.i64p[idx]=op1.i64p[idx];
	else if((op1.i64p[idx] != mss_val_int64) && (op2.i64p[idx] > op1.i64p[idx]))
	  op2.i64p[idx]=op1.i64p[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.ui64p[idx] > op1.ui64p[idx])  op2.ui64p[idx]=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if(op2.ui64p[idx] == mss_val_uint64) 
	  op2.ui64p[idx]=op1.ui64p[idx];
	else if((op1.ui64p[idx] != mss_val_uint64) && (op2.ui64p[idx] > op1.ui64p[idx]))
	  op2.ui64p[idx]=op1.ui64p[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
} /* end nco_var_min_bnr() */

void
nco_var_mlt /* [fnc] Multiply first operand by second operand */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of product on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: multiply value of first operand by value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Multiplication is currently defined as op2:=op1*op2 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]*=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]*=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]*=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]*=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]*=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]*=op1.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]*=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]*=op1.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]*=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]*=op1.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]*=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]*=op1.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]*=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]*=op1.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]*=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]*=op1.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_mlt() */

void
nco_var_mod /* [fnc] Remainder (modulo) operation of two variables */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of field */
 ptr_unn op2) /* I/O [val] Values of divisor on input, values of remainder on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide value of first operand by value of second operand 
     and store remainder in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Remainder (modulo) operation is currently defined as op2:=op1%op2 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT: /* Hand-code modulo operator for floating point arguments (intrinsic % requires integer arguments) */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]=op1.fp[idx]-op2.fp[idx]*(int)(op1.fp[idx]/op2.fp[idx]);
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]=op1.fp[idx]-op2.fp[idx]*(int)(op1.fp[idx]/op2.fp[idx]); else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break; /* end NC_FLOAT */
  case NC_DOUBLE: /* Hand-code modulo operator for floating point arguments (intrinsic % requires integer arguments) */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]=op1.dp[idx]-op2.dp[idx]*(int)(op1.dp[idx]/op2.dp[idx]);
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]=op1.dp[idx]-op2.dp[idx]*(int)(op1.dp[idx]/op2.dp[idx]); else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break; /* end NC_DOUBLE */
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]=op1.ip[idx]%op2.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]=op1.ip[idx]%op2.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break; /* end NC_INT */
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]=op1.sp[idx]%op2.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]=op1.sp[idx]%op2.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break; /* end NC_SHORT */
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]=op1.usp[idx]%op2.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]=op1.usp[idx]%op2.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break; /* end NC_USHORT */
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]=op1.uip[idx]%op2.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]=op1.uip[idx]%op2.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break; /* end NC_UINT */
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]=op1.i64p[idx]%op2.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]=op1.i64p[idx]%op2.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break; /* end NC_INT64 */
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]=op1.ui64p[idx]%op2.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]=op1.ui64p[idx]%op2.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break; /* end NC_UINT64 */
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_mod() */

void
nco_var_msk /* [fnc] Mask third operand where first and second operands fail comparison */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operand op3 */
 const int has_mss_val, /* I [flg] Flag for missing values (basically assumed to be true) */
 ptr_unn mss_val, /* I [val] Value of missing value */
 const double op1, /* I [val] Target value to compare against mask field (i.e., argument of -M) */
 const int op_typ_rlt, /* I [enm] Comparison type test for op2 and op1 */
 ptr_unn op2, /* I [val] Value of mask field */
 ptr_unn op3) /* I/O [val] Values of second operand on input, masked values on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Mask third operand where first and second operands fail comparison
     Set third operand to missing value wherever second operand fails comparison with first operand */
  
  /* Masking is currently defined as: if(op2 !op_typ_rlt op1) then op3:=mss_val */  
  
  long idx;
  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_int mss_val_ntg=nco_int_CEWI;
  nco_short mss_val_short=nco_short_CEWI;
  nco_ushort mss_val_ushort=nco_ushort_CEWI;
  nco_uint mss_val_uint=nco_uint_CEWI;
  nco_int64 mss_val_int64=nco_int64_CEWI;
  nco_uint64 mss_val_uint64=nco_uint64_CEWI;
  nco_byte mss_val_byte=nco_byte_CEWI;
  nco_ubyte mss_val_ubyte=nco_ubyte_CEWI;
  nco_char mss_val_char=nco_char_CEWI;
  /*  nco_string mss_val_string=nco_string_CEWI;*/ /* 20120206: mss_val_string is not yet used so do not define */
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  (void)cast_void_nctype(type,&op3);
  if(has_mss_val){
    (void)cast_void_nctype(type,&mss_val);
  }else{
    (void)fprintf(stdout,"%s: ERROR has_mss_val is inconsistent with purpose of var_ask(), i.e., has_mss_val is not True\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end else */
  
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_INT: mss_val_ntg=*mss_val.ip; break;
    case NC_SHORT: mss_val_short=*mss_val.sp; break;
    case NC_USHORT: mss_val_ushort=*mss_val.usp; break;
    case NC_UINT: mss_val_uint=*mss_val.uip; break;
    case NC_INT64: mss_val_int64=*mss_val.i64p; break;
    case NC_UINT64: mss_val_uint64=*mss_val.ui64p; break;
    case NC_BYTE: mss_val_byte=*mss_val.bp; break;
    case NC_UBYTE: mss_val_ubyte=*mss_val.ubp; break;
    case NC_CHAR: mss_val_char=*mss_val.cp; break;
      /*    case NC_STRING: mss_val_string=*mss_val.sngp; break;*/ /* 20120206: mss_val_string is not yet used so do not define */
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
  } /* endif */
  
  /* NB: Explicit coercion when comparing op2 to op1 is necessary */
  switch(type){
  case NC_FLOAT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.fp[idx] != (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.fp[idx] == (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >= (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <= (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >  (float)op1) op3.fp[idx]=mss_val_flt; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <  (float)op1) op3.fp[idx]=mss_val_flt; break;
    } /* end switch */
    break;
  case NC_DOUBLE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.dp[idx] != (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.dp[idx] == (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >= (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <= (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >  (double)op1) op3.dp[idx]=mss_val_dbl; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <  (double)op1) op3.dp[idx]=mss_val_dbl; break;
    } /* end switch */
    break;
  case NC_INT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.ip[idx] != (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.ip[idx] == (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.ip[idx] >= (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.ip[idx] <= (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.ip[idx] >  (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.ip[idx] <  (nco_int)op1) op3.ip[idx]=mss_val_ntg; break;
    } /* end switch */
    break;
  case NC_SHORT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.sp[idx] != (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.sp[idx] == (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >= (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <= (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >  (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <  (nco_short)op1) op3.sp[idx]=mss_val_short; break;
    } /* end switch */
    break;
  case NC_USHORT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.usp[idx] != (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.usp[idx] == (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.usp[idx] >= (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.usp[idx] <= (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.usp[idx] >  (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.usp[idx] <  (nco_ushort)op1) op3.usp[idx]=mss_val_ushort; break;
    } /* end switch */
    break;
  case NC_UINT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.uip[idx] != (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.uip[idx] == (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.uip[idx] >= (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.uip[idx] <= (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.uip[idx] >  (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.uip[idx] <  (nco_uint)op1) op3.uip[idx]=mss_val_uint; break;
    } /* end switch */
    break;
  case NC_INT64:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] != (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] == (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] >= (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] <= (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] >  (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.i64p[idx] <  (nco_int64)op1) op3.i64p[idx]=mss_val_int64; break;
    } /* end switch */
    break;
  case NC_UINT64:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] != (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] == (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] >= (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] <= (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] >  (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.ui64p[idx] <  (nco_uint64)op1) op3.ui64p[idx]=mss_val_uint64; break;
    } /* end switch */
    break;
  case NC_BYTE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.bp[idx] != (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.bp[idx] == (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >= (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <= (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >  (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <  (nco_byte)op1) op3.bp[idx]=mss_val_byte; break;
    } /* end switch */
    break;
  case NC_UBYTE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] != (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] == (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] >= (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] <= (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] >  (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.ubp[idx] <  (nco_ubyte)op1) op3.ubp[idx]=mss_val_ubyte; break;
    } /* end switch */
    break;
  case NC_CHAR:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.cp[idx] != (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.cp[idx] == (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >= (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <= (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >  (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <  (nco_char)op1) op3.cp[idx]=mss_val_char; break;
    } /* end switch */
    break;
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* It is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_msk() */

void
nco_var_tll_zro_mss_val /* [fnc] Write missing value into elements with zero tally */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 const long * const tally, /* I [nbr] Counter to normalize by */
 ptr_unn op1) /* I/O [val] Values of first operand on input, possibly missing values on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Write missing value into elements with zero tally
     Routine is necessary because initialization of accumulating sums (specified, e.g., with -y ttl or with -N)
     sets initial sum to zero (so augmenting works) regardless if first slice is missing.
     Such sums are usually normalized and set to missing if tally is zero.
     However, totals are integrals and thus are never normalized.
     Initialization value of zero will be output even if tally is zero,
     _unless field is processed with this routine after summing and prior to writing. */
  
  /* Filter currently works as op1:=mss_val where tally == 0 */
  
  long idx;
  
  /* Routine changes nothing unless a missing value is defined */
  if(!has_mss_val) return;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    {
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.fp[idx]=mss_val_flt;
    }
    break;
  case NC_DOUBLE:
    {
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.dp[idx]=mss_val_dbl;
    }
    break;
  case NC_INT:
    {
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.ip[idx]=mss_val_ntg;
    }
    break;
  case NC_SHORT:
    {
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.sp[idx]=mss_val_short;
    }
    break;
  case NC_USHORT:
    {
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.usp[idx]=mss_val_ushort;
    }
    break;
  case NC_UINT:
    {
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.uip[idx]=mss_val_uint;
    }
    break;
  case NC_INT64:
    {
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.i64p[idx]=mss_val_int64;
    }
    break;
  case NC_UINT64:
    {
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] == 0L) op1.ui64p[idx]=mss_val_uint64;
    }
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_tll_zro_mss_val() */

void
nco_var_nrm /* [fnc] Normalize value of first operand by count in tally array */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 const long * const tally, /* I [nbr] Counter to normalize by */
 ptr_unn op1) /* I/O [val] Values of first operand on input, normalized result on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Normalize value of first operand by count in tally array */
  
  /* Normalization is currently defined as op1:=op1/tally */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      /* Operations: 1 fp divide, 2 pointer offset, 2 user memory fetch
	 Repetitions: \dmnszavg^(\dmnnbr-\avgnbr)
	 Total Counts: \flpnbr=\dmnszavg^(\dmnnbr-\avgnbr), \rthnbr=2\dmnszavg^(\dmnnbr-\avgnbr), \mmrusrnbr=2\dmnszavg^(\dmnnbr-\avgnbr)
	 NB: Counted LHS+RHS+tally offsets and fetches */
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=tally[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.fp[idx]/=tally[idx]; else op1.fp[idx]=mss_val_flt;
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=tally[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.dp[idx]/=tally[idx]; else op1.dp[idx]=mss_val_dbl;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]/=tally[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.ip[idx]/=tally[idx]; else op1.ip[idx]=mss_val_ntg;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=tally[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.sp[idx]/=tally[idx]; else op1.sp[idx]=mss_val_short;
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]/=tally[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.usp[idx]/=tally[idx]; else op1.usp[idx]=mss_val_ushort;
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]/=tally[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.uip[idx]/=tally[idx]; else op1.uip[idx]=mss_val_uint;
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]/=tally[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.i64p[idx]/=tally[idx]; else op1.i64p[idx]=mss_val_int64;
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]/=tally[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.ui64p[idx]/=tally[idx]; else op1.ui64p[idx]=mss_val_uint64;
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_nrm() */

void
nco_var_nrm_sdn /* [fnc] Normalize value of first operand by count-1 in tally array */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 const long * const tally, /* I [nbr] Counter to normalize by */
 ptr_unn op1) /* I/O [val] Values of first operand on input, normalized result on output */
{
  /* Purpose: Normalize value of first operand by count-1 in tally array */
  
  /* Normalization is currently defined as op1:=op1/(--tally) */  
  
  /* nco_var_nrm_sdn() is based on nco_var_nrm() and algorithms should be kept consistent with eachother */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=tally[idx]-1L;
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.fp[idx]/=tally[idx]-1L; else op1.fp[idx]=mss_val_flt;
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=tally[idx]-1L;
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.dp[idx]/=tally[idx]-1L; else op1.dp[idx]=mss_val_dbl;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ip[idx]/=tally[idx]-1L;
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.ip[idx]/=tally[idx]-1L; else op1.ip[idx]=mss_val_ntg;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=tally[idx]-1L;
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.sp[idx]/=tally[idx]-1L; else op1.sp[idx]=mss_val_short;
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.usp[idx]/=tally[idx]-1L;
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.usp[idx]/=tally[idx]-1L; else op1.usp[idx]=mss_val_ushort;
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.uip[idx]/=tally[idx]-1L;
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.uip[idx]/=tally[idx]-1L; else op1.uip[idx]=mss_val_uint;
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.i64p[idx]/=tally[idx]-1L;
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.i64p[idx]/=tally[idx]-1L; else op1.i64p[idx]=mss_val_int64;
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.ui64p[idx]/=tally[idx]-1L;
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++) if(tally[idx] > 1L) op1.ui64p[idx]/=tally[idx]-1L; else op1.ui64p[idx]=mss_val_uint64;
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end of nco_var_nrm_sdn */

void
nco_var_pwr /* [fnc] Raise first operand to power of second operand */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of base */
 ptr_unn op2) /* I/O [val] Values of exponent on input, values of power on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Raise value of first operand to power of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Em-powering is currently defined as op2:=op1^op2 */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){ 
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]=powf(op1.fp[idx],op2.fp[idx]);
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] != mss_val_flt)) op2.fp[idx]=powf(op1.fp[idx],op2.fp[idx]); else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break; /* end NC_FLOAT */
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]=pow(op1.dp[idx],op2.dp[idx]);
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] != mss_val_dbl)) op2.dp[idx]=pow(op1.dp[idx],op2.dp[idx]); else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break; /* end NC_DOUBLE */
  case NC_INT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",prg_nm_get());
    break;
  case NC_SHORT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",prg_nm_get());
    break;
  case NC_USHORT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",prg_nm_get());
    break;
  case NC_UINT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",prg_nm_get());
    break;
  case NC_INT64:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",prg_nm_get());
    break;
  case NC_UINT64:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",prg_nm_get());
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_pwr */

void
nco_var_sbt /* [fnc] Subtract first operand from second operand */
(const nc_type type, /* I [type] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of difference on output */
{
  /* Purpose: Subtract value of first operand from value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory */
  
  /* Subtraction is currently defined as op2:=op2-op1 */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]-=op1.fp[idx];
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]-=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]-=op1.dp[idx];
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]-=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ip[idx]-=op1.ip[idx];
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if((op2.ip[idx] != mss_val_ntg) && (op1.ip[idx] != mss_val_ntg)) op2.ip[idx]-=op1.ip[idx]; else op2.ip[idx]=mss_val_ntg;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]-=op1.sp[idx];
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_short) && (op1.sp[idx] != mss_val_short)) op2.sp[idx]-=op1.sp[idx]; else op2.sp[idx]=mss_val_short;
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.usp[idx]-=op1.usp[idx];
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if((op2.usp[idx] != mss_val_ushort) && (op1.usp[idx] != mss_val_ushort)) op2.usp[idx]-=op1.usp[idx]; else op2.usp[idx]=mss_val_ushort;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.uip[idx]-=op1.uip[idx];
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if((op2.uip[idx] != mss_val_uint) && (op1.uip[idx] != mss_val_uint)) op2.uip[idx]-=op1.uip[idx]; else op2.uip[idx]=mss_val_uint;
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.i64p[idx]-=op1.i64p[idx];
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if((op2.i64p[idx] != mss_val_int64) && (op1.i64p[idx] != mss_val_int64)) op2.i64p[idx]-=op1.i64p[idx]; else op2.i64p[idx]=mss_val_int64;
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.ui64p[idx]-=op1.ui64p[idx];
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if((op2.ui64p[idx] != mss_val_uint64) && (op1.ui64p[idx] != mss_val_uint64)) op2.ui64p[idx]-=op1.ui64p[idx]; else op2.ui64p[idx]=mss_val_uint64;
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_sbt() */

void
nco_var_sqrt /* [fnc] Place squareroot of first operand in value of second operand */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [val] Value of missing value */
 long * restrict const tally, /* I/O [nbr] Counter space */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* O [val] Squareroot of first operand */
{
  /* Purpose: Place squareroot of first operand in value of second operand 
     Assume operands conform, are same type, and are in memory */
  
  /* Square root is currently defined as op2:=sqrt(op1) */
  
  /* NB: Many compilers need to #include "nco_rth_flt.h" for sqrtf() prototype */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]=sqrtf(op1.fp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt){
	  op2.fp[idx]=sqrtf(op1.fp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.dp[idx]=sqrt(op1.dp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl){
	  op2.dp[idx]=sqrt(op1.dp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ip[idx]=(nco_int)sqrt((double)(op1.ip[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int mss_val_ntg=*mss_val.ip;
      for(idx=0;idx<sz;idx++){
	if(op1.ip[idx] != mss_val_ntg){
	  op2.ip[idx]=(nco_int)sqrt((double)(op1.ip[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]=(nco_short)sqrt((double)(op1.sp[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_short mss_val_short=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_short){
	  op2.sp[idx]=(nco_short)sqrt((double)(op1.sp[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_USHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.usp[idx]=(nco_ushort)sqrt((double)(op1.usp[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_ushort mss_val_ushort=*mss_val.usp;
      for(idx=0;idx<sz;idx++){
	if(op1.usp[idx] != mss_val_ushort){
	  op2.usp[idx]=(nco_ushort)sqrt((double)(op1.usp[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.uip[idx]=(nco_uint)sqrt((double)(op1.uip[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint mss_val_uint=*mss_val.uip;
      for(idx=0;idx<sz;idx++){
	if(op1.uip[idx] != mss_val_uint){
	  op2.uip[idx]=(nco_uint)sqrt((double)(op1.uip[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_INT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.i64p[idx]=(nco_int64)sqrt((double)(op1.i64p[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_int64 mss_val_int64=*mss_val.i64p;
      for(idx=0;idx<sz;idx++){
	if(op1.i64p[idx] != mss_val_int64){
	  op2.i64p[idx]=(nco_int64)sqrt((double)(op1.i64p[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_UINT64:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.ui64p[idx]=(nco_uint64)sqrt((double)(op1.ui64p[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const nco_uint64 mss_val_uint64=*mss_val.ui64p;
      for(idx=0;idx<sz;idx++){
	if(op1.ui64p[idx] != mss_val_uint64){
	  op2.ui64p[idx]=(nco_uint64)sqrt((double)(op1.ui64p[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_sqrt() */

void
nco_var_zero /* [fnc] Zero value of first operand */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] Size (in elements) of operand */
 ptr_unn op1) /* O [val] Values of first operand zeroed on output */
{
  /* Purpose: Zero value of first operand */
  
  /* fxm: According to hjm, floats and ints all use same bit pattern for zero
     ccc --tst=bnr --int_foo=0 
     and
     ccc --dbg=0 --tst=gsl --gsl_a=0.0 
     confirm this.
     Hence, it may be faster to use memset() system call to zero memory 
     Same approach is used in nco_zero_long() */
  
  size_t sz_byt; /* [B] Number of bytes in variable buffer */
  sz_byt=(size_t)sz*nco_typ_lng(type);
  switch(type){
  case NC_FLOAT:
  case NC_DOUBLE:
  case NC_INT:
  case NC_SHORT:
  case NC_USHORT:
  case NC_UINT:
  case NC_INT64:
  case NC_UINT64:
    (void)memset(op1.vp,0,sz_byt);
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
#if 0
  /* Presumably this old method used until 20050321 is slower because of pointer de-referencing */
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  
  switch(type){
  case NC_FLOAT:
    for(idx=0;idx<sz;idx++) op1.fp[idx]=0.0;
    break;
  case NC_DOUBLE:
    for(idx=0;idx<sz;idx++) op1.dp[idx]=0.0;
    break;
  case NC_INT:
    for(idx=0;idx<sz;idx++) op1.ip[idx]=0L;
    break;
  case NC_SHORT:
    for(idx=0;idx<sz;idx++) op1.sp[idx]=0;
    break;
  case NC_USHORT:
    for(idx=0;idx<sz;idx++) op1.usp[idx]=0;
    break;
  case NC_UINT:
    for(idx=0;idx<sz;idx++) op1.uip[idx]=0;
    break;
  case NC_INT64:
    for(idx=0;idx<sz;idx++) op1.i64p[idx]=0;
    break;
  case NC_UINT64:
    for(idx=0;idx<sz;idx++) op1.ui64p[idx]=0;
    break;
  case NC_BYTE: break; /* Do nothing */
  case NC_UBYTE: break; /* Do nothing */
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
#endif /* !0 */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_zero() */
