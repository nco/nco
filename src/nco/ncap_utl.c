/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap_utl.c,v 1.40 2002-01-29 08:40:19 zender Exp $ */

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
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <time.h> /* machine time */
#include <unistd.h> /* POSIX stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF wrapper functions */

/* Personal headers */
#include "nco.h" /* netCDF operator universal def'ns */
#include "ncap.h" /* ncap functions, sym_sct, prs_sct, parse_sct */

var_sct *
ncap_var_init(char *var_nm,prs_sct *prs_arg)
{
  /* Purpose: Initialize variable structure, retrieve variable values from disk
     Parser calls ncap_var_init() when it encounters a new RHS variable */
  int var_id;
  int rcd;
  int fl_id;
  var_sct *vara;
  
  /* Check output file for var */  
  rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
  if(rcd == NC_NOERR){
    fl_id=prs_arg->out_id;
  }else{
    /* Check input file for ID */
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
    if(rcd == NC_NOERR ){
      fl_id=prs_arg->in_id;
    }else{
      (void)fprintf(stderr,"WARNING unable to find %s in %s or %s\n",var_nm,prs_arg->fl_in,prs_arg->fl_out);     
      return (var_sct*)NULL;
    }/* end else */
  } /* end else */
  
  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: parser VAR action called ncap_var_init() to retrieve %s from disk\n",prg_nm_get(),var_nm);
  vara=var_fll(fl_id,var_id,var_nm,prs_arg->dmn,prs_arg->nbr_dmn_xtr);
  vara->nm=nco_malloc((strlen(var_nm)+1)*sizeof(char)); strcpy(vara->nm,var_nm);
  vara->tally=(long *)malloc(vara->sz*sizeof(long));
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
ncap_var_write(var_sct *var,prs_sct *prs_arg)
{
  /* Purpose: Define variable in output file and write variable */
  int var_out_id;
  
  (void)nco_redef(prs_arg->out_id);
  
  /* Define variable */   
  (void)nco_def_var(prs_arg->out_id,var->nm,var->type,var->nbr_dim,var->dmn_id,&var_out_id);
  /* Put missing value */  
  if(var->has_mss_val) (void)nco_put_att(prs_arg->out_id,var_out_id,"missing_value",var->type,1,var->mss_val.vp);
  (void)nco_enddef(prs_arg->out_id);
  
  /* Write variable */ 
  if(var->nbr_dim == 0){
    (void)nco_put_var1(prs_arg->out_id,var_out_id,0L,var->val.vp,var->type);
  }else{
    (void)nco_put_vara(prs_arg->out_id,var_out_id,var->srt,var->cnt,var->val.vp,var->type);
  } /* end else */
  
  return 1;
} /* end ncap_var_write */

sym_sct *
ncap_sym_init(char *name,double (*fnc_dbl)(double),float (*fnc_flt)(float))
{ 
  /* Purpose: Allocate space for sym_sct then initialize */
  sym_sct *symbol;
  symbol=(sym_sct *)nco_malloc(sizeof(sym_sct));
  symbol->nm=strdup(name);
  symbol->fnc=fnc_dbl;
  symbol->fncf=fnc_flt;
  return symbol;
} /* end ncap_sym_init */

parse_sct 
ncap_ptr_unn_2_attribute(nc_type type, ptr_unn val)
{
  /* Purpose: Convert a ptr_unn to an attribute parse_sct
     Assumes that val is initially cast to void
     Note does not convert cp (strings) as these are not handled by parse_sct
     Note: netCDF attributes can contain MULTIPLE values
     Only FIRST value in memory block is converted */
  
  parse_sct a;
  (void)cast_void_nctype(type,&val);
  switch(type){
  case NC_FLOAT: a.val.f=*val.fp; break;
  case NC_DOUBLE: a.val.d =*val.dp; break;
  case NC_INT: a.val.l =*val.lp; break;
  case NC_SHORT: a.val.s=*val.sp; break;
  case NC_BYTE: a.val.b =*val.bp;  break;
  case NC_CHAR: break; /* do nothing */
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  a.type=type;
  /* Do not uncast pointer as we are working with a copy */
  return a;
} /* end ncap_ptr_unn_2_attribute */

ptr_unn
ncap_attribute_2_ptr_unn(parse_sct a)
{
  /* Purpose: Convert parse_sct to ptr_unn
     malloc() appropriate space for single type
     NB: Does not work on strings */
  ptr_unn val;
  nc_type type=a.type;
  val.vp=(void *)nco_malloc(nco_typ_lng(type));
  (void)cast_void_nctype(type,&val);
  
  switch(type){
  case NC_FLOAT: *val.fp=a.val.f; break;
  case NC_DOUBLE: *val.dp=a.val.d; break;
  case NC_INT: *val.lp=a.val.l; break;
  case NC_SHORT: *val.sp=a.val.s; break;
  case NC_BYTE: *val.bp=a.val.b; break;
  case NC_CHAR: break; /* do nothing */
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
  (void)cast_nctype_void(type,&val);
  return val;
} /* end ncap_attribute_2_ptr_unn */

var_sct *
ncap_var_var_add(var_sct *var_1,var_sct *var_2)
/* var_sct *var_1: input variable structure containing first operand
   var_sct *var_2: input variable structure containing second operand
   var_sct *ncap_var_var_add(): output sum of input variables */
{
  /* Purpose: Add two variables */
  var_sct *var_nsw;
  
  (void)ncap_var_retype(var_1,var_2);
  var_nsw=var_dpl(var_2);
  (void)ncap_var_conform_dim(&var_1,&var_nsw);
  /* fxm: bug in var_add. missing_value is not carried over to var_nsw in result when var_1->has_mss_val is true */
  if(var_1->has_mss_val){
    /*    (void)var_add(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->tally,var_1->val,var_nsw->val);*/
    (void)var_add_no_tally(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_nsw->val);
  }else{
    /*    (void)var_add(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->tally,var_1->val,var_nsw->val);*/
    (void)var_add_no_tally(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->val,var_nsw->val);
  } /* end if */
  return var_nsw;
} /* end ncap_var_var_add() */

var_sct *
ncap_var_var_sub(var_sct *var_2,var_sct *var_1)
/* var_sct *var_1: input variable structure containing first operand
   var_sct *var_2: input variable structure containing second operand
   var_sct *ncap_var_var_sub(): output var_2 - var_1 of input variables */
{
  /* Purpose: Subtraction of variables */
  var_sct *var_nsw;
  
  (void)ncap_var_retype(var_1,var_2);
  var_nsw=var_dpl(var_2);
  (void)ncap_var_conform_dim(&var_1,&var_nsw);
  if(var_1->has_mss_val){
    (void)var_subtract(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_nsw->val);
  }else{
    (void)var_subtract(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->val,var_nsw->val);
  }/* end else */
  return var_nsw;
} /* end ncap_var_var_sub() */

var_sct *
ncap_var_var_multiply(var_sct *var_1,var_sct *var_2)
/* var_sct *var_1: input variable structure containing first operand
   var_sct *var_2: input variable structure containing second operand
   var_sct *ncap_var_var_multiply(): output multiplication of individual elements */
{
  /* Purpose: Multiplication of variables */
  var_sct *var_nsw;
  (void)ncap_var_retype(var_1,var_2);
  var_nsw=var_dpl(var_2);
  (void)ncap_var_conform_dim(&var_1,&var_nsw);
  if(var_1->has_mss_val) {
     (void)var_multiply(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_nsw->val);
  }else{
     (void)var_multiply(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->val,var_nsw->val);
  }
  return var_nsw;
} /* end ncap_var_var_multiply() */

var_sct *
ncap_var_attribute_multiply(var_sct *var,parse_sct attribute)
{
  /* Purpose: Multiply variable by value in attribute */
  var_sct *var_nsw;
  var_nsw=var_dpl(var);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_multiply(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&attribute);
  
  return var_nsw;
} /* end ncap_var_attribute_multiply */

var_sct *
ncap_var_attribute_power(var_sct *var_in,parse_sct attribute)
{
  /* Purpose: Raise var to the power in attribute
     All values converted to type double before operation */
  long idx;
  long sz;
  ptr_unn op1;
  var_sct *var;

  /* Promote attribute and var to NC_FLOAT */
  if(var_in->type < NC_FLOAT) var_in=var_conform_type(NC_FLOAT,var_in);
  var=var_dpl(var_in);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  
  op1=var->val;
  sz=var->sz;
  (void)cast_void_nctype(var->type,&op1);
  if(var->has_mss_val) (void)cast_void_nctype(var->type,&(var->mss_val));
  
  switch(var->type){ 
  case NC_DOUBLE: {
    double att_dpl=attribute.val.d;
    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=pow(op1.dp[idx],att_dpl);
    }else{
      double mss_val_dbl=*(var->mss_val.dp); /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
        if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=pow(op1.dp[idx],att_dpl);
      } /* end for */
    } /* end else */
   break;
  }
  case NC_FLOAT: {
    float att_flt=attribute.val.f;
    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=powf(op1.fp[idx],att_flt);
    }else{
      float mss_val_flt=*(var->mss_val.fp); /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
        if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=powf(op1.fp[idx],att_flt);
      } /* end for */
    } /* end else */
   break;
  }
  default: nco_dfl_case_nctype_err(); break;
  }/* end switch */

  if(var->has_mss_val) (void)cast_nctype_void(var->type,&(var->mss_val));
  return var;
} /* end ncap_var_attribute_power */

var_sct *
ncap_var_function(var_sct *var_in, sym_sct *app)
{
  /* Purpose: Evaluate fnc(var) or fncf(var) for each value in variable
     Float and double functions are in app */
  long idx;
  long sz;
  ptr_unn op1;
  var_sct *var;

  /* Promote variable to NC_FLOAT */
  if(var_in->type < NC_FLOAT) var_in=var_conform_type(NC_FLOAT,var_in);
  var=var_dpl(var_in);
 
  op1=var->val;
  sz=var->sz;
  (void)cast_void_nctype(var->type,&op1);
  if(var->has_mss_val) (void)cast_void_nctype(var->type,&(var->mss_val));
  
  switch(var->type){ 
  case NC_DOUBLE: {
    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=(*(app->fnc))(op1.dp[idx]);
    }else{
      double mss_val_dbl=*(var->mss_val.dp); /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
        if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=(*(app->fnc))(op1.dp[idx]);
      } /* end for */
    } /* end else */
   break;
  }
  case NC_FLOAT: {
    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=(*(app->fncf))(op1.fp[idx]);
    }else{
      float mss_val_flt=*(var->mss_val.fp); /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
        if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=(*(app->fncf))(op1.fp[idx]);
      } /* end for */
    } /* end else */
   break;
  }
  default: nco_dfl_case_nctype_err(); break;
  }/* end switch */

  if(var->has_mss_val) (void)cast_nctype_void(var->type,&(var->mss_val));
  return var;
} /* end ncap_var_function */

var_sct *
ncap_var_attribute_add(var_sct *var,parse_sct attribute)
{
  /* Purpose: add the value in attribute to each element of var */
  var_sct *var_nsw;
  var_nsw=var_dpl(var);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_add(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&attribute);
  
  return var_nsw;
} /* end ncap_var_attribute_add */

var_sct *
ncap_var_attribute_sub(var_sct *var,parse_sct attribute)
{
  /* Purpose: Subtract the value in attribute from each element of var */
  var_sct *var_nsw;
  var_nsw=var_dpl(var);
  (void)ncap_attribute_minus(&attribute);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_add(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&attribute);
  
  return var_nsw;
} /* end ncap_var_attribute_sub */

var_sct *
ncap_var_var_divide(var_sct *var_1,var_sct *var_2)
/* var_sct *var_1: input variable structure containing first operand
   var_sct *var_2: input variable structure containing second operand
   var_sct *ncap_var_divide(): output quotient of individual elements */
{
  /* Routine called by parser */
  var_sct *var_nsw;
  (void)ncap_var_retype(var_1,var_2);
  var_nsw=var_dpl(var_2);
  (void)ncap_var_conform_dim(&var_1,&var_nsw);
  if(var_1->has_mss_val) {
    (void)var_divide(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_nsw->val);
  }else{
    (void)var_divide(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->val,var_nsw->val);
  } /* end else */ 
 return var_nsw;
} /* end ncap_var_var_divide() */

var_sct *
ncap_var_attribute_divide(var_sct *var,parse_sct attribute)
{
  /* Purpose: Divide each element of var by value in attribute */
  var_sct *var_nsw;
  var_nsw=var_dpl(var);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_divide(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&attribute);
  
  return var_nsw;
} /* end ncap_var_attribute_divide */

var_sct *
ncap_var_attribute_modulus(var_sct *var,parse_sct attribute)
{
  /* Purpose: var % attribute, take modulus of each element of var with value in attribute */
  
  var_sct *var_nsw;
  var_nsw=var_dpl(var);
  (void)ncap_attribute_conform_type(var->type,&attribute);
  (void)var_attribute_modulus(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&attribute);
  
  return var_nsw;
} /* ncap_var_attribute_modulus */

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
var_attribute_add(nc_type type,long sz,int has_mss_val,ptr_unn mss_val,ptr_unn op1,parse_sct *attribute)
     /* 
	nc_type type: I netCDF type of operands
	long sz: I size (in elements) of operands
	int has_mss_val: I flag for missing values
	ptr_unn mss_val: I value of missing value
	ptr_unn op1: I values of first operand
        attribute: */
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
  
  if(vara->type == varb->type) return vara->type;
  if(vara->type > varb->type){
    varb=var_conform_type(vara->type,varb);
    return vara->type;
  }else{
    vara=var_conform_type(varb->type,vara);
    return varb->type;
  } /* endif */
} /* end ncap_var_retype */

bool /* [flg] Do var_1 and var_2 conform after processing? */
ncap_var_conform_dim /* [fnc] Broadcast smaller variable into larger */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2) /* I/O [ptr] Second variable */
{
  /* Purpose: Return conforming variables. If this is not possible then die. 
     Routine is a wrapper for var_conform_dim() which does the hard work */

  bool DO_CONFORM; /* [flg] Do var_1 and var_2 conform after processing? */
  bool MUST_CONFORM=True; /* [flg] Must var_1 and var_2 conform? */
  var_sct *var_1_org; /* [ptr] Original location of var_1 */
  var_sct *var_2_org; /* [ptr] Original location of var_2 */

  var_1_org=*var_1; /* [ptr] Original location of var_1 */
  var_2_org=*var_2; /* [ptr] Original location of var_2 */

  if(var_1_org->nbr_dim > var_2_org->nbr_dim) *var_2=var_conform_dim(var_1_org,var_2_org,NULL,MUST_CONFORM,&DO_CONFORM); else *var_1=var_conform_dim(var_2_org,var_1_org,NULL,MUST_CONFORM,&DO_CONFORM);
  
  /* fxm: Memory leak?
     var_conform_dim does not do its own memory handling
     If original var_1 or var_2 was overwritten (replaced by conforming variable),
     then original must be free()'d now before its location is lost.
     Test for equality between pointers on entry and exit, and
     var_free() calling variable if it changed 
     20020114: Freeing variables here causes core dump, not sure why */
  /*  if(*var_1 != var_1_org) var_1_org=var_free(var_1_org);*/
  /*  if(*var_2 != var_2_org) var_2_org=var_free(var_2_org);*/

  if(!DO_CONFORM){
    (void)fprintf(stderr,"%s: Variables do not have have conforming dimensions. Cannot proceed with operation\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* endif */

  return DO_CONFORM; /* [flg] Do var_1 and var_2 conform after processing? */
} /* end ncap_var_conform_dim() */

int 
ncap_retype(parse_sct *a,parse_sct *b)
{
  /* Purpose: Convert attribute type if necessary so attributes are of same type */
  if(a->type == b->type) return a->type;
  if((a->type) > (b->type)){(void)ncap_attribute_conform_type(a->type,b);}
  else{(void)ncap_attribute_conform_type(b->type,a);}
  return a->type;    
} /* end ncap_retype */

int  
ncap_attribute_conform_type(nc_type type_new,parse_sct *a)
{
  /* Purpose: Convert an attribute to type_new using implicit C convertions */
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
  
  if(n == 0) return NULL;
  xtr_new_lst=(nm_id_sct*)nco_malloc(n*sizeof(nm_id_sct));
  for (i=0;i<n;i++){
    xtr_new_lst[i].nm=strdup(xtr_lst[i].nm);
    xtr_new_lst[i].id=xtr_lst[i].id;
  } /* end loop over variable */
  return xtr_new_lst;            
} /* end var_lst_copy */

nm_id_sct *
var_lst_sub(int in_id,nm_id_sct *xtr_lst,int *nbr_xtr,nm_id_sct *xtr_lst_b,int nbr_lst_b)
{
  /* Purpose: Subtract from xtr_lst any elements from xtr_lst_b which are present
     and return new list */
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
  /* Purpose: Add to xtr_lst any elements from xtr_lst_b which are not already present 
     and return a new list */
  int i;
  int j;
  int n;
  
  nm_id_sct *xtr_new_lst;
  
  bool match;
  
  if(*nbr_xtr >0 ){
    xtr_new_lst=(nm_id_sct*)nco_malloc(*nbr_xtr*sizeof(nm_id_sct));
    n=*nbr_xtr;
    for(i=0;i<*nbr_xtr;i++){
      xtr_new_lst[i].nm=strdup(xtr_lst[i].nm);
      xtr_new_lst[i].id=xtr_lst[i].id;
    } /* end loop over variables */
  }else{
    *nbr_xtr=nbr_lst_a;
    return var_lst_copy(xtr_lst_a,nbr_lst_a);
  }/* end if */
  
  for(i=0;i<nbr_lst_a;i++){
    match=False;
    for(j=0;j<*nbr_xtr;j++)
      if(!strcmp(xtr_lst[j].nm,xtr_lst_a[i].nm)){match=True;break;}
    if(match) continue;
    xtr_new_lst=(nm_id_sct*)nco_realloc(xtr_new_lst,(n+1)*sizeof(nm_id_sct));
    xtr_new_lst[n].nm=strdup(xtr_lst_a[i].nm);
    xtr_new_lst[n++].id=xtr_lst_a[i].id;
  }
  *nbr_xtr=n;
  return xtr_new_lst;            
} /* var_lst_add */

nm_id_sct *
ncap_var_lst_crd_make(int nc_id,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   nm_id_sct *xtr_lst: I/O current extraction list 
   int *nbr_xtr: I/O number of variables in current extraction list: Overwritten by new list 
   nm_id_sct ncap_var_lst_crd_make: list of coordinate dimensions 
 */
{
  /* Purpose: Make list co-ordinate dimensions from list of ordinary and co-ordinate variables */
  char dmn_nm[NC_MAX_NAME];

  int crd_id;
  int idx_dim;
  int idx_var;
  int nbr_dim;
  int rcd=NC_NOERR; /* [rcd] Return code */
  int nbr_new_lst=0;

  nm_id_sct *new_lst=NULL_CEWI;

  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dim,(int *)NULL,(int *)NULL,(int *)NULL);
  /* ...for each dimension in input file... */
  for(idx_dim=0;idx_dim<nbr_dim;idx_dim++){
    /* ...see if it is a coordinate dimension... */
    (void)nco_inq_dimname(nc_id,idx_dim,dmn_nm);
    rcd=nco_inq_varid_flg(nc_id,dmn_nm,&crd_id);
    if(rcd == NC_NOERR){
      /* Is this coordinate already on extraction list? */
      for(idx_var=0;idx_var<*nbr_xtr;idx_var++){
	if(crd_id == xtr_lst[idx_var].id) {
	  if(nbr_new_lst == 0) new_lst=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	  else new_lst=(nm_id_sct *)nco_realloc((void *)new_lst,(nbr_new_lst+1)*sizeof(nm_id_sct));
	  new_lst[nbr_new_lst].nm=(char *)strdup(dmn_nm);
	  new_lst[nbr_new_lst++].id=crd_id;
	  break;
        } /* end if */
      } /* end for */
    } /* end if */
  } /* end for */

  *nbr_xtr=nbr_new_lst;
  return new_lst;
} /* end ncap_var_lst_crd_make() */

void 
ncap_initial_scan
(prs_sct *prs_arg,char *spt_arg_cat, nm_id_sct** xtr_lst_a,int *nbr_lst_a,
 nm_id_sct** xtr_lst_b,int *nbr_lst_b,nm_id_sct** xtr_lst_c, int *nbr_lst_c)
{
  /* Purpose: Scan command script and return three lists
     list a: RHS variables present in input file
     list b: LHS variables present in input file
     list c: Variables of attributes on LHS which are present in input file */
  
  /* Get TOKENS and YYSTYPE prior to calling scanner */
#include "ncap.tab.h" /* ncap.tab.h is produced from ncap.y by parser generator */

  extern FILE *yyin; /* [fnc] File handle for script file */
  extern int yylex(YYSTYPE *,prs_sct *); /* [fnc] Scanner entrypoint */
  
/* Following declaration gets rid of implicit declaration compiler warning
   It is a condensation of the lexer declaration from lex.yy.c:
   YY_BUFFER_STATE yy_scan_string YY_PROTO(( yyconst char *yy_str )); */
  extern int yy_scan_string(const char *);

  bool match;
  
  char *var_nm;  

  int n_lst_a=0;
  int n_lst_b=0;
  int n_lst_c=0;
  int tkn_crr=-1; /* [tkn] Current token, must initialize to non-zero value */
  int var_id;
  int var_idx;

  nm_id_sct *lst_a=NULL_CEWI;
  nm_id_sct *lst_b=NULL_CEWI;
  nm_id_sct *lst_c=NULL_CEWI;
  
  YYSTYPE lval; /* [tkn] Token */

  if(spt_arg_cat){
    yy_scan_string(spt_arg_cat);
  }else{
    /* Open script file for reading */
    if((yyin=fopen(prs_arg->fl_spt,"r")) == NULL){
      (void)fprintf(stderr,"%s: ERROR Unable to open script file %s\n",prg_nm_get(),prs_arg->fl_spt);
      exit(EXIT_FAILURE);
    } /* endif error */
  } /* endif input from script */
  
  /* While there are more tokens... */
  while(tkn_crr != 0){
    /* ...obtain next token from lexer... */
    tkn_crr=yylex(&lval,prs_arg);
    /* ...determine which variables and attributes exist in input file... */
    switch (tkn_crr){
    case IGNORE: break; /* Do nothing  */
    case ATTRIBUTE: break; /* Do nothing  */
    case EPROVOKE: break; /* Do nothing */
    case VAR: 
      /* Search for RHS variables in input file */
      var_nm=lval.vara;
      if(NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id)){
	match=False;
	for(var_idx=0;var_idx<n_lst_a;var_idx++)
	  if(!strcmp(lst_a[var_idx].nm,var_nm)){match=True; break;}
	if(match) break;
	if(n_lst_a==0) lst_a=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_a=(nm_id_sct *)nco_realloc(lst_a,((n_lst_a+1)*sizeof(nm_id_sct)));
	lst_a[n_lst_a].nm=strdup(var_nm);
	lst_a[n_lst_a++].id=var_id;
      } /* endif RHS variable is in input file */
      break; 
    case OUT_VAR: 
      /* Search for LHS variables in input file */
      var_nm=lval.output_var;
      if(NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id)){
	match=False;
	for(var_idx=0;var_idx<n_lst_b;var_idx++)
	  if(!strcmp(lst_b[var_idx].nm,var_nm)){match=True; break;}
	if(match) break;
	if(n_lst_b == 0) lst_b=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_b=(nm_id_sct *)nco_realloc(lst_b,((n_lst_b+1)*sizeof(nm_id_sct)));
	lst_b[n_lst_b].nm=strdup(var_nm);
	lst_b[n_lst_b++].id=var_id;
      } /* endif LHS variable is in input file */
      break;
    case OUT_ATT:
      /* Search for LHS attribute's parent variable in input file */
      var_nm=lval.att.var_nm;     
      if(NC_NOERR == nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id)){
	match=False;
	for(var_idx=0;var_idx<n_lst_c;var_idx++)
	  if(!strcmp(lst_c[var_idx].nm,var_nm)){match=True; break;}
	if(match) break;
	if(n_lst_c == 0) lst_c=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	else lst_c=(nm_id_sct *)nco_realloc(lst_c,((n_lst_c+1)*sizeof(nm_id_sct)));
	lst_c[n_lst_c].nm=strdup(var_nm);
	lst_c[n_lst_c++].id=var_id;
      } /* endif LHS attribute's parent variable is in input file  */
      break;
    default: break;
    } /* end switch */
  } /* end while */
  
  if(n_lst_a>0){*xtr_lst_a=lst_a;*nbr_lst_a=n_lst_a;}
  if(n_lst_b>0){*xtr_lst_b=lst_b;*nbr_lst_b=n_lst_b;}
  if(n_lst_c>0){*xtr_lst_c=lst_c;*nbr_lst_c=n_lst_c;}
  
} /* end ncap_initial_scan() */

bool /* O [flg] Variables now conform */
ncap_var_stretch /* [fnc] Stretch variables */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2) /* I/O [ptr] Second variable */
{
  /* Purpose: Make input variables conform or die
     var_1 and var_2 are considered completely symmetrically
     No assumption is made about var_1 relative to var_2
     Main difference betwee ncap_var_stretch() and var_conform_dim() is
     If variables conform, then ncap_var_stretch() will broadcast
     If variables share no dimensions, then ncap_var_stretch() will convolve

     Terminology: 
     Broadcast: Inflate smaller conforming variable to larger variable
     Conform: Dimensions of one variable are subset of other variable
     Convolve: Construct array which is product of ranks of two variables
     Stretch: Union of broadcast and convolve

     Logic is pared down version of var_conform_dim()
     1. USE_DUMMY_WGT has been eliminated: 
     ncap has no reason not to stretch input variables because grammar
     ensures only arithmetic variables will be stretched.
     
     2. wgt_crr has been eliminated:
     ncap never does anything multiple times so no equivalent to wgt_crr exists

     3. ncap_var_stretch(), unlike var_conform_dim(), performs memory management
     Variables are var_free'd if they are superceded (replaced)

     4. Conformance logic is duplicated from var_conform_dim()
     var_gtr plays role of var
     var_lsr plays role of wgt
     var_lsr_out plays role of wgt_out
     var_lsr_out=var_lsr only if variables already conform
     var_gtr_out is required since both variables may change
     var_gtr_out=var_gtr unless convolution is required
  */

  bool CONFORMABLE=False; /* Whether var_lsr can be made to conform to var_gtr */
  bool CONVOLVE=False; /* [flg] var_1 and var_2 had to be convolved */
  bool DO_CONFORM; /* [flg] Did var_1 and var_2 conform? */
  bool MUST_CONFORM=False; /* [flg] Must var_1 and var_2 conform? */

  int idx;
  int idx_dmn;
  int var_lsr_var_gtr_dmn_shr_nbr=0; /* Number of dimensions shared by var_lsr and var_gtr */

  var_sct *var_gtr=NULL; /* [ptr] Pointer to variable structure of greater rank */
  var_sct *var_lsr=NULL; /* [ptr] Pointer to variable structure to lesser rank */
  var_sct *var_gtr_out=NULL; /* [ptr] Pointer to stretched version of greater rank variable */
  var_sct *var_lsr_out=NULL; /* [ptr] Pointer to stretched version of lesser rank variable */

  /* Initialize flag to false. Overwrite by true after successful conformance */
  DO_CONFORM=False;
  
  if((*var_1)->nbr_dim >= (*var_2)->nbr_dim){
    var_gtr=*var_1; 
    var_lsr=*var_2; 
  }else{
    var_gtr=*var_2; 
    var_lsr=*var_1; 
  } /* endif */

  /* var_gtr_out=var_gtr unless convolution is required */
  var_gtr_out=var_gtr; 

  /* Does lesser variable (var_lsr) conform to greater variable's dimensions? */
  if(var_lsr_out == NULL){
    if(var_gtr->nbr_dim > 0){
      /* Test that all dimensions in var_lsr appear in var_gtr */
      for(idx=0;idx<var_lsr->nbr_dim;idx++){
        for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(strstr(var_lsr->dim[idx]->nm,var_gtr->dim[idx_dmn]->nm)){
	    var_lsr_var_gtr_dmn_shr_nbr++; /* var_lsr and var_gtr share this dimension */
	    break;
	  } /* endif */
        } /* end loop over var_gtr dimensions */
      } /* end loop over var_lsr dimensions */
      /* Decide whether var_lsr and var_gtr dimensions conform, are mutually exclusive, or are partially exclusive */ 
      if(var_lsr_var_gtr_dmn_shr_nbr == var_lsr->nbr_dim){
	/* var_lsr and var_gtr conform */
	/* fxm: Variables do not conform when dimension list of one is subset of other if order of dimensions differs, i.e., a(lat,lev,lon) !~ b(lon,lev) */
	CONFORMABLE=True;
      }else if(var_lsr_var_gtr_dmn_shr_nbr == 0){
	/* Dimensions in var_lsr and var_gtr are mutually exclusive */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %s and template %s share no dimensions\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: DEBUG %s and %s share no dimensions: Attempting to convolve...\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      }else if(var_lsr_var_gtr_dmn_shr_nbr > 0 && var_lsr_var_gtr_dmn_shr_nbr < var_lsr->nbr_dim){
	/* Some, but not all, of var_lsr dimensions are in var_gtr */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr);
	  exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: DEBUG %d dimensions of %s belong to template %s but %d dimensions do not: Not broadcasting %s to %s, could attempt stretching???\n",prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      } /* end if */
      if(CONFORMABLE){
	if(var_gtr->nbr_dim == var_lsr->nbr_dim){
	  /* var_gtr and var_lsr conform and are same rank */
	  /* Test whether all var_lsr and var_gtr dimensions match in sequence */
	  for(idx=0;idx<var_gtr->nbr_dim;idx++){
	    if(!strstr(var_lsr->dim[idx]->nm,var_gtr->dim[idx]->nm)) break;
	  } /* end loop over dimensions */
	  /* If so, take shortcut and copy var_lsr to var_lsr_out */
	  if(idx == var_gtr->nbr_dim) DO_CONFORM=True;
	}else{
	  /* var_gtr and var_lsr conform but are not same rank, set flag to proceed to generic conform routine */
	  DO_CONFORM=False;
	} /* end else */
      } /* endif CONFORMABLE */
    }else{
      /* var_gtr is scalar, if var_lsr is also then set flag to copy var_lsr to var_lsr_out else proceed to generic conform routine */
      if(var_lsr->nbr_dim == 0) DO_CONFORM=True; else DO_CONFORM=False;
    } /* end else */
    if(CONFORMABLE && DO_CONFORM){
      var_lsr_out=var_dpl(var_lsr);
      (void)var_xrf(var_lsr,var_lsr_out);
    } /* end if */
  } /* end if */

  if(var_lsr_out == NULL && CONVOLVE){
    /* Convolve variables by returned stretched variables with minimum possible number of dimensions */
    int dmn_nbr; /* Number of dimensions in convolution */
    if(dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: WARNING Convolution not yet implented, results of operation between %s and %s are unpredictable\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
    /* Dimensions in convolution are union of dimensions in variables */
    dmn_nbr=var_lsr->nbr_dim+var_gtr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr; /* Number of dimensions in convolution */
    /* fxm: these should go away soon */
    var_lsr_out=var_dpl(var_lsr);
    var_gtr_out=var_dpl(var_gtr);
    for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){;}

    if(var_lsr_var_gtr_dmn_shr_nbr == 0) ; else ;

    /* Free calling variables */
    var_lsr=var_free(var_lsr);
    var_gtr=var_free(var_gtr);
  } /* endif STRETCH */

  if(var_lsr_out == NULL){
    /* Expand lesser variable (var_lsr) to match size of greater variable */
    char *var_lsr_cp;
    char *var_lsr_out_cp;

    int idx_var_lsr_var_gtr[NC_MAX_DIMS];
    int var_lsr_nbr_dim;
    int var_lsr_type_sz;
    int var_gtr_nbr_dmn_m1;

    long *var_gtr_cnt;
    long dmn_ss[NC_MAX_DIMS];
    long dmn_var_gtr_map[NC_MAX_DIMS];
    long dmn_var_lsr_map[NC_MAX_DIMS];
    long var_gtr_lmn;
    long var_lsr_lmn;
    long var_gtr_sz;

    /* Copy main attributes of greater variable into lesser variable */
    var_lsr_out=var_dpl(var_gtr);
    (void)var_xrf(var_lsr,var_lsr_out);

    /* Modify a few elements of lesser variable array */
    var_lsr_out->nm=var_lsr->nm;
    var_lsr_out->id=var_lsr->id;
    var_lsr_out->type=var_lsr->type;
    var_lsr_out->val.vp=(void *)nco_malloc(var_lsr_out->sz*nco_typ_lng(var_lsr_out->type));
    var_lsr_cp=(char *)var_lsr->val.vp;
    var_lsr_out_cp=(char *)var_lsr_out->val.vp;
    var_lsr_type_sz=nco_typ_lng(var_lsr_out->type);

    if(var_lsr_out->nbr_dim == 0){
      /* Variables are scalars, not arrays */

      (void)memcpy(var_lsr_out_cp,var_lsr_cp,var_lsr_type_sz);

    }else{
      /* Variables are arrays, not scalars */
      
      /* Create forward and reverse mappings from greater variable's dimensions to lesser variable's dimensions:

	 dmn_var_gtr_map[i] is number of elements between one value of i_th 
	 dimension of greater variable and next value of i_th dimension, i.e., 
	 number of elements in memory between indicial increments in i_th dimension. 
	 This is computed as product of one (1) times size of all dimensions (if any) after i_th 
	 dimension in greater variable.

	 dmn_var_lsr_map[i] contains analogous information, except for lesser variable

	 idx_var_lsr_var_gtr[i] contains index into greater variable's dimensions of i_th dimension of lesser variable
	 idx_var_gtr_var_lsr[i] contains index into lesser variable's dimensions of i_th dimension of greater variable 

	 Since lesser variable is a subset of greater variable, some elements of idx_var_gtr_var_lsr may be "empty", or unused

	 Since mapping arrays (dmn_var_gtr_map and dmn_var_lsr_map) are ultimately used for a
	 memcpy() operation, they could (read: should) be computed as byte offsets, not type offsets.
	 This is why netCDF generic hyperslab routines (ncvarputg(), ncvargetg())
	 request imap vector to specify offset (imap) vector in bytes.
      */

      for(idx=0;idx<var_lsr->nbr_dim;idx++){
	for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(strstr(var_gtr->dim[idx_dmn]->nm,var_lsr->dim[idx]->nm)){
	    idx_var_lsr_var_gtr[idx]=idx_dmn;
	    /*	    idx_var_gtr_var_lsr[idx_dmn]=idx;*/
	    break;
	  } /* end if */
	  /* Sanity check */
	  if(idx_dmn == var_gtr->nbr_dim-1){
	    (void)fprintf(stdout,"%s: ERROR var_lsr %s has dimension %s but var_gtr %s does not deep in ncap_var_stretch()\n",prg_nm_get(),var_lsr->nm,var_lsr->dim[idx]->nm,var_gtr->nm);
	    exit(EXIT_FAILURE);
	  } /* end if err */
	} /* end loop over greater variable dimensions */
      } /* end loop over lesser variable dimensions */
      
      /* Figure out map for each dimension of greater variable */
      for(idx=0;idx<var_gtr->nbr_dim;idx++) dmn_var_gtr_map[idx]=1L;
      for(idx=0;idx<var_gtr->nbr_dim-1;idx++)
	for(idx_dmn=idx+1;idx_dmn<var_gtr->nbr_dim;idx_dmn++)
	  dmn_var_gtr_map[idx]*=var_gtr->cnt[idx_dmn];
      
      /* Figure out map for each dimension of lesser variable */
      for(idx=0;idx<var_lsr->nbr_dim;idx++) dmn_var_lsr_map[idx]=1L;
      for(idx=0;idx<var_lsr->nbr_dim-1;idx++)
	for(idx_dmn=idx+1;idx_dmn<var_lsr->nbr_dim;idx_dmn++)
	  dmn_var_lsr_map[idx]*=var_lsr->cnt[idx_dmn];
      
      /* Define convenience variables to avoid repetitive indirect addressing */
      var_lsr_nbr_dim=var_lsr->nbr_dim;
      var_gtr_sz=var_gtr->sz;
      var_gtr_cnt=var_gtr->cnt;
      var_gtr_nbr_dmn_m1=var_gtr->nbr_dim-1;

      /* var_gtr_lmn is offset into 1-D array corresponding to N-D indices dmn_ss */
      for(var_gtr_lmn=0;var_gtr_lmn<var_gtr_sz;var_gtr_lmn++){
	dmn_ss[var_gtr_nbr_dmn_m1]=var_gtr_lmn%var_gtr_cnt[var_gtr_nbr_dmn_m1];
	for(idx=0;idx<var_gtr_nbr_dmn_m1;idx++){
	  dmn_ss[idx]=(long)(var_gtr_lmn/dmn_var_gtr_map[idx]);
	  dmn_ss[idx]%=var_gtr_cnt[idx];
	} /* end loop over dimensions */
	
	/* Map (shared) N-D array indices into 1-D index into original lesser variable data */
	var_lsr_lmn=0L;
	for(idx=0;idx<var_lsr_nbr_dim;idx++) var_lsr_lmn+=dmn_ss[idx_var_lsr_var_gtr[idx]]*dmn_var_lsr_map[idx];
	
	(void)memcpy(var_lsr_out_cp+var_gtr_lmn*var_lsr_type_sz,var_lsr_cp+var_lsr_lmn*var_lsr_type_sz,var_lsr_type_sz);
	
      } /* end loop over var_gtr_lmn */
      
    } /* end if greater variable (and lesser variable) are arrays, not scalars */
    
    DO_CONFORM=True;
  } /* end if we had to broadcast lesser variable to fit greater variable */
  
  /* Place variables in original order
     Not necessary if variables are used in binary operations that are associative
     But do not want to require that assumption of calling routines */
  if((*var_1)->nbr_dim >= (*var_2)->nbr_dim){
    *var_1=var_gtr_out; /* [ptr] First variable */
    *var_2=var_lsr_out; /* [ptr] Second variable */
  }else{
    *var_1=var_lsr_out; /* [ptr] First variable */
    *var_2=var_gtr_out; /* [ptr] Second variable */
  } /* endif */

  /* Variables now conform */
  return DO_CONFORM;
} /* end ncap_var_stretch() */

