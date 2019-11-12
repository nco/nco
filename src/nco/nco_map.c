/* $Header$ */

/* Purpose: NCO map-generation utilities */

/* Copyright (C) 2017--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_map.h" /* Map generation */


static rgr_sct *map_rgr;

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

  int area_in_id=NC_MIN_INT; /* [id] Area variable ID */
  int frc_in_id; /* [id] Fraction variable ID */
  int msk_in_id=NC_MIN_INT; /* [id] Mask variable ID */
  int src_grd_crn_lat_id; /* [id] Source grid corner latitudes  variable ID */
  int src_grd_crn_lon_id; /* [id] Source grid corner longitudes variable ID */
  int src_grd_ctr_lat_id; /* [id] Source grid center latitudes  variable ID */
  int src_grd_ctr_lon_id; /* [id] Source grid center longitudes variable ID */

  int area_out_id=NC_MIN_INT; /* [id] Area variable ID */
  int frc_out_id; /* [id] Fraction variable ID */
  int msk_out_id=NC_MIN_INT; /* [id] Mask variable ID */
  int dst_grd_crn_lat_id; /* [id] Destination grid corner latitudes  variable ID */
  int dst_grd_crn_lon_id; /* [id] Destination grid corner longitudes variable ID */
  int dst_grd_ctr_lat_id; /* [id] Destination grid center latitudes  variable ID */
  int dst_grd_ctr_lon_id; /* [id] Destination grid center longitudes variable ID */

  long *dmn_cnt=NULL;
  long *dmn_srt=NULL;

  nco_bool FL_RTR_RMT_LCN_DST;
  nco_bool FL_RTR_RMT_LCN_SRC;
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */

  nco_mpf_sct mpf;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t idx; /* [idx] Counting index for unrolled grids */

  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s obtaining source grid from %s\n",nco_prg_nm_get(),fnc_nm,rgr->fl_grd_src);
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s obtaining destination grid from %s\n",nco_prg_nm_get(),fnc_nm,rgr->fl_grd_dst);

  /* Duplicate (because nco_fl_mk_lcl() free()'s fl_in) */
  fl_in_dst=(char *)strdup(rgr->fl_grd_dst);
  fl_in_src=(char *)strdup(rgr->fl_grd_src);
  /* Make sure file is on local system and is readable or die trying */
  fl_in_dst=nco_fl_mk_lcl(fl_in_dst,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN_DST);
  fl_in_src=nco_fl_mk_lcl(fl_in_src,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN_SRC);
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
  rcd=nco_inq_varid_flg(in_id_src,"grid_area",&area_in_id); /* ESMF: area_a */
  if(rcd != NC_NOERR && nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s reports source grid file \"%s\" is missing \"grid_area\" variable. Will calculate area.\n",nco_prg_nm_get(),fnc_nm,rgr->fl_grd_src);

  rcd+=nco_inq_varid(in_id_src,"grid_center_lon",&src_grd_ctr_lon_id); /* ESMF: xc_a */
  rcd+=nco_inq_varid(in_id_src,"grid_center_lat",&src_grd_ctr_lat_id); /* ESMF: yc_a */
  rcd+=nco_inq_varid(in_id_src,"grid_corner_lon",&src_grd_crn_lon_id); /* ESMF: xv_a */
  rcd+=nco_inq_varid(in_id_src,"grid_corner_lat",&src_grd_crn_lat_id); /* ESMF: yv_a */

  rcd=nco_inq_varid_flg(in_id_src,"grid_imask",&msk_in_id); /* ESMF: msk_a */

  rcd=nco_inq_varid_flg(in_id_dst,"grid_area",&area_out_id); /* ESMF: area_b */
  if(rcd != NC_NOERR && nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s. dest grid file \"%s\" is missing \"grid_area\" variable. Will calculate area's.\n",nco_prg_nm_get(),fnc_nm,rgr->fl_grd_dst);

  rcd+=nco_inq_varid(in_id_dst,"grid_center_lon",&dst_grd_ctr_lon_id); /* ESMF: xc_b */
  rcd+=nco_inq_varid(in_id_dst,"grid_center_lat",&dst_grd_ctr_lat_id); /* ESMF: yc_b */
  rcd+=nco_inq_varid(in_id_dst,"grid_corner_lon",&dst_grd_crn_lon_id); /* ESMF: xv_b */
  rcd+=nco_inq_varid(in_id_dst,"grid_corner_lat",&dst_grd_crn_lat_id); /* ESMF: yv_b */
  rcd=nco_inq_varid_flg(in_id_dst,"grid_imask",&msk_out_id); /* ESMF: msk_b */

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

  /* 20191012: Always re-compute area so map-file area_a is consistent with weights */
  //  if(area_in_id == NC_MIN_INT)
  for(idx=0;idx<grd_sz_in;idx++) area_in[idx]=-1.0;

  /* if msk_in not in file the set each member to True */
  if(msk_in_id == NC_MIN_INT)
    for(idx=0;idx<grd_sz_in;idx++) msk_in[idx]=1;

  area_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  frc_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  msk_out=(int *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  lon_ctr_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  lat_ctr_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ));
  lon_crn_out=(double *)nco_malloc(mpf.dst_grid_corners*grd_sz_out*nco_typ_lng(crd_typ));
  lat_crn_out=(double *)nco_malloc(mpf.dst_grid_corners*grd_sz_out*nco_typ_lng(crd_typ));

  /* 20191012: Always re-compute area so map-file area_a is consistent with weights */
  // if(area_out_id == NC_MIN_INT)
  for(idx=0;idx<grd_sz_out;idx++) area_out[idx]=-1.0;

  /* if msk_in not in file the set each member to "True" */
  if(msk_out_id == NC_MIN_INT)
    for(idx=0;idx<grd_sz_out;idx++) msk_out[idx]=1;

  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_in;

  // if(area_in_id != NC_MIN_INT) rcd=nco_get_vara(in_id_src,area_in_id,dmn_srt,dmn_cnt,area_in,crd_typ);

  if(msk_in_id != NC_MIN_INT)
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

  // if(area_out_id != NC_MIN_INT) rcd=nco_get_vara(in_id_dst,area_out_id,dmn_srt,dmn_cnt,area_out,crd_typ);

  if(msk_out_id != NC_MIN_INT)
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
     This includes Mohammad Abouali code to create triangle list and Zender code to compute triangle areas */
  (void)nco_msh_mk
    (rgr, &mpf,
     area_in,msk_in,lat_ctr_in,lon_ctr_in,lat_crn_in,lon_crn_in,
     area_out,msk_out,lat_ctr_out,lon_ctr_out,lat_crn_out,lon_crn_out,
     frc_in,frc_out,&col_src_adr,&row_dst_adr,&wgt_raw,&lnk_nbr);

  if(False && nco_dbg_lvl_get() >= nco_dbg_quiet){
    /* 20191012: Normalize areas and weights to yield 4*pi steradians for global grids
       This worsens overall statistics, so do not turn-on by default
       However, may be useful for debugging so please leave in code */
    double area_out_ttl;
    double area_nrm_fct;
    area_out_ttl=0.0;
    for(size_t idx=0;idx<dst_grd_sz_nbr;idx++) area_out_ttl+=area_out[idx];
    area_nrm_fct=4*M_PI/area_out_ttl;
    if(fabs(area_nrm_fct-1.0) < 1.0e-13 && fabs(area_nrm_fct-1.0) > 1.0e-16){
      /* Output grid appears to be global so normalize by ratio of true-to-computed global area */
      if(nco_dbg_lvl_get() >= nco_dbg_quiet) (void)fprintf(stdout,"%s: INFO %s normalizing output area and weights by normalization ratio = %20.15e\n",nco_prg_nm_get(),fnc_nm,area_nrm_fct);
      for(size_t idx=0;idx<dst_grd_sz_nbr;idx++) area_out[idx]*=area_nrm_fct;
      for(size_t idx=0;idx<lnk_nbr;idx++) wgt_raw[idx]*=area_nrm_fct;
    } /* !fabs */
  } /* !dbg */

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

  /* Implement CMIP6 conventions in Taylor, Oehmke, Ullrich, Zender et al. (2019), "CMIP6 Specifications for Regridding Weights" https://docs.google.com/document/d/1BfVVsKAk9MAsOYstwFSWI2ZBt5mrO_Nmcu7rLGDuL08/edit */
  rcd=nco_char_att_put(out_id,NULL,"title","netCDF Operators (NCO) Offline Regridding Weight Generator");
  rcd=nco_char_att_put(out_id,NULL,"Conventions","NCAR-CSM");
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */
  rcd=nco_char_att_put(out_id,NULL,"created_by",usr_cpp);
  rcd=nco_char_att_put(out_id,NULL,"map_method","Conservative");
  rcd=nco_char_att_put(out_id,NULL,"weight_generator","NCO");
  const char vrs_cpp[]=TKN2SNG(NCO_VERSION); /* [sng] Version from C pre-processor */
  rcd=nco_char_att_put(out_id,NULL,"weight_generator_version",vrs_cpp);
  (void)nco_hst_att_cat(out_id,rgr->cmd_ln);
  (void)nco_vrs_att_cat(out_id);
  rcd=nco_char_att_put(out_id,"S","long_name","Weights to Map Variables from Source to Destination Grid");
  rcd=nco_char_att_put(out_id,"area_a","long_name","Solid Angle Subtended on Source Grid");
  rcd=nco_char_att_put(out_id,"area_a","standard_name","solid_angle");
  rcd=nco_char_att_put(out_id,"area_a","units","steradian");
  rcd=nco_char_att_put(out_id,"area_b","long_name","Solid Angle Subtended on Destination Grid");
  rcd=nco_char_att_put(out_id,"area_b","standard_name","solid_angle");
  rcd=nco_char_att_put(out_id,"area_b","units","steradian");
  rcd=nco_char_att_put(out_id,"col","long_name","Pointer to Source Grid Element (with 1, not 0, indicating the first element)");
  rcd=nco_char_att_put(out_id,"frac_a","long_name","Fraction of Source Gridcell That Participates in Remapping");
  rcd=nco_char_att_put(out_id,"frac_a","units","none");
  rcd=nco_char_att_put(out_id,"frac_b","long_name","Fraction of Destination Gridcell That Participates in Remapping");
  rcd=nco_char_att_put(out_id,"frac_b","units","none");
  rcd=nco_char_att_put(out_id,"mask_a","long_name","Binary Integer Mask for Source Grid");
  rcd=nco_char_att_put(out_id,"mask_a","units","none");
  rcd=nco_char_att_put(out_id,"mask_b","long_name","Binary Integer Mask for Destination Grid");
  rcd=nco_char_att_put(out_id,"mask_b","units","none");
  rcd=nco_char_att_put(out_id,"row","long_name","Pointer to Destination Grid Element (with 1, not 0, indicating the first element)");
  rcd=nco_char_att_put(out_id,"xc_a","long_name","Longitude of Source Grid Cell Centers");
  rcd=nco_char_att_put(out_id,"xc_a","standard_name","longitude");
  rcd=nco_char_att_put(out_id,"xc_a","units","degrees_east");
  rcd=nco_char_att_put(out_id,"xc_a","bounds","xv_a");
  rcd=nco_char_att_put(out_id,"xc_b","long_name","Longitude of Destination Grid Cell Centers");
  rcd=nco_char_att_put(out_id,"xc_b","standard_name","longitude");
  rcd=nco_char_att_put(out_id,"xc_b","units","degrees_east");
  rcd=nco_char_att_put(out_id,"xv_a","long_name","Longitude of Source Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,"xv_a","standard_name","longitude");
  rcd=nco_char_att_put(out_id,"xv_a","units","degrees_east");
  rcd=nco_char_att_put(out_id,"xv_b","long_name","Longitude of Destination Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,"xv_b","standard_name","longitude");
  rcd=nco_char_att_put(out_id,"xv_b","units","degrees_east");
  rcd=nco_char_att_put(out_id,"xc_b","bounds","xv_b");
  rcd=nco_char_att_put(out_id,"yc_a","long_name","Latitude of Source Grid Cell Centers");
  rcd=nco_char_att_put(out_id,"yc_a","standard_name","latitude");
  rcd=nco_char_att_put(out_id,"yc_a","units","degrees_north");
  rcd=nco_char_att_put(out_id,"yc_a","bounds","yv_a");
  rcd=nco_char_att_put(out_id,"yc_b","long_name","Latitude of Destination Grid Cell Centers");
  rcd=nco_char_att_put(out_id,"yc_b","standard_name","latitude");
  rcd=nco_char_att_put(out_id,"yc_b","units","degrees_north");
  rcd=nco_char_att_put(out_id,"yc_b","bounds","yv_b");
  rcd=nco_char_att_put(out_id,"yv_a","long_name","Latitude of Source Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,"yv_a","standard_name","latitude");
  rcd=nco_char_att_put(out_id,"yv_a","units","degrees_north");
  rcd=nco_char_att_put(out_id,"yv_b","long_name","Latitude of Destination Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,"yv_b","standard_name","latitude");
  rcd=nco_char_att_put(out_id,"yv_b","units","degrees_north");
  
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
  if(fl_out_tmp) fl_out_tmp=(char*)nco_free(fl_out_tmp);

  return rcd;
} /* !nco_map_mk() */


int /* O [enm] Return code */
nco_msh_mk /* [fnc] Compute overlap mesh and weights */
(rgr_sct * const rgr, /* I [sct] Regridding structure */
 nco_mpf_sct *mpf,
 double *area_in, /* I [sr] Area of source grid */
 int *msk_in, /* I [flg] Mask on source grid */
 double *lat_ctr_in, /* I [dgr] Latitude  centers of source grid */
 double *lon_ctr_in, /* I [dgr] Longitude centers of source grid */
 double *lat_crn_in, /* I [dgr] Latitude  corners of source grid */
 double *lon_crn_in, /* I [dgr] Longitude corners of source grid */
 //size_t grd_sz_in, /* I [nbr] Number of elements in single layer of source grid */
 //long grd_crn_nbr_in, /* I [nbr] Maximum number of corners in source gridcell */

 double *area_out, /* I [sr] Area of destination grid */
 int *msk_out, /* I [flg] Mask on destination grid */
 double *lat_ctr_out, /* I [dgr] Latitude  centers of destination grid */
 double *lon_ctr_out, /* I [dgr] Longitude centers of destination grid */
 double *lat_crn_out, /* I [dgr] Latitude  corners of destination grid */
 double *lon_crn_out, /* I [dgr] Longitude corners of destination grid */
 //size_t grd_sz_out, /* I [nbr] Number of elements in single layer of destination grid */
 //long grd_crn_nbr_out, /* I [nbr] Maximum number of corners in destination gridcell */

 double *frc_in, /* O [frc] Fraction of source grid */
 double *frc_out, /* O [frc] Fraction of destination grid */
 int **col_src_adr_ptr, /* O [idx] Source address (col) */
 int **row_dst_adr_ptr, /* O [idx] Destination address (row) */
 double **wgt_raw_ptr, /* O [frc] Remapping weights */
 size_t *lnk_nbr_ptr) /* O [nbr] Number of links */
{
  /* Purpose: Compute overlap mesh and weights */
  const char fnc_nm[]="nco_msh_mk()";

  /* set static variable */
  map_rgr=rgr;

  double *wgt_raw; /* [frc] Remapping weights */

  int pl_cnt_in=0;
  int pl_cnt_out=0;


  long grd_crn_nbr_in;    /* [nbr] Maximum number of corners in source gridcell */
  long grd_crn_nbr_out;  /*  [nbr] Maximum number of corners in destination gridcell */
  size_t grd_sz_in;       /* [nbr] Number of elements in single layer of source grid */
  size_t grd_sz_out;     /*  [nbr] Number of elements in single layer of destination grid */

  int *col_src_adr; /* [idx] Source address (col) */
  int *row_dst_adr; /* [idx] Destination address (row) */

  nco_grd_lon_typ_enm grd_lon_typ_in=nco_grd_lon_nil;
  nco_grd_lon_typ_enm grd_lon_typ_out=nco_grd_lon_nil;

  int rcd=NCO_NOERR;

  long grd_crn_nbr_vrl; /* [nbr] Maximum number of corners in overlap polygon */

  size_t idx; /* [idx] Counting index for unrolled grids */
  size_t lnk_nbr; /* [nbr] Number of links */
  //size_t lnk_idx; /* [idx] Link index */

  int pl_cnt_vrl=0;


  poly_sct *pl_glb_in=NULL_CEWI;
  poly_sct *pl_glb_out=NULL_CEWI;

  poly_sct **pl_lst_in=NULL_CEWI;
  poly_sct **pl_lst_out=NULL_CEWI;

  poly_sct **pl_lst_vrl=(poly_sct**)NULL_CEWI;

  poly_typ_enm pl_typ=poly_none  ;

  KDTree *rtree=NULL_CEWI;


  grd_crn_nbr_in=mpf->src_grid_corners;
  grd_crn_nbr_out=mpf->dst_grid_corners;

  grd_sz_in=mpf->src_grid_size;
  grd_sz_out=mpf->dst_grid_size;


  /* Construct overlap mesh here
     NB: Parallelize loop with OpenMP and/or MPI
     Final lnk_nbr and grd_crn_nbr_vrl are known only after a full loop through input grids */

  /* choose mesh overlap type based on rank of src and dst */
  if(mpf->src_grid_rank==2 && mpf->dst_grid_rank==2)
    pl_typ=poly_rll;
  else
    pl_typ=poly_sph;

  if(nco_dbg_lvl_get() >= nco_dbg_crr)
     (void)fprintf(stderr,"%s:%s(): Interpolation type=%s\n",nco_prg_nm_get(),fnc_nm, nco_poly_typ_sng_get(pl_typ)  );

  /* create some statistics on grid in and grid out */
  pl_glb_in=nco_msh_stats(area_in,msk_in,lat_ctr_in, lon_ctr_in, lat_crn_in, lon_crn_in,grd_sz_in, grd_crn_nbr_in);
  pl_glb_out=nco_msh_stats(area_out,msk_out,lat_ctr_out, lon_ctr_out, lat_crn_out, lon_crn_out,grd_sz_out, grd_crn_nbr_out);

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s:%s mesh in statistics\n",nco_prg_nm_get(),fnc_nm);
    nco_poly_prn(pl_glb_in,0);

    (void)fprintf(stderr,"\n%s:%s mesh out statistics\n",nco_prg_nm_get(),fnc_nm);
    nco_poly_prn(pl_glb_out,0);
  }

  /* determin lon_typ  nb temporary */
  grd_lon_typ_in=nco_poly_minmax_2_lon_typ(pl_glb_in);
  grd_lon_typ_out=nco_poly_minmax_2_lon_typ(pl_glb_out);

  /* run though just to check - some (0-360) SCRIP files have a negative lon for wrapped cells ??*/
  nco_msh_lon_crr(lon_crn_in,grd_sz_in,grd_crn_nbr_in, grd_lon_typ_in, grd_lon_typ_in );
  nco_msh_lon_crr(lon_crn_out,grd_sz_out,grd_crn_nbr_out, grd_lon_typ_out, grd_lon_typ_out );

  /* now convert corners */
  nco_msh_lon_crr(lon_crn_in,grd_sz_in,grd_crn_nbr_in, grd_lon_typ_in, grd_lon_typ_out);
  /* now convert centers */
  nco_msh_lon_crr(lon_ctr_in,grd_sz_in,1 , grd_lon_typ_in, grd_lon_typ_out);

  //nco_msh_wrt("nco_map_tst_in.nc", grd_sz_in, grd_crn_nbr_in, lat_crn_in, lon_crn_in);
  //nco_msh_wrt("nco_map_tst_out.nc", grd_sz_out, grd_crn_nbr_out, lat_crn_out, lon_crn_out);

  //  test nco_poly functions
  {

    switch(grd_lon_typ_out){

      case nco_grd_lon_nil:
      case nco_grd_lon_unk:
      case nco_grd_lon_bb:
        if(pl_typ == poly_crt)
          nco_crt_set_domain(0.0, 360.0, -90.0, 90.0);
        else if(pl_typ == poly_sph || pl_typ == poly_rll )
          nco_sph_set_domain(0.0, 2.0 * M_PI, -M_PI_2, M_PI_2);

        break;


      case nco_grd_lon_180_ctr:
      case nco_grd_lon_180_wst:
        if(pl_typ == poly_crt)
          nco_crt_set_domain(-180.0, 180.0, -90.0, 90.0);
        else if(pl_typ == poly_sph || pl_typ == poly_rll)
          nco_sph_set_domain(-M_PI, M_PI, -M_PI_2, M_PI_2);

        break;


      case nco_grd_lon_Grn_ctr:
      case nco_grd_lon_Grn_wst:
        if(pl_typ == poly_crt)
          nco_crt_set_domain(0.0, 360.0, -90.0, 90.0);
        else if(pl_typ == poly_sph || pl_typ == poly_rll)
          nco_sph_set_domain(0.0, 2.0 * M_PI, -M_PI_2, M_PI_2);

        break;
    }

    if(pl_typ==poly_sph )
      pl_lst_out = nco_poly_lst_mk_sph(area_out, msk_out, lat_ctr_out, lon_ctr_out, lat_crn_out, lon_crn_out, grd_sz_out, (size_t) grd_crn_nbr_out, grd_lon_typ_out);
    else if(pl_typ==poly_rll)
      pl_lst_out = nco_poly_lst_mk_rll(area_out, msk_out, lat_ctr_out, lon_ctr_out, lat_crn_out, lon_crn_out, grd_sz_out, (size_t) grd_crn_nbr_out, grd_lon_typ_out);

    pl_cnt_out=grd_sz_out;


    if(pl_typ==poly_sph)
      pl_lst_in = nco_poly_lst_mk_sph(area_in, msk_in, lat_ctr_in, lon_ctr_in, lat_crn_in, lon_crn_in, grd_sz_in,(size_t) grd_crn_nbr_in, grd_lon_typ_out);
    else if(pl_typ==poly_rll)
      pl_lst_in = nco_poly_lst_mk_rll(area_in, msk_in, lat_ctr_in, lon_ctr_in, lat_crn_in, lon_crn_in, grd_sz_in,(size_t) grd_crn_nbr_in, grd_lon_typ_out);

    pl_cnt_in=grd_sz_in;
    /* test new write func */
    // 20190526: Stop writing this file (the output grid) by default.
    //    nco_msh_poly_lst_wrt("nco_map_tst_out.nc", pl_lst_out, pl_cnt_out, grd_lon_typ_out  );

    /* debug output all
    for(idx=0;idx<pl_cnt_out  ;idx++)
      nco_poly_prn(pl_lst_out[idx],1);
    */

    /* create tree */
    rtree=kd_create();
    {
      nco_bool bSplit = False;

      kd_box size1;
      kd_box size2;

      KDElem* my_elem1=NULL_CEWI;
      KDElem* my_elem2=NULL_CEWI;

      for (idx = 0; idx < pl_cnt_out; idx++) {

        if(pl_lst_out[idx]->bmsk == False)
          continue;

        my_elem1 = (KDElem *) nco_calloc((size_t) 1, sizeof(KDElem));

        /* kd tree cannot handle wrapped coordinates so split minmax if needed*/
        bSplit = nco_poly_minmax_split(pl_lst_out[idx], grd_lon_typ_out, size1, size2);

        kd_insert(rtree, (kd_generic) pl_lst_out[idx], size1, (char *) my_elem1);

        if (bSplit) {
          my_elem2 = (KDElem *) nco_calloc((size_t) 1, sizeof(KDElem));
          kd_insert(rtree, (kd_generic) pl_lst_out[idx], size2, (char *) my_elem2);
        }

      }


    }

    /* call the overlap routine */
    if(pl_cnt_in && pl_cnt_out){
      /* temporarily disable crt code */
      /*  if(pl_typ == poly_crt) pl_lst_vrl=nco_poly_lst_mk_vrl(pl_lst_in, pl_cnt_in, rtree, &pl_cnt_vrl); */

      if(pl_typ == poly_sph || pl_typ == poly_rll) pl_lst_vrl=nco_poly_lst_mk_vrl_sph(pl_lst_in, grd_sz_in , grd_lon_typ_out, rtree, &pl_cnt_vrl);
    } /* !pl_cnt_in */

    if(nco_dbg_lvl_get() >= nco_dbg_dev)
      fprintf(stderr, "%s: INFO: num input polygons=%lu, num output polygons=%lu num overlap polygons=%d\n", nco_prg_nm_get(),grd_sz_in, grd_sz_out , pl_cnt_vrl);

  }


  /* fnd max crn_nbr from list of overlap polygons */
  grd_crn_nbr_vrl=0;
  for(idx=0;idx < pl_cnt_vrl;idx++)
    if( pl_lst_vrl[idx]->crn_nbr > grd_crn_nbr_vrl )
      grd_crn_nbr_vrl=pl_lst_vrl[idx]->crn_nbr;

  lnk_nbr=pl_cnt_vrl;

  wgt_raw=(double *)nco_malloc(lnk_nbr*nco_typ_lng(NC_DOUBLE));
  col_src_adr=(int *)nco_malloc(lnk_nbr*nco_typ_lng(NC_INT));
  row_dst_adr=(int *)nco_malloc(lnk_nbr*nco_typ_lng(NC_INT));


  /*
  for(idx=0;idx<lnk_nbr;idx++) wgt_raw[idx]=pl_lst_vrl[idx]->wgt;
  for(idx=0;idx<lnk_nbr;idx++) col_src_adr[idx]=pl_lst_vrl[idx]->src_id+1;
  for(idx=0;idx<lnk_nbr;idx++) row_dst_adr[idx]=pl_lst_vrl[idx]->dst_id+1;
  */

  for(idx=0; idx<lnk_nbr;idx++ )
  {
    wgt_raw[idx]=pl_lst_vrl[idx]->wgt;
    col_src_adr[idx]=pl_lst_vrl[idx]->src_id+1;
    row_dst_adr[idx]=pl_lst_vrl[idx]->dst_id+1;

  }
  
  for(idx=0;idx<grd_sz_in;idx++) 
  {
    if( pl_lst_in[idx]->wgt >0.0  ) {
      frc_in[idx] = pl_lst_in[idx]->wgt;
      msk_in[idx] = 1;
    }else{
      frc_in[idx]=0.0;
      msk_in[idx]=0;
    }
    
  }  
  

  for(idx=0;idx<grd_sz_out;idx++) {

    if( pl_lst_out[idx]->wgt >0.0  ) {
      frc_out[idx] = pl_lst_out[idx]->wgt;
      msk_out[idx] = 1;
    }else{
      frc_out[idx]=0.0;
      msk_out[idx]=0;
    }


  }  
    

  /* Write-out overlap mesh
     20190526: Allow users to name and output mesh-file with --rgr msh_fl=msh.nc. https://github.com/nco/nco/issues/135 */
  if(rgr->fl_msh) nco_msh_poly_lst_wrt(rgr->fl_msh,pl_lst_vrl,pl_cnt_vrl,grd_lon_typ_out);

  *wgt_raw_ptr=wgt_raw;
  *col_src_adr_ptr=col_src_adr;
  *row_dst_adr_ptr=row_dst_adr;
  *lnk_nbr_ptr=lnk_nbr;

  /* tally up weights see if they sum to number of dst grid cells */
  if( nco_dbg_lvl_get() >= nco_dbg_dev)
  {
    size_t irow;
    size_t sz;
    double sum=0.0;
    double *tally;

    sz=lnk_nbr;

    tally=(double*)nco_malloc( sizeof(double)*sz);
    for(idx=0;idx<sz;idx++)
      tally[idx]=0.0;



    for(idx=0;idx<sz;idx++)
    {
      irow=row_dst_adr[idx] - 1;

      if( irow < sz )
         tally[irow] += wgt_raw[idx];

      sum+=wgt_raw[idx];
    }

    fprintf(stderr, "%s(): S.total=%.10f  WARNING following is list of incomplete dst cells, by src_id no\n", fnc_nm,sum);
    for(idx=0; idx< pl_cnt_out;idx++)
      if(  fabs(tally[idx]-1.0) >1e-8  )
        fprintf(stderr,"%lu(%.20f)\n", idx, tally[idx]);

    // fprintf(stderr,"\n sum=%.20f\n", idx, sum);

    tally=(double*)nco_free(tally);

  }

  if(nco_dbg_lvl_get() >= nco_dbg_dev)
  {
    int io_flg=1;
    int pl_nbr=0;

    poly_sct **pl_lst_dbg=NULL_CEWI;


    /* find area mismatch between dst and overlap */
    fprintf(stderr,"%s(): Comparing dst areas with overlap areas\n",fnc_nm);
    pl_lst_dbg=nco_poly_lst_chk_dbg(pl_lst_out, grd_sz_out, pl_lst_vrl, pl_cnt_vrl, io_flg, &pl_nbr);

    if(pl_nbr) {
      nco_msh_poly_lst_wrt("nco_map_tst_out_dbg.nc", pl_lst_dbg, pl_nbr, grd_lon_typ_out);
      pl_lst_dbg=nco_poly_lst_free(pl_lst_dbg, pl_nbr);
    }


    pl_nbr=0;
    io_flg=0;

    /* find area mismatch between src and overlap */
    fprintf(stderr,"%s(): Comparing src areas with overlap areas\n",fnc_nm);
    pl_lst_dbg=nco_poly_lst_chk_dbg(pl_lst_in, grd_sz_in, pl_lst_vrl, pl_cnt_vrl, io_flg, &pl_nbr);

    if(pl_nbr) {
      nco_msh_poly_lst_wrt("nco_map_tst_in_dbg.nc", pl_lst_dbg, pl_nbr, grd_lon_typ_out);
      pl_lst_dbg=nco_poly_lst_free(pl_lst_dbg, pl_nbr);
    }


  }

  /* destroy kdtree */
  if(rtree)
     kd_destroy(rtree,NULL);

  pl_glb_in=nco_poly_free(pl_glb_in);
  pl_glb_out=nco_poly_free(pl_glb_out);

  if(grd_sz_in)
     pl_lst_in=nco_poly_lst_free(pl_lst_in,grd_sz_in);

  if(grd_sz_out)
     pl_lst_out=nco_poly_lst_free(pl_lst_out,grd_sz_out);



  if(pl_cnt_vrl)
    pl_lst_vrl=nco_poly_lst_free(pl_lst_vrl,pl_cnt_vrl);


  return rcd;
} /* !nco_msh_mk() */


int
nco_msh_wrt
(const  char *fl_out,
 size_t grd_sz_nbr,
 size_t grd_crn_nbr,
 double *lat_crn,
 double *lon_crn)
{
  const char fnc_nm[]="nco_grd_mk()"; /* [sng] Function name */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_1D=1; /* [nbr] Rank of 2-D grid variables */

  int rcd;
  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */

  int out_id; /* I [id] Output netCDF file ID */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_out_fmt=NC_FORMAT_CLASSIC; /* [enm] Output file format */
  int dmn_ids[2]; /* [id] Dimension IDs array for output variable */
  int dmn_id_grd_crn; /* [id] Grid corners dimension ID */
  int dmn_id_grd_sz; /* [id] Grid size dimension ID */
  int grd_crn_lat_id; /* [id] Grid corner latitudes  variable ID */
  int grd_crn_lon_id; /* [id] Grid corner longitudes variable ID */
  int grd_area_id; /* [id] Grid corner longitudes variable ID */


  double *area=NULL_CEWI;
  double *grd_ctr_lat=NULL_CEWI;
  double *grd_ctr_lon=NULL_CEWI;


  long dmn_srt[2];
  long dmn_cnt[2];

  const nc_type crd_typ=NC_DOUBLE;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  char *fl_out_tmp=NULL_CEWI;

  char grd_area_nm[]="grid_area"; /* 20150830: NB ESMF_RegridWeightGen --user_areas looks for variable named "grid_area" */
  char dmn_sz_nm[]="grid_dims";
  char grd_crn_lat_nm[]="grid_corner_lat";
  char grd_crn_lon_nm[]="grid_corner_lon";
  char grd_crn_nm[]="grid_corners";
  char grd_ctr_lat_nm[]="grid_center_lat";
  char grd_ctr_lon_nm[]="grid_center_lon";
  char grd_rnk_nm[]="grid_rank";
  char grd_sz_nm[]="grid_size";
  char msk_nm[]="grid_imask";

  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */


  area=(double*)nco_malloc( sizeof(double) * grd_sz_nbr);
  grd_ctr_lat=(double*)nco_malloc( sizeof(double) * grd_sz_nbr);
  grd_ctr_lon=(double*)nco_malloc( sizeof(double) * grd_sz_nbr);

  nco_msh_plg_area(lat_crn, lon_crn, grd_sz_nbr, grd_crn_nbr, area);

  /* Open grid file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);

  /* Define dimensions */
  rcd=nco_def_dim(out_id,grd_crn_nm,grd_crn_nbr,&dmn_id_grd_crn);
  rcd=nco_def_dim(out_id,grd_sz_nm,grd_sz_nbr,&dmn_id_grd_sz);

  dmn_ids[0]=dmn_id_grd_sz;
  dmn_ids[1]=dmn_id_grd_crn;


  deflate=(int)True;
  shuffle=NC_SHUFFLE;

  /* Define variables */


  (void)nco_def_var(out_id,grd_crn_lat_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lat_id,shuffle,deflate,dfl_lvl);

  (void)nco_def_var(out_id,grd_crn_lon_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lon_id,shuffle,deflate,dfl_lvl);

  (void)nco_def_var(out_id,grd_area_nm, crd_typ,dmn_nbr_1D, dmn_ids, &grd_area_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_area_id, shuffle, deflate, dfl_lvl);



  /* Begin data mode */
  (void)nco_enddef(out_id);

  dmn_srt[0]=0L;
  dmn_srt[1]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  dmn_cnt[1]=grd_crn_nbr;

  rcd=nco_put_vara(out_id,grd_crn_lat_id,dmn_srt,dmn_cnt,lat_crn,crd_typ);
  rcd=nco_put_vara(out_id,grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn,crd_typ);
  rcd=nco_put_vara(out_id,grd_area_id,dmn_srt,dmn_cnt, area, crd_typ);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);


  area=(double*)nco_free(area);
  grd_ctr_lat=(double*)nco_free(grd_ctr_lat);

  return True;

}

poly_sct *           /* return a ply_sct with lat/lon minmax and total area */
nco_msh_stats
(double *area,       /* I [sr] Area of  grid */
 int *msk,           /* I [flg] Mask on  grid */
 double *lat_ctr,    /* I [dgr] Latitude  centers of  grid */
 double *lon_ctr,    /* I [dgr] Longitude centers of  grid */
 double *lat_crn,    /* I [dgr] Latitude  corners of  grid */
 double *lon_crn,    /* I [dgr] Longitude corners of  grid */
 size_t grd_sz,      /* I [nbr] Number of elements in single layer of  grid */
 long grd_crn_nbr){



 size_t idx;
 long jdx;
 long ixt;

 int umsk_cnt=0;

 double tot_area=0.0;

 poly_sct *pl;

 pl=nco_poly_init_crn(poly_crt, 4, -1);

 pl->dp_x_minmax[0]=DBL_MAX;
 pl->dp_x_minmax[1]=-DBL_MAX;

 pl->dp_y_minmax[0]=DBL_MAX;
 pl->dp_y_minmax[1]=-DBL_MAX;



  for(idx=0; idx< grd_sz;idx++ ) {
    if(msk[idx]) {
      tot_area += area[idx];
      umsk_cnt++;
    }

    for (jdx = 0; jdx < grd_crn_nbr; jdx++)
    {
      ixt=idx*grd_crn_nbr + jdx;

      /* lon min max */
      if(lon_crn[ixt]  < pl->dp_x_minmax[0]  )
        pl->dp_x_minmax[0]=lon_crn[ixt];
      else if(lon_crn[ixt] > pl->dp_x_minmax[1])
        pl->dp_x_minmax[1]=lon_crn[ixt];

      /* lat min max */
      if(lat_crn[ixt]  < pl->dp_y_minmax[0]  )
        pl->dp_y_minmax[0]=lat_crn[ixt];
      else if(lat_crn[ixt] > pl->dp_y_minmax[1])
        pl->dp_y_minmax[1]=lat_crn[ixt];


    }

 }

 /* all masked return NULL */
 if(umsk_cnt==0)
 {
   pl=nco_poly_free(pl);
   return pl;
 }


 pl->area=tot_area;
  nco_poly_minmax_use_crn(pl);


 return pl;
}

void
nco_msh_lon_crr(
double *lon_crn,      /* I/O longitude to be corrected */
size_t grd_sz,     /* I [nbr] Number of elements in single layer of source grid */
long grd_crn_nbr,  /* I [nbr] Maximum number of corners in source gridcell */
nco_grd_lon_typ_enm typ_in,
nco_grd_lon_typ_enm typ_out)
{
  const char fnc_nm[]="nco_msh_lon_crr()";

  int idx;
  int sz;

  const char * typ_in_sng;
  const char * typ_out_sng;

  sz=grd_sz*grd_crn_nbr;

  /* do nothing */
  if(typ_in== nco_grd_lon_nil || typ_out == nco_grd_lon_nil) return;

  typ_in_sng=nco_grd_lon_sng(typ_in);
  typ_out_sng=nco_grd_lon_sng(typ_out);

  /* do nothing
  if(typ_in == typ_out)
    return;
  */

  /* Check type out */
  if(typ_out == nco_grd_lon_bb || typ_out == nco_grd_lon_unk){
    (void)fprintf(stderr,"%s(): ERROR %s cannot convert grd_lon to \"%s\"\n",nco_prg_nm_get(),fnc_nm,typ_out_sng);
    exit(EXIT_FAILURE);
  }

  if(False && nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s converting lon coord from \"%s\" to \"%s\"\n",nco_prg_nm_get(),fnc_nm,typ_in_sng,typ_out_sng);

  switch(typ_in){
  case nco_grd_lon_unk:
  case nco_grd_lon_bb:
    switch(typ_out){
    case nco_grd_lon_180_wst:
    case nco_grd_lon_180_ctr:
      for(idx=0;idx<sz;idx++)
	if(lon_crn[idx] >180.0  ) lon_crn[idx]-=360.0;
      break;
    case nco_grd_lon_Grn_wst:
    case nco_grd_lon_Grn_ctr:
    default:
      for(idx=0;idx<sz;idx++)
	if(lon_crn[idx] <0.0  ) lon_crn[idx]+=360.0;
      break;
    }
    break;
  case nco_grd_lon_180_wst:
  case nco_grd_lon_180_ctr:
    switch(typ_out){
    case nco_grd_lon_180_wst:
    case nco_grd_lon_180_ctr:
      for(idx=0;idx<sz;idx++)
	if(lon_crn[idx] >180.0) lon_crn[idx]-=360.0;
	else if(lon_crn[idx] < -180.0 ) lon_crn[idx]+=360.0;
      break;
    case nco_grd_lon_Grn_wst:
    case nco_grd_lon_Grn_ctr:
    default:
      for(idx=0;idx<sz;idx++)
	if(lon_crn[idx] <0.0  ) lon_crn[idx]+=360.0;
      break;
    }
    break;
  case nco_grd_lon_Grn_wst:
  case nco_grd_lon_Grn_ctr:

    switch(typ_out){
    case nco_grd_lon_180_wst:
    case nco_grd_lon_180_ctr:
      for(idx=0;idx<sz;idx++)
	if(lon_crn[idx] >180.0)
	  lon_crn[idx]-=360.0;
      break;
      /* additional check for lon<0 as some generated  grids have this horrible wrapping */
    case nco_grd_lon_Grn_wst:
    case nco_grd_lon_Grn_ctr:
    default:
      for(idx=0;idx<sz;idx++)
	if(lon_crn[idx] < 0.0) lon_crn[idx]+=360.0;
      break;
    }
    break;

  case nco_grd_lon_nil:
    break;
  }
  return;
}

void
nco_msh_poly_lst_wrt
(const char *fl_out,
poly_sct ** pl_lst,
int pl_nbr,
nco_grd_lon_typ_enm grd_lon_typ
)
{
  const char fnc_nm[]="nco_grd_mk()"; /* [sng] Function name */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_1D=1; /* [nbr] Rank of 2-D grid variables */

  int rcd;
  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */

  int out_id; /* I [id] Output netCDF file ID */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_out_fmt=NC_FORMAT_CLASSIC; /* [enm] Output file format */
  int dmn_ids[3]; /* [id] Dimension IDs array for output variable */
  int grd_crn_lat_id; /* [id] Grid corner latitudes  variable ID */
  int grd_crn_lon_id; /* [id] Grid corner longitudes variable ID */
  int grd_ctr_lat_id; /* [id] Grid corner latitudes  variable ID */
  int grd_ctr_lon_id; /* [id] Grid corner longitudes variable ID */
  int grd_area_id; /* [id] Grid corner longitudes variable ID */
  int grd_msk_id;  /* [id] integer mask  ID */
  int grd_rnk_nbr; /* [nbr]   will be 1 in most cases */
  int grd_rnk_id; /* [nbr]   will be 1 in most cases */

  size_t grd_sz_nbr;
  size_t grd_crn_nbr;

  int *msk=NULL_CEWI;
  double *lat_crn=NULL_CEWI;
  double *lon_crn=NULL_CEWI;
  double *area=NULL_CEWI;
  double *lon_ctr=NULL_CEWI;
  double *lat_ctr=NULL_CEWI;


  long idx;
  long jdx;

  const nc_type crd_typ=NC_DOUBLE;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  char *fl_out_tmp=NULL_CEWI;

  char grd_area_nm[]="grid_area"; /* 20150830: NB ESMF_RegridWeightGen --user_areas looks for variable named "grid_area" */
  char dmn_sz_nm[]="grid_dims";
  char grd_crn_lat_nm[]="grid_corner_lat";
  char grd_crn_lon_nm[]="grid_corner_lon";
  char grd_crn_nm[]="grid_corners";
  char grd_ctr_lat_nm[]="grid_center_lat";
  char grd_ctr_lon_nm[]="grid_center_lon";
  char grd_rnk_nm[]="grid_rank";
  char grd_sz_nm[]="grid_size";
  char msk_nm[]="grid_imask";

  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */

  grd_sz_nbr=pl_nbr;
  grd_crn_nbr=0;

  /* find grd_crn_nbr - max crn_nbr in list) */
  for(idx=0; idx<pl_nbr;idx++)
    if(pl_lst[idx]->crn_nbr > grd_crn_nbr)
       grd_crn_nbr=pl_lst[idx]->crn_nbr;


  area=(double*)nco_malloc( sizeof(double) * grd_sz_nbr);
  lat_ctr=(double*)nco_malloc( sizeof(double) * grd_sz_nbr);
  lon_ctr=(double*)nco_malloc( sizeof(double) * grd_sz_nbr);
  msk=(int*)nco_malloc( sizeof(int)*grd_sz_nbr);

  lat_crn=(double*)nco_malloc( sizeof(double) * grd_sz_nbr * grd_crn_nbr );
  lon_crn=(double*)nco_malloc( sizeof(double) * grd_sz_nbr * grd_crn_nbr );

   /* now populate above variables */
  for(idx=0; idx< grd_sz_nbr;idx++ ) {
    size_t lcl_crn_nbr = pl_lst[idx]->crn_nbr;
    double *lat_crn_ptr = lat_crn + idx * grd_crn_nbr;
    double *lon_crn_ptr = lon_crn + idx * grd_crn_nbr;

    area[idx] = pl_lst[idx]->area;
    lon_ctr[idx] = pl_lst[idx]->dp_x_ctr;
    lat_ctr[idx] = pl_lst[idx]->dp_y_ctr;
    msk[idx]=1;

    memcpy(lon_crn_ptr, pl_lst[idx]->dp_x, lcl_crn_nbr * sizeof(double));
    memcpy(lat_crn_ptr, pl_lst[idx]->dp_y, lcl_crn_nbr * sizeof(double));

    /* fill in remaining values with last values */
    if (lcl_crn_nbr < grd_crn_nbr)
      for (jdx = lcl_crn_nbr; jdx < grd_crn_nbr; jdx++) {
        lon_crn_ptr[jdx] = pl_lst[idx]->dp_x[lcl_crn_nbr - 1];
        lat_crn_ptr[jdx] = pl_lst[idx]->dp_y[lcl_crn_nbr - 1];
      }
  }

  /* Open grid file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);

  /* Define dimensions */
  rcd=nco_def_dim(out_id,grd_sz_nm,grd_sz_nbr, &dmn_ids[0]);
  rcd=nco_def_dim(out_id,grd_crn_nm,grd_crn_nbr, &dmn_ids[1]);
  grd_rnk_nbr=1;
  rcd=nco_def_dim(out_id,grd_rnk_nm,grd_rnk_nbr, &dmn_ids[2]);

  deflate=(int)True;
  shuffle=NC_SHUFFLE;

  /* Define variables */

  (void)nco_def_var(out_id,grd_crn_lat_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lat_id,shuffle,deflate,dfl_lvl);
  nco_msh_att_char(out_id, grd_crn_lat_id, grd_crn_lat_nm, "units", "degrees");

  (void)nco_def_var(out_id,grd_crn_lon_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lon_id,shuffle,deflate,dfl_lvl);
  nco_msh_att_char(out_id, grd_crn_lon_id, grd_crn_lon_nm, "units", "degrees");

  (void)nco_def_var(out_id,grd_area_nm, crd_typ,dmn_nbr_1D, dmn_ids, &grd_area_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_area_id, shuffle, deflate, dfl_lvl);
  nco_msh_att_char(out_id, grd_area_id, grd_area_nm, "units", "steradians");

  (void)nco_def_var(out_id, grd_ctr_lon_nm, crd_typ,dmn_nbr_1D, dmn_ids, &grd_ctr_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_ctr_lon_id, shuffle, deflate, dfl_lvl);
  nco_msh_att_char(out_id, grd_ctr_lon_id, grd_ctr_lon_nm, "units", "degrees");

  (void)nco_def_var(out_id, grd_ctr_lat_nm, crd_typ,dmn_nbr_1D, dmn_ids, &grd_ctr_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_ctr_lat_id, shuffle, deflate, dfl_lvl);
  nco_msh_att_char(out_id, grd_ctr_lat_id, grd_ctr_lat_nm, "units", "degrees");

  (void)nco_def_var(out_id, msk_nm, NC_INT, dmn_nbr_1D, dmn_ids, &grd_msk_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_msk_id, shuffle, deflate, dfl_lvl);

  (void)nco_def_var(out_id, "grid_dims", NC_INT, dmn_nbr_1D, &dmn_ids[2], &grd_rnk_id);

  /* Begin data mode */
  (void)nco_enddef(out_id);

  rcd=nco_put_var(out_id,grd_crn_lat_id,lat_crn,crd_typ);
  rcd=nco_put_var(out_id,grd_crn_lon_id,lon_crn,crd_typ);

  rcd=nco_put_var(out_id, grd_area_id, area, crd_typ);
  rcd=nco_put_var(out_id, grd_ctr_lon_id, lon_ctr, crd_typ);
  rcd=nco_put_var(out_id, grd_ctr_lat_id, lat_ctr, crd_typ);

  rcd=nco_put_var(out_id,grd_msk_id, msk, NC_INT);

  rcd=nco_put_var(out_id,grd_rnk_id,&grd_sz_nbr, NC_INT);


  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  fl_out_tmp=(char*)nco_free(fl_out_tmp);

  area=(double*)nco_free(area);
  lat_ctr=(double*)nco_free(lat_ctr);
  lat_crn=(double*)nco_free(lat_crn);

  lon_ctr=(double*)nco_free(lon_ctr);
  lon_crn=(double*)nco_free(lon_crn);
  msk=(int*)nco_free(msk);


}


int
nco_msh_att_char(
int out_id,
int var_id,
const char *var_nm,
const char *att_nm,
const char *att_val
){
  int iret;

  char *av;

  av=strdup(att_val);
  aed_sct aed_mtd;
  aed_mtd.att_nm=strdup(att_nm);
  aed_mtd.var_nm=strdup(var_nm);
  aed_mtd.id=var_id;
  aed_mtd.sz=strlen(av);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=av;
  aed_mtd.mode=aed_create;

  iret=nco_aed_prc(out_id,var_id,aed_mtd);

  aed_mtd.att_nm=(char*)nco_free(aed_mtd.att_nm);
  aed_mtd.var_nm=(char*)nco_free(aed_mtd.var_nm);
  av=(char*)nco_free(av);
  return iret;

}

void
nco_msh_plg_area /* [fnc] wrapper to nco_sph_plg_area() */
(const double * const lat_bnd, /* [dgr] Latitude  boundaries of rectangular grid */
 const double * const lon_bnd, /* [dgr] Longitude boundaries of rectangular grid */
 const long grd_sz_nbr, /* [nbr] Number of gridcells in grid */
 const int bnd_nbr, /* [nbr] Number of bounds in gridcell */
 double * const area_out) /* [sr] Gridcell area */
{

  nco_sph_plg_area(map_rgr, lat_bnd, lon_bnd, grd_sz_nbr, bnd_nbr, area_out);

  return;


}

nco_bool
nco_map_hst_mk
(var_sct* var_row,
int row_max,
int hst_ar[],
int hst_sz )
{


  int idx;
  int sz;
  int idx_row=0;
  int *row_bin=NULL_CEWI;


  (void)cast_void_nctype(NC_DOUBLE ,&(var_row->val));

  sz=var_row->sz;

  row_bin=(int*)nco_calloc( row_max+1, sizeof(int));

  /* row count  rember var_row is one based and so is row_bin*/
  for(idx=0; idx<sz; idx++ )
    if( (idx_row=var_row->val.ip[idx])<= row_max )
      row_bin[idx_row]++;

  /* histogram count - one based */
  for(idx=1; idx<=row_max;idx++) {
    idx_row = row_bin[idx];
      if (idx_row < hst_sz)
        hst_ar[idx_row]++;
      else
        hst_ar[hst_sz]++;
  }





  (void)cast_nctype_void(NC_INT,&(var_row->val));

  row_bin=(int*)nco_free(row_bin);

  return True;

}




var_sct *
nco_map_var_init(
int in_id,
const char *var_nm,
dmn_sct **dmn,
int dmn_in_nbr)
{
  var_sct *var;
  int rcd;
  int var_id;

  /* get var id */
  rcd = nco_inq_varid(in_id, var_nm  , &var_id);
  /* fill out dims */
  var = nco_var_fll(in_id, var_id, var_nm, dmn, dmn_in_nbr);
  /* read in data */
  (void) nco_var_get(in_id, var);

  return var;


}

void
nco_map_var_min_max_ttl(
var_sct *var,
double *min,
double *max,
double *ttl
){

  int idx=0;

  if(var->type==NC_DOUBLE){


     double dval=0.0;
     double min_dp=NC_MAX_DOUBLE;
     double max_dp=-(NC_MAX_DOUBLE);
     double ttl_dp=0.0;

    (void)cast_void_nctype(NC_DOUBLE ,&(var->val));

     for(idx=0; idx<var->sz; idx++ )
     {
       /* de-reference */
       dval=var->val.dp[idx];
       ttl_dp+=dval;

       if(dval<min_dp)
         min_dp=dval;

       if(dval>max_dp)
         max_dp=dval;

     }

    (void)cast_nctype_void(NC_DOUBLE ,&(var->val));

     *min=min_dp;
     *max=max_dp;
     *ttl=ttl_dp;

     return;
  }

  else if(var->type==NC_INT)
  {
    int dval=0;
    int min_dp=(NC_MAX_INT);
    int max_dp=(NC_MIN_INT);
    int ttl_dp=0;


    (void)cast_void_nctype(NC_INT,&(var->val));

    for(idx=0; idx<var->sz; idx++ )
    {
      /* de-reference */
      dval=var->val.ip[idx];
      ttl_dp+=dval;

      if(dval<min_dp)
        min_dp=dval;

      if(dval>max_dp)
        max_dp=dval;

    }

    *min=(double)min_dp;
    *max=(double)max_dp;
    *ttl=(double)ttl_dp;


    (void)cast_nctype_void(NC_INT,&(var->val));
    return;




  }

  return;


}



nco_bool
nco_map_rpt   /* print out stats report about map */
(const char *fl_in)
{
  char fnc_nm[]="nco_map_chk";

  int idx;
  int rcd;
  int in_id;
  int fl_in_fmt;
  int dmn_in_nbr;
  int var_id;
  int var_lst_nbr=0;
  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT;

  char dmn_nm[NC_MAX_NAME];
  dmn_sct **dmn_in=NULL_CEWI;


  var_sct **var_lst=NULL_CEWI;
  
  var_sct *var_area_a=NULL_CEWI;
  var_sct *var_frac_a=NULL_CEWI;
  var_sct *var_mask_a=NULL_CEWI;

  var_sct *var_area_b=NULL_CEWI;
  var_sct *var_frac_b=NULL_CEWI;
  var_sct *var_mask_b=NULL_CEWI;

  var_sct *var_col=NULL_CEWI;
  var_sct *var_row=NULL_CEWI;
  
  var_sct *var_S=NULL_CEWI;
  
  const char *var_lst_nm[]={ "area_a", "frac_a", "mask_a",  "area_b", "frac_b", "mask_b", "col", "row", "S" };




  rcd=nco_fl_open(fl_in,NC_NOWRITE,&bfr_sz_hnt,&in_id);
  (void)nco_inq_format(in_id,&fl_in_fmt);


  /* read in all dims from file */
  (void)nco_inq(in_id,&dmn_in_nbr,(int *)NULL,(int *)NULL,(int *)NULL);
  dmn_in=(dmn_sct **)nco_malloc(dmn_in_nbr*sizeof(dmn_sct *));

  for(idx=0;idx<dmn_in_nbr;idx++)
  {
    (void)nco_inq_dimname(in_id,idx,dmn_nm);
    dmn_in[idx] = nco_dmn_fll(in_id, idx, dmn_nm);
  }


  var_area_a= nco_map_var_init(in_id, "area_a", dmn_in, dmn_in_nbr);
  var_frac_a= nco_map_var_init(in_id, "frac_a", dmn_in, dmn_in_nbr);
  var_mask_a= nco_map_var_init(in_id, "mask_a", dmn_in, dmn_in_nbr);

  var_area_b= nco_map_var_init(in_id, "area_b", dmn_in, dmn_in_nbr);
  var_frac_b= nco_map_var_init(in_id, "frac_b", dmn_in, dmn_in_nbr);
  var_mask_b= nco_map_var_init(in_id, "mask_b", dmn_in, dmn_in_nbr);

  var_col= nco_map_var_init(in_id, "col", dmn_in, dmn_in_nbr);
  var_row= nco_map_var_init(in_id, "row", dmn_in, dmn_in_nbr);
  var_S= nco_map_var_init(in_id, "S", dmn_in, dmn_in_nbr);

  /* do some type conversions */
  if(var_area_a->type !=NC_DOUBLE)
    var_area_a=nco_var_cnf_typ(NC_DOUBLE, var_area_a);

  if(var_frac_a->type !=NC_DOUBLE)
    var_frac_a=nco_var_cnf_typ(NC_DOUBLE, var_frac_a);

  if(var_mask_a->type !=NC_INT)
    var_mask_a=nco_var_cnf_typ(NC_INT, var_mask_a);

  if(var_area_b->type !=NC_DOUBLE)
    var_area_b=nco_var_cnf_typ(NC_DOUBLE, var_area_b);

  if(var_frac_b->type !=NC_DOUBLE)
    var_frac_b=nco_var_cnf_typ(NC_DOUBLE, var_frac_b);

  if(var_mask_b->type !=NC_INT)
    var_mask_b=nco_var_cnf_typ(NC_INT, var_mask_b);


  /* start report  in own scope */
  {
    double col_min, col_max, col_ttl;
    double row_min, row_max, row_ttl;
    
    double area_a_min, area_a_max, area_a_ttl;
    double area_b_min, area_b_max, area_b_ttl;

    double frac_a_min, frac_a_max, frac_a_ttl;
    double frac_b_min, frac_b_max, frac_b_ttl;


    double mask_a_min, mask_a_max, mask_a_ttl;
    double mask_b_min, mask_b_max, mask_b_ttl;
     

    fprintf(stdout, "sparse matrix size n_s=%lu\n", var_S->sz);

    nco_map_var_min_max_ttl(var_col,&col_min, &col_max, &col_ttl);
    nco_map_var_min_max_ttl(var_row,&row_min, &row_max, &row_ttl);

    fprintf(stdout,"n_a=%lu col index min/max = %.0f %.0f\n", var_mask_a->sz, col_min, col_max );
    fprintf(stdout,"n_b=%lu row index min/max = %.0f %.0f\n", var_mask_b->sz, row_min, row_max );

    nco_map_var_min_max_ttl(var_mask_a, &mask_a_min, &mask_a_max, &mask_a_ttl);
    nco_map_var_min_max_ttl(var_mask_b, &mask_b_min, &mask_b_max, &mask_b_ttl);

    fprintf(stdout, "grid a mask min/max: %.0f %.0f\n", mask_a_min, mask_a_max );
    fprintf(stdout, "grid b mask min/max: %.0f %.0f\n", mask_b_min, mask_b_max );


    nco_map_var_min_max_ttl(var_area_a, &area_a_min, &area_a_max, &area_a_ttl);
    nco_map_var_min_max_ttl(var_area_b, &area_b_min, &area_b_max, &area_b_ttl);

    fprintf(stdout, "area_a min/max= %.15g %.15g\n", area_a_min, area_a_max  );
    fprintf(stdout, "area_b min/max= %.15g %.15g\n", area_b_min, area_b_max );

    fprintf(stdout, "sum area_a weights / 4pi: %.15f\n", area_a_ttl / 4.0 / M_PI );
    fprintf(stdout, "sum area_b weights / 4pi: %.15f\n", area_b_ttl / 4.0 / M_PI );



    /* own scope for histogram */
    {
      int hst_sz=31;
      int *hst_row;
      int *hst_col;

      hst_row=(int*)nco_calloc(hst_sz+1, sizeof(int));
      hst_col=(int*)nco_calloc(hst_sz+1, sizeof(int));


      nco_map_hst_mk(var_col, var_mask_a->sz, hst_col, hst_sz);
      nco_map_hst_mk(var_row, var_mask_b->sz, hst_row, hst_sz);

      fprintf(stdout, "column 1: number of nonzero entries (histogram bin)\n");
      fprintf(stdout, "column 2: number of columns with that many nonzero entries\n");
      fprintf(stdout, "column 3: number of rows with that many nonzero entries\n");

      for(idx=0;idx<hst_sz;idx++)
        fprintf(stdout, "%4d %12d %12d\n",idx, hst_col[idx], hst_row[idx] );

      /* print final row (slighly modified */
      fprintf(stdout, "%4d+ %12d %12d\n",idx, hst_col[idx], hst_row[idx] );


       fprintf(stdout, "Number of ignored source points (empty columns): %d\n", hst_col[0]);
       fprintf(stdout, "Number of ignored source points (empty rows)   : %d\n", hst_row[0]);


       hst_row=(int*)nco_free(hst_row);
       hst_col=(int*)nco_free(hst_col);




    }



  }




  nco_close(in_id);

  var_area_a=nco_var_free(var_area_a);
  var_frac_a=nco_var_free(var_frac_a);
  var_mask_a=nco_var_free(var_mask_a);

  var_area_b=nco_var_free(var_area_b);
  var_frac_b=nco_var_free(var_frac_b);
  var_mask_b=nco_var_free(var_mask_b);

  var_col=nco_var_free(var_col);
  var_row=nco_var_free(var_row);
  var_S=nco_var_free(var_S);



  
  dmn_in=nco_dmn_lst_free(dmn_in,dmn_in_nbr );
  

  return True;

}

