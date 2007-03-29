#ifndef NCAPVAR_HH
#define NCAPVAR_HH
#include <iostream>
#include <string>
#include "nco.h"
#include "Ncap2.hh"
#include "nco_var_utl.h"  // nco_var_free()
#include <antlr/AST.hpp>
#include <antlr/AST.hpp>

ANTLR_USING_NAMESPACE(antlr);

class NcapVar{

private:
  std::string var_nm;
  std::string att_nm;
  std::string fll_nm; // full name i.e var_nm@att_nm;
  RefAST  rfr_ast;
  ncap_type xpr_typ_rfr; 
public:
 var_sct *var;
 ncap_type xpr_typ;
 
  bool flg_udf;   // matches var->undefined


 bool flg_mem;    // true -- then var data is written to memory  
                  // rather than disk -- At the moment all meta-data
                  // is cached  ? --     
 int flg_stt;      // status flag
                   // 0 -- var is defined in memory
                   // 1 -- var is defined in output - but no data written
                   // 2 -- var is defined & data written

public:

  //constructors n.b var->nm should match var_nm@att_nm

  NcapVar(var_sct *var_in, std::string sin="") {
    size_t  att_char_posn;
    if(sin=="") sin=std::string(var_in->nm);

    var=var_in;
    fll_nm=sin;  
    //flg_mem=_flg_mem;  

    rfr_ast=ANTLR_USE_NAMESPACE(antlr)nullAST; 
    flg_udf=(var_in->undefined==True);
    flg_mem=false;
    flg_stt=0;
    xpr_typ_rfr=ncap_xpr_null;
    
        
    if( (att_char_posn =sin.find("@")) ==std::string::npos ){
      xpr_typ=ncap_var;
      var_nm=sin;
      att_nm="";
    }else{
      xpr_typ=ncap_att;
      var_nm=sin.substr(0,att_char_posn);
      att_nm=sin.substr(att_char_posn+1);
      
    }

    //make sure var->nm is the same as var_nm@att_nm 
    if( fll_nm != std::string(var->nm) ){ 
    var->nm=(char*)nco_free(var->nm);
    var->nm=strdup(fll_nm.c_str()); 
    }
  }
  /*
  NcapVar(var_sct *var_in) {
    std::string sin(var_in->nm);
    NcapVar(var_in,sin);
    
    } */


  //Copy Constructor
  NcapVar(NcapVar &Nvar){

   var_nm=Nvar.var_nm;;
   att_nm=Nvar.att_nm;
   fll_nm=Nvar.fll_nm; 
   rfr_ast=Nvar.rfr_ast;;

   xpr_typ_rfr=Nvar.xpr_typ_rfr; 

   var=nco_var_dpl(Nvar.var);
   xpr_typ=Nvar.xpr_typ;
 
   flg_udf=Nvar.flg_udf;
   flg_mem=Nvar.flg_mem;   
   flg_stt=Nvar.flg_stt;   
  } 
  

  //Copy variable type
  var_sct * cpyVar(){
    return nco_var_dpl(var);
  } 

  //Copy variable no data
  var_sct *cpyVarNoData(){
    void *vp;
    var_sct *var_ret;

    vp=var->val.vp;
    var->val.vp=(void*)NULL;
    var_ret=nco_var_dpl(var);
    
    // Restore Original Value
    var->val.vp=vp;

    return var_ret;
  }

  
  //Methods
  void setAST(RefAST &rAST) 
      { rfr_ast=rAST;} 
  void setAST(RefAST &rAST,ncap_type xpr_rtyp)
      { rfr_ast=rAST;
        xpr_typ_rfr=xpr_rtyp;
      } 
    

 
  RefAST getAst(){ return rfr_ast;}
  ncap_type getAst_typ() { return xpr_typ_rfr;}

  std::string getVar() {return var_nm; }
  std::string getAtt() {return att_nm; }
  std::string getFll() {return fll_nm; }

  //Destructor
  ~NcapVar() { if(var !=(var_sct*)NULL) var=nco_var_free(var); }


};


#endif // NCAPVAR_HH


