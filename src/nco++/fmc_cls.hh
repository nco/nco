/* Purpose: netCDF arithmetic processor class methods */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

// Standard C++ headers
#ifndef FMC_CLS_HH
#define FMC_CLS_HH

#include <string>
#include "vtl_cls.hh"


//forward declarations
// class vtl_cls;


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


#endif // FMC_CLS_HH
