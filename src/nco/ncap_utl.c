/* $Header: /data/zender/nco_20150216/nco/src/nco/ncap_utl.c,v 1.9 2000-04-05 21:41:55 zender Exp $ */

/* Purpose: Utilities for ncap operator */ 

/* Copyright (C) 1995--2000 Charlie Zender

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.
   
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
   Please contact me via e-mail at zender@uci.edu or by writing

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
#include <unistd.h>             /* all sorts of POSIX stuff */ 

#include <netcdf.h>             /* netCDF def'ns */
#include "nc.h"                 /* netCDF operator universal def'ns */

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

  var_sum=var_dup(var_2);
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
  
  /* The file must be in define mode */ 
  ncopts=0; 
  (void)ncredef(nc_id);
  ncopts=NC_VERBOSE | NC_FATAL; 

  /* Define the variable */ 
  (void)fprintf(stderr,"ncap_write_var(): nm = %s\n",var->nm);
  var_id=ncvardef(nc_id,var->nm,var->type,var->nbr_dim,var->dmn_id);
  /* Take output file out of define mode */ 
  (void)ncendef(nc_id);
  /* Write out data */ 
  if(var->nbr_dim==0){
    ncvarput1(nc_id,var_id,0L,var->val.vp);
  }else{ /* end if variable is a scalar */ 
    ncvarput(nc_id,var_id,var->srt,var->cnt,var->val.vp);
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

