/* $Header: /data/zender/nco_20150216/nco/src/nco/pck.c,v 1.6 2000-08-29 20:57:51 zender Exp $ */

/* Purpose: NCO utilities for packing and unpacking variables */

/* Copyright (C) 1995--2000 Charlie Zender

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
#include <stdio.h>              /* stderr, FILE, NULL, etc. */
#include <stdlib.h>             /* atof, atoi, malloc, getopt */
#include <string.h>             /* strcmp. . . */
#include <unistd.h>             /* POSIX stuff */

#include <netcdf.h>             /* netCDF definitions */
#include "nc.h"                 /* netCDF operator universal def'ns */
#include <assert.h>             /* assert() debugging macro */

bool /* O [flg] Variable is packed */
is_var_pck /* [fnc] Check whether variable is packed */
(int nc_id, /* I [idx] netCDF file ID */
 var_sct *var) /* I/O [sct] Variable */
{
  /* Purpose: Check whether variable is packed */
  /* ncea -O -D 3 -v pck ~/nco/data/in.nc ~/nco/data/foo.nc */

  /* From netCDF User's Guide: */
  /* scale_factor: If present for a variable, the data are to be multiplied by this factor after the data are read by the application that accesses the data */
  /* add_offset: If present for a variable, this number is to be added to the data after is is read by the application that acceses the data. If both scale_factor and add_offset attributes are present, the data are first scaled before the offset is added. 
     When scale_factor and add_offset are used for packing, the associated variable (containing the packed data) is typically of type byte or short, whereas the unpacked values are intended to be of type float or double. The attributes scale_factor and add_offset should both be of the type intended for the unpacked data, e.g., float or double. */

  int rcd; /* [rcd] Return success code */

  nc_type scl_fct_typ; /* [idx] Type of scale_factor attribute */
  size_t scl_fct_lng; /* [idx] Number of elements in scale_factor attribute */
  nc_type add_fst_typ; /* [idx] Type of add_offset attribute */
  size_t add_fst_lng; /* [idx] Number of elements in add_offset attribute */
  
  var_sct *scl_fct; /* [sct] Variable structure for scale_factor */
  var_sct *add_fst; /* [sct] Variable structure for add_offset */

  /* Initialize flags */
  /* fxm: initialize these in var_fll()?, scl_dbl_mk_var()? */
  var->has_scl_fct=False; /* [flg] Valid scale_factor attribute exists */
  var->has_add_fst=False; /* [flg] Valid add_offset attribute exists */
  var->is_pck=False; /* [flg] Valid add_offset attribute exists */

  /* Vet scale_factor */
  rcd=nc_inq_att(nc_id,var->id,"scale_factor",&scl_fct_typ,&scl_fct_lng);
  if(rcd == NC_NOERR){
    if(scl_fct_typ != NC_FLOAT && scl_fct_typ != NC_DOUBLE){
      (void)fprintf(stderr,"%s: WARNING is_var_pck() reports scale_factor for %s is not NC_FLOAT or NC_DOUBLE. Will not attempt to unpack using scale_factor.\n",prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(scl_fct_lng != 1){
      (void)fprintf(stderr,"%s: WARNING is_var_pck() reports %s has scale_factor of length %d. Will not attempt to unpack using scale_factor\n",prg_nm_get(),var->nm,scl_fct_lng); 
      return False;
    } /* endif */
    var->has_scl_fct=True; /* [flg] Valid scale_factor attribute exists */
  } /* endif */

  /* Vet add_offset */
  rcd=nc_inq_att(nc_id,var->id,"add_offset",&add_fst_typ,&add_fst_lng);
  if(rcd == NC_NOERR){
    if(add_fst_typ != NC_FLOAT && add_fst_typ != NC_DOUBLE){
      (void)fprintf(stderr,"%s: WARNING is_var_pck() reports add_offset for %s is not NC_FLOAT or NC_DOUBLE. Will not attempt to unpack.\n",prg_nm_get(),var->nm); 
      return False;
    } /* endif */
    if(add_fst_lng != 1){
      (void)fprintf(stderr,"%s: WARNING is_var_pck() reports %s has add_offset of length %d. Will not attempt to unpack.\n",prg_nm_get(),var->nm,add_fst_lng); 
      return False;
    } /* endif */
    var->has_add_fst=True; /* [flg] Valid add_offset attribute exists */
  } /* endif */

  if(var->has_scl_fct && var->has_add_fst){
    if(scl_fct_typ != add_fst_typ){
      (void)fprintf(stderr,"%s: WARNING is_var_pck() reports type of scale_factor does not equal type of add_offset. Will not attempt to unpack.\n",prg_nm_get());
      return False;
    } /* endif */
  } /* endif */

  if(var->has_scl_fct || var->has_add_fst){
    /* Variable is considered packed iff either or both valid scale_factor or add_offset exist */
    var->is_pck=True; /* [flg] Variable is packed on disk */
    var->typ_xpn=scl_fct_typ; /* Type of variable when unpacked (expanded) (in memory) */
    if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: PACKING Variable %s is type %s packed into type %s\n",prg_nm_get(),var->nm,nco_typ_sng(var->typ_xpn),nco_typ_sng(var->typ_dsk));
    if(dbg_lvl_get() > 2) (void)fprintf(stderr,"%s: PACKING Packing and unpacking are not yet implemented\n",prg_nm_get());
  } /* endif */

  if(var->has_scl_fct){ /* [flg] Valid scale_factor attribute exists */
    var->scl_fct.vp=(void *)nco_malloc(scl_fct_lng*nctypelen(scl_fct_typ));
    (void)ncattget(var->nc_id,var->id,"scale_factor",var->scl_fct.vp);
    scl_fct=scl_ptr_mk_var(var->scl_fct.vp,scl_fct_typ); /* [sct] Variable structure for scale_factor */
    /* Convert var to type of scale_factor for expansion */
    var=var_conform_type(scl_fct->type,var);
    /* Multiply var by scale_factor */
    (void)var_multiply(scl_fct->type,var->sz,var->has_mss_val,var->mss_val,scl_fct->val,var->val);
  } /* endif */

  if(var->has_add_fst){ /* [flg] Valid add_offset attribute exists */
    var->add_fst.vp=(void *)nco_malloc(add_fst_lng*nctypelen(add_fst_typ));
    (void)ncattget(var->nc_id,var->id,"add_offset",var->add_fst.vp);
    add_fst=scl_ptr_mk_var(var->add_fst.vp,add_fst_typ); /* [sct] Variable structure for add_offset */
    /* Convert var to type of add_offset for expansion */
    var=var_conform_type(add_fst->type,var);
    /* Add add_offset to var */
    (void)var_add(add_fst->type,var->sz,var->has_mss_val,var->mss_val,var->tally,add_fst->val,var->val);
  } /* endif */

  if(var->has_scl_fct || var->has_add_fst){
    (void)fprintf(stdout,"%s: DEBUG var->val.dp[0]=%g\n",prg_nm_get(),var->val.dp[0]);
  } /* endif */

  return var->is_pck;
  
} /* end is_var_pck */

var_sct * /* O [sct] Unpacked variable */
var_upk /* [fnc] Unpack variable */
(var_sct *var) /* I/O [sct] Variable to be unpacked */
{
  /* Purpose: Unpack variable */

  /* From netCDF User's Guide: */
  /* scale_factor: If present for a variable, the data are to be multiplied by this factor after the data are read by the application that accesses the data */
  /* add_offset: If present for a variable, this number is to be added to the data after is is read by the application that acceses the data. If both scale_factor and add_offset attributes are present, the data are first scaled before the offset is added. 
     When scale_factor and add_offset are used for packing, the associated variable (containing the packed data) is typically of type byte or short, whereas the unpacked values are intended to be of type float or double. The attributes scale_factor and add_offset should both be of the type intended for the unpacked data, e.g., float or double. */

  /* Return if variable is not currently packed */
  if(!var->is_pck) return var;

  /* Packed variables are guaranteed to have both scale_factor and add_offset
     The scale factor is guaranteed to be of type NC_FLOAT or NC_DOUBLE and of size 1 (a scalar) */

  /* Convert scalar values of scale_factor and add_offset into NCO variables */

  /*  */

  return var;
  
} /* end var_upk */
