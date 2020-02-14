// $Header$ 

// Purpose: Description (definition) of C++ interface to netCDF attribute routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

// Usage:
// #include <nco_att.hh> // C++ interface to netCDF attribute routines

#ifndef NCO_ATT_HH // Contents have not yet been inserted in current source file
#define NCO_ATT_HH

// C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <string> // Standard C++ string class
#include <vector> // STL vector class template

// Standard C headers

// 3rd party vendors
#include <netcdf.h> // netCDF C interface

// Personal headers
#include <nco_utl.hh> // C++ interface utilities for netCDF routines
#include <nco_var.hh> // C++ interface to netCDF variable routines

// Typedefs

// Define nco_cls class

// Prototype global functions with C++ linkages

// Begin nco_put_att() overloads

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const float * const &att_val, // I [sng] Attribute value
 const nc_type &att_typ=NC_FLOAT); // I [enm] Attribute type 
// end nco_put_att<float *>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const float &att_val, // I [sng] Attribute value
 const nc_type &att_typ=NC_FLOAT); // I [enm] Attribute type 
// end nco_put_att<float>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const double * const &att_val, // I [sng] Attribute value
 const nc_type &att_typ=NC_DOUBLE); // I [enm] Attribute type 
// end nco_put_att<double *>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const double &att_val, // I [sng] Attribute value
 const nc_type &att_typ=NC_DOUBLE); // I [enm] Attribute type 
// end nco_put_att<double>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const int * const &att_val, // I [frc] Attribute value
 const nc_type &att_typ=NC_INT); // I [enm] Attribute type 

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const int &att_val, // I [frc] Attribute value
 const nc_type &att_typ=NC_INT); // I [enm] Attribute type 
// end nco_put_att<int>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const short * const &att_val, // I [frc] Attribute value
 const nc_type &att_typ=NC_SHORT); // I [enm] Attribute type 

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const short &att_val, // I [frc] Attribute value
 const nc_type &att_typ=NC_SHORT); // I [enm] Attribute type 
// end nco_put_att<short>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const long * const &att_val, // I [frc] Attribute value
 const nc_type &att_typ=NC_INT); // I [enm] Attribute type 

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const long &att_val, // I [frc] Attribute value
 const nc_type &att_typ=NC_INT); // I [enm] Attribute type 
// end nco_put_att<long>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const std::string &att_val); // I [sng] Attribute value
// end nco_put_att<std::string>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm, // I [sng] Attribute name
 const std::string &att_val); // I [sng] Attribute value
// end nco_put_att<std::string>() prototype

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const std::vector<double> &att_val, // I [sng] Attribute value
 const nc_type &att_typ=NC_DOUBLE); // I [enm] Attribute type 
// end nco_put_att<std::vector<double>>() prototype

// End nco_put_att() overloads
// Begin nco_inq_att() overloads

int // O [enm] Return success code
nco_inq_att // [fnc] Inquire attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 nc_type &att_typ, // O [enm] Attribute type
 size_t &att_sz, // O [nbr] Attribute size
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_att() prototype 

// End nco_inq_att() overloads
// Begin nco_inq_attid() overloads

int // O [enm] Return success code
nco_inq_attid // [fnc] Inquire attribute ID
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [sng] Variable name
 const std::string &att_nm, // I [sng] Attribute name
 int &att_id, // O [id] Attribute ID
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_attid() prototype 

int // O [enm] Return success code
nco_inq_attid // [fnc] Inquire attribute ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm, // I [sng] Attribute name
 int &att_id, // O [id] Attribute ID
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_attid() prototype 

int // O [id] Attribute ID
nco_inq_attid // [fnc] Inquire attribute ID
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm); // I [sng] Attribute name
// end nco_inq_attid() prototype 

int // O [id] Attribute ID
nco_inq_attid // [fnc] Inquire attribute ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm); // I [sng] Attribute name
// end nco_inq_attid() prototype 

// End nco_inq_attid() overloads
// Begin nco_inq_attname() overloads

int // O [enm] Return success code
nco_inq_attname // [fnc] Inquire attribute name
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const int &att_id, // I [id] Attribute ID
 std::string &att_nm, // O [sng] Attribute name
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_attname() prototype 

std::string // O [sng] Attribute name
nco_inq_attname // [fnc] Inquire attribute name
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const int &att_id); // I [id] Attribute ID
// end nco_inq_attname() prototype 

std::string // O [sng] Attribute name
nco_inq_attname // [fnc] Inquire attribute name
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const int &att_id); // I [id] Attribute ID
// end nco_inq_attname() prototype 

// End nco_inq_attname() overloads
// Begin nco_inq_atttype() overloads

int // O [enm] Return success code
nco_inq_atttype // [fnc] Inquire attribute type
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 nc_type &att_typ, // O [enm] Variable type
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_atttype() prototype 

nc_type // O [enm] Variable type
nco_inq_atttype // [fnc] Inquire variable type
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm); // I [sng] Attribute name
// end nco_inq_atttype() prototype 

// End nco_inq_atttype() overloads
// Begin nco_inq_attlen() overloads

int // O [enm] Return success code
nco_inq_attlen // [fnc] Inquire attribute length
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 size_t &att_sz, // O [nbr] Attribute size
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_attlen() prototype 

size_t // O [nbr] Attribute size
nco_inq_attlen // [fnc] Inquire attribute length
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm); // I [sng] Attribute name
// end nco_inq_attlen() prototype 

size_t // O [nbr] Attribute size
nco_inq_attlen // [fnc] Inquire attribute length
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const int &att_id); // I [id] Attribute ID
// end nco_inq_attlen() prototype 

size_t // O [nbr] Attribute size
nco_inq_attlen // [fnc] Inquire attribute length
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm); // I [sng] Attribute name
// end nco_inq_attlen() prototype 

// End nco_inq_attlen() overloads
// Begin nco_get_att() overloads

int // O [enm] Return success code
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 std::string &att_val); // O [frc] Attribute value
// end nco_get_att<std::string>() prototype

int // O [enm] Return success code
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 float *&att_val); // O [frc] Attribute value
// end nco_get_att<float>() prototype

int // O [enm] Return success code
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 double *&att_val); // O [frc] Attribute value
// end nco_get_att<double>() prototype

template<class typ_ntr>
int // O [enm] Return success code
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm, // I [sng] Attribute name
 typ_ntr &att_val); // O [frc] Attribute value
// end nco_get_att<typ_ntr>() prototype
template<class typ_ntr>
int // O [enm] Return success code
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm, // I [sng] Attribute name
 typ_ntr &att_val) // O [frc] Attribute value
{
  /* 20050109 Unless nco_inq_varid() prototype appears here, g++ 3.4 produces
     "./nco_att.hh:369: error: there are no arguments to `nco_inq_varid' that depend on a template parameter, so a declaration of `nco_inq_varid' must be available
     ./nco_att.hh:369: error: (if you use `-fpermissive', G++ will accept your code, but allowing the use of an undeclared name is deprecated)"
     when compiling nco_var.cc
     Unclear why this nco_inq_varid() template must be here and in nco_var.hh 
     Probably g++ 3.4 is finicky and does not allow circular prototype references
     Eliminating this additional nco_inq_varid() reference may require adding 
     .hh files to eliminate circular references.
     Hence must weigh complexity of multiple (three) nco_inq_varid() prototypes
     (master prototype in nco_var.hh and two local prototypes in overloaded 
     nco_get_att() functions in nco_att.hh) against complexity of finer-grained
     (more) .hh files. 
     On balance, I prefer fewer files
     There are currently only two redundant nco_inq_varid() prototypes
     If this number grows appreciably then we should re-visit this decision */ 

  int // O [id] Variable ID
    nco_inq_varid // [fnc] Inquire variable ID
    (const int &nc_id, // I [enm] netCDF file ID
     const std::string &var_nm); // I [sng] Variable name
  
  // Purpose: Wrapper for nc_get_att()
  int rcd=nco_get_att(nc_id,nco_inq_varid(nc_id,var_nm),att_nm,att_val);
  return rcd;
} // end nco_get_att<typ_ntr>()

template<class typ_ntr>
typ_ntr // O [frc] Attribute value
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm); // I [sng] Attribute name
// end nco_get_att<typ_ntr>() prototype
template<class typ_ntr>
typ_ntr // O [frc] Attribute value
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm) // I [sng] Attribute name
{
  // Purpose: Wrapper for nco_get_att()
  typ_ntr att_val; // O [frc] Attribute value
  int rcd=nco_get_att(nc_id,var_id,att_nm,att_val);
  rcd+=0; // 20140531 CEWI to prevent -Wunused-variable
  return att_val;
} // end nco_get_att<typ_ntr>()

template<class typ_ntr>
typ_ntr // O [frc] Attribute value
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm); // I [sng] Attribute name
// end nco_get_att<typ_ntr>() prototype
template<class typ_ntr>
typ_ntr // O [frc] Attribute value
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm) // I [sng] Attribute name
{
  // Purpose: Wrapper for nco_get_att()

  // 20050109 Prototype nco_inq_varid() to satisfy g++ 3.4 finickiness described above
  int // O [id] Variable ID
    nco_inq_varid // [fnc] Inquire variable ID
    (const int &nc_id, // I [enm] netCDF file ID
     const std::string &var_nm); // I [sng] Variable name

  typ_ntr att_val; // O [frc] Attribute value
  int rcd=nco_get_att(nc_id,nco_inq_varid(nc_id,var_nm),att_nm,att_val);
  rcd+=0; // 20140531 CEWI to prevent -Wunused-variable
  return att_val;
} // end nco_get_att<typ_ntr>()

// End nco_get_att() overloads

// Define inline'd functions in header so source is visible to calling files

#endif // NCO_ATT_HH  






