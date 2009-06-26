#include "NcapVar.hh"


  NcapVar::NcapVar(var_sct *var_in, std::string sin) {
    size_t  att_char_posn;
    if(sin=="") sin=std::string(var_in->nm);

    var=var_in;
    fll_nm=sin;  
    //flg_mem=_flg_mem;  

    rfr_ast=ANTLR_USE_NAMESPACE(antlr)nullAST; 

    if(var !=(var_sct*)NULL) 
      flg_udf=(var_in->undefined==True);
    else
      flg_udf=False;
 
    flg_mem=false;
    flg_stt=0;
    flg_spl=false;
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
    if( var!=(var_sct*)NULL && fll_nm != std::string(var->nm) ){ 
    var->nm=(char*)nco_free(var->nm);
    var->nm=strdup(fll_nm.c_str()); 
    }
  }


