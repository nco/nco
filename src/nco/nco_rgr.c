/* $Header$ */

/* Purpose: NCO utilities for regridding */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_rgr.h" /* Regridding */

int /* O [enm] Return code */
nco_rgr_ctl /* [fnc] Control regridding logic */
(rgr_sct * const rgr_nfo, /* I/O [sct] Regridding structure */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal Table */
{
  /* Purpose: Control regridding logic */
  int rcd=NCO_NOERR;
  const char fnc_nm[]="nco_rgr_ctl()";

  nco_bool flg_smf=False; /* [flg] ESMF regridding */
  nco_bool flg_tps=False; /* [flg] Tempest regridding */
  nco_bool flg_map=False; /* [flg] Weight-based regridding */

  /* Main control branching occurs here
     Branching complexity and utility will increase as regridding features are added */
  if(rgr_nfo->flg_map) flg_map=True;
  if(rgr_nfo->flg_grd_src && rgr_nfo->flg_grd_dst) flg_smf=True;
  if(rgr_nfo->drc_tps) flg_tps=True;
  assert(!(flg_smf && flg_map));
  assert(!(flg_smf && flg_tps));
  assert(!(flg_map && flg_tps));
  
  if(flg_map){
    /* Regrid using external mapping weights */
    rcd=nco_rgr_map(rgr_nfo,trv_tbl);
  } /* !flg_map */

  if(flg_smf){
#ifdef ENABLE_ESMF
    /* Regrid using ESMF library */
    (void)fprintf(stderr,"%s: %s calling nco_rgr_esmf() to generate and apply regridding map\n",nco_prg_nm_get(),fnc_nm);
    rcd=nco_rgr_esmf(rgr_nfo);
    /* Close output and free dynamic memory */
    (void)nco_fl_out_cls(rgr_nfo->fl_out,rgr_nfo->fl_out_tmp,rgr_nfo->out_id);
    (void)nco_rgr_free(rgr_nfo);
#else /* !ENABLE_ESMF */
    (void)fprintf(stderr,"%s: ERROR %s reports attempt to use ESMF regridding without built-in support. Re-configure with --enable_esmf.\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
#endif /* !ENABLE_ESMF */
} /* !flg_smf */
  
  if(flg_tps){
    /* Regrid using Tempest regridding */
    rcd=nco_rgr_tps(rgr_nfo);
  } /* !flg_map */

  return rcd;
} /* end nco_rgr_ctl() */

void
nco_rgr_free /* [fnc] Deallocate regridding structure */
(rgr_sct * const rgr_nfo) /* I/O [sct] Regridding structure */
{
  /* [fnc] Free all dynamic memory in regridding structure */
  if(rgr_nfo->fl_grd_src) rgr_nfo->fl_grd_src=(char *)nco_free(rgr_nfo->fl_grd_src);
  if(rgr_nfo->fl_grd_dst) rgr_nfo->fl_grd_dst=(char *)nco_free(rgr_nfo->fl_grd_dst);
  if(rgr_nfo->fl_in) rgr_nfo->fl_in=(char *)nco_free(rgr_nfo->fl_in);
  if(rgr_nfo->fl_out) rgr_nfo->fl_out=(char *)nco_free(rgr_nfo->fl_out);
  if(rgr_nfo->fl_out_tmp) rgr_nfo->fl_out_tmp=(char *)nco_free(rgr_nfo->fl_out_tmp);
  if(rgr_nfo->fl_map) rgr_nfo->fl_map=(char *)nco_free(rgr_nfo->fl_map);
  if(rgr_nfo->var_nm) rgr_nfo->var_nm=(char *)nco_free(rgr_nfo->var_nm);

  /* Tempest */
  if(rgr_nfo->drc_tps) rgr_nfo->drc_tps=(char *)nco_free(rgr_nfo->drc_tps);

  /* Mapfiles */
  if(rgr_nfo->drc_tps) rgr_nfo->drc_tps=(char *)nco_free(rgr_nfo->drc_tps);

} /* end nco_rfr_free() */
  
int /* O [enm] Return code */
nco_rgr_ini /* [fnc] Initialize regridding structure */
(const int in_id, /* I [id] Input netCDF file ID */
 char **rgr_arg, /* [sng] Regridding arguments */
 const int rgr_nbr, /* [nbr] Number of regridding arguments */
 char * const rgr_in, /* I [sng] File containing fields to be regridded */
 char * const rgr_out, /* I [sng] File containing regridded fields */
 char * const rgr_grd_src, /* I [sng] File containing input grid */
 char * const rgr_grd_dst, /* I [sng] File containing destination grid */
 char * const rgr_map, /* I [sng] File containing mapping weights from source to destination grid */
 char * const rgr_var, /* I [sng] Variable for special regridding treatment */
 rgr_sct * const rgr_nfo) /* O [sct] Regridding structure */
{
  /* Purpose: Initialize regridding structure */
     
  /* Sample calls:
     Debugging:
     ncks -O -D 6 --rgr=Y ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc

     T62->T42 from scratch, minimal arguments:
     ncks -O -D 6 --rgr=Y ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T62->T42 from scratch, more arguments:
     ncks -O -D 6 --rgr=Y --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T62->T42 from scratch, explicit arguments:
     ncks -O --rgr=Y --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/dstmch90_clm.nc ~/foo.nc
     T42->T42 from scratch:
     ncks -O --rgr=Y --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     T42->POP43 from existing weights:
     ncks -O --rgr=Y --rgr_map=${DATA}/scrip/rmp_T42_to_POP43_conserv.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc */

  const char fnc_nm[]="nco_rgr_ini()";
  
  int rcd=NCO_NOERR;
  
  /* Initialize */
  rgr_nfo->flg_usr_rqs=False; /* [flg] User requested regridding */
  rgr_nfo->out_id=int_CEWI; /* [id] Output netCDF file ID */

  rgr_nfo->in_id=in_id; /* [id] Input netCDF file ID */
  rgr_nfo->rgr_arg=rgr_arg; /* [sng] Regridding arguments */
  rgr_nfo->rgr_nbr=rgr_nbr; /* [nbr] Number of regridding arguments */

  rgr_nfo->flg_grd_src= rgr_grd_src ? True : False; /* [flg] User-specified input grid */
  rgr_nfo->fl_grd_src=rgr_grd_src; /* [sng] File containing input grid */

  rgr_nfo->flg_grd_dst= rgr_grd_dst ? True : False; /* [flg] User-specified destination grid */
  rgr_nfo->fl_grd_dst=rgr_grd_dst; /* [sng] File containing destination grid */

  rgr_nfo->fl_in=rgr_in; /* [sng] File containing fields to be regridded */
  rgr_nfo->fl_out=rgr_out; /* [sng] File containing regridded fields */
  rgr_nfo->fl_out_tmp=NULL_CEWI; /* [sng] Temporary file containing regridded fields */

  rgr_nfo->flg_map= rgr_map ? True : False; /* [flg] User-specified mapping weights */
  rgr_nfo->fl_map=rgr_map; /* [sng] File containing mapping weights from source to destination grid */

  rgr_nfo->var_nm=rgr_var; /* [sng] Variable for special regridding treatment */
  
  /* Did user explicitly request regridding? */
  if(rgr_nbr > 0 || rgr_grd_src != NULL || rgr_grd_dst != NULL || rgr_out != NULL || rgr_map != NULL) rgr_nfo->flg_usr_rqs=True;

  /* Initialize arguments after copying */
  if(!rgr_nfo->fl_out) rgr_nfo->fl_out=(char *)strdup("/data/zender/rgr/rgr_out.nc");
  if(!rgr_nfo->fl_grd_dst) rgr_nfo->fl_grd_dst=(char *)strdup("/data/zender/scrip/grids/remap_grid_T42.nc");
  if(!rgr_nfo->var_nm) rgr_nfo->var_nm=(char *)strdup("ORO");
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"flg_usr_rqs = %d, ",rgr_nfo->flg_usr_rqs);
    (void)fprintf(stderr,"rgr_nbr = %d, ",rgr_nfo->rgr_nbr);
    (void)fprintf(stderr,"fl_grd_src = %s, ",rgr_nfo->fl_grd_src ? rgr_nfo->fl_grd_src : "NULL");
    (void)fprintf(stderr,"fl_grd_dst = %s, ",rgr_nfo->fl_grd_dst ? rgr_nfo->fl_grd_dst : "NULL");
    (void)fprintf(stderr,"fl_in = %s, ",rgr_nfo->fl_in ? rgr_nfo->fl_in : "NULL");
    (void)fprintf(stderr,"fl_out = %s, ",rgr_nfo->fl_out ? rgr_nfo->fl_out : "NULL");
    (void)fprintf(stderr,"fl_out_tmp = %s, ",rgr_nfo->fl_out_tmp ? rgr_nfo->fl_out_tmp : "NULL");
    (void)fprintf(stderr,"fl_map = %s, ",rgr_nfo->fl_map ? rgr_nfo->fl_map : "NULL");
    (void)fprintf(stderr,"\n");
  } /* endif dbg */
  
  return rcd;
} /* end nco_rgr_ini() */
  
int /* O [enm] Return code */
nco_rgr_map /* [fnc] Regrid using external weights */
(rgr_sct * const rgr_nfo, /* I/O [sct] Regridding structure */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal Table */
{
  /* Purpose: Regrid fields using external weights (i.e., a mapping file)

     Examine SCRIP remap file:
     ncks --cdl -m ${DATA}/scrip/rmp_T42_to_POP43_conserv.nc | m

     Test SCRIP remapping file:
     ncks -D 6 -O --rgr=Y --rgr_map=${DATA}/scrip/rmp_T42_to_POP43_conserv.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     Test ESMF remapping file:
     ncks -D 6 -O --rgr=Y --rgr_map=${DATA}/rgr/map_ne120np4_to_181x360_aave.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
 
     Conventions:
     grid_size: Number of grid cells (product of lat*lon)
     address: Source and destination index for each link pair
     num_links: Number of unique address pairs in remapping, i.e., size of sparse matrix
     num_wgts: Number of weights per vertice for given remapping
     = 1 Bilinear
         Destination grid value determined by weights times known source grid values 
         at vertices of source quadrilateral that bounds destination point P
         One weight per vertice guarantees fxm but is not conservative
         Bilinear requires logically rectangular grid
     = 1 Distance-based:
	 Distance-weighted uses values at num_neighbors points
	 The weight is inversely proportional to the angular distance from 
	 the destination point to each neighbor on the source grid
     = 3 Second-order conservative:
         Described in Jones, P. W. (1999), Monthly Weather Review, 127, 2204-2210
         First-order conservative schemes assume fluxes are constant within gridcell
	 Destination fluxes are simple summations of sources fluxes weighted by overlap areas
	 Old clm and bds remappers use a first-order algorithm
	 Second-order improves this by using a first-order Taylor expansion of flux
	 Source flux is centroid value plus directional offset determined by dot product
	 of directional gradient and vector pointing from vertice to centroid.
         Three weights per vertice are centroid weight, weight times local theta-gradient from
	 centroid to vertice, and weight times local phi-gradient from centroid to vertice.
     = 4 Bicubic: 
         The four weights are gradients in each direction plus a cross-gradient term
         Same principle as bilinear, but more weights per vertice
         Bicubic requires logically rectangular grid
     
     wgt: 
     Maximum number of source cells contributing to destination cell is not a dimension
     in SCRIP remapping files because SCRIP stores everying in 1-D sparse matrix arrays
     Sparse matrix formulations:

     for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
       // Normalization: fractional area
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0];
       // Normalization: destination area
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0]/dst_area[ddr_dst[lnk_idx]];
       // Normalization: none
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0]/(dst_area[ddr_dst[lnk_idx]]*dst_frc[ddr_dst[lnk_idx]);
  */

  const char fnc_nm[]="nco_rgr_map()"; /* [sng] Function name */

  char *fl_in=rgr_nfo->fl_map;
  char *fl_pth_lcl=NULL;

  int in_id; /* I [id] Input netCDF file ID */
  int out_id; /* I [id] Output netCDF file ID */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int rcd=NC_NOERR;

  int src_grid_size_id; /* [id] Source grid size dimension ID */
  int dst_grid_size_id; /* [id] Destination grid size dimension ID */
  int src_grid_corners_id; /* [id] Source grid corners dimension ID */
  int dst_grid_corners_id; /* [id] Destination grid corners dimension ID */
  int src_grid_rank_id; /* [id] Source grid rank dimension ID */
  int dst_grid_rank_id; /* [id] Destination grid rank dimension ID */
  int num_links_id; /* [id] Number of links dimension ID */
  int num_wgts_id; /* [id] Number of weights dimension ID */

  nco_bool FL_RTR_RMT_LCN;
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */

  nco_map_sct rgr_map;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  size_t lnk_nbr; /* [nbr] Number of links */
  size_t lnk_idx; 
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s obtaining mapping weights from %s\n",nco_prg_nm_get(),fnc_nm,rgr_nfo->fl_map);

  var_sct *wgt=NULL;
  var_sct *wgt_out=NULL;

  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  /* Identify mapping file type.
     Generated by SCRIP: conventions = "SCRIP"
     Generated by ESMF_Regridder: conventions = "NCAR-CSM" */
  char *att_val;
  char *cnv_sng=NULL_CEWI;
  /* netCDF standard is uppercase Conventions, though some models user lowercase */
  char cnv_sng_UC[]="Conventions"; /* Unidata standard     string (uppercase) */
  char cnv_sng_LC[]="conventions"; /* Unidata non-standard string (lowercase) */
  
  long att_sz;

  nc_type att_typ;

  nco_rgr_mpf_typ_enm nco_rgr_mpf_typ=nco_rgr_mpf_nil; /* [enm] Type of remapping file */
  
  /* Look for map-type signature in Conventions attribute */
  cnv_sng=cnv_sng_UC;
  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd != NC_NOERR){
    /* Re-try with lowercase string because some models, e.g., CLM, user lowercase "conventions" */
    cnv_sng=cnv_sng_LC;
    rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  } /* endif lowercase */
  
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1L);
    rcd+=nco_get_att(in_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate convention attribute before using strcmp() */
    att_val[att_sz]='\0';
    /* ESMF conventions */
    if(strstr(att_val,"NCAR-CSM")) nco_rgr_mpf_typ=nco_rgr_mpf_ESMF;
    if(strstr(att_val,"SCRIP")) nco_rgr_mpf_typ=nco_rgr_mpf_SCRIP;
    if(nco_rgr_mpf_typ == nco_rgr_mpf_nil) (void)fprintf(stderr,"%s: ERROR %s unrecognized map type specified in attribute Conventions = %s\n",nco_prg_nm_get(),fnc_nm,att_val);
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* end rcd && att_typ */

  switch(nco_rgr_mpf_typ){
  case nco_rgr_mpf_SCRIP:
    rcd+=nco_inq_dimid(in_id,"src_grid_size",&src_grid_size_id);
    rcd+=nco_inq_dimid(in_id,"dst_grid_size",&dst_grid_size_id);
    rcd+=nco_inq_dimid(in_id,"src_grid_corners",&src_grid_corners_id);
    rcd+=nco_inq_dimid(in_id,"dst_grid_corners",&dst_grid_corners_id);
    rcd+=nco_inq_dimid(in_id,"src_grid_rank",&src_grid_rank_id);
    rcd+=nco_inq_dimid(in_id,"dst_grid_rank",&dst_grid_rank_id);
    rcd+=nco_inq_dimid(in_id,"num_links",&num_links_id);
    rcd+=nco_inq_dimid(in_id,"num_wgts",&num_wgts_id);
  break;
  case nco_rgr_mpf_ESMF:
    rcd+=nco_inq_dimid(in_id,"n_a",&src_grid_size_id);
    rcd+=nco_inq_dimid(in_id,"n_b",&dst_grid_size_id);
    rcd+=nco_inq_dimid(in_id,"nv_a",&src_grid_corners_id);
    rcd+=nco_inq_dimid(in_id,"nv_b",&dst_grid_corners_id);
    rcd+=nco_inq_dimid(in_id,"src_grid_rank",&src_grid_rank_id);
    rcd+=nco_inq_dimid(in_id,"dst_grid_rank",&dst_grid_rank_id);
    rcd+=nco_inq_dimid(in_id,"n_s",&num_links_id);
    rcd+=nco_inq_dimid(in_id,"num_wgts",&num_wgts_id);
    break;
  default:
    (void)fprintf(stderr,"%s: ERROR %s unknown map file type\n",nco_prg_nm_get(),fnc_nm);
    nco_dfl_case_generic_err(); break;
  } /* end switch */
    
  /* Now we have dimension IDs, get dimension sizes */
  rcd+=nco_inq_dimlen(in_id,src_grid_size_id,&rgr_map.src_grid_size);
  rcd+=nco_inq_dimlen(in_id,dst_grid_size_id,&rgr_map.dst_grid_size);
  rcd+=nco_inq_dimlen(in_id,src_grid_corners_id,&rgr_map.src_grid_corners);
  rcd+=nco_inq_dimlen(in_id,dst_grid_corners_id,&rgr_map.dst_grid_corners);
  rcd+=nco_inq_dimlen(in_id,src_grid_rank_id,&rgr_map.src_grid_rank);
  rcd+=nco_inq_dimlen(in_id,dst_grid_rank_id,&rgr_map.dst_grid_rank);
  rcd+=nco_inq_dimlen(in_id,num_links_id,&rgr_map.num_links);
  rcd+=nco_inq_dimlen(in_id,num_wgts_id,&rgr_map.num_wgts);
  
  cnv_sng=strdup("normalization");
  nco_rgr_nrm_typ_enm nco_rgr_nrm_typ=nco_rgr_nrm_nil;
  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1L);
    rcd+=nco_get_att(in_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
  } /* endif */
    /* NUL-terminate convention attribute before using strcmp() */
  att_val[att_sz]='\0';
  if(strstr(att_val,"fracarea")) nco_rgr_nrm_typ=nco_rgr_nrm_fracarea;
  if(strstr(att_val,"destarea")) nco_rgr_nrm_typ=nco_rgr_nrm_destarea;
  if(strstr(att_val,"none")) nco_rgr_nrm_typ=nco_rgr_nrm_none;
  assert(nco_rgr_nrm_typ == nco_rgr_nrm_destarea);
  if(att_val) att_val=(char *)nco_free(att_val);
  if(cnv_sng) cnv_sng=(char *)nco_free(cnv_sng);

  cnv_sng=strdup("map_method");
  nco_rgr_mth_typ_enm nco_rgr_mth_typ=nco_rgr_mth_nil;
  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1L);
    rcd+=nco_get_att(in_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
  } /* endif */
  /* NUL-terminate convention attribute before using strcmp() */
  att_val[att_sz]='\0';
  if(strstr(att_val,"Conservative remapping")) nco_rgr_mth_typ=nco_rgr_mth_conservative;
  if(strstr(att_val,"Bilinear remapping")) nco_rgr_mth_typ=nco_rgr_mth_bilinear;
  if(strstr(att_val,"none")) nco_rgr_mth_typ=nco_rgr_mth_none;
  assert(nco_rgr_mth_typ != nco_rgr_mth_nil);
  if(att_val) att_val=(char *)nco_free(att_val);
  if(cnv_sng) cnv_sng=(char *)nco_free(cnv_sng);

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"map_method = %s, normalization = %s, src_grid_size = %li, dst_grid_size = %li, src_grid_corners = %li, dst_grid_corners = %li, src_grid_rank = %li, dst_grid_rank = %li, num_links = %li, num_wgts = %li\n",nco_rgr_mth_sng(nco_rgr_mth_typ),nco_rgr_nrm_sng(nco_rgr_nrm_typ),rgr_map.src_grid_size,rgr_map.dst_grid_size,rgr_map.src_grid_corners,rgr_map.dst_grid_corners,rgr_map.src_grid_rank,rgr_map.dst_grid_rank,rgr_map.num_links,rgr_map.num_wgts);
  } /* endif dbg */

  /* Obtain grid values necessary to compute output latitude and longitude coordinates */
  int dst_grd_ctr_lon_id; /* [id] Destination grid center longitudes variable ID */
  int dst_grd_ctr_lat_id; /* [id] Destination grid center latitudes  variable ID */
  int dst_grd_crn_lon_id; /* [id] Destination grid corner longitudes variable ID */
  int dst_grd_crn_lat_id; /* [id] Destination grid corner latitudes  variable ID */
  int dmn_sz_id; /* [id] Destination grid dimension sizes ID */
  int wgt_raw_id; /* [id] Remap matrix variable ID */

  switch(nco_rgr_mpf_typ){
    /* Obtain fields whose name depends on mapfile type */
  case nco_rgr_mpf_SCRIP:
    rcd+=nco_inq_varid(in_id,"dst_grid_center_lon",&dst_grd_ctr_lon_id);
    rcd+=nco_inq_varid(in_id,"dst_grid_center_lat",&dst_grd_ctr_lat_id);
    rcd+=nco_inq_varid(in_id,"dst_grid_corner_lon",&dst_grd_crn_lon_id);
    rcd+=nco_inq_varid(in_id,"dst_grid_corner_lat",&dst_grd_crn_lat_id);
    rcd+=nco_inq_varid(in_id,"remap_matrix",&wgt_raw_id); /* fxm: remap_matrix[num_links,num_wgts] != S[n_s] */
  break;
  case nco_rgr_mpf_ESMF:
    rcd+=nco_inq_varid(in_id,"xc_b",&dst_grd_ctr_lon_id);
    rcd+=nco_inq_varid(in_id,"yc_b",&dst_grd_ctr_lat_id);
    rcd+=nco_inq_varid(in_id,"xv_b",&dst_grd_crn_lon_id);
    rcd+=nco_inq_varid(in_id,"yv_b",&dst_grd_crn_lat_id);
    rcd+=nco_inq_varid(in_id,"S",&wgt_raw_id); /* fxm: remap_matrix[num_links,num_wgts] != S[n_s] */
    break;
  default:
    (void)fprintf(stderr,"%s: ERROR %s unknown map file type\n",nco_prg_nm_get(),fnc_nm);
    nco_dfl_case_generic_err(); break;
  } /* end switch */
  /* Obtain fields whose name is independent of mapfile type */
  rcd+=nco_inq_varid(in_id,"dst_grid_dims",&dmn_sz_id);

  long dmn_cnt[2];
  long dmn_srt[2];
  long dmn_srd[2];
  long idx; /* [idx] Counting index for unrolled grids */
  double *lon_ctr_out; /* [dgr] Longitude centers of rectangular destination grid */
  double *lat_ctr_out; /* [dgr] Latitude  centers of rectangular destination grid */
  double *lon_crn_out; /* [dgr] Longitude corners of rectangular destination grid */
  double *lat_crn_out; /* [dgr] Latitude  corners of rectangular destination grid */
  double *lon_ntf_out; /* [dgr] Longitude interfaces of rectangular destination grid */
  double *lat_ntf_out; /* [dgr] Latitude  interfaces of rectangular destination grid */
  double *lon_bnd_out; /* [dgr] Longitude boundaries of rectangular destination grid */
  double *lat_bnd_out; /* [dgr] Latitude  boundaries of rectangular destination grid */
  double *wgt_raw; /* [frc] Remapping weights */
  int *dmn_sz; /* [nbr] Array of dimension sizes of destination grid */
  const int lon_psn_dst=0; /* [idx] Ordinal position of longitude size in rectangular destination grid */
  const int lat_psn_dst=1; /* [idx] Ordinal position of latitude  size in rectangular destination grid */
  const int bnd_rnk=2; /* [nbr] Rank of output coordinate CF boundary variables */
    
  /* Allocate space to hold dimension metadata for rectangular destination grid */
  assert(rgr_map.dst_grid_rank == 2);
  dmn_srt[0]=0L;
  dmn_cnt[0]=rgr_map.dst_grid_rank;
  dmn_sz=(int *)nco_malloc(rgr_map.dst_grid_rank*nco_typ_lng((nc_type)NC_INT));
  rcd=nco_get_vara(in_id,dmn_sz_id,dmn_srt,dmn_cnt,dmn_sz,(nc_type)NC_INT);

  const long lon_nbr_out=dmn_sz[lon_psn_dst]; /* [idx] Number of longitudes in rectangular destination grid */
  const long lat_nbr_out=dmn_sz[lat_psn_dst]; /* [idx] Number of latitudes  in rectangular destination grid */
  
  /* Allocate space for and obtain coordinates and weights */
  nc_type crd_typ_out=NC_DOUBLE;
  lon_ctr_out=(double *)nco_malloc(lon_nbr_out*nco_typ_lng(crd_typ_out));
  lat_ctr_out=(double *)nco_malloc(lat_nbr_out*nco_typ_lng(crd_typ_out));
  lon_crn_out=(double *)nco_malloc(rgr_map.dst_grid_corners*lon_nbr_out*nco_typ_lng(crd_typ_out));
  lat_crn_out=(double *)nco_malloc(rgr_map.dst_grid_corners*lat_nbr_out*nco_typ_lng(crd_typ_out));
  lon_ntf_out=(double *)nco_malloc((lon_nbr_out+1L)*nco_typ_lng(crd_typ_out));
  lat_ntf_out=(double *)nco_malloc((lat_nbr_out+1L)*nco_typ_lng(crd_typ_out));
  lon_bnd_out=(double *)nco_malloc(bnd_rnk*lon_nbr_out*nco_typ_lng(crd_typ_out));
  lat_bnd_out=(double *)nco_malloc(bnd_rnk*lat_nbr_out*nco_typ_lng(crd_typ_out));
  wgt_raw=(double *)nco_malloc_dbg(rgr_map.num_links*nco_typ_lng(crd_typ_out),"Unable to malloc() value buffer for remapping weights",fnc_nm);
  
  /* Arrays unroll into all longitudes for first latitude, then second latitude, ...
     Thus longitudes obtained by reading first block contiguously (unstrided)
     Latitudes obtained by reading unrolled data with stride of lon_nbr */
  dmn_srt[0]=0L;
  dmn_cnt[0]=lon_nbr_out;
  rcd=nco_get_vara(in_id,dst_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_out,crd_typ_out);
  dmn_srt[0]=0L;
  dmn_cnt[0]=lat_nbr_out;
  dmn_srd[0]=lon_nbr_out;
  rcd=nco_get_vars(in_id,dst_grd_ctr_lat_id,dmn_srt,dmn_cnt,dmn_srd,lat_ctr_out,crd_typ_out);
  dmn_srt[0]=dmn_srt[1]=0L;
  dmn_cnt[0]=lon_nbr_out;
  dmn_cnt[1]=rgr_map.dst_grid_corners;
  rcd=nco_get_vara(in_id,dst_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn_out,crd_typ_out);
  dmn_srt[0]=0L;
  dmn_cnt[0]=lat_nbr_out;
  dmn_cnt[1]=rgr_map.dst_grid_corners;
  dmn_srd[0]=lon_nbr_out;
  dmn_srd[1]=1L;
  rcd=nco_get_vars(in_id,dst_grd_crn_lat_id,dmn_srt,dmn_cnt,dmn_srd,lat_crn_out,crd_typ_out);
  
  /* Derive interface boundaries from lat and lon grid-center values
     NB: Procedures to derive interfaces from midpoints on rectangular grids are theoretically possible 
     However, ESMF often outputs interfaces values (e.g., yv_b) for midpoint coordinates (e.g., yc_b)
     For example, ACME standard map from ne120np4 to 181x360 has yc_b[0] = yv_b[0] = -90.0
     Latitude = -90 is, by definition, not a midpoint coordinate
     Maybe there is an ESMF flag or something that resolves this special case?
     More safe to read boundary interfaces directly from grid corner/vertice arrays in map file

     Derivation of boundaries from _correct_ xc_b, yc_b is follows
     Do not implement this procedure until resolving midpoint/center issue described above:
     lon_ntf_out[0]=0.5*(lon_ctr_out[0]+lon_ctr_out[lon_nbr_out-1])-180.0;
     lat_ntf_out[0]=lat_ctr_out[0]-0.5*(lat_ctr_out[1]-lat_ctr_out[0]);
     for(idx=1;idx<lon_nbr_out;idx++) lon_ntf_out[idx]=0.5*(lon_ctr_out[idx-1]+lon_ctr_out[idx]);
     for(idx=1;idx<lat_nbr_out;idx++) lat_ntf_out[idx]=0.5*(lat_ctr_out[idx-1]+lat_ctr_out[idx]);
     lon_ntf_out[lon_nbr_out]=lon_ntf_out[0]+360.0;
     lat_ntf_out[lat_nbr_out]=lat_ctr_out[lat_nbr_out-1]+0.5*(lat_ctr_out[lat_nbr_out-1]-lat_ctr_out[lat_nbr_out-2]); */

  /* Obtain 1-D rectangular interfaces from unrolled 1-D vertice arrays */
  for(idx=0;idx<lon_nbr_out;idx++) lon_ntf_out[idx]=lon_crn_out[rgr_map.dst_grid_corners*idx];
  lon_ntf_out[lon_nbr_out]=lon_crn_out[rgr_map.dst_grid_corners*lon_nbr_out-(rgr_map.dst_grid_corners-1L)];
  for(idx=0;idx<lat_nbr_out;idx++) lat_ntf_out[idx]=lat_crn_out[rgr_map.dst_grid_corners*idx];
  lat_ntf_out[lat_nbr_out]=lat_crn_out[rgr_map.dst_grid_corners*lat_nbr_out-1L];

  /* Place 1-D rectangular interfaces into 2-D coordinate boundaries */
  for(idx=0;idx<lon_nbr_out;idx++){
    lon_bnd_out[idx]=lon_ntf_out[idx];
    lon_bnd_out[lon_nbr_out+idx]=lon_ntf_out[idx+1];
  } /* end loop over longitude */
  for(idx=0;idx<lat_nbr_out;idx++){
    lat_bnd_out[idx]=lat_ntf_out[idx];
    lat_bnd_out[lat_nbr_out+idx]=lat_ntf_out[idx+1];
  } /* end loop over latitude */
  
  if(nco_dbg_lvl_get() >= nco_dbg_vec){
    (void)fprintf(stderr,"%s: INFO %s reports destination rectangular latitude grid:\n",nco_prg_nm_get(),fnc_nm);
    for(idx=0;idx<lon_nbr_out;idx++) (void)fprintf(stderr,"lon[%li] = [%g, %g, %g]\n",idx,lon_bnd_out[idx],lon_ctr_out[idx],lon_bnd_out[lon_nbr_out+idx]);
    for(idx=0;idx<lat_nbr_out;idx++) (void)fprintf(stderr,"lat[%li] = [%g, %g, %g]\n",idx,lat_bnd_out[idx],lat_ctr_out[idx],lat_bnd_out[lat_nbr_out+idx]);
  } /* endif dbg */

  /* Obtain remap matrix (i.e., weights) from map file */
  dmn_srt[0]=0L;
  dmn_cnt[0]=rgr_map.num_links;
  rcd=nco_get_vara(in_id,wgt_raw_id,dmn_srt,dmn_cnt,wgt_raw,crd_typ_out);

  /* Close input netCDF file */
  nco_close(in_id);

  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  /* Initialize */
  in_id=rgr_nfo->in_id;
  out_id=rgr_nfo->out_id;
  
  int dmn_id_lat; /* [id] Dimension ID */
  int dmn_id_lon; /* [id] Dimension ID */
  int dmn_id_vrt; /* [id] Dimension ID */
  int lon_out_id; /* [id] Variable ID for longitude */
  int lat_out_id; /* [id] Variable ID for latitude */
  int lon_bnd_id; /* [id] Variable ID for lon_bnds */
  int lat_bnd_id; /* [id] Variable ID for lat_bnds */
  int dmn_ids_out[2]; /* [id] Dimension IDs array for output variable */
  long dmn_srt_out[2];
  long dmn_cnt_out[2];
  const int crd_idx_c_0bs_lat_dst=0; /* [dgr] 0-based index of latitude  in C       representation of rank-2 destination grids */
  const int crd_idx_c_0bs_lon_dst=1; /* [dgr] 0-based index of longitude in C       representation of rank-2 destination grids */

  /* Define new dimensions in regridded file */
  aed_sct aed_mtd;
  char *att_nm;
  char lat_nm_out[]="lat";
  char lon_nm_out[]="lon";
  char lat_bnd_nm[]="lat_bnds";
  char lon_bnd_nm[]="lon_bnds";
  rcd+=nco_def_dim(out_id,lat_nm_out,lat_nbr_out,&dmn_id_lat);
  rcd+=nco_def_dim(out_id,lon_nm_out,lon_nbr_out,&dmn_id_lon);
  rcd+=nco_def_dim(out_id,"nv",(int)2,&dmn_id_vrt);
  /* Define new coordinates in regridded file */
  (void)nco_def_var(out_id,lon_nm_out,crd_typ_out,(int)1,&dmn_id_lon,&lon_out_id);
  (void)nco_def_var(out_id,lat_nm_out,crd_typ_out,(int)1,&dmn_id_lat,&lat_out_id);
  dmn_ids_out[0]=dmn_id_lat;
  dmn_ids_out[1]=dmn_id_vrt;
  (void)nco_def_var(out_id,lat_bnd_nm,crd_typ_out,(int)2,dmn_ids_out,&lat_bnd_id);
  dmn_ids_out[0]=dmn_id_lon;
  dmn_ids_out[1]=dmn_id_vrt;
  (void)nco_def_var(out_id,lon_bnd_nm,crd_typ_out,(int)2,dmn_ids_out,&lon_bnd_id);

  /* Add metadata */
  att_nm=strdup("standard_name");
  att_val=strdup("latitude");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lat_nm_out;
  aed_mtd.id=lat_out_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lat_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  att_nm=strdup("bounds");
  att_val=strdup("lat_bnds");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lat_nm_out;
  aed_mtd.id=lat_out_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lat_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  att_nm=strdup("bounds");
  att_val=strdup("lon_bnds");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lon_nm_out;
  aed_mtd.id=lon_out_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lon_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  /* Begin data mode */
  (void)nco_enddef(out_id);

  dmn_srt_out[crd_idx_c_0bs_lat_dst]=0L;
  dmn_cnt_out[crd_idx_c_0bs_lat_dst]=lat_nbr_out;
  (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_out,lat_ctr_out,crd_typ_out);
  dmn_srt_out[crd_idx_c_0bs_lon_dst]=0L;
  dmn_cnt_out[crd_idx_c_0bs_lon_dst]=lon_nbr_out;
  (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_out,lon_ctr_out,crd_typ_out);
  dmn_srt_out[0]=dmn_srt_out[1]=0L;
  dmn_cnt_out[0]=lat_nbr_out;
  dmn_cnt_out[1]=2;
  (void)nco_put_vara(out_id,lat_bnd_id,dmn_srt_out,dmn_cnt_out,lat_bnd_out,crd_typ_out);
  dmn_srt_out[0]=dmn_srt_out[1]=0L;
  dmn_cnt_out[0]=lon_nbr_out;
  dmn_cnt_out[1]=2;
  (void)nco_put_vara(out_id,lon_bnd_id,dmn_srt_out,dmn_cnt_out,lon_bnd_out,crd_typ_out);

  /* Obtain variable */

  /* Weight variable */

  /* Write variable */

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(rgr_nfo->fl_out,rgr_nfo->fl_out_tmp,out_id);

  /* From this point on we parallel ncks logic for writing output files except:
     All actions take place in sand-boxed routine and output file visible to regridding only
     We work with a separate traversal table? 
     Once things work with copy of traversal table, we merge back into main logic */

  /* Free memory allocated for grid reading/writing */
  if(dmn_sz) dmn_sz=(int *)nco_free(dmn_sz);
  if(lon_ctr_out) lon_ctr_out=(double *)nco_free(lon_ctr_out);
  if(lat_ctr_out) lat_ctr_out=(double *)nco_free(lat_ctr_out);
  if(lon_crn_out) lon_crn_out=(double *)nco_free(lon_crn_out);
  if(lat_crn_out) lat_crn_out=(double *)nco_free(lat_crn_out);
  if(lon_ntf_out) lon_ntf_out=(double *)nco_free(lon_ntf_out);
  if(lat_ntf_out) lat_ntf_out=(double *)nco_free(lat_ntf_out);
  if(lon_bnd_out) lon_bnd_out=(double *)nco_free(lon_bnd_out);
  if(lat_bnd_out) lat_bnd_out=(double *)nco_free(lat_bnd_out);
  if(wgt_raw) wgt_raw=(double *)nco_free(wgt_raw);
  
  return rcd;
} /* nco_rgr_map() */

int /* O [enm] Return code */
nco_rgr_tps /* [fnc] Regrid using Tempest library */
(rgr_sct * const rgr_nfo) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields using Tempest remapping "library" (more precisely, executables)

     Test Tempest library: no way to activate yet
     export DATA_TEMPEST='/data/zender/rgr';ncks -O --rgr=Y ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc */

  const char fnc_nm[]="nco_rgr_tps()";
  
  const int fmt_chr_nbr=6;
  const char *cmd_rgr_fmt;
  char *cmd_rgr;
  char fl_grd_dst[]="/tmp/foo_outRLLMesh.g";
  char *fl_grd_dst_cdl;
  int rcd_sys;
  int lat_nbr_rqs=180;
  int lon_nbr_rqs=360;
  nco_rgr_cmd_typ nco_rgr_cmd; /* [enm] Tempest remap command enum */

  char *nvr_DATA_TEMPEST; /* [sng] Directory where Tempest grids, meshes, and weights are stored */
  nvr_DATA_TEMPEST=getenv("DATA_TEMPEST");
  rgr_nfo->drc_tps= (nvr_DATA_TEMPEST && strlen(nvr_DATA_TEMPEST) > 0) ? (char *)strdup(nvr_DATA_TEMPEST) : (char *)strdup("/tmp");

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"drc_tps = %s, ",rgr_nfo->drc_tps ? rgr_nfo->drc_tps : "NULL");
    (void)fprintf(stderr,"\n");
  } /* endif dbg */

  /* Allow for whitespace characters in fl_grd_dst
     Assume CDL translation results in acceptable name for shell commands */
  fl_grd_dst_cdl=nm2sng_fl(fl_grd_dst);
  //drc_tps=strcat(drc_tps,"/");
  //drc_tps=strcat(drc_tps,fl_grd_dst);

  /* Construct and execute regridding command */
  nco_rgr_cmd=nco_rgr_GenerateRLLMesh;
  cmd_rgr_fmt=nco_tps_cmd_fmt_sng(nco_rgr_cmd);
  cmd_rgr=(char *)nco_malloc((strlen(cmd_rgr_fmt)+strlen(fl_grd_dst_cdl)-fmt_chr_nbr+1UL)*sizeof(char));
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: %s reports generating %d by %d RLL mesh in %s...\n",nco_prg_nm_get(),fnc_nm,lat_nbr_rqs,lon_nbr_rqs,fl_grd_dst);
  (void)sprintf(cmd_rgr,cmd_rgr_fmt,lat_nbr_rqs,lon_nbr_rqs,fl_grd_dst_cdl);
  rcd_sys=system(cmd_rgr);
  if(rcd_sys == -1){
    (void)fprintf(stdout,"%s: ERROR %s unable to complete Tempest regridding command \"%s\"\n",nco_prg_nm_get(),fnc_nm,cmd_rgr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"done\n");

  /* Clean-up memory */
  if(fl_grd_dst_cdl) fl_grd_dst_cdl=(char *)nco_free(fl_grd_dst_cdl);
  if(cmd_rgr) cmd_rgr=(char *)nco_free(cmd_rgr);
  
  return NCO_NOERR;
} /* end nco_rgr_tps() */

const char * /* O [sng] String describing regridding method */
nco_rgr_mth_sng /* [fnc] Convert regridding method enum to string */
(const nco_rgr_mth_typ_enm nco_rgr_mth_typ) /* I [enm] Regridding method enum */
{
  /* Purpose: Convert regridding method enum to string */
  switch(nco_rgr_mth_typ){
  case nco_rgr_mth_conservative: return "Conservative remapping";
  case nco_rgr_mth_bilinear: return "Bilinear remapping";
  case nco_rgr_mth_none: return "none";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_mth_sng() */

const char * /* O [sng] String describing regridding normalization */
nco_rgr_nrm_sng /* [fnc] Convert regridding normalization enum to string */
(const nco_rgr_nrm_typ_enm nco_rgr_nrm_typ) /* I [enm] Regridding normalization enum */
{
  /* Purpose: Convert regridding normalization enum to string */
  switch(nco_rgr_nrm_typ){
  case nco_rgr_nrm_fracarea: return "fracarea";
  case nco_rgr_nrm_destarea: return "destarea";
  case nco_rgr_nrm_none: return "none";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_nrm_sng() */

const char * /* O [sng] String containing regridding command and format */
nco_tps_cmd_fmt_sng /* [fnc] Convert Tempest remap command enum to command string */
(const nco_rgr_cmd_typ nco_rgr_cmd) /* I [enm] Tempest remap command enum */
{
  /* Purpose: Convert Tempest remap command enum to command string and format */
  switch(nco_rgr_cmd){
  case nco_rgr_ApplyOfflineMap:
    return "ApplyOfflineMap";
  case nco_rgr_CalculateDiffNorms:
    return "CalculateDiffNorms";
  case nco_rgr_GenerateCSMesh:
    return "GenerateCSMesh --res %d --file %s";
  case nco_rgr_GenerateGLLMetaData:
    return "GenerateGLLMetaData";
  case nco_rgr_GenerateICOMesh:
    return "GenerateICOMesh";
  case nco_rgr_GenerateLambertConfConicMesh:
    return "GenerateLambertConfConicMesh";
  case nco_rgr_GenerateOfflineMap:
    return "GenerateOfflineMap --in_mesh %s --out_mesh %s --ov_mesh %s --in_data %s --out_data %s";
  case nco_rgr_GenerateOverlapMesh:
    return "GenerateOverlapMesh --a %s --b %s --out %s";
  case nco_rgr_GenerateRLLMesh:
    return "GenerateRLLMesh --lat %d --lon %d --file %s";
  case nco_rgr_GenerateTestData:
    return "GenerateTestData --mesh %s --np %d --test %d --out %s";
  case nco_rgr_MeshToTxt:
    return "MeshToTxt";
  case nco_rgr_AAA_nil:
  case nco_rgr_ZZZ_last:
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */
  
  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_tps_cmd_fmt_sng() */

const char * /* O [sng] String containing regridding command name */
nco_tps_cmd_sng /* [fnc] Convert Tempest remap command enum to command name */
(const nco_rgr_cmd_typ nco_rgr_cmd) /* I [enm] Tempest remap command enum */
{
  /* Purpose: Convert Tempest remap command enum to command string */
  switch(nco_rgr_cmd){
  case nco_rgr_ApplyOfflineMap: return "ApplyOfflineMap";
  case nco_rgr_CalculateDiffNorms: return "CalculateDiffNorms";
  case nco_rgr_GenerateCSMesh: return "GenerateCSMesh";
  case nco_rgr_GenerateGLLMetaData: return "GenerateGLLMetaData";
  case nco_rgr_GenerateICOMesh: return "GenerateICOMesh";
  case nco_rgr_GenerateLambertConfConicMesh: return "GenerateLambertConfConicMesh";
  case nco_rgr_GenerateOfflineMap: return "GenerateOfflineMap";
  case nco_rgr_GenerateOverlapMesh: return "GenerateOverlapMesh";
  case nco_rgr_GenerateRLLMesh: return "GenerateRLLMesh";
  case nco_rgr_GenerateTestData: return "GenerateTestData";
  case nco_rgr_MeshToTxt: return "MeshToTxt";
  case nco_rgr_AAA_nil:
  case nco_rgr_ZZZ_last:
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_tps_cmd_sng() */

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
(rgr_sct * const rgr_nfo) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields
     ESMC is C-interface to ESMF documented at
     http://www.earthsystemmodeling.org/esmf_releases/last_built/ESMC_crefdoc/ESMC_crefdoc.html
     ESMF Developer's Guide
     http://www.earthsystemmodeling.org/documents/dev_guide
     ESMF_RegridWeightGen
     http://www.earthsystemcog.org/projects/regridweightgen
     http://www.earthsystemmodeling.org/python_releases/last_esmpy/esmpy_doc/html/index.html

     ESMF C-interface examples:
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridUTest.C
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridCsrvUTest.C */
  
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
  in_id=rgr_nfo->in_id;
  out_id=rgr_nfo->out_id;
  
  /* Obtain input longitude type and length */
  char lon_nm_in[]="lon";
  (void)nco_inq_varid(in_id,lon_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr_nfo->fl_in);
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
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr_nfo->fl_in);
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
    goto rgr_cln;
  } /* endif */
  
  /* Set log to flush after every message */
  rcd_esmf=ESMC_LogSet(ESMF_TRUE);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Obtain VM */
  ESMC_VM vm;
  vm=ESMC_VMGetGlobal(&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Set-up local PET info */
  /* Source: ${DATA}/esmf/src/Infrastructure/VM/interface/ESMC_VM.C */
  rcd_esmf=ESMC_VMGet(vm,&localPet,&petCount,&peCount,(MPI_Comm *)NULL,&flg_pthreadsEnabledFlag,&flg_openMPEnabledFlag);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

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
  grd_dst=ESMC_GridCreateFromFile(rgr_nfo->fl_grd_dst,grd_fl_typ,&flg_isSphere,&flg_addCornerStagger,&flg_addUserArea,&flg_addMask,var_nm,crd_nm,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  int *msk_dst; /* [enm] Destination grid mask */
  enum ESMC_GridItem_Flag grd_itm=ESMC_GRIDITEM_MASK;
  msk_dst=ESMC_GridGetItem(grd_dst,grd_itm,stg_lcn_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  int *bnd_lwr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_dst; /* [dgr] Destination grid longitude */
  double *lat_dst; /* [dgr] Destination grid latitude */
  /* 20150427: Written documentation is somewhat inadequate or misleading to normal C-programmers
     Some ESMC_Grid***() functions, like this one, return allocated void pointers that must be cast to desired numeric type
     Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  lon_dst=ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lon_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  lat_dst=ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lat_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

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
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  enum ESMC_CoordSys_Flag crd_sys=ESMC_COORDSYS_SPH_DEG; /* NB: dyw sez ESMF supports ESMC_COORDSYS_SPH_DEG only */
  enum ESMC_TypeKind_Flag typ_knd=ESMC_TYPEKIND_R8; /* NB: NCO default is to use double precision for coordinates */
  ESMC_Grid grd_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  grd_src=ESMC_GridCreateNoPeriDim(max_idx_src,&crd_sys,&typ_knd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* fxm: why destroy this now? */
  ESMC_InterfaceIntDestroy(&max_idx_src);
  /* Define stagger locations on source grid. Necessary for remapping later? */
  rcd_esmf=ESMC_GridAddCoord(grd_src,stg_lcn_src);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  
  /* Allocate */
  int *bnd_lwr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_src; /* [dgr] Source grid longitude */
  double *lat_src; /* [dgr] Source grid latitude  */
  lon_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lon_src,stg_lcn_src,bnd_lwr_src,bnd_upr_src,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  lat_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lat_src,stg_lcn_src,NULL,NULL,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"ESMC_VMGet(): localPet = %d, petCount = %d, peCount = %d, pthreadsEnabledFlag = %s, openMPEnabledFlag = %s\n",localPet,petCount,peCount,flg_pthreadsEnabledFlag ? "Enabled" : "Disabled",flg_openMPEnabledFlag ? "Enabled" : "Disabled");
    (void)fprintf(stderr,"ESMC_GridCreateFromFile(): filename = %s, fileTypeFlag = %s, isSphere = %s, addCornerStagger = %s, addUserArea = %s, addMask = %s, var_nm = %s, crd_nm = %s\n",rgr_nfo->fl_grd_dst,nco_esmf_fl_fmt_sng(grd_fl_typ),flg_isSphere ? "Enabled" : "Disabled",flg_addCornerStagger ? "Enabled" : "Disabled",flg_addUserArea ? "Enabled" : "Disabled",flg_addMask ? "Enabled" : "Disabled",var_nm,crd_nm ? "non-NULL" : "NULL");
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
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  /* Create destination field from destination grid */
  ESMC_Field fld_dst;
  fld_dst=ESMC_FieldCreateGridTypeKind(grd_dst,typ_knd,stg_lcn_dst,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_dst",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Get field pointers */
  double *fld_src_ptr;
  int localDe=0; /* [idx] Local DE for which information is requested [0..localDeCount-1] */
  fld_src_ptr=(double *)ESMC_FieldGetPtr(fld_src,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;
  double *fld_dst_ptr;
  fld_dst_ptr=(double *)ESMC_FieldGetPtr(fld_dst,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Get variables from input file */
  (void)nco_inq_varid(in_id,rgr_nfo->var_nm,&var_in_id);
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
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  enum ESMC_Region_Flag rgn_flg=ESMC_REGION_TOTAL; /* [flg] Whether/how to zero input fields before regridding (default ESMC_REGION_TOTAL) */
  rcd_esmf=ESMC_FieldRegrid(fld_src,fld_dst,rte_hnd,&rgn_flg);
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

  /* Call once on each PET */
  rcd_esmf=ESMC_Finalize();
  if(rcd_esmf != ESMF_SUCCESS) goto rgr_cln;

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
  (void)nco_def_var(out_id,rgr_nfo->var_nm,var_typ_out,(int)2,dmn_ids_out,&var_out_id);
  (void)nco_enddef(out_id);
  dmn_srt_out[crd_idx_c_0bs_lat_dst]=0L;
  dmn_srt_out[crd_idx_c_0bs_lon_dst]=0L;
  dmn_cnt_out[crd_idx_c_0bs_lat_dst]=bnd_upr_dst[crd_idx_f_0bs_lat_dst];
  dmn_cnt_out[crd_idx_c_0bs_lon_dst]=bnd_upr_dst[crd_idx_f_0bs_lon_dst];
  (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_out,lat_out,crd_typ_out);
  (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_out,lon_out,crd_typ_out);
  (void)nco_put_vara(out_id,var_out_id,dmn_srt_out,dmn_cnt_out,fld_dst_ptr,var_typ_out);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(rgr_nfo->fl_out,rgr_nfo->fl_out_tmp,out_id);

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
} /* nco_rgr_esmf */
#endif /* !ENABLE_ESMF */

