// $Header$ 

// Implementation (declaration) of C++ interface to high-level NCO utilities

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include <nco_hgh.hh> // High-level NCO utilities

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

int // O [enm] Return success code
nco_var_dfn // [fnc] Define variables in output netCDF file
(const int &nc_id, // I [enm] netCDF file for output 
 var_mtd_sct *var_mtd, // I/O [sct] Array of structures containing variable metadata
 const int &var_mtd_nbr, // I [nbr] Number of variables in array
 const int &dmn_nbr_max) // I [nbr] Maximum number of dimensions allowed in single variable in output file
{
  /* Purpose: Process metadata structure, defining variables and attributes
     On entry, file is assumed to be open, and may be in define mode or in data mode
     On exit, file is placed in data mode
     Variable metadata is stored in single metadata input structure
     Routine only handles variable and attribute definition, not variable writing
     nc_id is modified as variables are defined
     var_mtd is modified as variable ID is returned after each definition
     fxm: It would be nice not to have to pass var_mtd_nbr from calling routine,
     but would presumably require a map container to accomplish
  */

  int rcd(0); // Return success code
  // Local
  long idx; // [idx] Counting index
  const std::string sbr_nm("nco_var_dfn"); // [sng] Name of subroutine
  const unsigned short int nco_dbg_lvl(0); // [sng] Debugging level
  if(nco_dbg_lvl >= nco_dbg_sbr) std::cerr << "DEBUG: Entering " << sbr_nm << "()..." << std::endl;
  if(nco_dbg_lvl >= nco_dbg_sbr) std::cerr << "DEBUG:"+sbr_nm+"() reports var_mtd_nbr = " << var_mtd_nbr << std::endl;

  // Allow file to already be in define mode
  rcd=nco_redef(nc_id,NC_EINDEFINE); // [fnc] Put open netCDF dataset into define mode

  int dmn_idx; // [idx] Counting index for dmn
  for(idx=0;idx<var_mtd_nbr;idx++){
    // Set dmn_nbr_max to, e.g., 1, to limit output file size at expense of losing all 2-D variables
    if(var_mtd[idx].dmn_nbr <= dmn_nbr_max){

      // Sanity check before output
      if(nco_dbg_lvl >= nco_dbg_io){
	std::string dmn_nm;
	size_t dmn_sz;
	for(dmn_idx=0;dmn_idx<var_mtd[idx].dmn_nbr;dmn_idx++){
	  rcd=nco_inq_dim(nc_id,dmn_idx,dmn_nm,dmn_sz);
	  std::cout << "Dimension " << dmn_nm << " is " << ((rcd != NC_NOERR) ? "valid" : "invalid" ) << " and has size " << dmn_sz << std::endl;
	} // end loop over dmn
      } // endif dbg

      rcd=nco_def_var // [fnc] Define variable
	(nc_id, // I [enm] netCDF file for output
	 var_mtd[idx].nm, // I [sng] Variable name
	 var_mtd[idx].type, // I [enm] netCDF type
	 var_mtd[idx].dmn_nbr, // I [nbr] Number of dimensions
	 var_mtd[idx].dmn_id, // I [dmn] Dimension list
	 var_mtd[idx].var_id); // O [var] Variable ID

      rcd=nco_put_att // [fnc] Create attribute
	(nc_id, // I [enm] netCDF file for output
	 var_mtd[idx].var_id, // [id] Variable ID
	 var_mtd[idx].att_1_nm, // [sng] Attribute name
	 var_mtd[idx].att_1_val); // [sng] Attribute value

      rcd=nco_put_att // [fnc] Create attribute
	(nc_id, // I [enm] netCDF file for output
	 var_mtd[idx].var_id, // [id] Variable ID
	 var_mtd[idx].att_2_nm, // [sng] Attribute name
	 var_mtd[idx].att_2_val); // [sng] Attribute value
    } // endif
  } // end loop over var

  // Be in data mode on exit so values may be written
  rcd=nco_enddef(nc_id); // [fnc] Leave define mode

  if(nco_dbg_lvl >= nco_dbg_sbr) std::cerr << "DEBUG: Exiting " << sbr_nm << "()..." << std::endl;
  return rcd; // [enm] Return success code
} // end nco_var_dfn()

// Global functions with C++ linkages end
