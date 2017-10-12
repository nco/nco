/* $Header$ */

/* Purpose: NCO map-generation utilities */

/* Copyright (C) 2017--2017 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_map.h" /* Map generation */

int /* O [enm] Return code */
nco_map_mk /* [fnc] Create ESMF-format map file */
(rgr_sct * const rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Use information from two grids to create map-file
     ncks -D 1 -O --grd_src=${DATA}/grids/ne30np4_pentagons.091226.nc --grd_dst=${DATA}/grids/129x256_SCRIP.20150901.nc --map=${HOME}/map_foo.nc ~/nco/data/in.nc ~/foo.nc */

  const char fnc_nm[]="nco_map_mk()"; /* [sng] Function name */

  char *fl_in_dst;
  char *fl_in_src;
  char *fl_pth_lcl=NULL;

  const int dmn_nbr_1D=1; /* [nbr] Rank of 1-D grid variables */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_3D=3; /* [nbr] Rank of 3-D grid variables */
  const int dmn_nbr_grd_max=dmn_nbr_3D; /* [nbr] Maximum rank of grid variables */

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fll_md_old; /* [enm] Old fill mode */
  int in_id_dst; /* I [id] Input netCDF file ID */
  int in_id_src; /* I [id] Input netCDF file ID */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int rcd=NC_NOERR;

  int dst_grid_corners_id; /* [id] Destination grid corners dimension ID */
  int dst_grid_rank_id; /* [id] Destination grid rank dimension ID */
  int dst_grid_size_id; /* [id] Destination grid size dimension ID */
  int src_grid_corners_id; /* [id] Source grid corners dimension ID */
  int src_grid_rank_id; /* [id] Source grid rank dimension ID */
  int src_grid_size_id; /* [id] Source grid size dimension ID */

  int *dmn_sz_in_int; /* [nbr] Array of dimension sizes of source grid */
  int *dmn_sz_out_int; /* [nbr] Array of dimension sizes of destination grid */

  int dmn_sz_in_int_id; /* [id] Source grid dimension sizes variable ID */
  int dmn_sz_out_int_id; /* [id] Destination grid dimension sizes variable ID */

  long *dmn_cnt=NULL;
  long *dmn_srt=NULL;

  nco_bool FL_RTR_RMT_LCN_DST;
  nco_bool FL_RTR_RMT_LCN_SRC;
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */

  nco_mpf_sct mpf;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s obtaining source grid from %s\n",nco_prg_nm_get(),fnc_nm,rgr->fl_grd_src);
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s obtaining destination grid from %s\n",nco_prg_nm_get(),fnc_nm,rgr->fl_grd_dst);

  /* Duplicate (because nco_fl_mk_lcl() free()'s fl_in) */
  fl_in_dst=(char *)strdup(rgr->fl_grd_dst);
  fl_in_src=(char *)strdup(rgr->fl_grd_src);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_dst=nco_fl_mk_lcl(fl_in_dst,fl_pth_lcl,&FL_RTR_RMT_LCN_DST);
  fl_in_src=nco_fl_mk_lcl(fl_in_src,fl_pth_lcl,&FL_RTR_RMT_LCN_SRC);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;

  rcd+=nco_fl_open(fl_in_dst,md_open,&bfr_sz_hnt,&in_id_dst);
  rcd+=nco_fl_open(fl_in_src,md_open,&bfr_sz_hnt,&in_id_src);
  
  rcd+=nco_inq_dimid(in_id_dst,"grid_corners",&dst_grid_corners_id);
  rcd+=nco_inq_dimid(in_id_dst,"grid_rank",&dst_grid_rank_id);
  rcd+=nco_inq_dimid(in_id_dst,"grid_size",&dst_grid_size_id);

  rcd+=nco_inq_dimid(in_id_src,"grid_corners",&src_grid_corners_id);
  rcd+=nco_inq_dimid(in_id_src,"grid_rank",&src_grid_rank_id);
  rcd+=nco_inq_dimid(in_id_src,"grid_size",&src_grid_size_id);

  /* Use dimension IDs to get dimension sizes */
  rcd+=nco_inq_dimlen(in_id_src,src_grid_size_id,&mpf.src_grid_size);
  rcd+=nco_inq_dimlen(in_id_dst,dst_grid_size_id,&mpf.dst_grid_size);
  rcd+=nco_inq_dimlen(in_id_src,src_grid_corners_id,&mpf.src_grid_corners);
  rcd+=nco_inq_dimlen(in_id_dst,dst_grid_corners_id,&mpf.dst_grid_corners);
  rcd+=nco_inq_dimlen(in_id_src,src_grid_rank_id,&mpf.src_grid_rank);
  rcd+=nco_inq_dimlen(in_id_dst,dst_grid_rank_id,&mpf.dst_grid_rank);
  assert(mpf.src_grid_size < INT_MAX && mpf.dst_grid_size < INT_MAX);
  
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s mapfile generation input metadata and grid sizes: ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"src_grid_size = n_a = %li, dst_grid_size = n_b = %li, src_grid_corners = nv_a = %li, dst_grid_corners = nv_b = %li, src_grid_rank = %li, dst_grid_rank = %li\n",mpf.src_grid_size,mpf.dst_grid_size,mpf.src_grid_corners,mpf.dst_grid_corners,mpf.src_grid_rank,mpf.dst_grid_rank);
  } /* endif dbg */

  /* Set type of grid conversion */
  nco_rgr_typ_enm nco_rgr_typ=nco_rgr_grd_nil; /* [enm] Type of grid conversion */
  if(mpf.src_grid_rank == 1 && mpf.dst_grid_rank == 1) nco_rgr_typ=nco_rgr_grd_1D_to_1D;
  if(mpf.src_grid_rank == 1 && mpf.dst_grid_rank == 2) nco_rgr_typ=nco_rgr_grd_1D_to_2D;
  if(mpf.src_grid_rank == 2 && mpf.dst_grid_rank == 1) nco_rgr_typ=nco_rgr_grd_2D_to_1D;
  if(mpf.src_grid_rank == 2 && mpf.dst_grid_rank == 2) nco_rgr_typ=nco_rgr_grd_2D_to_2D;
  assert(nco_rgr_typ != nco_rgr_grd_nil);

  /* Save typing later */
  nco_bool flg_grd_in_1D=False;
  nco_bool flg_grd_in_2D=False;
  nco_bool flg_grd_out_1D=False;
  nco_bool flg_grd_out_2D=False;
  if(nco_rgr_typ == nco_rgr_grd_1D_to_1D || nco_rgr_typ == nco_rgr_grd_1D_to_2D) flg_grd_in_1D=True;
  if(nco_rgr_typ == nco_rgr_grd_2D_to_1D || nco_rgr_typ == nco_rgr_grd_2D_to_2D) flg_grd_in_2D=True;
  if(nco_rgr_typ == nco_rgr_grd_1D_to_1D || nco_rgr_typ == nco_rgr_grd_2D_to_1D) flg_grd_out_1D=True;
  if(nco_rgr_typ == nco_rgr_grd_1D_to_2D || nco_rgr_typ == nco_rgr_grd_2D_to_2D) flg_grd_out_2D=True;
  int dmn_nbr_hrz_crd; /* [nbr] Number of horizontal dimensions in output grid */
  if(flg_grd_out_2D) dmn_nbr_hrz_crd=2; else dmn_nbr_hrz_crd=1;
  
  rcd+=nco_inq_varid(in_id_src,"grid_dims",&dmn_sz_in_int_id);
  rcd+=nco_inq_varid(in_id_dst,"grid_dims",&dmn_sz_out_int_id);
  assert(rcd == NC_NOERR);

  dmn_srt=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));
  dmn_cnt=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));

  dmn_sz_in_int=(int *)nco_malloc(mpf.src_grid_rank*nco_typ_lng((nc_type)NC_INT));
  dmn_sz_out_int=(int *)nco_malloc(mpf.dst_grid_rank*nco_typ_lng((nc_type)NC_INT));

  dmn_srt[0]=0L;
  dmn_cnt[0]=mpf.src_grid_rank;
  rcd=nco_get_vara(in_id_src,dmn_sz_in_int_id,dmn_srt,dmn_cnt,dmn_sz_in_int,(nc_type)NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=mpf.dst_grid_rank;
  rcd=nco_get_vara(in_id_dst,dmn_sz_out_int_id,dmn_srt,dmn_cnt,dmn_sz_out_int,(nc_type)NC_INT);

  /* Check-for and workaround faulty grid sizes, typically from bogus dual-grid generation algorithm */
  if(flg_grd_in_1D && (mpf.src_grid_size != dmn_sz_in_int[0])){
    (void)fprintf(stdout,"%s: INFO %s reports input grid dimension sizes disagree: mpf.src_grid_size = %ld != %d = dmn_sz_in[0]. Problem may be caused by incorrect grid_dims variable in source gridfile. This is a known issue with some gridfiles generated prior to ~20150901, particularly for spectral element dual-grids. This problem can be safely ignored if workaround succeeds. Attempting workaround ...\n",nco_prg_nm_get(),fnc_nm,mpf.src_grid_size,dmn_sz_in_int[0]);
      dmn_sz_in_int[0]=mpf.src_grid_size;
  } /* !bug */
  if(flg_grd_out_1D && (mpf.dst_grid_size != dmn_sz_out_int[0])){
    (void)fprintf(stdout,"%s: INFO %s reports output grid dimension sizes disagree: mpf.dst_grid_size = %ld != %d = dmn_sz_out[0]. Problem may be caused by incorrect grid_dims variable in destination gridfile. This is a known issue with gridfiles generated prior to ~20150901, particularly for spectral element dual-grids. This problem can be safely ignored if workaround succeeds. Attempting workaround ...\n",nco_prg_nm_get(),fnc_nm,mpf.dst_grid_size,dmn_sz_out_int[0]);
    dmn_sz_out_int[0]=mpf.dst_grid_size;
  } /* !bug */

  int lon_psn; /* [idx] Ordinal position of longitude in source grid dimension-size array */
  int lat_psn; /* [idx] Ordinal position of latitude  in source grid dimension-size array */
  lon_psn=0; /* SCRIP introduced [lon,lat] convention because more natural for Fortran */
  lat_psn=1;

  int bnd_nbr_in=int_CEWI; /* [nbr] Number of vertices for source grid coordinates */
  long col_nbr_in; /* [idx] Number of columns in source grid */
  long lon_nbr_in; /* [idx] Number of longitudes in source grid */
  long lat_nbr_in; /* [idx] Number of latitudes  in source grid */
  const size_t grd_sz_in=mpf.src_grid_size; /* [nbr] Number of elements in single layer of input grid */
  const size_t grd_sz_out=mpf.dst_grid_size; /* [nbr] Number of elements in single layer of output grid */
  bnd_nbr_in=mpf.src_grid_corners;
  if(flg_grd_in_1D){
    col_nbr_in=dmn_sz_in_int[0];
    lon_nbr_in=dmn_sz_in_int[0];
    lat_nbr_in=dmn_sz_in_int[0];
  }else if(flg_grd_in_2D){
    col_nbr_in=0;
    lon_nbr_in=dmn_sz_in_int[lon_psn];
    lat_nbr_in=dmn_sz_in_int[lat_psn];
    /* Sanity-check */
    assert(lat_nbr_in*lon_nbr_in == (long)grd_sz_in);
  } /* !src_grid_rank */

  int bnd_nbr_out=int_CEWI; /* [nbr] Number of vertices for destination grid coordinates */
  long col_nbr_out=long_CEWI; /* [nbr] Number of columns in destination grid */
  long lon_nbr_out=long_CEWI; /* [nbr] Number of longitudes in rectangular destination grid */
  long lat_nbr_out=long_CEWI; /* [nbr] Number of latitudes  in rectangular destination grid */
  bnd_nbr_out=mpf.dst_grid_corners;
  if(flg_grd_out_1D){
    col_nbr_out=dmn_sz_out_int[0];
    lat_nbr_out=dmn_sz_out_int[0];
    lon_nbr_out=dmn_sz_out_int[0];
    /* Sanity-check */
    assert(col_nbr_out == (long)grd_sz_out);
  }else if(flg_grd_out_2D){
    col_nbr_out=lat_nbr_out*lon_nbr_out;
    lat_nbr_out=dmn_sz_out_int[lat_psn];
    lon_nbr_out=dmn_sz_out_int[lon_psn];
    /* Sanity-check */
    assert(lat_nbr_out*lon_nbr_out == (long)grd_sz_out);
  } /* !dst_grid_rank */

  /* Close input netCDF files */
  nco_close(in_id_dst);
  nco_close(in_id_src);

  /* Remove local copy of input files */
  if(FL_RTR_RMT_LCN_DST && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in_dst);
  if(FL_RTR_RMT_LCN_SRC && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in_src);

  /* Copy arguments from mapfile structure to more easily recognized names */
  long dst_grd_crn_nbr; /* [nbr] Number of corners in gridcell */
  long dst_grd_rnk_nbr=int_CEWI; /* [nbr] Number of dimensions in grid */
  long dst_grd_sz_nbr; /* [nbr] Number of gridcells in grid */
  long src_grd_crn_nbr; /* [nbr] Number of corners in gridcell */
  long src_grd_rnk_nbr=int_CEWI; /* [nbr] Number of dimensions in grid */
  long src_grd_sz_nbr; /* [nbr] Number of gridcells in grid */
  dst_grd_rnk_nbr=mpf.dst_grid_rank;
  src_grd_rnk_nbr=mpf.src_grid_rank;
  dst_grd_crn_nbr=mpf.dst_grid_corners;
  src_grd_crn_nbr=mpf.src_grid_corners;
  dst_grd_sz_nbr=mpf.dst_grid_size;
  src_grd_sz_nbr=mpf.src_grid_size;

  /* Set-up output file */
  char *fl_out_tmp=NULL_CEWI;
  char *fl_out;

  int deflate; /* [flg] Turn-on deflate filter */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int out_id; /* I [id] Output netCDF file ID */
  int shuffle; /* [flg] Turn-on shuffle filter */

  int num_links_id; /* [id] Number of links dimension ID */
  int num_wgts_id; /* [id] Number of weights dimension ID */

  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */

  size_t wgt_nbr=1L; /* [nbr] Number of weights */
  size_t lnk_nbr; /* [nbr] Number of links */
  size_t lnk_idx; /* [idx] Link index */

  deflate=(int)True;
  shuffle=NC_SHUFFLE;
  dfl_lvl=rgr->dfl_lvl;
  fl_out_fmt=rgr->fl_out_fmt;
  fl_out=rgr->fl_map;

  /* Spoof arguments to be carefuly defined later */
  lnk_nbr=1L;

  /* Open mapfile */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);

  rcd+=nco_def_dim(out_id,"dst_grid_rank",dst_grd_rnk_nbr,&dst_grid_rank_id);
  rcd+=nco_def_dim(out_id,"n_a",src_grd_sz_nbr,&src_grid_size_id);
  rcd+=nco_def_dim(out_id,"n_b",dst_grd_sz_nbr,&dst_grid_size_id);
  rcd+=nco_def_dim(out_id,"n_s",lnk_nbr,&num_links_id);
  rcd+=nco_def_dim(out_id,"num_wgts",wgt_nbr,&num_wgts_id);
  rcd+=nco_def_dim(out_id,"nv_a",src_grd_crn_nbr,&src_grid_corners_id);
  rcd+=nco_def_dim(out_id,"nv_b",dst_grd_crn_nbr,&dst_grid_corners_id);
  rcd+=nco_def_dim(out_id,"src_grid_rank",src_grd_rnk_nbr,&src_grid_rank_id);

  rcd+=nco_def_var(out_id,"src_grid_dims",NC_INT,dmn_nbr_1D,&src_grid_rank_id,&dmn_sz_in_int_id);
  rcd+=nco_def_var(out_id,"dst_grid_dims",NC_INT,dmn_nbr_1D,&dst_grid_rank_id,&dmn_sz_out_int_id);

  /* Turn-off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
      
  /* Begin data mode */
  (void)nco_enddef(out_id);

  /* Write values to mapfile */
  dmn_srt[0]=0L;
  dmn_cnt[0]=mpf.src_grid_rank;
  rcd=nco_put_vara(out_id,dmn_sz_in_int_id,dmn_srt,dmn_cnt,dmn_sz_in_int,(nc_type)NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=mpf.dst_grid_rank;
  rcd=nco_put_vara(out_id,dmn_sz_out_int_id,dmn_srt,dmn_cnt,dmn_sz_out_int,(nc_type)NC_INT);

  /* Close input netCDF file */
  nco_close(out_id);
  
  assert(rcd == NC_NOERR);

  /* Clean-up dynamic memory */
  if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  if(dmn_sz_in_int) dmn_sz_in_int=(int *)nco_free(dmn_sz_in_int);
  if(dmn_sz_out_int) dmn_sz_out_int=(int *)nco_free(dmn_sz_out_int);

  if(fl_in_dst) fl_in_dst=(char *)nco_free(fl_in_dst);
  if(fl_in_src) fl_in_src=(char *)nco_free(fl_in_src);

  return rcd;
} /* !nco_map_mk() */
