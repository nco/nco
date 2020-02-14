// $Header$ 

// Purpose: Description (definition) of C++ interface to netCDF dimension routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

// Usage:
// #include <nco_dmn.hh> // C++ interface to netCDF dimension routines

#ifndef NCO_DMN_HH // Contents have not yet been inserted in current source file
#define NCO_DMN_HH

// C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <string> // Standard C++ string class

// Standard C headers

// 3rd party vendors
#include <netcdf.h> // netCDF C interface

// Personal headers
#include <nco_utl.hh> // C++ interface utilities for netCDF routines

// Typedefs

// Define nco_cls class

// Prototype global functions with C++ linkages

// Begin nco_def_dim() overloads

int // O [enm] Return success code
nco_def_dim // [fnc] Create dimension in netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm, // I [sng] Dimension name
 const size_t &dmn_sz, // I [nbr] Dimension size
 int &dmn_id); // O [id] Dimension ID
// end nco_def_dim() prototype

int // O [id] Dimension ID
nco_def_dim // [fnc] Create dimension in netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm, // I [sng] Dimension name
 const size_t &dmn_sz); // I [nbr] Dimension size
// end nco_def_dim() prototype

// End nco_def_dim() overloads

int // O [enm] Return success code
nco_inq_dim // [fnc] Inquire dimension
(const int &nc_id, // I [enm] netCDF file ID
 const int &dmn_id, // I [id] Dimension ID
 std::string &dmn_nm, // O [sng] Dimension name
 size_t &dmn_sz, // O [nbr] Dimension size
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_dim() prototype

// Begin nco_inq_dimid() overloads

int // O [enm] Return success code
nco_inq_dimid // [fnc] Inquire dimension ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm, // I [sng] Dimension name
 int &dmn_id, // O [id] Dimension ID
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_dimid() prototype

int // O [id] Dimension ID
nco_inq_dimid // [fnc] Inquire dimension ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm); // I [sng] Dimension name
// end nco_inq_dimid() prototype

// End nco_inq_dimid() overloads
// Begin nco_inq_dimlen() overloads

int // O [enm] Return success code
nco_inq_dimlen // [fnc] Inquire dimension length
(const int &nc_id, // I [enm] netCDF file ID
 const int &dmn_id, // I [id] Dimension ID
 size_t &dmn_sz, // O [nbr] Dimension size
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_dimlen() prototype

size_t // O [nbr] Dimension size
nco_inq_dimlen // [fnc] Inquire dimension length
(const int &nc_id, // I [enm] netCDF file ID
 const int &dmn_id); // I [id] Dimension ID
// end nco_inq_dimlen() prototype

size_t // O [nbr] Dimension size
nco_inq_dimlen // [fnc] Inquire dimension length
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm); // I [sng] Dimension name
// end nco_inq_dimlen() prototype

// End nco_inq_dimlen() overloads

// Define inline'd functions in header so source is visible to calling files

#endif // NCO_DMN_HH  






