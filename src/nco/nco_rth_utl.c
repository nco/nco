void 
nco_opr_drv(int cnt,int nco_op_typ,var_sct *var_prc_out, var_sct *var_prc)
{
  /* Purpose: Perform appropriate ncra/ncea operation (avg, min, max, ttl, ...) on operands
     nco_opr_drv() is called within the record loop of ncra, and within file loop of ncea
     These operations perform part, but not all, of the necessary operations for each procedure
     Most arithmetic operations require additional procedures such as normalization be performed after all files/records have been processed */
  
  /* var_prc_out->type and var_prc->type should be equal and thus interchangeable
     var_prc_out->sz and var_prc->sz should be equal and thus interchangeable */
  switch (nco_op_typ){
  case nco_op_min: /* Minimum */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(cnt == 0) (void)var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else	  
      (void)var_min_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;
  case nco_op_max: /* Maximium */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(cnt == 0) (void)var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else
      (void)var_max_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;	
  case nco_op_avg: /* Average */
  case nco_op_sqrt: /* Squareroot will produce the squareroot of the mean */
  case nco_op_ttl: /* Total */
  case nco_op_sqravg: /* Square of the mean */
    (void)var_add(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    break;
  case nco_op_rms: /* Root mean square */
  case nco_op_rmssdn: /* Root mean square normalized by N-1 */
  case nco_op_avgsqr: /* Mean square */
    /* Square values in var_prc first */
    var_multiply(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc->val);
    /* Sum the squares */
    (void)var_add(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    break;
  } /* end switch */
} /* end nco_opr_drv() */

int
nco_op_typ_get(char *nco_op_sng)
{
  /* Purpose: Process '-y' command line argument
     Convert user-specified string to enumerated operation type 
     Return nco_op_avg by default */
  if(!strcmp(nco_op_sng,"min")) return nco_op_min;
  if(!strcmp(nco_op_sng,"max")) return nco_op_max;
  if(!strcmp(nco_op_sng,"total") || !strcmp(nco_op_sng,"ttl")) return nco_op_ttl;
  if(!strcmp(nco_op_sng,"sqravg")) return nco_op_sqravg;
  if(!strcmp(nco_op_sng,"avgsqr")) return nco_op_avgsqr;  
  if(!strcmp(nco_op_sng,"sqrt")) return nco_op_sqrt;
  if(!strcmp(nco_op_sng,"rms")) return nco_op_rms;
  if(!strcmp(nco_op_sng,"rmssdn")) return nco_op_rmssdn;

  return nco_op_avg;
} /* end nco_op_typ_get() */

int
op_prs_rlt(char *op_sng)
/* 
   char *op_sng: I string containing Fortran representation of a reltional operator ("eq","lt"...)
 */
{
  /* Routine to parse the Fortran abbreviation for a relational operator into a unique numeric value
     representing that relation */

  /* Classify the relation */
  if(!strcmp(op_sng,"eq")){
    return nco_op_eq;
  }else if(!strcmp(op_sng,"ne")){
    return nco_op_ne;
  }else if(!strcmp(op_sng,"lt")){
    return nco_op_lt;
  }else if(!strcmp(op_sng,"gt")){
    return nco_op_gt;
  }else if(!strcmp(op_sng,"le")){
    return nco_op_le;
  }else if(!strcmp(op_sng,"ge")){
    return nco_op_ge;
  }else{
    (void)fprintf(stdout,"%s: ERROR %s not registered in op_prs_rlt()\n",prg_nm_get(),op_sng);
    exit(EXIT_FAILURE);
  } /* end else */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return 1;
} /* end op_prs_rlt() */

void
vec_set(nc_type type,long sz,ptr_unn op1,double op2)
/* 
  nc_type type: I netCDF type of operand
  long sz: I size (in elements) of operand
  ptr_unn op1: I values of first operand
  double op2: I value to fill vector with
 */
{
  /* Routine to fill every value of first operand with value of second operand */

  long idx;

  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);

  switch(type){
  case NC_FLOAT:
    for(idx=0;idx<sz;idx++) op1.fp[idx]=op2;
    break;
  case NC_DOUBLE:
    for(idx=0;idx<sz;idx++) op1.dp[idx]=op2;
    break;
  case NC_INT:
    for(idx=0;idx<sz;idx++) op1.lp[idx]=(long)op2; /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_SHORT:
    for(idx=0;idx<sz;idx++) op1.sp[idx]=(short)op2; /* Coerce to avoid C++ compiler assignment warning */
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

} /* end vec_set() */

void
zero_long(long sz,long *op1)
/* 
  long sz: I size (in elements) of operand
  long *op1: I values of first operand
 */
{
  /* Routine to zero value of first operand and store result in first operand. */

  long idx;
  if(op1 != NULL){
    for(idx=0;idx<sz;idx++) op1[idx]=0L;
  }else{
    (void)fprintf(stdout,"%s: ERROR zero_long() asked to zero NULL pointer\n",prg_nm_get());
    exit(EXIT_FAILURE);
  } /* endif */

} /* end zero_long() */

