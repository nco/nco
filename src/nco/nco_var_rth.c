/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_rth.c,v 1.3 2002-05-06 02:17:56 zender Exp $ */

/* Purpose: Variable arithmetic */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_var_rth.h" /* Variable arithmetic */

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
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_add_real(&sz,&has_mss_val,mss_val.fp,tally,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
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
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_add_double_precision(&sz,&has_mss_val,mss_val.dp,tally,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
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
#endif /* !USE_FORTRAN_ARITHMETIC */
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
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_subtract_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]-=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]-=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_subtract_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]-=op1.dp[idx];
    }else{
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != *mss_val.dp) && (op1.dp[idx] != *mss_val.dp)) op2.dp[idx]-=op1.dp[idx]; else op2.dp[idx]=*mss_val.dp;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
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
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_multiply_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]*=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]*=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_multiply_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]*=op1.dp[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]*=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
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
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_divide_real(&sz,&has_mss_val,mss_val.fp,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.fp[idx]/=op1.fp[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.fp[idx] != mss_val_flt) && (op1.fp[idx] != mss_val_flt)) op2.fp[idx]/=op1.fp[idx]; else op2.fp[idx]=mss_val_flt;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_divide_double_precision(&sz,&has_mss_val,mss_val.dp,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op2.dp[idx]/=op1.dp[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if((op2.dp[idx] != mss_val_dbl) && (op1.dp[idx] != mss_val_dbl)) op2.dp[idx]/=op1.dp[idx]; else op2.dp[idx]=mss_val_dbl;
      } /* end for */
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
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
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_sqrt_real(&sz,&has_mss_val,mss_val.fp,tally,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
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
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_sqrt_double_precision(&sz,&has_mss_val,mss_val.dp,tally,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
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
#endif /* !USE_FORTRAN_ARITHMETIC */
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
var_avg_reduce_ttl(nc_type type,long sz_op1,long sz_op2,int has_mss_val,ptr_unn mss_val,long *tally,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz_op1: I size (in elements) of op1
  long sz_op2: I size (in elements) of op2
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  long *tally: I/O counter space
  ptr_unn op1: I values of first operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2))
  ptr_unn op2: O values resulting from averaging each block of input operand
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Perform arithmetic operation on values in each contiguous block of first operand and place
     result in corresponding element in second operand. 
     Currently the arithmetic operation performed is a simple summation of the elements in op1
     Inpute operands are assumed to have conforming types, but not dimensions or sizes
     var_avg_reduce() knows nothing about dimensions, it is purely a one dimensional array operator
     acting serially on each element of the input buffer op1.
     The calling rouine knows exactly how the rank of the output, op2, is reduced from the rank of the input
     This routine currently only does summing rathering than averaging in order to remain flexible
     It is expected that operations which require normalization, e.g., averaging, will call var_normalize() 
     or var_divide() to divide the sum set in this routine by the tally set in this routine. */

  /* There is a GNUC, a non-GNUC, and a Fortran block for each operation
     GNUC: Utilize (non-ANSI-compliant) compiler support for local automatic arrays
     This results in more elegent loop structure and, theoretically, in faster performance
     non-GNUC: Fully ANSI-compliant structure
     Fortran: Same structure as GNUC blocks (g77 and all Fortran90 compilers support local autmatic arrays) */

#ifndef __GNUC__
  long blk_off;
  long idx_op1;
#endif /* !__GNUC__ */
  long idx_op2;
  long idx_blk;
  long sz_blk;
#ifndef USE_FORTRAN_ARITHMETIC
  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  unsigned char mss_val_chr;
  signed char mss_val_byt;
#endif /* USE_FORTRAN_ARITHMETIC */
  nco_long mss_val_lng=nco_long_CEWI;
  short mss_val_sht=short_CEWI;

  sz_blk=sz_op1/sz_op2;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);

#ifndef USE_FORTRAN_ARITHMETIC
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_sht=*mss_val.sp; break;
    case NC_INT: mss_val_lng=*mss_val.lp; break;
    case NC_BYTE: mss_val_byt=*mss_val.bp; break;
    case NC_CHAR: mss_val_chr=*mss_val.cp; break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */
  } /* endif */
#endif /* USE_FORTRAN_ARITHMETIC */

  switch(type){
  case NC_FLOAT:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_avg_reduce_real(&sz_blk,&sz_op2,&has_mss_val,mss_val.fp,tally,op1.fp,op2.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.fp[idx_op2]+=op1.fp[blk_off+idx_blk];
	tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.fp[idx_op1] != mss_val_flt){
	    op2.fp[idx_op2]+=op1.fp[idx_op1];
	    tally[idx_op2]++;
	  } /* end if */
	} /* end loop over idx_blk */
	if(tally[idx_op2] == 0L) op2.fp[idx_op2]=mss_val_flt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_flt){
	      op2.fp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(tally[idx_op2] == 0L) op2.fp[idx_op2]=mss_val_flt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_avg_reduce_double_precision(&sz_blk,&sz_op2,&has_mss_val,mss_val.dp,tally,op1.dp,op2.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.dp[idx_op2]+=op1.dp[blk_off+idx_blk];
	tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.dp[idx_op1] != mss_val_dbl){
	    op2.dp[idx_op2]+=op1.dp[idx_op1];
	    tally[idx_op2]++;
	  } /* end if */
	} /* end loop over idx_blk */
	if(tally[idx_op2] == 0L) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.dp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
	      op2.dp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(tally[idx_op2] == 0L) op2.dp[idx_op2]=mss_val_dbl;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_INT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.lp[idx_op2]+=op1.lp[blk_off+idx_blk];
	tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.lp[idx_op1] != mss_val_lng){
	    op2.lp[idx_op2]+=op1.lp[idx_op1];
	    tally[idx_op2]++;
	  } /* end if */
	} /* end loop over idx_blk */
	if(tally[idx_op2] == 0L) op2.lp[idx_op2]=mss_val_lng;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.lp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.lp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_lng){
	      op2.lp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(tally[idx_op2] == 0L) op2.lp[idx_op2]=mss_val_lng;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.sp[idx_op2]+=op1.sp[blk_off+idx_blk];
	tally[idx_op2]=sz_blk;
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.sp[idx_op1] != mss_val_sht){
	    op2.sp[idx_op2]+=op1.sp[idx_op1];
	    tally[idx_op2]++;
	  } /* end if */
	} /* end loop over idx_blk */
	if(tally[idx_op2] == 0L) op2.sp[idx_op2]=mss_val_sht;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      short op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++) op2.sp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	  tally[idx_op2]=sz_blk;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_sht){
	      op2.sp[idx_op2]+=op1_2D[idx_op2][idx_blk];
	      tally[idx_op2]++;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(tally[idx_op2] == 0L) op2.sp[idx_op2]=mss_val_sht;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Do nothing except avoid compiler warnings */
    mss_val_chr=mss_val_chr;
    break;
  case NC_BYTE:
    /* Do nothing except avoid compiler warnings */
    mss_val_byt=mss_val_byt;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_avg_reduce_ttl() */

void
var_avg_reduce_min(nc_type type,long sz_op1,long sz_op2,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz_op1: I size (in elements) of op1
  long sz_op2: I size (in elements) of op2
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  ptr_unn op1: I values of first operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2))
  ptr_unn op2: O minimum value of each block of input operand
 */
{
  /* Routine to find minium values in each contiguous block of first operand and place
     result in corresponding element in second operand. Operands are assumed to have
     conforming types, but not dimensions or sizes. */

  /* var_avg_reduce_min() is derived from var_avg_reduce_ttl()
     Routines are very similar but tallies are not incremented
     See var_avg_reduce_ttl() for more algorithmic documentation
     var_avg_reduce_max() is derived from var_avg_reduce_min() */

#ifndef __GNUC__
  long blk_off;
  long idx_op1;
#endif /* !__GNUC__ */
  long idx_op2;
  long idx_blk;
  long sz_blk;

  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_long mss_val_lng=nco_long_CEWI;
  short mss_val_sht=short_CEWI;
  unsigned char mss_val_chr;
  signed char mss_val_byt;
  
  bool flg_mss=False;
  
  sz_blk=sz_op1/sz_op2;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_sht=*mss_val.sp; break;
    case NC_INT: mss_val_lng=*mss_val.lp; break;
    case NC_BYTE: mss_val_byt=*mss_val.bp; break;
    case NC_CHAR: mss_val_chr=*mss_val.cp; break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */
  } /* endif */
  
  switch(type){
  case NC_FLOAT:
    
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.fp[idx_op2]=op1.fp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.fp[idx_op2] > op1.fp[blk_off+idx_blk]) op2.fp[idx_op2]=op1.fp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.fp[idx_op1] != mss_val_flt) {
	    if(!flg_mss || op2.fp[idx_op2] > op1.fp[idx_op1]) op2.fp[idx_op2]=op1.fp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.fp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.fp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_flt) {
	      if(!flg_mss || op2.fp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    
    break;
  case NC_DOUBLE:
    
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.dp[idx_op2]=op1.dp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.dp[idx_op2] > op1.dp[blk_off+idx_blk]) op2.dp[idx_op2]=op1.dp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.dp[idx_op1] != mss_val_dbl) {
	    if(!flg_mss || (op2.dp[idx_op2] > op1.dp[idx_op1])) op2.dp[idx_op2]=op1.dp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.dp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.dp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk] ;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
	      if(!flg_mss || (op2.dp[idx_op2] > op1_2D[idx_op2][idx_blk])) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk];	    
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.lp[idx_op2]=op1.lp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.lp[idx_op2] > op1.lp[blk_off+idx_blk]) op2.lp[idx_op2]=op1.lp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.lp[idx_op1] != mss_val_lng){
	    if(!flg_mss || op2.lp[idx_op2] > op1.lp[idx_op1]) op2.lp[idx_op2]=op1.lp[idx_op1];
	    flg_mss= True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.lp[idx_op2]=mss_val_lng;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.lp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.lp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.lp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.lp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_lng){
	      if(!flg_mss || op2.lp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.lp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.lp[idx_op2]=mss_val_lng;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.sp[idx_op2]=op1.sp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.sp[idx_op2] > op1.sp[blk_off+idx_blk]) op2.sp[idx_op2]=op1.sp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.sp[idx_op1] != mss_val_sht){
	    if(!flg_mss || op2.sp[idx_op2] > op1.sp[idx_op1]) op2.sp[idx_op2]=op1.sp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.sp[idx_op2]=mss_val_sht;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      short op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.sp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.sp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_sht){
	      if(!flg_mss  || op2.sp[idx_op2] > op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.sp[idx_op2]=mss_val_sht;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Do nothing except avoid compiler warnings */
    mss_val_chr=mss_val_chr;
    break;
  case NC_BYTE:
    /* Do nothing except avoid compiler warnings */
    mss_val_byt=mss_val_byt;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end  switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_avg_reduce_min() */

void
var_avg_reduce_max(nc_type type,long sz_op1,long sz_op2,int has_mss_val,ptr_unn mss_val,ptr_unn op1,ptr_unn op2)
/* 
  nc_type type: I netCDF type of operands
  long sz_op1: I size (in elements) of op1
  long sz_op2: I size (in elements) of op2
  int has_mss_val: I flag for missing values
  ptr_unn mss_val: I value of missing value
  ptr_unn op1: I values of first operand (sz_op2 contiguous blocks of size (sz_op1/sz_op2))
  ptr_unn op2: O maximum value of each block of input operand
 */
{
  /* Routine to find maximium values in each contiguous block of first operand and place
     result in corresponding element in second operand. Operands are assumed to have
     conforming types, but not dimensions or sizes. */
  
  /* var_avg_reduce_min() is derived from var_avg_reduce_ttl()
     Routines are very similar but tallies are not incremented
     var_avg_reduce_max() is identical to var_avg_reduce_min() except the ">" have been swapped for "<" 
     See var_avg_reduce_ttl() for more algorithmic documentation
  */

#ifndef __GNUC__
  long blk_off;
  long idx_op1;
#endif /* !__GNUC__ */
  long idx_op2;
  long idx_blk;
  long sz_blk;
  
  double mss_val_dbl=double_CEWI;
  float mss_val_flt=float_CEWI;
  nco_long mss_val_lng=nco_long_CEWI;
  short mss_val_sht=short_CEWI;
  unsigned char mss_val_chr;
  signed char mss_val_byt;
  
  bool flg_mss=False;
  
  sz_blk=sz_op1/sz_op2;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  (void)cast_void_nctype(type,&op2);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  if(has_mss_val){
    switch(type){
    case NC_FLOAT: mss_val_flt=*mss_val.fp; break;
    case NC_DOUBLE: mss_val_dbl=*mss_val.dp; break;
    case NC_SHORT: mss_val_sht=*mss_val.sp; break;
    case NC_INT: mss_val_lng=*mss_val.lp; break;
    case NC_BYTE: mss_val_byt=*mss_val.bp; break;
    case NC_CHAR: mss_val_chr=*mss_val.cp; break;
    default: nco_dfl_case_nctype_err(); break;
    } /* end switch */
  } /* endif */
  
  switch(type){
  case NC_FLOAT:
    
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){ 
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.fp[idx_op2]=op1.fp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.fp[idx_op2] < op1.fp[blk_off+idx_blk]) op2.fp[idx_op2]=op1.fp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.fp[idx_op1] != mss_val_flt) {
	    if(!flg_mss || op2.fp[idx_op2] < op1.fp[idx_op1]) op2.fp[idx_op2]=op1.fp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      float op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.fp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.fp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.fp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_flt) {
	      if(!flg_mss || op2.fp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.fp[idx_op2]=op1_2D[idx_op2][idx_blk];
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.fp[idx_op2]=mss_val_flt;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    
    break;
  case NC_DOUBLE:
    
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.dp[idx_op2]=op1.dp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.dp[idx_op2] < op1.dp[blk_off+idx_blk]) op2.dp[idx_op2]=op1.dp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.dp[idx_op1] != mss_val_dbl) {
	    if(!flg_mss || (op2.dp[idx_op2] < op1.dp[idx_op1])) op2.dp[idx_op2]=op1.dp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      double op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.dp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.dp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.dp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk] ;
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_dbl){
	      if(!flg_mss || (op2.dp[idx_op2] < op1_2D[idx_op2][idx_blk])) op2.dp[idx_op2]=op1_2D[idx_op2][idx_blk];	    
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.dp[idx_op2]=mss_val_dbl;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_INT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.lp[idx_op2]=op1.lp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.lp[idx_op2] < op1.lp[blk_off+idx_blk]) op2.lp[idx_op2]=op1.lp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.lp[idx_op1] != mss_val_lng){
	    if(!flg_mss || op2.lp[idx_op2] < op1.lp[idx_op1]) op2.lp[idx_op2]=op1.lp[idx_op1];
	    flg_mss= True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.lp[idx_op2]=mss_val_lng;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      long op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.lp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.lp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.lp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.lp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_lng){
	      if(!flg_mss || op2.lp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.lp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.lp[idx_op2]=mss_val_lng;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_SHORT:
#ifndef __GNUC__
    /* ANSI-compliant branch */
    if(!has_mss_val){
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	op2.sp[idx_op2]=op1.sp[blk_off];
	for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	  if(op2.sp[idx_op2] < op1.sp[blk_off+idx_blk]) op2.sp[idx_op2]=op1.sp[blk_off+idx_blk];
      } /* end loop over idx_op2 */
    }else{
      for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	blk_off=idx_op2*sz_blk;
	flg_mss=False;
	for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	  idx_op1=blk_off+idx_blk;
	  if(op1.sp[idx_op1] != mss_val_sht){
	    if(!flg_mss || op2.sp[idx_op2] < op1.sp[idx_op1]) op2.sp[idx_op2]=op1.sp[idx_op1];
	    flg_mss=True;
	  } /* end if */
	} /* end loop over idx_blk */
	if(!flg_mss) op2.sp[idx_op2]=mss_val_sht;
      } /* end loop over idx_op2 */
    } /* end else */
#else /* __GNUC__ */
    /* Compiler supports local automatic arrays. Not ANSI-compliant, but more elegant. */
    if(True){
      short op1_2D[sz_op2][sz_blk];
      
      (void)memcpy((void *)op1_2D,(void *)(op1.sp),sz_op1*nco_typ_lng(type));
      
      if(!has_mss_val){
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  op2.sp[idx_op2]=op1_2D[idx_op2][0];
	  for(idx_blk=1;idx_blk<sz_blk;idx_blk++) 
	    if(op2.sp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];
	} /* end loop over idx_op2 */
      }else{
	for(idx_op2=0;idx_op2<sz_op2;idx_op2++){
	  flg_mss=False;
	  for(idx_blk=0;idx_blk<sz_blk;idx_blk++){
	    if(op1_2D[idx_op2][idx_blk] != mss_val_sht){
	      if(!flg_mss  || op2.sp[idx_op2] < op1_2D[idx_op2][idx_blk]) op2.sp[idx_op2]=op1_2D[idx_op2][idx_blk];	      
	      flg_mss=True;
	    } /* end if */
	  } /* end loop over idx_blk */
	  if(!flg_mss) op2.sp[idx_op2]=mss_val_sht;
	} /* end loop over idx_op2 */
      } /* end else */
    } /* end if */
#endif /* __GNUC__ */
    break;
  case NC_CHAR:
    /* Do nothing except avoid compiler warnings */
    mss_val_chr=mss_val_chr;
    break;
  case NC_BYTE:
    /* Do nothing except avoid compiler warnings */
    mss_val_byt=mss_val_byt;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end  switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end var_avg_reduce_max() */

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
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_normalize_real(&sz,&has_mss_val,mss_val.fp,tally,op1.fp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=tally[idx];
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.fp[idx]/=tally[idx]; else op1.fp[idx]=mss_val_flt;
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
    break;
  case NC_DOUBLE:
#ifdef USE_FORTRAN_ARITHMETIC
    (void)FORTRAN_normalize_double_precision(&sz,&has_mss_val,mss_val.dp,tally,op1.dp);
#else /* !USE_FORTRAN_ARITHMETIC */
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=tally[idx];
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++) if(tally[idx] != 0L) op1.dp[idx]/=tally[idx]; else op1.dp[idx]=mss_val_dbl;
    } /* end else */
#endif /* !USE_FORTRAN_ARITHMETIC */
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

