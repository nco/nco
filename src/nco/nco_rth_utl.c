/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_rth_utl.c,v 1.8 2002-08-21 11:47:42 zender Exp $ */

/* Purpose: Arithmetic controls and utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_rth_utl.h" /* Arithmetic controls and utilities */

void 
nco_opr_drv /* [fnc] Intermediate control of arithmetic operations for ncra/ncea */
(const long idx_rec, /* I [idx] Index of current record */
 const int nco_op_typ, /* I [enm] Operation type */
 var_sct * const var_prc_out, /* I/O [sct] Variable in output file */
 const var_sct * const var_prc) /* I [sct] Variable in input file */
{
  /* Purpose: Perform appropriate ncra/ncea operation (avg, min, max, ttl, ...) on operands
     nco_opr_drv() is called within the record loop of ncra, and within file loop of ncea
     These operations perform some, but not all, of necessary operations for each procedure
     Most arithmetic operations require additional procedures such as normalization be performed after all files/records have been processed */
  
  /* var_prc_out->type and var_prc->type should be equal and thus interchangeable
     var_prc_out->sz and var_prc->sz should be equal and thus interchangeable */
  switch (nco_op_typ){
  case nco_op_min: /* Minimum */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(idx_rec == 0) (void)var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else	  
      (void)nco_var_min_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;
  case nco_op_max: /* Maximium */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(idx_rec == 0) (void)var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else
      (void)nco_var_max_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;	
  case nco_op_avg: /* Average */
  case nco_op_sqrt: /* Squareroot will produce the squareroot of the mean */
  case nco_op_ttl: /* Total */
  case nco_op_sqravg: /* Square of the mean */
    (void)nco_var_add(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    break;
  case nco_op_rms: /* Root mean square */
  case nco_op_rmssdn: /* Root mean square normalized by N-1 */
  case nco_op_avgsqr: /* Mean square */
    /* Square values in var_prc first */
    nco_var_mlt(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc->val);
    /* Sum the squares */
    (void)nco_var_add(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val);
    break;
  } /* end switch */
} /* end nco_opr_drv() */

int /* O [enm] Arithmetic operation */
nco_op_typ_get /* [fnc] Convert user-specified operation into operation key */
(const char * const nco_op_sng) /* I [sng] User-specified operation */
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

int /* O [enm] Relational operation */
nco_op_prs_rlt /* [fnc] Convert Fortran abbreviation for relational operator into NCO operation key */
(const char * const op_sng) /* I [sng] Fortran representation of relational operator */
{
  /* Purpose: Convert Fortran abbreviation for relational operator into NCO operation key */

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
    (void)fprintf(stdout,"%s: ERROR %s not registered in nco_op_prs_rlt()\n",prg_nm_get(),op_sng);
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return 1;
} /* end nco_op_prs_rlt() */

void
vec_set /* [fnc] Fill every value of first operand with value of second operand */
(const nc_type type, /* I [enm] netCDF type of operand */
 const long sz, /* I [nbr] size (in elements) of operand */
 ptr_unn op1, /* I [sct] Values of first operand */
 const double op2) /* I [frc] Value to fill vector with */
{
  /* Purpose: Fill every value of first operand with value of second operand */
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
    default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end vec_set() */

void
nco_zero_long /* [fnc] Zero all values of long array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 long * const op1) /* I/O [nbr] Array to be zeroed */
{
  /* Purpose: Zero all values of long array */

  long idx;
  if(op1 != NULL){
    for(idx=0;idx<sz;idx++) op1[idx]=0L;
  }else{
    (void)fprintf(stdout,"%s: ERROR nco_zero_long() asked to zero NULL pointer\n",prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */

} /* end nco_zero_long() */

