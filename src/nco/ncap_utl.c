/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap_utl.c,v 1.26 2002-01-04 16:05:15 hmb Exp $ */

/* Purpose: Utilities for ncap operator */

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
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */
#include <string.h>             /* strcmp. . . */
#include <time.h>               /* machine time */
#include <unistd.h>             /* POSIX stuff */

#include <netcdf.h>             /* netCDF definitions */
#include "nco_netcdf.h"         /* netCDF wrapper functions */

#include "nco.h"                /* netCDF operator universal def'ns */
#include "ncap.h"               /* include prs_arg */

var_sct *
ncap_var_init(char *var_nm,prs_sct *prs_arg)
{
  /* Purpose: Initialize var structure, retrieve var values from disk */
  
  int var_id;
  int rcd;
  int fl_id;
  var_sct *vara;
  
  /* Check output file for var */  
  rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
  
  if( rcd == NC_NOERR ){
    fl_id=prs_arg->out_id;
  }else{
    /* Check input file for id */
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
    if( rcd == NC_NOERR ){
      fl_id=prs_arg->in_id;
    }else{
      (void)fprintf(stderr,"can't find %s in %s or %s\n",var_nm,prs_arg->fl_in,prs_arg->fl_out);     
      return (var_sct*)NULL;
    }/* end else */
  } /* end else */
  
  if(dbg_lvl_get() > 1) (void)fprintf(stderr,"VAR: retriving %s from disk\n",var_nm);  
  vara=var_fll(fl_id,var_id,var_nm,prs_arg->dmn,prs_arg->nbr_dmn_xtr);
  vara->nm=nco_malloc((strlen(var_nm)+1)*sizeof(char)); strcpy(vara->nm,var_nm);
  vara->tally=(long *)malloc(vara->sz*nco_typ_lng(NC_INT));
  (void)zero_long(vara->sz,vara->tally);
  vara->val.vp=(void *)malloc(vara->sz*nco_typ_lng(vara->type));
  /* Retrieve variable values from disk into memory */
  (void)var_get(fl_id,vara);
  /* (void)var_free(var_nm);*/
  /* free(var_nm->nm);*/
  /* vara=var_upk(vara); */
  return vara;
  
} /* end ncap_var_init */

int 
ncap_var_write(var_sct *var, prs_sct *prs_arg)
{
  /* purpose: Define var in the output file and write the variables */
  int var_out_id;
  
  (void)nco_redef(prs_arg->out_id);
  
  /* define variable */   
  (void)nco_def_var(prs_arg->out_id,var->nm,var->type,var->nbr_dim,var->dmn_id,&var_out_id);
  /* Put missing value */  
  if(var->has_mss_val)
    (void)nco_put_att(prs_arg->out_id,var_out_id,"missing_value",var->type,1,var->mss_val.vp);
  (void)nco_enddef(prs_arg->out_id);
  
  /* Write variable */ 
  if(var->nbr_dim == 0 ){
    (void)nco_put_var1(prs_arg->out_id,var_out_id,0L,var->val.vp,var->type);
  }else{
    (void)nco_put_vara(prs_arg->out_id,var_out_id,var->srt,var->cnt,var->val.vp,var->type);
  } /* end else */
  
  return 1;
} /* end ncap_var_write */

sym_sct *
ncap_sym_init(char *name,double (*function)())
{ 
  /* purpose: Allocate space for sym_sct then initialize */
  sym_sct *symbol;
  symbol=malloc(sizeof(sym_sct));
  symbol->nm=strdup(name);
  symbol->fnc= function;
  return symbol;
} /* end ncap_sym_init */

parse_sct 
ncap_ptr_unn_2_attribute(nc_type type, ptr_unn val)
{
  /* convert a ptr_unn to an attribute parse_sct */
  /* Assumes theat val is initially cast to void */
  /* Note does not convert cp ( strings) as these */
  /* are not handled by parse_sct */
  /* Note: a netCDF attribute can contain MULTIPLE values */
  /* only the FIRST value in the memory block is converted */
  
  parse_sct a;
  (void)cast_void_nctype(type,&val);
  switch(type){
  case NC_FLOAT: a.val.f=*val.fp ; break;
  case NC_DOUBLE: a.val.d =*val.dp ; break;
  case NC_INT:    a.val.l =*val.lp ; break;
  case NC_SHORT:  a.val.s=*val.sp  ; break;
  case NC_BYTE:   a.val.b =*val.bp;  break;
  case NC_CHAR:   break; /* do nothing */
  default: nco_dfl_case_nctype_err(); break;
  } 
  a.type=type;
  /* don't have to uncast pointer as we are working with acopy */
  return a;
} /* end ncap_ptr_unn_2_attribute */

ptr_unn
ncap_attribute_2_ptr_unn(parse_sct a)
{
  /* converts a parse_sct to a ptr_unn */
  /* It mallocs the appropriate space for the single type */
  /* n.b It doesn't do strings */
  ptr_unn val;
  nc_type type=a.type;
  val.vp=(void *)nco_malloc(nco_typ_lng(type));
  (void)cast_void_nctype(type,&val);
  
  switch(type){
  case NC_FLOAT: *val.fp=a.val.f; break;
  case NC_DOUBLE: *val.dp=a.val.d; break;
  case NC_INT:   *val.lp=a.val.l; break;
  case NC_SHORT: *val.sp=a.val.s; break;
  case NC_BYTE:  *val.bp=a.val.b;  break;
  case NC_CHAR:   break; /* do nothing */
  default: nco_dfl_case_nctype_err(); break;
  } 
  (void)cast_nctype_void(type,&val);
  return val;
} /* end ncap_attribute_2_ptr_unn */

var_sct *
ncap_var_var_add(var_sct *var_1,var_sct *var_2)
     /* 
	var_sct *var_1: input variable structure containing first operand
	var_sct *var_2: input variable structure containing second operand
	var_sct *ncap_var_add(): output sum of input variables
     */
{
  /* Routine called by parser */
  var_sct *var_sum;
  
  (void)ncap_var_retype(var_1,var_2);
  var_sum=var_dpl(var_2);
  (void)ncap_var_conform_dim(var_1,var_sum);
  (void)var_add(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->tally,var_1->val,var_sum->val);
  
  return var_sum;
} /* end ncap_var_add() */

var_sct *
ncap_var_var_sub(var_sct *var_2,var_sct *var_1)
     /* 
	var_sct *var_1: input variable structure containing first operand
	var_sct *var_2: input variable structure containing second operand
	var_sct *ncap_var_sub(): output var_2 - var_1 of input variables
     */
{
  /* Routine called by parser */
  var_sct *var_sum;
  
  (void)ncap_var_retype(var_1,var_2);
  var_sum=var_dpl(var_2);
  (void)ncap_var_conform_dim(var_1,var_sum);
  (void)var_subtract(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_sum->val);
  
  return var_sum;
} /* end ncap_var_add() */

var_sct *
ncap_var_var_multiply(var_sct *var_1,var_sct *var_2)
     /* 
	var_sct *var_1: input variable structure containing first operand
	var_sct *var_2: input variable structure containing second operand
	var_sct *ncap_var_multipy(): output multiplacation of individual elements
     */
{
  /* Routine called by parser */
  var_sct *var_sum;
  (void)ncap_var_retype(var_1,var_2);
  var_sum=var_dpl(var_2);
  (void)ncap_var_conform_dim(var_1,var_sum);
  (void)var_multiply(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_sum->val);
  
  return var_sum;
} /* end ncap_var_add() */

var_sct *
ncap_var_attribute_multiply(var_sct *var,parse_sct attribute)
{
  /* purpose: Multiply variable by value in attribute */
  var_sct *var_sum;
  var_sum=var_dpl(var);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_multiply(var->type,var->sz,var->has_mss_val,var->mss_val,var_sum->val,&attribute);
  
  return var_sum;
} /* end ncap_var_attribute_multiply */

var_sct *
ncap_var_attribute_power(var_sct *var_in,parse_sct attribute)
{
  /* purpose: raise to the power in attribute, each value in var */
  /* All values converted to type double before operation        */
  long idx;
  long sz;
  double att_dpl;
  ptr_unn op1;
  var_sct *var;
  /* convert attribute and var to type DOUBLE */
  (void)ncap_attribute_conform_type(NC_DOUBLE,&attribute);
  var_in=var_conform_type(NC_DOUBLE,var_in);
  var=var_dpl(var_in);
  op1=var->val;
  (void)cast_void_nctype(NC_DOUBLE,&op1);
  if(var->has_mss_val) (void)cast_void_nctype(NC_DOUBLE,&(var->mss_val));
  
  att_dpl=attribute.val.d;
  sz=var->sz;
  if(!var->has_mss_val){
    for(idx=0;idx<sz;idx++) op1.dp[idx]=pow(op1.dp[idx],att_dpl);
    
  }else{
    double mss_val_dbl=*(var->mss_val.dp); /* Temporary variable reduces dereferencing */
    for(idx=0;idx<sz;idx++){
      if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=pow(op1.dp[idx],att_dpl);
    } /* end for */
  } /* end else */
  
  if(var->has_mss_val) (void)cast_nctype_void(NC_DOUBLE,&(var->mss_val));
  return var;
  
} /* end ncap_var_attribute_power */

var_sct *
ncap_var_function(var_sct *var_in, double(*fnc)())
{
  /* purpose: evalue fnc(var) for each value in variable */
  long idx;
  long sz;
  ptr_unn op1;
  var_sct *var;
  /* convert attribute and var to type DOUBLE */
  var_in=var_conform_type(NC_DOUBLE,var_in);
  var=var_dpl(var_in);
  op1=var->val;
  (void)cast_void_nctype(NC_DOUBLE,&op1);
  if(var->has_mss_val) (void)cast_void_nctype(NC_DOUBLE,&(var->mss_val));
  
  sz=var->sz;
  if(!var->has_mss_val){
    for(idx=0;idx<sz;idx++) op1.dp[idx]=fnc(op1.dp[idx]);
    
  }else{
    double mss_val_dbl=*(var->mss_val.dp); /* Temporary variable reduces dereferencing */
    for(idx=0;idx<sz;idx++){
      if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fnc(op1.dp[idx]);
    } /* end for */
  } /* end else */
  
  /* (void)cast_nctype_void(NC_DOUBLE,&var->val); */
  if(var->has_mss_val) (void)cast_nctype_void(NC_DOUBLE,&(var->mss_val));
  return var;
  
}/* end ncap_var_function */

var_sct *
ncap_var_attribute_add(var_sct *var,parse_sct attribute)
{
  /* purpose: add the value in attribute to each element of var */
  var_sct *var_sum;
  var_sum=var_dpl(var);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_add(var->type,var->sz,var->has_mss_val,var->mss_val,var_sum->val,&attribute);
  
  return var_sum;
  
} /* end ncap_var_attribute_add */

var_sct *
ncap_var_attribute_sub(var_sct *var,parse_sct attribute)
{
  /* purpose: Subtract the value in attribute from each element of var */
  var_sct *var_sum;
  var_sum=var_dpl(var);
  (void)ncap_attribute_minus(&attribute);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_add(var->type,var->sz,var->has_mss_val,var->mss_val,var_sum->val,&attribute);
  
  return var_sum;
} /* end ncap_var_attriibute_sub */

var_sct *
ncap_var_attribute_divide(var_sct *var,parse_sct attribute)
{
  /* purpose: Divide each element of var by value in attribute */
  var_sct *var_sum;
  var_sum=var_dpl(var);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_divide(var->type,var->sz,var->has_mss_val,var->mss_val,var_sum->val,&attribute);
  
  return var_sum;
} /* end ncap_var_attribute_divide */

var_sct *
ncap_var_attribute_modulus(var_sct *var,parse_sct attribute)
{
  /* purpose: var % attribute , Take the modulus of each element of var with the value in attribute */
  
  var_sct *var_sum;
  var_sum=var_dpl(var);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_modulus(var->type,var->sz,var->has_mss_val,var->mss_val,var_sum->val,&attribute);
  
  return var_sum;
} /* ncap_var_attribute_modulus */

var_sct *
ncap_var_abs(var_sct *var)
{
  /* purpose: Find the absolute value of each element of var */
  var_sct *var_sum;
  var_sum=var_dpl(var);
  (void)var_abs(var->type,var->sz,var->has_mss_val,var->mss_val,var_sum->val);
  return var_sum;
} /* end ncap_var_abs */

void
var_attribute_add(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,parse_sct *attribute)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        attribute: 
     */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: multiply all values in op1 by value in attrib
     Store result in first operand */    
  
  /* Addition  is currently defined as op1:=op1+attribute */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    float att_flt=attribute->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]+=att_flt;
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]+=att_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    double att_dpl=attribute->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]+=att_dpl;
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]+=att_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    nco_long att_lng=attribute->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]+=att_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]+=att_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short att_shrt=attribute->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]+=att_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]+=att_shrt;
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
  
} /* end var_attribute_add() */

void
var_attribute_multiply(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,parse_sct *attribute)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        attribute:   parse_sct containing multiplation value
     */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: multiply all values in op1 by value in attrib
     Store result in first operand */    
  
  /* Multiplication  is currently defined as op1:=op1*attribute */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    float att_flt=attribute->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]*=att_flt;
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]*=att_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    double att_dpl=attribute->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]*=att_dpl;
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]*=att_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    nco_long att_lng=attribute->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]*=att_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]*=att_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short att_shrt=attribute->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]*=att_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]*=att_shrt;
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
  
} /* end var_attribute_multiply() */

void
var_attribute_divide(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,parse_sct *attribute)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        attribute: 
     */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Divide all values in op1 by value in attrib
     Store result in first operand */    
  
  /* Addition  is currently defined as op1:=op1/attribute */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{
    float att_flt=attribute->val.f;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]/=att_flt;
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]/=att_flt; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_DOUBLE:{
    double att_dpl=attribute->val.d;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]/=att_dpl;
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]/=att_dpl;  
      } /* end for */
    } /* end else */
    break;
  }
  case NC_INT:{
    nco_long att_lng=attribute->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]/=att_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]/=att_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short att_shrt=attribute->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]/=att_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]/=att_shrt;
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
  
} /* end var_attribute_divide() */

void 
var_attribute_modulus(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,parse_sct *attribute)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        attribute: 
     */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Take modulus of  all values in op1 by value in attrib
     Store result in op1  */    
  
  /* Modulus  is currently defined as op1:=op1%attribute */  
  
  long idx;
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(has_mss_val) (void)cast_void_nctype(type,&mss_val);
  
  switch(type){
  case NC_FLOAT:{ 
    float att_flt=fabsf(attribute->val.f);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=fmodf(op1.fp[idx],att_flt);
    }else{
      float mss_val_flt=*mss_val.fp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=fmodf(op1.fp[idx],att_flt);
      } /* end for */
    } /* end else */
  }
  break; 
  case NC_DOUBLE:{
    double att_dpl=fabs(attribute->val.d);
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=fmod(op1.dp[idx],att_dpl);
    }else{
      double mss_val_dbl=*mss_val.dp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=fmod(op1.dp[idx],att_dpl);  
      } /* end for */
    } /* end else */
    break;
  }
  
  case NC_INT:{
    nco_long att_lng=attribute->val.l;
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.lp[idx]%=att_lng;
    }else{
      long mss_val_lng=*mss_val.lp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.lp[idx] != mss_val_lng) op1.lp[idx]%=att_lng; 
      } /* end for */
    } /* end else */
    break;
  }
  case NC_SHORT:{
    short att_shrt=attribute->val.s; 
    if(!has_mss_val){
      for(idx=0;idx<sz;idx++) op1.sp[idx]%=att_shrt;
    }else{
      short mss_val_shrt=*mss_val.sp; /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
	if(op1.sp[idx] != mss_val_shrt) op1.sp[idx]%=att_shrt;
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
  
} /* end var_attribute_modulus */

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

int 
ncap_var_retype(var_sct* vara, var_sct* varb)
{
  /* Purpose: Convert a variable if necessary so the vars are of the same type */
  
  if( vara->type == varb->type) return vara->type;
  if(vara->type > varb->type){
    varb=var_conform_type(vara->type,varb);
    return vara->type;
  }else{
    vara=var_conform_type(varb->type,vara);
    return varb->type;
  }
} /* end ncap_var_retype */

bool
ncap_var_conform_dim(var_sct* vara,var_sct *varb)
{
  /* Purpose: Make sure the vars have conforming variables. If this isn't possible then die */
  bool MUST_CONFORM=True;
  bool DO_CONFORM;
  if(vara->nbr_dim > varb->nbr_dim) varb=var_conform_dim(vara,varb,vara,MUST_CONFORM,&DO_CONFORM);
  else
    vara=var_conform_dim(varb,vara,varb,MUST_CONFORM,&DO_CONFORM);
  
  if(!DO_CONFORM){
    (void)fprintf(stderr,"%s: Variables don't have have conforming dimensions. Cannot proceed with operation\n",prg_nm_get());
    exit(EXIT_FAILURE);
  }
  return DO_CONFORM;
} /* end ncap_var_conform_dim */

int 
ncap_retype(parse_sct *a,parse_sct *b)
{
  /* Purpose: Convert an attribute if necessary so the attributes  are of the same type */
  if(a->type == b->type) return a->type;
  if((a->type) > (b->type)){ (void)ncap_attribute_conform_type(a->type,b);}
  else {(void)ncap_attribute_conform_type(b->type,a);}
  
  return a->type;    
} /* end ncap_retype */

int  
ncap_attribute_conform_type(nc_type type_new,parse_sct *a)
{
  /* purpose: Convert an attribute to type_new using implicit C convertions */
  nc_type type_old=a->type;
  
  parse_sct b;
  switch (type_new){ 
    
  case NC_BYTE:
    switch(type_old){
    case NC_FLOAT: b.val.b=(signed char)(a->val).f; break; 
    case NC_DOUBLE: b.val.b=(signed char)(a->val).d; break; 
    case NC_INT: b.val.b=(a->val).l; break;
    case NC_SHORT: b.val.b=(a->val).s; break;
    case NC_BYTE: b.val.b=(a->val).b; break;
    case NC_CHAR: break;
    case NC_NAT:  break;    
    } break;
    
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_SHORT:
    switch(type_old){
    case NC_FLOAT: b.val.s=(short)(a->val).f; break; 
    case NC_DOUBLE: b.val.s=(short)(a->val).d; break; 
    case NC_INT: b.val.s=(a->val).l; break;
    case NC_SHORT: b.val.s=(a->val).s; break;
    case NC_BYTE: b.val.s=(a->val).b; break;
    case NC_CHAR: break;
    case NC_NAT:  break;    
    } break;
    
    
  case NC_INT:
    switch(type_old){
    case NC_FLOAT: b.val.l=(long)(a->val).f; break; 
    case NC_DOUBLE: b.val.l=(long)(a->val).d; break; 
    case NC_INT:    b.val.l =a->val.l; break;
    case NC_SHORT: b.val.l=(a->val).s; break;
    case NC_BYTE: b.val.l=(a->val).b; break;
    case NC_CHAR: break;
    case NC_NAT:  break;
    } break;
    
  case NC_FLOAT:
    switch(type_old){
    case NC_FLOAT:  b.val.f=(a->val).f; break; 
    case NC_DOUBLE: b.val.f=(a->val).d; break; 
    case NC_INT: (b.val).f=(a->val).l; break;
    case NC_SHORT: (b.val).f=(a->val).s; break;
    case NC_BYTE: (b.val).f=(a->val).b; break;
    case NC_CHAR: break;
    case NC_NAT:  break;    
    } break;
    
  case NC_DOUBLE:
    switch(type_old){
    case NC_FLOAT:  b.val.d=(a->val).f; break; 
    case NC_DOUBLE: b.val.d =(a->val).d;  break; 
    case NC_INT:    b.val.d=(a->val).l; break;
    case NC_SHORT:  b.val.d=(a->val).s; break;
    case NC_BYTE:   b.val.d=(a->val).b; break;
    case NC_CHAR: break;
    case NC_NAT:  break;    
    } break;
  default: nco_dfl_case_nctype_err(); break;
    
  } /* end switch */
  b.type=type_new;
  *a=b;
  return 1;      
} /* end ncap_attribute_conform_type */

parse_sct  
ncap_attribute_calc(parse_sct a, char op, parse_sct b)
{
  /* Purpose: Calculate (a op b) . n.b Attributes must be of the same type */
  parse_sct c;
  c.type=a.type;
  switch(c.type){ 
    
  case NC_BYTE:
    switch(op){
      case'+': c.val.b=a.val.b + b.val.b;break;
      case'-': c.val.b=a.val.b - b.val.b;break;
      case'/': c.val.b=a.val.b / b.val.b;break;
      case'*': c.val.b=a.val.b * b.val.b;break;
      case'%': c.val.b=a.val.b % b.val.b;break;
    } break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_SHORT:
    switch(op){
      case'+': c.val.s=a.val.s + b.val.s;break;
      case'-': c.val.s=a.val.s - b.val.s;break;
      case'/': c.val.s=a.val.s / b.val.s;break;
      case'*': c.val.s=a.val.s * b.val.s;break;
      case'%': c.val.s=a.val.s % b.val.s;break;
    } break;
  case NC_INT:
    switch(op){
      case'+': c.val.l=a.val.l + b.val.l;break;
      case'-': c.val.l=a.val.l - b.val.l;break;
      case'/': c.val.l=a.val.l / b.val.l;break;
      case'*': c.val.l=a.val.l * b.val.l;break;
      case'%': c.val.l=a.val.l % b.val.l;break;
    } break;
    
  case NC_FLOAT:
    switch(op){
      case'+': c.val.f=a.val.f + b.val.f;break;
      case'-': c.val.f=a.val.f - b.val.f;break;
      case'/': c.val.f=a.val.f / b.val.f;break;
      case'*': c.val.f=a.val.f * b.val.f;break;
      case'%': c.val.f=fmodf(a.val.f, fabsf(b.val.f));break;
    } break;
    
  case NC_DOUBLE:
    switch(op){
      case'+': c.val.d=a.val.d + b.val.d;break;
      case'-': c.val.d=a.val.d - b.val.d;break;
      case'/': c.val.d=a.val.d / b.val.d;break;
      case'*': c.val.d=a.val.d * b.val.d;break;
      case'%': c.val.d=fmod(a.val.d, fabs(b.val.d));break;
    } break;
  default: nco_dfl_case_nctype_err(); break;
  }/* end switch */    
  
  return c;
} /* end ncap_attribute_calc_type */

parse_sct
ncap_attribute_abs(parse_sct a)
{
  /* Purpose: Find the absolute value of an attribute */
  
  parse_sct b;
  b.type=a.type;
  switch(a.type){ 
  case NC_BYTE:
    b.val.b=( (a.val.b >= 0) ? a.val.b : -a.val.b) ;
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_SHORT:
    b.val.s=( (a.val.s >= 0) ? a.val.s : -a.val.s) ;
    break;
  case NC_INT:
    b.val.l=abs(a.val.l);
    break;            
  case NC_FLOAT:
    b.val.f=fabsf(a.val.f);
    break;
  case NC_DOUBLE:
    b.val.d=fabs(a.val.d);
    break;
  default: nco_dfl_case_nctype_err(); break;    
  } /* end switch */
  return b;
} /* end ncap_attribute_abs */
int 
ncap_attribute_minus(parse_sct *a)
{
  switch(a->type){ 
  case NC_BYTE:
    a->val.b= -a->val.b; 
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_SHORT:
    a->val.s=-a->val.s;
    break;
  case NC_INT:
    a->val.l=-a->val.l;
    break;            
  case NC_FLOAT:
    a->val.f=-a->val.f;
    break;
  case NC_DOUBLE:
    a->val.d=-a->val.d;
    break;
  default: nco_dfl_case_nctype_err(); break;   
  }/* end switch */    
  return a->type;
}

nm_id_sct *
var_lst_copy(nm_id_sct *xtr_lst,int n)
{
  /* Purpose: Copy xtr_lst and return the new list */
  int i;
  nm_id_sct *xtr_new_lst;
  
  if(n == 0 ) return NULL;
  xtr_new_lst=(nm_id_sct*)nco_malloc(n*sizeof(nm_id_sct));
  for (i =0 ; i< n ; i++){
    xtr_new_lst[i].nm=strdup(xtr_lst[i].nm);
    xtr_new_lst[i].id=xtr_lst[i].id;
  }
  return xtr_new_lst;            
} /* end var_lst_copy */

nm_id_sct *
var_lst_sub(int in_id,nm_id_sct *xtr_lst,int *nbr_xtr,nm_id_sct *xtr_lst_b,int nbr_lst_b)
{
  /* Purpose: subtract from xtr_lst any elements from xtr_lst_b which are present */
  /* and return a new list */
  int i;
  int j;
  int n=0;
  
  bool match;
  nm_id_sct *xtr_new_lst=NULL;
  
  if(*nbr_xtr == 0 ) return xtr_lst;
  
  xtr_new_lst=(nm_id_sct*)nco_malloc((*nbr_xtr)*sizeof(nm_id_sct));  
  for(i =0 ; i < *nbr_xtr ; i++){
    match=False;
    for(j=0; j < nbr_lst_b ; j++)
      if(!strcmp(xtr_lst[i].nm,xtr_lst_b[j].nm)){ match=True ; break; }
    if(match) continue;
    xtr_new_lst[n].nm=strdup(xtr_lst[i].nm);
    xtr_new_lst[n++].id=xtr_lst[i].id;
  }
  *nbr_xtr=n;
  return xtr_new_lst;      
  
}/* end var_lst_sub */

nm_id_sct *
var_lst_add(int in_id,nm_id_sct *xtr_lst,int *nbr_xtr,nm_id_sct *xtr_lst_a,int nbr_lst_a)
{
  /* Purpose: Add to xtr_lst any elements from xtr_lst_b which are not already present */
  /* and return a new list */
  int i;
  int j;
  int n;
  
  nm_id_sct *xtr_new_lst;
  
  bool match;
  
  
  if(*nbr_xtr >0 ){
    xtr_new_lst=(nm_id_sct*)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
    n=*nbr_xtr;
    for (i =0 ; i< *nbr_xtr ; i++){
      xtr_new_lst[i].nm=strdup(xtr_lst[i].nm);
      xtr_new_lst[i].id=xtr_lst[i].id;
    }
  }else{
    *nbr_xtr=nbr_lst_a;
    return var_lst_copy(xtr_lst_a,nbr_lst_a);
  }/* end if */
  
  
  for( i=0 ; i < nbr_lst_a ; i++){
    match=False;
    for(j=0 ; j < *nbr_xtr ;j++)
      if(!strcmp(xtr_lst[j].nm,xtr_lst_a[i].nm)){ match=True ; break;}
    if(match) continue;
    xtr_new_lst=(nm_id_sct*)nco_realloc(xtr_new_lst,(n+1)*sizeof(nm_id_sct));
    xtr_new_lst[n].nm=strdup(xtr_lst_a[i].nm);
    xtr_new_lst[n++].id=xtr_lst_a[i].id;
  }
  
  *nbr_xtr=n;
  return xtr_new_lst;            
} /* var_lst_add */

void 
ncap_initial_scan(prs_sct *prs_arg,char *spt_arg_cat, nm_id_sct** xtr_lst_a,int *nbr_lst_a,
		  nm_id_sct** xtr_lst_b,int *nbr_lst_b,nm_id_sct** xtr_lst_c, int *nbr_lst_c)
{
  /* Purpose: Do a scan of the command script and return three lists  */
  /* list a -- variables on the RHS which are present in the input file*/
  /* list b -- variables on the LHS which are present in the input file*/
  /* list c -- variables of attributes on the LHS which are present in the input file*/
  
#include "ncap.tab.h"           /* TOKENS and YYSTYPE - produced by Bison */
                                /* We need these because we are calling the scanner */
  int i;                      
  int itoken;
  int n_lst_a=0;
  int n_lst_b=0;
  int n_lst_c=0;
  int var_id;
  bool match;
  char *var_nm;  
  
  nm_id_sct *lst_a=NULL_CEWI;
  nm_id_sct *lst_b=NULL_CEWI;
  nm_id_sct *lst_c=NULL_CEWI;
  
  YYSTYPE lvalp;
  extern FILE *yyin;
  extern int yylex(YYSTYPE*,prs_sct *);
  
  if(spt_arg_cat){
    yy_scan_string(spt_arg_cat);
  }else{
    /* Open script file for reading */
    if((yyin=fopen(prs_arg->fl_spt,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",prg_nm_get(),prs_arg->fl_spt);
      exit(EXIT_FAILURE);
    }
  }
  
  itoken=yylex(&lvalp,prs_arg);
  
  while(itoken != 0){
    
    switch (itoken){
      
    case IGNORE:      break; /* Do nothing  */
    case ATTRIBUTE:   break; /* Do nothing  */
    case EPROVOKE:    break; /* Do nothing */
    case VAR: 
      var_nm=lvalp.vara;
      if( NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id) ){
	match=False;
	for(i=0; i < n_lst_a ; i++)
	  if(!strcmp(lst_a[i].nm,var_nm)){ match=True; break; }
	if(match) break;
	if(n_lst_a==0) lst_a=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_a=(nm_id_sct *)nco_realloc(lst_a,((n_lst_a+1)*sizeof(nm_id_sct)));
	lst_a[n_lst_a].nm=strdup(var_nm);
	lst_a[n_lst_a++].id=var_id;
      }
      break; 
    case OUT_VAR: 
      var_nm=lvalp.output_var;
      
      if( NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id) ){
	match=False;
	for(i=0; i < n_lst_b ; i++)
	  if(!strcmp(lst_b[i].nm,var_nm)){ match=True; break; }
	if(match) break;
	if(n_lst_b == 0) lst_b=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_b=(nm_id_sct *)nco_realloc(lst_b,((n_lst_b+1)*sizeof(nm_id_sct)));
	lst_b[n_lst_b].nm=strdup(var_nm);
	lst_b[n_lst_b++].id=var_id;
      }
      break;
      
    case OUT_ATT:       var_nm=lvalp.att.var_nm;     
      
      if( NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id) ){
	match=False;
	for(i=0; i < n_lst_c ; i++)
	  if(!strcmp(lst_c[i].nm,var_nm)){ match=True; break; }
	if(match) break;
	if(n_lst_c == 0) lst_c=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_c=(nm_id_sct *)nco_realloc(lst_c,((n_lst_c+1)*sizeof(nm_id_sct)));
	lst_c[n_lst_c].nm=strdup(var_nm);
	lst_c[n_lst_c++].id=var_id;
      }
      break;
    default: break;
    }
    itoken=yylex(&lvalp,prs_arg);
  }
  
  if(n_lst_a >0){ *xtr_lst_a=lst_a ; *nbr_lst_a=n_lst_a ;}  
  if(n_lst_b >0){ *xtr_lst_b=lst_b ; *nbr_lst_b=n_lst_b ;}  
  if(n_lst_c >0){ *xtr_lst_c=lst_c ; *nbr_lst_c=n_lst_c ;}  
  
} /* end ncap_initial_scan */
