// $Header$ 

// Purpose: Prototypes, typedefs, and global variables for libnco++

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* libnco++.hh headers depend on system headers and libnco.a headers
   Thus libnco++.a requires libnco.a to function correctly
   User must also supply nco_prg_nm_get() and nco_dbg_lvl_get() */

// Usage:
// #include <libnco++.hh> // C++ interface to netCDF C library

#ifndef LIBNCOXX_HH // Contents have not yet been inserted in current source file  
#define LIBNCOXX_HH

// Personal headers
#include "sym_cls.hh"
#include "fmc_cls.hh"
#include "fmc_all_cls.hh"
#include "nco_gsl.hh"
#ifdef ENABLE_GSL
# include "fmc_gsl_cls.hh"
#endif // !ENABLE_GSL
#include "NcapVar.hh"
#include "NcapVarVector.hh"
#include "ncap2_utl.hh"
#include "ncap2_att.hh"
#include "prs_cls.hh"
#include "sdo_utl.hh"
#include "ncap2_utl.hh"

//#include "ncoLexer.hpp" // 
//#include "ncoParser.hpp" //
//#include "ncoTree.hpp" // 
//#include "NcapVarVector.hh" // 
//#include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()

#endif // LIBNCOXX_HH  
