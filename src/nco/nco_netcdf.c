/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_netcdf.c,v 1.29 2003-09-16 21:55:36 zender Exp $ */

/* Purpose: Wrappers for netCDF 3.X C-library */

/* Copyright (C) 1995--2003 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

#include "nco_netcdf.h" /* Wrappers for netCDF 3.X C-library */

/* Organization: 
   1. Utility routines, e.g., nco_typ_lng()
   These routines have no direct netCDF library counterpart
   2. File-routine wrappers, e.g., nco_open()
   3. Dimension-routine wrappers, e.g., nco_dimid()
   4. Variable-routine wrappers, e.g., nco_get_var()
   5. Attribute-routine wrappers, e.g., nco_put_att()

   Naming Convention: Where appropriate, routine name is identical to netCDF C-library name,
   except nc_ is replaced by nco_

   Argument Ordering Convention: Routines follow same argument order as netCDF C-library 
   Additional arguments, such as nc_type, are appended to end of argument list 

   Return value convention: Functions return a success/failure code
   Errors in netCDF functions cause NCO wrapper to abort, except *_flg() wrappers
   *_flg() wrappers allow limited, pre-defined, netCDF errors to be non-fatal

   Circularity convention: It is important that none of the error diagnostics
   in these functions call other nco_??? routines because if everything
   starts failing then errors will produce circular diagnostics.
   To ensure this is the case, it is only safe to print diagnostics on
   variables which are supposed to be valid on input.
*/

/* Utility routines not defined by netCDF library, but useful in working with it */
void 
nco_err_exit /* [fnc] Print netCDF error message, routine name, then exit */
(int rcd, /* I [enm] netCDF error code */ 
 char *msg) /* I [sng] supplemental error message */
{
  /* Purpose: Print netCDF error message, routine name, then exit
     Routine is called by all wrappers when a fatal error is encountered
     msg variable allows wrapper to pass more descriptive information than 
     is contained in the netCDF-defined error message.
     Use msg to print, e.g., the name of variable which caused the error */
  char sbr_nm[]="nco_err_exit()";
  switch(rcd){
  case NC_ERANGE: (void)fprintf(stdout,"ERROR Result not representable in output file\nHINT: This may occur when an arithmetic operation results in a value not representible by the output variable type and NCO attempts to write that variable to an output file, with, e.g., nc_put_var*(). If this occurs, you might permanently promote the variable before attempting the arithmetic operation. For example, ncap -s \"foo=double(foo);\""); break;
  } /* end switch */
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"%s: ERROR %s\n%s\n",sbr_nm,msg,nc_strerror(rcd));
    exit(EXIT_FAILURE);
  } /* endif error */
} /* end nco_err_exit() */

size_t
nco_typ_lng /* [fnc]   */
(nc_type nco_typ) 
{ 
  /* Purpose: Return native size of specified netCDF type
     Routine is used to determine memory required to store variables in RAM */
  switch(nco_typ){ 
  case NC_FLOAT: 
    return sizeof(float); 
  case NC_DOUBLE: 
    return sizeof(double); 
  case NC_INT: 
    return sizeof(long); 
  case NC_SHORT: 
    return sizeof(short); 
  case NC_CHAR: 
    return sizeof(unsigned char); 
  case NC_BYTE: 
    return sizeof(signed char); 
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */ 
  
  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return (int)-1;
} /* end nco_typ_lng() */ 

char *
nco_typ_sng /* [fnc]   */
(nc_type type)
/*  
   nc_type type: I [enm] netCDF type
   char *nco_typ_sng(): O [sng] string describing type
*/
{
  switch(type){
  case NC_FLOAT:
    return "NC_FLOAT";
  case NC_DOUBLE:
    return "NC_DOUBLE";
  case NC_INT:
    return "NC_INT";
  case NC_SHORT:
    return "NC_SHORT";
  case NC_CHAR:
    return "NC_CHAR";
  case NC_BYTE:
    return "NC_BYTE";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return (char *)NULL;
} /* end nco_typ_sng() */

char * /* O [sng] Native C type */
c_typ_nm /* [fnc] Return string describing native C type */
(nc_type type) /* O [enm] netCDF type */
{
  /* Purpose: Divine internal (native) C type string from netCDF external type enum
     fxm: This breaks on Crays where both NC_FLOAT and NC_DOUBLE are native type float */
  switch(type){
  case NC_FLOAT:
    return "float";
  case NC_DOUBLE:
    return "double";
  case NC_INT:
    return "signed long int";
  case NC_SHORT:
    return "signed short int";
  case NC_CHAR:
    return "unsigned char";
  case NC_BYTE:
    return "signed char";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return (char *)NULL;
} /* end c_typ_nm() */

char * /* O [sng] Native Fortran77 type */
f77_typ_nm /* [fnc] Return string describing native Fortran77 type */
(nc_type type) /* O [enm] netCDF type */
{
  /* Purpose: Divine internal (native) Fortran type string from netCDF external type enum */
  switch(type){
  case NC_FLOAT:
    return "real*4";
  case NC_DOUBLE:
    return "real*8";
  case NC_INT:
    return "integer*4";
  case NC_SHORT:
    return "integer*2";
  case NC_CHAR:
    return "character";
  case NC_BYTE:
    return "character";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return (char *)NULL;
} /* end f77_typ_nm() */

char * /* O [sng] Native Fortran90 type */
f90_typ_nm /* [fnc] Return string describing native Fortran90 type */
(nc_type type) /* O [enm] netCDF type */
{
  /* Purpose: Divine internal (native) Fortran type string from netCDF external type enum */
  switch(type){
  case NC_FLOAT:
    return "real(selected_real_kind(p=6))";
  case NC_DOUBLE:
    return "real(selected_real_kind(p=12))";
  case NC_INT:
    return "integer(selected_int_kind(6))";
  case NC_SHORT:
    return "integer(selected_int_kind(2))";
  case NC_CHAR:
    return "character(1)";
  case NC_BYTE:
    return "character(1)";
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */

  /* Some C compilers, e.g., SGI cc, need a return statement at the end of non-void functions */
  return (char *)NULL;
} /* end f90_typ_nm() */

void 
nco_dfl_case_nc_type_err(void) /* [fnc]   */
{
  /* Purpose: Convenience routine for printing error and exiting when
     switch(nctype) statement receives an illegal default case
     NCO emits warnings when compiled by GCC with -DNETCDF2_ONLY since, 
     apparently, there are a whole bunch of things besides numeric
     types in the old nctype enum and gcc warns about enums that are
     not exhaustively considered in switch() statements. 
     All these default statements can be removed with netCDF3 interface
     so perhaps these should be surrounded with #ifdef NETCDF2_ONLY
     constructs, but they actually do make sense for netCDF3 as well
     so I have implemented a uniform error function, nco_dfl_case_nc_type_err(), 
     to be called by all routines which emit errors only when compiled with
     NETCDF2_ONLY.
     This makes the behavior easy to modify or remove in the future.

     Placing this in its own routine also has the virtue of saving many lines 
     of code since this function is used in many many switch() statements. */
  char sbr_nm[]="nco_dfl_case_nc_type_err()";
  (void)fprintf(stdout,"%s: ERROR switch(nctype) statement fell through to default case, which is illegal.\nNot handling the default case causes gcc to emit warnings when compiling NCO with the NETCDF2_ONLY token (because nctype definition is braindead in netCDF2.x). Exiting...\n",sbr_nm);
  abort();
  exit(EXIT_FAILURE);
} /* end nco_dfl_case_nc_type_err() */

/* Begin file-level routines */
int
nco_create(const char *fl_nm,int cmode,int *nc_id)
{
  /* Purpose: Wrapper for nc_create() */
  int rcd=NC_NOERR;
  int fl_in_typ=nco_fl_typ_nc; /* [enm] File format */
  if(fl_in_typ == nco_fl_typ_nc){
    rcd=nc_create(fl_nm,cmode,nc_id);
    if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_create");
#ifdef HDF5
  }else if(fl_in_typ == nco_fl_typ_hdf5){
    hid_t hdf_out; /* [hnd] HDF file handle */
    hdf_out=H5Fcreate(fl_nm,H5F_ACC_TRUNC,H5P_DEFAULT,H5P_DEFAULT);
#endif /* HDF5 */
  }else{
    (void)fprintf(stderr,"nco_create() reports unknown fl_typ = %d\n",fl_in_typ);
    exit(EXIT_FAILURE);
  } /* endelse */
  return rcd;
} /* end nco_create */

int
nco_open(const char *fl_nm,int mode,int *nc_id)
{
  /* Purpose: Wrapper for nc_open() */
  int rcd=NC_NOERR;
  int fl_in_typ=nco_fl_typ_nc; /* [enm] File format */
  if(fl_in_typ == nco_fl_typ_nc){
    rcd=nc_open(fl_nm,mode,nc_id);
    if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_open");
#ifdef HDF5
  }else if(fl_in_typ == nco_fl_typ_hdf5){
    hid_t hdf_out; /* [hnd] HDF file handle */
    hdf_out=H5Fopen(fl_nm,H5F_ACC_RDWR,H5P_DEFAULT);
#endif /* HDF5 */
  }else{
    (void)fprintf(stderr,"nco_open() reports unknown fl_typ = %d\n",fl_in_typ);
    exit(EXIT_FAILURE);
  } /* endelse */
  return rcd;
} /* end nco_open */

int
nco_redef(int nc_id)
{
  /* Purpose: Wrapper for nc_redef() */
  int rcd=NC_NOERR;
  rcd=nc_redef(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_redef");
  return rcd;
} /* end nco_redef */

int 
nco_set_fill(int nc_id,int fill_mode,int *old_mode)
{
  /* Purpose: Wrapper for nc_set_fill() */
  int rcd=NC_NOERR;
  rcd=nc_set_fill(nc_id,fill_mode,old_mode);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_set_fill");
  return rcd;
} /* end nco_set_fill */

int 
nco_enddef(int nc_id)
{
  /* Purpose: Wrapper for nc_enddef() */
  int rcd=NC_NOERR;
  rcd=nc_enddef(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_enddef");
  return rcd;
} /* end nco_enddef */

int 
nco_sync(int nc_id)
{
  /* Purpose: Wrapper for nc_sync() */
  int rcd=NC_NOERR;
  rcd=nc_sync(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_sync");
  return rcd;
} /* end nco_sync */

int 
nco_abort(int nc_id)
{
  /* Purpose: Wrapper for nc_abort() */
  int rcd=NC_NOERR;
  rcd=nc_abort(nc_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_abort");
  return rcd;
} /* end nco_abort */

int
nco_close(int nc_id)
{
  /* Purpose: Wrapper for nc_close() */
  int rcd=NC_NOERR;
  int fl_in_typ=nco_fl_typ_nc; /* [enm] File format */
  if(fl_in_typ == nco_fl_typ_nc){
    rcd=nc_close(nc_id);
    if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_close");
#ifdef HDF5
  }else if(fl_in_typ == nco_fl_typ_hdf5){
    herr_t rcd_hdf; /* [enm] Return success code */
    hdf_out=H5Fclose(nc_id);
#endif /* HDF5 */
  }else{
    (void)fprintf(stderr,"Unknown fl_typ = %d in nco_close()\n",fl_in_typ);
    exit(EXIT_FAILURE);
  } /* endelse */
  return rcd;
} /* end nco_close */

int
nco_inq(int nc_id,int *dmn_nbr_fl,int *var_nbr_fl,int *att_glb_nbr,int *rec_dmn_id)
{
  /* Purpose: Wrapper for nc_inq() */
  int rcd=NC_NOERR ;
  rcd=nc_inq(nc_id,dmn_nbr_fl,var_nbr_fl,att_glb_nbr,rec_dmn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq");
  return rcd;
} /* end nco_inq */

int
nco_inq_ndims(int nc_id,int *dmn_nbr_fl)
{
  /* Purpose: Wrapper for nc_inq_ndims() */
  int rcd=NC_NOERR ;
  rcd=nc_inq_ndims(nc_id,dmn_nbr_fl);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_ndims");
  return rcd;
}/* end nco_inq_ndims */

int
nco_inq_nvars(int nc_id,int *var_nbr_fl)
{
  /* Purpose: Wrapper for nc_inq_nvars() */
  int rcd=NC_NOERR ;
  rcd=nc_inq_nvars(nc_id,var_nbr_fl);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_nvars");
  return rcd;
}/* end nco_inq_nvars */

int
nco_inq_natts(int nc_id,int *att_glb_nbr)
{
  /* Purpose: Wrapper for nc_inq_natts() */
  int rcd=NC_NOERR ;
  rcd=nc_inq_natts(nc_id,att_glb_nbr);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_natts");
  return rcd;
}/* end nco_inq_natts */

int
nco_inq_unlimdim(int nc_id,int *rec_dmn_id)
{
  /* Purpose: Wrapper for nc_inq_unlimdim() */
  int rcd=NC_NOERR ;
  rcd=nc_inq_unlimdim(nc_id,rec_dmn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_unlimdim");
  return rcd;
}/* end nco_inq_unlimdim */
/* End File routines */

/* Begin Dimension routines (_dim) */
int 
nco_def_dim(int nc_id,const char *dmn_nm,long dmn_sz,int *dmn_id)
{
  /* Purpose: Wrapper for nc_def_dim() */
  int rcd=NC_NOERR ;
  rcd=nc_def_dim(nc_id,dmn_nm,(size_t)dmn_sz,dmn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_dim");
  return rcd;
} /* end nco_def_dim */

int
nco_inq_dimid(int nc_id,char *dmn_nm,int *dmn_id)
{
  /* Purpose: Wrapper for nc_inq_dimid() */
  int rcd=NC_NOERR ;
  rcd=nc_inq_dimid(nc_id,dmn_nm,dmn_id);
  if(rcd == NC_EBADDIM){
    (void)fprintf(stdout,"ERROR nco_inq_dimid() reports requested dimension \"%s\" is not in input file\n",dmn_nm);
    exit(EXIT_FAILURE);
  } /* endif */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimid");
  return rcd;
} /* end nco_inq_dimid */

int
nco_inq_dimid_flg(int nc_id,char *dmn_nm,int *dmn_id)
{
  /* Purpose: Wrapper for nc_inq_dimid() which does not require success */
  int rcd=NC_NOERR ;
  rcd=nc_inq_dimid(nc_id,dmn_nm,dmn_id);
  if(rcd == NC_EBADDIM) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimid_flg");
  return rcd;
} /* end nco_inq_dimid */

int
nco_inq_dim(int nc_id,int dmn_id,char *dmn_nm,long *dmn_sz)
{
  /* Purpose: Wrapper for nc_inq_dim() */
  int rcd=NC_NOERR ;
  rcd=nc_inq_dim(nc_id,dmn_id,dmn_nm,(size_t *)dmn_sz);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dim");
  return rcd;
}/* end nco_inq_dim */

int
nco_inq_dim_flg(int nc_id,int dmn_id,char *dmn_nm,long *dmn_sz)
{
  /* Purpose: Wrapper for nc_inq_dim_flg() which does not require success */
  int rcd=NC_NOERR ;
  rcd=nc_inq_dim(nc_id,dmn_id,dmn_nm,(size_t *)dmn_sz);
  if(rcd == NC_EBADDIM) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dim_flg");
  return rcd;
}/* end nco_inq_dim */

int
nco_inq_dimname(int nc_id,int dmn_id,char *dmn_nm)
{
  /* Purpose: Wrapper for nc_inq_dimname() */
  int rcd=NC_NOERR ;
  rcd=nc_inq_dimname(nc_id,dmn_id,dmn_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimname");
  return rcd;
} /* end nco_inq_dimname */

int
nco_inq_dimlen(int nc_id,int dmn_id,long *dmn_sz)
{
  /* Purpose: Wrapper for nc_inq_dimlen() */
  int rcd=NC_NOERR;
  rcd=nc_inq_dimlen(nc_id,dmn_id,(size_t *)dmn_sz);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_dimlen");
  return rcd;
} /* end nco_inq_dimlen */

int
nco_rename_dim(int nc_id,int dmn_id,const char *dmn_nm)
{
  /* Purpose: Wrapper for nc_rename_dim() */
  int rcd=NC_NOERR;
  rcd=nc_rename_dim(nc_id,dmn_id,dmn_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_rename_dim");
  return rcd;
}  /* end nco_inq_rename_dim */

/* End Dimension routines */

/* Begin Variable routines (_var) */
int 
nco_def_var(int nc_id,const char *var_nm,nc_type var_typ,int dmn_nbr,int *dmn_id,int *var_id)
{
  /* Purpose: Wrapper for nc_def_var() */
  int rcd=NC_NOERR;
  rcd=nc_def_var(nc_id,var_nm,var_typ,dmn_nbr,dmn_id,var_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_def_var");
  return rcd;
} /* end nco_def_var */

int 
nco_inq_var(int nc_id,int var_id,char *var_nm,nc_type *var_typ,int *dmn_nbr,int *dmn_id,int *nbr_att)
{
  /* Purpose: Wrapper for nco_inq_var() */
  int rcd=NC_NOERR;
  rcd=nc_inq_var(nc_id,var_id,var_nm,var_typ,dmn_nbr,dmn_id,nbr_att);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_var");
  return rcd;
} /* end nco_inq_var */

int 
nco_inq_varid(const int nc_id,const char * const var_nm,int * const var_id)
{
  /* Purpose: Wrapper for nc_inq_varid() */
  int rcd=NC_NOERR;
  rcd=nc_inq_varid(nc_id,var_nm,var_id);
  if(rcd == NC_ENOTVAR) (void)fprintf(stdout,"ERROR nco_inq_varid() reports requested variable \"%s\" is not in input file\n",var_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varid");
  return rcd;
} /* end nco_inq_varid */

int 
nco_inq_varid_flg(const int nc_id,const char * const var_nm,int * const var_id)
{
  /* Purpose: Wrapper for nc_inq_varid_flg() which does not require success */
  int rcd=NC_NOERR;
  rcd=nc_inq_varid(nc_id,var_nm,var_id);
  if(rcd == NC_ENOTVAR) return rcd;
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varid_flg");
  return rcd;
} /* end nco_inq_varid */

int 
nco_inq_varname(int nc_id,int var_id,char *var_nm)
{
  /* Purpose: Wrapper for nc_inq_varname() */
  int rcd=NC_NOERR;
  rcd=nc_inq_varname(nc_id,var_id,var_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varname");
  return rcd;
} /* end nco_inq_varname */

int 
nco_inq_vartype(int nc_id,int var_id,nc_type *var_typ)
{
  /* Purpose: Wrapper for nc_inq_vartype() */
  int rcd=NC_NOERR;
  rcd=nc_inq_vartype(nc_id,var_id,var_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_vartype");
  return rcd;
} /* end nco_inq_vartype */

int 
nco_inq_varndims(int nc_id,int var_id,int *dmn_nbr)
{
  /* Purpose: Wrapper for nc_inq_varndims() */
  int rcd=NC_NOERR;
  rcd=nc_inq_varndims(nc_id,var_id,dmn_nbr);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varndims");
  return rcd;
} /* end nco_inq_varndims */

int 
nco_inq_vardimid(int nc_id,int var_id,int *dmn_id)
{
  /* Purpose: Wrapper for nc_inq_vardimid() */
  int rcd=NC_NOERR;
  rcd=nc_inq_vardimid(nc_id,var_id,dmn_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_vardimid");
  return rcd;
} /* end nco_inq_vardimid */

int 
nco_inq_varnatts(int nc_id,int var_id,int *nbr_att)
{
  /* Purpose: Wrapper for nc_inq_varnatts() */
  int rcd=NC_NOERR;
  rcd=nc_inq_varnatts(nc_id,var_id,nbr_att);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_varnatts");
  return rcd;
} /* end nco_inq_varnatts */

int
nco_rename_var(int nc_id,int var_id,const char *var_nm)
{
  /* Purpose: Wrapper for nc_rename_var() */
  int rcd=NC_NOERR;
  rcd=nc_rename_var(nc_id,var_id,var_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_rename_var");
  return rcd;
} /* end nco_rename_var */

int
nco_copy_var(int nc_in_id,int var_id,int nc_out_id)
{
  /* Purpose: Wrapper for nc_copy_var() */
  int rcd=NC_NOERR;
  rcd=nc_copy_var(nc_in_id,var_id,nc_out_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_copy_var");
  return rcd;
} /* end nco_copy_var */

/* End _var */

/* Start _get _put _var */
int 
nco_get_var1(int nc_id,int var_id,const long *srt,void *vp,nc_type var_typ)
{
  /* Purpose: Wrapper for nc_get_var1_*() */
  int rcd=NC_NOERR;
  switch(var_typ){
  case NC_FLOAT: rcd=nc_get_var1_float(nc_id,var_id,(size_t *)srt,(float *)vp); break;
  case NC_DOUBLE: rcd=nc_get_var1_double(nc_id,var_id,(size_t *)srt,(double *)vp); break;
  case NC_INT: rcd=nc_get_var1_long(nc_id,var_id,(size_t *)srt,(long *)vp); break;
  case NC_SHORT: rcd=nc_get_var1_short(nc_id,var_id,(size_t *)srt,(short *)vp); break;
  case NC_CHAR: rcd=nc_get_var1_text(nc_id,var_id,(size_t *)srt,(char *)vp); break;
  case NC_BYTE: rcd=nc_get_var1_schar(nc_id,var_id,(size_t *)srt,(signed char *)vp); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_var1");
  return rcd;
} /* end nco_get_var1 */

int 
nco_put_var1(int nc_id,int var_id,const long *srt,void *vp,nc_type type)
{
  /* Purpose: Wrapper for nc_put_var1_*() */
  int rcd=NC_NOERR;
  switch(type){
  case NC_FLOAT: rcd=nc_put_var1_float(nc_id,var_id,(size_t *)srt,(float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_var1_double(nc_id,var_id,(size_t *)srt,(double *)vp); break;
  case NC_INT: rcd=nc_put_var1_long(nc_id,var_id,(size_t *)srt,(long *)vp); break;
  case NC_SHORT: rcd=nc_put_var1_short(nc_id,var_id,(size_t *)srt,(short *)vp); break;
  case NC_CHAR: rcd=nc_put_var1_text(nc_id,var_id,(size_t *)srt,(const char *)vp); break;
  case NC_BYTE: rcd=nc_put_var1_schar(nc_id,var_id,(size_t *)srt,(signed char *)vp); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_var1");
  return rcd;
} /* end nco_put_var1 */

int 
nco_get_vara(int nc_id,int var_id,const long *srt,const long *cnt,void *vp,nc_type type)
{
  /* Purpose: Wrapper for nc_get_vara_*() */
  int rcd=NC_NOERR;
  switch(type){
  case NC_FLOAT: rcd=nc_get_vara_float(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(float *)vp); break;
  case NC_DOUBLE: rcd=nc_get_vara_double(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(double *)vp); break;
  case NC_INT: rcd=nc_get_vara_long(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(long *)vp); break;
  case NC_SHORT: rcd=nc_get_vara_short(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(short *)vp); break;
  case NC_CHAR: rcd=nc_get_vara_text(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(char *)vp); break;
  case NC_BYTE: rcd=nc_get_vara_schar(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(signed char *)vp); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_vara");
  return rcd;
} /* end nco_get_vara */

int 
nco_put_vara(int nc_id,int var_id,const long *srt,const long *cnt,void *vp,nc_type type)
{
  /* Purpose: Wrapper for nc_put_vara_*() */
  int rcd=NC_NOERR;
  switch(type){
  case NC_FLOAT: rcd=nc_put_vara_float(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_vara_double(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(double *)vp); break;
  case NC_INT: rcd=nc_put_vara_long(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(long *)vp); break;
  case NC_SHORT: rcd=nc_put_vara_short(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(short *)vp); break;
  case NC_CHAR: rcd=nc_put_vara_text(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(const char *)vp); break;
  case NC_BYTE: rcd=nc_put_vara_schar(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(signed char *)vp); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_vara");
  return rcd;
} /* end nco_put_vara */

int 
nco_get_varm(int nc_id,int var_id,const long *srt,const long *cnt,const long *srd,const long *map,void *vp,nc_type type)
{
  /* Purpose: Wrapper for nc_get_varm_*() */
  int rcd=NC_NOERR;
  
  switch(type){
  case NC_FLOAT: rcd=nc_get_varm_float(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(float *)vp); break;
  case NC_DOUBLE: rcd=nc_get_varm_double(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(double *)vp); break;
  case NC_INT: rcd=nc_get_varm_long(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(long *)vp); break;
  case NC_SHORT: rcd=nc_get_varm_short(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(short *)vp); break;
  case NC_CHAR: rcd=nc_get_varm_text(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(char *)vp); break;
  case NC_BYTE: rcd=nc_get_varm_schar(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(signed char *)vp); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_varm");
  return rcd;
} /* end nco_get_varm */

int 
nco_put_varm(int nc_id,int var_id,const long *srt,const long *cnt,const long *srd,const long *map,void *vp,nc_type type)
{
  /* Purpose: Wrapper for nc_put_varm_*() */
  int rcd=NC_NOERR;
  switch(type){
  case NC_FLOAT: rcd=nc_get_varm_float(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_varm_double(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(double *)vp); break;
  case NC_INT: rcd=nc_put_varm_long(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(long *)vp); break;
  case NC_SHORT: rcd=nc_put_varm_short(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(short *)vp); break;
  case NC_CHAR: rcd=nc_put_varm_text(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(const char *)vp); break;
  case NC_BYTE: rcd=nc_put_varm_schar(nc_id,var_id,(size_t *)srt,(size_t *)cnt,(ptrdiff_t *)srd,(ptrdiff_t *)map,(signed char *)vp); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_varm");
  return rcd;
} /* end nco_put_varm */
/* End Variable routines */

/* Begin Attribute routines (_att) */
int 
nco_inq_att(int nc_id,int var_id,const char *att_nm,nc_type *att_typ,long *att_sz)
{
  /* Purpose: Wrapper for nc_inq_att() */
  int rcd=NC_NOERR;
  rcd=nc_inq_att(nc_id,var_id,att_nm,att_typ,(size_t *)att_sz);
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"var_id: %d, att_nm: %s\n",var_id,att_nm);
    nco_err_exit(rcd,"nco_inq_att");
  } /* endif */
  return rcd;
} /* end nco_inq_att */

int 
nco_inq_att_flg(int nc_id,int var_id,const char *att_nm,nc_type *att_typ,long *att_sz) 
{
  /* Purpose: Wrapper for nc_inq_att() */
  int rcd=NC_NOERR;
  rcd=nc_inq_att(nc_id,var_id,att_nm,att_typ,(size_t *)att_sz);
  if(rcd == NC_ENOTATT) return rcd;
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"var_id: %d, att_nm: %s\n",var_id,att_nm);
    nco_err_exit(rcd,"nco_inq_att_flg");
  } /* endif */
  return rcd;
} /* end nco_inq_att_flg */

int 
nco_inq_attid(int nc_id,int var_id,const char *att_nm,int *att_id) 
{
  /* Purpose: Wrapper for nc_inq_attid() */
  int rcd=NC_NOERR;
  rcd=nc_inq_attid(nc_id,var_id,att_nm,att_id);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_attid");
  return rcd;
} /* end nco_inq_attid */

int 
nco_inq_attid_flg(int nc_id,int var_id,const char *att_nm,int *att_id) 
{
  /* Purpose: Wrapper for nc_inq_attid() */
  int rcd=NC_NOERR;
  rcd=nc_inq_attid(nc_id,var_id,att_nm,att_id);
  if(rcd == NC_ENOTATT) return rcd;
  if(rcd != NC_NOERR){
    (void)fprintf(stderr,"var_id: %d, att_nm: %s\n",var_id,att_nm);
    nco_err_exit(rcd,"nco_inq_attid_flg");
  } /* endif */
  return rcd;
} /* end nco_inq_attid_flg */

int 
nco_inq_atttype(int nc_id,int var_id,const char *att_nm,nc_type *att_typ) 
{
  /* Purpose: Wrapper for nc_inq_atttype() */
  int rcd=NC_NOERR;
  rcd=nc_inq_atttype(nc_id,var_id,att_nm,att_typ);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_atttype");
  return rcd;
} /* end nco_inq_atttype */

int 
nco_inq_attlen(int nc_id,int var_id,const char *att_nm,long *att_sz) 
{
  /* Purpose: Wrapper for nc_inq_attlen() */
  int rcd=NC_NOERR;
  rcd=nc_inq_attlen(nc_id,var_id,att_nm,(size_t *)att_sz);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_attlen");
  return rcd;
} /* end nco_inq_attlen */

int 
nco_inq_attname(int nc_id,int var_id,int att_id,char *att_nm) 
{
  /* Purpose: Wrapper for nc_inq_attname() */
  int rcd=NC_NOERR;
  rcd=nc_inq_attname(nc_id,var_id,att_id,att_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_attname");
  return rcd;
} /* end nco_inq_attname */

int
nco_copy_att(int nc_id_in,int var_id_in,const char *att_nm,int nc_id_out,int var_id_out)
{  
  /* Purpose: Wrapper for nc_copy_att() */
  int rcd=NC_NOERR;
  rcd=nc_copy_att(nc_id_in,var_id_in,att_nm,nc_id_out,var_id_out);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_inq_copy_att");
  return rcd;
}  /* end nco_copy_att */

int 
nco_rename_att(int nc_id,int var_id,const char *att_nm,const char *att_new_nm) 
{
  /* Purpose: Wrapper for nc_rename_att() */
  int rcd=NC_NOERR;
  rcd=nc_rename_att(nc_id,var_id,att_nm,att_new_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_rename_att");
  return rcd;
}  /* end nco_rename_att */

int 
nco_del_att(int nc_id,int var_id,const char *att_nm)
{
  /* Purpose: Wrapper for nc_del_att() */
  int rcd=NC_NOERR;
  rcd=nc_del_att(nc_id,var_id,att_nm);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_del_att");
  return rcd;
} /* end nco_del_att */

int 
nco_put_att(int nc_id,int var_id,const char *att_nm,nc_type att_typ,long att_len,void *vp)
{
  /* Purpose: Wrapper for nc_put_att_*() */
  int rcd=NC_NOERR;
  switch(att_typ){
  case NC_FLOAT: rcd=nc_put_att_float(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(float *)vp); break;
  case NC_DOUBLE: rcd=nc_put_att_double(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(double *)vp); break;
  case NC_INT: rcd=nc_put_att_long(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(long *)vp); break;
  case NC_SHORT: rcd=nc_put_att_short(nc_id,var_id,att_nm,att_typ,(size_t)att_len,(short *)vp); break;
  case NC_CHAR: rcd=nc_put_att_text(nc_id,var_id,att_nm,(size_t)att_len,(const char *)vp); break;
  case NC_BYTE: rcd=nc_put_att_schar(nc_id,var_id,att_nm,att_typ,(size_t) att_len,(signed char *)vp); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_put_att");
  return rcd;
} /* end nco_put_att */

int 
nco_get_att(int nc_id,int var_id,const char *att_nm,void *vp,nc_type att_typ)
{
  /* Purpose: Wrapper for nc_get_att_*() */
  int rcd=NC_NOERR;
  switch(att_typ){
  case NC_FLOAT: rcd=nc_get_att_float(nc_id,var_id,att_nm,(float *)vp); break;
  case NC_DOUBLE: rcd=nc_get_att_double(nc_id,var_id,att_nm,(double *)vp); break;
  case NC_INT: rcd=nc_get_att_long(nc_id,var_id,att_nm,(long *)vp); break;
  case NC_SHORT: rcd=nc_get_att_short(nc_id,var_id,att_nm,(short *)vp); break;
  case NC_CHAR: rcd=nc_get_att_text(nc_id,var_id,att_nm,(char *)vp); break;
  case NC_BYTE: rcd=nc_get_att_schar(nc_id,var_id,att_nm,(signed char *)vp); break;
  default: nco_dfl_case_nc_type_err(); break;
  } /* end switch */
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_att");
  return rcd;
} /* end nco_get_att */
/* End Attribute routines */
