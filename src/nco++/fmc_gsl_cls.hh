/* Purpose: netCDF arithmetic processor class methods */

/* Copyright (C) 1995--2009 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

#ifndef FMC_GSL_CLS_HH // Contents have not yet been inserted in current source file  
#define FMC_GSL_CLS_HH

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

// Only use when GSL is present and enabled
#ifdef ENABLE_GSL

#include <sstream>
#include <string>
#include <vector>

#include <antlr/AST.hpp>
#include <gsl/gsl_version.h>
#include <gsl/gsl_mode.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_sf.h>

#include <gsl/gsl_rng.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>

#include "ncoTree.hpp"
#include "ncap2_utl.hh"
#include "vtl_cls.hh"

#ifndef NCO_GSL_MINOR_VERSION
# define NCO_GSL_MINOR_VERSION 12
#endif // NCO_GSL_MINOR_VERSION

#define HANDLE_ARGS bool&is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker 

// Global variable initialized in ncap2.cc
extern int ncap_gsl_mode_prec; /* Precision for GSL functions with mode_t argument (Airy, hypergeometric) */ 



// Classify double-type arguments in handler function hnd_fnc_nd(),hnd_fnc_rnd()
enum { P1DBL,   P2DBL,   P3DBL,   P4DBL,  
       P1DBLMD, P2DBLMD, P3DBLMD, P4DBLMD,  
       P0DBLX,  P1DBLX,  P2DBLX,  P3DBLX,  P4DBLX, P5DBLX};  


// Classify Bessel/Legendre methods in array function function hnd_fnc_iidpd()
enum { PBESSEL, PLEGEND };
// Union class to hold GSL function pointers
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
   
   int (*bidpd)(int, double, double*); 
   int (*biidpd)(int, int, double, double*); 
   int (*biddpd)(int, double, double, double*); 
 
  
   double (*cd)( double);
   double (*cdd)(double, double);    
   double (*cddd)(double, double,double);    
   double (*cdddd)(double,double,double,double);    
   double (*cddddd)(double, double,double,double,double);    

   double (*cu)(unsigned);
   double (*cud)(unsigned,double);
   double (*cudd)(unsigned,double,double);
   double (*cuu)(unsigned,unsigned); 
   double (*cuuu)(unsigned,unsigned,unsigned); 
   double (*cuuuu)(unsigned,unsigned,unsigned,unsigned); 
   double (*cudu)(unsigned,double,unsigned);


   double (*cr)(const gsl_rng*);
   double (*crd)(const gsl_rng*,double);
   double (*crdd)(const gsl_rng*,double, double);    
   double (*crddd)(const gsl_rng*,double, double,double);    
   double (*crdddd)(const gsl_rng*,double,double,double,double);    
   double (*crddddd)(const gsl_rng*,double, double,double,double,double);    
   double (*cru)(const gsl_rng*,unsigned);
   double (*cruu)(const gsl_rng*,unsigned,unsigned);
   double (*cruuu)(const gsl_rng*,unsigned,unsigned,unsigned);
   
   unsigned int (*dru)(const gsl_rng*,unsigned);
   unsigned int (*druu)(const gsl_rng*,unsigned,unsigned);
   unsigned int (*druuu)(const gsl_rng*,unsigned,unsigned,unsigned);
   unsigned int (*drd)(const gsl_rng*,double);
   unsigned int (*drdd)(const gsl_rng*,double,double);

   unsigned long int (*er)(const gsl_rng*);
   unsigned long int (*eru)(const gsl_rng*,unsigned long int);


   double (*cid)(int,double);

  //Return type int
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

  f_unn(   int (*b)(int, double, double*) ){ bidpd=b;} 
  f_unn(   int (*b)(int, int, double, double*) ){ biidpd=b;} 
  f_unn(   int (*b)(int, double, double, double*) ){ biddpd=b;} 

  //Return type double
  f_unn(   double (*c)(double)     )                   { cd=c; }
  f_unn(   double (*c)( double,double) )               { cdd=c; }
  f_unn(   double (*c)( double,double,double) )        { cddd=c; }
  f_unn(   double (*c)( double,double,double,double) ) { cdddd=c; }
  f_unn(   double (*c)( double,double,double,double,double) ) { cddddd=c; }
  f_unn(   double (*c) (unsigned)       )                       {cu=c;}
  f_unn(   double (*c) (unsigned,double)        )               {cud=c;}
  f_unn(   double (*c) (unsigned,double,double)  )              {cudd=c;}
  f_unn(   double (*c) (unsigned,unsigned)        )             {cuu=c;}
  f_unn(   double (*c) (unsigned,unsigned,unsigned)  )          {cuuu=c;}
  f_unn(   double (*c) (unsigned,unsigned,unsigned,unsigned))   {cuuuu=c;}
  f_unn(   double (*c) (unsigned,double,unsigned) )             {cudu=c;} 



  f_unn(   double (*c)(const gsl_rng*)     )                          { cr=c; }
  f_unn(   double (*c)(const gsl_rng*,double)     )                   { crd=c; }
  f_unn(   double (*c)(const gsl_rng*, double,double) )               { crdd=c; }
  f_unn(   double (*c)(const gsl_rng*, double,double,double) )        { crddd=c; }
  f_unn(   double (*c)(const gsl_rng*, double,double,double,double) ) { crdddd=c; }
  f_unn(   double (*c)(const gsl_rng*, double,double,double,double,double) ) { crddddd=c; }
  f_unn(   double (*c)(const gsl_rng*,unsigned)     )                 { cru=c; }

  // return type unsigned int
  f_unn(unsigned int (*d)(const gsl_rng*, unsigned) )                   { dru=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, unsigned,unsigned) )          { druu=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, unsigned,unsigned,unsigned) ) { druuu=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, double) )                     { drd=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, double,double) )              { drdd=d; }

  f_unn(unsigned long int(*e)(const gsl_rng*))                          {er=e;}
  f_unn(unsigned long int(*e)(const gsl_rng*,unsigned long int))        {eru=e;}

};

// Class to hold GSL function name, function pointers, function handler
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

// GSL Function
class gsl_cls : public vtl_cls {
private:
   bool _flg_dbg;
  std::vector<gpr_cls> gpr_vtr;

public:
  gsl_cls(bool flg_dbg);
  void gsl_ini_sf(void);
  void gsl_ini_cdf(void);
  void gsl_ini_ran(void);

  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
static  var_sct *hnd_fnc_x(HANDLE_ARGS);
static  var_sct *hnd_fnc_xd(HANDLE_ARGS);
static  var_sct *hnd_fnc_iidpd(HANDLE_ARGS);
static  var_sct *hnd_fnc_idpd(HANDLE_ARGS);
static  var_sct *hnd_fnc_nd(HANDLE_ARGS);
static  var_sct *hnd_fnc_idd(HANDLE_ARGS);
static  var_sct *hnd_fnc_iid(HANDLE_ARGS);
static  var_sct *hnd_fnc_rnd(HANDLE_ARGS);
static  var_sct *hnd_fnc_udrx(HANDLE_ARGS);
static  var_sct *hnd_fnc_uerx(HANDLE_ARGS);

};



//GSL2  /****************************************/
// nb For custom gsl functions that don't fit into gsl_cls 
class gsl2_cls: public vtl_cls {
private:
   enum {PGSL_RNG_MIN,PGSL_RNG_MAX,PGSL_RNG_NAME };
   bool _flg_dbg;
public:
  gsl2_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};




#endif // !ENABLE_GSL

#endif // FMC_GSL_CLS_HH
