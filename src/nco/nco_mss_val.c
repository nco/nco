/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_mss_val.c,v 1.16 2004-01-05 17:29:05 zender Exp $ */

/* Purpose: Missing value utilities */

/* Copyright (C) 1995--2004 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

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
  case NC_INT: *mss_val.lp=NC_FILL_INT; break;
  case NC_SHORT: *mss_val.sp=NC_FILL_SHORT; break;
  case NC_CHAR: *mss_val.cp=NC_FILL_CHAR; break;
  case NC_BYTE: *mss_val.bp=NC_FILL_BYTE; break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Un-typecast pointer to values after access */
  (void)cast_nctype_void(type,&mss_val);
  
  return mss_val;

} /* end nco_mss_val_mk() */
  
bool /* O [flg] One or both operands have missing value */
nco_mss_val_cnf /* [fnc] Change missing_value of var2 to missing_value of var1 */
(var_sct * const var1, /* I [sct] Variable with template missing value to copy */
 var_sct * const var2) /* I/O [sct] Variable with missing value to fill in/overwrite */
{
  /* Purpose: Change missing_value of var2 to missing_value of var1 */
  int has_mss_val=False; /* [flg] One or both operands have missing value */
  bool MSS_VAL_EQL=False; /* [flg] Missing values of input operands are identical */
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
    case NC_INT: MSS_VAL_EQL=(*var1->mss_val.lp == *var2->mss_val.lp); break;
    case NC_SHORT: MSS_VAL_EQL=(*var1->mss_val.sp == *var2->mss_val.sp); break;
    case NC_CHAR: MSS_VAL_EQL=(*var1->mss_val.cp == *var2->mss_val.cp); break;
    case NC_BYTE: MSS_VAL_EQL=(*var1->mss_val.bp == *var2->mss_val.bp); break;
    default: nco_dfl_case_nc_type_err(); break;
    } /* end switch */
    /* fxm: Print statement only works with type NC_FLOAT */
    if(!MSS_VAL_EQL) (void)fprintf(stderr,"%s: WARNING Input variables have different missing_value's:\nFile 1 variable %s has missing_value type = %s, value = %f\nFile 2 variable %s has missing_value type = %s, value = %f\nFile 3 variable %s will have missing_value type = %s, value = %f\nWill translate values of var2 equaling mss_val2 to mss_val1 before arithmetic operation\n",prg_nm_get(),var1->nm,nco_typ_sng(var1->type),var1->mss_val.fp[0],var2->nm,nco_typ_sng(var2->type),var2->mss_val.fp[0],var1->nm,nco_typ_sng(var1->type),var1->mss_val.fp[0]);
    (void)cast_nctype_void(var_typ,&var1->mss_val);
    (void)cast_nctype_void(var_typ,&var2->mss_val);
  } /* end if both variables have missing values */
  
  /* Missing values are already equal */
  if(MSS_VAL_EQL) return has_mss_val;

  /* If both files have missing_value's and they differ,
     must translate mss_val_2 in var2 to mss_val_1 before binary operation.
     Otherwise mss_val_2 would be treated as regular value in var_2
     Unfixable bug is when var1 has mss_val1 and var2 does not have a missing_value
     Then var2 values that happen to equal mss_val1 are treated as missing_values
     A generic, satisfactory solution to this problem does not exist 
     Picking missing_values that are nearly out-of-range is best workaround */

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
    const float mss_val_1_dbl=*var1->mss_val.dp;
    const float mss_val_2_dbl=*var2->mss_val.dp; 
    for(idx=0L;idx<var_sz;idx++) if(var_val.dp[idx] == mss_val_2_dbl) var_val.dp[idx]=mss_val_1_dbl;
  } break;
  case NC_INT: {
    const float mss_val_1_lng=*var1->mss_val.lp;
    const float mss_val_2_lng=*var2->mss_val.lp; 
    for(idx=0L;idx<var_sz;idx++) if(var_val.lp[idx] == mss_val_2_lng) var_val.lp[idx]=mss_val_1_lng;
  } break;
  case NC_SHORT: {
    const float mss_val_1_sht=*var1->mss_val.sp;
    const float mss_val_2_sht=*var2->mss_val.sp; 
    for(idx=0L;idx<var_sz;idx++) if(var_val.sp[idx] == mss_val_2_sht) var_val.sp[idx]=mss_val_1_sht; 
  } break;
  case NC_CHAR: {
    const float mss_val_1_chr=*var1->mss_val.cp;
    const float mss_val_2_chr=*var2->mss_val.cp; 
    for(idx=0L;idx<var_sz;idx++) if(var_val.cp[idx] == mss_val_2_chr) var_val.cp[idx]=mss_val_1_chr; 
  } break;
  case NC_BYTE: {
    const float mss_val_1_byt=*var1->mss_val.bp;
    const float mss_val_2_byt=*var2->mss_val.bp; 
    for(idx=0L;idx<var_sz;idx++) if(var_val.bp[idx] == mss_val_2_byt) var_val.bp[idx]=mss_val_1_byt; 
  } break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  
  /* Un-typecast the pointer to values after access */
  (void)cast_nctype_void(var_typ,&var2->val);
  (void)cast_nctype_void(var_typ,&var1->mss_val);
  (void)cast_nctype_void(var_typ,&var2->mss_val);
  
  return has_mss_val;
} /* end nco_mss_val_cnf() */

void
nco_mss_val_cp /* [fnc] Copy missing value from var1 to var2 */
(const var_sct * const var1, /* I [sct] Variable with template missing value to copy */
 var_sct * const var2) /* I/O [sct] Variable with missing value to fill in/overwrite */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Copy missing value from var1 to var2
     On exit, var2 contains has_mss_val, and mss_val identical to var1
     Type of mss_val in var2 will agree with type of var2
     This maintains assumed consistency between type of variable and type of mss_val in all var_sct's */

  if(!var1->has_mss_val){
    var2->has_mss_val=False;
    if(var2->mss_val.vp != NULL) free(var2->mss_val.vp);
  }else{ /* endif no mss_val in var1 */
    var2->mss_val.vp=(void *)nco_realloc(var2->mss_val.vp,nco_typ_lng(var2->type));
    (void)val_cnf_typ(var1->type,var1->mss_val,var2->type,var2->mss_val);
    var2->has_mss_val=True;
  } /* endif var1 has mss_val */

} /* end nco_mss_val_cp() */
  
int /* O [flg] Variable has missing value on output */
nco_mss_val_get /* [fnc] Update number of attributes, missing_value of variable */
(const int nc_id, /* I [id] netCDF input-file ID */
 var_sct * const var) /* I/O [sct] Variable with missing_value to update */
{
  /* Purpose: Update number of attributes and missing_value attribute of variable
     No matter what type missing_value is on disk, this routine ensures that 
     copy of mss_val in var_sct is stored as same type as host variable.
     Routine does not allow output missing_value to have more than one element */

  /* has_mss_val is typed as int not bool because it was sent to Fortran routines */

  char att_nm[NC_MAX_NAME];
  
  long att_sz;
  int idx;
  
  size_t att_lng;

  nc_type att_typ;

  ptr_unn mss_tmp;
  
  /* Refresh netCDF "mss_val" attribute for this variable */
  var->has_mss_val=False;
  var->mss_val.vp=nco_free(var->mss_val.vp);

  /* Refresh number of attributes for variable */
  (void)nco_inq_varnatts(nc_id,var->id,&var->nbr_att);

  for(idx=0;idx<var->nbr_att;idx++){
    (void)nco_inq_attname(nc_id,var->id,idx,att_nm);
    if((int)strcasecmp(att_nm,"missing_value") != 0) continue;
    (void)nco_inq_att(nc_id,var->id,att_nm,&att_typ,&att_sz);
    if(att_sz != 1 && att_typ != NC_CHAR){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s has %li elements and so will not be used\n",prg_nm_get(),att_nm,var->nm,att_sz);
      continue;
    } /* end if */
    /* if(att_typ != var->type) (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s will be typecast from %s to %s for arithmetic purposes\n",prg_nm_get(),att_nm,var->nm,nco_typ_sng(att_typ),nco_typ_sng(var->type)); */
    /* If we got this far then try to retrieve attribute and make sure it conforms to variable's type */
    var->has_mss_val=True;
    /* Oddly, ARM uses NC_CHAR for type of missing_value, so we must make allowances for this */
    att_lng=att_sz*nco_typ_lng(att_typ);
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
    
    /* Ensure mss_val in memory is stored as same type as variable */
    var->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    (void)val_cnf_typ(att_typ,mss_tmp,var->type,var->mss_val);

    /* Release temporary memory */
    mss_tmp.vp=nco_free(mss_tmp.vp);
    break;
  } /* end loop over att */

  return var->has_mss_val;

} /* end nco_mss_val_get() */

