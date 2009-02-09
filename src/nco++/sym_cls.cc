// maths functions holder (for mth_cls) ********************/
#include <string>
#include "sym_cls.hh"


  sym_cls::sym_cls(std::string sfnm,  double (*fnc_dbl)(double), float (*fnc_flt)(float)){
    _fnm=sfnm;
    _fnc_dbl=fnc_dbl;
    _fnc_flt=fnc_flt; 
  } 
  sym_cls::sym_cls(const char *const pfnm,  double (*fnc_dbl)(double), float (*fnc_flt)(float)){
    _fnm= static_cast<std::string>(pfnm);
    _fnc_dbl=fnc_dbl;
    _fnc_flt=fnc_flt; 
  } 
  std::string sym_cls::fnm() { return _fnm;} 
