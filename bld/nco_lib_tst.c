/* Purpose: Test NCO link precedence 

   Usage:
   cd ~/nco/bld

   Linux:
   gcc -I../src/nco -o nco_lib_tst nco_lib_tst.c -L${MY_LIB_DIR} -lnco-2.9.6

   AIX:
   All at once:
   xlc_r -bnoquiet -I../src/nco -I/usr/local/include -o nco_lib_tst nco_lib_tst.c -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco -lnetcdf
   xlc_r -bnoquiet -I../src/nco -I/usr/local/include -o nco_lib_tst -Wl,-blibpath:${MY_LIB_DIR}:/usr/lpp/xlopt:/usr/lib/threads:/usr/lib:/lib nco_lib_tst.c -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco -lnetcdf

   xlc_r -c -I../src/nco -I/usr/local/include -o nco_lib_tst.o nco_lib_tst.c
   xlc_r -bnoquiet -o nco_lib_tst nco_lib_tst.o -L${MY_LIB_DIR} -L${NETCDF_LIB} -lnco -lnetcdf
   xlc_r -bnoquiet -o nco_lib_tst nco_lib_tst.o -L${NETCDF_LIB} -lnco -lnetcdf

   ld -o nco_lib_tst nco_lib_tst.o -L${MY_LIB_DIR},-lnco
   ld -o nco_lib_tst nco_lib_tst.o -L${MY_LIB_DIR},-lnco -L/usr/lpp/xlopt,-lxlopt,-lc nco_lib_tst.o /lib/crt0_64.o

  nco_lib_tst
*/

#include <stdio.h>
#include <math.h>
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */
int main()
{
  const char * const CVS_Id="$Id: nco_lib_tst.c,v 1.3 2004-06-16 00:35:38 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.3 $";
  (void)copyright_prn(CVS_Id,CVS_Revision);
}

