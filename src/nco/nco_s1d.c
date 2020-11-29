/* $Header$ */

/* Purpose: NCO utilities for Sparse-1D (S1D) datasets */

/* Copyright (C) 2015--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_s1d.h" /* Sparse-1D datasets */

const char * /* O [sng] String describing sparse-type */
nco_s1d_sng /* [fnc] Convert sparse-1D type enum to string */
(const nco_s1d_typ_enm nco_s1d_typ) /* I [enm] Sparse-1D type enum */
{
  /* Purpose: Convert sparse-type enum to string */
  switch(nco_s1d_typ){
  case nco_s1d_pft: return "Sparse PFT dimension (pfts1d format)" ;
  case nco_s1d_clm: return "Sparse Column dimension (cols1d format)";
  default: nco_dfl_case_generic_err(); break;
  } /* !nco_s1d_typ_enm */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_s1d_sng() */

int /* O [rcd] Return code */
nco_s1d_unpack /* [fnc] Unpack sparse-1D CLM/ELM variables into full file */
(rgr_sct * const rgr, /* I/O [sct] Regridding structure */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal Table */
{
  /* Purpose: Read sparse CLM/ELM input file, inflate and write into output file */

  /* Usage:
     ncks -v cols1d_topoglc ~/data/bm/elm_mali_rst.nc ~/foo.nc */

  const char fnc_nm[]="nco_s1d_unpack()"; /* [sng] Function name */

  int rcd=NC_NOERR;

  const int dmn_nbr_3D=3; /* [nbr] Rank of 3-D grid variables */
  const int dmn_nbr_grd_max=dmn_nbr_3D; /* [nbr] Maximum rank of grid variables */

#ifdef ENABLE_S1D

  char *fl_in;
  char *fl_out;
  char *fl_out_tmp=NULL_CEWI;
  char *var_nm;
  char *wvl_nm;
  char *xdm_nm;
  char *ydm_nm;

  int dmn_ids[dmn_nbr_grd_max]; /* [id] Dimension IDs array for output variable */

  int dmn_idx_wvl=int_CEWI; /* [idx] Index of wavelength dimension */
  int dmn_idx_ydm=int_CEWI; /* [idx] Index of y-coordinate dimension */
  int dmn_idx_xdm=int_CEWI; /* [idx] Index of x-coordinate dimension */
  int dmn_id_wvl; /* [id] Wavelength dimension ID */
  int dmn_id_xdm; /* [id] X-dimension ID */
  int dmn_id_ydm; /* [id] Y-dimension ID */
  int dfl_lvl; /* [enm] Deflate level [0..9] */
  /* Terraref raw image files can be ~64 GB large so use netCDF4 */
  int fl_out_fmt=NC_FORMAT_NETCDF4; /* [enm] Output file format */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;
  int var_id; /* [id] Current variable ID */

  long dmn_srt[dmn_nbr_grd_max];
  long dmn_cnt[dmn_nbr_grd_max];

  long wvl_nbr; /* [nbr] Number of wavelengths */
  long xdm_nbr; /* [nbr] Number of pixels in x-dimension */
  long ydm_nbr; /* [nbr] Number of pixels in y-dimension */
  long wvl_idx;
  long ydm_idx;
  long var_sz; /* [nbr] Size of variable */

  nc_type var_typ_in; /* [enm] NetCDF type-equivalent of binary data (raw imagery) */
  nc_type var_typ_out; /* [enm] NetCDF type of data in output file */

  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  nco_s1d_typ_enm nco_s1d_typ; /* [enm] Sparse-1D type of input variable */

  ptr_unn var_raw;
  ptr_unn var_val;
  
  if(strstr("cols1d",var_nm)){
    nco_s1d_typ=nco_s1d_clm;
  }else if(strstr("pfts1d",var_nm)){
    nco_s1d_typ=nco_s1d_pft;
  }else{
    (void)fprintf(stderr,"%s: ERROR %s reports variable %s does not appear to be sparse\n",nco_prg_nm_get(),fnc_nm,var_nm);
    nco_exit(EXIT_FAILURE);
  } /* !strstr() */

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s reports variable %s is sparse type %s",nco_prg_nm_get(),fnc_nm,var_nm,nco_s1d_sng(nco_s1d_typ));
  } /* !dbg */

  /* Initialize local copies of command-line values */
  dfl_lvl=rgr->dfl_lvl;
  fl_in=rgr->fl_in;
  fl_out=rgr->fl_out;
  var_nm=rgr->var_nm;

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s Terraref metadata: ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"wvl_nbr = %li, xdm_nbr = %li, ydm_nbr = %li, s1d_typ = %s, var_typ_in = %s, var_typ_out = %s\n",wvl_nbr,xdm_nbr,ydm_nbr,nco_s1d_sng(nco_s1d_typ),nco_typ_sng(var_typ_in),nco_typ_sng(var_typ_out));
  } /* endif dbg */

  /* Free output data memory */
#endif /* !ENABLE_S1D */

  return rcd;
} /* !nco_s1d_unpack() */
