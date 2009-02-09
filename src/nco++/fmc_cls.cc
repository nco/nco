// function/method class  /****************************************/

#include "fmc_cls.hh"
  
//constructors
  fmc_cls::fmc_cls(std::string cfnm,vtl_cls *cvtl_ref,int cfdx){
    _fnm=cfnm;
    _vtl_obj=cvtl_ref;
    _fdx=cfdx; 
  }

  fmc_cls::fmc_cls(const char *const pfnm,vtl_cls *cvtl_ref,int cfdx){
    _fnm=static_cast<std::string>(pfnm);
    _vtl_obj=cvtl_ref;
    _fdx=cfdx; 
  }


  fmc_cls::fmc_cls(std::string cfnm){
    _fnm=cfnm;
    _vtl_obj=NULL;
    _fdx=0; 
  }
