/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.c,v 1.26 2004-07-19 07:35:55 zender Exp $ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_var_rth.h" /* Variable arithmetic */

void
nco_var_abs(const nc_type type,const long sz,const int has_mss_val,ptr_unn mss_val,ptr_unn op1)
/* const nc_type type: I netCDF type of operands
   const long sz: I size (in elements) of operands
   const int has_mss_val: I flag for missing values
   ptr_unn mss_val: I value of missing value
   ptr_unn op1: I values of first operand */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Find the absolute value of all numbers in op1
     Store result in first operand */    
  
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
      for(idx=0;idx<sz;idx++) op1.lp[idx]=labs(op1.lp[idx]); /* int abs(int), long labs(long) */
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]=labs(op1.lp[idx]); /* int abs(int), long labs(long) */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) if(op1.sp[idx] < 0 ) op1.sp[idx]=-op1.sp[idx] ;
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht && op1.sp[idx] < 0 ) op1.sp[idx]=-op1.sp[idx];
      } /* end for */
    } /* end else */
    break;
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
     nco_var_add() does not increment a tally counter
     nco_var_add_tll_ncra() does increment a tally counter */

  /* Addition is currently defined as op2:=op1+op2 */

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
      for(idx=0;idx<sz;idx++) op2.lp[idx]+=op1.lp[idx];
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]+=op1.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]+=op1.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_sht) && (op1.sp[idx] != mss_val_sht)) op2.sp[idx]+=op1.sp[idx]; else op2.sp[idx]=mss_val_sht;
      } /* end for */
    } /* end else */
    break;
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
  
} /* end nco_var_add() */

void
nco_var_add_tll_ncflint /* [fnc] Add first operand to second operand, increment tally */
(const nc_type type, /* I [enm] netCDF type of operands */
 const long sz, /* I [nbr] Size (in elements) of operands */
 const int has_mss_val, /* I [flg] Flag for missing values */
 ptr_unn mss_val, /* I [flg] Value of missing value */
 long *tally, /* I/O [nbr] Counter space */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand on input, values of sum on output */
{
  /* Purpose: Add value of first operand to value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() does not increment a tally counter
     nco_var_add_tll_ncflint() does increment a tally counter */

  /* Addition is currently defined as op2:=op1+op2 */

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
	}/* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.lp[idx]+=op1.lp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)){
	  op2.lp[idx]+=op1.lp[idx];
	  tally[idx]++;
	}else{
	  op2.lp[idx]=mss_val_lng;
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
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_sht) && (op1.sp[idx] != mss_val_sht)){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	}else{
	  op2.sp[idx]=mss_val_sht;
	} /* end else */
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
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
	op2.lp[idx]+=op1.lp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)){
	  op2.lp[idx]+=op1.lp[idx];
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
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_sht) && (op1.sp[idx] != mss_val_sht)){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
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
 long *tally, /* I/O [nbr] Counter space */
 ptr_unn op1, /* I [val] Values of first operand */
 ptr_unn op2) /* I/O [val] Values of second operand (running sum) on input, values of new sum on output */
{
  /* Purpose: Add value of first operand to value of second operand 
     and store result in second operand. 
     Assume operands conform, are same type, and are in memory
     nco_var_add() adds if neither operand equals missing_value
     nco_var_add() does not increment tally counter.
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
  The only difference between nco_var_add_tll_ncra() and nco_var_add_tll_ncflint() is that 
  nco_var_add_tll_ncflint() checks both operands against the missing_value, whereas 
  nco_var_add_tll_ncra() checks only the first operand (the new record) against the missing_value
  The nco_var_add_tll_ncflint() algorithm fails as a running average algorithm when
  the missing value is zero because the running sum is bootstrapped to zero which 
  causes the comparison to missing_value to always be true.
  nco_var_add_tll_ncflint() also fails as a running average algorithm whenever the
  running sum happens to equal the missing_value (regardless if the missing value is zero).
  NCO uses nco_var_add_tll_ncflint() only for ncflint
  NCO uses nco_var_add_tll_ncra() only for ncra */

  /* Addition is currently defined as op2:=op1+op2 */

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
	op2.lp[idx]+=op1.lp[idx];
	tally[idx]++;
      } /* end for */
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng){
	  op2.lp[idx]+=op1.lp[idx];
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
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht){
	  op2.sp[idx]+=op1.sp[idx];
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
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

} /* end nco_var_add_tll_ncra() */

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
      for(idx=0;idx<sz;idx++) op2.lp[idx]/=op1.lp[idx];
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]/=op1.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break; /* end NC_INT */
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]/=op1.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_sht) && (op1.sp[idx] != mss_val_sht)) op2.sp[idx]/=op1.sp[idx]; else op2.sp[idx]=mss_val_sht;
      } /* end for */
    } /* end else */
    break; /* end NC_SHORT */
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
	if(op2.lp[idx] < op1.lp[idx]) 
	  op2.lp[idx]=op1.lp[idx];
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if(op2.lp[idx] == mss_val_lng) 
	  op2.lp[idx]=op1.lp[idx];
	else if((op1.lp[idx] != mss_val_lng) && (op2.lp[idx] < op1.lp[idx]))
	  op2.lp[idx]=op1.lp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.sp[idx] < op1.sp[idx])
	  op2.sp[idx]=op1.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] == mss_val_sht) 
	  op2.sp[idx]=op1.sp[idx];
	else if((op1.sp[idx] != mss_val_sht) && (op2.sp[idx] < op1.sp[idx]))
	  op2.sp[idx]=op1.sp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
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
	if(op2.lp[idx] > op1.lp[idx]) 
	  op2.lp[idx]=op1.lp[idx];
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if(op2.lp[idx] == mss_val_lng) 
	  op2.lp[idx]=op1.lp[idx];
	else if((op1.lp[idx] != mss_val_lng) && (op2.lp[idx] > op1.lp[idx]))
	  op2.lp[idx]=op1.lp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) 
	if(op2.sp[idx] > op1.sp[idx])  op2.sp[idx]=op1.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op2.sp[idx] == mss_val_sht) 
	  op2.sp[idx]=op1.sp[idx];
	else if((op1.sp[idx] != mss_val_sht) && (op2.sp[idx] > op1.sp[idx]))
	  op2.sp[idx]=op1.sp[idx]; 
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
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
      for(idx=0;idx<sz;idx++) op2.lp[idx]*=op1.lp[idx];
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]*=op1.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]*=op1.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_sht) && (op1.sp[idx] != mss_val_sht)) op2.sp[idx]*=op1.sp[idx]; else op2.sp[idx]=mss_val_sht;
      } /* end for */
    } /* end else */
    break;
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
      for(idx=0;idx<sz;idx++) op2.lp[idx]=op1.lp[idx]%op2.lp[idx];
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]=op1.lp[idx]%op2.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break; /* end NC_INT */
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]=op1.sp[idx]%op2.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_sht) && (op1.sp[idx] != mss_val_sht)) op2.sp[idx]=op1.sp[idx]%op2.sp[idx]; else op2.sp[idx]=mss_val_sht;
      } /* end for */
    } /* end else */
    break; /* end NC_SHORT */
  case NC_CHAR:
    /* Do nothing */
    break; /* end NC_CHAR */
  case NC_BYTE:
    /* Do nothing */
    break; /* end NC_BYTE */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end nco_var_mod() */

void
nco_var_msk(const nc_type type,const long sz,const int has_mss_val,ptr_unn mss_val,const double op1,const int op_typ_rlt,ptr_unn op2,ptr_unn op3)
/* const nc_type type: I netCDF type of operand op3
  const long sz: I size (in elements) of operand op3
  const int has_mss_val: I flag for missing values (basically assumed to be true)
  ptr_unn mss_val: I value of missing value
  const double op1: I Target value against which mask field will be compared (i.e., argument of -M)
  const int op_typ_rlt: I type of relationship to test for between op2 and op1
  ptr_unn op2: I Value of mask field
  ptr_unn op3: I/O values of second operand on input, masked values on output */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Mask third operand by second operand
     Set third operand to missing value wherever second operand does not equal first operand */

  /* Masking is currently defined as: if(op2 !op_typ_rlt op1) then op3:=mss_val */  

  long idx;
  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  unsigned char mss_val_chr=char_CEWI;
  signed char mss_val_byt=byte_CEWI;
  nco_long mss_val_lng=nco_long_CEWI;
  short mss_val_sht=short_CEWI;

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
    case NC_SHORT: mss_val_sht=*mss_val.sp; break;
    case NC_INT: mss_val_lng=*mss_val.lp; break;
    case NC_BYTE: mss_val_byt=*mss_val.bp; break;
    case NC_CHAR: mss_val_chr=*mss_val.cp; break;
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
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.lp[idx] != (long)op1) op3.lp[idx]=mss_val_lng; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.lp[idx] == (long)op1) op3.lp[idx]=mss_val_lng; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.lp[idx] >= (long)op1) op3.lp[idx]=mss_val_lng; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.lp[idx] <= (long)op1) op3.lp[idx]=mss_val_lng; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.lp[idx] >  (long)op1) op3.lp[idx]=mss_val_lng; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.lp[idx] <  (long)op1) op3.lp[idx]=mss_val_lng; break;
    } /* end switch */
    break;
  case NC_SHORT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.sp[idx] != (short)op1) op3.sp[idx]=mss_val_sht; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.sp[idx] == (short)op1) op3.sp[idx]=mss_val_sht; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >= (short)op1) op3.sp[idx]=mss_val_sht; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <= (short)op1) op3.sp[idx]=mss_val_sht; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >  (short)op1) op3.sp[idx]=mss_val_sht; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <  (short)op1) op3.sp[idx]=mss_val_sht; break;
    } /* end switch */
    break;
  case NC_CHAR:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.cp[idx] != (unsigned char)op1) op3.cp[idx]=mss_val_chr; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.cp[idx] == (unsigned char)op1) op3.cp[idx]=mss_val_chr; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >= (unsigned char)op1) op3.cp[idx]=mss_val_chr; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <= (unsigned char)op1) op3.cp[idx]=mss_val_chr; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >  (unsigned char)op1) op3.cp[idx]=mss_val_chr; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <  (unsigned char)op1) op3.cp[idx]=mss_val_chr; break;
    } /* end switch */
    break;
  case NC_BYTE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.bp[idx] != (signed char)op1) op3.bp[idx]=mss_val_byt; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.bp[idx] == (signed char)op1) op3.bp[idx]=mss_val_byt; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >= (signed char)op1) op3.bp[idx]=mss_val_byt; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <= (signed char)op1) op3.bp[idx]=mss_val_byt; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >  (signed char)op1) op3.bp[idx]=mss_val_byt; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <  (signed char)op1) op3.bp[idx]=mss_val_byt; break;
    } /* end switch */
    break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* It is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end nco_var_msk() */

void
nco_var_nrm(const nc_type type,const long sz,const int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1)
/* 
  const nc_type type: I netCDF type of operand
  const long sz: I size (in elements) of operand
  const int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I counter space
  ptr_unn op1: I/O values of first operand on input, normalized result on output
*/
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Normalize value of first operand by count in tally array 
     and store result in first operand. */

  /* Normalization is currently defined as op1:=op1/tally */  

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
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
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=tally[idx];
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.lp[idx]/=tally[idx]; else op1.lp[idx]=mss_val_lng;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=tally[idx];
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.sp[idx]/=tally[idx]; else op1.sp[idx]=mss_val_sht;
    } /* end else */
    break;
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

} /* end nco_var_nrm() */

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
      for(idx=0;idx<sz;idx++) op1.fp[idx]=powf(op1.fp[idx],op2.fp[idx]);
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if((op1.fp[idx] != mss_val_flt) && (op2.fp[idx] != mss_val_flt)) op1.fp[idx]=powf(op1.fp[idx],op2.fp[idx]); else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break; /* end NC_FLOAT */
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=pow(op1.dp[idx],op2.dp[idx]);
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] != mss_val_dbl)) op1.dp[idx]=pow(op1.dp[idx],op2.dp[idx]); else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break; /* end NC_DOUBLE */
  case NC_INT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",prg_nm_get());
    break;
  case NC_SHORT:
    (void)fprintf(stdout,"%s: ERROR Attempt to em-power integer type in nco_var_pwr(). See TODO #311.\n",prg_nm_get());
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
  
} /* end nco_var_pwr */

void
nco_var_nrm_sdn(const nc_type type,const long sz,const int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1)
/* 
  const nc_type type: I netCDF type of operand
  const long sz: I size (in elements) of operand
  const int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I counter space
  ptr_unn op1: I/O values of first operand on input, normalized result on output
*/
{
  /* Purpose: Normalize value of first operand by count-1 in tally array 
     and store result in first operand. */

  /* Normalization is currently defined as op1:=op1/(--tally) */  

  /* nco_var_nrm_sdn() is based on nco_var_nrm() and algorithms should be kept consistent with eachother */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=(tally[idx]-1);
    }else{
      const float mss_val_flt=*mss_val.fp;
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.fp[idx]/=(tally[idx]-1); else op1.fp[idx]=mss_val_flt;
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=(tally[idx]-1);
    }else{
      const double mss_val_dbl=*mss_val.dp;
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.dp[idx]/=(tally[idx]-1); else op1.dp[idx]=mss_val_dbl;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=(tally[idx]-1);
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.lp[idx]/=(tally[idx]-1); else op1.lp[idx]=mss_val_lng;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=(tally[idx]-1);
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.sp[idx]/=(tally[idx]-1); else op1.sp[idx]=mss_val_sht;
    } /* end else */
    break;
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

} /* end of nco_var_nrm_sdn */

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
      for(idx=0;idx<sz;idx++) op2.lp[idx]-=op1.lp[idx];
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]-=op1.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]-=op1.sp[idx];
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_sht) && (op1.sp[idx] != mss_val_sht)) op2.sp[idx]-=op1.sp[idx]; else op2.sp[idx]=mss_val_sht;
      } /* end for */
    } /* end else */
    break;
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
  
} /* end nco_var_sbt() */

void
nco_var_sqrt(const nc_type type,const long sz,const int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  const nc_type type: I netCDF type of operands
  const long sz: I size (in elements) of operands
  const int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I/O counter space
  ptr_unn op1: I values of first operand
  ptr_unn op2: O squareroot of first operand
 */
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
	op2.lp[idx]=(long)sqrt((double)(op1.lp[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const long mss_val_lng=*mss_val.lp;
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng){
	  op2.lp[idx]=(long)sqrt((double)(op1.lp[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]=(short)sqrt((double)(op1.sp[idx]));
	tally[idx]++;
      } /* end for */
    }else{
      const short mss_val_sht=*mss_val.sp;
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht){
	  op2.sp[idx]=(short)sqrt((double)(op1.sp[idx]));
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
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

} /* end nco_var_sqrt() */

void
nco_var_zero(const nc_type type,const long sz,ptr_unn op1)
/* 
  const nc_type type: I netCDF type of operand
  const long sz: I size (in elements) of operand
  ptr_unn op1: I values of first operand
 */
{
  /* Purpose: Zero value of first operand and store result in second operand. */

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
    for(idx=0;idx<sz;idx++) op1.lp[idx]=0L;
    break;
  case NC_SHORT:
    for(idx=0;idx<sz;idx++) op1.sp[idx]=0;
    break;
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

} /* end nco_var_zero() */

