/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap_utl.c,v 1.15 2001-09-24 11:28:38 hmb Exp $ */

/* Purpose: Utilities for ncap operator */

/* Copyright (C) 1995--2001 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
   As a special exception to the terms of the GPL, you are permitted 
   to link the NCO source code with the NetCDF and HDF libraries 
   and distribute the resulting executables under the terms of the GPL, 
   but in addition obeying the extra stipulations of the netCDF and 
   HDF library licenses.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   The file LICENSE contains the GNU General Public License, version 2
   It may be viewed interactively by typing, e.g., ncks -L

   The author of this software, Charlie Zender, would like to receive
   your suggestions, improvements, bug-reports, and patches for NCO.
   Please contact the project at http://sourceforge.net/projects/nco or by writing

   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100
 */

/* Standard header files */
#include <math.h>               /* sin cos cos sin 3.14159 */
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */
#include <string.h>             /* strcmp. . . */
#include <time.h>               /* machine time */
#include <unistd.h>             /* POSIX stuff */

#include <netcdf.h>             /* netCDF definitions */
#include "nc.h"                 /* netCDF operator universal def'ns */
#include "nco_netcdf.h"			/* netcdf3.x wrappers */

var_sct *
ncap_var_add(var_sct *var_1,var_sct *var_2)
/* 
   var_sct *var_1: input variable structure containing first operand
   var_sct *var_2: input variable structure containing second operand
   var_sct *ncap_var_add(): output sum of input variables
 */
{
  bool MUST_CONFORM=False; /* Must var_conform_dim() find truly conforming variables? */
  bool DO_CONFORM; /* Did var_conform_dim() find truly conforming variables? */

  /* Routine called by parser */
  var_sct *var_sum;

  var_sum=var_dpl(var_2);
  var_sum=var_conform_dim(var_1,var_2,var_sum,MUST_CONFORM,&DO_CONFORM);
  var_sum=var_conform_type(var_1->type,var_sum);
  (void)var_add(var_1->type,var_1->sz,var_1->has_mss_val,var_1->mss_val,var_1->tally,var_1->val,var_sum->val);
   
  return var_sum;
} /* end ncap_var_add() */

int
ncap_write_var(int nc_id,var_sct *var)
/* 
   int nc_id: input netCDF file ID
   var_sct *var: input/output variable structure
   var_sct *ncap_write_var(): output ID of var in output file
 */
{
  /* Routine called by parser */
  int var_id;
  int rcd;
  
  /* The file must be in define mode */
  
  rcd = nco_redef(nc_id);
  

  /* Define the variable */
  (void)fprintf(stderr,"ncap_write_var(): nm = %s\n",var->nm);
  rcd=nco_def_var(nc_id,var->nm,var->type,var->nbr_dim,var->dmn_id,&var_id);
  /* Take output file out of define mode */
  rcd=nco_enddef(nc_id);
  /* Write out data */
  if(var->nbr_dim==0){
    rcd=nco_put_var1(nc_id,var_id,0L,var->val.vp,var->type);
  }else{ /* end if variable is a scalar */
    nco_put_vara(nc_id,var_id,var->srt,var->cnt,var->val.vp,var->type);
  } /* end if variable is an array */

   return var_id;
} /* end ncap_write_var() */

int yyerror(char *sng)
{
  /* Purpose: Process errors reported by the parser. 
     The Bison parser detects a "parse error" or "syntax error" whenever
     it reads a token which cannot satisfy any syntax rule.
     Normally this function resides in liby.a but that library does not exist
     on Linux systems so we supply it here.
   */
  (void)fprintf(stderr,"%s\n",sng);
  /* I have no idea what yyerror() should return */
  return 0;
} /* end yyerror() */

