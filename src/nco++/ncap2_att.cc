#include "ncap2_att.hh"


nco_bool              // O [flg] true if var has an att name                         
ncap_var_is_att(      //   [fnc] check var          
var_sct *var)         // I [sct] var to check
{
  if( strchr(var->nm,'@') !=NULL_CEWI ) 
    return True;
  return False;
}


std::string           // O [sng] text of va
ncap_att2var          //   [fnc] return text content of var
( prs_cls *prs_arg,   // I [cls] var/att symbol tables
  std::string att_nm) // I [sn]  contains var name to read
{

  std::string fnc_nm("ncap_att2var");
  std::string sn; 
  var_sct *var_att=NULL_CEWI; 
  NcapVar *Nvar=NULL;

  if(prs_arg->ntl_scn)
    Nvar=prs_arg->int_vtr.find(att_nm);

  if(Nvar==NULL) 
    Nvar=prs_arg->var_vtr.find(att_nm);

  if(Nvar !=NULL)
    var_att=nco_var_dpl(Nvar->var);
  else    
    var_att=ncap_att_init(att_nm,prs_arg);

  if(prs_arg->ntl_scn==False  && var_att==NULL_CEWI )
    err_prn(fnc_nm,"Unable to locate attribute " +att_nm+ " in input or output files.");

            
  /* empty so return empty string */ 
  if(var_att==NULL_CEWI )
    return sn;

  /* cant resolve pointer here */
  if(var_att->val.vp==NULL_CEWI)
  {
    nco_var_free(var_att);      
    return sn;
  }

  if(var_att->type !=NC_STRING && var_att->type !=NC_CHAR )
    err_prn("To use that attribute "+ att_nm +" as a variable pointer it must be a text type  NC_CHAR or NC_STRING"); 
    
  cast_void_nctype(var_att->type, &var_att->val );
  if(var_att->type == NC_STRING)
  {
      sn=var_att->val.sngp[0];
  }
  else if( var_att->type==NC_CHAR)
  {        
      char buffer[100]={'\0'};
      strncpy(buffer, var_att->val.cp, var_att->sz);
      sn=buffer;  
  } 
 
  cast_nctype_void(var_att->type, &var_att->val);
  nco_var_free(var_att);  

  return sn;

}



int                    // O [flg] 0 - att doesnt exist 1 - att exists
ncap_att2var_chk       //   [fnc] returns nco_bool
( prs_cls *prs_arg,    // I [cls] var/att symbol tables
  std::string att_nm)  // I [sn]  contains var name to read
{

  std::string fnc_nm("ncap_att2var");
  std::string sn;
  var_sct *var_att = NULL_CEWI;
  NcapVar *Nvar = NULL;

  if (prs_arg->ntl_scn)
    Nvar = prs_arg->int_vtr.find(att_nm);

  if (Nvar == NULL)
    Nvar = prs_arg->var_vtr.find(att_nm);

  if (Nvar)
    return 1;

  var_att = ncap_att_init(att_nm, prs_arg);

  if (var_att) {
    var_att = (var_sct *) nco_free(var_att);
    return 1;
  }
  else
  {
    return 0;
  }

}





var_sct *                    // O [sct] variable containing attribute 
ncap_att_init                //   [fnc] Grab an attribute from input file or symbol table
(const std::string s_va_nm,  // I [sng] att name of form var_nm&att_nm */ 
 prs_cls *prs_arg)           // I/O [vec]  of atts & vars 
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
  
  if(var_nm == "global"){
    var_id=NC_GLOBAL;
  }else{
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm.c_str(),&var_id);
    if(rcd != NC_NOERR) return NULL_CEWI;
  }
  
  var_ret=ncap_att_get(var_id,var_nm.c_str(),att_nm.c_str(),1,prs_arg);
  return var_ret;
}



var_sct*                      // O [sct] variable containing attribute 
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


int
ncap_att_gnrl
(const std::string s_dst,
 const std::string s_src,
 int location, /* I [flg] 1: attributes from INPUT file, 2: attributes from OUTPUT file */
 prs_cls  *prs_arg){
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

  // this holds the idx of transient attributes in var_vtr
  // these are one shot attributes that are used once
  std::vector<int> tr_int_vtr;
  // De-reference 
  NcapVarVector &var_vtr=prs_arg->var_vtr;
  NcapVarVector att_vtr; // hold new attributes.


  
  if(location == 1) fl_id=prs_arg->in_id;  
  if(location == 2) fl_id=prs_arg->out_id;  
  
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
      // Add to list (change the name!)
      if(var_att){ 
	s_fll=s_dst+"@"+std::string(att_nm);
        Nvar=new NcapVar(var_att,s_fll);
	att_vtr.push_back(Nvar);
      } 
    } // end for
  }// end rcd
  
  sz=var_vtr.size();
  if(s_dst != s_src && (srt_idx=var_vtr.find_lwr(s_src+"@")) != -1){  
    
    for(idx=srt_idx;idx<sz;idx++){ 
      if(s_src != var_vtr[idx]->getVar()) break;
      if((var_vtr)[idx]->xpr_typ != ncap_att) continue;
      tmp_att_nm=var_vtr[idx]->getAtt().c_str();
      // skip missing value, scale_factor, add_offset
      if(!strcmp(tmp_att_nm,nco_mss_val_sng_get()) || !strcmp(tmp_att_nm,scl_fct_sng) || !strcmp(tmp_att_nm,add_fst_sng))
        continue;

      // Create string for new attribute
      s_fll=s_dst+"@"+(var_vtr[idx]->getAtt());
      // mark transient att
      if(var_vtr[idx]->flg_mem )
        tr_int_vtr.push_back(idx);

      var_att=nco_var_dpl(var_vtr[idx]->var);
      Nvar=new NcapVar(var_att,s_fll);
      att_vtr.push_back(Nvar);
    }
  }

  //delete transient atts
  // nb go through vector in reverse order deleting
  if(tr_int_vtr.size())
    for(idx=tr_int_vtr.size()-1; idx>=0; idx--)
      var_vtr.erase(tr_int_vtr[idx]);





  sz=att_vtr.size();
  // add new att to list;
  for(idx=0;idx< sz;idx++){
    
#ifdef _OPENMP
    if(omp_in_parallel())
      prs_arg->thr_vtr.push_back(att_vtr[idx]);
    else
      var_vtr.push_ow(att_vtr[idx]);         
#else
    var_vtr.push_ow(att_vtr[idx]);         
#endif
  }

  return sz;
  
} /* end ncap_att_gnrl() */


nco_bool /* O [flg] true if var has been stretched */
ncap_att_stretch /* stretch a single valued attribute from 1 to sz */
(var_sct* var, /* I/O [sct] variable */       
 long nw_sz) /* I [nbr] new var size */
{
  long  idx;
  long  var_typ_sz;  
  void* vp;
  char *cp;

  
  if(var->sz > 1L || nw_sz <1) return false; 
  if(nw_sz==1) return true;
  var_typ_sz=nco_typ_lng(var->type);

  // shouldnt really use this function with NC_STRING
  // All it does is free exising single string
  // and and calloc space for new ones
  if(var->type == (nc_type)NC_STRING)
  {
    ptr_unn nw_val;

    nw_val.sngp=(char**)nco_calloc(nw_sz,var_typ_sz);
    (void)cast_void_nctype((nc_type)NC_STRING,&var->val);

    for(idx=0;idx<nw_sz;idx++)
      nw_val.sngp[idx]=  var->val.sngp[0] ? strdup(var->val.sngp[0]) : NULL_CEWI ;


    var->val.sngp[0]=(nco_string )nco_free(var->val.sngp[0]);
    var->val.sngp=(nco_string*)nco_free(var->val.sngp);

    var->val=nw_val;

    (void)cast_nctype_void((nc_type)NC_STRING,&var->val);
    var->sz=nw_sz;
  }
  else
  {
    vp=(void*)nco_malloc(nw_sz*var_typ_sz);    
    for(idx=0;idx<nw_sz;idx++){
      cp=(char*)vp+(ptrdiff_t)(idx*var_typ_sz);
      memcpy(cp,var->val.vp,var_typ_sz);
    }

    var->val.vp=(void*)nco_free(var->val.vp);
    var->sz=nw_sz;
    var->val.vp=vp;

  }
  
  return true;
} /* end ncap_att_stretch */

nco_bool                 /* O [flg] true if var has been stretched */
ncap_att_char_stretch    /* pad out or truncate a NC_CHAR attribute */
(var_sct* var,           /* I/O [sct] var/att */
 long nw_sz)             /* I [nbr] new var size */
{
  long  var_typ_sz;
  char *cp;
  void* vp;

  if(var->type != NC_CHAR)
    return False;

  var_typ_sz=nco_typ_lng(NC_CHAR);


  vp=(void*)nco_calloc(var_typ_sz,nw_sz);
  cp=(char*)vp;
  cast_void_nctype(NC_CHAR,&var->val);

  if( nw_sz > var->sz )
    strncpy(cp, var->val.cp, var->sz);
  else
    strncpy(cp, var->val.cp,nw_sz);


  cast_nctype_void(NC_CHAR,&var->val);

  var->val.vp=(void*)nco_free(var->val.vp);
  var->sz=nw_sz;
  var->val.vp=vp;

  return true;

} /* end ncap_att_char_stretch */

int 
ncap_att_cpy_sct
(var_sct *var1,
 var_sct *var2,
 prs_cls  *prs_arg){
  
  NcapVar *Nvar=prs_arg->var_vtr.find(var1->nm);
  // Do attribute propagation only if
  // var doesn't already exist 
  
  if(!Nvar ||  Nvar->flg_stt==1)
    (void)ncap_att_cpy(var1->nm,var2->nm,prs_arg);
  
  return 0;
  
} 	


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
    if(var->type != NC_CHAR && var->type != NC_STRING)
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

  /* only print if format string NOT user defined  */
  if(att_in_sng ==(char*)NULL && var->type != NC_CHAR && var->type != NC_STRING )
      (void)fprintf(stdout,"\n");
  
  (void)cast_nctype_void(var->type,&var->val);
  
  (void)fflush(stdout);
  
} /* end ncap_att_prn() */


char * 
ncap_att_sprn     /* [fnc] Print a single attribute*/
(var_sct *var,   /* I Variable containing att */
 char *const att_in_sng) /* user defined format string */
{

  char char_foo;
  char dlm_sng[3];
  char att_sng[NCO_MAX_LEN_FMT_SNG];
   
  char *tp;
  char *cp;    
  char *cp_max;  

  long att_lmn;
  long att_sz;
  
  /* Copy value to avoid indirection in loop over att_sz */
  att_sz=var->sz;
  
  cp=(char*)nco_calloc(sizeof(char),(NC_MAX_ATTRS+100)); 
  tp=cp;  
  cp_max=cp+ (size_t)(NC_MAX_ATTRS+100); 



  if(att_in_sng ==(char*)NULL) {        
    /* default dont bother if att info if type is text */
    if(var->type != NC_CHAR && var->type != NC_STRING)
    {     
      (void)sprintf(tp,"%s, size = %li %s, value = ",var->nm,att_sz,nco_typ_sng(var->type));
      tp+=strlen(tp); 
    }
    /* Typecast pointer to values before access */
    (void)strcpy(dlm_sng,", ");
    (void)sprintf(att_sng,"%s%%s",nco_typ_fmt_sng(var->type));
    /* user defined format string */ 
  } else {
    (void)strcpy(att_sng,att_in_sng);
    (void)strcpy(dlm_sng,"");
  }
      
  (void)cast_void_nctype(var->type,&var->val);

  
  switch(var->type){
  case NC_FLOAT:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max ;att_lmn++) { (void)sprintf(tp,att_sng,var->val.fp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_DOUBLE:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max ;att_lmn++) { (void)sprintf(tp,att_sng,var->val.dp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");  tp+=strlen(tp); }
    break;
  case NC_SHORT:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max;att_lmn++) { (void)sprintf(tp,att_sng,var->val.sp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_INT:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max;att_lmn++) { (void)sprintf(tp,att_sng,(long)var->val.ip[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_CHAR:
    if(att_in_sng ==(char*)NULL)    
      /* remember string initialized to nulls */
      strncpy(tp,var->val.cp,att_sz);
    else          
    {
      for(att_lmn=0;att_lmn<att_sz && tp<cp_max ;att_lmn++)
        /* Assume \0 is string terminator and do not print it */
        if((char_foo=var->val.cp[att_lmn]) != '\0') 
	{ 
           (void)sprintf(tp,att_sng,char_foo);   
           tp+=strlen(tp); 
        } 
    } /* end loop over element */

    break;
  case NC_BYTE:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max;att_lmn++) { (void)sprintf(tp,att_sng,var->val.bp[att_lmn], (att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_UBYTE:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max;att_lmn++) { (void)sprintf(tp,att_sng,var->val.ubp[att_lmn], (att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_USHORT:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max;att_lmn++) { (void)sprintf(tp,att_sng,var->val.usp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_UINT:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max;att_lmn++) { (void)sprintf(tp,att_sng,var->val.uip[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_INT64:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max;att_lmn++) { (void)sprintf(tp,att_sng,var->val.i64p[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_UINT64:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max;att_lmn++) { (void)sprintf(tp,att_sng,var->val.ui64p[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : ""); tp+=strlen(tp); }
    break;
  case NC_STRING:
    for(att_lmn=0;att_lmn<att_sz && tp<cp_max ; att_lmn++) { (void)sprintf(tp,att_sng,var->val.sngp[att_lmn],(att_lmn != att_sz-1) ? dlm_sng : "");tp+=strlen(tp); }
    break;
  default: nco_dfl_case_nc_type_err();
    break;
  } /* end switch */
  
  (void)cast_nctype_void(var->type,&var->val);
  
  if( tp >= cp_max )
  {
    cp=(char*)nco_realloc(cp, NC_MAX_ATTRS*sizeof(char));     
    cp[NC_MAX_ATTRS-1]='\0';
  }
  else
  {
    cp=(char*)nco_realloc(cp, sizeof(char) * (strlen(cp)+1));     
  }       
  
  return cp;

} /* end ncap_att_prn() */


int          /* number appended */ 
ncap_att_str /* extract string(s) from a NC_CHAR or NC_STRING type attribute */
(var_sct *var_att, 
 std::vector<std::string> &str_vtr)
{
  int idx;
  int srt_size=str_vtr.size();  
  char *cstr;
  
  (void)cast_void_nctype((nc_type)var_att->type,&var_att->val);

  if(var_att->type==NC_STRING)
  {

    for(idx=0;idx<var_att->sz;idx++)
    {  
      cstr=var_att->val.sngp[idx];
      str_vtr.push_back(cstr);
    }     

  }

  if(var_att->type==NC_CHAR)
  { 
    char buffer[NC_MAX_NAME+1];
    strncpy(buffer, var_att->val.cp, var_att->sz);        
    buffer[var_att->sz+1]='\0'; 
    str_vtr.push_back(buffer);
  } 
  (void)cast_nctype_void((nc_type)var_att->type,&var_att->val);
  
  return (str_vtr.size() - srt_size);  



}


char *          /* new malloc'ed string */ 
ncap_att_char  /* extract string from a NC_CHAR or first NC_STRING */
(var_sct *var_att)
{
  char *cstr=NULL;
  
  (void)cast_void_nctype((nc_type)var_att->type,&var_att->val);

  if(var_att->type==NC_STRING)
  {
    /* grab only first string */
    cstr=strdup(var_att->val.sngp[0]);

  }

  if(var_att->type==NC_CHAR)
  { 
    cstr=(char*)nco_malloc( (var_att->sz+1) *sizeof(char));
    strncpy(cstr, var_att->val.cp, var_att->sz);        
    cstr[var_att->sz]='\0'; 
  } 
  (void)cast_nctype_void((nc_type)var_att->type,&var_att->val);
  
  return cstr;  

}


var_sct *
ncap_att_cll_mtd(
const char *nm,
dmn_sct **dim,
int nbr_dim,
enum nco_op_typ op_typ
){
  int idx;
  var_sct *var_att;
  std::string var_nm;
  std::string att_txt("");

  var_nm=std::string(nm)+SCS("@cell_methods");

  for(idx=0;idx<nbr_dim;idx++){
      att_txt += std::string(dim[idx]->nm);
    if(idx<nbr_dim-1) att_txt+=SCS(", ");
  }


  att_txt+= SCS(": ")+SCS( nco_op_typ_to_rdc_sng(op_typ));

  var_att=ncap_sclr_var_mk(var_nm, NC_CHAR,false);
  var_att->val.vp=(void*)nco_malloc( sizeof(char)*att_txt.size());

  cast_void_nctype(NC_CHAR,&var_att->val);
  strncpy(var_att->val.cp, att_txt.c_str(), att_txt.size());
  var_att->sz=att_txt.size();
  cast_nctype_void(NC_CHAR,&var_att->val);

  return var_att;

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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
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
ncap_sclr_var_mk
(const std::string var_nm,
 nco_uint64 val_uint64)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,(nc_type)NC_UINT64,true);
  (void)cast_void_nctype((nc_type)NC_UINT64,&var->val);
  *var->val.ui64p=val_uint64;
  (void)cast_nctype_void((nc_type)NC_UINT64,&var->val);
  return var;
}

/* fxm: Allow ncap2 to instantiate NC_STRINGs */
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

