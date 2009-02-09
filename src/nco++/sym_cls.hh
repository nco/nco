// maths functions holder (for mth_cls) ********************/

// Standard C++ headers
#ifndef SYM_CLS_HH
#define SYM_CLS_HH


class sym_cls{ /* sym_sct */
 public:
  std::string _fnm;
  double (*_fnc_dbl)(double); /* [fnc] Double-valued function */
  float (*_fnc_flt)(float);   /* [fnc] Float-valued function */
 public:
  sym_cls(std::string sfnm,  double (*fnc_dbl)(double), float (*fnc_flt)(float)); 
  sym_cls(const char *const pfnm,  double (*fnc_dbl)(double), float (*fnc_flt)(float));
  std::string fnm();
}; 


#endif
