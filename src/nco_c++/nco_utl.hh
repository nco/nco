// $Header: /data/zender/nco_20150216/nco/src/nco_c++/nco_utl.hh,v 1.1 2002-01-11 23:20:29 zender Exp $ 

// Purpose: Description (definition) of C++ interface utilities for netCDF routines

// Usage:
// #include <nco_utl.hh> // C++ interface utilities for netCDF routines

#ifndef NCO_UTL_HH // Contents have not yet been inserted in current source file
#define NCO_UTL_HH

// C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <string> // Standard C++ string class

// Standard C headers

// 3rd party vendors
#include <netcdf.h> // netCDF C interface

// Personal headers
#include <nco_att.hh> // C++ interface to netCDF attribute routines
#include <nco_dmn.hh> // C++ interface to netCDF dimension routines
#include <nco_var.hh> // C++ interface to netCDF variable routines

// Define nco_cls class

// Prototype global functions with C++ linkages

void 
nco_err_exit // [fnc] Lookup, print netCDF error message, exit
(const int &rcd, // I [enm] netCDF error code
 const std::string &msg, // I [sng] supplemental error message
 const std::string &msg_opt=""); // I [sng] Optional supplemental error message
// end nco_err_exit() prototype

// Begin nco_inq_varsz() overloads

int // O [enm] Return success code
nco_inq_varsz // [fnc] Compute size of variable
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 size_t &var_sz); // O [nbr] Variable size
// end nco_inq_varsz() prototype

size_t // O [nbr] Variable size
nco_inq_varsz // [fnc] Compute size of variable
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id); // I [id] Variable ID
// end nco_inq_varsz() prototype

// End nco_inq_varsz() overloads

int // O [enm] Return success code
nco_inq_varsrt // [fnc] Inquire variable dimension IDS
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 std::valarray<size_t> &srt, // O [idx] Starting indices
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_varsrt() prototype

int // [nbr] Internal (native) size (Bytes) of netCDF external type
nco_typ_lng // [fnc] Internal (native) size (Bytes) of netCDF external type
(const nc_type &nco_typ); // I [enm] netCDF external type
// end nco_typ_lng() prototype

std::string // O [sng] String version of netCDF external type enum
nco_typ_sng // [fnc] String version of netCDF external type enum
(const nc_type &nco_typ); // I [enm] netCDF external type
// end nco_typ_sng() prototype

std::string // O [sng] String version of C++ internal type for storing netCDF external type enum
nco_c_typ_sng // [fnc] String version of C++ internal type for storing netCDF external type enum
(const nc_type &nco_typ); // I [enm] netCDF external type
// end nco_c_typ_sng() prototype

std::string // O [sng] String version of C++ internal type for storing netCDF external type enum
nco_ftn_typ_sng // [fnc] String version of C++ internal type for storing netCDF external type enum
(const nc_type &nco_typ); // I [enm] netCDF external type
// end nco_ftn_typ_sng() prototype

void 
nco_dfl_case_nctype_err(void); // [fnc] Handle illegal nc_type references
// end nco_dfl_case_nctype_err() prototype

// Begin nco_get_xtype() overloads

nc_type // O [enm] External netCDF type
nco_get_xtype // [fnc] Determine external netCDF type
(const float &var_val); // I [frc] Variable value
// end nco_get_xtype<float>() prototype

nc_type // O [enm] External netCDF type
nco_get_xtype // [fnc] Determine external netCDF type
(const double &var_val); // I [frc] Variable value
// end nco_get_xtype<double>() prototype

// End nco_get_xtype() overloads

// Define inline'd functions in header so source is visible to calling files

#endif // NCO_UTL_HH  






