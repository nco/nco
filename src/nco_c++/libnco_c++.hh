// $Header$ 

// Purpose: Prototypes, typedefs, and global variables for libnco_c++

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* libnco_c++.hh headers depend only on system headers
   Thus libnco_c++.a may be built independently of any other libraries
   However, user must supply nco_prg_nm_get() and nco_dbg_lvl_get() */

// Usage:
// #include <libnco_c++.hh> // C++ interface to netCDF C library

#ifndef LIBNCO_CXX_HH // Contents have not yet been inserted in current source file  
#define LIBNCO_CXX_HH

// Personal headers
#include <nco_att.hh> // C++ interface to netCDF attribute routines
#include <nco_dmn.hh> // C++ interface to netCDF dimension routines
#include <nco_fl.hh> // C++ interface to netCDF file-level routines
#include <nco_hgh.hh> // High-level NCO utilities
#include <nco_utl.hh> // C++ interface utilities for netCDF routines
#include <nco_var.hh> // C++ interface to netCDF variable routines

#endif // LIBNCO_CXX_HH  
