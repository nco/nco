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

  const double rdn2dgr=180.0/M_PI;
  const double dgr2rdn=M_PI/180.0;
  const double eps_rlt=1.0e-14; /* [frc] Round-off error tolerance */

  double *area_in=NULL; /* [sr] Area of source grid */
  double *frc_in=NULL; /* [frc] Fraction of source grid */
  double *lat_crn_in=NULL; /* [dgr] Latitude  corners of source grid */
  double *lat_ctr_in=NULL_CEWI; /* [dgr] Latitude  centers of source grid */
  double *lon_crn_in=NULL; /* [dgr] Longitude corners of source grid */
  double *lon_ctr_in=NULL_CEWI; /* [dgr] Longitude centers of source grid */

  double *area_out=NULL; /* [sr] Area of destination grid */
  double *frc_out=NULL; /* [frc] Fraction of destination grid */
  double *lat_crn_out=NULL; /* [dgr] Latitude  corners of destination grid */
  double *lat_ctr_out=NULL_CEWI; /* [dgr] Latitude  centers of destination grid */
  double *lon_crn_out=NULL; /* [dgr] Longitude corners of destination grid */
  double *lon_ctr_out=NULL_CEWI; /* [dgr] Longitude centers of destination grid */
  
  int *msk_in=NULL; /* [flg] Mask on source grid */
  int *msk_out=NULL; /* [flg] Mask on destination grid */

  const int dmn_nbr_1D=1; /* [nbr] Rank of 1-D grid variables */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_3D=3; /* [nbr] Rank of 3-D grid variables */
  const int dmn_nbr_grd_max=dmn_nbr_3D; /* [nbr] Maximum rank of grid variables */

  int dmn_ids[dmn_nbr_grd_max]; /* [id] Dimension IDs array for output variable */

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

  int area_in_id; /* [id] Area variable ID */
  int frc_in_id; /* [id] Fraction variable ID */
  int msk_in_id=NC_MIN_INT; /* [id] Mask variable ID */
  int src_grd_crn_lat_id; /* [id] Source grid corner latitudes  variable ID */
  int src_grd_crn_lon_id; /* [id] Source grid corner longitudes variable ID */
  int src_grd_ctr_lat_id; /* [id] Source grid center latitudes  variable ID */
  int src_grd_ctr_lon_id; /* [id] Source grid center longitudes variable ID */

  int area_out_id; /* [id] Area variable ID */
  int frc_out_id; /* [id] Fraction variable ID */
  int msk_out_id=NC_MIN_INT; /* [id] Mask variable ID */
  int dst_grd_crn_lat_id; /* [id] Destination grid corner latitudes  variable ID */
  int dst_grd_crn_lon_id; /* [id] Destination grid corner longitudes variable ID */
  int dst_grd_ctr_lat_id; /* [id] Destination grid center latitudes  variable ID */
  int dst_grd_ctr_lon_id; /* [id] Destination grid center longitudes variable ID */
  
  long *dmn_cnt=NULL;
  long *dmn_srt=NULL;

  long idx; /* [idx] Counting index for unrolled grids */

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

  /* SCRIP introduced [lon,lat] convention because more natural for Fortran */
  const int lon_psn=0; /* [idx] Ordinal position of longitude in source grid dimension-size array */
  const int lat_psn=1; /* [idx] Ordinal position of latitude  in source grid dimension-size array */

  int bnd_nbr_in; /* [nbr] Number of vertices for source grid coordinates */
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

  int bnd_nbr_out; /* [nbr] Number of vertices for destination grid coordinates */
  long col_nbr_out; /* [nbr] Number of columns in destination grid */
  long lon_nbr_out; /* [nbr] Number of longitudes in destination grid */
  long lat_nbr_out; /* [nbr] Number of latitudes  in destination grid */
  bnd_nbr_out=mpf.dst_grid_corners;
  if(flg_grd_out_1D){
    col_nbr_out=dmn_sz_out_int[0];
    lat_nbr_out=dmn_sz_out_int[0];
    lon_nbr_out=dmn_sz_out_int[0];
    /* Sanity-check */
    assert(col_nbr_out == (long)grd_sz_out);
  }else if(flg_grd_out_2D){
    lat_nbr_out=dmn_sz_out_int[lat_psn];
    lon_nbr_out=dmn_sz_out_int[lon_psn];
    col_nbr_out=lat_nbr_out*lon_nbr_out;
    /* Sanity-check */
    assert(lat_nbr_out*lon_nbr_out == (long)grd_sz_out);
  } /* !dst_grid_rank */

  /* Obtain grid values necessary to compute overlap mesh */
  rcd+=nco_inq_varid(in_id_src,"grid_area",&area_in_id); /* ESMF: area_a */
  rcd+=nco_inq_varid(in_id_src,"grid_center_lon",&src_grd_ctr_lon_id); /* ESMF: xc_a */
  rcd+=nco_inq_varid(in_id_src,"grid_center_lat",&src_grd_ctr_lat_id); /* ESMF: yc_a */
  rcd+=nco_inq_varid(in_id_src,"grid_corner_lon",&src_grd_crn_lon_id); /* ESMF: xv_a */
  rcd+=nco_inq_varid(in_id_src,"grid_corner_lat",&src_grd_crn_lat_id); /* ESMF: yv_a */
  rcd+=nco_inq_varid(in_id_src,"grid_imask",&msk_in_id); /* ESMF: msk_a */

  rcd+=nco_inq_varid(in_id_dst,"grid_area",&area_out_id); /* ESMF: area_b */
  rcd+=nco_inq_varid(in_id_dst,"grid_center_lon",&dst_grd_ctr_lon_id); /* ESMF: xc_b */
  rcd+=nco_inq_varid(in_id_dst,"grid_center_lat",&dst_grd_ctr_lat_id); /* ESMF: yc_b */
  rcd+=nco_inq_varid(in_id_dst,"grid_corner_lon",&dst_grd_crn_lon_id); /* ESMF: xv_b */
  rcd+=nco_inq_varid(in_id_dst,"grid_corner_lat",&dst_grd_crn_lat_id); /* ESMF: yv_b */
  rcd+=nco_inq_varid(in_id_dst,"grid_imask",&msk_out_id); /* ESMF: msk_b */

  /* Ensure coordinates are in degrees not radians for simplicity and CF-compliance
     NB: ${DATA}/scrip/rmp_T42_to_POP43_conserv.nc has [xy]?_a in degrees and [xy]?_b in radians! */
  char *att_val;
  char unt_sng[]="units"; /* [sng] netCDF-standard units attribute name */
  long att_sz;
  nc_type att_typ;
  nco_bool flg_crd_rdn_src=False; /* [flg] Source coordinates are in radians not degrees */
  rcd=nco_inq_att_flg(in_id_src,src_grd_ctr_lat_id,unt_sng,&att_typ,&att_sz);
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    att_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
    rcd+=nco_get_att(in_id_src,src_grd_ctr_lat_id,unt_sng,att_val,att_typ);
    /* NUL-terminate attribute before using strstr() */
    att_val[att_sz]='\0';
    /* Match "radian" and "radians" */
    if(strstr(att_val,"radian")) flg_crd_rdn_src=True;
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* end rcd && att_typ */
  nco_bool flg_crd_rdn_dst=False; /* [flg] Destination coordinates are in radians not degrees */
  rcd=nco_inq_att_flg(in_id_dst,dst_grd_ctr_lat_id,unt_sng,&att_typ,&att_sz);
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    att_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
    rcd+=nco_get_att(in_id_dst,dst_grd_ctr_lat_id,unt_sng,att_val,att_typ);
    /* NUL-terminate attribute before using strstr() */
    att_val[att_sz]='\0';
    /* Match "radian" and "radians" */
    if(strstr(att_val,"radian")) flg_crd_rdn_dst=True;
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* end rcd && att_typ */

  nco_bool flg_grd_in_crv=False; /* [flg] Curvilinear coordinates */
  nco_bool flg_grd_in_rct=False; /* [flg] Rectangular coordinates */
  nco_bool flg_grd_out_crv=False; /* [flg] Curvilinear coordinates */
  nco_bool flg_grd_out_rct=False; /* [flg] Rectangular coordinates */
  const nc_type crd_typ=NC_DOUBLE;

  area_in=(double *)nco_malloc(grd_sz_in*nco_typ_lng(crd_typ));
  frc_in=(double *)nco_malloc(grd_sz_in*nco_typ_lng(crd_typ));
  msk_in=(int *)nco_malloc(grd_sz_in*nco_typ_lng(crd_typ));
  lon_ctr_in=(double *)nco_malloc(grd_sz_in*nco_typ_lng(crd_typ));
  lat_ctr_in=(double *)nco_malloc(grd_sz_in*nco_typ_lng(crd_typ));
  lon_crn_in=(double *)nco_malloc(mpf.src_grid_corners*grd_sz_in*nco_typ_lng(crd_typ));
  lat_crn_in=(double *)nco_malloc(mpf.src_grid_corners*grd_sz_in*nco_typ_lng(crd_typ));
  
  area_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  frc_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  msk_out=(int *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  lon_ctr_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  lat_ctr_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  lon_crn_out=(double *)nco_malloc(mpf.dst_grid_corners*grd_sz_out*nco_typ_lng(crd_typ));
  lat_crn_out=(double *)nco_malloc(mpf.dst_grid_corners*grd_sz_out*nco_typ_lng(crd_typ));
  
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_in;
  rcd=nco_get_vara(in_id_src,area_in_id,dmn_srt,dmn_cnt,area_in,crd_typ);
  rcd=nco_get_vara(in_id_src,msk_in_id,dmn_srt,dmn_cnt,msk_in,(nc_type)NC_INT);
  rcd=nco_get_vara(in_id_src,src_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_in,crd_typ);
  rcd=nco_get_vara(in_id_src,src_grd_ctr_lat_id,dmn_srt,dmn_cnt,lat_ctr_in,crd_typ);
  dmn_srt[0]=dmn_srt[1]=0L;
  dmn_cnt[0]=grd_sz_in;
  dmn_cnt[1]=mpf.src_grid_corners;
  rcd=nco_get_vara(in_id_src,src_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn_in,crd_typ);
  rcd=nco_get_vara(in_id_src,src_grd_crn_lat_id,dmn_srt,dmn_cnt,lat_crn_in,crd_typ);

  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_out;
  rcd=nco_get_vara(in_id_dst,area_out_id,dmn_srt,dmn_cnt,area_out,crd_typ);
  rcd=nco_get_vara(in_id_dst,msk_out_id,dmn_srt,dmn_cnt,msk_out,(nc_type)NC_INT);
  rcd=nco_get_vara(in_id_dst,dst_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_out,crd_typ);
  rcd=nco_get_vara(in_id_dst,dst_grd_ctr_lat_id,dmn_srt,dmn_cnt,lat_ctr_out,crd_typ);
  dmn_srt[0]=dmn_srt[1]=0L;
  dmn_cnt[0]=grd_sz_out;
  dmn_cnt[1]=mpf.dst_grid_corners;
  rcd=nco_get_vara(in_id_dst,dst_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn_out,crd_typ);
  rcd=nco_get_vara(in_id_dst,dst_grd_crn_lat_id,dmn_srt,dmn_cnt,lat_crn_out,crd_typ);
  
  if(flg_crd_rdn_src){
    for(idx=0;idx<grd_sz_in;idx++){
      lon_ctr_in[idx]*=rdn2dgr;
      lat_ctr_in[idx]*=rdn2dgr;
    } /* !idx */
    long grd_crn_sz_in;
    grd_crn_sz_in=mpf.src_grid_corners*grd_sz_in;
    for(idx=0;idx<grd_crn_sz_in;idx++){
      lon_crn_in[idx]*=rdn2dgr;
      lat_crn_in[idx]*=rdn2dgr;
    } /* !idx */
  } /* !rdn */
  
  if(flg_crd_rdn_dst){
    for(idx=0;idx<grd_sz_out;idx++){
      lon_ctr_out[idx]*=rdn2dgr;
      lat_ctr_out[idx]*=rdn2dgr;
    } /* !idx */
    long grd_crn_sz_out;
    grd_crn_sz_out=mpf.dst_grid_corners*grd_sz_out;
    for(idx=0;idx<grd_crn_sz_out;idx++){
      lon_crn_out[idx]*=rdn2dgr;
      lat_crn_out[idx]*=rdn2dgr;
    } /* !idx */
  } /* !rdn */

  /* User may specify curvilinear grid (with --rgr crv). Otherwise, manually test for curvilinear source grid. */
  if(flg_grd_in_2D){
    flg_grd_in_crv=rgr->flg_crv; /* [flg] Curvilinear coordinates */
    if(flg_grd_in_crv){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Output grid specified to be %s\n",nco_prg_nm_get(),flg_grd_in_crv ? "Curvilinear" : "Rectangular");
    }else{
      long idx_tst=long_CEWI; /* [idx] Index of first latitude or longitude */
      for(idx=0;idx<(long)grd_sz_in;idx++){
	if(idx%lon_nbr_in == 0) idx_tst=idx;
	if(lat_ctr_in[idx] != lat_ctr_in[idx_tst]) break;
      } /* !rectangular */
      if(idx != (long)grd_sz_in) flg_grd_in_crv=True; else flg_grd_in_rct=True;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Source grid detected to be %s\n",nco_prg_nm_get(),flg_grd_in_crv ? "Curvilinear" : "Rectangular");
    } /* !flg_grd_in_crv */
  } /* flg_grd_in_2D */

  /* User may specify curvilinear grid (with --rgr crv). Otherwise, manually test for curvilinear destination grid. */
  if(flg_grd_out_2D){
    flg_grd_out_crv=rgr->flg_crv; /* [flg] Curvilinear coordinates */
    if(flg_grd_out_crv){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Output grid specified to be %s\n",nco_prg_nm_get(),flg_grd_out_crv ? "Curvilinear" : "Rectangular");
    }else{
      long idx_tst=long_CEWI; /* [idx] Index of first latitude or longitude */
      for(idx=0;idx<(long)grd_sz_out;idx++){
	if(idx%lon_nbr_out == 0) idx_tst=idx;
	if(lat_ctr_out[idx] != lat_ctr_out[idx_tst]) break;
      } /* !rectangular */
      if(idx != (long)grd_sz_out) flg_grd_out_crv=True; else flg_grd_out_rct=True;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Destination grid detected to be %s\n",nco_prg_nm_get(),flg_grd_out_crv ? "Curvilinear" : "Rectangular");
    } /* !flg_grd_out_crv */
  } /* flg_grd_out_2D */
  
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

  int col_src_adr_id; /* [id] Source address (col) variable ID */
  int row_dst_adr_id; /* [id] Destination address (row) variable ID */
  int wgt_raw_id; /* [id] Remap matrix variable ID */

  double *wgt_raw=NULL; /* [frc] Remapping weights */
  int *col_src_adr=NULL; /* [idx] Source address (col) */
  int *row_dst_adr=NULL; /* [idx] Destination address (row) */

  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */

  size_t wgt_nbr=1L; /* [nbr] Number of weights */
  size_t lnk_nbr; /* [nbr] Number of links */
  //size_t lnk_idx; /* [idx] Link index */

  deflate=(int)True;
  shuffle=NC_SHUFFLE;
  dfl_lvl=rgr->dfl_lvl;
  fl_out_fmt=rgr->fl_out_fmt;
  fl_out=rgr->fl_map;

  /* Define overlap mesh vertices, count links, compute overlap weights
     This includes Mohammed Abouali code to create triangle list and Zender code to compute triangle areas */
  (void)nco_msh_mk
    (rgr,
     area_in,msk_in,lat_ctr_in,lon_ctr_in,lat_crn_in,lon_crn_in,grd_sz_in,mpf.src_grid_corners,
     area_out,msk_out,lat_ctr_out,lon_ctr_out,lat_crn_out,lon_crn_out,grd_sz_out,mpf.dst_grid_corners,
     frc_in,frc_out,&col_src_adr,&row_dst_adr,&wgt_raw,&lnk_nbr);

  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Defining mapfile based on %li links\n",nco_prg_nm_get(),lnk_nbr);

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

  rcd+=nco_def_var(out_id,"S",crd_typ,dmn_nbr_1D,&num_links_id,&wgt_raw_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,wgt_raw_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"col",(nc_type)NC_INT,dmn_nbr_1D,&num_links_id,&col_src_adr_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,col_src_adr_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"row",(nc_type)NC_INT,dmn_nbr_1D,&num_links_id,&row_dst_adr_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,row_dst_adr_id,shuffle,deflate,dfl_lvl);

  rcd+=nco_def_var(out_id,"area_a",crd_typ,dmn_nbr_1D,&src_grid_size_id,&area_in_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_in_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"frac_a",crd_typ,dmn_nbr_1D,&src_grid_size_id,&frc_in_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,frc_in_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"mask_a",(nc_type)NC_INT,dmn_nbr_1D,&src_grid_size_id,&msk_in_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,msk_in_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"xc_a",crd_typ,dmn_nbr_1D,&src_grid_size_id,&src_grd_ctr_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,src_grd_ctr_lon_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"yc_a",crd_typ,dmn_nbr_1D,&src_grid_size_id,&src_grd_ctr_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,src_grd_ctr_lat_id,shuffle,deflate,dfl_lvl);
  dmn_ids[0]=src_grid_size_id;
  dmn_ids[1]=src_grid_corners_id;
  rcd+=nco_def_var(out_id,"xv_a",crd_typ,dmn_nbr_2D,dmn_ids,&src_grd_crn_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,src_grd_crn_lon_id,shuffle,deflate,dfl_lvl);
  dmn_ids[0]=src_grid_size_id;
  dmn_ids[1]=src_grid_corners_id;
  rcd+=nco_def_var(out_id,"yv_a",crd_typ,dmn_nbr_2D,dmn_ids,&src_grd_crn_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,src_grd_crn_lat_id,shuffle,deflate,dfl_lvl);

  rcd+=nco_def_var(out_id,"area_b",crd_typ,dmn_nbr_1D,&dst_grid_size_id,&area_out_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_out_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"frac_b",crd_typ,dmn_nbr_1D,&dst_grid_size_id,&frc_out_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,frc_out_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"mask_b",(nc_type)NC_INT,dmn_nbr_1D,&dst_grid_size_id,&msk_out_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,msk_out_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"xc_b",crd_typ,dmn_nbr_1D,&dst_grid_size_id,&dst_grd_ctr_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,dst_grd_ctr_lon_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"yc_b",crd_typ,dmn_nbr_1D,&dst_grid_size_id,&dst_grd_ctr_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,dst_grd_ctr_lat_id,shuffle,deflate,dfl_lvl);
  dmn_ids[0]=dst_grid_size_id;
  dmn_ids[1]=dst_grid_corners_id;
  rcd+=nco_def_var(out_id,"xv_b",crd_typ,dmn_nbr_2D,dmn_ids,&dst_grd_crn_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,dst_grd_crn_lon_id,shuffle,deflate,dfl_lvl);
  dmn_ids[0]=dst_grid_size_id;
  dmn_ids[1]=dst_grid_corners_id;
  rcd+=nco_def_var(out_id,"yv_b",crd_typ,dmn_nbr_2D,dmn_ids,&dst_grd_crn_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,dst_grd_crn_lat_id,shuffle,deflate,dfl_lvl);
  
  /* Define global and "units" attributes */
  aed_sct aed_mtd;
  char *att_nm;

  att_nm=strdup("title");
  att_val=strdup(rgr->grd_ttl);
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=NULL;
  aed_mtd.id=NC_GLOBAL;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,NC_GLOBAL,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);
  
  att_nm=strdup("Conventions");
  att_val=strdup("netCDF Operators (NCO) Offline Regridding Weight Generator");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=NULL;
  aed_mtd.id=NC_GLOBAL;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,NC_GLOBAL,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);
  
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */
  att_nm=strdup("created_by");
  att_val=strdup(usr_cpp);
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=NULL;
  aed_mtd.id=NC_GLOBAL;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,NC_GLOBAL,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);
  
  (void)nco_hst_att_cat(out_id,rgr->cmd_ln);
  (void)nco_vrs_att_cat(out_id);

  /* Variables with units="degrees_north" */
  att_nm=strdup("units");
  att_val=strdup("degrees_north");
  aed_mtd.att_nm=att_nm;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;

  aed_mtd.var_nm="yc_a";
  aed_mtd.id=src_grd_ctr_lat_id;
  (void)nco_aed_prc(out_id,src_grd_ctr_lat_id,aed_mtd);
  aed_mtd.var_nm="yv_a";
  aed_mtd.id=src_grd_crn_lat_id;
  (void)nco_aed_prc(out_id,src_grd_crn_lat_id,aed_mtd);
  aed_mtd.var_nm="yc_b";
  aed_mtd.id=dst_grd_ctr_lat_id;
  (void)nco_aed_prc(out_id,dst_grd_ctr_lat_id,aed_mtd);
  aed_mtd.var_nm="yv_b";
  aed_mtd.id=dst_grd_crn_lat_id;
  (void)nco_aed_prc(out_id,dst_grd_crn_lat_id,aed_mtd);

  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  /* Variables with units="degrees_east" */
  att_nm=strdup("units");
  att_val=strdup("degrees_east");
  aed_mtd.att_nm=att_nm;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;

  aed_mtd.var_nm="xc_a";
  aed_mtd.id=src_grd_ctr_lon_id;
  (void)nco_aed_prc(out_id,src_grd_ctr_lon_id,aed_mtd);
  aed_mtd.var_nm="xv_a";
  aed_mtd.id=src_grd_crn_lon_id;
  (void)nco_aed_prc(out_id,src_grd_crn_lon_id,aed_mtd);
  aed_mtd.var_nm="xc_b";
  aed_mtd.id=dst_grd_ctr_lon_id;
  (void)nco_aed_prc(out_id,dst_grd_ctr_lon_id,aed_mtd);
  aed_mtd.var_nm="xv_b";
  aed_mtd.id=dst_grd_crn_lon_id;
  (void)nco_aed_prc(out_id,dst_grd_crn_lon_id,aed_mtd);

  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);
  
  /* Variables with units="none" */
  att_nm=strdup("units");
  att_val=strdup("none");
  aed_mtd.att_nm=att_nm;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;

  aed_mtd.var_nm="frac_a";
  aed_mtd.id=frc_in_id;
  (void)nco_aed_prc(out_id,frc_in_id,aed_mtd);
  aed_mtd.var_nm="mask_a";
  aed_mtd.id=msk_in_id;
  (void)nco_aed_prc(out_id,msk_in_id,aed_mtd);
  aed_mtd.var_nm="frac_b";
  aed_mtd.id=frc_out_id;
  (void)nco_aed_prc(out_id,frc_out_id,aed_mtd);
  aed_mtd.var_nm="mask_b";
  aed_mtd.id=msk_out_id;
  (void)nco_aed_prc(out_id,msk_out_id,aed_mtd);

  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  att_nm=strdup("units");
  att_val=strdup("steradian");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm="area_a";
  aed_mtd.id=area_in_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,area_in_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  att_nm=strdup("units");
  att_val=strdup("steradian");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm="area_b";
  aed_mtd.id=area_out_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,area_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

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

  dmn_srt[0]=0L;
  dmn_cnt[0]=lnk_nbr;
  rcd=nco_put_vara(out_id,wgt_raw_id,dmn_srt,dmn_cnt,wgt_raw,crd_typ);
  rcd=nco_put_vara(out_id,col_src_adr_id,dmn_srt,dmn_cnt,col_src_adr,(nc_type)NC_INT);
  rcd=nco_put_vara(out_id,row_dst_adr_id,dmn_srt,dmn_cnt,row_dst_adr,(nc_type)NC_INT);

  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_in;
  rcd=nco_put_vara(out_id,area_in_id,dmn_srt,dmn_cnt,area_in,crd_typ);
  rcd=nco_put_vara(out_id,frc_in_id,dmn_srt,dmn_cnt,frc_in,crd_typ);
  rcd=nco_put_vara(out_id,msk_in_id,dmn_srt,dmn_cnt,msk_in,(nc_type)NC_INT);
  rcd=nco_put_vara(out_id,src_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_in,crd_typ);
  rcd=nco_put_vara(out_id,src_grd_ctr_lat_id,dmn_srt,dmn_cnt,lat_ctr_in,crd_typ);
  dmn_srt[0]=dmn_srt[1]=0L;
  dmn_cnt[0]=grd_sz_in;
  dmn_cnt[1]=mpf.src_grid_corners;
  rcd=nco_put_vara(out_id,src_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn_in,crd_typ);
  rcd=nco_put_vara(out_id,src_grd_crn_lat_id,dmn_srt,dmn_cnt,lat_crn_in,crd_typ);
  
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_out;
  rcd=nco_put_vara(out_id,area_out_id,dmn_srt,dmn_cnt,area_out,crd_typ);
  rcd=nco_put_vara(out_id,frc_out_id,dmn_srt,dmn_cnt,frc_out,crd_typ);
  rcd=nco_put_vara(out_id,msk_out_id,dmn_srt,dmn_cnt,msk_out,(nc_type)NC_INT);
  rcd=nco_put_vara(out_id,dst_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_out,crd_typ);
  rcd=nco_put_vara(out_id,dst_grd_ctr_lat_id,dmn_srt,dmn_cnt,lat_ctr_out,crd_typ);
  dmn_srt[0]=dmn_srt[1]=0L;
  dmn_cnt[0]=grd_sz_out;
  dmn_cnt[1]=mpf.dst_grid_corners;
  rcd=nco_put_vara(out_id,dst_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn_out,crd_typ);
  rcd=nco_put_vara(out_id,dst_grd_crn_lat_id,dmn_srt,dmn_cnt,lat_crn_out,crd_typ);
  
  /* Close input netCDF file */
  nco_close(out_id);
  
  assert(rcd == NC_NOERR);

  /* Clean-up dynamic memory */
  if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  if(dmn_sz_in_int) dmn_sz_in_int=(int *)nco_free(dmn_sz_in_int);
  if(dmn_sz_out_int) dmn_sz_out_int=(int *)nco_free(dmn_sz_out_int);

  if(area_in) area_in=(double *)nco_free(area_in);
  if(frc_in) frc_in=(double *)nco_free(frc_in);
  if(msk_in) msk_in=(int *)nco_free(msk_in);

  if(area_out) area_out=(double *)nco_free(area_out);
  if(frc_out) frc_out=(double *)nco_free(frc_out);
  if(msk_out) msk_out=(int *)nco_free(msk_out);

  if(lat_ctr_in) lat_ctr_in=(double *)nco_free(lat_ctr_in);
  if(lon_ctr_in) lon_ctr_in=(double *)nco_free(lon_ctr_in);
  if(lat_crn_in) lat_crn_in=(double *)nco_free(lat_crn_in);
  if(lon_crn_in) lon_crn_in=(double *)nco_free(lon_crn_in);
  if(lat_ctr_out) lat_ctr_out=(double *)nco_free(lat_ctr_out);
  if(lon_ctr_out) lon_ctr_out=(double *)nco_free(lon_ctr_out);
  if(lat_crn_out) lat_crn_out=(double *)nco_free(lat_crn_out);
  if(lon_crn_out) lon_crn_out=(double *)nco_free(lon_crn_out);

  if(wgt_raw) wgt_raw=(double *)nco_free(wgt_raw);
  if(col_src_adr) col_src_adr=(int *)nco_free(col_src_adr);
  if(row_dst_adr) row_dst_adr=(int *)nco_free(row_dst_adr);

  if(fl_in_dst) fl_in_dst=(char *)nco_free(fl_in_dst);
  if(fl_in_src) fl_in_src=(char *)nco_free(fl_in_src);

  return rcd;
} /* !nco_map_mk() */

int /* O [enm] Return code */
nco_msh_mk /* [fnc] Compute overlap mesh and weights */
(rgr_sct * const rgr, /* I [sct] Regridding structure */

 double *area_in, /* I [sr] Area of source grid */
 int *msk_in, /* I [flg] Mask on source grid */
 double *lat_ctr_in, /* I [dgr] Latitude  centers of source grid */
 double *lon_ctr_in, /* I [dgr] Longitude centers of source grid */
 double *lat_crn_in, /* I [dgr] Latitude  corners of source grid */
 double *lon_crn_in, /* I [dgr] Longitude corners of source grid */
 size_t grd_sz_in, /* I [nbr] Number of elements in single layer of source grid */
 long grd_crn_nbr_in, /* I [nbr] Maximum number of corners in source gridcell */

 double *area_out, /* I [sr] Area of destination grid */
 int *msk_out, /* I [flg] Mask on destination grid */
 double *lat_ctr_out, /* I [dgr] Latitude  centers of destination grid */
 double *lon_ctr_out, /* I [dgr] Longitude centers of destination grid */
 double *lat_crn_out, /* I [dgr] Latitude  corners of destination grid */
 double *lon_crn_out, /* I [dgr] Longitude corners of destination grid */
 size_t grd_sz_out, /* I [nbr] Number of elements in single layer of destination grid */
 long grd_crn_nbr_out, /* I [nbr] Maximum number of corners in destination gridcell */

 double *frc_in, /* O [frc] Fraction of source grid */
 double *frc_out, /* O [frc] Fraction of destination grid */
 int **col_src_adr_ptr, /* O [idx] Source address (col) */
 int **row_dst_adr_ptr, /* O [idx] Destination address (row) */
 double **wgt_raw_ptr, /* O [frc] Remapping weights */ 
 size_t *lnk_nbr_ptr) /* O [nbr] Number of links */
{
  /* Purpose: Compute overlap mesh and weights */
  const char fnc_nm[]="nco_msh_mk()";

  double *wgt_raw; /* [frc] Remapping weights */ 

  double *lat_crn_vrl=NULL; /* [dgr] Latitude  corners of overlap grid */
  double *lat_ctr_vrl=NULL_CEWI; /* [dgr] Latitude  centers of overlap grid */
  double *lon_crn_vrl=NULL; /* [dgr] Longitude corners of overlap grid */
  double *lon_ctr_vrl=NULL_CEWI; /* [dgr] Longitude centers of overlap grid */

  int *col_src_adr; /* [idx] Source address (col) */
  int *row_dst_adr; /* [idx] Destination address (row) */

  int rcd=NCO_NOERR;

  long idx; /* [idx] Counting index for unrolled grids */
  long grd_crn_nbr_vrl; /* [nbr] Maximum number of corners in overlap polygon */

  size_t lnk_nbr; /* [nbr] Number of links */
  //size_t lnk_idx; /* [idx] Link index */

  /* Construct overlap mesh here
     NB: Parallelize loop with OpenMP and/or MPI
     Final lnk_nbr and grd_crn_nbr_vrl are known only after a full loop through input grids */

  lnk_nbr=1L; /* [nbr] Number of overlap polygons */
  grd_crn_nbr_vrl=1L; /* [nbr] Maximum number of vertices in overlap polygon */
  
  lat_crn_vrl=(double *)nco_malloc_dbg(lnk_nbr*grd_crn_nbr_vrl*nco_typ_lng(NC_DOUBLE),fnc_nm,"Unable to malloc() value buffer for overlap latitude corners");
  lat_ctr_vrl=(double *)nco_malloc_dbg(lnk_nbr*nco_typ_lng(NC_DOUBLE),fnc_nm,"Unable to malloc() value buffer for overlap latitude centers");
  lon_crn_vrl=(double *)nco_malloc_dbg(lnk_nbr*grd_crn_nbr_vrl*nco_typ_lng(NC_DOUBLE),fnc_nm,"Unable to malloc() value buffer for overlap longitude corners");
  lon_ctr_vrl=(double *)nco_malloc_dbg(lnk_nbr*nco_typ_lng(NC_DOUBLE),fnc_nm,"Unable to malloc() value buffer for overlap longitude centers");

  wgt_raw=(double *)nco_malloc_dbg(lnk_nbr*nco_typ_lng(NC_DOUBLE),fnc_nm,"Unable to malloc() value buffer for remapping weights");
  col_src_adr=(int *)nco_malloc_dbg(lnk_nbr*nco_typ_lng(NC_INT),fnc_nm,"Unable to malloc() value buffer for remapping addresses");
  row_dst_adr=(int *)nco_malloc_dbg(lnk_nbr*nco_typ_lng(NC_INT),fnc_nm,"Unable to malloc() value buffer for remapping addresses");

  /* Initialize arguments before they are actually computed */
  for(idx=0;idx<lnk_nbr;idx++) wgt_raw[idx]=0.0;
  for(idx=0;idx<lnk_nbr;idx++) col_src_adr[idx]=-1;
  for(idx=0;idx<lnk_nbr;idx++) row_dst_adr[idx]=-1;
  for(idx=0;idx<grd_sz_in;idx++) frc_in[idx]=0.0;
  for(idx=0;idx<grd_sz_out;idx++) frc_out[idx]=0.0;

  if(lat_crn_vrl) lat_crn_vrl=(double *)nco_free(lat_crn_vrl);
  if(lat_ctr_vrl) lat_ctr_vrl=(double *)nco_free(lat_ctr_vrl);
  if(lon_crn_vrl) lon_crn_vrl=(double *)nco_free(lon_crn_vrl);
  if(lon_ctr_vrl) lon_ctr_vrl=(double *)nco_free(lon_ctr_vrl);

  *wgt_raw_ptr=wgt_raw;
  *col_src_adr_ptr=col_src_adr;
  *row_dst_adr_ptr=row_dst_adr;
  *lnk_nbr_ptr=lnk_nbr;

  return rcd;
} /* !nco_msh_mk() */
