
/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap_utl.c,v 1.80 2002-12-28 07:07:21 zender Exp $ */

/* Purpose: netCDF arithmetic processor */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "ncap.h" /* netCDF arithmetic processor */

var_sct *
ncap_var_init(const char * const var_nm,prs_sct *prs_arg)
{
  /* Purpose: Initialize variable structure, retrieve variable values from disk
     Parser calls ncap_var_init() when it encounters a new RHS variable */

  int i;
  int j;
  int nbr_dmn_var;
  int *dim_id = NULL;
  int var_id;
  int rcd;
  int fl_id;
  int index =0;
  var_sct *var;
  dmn_sct *dmn_in;  
  dmn_sct **dim_new = NULL_CEWI;



  /* Check output file for var */  
  rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
  if(rcd == NC_NOERR){
    fl_id=prs_arg->out_id;
  }else{
    /* Check input file for ID */
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
    if(rcd != NC_NOERR ){
      /* return null if var not in input or output file */
      (void)fprintf(stderr,"WARNING unable to find %s in %s or %s\n",var_nm,prs_arg->fl_in,prs_arg->fl_out);     
      return (var_sct *)NULL;
    }
   
    /* find dimensions used in var and add any new ones not already in the */
    /* output list prs_arg->dmn_out  and the output file */                                   

      (void)nco_redef(prs_arg->out_id);
      fl_id=prs_arg->in_id;

      (void)nco_inq_varndims(fl_id,var_id,&nbr_dmn_var);
      if( nbr_dmn_var >0 ) {
        dim_id = (int *)nco_malloc(nbr_dmn_var*sizeof(int));
           
        (void)nco_inq_vardimid(fl_id,var_id,dim_id);
        for(i=0 ; i < nbr_dmn_var ; i++) 
	  for(j=0 ;j < prs_arg->nbr_dmn_in ; j++){
            
            /* de-referenece */
	    dmn_in = prs_arg->dmn_in[j]; 
	    if( dim_id[i] != dmn_in->id || dmn_in->xrf ) continue;
      
            /* define dimension  in (prs_arg->dmn_out) */ 
           dim_new = nco_dmn_out_grow(prs_arg); 
           *dim_new=nco_dmn_dpl(dmn_in);
           (void)nco_dmn_xrf(*dim_new,dmn_in);
           /* write dimension to output file */
           (void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,dim_new,1);
           /* printf("Dimension %s defined\n",(*dim_new)->nm); */
	    break;
	  } /* end for j */
	(void)nco_free(dim_id);
      } 
      (void)nco_enddef(prs_arg->out_id);  
  
  } /* end else */
  
  if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: parser VAR action called ncap_var_init() to retrieve %s from disk\n",prg_nm_get(),var_nm);
  var=nco_var_fll(fl_id,var_id,var_nm,*(prs_arg->dmn_out),*(prs_arg->nbr_dmn_out));
  var->nm=(char *)nco_malloc((strlen(var_nm)+1UL)*sizeof(char));
  (void)strcpy(var->nm,var_nm);
  var->tally=(long *)nco_malloc(var->sz*sizeof(long));
  (void)nco_zero_long(var->sz,var->tally);
  var->val.vp=(void *)nco_malloc(var->sz*nco_typ_lng(var->type));
  /* Retrieve variable values from disk into memory */
  (void)nco_var_get(fl_id,var);
  /* (void)nco_var_free(var_nm);*/
  /* free(var_nm->nm);*/
  /* var=nco_var_upk(var); */
  return var;
} /* end ncap_var_init() */



dmn_sct **
nco_dmn_out_grow(prs_sct * prs_arg)
{
  /* expand dimension list by 1 */
  /* and return pointer to the newly created member */
  int *size;
  size = prs_arg->nbr_dmn_out;

*(prs_arg->dmn_out)=(dmn_sct **)nco_realloc(*(prs_arg->dmn_out),(++*size)*sizeof(dmn_sct *));

 return (*(prs_arg->dmn_out) + (*size -1));

}

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

  /* Write/overwrite scale_factor and add_offset attributes */
  if(var->pck_ram){ /* Variable is packed in memory */
    if(var->has_scl_fct) (void)nco_put_att(prs_arg->out_id,var_out_id,"scale_factor",var->typ_upk,1,var->scl_fct.vp);
    if(var->has_add_fst) (void)nco_put_att(prs_arg->out_id,var_out_id,"add_offset",var->typ_upk,1,var->add_fst.vp);
  } /* endif pck_ram */
  (void)nco_enddef(prs_arg->out_id);
  
  /* Write variable */ 
  if(var->nbr_dim == 0){
    (void)nco_put_var1(prs_arg->out_id,var_out_id,0L,var->val.vp,var->type);
  }else{
    (void)nco_put_vara(prs_arg->out_id,var_out_id,var->srt,var->cnt,var->val.vp,var->type);
  } /* end else */
  
  return 1;
} /* end ncap_var_write() */

sym_sct *
ncap_sym_init(char *sym_nm,double (*fnc_dbl)(double),float (*fnc_flt)(float))
{ 
  /* Purpose: Allocate space for sym_sct then initialize */
  sym_sct *symbol;
  symbol=(sym_sct *)nco_malloc(sizeof(sym_sct));
  symbol->nm=(char *)strdup(sym_nm);
  symbol->fnc_dbl=fnc_dbl;
  symbol->fnc_flt=fnc_flt;
  return symbol;
} /* end ncap_sym_init() */

ptr_unn
ncap_scv_2_ptr_unn(scv_sct scv)
{
  /* Purpose: Convert scv_sct to ptr_unn
     malloc() appropriate space for single type
     NB: Does not work on strings */
  ptr_unn val;
  nc_type type=scv.type;
  val.vp=(void *)nco_malloc(nco_typ_lng(type));
  (void)cast_void_nctype(type,&val);
  
  switch(type){
  case NC_FLOAT: *val.fp=scv.val.f; break;
  case NC_DOUBLE: *val.dp=scv.val.d; break;
  case NC_INT: *val.lp=scv.val.l; break;
  case NC_SHORT: *val.sp=scv.val.s; break;
  case NC_BYTE: *val.bp=scv.val.b; break;
  case NC_CHAR: break; /* do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  (void)cast_nctype_void(type,&val);
  return val;
} /* end ncap_scv_2_ptr_unn() */

var_sct *
ncap_var_var_add(var_sct *var_1,var_sct *var_2)
/* var_sct *var_1: input variable structure containing first operand
   var_sct *var_2: input variable structure containing second operand
   var_sct *ncap_var_var_add(): output sum of input variables */
{
  /* Purpose: Add two variables */
  var_sct *var_nsw;
  
  (void)ncap_var_retype(var_1,var_2);
  var_nsw=nco_var_dpl(var_2);
  (void)ncap_var_cnf_dmn(&var_1,&var_nsw);
  /* fxm: bug in nco_var_add. missing_value is not carried over to var_nsw in result when var_1->has_mss_val is true */
  if(var_1->has_mss_val){
    /*    (void)nco_var_add(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->tally,var_1->val,var_nsw->val);*/
    (void)nco_var_add_no_tally(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_nsw->val);
  }else{
    /*    (void)nco_var_add(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->tally,var_1->val,var_nsw->val);*/
    (void)nco_var_add_no_tally(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->val,var_nsw->val);
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
  var_nsw=nco_var_dpl(var_2);
  (void)ncap_var_cnf_dmn(&var_1,&var_nsw);
  if(var_1->has_mss_val){
    (void)nco_var_sbt(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_nsw->val);
  }else{
    (void)nco_var_sbt(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->val,var_nsw->val);
  }/* end else */
  return var_nsw;
} /* end ncap_var_var_sub() */

var_sct *
ncap_var_var_mlt(var_sct *var_1,var_sct *var_2)
/* var_sct *var_1: input variable structure containing first operand
   var_sct *var_2: input variable structure containing second operand
   var_sct *ncap_var_var_mlt(): output multiplication of individual elements */
{
  /* Purpose: Multiplication of variables */
  var_sct *var_nsw;
  (void)ncap_var_retype(var_1,var_2);
  var_nsw=nco_var_dpl(var_2);
  (void)ncap_var_cnf_dmn(&var_1,&var_nsw);
  if(var_1->has_mss_val) {
     (void)nco_var_mlt(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_nsw->val);
  }else{
     (void)nco_var_mlt(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->val,var_nsw->val);
  }
  return var_nsw;
} /* end ncap_var_var_mlt() */

var_sct *
ncap_var_fnc(var_sct *var_in,sym_sct *app)
{
  /* Purpose: Evaluate fnc_dbl(var) or fnc_flt(var) for each value in variable
     Float and double functions are in app */
  long idx;
  long sz;
  ptr_unn op1;
  var_sct *var;

  /* Promote variable to NC_FLOAT */
  if(var_in->type < NC_FLOAT) var_in=nco_var_cnf_typ((nc_type)NC_FLOAT,var_in);
  var=nco_var_dpl(var_in);
 
  op1=var->val;
  sz=var->sz;
  (void)cast_void_nctype(var->type,&op1);
  if(var->has_mss_val) (void)cast_void_nctype(var->type,&(var->mss_val));
  
  switch(var->type){ 
  case NC_DOUBLE: {
    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=(*(app->fnc_dbl))(op1.dp[idx]);
    }else{
      double mss_val_dbl=*(var->mss_val.dp); /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
        if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=(*(app->fnc_dbl))(op1.dp[idx]);
      } /* end for */
    } /* end else */
   break;
  }
  case NC_FLOAT: {
    if(!var->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=(*(app->fnc_flt))(op1.fp[idx]);
    }else{
      float mss_val_flt=*(var->mss_val.fp); /* Temporary variable reduces dereferencing */
      for(idx=0;idx<sz;idx++){
        if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=(*(app->fnc_flt))(op1.fp[idx]);
      } /* end for */
    } /* end else */
   break;
  }
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */

  if(var->has_mss_val) (void)cast_nctype_void(var->type,&(var->mss_val));
  return var;
} /* end ncap_var_fnc() */

var_sct *
ncap_var_var_dvd(var_sct *var_1,var_sct *var_2)
/* var_sct *var_1: input variable structure containing first operand
   var_sct *var_2: input variable structure containing second operand
   var_sct *ncap_var_dvd(): output quotient of individual elements */
{
  /* Routine called by parser */
  var_sct *var_nsw;
  (void)ncap_var_retype(var_1,var_2);
  var_nsw=nco_var_dpl(var_2);
  (void)ncap_var_cnf_dmn(&var_1,&var_nsw);
  if(var_1->has_mss_val) {
    (void)nco_var_dvd(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->val,var_nsw->val);
  }else{
    (void)nco_var_dvd(var_1->type,var_1->sz,var_nsw->has_mss_val,var_nsw->mss_val,var_1->val,var_nsw->val);
  } /* end else */ 
 return var_nsw;
} /* end ncap_var_var_dvd() */

var_sct *
ncap_var_scv_add(var_sct *var,scv_sct scv)
{
  /* Purpose: add the value in scv to each element of var */
  var_sct *var_nsw;
  (void)ncap_var_scv_cnf_typ_hgh_prc(&var,&scv);
  var_nsw=nco_var_dpl(var);
  (void)var_scv_add(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&scv);
  
  return var_nsw;
} /* end ncap_var_scv_add() */

var_sct *
ncap_var_scv_sub(var_sct *var,scv_sct scv)
{
  /* Purpose: Subtract value in scv from each element of var */
  var_sct *var_nsw;
  (void)ncap_var_scv_cnf_typ_hgh_prc(&var,&scv);
  var_nsw=nco_var_dpl(var);
  (void)ncap_scv_minus(&scv);
  (void)var_scv_add(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&scv);
  
  return var_nsw;
} /* end ncap_var_scv_sub() */

var_sct *
ncap_var_scv_mlt(var_sct *var,scv_sct scv)
{
  /* Purpose: Multiply variable by value in scv */
  var_sct *var_nsw;
  /* fxm: 20020421 Old method was to always convert scv to var precision 
     (void)scv_conform_type(var->type,&scv); */
  (void)ncap_var_scv_cnf_typ_hgh_prc(&var,&scv);
  var_nsw=nco_var_dpl(var);
  (void)var_scv_mlt(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&scv);
  
  return var_nsw;
} /* end ncap_var_scv_mlt */

var_sct *
ncap_var_scv_dvd(var_sct *var,scv_sct scv)
{
  /* Purpose: Divide each element of var by value in scv */
  var_sct *var_nsw;
  (void)ncap_var_scv_cnf_typ_hgh_prc(&var,&scv);
  var_nsw=nco_var_dpl(var);
  (void)var_scv_dvd(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&scv);
  
  return var_nsw;
} /* end ncap_var_scv_dvd */

var_sct *
ncap_var_scv_mod(var_sct *var,scv_sct scv)
{
  /* Purpose: var % scv, take modulus of each element of var with value in scv */
  
  var_sct *var_nsw;
  (void)ncap_var_scv_cnf_typ_hgh_prc(&var,&scv);
  var_nsw=nco_var_dpl(var);
  (void)var_scv_mod(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val,&scv);
  
  return var_nsw;
} /* ncap_var_scv_mod */

var_sct *
ncap_var_abs(var_sct *var)
{
  /* Purpose: Find absolute value of each element of var */
  var_sct *var_nsw;
  var_nsw=nco_var_dpl(var);
  (void)nco_var_abs(var->type,var->sz,var->has_mss_val,var->mss_val,var_nsw->val);
  return var_nsw;
} /* end ncap_var_abs */

var_sct *
ncap_var_scv_pwr(var_sct *var_in,scv_sct scv)
{
  /* Purpose: Raise var to power in scv
     All values converted to type double before operation */
  long idx;
  long sz;
  ptr_unn op1;
  var_sct *var;

  /* Promote scv and var to NC_FLOAT */
  if(var_in->type < NC_FLOAT) var_in=nco_var_cnf_typ((nc_type)NC_FLOAT,var_in);
  var=nco_var_dpl(var_in);
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
  } /* end NC_DOUBLE */
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
  } /* end NC_FLOAT */
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */

  if(var->has_mss_val) (void)cast_nctype_void(var->type,&(var->mss_val));
  return var;
} /* end ncap_var_scv_pwr */

scv_sct  
ncap_scv_clc(scv_sct scv_1, char op, scv_sct scv_2)
{
  /* Purpose: Calculate (scv_1 op scv_2) NB: Scalar values must be of same type */

  extern float fmodf(float,float); /* Cannot insert fmodf in ncap_sym_init() because it takes two arguments TODO #20 */
  extern float fabsf(float); /* Sun math.h does not include fabsf() prototype */

  scv_sct scv_out;
  scv_out.type=scv_1.type;
  switch(scv_out.type){ 
  case NC_BYTE:
    switch(op){
      case '+': scv_out.val.b=scv_1.val.b + scv_2.val.b;break;
      case '-': scv_out.val.b=scv_1.val.b - scv_2.val.b;break;
      case '/': scv_out.val.b=scv_1.val.b / scv_2.val.b;break;
      case '*': scv_out.val.b=scv_1.val.b * scv_2.val.b;break;
      case '%': scv_out.val.b=scv_1.val.b % scv_2.val.b;break;
    } break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_SHORT:
    switch(op){
      case '+': scv_out.val.s=scv_1.val.s + scv_2.val.s;break;
      case '-': scv_out.val.s=scv_1.val.s - scv_2.val.s;break;
      case '/': scv_out.val.s=scv_1.val.s / scv_2.val.s;break;
      case '*': scv_out.val.s=scv_1.val.s * scv_2.val.s;break;
      case '%': scv_out.val.s=scv_1.val.s % scv_2.val.s;break;
    } break;
  case NC_INT:
    switch(op){
      case '+': scv_out.val.l=scv_1.val.l + scv_2.val.l;break;
      case '-': scv_out.val.l=scv_1.val.l - scv_2.val.l;break;
      case '/': scv_out.val.l=scv_1.val.l / scv_2.val.l;break;
      case '*': scv_out.val.l=scv_1.val.l * scv_2.val.l;break;
      case '%': scv_out.val.l=scv_1.val.l % scv_2.val.l;break;
    } break;
  case NC_FLOAT:
    switch(op){
      case '+': scv_out.val.f=scv_1.val.f + scv_2.val.f;break;
      case '-': scv_out.val.f=scv_1.val.f - scv_2.val.f;break;
      case '/': scv_out.val.f=scv_1.val.f / scv_2.val.f;break;
      case '*': scv_out.val.f=scv_1.val.f * scv_2.val.f;break;
      case '%': scv_out.val.f=fmodf(scv_1.val.f,fabsf(scv_2.val.f));break;
    } break;
  case NC_DOUBLE:
    switch(op){
      case '+': scv_out.val.d=scv_1.val.d + scv_2.val.d;break;
      case '-': scv_out.val.d=scv_1.val.d - scv_2.val.d;break;
      case '/': scv_out.val.d=scv_1.val.d / scv_2.val.d;break;
      case '*': scv_out.val.d=scv_1.val.d * scv_2.val.d;break;
      case '%': scv_out.val.d=fmod(scv_1.val.d,fabs(scv_2.val.d));break;
    } break;
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */    
  
  return scv_out;
} /* end ncap_scv_clc_type */

scv_sct
ncap_scv_abs(scv_sct scv)
{
  /* Purpose: Find the absolute value of a scalar value */
  extern float fabsf(float); /* Sun math.h does not include fabsf() prototype */
  
  scv_sct scv_out;
  scv_out.type=scv.type;
  switch(scv.type){ 
  case NC_BYTE:
    scv_out.val.b=((scv.val.b >= 0) ? scv.val.b : -scv.val.b) ;
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_SHORT:
    scv_out.val.s=((scv.val.s >= 0) ? scv.val.s : -scv.val.s) ;
    break;
  case NC_INT:
    scv_out.val.l=labs(scv.val.l); /* int abs(int), long labs(long) */
    break;            
  case NC_FLOAT:
    scv_out.val.f=fabsf(scv.val.f);
    break;
  case NC_DOUBLE:
    scv_out.val.d=fabs(scv.val.d);
    break;
  default: nco_dfl_case_nc_type_err(); break;    
  } /* end switch */
  return scv_out;
} /* end ncap_scv_abs */

int 
ncap_scv_minus(scv_sct *scv)
{
  switch(scv->type){ 
  case NC_BYTE:
    scv->val.b= -scv->val.b; 
    break;
  case NC_CHAR:
    /* Do nothing */
    break;
  case NC_SHORT:
    scv->val.s=-scv->val.s;
    break;
  case NC_INT:
    scv->val.l=-scv->val.l;
    break;            
  case NC_FLOAT:
    scv->val.f=-scv->val.f;
    break;
  case NC_DOUBLE:
    scv->val.d=-scv->val.d;
    break;
  default: nco_dfl_case_nc_type_err(); break;   
  } /* end switch */    
  return scv->type;
} /* end ncap_scv_minus() */

nm_id_sct *
nco_var_lst_copy(nm_id_sct *xtr_lst,int lst_nbr)
{
  /* Purpose: Copy xtr_lst and return new list */
  int idx;
  nm_id_sct *xtr_new_lst;
  
  if(lst_nbr == 0) return NULL;
  xtr_new_lst=(nm_id_sct *)nco_malloc(lst_nbr*sizeof(nm_id_sct));
  for(idx=0;idx<lst_nbr;idx++){
    xtr_new_lst[idx].nm=(char *)strdup(xtr_lst[idx].nm);
    xtr_new_lst[idx].id=xtr_lst[idx].id;
  } /* end loop over variable */
  return xtr_new_lst;            
} /* end nco_var_lst_copy() */

nm_id_sct *
nco_var_lst_free(nm_id_sct *xtr_lst,int lst_nbr)
{
  /* Purpose: free xtr_lst and return null pointer */
  int idx;

  for(idx=0;idx<lst_nbr;idx++) nco_free(xtr_lst[idx].nm);
  nco_free(xtr_lst);

  return NULL;
} /* end nco_var_lst_free() */

nm_id_sct *
nco_var_lst_sub(nm_id_sct *xtr_lst,int *nbr_xtr,nm_id_sct *xtr_lst_b,int nbr_lst_b)
{
  /* Purpose: Subtract from xtr_lst any elements from xtr_lst_b which are present and return new list */
  int idx;
  int xtr_idx;
  int xtr_nbr_new=0;
  
  bool match;

  nm_id_sct *xtr_new_lst=NULL;
  
  if(*nbr_xtr == 0) return xtr_lst;
  
  xtr_new_lst=(nm_id_sct*)nco_malloc((size_t)(*nbr_xtr)*sizeof(nm_id_sct));  
  for(idx=0;idx<*nbr_xtr;idx++){
    match=False;
    for(xtr_idx=0;xtr_idx<nbr_lst_b;xtr_idx++)
      if(!strcmp(xtr_lst[idx].nm,xtr_lst_b[xtr_idx].nm)){match=True;break;}
    if(match) continue;
    xtr_new_lst[xtr_nbr_new].nm=(char *)strdup(xtr_lst[idx].nm);
    xtr_new_lst[xtr_nbr_new++].id=xtr_lst[idx].id;
  } /* end loop over idx */
  *nbr_xtr=xtr_nbr_new;
  return xtr_new_lst;      
}/* end nco_var_lst_sub */

nm_id_sct *
nco_var_lst_add(nm_id_sct *xtr_lst,int *nbr_xtr,nm_id_sct *xtr_lst_a,int nbr_lst_a)
{
  /* Purpose: Add to xtr_lst any elements from xtr_lst_a not already present and return new list */
  int idx;
  int xtr_idx;
  int nbr_xtr_crr;
  
  nm_id_sct *xtr_new_lst;
  
  bool match;
  
  nbr_xtr_crr=*nbr_xtr;
  if(nbr_xtr_crr > 0){
    xtr_new_lst=(nm_id_sct*)nco_malloc((size_t)(*nbr_xtr)*sizeof(nm_id_sct));
    for(idx=0;idx<nbr_xtr_crr;idx++){
      xtr_new_lst[idx].nm=(char *)strdup(xtr_lst[idx].nm);
      xtr_new_lst[idx].id=xtr_lst[idx].id;
    } /* end loop over variables */
  }else{
    *nbr_xtr=nbr_lst_a;
    return nco_var_lst_copy(xtr_lst_a,nbr_lst_a);
  }/* end if */
  
  for(idx=0;idx<nbr_lst_a;idx++){
    match=False;
    for(xtr_idx=0;xtr_idx<*nbr_xtr;xtr_idx++)
      if(!strcmp(xtr_lst[xtr_idx].nm,xtr_lst_a[idx].nm)){match=True;break;}
    if(match) continue;
    xtr_new_lst=(nm_id_sct *)nco_realloc(xtr_new_lst,(size_t)(nbr_xtr_crr+1)*sizeof(nm_id_sct));
    xtr_new_lst[nbr_xtr_crr].nm=(char *)strdup(xtr_lst_a[idx].nm);
    xtr_new_lst[nbr_xtr_crr++].id=xtr_lst_a[idx].id;
  } /* end for */
  *nbr_xtr=nbr_xtr_crr;
  return xtr_new_lst;            
} /* nco_var_lst_add */

nm_id_sct *
nco_var_lst_crd_make(int nc_id,nm_id_sct *xtr_lst,int *nbr_xtr)
/* 
   int nc_id: I netCDF file ID
   nm_id_sct *xtr_lst: I/O current extraction list 
   int *nbr_xtr: I/O number of variables in current extraction list: Overwritten by new list 
   nm_id_sct nco_var_lst_crd_make: list of coordinate dimensions 
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
	if(!strcmp(dmn_nm,xtr_lst[idx_var].nm)) {
	  if(nbr_new_lst == 0) new_lst=(nm_id_sct *)nco_malloc(sizeof(nm_id_sct));
	  else new_lst=(nm_id_sct *)nco_realloc((void *)new_lst,(size_t)(nbr_new_lst+1)*sizeof(nm_id_sct));
	  new_lst[nbr_new_lst].nm=(char *)strdup(dmn_nm);
	  new_lst[nbr_new_lst++].id=crd_id;
	  break;
        } /* end if */
      } /* end for */
    } /* end if */
  } /* end for */

  *nbr_xtr=nbr_new_lst;
  return new_lst;
} /* end nco_var_lst_crd_make() */


nm_id_sct * /* O [sct] List of dimensions associated with input variable list */
nco_dmn_lst /* [fnc] Create list of all dimensions in file  */
(const int nc_id, /* I [id] netCDF input-file ID */
 int * const nbr_dmn) /* O [nbr] Number of dimensions in  list */

{
  int idx;
  int nbr_dmn_in; 
  char dmn_nm[NC_MAX_NAME];
  nm_id_sct  *dmn;
 /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dmn_in,(int *)NULL,(int *)NULL,(int *)NULL);

  dmn=(nm_id_sct *)nco_malloc(nbr_dmn_in*sizeof(nm_id_sct));
  
 
  for(idx=0; idx< nbr_dmn_in;idx++)  {
    (void)nco_inq_dimname(nc_id,idx,dmn_nm);
    dmn[idx].id = idx;
    dmn[idx].nm = (char *)strdup(dmn_nm);
  }

  *nbr_dmn=nbr_dmn_in;
  return dmn;
}


nm_id_sct * nco_att_lst_mk(
const int in_id,
const int out_id,
aed_sct **att_lst,
int nbr_att,
int *nbr_lst )
{
  int idx;
  int jdx;
  int rcd;
  int var_id;
  int size=0;
  nm_id_sct *xtr_lst=NULL;   
  for( idx =0 ; idx <nbr_att ; idx++) {
    rcd=nco_inq_varid_flg(out_id,att_lst[idx]->var_nm,&var_id);
    if(rcd== NC_NOERR) continue;    
    rcd=nco_inq_varid_flg(in_id,att_lst[idx]->var_nm,&var_id);    
    if(rcd == NC_NOERR) {
      /* eliminate any duplicates from list */
      for(jdx =0 ; jdx <size ; jdx++)
	if(!strcmp(xtr_lst[jdx].nm,att_lst[idx]->var_nm)) break;
        if(jdx!=size) continue;
        xtr_lst = (nm_id_sct*) nco_realloc(xtr_lst,(size+1)*sizeof(nm_id_sct));
        xtr_lst[size].id = var_id;
        xtr_lst[size++].nm =(char *)strdup(att_lst[idx]->var_nm);
      
    }
  }

  *nbr_lst = size;

  return xtr_lst;

}


bool /* O [flg] Variables now conform */
ncap_var_stretch /* [fnc] Stretch variables */
(var_sct **var_1, /* I/O [ptr] First variable */
 var_sct **var_2) /* I/O [ptr] Second variable */
{
  /* Purpose: Make input variables conform or die
     var_1 and var_2 are considered completely symmetrically
     No assumption is made about var_1 relative to var_2
     Main difference betwee ncap_var_stretch() and nco_var_cnf_dmn() is
     If variables conform, then ncap_var_stretch() will broadcast
     If variables share no dimensions, then ncap_var_stretch() will convolve

     Terminology: 
     Broadcast: Inflate smaller conforming variable to larger variable
     Conform: Dimensions of one variable are subset of other variable
     Convolve: Construct array which is product of ranks of two variables
     Stretch: Union of broadcast and convolve

     Logic is pared down version of nco_var_cnf_dmn()
     1. USE_DUMMY_WGT has been eliminated: 
     ncap has no reason not to stretch input variables because grammar
     ensures only arithmetic variables will be stretched.
     
     2. wgt_crr has been eliminated:
     ncap never does anything multiple times so no equivalent to wgt_crr exists

     3. ncap_var_stretch(), unlike nco_var_cnf_dmn(), performs memory management
     Variables are var_free'd if they are superceded (replaced)

     4. Conformance logic is duplicated from nco_var_cnf_dmn()
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
  
  /* Determine which variable is greater and which lesser rank */
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
	  nco_exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: DEBUG %s and %s share no dimensions: Attempting to convolve...\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      }else if(var_lsr_var_gtr_dmn_shr_nbr > 0 && var_lsr_var_gtr_dmn_shr_nbr < var_lsr->nbr_dim){
	/* Some, but not all, of var_lsr dimensions are in var_gtr */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  (void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr);
	  nco_exit(EXIT_FAILURE);
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
      var_lsr_out=nco_var_dpl(var_lsr);
      (void)nco_xrf_var(var_lsr,var_lsr_out);
    } /* end if */
  } /* end if */

  if(var_lsr_out == NULL && CONVOLVE){
    /* Convolve variables by returned stretched variables with minimum possible number of dimensions */
    int dmn_nbr; /* Number of dimensions in convolution */
    if(dbg_lvl_get() >= 1) (void)fprintf(stdout,"\n%s: WARNING Convolution not yet implemented, results of operation between %s and %s are unpredictable\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
    /* Dimensions in convolution are union of dimensions in variables */
    dmn_nbr=var_lsr->nbr_dim+var_gtr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr; /* Number of dimensions in convolution */
    /* fxm: these should go away soon */
    var_lsr_out=nco_var_dpl(var_lsr);
    var_gtr_out=nco_var_dpl(var_gtr);
    for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){;}

    if(var_lsr_var_gtr_dmn_shr_nbr == 0) ; else ;

    /* Free calling variables */
    var_lsr=nco_var_free(var_lsr);
    var_gtr=nco_var_free(var_gtr);
  } /* endif STRETCH */

  if(var_lsr_out == NULL){
    /* Expand lesser variable (var_lsr) to match size of greater variable */
    char *var_lsr_cp;
    char *var_lsr_out_cp;

    int idx_var_lsr_var_gtr[NC_MAX_DIMS];
    int var_lsr_nbr_dim;
    int var_gtr_nbr_dmn_m1;

    long *var_gtr_cnt;
    long dmn_ss[NC_MAX_DIMS];
    long dmn_var_gtr_map[NC_MAX_DIMS];
    long dmn_var_lsr_map[NC_MAX_DIMS];
    long var_gtr_lmn;
    long var_lsr_lmn;
    long var_gtr_sz;

    size_t var_lsr_typ_sz;

    /* Copy main attributes of greater variable into lesser variable */
    var_lsr_out=nco_var_dpl(var_gtr);
    (void)nco_xrf_var(var_lsr,var_lsr_out);

    /* Modify a few elements of lesser variable array */
    var_lsr_out->nm=var_lsr->nm;
    var_lsr_out->id=var_lsr->id;
    var_lsr_out->type=var_lsr->type;
    var_lsr_out->val.vp=(void *)nco_malloc(var_lsr_out->sz*nco_typ_lng(var_lsr_out->type));
    var_lsr_cp=(char *)var_lsr->val.vp;
    var_lsr_out_cp=(char *)var_lsr_out->val.vp;
    var_lsr_typ_sz=nco_typ_lng(var_lsr_out->type);

    if(var_lsr_out->nbr_dim == 0){
      /* Variables are scalars, not arrays */

      (void)memcpy(var_lsr_out_cp,var_lsr_cp,var_lsr_typ_sz);

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
	    nco_exit(EXIT_FAILURE);
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
	
	(void)memcpy(var_lsr_out_cp+var_gtr_lmn*var_lsr_typ_sz,var_lsr_cp+var_lsr_lmn*var_lsr_typ_sz,var_lsr_typ_sz);
	
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
