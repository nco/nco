// $Header: /data/zender/nco_20150216/nco/src/nco_c++/nco_utl.hh,v 1.17 2008-01-11 22:27:43 zender Exp $ 

// Purpose: Description (definition) of C++ interface utilities for netCDF routines

/* Copyright (C) 2001--2008 Charlie Zender
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

// Usage:
// #include <nco_utl.hh> // C++ interface utilities for netCDF routines

#ifndef NCO_UTL_HH // Contents have not yet been inserted in current source file
#define NCO_UTL_HH

#ifdef HAVE_CONFIG_H
# include <config.h> // Autotools tokens
#endif // !HAVE_CONFIG_H

// C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <string> // Standard C++ string class

// Standard C headers
#ifdef HAVE_CSTDLIB
#include <cstdlib> // abort, exit, getopt, malloc, strtod, strtol
#else // The only C++ compiler known to lack <cstdlib> is SGI's CC
#include <stdlib.h> // abort, exit, getopt, malloc, strtod, strtol
#endif // !HAVE_CSTDLIB

// 3rd party vendors
#include <netcdf.h> // netCDF C interface

// Library-wide headers
/* All headers in libnco_c++ include nco_utl.hh
   Hence nco_utl.hh is the appropriate place to define library-wide variables
   fxm: Make this extern and instantiate in .cc file? */
const int NCO_NOERR=NC_NOERR; // [enm] Variable'ize CPP macro for use in function parameter initialization

#ifndef NCO_FORMAT_UNDEFINED
  /* netcdf.h defines four NC_FORMAT tokens: NC_FORMAT_CLASSIC, ...
     The values are (currently) enumerated from one to four
     Operators need to check if fl_out_fmt has been user-specified
     Saftest way is to compare current value of fl_out_fmt to initial value 
     Initial value should be a number that will never be a true netCDF format */
# define NCO_FORMAT_UNDEFINED 0
#endif // NCO_FORMAT_UNDEFINED

// Same debugging types as in nco.h
enum nco_dbg_typ_enm{ /* [enm] Debugging levels */
  /* List in increasing levels of verbosity */
  nco_dbg_quiet, /* Quiet all non-error messages. */
  nco_dbg_std, /* Standard mode. Minimal, but some, messages. */
  nco_dbg_fl, /* Filenames */
  nco_dbg_scl, /* Scalars, other per-file information  */
  nco_dbg_var, /* Variables, highest level per-file loop information */
  nco_dbg_crr, /* Current task */
  nco_dbg_sbr, /* Subroutine names on entry and exit */
  nco_dbg_io, /* Subroutine I/O */
  nco_dbg_vec, /* Entire vectors */
  nco_dbg_vrb, /* Verbose, print everything possible */
  nco_dbg_old, /* Old debugging blocks not used anymore */
  nco_dbg_nbr /* Number of debugging types. Should be last enumerated value. */
  }; /* end nco_dbg_typ_enm */

// Personal headers
#include <nco_att.hh> // C++ interface to netCDF attribute routines
#include <nco_dmn.hh> // C++ interface to netCDF dimension routines
#include <nco_var.hh> // C++ interface to netCDF variable routines

// Typedefs

// Define nco_cls class

// Prototype global functions with C++ linkages

int // [rcd] Return code
nco_create_mode_prs // [fnc] Parse user-specified file format
(const std::string fl_fmt_sng, // I [sng] User-specified file format string
 int &fl_fmt_enm); // O [enm] Output file format
// end nco_create_mode_prs() prototype

void 
nco_err_exit // [fnc] Lookup, print netCDF error message, exit
(const int &rcd, // I [enm] netCDF error code
 const std::string &msg, // I [sng] supplemental error message
 const std::string &msg_opt=""); // I [sng] Optional supplemental error message
// end nco_err_exit() prototype

void 
nco_err_exit // [fnc] Print error message, exit
(const std::string &sbr_nm, // I [sng] Subroutine name
 const std::string &msg); // I [sng] Error message
// end nco_err_exit() prototype

void 
nco_wrn_prn // [fnc] Print NCO warning message and return
(const std::string &msg, // I [sng] Supplemental warning message
 const std::string &msg_opt=""); // I [sng] Optional supplemental warning message

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






