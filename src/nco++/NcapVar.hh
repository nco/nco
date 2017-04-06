#ifndef NCAPVAR_HH
#define NCAPVAR_HH
#include <iostream>
#include <string>
#include "nco.h"
#include "ncap2.hh"
#include "nco_var_utl.h"  // nco_var_free()
#include <antlr/AST.hpp>
#include <antlr/AST.hpp>

#ifdef ENABLE_GSL
#include <gsl/gsl_spline.h>
#endif

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


  bool flg_mem;   // true -- then var data is written to memory  
                  // rather than disk -- At the moment all meta-data
                  // is cached  ? --

                  // for an attribute flg_mem=true - means mark att as transient
                  // this means that it  is inherited or propgagated once then deleted
                  // This is requried so for example agg_cls() adds the attribute 'var_nm@cell_methods'
                  // we want this att be picked up on the LHS but NOT written to 'var_nm'
                  // for example   three_sum=three_dmn_var_dbl.sum();
                  // so only three_sum gets @cell_methods().

  int flg_stt;    // status flag
                  // 0 -- var is defined in memory
                  // 1 -- var is defined in output - but no data written
                  // 2 -- var is defined & data written

  bool flg_spl;   // true then var->val.vp is spline data 
                  // e.g., var->val.vp is gsl_spline*
                  // this needs to be freed up using
                  // the gsl spline library call   

public:

  //constructors n.b var->nm should match var_nm@att_nm

  NcapVar(var_sct *var_in, std::string sin="");

  // NcapVar(var_sct *var_in);

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
   flg_spl=Nvar.flg_spl;   
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
  ~NcapVar() { 

#ifdef ENABLE_GSL
    if(flg_spl && var->val.vp !=(void*)NULL ){ 
      gsl_spline_free( (gsl_spline*)var->val.vp); 
      var->val.vp=(void*)NULL;      
    }
#endif


    if(var !=(var_sct*)NULL) var=nco_var_free(var); 


    }


};


#endif // NCAPVAR_HH


