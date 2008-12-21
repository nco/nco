/* Purpose: netCDF arithmetic processor class methods */

/* Copyright (C) 1995--2009 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#ifndef INC_FMC_GSL_CLS_hh_
#define INC_FMC_GSL_CLS_hh_


#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <antlr/AST.hpp>
#include "ncoTree.hpp"
#include "ncap2_utl.hh"
#include "vtl_cls.hh"

#include <antlr/AST.hpp>

#include <gsl/gsl_mode.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf.h>

#define HANDLE_ARGS bool&is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker 

// used to classify double type args in handle function hnd_fnc_nd
enum { P1DBL, P1DBLMD, P2DBL, P2DBLMD,P3DBL, P3DBLMD, P4DBL, P4DBLMD };  

// union class to hold gsl function pointers
union f_unn{
 public:
   int (*ai)(int, gsl_sf_result*);
   int (*au)(unsigned int, gsl_sf_result*);

   int (*ad)(double, gsl_sf_result*);
   int (*add)(double, double,gsl_sf_result*);    
   int (*addd)(double, double,double,gsl_sf_result*);    
   int (*adddd)(double,double,double, double,gsl_sf_result*);    


   int (*aid) (int,double,gsl_sf_result*);
   int (*aiid)( int,int,double,gsl_sf_result*);          
   int (*aidd)( int,double,double,gsl_sf_result*);          

   int (*adm)( double, gsl_mode_t,gsl_sf_result*);  
   int (*addm)( double, double, gsl_mode_t,gsl_sf_result*);  
   int (*adddm)( double,double, double, gsl_mode_t,gsl_sf_result*);  
   int (*addddm)( double,double,double, double, gsl_mode_t,gsl_sf_result*);  

   double (*bd)( double);
   double (*bdd)(double, double);    
   double (*bid)(int,double);
   int (*bidpd)(int, double, double*); 
   int (*biidpd)(int, int, double, double*); 
 
   int (*cb)(bool);

  f_unn(   int (*a)( int,gsl_sf_result*) )            { ai=a; }
  f_unn(   int (*a)( unsigned int,gsl_sf_result*) )   { au=a; }

  f_unn(   int (*a)( int,int,double,gsl_sf_result*) )           { aiid=a; }
  f_unn(   int (*a)( int,double,double,gsl_sf_result*) )        { aidd=a; }
  f_unn(   int (*a)( double,gsl_sf_result*) )                  { ad=a; }
  f_unn(   int (*a)( double,double, gsl_sf_result*) )         { add=a; }
  f_unn(   int (*a)( double,double,double, gsl_sf_result*) )  { addd=a; }
  f_unn(   int (*a)( double,double,double,double, gsl_sf_result*) ) { adddd=a; }

  f_unn(   int (*a)(int, double,gsl_sf_result*) )     { aid=a; }
  f_unn(   int (*a)(double,gsl_mode_t,gsl_sf_result*)){adm=a; }  
  f_unn(   int (*a)(double,double,gsl_mode_t,gsl_sf_result*)){addm=a; }  
  f_unn(   int (*a)(double,double,double,gsl_mode_t,gsl_sf_result*)){adddm=a; }  
  f_unn(   int (*a)(double,double,double,double,gsl_mode_t,gsl_sf_result*)){addddm=a; }  

  f_unn(   double (*b)(double)     )    { bd=b; }
  f_unn(   double (*b)( double,double) ) { bdd=b; }
  f_unn(   double (*b)(int, double) )    { bid=b; }
  f_unn(   int (*b)(int, double, double*) ){ bidpd=b;} 
  f_unn(   int (*b)(int, int, double, double*) ){ biidpd=b;} 

};


// class to hold gsl function name, function pointers,function handler
class gpr_cls { 
 private:
  std::string _fnm;
  f_unn _pfptr_e;
  nc_type _type;  
public:
  var_sct* (*_hnd_fnc)(HANDLE_ARGS);

  gpr_cls(std::string ifnm, f_unn pfptr_e,var_sct* (*hnd_fnc)(HANDLE_ARGS) ):_pfptr_e(pfptr_e){
    _fnm=ifnm;
    _hnd_fnc=hnd_fnc;
    _type=NC_NAT;
  } 

  gpr_cls(const char *const pfnm, f_unn pfptr_e,var_sct* (*hnd_fnc)(HANDLE_ARGS)  ): _pfptr_e(pfptr_e) {
    _fnm= static_cast<std::string>(pfnm);
    _hnd_fnc=hnd_fnc;
    _type=NC_NAT; 
  }


  gpr_cls(const char *const pfnm, f_unn pfptr_e,var_sct* (*hnd_fnc)(HANDLE_ARGS), int type_in  ):_pfptr_e(pfptr_e){
    _fnm= static_cast<std::string>(pfnm);
    _hnd_fnc=hnd_fnc;
    _type=(nc_type)type_in;
  }


 
  std::string fnm() { return _fnm;} 
  f_unn g_args()    { return _pfptr_e; }     
  nc_type type(){ return _type;}
}; 


// GSL Function /****************************************/
class gsl_cls : public vtl_cls {
private:
   bool _flg_dbg;
  std::vector<gpr_cls> gpr_vtr;

public:
  gsl_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
};


var_sct *hnd_fnc_x(HANDLE_ARGS); 
var_sct *hnd_fnc_xd(HANDLE_ARGS);
var_sct *hnd_fnc_iidpd(HANDLE_ARGS);
var_sct *hnd_fnc_idpd(HANDLE_ARGS);
var_sct *hnd_fnc_dm(HANDLE_ARGS);
var_sct *hnd_fnc_ddm(HANDLE_ARGS);
var_sct *hnd_fnc_nd(HANDLE_ARGS);
var_sct *hnd_fnc_idd(HANDLE_ARGS);
var_sct *hnd_fnc_iid(HANDLE_ARGS);


#endif
