/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_netcdf.h,v 1.5 2001-10-08 07:25:39 zender Exp $ */

/* Purpose: Wrappers for netCDF 3.X C-library */

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
   your suggestions, improvements, bug-reports, and patches, for NCO.
   Please contact the project at http://sourceforge.net/projects/nco or by writing

   Charlie Zender
   Department of Earth System Science
   University of California at Irvine
   Irvine, CA 92697-3100
 */

/* Usage: */
/* #include "nco_netcdf.h" *//* Wrappers for netCDF 3.X C-library */

#ifndef NCO_NETCDF_H /* Contents have not yet been inserted in current source file */
#define NCO_NETCDF_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Definitions */
/* nco_fl_typ provides hooks for accessing non-netCDF files with nco_* routines */
enum nco_fl_typ{ /* [enm] File type */
  nco_fl_typ_nc, /* 0, netCDF file */
  nco_fl_typ_hd5 /* 1, HDF5 file */
}; /* end nco_fl_typ enum */

/* Utility Routines */
extern void nco_err_exit(int,char *);
extern char *nco_typ_sng(nc_type);
extern char *c_type_nm(nc_type);
extern char *fortran_type_nm(nc_type);
extern void nco_dfl_case_nctype_err(void);
extern int nco_typ_lng(nc_type);

/* File Routines */
extern int nco_create(const char *,int,int *);
extern int nco_open(const char *,int,int *);
extern int nco_redef(int);
extern int nco_enddef(int);
extern int nco_sync(int);
extern int nco_abort(int);
extern int nco_close(int);
extern int nco_inq(int,int *,int *,int *,int *);
extern int nco_inq_ndims(int,int *);
extern int nco_inq_nvars(int,int *);
extern int nco_inq_natts(int,int *);
extern int nco_inq_unlimdim(int,int *);

/* Dimension Routines */
extern int nco_def_dim(int,const char *,long,int *);
extern int nco_inq_dimid(int,char *,int *);
extern int nco_inq_dimid_flg(int,char *,int *);
extern int nco_inq_dim(int,int,char *,long *);
extern int nco_inq_dim_flg(int,int,char *,long *);
extern int nco_inq_dimname(int,int,char *);
extern int nco_inq_dimlen(int,int,long *);
extern int nco_rename_dim(int,int,const char *);

/* Variable Routines */
extern int nco_def_var(int,const char *,nc_type,int,int *,int *);
extern int nco_inq_var(int,int,char *,nc_type *,int *,int *,int *);
extern int nco_inq_varid(int,char *,int *);
extern int nco_inq_varid_flg(int,char *,int *);
extern int nco_inq_varname(int,int,char *);
extern int nco_inq_vartype(int,int,nc_type *);
extern int nco_inq_varndims(int,int,int *);
extern int nco_inq_vardimid(int,int,int *);
extern int nco_inq_varnatts(int,int,int *);
extern int nco_rename_var(int,int,const char *);
extern int nco_copy_var(int,int,int);

extern int nco_get_var1(int,int,const long *,void *,nc_type);
extern int nco_put_var1(int,int,const long *,void *,nc_type);
extern int nco_get_vara(int,int,const long *,const long *,void *,nc_type);
extern int nco_put_vara(int,int,const long *,const long *,void *,nc_type);
extern int nco_get_varm(int,int,const long *,const long *,const long *,const long *,void *,nc_type);
extern int nco_put_varm(int,int,const long *,const long *,const long *,const long *,void *,nc_type);

/* Attribute Routines */
extern int nco_inq_att(int,int,const char *,nc_type *,long *);
extern int nco_inq_att_flg(int,int,const char *,nc_type *,long *);
extern int nco_inq_attid(int,int,const char *,int *);
extern int nco_inq_atttype(int,int,const char *,nc_type *); 
extern int nco_inq_attlen(int,int,const char *,long *); 
extern int nco_inq_attname(int,int,int,char *); 
extern int nco_copy_att(int,int,const char *,int,int);
extern int nco_rename_att(int,int,const char *,const char *); 
extern int nco_del_att(int,int,const char *);
extern int nco_put_att(int,int,const char *,nc_type,long,void *);
extern int nco_get_att(int,int,const char *,void *,nc_type);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_NETCDF_H */
