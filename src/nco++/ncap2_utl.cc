/* $Header: /data/zender/nco_20150216/nco/src/nco++/ncap2_utl.cc,v 1.56 2007-05-09 05:58:27 zender Exp $ */

/* Purpose: netCDF arithmetic processor */

/* Copyright (C) 1995--2005 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#include <assert.h>
#include <ctype.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include "ncap2.hh" /* netCDF arithmetic processor */
#include "NcapVar.hh"
#include "sdo_utl.hh"

/* have removed extern -- (not linking to ncap_lex.l */
/*extern*/ char ncap_err_sng[200]; /* [sng] Buffer for error string (declared in ncap_lex.l) */

var_sct *                  /* O [sct] initialized variable */
ncap_var_init(
const std::string &snm,    /* I [sng] variable name constant */
prs_sct *prs_arg,          /* I/O  vectors of atts,vars,dims, filenames */
bool bfll)                 /* if true fill var with data */ 
{
  /* Purpose: Initialize variable structure, retrieve variable values from disk
     Parser calls ncap_var_init() when it encounters a new RHS variable */

  const char fnc_nm[]="ncap_var_init"; 

  int idx;
  int dmn_var_nbr;
  int *dim_id=NULL_CEWI;
  int var_id;
  int rcd;
  int fl_id;
  
  bool bskp_npt=false;
  
  char dmn_nm[NC_MAX_NAME];
  const char *var_nm;

  dmn_sct *dmn_fd; 
  dmn_sct *dmn_nw;
  
  dmn_sct **dmn_out;  // dereferencing
  int nbr_dmn_out  ;  // dereferencing
  
  var_sct *var;

  NcapVar *Nvar;

  var_nm=snm.c_str();


  // INITIAL SCAN
  if(prs_arg->ntl_scn){
    // check int vtr
    Nvar=prs_arg->ptr_int_vtr->find(var_nm);
    // check var_vtr (output)  
    if(Nvar==NULL)
      Nvar=prs_arg->ptr_var_vtr->find(var_nm);
   
    if(Nvar) {
      var=Nvar->cpyVarNoData();
      return var;
    }
    bfll=false;
  }


  // FINAL SCAN
  // We have a  dilema -- its possible for a variable to exist in input & output 
  // with the var in output defined but empty - -this could occur with the
  // increment/decrement ops e.g time++, four+=10 or a LHS hyperslab 
  // e.g time(0:2)=666 or var on LHS & RHS at the same time time()=time*10;
  // So what we want to happen in these cases is read the original var from
  // input. --Later on in the script the empty var in output will be populated 
  // and subsequent reads will occur from  output 
  if(!prs_arg->ntl_scn){
    Nvar=prs_arg->ptr_var_vtr->find(var_nm);
   
    // var is defined in O and populated
    if(Nvar && Nvar->flg_stt==2 && !Nvar->flg_mem){
      var=Nvar->cpyVarNoData();
      //var=Nvar->cpyVar();
      fl_id=prs_arg->out_id; 
      //yuck - yuck use a goto
      goto lbl_end; 
    }
    // var is defined in O and populated && a RAM variable
    if(Nvar && Nvar->flg_stt==2 && Nvar->flg_mem){
      var=Nvar->cpyVar();
      var->tally=(long *)NULL;
      return var;
    }


    // var is defined in O but NOT populated
    // Set flag so read is tried only from input
    // Maybe not the best solution ?
    // what else ??? 
    if(Nvar && Nvar->flg_stt==1){
      bskp_npt=true;

    } 
  }

  /* Check output file for var */  
  rcd=nco_inq_varid_flg(prs_arg->out_id,var_nm,&var_id);
  if(rcd == NC_NOERR && !bskp_npt){
    fl_id=prs_arg->out_id;
  }else{
    /* Check input file for ID */
    rcd=nco_inq_varid_flg(prs_arg->in_id,var_nm,&var_id);
    if(rcd != NC_NOERR){
      /* Return NULL if variable not in input or output file */
      std::ostringstream os;
      os<<"Unable to find variable " <<var_nm << " in " << prs_arg->fl_in <<" or " << prs_arg->fl_out;
      wrn_prn(fnc_nm,os.str());
      return NULL_CEWI;
    } /* end if */
    
    /* Find dimensions used in var
       Learn which are not already in output list prs_arg->dmn_out and output file
       Add these to output list and output file */
    (void)nco_redef(prs_arg->out_id);
    fl_id=prs_arg->in_id;
    
    (void)nco_inq_varndims(fl_id,var_id,&dmn_var_nbr);
    if(dmn_var_nbr>0){
      dim_id=(int *)nco_malloc(dmn_var_nbr*sizeof(int));
      
      (void)nco_inq_vardimid(fl_id,var_id,dim_id);
      for(idx=0;idx<dmn_var_nbr;idx++){ 
	// get dim name
	(void)nco_inq_dimname(fl_id,dim_id[idx],dmn_nm);
        // check if dim is already in output
        if(prs_arg->ptr_dmn_out_vtr->find(dmn_nm) != NULL) continue; 
	// Get dim from input list
        dmn_fd= prs_arg->ptr_dmn_in_vtr->find(dmn_nm);
	// not in list -- crash out
	if(dmn_fd == NULL_CEWI){
          std::ostringstream os;
          os<<"Unable to find dimension " <<dmn_nm << " in " << prs_arg->fl_in <<" or " << prs_arg->fl_out;
          err_prn(fnc_nm,os.str());
	}
	   
        dmn_nw=nco_dmn_dpl(dmn_fd);
	(void)nco_dmn_xrf(dmn_nw,dmn_fd);
	// write dim to output
	(void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,&dmn_nw,1);          
	// Add new dim to output list
	(void)prs_arg->ptr_dmn_out_vtr->push_back(dmn_nw);

	if(dbg_lvl_get() > 2) {
          std::ostringstream os;
          os << "Found new dimension " << dmn_nm << " in input variable " << var_nm <<" in file " <<prs_arg->fl_in;
          os << ". Defining dimension " << dmn_nm << " in output file " << prs_arg->fl_out;
          dbg_prn(fnc_nm,os.str());

	}
      }
      (void)nco_free(dim_id);
    }
    (void)nco_enddef(prs_arg->out_id); 
  } // end else  
  
  if(dbg_lvl_get() > 2) {
    std::ostringstream os;
    os<< "Parser VAR action called ncap_var_init() to retrieve " <<var_nm <<" from disk";
    dbg_prn(fnc_nm,os.str());  
  }



  nbr_dmn_out=prs_arg->ptr_dmn_out_vtr->size();
  dmn_out=& ( (*prs_arg->ptr_dmn_out_vtr)[0]);

  var=nco_var_fll(fl_id,var_id,var_nm,dmn_out,nbr_dmn_out);
  /*  var->nm=(char *)nco_malloc((strlen(var_nm)+1UL)*sizeof(char));
  (void)strcpy(var->nm,var_nm); */
  /* Tally is not required yet since ncap does not perform cross-file operations (yet) */
  /* var->tally=(long *)nco_malloc_dbg(var->sz*sizeof(long),"Unable to malloc() tally buffer in variable initialization",fnc_nm);
      (void)nco_zero_long(var->sz,var->tally); */

lbl_end:
  var->tally=(long *)NULL;

  /* Retrieve variable values from disk into memory */
  if(bfll)
     (void)nco_var_get(fl_id,var); 

  return var;
} /* end ncap_var_init() */

int                /* O  [bool] bool - true if sucessful */
ncap_var_write     /*   [fnc] Write var to output file prs_arg->fl_out */ 
(var_sct *var,     /* I  [sct] variable to be written - freed at end */  
 bool bram,        /* I  [bool] true if a ram only variable */
 prs_sct *prs_arg) /* I/O vectors of atts & vars & file names  */
{
  /* Purpose: Define variable in output file and write variable */
  /*  const char mss_val_sng[]="missing_value"; *//* [sng] Unidata standard string for missing value */
  const char add_fst_sng[]="add_offset"; /* [sng] Unidata standard string for add offset */
  const char scl_fct_sng[]="scale_factor"; /* [sng] Unidata standard string for scale factor */
  const char fnc_nm[]="ncap_var_write"; 

  int rcd; /* [rcd] Return code */
  int var_out_id;
  
  bool bdef=false;
  NcapVar *Nvar;

#ifdef NCO_RUSAGE_DBG
  long maxrss; /* [B] Maximum resident set size */
#endif /* !NCO_RUSAGE_DBG */

  // INITIAL SCAN
  if(prs_arg->ntl_scn){
    Nvar=prs_arg->ptr_var_vtr->find(var->nm);
    if(Nvar) { 
      var=nco_var_free(var);
      return True;
    }

    Nvar=prs_arg->ptr_int_vtr->find(var->nm);
    if(Nvar) { 
      var=nco_var_free(var);
      return True;
    }  
      
    Nvar=new NcapVar(var,"");
    Nvar->flg_mem=bram;
    prs_arg->ptr_int_vtr->push(Nvar);
    return True;
  } 

  // FINAL SCAN
  Nvar=prs_arg->ptr_var_vtr->find(var->nm);
  if(Nvar) {
    // temporary fix make typ_dsk same as type
    Nvar->var->typ_dsk=Nvar->var->type;
    bdef=true;
  } 


  // Deal with a RAM variable
  if(bdef && Nvar->flg_mem){
    var_sct *var_inf;
    ptr_unn ptr_swp;

    var_inf=Nvar->cpyVarNoData();


    /* check sizes are the same */
    if(var_inf->sz != var->sz) {
      std::ostringstream os;
      os<< "RAM Variable "<< var->nm << " size=" << var->sz << " has aleady been saved in ";
      os<< prs_arg->fl_out << " with size=" << var_inf->sz;
       
      wrn_prn(fnc_nm,os.str());  

      var = nco_var_free(var);
      var_inf=nco_var_free(var_inf);
      return False;
    }

    /* convert type to disk type */
    var=nco_var_cnf_typ(var_inf->type,var);    

    //Swap values about  

    var_inf->val=var->val;
    var->val.vp=(void*)NULL;


    // Check for "new" missing value;
    if(var->has_mss_val){
    //Swap values about  
     ptr_swp.vp=var_inf->mss_val.vp;
     var_inf->mss_val.vp=var->mss_val.vp; 
     var->mss_val.vp=ptr_swp.vp;
     var_inf->has_mss_val=true; 
    }     
    (void)nco_var_free(Nvar->var);  
     Nvar->var=var_inf;     
     Nvar->flg_stt=2;

     assert(Nvar->var->val.vp !=NULL);

    (void)nco_var_free(var);

    return true;
  }



  // var is already defined & populated in output 
  if(bdef && !Nvar->flg_mem && Nvar->flg_stt==2){
    var_sct* var_swp;
    var_sct* var_inf;
    var_inf=Nvar->cpyVarNoData();


    /* check sizes are the same */
    if(var_inf->sz != var->sz) {
      std::ostringstream os;
      os<< "Variable "<< var->nm << " size=" << var->sz << " has aleady been saved in ";
      os<< prs_arg->fl_out << " with size=" << var_inf->sz;
       
      wrn_prn(fnc_nm,os.str());  

      var = nco_var_free(var);
      var_inf=nco_var_free(var_inf);
      return False;
    }

    /* convert type to disk type */
    var=nco_var_cnf_typ(var_inf->type,var);    

    //Swap values about
    var_inf->val=var->val;var->val.vp=(void*)NULL;
    var_swp=var;var=var_inf;var_inf=var_swp;
    
    var_inf=nco_var_free(var_inf);

    var_out_id=var->id;
   
  } 

  // Deal with a new RAM only variable
  if(!bdef && bram){
    NcapVar *NewNvar=new NcapVar(var,"");
    NewNvar->flg_mem=bram;
    NewNvar->flg_stt=2;
    NewNvar->var->id=-1;
    NewNvar->var->nc_id=-1;
    prs_arg->ptr_var_vtr->push(NewNvar);
    return True;
  }


  rcd=nco_inq_varid_flg(prs_arg->out_id,var->nm,&var_out_id);

  /* Put file in define mode to allow metadata writing */
  (void)nco_redef(prs_arg->out_id);
  
  /* Define variable */   
  if(!bdef)(void)nco_def_var(prs_arg->out_id,var->nm,var->type,var->nbr_dim,var->dmn_id,&var_out_id);
  /* Put missing value */  
  if(var->has_mss_val) (void)nco_put_att(prs_arg->out_id,var_out_id,nco_mss_val_sng_get(),var->type,1,var->mss_val.vp);
  
      /* Write/overwrite scale_factor and add_offset attributes */
  if(var->pck_ram){ /* Variable is packed in memory */
  if(var->has_scl_fct) (void)nco_put_att(prs_arg->out_id,var_out_id,scl_fct_sng,var->typ_upk,1,var->scl_fct.vp);
  if(var->has_add_fst) (void)nco_put_att(prs_arg->out_id,var_out_id,add_fst_sng,var->typ_upk,1,var->add_fst.vp);
      } /* endif pck_ram */

  /* Take output file out of define mode */
  (void)nco_enddef(prs_arg->out_id);
  
  /* Write variable */ 
  if(var->nbr_dim == 0){
    (void)nco_put_var1(prs_arg->out_id,var_out_id,0L,var->val.vp,var->type);
  }else{
    (void)nco_put_vara(prs_arg->out_id,var_out_id,var->srt,var->cnt,var->val.vp,var->type);
  } /* end else */
  
#ifdef NCO_RUSAGE_DBG
  /* Compile: cd ~/nco/bld;make 'USR_TKN=-DNCO_RUSAGE_DBG';cd - */
  /* Print rusage memory usage statistics */
  if(dbg_lvl_get() >= 0) {
    std::ostringstream os;
    os<<" Writing variable "<<var_nm; <<" to disk.";
    dbg_prn(fnc_nm,os.str());
  }
  maxrss=nco_mmr_rusage_prn((int)0);
#endif /* !NCO_RUSAGE_DBG */

  // save variable to output vector if new
  if(!bdef) {
    var_sct *var1;
    var1=nco_var_dpl(var);
    var1->val.vp=(void*)nco_free(var1->val.vp);
    var1->id=var_out_id;
    var1->nc_id=prs_arg->out_id;
    //temporary fix .. make typ_dsk same as type
    var1->typ_dsk=var1->type;
    Nvar=new NcapVar(var1);
    (void)prs_arg->ptr_var_vtr->push(Nvar);          
  } 
    
   var=nco_var_free(var);

  //Set flag -  indicates var is DEFINED && POPULATED
  Nvar->flg_stt=2;

  return True;
} /* end ncap_var_write() */

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
  return var_ret;
}

var_sct*
ncap_att_get
(int var_id,
 const char *var_nm,
 const char *att_nm,
 prs_sct *prs_arg)
{
  long sz;
  int rcd;
  char *ln_nm;

  nc_type type;
  var_sct *var_ret;
  
  rcd=nco_inq_att_flg(prs_arg->in_id,var_id,att_nm,&type,&sz);
  if (rcd == NC_ENOTATT) 
    return NULL_CEWI;


  var_ret=(var_sct*)nco_malloc(sizeof(var_sct));
  (void)var_dfl_set(var_ret);
  
  // Make name of the form var_nm@att_nm
  ln_nm=(char*)nco_malloc( (strlen(var_nm)+strlen(att_nm)+2)*sizeof(char));
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
   rcd=nco_get_att(prs_arg->in_id,var_id,att_nm,var_ret->val.vp,type);
   if (rcd !=NC_NOERR) {
    var_ret=nco_var_free(var_ret);
    return NULL_CEWI;
   }
  }
  return var_ret; 
}

var_sct *                  /* O [sct] variable containing attribute */
ncap_att_init(             /*   [fnc] Grab an attribute from input file */
const std::string s_va_nm, /* I [sng] att name of form var_nm&att_nm */ 
prs_sct *prs_arg)          /* I/O vectors of atts & vars & file names  */
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

  var_ret=ncap_att_get(var_id,var_nm.c_str(),att_nm.c_str(),prs_arg);
  
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

  if(var->sz == 1L && nw_sz >1){
    
    var_typ_sz=nco_typ_lng(var->type);
    vp=(void*)nco_malloc(nw_sz*var_typ_sz);
    for(idx=0 ; idx < nw_sz ;idx++){
      cp=(char*)vp + (ptrdiff_t)(idx*var_typ_sz);
      memcpy(cp,var->val.vp ,var_typ_sz);
    }
  
    var->val.vp=(void*)nco_free(var->val.vp);
    var->sz=nw_sz;
    var->val.vp=vp;
    return true;
  }
  
    return false; 

} /* end ncap_att_stretch */

int
ncap_att_gnrl
(const std::string s_dst,
 const std::string s_src,
 prs_sct  *prs_arg
){
  int idx;
  int sz;
  int rcd;
  int var_id; 
  int nbr_att;
  char att_nm[NC_MAX_NAME];

  var_sct *var_att;

  std::string s_fll;
 
  NcapVar *Nvar;
  NcapVarVector var_vtr;   
  NcapVector <var_sct*> att_vtr; //hold new attributtes.
        
  // De-reference 
  var_vtr= *prs_arg->ptr_var_vtr;

    // get var_id
  rcd=nco_inq_varid_flg(prs_arg->in_id,s_src.c_str(),&var_id);
          
  if(rcd == NC_NOERR){
    (void)nco_inq_varnatts(prs_arg->in_id,var_id,&nbr_att);
    // loop though attributes
    for(idx=0; idx <nbr_att ; idx++){
      (void)nco_inq_attname(prs_arg->in_id,var_id,idx,att_nm);
       var_att=ncap_att_get(var_id,s_src.c_str(),att_nm,prs_arg);
       // now add to list( change the name!!)
       if(var_att){ 
          std::string s_att(att_nm);
	  s_fll=s_dst+"@"+ s_att;
          nco_free(var_att->nm);
          var_att->nm=strdup(s_fll.c_str());
          att_vtr.push_back(var_att); 
       } 
      } // end for
    }// end rcd

    sz=var_vtr.size();
    for(idx=0; idx < sz ; idx++){
      if( (var_vtr)[idx]->xpr_typ != ncap_att) continue;
      if( s_src == var_vtr[idx]->getVar() ){
        // Create string for new attribute
        s_fll= s_dst +"@"+(var_vtr[idx]->getAtt());
        var_att=nco_var_dpl(var_vtr[idx]->var);
	nco_free(var_att->nm);
	var_att->nm=strdup(s_fll.c_str());
        att_vtr.push_back(var_att);  
      } 
    }
    // add new att to list;
    for(idx=0 ; idx < att_vtr.size() ; idx++){
      std::string s_out(att_vtr[idx]->nm);
      // skip missing values
      if( s_out.find("@missing_value") != std::string::npos){
	(void)nco_var_free(att_vtr[idx]);
        continue;
      } 

      Nvar=new NcapVar(nco_var_dpl(att_vtr[idx]),s_out ); 
      prs_arg->ptr_var_vtr->push_ow(Nvar);
      (void)nco_var_free(att_vtr[idx]);
    }

    return att_vtr.size();
    
} /* end ncap_att_gnrl() */
  
int
ncap_att_cpy
(const std::string s_dst,
 const std::string s_src,
 prs_sct  *prs_arg)
{
 
  int nbr_att=0;

  if(prs_arg->ATT_PROPAGATE && s_dst != s_src )
      nbr_att=ncap_att_gnrl(s_dst,s_src,prs_arg);

  if(prs_arg->ATT_INHERIT)
      nbr_att=ncap_att_gnrl(s_dst,s_dst,prs_arg);

  return nbr_att;
}

sym_sct *                    /* O [sct] return sym_sct */
ncap_sym_init                /*  [fnc] populate & return a symbol table structure */
(const char * const sym_nm,  /* I [sng] symbol name */
 double (*fnc_dbl)(double),  /* I [fnc_dbl] Pointer to double function */
 float (*fnc_flt)(float))    /* I [fnc_flt] Pointer to float  function */
{ 
  /* Purpose: Allocate space for sym_sct then initialize */
  sym_sct *symbol;
  symbol=(sym_sct *)nco_malloc(sizeof(sym_sct));
  symbol->nm=(char *)strdup(sym_nm);
  symbol->fnc_dbl=fnc_dbl;
  symbol->fnc_flt=fnc_flt;
  return symbol;
} /* end ncap_sym_init() */

var_sct * /* O [sct] Remainder of modulo operation of input variables (var1%var2) */
ncap_var_var_mod /* [fnc] Remainder (modulo) operation of two variables */
(var_sct *var1, /* I [sc,t] Variable structure containing field */
 var_sct *var2) /* I [sct] Variable structure containing divisor */
{
  ptr_unn op_swp;

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

var_sct *        /* O [sct] Resultant variable (actually is var) */
ncap_var_abs(    /* Purpose: Find absolute value of each element of var */
var_sct *var)    /* I/O [sct] input variable */
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
  /* swap names about so attribute propagation works */
  /* most operations unlike this one put results in left operand */
  if( !ncap_var_is_att(var1) && isalpha(var1->nm[0])) {
    swp_nm=var1->nm; var1->nm=var2->nm; var2->nm=swp_nm;
  }  

  if(var1->undefined){ 
    var2->undefined=True;
    var1=nco_var_free(var1);
    return var2;
  }

  /* make sure vars are at least float  */
  if(var1->type < NC_FLOAT && var2->type <NC_FLOAT ) 
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
ncap_var_fnc(   
var_sct *var_in,    /* I/O [sng] input variable */ 
sym_sct *app)       /* I [fnc_ptr] to apply to variable */
{
  /* Purpose: Evaluate fnc_dbl(var) or fnc_flt(var) for each value in variable
     Float and double functions are in app */
  long idx;
  long sz;
  ptr_unn op1;

  if(var_in->undefined) return var_in;
  
  
  /* Promote variable to NC_FLOAT */
  if(var_in->type < NC_FLOAT) var_in=nco_var_cnf_typ( NC_FLOAT,var_in);

  /* deal with initial scan */
  if(var_in->val.vp==NULL) return var_in; 
  
  op1=var_in->val;
  sz=var_in->sz;
  (void)cast_void_nctype(var_in->type,&op1);
  if(var_in->has_mss_val) (void)cast_void_nctype(var_in->type,&(var_in->mss_val));
  
  switch(var_in->type){ 
  case NC_DOUBLE: {
    if(!var_in->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.dp[idx]=(*(app->fnc_dbl))(op1.dp[idx]);
    }else{
      double mss_val_dbl=*(var_in->mss_val.dp); /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op1.dp[idx] != mss_val_dbl) op1.dp[idx]=(*(app->fnc_dbl))(op1.dp[idx]);
      } /* end for */
    } /* end else */
    break;
  }
  case NC_FLOAT: {
    if(!var_in->has_mss_val){
      for(idx=0;idx<sz;idx++) op1.fp[idx]=(*(app->fnc_flt))(op1.fp[idx]);
    }else{
      float mss_val_flt=*(var_in->mss_val.fp); /* Temporary variable reduces de-referencing */
      for(idx=0;idx<sz;idx++){
        if(op1.fp[idx] != mss_val_flt) op1.fp[idx]=(*(app->fnc_flt))(op1.fp[idx]);
      } /* end for */
    } /* end else */
    break;
  }
  default: nco_dfl_case_nc_type_err(); break;
  }/* end switch */
  


  if(var_in->has_mss_val) (void)cast_nctype_void(var_in->type,&(var_in->mss_val));
  return var_in;
} /* end ncap_var_fnc() */

nm_id_sct *            /* O [sct] new copy of xtr_lst */
nco_var_lst_copy(      /*   [fnc] Purpose: Copy xtr_lst and return new list */
nm_id_sct *xtr_lst,    /* I  [sct] input list */ 
int lst_nbr)           /* I  [nbr] number of elements in list */
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
nco_var_lst_sub(
nm_id_sct *xtr_lst,     /* I [sct] input list */   
int *nbr_xtr,           /* I/O [ptr] size of xtr_lst and new list */
nm_id_sct *xtr_lst_b,   /* I [sct] list to be subtracted */   
int nbr_lst_b)          /* I [nbr] size eof xtr_lst_b */ 
{
  /* Purpose: Subtract from xtr_lst any elements from xtr_lst_b which are present and return new list */
  int idx;
  int xtr_idx;
  int xtr_nbr_new=0;
  
  nco_bool match;
  
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
  /* realloc to actual size */
  xtr_new_lst=(nm_id_sct*)nco_realloc(xtr_new_lst,xtr_nbr_new*sizeof(nm_id_sct)); 
  /* free old list */
  xtr_lst=nco_nm_id_lst_free(xtr_lst,*nbr_xtr);

  *nbr_xtr=xtr_nbr_new;
  return xtr_new_lst;     
}/* end nco_var_lst_sub */

nm_id_sct *            /* O [sct] -- new list */
nco_var_lst_add(
nm_id_sct *xtr_lst,    /* I [sct] input list */ 
int *nbr_xtr,          /* I/O [ptr] -- size of xtr_lst & new output list */ 
nm_id_sct *xtr_lst_a,  /* I [sct] list of elemenst to be added to new list */
int nbr_lst_a)         /* I [nbr] size of xtr_lst_a */
{
  /* Purpose: Add to xtr_lst any elements from xtr_lst_a not already present and return new list */
  int idx;
  int xtr_idx;
  int nbr_xtr_crr;
  
  nm_id_sct *xtr_new_lst;
  
  nco_bool match;
  
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
      xtr_lst[size++].nm=(char *)strdup(var_nm);
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
       //(void)fprintf(stdout,"%s: ERROR %s and template %s share no dimensions\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
       //nco_exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() >= 1) 
          wrn_prn(fnc_nm,std::string(var_lsr->nm)+ " and " +std::string(var_gtr->nm) +" share no dimensions. Attempting to convolve..."); 
	    //(void)fprintf(stdout,"\n%s: DEBUG %s and %s share no dimensions: Attempting to convolve...\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
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

	  //(void)fprintf(stdout,"%s: ERROR %d dimensions of %s belong to template %s but %d dimensions do not\n",prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr);
	  //nco_exit(EXIT_FAILURE);
	}else{
	  if(dbg_lvl_get() >= 1){ 
	  std::ostringstream os;
          os<<var_lsr_var_gtr_dmn_shr_nbr << " dimensions of " << var_lsr->nm << " belong to template ";
          os<<var_gtr->nm << " but " << (var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr) <<" do not:";
          os<<"Not broadcasting " << var_lsr->nm<< "to " <<var_gtr->nm;
          os<<"could attempt stretching???";
          wrn_prn(fnc_nm,os.str());
          }

	    //(void)fprintf(stdout,"\n%s: DEBUG %d dimensions of %s belong to template %s but %d dimensions do not: Not broadcasting %s to %s, could attempt stretching???\n",prg_nm_get(),var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm,var_lsr->nbr_dim-var_lsr_var_gtr_dmn_shr_nbr,var_lsr->nm,var_gtr->nm);
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
    if(dbg_lvl_get() >= 1) 
      dbg_prn(fnc_nm,"Convolution not yet implemented, results of operation between " +std::string(var_lsr->nm) 
              + " and "+std::string(var_gtr->nm) + "are unpredictable");
    //(void)fprintf(stdout,"\n%s: WARNING Convolution not yet implemented, results of operation between %s and %s are unpredictable\n",prg_nm_get(),var_lsr->nm,var_gtr->nm);
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
          
	    //(void)fprintf(stdout,"%s: ERROR var_lsr %s has dimension %s but var_gtr %s does not deep in ncap_var_stretch()\n",prg_nm_get(),var_lsr->nm,var_lsr->dim[idx]->nm,var_gtr->nm);
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
const char *dmn_nm,
long sz,
prs_sct *prs_arg){
  const char fnc_nm[]="ncap_def_dim"; 

  int idx;
  int len;
  char ch; 
          
  dmn_sct *dmn_nw;             
  dmn_sct *dmn_in_e;
  dmn_sct *dmn_out_e;
  
  
  
  len=strlen(dmn_nm);
  // Ckeck for a valid name 
  for(idx=0 ; idx < len ; idx++){ 
    ch=dmn_nm[idx];
    if( !( isalpha(ch) || ch=='.' || ch=='_' ||ch=='-') ){ 
      wrn_prn(fnc_nm,"dim \""+ std::string(dmn_nm) + "\" - Invalid dimension name.");
      return False;;
    }
  }         
  // Check if dimension already exists
  dmn_in_e=prs_arg->ptr_dmn_in_vtr->find(dmn_nm);
  dmn_out_e=prs_arg->ptr_dmn_out_vtr->find(dmn_nm);

  if(dmn_in_e !=NULL_CEWI || dmn_out_e !=NULL_CEWI  ){ 
     wrn_prn(fnc_nm,"dim \""+ std::string(dmn_nm) + "\" - already exists in input/output."); 
     return False;
  }

  if( sz < 0  ){
    std::ostringstream os;
    os<<"dim " << dmn_nm << "(size=" <<sz <<") dimension can't be negative.";
    wrn_prn(fnc_nm,os.str()); 
    return False;
  }
  
  dmn_nw=(dmn_sct *)nco_malloc(sizeof(dmn_sct));
  
  dmn_nw->nm=(char *)strdup(dmn_nm);
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
  (void)prs_arg->ptr_dmn_out_vtr->push_back(dmn_nw); 
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
 
// this defines an anonymous enum containing parser tokens
#undef __cplusplus
#include "ncoParserTokenTypes.hpp"
#define __cplusplus

#include "VarOp.hh" 

var_sct *
ncap_var_var_stc
(var_sct *var1,
 var_sct *var2,
 int op)
{

  static VarOp<short> Vs;
  static VarOp<nco_int> Vl;
  static VarOp<float> Vf;
  static VarOp<double> Vd;
  
  var_sct *var_ret=NULL_CEWI;


  //If var2 is null then we are dealing with a unary function
  if( var2 == NULL_CEWI) {

   switch (var1->type) {
    case NC_BYTE:
    /* Do nothing */
      break;
    case NC_CHAR:
    /* Do nothing */
      break;
    case NC_SHORT:
      var_ret=Vs.var_op(var1,op);
      break;
    case NC_INT:
      var_ret=Vl.var_op(var1,op);
      break;            
    case NC_FLOAT:
      var_ret=Vf.var_op(var1,op);
      break;
  case NC_DOUBLE:
      var_ret=Vd.var_op(var1,op);
      break;

  default:
    break;

   }
   return var_ret;
  }

  switch (var1->type) {
    case NC_BYTE:
    /* Do nothing */
      break;
    case NC_CHAR:
    /* Do nothing */
      break;
    case NC_SHORT:
      var_ret=Vs.var_var_op(var1, var2,op);
      break;
    case NC_INT:
      var_ret=Vl.var_var_op(var1, var2,op);
      break;            
    case NC_FLOAT:
      var_ret=Vf.var_var_op(var1, var2,op);
      break;
  case NC_DOUBLE:
      var_ret=Vd.var_var_op(var1, var2,op);
      break;

  default:
    break;

  } 

  return var_ret;

}

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

  // Deal with pwr fuction
  if(op== CARET && var1->type < NC_FLOAT && var2->type <NC_FLOAT ) 
    var1=nco_var_cnf_typ((nc_type)NC_FLOAT,var1);
   
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
	os<<"Hyperslabbed variable:"<<var1->nm <<" and variable:"<<var2->nm <<"have differnet number of elements,so connot perform arithmetic operation.";
	err_prn(fnc_nm,os.str());
         
      }
      if( nco_shp_chk(var1,var2)==False){ 
	std::ostringstream os;
        os<<"Hyperslabbed variable:"<<var1->nm <<" and variable:"<<var2->nm <<"have same  number of elements, but different shapes.";
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
      os<<"Cannot make variable:"<<var1->nm <<" and attribute:"<<var2->nm <<" conform. So connot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
      
       
    // att & var
    }else if( vb1 && !vb2){
      var_sct *var_swp;
      ptr_unn val_swp;  // Used to swap values around

      var1=nco_var_cnf_typ(var2->type,var1);
     if(var2->sz > 1 && var1->sz==1)
      (void)ncap_var_cnf_dmn(&var1,&var2);
      
     if(var1->sz != var2->sz) {
      std::ostringstream os;
      os<<"Cannot make attribute:"<<var1->nm <<" and variable:"<<var2->nm <<" conform. So connot perform arithmetic operation.";
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
        os<<"Cannot make attribute:"<<var1->nm <<" and attribute:"<<var2->nm <<" conform. So connot perform arithmetic operation.";
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

  //If var2 is null then we are dealing with a unary function
  if( var2 == NULL_CEWI)
    return var1;
  


  // deal with pwr fuction
  if(op== CARET && var1->type < NC_FLOAT && var2->type <NC_FLOAT ) 
    var1=nco_var_cnf_typ((nc_type)NC_FLOAT,var1);
   
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
 prs_sct *prs_arg)
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
      (void)ncap_var_write(var1,bram,prs_arg);  
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
      (void)ncap_var_write(var1,bram,prs_arg);  
    }else{
      std::string sa(var1->nm);
      NcapVar *Nvar=new NcapVar(var1,sa);
      prs_arg->ptr_var_vtr->push_ow(Nvar);       
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
      os<<"Cannot make variable:"<<var1->nm <<" and variable:"<<var2->nm <<" conform. So connot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
    }
    // att & var ,att & att  
  } else {
    
    if(var1->sz > 1 && var2->sz==1)
      (void)ncap_att_stretch(var2,var1->sz);
  }   


  if(var1->sz != var2->sz) {
      std::ostringstream os;
      os<<"Cannot make " << cvar1<<":"<<var1->nm <<" and " <<cvar2 <<":"<<var2->nm <<" conform. So connot perform arithmetic operation.";
      err_prn(fnc_nm,os.str()); 
  }

  var1=ncap_var_var_stc(var1,var2,op);
  
  var_ret=nco_var_dpl(var1);
   
  // if LHS is a variable then write to disk
  if(!vb1){
    ncap_var_write(var1,bram,prs_arg);
  }else{
    // deal with attribute
   std::string sa(var1->nm);
   NcapVar *Nvar=new NcapVar(var1,sa);
   prs_arg->ptr_var_vtr->push_ow(Nvar);       

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
      const short mss_val_sht=*(var->mss_val.sp);
      for(idx=0;idx<sz;idx++) 
	if( !op1.sp[idx] &&  op1.sp[idx] !=mss_val_sht ) break; 
    }
    
  if(idx <sz) bret=false;

  if(var->has_mss_val) (void)cast_nctype_void(type,&var->mss_val);

  return bret;
}




var_sct*                           /* O [sct] casting variable has its own private dims */ 
ncap_cst_mk(                       /* [fnc] create casting var from a list of dims */
std::vector<std::string> &str_vtr,  /* I [sng] list of dimension subscripts */
prs_sct *prs_arg)
{
  const char fnc_nm[]="ncap_cst_mk"; 
  static const char * const tpl_nm="Internally generated template";
  
  

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
  (void)nco_redef(prs_arg->out_id);
  for(idx=0;idx<dmn_nbr;idx++){
    lst_nm=str_vtr[idx].c_str();
    // Search dmn_out_vtr for dimension
    dmn_item=prs_arg->ptr_dmn_out_vtr->find(lst_nm);
    if(dmn_item != NULL){ 
      dmn[idx]=dmn_item;
      continue;
    }
    // Search dmn_in_vtr for dimension
    dmn_item=prs_arg->ptr_dmn_in_vtr->find(lst_nm);
    // die if not in list
    if(dmn_item == NULL_CEWI) {
      err_prn(fnc_nm,"Unrecognized dimension \""+std::string(lst_nm)+ "\"in LHS subscripts");
    }  
    dmn_new=nco_dmn_dpl(dmn_item);
    // Define in output file 
    (void)nco_dmn_dfn(prs_arg->fl_out,prs_arg->out_id,&dmn_new,1);
    // add to out list
    (void)prs_arg->ptr_dmn_out_vtr->push_back(dmn_new);
    (void)nco_dmn_xrf(dmn_new,dmn_item);
    dmn[idx]=dmn_new;
  }
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
  var->type=NC_DOUBLE;
  var->nbr_dim=dmn_nbr;
  var->dim=dmn;
  /* Allocate space for dimension structures */
  if(var->nbr_dim > 0) var->dmn_id=(int *)nco_malloc(var->nbr_dim*sizeof(int)); else var->dmn_id=(int *)NULL;
  if(var->nbr_dim > 0) var->cnt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->cnt=(long *)NULL;
  if(var->nbr_dim > 0) var->srt=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srt=(long *)NULL;
  if(var->nbr_dim > 0) var->end=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->end=(long *)NULL;
  if(var->nbr_dim > 0) var->srd=(long *)nco_malloc(var->nbr_dim*sizeof(long)); else var->srd=(long *)NULL;
  
  /* Defensive programming */
  var->sz=1L; 
  /* Attach LHS dimensions to variable */
  for(idx=0;idx<dmn_nbr;idx++){
    var->dim[idx]=dmn[idx];
    var->dmn_id[idx]=dmn[idx]->id;
    var->cnt[idx]=dmn[idx]->cnt;
    var->srt[idx]=dmn[idx]->srt;
    var->end[idx]=dmn[idx]->end;
    var->srd[idx]=dmn[idx]->srd;
    var->sz*=var->cnt[idx];
  } /* end loop over dim */

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

   
  if(dbg_lvl_get() > 2) {
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
  
   if(dbg_lvl_get() > 2){
     std::ostringstream os;
    os<<"Stretching variable "<<var->nm << "with LHS template var->nm "<<var_cst->nm <<"var->nbr_dim " <<var_cst->nbr_dim; 
    os<<" var->sz " <<var_cst->sz;
    wrn_prn(fnc_nm,os.str());
   }
    
   var->undefined=False;
  }

return var;

}

// returns true if string is only made of 0..9 chars
bool
str_is_num(
std::string snm)
{ 
 unsigned idx;
 for(idx=0 ; idx < snm.size(); idx++)
   if( !isdigit(snm[idx])) break;

 return ( idx==snm.size() ? true: false);
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
  // deal with position args e.g $0,$1,$2 etc
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

/* Create a scalar variable of type, if bfll then malloc ptr_unn */
var_sct*
ncap_sclr_var_mk(
char* var_nm,
nc_type type,
bool bfll)
{
  var_sct *var;
  var=(var_sct *)nco_malloc(sizeof(var_sct));
  /* Set defaults */
  (void)var_dfl_set(var); 
  /* Overwrite with variable expression information */
  var->nm=strdup(var_nm);
  var->nbr_dim=0;
  var->sz=1;
  var->type=type;
  var->typ_dsk=type;
  
  if(bfll)
    var->val=nco_mss_val_mk(type);  
   
  return var;
}

var_sct *
ncap_sclr_var_mk(
char *var_nm,
unsigned char cdt)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,NC_CHAR,true);
  (void)cast_void_nctype(NC_CHAR,&var->val);
  *var->val.cp=cdt;
  (void)cast_nctype_void(NC_CHAR,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
char *var_nm,
signed char bdt)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,NC_BYTE,true);
  (void)cast_void_nctype(NC_BYTE,&var->val);
  *var->val.bp=bdt;
  (void)cast_nctype_void(NC_BYTE,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
char *var_nm,
short sdt)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,NC_SHORT,true);
  (void)cast_void_nctype(NC_SHORT,&var->val);
  *var->val.sp=sdt;
  (void)cast_nctype_void(NC_SHORT,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
char *var_nm,
int idt)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,NC_INT,true);
  (void)cast_void_nctype(NC_INT,&var->val);
  *var->val.lp=idt;
  (void)cast_nctype_void(NC_INT,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
char *var_nm,
long ldt)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,NC_INT,true);
  (void)cast_void_nctype(NC_INT,&var->val);
  *var->val.lp=ldt;
  (void)cast_nctype_void(NC_INT,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
char *var_nm,
float fdt)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,NC_FLOAT,true);
  (void)cast_void_nctype(NC_FLOAT,&var->val);
  *var->val.fp=fdt;
  (void)cast_nctype_void(NC_FLOAT,&var->val);
  return var;
}

var_sct *
ncap_sclr_var_mk(
char *var_nm,
double ddt)
{
  var_sct *var;
  var=ncap_sclr_var_mk(var_nm,NC_DOUBLE,true);
  (void)cast_void_nctype(NC_DOUBLE,&var->val);
  *var->val.dp=ddt;
  (void)cast_nctype_void(NC_DOUBLE,&var->val);
  return var;
}

// define variables captured on first parse
void ncap_def_ntl_scn(prs_sct *prs_arg)
{
int idx;
int sz;
int var_id;
NcapVar *Nvar;
NcapVar *Cvar;
var_sct *var1;

const std::string fnc_nm("ncap_def_ntl_scn"); 

sz=prs_arg->ptr_int_vtr->size();


 for(idx=0; idx < sz ; idx++){
   // de-reference
   Nvar=(*prs_arg->ptr_int_vtr)[idx];
   var1=Nvar->var;
   if( !Nvar->flg_udf && Nvar->xpr_typ==ncap_var) {
               
     if(dbg_lvl_get() > 0)
       dbg_prn(fnc_nm, Nvar->getFll()+ ( !Nvar->flg_mem ? " - defined in output.": " - RAM variable") );

     // define variable
     if(!Nvar->flg_mem) {
       (void)nco_def_var(prs_arg->out_id,var1->nm,var1->type,var1->nbr_dim,var1->dmn_id,&var_id);
       Nvar->var->id=var_id;
       Nvar->var->nc_id=prs_arg->out_id;
       Nvar->flg_stt=1;
     } else { 
       //deal with RAM only var        
       Nvar->var->id=-1;
       Nvar->var->nc_id=-1;
       Nvar->flg_stt=1;
     }
     // save newly defined var in output vector

     Cvar=new NcapVar(*Nvar);
     prs_arg->ptr_var_vtr->push(Cvar);
      
   } 
   delete Nvar;  
 }
 
 // empty int_vtr
  for(idx=0 ; idx <sz ; idx++)
   (void)prs_arg->ptr_int_vtr->pop();

}

// Do an in-memory hyperslab !!
void 
ncap_get_var_mem( 
int dpt,                       // Current depth
int dpt_max,                   // Max depth ( same as number of dims) 
std::vector<int> &shp_vtr,     // shape of input var (in bytes)
NcapVector<dmn_sct*> &dmn_vtr, // New vectors
char *cp_in,                   // Pointer to (char*)var_in->val.vp
char *&cp_out){                // Reference pointer to space for new var values

const std::string fnc_nm("ncap_get_var_mem"); 


long idx;      
long srt=dmn_vtr[dpt]->srt;
long end=dmn_vtr[dpt]->end;
long cnt=dmn_vtr[dpt]->cnt;
long srd=dmn_vtr[dpt]->srd;
long slb_sz=shp_vtr[dpt];

char *cp_srt=cp_in+ptrdiff_t(srt*slb_sz);
char *cp_end=cp_out;


  if(dbg_lvl_get() > 2){
      std::ostringstream os;
      os<<"Depth=" << dpt<<" "<<dmn_vtr[dpt]->nm<<" "<<srt<<" "<<end<<" "<<cnt<<" "<<srd;
      dbg_prn(fnc_nm,os.str());
  }


  if(dpt == dpt_max){
    

    if(srd==1) 
      (void)memcpy(cp_end, cp_srt, ptrdiff_t(cnt*shp_vtr[dpt]));
    else { 
     
      for(idx=0 ; idx<cnt ; idx++ ){
        (void)memcpy(cp_end,cp_srt,slb_sz);
        cp_end+=slb_sz;
        cp_srt+=(ptrdiff_t)(srd*slb_sz);
      }
    } // end else
    // increment output pointer (n.b space already alloc-ed)
   cp_out+=ptrdiff_t(cnt*slb_sz);
   
  }

  if(dpt < dpt_max ){
     for(idx=0; idx <cnt ;idx++){
       (void)ncap_get_var_mem(dpt+1,dpt_max,shp_vtr,dmn_vtr,cp_srt,cp_out);
       cp_srt+= ptrdiff_t(srd*slb_sz);
     }
  }  
} /* ncap_get_var_mem */

 
var_sct*
nco_get_var_mem(
var_sct *var_in,
NcapVector<dmn_sct*> &dmn_vtr){

int idx;
int ncnt;
int dmn_nbr;
 int dpt_max;

void  *vp_out;
char *cp_out; 
std::vector<int> shp_vtr;

var_sct* var_ret;  

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

 // Alloc space for output variables value
 vp_out=(void*)nco_malloc(ncnt*nco_typ_lng(var_in->type));

 cp_out=(char*)vp_out;

 // Work out max depth we have to go to 
 dpt_max=dmn_nbr;
 for(idx=dmn_nbr-1; idx>0 ; idx--)
   if( var_in->dim[idx]->cnt == dmn_vtr[idx]->cnt) 
     dpt_max--;
   else
     break;
 
 // Call in-memory nco_get_var() (n.b is recursive of course!!)
 (void)ncap_get_var_mem(0,dpt_max-1,shp_vtr,dmn_vtr,(char*)var_in->val.vp,cp_out);
 
 var_ret=nco_var_dpl(var_in);
 var_ret->sz=ncnt;

 (void)nco_free(var_ret->val.vp);
 var_ret->val.vp=vp_out;  

  return var_ret;
} /* end nco_get_var_mem()  */




void
ncap_put_var_mem(
int dpt,                       // Current depth
int dpt_max,                   // Max depth ( same as number of dims) 
std::vector<int> &shp_vtr,     // shape of input var (in bytes)
NcapVector<lmt_sct*> &dmn_vtr, // New vectors
char *cp_out,                  // Pointer to (char*)var_in->val.vp
char *&cp_in)                  // Slab to be "put" 
{


const std::string fnc_nm("ncap_put_var_mem"); 


long idx;      
long srt=dmn_vtr[dpt]->srt;
long end=dmn_vtr[dpt]->end;
long cnt=dmn_vtr[dpt]->cnt;
long srd=dmn_vtr[dpt]->srd;
long slb_sz=shp_vtr[dpt];

char *cp_srt=cp_in;
char *cp_end=cp_out+ptrdiff_t(srt*slb_sz);;


  if(dbg_lvl_get() > 2){
      std::ostringstream os;
      os<<"Depth=" << dpt<<" "<<dmn_vtr[dpt]->nm<<" "<<srt<<" "<<end<<" "<<cnt<<" "<<srd;
      dbg_prn(fnc_nm,os.str());
  }


  if(dpt == dpt_max){
    

    if(srd==1) 
      (void)memcpy(cp_end, cp_srt, ptrdiff_t(cnt*shp_vtr[dpt]));
    else { 
     
      for(idx=0 ; idx<cnt ; idx++ ){
        (void)memcpy(cp_end,cp_srt,slb_sz);
        cp_srt+=slb_sz;
        cp_end+=(ptrdiff_t)(srd*slb_sz);
      }
    } // end else
    // increment input pointer 
   cp_in+=ptrdiff_t(cnt*slb_sz);
   
  }

  if(dpt < dpt_max){
     for(idx=0; idx <cnt ;idx++){
       (void)ncap_put_var_mem(dpt+1,dpt_max,shp_vtr,dmn_vtr,cp_end,cp_in);
       cp_srt+= ptrdiff_t(srd*slb_sz);
     }
  }  

}



// Do an in memory nco_put_var()  
void
nco_put_var_mem(
var_sct *var_in,
var_sct *var_nw,
NcapVector<lmt_sct*> &dmn_vtr)
{

int idx;
int ncnt;
int dmn_nbr;
int dpt_max;

char *cp_out; 
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

 // Work out max depth we have to go to 
 dpt_max=dmn_nbr;
 for(idx=dmn_nbr-1; idx>0 ; idx--)
   if( var_in->dim[idx]->cnt == dmn_vtr[idx]->cnt) 
     dpt_max--;
   else
     break;

 cp_out=(char*)var_nw->val.vp;


 // Call in-memory nco_put_var_mem (n.b is recursive of course!!)
 (void)ncap_put_var_mem(0,dpt_max-1,shp_vtr,dmn_vtr,(char*)var_in->val.vp,cp_out);


} /* end nco_put_var_mem() */







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
std::vector<exp_sct_tmp**> &srp_vtr //self reverential pointer
)
{
  unsigned int idx;

  if(srp_vtr.empty()) 
    return true;

  for(idx=0 ; idx < srp_vtr.size() ; idx++)
    if( *srp_vtr[idx] != NULL_CEWI) 
      return false;

  return true;


}

// Sort expressions for MPI optimization
//ntr is a pointer to nbr_lst statements of type EXPR
int ncap_mpi_srt(
RefAST ntr,   
int nbr_lst,
std::vector< std::vector<RefAST> > &all_ast_vtr)
{
const std::string fnc_nm("ncap_mpi_srt"); 
  int idx;
  int jdx;
  RefAST tr;

  exp_sct *exp_ptr;

  std::vector<exp_sct*> exp_vtr;


 // populate exp_vtr;
    if(dbg_lvl_get() > 0)
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
         std::vector<std::string>::iterator we=std::unique(cl_vtr.begin(), cl_vtr.end());
         if(we < cl_vtr.end())  
           cl_vtr.erase(we,cl_vtr.end()); 

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
        std::vector<std::string> out_lvl_vtr=exp_ptr->lvl_vtr; 	
        // expression without Lvalues
        if(out_lvl_vtr.empty()) 
          continue;
      
	//Inner loop
        for(jdx=(idx+1); jdx< nbr_lst ; jdx++){
          // See if inner-loop exprssion depends
          // on outer-loop expression 
	  if( ncap_mpi_srh_str(exp_ptr->etr, exp_vtr[jdx]->lvl_vtr) ||
              ncap_mpi_srh_str(exp_vtr[jdx]->etr,out_lvl_vtr)) {
 
	    exp_vtr[jdx]->dpd_vtr.push_back(idx);
            exp_vtr[jdx]->srp_vtr.push_back( &exp_vtr[idx]);
            // subtract Lvalues as we go
            (bool)ncap_sub_str_str(out_lvl_vtr, exp_vtr[jdx]->lvl_vtr);
	  }
	  // break out of inner loop if out_lvl_vtr empty
	  if(out_lvl_vtr.empty())
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
        std::vector<RefAST> ast_vtr;
	std::vector<int> grp_vtr;
       
        for(idx=0; idx <nbr_lst; idx++){
          if( exp_vtr[idx]==NULL) 
            continue;
          if( ncap_evl_srp(exp_vtr[idx]->srp_vtr))
	    grp_vtr.push_back(idx);
	}// end idx

        for(idx=0 ; idx <(int)grp_vtr.size(); idx++){
 	  ast_vtr.push_back( exp_vtr[grp_vtr[idx]]->etr);
          delete exp_vtr[grp_vtr[idx]];
          exp_vtr[grp_vtr[idx]]=NULL;
        } //end idx

        // Save vector in another vector !! 
        all_ast_vtr.push_back(ast_vtr);
        icnt+=(int)grp_vtr.size();
          

      } // end while

      //Print out vectors
      if(dbg_lvl_get() >0) {
        for(idx=0 ; idx<(int)all_ast_vtr.size(); idx++){
          for(jdx=0 ; jdx<(int)all_ast_vtr[idx].size(); jdx++)
	    std::cout << all_ast_vtr[idx][jdx]->toStringTree()<<std::endl;
	  std::cout <<"-------------------------------\n";
          } //end idx
      }

    if(dbg_lvl_get() > 0)
      dbg_prn(fnc_nm,"End");       


    return 0;

}


