/* Purpose: Test NCO link precedence 
   gcc -I ~/nco/src/nco -o ~/nco/bld/nco_lib_tst ~/nco/bld/nco_lib_tst.c -L${MY_LIB_DIR} -lnco-2.9.6

   AIX:
   All at once:
   xlc_r -q64 -bnoquiet -brtl -I ~/nco/src/nco -I /usr/local/include -o ~/nco/bld/nco_lib_tst ~/nco/bld/nco_lib_tst.c -L${MY_LIB_DIR},-lnco-2.9.6
   xlc_r -c -DHAVE_STRDUP -I ~/nco/src/nco -I /usr/local/include -o ~/nco/bld/nco_lib_tst.o ~/nco/bld/nco_lib_tst.c
   xlc_r -q64 -bnoquiet -brtl -I ~/nco/src/nco -I /usr/local/include -o ~/nco/bld/nco_lib_tst -L${MY_LIB_DIR},-lnco-2.9.6 ~/nco/bld/nco_lib_tst.o
   xlc_r -q64 -bnoquiet -I ~/nco/src/nco -I /usr/local/include -o ~/nco/bld/nco_lib_tst ~/nco/bld/nco_lib_tst.c -L/usr/local/lib,-lnco
   ld -b64 -o ~/nco/bld/nco_lib_tst nco_lib_tst.o -L${MY_LIB_DIR},-lnco
   ld -b64 -o ~/nco/bld/nco_lib_tst nco_lib_tst.o -L${MY_LIB_DIR},-lnco -L/usr/lpp/xlopt,-lxlopt,-lc nco_lib_tst.o /lib/crt0_64.o

  ~/nco/bld/nco_lib_tst
*/

#include <stdio.h>
#include <math.h>
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */
int main()
{
  const char * const CVS_Id="$Id: nco_lib_tst.c,v 1.2 2004-06-15 22:37:47 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.2 $";
  (void)copyright_prn(CVS_Id,CVS_Revision);
}

