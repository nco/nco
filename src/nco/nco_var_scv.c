/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_var_scv.c,v 1.1 2002-04-27 00:04:27 zender Exp $ */

/* Purpose: NCO utilities for arithmetic involving var and scv types */

/* Copyright (C) 1995--2002 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the NetCDF and HDF libraries 
   and distribute the resulting executables under the terms of the GPL, 
   but in addition obeying the extra stipulations of the netCDF and 
   HDF library licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   The file LICENSE contains the GNU General Public License, version 2
   It may be viewed interactively by typing, e.g., ncks -L

   The author of this software, Charlie Zender, would like to receive
   your suggestions, improvements, bug-reports, and patches for NCO.
   Please contact the project at http://sourceforge.net/projects/nco or by writing

   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100
 */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, etc. */

#include <netcdf.h> /* netCDF definitions */
#include "nco.h" /* netCDF operator universal def'ns */

void
var_scv_add(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /*	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
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
    float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]+=scv_flt;
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]+=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    double scv_dpl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]+=scv_dpl;
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]+=scv_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]+=scv_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]+=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short scv_shrt=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]+=scv_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]+=scv_shrt;
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_add() */

void
var_scv_sub(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /*	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
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
    float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]-=scv_flt;
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]-=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    double scv_dpl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]-=scv_dpl;
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]-=scv_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]-=scv_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]-=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short scv_shrt=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]-=scv_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]-=scv_shrt;
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_sub() */

void
var_scv_multiply(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
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
    float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]*=scv_flt;
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]*=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    double scv_dpl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]*=scv_dpl;
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]*=scv_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]*=scv_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]*=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short scv_shrt=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]*=scv_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]*=scv_shrt;
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_multiply() */

void
var_scv_divide(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
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
    float scv_flt=scv->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=scv_flt;
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]/=scv_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    double scv_dpl=scv->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=scv_dpl;
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]/=scv_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=scv_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]/=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short scv_shrt=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=scv_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]/=scv_shrt;
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_scv_divide() */

ncap_var_scv_modulus(var_sct *var,scv_sct scv)
{
  /* Purpose: var % scv, take modulus of each element of var with value in scv */
  
  var_sct *var_nsw;
  (void)ncap_var_scv_cnf_typ_hgh_prc(&var,&scv);
  var_nsw=var_dpl(var);
  (void)var_scv_modulus(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&scv);
  
  return var_nsw;
} /* ncap_var_scv_modulus */

var_sct *
ncap_var_abs(var_sct *var)
{
  /* Purpose: Find absolute value of each element of var */
  var_sct *var_nsw;
  var_nsw=var_dpl(var);
  (void)var_abs(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val);
  return var_nsw;
} /* end ncap_var_abs */

void 
var_scv_modulus(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,scv_sct *scv)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        scv_sct scv: I pointer to scalar value (second operand) */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Take modulus of  all values in op1 by value in attrib
     Store result in op1  */    
  
  /* Modulus  is currently defined as op1:=op1%scv */  
  
  extern float fmodf(float,float); /* Cannot insert fmodf in ncap_sym_init() because it takes two arguments TODO #20 */
  extern float fabsf(float); /* Sun math.h does not include fabsf() prototype */

  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{ 
    float scv_flt=fabsf(scv->val.f);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=fmodf(op1.fp[idx],scv_flt);
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=fmodf(op1.fp[idx],scv_flt);
      } /* end for */
    } /* end else */
  }
  break; 
  case NC_DOUBLE:{
    double scv_dpl=fabs(scv->val.d);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=fmod(op1.dp[idx],scv_dpl);
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fmod(op1.dp[idx],scv_dpl);  
      } /* end for */
    } /* end else */
    break;
  }
  
  case NC_INT:{
    nco_long scv_lng=scv->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]%=scv_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]%=scv_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short scv_shrt=scv->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]%=scv_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]%=scv_shrt;
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
} /* end var_scv_modulus */

void
var_abs(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
     */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Find the absolute value of all numbers in op1
     Store result in first operand */    
  
  /*   op1:=abs(op1) */  
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  extern float fabsf(float); /* Sun math.h does not include fabsf() prototype */
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=fabsf(op1.fp[idx]);
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=fabsf(op1.fp[idx]); 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=fabs(op1.dp[idx]);
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fabs(op1.dp[idx]);
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]=abs(op1.lp[idx]);
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]=abs(op1.lp[idx]); 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) if(op1.sp[idx] < 0 ) op1.sp[idx]=-op1.sp[idx] ;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt && op1.sp[idx] < 0 ) op1.sp[idx]=-op1.sp[idx];
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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  
  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */
  
} /* end var_abs() */

var_sct *
ncap_var_scv_power(var_sct *var_in,scv_sct scv)
{
  /* Purpose: Raise var to the power in scv
     All values converted to type double before operation */
  long idx;
  long sz;
  ptr_unn op1;
  var_sct *var;

  /* Promote scv and var to NC_FLOAT */
  if(var_in->type < NC_FLOAT) var_in=var_conform_type(NC_FLOAT,var_in);
  var=var_dpl(var_in);
  (void)scv_conform_type(var->type,&scv);
  
  op1=var->val;
  sz=var->sz;
  (void)cast_void_nctype(var->type,&op1);
  if(var->has_mss_val) (void)cast_void_nctype(var->type,&(var->mss_val));
  
  switch(var->type){ 
  case NC_DOUBLE: {
    double scv_dpl=scv.val.d;
    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=pow(op1.dp[idx],scv_dpl);
    }else{
      double mss_val_dbl=*(var->mss_val.dp); /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
        if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=pow(op1.dp[idx],scv_dpl);
      } /* end for */
    } /* end else */
   break;
  }
  case NC_FLOAT: {
    float scv_flt=scv.val.f;
    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=powf(op1.fp[idx],scv_flt);
    }else{
      float mss_val_flt=*(var->mss_val.fp); /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
        if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=powf(op1.fp[idx],scv_flt);
      } /* end for */
    } /* end else */
   break;
  }
  default: nco_dfl_case_nctype_err(); break;
  }/* end switch */

  if(var->has_mss_val) (void)cast_nctype_void(var->type,&(var->mss_val));
  return var;
} /* end ncap_var_scv_power */
