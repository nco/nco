/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_typ.c,v 1.1 2002-05-02 06:10:30 zender Exp $ */

/* Purpose: netCDF type utilities for NCO netCDF operators */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <stdlib.h> /* atof, atoi, malloc, getopt */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <time.h> /* machine time */
#include <unistd.h> /* POSIX stuff */
#include <assert.h> /* assert() debugging macro */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#ifdef _OPENMP
#include <omp.h> /* OpenMP pragmas */
#endif /* not _OPENMP */

/* Personal headers */
#include "nco.h" /* netCDF operator universal def'ns */
#include "nco_netcdf.h"  /* NCO wrappers for libnetcdf.a */

void
cast_void_nctype(nc_type type,ptr_unn *ptr)
/*  
   nc_type type: I netCDF type to cast void pointer to
   ptr_unn *ptr: I/O pointer to pointer union whose vp element will be cast to type type
*/
{
  /* Cast generic pointer in ptr_unn structure from type void to output netCDF type */
  switch(type){
  case NC_FLOAT:
    ptr->fp=(float *)ptr->vp;
    break;
  case NC_DOUBLE:
    ptr->dp=(double *)ptr->vp;
    break;
  case NC_INT:
    ptr->lp=(nco_long *)ptr->vp;
    break;
  case NC_SHORT:
    ptr->sp=(short *)ptr->vp;
    break;
  case NC_CHAR:
    ptr->cp=(unsigned char *)ptr->vp;
    break;
  case NC_BYTE:
    ptr->bp=(signed char *)ptr->vp;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end cast_void_nctype() */

void
cast_nctype_void(nc_type type,ptr_unn *ptr)
/*  
   nc_type type: I netCDF type of pointer
   ptr_unn *ptr: I/O pointer to pointer union which to cast from type type to type void
*/
{
  /* Cast generic pointer in ptr_unn structure from type type to type void */
  switch(type){
  case NC_FLOAT:
    ptr->vp=(void *)ptr->fp;
    break;
  case NC_DOUBLE:
    ptr->vp=(void *)ptr->dp;
    break;
  case NC_INT:
    ptr->vp=(void *)ptr->lp;
    break;
  case NC_SHORT:
    ptr->vp=(void *)ptr->sp;
    break;
  case NC_CHAR:
    ptr->vp=(void *)ptr->cp;
    break;
  case NC_BYTE:
    ptr->vp=(void *)ptr->bp;
    break;
  default: nco_dfl_case_nctype_err(); break;
  } /* end switch */
} /* end cast_nctype_void() */

