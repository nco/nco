ptr_unn
mss_val_mk(nc_type type)
/* 
  nc_type type: I netCDF type of operand
  ptr_unn mss_val_mk(): O ptr_unn containing default missing value for type type
 */
{
  /* Threads: Routine is thread safe and makes no unsafe routines */
  /* Routine to return a pointer union containing default missing value for type type */

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
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */

  /* Un-typecast pointer to values after access */
  (void)cast_nctype_void(type,&mss_val);
  
  return mss_val;

} /* end mss_val_mk() */
  
void
mss_val_cp(var_sct *var1,var_sct *var2)
/* 
  var_sct *var1: I variable structure with template missing value to copy
  var_sct *var2: I/O variable structure with missing value to fill in/overwrite
  mss_val_cp(): 
 */
{
  /* Threads: Routine is thread safe and calls no unsafe routines */
  /* Purpose: Copy missing value from var1 to var2
     On exit, var2 contains has_mss_val, and mss_val identical to var1
     Type of mss_val in var2 will agree with type of var2
     This maintains assumed consistency between type of variable and
     type of mss_val in all var_sct's
   */

  if(!var1->has_mss_val){
    var2->has_mss_val=False;
    if(var2->mss_val.vp != NULL) free(var2->mss_val.vp);
  }else{ /* endif no mss_val in var1 */
    var2->mss_val.vp=(void *)nco_realloc(var2->mss_val.vp,nco_typ_lng(var2->type));
    (void)val_conform_type(var1->type,var1->mss_val,var2->type,var2->mss_val);
    var2->has_mss_val=True;
  } /* endif var1 has mss_val */

} /* end mss_val_cp() */
  
int
mss_val_get(int nc_id,var_sct *var)
/* 
   int nc_id: I netCDF input-file ID
   var_sct *var: I/O variable structure
   int mss_val_get(): O flag whether variable has missing value on output or not
 */
{
  /* Purpose: Update number of attributes and missing_value attribute of variable
     No matter what type of missing_value is as stored on disk, this routine
     ensures that the copy in mss_val in the var_sct is stored in type as
     the host variable.
     Routine does not allow output missing_value to have more than one element */

  /* has_mss_val is defined typed as int not bool because it is often sent to Fortran routines */

  char att_nm[NC_MAX_NAME];
  
  long att_sz;
  int idx;
  
  long att_len;

  nc_type att_typ;

  ptr_unn mss_tmp;
  
  /* Refresh netCDF "mss_val" attribute for this variable */
  var->has_mss_val=False;
  var->mss_val.vp=nco_free(var->mss_val.vp);

  /* Refresh number of attributes for variable */
  (void)nco_inq_varnatts(var->nc_id,var->id,&var->nbr_att);

  for(idx=0;idx<var->nbr_att;idx++){
    (void)nco_inq_attname(var->nc_id,var->id,idx,att_nm);
    if((int)strcasecmp(att_nm,"missing_value") != 0) continue;
    (void)nco_inq_att(var->nc_id,var->id,att_nm,&att_typ,&att_sz);
    if(att_sz != 1 && att_typ != NC_CHAR){
      (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s has %li elements and so will not be used\n",prg_nm_get(),att_nm,var->nm,att_sz);
      continue;
    } /* end if */
    /* if(att_typ != var->type) (void)fprintf(stderr,"%s: WARNING the \"%s\" attribute for %s will be typecast from %s to %s for arithmetic purposes\n",prg_nm_get(),att_nm,var->nm,nco_typ_sng(att_typ),nco_typ_sng(var->type)); */
    /* If we got this far then try to retrieve attribute and make sure it conforms to variable's type */
    var->has_mss_val=True;
    /* Oddly, ARM uses NC_CHAR for type of missing_value, so we must make allowances for this */
    att_len=att_sz*nco_typ_lng(att_typ);
    mss_tmp.vp=(void *)nco_malloc(att_len);
    (void)nco_get_att(var->nc_id,var->id,att_nm,mss_tmp.vp,att_typ);
    if(att_typ == NC_CHAR){
      /* NUL-terminate missing value string */
      if(mss_tmp.cp[att_len-1] != '\0'){
	att_len++;
	mss_tmp.vp=(void *)nco_realloc(mss_tmp.vp,att_len);
	mss_tmp.cp[att_len-1]='\0';
	/* Un-typecast pointer to values after access */
	(void)cast_nctype_void(att_typ,&mss_tmp);
      } /* end if */
    } /* end if */
    
    /* Ensure mss_val in memory is stored as same type as variable */
    var->mss_val.vp=(void *)nco_malloc(nco_typ_lng(var->type));
    (void)val_conform_type(att_typ,mss_tmp,var->type,var->mss_val);

    /* Release temporary memory */
    mss_tmp.vp=nco_free(mss_tmp.vp);
    break;
  } /* end loop over att */

  return var->has_mss_val;

} /* end mss_val_get() */

