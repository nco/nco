/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_netcdf.h,v 1.88 2012-10-22 22:09:31 zender Exp $ */

/* Purpose: NCO wrappers for netCDF C library */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
#include "nco_netcdf.h" *//* NCO wrappers for netCDF C library */

#ifndef NCO_NETCDF_H /* Contents have not yet been inserted in current source file */
#define NCO_NETCDF_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#ifdef ENABLE_PNETCDF
#include <mpi.h> /* MPI definitions */
#include <pnetcdf.h> /* ANL Parallel netCDF definitions and C library */
#endif /* !ENABLE_PNETCDF */

/* Personal headers */
#include "nco_typ.h" /* Type definitions, opaque types */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Definitions */

/* Begin Utility Routines */
const char * /* O [sng] Native C type */
c_typ_nm /* [fnc] Return string describing native C type */
(const nc_type type); /* O [enm] netCDF type */

const char * /* O [sng] Native Fortran77 type */
f77_typ_nm /* [fnc] Return string describing native Fortran77 type */
(const nc_type type); /* O [enm] netCDF type */

const char * /* O [sng] Native Fortran90 type */
f90_typ_nm /* [fnc] Return string describing native Fortran90 type */
(const nc_type type); /* O [enm] netCDF type */

void 
nco_dfl_case_nc_type_err /* [fnc] Print error and exit for illegal switch(nc_type) case */
(void);

void 
nco_dfl_case_prg_id_err /* [fnc] Print error and exit for illegal switch(prg_id) case */
(void);

void
nco_sng_cnv_err /* [fnc] Print error and exit for failed strtol()-type calls */
(const char *cnv_sng, /* I [sng] String to convert */
 const char *sng_cnv_fnc, /* I [sng] Name of function used to convert string */
 const char *err_ptr); /* I [chr] First illegal character in string */
/* end nco_sng_cnv_err() */

void 
nco_err_exit /* [fnc] Print netCDF error message, routine name, then exit */
(const int rcd, /* I [enm] netCDF error code */ 
 const char * const msg); /* I [sng] Supplemental error message */

size_t /* O [B] Native type size */
nco_typ_lng /* [fnc] Convert netCDF type enum to native type size */
(const nc_type nco_typ); /* I [enm] netCDF type */

const char * /* O [sng] String describing type */
nco_typ_sng /* [fnc] Convert netCDF type enum to string */
(const nc_type type); /* I [enm] netCDF type */

const char * /* O [sng] String describing file format */
nco_fmt_sng /* [fnc] Convert netCDF file format enum to string */
(const int fl_fmt); /* I [enm] netCDF file format */
/* End Utility Routines */

#if 0
#ifdef HAVE_NETCDF4_H
/* netCDF4 routines defined by Unidata netCDF4 Library libnetcdf.a */
int nco_open_par(const char * const fl_nm,const int mode,MPI_Comm mpi_cmm,MPI_Info mpi_nfo,int * const nc_id);
#endif /* !HAVE_NETCDF4_H */
#endif /* !0 */
#ifdef ENABLE_PNETCDF
/* pnetCDF routines defined by ANL Parallel netCDF Library libpnetcdf.a */
int ncompi_open(MPI_Comm mpi_cmm,const char * const fl_nm,const int mode,MPI_Info mpi_nfo,int * const nc_id);
#endif /* !ENABLE_PNETCDF */

/* Begin file-level routines */
int nco_create(const char * const fl_nm,const int cmode,int * const nc_id);
int nco__create(const char * const fl_nm,const int cmode,const size_t sz_ntl,size_t * const bfr_sz_hnt,int * const nc_id);
int nco_open(const char * const fl_nm,const int mode,int * const nc_id);
int nco_open_flg(const char * const fl_nm,const int mode,int * const nc_id);
int nco__open(const char * const fl_nm,const int mode,size_t * const bfr_sz_hnt,int * const nc_id);
int nco_redef(const int nc_id);
int nco_set_fill(const int nc_id,const int fill_mode,int * const old_mode);
int nco_enddef(const int nc_id);
int nco__enddef(const int nc_id,const size_t hdr_pad);
int nco_sync(const int nc_id);
int nco_abort(const int nc_id);
int nco_close(const int nc_id);
int nco_inq(const int nc_id,int * const dmn_nbr_fl,int * const var_nbr_fl,int * const att_glb_nbr,int * const rec_dmn_id);
#if NEED_NC_INQ_FORMAT
/* Stub for nc_inq_format(), which appeared in netCDF 3.6.1 or 3.6.2 */
int nc_inq_format(int nc_id, int * const fl_fmt);
#endif /* !NEED_NC_INQ_FORMAT */
int nco_inq_format(const int nc_id,int * const fl_fmt);
int nco_inq_ncid(const int nc_id,const char * const grp_nm,int * const grp_id);
int nco_inq_ncid_flg(const int nc_id,const char * const grp_nm,int * const grp_id);
int nco_inq_ndims(const int nc_id,int * const dmn_nbr_fl);
int nco_inq_nvars(const int nc_id,int * const var_nbr_fl);
int nco_inq_natts(const int nc_id,int * const att_glb_nbr);
int nco_inq_unlimdim(const int nc_id,int * const rec_dmn_id);
int nco_inq_unlimdim_flg(const int nc_id,int * const rec_dmn_id);
/* End File routines */

/* Begin Group routines (_grp) */
int nco_def_grp(const int nc_id,const char * const grp_nm,int * const grp_id);
int nco_rename_grp(const int nc_id,const int grp_id,const char * const grp_nm);
int nco_inq_dimids(const int nc_id,int * const dmn_nbr,int * const dmn_ids,int flg_prn);
int nco_inq_grpname(const int nc_id,char * const grp_nm);
int nco_inq_grpname_full(const int nc_id,size_t * grp_nm_lng,char * const grp_nm_fll);
int nco_inq_grpname_len(const int nc_id,size_t * const grp_nm_lng);
int nco_inq_grps(const int nc_id,int * const grp_nbr,int * const grp_ids);
int nco_inq_grp_full_ncid(const int nc_id,char * const grp_nm_fll,int * const grp_id);
int nco_inq_grp_full_ncid_flg(const int nc_id,char * const grp_nm_fll,int * const grp_id);
int nco_inq_grp_ncid(const int nc_id,char * const grp_nm,int * const grp_id);
int nco_inq_grp_ncid_flg(const int nc_id,char * const grp_nm,int * const grp_id);
int nco_inq_grp_parent(const int nc_id,int * const prn_id);
int nco_inq_varids(const int nc_id,int * const var_nbr,int * const var_ids);
/* End Group routines */

/* Begin Dimension routines (_dim) */
int nco_def_dim(const int nc_id,const char * const dmn_nm,const long dmn_sz,int * const dmn_id);
int nco_inq_dimid(const int nc_id,const char * const dmn_nm,int * const dmn_id);
int nco_inq_dimid_flg(const int nc_id,const char * const dmn_nm,int * const dmn_id);
int nco_inq_dim(const int nc_id,const int dmn_id,char *dmn_nm,long *dmn_sz);
int nco_inq_dim_flg(const int nc_id,const int dmn_id,char *dmn_nm,long *dmn_sz);
int nco_inq_dimname(const int nc_id,const int dmn_id,char *dmn_nm);
int nco_inq_dimlen(const int nc_id,const int dmn_id,long *dmn_sz);
int nco_rename_dim(const int nc_id,const int dmn_id,const char * const dmn_nm);
int nco_inq_unlimdims(const int nc_id,int *nbr_dmn_ult,int *dmn_ids_ult);
/* End Dimension routines */

/* Begin Variable routines (_var) */
int nco_copy_var(const int nc_in_id,const int var_id,const int nc_out_id);
int nco_def_var(const int nc_id,const char * const var_nm,const nc_type var_typ,const int dmn_nbr,const int * const dmn_id,int * const var_id);
int nco_def_var_chunking(const int nc_id,const int var_id,const int srg_typ,const size_t * const cnk_sz);
int nco_def_var_deflate(const int nc_id,const int var_id,const int shuffle,const int deflate,const int dfl_lvl);
int nco_inq_var(const int nc_id,const int var_id,char * const var_nm,nc_type * const var_typ,int * const dmn_nbr,int * const dmn_id,int * const nbr_att);
int nco_inq_var_chunking(const int nc_id,const int var_id,int * const srg_typ,size_t * const cnk_sz);
int nco_inq_var_deflate(const int nc_id,const int var_id,int * const shuffle,int * const deflate,int * const dfl_lvl);
int nco_inq_var_fletcher32(const int nc_id,const int var_id,int * const chk_typ);
int nco_inq_var_packing(const int nc_id,const int var_id,int * const packing);
int nco_inq_vardimid(const int nc_id,const int var_id,int * const dmn_id);
int nco_inq_varid(const int nc_id,const char * const var_nm,int * const var_id);
int nco_inq_varid_flg(const int nc_id,const char * const var_nm,int * const var_id);
int nco_inq_varname(const int nc_id,const int var_id,char * const var_nm);
int nco_inq_varnatts(const int nc_id,const int var_id,int * const nbr_att);
int nco_inq_varndims(const int nc_id,const int var_id,int * const dmn_nbr);
int nco_inq_vartype(const int nc_id,const int var_id,nc_type * const var_typ);
int nco_rename_var(const int nc_id,const int var_id,const char * const var_nm);

/* Start _get _put _var */
int nco_get_var1(const int nc_id,const int var_id,const long * const srt,void * const vp,const nc_type var_typ);
int nco_put_var1(const int nc_id,const int var_id,const long * const srt,const void * const vp,const nc_type type);
int nco_get_vara(const int nc_id,const int var_id,const long * const srt,const long * const cnt,void * const vp,const nc_type type);
int nco_put_vara(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const void * const vp,const nc_type type);
int nco_get_vars(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const long * const srd,void * const vp,const nc_type type);
int nco_put_vars(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const long * const srd,const void * const vp,const nc_type type);
int nco_get_varm(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const long * const srd,const long * const map,void * const vp,const nc_type type);
int nco_put_varm(const int nc_id,const int var_id,const long * const srt,const long * const cnt,const long * const srd,const long * const map,const void * const vp,const nc_type type);
/* End Variable routines */

/* Begin Attribute routines (_att) */
int nco_inq_att(const int nc_id,const int var_id,const char * const att_nm,nc_type * const att_typ,long * const att_sz);
int nco_inq_att_flg(const int nc_id,const int var_id,const char * const att_nm,nc_type * const att_typ,long * const att_sz); 
int nco_inq_attid(const int nc_id,const int var_id,const char * const att_nm,int * const att_id); 
int nco_inq_attid_flg(const int nc_id,const int var_id,const char * const att_nm,int * const att_id); 
int nco_inq_atttype(const int nc_id,const int var_id,const char * const att_nm,nc_type * const att_typ); 
int nco_inq_attlen(const int nc_id,const int var_id,const char * const att_nm,long * const att_sz); 
int nco_inq_attlen_flg(const int nc_id,const int var_id,const char * const att_nm,long * const att_sz); 
int nco_inq_attname(const int nc_id,const int var_id,const int att_id,char * const att_nm); 
int nco_copy_att(const int nc_id_in,const int var_id_in,const char * const att_nm,const int nc_id_out,const int var_id_out);
int nco_rename_att(const int nc_id,const int var_id,const char * const att_nm,const char * const att_new_nm); 
int nco_del_att(const int nc_id,const int var_id,const char * const att_nm);
int nco_put_att(const int nc_id,const int var_id,const char * const att_nm,const nc_type att_typ,const long att_len,const void * const vp);
int nco_get_att(const int nc_id,const int var_id,const char * const att_nm,void * const vp,const nc_type att_typ);
/* End Attribute routines */

/* Begin libnetcdf prototypes (i.e., for completely new library routines not yet in netcdf.h */
  int nc_rename_grp(const int nc_id,const int grp_id,const char * const grp_nm);

/* Begin netCDF4 stubs */
#ifndef HAVE_NETCDF4_H
  /* Stubs so netCDF4 functions work without protection in netCDF3 environments */
# ifdef NC_HAVE_NEW_CHUNKING_API
  /* Newer, post-200906 netCDF4 API has chk_sz as const
     netcdf.h signals this API with NC_HAVE_NEW_CHUNKING_API as of ~200911 */
  int nc_def_var_chunking(const int nc_id,const int var_id,const int srg_typ,const size_t * const cnk_sz);
# else /* !NC_HAVE_NEW_CHUNKING_API */
  /* Older, pre-200906 netCDF4 API has chk_sz as non-const */
  int nc_def_var_chunking(const int nc_id,const int var_id,const int srg_typ,size_t * const cnk_sz);
# endif /* !NC_HAVE_NEW_CHUNKING_API */
  int nc_inq_var_chunking(const int nc_id,const int var_id,int * const srg_typ,size_t * const cnk_sz);
  int nc_def_var_deflate(const int nc_id,const int var_id,const int shuffle,const int deflate,const int dfl_lvl);
  int nc_inq_var_deflate(const int nc_id,const int var_id,int * const shuffle, int * const deflate,int * const dfl_lvl);
  int nc_inq_var_fletcher32(const int nc_id,const int var_id,int * const chk_typ);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  int NCO_GET_VAR1_UBYTE(const int nc_id,const int var_id,const size_t *srt,nco_ubyte *ubp);
  int NCO_GET_VAR1_USHORT(const int nc_id,const int var_id,const size_t *srt,nco_ushort *usp);
  int NCO_GET_VAR1_UINT(const int nc_id,const int var_id,const size_t *srt,nco_uint *uip);
  int NCO_GET_VAR1_INT64(const int nc_id,const int var_id,const size_t *srt,nco_int64 *i64p);
  int NCO_GET_VAR1_UINT64(const int nc_id,const int var_id,const size_t *srt,nco_uint64 *ui64p);
  int NCO_GET_VAR1_STRING(const int nc_id,const int var_id,const size_t *srt,nco_string *sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  int NCO_PUT_VAR1_UBYTE(const int nc_id,const int var_id,const size_t *srt,const nco_ubyte *ubp);
  int NCO_PUT_VAR1_USHORT(const int nc_id,const int var_id,const size_t *srt,const nco_ushort *usp);
  int NCO_PUT_VAR1_UINT(const int nc_id,const int var_id,const size_t *srt,const nco_uint *uip);
  int NCO_PUT_VAR1_INT64(const int nc_id,const int var_id,const size_t *srt,const nco_int64 *i64p);
  int NCO_PUT_VAR1_UINT64(const int nc_id,const int var_id,const size_t *srt,const nco_uint64 *ui64p);
  /* 20090712: For unknown reason autobuild sees netcdf4.h header when it should not
     Moreover, it thinks "const nco_string *sng" conflicts with "const char **sng"
     in all nc_put_var?_string() and nc_put_att_string() functions. */
  /*  int NCO_PUT_VAR1_STRING(const int nc_id,const int var_id,const size_t *srt,const nco_string * const sngp);*/
  int NCO_PUT_VAR1_STRING(const int nc_id,const int var_id,const size_t *srt,const char **sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  int NCO_GET_VARA_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_ubyte *ubp);
  int NCO_GET_VARA_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_ushort *usp);
  int NCO_GET_VARA_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_uint *uip);
  int NCO_GET_VARA_INT64(const int ncrd_id,const int var_id,const size_t *srt,const size_t *cnt,nco_int64 *i64p);
  int NCO_GET_VARA_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_uint64 *ui64p);
  int NCO_GET_VARA_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,nco_string *sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  int NCO_PUT_VARA_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_ubyte *ubp);
  int NCO_PUT_VARA_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_ushort *usp);
  int NCO_PUT_VARA_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_uint *uip);
  int NCO_PUT_VARA_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_int64 *i64p);
  int NCO_PUT_VARA_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const nco_uint64 *ui64p);
  int NCO_PUT_VARA_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const char **sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  int NCO_GET_VARS_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_ubyte *ubp);
  int NCO_GET_VARS_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_ushort *usp);
  int NCO_GET_VARS_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_uint *uip);
  int NCO_GET_VARS_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_int64 *i64p);
  int NCO_GET_VARS_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_uint64 *ui64p);
  int NCO_GET_VARS_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,nco_string *sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  int NCO_PUT_VARS_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const nco_ubyte *ubp);
  int NCO_PUT_VARS_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd, const nco_ushort *usp);
  int NCO_PUT_VARS_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd, const nco_uint *uip);
  int NCO_PUT_VARS_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd, const nco_int64 *i64p);
  int NCO_PUT_VARS_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const nco_uint64 *ui64p);
  int NCO_PUT_VARS_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const char **sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  int NCO_GET_VARM_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_ubyte *ubp);
  int NCO_GET_VARM_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_ushort *usp);
  int NCO_GET_VARM_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_uint *uip);
  int NCO_GET_VARM_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_int64 *i64p);
  int NCO_GET_VARM_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_uint64 *ui64p);
  int NCO_GET_VARM_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,nco_string *sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  int NCO_PUT_VARM_UBYTE(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_ubyte *ubp);
  int NCO_PUT_VARM_USHORT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_ushort *usp);
  int NCO_PUT_VARM_UINT(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_uint *uip);
  int NCO_PUT_VARM_INT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_int64 *i64p);
  int NCO_PUT_VARM_UINT64(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const nco_uint64 *ui64p);
  int NCO_PUT_VARM_STRING(const int nc_id,const int var_id,const size_t *srt,const size_t *cnt,const ptrdiff_t *srd,const ptrdiff_t *map,const char **sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  /* 20051119: netcdf4 library did not support these until alpha10, still does not support nco_put/get_att_ubyte() */
  int NCO_PUT_ATT_UBYTE(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_ubyte *ubp);
  int NCO_PUT_ATT_USHORT(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_ushort *usp);
  int NCO_PUT_ATT_UINT(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_uint *uip);
  int NCO_PUT_ATT_INT64(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_int64 *i64p);
  int NCO_PUT_ATT_UINT64(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const nco_uint64 *ui64p);
  int NCO_PUT_ATT_STRING(const int nc_id,const int var_id,const char *att_nm,const nc_type att_typ,size_t att_len,const char **sngp);
#endif /* HAVE_NETCDF4_H */
#ifndef HAVE_NETCDF4_H
  /* 20051119: netcdf4 library did not support these until alpha10, still does not support nco_put/get_att_ubyte() */
  int NCO_GET_ATT_UBYTE(const int nc_id,const int var_id,const char *att_nm,nco_ubyte *ubp);
  int NCO_GET_ATT_USHORT(const int nc_id,const int var_id,const char *att_nm,nco_ushort *usp);
  int NCO_GET_ATT_UINT(const int nc_id,const int var_id,const char *att_nm,nco_uint *uip);
  int NCO_GET_ATT_INT64(const int nc_id,const int var_id,const char *att_nm,nco_int64 *i64p);
  int NCO_GET_ATT_UINT64(const int nc_id,const int var_id,const char *att_nm,nco_uint64 *ui64p);
  int NCO_GET_ATT_STRING(const int nc_id,const int var_id,const char *att_nm,nco_string *sngp);
#endif /* HAVE_NETCDF4_H */
/* End netCDF4 stubs */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_NETCDF_H */
