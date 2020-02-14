// $Header$ 

// Purpose: Description (definition) of high-level NCO routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

// Usage:
// #include <nco_hgh.hh> // High-level NCO routines

#ifndef NCO_HGH_HH // Contents have not yet been inserted in current source file
#define NCO_HGH_HH

// C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <string> // Standard C++ string class

// Standard C headers

// 3rd party vendors
#include <netcdf.h> // netCDF C interface

// Personal headers
#include <nco_fl.hh> // C++ interface to netCDF file-level routines
#include <nco_utl.hh> // C++ interface utilities for netCDF routines

// Typedefs
typedef struct{ // [sct] var_mtd_sct Structure for variable metadata
  int var_id; // [id] Variable ID
  std::string nm; // [sng] Variable name
  nc_type type; // [enm] netCDF type
  int dmn_nbr; // [nbr] Number of dimensions
  const int *dmn_id; // [dmn] Dimension list 
  std::string att_1_nm; // [sng] Attribute name
  std::string att_1_val; // [sng] Attribute value
  std::string att_2_nm; // [sng] Attribute name
  std::string att_2_val; // [sng] Attribute value
} var_mtd_sct; // [sct] Structure for variable metadata

// Define nco_cls class

// Prototype global functions with C++ linkages

int // O [enm] Return success code
nco_var_dfn // [fnc] Define variables in output netCDF file
(const int &nc_id, // I [enm] netCDF output file ID
 var_mtd_sct *var_mtd, // I/O [sct] Array of structures containing variable metadata
 const int &var_mtd_nbr, // I [nbr] Number of variables in array
 const int &dmn_nbr_max=10); // I [nbr] Maximum number of dimensions allowed in single variable in output file
// end nco_var_dfn() prototype

// Define inline'd functions in header so source is visible to calling files

#endif // NCO_HGH_HH  






