// $Header$

// Purpose: Test for C++ programs using libnco_c++ API


/* Compilation
   make -W tst.cc OPTS=D tst
   make OPTS=D tst
   scp ~/nco/src/nco_c++/tstcc dust.ess.uci.edu:nco/src/nco_c++
   cd ${HOME}/nco/src/nco_c++;make -f Makefile.old OPTS=D -W tst.cc tst;cd -
   cd ${HOME}/nco/src/nco_c++;make -f Makefile.old OPTS=D lib_cln cln;make -f Makefile.old OPTS=D all;cd -
   cd ${HOME}/nco/src/nco_c++;make OPTS=D tst;cd - */

/* Usage:
   tst */

// etags ~/nco/src/nco_c++/*.cc ~/nco/src/nco_c++/*.hh ~/mie/*.cc ~/mie/*.hh ~/slr_spc/*.cc ~/slr_spc/*.hh ~/ck/htrn.c ~/c++/*.cc ~/c++/*.hh 

// Autotools may enable/disable library features
#ifdef HAVE_CONFIG_H
# include <config.h> // Autotools tokens
#endif // !HAVE_CONFIG_H

// Standard C++ headers
#include <iostream> // Standard C++ I/O streams cout, cin
#include <map> // STL multimap and map classes
#include <string> // Standard C++ string class
#include <valarray> // STL valarray class template

// Standard C headers
#if !(defined __xlC__) && !(defined SGIMP64) // C++ compilers that do not allow stdint.h
# include <stdint.h> // Required by g++ for LLONG_MAX, ULLONG_MAX, by icpc for int64_t
#endif // C++ compilers that do not allow stdint.h

// 3rd party vendors
#include <netcdf.h> // netCDF C interface

// Personal headers
#include <libnco_c++.hh> // C++ interface to netCDF C library

// Typedefs
typedef float prc_cmp; // [prc] Computational precision is float (usually 4 bytes)
//typedef double prc_cmp; // [prc] Computational precision is double (usually 8 bytes)

typedef std::map<std::string,var_mtd_sct,std::less<std::string> > sng2var_mtd_map; // String-to-var_mtd_sct map

int main(int argc,char **argv)
{
  const std::string fl_in("in.nc"); // [sng] Input file
  const std::string fl_out("out.nc"); // [sng] Output file
  int rcd(0); // [rcd] Return success code
  long bnd_nbr(1); // [nbr] Number of bands
  long lat_nbr(1); // [nbr] Number of latitudes
  long lon_nbr(1); // [nbr] Number of longitudes
  long sz_nbr(1); // [nbr] Number of sizes
  long idx; // [idx] Counting index

  const std::string CVS_Date("$Date$"); // [sng] CVS date string
  const std::string CVS_Header("$Header$"); // [sng] CVS header string
  const std::string CVS_Id("$Id$"); // [sng] CVS identification string
  const std::string CVS_Revision("$Revision$"); // [sng] CVS revision string
  const std::string date_cvs(CVS_Date.length() > 7 ? CVS_Date.substr(7,19) : static_cast<std::string>("Unknown")); // [sng] Date from CVS
  const std::string sbr_nm("main"); // [sng] Subroutine name
  const std::string prg_nm("libnco_c++"); // [sng] Program name
  const std::string vrs_cvs(CVS_Revision.length() > 10 ? CVS_Revision.substr(10,4) : static_cast<std::string>("Unknown")); // [sng] Version from CVS
#define XTKN2SNG(x) #x
#define TKN2SNG(x) XTKN2SNG(x)
  const std::string date_cpp(__DATE__); // [sng] Date from C pre-processor
  const std::string time_cpp(__TIME__); // [sng] Time from C pre-processor
  const std::string vrs_cpp(TKN2SNG(NCO_VERSION)); // [sng] Version from C pre-processor
  const std::string hst_cpp(TKN2SNG(HOSTNAME)); // [sng] Hostname from C pre-processor
  const std::string usr_cpp(TKN2SNG(USER)); // [sng] Hostname from C pre-processor
  if(vrs_cvs == "Unknown") std::cerr << prg_nm << " version " << vrs_cpp << " built " << date_cpp << " on " << hst_cpp << " by " << usr_cpp << std::endl;
  if(vrs_cvs != "Unknown") std::cerr << prg_nm << " version " << vrs_cvs << " last modified " << date_cvs << " built " << date_cpp << " on " << hst_cpp << " by " << usr_cpp << std::endl;

  // Open input file
  int nc_id=nco_open(fl_in,NC_NOWRITE); // [fnc] Open netCDF file
  // Input required data
  const long wvl_nbr(nco_inq_dimlen(nc_id,static_cast<std::string>("wvl"))); // [nbr] Number of wavelengths
  std::cerr << "Number of wavelengths in "+fl_in+" is "<< wvl_nbr << std::endl;
  /* netCDF C++ interface automatically allocates memory required by get_var()
     User is responsible for freeing this memory when no longer needed
     Currently this is done after input array is written to output file */
  prc_cmp *wvl; // [m] Wavelength
  rcd=nco_get_var(nc_id,static_cast<std::string>("wvl"),wvl); // [m] Wavelength
  std::cerr << "Value of wvl[0] in "+fl_in+" is "<< wvl[0] << std::endl;
  rcd=nco_close(nc_id); // [fnc] Close netCDF file
  std::valarray<prc_cmp> wvl_valarray(0.0,wvl_nbr); // [frc] 

  // Allocate dynamic arrays
  prc_cmp *sz=new prc_cmp[sz_nbr]; // [m] Size at bin center
  long double *sz_ldb=new long double[sz_nbr]; // [m] Size at bin center

  // Dummy data
  for(idx=0;idx<sz_nbr;idx++){
    sz[idx]=0.5e-6; // [m] Size at bin center
    sz_ldb[idx]=sz[idx]; // [m] Size at bin center
  } // end loop over idx

  // Open output file
  /* Compatibility tokens not available until netCDF 3.6.1 netcdf.h
     OPeNDAP libraries lack these tokens */
#ifndef NC_FORMAT_CLASSIC
# define NC_FORMAT_CLASSIC (1)
#endif
  int fl_out_fmt(NC_FORMAT_CLASSIC); // [enm] Output file format
  int md_create(NC_CLOBBER); // [enm] Mode flag for nco_create() call
#ifdef ENABLE_NETCDF4
  fl_out_fmt=NC_FORMAT_NETCDF4; // [enm] Output file format
  if(fl_out_fmt == NC_FORMAT_64BIT){
    md_create|=NC_64BIT_OFFSET;
  }else if(fl_out_fmt == NC_FORMAT_NETCDF4){
    md_create|=NC_NETCDF4;
  }else if(fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
    md_create|=NC_NETCDF4|NC_CLASSIC_MODEL;
  } /* end else fl_out_fmt */
#else // !ENABLE_NETCDF4
  if(fl_out_fmt == NC_FORMAT_CLASSIC) md_create+=0; // CEWI
#endif // !ENABLE_NETCDF4
  const int nc_out(nco_create(fl_out,md_create)); 
  const nc_type nco_xtyp(nco_get_xtype(static_cast<prc_cmp>(1.0))); // [enm] External netCDF type
  std::cout << "INFO External netCDF type of prc_cmp variables will be " << nco_typ_sng(nco_xtyp) << std::endl;
 
  // Create dimensions
  const int sz_dmn(nco_def_dim(nc_out,static_cast<std::string>("sz"),sz_nbr)); // [dmn] Size dimension
  const int bnd_dmn(nco_def_dim(nc_out,static_cast<std::string>("bnd"),bnd_nbr)); // [dmn] Band dimension
  const int wvl_dmn(nco_def_dim(nc_out,static_cast<std::string>("wvl"),wvl_nbr)); // [dmn] Wavelength dimension

  // Derive dimensions
  const int dmn_bnd_sz[2]={bnd_dmn,sz_dmn};
  const int *dmn_sz(&sz_dmn); // [dmn] Pointer to size dimension
  const int *dmn_wvl(&wvl_dmn); // [dmn] Pointer to wavelength dimension
  const int CEWI_int(-2147483647); // [frc] Compiler Error Warning Initializer for int
  const int *dmn_scl(&CEWI_int); // [dmn] Dummy argument, not used

  std::cerr << "Currently there are " << nco_inq_ndims(nc_out) << " dimensions defined" << std::endl;

  // Global attributes
  rcd=nco_put_att(nc_out,NC_GLOBAL,"CVS_Id",CVS_Id);

  var_mtd_sct var_mtd[]={
    {0,"wvl",nco_xtyp,1,dmn_wvl,"long_name","Wavelength at band center","units","meter"},
    {0,"wvl_valarray",nco_xtyp,1,dmn_wvl,"long_name","valarray","units","meter"},
    {0,"sz",nco_xtyp,1,dmn_sz,"long_name","Size at bin center","units","meter"},
    {0,"sz_ldb",nco_xtyp,1,dmn_sz,"long_name","Size at bin center","units","meter"},
    {0,"sz_nbr",NC_INT,0,dmn_scl,"long_name","Number of sizes","units","number"},
  }; // end var_mtd_sct var_mtd[]
  const int var_mtd_nbr(sizeof(var_mtd)/sizeof(var_mtd_sct));

  sng2var_mtd_map var_mtd_map;
  for(idx=0;idx<var_mtd_nbr;idx++){
    /* fxm: Define variables before inserting into map, because map values 
       seem to be unwritable (read-only) once they are in map. */
    rcd=nco_def_var(nc_out,var_mtd[idx].nm,var_mtd[idx].type,var_mtd[idx].dmn_nbr,var_mtd[idx].dmn_id,var_mtd[idx].var_id);
    var_mtd_map.insert(sng2var_mtd_map::value_type(var_mtd[idx].nm,var_mtd[idx]));
  } // end loop over itr

  sng2var_mtd_map::const_iterator var_mtd_itr;
  for(var_mtd_itr=var_mtd_map.begin();var_mtd_itr!=var_mtd_map.end();++var_mtd_itr){
    // Write first attribute (long_name)
    rcd=nco_put_att(nc_out,var_mtd_itr->second.var_id,var_mtd_itr->second.att_1_nm,var_mtd_itr->second.att_1_val);
    // Write second attribute (units)
    rcd=nco_put_att(nc_out,var_mtd_itr->second.var_id,var_mtd_itr->second.att_2_nm,var_mtd_itr->second.att_2_val);
    std::cout << "Defined " << var_mtd_itr->first << " with long_name = " << var_mtd_itr->second.att_1_val << " and units = " << var_mtd_itr->second.att_2_val << std::endl;
  } // end loop over var_mtd_itr

  // Leave define mode
  rcd=nco_enddef(nc_out,NC_ENOTINDEFINE); // [fnc] Leave define mode

  // Write data and delete dynamic arrays
  // Syntax valid for statically allocated arrays foo[dim1][dim2]
  // rcd=nco_put_var(nc_out,static_cast<std::string>("tpt_d2d"),&tpt_d2d[0]);
  // Syntax valid for vector< vector<prc_cmp> > foo(dim1,vector<prc_cmp>(dim2))
  // rcd=nco_put_var(nc_out,static_cast<std::string>("tpt_v2d"),&tpt_v2d[0][0]);
  // Syntax valid for vector<prc_cmp> foo(dim1*dim2)
  // rcd=nco_put_var(nc_out,static_cast<std::string>("tpt_v1d"),&tpt_v1d[0]);
  // Syntax valid for a2d_cls<prc_cmp> foo(dim1,dim2)
  // rcd=nco_put_var(nc_out,static_cast<std::string>("tpt_a2d"),&tpt_a2d(0,0));
  rcd=nco_put_var(nc_out,static_cast<std::string>("wvl_valarray"),wvl_valarray);
  rcd=nco_put_var(nc_out,static_cast<std::string>("wvl"),wvl); delete []wvl;
  rcd=nco_put_var(nc_out,static_cast<std::string>("sz"),sz); delete []sz;
  rcd=nco_put_var(nc_out,static_cast<std::string>("sz_ldb"),sz_ldb); delete []sz_ldb;
  rcd=nco_put_var(nc_out,static_cast<std::string>("sz_nbr"),sz_nbr);

  int rcd_opt(NC_ENOTVAR); // [enm] Acceptable return code
  int var_id; // [id] Variable ID
  int var_ndims; // [nbr] Number of dimensions
  const std::string var_nm_nonesuch("quark"); // [sng]
  rcd=nco_inq_varid(nc_out,var_nm_nonesuch,var_id,rcd_opt);
  std::cout << "tst: nco_inq_varid() for variable \"" << var_nm_nonesuch << "\" with rcd_opt = " << rcd_opt << " yields rcd = " << rcd << std::endl;
  rcd=nco_inq_varndims(nc_out,var_nm_nonesuch,var_ndims,rcd_opt);
  std::cout << "tst: nco_inq_varndims() for variable \"" << var_nm_nonesuch << "\" with rcd_opt = " << rcd_opt << " yields rcd = " << rcd << std::endl;

  // Close output file
  rcd=nco_close(nc_out); // [fnc] Close netCDF file
  std::cerr << "Wrote results to " << fl_out << std::endl;
  std::cerr << "ncks: ncks -C -H -F -m -u -d wvl,0.5e-6 -v wvl " << fl_out << std::endl;

  // Fix all unused identifiers at end where no harm can be done
  idx=0*(rcd+lat_nbr+lon_nbr+dmn_bnd_sz[0]+argc+sizeof(argv)); // [idx] Counting index CEWI

  return EXIT_SUCCESS;
} // end main()

