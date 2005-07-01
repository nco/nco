/* Purpose: Test NCO library libnco_c++

   Usage:
   cd ~/nco/bld

   Linux:
   g++ -I../src/nco_c++ -o libnco_c++_tst libnco_c++_tst.cc -L${MY_LIB_DIR} -lnco_c++

   AIX:
   All at once:
   xlC_r -bnoquiet -I../src/nco_c++ -I/usr/local/include -o libnco_c++_tst libnco_c++_tst.cc -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco_c++ -lnetcdf
   xlC_r -bnoquiet -I../src/nco_c++ -I/usr/local/include -o libnco_c++_tst -Wl,-blibpath:${MY_LIB_DIR}:/usr/lpp/xlopt:/usr/lib/threads:/usr/lib:/lib libnco_c++_tst.cc -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco_c++ -lnetcdf

   xlC_r -c -I../src/nco_c++ -I/usr/local/include -o libnco_c++_tst.o libnco_c++_tst.cc
   xlC_r -bnoquiet -o libnco_c++_tst libnco_c++_tst.o -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco_c++ -lnetcdf
   xlC_r -bnoquiet -o libnco_c++_tst libnco_c++_tst.o -L${NETCDF_LIB} -lnco_c++ -lnetcdf

   ld -o libnco_c++_tst libnco_c++_tst.o -L${MY_LIB_DIR},-lnco_c++
   ld -o libnco_c++_tst libnco_c++_tst.o -L${MY_LIB_DIR},-lnco_c++ -L/usr/lpp/xlopt,-lxlopt,-lc libnco_c++_tst.o /lib/crt0_64.o */

#include <iostream> // Standard C++ I/O streams cout, cin
#include <string> // Standard C++ string class
#include <netcdf.h> // netCDF C interface
#include <libnco_c++.hh> // C++ interface to netCDF C library
int main()
{
  const nc_type nco_xtyp(nco_get_xtype(static_cast<double>(1.0))); // [enm] External netCDF type
  std::cout << "INFO External netCDF type of prc_cmp variables will be " << nco_typ_sng(nco_xtyp) << std::endl;
}

