/* $Header$ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_sld.h" /* Swath-Like Data */

trr_sct * /* O [sct] Terraref structure */
nco_trr_ini /* [fnc] Initialize Terraref structure */
(const char * const cmd_ln, /* I [sng] Command-line */
 const int dfl_lvl, /* I [enm] Deflate level [0..9] */
 char **trr_arg, /* I [sng] Terraref arguments */
 const int trr_arg_nbr, /* I [nbr] Number of Terraref arguments */
 char * const trr_in, /* I [sng] File containing raw Terraref imagery */
 char * const trr_out, /* I [sng] File containing netCDF Terraref imagery */
 char * const trr_wxy) /* I [sng] Terraref dimension sizes */
{
  /* Purpose: Initialize Terraref structure */
     
  const char fnc_nm[]="nco_trr_ini()";
  
  int cnv_nbr; /* [nbr] Number of elements converted by sscanf() */

  trr_sct *trr;

  /* Allocate */
  trr=(trr_sct *)nco_malloc(sizeof(trr_sct));
  
  /* Initialize variable directly or indirectly set via command-line (except for key-value arguments) */
  trr->cmd_ln=strdup(cmd_ln); /* [sng] Command-line */
  trr->dfl_lvl=dfl_lvl; /* I [enm] Deflate level [0..9] */

  trr->trr_arg=trr_arg; /* [sng] Terraref arguments */
  trr->trr_nbr=trr_arg_nbr; /* [nbr] Number of Terraref arguments */

  trr->fl_in=trr_in; /* [sng] File containing raw Terrraref imagery */
  trr->fl_out=trr_out; /* [sng] File containing netCDF Terraref imagery */
  trr->fl_out_tmp=NULL_CEWI; /* [sng] Temporary file containing netCDF Terraref imagery */

  /* Initialize arguments after copying */
  if(!trr->fl_out) trr->fl_out=(char *)strdup("/data/zender/terraref/trr_out.nc");
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"trr_nbr = %d, ",trr->trr_nbr);
    (void)fprintf(stderr,"fl_in = %s, ",trr->fl_in ? trr->fl_in : "NULL");
    (void)fprintf(stderr,"fl_out = %s, ",trr->fl_out ? trr->fl_out : "NULL");
    (void)fprintf(stderr,"fl_out_tmp = %s, ",trr->fl_out_tmp ? trr->fl_out_tmp : "NULL");
    (void)fprintf(stderr,"\n");
  } /* endif dbg */
  
  /* Parse extended kvm options */
  int trr_var_idx; /* [idx] Index over trr_lst (i.e., all names explicitly specified in all "--trr var1[,var2]=val" options) */
  int trr_var_nbr=0;
  kvm_sct *trr_lst; /* [sct] List of all regrid specifications */
  char *sng_fnl=NULL;

  /* Join arguments together */
  sng_fnl=nco_join_sng(trr_arg,trr_arg_nbr);
  trr_lst=nco_arg_mlt_prs(sng_fnl);

  if(sng_fnl) sng_fnl=(char *)nco_free(sng_fnl);

  for(int index=0;(trr_lst+index)->key;index++, trr_var_nbr++);
  
  /* NULL-initialize key-value properties required for string variables */
  trr->ttl=NULL; /* [sng] Title */
  trr->wvl_nm=NULL; /* [sng] Name of wavelength dimension */
  trr->xdm_nm=NULL; /* [sng] Name of x-coordinate dimension */
  trr->ydm_nm=NULL; /* [sng] Name of y-coordinate dimension */
  trr->var_nm=NULL; /* [sng] Variable containing imagery */
  trr->wvl_bnd_nm=NULL; /* [sng] Name of dimension to employ for wavelength bounds */
  trr->xdm_bnd_nm=NULL; /* [sng] Name of dimension to employ for x-coordinate bounds */
  trr->ydm_bnd_nm=NULL; /* [sng] Name of dimension to employ for y-coordinate bounds */

  /* Initialize numeric key-value properties used in data processing */
  trr->var_typ_in=NC_USHORT; /* [enm] NetCDF type-equivalent of binary data (raw imagery) */
  trr->var_typ_out=NC_USHORT; /* [enm] NetCDF type of data in output file */
  trr->wvl_nbr=272; /* [nbr] Number of wavelengths */
  trr->xdm_nbr=384; /* [nbr] Number of pixels in x-dimension */
  trr->ydm_nbr=893; /* [nbr] Number of pixels in y-dimension */
  trr->ntl_typ_in=nco_trr_ntl_bil; /* [enm] Interleave-type of raw data */
  trr->ntl_typ_out=nco_trr_ntl_bsq; /* [enm] Interleave-type of output data */

  /* Initialize variables settable by global switches */
  if(trr_wxy){
    cnv_nbr=sscanf(trr_wxy,"%ld,%ld,%ld",&trr->wvl_nbr,&trr->xdm_nbr,&trr->ydm_nbr);
    assert(cnv_nbr == 3);
    if(cnv_nbr == 3) cnv_nbr=3; /* CEWI */
  } /* !trr_wxy */

  /* Parse key-value properties */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  for(trr_var_idx=0;trr_var_idx<trr_var_nbr;trr_var_idx++){
    if(!strcasecmp(trr_lst[trr_var_idx].key,"ntl_in")){
      trr->ntl_typ_in=nco_trr_sng_ntl(trr_lst[trr_var_idx].val);
      continue;
    } /* !ntl_in */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"ntl_out")){
      trr->ntl_typ_out=nco_trr_sng_ntl(trr_lst[trr_var_idx].val);
      continue;
    } /* !ntl_out */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"ttl")){
      trr->ttl=(char *)strdup(trr_lst[trr_var_idx].val);
      continue;
    } /* !ttl */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"var_nm")){
      trr->var_nm=(char *)strdup(trr_lst[trr_var_idx].val);
      continue;
    } /* !var_nm */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"typ_in")){
      trr->var_typ_in=nco_sng2typ(trr_lst[trr_var_idx].val);
      continue;
    } /* !var_typ_in */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"typ_out")){
      trr->var_typ_out=nco_sng2typ(trr_lst[trr_var_idx].val);
      continue;
    } /* !var_typ_out */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"wxy")){
      cnv_nbr=sscanf(trr_lst[trr_var_idx].val,"%ld,%ld,%ld",&trr->wvl_nbr,&trr->xdm_nbr,&trr->ydm_nbr);
      assert(cnv_nbr == 3);
      continue;
    } /* !wxy */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"wvl_nbr")){
      trr->wvl_nbr=strtol(trr_lst[trr_var_idx].val,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(trr_lst[trr_var_idx].val,"strtol",sng_cnv_rcd);
      continue;
    } /* !wvl_nbr */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"xdm_nbr")){
      trr->xdm_nbr=strtol(trr_lst[trr_var_idx].val,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(trr_lst[trr_var_idx].val,"strtol",sng_cnv_rcd);
      continue;
    } /* !xdm_nbr */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"ydm_nbr")){
      trr->ydm_nbr=strtol(trr_lst[trr_var_idx].val,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(trr_lst[trr_var_idx].val,"strtol",sng_cnv_rcd);
      continue;
    } /* !ydm_nbr */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"wvl_nm")){
      trr->wvl_nm=(char *)strdup(trr_lst[trr_var_idx].val);
      continue;
    } /* !wvl_nm */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"xdm_nm")){
      trr->xdm_nm=(char *)strdup(trr_lst[trr_var_idx].val);
      continue;
    } /* !xdm_nm */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"ydm_nm")){
      trr->ydm_nm=(char *)strdup(trr_lst[trr_var_idx].val);
      continue;
    } /* !ydm_nm */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"wvl_bnd_nm")){
      trr->wvl_bnd_nm=(char *)strdup(trr_lst[trr_var_idx].val);
      continue;
    } /* !wvl_bnd_nm */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"xdm_bnd_nm")){
      trr->xdm_bnd_nm=(char *)strdup(trr_lst[trr_var_idx].val);
      continue;
    } /* !xdm_bnd_nm */
    if(!strcasecmp(trr_lst[trr_var_idx].key,"ydm_bnd_nm")){
      trr->ydm_bnd_nm=(char *)strdup(trr_lst[trr_var_idx].val);
      continue;
    } /* !ydm_bnd_nm */
    (void)fprintf(stderr,"%s: ERROR %s reports unrecognized key-value option to --trr switch: %s\n",nco_prg_nm_get(),fnc_nm,trr_lst[trr_var_idx].key);
    nco_exit(EXIT_FAILURE);
  } /* end for */
  
  /* Revert to defaults for any names not specified on command-line */
  if(!trr->ttl) trr->ttl=(char *)strdup("None given (supply with --trr ttl=\"Title\")"); /* [sng] Title */
  if(!trr->wvl_nm) trr->wvl_nm=(char *)strdup("wavelength"); /* [sng] Name of wavelength dimension */
  if(!trr->xdm_nm) trr->xdm_nm=(char *)strdup("x"); /* [sng] Name of x-coordinate dimension */
  if(!trr->ydm_nm) trr->ydm_nm=(char *)strdup("y"); /* [sng] Name of y-coordinate dimension */
  if(!trr->var_nm) trr->var_nm=(char *)strdup("xps_img"); /* [sng] Variable containing imagery */
  if(!trr->wvl_bnd_nm) trr->wvl_bnd_nm=(char *)strdup("wvl_bnds"); /* [sng] Name of dimension to employ for wavelength bounds */
  if(!trr->xdm_bnd_nm) trr->xdm_bnd_nm=(char *)strdup("x_bnds"); /* [sng] Name of dimension to employ for x-coordinate bounds */
  if(!trr->ydm_bnd_nm) trr->ydm_bnd_nm=(char *)strdup("y_bnds"); /* [sng] Name of dimension to employ for y-coordinate bounds */
  
  /* Free kvms */
  if(trr_lst) trr_lst=nco_kvm_lst_free(trr_lst,trr_var_nbr);
  
  return trr;
} /* end nco_trr_ini() */

trr_sct * /* O [sct] Pointer to free'd Terraref structure */
nco_trr_free /* [fnc] Deallocate Terraref structure */
(trr_sct *trr) /* I/O [sct] Terraref structure */
{
  /* Purpose: Free all dynamic memory in Terraref structure */

  /* free() standalone command-line arguments */
  if(trr->cmd_ln) trr->cmd_ln=(char *)nco_free(trr->cmd_ln);
  if(trr->fl_in) trr->fl_in=(char *)nco_free(trr->fl_in);
  if(trr->fl_out) trr->fl_out=(char *)nco_free(trr->fl_out);
  if(trr->fl_out_tmp) trr->fl_out_tmp=(char *)nco_free(trr->fl_out_tmp);
  if(trr->var_nm) trr->var_nm=(char *)nco_free(trr->var_nm);

  /* free() memory used to construct KVMs */
  if(trr->trr_nbr > 0) trr->trr_arg=nco_sng_lst_free(trr->trr_arg,trr->trr_nbr);

  /* free() memory copied from KVMs */
  if(trr->ttl) trr->ttl=(char *)nco_free(trr->ttl);
  if(trr->wvl_nm) trr->wvl_nm=(char *)nco_free(trr->wvl_nm);
  if(trr->xdm_nm) trr->xdm_nm=(char *)nco_free(trr->xdm_nm);
  if(trr->ydm_nm) trr->ydm_nm=(char *)nco_free(trr->ydm_nm);
  if(trr->var_nm) trr->var_nm=(char *)nco_free(trr->var_nm);
  if(trr->wvl_bnd_nm) trr->wvl_bnd_nm=(char *)nco_free(trr->wvl_bnd_nm);
  if(trr->xdm_bnd_nm) trr->xdm_bnd_nm=(char *)nco_free(trr->xdm_bnd_nm);
  if(trr->ydm_bnd_nm) trr->ydm_bnd_nm=(char *)nco_free(trr->ydm_bnd_nm);

  /* Lastly, free() regrid structure itself */
  if(trr) trr=(trr_sct *)nco_free(trr);

  return trr;
} /* end nco_trr_free() */
  
int /* O [rcd] Return code */
nco_trr_read /* [fnc] Read, parse, and print contents of TERRAREF file */
(trr_sct *trr) /* I/O [sct] Terraref information */
{
  /* Purpose: Read raw TERRAREF input file, write netCDF4 output */
  const char fnc_nm[]="nco_trr_read()"; /* [sng] Function name */

  const int dmn_nbr_3D=3; /* [nbr] Rank of 3-D grid variables */
  const int dmn_nbr_grd_max=dmn_nbr_3D; /* [nbr] Maximum rank of grid variables */

  char *fl_in;
  char *fl_out;
  char *fl_out_tmp=NULL_CEWI;
  char *var_nm;
  char *wvl_nm;
  char *xdm_nm;
  char *ydm_nm;

  FILE *fp_in=NULL; /* [fl] Unformatted binary input file handle */

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

  nco_trr_ntl_typ_enm ntl_typ_in; /* [enm] Interleave-type of raw data */
  nco_trr_ntl_typ_enm ntl_typ_out; /* [enm] Interleave-type of output data */

  ptr_unn var_raw;
  ptr_unn var_val;
  
  /* Initialize local copies of command-line values */
  fl_in=trr->fl_in;
  fl_out=trr->fl_out;
  var_nm=trr->var_nm;
  wvl_nm=trr->wvl_nm;
  xdm_nm=trr->xdm_nm;
  ydm_nm=trr->ydm_nm;
  
  wvl_nbr=trr->wvl_nbr; /* bands */
  xdm_nbr=trr->xdm_nbr; /* samples */
  ydm_nbr=trr->ydm_nbr; /* lines */

  ntl_typ_in=trr->ntl_typ_in; /* [enm] Interleave-type of raw data */
  ntl_typ_out=trr->ntl_typ_out; /* [enm] Interleave-type of output data */

  var_typ_in=trr->var_typ_in; /* [enm] NetCDF type-equivalent of binary data (raw imagery) */
  var_typ_out=trr->var_typ_out; /* [enm] NetCDF type of data in output file */

  dfl_lvl=trr->dfl_lvl;

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s Terraref metadata: ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"wvl_nbr = %li, xdm_nbr = %li, ydm_nbr = %li, ntl_typ_in = %s, ntl_typ_out = %s, var_typ_in = %s, var_typ_out = %s\n",wvl_nbr,xdm_nbr,ydm_nbr,nco_trr_ntl_sng(ntl_typ_in),nco_trr_ntl_sng(ntl_typ_out),nco_typ_sng(var_typ_in),nco_typ_sng(var_typ_out));
  } /* endif dbg */

  var_sz=wvl_nbr*xdm_nbr*ydm_nbr;
  var_val.vp=(void *)nco_malloc(var_sz*nctypelen(var_typ_in));
  var_raw.vp=(void *)nco_malloc(var_sz*nctypelen(var_typ_in));
  
  /* [fnc] Open unformatted binary data file for reading */
  fp_in=nco_bnr_open(fl_in,"r");
      
  /* [fnc] Read unformatted binary data */
  nco_bnr_rd(fp_in,var_nm,var_sz,var_typ_in,var_raw.vp);

  /* [fnc] Close unformatted binary data file */
  if(fp_in) (void)nco_bnr_close(fp_in,fl_in);

  /* ENVI Image Files documentation:
     http://www.harrisgeospatial.com/docs/enviimagefiles.html
     Band Sequential: BSQ format is the simplest format, where each line of the data is followed immediately by the next line in the same spectral band. This format is optimal for spatial (x,y) access of any part of a single spectral band.
     Band-interleaved-by-pixel: BIP format stores the first pixel for all bands in sequential order, followed by the second pixel for all bands, followed by the third pixel for all bands, and so forth, interleaved up to the number of pixels. This format provides optimum performance for spectral (z) access of the image data.
     Band-interleaved-by-line: BIL format stores the first line of the first band, followed by the first line of the second band, followed by the first line of the third band, interleaved up to the number of bands. Subsequent lines for each band are interleaved in similar fashion. This format provides a compromise in performance between spatial and spectral processing and is the recommended file format for most ENVI processing tasks. */

  if(ntl_typ_in == nco_trr_ntl_bil && ntl_typ_out == nco_trr_ntl_bsq){
    /* De-interleave */
    long ln_sz; /* [nbr] Number of pixels in line */
    long ln_sz_byt; /* [B] Number of bytes in line */
    long img_sz_byt; /* [B] Image size in bytes */
    long src_fst_byt; /* [B] Line offset in bytes, source */
    long dst_fst_byt; /* [B] Line offset in bytes, destination */
    
    ln_sz=xdm_nbr;
    ln_sz_byt=ln_sz*nctypelen(var_typ_in);
    img_sz_byt=xdm_nbr*ydm_nbr*nctypelen(var_typ_in);
    
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s de-interleaving input image from ENVI type %s\n",nco_prg_nm_get(),fnc_nm,nco_trr_ntl_sng(ntl_typ_in));
    for(ydm_idx=0;ydm_idx<ydm_nbr;ydm_idx++){
      for(wvl_idx=0;wvl_idx<wvl_nbr;wvl_idx++){
	src_fst_byt=(ydm_idx*wvl_nbr+wvl_idx)*ln_sz_byt;
	dst_fst_byt=wvl_idx*img_sz_byt+ydm_idx*ln_sz_byt;
	memcpy((void *)(var_val.cp+dst_fst_byt),(void *)(var_raw.cp+src_fst_byt),ln_sz_byt);
      } /* !wvl_idx */
    } /* !ydm_idx */
  }else{
    if(var_val.vp) var_val.vp=(void *)nco_free(var_val.vp);
    var_val.vp=var_raw.vp;
    var_raw.vp=NULL;
  } /* !ntl_bil */

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    if(var_typ_in == NC_USHORT){
      long idx;
      double val_min;
      double val_max;
      double val_avg;
      val_min=var_val.usp[0];
      val_max=var_val.usp[0];
      val_avg=0.0;
      for(idx=0;idx<var_sz;idx++){
	if(var_val.usp[idx] < val_min) val_min=var_val.usp[idx];
	if(var_val.usp[idx] > val_max) val_max=var_val.usp[idx];
	val_avg+=var_val.usp[idx];
      } /* !idx */
      val_avg/=var_sz;
      (void)fprintf(stderr,"%s: INFO %s image diagnostics: min=%g, max=%g, avg=%g\n",nco_prg_nm_get(),fnc_nm,val_min,val_max,val_avg);
    } /* !NC_USHORT */
  } /* !dbg */

    /* Free input data memory */
  if(var_raw.vp) var_raw.vp=(void *)nco_free(var_raw.vp);

  /* Open grid file */  
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  /* Define dimensions */
  rcd=nco_def_dim(out_id,wvl_nm,wvl_nbr,&dmn_id_wvl);
  rcd=nco_def_dim(out_id,xdm_nm,xdm_nbr,&dmn_id_xdm);
  rcd=nco_def_dim(out_id,ydm_nm,ydm_nbr,&dmn_id_ydm);
  
  /* Define variables */
  if(ntl_typ_out == nco_trr_ntl_bsq){
    /* Band-sequential order */
    dmn_idx_wvl=0;
    dmn_idx_ydm=1;
    dmn_idx_xdm=2;
  }else if(ntl_typ_out == nco_trr_ntl_bip){
    /* Band-interleaved-by-pixel order */
    dmn_idx_wvl=2;
    dmn_idx_ydm=0;
    dmn_idx_xdm=1;
  }else if(ntl_typ_out == nco_trr_ntl_bil){
    /* Band-interleaved-by-line order */
    dmn_idx_wvl=1;
    dmn_idx_ydm=0;
    dmn_idx_xdm=2;
  }else{
    (void)fprintf(stderr,"%s: ERROR %s reports unknown ntl_typ_out = %d\n",nco_prg_nm_get(),fnc_nm,ntl_typ_out);
    nco_exit(EXIT_FAILURE);
  } /* !ntl_typ_out */
  dmn_ids[dmn_idx_wvl]=dmn_id_wvl;
  dmn_ids[dmn_idx_xdm]=dmn_id_xdm;
  dmn_ids[dmn_idx_ydm]=dmn_id_ydm;
  dmn_cnt[dmn_idx_wvl]=wvl_nbr;
  dmn_cnt[dmn_idx_xdm]=xdm_nbr;
  dmn_cnt[dmn_idx_ydm]=ydm_nbr;

  (void)nco_def_var(out_id,var_nm,var_typ_out,dmn_nbr_3D,dmn_ids,&var_id);

  if(dfl_lvl > 0){
    int shuffle; /* [flg] Turn-on shuffle filter */
    int deflate; /* [flg] Turn-on deflate filter */
    deflate=(int)True;
    shuffle=NC_SHUFFLE;
    (void)nco_def_var_deflate(out_id,var_id,shuffle,deflate,dfl_lvl);
  } /* !dfl_lvl */
  
  /* Define "units" attributes */
  rcd=nco_char_att_put(out_id,NULL,"title",trr->ttl);
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */
  rcd=nco_char_att_put(out_id,NULL,"created_by",usr_cpp);
  (void)nco_hst_att_cat(out_id,trr->cmd_ln);
  (void)nco_vrs_att_cat(out_id);
  rcd=nco_char_att_put(out_id,var_nm,"long_name","Exposure counts");
  rcd=nco_char_att_put(out_id,var_nm,"meaning","Counts on scale from 0 to 2^16-1 = 65535");
  rcd=nco_char_att_put(out_id,var_nm,"units","1");
  
  /* Begin data mode */
  (void)nco_enddef(out_id);
  
  /* Write variables */
  dmn_srt[0]=0L;
  dmn_srt[1]=0L;
  dmn_srt[2]=0L;
  rcd=nco_put_vara(out_id,var_id,dmn_srt,dmn_cnt,var_val.vp,var_typ_in);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  /* Free output data memory */
  if(var_val.vp) var_val.vp=(void *)nco_free(var_val.vp);

  return rcd;
} /* end nco_trr_read() */
  
nco_trr_ntl_typ_enm /* O [enm] Interleave-type */
nco_trr_sng_ntl /* [fnc] Convert user-supplied string to interleave-type enum */
(const char * const typ_sng) /* I [sng] String indicating interleave-type */
{
  /* Purpose: Convert user-supplied string to interleave-type */
  if(!strcasecmp(typ_sng,"bsq") || !strcasecmp(typ_sng,"band_sequential")) return nco_trr_ntl_bsq;
  else if(!strcasecmp(typ_sng,"bip") || !strcasecmp(typ_sng,"band_interleaved_by_pixel")) return nco_trr_ntl_bip;
  else if(!strcasecmp(typ_sng,"bil") || !strcasecmp(typ_sng,"band_interleaved_by_line")) return nco_trr_ntl_bil;
  else abort();
  
  return nco_trr_ntl_unk;
} /* end nco_trr_sng_ntl() */

const char * /* O [sng] String describing interleave-type */
nco_trr_ntl_sng /* [fnc] Convert interleave-type enum to string */
(const nco_trr_ntl_typ_enm nco_trr_ntl_typ) /* I [enm] Interleave-type enum */
{
  /* Purpose: Convert interleave-type enum to string */
  switch(nco_trr_ntl_typ){
  case nco_trr_ntl_bsq: return "band_sequential (BSQ format)";
  case nco_trr_ntl_bip: return "band_interleaved_by_pixel (BIP format)";
  case nco_trr_ntl_bil: return "band_interleaved_by_line (BIL format)";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_trr_ntl_sng() */

#ifdef ENABLE_ESMF
const char * /* O [sng] String version of ESMC_FileFormat_Flag */
nco_esmf_fl_fmt_sng /* [fnc] Convert ESMF file format to string */
(const int nco_esmf_fl_fmt) /* I [enm] ESMF file format enum ESMC_FileFormat_Flag */
{
  /* Purpose: Convert ESMF fileTypeFlag enum to string */
  switch(nco_esmf_fl_fmt){
  case ESMC_FILEFORMAT_UNDEFINED: return "ESMC_FILEFORMAT_UNDEFINED";
  case ESMC_FILEFORMAT_VTK: return "ESMC_FILEFORMAT_VTK";
  case ESMC_FILEFORMAT_SCRIP: return "ESMC_FILEFORMAT_SCRIP";
  case ESMC_FILEFORMAT_ESMFMESH: return "ESMC_FILEFORMAT_ESMFMESH";
  case ESMC_FILEFORMAT_ESMCGRID: return "ESMC_FILEFORMAT_ESMCGRID";
  case ESMC_FILEFORMAT_UGRID: return "ESMC_FILEFORMAT_UGRID";
  case ESMC_FILEFORMAT_GRIDSPEC: return "ESMC_FILEFORMAT_GRIDSPEC";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_fl_fmt_sng() */

const char * /* O [sng] String version of ESMC_StaggerLoc enum */
nco_esmf_stg_lcn_sng /* [fnc] Convert ESMF stagger location enum to string */
(const int nco_esmf_stg_lcn) /* I [enm] ESMF stagger location enum ESMC_StaggerLoc */
{
  /* Purpose: Convert ESMF stagger location enum ESMC_StaggerLoc to string */
  switch(nco_esmf_stg_lcn){
  case ESMC_STAGGERLOC_INVALID: return "ESMC_STAGGERLOC_INVALID";
  case ESMC_STAGGERLOC_UNINIT: return "ESMC_STAGGERLOC_UNINIT";
  case ESMC_STAGGERLOC_CENTER: return "ESMC_STAGGERLOC_CENTER";
  case ESMC_STAGGERLOC_EDGE1: return "ESMC_STAGGERLOC_EDGE1";
  case ESMC_STAGGERLOC_EDGE2: return "ESMC_STAGGERLOC_EDGE2";
  case ESMC_STAGGERLOC_CORNER: return "ESMC_STAGGERLOC_CORNER";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_stg_lcn_sng() */

const char * /* O [sng] String version of ESMC_GridItem_Flag enum */
nco_esmf_grd_itm_sng /* [fnc] Convert ESMF grid item enum to string */
(const int nco_esmf_grd_itm) /* I [enm] ESMF grid item enum ESMC_GridItem_Flag */
{
  /* Purpose: Convert ESMF grid item enum ESMC_GridItem_Flag to string */
  switch(nco_esmf_grd_itm){
  case ESMC_GRIDITEM_INVALID: return "ESMC_GRIDITEM_INVALID";
  case ESMC_GRIDITEM_UNINIT: return "ESMC_GRIDITEM_UNINIT";
  case ESMC_GRIDITEM_MASK: return "ESMC_GRIDITEM_MASK";
  case ESMC_GRIDITEM_AREA: return "ESMC_GRIDITEM_AREA";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_grd_itm_sng() */

const char * /* O [sng] String version of ESMC_CoordSys_Flag enum */
nco_esmf_crd_sys_sng /* [fnc] Convert ESMF coordinate system enum to string */
(const int nco_esmf_crd_sys) /* I [enm] ESMF coordinate system enum ESMC_CoordSys_Flag */
{
  /* Purpose: Convert ESMF coordinate system enum ESMC_CoordSys_Flag to string */
  switch(nco_esmf_crd_sys){
  case ESMC_COORDSYS_INVALID: return "ESMC_COORDSYS_INVALID";
  case ESMC_COORDSYS_UNINIT: return "ESMC_COORDSYS_UNINIT";
  case ESMC_COORDSYS_CART: return "ESMC_COORDSYS_CART";
  case ESMC_COORDSYS_SPH_DEG: return "ESMC_COORDSYS_SPH_DEG";
  case ESMC_COORDSYS_SPH_RAD: return "ESMC_COORDSYS_SPH_RAD";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_crd_sys_sng() */

const char * /* O [sng] String version of ESMC_TypeKind_Flag enum */
nco_esmf_typ_knd_sng /* [fnc] Convert ESMF type kind enum to string */
(const int nco_esmf_typ_knd) /* I [enm] ESMF type kind enum ESMC_TypeKind_Flag */
{
  /* Purpose: Convert ESMF type kind enum ESMC_TypeKind_Flag to string */
  switch(nco_esmf_typ_knd){
  case ESMC_TYPEKIND_I1: return "ESMC_TYPEKIND_I1";
  case ESMC_TYPEKIND_I2: return "ESMC_TYPEKIND_I2";
  case ESMC_TYPEKIND_I4: return "ESMC_TYPEKIND_I4";
  case ESMC_TYPEKIND_I8: return "ESMC_TYPEKIND_I8";
  case ESMC_TYPEKIND_R4: return "ESMC_TYPEKIND_R4";
  case ESMC_TYPEKIND_R8: return "ESMC_TYPEKIND_R8";
  case ESMC_TYPEKIND_LOGICAL: return "ESMC_TYPEKIND_LOGICAL";
  case ESMC_TYPEKIND_CHARACTER: return "ESMC_TYPEKIND_CHARACTER";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_typ_knd_sng() */

const char * /* O [sng] String version of ESMC_LogMsgType_Flag enum */
nco_esmf_log_msg_sng /* [fnc] Convert ESMF log message type enum to string */
(const int nco_esmf_log_msg) /* I [enm] ESMF log message type enum ESMC_LogMsgType_Flag */
{
  /* Purpose: Convert ESMF coordinate system enum ESMC_LogMsgType_Flag to string */
  switch(nco_esmf_log_msg){
  case ESMC_LOGMSG_INFO: return "ESMC_LOGMSG_INFO";
  case ESMC_LOGMSG_WARN: return "ESMC_LOGMSG_WARN";
  case ESMC_LOGMSG_ERROR: return "ESMC_LOGMSG_ERROR";
  case ESMC_LOGMSG_TRACE: return "ESMC_LOGMSG_TRACE";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_log_msg_sng() */

const char * /* O [sng] String version of ESMC_RegridMethod_Flag enum */
nco_esmf_rgr_mth_sng /* [fnc] Convert ESMF regrid method type enum to string */
(const int nco_esmf_rgr_mth) /* I [enm] ESMF regrid method type enum ESMC_RegridMethod_Flag */
{
  /* Purpose: Convert ESMF regrid method enum ESMC_RegridMethod_Flag to string */
  switch(nco_esmf_rgr_mth){
  case ESMC_REGRIDMETHOD_BILINEAR: return "ESMC_REGRIDMETHOD_BILINEAR";
  case ESMC_REGRIDMETHOD_PATCH: return "ESMC_REGRIDMETHOD_PATCH";
  case ESMC_REGRIDMETHOD_CONSERVE: return "ESMC_REGRIDMETHOD_CONSERVE";
  case ESMC_REGRIDMETHOD_NEAREST_STOD: return "ESMC_REGRIDMETHOD_NEAREST_STOD";
  case ESMC_REGRIDMETHOD_NEAREST_DTOS: return "ESMC_REGRIDMETHOD_NEAREST_DTOS";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_rgr_mth_sng() */

const char * /* O [sng] String version of ESMC_PoleMethod_Flag enum */
nco_esmf_pll_mth_sng /* [fnc] Convert ESMF pole method type enum to string */
(const int nco_esmf_pll_mth) /* I [enm] ESMF pole method type enum ESMC_PoleMethod_Flag */
{
  /* Purpose: Convert ESMF pole method enum ESMC_PoleMethod_Flag to string */
  switch(nco_esmf_pll_mth){
  case ESMC_POLEMETHOD_NONE: return "ESMC_POLEMETHOD_NONE";
  case ESMC_POLEMETHOD_ALLAVG: return "ESMC_POLEMETHOD_ALLAVG";
  case ESMC_POLEMETHOD_NPNTAVG: return "ESMC_POLEMETHOD_NPNTAVG";
  case ESMC_POLEMETHOD_TEETH: return "ESMC_POLEMETHOD_TEETH";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_pll_mth_sng() */

const char * /* O [sng] String version of ESMC_PoleKind_Flag enum */
nco_esmf_pll_knd_sng /* [fnc] Convert ESMF pole type enum to string */
(const int nco_esmf_pll_knd) /* I [enm] ESMF pole type enum ESMC_PoleKind_Flag */
{
  /* Purpose: Convert ESMF pole method enum ESMC_PoleKind_Flag to string */
  switch(nco_esmf_pll_knd){
  case ESMC_POLEKIND_NONE: return "ESMC_POLEKIND_NONE";
  case ESMC_POLEKIND_MONOPOLE: return "ESMC_POLEKIND_MONOPOLE";
  case ESMC_POLEKIND_BIPOLE: return "ESMC_POLEKIND_BIPOLE";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_pll_knd_sng() */

const char * /* O [sng] String version of ESMC_UnmappedAction_Flag enum */
nco_esmf_unm_act_sng /* [fnc] Convert ESMF unmapped action type enum to string */
(const int nco_esmf_unm_act) /* I [enm] ESMF unmapped action type enum ESMC_UnmappedAction_Flag */
{
  /* Purpose: Convert ESMF unmapped action enum ESMC_UnmappedAction_Flag to string */
  switch(nco_esmf_unm_act){
  case ESMC_UNMAPPEDACTION_ERROR: return "ESMC_UNMAPPEDACTION_ERROR";
  case ESMC_UNMAPPEDACTION_IGNORE: return "ESMC_UNMAPPEDACTION_IGNORE";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_unm_act_sng() */

const char * /* O [sng] String version of ESMC_Region_Flag enum */
nco_esmf_rgn_flg_sng /* [fnc] Convert ESMF region flag enum to string */
(const int nco_esmf_rgn_flg) /* I [enm] ESMF region flag enum ESMC_Region_Flag */
{
  /* Purpose: Convert ESMF region flag enum ESMC_Region_Flag to string */
  switch(nco_esmf_rgn_flg){
  case ESMC_REGION_TOTAL: return "ESMC_REGION_TOTAL";
  case ESMC_REGION_SELECT: return "ESMC_REGION_SELECT";
  case ESMC_REGION_EMPTY: return "ESMC_REGION_EMPTY";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_rgn_flg_sng() */

int /* O [enm] Return code */
nco_rgr_esmf /* [fnc] Regrid using ESMF library */
(rgr_sct * const rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields using ESMF library (actually ESMC interface to ESMF library)
     ESMC is C-interface to ESMF documented at
     http://www.earthsystemmodeling.org/esmf_releases/last_built/ESMC_crefdoc/ESMC_crefdoc.html
     ESMF Developer's Guide
     http://www.earthsystemmodeling.org/documents/dev_guide
     ESMF_RegridWeightGen
     http://www.earthsystemcog.org/projects/regridweightgen
     http://www.earthsystemmodeling.org/python_releases/last_esmpy/esmpy_doc/html/index.html

     ESMF C-interface examples:
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridUTest.C
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridCsrvUTest.C

     Sample call, T42->T42:
     ncks -O --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     ncks -O --rgr_var=LANDFRAC --rgr_grd_src=${DATA}/grids/T42.nc --rgr_grd_dst=${DATA}/grids/T62.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     ncks -O --rgr lat_nbr=180 --rgr_grd_src=${DATA}/grids/T62.nc --rgr_grd_dst=${DATA}/grids/T42.nc ${DATA}/dstmch90/dstmch90_clm.nc ~/foo.nc */
  
  const char fnc_nm[]="nco_rgr_esmf()"; /* [sng] Function name */
  const char fl_nm_esmf_log[]="nco_rgr_log_foo.txt"; /* [sng] Log file for ESMF routines */
  
  enum ESMC_StaggerLoc stg_lcn_src=ESMC_STAGGERLOC_CENTER; /* [enm] Stagger location of source grid */
  enum ESMC_StaggerLoc stg_lcn_dst=ESMC_STAGGERLOC_CENTER; /* [enm] Stagger location of destination grid */

  int flg_openMPEnabledFlag; /* [flg] ESMF library was compiled with OpenMP enabled */
  int flg_pthreadsEnabledFlag; /* [flg] ESMF library was compiled with Pthreads enabled */
  int localPet; /* [id] ID of PET that issued call */
  int peCount; /* [nbr] Number of PEs referenced by ESMF_VM */
  int petCount; /* [nbr] Number of PETs referenced by ESMF_VM */
  int rcd_esmf; /* [enm] Return codes from ESMF functions */
  
  int dmn_nbr_grd=2;
  int rcd=ESMF_SUCCESS;
  
  int *dmn_id;
  
  int dmn_nbr;
  int idx;
  int var_in_id;
  
  long *dmn_cnt;
  long *dmn_srt;
  
  nc_type var_typ_in;
  nc_type crd_typ_out=NC_DOUBLE;
  nc_type var_typ_out=NC_DOUBLE;
  
  double *lon_in;
  double *lat_in;
  void *void_ptr_var;
  
  int in_id; /* I [id] Input netCDF file ID */
  int out_id; /* I [id] Output netCDF file ID */

  /* SCRIP rank-2 grids are almost always lat,lon (C) and lon,lat (Fortran)
     Indexing is confusing because ESMF always uses Fortran-ordering convention with mixture of 0- and 1-based indices
     netCDF always uses C-ordering convention with 0-based indices
     No data transposition is necessary because variable RAM is always in C-order (same as netCDF)
     However, Fortran and C (i.e., ESMF and netCDF) access that same RAM using different conventions
     2-D data are always C[lat,lon] or F(lon,lat) in practice
     2-D stored in netCDF or RAM as C[lon,lat] and F(lat,lon) are possible, though not seen in practice
     NCO regridding below assumes data are in C[lat,lon] order
     NCO hardcoded subscript conventions follow this pattern:
     crd_idx_[axs]_[ibs]_[dmn]_[grd] where
     axs = access convention = C or Fortran = _c_ or _f_ 
     ibs = index base = 0 or 1 = _0bs_ or _1bs_ for zero-based or one-based indices
     NB: axs is independent from ibs! 
     dmn = dimension = lon or lat
     grd = grid = source or destination */
  const int crd_idx_c_0bs_lat_dst=0; /* [dgr] 0-based index of latitude  in C       representation of rank-2 destination grids */
  const int crd_idx_c_0bs_lon_dst=1; /* [dgr] 0-based index of longitude in C       representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lat_dst=1; /* [dgr] 0-based index of latitude  in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lat_src=1; /* [dgr] 0-based index of latitude  in Fortran representation of rank-2 source grids */
  const int crd_idx_f_0bs_lon_dst=0; /* [dgr] 0-based index of longitude in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lon_src=0; /* [dgr] 0-based index of longitude in Fortran representation of rank-2 source grids */
  const int crd_idx_f_1bs_lat_dst=2; /* [dgr] 1-based index of latitude  in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_1bs_lat_src=2; /* [dgr] 1-based index of latitude  in Fortran representation of rank-2 source grids */
  const int crd_idx_f_1bs_lon_dst=1; /* [dgr] 1-based index of longitude in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_1bs_lon_src=1; /* [dgr] 1-based index of longitude in Fortran representation of rank-2 source grids */

  /* Initialize */
  in_id=rgr->in_id;
  out_id=rgr->out_id;
  
  /* Obtain input longitude type and length */
  char lon_nm_in[]="lon";
  (void)nco_inq_varid(in_id,lon_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr->fl_in);
  } /* !var_typ_in */

  /* Allocate space to hold dimension metadata */
  dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
  dmn_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  /* Get input dimension sizes */
  long dmn_sz=1L; /* [nbr] Number of elements in dimension (will be self-multiplied) */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    dmn_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  
  int *max_idx; /* [nbr] Zero-based array containing dimension sizes in order? */
  max_idx=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  max_idx[crd_idx_f_0bs_lon_src]=dmn_sz; /* [nbr] Number of elements in dimensions */
  
  /* Allocate space for and obtain longitude */
  lon_in=(double *)nco_malloc(dmn_sz*nco_typ_lng(crd_typ_out));
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,lon_in,crd_typ_out);

  /* Obtain input latitude type and length */
  char lat_nm_in[]="lat";
  (void)nco_inq_varid(in_id,lat_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr->fl_in);
  } /* !var_typ_in */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  dmn_sz=1L;
  /* Get dimension sizes from input file */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    dmn_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  max_idx[crd_idx_f_0bs_lat_src]=dmn_sz; /* [nbr] Number of elements in dimensions */

  /* Allocate space for and obtain latitude */
  lat_in=(double *)nco_malloc(dmn_sz*nco_typ_lng(crd_typ_out));
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,lat_in,crd_typ_out);

  /* Initialize before any other ESMC API calls!
     ESMC_ArgLast is ALWAYS at the end to indicate the end of opt args */
  ESMC_Initialize(&rcd_esmf,
		  ESMC_InitArgDefaultCalKind(ESMC_CALKIND_GREGORIAN),
		  ESMC_InitArgLogFilename(fl_nm_esmf_log),
		  ESMC_InitArgLogKindFlag(ESMC_LOGKIND_MULTI),
		  ESMC_ArgLast);
  enum ESMC_LogMsgType_Flag log_msg_typ=ESMC_LOGMSG_TRACE;
  if(rcd_esmf != ESMF_SUCCESS){
    ESMC_LogWrite("ESMC_Initialize() failed",log_msg_typ);
    /* 20150621: 
       Prevent clang sometimes uninitialized warnings by using abort() before goto
       All of the following "abort()" statements could be replace "goto rgr_cln:" statements
       gcc is fine with this, but it triggers clang "sometimes uninitialized" warnings
       g++ (without -fpermissive) emits "jump to label" "crosses initialization" warnings 
       when pointers are declared between the goto statement and the label statement 
       Hence we abandon the false premise of recovery after ESMC errors, and just abort() */
    abort();
  } /* endif */
  
  /* Set log to flush after every message */
  rcd_esmf=ESMC_LogSet(ESMF_TRUE);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* Obtain VM */
  ESMC_VM vm;
  vm=ESMC_VMGetGlobal(&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Set-up local PET info */
  /* Source: ${DATA}/esmf/src/Infrastructure/VM/interface/ESMC_VM.C */
  rcd_esmf=ESMC_VMGet(vm,&localPet,&petCount,&peCount,(MPI_Comm *)NULL,&flg_pthreadsEnabledFlag,&flg_openMPEnabledFlag);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Create destination grid from SCRIP file set as rgr_grd_dst */
  int flg_isSphere=1; /* [flg] Set to 1 for a spherical grid, or 0 for regional. Defaults to 1. */
  int flg_addCornerStagger=0; /* [flg] Add corner stagger to grid. Defaults to 0. */
  int flg_addUserArea=0; /* [flg] Read cell area from Grid file (instead of calculate it). Defaults to 0. */
  int flg_addMask=0; /* [flg] Generate mask using missing value attribute in var_nm (iff GRIDSPEC) */
  /* 20150424: ESMF library bug at ESMCI_Grid.C line 365 means var_nm must non-NULL so set to blank name */
  char var_nm[]=""; /* [sng] Iff addMask == 1 use this variable's missing value attribute */
  char **crd_nm=NULL; /* [sng] Iff GRIDSPEC use these lon/lat variable coordinates */
  enum ESMC_FileFormat_Flag grd_fl_typ=ESMC_FILEFORMAT_SCRIP;
  ESMC_Grid grd_dst;
  grd_dst=ESMC_GridCreateFromFile(rgr->fl_grd_dst,grd_fl_typ,&flg_isSphere,&flg_addCornerStagger,&flg_addUserArea,&flg_addMask,var_nm,crd_nm,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int *msk_dst; /* [enm] Destination grid mask */
  enum ESMC_GridItem_Flag grd_itm=ESMC_GRIDITEM_MASK;
  msk_dst=(int *)ESMC_GridGetItem(grd_dst,grd_itm,stg_lcn_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int *bnd_lwr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_dst; /* [dgr] Destination grid longitude */
  double *lat_dst; /* [dgr] Destination grid latitude */
  /* 20150427: Written documentation is somewhat inadequate or misleading to normal C-programmers
     Some ESMC_Grid***() functions, like this one, return allocated void pointers that must be cast to desired numeric type
     Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  lon_dst=(double *)ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lon_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  lat_dst=(double *)ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lat_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  double *lon_out; /* [dgr] Longitude dimension for output file */
  double *lat_out; /* [dgr] Latitude  dimension for output file */
  lon_out=(double *)nco_malloc(bnd_upr_dst[crd_idx_f_0bs_lon_dst]*sizeof(double));
  lat_out=(double *)nco_malloc(bnd_upr_dst[crd_idx_f_0bs_lat_dst]*sizeof(double));
  for(idx=0;idx<bnd_upr_dst[crd_idx_f_0bs_lon_dst];idx++) lon_out[idx]=lon_dst[idx];
  for(idx=0;idx<bnd_upr_dst[crd_idx_f_0bs_lat_dst];idx++) lat_out[idx]=lat_dst[idx];
  const long var_sz_dst=bnd_upr_dst[crd_idx_f_0bs_lon_dst]*bnd_upr_dst[crd_idx_f_0bs_lat_dst]; /* [nbr] Number of elements in variable */
  for(idx=0;idx<var_sz_dst;idx++) msk_dst[idx]=0;

  /* Create source grid with same sizes as those in input data file */
  ESMC_InterfaceInt max_idx_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Util/interface/ESMC_Interface.C
     NB: ESMF is fragile in that dynamic memory provided as input to grids cannot be immediately freed
     In other words, ESMF copies the values of pointers but not the contents of pointers to provided arrays
     To be concrete, the max_idx array provided below cannot be freed until after the ESMC_Finalize() is called */
  max_idx_src=ESMC_InterfaceIntCreate(max_idx,dmn_nbr_grd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  enum ESMC_CoordSys_Flag crd_sys=ESMC_COORDSYS_SPH_DEG; /* NB: dyw sez ESMF supports ESMC_COORDSYS_SPH_DEG only */
  enum ESMC_TypeKind_Flag typ_knd=ESMC_TYPEKIND_R8; /* NB: NCO default is to use double precision for coordinates */
  ESMC_Grid grd_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  grd_src=ESMC_GridCreateNoPeriDim(max_idx_src,&crd_sys,&typ_knd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* fxm: why destroy this now? */
  ESMC_InterfaceIntDestroy(&max_idx_src);
  /* Define stagger locations on source grid. Necessary for remapping later? */
  rcd_esmf=ESMC_GridAddCoord(grd_src,stg_lcn_src);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  
  /* Allocate */
  int *bnd_lwr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_src; /* [dgr] Source grid longitude */
  double *lat_src; /* [dgr] Source grid latitude  */
  lon_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lon_src,stg_lcn_src,bnd_lwr_src,bnd_upr_src,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  lat_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lat_src,stg_lcn_src,NULL,NULL,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"ESMC_VMGet(): localPet = %d, petCount = %d, peCount = %d, pthreadsEnabledFlag = %s, openMPEnabledFlag = %s\n",localPet,petCount,peCount,flg_pthreadsEnabledFlag ? "Enabled" : "Disabled",flg_openMPEnabledFlag ? "Enabled" : "Disabled");
    (void)fprintf(stderr,"ESMC_GridCreateFromFile(): filename = %s, fileTypeFlag = %s, isSphere = %s, addCornerStagger = %s, addUserArea = %s, addMask = %s, var_nm = %s, crd_nm = %s\n",rgr->fl_grd_dst,nco_esmf_fl_fmt_sng(grd_fl_typ),flg_isSphere ? "Enabled" : "Disabled",flg_addCornerStagger ? "Enabled" : "Disabled",flg_addUserArea ? "Enabled" : "Disabled",flg_addMask ? "Enabled" : "Disabled",var_nm,crd_nm ? "non-NULL" : "NULL");
    (void)fprintf(stderr,"ESMC_GridGetCoord(): bnd_lwr_dst[0..1] = %d, %d; bnd_upr_dst[0..1] = %d, %d;\n",bnd_lwr_dst[0],bnd_lwr_dst[1],bnd_upr_dst[0],bnd_upr_dst[1]);
    (void)fprintf(stderr,"ESMC_InterfaceIntCreate(): No printable information\n");
    (void)fprintf(stderr,"ESMC_GridCreateNoPeriDim(): crd_sys = %s, typ_knd = %s\n",nco_esmf_crd_sys_sng(crd_sys),nco_esmf_typ_knd_sng(typ_knd));
    (void)fprintf(stderr,"ESMC_GridAddCoord(): stg_lcn_src = %s,  stg_lcn_dst = %s\n",nco_esmf_stg_lcn_sng(stg_lcn_src),nco_esmf_stg_lcn_sng(stg_lcn_dst));
    (void)fprintf(stderr,"ESMC_GridGetCoord(): bnd_lwr_src[0..1] = %d, %d; bnd_upr_src[0..1] = %d, %d;\n",bnd_lwr_src[0],bnd_lwr_src[1],bnd_upr_src[0],bnd_upr_src[1]);
    //    (void)fprintf(stderr,"ESMC_InterfaceIntCreate(): max_idx_src[0..1] = %d, %d\n",max_idx_src[0],max_idx_src[1]);
  } /* endif dbg */

  /* Type-conversion and cell-center coordinates */
  for(idx=0;idx<bnd_upr_src[crd_idx_f_0bs_lat_src];idx++) lat_src[idx]=lat_in[idx];
  for(idx=0;idx<bnd_upr_src[crd_idx_f_0bs_lon_src];idx++) lon_src[idx]=lon_in[idx];
  idx=0;
  for(int idx_lat=0;idx_lat<bnd_upr_src[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=0;idx_lon<bnd_upr_src[crd_idx_f_0bs_lon_src];idx_lon++){
      lon_src[idx]=lon_in[idx_lon];
      lat_src[idx]=lat_in[idx_lat];
      idx++;
    } /* endfor */
  } /* endfor */
  
  /* Create source field from source grid */
  ESMC_Field fld_src;
  ESMC_InterfaceInt *gridToFieldMap=NULL; /* [idx] Map of all grid dimensions to field dimensions */
  ESMC_InterfaceInt *ungriddedLBound=NULL; /* [idx] Lower bounds of ungridded dimensions */
  ESMC_InterfaceInt *ungriddedUBound=NULL; /* [idx] Upper bounds of ungridded dimensions */
  fld_src=ESMC_FieldCreateGridTypeKind(grd_src,typ_knd,stg_lcn_src,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_src",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* Create destination field from destination grid */
  ESMC_Field fld_dst;
  fld_dst=ESMC_FieldCreateGridTypeKind(grd_dst,typ_knd,stg_lcn_dst,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_dst",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Get field pointers */
  double *fld_src_ptr;
  int localDe=0; /* [idx] Local DE for which information is requested [0..localDeCount-1] */
  fld_src_ptr=(double *)ESMC_FieldGetPtr(fld_src,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  double *fld_dst_ptr;
  fld_dst_ptr=(double *)ESMC_FieldGetPtr(fld_dst,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Get variables from input file */
  (void)nco_inq_varid(in_id,rgr->var_nm,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  long var_sz_src=1L; /* [nbr] Number of elements in variable (will be self-multiplied) */
  /* Get input dimension sizes */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    var_sz_src*=dmn_cnt[idx];
  } /* end loop over dim */

  /* Allocate space for and obtain input latitude */
  void_ptr_var=(void *)nco_malloc_dbg(var_sz_src*nco_typ_lng(var_typ_in),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr_var,var_typ_in);
  float *var_fld=(float *)void_ptr_var;

  /* Type-conversion and ensure every cell has data */
  idx=0;
  for(int idx_lat=bnd_lwr_src[crd_idx_f_0bs_lat_src];idx_lat<=bnd_upr_src[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=bnd_lwr_src[crd_idx_f_0bs_lon_src];idx_lon<=bnd_upr_src[crd_idx_f_0bs_lon_src];idx_lon++){
      fld_src_ptr[idx]=(double)var_fld[idx];
      idx++;
    } /* idx_lon */
  } /* idx_lat */

  /* Initialize dst data ptr */
  idx=0;
  for(int idx_lat=bnd_lwr_dst[crd_idx_f_0bs_lat_src];idx_lat<=bnd_upr_dst[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=bnd_lwr_dst[crd_idx_f_0bs_lon_src];idx_lon<=bnd_upr_dst[crd_idx_f_0bs_lon_src];idx_lon++){
      fld_dst_ptr[idx]=0.0;
      idx++;
    } /* idx_lon */
  } /* idx_lat */

  ESMC_LogWrite("nco_rgr_esmf() invoking ESMC to start regridstore actions",log_msg_typ);
  /* int *msk_val=(int *)nco_malloc(sizeof(int));
     msk_val[0]=1;
     ESMC_InterfaceInt i_msk_val=ESMC_InterfaceIntCreate(msk_val,1,&rcd_esmf);
     rcd_esmf = ESMC_FieldRegridStore(fld_src,fld_dst,&i_msk_val,&i_msk_val,&rte_hnd,NULL,NULL,NULL,&unmap_act,NULL,NULL);
     rcd_esmf=ESMC_FieldRegridStore(fld_src,fld_dst,NULL,NULL,&rte_hnd,NULL,NULL,NULL,&unmap_act,NULL,NULL); */

  ESMC_Field *cll_frc_dst=NULL; /* [frc] Fraction of each cell participating in regridding, destination grid */
  ESMC_Field *cll_frc_src=NULL; /* [frc] Fraction of each cell participating in regridding, source grid */
  ESMC_InterfaceInt *msk_src_rgr=NULL; /* [idx] Points to mask while regridding, source grid */
  /* fxm: unsure whether/why need both msk_dst (above) and msk_dst_rgr (below) */
  ESMC_InterfaceInt *msk_dst_rgr=NULL; /* [idx] Points to mask while regridding, destination grid */
  ESMC_RouteHandle rte_hnd;
  enum ESMC_RegridMethod_Flag rgr_mth=ESMC_REGRIDMETHOD_BILINEAR; /* [flg] Regrid method (default bilinear) */
  enum ESMC_PoleMethod_Flag pll_mth=ESMC_POLEMETHOD_ALLAVG; /* [flg] Regrid method (default ESMC_POLEMETHOD_ALLAVG) */
  enum ESMC_UnmappedAction_Flag unm_act=ESMC_UNMAPPEDACTION_ERROR; /* [flg] Unmapped action (default ESMC_UNMAPPEDACTION_ERROR) */
  int pll_nbr=int_CEWI; /* [nbr] Number of points to average (iff ESMC_POLEMETHOD_NPNTAVG) */
  /* Source: ${DATA}/esmf/src/Infrastructure/Field/interface/ESMC_Field.C */
  rcd_esmf=ESMC_FieldRegridStore(fld_src,fld_dst,msk_src_rgr,msk_dst_rgr,&rte_hnd,&rgr_mth,&pll_mth,&pll_nbr,&unm_act,cll_frc_src,cll_frc_dst);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  enum ESMC_Region_Flag rgn_flg=ESMC_REGION_TOTAL; /* [flg] Whether/how to zero input fields before regridding (default ESMC_REGION_TOTAL) */
  rcd_esmf=ESMC_FieldRegrid(fld_src,fld_dst,rte_hnd,&rgn_flg);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Call once on each PET */
  rcd_esmf=ESMC_Finalize();
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Write regridded data to netCDF file */
  int var_out_id; /* [id] Variable ID */
  int lon_out_id; /* [id] Variable ID for longitude */
  int lat_out_id; /* [id] Variable ID for latitude  */
  int dmn_id_lat; /* [id] Dimension ID */
  int dmn_id_lon; /* [id] Dimension ID */
  char *lat_nm_out=lat_nm_in;
  char *lon_nm_out=lon_nm_in;
  (void)nco_def_dim(out_id,lat_nm_out,bnd_upr_dst[crd_idx_f_0bs_lat_dst],&dmn_id_lat);
  (void)nco_def_dim(out_id,lon_nm_out,bnd_upr_dst[crd_idx_f_0bs_lon_dst],&dmn_id_lon);
  int dmn_ids_out[2]; /* [id] Dimension IDs array for output variable */
  long dmn_srt_out[2];
  long dmn_cnt_out[2];
  dmn_ids_out[crd_idx_c_0bs_lat_dst]=dmn_id_lat;
  dmn_ids_out[crd_idx_c_0bs_lon_dst]=dmn_id_lon;
  (void)nco_def_var(out_id,lon_nm_out,crd_typ_out,(int)1,&dmn_id_lon,&lon_out_id);
  (void)nco_def_var(out_id,lat_nm_out,crd_typ_out,(int)1,&dmn_id_lat,&lat_out_id);
  (void)nco_def_var(out_id,rgr->var_nm,var_typ_out,(int)2,dmn_ids_out,&var_out_id);
  (void)nco_enddef(out_id);
  dmn_srt_out[crd_idx_c_0bs_lat_dst]=0L;
  dmn_srt_out[crd_idx_c_0bs_lon_dst]=0L;
  dmn_cnt_out[crd_idx_c_0bs_lat_dst]=bnd_upr_dst[crd_idx_f_0bs_lat_dst];
  dmn_cnt_out[crd_idx_c_0bs_lon_dst]=bnd_upr_dst[crd_idx_f_0bs_lon_dst];
  (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_out,lat_out,crd_typ_out);
  (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_out,lon_out,crd_typ_out);
  (void)nco_put_vara(out_id,var_out_id,dmn_srt_out,dmn_cnt_out,fld_dst_ptr,var_typ_out);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(rgr->fl_out,rgr->fl_out_tmp,out_id);

  goto rgr_cln; /* Prevent 'rgr_cln' defined but not used warning */

 rgr_cln:
  if(rcd_esmf != ESMF_SUCCESS){
    /* ESMF return codes are enumerated in ESMC_ReturnCodes.h and ESMC_LogErr.h
       However, there seems to be no translator function for these codes */
    (void)fprintf(stderr,"%s: ERROR %s received ESMF return code %d\nSee ESMC_ReturnCodes.h or ESMC_LogErr.h for more information, e.g.,\n/bin/more /usr/local/include/ESMC_ReturnCodes.h | grep %d\n",nco_prg_nm_get(),fnc_nm,rcd_esmf,rcd_esmf);
  } /* rcd_esmf */
  if(max_idx) max_idx=(int *)nco_free(max_idx);
  if(bnd_lwr_src) bnd_lwr_src=(int *)nco_free(bnd_lwr_src);
  if(bnd_upr_src) bnd_upr_src=(int *)nco_free(bnd_upr_src);
  if(bnd_lwr_dst) bnd_lwr_dst=(int *)nco_free(bnd_lwr_dst);
  if(bnd_upr_dst) bnd_upr_dst=(int *)nco_free(bnd_upr_dst);
  if(void_ptr_var) void_ptr_var=(void *)nco_free(void_ptr_var);
  if(lat_out) lat_out=(double *)nco_free(lat_out);
  if(lon_out) lon_out=(double *)nco_free(lon_out);
  if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
  if(dmn_id) dmn_id=(int *)nco_free(dmn_id);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);

  rcd_esmf=ESMC_FieldRegridRelease(&rte_hnd);
  rcd_esmf=ESMC_FieldDestroy(&fld_src);
  rcd_esmf=ESMC_FieldDestroy(&fld_dst);
  rcd_esmf=ESMC_GridDestroy(&grd_src);
  rcd_esmf=ESMC_GridDestroy(&grd_dst);
  ESMC_Finalize();

  return rcd;
} /* !nco_rgr_esmf() */

int /* O [enm] Return code */
nco_rgr_esmf2 /* [fnc] Regrid using ESMF library */
(rgr_sct * const rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields using ESMF library (actually ESMC interface to ESMF library)
     ESMC is C-interface to ESMF documented at
     http://www.earthsystemmodeling.org/esmf_releases/last_built/ESMC_crefdoc/ESMC_crefdoc.html
     ESMF Developer's Guide
     http://www.earthsystemmodeling.org/documents/dev_guide
     ESMF_RegridWeightGen
     http://www.earthsystemcog.org/projects/regridweightgen
     http://www.earthsystemmodeling.org/python_releases/last_esmpy/esmpy_doc/html/index.html

     ESMF C-interface examples:
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridUTest.C
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridCsrvUTest.C

     Sample call, T42->T42:
     ncks -O --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc */

  const char fnc_nm[]="nco_rgr_esmf2()"; /* [sng] Function name */
  const char fl_nm_esmf_log[]="nco_rgr_log_foo.txt"; /* [sng] Log file for ESMF routines */
  
  /* SCRIP rank-2 grids are almost always lat,lon (C) and lon,lat (Fortran)
     Indexing is confusing because ESMF always uses Fortran-ordering convention with mixture of 0- and 1-based indices
     netCDF always uses C-ordering convention with 0-based indices
     No data transposition is necessary because variable RAM is always in C-order (same as netCDF)
     However, Fortran and C (i.e., ESMF and netCDF) access that same RAM using different conventions
     2-D data are always C[lat,lon] or F(lon,lat) in practice
     2-D stored in netCDF or RAM as C[lon,lat] and F(lat,lon) are possible, though not seen in practice
     NCO regridding below assumes data are in C[lat,lon] order
     NCO hardcoded subscript conventions follow this pattern:
     crd_idx_[axs]_[ibs]_[dmn]_[grd] where
     axs = access convention = C or Fortran = _c_ or _f_ 
     ibs = index base = 0 or 1 = _0bs_ or _1bs_ for zero-based or one-based indices
     NB: axs is independent from ibs! 
     dmn = dimension = lon or lat
     grd = grid = source or destination */
  const int crd_idx_c_0bs_lat_dst=0; /* [dgr] 0-based index of latitude  in C       representation of rank-2 destination grids */
  const int crd_idx_c_0bs_lon_dst=1; /* [dgr] 0-based index of longitude in C       representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lat_dst=1; /* [dgr] 0-based index of latitude  in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lat_src=1; /* [dgr] 0-based index of latitude  in Fortran representation of rank-2 source grids */
  const int crd_idx_f_0bs_lon_dst=0; /* [dgr] 0-based index of longitude in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_0bs_lon_src=0; /* [dgr] 0-based index of longitude in Fortran representation of rank-2 source grids */
  const int crd_idx_f_1bs_lat_dst=2; /* [dgr] 1-based index of latitude  in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_1bs_lat_src=2; /* [dgr] 1-based index of latitude  in Fortran representation of rank-2 source grids */
  const int crd_idx_f_1bs_lon_dst=1; /* [dgr] 1-based index of longitude in Fortran representation of rank-2 destination grids */
  const int crd_idx_f_1bs_lon_src=1; /* [dgr] 1-based index of longitude in Fortran representation of rank-2 source grids */

  double *lon_in;
  double *lat_in;

  enum ESMC_StaggerLoc stg_lcn_src=ESMC_STAGGERLOC_CENTER; /* [enm] Stagger location of source grid */
  enum ESMC_StaggerLoc stg_lcn_dst=ESMC_STAGGERLOC_CENTER; /* [enm] Stagger location of destination grid */

  int *dmn_id;
  int dmn_nbr;
  int dmn_nbr_grd=2;
  int flg_openMPEnabledFlag; /* [flg] ESMF library was compiled with OpenMP enabled */
  int flg_pthreadsEnabledFlag; /* [flg] ESMF library was compiled with Pthreads enabled */
  int idx;
  int in_id; /* I [id] Input netCDF file ID */
  int localPet; /* [id] ID of PET that issued call */
  int out_id; /* I [id] Output netCDF file ID */
  int peCount; /* [nbr] Number of PEs referenced by ESMF_VM */
  int petCount; /* [nbr] Number of PETs referenced by ESMF_VM */
  int rcd=ESMF_SUCCESS;
  int rcd_esmf; /* [enm] Return codes from ESMF functions */
  int var_in_id;
  
  long *dmn_cnt;
  long *dmn_srt;
  
  nc_type var_typ_in;
  nc_type crd_typ_out=NC_DOUBLE;
  nc_type var_typ_out=NC_DOUBLE;
  
  void *void_ptr_var;
  
  /* Initialize */
  in_id=rgr->in_id;
  out_id=rgr->out_id;
  
  /* Obtain input longitude type and length */
  char lon_nm_in[]="lon";
  (void)nco_inq_varid(in_id,lon_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr->fl_in);
  } /* !var_typ_in */

  /* Allocate space to hold dimension metadata */
  dmn_cnt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  dmn_id=(int *)nco_malloc(dmn_nbr*sizeof(int));
  dmn_srt=(long *)nco_malloc(dmn_nbr*sizeof(long));
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  /* Get input dimension sizes */
  long dmn_sz=1L; /* [nbr] Number of elements in dimension (will be self-multiplied) */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    dmn_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  
  int *max_idx; /* [nbr] Zero-based array containing dimension sizes in order? */
  max_idx=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  max_idx[crd_idx_f_0bs_lon_src]=dmn_sz; /* [nbr] Number of elements in dimensions */
  
  /* Allocate space for and obtain longitude */
  lon_in=(double *)nco_malloc(dmn_sz*nco_typ_lng(crd_typ_out));
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,lon_in,crd_typ_out);

  /* Obtain input latitude type and length */
  char lat_nm_in[]="lat";
  (void)nco_inq_varid(in_id,lat_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr->fl_in);
  } /* !var_typ_in */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  dmn_sz=1L;
  /* Get dimension sizes from input file */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    dmn_sz*=dmn_cnt[idx];
  } /* end loop over dim */
  max_idx[crd_idx_f_0bs_lat_src]=dmn_sz; /* [nbr] Number of elements in dimensions */

  /* Allocate space for and obtain latitude */
  lat_in=(double *)nco_malloc(dmn_sz*nco_typ_lng(crd_typ_out));
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,lat_in,crd_typ_out);

  /* Initialize before any other ESMC API calls!
     ESMC_ArgLast is ALWAYS at the end to indicate the end of opt args */
  ESMC_Initialize(&rcd_esmf,
		  ESMC_InitArgDefaultCalKind(ESMC_CALKIND_GREGORIAN),
		  ESMC_InitArgLogFilename(fl_nm_esmf_log),
		  ESMC_InitArgLogKindFlag(ESMC_LOGKIND_MULTI),
		  ESMC_ArgLast);
  enum ESMC_LogMsgType_Flag log_msg_typ=ESMC_LOGMSG_TRACE;
  if(rcd_esmf != ESMF_SUCCESS){
    ESMC_LogWrite("ESMC_Initialize() failed",log_msg_typ);
    /* 20150621: 
       Prevent clang sometimes uninitialized warnings by using abort() before goto
       All of the following "abort()" statements could be replace "goto rgr_cln:" statements
       gcc is fine with this, but it triggers clang "sometimes uninitialized" warnings
       g++ (without -fpermissive) emits "jump to label" "crosses initialization" warnings 
       when pointers are declared between the goto statement and the label statement 
       Hence we abandon the false premise of recovery after ESMC errors, and just abort() */
    abort();
  } /* endif */
  
  /* Set log to flush after every message */
  rcd_esmf=ESMC_LogSet(ESMF_TRUE);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* Obtain VM */
  ESMC_VM vm;
  vm=ESMC_VMGetGlobal(&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Set-up local PET info */
  /* Source: ${DATA}/esmf/src/Infrastructure/VM/interface/ESMC_VM.C */
  rcd_esmf=ESMC_VMGet(vm,&localPet,&petCount,&peCount,(MPI_Comm *)NULL,&flg_pthreadsEnabledFlag,&flg_openMPEnabledFlag);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* 20150718: Create destination grid from user specifications */
  /* First create ESMC_InterfaceInt that describes grid size 
     Then use that ESMC_InterfaceInt to create ESMC_Grid
     ESMC_InterfaceInt stores grid indexing information for all dimensions */
  int *max_idx_dst; /* [nbr] Zero-based array containing dimension sizes in order? */
  int dmn_nbr_grd_dst=2;
  max_idx_dst=(int *)nco_malloc(dmn_nbr_grd_dst*sizeof(int));
  max_idx_dst[crd_idx_f_0bs_lon_src]=rgr->lat_nbr*rgr->lon_nbr; /* [nbr] Number of elements in dimensions */

  ESMC_InterfaceInt grd_ntf_dst;
  /* Source: ${DATA}/esmf/src/Infrastructure/Util/interface/ESMC_Interface.C
     NB: ESMF is fragile in that dynamic memory provided as input to grids cannot be immediately freed
     In other words, ESMF copies the values of pointers but not the contents of pointers to provided arrays
     To be concrete, the max_idx_dst array provided below cannot be freed until after the ESMC_Finalize() is called */
  grd_ntf_dst=ESMC_InterfaceIntCreate(max_idx_dst,dmn_nbr_grd_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int flg_isSphere=1; /* [flg] Set to 1 for spherical grid, or 0 for regional. Defaults to 1 (True). */
  int flg_addCornerStagger=0; /* [flg] Add corner stagger to grid. Defaults to 0 (False). */
  int flg_addUserArea=0; /* [flg] Read cell area from Grid file (instead of calculate it). Defaults to 0 (False). */
  int flg_addMask=0; /* [flg] Generate mask using missing value attribute in var_nm (iff GRIDSPEC) */
  /* 20150424: ESMF library bug at ESMCI_Grid.C line 365 means var_nm must non-NULL so set to blank name */
  char var_nm[]=""; /* [sng] Iff addMask == 1 use this variable's missing value attribute */
  char **crd_nm=NULL; /* [sng] Iff GRIDSPEC use these lon/lat variable coordinates */
  enum ESMC_FileFormat_Flag grd_fl_typ=ESMC_FILEFORMAT_SCRIP;
  enum ESMC_PoleKind_Flag poleKind[2]={ESMC_POLEKIND_MONOPOLE,ESMC_POLEKIND_MONOPOLE}; /* [enm] Pole type */
  enum ESMC_CoordSys_Flag crd_sys=ESMC_COORDSYS_SPH_DEG; /* NB: ESMF supports ESMC_COORDSYS_SPH_DEG only? */
  enum ESMC_TypeKind_Flag typ_knd=ESMC_TYPEKIND_R8; /* NB: NCO default is to use double precision for coordinates */
  ESMC_Grid grd_dst;
  // int periodicDim=crd_idx_f_1bs_lon_dst; /* [idx] Periodic dimension */
  // int poleDim=crd_idx_f_1bs_lat_dst; /* [idx] Pole dimension */
  grd_dst=ESMC_GridCreate1PeriDim(grd_ntf_dst,&crd_sys,&typ_knd,poleKind,&rcd_esmf); // API: /usr/local/include/ESMC_Grid.h
  //  grd_dst=ESMC_GridCreate1PeriDim(grd_ntf_dst,&periodicDim,&poleDim,&crd_sys,&typ_knd,&poleKind,&rcd_esmf); // API: http://www.earthsystemmodeling.org/esmf_releases/last_built/ESMC_crefdoc/node5.html#SECTION05055200000000000000
  //  grd_dst=ESMC_GridCreateFromFile(rgr->fl_grd_dst,grd_fl_typ,&flg_isSphere,&flg_addCornerStagger,&flg_addUserArea,&flg_addMask,var_nm,crd_nm,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int *msk_dst; /* [enm] Destination grid mask */
  enum ESMC_GridItem_Flag grd_itm=ESMC_GRIDITEM_MASK;
  msk_dst=(int *)ESMC_GridGetItem(grd_dst,grd_itm,stg_lcn_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int *bnd_lwr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_dst; /* [dgr] Destination grid longitude */
  double *lat_dst; /* [dgr] Destination grid latitude */
  /* 20150427: Written documentation is somewhat inadequate or misleading to normal C-programmers
     Some ESMC_Grid***() functions, like this one, return allocated void pointers that must be cast to desired numeric type
     Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  lon_dst=(double *)ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lon_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  lat_dst=(double *)ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lat_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  double *lon_out; /* [dgr] Longitude dimension for output file */
  double *lat_out; /* [dgr] Latitude  dimension for output file */
  lon_out=(double *)nco_malloc(bnd_upr_dst[crd_idx_f_0bs_lon_dst]*sizeof(double));
  lat_out=(double *)nco_malloc(bnd_upr_dst[crd_idx_f_0bs_lat_dst]*sizeof(double));
  for(idx=0;idx<bnd_upr_dst[crd_idx_f_0bs_lon_dst];idx++) lon_out[idx]=lon_dst[idx];
  for(idx=0;idx<bnd_upr_dst[crd_idx_f_0bs_lat_dst];idx++) lat_out[idx]=lat_dst[idx];
  const long var_sz_dst=bnd_upr_dst[crd_idx_f_0bs_lon_dst]*bnd_upr_dst[crd_idx_f_0bs_lat_dst]; /* [nbr] Number of elements in variable */
  for(idx=0;idx<var_sz_dst;idx++) msk_dst[idx]=0;

  /* Create source grid with same sizes as those in input data file */
  ESMC_InterfaceInt max_idx_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Util/interface/ESMC_Interface.C
     NB: ESMF is fragile in that dynamic memory provided as input to grids cannot be immediately freed
     In other words, ESMF copies the values of pointers but not the contents of pointers to provided arrays
     To be concrete, the max_idx array provided below cannot be freed until after the ESMC_Finalize() is called */
  max_idx_src=ESMC_InterfaceIntCreate(max_idx,dmn_nbr_grd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  ESMC_Grid grd_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  grd_src=ESMC_GridCreateNoPeriDim(max_idx_src,&crd_sys,&typ_knd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* fxm: why destroy this now? */
  ESMC_InterfaceIntDestroy(&max_idx_src);
  /* Define stagger locations on source grid. Necessary for remapping later? */
  rcd_esmf=ESMC_GridAddCoord(grd_src,stg_lcn_src);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  
  /* Allocate */
  int *bnd_lwr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_src; /* [dgr] Source grid longitude */
  double *lat_src; /* [dgr] Source grid latitude  */
  lon_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lon_src,stg_lcn_src,bnd_lwr_src,bnd_upr_src,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  lat_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lat_src,stg_lcn_src,NULL,NULL,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"ESMC_VMGet(): localPet = %d, petCount = %d, peCount = %d, pthreadsEnabledFlag = %s, openMPEnabledFlag = %s\n",localPet,petCount,peCount,flg_pthreadsEnabledFlag ? "Enabled" : "Disabled",flg_openMPEnabledFlag ? "Enabled" : "Disabled");
    (void)fprintf(stderr,"ESMC_GridCreateFromFile(): filename = %s, fileTypeFlag = %s, isSphere = %s, addCornerStagger = %s, addUserArea = %s, addMask = %s, var_nm = %s, crd_nm = %s\n",rgr->fl_grd_dst,nco_esmf_fl_fmt_sng(grd_fl_typ),flg_isSphere ? "Enabled" : "Disabled",flg_addCornerStagger ? "Enabled" : "Disabled",flg_addUserArea ? "Enabled" : "Disabled",flg_addMask ? "Enabled" : "Disabled",var_nm,crd_nm ? "non-NULL" : "NULL");
    (void)fprintf(stderr,"ESMC_GridGetCoord(): bnd_lwr_dst[0..1] = %d, %d; bnd_upr_dst[0..1] = %d, %d;\n",bnd_lwr_dst[0],bnd_lwr_dst[1],bnd_upr_dst[0],bnd_upr_dst[1]);
    (void)fprintf(stderr,"ESMC_InterfaceIntCreate(): No printable information\n");
    (void)fprintf(stderr,"ESMC_GridCreateNoPeriDim(): crd_sys = %s, typ_knd = %s\n",nco_esmf_crd_sys_sng(crd_sys),nco_esmf_typ_knd_sng(typ_knd));
    (void)fprintf(stderr,"ESMC_GridAddCoord(): stg_lcn_src = %s,  stg_lcn_dst = %s\n",nco_esmf_stg_lcn_sng(stg_lcn_src),nco_esmf_stg_lcn_sng(stg_lcn_dst));
    (void)fprintf(stderr,"ESMC_GridGetCoord(): bnd_lwr_src[0..1] = %d, %d; bnd_upr_src[0..1] = %d, %d;\n",bnd_lwr_src[0],bnd_lwr_src[1],bnd_upr_src[0],bnd_upr_src[1]);
    //    (void)fprintf(stderr,"ESMC_InterfaceIntCreate(): max_idx_src[0..1] = %d, %d\n",max_idx_src[0],max_idx_src[1]);
  } /* endif dbg */

  /* Type-conversion and cell-center coordinates */
  for(idx=0;idx<bnd_upr_src[crd_idx_f_0bs_lat_src];idx++) lat_src[idx]=lat_in[idx];
  for(idx=0;idx<bnd_upr_src[crd_idx_f_0bs_lon_src];idx++) lon_src[idx]=lon_in[idx];
  idx=0;
  for(int idx_lat=0;idx_lat<bnd_upr_src[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=0;idx_lon<bnd_upr_src[crd_idx_f_0bs_lon_src];idx_lon++){
      lon_src[idx]=lon_in[idx_lon];
      lat_src[idx]=lat_in[idx_lat];
      idx++;
    } /* endfor */
  } /* endfor */
  
  /* Create source field from source grid */
  ESMC_Field fld_src;
  ESMC_InterfaceInt *gridToFieldMap=NULL; /* [idx] Map of all grid dimensions to field dimensions */
  ESMC_InterfaceInt *ungriddedLBound=NULL; /* [idx] Lower bounds of ungridded dimensions */
  ESMC_InterfaceInt *ungriddedUBound=NULL; /* [idx] Upper bounds of ungridded dimensions */
  fld_src=ESMC_FieldCreateGridTypeKind(grd_src,typ_knd,stg_lcn_src,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_src",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* Create destination field from destination grid */
  ESMC_Field fld_dst;
  fld_dst=ESMC_FieldCreateGridTypeKind(grd_dst,typ_knd,stg_lcn_dst,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_dst",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Get field pointers */
  double *fld_src_ptr;
  int localDe=0; /* [idx] Local DE for which information is requested [0..localDeCount-1] */
  fld_src_ptr=(double *)ESMC_FieldGetPtr(fld_src,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  double *fld_dst_ptr;
  fld_dst_ptr=(double *)ESMC_FieldGetPtr(fld_dst,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Get variables from input file */
  (void)nco_inq_varid(in_id,rgr->var_nm,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  /* Get dimension IDs from input file */
  (void)nco_inq_vardimid(in_id,var_in_id,dmn_id);
  long var_sz_src=1L; /* [nbr] Number of elements in variable (will be self-multiplied) */
  /* Get input dimension sizes */
  for(idx=0;idx<dmn_nbr;idx++){
    (void)nco_inq_dimlen(in_id,dmn_id[idx],dmn_cnt+idx);
    dmn_srt[idx]=0L;
    var_sz_src*=dmn_cnt[idx];
  } /* end loop over dim */

  /* Allocate space for and obtain input latitude */
  void_ptr_var=(void *)nco_malloc_dbg(var_sz_src*nco_typ_lng(var_typ_in),"Unable to malloc() value buffer when copying hyperslab from input to output file",fnc_nm);
  rcd=nco_get_vara(in_id,var_in_id,dmn_srt,dmn_cnt,void_ptr_var,var_typ_in);
  float *var_fld=(float *)void_ptr_var;

  /* Type-conversion and ensure every cell has data */
  idx=0;
  for(int idx_lat=bnd_lwr_src[crd_idx_f_0bs_lat_src];idx_lat<=bnd_upr_src[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=bnd_lwr_src[crd_idx_f_0bs_lon_src];idx_lon<=bnd_upr_src[crd_idx_f_0bs_lon_src];idx_lon++){
      fld_src_ptr[idx]=(double)var_fld[idx];
      idx++;
    } /* idx_lon */
  } /* idx_lat */

  /* Initialize dst data ptr */
  idx=0;
  for(int idx_lat=bnd_lwr_dst[crd_idx_f_0bs_lat_src];idx_lat<=bnd_upr_dst[crd_idx_f_0bs_lat_src];idx_lat++){
    for(int idx_lon=bnd_lwr_dst[crd_idx_f_0bs_lon_src];idx_lon<=bnd_upr_dst[crd_idx_f_0bs_lon_src];idx_lon++){
      fld_dst_ptr[idx]=0.0;
      idx++;
    } /* idx_lon */
  } /* idx_lat */

  ESMC_LogWrite("nco_rgr_esmf2() invoking ESMC to start regridstore actions",log_msg_typ);
  /* int *msk_val=(int *)nco_malloc(sizeof(int));
     msk_val[0]=1;
     ESMC_InterfaceInt i_msk_val=ESMC_InterfaceIntCreate(msk_val,1,&rcd_esmf);
     rcd_esmf = ESMC_FieldRegridStore(fld_src,fld_dst,&i_msk_val,&i_msk_val,&rte_hnd,NULL,NULL,NULL,&unmap_act,NULL,NULL);
     rcd_esmf=ESMC_FieldRegridStore(fld_src,fld_dst,NULL,NULL,&rte_hnd,NULL,NULL,NULL,&unmap_act,NULL,NULL); */

  ESMC_Field *cll_frc_dst=NULL; /* [frc] Fraction of each cell participating in regridding, destination grid */
  ESMC_Field *cll_frc_src=NULL; /* [frc] Fraction of each cell participating in regridding, source grid */
  ESMC_InterfaceInt *msk_src_rgr=NULL; /* [idx] Points to mask while regridding, source grid */
  /* fxm: unsure whether/why need both msk_dst (above) and msk_dst_rgr (below) */
  ESMC_InterfaceInt *msk_dst_rgr=NULL; /* [idx] Points to mask while regridding, destination grid */
  ESMC_RouteHandle rte_hnd;
  enum ESMC_RegridMethod_Flag rgr_mth=ESMC_REGRIDMETHOD_BILINEAR; /* [flg] Regrid method (default bilinear) */
  enum ESMC_PoleMethod_Flag pll_mth=ESMC_POLEMETHOD_ALLAVG; /* [flg] Regrid method (default ESMC_POLEMETHOD_ALLAVG) */
  enum ESMC_UnmappedAction_Flag unm_act=ESMC_UNMAPPEDACTION_ERROR; /* [flg] Unmapped action (default ESMC_UNMAPPEDACTION_ERROR) */
  int pll_nbr=int_CEWI; /* [nbr] Number of points to average (iff ESMC_POLEMETHOD_NPNTAVG) */
  /* Source: ${DATA}/esmf/src/Infrastructure/Field/interface/ESMC_Field.C */
  rcd_esmf=ESMC_FieldRegridStore(fld_src,fld_dst,msk_src_rgr,msk_dst_rgr,&rte_hnd,&rgr_mth,&pll_mth,&pll_nbr,&unm_act,cll_frc_src,cll_frc_dst);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  enum ESMC_Region_Flag rgn_flg=ESMC_REGION_TOTAL; /* [flg] Whether/how to zero input fields before regridding (default ESMC_REGION_TOTAL) */
  rcd_esmf=ESMC_FieldRegrid(fld_src,fld_dst,rte_hnd,&rgn_flg);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Call once on each PET */
  rcd_esmf=ESMC_Finalize();
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Write regridded data to netCDF file */
  int var_out_id; /* [id] Variable ID */
  int lon_out_id; /* [id] Variable ID for longitude */
  int lat_out_id; /* [id] Variable ID for latitude  */
  int dmn_id_lat; /* [id] Dimension ID */
  int dmn_id_lon; /* [id] Dimension ID */
  char *lat_nm_out=lat_nm_in;
  char *lon_nm_out=lon_nm_in;
  (void)nco_def_dim(out_id,lat_nm_out,bnd_upr_dst[crd_idx_f_0bs_lat_dst],&dmn_id_lat);
  (void)nco_def_dim(out_id,lon_nm_out,bnd_upr_dst[crd_idx_f_0bs_lon_dst],&dmn_id_lon);
  int dmn_ids_out[2]; /* [id] Dimension IDs array for output variable */
  long dmn_srt_out[2];
  long dmn_cnt_out[2];
  dmn_ids_out[crd_idx_c_0bs_lat_dst]=dmn_id_lat;
  dmn_ids_out[crd_idx_c_0bs_lon_dst]=dmn_id_lon;
  (void)nco_def_var(out_id,lon_nm_out,crd_typ_out,(int)1,&dmn_id_lon,&lon_out_id);
  (void)nco_def_var(out_id,lat_nm_out,crd_typ_out,(int)1,&dmn_id_lat,&lat_out_id);
  (void)nco_def_var(out_id,rgr->var_nm,var_typ_out,(int)2,dmn_ids_out,&var_out_id);
  (void)nco_enddef(out_id);
  dmn_srt_out[crd_idx_c_0bs_lat_dst]=0L;
  dmn_srt_out[crd_idx_c_0bs_lon_dst]=0L;
  dmn_cnt_out[crd_idx_c_0bs_lat_dst]=bnd_upr_dst[crd_idx_f_0bs_lat_dst];
  dmn_cnt_out[crd_idx_c_0bs_lon_dst]=bnd_upr_dst[crd_idx_f_0bs_lon_dst];
  (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_out,lat_out,crd_typ_out);
  (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_out,lon_out,crd_typ_out);
  (void)nco_put_vara(out_id,var_out_id,dmn_srt_out,dmn_cnt_out,fld_dst_ptr,var_typ_out);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(rgr->fl_out,rgr->fl_out_tmp,out_id);

  goto rgr_cln; /* Prevent 'rgr_cln' defined but not used warning */

 rgr_cln:
  if(rcd_esmf != ESMF_SUCCESS){
    /* ESMF return codes are enumerated in ESMC_ReturnCodes.h and ESMC_LogErr.h
       However, there seems to be no translator function for these codes */
    (void)fprintf(stderr,"%s: ERROR %s received ESMF return code %d\nSee ESMC_ReturnCodes.h or ESMC_LogErr.h for more information, e.g.,\n/bin/more /usr/local/include/ESMC_ReturnCodes.h | grep %d\n",nco_prg_nm_get(),fnc_nm,rcd_esmf,rcd_esmf);
  } /* rcd_esmf */
  if(max_idx) max_idx=(int *)nco_free(max_idx);
  if(bnd_lwr_src) bnd_lwr_src=(int *)nco_free(bnd_lwr_src);
  if(bnd_upr_src) bnd_upr_src=(int *)nco_free(bnd_upr_src);
  if(bnd_lwr_dst) bnd_lwr_dst=(int *)nco_free(bnd_lwr_dst);
  if(bnd_upr_dst) bnd_upr_dst=(int *)nco_free(bnd_upr_dst);
  if(void_ptr_var) void_ptr_var=(void *)nco_free(void_ptr_var);
  if(lat_out) lat_out=(double *)nco_free(lat_out);
  if(lon_out) lon_out=(double *)nco_free(lon_out);
  if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
  if(dmn_id) dmn_id=(int *)nco_free(dmn_id);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);

  rcd_esmf=ESMC_FieldRegridRelease(&rte_hnd);
  rcd_esmf=ESMC_FieldDestroy(&fld_src);
  rcd_esmf=ESMC_FieldDestroy(&fld_dst);
  rcd_esmf=ESMC_GridDestroy(&grd_src);
  rcd_esmf=ESMC_GridDestroy(&grd_dst);
  ESMC_Finalize();

  return rcd;
} /* !nco_rgr_esmf2() */
#endif /* !ENABLE_ESMF */
