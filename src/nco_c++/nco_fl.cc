// $Header$ 

// Purpose: Implementation (declaration) of C++ interface to netCDF file-level routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include <nco_fl.hh> // C++ interface to netCDF file-level routines

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

// Begin nco_create() overloads

int // O [enm] Return success code
nco_create // [fnc] Create netCDF file
(const std::string &fl_nm, // I [sng] File name
 const int &cmode, // I [enm] File create mode
 int &nc_id) // O [id] netCDF file ID
{
  /* Purpose: Wrapper for nc_create() */
  int rcd=nc_create(fl_nm.c_str(),cmode,&nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_create","Failed to create "+fl_nm);
  return rcd;
} // end nco_create()

int // O [id] netCDF file ID
nco_create // [fnc] Create netCDF file
(const std::string &fl_nm, // I [sng] File name
 const int &cmode) // I [enm] File open mode
{
  /* Purpose: Wrapper for nc_create() 
     Usage: nc_id=nco_create(fl_nm,NC_NOWRITE); */
  int nc_id; // O [id] netCDF file ID
  int rcd=nco_create(fl_nm,cmode,nc_id);
  rcd+=0; // CEWI
  return nc_id;
} // end nco_create()

// End nco_create() overloads
// Begin nco_open() overloads

int // O [enm] Return success code
nco_open // [fnc] Open netCDF file
(const std::string &fl_nm, // I [sng] File name
 const int &cmode, // I [enm] File open mode
 int &nc_id) // O [id] netCDF file ID
{
  /* Purpose: Wrapper for nc_open() 
     Usage: rcd=nco_open(fl_nm,NC_NOWRITE,nc_id); */
  int rcd=nc_open(fl_nm.c_str(),cmode,&nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_open","Failed to open "+fl_nm);
  return rcd;
} // end nco_open()

int // O [id] netCDF file ID
nco_open // [fnc] Open netCDF file
(const std::string &fl_nm, // I [sng] File name
 const int &cmode) // I [enm] File open mode
{
  /* Purpose: Wrapper for nc_open() 
     Usage: nc_id=nco_open(fl_nm,NC_NOWRITE); */
  int nc_id; // O [id] netCDF file ID
  int rcd=nco_open(fl_nm,cmode,nc_id);
  rcd+=0; // CEWI
  return nc_id;
} // end nco_open()

// End nco_open() overloads

int // O [enm] Return success code
nco_inq // [fnc] Inquire file
(const int &nc_id, // I [enm] netCDF file ID
 int &dmn_nbr, // O [nbr] Number of dimensions
 int &var_nbr, // O [nbr] Number of variables
 int &att_nbr, // O [nbr] Number of global attributes
 int &rec_dmn_id, // O [id] Record dimension ID
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq(nc_id,&dmn_nbr,&var_nbr,&att_nbr,&rec_dmn_id);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq");
  return rcd;
} // end nco_inq() 

// Begin nco_inq_ndims() overloads

int // O [enm] Return success code
nco_inq_ndims // [fnc] Inquire file rank
(const int &nc_id, // I [enm] netCDF file ID
 int &dmn_nbr, // O [nbr] Number of dimensions
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_ndims()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_ndims(nc_id,&dmn_nbr);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_ndims");
  return rcd;
} // end nco_inq_ndims() 

int // O [nbr] Number of dimensions
nco_inq_ndims // [fnc] Inquire file dimensions
(const int &nc_id) // I [enm] netCDF file ID
{
  // Purpose: Wrapper for nco_inq_ndims()
  int dmn_nbr; // O [nbr] Number of dimensions
  int rcd=nc_inq_ndims(nc_id,&dmn_nbr);
  rcd+=0; // CEWI
  return dmn_nbr;
} // end nco_inq_ndims() 

// End nco_inq_ndims() overloads
// Begin nco_inq_nvars() overloads

int // O [enm] Return success code
nco_inq_nvars // [fnc] Inquire file variables
(const int &nc_id, // I [enm] netCDF file ID
 int &var_nbr, // O [nbr] Number of variables
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_nvars()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_nvars(nc_id,&var_nbr);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_nvars");
  return rcd;
} // end nco_inq_nvars() 

int // O [nbr] Number of variables
nco_inq_nvars // [fnc] Inquire file variables
(const int &nc_id) // I [enm] netCDF file ID
{
  // Purpose: Wrapper for nco_inq_nvars()
  int var_nbr; // O [nbr] Number of variables
  int rcd=nc_inq_nvars(nc_id,&var_nbr);
  rcd+=0; // CEWI
  return var_nbr;
} // end nco_inq_nvars() 

// End nco_inq_nvars() overloads

int // O [enm] Return success code
nco_inq_natts // [fnc] Inquire file attributes
(const int &nc_id, // I [enm] netCDF file ID
 int &att_nbr, // O [nbr] Number of attributes
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_natts()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_natts(nc_id,&att_nbr);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_natts");
  return rcd;
} // end nco_inq_natts() 

int // O [enm] Return success code
nco_inq_unlimdim // [fnc] Inquire file record ID
(const int &nc_id, // I [enm] netCDF file ID
 int &dmn_id, // O [id] Record dimension ID
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_unlimdim()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_unlimdim(nc_id,&dmn_id);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_unlimdim");
  return rcd;
} // end nco_inq_unlimdim() 

int // O [enm] Return success code
nco_redef // [fnc] Put open netCDF dataset into define mode
(const int &nc_id, // I [id] netCDF file ID
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  // Purpose: Wrapper for nc_redef()
  int rcd=nc_redef(nc_id);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_redef");
  return rcd;
} // end nco_redef()

int // O [enm] Return success code
nco_enddef // [fnc] Leave define mode
(const int &nc_id, // I [id] netCDF file ID
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  // Purpose: Wrapper for nc_enddef()
  int rcd=nc_enddef(nc_id);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_enddef");
  return rcd;
} // end nco_enddef()

int // O [enm] Return success code
nco_set_fill // [fnc] Set fill mode for writes
(const int &nc_id, // I [id] netCDF file ID
 const int &fillmode, // I [enm] New fill mode
 int &oldmode) // O [enm] Old fill mode
{
  /* Purpose: Wrapper for nc_set_fill()
     Usage: rcd=nco_set_fill(nc_id,NC_NOFILL,oldfill); */
  int rcd=nc_set_fill(nc_id,fillmode,&oldmode);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_set_fill");
  return rcd;
} // end nco_set_fill()

int // O [enm] Return success code
nco_close // [fnc] Close netCDF file
(const int &nc_id) // I [id] netCDF file ID
{
  /* Purpose: Wrapper for nc_close() 
     Usage: rcd=nco_close(nc_id); */
  int rcd=nc_close(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_close");
  return rcd;
} // end nco_close()

// Global functions with C++ linkages end
