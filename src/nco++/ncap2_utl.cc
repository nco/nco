/* $Header: /data/zender/nco_20150216/nco/src/nco++/ncap2_utl.cc,v 1.157 2014-02-17 23:12:39 zender Exp $ */

/* Purpose: netCDF arithmetic processor */

/* Copyright (C) 1995--2014 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include "ncap2_utl.hh"

// check if var is really an attribute
nco_bool 
ncap_var_is_att( var_sct *var) {
  if( strchr(var->nm,'@') !=NULL_CEWI ) 
    return True;
  return False;
}

// initialize var to defaults & undefined to true;
var_sct *
ncap_var_udf(const char *var_nm)
{ 
  var_sct *var_ret;
  
  var_ret=(var_sct *)nco_malloc(sizeof(var_sct));
  
  // Set defaults 
  (void)var_dfl_set(var_ret); 
  var_ret->nm=strdup(var_nm);
  var_ret->undefined=True;
  
  // Try out code. var_dfl_set sets type=NC_NAT 
  // This means its not possible to change the 
  // type as nco_var_cnf_typ bombs out
  // Temporary solution -- set the type to NC_INT 
  //var_ret->type=NC_INT;

  return var_ret;
}

var_sct*
ncap_att_get
(int var_id,
 const char *var_nm,
 const char *att_nm,
 int location, /* I [flg] 1 - att from INPUT file  2 - att from OUTPUT file */
 prs_cls *prs_arg)
{
  int rcd;
  int fl_id=int_CEWI;
  long sz;

  char *ln_nm;
  
  nc_type type;
  var_sct *var_ret;
  
  if(location == 1) fl_id=prs_arg->in_id;  
  if(location == 2) fl_id=prs_arg->out_id;  

  rcd=nco_inq_att_flg(fl_id,var_id,att_nm,&type,&sz);
  if(rcd == NC_ENOTATT) return NULL_CEWI;
  
  var_ret=(var_sct*)nco_malloc(sizeof(var_sct));
  (void)var_dfl_set(var_ret);
  
  // Make name of the form var_nm@att_nm
  ln_nm=(char *)nco_malloc((strlen(var_nm)+strlen(att_nm)+2)*sizeof(char));
  strcpy(ln_nm,var_nm);strcat(ln_nm,"@");strcat(ln_nm,att_nm);
  
  var_ret->nm=ln_nm;
  var_ret->id=var_id;
  var_ret->nc_id=prs_arg->in_id;
  var_ret->type=type;
  var_ret->sz=sz;
  // maybe needed ?
  var_ret->nbr_dim=0;
  
  // Fill with data if NOT an initial scan
  if(!prs_arg->ntl_scn){
    var_ret->val.vp=(void *)nco_malloc(sz*nco_typ_lng(type));
    rcd=nco_get_att(fl_id,var_id,att_nm,var_ret->val.vp,type);
    if (rcd != NC_NOERR) {
      var_ret=nco_var_free(var_ret);
      return NULL_CEWI;
    }
  }
  return var_ret; 
}

var_sct *                  /* O [sct] variable containing attribute */
ncap_att_init(             /*   [fnc] Grab an attribute from input file */
	      const std::string s_va_nm, /* I [sng] att name of form var_nm&att_nm */ 
	      prs_cls *prs_arg)          /* I/O vectors of atts & vars & file names  */
{
  int rcd;
  int var_id;
  
  std::string var_nm;
  std::string att_nm;
  size_t  att_char_posn;
  
  var_sct *var_ret;
  
  //check if we have an attribute
  if( (att_char_posn =s_va_nm.find("@")) ==std::string::npos )
    return NULL_CEWI; 
  
  var_nm=s_va_nm.substr(0,att_char_posn);
  att_nm=s_va_nm.substr(att_char_posn+1);
  
  if(var_nm == "global")
    var_id=NC_GLOBAL;
  else{
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm.c_str(),&var_id);
    if (rcd !=NC_NOERR) 
      return NULL_CEWI;
  }
  
  var_ret=ncap_att_get(var_id,var_nm.c_str(),att_nm.c_str(),1,prs_arg);
  
  return var_ret;
}

nco_bool     /* O [flg] true if var has been stretched */
ncap_att_stretch  /* stretch a single valued attribute from 1 to sz */
(var_sct* var,    /* I/O [sct] variable */       
 long nw_sz)      /* I [nbr] new var size */
{
  
  long  idx;
  long  var_typ_sz;  
  void* vp;
  char *cp;
  char **sng_cp;
    
  if(var->sz > 1L || nw_sz <1) return false; 

  if(nw_sz==1) return true;
    
  var_typ_sz=nco_typ_lng(var->type);


  // handle NC_STRING -special case 
  // nb var->val.sngp is a ragged array of chars    
  if(var->type == (nc_type)NC_STRING) {
    sng_cp=(char**)nco_malloc(nw_sz*var_typ_sz);    

    (void)cast_void_nctype((nc_type)NC_STRING,&var->val);

    for(idx=0 ; idx <nw_sz; idx++)
      sng_cp[idx]=strdup(var->val.sngp[0]);    

    nco_free(var->val.sngp[0]);  
    (void)cast_nctype_void((nc_type)NC_STRING,&var->val);
    
    vp=(void*)sng_cp;

  }else{
    vp=(void*)nco_malloc(nw_sz*var_typ_sz);    

    for(idx=0 ; idx < nw_sz ;idx++){
      cp=(char*)vp + (ptrdiff_t)(idx*var_typ_sz);
      memcpy(cp,var->val.vp ,var_typ_sz);
    }
  
  }

  
  var->val.vp=(void*)nco_free(var->val.vp);
  var->sz=nw_sz;
  var->val.vp=vp;
  return true;

  

  
} /* end ncap_att_stretch */

int
ncap_att_gnrl
(const std::string s_dst,
 const std::string s_src,
 int location,         /*   I [flg] 1 - att from INPUT file  2 - att from OUTPUT file */
 prs_cls  *prs_arg
 ){
  int idx;
  int srt_idx;
  int sz;
  int rcd;
  int var_id; 
  int nbr_att;
  int fl_id=int_CEWI;
  char att_nm[NC_MAX_NAME]; 
  const char *tmp_att_nm;
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  
  var_sct *var_att;
  
  std::string s_fll;
  
  NcapVar *Nvar;

  // De-reference 
  NcapVarVector &var_vtr=prs_arg->var_vtr;
  NcapVarVector att_vtr; //hold new attributes.
  
  if(location == 1 ) fl_id=prs_arg->in_id;  
  if(location == 2 ) fl_id=prs_arg->out_id;  

  // get var_id
  rcd=nco_inq_varid_flg(fl_id,s_src.c_str(),&var_id);
  
  if(rcd == NC_NOERR){
    (void)nco_inq_varnatts(fl_id,var_id,&nbr_att);
    // loop though attributes
    for(idx=0; idx <nbr_att ; idx++){
      (void)nco_inq_attname(fl_id,var_id,idx,att_nm);
      //skip missing value, scale_factor , add_offset
      if(!strcmp(att_nm,nco_mss_val_sng_get()) || !strcmp(att_nm,scl_fct_sng) || !strcmp(att_nm,add_fst_sng))
        continue;
      var_att=ncap_att_get(var_id,s_src.c_str(),att_nm,location,prs_arg);
      // now add to list( change the name!!)
      if(var_att){ 
	s_fll=s_dst+"@"+std::string(att_nm);
        Nvar=new NcapVar(var_att,s_fll ); 
	att_vtr.push_back(Nvar); 
      } 
    } // end for
  }// end rcd
  
  sz=var_vtr.size();
  if(s_dst != s_src && (srt_idx=var_vtr.find_lwr(s_src+"@"))!=-1) {  
    
    for(idx=srt_idx ;idx<sz; idx++) { 
      if (s_src!=var_vtr[idx]->getVar()) break;
      if( (var_vtr)[idx]->xpr_typ != ncap_att) continue;
        tmp_att_nm=var_vtr[idx]->getAtt().c_str();
        //skip missing value, scale_factor , add_offset
        if(!strcmp(tmp_att_nm,nco_mss_val_sng_get()) || !strcmp(tmp_att_nm,scl_fct_sng) || !strcmp(tmp_att_nm,add_fst_sng))
        continue;

        // Create string for new attribute
        s_fll= s_dst +"@"+(var_vtr[idx]->getAtt());
        var_att=nco_var_dpl(var_vtr[idx]->var);
        Nvar=new NcapVar(var_att,s_fll ); 
        att_vtr.push_back(Nvar);  
    } 
  }

  sz=att_vtr.size();
  // add new att to list;
  for(idx=0 ; idx < sz ; idx++){

#ifdef _OPENMP
    if( omp_in_parallel())
      prs_arg->thr_vtr.push_back(att_vtr[idx]);
    else
      var_vtr.push_ow(att_vtr[idx]);         
#else
      var_vtr.push_ow(att_vtr[idx]);         
#endif
  }
  return sz;
  
} /* end ncap_att_gnrl() */

int
ncap_att_cpy
(const std::string s_dst,
 const std::string s_src,
 prs_cls  *prs_arg)
{
  
  int nbr_att=0;
  //Don't propagate if s_src is a tree-parser generated var
  if(prs_arg->ATT_PROPAGATE && s_dst != s_src && s_src[0]!='~' )
    nbr_att=ncap_att_gnrl(s_dst,s_src,1,prs_arg);
  
  if(prs_arg->ATT_INHERIT)
    nbr_att=ncap_att_gnrl(s_dst,s_dst,1,prs_arg);
  
  return nbr_att;
}

void 
ncap_att_prn     /* [fnc] Print a single attribute*/
(var_sct *var,   /* I Variable containing att */
 char *const att_in_sng) /* user defined format string */
{
  char dlm_sng[3];
  char att_sng[NCO_MAX_LEN_FMT_SNG];

  long att_lmn;
  long att_sz;
  
    /* Copy value to avoid indirection in loop over att_sz */
    att_sz=var->sz;

    if(att_in_sng ==(char*)NULL) {        
      (void)fprintf(stdout,"%s, size = %li %s, value = ",var->nm,att_sz,nco_typ_sng(var->type));
    /* Typecast pointer to values before access */
      (void)cast_void_nctype(var->type,&var->val);
      (void)strcpy(dlm_sng,", ");
      (void)sprintf(att_sng,"%s%%s",nco_typ_fmt_sng(var->type));
    /* user defined format string */ 
     } else {
      (void)strcpy(att_sng,att_in_sng);
      (void)strcpy(dlm_sng,"");
    }
 
    switch(var->type){
    case NC_FLOAT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.fp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_DOUBLE:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.dp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_SHORT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.sp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_INT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,(long)var->val.ip[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_CHAR:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++){
        char char_foo;
	/* Assume \0 is string terminator and do not print it */
	if((char_foo=var->val.cp[att_lmn]) != '\0') (void)fprintf(stdout,"%c",char_foo);
      } /* end loop over element */
      break;
    case NC_BYTE:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.bp[att_lmn]);
      break;
    case NC_UBYTE:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.ubp[att_lmn]);
      break;
    case NC_USHORT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.usp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_UINT:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.uip[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_INT64:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.i64p[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_UINT64:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.ui64p[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    case NC_STRING:
      for(att_lmn=0;att_lmn<att_sz;att_lmn++) (void)fprintf(stdout,att_sng,var->val.sngp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");
      break;
    default: nco_dfl_case_nc_type_err();
      break;
    } /* end switch */
    (void)fprintf(stdout,"\n");
    
 (void)cast_nctype_void(var->type,&var->val);

  (void)fflush(stdout);

} /* end ncap_att_prn() */

var_sct * /* O [sct] Remainder of modulo operation of input variables (var1%var2) */
ncap_var_var_mod /* [fnc] Remainder (modulo) operation of two variables */
(var_sct *var1, /* I [sc,t] Variable structure containing field */
 var_sct *var2) /* I [sct] Variable structure containing divisor */
{
  ptr_unn op_swp;
  const char fnc_nm[]="ncap_var_var_mod"; 

  if(nco_dbg_lvl_get() >= 4) 
      dbg_prn(fnc_nm,"Entered function");
  

  if(var1->has_mss_val){
    (void)nco_var_mod(var1->type,var1->sz,var1->has_mss_val,var1->mss_val,var1->val,var2->val);
  }else{
    (void)nco_var_mod(var1->type,var1->sz,var2->has_mss_val,var2->mss_val,var1->val,var2->val);
    (void)nco_mss_val_cnf(var2,var1);
  } /* end else */
  
  // Swap values about
  op_swp=var1->val;var1->val=var2->val;var2->val=op_swp;
  
  var2=nco_var_free(var2);

  return var1;
} /* end ncap_var_var_mod() */

var_sct *           /* O [sct] Calculate atan2 for each element */
ncap_var_var_atan2  
(var_sct *var1,     /* I [sc,t] Variable structure containing field  */
 var_sct *var2)     /* I [sct] Variable structure containing divisor */
{
  long idx;
  long sz;
  bool has_mss_val=false;
  double mss_val_dbl;
  ptr_unn op1,op2;

  const char fnc_nm[]="ncap_var_var_atan2"; 
  if(nco_dbg_lvl_get() >= 4) dbg_prn(fnc_nm,"Entered function");

  sz=var1->sz;

  //Dereference
  op1=var1->val;
  op2=var2->val; 
  
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(NC_DOUBLE,&op1);
  (void)cast_void_nctype(NC_DOUBLE,&op2);

  if(var1->has_mss_val){
     has_mss_val=true; 
     (void)cast_void_nctype(NC_DOUBLE,&var1->mss_val);
     mss_val_dbl=*var1->mss_val.dp;
  } else if(var2->has_mss_val){
     has_mss_val=true; 
     (void)cast_void_nctype(NC_DOUBLE,&var2->mss_val);
     mss_val_dbl=*var2->mss_val.dp;
  }

 if(!has_mss_val){
    for(idx=0;idx<sz;idx++) op1.dp[idx]=atan2(op1.dp[idx],op2.dp[idx]);
 }else{
    for(idx=0;idx<sz;idx++){
    if((op1.dp[idx] != mss_val_dbl) && (op2.dp[idx] != mss_val_dbl)) op1.dp[idx]=atan2(op1.dp[idx],op2.dp[idx]); else op2.dp[idx]=mss_val_dbl;
      } /* end for */
  
 } /* end else */

 // cast misssing value back to void
 if(var1->has_mss_val)
   (void)cast_nctype_void(NC_DOUBLE,&var1->mss_val);  
 else if(var2->has_mss_val)
   (void)cast_nctype_void(NC_DOUBLE,&var2->mss_val);  

 var2=nco_var_free(var2); 

  return var1;
} /* end ncap_var_var_atan2 */


var_sct *        /* O [sct] Resultant variable (actually is var) */
ncap_var_abs /* Purpose: Find absolute value of each element of var */
(var_sct *var)    /* I/O [sct] input variable */
{
  
  if(var->undefined) return var;
  
  /* deal with initial scan */
  if(var->val.vp==NULL) return var; 
  
  (void)nco_var_abs(var->type,var->sz,var->has_mss_val,var->mss_val,var->val);
  return var;
} /* end ncap_var_abs */

var_sct * /* O [sct] Empowerment of input variables (var1^var_2) */
ncap_var_var_pwr_old /* [fnc] Empowerment of two variables */ 
(var_sct *var1, /* I [sct] Variable structure containing base */
 var_sct *var2) /* I [sct] Variable structure containing exponent */
{
  char *swp_nm;
  
  /* Purpose: Empower two variables (var1^var2) */
  
  /* Temporary fix */ 
  /* Swap names about so attribute propagation works */
  /* most operations unlike this one put results in left operand */
  if( !ncap_var_is_att(var1) && isalpha(var1->nm[0])) {
    swp_nm=var1->nm; var1->nm=var2->nm; var2->nm=swp_nm;
  }  
  
  if(var1->undefined){ 
    var2->undefined=True;
    var1=nco_var_free(var1);
    return var2;
  }
  
  /* Make sure variables are at least float */
  if(nco_rth_prc_rnk(var1->type) < nco_rth_prc_rnk_float && 
     nco_rth_prc_rnk(var2->type) < nco_rth_prc_rnk_float )
    var1=nco_var_cnf_typ((nc_type)NC_FLOAT,var1);
  
  (void)ncap_var_retype(var1,var2);   
  
  /* Handle initial scan */
  if(var1->val.vp==(void*)NULL ) {
    if(var1->nbr_dim > var2->nbr_dim) {
      var2=nco_var_free(var2);
      return var1;
    }else{
      var1=nco_var_free(var1);
      return var2;
    }
  } 
  
  (void)ncap_var_cnf_dmn(&var1,&var2);
  if(var1->has_mss_val){
    (void)nco_var_pwr(var1->type,var1->sz,var1->has_mss_val,var1->mss_val,var1->val,var2->val);
  }else{
    (void)nco_var_pwr(var1->type,var1->sz,var2->has_mss_val,var2->mss_val,var1->val,var2->val);
  } /* end else */
  
  var1=nco_var_free(var1);
  return var2;
} /* end ncap_var_var_pwr() */

var_sct * /* O [sct] Empowerment of input variables (var1^var_2) */
ncap_var_var_pwr  /* [fnc] Empowerment of two variables */ 
(var_sct *var1,   /* I [sct] Variable structure containing base */
 var_sct *var2)   /* I [sct] Variable structure containing exponent */
{
  
  ptr_unn op_swp;

  const char fnc_nm[]="ncap_var_var_pwr";
   
  if(nco_dbg_lvl_get() >= 4) 
      dbg_prn(fnc_nm,"Entered function");


  if(var1->has_mss_val){
    (void)nco_var_pwr(var1->type,var1->sz,var1->has_mss_val,var1->mss_val,var1->val,var2->val);
  }else{
    (void)nco_var_pwr(var1->type,var1->sz,var2->has_mss_val,var2->mss_val,var1->val,var2->val);
    (void)nco_mss_val_cnf(var2,var1);
    
  } /* end else */
  
  // Swap values about 
  op_swp=var1->val;var1->val=var2->val;var2->val=op_swp;
  
  var2=nco_var_free(var2);

  return var1;
  
} /* end ncap_var_var_pwr() */

var_sct *           /* O [sct] Resultant variable (actually is var_in) */
ncap_var_fnc
(var_sct *var_in,    
 double(*fnc_dbl)(double),
 float(*fnc_flt)(float))
{
  /* Purpose: Evaluate fnc_dbl(var) or fnc_flt(var) for each value in variable
     Float and double functions are in app */
  const char fnc_nm[]="ncap_var_fnc"; 

  long idx;
  long sz;
  ptr_unn op1;


  if(nco_dbg_lvl_get() >= 4) 
      dbg_prn(fnc_nm,"Entered function");
  
  if(var_in->undefined) return var_in;
  
  /* Promote variable to NC_FLOAT */
  if(nco_rth_prc_rnk(var_in->type) < nco_rth_prc_rnk_float) var_in=nco_var_cnf_typ(NC_FLOAT,var_in);
  
  /* Deal with initial scan */
  if(var_in->val.vp==NULL) return var_in; 
  
  op1=var_in->val;
  sz=var_in->sz;
  (void)cast_void_nctype(var_in->type,&op1);
  if(var_in->has_mss_val) (void)cast_void_nctype(var_in->type,&(var_in->mss_val));
  
  switch(var_in->type){ 
  case NC_DOUBLE: {
    if(!var_in->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=(*fnc_dbl)(op1.dp[idx]);
    }else{
      double mss_val_dbl=*(var_in->mss_val.dp); /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=(*fnc_dbl)(op1.dp[idx]);
      } /* end for */
    } /* end else */
    break;
  }
  case NC_FLOAT: {
    if(!var_in->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=(*fnc_flt)(op1.fp[idx]);
    }else{
      float mss_val_flt=*(var_in->mss_val.fp); /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=(*fnc_flt)(op1.fp[idx]);
      } /* end for */
    } /* end else */
    break;
  }
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */
  
  if(var_in->has_mss_val) (void)cast_nctype_void(var_in->type,&(var_in->mss_val));


  return var_in;
} /* end ncap_var_fnc() */

void 
ncap_lmt_evl
(int nc_id,
 lmt_sct* lmt_ptr,
 prs_cls *prs_arg){

long cnt_dmn;
long srt;
long end;
long cnt;
long srd;
dmn_sct *dmn_ptr;

//Dereference
NcapVector<dmn_sct*> dmn_vtr;

const char fnc_nm[]="ncap_lmt_evl"; 

dmn_vtr=(nc_id==prs_arg->in_id ? prs_arg->dmn_in_vtr: prs_arg->dmn_out_vtr);

   

  dmn_ptr=dmn_vtr.find(lmt_ptr->nm);

  if(dmn_ptr==NULL)
    err_prn(fnc_nm,"Dimension "+ std::string(lmt_ptr->nm)+" in limits not found");

  cnt_dmn=dmn_ptr->sz;

  //fill out defaults
  srt=( lmt_ptr->is_usr_spc_min ? lmt_ptr->srt:0L);
  end=( lmt_ptr->is_usr_spc_max ? lmt_ptr->end:cnt_dmn-1);
  srd=( lmt_ptr->srd_sng!=NULL_CEWI  ? lmt_ptr->srd:1L);  
 
  // do error checking 
  if(prs_arg->FORTRAN_IDX_CNV){ 
    std::ostringstream os;
  
    if(lmt_ptr->is_usr_spc_min && (srt<1 || srt>cnt_dmn)) {

      os<<"Lower limit " <<srt<<" for dim "<<lmt_ptr->nm<<" is outside range "<<1L<<"-"<<cnt_dmn;
      err_prn(fnc_nm,os.str());
    }

    if(lmt_ptr->is_usr_spc_max && (end<1 || end>cnt_dmn)) {
      os<<"Upper limit " <<end<<" for dim "<<lmt_ptr->nm<<" is outside range "<<1L<<"-"<<cnt_dmn;
      err_prn(fnc_nm,os.str());
    }
   
    if(srt>end){	 
      os<<"Lower limit " <<srt<<" for dim "<<lmt_ptr->nm<<" is greater than upper limit "<<end;  
      err_prn(fnc_nm,os.str());  
    }	 	

    if(srd<1){	 
      os<<"Sride " <<srd<<" for dim "<<lmt_ptr->nm<<" is less than 1"<<end;  
      err_prn(fnc_nm,os.str());  
    }	 	
      

    if(lmt_ptr->is_usr_spc_min) srt--;
    if(lmt_ptr->is_usr_spc_max) end--;
  }

  // do error checking 
  if(!prs_arg->FORTRAN_IDX_CNV){ 
    std::ostringstream os;
    if(srt<0) srt+=cnt_dmn-1;       
    if(end<0) end+=cnt_dmn-1;       
    if(lmt_ptr->is_usr_spc_min && (srt<0 || srt>cnt_dmn-1)) {
           
      os<<"Lower limit " <<srt<<" for dim "<<lmt_ptr->nm<<" is outside range "<<0L<<"-"<<cnt_dmn-1;
      err_prn(fnc_nm,os.str());
    }

    if(lmt_ptr->is_usr_spc_max && (end<0 || end>cnt_dmn-1)) {
      os<<"Upper limit " <<end<<" for dim "<<lmt_ptr->nm<<" is outside range "<<0L<<"-"<<cnt_dmn-1;
      err_prn(fnc_nm,os.str());
    }
   
    if(srt>end){	 
      os<<"Lower limit " <<srt<<" for dim "<<lmt_ptr->nm<<" is greater than upper limit "<<end;  
      err_prn(fnc_nm,os.str());  
    }	 	

    if(srd<1){	 
      os<<"Sride " <<srd<<" for dim "<<lmt_ptr->nm<<" is less than 1"<<end;  
      err_prn(fnc_nm,os.str());  
    }	 	
  }

  cnt=1+(end-srt)/srd;

  lmt_ptr->srt=srt;
  lmt_ptr->end=end;
  lmt_ptr->cnt=cnt;
  lmt_ptr->srd=srd;

  return;
} /* end ncap_lmt_evl() */

nm_id_sct *            /* O [sct] new copy of xtr_lst */
nco_var_lst_copy      /* [fnc] Purpose: Copy xtr_lst and return new list */
(nm_id_sct *xtr_lst,    /* I  [sct] input list */ 
 int lst_nbr)           /* I  [nbr] Number of elements in list */
{
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

nm_id_sct *             /* O [sct] New list */
nco_var_lst_sub
(nm_id_sct *xtr_lst,     /* I [sct] input list */   
 int *xtr_nbr,           /* I/O [ptr] size of xtr_lst and new list */
 nm_id_sct *xtr_lst_b,   /* I [sct] list to be subtracted */   
 int nbr_lst_b)          /* I [nbr] size eof xtr_lst_b */ 
{
  /* Purpose: Subtract from xtr_lst any elements from xtr_lst_b which are present and return new list */
  int idx;
  int xtr_idx;
  int xtr_nbr_new=0;
  
  nco_bool match;
  
  nm_id_sct *xtr_new_lst=NULL;
  
  if(*xtr_nbr == 0) return xtr_lst;
  
  xtr_new_lst=(nm_id_sct*)nco_malloc((size_t)(*xtr_nbr)*sizeof(nm_id_sct)); 
  for(idx=0;idx<*xtr_nbr;idx++){
    match=False;
    for(xtr_idx=0;xtr_idx<nbr_lst_b;xtr_idx++)
      if(!strcmp(xtr_lst[idx].nm,xtr_lst_b[xtr_idx].nm)){match=True;break;}
    if(match) continue;
    xtr_new_lst[xtr_nbr_new].nm=(char *)strdup(xtr_lst[idx].nm);
    xtr_new_lst[xtr_nbr_new].id=xtr_lst[idx].id;


    /* Increment */
    xtr_nbr_new++;


  } /* end loop over idx */
  /* realloc to actual size */
  xtr_new_lst=(nm_id_sct*)nco_realloc(xtr_new_lst,xtr_nbr_new*sizeof(nm_id_sct)); 
  /* free old list */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,*xtr_nbr);
  
  *xtr_nbr=xtr_nbr_new;
  return xtr_new_lst;     
}/* end nco_var_lst_sub */

nm_id_sct *            /* O [sct] -- new list */
nco_var_lst_add
(nm_id_sct *xtr_lst,    /* I [sct] input list */ 
 int *xtr_nbr,          /* I/O [ptr] -- size of xtr_lst & new output list */ 
 nm_id_sct *xtr_lst_a,  /* I [sct] list of elemenst to be added to new list */
 int nbr_lst_a)         /* I [nbr] size of xtr_lst_a */
{
  /* Purpose: Add to xtr_lst any elements from xtr_lst_a not already present and return new list */
  int idx;
  int xtr_idx;
  int xtr_nbr_crr;
  
  nm_id_sct *xtr_new_lst;
  
  nco_bool match;
  
  xtr_nbr_crr=*xtr_nbr;
  if(xtr_nbr_crr > 0){
    xtr_new_lst=(nm_id_sct*)nco_malloc((size_t)(*xtr_nbr)*sizeof(nm_id_sct));
    for(idx=0;idx<xtr_nbr_crr;idx++){
      xtr_new_lst[idx].nm=(char *)strdup(xtr_lst[idx].nm);
      xtr_new_lst[idx].id=xtr_lst[idx].id;


    } /* end loop over variables */
  }else{
    *xtr_nbr=nbr_lst_a;
    return nco_var_lst_copy(xtr_lst_a,nbr_lst_a);
  }/* end if */
  
  for(idx=0;idx<nbr_lst_a;idx++){
    match=False;
    for(xtr_idx=0;xtr_idx<*xtr_nbr;xtr_idx++)
      if(!strcmp(xtr_lst[xtr_idx].nm,xtr_lst_a[idx].nm)){match=True;break;}
    if(match) continue;
    xtr_new_lst=(nm_id_sct *)nco_realloc(xtr_new_lst,(size_t)(xtr_nbr_crr+1)*sizeof(nm_id_sct));
    xtr_new_lst[xtr_nbr_crr].nm=(char *)strdup(xtr_lst_a[idx].nm);
    xtr_new_lst[xtr_nbr_crr].id=xtr_lst_a[idx].id;


    xtr_nbr_crr++;

  } /* end for */
  *xtr_nbr=xtr_nbr_crr;
  return xtr_new_lst;           
} /* end nco_var_lst_add */

nm_id_sct * /* O [sct] List of dimensions associated with input variable list */
nco_dmn_lst /* [fnc] Create list of all dimensions in file  */
(const int nc_id, /* I [id] netCDF input-file ID */
 int * const nbr_dmn) /* O [nbr] Number of dimensions in  list */
{
  int idx;
  int nbr_dmn_in;
  char dmn_nm[NC_MAX_NAME];
  nm_id_sct *dmn;
  /* Get number of dimensions */
  (void)nco_inq(nc_id,&nbr_dmn_in,(int *)NULL,(int *)NULL,(int *)NULL);
  
  dmn=(nm_id_sct *)nco_malloc(nbr_dmn_in*sizeof(nm_id_sct));
  
  for(idx=0;idx<nbr_dmn_in;idx++){
    (void)nco_inq_dimname(nc_id,idx,dmn_nm);
    dmn[idx].id=idx;
    dmn[idx].nm=(char *)strdup(dmn_nm);

    

  } /* end loop over dmn */
  
  *nbr_dmn=nbr_dmn_in;
  return dmn;
} /* end nco_dmn_lst() */

nm_id_sct *                /* O [sct] output list */ 
nco_att_lst_mk      
(const int in_id,         /* I [id] of input file */
 const int out_id,        /* I [id] id of output file */
 NcapVarVector &var_vtr,  /* I [vec] vector of vars & att */
 int *nbr_lst)            /* O [ptr] size of output list */
{
  int idx;
  int jdx;
  int rcd;
  int var_id;
  int size=0;
  char var_nm[NC_MAX_NAME];
  nm_id_sct *xtr_lst=NULL_CEWI;  
  for(idx=0;idx<var_vtr.size();idx++){
    // Check for attribute
    if( var_vtr[idx]->xpr_typ !=ncap_att) continue;
    (void)strcpy(var_nm, var_vtr[idx]->getVar().c_str());

    rcd=nco_inq_varid_flg(out_id,var_nm,&var_id);
    if(rcd== NC_NOERR) continue;   
    rcd=nco_inq_varid_flg(in_id,var_nm,&var_id);   
    if(rcd == NC_NOERR){
      /* eliminate any duplicates from list */
      for(jdx=0;jdx<size;jdx++)
        if(!strcmp(xtr_lst[jdx].nm,var_nm)) break;
      if(jdx!=size) continue;
      /* fxm mmr TODO 491: memory leak xtr_lst */
      xtr_lst=(nm_id_sct *)nco_realloc(xtr_lst,(size+1)*sizeof(nm_id_sct));
      xtr_lst[size].id=var_id;
      xtr_lst[size].nm=(char *)strdup(var_nm); 

      /* Increment */
      size++;

    } /* end if */
  } /* end loop over att */

  *nbr_lst=size;

  return xtr_lst;
} /* end nco_att_lst_mk() */

nco_bool /* O [flg] Variables now conform */
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
     
     Terminology--- 
     Broadcast: Inflate smaller conforming variable to larger variable
     Conform: Dimensions of one variable are subset of other variable
     Convolve: Construct array whose rank is sum of non-duplicated dimensions
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
     var_gtr_out=var_gtr unless convolution is required */
 
   const char fnc_nm[]="ncap_var_stretch"; 
  
  nco_bool CONFORMABLE=False; /* [flg] Whether var_lsr can be made to conform to var_gtr */
  nco_bool CONVOLVE=False; /* [flg] var_1 and var_2 had to be convolved */
  nco_bool DO_CONFORM; /* [flg] Did var_1 and var_2 conform? */
  nco_bool MUST_CONFORM=False; /* [flg] Must var_1 and var_2 conform? */
  
  int idx;
  int idx_dmn;
  int var_lsr_var_gtr_dmn_shr_nbr=0; /* [nbr] Number of dimensions shared by var_lsr and var_gtr */
  
  var_sct *var_gtr=NULL_CEWI; /* [ptr] Pointer to variable structure of greater rank */
  var_sct *var_lsr=NULL_CEWI; /* [ptr] Pointer to variable structure to lesser rank */
  var_sct *var_gtr_out=NULL_CEWI; /* [ptr] Pointer to stretched version of greater rank variable */
  var_sct *var_lsr_out=NULL_CEWI; /* [ptr] Pointer to stretched version of lesser rank variable */
  
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
  if(var_lsr_out == NULL_CEWI){
    if(var_gtr->nbr_dim > 0){
      /* Test that all dimensions in var_lsr appear in var_gtr */
      for(idx=0;idx<var_lsr->nbr_dim;idx++){
        for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(!strcmp(var_lsr->dim[idx]->nm,var_gtr->dim[idx_dmn]->nm)){
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
          err_prn(fnc_nm,std::string(var_lsr->nm)+ " and " +std::string(var_gtr->nm) +" share no dimensions. "); 
	  //(void)fprintf(stdout,"%s: ERROR %s and template %s share no dimensions\n",nco_prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  //nco_exit(EXIT_FAILURE);
	}else{
	  if(nco_dbg_lvl_get() >= 1) 
	    wrn_prn(fnc_nm,std::string(var_lsr->nm)+ " and " +std::string(var_gtr->nm) +" share no dimensions. Attempting to convolve..."); 
	  //(void)fprintf(stdout,"\n%s: DEBUG %s and %s share no dimensions: Attempting to convolve...\n",nco_prg_nm_get(),var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      }else if(var_lsr_var_gtr_dmn_shr_nbr > 0 && var_lsr_var_gtr_dmn_shr_nbr < var_lsr->nbr_dim){
	/* Some, but not all, of var_lsr dimensions are in var_gtr */
	CONFORMABLE=False;
	if(MUST_CONFORM){
	  std::ostringstream os;
          os<<var_lsr_var_gtr_dmn_shr_nbr<< " dimensions of " << var_lsr->nm << " belong to template ";
          os<<var_gtr->nm <<" but " << (var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr) <<" do not.";
          err_prn(fnc_nm,os.str());
	  
	  //(void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",nco_prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr);
	  //nco_exit(EXIT_FAILURE);
	}else{
	  if(nco_dbg_lvl_get() >= 1){ 
	    std::ostringstream os;
	    os<<var_lsr_var_gtr_dmn_shr_nbr << " dimensions of " << var_lsr->nm << " belong to template ";
	    os<<var_gtr->nm << " but " << (var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr) <<" do not:";
	    os<<"Not broadcasting " << var_lsr->nm<< "to " <<var_gtr->nm;
	    os<<"could attempt stretching???";
	    wrn_prn(fnc_nm,os.str());
          }
	  
	  //(void)fprintf(stdout,"\n%s: DEBUG %d dimensions of %s belong to template %s but %d dimensions do not: Not broadcasting %s to %s, could attempt stretching???\n",nco_prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm);
	  CONVOLVE=True;
	} /* endif */
      } /* end if */
      if(CONFORMABLE){
	if(var_gtr->nbr_dim == var_lsr->nbr_dim){
	  /* var_gtr and var_lsr conform and are same rank */
	  /* Test whether all var_lsr and var_gtr dimensions match in sequence */
	  for(idx=0;idx<var_gtr->nbr_dim;idx++){
	    if(strcmp(var_lsr->dim[idx]->nm,var_gtr->dim[idx]->nm)) break;
	  } /* end loop over dimensions */
	  /* If so, take shortcut and copy var_lsr to var_lsr_out */
	  if(idx == var_gtr->nbr_dim) DO_CONFORM=True;
	}else{
	  /* var_gtr and var_lsr conform but are not same rank, set flag to proceed to generic conform routine */
	  DO_CONFORM=False;
	} /* end else */
      } /* endif CONFORMABLE */
    }else{ /* nbr_dmn == 0 */
      /* var_gtr is scalar, if var_lsr is also then set flag to copy var_lsr to var_lsr_out else proceed to generic conform routine */
      if(var_lsr->nbr_dim == 0) DO_CONFORM=True; else DO_CONFORM=False;
    } /* end else nbr_dmn == 0 */
    if(CONFORMABLE && DO_CONFORM){
      var_lsr_out=nco_var_dpl(var_lsr);
      (void)nco_xrf_var(var_lsr,var_lsr_out);
    } /* end if */
  } /* endif var_lsr_out == NULL */
  
  if(var_lsr_out == NULL_CEWI && CONVOLVE){
    /* Convolve variables by returned stretched variables with minimum possible number of dimensions */
    int dmn_nbr; /* Number of dimensions in convolution */
    if(nco_dbg_lvl_get() >= 1) 
      dbg_prn(fnc_nm,"Convolution not yet implemented, results of operation between " +std::string(var_lsr->nm) 
              + " and "+std::string(var_gtr->nm) + "are unpredictable");
    //(void)fprintf(stdout,"\n%s: WARNING Convolution not yet implemented, results of operation between %s and %s are unpredictable\n",nco_prg_nm_get(),var_lsr->nm,var_gtr->nm);
    /* Dimensions in convolution are union of dimensions in variables */
    dmn_nbr=var_lsr->nbr_dim+var_gtr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr; /* Number of dimensions in convolution */
    dmn_nbr=dmn_nbr; /* CEWI: Avert compiler warning that variable is set but never used */
    /* fxm: these should go away soon */
    var_lsr_out=nco_var_dpl(var_lsr);
    var_gtr_out=nco_var_dpl(var_gtr);
    
    /* for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){;}
       if(var_lsr_var_gtr_dmn_shr_nbr == 0); else; */
    
    /* Free calling variables */
    var_lsr=nco_var_free(var_lsr);
    var_gtr=nco_var_free(var_gtr);
  } /* endif STRETCH */
  
  if(var_lsr_out == NULL_CEWI){
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
    
    /* Modify elements of lesser variable array */
    var_lsr_out->nm=(char *)nco_free(var_lsr_out->nm);
    var_lsr_out->nm=(char *)strdup(var_lsr->nm);
    var_lsr_out->id=var_lsr->id;
    var_lsr_out->type=var_lsr->type;
    /* Added 20050323: 
       Not quite sure why, but var->val.vp may already have values here when LHS-casting
       Perform safety free to guard against memory leaks */
    var_lsr_out->val.vp=nco_free(var_lsr_out->val.vp);
    var_lsr_out->val.vp=(void *)nco_malloc_dbg(var_lsr_out->sz*nco_typ_lng(var_lsr_out->type),"Unable to malloc() value buffer in variable stretching",fnc_nm);
    var_lsr_cp=(char *)var_lsr->val.vp;
    var_lsr_out_cp=(char *)var_lsr_out->val.vp;
    var_lsr_typ_sz=nco_typ_lng(var_lsr_out->type);
    
    if(var_lsr_out->nbr_dim == 0){
      /* Variables are scalars, not arrays */
      (void)memcpy(var_lsr_out_cp,var_lsr_cp,var_lsr_typ_sz);
    }else if(var_lsr->nbr_dim == 0){
      /* Lesser-ranked input variable is scalar 
	 Expansion in this degenerate case needs no index juggling (reverse-mapping)
	 Code as special case to speed-up important applications of ncap
	 for synthetic file creation */
      var_gtr_sz=var_gtr->sz;
      for(var_gtr_lmn=0;var_gtr_lmn<var_gtr_sz;var_gtr_lmn++){
	(void)memcpy(var_lsr_out_cp+var_gtr_lmn*var_lsr_typ_sz,var_lsr_cp,var_lsr_typ_sz);
      } /* end loop over var_gtr_lmn */
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
      request imap vector to specify offset (imap) vector in bytes. */
      for(idx=0;idx<var_lsr->nbr_dim;idx++){
	for(idx_dmn=0;idx_dmn<var_gtr->nbr_dim;idx_dmn++){
	  /* Compare names, not dimension IDs */
	  if(!strcmp(var_gtr->dim[idx_dmn]->nm,var_lsr->dim[idx]->nm)){
	    idx_var_lsr_var_gtr[idx]=idx_dmn;
	    /*	    idx_var_gtr_var_lsr[idx_dmn]=idx;*/
	    break;
	  } /* end if */
	  /* Sanity check */
	  if(idx_dmn == var_gtr->nbr_dim-1){
            err_prn(fnc_nm,"var_lsr " + std::string(var_lsr->nm)+ " has dimension "+ std::string(var_lsr->dim[idx]->nm)
                    + " but var_gtr " + std::string(var_gtr->nm)+ " does not deep in ncap_var_stretch.");
	    
	    //(void)fprintf(stdout,"%s: ERROR var_lsr %s has dimension %s but var_gtr %s does not deep in ncap_var_stretch()\n",nco_prg_nm_get(),var_lsr->nm,var_lsr->dim[idx]->nm,var_gtr->nm);
	    //nco_exit(EXIT_FAILURE);
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

nco_bool
ncap_def_dim(
	     std::string dmn_nm,
	     long sz,
	     prs_cls *prs_arg){
  const char fnc_nm[]="ncap_def_dim"; 
  
  dmn_sct *dmn_nw;             
  dmn_sct *dmn_in_e;
  dmn_sct *dmn_out_e;

  // Check if dimension already exists
  dmn_in_e=prs_arg->dmn_in_vtr.find(dmn_nm);
  dmn_out_e=prs_arg->dmn_out_vtr.find(dmn_nm);
  
  if(dmn_in_e !=NULL_CEWI || dmn_out_e !=NULL_CEWI  ){ 
    wrn_prn(fnc_nm,"dim \""+ dmn_nm + "\" - already exists in input/output."); 
    return False;
  }
  
  if( sz < 0  ){
    std::ostringstream os;
    os<<"dim " << dmn_nm << "(size=" <<sz <<") dimension can't be negative.";
    wrn_prn(fnc_nm,os.str()); 
    return False;
  }
  
  dmn_nw=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
  
  dmn_nw->nm=(char *)strdup(dmn_nm.c_str());
  //dmn_nw->id=dmn_id;
  dmn_nw->nc_id=prs_arg->out_id;
  dmn_nw->xrf=NULL_CEWI;
  dmn_nw->val.vp=NULL_CEWI;
  dmn_nw->is_crd_dmn=False;
  dmn_nw->is_rec_dmn=False;
  dmn_nw->sz=sz;
  dmn_nw->cnt=sz;
  dmn_nw->srt=0L;
  dmn_nw->end=sz-1;
  dmn_nw->srd=1L;
  
  // finally define dimension in output
  (void)nco_redef(prs_arg->out_id);
  (void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,&dmn_nw,1);          
  (void)nco_enddef(prs_arg->out_id);  
  
  // Add dim to list 
  (void)prs_arg->dmn_out_vtr.push_back(dmn_nw); 
  return True; 
}

nco_bool         /* Returns True if shape of vars match (using cnt vectors) */
nco_shp_chk(
	    var_sct* var1, 
	    var_sct* var2)
{
  
  long idx;
  long nbr_rdmn1;
  long nbr_rdmn2;
  long srt1=0;
  long srt2=0;
  
  long nbr_cmp;
  
  /* Check sizes */
  if( var1->sz != var2->sz )
    return False;
  
  
  
  nbr_rdmn1=var1->nbr_dim;  
  nbr_rdmn2=var2->nbr_dim;  
  
  /* skip leading 1D dims */
  for(idx=0 ; idx < (nbr_rdmn1-1) ; idx++)
    if( var1->cnt[idx] == 1){
      srt1++;continue;
    } else break;
  
  /* skip leading 1D dims */
  for(idx=0 ; idx < (nbr_rdmn2-1) ; idx++)
    if( var2->cnt[idx] == 1){
      srt2++;continue;
    } else break;
  
  
  /* check sizes again */
  if( nbr_rdmn1-srt1 != nbr_rdmn2-srt2 )
    return False;
  
  nbr_cmp=nbr_rdmn1-srt1;
  /* Now compare  values */
  for(idx=0 ; idx < nbr_cmp ;idx++)
    if( var1->cnt[srt1++] != var2->cnt[srt2++]) break;
  
  if( idx==nbr_cmp) 
    return True;
  else
    return False;
}



/* This file is generated in makefile from ncoParserTokenTypes.hpp */ 
#include "ncoEnumTokenTypes.hpp"

var_sct *
ncap_var_var_stc
(var_sct *var1,
 var_sct *var2,
 int op)
{
  static VarOp<double> Vd;
  static VarOp<float> Vf;
  static VarOp<nco_int> Vi;
  static VarOp<nco_short> Vs;
  static VarOp<nco_ushort> Vus;
  static VarOp<nco_uint> Vui;
  static VarOp<nco_int64> Vi64;
  static VarOp<nco_uint64> Vui64;
  static VarOp<nco_byte> Vb;
  static VarOp<nco_ubyte> Vub;
  static VarOp<nco_char> Vc;
  var_sct *var_ret=NULL_CEWI;
  
  //If var2 is null then we are dealing with a unary function
  if( var2 == NULL_CEWI) {
    
    switch (var1->type) {
    case NC_DOUBLE: var_ret=Vd.var_op(var1,op); break;
    case NC_FLOAT: var_ret=Vf.var_op(var1,op); break;
    case NC_INT: var_ret=Vi.var_op(var1,op); break;            
    case NC_SHORT: var_ret=Vs.var_op(var1,op); break;
    case NC_USHORT: var_ret=Vus.var_op(var1,op); break;
    case NC_UINT: var_ret=Vui.var_op(var1,op); break;
    case NC_INT64: var_ret=Vi64.var_op(var1,op); break;
    case NC_UINT64: var_ret=Vui64.var_op(var1,op); break;
    case NC_BYTE: var_ret=Vb.var_op(var1,op); break;
    case NC_UBYTE: var_ret=Vub.var_op(var1,op); break;
    case NC_CHAR:  var_ret=Vs.var_op(var1,op); break;
    case NC_STRING:  break; /* Do nothing */
    default: nco_dfl_case_nc_type_err(); break;
    }
    return var_ret;
  }
  
  switch (var1->type) {
  case NC_DOUBLE: var_ret=Vd.var_var_op(var1,var2,op); break;
  case NC_FLOAT: var_ret=Vf.var_var_op(var1,var2,op); break;
  case NC_INT:var_ret=Vi.var_var_op(var1,var2,op); break;            
  case NC_SHORT: var_ret=Vs.var_var_op(var1,var2,op); break;
  case NC_USHORT: var_ret=Vus.var_var_op(var1,var2,op); break;
  case NC_UINT: var_ret=Vui.var_var_op(var1,var2,op); break;
  case NC_INT64: var_ret=Vi64.var_var_op(var1,var2,op); break;
  case NC_UINT64: var_ret=Vui64.var_var_op(var1,var2,op); break;
  case NC_BYTE: var_ret=Vb.var_var_op(var1,var2,op); break;
  case NC_UBYTE: var_ret=Vub.var_var_op(var1,var2,op); break;
  case NC_CHAR: break; /* Do nothing */
  case NC_STRING: break; /* Do nothing */
  default: nco_dfl_case_nc_type_err(); break;
  } 
  
  return var_ret;
}



int                /* [flg] true they conform */         
ncap_var_att_cnf   /* [fnc] Make vars/atts conform */
(var_sct *&var1,   /* I [sct] Input variable structure  */
 var_sct *&var2    /* I [sct] Input variable structure  */
)
{ 
  const char fnc_nm[]="ncap_var_att_cnf"; 
  
  nco_bool vb1;
  nco_bool vb2;
  
  vb1 = ncap_var_is_att(var1);
  vb2 = ncap_var_is_att(var2);
  
  // var & var
  if( !vb1 && !vb2 ) { 
    char *swp_nm;
    // (void)ncap_var_retype(var1,var2);
    
    // if hyperslabs then check they conform
    if( (var1->has_dpl_dmn ==-1 || var2->has_dpl_dmn==-1) && var1->sz >1 && var2->sz>1){  
      if(var1->sz != var2->sz) {
	std::ostringstream os;
	os<<"Hyperslabbed variable:"<<var1->nm <<" and variable:"<<var2->nm <<" have different number of elements, so cannot perform arithmetic operation.";
	err_prn(fnc_nm,os.str());
      }
      if( nco_shp_chk(var1,var2)==False){ 
	std::ostringstream os;
        os<<"Hyperslabbed variable:"<<var1->nm <<" and variable:"<<var2->nm <<" have same  number of elements, but different shapes.";
	wrn_prn(fnc_nm,os.str());
      }
    }else{   
      (void)ncap_var_cnf_dmn(&var1,&var2);
    }
    // Bare numbers have name prefixed with"_"
    // for attribute propagation to work we need
    // to swap names about if first operand is a bare number
    // and second operand is a var
    if( (var1->nm[0]=='~') && (var2->nm[0]!='~') ){
      swp_nm=var1->nm; var1->nm=var2->nm; var2->nm=swp_nm; 
    }  
    // var & att
  }else  if( !vb1 && vb2 ){ 
    // var2=nco_var_cnf_typ(var1->type,var2);
    if(var1->sz > 1 && var2->sz==1)
      (void)ncap_var_cnf_dmn(&var1,&var2);
    
    if(var1->sz != var2->sz) {
      std::ostringstream os;
      os<<"Cannot make variable:"<<var1->nm <<" and attribute:"<<var2->nm <<" conform. So cannot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
    
    // att & var
  }else if( vb1 && !vb2){
    var_sct *var_tmp;
    
    if(var2->sz > 1 && var1->sz==1) 
       (void)ncap_att_stretch(var1,var2->sz);
    
    if(var1->sz != var2->sz){
      std::ostringstream os;
      os<<"Cannot make attribute:"<<var1->nm <<" and variable:"<<var2->nm <<" conform. So cannot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
    // turn att var1 into a variable;
    var_tmp=nco_var_dpl(var2);
    var_tmp->val.vp=nco_free(var_tmp->val.vp);
    var_tmp=nco_var_cnf_typ(var1->type,var_tmp);  
    var_tmp->val.vp=var1->val.vp;
    
    var1->val.vp=(void*)NULL;
    nco_var_free(var1);
    var1=var_tmp;   
     
    
    // att && att
  } else if (vb1 && vb2) {
    (void)ncap_var_retype(var1,var2);
    
    if( var1->sz ==1 && var2->sz >1) 
      (void)ncap_att_stretch(var1,var2->sz);
    else if(var1->sz >1 && var2->sz==1)
      (void)ncap_att_stretch(var2,var1->sz);
    
    // Crash out if atts not equal size
    if(var1->sz != var2->sz) {
      std::ostringstream os;
      os<<"Cannot make attribute:"<<var1->nm <<" and attribute:"<<var2->nm <<" conform. So cannot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
  }
  return True;

} /* end ncap_var_att_cnf */


var_sct *             /* O [sct] Result if ntl_scn true otherwize null */ 
ncap_var_att_arr_cnf( /* I [fnc] Make all of vars in array conform to each other */
bool ntl_scn,         /* [flg] reurb dim with correct shape as ig op had happened */
var_sct ***var_arr,   /* I/O [sct] Array of variables */
int sz)               /* size of array */
{
  bool undef=false;
  int idx;
  int max_idx=0;
  int nbr_max_dim=0;

  for(idx=0 ; idx<sz ; idx++){
    if( (*var_arr[idx])->nbr_dim >nbr_max_dim ){
      nbr_max_dim=(*var_arr[idx])->nbr_dim;      
      max_idx=idx;   
    }
    if((*var_arr[idx])->undefined )
     undef=true; 
  }

  if(ntl_scn) {
    if(undef) 
      max_idx=-1;
    // delete var_sct's
    for(idx=0; idx<sz ; idx++)   
      if(idx != max_idx)
        *var_arr[idx]=nco_var_free(*var_arr[idx]);   
     
    if(undef) 
     return ncap_var_udf("~gsl_cls");
    else
      return *var_arr[max_idx];

  } // end ntl_scn 		       

  for(idx=0 ; idx<sz ;idx++){
    if(idx==max_idx) continue;
    //  printf("Conforming var=%s making var conform=%s\n",(*var_arr[max_idx])->nm,(*var_arr[idx])->nm);
    (void)ncap_var_att_cnf( *var_arr[max_idx],*var_arr[idx]);
  }  
  return NULL; // fxm correct? csz 20090226
} /* end ncap_var_att_arr_cnf */

var_sct *    
ncap_var_att_cnf_ntl   /*   [fnc] determine resultant struct */
(var_sct *var1,        /* I [sct] Input variable structure containing first operand */
 var_sct *var2)         /* I [sct] Input variable structure containing second operand */
{ 
  nco_bool vb1;
  nco_bool vb2;
  
  
  vb1 = ncap_var_is_att(var1);
  vb2 = ncap_var_is_att(var2);
  
  // var & var
  if( !vb1 && !vb2 ) { 
    if(var1->undefined ||var2->undefined){
      var1->undefined=True;
      var2=nco_var_free(var2);
      return var1;
    }
    
    // Do variable conformance with empty variables
    if(var1->nbr_dim > var2->nbr_dim) {
      var2=nco_var_free(var2);
      return var1;
    }else{
      var1=nco_var_free(var1);
      return var2;
    }
    
  }
  // var & att
  else if( !vb1 && vb2 ){ 
    var2=nco_var_free(var2);    
    return var1;
  }   
  // att & var
  else if( vb1 && !vb2){
    var1=nco_var_free(var1);
    return var2;  
  }
  // att && att
  else if (vb1 && vb2) {
    if(var1->sz >= var2->sz) {
      var2=nco_var_free(var2);
      return var1;
    } else {
      var1=nco_var_free(var1);
      return var2;
    }
  }
  /* Should not reach end of this function */
  nco_exit(EXIT_FAILURE);
  return var1;

} /* ncap_var_att_cnf_ntl */

var_sct *         /* O [sct] Sum of input variables (var1+var2) */
ncap_var_var_op   /* [fnc] Add two variables */
(var_sct *var1,  /* I [sct] Input variable structure containing first operand */
 var_sct *var2,  /* I [sct] Input variable structure containing second operand */
 int op)        /* Operation +-% */
{ 
  const char fnc_nm[]="ncap_var_var_op"; 
  
  nco_bool vb1;
  nco_bool vb2;
  
  var_sct *var_ret=NULL_CEWI;
  
  // If initial scan than call up "shadow" function 
  if(var1->val.vp == (void*)NULL  ){
    var_ret=ncap_var_var_op_ntl(var1,var2,op);
    return var_ret;
  }
  
  // If var2 is null then we are dealing with a unary function
  if( var2 == NULL_CEWI){ 
    var_ret=ncap_var_var_stc(var1,var2,op);   
    return var_ret;
  }
  
  // Deal with pwr_in fuction
  if( (op == CARET ) && nco_rth_prc_rnk(var1->type) < nco_rth_prc_rnk_float &&  nco_rth_prc_rnk(var2->type) < nco_rth_prc_rnk_float) var1=nco_var_cnf_typ((nc_type)NC_FLOAT,var1);
  
  //Deal with atan2 function
  if(op==ATAN2 ){
    var1=nco_var_cnf_typ((nc_type)NC_DOUBLE,var1);
    var2=nco_var_cnf_typ((nc_type)NC_DOUBLE,var2);
  }  
  
  vb1 = ncap_var_is_att(var1);
  vb2 = ncap_var_is_att(var2);
  
  // var & var
  if( !vb1 && !vb2 ) { 
    char *swp_nm;
    (void)ncap_var_retype(var1,var2);
    
    // if hyperslabs then check they conform
    if( (var1->has_dpl_dmn ==-1 || var2->has_dpl_dmn==-1) && var1->sz >1 && var2->sz>1){  
      if(var1->sz != var2->sz) {
	std::ostringstream os;
	os<<"Hyperslabbed variable:"<<var1->nm <<" and variable:"<<var2->nm <<" have differnet number of elements, so cannot perform arithmetic operation.";
	err_prn(fnc_nm,os.str());
      }
      if( nco_shp_chk(var1,var2)==False){ 
	std::ostringstream os;
        os<<"Hyperslabbed variable:"<<var1->nm <<" and variable:"<<var2->nm <<" have same  number of elements, but different shapes.";
	wrn_prn(fnc_nm,os.str());
      }
    }else{   
      (void)ncap_var_cnf_dmn(&var1,&var2);
    }
    // Bare numbers have name prefixed with"_"
    // for attribute propagation to work we need
    // to swap names about if first operand is a bare number
    // and second operand is a var
    if( (var1->nm[0]=='~') && (var2->nm[0]!='~') ){
      swp_nm=var1->nm; var1->nm=var2->nm; var2->nm=swp_nm; 
    }  
    // var & att
  }else  if( !vb1 && vb2 ){ 
    var2=nco_var_cnf_typ(var1->type,var2);
    if(var1->sz > 1 && var2->sz==1)
      (void)ncap_var_cnf_dmn(&var1,&var2);
    
    if(var1->sz != var2->sz) {
      std::ostringstream os;
      os<<"Cannot make variable:"<<var1->nm <<" and attribute:"<<var2->nm <<" conform. So cannot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
    
    // att & var
  }else if( vb1 && !vb2){
    var_sct *var_swp;
    ptr_unn val_swp;  // Used to swap values around
    
    var1=nco_var_cnf_typ(var2->type,var1);
    if(var2->sz > 1 && var1->sz==1) (void)ncap_var_cnf_dmn(&var1,&var2);
    
    if(var1->sz != var2->sz){
      std::ostringstream os;
      os<<"Cannot make attribute:"<<var1->nm <<" and variable:"<<var2->nm <<" conform. So cannot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
    // Swap values around in var1 and var2;   
    val_swp=var1->val;
    var1->val=var2->val;
    var2->val=val_swp;;
    // Swap names about 
    var_swp=var1;
    var1=var2;
    var2=var_swp;
    
    // att && att
  } else if (vb1 && vb2) {
    (void)ncap_var_retype(var1,var2);
    
    if( var1->sz ==1 && var2->sz >1) 
      (void)ncap_att_stretch(var1,var2->sz);
    else if(var1->sz >1 && var2->sz==1)
      (void)ncap_att_stretch(var2,var1->sz);
    
    // Crash out if atts not equal size
    if(var1->sz != var2->sz) {
      std::ostringstream os;
      os<<"Cannot make attribute:"<<var1->nm <<" and attribute:"<<var2->nm <<" conform. So cannot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
  }
  
  // Deal with pwr fuction ( nb pwr function can't be templated )
  if(op== CARET){
    var_ret=ncap_var_var_pwr(var1,var2);
    return var_ret;     
  }
 
 // deal with mod function
  if(op==MOD){
    var_ret=ncap_var_var_mod(var1,var2);
    return var_ret;     
  }

  if(op==ATAN2){
    var_ret=ncap_var_var_atan2(var1,var2);
    return var_ret;
  }
  
  var_ret=ncap_var_var_stc(var1,var2,op);
  var2=nco_var_free(var2);
  return var_ret;
}



var_sct *             /* O [sct] Sum of input variables (var1+var2) INITIAL SCAN ONLY */
ncap_var_var_op_ntl   /* [fnc] Add two variables */
(var_sct *var1,       /* I [sct] Input variable structure containing first operand */
 var_sct *var2,       /* I [sct] Input variable structure containing second operand */
 int op)              /* Operation +-% */
{ 
  nco_bool vb1;
  nco_bool vb2;
  
  // If var2 is null then we are dealing with a unary function
  if(var2 == NULL_CEWI)
    return var1;
  
  // deal with pwr fuction
  if( (op == CARET ) && nco_rth_prc_rnk(var1->type) < nco_rth_prc_rnk_float && nco_rth_prc_rnk(var2->type) < nco_rth_prc_rnk_float) var1=nco_var_cnf_typ((nc_type)NC_FLOAT,var1);

  //Deal with atan2 function
  if(op==ATAN2 ){
    var1=nco_var_cnf_typ((nc_type)NC_DOUBLE,var1);
    var2=nco_var_cnf_typ((nc_type)NC_DOUBLE,var2);
  }  

  
  vb1 = ncap_var_is_att(var1);
  vb2 = ncap_var_is_att(var2);
  
  // var & var
  if( !vb1 && !vb2 ) { 
    if(var1->undefined ||var2->undefined){
      var1->undefined=True;
      var2=nco_var_free(var2);
      return var1;
    }
    
    (void)ncap_var_retype(var1,var2);
    // Do variable conformance with empty variables
    if(var1->nbr_dim > var2->nbr_dim) {
      var2=nco_var_free(var2);
      return var1;
    }else{
      var1=nco_var_free(var1);
      return var2;
    }
    
  }
  // var & att
  else if( !vb1 && vb2 ){ 
    var2=nco_var_free(var2);    
    return var1;
  }   
  // att & var
  else if( vb1 && !vb2){
    var1=nco_var_free(var1);
    return var2;  
  }
  // att && att
  else if (vb1 && vb2) {
    (void)ncap_var_retype(var1,var2);
    
    if(var1->sz >= var2->sz) {
      var2=nco_var_free(var2);
      return var1;
    } else {
      var1=nco_var_free(var1);
      return var2;
    }
  }
  /* Should not reach end of this function */
  nco_exit(EXIT_FAILURE);
  return var1;
}

var_sct *          /* O [sct] Sum of input variables (var1+var2) */
ncap_var_var_inc   /* [fnc] Add two variables */
(var_sct *var1,    /* I [sct] Input variable structure containing first operand */
 var_sct *var2,    /* I [sct] Input variable structure containing second operand */
 int op,            /* Deal with incremental operators i.e +=,-=,*=,/= */
 bool bram,         /* I [bool] If true make a RAM variable */ 
 prs_cls *prs_arg)
{
  const char fnc_nm[]="ncap_var_var_inc"; 
  const char *cvar1;
  const char *cvar2;
  nco_bool vb1;
  nco_bool vb2;
  
  var_sct *var_ret=NULL_CEWI;
  
  vb1 = ncap_var_is_att(var1);
  
  // If initial Scan
  if(prs_arg->ntl_scn){
    
    // deal with variable
    if(!vb1){
      var_ret=nco_var_dpl(var1); 
      (void)prs_arg->ncap_var_write(var1,bram);  
      // deal with attribute 
    }else{
      var_ret=var1;
    }
    if(var2) 
      var2=(var_sct*)nco_var_free(var2);
    
    return var_ret;
  }   
  
  //Deal with unary functions first
  if(var2==NULL_CEWI){
    if(op==INC||op==DEC){ 
      var1=ncap_var_var_stc(var1,var2,op);
      var_ret=nco_var_dpl(var1);
    }
    if(op==POST_INC||op==POST_DEC){ 
      var_ret=nco_var_dpl(var1);
      var1=ncap_var_var_stc(var1,var2,op);
    }
    if(!vb1){
      (void)prs_arg->ncap_var_write(var1,bram);  
    }else{
      std::string sa(var1->nm);
      NcapVar *Nvar=new NcapVar(var1,sa);
      prs_arg->var_vtr.push_ow(Nvar);       
    }
    
    return var_ret;    
  }
  
  vb2 = ncap_var_is_att(var2);
  
  cvar1= vb1? "attribute" : "variable";
  cvar2= vb2? "attribute" : "variable";
  
  // var2 to type of var1
  var2=nco_var_cnf_typ(var1->type,var2);
  
  // var & var
  if(!vb1 && !vb2) {
    nco_bool DO_CONFORM=True;;
    
    var_sct *var_tmp=NULL_CEWI;
    
    var_tmp=nco_var_cnf_dmn(var1,var2,var_tmp,True,&DO_CONFORM);
    if(var2 != var_tmp){
      var2=nco_var_free(var2);
      var2=var_tmp;
    }
    
    if(DO_CONFORM==False) {
      std::ostringstream os;
      os<<"Cannot make variable:"<<var1->nm <<" and variable:"<<var2->nm <<" conform. So cannot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
    // att & var ,att & att  
  } else {
    
    if(var1->sz > 1 && var2->sz==1)
      (void)ncap_att_stretch(var2,var1->sz);
  }   
  
  if(var1->sz != var2->sz) {
    std::ostringstream os;
    os<<"Cannot make " << cvar1<<":"<<var1->nm <<" and " <<cvar2 <<":"<<var2->nm <<" conform. So cannot perform arithmetic operation.";
    err_prn(fnc_nm,os.str()); 
  }
  
  var1=ncap_var_var_stc(var1,var2,op);
  var_ret=nco_var_dpl(var1);
  
  // if LHS is a variable then write to disk
  if(!vb1){
    prs_arg->ncap_var_write(var1,bram);
  }else{
    // deal with attribute
    std::string sa(var1->nm);
    NcapVar *Nvar=new NcapVar(var1,sa);
    prs_arg->var_vtr.push_ow(Nvar);       
    
  }
  var2=nco_var_free(var2);
  return var_ret;
}


bool            /* O [flg] true if all var elemenst are true */
ncap_var_lgcl   /* [fnc] calculate a aggregate bool value from a variable */
(var_sct* var)  /* I [sct] input variable */
{
  int idx;
  int sz;
  nc_type type;
  bool bret=true;
  ptr_unn op1;
  
  // Convert to type SHORT
  var=nco_var_cnf_typ((nc_type)NC_SHORT,var);  
  
  type=NC_SHORT;
  sz = var->sz;
  op1=var->val;
  /* Typecast pointer to values before access */
  (void)cast_void_nctype(type,&op1);
  if(var->has_mss_val) (void)cast_void_nctype(type,&var->mss_val);
  
  if(!var->has_mss_val){
    for(idx=0;idx<sz;idx++) 
      if(!op1.sp[idx]) break;
  }else{
    const short mss_val_short=*(var->mss_val.sp);
    for(idx=0;idx<sz;idx++) 
      if(!op1.sp[idx] && op1.sp[idx] != mss_val_short) break; 
  }
  if(idx < sz) bret=false;
  
  if(var->has_mss_val) (void)cast_nctype_void(type,&var->mss_val);
  
  return bret;
}

var_sct* /* O [sct] casting variable has its own private dims */ 
ncap_cst_mk( /* [fnc] create casting var from a list of dims */
	    std::vector<std::string> &str_vtr,  /* I [sng] list of dimension subscripts */
	    prs_cls *prs_arg)
{
  const char fnc_nm[]="ncap_cst_mk"; 
  static const char * const tpl_nm="Internally generated template";
  
  bool bdef=false;  //  
  int dmn_nbr; /* [nbr] Number of dimensions */
  int idx; /* [idx] Counter */
  
  const char *lst_nm;    /* for dereferencing */  
  double val=1.0; /* [frc] Value of template */
  
  var_sct *var=NULL_CEWI; /* [sct] Variable */
  
  dmn_sct **dmn; /* [dmn] Dimension structure list */
  dmn_sct *dmn_item;
  dmn_sct *dmn_new;
  
  dmn_nbr = str_vtr.size();
  
  //  sbs_lst=nco_lst_prs_2D(sbs_sng,sbs_dlm,&dmn_nbr); 
  
  dmn=(dmn_sct **)nco_malloc(dmn_nbr*sizeof(dmn_sct *));
  for(idx=0;idx<dmn_nbr;idx++){
    lst_nm=str_vtr[idx].c_str();
    // Search dmn_out_vtr for dimension
    dmn_item=prs_arg->dmn_out_vtr.find(lst_nm);
    if(dmn_item){ 
      dmn[idx]=dmn_item;
      continue;
    }
    // Search dmn_in_vtr for dimension
    dmn_item=prs_arg->dmn_in_vtr.find(lst_nm);
    // die if not in list
    if(dmn_item == NULL_CEWI) {
      err_prn(fnc_nm,"Unrecognized dimension \""+std::string(lst_nm)+ "\"in LHS subscripts");
    }
    if(!bdef) { 

#ifdef _OPENMP
       if( omp_in_parallel())
	 err_prn(fnc_nm,"Attempt to go into netCDF define mode while in OpenMP parallel mode");
      
#endif

       bdef=true;  
       (void)nco_redef(prs_arg->out_id);
    }

    dmn_new=nco_dmn_dpl(dmn_item);
    // Define in output file 
    (void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,&dmn_new,1);
    // add to out list
    (void)prs_arg->dmn_out_vtr.push_back(dmn_new);
    (void)nco_dmn_xrf(dmn_new,dmn_item);
    dmn[idx]=dmn_new;
  }
  if(bdef)
    (void)nco_enddef(prs_arg->out_id);
  
  /* Check that un-limited dimension is first dimension */
  for(idx=1;idx<dmn_nbr;idx++)
    if(dmn[idx]->is_rec_dmn){
      wrn_prn(fnc_nm,"\""+std::string(dmn[idx]->nm)+"\" is the record dimension. It must be the first dimension in a list");
      goto end_LHS_sbs;                     
    } /* endif */
  
  /* Create template variable to cast all RHS expressions */
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  
  /* Set defaults */
  (void)var_dfl_set(var); /* [fnc] Set defaults for each member of variable structure */
  /* Overwrite with LHS information */
  /* fxm mmr: memory leak with var->nm */
  var->nm=(char *)strdup(tpl_nm);
  var->nm_fll=NULL;
  var->type=NC_DOUBLE;
  var->nbr_dim=dmn_nbr;
  var->dim=dmn;
  /* Allocate space for dimension structures */
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;
  if(var->nbr_dim > 0) var->cnk_sz=(size_t *)nco_malloc(var->nbr_dim*sizeof(size_t)); else var->cnk_sz=(size_t *)NULL;
  
  /* Defensive programming */
  var->sz=1L; 
  /* Attach LHS dimensions to variable */
  for(idx=0;idx<dmn_nbr;idx++){
    var->cnk_sz[idx]=(size_t)0;   
    var->dim[idx]=dmn[idx];
    var->dmn_id[idx]=dmn[idx]->id;
    var->cnt[idx]=dmn[idx]->cnt;
    var->srt[idx]=dmn[idx]->srt;
    var->end[idx]=dmn[idx]->end;
    var->srd[idx]=dmn[idx]->srd;
    var->sz*=var->cnt[idx];

  } /* end loop over dim */
  

  /* NOTE VERY IMPORTANT 27-01-2009 */
  /* var->val.vp is never used in an initial or final scan 
     So we can safely set it to null here and get an immediate 
     performance boost */
        
     var->val.vp=(void*)NULL;
     goto end_var;   
   

  /* Do not initialize val in initial scan  */
  if(prs_arg->ntl_scn) {
    var->val.vp=(void*)NULL;
    goto end_var;
  }
  
  /* Allocate space for variable values 
     fxm: more efficient and safer to use nco_calloc() and not fill with values? */
  var->val.vp=(void *)nco_malloc_flg(var->sz*nco_typ_lng(var->type));
  
  /* Copy arbitrary value into variable 
     Placing a uniform value in variable should be unnecessary since variable
     is intended for use solely as dimensional template for nco_var_cnf_dmn() 
     Nevertheless, copy 1.0 into value for safety */
  { /* Change scope to define convenience variables which reduce de-referencing */
    long var_sz; /* [nbr] Number of elements in variable */
    size_t var_typ_sz; /* [B] Size of single element of variable */
    char *var_val_cp; /* [ptr] Pointer to values */
    
    var_sz=var->sz; /* [nbr] Number of elements in variable */
    var_typ_sz=nco_typ_lng(var->type);
    var_val_cp=(char *)var->val.vp;
    for(idx=0;idx<var_sz;idx++) (void)memcpy(var_val_cp+idx,(void *)(&val),var_typ_sz);
  } /* end scope */
 end_var:
  
  if(nco_dbg_lvl_get() >= nco_dbg_scl) {
    std::ostringstream os;
    os<<"creating LHS cast template var->nm " <<var->nm <<" var->nbr_dim " <<var->nbr_dim <<" var->sz " <<var->sz; 
    wrn_prn(fnc_nm,os.str());
  }
  /* Free dimension list memory */
  
 end_LHS_sbs: /* Errors encountered during LHS processing jump to here */
  /* Return to default state, known as INITIAL state or 0 state LMB92 p. 43 */    
  
  return var;
} // end ncap_cst_mk


var_sct*
ncap_cst_do(
	    var_sct* var,
	    var_sct* var_cst,
	    bool bntlscn)
{
  const char fnc_nm[]="ncap_cst_do"; 
  
  var_sct* var_tmp;
  
  if(bntlscn) {
    var_tmp=nco_var_dpl(var_cst);
    var_tmp->id=var->id;
    var_tmp->nm=(char*)nco_free(var_tmp->nm);
    var_tmp->nm=strdup(var->nm);
    var_tmp->type=var->type;
    var_tmp->typ_dsk=var->typ_dsk;
    var_tmp->undefined=False;
    var_tmp->val.vp=(void*)NULL;
    var=nco_var_free(var);
    var=var_tmp;
    
  }else{
    
    /* User intends LHS to cast RHS to same dimensionality
       Stretch newly initialized variable to size of LHS template */
    var_tmp=var;
    (void)ncap_var_stretch(&var_tmp,&var_cst);
    if(var_tmp != var) { 
      var=nco_var_free(var); 
      var=var_tmp;
    }
    
    if(nco_dbg_lvl_get() >= nco_dbg_scl){
      std::ostringstream os;
      os<<"Stretching variable "<<var->nm << "with LHS template var->nm "<<var_cst->nm <<"var->nbr_dim " <<var_cst->nbr_dim; 
      os<<" var->sz " <<var_cst->sz;
      wrn_prn(fnc_nm,os.str());
    }
    
    var->undefined=False;
  }
  
  return var;
}

// Return true if string is only made of 0..9 chars
bool
str_is_num(
	   std::string snm)
{ 
  unsigned idx;
  for(idx=0 ; idx < snm.size(); idx++)
    if(!isdigit(snm[idx])) break;
  
  return (idx==snm.size() ? true: false);
}

NcapVector<dmn_sct*>                /* O    [sct] list of new dims to limit over */ 
ncap_dmn_mtd(
	     var_sct *var,                      /* I   [sct] var with list of dims */
	     std::vector<std::string> &str_vtr)  /* I   [sng] list of dimension names */
{
  int idx;
  int jdx;
  int icnt;
  int nbr_dim=var->nbr_dim;
  int str_vtr_sz;
  
  NcapVector<dmn_sct*> dmn_vtr;
  
  str_vtr_sz=str_vtr.size();
  for(idx=0 ; idx <str_vtr_sz ; idx++){
    // deal with position args e.g., $0,$1,$2 etc
    if( str_is_num(str_vtr[idx])) {
      icnt=atoi(str_vtr[idx].c_str());
      if(icnt < nbr_dim && !dmn_vtr.find(var->dim[icnt]->nm)) 
	dmn_vtr.push_back( var->dim[icnt]);
    }else{
      // deal with regular dim names
      for(jdx=0 ; jdx < nbr_dim ; jdx++)
	if( str_vtr[idx]== std::string(var->dim[jdx]->nm))
	  break;
      if(jdx <nbr_dim && !dmn_vtr.find(str_vtr[idx])) 
	if(jdx <nbr_dim) 
	  dmn_vtr.push_back(var->dim[jdx]);
    }
    
  } // end loop
  return dmn_vtr;  
  
}

var_sct *
ncap_sclr_var_mk
(const std::string var_nm,
 nc_type type,
 bool bfll)
{
  /* Purpose: Create a scalar variable of type, if bfll then malloc ptr_unn */
  var_sct *var;
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  /* Set defaults */
  (void)var_dfl_set(var); 
  /* Overwrite with variable expression information */
  var->nm=strdup(var_nm.c_str());
  var->nbr_dim=0;
  var->sz=1;
  var->type=type;
  var->typ_dsk=type;
  
  if(bfll) var->val=nco_mss_val_mk(type);  
  
  return var;
} // end ncap_sclr_var_mk<string,int,bool>()

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 float val_float)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_FLOAT,true);
  (void)cast_void_nctype((nc_type)NC_FLOAT,&var->val);
  *var->val.fp=val_float;
  (void)cast_nctype_void((nc_type)NC_FLOAT,&var->val);
  return var;
} // end ncap_sclr_var_mk<float>()

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 double val_double)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_DOUBLE,true);
  (void)cast_void_nctype((nc_type)NC_DOUBLE,&var->val);
  *var->val.dp=val_double;
  (void)cast_nctype_void((nc_type)NC_DOUBLE,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_int val_int)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_INT,true);
  (void)cast_void_nctype((nc_type)NC_INT,&var->val);
  *var->val.ip=val_int;

  (void)cast_nctype_void((nc_type)NC_INT,&var->val);
  return var;
}
 
var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_short val_short)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_SHORT,true);
  (void)cast_void_nctype((nc_type)NC_SHORT,&var->val);
  *var->val.sp=val_short;
  (void)cast_nctype_void((nc_type)NC_SHORT,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_char val_char)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_CHAR,true);
  (void)cast_void_nctype((nc_type)NC_CHAR,&var->val);
  *var->val.cp=val_char;
  (void)cast_nctype_void((nc_type)NC_CHAR,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_byte val_byte)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_BYTE,true);
  (void)cast_void_nctype((nc_type)NC_BYTE,&var->val);
  *var->val.bp=val_byte;
  (void)cast_nctype_void((nc_type)NC_BYTE,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_ubyte val_ubyte)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_UBYTE,true);
  (void)cast_void_nctype((nc_type)NC_UBYTE,&var->val);
  *var->val.ubp=val_ubyte;
  (void)cast_nctype_void((nc_type)NC_UBYTE,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_ushort val_ushort)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_USHORT,true);
  (void)cast_void_nctype((nc_type)NC_USHORT,&var->val);
  *var->val.usp=val_ushort;
  (void)cast_nctype_void((nc_type)NC_USHORT,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_uint val_uint)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_UINT,true);
  (void)cast_void_nctype((nc_type)NC_UINT,&var->val);
  *var->val.uip=val_uint;
  (void)cast_nctype_void((nc_type)NC_UINT,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_int64 val_int64)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_INT64,true);
  (void)cast_void_nctype((nc_type)NC_INT64,&var->val);
  *var->val.i64p=val_int64;
  (void)cast_nctype_void((nc_type)NC_INT64,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
		 const std::string var_nm,
		 nco_uint64 val_uint64)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_UINT64,true);
  (void)cast_void_nctype((nc_type)NC_UINT64,&var->val);
  *var->val.ui64p=val_uint64;
  (void)cast_nctype_void((nc_type)NC_UINT64,&var->val);
  return var;
}

/* fxm: Allow ncap2 to instantiate NC_STRINGs
   var_sct *
   ncap_sclr_var_mk(
   const std::string var_nm,
   nco_string val_string)
   {
   var_sct *var;
   var=ncap_sclr_var_mk(var_nm,(nc_type)NC_STRING,true);
   (void)cast_void_nctype((nc_type)NC_STRING,&var->val);
   *var->val.sngp=val_string;
   (void)cast_nctype_void((nc_type)NC_STRING,&var->val);
   return var;
   }
*/ 


// Do an in-memory hyperslab !!
void 
ncap_get_var_mem( 
		 int dpt,                       // Current depth
		 int dpt_max,                   // Max depth ( same as number of dims) 
		 std::vector<int> &shp_vtr,     // shape of input var (in bytes)
		 NcapVector<dmn_sct*> &dmn_vtr, // New vectors
                 var_sct* var_in,
                 long nbr_lpp,
		 char *cp_in,                   // Pointer to (char*)var_in->val.vp
		 char *&cp_out){                // Reference pointer to space for new var values
  
  const std::string fnc_nm("ncap_get_var_mem"); 
  
  
  long idx;      
  long jdx;
  long srt=dmn_vtr[dpt]->srt;
  long end=dmn_vtr[dpt]->end;
  long cnt=dmn_vtr[dpt]->cnt;
  long srd=dmn_vtr[dpt]->srd;
  long slb_sz=shp_vtr[dpt];
  long dpt_cnt=var_in->dim[dpt]->cnt;;
  
  char *cp_srt=cp_in+ptrdiff_t(srt*slb_sz);
  char *cp_lcl;
  
  
  if(nco_dbg_lvl_get() >= nco_dbg_scl){
    std::ostringstream os;
    os<<"Depth=" << dpt<<" "<<dmn_vtr[dpt]->nm<<" "<<srt<<" "<<end<<" "<<cnt<<" "<<srd;
    dbg_prn(fnc_nm,os.str());
  }
  
  
  if(dpt == dpt_max){
    
    
    if(srd==1) {
        
      for(jdx=0 ; jdx <nbr_lpp ; jdx++){  
        (void)memcpy(cp_out, cp_srt, ptrdiff_t(cnt*slb_sz));
        cp_out+=ptrdiff_t(cnt*slb_sz);
        cp_srt+=ptrdiff_t(dpt_cnt*slb_sz); 
      }
    }
    if(srd>1) {  
      
      for(jdx=0 ; jdx<nbr_lpp ;jdx++){ 
        cp_lcl=cp_srt;
        for(idx=0 ; idx<cnt ; idx++ ){
          
          (void)memcpy(cp_out,cp_lcl,slb_sz);
          cp_out+=slb_sz;
          cp_lcl+=(ptrdiff_t)(srd*slb_sz);
        }
        cp_srt+=ptrdiff_t(dpt_cnt*slb_sz); 
      }

    }
    
  }
  
  if(dpt < dpt_max ){

    // The whole slab
    if( srd==1 && cnt==dpt_cnt) {
      (void)ncap_get_var_mem(dpt+1,dpt_max,shp_vtr,dmn_vtr,var_in, nbr_lpp*cnt,cp_srt,cp_out);
    }  

    //Single slab or contiguous block
    if(srd==1 && cnt<dpt_cnt){
      for(idx=0; idx<nbr_lpp;idx++){
        (void)ncap_get_var_mem(dpt+1,dpt_max,shp_vtr,dmn_vtr,var_in,cnt,cp_srt,cp_out);
	 cp_srt+=dpt_cnt*slb_sz; 
      }
    }
    // Discontinuous slab i.e srd > 1
    if(srd>1) {

      for(jdx=0; jdx<nbr_lpp;jdx++){
        cp_lcl=cp_srt; 
        for(idx=0; idx <cnt ;idx++){
          (void)ncap_get_var_mem(dpt+1,dpt_max,shp_vtr,dmn_vtr,var_in,1,cp_lcl,cp_out);
          cp_lcl+= ptrdiff_t(srd*slb_sz);
        }
        cp_srt+=dpt_cnt*slb_sz;
      }  
    }

  }
   
} /* ncap_get_var_mem */




void
nco_get_var_mem(
		var_sct *var_in,
		NcapVector<dmn_sct*> &dmn_vtr){
  
  int idx;
  int ncnt;
  int dmn_nbr;
  int dpt_max;
  
  void  *vp_out;
  char *cp_out; 
  char *cp_in;
  std::vector<int> shp_vtr;
  
  
  dmn_nbr=var_in->nbr_dim;
  
  ncnt=nco_typ_lng(var_in->type); 
  // Create shape vector for var_in
  shp_vtr.push_back(ncnt);
  for(idx=dmn_nbr-1 ; idx>0 ; idx--){
    ncnt*=var_in->dim[idx]->cnt;  
    shp_vtr.push_back(ncnt);
  }
  
  // Reverse vector
  std::reverse(shp_vtr.begin(),shp_vtr.end() ); 
  
  
  // Find space for output data
  ncnt=1;
  for(idx=0 ;idx < dmn_nbr ; idx++)
    ncnt*=dmn_vtr[idx]->cnt; 

  // Do nothing if whole slab is specified
  if(var_in->sz == ncnt)
    return;
  
  // Alloc space for output variables value
  vp_out=(void*)nco_malloc(ncnt*nco_typ_lng(var_in->type));
  
  cp_out=(char*)vp_out;
  cp_in=(char*)var_in->val.vp;
  
  // Work out max depth we have to go to 
  dpt_max=dmn_nbr;
  for(idx=dmn_nbr-1; idx>0 ; idx--)
    if( var_in->dim[idx]->cnt == dmn_vtr[idx]->cnt) 
      dpt_max--;
    else
      break;
  
  // Call in-memory nco_get_var() (n.b is recursive of course!!)
  (void)ncap_get_var_mem(0,dpt_max-1,shp_vtr,dmn_vtr,var_in,1L,cp_in,cp_out);
  

  var_in->sz=ncnt;
  
  (void)nco_free(var_in->val.vp);
  var_in->val.vp=vp_out;  
  
} /* end nco_get_var_mem()  */




void
ncap_put_var_mem(
int dpt,                        // Current 
int dpt_max,                    // Max depth ( same as number of dims) 
std::vector<int> &shp_vtr,      // shape of input var (in bytes)
NcapVector<lmt_sct*> &dmn_vtr,  // New vectors
var_sct* var_out,               // needed 
long nbr_lpp,                    // number of iterations
char *&cp_in,                   // Pointer to (char*)var_in->val.vp
char *cp_out)                   // Slab to be "put" 
{
  
  const std::string fnc_nm("ncap_put_var_mem"); 
  
  
  long idx;      
  long jdx;
  long srt=dmn_vtr[dpt]->srt;
  long end=dmn_vtr[dpt]->end;
  long cnt=dmn_vtr[dpt]->cnt;
  long srd=dmn_vtr[dpt]->srd;
  long slb_sz=shp_vtr[dpt];
  long dpt_out=var_out->dim[dpt]->cnt;
  
  //char *cp_end=cp_out+ptrdiff_t(srt*slb_sz);;
  char *cp_end;
  
    
  if(nco_dbg_lvl_get() >= nco_dbg_scl){
    std::ostringstream os;
    os<<"Depth=" << dpt<<" "<<dmn_vtr[dpt]->nm<<" "<<srt<<" "<<end<<" "<<cnt<<" "<<srd<<" "<<slb_sz<<" nbr_lpp="<<nbr_lpp;
    dbg_prn(fnc_nm,os.str());
  }
  

  
  if(dpt == dpt_max){
    cp_end=cp_out+ptrdiff_t(srt*slb_sz);
    for(idx=0; idx<nbr_lpp; idx++){
    
      if(srd==1) {
        (void)memcpy(cp_end,cp_in,cnt*slb_sz);
        cp_in+=(ptrdiff_t)cnt*slb_sz;
      }
      if(srd >1) {
        char *cp_lcl=cp_end;
        for(jdx=0 ; jdx<cnt ; jdx++ ){
          (void)memcpy(cp_lcl,cp_in,slb_sz);
          cp_in+=(ptrdiff_t)slb_sz;
          cp_lcl+=(ptrdiff_t)(srd*slb_sz);
        } //loop jdx
      }
      cp_end+=(ptrdiff_t)(dpt_out*slb_sz);

    }//loop idx
  } 

  if(dpt < dpt_max){
   

    // Slab the whole lot 
    if( srd==1 && cnt == dpt_out) {
      cp_end=cp_out;
      (void)ncap_put_var_mem(dpt+1,dpt_max,shp_vtr,dmn_vtr,var_out,nbr_lpp*cnt,cp_in,cp_end);
    } 

    // Slab single or contiguous block
    if(srd==1 && cnt < dpt_out){
      cp_end=cp_out+ptrdiff_t(srt*slb_sz);
      for(idx=0; idx<nbr_lpp; idx++){
        (void)ncap_put_var_mem(dpt+1,dpt_max,shp_vtr,dmn_vtr,var_out,cnt,cp_in,cp_end);
        cp_end+= ptrdiff_t(dpt_out*slb_sz);
      } //loop idx
    }

    // Slab discontinuous
    if(srd > 1) { 
      for(idx=0; idx<nbr_lpp; idx++){
        cp_end=cp_out+ptrdiff_t(srt*slb_sz);;
        for(jdx=0; jdx <cnt ;jdx++){
          (void)ncap_put_var_mem(dpt+1,dpt_max,shp_vtr,dmn_vtr,var_out,1,cp_in,cp_end);
          cp_end+= ptrdiff_t(srd*slb_sz);
        } //loop jdx
        cp_out+=ptrdiff_t(dpt_out*slb_sz);
      } //loop idx

    }
    
  }

  
}




// Do an in memory nco_put_var()  
void
nco_put_var_mem(
var_sct *var_in,
var_sct *var_out,
NcapVector<lmt_sct*> &dmn_vtr)
{
  
  int idx;
  int ncnt;
  int dmn_nbr;
  int dpt_max;
  
  char *cp_in;
  char *cp_out; 
  
  std::vector<int> shp_vtr;
  
  
  dmn_nbr=var_out->nbr_dim;
  
  ncnt=nco_typ_lng(var_out->type); 
  // Create shape vector for var_out
  shp_vtr.push_back(ncnt);
  for(idx=dmn_nbr-1 ; idx>0 ; idx--){
    ncnt*=var_out->dim[idx]->cnt;  
    shp_vtr.push_back(ncnt);
  }
  
  // Reverse vector
  std::reverse(shp_vtr.begin(),shp_vtr.end() ); 
  
  // Work out max depth we have to go to 
  dpt_max=dmn_nbr;

    
  for(idx=dmn_nbr-1; idx>0 ; idx--)
    if( var_out->dim[idx]->cnt == dmn_vtr[idx]->cnt) 
      dpt_max--;
    else
      break;

  cp_in=(char*)var_in->val.vp;
  cp_out=(char*)var_out->val.vp;
   
  // user has specified  the whole hyperslab
  if(var_in->sz==var_out->sz){
    (void)memcpy(cp_out,cp_in, var_in->sz*nco_typ_lng(var_in->type)); 
  }
  else
  // Call in-memory nco_put_var_mem (n.b is recursive of course!!)
    (void)ncap_put_var_mem(0,dpt_max-1,shp_vtr,dmn_vtr,var_out,1L,cp_in,cp_out);

  
} /* end nco_put_var_mem() */




// See if node contains any utility fuctions
// if so return true
bool 
ncap_fnc_srh(
RefAST tr
)
{
 std::string fnm;
  
  if(tr->getType()==FUNC){
    fnm=tr->getText();
    if(fnm=="set_miss" || fnm=="change_miss" || fnm=="delete_miss" || fnm=="ram_write" || fnm=="ram_delete") 
      return true;
  }
 
  
  tr=tr->getFirstChild();
 // examine all child nodes
  while(tr){
    if( ncap_fnc_srh(tr) )
      return true;
    tr=tr->getNextSibling();
  }
 
 return false;

}






/*********************************************************************************/
/* Following - all MPI optimization routines                                     */
/*********************************************************************************/  




// See if any VAR_ID/ATT_ID match any in str_vtr
// if so return true
bool 
ncap_mpi_srh_str(
		 RefAST ntr,
		 std::vector<std::string> &str_vtr
		 )
{
  
  RefAST tr=ntr->getFirstChild();
  
  if(ntr->getType()== VAR_ID || ntr->getType() == ATT_ID )
    // see if ID is in vector
    if( std::binary_search(str_vtr.begin(),str_vtr.end(),ntr->getText()))
      return true;
  
  
  /*  
      if(ntr->getType()== VAR_ID || ntr->getType() == ATT_ID ){
      for(int idx=0 ; idx<str_vtr.size() ; idx++)
      if( ntr->getText() == str_vtr[idx])
      return true;
      }
  */
  // examine all child nodes
  while(tr){
    if( ncap_mpi_srh_str(tr,str_vtr) )
      return true;
    tr=tr->getNextSibling();
  }  
  
  return false;
}

// Subtract matching elements of
// one list of strings from another
bool
ncap_sub_str_str(
		 std::vector<std::string> &in_vtr,
		 std::vector<std::string> &mb_vtr
		 )
{
  unsigned int idx;
  unsigned int jdx;
  unsigned int nbr_mb=mb_vtr.size();
  
  std::vector<std::string> out_vtr;
  
  if(mb_vtr.size()==0) 
    return false;
  
  for(idx=0 ; idx < in_vtr.size() ; idx++){
    for(jdx=0 ; jdx < nbr_mb; jdx++)       
      if( in_vtr[idx]== mb_vtr[jdx])
	break;
    if(jdx==nbr_mb) 
      out_vtr.push_back(in_vtr[idx]);
  }
  // no change
  if(in_vtr.size()==out_vtr.size())
    return false;
  
  // some elements subtracted
  in_vtr.swap(out_vtr);
  
  return true;
  /*
    in_vtr.clear();
    for(idx=0 ; idx <out_vtr.size() ; idx++)
    in_vtr.push_back(out_vtr[idx]);
    return true;
  */
}




//Extract all VAR_ID & ATT_ID from Expression
void ncap_mpi_get_id(
		     RefAST ntr,
		     std::vector<std::string> &str_vtr
		     )
{
  
  RefAST tr=ntr->getFirstChild();
  
  if(ntr->getType()== VAR_ID || ntr->getType() == ATT_ID )
    str_vtr.push_back(ntr->getText());
  
  // examine all child nodes
  while(tr){
    (void)ncap_mpi_get_id(tr,str_vtr);
    tr=tr->getNextSibling();
  }  
}


//Extract all LValues from Expression
void ncap_mpi_get_lvl(
		      RefAST ntr,
		      std::vector<std::string> &str_vtr
		      )
{
  RefAST tr=ntr->getFirstChild();
  
  switch(ntr->getType()){
  case PLUS_ASSIGN:
  case MINUS_ASSIGN:
  case TIMES_ASSIGN:
  case DIVIDE_ASSIGN:
    str_vtr.push_back(tr->getText());        
    (void)ncap_mpi_get_lvl(tr->getNextSibling(),str_vtr);
    break;
    
  case ASSIGN:
    // Grab text from first element in assign ie. text for VAR_ID or ATT_ID
    str_vtr.push_back(tr->getText());
    // Check child for LValues in LMT_LIST
    (void)ncap_mpi_get_lvl(tr,str_vtr);
    // Process RHS of equal sign
    (void)ncap_mpi_get_lvl(tr->getNextSibling(),str_vtr);
    
    break;
    
    // All these unary ops are Gauranteed to only
    // contain VAR_ID or ATT_ID !! no need to error
    // check
  case INC:
  case DEC:
  case POST_INC:
  case POST_DEC:
  case CALL_REF:
    str_vtr.push_back(tr->getText());
    break;
    
  default:
    while(tr){
      (void)ncap_mpi_get_lvl(tr,str_vtr);
      tr=tr->getNextSibling();
    }  
    break; 
  }
  
} // ncap_mpi_get_lvl   



bool
ncap_evl_srp(
	     std::vector<exp_sct**> &srp_vtr //self reverential pointer
	     )
{
  unsigned int idx;
  
  if(srp_vtr.empty()) 
    return true;
  
  for(idx=0 ; idx < srp_vtr.size() ; idx++)
    if( *srp_vtr[idx]) 
      return false;
  
  return true;
  
  
}

// Sort expressions for MPI optimization
//ntr is a pointer to nbr_lst statements of type EXPR
int ncap_mpi_srt(
                 RefAST ntr,   
		 int nbr_lst,
		 std::vector< std::vector<RefAST> > &all_ast_vtr,
                 prs_cls *prs_arg)
{
  const std::string fnc_nm("ncap_mpi_srt"); 
  int idx;
  int jdx;
  RefAST tr;
  
  exp_sct *exp_ptr;
  
  std::vector<exp_sct*> exp_vtr;
  
  
  // populate exp_vtr;
  if(nco_dbg_lvl_get() >= nco_dbg_std)
    dbg_prn(fnc_nm,"Start");
  
  
  
  
  idx=0;tr=ntr;
  
  while(idx++<nbr_lst ) {
    
    std::vector<std::string> cl_vtr;
    exp_ptr=new exp_sct();
    
    // Initialise structure with AST and Lvalues
    //exp_ptr->etr=tr->getFirstChild();         
    exp_ptr->etr=tr;         
    (void)ncap_mpi_get_lvl(tr->getFirstChild(),cl_vtr);
    //Sort Lvalues for speed
    std::sort(cl_vtr.begin(), cl_vtr.end());
    // remove any duplicates
    cl_vtr.erase(  std::unique(cl_vtr.begin(), cl_vtr.end()), 
                   cl_vtr.end()
                ); 
    
    exp_ptr->lvl_vtr=cl_vtr;
    
    exp_vtr.push_back(exp_ptr);
    tr=tr->getNextSibling();
  }
  
  
  // Populate dependency vector
  for(idx=0 ; idx < nbr_lst ;idx++) {
    // initialize local vars
    exp_ptr=exp_vtr[idx];
    // need a local copy of Lvalues
    // will subtract strings from this as we go through
    // the inner loop
    std::vector<std::string> out_vtr=exp_ptr->lvl_vtr; 	
    // expression without Lvalues
    if(out_vtr.empty()) 
      continue;
    
    //Inner loop
    for(jdx=(idx+1); jdx< nbr_lst ; jdx++){
      // See if inner-loop exprssion depends
      // on outer-loop expression 
      if( ncap_mpi_srh_str(exp_ptr->etr, exp_vtr[jdx]->lvl_vtr) ||
	  ncap_mpi_srh_str(exp_vtr[jdx]->etr,out_vtr)) {
	
	exp_vtr[jdx]->dpd_vtr.push_back(idx);
	exp_vtr[jdx]->srp_vtr.push_back( &exp_vtr[idx]);
	// Subtract strings  from out_vtr that are are also in exp_vtr[jdx]->lvl_vtr 
        out_vtr.erase(
          std::set_difference(
                    out_vtr.begin(),out_vtr.end(),
                    exp_vtr[jdx]->lvl_vtr.begin(),exp_vtr[jdx]->lvl_vtr.end(), 
                    out_vtr.begin()
		              ),
                    out_vtr.end()
                     );


      }
      // break out of inner loop if out_lvl_vtr empty
      if(out_vtr.empty())
	break ;
      
    }// loop jdx 
  }// loop idx
  
  
  // print out whole  structure
  /*
    for(idx=0; idx<nbr_lst ; idx++){
    // dereference
      exp_ptr=exp_vtr[idx];
      std::cout << exp_ptr->etr->toStringTree();
      std::cout <<"\n Lvalues  ";
      for(int jdx=0 ; jdx < exp_ptr->lvl_vtr.size() ; jdx++)
      std::cout << exp_ptr->lvl_vtr[jdx] <<" ";
    
      std::cout <<"\n Dependency  ";
      for(int jdx=0 ; jdx < exp_ptr->dpd_vtr.size() ; jdx++)
      std::cout << exp_ptr->dpd_vtr[jdx] <<" ";
      std::cout<<"\n";
    }
    
  */ 
  
  
  
  int icnt=0;
  
  while(icnt <nbr_lst){
    
    // Store list of dependent structures
    std::vector<RefAST> ast_vtr;      // Expressions -all lvalues defined in O
    std::vector<RefAST> ast_no_vtr;   // Expressions with an Lvalue(s) not defined in O        
    std::vector<int> grp_vtr;
    
    for(idx=0; idx <nbr_lst; idx++){
      if( exp_vtr[idx]==NULL) 
	continue;
      if( ncap_evl_srp(exp_vtr[idx]->srp_vtr))
	grp_vtr.push_back(idx);
    }// end idx
    
    for(idx=0 ; idx <(int)grp_vtr.size(); idx++){
      // Check for any Lvalues NOT in output  
      std::vector<std::string> out_lvl_vtr=exp_vtr[grp_vtr[idx]]->lvl_vtr;	
      int nbr_lvl=(int)out_lvl_vtr.size();

      for(jdx=0 ; jdx <nbr_lvl; jdx++) 
	if( prs_arg->var_vtr.find(out_lvl_vtr[jdx]) == NULL  ) break; 

      if(jdx==nbr_lvl)    
        ast_vtr.push_back( exp_vtr[grp_vtr[idx]]->etr);
      else  
        ast_no_vtr.push_back( exp_vtr[grp_vtr[idx]]->etr);

      delete exp_vtr[grp_vtr[idx]];
      exp_vtr[grp_vtr[idx]]=NULL;
    } //end idx
    
    // Save vectors in another vector !! 
    all_ast_vtr.push_back(ast_no_vtr);
    all_ast_vtr.push_back(ast_vtr);   
    icnt+=(int)grp_vtr.size();
    
    
  } // end while
  
  //Print out vectors
  if(nco_dbg_lvl_get() >0) {
    for(idx=0 ; idx<(int)all_ast_vtr.size(); idx++){
      for(jdx=0 ; jdx<(int)all_ast_vtr[idx].size(); jdx++)
	std::cout << all_ast_vtr[idx][jdx]->toStringTree()<<std::endl;
      std::cout <<"-------------------------------\n";
    } //end idx
  }
  if(nco_dbg_lvl_get() >= nco_dbg_std) dbg_prn(fnc_nm,"End");       
  
  return 0;
}

