// $Header$ 

// Implementation (declaration) of C++ interface to netCDF dimension routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include <nco_dmn.hh> // C++ interface to netCDF dimension routines

// netCDF classes

// Friendly functions begin

// Friendly functions end
// Static members begin

// Static members end
// Static member functions begin

// Static member functions end
// Public member functions begin

// Public member functions end
// Private member functions begin

// Private member functions end
// Global functions with C++ linkages begin

// Begin nco_def_dim() overloads

int // O [enm] Return success code
nco_def_dim // [fnc] Create dimension in netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm, // I [sng] Dimension name
 const size_t &dmn_sz, // I [nbr] Dimension size
 int &dmn_id) // O [id] Dimension ID
{
  /* Purpose: Wrapper for nc_def_dim() */
  int rcd=nc_def_dim(nc_id,dmn_nm.c_str(),dmn_sz,&dmn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_dim");
  return rcd;
} /* end nco_def_dim */

int // O [id] Dimension ID
nco_def_dim // [fnc] Create dimension in netCDF file
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm, // I [sng] Dimension name
 const size_t &dmn_sz) // I [nbr] Dimension size
{
  /* Purpose: Wrapper for nc_def_dim() */
  int dmn_id; // O [id] Dimension ID
  int rcd=nco_def_dim(nc_id,dmn_nm,dmn_sz,dmn_id);
  rcd+=0; // CEWI
  return dmn_id;
} /* end nco_def_dim */

// End nco_def_dim() overloads

int // O [enm] Return success code
nco_inq_dim // [fnc] Inquire dimension
(const int &nc_id, // I [enm] netCDF file ID
 const int &dmn_id, // I [id] Dimension ID
 std::string &dmn_nm, // O [sng] Dimension name
 size_t &dmn_sz, // O [nbr] Dimension size
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_dim()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  char dmn_nm_chr[NC_MAX_NAME];
  int rcd=nc_inq_dim(nc_id,dmn_id,dmn_nm_chr,&dmn_sz);
  dmn_nm=dmn_nm_chr;
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_dim");
  return rcd;
} // end nco_inq_dim() 

// Begin nco_inq_dimid() overloads

int // O [enm] Return success code
nco_inq_dimid // [fnc] Inquire dimension ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm, // I [sng] Dimension name
 int &dmn_id, // O [id] Dimension ID
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_dimid()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_dimid(nc_id,dmn_nm.c_str(),&dmn_id);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_dimid","Unable to to find dimension ID for dimension \""+dmn_nm+"\", exiting...");
  return rcd;
} // end nco_inq_dimid() 

int // O [id] Dimension ID
nco_inq_dimid // [fnc] Inquire dimension ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm) // I [sng] Dimension name
{
  /* Purpose: Wrapper for nco_inq_dimid()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int dmn_id; // O [id] Dimension ID
  int rcd=nco_inq_dimid(nc_id,dmn_nm,dmn_id);
  rcd+=0; // CEWI
  return dmn_id;
} // end nco_inq_dimid() 

// End nco_inq_dimid() overloads
// Begin nco_inq_dimlen() overloads

int // O [enm] Return success code
nco_inq_dimlen // [fnc] Inquire dimension length
(const int &nc_id, // I [enm] netCDF file ID
 const int &dmn_id, // I [id] Dimension ID
 size_t &dmn_sz, // O [nbr] Dimension size
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_dimlen()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_dimlen(nc_id,dmn_id,&dmn_sz);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_dimlen");
  return rcd;
} // end nco_inq_dimlen() 

size_t // O [nbr] Dimension size
nco_inq_dimlen // [fnc] Inquire dimension length
(const int &nc_id, // I [enm] netCDF file ID
 const int &dmn_id) // I [id] Dimension ID
{
  // Purpose: Wrapper for nco_inq_dimlen()
  size_t dmn_sz; // O [nbr] Dimension size
  int rcd=nco_inq_dimlen(nc_id,dmn_id,dmn_sz);
  rcd+=0; // CEWI
  return dmn_sz;
} // end nco_inq_dimlen() 

size_t // O [nbr] Dimension size
nco_inq_dimlen // [fnc] Inquire dimension length
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &dmn_nm) // I [sng] Dimension name
{
  // Purpose: Wrapper for nco_inq_dimlen(nc_inq_dimid())
  size_t dmn_sz; // O [nbr] Dimension size
  int rcd=nco_inq_dimlen(nc_id,nco_inq_dimid(nc_id,dmn_nm),dmn_sz);
  rcd+=0; // CEWI
  return dmn_sz;
} // end nco_inq_dimlen() 

// End nco_inq_dimlen() overloads

// Global functions with C++ linkages end
