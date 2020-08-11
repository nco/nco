/* $Header$ */

/* Purpose: Arithmetic controls and utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_rth_utl.h" /* Arithmetic controls and utilities */

nco_rth_prc_rnk_enm /* [enm] Ranked precision of arithmetic type */
nco_rth_prc_rnk /* [fnc] Rank precision of arithmetic type */
(const nc_type nco_typ) /* I [enm] netCDF type of operand */
{
  /* Purpose: Ranked precision of arithmetic type */
  switch(nco_typ){
  case NC_FLOAT: return nco_rth_prc_rnk_float;
  case NC_DOUBLE: return nco_rth_prc_rnk_double;
  case NC_INT: return nco_rth_prc_rnk_int;
  case NC_SHORT: return nco_rth_prc_rnk_short;
  case NC_CHAR: return nco_rth_prc_rnk_char;
  case NC_BYTE: return nco_rth_prc_rnk_byte;
  case NC_UBYTE: return nco_rth_prc_rnk_ubyte;
  case NC_USHORT: return nco_rth_prc_rnk_ushort;
  case NC_UINT: return nco_rth_prc_rnk_uint;
  case NC_INT64: return nco_rth_prc_rnk_int64;
  case NC_UINT64: return nco_rth_prc_rnk_uint64;
  case NC_STRING: return nco_rth_prc_rnk_string;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return (nco_rth_prc_rnk_enm)0;
} /* end nco_rth_prc_rnk() */

void 
nco_opr_nrm /* [fnc] Normalization of arithmetic operations for ncra/nces */
(const int nco_op_typ, /* I [enm] Operation type */
 const int nbr_var_prc, /* I [nbr] Number of processed variables */
 X_CST_PTR_CST_PTR_Y(var_sct,var_prc), /* I [sct] Variables in input file */
 X_CST_PTR_CST_PTR_Y(var_sct,var_prc_out), /* I/O [sct] Variables in output file */
 const char * const rec_nm_fll, /* I [sng] Full name of record dimension */
 const trv_tbl_sct * const trv_tbl) /* I [sct] Traversal table */
{
  /* Purpose: Normalize appropriate ncra/nces operation (avg, min, max, ttl, ...) on operands
     Values of var_prc are not altered but are not const because missing values are cast
     Values of var_prc_out are altered (i.e., normalized) */

  int idx=int_CEWI;
  int nbr_var_prc_cpy;
  int nco_op_typ_cpy;

  nco_op_typ_cpy=nco_op_typ;
  nbr_var_prc_cpy=nbr_var_prc;

#ifdef _OPENMP
# pragma omp parallel for private(idx) shared(nbr_var_prc_cpy,nco_op_typ_cpy,var_prc,var_prc_out)
#endif /* !_OPENMP */
  for(idx=0;idx<nbr_var_prc_cpy;idx++){

    /* In normalizations over record dimension, only normalize those variables that contain current record dimension */
    if(rec_nm_fll){
      nco_bool flg_skp=nco_skp_var(var_prc[idx],rec_nm_fll,trv_tbl);
      if(flg_skp) continue;
    } /* !rec_nm_fll */

    if(var_prc[idx]->is_crd_var){
      /* Return linear averages of coordinates unless computing extrema
	 Prevent coordinate variables from encountering nco_var_nrm_sdn() */
      (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
    }else{ /* !var_prc[idx]->is_crd_var */
      switch(nco_op_typ_cpy){
      case nco_op_avg: /* Normalize sum by tally to create mean */
      case nco_op_mebs: /* Normalize sum by tally to create mean */
      case nco_op_sqrt: /* Normalize sum by tally to create mean */
      case nco_op_sqravg: /* Normalize sum by tally to create mean */
      case nco_op_rms: /* Normalize sum of squares by tally to create mean square */
      case nco_op_avgsqr: /* Normalize sum of squares by tally to create mean square */
        (void)nco_var_nrm(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
        break;
      case nco_op_rmssdn: /* Normalize sum of squares by tally-1 to create mean square for sdn */
        (void)nco_var_nrm_sdn(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
        break;
      case nco_op_min: /* Minimum is already in buffer, do nothing */
      case nco_op_max: /* Maximum is already in buffer, do nothing */
      case nco_op_mibs: /* Minimum absolute value is already in buffer, do nothing */
      case nco_op_mabs: /* Maximum absolute value is already in buffer, do nothing */
        break;
      case nco_op_tabs: /* Total absolute value is already in buffer, stuff missing values into elements with zero tally */
      case nco_op_ttl: /* Total is already in buffer, stuff missing values into elements with zero tally */
        (void)nco_var_tll_zro_mss_val(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val);
        break;
      default:
	nco_dfl_case_generic_err();
        break;
      } /* end switch */
      /* A few operations require additional processing */
      switch(nco_op_typ_cpy) {
      case nco_op_rms: /* Take root of mean of sum of squares to create root mean square */
      case nco_op_rmssdn: /* Take root of sdn mean of sum of squares to create root mean square for sdn */
      case nco_op_sqrt: /* Take root of mean to create root mean */
        (void)nco_var_sqrt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc[idx]->tally,var_prc_out[idx]->val,var_prc_out[idx]->val);
        break;
      case nco_op_sqravg: /* Square mean to create square of the mean (for sdn) */
        (void)nco_var_mlt(var_prc_out[idx]->type,var_prc_out[idx]->sz,var_prc_out[idx]->has_mss_val,var_prc_out[idx]->mss_val,var_prc_out[idx]->val,var_prc_out[idx]->val);
        break;
      case nco_op_avg:
      case nco_op_ttl:
      case nco_op_min:
      case nco_op_max:
      case nco_op_mibs:
      case nco_op_mabs:
      case nco_op_mebs:
      case nco_op_tabs:
      case nco_op_avgsqr:
        break;
      default:
	nco_dfl_case_generic_err();
        break;
      } /* end switch */
    } /* !var_prc[idx]->is_crd_var */
  } /* end (OpenMP parallel for) loop over variables */

} /* end nco_opr_nrm() */

void 
nco_opr_drv /* [fnc] Intermediate control of arithmetic operations for ncra/nces */
(const long idx_rec, /* I [idx] Index of record (ncra), file (ncfe), or group (ncge) in current operation */
 const int nco_op_typ, /* I [enm] Operation type */
 const var_sct * const var_prc, /* I [sct] Variable in input file */
 var_sct * const var_prc_out) /* I/O [sct] Variable in output file */
{
  /* Purpose: Perform appropriate ncra/nces operation (avg, min, max, ttl, ...) on operands
     nco_opr_drv() is called within the record loop of ncra, and within file loop of nces
     These operations perform some, but not all, of necessary operations for each procedure
     Most arithmetic operations require additional procedures such as normalization be performed after all files/records have been processed
     Some operations require special care at initialization
     This determination is based on the idx_rec variable
     When idx_rec == 0, these operations may perform special initializations
     The exact numeric value of idx_rec does not matter
     What matters is whether it is zero or non-zero */
  
  /* NCO's paradigm is that coordinate variables represent grid axes
     Reducing such grids to a single-value must be done
     The most representative value of the grid is the average 
     The total, min, max, rms, etc. of the grid usually makes no sense
     Users are most interested in the mean grid coordinate
     20130112: The same logic applies to CF-style coordinates, e.g., 
     to variables matching CF "bounds", "climatology", and "coordinates" conventions */
  if(var_prc->is_crd_var){
    (void)nco_var_add_tll_ncra(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->wgt_crr,var_prc->wgt_sum,var_prc->val,var_prc_out->val);
    return;
  } /* !var_prc->is_crd_var */

  /* var_prc_out->type and var_prc->type should be equal and thus interchangeable
     var_prc_out->sz and var_prc->sz should be equal and thus interchangeable */
  switch (nco_op_typ){
  case nco_op_min: /* Minimum */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(idx_rec == 0) (void)nco_var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else (void)nco_var_min_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;
  case nco_op_max: /* Maximum */
    /* On first loop, simply copy variables from var_prc to var_prc_out */
    if(idx_rec == 0) (void)nco_var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else (void)nco_var_max_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;	
  case nco_op_mabs: /* Maximum absolute value */
    /* Always take the absolute value of the fresh input
       Then, on first loop, copy variable from var_prc to var_prc_out like min and max
       Following loops, do comparative maximum after taking absolute */
    (void)nco_var_abs(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val);
    if(idx_rec == 0) (void)nco_var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else (void)nco_var_max_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;	
  case nco_op_mebs: /* Mean absolute value */
    /* Always take the absolute value of the fresh input
       Every loop add and increment tally like avg, sqrt, sqravg */
    (void)nco_var_abs(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val);
    (void)nco_var_add_tll_ncra(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->wgt_crr,var_prc->wgt_sum,var_prc->val,var_prc_out->val);
    break;	
  case nco_op_mibs: /* Mean absolute value */
    /* Always take the absolute value of the fresh input
       Then, on first loop, copy variable from var_prc to var_prc_out like min and max
       Following loops, do comparative minimum after taking absolute value */
    (void)nco_var_abs(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val);
    if(idx_rec == 0) (void)nco_var_copy(var_prc->type,var_prc->sz,var_prc->val,var_prc_out->val); else (void)nco_var_min_bnr(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc_out->val);
    break;	
  case nco_op_tabs: /* Total absolute value */
    /* Same as ttl but take absolute first */
    (void)nco_var_abs(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val);
    if(idx_rec == 0) (void)nco_var_copy_tll(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val); else (void)nco_var_add_tll_ncra(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->wgt_crr,var_prc->wgt_sum,var_prc->val,var_prc_out->val);
    break;
  case nco_op_ttl: /* Total */
    /* NB: Copying input to output on first loop for nco_op_ttl, in similar manner to nco_op_[max/min], can work
       However, copying with nco_var_copy() would not change the tally variable, leaving it equal to zero
       Then an extra step would be necessary to set tally equal to one where missing values were not present
       Otherwise, e.g., ensemble averages of one file would never have non-zero tallies
       Hence, use special nco_var_copy_tll() function to copy and change tally only in first loop iteration
       This way, tally is self-consistent with var_prc_out at all times
       Moreover, running total must never be set to missing_value, because subsequent additions
       (with nco_var_add_tll_ncra()) only check new addend (not running sum) against missing value.
       Hence (as of 20120521) nco_var_copy_tll() specifically resets sum to zero rather than to missing value
       Parent function (e.g., ncra.c) must post-process ttl buffers nco_op_ttl with nco_var_tll_zro_mss_val() */
    if(idx_rec == 0) (void)nco_var_copy_tll(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->val,var_prc_out->val); else (void)nco_var_add_tll_ncra(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->wgt_crr,var_prc->wgt_sum,var_prc->val,var_prc_out->val);
    break;
  case nco_op_avg: /* Average */
  case nco_op_sqrt: /* Squareroot will produce the squareroot of the mean */
  case nco_op_sqravg: /* Square of the mean */
    /* These operations all require subsequent normalization, where degenerate tallies are accounted for
       Thus, they all call nco_var_add_tll_ncra() every iteration, without special treatment on first iteration */
    (void)nco_var_add_tll_ncra(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->wgt_crr,var_prc->wgt_sum,var_prc->val,var_prc_out->val);
    break;
  case nco_op_rms: /* Root mean square */
  case nco_op_rmssdn: /* Root mean square normalized by N-1 */
  case nco_op_avgsqr: /* Mean square */
    /* Square values in var_prc first */
    nco_var_mlt(var_prc->type,var_prc->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->val,var_prc->val);
    /* Sum the squares */
    (void)nco_var_add_tll_ncra(var_prc_out->type,var_prc_out->sz,var_prc->has_mss_val,var_prc->mss_val,var_prc->tally,var_prc->wgt_crr,var_prc->wgt_sum,var_prc->val,var_prc_out->val);
    break;
  default:
    nco_dfl_case_generic_err();
    break; /* [enm] Nil or undefined operation type */
  } /* end switch */
} /* end nco_opr_drv() */

const char * /* O [enm] Arithmetic operation */
nco_op_typ_cf_sng /* [fnc] Convert arithmetic operation type enum to string */
(const int nco_op_typ) /* I [enm] Arithmetic operation type */
{
  /* Purpose: Convert arithmetic operation type enum to string for use in CF Cell Methods 
     http://cfconventions.org/Data/cf-conventions/cf-conventions-1.7/build/cf-conventions.html#cell-methods */
  switch(nco_op_typ){
  case nco_op_avg: return "mean"; break; /* [enm] Average */
  case nco_op_min: return "minimum"; break; /* [enm] Minimum value */
  case nco_op_max: return "maximum"; break; /* [enm] Maximum value */
  case nco_op_ttl: return "sum"; break; /* [enm] Linear sum */
  case nco_op_tabs: return "sum_absolute_value"; break; /* [enm] Total absolute value */
  case nco_op_mabs: return "maximum_absolute_value"; break; /* [enm] Maximum absolute value */
  case nco_op_mebs: return "mean_absolute_value"; break; /* [enm] Mean absolute value */
  case nco_op_mibs: return "minimum_absolute_value"; break; /* [enm] Minimum absolute value */
  case nco_op_sqravg: return "square_of_mean"; break; /* [enm] Square of mean */
  case nco_op_avgsqr: return "variance"; break; /* [enm] Mean of sum of squares */
  case nco_op_sqrt: return "square_root_of_mean"; break; /* [enm] Square root of mean */
  case nco_op_rms: return "root_mean_square"; break; /* [enm] Root-mean-square (normalized by N) */
  case nco_op_rmssdn: return "root_mean_square_nm1"; break; /* [enm] Root-mean square normalized by N-1 */
  case nco_op_add:
  case nco_op_sbt:
  case nco_op_mlt:
  case nco_op_dvd:
  case nco_op_nil:
  default:
    nco_dfl_case_generic_err();
    return "BROKEN"; /* CEWI */
    break; /* [enm] Nil or undefined operation type */
  } /* end switch */
} /* end nco_op_typ_cf_sng() */

int /* O [enm] Arithmetic operation */
nco_op_typ_get /* [fnc] Convert user-specified operation into operation key */
(const char * const nco_op_sng) /* I [sng] User-specified operation */
{
  /* Purpose: Process '-y' command line argument
     Convert user-specified string to enumerated operation type */
  const char fnc_nm[]="nco_op_typ_get()"; /* [sng] Function name */
  char *nco_prg_nm; /* [sng] Program name */
  int nco_prg_id; /* [enm] Program ID */

  nco_prg_nm=nco_prg_nm_get(); /* [sng] Program name */
  nco_prg_id=nco_prg_id_get(); /* [enm] Program ID */

  if(nco_op_sng == NULL){
    /* If nco_op_typ_get() is called when user-specified option string is NULL, 
       then operation type may be implied by program name itself */
    if(!strcmp(nco_prg_nm,"ncadd")) return nco_op_add;
    if(!strcmp(nco_prg_nm,"mpncbo")) return nco_op_sbt;
    if(!strcmp(nco_prg_nm,"mpncdiff")) return nco_op_sbt;
    if(!strcmp(nco_prg_nm,"ncbo")) return nco_op_sbt;
    if(!strcmp(nco_prg_nm,"ncdiff")) return nco_op_sbt;
    if(!strcmp(nco_prg_nm,"ncsub")) return nco_op_sbt;
    if(!strcmp(nco_prg_nm,"ncsubtract")) return nco_op_sbt;
    if(!strcmp(nco_prg_nm,"ncmult")) return nco_op_mlt;
    if(!strcmp(nco_prg_nm,"ncmultiply")) return nco_op_mlt;
    if(!strcmp(nco_prg_nm,"ncdivide")) return nco_op_dvd;
    (void)fprintf(stderr,"%s: ERROR %s reports empty user-specified operation string in conjunction with unknown or ambiguous executable name %s\n",nco_prg_nm,fnc_nm,nco_prg_nm);
    nco_exit(EXIT_FAILURE);
  } /* endif */

  if(!strcmp(nco_op_sng,"avg") || !strcmp(nco_op_sng,"average") || !strcmp(nco_op_sng,"mean")) return nco_op_avg;
  if(!strcmp(nco_op_sng,"avgsqr")) return nco_op_avgsqr;
  if(!strcmp(nco_op_sng,"mabs") || !strcmp(nco_op_sng,"maximum_absolute_value")) return nco_op_mabs;
  if(!strcmp(nco_op_sng,"mebs") || !strcmp(nco_op_sng,"mean_absolute_value")) return nco_op_mebs;
  if(!strcmp(nco_op_sng,"mibs") || !strcmp(nco_op_sng,"minimum_absolute_value")) return nco_op_mibs;
  if(!strcmp(nco_op_sng,"max") || !strcmp(nco_op_sng,"maximum")) return nco_op_max;
  if(!strcmp(nco_op_sng,"min") || !strcmp(nco_op_sng,"minimum")) return nco_op_min;
  if(!strcmp(nco_op_sng,"rms") || !strcmp(nco_op_sng,"root-mean-square")) return nco_op_rms;
  if(!strcmp(nco_op_sng,"rmssdn")) return nco_op_rmssdn;
  if(!strcmp(nco_op_sng,"sqravg")) return nco_op_sqravg;
  if(!strcmp(nco_op_sng,"sqrt") || !strcmp(nco_op_sng,"square-root")) return nco_op_sqrt;
  if(!strcmp(nco_op_sng,"total") || !strcmp(nco_op_sng,"ttl") || !strcmp(nco_op_sng,"sum")) return nco_op_ttl;
  if(!strcmp(nco_op_sng,"tabs") || !strcmp(nco_op_sng,"ttlabs") || !strcmp(nco_op_sng,"sumabs")) return nco_op_tabs;

  if(!strcmp(nco_op_sng,"add") || !strcmp(nco_op_sng,"+") || !strcmp(nco_op_sng,"addition")) return nco_op_add;
  if(!strcmp(nco_op_sng,"sbt") || !strcmp(nco_op_sng,"-") || !strcmp(nco_op_sng,"dff") || !strcmp(nco_op_sng,"diff") || !strcmp(nco_op_sng,"sub") || !strcmp(nco_op_sng,"subtract") || !strcmp(nco_op_sng,"subtraction")) return nco_op_sbt;
  if(!strcmp(nco_op_sng,"dvd") || !strcmp(nco_op_sng,"/") || !strcmp(nco_op_sng,"divide") || !strcmp(nco_op_sng,"division")) return nco_op_dvd;
  if(!strcmp(nco_op_sng,"mlt") || !strcmp(nco_op_sng,"*") || !strcmp(nco_op_sng,"mult") || !strcmp(nco_op_sng,"multiply") || !strcmp(nco_op_sng,"multiplication")) return nco_op_mlt;

  (void)fprintf(stderr,"%s: ERROR %s reports unknown user-specified operation type \"%s\"\n",nco_prg_nm,fnc_nm,nco_op_sng);
  (void)fprintf(stderr,"%s: HINT Valid operation type (op_typ) choices:\n",nco_prg_nm);
  if(nco_prg_id == ncbo) (void)fprintf(stderr,"addition: add,+,addition\nsubtraction: sbt,-,dff,diff,sub,subtract,subtraction\nmultiplication: mlt,*,mult,multiply,multiplication\ndivision: dvd,/,divide,division\n"); else (void)fprintf(stderr,"min or minimum, max or maximum, mabs or maximum_absolute_value, mebs or mean_absolute_value, mibs or maximum_absolute_value, tabs or ttlabs or sumabs, ttl or total or sum, avg or average or mean, sqrt or square-root, sqravg, avgsqr, rms or root-mean-square, rmssdn\n");
  nco_exit(EXIT_FAILURE);
  return False; /* Statement should not be reached */
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
    (void)fprintf(stdout,"%s: ERROR %s not registered in nco_op_prs_rlt()\n",nco_prg_nm_get(),op_sng);
    nco_exit(EXIT_FAILURE);
  } /* end else */

  /* Some compilers, e.g., SGI cc, need return statement to end non-void functions */
  return False; /* Statement should not be reached */
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
    for(idx=0;idx<sz;idx++) op1.fp[idx]=(float)op2;
    break;
  case NC_DOUBLE:
    for(idx=0;idx<sz;idx++) op1.dp[idx]=op2;
    break;
  case NC_INT:
    for(idx=0;idx<sz;idx++) op1.ip[idx]=(nco_int)lrint(op2); /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_SHORT:
    for(idx=0;idx<sz;idx++) op1.sp[idx]=(nco_short)lrint(op2); /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_USHORT:
    for(idx=0;idx<sz;idx++) op1.usp[idx]=(nco_ushort)lrint(op2); /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_UINT:
    for(idx=0;idx<sz;idx++) op1.uip[idx]=(nco_uint)lrint(op2); /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_INT64:
    for(idx=0;idx<sz;idx++) op1.i64p[idx]=(nco_int64)llrint(op2); /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_UINT64:
    for(idx=0;idx<sz;idx++) op1.ui64p[idx]=(nco_uint64)llrint(op2); /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_BYTE:
    for(idx=0;idx<sz;idx++) op1.bp[idx]=(nco_byte)llrint(op2); /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_UBYTE:
    for(idx=0;idx<sz;idx++) op1.ubp[idx]=(nco_ubyte)llrint(op2); /* Coerce to avoid C++ compiler assignment warning */
    break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
    default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* NB: it is not neccessary to un-typecast pointers to values after access 
     because we have only operated on local copies of them. */

} /* end vec_set() */

void
nco_zero_double /* [fnc] Zero all values of double array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 double * restrict const op1) /* I/O [nbr] Array to be zeroed */
{
  /* Purpose: Zero all values of long array */

  /* Presumably this old method used until 20050321, and then again after 20120330,
     is slower than memset() because of pointer de-referencing. 
     However, it does have the virtue of being correct. */
  if(op1 == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_zero_double() asked to zero NULL pointer\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  size_t sz_byt; /* [B] Number of bytes in variable buffer */
  sz_byt=(size_t)sz*sizeof(long);
  (void)memset((void *)op1,0,sz_byt);

} /* end nco_zero_double() */

void
nco_zero_long /* [fnc] Zero all values of long array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 long * restrict const op1) /* I/O [nbr] Array to be zeroed */
{
  /* Purpose: Zero all values of long array */

  /* Presumably this old method used until 20050321, and then again after 20120330,
     is slower than memset() because of pointer de-referencing. 
     However, it does have the virtue of being correct. */
  if(op1 == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_zero_long() asked to zero NULL pointer\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  size_t sz_byt; /* [B] Number of bytes in variable buffer */
  sz_byt=(size_t)sz*sizeof(long);
  (void)memset((void *)op1,0,sz_byt);

} /* end nco_zero_long() */

void
nco_set_long /* [fnc] Set all values of long array */
(const long sz, /* I [nbr] Size (in elements) of operand */
 const long val, /* I [] Number to set array to */
 long * restrict const op1) /* I/O [nbr] Array to be set */
{
  /* Purpose: Set all values of long array to input value */

  long idx;
  if(op1 == NULL){
    (void)fprintf(stdout,"%s: ERROR nco_set_long() asked to set NULL pointer\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* endif */
  for(idx=0;idx<sz;idx++) op1[idx]=val;

} /* end nco_set_long() */
