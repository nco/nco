/* $Header$ */

/* Purpose: Missing value utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_mss_val.h" /* Missing value utilities */

ptr_unn /* O [sct] Default missing value for type type */
nco_mss_val_mk /* [fnc] Return default missing value for type type */
(const nc_type type) /* I [enm] netCDF type of operand */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Purpose: Return pointer union containing default missing value for type type */

  ptr_unn mss_val;

  mss_val.vp=(void *)nco_malloc(nco_typ_lng(type));

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&mss_val);

  switch(type){
  case NC_FLOAT: *mss_val.fp=NC_FILL_FLOAT; break; 
  case NC_DOUBLE: *mss_val.dp=NC_FILL_DOUBLE; break; 
  case NC_INT: *mss_val.ip=NC_FILL_INT; break;
  case NC_SHORT: *mss_val.sp=NC_FILL_SHORT; break;
  case NC_CHAR: *mss_val.cp=NC_FILL_CHAR; break;
  case NC_BYTE: *mss_val.bp=NC_FILL_BYTE; break;
  case NC_UBYTE: *mss_val.ubp=NC_FILL_UBYTE; break;
  case NC_USHORT: *mss_val.usp=NC_FILL_USHORT; break;
  case NC_UINT: *mss_val.uip=NC_FILL_UINT; break;
  case NC_INT64: *mss_val.i64p=NC_FILL_INT64; break;
  case NC_UINT64: *mss_val.ui64p=NC_FILL_UINT64; break;
    /* 20140526: Changed to NC_FILL_STRING (available as of netCDF 4.3.1?) */
    /* case NC_STRING: *mss_val.sngp='\0'; break; fxm: NC_FILL_STRING? TODO nco839 */
  case NC_STRING: *mss_val.sngp=(char *)(NC_FILL_STRING); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Un-typecast pointer to values after access */
  (void)cast_nctype_void(type,&mss_val);
  
  return mss_val;

} /* end nco_mss_val_mk() */
  
nco_bool /* O [flg] One or both operands have missing value */
nco_mss_val_cnf /* [fnc] Change missing_value of var2 to missing_value of var1 */
(var_sct * const var1, /* I [sct] Variable with template missing value to copy */
 var_sct * const var2) /* I/O [sct] Variable with missing value to fill-in/overwrite */
{
  /* Purpose: 
     1. Change missing_value of var2 to missing_value of var1 when both exist 
     2. Change missing_value of var1 to missing_value of var2 when only var2 has a missing_value
     3. Change missing_value of var2 to missing_value of var1 when only var1 has a missing_value
     4. Return false when neither operand has missing value */
  int has_mss_val=False; /* [flg] One or both operands have missing value */
  nco_bool MSS_VAL_EQL=False; /* [flg] Missing values of input operands are identical */
  long idx;
  long var_sz=long_CEWI;
  nc_type var_typ=NC_NAT; /* CEWI [enm] Type of input values */
  ptr_unn var_val;

  has_mss_val=var1->has_mss_val || var2->has_mss_val; 

  /* No missing values */
  if(!var1->has_mss_val && !var2->has_mss_val) return has_mss_val;

  if(var1->has_mss_val && var2->has_mss_val){
    var_typ=var1->type;
    (void)cast_void_nctype(var_typ,&var1->mss_val);
    (void)cast_void_nctype(var_typ,&var2->mss_val);
    switch(var_typ){
    case NC_FLOAT: MSS_VAL_EQL=(*var1->mss_val.fp == *var2->mss_val.fp); break;
    case NC_DOUBLE: MSS_VAL_EQL=(*var1->mss_val.dp == *var2->mss_val.dp); break;
    case NC_INT: MSS_VAL_EQL=(*var1->mss_val.ip == *var2->mss_val.ip); break;
    case NC_SHORT: MSS_VAL_EQL=(*var1->mss_val.sp == *var2->mss_val.sp); break;
    case NC_CHAR: MSS_VAL_EQL=(*var1->mss_val.cp == *var2->mss_val.cp); break;
    case NC_BYTE: MSS_VAL_EQL=(*var1->mss_val.bp == *var2->mss_val.bp); break;
    case NC_UBYTE: MSS_VAL_EQL=(*var1->mss_val.ubp == *var2->mss_val.ubp); break;
    case NC_USHORT: MSS_VAL_EQL=(*var1->mss_val.usp == *var2->mss_val.usp); break;
    case NC_UINT: MSS_VAL_EQL=(*var1->mss_val.uip == *var2->mss_val.uip); break;
    case NC_INT64: MSS_VAL_EQL=(*var1->mss_val.i64p == *var2->mss_val.i64p); break;
    case NC_UINT64: MSS_VAL_EQL=(*var1->mss_val.ui64p == *var2->mss_val.ui64p); break;
    case NC_STRING: MSS_VAL_EQL=(*var1->mss_val.sngp == *var2->mss_val.sngp); break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
    if(!MSS_VAL_EQL){
      char mss_val_1_sng[NCO_MAX_LEN_FMT_SNG];
      char mss_val_2_sng[NCO_MAX_LEN_FMT_SNG];
      const char *fmt_sng;
      fmt_sng=nco_typ_fmt_sng(var1->type);
      switch(var1->type){
      case NC_FLOAT: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.fp[0]); break;
      case NC_DOUBLE: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.dp[0]); break;
      case NC_SHORT: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.sp[0]); break;
      case NC_INT: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.ip[0]); break;
      case NC_CHAR: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.cp[0]); break;
      case NC_BYTE: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.bp[0]); break;
      case NC_UBYTE: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.ubp[0]); break;
      case NC_USHORT: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.usp[0]); break;
      case NC_UINT: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.uip[0]); break;
      case NC_INT64: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.i64p[0]); break;
      case NC_UINT64: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.ui64p[0]); break;
      case NC_STRING: (void)sprintf(mss_val_1_sng,fmt_sng,var1->mss_val.sngp[0]); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
      fmt_sng=nco_typ_fmt_sng(var2->type);
      switch(var2->type){
      case NC_FLOAT: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.fp[0]); break;
      case NC_DOUBLE: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.dp[0]); break;
      case NC_SHORT: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.sp[0]); break;
      case NC_INT: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.ip[0]); break;
      case NC_CHAR: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.cp[0]); break;
      case NC_BYTE: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.bp[0]); break;
      case NC_UBYTE: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.ubp[0]); break;
      case NC_USHORT: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.usp[0]); break;
      case NC_UINT: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.uip[0]); break;
      case NC_INT64: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.i64p[0]); break;
      case NC_UINT64: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.ui64p[0]); break;
      case NC_STRING: (void)sprintf(mss_val_2_sng,fmt_sng,var2->mss_val.sngp[0]); break;
      default: nco_dfl_case_nc_type_err(); break;
      } /* end switch */
      /* World's most anally formatted warning message... */
      (void)fprintf(stderr,"%s: WARNING Input variables have different NCO_MSS_VAL_SNG's:\nVariable #1 = %s has NCO_MSS_VAL_SNG type = %s, value = %s\nVariable #2 = %s has NCO_MSS_VAL_SNG type = %s, value = %s\nVariable #3 = output = %s will have NCO_MSS_VAL_SNG type = %s, value = %s\nWill translate values of var2 equaling mss_val2 to mss_val1 before evaluating arithmetic operation to compute var3\n",nco_prg_nm_get(),var1->nm,nco_typ_sng(var1->type),mss_val_1_sng,var2->nm,nco_typ_sng(var2->type),mss_val_2_sng,var1->nm,nco_typ_sng(var1->type),mss_val_1_sng);
    } /* MSS_VAL_EQL */
    (void)cast_nctype_void(var_typ,&var1->mss_val);
    (void)cast_nctype_void(var_typ,&var2->mss_val);
    
    /* Missing values are already equal */
    if(MSS_VAL_EQL) return has_mss_val;
    
    /* If both files have missing_value's and they differ,
       must translate mss_val_2 in var2 to mss_val_1 before binary operation.
       Otherwise mss_val_2 would be treated as regular value in var_2
       Unfixable bug is when var1 has mss_val1 and var2 does not have a missing_value
       Then var2 values that happen to equal mss_val1 are treated as missing_values
       A generic, satisfactory solution to this problem does not exist 
       Picking missing_values that are nearly out-of-range is best workaround
       The user must be smart enough to do this, NCO cannot help */
    
    /* Typecast pointer to values before access */
    (void)cast_void_nctype(var_typ,&var1->mss_val);
    (void)cast_void_nctype(var_typ,&var2->mss_val);
    (void)cast_void_nctype(var_typ,&var2->val);
    
    /* Shortcuts to avoid indirection */
    var_val=var2->val;
    var_sz=var2->sz;
    switch(var_typ){
    case NC_FLOAT: {
      const float mss_val_1_flt=*var1->mss_val.fp;
      const float mss_val_2_flt=*var2->mss_val.fp; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.fp[idx] == mss_val_2_flt) var_val.fp[idx]=mss_val_1_flt;
    } break;
    case NC_DOUBLE: {
      const double mss_val_1_dbl=*var1->mss_val.dp;
      const double mss_val_2_dbl=*var2->mss_val.dp; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.dp[idx] == mss_val_2_dbl) var_val.dp[idx]=mss_val_1_dbl;
    } break;
    case NC_INT: {
      const nco_int mss_val_1_ntg=*var1->mss_val.ip;
      const nco_int mss_val_2_ntg=*var2->mss_val.ip; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.ip[idx] == mss_val_2_ntg) var_val.ip[idx]=mss_val_1_ntg;
    } break;
    case NC_SHORT: {
      const short mss_val_1_sht=*var1->mss_val.sp;
      const short mss_val_2_sht=*var2->mss_val.sp; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.sp[idx] == mss_val_2_sht) var_val.sp[idx]=mss_val_1_sht; 
    } break;
    case NC_CHAR: {
      const nco_char mss_val_1_chr=*var1->mss_val.cp;
      const nco_char mss_val_2_chr=*var2->mss_val.cp; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.cp[idx] == mss_val_2_chr) var_val.cp[idx]=mss_val_1_chr; 
    } break;
    case NC_BYTE: {
      const nco_byte mss_val_1_byt=*var1->mss_val.bp;
      const nco_byte mss_val_2_byt=*var2->mss_val.bp; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.bp[idx] == mss_val_2_byt) var_val.bp[idx]=mss_val_1_byt; 
    } break;
    case NC_UBYTE: {
      const nco_ubyte mss_val_1_ubyte=*var1->mss_val.ubp;
      const nco_ubyte mss_val_2_ubyte=*var2->mss_val.ubp; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.ubp[idx] == mss_val_2_ubyte) var_val.ubp[idx]=mss_val_1_ubyte; 
    } break;
    case NC_USHORT: {
      const nco_ushort mss_val_1_ushort=*var1->mss_val.usp;
      const nco_ushort mss_val_2_ushort=*var2->mss_val.usp; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.usp[idx] == mss_val_2_ushort) var_val.usp[idx]=mss_val_1_ushort; 
    } break;
    case NC_UINT: {
      const nco_uint mss_val_1_uint=*var1->mss_val.uip;
      const nco_uint mss_val_2_uint=*var2->mss_val.uip; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.uip[idx] == mss_val_2_uint) var_val.uip[idx]=mss_val_1_uint; 
    } break;
    case NC_INT64: {
      const nco_int64 mss_val_1_int64=*var1->mss_val.i64p;
      const nco_int64 mss_val_2_int64=*var2->mss_val.i64p; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.i64p[idx] == mss_val_2_int64) var_val.i64p[idx]=mss_val_1_int64; 
    } break;
    case NC_UINT64: {
      const nco_uint64 mss_val_1_uint64=*var1->mss_val.ui64p;
      const nco_uint64 mss_val_2_uint64=*var2->mss_val.ui64p; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.ui64p[idx] == mss_val_2_uint64) var_val.ui64p[idx]=mss_val_1_uint64; 
    } break;
    case NC_STRING: {
      const nco_string mss_val_1_string=*var1->mss_val.sngp;
      const nco_string mss_val_2_string=*var2->mss_val.sngp; 
      for(idx=0L;idx<var_sz;idx++) if(var_val.sngp[idx] == mss_val_2_string) var_val.sngp[idx]=mss_val_1_string; 
    } break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */

    /* Un-typecast the pointer to values after access */
    (void)cast_nctype_void(var_typ,&var2->val);
    (void)cast_nctype_void(var_typ,&var1->mss_val);
    (void)cast_nctype_void(var_typ,&var2->mss_val);
  
  } /* end if both variables have missing values */
  
  /* Handle cases where only one variable has missing value.
     First, if var2 has missing_value and var1 does not,
     then copy missing_value from var2 to var1 to ensure binary arithmetic 
     (which uses var1's missing_value) handles missing_value's correctly
     Assume var1 and var2 already have same type */
  if(!var1->has_mss_val && var2->has_mss_val) (void)nco_mss_val_cp(var2,var1);
  if(var1->has_mss_val && !var2->has_mss_val) (void)nco_mss_val_cp(var1,var2);
  
  return has_mss_val;
} /* end nco_mss_val_cnf() */

void
nco_mss_val_cp /* [fnc] Copy missing value from var1 to var2 */
(const var_sct * const var1, /* I [sct] Variable with template missing value to copy */
 var_sct * const var2) /* I/O [sct] Variable with missing value to fill-in/overwrite */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Copy missing value from var1 to var2
     On exit, var2 contains has_mss_val, and mss_val identical to var1
     Type of mss_val in var2 will agree with type of var2
     This maintains assumed consistency between type of variable and type of mss_val in all var_sct's */

  if(!var1->has_mss_val){
    var2->has_mss_val=False;
    if(var2->mss_val.vp) free(var2->mss_val.vp);
  }else{ /* endif no mss_val in var1 */
    var2->mss_val.vp=(void *)nco_realloc(var2->mss_val.vp,nco_typ_lng(var2->type));
    (void)nco_val_cnf_typ(var1->type,var1->mss_val,var2->type,var2->mss_val);
    var2->has_mss_val=True;
  } /* endif var1 has mss_val */

} /* end nco_mss_val_cp() */
  
int /* O [flg] Variable has missing value on output */
nco_mss_val_get /* [fnc] Update number of attributes, missing value of variable */
(const int nc_id, /* I [id] netCDF input-file ID */
 var_sct * const var) /* I/O [sct] Variable with missing_value to update */
{
  /* Purpose: Update number of attributes and missing_value attribute of variable
     No matter what type missing_value is on disk, this routine ensures that 
     copy of mss_val in var_sct is stored as same type as host variable.
     Routine does not allow output missing_value to have more than one element */

  /* has_mss_val is typed as int not bool because it was sent to Fortran routines */

  static nco_bool WRN_FIRST=True; /* [flg] No warnings yet for _FillValue/missing_value mismatch */

  const char fnc_nm[]="nco_mss_val_get()";

  char att_nm[NC_MAX_NAME];
  
  int idx;

  long att_sz;
  
  nco_bool has_fll_val=False; /* [flg] Has _FillValue attribute */

  nc_type att_typ;
  nc_type bs_typ;
  nc_type cls_typ;

  ptr_unn mss_tmp;
  
  size_t att_lng;

  /* Refresh netCDF "mss_val" attribute for this variable */
  if(var->has_mss_val)
     if(var->mss_val.vp)
       var->mss_val.vp=(void *)nco_free(var->mss_val.vp);
  var->has_mss_val=False;

  /* Refresh number of attributes for variable */
  (void)nco_inq_varnatts(nc_id,var->id,&var->nbr_att);

  for(idx=0;idx<var->nbr_att;idx++){
    (void)nco_inq_attname(nc_id,var->id,idx,att_nm);
    if(WRN_FIRST && !(int)strcasecmp(att_nm,nco_not_mss_val_sng_get())) has_fll_val=True;

    if((int)strcasecmp(att_nm,nco_mss_val_sng_get())) continue;
    (void)nco_inq_att(nc_id,var->id,att_nm,&att_typ,&att_sz);
    bs_typ=cls_typ=att_typ;
    if(att_sz != 1L && att_typ != NC_CHAR){
      (void)fprintf(stderr,"%s: WARNING \"%s\" attribute for %s has %li elements and so will not be used\n",nco_prg_nm_get(),att_nm,var->nm,att_sz);
      continue;
    } /* end if */
    /* if(att_typ != var->type) (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s will be typecast from %s to %s for arithmetic purposes\n",nco_prg_nm_get(),att_nm,var->nm,nco_typ_sng(att_typ),nco_typ_sng(var->type)); */
    /* If we got this far then try to retrieve attribute and make sure it conforms to variable's type */
    var->has_mss_val=True;
    /* Oddly, ARM uses NC_CHAR for type of missing_value, so make allowances for this */
    att_lng=att_sz*nco_typ_lng_udt(nc_id,att_typ);
    mss_tmp.vp=(void *)nco_malloc(att_lng);
    (void)nco_get_att(nc_id,var->id,att_nm,mss_tmp.vp,att_typ);
    if(att_typ == NC_CHAR){
      /* NUL-terminate missing value string */
      if(mss_tmp.cp[att_lng-1] != '\0'){
	att_lng++;
	mss_tmp.vp=(void *)nco_realloc(mss_tmp.vp,att_lng);
	mss_tmp.cp[att_lng-1]='\0';
	/* Un-typecast pointer to values after access */
	(void)cast_nctype_void(att_typ,&mss_tmp);
      } /* end if */
    } /* end if */
    
    /* Store mss_val in memory as scalar of same type as variable */
    var->mss_val.vp=(void *)nco_malloc(1L*nco_typ_lng_udt(nc_id,var->type));
    if((att_typ <= NC_MAX_ATOMIC_TYPE) && (var->type <= NC_MAX_ATOMIC_TYPE)){
      (void)nco_val_cnf_typ(att_typ,mss_tmp,var->type,var->mss_val);
    }else{
      /* Assume att_typ equals var_typ for user-defined types otherwise things get too hairy */
      assert(att_typ == var->type);
      nco_inq_user_type(nc_id,att_typ,NULL,NULL,&bs_typ,NULL,&cls_typ);
      if(cls_typ == NC_ENUM){
	memcpy(var->mss_val.vp,mss_tmp.vp,nco_typ_lng(bs_typ));
      } /* !NC_ENUM */
      if(cls_typ == NC_VLEN){
	/* 20180514: De-reference first element of missing value ragged array
	   netCDF may require missing value to be same type as variable, but NCO needs a single scalar missing value
	   Hence NCO points var->mss_val to mss_tmp.vp.p[0], i.e., to first single scalar of same base type as actual VLEN on disk */
	nco_vlen mss_tmp_vlen;
	mss_tmp_vlen=mss_tmp.vlnp[0];
	if(nco_dbg_lvl_get() >= nco_dbg_std && mss_tmp_vlen.len > 1L) (void)fprintf(stderr,"%s: WARNING %s reports VLEN %s attribute for variable %s has %lu elements. NCO assumes VLEN %s attributes have only a single element. Results of using this %s in arithmetic are unpredictable.\n",nco_prg_nm_get(),fnc_nm,nco_mss_val_sng_get(),var->nm,(unsigned long)mss_tmp_vlen.len,nco_mss_val_sng_get(),nco_mss_val_sng_get());
	memcpy(var->mss_val.vp,mss_tmp_vlen.p,nco_typ_lng(bs_typ));
      } /* !NC_VLEN */
    } /* !NC_MAX_ATOMIC_TYPE */

    /* Release temporary memory */
    if(att_typ > NC_MAX_ATOMIC_TYPE && cls_typ == NC_VLEN) (void)nco_free_vlens(att_sz,mss_tmp.vlnp); else mss_tmp.vp=nco_free(mss_tmp.vp);
      
    break;
  } /* end loop over att */

  /* Warn when NCO looks for _FillValue and file has missing_value, and/or
     warn when NCO looks for missing_value and file has _FillValue.
     20101129: This is a long warning, only print when nco_dbg_lvl > 0 */
  if(nco_dbg_lvl_get() >= nco_dbg_std && has_fll_val && !var->has_mss_val && WRN_FIRST){
    char sa[1000];
    char sa1[1000];
    char sa2[1000]; 
    WRN_FIRST=False;
    (void)sprintf(sa,"%s: WARNING %s reports that variable %s has attribute \"%s\" but not \"%s\". To comply with netCDF conventions, NCO ignores values that equal the %s attribute when performing arithmetic.",nco_prg_nm_get(),fnc_nm,var->nm,nco_not_mss_val_sng_get(),nco_mss_val_sng_get(),nco_mss_val_sng_get());
    (void)sprintf(sa1," Confusingly, values equal to the missing_value should also be neglected. However, it is tedious and (possibly) computationally expensive to check each value against multiple missing values during arithmetic on large variables. So NCO thinks that processing variables with a \"%s\" attribute and no \"%s\" attribute may produce undesired arithmetic results (i.e., where values that were intended to be neglected were not, in fact, neglected).",nco_not_mss_val_sng_get(),nco_mss_val_sng_get());
    (void)sprintf(sa2, " We suggest you rename all \"%s\" attributes to \"%s\" or include both \"%s\" and \"%s\" attributes (with the _same values_) for all variables that have either attribute. Because it is long, this message is only printed once per operator even though multiple variables may have the same attribute configuration. More information on missing values is given at:\nhttp://nco.sf.net/nco.html#mss_val\nExamples of renaming attributes are at:\nhttp://nco.sf.net/nco.html#xmp_ncrename\nExamples of creating and deleting attributes are at:\nhttp://nco.sf.net/nco.html#xmp_ncatted\n",nco_not_mss_val_sng_get(),nco_mss_val_sng_get(),nco_not_mss_val_sng_get(),nco_mss_val_sng_get());
    (void)fprintf(stderr,"%s%s%s",sa,sa1,sa2); 
  } /* endif missing_value is and _FillValue is not defined */

  return var->has_mss_val;

} /* end nco_mss_val_get() */

nco_bool /* O [flg] Variable has missing value */
nco_mss_val_get_dbl /* [fnc] Return missing value of variable, if any, as double precision number */
(const int nc_id, /* I [id] netCDF input-file ID */
 const int var_id, /* I [id] netCDF variable ID */
 double *mss_val_dbl) /* O [frc] Missing value in double precision */
{
  /* Purpose: Return missing_value of variable, if any, as double precision number
     Basically this is a stripped-down, fast version of nco_mss_val_get()
     Created for use in regridding
     No matter what type missing_value is on disk, this routine returns a double precision value */
  
  static nco_bool WRN_FIRST=True; /* [flg] No warnings yet for _FillValue/missing_value mismatch */
  
  char att_nm[NC_MAX_NAME];
  char var_nm[NC_MAX_NAME];
  int idx;
  int var_nbr_att;
  long att_sz;
  nco_bool has_fll_val=False; /* [flg] Has _FillValue attribute */
  nco_bool has_mss_val=False; /* [flg] Has missing value attribute */
  nc_type att_typ;
  
  /* Refresh number of attributes for variable */
  (void)nco_inq_varnatts(nc_id,var_id,&var_nbr_att);
  (void)nco_inq_varname(nc_id,var_id,var_nm);
  
  for(idx=0;idx<var_nbr_att;idx++){
    (void)nco_inq_attname(nc_id,var_id,idx,att_nm);
    if(WRN_FIRST && !(int)strcasecmp(att_nm,nco_not_mss_val_sng_get())) has_fll_val=True;
    
    if((int)strcasecmp(att_nm,nco_mss_val_sng_get())) continue;
    (void)nco_inq_att(nc_id,var_id,att_nm,&att_typ,&att_sz);
    if(att_sz != 1L){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s has %li elements and so will not be used\n",nco_prg_nm_get(),att_nm,var_nm,att_sz);
      continue;
    } /* end if */
    if(att_typ == NC_CHAR || att_typ == NC_STRING){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s has type %s and so will not be used\n",nco_prg_nm_get(),att_nm,var_nm,nco_typ_sng(att_typ));
      continue;
    } /* end if */
    /* If we got this far then retrieve attribute */
    has_mss_val=True;
    /* Only retrieve value when pointer is non-NULL
       This prevents excessive retrievals and (potentially) printing of lengthy WARNINGS below */
    if(mss_val_dbl){
      /* Oddly, ARM uses NC_CHAR for type of missing_value, so make allowances for this */
      (void)nco_get_att(nc_id,var_id,att_nm,mss_val_dbl,NC_DOUBLE);
      //(void)fprintf(stderr,"%s: INFO NC_DOUBLE version of \"%s\" attribute for %s is %g\n",nco_prg_nm_get(),att_nm,var_nm,*mss_val_dbl);
      if(!isfinite(*mss_val_dbl)) (void)fprintf(stderr,"%s: WARNING NC_DOUBLE version of \"%s\" attribute for %s fails isfinite(), value is %s, which can cause unpredictable results.\nHINT: If arithmetic results (e.g., from regridding) fails or values seem weird, retry after first converting %s to normal number with, e.g., \"ncatted -a %s,%s,m,f,1.0e36 in.nc out.nc\"\n",nco_prg_nm_get(),att_nm,var_nm,(isnan(*mss_val_dbl)) ? "NaN" : ((isinf(*mss_val_dbl)) ? "Infinity" : ""),nco_mss_val_sng_get(),nco_mss_val_sng_get(),var_id != NC_GLOBAL ? var_nm : "");
    } /* !mss_val_dbl */
    break;
  } /* end loop over att */

  /* Warn when NCO looks for _FillValue but file has missing_value, and/or
     warn when NCO looks for missing_value but file has _FillValue.
     20101129: This is a long warning, only print when nco_dbg_lvl > 0 */
  if(nco_dbg_lvl_get() >= nco_dbg_std && has_fll_val && !has_mss_val && WRN_FIRST){
    char sa[1000];
    char sa1[1000];
    char sa2[1000]; 
    WRN_FIRST=False;
    (void)sprintf(sa,"%s: WARNING Variable %s has attribute \"%s\" but not \"%s\". To comply with netCDF conventions, NCO ignores values that equal the %s attribute when performing arithmetic.",nco_prg_nm_get(),var_nm,nco_not_mss_val_sng_get(),nco_mss_val_sng_get(),nco_mss_val_sng_get()); 
    (void)sprintf(sa1," Confusingly, values equal to the missing_value should also be neglected. However, it is tedious and (possibly) computationally expensive to check each value against multiple missing values during arithmetic on large variables. So NCO thinks that processing variables with a \"%s\" attribute and no \"%s\" attribute may produce undesired arithmetic results (i.e., where values that were intended to be neglected were not, in fact, neglected).",nco_not_mss_val_sng_get(),nco_mss_val_sng_get());
    (void)sprintf(sa2, " We suggest you rename all \"%s\" attributes to \"%s\" or include both \"%s\" and \"%s\" attributes (with the _same values_) for all variables that have either attribute. Because it is long, this message is only printed once per operator even though multiple variables may have the same attribute configuration. More information on missing values is given at:\nhttp://nco.sf.net/nco.html#mss_val\nExamples of renaming attributes are at:\nhttp://nco.sf.net/nco.html#xmp_ncrename\nExamples of creating and deleting attributes are at:\nhttp://nco.sf.net/nco.html#xmp_ncatted\n",nco_not_mss_val_sng_get(),nco_mss_val_sng_get(),nco_not_mss_val_sng_get(),nco_mss_val_sng_get());
    (void)fprintf(stderr,"%s%s%s",sa,sa1,sa2); 
  } /* !mss_val */

  return has_mss_val;
  
} /* end nco_mss_val_get_dbl() */

