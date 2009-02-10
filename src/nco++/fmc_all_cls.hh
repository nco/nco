/* Purpose: netCDF arithmetic processor class methods */

/* Copyright (C) 1995--2007 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

// Standard C++ headers
#ifndef FMC_ALL_CLS_HH
#define FMC_ALL_CLS_HH

#include <math.h>

#include <string>
#include <vector>

#include <antlr/AST.hpp>

#include "ncoTree.hpp"
#include "ncap2_utl.hh"
#include "vtl_cls.hh"

#include "sym_cls.hh" // holder for float/double math function pointers



  /* Math float prototypes required by AIX, Solaris, but not by Linux, IRIX */
  /* Basic math: acos, asin, atan, cos, exp, fabs, log, log10, sin, sqrt, tan */
  
  /* GNU g++ barfs at these float declartions -- remove if g++ used */
#ifndef __GNUG__
  extern float acosf(float);
  extern float asinf(float);
  extern float atanf(float);
  extern float cosf(float);
  extern float expf(float);
  extern float fabsf(float); /* 20040629: Only AIX may need this */
  extern float logf(float);
  extern float log10f(float);
  extern float rnd_nbrf(float);
  extern float sinf(float);
  extern float sqrtf(float);
  extern float tanf(float);
  
  /* Advanced math: erf, erfc, gamma */
  extern float erff(float);
  extern float erfcf(float);
  extern float gammaf(float);

  /* Hyperbolic trigonometric: acosh, asinh, atanh, cosh, sinh, tanh */
  extern float acoshf(float);
  extern float asinhf(float);
  extern float atanhf(float);
  extern float coshf(float);
  extern float sinhf(float);
  extern float tanhf(float);
  
  /* Basic Rounding: ceil, floor */
  extern float ceilf(float);
  extern float floorf(float);
  
  /* Advanced Rounding: nearbyint, rint, round, trunc */
  extern float nearbyintf(float);
  extern float rintf(float);
  extern float roundf(float);
  extern float truncf(float);
#endif




//Conversion Functions **************************************/
class cnv_cls: public vtl_cls {
private:
  bool _flg_dbg;
public:
    cnv_cls(bool flg_dbg);
    var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};


//Aggregate Functions /***************************************/
class agg_cls: public vtl_cls {
private:
  enum{ PAVG ,PAVGSQR ,PMAX ,PMIN ,PRMS,
	PRMSSDN, PSQRAVG, PTTL};
  bool _flg_dbg;
public:
  agg_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
};


//Utility Functions /****************************************/
class utl_cls: public vtl_cls {
private:
   enum {SET_MISS,CH_MISS,DEL_MISS,GET_MISS,RAM_WRITE,RAM_DELETE};
   bool _flg_dbg;
public:
  utl_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};



//Basic Functions /****************************************/
class bsc_cls: public vtl_cls {
private:
   enum {PSIZE,PTYPE,PNDIMS };
   bool _flg_dbg;
public:
  bsc_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};





//Maths Functions /****************************************/
class mth_cls: public vtl_cls {
private:
   bool _flg_dbg;
  std::vector<sym_cls> sym_vtr;
public:
  mth_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
};


//Maths2 - Maths functions that take 2 args /*********/
class mth2_cls: public vtl_cls {
private:
   enum {PPOW,PATAN2,PCONVERT};
   bool _flg_dbg;
public:
  mth2_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
};


//PDQ Functions /****************************************/
class pdq_cls: public vtl_cls {
private: 
   enum {PREVERSE,PPERMUTE };
   bool _flg_dbg;
public:
  pdq_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};



//Mask Function /****************************************/
class msk_cls: public vtl_cls {
private:
   enum {PMASK1, PMASK_CLIP };
   bool _flg_dbg;
public:
  msk_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};

//Pack Function /****************************************/
class pck_cls: public vtl_cls {
private:
   enum {PPACK, PPACK_BYTE, PPACK_CHAR, PPACK_SHORT, PPACK_INT, PUNPACK,};
   bool _flg_dbg;
public:
  pck_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};

#endif // FMC_ALL_CLS_HH
