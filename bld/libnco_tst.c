/* Purpose: Test NCO library libnco

   Usage:
   cd ~/nco/bld

   Linux:
   gcc -I../src/nco -o libnco_tst libnco_tst.c -L${MY_LIB_DIR} -lnco

   AIX:
   All at once:
   xlc_r -bnoquiet -I../src/nco -I/usr/local/include -o libnco_tst libnco_tst.c -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco -lnetcdf
   xlc_r -bnoquiet -I../src/nco -I/usr/local/include -o libnco_tst -Wl,-blibpath:${MY_LIB_DIR}:/usr/lpp/xlopt:/usr/lib/threads:/usr/lib:/lib libnco_tst.c -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco -lnetcdf

   xlc_r -c -I../src/nco -I/usr/local/include -o libnco_tst.o libnco_tst.c
   xlc_r -bnoquiet -o libnco_tst libnco_tst.o -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco -lnetcdf
   xlc_r -bnoquiet -o libnco_tst libnco_tst.o -L${NETCDF_LIB} -lnco -lnetcdf

   ld -o libnco_tst libnco_tst.o -L${MY_LIB_DIR},-lnco
   ld -o libnco_tst libnco_tst.o -L${MY_LIB_DIR},-lnco -L/usr/lpp/xlopt,-lxlopt,-lc libnco_tst.o /lib/crt0_64.o */

#include <stdio.h>
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */
int main()
{
  const char * const CVS_Id="$Id$"; 
  const char * const CVS_Revision="$Revision$";
  (void)copyright_prn(CVS_Id,CVS_Revision);
}

