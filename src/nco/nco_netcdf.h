/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_netcdf.h,v 1.14 2002-08-21 11:47:42 zender Exp $ */

/* Purpose: Wrappers for netCDF 3.X C-library */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
#include "nco_netcdf.h" *//* Wrappers for netCDF 3.X C-library */

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
void nco_err_exit(int,char *);
char *nco_typ_sng(nc_type);
char *c_typ_nm(nc_type);
char *f77_typ_nm(nc_type);

char * /* O [sng] Native C type */
c_typ_nm /* [fnc] Return string describing native C type */
(nc_type type); /* O [enm] netCDF type */

char * /* O [sng] Native Fortran77 type */
f77_typ_nm /* [fnc] Return string describing native Fortran77 type */
(nc_type type); /* O [enm] netCDF type */

char * /* O [sng] Native Fortran90 type */
f90_typ_nm /* [fnc] Return string describing native Fortran90 type */
(nc_type type); /* O [enm] netCDF type */

void nco_dfl_case_nc_type_err(void);
size_t nco_typ_lng(nc_type);

/* File Routines */
int nco_create(const char *,int,int *);
int nco_open(const char *,int,int *);
int nco_redef(int);
int nco_enddef(int);
int nco_sync(int);
int nco_set_fill(int,int,int *);
int nco_abort(int);
int nco_close(int);
int nco_inq(int,int *,int *,int *,int *);
int nco_inq_ndims(int,int *);
int nco_inq_nvars(int,int *);
int nco_inq_natts(int,int *);
int nco_inq_unlimdim(int,int *);

/* Dimension Routines */
int nco_def_dim(int,const char *,long,int *);
int nco_inq_dimid(int,char *,int *);
int nco_inq_dimid_flg(int,char *,int *);
int nco_inq_dim(int,int,char *,long *);
int nco_inq_dim_flg(int,int,char *,long *);
int nco_inq_dimname(int,int,char *);
int nco_inq_dimlen(int,int,long *);
int nco_rename_dim(int,int,const char *);

/* Variable Routines */
int nco_def_var(int,const char *,nc_type,int,int *,int *);
int nco_inq_var(int,int,char *,nc_type *,int *,int *,int *);
int nco_inq_varid(const int,const char * const,int * const);
int nco_inq_varid_flg(const int,const char * const,int * const);
int nco_inq_varname(int,int,char *);
int nco_inq_vartype(int,int,nc_type *);
int nco_inq_varndims(int,int,int *);
int nco_inq_vardimid(int,int,int *);
int nco_inq_varnatts(int,int,int *);
int nco_rename_var(int,int,const char *);
int nco_copy_var(int,int,int);

int nco_get_var1(int,int,const long *,void *,nc_type);
int nco_put_var1(int,int,const long *,void *,nc_type);
int nco_get_vara(int,int,const long *,const long *,void *,nc_type);
int nco_put_vara(int,int,const long *,const long *,void *,nc_type);
int nco_get_varm(int,int,const long *,const long *,const long *,const long *,void *,nc_type);
int nco_put_varm(int,int,const long *,const long *,const long *,const long *,void *,nc_type);

/* Attribute Routines */
int nco_inq_att(int,int,const char *,nc_type *,long *);
int nco_inq_att_flg(int,int,const char *,nc_type *,long *);
int nco_inq_attid(int,int,const char *,int *);
int nco_inq_attid_flg(int,int,const char *,int *);
int nco_inq_atttype(int,int,const char *,nc_type *); 
int nco_inq_attlen(int,int,const char *,long *); 
int nco_inq_attname(int,int,int,char *); 
int nco_copy_att(int,int,const char *,int,int);
int nco_rename_att(int,int,const char *,const char *); 
int nco_del_att(int,int,const char *);
int nco_put_att(int,int,const char *,nc_type,long,void *);
int nco_get_att(int,int,const char *,void *,nc_type);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_NETCDF_H */
