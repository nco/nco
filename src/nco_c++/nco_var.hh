// $Header$ 

// Purpose: Description (definition) of C++ interface to netCDF variable routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

// Usage:
// #include <nco_var.hh> // C++ interface to netCDF variable routines

#ifndef NCO_VAR_HH // Contents have not yet been inserted in current source file
#define NCO_VAR_HH

// C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <string> // Standard C++ string class
#include <valarray> // STL valarray class template

// Standard C headers
#if !(defined __xlC__) && !(defined SGIMP64) // C++ compilers that do not allow stdint.h
# include <stdint.h> // Required by g++ for LLONG_MAX, ULLONG_MAX, by icpc for int64_t
#endif // C++ compilers that do not allow stdint.h

// 3rd party vendors
#include <netcdf.h> // netCDF C interface

// Personal headers
#include <nco_utl.hh> // C++ interface utilities for netCDF routines

// Forward declarations (DeD01 p. 500)

// Typedefs

// Define nco_cls class

// Prototype global functions with C++ linkages

// Begin nco_def_var() overloads

int // O [enm] Return success code
nco_def_var // [fnc] Create variable in netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const nc_type &var_xtype, // I [enm] External type of variable
 const int &dmn_nbr, // I [nbr] Number of dimensions
 const int * const &dmn_id, // I [id] Dimension IDs
 int &var_id); // O [id] Variable ID
// end nco_def_var<int *>() prototype

int // O [enm] Return success code
nco_def_var // [fnc] Create variable in netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const nc_type &var_xtype, // I [enm] External type of variable
 const std::valarray<int> &dmn_id, // I [id] Dimension IDs
 int &var_id); // O [id] Variable ID
// end nco_def_var<std::valarray<int>>() prototype

// End nco_def_var() overloads
// Begin nco_inq_var() overloads

int // O [enm] Return success code
nco_inq_var // [fnc] Inquire variable
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 std::string &var_nm, // O [sng] Variable name
 nc_type &var_typ, // O [enm] Variable type
 int &dmn_nbr, // O [nbr] Number of dimensions
 int *&dmn_id, // O [id] Dimension IDs
 int &att_nbr, // O [nbr] Number of attributes
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_var() prototype

// Begin nco_inq_varid() overloads

int // O [enm] Return success code
nco_inq_varid // [fnc] Inquire variable ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 int &var_id, // O [id] Variable ID
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_varid() prototype

int // O [id] Variable ID
nco_inq_varid // [fnc] Inquire variable ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm); // I [sng] Variable name
// end nco_inq_varid() prototype

// End nco_inq_varid() overloads
// Begin nco_inq_varname() overloads

int // O [enm] Return success code
nco_inq_varname // [fnc] Inquire variable name
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 std::string &var_nm, // O [sng] Variable name
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_varname() prototype

std::string // O [sng] Variable name
nco_inq_varname // [fnc] Inquire variable name
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id); // I [id] Variable ID
// end nco_inq_varname() prototype

// End nco_inq_varname() overloads
// Begin nco_inq_vartype() overloads

int // O [enm] Return success code
nco_inq_vartype // [fnc] Inquire variable type
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 nc_type &var_typ, // O [enm] Variable type
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_vartype() prototype

nc_type // O [enm] Variable type
nco_inq_vartype // [fnc] Inquire variable type
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id); // I [id] Variable ID
// end nco_inq_vartype() prototype

nc_type // O [enm] Variable type
nco_inq_vartype // [fnc] Inquire variable type
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm); // I [sng] Variable name
// end nco_inq_vartype() prototype

// End nco_inq_vartype() overloads
// Begin nco_inq_varndims() overloads

int // O [enm] Return success code
nco_inq_varndims // [fnc] Inquire variable rank
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 int &dmn_nbr, // O [nbr] Number of dimensions
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_varndims() prototype

int // O [enm] Return success code
nco_inq_varndims // [fnc] Inquire variable rank
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 int &dmn_nbr, // O [nbr] Number of dimensions
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_varndims() prototype

int // O [nbr] Number of dimensions
nco_inq_varndims // [fnc] Inquire variable rank
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id); // I [id] Variable ID
// end nco_inq_varndims() prototype

int // O [nbr] Number of dimensions
nco_inq_varndims // [fnc] Inquire variable rank
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm); // I [sng] Variable name
// end nco_inq_varndims() prototype

// End nco_inq_varndims() overloads
// Begin nco_inq_varnatts() overloads

int // O [enm] Return success code
nco_inq_varnatts // [fnc] Inquire variable attributes
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 int &att_nbr, // O [nbr] Number of attributes
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_varnatts() prototype

int // O [nbr] Number of attributes
nco_inq_varnatts // [fnc] Inquire variable attributes
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id); // I [id] Variable ID
// end nco_inq_varnatts() prototype

int // O [nbr] Number of attributes
nco_inq_varnatts // [fnc] Inquire variable attributes
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm); // I [sng] Variable name
// end nco_inq_varnatts() prototype

// End nco_inq_varnatts() overloads

int // O [enm] Return success code
nco_inq_vardimid // [fnc] Inquire variable dimension IDS
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 int *&dmn_id, // O [id] Dimension IDs
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_vardimid() prototype

// End nco_inq_var() overloads
// Begin nco_put_vara() overloads

// Overload 1:  Write array given ID, start, count vectors
int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::valarray<size_t> &var_srt, // I [idx] Start vector
 const std::valarray<size_t> &var_cnt, // I [nbr] Count vector
 const float * const &var_val); // I [frc] Variable value
// end nco_put_vara<valarray,valarray,float *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const float * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,float *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const double * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,double *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const long double * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,long double *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const int * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,int *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const long * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,long *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const short * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,short *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const char * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,char *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const signed char * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,signed char *>() prototype

#ifdef ENABLE_NETCDF4
int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const unsigned char * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,unsigned char *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const unsigned short * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,unsigned short *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const unsigned int * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,unsigned int *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const long long * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,long long *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const unsigned long long * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,unsigned long long *>() prototype

int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const char * * const &var_val); // I [frc] Variable value
// end nco_put_vara<size_t *,size_t *,char * *>() prototype
#endif // !ENABLE_NETCDF4

// Overload 2: Write array given name, start, count vectors
template<typename typ_ntr>
int // O [enm] Return success code
nco_put_vara // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const size_t * const &var_srt, // I [idx] Start vector
 const size_t * const &var_cnt, // I [nbr] Count vector
 const typ_ntr * const &var_val) // I [frc] Variable value
{
  // Purpose: Wrapper for nco_put_vara()
  int rcd=nco_put_vara(nc_id,nco_inq_varid(nc_id,var_nm),var_srt,var_cnt,var_val);
  return rcd;
} // end nco_put_vara<size_t *,size_t *,typ_ntr *>()

// Overload 5: Write coordinate given name, size
template<typename typ_ntr>
int // O [enm] Return success code
nco_put_vara_crd // [fnc] Write coordinate to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Coordinate name
 const size_t &dmn_sz, // I [nbr] Dimension size
 const typ_ntr * const &var_val) // I [frc] Coordinate value
{
  /* Purpose: Wrapper for nco_put_vara() for coordinates
     Routine writes all values input coordinate variable with nco_put_vara() interface
     Routine works with writing any coordinate, record or not
     Non-record coordinates may be written with nco_put_var() interface
     since dimension fixed dimension size is always known by netCDF layer.
     Record coordinate size is unknown until nco_put_vara() interface is called
     Hence, first record variable write in program _should_ call this routine
     This facilitates communicating record variable size netCDF file */
  const size_t cnt_crd(dmn_sz); // [nbr] Count for dimension
  const size_t *cnt_vct_crd(&cnt_crd); // [nbr] Count vector for dimension
  const size_t srt_crd(0); // [idx] Starting offset for dimension
  const size_t *srt_vct_crd(&srt_crd); // [idx] Starting offset vector for dimension

  int rcd=nco_put_vara(nc_id,nco_inq_varid(nc_id,var_nm),srt_vct_crd,cnt_vct_crd,var_val);
  return rcd;
} // end nco_put_vara_crd<string,size_t,typ_ntr *>()

// End nco_put_vara() overloads
// Begin nco_put_var() overloads

// Overload 1: Write array given ID
int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const float * const &var_val); // I [frc] Variable value
// end nco_put_var<float *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const double * const &var_val); // I [frc] Variable value
// end nco_put_var<double *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const long double * const &var_val); // I [frc] Variable value
// end nco_put_var<long double *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const int * const &var_val); // I [frc] Variable value
// end nco_put_var<int *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const long * const &var_val); // I [frc] Variable value
// end nco_put_var<long *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const short * const &var_val); // I [frc] Variable value
// end nco_put_var<short *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const char * const &var_val); // I [frc] Variable value
// end nco_put_var<char *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const signed char * const &var_val); // I [frc] Variable value
// end nco_put_var<signed char *>() prototype

#ifdef ENABLE_NETCDF4
int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const unsigned char * const &var_val); // I [frc] Variable value
// end nco_put_var<unsigned char *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const unsigned short * const &var_val); // I [frc] Variable value
// end nco_put_var<unsigned short *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const unsigned int * const &var_val); // I [frc] Variable value
// end nco_put_var<unsigned int *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const long long * const &var_val); // I [frc] Variable value
// end nco_put_var<long long *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const unsigned long long * const &var_val); // I [frc] Variable value
// end nco_put_var<unsigned long long *>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const char * * const &var_val); // I [frc] Variable value
// end nco_put_var<char * *>() prototype
#endif // !ENABLE_NETCDF4

template<typename typ_ntr>
int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 // fxm: make this typename?
 const std::valarray<typ_ntr> &var_val) // I [frc] Variable value
{
  // Purpose: Wrapper for nco_put_var()
  int rcd=nco_put_var(nc_id,var_id,&(const_cast<std::valarray<typ_ntr> &>(var_val)[0]));
  // Simpler statement fails unless const replaced by typename in var_val qualifier because taking address of non-lvalue
  //  int rcd=nco_put_var(nc_id,var_id,&var_val[0]);
  return rcd;
} // end nco_put_var<std::valarray<typ_ntr> >()

// Overload 2: Write array given name
// Overload 2: int nco_put_var(const int &nc_id,const int &var_id,const <T> * const &var_val)
template<typename typ_ntr>
int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const typ_ntr * const &var_val) // I [frc] Variable value
{
  // Purpose: Wrapper for nco_put_var()
  // std::cout << "template<typename typ_ntr> int nco_put_var(const int &nc_id,const std::string &var_nm,const typ_ntr * const &var_val) received variable \"" << var_nm << "\"" << std::endl;
  int rcd=nco_put_var(nc_id,nco_inq_varid(nc_id,var_nm),var_val);
  return rcd;
} // end nco_put_var<typ_ntr *>()

// Overload 2: int nco_put_var(const int &nc_id,const int &var_id,const <T> * const &var_val)
template<typename typ_ntr>
int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 // fxm: make this typename?
 const std::valarray<typ_ntr> &var_val) // I [frc] Variable value
{
  // Purpose: Wrapper for nco_put_var()
  int rcd=nco_put_var(nc_id,nco_inq_varid(nc_id,var_nm),&(const_cast<std::valarray<typ_ntr> &>(var_val)[0]));
  // int rcd=nco_put_var(nc_id,nco_inq_varid(nc_id,var_nm),&var_val[0]);
  return rcd;
} // end nco_put_var<std::valarray<typ_ntr> >()

// Overload 3: Write scalar given ID
int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const float &var_val); // I [frc] Variable value
// end nco_put_var<float>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const double &var_val); // I [frc] Variable value
// end nco_put_var<double>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const long double &var_val); // I [frc] Variable value
// end nco_put_var<long double>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const int &var_val); // I [frc] Variable value
// end nco_put_var<int>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const long &var_val); // I [frc] Variable value
// end nco_put_var<long>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const short &var_val); // I [frc] Variable value
// end nco_put_var<short>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const char &var_val); // I [frc] Variable value
// end nco_put_var<char>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const signed char &var_val); // I [frc] Variable value
// end nco_put_var<signed char>() prototype

#ifdef ENABLE_NETCDF4
int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const unsigned char &var_val); // I [frc] Variable value
// end nco_put_var<unsigned char>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const unsigned short &var_val); // I [frc] Variable value
// end nco_put_var<unsigned short>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const unsigned int &var_val); // I [frc] Variable value
// end nco_put_var<unsigned int>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const long long &var_val); // I [frc] Variable value
// end nco_put_var<long long>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const unsigned long long &var_val); // I [frc] Variable value
// end nco_put_var<unsigned long long>() prototype

int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const char * &var_val); // I [frc] Variable value
// end nco_put_var<char *>() prototype
#endif // !ENABLE_NETCDF4

// Overload 4: Write scalar given name
// Overload 4: int nco_put_var(const int &nc_id,const int &var_id,const <T> &var_val)
template<typename typ_ntr>
int // O [enm] Return success code
nco_put_var // [fnc] Write variable to netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const typ_ntr &var_val) // I [frc] Variable value
{
  // Purpose: Wrapper for nco_put_var()
  // std::cout << "template<typename typ_ntr> int nco_put_var(const int &nc_id,const std::string &var_nm,const typ_ntr &var_val) received variable \"" << var_nm << "\"" << std::endl;
  int rcd=nco_put_var(nc_id,nco_inq_varid(nc_id,var_nm),var_val);
  return rcd;
} // end nco_put_var<typ_ntr>()

// End nco_put_var() overloads
// Begin nco_get_var() overloads

// Overload 1: Get array given ID
int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 float *&var_val); // O [frc] Variable value
// end nco_get_var<float *>() prototype

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 double *&var_val); // O [frc] Variable value
// end nco_get_var<double *>() prototype

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 long double *&var_val); // O [frc] Variable value
// end nco_get_var<long double *>() prototype

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 int *&var_val); // O [frc] Variable value
// end nco_get_var<int *>() prototype

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 long *&var_val); // O [frc] Variable value
// end nco_get_var<long *>() prototype

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 short *&var_val); // O [frc] Variable value
// end nco_get_var<short *>() prototype

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 char *&var_val); // O [frc] Variable value
// end nco_get_var<char *>() prototype

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 signed char *&var_val); // O [frc] Variable value
// end nco_get_var<signed char *>() prototype

#ifdef ENABLE_NETCDF4
int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 unsigned char *&var_val); // O [frc] Variable value
// end nco_get_var<unsigned char *>()

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 unsigned short *&var_val); // O [frc] Variable value
// end nco_get_var<unsigned short *>()

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 unsigned int *&var_val); // O [frc] Variable value
// end nco_get_var<unsigned int *>()

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 long long *&var_val); // O [frc] Variable value
// end nco_get_var<long long *>()

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 unsigned long long *&var_val); // O [frc] Variable value
// end nco_get_var<unsigned long long *>()

int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 char * *&var_val); // O [frc] Variable value
// end nco_get_var<char * *>()
#endif // !ENABLE_NETCDF4

// Overload 1.5: Get array given ID
float * // O [frc] Variable value
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id); // I [id] Variable ID
// end nco_get_var<float> prototype

// Overload 2: Get array given name
template<typename typ_ntr>
int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 typ_ntr *&var_val) // O [frc] Variable value
{
  // Purpose: Wrapper for nco_get_var()
  int rcd=nco_get_var(nc_id,nco_inq_varid(nc_id,var_nm),var_val);
  return rcd;
} // end nco_get_var<typ_ntr *>()

// Overload 2.5: Get array given name
float * // O [frc] Variable value
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm); // I [sng] Variable name
// end nco_get_var<float> prototype

// Overload 3: Get scalar given ID

// Overload 4: Get scalar given name
template<typename typ_ntr>
int // O [enm] Return success code
nco_get_var // [fnc] Ingest variable from netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 typ_ntr &var_val) // O [frc] Variable value
{
  // Purpose: Wrapper for nco_get_var()
  int rcd=nco_get_var(nc_id,nco_inq_varid(nc_id,var_nm),var_val);
  return rcd;
} // end nco_get_var<typ_ntr>()

// End nco_get_var() overloads

// Define inline'd functions in header so source is visible to calling files

#endif // NCO_VAR_HH  






