/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.c,v 1.6 2002-06-07 01:39:54 zender Exp $ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_var_rth.h" /* Variable arithmetic */

void
var_abs(nc_type type,const long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1)
     /* 
	nc_type type: I netCDF type of operands
	const long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
     */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Find the absolute value of all numbers in op1
     Store result in first operand */    
  
  /* Absolute value is currently defined as op1:=abs(op1) */  
  
#ifndef __GNUG__
  float fabsf(float); /* Sun math.h does not include fabsf() prototype */
#endif /* __GNUG__ */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=fabsf(op1.fp[idx]);
    }else{
      const float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=fabsf(op1.fp[idx]); 
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=fabs(op1.dp[idx]);
    }else{
      const double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fabs(op1.dp[idx]);
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]=labs(op1.lp[idx]); /* int abs(int), long labs(long) */
    }else{
      const long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]=labs(op1.lp[idx]); /* int abs(int), long labs(long) */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) if(op1.sp[idx] < 0 ) op1.sp[idx]=-op1.sp[idx] ;
    }else{
      const short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_abs() */

void
var_add(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz: I size (in elements) of operands
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I/O counter space
  ptr_unn op1: I values of first operand
  ptr_unn op2: I/O values of second operand on input, values of sum on output
 */
{
  /* Routine to add value of first operand to value of second operand 
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and be of the specified type. Operands' values are 
     assumed to be in memory already. */

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
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
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
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
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
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
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
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_add() */

void
var_add_no_tally(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* nc_type type: I [type] netCDF type of operands
  long sz: I [nbr] Size (in elements) of operands
  int has_mss_val: I [flg] Flag for missing values
  ptr_unn mss_val: I [flg] Value of missing value
  ptr_unn op1: I [val] Values of first operand
  ptr_unn op2: I/O [val] Values of second operand on input, values of sum on output */
{
  /* Routine to add value of first operand to value of second operand 
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and be of the specified type. Operands' values are 
     assumed to be in memory already. */

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
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]+=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]+=op1.dp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)) op2.dp[idx]+=op1.dp[idx]; else op2.dp[idx]=*mss_val.dp;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]+=op1.lp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != *mss_val.lp) && (op1.lp[idx] != *mss_val.lp)) op2.lp[idx]+=op1.lp[idx]; else op2.lp[idx]=*mss_val.lp;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]+=op1.sp[idx];
    }else{
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
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
    default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_add_no_tally() */

void
var_subtract(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I [type] netCDF type of operands
  long sz: I [nbr] Size (in elements) of operands
  int has_mss_val: I [flg] Flag for missing values
  ptr_unn mss_val: I [flg] Value of missing value
  ptr_unn op1: I [val] Values of first operand
  ptr_unn op2: I/O [val] Values of second operand on input, values of difference on output
 */
{
  /* Purpose: Subtract value of first operand from value of second operand 
     and store result in second operand. 
     Operands are assumed to have conforming dimensions, and be of specified type. 
     Operands' values are assumed to be in memory already. */

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
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]-=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]-=op1.dp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)) op2.dp[idx]-=op1.dp[idx]; else op2.dp[idx]=*mss_val.dp;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]-=op1.lp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != *mss_val.lp) && (op1.lp[idx] != *mss_val.lp)) op2.lp[idx]-=op1.lp[idx]; else op2.lp[idx]=*mss_val.lp;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]-=op1.sp[idx];
    }else{
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
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
    default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_subtract() */

void
var_multiply(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
	ptr_unn op2: I/O values of second operand on input, values of product on output
     */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: multiply value of first operand by value of second operand 
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and to both be of the specified type. Operands' values are 
     assumed to be in memory already. */
  
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
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]*=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]*=op1.dp[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]*=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]*=op1.lp[idx];
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]*=op1.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]*=op1.sp[idx];
    }else{
      long mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_multiply() */

void
var_divide(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz: I size (in elements) of operands
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  ptr_unn op1: I values of first operand
  ptr_unn op2: I/O values of second operand on input, values of quotient on output
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide value of first operand by value of second operand 
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and to both be of specified type. Operands' values are 
     assumed to be in memory already. */

  /* Division is currently defined as op2:=op2/op1 */  

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
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]/=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]/=op1.dp[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]/=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.lp[idx]/=op1.lp[idx];
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.lp[idx] != mss_val_lng) && (op1.lp[idx] != mss_val_lng)) op2.lp[idx]/=op1.lp[idx]; else op2.lp[idx]=mss_val_lng;
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.sp[idx]/=op1.sp[idx];
    }else{
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.sp[idx] != mss_val_sht) && (op1.sp[idx] != mss_val_sht)) op2.sp[idx]/=op1.sp[idx]; else op2.sp[idx]=mss_val_sht;
      } /* end for */
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_divide() */

void
var_min_bnr(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz: I size (in elements) of operands
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  ptr_unn op1: I values of first operand
  ptr_unn op2: I/O values of second operand on input, values of maximium on output
*/
{
  /* Purpose: Find minimium value(s) of two operands and store result in second operand 
     Operands are assumed to have conforming dimensions, and to both be of the specified type
     Operands' values are assumed to be in memory already */
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
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
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
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
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
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
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
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end var_min_bnr() */

void
var_max_bnr(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
   nc_type type: I netCDF type of operands
   long sz: I size (in elements) of operands
   int has_mss_val: I flag for missing values
   ptr_unn mss_val: I value of missing value
   ptr_unn op1: I values of first operand
   ptr_unn op2: I/O values of second operand on input, values of maximium on output
*/
{
  /* Routine to find maximium value(s) of the two operands
     and store result in second operand. Operands are assumed to have conforming
     dimensions, and to both be of the specified type. Operands' values are 
     assumed to be in memory already. */
  
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
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
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
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
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
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
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
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end var_max_bnr() */

void
var_sqrt(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz: I size (in elements) of operands
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I/O counter space
  ptr_unn op1: I values of first operand
  ptr_unn op2: O squareroot of first operand
 */
{
  /* Purpose: Place squareroot of first operand in value of second operand 
     Operands are assumed to have conforming dimensions, and be of specified type 
     Operands' values are assumed to be in memory already */

  /* Square root is currently defined as op2:=sqrt(op1) */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.fp[idx]=sqrt(op1.fp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt){
	  op2.fp[idx]=sqrt(op1.fp[idx]);
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
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
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
	op2.lp[idx]=(long)sqrt(op1.lp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng){
	  op2.lp[idx]=(long)sqrt(op1.lp[idx]);
	  tally[idx]++;
	} /* end if */
      } /* end for */
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++){
	op2.sp[idx]=(short)sqrt(op1.sp[idx]);
	tally[idx]++;
      } /* end for */
    }else{
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_sht){
	  op2.sp[idx]=(short)sqrt(op1.sp[idx]);
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_sqrt() */

void
var_normalize(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1)
/* 
  nc_type type: I netCDF type of operand
  long sz: I size (in elements) of operand
  int has_mss_val: I flag for missing values
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
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.fp[idx]/=tally[idx]; else op1.fp[idx]=mss_val_flt;
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=tally[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.dp[idx]/=tally[idx]; else op1.dp[idx]=mss_val_dbl;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=tally[idx];
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.lp[idx]/=tally[idx]; else op1.lp[idx]=mss_val_lng;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=tally[idx];
    }else{
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.sp[idx]/=tally[idx]; else op1.sp[idx]=mss_val_sht;
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_normalize() */

void
var_normalize_sdn(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1)
/* 
  nc_type type: I netCDF type of operand
  long sz: I size (in elements) of operand
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I counter space
  ptr_unn op1: I/O values of first operand on input, normalized result on output
*/
{
  /* Purpose: Normalize value of first operand by count-1 in tally array 
     and store result in first operand. */

  /* Normalization is currently defined as op1:=op1/(--tally) */  

  /* var_normalize_sdn() is based on var_normalize() and algorithms should be kept consistent with eachother */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=(tally[idx]-1);
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.fp[idx]/=(tally[idx]-1); else op1.fp[idx]=mss_val_flt;
    } /* end else */
    break;
  case NC_DOUBLE:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=(tally[idx]-1);
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.dp[idx]/=(tally[idx]-1); else op1.dp[idx]=mss_val_dbl;
    } /* end else */
    break;
  case NC_INT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=(tally[idx]-1);
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.lp[idx]/=(tally[idx]-1); else op1.lp[idx]=mss_val_lng;
    } /* end else */
    break;
  case NC_SHORT:
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=(tally[idx]-1);
    }else{
      short mss_val_sht=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if((tally[idx]-1) != 0L) op1.sp[idx]/=(tally[idx]-1); else op1.sp[idx]=mss_val_sht;
    } /* end else */
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_BYTE:
    /* Do nothing */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end of var_normalize_sdn */

void
var_mask(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,double op1,int op_typ_rlt,ptr_unn op2,ptr_unn op3)
/* 
  nc_type type: I netCDF type of operand op3
  long sz: I size (in elements) of operand op3
  int has_mss_val: I flag for missing values (basically assumed to be true)
  ptr_unn mss_val: I value of missing value
  double op1: I Target value against which mask field will be compared (i.e., argument of -M)
  int op_typ_rlt: I type of relationship to test for between op2 and op1
  ptr_unn op2: I Value of mask field
  ptr_unn op3: I/O values of second operand on input, masked values on output
 */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Routine to mask third operand by second operand. Wherever second operand does not 
     equal first operand the third operand will be set to its missing value. */

  /* Masking is currently defined as if(op2 !op_typ_rlt op1) then op3:=mss_val */  

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op2);
  (void)cast_void_nctype(type,&op3);
  if(has_mss_val){
    (void)cast_void_nctype(type,&mss_val);
  }else{
    (void)fprintf(stdout,"%s: ERROR has_mss_val is inconsistent with purpose of var_ask(), i.e., has_mss_val is not True\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* NB: Explicit coercion when comparing op2 to op1 is necessary */
  switch(type){
  case NC_FLOAT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.fp[idx] != (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.fp[idx] == (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >= (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <= (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.fp[idx] >  (float)op1) op3.fp[idx]=*mss_val.fp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.fp[idx] <  (float)op1) op3.fp[idx]=*mss_val.fp; break;
    } /* end switch */
    break;
  case NC_DOUBLE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.dp[idx] != (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.dp[idx] == (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >= (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <= (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.dp[idx] >  (double)op1) op3.dp[idx]=*mss_val.dp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.dp[idx] <  (double)op1) op3.dp[idx]=*mss_val.dp; break;
    } /* end switch */
    break;
  case NC_INT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.lp[idx] != (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.lp[idx] == (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.lp[idx] >= (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.lp[idx] <= (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.lp[idx] >  (long)op1) op3.lp[idx]=*mss_val.lp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.lp[idx] <  (long)op1) op3.lp[idx]=*mss_val.lp; break;
    } /* end switch */
    break;
  case NC_SHORT:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.sp[idx] != (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.sp[idx] == (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >= (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <= (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.sp[idx] >  (short)op1) op3.sp[idx]=*mss_val.sp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.sp[idx] <  (short)op1) op3.sp[idx]=*mss_val.sp; break;
    } /* end switch */
    break;
  case NC_CHAR:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.cp[idx] != (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.cp[idx] == (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >= (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <= (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.cp[idx] >  (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.cp[idx] <  (unsigned char)op1) op3.cp[idx]=*mss_val.cp; break;
    } /* end switch */
    break;
  case NC_BYTE:
    switch(op_typ_rlt){
    case nco_op_eq: for(idx=0;idx<sz;idx++) if(op2.bp[idx] != (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_ne: for(idx=0;idx<sz;idx++) if(op2.bp[idx] == (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_lt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >= (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_gt: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <= (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_le: for(idx=0;idx<sz;idx++) if(op2.bp[idx] >  (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    case nco_op_ge: for(idx=0;idx<sz;idx++) if(op2.bp[idx] <  (signed char)op1) op3.bp[idx]=*mss_val.bp; break;
    } /* end switch */
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* It is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_mask() */

void
var_zero(nc_type type,long sz,ptr_unn op1)
/* 
  nc_type type: I netCDF type of operand
  long sz: I size (in elements) of operand
  ptr_unn op1: I values of first operand
 */
{
  /* Routine to zero value of first operand and store result in second operand. */

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
    default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_zero() */

