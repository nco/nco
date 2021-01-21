// $Header$ 

// Purpose: Description (definition) of SDO stand-alone utilities

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

// Source: Routines are subset of CSZ's ~/c++/utl.[cc/hh]

// Usage:
// #include "sdo_utl.hh" // SDO stand-alone utilities: dbg/err/wrn_prn()

#ifndef SDO_UTL_HH // Contents have not yet been inserted in current source file  
#define SDO_UTL_HH

// C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <sstream> // Standard C++ string stream processing
#include <string> // Standard C++ string class

// Standard C headers
#include <cassert> // Assertions
#include <cstdlib> // abort, exit, getopt, malloc, strtod, strtol

// 3rd party vendors

// Personal headers
//#include <sdo_dbg.hh> // Debugging constants

// Namespaces

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#endif // EXIT_SUCCESS is not defined in SUN4

// Prototype functions that have C linkages
extern "C" {
  char *nco_prg_nm_get(void);
  unsigned short nco_dbg_lvl_get(void);
} // end extern C

// Define inline'd functions in header so source is visible to calling files

// Prototype functions with C++ linkages
void dbg_prn(std::string msg);
void dbg_prn(std::string fnc_nm,std::string msg);
void dbg_prn(std::string nco_prg_nm,std::string fnc_nm,std::string msg);
void err_prn(std::string msg);
void err_prn(std::string fnc_nm,std::string msg);
void err_prn(std::string nco_prg_nm,std::string fnc_nm,std::string msg);
void nfo_prn(std::string fnc_nm,std::string msg);
void nfo_prn(std::string nco_prg_nm,std::string fnc_nm,std::string msg);
void wrn_prn(std::string fnc_nm,std::string msg);
void wrn_prn(std::string nco_prg_nm,std::string fnc_nm,std::string msg);

// Templates
template<class val_T>std::string nbr2sng(const val_T nbr); // O [sng] Number stored as string
template<class val_T> // [obj] Object type
std::string // [sng] Number stored as string
  nbr2sng // [fnc] Convert number to string
  (const val_T val) // [frc] Number to convert to string
{
  /* Purpose: Convert number to string
     Method taken from CUED C++ FAQ
     NB: String streams require GCC 3.x */
  const std::string fnc_nm("nbr2sng"); // [sng] Subroutine name
  std::ostringstream sng_srm_out; // [srm] Output string stream
  if(sng_srm_out << val) return sng_srm_out.str(); 
  // If control reaches this point then error was encountered
  err_prn(fnc_nm,"Unable to convert number to string");
  // Need return value here to avoid compiler warnings
  return sng_srm_out.str(); // 
} // end nbr2sng()

template<class val_T>std::string nbr2sng(const val_T nbr,int dcm_plc_prc); // O [sng] Number stored as string
template<class val_T> // [obj] Object type
std::string // [sng] Number stored as string
  nbr2sng // [fnc] Convert number to string
  (const val_T val, // I [frc] Number to convert to string
   const int dcm_plc_prc) // I [nbr] Decimal places of precision
{
  /* Purpose: Convert number to string
     Method taken from CUED C++ FAQ
     NB: String streams require GCC 3.x */
  const std::string fnc_nm("nbr2sng"); // [sng] Subroutine name
  std::ostringstream sng_srm_out; // [srm] Output string stream
  sng_srm_out.precision(dcm_plc_prc);
  if(sng_srm_out << val) return sng_srm_out.str(); 
  // If control reaches this point then error was encountered
  err_prn(fnc_nm,"Unable to convert number to string");
  // Need return value here to avoid compiler warnings
  return sng_srm_out.str(); // 
} // end nbr2sng()

template<class val_T>void sng2nbr(const std::string sng,val_T *nbr); // O [sng] String stored as number
template<class val_T> // [obj] Object type
void
sng2nbr // [fnc] Convert string to number
(const std::string sng, // I [frc] String to convert to number
 val_T *nbr) // O [frc] Number that was converted from string
{
  /* Purpose: Convert string to number
     Method inverse of nbr2sng() (see below)
     This method suggested by Martin York in
     comp.lang.c++.moderated on 20050522 in response to my thread on 
     "Cross-platform strtoll() functionality" */
  const std::string sbr_nm("sng2nbr"); // [sng] Subroutine name
  std::stringstream sng_srm_in(sng); // [srm] Input string stream
  if(!(sng_srm_in >> *nbr)) err_prn(sbr_nm,"Unable to convert string \""+sng+"\" to number");
} // end sng2nbr()

template<class val_T>val_T sng2nbr(const std::string sng,const val_T nbr); // O [sng] String stored as number
template<class val_T> // [obj] Object type
val_T // O [sng] String stored as number
sng2nbr // [fnc] Convert string to number
(const std::string sng, // I [frc] String to convert to number
 const val_T nbr) // I [frc] Number of type to convert to (not touched)
{
  /* Purpose: Convert string to number
     Method inverse of nbr2sng() (see below)
     This method suggested by Martin York in
     comp.lang.c++.moderated on 20050522 in response to my thread on 
     "Cross-platform strtoll() functionality" */
  val_T val_out; // O [nbr] String stored as number
  const std::string sbr_nm("sng2nbr"); // [sng] Subroutine name
  std::stringstream sng_srm_in(sng); // [srm] Input string stream
  if(sng_srm_in >> val_out) return val_out; else err_prn(sbr_nm,"Unable to convert string \""+sng+"\" to number");
  // Redundant return value here avoids two compiler warnings
  return nbr; // O [nbr] String stored as number
} // end sng2nbr()

#endif // SDO_UTL_HH  
