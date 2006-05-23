#ifndef NCAPVAR_HH
#define NCAPVAR_HH

#include <string>
#include "nco.h"
#include "Ncap2.hh"
#include "nco_var_utl.h"

class NcapVar{

private:
  std::string var_nm;
  std::string att_nm;
public:
 std::string s_va_nm; 
 var_sct *var;
 
 ncap_type type;

 int status;

public:
  NcapVar() {
    s_va_nm=""; 
    var=(var_sct*)NULL;
    type=ncap_undef;
  }

  NcapVar(std::string s_in_nm,var_sct *var_in) {
    size_t  att_char_posn;
    s_va_nm=s_in_nm;
    var=var_in;
    
    if( (att_char_posn =s_va_nm.find("@")) ==std::string::npos ){
      type=ncap_var;
      var_nm=s_va_nm;
      att_nm="";
    }else{
      type=ncap_att;
      var_nm=s_va_nm.substr(0,att_char_posn);
      att_nm=s_va_nm.substr(att_char_posn+1);
    }
    //var->nm && s_va_nm are the same

    if( var->nm) 
    var->nm=(char*)nco_free(var->nm);
    var->nm=strdup(s_va_nm.c_str()); 
    
  }
    
  NcapVar(std::string s_in_nm) {
    NcapVar(s_in_nm,(var_sct *)NULL);
  }

  ~NcapVar() { if(var !=(var_sct*)NULL) var=nco_var_free(var); }
 
  std::string getVar() {return var_nm; }
  std::string getAtt() {return att_nm; }

};

#endif // NCAPVAR_HH
