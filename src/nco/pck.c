/* $Header: /data/zender/nco_20150216/nco/src/nco/pck.c,v 1.1 2000-04-10 07:16:12 zender Exp $ */

/* Purpose: NCO utilities for packing and unpacking variables */

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
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */ 
#include <string.h>             /* strcmp. . . */
#include <unistd.h>             /* all sorts of POSIX stuff */ 

#include <netcdf.h>             /* netCDF def'ns */
#include "nc.h"                 /* netCDF operator universal def'ns */
#include <assert.h>             /* assert() debugging macro */ 

bool /* O [flg] Variable is packed */
is_var_pck
(int nc_id, /* I [idx] netCDF file ID */
 var_sct var) /* I/O [sct] Variable */
{
  /* Purpose: Check whether variable is packed */

  /* From netCDF User's Guide: */
  /* scale_factor: If present for a variable, the data are to be multiplied by this factor after the data are read by the application that accesses the data */
  /* add_offset: If present for a variable, this number is to be added to the data after is is read by the application that acceses the data. If both scale_factor and add_offset attributes are present, the data are first scaled before the offset id added. 
     When scale_factor and add_offset are used for packing, the associated variable (containing the packed data) is typically of type byte or short, whereas the unpacked values are intended to be of type float or double. The attributes scale_factor and add_offset should both be of the type intended for the unpacked data, e.g., float or double. */

  bool is_pck=False; /* [flg] Variable is packed */

  char *att_val; /* [sng] Attribute value */

  int att_sz; /* [nbr] Attribute size */
  int rcd; /* [rcd] Return success code */

  nc_type att_typ;

  nc_type scl_fct_typ; /* [idx] Type of scale_factor attribute */
  size_t scl_fct_lng; /* [idx] Number of elements in scale_factor attribute */
  
  /* Vet scale_factor */
  rcd=nc_inq_att(nc_id,var->id,"scale_factor",&scl_fct_typ,&scl_fct_lng);
  if(rcd == NC_NOERR){
    if(scl_fct_typ != NC_FLOAT && scl_fct_typ != NC_DOUBLE){
      (void)fprintf(stderr,"%s: WARNING is_var_pck() reports scale_factor for %s is not NC_FLOAT or NC_DOUBLE. Will not attempt to unpack.\n",prg_nm_get(),var->nm); 
      has_scl_fct=False; /* [flg] Variable is packed */
    } /* endif */
    if(scl_fct_lng != 1){
      (void)fprintf(stderr,"%s: WARNING is_var_pck() reports length of scale_factor not 1. Will not attempt to unpack.\n",prg_nm_get(),var->nm); 
      has_scl_fct=False; /* [flg] Variable is packed */
    } /* endif */
    has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
  } /* endif */

  if(var->has_scl_fct || var->has_add_fst){
    var->is_pck=True; /* [flg] Variable is packed on disk */
    (void)fprintf(stderr,"%s: PACKING Variable %s is packed\n",prg_nm_get(),var->nm); 
  } /* endif */

  if(rcd != -1 && att_typ == NC_CHAR){
    /* Add one for NULL byte */
    att_val=(char *)malloc(att_sz*nctypelen(att_typ)+1);
    (void)ncattget(nc_id,NC_GLOBAL,"convention",att_val);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    if(!strcmp("NCAR-CSM",att_val)) is_pck=True;
    if(att_val != NULL) (void)free(att_val);
  } /* endif */ 

  if(is_pck) (void)fprintf(stderr,"%s: CONVENTION File convention is NCAR CSM\n",prg_nm_get()); 

  return is_pck;
  
} /* end is_var_pck */ 


