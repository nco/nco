// $Header: /data/zender/nco_20150216/nco/src/nco_c++/libnco_c++.hh,v 1.14 2010-01-05 20:02:18 zender Exp $ 

// Purpose: Prototypes, typedefs, and global variables for libnco_c++

/* Copyright (C) 2001--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* libnco_c++.hh headers depend only on system headers
   Thus libnco_c++.a may be built independently of any other libraries
   However, user must supply prg_nm_get() and dbg_lvl_get() */

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
