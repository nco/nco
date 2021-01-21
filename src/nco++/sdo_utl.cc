// $Header$ 

// Purpose: Implementation (declaration) of SDO stand-alone utilities 

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()

// Global functions with C linkages begin

// Declare global functions with C++ linkages

void 
dbg_prn(std::string nco_prg_nm,std::string fnc_nm,std::string msg) // [fnc] Print uniform debugging message
{
  // Purpose: Print a uniform debugging message
  std::cerr << nco_prg_nm << ": DEBUG " << fnc_nm << "(): "+msg << std::endl;
} // end dbg_prn()

void 
dbg_prn(std::string fnc_nm,std::string msg) // [fnc] Print uniform debugging message
{
  // Purpose: Print a uniform debugging message
  std::cerr << nco_prg_nm_get() << ": DEBUG " << fnc_nm << "(): "+msg << std::endl;
} // end dbg_prn()

void 
dbg_prn(std::string msg) // [fnc] Print uniform debugging message
{
  // Purpose: Print a uniform debugging message
  std::cerr << "DEBUG: "+msg << std::endl;
} // end dbg_prn()

void 
err_prn(std::string nco_prg_nm,std::string fnc_nm,std::string msg) // [fnc] Print uniform error message and exit 
{
  // Purpose: Print a uniform error message and exit 
  std::cerr << nco_prg_nm << ": ERROR " << fnc_nm << "(): "+msg << std::endl;
#ifdef ABORT_ON_ERROR
  // abort() produces a core dump and traceback information useful to debuggers
  std::abort(); // [fnc] Exit with core dump
#else // !ABORT_ON_ERROR
  // exit() produces no core dump or useful debugger information
  std::exit(EXIT_FAILURE); // [fnc] Exit nicely
#endif // !ABORT_ON_ERROR
} // end err_prn()

void 
err_prn(std::string fnc_nm,std::string msg) // [fnc] Print uniform error message and exit 
{
  // Purpose: Print a uniform error message and exit 
  std::cerr << nco_prg_nm_get() << ": ERROR " << fnc_nm << "(): "+msg << std::endl;
#ifdef ABORT_ON_ERROR
  std::abort(); // [fnc] Produce core dump
#else
  std::exit(EXIT_FAILURE); // [fnc] Exit nicely
#endif // !ABORT_ON_ERROR
} // end err_prn()

void 
err_prn(std::string msg) // [fnc] Print uniform error message and exit
{
  // Purpose: Print uniform error message and exit 
  std::cerr << msg << std::endl;
#ifdef ABORT_ON_ERROR
  std::abort(); // [fnc] Produce core dump
#else
  std::exit(EXIT_FAILURE); // [fnc] Exit nicely
#endif // !ABORT_ON_ERROR
} // end err_prn()

void 
wrn_prn(std::string nco_prg_nm,std::string fnc_nm,std::string msg) // [fnc] Print uniform warning message
{
  // Purpose: Print a uniform warning message
  std::cerr << nco_prg_nm << ": WARNING " << fnc_nm << "(): "+msg << std::endl;
} // end wrn_prn()

void 
wrn_prn(std::string fnc_nm,std::string msg) // [fnc] Print uniform warning message
{
  // Purpose: Print a uniform warning message
  std::cerr << nco_prg_nm_get() << ": WARNING " << fnc_nm << "(): "+msg << std::endl;
} // end wrn_prn()

void 
nfo_prn(std::string nco_prg_nm,std::string fnc_nm,std::string msg) // [fnc] Print uniform informational message
{
  // Purpose: Print a uniform informational message
  std::cerr << nco_prg_nm << ": INFO " << fnc_nm << "(): "+msg << std::endl;
} // end nfo_prn()

void 
nfo_prn(std::string fnc_nm,std::string msg) // [fnc] Print uniform informational message
{
  // Purpose: Print a uniform informational message
  std::cerr << nco_prg_nm_get() << ": INFO " << fnc_nm << "(): "+msg << std::endl;
} // end nfo_prn()

