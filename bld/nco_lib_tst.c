/* Test NCO link precedence 
   gcc -DHAVE_STRDUP -I ~/nco/src/nco -o ~/nco/bld/nco_lib_tst ~/nco/bld/nco_lib_tst.c -L${MY_LIB_DIR} -lnco-2.9.5 
   xlc_r -DHAVE_STRDUP -I ~/nco/src/nco -o ~/nco/bld/nco_lib_tst ~/nco/bld/nco_lib_tst.c -L${MY_LIB_DIR} -lnco-2.9.5 

   ~/nco/bld/nco_lib_tst
*/

#include <stdio.h>
#include <math.h>
#define MAIN_PROGRAM_FILE
#include "libnco.h" /* netCDF Operator (NCO) library */
int main()
{
  const char * const CVS_Id="$Id: nco_lib_tst.c,v 1.1 2004-06-15 17:52:22 zender Exp $"; 
  const char * const CVS_Revision="$Revision: 1.1 $";
  (void)copyright_prn(CVS_Id,CVS_Revision);
}

