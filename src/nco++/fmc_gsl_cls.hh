/* Purpose: netCDF arithmetic processor class methods */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#ifndef FMC_GSL_CLS_HH // Contents have not yet been inserted in current source file  
#define FMC_GSL_CLS_HH

#ifdef HAVE_CONFIG_H   
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

// Only use when GSL is present and enabled
#ifdef ENABLE_GSL

/* Used in some _ran handles, need it for INT_MAX */
#include <limits.h> 

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

#include <gsl/gsl_statistics.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_fit.h>

#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_multifit.h>

#include "ncoTree.hpp"
#include "ncap2_utl.hh"
#include "vtl_cls.hh"

#ifdef NCO_GSL_MAJOR_VERSION
# if NCO_GSL_MAJOR_VERSION >= 2
#  define NCO_GSL_VERSION ( NCO_GSL_MAJOR_VERSION * 100 + NCO_GSL_MINOR_VERSION * 10 + NCO_GSL_PATCH_VERSION )
# endif // NCO_GSL_MAJOR_VERSION
#endif // NCO_GSL_MAJOR_VERSION
#ifndef NCO_GSL_VERSION
# ifdef _MSC_VER
#  define NCO_GSL_VERSION 107
# else
#  define NCO_GSL_VERSION 221
# endif // _MSC_VER
#endif // NCO_GSL_VERSION

// Some of the gsl_ran_* functions return an unsigned int (NC_UINT)
// netcdf3 has no NC_UINT type So we converte the returned values to an NC_INT
// For some of the _ran functions may cause an overflow. In this 
// case we recomend that users switch ntecdf4 and use the NC_UINT return type

#define NCO_TYP_GSL_UINT NC_INT
//#define NCO_TYP_GSL_UINT NC_UINT


#define HANDLE_ARGS bool&is_mtd,std::vector<RefAST>&args_vtr,gpr_cls&gpr_obj,ncoTree&walker 

// macro to a function that converts an array of f_unn to a vector of f_unn
// used in constructor args to gpr_cls
#define ARR2VTR(arr_nm) gpr_cls::hlp_arr2vtr((arr_nm), sizeof((arr_nm))/sizeof(f_unn))

// Global variable initialized in ncap2.cc
extern int ncap_gsl_mode_prec; /* Precision for GSL functions with mode_t argument (Airy, hypergeometric) */ 



// Classify double-type arguments in handler function hnd_fnc_nd(),hnd_fnc_rnd()
enum { P1DBL,   P2DBL,   P3DBL,   P4DBL,  
       P1DBLMD, P2DBLMD, P3DBLMD, P4DBLMD,  
       P0DBLX,  P1DBLX,  P2DBLX,  P3DBLX,  P4DBLX, P5DBLX};  


// Classify Bessel/Legendre methods in array function function hnd_fnc_iidpd()
enum { PBESSEL, PLEGEND };

// Classify some of the stats methods --fxm: all type args need to be included
enum { PS_COV,PS_COR, PS_PVAR, PS_TTST, PS_MAX, PS_MIN, PS_MAX_IDX , PS_MIN_IDX };


// Union class to hold GSL function pointers
union f_unn{
 public:
   int (*av)(void);
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
  
  //gsl_statistics function prototypes

   double (*cscpss)(const char *,size_t,size_t);
   double (*csspss)(const short* ,size_t,size_t);
   double (*csipss)(const int* ,size_t,size_t);
   double (*csfpss)(const float* ,size_t,size_t);
    double (*csdpss)(const double*,size_t,size_t);
   double (*csucpss)(const unsigned char *,size_t,size_t);
   double (*csuspss)(const unsigned short* ,size_t,size_t);
   double (*csuipss)(const unsigned int* ,size_t,size_t);
   double (*cslpss)(const long* ,size_t,size_t);   
   double (*csulpss)(const unsigned long* ,size_t,size_t);   


   double (*cscpssd)(const char *,size_t,size_t,double);
   double (*csspssd)(const short* ,size_t,size_t,double);
   double (*csipssd)(const int* ,size_t,size_t,double);
   double (*csfpssd)(const float* ,size_t,size_t,double);
   double (*csdpssd)(const double*,size_t,size_t,double);
   double (*csucpssd)(const unsigned char *,size_t,size_t,double);
   double (*csuspssd)(const unsigned short* ,size_t,size_t,double);
   double (*csuipssd)(const unsigned int* ,size_t,size_t,double);
   double (*cslpssd)(const long* ,size_t,size_t,double);   
   double (*csulpssd)(const unsigned long* ,size_t,size_t,double);   




   unsigned int (*dru)(const gsl_rng*,unsigned);
   unsigned int (*druu)(const gsl_rng*,unsigned,unsigned);
   unsigned int (*druuu)(const gsl_rng*,unsigned,unsigned,unsigned);
   unsigned int (*drd)(const gsl_rng*,double);
   unsigned int (*drdd)(const gsl_rng*,double,double);
   unsigned int (*drdu)(const gsl_rng*,double,unsigned);


   unsigned long int (*er)(const gsl_rng*);
   unsigned long int (*eru)(const gsl_rng*,unsigned long int);


   double (*cid)(int,double);




  //Return type int
  f_unn(   int (*a)(void) )                           { av=a; }
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

  
  //gsl_statistics function prototypes
  f_unn(double (*c)(const unsigned char *,size_t,size_t))            {csucpss=c;}
  f_unn(double (*c)(const char *,size_t,size_t))                     {cscpss=c;}
  f_unn(double (*c)(const short* ,size_t,size_t))                    {csspss=c;}
  f_unn(double (*c)(const int* ,size_t,size_t))                      {csipss=c;}
  f_unn(double (*c)(const float* , size_t,size_t)  )                 {csfpss=c;}
  f_unn(double (*c)(const double*, size_t,size_t) )                  {csdpss=c;}
  f_unn(double (*c)(const unsigned short* ,size_t,size_t))           {csuspss=c;} 
  f_unn(double (*c)(const unsigned int* ,size_t,size_t))             {csuipss=c;} 
  f_unn(double (*c)(const long* ,size_t,size_t))                     {cslpss=c;}      
  f_unn(double (*c)(const unsigned long* ,size_t,size_t))            {csulpss=c;}  

  f_unn(double (*c)(const unsigned char *,size_t,size_t,double))            {csucpssd=c;}
  f_unn(double (*c)(const char *,size_t,size_t,double))                     {cscpssd=c;}
  f_unn(double (*c)(const short* ,size_t,size_t,double))                    {csspssd=c;}
  f_unn(double (*c)(const int* ,size_t,size_t,double))                      {csipssd=c;}
  f_unn(double (*c)(const float* , size_t,size_t,double)  )                 {csfpssd=c;}
  f_unn(double (*c)(const double*, size_t,size_t,double) )                  {csdpssd=c;}
  f_unn(double (*c)(const unsigned short* ,size_t,size_t,double))           {csuspssd=c;} 
  f_unn(double (*c)(const unsigned int* ,size_t,size_t,double))             {csuipssd=c;} 
  f_unn(double (*c)(const long* ,size_t,size_t,double))                     {cslpssd=c;}      
  f_unn(double (*c)(const unsigned long* ,size_t,size_t,double))            {csulpssd=c;}  

  
  // return type unsigned int
  f_unn(unsigned int (*d)(const gsl_rng*, unsigned) )                   { dru=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, unsigned,unsigned) )          { druu=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, unsigned,unsigned,unsigned) ) { druuu=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, double) )                     { drd=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, double,double) )              { drdd=d; }
  f_unn(unsigned int (*d)(const gsl_rng*, double,unsigned) )            { drdu=d; }

  f_unn(unsigned long int(*e)(const gsl_rng*))                          {er=e;}
  f_unn(unsigned long int(*e)(const gsl_rng*,unsigned long int))        {eru=e;}

};


// dummy function
int ncap_void(void);


// Class to hold GSL function name, function pointers, function handler
class gpr_cls { 
 private:
  std::string _fnm;

 // for regular constructors this holds only one gsl function pointer
  std::vector<f_unn> _in_f_unn_vtr; 
  nc_type _type;  
public:
  var_sct* (*_hnd_fnc)(HANDLE_ARGS);

  gpr_cls(std::string ifnm, f_unn pfptr_e,var_sct* (*hnd_fnc)(HANDLE_ARGS) ){
    _fnm=ifnm;
    _in_f_unn_vtr.push_back(pfptr_e);
    _hnd_fnc=hnd_fnc;
    _type=NC_NAT;

  } 
  
  gpr_cls(const char *const pfnm, f_unn pfptr_e,var_sct* (*hnd_fnc)(HANDLE_ARGS) ) {
    _fnm= static_cast<std::string>(pfnm);
    _in_f_unn_vtr.push_back(pfptr_e);
    _hnd_fnc=hnd_fnc;
    _type=NC_NAT; 

  }

  gpr_cls(const char *const pfnm, f_unn pfptr_e,var_sct* (*hnd_fnc)(HANDLE_ARGS), int type_in){
    _fnm= static_cast<std::string>(pfnm);
    _in_f_unn_vtr.push_back(pfptr_e);
    _hnd_fnc=hnd_fnc;
    _type=(nc_type)type_in;

  }

  // Constructor with vector of gsl func args
  gpr_cls(const char *const pfnm, std::vector<f_unn> in_f_unn_vtr, var_sct* (*hnd_fnc)(HANDLE_ARGS) ) {
    _fnm= static_cast<std::string>(pfnm);
    _in_f_unn_vtr=in_f_unn_vtr;
    _hnd_fnc=hnd_fnc;
    _type=NC_NAT; 

  }

  // Constructor with vector of gsl func args
  gpr_cls(const char *const pfnm, std::vector<f_unn> in_f_unn_vtr, var_sct* (*hnd_fnc)(HANDLE_ARGS), int type_in ){
    _fnm= static_cast<std::string>(pfnm);
    _in_f_unn_vtr=in_f_unn_vtr;
    _hnd_fnc=hnd_fnc;
    _type=(nc_type)type_in;

  }
  
  // helper method used in constructor args to convert an array of f_unn to a vector
  // we can call it within constructor args as it is a static method
  static std::vector<f_unn> hlp_arr2vtr( f_unn *arr_nm,unsigned sz){
    std::vector<f_unn> in_f_unn_vtr;
    for(unsigned idx=0 ; idx<sz; idx++)
      in_f_unn_vtr.push_back(arr_nm[idx]);

    return in_f_unn_vtr;
  }
  

  std::string fnm() { return _fnm;} 
  
  // maintain compatibility with older code  
  //f_unn g_args(void)    { return _in_f_unn_vtr[0]; }

  // For functions that have multiple gsl function prototypes  
  f_unn g_args(int idx=0) { return _in_f_unn_vtr[idx]; }     

  nc_type type(){ return _type;}
}; 

// GSL Function
class gsl_cls : public vtl_cls {
private:
   //bool _flg_dbg;
  std::vector<gpr_cls> gpr_vtr;

public:
  gsl_cls(bool flg_dbg);
  void gsl_ini_sf(void);
  void gsl_ini_cdf(void);
  void gsl_ini_ran(void);
  void gsl_ini_stats(void);

  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
static  var_sct *hnd_fnc_x(HANDLE_ARGS);
static  var_sct *hnd_fnc_xd(HANDLE_ARGS);
static  var_sct *hnd_fnc_iidpd(HANDLE_ARGS);
static  var_sct *hnd_fnc_idpd(HANDLE_ARGS);
static  var_sct *hnd_fnc_nd(HANDLE_ARGS);
static  var_sct *hnd_fnc_idd(HANDLE_ARGS);
static  var_sct *hnd_fnc_iid(HANDLE_ARGS);
static  var_sct *hnd_fnc_ud(HANDLE_ARGS);
static  var_sct *hnd_fnc_udu(HANDLE_ARGS);

// the following functions handle explicitly _ran & _rng functions
static  var_sct *hnd_fnc_rnd(HANDLE_ARGS);
static  var_sct *hnd_fnc_ru(HANDLE_ARGS);
static  var_sct *hnd_fnc_udrx(HANDLE_ARGS);
static  var_sct *hnd_fnc_uerx(HANDLE_ARGS);
static  var_sct *hnd_fnc_udrdu(HANDLE_ARGS); // explict handler

// The following functions handle gsl_statistical functions
static  var_sct *hnd_fnc_stat1(HANDLE_ARGS);
static  var_sct *hnd_fnc_stat2(HANDLE_ARGS);
static  var_sct *hnd_fnc_stat3(HANDLE_ARGS);
static  var_sct *hnd_fnc_stat4(HANDLE_ARGS);
};



//GSL2  /****************************************/
// nb For custom gsl functions that don't fit into gsl_cls 
class gsl2_cls: public vtl_cls {
private:
   enum {PGSL_RNG_MIN,PGSL_RNG_MAX,PGSL_RNG_NAME };
   //bool _flg_dbg;
public:
   gsl2_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};



//GSL STATISTICS 2  /****************************************/
// gsl statistic functions for floating points only
class gsl_stt2_cls: public vtl_cls {
private:
  enum { PWMEAN,    PWVAR,    PWSD,   PWVAR_MEAN,
         PWSD_MEAN, PWABSDEV, PWSKEW, PWKURTOSIS,
         PWVAR_M,   PWSD_M,   PWABSDEV_M, PWSKEW_M_SD,
         PWKURTOSIS_M_SD
       };
   //bool _flg_dbg;
public:
  gsl_stt2_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);

};



//GSL  /****************************************/
// gsl spline interpolation 
class gsl_spl_cls: public vtl_cls {
private:
   enum {PLINEAR,PPOLY,PCSPLINE,PCSPLINE_PER,PAKIMA,PAKIMA_PER,PEVAL };
   //bool _flg_dbg;
public:
   gsl_spl_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
  var_sct *eval_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker);  
  var_sct *spl_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker);  

};



//GSL  /****************************************/
// gsl Least Square Fitting 
class gsl_fit_cls: public vtl_cls {
private:
   enum { PLIN,PWLIN,PLIN_EST,PMUL,PWMUL,PMUL_EST };
   //bool _flg_dbg;
public:
   gsl_fit_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
  var_sct *fit_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker);  
  var_sct *fit_est_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker);  
  // int rm_miss_arr(double *x_in,long long x_stride,double *y_in, long long y_stride, double *w_in,long long w_stride,long long n);
};


//GSL  /****************************************/
// gsl multi-parameter Least Square Fitting 
class gsl_mfit_cls: public vtl_cls {
private:
  enum { PMLIN, PMWLIN, PMLIN_EST};
   //bool _flg_dbg;
public:
   gsl_mfit_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr, RefAST fargs,fmc_cls &fmc_obj, ncoTree &walker);
  var_sct *mfit_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker);
  var_sct *mfit_est_fnd(bool &is_mtd, std::vector<RefAST> &args_vtr, fmc_cls &fmc_obj, ncoTree &walker);    

};

#endif // !ENABLE_GSL

#ifdef ENABLE_GSL
class nco_gsl_cls: public vtl_cls 
{
private:
  enum {NCO_GSL_FUNC1,NCO_GSL_FUNC2};
  //bool _flg_dbg;
  std::vector<gpr_cls> gpr_vtr;
public:
  nco_gsl_cls(bool flg_dbg);
  var_sct *fnd(RefAST expr,RefAST fargs,fmc_cls &fmc_obj,ncoTree &walker);
  var_sct *fit_fnd(bool &is_mtd,std::vector<RefAST> &args_vtr,fmc_cls &fmc_obj,ncoTree &walker);   
};
#endif // !ENABLE_GSL

#endif // FMC_GSL_CLS_HH
