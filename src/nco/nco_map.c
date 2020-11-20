/* $Header$ */

/* Purpose: NCO map-generation utilities */

/* Copyright (C) 2017--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_map.h" /* Map generation */

rgr_sct *map_rgr;

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
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=rgr->flg_uio; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=rgr->flg_uio; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */

  nco_mpf_sct mpf;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t idx; /* [idx] Counting index for unrolled grids */
  size_t hdr_pad=0UL; /* [B] Pad at end of header section */

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
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;

  rcd+=nco_fl_open(fl_in_src,md_open,&bfr_sz_hnt,&in_id_src);
  rcd+=nco_fl_open(fl_in_dst,md_open,&bfr_sz_hnt,&in_id_dst);

  rcd+=nco_inq_dimid(in_id_src,"grid_corners",&src_grid_corners_id);
  rcd+=nco_inq_dimid(in_id_src,"grid_rank",&src_grid_rank_id);
  rcd+=nco_inq_dimid(in_id_src,"grid_size",&src_grid_size_id);

  rcd+=nco_inq_dimid(in_id_dst,"grid_corners",&dst_grid_corners_id);
  rcd+=nco_inq_dimid(in_id_dst,"grid_rank",&dst_grid_rank_id);
  rcd+=nco_inq_dimid(in_id_dst,"grid_size",&dst_grid_size_id);

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

  /* 20200115: Tempest-generated (with ConvertExodustoSCRIP) physics grids "pg" grid-files, e.g., ne256pg2.nc, lack "grid_dims" variables */
  nco_bool has_dmn_sz_in=True; /* [flg] Source grid-file has "grid_dims" variable */
  nco_bool has_dmn_sz_out=True; /* [flg] Destination grid-file has "grid_dims" variable */
  rcd=nco_inq_varid_flg(in_id_src,"grid_dims",&dmn_sz_in_int_id);
  if(rcd != NC_NOERR) has_dmn_sz_in=False;
  rcd=nco_inq_varid_flg(in_id_dst,"grid_dims",&dmn_sz_out_int_id);
  if(rcd != NC_NOERR) has_dmn_sz_out=False;
  rcd=NC_NOERR;
  
  dmn_srt=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));
  dmn_cnt=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));

  dmn_sz_in_int=(int *)nco_malloc(mpf.src_grid_rank*nco_typ_lng((nc_type)NC_INT));
  dmn_sz_out_int=(int *)nco_malloc(mpf.dst_grid_rank*nco_typ_lng((nc_type)NC_INT));

  if(has_dmn_sz_in){
    dmn_srt[0]=0L;
    dmn_cnt[0]=mpf.src_grid_rank;
    rcd=nco_get_vara(in_id_src,dmn_sz_in_int_id,dmn_srt,dmn_cnt,dmn_sz_in_int,(nc_type)NC_INT);
  }else{ /* !has_dmn_sz_in */
    /* 20200115: If grid-files that lack "grid_dims" variables (like TR pg grids) are unstructured then skip sanity check otherwise bail */
    assert(mpf.src_grid_rank == 1);
    dmn_sz_in_int[0]=mpf.src_grid_size;
  } /* !has_dmn_sz_in */
  if(has_dmn_sz_out){
    dmn_srt[0]=0L;
    dmn_cnt[0]=mpf.dst_grid_rank;
    rcd=nco_get_vara(in_id_dst,dmn_sz_out_int_id,dmn_srt,dmn_cnt,dmn_sz_out_int,(nc_type)NC_INT);
  }else{ /* !has_dmn_sz_out */
    /* 20200115: If grid-files that lack "grid_dims" variables (like TR pg grids) are unstructured then skip sanity check otherwise bail */
    assert(mpf.dst_grid_rank == 1);
    dmn_sz_out_int[0]=mpf.dst_grid_size;
  } /* !has_dmn_sz_out */

  /* Check-for and workaround faulty grid sizes, typically from bogus dual-grid generation algorithm */
  if(flg_grd_in_1D && (mpf.src_grid_size != dmn_sz_in_int[0])){
    (void)fprintf(stdout,"%s: INFO %s reports unstructured input grid dimension sizes disagree: mpf.src_grid_size = %ld != %d = dmn_sz_in[0]. Problem may be caused by incorrect or non-existent \"grid_dims\" variable in source gridfile. This is a known issue with some gridfiles generated prior to ~20150901, particularly for spectral element dual-grids, and for grids created by TempestRemap. This problem can be safely ignored if workaround succeeds. Attempting workaround ...\n",nco_prg_nm_get(),fnc_nm,mpf.src_grid_size,dmn_sz_in_int[0]);
      dmn_sz_in_int[0]=mpf.src_grid_size;
  } /* !bug */
  /* 20200119: Why do PG2 grids still trigger this warning despite above workaround? */
  if(flg_grd_out_1D && (mpf.dst_grid_size != dmn_sz_out_int[0])){
    (void)fprintf(stdout,"%s: INFO %s reports unstructured output grid dimension sizes disagree: mpf.dst_grid_size = %ld != %d = dmn_sz_out[0]. Problem may be caused by incorrect or non-existent \"grid_dims\" variable in destination gridfile. This is a known issue with gridfiles generated prior to ~20150901, particularly for spectral element dual-grids, and for grids created by TempestRemap. This problem can be safely ignored if workaround succeeds. Attempting workaround ...\n",nco_prg_nm_get(),fnc_nm,mpf.dst_grid_size,dmn_sz_out_int[0]);
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
  if(rcd != NC_NOERR && nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s reports source grid file \"%s\" does not contain \"grid_area\" variable. By default, NCO calculates gridcell areas itself anyway, so this omission is fine.\n",nco_prg_nm_get(),fnc_nm,rgr->fl_grd_src);

  rcd+=nco_inq_varid(in_id_src,"grid_center_lon",&src_grd_ctr_lon_id); /* ESMF: xc_a */
  rcd+=nco_inq_varid(in_id_src,"grid_center_lat",&src_grd_ctr_lat_id); /* ESMF: yc_a */
  rcd+=nco_inq_varid(in_id_src,"grid_corner_lon",&src_grd_crn_lon_id); /* ESMF: xv_a */
  rcd+=nco_inq_varid(in_id_src,"grid_corner_lat",&src_grd_crn_lat_id); /* ESMF: yv_a */

  rcd=nco_inq_varid_flg(in_id_src,"grid_imask",&msk_in_id); /* ESMF: msk_a */

  rcd=nco_inq_varid_flg(in_id_dst,"grid_area",&area_out_id); /* ESMF: area_b */
  if(rcd != NC_NOERR && nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s reports destination grid file \"%s\" does not contain \"grid_area\" variable. By default, NCO calculates gridcell areas itself anyway, so this omission is fine.\n",nco_prg_nm_get(),fnc_nm,rgr->fl_grd_dst);

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

  /* If msk_in not in file then set each member to True */
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

  /* If msk_in not in file then set mask in each to True */
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

  if(msk_out_id != NC_MIN_INT) rcd=nco_get_vara(in_id_dst,msk_out_id,dmn_srt,dmn_cnt,msk_out,(nc_type)NC_INT);

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

  if(flg_grd_out_crv || flg_grd_in_crv) rgr->flg_crv=True;

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
  char *fl_grd_dst;
  char *fl_grd_src;
  char *fl_out;
  char *sls_ptr;

  int deflate; /* [flg] Turn-on deflate filter */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int out_id; /* I [id] Output netCDF file ID */
  int shuffle; /* [flg] Turn-on shuffle filter */
  int thr_nbr=int_CEWI; /* [nbr] Thread number */

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
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */

  size_t wgt_nbr=1L; /* [nbr] Number of weights */
  size_t lnk_nbr; /* [nbr] Number of links */

  deflate=(int)True;
  shuffle=NC_SHUFFLE;
  dfl_lvl=rgr->dfl_lvl;
  fl_grd_dst=rgr->fl_grd_dst;
  fl_grd_src=rgr->fl_grd_src;
  fl_out_fmt=rgr->fl_out_fmt;
  fl_out=rgr->fl_map;
  thr_nbr=rgr->thr_nbr;

  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG calling nco_msh_mk()...\n",nco_prg_nm_get());

  /* Define overlap mesh vertices, count links, compute overlap weights
     This includes Mohammad Abouali code to create triangle list and Zender code to compute triangle areas */
  (void)nco_msh_mk
    (rgr, &mpf,
     area_in,msk_in,lat_ctr_in,lon_ctr_in,lat_crn_in,lon_crn_in,
     area_out,msk_out,lat_ctr_out,lon_ctr_out,lat_crn_out,lon_crn_out,
     frc_in,frc_out,&col_src_adr,&row_dst_adr,&wgt_raw,&lnk_nbr);

  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG return from nco_msh_mk()...\n",nco_prg_nm_get());

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

  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Defining mapfile in format %s with n_s = %li, n_a = %li, n_b = %li. Mean number of links per destination cell = n_s/n_b = %li/%li = %g. Mean number of links per geometric mean src/dst cell = n_s/sqrt(n_a*n_b) = %li/sqrt(%li*%li) = %g. RAM size of weight variable n_s is sizeof(double)*n_s = 8*%li = %g MB. RAM sizes of vertex variables x/y_va are sizeof(double)*n_a*nv_a = 8*%li*%li = %g MB. RAM sizes of vertex variables x/y_vb are sizeof(double)*n_b*nv_b = 8*%li*%li = %g MB. Storage reduction due to employing sparse-matrix instead of full-matrix formulation is a factor of %ld/%ld ~ %ld.\n",nco_prg_nm_get(),nco_fmt_sng(fl_out_fmt),lnk_nbr,src_grd_sz_nbr,dst_grd_sz_nbr,lnk_nbr,dst_grd_sz_nbr,lnk_nbr/(1.0*dst_grd_sz_nbr),lnk_nbr,src_grd_sz_nbr,dst_grd_sz_nbr,lnk_nbr/sqrt(src_grd_sz_nbr*dst_grd_sz_nbr),lnk_nbr,sizeof(double)*lnk_nbr/1.0e6,src_grd_sz_nbr,src_grd_crn_nbr,sizeof(double)*src_grd_sz_nbr*src_grd_crn_nbr/1.0e6,dst_grd_sz_nbr,dst_grd_crn_nbr,sizeof(double)*dst_grd_sz_nbr*dst_grd_crn_nbr/1.0e6,src_grd_sz_nbr*dst_grd_sz_nbr*sizeof(double),lnk_nbr*sizeof(double)+(src_grd_sz_nbr+dst_grd_sz_nbr)*sizeof(int),(src_grd_sz_nbr*dst_grd_sz_nbr*sizeof(double))/(lnk_nbr*sizeof(double)+(src_grd_sz_nbr+dst_grd_sz_nbr)*sizeof(int)));

  /* Open mapfile */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  rcd+=nco_def_dim(out_id,"n_s",lnk_nbr,&num_links_id);
  rcd+=nco_def_dim(out_id,"n_a",src_grd_sz_nbr,&src_grid_size_id);
  rcd+=nco_def_dim(out_id,"n_b",dst_grd_sz_nbr,&dst_grid_size_id);
  rcd+=nco_def_dim(out_id,"nv_a",src_grd_crn_nbr,&src_grid_corners_id);
  rcd+=nco_def_dim(out_id,"nv_b",dst_grd_crn_nbr,&dst_grid_corners_id);
  rcd+=nco_def_dim(out_id,"src_grid_rank",src_grd_rnk_nbr,&src_grid_rank_id);
  rcd+=nco_def_dim(out_id,"dst_grid_rank",dst_grd_rnk_nbr,&dst_grid_rank_id);
  rcd+=nco_def_dim(out_id,"num_wgts",wgt_nbr,&num_wgts_id);

  rcd+=nco_def_var(out_id,"src_grid_dims",NC_INT,dmn_nbr_1D,&src_grid_rank_id,&dmn_sz_in_int_id);
  rcd+=nco_def_var(out_id,"dst_grid_dims",NC_INT,dmn_nbr_1D,&dst_grid_rank_id,&dmn_sz_out_int_id);

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

  /* Defining largest array last to help evade netCDF3-classic file format limits only works if
     largest array is S because x/yv_a/b come in pairs of equal size. So define S last. */
  rcd+=nco_def_var(out_id,"col",(nc_type)NC_INT,dmn_nbr_1D,&num_links_id,&col_src_adr_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,col_src_adr_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"row",(nc_type)NC_INT,dmn_nbr_1D,&num_links_id,&row_dst_adr_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,row_dst_adr_id,shuffle,deflate,dfl_lvl);
  rcd+=nco_def_var(out_id,"S",crd_typ,dmn_nbr_1D,&num_links_id,&wgt_raw_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,wgt_raw_id,shuffle,deflate,dfl_lvl);

  /* Implement CMIP6 conventions in Taylor, Oehmke, Ullrich, Zender et al. (2019), "CMIP6 Specifications for Regridding Weights" https://docs.google.com/document/d/1BfVVsKAk9MAsOYstwFSWI2ZBt5mrO_Nmcu7rLGDuL08/edit */
  rcd=nco_char_att_put(out_id,NULL,"title","netCDF Operators (NCO) Offline Regridding Weight Generator");
  rcd=nco_char_att_put(out_id,NULL,"Conventions","NCAR-CSM");
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */
  rcd=nco_char_att_put(out_id,NULL,"created_by",usr_cpp);
  if(rgr->wgt_typ == nco_wgt_con) rcd=nco_char_att_put(out_id,NULL,"map_method","Conservative");
  else if(rgr->wgt_typ == nco_wgt_dwe) rcd=nco_char_att_put(out_id,NULL,"map_method","Bilinear"); /* NB: map_method values are constrained by SCRIP/ESMF precedence. Values besides "Conservative" and "Bilinear" may be unsupported by other regridders. */
  /* ERWG stores actual weight generation in "ESMF_regrid_method" */
  if(rgr->wgt_typ == nco_wgt_con) rcd=nco_char_att_put(out_id,NULL,"NCO_regrid_method","Integrated piecewise-constant reconstruction of common intersection mesh. Conservative, monotone, suitable for downscaling, blocky results for upscaling. First order accurate. Similar to ESMF conserve method.");
  else if(rgr->wgt_typ == nco_wgt_dwe) rcd=nco_char_att_put(out_id,NULL,"NCO_regrid_method","Distance-Weighted Extrapolation");
  rcd=nco_char_att_put(out_id,NULL,"weight_generator","NCO");
  char vrs_cpp[]=TKN2SNG(NCO_VERSION); /* [sng] Version from C pre-processor */
  /* 20170417: vrs_cpp is typically something like "4.6.6-alpha10" (quotes included) 
     The quotation marks annoy me yet are necessary to protect the string in nco.h 
     Here we remove the quotation marks by pointing past the first and putting NUL in the last */
  char *vrs_sng; /* [sng] NCO version */
  vrs_sng=vrs_cpp;
  if(vrs_cpp[0L] == '"'){
    vrs_cpp[strlen(vrs_cpp)-1L]='\0';
    vrs_sng=vrs_cpp+1L;
  } /* endif */
  rcd=nco_char_att_put(out_id,NULL,"weight_generator_version",vrs_sng);
  rcd=nco_char_att_put(out_id,NULL,"grid_file_src",(sls_ptr=strrchr(fl_grd_src,'/')) == NULL ? fl_grd_src : sls_ptr+1);
  rcd=nco_char_att_put(out_id,NULL,"grid_file_dst",(sls_ptr=strrchr(fl_grd_dst,'/')) == NULL ? fl_grd_dst : sls_ptr+1);
  (void)nco_hst_att_cat(out_id,rgr->cmd_ln);
  (void)nco_vrs_att_cat(out_id);
  if(thr_nbr >= 1) (void)nco_thr_att_cat(out_id,thr_nbr);
  rcd=nco_char_att_put(out_id,"S","long_name","Weights to Map Variables from Source to Destination Grid");
  rcd=nco_char_att_put(out_id,"area_a","long_name","Solid Angle Subtended on Source Grid");
  rcd=nco_char_att_put(out_id,"area_a","standard_name","solid_angle");
  rcd=nco_char_att_put(out_id,"area_a","units","steradian");
  rcd=nco_char_att_put(out_id,"area_b","long_name","Solid Angle Subtended on Destination Grid");
  rcd=nco_char_att_put(out_id,"area_b","standard_name","solid_angle");
  rcd=nco_char_att_put(out_id,"area_b","units","steradian");
  rcd=nco_char_att_put(out_id,"col","long_name","Pointer to Source Grid Element (with 1, not 0, indicating the first element)");
  rcd=nco_char_att_put(out_id,"dst_grid_dims","long_name","Size of each logical dimension in the Destination Grid, in Fortran order (for historical reasons), i.e., from most- to least-rapidly varying");
  rcd=nco_char_att_put(out_id,"frac_a","long_name","Fraction of Source Gridcell That Participates in Remapping");
  rcd=nco_char_att_put(out_id,"frac_a","units","none");
  rcd=nco_char_att_put(out_id,"frac_b","long_name","Fraction of Destination Gridcell That Participates in Remapping");
  rcd=nco_char_att_put(out_id,"frac_b","units","none");
  rcd=nco_char_att_put(out_id,"mask_a","long_name","Binary Integer Mask for Source Grid");
  rcd=nco_char_att_put(out_id,"mask_a","units","none");
  rcd=nco_char_att_put(out_id,"mask_b","long_name","Binary Integer Mask for Destination Grid");
  rcd=nco_char_att_put(out_id,"mask_b","units","none");
  rcd=nco_char_att_put(out_id,"row","long_name","Pointer to Destination Grid Element (with 1, not 0, indicating the first element)");
  rcd=nco_char_att_put(out_id,"src_grid_dims","long_name","Size of each logical dimension in the Source Grid, in Fortran order (for historical reasons), from most- to least-rapidly varying");
  rcd=nco_char_att_put(out_id,"xc_a","bounds","xv_a");
  rcd=nco_char_att_put(out_id,"xc_a","long_name","Longitude of Source Grid Cell Centers");
  rcd=nco_char_att_put(out_id,"xc_a","standard_name","longitude");
  rcd=nco_char_att_put(out_id,"xc_a","units","degrees_east");
  rcd=nco_char_att_put(out_id,"xc_b","bounds","xv_b");
  rcd=nco_char_att_put(out_id,"xc_b","long_name","Longitude of Destination Grid Cell Centers");
  rcd=nco_char_att_put(out_id,"xc_b","standard_name","longitude");
  rcd=nco_char_att_put(out_id,"xc_b","units","degrees_east");
  rcd=nco_char_att_put(out_id,"xv_a","long_name","Longitude of Source Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,"xv_a","standard_name","longitude");
  rcd=nco_char_att_put(out_id,"xv_a","units","degrees_east");
  rcd=nco_char_att_put(out_id,"xv_b","long_name","Longitude of Destination Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,"xv_b","standard_name","longitude");
  rcd=nco_char_att_put(out_id,"xv_b","units","degrees_east");
  rcd=nco_char_att_put(out_id,"yc_a","bounds","yv_a");
  rcd=nco_char_att_put(out_id,"yc_a","long_name","Latitude of Source Grid Cell Centers");
  rcd=nco_char_att_put(out_id,"yc_a","standard_name","latitude");
  rcd=nco_char_att_put(out_id,"yc_a","units","degrees_north");
  rcd=nco_char_att_put(out_id,"yc_b","bounds","yv_b");
  rcd=nco_char_att_put(out_id,"yc_b","long_name","Latitude of Destination Grid Cell Centers");
  rcd=nco_char_att_put(out_id,"yc_b","standard_name","latitude");
  rcd=nco_char_att_put(out_id,"yc_b","units","degrees_north");
  rcd=nco_char_att_put(out_id,"yv_a","long_name","Latitude of Source Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,"yv_a","standard_name","latitude");
  rcd=nco_char_att_put(out_id,"yv_a","units","degrees_north");
  rcd=nco_char_att_put(out_id,"yv_b","long_name","Latitude of Destination Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,"yv_b","standard_name","latitude");
  rcd=nco_char_att_put(out_id,"yv_b","units","degrees_north");
  
  /* Turn-off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);

  /* Take output file out of define mode */
  /* 20200119: Map-writing fails mysteriously here with E_VARSIZE, for netCDF 4.6.3 compy */
  hdr_pad=rgr->hdr_pad; /* [B] Pad at end of header section */
  if(hdr_pad == 0UL){
    (void)nco_enddef(out_id);
  }else{
    (void)nco__enddef(out_id,hdr_pad);
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  /* Begin data mode */

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

  /* Set static variable */
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



  size_t idx; /* [idx] Counting index for unrolled grids */
  size_t lnk_nbr=0; /* [nbr] Number of links */
  //size_t lnk_idx; /* [idx] Link index */

  int pl_cnt_vrl=0;

  int lst_typ=0;  /* 0 - no list, 1 - poly_sct list, 2 - wgt_lst */



  poly_sct *pl_glb_in=NULL_CEWI;
  poly_sct *pl_glb_out=NULL_CEWI;

  poly_sct **pl_lst_in=NULL_CEWI;
  poly_sct **pl_lst_out=NULL_CEWI;

  poly_sct **pl_lst_vrl=(poly_sct**)NULL_CEWI;
  wgt_sct **wgt_lst_vrl=(wgt_sct**)NULL_CEWI;


  poly_typ_enm pl_typ=poly_none  ;





  grd_crn_nbr_in=mpf->src_grid_corners;
  grd_crn_nbr_out=mpf->dst_grid_corners;

  grd_sz_in=mpf->src_grid_size;
  grd_sz_out=mpf->dst_grid_size;

  /* Construct overlap mesh here
     NB: Parallelize loop with OpenMP and/or MPI
     Final lnk_nbr and grd_crn_nbr_vrl are known only after a full loop through input grids */

  /* Choose mesh overlap type based on rank of src and dst */
  if(!rgr->flg_crv && /* Grid is not curvilinear (fxm: check should be if _either_ grid is curvilinear) */
     (rgr->edg_typ != nco_edg_gtc) && /* User did not specify great circle edges */
     (mpf->src_grid_rank == 2 && mpf->dst_grid_rank == 2)) /* Both grids are 2-D */
  {
    /* Use small circle triangles for RLL mapping if user did specify great circles edges */
    if(rgr->edg_typ == nco_edg_nil) rgr->edg_typ=nco_edg_smc;
    pl_typ=poly_rll;
  }else{
    if(rgr->edg_typ == nco_edg_nil) rgr->edg_typ=nco_edg_gtc;
    pl_typ=poly_sph;
  } /* !edg_typ */
  /* 20200304: NB: edg_typ and pl_typ are NOT synonyms:
     pl_typ determines which area routines to use
     edg_typ determines which edge types to assume for intersections and within the spherical area routines
     Polygons of type poly_sph may have edges of type edg_typ_smc
     edg_typ_smc means edges with same latitude are small circles, other edges are great circles */

  if(nco_dbg_lvl_get() >= nco_dbg_crr)
    (void)fprintf(stderr,"%s:%s(): Interpolation type=%s\n",nco_prg_nm_get(),fnc_nm, nco_poly_typ_sng_get(pl_typ));

  /* Create some statistics on grid in and grid out */
  pl_glb_in=nco_msh_stats(area_in,msk_in,lat_ctr_in, lon_ctr_in, lat_crn_in, lon_crn_in,grd_sz_in, grd_crn_nbr_in);
  pl_glb_out=nco_msh_stats(area_out,msk_out,lat_ctr_out, lon_ctr_out, lat_crn_out, lon_crn_out,grd_sz_out, grd_crn_nbr_out);

  /* determine lon_typ  nb temporary */
  grd_lon_typ_in=nco_poly_minmax_2_lon_typ(pl_glb_in);
  grd_lon_typ_out=nco_poly_minmax_2_lon_typ(pl_glb_out);

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s:%s mesh in statistics (grd_lon_typ=%s)\n",nco_prg_nm_get(),fnc_nm, nco_grd_lon_sng(grd_lon_typ_in) );
    nco_poly_prn(pl_glb_in,0);

    (void)fprintf(stderr,"\n%s:%s mesh out statistics (grd_lon_typ=%s)\n",nco_prg_nm_get(),fnc_nm, nco_grd_lon_sng(grd_lon_typ_out));
    nco_poly_prn(pl_glb_out,0);
  }

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
        else if(pl_typ == poly_sph || pl_typ == poly_rll)
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
    } /* !grd_lon_typ_out */

    if(pl_typ == poly_sph)
      pl_lst_out = nco_poly_lst_mk_sph(area_out, msk_out, lat_ctr_out, lon_ctr_out, lat_crn_out, lon_crn_out,grd_sz_out, (size_t) grd_crn_nbr_out, grd_lon_typ_out);
    else if(pl_typ == poly_rll)
      pl_lst_out = nco_poly_lst_mk_rll(area_out, msk_out, lat_ctr_out, lon_ctr_out, lat_crn_out, lon_crn_out, grd_sz_out, (size_t) grd_crn_nbr_out, grd_lon_typ_out);

    pl_cnt_out = grd_sz_out;

    if(pl_typ == poly_sph)
      pl_lst_in = nco_poly_lst_mk_sph(area_in, msk_in, lat_ctr_in, lon_ctr_in, lat_crn_in, lon_crn_in, grd_sz_in, (size_t) grd_crn_nbr_in, grd_lon_typ_out);
    else if(pl_typ == poly_rll)
      pl_lst_in = nco_poly_lst_mk_rll(area_in, msk_in, lat_ctr_in, lon_ctr_in, lat_crn_in, lon_crn_in, grd_sz_in, (size_t) grd_crn_nbr_in, grd_lon_typ_out);

    pl_cnt_in = grd_sz_in;
    /* test new write func */
    // 20190526: Stop writing this file (the output grid) by default.
    //    nco_msh_poly_lst_wrt("nco_map_tst_out.nc", pl_lst_out, pl_cnt_out, grd_lon_typ_out  );

    /* debug output all
    for(idx=0;idx<pl_cnt_out  ;idx++)
      nco_poly_prn(pl_lst_out[idx],1);
    */
  }

  if(rgr->wgt_typ == nco_wgt_con){
    int nbr_tr;
    KDTree **tree;
    void **void_lst_vrl=NULL_CEWI;

    if(rgr->fl_msh)
      lst_typ=2;
    else
      lst_typ=1;

    tree=nco_map_kd(pl_lst_out, pl_cnt_out, grd_lon_typ_out,&nbr_tr);

      /* temporarily disable crt code */
      /*  if(pl_typ == poly_crt) pl_lst_vrl=nco_poly_lst_mk_vrl_crt(pl_lst_in, pl_cnt_in, rtree, &pl_cnt_vrl); */

     /* add "centers" - */
     if(pl_typ == poly_sph){
       nco_poly_lst_ctr_add(pl_lst_in, grd_sz_in,1);
       nco_poly_lst_ctr_add(pl_lst_out, grd_sz_out,1);
     }

    if(pl_typ == poly_sph || pl_typ == poly_rll)
      /* REMEMBER the return type is void ** but it may actually be wgt_sct ** or poly_sct **, so we recast it */
      void_lst_vrl=nco_poly_lst_mk_vrl(pl_lst_in, grd_sz_in, grd_lon_typ_out, pl_typ, tree, nbr_tr, lst_typ, &pl_cnt_vrl);

    if(lst_typ == 1)
      wgt_lst_vrl=(wgt_sct **)void_lst_vrl;
    else if(lst_typ == 2)
      pl_lst_vrl=(poly_sct **)void_lst_vrl;

    if(nco_dbg_lvl_get() >= nco_dbg_dev)
      fprintf(stderr, "%s: INFO: num input polygons=%lu, num output polygons=%lu num overlap polygons=%d\n", nco_prg_nm_get(), grd_sz_in, grd_sz_out, pl_cnt_vrl);

    /* Write-out overlap mesh
     20190526: Allow users to name and output mesh-file with --rgr msh_fl=msh.nc. https://github.com/nco/nco/issues/135 */
    if(rgr->fl_msh) nco_msh_poly_lst_wrt(rgr->fl_msh, pl_lst_vrl, pl_cnt_vrl, grd_lon_typ_out, rgr->fl_out_fmt);

    if(nco_dbg_lvl_get() >= nco_dbg_dev && lst_typ==2 && pl_lst_vrl){
      int io_flg = 1;
      int pl_nbr = 0;

      poly_sct **pl_lst_dbg = NULL_CEWI;

      /* Find area mismatch between dst and overlap */
      fprintf(stderr, "%s(): Comparing dst areas with overlap areas\n", fnc_nm);
      pl_lst_dbg = nco_poly_lst_chk_dbg(pl_lst_out, grd_sz_out, pl_lst_vrl, pl_cnt_vrl, io_flg, &pl_nbr);

      if(pl_nbr){
        nco_msh_poly_lst_wrt("nco_map_tst_out_dbg.nc", pl_lst_dbg, pl_nbr, grd_lon_typ_out, rgr->fl_out_fmt);
        pl_lst_dbg = nco_poly_lst_free(pl_lst_dbg, pl_nbr);
      }

      pl_nbr = 0;
      io_flg = 0;

      /* find area mismatch between src and overlap */
      fprintf(stderr,"%s(): Comparing src areas with overlap areas\n",fnc_nm);
      pl_lst_dbg=nco_poly_lst_chk_dbg(pl_lst_in, grd_sz_in, pl_lst_vrl, pl_cnt_vrl, io_flg, &pl_nbr);

      if(pl_nbr){
        nco_msh_poly_lst_wrt("nco_map_tst_in_dbg.nc", pl_lst_dbg, pl_nbr, grd_lon_typ_out, rgr->fl_out_fmt);
        pl_lst_dbg = nco_poly_lst_free(pl_lst_dbg, pl_nbr);
      }
    }

    for(idx=0;idx < nbr_tr;idx++)
      kd_destroy(tree[idx],NULL);

    tree=(KDTree **)nco_free(tree);
    lnk_nbr=pl_cnt_vrl;

  }else if(rgr->wgt_typ == nco_wgt_dwe){

    int nbr_tr;
    KDTree **tree;

    tree=nco_map_kd(pl_lst_in, pl_cnt_in, grd_lon_typ_out,&nbr_tr);
    lst_typ=1;

    /* temporarily disable crt code */
    /* if(pl_typ == poly_crt) pl_lst_vrl=nco_poly_lst_mk_vrl_crt(pl_lst_in, pl_cnt_in, rtree, &pl_cnt_vrl); */

    if(pl_typ == poly_sph || pl_typ == poly_rll)
      wgt_lst_vrl = nco_poly_lst_mk_dwe_sph(map_rgr, pl_lst_out, grd_sz_out, grd_lon_typ_out, tree, nbr_tr, &pl_cnt_vrl);

    pl_lst_vrl=(poly_sct**)NULL_CEWI;

    if(nco_dbg_lvl_get() >= nco_dbg_dev)
      fprintf(stderr, "%s: INFO: num input polygons=%lu, num output polygons=%lu num overlap weights(nni)=%d\n", nco_prg_nm_get(), grd_sz_in, grd_sz_out, pl_cnt_vrl);

    /* dont really like this - for weight of input cells maybe greater than 1 - some maybe 0.0  */
    for(idx=0;idx<pl_cnt_vrl;idx++)
      pl_lst_in[ wgt_lst_vrl[idx]->src_id]->wgt+=wgt_lst_vrl[idx]->wgt;

    for (idx = 0; idx < nbr_tr; idx++)
      kd_destroy(tree[idx], NULL);

    tree = (KDTree **) nco_free(tree);

    lnk_nbr=pl_cnt_vrl;
  }

  wgt_raw = (double *) nco_malloc(lnk_nbr * nco_typ_lng(NC_DOUBLE));
  col_src_adr = (int *) nco_malloc(lnk_nbr * nco_typ_lng(NC_INT));
  row_dst_adr = (int *) nco_malloc(lnk_nbr * nco_typ_lng(NC_INT));

  if(lst_typ == 1){
    wgt_sct *wgt_lcl;

    for (idx = 0; idx < lnk_nbr; idx++) {
      /* de-refernce */
      wgt_lcl = wgt_lst_vrl[idx];

      if (wgt_lcl->wgt > 1.0 && wgt_lcl->wgt < 1.0 + 1.0e-10)
        wgt_lcl->wgt = 1.0;

      wgt_raw[idx] = wgt_lcl->wgt;

      pl_lst_out[wgt_lcl->dst_id]->wgt += wgt_lcl->wgt;

      col_src_adr[idx] = wgt_lcl->src_id + 1;
      row_dst_adr[idx] = wgt_lcl->dst_id + 1;
    } /* !idx */
  }else if(lst_typ == 2){
    for (idx = 0; idx < lnk_nbr; idx++) {

      if (pl_lst_vrl[idx]->wgt > 1.0 && pl_lst_vrl[idx]->wgt < 1.0 + 1.0e-10)
        pl_lst_vrl[idx]->wgt = 1.0;

      wgt_raw[idx] = pl_lst_vrl[idx]->wgt;

      pl_lst_out[pl_lst_vrl[idx]->dst_id]->wgt += pl_lst_vrl[idx]->wgt;

      col_src_adr[idx] = pl_lst_vrl[idx]->src_id + 1;
      row_dst_adr[idx] = pl_lst_vrl[idx]->dst_id + 1;
    } /* !idx */
  }

  for (idx = 0; idx < grd_sz_in; idx++) {
    if (pl_lst_in[idx]->wgt > 0.0) {
      frc_in[idx] = pl_lst_in[idx]->wgt;
      msk_in[idx] = 1;
    } else {
      frc_in[idx] = 0.0;
      //msk_in[idx] = 0;
    }
  } /* !idx */

  for (idx = 0; idx < grd_sz_out; idx++) {
    if (pl_lst_out[idx]->wgt > 0.0) {
      frc_out[idx] = pl_lst_out[idx]->wgt;
      msk_out[idx] = 1;
    } else {
      frc_out[idx] = 0.0;
      //msk_out[idx] = 0;
    }
  } /* !idx */

  *wgt_raw_ptr = wgt_raw;
  *col_src_adr_ptr = col_src_adr;
  *row_dst_adr_ptr = row_dst_adr;
  *lnk_nbr_ptr = lnk_nbr;

  pl_glb_in = nco_poly_free(pl_glb_in);
  pl_glb_out = nco_poly_free(pl_glb_out);

  if(lst_typ==1 && wgt_lst_vrl){
    for(idx=0;idx < pl_cnt_vrl;idx++)
      wgt_lst_vrl[idx]=(wgt_sct*)nco_free(wgt_lst_vrl[idx]);

    wgt_lst_vrl=(wgt_sct**)nco_free(wgt_lst_vrl);
  }else if(lst_typ==2 && pl_lst_vrl)
      pl_lst_vrl = nco_poly_lst_free(pl_lst_vrl, pl_cnt_vrl);

  if (grd_sz_in) pl_lst_in = nco_poly_lst_free(pl_lst_in, grd_sz_in);
  if (grd_sz_out) pl_lst_out = nco_poly_lst_free(pl_lst_out, grd_sz_out);

  return rcd;
} /* !nco_msh_mk() */

/* create tree */
KDTree**
nco_map_kd(
poly_sct **pl_lst,
int pl_cnt,
nco_grd_lon_typ_enm grd_lon_typ,
int *nbr_tr)
{
  int thr_idx;
  /* nbr nodes (to insert) per tree */
  size_t quota = 0;
  /* extra to add to last last tree */
  size_t nbr_xcs;
  size_t idx;


  FILE *fp_stderr = stderr;
  KDTree ** tree;

  if (pl_cnt < 10000)
    *nbr_tr = 1;
  else
    *nbr_tr = omp_get_max_threads();


  quota = pl_cnt / *nbr_tr;
  nbr_xcs = pl_cnt % *nbr_tr;

  tree = (KDTree **) nco_calloc(*nbr_tr, sizeof(KDTree *));

  #if defined(__INTEL_COMPILER)
  # pragma omp parallel for default(none) private(idx,thr_idx) shared(fp_stderr,  grd_lon_typ, nbr_tr, nbr_xcs, pl_lst, quota, tree)
  #else /* !__INTEL_COMPILER */
  # ifdef GXX_OLD_OPENMP_SHARED_TREATMENT
  #  pragma omp parallel for default(none) private(idx,thr_idx) shared(fp_stderr, grd_lon_typ, nbr_tr, nbr_xcs, pl_lst, quota, tree)
  # else /* !old g++ */
  #  pragma omp parallel for private(idx, thr_idx) shared(fp_stderr, grd_lon_typ, nbr_tr, nbr_xcs, pl_lst, quota, tree)
  # endif /* !old g++ */
  #endif /* !__INTEL_COMPILER */
  for (idx = 0; idx < *nbr_tr; idx++)
  {

    thr_idx = omp_get_thread_num();
    tree[idx] = nco_map_kd_init(pl_lst + quota * idx, quota + (idx == *nbr_tr - 1 ? nbr_xcs : 0), grd_lon_typ);

    if (nco_dbg_lvl_get() >= 3)
      (void) fprintf(fp_stderr, "%s: thread %d created a kdtree of %d nodes\n", nco_prg_nm_get(), thr_idx,tree[idx]->item_count);

  } /* end for */

  return tree;
} /* end tree scope */

KDTree *
nco_map_kd_init( poly_sct **pl_lst, int pl_cnt, nco_grd_lon_typ_enm grd_lon_typ   )
{
  int idx;
  KDTree *rtree;

  rtree=kd_create();

  nco_bool bSplit = False;

  kd_box size1;
  kd_box size2;

  KDElem* my_elem1=NULL_CEWI;
  KDElem* my_elem2=NULL_CEWI;

  for (idx = 0; idx < pl_cnt; idx++) {

    if(!pl_lst[idx]->bmsk )
      continue;

    my_elem1 = (KDElem *) nco_calloc((size_t) 1, sizeof(KDElem));

    /* kd tree cannot handle wrapped coordinates so split minmax if needed*/
    bSplit = nco_poly_minmax_split(pl_lst[idx], grd_lon_typ, size1, size2);

    kd_insert(rtree, (kd_generic) pl_lst[idx], size1, (char *) my_elem1);

    if (bSplit) {
      my_elem2 = (KDElem *) nco_calloc((size_t) 1, sizeof(KDElem));
      kd_insert(rtree, (kd_generic) pl_lst[idx], size2, (char *) my_elem2);
    }

  }
  return rtree;
}

int
nco_msh_wrt
(const  char *fl_out,
 size_t grd_sz_nbr,
 size_t grd_crn_nbr,
 double *lat_crn,
 double *lon_crn)
{
  // const char fnc_nm[]="nco_msh_wrt()"; /* [sng] Function name */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_1D=1; /* [nbr] Rank of 2-D grid variables */

  int rcd;
  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */
  //int thr_nbr=int_CEWI; /* [nbr] Thread number */

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

  long dmn_srt[2];
  long dmn_cnt[2];

  const nc_type crd_typ=NC_DOUBLE;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  char *fl_out_tmp=NULL_CEWI;

  char grd_area_nm[]="grid_area"; /* 20150830: NB ESMF_RegridWeightGen --user_areas looks for variable named "grid_area" */
  char grd_crn_lat_nm[]="grid_corner_lat";
  char grd_crn_lon_nm[]="grid_corner_lon";
  char grd_crn_nm[]="grid_corners";

  char grd_sz_nm[]="grid_size";
  // char msk_nm[]="grid_imask";

  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */
  
  size_t hdr_pad=10000UL; /* [B] Pad at end of header section */

  area=(double*)nco_malloc( sizeof(double) * grd_sz_nbr);

  nco_sph_plg_area(map_rgr,lat_crn,lon_crn,grd_sz_nbr,grd_crn_nbr,area);

  /* Open grid file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  /* Define dimensions */
  rcd=nco_def_dim(out_id,grd_crn_nm,grd_crn_nbr,&dmn_id_grd_crn);
  rcd=nco_def_dim(out_id,grd_sz_nm,grd_sz_nbr,&dmn_id_grd_sz);

  dmn_ids[0]=dmn_id_grd_sz;
  dmn_ids[1]=dmn_id_grd_crn;

  deflate=(int)True;
  shuffle=NC_SHUFFLE;
  //thr_nbr=rgr->thr_nbr;

  /* Define variables */
  (void)nco_def_var(out_id,grd_crn_lat_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lat_id,shuffle,deflate,dfl_lvl);

  (void)nco_def_var(out_id,grd_crn_lon_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lon_id,shuffle,deflate,dfl_lvl);

  (void)nco_def_var(out_id,grd_area_nm,crd_typ,dmn_nbr_1D,dmn_ids,&grd_area_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_area_id,shuffle,deflate,dfl_lvl);

  //if(thr_nbr >= 1) (void)nco_thr_att_cat(out_id,thr_nbr);

  /* Take output file out of define mode */
  /* 20200119: Map-writing fails mysteriously here with E_VARSIZE, for netCDF 4.6.3 compy */
  if(hdr_pad == 0UL){
    (void)nco_enddef(out_id);
  }else{
    (void)nco__enddef(out_id,hdr_pad);
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO Padding header with %lu extra bytes\n",nco_prg_nm_get(),(unsigned long)hdr_pad);
  } /* hdr_pad */

  dmn_srt[0]=0L;
  dmn_srt[1]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  dmn_cnt[1]=grd_crn_nbr;

  rcd=nco_put_vara(out_id,grd_crn_lat_id,dmn_srt,dmn_cnt,lat_crn,crd_typ);
  rcd=nco_put_vara(out_id,grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn,crd_typ);
  rcd=nco_put_vara(out_id,grd_area_id,dmn_srt,dmn_cnt,area,crd_typ);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  area=(double*)nco_free(area);

  return True;
} /* !nco_msh_wrt() */

poly_sct *           /* return a ply_sct with lat/lon minmax and total area */
nco_msh_stats
(double *area,       /* I [sr] Area of grid */
 int *msk,           /* I [flg] Mask on grid */
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

 pl->dp_x_minmax[0]=KD_DBL_MAX;
 pl->dp_x_minmax[1]=-KD_DBL_MAX;

 pl->dp_y_minmax[0]=KD_DBL_MAX;
 pl->dp_y_minmax[1]=-KD_DBL_MAX;
 
 for(idx=0; idx< grd_sz;idx++ ){
   if(msk[idx]){
     tot_area += area[idx];
     umsk_cnt++;
   }
   
   for (jdx = 0; jdx < grd_crn_nbr; jdx++){
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
 if(umsk_cnt==0){
   pl=nco_poly_free(pl);
   return pl;
 }

 pl->area=tot_area;
 nco_poly_minmax_use_crn(pl);

 return pl;
} /* !nco_msh_stats() */

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
} /* !nco_msh_lon_crr() */

void
nco_msh_poly_lst_wrt
(const char *fl_out,
poly_sct ** pl_lst,
int pl_nbr,
nco_grd_lon_typ_enm grd_lon_typ,
int fl_out_fmt)
{
  //const char fnc_nm[]="nco_msh_poly_lst_wrt()"; /* [sng] Function name */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_1D=1; /* [nbr] Rank of 2-D grid variables */

  int rcd;
  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */

  int out_id; /* I [id] Output netCDF file ID */
  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
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
  nco_bool SHARE_CREATE=False; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=False; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
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

    /* fill-in remaining values with last values */
    if(lcl_crn_nbr < grd_crn_nbr)
      for(jdx = lcl_crn_nbr; jdx < grd_crn_nbr; jdx++){
        lon_crn_ptr[jdx] = pl_lst[idx]->dp_x[lcl_crn_nbr - 1];
        lat_crn_ptr[jdx] = pl_lst[idx]->dp_y[lcl_crn_nbr - 1];
      }
  }
  
  /* Open grid file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  /* Define dimensions */
  rcd=nco_def_dim(out_id,grd_sz_nm,grd_sz_nbr,&dmn_ids[0]);
  rcd=nco_def_dim(out_id,grd_crn_nm,grd_crn_nbr,&dmn_ids[1]);
  grd_rnk_nbr=1;
  rcd=nco_def_dim(out_id,grd_rnk_nm,grd_rnk_nbr,&dmn_ids[2]);

  deflate=(int)True;
  shuffle=NC_SHUFFLE;

  /* Define variables */
  (void)nco_def_var(out_id,grd_crn_lat_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lat_id,shuffle,deflate,dfl_lvl);
  nco_msh_att_char(out_id,grd_crn_lat_id,grd_crn_lat_nm,"units","degrees");

  (void)nco_def_var(out_id,grd_crn_lon_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lon_id,shuffle,deflate,dfl_lvl);
  nco_msh_att_char(out_id,grd_crn_lon_id,grd_crn_lon_nm,"units","degrees");

  (void)nco_def_var(out_id,grd_area_nm,crd_typ,dmn_nbr_1D,dmn_ids,&grd_area_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_area_id,shuffle,deflate,dfl_lvl);
  nco_msh_att_char(out_id,grd_area_id,grd_area_nm,"units","steradians");

  (void)nco_def_var(out_id,grd_ctr_lon_nm,crd_typ,dmn_nbr_1D,dmn_ids,&grd_ctr_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_ctr_lon_id,shuffle,deflate,dfl_lvl);
  nco_msh_att_char(out_id,grd_ctr_lon_id,grd_ctr_lon_nm,"units","degrees");

  (void)nco_def_var(out_id,grd_ctr_lat_nm,crd_typ,dmn_nbr_1D,dmn_ids,&grd_ctr_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_ctr_lat_id,shuffle,deflate,dfl_lvl);
  nco_msh_att_char(out_id,grd_ctr_lat_id,grd_ctr_lat_nm,"units","degrees");

  (void)nco_def_var(out_id,msk_nm,NC_INT,dmn_nbr_1D,dmn_ids,&grd_msk_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_msk_id,shuffle,deflate,dfl_lvl);

  (void)nco_def_var(out_id,dmn_sz_nm,NC_INT,dmn_nbr_1D,&dmn_ids[2],&grd_rnk_id);

  /* Begin data mode */
  (void)nco_enddef(out_id);

  rcd=nco_put_var(out_id,grd_crn_lat_id,lat_crn,crd_typ);
  rcd=nco_put_var(out_id,grd_crn_lon_id,lon_crn,crd_typ);

  rcd=nco_put_var(out_id,grd_area_id,area,crd_typ);
  rcd=nco_put_var(out_id,grd_ctr_lon_id,lon_ctr,crd_typ);
  rcd=nco_put_var(out_id,grd_ctr_lat_id,lat_ctr,crd_typ);

  rcd=nco_put_var(out_id,grd_msk_id,msk,NC_INT);

  rcd=nco_put_var(out_id,grd_rnk_id,&grd_sz_nbr,NC_INT);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

  fl_out_tmp=(char*)nco_free(fl_out_tmp);

  area=(double*)nco_free(area);
  lat_ctr=(double*)nco_free(lat_ctr);
  lat_crn=(double*)nco_free(lat_crn);

  lon_ctr=(double*)nco_free(lon_ctr);
  lon_crn=(double*)nco_free(lon_crn);
  msk=(int*)nco_free(msk);
} /* !nco_msh_poly_lst_wrt() */

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
} /* !nco_msh_att_char() */


void
nco_poly_area_add(
poly_sct *pl){

  const char fnc_nm[]="nco_poly_area_add()";

  /* only situation where map_rgr is null is when running debug program vrl-tst */
  if(!map_rgr) {
    map_rgr = (rgr_sct *) nco_calloc(1, sizeof(rgr_sct));
    map_rgr->ply_tri_mth = nco_ply_tri_mth_csz;
    map_rgr->edg_typ = nco_edg_gtc;
    map_rgr->area_mth = 2;
  }



  if(pl->crn_nbr <3) {
    pl->area = 0.0;
    return;
  }

  if(pl->pl_typ == poly_sph )
  {

    if(map_rgr->area_mth==1) {
      nco_sph_plg_area(map_rgr, pl->dp_y, pl->dp_x, 1, pl->crn_nbr, &pl->area);
    }
    else if(map_rgr->area_mth==2) {

      if(!pl->shp)
      {
        (void)fprintf(stderr, "%s:%s: attempt to call nco_sph_area_quadrature() with pl->shp==null\n", nco_prg_nm_get(), fnc_nm);
         abort();
      }

      pl->area = nco_sph_area_quadrature(pl->shp, pl->crn_nbr);
    }

    if (isnan(pl->area))
      pl->area = 0.0;

  }

  /* rll poly polygon should only have 3 or 4 vertex */
  if(pl->pl_typ == poly_rll){
    /* full formula area=(lon1-lon0)*(sin(lat1)-sin(lat0) ) */
    double dp_tmp=sin(D2R( pl->dp_y_minmax[1] )) - sin(D2R(pl->dp_y_minmax[0]) );

    double dff=pl->dp_x_minmax[1] - pl->dp_x_minmax[0];

    if(pl->bwrp )
      dp_tmp *= D2R(360.0 - dff );
    else
      dp_tmp*=D2R(dff);


    pl->area = fabs(dp_tmp);



  }
  //nco_rll_area(pl);

  return;

}






nco_bool
nco_map_hst_mk /* Create histogram */
(var_sct * var_row,
 int row_max,
 int hst_ar[],
 int hst_sz )
{
  int idx;
  int sz;
  int idx_row=0;
  int *row_bin=NULL_CEWI;
  
  (void)cast_void_nctype(NC_DOUBLE,&(var_row->val));
  
  sz=var_row->sz;
  
  row_bin=(int *)nco_calloc(row_max+1,sizeof(int));
  
  /* Row count: var_row and row_bin are one-based */
  for(idx=0;idx<sz;idx++)
    if((idx_row=var_row->val.ip[idx]) <= row_max)
      row_bin[idx_row]++;
  
  /* Histogram count is one-based */
  for(idx=1;idx<=row_max;idx++){
    idx_row=row_bin[idx];
    if(idx_row < hst_sz) hst_ar[idx_row]++; else hst_ar[hst_sz]++;
  } /* !idx */
  
  (void)cast_nctype_void(NC_INT,&(var_row->val));

  row_bin=(int*)nco_free(row_bin);

  return True;
} /* !nco_map_hst_mk() */

var_sct * /* [sct] Variable structure */
nco_map_var_init /* Fill-in variable or return NULL */
(int in_id,
 char * var_nm,
 dmn_sct **dmn,
 int dmn_in_nbr)
{
  const char fnc_nm[]="nco_map_var_init()"; /* [sng] Function name */
  int rcd;
  int var_id;
  var_sct *var;

  /* Get variable ID */
  rcd=nco_inq_varid_flg(in_id,var_nm,&var_id);
  /* 20191118 Global (all?) Tempest map-files lack mask_a/b variables */
  if(rcd != NC_NOERR){
    if(!strcmp(var_nm,"S")){
      (void)fprintf(stderr,"%s: ERROR %s cannot find variable \"S\", a required variable in supported map-files\n",nco_prg_nm_get(),fnc_nm);
      (void)fprintf(stderr,"%s: HINT ncks --chk_map works on map-files that contain sparse array information in the ESMF/CMIP6-format used by CESM, E3SM, CMIP6, and other projects. The map-file checker does not work on Exodus (\".g\") or SCRIP grid-files or overlap/interface mesh-files. The original SCRIP (not ESMF) format names the weight variable \"remap_matrix\" instead of \"S\". The NCO regridder can employ such SCRIP map-files, though the map-file checker does not yet support them. Please contact the NCO project if supporting SCRIP map-file checking would be useful to you.\n",nco_prg_nm_get());
      nco_exit(EXIT_FAILURE); 
    }else{
      return NULL;
    } /* !S */
  } /* !rcd */
    
  /* Fill-in dimensions */
  var=nco_var_fll(in_id,var_id,var_nm,dmn,dmn_in_nbr);
  /* Read data */
  (void)nco_var_get(in_id,var);

  return var;
} /* !nco_map_var_init() */

void
nco_map_var_min_max_ttl
(var_sct *var,
 double *area,
 nco_bool flg_area_wgt,
 double *min,
 size_t *idx_min,
 double *max,
 size_t *idx_max,
 double *ttl,
 double *avg,
 double *mebs,
 double *rms,
 double *sdn)
{
  const double one=1.0;
  const size_t sz=var->sz;
  double *area_lcl=NULL;
  double area_ttl=0.0;
  int idx=0;

  area_lcl=(double *)nco_malloc(sz*sizeof(double));
  if(flg_area_wgt && area != NULL)
    (void)memcpy(area_lcl,area,sz*sizeof(double));
  else
    for(idx=0;idx<sz;idx++) area_lcl[idx]=1.0;
  
  for(idx=0;idx<sz;idx++) area_ttl+=area_lcl[idx];

  if(var->type == NC_DOUBLE){

    double dval=0.0;
    double min_dp=NC_MAX_DOUBLE;
    double max_dp=NC_MIN_DOUBLE;
    double ttl_dp=0.0;
    double avg_dp=0.0;
    double mebs_dp=0.0;
    double rms_dp=0.0;
    double sdn_dp=0.0;
    
    (void)cast_void_nctype(NC_DOUBLE,&(var->val));
    for(idx=0;idx<sz;idx++){
      /* De-reference */
      dval=var->val.dp[idx];
      ttl_dp+=dval*area_lcl[idx];
      mebs_dp+=fabs(dval-one)*area_lcl[idx];
      rms_dp+=(dval-one)*(dval-one)*area_lcl[idx];
      if(dval < min_dp){
	min_dp=dval;
	*idx_min=idx;
      } /* !dval */
      if(dval > max_dp){
	max_dp=dval;
	*idx_max=idx;
      } /* !dval */
    } /* !idx */
    avg_dp=ttl_dp/area_ttl;
    mebs_dp/=area_ttl;
    rms_dp=sqrt(rms_dp/area_ttl);
    for(idx=0;idx<sz;idx++){
      dval=var->val.dp[idx];
      sdn_dp+=(dval-avg_dp)*(dval-avg_dp);
    } /* !idx */
    sdn_dp=sqrt(sdn_dp/(sz-1ULL));
    (void)cast_nctype_void(NC_DOUBLE,&(var->val));

    *min=min_dp;
    *max=max_dp;
    *ttl=ttl_dp;
    *avg=avg_dp;
    *mebs=mebs_dp;
    *rms=rms_dp;
    *sdn=sdn_dp;

  }else if(var->type == NC_INT){

    int ival=0;
    int min_ip=NC_MAX_INT;
    int max_ip=NC_MIN_INT;
    double ttl_dp=0.0;
    double avg_dp=0.0;
    double mebs_dp=0.0;
    double rms_dp=0.0;
    double sdn_dp=0.0;

    (void)cast_void_nctype(NC_INT,&(var->val));
    for(idx=0;idx<sz;idx++){
	/* De-reference */
	ival=var->val.ip[idx];
	ttl_dp+=ival;
	mebs_dp+=fabs((double)ival-one);
	rms_dp+=((double)ival-one)*((double)ival-one);
	if(ival < min_ip){
	  min_ip=ival;
	  *idx_min=idx;
	} /* !ival */
	if(ival > max_ip){
	  max_ip=ival;
	  *idx_max=idx;
	} /* !ival */
    } /* !idx */
    avg_dp=ttl_dp/sz;
    mebs_dp/=sz;
    rms_dp=sqrt(rms_dp/sz);
    for(idx=0;idx<sz;idx++){
      ival=var->val.ip[idx];
      sdn_dp+=((double)ival-avg_dp)*((double)ival-avg_dp);
    } /* !idx */
    sdn_dp=sqrt(sdn_dp/(sz-1ULL));
    (void)cast_nctype_void(NC_INT,&(var->val));

    *min=(double)min_ip;
    *max=(double)max_ip;
    *ttl=ttl_dp;
    *avg=avg_dp;
    *mebs=mebs_dp;
    *rms=rms_dp;
    *sdn=sdn_dp;

  } /* !NC_INT */
  
  if(area_lcl) area_lcl=(double *)nco_free(area_lcl);

  return;
} /* !nco_map_var_min_max_ttl() */

nco_bool
nco_map_frac_b_clc /* Compute frac_b as row sums of the weight matrix S */
(var_sct *var_S,
 var_sct *var_row,
 var_sct *var_frac_b)
{
  /* Purpose: Compute frac_b as row sums of the weight matrix S */
  int idx_row;

  size_t idx;
  size_t sz;

  (void)cast_void_nctype(NC_DOUBLE,&(var_S->val));
  (void)cast_void_nctype(NC_INT,&(var_row->val));
  (void)cast_void_nctype(NC_DOUBLE,&(var_frac_b->val));

  /* Set computed frac_b to zero */
  memset(var_frac_b->val.dp,0,var_frac_b->sz*nco_typ_lng(var_frac_b->type));

  /* Perform row-sum of weights for each row */
  sz=var_S->sz;
  for(idx=0;idx<sz;idx++){
    /* var_row is one-based, guard against bogus row-indices, add weight to appropriate row */
    if((idx_row=var_row->val.ip[idx]-1) >= var_frac_b->sz) continue;
    var_frac_b->val.dp[idx_row]+=var_S->val.dp[idx];
  } /* !idx */

  (void)cast_nctype_void(NC_DOUBLE,&(var_S->val));
  (void)cast_nctype_void(NC_INT,&(var_row->val));
  (void)cast_nctype_void(NC_DOUBLE,&(var_frac_b->val));
  
  return True;
} /* !nco_map_frac_b_clc() */

nco_bool
nco_map_frac_a_clc /* Compute frac_a as area_b-weighted column sums of the weight matrix S normalized by area_a */
(var_sct *var_S,
 var_sct *var_row,
 var_sct *var_col,
 var_sct *var_area_a,
 var_sct *var_area_b,
 var_sct *var_frac_a)
{
  /* Purpose: Compute frac_a as area_b-weighted column sums of the weight matrix S normalized by area_a */

  //char fnc_nm[]="nco_map_frac_a_clc()";
  int idx_row;
  int idx_col;
  size_t cnt_zro; /* [nbr] Number of valid areas in Grid A */
  size_t idx;
  size_t sz;

  (void)cast_void_nctype(NC_DOUBLE,&(var_S->val));
  (void)cast_void_nctype(NC_INT,&(var_row->val));
  (void)cast_void_nctype(NC_INT,&(var_col->val));
  (void)cast_void_nctype(NC_DOUBLE,&(var_area_a->val));
  (void)cast_void_nctype(NC_DOUBLE,&(var_area_b->val));
  (void)cast_void_nctype(NC_DOUBLE,&(var_frac_a->val));

  /* Set frac_a to zero */
  memset(var_frac_a->val.dp,0,var_frac_a->sz*nco_typ_lng(var_frac_a->type));
  
  sz=var_S->sz;
  for(idx=0;idx<sz;idx++){
    /* var_row and var_dol are one-based, guard against bogus row-indices, add area_b-weighted weight to appropriate column */
    idx_row=var_row->val.ip[idx]-1L;
    idx_col=var_col->val.ip[idx]-1L;
    if(idx_row >= var_area_b->sz || idx_col >= var_area_a->sz) continue;
    var_frac_a->val.dp[idx_col]+=var_S->val.dp[idx]*var_area_b->val.dp[idx_row];
  } /* !idx */

  /* Sum of area_b-weighted weights should equal area_a (for complete overlap)
     Normalize result by area_a to create a metric whose ideal value is 1.0 */
  sz=var_frac_a->sz;
  cnt_zro=0L;
  for(idx=0;idx<sz;idx++)
    if(var_area_a->val.dp[idx] == 0.0) cnt_zro++;

  if(cnt_zro == sz){
    (void)fprintf(stdout,"INFO area_a = 0.0 or is undefined for all cells in Grid A. This is legal (e.g., for bilinear maps) though it prevents computing the frac_a statistic. Instead, frac_a will be reported as 0.0 for all cells. These frac_a statistics may be safely ignored.\n");
  }else{
    for(idx=0;idx<sz;idx++){
      if(var_area_a->val.dp[idx] != 0.0) var_frac_a->val.dp[idx]/=var_area_a->val.dp[idx]; else
	(void)fprintf(stdout,"WARNING area_a = %g for grid A cell %lu: Unable to normalize area_b-weighted column sum to compute frac_a for this gridcell\n",var_area_a->val.dp[idx],idx+1UL);
    } /* !idx */
  }  /* !cnt_zro */
    
  (void)cast_nctype_void(NC_DOUBLE,&(var_S->val));
  (void)cast_nctype_void(NC_INT,&(var_row->val));
  (void)cast_nctype_void(NC_INT,&(var_col->val));
  (void)cast_nctype_void(NC_DOUBLE,&(var_area_a->val));
  (void)cast_nctype_void(NC_DOUBLE,&(var_area_b->val));
  (void)cast_nctype_void(NC_DOUBLE,&(var_frac_a->val));

   return True;
} /* !nco_map_frac_a_clc() */

nco_bool
nco_map_chk /* Map-file evaluation */
(const char *fl_in,
 nco_bool flg_area_wgt, /* [flg] Area-weight map-file statistics */
 nco_bool flg_frac_b_nrm) /* [flg] Normalize map-file weights when frac_b >> 1 */
{
  /* Purpose: Perform high-level evaluation of map-file */

  char fnc_nm[]="nco_map_chk()";
  char idx_sng_fmt[5];
  char *idx_sng=NULL;
  
  char dmn_nm[NC_MAX_NAME];

  dmn_sct **dmn_in=NULL_CEWI;
  
  double *val=NULL;
  double wgt;
  
  int *ival=int_CEWI;

  int dmn_in_nbr;
  int fl_in_fmt;
  int idx_sng_lng_max;
  int in_id;
  int hst_sz_nnz;
  int rcd;

  nco_bool area_wgt_a;
  nco_bool area_wgt_b;
  nco_bool grid_a_tiles_sphere=False;
  nco_bool grid_b_tiles_sphere=False;
  nco_bool has_area_a=False;
  nco_bool has_area_b=False;
  nco_bool has_frac_a=False;
  nco_bool has_frac_b=False;
  nco_bool has_mask_a=False;
  nco_bool has_mask_b=False;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT;
  size_t idx;
  size_t idx_min;
  size_t idx_max;
  size_t sz;

  


  var_sct *var_S=NULL_CEWI;
  var_sct *var_area_a=NULL_CEWI;
  var_sct *var_area_b=NULL_CEWI;
  var_sct *var_col=NULL_CEWI;
  var_sct *var_frac_a=NULL_CEWI;
  var_sct *var_frac_b=NULL_CEWI;
  var_sct *var_mask_a=NULL_CEWI;
  var_sct *var_mask_b=NULL_CEWI;
  var_sct *var_row=NULL_CEWI;
  var_sct *var_xc_a=NULL_CEWI;
  var_sct *var_xc_b=NULL_CEWI;
  var_sct *var_yc_a=NULL_CEWI;
  var_sct *var_yc_b=NULL_CEWI;

  area_wgt_a=flg_area_wgt;
  area_wgt_b=flg_area_wgt;

  if(flg_frac_b_nrm) rcd=nco_fl_open(fl_in,NC_WRITE,&bfr_sz_hnt,&in_id); else rcd=nco_fl_open(fl_in,NC_NOWRITE,&bfr_sz_hnt,&in_id);
  (void)nco_inq_format(in_id,&fl_in_fmt);

  /* Read all dimensions from file */
  (void)nco_inq(in_id,&dmn_in_nbr,(int *)NULL,(int *)NULL,(int *)NULL);
  dmn_in=(dmn_sct **)nco_malloc(dmn_in_nbr*sizeof(dmn_sct *));

  for(idx=0;idx<dmn_in_nbr;idx++){
    (void)nco_inq_dimname(in_id,idx,dmn_nm);
    dmn_in[idx]=nco_dmn_fll(in_id,idx,dmn_nm);
  } /* !idx */

  var_S=nco_map_var_init(in_id,"S",dmn_in,dmn_in_nbr);
  var_area_a=nco_map_var_init(in_id,"area_a",dmn_in,dmn_in_nbr);
  var_area_b=nco_map_var_init(in_id,"area_b",dmn_in,dmn_in_nbr);
  var_col=nco_map_var_init(in_id,"col",dmn_in,dmn_in_nbr);
  var_frac_a=nco_map_var_init(in_id,"frac_a",dmn_in,dmn_in_nbr);
  var_frac_b=nco_map_var_init(in_id,"frac_b",dmn_in,dmn_in_nbr);
  var_mask_a=nco_map_var_init(in_id,"mask_a",dmn_in,dmn_in_nbr);
  var_mask_b=nco_map_var_init(in_id,"mask_b",dmn_in,dmn_in_nbr);
  var_row=nco_map_var_init(in_id,"row",dmn_in,dmn_in_nbr);
  var_xc_a=nco_map_var_init(in_id,"xc_a",dmn_in,dmn_in_nbr);
  var_xc_b=nco_map_var_init(in_id,"xc_b",dmn_in,dmn_in_nbr);
  var_yc_a=nco_map_var_init(in_id,"yc_a",dmn_in,dmn_in_nbr);
  var_yc_b=nco_map_var_init(in_id,"yc_b",dmn_in,dmn_in_nbr);

  /* Type conversions */
  if(var_area_a->type != NC_DOUBLE) var_area_a=nco_var_cnf_typ(NC_DOUBLE,var_area_a);
  if(var_area_b->type != NC_DOUBLE) var_area_b=nco_var_cnf_typ(NC_DOUBLE,var_area_b);
  if(var_frac_a->type != NC_DOUBLE) var_frac_a=nco_var_cnf_typ(NC_DOUBLE,var_frac_a);
  if(var_frac_b->type != NC_DOUBLE) var_frac_b=nco_var_cnf_typ(NC_DOUBLE,var_frac_b);
  if(var_mask_a && var_mask_a->type != NC_INT) var_mask_a=nco_var_cnf_typ(NC_INT,var_mask_a);
  if(var_mask_b && var_mask_b->type != NC_INT) var_mask_b=nco_var_cnf_typ(NC_INT,var_mask_b);

  /* Odd cells */
  size_t wgt_zro_nbr;
  wgt_zro_nbr=0UL;
  val=var_S->val.dp;
  for(idx=0;idx<var_S->sz;idx++)
    if(val[idx] == 0.0) wgt_zro_nbr++;

  /* Turn-off requested area weighting for any grid with any zero areas
     ESMF bilinear grids, for example, often have area fields filled with zeros */
  if(var_area_a){
    has_area_a=True;
    sz=var_area_a->sz;
    val=var_area_a->val.dp;
    for(idx=0;idx<sz;idx++)
      if(val[idx] == 0.0) break;
    if(idx < sz) area_wgt_a=False;
    if(idx < sz) has_area_a=False;
    if(idx < sz) fprintf(stdout,"WARNING area_a = %g for grid A cell [%lu,%+g,%+g] (and possibly others). Empty areas are legal (e.g., for bilinear maps) yet prevent calculation of some diagnostics.\n",val[idx],idx+1UL,var_yc_a->val.dp[idx],var_xc_a->val.dp[idx]);
  } /* !var_area_a */
  if(var_area_b){
    has_area_b=True;
    sz=var_area_b->sz;
    val=var_area_b->val.dp;
    for(idx=0;idx<sz;idx++)
      if(val[idx] == 0.0) break;
    if(idx < sz) area_wgt_b=False;
    if(idx < sz) has_area_b=False;
    if(idx < sz) fprintf(stdout,"WARNING area_b = %g for grid B cell [%lu,%+g,%+g] (and possibly others). Empty areas are legal (e.g., for bilinear maps) yet prevent calculation of some diagnostics.\n",val[idx],idx+1UL,var_yc_b->val.dp[idx],var_xc_b->val.dp[idx]);
  } /* !var_area_b */
  if(var_frac_a){
    has_frac_a=True;
    sz=var_frac_a->sz;
    val=var_frac_a->val.dp;
    /* frac_a must have at least one non-zero value */
    for(idx=0;idx<sz;idx++)
      if(val[idx] != 0.0) break;
    if(idx == sz) has_frac_a=False;
  } /* !var_frac_a */
  if(var_frac_b){
    has_frac_b=True;
    sz=var_frac_b->sz;
    val=var_frac_b->val.dp;
    /* frac_b must have at least one non-zero value */
    for(idx=0;idx<sz;idx++)
      if(val[idx] != 0.0) break;
    if(idx == sz) has_frac_a=False;
  } /* !var_frac_b */
  if(var_mask_a) has_mask_a=True;
  if(var_mask_b) has_mask_b=True;

  size_t mask_a_one=0UL;
  size_t mask_a_zro=0UL;
  if(has_mask_a){
    sz=var_mask_a->sz;
    ival=var_mask_a->val.ip;
    for(idx=0;idx<sz;idx++)
      if(ival[idx] == 0) mask_a_zro++; else if(ival[idx] == 1) mask_a_one++;
    /* Source mask must have at least one unmasked value, i.e, one value of 1 */
    //assert(mask_a_zro != sz);
    if(mask_a_zro == sz) (void)fprintf(stdout,"WARNING mask_a has no unmasked values, i.e., has no 1's. This is possibly a bookkeeping issue with the map generator for this algorithm (because the generator would have failed outright if there were truly no valid source gridcells) and not itself an issue with the weights, which may well still be valid and usable. 20200901: this is a known issue with the NCO DWE algorithm that will soon be fixed. Also, the other WARNINGs below can probably be ignored for DWE maps. These warnings will be quieted in a future version.\n");
  } /* !var_mask_a */

  size_t mask_b_one=0UL;
  size_t mask_b_zro=0UL;
  if(has_mask_b){
    sz=var_mask_b->sz;
    ival=var_mask_b->val.ip;
    for(idx=0;idx<sz;idx++)
      if(ival[idx] == 0) mask_b_zro++; else if(ival[idx] == 1) mask_b_one++;
    /* Destination mask must have at least one unmasked value, i.e, one value of 1 */
    //assert(mask_b_zro != sz);
    if(mask_b_zro == sz) (void)fprintf(stdout,"WARNING mask_b has no unmasked values, i.e., has no 1's. This is possibly a bookkeeping issue with the map generator for this algorithm (because the generator would have failed outright if there were truly no valid source gridcells) and not itself an issue with the weights, which may well still be valid and usable. 20200901: this is a known issue with the NCO DWE algorithm that will soon be fixed. Also, the other WARNINGs below can probably be ignored for DWE maps. These warnings will be quieted in a future version.\n");
  } /* !var_mask_b */
  
  /* Start Report in own scope */
  {
    const double eps_abs=5.0e-16;
    double area_a_min,area_a_max,area_a_ttl;
    double area_b_min,area_b_max,area_b_ttl;
    double avg,mebs,rms,sdn;
    double col_min,col_max,col_ttl;
    double frac_avg_cmp,frac_min_cmp,frac_max_cmp,frac_ttl_cmp;
    double frac_avg_dsk,frac_min_dsk,frac_max_dsk,frac_ttl_dsk;
    double mask_a_min,mask_a_max,mask_a_ttl;
    double mask_b_min,mask_b_max,mask_b_ttl;
    double row_min,row_max,row_ttl;
    double s_min,s_max,s_ttl;

    nco_map_var_min_max_ttl(var_col,(double *)NULL,flg_area_wgt,&col_min,&idx_min,&col_max,&idx_max,&col_ttl,&avg,&mebs,&rms,&sdn);
    nco_map_var_min_max_ttl(var_row,(double *)NULL,flg_area_wgt,&row_min,&idx_min,&row_max,&idx_max,&row_ttl,&avg,&mebs,&rms,&sdn);

    if(col_min < 1){fprintf(stdout,"WARNING: minimum column index < 1\n");}
    else if(col_max > var_area_a->sz){fprintf(stdout,"WARNING: maximum col index > n_a\n");}
    if(row_min < 1){fprintf(stdout,"WARNING: minimum row index < 1\n" );}
    else if(row_max > var_area_b->sz){fprintf(stdout,"WARNING: maximum row index > n_b\n");}
    
    int hst_sz=31;
    int *hst_row;
    int *hst_col;
    hst_row=(int*)nco_calloc(hst_sz+1,sizeof(int));
    hst_col=(int*)nco_calloc(hst_sz+1,sizeof(int));
    nco_map_hst_mk(var_col,var_area_a->sz,hst_col,hst_sz);
    nco_map_hst_mk(var_row,var_area_b->sz,hst_row,hst_sz);

    if(has_area_a) nco_map_var_min_max_ttl(var_area_a,(double *)NULL,flg_area_wgt,&area_a_min,&idx_min,&area_a_max,&idx_max,&area_a_ttl,&avg,&mebs,&rms,&sdn);
    if(var_mask_a) nco_map_var_min_max_ttl(var_mask_a,(double *)NULL,flg_area_wgt,&mask_a_min,&idx_min,&mask_a_max,&idx_max,&mask_a_ttl,&avg,&mebs,&rms,&sdn);

    fprintf(stdout,"Characterization of map-file %s\n",fl_in);
    fprintf(stdout,"Cell triplet elements : [Fortran (1-based) index, center latitude, center longitude]\n");
    fprintf(stdout,"Sparse-matrix size n_s: %lu\n",var_S->sz);
    nco_map_var_min_max_ttl(var_S,(double *)NULL,flg_area_wgt,&s_min,&idx_min,&s_max,&idx_max,&s_ttl,&avg,&mebs,&rms,&sdn);
    idx_sng_lng_max=(long)ceil(log10((double)var_S->sz));
    if(idx_sng_lng_max == (long)log10((double)var_S->sz)) idx_sng_lng_max++;
    (void)sprintf(idx_sng_fmt,"%%%dlu",idx_sng_lng_max);
    idx_sng=(char *)nco_malloc((idx_sng_lng_max+1UL)*sizeof(char));
    (void)sprintf(idx_sng,idx_sng_fmt,idx_min+1UL);
    //(void)fprintf(stdout,"idx_sng_fmt = %s, idx_sng = %s\n",idx_sng_fmt,idx_sng);
    fprintf(stdout,"Weight min S(%s): % 0.16e from cell [%d,%+g,%+g] to [%d,%+g,%+g]\n",idx_sng,s_min,var_col->val.ip[idx_min],var_yc_a->val.dp[var_col->val.ip[idx_min]-1],var_xc_a->val.dp[var_col->val.ip[idx_min]-1],var_row->val.ip[idx_min],var_yc_b->val.dp[var_row->val.ip[idx_min]-1],var_xc_b->val.dp[var_row->val.ip[idx_min]-1]);
    (void)sprintf(idx_sng,idx_sng_fmt,idx_max+1UL);
    fprintf(stdout,"Weight max S(%s): % 0.16e from cell [%d,%+g,%+g] to [%d,%+g,%+g]\n",idx_sng,s_max,var_col->val.ip[idx_max],var_yc_a->val.dp[var_col->val.ip[idx_max]-1],var_xc_a->val.dp[var_col->val.ip[idx_max]-1],var_row->val.ip[idx_max],var_yc_b->val.dp[var_row->val.ip[idx_max]-1],var_xc_b->val.dp[var_row->val.ip[idx_max]-1]);
    fprintf(stdout,"Ignored weights (S=0.0): %ld\n",wgt_zro_nbr);
    if(nco_dbg_lvl_get() >= nco_dbg_std){
      fprintf(stdout,"Commands to examine extrema:\n");
      fprintf(stdout,"min(S): ncks --fortran -H --trd -d n_s,%lu -d n_a,%d -d n_b,%d -v S,row,col,.?_a,.?_b %s\n",idx_min+1UL,var_col->val.ip[idx_min],var_row->val.ip[idx_min],fl_in);
      fprintf(stdout,"max(S): ncks --fortran -H --trd -d n_s,%lu -d n_a,%d -d n_b,%d -v S,row,col,.?_a,.?_b %s\n",idx_max+1UL,var_col->val.ip[idx_max],var_row->val.ip[idx_max],fl_in);
    } /* !dbg */
    /* Check for and report NaNs in weight array */
    sz=var_S->sz;
    val=var_S->val.dp;
    /* Print NaN locations in weight array */
    for(idx=0;idx<sz;idx++){
      if(isnan(val[idx])){
	(void)sprintf(idx_sng,idx_sng_fmt,idx+1UL);
	if(nco_dbg_lvl_get() >= nco_dbg_quiet) fprintf(stdout,"WARNING: Weight S(%s) = NaN from cell [%d,%+g,%+g] to [%d,%+g,%+g]\n",idx_sng,var_col->val.ip[idx],var_yc_a->val.dp[var_col->val.ip[idx]-1],var_xc_a->val.dp[var_col->val.ip[idx]-1],var_row->val.ip[idx],var_yc_b->val.dp[var_row->val.ip[idx]-1],var_xc_b->val.dp[var_row->val.ip[idx]-1]);
      } /* !isnan */
    } /* !idx */
    fprintf(stdout,"\n");
    if(idx_sng) idx_sng=(char *)nco_free(idx_sng);
      
    int hst_idx;
    int *hst_wgt;
    double hst_wgt_ntf[]={NC_MIN_INT,-1,0,1,2,10,NC_MAX_INT};
    const int hst_wgt_nbr=-1+sizeof(hst_wgt_ntf)/sizeof(double);
    hst_wgt_ntf[0]=s_min;
    hst_wgt_ntf[hst_wgt_nbr]=s_max;
    hst_wgt=(int *)nco_calloc(hst_wgt_nbr,sizeof(int));
    sz=var_S->sz;
    for(idx=0;idx<sz;idx++){
      wgt=var_S->val.dp[idx];
      for(hst_idx=0;hst_idx<hst_wgt_nbr;hst_idx++){
	if(wgt <= hst_wgt_ntf[hst_idx+1]){
	  hst_wgt[hst_idx]++;
	  break;
	} /* !wgt */
      } /* !hst_idx */
    } /* !idx */
    
    fprintf(stdout,"Grid A size n_a: %lu // Number of columns/sources\n",var_area_a->sz);
    if(var_mask_a) fprintf(stdout,"mask_a 0's, 1's: %lu, %lu\n",mask_a_zro,mask_a_one);
    if(var_mask_a) fprintf(stdout,"mask_a min, max: %.0f, %.0f\n",mask_a_min,mask_a_max); else fprintf(stdout,"mask_a min, max: map-file omits mask_a\n");
    if(has_area_a){
      fprintf(stdout,"area_a sum/4*pi: %0.16f = 1.0%s%0.1e // Perfect is 1.0 for global Grid A\n",area_a_ttl/4.0/M_PI,area_a_ttl/4.0/M_PI > 1 ? "+" : "-",fabs(1.0-area_a_ttl/4.0/M_PI));
      fprintf(stdout,"area_a min, max: %0.16e, %0.16e\n",area_a_min,area_a_max);
      if(fabs(1.0-area_a_ttl/4.0/M_PI) < 1.0e-2) grid_a_tiles_sphere=True;
    }else{
      fprintf(stdout,"area_a sum/4*pi: map-file does not provide completely non-zero area_a\n");
      fprintf(stdout,"area_a min, max: map-file does not provide completely non-zero area_a\n");
    } /* !has_area_a */
    fprintf(stdout,"Column (source cell) indices utilized min, max: %.0f, %.0f\n",col_min,col_max);
    fprintf(stdout,"Ignored source cells (empty columns): %d\n\n",hst_col[0]);

    if(has_area_b) nco_map_var_min_max_ttl(var_area_b,(double *)NULL,flg_area_wgt,&area_b_min,&idx_min,&area_b_max,&idx_max,&area_b_ttl,&avg,&mebs,&rms,&sdn);
    if(var_mask_b) nco_map_var_min_max_ttl(var_mask_b,(double *)NULL,flg_area_wgt,&mask_b_min,&idx_min,&mask_b_max,&idx_max,&mask_b_ttl,&avg,&mebs,&rms,&sdn);

    fprintf(stdout,"Grid B size n_b: %lu // Number of rows/destinations\n",var_area_b->sz);
    if(var_mask_b) fprintf(stdout,"mask_b 0's, 1's: %lu, %lu\n",mask_b_zro,mask_b_one);
    if(var_mask_b) fprintf(stdout,"mask_b min, max: %.0f, %.0f\n",mask_b_min,mask_b_max); else fprintf(stdout,"mask_b min, max: map-file omits mask_b\n");
    if(has_area_b){
      fprintf(stdout,"area_b sum/4*pi: %0.16f = 1.0%s%0.1e // Perfect is 1.0 for global Grid B\n",area_b_ttl/4.0/M_PI,area_b_ttl/4.0/M_PI > 1 ? "+" : "-",fabs(1.0-area_b_ttl/4.0/M_PI));
      fprintf(stdout,"area_b min, max: %0.16e, %0.16e\n",area_b_min,area_b_max);
      if(fabs(1.0-area_b_ttl/4.0/M_PI) < 1.0e-2) grid_b_tiles_sphere=True;
    }else{
      fprintf(stdout,"area_b sum/4*pi: map-file does not provide completely non-zero area_b\n");
      fprintf(stdout,"area_b min, max: map-file does not provide completely non-zero area_b\n");
    } /* !has_area_b */
    fprintf(stdout,"Row (destination cell) indices utilized min, max: %.0f, %.0f\n",row_min,row_max);
    fprintf(stdout,"Ignored destination cells (empty rows): %d\n\n",hst_row[0]);

    /* Compute frac_a statistics from frac_a disk values */
    if(has_frac_a) nco_map_var_min_max_ttl(var_frac_a,var_area_a->val.dp,area_wgt_a,&frac_min_dsk,&idx_min,&frac_max_dsk,&idx_max,&frac_ttl_dsk,&frac_avg_dsk,&mebs,&rms,&sdn);
    /* Compute and report frac_a as area_b-weighted column sums/area_a */
    nco_map_frac_a_clc(var_S,var_row,var_col,var_area_a,var_area_b,var_frac_a);
    nco_map_var_min_max_ttl(var_frac_a,var_area_a->val.dp,area_wgt_a,&frac_min_cmp,&idx_min,&frac_max_cmp,&idx_max,&frac_ttl_cmp,&frac_avg_cmp,&mebs,&rms,&sdn);
    
    /* Ignore frac_a values when area_a or area_b are all invalid or zero */
    if(!has_area_a || !has_area_b) fprintf(stdout,"HINT: The following frac_a metrics may be safely ignored because either or both area_a and area_b are everywhere undefined or zero\n");
    fprintf(stdout,"Conservation metrics (column-sums of area_b-weighted weights normalized by area_a) and errors\nPerfect metrics for global Grid B are avg = min = max = 1.0, mbs = rms = sdn = 0.0:\n");
    fprintf(stdout,"frac_a avg: %0.16f = 1.0%s%0.1e // %sean\n",frac_avg_cmp,frac_avg_cmp > 1 ? "+" : "-",fabs(1.0-frac_avg_cmp),area_wgt_a ? "Area-weighted m" : "M");
    fprintf(stdout,"frac_a min: %0.16f = 1.0%s%0.1e // Minimum in grid A cell [%lu,%+g,%+g]\n",frac_min_cmp,frac_min_cmp > 1 ? "+" : "-",fabs(1.0-frac_min_cmp),idx_min+1UL,var_yc_a->val.dp[idx_min],var_xc_a->val.dp[idx_min]);
    fprintf(stdout,"frac_a max: %0.16f = 1.0%s%0.1e // Maximum in grid A cell [%lu,%+g,%+g]\n",frac_max_cmp,frac_max_cmp > 1 ? "+" : "-",fabs(1.0-frac_max_cmp),idx_max+1UL,var_yc_a->val.dp[idx_max],var_xc_a->val.dp[idx_max]);
    fprintf(stdout,"frac_a mbs: %0.16f =     %0.1e // %sean absolute bias from 1.0\n",mebs,mebs,area_wgt_a ? "Area-weighted m" : "M");
    fprintf(stdout,"frac_a rms: %0.16f =     %0.1e // %sRMS relative to 1.0\n",rms,rms,area_wgt_a ? "Area-weighted " : "");
    fprintf(stdout,"frac_a sdn: %0.16f =     %0.1e // Standard deviation\n",sdn,sdn);

    /* Inform/Warn if difference between disk and computed values */
    double cmp_dsk_dff;
    if(has_frac_a){
      cmp_dsk_dff=frac_min_cmp-frac_min_dsk;
      if(fabs(cmp_dsk_dff) > eps_abs){fprintf(stdout,"%s: Computed (as area_b-weighted column sums/area_a) and disk-values of min(frac_a) disagree by more than %0.1e:\n  %0.16f - %0.16f = %g\n",fabs(cmp_dsk_dff) < 10*eps_abs ? "INFO" : "WARNING",eps_abs,frac_min_cmp,frac_min_dsk,cmp_dsk_dff);}
      cmp_dsk_dff=frac_max_cmp-frac_max_dsk;
      if(fabs(cmp_dsk_dff) > eps_abs){fprintf(stdout,"%s: Computed (as area_b-weighted column sums/area_a) and disk-values of max(frac_a) disagree by more than %0.1e:\n  %0.16f - %0.16f = %g\n",fabs(cmp_dsk_dff) < 10*eps_abs ? "INFO" : "WARNING",eps_abs,frac_max_cmp,frac_max_dsk,cmp_dsk_dff);}
    } /* !has_frac_a */
    
    const double eps_max_wrn=1.0e-1; /* [frc] Maximum error in column-sum/row-sums before WARNING is printed */
    size_t cnt_zro; /* [nbr] Number of valid areas in Grid A */
    sz=var_frac_a->sz;
    cnt_zro=0L;
    for(idx=0;idx<sz;idx++)
      if(var_area_a->val.dp[idx] == 0.0) cnt_zro++;

    if(has_area_a && has_area_b)
      if(fabs(frac_max_cmp-1.0) > eps_max_wrn || (grid_b_tiles_sphere && (fabs(frac_min_cmp-1.0) > eps_max_wrn))) fprintf(stdout,"\nWARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING\n\tDanger, Will Robinson! max(frac_a) or min(frac_a) error exceeds %0.1e\n\tRegridding with these embarrassing weights will produce funny results\n\tSuggest re-generating weights with a better algorithm/weight-generator\n\tHave both input grid-files been validated? If not, one might be barmy\nWARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING\n\n",eps_max_wrn);
    
    if(nco_dbg_lvl_get() >= nco_dbg_std){
      sz=var_frac_a->sz;
      val=var_frac_a->val.dp;
      int wrn_nbr=0; // [nbr] Number of warnings
      const double eps_err=1.0e-8;
      for(idx=0;idx<sz;idx++){
	if(!has_mask_a || (has_mask_a && var_mask_a->val.ip[idx] == 1)){
	  if((val[idx]-1.0 > eps_err) || (grid_b_tiles_sphere && (fabs(val[idx]-1.0) > eps_err))){
	    if(nco_dbg_lvl_get() >= nco_dbg_fl) fprintf(stdout,"WARNING conservation = %0.16f = 1.0%s%0.1e for grid A cell [%lu,%+g,%+g]\n",val[idx],val[idx] > 1 ? "+" : "-",fabs(1.0-val[idx]),idx+1UL,var_yc_a->val.dp[idx],var_xc_a->val.dp[idx]);
	    wrn_nbr++;
	  } /* !err */
	} /* !msk */
      } /* !idx */
      if(wrn_nbr > 0) fprintf(stdout,"WARNING non-conservative weighted column-sums (error exceeds tolerance = %0.1e) for %d of %lu grid A cells\nNB: conservation WARNINGS may be safely ignored for Grid A cells not completely overlapped with unmasked Grid B cells (e.g., coastlines)\nThese diagnostics imperfectly attempt to rule-out such false-positive WARNINGs\nTrue-positive WARNINGs occur in source gridcells that this map under-extracts (error < 0) or over-extracts (error > 0)\n\n",eps_err,wrn_nbr,var_area_a->sz);
    } /* !dbg */
    
    if(nco_dbg_lvl_get() >= nco_dbg_std){
      fprintf(stdout,"Commands to examine conservation extrema:\n");
      fprintf(stdout,"min(frac_a): ncks --fortran -H --trd -d n_a,%lu -v .?_a %s\n",idx_min+1UL,fl_in);
      fprintf(stdout,"max(frac_a): ncks --fortran -H --trd -d n_a,%lu -v .?_a %s\n",idx_max+1UL,fl_in);
    } /* !dbg */

    /* Compute frac_b statistics from frac_b disk values */
    if(has_frac_b) nco_map_var_min_max_ttl(var_frac_b,var_area_b->val.dp,area_wgt_b,&frac_min_dsk,&idx_min,&frac_max_dsk,&idx_max,&frac_ttl_dsk,&frac_avg_dsk,&mebs,&rms,&sdn);
    /* Compute and report frac_b as row sums */
    nco_map_frac_b_clc(var_S,var_row,var_frac_b);
    nco_map_var_min_max_ttl(var_frac_b,var_area_b->val.dp,area_wgt_b,&frac_min_cmp,&idx_min,&frac_max_cmp,&idx_max,&frac_ttl_cmp,&frac_avg_cmp,&mebs,&rms,&sdn);

    fprintf(stdout,"\n");
    fprintf(stdout,"Consistency metrics (row-sums of weights) and errors\nPerfect metrics for global Grid A are avg = min = max = 1.0, mbs = rms = sdn = 0.0:\n");
    fprintf(stdout,"frac_b avg: %0.16f = 1.0%s%0.1e // %sean\n",frac_avg_cmp,frac_avg_cmp > 1 ? "+" : "-",fabs(1.0-frac_avg_cmp),area_wgt_b ? "Area-weighted m" : "M");
    fprintf(stdout,"frac_b min: %0.16f = 1.0%s%0.1e // Minimum in grid B cell [%lu,%+g,%+g]\n",frac_min_cmp,frac_min_cmp > 1 ? "+" : "-",fabs(1.0-frac_min_cmp),idx_min+1UL,var_yc_b->val.dp[idx_min],var_xc_b->val.dp[idx_min]);
    fprintf(stdout,"frac_b max: %0.16f = 1.0%s%0.1e // Maximum in grid B cell [%lu,%+g,%+g]\n",frac_max_cmp,frac_max_cmp > 1 ? "+" : "-",fabs(1.0-frac_max_cmp),idx_max+1UL,var_yc_b->val.dp[idx_max],var_xc_b->val.dp[idx_max]);
    fprintf(stdout,"frac_b mbs: %0.16f =     %0.1e // %sean absolute bias from 1.0\n",mebs,mebs,area_wgt_b ? "Area-weighted m" : "M");
    fprintf(stdout,"frac_b rms: %0.16f =     %0.1e // %sRMS relative to 1.0\n",rms,rms,area_wgt_b ? "Area-weighted " : "");
    fprintf(stdout,"frac_b sdn: %0.16f =     %0.1e // Standard deviation\n",sdn,sdn);

    /* Inform/Warn if difference between disk and computed values */
    if(has_frac_b){
      cmp_dsk_dff=frac_min_cmp-frac_min_dsk;
      if(fabs(cmp_dsk_dff) > eps_abs){fprintf(stdout,"%s: Computed (as row sums) and disk-values of min(frac_b) disagree by more than %0.1e:\n  %0.16f - %0.16f = %g\n",fabs(cmp_dsk_dff) < 10*eps_abs ? "INFO" : "WARNING",eps_abs,frac_min_cmp,frac_min_dsk,cmp_dsk_dff);}
      cmp_dsk_dff=frac_max_cmp-frac_max_dsk;
      if(fabs(cmp_dsk_dff) > eps_abs){fprintf(stdout,"%s: Computed (as row sums) and disk-values of max(frac_b) disagree by more than %0.1e:\n  %0.16f - %0.16f = %g\n",fabs(cmp_dsk_dff) < 10*eps_abs ? "INFO" : "WARNING",eps_abs,frac_max_cmp,frac_max_dsk,cmp_dsk_dff);}
    } /* !has_frac_b */
      
    /* NB: fabs() does not enclose frac_max_cmp below yet does in corresponding expression for frac_a above
       I think this is correct, or at least harmless, and rejects some false positive WARNINGs */
    if(frac_max_cmp-1.0 > eps_max_wrn || (grid_a_tiles_sphere && (fabs(frac_min_cmp-1.0) > eps_max_wrn))) fprintf(stdout,"\nWARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING\n\tDanger, Will Robinson! max(frac_b) or min(frac_b) error exceeds %0.1e\n\tRegridding with these embarrassing weights will produce funny results\n\tSuggest re-generating weights with a better algorithm/weight-generator\n\tHave both input grid-files been validated? If not, one might be barmy\n%sWARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING\n\n",eps_max_wrn,(frac_max_cmp-1.0 > eps_max_wrn) ? "\tFor example, a source grid that overlaps itself will usually result in frac_b >> 1\n" : "");

    if(nco_dbg_lvl_get() >= nco_dbg_std){
      sz=var_frac_b->sz;
      val=var_frac_b->val.dp;
      int wrn_nbr=0; // [nbr] Number of warnings
      const double eps_err=1.0e-8;
      for(idx=0;idx<sz;idx++){
	if(!has_mask_b || (has_mask_b && var_mask_b->val.ip[idx] == 1)){
	  if((val[idx]-1.0 > eps_err) || (grid_a_tiles_sphere && (fabs(val[idx]-1.0) > eps_err))){
	    if(nco_dbg_lvl_get() >= nco_dbg_fl) fprintf(stdout,"WARNING consistency = %0.16f = 1.0%s%0.1e for grid B cell [%lu,%+g,%+g]\n",val[idx],val[idx] > 1 ? "+" : "-",fabs(1.0-val[idx]),idx+1UL,var_yc_b->val.dp[idx],var_xc_b->val.dp[idx]);
	    wrn_nbr++;
	  } /* !err */
	} /* !msk */
      } /* !idx */
      if(wrn_nbr > 0) fprintf(stdout,"WARNING non-consistent row-sums (error exceeds tolerance = %0.1e) for %d of %lu grid B cells\nNB: consistency WARNINGS may be safely ignored for Grid B cells not completely overlapped with unmasked Grid A cells (e.g., coastlines)\nThese diagnostics imperfectly attempt to rule-out such false-positive WARNINGs\nTrue-positive WARNINGs occur in destination gridcells that this map underfills (error < 0) or overfills (error > 0)\n\n",eps_err,wrn_nbr,var_area_b->sz);
    } /* !dbg */

    if(flg_frac_b_nrm){
      size_t idx_row;
      size_t idx_crr_row;
      size_t sz_row;
      sz_row=var_frac_b->sz;
      val=var_frac_b->val.dp;
      int wrn_nbr=0; // [nbr] Number of warnings
      const double eps_err=1.0e-8;
      for(idx_row=0;idx_row<sz_row;idx_row++){
	if(!has_mask_b || (has_mask_b && var_mask_b->val.ip[idx_row] == 1)){
	  if((val[idx_row]-1.0 > eps_err) || (grid_a_tiles_sphere && (fabs(val[idx_row]-1.0) > eps_err))){
	    wrn_nbr++;
	    (void)fprintf(stdout,"FIXING consistency = %0.16f = 1.0%s%0.1e for grid B cell [%lu,%+g,%+g]\n",val[idx_row],val[idx_row] > 1 ? "+" : "-",fabs(1.0-val[idx_row]),idx_row+1UL,var_yc_b->val.dp[idx_row],var_xc_b->val.dp[idx_row]);
	    /* Renormalize all weights that contribute to this inconsistent frac_b */
	    sz=var_S->sz;
	    for(idx=0;idx<sz;idx++){
	      if((idx_crr_row=var_row->val.ip[idx]-1) == idx_row) var_S->val.dp[idx]/=val[idx_row];
	    } /* !idx */
	  } /* !err */
	} /* !msk */
      } /* !idx_row */
      if(wrn_nbr > 0){
	/* Compute frac_a as area_b-weighted column sums/area_a with renormalized weights */
	nco_map_frac_a_clc(var_S,var_row,var_col,var_area_a,var_area_b,var_frac_a);
	/* Compute frac_b as row sums with re-normalized weights */
	nco_map_frac_b_clc(var_S,var_row,var_frac_b);
	(void)fprintf(stdout,"%s: INFO Re-writing S, frac_a, and frac_b arrays to fix %d (presumed) self-overlaps detected via frac_b >> 1.0 search\nNB: The \"fixed\" file should no longer report any frac_b WARNINGs because the weights have been normalized to prevent this. However, the fixed file is expected to produce frac_a WARNINGs because weights of the self-overlapping grid_a cells were reduced to compensate for the self-overlap. So long as all affected grid_a cells contain valid data the net result should be correct. The best solution is to remove/re-bin the self-overlapping grid_a cells before remapping.\n",nco_prg_nm_get(),wrn_nbr);
	rcd=nco_put_var(in_id,var_frac_a->id,var_frac_a->val.vp,(nc_type)NC_DOUBLE);
	rcd=nco_put_var(in_id,var_frac_b->id,var_frac_b->val.vp,(nc_type)NC_DOUBLE);
	rcd=nco_put_var(in_id,var_S->id,var_S->val.vp,(nc_type)NC_DOUBLE);
      }else{ /* !wrn_nbr */
	(void)fprintf(stdout,"%s: INFO User requested map re-weight with --frac_b_nrm to adjust for self-overlapped Grid A gridcells but %s finds no frac_b >> 1.0 gridcells that would indicate potential self-overlaps in Grid A\n",nco_prg_nm_get(),fnc_nm);
      } /* !wrn_nbr */
    } /* !flg_frac_b_nrm */
    
    if(nco_dbg_lvl_get() >= nco_dbg_std){
      fprintf(stdout,"Commands to examine consistency extrema:\n");
      fprintf(stdout,"min(frac_b): ncks --fortran -H --trd -d n_b,%lu -v .?_b %s\n",idx_min+1UL,fl_in);
      fprintf(stdout,"max(frac_b): ncks --fortran -H --trd -d n_b,%lu -v .?_b %s\n",idx_max+1UL,fl_in);
    } /* !dbg */

    fprintf(stdout,"\nHistogram of non-zero entries in sparse-matrix:\n");
    fprintf(stdout,"  Column 1: Number of non-zero entries (histogram bin)\n");
    fprintf(stdout,"  Column 2: Number of columns (source cells) with that many non-zero entries\n");
    fprintf(stdout,"  Column 3: Number of rows (destination cells) with that many non-zero entries\n");
    fprintf(stdout,"  [");
    hst_sz_nnz=hst_sz;
    for(idx=0;idx<=hst_sz;idx++)
      if(hst_col[idx] > 0 || hst_row[idx] > 0) hst_sz_nnz=idx;
    for(idx=0;idx<=hst_sz_nnz;idx++)
      if(hst_col[idx] > 0 || hst_row[idx] > 0) fprintf(stdout,"[%s%lu,%d,%d]%s",idx == hst_sz ? ">= " : "",idx,hst_col[idx],hst_row[idx],idx != hst_sz_nnz ? ", " : "]\n");

    fprintf(stdout,"\nHistogram of weights S: [bin_min < weights <= bin_max]\n");
    fprintf(stdout,"  Column 1: Lower bound on weights (bin_min)\n");
    fprintf(stdout,"  Column 2: Number of weights in bin\n");
    fprintf(stdout,"  Column 3: Upper bound on weights (bin_max)\n");
    fprintf(stdout,"  [");
    hst_sz_nnz=0;
    int hst_vld_crr=0;
    for(idx=0;idx<hst_wgt_nbr;idx++)
      if(hst_wgt[idx] > 0) hst_sz_nnz++;
    for(idx=0;idx<hst_wgt_nbr;idx++)
      if(hst_wgt[idx] > 0){
	hst_vld_crr++;
	fprintf(stdout,"[%g,%d,%g]%s",hst_vld_crr == 1 ? s_min : hst_wgt_ntf[idx],hst_wgt[idx],hst_vld_crr == hst_sz_nnz ? s_max : hst_wgt_ntf[idx+1],hst_vld_crr != hst_sz_nnz ? ", " : "]\n");
      } /* !hst_wgt */

    if(hst_row) hst_row=(int *)nco_free(hst_row);
    if(hst_col) hst_col=(int *)nco_free(hst_col);
    if(hst_wgt) hst_wgt=(int*)nco_free(hst_wgt);
  } /* !report */
  
  nco_close(in_id);

  /* Free memory */
  if(dmn_in) dmn_in=nco_dmn_lst_free(dmn_in,dmn_in_nbr );
  if(var_S) var_S=nco_var_free(var_S);
  if(var_area_a) var_area_a=nco_var_free(var_area_a);
  if(var_area_b) var_area_b=nco_var_free(var_area_b);
  if(var_col) var_col=nco_var_free(var_col);
  if(var_frac_a) var_frac_a=nco_var_free(var_frac_a);
  if(var_frac_b) var_frac_b=nco_var_free(var_frac_b);
  if(var_mask_a) var_mask_a=nco_var_free(var_mask_a);
  if(var_mask_b) var_mask_b=nco_var_free(var_mask_b);
  if(var_row) var_row=nco_var_free(var_row);
  if(var_xc_a) var_xc_a=nco_var_free(var_xc_a);
  if(var_xc_b) var_xc_b=nco_var_free(var_xc_b);
  if(var_yc_a) var_yc_a=nco_var_free(var_yc_a);
  if(var_yc_b) var_yc_b=nco_var_free(var_yc_b);

  return True;
} /* !nco_map_chk() */
