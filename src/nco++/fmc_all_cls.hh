/* Purpose: netCDF arithmetic processor class methods */

/* Copyright (C) 1995--2007 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

// Standard C++ headers
#ifndef FMC_ALL_CLS_HH
#define FMC_ALL_CLS_HH

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <antlr/AST.hpp>

#include "ncoTree.hpp"
#include "ncap2_utl.hh"
#include "vtl_cls.hh"

#include "sym_cls.hh" // holder for float/double math function pointers


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
