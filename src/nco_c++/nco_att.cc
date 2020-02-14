// $Header$ 

// Implementation (declaration) of C++ interface to netCDF attribute routines

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include <nco_att.hh> // C++ interface to netCDF attribute routines

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

// Begin nco_put_att() overloads

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const float * const &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_float()
  int rcd=nc_put_att_float(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,att_val);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_att<float>");
  return rcd;
} // end nco_put_att<float>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const float &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nco_put_att<float>()
  const size_t att_sz(1);
  /* fxm: NB: float is the source from which all other nco_put_att<type> functions should
     be copied/templated, once the best method for handling single value puts is determined, 
     that is */
  //  int rcd=nco_put_att(nc_id,var_id,att_nm,att_sz,&(const_cast<float &>(att_val)),att_typ);
  // float foo_val(att_val);
  // int rcd=nco_put_att(nc_id,var_id,att_nm,att_sz,&foo_val,att_typ);
  int rcd=nc_put_att_float(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,&(const_cast<float &>(att_val)));
  return rcd;
} // end nco_put_att<float>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const double * const &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_double()
  int rcd=nc_put_att_double(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,att_val);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_att<double>");
  return rcd;
} // end nco_put_att<double>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const double &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_double()
  const size_t att_sz(1);
  int rcd=nc_put_att_double(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,&(const_cast<double &>(att_val)));
  return rcd;
} // end nco_put_att<double>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const int * const &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_int()
  int rcd=nc_put_att_int(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,att_val);
  return rcd;
} // end nco_put_att<int>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const int &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_int()
  const size_t att_sz(1);
  int rcd=nc_put_att_int(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,&(const_cast<int &>(att_val)));
  return rcd;
} // end nco_put_att<int>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const short * const &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_short()
  int rcd=nc_put_att_short(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,att_val);
  return rcd;
} // end nco_put_att<short>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const short &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_short()
  const size_t att_sz(1);
  int rcd=nc_put_att_short(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,&(const_cast<short &>(att_val)));
  return rcd;
} // end nco_put_att<short>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const size_t &att_sz, // I [nbr] Attribute length
 const long * const &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_long()
  int rcd=nc_put_att_long(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,att_val);
  return rcd;
} // end nco_put_att<long>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const long &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_long()
  const size_t att_sz(1);
  int rcd=nc_put_att_long(nc_id,var_id,att_nm.c_str(),att_typ,att_sz,&(const_cast<long &>(att_val)));
  return rcd;
} // end nco_put_att<long>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const std::string &att_val) // I [frc] Attribute value
{
  // Purpose: Wrapper for nc_put_att_text()
  int rcd=nc_put_att_text(nc_id,var_id,att_nm.c_str(),att_val.size(),att_val.c_str());
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_att<std::string>");
  return rcd;
} // end nco_put_att<std::string>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm, // I [sng] Attribute name
 const std::string &att_val) // I [sng] Attribute value
{
  // Purpose: Wrapper for nco_put_att<std::string>()
  int rcd=nco_put_att(nc_id,nco_inq_varid(nc_id,var_nm),att_nm,att_val);
  return rcd;
} // end nco_put_att<std::string>()

int // O [enm] Return success code
nco_put_att // [fnc] Create attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 const std::vector<double> &att_val, // I [frc] Attribute value
 const nc_type &att_typ) // I [enm] Attribute type 
{
  // Purpose: Wrapper for nc_put_att_double()
  // fxm: For some reason vector works here but valarray does not
  int rcd=nc_put_att_double(nc_id,var_id,att_nm.c_str(),att_typ,att_val.size(),&att_val[0]);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_att<std::vector<double>>");
  return rcd;
} // end nco_put_att<std::vector<double>>()

// End nco_put_att() overloads
// Begin nco_inq_att() overloads

int // O [enm] Return success code
nco_inq_att // [fnc] Inquire attribute
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 nc_type &att_typ, // O [enm] Attribute type
 size_t &att_sz, // O [nbr] Attribute size
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_att()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_att(nc_id,var_id,att_nm.c_str(),&att_typ,&att_sz);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_att");
  return rcd;
} // end nco_inq_att() 

// End nco_inq_att() overloads
// Begin nco_inq_attid() overloads

int // O [enm] Return success code
nco_inq_attid // [fnc] Inquire attribute ID
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [sng] Variable name
 const std::string &att_nm, // I [sng] Attribute name
 int &att_id, // O [id] Attribute ID
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_attid()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_attid(nc_id,var_id,att_nm.c_str(),&att_id);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_attid","Unable to find attribute ID for attribute \""+att_nm+"\" of variable \""+nco_inq_varname(nc_id,var_id)+"\", exiting...");
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_attid");
  return rcd;
} // end nco_inq_attid() 

int // O [enm] Return success code
nco_inq_attid // [fnc] Inquire attribute ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm, // I [sng] Attribute name
 int &att_id, // O [id] Attribute ID
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nco_inq_attid()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nco_inq_attid(nc_id,nco_inq_varid(nc_id,var_nm),att_nm,att_id);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_attid");
  return rcd;
} // end nco_inq_attid() 

int // O [id] Attribute ID
nco_inq_attid // [fnc] Inquire attribute ID
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm) // I [sng] Attribute name
{
  // Purpose: Wrapper for nco_inq_attid()
  int att_id; // O [id] Attribute ID
  int rcd=nco_inq_attid(nc_id,var_id,att_nm,att_id);
  rcd+=0; // CEWI
  return att_id;
} // end nco_inq_attid() 

int // O [id] Attribute ID
nco_inq_attid // [fnc] Inquire attribute ID
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm) // I [sng] Attribute name
{
  // Purpose: Wrapper for nco_inq_attid()
  int att_id; // O [id] Attribute ID
  int rcd=nco_inq_attid(nc_id,nco_inq_varid(nc_id,var_nm),att_nm,att_id);
  rcd+=0; // CEWI
  return att_id;
} // end nco_inq_attid() 

// End nco_inq_attid() overloads
// Begin nco_inq_attname() overloads

int // O [enm] Return success code
nco_inq_attname // [fnc] Inquire attribute name
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const int &att_id, // I [id] Attribute ID
 std::string &att_nm, // O [sng] Attribute name
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_attname()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  char att_nm_chr[NC_MAX_NAME];
  int rcd=nc_inq_attname(nc_id,var_id,att_id,att_nm_chr);
  att_nm=att_nm_chr;
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_attname");
  return rcd;
} // end nco_inq_attname() 

std::string // O [sng] Attribute name
nco_inq_attname // [fnc] Inquire attribute name
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const int &att_id) // I [id] Attribute ID
{
  // Purpose: Wrapper for nco_inq_attname()
  std::string att_nm; // O [id] Variable name
  int rcd=nco_inq_attname(nc_id,var_id,att_id,att_nm);
  rcd+=0; // CEWI
  return att_nm;
} // end nco_inq_attname() 

std::string // O [sng] Attribute name
nco_inq_attname // [fnc] Inquire attribute name
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const int &att_id) // I [id] Attribute ID
{
  // Purpose: Wrapper for nco_inq_attname()
  std::string att_nm; // O [id] Variable name
  int rcd=nco_inq_attname(nc_id,nco_inq_varid(nc_id,var_nm),att_id,att_nm);
  rcd+=0; // CEWI
  return att_nm;
} // end nco_inq_attname() 

// End nco_inq_attname() overloads
// Begin nco_inq_atttype() overloads

int // O [enm] Return success code
nco_inq_atttype // [fnc] Inquire attribute type
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 nc_type &att_typ, // O [enm] Variable type
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_atttype()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_atttype(nc_id,var_id,att_nm.c_str(),&att_typ);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_atttype");
  return rcd;
} // end nco_inq_atttype() 

nc_type // O [enm] Variable type
nco_inq_atttype // [fnc] Inquire variable type
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm) // I [sng] Attribute name
{
  // Purpose: Wrapper for nco_inq_atttype()
  nc_type att_typ; // O [enm] Variable type
  int rcd=nco_inq_atttype(nc_id,nco_inq_varid(nc_id,var_nm),att_nm,att_typ);
  rcd+=0; // CEWI
  return att_typ;
} // end nco_inq_atttype() 

// End nco_inq_atttype() overloads

// Begin nco_inq_attlen() overloads

int // O [enm] Return success code
nco_inq_attlen // [fnc] Inquire attribute length
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 size_t &att_sz, // O [nbr] Attribute size
 const int &rcd_opt) // I [enm] Optional non-fatal return code 
{
  /* Purpose: Wrapper for nc_inq_attlen()
     User may omit rcd_opt, or supply value of non-fatal netCDF return code */
  int rcd=nc_inq_attlen(nc_id,var_id,att_nm.c_str(),&att_sz);
  if(rcd != NC_NOERR && rcd != rcd_opt) nco_err_exit(rcd,"nco_inq_attlen");
  return rcd;
} // end nco_inq_attlen() 

size_t // O [nbr] Attribute size
nco_inq_attlen // [fnc] Inquire attribute length
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm) // I [sng] Attribute name
{
  // Purpose: Wrapper for nco_inq_attlen()
  size_t att_sz; // O [nbr] Attribute size
  int rcd=nco_inq_attlen(nc_id,var_id,att_nm,att_sz);
  rcd+=0; // CEWI
  return att_sz;
} // end nco_inq_attlen() 

size_t // O [nbr] Attribute size
nco_inq_attlen // [fnc] Inquire attribute length
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const int &att_id) // I [id] Attribute ID
{
  // Purpose: Wrapper for nco_inq_attlen()
  size_t att_sz; // O [nbr] Attribute size
  int rcd=nco_inq_attlen(nc_id,var_id,nco_inq_attname(nc_id,var_id,att_id),att_sz);
  rcd+=0; // CEWI
  return att_sz;
} // end nco_inq_attlen() 

size_t // O [nbr] Attribute size
nco_inq_attlen // [fnc] Inquire attribute length
(const int &nc_id, // I [enm] netCDF file ID
 const std::string &var_nm, // I [sng] Variable name
 const std::string &att_nm) // I [sng] Attribute name
{
  // Purpose: Wrapper for nco_inq_attlen(nc_inq_dimid())
  size_t att_sz; // O [nbr] Attribute size
  int rcd=nco_inq_attlen(nc_id,nco_inq_varid(nc_id,var_nm),att_nm,att_sz);
  rcd+=0; // CEWI
  return att_sz;
} // end nco_inq_attlen() 

// End nco_inq_attlen() overloads
// Begin nco_get_att() overloads

int // O [enm] Return success code
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 std::string &att_val) // O [frc] Attribute value
{
  // Purpose: Wrapper for nc_get_att_text()
  size_t att_sz=nco_inq_attlen(nc_id,var_id,att_nm);
  if(att_sz == 0) nco_wrn_prn("nco_get_att<std::string> will attempt to read NC_CHAR of size zero");
  // Allocate one extra byte for manual NUL-termination
  char *att_val_chr=new char[att_sz+1];
  int rcd=nc_get_att_text(nc_id,var_id,att_nm.c_str(),att_val_chr);
  /* Attribute on disk may or may not be NUL-terminated
     In either case, NUL-terminate it before assigning it to string
     Otherwise, overloaded string:=(char *) assignment will not terminate */
  att_val_chr[att_sz]='\0';
  att_val=att_val_chr;
  delete []att_val_chr;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_att<std::string>");
  return rcd;
} // end nco_get_att<std::string>()

int // O [enm] Return success code
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 float *&att_val) // O [frc] Attribute value
{
  // Purpose: Wrapper for nc_get_att_float()
  size_t att_sz=nco_inq_attlen(nc_id,var_id,att_nm);
  // Allocate space for variable: User is responsible for freeing space
  att_val=new float[att_sz];
  int rcd=nc_get_att_float(nc_id,var_id,att_nm.c_str(),att_val);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_att<float>");
  return rcd;
} // end nco_get_att<float>()

int // O [enm] Return success code
nco_get_att // [fnc] Get attribute value
(const int &nc_id, // I [enm] netCDF file ID
 const int &var_id, // I [id] Variable ID
 const std::string &att_nm, // I [sng] Attribute name
 double *&att_val) // O [frc] Attribute value
{
  // Purpose: Wrapper for nc_get_att_double()
  size_t att_sz=nco_inq_attlen(nc_id,var_id,att_nm);
  // Allocate space for variable: User is responsible for freeing space
  att_val=new double[att_sz];
  int rcd=nc_get_att_double(nc_id,var_id,att_nm.c_str(),att_val);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_att<double>");
  return rcd;
} // end nco_get_att<double>()

// End nco_get_att() overloads

// Global functions with C++ linkages end
