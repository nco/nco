/* Purpose: netCDF arithmetic processor class methods */

/* Copyright (C) 1995--2007 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

// Standard C++ headers
#ifndef INC_FMC_CLS_hh_
#define INC_FMC_CLS_hh_

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <antlr/AST.hpp>
#include "ncoTree.hpp"

//forward declarations
class vtl_cls;
class ncoTree;

//local enum used to classify expressions 
enum vtl_typ { VVAR,VLVAR,VDVAR, VATT,VLATT,VDATT,VDIM, VDIM_SIZE,VEXPR };

// function/method class  ****************************************/

class fmc_cls {
public:
  std::string _fnm;
  vtl_cls *_vtl_obj;
  int _fdx;
public:
  // Constructors
  fmc_cls(std::string cfnm,vtl_cls *cvtl_ref,int cfdx);
  fmc_cls(const char *const pfnm, vtl_cls *cvtl_ref,int cfdx);
  fmc_cls(std::string cfnm);


  //Return properties
  std::string fnm()  {return _fnm;}
  vtl_cls *vfnc()    {return _vtl_obj;} 
  int fdx()          {return _fdx;}

  //Use for sorting
  bool operator<(const fmc_cls &right)const {
    return ( _fnm < right._fnm);
  }
};


//Virtual Class      /******************************************/
class vtl_cls: public ncoParserTokenTypes {
public:
  // expression types,

  std::vector<fmc_cls> fmc_vtr;

  virtual void fnc(int){;}
  virtual int size(void) { return fmc_vtr.size(); }
  virtual std::vector<fmc_cls> *lst_vtr(void){ return &fmc_vtr;}
  virtual var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker)=0;
  virtual ~vtl_cls(){;}
  virtual vtl_typ expr_typ(RefAST expr);
};



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
   enum {SET_MISS,CH_MISS,DEL_MISS,RAM_WRITE,RAM_DELETE};
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



//Maths functions holder (for mth_cls) ********************/
class sym_cls{ /* sym_sct */
 public:
  std::string _fnm;
  double (*_fnc_dbl)(double); /* [fnc] Double-valued function */
  float (*_fnc_flt)(float);   /* [fnc] Float-valued function */
 public:
  sym_cls(std::string sfnm,  double (*fnc_dbl)(double), float (*fnc_flt)(float)){
    _fnm=sfnm;
    _fnc_dbl=fnc_dbl;
    _fnc_flt=fnc_flt; 
  } 
  sym_cls(const char *const pfnm,  double (*fnc_dbl)(double), float (*fnc_flt)(float)){
    _fnm= static_cast<std::string>(pfnm);
    _fnc_dbl=fnc_dbl;
    _fnc_flt=fnc_flt; 
  } 
  std::string fnm() { return _fnm;} 
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
   enum {PPOW,PATAN2,CONVERT};
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

#endif
