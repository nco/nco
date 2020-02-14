/* $Header$ */

/* Purpose: Scalar utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_scl_utl.h" /* Scalar utilities */

var_sct * /* O [sct] netCDF variable structure representing val */
scl_dbl_mk_var /* [fnc] Convert scalar double into netCDF variable */
(const double val) /* I [frc] Double precision value to turn into netCDF variable */
{
  /* Purpose: Convert scalar double into netCDF variable
     Routine duplicates most functions of nco_var_fll() 
     Both functions should share as much initialization code as possible */
  var_sct *var;

  var=(var_sct *)nco_malloc(sizeof(var_sct));

  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */

  /* Overwrite defaults with values appropriate for artificial variable */
  var->nm=(char *)strdup("Internally_generated_variable");
  var->nm_fll=NULL;
  var->nbr_dim=0;
  var->type=NC_DOUBLE;
  var->val.vp=(void *)nco_malloc(nco_typ_lng(var->type));
  (void)memcpy((void *)var->val.vp,(const void *)(&val),nco_typ_lng(var->type));

  return var;
} /* end scl_dbl_mk_var() */

var_sct * /* O [sct] Output netCDF variable structure representing val */
scl_mk_var /* [fnc] Convert scalar value of any type into NCO variable */
(val_unn val, /* I [frc] Scalar value to turn into netCDF variable */
 const nc_type val_typ) /* I [enm] netCDF type of value */
{
  /* Purpose: Turn scalar value of any type into NCO variable
     Routine is just a wrapper for scl_ptr_mk_var()
     This routine creates the void * argument needed for scl_ptr_mk_var(),
     calls, scl_ptr_mk_var(), then passes back the result */

  var_sct *var;
  ptr_unn val_ptr_unn; /* [ptr] void pointer to value */
  
  switch(val_typ){
  case NC_FLOAT: val_ptr_unn.fp=&val.f; break; 
  case NC_DOUBLE: val_ptr_unn.dp=&val.d; break; 
  case NC_INT: val_ptr_unn.ip=&val.i; break;
  case NC_SHORT: val_ptr_unn.sp=&val.s; break;
  case NC_CHAR: val_ptr_unn.cp=&val.c; break;
  case NC_BYTE: val_ptr_unn.bp=&val.b; break;
  case NC_UBYTE: val_ptr_unn.ubp=&val.ub; break;
  case NC_USHORT: val_ptr_unn.usp=&val.us; break;
  case NC_UINT: val_ptr_unn.uip=&val.ui; break;
  case NC_INT64: val_ptr_unn.i64p=&val.i64; break;
  case NC_UINT64: val_ptr_unn.ui64p=&val.ui64; break;
  case NC_STRING: val_ptr_unn.sngp=&val.sng; break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Un-typecast pointer to values after access */
  (void)cast_nctype_void(val_typ,&val_ptr_unn);

  var=scl_ptr_mk_var(val_ptr_unn,val_typ);

  return var;
} /* end scl_mk_var() */

var_sct * /* O [sct] Output NCO variable structure representing value */
scl_ptr_mk_var /* [fnc] Convert void pointer to scalar of any type into NCO variable */
(const ptr_unn val_ptr_unn, /* I [unn] Scalar value to turn into netCDF variable */
 const nc_type val_typ) /* I [enm] netCDF type of existing pointer/value */
{
  /* Purpose: Convert void pointer to scalar of any type into NCO variable
     Routine duplicates many functions of nco_var_fll() 
     Both functions should share as much initialization code as possible */
  var_sct *var;
  
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  
  /* Overwrite defaults with values appropriate for artificial variable */
  var->nm=(char *)strdup("Internally_generated_variable");
  var->nm_fll=NULL;
  var->nbr_dim=0;
  var->type=val_typ;
  /* Allocate new space here so that variable can eventually be deleted 
     and associated memory free()'d */
  /* free(val_ptr_unn.vp) is unpredictable since val_ptr_unn may point to constant data, e.g.,
     a constant in scl_mk_var */
  var->val.vp=(void *)nco_malloc(nco_typ_lng(var->type));

  /* Copy value into variable structure */
  (void)memcpy((void *)var->val.vp,val_ptr_unn.vp,nco_typ_lng(var->type)); 

  return var;
} /* end scl_ptr_mk_var() */

double /* O [frc] Double precision representation of var->val.?p[0] */
ptr_unn_2_scl_dbl /* [fnc] Convert first element of NCO variable to a scalar double */
(const ptr_unn val, /* I [sct] Pointer union to convert to scalar double */
 const nc_type type) /* I [enm] Type of values pointed to by pointer union */
{
  /* Purpose: Return first element of NCO variable converted to a scalar double */

  double scl_dbl; /* [sct] Double precision value of scale_factor */

  ptr_unn ptr_unn_scl_dbl; /* [unn] Pointer union to double precision value of first element */

  /* Variable must be in memory already */
  if(val.vp == NULL){ 
    (void)fprintf(stdout,"%s: ERROR ptr_unn_2_scl_dbl() called with empty val.vp\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  
  /* Valid memory address exists */
  ptr_unn_scl_dbl.vp=(void *)nco_malloc(nco_typ_lng(NC_DOUBLE)); /* [unn] Pointer union to double precision value of first element */
  (void)nco_val_cnf_typ(type,val,NC_DOUBLE,ptr_unn_scl_dbl);
  scl_dbl=ptr_unn_scl_dbl.dp[0];
  ptr_unn_scl_dbl.vp=nco_free(ptr_unn_scl_dbl.vp);

  return scl_dbl;

} /* end ptr_unn_2_scl_dbl() */

scv_sct  /* O [sct] Scalar value structure representing val */
ptr_unn_2_scv /* [fnc] Convert ptr_unn to scalar value structure */
(const nc_type type, /* I [enm] netCDF type of value */
 ptr_unn val) /* I [sct] Value to convert to scalar value structure */
{
  /* Purpose: Convert ptr_unn to scalar value structure
     Assumes that val is initially cast to void
     Does not convert cp (strings) as these are not handled by scv_sct
     NB: netCDF attributes may contain multiple values
     Only FIRST value in memory block is converted */
  
  scv_sct scv;
  (void)cast_void_nctype(type,&val);
  switch(type){
  case NC_FLOAT: scv.val.f=*val.fp; break;
  case NC_DOUBLE: scv.val.d=*val.dp; break;
  case NC_INT: scv.val.i=*val.ip; break;
  case NC_SHORT: scv.val.s=*val.sp; break;
  case NC_BYTE: scv.val.b=*val.bp; break;
  case NC_CHAR: break; /* Do nothing */
  case NC_UBYTE: scv.val.ub=*val.ubp; break;
  case NC_USHORT: scv.val.us=*val.usp; break;
  case NC_UINT: scv.val.ui=*val.uip; break;
  case NC_INT64: scv.val.i64=*val.i64p; break;
  case NC_UINT64: scv.val.ui64=*val.ui64p; break;
  case NC_STRING: scv.val.sng=*val.sngp; break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  scv.type=type;
  /* Do not uncast pointer as we are working with a copy */
  return scv;
} /* end ptr_unn_2_scv */
