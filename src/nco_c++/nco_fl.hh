// $Header$ 

// Purpose: Description (definition) of C++ interface to netCDF file-level routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

// Usage:
// #include <nco_fl.hh> // C++ interface to netCDF file-level routines

#ifndef NCO_FL_HH // Contents have not yet been inserted in current source file
#define NCO_FL_HH

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

// Begin nco_create() overloads

int // O [enm] Return success code
nco_create // [fnc] Create netCDF file
(const std::string &fl_nm, // I [sng] File name
 const int &cmode, // I [enm] File create mode
 int &nc_id); // O [id] netCDF file ID
// end nco_create() prototype

int // O [id] netCDF file ID
nco_create // [fnc] Create netCDF file
(const std::string &fl_nm, // I [sng] File name
 const int &cmode); // I [enm] File create mode
// end nco_create() prototype

// End nco_create() overloads
// Begin nco_open() overloads

int // O [enm] Return success code
nco_open // [fnc] Open netCDF file
(const std::string &fl_nm, // I [sng] File name
 const int &cmode, // I [enm] File open mode
 int &nc_id); // O [id] netCDF file ID
// end nco_open() prototype

int // O [id] netCDF file ID
nco_open // [fnc] Open netCDF file
(const std::string &fl_nm, // I [sng] File name
 const int &cmode); // I [enm] File open mode
// end nco_open() prototype

// End nco_open() overloads

int // O [enm] Return success code
nco_inq // [fnc] Inquire file
(const int &nc_id, // I [enm] netCDF file ID
 int &dmn_nbr, // O [nbr] Number of dimensions
 int &var_nbr, // O [nbr] Number of variables
 int &att_nbr, // O [nbr] Number of global attributes
 int &rec_dmn_id, // O [id] Record dimension ID
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq() prototype

// Begin nco_inq_ndims() overloads

int // O [enm] Return success code
nco_inq_ndims // [fnc] Inquire file rank
(const int &nc_id, // I [enm] netCDF file ID
 int &dmn_nbr, // O [nbr] Number of dimensions
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_ndims() prototype

int // O [nbr] Number of dimensions
nco_inq_ndims // [fnc] Inquire file dimensions
(const int &nc_id); // I [enm] netCDF file ID
// end nco_inq_ndims() prototype

// End nco_inq_ndims() overloads
// Begin nco_inq_nvars() overloads

int // O [enm] Return success code
nco_inq_nvars // [fnc] Inquire file variables
(const int &nc_id, // I [enm] netCDF file ID
 int &var_nbr, // O [nbr] Number of variables
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_nvars() prototype

int // O [nbr] Number of variables
nco_inq_nvars // [fnc] Inquire file variables
(const int &nc_id); // I [enm] netCDF file ID

// End nco_inq_nvars() overloads

int // O [enm] Return success code
nco_inq_natts // [fnc] Inquire file attributes
(const int &nc_id, // I [enm] netCDF file ID
 int &att_nbr, // O [nbr] Number of attributes
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_natts() prototype

int // O [enm] Return success code
nco_inq_unlimdim // [fnc] Inquire file record ID
(const int &nc_id, // I [enm] netCDF file ID
 int &dmn_id, // O [id] Record dimension ID
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_inq_unlimdim() prototype

int // O [enm] Return success code
nco_redef // [fnc] Put open netCDF dataset into define mode
(const int &nc_id, // I [id] netCDF file ID
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_redef() prototype

int // O [enm] Return success code
nco_enddef // [fnc] Leave define mode
(const int &nc_id, // I [id] netCDF file ID
 const int &rcd_opt=NC_NOERR); // I [enm] Optional non-fatal return code 
// end nco_enddef()

int // O [enm] Return success code
nco_set_fill // [fnc] Set fill mode for writes
(const int &nc_id, // I [id] netCDF file ID
 const int &fillmode, // I [enm] New fill mode
 int &oldmode); // O [enm] Old fill mode
// end nco_set_fill() prototype

int // O [enm] Return success code
nco_close // [fnc] Close netCDF file
(const int &nc_id); // I [id] netCDF file ID
// end nco_close() prototype

// Define inline'd functions in header so source is visible to calling files

#endif // NCO_FL_HH  






