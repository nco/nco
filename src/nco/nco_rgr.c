/* $Header$ */

/* Purpose: NCO regridding utilities */

/* Copyright (C) 2015--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_rgr.h" /* Regridding */

extern double min_dbl(double a, double b);
extern double max_dbl(double a, double b);
inline double min_dbl(double a, double b){return (a < b) ? a : b;}
inline double max_dbl(double a, double b){return (a > b) ? a : b;}

int /* O [enm] Return code */
nco_rgr_ctl /* [fnc] Control regridding logic */
(rgr_sct * const rgr, /* I/O [sct] Regridding structure */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal Table */
{
  /* Purpose: Control regridding logic */
  int rcd=NCO_NOERR;
  const char fnc_nm[]="nco_rgr_ctl()";

  nco_bool flg_grd=False; /* [flg] Create SCRIP-format grid file */
  nco_bool flg_map=False; /* [flg] Create ESMF-format mapfile */
  nco_bool flg_nfr=False; /* [flg] Infer SCRIP-format grid file */
  nco_bool flg_smf=False; /* [flg] ESMF regridding (unused) */
  nco_bool flg_s1d=False; /* [flg] Unpack sparse-1D CLM/ELM variables */
  nco_bool flg_tps=False; /* [flg] Tempest regridding (unused) */
  nco_bool flg_vrt=False; /* [flg] Interpolate to new vertical grid */
  nco_bool flg_wgt=False; /* [flg] Regrid with external weights */

  /* Main control branching occurs here
     Branching complexity and utility will increase as regridding features are added */
  if(rgr->flg_grd) flg_grd=True;
  if(rgr->flg_grd_src && rgr->flg_grd_dst && rgr->flg_wgt) flg_map=True;
  if(rgr->flg_nfr) flg_nfr=True;
  if(rgr->flg_wgt && !(rgr->flg_grd_src && rgr->flg_grd_dst)) flg_wgt=True;
  if(rgr->flg_s1d) flg_s1d=True;
  if(rgr->fl_vrt) flg_vrt=True;
  assert(!flg_smf);
  assert(!flg_tps);
  
  /* Create SCRIP-format grid file */
  if(flg_grd) rcd=nco_grd_mk(rgr);

  /* Create ESMF-format map file */
  if(flg_map) rcd=nco_map_mk(rgr);

  /* Infer SCRIP-format grid file from data file */
  if(flg_nfr) rcd=nco_grd_nfr(rgr);

  /* Interpolate data file to new vertical grid */
  if(flg_vrt) rcd=nco_ntp_vrt(rgr,trv_tbl);

  /* Unpack sparse-1D CLM/ELM variables into full file */
  if(flg_s1d) rcd=nco_s1d_unpack(rgr,trv_tbl);

  /* Regrid data horizontally using weights from mapping file */
  if(flg_wgt) rcd=nco_rgr_wgt(rgr,trv_tbl);

  /* Regrid using ESMF library
     20150701: On-line weight generation with ESMF never worked well and was abandoned */
  if(flg_smf){
#ifdef ENABLE_ESMF
    (void)fprintf(stderr,"%s: %s calling nco_rgr_esmf() to generate and apply regridding map\n",nco_prg_nm_get(),fnc_nm);
    rcd=nco_rgr_esmf(rgr);
    /* Close output and free dynamic memory */
    (void)nco_fl_out_cls(rgr->fl_out,rgr->fl_out_tmp,rgr->out_id);
#else /* !ENABLE_ESMF */
    (void)fprintf(stderr,"%s: ERROR %s reports attempt to use ESMF regridding without built-in support. Re-configure with --enable_esmf.\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
#endif /* !ENABLE_ESMF */
  } /* !flg_smf */
  
  /* Regrid using TempestRemap regridding
     20180314: Weight generation with Tempest is implemented off-line via ncremap, not internally on-line
     However, do not deprecate this since TempestRemap2 has a library that could be accessed on-line */
  if(flg_tps) rcd=nco_rgr_tps(rgr);

  return rcd;
} /* end nco_rgr_ctl() */

rgr_sct * /* O [sct] Pointer to free'd regridding structure */
nco_rgr_free /* [fnc] Deallocate regridding structure */
(rgr_sct *rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Free all dynamic memory in regridding structure */

  /* free() standalone command-line arguments */
  if(rgr->cmd_ln) rgr->cmd_ln=(char *)nco_free(rgr->cmd_ln);
  if(rgr->grd_ttl) rgr->grd_ttl=(char *)nco_free(rgr->grd_ttl);
  if(rgr->fl_grd_src) rgr->fl_grd_src=(char *)nco_free(rgr->fl_grd_src);
  if(rgr->fl_grd_dst) rgr->fl_grd_dst=(char *)nco_free(rgr->fl_grd_dst);
  if(rgr->fl_hrz) rgr->fl_hrz=(char *)nco_free(rgr->fl_hrz);
  if(rgr->fl_in) rgr->fl_in=(char *)nco_free(rgr->fl_in);
  if(rgr->fl_map) rgr->fl_map=(char *)nco_free(rgr->fl_map);
  if(rgr->fl_msh) rgr->fl_msh=(char *)nco_free(rgr->fl_msh);
  if(rgr->fl_out) rgr->fl_out=(char *)nco_free(rgr->fl_out);
  if(rgr->fl_out_tmp) rgr->fl_out_tmp=(char *)nco_free(rgr->fl_out_tmp);
  if(rgr->fl_vrt) rgr->fl_vrt=(char *)nco_free(rgr->fl_vrt);

  if(rgr->var_nm) rgr->var_nm=(char *)nco_free(rgr->var_nm);
  if(rgr->xtn_var) rgr->xtn_var=(char **)nco_sng_lst_free(rgr->xtn_var,rgr->xtn_nbr);

  /* free() strings associated with grid properties */
  if(rgr->fl_grd) rgr->fl_grd=(char *)nco_free(rgr->fl_grd);
  if(rgr->fl_hnt_dst) rgr->fl_hnt_dst=(char *)nco_free(rgr->fl_hnt_dst);
  if(rgr->fl_hnt_src) rgr->fl_hnt_src=(char *)nco_free(rgr->fl_hnt_src);
  if(rgr->fl_skl) rgr->fl_skl=(char *)nco_free(rgr->fl_skl);
  if(rgr->fl_ugrid) rgr->fl_ugrid=(char *)nco_free(rgr->fl_ugrid);

  /* Tempest */
  if(rgr->drc_tps) rgr->drc_tps=(char *)nco_free(rgr->drc_tps);

  /* free() memory used to construct KVMs */
  if(rgr->rgr_nbr > 0) rgr->rgr_arg=nco_sng_lst_free(rgr->rgr_arg,rgr->rgr_nbr);

  /* free() memory copied from KVMs */
  if(rgr->area_nm) rgr->area_nm=(char *)nco_free(rgr->area_nm);
  if(rgr->bnd_nm) rgr->bnd_nm=(char *)nco_free(rgr->bnd_nm);
  if(rgr->bnd_tm_nm) rgr->bnd_tm_nm=(char *)nco_free(rgr->bnd_tm_nm);
  if(rgr->col_nm_in) rgr->col_nm_in=(char *)nco_free(rgr->col_nm_in);
  if(rgr->col_nm_out) rgr->col_nm_out=(char *)nco_free(rgr->col_nm_out);
  if(rgr->frc_nm) rgr->frc_nm=(char *)nco_free(rgr->frc_nm);
  if(rgr->ilev_nm_in) rgr->ilev_nm_in=(char *)nco_free(rgr->ilev_nm_in);
  if(rgr->ilev_nm_out) rgr->ilev_nm_out=(char *)nco_free(rgr->ilev_nm_out);
  if(rgr->lat_bnd_nm) rgr->lat_bnd_nm=(char *)nco_free(rgr->lat_bnd_nm);
  if(rgr->lat_nm_in) rgr->lat_nm_in=(char *)nco_free(rgr->lat_nm_in);
  if(rgr->lat_nm_out) rgr->lat_nm_out=(char *)nco_free(rgr->lat_nm_out);
  if(rgr->lat_vrt_nm) rgr->lat_vrt_nm=(char *)nco_free(rgr->lat_vrt_nm);
  if(rgr->lat_wgt_nm) rgr->lat_wgt_nm=(char *)nco_free(rgr->lat_wgt_nm);
  if(rgr->lev_nm_in) rgr->lev_nm_in=(char *)nco_free(rgr->lev_nm_in);
  if(rgr->lev_nm_out) rgr->lev_nm_out=(char *)nco_free(rgr->lev_nm_out);
  if(rgr->lon_bnd_nm) rgr->lon_bnd_nm=(char *)nco_free(rgr->lon_bnd_nm);
  if(rgr->lon_nm_in) rgr->lon_nm_in=(char *)nco_free(rgr->lon_nm_in);
  if(rgr->lon_nm_out) rgr->lon_nm_out=(char *)nco_free(rgr->lon_nm_out);
  if(rgr->lon_vrt_nm) rgr->lon_vrt_nm=(char *)nco_free(rgr->lon_vrt_nm);
  if(rgr->msk_nm) rgr->msk_nm=(char *)nco_free(rgr->msk_nm);
  if(rgr->plev_nm_in) rgr->plev_nm_in=(char *)nco_free(rgr->plev_nm_in);
  if(rgr->vrt_nm) rgr->vrt_nm=(char *)nco_free(rgr->vrt_nm);

  /* Lastly, free() regrid structure itself */
  if(rgr) rgr=(rgr_sct *)nco_free(rgr);

  return rgr;
} /* end nco_rgr_free() */
  
rgr_sct * /* O [sct] Regridding structure */
nco_rgr_ini /* [fnc] Initialize regridding structure */
(const char * const cmd_ln, /* I [sng] Command-line */
 const int in_id, /* I [id] Input netCDF file ID */
 char **rgr_arg, /* [sng] Regridding arguments */
 const int rgr_arg_nbr, /* [nbr] Number of regridding arguments */
 char * const rgr_in, /* I [sng] File containing fields to be regridded */
 char * const rgr_out, /* I [sng] File containing regridded fields */
 char * const rgr_grd_src, /* I [sng] File containing input grid */
 char * const rgr_grd_dst, /* I [sng] File containing destination grid */
 char * const rgr_hrz, /* I [sng] File containing horizontal coordinate grid */
 char * const rgr_map, /* I [sng] File containing mapping weights from source to destination grid */
 char * const rgr_var, /* I [sng] Variable for special regridding treatment */
 char * const rgr_vrt, /* I [sng] File containing vertical coordinate grid */
 const double wgt_vld_thr, /* I [frc] Weight threshold for valid destination value */
 char **xtn_var, /* [sng] I Extensive variables */
 const int xtn_nbr) /* [nbr] I Number of extensive variables */
{
  /* Purpose: Initialize regridding structure */
     
  const char fnc_nm[]="nco_rgr_ini()";
  
  rgr_sct *rgr;

  /* Allocate */
  rgr=(rgr_sct *)nco_malloc(sizeof(rgr_sct));
  
  /* Initialize variables directly or indirectly set via command-line (except for key-value arguments) */
  rgr->cmd_ln=strdup(cmd_ln); /* [sng] Command-line */

  rgr->flg_usr_rqs=False; /* [flg] User requested regridding */
  rgr->out_id=int_CEWI; /* [id] Output netCDF file ID */

  rgr->in_id=in_id; /* [id] Input netCDF file ID */
  rgr->rgr_arg=rgr_arg; /* [sng] Regridding arguments */
  rgr->rgr_nbr=rgr_arg_nbr; /* [nbr] Number of regridding arguments */

  rgr->drc_tps=NULL; /* [sng] Directory where Tempest grids, meshes, and weights are stored */

  rgr->flg_grd_src= rgr_grd_src ? True : False; /* [flg] User-specified input grid */
  rgr->fl_grd_src=rgr_grd_src; /* [sng] File containing input grid */

  rgr->flg_grd_dst= rgr_grd_dst ? True : False; /* [flg] User-specified destination grid */
  rgr->fl_grd_dst=rgr_grd_dst; /* [sng] File containing destination grid */

  rgr->fl_in=rgr_in; /* [sng] File containing fields to be regridded */
  rgr->fl_out=rgr_out; /* [sng] File containing regridded fields */
  rgr->fl_out_tmp=NULL_CEWI; /* [sng] Temporary file containing regridded fields */

  rgr->flg_wgt= rgr_map ? True : False; /* [flg] User-specified mapping weights */
  rgr->fl_map=rgr_map; /* [sng] File containing mapping weights from source to destination grid */

  rgr->fl_hrz=rgr_hrz; /* [sng] [sng] File containing horizontal coordinate grid (for S1D) */
  rgr->fl_vrt=rgr_vrt; /* [sng] [sng] File containing vertical coordinate grid */

  rgr->var_nm=rgr_var; /* [sng] Variable for special regridding treatment */
  
  rgr->xtn_var=xtn_var; /* [sng] Extensive variables */
  rgr->xtn_nbr=xtn_nbr; /* [nbr] Number of extensive variables */

  /* Did user explicitly request regridding? */
  if(rgr_arg_nbr > 0 || rgr_grd_src != NULL || rgr_grd_dst != NULL || rgr_map != NULL || rgr_vrt != NULL) rgr->flg_usr_rqs=True;

  /* Initialize arguments after copying */
  if(!rgr->fl_out) rgr->fl_out=(char *)strdup("/data/zender/rgr/rgr_out.nc");
  if(!rgr->fl_grd_dst) rgr->fl_grd_dst=(char *)strdup("/data/zender/scrip/grids/remap_grid_T42.nc");
  //  if(!rgr->var_nm) rgr->var_nm=(char *)strdup("ORO");
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"flg_usr_rqs = %d, ",rgr->flg_usr_rqs);
    (void)fprintf(stderr,"rgr_nbr = %d, ",rgr->rgr_nbr);
    (void)fprintf(stderr,"fl_grd_src = %s, ",rgr->fl_grd_src ? rgr->fl_grd_src : "NULL");
    (void)fprintf(stderr,"fl_grd_dst = %s, ",rgr->fl_grd_dst ? rgr->fl_grd_dst : "NULL");
    (void)fprintf(stderr,"fl_hrz = %s, ",rgr->fl_hrz ? rgr->fl_hrz : "NULL");
    (void)fprintf(stderr,"fl_in = %s, ",rgr->fl_in ? rgr->fl_in : "NULL");
    (void)fprintf(stderr,"fl_out = %s, ",rgr->fl_out ? rgr->fl_out : "NULL");
    (void)fprintf(stderr,"fl_out_tmp = %s, ",rgr->fl_out_tmp ? rgr->fl_out_tmp : "NULL");
    (void)fprintf(stderr,"fl_map = %s, ",rgr->fl_map ? rgr->fl_map : "NULL");
    (void)fprintf(stderr,"fl_vrt = %s, ",rgr->fl_vrt ? rgr->fl_vrt : "NULL");
    (void)fprintf(stderr,"\n");
  } /* endif dbg */
  
  /* Flags */
  if(wgt_vld_thr == NC_MIN_DOUBLE){
    rgr->flg_rnr=False;
  }else if(wgt_vld_thr >= 0.0 && wgt_vld_thr <= 1.0){
    /* NB: Weight thresholds of 0.0 or nearly zero can lead to underflow or divide-by-zero errors */
    // const double wgt_vld_thr_min=1.0e-10; /* [frc] Minimum weight threshold for valid destination value */
    rgr->flg_rnr=True;
    rgr->wgt_vld_thr=wgt_vld_thr;
  }else{
    (void)fprintf(stderr,"%s: ERROR weight threshold must be in [0.0,1.0] and user supplied wgt_vld_thr = %g\n",nco_prg_nm_get(),wgt_vld_thr);
    nco_exit(EXIT_FAILURE);
  } /* endif */
  
  /* Parse extended kvm options */
  char *sng_fnl=NULL;
  int cnv_nbr; /* [nbr] Number of elements converted by sscanf() */
  int rgr_var_idx; /* [idx] Index over rgr_lst (i.e., all names explicitly specified in all "--rgr var1[,var2]=val" options) */
  int rgr_var_nbr=0;
  kvm_sct *rgr_lst=NULL; /* [sct] List of all regrid specifications */

  if(rgr_arg_nbr > 0){
    /* Join arguments together */
    sng_fnl=nco_join_sng(rgr_arg,rgr_arg_nbr);
    rgr_lst=nco_arg_mlt_prs(sng_fnl);
    if(sng_fnl) sng_fnl=(char *)nco_free(sng_fnl);

    /* Count number of keys */
    for(rgr_var_idx=0;(rgr_lst+rgr_var_idx)->key;rgr_var_idx++,rgr_var_nbr++);/* !rgr_var_idx */
  } /* !rgr_arg_nbr */

  /* NULL-initialize key-value properties required for string variables */
  rgr->area_nm=NULL; /* [sng] Name of variable containing gridcell area */
  rgr->bnd_nm=NULL; /* [sng] Name of dimension to employ for spatial bounds */
  rgr->bnd_tm_nm=NULL; /* [sng] Name of dimension to employ for temporal bounds */
  rgr->col_nm_in=NULL; /* [sng] Name to recognize as input horizontal spatial dimension on unstructured grid */
  rgr->col_nm_out=NULL; /* [sng] Name of horizontal spatial output dimension on unstructured grid */
  rgr->frc_nm=NULL; /* [sng] Name of variable containing gridcell fraction */
  rgr->ilev_nm_in=NULL; /* [sng] Name of input dimension to recognize as vertical dimension at layer interfaces */
  rgr->ilev_nm_out=NULL; /* [sng] Name of output vertical dimension at layer interfaces */
  rgr->lat_bnd_nm=NULL; /* [sng] Name of rectangular boundary variable for latitude */
  rgr->lat_dmn_nm=NULL; /* [sng] Name of latitude dimension in inferred grid */
  rgr->lat_nm_in=NULL; /* [sng] Name of input dimension to recognize as latitude */
  rgr->lat_nm_out=NULL; /* [sng] Name of output dimension for latitude */
  rgr->lat_vrt_nm=NULL; /* [sng] Name of non-rectangular boundary variable for latitude */
  rgr->lat_wgt_nm=NULL; /* [sng] Name of variable containing latitude weights */
  rgr->lev_nm_in=NULL; /* [sng] Name of input dimension to recognize as vertical dimension at layer midpoints */
  rgr->lev_nm_out=NULL; /* [sng] Name of output vertical dimension at layer midpoints */
  rgr->lon_bnd_nm=NULL; /* [sng] Name of rectangular boundary variable for longitude */
  rgr->lon_dmn_nm=NULL; /* [sng] Name of longitude dimension in inferred grid */
  rgr->lon_nm_in=NULL; /* [sng] Name of dimension to recognize as longitude */
  rgr->lon_nm_out=NULL; /* [sng] Name of output dimension for longitude */
  rgr->lon_vrt_nm=NULL; /* [sng] Name of non-rectangular boundary variable for longitude */
  rgr->msk_nm=NULL; /* [sng] Name of variable containing destination mask */
  rgr->plev_nm_in=NULL; /* [sng] Name of input variable recognize as pure-pressure coordinate */
  rgr->sgs_frc_nm=NULL; /* [sng] Name of variable sub-gridscale fraction */
  rgr->sgs_msk_nm=NULL; /* [sng] Name of variable sub-gridscale mask */
  rgr->vrt_nm=NULL; /* [sng] Name of dimension to employ for vertices */

  /* Initialize key-value properties used in grid and weight generation */
  rgr->area_mth=1; /* [enm] Method to compute grid cell area */
  rgr->edg_typ=nco_edg_nil; /* [enm] Edge/Arc-type for triangle edges */
  rgr->fl_grd=NULL; /* [sng] Name of SCRIP grid file to create */
  rgr->fl_hnt_dst=NULL; /* [sng] ERWG hint destination */
  rgr->fl_hnt_src=NULL; /* [sng] ERWG hint source */
  rgr->fl_msh=NULL; /* [sng] Name of SCRIP intersection mesh file to create */
  rgr->fl_skl=NULL; /* [sng] Name of skeleton data file to create */
  rgr->fl_ugrid=NULL; /* [sng] Name of UGRID grid file to create */
  rgr->flg_area_out=True; /* [flg] Add area to output */
  rgr->flg_cf_units=False; /* [flg] Generate CF-compliant (breaks ERWG 7.1.0r-) units fields in SCRIP-format grid files */
  rgr->flg_cll_msr=True; /* [flg] Add cell_measures attribute */
  rgr->flg_crv=False; /* [flg] Use curvilinear coordinates */
  rgr->flg_dgn_area=False; /* [flg] Diagnose rather than copy inferred area */
  rgr->flg_dgn_bnd=False; /* [flg] Diagnose rather than copy inferred bounds */
  rgr->flg_erwg_units=True; /* [flg] Generate ERWG 7.1.0r-compliant SCRIP-format grid files */
  rgr->flg_grd=False; /* [flg] Create SCRIP-format grid file */
  rgr->flg_msk_out=False; /* [flg] Add mask to output */
  rgr->flg_nfr=False; /* [flg] Infer SCRIP-format grid file */
  rgr->flg_s1d=False; /* [flg] Unpack sparse-1D CLM/ELM variables */
  rgr->flg_stg=True; /* [flg] Write staggered grid with FV output */
  rgr->grd_ttl=strdup("None given (supply with --rgr grd_ttl=\"Grid Title\")"); /* [enm] Grid title */
  rgr->grd_typ=nco_grd_2D_eqa; /* [enm] Grid type */
  rgr->idx_dbg=0; /* [idx] Index of gridcell for debugging */
  rgr->lat_drc=nco_grd_lat_drc_s2n; /* [enm] Latitude grid direction */
  rgr->lat_typ=nco_grd_lat_eqa; /* [enm] Latitude grid type */
  rgr->lon_typ=nco_grd_lon_Grn_ctr; /* [enm] Longitude grid type */
  rgr->lat_nbr=180; /* [nbr] Number of latitudes in destination grid */
  rgr->lon_nbr=360; /* [nbr] Number of longitudes in destination grid */
  rgr->lat_crv=0.0; /* [dgr] Latitudinal  curvilinearity */
  rgr->lon_crv=0.0; /* [dgr] Longitudinal curvilinearity */
  rgr->lat_sth=NC_MAX_DOUBLE; /* [dgr] Latitude of southern edge of grid */
  rgr->lon_wst=NC_MAX_DOUBLE; /* [dgr] Longitude of western edge of grid */
  rgr->lat_nrt=NC_MAX_DOUBLE; /* [dgr] Latitude of northern edge of grid */
  rgr->lon_est=NC_MAX_DOUBLE; /* [dgr] Longitude of eastern edge of grid */
  rgr->msk_var=NULL; /* [sng] Mask-template variable */
  rgr->ply_tri_mth=nco_ply_tri_mth_csz; /* [enm] Polygon-to-triangle decomposition method */ 
  rgr->sgs_nrm=1.0; /* [sng] Sub-gridscale normalization */
  rgr->tst=0L; /* [enm] Generic key for testing (undocumented) */
  rgr->ntp_mth=nco_ntp_log; /* [enm] Interpolation method */
  rgr->xtr_mth=nco_xtr_fll_ngh; /* [enm] Extrapolation method */
  rgr->xtr_nsp=8; /* [sng] Extrapolation number of source points */
  rgr->xtr_xpn=2.0; /* [sng] Exponent of distance in extrapolation (absolute value) */
  rgr->wgt_typ=nco_wgt_con; /* [enm] Weight generation method */

  /* Parse key-value properties */
  char *sng_cnv_rcd=NULL_CEWI; /* [sng] strtol()/strtoul() return code */
  for(rgr_var_idx=0;rgr_var_idx<rgr_var_nbr;rgr_var_idx++){
    if(!strcmp(rgr_lst[rgr_var_idx].key,"grid") || !strcasecmp(rgr_lst[rgr_var_idx].key,"scrip")){
      rgr->fl_grd=(char *)strdup(rgr_lst[rgr_var_idx].val);
      rgr->flg_grd=True;
      continue;
    } /* !grid */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"hnt_dst") || !strcmp(rgr_lst[rgr_var_idx].key,"fl_hnt_dst")){
      rgr->fl_hnt_dst=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !hnt_dst */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"hnt_src") || !strcmp(rgr_lst[rgr_var_idx].key,"fl_hnt_src")){
      rgr->fl_hnt_src=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !hnt_src */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"msk_var") || !strcmp(rgr_lst[rgr_var_idx].key,"mask_var") || !strcmp(rgr_lst[rgr_var_idx].key,"mask") || !strcmp(rgr_lst[rgr_var_idx].key,"mask_variable")){
      rgr->msk_var=(char *)strdup(rgr_lst[rgr_var_idx].val);
      rgr->flg_msk_out=True;
      continue;
    } /* !msk_var */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"msh") || !strcmp(rgr_lst[rgr_var_idx].key,"mesh")){
      rgr->fl_msh=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !msh */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"skl")){
      rgr->fl_skl=(char *)strdup(rgr_lst[rgr_var_idx].val);
      rgr->flg_grd=True;
      continue;
    } /* !skl */
    if(!strcasecmp(rgr_lst[rgr_var_idx].key,"ugrid")){
      rgr->fl_ugrid=(char *)strdup(rgr_lst[rgr_var_idx].val);
      rgr->flg_nfr=True;
      continue;
    } /* !ugrid */
    if(!strcasecmp(rgr_lst[rgr_var_idx].key,"fl_hrz") || !strcasecmp(rgr_lst[rgr_var_idx].key,"hrz")){
      rgr->fl_hrz=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !hrz */
    if(!strcasecmp(rgr_lst[rgr_var_idx].key,"fl_vrt") || !strcasecmp(rgr_lst[rgr_var_idx].key,"vrt")){
      rgr->fl_vrt=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !vrt */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"no_area") || !strcmp(rgr_lst[rgr_var_idx].key,"no_area_out")){
      rgr->flg_area_out=False;
      continue;
    } /* !area */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"no_msk") || !strcmp(rgr_lst[rgr_var_idx].key,"no_msk_out") || !strcmp(rgr_lst[rgr_var_idx].key,"no_mask") || !strcmp(rgr_lst[rgr_var_idx].key,"no_mask_out")){
      rgr->flg_msk_out=False;
      continue;
    } /* !msk */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"msk_out") || !strcmp(rgr_lst[rgr_var_idx].key,"mask_out")){
      rgr->flg_msk_out=True;
      continue;
    } /* !mask */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"cell_measures") || !strcmp(rgr_lst[rgr_var_idx].key,"cll_msr")){
      rgr->flg_cll_msr=True;
      continue;
    } /* !cell_measures */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"no_cell_measures") || !strcmp(rgr_lst[rgr_var_idx].key,"no_cll_msr")){
      rgr->flg_cll_msr=False;
      continue;
    } /* !cell_measures */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"curvilinear") || !strcmp(rgr_lst[rgr_var_idx].key,"crv")){
      rgr->flg_crv=True;
      continue;
    } /* !curvilinear */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"diagnose_area") || !strcmp(rgr_lst[rgr_var_idx].key,"dgn_area")){
      rgr->flg_dgn_area=True;
      continue;
    } /* !diagnose_area */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"diagnose_bounds") || !strcmp(rgr_lst[rgr_var_idx].key,"dgn_bnd")){
      rgr->flg_dgn_bnd=True;
      continue;
    } /* !diagnose_bounds */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"cf_units") || !strcmp(rgr_lst[rgr_var_idx].key,"CF_units")){
      rgr->flg_cf_units=True;
      rgr->flg_erwg_units=False;
      continue;
    } /* !erwg_units */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"cell_area_quad")){
      rgr->area_mth=2;
      continue;
    } /* !area_nco */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"cell_area_nco")){
      rgr->area_mth=1;
      continue;
    } /* !area_nco */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"edg_typ") || !strcmp(rgr_lst[rgr_var_idx].key,"tri_arc") || !strcmp(rgr_lst[rgr_var_idx].key,"vrt_cnc")){
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"grt_crc") || !strcasecmp(rgr_lst[rgr_var_idx].val,"gtc") || !strcasecmp(rgr_lst[rgr_var_idx].val,"great_circle") || !strcasecmp(rgr_lst[rgr_var_idx].val,"geodesic") || !strcasecmp(rgr_lst[rgr_var_idx].val,"orthodrome")){
	rgr->edg_typ=nco_edg_gtc;
      }else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"sml_crc") || !strcasecmp(rgr_lst[rgr_var_idx].val,"ltr") || !strcasecmp(rgr_lst[rgr_var_idx].val,"small_circle") || !strcasecmp(rgr_lst[rgr_var_idx].val,"latitude_triangle") || !strcasecmp(rgr_lst[rgr_var_idx].val,"true")){
	rgr->edg_typ=nco_edg_smc;
	(void)fprintf(stderr,"%s: WARNING Requested to run with small-circle edges. This option has not yet been tested and validated. Use only at your own risk.\n",nco_prg_nm_get());
      }else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"crt") || !strcasecmp(rgr_lst[rgr_var_idx].val,"cartesian") || !strcasecmp(rgr_lst[rgr_var_idx].val,"planar") || !strcasecmp(rgr_lst[rgr_var_idx].val,"flat")){
	rgr->edg_typ=nco_edg_crt;
      }else{
	(void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
	abort();
      } /* !val */
      continue;
    } /* !edg_typ */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"erwg_units") || !strcmp(rgr_lst[rgr_var_idx].key,"esmf_units") || !strcmp(rgr_lst[rgr_var_idx].key,"degrees")){
      rgr->flg_cf_units=False;
      rgr->flg_erwg_units=True;
      continue;
    } /* !erwg_units */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"infer") || !strcmp(rgr_lst[rgr_var_idx].key,"nfr")){
      rgr->flg_nfr=True;
      continue;
    } /* !infer */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"no_stagger") || !strcmp(rgr_lst[rgr_var_idx].key,"no_stg")){
      rgr->flg_stg=False;
      continue;
    } /* !stagger */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"grd_ttl") || !strcmp(rgr_lst[rgr_var_idx].key,"ttl")){
      if(rgr->grd_ttl) rgr->grd_ttl=(char *)nco_free(rgr->grd_ttl);
      rgr->grd_ttl=(char *)strdup(rgr_lst[rgr_var_idx].val);
      /* 20180828 Replace unquoted tildes with spaces (like LaTeX, NCL) so ncremap users can put tildes in place of spaces in ttl
	 20180905 Reverted this since quoting command in ncremap is superior solution */
      if(False){
	size_t ttl_lng=strlen(rgr->grd_ttl);
	for(size_t ttl_idx=0L;ttl_idx<ttl_lng;ttl_idx++)
	  if(rgr->grd_ttl[ttl_idx] == '~'){
	    if(ttl_idx == 0L) rgr->grd_ttl[ttl_idx]=' '; // Always convert tilde to space if first character
	    else if(rgr->grd_ttl[ttl_idx-1L] != '\\') rgr->grd_ttl[ttl_idx]=' '; // Convert tilde in other locations unless backslash-quoted
	  } /* !tilde */
      } /* !0 */
      continue;
    } /* !grd_ttl */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"idx_dbg")){
      rgr->idx_dbg=strtol(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtol",sng_cnv_rcd);
      continue;
    } /* !idx_dbg */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"latlon")){
      cnv_nbr=sscanf(rgr_lst[rgr_var_idx].val,"%ld,%ld",&rgr->lat_nbr,&rgr->lon_nbr);
      assert(cnv_nbr == 2);
      continue;
    } /* !latlon */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lonlat")){
      cnv_nbr=sscanf(rgr_lst[rgr_var_idx].val,"%ld,%ld",&rgr->lon_nbr,&rgr->lat_nbr);
      assert(cnv_nbr == 2);
      continue;
    } /* !lonlat */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_nbr")){
      rgr->lat_nbr=strtol(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtol",sng_cnv_rcd);
      continue;
    } /* !lat_nbr */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_nbr")){
      rgr->lon_nbr=strtol(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtol",sng_cnv_rcd);
      continue;
    } /* !lon_nbr */
    if(!strcasecmp(rgr_lst[rgr_var_idx].key,"snwe")){
      cnv_nbr=sscanf(rgr_lst[rgr_var_idx].val,"%lf,%lf,%lf,%lf",&rgr->lat_sth,&rgr->lat_nrt,&rgr->lon_wst,&rgr->lon_est);
      if(cnv_nbr != 4) (void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
      assert(cnv_nbr == 4);
      if(cnv_nbr != 4) abort(); /* CEWI Use cnv_nbr at least once outside of assert() to avoid gcc 4.8.2 set-but-not-used warning */
      continue;
    } /* !snwe */
    if(!strcasecmp(rgr_lst[rgr_var_idx].key,"wesn")){
      if(cnv_nbr != 4) cnv_nbr=sscanf(rgr_lst[rgr_var_idx].val,"%lf,%lf,%lf,%lf",&rgr->lon_wst,&rgr->lon_est,&rgr->lat_sth,&rgr->lat_nrt);
      assert(cnv_nbr == 4);
      continue;
    } /* !wesn */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_crv")){
      rgr->lat_crv=strtod(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtod",sng_cnv_rcd);
      continue;
    } /* !lat_crv */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_crv")){
      rgr->lon_crv=strtod(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtod",sng_cnv_rcd);
      continue;
    } /* !lon_crv */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_sth")){
      rgr->lat_sth=strtod(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtod",sng_cnv_rcd);
      //      rgr->lat_typ=nco_grd_lat_bb;
      continue;
    } /* !lat_sth */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_wst")){
      rgr->lon_wst=strtod(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtod",sng_cnv_rcd);
      rgr->lon_typ=nco_grd_lon_bb;
      continue;
    } /* !lon_wst */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_nrt")){
      rgr->lat_nrt=strtod(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtod",sng_cnv_rcd);
      //rgr->lat_typ=nco_grd_lat_bb;
      continue;
    } /* !lat_nrt */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_est")){
      rgr->lon_est=strtod(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtod",sng_cnv_rcd);
      rgr->lon_typ=nco_grd_lon_bb;
      continue;
    } /* !lon_est */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_drc")){
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"s2n") || !strcasecmp(rgr_lst[rgr_var_idx].val,"south2north") || !strcasecmp(rgr_lst[rgr_var_idx].val,"ston") || !strcasecmp(rgr_lst[rgr_var_idx].val,"southnorth")){
	rgr->lat_drc=nco_grd_lat_drc_s2n;
      }else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"n2s") || !strcasecmp(rgr_lst[rgr_var_idx].val,"north2south") || !strcasecmp(rgr_lst[rgr_var_idx].val,"ntos") || !strcasecmp(rgr_lst[rgr_var_idx].val,"northsouth")){
	rgr->lat_drc=nco_grd_lat_drc_n2s;
      }else{
	(void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
	abort();
      } /* !val */
      continue;
    } /* !lat_drc */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_typ")){
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"cap") || !strcasecmp(rgr_lst[rgr_var_idx].val,"fv") || !strcasecmp(rgr_lst[rgr_var_idx].val,"fix") || !strcasecmp(rgr_lst[rgr_var_idx].val,"yarmulke")){
	rgr->lat_typ=nco_grd_lat_fv;
	rgr->grd_typ=nco_grd_2D_fv;
      }else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"eqa") || !strcasecmp(rgr_lst[rgr_var_idx].val,"rgl") || !strcasecmp(rgr_lst[rgr_var_idx].val,"unf") || !strcasecmp(rgr_lst[rgr_var_idx].val,"uni")){
	rgr->lat_typ=nco_grd_lat_eqa;
	rgr->grd_typ=nco_grd_2D_eqa;
      }else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"gss")){
	rgr->lat_typ=nco_grd_lat_gss;
	rgr->grd_typ=nco_grd_2D_gss;
      }else{
	(void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
	abort();
      } /* !val */
      continue;
    } /* !lat_typ */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_typ")){
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"180_wst") || !strcasecmp(rgr_lst[rgr_var_idx].val,"wst_180"))
	rgr->lon_typ=nco_grd_lon_180_wst;
      else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"180_ctr") || !strcasecmp(rgr_lst[rgr_var_idx].val,"ctr_180"))
	rgr->lon_typ=nco_grd_lon_180_ctr;
      else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"Grn_wst") || !strcasecmp(rgr_lst[rgr_var_idx].val,"wst_Grn"))
	rgr->lon_typ=nco_grd_lon_Grn_wst;
      else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"Grn_ctr") || !strcasecmp(rgr_lst[rgr_var_idx].val,"ctr_Grn"))
	rgr->lon_typ=nco_grd_lon_Grn_ctr;
      else{
	(void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
	abort();
      } /* !val */
      continue;
    } /* !lon_typ */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"area_nm")){
      rgr->area_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !area_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"bnd_nm")){
      rgr->bnd_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !bnd_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"bnd_tm_nm")){
      rgr->bnd_tm_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !bnd_tm_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"col_nm_in") || !strcmp(rgr_lst[rgr_var_idx].key,"col_nm")){
      rgr->col_nm_in=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !col_nm_in */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"col_nm_out")){
      rgr->col_nm_out=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !col_nm_out */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"frc_nm")){
      rgr->frc_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !frc_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"ilev_nm_in") || !strcmp(rgr_lst[rgr_var_idx].key,"ilev_nm")){
      rgr->ilev_nm_in=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !ilev_nm_in */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"ilev_nm_out")){
      rgr->ilev_nm_out=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !ilev_nm_out */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_bnd_nm")){
      rgr->lat_bnd_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lat_bnd_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_dmn_nm") || !strcmp(rgr_lst[rgr_var_idx].key,"lat_dmn")){
      rgr->lat_dmn_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lat_dmn_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_nm_in") || !strcmp(rgr_lst[rgr_var_idx].key,"lat_nm")){
      rgr->lat_nm_in=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lat_nm_in */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_nm_out")){
      rgr->lat_nm_out=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lat_nm_out */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_vrt_nm")){
      rgr->lat_vrt_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lat_vrt_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lat_wgt_nm")){
      rgr->lat_wgt_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lat_wgt_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lev_nm_in") || !strcmp(rgr_lst[rgr_var_idx].key,"lev_nm")){
      rgr->lev_nm_in=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lev_nm_in */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lev_nm_out")){
      rgr->lev_nm_out=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lev_nm_out */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_bnd_nm")){
      rgr->lon_bnd_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lon_bnd_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_dmn_nm") || !strcmp(rgr_lst[rgr_var_idx].key,"lon_dmn")){
      rgr->lon_dmn_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lon_dmn_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_nm_in") || !strcmp(rgr_lst[rgr_var_idx].key,"lon_nm")){
      rgr->lon_nm_in=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lon_nm_in */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_nm_out")){
      rgr->lon_nm_out=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lon_nm_out */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"lon_vrt_nm")){
      rgr->lon_vrt_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !lon_vrt_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"plev_nm_in") || !strcmp(rgr_lst[rgr_var_idx].key,"plev_nm")){
      rgr->plev_nm_in=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !plev_nm_in */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"ply_tri")){
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"csz")){
	rgr->ply_tri_mth=nco_ply_tri_mth_csz;
      }else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"ctr") || !strcasecmp(rgr_lst[rgr_var_idx].val,"centroid") || !strcasecmp(rgr_lst[rgr_var_idx].val,"snl") || !strcasecmp(rgr_lst[rgr_var_idx].val,"mat")){
	rgr->ply_tri_mth=nco_ply_tri_mth_ctr;
      }else{
	(void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
	abort();
      } /* !val */
      continue;
    } /* !ply_tri */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"sgs_frc_nm")){
      rgr->sgs_frc_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !sgs_frc */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"sgs_msk_nm")){
      rgr->sgs_msk_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !sgs_msk */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"sgs_nrm")){
      rgr->sgs_nrm=strtod(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtod",sng_cnv_rcd);
      continue;
    } /* !sgs_nrm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"tst")){
      rgr->tst=strtol(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtol",sng_cnv_rcd);
      continue;
    } /* !tst */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"msk_nm") || !strcmp(rgr_lst[rgr_var_idx].key,"mask_nm")){
      rgr->msk_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      rgr->flg_msk_out=True;
      continue;
    } /* !msk_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"vrt_nm")){
      rgr->vrt_nm=(char *)strdup(rgr_lst[rgr_var_idx].val);
      continue;
    } /* !vrt_nm */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"vrt_ntp") || !strcmp(rgr_lst[rgr_var_idx].key,"ntp_mth")){
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"lin") || !strcasecmp(rgr_lst[rgr_var_idx].val,"linear") || !strcasecmp(rgr_lst[rgr_var_idx].val,"lnr")){
	rgr->ntp_mth=nco_ntp_lnr;
      }else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"log") || !strcasecmp(rgr_lst[rgr_var_idx].val,"logarithmic") || !strcasecmp(rgr_lst[rgr_var_idx].val,"lgr")){
	rgr->ntp_mth=nco_ntp_log;
      }else{
	(void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
	abort();
      } /* !val */
      continue;
    } /* !ntp_mth */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"xtr_mth")){
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"nrs_ngh") || !strcasecmp(rgr_lst[rgr_var_idx].val,"ngh") || !strcasecmp(rgr_lst[rgr_var_idx].val,"nearest_neighbor") || !strcasecmp(rgr_lst[rgr_var_idx].val,"nn")){
	rgr->xtr_mth=nco_xtr_fll_ngh;
      }else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"mss_val") || !strcasecmp(rgr_lst[rgr_var_idx].val,"msv") || !strcasecmp(rgr_lst[rgr_var_idx].val,"fll_val") || !strcasecmp(rgr_lst[rgr_var_idx].val,"missing_value")){
	rgr->xtr_mth=nco_xtr_fll_msv;
      }else{
	(void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
	abort();
      } /* !val */
      continue;
    } /* !xtr_mth */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"xtr_nsp") || !strcmp(rgr_lst[rgr_var_idx].key,"xtr_nbr_src_pnt") || !strcmp(rgr_lst[rgr_var_idx].key,"number_source_points") || !strcmp(rgr_lst[rgr_var_idx].key,"extrapolation_number_source_points")){
      rgr->xtr_nsp=strtol(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd,NCO_SNG_CNV_BASE10);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtol",sng_cnv_rcd);
      continue;
    } /* !xtr_nsp */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"xtr_xpn") || !strcmp(rgr_lst[rgr_var_idx].key,"extrapolation_exponent") || !strcmp(rgr_lst[rgr_var_idx].key,"exponent_of_distance_in_extrapolation")){
      rgr->xtr_xpn=strtod(rgr_lst[rgr_var_idx].val,&sng_cnv_rcd);
      if(*sng_cnv_rcd) nco_sng_cnv_err(rgr_lst[rgr_var_idx].val,"strtod",sng_cnv_rcd);
      continue;
    } /* !xtr_xpn */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"wgt_typ") || !strcmp(rgr_lst[rgr_var_idx].key,"weight_type")){
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"con") || !strcasecmp(rgr_lst[rgr_var_idx].val,"nco_con") || !strcasecmp(rgr_lst[rgr_var_idx].val,"conservative") || !strcasecmp(rgr_lst[rgr_var_idx].val,"wgt_con"))
        rgr->wgt_typ=nco_wgt_con;
      else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"dwe") || !strcasecmp(rgr_lst[rgr_var_idx].val,"nco_dwe") || !strcasecmp(rgr_lst[rgr_var_idx].val,"distance_weighted") || !strcasecmp(rgr_lst[rgr_var_idx].val,"wgt_dwe"))
        rgr->wgt_typ=nco_wgt_dwe;
      else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"bln") || !strcasecmp(rgr_lst[rgr_var_idx].val,"nco_bln") || !strcasecmp(rgr_lst[rgr_var_idx].val,"bilinear") || !strcasecmp(rgr_lst[rgr_var_idx].val,"wgt_bln"))
        rgr->wgt_typ=nco_wgt_bln;
      else {
        (void)fprintf(stderr,"%s: ERROR %s unable to parse \"%s\" option value \"%s\" (possible typo in value?), aborting...\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key,rgr_lst[rgr_var_idx].val);
        abort();
      } /* !val */
      continue;
    } /* !wgt_typ */
    (void)fprintf(stderr,"%s: ERROR %s reports unrecognized key-value option to --rgr switch: %s\n",nco_prg_nm_get(),fnc_nm,rgr_lst[rgr_var_idx].key);
    nco_exit(EXIT_FAILURE);
  } /* end for */

  /* Eliminate sticky wickets: Give nfr precedence over grd */
  if(rgr->flg_nfr && rgr->flg_grd) rgr->flg_grd=False; 

  /* Revert to defaults for any names not specified on command-line */
  if(!rgr->area_nm) rgr->area_nm=(char *)strdup("area"); /* [sng] Name of variable containing gridcell area */
  if(!rgr->bnd_nm) rgr->bnd_nm=(char *)strdup("nvertices"); /* [sng] Name of dimension to employ for spatial bounds */
  /* NB: CESM uses nbnd and ilev for temporal and vertical bounds, respectively (CESM outputs no horizontal spatial bounds). NCO defaults to nbnd for all bounds with two endpoints. */
  if(!rgr->bnd_tm_nm) rgr->bnd_tm_nm=(char *)strdup("nbnd"); /* [sng] Name of dimension to employ for temporal bounds */
  if(!rgr->col_nm_in) rgr->col_nm_in=(char *)strdup("ncol"); /* [sng] Name to recognize as input horizontal spatial dimension on unstructured grid */
  if(!rgr->frc_nm) rgr->frc_nm=(char *)strdup("frac_b"); /* [sng] Name of variable containing gridcell fraction */
  if(!rgr->ilev_nm_in) rgr->ilev_nm_in=(char *)strdup("ilev"); /* [sng] Name of input dimension to recognize as vertical dimension at layer interfaces */
  if(!rgr->lat_bnd_nm) rgr->lat_bnd_nm=(char *)strdup("lat_bnds"); /* [sng] Name of rectangular boundary variable for latitude */
  if(!rgr->lat_nm_in) rgr->lat_nm_in=(char *)strdup("lat"); /* [sng] Name of input dimension to recognize as latitude */
  if(!rgr->lev_nm_in) rgr->lev_nm_in=(char *)strdup("lev"); /* [sng] Name of input dimension to recognize as vertical dimension at layer midpoints */
  if(!rgr->lat_vrt_nm) rgr->lat_vrt_nm=(char *)strdup("lat_vertices"); /* [sng] Name of non-rectangular boundary variable for latitude */
  if(!rgr->lat_wgt_nm) rgr->lat_wgt_nm=(char *)strdup("gw"); /* [sng] Name of variable containing latitude weights */
  if(!rgr->lon_bnd_nm) rgr->lon_bnd_nm=(char *)strdup("lon_bnds"); /* [sng] Name of rectangular boundary variable for longitude */
  if(!rgr->lon_nm_in) rgr->lon_nm_in=(char *)strdup("lon"); /* [sng] Name of dimension to recognize as longitude */
  if(!rgr->lon_vrt_nm) rgr->lon_vrt_nm=(char *)strdup("lon_vertices"); /* [sng] Name of non-rectangular boundary variable for longitude */
  if(!rgr->msk_nm) rgr->msk_nm=(char *)strdup("mask"); /* [sng] Name of variable containing destination mask */
  if(!rgr->vrt_nm) rgr->vrt_nm=(char *)strdup("nv"); /* [sng] Name of dimension to employ for vertices */
  if(!rgr->plev_nm_in) rgr->plev_nm_in=(char *)strdup("plev"); /* [sng] Name of variable to recognize as pure pressure coordinate */

  /* Derived from defaults and command-line arguments */
  // On second thought, do not strdup() these here. This way, NULL means user never specified lon/lat-out names
  //  if(!rgr->col_nm_out) rgr->col_nm_out=(char *)strdup("ncol"); /* [sng] Name of dimension to output as horizontal spatial dimension on unstructured grid */
  //  if(!rgr->lat_nm_out) rgr->lat_nm_out=(char *)strdup("lat"); /* [sng] Name of dimension to output as latitude */
  //  if(!rgr->lon_nm_out) rgr->lon_nm_out=(char *)strdup("lon"); /* [sng] Name of dimension to output as longitude */
  //  if(!rgr->lat_nm_out) rgr->lat_nm_out=(char *)strdup(rgr_lat_nm_in); /* [sng] Name of output dimension for latitude */
  //  if(!rgr->lon_nm_out) rgr->lon_nm_out=(char *)strdup(rgr_lon_nm_in); /* [sng] Name of output dimension for longitude */

  /* Free kvms */
  if(rgr_lst) rgr_lst=nco_kvm_lst_free(rgr_lst,rgr_var_nbr);

  return rgr;
} /* end nco_rgr_ini() */
  
int /* O [enm] Return code */
nco_ntp_vrt /* [fnc] Interpolate vertically */
(rgr_sct * const rgr, /* I/O [sct] Regridding structure */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal Table */
{
  /* Purpose: Interpolate fields to new vertical grid specified in a vertical file */

  const char fnc_nm[]="nco_ntp_vrt()"; /* [sng] Function name */

  char *fl_tpl; /* [sng] Template file (vertical grid file) */
  char *fl_pth_lcl=NULL;

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int in_id; /* I [id] Input netCDF file ID */
  int tpl_id; /* [id] Input netCDF file ID (for vertical grid template) */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;

  int dmn_idx; /* [idx] Dimension index */
  int rec_idx; /* [idx] Record dimension index */

  nco_bool FL_RTR_RMT_LCN;
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_OPEN=rgr->flg_uio; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s obtaining vertical grid from %s\n",nco_prg_nm_get(),fnc_nm,rgr->fl_vrt);

  /* Duplicate (because nco_fl_mk_lcl() free()'s its fl_in) */
  fl_tpl=(char *)strdup(rgr->fl_vrt);
  /* Make sure file is on local system and is readable or die trying */
  fl_tpl=nco_fl_mk_lcl(fl_tpl,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  rcd+=nco_fl_open(fl_tpl,md_open,&bfr_sz_hnt,&tpl_id);

  /* Formula-terms for hybrid pressure vertical grid on unstructured CAM/EAM horizontal grid:
     prs_mdp[time,lev,col]=P0*hyam[lev] +PS[time,col]*hybm[lev]
     prs_ntf[time,lev,col]=P0*hyai[ilev]+PS[time,col]*hybi[ilev] */

  /* Formula-terms for hybrid pressure vertical grid on ECMWF RLL horizontal grid:
     prs_mdp[time,lev,lat,lon]=hyam[lev] +exp(lnsp[time,lat,lon])*hybm[lev]
     prs_ntf[time,lev,lat,lon]=hyai[ilev]+exp(lnsp[time,lat,lon])*hybi[ilev] */

  /* For simplicity and code re-use, all single-variable (not hybrid-variable) coordinate systems adopt "lev" semantics
     This includes pure pressure coordinates and eventually will include sigma, depth, and height coordinates
     Only hybrid coordinates will refer to the "ilev" levels and indices
     All single coordinate systems will refer to "lev" levels and indices */

  int dpt_id; /* [id] Ocean depth ID */
  int hyai_id=NC_MIN_INT; /* [id] Hybrid A coefficient at layer interfaces ID */
  int hyam_id=NC_MIN_INT; /* [id] Hybrid A coefficient at layer midpoints ID */
  int hybi_id=NC_MIN_INT; /* [id] Hybrid B coefficient at layer interfaces ID */
  int hybm_id=NC_MIN_INT; /* [id] Hybrid B coefficient at layer midpoints ID */
  int ilev_id=NC_MIN_INT; /* [id] Interface pressure ID */
  int lev_id=NC_MIN_INT; /* [id] Midpoint pressure ID */
  int p0_id=NC_MIN_INT; /* [id] Reference pressure ID */
  int ps_id=NC_MIN_INT; /* [id] Surface pressure ID */
  int plev_id; /* [id] Air pressure ID */
  nco_bool flg_grd_hyb_cameam=False; /* [flg] Hybrid coordinate vertical grid uses CAM/EAM conventions */
  nco_bool flg_grd_hyb_ecmwf=False; /* [flg] Hybrid coordinate vertical grid uses ECMWF conventions */
  nco_bool flg_grd_in_dpt=False; /* [flg] Input depth coordinate vertical grid */
  nco_bool flg_grd_in_hyb=False; /* [flg] Input hybrid coordinate vertical grid */
  nco_bool flg_grd_in_prs=False; /* [flg] Input pressure coordinate vertical grid */
  nco_bool flg_grd_out_dpt=False; /* [flg] Output depth coordinate vertical grid */
  nco_bool flg_grd_out_hyb=False; /* [flg] Output hybrid coordinate vertical grid */
  nco_bool flg_grd_out_prs=False; /* [flg] Output pressure coordinate vertical grid */
  nco_bool flg_vrt_tm=False; /* [flg] Output depends on time-varying vertical grid */
  nco_grd_vrt_typ_enm nco_vrt_grd_in=nco_vrt_grd_nil; /* [enm] Vertical grid type for input grid */
  nco_grd_vrt_typ_enm nco_vrt_grd_out=nco_vrt_grd_nil; /* [enm] Vertical grid type for output grid */
  nco_ntp_typ_enm ntp_mth=rgr->ntp_mth; /* [enm] Interpolation method */
  nco_xtr_typ_enm xtr_mth=rgr->xtr_mth; /* [enm] Extrapolation method */

  /* Determine output grid type */
  if((rcd=nco_inq_varid_flg(tpl_id,"hyai",&hyai_id)) == NC_NOERR){
    nco_vrt_grd_out=nco_vrt_grd_hyb; /* EAM */
    flg_grd_out_hyb=True;
  }else if((rcd=nco_inq_varid_flg(tpl_id,"plev",&plev_id)) == NC_NOERR){
    nco_vrt_grd_out=nco_vrt_grd_prs; /* NCEP */
    flg_grd_out_prs=True;
  }else if((rcd=nco_inq_varid_flg(tpl_id,"depth",&dpt_id)) == NC_NOERR){
    nco_vrt_grd_out=nco_vrt_grd_dpt; /* MPAS */
    flg_grd_out_dpt=True;
  }else{ /* !hyai */
    (void)fprintf(stdout,"%s: ERROR %s Unable to locate hybrid-sigma/pressure or pure-pressure vertical grid coordinate information in vertical grid file\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stdout,"%s: HINT ensure vertical grid coordinate file contains a valid vertical grid coordinate\n",nco_prg_nm_get());
    return NCO_ERR;
  } /* !hyai */
    
  if(flg_grd_out_hyb){
    rcd=nco_inq_varid(tpl_id,"hyai",&hyai_id);
    rcd=nco_inq_varid(tpl_id,"hyam",&hyam_id);
    rcd=nco_inq_varid(tpl_id,"hybi",&hybi_id);
    rcd=nco_inq_varid(tpl_id,"hybm",&hybm_id);
    rcd=nco_inq_varid(tpl_id,"P0",&p0_id);
    rcd=nco_inq_varid_flg(tpl_id,"ilev",&ilev_id);
    rcd=nco_inq_varid_flg(tpl_id,"lev",&lev_id);
    rcd=nco_inq_varid_flg(tpl_id,"PS",&ps_id);
  } /* !flg_grd_out_hyb */
  
  if(flg_grd_out_prs){
    rcd=nco_inq_varid(tpl_id,"plev",&lev_id);
  } /* !flg_grd_out_prs */

  if(flg_grd_out_dpt){
    rcd=nco_inq_varid(tpl_id,"depth",&lev_id);
  } /* !flg_grd_out_dpt */

  const int hyai_id_tpl=hyai_id; /* [id] Hybrid A coefficient at layer interfaces ID */
  const int hyam_id_tpl=hyam_id; /* [id] Hybrid A coefficient at layer midpoints ID */
  const int hybi_id_tpl=hybi_id; /* [id] Hybrid B coefficient at layer interfaces ID */
  const int hybm_id_tpl=hybm_id; /* [id] Hybrid B coefficient at layer midpoints ID */
  const int p0_id_tpl=p0_id; /* [id] Reference pressure ID */
  const int ilev_id_tpl=ilev_id; /* [id] Interface pressure ID */
  const int lev_id_tpl=lev_id; /* [id] Midpoint pressure ID */
  const int ps_id_tpl=ps_id; /* [id] Surface pressure ID */

  char *ilev_nm_in=NULL; /* [sng] Interface level name */
  char *lev_nm_in;
  char *ilev_nm_out;
  char *lev_nm_out;
  char *plev_nm_in; /* [sng] Pure-pressure coordnate name */
  char dmn_nm[NC_MAX_NAME]; /* [sng] Dimension name */
  int *dmn_ids_in=NULL; /* [nbr] Input file dimension IDs */
  int *dmn_ids_out=NULL; /* [nbr] Output file dimension IDs */
  int *dmn_ids_rec=NULL; /* [id] Unlimited dimension IDs */
  int dmn_nbr_ps; /* [nbr] Number of dimensions in PS variable */
  int dmn_nbr_in; /* [nbr] Number of dimensions in input file */
  int dmn_nbr_out; /* [nbr] Number of dimensions in output file */
  int dmn_id_ilev_out=NC_MIN_INT; /* [id] Dimension ID for interface level in output file */
  int dmn_id_lev_out=NC_MIN_INT; /* [id] Dimension ID for midpoint level in output file */
  int dmn_id_ilev_in=NC_MIN_INT; /* [id] Dimension ID for interface level in file to be interpolated */
  int dmn_id_lev_in=NC_MIN_INT; /* [id] Dimension ID for midpoint level in file to be interpolated */
  int dmn_id_tm_in=NC_MIN_INT; /* [id] Dimension ID for time in file to be interpolated */
  int dmn_nbr_rec; /* [nbr] Number of unlimited dimensions */
  int dmn_idx_tm_in=NC_MIN_INT; /* [idx] Index of record coordinate in input hybrid coordinate PS field */
  long *dmn_cnt_in=NULL;
  long *dmn_cnt_out=NULL;
  long *dmn_srt=NULL;
  long ilev_nbr_in;
  long lev_nbr_in;
  long ilev_nbr_out;
  long lev_nbr_out;
  long tm_idx=0L; /* [idx] Current timestep */
  long tm_nbr=1L; /* [idx] Number of timesteps in vertical grid */
  long tm_nbr_in=1L; /* [nbr] Number of timesteps in input vertical grid definition */
  long tm_nbr_out=1L; /* [nbr] Number of timesetps in output vertical grid definition */
  size_t grd_idx; /* [idx] Gridcell index */
  size_t grd_sz_in=1L; /* [nbr] Number of elements in single layer of input grid */
  size_t grd_sz_out=1L; /* [nbr] Number of elements in single layer of output grid */
  size_t idx_fst; /* [idx] Index-offset to current surface pressure timeslice */

  if(flg_grd_out_hyb){
    /* Interrogate hyai/hyam to obtain ilev/lev dimensions */
    rcd=nco_inq_vardimid(tpl_id,hyai_id,&dmn_id_ilev_out);
    rcd=nco_inq_vardimid(tpl_id,hyam_id,&dmn_id_lev_out);
    rcd=nco_inq_dimlen(tpl_id,dmn_id_ilev_out,&ilev_nbr_out);
    rcd=nco_inq_dimlen(tpl_id,dmn_id_lev_out,&lev_nbr_out);
    rcd=nco_inq_dimname(tpl_id,dmn_id_ilev_out,dmn_nm);
    ilev_nm_out=strdup(dmn_nm);
    rcd=nco_inq_dimname(tpl_id,dmn_id_lev_out,dmn_nm);
    lev_nm_out=strdup(dmn_nm);
    
    /* Interrogate PS, if any, for horizontal dimensions */ 
    if(ps_id_tpl != NC_MIN_INT){
      rcd=nco_inq_varndims(tpl_id,ps_id,&dmn_nbr_ps);
      dmn_nbr_out=dmn_nbr_ps;
      dmn_ids_out=(int *)nco_malloc(dmn_nbr_out*sizeof(int));
      dmn_cnt_out=(long *)nco_malloc((dmn_nbr_out+1)*sizeof(long));
      dmn_srt=(long *)nco_malloc((dmn_nbr_out+1)*sizeof(long));
      rcd=nco_inq_vardimid(tpl_id,ps_id,dmn_ids_out);
      rcd=nco_inq_unlimdims(tpl_id,&dmn_nbr_rec,(int *)NULL);
      if(dmn_nbr_rec > 0){
	dmn_ids_rec=(int *)nco_malloc(dmn_nbr_rec*sizeof(int));
	rcd=nco_inq_unlimdims(tpl_id,&dmn_nbr_rec,dmn_ids_rec);
      } /* !dmn_nbr_rec */
      for(dmn_idx=0;dmn_idx<dmn_nbr_out;dmn_idx++){
	rcd=nco_inq_dimlen(tpl_id,dmn_ids_out[dmn_idx],dmn_cnt_out+dmn_idx);
	/* 20190330: Allow possibility that PS has time dimension > 1 
	   We want horizontal not temporal dimensions to contribute to grd_sz 
	   Temporal dimension is usually unlimited 
	   Only multiply grd_sz by fixed (non-unlimited) dimension sizes
	   Corner-case exception when PS spatial dimension on unstructured grid is unlimited */
	for(rec_idx=0;rec_idx<dmn_nbr_rec;rec_idx++)
	  if(dmn_ids_out[dmn_idx] == dmn_ids_rec[rec_idx])
	    break; 
	if(rec_idx == dmn_nbr_rec || dmn_nbr_out == 1) grd_sz_out*=dmn_cnt_out[dmn_idx];
	if(rec_idx != dmn_nbr_rec && dmn_nbr_out > 1 && dmn_cnt_out[dmn_idx] > 1L){
	  tm_nbr_out=dmn_cnt_out[dmn_idx];
	  if(tm_nbr_out > 1L) flg_vrt_tm=True;
	} /* tm_nbr_out > 1 */
	dmn_srt[dmn_idx]=0L;
      } /* !dmn_idx */
      if(dmn_ids_rec) dmn_ids_rec=(int *)nco_free(dmn_ids_rec);
    } /* !ps_id_tpl */
  } /* !flg_grd_out_hyb */
  
  if(flg_grd_out_prs){
    /* Interrogate plev to obtain plev dimensions */
    rcd=nco_inq_vardimid(tpl_id,lev_id,&dmn_id_lev_out);
    rcd=nco_inq_dimlen(tpl_id,dmn_id_lev_out,&lev_nbr_out);
    rcd=nco_inq_dimname(tpl_id,dmn_id_lev_out,dmn_nm);
    ilev_nbr_out=lev_nbr_out;
  } /* !flg_grd_out_prs */

  double *hyai_out=NULL; /* [frc] Hybrid A coefficient at layer interfaces on output grid */
  double *hyam_out=NULL; /* [frc] Hybrid A coefficient at layer midpoints on output grid */
  double *hybi_out=NULL; /* [frc] Hybrid B coefficient at layer interfaces on output grid */
  double *hybm_out=NULL; /* [frc] Hybrid B coefficient at layer midpoints on output grid */
  double *ilev_out=NULL; /* [hPa] Interface pressure on output grid */
  double *lev_out=NULL; /* [hPa] Midpoint pressure on output grid */
  double *ps_out=NULL; /* [Pa] Surface pressure on output grid */
  double *prs_mdp_out=NULL; /* [Pa] Midpoint pressure on output grid */
  double *prs_ntf_out=NULL; /* [Pa] Interface pressure on output grid */
  double p0_out; /* [Pa] Reference pressure on output grid */
  long ilev_idx; /* [idx] Interface level index */
  long lev_idx; /* [idx] Level index */

  const nc_type crd_typ_out=NC_DOUBLE;
  nc_type var_typ_rgr; /* [enm] Variable type used during regridding */
  var_typ_rgr=NC_DOUBLE; /* NB: Perform interpolation in double precision */

  if(flg_grd_out_hyb){
    hyai_out=(double *)nco_malloc(ilev_nbr_out*nco_typ_lng(var_typ_rgr));
    hyam_out=(double *)nco_malloc(lev_nbr_out*nco_typ_lng(var_typ_rgr));
    hybi_out=(double *)nco_malloc(ilev_nbr_out*nco_typ_lng(var_typ_rgr));
    hybm_out=(double *)nco_malloc(lev_nbr_out*nco_typ_lng(var_typ_rgr));
    ilev_out=(double *)nco_malloc(ilev_nbr_out*nco_typ_lng(var_typ_rgr));
    lev_out=(double *)nco_malloc(lev_nbr_out*nco_typ_lng(var_typ_rgr));
    
    rcd=nco_get_var(tpl_id,hyai_id,hyai_out,crd_typ_out);
    rcd=nco_get_var(tpl_id,hyam_id,hyam_out,crd_typ_out);
    rcd=nco_get_var(tpl_id,hybi_id,hybi_out,crd_typ_out);
    rcd=nco_get_var(tpl_id,hybm_id,hybm_out,crd_typ_out);
    rcd=nco_get_var(tpl_id,p0_id,&p0_out,crd_typ_out);
    if(ilev_id_tpl != NC_MIN_INT){
      rcd=nco_get_var(tpl_id,ilev_id,ilev_out,crd_typ_out);
    }else{
      /* p0 is in Pa but ilev traditionally given in hPa */
      for(ilev_idx=0;ilev_idx<ilev_nbr_out;ilev_idx++) ilev_out[ilev_idx]=p0_out*(hyai_out[ilev_idx]+hybi_out[ilev_idx])/100.0;
    } /* !ilev_id_tpl */
    if(lev_id_tpl != NC_MIN_INT){
      rcd=nco_get_var(tpl_id,lev_id,lev_out,crd_typ_out);
    }else{
      /* p0 is in Pa but lev traditionally given in hPa */
      for(lev_idx=0;lev_idx<lev_nbr_out;lev_idx++) lev_out[lev_idx]=p0_out*(hyam_out[lev_idx]+hybm_out[lev_idx])/100.0;
    } /* !ilev_id_tpl */
  } /* !flg_grd_out_hyb */
  
  if(flg_grd_out_prs){
    lev_out=(double *)nco_malloc(lev_nbr_out*nco_typ_lng(var_typ_rgr));
    rcd=nco_get_var(tpl_id,lev_id,lev_out,crd_typ_out);
  } /* !flg_grd_out_prs */

  /* For vertical interpolation (unlike horizontal regridding), the destination grid is known a priori
     Straightforward copy all variables and attributes that define grid from fl_tpl to output
     would work in theory, but would not allow dynamic identification and relabeling of names */
  /* if(flg_grd_out_hyb){
     const int vrt_grd_lst_nbr=8;
     const char *vrt_grd_lst[]={"/hyai","/hyam","/hybi","/hybm","/ilev","/lev","/P0","/PS"};
     }
     if(flg_grd_out_prs){
     const int vrt_grd_lst_nbr=1;
     const char *vrt_grd_lst[]={"/plev"};
     }  */

  /* Above this line, fl_tpl and tpl_id refer to vertical coordinate file (i.e., template file)
     Below this line, fl_in and in_id refer to input file to be vertically regridded
     Do not close template file until all grid variables have been copied
     For maximum efficiency, do this after defining all interpolated variables in output 
     That way no file needs to exit define mode or enter data mode more than once
     However this requires keeping template file, input data file, and output file simulataneously open */
  in_id=rgr->in_id;
  out_id=rgr->out_id;

  /* Determine input grid type */
  if(rgr->plev_nm_in) plev_nm_in=rgr->plev_nm_in;
  if((rcd=nco_inq_varid_flg(in_id,"hyai",&hyai_id)) == NC_NOERR){
    nco_vrt_grd_in=nco_vrt_grd_hyb; /* EAM */
    flg_grd_in_hyb=True;
  }else if((rcd=nco_inq_varid_flg(in_id,plev_nm_in,&plev_id)) == NC_NOERR){
    nco_vrt_grd_in=nco_vrt_grd_prs; /* NCEP */
    flg_grd_in_prs=True;
  }else if((rcd=nco_inq_varid_flg(in_id,"depth",&dpt_id)) == NC_NOERR){
    nco_vrt_grd_in=nco_vrt_grd_dpt; /* NCEP */
    flg_grd_in_dpt=True;
  }else{ /* !hyai */
    (void)fprintf(stdout,"%s: ERROR %s Unable to locate hybrid-sigma/pressure or pure-pressure vertical grid coordinate information in input file\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stdout,"%s: HINT only invoke vertical interpolation on files that contain variables with vertical dimensions, and with known vertical coordinate variable names. These default to \"hyai\" for hybrid, \"plev\" for pressure, \"depth\" for depth. See http://nco.sf.net/nco.html#lev_nm for options to change these names at run-time, e.g., \"--rgr plev_nm=vrt_nm\"\n",nco_prg_nm_get());
    return NCO_ERR;
  } /* !hyai */

  /* Sanity checks: One type of input and one type of output grid detected */
  assert(!(flg_grd_in_hyb && flg_grd_in_prs));
  assert(!(flg_grd_in_hyb && flg_grd_in_dpt));
  assert(!(flg_grd_in_prs && flg_grd_in_dpt));
  assert(flg_grd_in_hyb || flg_grd_in_prs || flg_grd_in_dpt);
  assert(!(flg_grd_out_hyb && flg_grd_out_prs));
  assert(!(flg_grd_out_hyb && flg_grd_out_dpt));
  assert(!(flg_grd_out_prs && flg_grd_out_dpt));
  assert(flg_grd_out_hyb || flg_grd_out_prs || flg_grd_out_dpt);
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: DEBUG Input grid flags : flg_grd_in_hyb = %d, flg_grd_in_prs = %d, flg_grd_in_dpt = %d\n",nco_prg_nm_get(),flg_grd_in_hyb,flg_grd_in_prs,flg_grd_in_dpt);
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: DEBUG Output grid flags: flg_grd_out_hyb = %d, flg_grd_out_prs = %d, flg_grd_out_dpt = %d\n",nco_prg_nm_get(),flg_grd_out_hyb,flg_grd_out_prs,flg_grd_out_dpt);

  /* 20191219: This block is not used, deprecate it? Or use once new coordinates like altitude, depth supported? */
  nco_vrt_ntp_typ_enm nco_vrt_ntp_typ=nco_ntp_nil; /* Vertical interpolation type */
  if(nco_vrt_grd_in == nco_vrt_grd_hyb && nco_vrt_grd_out == nco_vrt_grd_hyb) nco_vrt_ntp_typ=nco_ntp_hyb_to_hyb;
  if(nco_vrt_grd_in == nco_vrt_grd_hyb && nco_vrt_grd_out == nco_vrt_grd_prs) nco_vrt_ntp_typ=nco_ntp_hyb_to_prs;
  if(nco_vrt_grd_in == nco_vrt_grd_prs && nco_vrt_grd_out == nco_vrt_grd_hyb) nco_vrt_ntp_typ=nco_ntp_prs_to_hyb;
  if(nco_vrt_grd_in == nco_vrt_grd_prs && nco_vrt_grd_out == nco_vrt_grd_prs) nco_vrt_ntp_typ=nco_ntp_prs_to_prs;
  assert(nco_vrt_ntp_typ != nco_ntp_nil);

  /* Variables on input grid, i.e., on grid in data file to be interpolated */
  if(flg_grd_in_hyb){
    rcd=nco_inq_varid(in_id,"hyai",&hyai_id);
    rcd=nco_inq_varid(in_id,"hyam",&hyam_id);
    rcd=nco_inq_varid(in_id,"hybi",&hybi_id);
    rcd=nco_inq_varid(in_id,"hybm",&hybm_id);
    /* 20190602: ECMWF hybrid vertical grid parameters and dimensions differ from CAM/EAM:
       ECMWF defines vertical dimensions "nhym" and "nhyi" specifically for hy[ab][im] and uses "lev" and "lev_2" for all other variables, whereas CAM/EAM uses same dimensions "lev" and "ilev" for all vertical variables including hybrid coefficients
       ECMWF provides "hya?" as a constant in Pa and "hyb?" as a dimensionless coefficient of PS, whereas CAM/EAM provides "hya?" and "hyb?" both as dimensionless coefficients of P0 and PS
       ECMWF provides "lev" and "lev_2" with midpoint and surface pressure indices (not values), respectively, whereas CAM/EAM provides "lev" and "ilev" coordinate values in hPa
       ECMWF provides dimensionless "lnsp" for log(surface pressure) whereas CAM/EAM provides "PS" for surface pressure in Pa
       ECMWF "lnsp" has degenerate level dimension "lev_2" whereas CAM/EAM "PS" has no "ilev" dimension
       ECMWF uses hya? instead of reference pressure whereas CAM/EAM provides "P0" in hPa */
    if((rcd=nco_inq_varid_flg(in_id,"lnsp",&ps_id)) == NC_NOERR) flg_grd_hyb_ecmwf=True;
    else if((rcd=nco_inq_varid_flg(in_id,"PS",&ps_id)) == NC_NOERR) flg_grd_hyb_cameam=True;
    else{
      (void)fprintf(stderr,"%s: ERROR %s Unable to find surface pressure variable required for hybrid grid in input file\n",nco_prg_nm_get(),fnc_nm);
      abort();
    } /* !rcd */

    if(flg_grd_hyb_cameam){
      rcd=nco_inq_varid(in_id,"P0",&p0_id);
      ilev_id=NC_MIN_INT;
      lev_id=NC_MIN_INT;
      if(ilev_id_tpl == NC_MIN_INT) rcd=nco_inq_varid_flg(in_id,"ilev",&ilev_id);
      if(lev_id_tpl == NC_MIN_INT) rcd=nco_inq_varid_flg(in_id,"lev",&lev_id);
    } /* !flg_grd_hyb_cameam */
    /* 20190603: We require ECMWF IFS input to have a "lev" coordinate so we can use "lev" dimension not "nhyb" */
    if(flg_grd_hyb_ecmwf)
      rcd=nco_inq_varid(in_id,"lev",&lev_id);
  } /* !flg_grd_in_hyb */

  if(flg_grd_in_prs){
    rcd=nco_inq_varid(in_id,plev_nm_in,&lev_id);
    if((rcd=nco_inq_varid_flg(in_id,"PS",&ps_id)) == NC_NOERR){
      /* Output file creation procedure discriminates between input surface pressure dimensioned as CAM/EAM vs. ECMWF */
      flg_grd_hyb_cameam=True;
      if(flg_grd_out_hyb && (ps_id_tpl == NC_MIN_INT)) (void)fprintf(stderr,"%s: INFO %s detects variable PS (canonical name for spatially varying surface pressure field in hybrid grids) in pure-pressure input data file. PS will be copied directly from pure-pressure grid input dataset to, and used to construct the pressures of, the output hybrid-coordinate data file.\n",nco_prg_nm_get(),fnc_nm);
      if(flg_grd_out_hyb && (ps_id_tpl != NC_MIN_INT)) (void)fprintf(stderr,"%s: INFO %s detects variable PS (canonical name for spatially varying surface pressure field in hybrid grids) in both vertical-grid file and pure-pressure input data file. The vertical grid-file takes precedence. PS will be copied directly from vertical-grid file to, and used to construct the pressures of, the output hybrid-coordinate data file. PS in input pure-pressure file will be ignored.\n",nco_prg_nm_get(),fnc_nm);
    }else{
      if(flg_grd_out_hyb && (ps_id_tpl == NC_MIN_INT)){
	(void)fprintf(stderr,"%s: ERROR %s does not find variable PS (canonical name for spatially varying surface pressure field in hybrid grids) in pure-pressure input data file or in vertical grid-file for hybrid-pressure output. PS must be present in at least one of these files in order to construct the output hybrid-coordinate pressures.\nHINT: Append a valid PS to the inpud data file or vertical grid-file.\n",nco_prg_nm_get(),fnc_nm);
	nco_exit(EXIT_FAILURE);
      } /* !ps_id_tpl */
    } /* !ps_id */
  } /* !flg_grd_in_prs */

  if(flg_grd_in_dpt){
    rcd=nco_inq_varid(in_id,"depth",&lev_id);
  } /* !flg_grd_in_dpt */

  const int ilev_id_in=ilev_id; /* [id] Interface pressure ID */
  const int lev_id_in=lev_id; /* [id] Midpoint pressure ID */
  const int ps_id_in=ps_id; /* [id] Surface pressure ID */

  /* Identify all record-dimensions in input file */
  rcd=nco_inq_unlimdims(in_id,&dmn_nbr_rec,(int *)NULL);
  if(dmn_nbr_rec > 0){
    dmn_ids_rec=(int *)nco_malloc(dmn_nbr_rec*sizeof(int));
    rcd+=nco_inq_unlimdims(in_id,&dmn_nbr_rec,dmn_ids_rec);
  } /* !dmn_nbr_rec */

  if(flg_grd_in_hyb){
    /* Get hybrid vertical information first */
    rcd=nco_inq_varndims(in_id,ps_id,&dmn_nbr_in);
    rcd=nco_inq_vardimid(in_id,hyai_id,&dmn_id_ilev_in);
    if(flg_grd_hyb_cameam) rcd=nco_inq_vardimid(in_id,hyam_id,&dmn_id_lev_in);
    if(flg_grd_hyb_ecmwf) rcd=nco_inq_vardimid(in_id,lev_id,&dmn_id_lev_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_ilev_in,&ilev_nbr_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_lev_in,&lev_nbr_in);
    rcd=nco_inq_dimname(in_id,dmn_id_ilev_in,dmn_nm);
    ilev_nm_in=strdup(dmn_nm);
    rcd=nco_inq_dimname(in_id,dmn_id_lev_in,dmn_nm);
    lev_nm_in=strdup(dmn_nm);
  } /* !flg_grd_in_hyb */

  if(flg_grd_in_prs){
    /* Interrogate plev to obtain plev dimensions */
    rcd=nco_inq_vardimid(in_id,lev_id,&dmn_id_lev_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_lev_in,&lev_nbr_in);
    rcd=nco_inq_dimname(in_id,dmn_id_lev_in,dmn_nm);
    lev_nm_in=strdup(dmn_nm);

    /* Define horizontal grid if no PS is provided (i.e., pure-pressure to pure-pressure interpolation) */
    if(!flg_grd_out_hyb){
      /* Problem: What is horizontal grid size of pressure grid file?
	 Algorithm: 
	 Examine first multi-dimensional variable that includes plev dimension 
	 Assume horizontal dimensions vary more rapidly than (i.e., follow) plev
	 Compute horizontal grid size accordingly
	 Set output horizontal size to input horizontal size */
      int var_nbr; /* [nbr] Number of variables in file */
      int var_idx; /* [idx] Index over variables in file */
      rcd=nco_inq(in_id,&dmn_nbr_in,&var_nbr,(int *)NULL,(int *)NULL);
      dmn_ids_in=(int *)nco_malloc(dmn_nbr_in*sizeof(int));
      dmn_cnt_in=(long *)nco_malloc(dmn_nbr_in*sizeof(long));
      for(var_idx=0;var_idx<var_nbr;var_idx++){
	rcd=nco_inq_varndims(in_id,var_idx,&dmn_nbr_in);
	rcd=nco_inq_vardimid(in_id,var_idx,dmn_ids_in);
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++)
	  if(dmn_ids_in[dmn_idx] == dmn_id_lev_in)
	    break;
	/* Does current variable have lev dimension? */
	if(dmn_idx < dmn_nbr_in){
	  /* Yes. Do any dimensions vary more rapidly than lev? */
	  if(dmn_idx < dmn_nbr_in-1){
	    /* Yes. Assume remaining dimension are horizontal spatial dimensions */
	    char var_nm[NC_MAX_NAME+1L];
	    (void)nc_inq_varname(in_id,var_idx,var_nm);
	    for(int dmn_idx_hrz=dmn_idx+1;dmn_idx_hrz<dmn_nbr_in;dmn_idx_hrz++){
	      rcd=nco_inq_dimlen(in_id,dmn_ids_in[dmn_idx_hrz],dmn_cnt_in+dmn_idx_hrz);
	      grd_sz_in*=dmn_cnt_in[dmn_idx_hrz];
	    } /* !dmn_idx_hrz */
	    break;
	  } /* !dmn_idx */
	} /* !dmn_idx */
      } /* !var_idx */
      assert(var_idx != var_nbr);
      grd_sz_out=grd_sz_in;
    } /* !flg_grd_out_hyb */
  } /* !flg_grd_in_prs */

  double *hyai_in=NULL; /* [frc] Hybrid A coefficient at layer interfaces on input grid */
  double *hyam_in=NULL; /* [frc] Hybrid A coefficient at layer midpoints on input grid */
  double *hybi_in=NULL; /* [frc] Hybrid B coefficient at layer interfaces on input grid */
  double *hybm_in=NULL; /* [frc] Hybrid B coefficient at layer midpoints on input grid */
  double *lev_in=NULL; /* [Pa] Air pressure on input grid */
  double *prs_mdp_in=NULL; /* [Pa] Midpoint pressure on input grid */
  double *prs_ntf_in=NULL; /* [Pa] Interface pressure on input grid */
  double *ps_in=NULL; /* [Pa] Surface pressure on input grid */
  double p0_in; /* [Pa] Reference pressure on input grid */
  
  if(flg_grd_in_hyb){
    hyai_in=(double *)nco_malloc(ilev_nbr_in*nco_typ_lng(var_typ_rgr));
    hyam_in=(double *)nco_malloc(lev_nbr_in*nco_typ_lng(var_typ_rgr));
    hybi_in=(double *)nco_malloc(ilev_nbr_in*nco_typ_lng(var_typ_rgr));
    hybm_in=(double *)nco_malloc(lev_nbr_in*nco_typ_lng(var_typ_rgr));
    
    rcd=nco_get_var(in_id,hyai_id,hyai_in,crd_typ_out);
    rcd=nco_get_var(in_id,hyam_id,hyam_in,crd_typ_out);
    rcd=nco_get_var(in_id,hybi_id,hybi_in,crd_typ_out);
    rcd=nco_get_var(in_id,hybm_id,hybm_in,crd_typ_out);
    if(flg_grd_hyb_cameam) rcd=nco_get_var(in_id,p0_id,&p0_in,crd_typ_out);
    /* ECMWF distributes IFS forecasts with lnsp = log(surface pressure) */
    if(flg_grd_hyb_ecmwf){
      /* Decompose ECMWF hya? convention into CAM/EAM-like product of P0 and hya? */
      p0_in=100000.0;
      for(size_t idx=0;idx<lev_nbr_in;idx++){
	hyai_in[idx]/=p0_in;
	hyam_in[idx]/=p0_in;
      } /* !idx */
    } /* flg_grd_hyb_ecmwf */
    
  } /* !flg_grd_in_hyb */

  if(flg_grd_in_prs){
    lev_in=(double *)nco_malloc(lev_nbr_in*nco_typ_lng(var_typ_rgr));
    rcd=nco_get_var(in_id,lev_id,lev_in,crd_typ_out);
  } /* !flg_grd_in_prs */
  
  /* Always obtain surface pressure if input or output grid is hybrid */
  if(flg_grd_in_hyb || flg_grd_out_hyb){

    /* Copy horizontal grid information from input file
       LHS variables were set above if PS is in template file */
    if(ps_id_tpl == NC_MIN_INT){

      /* NB: dmn_nbr_in/out in this block refer only to horizontal dimensions necessary to define PS */
      rcd=nco_inq_varndims(in_id,ps_id,&dmn_nbr_in); /* This is harmlessly repeated for hybrid input files */
      dmn_ids_in=(int *)nco_malloc(dmn_nbr_in*sizeof(int));
      dmn_cnt_in=(long *)nco_malloc((dmn_nbr_in+1)*sizeof(long));
      if(!dmn_srt) dmn_srt=(long *)nco_malloc((dmn_nbr_in+1)*sizeof(long)); /* NB: Only allocate dmn_srt once  */

      rcd=nco_inq_vardimid(in_id,ps_id,dmn_ids_in);
      for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	rcd=nco_inq_dimlen(in_id,dmn_ids_in[dmn_idx],dmn_cnt_in+dmn_idx);
	/* 20190330: Allow possibility that PS has time dimension > 1 
	   We want horizontal not temporal dimensions to contribute to grd_sz 
	   Temporal dimension is usually unlimited 
	   Only multiply grd_sz by fixed (non-unlimited) dimension sizes
	   Corner-case exception when PS spatial dimension on unstructured grid is unlimited */
	for(rec_idx=0;rec_idx<dmn_nbr_rec;rec_idx++)
	  if(dmn_ids_in[dmn_idx] == dmn_ids_rec[rec_idx])
	    break; 
	if(rec_idx == dmn_nbr_rec || dmn_nbr_in == 1) grd_sz_in*=dmn_cnt_in[dmn_idx];
	if(rec_idx != dmn_nbr_rec && dmn_nbr_in > 1 && dmn_cnt_in[dmn_idx] > 1L){
	  dmn_id_tm_in=dmn_ids_in[dmn_idx];
	  dmn_idx_tm_in=dmn_idx;
	  tm_nbr_in=dmn_cnt_in[dmn_idx_tm_in];
	  if(tm_nbr_in > 1L) flg_vrt_tm=True;
	} /* tm_nbr_in > 1 */
	dmn_srt[dmn_idx]=0L;
      } /* !dmn_idx */

      /* Given all input PS information, define output PS information */
      dmn_nbr_ps=dmn_nbr_out=dmn_nbr_in;
      dmn_ids_out=(int *)nco_malloc(dmn_nbr_out*sizeof(int));
      dmn_cnt_out=(long *)nco_malloc((dmn_nbr_out+1)*sizeof(long));
      /* fxm: next line works for hyb_in and is buggy for prs_in */
      memcpy(dmn_ids_out,dmn_ids_in,dmn_nbr_in*sizeof(int));
      memcpy(dmn_cnt_out,dmn_cnt_in,dmn_nbr_in*sizeof(long));
      grd_sz_out=grd_sz_in;
      tm_nbr_out=tm_nbr_in;
    }else{ /* !ps_id_tpl */
      /* 20200825: 
	 We have already defined grd_sz_out if PS is in template file
	 We have already defined grd_sz_in and grd_sz_out := grd_sz_in when PS not in template file
	 We have already defined grd_sz_in if input file is pure-pressure
	 However, we have not yet defined grd_sz_in if input file is hybrid
	 Expectation is that grd_sz_in (from input file) = grd_sz_out (from template file)
	 An independent check on this would examine dimension sizes in input file
	 Such a check would immediately flag horizontal mismatches between vertical file and input file
	 The check could not rely on PS being present in input file
	 The check could/should examine the first horizontal variable in input file
	 This would require a lot of code, so we just assume it is true */
      grd_sz_in=grd_sz_out;
    } /* !ps_id_tpl */

    /* Timestep sequencing
       NB: tm_nbr_??? variables count timesteps in vertical grid definitions
       These are not necessarily the same as the number of timesteps in either file
       Time-invariant hybrid or pure-pressure coordinates are valid vertical grids for timeseries
       Usually hybrid grids have as many timesteps in the grids as in the timeseries 
       Usually pressure grids are time-invariant (as of 20190511 time-varying pure pressure grids are still not supported)
       This implementation interpolates timeseries to/from time-invariant vertical grids in one OpenMP call! */
    if(tm_nbr_in > 1L || tm_nbr_out > 1L){
      if(tm_nbr_in > tm_nbr_out) assert((float)tm_nbr_in/(float)tm_nbr_out == tm_nbr_in/tm_nbr_out); else assert((float)tm_nbr_out/(float)tm_nbr_in == tm_nbr_out/tm_nbr_in);
    } /* !tm_nbr_in */
    tm_nbr=tm_nbr_in > tm_nbr_out ? tm_nbr_in : tm_nbr_out;

    /* Sanity checks */
    if(grd_sz_in != grd_sz_out || tm_nbr_in != tm_nbr_out) (void)fprintf(stdout,"%s: ERROR %s reports that temporal or horizontal spatial dimensions differ: grd_sz_in = %ld != %ld = grd_sz_out, and/or tm_nbr_in = %ld != %ld = tm_nbr_out\n",nco_prg_nm_get(),fnc_nm,grd_sz_in,grd_sz_out,tm_nbr_in,tm_nbr_out);
    assert(grd_sz_in == grd_sz_out);
    assert(tm_nbr_in == tm_nbr_out);

    ps_in=(double *)nco_malloc_dbg(tm_nbr_in*grd_sz_in*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() ps_in value buffer");

    /* Surface pressure comes from either hybrid vertical grid-files, hybrid data files, or pressure data files that provide surface pressure */
    if(flg_grd_in_hyb || (flg_grd_in_prs && ps_id_tpl == NC_MIN_INT)) rcd=nco_get_var(in_id,ps_id,ps_in,crd_typ_out);

    /* ECMWF distributes IFS forecasts with lnsp = log(surface pressure) */
    if(flg_grd_hyb_ecmwf){
      /* Convert ECMWF-provided log(surface_pressure) to surface_pressure */
      const size_t ps_sz_in=tm_nbr_in*grd_sz_in; /* [nbr] Number of elements in ps_in */
      for(size_t idx=0;idx<ps_sz_in;idx++) ps_in[idx]=exp(ps_in[idx]); 
    } /* flg_grd_hyb_ecmwf */

    /* Finally have enough information to allocate output pressure grid */
    ps_out=(double *)nco_malloc_dbg(tm_nbr_out*grd_sz_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() ps_out value buffer");
    
    /* Get PS from output horizontal grid, if available, otherwise copy from input horizontal grid */
    if(ps_id_tpl != NC_MIN_INT){
      rcd=nco_get_var(tpl_id,ps_id_tpl,ps_out,crd_typ_out); /* NB: Here we read from tpl_id one last time */
    }else{
      memcpy(ps_out,ps_in,tm_nbr_in*grd_sz_in*nco_typ_lng(var_typ_rgr));
    } /* !ps_id_tpl */
  } /* ! */

  /* Compare input and output surface pressure fields to determine whether subterranean extrapolation required */
  nco_bool flg_add_msv_att; /* [flg] Extrapolation requires _FillValue */
  flg_add_msv_att=False;
  /* Extrapolation type xtr_fll_msv may cause need to create _FillValue attributes */
  if(xtr_mth == nco_xtr_fll_msv){
    const size_t ps_sz=tm_nbr*grd_sz_in; // [nbr] Size of surface-pressure field
    double *prs_max_in=NULL; /* [Pa] Maximum midpoint pressure on input grid */
    double *prs_max_out=NULL; /* [Pa] Maximum midpoint pressure on output grid */
    double *prs_min_in=NULL; /* [Pa] Minimum midpoint pressure on input grid */
    double *prs_min_out=NULL; /* [Pa] Minimum midpoint pressure on output grid */
    long idx_lev_max; // [idx] Index of midpoint level with greatest pressure
    long idx_lev_min; // [idx] Index of midpoint level with lowest pressure
    size_t idx; // [idx] Counting index
    prs_max_in=(double *)nco_malloc_dbg(ps_sz*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() prs_max_in value buffer");
    prs_max_out=(double *)nco_malloc_dbg(ps_sz*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() prs_max_out value buffer");
    prs_min_in=(double *)nco_malloc_dbg(ps_sz*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() prs_min_in value buffer");
    prs_min_out=(double *)nco_malloc_dbg(ps_sz*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() prs_min_out value buffer");
    if(flg_grd_in_hyb){
      // fxm: assumes hybrid grid has least/greatest pressure at top/bottom level
      idx_lev_max=lev_nbr_in-1;
      idx_lev_min=0L;
      for(tm_idx=0;tm_idx<tm_nbr;tm_idx++){
	idx_fst=tm_idx*grd_sz_in;
	for(grd_idx=0;grd_idx<grd_sz_in;grd_idx++){
	  prs_max_in[grd_idx+idx_fst]=p0_in*hyam_in[idx_lev_max]+ps_in[idx_fst+grd_idx]*hybm_in[idx_lev_max];
	  prs_min_in[grd_idx+idx_fst]=p0_in*hyam_in[idx_lev_min]+ps_in[idx_fst+grd_idx]*hybm_in[idx_lev_min];
	} /* !grd_idx */
      } /* !tm_idx */
    } /* !flg_grd_in_hyb */
    if(flg_grd_out_hyb){
      // fxm: assumes hybrid grid has least/greatest pressure at top/bottom level
      idx_lev_max=lev_nbr_out-1;
      idx_lev_min=0L;
      for(tm_idx=0;tm_idx<tm_nbr;tm_idx++){
	idx_fst=tm_idx*grd_sz_out;
	for(grd_idx=0;grd_idx<grd_sz_out;grd_idx++){
	  prs_max_out[grd_idx+idx_fst]=p0_out*hyam_out[idx_lev_max]+ps_out[idx_fst+grd_idx]*hybm_out[idx_lev_max];
	  prs_min_out[grd_idx+idx_fst]=p0_out*hyam_out[idx_lev_min]+ps_out[idx_fst+grd_idx]*hybm_out[idx_lev_min];
	} /* !grd_idx */
      } /* !tm_idx */
    } /* !flg_grd_out_hyb */
    if(flg_grd_in_prs){
      double lev_in_max;
      double lev_in_min;
      if(lev_in[0] < lev_in[1]) lev_in_max=lev_in[lev_nbr_in-1]; else lev_in_max=lev_in[0];
      if(lev_in[0] < lev_in[1]) lev_in_min=lev_in[0]; else lev_in_max=lev_in[lev_nbr_in-1];
      for(size_t idx_in=0;idx_in<ps_sz;idx_in++) prs_max_in[idx_in]=lev_in_max;
      for(size_t idx_in=0;idx_in<ps_sz;idx_in++) prs_min_in[idx_in]=lev_in_min;
    } /* !flg_grd_in_prs */
    if(flg_grd_out_prs){
      double lev_out_max;
      double lev_out_min;
      if(lev_out[0] < lev_out[1]) lev_out_max=lev_out[lev_nbr_out-1]; else lev_out_max=lev_out[0];
      if(lev_out[0] < lev_out[1]) lev_out_min=lev_out[0]; else lev_out_min=lev_out[lev_nbr_out-1];
      for(size_t idx_out=0;idx_out<ps_sz;idx_out++) prs_max_out[idx_out]=lev_out_max;
      for(size_t idx_out=0;idx_out<ps_sz;idx_out++) prs_min_out[idx_out]=lev_out_min;
    } /* !flg_grd_out_prs */
    for(idx=0;idx<ps_sz;idx++)
      if(prs_max_out[idx] > prs_max_in[idx]) break;
    if(idx < ps_sz) flg_add_msv_att=True;
    for(idx=0;idx<ps_sz;idx++)
      if(prs_min_out[idx] < prs_min_in[idx]) break;
    if(idx < ps_sz) flg_add_msv_att=True;
    if(flg_add_msv_att && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports at least one point in at least one output level requires extrapolation (not interpolation). Will ensure that all interpolated fields have _FillValue attribute.\n",nco_prg_nm_get(),fnc_nm);
    if(prs_max_in) prs_max_in=(double *)nco_free(prs_max_in);
    if(prs_max_out) prs_max_out=(double *)nco_free(prs_max_out);
    if(prs_min_in) prs_min_in=(double *)nco_free(prs_min_in);
    if(prs_min_out) prs_min_out=(double *)nco_free(prs_min_out);
  } /* !xtr_mth */
  
  /* Lay-out regridded file */

  //(void)fprintf(stdout,"%s: DEBUG quark1 dmn_nbr_out = %d, dmn_nbr_ps = %d\n",nco_prg_nm_get(),dmn_nbr_out,dmn_nbr_ps);

  /* Use explicitly specified output names, if any, otherwise use template names (either explicitly specified or discovered by fuzzing) */
  if(rgr->lev_nm_out) lev_nm_out=rgr->lev_nm_out;
  if(rgr->ilev_nm_out){
    if(flg_grd_out_hyb) ilev_nm_out=rgr->ilev_nm_out;
    if(flg_grd_out_prs) lev_nm_out=rgr->ilev_nm_out;
  } /* !ilev_nm_out */
  if(flg_grd_out_prs){
    /* Unless user explicitly specifies output name, use same name as input */
    if(!rgr->lev_nm_out) lev_nm_out=(char *)strdup(plev_nm_in);
    /* Hybrid-sigma/pressure interface variables, if any, must also be output to pure-pressure files on lev grid */
    ilev_nm_out=(char *)strdup(lev_nm_out);
  } /* !flg_grd_out_prs */

  /* Define new vertical dimensions before all else */
  if(flg_grd_out_hyb){
    rcd=nco_def_dim(out_id,ilev_nm_out,ilev_nbr_out,&dmn_id_ilev_out);
    rcd=nco_def_dim(out_id,lev_nm_out,lev_nbr_out,&dmn_id_lev_out);
    /* Horizontal dimensions necessary to define PS variable */
    for(dmn_idx=0;dmn_idx<dmn_nbr_out;dmn_idx++){
      if(ps_id_tpl != NC_MIN_INT){
	rcd=nco_inq_dimname(tpl_id,dmn_ids_out[dmn_idx],dmn_nm);
      }else{
	rcd=nco_inq_dimname(in_id,dmn_ids_in[dmn_idx],dmn_nm);
	rcd=nco_inq_dimlen(in_id,dmn_ids_in[dmn_idx],dmn_cnt_out+dmn_idx);
      } /* !ps_id_tpl */
      if(flg_grd_hyb_cameam) rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt_out[dmn_idx],dmn_ids_out+dmn_idx);
      /* 20190602: ECMWF IFS PS variable has degenerate vertical dimension (lev_2). Avoid re-definition */
      if(flg_grd_hyb_ecmwf)
	if(strcmp(dmn_nm,ilev_nm_out))
	  if(strcmp(dmn_nm,lev_nm_out))
	    rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt_out[dmn_idx],dmn_ids_out+dmn_idx);
    } /* !dmn_idx */
  } /* !flg_grd_out_hyb */

  if(flg_grd_out_prs){
    rcd=nco_def_dim(out_id,lev_nm_out,lev_nbr_out,&dmn_id_lev_out);
  } /* !flg_grd_out_prs */
  
  /* Do not extract grid variables (that are also extensive variables) like ilev, lev, hyai, hyam, hybi, hybm */ 
  /* Exception list source:
     CAM: hyai, hyam, hybi, hybm, ilev, lev, P0, PS
     EAM: hyai, hyam, hybi, hybm, ilev, lev, P0, PS
     ECMWF: hyai, hyam, hybi, hybm, lev, lnsp
     NCEP: plev */
  const int var_xcl_lst_nbr=10; /* [nbr] Number of objects on exclusion list */
  const char *var_xcl_lst[]={"/hyai","/hyam","/hybi","/hybm","/ilev","/lev","/P0","/plev","/PS","/lnsp"};
  int var_cpy_nbr=0; /* [nbr] Number of copied variables */
  int var_rgr_nbr=0; /* [nbr] Number of regridded variables */
  int var_xcl_nbr=0; /* [nbr] Number of deleted variables */
  int var_crt_nbr=0; /* [nbr] Number of created variables */
  long idx; /* [idx] Generic index */
  unsigned int idx_tbl; /* [idx] Counter for traversal table */
  const unsigned int trv_nbr=trv_tbl->nbr; /* [idx] Number of traversal table entries */
  for(idx=0;idx<var_xcl_lst_nbr;idx++){
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++)
      if(!strcmp(trv_tbl->lst[idx_tbl].nm_fll,var_xcl_lst[idx])) break;
    if(idx_tbl < trv_nbr){
      if(trv_tbl->lst[idx_tbl].flg_xtr){
	if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO automatically omitting (not copying or regridding from input) pre-defined exclusion-list variable %s\n",nco_prg_nm_get(),trv_tbl->lst[idx_tbl].nm_fll);
	var_xcl_nbr++;
      } /* endif */
      trv_tbl->lst[idx_tbl].flg_xtr=False;
    } /* !idx_tbl */
  } /* !idx */
  /* 20191001: Do not automatically define plev_nm_in in pressure-grid output files 
     The variable named lev_nm_out in the input data file is always defined in the output file
     So if plev_nm_in == lev_nm_out it will be defined anyway */
  if(flg_grd_in_prs && flg_grd_out_prs && strcmp(plev_nm_in,lev_nm_out)){
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++)
      if(!strcmp(trv_tbl->lst[idx_tbl].nm,plev_nm_in)) break;
    if(idx_tbl < trv_nbr){
      if(trv_tbl->lst[idx_tbl].flg_xtr){
	if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO automatically omitting (not copying or regridding from input) pre-defined exclusion-list variable %s\n",nco_prg_nm_get(),trv_tbl->lst[idx_tbl].nm_fll);
	var_xcl_nbr++;
      } /* endif */
      trv_tbl->lst[idx_tbl].flg_xtr=False;
    } /* !idx_tbl */
  } /* !idx */

  char *var_nm; /* [sng] Variable name */
  int *dmn_id_in=NULL; /* [id] Dimension IDs */
  int *dmn_id_out=NULL; /* [id] Dimension IDs */
  int var_id_in; /* [id] Variable ID */
  int var_id_out; /* [id] Variable ID */
  nc_type var_typ_out; /* [enm] Variable type to write to disk */
  nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */

  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */
  deflate=(int)True;
  shuffle=NC_SHUFFLE;
  dfl_lvl=rgr->dfl_lvl;
  fl_out_fmt=rgr->fl_out_fmt;

  /* Define new coordinates and grid variables in regridded file */
  const int dmn_nbr_0D=0; /* [nbr] Rank of 0-D grid variables (scalars) */
  const int dmn_nbr_1D=1; /* [nbr] Rank of 1-D grid variables */
  //const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  //const int dmn_nbr_3D=3; /* [nbr] Rank of 3-D grid variables */
  //const int dmn_nbr_grd_max=dmn_nbr_3D; /* [nbr] Maximum rank of grid variables */
  
  if(flg_grd_out_hyb){
    rcd+=nco_def_var(out_id,"hyai",crd_typ_out,dmn_nbr_1D,&dmn_id_ilev_out,&hyai_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,hyai_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,"hyam",crd_typ_out,dmn_nbr_1D,&dmn_id_lev_out,&hyam_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,hyam_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,"hybi",crd_typ_out,dmn_nbr_1D,&dmn_id_ilev_out,&hybi_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,hybi_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,"hybm",crd_typ_out,dmn_nbr_1D,&dmn_id_lev_out,&hybm_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,hybm_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,ilev_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_ilev_out,&ilev_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,ilev_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lev_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_lev_out,&lev_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lev_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,"P0",crd_typ_out,dmn_nbr_0D,(int *)NULL,&p0_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,p0_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    //    for(dmn_idx=0;dmn_idx<dmn_nbr_out;dmn_idx++){
    //      rcd=nco_inq_dimname(out_id,dmn_ids_out[dmn_idx],dmn_nm);
    //      (void)fprintf(stdout,"%s: DEBUG quark5 dmn_nbr_out = %d, dmn_nbr_ps = %d, dmn_idx = %d, dmn_ids_out[%d] = %d, dmn_nm = %s\n",nco_prg_nm_get(),dmn_nbr_out,dmn_nbr_ps,dmn_idx,dmn_idx,dmn_ids_out[dmn_idx],dmn_nm);
    //    } /* !dmn_idx */
    if(flg_grd_hyb_cameam) rcd+=nco_def_var(out_id,"PS",crd_typ_out,dmn_nbr_ps,dmn_ids_out,&ps_id);
    if(flg_grd_hyb_ecmwf){
      /* Remove degenerate ECMWF vertical dimension so that output PS has dmn_nbr_ps-1 not dmn_nbr_ps dimensions */
      int dmn_nbr_out_ecmwf=0;
      for(dmn_idx=0;dmn_idx<dmn_nbr_ps;dmn_idx++){
	rcd=nco_inq_dimname(in_id,dmn_ids_in[dmn_idx],dmn_nm);
	if(strcmp(dmn_nm,ilev_nm_out) && strcmp(dmn_nm,lev_nm_out) && strcmp(dmn_nm,"lev_2"))
	  rcd=nco_inq_dimid(out_id,dmn_nm,dmn_ids_out+dmn_nbr_out_ecmwf++);
      } /* !dmn_idx */
      rcd+=nco_def_var(out_id,"PS",crd_typ_out,dmn_nbr_out_ecmwf,dmn_ids_out,&ps_id);
    } /* !flg_grd_hyb_ecmwf */
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,ps_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    (void)nco_att_cpy(tpl_id,out_id,hyai_id_tpl,hyai_id,PCK_ATT_CPY);
    (void)nco_att_cpy(tpl_id,out_id,hyam_id_tpl,hyam_id,PCK_ATT_CPY);
    (void)nco_att_cpy(tpl_id,out_id,hybi_id_tpl,hybi_id,PCK_ATT_CPY);
    (void)nco_att_cpy(tpl_id,out_id,hybm_id_tpl,hybm_id,PCK_ATT_CPY);

    if(p0_id_tpl != NC_MIN_INT) (void)nco_att_cpy(tpl_id,out_id,p0_id_tpl,p0_id,PCK_ATT_CPY); /* p0 not expected to be in ECMWF grids */
    if(ilev_id_tpl != NC_MIN_INT) (void)nco_att_cpy(tpl_id,out_id,ilev_id_tpl,ilev_id,PCK_ATT_CPY); else if(ilev_id_in != NC_MIN_INT) (void)nco_att_cpy(in_id,out_id,ilev_id_in,ilev_id,PCK_ATT_CPY);
    if(lev_id_tpl != NC_MIN_INT) (void)nco_att_cpy(tpl_id,out_id,lev_id_tpl,lev_id,PCK_ATT_CPY); else if(lev_id_in != NC_MIN_INT) (void)nco_att_cpy(in_id,out_id,lev_id_in,lev_id,PCK_ATT_CPY);
    if(ps_id_tpl != NC_MIN_INT) (void)nco_att_cpy(tpl_id,out_id,ps_id_tpl,ps_id,PCK_ATT_CPY); else (void)nco_att_cpy(in_id,out_id,ps_id_in,ps_id,PCK_ATT_CPY);
  } /* !flg_grd_out_hyb */

  if(flg_grd_out_prs){
    rcd+=nco_def_var(out_id,lev_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_lev_out,&lev_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lev_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    (void)nco_att_cpy(tpl_id,out_id,lev_id_tpl,lev_id,PCK_ATT_CPY);
    dmn_id_ilev_out=dmn_id_lev_out;
  } /* !flg_grd_out_prs */

  /* No further access to template file, close it */
  nco_close(tpl_id);

  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_tpl);

  char *dmn_nm_cp; /* [sng] Dimension name as char * to reduce indirection */
  nco_bool has_ilev; /* [flg] Contains interface level dimension */
  nco_bool has_lev; /* [flg] Contains midpoint level dimension */
  nco_bool has_tm; /* [flg] Contains time dimension */
  nco_bool need_prs_ntf=False; /* [flg] At least one variable to regrid is on interface levels */
  nco_bool need_prs_mdp=False; /* [flg] At least one variable to regrid is on midpoint levels */
  trv_sct trv; /* [sct] Traversal table object structure to reduce indirection */
  /* Define regridding flag for each variable */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      dmn_nbr_in=trv_tbl->lst[idx_tbl].nbr_dmn;
      has_ilev=False;
      has_lev=False;
      for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	/* Pre-determine flags necessary during next loop */
	dmn_nm_cp=trv.var_dmn[dmn_idx].dmn_nm;
	/* fxm: Generalize to include any variable containing coordinates with "standard_name" = "atmosphere_hybrid_sigma_pressure_coordinate" */
	if(!has_ilev && ilev_nm_in) has_ilev=!strcmp(dmn_nm_cp,ilev_nm_in);
	if(!has_lev) has_lev=!strcmp(dmn_nm_cp,lev_nm_in);
      } /* end loop over dimensions */
      /* Regrid variables that contain either vertical dimension */
      if(has_ilev || has_lev){
	trv_tbl->lst[idx_tbl].flg_rgr=True;
	var_rgr_nbr++;
	if(has_ilev) need_prs_ntf=True;
	if(has_lev) need_prs_mdp=True;
      } /* endif */
      assert(!(has_ilev && has_lev));
      /* Copy all variables that are not regridded or omitted */
      if(!trv_tbl->lst[idx_tbl].flg_rgr) var_cpy_nbr++;
    } /* end nco_obj_typ_var */
  } /* end idx_tbl */
  if(!var_rgr_nbr) (void)fprintf(stdout,"%s: WARNING %s reports no variables fit interpolation criteria. The vertical interpolator expects something to interpolate, and variables not interpolated are copied straight to output. HINT: If the name(s) of the input vertical grid dimensions (e.g., ilev and lev) do not match NCO's preset defaults (case-insensitive unambiguous forms and abbreviations of \"ilev\", \"lev\", and/or \"plev\", respectively) then change the dimension names that NCO looks for. Instructions are at http://nco.sf.net/nco.html#regrid. For hybrid-pressure coordinate grids, ensure that the \"ilev\" and \"lev\" variable names are known with, e.g., \"ncks --rgr ilev_nm=interface_level --rgr lev_nm=midpoint_level\" or \"ncremap -R '--rgr ilev=interface_level --rgr lev=midpoint_level'\". For pure pressure grids, ensure the \"plev\" coordinate name is defined with, e.g., \"ncks --rgr plev_nm=pressure_level\" or \"ncremap -R '--rgr plev=pressure_level'\".\n",nco_prg_nm_get(),fnc_nm);
  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
      trv=trv_tbl->lst[idx_tbl];
      if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr) (void)fprintf(stderr,"Interpolate %s? %s\n",trv.nm,trv.flg_rgr ? "Yes" : "No");
    } /* end idx_tbl */
  } /* end dbg */

  /* Pre-allocate dimension ID and cnt/srt space */
  int dmn_nbr_max; /* [nbr] Maximum number of dimensions variable can have in input or output */
  rcd+=nco_inq_ndims(in_id,&dmn_nbr_max);
  dmn_id_in=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  dmn_id_out=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
  if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
  if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
  dmn_cnt_in=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
  dmn_cnt_out=(long *)nco_malloc(dmn_nbr_max*sizeof(long));

  aed_sct aed_mtd_fll_val;
  char *att_nm_fll_val=strdup("_FillValue");
  int flg_pck; /* [flg] Variable is packed on disk  */
  nco_bool has_mss_val; /* [flg] Has numeric missing value attribute */
  float mss_val_flt;
  double mss_val_dbl;
  if(flg_add_msv_att){
    aed_mtd_fll_val.att_nm=att_nm_fll_val;
    aed_mtd_fll_val.mode=aed_create;
    aed_mtd_fll_val.sz=1L;
    mss_val_dbl=NC_FILL_DOUBLE;
    mss_val_flt=NC_FILL_FLOAT;
  } /* !flg_add_msv_att */

  /* Define interpolated and copied variables in output file */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      var_nm=trv.nm;
      /* Preserve input type in output type */
      var_typ_out=trv.var_typ;
      dmn_nbr_in=trv.nbr_dmn;
      dmn_nbr_out=trv.nbr_dmn;
      rcd=nco_inq_varid(in_id,var_nm,&var_id_in);
      rcd=nco_inq_varid_flg(out_id,var_nm,&var_id_out);
      /* If variable has not been defined, define it */
      if(rcd != NC_NOERR){
	if(trv.flg_rgr){
	  /* Interpolate */
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_id_in);
	  rcd=nco_inq_var_packing(in_id,var_id_in,&flg_pck);
	  if(flg_pck) (void)fprintf(stdout,"%s: WARNING %s reports variable \"%s\" is packed so results unpredictable. HINT: If regridded values seems weird, retry after unpacking input file with, e.g., \"ncpdq -U in.nc out.nc\"\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    rcd=nco_inq_dimname(in_id,dmn_id_in[dmn_idx],dmn_nm);
	    if(ilev_nm_in && !strcmp(dmn_nm,ilev_nm_in)){
	      /* Change ilev dimension */
	      dmn_id_out[dmn_idx]=dmn_id_ilev_out;
	      dmn_cnt_out[dmn_idx]=ilev_nbr_out;
	    }else if(!strcmp(dmn_nm,lev_nm_in)){
	      /* Change lev dimension */
	      dmn_id_out[dmn_idx]=dmn_id_lev_out;
	      dmn_cnt_out[dmn_idx]=lev_nbr_out;
	    }else{
	      /* Dimensions ilev/lev_nm_in have already been defined as ilev/lev_nm_out, replicate all other dimensions */
	      rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_id_out+dmn_idx);
	    } /* !ilev */
	    if(rcd != NC_NOERR){
	      rcd=nco_inq_dimlen(in_id,dmn_id_in[dmn_idx],dmn_cnt_out+dmn_idx);
	      /* Check-for and, if found, retain record dimension property */
	      for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		if(dmn_id_in[dmn_idx] == dmn_ids_rec[dmn_rec_idx])
		  dmn_cnt_out[dmn_idx]=NC_UNLIMITED;
	      rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt_out[dmn_idx],dmn_id_out+dmn_idx);
	    } /* !rcd */
	  } /* !dmn_idx */
	}else{ /* !flg_rgr */
	  /* Replicate non-interpolated variables */
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_id_in);
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    rcd=nco_inq_dimname(in_id,dmn_id_in[dmn_idx],dmn_nm);
	    rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_id_out+dmn_idx);
	    if(rcd != NC_NOERR){
	      rcd=nco_inq_dimlen(in_id,dmn_id_in[dmn_idx],dmn_cnt_out+dmn_idx);
	      /* Check-for and, if found, retain record dimension property */
	      for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		if(dmn_id_in[dmn_idx] == dmn_ids_rec[dmn_rec_idx])
		  dmn_cnt_out[dmn_idx]=NC_UNLIMITED;
	      rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt_out[dmn_idx],dmn_id_out+dmn_idx);
	    } /* !rcd */
	  } /* !dmn_idx */
	} /* !flg_rgr */
	rcd=nco_def_var(out_id,var_nm,var_typ_out,dmn_nbr_out,dmn_id_out,&var_id_out);
	/* Duplicate netCDF4 settings when possible */
	if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
	  /* Deflation */
	  if(dmn_nbr_out > 0){
	    int dfl_lvl_in; /* [enm] Deflate level [0..9] */
	    rcd=nco_inq_var_deflate(in_id,var_id_in,&shuffle,&deflate,&dfl_lvl_in);
	    /* Copy original deflation settings */
	    if(deflate || shuffle) (void)nco_def_var_deflate(out_id,var_id_out,shuffle,deflate,dfl_lvl_in);
	    /* Overwrite HDF Lempel-Ziv compression level, if requested */
	    if(dfl_lvl == 0) deflate=(int)False; else deflate=(int)True;
	    /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
	    if(dfl_lvl == 0) shuffle=NC_NOSHUFFLE;
	    /* Shuffle never, to my knowledge, increases filesize, so shuffle by default when manually deflating */
	    if(dfl_lvl >= 0) shuffle=NC_SHUFFLE;
	    if(dfl_lvl >= 0) (void)nco_def_var_deflate(out_id,var_id_out,shuffle,deflate,dfl_lvl);
	  } /* !dmn_nbr_out */
	} /* !NC_FORMAT_NETCDF4 */
	(void)nco_att_cpy(in_id,out_id,var_id_in,var_id_out,PCK_ATT_CPY);
	/* Variables with subterranean levels and missing-value extrapolation must have _FillValue attribute */
	if(flg_add_msv_att && trv.flg_rgr){
	  has_mss_val=nco_mss_val_get_dbl(in_id,var_id_in,&mss_val_dbl);
	  if(!has_mss_val){
	    nco_bool flg_att_chg; /* [flg] _FillValue attribute was written */
	    aed_mtd_fll_val.var_nm=var_nm;
	    aed_mtd_fll_val.id=var_id_out;
	    aed_mtd_fll_val.type=var_typ_out;
	    if(var_typ_out == NC_FLOAT) aed_mtd_fll_val.val.fp=&mss_val_flt;
	    else if(var_typ_out == NC_DOUBLE) aed_mtd_fll_val.val.dp=&mss_val_dbl;
	    flg_att_chg=nco_aed_prc(out_id,var_id_out,aed_mtd_fll_val);
	    if(!flg_att_chg && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING %s reports unsuccessful attempt to create _FillValue attribute for variable %s\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  } /* !has_mss_val */
	} /* !flg_add_msv_att */
      } /* !rcd */
    } /* !var */
  } /* end idx_tbl */
  
  /* Free pre-allocated array space */
  if(dmn_id_in) dmn_id_in=(int *)nco_free(dmn_id_in);
  if(dmn_id_out) dmn_id_out=(int *)nco_free(dmn_id_out);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
  if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
  if(dmn_ids_rec) dmn_ids_rec=(int *)nco_free(dmn_ids_rec);

  /* Turn-off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
      
  /* Begin data mode */
  (void)nco_enddef(out_id);

  /* Copy all grid variables */
  if(flg_grd_out_hyb){
    (void)nco_put_var(out_id,hyai_id,hyai_out,crd_typ_out);
    (void)nco_put_var(out_id,hyam_id,hyam_out,crd_typ_out);
    (void)nco_put_var(out_id,hybi_id,hybi_out,crd_typ_out);
    (void)nco_put_var(out_id,hybm_id,hybm_out,crd_typ_out);
    (void)nco_put_var(out_id,ilev_id,ilev_out,crd_typ_out);
    (void)nco_put_var(out_id,lev_id,lev_out,crd_typ_out);
    (void)nco_put_var(out_id,p0_id,&p0_out,crd_typ_out);
    (void)nco_put_var(out_id,ps_id,ps_out,crd_typ_out);
  } /* !flg_grd_out_hyb */
  
  if(flg_grd_out_prs){
    (void)nco_put_var(out_id,lev_id,lev_out,crd_typ_out);
  } /* !flg_grd_out_prs */

  nco_bool flg_ntp_log=True; /* [flg] Interpolate in log(vertical_coordinate) */
  if(ntp_mth == nco_ntp_lnr) flg_ntp_log=False;
  size_t idx_in; /* [idx] Index into 3D input variables */
  size_t idx_out; /* [idx] Index into 3D output variables */
  size_t var_sz_in; /* [nbr] Number of elements in variable (will be self-multiplied) */
  size_t var_sz_out; /* [nbr] Number of elements in variable (will be self-multiplied) */

  /* Interpolate or copy variable values */
  double *var_val_dbl_in=NULL;
  double *var_val_dbl_out=NULL;
  double *prs_ntp_in; /* [Pa] Interpolated pressure array on input grid */
  double *prs_ntp_out; /* [Pa] Interpolated pressure array on output grid */
  int lvl_idx_in; /* [idx] Level index on input grid */
  int lvl_idx_out; /* [idx] Level index on output grid */
  int lvl_nbr_in; /* [nbr] Number of levels for current interpolated variable on input grid */
  int lvl_nbr_out; /* [nbr] Number of levels for current interpolated variable on output grid */
  int thr_idx; /* [idx] Thread index */
  size_t grd_nbr=grd_sz_in; /* [nbr] Horizonal grid size */
  size_t idx_dbg=rgr->idx_dbg;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped as shared in parallel clause */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  /* Repeating above documentation for the forgetful:
     NB: tm_nbr is max(timesteps) in vertical grid definitions, not number of records in either file
     This implementation interpolates timeseries to/from time-invariant vertical grids in one OpenMP call! */
  for(tm_idx=0;tm_idx<tm_nbr;tm_idx++){

    /* Index-offset to current surface pressure timeslice */
    idx_fst=tm_idx*grd_sz_in;
    
    if(need_prs_mdp){
      /* Allocated and define midpoint pressures */
      if(tm_idx == 0) prs_mdp_in=(double *)nco_malloc_dbg(grd_sz_in*lev_nbr_in*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() prs_mdp_in value buffer");
      if(tm_idx == 0) prs_mdp_out=(double *)nco_malloc_dbg(grd_sz_out*lev_nbr_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() prs_mdp_out value buffer");
      if(flg_grd_in_hyb)
	for(grd_idx=0;grd_idx<grd_sz_in;grd_idx++)
	  for(lev_idx=0;lev_idx<lev_nbr_in;lev_idx++)
	    prs_mdp_in[grd_idx+lev_idx*grd_sz_in]=p0_in*hyam_in[lev_idx]+ps_in[idx_fst+grd_idx]*hybm_in[lev_idx];
      if(flg_grd_out_hyb)
	for(grd_idx=0;grd_idx<grd_sz_out;grd_idx++)
	  for(lev_idx=0;lev_idx<lev_nbr_out;lev_idx++)
	    prs_mdp_out[grd_idx+lev_idx*grd_sz_out]=p0_out*hyam_out[lev_idx]+ps_out[idx_fst+grd_idx]*hybm_out[lev_idx];
      if(flg_grd_in_prs)
	for(grd_idx=0;grd_idx<grd_sz_in;grd_idx++)
	  for(lev_idx=0;lev_idx<lev_nbr_in;lev_idx++)
	    prs_mdp_in[grd_idx+lev_idx*grd_sz_in]=lev_in[lev_idx];
      if(flg_grd_out_prs)
	for(grd_idx=0;grd_idx<grd_sz_out;grd_idx++)
	  for(lev_idx=0;lev_idx<lev_nbr_out;lev_idx++)
	    prs_mdp_out[grd_idx+lev_idx*grd_sz_out]=lev_out[lev_idx];
      if(flg_ntp_log){
	var_sz_in=grd_sz_in*lev_nbr_in;
	for(idx_in=0;idx_in<var_sz_in;idx_in++) prs_mdp_in[idx_in]=log(prs_mdp_in[idx_in]);
	var_sz_out=grd_sz_out*lev_nbr_out;
	for(idx_out=0;idx_out<var_sz_out;idx_out++) prs_mdp_out[idx_out]=log(prs_mdp_out[idx_out]);
      } /* !flg_ntp_log */
    } /* !need_prs_mdp */

    if(need_prs_ntf){
      /* Allocate and define interface pressures */
      if(tm_idx == 0) prs_ntf_in=(double *)nco_malloc_dbg(grd_sz_in*ilev_nbr_in*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() prs_ntf_in value buffer");
      if(tm_idx == 0) prs_ntf_out=(double *)nco_malloc_dbg(grd_sz_out*ilev_nbr_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() prs_ntf_out value buffer");
      if(flg_grd_in_hyb)
	for(grd_idx=0;grd_idx<grd_sz_in;grd_idx++)
	  for(ilev_idx=0;ilev_idx<ilev_nbr_in;ilev_idx++)
	    prs_ntf_in[grd_idx+ilev_idx*grd_sz_in]=p0_in*hyai_in[ilev_idx]+ps_in[idx_fst+grd_idx]*hybi_in[ilev_idx];
      if(flg_grd_out_hyb)
	for(grd_idx=0;grd_idx<grd_sz_out;grd_idx++)
	  for(ilev_idx=0;ilev_idx<ilev_nbr_out;ilev_idx++)
	    prs_ntf_out[grd_idx+ilev_idx*grd_sz_out]=p0_out*hyai_out[ilev_idx]+ps_out[idx_fst+grd_idx]*hybi_out[ilev_idx];
      if(flg_grd_in_prs)
	for(grd_idx=0;grd_idx<grd_sz_in;grd_idx++)
	  for(ilev_idx=0;ilev_idx<ilev_nbr_in;ilev_idx++)
	    prs_ntf_in[grd_idx+ilev_idx*grd_sz_in]=lev_in[ilev_idx];
      if(flg_grd_out_prs)
	for(grd_idx=0;grd_idx<grd_sz_out;grd_idx++)
	  for(ilev_idx=0;ilev_idx<ilev_nbr_out;ilev_idx++)
	    prs_ntf_out[grd_idx+ilev_idx*grd_sz_out]=lev_out[ilev_idx];
      if(flg_ntp_log){
	var_sz_in=grd_sz_in*ilev_nbr_in;
	for(idx_in=0;idx_in<var_sz_in;idx_in++) prs_ntf_in[idx_in]=log(prs_ntf_in[idx_in]);
	var_sz_out=grd_sz_out*ilev_nbr_out;
	for(idx_out=0;idx_out<var_sz_out;idx_out++) prs_ntf_out[idx_out]=log(prs_ntf_out[idx_out]);
      } /* !flg_ntp_log */
    } /* !need_prs_ntf */

    /* Set firstprivate variables to initial values */
    has_ilev=False;
    has_lev=False;
    has_tm=False;
    
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"Interpolation progress: # means interpolated, ~ means copied\n");

#ifdef __GNUG__
# define GCC_LIB_VERSION ( __GNUC__ * 100 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__ )
# if GCC_LIB_VERSION < 490
#  define GXX_OLD_OPENMP_SHARED_TREATMENT 1
# endif /* 480 */
# if GCC_LIB_VERSION >= 900
#  define GXX_WITH_OPENMP5_GPU_SUPPORT 1
# endif /* 900 */
#endif /* !__GNUC__ */
#if defined( __INTEL_COMPILER)
#  pragma omp parallel for default(none) firstprivate(has_ilev,has_lev,has_tm,var_val_dbl_in,var_val_dbl_out) private(dmn_cnt_in,dmn_cnt_out,dmn_id_in,dmn_id_out,dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dmn_nm,dmn_srt,grd_idx,has_mss_val,idx_in,idx_out,idx_tbl,in_id,lvl_idx_in,lvl_idx_out,lvl_nbr_in,lvl_nbr_out,mss_val_dbl,prs_ntp_in,prs_ntp_out,rcd,thr_idx,trv,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr) shared(dmn_id_ilev_in,dmn_id_ilev_out,dmn_id_lev_in,dmn_id_lev_out,dmn_id_tm_in,flg_ntp_log,flg_vrt_tm,fnc_nm,grd_nbr,idx_dbg,ilev_nbr_in,ilev_nbr_out,lev_nbr_in,lev_nbr_out,out_id,prs_mdp_in,prs_mdp_out,prs_ntf_in,prs_ntf_out,tm_idx,xtr_mth)
#else /* !__INTEL_COMPILER */
# ifdef GXX_OLD_OPENMP_SHARED_TREATMENT
#  pragma omp parallel for default(none) firstprivate(has_ilev,has_lev,has_tm,var_val_dbl_in,var_val_dbl_out) private(dmn_cnt_in,dmn_cnt_out,dmn_id_in,dmn_id_out,dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dmn_nm,dmn_srt,grd_idx,has_mss_val,idx_in,idx_out,idx_tbl,in_id,lvl_idx_in,lvl_idx_out,lvl_nbr_in,lvl_nbr_out,mss_val_dbl,prs_ntp_in,prs_ntp_out,rcd,thr_idx,trv,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr) shared(dmn_id_ilev_in,dmn_id_ilev_out,dmn_id_lev_in,dmn_id_lev_out,dmn_id_tm_in,flg_ntp_log,flg_vrt_tm,fnc_nm,grd_nbr,idx_dbg,ilev_nbr_in,ilev_nbr_out,lev_nbr_in,lev_nbr_out,out_id,prs_mdp_in,prs_mdp_out,prs_ntf_in,prs_ntf_out,tm_idx,xtr_mth)
# else /* !old g++ */
#  if defined(GXX_WITH_OPENMP5_GPU_SUPPORT) && 0
#   pragma omp target teams distribute parallel for
#  else
#   pragma omp parallel for firstprivate(has_ilev,has_lev,has_tm,var_val_dbl_in,var_val_dbl_out) private(dmn_cnt_in,dmn_cnt_out,dmn_id_in,dmn_id_out,dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dmn_nm,dmn_srt,grd_idx,has_mss_val,idx_in,idx_out,idx_tbl,in_id,lvl_idx_in,lvl_idx_out,lvl_nbr_in,lvl_nbr_out,mss_val_dbl,prs_ntp_in,prs_ntp_out,rcd,thr_idx,trv,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr) shared(dmn_id_ilev_in,dmn_id_ilev_out,dmn_id_lev_in,dmn_id_lev_out,dmn_id_tm_in,flg_ntp_log,flg_vrt_tm,grd_nbr,idx_dbg,ilev_nbr_in,ilev_nbr_out,lev_nbr_in,lev_nbr_out,out_id,prs_mdp_in,prs_mdp_out,prs_ntf_in,prs_ntf_out,tm_idx,xtr_mth)
#  endif /* !GCC > 9.0 */
# endif /* !GCC < 4.9 */
#endif /* !__INTEL_COMPILER */
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
      trv=trv_tbl->lst[idx_tbl];
      thr_idx=omp_get_thread_num();
      in_id=trv_tbl->in_id_arr[thr_idx];
#ifdef _OPENMP
      if(nco_dbg_lvl_get() >= nco_dbg_grp && !thr_idx && !idx_tbl) (void)fprintf(fp_stdout,"%s: INFO %s reports regrid loop uses %d thread%s\n",nco_prg_nm_get(),fnc_nm,omp_get_num_threads(),(omp_get_num_threads() > 1) ? "s" : "");
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(fp_stdout,"%s: INFO thread = %d, idx_tbl = %d, nm = %s\n",nco_prg_nm_get(),thr_idx,idx_tbl,trv.nm);
#endif /* !_OPENMP */
      if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
	if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(fp_stdout,"%s%s ",trv.flg_rgr ? "#" : "~",trv.nm);
	if(trv.flg_rgr){
	  /* Interpolate variable */
	  var_nm=trv.nm;
	  if(!strcmp(var_nm,"US") || !strcmp(var_nm,"VS")) (void)fprintf(fp_stdout,"%s: WARNING %s reports attempt to vertically interpolate a variable named \"%s\". If this variable is from a CESM CAM or E3SM EAM output or initial condition file on a rectangular grid (e.g., FV 0.9x1.25), then expect this program to fail and dump core when interpolating US and to produce slightly incorrect answers for VS. The vertical interpolation routine requires that interpolated variables be on the same horizontal grid as the supplied pressure field. However, the CAM/EAM US and VS variables from rectangular grid simulations are often on a horizontal grid, called the staggered grid, that is offset from the rest of the variables including the surface pressure. US usually sits on a grid that is staggered in latitude from, and is a slightly different size than, the surface pressure grid. This leads to a core dump. VS sits on a grid staggered in longitude from, though the same size as, the surface pressure field. The resulting interpolation will be based on surface pressure half a gridcell to the east rather than centered with VS. The correct procedure to vertically interpolate US and VS is to 1) horizontally regrid the supplied surface pressure (often \"PS\") to the staggered grid, then 2) vertically interpolate US and VS to the desired vertical grid based on the surface pressure on the staggered grid, then 3) re-combine the interpolated US and VS with the interpolated versions of the rest of the variables. The best solution to this dilemma is to script this workflow. Contact Charlie if you need help with this.\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  var_typ_rgr=NC_DOUBLE; /* NB: Perform regridding in double precision */
	  var_typ_out=trv.var_typ; /* NB: Output type in file is same as input type */
	  var_sz_in=1L;
	  var_sz_out=1L;
	  rcd=nco_inq_varid(in_id,var_nm,&var_id_in);
	  rcd=nco_inq_varid(out_id,var_nm,&var_id_out);
	  rcd=nco_inq_varndims(in_id,var_id_in,&dmn_nbr_in);
	  rcd=nco_inq_varndims(out_id,var_id_out,&dmn_nbr_out);
	  dmn_nbr_max= dmn_nbr_in > dmn_nbr_out ? dmn_nbr_in : dmn_nbr_out;
	  dmn_id_in=(int *)nco_malloc(dmn_nbr_in*sizeof(int));
	  dmn_id_out=(int *)nco_malloc(dmn_nbr_out*sizeof(int));
	  dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long)); /* max() for both input and output grids */
	  dmn_cnt_in=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
	  dmn_cnt_out=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_id_in);
	  rcd=nco_inq_vardimid(out_id,var_id_out,dmn_id_out);
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    rcd=nco_inq_dimlen(in_id,dmn_id_in[dmn_idx],dmn_cnt_in+dmn_idx);
	    if(dmn_id_in[dmn_idx] == dmn_id_ilev_in) has_ilev=True;
	    if(dmn_id_in[dmn_idx] == dmn_id_lev_in) has_lev=True;
	    if(dmn_id_in[dmn_idx] == dmn_id_tm_in) has_tm=True;
	    if(flg_vrt_tm && has_tm && dmn_id_in[dmn_idx] == dmn_id_tm_in){
	      dmn_cnt_in[dmn_idx]=1L;
	      dmn_srt[dmn_idx]=tm_idx;
	    }else{
	      dmn_srt[dmn_idx]=0L;
	    } /* !flg_vrt_tm */
	    var_sz_in*=dmn_cnt_in[dmn_idx];
	  } /* !dmn_idx */
	  var_val_dbl_in=(double *)nco_malloc_dbg(var_sz_in*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() input value buffer");
	  rcd=nco_get_vara(in_id,var_id_in,dmn_srt,dmn_cnt_in,var_val_dbl_in,var_typ_rgr);

	  for(dmn_idx=0;dmn_idx<dmn_nbr_out;dmn_idx++){
	    /* Dimension count vector is same as input except for lvl dimension */
	    dmn_cnt_out[dmn_idx]=dmn_cnt_in[dmn_idx];
	    if(has_ilev && dmn_id_out[dmn_idx] == dmn_id_ilev_out) dmn_cnt_out[dmn_idx]=ilev_nbr_out;
	    if(has_lev && dmn_id_out[dmn_idx] == dmn_id_lev_out) dmn_cnt_out[dmn_idx]=lev_nbr_out;
	    var_sz_out*=dmn_cnt_out[dmn_idx];
	  } /* !dmn_idx */
	  var_val_dbl_out=(double *)nco_malloc_dbg(var_sz_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() output value buffer");

	  /* Missing value setup */
	  has_mss_val=nco_mss_val_get_dbl(in_id,var_id_in,&mss_val_dbl);
	  if(!has_mss_val) mss_val_dbl=NC_FILL_DOUBLE;
	  
	  if(has_ilev){
	    /* Interpolate current variable from input interface pressure grid to output interface pressure grid */
	    lvl_nbr_in=ilev_nbr_in;
	    lvl_nbr_out=ilev_nbr_out;
	    prs_ntp_in=prs_ntf_in;
	    prs_ntp_out=prs_ntf_out;
	  }else{
	    /* Interpolate current variable from input midpoint pressure grid to output midpoint pressure grid */
	    lvl_nbr_in=lev_nbr_in;
	    lvl_nbr_out=lev_nbr_out;
	    prs_ntp_in=prs_mdp_in;
	    prs_ntp_out=prs_mdp_out;
	  } /* !ilev */	  
	  
	  /* Procedure: Extract input/output coordinate/data arrays into 1D column order
	     This enables actual interpolation code to be written for, or take advantage of, 1D interpolation routines 
	     After interpolating into 1D sequential memory, copy back to ND output and repeat */
	  double *crd_in=NULL; /* Input vertical coordinate (must be monotonic) */
	  double *crd_out=NULL; /* Output vertical coordinate (must be monotonic) */
	  double *dat_in=NULL; /* Input data (to be interpolated) on input vertical coordinate grid */
	  double *dat_out=NULL; /* Output data (interpolated) output vertical coordinate grid (i.e., the answer) */
	  double *crd_in_mnt; /* Input vertical coordinate reversed if necessary to be monotonically increasing */
	  double *crd_out_mnt; /* Output vertical coordinate reversed if necessary to be monotonically increasing */
	  double *dat_in_mnt; /* Input data (to be interpolated) reversed if necessary along with input grid */
	  double *dat_out_mnt; /* Output data (interpolated) reversed if necessary along with output grid */
	  nco_xtr_sct xtr_LHS;
	  nco_xtr_sct xtr_RHS;
	  size_t brk_lft_idx;
	  size_t brk_rgt_idx;
	  size_t in_idx;
	  size_t in_nbr;
	  size_t out_nbr;
	  size_t out_idx;
	  /* Default extrapolation uses nearest valid neighbor */
	  xtr_LHS.xtr_fll=True;
	  xtr_LHS.xtr_vrb=False;
	  xtr_LHS.typ_fll=xtr_mth;
	  xtr_RHS.xtr_fll=True;
	  xtr_RHS.xtr_vrb=False;
	  xtr_RHS.typ_fll=xtr_mth;
	  /* Special-case extrapolation methods allowed for all except missing-value extrapolation types */
	  if(xtr_mth != nco_xtr_fll_msv){
	    if(!strcmp(var_nm,"T") || !strcmp(var_nm,"ta")) xtr_RHS.typ_fll=nco_xtr_fll_tpt;
	    else if(!strcmp(var_nm,"Z3") || !strcmp(var_nm,"zg")) xtr_LHS.typ_fll=xtr_RHS.typ_fll=nco_xtr_fll_gph;
	  } /* !xtr_mth */
	  crd_in=(double *)nco_malloc(lvl_nbr_in*sizeof(double));
	  crd_out=(double *)nco_malloc(lvl_nbr_out*sizeof(double));
	  dat_in=(double *)nco_malloc(lvl_nbr_in*sizeof(double));
	  dat_out=(double *)nco_malloc(lvl_nbr_out*sizeof(double));
	  in_nbr=lvl_nbr_in;
	  out_nbr=lvl_nbr_out;
	  
	  nco_bool in_ncr; /* [flg] Input coordinate monotonically increases */
	  nco_bool out_ncr; /* [flg] Output coordinate monotonically increases */
	  
	  /* Determine monotonicity direction only once, based on first vertical column */
	  if(prs_ntp_in[grd_nbr]-prs_ntp_in[0] > 0.0) in_ncr=True; else in_ncr=False;
	  out_ncr=True;
	  if(out_nbr > 1)
	    if(prs_ntp_out[grd_nbr]-prs_ntp_out[0] < 0.0)
	      out_ncr=False;
	  
	  /* If necessary, allocate (once, and re-use it) additional memory to hold reversed arrays */
	  if(!in_ncr){
	    crd_in_mnt=(double *)nco_malloc(lvl_nbr_in*sizeof(double));
	    dat_in_mnt=(double *)nco_malloc(lvl_nbr_in*sizeof(double));
	  } /* !in_ncr */
	  if(!out_ncr){
	    crd_out_mnt=(double *)nco_malloc(lvl_nbr_out*sizeof(double));
	    dat_out_mnt=(double *)nco_malloc(lvl_nbr_out*sizeof(double));
	  } /* !out_ncr */
	  
	  /* Constants and parameters for extrapolation */
	  const double gamma_moist=6.5/10000.0; /* [K/Pa] Temperature extrapolation assumes constant moist adiabatic lower atmosphere lapse rate dT/dp=constant=(6.5 K)/(100 mb) = (6.5 K)/(10000 Pa) */
	  const double Rd_rcp_g0=287.0/9.81; /* [K/Pa] Geopotential height extrapolation uses hypsometric equation Z2-Z1=(Rd*Tv_avg/g0)*ln(p1/p2)=(Rd*Tv_avg/g0)*(ln(p1)-ln(p2)) */
	  const double tpt_vrt_avg=288.0; /* [K] Mean virtual temperature assumed for geopotential height extrapolation */
	  nco_bool FIRST_WARNING_LHS; /* [flg] First warning for LHS extrapolation */
	  nco_bool FIRST_WARNING_RHS; /* [flg] First warning for RHS extrapolation */
	  if(tm_idx == 0){
	    /* Only print extrapolation warnings for first timestep to prevent noisy output
	       NB: Algorithm prevents any warnings for extrapolations that appear after first timestep */
	    FIRST_WARNING_LHS=True;
	    FIRST_WARNING_RHS=True;
	  } /* !tm_idx */
	  
	  /* Outer loop over columns */
	  for(grd_idx=0;grd_idx<grd_nbr;grd_idx++){
	    
	    /* Initialize pseudo-1D variables with consecutive memory addresses to avoid indirection */
	    for(lvl_idx_in=0;lvl_idx_in<lvl_nbr_in;lvl_idx_in++){
	      idx_in=grd_idx+lvl_idx_in*grd_nbr;
	      crd_in[lvl_idx_in]=prs_ntp_in[idx_in];
	      dat_in[lvl_idx_in]=var_val_dbl_in[idx_in];
	    } /* !lvl_idx_in */
	    for(lvl_idx_out=0;lvl_idx_out<lvl_nbr_out;lvl_idx_out++){
	      idx_out=grd_idx+lvl_idx_out*grd_nbr;
	      crd_out[lvl_idx_out]=prs_ntp_out[idx_out];
	    } /* !lvl_idx_out */
	    
	    /* Interpolation code easier to write/debug if crd_in and crd_out both monotonically increase
	       However, monotonically decreasing coordinates useful in many cases, such as depth coordinate, 
	       and pressure levels arranged largest to smallest (favored by CMIP)
	       Next code block reverses array(s) if necessary so coordinates monotonically increase
	       Code uses crd_in_mnt, dat_in_mnt, crd_out_mnt where "_mnt" reminds of "monotonically increasing" assumption
	       Following code lifted from CSZ's libcsz.a library source code ~/sw/c++/vec.hh */

	    if(in_ncr){
	      crd_in_mnt=crd_in;
	      dat_in_mnt=dat_in;
	    }else{
	      for(in_idx=0;in_idx<in_nbr;in_idx++){
		crd_in_mnt[in_idx]=crd_in[in_nbr-in_idx-1];
		dat_in_mnt[in_idx]=dat_in[in_nbr-in_idx-1];
	      } /* !in_idx */
	    } /* !in_ncr */
	    
	    if(out_ncr){
	      crd_out_mnt=crd_out;
	      dat_out_mnt=dat_out;
	    }else{
	      for(out_idx=0;out_idx<out_nbr;out_idx++)
		crd_out_mnt[out_idx]=crd_out[out_nbr-out_idx-1];
	    } /* !out_ncr */
	    
	    // Initialize bracketing index
	    brk_lft_idx=0;
	    // Loop over desired output coordinates
	    for(out_idx=0;out_idx<out_nbr;out_idx++){
	      // Order of conditions is important since second condition is illegal if brk_lft_idx >= in_nbr
	      while((brk_lft_idx < in_nbr) && (crd_in_mnt[brk_lft_idx] < crd_out_mnt[out_idx])){
		brk_lft_idx++;
	      } // !while
	      brk_lft_idx--;
	      // Handle identity interpolation separately to preserve symmetry in extrapolation code 
	      if(brk_lft_idx != in_nbr-1){
		if(crd_in_mnt[brk_lft_idx+1] == crd_out_mnt[out_idx]){
		  dat_out_mnt[out_idx]=dat_in_mnt[brk_lft_idx+1];
		  if(brk_lft_idx == -1) brk_lft_idx=0; // Reset brk_lft_idx to 0 so next while loop works
		  continue; // Jump to next iteration
		} // !crd_in_mnt
	      } // !brk_lft_idx
	      if(brk_lft_idx == -1){
		// LHS Extrapolation required
		// Degenerate case: crd_out_mnt[out_idx] < crd_in_mnt[0]
		brk_lft_idx=0; // Reset brk_lft_idx to 0 so next while loop works
		if(xtr_LHS.xtr_vrb) (void)fprintf(fp_stdout,"%s: WARNING %s reports variable %s column %lu output value dat_out_mnt[%lu] at coordinate crd_out_mnt[%lu] = %g requires LHS extrapolation beyond leftmost valid coordinate at crd_in_mnt[%lu] = %g. Nearest valid datum is dat_in_mnt[%lu] = %g\n",nco_prg_nm_get(),fnc_nm,var_nm,grd_idx,out_idx,out_idx,crd_out_mnt[out_idx],brk_lft_idx,crd_in_mnt[brk_lft_idx],brk_lft_idx,dat_in_mnt[brk_lft_idx]);
		// Extrapolation options are presented in decreasing order of preference
		if(!xtr_LHS.xtr_fll){
		  (void)fprintf(fp_stdout,"%s: ERROR %s Full LHS extrapolation required but not permitted\n",nco_prg_nm_get(),fnc_nm);
		  // return NCO_ERR;
		} /* !xtr_LHS.xtr_fll */
		switch(xtr_LHS.typ_fll){
		case nco_xtr_fll_nil:
		  dat_out_mnt[out_idx]=0.0;
		  break;
		case nco_xtr_fll_msv:
		  dat_out_mnt[out_idx]=mss_val_dbl;
		  break;
		case nco_xtr_fll_ngh:
		  dat_out_mnt[out_idx]=dat_in_mnt[0];
		  break;
		case nco_xtr_fll_lnr:
		  dat_out_mnt[out_idx]=dat_in_mnt[0]-
		    (crd_in_mnt[0]-crd_out_mnt[out_idx])*
		    (dat_in_mnt[1]-dat_in_mnt[0])/(crd_in_mnt[1]-crd_in_mnt[0]);
		  break;
		case nco_xtr_fll_gph:
		  if(flg_ntp_log) /* Coordinates are already logarithmic in pressure */
		    dat_out_mnt[out_idx]=dat_in_mnt[0]+
		      Rd_rcp_g0*tpt_vrt_avg*(crd_in_mnt[0]-crd_out_mnt[out_idx]);
		  else /* Interpolate with logarithm of pressure coordinates */
		    dat_out_mnt[out_idx]=dat_in_mnt[0]+
		      Rd_rcp_g0*tpt_vrt_avg*log(crd_in_mnt[0]/crd_out_mnt[out_idx]);
		  if(FIRST_WARNING_LHS) (void)fprintf(fp_stdout,"%s: INFO %s geopotential height extrapolated upward towards space using hypsometric equation with constant global mean virtual temperature = %g for variable %s\n",nco_prg_nm_get(),fnc_nm,tpt_vrt_avg,var_nm);
		  FIRST_WARNING_LHS=False;
		  break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR %s Unknown xtr_LHS.typ_fll\n",nco_prg_nm_get(),fnc_nm);
		  // return NCO_ERR;
		  break;
		} // !xtr_LHS.typ_fll
		if(xtr_LHS.xtr_vrb) (void)fprintf(fp_stdout,"%s: INFO %s LHS extrapolation yields dat_out_mnt[%lu] = %g\n",nco_prg_nm_get(),fnc_nm,out_idx,dat_out_mnt[out_idx]);
	      }else if(brk_lft_idx < in_nbr-1){
		// Normal case: crd_out_mnt is interpolable
		brk_rgt_idx=brk_lft_idx+1; 
		// NB: brk_rgt_idx is ALWAYS greater than brk_lft_idx
		// This simulaneously meets two criteria:
		// 1. Divide-by-zero errors are impossible in the next step
		// 2. The identity interpolation is satisfied since crd_dlt == 0.0: 
		// i.e., If crd_out_mnt[idx] == crd_in_mnt[brk_lft_idx] then dat_out_mnt[out_idx] := dat_in_mnt[brk_lft_idx]
		// Linearly interpolate
		dat_out_mnt[out_idx]=
		  dat_in_mnt[brk_lft_idx]+
		  (crd_out_mnt[out_idx]-crd_in_mnt[brk_lft_idx])*
		  (dat_in_mnt[brk_rgt_idx]-dat_in_mnt[brk_lft_idx])/
		  (crd_in_mnt[brk_rgt_idx]-crd_in_mnt[brk_lft_idx]);
	      }else if(brk_lft_idx == in_nbr-1){
		// RHS Extrapolation required
		// Degenerate case: brk_lft_idx is last element of crd_in_mnt 
		brk_rgt_idx=brk_lft_idx;
		if(xtr_RHS.xtr_vrb) (void)fprintf(fp_stdout,"%s: WARNING %s reports variable %s column %lu output value dat_out_mnt[%lu] at coordinate crd_out_mnt[%lu] = %g requires RHS extrapolation beyond rightmost valid coordinate at crd_in_mnt[%lu] = %g. Nearest valid datum is dat_in_mnt[%lu] = %g\n",nco_prg_nm_get(),fnc_nm,var_nm,grd_idx,out_idx,out_idx,crd_out_mnt[out_idx],brk_rgt_idx,crd_in_mnt[brk_rgt_idx],brk_rgt_idx,dat_in_mnt[brk_rgt_idx]);
		// Extrapolation options are presented in decreasing order of preference
		if(!xtr_RHS.xtr_fll){
		  (void)fprintf(fp_stdout,"%s: ERROR %s Full RHS extrapolation required but not permitted\n",nco_prg_nm_get(),fnc_nm);
		// return NCO_ERR;
		} /* !xtr_RHS.xtr_fll */
		switch(xtr_RHS.typ_fll){
		case nco_xtr_fll_nil:
		  dat_out_mnt[out_idx]=0.0;
		  break;
		case nco_xtr_fll_msv:
		  dat_out_mnt[out_idx]=mss_val_dbl;
		  break;
		case nco_xtr_fll_ngh:
		  dat_out_mnt[out_idx]=dat_in_mnt[in_nbr-1];
		  break;
		case nco_xtr_fll_lnr:
		  dat_out_mnt[out_idx]=dat_in_mnt[in_nbr-1]+
		    (crd_out_mnt[out_idx]-crd_in_mnt[in_nbr-1])*
		    (dat_in_mnt[in_nbr-1]-dat_in_mnt[in_nbr-2])/
		    (crd_in_mnt[in_nbr-1]-crd_in_mnt[in_nbr-2]);
		  break;
		case nco_xtr_fll_tpt:
		  if(flg_ntp_log) /* Exponentiate so coordinates are linear in pressure */
		    dat_out_mnt[out_idx]=dat_in_mnt[in_nbr-1]+
		      (exp(crd_out_mnt[out_idx])-exp(crd_in_mnt[in_nbr-1]))*gamma_moist;
		  else /* Coordinates are already linear in pressure */
		    dat_out_mnt[out_idx]=dat_in_mnt[in_nbr-1]+
		      (crd_out_mnt[out_idx]-crd_in_mnt[in_nbr-1])*gamma_moist;
		  if(FIRST_WARNING_RHS) (void)fprintf(fp_stdout,"%s: INFO %s temperature extrapolated toward/into surface assuming constant moist adiabatic lapse rate = %g K/(100 mb) for variable %s\n",nco_prg_nm_get(),fnc_nm,gamma_moist*10000.0,var_nm);
		  FIRST_WARNING_RHS=False;
		  break;
		case nco_xtr_fll_gph:
		  if(flg_ntp_log) /* Coordinates are already logarithmic in pressure */
		    dat_out_mnt[out_idx]=dat_in_mnt[in_nbr-1]-
		      Rd_rcp_g0*tpt_vrt_avg*(crd_out_mnt[out_idx]-crd_in_mnt[in_nbr-1]);
		  else /* Interpolate with logarithm of pressure coordinates */
		    dat_out_mnt[out_idx]=dat_in_mnt[in_nbr-1]-
		      Rd_rcp_g0*tpt_vrt_avg*log(crd_out_mnt[out_idx]/crd_in_mnt[in_nbr-1]);
		  if(FIRST_WARNING_RHS) (void)fprintf(fp_stdout,"%s: INFO %s geopotential height extrapolated toward/into surface using hypsometric equation with constant global mean virtual temperature = %g for variable %s\n",nco_prg_nm_get(),fnc_nm,tpt_vrt_avg,var_nm);
		  FIRST_WARNING_RHS=False;
		  break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR %s Unknown xtr_RHS\n",nco_prg_nm_get(),fnc_nm);
		  // return NCO_ERR;
		  break;
		} // !xtr_RHS.typ_fll
		if(xtr_RHS.xtr_vrb) (void)fprintf(fp_stdout,"%s: INFO %s RHS extrapolation yields dat_out_mnt[%lu] = %g\n",nco_prg_nm_get(),fnc_nm,out_idx,dat_out_mnt[out_idx]);
	      }else{
		(void)fprintf(fp_stdout,"%s: ERROR %s Unforeseen value of brk_lft_idx\n",nco_prg_nm_get(),fnc_nm);
		// return NCO_ERR;
	      } // !RHS
	    } // !out_idx
	    
	    /* Un-reverse output data to be on original grid */
	    if(!out_ncr)
	      for(out_idx=0;out_idx<out_nbr;out_idx++)
		dat_out[out_idx]=dat_out_mnt[out_nbr-out_idx-1];
	    // End of vec.hh code
	    
	    /* Copy answers into output array */
	    for(lvl_idx_out=0;lvl_idx_out<lvl_nbr_out;lvl_idx_out++){
	      idx_out=grd_idx+lvl_idx_out*grd_nbr;
	      var_val_dbl_out[idx_out]=dat_out[lvl_idx_out];
	    } /* !lvl_idx_out */
	    
	    if(nco_dbg_lvl_get() >= nco_dbg_io && grd_idx == idx_dbg){
	      (void)fprintf(fp_stdout,"%s: DEBUG %s variable %s at idx_dbg = %lu\n",nco_prg_nm_get(),fnc_nm,var_nm,idx_dbg);
	      for(out_idx=0;out_idx<out_nbr;out_idx++){
		(void)fprintf(fp_stdout,"out_idx = %lu dat_out = %g\n",out_idx,dat_out[out_idx]);
	      } /* !out_idx */
	    } /* !dbg */
	    
	  } /* !grd_idx */
	  
	  if(crd_in) crd_in=(double *)nco_free(crd_in);
	  if(crd_out) crd_out=(double *)nco_free(crd_out);
	  if(dat_in) dat_in=(double *)nco_free(dat_in);
	  if(dat_out) dat_out=(double *)nco_free(dat_out);
	  
	  if(!in_ncr){
	    if(crd_in_mnt) crd_in_mnt=(double *)nco_free(crd_in_mnt);
	    if(dat_in_mnt) dat_in_mnt=(double *)nco_free(dat_in_mnt);
	  } /* !in_ncr */
	  if(!out_ncr){
	    if(crd_out_mnt) crd_out_mnt=(double *)nco_free(crd_out_mnt);
	    if(dat_out_mnt) dat_out_mnt=(double *)nco_free(dat_out_mnt);
	  } /* !out_ncr */
	  
#pragma omp critical
	  { /* begin OpenMP critical */
	    rcd=nco_put_vara(out_id,var_id_out,dmn_srt,dmn_cnt_out,var_val_dbl_out,var_typ_rgr);
	  } /* end OpenMP critical */

	  if(dmn_id_in) dmn_id_in=(int *)nco_free(dmn_id_in);
	  if(dmn_id_out) dmn_id_out=(int *)nco_free(dmn_id_out);
	  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
	  if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
	  if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
	  if(var_val_dbl_in) var_val_dbl_in=(double *)nco_free(var_val_dbl_in);
	  if(var_val_dbl_out) var_val_dbl_out=(double *)nco_free(var_val_dbl_out);
	}else{ /* !trv.flg_rgr */
	  /* Use standard NCO copy routine for variables that are not regridded
	     20190511: Copy them only once */
	  if(tm_idx == 0){
#pragma omp critical
	    { /* begin OpenMP critical */
	      (void)nco_cpy_var_val(in_id,out_id,(FILE *)NULL,(md5_sct *)NULL,trv.nm,trv_tbl);
	    } /* end OpenMP critical */
	  } /* !tm_idx */
	} /* !flg_rgr */
      } /* !xtr */
    } /* end (OpenMP parallel for) loop over idx_tbl */
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"\n");
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s completion report: Variables interpolated = %d, copied unmodified = %d, omitted = %d, created = %d\n",nco_prg_nm_get(),fnc_nm,var_rgr_nbr,var_cpy_nbr,var_xcl_nbr,var_crt_nbr);

  } /* !tm_idx */

  if(att_nm_fll_val) att_nm_fll_val=(char *)nco_free(att_nm_fll_val);

  if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
  if(dmn_ids_in) dmn_ids_in=(int *)nco_free(dmn_ids_in);
  if(dmn_ids_out) dmn_ids_out=(int *)nco_free(dmn_ids_out);

  if(ilev_nm_in) ilev_nm_in=(char *)nco_free(ilev_nm_in);
  if(lev_nm_in) lev_nm_in=(char *)nco_free(lev_nm_in);

  if(hyai_in) hyai_in=(double *)nco_free(hyai_in);
  if(hyam_in) hyam_in=(double *)nco_free(hyam_in);
  if(hybi_in) hybi_in=(double *)nco_free(hybi_in);
  if(hybm_in) hybm_in=(double *)nco_free(hybm_in);
  if(ps_in) ps_in=(double *)nco_free(ps_in);
  if(prs_mdp_in) prs_mdp_in=(double *)nco_free(prs_mdp_in);
  if(prs_ntf_in) prs_ntf_in=(double *)nco_free(prs_ntf_in);

  if(hyai_out) hyai_out=(double *)nco_free(hyai_out);
  if(hyam_out) hyam_out=(double *)nco_free(hyam_out);
  if(hybi_out) hybi_out=(double *)nco_free(hybi_out);
  if(hybm_out) hybm_out=(double *)nco_free(hybm_out);
  if(ilev_out) ilev_out=(double *)nco_free(ilev_out);
  if(lev_in) lev_in=(double *)nco_free(lev_in);
  if(lev_out) lev_out=(double *)nco_free(lev_out);
  if(ps_out) ps_out=(double *)nco_free(ps_out);
  if(prs_mdp_out) prs_mdp_out=(double *)nco_free(prs_mdp_out);
  if(prs_ntf_out) prs_ntf_out=(double *)nco_free(prs_ntf_out);
  
  return rcd;
} /* !nco_ntp_vrt() */

int /* O [enm] Return code */
nco_rgr_wgt /* [fnc] Regrid with external weights */
(rgr_sct * const rgr, /* I/O [sct] Regridding structure */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal Table */
{
  /* Purpose: Regrid fields using external weights contained in a mapfile

     Examine ESMF, SCRIP, Tempest map-files:
     ncks --cdl -M -m ${DATA}/scrip/rmp_T42_to_POP43_conserv.nc | m
     ncks --cdl -M -m ${DATA}/maps/map_t42_to_fv129x256_aave.20150621.nc | m
     ncks --cdl -M -m ${DATA}/maps/map_ne30np4_to_ne120np4_tps.20150618.nc | m

     Test ESMF, SCRIP, Tempest map-files:
     ncks -D 5 -O --map=${DATA}/scrip/rmp_T42_to_POP43_conserv.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     ncks -D 5 -O --map=${DATA}/maps/map_t42_to_fv129x256_aave.20150621.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     ncks -D 5 -O --map=${DATA}/maps/map_ne30np4_to_ne120np4_tps.20150618.nc ${DATA}/ne30/rgr/ne30_1D.nc ~/foo.nc
 
     Mapfile formats ESMF, GRIDSPEC, SCRIP, and UGRID described here:
     http://www.earthsystemmodeling.org/esmf_releases/public/ESMF_6_3_0rp1/ESMF_refdoc/node3.html#sec:fileformat:scrip

     Conventions:
     grid_size: Number of gridcells (product of lat*lon)
     address: Source and destination index for each link pair
     num_links: Number of unique address pairs in remapping, i.e., size of sparse matrix
     num_wgts: Number of weights per vertice for given remapping (we only handle num_wgts == 1 below)
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

     Definition of sparse matrix formulations and normalization terminology, SCRIP manual p. 8, 13, 16:
     for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
       // Remap source function f = 1 in all unmasked source gridcells, zero elsewhere, to function F on destination grid
       // Normalization: fractional area (fracarea) (F = 1 where destination overlaps umasked source grid)
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0];
       // Normalization: destination area (destarea) (weights in each destination cell sum to its area frcation)
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0]/dst_area[ddr_dst[lnk_idx]];
       // Normalization: none (F = angular area that participates in remapping)
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0]/(dst_area[ddr_dst[lnk_idx]]*dst_frc[ddr_dst[lnk_idx]);
     } // end loop over lnk

     Documentation:
     NCL special cases described in popRemap.ncl, e.g., at
     https://github.com/yyr/ncl/blob/master/ni/src/examples/gsun/popRemap.ncl

     ESMF Regridding Status:
     https://www.earthsystemcog.org/projects/esmf

     Sample regrid T42->POP43, SCRIP:
     ncks -O --map=${DATA}/scrip/rmp_T42_to_POP43_conserv.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc */

  const char fnc_nm[]="nco_rgr_wgt()"; /* [sng] Function name */

  char *fl_in;
  char *fl_pth_lcl=NULL;

  const double rdn2dgr=180.0/M_PI;
  const double dgr2rdn=M_PI/180.0;
  const double eps_rlt=1.0e-14; /* [frc] Round-off error tolerance */

  double lat_wgt_ttl=0.0; /* [frc] Actual sum of quadrature weights */
  double area_out_ttl=0.0; /* [frc] Exact sum of area */

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int in_id; /* I [id] Input netCDF file ID */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;

  int dmn_idx; /* [idx] Dimension index */
  int dst_grid_corners_id; /* [id] Destination grid corners dimension ID */
  int dst_grid_rank_id; /* [id] Destination grid rank dimension ID */
  int dst_grid_size_id; /* [id] Destination grid size dimension ID */
  int num_links_id; /* [id] Number of links dimension ID */
  int num_wgts_id=NC_MIN_INT; /* [id] Number of weights dimension ID */
  int src_grid_corners_id; /* [id] Source grid corners dimension ID */
  int src_grid_rank_id; /* [id] Source grid rank dimension ID */
  int src_grid_size_id; /* [id] Source grid size dimension ID */

  long int lat_idx;
  long int lon_idx;

  short int bnd_idx;

  nco_bool FL_RTR_RMT_LCN;
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_OPEN=rgr->flg_uio; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool flg_dgn_area_out=False; /* [flg] Diagnose area_out from grid boundaries */
  nco_bool flg_bnd_1D_usable=False; /* [flg] Usable 1D cell vertices exist */
  nco_bool flg_stg=rgr->flg_stg; /* [flg] Write staggered grid with FV output */
  
  nco_grd_2D_typ_enm nco_grd_2D_typ=nco_grd_2D_nil; /* [enm] Two-dimensional grid-type enum */
  nco_grd_lat_typ_enm nco_grd_lat_typ=nco_grd_lat_nil; /* [enm] Latitude grid-type enum */
  nco_grd_lon_typ_enm nco_grd_lon_typ=nco_grd_lon_nil; /* [enm] Longitude grid-type enum */

  nco_mpf_sct mpf;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s obtaining mapping weights from %s\n",nco_prg_nm_get(),fnc_nm,rgr->fl_map);

  /* Duplicate (because nco_fl_mk_lcl() free()'s fl_in) */
  fl_in=(char *)strdup(rgr->fl_map);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  /* Identify mapping file type using string generated by weight-generator:
     ESMF: title = "ESMF Offline Regridding Weight Generator"
     ESMF_weight_only: title = "ESMF Regrid Weight Generator"
     NCO: Title = "netCDF Operators (NCO) Offline Regridding Weight Generator"
     SCRIP: conventions = "SCRIP"
     Tempest: Title = "TempestRemap Offline Regridding Weight Generator" */
  char *att_val;
  char *att_cnv_val=NULL;
  char *att_gnr_val=NULL;
  char *att_ttl_val=NULL;
  char *cnv_sng=NULL;
  /* netCDF standard is uppercase Conventions, though some models user lowercase */
  char att_sng_Cnv[]="Conventions"; /* [sng] Unidata standard     string (uppercase) */
  char att_sng_cnv[]="conventions"; /* [sng] Unidata non-standard string (lowercase) */
  char att_sng_gnr[]="weight_generator"; /* [sng] CMIP6 standard string */
  char att_sng_Ttl[]="Title"; /* [sng] NCO and Tempest use "Title" attribute, and Tempest does not use "Conventions" */
  char att_sng_ttl[]="title"; /* [sng] ERWG 7.1 weight_only uses "title" not "Conventions" attribute */
  char name0_sng[]="name0"; /* [sng] Attribute where Tempest stores least-rapidly-varying dimension name */
  
  nco_rgr_mpf_typ_enm nco_rgr_mpf_typ=nco_rgr_mpf_nil; /* [enm] Type of remapping file */
  nco_rgr_typ_enm nco_rgr_typ=nco_rgr_grd_nil; /* [enm] Type of grid conversion */
  
  /* Look for map-type signature in [cC]onventions or [tT]itle attribute */
  att_cnv_val=nco_char_att_get(in_id,NC_GLOBAL,att_sng_cnv);
  if(!att_cnv_val) att_cnv_val=nco_char_att_get(in_id,NC_GLOBAL,att_sng_Cnv);
  att_gnr_val=nco_char_att_get(in_id,NC_GLOBAL,att_sng_gnr);
  att_ttl_val=nco_char_att_get(in_id,NC_GLOBAL,att_sng_ttl);
  if(!att_ttl_val) att_ttl_val=nco_char_att_get(in_id,NC_GLOBAL,att_sng_Ttl);

  /* Either "[cC]onventions" or "[tT]itle" attribute determines map-file type... */
  if(att_cnv_val && strstr(att_cnv_val,"SCRIP")) nco_rgr_mpf_typ=nco_rgr_mpf_SCRIP;
  if(nco_rgr_mpf_typ == nco_rgr_mpf_nil && att_ttl_val){
    if(strstr(att_ttl_val,"ESMF Offline Regridding Weight Generator")) nco_rgr_mpf_typ=nco_rgr_mpf_ESMF;
    else if(strstr(att_ttl_val,"netCDF Operators")) nco_rgr_mpf_typ=nco_rgr_mpf_NCO;
    else if(strstr(att_ttl_val,"Tempest")) nco_rgr_mpf_typ=nco_rgr_mpf_Tempest;
    else if(strstr(att_ttl_val,"ESMF Regrid Weight Generator")) nco_rgr_mpf_typ=nco_rgr_mpf_ESMF_weight_only;
  } /* !att_ttl_val */
  if(nco_rgr_mpf_typ == nco_rgr_mpf_nil && att_cnv_val){
    if(strstr(att_cnv_val,"NCO")) nco_rgr_mpf_typ=nco_rgr_mpf_NCO;
  } /* !att_gnr_val */
  if(nco_rgr_mpf_typ == nco_rgr_mpf_nil && att_gnr_val){
    if(strstr(att_gnr_val,"NCO")) nco_rgr_mpf_typ=nco_rgr_mpf_NCO;
  } /* !att_gnr_val */
  if(nco_rgr_mpf_typ == nco_rgr_mpf_nil){
    (void)fprintf(stderr,"%s: WARNING %s unable to discern map-file type from global attributes \"[cC]onventions\" = \"%s\" and/or \"[tT]itle\" = \"%s\" and/or \"weight_generator\" = \"%s\"\n",nco_prg_nm_get(),fnc_nm,att_cnv_val ? att_cnv_val : "",att_ttl_val ? att_ttl_val : "",att_gnr_val ? att_gnr_val : "");
    nco_rgr_mpf_typ=nco_rgr_mpf_unknown;
  } /* !nco_rgr_mpf_typ */
  if(att_cnv_val) att_cnv_val=(char *)nco_free(att_cnv_val);
  if(att_gnr_val) att_gnr_val=(char *)nco_free(att_gnr_val);
  if(att_ttl_val) att_ttl_val=(char *)nco_free(att_ttl_val);

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
  case nco_rgr_mpf_ESMF_weight_only:
    rcd+=nco_inq_dimid(in_id,"n_s",&num_links_id);
    break;
  case nco_rgr_mpf_ESMF:
  case nco_rgr_mpf_NCO:
  case nco_rgr_mpf_Tempest:
  case nco_rgr_mpf_unknown:
    rcd+=nco_inq_dimid(in_id,"n_a",&src_grid_size_id);
    rcd+=nco_inq_dimid(in_id,"n_b",&dst_grid_size_id);
    rcd+=nco_inq_dimid(in_id,"nv_a",&src_grid_corners_id);
    rcd+=nco_inq_dimid(in_id,"nv_b",&dst_grid_corners_id);
    rcd+=nco_inq_dimid(in_id,"src_grid_rank",&src_grid_rank_id);
    rcd+=nco_inq_dimid(in_id,"dst_grid_rank",&dst_grid_rank_id);
    if(nco_rgr_mpf_typ != nco_rgr_mpf_Tempest){
      rcd+=nco_inq_dimid_flg(in_id,"num_wgts",&num_wgts_id);
      if(rcd != NC_NOERR){
	if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO %s reports map-file does not contain \"num_wgts\" dimension. ERWG always produces this as an orphan dimension, so post-processing could have removed it without harming other map-file fields. No harm, no foul.\n",nco_prg_nm_get(),fnc_nm);
	rcd=NC_NOERR;
      } /* !rcd */
    } /* !nco_rgr_mpf_Tempest */
    rcd+=nco_inq_dimid(in_id,"n_s",&num_links_id);
    break;
  default:
    (void)fprintf(stderr,"%s: ERROR %s (aka \"the regridder\") reports unknown map-file type\n",nco_prg_nm_get(),fnc_nm);
    nco_dfl_case_generic_err();
    /* NB: This return never executes because nco_dfl_case_generic_err() calls exit()
       Return placed here to suppress clang -Wsometimes-uninitialized warnings
       This is done many other times throughout the code, though explained only once, here */
    return NCO_ERR;
    break;
  } /* end switch */

  /* Use dimension IDs to get dimension sizes */
  rcd+=nco_inq_dimlen(in_id,num_links_id,&mpf.num_links);
  if(nco_rgr_mpf_typ != nco_rgr_mpf_ESMF_weight_only){
    rcd+=nco_inq_dimlen(in_id,src_grid_size_id,&mpf.src_grid_size);
    rcd+=nco_inq_dimlen(in_id,dst_grid_size_id,&mpf.dst_grid_size);
    rcd+=nco_inq_dimlen(in_id,src_grid_corners_id,&mpf.src_grid_corners);
    rcd+=nco_inq_dimlen(in_id,dst_grid_corners_id,&mpf.dst_grid_corners);
    rcd+=nco_inq_dimlen(in_id,src_grid_rank_id,&mpf.src_grid_rank);
    rcd+=nco_inq_dimlen(in_id,dst_grid_rank_id,&mpf.dst_grid_rank);
    /* TempestRemap does not generate num_wgts */
    if(nco_rgr_mpf_typ == nco_rgr_mpf_Tempest || num_wgts_id == NC_MIN_INT){
      mpf.num_wgts=int_CEWI;
    }else{
      rcd+=nco_inq_dimlen(in_id,num_wgts_id,&mpf.num_wgts);
    } /* !num_wgts_id */
    assert(mpf.src_grid_size < INT_MAX && mpf.dst_grid_size < INT_MAX);
  }else{
    mpf.src_grid_size=long_CEWI;
    mpf.dst_grid_size=long_CEWI;
    mpf.src_grid_corners=long_CEWI;
    mpf.dst_grid_corners=long_CEWI;
    mpf.src_grid_rank=long_CEWI;
    mpf.dst_grid_rank=long_CEWI;
    mpf.num_wgts=int_CEWI;
  } /* !ESMF_weight_only */
  
  cnv_sng=strdup("normalization");
  nco_rgr_nrm_typ_enm nco_rgr_nrm_typ=nco_rgr_nrm_nil;
  att_val=nco_char_att_get(in_id,NC_GLOBAL,cnv_sng);
  if(att_val){
    if(strstr(att_val,"fracarea")) nco_rgr_nrm_typ=nco_rgr_nrm_fracarea; /* 20190912: map_gx1v6T_to_1x1_bilin.nc and map_0.1T_tripole_to_0.1x0.1_bilin.nc store "fracarea" in normalization attribute. I think NCAR created both maps for POP, probably by running ERWG with option --norm_type=fracarea. Hence "fracarea" seems to be the NCAR-way of guaranteeing that ESMF re-normalization is not performed by default. */
    if(strstr(att_val,"destarea")) nco_rgr_nrm_typ=nco_rgr_nrm_destarea; /* ESMF conserve "aave" and bilinear "bilin" generate "destarea" by default */
    if(strstr(att_val,"none")) nco_rgr_nrm_typ=nco_rgr_nrm_none;
    if(att_val) att_val=(char *)nco_free(att_val);
  }else{
    /* 20150712: Tempest does not store a normalization attribute
       20170620: ESMF weight_only does not store a normalization attribute
       20190312: NCO does not yet store a normalization attribute */
    if(nco_rgr_mpf_typ == nco_rgr_mpf_Tempest || nco_rgr_mpf_typ == nco_rgr_mpf_NCO || nco_rgr_mpf_typ == nco_rgr_mpf_unknown || nco_rgr_mpf_typ == nco_rgr_mpf_ESMF_weight_only) nco_rgr_nrm_typ=nco_rgr_nrm_unknown;
  } /* endif normalization */
  assert(nco_rgr_nrm_typ != nco_rgr_nrm_nil);
  if(cnv_sng) cnv_sng=(char *)nco_free(cnv_sng);

  cnv_sng=strdup("map_method");
  nco_rgr_mth_typ_enm nco_rgr_mth_typ=nco_rgr_mth_nil;
  att_val=nco_char_att_get(in_id,NC_GLOBAL,cnv_sng);
  if(att_val){
    if(strcasestr(att_val,"Conservative")) nco_rgr_mth_typ=nco_rgr_mth_conservative;
    if(strcasestr(att_val,"Bilinear")) nco_rgr_mth_typ=nco_rgr_mth_bilinear;
    if(strcasestr(att_val,"none")) nco_rgr_mth_typ=nco_rgr_mth_none;
    if(att_val) att_val=(char *)nco_free(att_val);
  }else{
    /* Tempest does not store a map_method attribute */
    if(nco_rgr_mpf_typ == nco_rgr_mpf_NCO || nco_rgr_mpf_typ == nco_rgr_mpf_Tempest || nco_rgr_mpf_typ == nco_rgr_mpf_unknown) nco_rgr_mth_typ=nco_rgr_mth_unknown;
  } /* endif */
  if(nco_rgr_mth_typ == nco_rgr_mth_nil) (void)fprintf(stdout,"%s: WARNING %s reports map global attribute %s = %s does not match SCRIP/ESMF conventions that support only values of \"Conservative\" and \"Bilinear\" for this attribute. Proceeding anyway...\n",nco_prg_nm_get(),fnc_nm,cnv_sng,att_val);
  if(cnv_sng) cnv_sng=(char *)nco_free(cnv_sng);

  if(nco_dbg_lvl_get() >= nco_dbg_scl){
    (void)fprintf(stderr,"%s: INFO %s regridding input metadata and grid sizes: ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"mapfile_generator = %s, map_method = %s, normalization = %s, src_grid_size = n_a = %li, dst_grid_size = n_b = %li, src_grid_corners = nv_a = %li, dst_grid_corners = nv_b = %li, src_grid_rank = %li, dst_grid_rank = %li, num_links = n_s = %li, num_wgts = %li\n",nco_rgr_mpf_sng(nco_rgr_mpf_typ),nco_rgr_mth_sng(nco_rgr_mth_typ),nco_rgr_nrm_sng(nco_rgr_nrm_typ),mpf.src_grid_size,mpf.dst_grid_size,mpf.src_grid_corners,mpf.dst_grid_corners,mpf.src_grid_rank,mpf.dst_grid_rank,mpf.num_links,mpf.num_wgts);
  } /* endif dbg */
  /* 20190726: Allow normalization type to be "none" for bilinear regridding which UKMO SCRIP files set to "none"*/
  if(nco_rgr_mth_typ == nco_rgr_mth_conservative && nco_rgr_nrm_typ == nco_rgr_nrm_none){
    (void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") reports requested normalization type = %s is not yet supported. Specifically, masks specified by a mask variable (dst_grid_imask,mask_b) are ignored. More specifically, any destination mask information is assumed to be built into the weight array so that no source points will contribute to masked locations. Talk to Charlie if you want this changed.\n",nco_prg_nm_get(),fnc_nm,nco_rgr_nrm_sng(nco_rgr_nrm_typ));
    nco_exit(EXIT_FAILURE);
  } /* !msk */
    
  /* Got to here in bullet-proofing code for weight-only map-files */
  if(nco_rgr_mpf_typ == nco_rgr_mpf_ESMF_weight_only) (void)fprintf(stderr,"%s: WARNING %s reached end of ESMF_weight_only section\n",nco_prg_nm_get(),fnc_nm);
  assert(nco_rgr_mpf_typ != nco_rgr_mpf_ESMF_weight_only);
 
  /* Set type of grid conversion */
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

  /* Obtain grid values necessary to compute output latitude and longitude coordinates */
  int area_dst_id; /* [id] Area variable ID */
  int col_src_adr_id; /* [id] Source address (col) variable ID */
  int dmn_sz_in_int_id; /* [id] Source grid dimension sizes ID */
  int dmn_sz_out_int_id; /* [id] Destination grid dimension sizes ID */
  int dst_grd_crn_lat_id; /* [id] Destination grid corner latitudes  variable ID */
  int dst_grd_crn_lon_id; /* [id] Destination grid corner longitudes variable ID */
  int dst_grd_ctr_lat_id; /* [id] Destination grid center latitudes  variable ID */
  int dst_grd_ctr_lon_id; /* [id] Destination grid center longitudes variable ID */
  int frc_dst_id; /* [id] Fraction variable ID */
  int msk_dst_id=NC_MIN_INT; /* [id] Mask variable ID */
  int row_dst_adr_id; /* [id] Destination address (row) variable ID */
  int wgt_raw_id; /* [id] Remap matrix variable ID */

  switch(nco_rgr_mpf_typ){
    /* Obtain fields whose name depends on mapfile type */
  case nco_rgr_mpf_SCRIP:
    rcd+=nco_inq_varid(in_id,"dst_grid_area",&area_dst_id); /* ESMF: area_b */
    rcd+=nco_inq_varid(in_id,"dst_grid_center_lon",&dst_grd_ctr_lon_id); /* ESMF: xc_b */
    rcd+=nco_inq_varid(in_id,"dst_grid_center_lat",&dst_grd_ctr_lat_id); /* ESMF: yc_b */
    rcd+=nco_inq_varid(in_id,"dst_grid_corner_lon",&dst_grd_crn_lon_id); /* ESMF: xv_b */
    rcd+=nco_inq_varid(in_id,"dst_grid_corner_lat",&dst_grd_crn_lat_id); /* ESMF: yv_b */
    rcd+=nco_inq_varid(in_id,"dst_grid_frac",&frc_dst_id); /* ESMF: frac_b */
    rcd+=nco_inq_varid(in_id,"dst_address",&row_dst_adr_id); /* ESMF: row */
    rcd+=nco_inq_varid(in_id,"src_address",&col_src_adr_id); /* ESMF: col */
    rcd+=nco_inq_varid(in_id,"remap_matrix",&wgt_raw_id); /* NB: remap_matrix[num_links,num_wgts] != S[n_s] */
    break;
  case nco_rgr_mpf_ESMF:
  case nco_rgr_mpf_ESMF_weight_only:
  case nco_rgr_mpf_NCO:
  case nco_rgr_mpf_Tempest:
  case nco_rgr_mpf_unknown:
    if(nco_rgr_mpf_typ != nco_rgr_mpf_ESMF_weight_only){
      rcd+=nco_inq_varid(in_id,"area_b",&area_dst_id); /* SCRIP: dst_grid_area */
      rcd+=nco_inq_varid(in_id,"xc_b",&dst_grd_ctr_lon_id); /* SCRIP: dst_grid_center_lon */
      rcd+=nco_inq_varid(in_id,"yc_b",&dst_grd_ctr_lat_id); /* SCRIP: dst_grid_center_lat */
      rcd+=nco_inq_varid(in_id,"xv_b",&dst_grd_crn_lon_id); /* SCRIP: dst_grid_corner_lon */
      rcd+=nco_inq_varid(in_id,"yv_b",&dst_grd_crn_lat_id); /* SCRIP: dst_grid_corner_lat */
      rcd+=nco_inq_varid(in_id,"frac_b",&frc_dst_id); /* SCRIP: dst_grid_frac */
    } /* !nco_rgr_mpf_ESMF_weight_only */
    rcd+=nco_inq_varid(in_id,"row",&row_dst_adr_id); /* SCRIP: dst_address */
    rcd+=nco_inq_varid(in_id,"col",&col_src_adr_id); /* SCRIP: src_address */
    rcd+=nco_inq_varid(in_id,"S",&wgt_raw_id); /* NB: remap_matrix[num_links,num_wgts] != S[n_s] */
    break;
  default:
    (void)fprintf(stderr,"%s: ERROR %s (aka \"the regridder\") reports unknown map file type\n",nco_prg_nm_get(),fnc_nm);
    nco_dfl_case_generic_err();
    /* NB: This return never executes because nco_dfl_case_generic_err() calls exit()
       Return placed here to suppress clang -Wsometimes-uninitialized warnings
       This is done many other times throughout the code, though explained only once, here */
    return NCO_ERR;
    break;
  } /* end switch */

  /* Obtain fields whose presence depends on mapfile type */
  nco_bool flg_msk_out=rgr->flg_msk_out; /* [flg] Add mask to output */
  msk_dst_id=NC_MIN_INT;
  if(flg_msk_out){
    switch(nco_rgr_mpf_typ){
    case nco_rgr_mpf_ESMF:
    case nco_rgr_mpf_NCO:
      rcd+=nco_inq_varid(in_id,"mask_b",&msk_dst_id); /* SCRIP: dst_grid_imask */
      break;
    case nco_rgr_mpf_SCRIP:
      rcd+=nco_inq_varid(in_id,"dst_grid_imask",&msk_dst_id); /* ESMF: mask_b */
      break;
    case nco_rgr_mpf_Tempest:
    case nco_rgr_mpf_unknown:
      /* 20190315: TempestRemap did not propagate mask_b (or mask_a) until ~201902 */
      rcd+=nco_inq_varid_flg(in_id,"mask_b",&msk_dst_id);
      if(rcd == NC_ENOTVAR){
	(void)fprintf(stderr,"%s: INFO %s reports map-file lacks mask_b. %sContinuing anyway without masks...\n",nco_prg_nm_get(),fnc_nm,(nco_rgr_mpf_typ == nco_rgr_mpf_Tempest) ? "Probably this TempestRemap map-file was created before ~201902 when TR began to propagate mask_a/b variables." : "");
      } /* !rcd */
      rcd=NC_NOERR;
      break;
    default:
      (void)fprintf(stderr,"%s: ERROR %s (aka \"the regridder\") reports unknown map-file type\n",nco_prg_nm_get(),fnc_nm);
      nco_dfl_case_generic_err();
    } /* !nco_rgr_mpf_typ */
    if(msk_dst_id == NC_MIN_INT) flg_msk_out=False;
  } /* !flg_msk_out */
  /* Obtain fields whose names are independent of mapfile type */
  rcd+=nco_inq_varid(in_id,"src_grid_dims",&dmn_sz_in_int_id);
  rcd+=nco_inq_varid(in_id,"dst_grid_dims",&dmn_sz_out_int_id);

  int lon_psn_src; /* [idx] Ordinal position of longitude in rectangular source grid dimension-size array */
  int lat_psn_src; /* [idx] Ordinal position of latitude  in rectangular source grid dimension-size array */
  int lon_psn_dst=int_CEWI; /* [idx] Ordinal position of longitude in rectangular destination grid dimension-size array */
  int lat_psn_dst=int_CEWI; /* [idx] Ordinal position of latitude  in rectangular destination grid dimension-size array */
  if(flg_grd_in_2D){
    lon_psn_src=0; /* SCRIP introduced [lon,lat] convention because more natural for Fortran */
    lat_psn_src=1;
    if(nco_rgr_mpf_typ == nco_rgr_mpf_Tempest){
      /* Until 20150814, Tempest stored [src/dst]_grid_dims as [lat,lon] unlike SCRIP's [lon,lat] order
	 Newer behavior follows SCRIP [lon,lat] order
	 Challenge: Support both older and newer Tempest mapfiles
	 Tempest (unlike SCRIP and ESMF) annotates mapfile [src/dst]_grid_dims with attributes that identify axis to which each element of [src/dst]_grid_dims refers
	 Solution: Use Tempest mapfile [src/dst]_grid_dims attributes "name0" and/or "name1" to determine if axes' positions follow old order */
      att_val=nco_char_att_get(in_id,dmn_sz_in_int_id,name0_sng);
      if(att_val){
	if(strstr(att_val,"lat")){
	  lon_psn_src=1;
	  lat_psn_src=0;
	} /* !lat */
	if(att_val) att_val=(char *)nco_free(att_val);
      } /* end rcd && att_typ */
    } /* !Tempest */
  } /* !flg_grd_in_2D */
  if(flg_grd_out_2D){
    lon_psn_dst=0;
    lat_psn_dst=1;
    if(nco_rgr_mpf_typ == nco_rgr_mpf_Tempest){
      att_val=nco_char_att_get(in_id,dmn_sz_in_int_id,name0_sng);
      if(att_val){
	if(strstr(att_val,"lat")){
	  lon_psn_dst=1;
	  lat_psn_dst=0;
	} /* !lat */
	if(att_val) att_val=(char *)nco_free(att_val);
      } /* end rcd && att_typ */
    } /* !Tempest */
  } /* !flg_grd_out_2D */
  const int dmn_nbr_1D=1; /* [nbr] Rank of 1-D grid variables */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_3D=3; /* [nbr] Rank of 3-D grid variables */
  const int dmn_nbr_grd_max=dmn_nbr_3D; /* [nbr] Maximum rank of grid variables */
  double *area_out; /* [sr] Area of destination grid */
  double *frc_out=NULL; /* [frc] Fraction of destination grid */
  double *lat_bnd_out=NULL_CEWI; /* [dgr] Latitude  boundaries of rectangular destination grid */
  double *lat_crn_out=NULL; /* [dgr] Latitude  corners of rectangular destination grid */
  double *lat_ctr_out=NULL_CEWI; /* [dgr] Latitude  centers of rectangular destination grid */
  double *lat_ntf_out=NULL; /* [dgr] Latitude  interfaces of rectangular destination grid */
  double *lat_wgt_out=NULL; /* [dgr] Latitude  weights of rectangular destination grid */
  double *lon_bnd_out=NULL_CEWI; /* [dgr] Longitude boundaries of rectangular destination grid */
  double *lon_crn_out=NULL; /* [dgr] Longitude corners of rectangular destination grid */
  double *lon_ctr_out=NULL_CEWI; /* [dgr] Longitude centers of rectangular destination grid */
  double *lon_ntf_out=NULL; /* [dgr] Longitude interfaces of rectangular destination grid */
  double *slat_ctr_out=NULL_CEWI; /* [dgr] Latitude  centers of staggered FV destination grid */
  double *slat_wgt_out=NULL_CEWI; /* [frc] Latitude  weights of staggered FV destination grid */
  double *slon_ctr_out=NULL_CEWI; /* [dgr] Longitude centers of staggered FV destination grid */
  double *wgt_raw; /* [frc] Remapping weights */
  int *col_src_adr; /* [idx] Source address (col) */
  int *row_dst_adr; /* [idx] Destination address (row) */
  int *msk_out=NULL; /* [flg] Mask on destination grid */
  int *dmn_sz_in_int; /* [nbr] Array of dimension sizes of source grid */
  int *dmn_sz_out_int; /* [nbr] Array of dimension sizes of destination grid */
  long *dmn_cnt_in=NULL;
  long *dmn_cnt_out=NULL;
  long *dmn_cnt=NULL;
  long *dmn_srt=NULL;
  long *dmn_srd=NULL;
  long idx; /* [idx] Counting index for unrolled grids */
    
  /* Allocate space to hold dimension metadata for destination grid */
  dmn_srt=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));
  dmn_cnt=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));
  dmn_srd=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));

  dmn_srt[0]=0L;
  dmn_cnt[0]=mpf.src_grid_rank;
  dmn_sz_in_int=(int *)nco_malloc(mpf.src_grid_rank*nco_typ_lng((nc_type)NC_INT));
  rcd=nco_get_vara(in_id,dmn_sz_in_int_id,dmn_srt,dmn_cnt,dmn_sz_in_int,(nc_type)NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=mpf.dst_grid_rank;
  dmn_sz_out_int=(int *)nco_malloc(mpf.dst_grid_rank*nco_typ_lng((nc_type)NC_INT));
  rcd=nco_get_vara(in_id,dmn_sz_out_int_id,dmn_srt,dmn_cnt,dmn_sz_out_int,(nc_type)NC_INT);

  /* Check-for and workaround faulty Tempest and MPAS-O/I grid sizes */
  if(flg_grd_in_1D && (mpf.src_grid_size != dmn_sz_in_int[0])){
    (void)fprintf(stdout,"%s: INFO %s reports input grid dimension sizes disagree: mpf.src_grid_size = %ld != %d = dmn_sz_in[0]. Problem may be caused by incorrect src_grid_dims variable. This is a known issue with some TempestRemap mapfiles generated prior to ~20150901, and in some ESMF mapfiles for MPAS-O/I. This problem can be safely ignored if workaround succeeds. Attempting workaround ...\n",nco_prg_nm_get(),fnc_nm,mpf.src_grid_size,dmn_sz_in_int[0]);
      dmn_sz_in_int[0]=mpf.src_grid_size;
  } /* !bug */
  if(flg_grd_out_1D && (mpf.dst_grid_size != dmn_sz_out_int[0])){
    (void)fprintf(stdout,"%s: INFO %s reports output grid dimension sizes disagree: mpf.dst_grid_size = %ld != %d = dmn_sz_out[0]. Problem may be caused by incorrect dst_grid_dims variable. This is a known issue with some TempestRemap mapfiles generated prior to ~20150901, and in some ESMF mapfiles for MPAS-O/I. This problem can be safely ignored if workaround succeeds. Attempting workaround ...\n",nco_prg_nm_get(),fnc_nm,mpf.dst_grid_size,dmn_sz_out_int[0]);
    dmn_sz_out_int[0]=mpf.dst_grid_size;
  } /* !bug */
 
  long col_nbr_in; /* [idx] Number of columns in source grid */
  long lon_nbr_in; /* [idx] Number of longitudes in rectangular source grid */
  long lat_nbr_in; /* [idx] Number of latitudes  in rectangular source grid */
  const size_t grd_sz_in=mpf.src_grid_size; /* [nbr] Number of elements in single layer of input grid */
  const size_t grd_sz_out=mpf.dst_grid_size; /* [nbr] Number of elements in single layer of output grid */
  if(flg_grd_in_1D){
    col_nbr_in=dmn_sz_in_int[0];
    lon_nbr_in=dmn_sz_in_int[0];
    lat_nbr_in=dmn_sz_in_int[0];
  }else if(flg_grd_in_2D){
    col_nbr_in=0;
    lon_nbr_in=dmn_sz_in_int[lon_psn_src];
    lat_nbr_in=dmn_sz_in_int[lat_psn_src];
    /* Sanity-check */
    assert(lat_nbr_in*lon_nbr_in == (long)grd_sz_in);
  } /* !src_grid_rank */

  const int bnd_tm_nbr_out=2; /* [nbr] Number of boundaries for output time */
  int bnd_nbr_out=int_CEWI; /* [nbr] Number of boundaries for output time and rectangular grid coordinates, and number of vertices for output non-rectangular grid coordinates */
  long col_nbr_out=long_CEWI; /* [nbr] Number of columns in destination grid */
  long lon_nbr_out=long_CEWI; /* [nbr] Number of longitudes in rectangular destination grid */
  long lat_nbr_out=long_CEWI; /* [nbr] Number of latitudes  in rectangular destination grid */
  long slat_nbr_out=long_CEWI; /* [nbr] Number of latitudes in staggered FV grid destination grid */
  long slon_nbr_out=long_CEWI; /* [nbr] Number of longitudes in staggered FV grid destination grid */
  if(flg_grd_out_1D){
    bnd_nbr_out=mpf.dst_grid_corners;
    col_nbr_out=dmn_sz_out_int[0];
    lat_nbr_out=dmn_sz_out_int[0];
    lon_nbr_out=dmn_sz_out_int[0];
    /* Sanity-check */
    assert(col_nbr_out == (long)grd_sz_out);
  }else if(flg_grd_out_2D){
    col_nbr_out=lat_nbr_out*lon_nbr_out;
    lat_nbr_out=dmn_sz_out_int[lat_psn_dst];
    lon_nbr_out=dmn_sz_out_int[lon_psn_dst];
    slat_nbr_out=lat_nbr_out-1L;
    slon_nbr_out=lon_nbr_out;
    /* Sanity-check */
    assert(lat_nbr_out*lon_nbr_out == (long)grd_sz_out);
  } /* !dst_grid_rank */

  /* Ensure coordinates are in degrees not radians for simplicity and CF-compliance
     NB: ${DATA}/scrip/rmp_T42_to_POP43_conserv.nc has [xy]?_a in degrees and [xy]?_b in radians! */
  nco_bool flg_crd_rdn=False; /* [flg] Destination coordinates are in radians not degrees */
  char unt_sng[]="units"; /* [sng] netCDF-standard units attribute name */
  att_val=nco_char_att_get(in_id,dst_grd_ctr_lat_id,unt_sng);
  if(att_val){
    /* Match "radian" and "radians" */
    if(strstr(att_val,"radian")) flg_crd_rdn=True;
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* end rcd && att_typ */

  nco_bool flg_grd_out_crv=False; /* [flg] Curvilinear coordinates */
  nco_bool flg_grd_out_rct=False; /* [flg] Rectangular coordinates */
  const nc_type crd_typ_out=NC_DOUBLE;
  if(flg_grd_out_2D){
    lon_ctr_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ_out));
    lat_ctr_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ_out));
    lon_crn_out=(double *)nco_malloc(mpf.dst_grid_corners*grd_sz_out*nco_typ_lng(crd_typ_out));
    lat_crn_out=(double *)nco_malloc(mpf.dst_grid_corners*grd_sz_out*nco_typ_lng(crd_typ_out));

    dmn_srt[0]=0L;
    dmn_cnt[0]=grd_sz_out;
    rcd=nco_get_vara(in_id,dst_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_out,crd_typ_out);
    rcd=nco_get_vara(in_id,dst_grd_ctr_lat_id,dmn_srt,dmn_cnt,lat_ctr_out,crd_typ_out);
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=grd_sz_out;
    dmn_cnt[1]=mpf.dst_grid_corners;
    rcd=nco_get_vara(in_id,dst_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn_out,crd_typ_out);
    rcd=nco_get_vara(in_id,dst_grd_crn_lat_id,dmn_srt,dmn_cnt,lat_crn_out,crd_typ_out);

    /* User may specify curvilinear grid (with --rgr crv). Otherwise, manually test for curvilinear source grid. */
    flg_grd_out_crv=rgr->flg_crv; /* [flg] Curvilinear coordinates */
    if(flg_grd_out_crv){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Output grid specified to be %s\n",nco_prg_nm_get(),flg_grd_out_crv ? "Curvilinear" : "Rectangular");
    }else{
      long idx_tst=long_CEWI; /* [idx] Index of first latitude or longitude */
      for(idx=0;idx<(long)grd_sz_out;idx++){
	if(idx%lon_nbr_out == 0) idx_tst=idx;
	if(lat_ctr_out[idx] != lat_ctr_out[idx_tst]) break;
	// (void)fprintf(stdout,"%s: DEBUG lat_ctr_out[%li] = %g, lat_ctr_out[%li] = %g\n",nco_prg_nm_get(),idx,lat_ctr_out[idx],idx_tst,lat_ctr_out[idx_tst]);
	/* fxm: also test lon */
      } /* !rectangular */
      if(idx != (long)grd_sz_out) flg_grd_out_crv=True; else flg_grd_out_rct=True;
      if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO Output grid detected to be %s\n",nco_prg_nm_get(),flg_grd_out_crv ? "Curvilinear" : "Rectangular");
    } /* !flg_grd_out_crv */

    if(flg_grd_out_crv) bnd_nbr_out=mpf.dst_grid_corners;
    if(flg_grd_out_rct) bnd_nbr_out=2; /* NB: Assumes rectangular latitude and longitude and is invalid for other quadrilaterals */
  } /* !flg_grd_out_2D */

  if(nco_dbg_lvl_get() >= nco_dbg_scl){
    (void)fprintf(stderr,"%s: INFO %s grid conversion type = %s with expected input and prescribed output grid sizes: ",nco_prg_nm_get(),fnc_nm,nco_rgr_grd_sng(nco_rgr_typ));
    (void)fprintf(stderr,"lat_in = %li, lon_in = %li, col_in = %li, lat_out = %li, lon_out = %li, col_out = %li\n",lat_nbr_in,lon_nbr_in,col_nbr_in,lat_nbr_out,lon_nbr_out,col_nbr_out);
  } /* endif dbg */

  /* Allocate space for and obtain coordinates */
  if(flg_grd_out_1D){
    lon_ctr_out=(double *)nco_malloc(col_nbr_out*nco_typ_lng(crd_typ_out));
    lat_ctr_out=(double *)nco_malloc(col_nbr_out*nco_typ_lng(crd_typ_out));
    lon_bnd_out=(double *)nco_malloc(col_nbr_out*bnd_nbr_out*nco_typ_lng(crd_typ_out));
    lat_bnd_out=(double *)nco_malloc(col_nbr_out*bnd_nbr_out*nco_typ_lng(crd_typ_out));
  } /* !flg_grd_out_1D */

  if(flg_grd_out_rct){
    if(lat_ctr_out) lat_ctr_out=(double *)nco_free(lat_ctr_out);
    if(lon_ctr_out) lon_ctr_out=(double *)nco_free(lon_ctr_out);
    if(lat_crn_out) lat_crn_out=(double *)nco_free(lat_crn_out);
    if(lon_crn_out) lon_crn_out=(double *)nco_free(lon_crn_out);
    lon_ctr_out=(double *)nco_malloc(lon_nbr_out*nco_typ_lng(crd_typ_out));
    lat_ctr_out=(double *)nco_malloc(lat_nbr_out*nco_typ_lng(crd_typ_out));
    lon_crn_out=(double *)nco_malloc(mpf.dst_grid_corners*lon_nbr_out*nco_typ_lng(crd_typ_out));
    lat_crn_out=(double *)nco_malloc(mpf.dst_grid_corners*lat_nbr_out*nco_typ_lng(crd_typ_out));
    lat_wgt_out=(double *)nco_malloc(lat_nbr_out*nco_typ_lng(crd_typ_out));
    lon_ntf_out=(double *)nco_malloc((lon_nbr_out+1L)*nco_typ_lng(crd_typ_out));
    lat_ntf_out=(double *)nco_malloc((lat_nbr_out+1L)*nco_typ_lng(crd_typ_out));
    lon_bnd_out=(double *)nco_malloc(lon_nbr_out*bnd_nbr_out*nco_typ_lng(crd_typ_out));
    lat_bnd_out=(double *)nco_malloc(lat_nbr_out*bnd_nbr_out*nco_typ_lng(crd_typ_out));
  } /* !flg_grd_out_rct */

  /* Arrays unroll into all longitudes for first latitude, then second latitude, ...
     Obtain longitudes by reading first block contiguously (unstrided)
     Obtain latitudes by reading unrolled data with stride of lon_nbr */
  if(flg_grd_out_1D){
    dmn_srt[0]=0L;
    dmn_cnt[0]=col_nbr_out;
    rcd=nco_get_vara(in_id,dst_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_out,crd_typ_out);
    dmn_srt[0]=0L;
    dmn_cnt[0]=col_nbr_out;
    rcd=nco_get_vara(in_id,dst_grd_ctr_lat_id,dmn_srt,dmn_cnt,lat_ctr_out,crd_typ_out);
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=col_nbr_out;
    dmn_cnt[1]=bnd_nbr_out;
    rcd=nco_get_vara(in_id,dst_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_bnd_out,crd_typ_out);
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=col_nbr_out;
    dmn_cnt[1]=bnd_nbr_out;
    rcd=nco_get_vara(in_id,dst_grd_crn_lat_id,dmn_srt,dmn_cnt,lat_bnd_out,crd_typ_out);
    if(flg_crd_rdn){
      for(idx=0;idx<col_nbr_out;idx++){
	lon_ctr_out[idx]*=rdn2dgr;
	lat_ctr_out[idx]*=rdn2dgr;
      } /* !idx */
      for(idx=0;idx<col_nbr_out*bnd_nbr_out;idx++){
	lon_bnd_out[idx]*=rdn2dgr;
	lat_bnd_out[idx]*=rdn2dgr;
      } /* !idx */
    } /* !rdn */
    /* Is 1D interface information usable? Yes, unless if all interfaces are zeros
       NB: fxm Better algorithm for "usable" is that not all interfaces in any cell are equal */
    flg_bnd_1D_usable=True;
    for(idx=0;idx<col_nbr_out*bnd_nbr_out;idx++)
      if(lon_bnd_out[idx] != 0.0) break;
    if(idx == col_nbr_out*bnd_nbr_out){
      flg_bnd_1D_usable=False;
    }else{
      for(idx=0;idx<col_nbr_out*bnd_nbr_out;idx++)
	if(lat_bnd_out[idx] != 0.0) break;
      if(idx == col_nbr_out*bnd_nbr_out) flg_bnd_1D_usable=False;
    } /* !usable */
      
    if(nco_dbg_lvl_get() >= nco_dbg_crr){
      for(idx=0;idx<lat_nbr_out;idx++){
	(void)fprintf(stdout,"lat[%li] = %g, vertices = ",idx,lat_ctr_out[idx]);
	for(bnd_idx=0;bnd_idx<bnd_nbr_out;bnd_idx++)
	  (void)fprintf(stdout,"%s%g%s",bnd_idx == 0 ? "[" : "",lat_bnd_out[bnd_nbr_out*idx+bnd_idx],bnd_idx == bnd_nbr_out-1 ? "]\n" : ", ");
      } /* end loop over lat */
      for(idx=0;idx<lon_nbr_out;idx++){
	(void)fprintf(stdout,"lon[%li] = %g, vertices = ",idx,lon_ctr_out[idx]);
	for(bnd_idx=0;bnd_idx<bnd_nbr_out;bnd_idx++)
	  (void)fprintf(stdout,"%s%g%s",bnd_idx == 0 ? "[" : "",lon_bnd_out[bnd_nbr_out*idx+bnd_idx],bnd_idx == bnd_nbr_out-1 ? "]\n" : ", ");
      } /* end loop over lon */
    } /* endif dbg */
  } /* !flg_grd_out_1D */

  if(flg_grd_out_rct){
    /* fxm: sub-sample these from the already-read ctr/crn arrays */
    dmn_srt[0L]=0L;
    dmn_cnt[0L]=lon_nbr_out;
    rcd=nco_get_vara(in_id,dst_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_out,crd_typ_out);
    dmn_srt[0L]=0L;
    dmn_cnt[0L]=lat_nbr_out;
    dmn_srd[0L]=lon_nbr_out;
    rcd=nco_get_vars(in_id,dst_grd_ctr_lat_id,dmn_srt,dmn_cnt,dmn_srd,lat_ctr_out,crd_typ_out);
    dmn_srt[0L]=dmn_srt[1]=0L;
    dmn_cnt[0L]=lon_nbr_out;
    dmn_cnt[1]=mpf.dst_grid_corners;
    rcd=nco_get_vara(in_id,dst_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn_out,crd_typ_out);
    dmn_srt[0L]=0L;
    dmn_cnt[0L]=lat_nbr_out;
    dmn_srd[0L]=lon_nbr_out;
    dmn_srt[1]=0L;
    dmn_cnt[1]=mpf.dst_grid_corners;
    dmn_srd[1]=1L;
    rcd=nco_get_vars(in_id,dst_grd_crn_lat_id,dmn_srt,dmn_cnt,dmn_srd,lat_crn_out,crd_typ_out);
    if(flg_crd_rdn){
      for(idx=0L;idx<lon_nbr_out;idx++) lon_ctr_out[idx]*=rdn2dgr;
      for(idx=0L;idx<lat_nbr_out;idx++) lat_ctr_out[idx]*=rdn2dgr;
      for(idx=0L;idx<lon_nbr_out*mpf.dst_grid_corners;idx++) lon_crn_out[idx]*=rdn2dgr;
      for(idx=0L;idx<lat_nbr_out*mpf.dst_grid_corners;idx++) lat_crn_out[idx]*=rdn2dgr;
    } /* !rdn */
  } /* !flg_grd_out_rct */
    
  if(flg_grd_out_crv){
    if(flg_crd_rdn){
      for(idx=0L;idx<(long)grd_sz_out;idx++) lon_ctr_out[idx]*=rdn2dgr;
      for(idx=0L;idx<(long)grd_sz_out;idx++) lat_ctr_out[idx]*=rdn2dgr;
      for(idx=0L;idx<(long)grd_sz_out*mpf.dst_grid_corners;idx++) lon_crn_out[idx]*=rdn2dgr;
      for(idx=0L;idx<(long)grd_sz_out*mpf.dst_grid_corners;idx++) lat_crn_out[idx]*=rdn2dgr;
    } /* !rdn */
  } /* !flg_grd_out_crv */

  /* Allocate space for and obtain area, fraction, and mask, which are needed for both 1D and 2D grids */
  area_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ_out));
  dmn_srt[0L]=0L;
  dmn_cnt[0L]=grd_sz_out;
  rcd=nco_get_vara(in_id,area_dst_id,dmn_srt,dmn_cnt,area_out,crd_typ_out);

  frc_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ_out));
  dmn_srt[0L]=0L;
  dmn_cnt[0L]=grd_sz_out;
  rcd=nco_get_vara(in_id,frc_dst_id,dmn_srt,dmn_cnt,frc_out,crd_typ_out);

  if(msk_dst_id != NC_MIN_INT){
    msk_out=(int *)nco_malloc(grd_sz_out*nco_typ_lng(NC_INT));
    dmn_srt[0L]=0L;
    dmn_cnt[0L]=grd_sz_out;
    rcd=nco_get_vara(in_id,msk_dst_id,dmn_srt,dmn_cnt,msk_out,(nc_type)NC_INT);
  } /* !msk */
  
  /* Derive 2D interface boundaries from lat and lon grid-center values
     NB: Procedures to derive interfaces from midpoints on rectangular grids are theoretically possible 
     However, ESMF often outputs interfaces values (e.g., yv_b) for midpoint coordinates (e.g., yc_b)
     For example, ACME standard map from ne120np4 to 181x360 has yc_b[0] = yv_b[0] = -90.0
     Latitude = -90 is, by definition, not a midpoint coordinate
     This appears to be an artifact of the non-physical representation of the FV grid, i.e.,
     a grid center located at the pole where longitudes collapse in the model, but cannot be
     represented as collapsed on a rectangular 2D grid with non-zero areas.
     Unfortunately, ESMF supports this nonsense by labeling the grid center as at the pole
     so that applications can easily diagnose an FV grid when they read-in datasets.
     A superior application could diagnose FV just fine from actual non-polar gridcell centers
     Maybe ESMF could introduce a flag or something to indicate/avoid this special case?
     Safer to read boundary interfaces directly from grid corner/vertice arrays in map file
     
     Derivation of boundaries xv_b, yv_b from _correct_ xc_b, yc_b is follows
     Do not implement this procedure until resolving midpoint/center issue described above:
     lon_ntf_out[0L]=0.5*(lon_ctr_out[0L]+lon_ctr_out[lon_nbr_out-1L])-180.0; // Extrapolation
     lat_ntf_out[0L]=lat_ctr_out[0L]-0.5*(lat_ctr_out[1L]-lat_ctr_out[0L]); // Extrapolation
     for(idx=1L;idx<lon_nbr_out;idx++) lon_ntf_out[idx]=0.5*(lon_ctr_out[idx-1L]+lon_ctr_out[idx]);
     for(idx=1L;idx<lat_nbr_out;idx++) lat_ntf_out[idx]=0.5*(lat_ctr_out[idx-1L]+lat_ctr_out[idx]);
     lon_ntf_out[lon_nbr_out]=lon_ntf_out[0L]+360.0;
     lat_ntf_out[lat_nbr_out]=lat_ctr_out[lat_nbr_out-1L]+0.5*(lat_ctr_out[lat_nbr_out-1L]-lat_ctr_out[lat_nbr_out-2L]); */

  if(flg_grd_out_rct){
    double lon_spn; /* [dgr] Longitude span */
    double lat_spn; /* [dgr] Latitude span */
    nco_bool flg_s2n=True; /* I [enm] Latitude grid-direction is South-to-North */
    if(lat_ctr_out[1L] < lat_ctr_out[0L]) flg_s2n=False;

    /* Obtain 1-D rectangular interfaces from unrolled 1-D vertice arrays */
    for(idx=0L;idx<lon_nbr_out;idx++) lon_ntf_out[idx]=lon_crn_out[mpf.dst_grid_corners*idx];
    /* 20201009 
       The four possible CCW RLL orderings start with the ul, ll, lr, or ur vertice
       NCO grid generators store vertices in order (0,1,2,3)=(ul,ll,lr,ur)
       NCO final latitude is in upper vertices (0,3) for S2N grids, lower vertices (1,2) for N2S grids
       NCO final longitude is in RHS vertices (2,3) for S2N and N2S grids
       Need generic algorithm to pick easternmost longitude for any of the four CCW orderings
       What is ESMF vertice ordering? or does ESMF always copy from input grid? 
       Most grid generators probably start with ul or ll so vertice 2 is good choice for easternmost */
    // lon_ntf_out[lon_nbr_out]=lon_crn_out[mpf.dst_grid_corners*lon_nbr_out-(mpf.dst_grid_corners-1L)]; // ESMF?
    lon_ntf_out[lon_nbr_out]=lon_crn_out[mpf.dst_grid_corners*lon_nbr_out-2L]; // NCO lr
    if(lon_ntf_out[lon_nbr_out-1] == lon_ntf_out[lon_nbr_out]) lon_ntf_out[lon_nbr_out]=lon_crn_out[mpf.dst_grid_corners*lon_nbr_out-1L]; // NCO ur
    if(lon_ntf_out[lon_nbr_out-1] == lon_ntf_out[lon_nbr_out]) lon_ntf_out[lon_nbr_out]=lon_crn_out[mpf.dst_grid_corners*lon_nbr_out-3L]; // NCO ll
    assert(lon_ntf_out[lon_nbr_out-1] != lon_ntf_out[lon_nbr_out]);
    lon_spn=lon_ntf_out[lon_nbr_out]-lon_ntf_out[0L];
    for(idx=0L;idx<lat_nbr_out;idx++) lat_ntf_out[idx]=lat_crn_out[mpf.dst_grid_corners*idx];
    if(flg_s2n) lat_ntf_out[lat_nbr_out]=max_dbl(lat_crn_out[mpf.dst_grid_corners*lat_nbr_out-1L],lat_crn_out[mpf.dst_grid_corners*lat_nbr_out-2L]); else lat_ntf_out[lat_nbr_out]=min_dbl(lat_crn_out[mpf.dst_grid_corners*lat_nbr_out-1L],lat_crn_out[mpf.dst_grid_corners*lat_nbr_out-2L]);
    assert(lat_ntf_out[lat_nbr_out] != lat_ntf_out[lat_nbr_out-1]);
    lat_spn=fabs(lat_ntf_out[lat_nbr_out]-lat_ntf_out[0L]);

    /* Place 1-D rectangular interfaces into 2-D coordinate boundaries */
    for(idx=0L;idx<lon_nbr_out;idx++){
      lon_bnd_out[2L*idx]=lon_ntf_out[idx];
      lon_bnd_out[2L*idx+1L]=lon_ntf_out[idx+1L];
    } /* !lon_nbr_out */
    for(idx=0L;idx<lat_nbr_out;idx++){
      lat_bnd_out[2L*idx]=lat_ntf_out[idx];
      lat_bnd_out[2L*idx+1L]=lat_ntf_out[idx+1L];
    } /* !lat_nbr_out */

    if(nco_dbg_lvl_get() >= nco_dbg_crr){
      for(idx=0L;idx<lon_nbr_out;idx++) (void)fprintf(stdout,"lon[%li] = [%g, %g, %g]\n",idx,lon_bnd_out[2L*idx],lon_ctr_out[idx],lon_bnd_out[2L*idx+1L]);
      for(idx=0L;idx<lat_nbr_out;idx++) (void)fprintf(stdout,"lat[%li] = [%g, %g, %g]\n",idx,lat_bnd_out[2L*idx],lat_ctr_out[idx],lat_bnd_out[2L*idx+1L]);
    } /* endif dbg */

    /* Global or regional grid? */
    nco_grd_xtn_enm nco_grd_xtn; /* [enm] Extent of grid */
    if((float)lon_spn == 360.0f && (float)lat_spn == 180.0f) nco_grd_xtn=nco_grd_xtn_glb; else nco_grd_xtn=nco_grd_xtn_rgn;
    /* Diagnose type of latitude output grid by testing second latitude center against formulae */
    double lat_ctr_tst_eqa;
    double lat_ctr_tst_fv;
    if(flg_s2n) lat_ctr_tst_eqa=lat_ntf_out[0L]+lat_spn*1.5/lat_nbr_out; else lat_ctr_tst_eqa=lat_ntf_out[0L]-lat_spn*1.5/lat_nbr_out;
    if(flg_s2n) lat_ctr_tst_fv=lat_ntf_out[0L]+lat_spn/(lat_nbr_out-1L); else lat_ctr_tst_fv=lat_ntf_out[0L]-lat_spn/(lat_nbr_out-1L);
    double lat_ctr_tst_gss;
    /* In diagnosing grids, agreement to slightly worse than single-precision is "good enough for government work"
       Hence some comparisons cast from double to float before comparison
       20150526: T42 grid from SCRIP and related maps, and NCL-generated Gaussian grids for CESM, are accurate to at most ~eight digits
       20150611: map_ne120np4_to_fv801x1600_bilin.150418.nc has yc_b[1600]=-89.775000006 not expected exact value lat_ctr[1]=-89.775000000000006
       20170521: T62 grid from NCEP-NCAR Reanalysis 1 is worse than single precision, has yc_[192]=-86.6531 not expected exact value lat_ctr[1]=-86.6532 */

    if((float)lat_ctr_out[1L] == (float)lat_ctr_tst_eqa) nco_grd_lat_typ=nco_grd_lat_eqa;
    if((float)lat_ctr_out[1L] == (float)lat_ctr_tst_fv) nco_grd_lat_typ=nco_grd_lat_fv;
    double *wgt_Gss_out=NULL; // [frc] Gaussian weights double precision
    if(nco_grd_lat_typ == nco_grd_lat_nil){
      /* Check for Gaussian grid */
      double *lat_sin_out; // [frc] Sine of Gaussian latitudes double precision
      lat_sin_out=(double *)nco_malloc(lat_nbr_out*sizeof(double));
      wgt_Gss_out=(double *)nco_malloc(lat_nbr_out*sizeof(double));
      (void)nco_lat_wgt_gss(lat_nbr_out,flg_s2n,lat_sin_out,wgt_Gss_out);
      lat_ctr_tst_gss=rdn2dgr*asin(lat_sin_out[1L]);
      /* Gaussian weights on output grid will be double-precision accurate
	 Grid itself is kept as user-specified so area diagnosed by ESMF_RegridWeightGen may be slightly inconsistent with weights */
      if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stderr,"%s: INFO %s reports lat_ctr_out[1] = %g, lat_ctr_tst_gss = %g\n",nco_prg_nm_get(),fnc_nm,lat_ctr_out[1L],lat_ctr_tst_gss);
      if((float)lat_ctr_out[1L] == (float)lat_ctr_tst_gss) nco_grd_lat_typ=nco_grd_lat_gss;
      if(lat_sin_out) lat_sin_out=(double *)nco_free(lat_sin_out);
    } /* !Gaussian */
    if(nco_grd_lat_typ == nco_grd_lat_nil){
      /* If still of unknown type, this 2D grid may be weird
	 This occurs, e.g., with POP3 destination grid
	 Change gridtype from nil (which means not-yet-set) to unknown (which means none of the others matched) */
      nco_grd_lat_typ=nco_grd_lat_unk;
    } /* !nil */

    /* Currently grd_lat_typ and grd_2D_typ are equivalent, though that may be relaxed in future */
    if(nco_grd_lat_typ == nco_grd_lat_unk) nco_grd_2D_typ=nco_grd_2D_unk;
    else if(nco_grd_lat_typ == nco_grd_lat_gss) nco_grd_2D_typ=nco_grd_2D_gss;
    else if(nco_grd_lat_typ == nco_grd_lat_fv) nco_grd_2D_typ=nco_grd_2D_fv;
    else if(nco_grd_lat_typ == nco_grd_lat_eqa) nco_grd_2D_typ=nco_grd_2D_eqa;
    else assert(False);

    if(nco_grd_lon_typ == nco_grd_lon_nil){
      /* NB: Longitude grid diagnosis is susceptible to mistakes when input mapfile embeds common faulty grids, e.g., ACME *150418* FV maps
	 map_ne30np4_to_fv129x256_aave.150418.nc is diagnosed as regional grid of unknown type because of input grid flaws
	 map_ne30np4_to_fv129x256_aave.20150901.nc is (correctly) diagnosed as global grid of with lon_Grn_ctr */
      if(     (float)lon_ctr_out[0L] ==    0.0f && (float)lon_ctr_out[1L] == (float)(lon_ctr_out[0L]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_Grn_ctr;
      else if((float)lon_ctr_out[0L] == -180.0f && (float)lon_ctr_out[1L] == (float)(lon_ctr_out[0L]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_180_ctr;
      else if((float)lon_ntf_out[0L] ==    0.0f && (float)lon_ntf_out[1L] == (float)(lon_ntf_out[0L]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_Grn_wst;
      else if((float)lon_ntf_out[0L] == -180.0f && (float)lon_ntf_out[1L] == (float)(lon_ntf_out[0L]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_180_wst;
      else if((float)lon_ctr_out[1L] == (float)(lon_ctr_out[0L]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_bb;
      else nco_grd_lon_typ=nco_grd_lon_unk;
    } /* !nco_grd_lon_typ */

    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO %s diagnosed output latitude grid-type: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_lat_sng(nco_grd_lat_typ));
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO %s diagnosed output longitude grid-type: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_lon_sng(nco_grd_lon_typ));
    if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stderr,"%s: INFO %s diagnosed output grid-extent: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_xtn_sng(nco_grd_xtn));
    
    if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
      slat_ctr_out=(double *)nco_malloc(slat_nbr_out*nco_typ_lng(crd_typ_out));
      slat_wgt_out=(double *)nco_malloc(slat_nbr_out*nco_typ_lng(crd_typ_out));
      slon_ctr_out=(double *)nco_malloc(slon_nbr_out*nco_typ_lng(crd_typ_out));
      for(idx=0L;idx<slat_nbr_out;idx++){
	slat_ctr_out[idx]=lat_ntf_out[idx+1L];
	slat_wgt_out[idx]=fabs(sin(dgr2rdn*lat_ctr_out[idx+1L])-sin(dgr2rdn*lat_ctr_out[idx])); /* fabs() ensures positive area in n2s grids */
      } /* !lat_nbr_out */
      for(idx=0L;idx<slon_nbr_out;idx++){
	slon_ctr_out[idx]=lon_ntf_out[idx];
      } /* !lat_nbr_out */
    } /* !nco_grd_lat_fv */

    switch(nco_grd_lat_typ){
    case nco_grd_lat_eqa:
    case nco_grd_lat_fv:
      for(idx=0L;idx<lat_nbr_out;idx++) lat_wgt_out[idx]=fabs(sin(dgr2rdn*lat_bnd_out[2*idx+1L])-sin(dgr2rdn*lat_bnd_out[2*idx])); /* fabs() ensures positive area in n2s grids */
      break;
    case nco_grd_lat_gss:
      for(idx=0L;idx<lat_nbr_out;idx++) lat_wgt_out[idx]=wgt_Gss_out[idx];
      if(wgt_Gss_out) wgt_Gss_out=(double *)nco_free(wgt_Gss_out);
      break;
    case nco_grd_lat_unk:
      for(idx=0L;idx<lat_nbr_out;idx++) lat_wgt_out[idx]=0.0;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING %s reports unknown output latitude grid-type. Unable to guess what latitude weights should be.\n",nco_prg_nm_get(),fnc_nm);
      break;
    default:
      nco_dfl_case_generic_err(); break;
    } /* end nco_grd_lat_typ switch */
    
    /* Fuzzy test of latitude weight normalization */
    double lat_wgt_ttl_xpc; /* [frc] Expected sum of latitude weights */
    lat_wgt_ttl=0.0;
    for(idx=0L;idx<lat_nbr_out;idx++) lat_wgt_ttl+=lat_wgt_out[idx];
    lat_wgt_ttl_xpc=fabs(sin(dgr2rdn*lat_bnd_out[2L*(lat_nbr_out-1L)+1L])-sin(dgr2rdn*lat_bnd_out[0L])); /* fabs() ensures positive area in n2s grids */
    if(nco_grd_lat_typ != nco_grd_lat_unk){
      assert(1.0-lat_wgt_ttl/lat_wgt_ttl_xpc < eps_rlt);
      if(lat_wgt_ttl_xpc < 0.0) abort(); /* CEWI Use lat_wgt_ttl_xpc at least once outside of assert() to avoid gcc 4.8.2 set-but-not-used warning */
    } /* !nco_grd_lat_unk */
  } /* !flg_grd_out_rct */
    
  /* When possible, ensure area_out is non-zero
     20150722: ESMF documentation says "The grid area array is only output when the conservative remapping option is used"
     Actually, ESMF does (always?) output area, but area == 0.0 unless conservative remapping is used
     20150721: ESMF bilinear interpolation map ${DATA}/maps/map_ne30np4_to_fv257x512_bilin.150418.nc has area == 0.0
     20150710: Tempest regionally refined grids like bilinearly interpolated CONUS for ACME RRM has area_out == 0
     20150821: ESMF always outputs area_out == 0.0 for bilinear interpolation
     Check whether NCO must diagnose and provide its own area_out */
  /* If area_out contains any zero... */
  for(idx=0;idx<(long)grd_sz_out;idx++)
    if(area_out[idx] == 0.0) break;
  if(idx != (long)grd_sz_out){
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Output grid detected with zero-valued output area(s) at idx = %ld (and likely others, too).\n",nco_prg_nm_get(),idx);
  } /* !zero */

  for(idx=0;idx<(long)grd_sz_out;idx++)
    if(area_out[idx] != 0.0) break;
  if(idx == (long)grd_sz_out){
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports area_out from mapfile is everywhere zero. This is expected for bilinearly interpolated output maps produced by ESMF_RegridWeightGen. ",nco_prg_nm_get(),fnc_nm);
    if(flg_grd_out_2D && flg_grd_out_rct && (bnd_nbr_out == 2 || bnd_nbr_out == 4)){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"Since the destination grid provides cell bounds information, NCO will diagnose area (and output it as a variable named \"%s\") from the destination gridcell boundaries. NCO diagnoses quadrilateral area for rectangular output grids from a formula that assumes that cell boundaries follow arcs of constant latitude and longitude. This differs from the area of cells with boundaries that follow great circle arcs (used by, e.g., ESMF_RegridWeightGen and TempestRemap). Be warned that NCO correctly diagnoses area for all convex polygons, yet not for most concave polygons. To determine whether the diagnosed areas are fully consistent with the output grid, one must know such exact details. If your grid has analytic areas that NCO does not yet diagnose correctly from provided cell boundaries, please contact us.\n",rgr->area_nm);
      flg_dgn_area_out=True;
    }else if(flg_grd_out_2D && flg_grd_out_crv && (bnd_nbr_out == 2 || bnd_nbr_out == 4)){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"Since the destination grid provides cell bounds information, NCO will diagnose area (and output it as a variable named \"%s\") from the destination gridcell boundaries. NCO diagnoses quadrilateral area for curvilinear output grids from formulae that assume that cell boundaries follow great circle arcs (as do, e.g., ESMF_RegridWeightGen and TempestRemap). This differs from the area of cells with boundaries that follow lines of constant latitude or longitude. Be warned that NCO correctly diagnoses area for all convex polygons, yet not for most concave polygons. To determine whether the diagnosed areas are fully consistent with the output grid, one must know such exact details. If your grid has analytic areas that NCO does not yet diagnose correctly from provided cell boundaries, please contact us.\n",rgr->area_nm);
      flg_dgn_area_out=True;
    }else if(flg_grd_out_1D && flg_bnd_1D_usable){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"Since the destination grid provides cell bounds information, NCO will diagnose area (and output it as a variable name \"%s\") from the destination gridcell boundaries. NCO diagnoses spherical polygon area for unstructured output grids from formulae that assume that cell boundaries follow great circle arcs (as do, e.g., ESMFRegridWeightGen and TempestRemap). This differs from the area of cells with boundaries that follow lines of constant latitude or longitude. Be warned that NCO correctly diagnoses area for all convex polygons, yet not for most concave polygons. To determine whether the diagnosed areas are fully consistent with the output grid, one must know such exact details. If your grid has analytic areas that NCO does not yet diagnose correctly from provided cell boundaries, please contact us.\n",rgr->area_nm);
      flg_dgn_area_out=True;
    }else{ /* !1D */
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"However, NCO cannot find enough boundary information, or it is too stupid about spherical trigonometry, to diagnose area_out. NCO will output an area variable (named \"%s\") copied from the input mapfile. This area will be everywhere zero.\n",rgr->area_nm);
    } /* !2D */
  } /* !area */
      
  if(flg_dgn_area_out){
    if(flg_grd_out_1D && flg_bnd_1D_usable){
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"INFO: Diagnosing area_out for 1D grid\n");
      /* Area of unstructured grids requires spherical trigonometry */
      nco_sph_plg_area(rgr,lat_bnd_out,lon_bnd_out,col_nbr_out,bnd_nbr_out,area_out);
    } /* !1D */
    if(flg_grd_out_crv){
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"INFO: Diagnosing area_out for curvilinear grid\n");
      /* Area of curvilinear grids requires spherical trigonometry */
      nco_sph_plg_area(rgr,lat_crn_out,lon_crn_out,grd_sz_out,bnd_nbr_out,area_out);
    } /* !flg_grd_out_crv */
    if(flg_grd_out_rct && nco_grd_2D_typ != nco_grd_2D_unk){
      /* Mr. Enenstein and George O. Abell taught me the area of spherical zones
	 Spherical zone area is exact and faithful to underlying rectangular equi-angular grid
	 However, ESMF and Tempest approximate spherical polygons as connected by great circle arcs
	 fxm: Distinguish spherical zone shapes (e.g., equi-angular) from great circle arcs (e.g., unstructured polygons) */
      for(lat_idx=0;lat_idx<lat_nbr_out;lat_idx++)
	for(lon_idx=0;lon_idx<lon_nbr_out;lon_idx++)
	  area_out[lat_idx*lon_nbr_out+lon_idx]=fabs(dgr2rdn*(lon_bnd_out[2*lon_idx+1]-lon_bnd_out[2*lon_idx])*(sin(dgr2rdn*lat_bnd_out[2*lat_idx+1])-sin(dgr2rdn*lat_bnd_out[2*lat_idx]))); /* fabs() ensures positive area in n2s grids */
    } /* !spherical zones */
  } /* !flg_dgn_area_out */

  if(rgr->tst == -1){
    /* Passing --rgr tst=-1 causes regridder to fail here 
       This failure should cause host climo script to abort */
    (void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") reports regridder instructed to fail here. This tests failure mode in climo scripts...\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !tst */

  /* Verify frc_out is sometimes non-zero
     ESMF: "The grid frac arrays (frac_a and frac_b) are calculated by ESMF_RegridWeightGen. For conservative remapping, the grid frac array returns the area fraction of the grid cell which participates in the remapping. For bilinear and patch remapping, the destination grid frac array [frac_b] is one where the grid point participates in the remapping and zero otherwise. For bilinear and patch remapping, the source grid frac array is always set to zero."
     SCRIP: Similar to ESMF
     For both ESMF+SCRIP frac_[ab] are computed by the weight-generation algorithm and are not specified as part of the input grids
     How does an input ocean grid indicate that, say, half the gridcell is land and half ocean?
     Does it use the area variable to tell the weight generation algorithm that a gridcell is fractional?
     In other words does it use grid_imask=1 and grid_area=0.5*full_gridcell_area and, e.g., T=273.0? */
  for(idx=0;idx<(long)grd_sz_out;idx++)
    if(frc_out[idx] != 0.0) break;
  if(idx == (long)grd_sz_out){
    (void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") reports frc_out == frac_b contains all zeros\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !always zero */
  /* Test whether frc_out is ever zero... */
  for(idx=0;idx<(long)grd_sz_out;idx++)
    if(frc_out[idx] == 0.0) break;
  if(nco_dbg_lvl_get() >= nco_dbg_std)
    if(idx != (long)grd_sz_out)
      (void)fprintf(stdout,"%s: INFO %s reports frc_out == frac_b contains zero-elements (e.g., at 1D idx=%ld)\n",nco_prg_nm_get(),fnc_nm,idx);
  /* Normalizing by frc_out is redundant iff frc_out == 1.0, so we can save time without sacrificing accuracy
     However, frc_out is often (e.g., for CS <-> RLL maps) close but not equal to unity (ESMF_RegridWeightGen issue?)
     Hence, decide whether to normalize by frc_out by diagnosing the furthest excursion of frc_out from unity */
  nco_bool flg_frc_out_one=True; /* [flg] Destination gridcell fraction frc_out == frac_b is in [1-epsilon,frc_out,1+epsilon] */
  nco_bool flg_frc_out_wrt=False; /* [flg] Write destination gridcell fraction frc_out == frac_b to regridded files */
  double frc_out_dff_one; /* [frc] Deviation of frc_out from 1.0 */
  double frc_out_dff_one_max=0.0; /* [frc] Maximum deviation of frc_out from 1.0 */
  long idx_max_dvn; /* [idx] Index of maximum deviation from 1.0 */
  for(idx=0;idx<(long)grd_sz_out;idx++){
    frc_out_dff_one=fabs(frc_out[idx]-1.0);
    if(frc_out_dff_one > frc_out_dff_one_max){
      frc_out_dff_one_max=frc_out_dff_one;
      idx_max_dvn=idx;
    } /* !max */
  } /* !idx */
  if(frc_out_dff_one_max > eps_rlt) flg_frc_out_one=False;
  nco_bool flg_frc_nrm=False; /* [flg] Must normalize by frc_out == frac_b because frc_out is not always unity and specified normalization is destarea or none */
  if(!flg_frc_out_one && /* If fraction is sometimes "far" from 1.0 and ... */
     ((nco_rgr_mpf_typ == nco_rgr_mpf_ESMF && nco_rgr_mth_typ == nco_rgr_mth_conservative && (nco_rgr_nrm_typ == nco_rgr_nrm_destarea || nco_rgr_nrm_typ == nco_rgr_nrm_none)) || /* ESMF map-file specifies conservative regridding with "destarea" or "none" or ...  */
      (nco_rgr_mpf_typ != nco_rgr_mpf_ESMF)) /* 20191003:  Weight-generator does not adhere to ESMF "normalization type" convention */
     && True){
    flg_frc_nrm=True;
    /* Avoid writing frc_out unless discrepancies are particularly egregious
       Otherwise would frc_out for standard remaps like ne30->fv129x256 for which eps=2.46e-13 */
    double eps_rlt_wrt_thr=3.0e-13;
    /* 20181104: Never write frac_b for CMIP6! */
    /* if(frc_out_dff_one_max > eps_rlt_wrt_thr) flg_frc_out_wrt=True; */
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s reports global metadata specifies conservative remapping with normalization of type = %s. Furthermore, destination fractions frc_dst = dst_frac = frac_b = frc_out contain non-unity elements (maximum deviation from unity of %g exceeds hard-coded (in variable eps_rlt) relative-epsilon threshold of %g for frc_out[%ld] = %g). Thus normalization issues will be explicitly treated. Will apply \'destarea\' normalization (i.e., divide by non-zero frc_out[dst_idx]) to all regridded arrays.\n",nco_prg_nm_get(),fnc_nm,nco_rgr_nrm_sng(nco_rgr_nrm_typ),frc_out_dff_one_max,eps_rlt,idx_max_dvn,frc_out[idx_max_dvn]);
    if(nco_dbg_lvl_get() >= nco_dbg_std && flg_frc_out_wrt) (void)fprintf(stdout,"%s: INFO %s Maximum deviation %g exceeds threshold of %g that triggers automatic writing of fractional destination area as variable named frac_b in regridded output.\n",nco_prg_nm_get(),fnc_nm,frc_out_dff_one_max,eps_rlt_wrt_thr);
  } /* !sometimes non-unity */
  if(flg_frc_nrm && rgr->flg_rnr){
    // 20190918: Weaken from WARNING to INFO because NCO no longer renormalizes when using "destarea" maps unless specifically requested to with --rnr_thr
    (void)fprintf(stdout,"%s: INFO %s reports manual request to renormalize fields to preserve mean-values (rather than integral values) in destination gridcells that are incompletely covered by valid data in source gridcells (i.e., non-unity frc_dst = dst_frac = frac_b)\n",nco_prg_nm_get(),fnc_nm);
    //(void)fprintf(stdout,"%s: INFO %s reports manual request (with --rnr) to renormalize fields with non-unity frc_dst = dst_frac = frac_b at same time global metadata specifies normalization type = %s. Normalizing twice can be an error, depending on intent of each. Charlie is all ears on how NCO should handle this :)\n",nco_prg_nm_get(),fnc_nm,nco_rgr_nrm_sng(nco_rgr_nrm_typ));
    //nco_exit(EXIT_FAILURE);
  } /* !flg_rnr */

  /* Detailed summary of 2D grids now available including quality-checked coordinates and area */
  if(flg_grd_out_2D && nco_dbg_lvl_get() >= nco_dbg_sbr){
    lat_wgt_ttl=0.0;
    area_out_ttl=0.0;
    if(flg_grd_out_rct){
      (void)fprintf(stderr,"%s: INFO %s reports destination rectangular latitude grid:\n",nco_prg_nm_get(),fnc_nm);
      for(idx=0;idx<lat_nbr_out;idx++)
	lat_wgt_ttl+=lat_wgt_out[idx];
    } /* !flg_grd_out_rct */
    for(lat_idx=0;lat_idx<lat_nbr_out;lat_idx++)
      for(lon_idx=0;lon_idx<lon_nbr_out;lon_idx++)
	area_out_ttl+=area_out[lat_idx*lon_nbr_out+lon_idx];
    (void)fprintf(stdout,"lat_wgt_ttl = %20.15f, frc_lat_wgt = %20.15f, area_ttl = %20.15f, frc_area = %20.15f\n",lat_wgt_ttl,lat_wgt_ttl/2.0,area_out_ttl,area_out_ttl/(4.0*M_PI));
    if(flg_grd_out_rct){
      for(idx=0;idx<lon_nbr_out;idx++) (void)fprintf(stdout,"lon[%li] = [%g, %g, %g]\n",idx,lon_bnd_out[2*idx],lon_ctr_out[idx],lon_bnd_out[2*idx+1]);
      for(idx=0;idx<lat_nbr_out;idx++) (void)fprintf(stdout,"lat[%li] = [%g, %g, %g]\n",idx,lat_bnd_out[2*idx],lat_ctr_out[idx],lat_bnd_out[2*idx+1]);
      for(idx=0;idx<lat_nbr_out;idx++) (void)fprintf(stdout,"lat[%li], wgt[%li] = %20.15f, %20.15f\n",idx,idx,lat_ctr_out[idx],lat_wgt_out[idx]);
    } /* !flg_grd_out_rct */
    if(nco_dbg_lvl_get() > nco_dbg_crr)
      for(lat_idx=0;lat_idx<lat_nbr_out;lat_idx++)
	for(lon_idx=0;lon_idx<lon_nbr_out;lon_idx++)
	  (void)fprintf(stdout,"lat[%li] = %g, lon[%li] = %g, area[%li,%li] = %g\n",lat_idx,lat_ctr_out[lat_idx],lon_idx,lon_ctr_out[lon_idx],lat_idx,lon_idx,area_out[lat_idx*lon_nbr_out+lon_idx]);
    assert(area_out_ttl > 0.0);
    assert(area_out_ttl <= 4.0*M_PI + 5.0e-15);
  } /* !flg_grd_out_2D && !dbg */

  /* Allocate space for and obtain weights and addresses */
  wgt_raw=(double *)nco_malloc_dbg(mpf.num_links*nco_typ_lng(NC_DOUBLE),fnc_nm,"Unable to malloc() value buffer for remapping weights");
  col_src_adr=(int *)nco_malloc_dbg(mpf.num_links*nco_typ_lng(NC_INT),fnc_nm,"Unable to malloc() value buffer for remapping addresses");
  row_dst_adr=(int *)nco_malloc_dbg(mpf.num_links*nco_typ_lng(NC_INT),fnc_nm,"Unable to malloc() value buffer for remapping addresses");

  /* Obtain remap matrix addresses and weights from map file */
  dmn_srt[0]=0L;
  dmn_cnt[0]=mpf.num_links;
  rcd=nco_get_vara(in_id,col_src_adr_id,dmn_srt,dmn_cnt,col_src_adr,NC_INT);
  rcd=nco_get_vara(in_id,row_dst_adr_id,dmn_srt,dmn_cnt,row_dst_adr,NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=mpf.num_links;
  if(nco_rgr_mpf_typ != nco_rgr_mpf_SCRIP){
    rcd=nco_get_vara(in_id,wgt_raw_id,dmn_srt,dmn_cnt,wgt_raw,NC_DOUBLE);
  }else{
    /* SCRIP mapfiles store 2D weight array remap_matrix[num_links,num_wgts]
       Apply only first weight for first-order conservative accuracy (i.e., area overlap)
       Apply all three weights for second-order conservative accuracy (by including gradients from centroid to vertices) */
    dmn_srd[0]=1L;
    dmn_srt[1]=0L;
    dmn_cnt[1]=1L;
    dmn_srd[1]=mpf.num_wgts;
    rcd=nco_get_vars(in_id,wgt_raw_id,dmn_srt,dmn_cnt,dmn_srd,wgt_raw,NC_DOUBLE);
  } /* !SCRIP */

  /* Pre-subtract one from row/column addresses (stored, by convention, as Fortran indices) to optimize access with C indices */
  size_t lnk_nbr; /* [nbr] Number of links */
  size_t lnk_idx; /* [idx] Link index */
  lnk_nbr=mpf.num_links;
  for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++) row_dst_adr[lnk_idx]--;
  for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++) col_src_adr[lnk_idx]--;
  if(nco_dbg_lvl_get() >= nco_dbg_io){
    (void)fprintf(stdout,"idx row_dst col_src wgt_raw\n");
    for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++) (void)fprintf(stdout,"%li %d %d %g\n",lnk_idx,row_dst_adr[lnk_idx],col_src_adr[lnk_idx],wgt_raw[lnk_idx]);
  } /* endif dbg */

  /* Free memory associated with input file */
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
  if(dmn_srd) dmn_srd=(long *)nco_free(dmn_srd);

  /* Close input netCDF file */
  nco_close(in_id);

  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  /* Above this line, fl_in and in_id refer to map file
     Below this line, fl_in and in_id refer to input file to be regridded */

  /* Initialize */
  in_id=rgr->in_id;
  out_id=rgr->out_id;

  /* Sanity check that input data file matches expectations from mapfile */
  char *col_nm_in=rgr->col_nm_in; /* [sng] Name to recognize as input horizontal spatial dimension on unstructured grid */
  char *lat_nm_in=rgr->lat_nm_in; /* [sng] Name of input dimension to recognize as latitude */
  char *lon_nm_in=rgr->lon_nm_in; /* [sng] Name of input dimension to recognize as longitude */
  int dmn_id_col=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lat; /* [id] Dimension ID */
  int dmn_id_lon; /* [id] Dimension ID */

  /* 20160503 Discover coordinates via CF Convention if indicated
     This copies method used in nco_grd_nfr() */
  /* Begin CF-coordinates block */
  cf_crd_sct *cf=NULL;
  char *rgr_var; /* [sng] Variable for special regridding treatment */
  nco_bool flg_cf=False; /* [flg] Follow CF Coordinates convention to find and infer grid */
  rgr_var=rgr->var_nm;
  if(rgr_var){
    /* Infer grid from special variable
       Intended to be variable that has both horizontal dimensions and "coordinates" attribute, e.g.,
       ncks --cdl -m ${DATA}/hdf/narrmon-a_221_20100101_0000_000.nc | grep coordinates
       4LFTX_221_SPDY_S113:coordinates = "gridlat_221 gridlon_221" ;
       Usage:
       ncks -O -D 3 --rgr infer --rgr_var=ALBDO_221_SFC_S113 --rgr grid=${HOME}/grd_narr.nc ${DATA}/hdf/narrmon-a_221_20100101_0000_000.nc ~/foo.nc */
    char crd_sng[]="coordinates"; /* CF-standard coordinates attribute name */
    
    cf=(cf_crd_sct *)nco_malloc(sizeof(cf_crd_sct));
    cf->crd=False; /* [flg] CF coordinates information is complete */
    cf->crd_id[0]=NC_MIN_INT; /* [id] Coordinate ID, first */
    cf->crd_id[1]=NC_MIN_INT; /* [id] Coordinate ID, second */
    cf->crd_nm[0]=NULL; /* [sng] Coordinate name, first */
    cf->crd_nm[1]=NULL; /* [sng] Coordinate name, second */
    cf->crd_sng=NULL; /* [sng] Coordinates attribute value */
    cf->dmn_id[0]=NC_MIN_INT; /* [id] Dimension ID, first */
    cf->dmn_id[1]=NC_MIN_INT; /* [id] Dimension ID, second */
    cf->dmn_nm[0]=NULL; /* [sng] Dimension name, first */
    cf->dmn_nm[1]=NULL; /* [sng] Dimension name, second */
    cf->unt_sng[0]=NULL; /* [sng] Units string, first coordinate */
    cf->unt_sng[1]=NULL; /* [sng] Units string, second coordinate */
    cf->var_id=NC_MIN_INT; /* [id] Coordinate variable ID */
    cf->var_nm=NULL; /* [sng] Coordinates variable name */
    cf->var_type=NC_NAT; /* [enm] Coordinates variable type */

    if((rcd=nco_inq_varid_flg(in_id,rgr_var,&cf->var_id)) != NC_NOERR){
      (void)fprintf(stderr,"%s: WARNING %s reports special \"coordinates\" variable %s not found. Turning-off CF coordinates search.\n",nco_prg_nm_get(),fnc_nm,rgr_var);
      goto skp_cf;
    } /* !rcd */ 

    cf->crd_sng=nco_char_att_get(in_id,cf->var_id,crd_sng);
    if(cf->crd_sng){
      cf->crd=True;
    }else{ /* !rcd && att_typ */
      (void)fprintf(stderr,"%s: WARNING %s reports coordinates variable %s does not have character-valued \"coordinates\" attribute. Turning-off CF coordinates search.\n",nco_prg_nm_get(),fnc_nm,rgr_var);
      goto skp_cf;
    } /* !rcd && att_typ */
      
    /* Valid coordinates attribute requires two coordinate names separated by space character */
    char *crd_nm[NCO_MAX_CRD_PER_VAR]; /* [sng] Coordinate name start position */
    char *crd_dpl; /* [sng] Modifiable duplicate of coordinates string */
    char *spc_ptr; /* [sng] Pointer to space character (' ') */
    int crd_nbr=0; /* [nbr] Number of names in coordinates attribute */
    int crd_spt=0; /* [nbr] Number of "spatial-like" (that include "degree" in units) coordinates */
    int crd_idx=0; /* [idx] Counter for coordinate names */
    for(crd_idx=0;crd_idx<NCO_MAX_CRD_PER_VAR;crd_idx++) crd_nm[crd_idx]=NULL;
    crd_dpl=(char *)strdup(cf->crd_sng);
    /* Search for spaces starting from end of string */
    while((spc_ptr=strrchr(crd_dpl,' '))){
      crd_nm[crd_nbr]=spc_ptr+1L;
      crd_nbr++;
      /* NUL-terminate so next search ends here */
      *spc_ptr='\0'; 
    } /* !sbs_ptr */
    /* Final coordinate name begins where coordinate string starts */
    crd_nm[crd_nbr]=crd_dpl;
    /* Change crd_nbr from 0-based index to actual coordinate number */
    crd_nbr++;
    if(crd_nbr < 2){
      (void)fprintf(stderr,"%s: WARNING %s found only %d coordinate(s) in \"coordinates\" attribute \"%s\", at least two are required. Turning-off CF coordinates search.\n",nco_prg_nm_get(),fnc_nm,crd_nbr,cf->crd_sng);
      goto skp_cf;
    } /* !crd_nbr */
    /* If more than two coordinate names are present, choose first two (searching backwards from end) with "degree" in units attributes, otherwise just choose first two */
    crd_idx=crd_spt=0;
    while(crd_spt < 2 && crd_idx < crd_nbr){
      cf->crd_nm[crd_spt]=crd_nm[crd_idx];
      if((rcd=nco_inq_varid_flg(in_id,cf->crd_nm[crd_spt],&cf->crd_id[crd_spt])) == NC_NOERR){
	cf->unt_sng[crd_spt]=nco_char_att_get(in_id,cf->crd_id[crd_spt],unt_sng);
	if(cf->unt_sng[crd_spt]){
	  if(strcasestr(cf->unt_sng[crd_spt],"degree")){
	    /* Increment count of spatial-like coordinates... */
	    crd_spt++;
	  }else{
	    /* ...or free() memory allocated during search */
	    cf->unt_sng[crd_spt]=(char *)nco_free(cf->unt_sng[crd_spt]);
	  } /* !strcasestr() */
	  crd_idx++;
	} /* !rcd && att_typ */
      } /* !rcd */ 
    } /* !crd_spt */

    /* If while()-loop above was successful, our search is over
       Otherwise, use first two coordinate names regardless of units, and print more diagnostics */
    if(crd_spt < 2){
      cf->crd_nm[0]=crd_nm[0];
      cf->crd_nm[1]=crd_nm[1];
      if((rcd=nco_inq_varid_flg(in_id,cf->crd_nm[0],&cf->crd_id[0])) != NC_NOERR){
	(void)fprintf(stderr,"%s: WARNING %s reports first coordinates variable %s not found. Turning-off CF coordinates search for this file.\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0]);
	goto skp_cf;
      } /* !rcd */ 
      if((rcd=nco_inq_varid_flg(in_id,cf->crd_nm[1],&cf->crd_id[1])) != NC_NOERR){
	(void)fprintf(stderr,"%s: WARNING %s reports second coordinates variable %s not found. Turning-off CF coordinates search for this file.\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[1]);
	goto skp_cf;
      } /* !rcd */ 
      
      cf->unt_sng[0]=nco_char_att_get(in_id,cf->crd_id[0],unt_sng);
      if(cf->unt_sng[0]){
	if(!strcasestr(cf->unt_sng[0],"degrees_")) (void)fprintf(stderr,"%s: WARNING %s reports first coordinates variable %s has weird units attribute = %s. May not detect correct ordering of latitude and longitude coordinates\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0],cf->unt_sng[0]);
      } /* !rcd && att_typ */
      cf->unt_sng[1]=nco_char_att_get(in_id,cf->crd_id[1],unt_sng);
      if(cf->unt_sng[1]){
	if(!strcasestr(cf->unt_sng[1],"degrees_")) (void)fprintf(stderr,"%s: WARNING %s reports second coordinates variable %s has weird units attribute = %s. May not detect correct ordering of latitude and longitude coordinates\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[1],cf->unt_sng[1]);
      } /* !rcd && att_typ */
    } /* !crd_spt */
      
    int crd_rnk; /* [nbr] Coordinate rank */
    rcd=nco_inq_varndims(in_id,cf->crd_id[0],&crd_rnk);
    if(crd_rnk != 2){
      (void)fprintf(stderr,"%s: INFO %s reports coordinates variable %s has %i dimension(s). Skipping CF coordinates method.\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0],crd_rnk);
      goto skp_cf;
    } /* !crd_rnk */
    rcd=nco_inq_vardimid(in_id,cf->crd_id[0],cf->dmn_id);
    cf->dmn_nm[0]=(char *)nco_malloc(NC_MAX_NAME*sizeof(NC_CHAR));
    cf->dmn_nm[1]=(char *)nco_malloc(NC_MAX_NAME*sizeof(NC_CHAR));
    rcd=nco_inq_dimname(in_id,cf->dmn_id[0],cf->dmn_nm[0]);
    rcd=nco_inq_dimname(in_id,cf->dmn_id[1],cf->dmn_nm[1]);
    
    /* "coordinates" convention does not guarantee lat, lon are specified in that order
       Use "units" values, if any, to determine order
       In absence of "units", assume order is lat, lon */ 
    nco_bool crd0_is_lat=False; /* [flg] First coordinate is latitude */
    nco_bool crd0_is_lon=False; /* [flg] First coordinate is longitude */
    nco_bool crd1_is_lat=False; /* [flg] Second coordinate is latitude */
    nco_bool crd1_is_lon=False; /* [flg] Second coordinate is longitude */
    if(cf->unt_sng[0]){
      if(!strcasecmp(cf->unt_sng[0],"degrees_north")) crd0_is_lat=True;
      if(!strcasecmp(cf->unt_sng[0],"degrees_east")) crd0_is_lon=True;
    } /* endif */      
    if(cf->unt_sng[1]){
      if(!strcasecmp(cf->unt_sng[1],"degrees_north")) crd1_is_lat=True;
      if(!strcasecmp(cf->unt_sng[1],"degrees_east")) crd1_is_lon=True;
    } /* endif */      
    assert((crd0_is_lat && crd1_is_lon) || (crd0_is_lon && crd1_is_lat));
    int idx_lat;
    int idx_lon;
    if(crd0_is_lat && crd1_is_lon){
      idx_lat=0;
      idx_lon=1;
    }else{
      idx_lat=1;
      idx_lon=0;
    } /* endif */
    
    /* Dimensions and coordinates have been vetted. Store as primary lookup names.
       Dimensions are always returned in order [LRV,MRV]=[0,1]
       LRV is along-track direction, and MRV is across-track (at least in NASA data)
       Internally we label LRV as "lat" and MRV as "lon" so that code looks similar for curvilinear and rectangular grids */
    dmn_id_lat=cf->dmn_id[0];
    dmn_id_lon=cf->dmn_id[1];
    /* Subtlety: lat_nm_in is coordinate (variable+dimension) name when specified from command-line (as in nco_grd_nfr()), dimension name when found through CF-method (as in nco_rgr_wgt()). This confusing distinction could be avoided by passing command-line dimension names through-to nco_rgr_wgt(). However, that route would require complex priorities for what to do when passing command-line coordinate names not dimension names and visa-versa. */
    lat_nm_in=strdup(cf->dmn_nm[0]);
    lon_nm_in=strdup(cf->dmn_nm[1]);
    //lat_nm_in=strdup(cf->crd_nm[idx_lat]);
    //lon_nm_in=strdup(cf->crd_nm[idx_lon]);
    /* Next four lines unnecessary in nco_rgr_wgt() which only needs dimension names (it reads input coordinates from map-file not data-file) */
    //lat_ctr_id=cf->crd_id[idx_lat];
    //lon_ctr_id=cf->crd_id[idx_lon];
    //lat_dmn_nm=strdup(cf->dmn_nm[0]);
    //lon_dmn_nm=strdup(cf->dmn_nm[1]);
    
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s reports coordinates variable %s \"coordinates\" attribute \"%s\" points to coordinates %s and %s. Latitude coordinate \"%s\" has dimensions \"%s\" and \"%s\". Longitude coordinate \"%s\" has dimensions \"%s\" and \"%s\".\n",nco_prg_nm_get(),fnc_nm,rgr_var,cf->crd_sng,cf->crd_nm[0],cf->crd_nm[1],cf->crd_nm[idx_lat],cf->dmn_nm[idx_lat],cf->dmn_nm[idx_lon],cf->crd_nm[idx_lon],cf->dmn_nm[idx_lat],cf->dmn_nm[idx_lon]);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s Coordinates %s and %s \"units\" values are \"%s\" and \"%s\", respectively.\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0],cf->crd_nm[1],cf->unt_sng[0] ? cf->unt_sng[0] : "(non-existent)",cf->unt_sng[1] ? cf->unt_sng[1] : "(non-existent)");

    /* Clean-up CF coordinates memory */
    if(crd_dpl) crd_dpl=(char *)nco_free(crd_dpl);
    if(cf->crd_sng) cf->crd_sng=(char *)nco_free(cf->crd_sng);
    if(cf->dmn_nm[0]) cf->dmn_nm[0]=(char *)nco_free(cf->dmn_nm[0]);
    if(cf->dmn_nm[1]) cf->dmn_nm[1]=(char *)nco_free(cf->dmn_nm[1]);
    if(cf->unt_sng[0]) cf->unt_sng[0]=(char *)nco_free(cf->unt_sng[0]);
    if(cf->unt_sng[1]) cf->unt_sng[1]=(char *)nco_free(cf->unt_sng[1]);
    //    if(foo) foo=(char *)nco_free(foo);
  } /* !rgr_var */

  /* goto skp_cf */
 skp_cf: 
  /* free() any abandoned cf structure now */
  if(!flg_cf)
    if(cf) cf=(cf_crd_sct *)nco_free(cf);
  rcd=NC_NOERR;
  /* End CF-coordinates block */

  if(flg_grd_in_1D){
    long col_nbr_in_dat; /* [nbr] Number of columns in input datafile */
    /* Check default or command-line option first, then search usual suspects, and if that fails then guess
       unstructured dimension is dimension in input file with size n_a expected by input map file, suggested by PJCS
       Using internal database names first ensures users can pick between multiple dimensions of size n_a 
       20180313: fxm New PJCS algorithm is superior, should eliminate internal database for unstructured grids? 
       Database is necessary for 2D grids because otherwise no good way to disambiguate latitude from longitude */
    if(col_nm_in && (rcd=nco_inq_dimid_flg(in_id,col_nm_in,&dmn_id_col)) == NC_NOERR) /* do nothing */; 
    else if((rcd=nco_inq_dimid_flg(in_id,"lndgrid",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("lndgrid"); /* CLM */
    else if((rcd=nco_inq_dimid_flg(in_id,"nCells",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("nCells"); /* MPAS-O/I */
    else if((rcd=nco_inq_dimid_flg(in_id,"nEdges",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("nEdges"); /* MPAS-O/I */
    else if((rcd=nco_inq_dimid_flg(in_id,"ncol_d",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("ncol_d"); /* EAM dynamics grid */
    else if((rcd=nco_inq_dimid_flg(in_id,"ncol_p",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("ncol_d"); /* EAM physics grid */
    else if((rcd=nco_inq_dimid_flg(in_id,"sounding_id",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("sounding_id"); /* OCO2 */
    /* 20180605: Database matches to above names may be false-positives
       ALM/CLM/CTSM/ELM store all possible dimension names that archived variables could use
       NCO only prints dimensions used in variables, while ncdump prints all dimensions 
       From ncdump we find usually unused ALM/CLM/CTSM/ELM dimensions: gridcell, lndunit, column, pft, levurb, numrad, levsno
       Check that matched dimension has expected size: */
    if(dmn_id_col != NC_MIN_INT){
      rcd=nco_inq_dimlen(in_id,dmn_id_col,&col_nbr_in_dat);
      if(col_nbr_in != col_nbr_in_dat){
	dmn_id_col=NC_MIN_INT;
	if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s database-prioritized unstructured dimension candidate \"%s\" has size not expected by supplied map-file: mapfile col_nbr_in = %ld != %ld = col_nbr_in from datafile. HINT: Check that source grid (i.e., \"grid A\") used to create mapfile matches grid on which data are stored in input datafile.\n",nco_prg_nm_get(),fnc_nm,col_nm_in,col_nbr_in,col_nbr_in_dat);
      } /* !col_nbr_in */
    }else{
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s expects data on an unstructured grid yet cannot find a dimension name that matches the usual suspects for unstructured dimensions (ncol, gridcell, lndgrid, nCells, nEdges, sounding_id). Consider specifying horizontal dimension name to ncks with \"--rgr col_nm=foo\" or to ncremap with \"ncremap -R '--rgr col_nm=foo'\", and consider requesting the NCO project to add this horizontal dimension name to its internal database.\n",nco_prg_nm_get(),fnc_nm);
    } /* !dmn_id_col */
    if(dmn_id_col == NC_MIN_INT){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s Proceeding with fallback algorithm to guess unstructured dimension as first dimension in data file of equal size to that expected by supplied map-file...\n",nco_prg_nm_get(),fnc_nm);
      /* 20180312: Unstructured dimension must have same size as input map file, suggested by PJCS */
      int *dmn_ids_in; /* [nbr] Input file dimension IDs */
      int dmn_nbr_in; /* [nbr] Number of dimensions in input file */
      const int flg_prn=0; /* [enm] Parent flag */
      rcd=nco_inq_dimids(in_id,&dmn_nbr_in,NULL,flg_prn);
      dmn_ids_in=(int *)nco_malloc(dmn_nbr_in*sizeof(int));
      rcd=nco_inq_dimids(in_id,NULL,dmn_ids_in,flg_prn);
      /* Find dimension, if any, with same size as map "a" src_grid_dims[0] = n_a dimension */
      for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	dmn_id_col=dmn_ids_in[dmn_idx];
	rcd=nco_inq_dimlen(in_id,dmn_id_col,&col_nbr_in_dat);
	if(col_nbr_in == col_nbr_in_dat){
	  rcd=nco_inq_dimname(in_id,dmn_id_col,col_nm_in);
	  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s found that dimension %s in datafile has same size (n_a = %ld) expected by map-file. Assuming %s is the unstructured dimension.\n",nco_prg_nm_get(),fnc_nm,col_nm_in,col_nbr_in,col_nm_in);
	  break;
	} /* !col_nbr_in */
      } /* !dmn_idx */
      if(dmn_ids_in) dmn_ids_in=(int *)nco_free(dmn_ids_in);
      if(dmn_idx == dmn_nbr_in){
	dmn_id_col=NC_MIN_INT;
	(void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") expects data on an unstructured grid but cannot find a dimension in the input data file (or, with ncremap, a possibly already subsetted intermediate file) that matches the size of the unstructured dimension in the supplied map-file = src_grd_dims[0] = n_a = %ld.\nHINT: Ensure at least one member of the variable extraction list has a spatial dimension of size = %ld\n",nco_prg_nm_get(),fnc_nm,col_nbr_in,col_nbr_in);
	nco_exit(EXIT_FAILURE);
      } /* !dmn_idx */
    } /* !col_nm_in */
  } /* !1D */
  if(flg_grd_in_2D){
    long lat_nbr_in_dat; /* [nbr] Number of latitudes in input datafile */
    if(lat_nm_in && (rcd=nco_inq_dimid_flg(in_id,lat_nm_in,&dmn_id_lat)) == NC_NOERR) /* do nothing */; 
    else if((rcd=nco_inq_dimid_flg(in_id,"latitude",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("latitude");
    else if((rcd=nco_inq_dimid_flg(in_id,"lat",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("lat");
    else if((rcd=nco_inq_dimid_flg(in_id,"Latitude",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("Latitude");
    else if((rcd=nco_inq_dimid_flg(in_id,"Lat",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("Lat");
    else if((rcd=nco_inq_dimid_flg(in_id,"south_north",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("south_north"); /* WRF */
    else if((rcd=nco_inq_dimid_flg(in_id,"south_north_stag",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("south_north_stag");
    else if((rcd=nco_inq_dimid_flg(in_id,"YDim:location",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("YDim:location"); /* AIRS L3 */
    else if((rcd=nco_inq_dimid_flg(in_id,"YDim:MOD_Grid_monthly_CMG_VI",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("YDim:MOD_Grid_monthly_CMG_VI"); /* MODIS MOD13C2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"natrack",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("natrack"); /* MODIS DeepBlue SeaWiFS L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"nj",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("nj"); /* CICE RTM */
    else if((rcd=nco_inq_dimid_flg(in_id,"nlat",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("nlat"); /* POP */
    else if((rcd=nco_inq_dimid_flg(in_id,"rlat",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("rlat"); /* RACMO */
    else if((rcd=nco_inq_dimid_flg(in_id,"nscan",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("nscan"); /* AMSR, TRMM */
    else if((rcd=nco_inq_dimid_flg(in_id,"nTimes",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("nTimes"); /* OMI L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"number_of_lines",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("number_of_lines"); /* DSCOVR L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"GeoTrack",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("GeoTrack"); /* AIRS L2 DAP NC */
    else if((rcd=nco_inq_dimid_flg(in_id,"GeoTrack:L2_Standard_atmospheric&surface_product",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("GeoTrack:L2_Standard_atmospheric&surface_product"); /* AIRS L2 HDF */
    else if((rcd=nco_inq_dimid_flg(in_id,"Cell_Along_Swath:mod04",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("Cell_Along_Swath:mod04"); /* MODIS MOD04 L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"Cell_Along_Swath_mod04",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("Cell_Along_Swath_mod04"); /* MODIS MOD04 L2 (ncl_convert2nc changes colon to underscore) */
    else if((rcd=nco_inq_dimid_flg(in_id,"CO_Latitude",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("CO_Latitude");
    else if((rcd=nco_inq_dimid_flg(in_id,"j",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("j"); /* CMIP5 NorESM1 ocean */
    else if((rcd=nco_inq_dimid_flg(in_id,"latitude0",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("latitude0"); /* Oxford */
    else if((rcd=nco_inq_dimid_flg(in_id,"y",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("y"); /* NEMO */
    else if((rcd=nco_inq_dimid_flg(in_id,"x",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("x"); /* NSIDC polar stereographic (NB: unfortunate incompatible conflict between NEMO & NSIDC names) */
    else if((rcd=nco_inq_dimid_flg(in_id,"y1",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("y1"); /* NSIDC EASE */
    else if((rcd=nco_inq_dimid_flg(in_id,"ygrid_0",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("ygrid_0"); /* NWS HRRR */
    else{
      (void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") reports unable to find latitude dimension in input file. Tried the usual suspects. HINT: Inform regridder of input latitude dimension name with \"ncks --rgr lat_nm_in=name\" or \"ncremap -R '--rgr lat_nm_in=name'\"\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    } /* !lat */
    rcd=nco_inq_dimlen(in_id,dmn_id_lat,&lat_nbr_in_dat);
    if(lat_nbr_in != lat_nbr_in_dat){
      (void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") reports mapfile and data file dimension sizes disagree: mapfile lat_nbr_in = %ld != %ld = lat_nbr_in from datafile. HINT: Check that source grid (i.e., \"grid A\") used to create mapfile matches grid on which data are stored in input datafile.\n",nco_prg_nm_get(),fnc_nm,lat_nbr_in,lat_nbr_in_dat);
      nco_exit(EXIT_FAILURE);
    } /* !err */
    long lon_nbr_in_dat; /* [nbr] Number of longitudes in input datafile */
    if(lon_nm_in && (rcd=nco_inq_dimid_flg(in_id,lon_nm_in,&dmn_id_lon)) == NC_NOERR) /* do nothing */; 
    else if((rcd=nco_inq_dimid_flg(in_id,"longitude",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("longitude");
    else if((rcd=nco_inq_dimid_flg(in_id,"lon",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("lon");
    else if((rcd=nco_inq_dimid_flg(in_id,"Longitude",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("Longitude");
    else if((rcd=nco_inq_dimid_flg(in_id,"Lon",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("Lon");
    else if((rcd=nco_inq_dimid_flg(in_id,"west_east",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("west_east"); /* WRF */
    else if((rcd=nco_inq_dimid_flg(in_id,"west_east_stag",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("west_east_stag");
    else if((rcd=nco_inq_dimid_flg(in_id,"XDim:location",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("XDim:location"); /* AIRS L3 */
    else if((rcd=nco_inq_dimid_flg(in_id,"XDim:MOD_Grid_monthly_CMG_VI",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("XDim:MOD_Grid_monthly_CMG_VI"); /* MODIS MOD13C2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"ni",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("ni"); /* CICE RTM */
    else if((rcd=nco_inq_dimid_flg(in_id,"nlon",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("nlon"); /* POP */
    else if((rcd=nco_inq_dimid_flg(in_id,"rlon",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("rlon"); /* POP */
    else if((rcd=nco_inq_dimid_flg(in_id,"npix",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("npix"); /* AMSR */
    else if((rcd=nco_inq_dimid_flg(in_id,"npixel",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("npixel"); /* TRMM */
    else if((rcd=nco_inq_dimid_flg(in_id,"nxtrack",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("nxtrack"); /* MODIS DeepBlue SeaWiFS L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"nXtrack",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("nXtrack"); /* OMI L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"number_of_pixels",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("number_of_pixels"); /* DSCOVR L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"GeoXTrack",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("GeoXTrack"); /* AIRS L2 DAP NC */
    else if((rcd=nco_inq_dimid_flg(in_id,"GeoXTrack:L2_Standard_atmospheric&surface_product",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("GeoXTrack:L2_Standard_atmospheric&surface_product"); /* AIRS L2 HDF */
    else if((rcd=nco_inq_dimid_flg(in_id,"Cell_Across_Swath:mod04",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("Cell_Across_Swath:mod04"); /* MODIS MOD04 L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"Cell_Across_Swath_mod04",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("Cell_Across_Swath_mod04"); /* MODIS MOD04 L2 (ncl_convert2nc changes colon to underscore) */
    else if((rcd=nco_inq_dimid_flg(in_id,"i",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("i"); /* CMIP5 NorESM1 ocean */
    else if((rcd=nco_inq_dimid_flg(in_id,"longitude0",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("longitude0"); /* Oxford */
    else if((rcd=nco_inq_dimid_flg(in_id,"x",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("x"); /* NEMO */
    else if((rcd=nco_inq_dimid_flg(in_id,"y",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("y"); /* NSIDC polar stereographic (NB: unfortunate incompatible conflict between NEMO & NSIDC names) */
    else if((rcd=nco_inq_dimid_flg(in_id,"x1",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("x1"); /* NSIDC EASE */
    else if((rcd=nco_inq_dimid_flg(in_id,"xgrid_0",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("xgrid_0"); /* NWS HRRR */
    else{
      (void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") reports unable to find longitude dimension in input file. Tried the usual suspects. HINT: Inform regridder of input longitude dimension name with \"ncks --rgr lon_nm_in=name\" or \"ncremap -R '--rgr lon_nm_in=name'\"\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    } /* !lat */
    rcd=nco_inq_dimlen(in_id,dmn_id_lon,&lon_nbr_in_dat);
    if(lon_nbr_in != lon_nbr_in_dat){
      (void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") reports mapfile and data file dimension sizes disagree: mapfile lon_nbr_in = %ld != %ld = lon_nbr_in from datafile. HINT: Check that source grid (i.e., \"grid A\") used to create mapfile matches grid on which data are stored in input datafile.\n",nco_prg_nm_get(),fnc_nm,lon_nbr_in,lon_nbr_in_dat);
      nco_exit(EXIT_FAILURE);
    } /* !err */
  } /* !2D */
    
  /* Do not extract grid variables (that are also extensive variables) like lon, lat, area, and masks
     If necessary, use remap data to diagnose them from scratch
     Other extensive variables (like counts, population) will be extracted and summed not averaged */
  /* Exception list source:
     ALM/CLM: landmask (20170504: Debatable, including erroneous mask may be better than completely excluding an expected mask) (20170504: must keep landfrac since regridded by ncremap for SGS option)
     AMSR: Latitude, Longitude
     CAM, CERES, CMIP5: lat, lon
     CAM, CMIP5: gw, lat_bnds, lon_bnds
     CAM-FV: slon, slat, w_stag (w_stag is weights for slat grid, analagous to gw for lat grid)
     CAM-SE, EAM, MOSART: area
     CICE: latt_bounds, lont_bounds, latu_bounds, lonu_bounds, TLAT, TLON, ULAT, ULON (NB: CICE uses ?LON and POP uses ?LONG) (aice is ice area, tmask is state-variable mask, both not currently excluded, although all binary masks like tmask should be recomputed on new grid)
     DSCOVR L2: latitude, longitude
     ESMF: gridcell_area
     GPM: S1_Latitude, S1_Longitude
     HIRDLS: Latitude
     MAR/RACMO: LAT, LON
     MLS: CO_Latitude
     MPAS-O/I/LI: areaCell, latCell, lonCell and others that are all handled by separated MPAS convention implementation below
     NCO: lat_vertices, lon_vertices
     NEMO: nav_lat, nav_lon
     NWS HRRR: gridlat_0, gridlon_0
     OCO2: latitude_bnds, longitude_bnds
     OMI DOMINO: Latitude, LatitudeCornerpoints, Longitude, LongitudeCornerpoints
     Oxford: global_latitude0, global_longitude0, latitude0, longitude0
     POP: TLAT, TLONG, ULAT, ULONG  (NB: CICE uses ?LON and POP uses ?LONG) (POP does not archive spatial bounds)
     RACMO: rlat, rlon
     TRMM: Latitude, Longitude
     UV-CDAT regridder: bounds_lat, bounds_lon
     Unknown: XLAT_M, XLONG_M
     WRF: XLAT, XLONG */
  const int var_xcl_lst_nbr=51; /* [nbr] Number of objects on exclusion list */
  const char *var_xcl_lst[]={"/area","/gridcell_area","/gw","/LAT","/lat","/Latitude","/latitude","/nav_lat","/global_latitude0","gridlat_0","/latitude0","/rlat","/slat","/TLAT","/ULAT","/XLAT","/XLAT_M","/CO_Latitude","/S1_Latitude","/lat_bnds","/lat_vertices","/latt_bounds","/latu_bounds","/latitude_bnds","/LatitudeCornerpoints","/bounds_lat","/LON","/lon","/Longitude","/longitude","/nav_lon","/global_longitude0","gridlon_0","/longitude0","/rlon","/slon","/TLON","/TLONG","/ULON","/ULONG","/XLONG","/XLONG_M","/CO_Longitude","/S1_Longitude","/lon_bnds","/lon_vertices","/lont_bounds","/lonu_bounds","/longitude_bnds","/LongitudeCornerpoints","/bounds_lon","/w_stag"};
  int var_cpy_nbr=0; /* [nbr] Number of copied variables */
  int var_rgr_nbr=0; /* [nbr] Number of regridded variables */
  int var_xcl_nbr=0; /* [nbr] Number of deleted variables */
  int var_crt_nbr=0; /* [nbr] Number of created variables */
  int var_xtn_nbr=0; /* [nbr] Number of extensive variables */
  unsigned int idx_tbl; /* [idx] Counter for traversal table */
  const unsigned int trv_nbr=trv_tbl->nbr; /* [idx] Number of traversal table entries */
  for(idx=0;idx<var_xcl_lst_nbr;idx++){
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++)
      if(!strcmp(trv_tbl->lst[idx_tbl].nm_fll,var_xcl_lst[idx])) break;
    if(idx_tbl < trv_nbr){
      if(trv_tbl->lst[idx_tbl].flg_xtr){
	if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO automatically omitting (not copying or regridding from input) pre-defined exclusion-list variable %s\n",nco_prg_nm_get(),trv_tbl->lst[idx_tbl].nm_fll);
	var_xcl_nbr++;
      } /* endif */
      trv_tbl->lst[idx_tbl].flg_xtr=False;
    } /* endif */
  } /* !idx */

  cnv_sct *cnv; /* [sct] Convention structure */
  /* Determine conventions (ARM/CCM/CCSM/CF/MPAS) for treating file */
  cnv=nco_cnv_ini(in_id);
  if(cnv->MPAS){
    /* 20160228: MPAS has a host of mysterious grid and extensive variables that should probably not be regridded
       20180206: Add from MPAS-LI xCell, yCell, zCell, and [xyz]Edge, and [xyz]Vertex
       20180917: Restrict exclusion list to a subset of variables with nCells-dimension
                 Six nCells-variables may be valuable when regridded to lat/lon
		 mpas_xcl_lst in nco_rgr_wgt() and MPAS var_xcl_lst in nco_var_is_fix() differ by these six variables:
		 areaCell for comparison to area(lat,lon)
		 cellMask for area-weighted mask
		 maxLevelCell for area-weighted underwater topographic mask
		 xCell, yCell, zCell for area-weighted cartesian coordinates
       20180918: Regridder currently only works on cell-based coordinates
		 Decided regridder will omit not copy fields on vertex- or edge-based coordinates until it can regrid them
		 Regridding vertex- or edge-based fields would require new sparse matrix for vertices or edges
		 How would ERWG or TempestRemap handle that?
                 MPAS geophysical variables on vertex-based (not cell-based) coordinates include:
                 avg_airStressVertexUGeo_1, avg_airStressVertexVGeo_1, uOceanVelocityVertexGeo_1, uVelocityGeo_1, vOceanVelocityVertexGeo_1, vVelocityGeo_1
		 MPAS geophysical variables on edge-based (not cell-based) coordinates include:
		 principalStress1Var_1, principalStress2Var_1 */
    const int mpas_xcl_lst_nbr=35;
    const char *mpas_xcl_lst[]={"/angleEdge","/areaTriangle","/cellsOnCell","/cellsOnEdge","/cellsOnVertex","/dcEdge","/dvEdge","/edgeMask","/edgesOnCell","/edgesOnEdge","/edgesOnVertex","/indexToCellID","/indexToEdgeID","/indexToVertexID","/kiteAreasOnVertex","/latCell","/latEdge","/latVertex","/lonCell","/lonEdge","/lonVertex","/maxLevelEdgeTop","/meshDensity","/nEdgesOnCell","/nEdgesOnEdge","/vertexMask","/verticesOnCell","/verticesOnEdge","/weightsOnEdge","/xEdge","/yEdge","/zEdge","/xVertex","/yVertex","/zVertex"};
    for(idx=0;idx<mpas_xcl_lst_nbr;idx++){
      for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++)
	if(!strcmp(trv_tbl->lst[idx_tbl].nm_fll,mpas_xcl_lst[idx])) break;
      if(idx_tbl < trv_nbr){
	if(trv_tbl->lst[idx_tbl].flg_xtr){
	  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO automatically omitting (not copying or regridding from input) pre-defined MPAS exclusion-list variable %s\n",nco_prg_nm_get(),trv_tbl->lst[idx_tbl].nm_fll);
	  var_xcl_nbr++;
	} /* endif */
	trv_tbl->lst[idx_tbl].flg_xtr=False;
      } /* endif */
    } /* !idx */
  } /* !MPAS */
  
  char *dmn_nm_cp; /* [sng] Dimension name as char * to reduce indirection */
  int dmn_nbr_in; /* [nbr] Number of dimensions in input variable */
  int dmn_nbr_out; /* [nbr] Number of dimensions in output variable */
  nco_bool has_lon; /* [flg] Contains longitude dimension */
  nco_bool has_lat; /* [flg] Contains latitude dimension */
  trv_sct trv; /* [sct] Traversal table object structure to reduce indirection */
  /* Define regridding flag for each variable */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    dmn_nbr_in=trv_tbl->lst[idx_tbl].nbr_dmn;
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      has_lon=False;
      has_lat=False;
      if(flg_grd_in_2D){
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	  /* Pre-determine flags necessary during next loop */
	  dmn_nm_cp=trv.var_dmn[dmn_idx].dmn_nm;
	  /* fxm: Generalize to include any variable containing two coordinates with "standard_name" = "latitude" and "longitude" */
	  if(!has_lon) has_lon=!strcmp(dmn_nm_cp,lon_nm_in);
	  if(!has_lat) has_lat=!strcmp(dmn_nm_cp,lat_nm_in);
	} /* end loop over dimensions */
      } /* !flg_grd_in_2D */
      for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	dmn_nm_cp=trv.var_dmn[dmn_idx].dmn_nm;
	/* Regrid variables containing the horizontal spatial dimension on 1D grids, and both latitude and longitude on 2D grids */
	if(!strcmp(dmn_nm_cp,col_nm_in) || (has_lon && has_lat)){
	  trv_tbl->lst[idx_tbl].flg_rgr=True;
	  var_rgr_nbr++;
	  break;
	} /* endif */
      } /* end loop over dimensions */
      if(dmn_idx == dmn_nbr_in){
	/* Not regridded, so must be omitted or copied... */
	if(flg_grd_in_2D && (has_lon || has_lat)){
	/* Single spatial dimensional variables on 2D input grids are likely extensive (e.g., grd_mrd_lng from bds)
	   These could be salvaged with explicit rules or implicit assumptions */
	  trv_tbl->lst[idx_tbl].flg_xtr=False;
	  var_xcl_nbr++;
	  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO automatically omitting (not copying or regridding from input) extensive-seeming (e.g., 1D spatial variable in 2D input grid, or 2D spatial variable without primary grid dimensions from multi-grid file (e.g., west_east_stag or south_north_stag instead of west_east or south_north)) variable %s\n",nco_prg_nm_get(),trv_tbl->lst[idx_tbl].nm_fll);
	}else{ /* !omitted */
	  /* Copy all variables that are not regridded or omitted */
	  var_cpy_nbr++;
	} /* !omitted */
      } /* endif not regridded */
    } /* end nco_obj_typ_var */
  } /* end idx_tbl */
  if(!var_rgr_nbr) (void)fprintf(stdout,"%s: WARNING %s reports no variables fit regridding criteria. The regridder expects something to regrid, and variables not regridded are copied straight to output. HINT: If the name(s) of the input horizontal spatial dimensions to be regridded (e.g., latitude and longitude or column) do not match NCO's preset defaults (case-insensitive unambiguous forms and abbreviations of \"latitude\", \"longitude\", and \"ncol\", respectively) then change the dimension names that NCO looks for. Instructions are at http://nco.sf.net/nco.html#regrid, e.g., \"ncks --rgr col=lndgrid --rgr lat=north\" or \"ncremap -R '--rgr col=lndgrid --rgr lat=north'\".\n",nco_prg_nm_get(),fnc_nm);
  
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    if(trv.flg_rgr){
      for(int xtn_idx=0;xtn_idx<rgr->xtn_nbr;xtn_idx++){
	/* 20150927: Extensive variable treatments are still in alpha-development
	   Currently testing on AIRS TSurfStd_ct (by summing not averaging)
	   In future may consider variables that need more complex (non-summing) extensive treatment
	   MPAS-O/I has a zillion of these [xyz]Cell, cellsOnCell, fCell, indexToCellID, maxLevelCell, meshDensity
	   Not to mention the variables that depend on nEdges and nVertices... */
        if(!strcmp(trv.nm,rgr->xtn_var[xtn_idx])){
          trv_tbl->lst[idx_tbl].flg_xtn=True;
	  var_xtn_nbr++;
	  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: INFO Variable %s will be treated as extensive (summed not averaged)\n",nco_prg_nm_get(),trv.nm_fll);
	} /* !strcmp */
      } /* !xtn_idx */
    } /* !flg_rgr */
  } /* !idx_tbl */
    
  if(nco_dbg_lvl_get() >= nco_dbg_sbr){
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
      trv=trv_tbl->lst[idx_tbl];
      if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr) (void)fprintf(stderr,"Regrid %s? %s\n",trv.nm,trv.flg_rgr ? "Yes" : "No");
    } /* end idx_tbl */
  } /* end dbg */

  /* Lay-out regridded file */
  aed_sct aed_mtd;
  char *area_nm_out;
  char *att_nm;
  char *bnd_nm_out;
  char *bnd_tm_nm_out;
  char *col_nm_out;
  char *frc_nm_out;
  char *lat_bnd_nm_out;
  char *lat_dmn_nm_out;
  char *lat_nm_out;
  char *lat_wgt_nm;
  char *lon_bnd_nm_out;
  char *lon_dmn_nm_out;
  char *lon_nm_out;
  char *msk_nm_out;
  char *slat_nm_out=NULL;
  char *slat_wgt_nm_out=NULL;
  char *slon_nm_out=NULL;
  int dmn_id_bnd; /* [id] Dimension ID */
  int dmn_id_bnd_tm; /* [id] Dimension ID */
  int dmn_id_slat; /* [id] Dimension ID */
  int dmn_id_slon; /* [id] Dimension ID */
  int area_out_id; /* [id] Variable ID for area */
  int frc_out_id; /* [id] Variable ID for fraction */
  int lon_out_id; /* [id] Variable ID for longitude */
  int lat_out_id; /* [id] Variable ID for latitude */
  int lat_wgt_id; /* [id] Variable ID for latitude weight */
  int lon_bnd_id; /* [id] Variable ID for lon_bnds/lon_vertices */
  int lat_bnd_id; /* [id] Variable ID for lat_bnds/lat_vertices */
  int msk_out_id; /* [id] Variable ID for mask */
  int slat_out_id; /* [id] Variable ID for staggered latitude */
  int slat_wgt_id; /* [id] Variable ID for staggered latitude weight */
  int slon_out_id; /* [id] Variable ID for staggered longitude */
  int dmn_ids_out[dmn_nbr_grd_max]; /* [id] Dimension IDs array for output variable */
  long dmn_srt_out[dmn_nbr_grd_max];
  long dmn_cnt_tuo[dmn_nbr_grd_max];

  /* Name output dimensions/variables */
  area_nm_out=rgr->area_nm;
  bnd_tm_nm_out=rgr->bnd_tm_nm;
  frc_nm_out=rgr->frc_nm;
  lat_bnd_nm_out=rgr->lat_bnd_nm;
  lat_wgt_nm=rgr->lat_wgt_nm;
  lon_bnd_nm_out=rgr->lon_bnd_nm;
  msk_nm_out=rgr->msk_nm;
  /* Use explicitly specified output names, if any, otherwise use input names (either explicitly specified or discovered by fuzzing) */
  if(rgr->col_nm_out) col_nm_out=rgr->col_nm_out; else col_nm_out=col_nm_in;
  if(rgr->lat_dmn_nm) lat_dmn_nm_out=rgr->lat_dmn_nm; else lat_dmn_nm_out=lat_nm_in;
  if(rgr->lon_dmn_nm) lon_dmn_nm_out=rgr->lon_dmn_nm; else lon_dmn_nm_out=lon_nm_in;
  if(rgr->lat_nm_out) lat_nm_out=rgr->lat_nm_out; else lat_nm_out=lat_nm_in;
  if(rgr->lon_nm_out) lon_nm_out=rgr->lon_nm_out; else lon_nm_out=lon_nm_in;
  if(flg_grd_out_1D){
    bnd_nm_out=rgr->vrt_nm;
    lat_bnd_nm_out=rgr->lat_vrt_nm;
    lon_bnd_nm_out=rgr->lon_vrt_nm;
  } /* !flg_grd_out_1D */
  if(flg_grd_out_crv){
    bnd_nm_out=rgr->bnd_nm;
  } /* !flg_grd_out_crv */
  if(flg_grd_out_rct){
    bnd_nm_out=rgr->bnd_tm_nm; /* NB: default to bnd_tm_nm for spatial bounds */
  } /* !flg_grd_out_rct */
  if(flg_grd_out_2D){
    lat_bnd_nm_out=rgr->lat_bnd_nm;
    lon_bnd_nm_out=rgr->lon_bnd_nm;
  } /* !flg_grd_out_2D */
  if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
    slat_nm_out=strdup("slat");
    slat_wgt_nm_out=strdup("w_stag");
    slon_nm_out=strdup("slon");
  } /* !nco_grd_lat_fv */
  /* Ensure temporal bounds dimension name is distinct from spatial bounds when their sizes differ */
  if(bnd_nbr_out != bnd_tm_nbr_out){
    if(!strcmp(bnd_nm_out,bnd_tm_nm_out)){
      (void)fprintf(stdout,"%s: INFO %s reports spatial and temporal output bounds dimensions are identical (and named \"%s\") by default for rectangular output grids because both can be stored as 2D arrays. That cannot work for this mapping because temporal and spatial bounds dimensions sizes differ (bnd_nbr_out = %d, bnd_tm_nbr_out = %d). Using fall-back spatial bounds name \"%s\" instead. HINT: You may change one or both manually with \"ncks --rgr bnd_nm=name\" or \"ncks --rgr bnd_tm_nm=name\", or, using ncremap, with \"ncremap -R '--rgr bnd_nm=name'\" or \"ncremap -R '--rgr bnd_tm_nm=name'\"\n",nco_prg_nm_get(),fnc_nm,bnd_tm_nm_out,bnd_nbr_out,bnd_tm_nbr_out,bnd_nm_out);
    } /* !strcmp() */
  } /* !bnd_nbr_out */

  /* Persistent metadata */
  aed_sct aed_mtd_crd;
  char *att_val_crd=NULL;
  char *att_nm_crd=NULL;
  att_nm_crd=strdup("coordinates");
  aed_mtd_crd.att_nm=att_nm_crd;
  if(flg_grd_out_1D || flg_grd_out_crv) aed_mtd_crd.mode=aed_overwrite; else aed_mtd_crd.mode=aed_delete;
  aed_mtd_crd.type=NC_CHAR;
  aed_mtd_crd.sz=strlen(lat_nm_out)+strlen(lon_nm_out)+1L;
  att_val_crd=(char *)nco_malloc((aed_mtd_crd.sz+1L)*nco_typ_lng(aed_mtd_crd.type));
  (void)sprintf(att_val_crd,"%s %s",lat_nm_out,lon_nm_out);
  aed_mtd_crd.val.cp=att_val_crd;

  /* Reminder: 
     Regridder area_out options, e.g., --rgr area_out, set flg_area_out to control adding "area" variable to regridded output
     Regridder cll_msr options, --rgr cll_msr, set flg_cll_msr to control adding "cell_measures" attribute to regridded output
     ncks & ncra cll_msr options, --cll_msr, set EXTRACT_CLL_MSR to control adding "cell_measures" variables (e.g., area) to extraction list of input file
     EXTRACT_CLL_MSR supercedes --rgr area_out in determining whether to add "area" to regridded output */
  nco_bool flg_area_out=rgr->flg_area_out; /* [flg] Add area to output */
  nco_bool flg_cll_msr=rgr->flg_cll_msr; /* [flg] Add cell_measures attribute */
  aed_sct aed_mtd_cll_msr;
  char *att_nm_cll_msr=NULL;
  char *att_val_cll_msr=NULL;
  if(flg_cll_msr){
    att_nm_cll_msr=strdup("cell_measures");
    aed_mtd_cll_msr.att_nm=att_nm_cll_msr;
    aed_mtd_cll_msr.mode=aed_overwrite;
    aed_mtd_cll_msr.type=NC_CHAR;
    att_val_cll_msr=(char *)nco_malloc((strlen(area_nm_out)+6L+1L)*nco_typ_lng(aed_mtd_cll_msr.type));
    (void)sprintf(att_val_cll_msr,"area: %s",area_nm_out);
    aed_mtd_cll_msr.sz=strlen(att_val_cll_msr);
    aed_mtd_cll_msr.val.cp=att_val_cll_msr;
  } /* !flg_cll_msr */
  
  /* Define new horizontal dimensions before all else */
  if(flg_grd_out_1D){
    rcd+=nco_def_dim(out_id,col_nm_out,col_nbr_out,&dmn_id_col);
  } /* !flg_grd_out_1D */
  if(flg_grd_out_2D){
    rcd+=nco_def_dim(out_id,lat_dmn_nm_out,lat_nbr_out,&dmn_id_lat);
    rcd+=nco_def_dim(out_id,lon_dmn_nm_out,lon_nbr_out,&dmn_id_lon);
    if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
      rcd+=nco_def_dim(out_id,slat_nm_out,slat_nbr_out,&dmn_id_slat);
      rcd+=nco_def_dim(out_id,slon_nm_out,slon_nbr_out,&dmn_id_slon);
    } /* !nco_grd_lat_fv */
  } /* !flg_grd_out_2D */
  /* If dimension has not been defined, define it */
  rcd=nco_inq_dimid_flg(out_id,bnd_tm_nm_out,&dmn_id_bnd_tm);
  if(rcd != NC_NOERR) rcd=nco_def_dim(out_id,bnd_tm_nm_out,bnd_tm_nbr_out,&dmn_id_bnd_tm);
  /* If dimension has not been defined, define it */
  rcd=nco_inq_dimid_flg(out_id,bnd_nm_out,&dmn_id_bnd);
  if(rcd != NC_NOERR) rcd=nco_def_dim(out_id,bnd_nm_out,bnd_nbr_out,&dmn_id_bnd);

  char dmn_nm[NC_MAX_NAME]; /* [sng] Dimension name */
  char *var_nm; /* [sng] Variable name */
  int *dmn_id_in=NULL; /* [id] Dimension IDs */
  int *dmn_id_out=NULL; /* [id] Dimension IDs */
  int var_id_in; /* [id] Variable ID */
  int var_id_out; /* [id] Variable ID */
  nc_type var_typ_out; /* [enm] Variable type to write to disk */
  nc_type var_typ_rgr; /* [enm] Variable type used during regridding */
  nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */

  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */
  deflate=(int)True;
  shuffle=NC_SHUFFLE;
  dfl_lvl=rgr->dfl_lvl;
  fl_out_fmt=rgr->fl_out_fmt;

  /* Define new coordinates and grid variables in regridded file */
  if(flg_grd_out_1D){
    rcd+=nco_def_var(out_id,lat_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_col,&lat_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_out_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lon_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_col,&lon_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_out_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    dmn_ids_out[0]=dmn_id_col;
    dmn_ids_out[1]=dmn_id_bnd;
    rcd+=nco_def_var(out_id,lat_bnd_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lat_bnd_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_bnd_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    dmn_ids_out[0]=dmn_id_col;
    dmn_ids_out[1]=dmn_id_bnd;
    rcd+=nco_def_var(out_id,lon_bnd_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lon_bnd_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_bnd_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    if(flg_area_out){
      rcd+=nco_def_var(out_id,area_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_col,&area_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_area_out */
    if(flg_frc_out_wrt){
      rcd+=nco_def_var(out_id,frc_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_col,&frc_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,frc_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_frc_out_wrt */
    if(flg_msk_out){
      rcd+=nco_def_var(out_id,msk_nm_out,(nc_type)NC_INT,dmn_nbr_1D,&dmn_id_col,&msk_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,msk_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_msk_out */
  } /* !flg_grd_out_1D */
  if(flg_grd_out_crv){
    dmn_ids_out[0]=dmn_id_lat;
    dmn_ids_out[1]=dmn_id_lon;
    rcd+=nco_def_var(out_id,lat_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lat_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_out_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lon_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lon_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_out_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    if(flg_area_out){
      rcd+=nco_def_var(out_id,area_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&area_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_area_out */
    if(flg_frc_out_wrt){
      rcd+=nco_def_var(out_id,frc_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&frc_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,frc_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_frc_out_wrt */
    if(flg_msk_out){
      rcd+=nco_def_var(out_id,msk_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&msk_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,msk_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_msk_out */
    dmn_ids_out[0]=dmn_id_lat;
    dmn_ids_out[1]=dmn_id_lon;
    dmn_ids_out[2]=dmn_id_bnd;
    rcd+=nco_def_var(out_id,lat_bnd_nm_out,crd_typ_out,dmn_nbr_3D,dmn_ids_out,&lat_bnd_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_bnd_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lon_bnd_nm_out,crd_typ_out,dmn_nbr_3D,dmn_ids_out,&lon_bnd_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_bnd_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
  } /* !flg_grd_out_crv */
  if(flg_grd_out_rct){
    rcd+=nco_def_var(out_id,lat_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_lat,&lat_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_out_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lon_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_lon,&lon_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_out_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
      rcd+=nco_def_var(out_id,slat_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_slat,&slat_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,slat_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
      rcd+=nco_def_var(out_id,slat_wgt_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_slat,&slat_wgt_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,slat_wgt_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
      rcd+=nco_def_var(out_id,slon_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_slon,&slon_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,slon_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !nco_grd_lat_fv */
    dmn_ids_out[0]=dmn_id_lat;
    dmn_ids_out[1]=dmn_id_bnd;
    rcd+=nco_def_var(out_id,lat_bnd_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lat_bnd_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_bnd_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    dmn_ids_out[0]=dmn_id_lon;
    dmn_ids_out[1]=dmn_id_bnd;
    rcd+=nco_def_var(out_id,lon_bnd_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lon_bnd_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_bnd_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lat_wgt_nm,crd_typ_out,dmn_nbr_1D,&dmn_id_lat,&lat_wgt_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_wgt_id,shuffle,deflate,dfl_lvl);
    var_crt_nbr++;
    dmn_ids_out[0]=dmn_id_lat;
    dmn_ids_out[1]=dmn_id_lon;
    if(flg_area_out){
      rcd+=nco_def_var(out_id,area_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&area_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_area_out */
    if(flg_frc_out_wrt){
      rcd+=nco_def_var(out_id,frc_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&frc_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,frc_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_frc_out_wrt */
    if(flg_msk_out){
      rcd+=nco_def_var(out_id,msk_nm_out,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&msk_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,msk_out_id,shuffle,deflate,dfl_lvl);
      var_crt_nbr++;
    } /* !flg_msk_out */
  } /* !flg_grd_out_rct */

  /* Pre-allocate dimension ID and cnt/srt space */
  int dmn_nbr_max; /* [nbr] Maximum number of dimensions variable can have in input or output */
  int dmn_in_fst; /* [idx] Offset of input- relative to output-dimension due to non-MRV dimension insertion */
  int dmn_nbr_rec; /* [nbr] Number of unlimited dimensions */
  int *dmn_ids_rec=NULL; /* [id] Unlimited dimension IDs */
  rcd+=nco_inq_ndims(in_id,&dmn_nbr_max);
  dmn_nbr_max++; /* Safety in case regridding adds dimension */
  dmn_id_in=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  dmn_id_out=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
  dmn_cnt=(long *)nco_malloc(dmn_nbr_max*sizeof(long));

  /* Identify all record-dimensions in input file */
  rcd+=nco_inq_unlimdims(in_id,&dmn_nbr_rec,dmn_ids_rec);
  if(dmn_nbr_rec > 0){
    dmn_ids_rec=(int *)nco_malloc(dmn_nbr_rec*sizeof(int));
    rcd+=nco_inq_unlimdims(in_id,&dmn_nbr_rec,dmn_ids_rec);
  } /* !dmn_nbr_rec */
  
  int flg_pck; /* [flg] Variable is packed on disk  */
  nco_bool has_mss_val; /* [flg] Has numeric missing value attribute */
  double mss_val_dbl;
  /* Define regridded and copied variables in output file */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv_tbl->lst[idx_tbl].flg_mrv=True;
    trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      var_nm=trv.nm;
      /* Preserve input type in output type */
      var_typ_out=trv.var_typ;
      /* Demote DP to SP to save space. fxm: missing value type will then be inconsistent if copied without demotion */
      //if(trv.var_typ == NC_DOUBLE) var_typ_out=NC_FLOAT; else var_typ_out=trv.var_typ;
      dmn_nbr_in=trv.nbr_dmn;
      dmn_nbr_out=trv.nbr_dmn;
      rcd=nco_inq_varid(in_id,var_nm,&var_id_in);
      rcd=nco_inq_varid_flg(out_id,var_nm,&var_id_out);
      /* If variable has not been defined, define it */
      if(rcd != NC_NOERR){
	if(trv.flg_rgr){
	  /* Regrid */
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_id_in);
	  dmn_in_fst=0;
	  rcd=nco_inq_var_packing(in_id,var_id_in,&flg_pck);
	  if(flg_pck) (void)fprintf(stdout,"%s: WARNING %s reports variable \"%s\" is packed so results unpredictable. HINT: If regridded values seems weird, retry after unpacking input file with, e.g., \"ncpdq -U in.nc out.nc\"\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  has_mss_val=nco_mss_val_get_dbl(in_id,var_id_in,(double *)NULL);
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    rcd=nco_inq_dimname(in_id,dmn_id_in[dmn_idx],dmn_nm);
	    /* Is horizontal dimension last, i.e., most-rapidly-varying? */
	    if(flg_grd_in_1D && !strcmp(dmn_nm,col_nm_in)){
	      if(dmn_idx != dmn_nbr_in-1){
		/* Unstructured input grid has col in non-MRV location (expect this with, e.g., MPAS-O/I native grid dimension-ordering */
		(void)fprintf(stdout,"%s: WARNING %s reports unstructured grid spatial coordinate %s is (zero-based) dimension %d of input variable to be regridded %s which has %d dimensions. The NCO regridder does not support unstructured spatial dimensions that are not the last (i.e., most rapidly varying) dimension of an input variable, so results are likely garbage.\nHINT: Re-arrange input file dimensions to place horizontal dimension(s) last with, e.g., \'ncpdq -a time,lev,%s in.nc out.nc\' prior to calling the regridder. E3SM users: If this is an MPAS dataset with a new (unknown to ncremap) dimension, please ask Charlie to add the dimension to the ncremap dimension permutation list.\n",nco_prg_nm_get(),fnc_nm,dmn_nm,dmn_idx,var_nm,dmn_nbr_in,dmn_nm);
		trv_tbl->lst[idx_tbl].flg_mrv=False;
	      } /* !dmn_idx */
	    } /* !flg_grd_in_1D */
	    if(flg_grd_in_2D && (!strcmp(dmn_nm,lat_nm_in) || !strcmp(dmn_nm,lon_nm_in))){
	      /* Are horizontal dimensions most-rapidly-varying? */
	      if(dmn_idx != dmn_nbr_in-1 && dmn_idx != dmn_nbr_in-2){
		/* NB: Lat/lon input grid has lat/lon in non-MRV location (expect this with, e.g., AIRS L2 grid dimension-ordering */
		(void)fprintf(stdout,"%s: WARNING %s reports lat-lon grid spatial coordinate %s is (zero-based) dimension %d of input variable to be regridded %s which has %d dimensions. The NCO regridder does not support rectangular lat-lon dimension(s) that are not the last two (i.e., most rapidly varying) dimensions of an input variable, so results are likely garbage.\nHINT: Re-arrange input file dimensions to place horizontal dimensions last with, e.g., \'ncpdq -a time,lev,lat,lon in.nc out.nc\' prior to calling the regridder.\n",nco_prg_nm_get(),fnc_nm,dmn_nm,dmn_idx,var_nm,dmn_nbr_in);
		trv_tbl->lst[idx_tbl].flg_mrv=False;
	      } /* !dmn_idx */
	    } /* !flg_grd_in_2D */	      
	    if(flg_grd_out_1D){
	      if((nco_rgr_typ == nco_rgr_grd_2D_to_1D) && (!strcmp(dmn_nm,lat_nm_in) || !strcmp(dmn_nm,lon_nm_in))){
		/* Replace orthogonal horizontal dimensions by unstructured horizontal dimension already defined */
		if(!strcmp(dmn_nm,lat_nm_in)){
		  /* Replace lat with col */
		  dmn_id_out[dmn_idx]=dmn_id_col;
		  dmn_cnt[dmn_idx]=col_nbr_out;
		} /* endif lat */
		if(!strcmp(dmn_nm,lon_nm_in)){
		  /* Assume non-MRV dimensions are ordered lat/lon. Replace lat with col. Shift MRV dimensions to left after deleting lon. */
		  dmn_id_out[dmn_idx]=NC_MIN_INT;
		  dmn_cnt[dmn_idx]=NC_MIN_INT;
		  dmn_nbr_out--;
		  /* Reduce output dimension position of all subsequent input dimensions by one */
		  if(!trv_tbl->lst[idx_tbl].flg_mrv) dmn_in_fst=-1; 
		} /* endif lon */
	      }else{
		/* Dimension col_nm_in has already been defined as col_nm_out, replicate all other dimensions */
		if(!strcmp(dmn_nm,col_nm_in)) rcd=nco_inq_dimid_flg(out_id,col_nm_out,dmn_id_out+dmn_idx);
		else rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_id_out+dmn_idx+dmn_in_fst);
		if(rcd != NC_NOERR){
		  rcd=nco_inq_dimlen(in_id,dmn_id_in[dmn_idx],dmn_cnt+dmn_idx+dmn_in_fst);
		  /* Check-for and, if found, retain record dimension property */
		  for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		    if(dmn_id_in[dmn_idx] == dmn_ids_rec[dmn_rec_idx])
		      dmn_cnt[dmn_idx+dmn_in_fst]=NC_UNLIMITED;
		  rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt[dmn_idx+dmn_in_fst],dmn_id_out+dmn_idx+dmn_in_fst);
		} /* !rcd */
	      } /* !lat && !lon */
	    } /* !flg_grd_out_1D */
	    if(flg_grd_out_2D){
	      if(nco_rgr_typ == nco_rgr_grd_1D_to_2D && !strcmp(dmn_nm,col_nm_in)){
		/* Replace unstructured horizontal dimension by orthogonal horizontal dimensions already defined */
		dmn_id_out[dmn_idx]=dmn_id_lat;
		dmn_id_out[dmn_idx+1]=dmn_id_lon;
		dmn_cnt[dmn_idx]=lat_nbr_out;
		dmn_cnt[dmn_idx+1]=lon_nbr_out;
		dmn_nbr_out++;
		/* Increase output dimension position of all subsequent input dimensions by one */
		if(!trv_tbl->lst[idx_tbl].flg_mrv) dmn_in_fst=1; 
	      }else{
		/* Dimensions lat/lon_nm_in have already been defined as lat/lon_nm_out, replicate all other dimensions */
		if(!strcmp(dmn_nm,lat_nm_in)) rcd=nco_inq_dimid_flg(out_id,lat_dmn_nm_out,dmn_id_out+dmn_idx);
		else if(!strcmp(dmn_nm,lon_nm_in)) rcd=nco_inq_dimid_flg(out_id,lon_dmn_nm_out,dmn_id_out+dmn_idx);
		else rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_id_out+dmn_idx+dmn_in_fst);
		if(rcd != NC_NOERR){
		  rcd=nco_inq_dimlen(in_id,dmn_id_in[dmn_idx],dmn_cnt+dmn_idx+dmn_in_fst);
		  /* Check-for and, if found, retain record dimension property */
		  for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		    if(dmn_id_in[dmn_idx] == dmn_ids_rec[dmn_rec_idx])
		      dmn_cnt[dmn_idx+dmn_in_fst]=NC_UNLIMITED;
		  rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt[dmn_idx+dmn_in_fst],dmn_id_out+dmn_idx+dmn_in_fst);
		} /* !rcd */
	      } /* !col */
	    } /* !1D_to_2D */
	  } /* !dmn_idx */
	}else{ /* !flg_rgr */
	  /* Replicate non-regridded variables */
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_id_in);
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    rcd=nco_inq_dimname(in_id,dmn_id_in[dmn_idx],dmn_nm);
	    rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_id_out+dmn_idx);
	    if(rcd != NC_NOERR){
	      rcd=nco_inq_dimlen(in_id,dmn_id_in[dmn_idx],dmn_cnt+dmn_idx);
	      /* Check-for and, if found, retain record dimension property */
	      for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		if(dmn_id_in[dmn_idx] == dmn_ids_rec[dmn_rec_idx])
		  dmn_cnt[dmn_idx]=NC_UNLIMITED;
	      rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt[dmn_idx],dmn_id_out+dmn_idx);
	    } /* !rcd */
	  } /* !dmn_idx */
	} /* !flg_rgr */
	rcd=nco_def_var(out_id,var_nm,var_typ_out,dmn_nbr_out,dmn_id_out,&var_id_out);
	/* Duplicate netCDF4 settings when possible */
	if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC){
	  /* Deflation */
	  if(dmn_nbr_out > 0){
	    int dfl_lvl_in; /* [enm] Deflate level [0..9] */
	    rcd=nco_inq_var_deflate(in_id,var_id_in,&shuffle,&deflate,&dfl_lvl_in);
	    /* Copy original deflation settings */
	    if(deflate || shuffle) (void)nco_def_var_deflate(out_id,var_id_out,shuffle,deflate,dfl_lvl_in);
	    /* Overwrite HDF Lempel-Ziv compression level, if requested */
	    if(dfl_lvl == 0) deflate=(int)False; else deflate=(int)True;
	    /* Turn-off shuffle when uncompressing otherwise chunking requests may fail */
	    if(dfl_lvl == 0) shuffle=NC_NOSHUFFLE;
	    /* Shuffle never, to my knowledge, increases filesize, so shuffle by default when manually deflating */
	    if(dfl_lvl >= 0) shuffle=NC_SHUFFLE;
	    if(dfl_lvl >= 0) (void)nco_def_var_deflate(out_id,var_id_out,shuffle,deflate,dfl_lvl);
	  } /* !dmn_nbr_out */
	} /* !NC_FORMAT_NETCDF4 */ 
	(void)nco_att_cpy(in_id,out_id,var_id_in,var_id_out,PCK_ATT_CPY);
	if(trv.flg_rgr){
	  aed_mtd_crd.var_nm=var_nm;
	  aed_mtd_crd.id=var_id_out;
	  (void)nco_aed_prc(out_id,var_id_out,aed_mtd_crd);
	  if(flg_cll_msr){
	    aed_mtd_cll_msr.var_nm=var_nm;
	    aed_mtd_cll_msr.id=var_id_out;
	    (void)nco_aed_prc(out_id,var_id_out,aed_mtd_cll_msr);
	  } /* !flg_cll_msr */
	} /* !flg_rgr */
      } /* !rcd */
    } /* !var */
  } /* end idx_tbl */

  /* Free pre-allocated array space */
  /* col_nm_in will not otherwise be free'd if it was guessed as usual suspect */
  if(col_nm_in != rgr->col_nm_in) col_nm_in=(char *)nco_free(col_nm_in);
  if(dmn_id_in) dmn_id_in=(int *)nco_free(dmn_id_in);
  if(dmn_id_out) dmn_id_out=(int *)nco_free(dmn_id_out);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  if(dmn_cnt) dmn_cnt=(long *)nco_free(dmn_cnt);
  if(dmn_ids_rec) dmn_ids_rec=(int *)nco_free(dmn_ids_rec);

  /* Define new metadata in regridded file */
  if(flg_area_out){
    rcd=nco_char_att_put(out_id,area_nm_out,"long_name","Solid angle subtended by gridcell");
    rcd=nco_char_att_put(out_id,area_nm_out,"standard_name","solid_angle");
    rcd=nco_char_att_put(out_id,area_nm_out,"units","steradian");
    if(flg_grd_out_1D || flg_grd_out_crv) rcd=nco_char_att_put(out_id,area_nm_out,att_nm_crd,att_val_crd);
    att_val=(char *)nco_calloc((strlen(lat_dmn_nm_out)+strlen(lon_dmn_nm_out)+8L),sizeof(char));
    (void)sprintf(att_val,"%s, %s: sum",lat_dmn_nm_out,lon_dmn_nm_out);
    rcd=nco_char_att_put(out_id,area_nm_out,"cell_mathods",att_val);
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* !flg_area_out */

  if(flg_frc_out_wrt){
    rcd=nco_char_att_put(out_id,frc_nm_out,"long_name","Fraction of gridcell valid on destination grid");
    if(flg_grd_out_1D || flg_grd_out_crv) rcd=nco_char_att_put(out_id,area_nm_out,att_nm_crd,att_val_crd);
    att_val=(char *)nco_calloc((strlen(lat_dmn_nm_out)+strlen(lon_dmn_nm_out)+8L),sizeof(char));
    (void)sprintf(att_val,"%s, %s: sum",lat_dmn_nm_out,lon_dmn_nm_out);
    rcd=nco_char_att_put(out_id,frc_nm_out,"cell_mathods",att_val);
  } /* !flg_frc_out_wrt */
  
  if(flg_msk_out){
    rcd=nco_char_att_put(out_id,msk_nm_out,"long_name","Mask (0 = invalid destination, 1 = valid destination)");
    if(flg_grd_out_1D || flg_grd_out_crv) rcd=nco_char_att_put(out_id,area_nm_out,att_nm_crd,att_val_crd);
  } /* !flg_msk_out */

  rcd=nco_char_att_put(out_id,lat_nm_out,"long_name","Latitude of Grid Cell Centers");
  rcd=nco_char_att_put(out_id,lat_nm_out,"standard_name","latitude");
  rcd=nco_char_att_put(out_id,lat_nm_out,"units","degrees_north");
  // 20200205: Attach "axis" attribute to single-dimensional geospatial coordinates not to two-dimensional coordinate variables per CF Conventions section 5.2
  if(!flg_grd_out_crv) rcd=nco_char_att_put(out_id,lat_nm_out,"axis","Y");

  double vld_min;
  vld_min=-90.0;
  att_nm=strdup("valid_min");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lat_nm_out;
  aed_mtd.id=lat_out_id;
  aed_mtd.sz=1;
  aed_mtd.type=NC_DOUBLE;
  aed_mtd.val.dp=&vld_min;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lat_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);

  double vld_max;
  vld_max=90.0;
  att_nm=strdup("valid_max");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lat_nm_out;
  aed_mtd.id=lat_out_id;
  aed_mtd.sz=1;
  aed_mtd.type=NC_DOUBLE;
  aed_mtd.val.dp=&vld_max;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lat_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);

  rcd=nco_char_att_put(out_id,lat_nm_out,"bounds",lat_bnd_nm_out);
  
  if(flg_grd_out_rct) att_val=strdup("Gridcell latitude interfaces"); else att_val=strdup("Gridcell latitude vertices");
  rcd=nco_char_att_put(out_id,lat_bnd_nm_out,"long_name",att_val);

  rcd=nco_char_att_put(out_id,lon_nm_out,"long_name","Longitude of Grid Cell Centers");
  rcd=nco_char_att_put(out_id,lon_nm_out,"standard_name","longitude");
  rcd=nco_char_att_put(out_id,lon_nm_out,"units","degrees_east");
  // 20200205: Attach "axis" attribute to single-dimensional geospatial coordinates not to two-dimensional coordinate variables per CF Conventions section 5.2
  if(!flg_grd_out_crv) rcd=nco_char_att_put(out_id,lon_nm_out,"axis","X");
  /* UGRID Conventions define "topology" and "modulo" attributes 
     https://github.com/ugrid-conventions/ugrid-conventions
     My understanding is these should only be utilized for global grids */
  if(nco_rgr_typ == nco_rgr_grd_2D_to_2D){
    /* fxm: change this to check whether lon_spn >= 360 or nco_grd_xtn == global */
    att_nm=strdup("modulo");
    double modulo=360.0;
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lon_nm_out;
    aed_mtd.id=lon_out_id;
    aed_mtd.sz=1;
    aed_mtd.type=NC_DOUBLE;
    aed_mtd.val.dp=&modulo;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lon_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    
    rcd=nco_char_att_put(out_id,lon_nm_out,"topology","circular");
  } /* !nco_rgr_grd_2D_to_2D */

  if(lon_ctr_out[0] >= 0.0) vld_min=0.0; else vld_min=-180.0;
  att_nm=strdup("valid_min");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lon_nm_out;
  aed_mtd.id=lon_out_id;
  aed_mtd.sz=1;
  aed_mtd.type=NC_DOUBLE;
  aed_mtd.val.dp=&vld_min;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lon_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  
  if(lon_ctr_out[0] >= 0.0) vld_max=360.0; else vld_max=180.0;
  att_nm=strdup("valid_max");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lon_nm_out;
  aed_mtd.id=lon_out_id;
  aed_mtd.sz=1;
  aed_mtd.type=NC_DOUBLE;
  aed_mtd.val.dp=&vld_max;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lon_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);

  rcd=nco_char_att_put(out_id,lon_nm_out,"bounds",lon_bnd_nm_out);
  
  att_nm=strdup("bounds");
  att_val=lon_bnd_nm_out;
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lon_nm_out;
  aed_mtd.id=lon_out_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lon_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);

  if(flg_grd_out_rct) att_val=strdup("Gridcell longitude interfaces"); else att_val=strdup("Gridcell longitude vertices");
  rcd=nco_char_att_put(out_id,lon_bnd_nm_out,"long_name",att_val);

  if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
    rcd=nco_char_att_put(out_id,slat_nm_out,"long_name","Latitude for staggered FV grid");
    rcd=nco_char_att_put(out_id,slat_nm_out,"units","degrees_north");

    rcd=nco_char_att_put(out_id,slat_wgt_nm_out,"long_name","Latitude weights for staggered FV grid");

    rcd=nco_char_att_put(out_id,slon_nm_out,"long_name","Longitude for staggered FV grid");
    rcd=nco_char_att_put(out_id,slon_nm_out,"units","degrees_east");
  } /* !nco_grd_lat_fv */

  if(flg_grd_out_rct) rcd=nco_char_att_put(out_id,lat_wgt_nm,"long_name","Latitude quadrature weights (normalized to sum to 2.0 on global grids)");
  
  rcd=nco_char_att_put(out_id,NULL,"map_file",fl_in);
  rcd=nco_char_att_put(out_id,NULL,"input_file",rgr->fl_in);

  /* Annotate persistent metadata that should appear last in attribute list */
  if(flg_grd_out_1D){
    if(flg_area_out) rcd=nco_char_att_put(out_id,area_nm_out,att_nm_crd,att_val_crd);
    if(flg_frc_out_wrt) rcd=nco_char_att_put(out_id,frc_nm_out,att_nm_crd,att_val_crd);
    if(flg_msk_out) rcd=nco_char_att_put(out_id,msk_nm_out,att_nm_crd,att_val_crd);
  } /* !flg_grd_out_1D */
  
  /* Persistent metadata */
  if(att_nm_crd) att_nm_crd=(char *)nco_free(att_nm_crd);
  if(att_val_crd) att_val_crd=(char *)nco_free(att_val_crd);
  if(flg_cll_msr){
    if(att_nm_cll_msr) att_nm_cll_msr=(char *)nco_free(att_nm_cll_msr);
    if(att_val_cll_msr) att_val_cll_msr=(char *)nco_free(att_val_cll_msr);
  } /* !flg_cll_msr */

  if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
    if(slat_nm_out) slat_nm_out=(char *)nco_free(slat_nm_out);
    if(slat_wgt_nm_out) slat_wgt_nm_out=(char *)nco_free(slat_wgt_nm_out);
    if(slon_nm_out) slon_nm_out=(char *)nco_free(slon_nm_out);
  } /* !nco_grd_lat_fv */
  
  /* Turn-off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
      
  /* Begin data mode */
  (void)nco_enddef(out_id);

  /* Write new coordinates and variables to regridded file */
  if(flg_grd_out_1D){
    dmn_srt_out[0]=0L;
    dmn_cnt_tuo[0]=col_nbr_out;
    (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_tuo,lat_ctr_out,crd_typ_out);
    dmn_srt_out[0]=0L;
    dmn_cnt_tuo[0]=col_nbr_out;
    (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_tuo,lon_ctr_out,crd_typ_out);
    dmn_srt_out[0]=dmn_srt_out[1]=0L;
    dmn_cnt_tuo[0]=col_nbr_out;
    dmn_cnt_tuo[1]=bnd_nbr_out;
    (void)nco_put_vara(out_id,lat_bnd_id,dmn_srt_out,dmn_cnt_tuo,lat_bnd_out,crd_typ_out);
    dmn_srt_out[0]=dmn_srt_out[1]=0L;
    dmn_cnt_tuo[0]=col_nbr_out;
    dmn_cnt_tuo[1]=bnd_nbr_out;
    (void)nco_put_vara(out_id,lon_bnd_id,dmn_srt_out,dmn_cnt_tuo,lon_bnd_out,crd_typ_out);
    if(flg_area_out){
      dmn_srt_out[0]=0L;
      dmn_cnt_tuo[0]=col_nbr_out;
      (void)nco_put_vara(out_id,area_out_id,dmn_srt_out,dmn_cnt_tuo,area_out,crd_typ_out);
    } /* !flg_area_out */
    if(flg_msk_out){
      dmn_srt_out[0]=0L;
      dmn_cnt_tuo[0]=col_nbr_out;
      (void)nco_put_vara(out_id,msk_out_id,dmn_srt_out,dmn_cnt_tuo,msk_out,crd_typ_out);
    } /* !flg_msk_out */
  } /* !flg_grd_out_1D */
  if(flg_grd_out_crv){
    dmn_srt_out[0]=dmn_srt_out[1]=0L;
    dmn_cnt_tuo[0]=lat_nbr_out;
    dmn_cnt_tuo[1]=lon_nbr_out;
    (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_tuo,lat_ctr_out,crd_typ_out);
    (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_tuo,lon_ctr_out,crd_typ_out);
    if(flg_area_out){
      (void)nco_put_vara(out_id,area_out_id,dmn_srt_out,dmn_cnt_tuo,area_out,crd_typ_out);
    } /* !flg_area_out */
    if(flg_frc_out_wrt){
      (void)nco_put_vara(out_id,frc_out_id,dmn_srt_out,dmn_cnt_tuo,frc_out,crd_typ_out);
    } /* !flg_frc_out_wrt */
    if(flg_msk_out){
      (void)nco_put_vara(out_id,msk_out_id,dmn_srt_out,dmn_cnt_tuo,msk_out,crd_typ_out);
    } /* !flg_msk_out */
    dmn_srt_out[0]=dmn_srt_out[1]=dmn_srt_out[2]=0L;
    dmn_cnt_tuo[0]=lat_nbr_out;
    dmn_cnt_tuo[1]=lon_nbr_out;
    dmn_cnt_tuo[2]=bnd_nbr_out;
    /* NB: 20160803 Semantically confusing---curvilinear grids must write *_crn_out data into *_bnd_out arrays */
    (void)nco_put_vara(out_id,lat_bnd_id,dmn_srt_out,dmn_cnt_tuo,lat_crn_out,crd_typ_out);
    (void)nco_put_vara(out_id,lon_bnd_id,dmn_srt_out,dmn_cnt_tuo,lon_crn_out,crd_typ_out);
  } /* !flg_grd_out_crv */
  if(flg_grd_out_rct){
    dmn_srt_out[0]=0L;
    dmn_cnt_tuo[0]=lat_nbr_out;
    (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_tuo,lat_ctr_out,crd_typ_out);
    dmn_srt_out[0]=0L;
    dmn_cnt_tuo[0]=lon_nbr_out;
    (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_tuo,lon_ctr_out,crd_typ_out);
    if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
      dmn_srt_out[0]=0L;
      dmn_cnt_tuo[0]=slat_nbr_out;
      (void)nco_put_vara(out_id,slat_out_id,dmn_srt_out,dmn_cnt_tuo,slat_ctr_out,crd_typ_out);
      (void)nco_put_vara(out_id,slat_wgt_id,dmn_srt_out,dmn_cnt_tuo,slat_wgt_out,crd_typ_out);
      dmn_srt_out[0]=0L;
      dmn_cnt_tuo[0]=slon_nbr_out;
      (void)nco_put_vara(out_id,slon_out_id,dmn_srt_out,dmn_cnt_tuo,slon_ctr_out,crd_typ_out);
      if(slat_ctr_out) slat_ctr_out=(double *)nco_free(slat_ctr_out);
      if(slat_wgt_out) slat_wgt_out=(double *)nco_free(slat_wgt_out);
      if(slon_ctr_out) slon_ctr_out=(double *)nco_free(slon_ctr_out);
    } /* !nco_grd_lat_fv */
    dmn_srt_out[0]=0L;
    dmn_cnt_tuo[0]=lat_nbr_out;
    (void)nco_put_vara(out_id,lat_wgt_id,dmn_srt_out,dmn_cnt_tuo,lat_wgt_out,crd_typ_out);
    dmn_srt_out[0]=dmn_srt_out[1]=0L;
    dmn_cnt_tuo[0]=lat_nbr_out;
    dmn_cnt_tuo[1]=bnd_nbr_out;
    (void)nco_put_vara(out_id,lat_bnd_id,dmn_srt_out,dmn_cnt_tuo,lat_bnd_out,crd_typ_out);
    dmn_srt_out[0]=dmn_srt_out[1]=0L;
    dmn_cnt_tuo[0]=lon_nbr_out;
    dmn_cnt_tuo[1]=bnd_nbr_out;
    (void)nco_put_vara(out_id,lon_bnd_id,dmn_srt_out,dmn_cnt_tuo,lon_bnd_out,crd_typ_out);
    dmn_srt_out[0]=dmn_srt_out[1]=0L;
    dmn_cnt_tuo[0]=lat_nbr_out;
    dmn_cnt_tuo[1]=lon_nbr_out;
    if(flg_area_out){
      (void)nco_put_vara(out_id,area_out_id,dmn_srt_out,dmn_cnt_tuo,area_out,crd_typ_out);
    } /* !flg_area_out */
    if(flg_frc_out_wrt){
      (void)nco_put_vara(out_id,frc_out_id,dmn_srt_out,dmn_cnt_tuo,frc_out,crd_typ_out);
    } /* !flg_frc_out_wrt */
    if(flg_msk_out){
      (void)nco_put_vara(out_id,msk_out_id,dmn_srt_out,dmn_cnt_tuo,msk_out,crd_typ_out);
    } /* !flg_msk_out */
  } /* !flg_grd_out_rct */

  /* Regrid or copy variable values */
  const double wgt_vld_thr=rgr->wgt_vld_thr; /* [frc] Weight threshold for valid destination value */
  const nco_bool flg_rnr=rgr->flg_rnr; /* [flg] Renormalize destination values by valid area */
  char *sgs_frc_nm=NULL;
  char *sgs_msk_nm=NULL;
  double *sgs_frc_in=NULL;
  double *sgs_frc_out=NULL;
  double *var_val_dbl_in=NULL;
  double *var_val_dbl_out=NULL;
  double *wgt_vld_out=NULL;
  double var_val_crr;
  int *tally=NULL; /* [nbr] Number of valid (non-missing) values */
  int lvl_idx; /* [idx] Level index */
  int lvl_nbr; /* [nbr] Number of levels */
  int thr_idx; /* [idx] Thread index */
  size_t dst_idx; 
  size_t idx_in; /* [idx] Input grid index */
  size_t idx_out; /* [idx] Output grid index */
  size_t var_sz_in; /* [nbr] Number of elements in variable (will be self-multiplied) */
  size_t var_sz_out; /* [nbr] Number of elements in variable (will be self-multiplied) */
  size_t val_in_fst; /* [nbr] Number of elements by which current N-D slab input values are offset from origin */
  size_t val_out_fst; /* [nbr] Number of elements by which current N-D slab output values are offset from origin */
  
  /* 20190322: Prior to entering OpenMP loop, collect specified SGS information */
  const double sgs_nrm=rgr->sgs_nrm; /* [frc] Sub-gridscale normalization */
  if(rgr->sgs_frc_nm){
    /* Normalization test:
       fl_in=20181217.CNTL_CNPCTC1850_OIBGC.ne30_oECv3.edison.clm2.h0.2000-12.nc
       /bin/cp -f ${DATA}/hdf/${fl_in} ~/elm_raw.nc
       ncremap -P sgs -v FSDS,TBOT,GPP -a aave -s ${DATA}/grids/ne30np4_pentagons.091226.nc -g ${DATA}/grids/cmip6_180x360_scrip.20181001.nc ~/elm_raw.nc ~/elm_sgs.nc # Original SGS method
       ncks -A -v grid_area ${DATA}/grids/ne30np4_pentagons.091226.nc ~/elm_sgs.nc
       ncremap -P gsg -v FSDS,TBOT,GPP -m ${DATA}/maps/map_ne30np4_to_cmip6_180x360_aave.20181001.nc ~/elm_raw.nc ~/elm_gsg.nc # New SGS method */
    if(rgr->sgs_msk_nm) sgs_msk_nm=(char *)strdup(rgr->sgs_msk_nm);
    sgs_frc_nm=(char *)strdup(rgr->sgs_frc_nm);
    var_nm=sgs_frc_nm;
    var_typ_rgr=NC_DOUBLE; /* NB: Regrid in double precision */
    var_typ_out=NC_DOUBLE; /* NB: sgs_frc_out must be double precision */
    var_sz_in=1L; /* Compute from scratch to be sure it matches grd_sz_in */
    var_sz_out=grd_sz_out; /* Assume this holds */
    char *fl_sgs=NULL; /* [sng] External sub-gridscale file name */
    int sgs_id; /* [id] netCDF file ID for external sub-gridscale file */
    sgs_id=in_id;
    if((rcd=nco_inq_varid_flg(sgs_id,var_nm,&var_id_in)) != NC_NOERR){
      /* If sgs_frc_nm is not in input file then search for it in external area file */
      char *sls_ptr; /* [sng] Pointer to last slash character (' ') */
      sls_ptr=strrchr(var_nm,'/');
      if(!sls_ptr){
	(void)fprintf(stderr,"%s: ERROR %s (aka \"the regridder\") reports unable to find sgs_frc_nm = %s in current input file, and unable to identify filename (ending with slash '/') portion of that string to serve as local external file for sgs_frc input, exiting\n",nco_prg_nm_get(),fnc_nm,sgs_frc_nm);
	nco_exit(EXIT_FAILURE);
      } /* !sls_ptr */
      sgs_frc_nm=(char *)strdup(sls_ptr+1L); /* Copy variable-name portion of string */
      *sls_ptr='\0'; /* NULL-terminate filename */
      fl_sgs=(char *)strdup(var_nm);
      var_nm=sgs_frc_nm; /* NB: too tricky? */
      rcd=nco_open(fl_sgs,NC_NOWRITE,&sgs_id);
      if((rcd=nco_inq_varid_flg(sgs_id,var_nm,&var_id_in)) != NC_NOERR){
	(void)fprintf(stderr,"%s: ERROR %s (aka \"the regridder\") reports unable to find sgs_frc_nm = \"%s\" in local external file %s, exiting\n",nco_prg_nm_get(),fnc_nm,sgs_frc_nm,fl_sgs);
	nco_exit(EXIT_FAILURE);
      } /* !rcd */
      if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s obtaining sgs_frc = %s from file %s\n",nco_prg_nm_get(),fnc_nm,sgs_frc_nm,fl_sgs);
    } /* !rcd */ 
    rcd=nco_inq_varndims(sgs_id,var_id_in,&dmn_nbr_in);
    dmn_nbr_max= dmn_nbr_in > dmn_nbr_out ? dmn_nbr_in : dmn_nbr_out;
    dmn_id_in=(int *)nco_malloc(dmn_nbr_in*sizeof(int));
    dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long)); /* max() for both input and output grids */
    dmn_cnt_in=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
    rcd=nco_inq_vardimid(sgs_id,var_id_in,dmn_id_in);
    for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
      rcd=nco_inq_dimlen(sgs_id,dmn_id_in[dmn_idx],dmn_cnt_in+dmn_idx);
      var_sz_in*=dmn_cnt_in[dmn_idx];
      dmn_srt[dmn_idx]=0L;
    } /* !dmn_idx */
    if(var_sz_in != grd_sz_in){
      (void)fprintf(stdout,"%s: ERROR %s (aka \"the regridder\") requires that sgs_frc = %s be same size as spatial grid but var_sz_in = %lu != %lu = grd_sz_in\n",nco_prg_nm_get(),fnc_nm,var_nm,var_sz_in,grd_sz_in);
      nco_exit(EXIT_FAILURE);
    } /* !var_sz_in */
    /* Missing value setup (NB: ELM landfrac has _FillValue and is _FillValue where masked */
    has_mss_val=nco_mss_val_get_dbl(sgs_id,var_id_in,&mss_val_dbl);
    sgs_frc_in=(double *)nco_malloc_dbg(var_sz_in*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() sgs_frc_in value buffer");
    rcd=nco_get_vara(sgs_id,var_id_in,dmn_srt,dmn_cnt_in,sgs_frc_in,var_typ_rgr);

    /* If sgs_frc comes from external local file, close it now */
    if(fl_sgs){
      rcd=nco_close(sgs_id);
      fl_sgs=(char *)nco_free(fl_sgs);
    } /* !fl_sgs */
    
    /* Initialize output */
    sgs_frc_out=(double *)nco_malloc_dbg(grd_sz_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() sgs_frc_out value buffer");
    
    /* Initialize and regrid sgs_frc_out
       20190907: sgs_frc_in (landfrac) is _FillValue (1.0e36) for ELM datasets in all masked gridcells, and is always positive definite (never zero) in all unmasked gridcells because it it a true area. ELM sgs_frc_out is always positive definite gridcell area everywhere, with no missing values and no zero values.
       20190910: MPAS-Seaice datasets have no mask, and sgs_frc_in (timeMonthly_avg_iceAreaCell) is never (ncatted-appended) _FillValue (-9.99999979021477e+33) and is usually zero because it is time-mean area-fraction of sea ice which only exists in polar regions. MPAS-Seaice sgs_frc_out is zero in all gridcells without sea-ice.
       Regardless of input source, following blocks guarantee that sgs_frc_out is defined everywhere, is never a missing value (sgs_frc_out is zero where sgs_frc_in may have been _FillValue), and is always safe to multiply and normalize by sgs_frc_out in main regridding loop */
    for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++) sgs_frc_out[dst_idx]=0.0;
    if(!has_mss_val)
      for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
	sgs_frc_out[row_dst_adr[lnk_idx]]+=sgs_frc_in[col_src_adr[lnk_idx]]*wgt_raw[lnk_idx];

    if(has_mss_val)
      for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
	if((var_val_crr=sgs_frc_in[col_src_adr[lnk_idx]]) != mss_val_dbl)
	  sgs_frc_out[row_dst_adr[lnk_idx]]+=var_val_crr*wgt_raw[lnk_idx];

    /* Sanity check sgs_frc_out */
    if(nco_dbg_lvl_get() >= nco_dbg_fl){
      /* 20190326: sgs_frc expressed as a fraction must never exceed sgs_nrm 
	 CICE expresses sgs_frc (aice) in percent, i.e., sgs_nrm=100.0
	 Sum total value of sgs_frc (as opposed to gridcell_area) depends on grid resolution */
      for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++){
	/* 20190907: Approximate comparison because rounding causes frequent exceedances of sgs_nrm by epsilon ~ 1.0e-15 */
	if((float)sgs_frc_out[dst_idx] > sgs_nrm) (void)fprintf(stdout,"%s: INFO %s reports sgs_frc_out[%lu] = %19.15f > %g = sgs_nrm\n",nco_prg_nm_get(),fnc_nm,dst_idx,sgs_frc_out[dst_idx],sgs_nrm);
      } /* !dst_idx */
    } /* !dbg */
    //    for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++){
    //      (void)fprintf(stdout,"%s: INFO %s reports sgs_frc_out[%lu] = %19.15f\n",nco_prg_nm_get(),fnc_nm,dst_idx,sgs_frc_out[dst_idx]);
    //    } /* !dst_idx */

    if(dmn_id_in) dmn_id_in=(int *)nco_free(dmn_id_in);
    if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
    if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
  } /* !sgs_frc_nm */

  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"Regridding progress: # means regridded, ~ means copied\n");

  /* Using naked stdin/stdout/stderr in parallel region generates
     warning Copy appropriate filehandle to variable scoped as shared
     in parallel clause */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  /* OpenMP notes:
     default(none): GCC9.x does not accept this (https://github.com/nco/nco/issues/114) perhaps because of fp_stdout/stderr? Intel accepts it. 
     firstprivate(): Pointers that could be inadvertently free()'d if they lost their NULL-initialization
     private(): Almost everything else
     shared(): uggh...shared clause depends on both compiler and compiler-version
     1. All const variables are default shared for gcc >= 4.9.2,
     2. fnc_nm (only!) must be explicit shared for g++ 4.6.3 (travis)
     3. flg_rnr,fnc_nm,wgt_vld_thr must be explicit shared for icc 13.1.3 (rhea)
     4. assert() cannot be used in OpenMP blocks
     5. Good discussion of "const" variables in shared() clause here http://jakascorner.com/blog/2016/07/omp-default-none-and-const.html
     20200221: fxm Revisit default(none) in light of above article */
#ifdef __GNUG__
# define GCC_LIB_VERSION ( __GNUC__ * 100 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__ )
# if GCC_LIB_VERSION < 490
#  define GXX_OLD_OPENMP_SHARED_TREATMENT 1
# endif /* 480 */
# if GCC_LIB_VERSION >= 900
#  define GXX_WITH_OPENMP5_GPU_SUPPORT 1
# endif /* 900 */
#endif /* !__GNUC__ */
#if defined( __INTEL_COMPILER)
# pragma omp parallel for default(none) firstprivate(dmn_cnt_in,dmn_cnt_out,dmn_srt,dmn_id_in,dmn_id_out,tally,var_val_dbl_in,var_val_dbl_out,wgt_vld_out) private(dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dst_idx,has_mss_val,idx,idx_in,idx_out,idx_tbl,in_id,lnk_idx,lvl_idx,lvl_nbr,mss_val_dbl,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr,var_val_crr) shared(col_src_adr,dmn_nbr_hrz_crd,flg_frc_nrm,flg_rnr,fnc_nm,frc_out,lnk_nbr,out_id,row_dst_adr,sgs_frc_nm,sgs_frc_in,sgs_frc_out,sgs_msk_nm,wgt_raw,wgt_vld_thr)
#else /* !__INTEL_COMPILER */
# ifdef GXX_OLD_OPENMP_SHARED_TREATMENT
#  pragma omp parallel for default(none) firstprivate(dmn_cnt_in,dmn_cnt_out,dmn_srt,dmn_id_in,dmn_id_out,tally,var_val_dbl_in,var_val_dbl_out,wgt_vld_out) private(dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dst_idx,has_mss_val,idx,idx_in,idx_out,idx_tbl,in_id,lnk_idx,lvl_idx,lvl_nbr,mss_val_dbl,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr,var_val_crr) shared(col_src_adr,dmn_nbr_hrz_crd,flg_frc_nrm,fnc_nm,frc_out,lnk_nbr,out_id,row_dst_adr,sgs_frc_nm,sgs_frc_in,sgs_frc_out,sgs_msk_nm,wgt_raw)
# else /* !old g++ */
#  if defined(GXX_WITH_OPENMP5_GPU_SUPPORT) && 0
#   pragma omp target teams distribute parallel for firstprivate(dmn_cnt_in,dmn_cnt_out,dmn_srt,dmn_id_in,dmn_id_out,tally,var_val_dbl_in,var_val_dbl_out,wgt_vld_out) private(dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dst_idx,has_mss_val,idx,idx_in,idx_out,idx_tbl,in_id,lnk_idx,lvl_idx,lvl_nbr,mss_val_dbl,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr,var_val_crr) shared(col_src_adr,dmn_nbr_hrz_crd,flg_frc_nrm,frc_out,lnk_nbr,out_id,row_dst_adr,sgs_frc_nm,sgs_frc_in,sgs_frc_out,sgs_msk_nm,wgt_raw)
#  else
#   pragma omp parallel for firstprivate(dmn_cnt_in,dmn_cnt_out,dmn_srt,dmn_id_in,dmn_id_out,tally,var_val_dbl_in,var_val_dbl_out,wgt_vld_out) private(dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dst_idx,has_mss_val,idx,idx_in,idx_out,idx_tbl,in_id,lnk_idx,lvl_idx,lvl_nbr,mss_val_dbl,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr,var_val_crr) shared(col_src_adr,dmn_nbr_hrz_crd,flg_frc_nrm,frc_out,lnk_nbr,out_id,row_dst_adr,sgs_frc_nm,sgs_frc_in,sgs_frc_out,sgs_msk_nm,wgt_raw)
#  endif /* !GCC >= 9.0 */
# endif /* !GCC < 4.9 */
#endif /* !__INTEL_COMPILER */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    thr_idx=omp_get_thread_num();
    in_id=trv_tbl->in_id_arr[thr_idx];
#ifdef _OPENMP
    if(nco_dbg_lvl_get() >= nco_dbg_grp && !thr_idx && !idx_tbl) (void)fprintf(fp_stdout,"%s: INFO %s reports regrid loop uses %d thread%s\n",nco_prg_nm_get(),fnc_nm,omp_get_num_threads(),(omp_get_num_threads() > 1) ? "s" : "");
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(fp_stdout,"%s: INFO thread = %d, idx_tbl = %d, nm = %s\n",nco_prg_nm_get(),thr_idx,idx_tbl,trv.nm);
#endif /* !_OPENMP */
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(fp_stdout,"%s%s ",trv.flg_rgr ? "#" : "~",trv.nm);
      if(trv.flg_rgr){
	/* Regrid variable */
	var_nm=trv.nm;
	var_typ_rgr=NC_DOUBLE; /* NB: Perform regridding in double precision */
	var_typ_out=trv.var_typ; /* NB: Output type in file is same as input type */
	var_sz_in=1L;
	var_sz_out=1L;
	rcd=nco_inq_varid(in_id,var_nm,&var_id_in);
	rcd=nco_inq_varid(out_id,var_nm,&var_id_out);
	rcd=nco_inq_varndims(in_id,var_id_in,&dmn_nbr_in);
	rcd=nco_inq_varndims(out_id,var_id_out,&dmn_nbr_out);
	dmn_nbr_max= dmn_nbr_in > dmn_nbr_out ? dmn_nbr_in : dmn_nbr_out;
	dmn_id_in=(int *)nco_malloc(dmn_nbr_in*sizeof(int));
	dmn_id_out=(int *)nco_malloc(dmn_nbr_out*sizeof(int));
	dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long)); /* max() for both input and output grids */
	dmn_cnt_in=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
	dmn_cnt_out=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
	rcd=nco_inq_vardimid(in_id,var_id_in,dmn_id_in);
	rcd=nco_inq_vardimid(out_id,var_id_out,dmn_id_out);
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	  rcd=nco_inq_dimlen(in_id,dmn_id_in[dmn_idx],dmn_cnt_in+dmn_idx);
	  var_sz_in*=dmn_cnt_in[dmn_idx];
	  dmn_srt[dmn_idx]=0L;
	} /* !dmn_idx */

	for(dmn_idx=0;dmn_idx<dmn_nbr_out;dmn_idx++){
	  rcd=nco_inq_dimlen(out_id,dmn_id_out[dmn_idx],dmn_cnt_out+dmn_idx);
	  if(dmn_cnt_out[dmn_idx] == 0L){
	    /* No records have been written, so overwrite zero output record size with input record size */
	    char dmn_rec_nm[NC_MAX_NAME]; /* [sng] Record dimension name */
	    int dmn_rec_id_in;
	    rcd=nco_inq_dimname(out_id,dmn_id_out[dmn_idx],dmn_rec_nm);
	    rcd=nco_inq_dimid(in_id,dmn_rec_nm,&dmn_rec_id_in);
	    rcd=nco_inq_dimlen(in_id,dmn_rec_id_in,dmn_cnt_out+dmn_idx);
	  } /* !dmn_cnt_out */
	  var_sz_out*=dmn_cnt_out[dmn_idx];
	  dmn_srt[dmn_idx]=0L;
	} /* !dmn_idx */

	/* Compute number and size of non-lat/lon or non-col dimensions (e.g., level, time, species, wavelength)
	   Denote their convolution by level or 'lvl' for shorthand
	   There are lvl_nbr elements for each lat/lon or col position
	   20151011: Until today assume lat/lon and col are most-rapidly varying dimensions 
	   20151011: Until today lvl_nbr missed last non-spatial dimension for 1D output */
	lvl_nbr=1;
	/* Simple prescription of lvl_nbr works when horizontal dimension(s) is/are MRV */
	for(dmn_idx=0;dmn_idx<dmn_nbr_out-dmn_nbr_hrz_crd;dmn_idx++) lvl_nbr*=dmn_cnt_out[dmn_idx];
	/* Missing value setup */
	has_mss_val=nco_mss_val_get_dbl(in_id,var_id_in,&mss_val_dbl);

	/* Memory requirements of next four malloc's (i.e., exclusive of wgt_raw) sum to ~7*sizeof(uncompressed var) for NC_FLOAT and ~3.5*sizeof(uncompressed var) for NC_DOUBLE */
	var_val_dbl_in=(double *)nco_malloc_dbg(var_sz_in*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() input value buffer");
	var_val_dbl_out=(double *)nco_malloc_dbg(var_sz_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() output value buffer");
	if(has_mss_val) tally=(int *)nco_malloc_dbg(var_sz_out*nco_typ_lng(NC_INT),fnc_nm,"Unable to malloc() tally buffer");
	if(has_mss_val && flg_rnr) wgt_vld_out=(double *)nco_malloc_dbg(var_sz_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() output renormalization weight buffer");

	/* Initialize output */
	(void)memset(var_val_dbl_out,0,var_sz_out*nco_typ_lng(var_typ_rgr));
	if(has_mss_val) (void)memset(tally,0,var_sz_out*nco_typ_lng(NC_INT));
	if(wgt_vld_out) (void)memset(wgt_vld_out,0,var_sz_out*nco_typ_lng(var_typ_rgr));
	      
	/* Obtain input variable */
	rcd=nco_get_vara(in_id,var_id_in,dmn_srt,dmn_cnt_in,var_val_dbl_in,var_typ_rgr);
	
	/* 20150914: Intensive variables require normalization, extensive do not
	   Intensive variables (temperature, wind speed, mixing ratio) do not depend on gridcell boundaries
	   Extensive variables (population, counts, numbers of things) depend on gridcell boundaries
	   Extensive variables are the exception in models, yet are commonly used for sampling information, e.g., 
	   number of photons, number of overpasses 
	   Pass extensive variable list to NCO with, e.g., --xtn=TSurfStd_ct,...
	   20190420: Remove languishing, unfinished intensive variable code */
	  
	clock_t tm_srt; /* [us] Microseconds at start */
	clock_t tm_end; /* [us] Microseconds at end */
	float tm_drn; /* [s] Seconds elapsed */
	if(nco_dbg_lvl_get() >= nco_dbg_var) tm_srt=clock();
 
	/* This first block is for "normal" variables without sub-gridscale fractions */
	  if(!sgs_frc_out){
	  /* Apply weights */
	  if(!has_mss_val){
	    if(lvl_nbr == 1){
	      /* Weight single-level fields without missing values */
#ifdef ENABLE_GPU
# pragma omp target data map(to:col_src_adr[0:lnk_nbr],row_dst_adr[0:lnk_nbr],var_val_dbl_in[0:var_sz_in],wgt_raw[0:lnk_nbr]) map(tofrom:var_val_dbl_out[0:var_sz_out])
# pragma omp target teams distribute parallel for simd schedule(static,1)
#else /* !ENABLE_GPU */
# if ( __GNUC__ >= 8 ) || ( __clang_major__ >= 8 )
#  pragma omp simd
# endif /* !__GNUC__ */
#endif /* !ENABLE_GPU */
	      for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
		var_val_dbl_out[row_dst_adr[lnk_idx]]+=var_val_dbl_in[col_src_adr[lnk_idx]]*wgt_raw[lnk_idx];
	    }else{
	      val_in_fst=0L;
	      val_out_fst=0L;
	      /* Weight multi-level fields without missing values */
#ifdef ENABLE_GPU
# pragma omp target data map(to:col_src_adr[0:lnk_nbr],row_dst_adr[0:lnk_nbr],var_val_dbl_in[0:var_sz_in],wgt_raw[0:lnk_nbr]) map(tofrom:var_val_dbl_out[0:var_sz_out])
# pragma omp parallel for reduction(+:val_in_fst,val_out_fst)
#endif /* !ENABLE_GPU */
	      for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
		//if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(fp_stdout,"%s lvl_idx = %d val_in_fst = %li, val_out_fst = %li\n",trv.nm,lvl_idx,val_in_fst,val_out_fst);
#ifdef ENABLE_GPU
# pragma omp target teams distribute parallel for simd schedule(static,1)
#else /* !ENABLE_GPU */
# if ( __GNUC__ >= 8 ) || ( __clang_major__ >= 8 )
#  pragma omp simd
# endif /* !__GNUC__ */
#endif /* !ENABLE_GPU */
		for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
		  var_val_dbl_out[row_dst_adr[lnk_idx]+val_out_fst]+=var_val_dbl_in[col_src_adr[lnk_idx]+val_in_fst]*wgt_raw[lnk_idx];
		val_in_fst+=grd_sz_in;
		val_out_fst+=grd_sz_out;
	      } /* !lvl_idx */
	    } /* lvl_nbr > 1 */
	  }else{ /* has_mss_val */
	    if(lvl_nbr == 1){
	      /* Weight single-level fields with missing values */
	      for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		idx_in=col_src_adr[lnk_idx];
		idx_out=row_dst_adr[lnk_idx];
		if((var_val_crr=var_val_dbl_in[idx_in]) != mss_val_dbl){
		  var_val_dbl_out[idx_out]+=var_val_crr*wgt_raw[lnk_idx];
		  if(wgt_vld_out) wgt_vld_out[idx_out]+=wgt_raw[lnk_idx];
		  tally[idx_out]++;
		} /* !mss_val_dbl */
	      } /* !lnk_idx */
	    }else{ /* lvl_nbr > 1 */
	      val_in_fst=0L;
	      val_out_fst=0L;
	      /* Weight multi-level fields with missing values */
	      for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
		for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		  idx_in=col_src_adr[lnk_idx]+val_in_fst;
		  idx_out=row_dst_adr[lnk_idx]+val_out_fst;
		  if((var_val_crr=var_val_dbl_in[idx_in]) != mss_val_dbl){
		    var_val_dbl_out[idx_out]+=var_val_crr*wgt_raw[lnk_idx];
		    if(wgt_vld_out) wgt_vld_out[idx_out]+=wgt_raw[lnk_idx];
		    tally[idx_out]++;
		  } /* !mss_val_dbl */
		} /* !lnk_idx */
		val_in_fst+=grd_sz_in;
		val_out_fst+=grd_sz_out;
	      } /* !lvl_idx */
	    } /* lvl_nbr > 1 */
	  } /* !has_mss_val */
	  
	  if(!has_mss_val){
	      /* frc_dst = frc_out = dst_frac = frac_b contains non-unity elements and normalization type is "destarea" or "dstarea" or "none"
		 When this occurs for conservative remapping, follow "destarea" normalization procedure
		 See SCRIP manual p. 11 and http://www.earthsystemmodeling.org/esmf_releases/public/last, specifically
		 http://www.earthsystemmodeling.org/esmf_releases/public/last/ESMF_refdoc/node3.html#SECTION03029000000000000000
		 "frac_a: When a conservative regridding method is used, this contains the fraction of each source cell that participated in the regridding. When a non-conservative regridding method is used, this array is set to 0.0.
		 frac_b: When a conservative regridding method is used, this contains the fraction of each destination cell that participated in the regridding. When a non-conservative regridding method is used, this array is set to 1.0 where the point participated in the regridding (i.e. was within the unmasked source grid), and 0.0 otherwise.
		 If the first-order conservative interpolation method is specified ("-m conserve") then the destination field may need to be adjusted by the destination fraction (frac_b). This should be done if the normalization type is ``dstarea'' (sic, really "destarea") and if the destination grid extends outside the unmasked source grid. If it isn't known if the destination extends outside the source, then it doesn't hurt to apply the destination fraction. (If it doesn't extend outside, then the fraction will be 1.0 everywhere anyway.) The following code shows how to adjust an already interpolated destination field (dst_field) by the destination fraction. The variables n_b, and frac_b are from the weight file:
		 ! Adjust destination field by fraction
		 do i=1, n_b
		   if (frac_b(i) .ne. 0.0) then
		     dst_field(i)=dst_field(i)/frac_b(i)
		   endif
		 enddo"
		 NB: Non-conservative interpolation methods (e.g., bilinear) should NOT apply this normalization (theoretically there is no danger in doing so because frc_out == 1 always for all gridcells that participate in bilinear remapping and frc_out == 0 otherwise)

		 NCO's renormalization procedure below is similar to the ESMF-recommended procedure above. However, users can control NCO renormalization with, e.g., --rnr_thr=0.1, or override it completely with --rnr_thr=none. Moreover, frac_b == frc_dst is determined solely by solely by gridcell binary mask overlaps during weight generation. It is time-invariant and 2D. Missing values (e.g., AOD) can vary in time and can be 3D (or N-D) and so can wgt_vld_out. Hence NCO renormalization is more flexible. flg_frc_nrm (i.e., ESMF-recommended) normalization makes fields pretty for graphics, yet is non-conservative because e.g., MPAS Ocean gridcells projected onto global uniform grids would have their SSTs normalized for prettiness on coastal gridpoints, which is inherently non-conservative.

		 20190912: Make "ESMF renormalization" of fields without missing values (i.e., "destarea") opt-in rather than default
		 "destarea" and frac_b = frc_dst together set flg_frc_nrm
		 Formerly flg_frc_nrm triggered ESMF renormalization by default
		 Now flg_frc_nrm and user-explicitly-set --rnr_thr to [0.0,1.0] must both be true to trigger it
		 This keep conservative maps conservative by default
		 NB: This "ESMF renormalization" normalizes by frac_b == frc_dst (not by wgt_vld_out) regardless of rnr_thr

		 20151018: Avoid double-normalizing by only executing fractional normalization 
		 (flg_frc_nrm) block when !has_mss_val, and valid area normalization when has_mss_val */

	    if(flg_frc_nrm){ /* Only renormalize when frac_b < 1.0 (because frac_b == 1.0 does nothing) */
	      if(flg_rnr){ /* 20190912: Only renormalize when user explicitly requests it (because renormalization is non-conservative). Prior to today, renormalization was by default, henceforth it is opt-in. */
		if(lvl_nbr == 1){
		  /* Fractionally renormalize single-level fields without missing values */
		  for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++)
		    if(frc_out[dst_idx] != 0.0) var_val_dbl_out[dst_idx]/=frc_out[dst_idx];
		}else{
		  /* Fractionally renormalize multi-level fields without missing values */
		  for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++){
		    if(frc_out[dst_idx] != 0.0){
		      for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
			var_val_dbl_out[dst_idx+lvl_idx*grd_sz_out]/=frc_out[dst_idx];
		      } /* !lvl_idx */
		    } /* !frc_out */
		  } /* !dst_idx */
		} /* lvl_nbr > 1 */
	      } /* !flg_rnr */
	    } /* !flg_frc_nrm */
	  } /* !has_mss_val */
	  
	  if(has_mss_val){
	    /* NCL and ESMF treatment of weights and missing values described at
	       https://www.ncl.ucar.edu/Applications/ESMF.shtml#WeightsAndMasking
	       http://earthsystemmodeling.org/esmf_releases/non_public/ESMF_6_1_1/ESMF_refdoc/node5.html#SECTION05012600000000000000
	       NCO implements one of two procedures: "conservative" or "renormalized"
	       The "conservative" algorithm uses all valid data from the input grid on the output grid
	       Destination cells receive the weighted valid values of the source cells
	       This is conservative because the global integrals of the source and destination fields are equal
	       The "renormalized" algorithm divides the destination value by the sum of the valid weights
	       This returns "reasonable" values, i.e., the mean of the valid input values
	       However, renormalization is equivalent to extrapolating valid data to missing regions
	       Hence the input and output integrals are unequal and the regridding is not conservative */
	    
	    /* In fields with missing value, destination cells with no accumulated weight are missing value */
	    for(dst_idx=0;dst_idx<var_sz_out;dst_idx++)
	      if(!tally[dst_idx]) var_val_dbl_out[dst_idx]=mss_val_dbl;
	    
	    if(flg_rnr){
	      //	      if(nco_dbg_lvl_get() >= nco_dbg_quiet) (void)fprintf(fp_stdout,"%s: DEBUG renormalization for %s uses flg_rnr block\n",nco_prg_nm_get(),var_nm);
	      if(wgt_vld_thr == 0.0){
		/* Renormalize cells with no threshold by valid accumulated weight */
		for(dst_idx=0;dst_idx<var_sz_out;dst_idx++)
		  if(tally[dst_idx]) var_val_dbl_out[dst_idx]/=wgt_vld_out[dst_idx];
	      }else{
		/* Renormalize cells with threshold by valid accumulated weight if weight exceeds threshold */
		for(dst_idx=0;dst_idx<var_sz_out;dst_idx++)
		  if(wgt_vld_out[dst_idx] >= wgt_vld_thr){var_val_dbl_out[dst_idx]/=wgt_vld_out[dst_idx];}else{var_val_dbl_out[dst_idx]=mss_val_dbl;}
	      } /* !wgt_vld_thr */
	    } /* !flg_rnr */
	    
	  } /* !has_mss_val */
	} /* !sgs_frc_out */
	  
	/* Variables with sub-gridscale fractions require "double-weighting" and normalization */
	if(sgs_frc_out){
	  if(!strcmp(var_nm,sgs_frc_nm)){
	    /* Copy shared variable sgs_frc_out that was regridded before OpenMP loop
	       20190911: Reasons to copy sgs_frc_out into sgs_frc_nm data include speed, consistency, and well-definedness of sgs_frc_out. One reason to regrid sgs_frc_nm here is consistency with original, raw dataset: ELM landfrac is masked so regridding it here (rather than using sgs_frc_out) would produce a regridded dataset more identical to raw ELM output. The same can be said for CICE (I think). MPAS cellMask and timeMonthly_avg_iceAreaCell are not masked, and so should produce the same values as sgs_frc_out if regridded here. */
	    memcpy(var_val_dbl_out,sgs_frc_out,grd_sz_out*nco_typ_lng(var_typ_rgr));
	  }else if(sgs_msk_nm && !strcmp(var_nm,sgs_msk_nm)){
	    /* Compute binary mask directly from shared sgs_frc_out (guaranteed to be all valid values) */
	    for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++)
	      if(sgs_frc_out[dst_idx] != 0.0) var_val_dbl_out[dst_idx]=1.0;
	  }else{ /* !sgs_msk_nm */
	    /* "Double-weight" all other sub-gridscale input values by sgs_frc_in and overlap weight, normalize by sgs_frc_out */
	    if(!has_mss_val){
	      if(lvl_nbr == 1){
		/* SGS-regrid single-level fields without missing values */
		for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
		  var_val_dbl_out[row_dst_adr[lnk_idx]]+=var_val_dbl_in[col_src_adr[lnk_idx]]*wgt_raw[lnk_idx]*sgs_frc_in[col_src_adr[lnk_idx]];
		/* NB: MPAS-Seaice dataset sgs_frc_out is usually zero in non-polar regions */
		for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++)
		  if(sgs_frc_out[dst_idx] != 0.0) var_val_dbl_out[dst_idx]/=sgs_frc_out[dst_idx];
	      }else{ /* lvl_nbr > 1 */
		/* SGS-regrid multi-level fields without missing values */
		val_in_fst=0L;
		val_out_fst=0L;
		for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
		  for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		    idx_in=col_src_adr[lnk_idx];
		    idx_out=row_dst_adr[lnk_idx];
		    var_val_dbl_out[idx_out+val_out_fst]+=var_val_dbl_in[idx_in+val_in_fst]*wgt_raw[lnk_idx]*sgs_frc_in[idx_in];
		  } /* !lnk_idx */
		  /* Normalize current level values */
		  for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++)
		    if(sgs_frc_out[dst_idx] != 0.0) var_val_dbl_out[dst_idx+val_out_fst]/=sgs_frc_out[dst_idx];
		  val_in_fst+=grd_sz_in;
		  val_out_fst+=grd_sz_out;
		} /* !lvl_idx */
	      } /* lvl_nbr > 1 */
	    }else{ /* !has_mss_val */
	      if(lvl_nbr == 1){
		/* SGS-regrid single-level fields with missing values */
		for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		  idx_in=col_src_adr[lnk_idx];
		  idx_out=row_dst_adr[lnk_idx];
		  if((var_val_crr=var_val_dbl_in[idx_in]) != mss_val_dbl){
		    var_val_dbl_out[idx_out]+=var_val_crr*wgt_raw[lnk_idx]*sgs_frc_in[idx_in];
		    tally[idx_out]++;
		  } /* !mss_val_dbl */
		} /* !lnk_idx */
		/* NB: Normalization clause is complex to support sgs_frc_out from both ELM and MPAS-Seaice */
		for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++)
		  if(!tally[dst_idx]){var_val_dbl_out[dst_idx]=mss_val_dbl;}else{if(sgs_frc_out[dst_idx] != 0.0) var_val_dbl_out[dst_idx]/=sgs_frc_out[dst_idx];}
	      }else{ /* lvl_nbr > 1 */
		/* SGS-regrid multi-level fields with missing values */
		val_in_fst=0L;
		val_out_fst=0L;
		for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
		  for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		    idx_in=col_src_adr[lnk_idx]+val_in_fst;
		    idx_out=row_dst_adr[lnk_idx]+val_out_fst;
		    if((var_val_crr=var_val_dbl_in[idx_in]) != mss_val_dbl){
		      var_val_dbl_out[idx_out]+=var_val_crr*wgt_raw[lnk_idx]*sgs_frc_in[col_src_adr[lnk_idx]];
		      tally[idx_out]++;
		    } /* !mss_val_dbl */
		  } /* !lnk_idx */
		  /* Normalize current level values */
		  for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++){
		    idx_out=dst_idx+val_out_fst;
		    if(!tally[idx_out]){var_val_dbl_out[idx_out]=mss_val_dbl;}else{if(sgs_frc_out[dst_idx] != 0.0) var_val_dbl_out[idx_out]/=sgs_frc_out[dst_idx];}
		  } /* dst_idx */
		  val_in_fst+=grd_sz_in;
		  val_out_fst+=grd_sz_out;
		} /* !lvl_idx */
	      } /* lvl_nbr > 1 */
	    } /* !has_mss_val */
	  } /* !sgs_msk_nm */
	} /* !sgs_frc_out */

	if(nco_dbg_lvl_get() >= nco_dbg_var){
	  tm_end=clock();
	  tm_drn=(float)(tm_end-tm_srt)/CLOCKS_PER_SEC;
	  (void)fprintf(fp_stdout,"%s: INFO Compute time for %s (thread %d/%d): %g s\n",nco_prg_nm_get(),trv.nm,thr_idx,omp_get_num_threads(),tm_drn);
	} /* !dbg */

#pragma omp critical
	{ /* begin OpenMP critical */
	  //	  rcd=nco_put_var(out_id,var_id_out,var_val_dbl_out,var_typ_rgr);
	  rcd=nco_put_vara(out_id,var_id_out,dmn_srt,dmn_cnt_out,var_val_dbl_out,var_typ_rgr);
	} /* end OpenMP critical */
	
	if(dmn_id_in) dmn_id_out=(int *)nco_free(dmn_id_in);
	if(dmn_id_out) dmn_id_out=(int *)nco_free(dmn_id_out);
	if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
	if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
	if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
	if(tally) tally=(int *)nco_free(tally);
	if(var_val_dbl_out) var_val_dbl_out=(double *)nco_free(var_val_dbl_out);
	if(var_val_dbl_in) var_val_dbl_in=(double *)nco_free(var_val_dbl_in);
	if(wgt_vld_out) wgt_vld_out=(double *)nco_free(wgt_vld_out);
      }else{ /* !trv.flg_rgr */
	/* Use standard NCO copy routine for variables that are not regridded */
#pragma omp critical
	{ /* begin OpenMP critical */
	  (void)nco_cpy_var_val(in_id,out_id,(FILE *)NULL,(md5_sct *)NULL,trv.nm,trv_tbl);
	} /* end OpenMP critical */
      } /* !flg_rgr */
    } /* !xtr */
  } /* end (OpenMP parallel for) loop over idx_tbl */
  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"\n");
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s completion report: Variables regridded = %d (%d extensive), copied unmodified = %d, omitted = %d, created = %d\n",nco_prg_nm_get(),fnc_nm,var_rgr_nbr,var_xtn_nbr,var_cpy_nbr,var_xcl_nbr,var_crt_nbr);
  
  /* Free memory allocated for grid reading/writing */
  if(area_out) area_out=(double *)nco_free(area_out);
  if(col_src_adr) col_src_adr=(int *)nco_free(col_src_adr);
  if(dmn_sz_in_int) dmn_sz_in_int=(int *)nco_free(dmn_sz_in_int);
  if(dmn_sz_out_int) dmn_sz_out_int=(int *)nco_free(dmn_sz_out_int);
  if(frc_out) frc_out=(double *)nco_free(frc_out);
  if(lat_bnd_out) lat_bnd_out=(double *)nco_free(lat_bnd_out);
  if(lat_crn_out) lat_crn_out=(double *)nco_free(lat_crn_out);
  if(lat_ctr_out) lat_ctr_out=(double *)nco_free(lat_ctr_out);
  if(lat_ntf_out) lat_ntf_out=(double *)nco_free(lat_ntf_out);
  if(lat_wgt_out) lat_wgt_out=(double *)nco_free(lat_wgt_out);
  if(lon_bnd_out) lon_bnd_out=(double *)nco_free(lon_bnd_out);
  if(lon_crn_out) lon_crn_out=(double *)nco_free(lon_crn_out);
  if(lon_ctr_out) lon_ctr_out=(double *)nco_free(lon_ctr_out);
  if(lon_ntf_out) lon_ntf_out=(double *)nco_free(lon_ntf_out);
  if(msk_out) msk_out=(int *)nco_free(msk_out);
  if(row_dst_adr) row_dst_adr=(int *)nco_free(row_dst_adr);
  if(sgs_frc_nm) sgs_frc_nm=(char *)nco_free(sgs_frc_nm);
  if(sgs_frc_in) sgs_frc_in=(double *)nco_free(sgs_frc_in);
  if(sgs_frc_out) sgs_frc_out=(double *)nco_free(sgs_frc_out);
  if(sgs_msk_nm) sgs_msk_nm=(char *)nco_free(sgs_msk_nm);
  if(wgt_raw) wgt_raw=(double *)nco_free(wgt_raw);
  
  return rcd;
} /* end nco_rgr_wgt() */

void
nco_bsl_zro /* Return Bessel function zeros */
(const int bsl_zro_nbr, /* O [nbr] Order of Bessel function */
 double * const bsl_zro) /* O [frc] Bessel zero */
{
  /* Purpose: Return Bessel function zeros
     Source: CCM code /fs/cgd/csm/models/atm/ccm3.5.8/src/ccmlsm_share/bsslzr.F
     Return bsl_zro_nbr zeros (or if bsl_zro_nbr > 50, approximate zeros), of the Bessel function j0
     First 50 zeros are given exactly, and remaining zeros are computed by extrapolation, and therefore are not exact
     Original version:  CCM1
     Standardized:      J. Rosinski, June 1992
     Reviewed:          J. Hack, D. Williamson, August 1992
     Reviewed:          J. Hack, D. Williamson, April 1996
     Modified 19970123 by Jim Rosinski to use double precision arithmetic
     ~2000: Converted to Fortran9X by C. Zender, changed all real*16 statements to double precision (real*8)
     20150530: Converted to C99 by C. Zender */
  const char fnc_nm[]="nco_bsl_zro()"; /* [sng] Function name */
  const double pi=M_PI; // [frc] 3
  const double bsl_zro_tbl[]={ // Zeros of Bessel functions of order 1 to 50
    -1.e36, 2.4048255577,   5.5200781103, 
    8.6537279129,  11.7915344391,  14.9309177086,  18.0710639679, 
    21.2116366299,  24.3524715308,  27.4934791320,  30.6346064684, 
    33.7758202136,  36.9170983537,  40.0584257646,  43.1997917132, 
    46.3411883717,  49.4826098974,  52.6240518411,  55.7655107550, 
    58.9069839261,  62.0484691902,  65.1899648002,  68.3314693299, 
    71.4729816036,  74.6145006437,  77.7560256304,  80.8975558711, 
    84.0390907769,  87.1806298436,  90.3221726372,  93.4637187819, 
    96.6052679510,  99.7468198587, 102.8883742542, 106.0299309165, 
    109.1714896498, 112.3130502805, 115.4546126537, 118.5961766309, 
    121.7377420880, 124.8793089132, 128.0208770059, 131.1624462752, 
    134.3040166383, 137.4455880203, 140.5871603528, 143.7287335737, 
    146.8703076258, 150.0118824570, 153.1534580192, 156.2950342685};
  const int bsl_zro_tbl_nbr_max=50; /* [nbr] */
  int bsl_idx; /* [idx] Counting index */
    
  /* Main Code */
  if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: DEBUG Entering %s\n",nco_prg_nm_get(),fnc_nm);
    
  /* NB: Initialize bsl_zro[0] but (in C) never use it
     Initialization prevents uninitialized memory warnings */
  for(bsl_idx=0;bsl_idx<=bsl_zro_nbr;bsl_idx++)
    if(bsl_idx <= bsl_zro_tbl_nbr_max) bsl_zro[bsl_idx]=bsl_zro_tbl[bsl_idx];

  if(bsl_zro_nbr > bsl_zro_tbl_nbr_max)
    for(bsl_idx=bsl_zro_tbl_nbr_max+1;bsl_idx<=bsl_zro_nbr;bsl_idx++)
      bsl_zro[bsl_idx]=bsl_zro[bsl_idx-1]+pi;
    
  if(nco_dbg_lvl_get() == nco_dbg_old){
    (void)fprintf(stdout,"%s: DEBUG %s reports bsl_zro_nbr = %d\n",nco_prg_nm_get(),fnc_nm,bsl_zro_nbr);
    (void)fprintf(stdout,"idx\tbsl_zro\n");
    for(bsl_idx=1;bsl_idx<=bsl_zro_nbr;bsl_idx++)
      (void)fprintf(stdout,"%d\t%g\n",bsl_idx,bsl_zro[bsl_idx]);
  } /* endif dbg */

  return;
} /* end nco_bsl_zro() */

void
nco_lat_wgt_gss /* [fnc] Compute and return sine of Gaussian latitudes and their weights */
(const int lat_nbr, /* I [nbr] Latitude number */
 const nco_bool flg_s2n, /* I [enm] Latitude grid-direction is South-to-North */
 double * const lat_sin, /* O [frc] Sine of latitudes */
 double * const wgt_Gss) /* O [frc] Gaussian weights */
{  
  /* Purpose: Compute and return sine of Gaussian latitudes and their weights
     Returned arrays are ordered south-to-north (S->N), not (N->S)
     Source: CCM /fs/cgd/csm/models/atm/ccm3.5.8/src/ccmlsm_share/gauaw.F
     Calculate sine of latitudes lat_sin(lat_nbr) and weights wgt_Gss(lat_nbr) for Gaussian quadrature
     Algorithm described in Davis and Rabinowitz, Journal of Research of the NBS, V 56, Jan 1956
     Zeros of Bessel function j0, obtained from nco_bsl_zro(), are first guess for abscissae
     Original version: CCM1
     Standardized: L. Bath, Jun 1992
                   L. Buja, Feb 1996
     Reviewed:     D. Williamson, J. Hack, Aug 1992
                   D. Williamson, J. Hack, Feb 1996
     19970123 Modified by Jim Rosinski to use real*16 arithmetic in order to 
     achieve (nearly) identical weights and latitudes on all machines.
     ~2000: Converted to Fortran9X by C. Zender, changed all real*16 statements to double precision (real*8)
     20150530: Converted to C99 by C. Zender
     20150725: Verified against tabulation at http://pomax.github.io/bezierinfo/legendre-gauss.html#n64 */
  
  const char fnc_nm[]="nco_lat_wgt_gss()"; /* [sng] Function name */
  const double eps_rlt=1.0e-16; // Convergence criterion (NB: Threshold was 1.0d-27 in real*16, 1.0e-15 fine for real*8, 1.0e-16 pushes double precision to the brink)
  const double pi=M_PI; // [frc] 3
  const int itr_nbr_max=20; // [nbr] Maximum number of iterations
  double c_cff; // Constant combination coefficient
  double lat_idx_dbl; // Latitude index, double precision
  double lat_nnr_idx_dbl; // Inner latitude index, double precision
  double lat_nbr_dbl; // [nbr] Number of latitudes, double precision
  double pk=double_CEWI; // Polynomial
  double pkm1; // Polynomial
  double pkm2; // Polynomial
  double pkmrk; // Polynomial
  double sp; // Current iteration latitude increment
  double xz; // Abscissa estimate
  double cos_arg; // Intermediate parameter introduced while attempting to eliminate valgrind "uninitialised value" warnings
  int itr_cnt; // Iteration counter
  int lat_idx; // [idx] Counting index (latitude)
  int lat_sym_idx; // [idx] Counting index (symmetric latitude)
  int lat_nnr_idx; // [idx] Counting index (inner latitude loop)
  int lat_nbr_rcp2; // lat_nbr/2 (number of latitudes in hemisphere)
  double *lat_sin_p1; // Sine of Gaussian latitudes double precision
  double *wgt_Gss_p1; // Gaussian weights double precision

  /* Main Code */
  if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stdout,"%s: DEBUG Entering %s\n",nco_prg_nm_get(),fnc_nm);
    
  /* Arrays with Fortran indexing (indicated by "plus one" = "_p1") keep numerical algorithm in C identical to Fortran */
  lat_sin_p1=(double *)nco_malloc((lat_nbr+1)*sizeof(double)); // Sine of Gaussian latitudes double precision
  wgt_Gss_p1=(double *)nco_malloc((lat_nbr+1)*sizeof(double)); // Gaussian weights double precision
    
  /* Use Newton iteration to find abscissae */
  c_cff=0.25*(1.0-4.0/(pi*pi));
  lat_nbr_dbl=lat_nbr;
  lat_nbr_rcp2=lat_nbr/2; // NB: Integer arithmetic
  (void)nco_bsl_zro(lat_nbr_rcp2,lat_sin_p1);
  for(lat_idx=1;lat_idx<=lat_nbr_rcp2;lat_idx++){ // NB: Loop starts at 1
    // 20150713: Introduce intermediate parameter cos_arg in attempt to eliminate valgrind "uninitialised value" warnings emitted by cos() (actually __cos_sse())
    // Warnings occur with gcc-compiled code, not with clang-compiled code
    cos_arg=lat_sin_p1[lat_idx]/sqrt((lat_nbr_dbl+0.5)*(lat_nbr_dbl+0.5)+c_cff);
    xz=cos(cos_arg);
    /* First approximation to xz */
    itr_cnt=0;
    /* goto label_73 */
  label_73:
    pkm2=1.0;
    pkm1=xz;
    if(++itr_cnt > itr_nbr_max){
      (void)fprintf(stdout,"%s: ERROR %s reports convergence only %g after %d iterations for lat_idx = %d\n",nco_prg_nm_get(),fnc_nm,fabs(sp),itr_nbr_max,lat_idx);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    /* Compute Legendre polynomial */
    for(lat_nnr_idx=2;lat_nnr_idx<=lat_nbr;lat_nnr_idx++){
      lat_nnr_idx_dbl=lat_nnr_idx;
      pk=((2.0*lat_nnr_idx_dbl-1.0)*xz*pkm1-(lat_nnr_idx_dbl-1.0)*pkm2)/lat_nnr_idx_dbl;
      pkm2=pkm1;
      pkm1=pk;
    } /* end inner loop over lat_nnr */
    pkm1=pkm2;
    pkmrk=(lat_nbr_dbl*(pkm1-xz*pk))/(1.0-xz*xz);
    sp=pk/pkmrk;
    xz=xz-sp;
    /* NB: Easy to introduce bug here by not replacing Fortran abs() with C fabs() */
    if(fabs(sp) > eps_rlt) goto label_73;
    lat_sin_p1[lat_idx]=xz;
    wgt_Gss_p1[lat_idx]=(2.0*(1.0-xz*xz))/((lat_nbr_dbl*pkm1)*(lat_nbr_dbl*pkm1));
  } /* end outer loop over lat */
  if(lat_nbr != lat_nbr_rcp2*2){
    /* When lat_nbr is odd, compute weight at Equator */
    lat_sin_p1[lat_nbr_rcp2+1]=0.0;
    pk=2.0/(lat_nbr_dbl*lat_nbr_dbl);
    for(lat_idx=2;lat_idx<=lat_nbr;lat_idx+=2){
      lat_idx_dbl=lat_idx;
      pk=pk*lat_idx_dbl*lat_idx_dbl/((lat_idx_dbl-1.0)*(lat_idx_dbl-1.0));
    } /* end loop over lat */
    wgt_Gss_p1[lat_nbr_rcp2+1]=pk;
  } /* endif lat_nbr is odd */
    
  /* Complete sets of abscissas and weights, using symmetry properties */
  for(lat_idx=1;lat_idx<=lat_nbr_rcp2;lat_idx++){
    lat_sym_idx=lat_nbr-lat_idx+1;
    lat_sin_p1[lat_sym_idx]=-lat_sin_p1[lat_idx];
    wgt_Gss_p1[lat_sym_idx]=wgt_Gss_p1[lat_idx];
  } /* end loop over lat */
    
  /* Shift by one to remove Fortran offset in p1 arrays */
  //memcpy(lat_sin,lat_sin_p1,lat_nbr*sizeof(double));
  //memcpy(wgt_Gss,wgt_Gss_p1,lat_nbr*sizeof(double));
  
  /* Reverse and shift arrays because original CCM code algorithm computes latitudes from north-to-south
     Shift by one to remove Fortran offset in p1 arrays */
  if(flg_s2n){
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
      lat_sin[lat_idx]=lat_sin_p1[lat_nbr-lat_idx];
      wgt_Gss[lat_idx]=wgt_Gss_p1[lat_nbr-lat_idx];
    } /* end loop over lat */
  }else{
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
      lat_sin[lat_idx]=lat_sin_p1[lat_idx+1];
      wgt_Gss[lat_idx]=wgt_Gss_p1[lat_idx+1];
    } /* end loop over lat */
  } /* !flg_s2n */
    
  if(nco_dbg_lvl_get() == nco_dbg_old){
    (void)fprintf(stdout,"%s: DEBUG %s reports lat_nbr = %d\n",nco_prg_nm_get(),fnc_nm,lat_nbr);
    (void)fprintf(stdout,"idx\tasin\tngl_rad\tngl_dgr\tgw\n");
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
      (void)fprintf(stdout,"%d\t%g\t%g\t%g%g\n",lat_idx,lat_sin[lat_idx],asin(lat_sin[lat_idx]),180.0*asin(lat_sin[lat_idx])/pi,wgt_Gss[lat_idx]);
  } /* endif dbg */
  
  if(wgt_Gss_p1) wgt_Gss_p1=(double *)nco_free(wgt_Gss_p1);
  if(lat_sin_p1) lat_sin_p1=(double *)nco_free(lat_sin_p1);
  return;
} /* end nco_lat_wgt_gss() */
  
void
nco_sph_plg_area /* [fnc] Compute area of spherical polygon */
(rgr_sct * const rgr,  /* I [sct] Regridding structure */
 const double * const lat_bnd, /* [dgr] Latitude  boundaries of rectangular grid */
 const double * const lon_bnd, /* [dgr] Longitude boundaries of rectangular grid */
 const long col_nbr, /* [nbr] Number of columns in grid */
 const int bnd_nbr, /* [nbr] Number of bounds in gridcell */
 double * const area) /* [sr] Gridcell area */
{
  /* Purpose: Compute area of spherical polygon */

  /* Computing triangular area accurately is hard in corner cases
     Spherical triangle suffer from at least as many issues as planar, which are described by
     "Miscalculating Area and Angles of a Needle-like Triangle" by W. Kahan, UC Berkeley
     In particular, the Law of Cosines and Heron's formula can be ill-conditioned

     For spherical triangles L'Huilier's Theorem is superior to Girard's Formula:
     http://mathworld.wolfram.com/LHuiliersTheorem.html
     Girard's formula depends on pi-minus-angle and angle is usually quite small in our applications so precision would be lost
     L'Huilier's theorem depends only on angles (a,b,c) and semi-perimeter (s) and is well-conditioned for small angles
     semi-perimeter = half-perimeter of triangle = 0.5*(a+b+c)
     Spherical Excess (SE) difference between the sum of the angles of a spherical triangle area and a planar triangle area with same interior angles (that sum to pi)
     SE is also the solid angle subtended by the spherical triangle and that's, well, astonishing and pretty cool
     Wikipedia shows a better SE formula for triangles that are ill-conditioned for L'Huilier's formula because a = b ~ 0.5c
     https://en.wikipedia.org/wiki/Spherical_trigonometry#Area_and_spherical_excess 
     See also interesting discussion of L'Huilier by Charles Karney who suggests his own alternative:
     http://osgeo-org.1560.x6.nabble.com/Area-of-a-spherical-polygon-td3841625.html
     The discussion mentions Mil94
     Robert D. Miller, Computing the area of a spherical polygon, Graphic Gems IV, chapter II.4, pages 132-137.
     http://books.google.com/books?id=CCqzMm_-WucC&pg=PA132&lpg=PA132&dq=miller+area+spherical+polygon+gems&source=bl&ots=mrnvZ6NJcm&sig=CMg8eaD8dzP5snMaPeCQzgoFWUk&hl=sv&ei=4G-YTKv5GsWZOI-mmZQP&sa=X&oi=book_result&ct=result&resnum=1&ved=0CBQQ6AEwAA#v=onepage&q&f=false  
     Mil94 contains similar ideas to my method for spherical polygons (decomposing into adjacent multiple triangles from single vertex)
     However, his method places single vertex at pole, then adds signed areas to obtain full polygon area
     His method may suffer from degraded precision because of roundoff error and long side-lengths
     
     So-called "proper" spherical triangle are those for which all angles are less than pi, so a+b+c<3*pi
     Cartesian coordinates of (lat,lon)=(theta,phi) are (x,y,z)=(cos(theta)*cos(phi),cos(theta)*sin(phi),sin(theta)) 
     Dot-product rule for vectors gives interior angle/arc length between two points:
     cos(a)=u dot v=cos(theta1)*cos(phi1)*cos(theta2)*cos(phi2)+cos(theta1)*sin(phi1)*cos(theta2)*sin(phi2)+sin(theta1)*sin(theta2)
     Spherical law of cosines relates interior angles/arc-lengths (a,b,c) to surface angles (A,B,C) in spherical triangle:
     https://en.wikipedia.org/wiki/Spherical_law_of_cosines
     cos(a)=cos(b)*cos(c)+sin(b)*sin(c)*cos(A)
     cos(b)=cos(c)*cos(a)+sin(c)*sin(a)*cos(B)
     cos(c)=cos(a)*cos(b)+sin(a)*sin(b)*cos(C)
     cos(A)=[cos(a)-cos(b)*cos(c)]/[sin(b)*sin(c)]
     cos(B)=[cos(b)-cos(c)*cos(a)]/[sin(c)*sin(a)]
     cos(C)=[cos(c)-cos(a)*cos(b)]/[sin(a)*sin(b)]
     Bounds information on unstructured grids will use bounds_nbr=maximum(vertice_nbr)
     Unused vertices are stored as either repeated points (ACME does this) or, conceiveably, as missing values
     Given (lat,lon) for N-points algorithm to find area of spherical polygon is:
     1. Any decomposition, Girard areas: Loses precision due to mismatch between pi and small spherical excesses
        A. Find interior angles/arc-lengths (a,b,c,d...) using spherical law of cosines along each edge
        B. Apply generalized Girard formula SE_n = Sum(A_n) - (N-2) - pi
     2. CSZ decomposition (N-2 triangles) with L'Huilier areas, 
        Convert polygon into triangles by cycling spoke through all sides from common apex
        This method requires computation of N-2 (not N) triangles, though fewer sides due to optimization
	It works on all convex polygons (interior angles less than 180) but not, in general, concave polygons
	Whether it works or not on concave polygons depends upon their exact shape and the choice of apex point
        A. First three non-identical points form first triangle with sides A,B,C (first+second point define A, etc.)
	   i. First vertice anchors all triangles
	   ii. Third vertice of preceding triangle becomes second vertice of next triangle
	   iii. Next non-identical point becomes last vertice of next triangle
	   iv. Side C of previous triangle is side A of next triangle
	B. For each triangle, compute area with L'Huilier formula unless A = B ~ 0.5*C then use SAS formula
     3. centroidal decomposition, N triangle version by Taylor, L'Huilier areas: 
        Compute polygon centroid and treat this as hub from which spokes are drawn to all vertices
        This method requires computation of N triangles, though fewer sides due to optimization
	Moreover, it works on all convex polygons and on slightly concave polygons
	Centroid/hub has clear view of interior of most simple concave polygons
     4. Any decomposition but with exact RLL grids by Zender and Agress 20160918
        A. Decompose polygon into triangles via any method (e.g., method 2 or 3 above)
	B. Determine whether triangle is spherical or contains RLL (constant latitude)
	C. Spherical triangles use L'Huilier, RLL triangles use series expansion */
  const char fnc_nm[]="nco_sph_plg_area()";
  const double dgr2rdn=M_PI/180.0;
  int bnd_nbr_ttl; /* [nbr] Number of bounds in gridcell accounting for possibility of centroid information */
  long idx; /* [idx] Counting index for unrolled grids */
  short int bnd_idx;

  /* Shift to this method once we pass rgr into nco_sph_plg_area() */
  nco_bool flg_mth_csz=False; /* [flg] Use CSZ's advancing polygon bisector method */
  nco_bool flg_mth_ctr=False; /* [flg] Use centroid method to compute polygon area */
  nco_edg_typ_enm edg_typ; /* [enm] Arc-type for triangle edges */
  nco_ply_tri_mth_typ_enm ply_tri_mth; /* [enm] Polygon decomposition method */ 
  if(rgr->edg_typ == nco_edg_nil) rgr->edg_typ=nco_edg_gtc;
  edg_typ=rgr->edg_typ; /* [enm] Arc-type for triangle edges */
  ply_tri_mth=rgr->ply_tri_mth; /* [enm] Polygon decomposition method */ 
  if(ply_tri_mth == nco_ply_tri_mth_csz) flg_mth_csz=True;
  if(ply_tri_mth == nco_ply_tri_mth_ctr) flg_mth_ctr=True;
  assert(flg_mth_ctr != flg_mth_csz);
  bnd_nbr_ttl=bnd_nbr;
  // Allocate space for one extra boundary to store centroid information if necessary
  if(flg_mth_ctr) bnd_nbr_ttl=bnd_nbr+1;
  
  double *lat_bnd_rdn=NULL_CEWI; /* [rdn] Latitude  boundaries of rectangular destination grid */
  double *lon_bnd_rdn=NULL_CEWI; /* [rdn] Longitude boundaries of rectangular destination grid */
  double *lat_bnd_sin=NULL_CEWI; /* [frc] Sine of latitude  boundaries of rectangular destination grid */
  double *lon_bnd_sin=NULL_CEWI; /* [frc] Sine of longitude boundaries of rectangular destination grid */
  double *lat_bnd_cos=NULL_CEWI; /* [frc] Cosine of latitude  boundaries of rectangular destination grid */
  double *lon_bnd_cos=NULL_CEWI; /* [frc] Cosine of longitude boundaries of rectangular destination grid */
  /* Allocate one extra space for some arrays to store polygon centroid values for each column for ply_tri_mth=ctr */
  lon_bnd_rdn=(double *)nco_malloc(col_nbr*bnd_nbr_ttl*sizeof(double));
  lat_bnd_rdn=(double *)nco_malloc(col_nbr*bnd_nbr_ttl*sizeof(double));
  lon_bnd_cos=(double *)nco_malloc(col_nbr*bnd_nbr*sizeof(double));
  lat_bnd_cos=(double *)nco_malloc(col_nbr*bnd_nbr_ttl*sizeof(double));
  lon_bnd_sin=(double *)nco_malloc(col_nbr*bnd_nbr*sizeof(double));
  lat_bnd_sin=(double *)nco_malloc(col_nbr*bnd_nbr*sizeof(double));
  memcpy(lat_bnd_rdn,lat_bnd,col_nbr*bnd_nbr*sizeof(double));
  memcpy(lon_bnd_rdn,lon_bnd,col_nbr*bnd_nbr*sizeof(double));
  for(idx=0;idx<col_nbr*bnd_nbr;idx++){
    lon_bnd_rdn[idx]*=dgr2rdn;
    lat_bnd_rdn[idx]*=dgr2rdn;
    lon_bnd_cos[idx]=cos(lon_bnd_rdn[idx]);
    lat_bnd_cos[idx]=cos(lat_bnd_rdn[idx]);
    lon_bnd_sin[idx]=sin(lon_bnd_rdn[idx]);
    lat_bnd_sin[idx]=sin(lat_bnd_rdn[idx]);
  } /* !idx */
  double area_smc_crc; /* [sr] Small-circle correction to spherical triangle area */
  double area_smc; /* [sr] Gridcell area allowing for latitude-triangles */
  double area_ttl; /* [sr] Total area of input polygon list assuming spherical triangles */
  double area_smc_ttl; /* [sr] Total area of input polygon list allowing for latitude-triangles */
  double area_smc_crc_ttl; /* [sr] Latitude-triangle correction (should be small) to total area of input polygon list */
  double area_smc_crc_abs_ttl; /* [sr] Latitude-triangle absolute correction (no compensation of positive/negative contributions, should be no smaller than above) to total area of input polygon list */
  double lat_ctr; /* [dgr] Latitude of polygon centroid */
  double lon_ctr; /* [dgr] Longitude of polygon centroid */
  double lat_ctr_rdn; /* [rdn] Latitude of polygon centroid */
  double lon_ctr_rdn; /* [rdn] Longitude of polygon centroid */
  double lat_ctr_cos; /* [frc] Cosine latitude of polygon centroid */
  double lat_dlt; /* [rdn] Latitudinal difference */
  double lon_dlt; /* [rdn] Longitudinal difference */
  double ngl_a; /* [rdn] Interior angle/great circle arc a */
  double ngl_b; /* [rdn] Interior angle/great circle arc b */
  double ngl_c; /* [rdn] Interior angle/great circle arc c */
  double ngl_ltr_a; /* [rdn] Interior angle/small circle arc a, canonical latitude-triangle geometry */
  double ngl_ltr_b; /* [rdn] Interior angle/great circle arc b, canonical latitude-triangle geometry */
  double ngl_ltr_c; /* [rdn] Interior angle/great circle arc c, canonical latitude-triangle geometry */
  double prm_smi; /* [rdn] Semi-perimeter of triangle */
  double sin_hlf_tht; /* [frc] Sine of half angle/great circle arc theta connecting two points */
  double xcs_sph; /* [sr] Spherical excess */
  int tri_nbr; /* [nbr] Number of triangles in polygon */
  long bnd_vld_nbr=NC_MIN_INT; /* [idx] Number of valid (non-duplicative) vertices in each triangle */
  long *a_idx; /* [idx] Point A 1-D indices for each triangle in polygon */
  long *b_idx; /* [idx] Point B 1-D indices for each triangle in polygon */
  long *c_idx; /* [idx] Point C 1-D indices for each triangle in polygon */
  long *vrt_vld=NULL; /* [idx] Absolute 1-D indices of valid vertices */
  long idx_a; /* [idx] Point A 1-D index */
  long idx_b; /* [idx] Point B 1-D index */
  long idx_c; /* [idx] Point C 1-D index */
  nco_bool flg_sas_ndl=False; /* [flg] L'Huilier's formula will fail due to needle where one side exceeds semi-perimeter */
  nco_bool flg_sas_isc=False; /* [flg] L'Huilier's formula is ill-conditioned due to flat, near-isoceles triangle */
  nco_bool flg_sas_a=False; /* [flg] Use SAS triangle formula with central angle a */
  nco_bool flg_sas_b=False; /* [flg] Use SAS triangle formula with central angle b */
  nco_bool flg_sas_c=False; /* [flg] Use SAS triangle formula with central angle c */
  nco_bool flg_ply_has_smc; /* [flg] Any triangle in polygon has small-circle edge */
  nco_bool flg_tri_crr_smc; /* [flg] Current triangle has small_circle edge */
  /* Initialize global accumulators */
  area_ttl=0.0;
  area_smc_ttl=0.0;
  area_smc_crc_ttl=0.0;
  area_smc_crc_abs_ttl=0.0;
  for(long col_idx=0;col_idx<col_nbr;col_idx++){
    /* Initialize local properties and accumulators for this cell/polygon */
    flg_ply_has_smc=False;
    ngl_c=double_CEWI; /* Otherwise compiler unsure ngl_c is initialized first use */
    area[col_idx]=0.0;
    area_smc=0.0;
    tri_nbr=0;

    if(col_idx == 0){
      a_idx=(long *)nco_calloc(bnd_nbr,sizeof(long));
      b_idx=(long *)nco_calloc(bnd_nbr,sizeof(long));
      c_idx=(long *)nco_calloc(bnd_nbr,sizeof(long));
      vrt_vld=(long *)nco_calloc(bnd_nbr,sizeof(long));
    } /* !col_idx */
      
    /* Safety re-initialization to ease debugging, not strictly necessary */
    for(bnd_idx=0;bnd_idx<bnd_nbr;bnd_idx++){
      vrt_vld[bnd_idx]=NC_MIN_INT;
      a_idx[bnd_idx]=NC_MIN_INT;
      b_idx[bnd_idx]=NC_MIN_INT;
      c_idx[bnd_idx]=NC_MIN_INT;
    } /* !bnd_idx */

    if(flg_mth_ctr){
      double lon_dff; /* [dgr] Longitude difference */
      long bnd_srt_idx; /* [idx] Absolute starting index of vertices in polygon */
      long bnd_idx; /* [idx] Offset of current valid vertex index from starting index */
      long bnd_vld_idx; /* [idx] Absolute index of last valid vertex */
      /* First vertice is always valid */
      bnd_srt_idx=bnd_nbr*col_idx;
      bnd_vld_idx=bnd_srt_idx;
      vrt_vld[0]=bnd_vld_idx;
      lat_ctr=lat_bnd[bnd_srt_idx];
      lon_ctr=lon_bnd[bnd_srt_idx];
      bnd_vld_nbr=1;
      /* First guess for next valid index */
      bnd_idx=1;
      /* bnd_idx labels offset from first vertex of next valid (i.e., non-duplicative) vertex */
      while(bnd_idx<bnd_nbr){
	/* Skip repeated points that must occur when polygon has fewer than allowed vertices */
	while(lon_bnd[bnd_vld_idx] == lon_bnd[bnd_srt_idx+bnd_idx] && lat_bnd[bnd_srt_idx] == lat_bnd[bnd_srt_idx+bnd_idx]){
	  /* Next valid vertice must not duplicate first vertex */
	  bnd_idx++;
	  /* Have we already found all valid vertices? */
	  if(bnd_idx == bnd_nbr) break;
	} /* !while */
	/* Jump to normalization when all valid vertices found */
	if(bnd_idx == bnd_nbr) break;
	/* Current vertex is valid (non-duplicative) */
	bnd_vld_idx=bnd_srt_idx+bnd_idx;
	vrt_vld[bnd_vld_nbr]=bnd_vld_idx;
	bnd_vld_nbr++;
	if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: DEBUG %s reports centroidal decomposition col_idx=%lu, bnd_nbr=%d, bnd_idx=%ld, bnd_vld_idx=%ld, bnd_vld_nbr=%ld\n",nco_prg_nm_get(),fnc_nm,col_idx,bnd_nbr,bnd_idx,bnd_vld_idx,bnd_vld_nbr);
	assert(bnd_vld_nbr <= bnd_nbr);
	lat_ctr+=lat_bnd[bnd_vld_idx];
	lon_ctr+=lon_bnd[bnd_vld_idx];
	lon_dff=lon_bnd[bnd_vld_idx]-lon_bnd[0];
	if(lon_dff >= 180.0){
	  lon_ctr-=360.0;
	}else if(lon_dff <= -180.0){
	  lon_ctr+=360.0;
	} /* !lon_dff */
	/* Search for next valid vertice in next iteration */
	bnd_idx++;
      } /* !bnd_idx */
      /* Compute centroid */
      lat_ctr/=bnd_vld_nbr;
      lon_ctr/=bnd_vld_nbr;
      /* Centroid can become point A of bnd_nbr polygons or optimize algorithm:
	 1. Skip sub-dividing polygon into centroid-based triangles for bnd_vld_nbr == 3
	 2. Split quadrilaterals into two (non-centroid) triangles for bnd_vld_nbr == 4
	 3. Use full centroid-based triangle algorithm for bnd_vld_nbr >= 5 */
      lat_ctr_rdn=lat_ctr*dgr2rdn;
      lon_ctr_rdn=lon_ctr*dgr2rdn;
      lat_ctr_cos=cos(lat_ctr_rdn);

      /* Place centroid values in extended arrays for easy access */
      lat_bnd_rdn[(col_idx+1)*bnd_nbr_ttl-1L]=lat_ctr_rdn;
      lon_bnd_rdn[(col_idx+1)*bnd_nbr_ttl-1L]=lon_ctr_rdn;
      lat_bnd_cos[(col_idx+1)*bnd_nbr_ttl-1L]=lat_ctr_cos;
      
      /* Polygon centroid and valid vertices are now known */
      assert(bnd_vld_nbr > 2);
      if(bnd_vld_nbr == 3){
	/* Three vertices only means polygon is already decomposed into a triangle */
	tri_nbr=1;
	a_idx[0]=vrt_vld[0];
	b_idx[0]=vrt_vld[1];
	c_idx[0]=vrt_vld[2];
      }else if(bnd_vld_nbr == 4){
	/* Bisect quadrilateral into two triangles rather than use centroid and have four triantles */
	tri_nbr=2;
	a_idx[0]=vrt_vld[0];
	b_idx[0]=vrt_vld[1];
	c_idx[0]=vrt_vld[2];
	a_idx[1]=vrt_vld[0]; /* NB: Order is important. This way side C of triangle[0] = side A of trangle[1] */
	b_idx[1]=vrt_vld[2];
	c_idx[1]=vrt_vld[3];
      }else if(bnd_vld_nbr >= 5){
	/* Centroid method has as many triangles as valid vertices */
	tri_nbr=bnd_vld_nbr;
	for(int tri_idx=0;tri_idx<tri_nbr;tri_idx++){
	  a_idx[tri_idx]=(col_idx+1)*bnd_nbr_ttl-1L; /* A is always centroid, store values at end of arrays */
	  b_idx[tri_idx]=vrt_vld[tri_idx];
	  c_idx[tri_idx]=vrt_vld[(tri_idx+1)%tri_nbr];
	} /* !tri_idx */
      }	/* !bnd_vld_nbr */
    } /* !flg_mth_ctr */

    if(flg_mth_csz){
      /* A is always first vertice of all triangles */
      idx_a=bnd_nbr*col_idx; 
      /* Start search for B at next vertice */
      bnd_idx=1;
      /* bnd_idx labels offset from point A of potential location of triangle points B and C 
	 We know that bnd_idx(A) == 0, bnd_idx(B) < bnd_nbr-1, bnd_idx(C) < bnd_nbr */
      while(bnd_idx<bnd_nbr-1){
	/* Only first triangle must search for B, subsequent triangles recycle previous C as current B */
	if(tri_nbr == 0){
	  /* Skip repeated points that must occur when polygon has fewer than allowed vertices */
	  /* 20200115: Prior to today we never skipped polar points (same latitudes but different longitudes)
	     That worked fine in practice for spherical triangles partly because triangles from CSZ decomposition 
	     (aka hub-and-spoke decomposition) are additive, even with multiple points on the same great circle,
	     and partly due to luck (a starting vertex surrounded by points on the same geodesic would break it).
	     Moreover, repeated polar points pose no issues for L'Huilier's (or Girard's) method which depends
	     only on the interior angles and side lengths, not the longitudes of polar points.
	     Small circles change that last part, and we must now eliminate repeated polar points. */
	  if(edg_typ == nco_edg_smc){
	    /* Skip repeated numerically identical points */
	    while(lon_bnd[idx_a] == lon_bnd[idx_a+bnd_idx] && lat_bnd[idx_a] == lat_bnd[idx_a+bnd_idx]){
	      /* Next vertice may not duplicate A */
	      bnd_idx++;
	      /* If there is no room for C then all triangles found */
	      if(bnd_idx == bnd_nbr-1) break;
	    } /* !while */
	    /* Skip geometrically identical (i.e., repeated polar) points */
	    while((fabs(lat_bnd[idx_a]) == 90.0) && (fabs(lat_bnd[idx_a+bnd_idx]) == 90.0)){
	      bnd_idx++;
	      if(bnd_idx == bnd_nbr-1) break;
	    } /* !while */
	  }else if(edg_typ != nco_edg_smc){
	    /* Spherical polygongs can use simpler, pre-20200116 algorithm to eliminate repeated points */
	    while(lon_bnd[idx_a] == lon_bnd[idx_a+bnd_idx] && lat_bnd[idx_a] == lat_bnd[idx_a+bnd_idx]){
	      /* Next vertice may not duplicate A */
	      bnd_idx++;
	      /* If there is no room for C then all triangles found */
	      if(bnd_idx == bnd_nbr-1) break;
	    } /* !while */
	  }else{
	    abort();
	  } /* !edg_typ */
	  /* Jump to next column when all triangles found */
	  if(bnd_idx == bnd_nbr-1) break;
	} /* !tri_nbr */
	idx_b=idx_a+bnd_idx;
	/* Search for C at next vertice */
	bnd_idx++;
	/* fxm */
	while(lon_bnd[idx_b] == lon_bnd[idx_a+bnd_idx] && lat_bnd[idx_b] == lat_bnd[idx_a+bnd_idx]){
	  /* Next vertice may not duplicate B */
	  bnd_idx++;
	  /* If there is no room for C then all triangles found */
	  if(bnd_idx == bnd_nbr) break;
	} /* !while */
	/* Jump to next column when all triangles found */
	if(bnd_idx == bnd_nbr) break;
	idx_c=idx_a+bnd_idx;

	/* Valid triangle, vertices are known and labeled */
	a_idx[tri_nbr]=idx_a;
	b_idx[tri_nbr]=idx_b;
	c_idx[tri_nbr]=idx_c;
	tri_nbr++;

	/* Begin search for next B at current C */
	bnd_idx=idx_c-idx_a;
      } /* !bnd_idx */
    } /* !flg_mth_csz */
	
    /* Triangles are known for requested decomposition method 
       Compute and accumulate their area 
       Optimized algorithm recycles previous arc c as current arc a (after first triangle) */
    for(int tri_idx=0;tri_idx<tri_nbr;tri_idx++){

      idx_a=a_idx[tri_idx];
      idx_b=b_idx[tri_idx];
      idx_c=c_idx[tri_idx];

      if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: DEBUG %s reports triangle vertices: col_idx=%lu, tri_idx=%d, idx_a=%ld, idx_b=%ld, idx_c=%ld\n",nco_prg_nm_get(),fnc_nm,col_idx,tri_idx,idx_a,idx_b,idx_c);

      /* Compute interior angle/great circle arc a for first triangle; subsequent triangles recycle previous arc c */
      if(tri_idx == 0){
	/* 20150831: Test by computing ncol=0 area in conus chevrons grid, compare to MAT results
	   ncremap -s ${DATA}/grids/ne30np4_pentagons.091226.nc -g ${DATA}/grids/257x512_SCRIP.20150901.nc -m ${DATA}/maps/map_ne30np4_to_fv257x512_bilin.20150901.nc
	   ncremap -s ${DATA}/grids/257x512_SCRIP.20150901.nc -g ${DATA}/grids/conusx4v1np4_chevrons_scrip_c150815.nc -m ${DATA}/maps/map_fv257x512_to_conusx4v1np4_chevrons_bilin.20150901.nc
	   ncks -O -D 5 -v FSNT --map ${DATA}/maps/map_ne30np4_to_fv257x512_bilin.150418.nc ${DATA}/ne30/raw/famipc5_ne30_v0.3_00003.cam.h0.1979-01.nc ${DATA}/ne30/rgr/fv_FSNT.nc
	   ncks -O -D 5 -v FSNT --rgr diagnose_area --map ${DATA}/maps/map_fv257x512_to_conusx4v1np4_chevrons_bilin.20150901.nc ${DATA}/ne30/rgr/fv_FSNT.nc ${DATA}/ne30/rgr/dogfood.nc
	   ncks -O -D 1 --rgr infer#diagnose_area --rgr grid=${HOME}/grd.nc ${DATA}/ne30/rgr/dogfood.nc ~/foo.nc
	   ncks -H -s %20.15e, -v area -d ncol,0 ${DATA}/ne30/rgr/dogfood.nc
	   ncks -H -s %20.15e, -v grid_area -d grid_size,0 ${DATA}/grids/conusx4v1np4_chevrons_scrip_c150815.nc
	   ncks -H -s %20.15e, -v grid_area -d grid_size,0 ${HOME}/grd.nc
	   
	   ncol=0 on conus chevrons file:
	   3.653857995295246e-05 raw GLL weight
	   3.653857995294305e-05 ESMF weight (area_b from map-file)
	   3.653857995294302e-05 matlab CSZ decomposition (N-2 triangles)    computed at SNL by MAT
	   3.653857995294301e-05 matlab centroidal decomposition (N triangles) computed at SNL by MAT
	   3.653857995294258e-05 NCO CSZ _and_ centroidal decompositions (new haversine)
	   3.653857995289623e-05 NCO CSZ decomposition (old acos)

	   20191011: Tested this same polygon in ESMF and NCO weight-generator
	   NCO maps begin with first destination gridcell, find next ESMF gridcell by searching for first col:
	   ncks --trd -C -v col ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_aave.20191001.nc | egrep "=1 "
	   ncks -H --trd -s %20.15e -C -d n_b,0 -v area_b ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_aave.20191001.nc
	   3.653857995294305e-05
	   ncks -H --trd -s '%20.15e, ' -C -d n_b,0 -v area_b ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_nco.20191001.nc
	   3.653857995295246e-05
	   ESMF and NCO weight-generators produce nearly identical S results to double-precision:
	   ncks -H --trd -s '%20.15e, ' -C -d n_s,0,1 -v S ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_nco.20191001.nc
	   2.181999640069480e-03, 1.309571213636605e-02
	   ncks -H --trd -s %20.15e -C -d n_s,207436 -d n_s,209617 -v S ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_aave.20191001.nc
	   2.181999640069454e-03, 1.309571213636510e-02

	   Compare first five polygon areas:
	   ncks --trd -H -C -s '%20.15e, ' -d n_b,0,4 -v area_b ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_aave.20191001.nc
	   ncks --trd -H -C -s '%20.15e, ' -d n_b,0,4 -v area_b ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_nco.20191001.nc
	   3.653857995294305e-05, 1.250459284052488e-04, 1.448204605591709e-04, 8.223598867312266e-05, 8.585831933875070e-05, # aave
	   3.653857995294258e-05, 1.250459284052470e-04, 1.448204605591675e-04, 8.223598867312247e-05, 8.585831933875186e-05,

	   Compare total areas:
	   ncwa -O -y ttl -v area.? ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_aave.20191001.nc ~/foo_aave.nc
	   ncwa -O -y ttl -v area.? ${DATA}/maps/map_cmip6_180x360_to_conusx4v1np4_chevrons_nco.20191001.nc ~/foo_nco.nc
	   ncks --trd -H -C -s '%20.15e, ' -v area.? ~/foo_aave.nc
	   ncks --trd -H -C -s '%20.15e, ' -v area.? ~/foo_nco.nc
	   aave: 1.256637061435867e+01, 1.256637061435973e+01 
	   nco:  1.256637061435857e+01, 1.256637061435955e+01
	   4*pi: 1.25663706143591729538e+01 
	   Does (tru_glb_ttl/NCO_glb_ttl)*NCO_lcl = ESMF_lcl ?
	   (1.25663706143591729538/1.256637061435857)*3.653857995294258=3.6538579952944333
	   No, normalization alone does not explain differences between ESMF and NCO
	   It does not appear that ESMF does a global normalization of areas/weights */

	/* Computing great circle arcs over small arcs requires care since central angle is near 0 degrees
	   Cosine small angles changes slowly for such angles, and leads to precision loss
	   Use haversine formula instead of spherical law of cosines formula
	   https://en.wikipedia.org/wiki/Great-circle_distance */
	/* Interior angle/great circle arc a, spherical law of cosines formula (loses precision):
	   cos_a=lat_bnd_cos[idx_a]*lon_bnd_cos[idx_a]*lat_bnd_cos[idx_b]*lon_bnd_cos[idx_b]+
	   lat_bnd_cos[idx_a]*lon_bnd_sin[idx_a]*lat_bnd_cos[idx_b]*lon_bnd_sin[idx_b]+
	   lat_bnd_sin[idx_a]*lat_bnd_sin[idx_b];ngl_a=acos(cos_a); */
	/* Interior angle/great circle arc a, haversine formula: */
	// 20160918: Use branch cut rules for longitude
	lon_dlt=fabs(nco_lon_dff_brnch_rdn(lon_bnd_rdn[idx_a],lon_bnd_rdn[idx_b]));
	lat_dlt=fabs(lat_bnd_rdn[idx_a]-lat_bnd_rdn[idx_b]);
	sin_hlf_tht=sqrt(pow(sin(0.5*lat_dlt),2)+lat_bnd_cos[idx_a]*lat_bnd_cos[idx_b]*pow(sin(0.5*lon_dlt),2));
	ngl_a=2.0*asin(sin_hlf_tht);
      }else{ /* !tri_idx == 0 */
	ngl_a=ngl_c;
      } /* !tri_idx == 0 */
      /* Interior angle/great circle arc b */
      lon_dlt=fabs(nco_lon_dff_brnch_rdn(lon_bnd_rdn[idx_b],lon_bnd_rdn[idx_c]));
      lat_dlt=fabs(lat_bnd_rdn[idx_b]-lat_bnd_rdn[idx_c]);
      sin_hlf_tht=sqrt(pow(sin(0.5*lat_dlt),2)+lat_bnd_cos[idx_b]*lat_bnd_cos[idx_c]*pow(sin(0.5*lon_dlt),2));
      ngl_b=2.0*asin(sin_hlf_tht);
      /* Interior angle/great circle arc c */
      lon_dlt=fabs(nco_lon_dff_brnch_rdn(lon_bnd_rdn[idx_c],lon_bnd_rdn[idx_a]));
      lat_dlt=fabs(lat_bnd_rdn[idx_c]-lat_bnd_rdn[idx_a]);
      sin_hlf_tht=sqrt(pow(sin(0.5*lat_dlt),2)+lat_bnd_cos[idx_c]*lat_bnd_cos[idx_a]*pow(sin(0.5*lon_dlt),2));
      ngl_c=2.0*asin(sin_hlf_tht);
      /* Semi-perimeter */
      prm_smi=0.5*(ngl_a+ngl_b+ngl_c);
      /* L'Huilier's formula results in NaN if any side exceeds semi-perimeter
	 This can occur in needle-shaped triangles due to rounding errors in derived arc lengths a, b, c 
	 20200203: Problematic needles occurs a few dozen times in ne120pg2 -> cmip6 maps
	 Problematic isoceles triangles are much rarer than problematic needles
	 Therefore look for needle-issues first, then, if none found, look for isoceles issues
	 Wikipedia recommends treating ill-conditioned triangles by Side-Angle-Side (SAS) formula
	 https://en.wikipedia.org/wiki/Spherical_trigonometry
	 Diagnose needles beforehand and call SAS routines as above to avoid NaN in L'Huilier
	 Label problematic needle triangles by shortest side, e.g., "flg_sas_a" means (b ~ c) and a ~ 0.0 */
      flg_sas_ndl=flg_sas_isc=flg_sas_a=flg_sas_b=flg_sas_c=False;
      if(ngl_a > prm_smi){if(ngl_b > ngl_c) flg_sas_c=True; else flg_sas_b=True;} /* a exceeds semi-perimeter */
      else if(ngl_b > prm_smi){if(ngl_c > ngl_a) flg_sas_a=True; else flg_sas_c=True;} /* b exceeds semi-perimeter */
      else if(ngl_c > prm_smi){if(ngl_a > ngl_b) flg_sas_b=True; else flg_sas_a=True;} /* c exceeds semi-perimeter */
      if(flg_sas_a || flg_sas_b || flg_sas_c) flg_sas_ndl=True;
      if(!flg_sas_ndl){
	/* L'Huilier's formula becomes ill-conditioned when two sides are one half the third side
	   This occurs for flat, isoceles-shaped triangles
	   Label problematic isoceles triangles by longest side, e.g., "flg_sas_a" means (b ~ c) ~ 0.5*a */
	/* Sensitivity tests on ~20191014 showed that triangular ill-conditioning treatment (i.e., switching to SAS method) does not improve (and may degrade) accuracy for eps_ill_cnd > 1.0e-15 */
	const double eps_ill_cnd=1.0e-15; /* [frc] Ill-conditioned tolerance for interior angle/great circle arcs in triangle */
	const double eps_ill_cnd_dbl=2.0*eps_ill_cnd; /* [frc] Ill-conditioned tolerance for interior angle/great circle arcs in triangle */
	if((fabs(ngl_a-ngl_b) < eps_ill_cnd) && (fabs(ngl_a-0.5*ngl_c) < eps_ill_cnd_dbl)) flg_sas_c=True; /* c is twice a and b */
	else if((fabs(ngl_b-ngl_c) < eps_ill_cnd) && (fabs(ngl_b-0.5*ngl_a) < eps_ill_cnd_dbl)) flg_sas_a=True; /* a is twice b and c */
	else if((fabs(ngl_c-ngl_a) < eps_ill_cnd) && (fabs(ngl_c-0.5*ngl_b) < eps_ill_cnd_dbl)) flg_sas_b=True; /* b is twice c and a */
	if(flg_sas_a || flg_sas_b || flg_sas_c) flg_sas_isc=True;
      } /* !flg_sas_ndl */
      if(flg_sas_isc || flg_sas_ndl){
	/* Compute area using SAS formula */
	double cos_hlf_C; /* [frc] Cosine of half of canoncal surface angle C */
	//double sin_hlf_C; /* [frc] Sine of half of canoncal surface angle C */
	double ngl_sfc_ltr_C; /* [rdn] Canonical surface angle/great circle arc C */
	double tan_hlf_a_tan_hlf_b; /* [frc] Product of tangents of one-half of nearly equal canoncal sides */
	double xcs_sph_hlf_tan; /* [frc] Tangent of one-half the spherical excess */
	/* Transform sides into canonical order for formula where C is surface angle between arcs a and b */
	if(flg_sas_c){
	  ngl_ltr_a=ngl_a;
	  ngl_ltr_b=ngl_b;
	  ngl_ltr_c=ngl_c;
	} /* !flg_sas_c */
	if(flg_sas_a){
	  ngl_ltr_a=ngl_b;
	  ngl_ltr_b=ngl_c;
	  ngl_ltr_c=ngl_a;
	} /* !flg_sas_a */
	if(flg_sas_b){
	  ngl_ltr_a=ngl_c;
	  ngl_ltr_b=ngl_a;
	  ngl_ltr_c=ngl_b;
	} /* !flg_sas_b */
	if(flg_sas_ndl && (nco_dbg_lvl_get() >= nco_dbg_scl)) (void)fprintf(stdout,"%s: INFO %s reports col_idx = %li triangle %d is needle-shaped triangle with a side that exceeds semi-perimeter = %0.16e. Eschew L'Huilier's formula for spherical excess to avoid NaN. Could use SAS formula with canonical central interior arc c = %0.16e.\n",nco_prg_nm_get(),fnc_nm,col_idx,tri_idx,prm_smi,ngl_ltr_c);
	if(flg_sas_isc && (nco_dbg_lvl_get() >= nco_dbg_scl)) (void)fprintf(stdout,"%s: INFO %s reports col_idx = %li triangle %d is nearly flat isoceles-shaped triangle. Canonical arcs a and b differ by %0.16e. Eschew L'Huilier's formula for spherical excess to avoid low precision. Could use SAS formula.\n",nco_prg_nm_get(),fnc_nm,col_idx,tri_idx,fabs(ngl_ltr_a-ngl_ltr_b));
	/* Determine canonical surface angle C
	   To find any angle given three spherical triangle sides, Wikipedia opines: 
	   "The cosine rule may be used to give the angles A, B, and C but, to avoid ambiguities, the half-angle formulae are preferred."
	   Half-angle formulae include two applicable variants that yield the sine or cosine of half C
	   Then C is determined as twice the asin() or acos() function, respectively
	   For needle-shaped triangles, RHS sin formula is ~ sin^2(0)/sin(a)*sin(b) ~ 0.0 
	   For needle-shaped triangles, RHS cos formula is ~ sin^2(s)/sin(a)*sin(b) ~ 0.5
	   For flat isoceles triangles, RHS sin formula is ~ sin^2(0)/sin(a)*sin(b) ~ 0.0 
	   For flat isoceles triangles, RHS cos formula is ~ sin(s)*sin(0)/sin(a)*sin(b) ~ 0.0 
	   Use sin formula since both needle- and isoceles-shaped triangles have RHS ~ 0.0 where arcsin() is most precise 
	   20200203: Half-angle sine formula gives NaNs, and half-angle cosine formula works on ne120pg2->cmip. Why?
	   Adopting cosine formula because it works */
	//sin_hlf_C=sqrt(sin(prm_smi-ngl_ltr_a)*sin(prm_smi-ngl_ltr_b)/(sin(ngl_ltr_a)*sin(ngl_ltr_b))); // Half-angle sine formula
	cos_hlf_C=sqrt(sin(prm_smi)*sin(prm_smi-ngl_ltr_c)/(sin(ngl_ltr_a)*sin(ngl_ltr_b))); // Half-angle cosine formula
	//ngl_sfc_ltr_C=2.0*asin(sin_hlf_C);
	ngl_sfc_ltr_C=2.0*acos(cos_hlf_C);
	/* SAS formula */
	tan_hlf_a_tan_hlf_b=tan(0.5*ngl_ltr_a)*tan(0.5*ngl_ltr_b);
	xcs_sph_hlf_tan=tan_hlf_a_tan_hlf_b*sin(ngl_sfc_ltr_C)/(1.0+tan_hlf_a_tan_hlf_b*cos(ngl_sfc_ltr_C));
	assert(fabs(xcs_sph_hlf_tan) != M_PI_2);
	xcs_sph=2.0*atan(xcs_sph_hlf_tan);
	if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO SAS area formula for polygon col_idx = %li, triangle %d, vertices A, B, C at (lat,lon) [dgr] = (%0.16f, %0.16f), (%0.16f, %0.16f), (%0.16f, %0.16f). Interior angles/great circle arcs (a, b, c) [rdn] = (%0.16e, %0.16e, %0.16e). Spherical excess = %0.16e.\n",nco_prg_nm_get(),col_idx,tri_idx,lat_bnd[idx_a],lon_bnd[idx_a],lat_bnd[idx_b],lon_bnd[idx_b],lat_bnd[idx_c],lon_bnd[idx_c],ngl_a,ngl_b,ngl_c,xcs_sph);
	// Single-line version
	// xcs_sph=2.0*atan(tan(0.5*ngl_ltr_a)*tan(0.5*ngl_ltr_b)*sin(2.0*acos(sqrt(sin(prm_smi)*sin(prm_smi-ngl_c)/(sin(ngl_a)*sin(ngl_b)))))/(1.0+tan_hlf_a_tan_hlf_b*cos(2.0*acos(sqrt(sin(prm_smi)*sin(prm_smi-ngl_c)/(sin(ngl_a)*sin(ngl_b)))))));
	/* Above procedure for problematic needle-shaped and isoceles-shaped triangles degrades statistics
	   For ne30pg2, ne120pg2 -> cmip, setting area = 0.0 _greatly_ improves area statistics (Why?)
	   Set spherical excess to zero for problematic needle-shaped and isoceles-shaped triangles */
	/* fxm: Make zeroing skinny needles/isoceles-shaped triangle-areas a command-line option? */
	if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO Setting SAS area = 0.0\n",nco_prg_nm_get());
	  xcs_sph=0.0;
	/* !flg_sas */
      }else{
	double xcs_sph_qtr_tan; /* [frc] Tangent of one-quarter the spherical excess */
	xcs_sph_qtr_tan=sqrt(tan(0.5*prm_smi)*tan(0.5*(prm_smi-ngl_a))*tan(0.5*(prm_smi-ngl_b))*tan(0.5*(prm_smi-ngl_c)));
	assert(fabs(xcs_sph_qtr_tan) != M_PI_2);
	xcs_sph=4.0*atan(xcs_sph_qtr_tan);
	/* 20191014: Aggregate all previous area-related commands into one, gigantic, unreadable, possibly more precise command (tested and it is more obfuscated but not more precise) */
	// xcs_sph=4.0*atan(sqrt(tan(0.5*0.5*(ngl_a+ngl_b+ngl_c))*tan(0.5*(0.5*(ngl_a+ngl_b+ngl_c)-ngl_a))*tan(0.5*(0.5*(ngl_a+ngl_b+ngl_c)-ngl_b))*tan(0.5*(0.5*(ngl_a+ngl_b+ngl_c)-ngl_c))));
      } /* !flg_sas */
      if(isnan(xcs_sph)){
	const double eps_ngl_skn=1.0e-13; /* [frc] Angles skinnier than this form needles whose area ~ 0.0 */
	/* Categorize reason for NaN */
	(void)fprintf(stdout,"%s: WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING\nUnxpected NaN polygon col_idx = %li, triangle %d, vertices A, B, C at (lat,lon) [dgr] = (%0.16f, %0.16f), (%0.16f, %0.16f), (%0.16f, %0.16f). Interior angles/great circle arcs (a, b, c) [rdn] = (%0.16e, %0.16e, %0.16e).\n",nco_prg_nm_get(),col_idx,tri_idx,lat_bnd[idx_a],lon_bnd[idx_a],lat_bnd[idx_b],lon_bnd[idx_b],lat_bnd[idx_c],lon_bnd[idx_c],ngl_a,ngl_b,ngl_c);
	if( /* Side exceeds semi-perimeter */
		 (ngl_a > prm_smi) ||
		 (ngl_b > prm_smi) ||
		 (ngl_c > prm_smi)
		 ){	  
	  (void)fprintf(stdout,"%s: WARNING Triangle side exceeds semi-perimeter = %0.16e polygon col_idx = %li, triangle %d, vertices A, B, C at (lat,lon) [dgr] = (%0.16f, %0.16f), (%0.16f, %0.16f), (%0.16f, %0.16f). Interior angles/great circle arcs (a, b, c) [rdn] = (%0.16e, %0.16e, %0.16e). Assigned triangle area = 0.0.\n",nco_prg_nm_get(),prm_smi,col_idx,tri_idx,lat_bnd[idx_a],lon_bnd[idx_a],lat_bnd[idx_b],lon_bnd[idx_b],lat_bnd[idx_c],lon_bnd[idx_c],ngl_a,ngl_b,ngl_c);
	}else if( /* Are angles too skinny? Quite often on ne30pg2, ne120pg2 */
		 (ngl_a < eps_ngl_skn) ||
		 (ngl_b < eps_ngl_skn) ||
		 (ngl_c < eps_ngl_skn)
		 ){	  
	  (void)fprintf(stdout,"%s: WARNING Triangle has at least one skinny angles < %g [rdn] for polygon col_idx = %li, triangle %d, vertices A, B, C at (lat,lon) [dgr] = (%0.16f, %0.16f), (%0.16f, %0.16f), (%0.16f, %0.16f). Interior angles/great circle arcs (a, b, c) [rdn] = (%0.16f, %0.16f, %0.16f). Assigned triangle area = 0.0.\n",nco_prg_nm_get(),eps_ngl_skn,col_idx,tri_idx,lat_bnd[idx_a],lon_bnd[idx_a],lat_bnd[idx_b],lon_bnd[idx_b],lat_bnd[idx_c],lon_bnd[idx_c],ngl_a,ngl_b,ngl_c);
	}else if( /* Are two vertices identical to double-precision? Never on ne30pg2, ne120pg2 */
	   ((lat_bnd[idx_a] == lat_bnd[idx_b]) && (lon_bnd[idx_a] == lon_bnd[idx_b])) ||
	   ((lat_bnd[idx_b] == lat_bnd[idx_c]) && (lon_bnd[idx_b] == lon_bnd[idx_c])) ||
	   ((lat_bnd[idx_c] == lat_bnd[idx_a]) && (lon_bnd[idx_c] == lon_bnd[idx_a])) 
	   ){
	  (void)fprintf(stdout,"%s: WARNING Triangle has repeated points for polygon col_idx = %li, triangle %d, vertices A, B, C at (lat,lon) [dgr] = (%g, %g), (%g, %g), (%g, %g). Assigned triangle area = 0.0.\n",nco_prg_nm_get(),col_idx,tri_idx,lat_bnd[idx_a],lon_bnd[idx_a],lat_bnd[idx_b],lon_bnd[idx_b],lat_bnd[idx_c],lon_bnd[idx_c]);
	}else{
	  (void)fprintf(stdout,"%s: WARNING Triangle area formula yields NaN for polygon col_idx = %li, triangle %d, vertices A, B, C at (lat,lon) [dgr] = (%0.16f, %0.16f), (%0.16f, %0.16f), (%0.16f, %0.16f). Interior angles/great circle arcs (a, b, c) [rdn] = (%0.16f, %0.16f, %0.16f). Are points co-linear? Assigned triangle area = 0.0.\n",nco_prg_nm_get(),col_idx,tri_idx,lat_bnd[idx_a],lon_bnd[idx_a],lat_bnd[idx_b],lon_bnd[idx_b],lat_bnd[idx_c],lon_bnd[idx_c],ngl_a,ngl_b,ngl_c);
	} /* !co-linear */
	xcs_sph=0.0;
      } /* !NaN */
      area[col_idx]+=xcs_sph; /* Accumulate spherical triangle area into reported polygon area and adjust below */
      area_smc+=xcs_sph; /* Accumulate spherical triangle area into small-circle polygon area and adjust below */
      area_ttl+=xcs_sph; /* Accumulate spherical triangle area into spherical polygon area */
      area_smc_ttl+=xcs_sph; /* Accumulate spherical triangle area into total polygon area and adjust below */
      /* 20160918 from here to end of loop is non-spherical work
	 20170217: Temporarily turn-off latitude circle diagnostics because Sungduk's POP case breaks them
	 Canonical latitude-triangle geometry has point A at apex and points B and C at same latitude
	 ncremap --dbg=1 --alg_typ=nco --grd_src=${DATA}/grids/ne30np4_pentagons.091226.nc --grd_dst=${DATA}/grids/cmip6_180x360_scrip.20181001.nc --map=${DATA}/maps/map_ne30np4_to_cmip6_180x360_nco.20190601.nc
	 ncremap --dbg=1 -R 'edg_typ=smc' --alg_typ=nco --grd_src=${DATA}/grids/ne30np4_pentagons.091226.nc --grd_dst=${DATA}/grids/cmip6_180x360_scrip.20181001.nc --map=${DATA}/maps/map_ne30np4_to_cmip6_180x360_smc.20190601.nc */
      flg_tri_crr_smc=False;
      if(lat_bnd_rdn[idx_a] == lat_bnd_rdn[idx_b] ||
	 lat_bnd_rdn[idx_b] == lat_bnd_rdn[idx_c] ||
	 lat_bnd_rdn[idx_c] == lat_bnd_rdn[idx_a]){
	/* Set flag only if triangle is not degenerate. Degenerate triangles (3 points on a geodesic) have zero area */
	if(xcs_sph != 0.0) flg_ply_has_smc=flg_tri_crr_smc=True;
	if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: DEBUG Found small circle triangle with vertices A, B, C at (lat,lon) [dgr] = (%g, %g), (%g, %g), (%g, %g)\n",nco_prg_nm_get(),lat_bnd[idx_a],lon_bnd[idx_a],lat_bnd[idx_b],lon_bnd[idx_b],lat_bnd[idx_c],lon_bnd[idx_c]);
      } /* endif */
      if((edg_typ == nco_edg_smc) && flg_tri_crr_smc){
	double ngl_plr; /* [rdn] Polar angle (co-latitude) */
	long idx_ltr_a; /* [idx] Point A (apex) of canonical latitude-triangle geometry, 1-D index */
	long idx_ltr_b; /* [idx] Point B (base) of canonical latitude-triangle geometry, 1-D index */
	long idx_ltr_c; /* [idx] Point C (base) of canonical latitude-triangle geometry, 1-D index */
	/* Rotate labels to standard position with vertex A, equi-latitude points B and C */
	if(lat_bnd_rdn[idx_a] == lat_bnd_rdn[idx_b]){
	  idx_ltr_a=idx_c;
	  idx_ltr_b=idx_a;
	  idx_ltr_c=idx_b;
	  ngl_ltr_a=ngl_c;
	  ngl_ltr_b=ngl_a;
	  ngl_ltr_c=ngl_b;
	  ngl_plr=fabs(M_PI_2-lat_bnd_rdn[idx_a]);
	}else if(lat_bnd_rdn[idx_b] == lat_bnd_rdn[idx_c]){
	  idx_ltr_a=idx_a;
	  idx_ltr_b=idx_b;
	  idx_ltr_c=idx_c;
	  ngl_ltr_a=ngl_a;
	  ngl_ltr_b=ngl_b;
	  ngl_ltr_c=ngl_c;
	  ngl_plr=fabs(M_PI_2-lat_bnd_rdn[idx_b]);
	}else if(lat_bnd_rdn[idx_c] == lat_bnd_rdn[idx_a]){
	  idx_ltr_a=idx_b;
	  idx_ltr_b=idx_c;
	  idx_ltr_c=idx_a;
	  ngl_ltr_a=ngl_b;
	  ngl_ltr_b=ngl_c;
	  ngl_ltr_c=ngl_a;
	  ngl_plr=fabs(M_PI_2-lat_bnd_rdn[idx_c]);
	}else{
	  (void)fprintf(stdout,"%s: ERROR latitudes not equal in small circle section. Vertices A, B, C at (lat,lon) [dgr] = (%g, %g), (%g, %g), (%g, %g)\n",nco_prg_nm_get(),lat_bnd[idx_a],lon_bnd[idx_a],lat_bnd[idx_b],lon_bnd[idx_b],lat_bnd[idx_c],lon_bnd[idx_c]);
	  abort();
	} /* endif */
	/* 20160918: Compute exact area of latitude triangle wedge */
	double xpn_x; /* [frc] Expansion parameter */
	lon_dlt=fabs(nco_lon_dff_brnch_rdn(lon_bnd_rdn[idx_ltr_b],lon_bnd_rdn[idx_ltr_c]));
	assert(lon_dlt != 0.0); // Latitude triangles must have bases with distinct longitudes
	if(lon_dlt != M_PI){
	  /* Normal clause executed for small-circle triangles */
	  /* Numeric conditioning uncertain. Approaches divide-by-zero when lon_dlt << 1 */
	  xpn_x=lat_bnd_sin[idx_ltr_b]*(1.0-cos(lon_dlt))/sin(lon_dlt);
	  assert(fabs(xpn_x) != M_PI_2);
	  area_smc_crc=2.0*atan(xpn_x);
	  /* 20170217: Sungduk's POP regrid triggers following abort():
	     ncremap -D 1 -i ~/pop_g16.nc -d ~/cam_f19.nc -o ~/foo.nc */
	  //assert(xpn_x >= 0.0);
	  //if(lat_bnd[idx_ltr_b] > 0.0) area_smc_crc+=-lon_dlt*lat_bnd_sin[idx_ltr_b]; else area_smc_crc+=+lon_dlt*lat_bnd_sin[idx_ltr_b];
	  area_smc_crc+=-lon_dlt*lat_bnd_sin[idx_ltr_b];
	}else{
	/* 20200228: Latitude triangles may have bases with longitudes that differ by 180 degrees 
	   Consider a quadrilateral with four equidistant vertices in longitude, and that caps a pole:
	   CSZ decomposition technique divides this into two triangles each with three co-latitudinal points and no vertex at pole
	   Solution candidates:
	   1. Divide such quadrilaterals using centroid technique 
	   Just realized current implementation of centroid decomposition fails on polar caps
	   Failure occurs because centroid latitude is +/- ~90 not mean of vertices' latitudes
	   Must impute "pseudo-centroid" with latitude +/- 90 instead of averaging vertex latitudes
	   Requires testing each polygon to determine if it contains pole <- Too difficult/expensive
	   2. Assume latitude triangles whose base is 180 degrees are at pole
	   Compute area exactly using analytic formula for annular lune */
	  (void)fprintf(stdout,"%s: INFO longitudes differ by pi in small circle section. Vertices A, B, C at (lat,lon) [dgr] = (%g, %g), (%g, %g), (%g, %g)\n",nco_prg_nm_get(),lat_bnd[idx_ltr_a],lon_bnd[idx_ltr_a],lat_bnd[idx_ltr_b],lon_bnd[idx_ltr_b],lat_bnd[idx_ltr_c],lon_bnd[idx_ltr_c]);
	  (void)fprintf(stdout,"%s: DEBUG col_nbr=%lu, bnd_nbr=%d, col_idx=%ld, area=%g. Vertices [0..bnd_nbr-1] in format idx (lat,lon)\n",nco_prg_nm_get(),col_nbr,bnd_nbr,col_idx,xcs_sph);
	  for(int bnd_idx=0;bnd_idx<bnd_nbr;bnd_idx++)
	    (void)fprintf(stdout,"%2d (%g, %g)\n",bnd_idx,lat_bnd[bnd_nbr*col_idx+bnd_idx],lon_bnd[bnd_nbr*col_idx+bnd_idx]);
	  (void)fprintf(stdout,"%s: INFO Assuming this triangle is decomposed from polar cap polygon. Treating area with analytic formula for annular lune\n",nco_prg_nm_get());
	  /* Compute small circle correction as difference between spherical triangle area and standard annuular lune formula 
	     Small circle correction is positive-definite for polar triangles so use fabs(sin(lat_bnd_sin)) */
	  area_smc_crc=lon_dlt*fabs(lat_bnd_sin[idx_ltr_b])-area_smc;
	} /* !lon_dlt */
	// Adjust diagnostic areas by small-circle area correction
	area_smc+=area_smc_crc;
	area_smc_ttl+=area_smc_crc;
	area_smc_crc_ttl+=area_smc_crc;
	area_smc_crc_abs_ttl+=fabs(area_smc_crc);
	// 20200109: Adjust area reported to calling code by small-circle area correction
	area[col_idx]+=area_smc_crc;
	if(0){
	  /* 20160918: Approximate area of latitude triangle wedge. Use truncated power expansion of exact formula. */
	  double xpn_x_sqr; /* [frc] Expansion parameter squared */
	  double xpn_sum; /* [frc] Expansion sum */
	  double xpn_nmr; /* [frc] Expansion term numerator */
	  double xpn_trm; /* [frc] Expansion term */
	  double xpn_dnm; /* [frc] Expansion term denominator */
	  const unsigned short int rdr_xpn=3; /* [nbr] Order of N in trigonometric series expansion */
	  unsigned short int idx_xpn; /* [idx] Index in series expansion */
	  xpn_x=cos(ngl_plr)*(1.0-cos(lon_dlt))/sin(lon_dlt);
	  xpn_x_sqr=xpn_x*xpn_x;
	  xpn_nmr=xpn_x;
	  xpn_dnm=1.0;
	  xpn_trm=xpn_nmr/xpn_dnm;
	  xpn_sum+=xpn_trm;
	  for(idx_xpn=3;idx_xpn<=rdr_xpn;idx_xpn+=2){
	    xpn_nmr*=xpn_x_sqr;
	    xpn_dnm*=(idx_xpn-1)*idx_xpn;
	    xpn_trm=xpn_nmr/xpn_dnm;
	    xpn_sum+=xpn_trm;
	  } /* !idx_xpn */
	  (void)fprintf(stdout,"%s: Small-circle area using series approximation...not implemented yet\n",nco_prg_nm_get());
	} /* !0 */
	if(nco_dbg_lvl_get() >= nco_dbg_scl){
	  (void)fprintf(stdout,"%s: INFO %s col_idx = %li triangle %d spherical area, latitude-triangle area, %% difference: %g, %g, %g%%\n",nco_prg_nm_get(),fnc_nm,col_idx,tri_idx,xcs_sph,xcs_sph+area_smc_crc,100.0*area_smc_crc/xcs_sph);
	  if(fabs(area_smc_crc/xcs_sph) > 0.1){
	    (void)fprintf(stdout,"%s: DEBUG Non-spherical correction exceeds 10%% for current triangle with vertices A, B, C at (lat,lon) [dgr] = (%g, %g), (%g, %g), (%g, %g)\n",nco_prg_nm_get(),lat_bnd[idx_ltr_a],lon_bnd[idx_ltr_a],lat_bnd[idx_ltr_b],lon_bnd[idx_ltr_b],lat_bnd[idx_ltr_c],lon_bnd[idx_ltr_c]);
	  } /* !fabs */
	} /* !dbg */
      } /* !edg_typ && flg_tri_crr_smc */
    } /* !tri_idx */
    if(edg_typ == nco_edg_smc && flg_ply_has_smc){
      /* Current gridcell contained at least one latitude-triangle */
      if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s col_idx = %li spherical area, small circle area, %% difference: %g, %g, %g%%\n",nco_prg_nm_get(),fnc_nm,col_idx,area[col_idx],area_smc,100.0*(area_smc-area[col_idx])/area[col_idx]);
    } /* !edg_typ && !flg_ply_has_smc */    
  } /* !col_idx */
  if(edg_typ == nco_edg_smc && nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s total spherical area, small circle area, %% difference, crc_ttl, crc_abs_ttl: %g, %g, %g%%, %g, %g\n",nco_prg_nm_get(),fnc_nm,area_ttl,area_smc_ttl,100.0*(area_smc_ttl-area_ttl)/area_ttl,area_smc_crc_ttl,area_smc_crc_abs_ttl);
  if(vrt_vld) vrt_vld=(long *)nco_free(vrt_vld);
  if(a_idx) a_idx=(long *)nco_free(a_idx);
  if(b_idx) b_idx=(long *)nco_free(b_idx);
  if(c_idx) c_idx=(long *)nco_free(c_idx);
  if(lat_bnd_rdn) lat_bnd_rdn=(double *)nco_free(lat_bnd_rdn);
  if(lon_bnd_rdn) lon_bnd_rdn=(double *)nco_free(lon_bnd_rdn);
  if(lat_bnd_cos) lat_bnd_cos=(double *)nco_free(lat_bnd_cos);
  if(lon_bnd_cos) lon_bnd_cos=(double *)nco_free(lon_bnd_cos);
  if(lat_bnd_sin) lat_bnd_sin=(double *)nco_free(lat_bnd_sin);
  if(lon_bnd_sin) lon_bnd_sin=(double *)nco_free(lon_bnd_sin);
} /* !nco_sph_plg_area() */

int /* O [enm] Return code */
nco_rgr_tps /* [fnc] Regrid using TempestRemap library */
(rgr_sct * const rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields using TempestRemap "library" (more precisely, executables)
     Routine was originally written to call Tempest executables
     However, that functionality was all placed into the ncremap shell script
     Thus this C-interface is currently unused
     TempestRemap2 has a library that may be accessed on-line

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
  nco_rgr_tps_cmd nco_tps_cmd; /* [enm] TempestRemap command enum */

  char *nvr_DATA_TEMPEST; /* [sng] Directory where Tempest grids, meshes, and weights are stored */
  nvr_DATA_TEMPEST=getenv("DATA_TEMPEST");
  rgr->drc_tps= (nvr_DATA_TEMPEST && strlen(nvr_DATA_TEMPEST) > 0L) ? (char *)strdup(nvr_DATA_TEMPEST) : (char *)strdup("/tmp");

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"drc_tps = %s, ",rgr->drc_tps ? rgr->drc_tps : "NULL");
    (void)fprintf(stderr,"\n");
  } /* endif dbg */

  /* Allow for whitespace characters in fl_grd_dst
     Assume CDL translation results in acceptable name for shell commands */
  fl_grd_dst_cdl=nm2sng_fl(fl_grd_dst);

  /* Construct and execute regridding command */
  nco_tps_cmd=nco_rgr_GenerateRLLMesh;
  cmd_rgr_fmt=nco_tps_cmd_fmt_sng(nco_tps_cmd);
  cmd_rgr=(char *)nco_malloc((strlen(cmd_rgr_fmt)+strlen(fl_grd_dst_cdl)-fmt_chr_nbr+1UL)*sizeof(char));
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stderr,"%s: %s reports generating %d by %d RLL mesh in %s...\n",nco_prg_nm_get(),fnc_nm,lat_nbr_rqs,lon_nbr_rqs,fl_grd_dst);
  (void)sprintf(cmd_rgr,cmd_rgr_fmt,lat_nbr_rqs,lon_nbr_rqs,fl_grd_dst_cdl);
  rcd_sys=system(cmd_rgr);
  if(rcd_sys == -1){
    (void)fprintf(stdout,"%s: ERROR %s unable to complete TempestRemap regridding command \"%s\"\n",nco_prg_nm_get(),fnc_nm,cmd_rgr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"done\n");

  /* Clean-up memory */
  if(fl_grd_dst_cdl) fl_grd_dst_cdl=(char *)nco_free(fl_grd_dst_cdl);
  if(cmd_rgr) cmd_rgr=(char *)nco_free(cmd_rgr);
  
  return NCO_NOERR;
} /* end nco_rgr_tps() */

const char * /* O [sng] String describing two-dimensional grid-type */
nco_grd_2D_sng /* [fnc] Convert two-dimensional grid-type enum to string */
(const nco_grd_2D_typ_enm nco_grd_2D_typ) /* I [enm] Two-dimensional grid-type enum */
{
  /* Purpose: Convert two-dimensional grid-type enum to string */
  switch(nco_grd_2D_typ){
  case nco_grd_2D_unk: return "Unknown, unclassified, or unrepresentable 2D grid type (e.g., unstructured, curvilinear, POP displaced-pole)";
  case nco_grd_2D_gss: return "Gaussian latitude grid. Used by spectral transform models, e.g., CCM 1-3, CAM 1-3, ECMWF Forecast, LSM, MATCH, NCEP (R1, R2), UCICTM.";
  case nco_grd_2D_fv: return "Cap-latitude grid, aka FV-scalar grid (in Lin-Rood representation). When global (not regional) in extent and with odd number of latitudes, poles are considered at (and labeled as) centers of first and last gridcells. For example lat_ctr=-90,-89,-88,... and lat_crn=-89.5,-88.5,-87.5,... Thus pole-gridcells span half the equi-angular latitude increment of the rest of the grid. Used by CAM FV (i.e., CAM 4-6), ECMWF (ERA-I, ERA40, ERA5), GEOS-CHEM, UCICTM, UKMO.";
  case nco_grd_2D_eqa: return "Uniform/Equi-Angular latitude grid. Uniform/Equi-angle (everywhere) latitude grid. When global (not regional) in extent and with even number of latitudes, poles are at corners/edges of first and last gridcells. For example lat_ctr=-89.5,-88.5,-87.5,... and lat_crn=-90,-89,-88,.... When global, forms valid FV-staggered (aka FV-velocity, aka offset) grid (for Lin-Rood representation). Used by CIESIN/SEDAC, IGBP-DIS, TOMS AAI, WOCE.";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_grd_2D_sng() */

const char * /* O [sng] String describing latitude grid-type */
nco_grd_lat_sng /* [fnc] Convert latitude grid-type enum to string */
(const nco_grd_lat_typ_enm nco_grd_lat_typ) /* I [enm] Latitude grid-type enum */
{
  /* Purpose: Convert latitude grid-type enum to string */
  switch(nco_grd_lat_typ){
  case nco_grd_lat_unk: return "Unknown, unclassified, or unrepresentable latitude grid type (e.g., unstructured, curvilinear, POP3)";
  case nco_grd_lat_gss: return "Gaussian latitude grid used by global spectral models: CCM 1-3, CAM 1-3, ECMWF Forecast, LSM, MATCH, NCEP (R1, R2), UCICTM.";
  case nco_grd_lat_fv: return "Cap-latitude grid, aka FV-scalar grid (in Lin-Rood representation). When global (not regional) in extent and with odd number of latitudes, poles are considered at (and labeled as) centers of first and last gridcells. For example lat_ctr=-90,-89,-88,... and lat_crn=-89.5,-88.5,-87.5,... Thus pole-gridcells span half the equi-angular latitude increment of the rest of the grid. Used by CAM FV (i.e., CAM 4-6), ECMWF (ERA-I, ERA40, ERA5), GEOS-CHEM, UCICTM, UKMO.";
  case nco_grd_lat_eqa: return "Uniform/Equi-Angular latitude grid. Uniform/Equi-angle (everywhere) latitude grid. When global (not regional) in extent and with even number of latitudes, poles are at corners/edges of first and last gridcells. For example lat_ctr=-89.5,-88.5,-87.5,... and lat_crn=-90,-89,-88,.... When global, forms valid FV-staggered (aka FV-velocity, aka offset) grid (for Lin-Rood representation). Used by CIESIN/SEDAC, IGBP-DIS, TOMS AAI, WOCE.";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_grd_lat_sng() */

const char * /* O [sng] String describing longitude grid-type */
nco_grd_lon_sng /* [fnc] Convert longitude grid-type enum to string */
(const nco_grd_lon_typ_enm nco_grd_lon_typ) /* I [enm] Longitude grid-type enum */
{
  /* Purpose: Convert longitude grid-type enum to string */
  switch(nco_grd_lon_typ){
  case nco_grd_lon_unk: return "Unknown, unclassified, or unrepresentable longitude grid type (e.g., unstructured, curvilinear)";
  case nco_grd_lon_180_wst: return "Date line at west edge of first longitude cell";
  case nco_grd_lon_180_ctr: return "Date line at center of first longitude cell";
  case nco_grd_lon_Grn_wst: return "Greenwich at west edge of first longitude cell";
  case nco_grd_lon_Grn_ctr: return "Greenwich at center of first longitude cell";
  case nco_grd_lon_bb: return "Longitude grid determined by bounding box (lon_wst/lon_est) and gridcell number (lon_nbr)";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_grd_lon_sng() */

const char * /* O [sng] String describing grid extent */
nco_grd_xtn_sng /* [fnc] Convert two-dimensional grid-extent enum to string */
(const nco_grd_xtn_enm nco_grd_xtn) /* I [enm] Grid-extent enum */
{
  /* Purpose: Convert grid-extent enum to string */
  switch(nco_grd_xtn){
  case nco_grd_xtn_nil: return "Unknown";
  case nco_grd_xtn_glb: return "Global";
  case nco_grd_xtn_rgn: return "Regional"; 
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_grd_xtn_sng() */

const char * /* O [sng] String describing grid conversion */
nco_rgr_grd_sng /* [fnc] Convert grid conversion enum to string */
(const nco_rgr_typ_enm nco_rgr_typ) /* I [enm] Grid conversion enum */
{
  /* Purpose: Convert grid conversion enum to string */
  switch(nco_rgr_typ){
  case nco_rgr_grd_1D_to_1D: return "1D_to_1D";
  case nco_rgr_grd_1D_to_2D: return "1D_to_2D";
  case nco_rgr_grd_2D_to_1D: return "2D_to_1D";
  case nco_rgr_grd_2D_to_2D: return "2D_to_2D";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_grd_sng() */

const char * /* O [sng] String describing regridding method */
nco_rgr_mth_sng /* [fnc] Convert regridding method enum to string */
(const nco_rgr_mth_typ_enm nco_rgr_mth_typ) /* I [enm] Regridding method enum */
{
  /* Purpose: Convert regridding method enum to string */
  switch(nco_rgr_mth_typ){
  case nco_rgr_mth_conservative: return "Conservative remapping";
  case nco_rgr_mth_bilinear: return "Bilinear remapping";
  case nco_rgr_mth_none: return "none";
  case nco_rgr_mth_unknown: return "Unknown (TempestRemap or ESMF_weight_only)";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_mth_sng() */

const char * /* O [sng] String describing mapfile generator */
nco_rgr_mpf_sng /* [fnc] Convert mapfile generator enum to string */
(const nco_rgr_mpf_typ_enm nco_rgr_mpf_typ) /* I [enm] Mapfile generator enum */
{
  /* Purpose: Convert mapfile generator enum to string */
  switch(nco_rgr_mpf_typ){
  case nco_rgr_mpf_ESMF: return "ESMF Offline Regridding Weight Generator (ERWG), either from ESMF_RegridWeightGen directly or via NCL";
  case nco_rgr_mpf_SCRIP: return "SCRIP (original LANL package)";
  case nco_rgr_mpf_Tempest: return "TempestRemap (GenerateOfflineMap)";
  case nco_rgr_mpf_ESMF_weight_only: return "ESMF Offline Regridding Weight Generator (ERWG), either from ESMF_RegridWeightGen directly or via NCL, with --weight_only option from ERWG 7.1+";
  case nco_rgr_mpf_NCO: return "netCDF Operators (NCO) Offline Regridding Weight Generator";
  case nco_rgr_mpf_unknown: return "Unknown Weight Generator";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_mpf_sng() */

const char * /* O [sng] String describing regridding normalization */
nco_rgr_nrm_sng /* [fnc] Convert regridding normalization enum to string */
(const nco_rgr_nrm_typ_enm nco_rgr_nrm_typ) /* I [enm] Regridding normalization enum */
{
  /* Purpose: Convert regridding normalization enum to string */
  switch(nco_rgr_nrm_typ){
  case nco_rgr_nrm_fracarea: return "fracarea";
  case nco_rgr_nrm_destarea: return "destarea";
  case nco_rgr_nrm_none: return "none";
  case nco_rgr_nrm_unknown: return "Unknown (possibilities include ESMF_weight_only, NCO, and TempestRemap)";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_nrm_sng() */

const char * /* O [sng] String containing regridding command and format */
nco_tps_cmd_fmt_sng /* [fnc] Convert TempestRemap command enum to command string */
(const nco_rgr_tps_cmd nco_tps_cmd) /* I [enm] TempestRemap command enum */
{
  /* Purpose: Convert TempestRemap command enum to command string and format */
  switch(nco_tps_cmd){
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
nco_tps_cmd_sng /* [fnc] Convert TempestRemap command enum to command name */
(const nco_rgr_tps_cmd nco_tps_cmd) /* I [enm] TempestRemap command enum */
{
  /* Purpose: Convert TempestRemap command enum to command string */
  switch(nco_tps_cmd){
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

int /* O [enm] Return code */
nco_grd_mk /* [fnc] Create SCRIP-format grid file */
(rgr_sct * const rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Use grid information to create SCRIP-format grid file

     Spherical geometry terminology:
     spherical cap = spherical dome = volume cut-off by plane
     spherical lune = digon = area bounded by two half-great circles = base of spherical wedge
     spherical segment = volume defined by cutting sphere with pair parallel planes
     spherical sector = volume subtended by lat1
     spherical wedge = ungula = volume subtended by lon2-lon1
     spherical zone = area of spherical segment excluding bases
     spherical quadrangle = area of intersection of spherical zone and lune (i.e., area of 

     bearing = angle from true north
     geodesic = shortest path between points on a surface
     great circle = orthodrome = "straight path" = geodesic of the sphere
     convergency = difference (in azimuth?) between great circle tracks at two different positions
     conversion angle = angle between geodesic and rhumb line 
     rhumb line = loxodrome = "oblique (or slanted) path" = line of constant azimuth
     
     Formulae:
     http://www.movable-type.co.uk/scripts/latlong.html # On-line Javascript implementation
     http://williams.best.vwh.net/avform.htm

     ACME:
     https://acme-svn2.ornl.gov/acme-repo/acme/mapping/grids
     https://acme-svn2.ornl.gov/acme-repo/acme/inputdata/cpl/gridmaps

     NCAR:
     yellowstone.ucar.edu:/glade/p/cesm/cseg/mapping/grids
     yellowstone.ucar.edu:/glade/p_old/cesm/cseg/mapping/grids

     Global RLL grids:
     ncks -O -D 1 --rgr ttl='Equiangular grid 180x360' --rgr grid=${DATA}/grids/180x360_SCRIP.20150901.nc --rgr latlon=180,360 --rgr lat_typ=eqa --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='Equiangular grid 90x180' --rgr grid=${DATA}/grids/90x180_SCRIP.20150901.nc --rgr latlon=90,180 --rgr lat_typ=eqa --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc

     Maps for global RLL grids:
     ESMF_RegridWeightGen -s ${DATA}/grids/180x360_SCRIP.20150901.nc -d ${DATA}/grids/90x180_SCRIP.20150901.nc -w ${DATA}/maps/map_180x360_to_90x180.20150901.nc --method conserve
     ESMF_RegridWeightGen -s ${DATA}/grids/90x180_SCRIP.20150901.nc -d ${DATA}/grids/180x360_SCRIP.20150901.nc -w ${DATA}/maps/map_90x180_to_180x360.20150901.nc --method conserve

     ACME grids:
     ncks -O -D 1 --rgr ttl='FV-scalar grid 129x256' --rgr grid=${DATA}/grids/129x256_SCRIP.20150910.nc --rgr latlon=129,256 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr  ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='FV-scalar grid 257x512' --rgr grid=${DATA}/grids/257x512_SCRIP.20150910.nc --rgr latlon=257,512 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr  ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='FV-scalar grid 801x1600' --rgr grid=${DATA}/grids/801x1600_SCRIP.20150910.nc --rgr latlon=801,1600 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr  ~/nco/data/in.nc ~/foo.nc

     ACME maps:
     ESMF_RegridWeightGen -s ${DATA}/grids/ne30np4_pentagons.091226.nc -d ${DATA}/grids/129x256_SCRIP.20150910.nc -w ${DATA}/maps/map_ne30np4_to_fv129x256_aave.20150910.nc --method conserve
     ESMF_RegridWeightGen -s ${DATA}/grids/ne30np4_pentagons.091226.nc -d ${DATA}/grids/257x512_SCRIP.20150910.nc -w ${DATA}/maps/map_ne30np4_to_fv257x512_bilin.20150910.nc --method bilinear
     ESMF_RegridWeightGen -s ${DATA}/grids/ne120np4_pentagons.100310.nc -d ${DATA}/grids/257x512_SCRIP.20150910.nc -w ${DATA}/maps/map_ne120np4_to_fv257x512_aave.20150910.nc --method conserve
     ESMF_RegridWeightGen -s ${DATA}/grids/ne120np4_pentagons.100310.nc -d ${DATA}/grids/801x1600_SCRIP.20150910.nc -w ${DATA}/maps/map_ne120np4_to_fv801x1600_bilin.20150910.nc --method bilinear

     AMWG grids: AMWG diagnostics (until ~2016) mis-diagnose FV grids with odd numbers of latitudes as Gaussian Grids
     ncks -O -D 1 --rgr ttl='CAM FV-scalar grid 96x144 for horizontal resolution 1.9x2.5 degrees' --rgr grid=${DATA}/grids/96x144_SCRIP.20160301.nc --rgr latlon=96,144 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='CAM FV-scalar grid 192x288 for horizontal resolution 0.9x1.25 degrees' --rgr grid=${DATA}/grids/192x288_SCRIP.20160301.nc --rgr latlon=192,288 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='CAM FV-scalar grid 128x256 for horizontal resolution 1.4x1.4 degrees' --rgr grid=${DATA}/grids/128x256_SCRIP.20160301.nc --rgr latlon=128,256 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='CAM FV-scalar grid 256x512 for horizontal resolution 0.7x0.7 degrees' --rgr grid=${DATA}/grids/256x512_SCRIP.20160301.nc --rgr latlon=256,512 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='CAM FV-scalar grid 800x1600 for horizontal resolution 0.225x0.225 degrees' --rgr grid=${DATA}/grids/800x1600_SCRIP.20160301.nc --rgr latlon=800,1600 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='Equiangular grid 360x720 produced by RTM' --rgr grid=${DATA}/grids/360x720rtm_SCRIP.20160301.nc --rgr latlon=360,720 --rgr lat_typ=eqa --rgr lon_typ=180_wst ~/nco/data/in.nc ~/foo.nc

     AMWG maps old method (no provenance archived):
     ESMF_RegridWeightGen -s ${DATA}/grids/ne30np4_pentagons.091226.nc -d ${DATA}/grids/128x256_SCRIP.20160301.nc -w ${DATA}/maps/map_ne30np4_to_fv128x256_aave.20160301.nc --method conserve
     ESMF_RegridWeightGen -s ${DATA}/grids/ne30np4_pentagons.091226.nc -d ${DATA}/grids/256x512_SCRIP.20160301.nc -w ${DATA}/maps/map_ne30np4_to_fv256x512_bilin.20160301.nc --method bilinear
     ESMF_RegridWeightGen -s ${DATA}/grids/ne30np4_pentagons.091226.nc -d ${DATA}/grids/256x512_SCRIP.20160301.nc -w ${DATA}/maps/map_ne30np4_to_fv256x512_aave.20160301.nc --method conserve
     ESMF_RegridWeightGen -s ${DATA}/grids/ne30np4_pentagons.091226.nc -d ${DATA}/grids/800x1600_SCRIP.20160301.nc -w ${DATA}/maps/map_ne30np4_to_fv800x1600_bilin.20160301.nc --method bilinear

     AMWG maps with ncremap (preferred method):
     ncremap -s ${DATA}/grids/ne30np4_pentagons.091226.nc -g ${DATA}/grids/128x256_SCRIP.20160301.nc -m ${DATA}/maps/map_ne30np4_to_fv128x256_aave.20160301.nc -w esmf -a conserve
     ncremap -s ${DATA}/grids/ne30np4_pentagons.091226.nc -g ${DATA}/grids/256x512_SCRIP.20160301.nc -m ${DATA}/maps/map_ne30np4_to_fv256x512_bilin.20160301.nc -w esmf -a bilinear
     ncremap -s ${DATA}/grids/ne120np4_pentagons.100310.nc -g ${DATA}/grids/256x512_SCRIP.20160301.nc -m ${DATA}/maps/map_ne120np4_to_fv256x512_aave.20160301.nc -w esmf -a conserve
     ncremap -s ${DATA}/grids/ne120np4_pentagons.100310.nc -g ${DATA}/grids/800x1600_SCRIP.20160301.nc -m ${DATA}/maps/map_ne120np4_to_fv800x1600_bilin.20160301.nc -w esmf -a bilinear

     MPAS grids:
     NCO cannot yet generate MPAS grids, but given an MPAS grid it can generate appropriate maps

     MPAS maps:
     ncremap -s ${DATA}/grids/oEC60to30.SCRIP.150729.nc -g ${DATA}/grids/t62_SCRIP.20150901.nc -m ${DATA}/maps/map_oEC60to30_to_t62_aave.20160301.nc -w esmf -a conserve
     ncremap -s ${DATA}/grids/oEC60to30.SCRIP.150729.nc -g ${DATA}/grids/t62_SCRIP.20150901.nc -m ${DATA}/maps/map_oEC60to30_to_t62_bilin.20160301.nc -w esmf -a bilinear

     Regional RLL grids:
     ncks -O -D 1 --rgr ttl='Equiangular grid 180x360' --rgr grid=${DATA}/sld/rgr/grd_dst.nc --rgr latlon=100,100 --rgr snwe=30.0,70.0,-120.0,-90.0 ~/nco/data/in.nc ~/foo.nc

     Global RLL skeleton:
     ncks -O -D 1 --rgr ttl='Equiangular grid 180x360' --rgr skl=${DATA}/sld/rgr/skl_180x360.nc --rgr grid=${DATA}/grids/180x360_SCRIP.20150901.nc --rgr latlon=180,360#lat_typ=eqa#lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc

     Curvilinear grids:
     ncks -O -D 1 --rgr ttl='Curvilinear grid 10x20. Degenerate case.' --rgr crv --rgr lon_crv=0.0 --rgr skl=${DATA}/sld/rgr/skl_crv.nc --rgr grid=${DATA}/sld/rgr/grd_crv.nc --rgr latlon=10,20 --rgr snwe=-5.0,5.0,-10.0,10.0 ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='Curvilinear grid 10x20. Curvilinearity = 1.0 lon' --rgr lon_crv=1.0 --rgr skl=${DATA}/sld/rgr/skl_crv.nc --rgr grid=${DATA}/sld/rgr/grd_crv.nc --rgr latlon=10,20 --rgr snwe=-5.0,5.0,-10.0,10.0 ~/nco/data/in.nc ~/foo.nc

     1-D Latitude (no longitude) grids:
     ncks -O -D 1 --rgr ttl='Latitude-only zonal grid' --rgr skl=${DATA}/sld/rgr/skl_lat_10dgr_uni.nc --rgr grid=${DATA}/sld/rgr/grd_lat_10dgr_uni.nc --rgr latlon=18,1 --rgr snwe=-90,90,0,360  ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr ttl='Latitude-only zonal grid' --rgr skl=${DATA}/sld/rgr/skl_lat_05dgr_cap.nc --rgr grid=${DATA}/sld/rgr/grd_lat_05dgr_cap.nc --rgr latlon=37,1 --rgr snwe=-90,90,0,360  ~/nco/data/in.nc ~/foo.nc
     ncremap -i ${DATA}/sld/rgr/skl_lat_10dgr_uni.nc -d ${DATA}/sld/rgr/skl_lat_05dgr_cap.nc -m ${DATA}/maps/map_lat10uni_to_lat05cap_aave.nc -o ~/rgr/lat10to05.nc
     ESMF_RegridWeightGen -s ${DATA}/sld/rgr/grd_lat_10dgr_uni.nc -d ${DATA}/sld/rgr/grd_lat_05dgr_cap.nc -w ${DATA}/maps/map_lat10uni_to_lat05cap_aave.nc --method conserve */

  const char fnc_nm[]="nco_grd_mk()"; /* [sng] Function name */

  const double rdn2dgr=180.0/M_PI;
  const double dgr2rdn=M_PI/180.0;

  const int dmn_nbr_1D=1; /* [nbr] Rank of 1-D grid variables */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_3D=3; /* [nbr] Rank of 3-D grid variables */
  const int dmn_nbr_grd_max=dmn_nbr_3D; /* [nbr] Maximum rank of grid variables */
  const int itr_nbr_max=20; // [nbr] Maximum number of iterations
 
  const nc_type crd_typ=NC_DOUBLE;

  char *fl_out_tmp=NULL_CEWI;
  char *fl_out;
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
  
  double *grd_ctr_lat; /* [dgr] Latitude  centers of grid */
  double *grd_ctr_lon; /* [dgr] Longitude centers of grid */
  double *grd_crn_lat; /* [dgr] Latitude  corners of grid */
  double *grd_crn_lon; /* [dgr] Longitude corners of grid */
  double *area; /* [sr] Area of grid */
  double *lat_bnd=NULL_CEWI; /* [dgr] Latitude  boundaries of rectangular grid */
  double *lat_crn=NULL; /* [dgr] Latitude  corners of rectangular grid */
  double *lat_ctr=NULL_CEWI; /* [dgr] Latitude  centers of rectangular grid */
  double *lat_ntf=NULL; /* [dgr] Latitude  interfaces of rectangular grid */
  double *lat_wgt=NULL; /* [dgr] Latitude  weights of rectangular grid */
  double *lon_bnd=NULL_CEWI; /* [dgr] Longitude boundaries of rectangular grid */
  double *lon_crn=NULL; /* [dgr] Longitude corners of rectangular grid */
  double *lon_ctr=NULL_CEWI; /* [dgr] Longitude centers of rectangular grid */
  double *lon_ntf=NULL; /* [dgr] Longitude interfaces of rectangular grid */

  double area_ttl=0.0; /* [frc] Exact sum of area */
  double lat_crv; /* [dgr] Latitudinal  curvilinearity */
  double lon_crv; /* [dgr] Longitudinal curvilinearity */
  double lat_nrt; /* [dgr] Latitude of northern edge of grid */
  double lat_sth; /* [dgr] Latitude of southern edge of grid */
  double lat_wgt_ttl=0.0; /* [frc] Actual sum of quadrature weights */
  double lat_wgt_gss; /* [frc] Latitude weight estimated from interface latitudes */
  double lon_est; /* [dgr] Longitude of eastern edge of grid */
  double lon_wst; /* [dgr] Longitude of western edge of grid */
  double lon_ncr; /* [dgr] Longitude increment */
  double lat_ncr; /* [dgr] Latitude increment */
  double lon_spn; /* [dgr] Longitude span */
  double lat_spn; /* [dgr] Latitude span */
  double *wgt_Gss=NULL; // [frc] Gaussian weights double precision

  int *msk=NULL; /* [flg] Mask of grid */
  int *dmn_sz_int; /* [nbr] Array of dimension sizes of grid */

  int dmn_ids[dmn_nbr_grd_max]; /* [id] Dimension IDs array for output variable */

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_out_fmt=NC_FORMAT_CLASSIC; /* [enm] Output file format */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;

  int area_id; /* [id] Area variable ID */
  int dmn_id_grd_crn; /* [id] Grid corners dimension ID */
  int dmn_id_grd_rnk; /* [id] Grid rank dimension ID */
  int dmn_id_grd_sz; /* [id] Grid size dimension ID */
  int dmn_sz_int_id; /* [id] Grid dimension sizes ID */
  int grd_crn_lat_id; /* [id] Grid corner latitudes  variable ID */
  int grd_crn_lon_id; /* [id] Grid corner longitudes variable ID */
  int grd_ctr_lat_id; /* [id] Grid center latitudes  variable ID */
  int grd_ctr_lon_id; /* [id] Grid center longitudes variable ID */
  int itr_cnt; /* Iteration counter */
  int msk_id; /* [id] Mask variable ID */

  long dmn_srt[dmn_nbr_grd_max];
  long dmn_cnt[dmn_nbr_grd_max];

  long bnd_nbr; /* [nbr] Number of bounds in gridcell */
  long col_nbr; /* [nbr] Number of columns in grid */
  long crn_idx; /* [idx] Counting index for corners */
  long grd_crn_nbr; /* [nbr] Number of corners in gridcell */
  long grd_rnk_nbr; /* [nbr] Number of dimensions in grid */
  long grd_sz_nbr; /* [nbr] Number of gridcells in grid */
  long idx2; /* [idx] Counting index for unrolled grids */
  long idx; /* [idx] Counting index for unrolled grids */
  long lat_idx2; /* [idx] Counting index for unrolled latitude */
  long lat_idx;
  long lat_nbr; /* [nbr] Number of latitudes in grid */
  long lon_idx2; /* [idx] Counting index for unrolled longitude */
  long lon_idx;
  long lon_nbr; /* [nbr] Number of longitudes in grid */
  
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=rgr->flg_uio; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=rgr->flg_uio; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */
  nco_bool flg_grd_1D=False;
  nco_bool flg_grd_2D=False;
  nco_bool flg_grd_crv=False;
  nco_bool flg_s2n=True; /* I [enm] Latitude grid-direction is South-to-North */

  nco_grd_2D_typ_enm grd_typ; /* [enm] Grid-type enum */
  nco_grd_lat_drc_enm lat_drc; /* [enm] Latitude grid-direction enum */
  nco_grd_lat_typ_enm lat_typ; /* [enm] Latitude grid-type enum */
  nco_grd_lon_typ_enm lon_typ; /* [enm] Longitude grid-type enum */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  dfl_lvl=rgr->dfl_lvl;
  grd_typ=rgr->grd_typ; /* [enm] Grid type */
  fl_out=rgr->fl_grd;
  fl_out_fmt=rgr->fl_out_fmt;
  lat_drc=rgr->lat_drc; /* [enm] Latitude grid direction */
  lat_typ=rgr->lat_typ; /* [enm] Latitude grid type */
  lon_typ=rgr->lon_typ; /* [enm] Longitude grid type */
  lat_nbr=rgr->lat_nbr; /* [nbr] Number of latitudes in grid */
  lon_nbr=rgr->lon_nbr; /* [nbr] Number of longitudes in grid */
  lat_crv=rgr->lat_crv; /* [dgr] Latitude  curvilinearity */
  lon_crv=rgr->lon_crv; /* [dgr] Longitude curvilinearity */
  lat_sth=rgr->lat_sth; /* [dgr] Latitude of southern edge of grid */
  lon_wst=rgr->lon_wst; /* [dgr] Longitude of western edge of grid */
  lat_nrt=rgr->lat_nrt; /* [dgr] Latitude of northern edge of grid */
  lon_est=rgr->lon_est; /* [dgr] Longitude of eastern edge of grid */

  /* Use curvilinear coordinates (lat and lon are 2D arrays) if flg_crv already set or it lat_crv or lon_crv set */
  if(lat_crv != 0.0 || lon_crv != 0.0 || rgr->flg_crv) flg_grd_crv=True;
  if(lat_drc == nco_grd_lat_drc_n2s) flg_s2n=False;

  /* Assume 2D grid */
  flg_grd_2D=True;
  grd_rnk_nbr=dmn_nbr_2D;
  /* Assume quadrilaterals */
  grd_crn_nbr=4;
  /* Assume rectangles */
  bnd_nbr=2;
  col_nbr=lat_nbr*lon_nbr;
  grd_sz_nbr=lat_nbr*lon_nbr;

  /* Allocate space for output data */
  area=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
  dmn_sz_int=(int *)nco_malloc(grd_rnk_nbr*nco_typ_lng((nc_type)NC_INT));
  msk=(int *)nco_malloc(grd_sz_nbr*nco_typ_lng((nc_type)NC_INT));
  
  lat_bnd=(double *)nco_malloc(lat_nbr*bnd_nbr*nco_typ_lng(crd_typ));
  lat_crn=(double *)nco_malloc(lat_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
  lat_ctr=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
  lat_ntf=(double *)nco_malloc((lat_nbr+1L)*nco_typ_lng(crd_typ));
  lat_wgt=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
  lon_bnd=(double *)nco_malloc(lon_nbr*bnd_nbr*nco_typ_lng(crd_typ));
  lon_crn=(double *)nco_malloc(lon_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
  lon_ctr=(double *)nco_malloc(lon_nbr*nco_typ_lng(crd_typ));
  lon_ntf=(double *)nco_malloc((lon_nbr+1L)*nco_typ_lng(crd_typ));
  wgt_Gss=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
  
  grd_ctr_lat=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
  grd_ctr_lon=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
  grd_crn_lat=(double *)nco_malloc(grd_crn_nbr*grd_sz_nbr*nco_typ_lng(crd_typ));
  grd_crn_lon=(double *)nco_malloc(grd_crn_nbr*grd_sz_nbr*nco_typ_lng(crd_typ));
  
  /* Define variable values */
  int lon_psn=int_CEWI; /* [idx] Ordinal position of longitude in rectangular grid dimension-size array */
  int lat_psn=int_CEWI; /* [idx] Ordinal position of latitude  in rectangular grid dimension-size array */
  if(grd_rnk_nbr == dmn_nbr_2D){
    lon_psn=0; /* SCRIP introduced [lon,lat] convention because more natural for Fortran */
    lat_psn=1;
  } /* !flg_grd_in_2D */
  dmn_sz_int[lon_psn]=lon_nbr;
  dmn_sz_int[lat_psn]=lat_nbr;

  for(idx=0;idx<grd_sz_nbr;idx++) msk[idx]=1;

  /* Compute rectangular arrays
     NB: Much is a more-generic rewrite of map/map_grd.F90:map_grd_mk() */

  /* 20150827: 
     Old rule: Longitude grid was entirely specified by one of four longitude map tokens: Grn_ctr,Grn_wst,180_ctr,180_wst
     New rule: User may specify bounds (lon_wst,lon_est,lat_sth,lat_nrt) independently of grid token
     Such bounds ALWAYS refer bounding box interface edges, NEVER to centers of first last gridcells
     Bounds and number of gridcells completely determine uniform grid so former longitude-type tokens have no effect when bounds specified (so letting grid-type tokens affect grid would over-determine grid and lead to errors)
     Hence, grid-type tokens may be used as short-hand to specify grids but may not be required to exist later (because regional grids would not have specified them)
     Grid grid-type tokens lon_bb/lat_bb imply bounding box was originally used to specify bounds
     1x1 degree global grid with first longitude centered at Greenwich:
     --lon_nbr=360 --lon_typ Grn_ctr
     --lon_nbr=360 --lon_wst=-0.5 --lon_est=359.5
     1x1 degree global grid with Greenwich at west edge of first longitude:
     --lon_nbr=360 --lon_typ Grn_wst
     --lon_nbr=360 --lon_wst=0.0 --lon_est=360.0
     1x1 degree regional grid, total size 9x9 degrees, Greenwich at center of middle gridcell:
     --lon_nbr=9 --lon_wst=-4.5 --lon_est=4.5
     1x1 degree regional grid, total size 10x10 degrees, Greenwich at east/west edges of middle two gridcells
     --lon_nbr=10 --lon_wst=-5.0 --lon_est=5.0 */
  
  /* Were east/west longitude bounds set explicitly or implicitly?
     NB: This is redundant since it was done in nco_rgr_ini(), yet better safe than sorry */
  if(lon_wst != NC_MAX_DOUBLE || lon_est != NC_MAX_DOUBLE) lon_typ=rgr->lon_typ=nco_grd_lon_bb;
  
  if(lon_wst == NC_MAX_DOUBLE){
    /* Precomputed longitude grids begin with longitude 0.0 or -180.0 degrees */
    switch(lon_typ){
    case nco_grd_lon_bb:
    case nco_grd_lon_Grn_ctr:
    case nco_grd_lon_Grn_wst:
      lon_wst=0.0;
      break;
    case nco_grd_lon_180_ctr:
    case nco_grd_lon_180_wst:
      lon_wst=-180.0;
      break;
    default:
      nco_dfl_case_generic_err(); break;
    } /* !lon_typ */
  } /* !lon */

  if(lon_est == NC_MAX_DOUBLE){
    /* Precomputed longitude grids end with longitude 360.0 or 180.0 degrees */
    switch(lon_typ){
    case nco_grd_lon_bb:
    case nco_grd_lon_Grn_ctr:
    case nco_grd_lon_Grn_wst:
      lon_est=360.0;
      break;
    case nco_grd_lon_180_ctr:
    case nco_grd_lon_180_wst:
      lon_est=180.0;
      break;
    default:
      nco_dfl_case_generic_err(); break;
    } /* !lon_typ */
  } /* !lon */

  /* Determine longitude increment from span of pre-centered bounding box (centering will not change span) */
  lon_spn=lon_est-lon_wst;
  lon_ncr=lon_spn/lon_nbr;

  /* Centering: If user did not set explicit longitude bounds then... */
  if(lon_typ != nco_grd_lon_bb)
    /* map_lon_ctr_typ determines whether lon_wst refers to cell center or Western edge */
    if((lon_typ == nco_grd_lon_Grn_ctr) || (lon_typ == nco_grd_lon_180_ctr)) lon_wst=lon_wst-(lon_ncr/2.0);

  /* Re-derive lon_est from lon_wst and lon_nbr (more fundamental properties) */
  lon_est=lon_wst+lon_ncr*lon_nbr;

  /* lon_wst and lon_est have been set and will not change */
  assert(lon_wst < lon_est);
  lon_ntf[0L]=lon_wst;
  lon_ntf[lon_nbr]=lon_est;

  for(lon_idx=1L;lon_idx<lon_nbr;lon_idx++)
    lon_ntf[lon_idx]=lon_ntf[0L]+lon_idx*lon_ncr;
  /* Ensure rounding errors do not produce unphysical grid */
  lon_ntf[lon_nbr]=lon_ntf[0L]+lon_spn;
  
  /* Finished with longitude, now tackle latitude */
  
  /* Were south/north latitude bounds set explicitly or implicitly? */
  //  if(lat_sth != NC_MAX_DOUBLE || lat_nrt != NC_MAX_DOUBLE) lon_typ=rgr->lat_typ=nco_grd_lat_bb;
  if(lat_sth == NC_MAX_DOUBLE) lat_sth=-90.0;
  if(lat_nrt == NC_MAX_DOUBLE) lat_nrt=90.0;
  
  /* Determine latitude increment from span of pre-centered bounding box (centering will not change span) */
  lat_spn=lat_nrt-lat_sth;
  lat_ncr=lat_spn/lat_nbr;

  const long lat_nbr_hlf=lat_nbr/2L; // [nbr] Half number of latitudes (e.g., lat_nbr_hlf=32 for lat_nbr=64 and 65)
  double *lat_sin=NULL; // [frc] Sine of Gaussian latitudes double precision
  /* Create S->N grid. If user requested N->S, flip grid at end */
  //  if(flg_s2n) lat_ntf[0L]=lat_sth; else lat_ntf[0L]=lat_nrt;
  lat_ntf[0L]=lat_sth;
  switch(lat_typ){
  case nco_grd_lat_fv:
    lat_ncr=lat_spn/(lat_nbr-1L);
    lat_ntf[1L]=lat_ntf[0L]+0.5*lat_ncr;
    for(lat_idx=2L;lat_idx<lat_nbr;lat_idx++)
      lat_ntf[lat_idx]=lat_ntf[1L]+(lat_idx-1L)*lat_ncr;
    break;
  case nco_grd_lat_eqa:
    lat_ncr=lat_spn/lat_nbr;
    for(lat_idx=1L;lat_idx<lat_nbr;lat_idx++)
      lat_ntf[lat_idx]=lat_ntf[0L]+lat_idx*lat_ncr;
    break;
  case nco_grd_lat_gss:
    lat_sin=(double *)nco_malloc(lat_nbr*sizeof(double));
    (void)nco_lat_wgt_gss(lat_nbr,True,lat_sin,wgt_Gss);
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++)
      lat_ctr[lat_idx]=rdn2dgr*asin(lat_sin[lat_idx]);
    /* First guess for lat_ntf is midway between Gaussian abscissae */
    for(lat_idx=1L;lat_idx<lat_nbr;lat_idx++)
      lat_ntf[lat_idx]=0.5*(lat_ctr[lat_idx-1L]+lat_ctr[lat_idx]);
    /* Iterate guess until area between interfaces matches Gaussian weight (compute for one hemisphere, make other symmetric) */
    for(lat_idx=1L;lat_idx<lat_nbr_hlf;lat_idx++){
      double fofx_at_x0; /* [frc] Function to iterate evaluated at current guess */
      double dfdx_at_x0; /* [frc] Derivative of equation evaluated at current guess */
      const double eps_rlt_cnv=1.0e-15; // Convergence criterion (1.0e-16 pushes double precision to the brink)
      itr_cnt=0;
      lat_wgt_gss=fabs(sin(dgr2rdn*lat_ntf[lat_idx])-sin(dgr2rdn*lat_ntf[lat_idx-1L]));
      fofx_at_x0=wgt_Gss[lat_idx-1L]-lat_wgt_gss;
      while(fabs(fofx_at_x0) > eps_rlt_cnv){
	/* Newton-Raphson iteration:
	   Let x=lat_ntf[lat_idx], y0=lat_ntf[lat_idx-1L], gw = Gaussian weight (exact solution)
	       f(x)=sin(dgr2rdn*x)-sin(dgr2rdn*y0)-gw=0 # s2n grid
	       f(x)=sin(dgr2rdn*y0)-sin(dgr2rdn*x)-gw=0 # n2s grid
	       dfdx(x)= dgr2rdn*cos(dgr2rdn*x) # s2n grid
	       dfdx(x)=-dgr2rdn*cos(dgr2rdn*x) # n2s grid
	       x_better=x0-f(x0)/f'(x0) */
	dfdx_at_x0=dgr2rdn*cos(dgr2rdn*lat_ntf[lat_idx]);
	/* 20190613: n2s latitudes are constructed s2n and flipped to n2s later
	   Hence next line is commented-out in construction mode but used in infer mode */
	// if(!flg_s2n) dfdx_at_x0=-dfdx_at_x0;
	lat_ntf[lat_idx]+=fofx_at_x0/dfdx_at_x0; /* NB: not sure why this is minus not plus but it works :) */
	lat_wgt_gss=fabs(sin(dgr2rdn*lat_ntf[lat_idx])-sin(dgr2rdn*lat_ntf[lat_idx-1L]));
	fofx_at_x0=wgt_Gss[lat_idx-1L]-lat_wgt_gss;
	if(++itr_cnt > itr_nbr_max){
	  (void)fprintf(stdout,"%s: ERROR %s reports convergence only %g after %d iterations for lat_idx = %ld\n",nco_prg_nm_get(),fnc_nm,fabs(fofx_at_x0),itr_nbr_max,lat_idx);
	  nco_exit(EXIT_FAILURE);
	} /* endif */
      } /* !while */
    } /* !lat_idx */
    /* Use Gaussian grid symmetry to obtain same interfaces in both hemispheres (avoids cumulative rounding errors) */
    if(lat_nbr%2){
      /* lat_nbr is odd */
      for(lat_idx=1L;lat_idx<=lat_nbr_hlf+1L;lat_idx++) lat_ntf[lat_nbr_hlf+lat_idx]=-lat_ntf[lat_nbr_hlf-lat_idx+1L];
    }else{
      /* lat_nbr is even */
      for(lat_idx=1L;lat_idx<lat_nbr_hlf;lat_idx++) lat_ntf[lat_nbr_hlf+lat_idx]=-lat_ntf[lat_nbr_hlf-lat_idx];
    } /* !flg_lat_evn */
    break;
  default:
    nco_dfl_case_generic_err(); break;
  } /* !lat_typ */
  /* Ensure rounding errors do not produce unphysical grid */
  lat_ntf[lat_nbr]=lat_nrt;
  
  if(nco_dbg_lvl_get() > nco_dbg_old){
    (void)fprintf(stderr,"%s: DEBUG %s Gaussian abscissae/interfaces for lat_nbr=%ld\n",nco_prg_nm_get(),fnc_nm,lat_nbr);
    (void)fprintf(stderr,"idx\tlat_ctr\tlat_ntf\tntf_p1\n");
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++){
      (void)fprintf(stderr,"%ld\t%20.15f\t%20.15f\t%20.15f\n",lat_idx,lat_ctr[lat_idx],lat_ntf[lat_idx],lat_ntf[lat_idx+1L]);
    } /* !lat_idx */
  } /* !dbg */
  
  /* Always define longitude centers midway between interfaces */
  for(lon_idx=0L;lon_idx<=lon_nbr-1L;lon_idx++)
    lon_ctr[lon_idx]=0.5*(lon_ntf[lon_idx]+lon_ntf[lon_idx+1L]);

  /* Many grids have center latitude equally spaced between interfaces */
  if(lat_typ != nco_grd_lat_fv && lat_typ != nco_grd_lat_gss){
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++)
      lat_ctr[lat_idx]=0.5*(lat_ntf[lat_idx]+lat_ntf[lat_idx+1L]);
  } /* !lat_typ */
  
  /* Cap grids excepted---they place centers of first/last gridcells at poles */
  if(lat_typ == nco_grd_lat_fv){
    lat_ctr[0L]=lat_ntf[0L];
    for(lat_idx=1L;lat_idx<lat_nbr-1L;lat_idx++)
      lat_ctr[lat_idx]=0.5*(lat_ntf[lat_idx]+lat_ntf[lat_idx+1L]);
    lat_ctr[lat_nbr-1L]=lat_ntf[lat_nbr];
  } /* !cap */
  /* Gaussian grid centerpoints are defined by solutions to Legendre polynomials */
  if(lat_typ == nco_grd_lat_gss){
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++)
      lat_ctr[lat_idx]=rdn2dgr*asin(lat_sin[lat_idx]);
  } /* !Gaussian */
  
  for(idx=0L;idx<lon_nbr;idx++){
    lon_bnd[2*idx]=lon_ntf[idx];
    lon_bnd[2*idx+1L]=lon_ntf[idx+1L];
  } /* !idx */
  for(idx=0L;idx<lat_nbr;idx++){
    lat_bnd[2*idx]=lat_ntf[idx];
    lat_bnd[2*idx+1L]=lat_ntf[idx+1L];
  } /* !idx */
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    for(idx=0L;idx<lat_nbr;idx++){
    (void)fprintf(stdout,"lat[%li] = %g, vertices = ",idx,lat_ctr[idx]);
    for(int bnd_idx=0L;bnd_idx<bnd_nbr;bnd_idx++)
      (void)fprintf(stdout,"%s%g%s",bnd_idx == 0 ? "[" : "",lat_bnd[bnd_nbr*idx+bnd_idx],bnd_idx == bnd_nbr-1 ? "]\n" : ", ");
    } /* end loop over lat */
  } /* endif dbg */

  /* Use centers and boundaries to diagnose latitude weights */
  switch(lat_typ){
  case nco_grd_lat_eqa:
  case nco_grd_lat_fv:
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=fabs(sin(dgr2rdn*lat_ntf[lat_idx+1L])-sin(dgr2rdn*lat_ntf[lat_idx]));
    break;
  case nco_grd_lat_gss:
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=wgt_Gss[lat_idx];
    break;
  default:
    nco_dfl_case_generic_err(); break;
  } /* !lat_typ */

  /* Fuzzy test of latitude weight normalization
     20180903 Tolerance threshold of eps_rlt_max=1.0e-14 is too strict for Gaussian grids somewhere lat_nbr >~ 150
     20180904 Tolerance threshold of eps_rlt_max=1.0e-12 allows Gaussian grids like ECMWF O1280
     Newton-Raphson method of interface determination may need improvement to fix that
     Tolerance threshold of 1.0e-14 works for all relevant E3SM Uniform and Cap grids */
  //const double eps_rlt_max=1.0e-14; /* [frc] Round-off error tolerance: Used 1.0e-14 until 20180904 */
  const double eps_rlt_max=1.0e-12; /* [frc] Round-off error tolerance: Used 1.0e-12 since 20180904 */
  double lat_wgt_ttl_xpc; /* [frc] Expected sum of latitude weights */
  lat_wgt_ttl=0.0;
  for(idx=0L;idx<lat_nbr;idx++) lat_wgt_ttl+=lat_wgt[idx];
  lat_wgt_ttl_xpc=fabs(sin(dgr2rdn*lat_bnd[2*(lat_nbr-1)+1L])-sin(dgr2rdn*lat_bnd[0L]));
  if(grd_typ != nco_grd_2D_unk && 1.0-lat_wgt_ttl/lat_wgt_ttl_xpc > eps_rlt_max){
    (void)fprintf(stdout,"%s: ERROR %s reports grid normalization does not meet precision tolerance eps_rlt_max = %20.15f\nlat_wgt_ttl = %20.15f, lat_wgt_ttl_xpc = %20.15f, lat_wgt_frc = %20.15f, eps_rlt = %20.15f\n",nco_prg_nm_get(),fnc_nm,eps_rlt_max,lat_wgt_ttl,lat_wgt_ttl_xpc,lat_wgt_ttl/lat_wgt_ttl_xpc,1.0-lat_wgt_ttl/lat_wgt_ttl_xpc);
    nco_exit(EXIT_FAILURE);
  } /* !imprecise */

  /* 20180831 Code above assumes grids run S->N
     User can request N->S grids with --rgr lat_drc=n2s
     If so, flip grid before unrolling into output arrays */
  if(!flg_s2n){
    double *lat_ctr_tmp=NULL_CEWI; /* [dgr] Temporary Latitude centers of rectangular grid */
    double *lat_wgt_tmp=NULL; /* [dgr] Temporary Latitude weights of rectangular grid */
    double *lat_ntf_tmp=NULL; /* [dgr] Temporary Latitude interfaces of rectangular grid */
    lat_ctr_tmp=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
    lat_ntf_tmp=(double *)nco_malloc((lat_nbr+1L)*nco_typ_lng(crd_typ));
    lat_wgt_tmp=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
    long tmp_idx; /* [idx] Temporary index for swapping values */
    for(idx=0L;idx<lat_nbr;idx++){
      lat_ctr_tmp[idx]=lat_ctr[idx];
      lat_wgt_tmp[idx]=lat_wgt[idx];
    } /* !idx */
    for(idx=0L;idx<lat_nbr;idx++){
      tmp_idx=lat_nbr-idx-1L;
      lat_ctr[idx]=lat_ctr_tmp[tmp_idx];
      lat_wgt[idx]=lat_wgt_tmp[tmp_idx];
    } /* !idx */
    for(idx=0L;idx<lat_nbr+1L;idx++){
      lat_ntf_tmp[idx]=lat_ntf[idx];
    } /* !idx */
    for(idx=0L;idx<lat_nbr+1L;idx++){
      tmp_idx=lat_nbr+1L-idx-1L; /* NB: Subtle index difference */
      lat_ntf[idx]=lat_ntf_tmp[tmp_idx];
    } /* !idx */
    for(idx=0L;idx<lat_nbr;idx++){
      lat_bnd[2*idx]=lat_ntf[idx];
      lat_bnd[2*idx+1L]=lat_ntf[idx+1L];
    } /* !idx */
    if(lat_ctr_tmp) lat_ctr_tmp=(double *)nco_free(lat_ctr_tmp);
    if(lat_ntf_tmp) lat_ntf_tmp=(double *)nco_free(lat_ntf_tmp);
    if(lat_wgt_tmp) lat_wgt_tmp=(double *)nco_free(lat_wgt_tmp);
  } /* !flg_s2n */
  
  assert(grd_crn_nbr == 4);
  for(lon_idx=0L;lon_idx<lon_nbr;lon_idx++){
    idx=grd_crn_nbr*lon_idx;
    lon_crn[idx]=lon_ntf[lon_idx];
    lon_crn[idx+1L]=lon_ntf[lon_idx+1L];
    lon_crn[idx+2L]=lon_ntf[lon_idx+1L];
    lon_crn[idx+3L]=lon_ntf[lon_idx];
  } /* !lon_idx */

  for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++){
    idx=grd_crn_nbr*lat_idx;
    lat_crn[idx]=lat_ntf[lat_idx];
    lat_crn[idx+1L]=lat_ntf[lat_idx];
    lat_crn[idx+2L]=lat_ntf[lat_idx+1L];
    lat_crn[idx+3L]=lat_ntf[lat_idx+1L];
  } /* !lat_idx */
  
  /* Stuff rectangular arrays into unrolled arrays */
  for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++){
    for(lon_idx=0L;lon_idx<lon_nbr;lon_idx++){
      idx=lat_idx*lon_nbr+lon_idx;
      grd_ctr_lat[idx]=lat_ctr[lat_idx];
      grd_ctr_lon[idx]=lon_ctr[lon_idx];
      for(crn_idx=0L;crn_idx<grd_crn_nbr;crn_idx++){
	idx2=grd_crn_nbr*idx+crn_idx;
	lat_idx2=lat_idx*grd_crn_nbr+crn_idx;
	lon_idx2=lon_idx*grd_crn_nbr+crn_idx;
	grd_crn_lat[idx2]=lat_crn[lat_idx2];
	grd_crn_lon[idx2]=lon_crn[lon_idx2];
      } /* !crn */
    } /* !lon */
  } /* !lat */
  
  if(flg_grd_crv){
    /* Impose curvilinearity by adding lon_crv offset to each row relative to previous row, and lat_crv offset to each column relative to previous column */
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++){
      for(lon_idx=0L;lon_idx<lon_nbr;lon_idx++){
	idx=lat_idx*lon_nbr+lon_idx;
	grd_ctr_lat[idx]+=lon_idx*lat_crv;
	grd_ctr_lon[idx]+=lat_idx*lon_crv;
	for(crn_idx=0L;crn_idx<grd_crn_nbr;crn_idx++){
	  idx2=grd_crn_nbr*idx+crn_idx;
	  lat_idx2=lat_idx*grd_crn_nbr+crn_idx;
	  lon_idx2=lon_idx*grd_crn_nbr+crn_idx;
	  grd_crn_lat[idx2]=lat_crn[lat_idx2];
	  grd_crn_lon[idx2]=lon_crn[lon_idx2];
	  if(crn_idx == 0L || crn_idx == 1L){
	    grd_crn_lat[idx2]+=lat_idx*lat_crv; /* LL, LR */
	    grd_crn_lon[idx2]+=lat_idx*lon_crv; /* LL, LR */
	  }else if(crn_idx == 2L || crn_idx == 3L){
	    grd_crn_lat[idx2]+=(lat_idx+1L)*lat_crv; /* UL, UR */
	    grd_crn_lon[idx2]+=(lat_idx+1L)*lon_crv; /* UL, UR */
	  } /* !crn */
	} /* !crn */
      } /* !lon */
    } /* !lat */
  } /* !flg_grd_crv */

  /* 20190613: Convert CW quadrilaterals to CCW quadrilaterals so TempestRemap accepts grids
     Default construction/inferral method orders corners CCW and CW for s2n and n2s grids, respectively */
  if(!flg_s2n){
    nco_bool flg_ccw; /* [flg] Gridcell is CCW */
    const int rcr_lvl=1; /* [nbr] Recursion level (1 is top level, 2 and greater are recursed */
    const int idx_ccw=0; /* [idx] Index of starting vertice for CCW check (Point A = tail side AB) */
    for(idx=0L;idx<grd_sz_nbr;idx++){
      idx2=grd_crn_nbr*idx;
      flg_ccw=nco_ccw_chk(grd_crn_lat+idx2,grd_crn_lon+idx2,grd_crn_nbr,idx_ccw,rcr_lvl);
      if(!flg_ccw && nco_dbg_lvl_get() >= nco_dbg_vec) (void)fprintf(stderr,"%s: DEBUG %s reports nco_ccw_chk() tried to change idx = %lu from CW to CCW\n",nco_prg_nm_get(),fnc_nm,idx);
    } /* !idx */
  } /* !flg_s2n */
 
  if(nco_dbg_lvl_get() >= nco_dbg_std){
    long int idx_crn_ll;
    long int idx_crn_lr;
    long int idx_crn_ur;
    long int idx_crn_ul;
    long idx_dbg;
    idx_dbg=rgr->idx_dbg;
    idx_crn_ll=grd_crn_nbr*idx_dbg+0L;
    idx_crn_lr=grd_crn_nbr*idx_dbg+1L;
    idx_crn_ur=grd_crn_nbr*idx_dbg+2L;
    idx_crn_ul=grd_crn_nbr*idx_dbg+3L;
    (void)fprintf(stderr,"%s: INFO %s idx_dbg = %li, Center [lat,lon]=[%g,%g]; Corners LL [%g,%g] LR [%g,%g] UR [%g,%g] UL [%g,%g]\n",nco_prg_nm_get(),fnc_nm,idx_dbg,grd_ctr_lat[idx_dbg],grd_ctr_lon[idx_dbg],grd_crn_lat[idx_crn_ll],grd_crn_lon[idx_crn_ll],grd_crn_lat[idx_crn_lr],grd_crn_lon[idx_crn_lr],grd_crn_lat[idx_crn_ur],grd_crn_lon[idx_crn_ur],grd_crn_lat[idx_crn_ul],grd_crn_lon[idx_crn_ul]);
  } /* !dbg */

  if(flg_grd_crv){
    /* Area of arbitrary curvilinear grids requires spherical trigonometry */
    nco_sph_plg_area(rgr,grd_crn_lat,grd_crn_lon,grd_sz_nbr,grd_crn_nbr,area);
  }else{
    /* Area of rectangular spherical zones from elementary calculus results
       20150906: Half-angle formulae for better conditioning improve area normalization for 801x1600 by 2.0e-15 
       area[lat_idx*lon_nbr+lon_idx]=dgr2rdn*(lon_bnd[2*lon_idx+1L]-lon_bnd[2*lon_idx])*2.0*(sin(0.5*dgr2rdn*lat_bnd[2*lat_idx+1L])*cos(0.5*dgr2rdn*lat_bnd[2*lat_idx+1L])-sin(0.5*dgr2rdn*lat_bnd[2*lat_idx])*cos(0.5*dgr2rdn*lat_bnd[2*lat_idx])); 
       Gain not worth the extra complexity */
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++)
      for(lon_idx=0L;lon_idx<lon_nbr;lon_idx++)
	/* fabs() ensures positive area in n2s grids */
	area[lat_idx*lon_nbr+lon_idx]=fabs(dgr2rdn*(lon_bnd[2*lon_idx+1L]-lon_bnd[2*lon_idx])*(sin(dgr2rdn*lat_bnd[2*lat_idx+1L])-sin(dgr2rdn*lat_bnd[2*lat_idx])));
  } /* !flg_grd_2D */

  if(nco_dbg_lvl_get() >= nco_dbg_sbr){
    lat_wgt_ttl=0.0;
    area_ttl=0.0;
    if(flg_grd_2D){
      (void)fprintf(stderr,"%s: INFO %s reports destination rectangular latitude grid:\n",nco_prg_nm_get(),fnc_nm);
      for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++)
	lat_wgt_ttl+=lat_wgt[lat_idx];
    } /* !flg_grd_2D */
    for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++)
      for(lon_idx=0L;lon_idx<lon_nbr;lon_idx++)
	area_ttl+=area[lat_idx*lon_nbr+lon_idx];
    (void)fprintf(stdout,"lat_wgt_ttl = %20.15f, frc_lat_wgt = %20.15f, area_ttl = %20.15f, frc_area = %20.15f\n",lat_wgt_ttl,lat_wgt_ttl/2.0,area_ttl,area_ttl/(4.0*M_PI));
    assert(area_ttl > 0.0);
    assert(area_ttl <= 4.0*M_PI);
  } /* endif dbg */

  /* Open grid file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  /* Define dimensions */
  rcd=nco_def_dim(out_id,grd_crn_nm,grd_crn_nbr,&dmn_id_grd_crn);
  rcd=nco_def_dim(out_id,grd_sz_nm,grd_sz_nbr,&dmn_id_grd_sz);
  rcd=nco_def_dim(out_id,grd_rnk_nm,grd_rnk_nbr,&dmn_id_grd_rnk);
  
  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */
  deflate=(int)True;
  shuffle=NC_SHUFFLE;

  /* Define variables */
  (void)nco_def_var(out_id,dmn_sz_nm,(nc_type)NC_INT,dmn_nbr_1D,&dmn_id_grd_rnk,&dmn_sz_int_id); /* NB: Too small to deflate */
  (void)nco_def_var(out_id,grd_area_nm,(nc_type)crd_typ,dmn_nbr_1D,&dmn_id_grd_sz,&area_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_id,shuffle,deflate,dfl_lvl);
  (void)nco_def_var(out_id,msk_nm,(nc_type)NC_INT,dmn_nbr_1D,&dmn_id_grd_sz,&msk_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,msk_id,shuffle,deflate,dfl_lvl);
  (void)nco_def_var(out_id,grd_ctr_lat_nm,crd_typ,dmn_nbr_1D,&dmn_id_grd_sz,&grd_ctr_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_ctr_lat_id,shuffle,deflate,dfl_lvl);
  (void)nco_def_var(out_id,grd_ctr_lon_nm,crd_typ,dmn_nbr_1D,&dmn_id_grd_sz,&grd_ctr_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_ctr_lon_id,shuffle,deflate,dfl_lvl);
  dmn_ids[0]=dmn_id_grd_sz;
  dmn_ids[1]=dmn_id_grd_crn;
  (void)nco_def_var(out_id,grd_crn_lat_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lat_id,shuffle,deflate,dfl_lvl);
  dmn_ids[0]=dmn_id_grd_sz;
  dmn_ids[1]=dmn_id_grd_crn;
  (void)nco_def_var(out_id,grd_crn_lon_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lon_id,shuffle,deflate,dfl_lvl);

  /* Define global and "units" attributes */
  char *att_val;
  
  rcd=nco_char_att_put(out_id,NULL,"title",rgr->grd_ttl);
  rcd=nco_char_att_put(out_id,NULL,"Conventions","SCRIP");
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */
  rcd=nco_char_att_put(out_id,NULL,"created_by",usr_cpp);
  rcd=nco_char_att_put(out_id,NULL,"grid_generator","NCO");
  (void)nco_hst_att_cat(out_id,rgr->cmd_ln);
  (void)nco_vrs_att_cat(out_id);
  rcd=nco_char_att_put(out_id,NULL,"latitude_grid_type",nco_grd_lat_sng(lat_typ));
  rcd=nco_char_att_put(out_id,NULL,"longitude_grid_type",nco_grd_lon_sng(lon_typ));

  rcd=nco_char_att_put(out_id,dmn_sz_nm,"long_name","Size(s) of horizontal dimensions (in Fortran storage order for historical reasons)");

  rcd=nco_char_att_put(out_id,grd_area_nm,"long_name","Solid Angle Subtended on Source Grid");
  rcd=nco_char_att_put(out_id,grd_area_nm,"standard_name","solid_angle");
  rcd=nco_char_att_put(out_id,grd_area_nm,"units","steradian");

  rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"long_name","Latitude of Grid Cell Centers");
  rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"standard_name","latitude");
  if(rgr->flg_cf_units) rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"units","degrees_north"); else rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"units","degrees"); /* 20191009: ERWG 7.1.0r- breaks on CF-compliant units strings */
  rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"bounds",grd_crn_lat_nm);
  
  rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"long_name","Longitude of Grid Cell Centers");
  rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"standard_name","longitude");
  if(rgr->flg_cf_units) rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"units","degrees_east"); else rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"units","degrees"); /* 20191009: ERWG 7.1.0r- breaks on CF-compliant units strings */
  rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"bounds",grd_crn_lon_nm);
  
  rcd=nco_char_att_put(out_id,grd_crn_lat_nm,"long_name","Latitude of Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,grd_crn_lat_nm,"standard_name","latitude");
  if(rgr->flg_cf_units) rcd=nco_char_att_put(out_id,grd_crn_lat_nm,"units","degrees_north"); else rcd=nco_char_att_put(out_id,grd_crn_lat_nm,"units","degrees"); /* 20191009: ERWG 7.1.0r- breaks on CF-compliant units strings */
  
  rcd=nco_char_att_put(out_id,grd_crn_lon_nm,"long_name","Longitude of Grid Cell Vertices");
  rcd=nco_char_att_put(out_id,grd_crn_lon_nm,"standard_name","longitude");
  if(rgr->flg_cf_units) rcd=nco_char_att_put(out_id,grd_crn_lon_nm,"units","degrees_east"); else rcd=nco_char_att_put(out_id,grd_crn_lon_nm,"units","degrees"); /* 20191009: ERWG 7.1.0r- breaks on CF-compliant units strings */
  
  rcd=nco_char_att_put(out_id,msk_nm,"long_name","Binary Integer Mask for Grid");
  rcd=nco_char_att_put(out_id,msk_nm,"units","none");

  /* Begin data mode */
  (void)nco_enddef(out_id);
  
  /* Write variables */
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_rnk_nbr;
  rcd=nco_put_vara(out_id,dmn_sz_int_id,dmn_srt,dmn_cnt,dmn_sz_int,(nc_type)NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  rcd=nco_put_vara(out_id,area_id,dmn_srt,dmn_cnt,area,crd_typ);
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  rcd=nco_put_vara(out_id,msk_id,dmn_srt,dmn_cnt,msk,(nc_type)NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  rcd=nco_put_vara(out_id,grd_ctr_lat_id,dmn_srt,dmn_cnt,grd_ctr_lat,crd_typ);
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  rcd=nco_put_vara(out_id,grd_ctr_lon_id,dmn_srt,dmn_cnt,grd_ctr_lon,crd_typ);
  dmn_srt[0]=0L;
  dmn_srt[1]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  dmn_cnt[1]=grd_crn_nbr;
  rcd=nco_put_vara(out_id,grd_crn_lat_id,dmn_srt,dmn_cnt,grd_crn_lat,crd_typ);
  dmn_srt[0]=0L;
  dmn_srt[1]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  dmn_cnt[1]=grd_crn_nbr;
  rcd=nco_put_vara(out_id,grd_crn_lon_id,dmn_srt,dmn_cnt,grd_crn_lon,crd_typ);
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  fl_out=rgr->fl_skl;
  if(fl_out){
    /* Write skeleton data file on requested grid
       Skeleton file can then be populated with data for testing */
    char *area_nm;
    char *bnd_nm;
    //    char *bnd_tm_nm;
    char *col_nm_out;
    char *lat_nm_out; /* [sng] Name of output dimension for latitude */
    char *lat_wgt_nm;
    char *lon_nm_out; /* [sng] Name of variable to recognize as longitude */
    char *lat_bnd_nm; /* [sng] Name of latitude  boundary variable */
    char *lon_bnd_nm; /* [sng] Name of longitude boundary variable */

    //    int area_id; /* [id] Variable ID for area */
    int dmn_id_bnd; /* [id] Dimension ID */
    //int dmn_id_bnd_tm; /* [id] Dimension ID */
    int dmn_id_col; /* [id] Dimension ID */
    int dmn_id_lat; /* [id] Dimension ID */
    int dmn_id_lon; /* [id] Dimension ID */
    int lat_bnd_id; /* [id] Variable ID for lat_bnds/lat_vertices */
    int lat_id; /* [id] Variable ID for latitude */
    int lat_wgt_id; /* [id] Variable ID for latitude weight */
    int lon_bnd_id; /* [id] Variable ID for lon_bnds/lon_vertices */
    int lon_id; /* [id] Variable ID for longitude */
    
    /* Use explicitly specified output names, if any, otherwise use input names (either explicitly specified or discovered by fuzzing) */
    if(rgr->lat_nm_out) lat_nm_out=rgr->lat_nm_out; else lat_nm_out=(char *)strdup("lat");
    if(rgr->lon_nm_out) lon_nm_out=rgr->lon_nm_out; else lon_nm_out=(char *)strdup("lon");
    if(rgr->col_nm_out) col_nm_out=rgr->col_nm_out; else col_nm_out=(char *)strdup("ncol");

    /* Name output dimensions */
    area_nm=rgr->area_nm;
    bnd_nm=rgr->bnd_nm;
    //bnd_tm_nm=rgr->bnd_tm_nm;
    lat_bnd_nm=rgr->lat_bnd_nm;
    lat_wgt_nm=rgr->lat_wgt_nm;
    lon_bnd_nm=rgr->lon_bnd_nm;
    /* Use names discovered by fuzzing */
    if(flg_grd_1D){
      bnd_nm=rgr->vrt_nm;
      lat_bnd_nm=rgr->lat_vrt_nm;
      lon_bnd_nm=rgr->lon_vrt_nm;
    } /* !flg_grd_1D */
    if(flg_grd_2D){
      bnd_nm=rgr->bnd_nm;
      lat_bnd_nm=rgr->lat_bnd_nm;
      lon_bnd_nm=rgr->lon_bnd_nm;
    } /* !flg_grd_2D */
    
    /* Open grid file */
    fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);
    
    /* Define dimensions */
    if(flg_grd_crv){
      rcd=nco_def_dim(out_id,bnd_nm,grd_crn_nbr,&dmn_id_bnd);
    }else{
      rcd=nco_def_dim(out_id,bnd_nm,bnd_nbr,&dmn_id_bnd);
    } /* !flg_grd_crv */
    if(flg_grd_1D){
      rcd=nco_def_dim(out_id,col_nm_out,col_nbr,&dmn_id_col);
    } /* !flg_grd_1D */
    if(flg_grd_2D){
      rcd=nco_def_dim(out_id,lat_nm_out,lat_nbr,&dmn_id_lat);
      rcd=nco_def_dim(out_id,lon_nm_out,lon_nbr,&dmn_id_lon);
    } /* !flg_grd_2D */
    
    /* Define new coordinates and variables in regridded file */
    if(flg_grd_1D){
      (void)nco_def_var(out_id,lat_nm_out,crd_typ,dmn_nbr_1D,&dmn_id_col,&lat_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_id,shuffle,deflate,dfl_lvl);
      (void)nco_def_var(out_id,lon_nm_out,crd_typ,dmn_nbr_1D,&dmn_id_col,&lon_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_id,shuffle,deflate,dfl_lvl);
      dmn_ids[0]=dmn_id_col;
      dmn_ids[1]=dmn_id_bnd;
      (void)nco_def_var(out_id,lat_bnd_nm,crd_typ,dmn_nbr_2D,dmn_ids,&lat_bnd_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_bnd_id,shuffle,deflate,dfl_lvl);
      dmn_ids[0]=dmn_id_col;
      dmn_ids[1]=dmn_id_bnd;
      (void)nco_def_var(out_id,lon_bnd_nm,crd_typ,dmn_nbr_2D,dmn_ids,&lon_bnd_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_bnd_id,shuffle,deflate,dfl_lvl);
      (void)nco_def_var(out_id,area_nm,crd_typ,dmn_nbr_1D,&dmn_id_col,&area_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_id,shuffle,deflate,dfl_lvl);
    } /* !flg_grd_1D */
    if(flg_grd_crv){
      dmn_ids[0]=dmn_id_lat;
      dmn_ids[1]=dmn_id_lon;
      (void)nco_def_var(out_id,lat_nm_out,crd_typ,dmn_nbr_2D,dmn_ids,&lat_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_id,shuffle,deflate,dfl_lvl);
      (void)nco_def_var(out_id,lon_nm_out,crd_typ,dmn_nbr_2D,dmn_ids,&lon_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_id,shuffle,deflate,dfl_lvl);
      (void)nco_def_var(out_id,area_nm,crd_typ,dmn_nbr_2D,dmn_ids,&area_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_id,shuffle,deflate,dfl_lvl);
      dmn_ids[0]=dmn_id_lat;
      dmn_ids[1]=dmn_id_lon;
      dmn_ids[2]=dmn_id_bnd;
      (void)nco_def_var(out_id,lat_bnd_nm,crd_typ,dmn_nbr_3D,dmn_ids,&lat_bnd_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_bnd_id,shuffle,deflate,dfl_lvl);
      (void)nco_def_var(out_id,lon_bnd_nm,crd_typ,dmn_nbr_3D,dmn_ids,&lon_bnd_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_bnd_id,shuffle,deflate,dfl_lvl);
    }else if(flg_grd_2D){
      (void)nco_def_var(out_id,lat_nm_out,crd_typ,dmn_nbr_1D,&dmn_id_lat,&lat_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_id,shuffle,deflate,dfl_lvl);
      (void)nco_def_var(out_id,lon_nm_out,crd_typ,dmn_nbr_1D,&dmn_id_lon,&lon_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_id,shuffle,deflate,dfl_lvl);
      dmn_ids[0]=dmn_id_lat;
      dmn_ids[1]=dmn_id_bnd;
      (void)nco_def_var(out_id,lat_bnd_nm,crd_typ,dmn_nbr_2D,dmn_ids,&lat_bnd_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_bnd_id,shuffle,deflate,dfl_lvl);
      dmn_ids[0]=dmn_id_lon;
      dmn_ids[1]=dmn_id_bnd;
      (void)nco_def_var(out_id,lon_bnd_nm,crd_typ,dmn_nbr_2D,dmn_ids,&lon_bnd_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_bnd_id,shuffle,deflate,dfl_lvl);
      (void)nco_def_var(out_id,lat_wgt_nm,crd_typ,dmn_nbr_1D,&dmn_id_lat,&lat_wgt_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_wgt_id,shuffle,deflate,dfl_lvl);
      dmn_ids[0]=dmn_id_lat;
      dmn_ids[1]=dmn_id_lon;
      (void)nco_def_var(out_id,area_nm,crd_typ,dmn_nbr_2D,dmn_ids,&area_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_id,shuffle,deflate,dfl_lvl);
    } /* !flg_grd_2D */
    
    /* Define attributes */
    rcd=nco_char_att_put(out_id,NULL,"title",rgr->grd_ttl);
    rcd=nco_char_att_put(out_id,NULL,"Conventions","CF-1.6");
    rcd=nco_char_att_put(out_id,NULL,"created_by",usr_cpp);
    (void)nco_hst_att_cat(out_id,rgr->cmd_ln);
    (void)nco_vrs_att_cat(out_id);
    rcd=nco_char_att_put(out_id,NULL,"latitude_grid_type",nco_grd_lat_sng(lat_typ));
    rcd=nco_char_att_put(out_id,NULL,"longitude_grid_type",nco_grd_lon_sng(lon_typ));
    rcd=nco_char_att_put(out_id,area_nm,"long_name","Solid angle subtended by gridcell");
    rcd=nco_char_att_put(out_id,area_nm,"standard_name","solid_angle");
    rcd=nco_char_att_put(out_id,area_nm,"units","steradian");
    
    rcd=nco_char_att_put(out_id,lat_nm_out,"long_name","Latitude of Grid Cell Centers");
    rcd=nco_char_att_put(out_id,lat_nm_out,"standard_name","latitude");
    rcd=nco_char_att_put(out_id,lat_nm_out,"units","degrees_north");
    rcd=nco_char_att_put(out_id,lat_nm_out,"axis","Y");
    rcd=nco_char_att_put(out_id,lat_nm_out,"bounds",lat_bnd_nm);

    if(flg_grd_2D) att_val=strdup("Gridcell latitude interfaces"); else att_val=strdup("Gridcell latitude vertices");
    rcd=nco_char_att_put(out_id,lat_bnd_nm,"long_name",att_val);

    if(flg_grd_2D) rcd=nco_char_att_put(out_id,lat_wgt_nm,"long_name","Latitude quadrature weights (normalized to sum to 2.0 on global grids)");
    
    rcd=nco_char_att_put(out_id,lon_nm_out,"long_name","Longitude of Grid Cell Centers");
    rcd=nco_char_att_put(out_id,lon_nm_out,"standard_name","longitude");
    rcd=nco_char_att_put(out_id,lon_nm_out,"units","degrees_east");
    rcd=nco_char_att_put(out_id,lon_nm_out,"axis","X");
    rcd=nco_char_att_put(out_id,lon_nm_out,"bounds",lon_bnd_nm);

    if(flg_grd_2D) att_val=strdup("Gridcell longitude interfaces"); else att_val=strdup("Gridcell longitude vertices");
    rcd=nco_char_att_put(out_id,lon_bnd_nm,"long_name",att_val);

    /* Begin data mode */
    (void)nco_enddef(out_id);
    
    /* Write new coordinates and variables to regridded file */
    if(flg_grd_1D){
      dmn_srt[0]=0L;
      dmn_cnt[0]=col_nbr;
      (void)nco_put_vara(out_id,lat_id,dmn_srt,dmn_cnt,lat_ctr,crd_typ);
      dmn_srt[0]=0L;
      dmn_cnt[0]=col_nbr;
      (void)nco_put_vara(out_id,lon_id,dmn_srt,dmn_cnt,lon_ctr,crd_typ);
      dmn_srt[0]=dmn_srt[1]=0L;
      dmn_cnt[0]=col_nbr;
      dmn_cnt[1]=bnd_nbr;
      (void)nco_put_vara(out_id,lat_bnd_id,dmn_srt,dmn_cnt,lat_bnd,crd_typ);
      dmn_srt[0]=dmn_srt[1]=0L;
      dmn_cnt[0]=col_nbr;
      dmn_cnt[1]=bnd_nbr;
      (void)nco_put_vara(out_id,lon_bnd_id,dmn_srt,dmn_cnt,lon_bnd,crd_typ);
      dmn_srt[0]=0L;
      dmn_cnt[0]=col_nbr;
      (void)nco_put_vara(out_id,area_id,dmn_srt,dmn_cnt,area,crd_typ);
    } /* !flg_grd_1D */
    if(flg_grd_crv){
      dmn_srt[0]=dmn_srt[1]=0L;
      dmn_cnt[0]=lat_nbr;
      dmn_cnt[1]=lon_nbr;
      (void)nco_put_vara(out_id,lat_id,dmn_srt,dmn_cnt,grd_ctr_lat,crd_typ);
      (void)nco_put_vara(out_id,lon_id,dmn_srt,dmn_cnt,grd_ctr_lon,crd_typ);
      (void)nco_put_vara(out_id,area_id,dmn_srt,dmn_cnt,area,crd_typ);
      dmn_srt[0]=dmn_srt[1]=0L;dmn_srt[2]=0L;
      dmn_cnt[0]=lat_nbr;
      dmn_cnt[1]=lon_nbr;
      dmn_cnt[2]=grd_crn_nbr;
      (void)nco_put_vara(out_id,lat_bnd_id,dmn_srt,dmn_cnt,grd_crn_lat,crd_typ);
      (void)nco_put_vara(out_id,lon_bnd_id,dmn_srt,dmn_cnt,grd_crn_lon,crd_typ);
    }else if(flg_grd_2D){
      dmn_srt[0]=0L;
      dmn_cnt[0]=lat_nbr;
      (void)nco_put_vara(out_id,lat_id,dmn_srt,dmn_cnt,lat_ctr,crd_typ);
      dmn_srt[0]=0L;
      dmn_cnt[0]=lon_nbr;
      (void)nco_put_vara(out_id,lon_id,dmn_srt,dmn_cnt,lon_ctr,crd_typ);
      dmn_srt[0]=0L;
      dmn_cnt[0]=lat_nbr;
      (void)nco_put_vara(out_id,lat_wgt_id,dmn_srt,dmn_cnt,lat_wgt,crd_typ);
      dmn_srt[0]=dmn_srt[1]=0L;
      dmn_cnt[0]=lat_nbr;
      dmn_cnt[1]=bnd_nbr;
      (void)nco_put_vara(out_id,lat_bnd_id,dmn_srt,dmn_cnt,lat_bnd,crd_typ);
      dmn_srt[0]=dmn_srt[1]=0L;
      dmn_cnt[0]=lon_nbr;
      dmn_cnt[1]=bnd_nbr;
      (void)nco_put_vara(out_id,lon_bnd_id,dmn_srt,dmn_cnt,lon_bnd,crd_typ);
      dmn_srt[0]=dmn_srt[1]=0L;
      dmn_cnt[0]=lat_nbr;
      dmn_cnt[1]=lon_nbr;
      (void)nco_put_vara(out_id,area_id,dmn_srt,dmn_cnt,area,crd_typ);
    } /* !flg_grd_2D */
    
    /* Close output file and move it from temporary to permanent location */
    (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  } /* !fl_out */
  
  /* Free memory associated with input file */
  if(dmn_sz_int) dmn_sz_int=(int *)nco_free(dmn_sz_int);
  if(msk) msk=(int *)nco_free(msk);
  if(area) area=(double *)nco_free(area);
  if(grd_ctr_lat) grd_ctr_lat=(double *)nco_free(grd_ctr_lat);
  if(grd_ctr_lon) grd_ctr_lon=(double *)nco_free(grd_ctr_lon);
  if(grd_crn_lat) grd_crn_lat=(double *)nco_free(grd_crn_lat);
  if(grd_crn_lon) grd_crn_lon=(double *)nco_free(grd_crn_lon);
  if(lat_bnd) lat_bnd=(double *)nco_free(lat_bnd);
  if(lat_crn) lat_crn=(double *)nco_free(lat_crn);
  if(lat_ctr) lat_ctr=(double *)nco_free(lat_ctr);
  if(lat_ntf) lat_ntf=(double *)nco_free(lat_ntf);
  if(lat_sin) lat_sin=(double *)nco_free(lat_sin);
  if(lat_wgt) lat_wgt=(double *)nco_free(lat_wgt);
  if(lon_bnd) lon_bnd=(double *)nco_free(lon_bnd);
  if(lon_crn) lon_crn=(double *)nco_free(lon_crn);
  if(lon_ctr) lon_ctr=(double *)nco_free(lon_ctr);
  if(lon_ntf) lon_ntf=(double *)nco_free(lon_ntf);
  if(wgt_Gss) wgt_Gss=(double *)nco_free(wgt_Gss);
  
  return rcd;
} /* !nco_grd_mk() */

int /* O [enm] Return code */
nco_grd_nfr /* [fnc] Infer SCRIP-format grid file from input data file */
(rgr_sct * const rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Use grid information and guesswork to create SCRIP-format grid file from input data file
     
     Test curvilinear grids:
     ncks -O -D 1 --rgr infer --rgr grid=${DATA}/sld/rgr/grd_airs.nc ${DATA}/sld/raw/AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.nc ~/foo.nc
     ncks -O -D 1 --rgr infer --rgr grid=${DATA}/sld/rgr/grd_airs.nc ${DATA}/sld/raw/AIRS.2014.10.01.202.L2.TSurfStd.Regrid010.1DLatLon.hole.nc ~/foo.nc */

  const char fnc_nm[]="nco_grd_nfr()"; /* [sng] Function name */

  const double rdn2dgr=180.0/M_PI;
  const double dgr2rdn=M_PI/180.0;

  const int dmn_nbr_0D=0; /* [nbr] Rank of 0-D grid variables */
  const int dmn_nbr_1D=1; /* [nbr] Rank of 1-D grid variables */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_grd_max=4; /* [nbr] Maximum rank of grid variables (msk_[src/dst] could be rank 4) */
  const int itr_nbr_max=20; // [nbr] Maximum number of iterations
  const int idx_ccw=0; /* [idx] Index of starting vertice for CCW check (Point A = tail side AB) */
  const int rcr_lvl=1; /* [nbr] Recursion level (1 is top level, 2 and greater are recursed */
 
  const nc_type crd_typ=NC_DOUBLE;

  char *area_nm_in=NULL;
  char *fl_in;
  char *fl_out;
  char *fl_out_tmp=NULL_CEWI;
  char *fl_pth_lcl=NULL;
  char *msk_nm_in=NULL;
  char dmn_nm[NC_MAX_NAME]; /* [sng] Dimension name */

  /* SCRIP-format grid names are non-negotiable and thus fixed not dynamic */
  char area_nm[]="grid_area"; /* 20150830: NB ESMF_RegridWeightGen --user_areas looks for variable named "grid_area" */
  char dmn_sz_nm[]="grid_dims";
  char grd_crn_lat_nm[]="grid_corner_lat";
  char grd_crn_lon_nm[]="grid_corner_lon";
  char grd_crn_nm[]="grid_corners";
  char grd_ctr_lat_nm[]="grid_center_lat";
  char grd_ctr_lon_nm[]="grid_center_lon";
  char grd_rnk_nm[]="grid_rank";
  char grd_sz_nm[]="grid_size";
  char msk_nm[]="grid_imask";
  char unt_sng[]="units"; /* netCDF-standard units attribute name */
    
  double *grd_ctr_lat; /* [dgr] Latitude  centers of grid */
  double *grd_ctr_lon; /* [dgr] Longitude centers of grid */
  double *grd_crn_lat; /* [dgr] Latitude  corners of grid */
  double *grd_crn_lon; /* [dgr] Longitude corners of grid */
  double *area; /* [sr] Area of grid */
  double *lat_bnd=NULL_CEWI; /* [dgr] Latitude  boundaries of rectangular grid */
  double *lat_crn=NULL; /* [dgr] Latitude  corners of rectangular grid */
  double *lat_ctr=NULL_CEWI; /* [dgr] Latitude  centers of rectangular grid */
  double *lat_ntf=NULL; /* [dgr] Latitude  interfaces of rectangular grid */
  double *lat_wgt=NULL; /* [dgr] Latitude  weights of rectangular grid */
  double *lon_bnd=NULL_CEWI; /* [dgr] Longitude boundaries of rectangular grid */
  double *lon_crn=NULL; /* [dgr] Longitude corners of rectangular grid */
  double *lon_ctr=NULL_CEWI; /* [dgr] Longitude centers of rectangular grid */
  double *lon_ntf=NULL; /* [dgr] Longitude interfaces of rectangular grid */
  double *vrt_lat=NULL; /* [rdn] MPAS latitude boundary variable latVertex */
  double *vrt_lon=NULL; /* [rdn] MPAS longitude boundary variable lonVertex */

  double area_ttl=0.0; /* [frc] Exact sum of area */
  //double lat_nrt; /* [dgr] Latitude of northern edge of grid */
  double lat_sth; /* [dgr] Latitude of southern edge of grid */
  double lat_wgt_ttl=0.0; /* [frc] Actual sum of quadrature weights */
  double lat_wgt_gss; /* [frc] Latitude weight estimated from interface latitudes */
  //  double lon_est; /* [dgr] Longitude of eastern edge of grid */
  double lon_wst; /* [dgr] Longitude of western edge of grid */
  double lon_ncr; /* [dgr] Longitude increment */
  double lat_ncr; /* [dgr] Latitude increment */
  double lon_spn; /* [dgr] Longitude span */
  double lat_spn; /* [dgr] Latitude span */
  double mss_val_area_dbl;
  double mss_val_ctr_dbl;
  double mss_val_msk_dbl;
  
  int *msk=NULL; /* [flg] Mask of grid */
  int *vrt_cll=NULL; /* [enm] MPAS variable verticesOnCell */
  int *dmn_sz_int; /* [nbr] Array of dimension sizes of grid */

  int dmn_ids[dmn_nbr_grd_max]; /* [id] Dimension IDs array for output variable */

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int dmn_idx; /* [idx] Dimension index */
  int fl_out_fmt=NC_FORMAT_CLASSIC; /* [enm] Output file format */
  int in_id; /* I [id] Input netCDF file ID */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;

  int area_id=NC_MIN_INT; /* [id] Area variable ID */
  int dmn_id_grd_crn; /* [id] Grid corners dimension ID */
  int dmn_id_grd_rnk; /* [id] Grid rank dimension ID */
  int dmn_id_grd_sz; /* [id] Grid size dimension ID */
  int dmn_sz_int_id; /* [id] Grid dimension sizes ID */
  int grd_crn_lat_id; /* [id] Grid corner latitudes  variable ID */
  int grd_crn_lon_id; /* [id] Grid corner longitudes variable ID */
  int grd_ctr_lat_id; /* [id] Grid center latitudes  variable ID */
  int grd_ctr_lon_id; /* [id] Grid center longitudes variable ID */
  int itr_cnt; /* Iteration counter */
  int lat_rnk; /* [nbr] Rank of latitude coordinate */
  int lon_rnk; /* [nbr] Rank of longitude coordinate */
  int lat_ctr_id=NC_MIN_INT; /* [id] Latitude centers of rectangular grid variable ID */
  int lon_ctr_id=NC_MIN_INT; /* [id] Longitude centers of rectangular grid variable ID */
  int lat_bnd_id=NC_MIN_INT; /* [id] Latitude centers of rectangular grid variable ID */
  int lon_bnd_id=NC_MIN_INT; /* [id] Longitude centers of rectangular grid variable ID */
  int msk_id=NC_MIN_INT; /* [id] Mask variable ID */
  int msk_rnk_nbr; /* [id] Mask rank */
  int mss_val_int_out=NC_MIN_INT; /* [nbr] Value that can be non-erroneously pointed to */
  int val_two=2; /* [nbr] Value that can be non-erroneously pointed to */
  int val_zero=0; /* [nbr] Value that can be non-erroneously pointed to */
  int var_id; /* [id] Current variable ID */
  int vrt_cll_id=NC_MIN_INT; /* [id] MPAS variable verticesOnCell ID */
  int vrt_lat_id=NC_MIN_INT; /* [id] MPAS latitude boundary variable latVertex ID */
  int vrt_lon_id=NC_MIN_INT; /* [id] MPAS longitude boundary variable lonVertex ID */

  long dmn_srt[dmn_nbr_grd_max];
  long dmn_cnt[dmn_nbr_grd_max];

  long bnd_idx;
  long bnd_nbr=NC_MIN_INT; /* [nbr] Number of bounds in gridcell */
  long col_idx;
  long col_nbr; /* [nbr] Number of columns in grid */
  long crn_idx; /* [idx] Counting index for corners */
  long ttl_idx; /* [idx] Total (unrolled) counting index for grid+corners */
  long dmn_sz; /* [nbr] Size of current dimension */
  long grd_crn_nbr; /* [nbr] Number of corners in gridcell */
  long grd_rnk_nbr=int_CEWI; /* [nbr] Number of dimensions in grid */
  long grd_sz_nbr; /* [nbr] Number of gridcells in grid */
  long idx2; /* [idx] Counting index for unrolled grids */
  long idx; /* [idx] Counting index for unrolled grids */
  long idx_crn;
  long idx_ctr;
  long idx_fst; /* [idx] Index offset */
  long idx_tmp; /* [idx] Temporary index */
  long lat_idx2; /* [idx] Counting index for unrolled latitude */
  long lat_idx;
  long lat_nbr; /* [nbr] Number of latitudes in grid */
  long lon_idx2; /* [idx] Counting index for unrolled longitude */
  long lon_idx;
  long lon_nbr; /* [nbr] Number of longitudes in grid */
  long vrt_idx; /* [idx] Counting index for vertices */
  long vrt_nbr; /* [nbr] Number of vertices in MPAS grid */
  
  long int idx_crn_ll;
  long int idx_crn_lr;
  long int idx_crn_ur;
  long int idx_crn_ul;
  
  nco_bool FL_RTR_RMT_LCN;
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool SHARE_CREATE=rgr->flg_uio; /* [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool SHARE_OPEN=rgr->flg_uio; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */
  nco_bool flg_1D_mpas_bnd=False; /* [flg] Unstructured input grid with MPAS bounds */
  nco_bool flg_1D_psd_rct_bnd=False; /* [flg] Unstructured input grid with pseudo-rectangular bounds */
  nco_bool flg_ccw; /* [flg] Gridcell is CCW */
  nco_bool flg_grd_1D=False;
  nco_bool flg_grd_2D=False;
  nco_bool flg_grd_crv=False;
  nco_bool flg_s2n=True; /* [enm] Latitude grid-direction is South-to-North */
  nco_bool flg_wrt_crn=True;
  nco_bool flg_crn_grd_lat_lon=False; /* [flg] Curvilinear corner array ordered non-canonically as grd_nbr,lat_nbr,lon_nbr */
  nco_bool use_mss_val_area=False;
  nco_bool has_mss_val_area=False;
  nco_bool has_mss_val_bnd=False;
  nco_bool has_mss_val_ctr=False;
  nco_bool has_mss_val_msk=False;

  nco_grd_2D_typ_enm grd_typ; /* [enm] Grid-type enum */
  nco_grd_lat_typ_enm lat_typ; /* [enm] Latitude grid-type enum */
  nco_grd_lon_typ_enm lon_typ; /* [enm] Longitude grid-type enum */
  nco_grd_xtn_enm nco_grd_xtn=nco_grd_xtn_nil; /* [enm] Grid-extent enum */

  nc_type msk_typ;

  ptr_unn msk_unn;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  /* Algorithm:
     Read grid information from input data file (aka *_in)
     Close input file
     Once grid dimensions known, allocate output grid arrays (aka *_out)
     Open output file (aka grid-file)
     Use guesswork and standard algorithms to fill-in output arrays */

  /* Duplicate (because nco_fl_mk_lcl() free()'s fl_in) */
  fl_in=(char *)strdup(rgr->fl_in);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  if(SHARE_OPEN) md_open=md_open|NC_SHARE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  char *bnd_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as bounds */
  char *col_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as column */
  char *lat_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as latitude */
  char *lon_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as longitude */
  char *lat_nm_in=NULL_CEWI; /* [sng] Name of variable to recognize as latitude */
  char *lon_nm_in=NULL_CEWI; /* [sng] Name of variable to recognize as longitude */
  char *lat_bnd_nm=NULL_CEWI; /* [sng] Name of latitude  boundary variable */
  char *lon_bnd_nm=NULL_CEWI; /* [sng] Name of longitude boundary variable */
  char *vrt_dmn_nm=NULL_CEWI; /* [sng] Name of MPAS vertices dimension nVertices */
  char *vrt_cll_nm=NULL_CEWI; /* [sng] Name of MPAS variable verticesOnCell */
  char *vrt_lat_nm=NULL_CEWI; /* [sng] Name of MPAS latitude boundary variable latVertex */
  char *vrt_lon_nm=NULL_CEWI; /* [sng] Name of MPAS longitude boundary variable lonVertex */
  int dmn_id_bnd=NC_MIN_INT; /* [id] Dimension ID for spatial bounds */
  int dmn_id_col=NC_MIN_INT; /* [id] Dimension ID for unstructured grids */
  int dmn_id_lat=NC_MIN_INT; /* [id] Dimension ID for latitude */
  int dmn_id_lon=NC_MIN_INT; /* [id] Dimension ID for longitude */
  int dmn_id_vrt=NC_MIN_INT; /* [id] Dimension ID for MPAS vertices */

  /* Begin CF-coordinates block */
  cf_crd_sct *cf=NULL;
  char *rgr_var; /* [sng] Variable for special regridding treatment */
  nco_bool flg_cf=False; /* [flg] Follow CF Coordinates convention to find and infer grid */
  rgr_var=rgr->var_nm;
  if(rgr_var){
    /* Infer grid from special variable
       Intended to be variable that has both horizontal dimensions and "coordinates" attribute, e.g.,
       ncks --cdl -m ${DATA}/hdf/narrmon-a_221_20100101_0000_000.nc | grep coordinates
       4LFTX_221_SPDY_S113:coordinates = "gridlat_221 gridlon_221" ;
       Usage:
       ncks -O -D 3 --rgr infer --rgr_var=4LFTX_221_SPDY_S113 --rgr grid=~/grd_narr.nc ${DATA}/hdf/narrmon-a_221_20100101_0000_000.nc ~/foo.nc */
    char crd_sng[]="coordinates"; /* CF-standard coordinates attribute name */
    
    cf=(cf_crd_sct *)nco_malloc(sizeof(cf_crd_sct));
    cf->crd=False; /* [flg] CF coordinates information is complete */
    cf->crd_id[0]=NC_MIN_INT; /* [id] Coordinate ID, first */
    cf->crd_id[1]=NC_MIN_INT; /* [id] Coordinate ID, second */
    cf->crd_nm[0]=NULL; /* [sng] Coordinate name, first */
    cf->crd_nm[1]=NULL; /* [sng] Coordinate name, second */
    cf->crd_sng=NULL; /* [sng] Coordinates attribute value */
    cf->dmn_id[0]=NC_MIN_INT; /* [id] Dimension ID, first */
    cf->dmn_id[1]=NC_MIN_INT; /* [id] Dimension ID, second */
    cf->dmn_nm[0]=NULL; /* [sng] Dimension name, first */
    cf->dmn_nm[1]=NULL; /* [sng] Dimension name, second */
    cf->unt_sng[0]=NULL; /* [sng] Units string, first coordinate */
    cf->unt_sng[1]=NULL; /* [sng] Units string, second coordinate */
    cf->var_id=NC_MIN_INT; /* [id] Coordinate variable ID */
    cf->var_nm=NULL; /* [sng] Coordinates variable name */
    cf->var_type=NC_NAT; /* [enm] Coordinates variable type */

    if((rcd=nco_inq_varid_flg(in_id,rgr_var,&cf->var_id)) != NC_NOERR){
      (void)fprintf(stderr,"%s: WARNING %s reports special \"coordinates\" variable %s not found. Turning-off CF coordinates search.\n",nco_prg_nm_get(),fnc_nm,rgr_var);
      goto skp_cf;
    } /* !rcd */ 

    cf->crd_sng=nco_char_att_get(in_id,cf->var_id,crd_sng);
    if(cf->crd_sng){
      cf->crd=True;
    }else{ /* !rcd && att_typ */
      (void)fprintf(stderr,"%s: WARNING %s reports coordinates variable %s does not have character-valued \"coordinates\" attribute. Turning-off CF coordinates search.\n",nco_prg_nm_get(),fnc_nm,rgr_var);
      goto skp_cf;
    } /* !rcd && att_typ */
      
    /* Valid coordinates attribute requires two coordinate names separated by space character */
    char *crd_nm[NCO_MAX_CRD_PER_VAR]; /* [sng] Coordinate name start position */
    char *crd_dpl; /* [sng] Modifiable duplicate of coordinates string */
    char *spc_ptr; /* [sng] Pointer to space character (' ') */
    int crd_nbr=0; /* [nbr] Number of names in coordinates attribute */
    int crd_spt=0; /* [nbr] Number of "spatial-like" (that include "degree" in units) coordinates */
    int crd_idx=0; /* [idx] Counter for coordinate names */
    for(crd_idx=0;crd_idx<NCO_MAX_CRD_PER_VAR;crd_idx++) crd_nm[crd_idx]=NULL;
    crd_dpl=(char *)strdup(cf->crd_sng);
    /* Search for spaces starting from end of string */
    while((spc_ptr=strrchr(crd_dpl,' '))){
      crd_nm[crd_nbr]=spc_ptr+1L;
      crd_nbr++;
      /* NUL-terminate so next search ends here */
      *spc_ptr='\0'; 
    } /* !sbs_ptr */
    /* Final coordinate name begins where coordinate string starts */
    crd_nm[crd_nbr]=crd_dpl;
    /* Change crd_nbr from 0-based index to actual coordinate number */
    crd_nbr++;
    if(crd_nbr < 2){
      (void)fprintf(stderr,"%s: WARNING %s found only %d coordinate(s) in \"coordinates\" attribute \"%s\", at least two are required. Turning-off CF coordinates search.\n",nco_prg_nm_get(),fnc_nm,crd_nbr,cf->crd_sng);
      goto skp_cf;
    } /* !crd_nbr */
    /* If more than two coordinate names are present, choose first two (searching backwards from end) with "degree" in units attributes, otherwise just choose first two */
    crd_idx=crd_spt=0;
    while(crd_spt < 2 && crd_idx < crd_nbr){
      cf->crd_nm[crd_spt]=crd_nm[crd_idx];
      if((rcd=nco_inq_varid_flg(in_id,cf->crd_nm[crd_spt],&cf->crd_id[crd_spt])) == NC_NOERR){
	cf->unt_sng[crd_spt]=nco_char_att_get(in_id,cf->crd_id[crd_spt],unt_sng);
	if(cf->unt_sng[crd_spt]){
	  if(strcasestr(cf->unt_sng[crd_spt],"degree")){
	    /* Increment count of spatial-like coordinates... */
	    crd_spt++;
	  }else{
	    /* ...or free() memory allocated during search */
	    cf->unt_sng[crd_spt]=(char *)nco_free(cf->unt_sng[crd_spt]);
	  } /* !strcasestr() */
	  crd_idx++;
	} /* !rcd && att_typ */
      } /* !rcd */ 
    } /* !crd_spt */

    /* If while()-loop above was successful, our search is over
       Otherwise, use first two coordinate names regardless of units, and print more diagnostics */
    if(crd_spt < 2){
      cf->crd_nm[0]=crd_nm[0];
      cf->crd_nm[1]=crd_nm[1];
      if((rcd=nco_inq_varid_flg(in_id,cf->crd_nm[0],&cf->crd_id[0])) != NC_NOERR){
	(void)fprintf(stderr,"%s: WARNING %s reports first coordinates variable %s not found. Turning-off CF coordinates search.\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0]);
	goto skp_cf;
      } /* !rcd */ 
      if((rcd=nco_inq_varid_flg(in_id,cf->crd_nm[1],&cf->crd_id[1])) != NC_NOERR){
	(void)fprintf(stderr,"%s: WARNING %s reports second coordinates variable %s not found. Turning-off CF coordinates search.\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[1]);
	goto skp_cf;
      } /* !rcd */ 

      cf->unt_sng[0]=nco_char_att_get(in_id,cf->crd_id[0],unt_sng);
      if(cf->unt_sng[0]){
	if(!strcasestr(cf->unt_sng[0],"degree")) (void)fprintf(stderr,"%s: WARNING %s reports first coordinates variable %s has weird units attribute = %s. May not detect correct ordering of latitude and longitude coordinates\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0],cf->unt_sng[0]);
      } /* !rcd && att_typ */
      cf->unt_sng[1]=nco_char_att_get(in_id,cf->crd_id[1],unt_sng);
      if(cf->unt_sng[1]){
	if(!strcasestr(cf->unt_sng[1],"degree")) (void)fprintf(stderr,"%s: WARNING %s reports second coordinates variable %s has weird units attribute = %s. May not detect correct ordering of latitude and longitude coordinates\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[1],cf->unt_sng[1]);
      } /* !rcd && att_typ */
    } /* !crd_spt */
      
    int crd_rnk; /* [nbr] Coordinate rank */
    rcd=nco_inq_varndims(in_id,cf->crd_id[0],&crd_rnk);
    if(crd_rnk != 2){
      (void)fprintf(stderr,"%s: INFO %s reports coordinates variable %s has %i dimension(s). Skipping CF coordinates method.\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0],crd_rnk);
      goto skp_cf;
    } /* !crd_rnk */
    rcd=nco_inq_vardimid(in_id,cf->crd_id[0],cf->dmn_id);
    cf->dmn_nm[0]=(char *)nco_malloc(NC_MAX_NAME*sizeof(NC_CHAR));
    cf->dmn_nm[1]=(char *)nco_malloc(NC_MAX_NAME*sizeof(NC_CHAR));
    rcd=nco_inq_dimname(in_id,cf->dmn_id[0],cf->dmn_nm[0]);
    rcd=nco_inq_dimname(in_id,cf->dmn_id[1],cf->dmn_nm[1]);
    
    /* "coordinates" convention does not guarantee lat, lon are specified in that order
       Use "units" values, if any, to determine order
       In absence of "units", assume order is lat, lon */ 
    nco_bool crd0_is_lat=False; /* [flg] First coordinate is latitude */
    nco_bool crd0_is_lon=False; /* [flg] First coordinate is longitude */
    nco_bool crd1_is_lat=False; /* [flg] Second coordinate is latitude */
    nco_bool crd1_is_lon=False; /* [flg] Second coordinate is longitude */
    if(cf->unt_sng[0]){
      if(!strcasecmp(cf->unt_sng[0],"degrees_north") || !strcasecmp(cf->unt_sng[0],"degree_north") || !strcasecmp(cf->unt_sng[0],"degree_N") || !strcasecmp(cf->unt_sng[0],"degrees_N") || !strcasecmp(cf->unt_sng[0],"degreeN") || !strcasecmp(cf->unt_sng[0],"degreesN")) crd0_is_lat=True;
      if(!strcasecmp(cf->unt_sng[0],"degrees_east") || !strcasecmp(cf->unt_sng[0],"degree_east") || !strcasecmp(cf->unt_sng[0],"degree_E") || !strcasecmp(cf->unt_sng[0],"degrees_E") || !strcasecmp(cf->unt_sng[0],"degreeE") || !strcasecmp(cf->unt_sng[0],"degreesE")) crd0_is_lon=True;
    } /* endif */      
    if(cf->unt_sng[1]){
      if(!strcasecmp(cf->unt_sng[1],"degrees_north") || !strcasecmp(cf->unt_sng[1],"degree_north") || !strcasecmp(cf->unt_sng[1],"degree_N") || !strcasecmp(cf->unt_sng[1],"degrees_N") || !strcasecmp(cf->unt_sng[1],"degreeN") || !strcasecmp(cf->unt_sng[1],"degreesN")) crd1_is_lat=True;
      if(!strcasecmp(cf->unt_sng[1],"degrees_east") || !strcasecmp(cf->unt_sng[1],"degree_east") || !strcasecmp(cf->unt_sng[1],"degree_E") || !strcasecmp(cf->unt_sng[1],"degrees_E") || !strcasecmp(cf->unt_sng[1],"degreeE") || !strcasecmp(cf->unt_sng[1],"degreesE")) crd1_is_lon=True;
    } /* endif */      
    assert((crd0_is_lat && crd1_is_lon) || (crd0_is_lon && crd1_is_lat));
    int idx_lat;
    int idx_lon;
    if(crd0_is_lat && crd1_is_lon){
      idx_lat=0;
      idx_lon=1;
    }else{
      idx_lat=1;
      idx_lon=0;
    } /* endif */
    
    /* Dimensions and coordinates have been vetted. Store as primary lookup names.
       Dimensions are always returned in order [LRV,MRV]=[0,1]
       LRV is along-track direction, and MRV is across-track (at least in NASA data)
       Internally we label LRV as "lat" and MRV as "lon" so that code looks similar for curvilinear and rectangular grids */
    dmn_id_lat=cf->dmn_id[0];
    dmn_id_lon=cf->dmn_id[1];
    /* Subtlety: lat_nm_in is coordinate (variable+dimension) name when specified from command-line (as in nco_grd_nfr()), dimension name when found through CF-method (as in nco_rgr_wgt()). This confusing distinction could be avoided by passing command-line dimension names through-to nco_rgr_wgt(). However, that route would require complex priorities for what to do when passing command-line coordinate names not dimension names and visa-versa. */
    //lat_nm_in=strdup(cf->dmn_nm[0]);
    //lon_nm_in=strdup(cf->dmn_nm[1]);
    lat_nm_in=strdup(cf->crd_nm[idx_lat]);
    lon_nm_in=strdup(cf->crd_nm[idx_lon]);
    /* Next four lines unnecessary in nco_rgr_wgt() which only needs dimension names (it reads input coordinates from map- not data-file) */
    lat_ctr_id=cf->crd_id[idx_lat];
    lon_ctr_id=cf->crd_id[idx_lon];
    lat_dmn_nm=strdup(cf->dmn_nm[0]);
    lon_dmn_nm=strdup(cf->dmn_nm[1]);
    
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s reports coordinates variable %s \"coordinates\" attribute \"%s\" points to coordinates %s and %s. Latitude coordinate \"%s\" has LRV (along-track) and MRV (across-track) dimensions \"%s\" and \"%s\", respectively.\n",nco_prg_nm_get(),fnc_nm,rgr_var,cf->crd_sng,cf->crd_nm[0],cf->crd_nm[1],cf->crd_nm[idx_lat],cf->dmn_nm[0],cf->dmn_nm[1]);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s Coordinates %s and %s \"units\" values are \"%s\" and \"%s\", respectively.\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0],cf->crd_nm[1],cf->unt_sng[0] ? cf->unt_sng[0] : "(non-existent)",cf->unt_sng[1] ? cf->unt_sng[1] : "(non-existent)");

    /* Clean-up CF coordinates memory */
    if(crd_dpl) crd_dpl=(char *)nco_free(crd_dpl);
    if(cf->crd_sng) cf->crd_sng=(char *)nco_free(cf->crd_sng);
    if(cf->dmn_nm[0]) cf->dmn_nm[0]=(char *)nco_free(cf->dmn_nm[0]);
    if(cf->dmn_nm[1]) cf->dmn_nm[1]=(char *)nco_free(cf->dmn_nm[1]);
    if(cf->unt_sng[0]) cf->unt_sng[0]=(char *)nco_free(cf->unt_sng[0]);
    if(cf->unt_sng[1]) cf->unt_sng[1]=(char *)nco_free(cf->unt_sng[1]);
    //    if(foo) foo=(char *)nco_free(foo);
  } /* !rgr_var */

  /* goto skp_cf */
 skp_cf: 
  /* free() any abandoned cf structure now */
  if(!flg_cf)
    if(cf) cf=(cf_crd_sct *)nco_free(cf);
  rcd=NC_NOERR;
  /* End CF-coordinates block */
  
  /* Locate fields that must be present in input file
     Required variables are usually latitude and longitude
     Currently these variables must be in root group
     This fails for, e.g., OMI L2 which has coordinates /GEOLOCATION_DATA/[Latitude,Longitude]
     fxm: Generalize with traversal table so usual suspect coordinates may be in any group */
  if(lat_ctr_id == NC_MIN_INT){
    if(rgr->lat_nm_in && (rcd=nco_inq_varid_flg(in_id,rgr->lat_nm_in,&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup(rgr->lat_nm_in);
    else if((rcd=nco_inq_varid_flg(in_id,"latitude",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("latitude");
    else if((rcd=nco_inq_varid_flg(in_id,"Latitude",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("Latitude"); /* AMSR, HIRDLS, TRMM */
    else if((rcd=nco_inq_varid_flg(in_id,"lat",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("lat"); /* CAM */
    else if((rcd=nco_inq_varid_flg(in_id,"lat_d",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("lat_d"); /* EAM dynamics grid */
    else if((rcd=nco_inq_varid_flg(in_id,"Lat",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("Lat");
    else if((rcd=nco_inq_varid_flg(in_id,"XLAT",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("XLAT"); /* WRF */
    else if((rcd=nco_inq_varid_flg(in_id,"XLAT_M",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("XLAT_M"); /* Unknown */
    else if((rcd=nco_inq_varid_flg(in_id,"LAT",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("LAT"); /* MAR/RACMO */
    else if((rcd=nco_inq_varid_flg(in_id,"TLAT",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("TLAT"); /* CICE, POP */
    else if((rcd=nco_inq_varid_flg(in_id,"ULAT",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("ULAT"); /* CICE, POP */
    else if((rcd=nco_inq_varid_flg(in_id,"latCell",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("latCell"); /* MPAS-O/I */
    else if((rcd=nco_inq_varid_flg(in_id,"nav_lat",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("nav_lat"); /* NEMO */
    else if((rcd=nco_inq_varid_flg(in_id,"rlat",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("rlat"); /* RACMO */
    else if((rcd=nco_inq_varid_flg(in_id,"global_latitude0",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("global_latitude0"); /* Oxford */
    else if((rcd=nco_inq_varid_flg(in_id,"latitude0",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("latitude0"); /* Oxford NB: Must search for global_* first */
    else if((rcd=nco_inq_varid_flg(in_id,"CO_Latitude",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("CO_Latitude"); /* MLS */
    else if((rcd=nco_inq_varid_flg(in_id,"S1_Latitude",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("S1_Latitude"); /* GPM */
    else if((rcd=nco_inq_varid_flg(in_id,"yc",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("yc"); /* RTM */
    else if((rcd=nco_inq_varid_flg(in_id,"gridlat_0",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("gridlat_0"); /* NWS HRRR */
  } /* !lat_ctr_id */
  
  if(lon_ctr_id == NC_MIN_INT){
    if(rgr->lon_nm_in && (rcd=nco_inq_varid_flg(in_id,rgr->lon_nm_in,&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup(rgr->lon_nm_in);
    else if((rcd=nco_inq_varid_flg(in_id,"longitude",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("longitude");
    else if((rcd=nco_inq_varid_flg(in_id,"Longitude",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("Longitude"); /* AMSR, TRMM */
    else if((rcd=nco_inq_varid_flg(in_id,"lon",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("lon"); /* CAM */
    else if((rcd=nco_inq_varid_flg(in_id,"lon_d",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("lon"); /* EAM dynamics grid */
    else if((rcd=nco_inq_varid_flg(in_id,"Lon",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("Lon");
    else if((rcd=nco_inq_varid_flg(in_id,"XLONG",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("XLONG"); /* WRF */
    else if((rcd=nco_inq_varid_flg(in_id,"XLONG_M",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("XLONG_M"); /* Unknown */
    else if((rcd=nco_inq_varid_flg(in_id,"LON",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("LON"); /* MAR/RACMO */
    else if((rcd=nco_inq_varid_flg(in_id,"TLON",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("TLON"); /* CICE */
    else if((rcd=nco_inq_varid_flg(in_id,"TLONG",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("TLONG"); /* POP */
    else if((rcd=nco_inq_varid_flg(in_id,"ULON",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("ULON"); /* CICE */
    else if((rcd=nco_inq_varid_flg(in_id,"ULONG",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("ULONG"); /* POP */
    else if((rcd=nco_inq_varid_flg(in_id,"lonCell",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("lonCell"); /* MPAS-O/I */
    else if((rcd=nco_inq_varid_flg(in_id,"nav_lon",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("nav_lon"); /* NEMO */
    else if((rcd=nco_inq_varid_flg(in_id,"rlon",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("rlon"); /* RACMO */
    else if((rcd=nco_inq_varid_flg(in_id,"global_longitude0",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("global_longitude0"); /* Oxford NB: Must search for global_* first */
    else if((rcd=nco_inq_varid_flg(in_id,"longitude0",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("longitude0"); /* Oxford */
    else if((rcd=nco_inq_varid_flg(in_id,"CO_Longitude",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("CO_Longitude"); /* MLS */
    else if((rcd=nco_inq_varid_flg(in_id,"S1_Longitude",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("S1_Longitude"); /* GPM */
    else if((rcd=nco_inq_varid_flg(in_id,"xc",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("xc"); /* RTM */
    else if((rcd=nco_inq_varid_flg(in_id,"gridlon_0",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("gridlon_0"); /* NWS HRRR */
  } /* !lon_ctr_id */
  
  if(!lat_nm_in || !lon_nm_in){
    (void)fprintf(stdout,"%s: ERROR %s unable to identify latitude and/or longitude variable.\nHINT: Potential causes and workarounds for this include: 1. Coordinate variables must be in the root directory (not in a group). If this might be the problem, try to \"flatten\" the input file before regridding it (see http://nco.sf.net/nco.html#flatten). 2. Horizontal dimensions with \"unusual\" names are hard to identify unless the user designates them somehow. ncremap will search for horizontal dimensions named in the \"coordinates\" attribute in a template variable specified with the \"-V rgr_var\" option. 3. NCO will also search its own internal database for likely names of horizontal coordinate variables (lat, latitude, LAT, XLAT, etc.). Contact the NCO project to have your idiosyncratic coordinate names added to the internal database.\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !lat_nm_in */
    
  /* Rank of coordinates determines whether grid is curvilinear */
  rcd+=nco_inq_varndims(in_id,lat_ctr_id,&lat_rnk);
  rcd+=nco_inq_varndims(in_id,lon_ctr_id,&lon_rnk);
  /* If lat_ctr and lon_ctr share same and only dimension then grid is unstructured */
  if(lat_rnk*lon_rnk == 1){
    rcd+=nco_inq_vardimid(in_id,lat_ctr_id,&dmn_id_lat);
    rcd+=nco_inq_vardimid(in_id,lon_ctr_id,&dmn_id_lon);
    if(dmn_id_lat == dmn_id_lon){
      dmn_id_col=dmn_id_lat;
      dmn_id_lat=NC_MIN_INT;
      dmn_id_lon=NC_MIN_INT;
      rcd+=nco_inq_dimname(in_id,dmn_id_col,dmn_nm);
      col_dmn_nm=(char *)strdup(dmn_nm);
      flg_grd_1D=True;
    } /* !unstructured */
  } /* lat_rnk == lon_rnk == 1 */
  if(lat_rnk*lon_rnk == 1 && dmn_id_lat != NC_MIN_INT && dmn_id_lon != NC_MIN_INT){
    flg_grd_crv=False;
    flg_grd_2D=True;
  } /* !lat_rnk */
  if(lat_rnk == dmn_nbr_2D || lon_rnk == dmn_nbr_2D){
    flg_grd_crv=True;
    flg_grd_2D=False;
  } /* !lat_rnk */
  if(lat_rnk > dmn_nbr_2D || lon_rnk > dmn_nbr_2D){
    (void)fprintf(stdout,"%s: ERROR %s reports an identified grid variable (%s with rank %d and/or %s with rank %d) has rank greater than two---grid variables currently must have rank 1 or 2.\nHINT: If grid variables do not vary in time, then temporally average them (with, e.g., ncwa -a time in.nc out.nc) prior to inferring grid\n",nco_prg_nm_get(),fnc_nm,lat_nm_in,lat_rnk,lon_nm_in,lon_rnk);
    nco_exit(EXIT_FAILURE);
  } /* !3D */
  if(lat_rnk*lon_rnk != 1 && lat_rnk*lon_rnk != 4) assert(False);

  /* Scrutinize coordinates for their dimensions
     NB: Unstructured already known */
  if(flg_grd_2D){
    rcd+=nco_inq_dimname(in_id,dmn_id_lat,dmn_nm);
    lat_dmn_nm=(char *)strdup(dmn_nm);
    rcd+=nco_inq_dimname(in_id,dmn_id_lon,dmn_nm);
    lon_dmn_nm=(char *)strdup(dmn_nm);
  } /* !flg_grd_2D */
  if(flg_grd_crv){
    rcd+=nco_inq_vardimid(in_id,lat_ctr_id,dmn_ids);
    /* fxm: use cf struct and match with units name, if any? normally curvilinear grid dimensions are just pixel dimensions that are not aligned north-south or east-west */
    dmn_id_lat=dmn_ids[0];
    dmn_id_lon=dmn_ids[1];
    rcd+=nco_inq_dimname(in_id,dmn_id_lat,dmn_nm);
    lat_dmn_nm=(char *)strdup(dmn_nm);
    rcd+=nco_inq_dimname(in_id,dmn_id_lon,dmn_nm);
    lon_dmn_nm=(char *)strdup(dmn_nm);
  } /* !flg_grd_crv */
  
  if(!(lat_dmn_nm && lon_dmn_nm) && !col_dmn_nm){
    (void)fprintf(stdout,"%s: ERROR %s unable to identify latitude and/or longitude dimension and/or column dimension.\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !col_dmn_nm !lat_dmn_nm !lon_dmn_nm */
    
  /* Locate spatial dimensions that may be present
     NB: bounds dimensions may present a special problem
     CAM-FV and CAM-SE use nbnd for temporal bounds and have no spatial bounds dimension
     CAM3 uses tbnd for temporal bounds and has no spatial bounds dimension
     CICE and POP use d2 for temporal bounds, and CICE uses nvertices for spatial bounds while POP uses nothing
     Hence search for nvertices before nbnd to ensure spatial bound is found first */
  if((rcd=nco_inq_dimid_flg(in_id,"nv",&dmn_id_bnd)) == NC_NOERR) bnd_dmn_nm=strdup("nv"); /* fxm */
  else if((rcd=nco_inq_dimid_flg(in_id,"nvertices",&dmn_id_bnd)) == NC_NOERR) bnd_dmn_nm=strdup("nvertices"); /* CICE */
  else if((rcd=nco_inq_dimid_flg(in_id,"maxEdges",&dmn_id_bnd)) == NC_NOERR) bnd_dmn_nm=strdup("maxEdges"); /* MPAS */
  if((rcd=nco_inq_dimid_flg(in_id,"nVertices",&dmn_id_vrt)) == NC_NOERR) vrt_dmn_nm=strdup("nVertices"); /* MPAS */
  
  /* Use dimension IDs to get dimension sizes and grid size */
  if(flg_grd_1D){
    rcd+=nco_inq_dimlen(in_id,dmn_id_col,&col_nbr);
    lat_nbr=lon_nbr=col_nbr;
  }else{
    rcd+=nco_inq_dimlen(in_id,dmn_id_lat,&lat_nbr);
    rcd+=nco_inq_dimlen(in_id,dmn_id_lon,&lon_nbr);
    col_nbr=NC_MIN_INT;
  } /* !flg_grd_1D */
  if(dmn_id_bnd != NC_MIN_INT) rcd+=nco_inq_dimlen(in_id,dmn_id_bnd,&grd_crn_nbr);
  if(dmn_id_bnd != NC_MIN_INT) rcd+=nco_inq_dimlen(in_id,dmn_id_bnd,&bnd_nbr);
  if(dmn_id_vrt != NC_MIN_INT) rcd+=nco_inq_dimlen(in_id,dmn_id_vrt,&vrt_nbr);
  
  if(flg_grd_1D){
    /* Unstructured grid (e.g., CAM-SE) */
    grd_rnk_nbr=dmn_nbr_1D;
    grd_typ=nco_grd_2D_unk;
    lat_typ=nco_grd_lat_unk;
    lon_typ=nco_grd_lon_unk;
    /* 1D grids without their own boundaries are at the mercy of the weight generator */
    if(dmn_id_bnd == NC_MIN_INT){
      (void)fprintf(stdout,"%s: WARNING %s reports an unstructured grid without spatial boundary information. NCO can copy but not infer spatial boundaries from unstructured grids. Thus NCO will not write spatial bounds to the gridfile inferred from this input file. Instead, the weight generator that ingests this gridfile must generate weights for gridcells with unknown spatial extent. This is feasible for grids and mappings where weights masquerade as areas and are determined by underlying grid and interpolation type (e.g., bilinear remapping of spectral element grid). Unfortunately, the ESMF_RegridWeightGen (ERWG) program requires cell interfaces in both grid files, so ERWG will break on this gridfile. Other weight generators such as TempestRemap may be more successful with this SCRIP file.\n",nco_prg_nm_get(),fnc_nm);
      (void)fprintf(stdout,"%s: HINT Re-run the regridder, this time adding the \"-s src_grd\" option to specify the source grid file in SCRIP format. That SCRIP file will have the spatial bounds information required by the ESMF_RegridWeightGen (ERWG) program, so that the regridder will circumvent inferring the underlying grid through its black but fragile magic.\n",nco_prg_nm_get());
      flg_wrt_crn=False;
      /* Input could actually be from grid with no polygonal definition, e.g., CAM-SE 
	 Corner number is non-deterministic since, e.g., CAM-SE dual grid can be fit to quadrilaterals, pentagons, chevrons, etc.
	 Bounds will not be diagnosed so safe to set grd_crn_nbr to harmless (though weird) value like 4
	 However, ERWG requires presence of valid corner dimension "grid_corners" and arrays in input SCRIP file 
	 So ERWG will break when reading this SCRIP file regardless of whether it contains arrays (with bogus values) 
	 By default do not write grid corner values */
      grd_crn_nbr=4;
    } /* !dmn_id_bnd */
    if(bnd_nbr == 2){
      /* Unstructured grids with bounds information (e.g., OCO2) may use a pseudo-rectangular convention of archiving
	 latitude and longitude bounds as 2xN (rather than 4XN) arrays even though cell have four corners.
	 "convention" is that two latitudes and two longitudes can specify rectangular boundary cell
	 In this case, bnd_nbr=grd_crn_nbr=2=sizeof(nv)=sizeof(nvertices) currently
	 Set number of corners to rectangular and leave bnd_nbr as is */
      grd_crn_nbr=4;
      flg_1D_psd_rct_bnd=True;
    } /* !bnd_nbr */
    if(!strcmp(bnd_dmn_nm,"maxEdges")){
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Unstructured grid has dimension \"%s\" which indicates an MPAS grid. Will attempt to locate other MPAS information (dimension nVertices and variables verticesOnCell, lonVertex, and latVertex) to construct SCRIP-compliant bounds variables...\n",nco_prg_nm_get(),bnd_dmn_nm);
      if((rcd=nco_inq_varid_flg(in_id,"verticesOnCell",&vrt_cll_id)) == NC_NOERR) vrt_cll_nm=strdup("verticesOnCell");
      if((rcd=nco_inq_varid_flg(in_id,"lonVertex",&vrt_lon_id)) == NC_NOERR) vrt_lon_nm=strdup("lonVertex");
      if((rcd=nco_inq_varid_flg(in_id,"latVertex",&vrt_lat_id)) == NC_NOERR) vrt_lat_nm=strdup("latVertex");

      if(dmn_id_vrt != NC_MIN_INT) rcd+=nco_inq_dimlen(in_id,dmn_id_vrt,&vrt_nbr);
      if(vrt_dmn_nm && vrt_cll_nm && vrt_lon_nm && vrt_lat_nm){
	flg_1D_mpas_bnd=True;
	if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Found all MPAS information needed to construct SCRIP-compliant bounds variables.\n",nco_prg_nm_get());
      }else{
	(void)fprintf(stdout,"%s: INFO Unable to find all MPAS information needed to construct SCRIP-compliant bounds variables. Will not write bounds coordinates. This will degrade usefulness of SCRIP file for regridding schemes (e.g., conservative) that require cell boundaries.\n",nco_prg_nm_get());
	(void)fprintf(stdout,"%s: HINT Often MPAS restart files contain the required bounds variables (verticesOnCell, lonVertex, latVertex) that normal MPAS data files lack. Try inferring the SCRIP grid from a restart file not a normal time-varying output dataset.\n",nco_prg_nm_get());
	flg_wrt_crn=False;
      } /* !vrt_cll_nm */
    } /* !bnd_dmn_nm */
  }else if(flg_grd_2D){ /* !flg_grd_1D */
    /* Assume 2D grid of uninitialized type */
    grd_rnk_nbr=dmn_nbr_2D;
    grd_typ=nco_grd_2D_nil;
    lat_typ=nco_grd_lat_nil;
    lon_typ=nco_grd_lon_nil;
    /* Assume rectangular grids that do not specify otherwise use quadrilaterals */
    if(dmn_id_bnd == NC_MIN_INT) grd_crn_nbr=4;
    /* Sometimes we infer from a 2D grid, like those produced by nco_grd_mk(), that has bounds with nv=2
       This signals rectangular gridcell bounds are interfaces not vertices (to save half the space)
       These rectangles really have four corners so we change grd_crn_nbr (not bnd_nbr) accordingly */
    if(grd_crn_nbr == 2) grd_crn_nbr=4;
    /* Convention is to archive only two bounds for rectangular grids (since sides are identical)
       Non-quadrilateral rectangular grids are untested */
    if(grd_crn_nbr == 4) bnd_nbr=2;
  }else if(flg_grd_crv){ /* !flg_grd_2D */
    /* Assume curvilinear grid (e.g., WRF) */
    flg_grd_2D=False;
    grd_rnk_nbr=dmn_nbr_2D;
    grd_typ=nco_grd_2D_unk;
    lat_typ=nco_grd_lat_unk;
    lon_typ=nco_grd_lon_unk;
    /* Assume curvilinear grids that do not specify otherwise use quadrilaterals */
    if(dmn_id_bnd == NC_MIN_INT) grd_crn_nbr=4;
    /* Assume quadrilaterals are, well, quadrilaterals (e.g., rhomboids) not necessarily rectangles
       Non-quadrilateral curvilinear grids are untested */
    if(grd_crn_nbr == 4) bnd_nbr=4; else assert(False);
  } /* !flg_grd_crv */

  /* Allocate space for output data */
  if(flg_grd_1D) grd_sz_nbr=col_nbr; else grd_sz_nbr=lat_nbr*lon_nbr;
  dmn_sz_int=(int *)nco_malloc(grd_rnk_nbr*nco_typ_lng((nc_type)NC_INT));
  area=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
  msk=(int *)nco_malloc(grd_sz_nbr*nco_typ_lng((nc_type)NC_INT));
  
  if(flg_grd_1D){
    if(bnd_nbr != NC_MIN_INT) lat_bnd=(double *)nco_malloc(grd_sz_nbr*bnd_nbr*nco_typ_lng(crd_typ));
    lat_crn=(double *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
    lat_ctr=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
    lat_ntf=(double *)nco_malloc((lat_nbr+1L)*nco_typ_lng(crd_typ));
    lat_wgt=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
    if(bnd_nbr != NC_MIN_INT) lon_bnd=(double *)nco_malloc(grd_sz_nbr*bnd_nbr*nco_typ_lng(crd_typ));
    lon_crn=(double *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
    lon_ctr=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
    lon_ntf=(double *)nco_malloc((lon_nbr+1L)*nco_typ_lng(crd_typ));
  }else if(flg_grd_2D){ /* !flg_grd_1D */
    lat_bnd=(double *)nco_malloc(lat_nbr*bnd_nbr*nco_typ_lng(crd_typ));
    lat_crn=(double *)nco_malloc(lat_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
    lat_ctr=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
    lat_ntf=(double *)nco_malloc((lat_nbr+1L)*nco_typ_lng(crd_typ));
    lat_wgt=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
    lon_bnd=(double *)nco_malloc(lon_nbr*bnd_nbr*nco_typ_lng(crd_typ));
    lon_crn=(double *)nco_malloc(lon_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
    lon_ctr=(double *)nco_malloc(lon_nbr*nco_typ_lng(crd_typ));
    lon_ntf=(double *)nco_malloc((lon_nbr+1L)*nco_typ_lng(crd_typ));
  }else if(flg_grd_crv){ /* !flg_grd_2D */
    lat_bnd=(double *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
    lat_crn=(double *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
    lat_ctr=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
    lat_ntf=(double *)nco_malloc((lat_nbr+1L)*nco_typ_lng(crd_typ));
    lat_wgt=(double *)nco_malloc(lat_nbr*nco_typ_lng(crd_typ));
    lon_bnd=(double *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
    lon_crn=(double *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
    lon_ctr=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
    lon_ntf=(double *)nco_malloc((lon_nbr+1L)*nco_typ_lng(crd_typ));
  } /* !flg_grd_crv */
  
  grd_ctr_lat=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
  grd_ctr_lon=(double *)nco_malloc(grd_sz_nbr*nco_typ_lng(crd_typ));
  grd_crn_lat=(double *)nco_malloc(grd_crn_nbr*grd_sz_nbr*nco_typ_lng(crd_typ));
  grd_crn_lon=(double *)nco_malloc(grd_crn_nbr*grd_sz_nbr*nco_typ_lng(crd_typ));
  
  /* Locate fields that may be present in input file */
  if((rcd=nco_inq_varid_flg(in_id,"lat_bnds",&lat_bnd_id)) == NC_NOERR) lat_bnd_nm=strdup("lat_bnds");
  else if((rcd=nco_inq_varid_flg(in_id,"latt_bounds",&lat_bnd_id)) == NC_NOERR) lat_bnd_nm=strdup("latt_bounds");
  else if((rcd=nco_inq_varid_flg(in_id,"latu_bounds",&lat_bnd_id)) == NC_NOERR) lat_bnd_nm=strdup("latu_bounds");
  else if((rcd=nco_inq_varid_flg(in_id,"lat_ntf",&lat_bnd_id)) == NC_NOERR) lat_bnd_nm=strdup("lat_ntf");
  else if((rcd=nco_inq_varid_flg(in_id,"lat_vertices",&lat_bnd_id)) == NC_NOERR) lat_bnd_nm=strdup("lat_vertices");
  else if((rcd=nco_inq_varid_flg(in_id,"latitude_bnds",&lat_bnd_id)) == NC_NOERR) lat_bnd_nm=strdup("latitude_bnds"); /* OCO2 */
  else if((rcd=nco_inq_varid_flg(in_id,"LatitudeCornerpoints",&lat_bnd_id)) == NC_NOERR) lat_bnd_nm=strdup("LatitudeCornerpoints"); /* OMI */

  if((rcd=nco_inq_varid_flg(in_id,"lon_bnds",&lon_bnd_id)) == NC_NOERR) lon_bnd_nm=strdup("lon_bnds");
  else if((rcd=nco_inq_varid_flg(in_id,"lont_bounds",&lon_bnd_id)) == NC_NOERR) lon_bnd_nm=strdup("lont_bounds");
  else if((rcd=nco_inq_varid_flg(in_id,"lonu_bounds",&lon_bnd_id)) == NC_NOERR) lon_bnd_nm=strdup("lonu_bounds");
  else if((rcd=nco_inq_varid_flg(in_id,"lon_ntf",&lon_bnd_id)) == NC_NOERR) lon_bnd_nm=strdup("lon_ntf");
  else if((rcd=nco_inq_varid_flg(in_id,"lon_vertices",&lon_bnd_id)) == NC_NOERR) lon_bnd_nm=strdup("lon_vertices");
  else if((rcd=nco_inq_varid_flg(in_id,"longitude_bnds",&lon_bnd_id)) == NC_NOERR) lon_bnd_nm=strdup("longitude_bnds"); /* OCO2 */
  else if((rcd=nco_inq_varid_flg(in_id,"LongitudeCornerpoints",&lon_bnd_id)) == NC_NOERR) lon_bnd_nm=strdup("LongitudeCornerpoints"); /* OMI */

  if((rcd=nco_inq_varid_flg(in_id,"area",&area_id)) == NC_NOERR) area_nm_in=strdup("area");
  else if((rcd=nco_inq_varid_flg(in_id,"Area",&area_id)) == NC_NOERR) area_nm_in=strdup("Area");
  else if((rcd=nco_inq_varid_flg(in_id,"areaCell",&area_id)) == NC_NOERR) area_nm_in=strdup("areaCell"); /* MPAS-O/I */
  else if((rcd=nco_inq_varid_flg(in_id,"grid_area",&area_id)) == NC_NOERR) area_nm_in=strdup("grid_area");
  else if((rcd=nco_inq_varid_flg(in_id,"area_d",&area_id)) == NC_NOERR) area_nm_in=strdup("area_d"); /* EAM dynamics grid */
  else if((rcd=nco_inq_varid_flg(in_id,"area_p",&area_id)) == NC_NOERR) area_nm_in=strdup("area_p"); /* EAM physics grid */
  // else if((rcd=nco_inq_varid_flg(in_id,"aice",&area_id)) == NC_NOERR) area_nm_in=strdup("aice"); /* CICE time-dependent ice area (3D), not total gridcell area */
  else if((rcd=nco_inq_varid_flg(in_id,"tarea",&area_id)) == NC_NOERR) area_nm_in=strdup("tarea"); /* CICE time-invariant state-variable gridcell area (2D) */
  else if((rcd=nco_inq_varid_flg(in_id,"uarea",&area_id)) == NC_NOERR) area_nm_in=strdup("uarea"); /* CICE time-invariant dynamics variables (2D) */

  msk_nm_in=rgr->msk_var;
  if(msk_nm_in){
    if(!strcasecmp(msk_nm_in,"none")){
      /* 20170814: Some variables named "*mask*" are, e.g., quality control masks not regridding masks per se */
      msk_nm_in=(char *)nco_free(msk_nm_in);
    }else{
      /* User-supplied name overrides database */
      rcd=nco_inq_varid(in_id,msk_nm_in,&msk_id);
    } /* !msk_nm_in */
  }else{
    /* Otherwise search database */
    if((rcd=nco_inq_varid_flg(in_id,"mask",&msk_id)) == NC_NOERR) msk_nm_in=strdup("mask");
    else if((rcd=nco_inq_varid_flg(in_id,"Mask",&msk_id)) == NC_NOERR) msk_nm_in=strdup("Mask");
    else if((rcd=nco_inq_varid_flg(in_id,"grid_imask",&msk_id)) == NC_NOERR) msk_nm_in=strdup("grid_imask");
    else if((rcd=nco_inq_varid_flg(in_id,"landmask",&msk_id)) == NC_NOERR) msk_nm_in=strdup("landmask"); /* ALM/CLM */
    else if((rcd=nco_inq_varid_flg(in_id,"tmask",&msk_id)) == NC_NOERR) msk_nm_in=strdup("tmask"); /* CICE */
  } /* !msk_nm_in */
  
  /* Mask field requires special handling for non-conformant models */
  if(msk_id != NC_MIN_INT){
    /* 20151201: All models tested define mask as NC_INT except CICE which uses NC_FLOAT
       20160111: Few observations tested define mask. Exceptions include AMSR and GHRSST. AMSR uses NC_SHORT to store bitmasks. Bitmask is 1 for missing data, and up to 128 for various quality levels of valid data. Hence, almost better to ignore AMSR mask variable. GHRSST uses NC_BYTE for its 3D "mask" bit-mask of surface-type values 1,2,4,8,16. */
    rcd=nco_inq_varndims(in_id,msk_id,&msk_rnk_nbr);
    if(msk_rnk_nbr != grd_rnk_nbr && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports input mask variable \"%s\" is rank %d while grid is rank %ld so will use first timestep/layer to determine output mask\n",nco_prg_nm_get(),fnc_nm,msk_nm_in,msk_rnk_nbr,grd_rnk_nbr);
    rcd=nco_inq_vartype(in_id,msk_id,&msk_typ);
    msk_unn.vp=(void *)nco_malloc(grd_sz_nbr*nco_typ_lng(msk_typ));
  } /* !msk */

  /* All grids: 
     Some real-world datasets violate convention that coordinates ought never have missing values 
     CICE lists missing value for lat/lon_ctr arrays (TLAT, TLONG) and re-uses that for bounds arrays (latt_bounds, lont_bounds) that do not bother to have their own missing value attributes
     Without counter-example, assume has_mss_val_bnd=has_mss_val_ctr and mss_val_bnd_dbl=mss_val_ctr_dbl */
  has_mss_val_bnd=has_mss_val_ctr=nco_mss_val_get_dbl(in_id,lat_ctr_id,&mss_val_ctr_dbl);
  
  char *att_val;
  char *area_unt=NULL; /* [sng] Dimensional units used in area */
  char *ngl_unt=NULL; /* [sng] Angular units used in coordinates */
  long att_sz;
  nc_type att_typ;
  nco_bool flg_area_sr=True; /* [flg] Input area is in sterradians not something weird like km2 */
  nco_bool flg_crd_rdn=False; /* [flg] Input coordinates are in radians not degrees */
  
  if(flg_grd_1D){
    /* Obtain fields that must be present in unstructured input file */
    dmn_srt[0]=0L;
    dmn_cnt[0]=col_nbr;
    rcd=nco_get_vara(in_id,lat_ctr_id,dmn_srt,dmn_cnt,lat_ctr,crd_typ);
    rcd=nco_get_vara(in_id,lon_ctr_id,dmn_srt,dmn_cnt,lon_ctr,crd_typ);

    /* Obtain fields that may be present in unstructured input file */
    if(area_id != NC_MIN_INT) rcd=nco_get_vara(in_id,area_id,dmn_srt,dmn_cnt,area,crd_typ);
    if(msk_id != NC_MIN_INT){
      if(msk_rnk_nbr > grd_rnk_nbr){
	/* Retrieve mask elements only from first horizontal grid, e.g., first timestep, first layer... */
	for(dmn_idx=0;dmn_idx<msk_rnk_nbr-grd_rnk_nbr;dmn_idx++){
	  dmn_srt[dmn_idx]=0L;
	  dmn_cnt[dmn_idx]=1L;
	} /* !dmn_idx */
	dmn_srt[dmn_idx]=0L;
	dmn_cnt[dmn_idx]=col_nbr;
      } /* !msk_rnk_nbr */
      rcd=nco_get_vara(in_id,msk_id,dmn_srt,dmn_cnt,msk_unn.vp,msk_typ);
    } /* !msk_id */
    dmn_srt[0]=dmn_srt[1]=0L;
    if(flg_1D_psd_rct_bnd){
      dmn_cnt[0]=col_nbr;
      dmn_cnt[1]=bnd_nbr;
      if(lat_bnd_id != NC_MIN_INT) rcd=nco_get_vara(in_id,lat_bnd_id,dmn_srt,dmn_cnt,lat_bnd,crd_typ);
      if(lon_bnd_id != NC_MIN_INT) rcd=nco_get_vara(in_id,lon_bnd_id,dmn_srt,dmn_cnt,lon_bnd,crd_typ);
    }else if(flg_1D_mpas_bnd){
      const long grd_crn_nbrm1=grd_crn_nbr-1L; /* [nbr] Number of corners in gridcell minus one */
      vrt_cll=(int *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng((nc_type)NC_INT));
      vrt_lat=(double *)nco_malloc(vrt_nbr*nco_typ_lng(crd_typ));
      vrt_lon=(double *)nco_malloc(vrt_nbr*nco_typ_lng(crd_typ));
      dmn_cnt[0]=col_nbr;
      dmn_cnt[1]=grd_crn_nbr;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports dimension sizes bnd_nbr=%ld, col_nbr=%ld, grd_crn_nbr=%ld, vrt_nbr=%ld\n",nco_prg_nm_get(),fnc_nm,bnd_nbr,col_nbr,grd_crn_nbr,vrt_nbr);
      if(vrt_cll_id != NC_MIN_INT) rcd=nco_get_vara(in_id,vrt_cll_id,dmn_srt,dmn_cnt,vrt_cll,(nc_type)NC_INT);
      dmn_cnt[0]=vrt_nbr;
      if(vrt_lat_id != NC_MIN_INT) rcd=nco_get_vara(in_id,vrt_lat_id,dmn_srt,dmn_cnt,vrt_lat,crd_typ);
      if(vrt_lon_id != NC_MIN_INT) rcd=nco_get_vara(in_id,vrt_lon_id,dmn_srt,dmn_cnt,vrt_lon,crd_typ);
      rcd=nco_inq_att_flg(in_id,vrt_lat_id,unt_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	att_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd+=nco_get_att(in_id,vrt_lat_id,unt_sng,att_val,att_typ);
	/* NUL-terminate attribute before using strstr() */
	att_val[att_sz]='\0';
	/* Match "radian" and "radians" */
	if(strstr(att_val,"radian")) flg_crd_rdn=True;
	if(att_val) ngl_unt=(char *)strdup(att_val);
	if(att_val) att_val=(char *)nco_free(att_val);
      } /* end rcd && att_typ */
      for(col_idx=0;col_idx<col_nbr;col_idx++){
	idx=col_idx*grd_crn_nbr;
	for(crn_idx=0;crn_idx<grd_crn_nbr;crn_idx++){
	  ttl_idx=idx+crn_idx;
	  vrt_idx=vrt_cll[ttl_idx];
	  assert(vrt_idx >= 0);
	  //if(vrt_idx >= vrt_nbr) (void)fprintf(stdout,"%s: WARNING %s input gridcell %ld corner %ld has extreme MPAS input verticesOnCell value %ld (maximum valid vertex = vrt_nbr-1 = %ld-1 = %ld)\n",nco_prg_nm_get(),fnc_nm,col_idx,crn_idx,vrt_idx,vrt_nbr,vrt_nbr-1);
	  if(vrt_idx == 0){
	    /* 20201220: Convert values of zero to neighboring valid vertex index */
	    for(idx_fst=1;idx_fst<grd_crn_nbr;idx_fst++){
	      idx_tmp=crn_idx+idx_fst;
	      /* Wrap to initial corner of this cell when candidate corner would be in next cell */
	      if(idx_tmp > grd_crn_nbrm1) idx_tmp-=grd_crn_nbr;
	      ttl_idx=idx+idx_tmp;
	      vrt_idx=vrt_cll[ttl_idx];
	      if(vrt_idx != 0) break;
	    } /* !idx_fst */
	    assert(idx_fst < grd_crn_nbr);
	  } /* !vrt_idx */
	  /* 20201220: Stored vertex indices use Fortran-based convention---subtract one for C */
	  vrt_idx--;
	  lat_crn[ttl_idx]=vrt_lat[vrt_idx];
	  lon_crn[ttl_idx]=vrt_lon[vrt_idx];
	  //(void)fprintf(stdout,"%s: DEBUG %s reports col_idx = %ld, crn_idx = %ld, ttl_idx = %ld, vrt_idx = %ld, vrt_lat = %g, vrt_lon = %g\n",nco_prg_nm_get(),fnc_nm,col_idx,crn_idx,ttl_idx,vrt_idx,vrt_lat[vrt_idx],vrt_lon[vrt_idx]);
	} /* !crn_idx */
      } /* !col_idx */
    }else{ /* !flg_1D_mpas_bnd */
      dmn_cnt[0]=col_nbr;
      dmn_cnt[1]=grd_crn_nbr;
      if(lat_bnd_id != NC_MIN_INT) rcd=nco_get_vara(in_id,lat_bnd_id,dmn_srt,dmn_cnt,lat_crn,crd_typ);
      if(lon_bnd_id != NC_MIN_INT) rcd=nco_get_vara(in_id,lon_bnd_id,dmn_srt,dmn_cnt,lon_crn,crd_typ);
    } /* !flg_1D_psd_rct_bnd */
  } /* !flg_grd_1D */

  if(flg_grd_crv){
    /* Obtain fields that must be present in curvilinear input file */
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=lat_nbr;
    dmn_cnt[1]=lon_nbr;
    rcd=nco_get_vara(in_id,lat_ctr_id,dmn_srt,dmn_cnt,lat_ctr,crd_typ);
    rcd=nco_get_vara(in_id,lon_ctr_id,dmn_srt,dmn_cnt,lon_ctr,crd_typ);
    
    /* 20150923: Also input, if present in curvilinear file, corners, area, and mask
       area and mask are same size as lat and lon */
    if(area_id != NC_MIN_INT) rcd=nco_get_vara(in_id,area_id,dmn_srt,dmn_cnt,area,crd_typ);
    if(msk_id != NC_MIN_INT){
      if(msk_rnk_nbr > grd_rnk_nbr){
	/* Retrieve mask elements only from first horizontal grid, e.g., first timestep, first layer... */
	for(dmn_idx=0;dmn_idx<msk_rnk_nbr-grd_rnk_nbr;dmn_idx++){
	  dmn_srt[dmn_idx]=0L;
	  dmn_cnt[dmn_idx]=1L;
	} /* !dmn_idx */
	dmn_srt[dmn_idx]=dmn_srt[dmn_idx+1]=0L;
	dmn_cnt[dmn_idx]=lat_nbr;
	dmn_cnt[dmn_idx+1]=lon_nbr;
      } /* !msk_rnk_nbr */
      rcd=nco_get_vara(in_id,msk_id,dmn_srt,dmn_cnt,msk_unn.vp,msk_typ);
    } /* !msk_id */

    /* Corners are on curvilinear corner grid
       Rectangular boundaries (i.e., lat_bnd=[lat_nbr,2]) DNE for curvilinear grids 
       Read-in *_crn arrays in curvilinear grids, and *_bnd arrays for rectilinear grids
       Rank-ordering of corner arrays is usually lat_nbr,lon_nbr,grd_crn_nbr as produced/expected by SCRIP
       However some datasets, e.g., OMI DOMINO use grd_crn_nbr,lat_nbr,lon_nbr
       Sigh... */
    dmn_srt[0]=dmn_srt[1]=dmn_srt[2]=0L;
    if(lat_bnd_id != NC_MIN_INT && lon_bnd_id != NC_MIN_INT){
      rcd=nco_inq_vardimid(in_id,lat_bnd_id,dmn_ids);
      if((dmn_ids[0] == dmn_id_lat && dmn_ids[1] == dmn_id_lon) || (dmn_ids[0] == dmn_id_lon && dmn_ids[1] == dmn_id_lat)){
	dmn_id_bnd=dmn_ids[2];
	dmn_cnt[0]=lat_nbr;
	dmn_cnt[1]=lon_nbr;
	dmn_cnt[2]=grd_crn_nbr;
      }else if((dmn_ids[1] == dmn_id_lat && dmn_ids[2] == dmn_id_lon) || (dmn_ids[1] == dmn_id_lon && dmn_ids[2] == dmn_id_lat)){
	dmn_id_bnd=dmn_ids[0];
	dmn_cnt[0]=grd_crn_nbr;
	dmn_cnt[1]=lat_nbr;
	dmn_cnt[2]=lon_nbr;
	flg_crn_grd_lat_lon=True;
      }else{
	(void)fprintf(stdout,"%s: WARNING %s confused by dimension-ordering of latitude bounds variable \"%s\". Will ignore this bounds variable and attempt to extrapolate vertices from centers internally...\n",nco_prg_nm_get(),fnc_nm,lat_nm_in);
	lat_bnd_id=NC_MIN_INT;
	lon_bnd_id=NC_MIN_INT;
      } /* !dmn_ids */
      rcd=nco_get_vara(in_id,lat_bnd_id,dmn_srt,dmn_cnt,lat_crn,crd_typ);
      rcd=nco_get_vara(in_id,lon_bnd_id,dmn_srt,dmn_cnt,lon_crn,crd_typ);
      if(flg_crn_grd_lat_lon){
	/* Permute corner arrays from non-canonical (grd_nbr,lat_nbr,lon_nbr) to canonical (lat_nbr,lon_nbr,grd_nbr) order */
	double *lat_crn_tmp=NULL;
	double *lon_crn_tmp=NULL;
	lat_crn_tmp=(double *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
	lon_crn_tmp=(double *)nco_malloc(grd_sz_nbr*grd_crn_nbr*nco_typ_lng(crd_typ));
	memcpy(lat_crn_tmp,lat_crn,grd_sz_nbr*grd_crn_nbr*sizeof(double));
	memcpy(lon_crn_tmp,lon_crn,grd_sz_nbr*grd_crn_nbr*sizeof(double));
	for(crn_idx=0;crn_idx<grd_crn_nbr;crn_idx++){
	  for(idx=0;idx<grd_sz_nbr;idx++){
	    lat_idx=idx/lon_nbr;
	    lon_idx=idx%lon_nbr;
	    /* NB: Variables differ (lat vs. lon) but indexes are identical in next two lines */
	    lat_crn[lat_idx*lon_nbr*grd_crn_nbr+lon_idx*grd_crn_nbr+crn_idx]=lat_crn_tmp[crn_idx*grd_sz_nbr+idx];
	    lon_crn[lat_idx*lon_nbr*grd_crn_nbr+lon_idx*grd_crn_nbr+crn_idx]=lon_crn_tmp[crn_idx*grd_sz_nbr+idx];
	  } /* !idx */
	} /* !crn_idx */
	if(lat_crn_tmp) lat_crn_tmp=(double *)nco_free(lat_crn_tmp);
	if(lon_crn_tmp) lon_crn_tmp=(double *)nco_free(lon_crn_tmp);
	/* In this code branch, thought to be executed only for OMI DOMINO grids, re-compute grid center arrays (known to contain missing values) as centroids of supplied grid corners */
	for(idx=0;idx<grd_sz_nbr;idx++){
	  lat_idx=idx/lon_nbr;
	  lon_idx=idx%lon_nbr;
	  lat_ctr[idx]=0.25*(lat_crn[idx*grd_crn_nbr+0L]+lat_crn[idx*grd_crn_nbr+1L]+lat_crn[idx*grd_crn_nbr+2L]+lat_crn[idx*grd_crn_nbr+3L]);
	  lon_ctr[idx]=nco_lon_crn_avg_brnch(lon_crn[idx*grd_crn_nbr+0L],lon_crn[idx*grd_crn_nbr+1L],lon_crn[idx*grd_crn_nbr+2L],lon_crn[idx*grd_crn_nbr+3L]);
	} /* !idx */
      } /* !flg_crd_grd_lat_lon */
    } /* !lat_bnd_id */
  } /* !flg_grd_crv */

  if(flg_grd_2D){
    int lon_psn_in=1L; /* [idx] Ordinal position of longitude dimension in rectangular grid variables like area */
    int lat_psn_in=0L; /* [idx] Ordinal position of latitude  dimension in rectangular grid variables like area */
    int tpl_id=NC_MIN_INT; /* [id] ID of template field */

    /* Obtain fields that must be present in input file */
    dmn_srt[0L]=0L;
    dmn_cnt[0L]=lat_nbr;
    rcd=nco_get_vara(in_id,lat_ctr_id,dmn_srt,dmn_cnt,lat_ctr,crd_typ);
    dmn_srt[0L]=0L;
    dmn_cnt[0L]=lon_nbr;
    rcd=nco_get_vara(in_id,lon_ctr_id,dmn_srt,dmn_cnt,lon_ctr,crd_typ);
    if(lat_ctr[1L] < lat_ctr[0L]) flg_s2n=False;

    /* Use fields that may be present in input file to override, if necessary, default lon/lat order
       area and mask are both suitable templates for determining input lat/lon ordering
       NB: Algorithm assumes area is same rank as grid, and falls-back to mask if that has same rank as grid */
    if(area_id != NC_MIN_INT) tpl_id=area_id;
    else if(msk_id != NC_MIN_INT && msk_rnk_nbr == grd_rnk_nbr) tpl_id=msk_id;

    if(tpl_id != NC_MIN_INT){
      int tpl_rnk_nbr;
      var_id=tpl_id;
      /* NB: Template variable rank may exceed two with --msk_[src/dst] (e.g., SST(time,lat,lon)) */
      rcd=nco_inq_varndims(in_id,var_id,&tpl_rnk_nbr);
      rcd=nco_inq_vardimid(in_id,var_id,dmn_ids);
      /* fxm: Optimize discovery of lat/lon ordering */
      for(dmn_idx=0;dmn_idx<grd_rnk_nbr;dmn_idx++){
	rcd=nco_inq_dimname(in_id,dmn_ids[dmn_idx],dmn_nm);
	rcd+=nco_inq_dimlen(in_id,dmn_ids[dmn_idx],&dmn_sz);
	if(!strcmp(dmn_nm,lat_dmn_nm)){
	  assert(dmn_sz == lat_nbr);
	  assert(dmn_idx == 0);
	  lat_psn_in=dmn_idx;
	} /* !lat */
	if(!strcmp(dmn_nm,lon_dmn_nm)){
	  assert(dmn_sz == lon_nbr);
	  assert(dmn_idx == 1);
	  lon_psn_in=dmn_idx;
	} /* !lon */
      } /* !dmn_idx */
    } /* !tpl */

    /* Obtain fields that may be present in input file */
    if(area_id != NC_MIN_INT){
      var_id=area_id;
      rcd=nco_inq_vardimid(in_id,var_id,dmn_ids);
      dmn_srt[lat_psn_in]=0L;
      dmn_cnt[lat_psn_in]=lat_nbr;
      dmn_srt[lon_psn_in]=0L;
      dmn_cnt[lon_psn_in]=lon_nbr;
      rcd=nco_get_vara(in_id,area_id,dmn_srt,dmn_cnt,area,crd_typ);
    } /* !area */

    if(msk_id != NC_MIN_INT){
      var_id=msk_id;
      rcd=nco_inq_vardimid(in_id,var_id,dmn_ids);
      dmn_srt[lat_psn_in]=0L;
      dmn_cnt[lat_psn_in]=lat_nbr;
      dmn_srt[lon_psn_in]=0L;
      dmn_cnt[lon_psn_in]=lon_nbr;
      if(msk_rnk_nbr != grd_rnk_nbr){
	/* Retrieve mask elements only from first horizontal grid, e.g., first timestep, first layer... */
	for(dmn_idx=0;dmn_idx<msk_rnk_nbr-grd_rnk_nbr;dmn_idx++){
	  dmn_srt[dmn_idx]=0L;
	  dmn_cnt[dmn_idx]=1L;
	} /* !dmn_idx */
	dmn_srt[dmn_idx]=dmn_srt[dmn_idx+1]=0L;
	dmn_cnt[dmn_idx+lat_psn_in]=lat_nbr;
	dmn_cnt[dmn_idx+lon_psn_in]=lon_nbr;
      } /* !msk_rnk_nbr */
      rcd=nco_get_vara(in_id,msk_id,dmn_srt,dmn_cnt,msk_unn.vp,msk_typ);
    } /* !msk */

    /* Rectangular boundaries are often on "abbreviated" bounds grid (two bounds per center)
       Read-in *_crn arrays for 1D and curvilinear grids, and *_bnd arrays for rectilinear grids */
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=lat_nbr;
    dmn_cnt[1]=bnd_nbr;
    if(lat_bnd_id != NC_MIN_INT) rcd=nco_get_vara(in_id,lat_bnd_id,dmn_srt,dmn_cnt,lat_bnd,crd_typ);
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=lon_nbr;
    dmn_cnt[1]=bnd_nbr;
    if(lon_bnd_id != NC_MIN_INT) rcd=nco_get_vara(in_id,lon_bnd_id,dmn_srt,dmn_cnt,lon_bnd,crd_typ);
  } /* !flg_grd_2D */

  /* Obtain units, if any, of input area */
  if(area_id != NC_MIN_INT){
    rcd=nco_inq_att_flg(in_id,area_id,unt_sng,&att_typ,&att_sz);
    if(rcd == NC_NOERR && att_typ == NC_CHAR){
      att_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
      rcd+=nco_get_att(in_id,area_id,unt_sng,att_val,att_typ);
      /* NUL-terminate attribute before using strstr() */
      att_val[att_sz]='\0';
      if(!strcasestr(att_val,"radian")) flg_area_sr=False;
      if(att_val) area_unt=(char *)strdup(att_val);
      if(att_val) att_val=(char *)nco_free(att_val);
    } /* end rcd && att_typ */
  } /* !area_id */
  
  /* Additional information that may be required for any input grid */
  if(area_id != NC_MIN_INT) has_mss_val_area=nco_mss_val_get_dbl(in_id,area_id,&mss_val_area_dbl);
  if(msk_id != NC_MIN_INT) has_mss_val_msk=nco_mss_val_get_dbl(in_id,msk_id,&mss_val_msk_dbl);
  
  /* 20160115: AMSR coordinates are packed as NC_SHORT with scale_value=0.01f. What to do? Is it worth unpacking everything? */
  int flg_pck; /* [flg] Variable is packed on disk  */
  rcd=nco_inq_var_packing(in_id,lat_ctr_id,&flg_pck);
  if(flg_pck) (void)fprintf(stdout,"%s: WARNING %s reports lat_ctr variable \"%s\" is packed so results unpredictable. HINT: If grid-generation causes problems, retry after unpacking input file with, e.g., \"ncpdq -U in.nc out.nc\"\n",nco_prg_nm_get(),fnc_nm,lat_nm_in);
  rcd=nco_inq_var_packing(in_id,lon_ctr_id,&flg_pck);
  if(flg_pck) (void)fprintf(stdout,"%s: WARNING %s reports lon_ctr variable \"%s\" is packed so results unpredictable. HINT: If grid-generation causes problems, retry after unpacking input file with, e.g., \"ncpdq -U in.nc out.nc\"\n",nco_prg_nm_get(),fnc_nm,lon_nm_in);

  /* Close input netCDF file */
  nco_close(in_id);

  /* Remove local copy of file */
  if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_in);

  /* Above this line, fl_in and in_id refer to input file to be regridded
     Below this line, fl_out and out_id refer to grid-file to be output */
  dfl_lvl=rgr->dfl_lvl;
  fl_out=rgr->fl_grd;
  fl_out_fmt=rgr->fl_out_fmt;
  if(!fl_out){
    (void)fprintf(stdout,"%s: ERROR %s filename for inferred SCRIP grid-file is uninitialized, supply it with \"ncks --rgr grid=filename.nc\" or \"ncremap -R '--rgr grid=filename.nc'\"\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stdout,"%s: HINT ncremap supplies an automatically generated default name for any output SCRIP grid-file. Users of the standalone regridder (ncks) must explicitly specify a name for the inferred SCRIP grid-file.\n",nco_prg_nm_get());
    nco_exit(EXIT_FAILURE);
  } /* !fl_out */
  
  /* Define output variable values */
  int lon_psn; /* [idx] Ordinal position of longitude dimension in rectangular grid dimension-size array */
  int lat_psn; /* [idx] Ordinal position of latitude  dimension in rectangular grid dimension-size array */
  if(grd_rnk_nbr == dmn_nbr_1D){
    dmn_sz_int[0]=col_nbr;
  }else if(grd_rnk_nbr == dmn_nbr_2D){ /* !dmn_nbr_1D */
    /* SCRIP introduced [lon,lat] convention because more natural for Fortran 
       NB: This [lon,lat] convention applies ONLY to grid_dims variable 
       Write all other SCRIP variables as [lat,lon]
       Nonsensical? Yes, but backwards compatibility is priceless */
    lon_psn=0;
    lat_psn=1;
    dmn_sz_int[lon_psn]=lon_nbr;
    dmn_sz_int[lat_psn]=lat_nbr;
  } /* !dmn_nbr_2D */

  if(flg_grd_crv){
    /* For curvilinear grids first, if necessary, infer corner boundaries
       Then perform sanity check using same code on inferred and copied grids */
    
    if(False && has_mss_val_bnd && grd_crn_nbr == 4 && !strcmp(lat_bnd_nm,"latt_bounds") && !strcmp(lon_bnd_nm,"lont_bounds") && lat_bnd_id != NC_MIN_INT && lon_bnd_id != NC_MIN_INT){
      /* Only CESM CICE is known to fit these constraints
	 Cell center locations are (misleadingly) reported in a regular, rectangular, regional grid
	 Cell corners/boundaries are regular only in SH, curvilinear in NH, i.e., displaced or tripole grid
	 Grid is from southernmost Antarctic Ocean latitude and longitude near 79S,320E to North Pole
	 Nominal centers do not agree with true centers computed from corners
	 CICE may run in decomposed/unstructured mode, each column writes separately to output buffer?
	 This could explain missing coordinates in non-ocean gridcells
	 However, land points are completely masked (grid centers and corners are missing)
	 Oversight? Why not write coordinates for land-masked cells?
	 Regridder needs corners so we fill-in missing boundaries with derived grid
	 Gave up on inferring 20170521 once tri-pole grid complexity became apparent */
      const long idx_dbg=rgr->idx_dbg;
      double lat_ctr_drv; /* [dgr] Latitude center, derived */
      double lon_ctr_drv; /* [dgr] Longitude center, derived */
      double lat_crn_drv; /* [dgr] Latitude corner, derived */
      double lon_crn_drv; /* [dgr] Longitude corner, derived */
      long idx_ctr_sth; /* [idx] Index of southern neighbor */
      long idx_ctr_nrt; /* [idx] Index of northern neighbor */
      long idx_crn_sth; /* [idx] Index of southern neighbor */
      long idx_crn_nrt; /* [idx] Index of northern neighbor */
      long lon_idx_crr; /* [idx] Current longitude index */
      long lon_vld_frs; /* [idx] First valid longitude in latitude row */
      long *lon_vld_prv=NULL; /* [idx] Previous valid longitude in latitude row */
      long *lon_vld_nxt=NULL; /* [idx] Next valid longitude in latitude row */

      lon_vld_prv=(long *)nco_malloc(lon_nbr*sizeof(long));
      lon_vld_nxt=(long *)nco_malloc(lon_nbr*sizeof(long));

      /* First valid gridcell sets west and south bounds of entire grid */
      for(idx_ctr=0;idx_ctr<grd_sz_nbr;idx_ctr++){
	if(lat_ctr[idx_ctr] != mss_val_ctr_dbl) break;
      } /* !grd_sz_nbr */
      assert(idx_ctr != grd_sz_nbr);
      idx_crn=idx_ctr*grd_crn_nbr;
      lat_sth=lat_crn[idx_crn];
      lat_ncr=lat_crn[idx_crn+3]-lat_crn[idx_crn]; /* ul-ll */
      lon_wst=lon_crn[idx_crn];
      lon_ncr=lon_crn[idx_crn+1]-lon_crn[idx_crn]; /* lr-ll */
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s will assume grid is regional CICE in curvilinear format with masked land. Will diagnose missing cell boundaries and centers from present boundaries and centers in grid of size lat_nbr=%ld, lon_nbr=%ld.\n",nco_prg_nm_get(),fnc_nm,lat_nbr,lon_nbr);
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
	idx_ctr=lat_idx*lon_nbr;
	/* Find first valid longitude at this latitude */
	for(lon_idx=0;lon_idx<lon_nbr;lon_idx++)
	  if(lat_ctr[idx_ctr+lon_idx] != mss_val_ctr_dbl) break;
	lon_vld_frs=lon_idx;
	/* 20170519: Verified all tri-pole grid latitudes have at least one valid point */
	if(lon_vld_frs == -1L) abort();
	for(lon_idx_crr=0;lon_idx_crr<lon_nbr;lon_idx++){
	  /* Find previous and next valid longitude for all longitudes at this latitude
	     Cells can be their own previous/next valid longitude */
	  lon_vld_prv[lon_idx_crr]=-1L;
	  lon_vld_nxt[lon_idx_crr]=-1L;
	  /* Start from current longitude and move left (west)... */
	  for(lon_idx=lon_idx_crr;lon_idx>=0;lon_idx--)
	    if(lat_ctr[idx_ctr+lon_idx] != mss_val_ctr_dbl) break;
	  if(lon_idx >= 0) lon_vld_prv[lon_idx_crr]=lon_idx;
	  /* Start from current longitude and move right (east)... */
	  for(lon_idx=lon_idx_crr;lon_idx<lon_nbr;lon_idx++)
	    if(lat_ctr[idx_ctr+lon_idx] != mss_val_ctr_dbl) break;
	  if(lon_idx < lon_nbr) lon_vld_nxt[lon_idx_crr]=lon_idx;
	  /* Wrap west if previous valid cell not found */
	  lon_vld_prv[lon_idx_crr]=lon_vld_prv[lon_nbr-1L];
	  /* Wrap east if next valid cell not found */
	  lon_vld_nxt[lon_idx_crr]=lon_vld_nxt[0];
	} /* !lon_idx_crr */
	/* Derive centers and corners for each missing point */
	for(lon_idx=0;lon_idx<lon_nbr;lon_idx++){
	  idx_ctr=lat_idx*lon_nbr+lon_idx;
	  idx_crn=idx_ctr*grd_crn_nbr;
	  if(lat_ctr[idx_ctr] != mss_val_ctr_dbl){
	    lat_sth=lat_crn[idx_crn];
	    lat_ncr=lat_crn[idx_crn+3]-lat_crn[idx_crn]; /* ul-ll */
	    lat_ctr_drv=lat_sth+0.5*lat_ncr;
	    lat_crn_drv=lat_sth;
	    lon_wst=lon_crn[idx_crn];
	    lon_ncr=lon_crn[idx_crn+1]-lon_crn[idx_crn]; /* lr-ll */
	    lon_ctr_drv=lon_wst+lon_ncr*(lon_idx+0.5);
	    if(nco_dbg_lvl_get() >= nco_dbg_std && idx_ctr == idx_dbg) (void)fprintf(stdout,"%s: DEBUG %s idx=%ld lat_idx=%ld, lon_idx=%ld, lat_sth=%g, lat_ncr=%g, lon_wst=%g, lon_ncr=%g\n",nco_prg_nm_get(),fnc_nm,idx_ctr,lat_idx,lon_idx,lat_sth,lat_ncr,lon_wst,lon_ncr);
	  } /* !idx_ctr */
	  if(lat_ctr[idx_ctr] == mss_val_ctr_dbl){
	    if(lat_idx != 0L){
	      /* Not bottom row */
	      idx_ctr_sth=idx_ctr-lon_nbr;
	      if(lat_ctr[idx_ctr_sth] != mss_val_ctr_dbl){
		/* Copy southern corners from northern corners of southern neighbor */
		idx_crn_sth=idx_ctr_sth*grd_crn_nbr;
		lat_crn[idx_crn+0L]=lat_crn[idx_crn_sth+3L];
		lat_crn[idx_crn+1L]=lat_crn[idx_crn_sth+2L];
		lon_crn[idx_crn+0L]=lon_crn[idx_crn_sth+3L];
		lon_crn[idx_crn+1L]=lon_crn[idx_crn_sth+2L];
	      } /* !mss_val */
	    } /* !lat_idx */
	    if(lat_idx != lat_nbr-1L){
	      /* Not top row */
	      idx_ctr_nrt=idx_ctr+lon_nbr;
	      if(lat_ctr[idx_ctr_nrt] != mss_val_ctr_dbl){
		/* Copy northern corners from southern corners of northern neighbor */
		idx_crn_nrt=idx_ctr_nrt*grd_crn_nbr;
		lat_crn[idx_crn+2L]=lat_crn[idx_crn_nrt+1L];
		lat_crn[idx_crn+3L]=lat_crn[idx_crn_nrt+0L];
		lon_crn[idx_crn+2L]=lon_crn[idx_crn_nrt+1L];
		lon_crn[idx_crn+3L]=lon_crn[idx_crn_nrt+0L];
	      } /* !mss_val */
	    } /* !lat_idx */
	    /* Got to here before giving up
	       Idea was to interpolate missing cell corners between previous and next valid cell */
	    /* Algorithm assumes lon_wst never changes (too simple for displaced/tri_pole) */
	    lon_ctr_drv=lon_wst+lon_ncr*(lon_idx+0.5);
	    lon_crn_drv=lon_wst+lon_ncr*lon_idx;
	    if(lon_ctr_drv >= 360.0) lon_ctr_drv-=360.0;
	    lat_ctr[idx_ctr]=lat_ctr_drv;
	    lon_ctr[idx_ctr]=lon_ctr_drv;
	    lat_crn[idx_crn+0L]=lat_crn[idx_crn+1L]=lat_crn_drv;
	    lat_crn[idx_crn+2L]=lat_crn[idx_crn+3L]=lat_crn_drv+lat_ncr;
	    lon_crn[idx_crn+0L]=lon_crn[idx_crn+3L]=lon_crn_drv;
	    lon_crn[idx_crn+1L]=lon_crn[idx_crn+2L]=lon_crn_drv+lon_ncr;
	    /* Branch-cut rule */
	    if(lon_crn_drv+lon_ncr >= 360.0){
	      lon_crn[idx_crn+0L]=lon_crn[idx_crn+3L]=lon_crn_drv-360.0;
	      lon_crn[idx_crn+1L]=lon_crn[idx_crn+2L]=lon_crn_drv+lon_ncr-360.0;
	    } /* !brnch */
	  } /* !mss_val */
	} /* !lon_idx */
      } /* !lat_idx */
      if(lon_vld_nxt) lon_vld_nxt=(long *)nco_free(lon_vld_nxt);
      if(lon_vld_prv) lon_vld_prv=(long *)nco_free(lon_vld_prv);
    } /* !CICE */

    if(lat_bnd_id == NC_MIN_INT && lon_bnd_id == NC_MIN_INT){
      /* Interfaces (ntf) and boundaries (bnd) for curvilinear grids are ill-defined since sides need not follow latitudes nor meridians 
	 Simplest representation that contains equivalent information to interfaces/boundaries is grid corners array
	 Diagnose grid corners from midpoints
	 Most curvilinear data (e.g., WRF) is dimensioned lat x lon unlike SCRIP which uses lon x lat 
	 Hence we keep lat_ctr, lon_ctr, lat_crn, lon_crn with same order (likely lat x lon) as data file from which we infer grid
	 Always use input order to write skeleton file
	 Change that order, if necessary, to write SCRIP grid file
	 In the interior of a curvilinear grid, nine points contribute to the four corners of a quadrilateral surrounding each center point
	 These are the three points above the point, the three points at the same latitude, and the three points beneath the point
	 In other words, a nine-point stencil is required to define the four corners inferred around each gridcell center
	 It is cleanest to use this stencil only once for all cells in the "real"-grid, including those on the edges, not the interior
	 For this to work cleanly we define an enlarged "fake"-grid where we pre-copy the values that lead to the desired extrapolation on "real"-grid edges
	 Inspired by array-based solutions to integration of PDEs on meshes in Juri Toomre's class
	 NB: implementation is not robust to missing value points in interior of grid. Hopefully grids have no missing values in coordinate variables, although they may have missing values in non-grid fields (e.g., mask, temperature) */
      double *lat_ctr_fk; /* [dgr] Latitude  grid with extrapolated boundaries necessary for 9-point template to find four grid corners for each real center */
      double *lon_ctr_fk; /* [dgr] Longitude grid with extrapolated boundaries necessary for 9-point template to find four grid corners for each real center */
      lat_ctr_fk=(double *)nco_malloc((lat_nbr+2)*(lon_nbr+2)*sizeof(double));
      lon_ctr_fk=(double *)nco_malloc((lat_nbr+2)*(lon_nbr+2)*sizeof(double));
      long int idx_rl; /* [idx] Index into real unrolled array */
      long int idx_fk; /* [idx] Index into fake unrolled array */
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){ /* lat idx on real grid */
	for(lon_idx=0;lon_idx<lon_nbr;lon_idx++){ /* lon idx on real grid */
	  idx_rl=lat_idx*lon_nbr+lon_idx;
	  idx_fk=(lat_idx+1)*(lon_nbr+2)+lon_idx+1;
	  /* Copy real grid to interior of fake grid */
	  lat_ctr_fk[idx_fk]=lat_ctr[idx_rl];
	  lon_ctr_fk[idx_fk]=lon_ctr[idx_rl];
	} /* !lon */
      } /* !lat */
      /* Formulae to extrapolate sides and corners of fake grid are written as a starting lat/lon plus or minus adjustment
	 Adjustment is positive-definite if grid monotonically increases in latitude and longitude from LL to UR 
	 20160111: Use macros/functions to determine longitude adjustments that are always less than 180
	 This ensures all longitudes contributing to extrapolated longitude are from same branch cut */
      /* Bottom row */
      lat_idx=0; /* lat idx of extrapolated point on fake grid */
      for(lon_idx=1;lon_idx<lon_nbr+1;lon_idx++){ /* lon idx of extrapolated point on fake grid */
	idx_fk=lat_idx*(lon_nbr+2)+lon_idx; /* 1D-offset of extrapolated point on bottom row of fake grid */
	idx_rl=lat_idx*lon_nbr+lon_idx-1; /* 1D-offset of neighboring point on bottom row of real grid */
	lat_ctr_fk[idx_fk]=lat_ctr[idx_rl]-(lat_ctr[idx_rl+lon_nbr]-lat_ctr[idx_rl]);
	lon_ctr_fk[idx_fk]=lon_ctr[idx_rl]-nco_lon_dff_brnch_dgr(lon_ctr[idx_rl+lon_nbr],lon_ctr[idx_rl]);
      } /* !lon */
      /* Top row */
      lat_idx=lat_nbr+1; /* lat idx of extrapolated point on fake grid */
      for(lon_idx=1;lon_idx<lon_nbr+1;lon_idx++){ /* lon idx of extrapolated point on fake grid */
	idx_fk=lat_idx*(lon_nbr+2)+lon_idx; /* 1D-offset of extrapolated point on top row of fake grid */
	idx_rl=(lat_nbr-1)*lon_nbr+lon_idx-1; /* 1D-offset of neighboring point on top row of real grid */
	lat_ctr_fk[idx_fk]=lat_ctr[idx_rl]+(lat_ctr[idx_rl]-lat_ctr[idx_rl-lon_nbr]);
	lon_ctr_fk[idx_fk]=lon_ctr[idx_rl]+nco_lon_dff_brnch_dgr(lon_ctr[idx_rl],lon_ctr[idx_rl-lon_nbr]);
      } /* !lon */
      /* Left side */
      lon_idx=0; /* lon idx of extrapolated point on fake grid */
      for(lat_idx=1;lat_idx<lat_nbr+1;lat_idx++){ /* lat idx of extrapolated point on fake grid */
	idx_fk=lat_idx*(lon_nbr+2)+lon_idx; /* 1D-offset of extrapolated point on left side of fake grid */
	idx_rl=(lat_idx-1)*lon_nbr+lon_idx; /* 1D-offset of neighboring point on left side of real grid */
	lat_ctr_fk[idx_fk]=lat_ctr[idx_rl]-(lat_ctr[idx_rl+1]-lat_ctr[idx_rl]);
	lon_ctr_fk[idx_fk]=lon_ctr[idx_rl]-nco_lon_dff_brnch_dgr(lon_ctr[idx_rl+1],lon_ctr[idx_rl]);
      } /* !lat */
      /* Right side */
      lon_idx=lon_nbr+1; /* lon idx of extrapolated point on fake grid */
      for(lat_idx=1;lat_idx<lat_nbr+1;lat_idx++){ /* lat idx of extrapolated point on fake grid */
	idx_fk=lat_idx*(lon_nbr+2)+lon_idx; /* 1D-offset of extrapolated point on right side of fake grid */
	idx_rl=(lat_idx-1)*lon_nbr+lon_idx-2; /* 1D-offset of neighboring point on right side of real grid */
	lat_ctr_fk[idx_fk]=lat_ctr[idx_rl]+(lat_ctr[idx_rl]-lat_ctr[idx_rl-1]);
	lon_ctr_fk[idx_fk]=lon_ctr[idx_rl]+nco_lon_dff_brnch_dgr(lon_ctr[idx_rl],lon_ctr[idx_rl-1]);
      } /* !lat */
      /* LL */
      lat_ctr_fk[0]=lat_ctr_fk[lon_nbr+2]-(lat_ctr_fk[2*(lon_nbr+2)]-lat_ctr_fk[lon_nbr+2]);
      lon_ctr_fk[0]=lon_ctr_fk[1]-nco_lon_dff_brnch_dgr(lon_ctr_fk[2],lon_ctr_fk[1]);
      /* LR */
      lat_ctr_fk[lon_nbr+1]=lat_ctr_fk[2*(lon_nbr+2)-1]-(lat_ctr_fk[3*(lon_nbr+2)-1]-lat_ctr_fk[2*(lon_nbr+2)-1]);
      lon_ctr_fk[lon_nbr+1]=lon_ctr_fk[lon_nbr]+nco_lon_dff_brnch_dgr(lon_ctr_fk[lon_nbr],lon_ctr_fk[lon_nbr-1]);
      /* UR */
      lat_ctr_fk[(lat_nbr+2)*(lon_nbr+2)-1]=lat_ctr_fk[(lat_nbr+1)*(lon_nbr+2)-1]+(lat_ctr_fk[(lat_nbr+1)*(lon_nbr+2)-1]-lat_ctr_fk[lat_nbr*(lon_nbr+2)-1]);
      lon_ctr_fk[(lat_nbr+2)*(lon_nbr+2)-1]=lon_ctr_fk[(lat_nbr+1)*(lon_nbr+2)-2]+nco_lon_dff_brnch_dgr(lon_ctr_fk[(lat_nbr+1)*(lon_nbr+2)-2],lon_ctr_fk[(lat_nbr+1)*(lon_nbr+2)-3]);
      /* UL */
      lat_ctr_fk[(lat_nbr+1)*(lon_nbr+2)]=lat_ctr_fk[lat_nbr*(lon_nbr+2)]+(lat_ctr_fk[lat_nbr*(lon_nbr+2)]-lat_ctr_fk[(lat_nbr-1)*(lon_nbr+2)]);
      lon_ctr_fk[(lat_nbr+1)*(lon_nbr+2)]=lon_ctr_fk[lat_nbr*(lon_nbr+2)+1]-nco_lon_dff_brnch_dgr(lon_ctr_fk[lat_nbr*(lon_nbr+2)+2],lon_ctr_fk[lat_nbr*(lon_nbr+2)+1]);

      if(nco_dbg_lvl_get() >= nco_dbg_std){
	long idx_dbg;
	idx_dbg=rgr->idx_dbg;
	(void)fprintf(stderr,"%s: INFO %s idx_dbg = %li, Fake Center [lat,lon]=[%g,%g]\n",nco_prg_nm_get(),fnc_nm,idx_dbg,lat_ctr_fk[idx_dbg],lon_ctr_fk[idx_dbg]);
      } /* !dbg */
      
      long int lat_idx_fk; /* [idx] Index into fake (extrapolated) latitude  array */
      long int lon_idx_fk; /* [idx] Index into fake (extrapolated) longitude array */
      long int idx_fk_crn_ll_ctr_ll;
      long int idx_fk_crn_ll_ctr_lr;
      long int idx_fk_crn_ll_ctr_ur;
      long int idx_fk_crn_ll_ctr_ul;
      long int idx_fk_crn_lr_ctr_ll;
      long int idx_fk_crn_lr_ctr_lr;
      long int idx_fk_crn_lr_ctr_ur;
      long int idx_fk_crn_lr_ctr_ul;
      long int idx_fk_crn_ur_ctr_ll;
      long int idx_fk_crn_ur_ctr_lr;
      long int idx_fk_crn_ur_ctr_ur;
      long int idx_fk_crn_ur_ctr_ul;
      long int idx_fk_crn_ul_ctr_ll;
      long int idx_fk_crn_ul_ctr_lr;
      long int idx_fk_crn_ul_ctr_ur;
      long int idx_fk_crn_ul_ctr_ul;
      double *crn_lat;
      double *crn_lon;
      crn_lat=(double *)nco_malloc(grd_crn_nbr*sizeof(double));
      crn_lon=(double *)nco_malloc(grd_crn_nbr*sizeof(double));
      size_t wrn_nbr_max=20;
      size_t wrn_nbr=0;
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
	for(lon_idx=0;lon_idx<lon_nbr;lon_idx++){
	  /* 9-point template valid at all interior (non-edge) points in real grid, and at all points (including edges) in fake grid
	     Read variables idx_crn_ll_ctr_ul as "index of upper left gridcell center that contributes to lower-left gridcell corner"
	     Algorithms execute in counter-clockwise (CCW) direction: lower-left, lower-right, upper-right, upper-left
	     lat_idx and lon_idx are true indices and are used to write into grd_crn_lat/lon arrays
	     lat_idx_fk and lon_idx_fk are indices into fake arrays with extrapolated boundaries and are used to read data from fake arrays */
	  lon_idx_fk=lon_idx+1;
	  lat_idx_fk=lat_idx+1;
	  
	  idx_rl=lat_idx*lon_nbr+lon_idx;
	  idx_fk=lat_idx_fk*(lon_nbr+2)+lon_idx_fk;
	  
	  /* Determine index into fake array (valid everywhere it is applied) 
	     Comments after each equation are formula for real index (valid only at interior gridcells) */
	  idx_fk_crn_ll_ctr_ll=idx_fk-(lon_nbr+2)-1; // (lat_idx-1)*lon_nbr+lon_idx-1
	  idx_fk_crn_ll_ctr_lr=idx_fk-(lon_nbr+2); // (lat_idx-1)*lon_nbr+lon_idx
	  idx_fk_crn_ll_ctr_ur=idx_fk; // lat_idx*lon_nbr+lon_idx
	  idx_fk_crn_ll_ctr_ul=idx_fk-1; // lat_idx*lon_nbr+lon_idx-1;
	  
	  idx_fk_crn_lr_ctr_ll=idx_fk-(lon_nbr+2); // (lat_idx-1)*lon_nbr+lon_idx
	  idx_fk_crn_lr_ctr_lr=idx_fk-(lon_nbr+2)+1; // (lat_idx-1)*lon_nbr+lon_idx+1
	  idx_fk_crn_lr_ctr_ur=idx_fk+1; // lat_idx*lon_nbr+lon_idx+1
	  idx_fk_crn_lr_ctr_ul=idx_fk; // lat_idx*lon_nbr+lon_idx;
	  
	  idx_fk_crn_ur_ctr_ll=idx_fk; // lat_idx*lon_nbr+lon_idx
	  idx_fk_crn_ur_ctr_lr=idx_fk+1; // lat_idx*lon_nbr+lon_idx+1
	  idx_fk_crn_ur_ctr_ur=idx_fk+(lon_nbr+2)+1; // (lat_idx+1)*lon_nbr+lon_idx+1
	  idx_fk_crn_ur_ctr_ul=idx_fk+(lon_nbr+2); // (lat_idx+1)*lon_nbr+lon_idx;
	  
	  idx_fk_crn_ul_ctr_ll=idx_fk-1; // lat_idx*lon_nbr+lon_idx-1
	  idx_fk_crn_ul_ctr_lr=idx_fk; // lat_idx*lon_nbr+lon_idx
	  idx_fk_crn_ul_ctr_ur=idx_fk+(lon_nbr+2); // (lat_idx+1)*lon_nbr+lon_idx
	  idx_fk_crn_ul_ctr_ul=idx_fk+(lon_nbr+2)-1; // (lat_idx+1)*lon_nbr+lon_idx-1;
	  
	  /* 20160111: Algorithm requires that all longitudes in template be on same "branch cut"
	     If, say, LL longitude is 179.0 and LR longitude is -179.0 then their sum and average are zero, not 180.0 or -180.0 as desired
	     Routines labeled "*_brnch" in the following ensure that branch-cut rules are followed */
	  idx_crn_ll=grd_crn_nbr*idx_rl+0;
	  lat_crn[idx_crn_ll]=0.25*(lat_ctr_fk[idx_fk_crn_ll_ctr_ll]+lat_ctr_fk[idx_fk_crn_ll_ctr_lr]+lat_ctr_fk[idx_fk_crn_ll_ctr_ur]+lat_ctr_fk[idx_fk_crn_ll_ctr_ul]);
	  lon_crn[idx_crn_ll]=nco_lon_crn_avg_brnch(lon_ctr_fk[idx_fk_crn_ll_ctr_ll],lon_ctr_fk[idx_fk_crn_ll_ctr_lr],lon_ctr_fk[idx_fk_crn_ll_ctr_ur],lon_ctr_fk[idx_fk_crn_ll_ctr_ul]);
	  idx_crn_lr=grd_crn_nbr*idx_rl+1;
	  lat_crn[idx_crn_lr]=0.25*(lat_ctr_fk[idx_fk_crn_lr_ctr_ll]+lat_ctr_fk[idx_fk_crn_lr_ctr_lr]+lat_ctr_fk[idx_fk_crn_lr_ctr_ur]+lat_ctr_fk[idx_fk_crn_lr_ctr_ul]);
	  lon_crn[idx_crn_lr]=nco_lon_crn_avg_brnch(lon_ctr_fk[idx_fk_crn_lr_ctr_ll],lon_ctr_fk[idx_fk_crn_lr_ctr_lr],lon_ctr_fk[idx_fk_crn_lr_ctr_ur],lon_ctr_fk[idx_fk_crn_lr_ctr_ul]);
	  idx_crn_ur=grd_crn_nbr*idx_rl+2;
	  lat_crn[idx_crn_ur]=0.25*(lat_ctr_fk[idx_fk_crn_ur_ctr_ll]+lat_ctr_fk[idx_fk_crn_ur_ctr_lr]+lat_ctr_fk[idx_fk_crn_ur_ctr_ur]+lat_ctr_fk[idx_fk_crn_ur_ctr_ul]);
	  lon_crn[idx_crn_ur]=nco_lon_crn_avg_brnch(lon_ctr_fk[idx_fk_crn_ur_ctr_ll],lon_ctr_fk[idx_fk_crn_ur_ctr_lr],lon_ctr_fk[idx_fk_crn_ur_ctr_ur],lon_ctr_fk[idx_fk_crn_ur_ctr_ul]);
	  idx_crn_ul=grd_crn_nbr*idx_rl+3;
	  lat_crn[idx_crn_ul]=0.25*(lat_ctr_fk[idx_fk_crn_ul_ctr_ll]+lat_ctr_fk[idx_fk_crn_ul_ctr_lr]+lat_ctr_fk[idx_fk_crn_ul_ctr_ur]+lat_ctr_fk[idx_fk_crn_ul_ctr_ul]);
	  lon_crn[idx_crn_ul]=nco_lon_crn_avg_brnch(lon_ctr_fk[idx_fk_crn_ul_ctr_ll],lon_ctr_fk[idx_fk_crn_ul_ctr_lr],lon_ctr_fk[idx_fk_crn_ul_ctr_ur],lon_ctr_fk[idx_fk_crn_ul_ctr_ul]);

	  crn_lat[0]=lat_crn[idx_crn_ll];
	  crn_lat[1]=lat_crn[idx_crn_lr];
	  crn_lat[2]=lat_crn[idx_crn_ur];
	  crn_lat[3]=lat_crn[idx_crn_ul];
	  crn_lon[0]=lon_crn[idx_crn_ll];
	  crn_lon[1]=lon_crn[idx_crn_lr];
	  crn_lon[2]=lon_crn[idx_crn_ur];
	  crn_lon[3]=lon_crn[idx_crn_ul];
	  flg_ccw=nco_ccw_chk(crn_lat,crn_lon,grd_crn_nbr,idx_ccw,rcr_lvl);
	  if(!flg_ccw && wrn_nbr < wrn_nbr_max){
	    (void)fprintf(stdout,"%s: %s WARNING reports non-CCW gridcell at idx=%li, (lat,lon)_idx=(%li,%li), (lat,lon) = (%g, %g)\n",nco_prg_nm_get(),fnc_nm,idx_rl,lat_idx,lon_idx,lat_ctr[lat_idx],lon_ctr[lon_idx]);
	    wrn_nbr++;
	    if(wrn_nbr == wrn_nbr_max) (void)fprintf(stdout,"%s: %s INFO Number of non-CCW errors reached maximum = %li, not printing anymore\n",nco_prg_nm_get(),fnc_nm,wrn_nbr_max);
	  } /* endif */
	  lat_crn[idx_crn_ll]=crn_lat[0];
	  lat_crn[idx_crn_lr]=crn_lat[1];
	  lat_crn[idx_crn_ur]=crn_lat[2];
	  lat_crn[idx_crn_ul]=crn_lat[3];
	  lon_crn[idx_crn_ll]=crn_lon[0];
	  lon_crn[idx_crn_lr]=crn_lon[1];
	  lon_crn[idx_crn_ur]=crn_lon[2];
	  lon_crn[idx_crn_ul]=crn_lon[3];

	} /* !lon */
      } /* !lat */
      if(lat_ctr_fk) lat_ctr_fk=(double *)nco_free(lat_ctr_fk);
      if(lon_ctr_fk) lon_ctr_fk=(double *)nco_free(lon_ctr_fk);
      if(crn_lon) crn_lon=(double *)nco_free(crn_lon);
      if(crn_lat) crn_lat=(double *)nco_free(crn_lat);
    } /* !(lat_bnd_id && lon_bnd_id) */
    
  } /* !flg_grd_crv */

  if(flg_1D_psd_rct_bnd){
    double lon_brnch_min;
    double lon_brnch_max;
    double lon_dff;
    assert(grd_crn_nbr == 4);
    /* Make boundaries that were provided as pseudo-rectangular branch-cut-compliant */
    for(col_idx=0;col_idx<col_nbr;col_idx++){
      lon_brnch_min=(lon_bnd[2*col_idx] <= lon_bnd[2*col_idx+1]) ? lon_bnd[2*col_idx] : lon_bnd[2*col_idx+1];
      lon_brnch_max=(lon_bnd[2*col_idx] >= lon_bnd[2*col_idx+1]) ? lon_bnd[2*col_idx] : lon_bnd[2*col_idx+1];
      lon_dff=lon_brnch_max-lon_brnch_min;
      if(lon_dff >= 180.0){
	if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: INFO %s reports 1D pseudo-rectangular bounds branch-cut straddle at col_idx=%ld lon_brnch_max, lon_brnch_min, lon_dff = %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,col_idx,lon_brnch_max,lon_brnch_min,lon_dff);
	lon_brnch_max-=360.0;
      }else if(lon_dff <= -180.0){
	lon_brnch_max+=360.0;
      } /* !lon_dff */
      /* Extra condition to convert CW bounds to CCW bounds (necessary for OCO2) */
      if(lon_brnch_min <= lon_brnch_max){
	lon_bnd[2*col_idx]=lon_brnch_min;
	lon_bnd[2*col_idx+1]=lon_brnch_max;
      }else{
	lon_bnd[2*col_idx]=lon_brnch_max;
	lon_bnd[2*col_idx+1]=lon_brnch_min;
      } /* end else */
    } /* !col_idx */
    /* Convert boundaries that were provided as pseudo-rectangular to corners */
    for(col_idx=0;col_idx<col_nbr;col_idx++){
      idx=grd_crn_nbr*col_idx;
      /* fxm: OCO2 provides boundaries in CW not CCW orientation */
      lon_crn[idx]=lon_bnd[2*col_idx]; /* LL */
      lon_crn[idx+1]=lon_bnd[2*col_idx+1]; /* LR */
      lon_crn[idx+2]=lon_bnd[2*col_idx+1]; /* UR */
      lon_crn[idx+3]=lon_bnd[2*col_idx]; /* UL */
      lat_crn[idx]=lat_bnd[2*col_idx]; /* LL */
      lat_crn[idx+1]=lat_bnd[2*col_idx]; /* LR */
      lat_crn[idx+2]=lat_bnd[2*col_idx+1]; /* UR */
      lat_crn[idx+3]=lat_bnd[2*col_idx+1]; /* UL */
      /* fxm: OCO2 provides boundaries in CW not CCW orientation */
    } /* !col_idx */
  } /* flg_1D_psd_rct_bnd */

  if(flg_grd_crv || flg_1D_psd_rct_bnd){
    /* As of 20160308, use same sanity check for 1D pseudo-rectangular grids as for curvilinear grids
       Pseudo-rectangular grids rely on user-produced boundaries that may be psychotic (CW, non-branch-cut)
       Starting 20151205, use same sanity check for both inferred and copied curvilinear grids
       20151129: Curvilinear extrapolation technique above yields corners outside [-90.0,90.0], [-180.0,360.0]
       Also, it may assume input is ascending swath and fail for descending swaths
       Complications not fully addressed:
       Swaths may (verify this) turn from ascending to descending, or visa-versa, when satellite crosses latitude extrema
       Swaths may cross the date-line (and back!) */

    /* Determine numeric bounds of input coordinate system */
    double lon_min_min;
    double lon_max_max;
    nco_bool NCO_LON_0_TO_360=True;
    if(has_mss_val_ctr){
      for(idx=0;idx<grd_sz_nbr;idx++)
	if(lon_ctr[idx] != mss_val_ctr_dbl && lon_ctr[idx] < 0.0) break;
    }else{
      for(idx=0;idx<grd_sz_nbr;idx++)
	if(lon_ctr[idx] < 0.0) break;
    } /* !has_mss_val_ctr */
    if(idx != grd_sz_nbr) NCO_LON_0_TO_360=False;
    if(NCO_LON_0_TO_360){
      lon_min_min=0.0;
      lon_max_max=360.0;
    }else{
      lon_min_min=-180.0;
      lon_max_max=180.0;
    } /* !NCO_LON_0_TO_360 */

    /* Correct for extrapolation outside boundaries */
    for(idx=0;idx<grd_sz_nbr*grd_crn_nbr;idx++){
      idx_ctr=idx/grd_crn_nbr;
      if(has_mss_val_ctr)
	if(lat_ctr[idx_ctr] == mss_val_ctr_dbl)
	  continue;
      if(lat_crn[idx] < -90.0 || lat_crn[idx] > 90.0 || lon_crn[idx] < lon_min_min || lon_crn[idx] > lon_max_max){
	  idx_crn_ll=grd_crn_nbr*idx_ctr+0;
	  idx_crn_lr=grd_crn_nbr*idx_ctr+1;
	  idx_crn_ur=grd_crn_nbr*idx_ctr+2;
	  idx_crn_ul=grd_crn_nbr*idx_ctr+3;
	  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s reports %s corner outside canonical bounds at idx = %li, Center [lat,lon]=[%g,%g]; Corners LL [%g,%g] LR [%g,%g] UR [%g,%g] UL [%g,%g]\n",nco_prg_nm_get(),fnc_nm,(lat_bnd_id == NC_MIN_INT) ? "inferred" : "copied",idx_ctr,lat_ctr[idx_ctr],lon_ctr[idx_ctr],lat_crn[idx_crn_ll],lon_crn[idx_crn_ll],lat_crn[idx_crn_lr],lon_crn[idx_crn_lr],lat_crn[idx_crn_ur],lon_crn[idx_crn_ur],lat_crn[idx_crn_ul],lon_crn[idx_crn_ul]);
	  /* Restrict grid to real latitudes and to the 360-degree range detected from input cell-centers */
	  if(lat_crn[idx] < -90.0) lat_crn[idx]=-90.0;
	  if(lat_crn[idx] >  90.0) lat_crn[idx]=90.0;
	  if(lon_crn[idx] < lon_min_min) lon_crn[idx]+=360.0;
	  if(lon_crn[idx] > lon_max_max) lon_crn[idx]-=360.0;
      } /* !sanity */
    } /* !idx */

    /* Vertices (for valid points) are now within 360 degrees (either [0,360] or [-180,180]) implied by input coordinate system
       Curvilinear inferred grid are, by construction, branch-cut compliant
       fxm: Curvilinear and 1D pseudo-rectangular grids prescribed by (i.e., read-in from) input may not be branch-cut compliant */

    if(nco_dbg_lvl_get() >= nco_dbg_std){
      long idx_dbg;
      idx_dbg=rgr->idx_dbg;
      idx_crn_ll=grd_crn_nbr*idx_dbg+0;
      idx_crn_lr=grd_crn_nbr*idx_dbg+1;
      idx_crn_ur=grd_crn_nbr*idx_dbg+2;
      idx_crn_ul=grd_crn_nbr*idx_dbg+3;
      (void)fprintf(stderr,"%s: INFO %s idx_dbg = %li, Center [lat,lon]=[%g,%g]; Corners LL [%g,%g] LR [%g,%g] UR [%g,%g] UL [%g,%g]\n",nco_prg_nm_get(),fnc_nm,idx_dbg,lat_ctr[idx_dbg],lon_ctr[idx_dbg],lat_crn[idx_crn_ll],lon_crn[idx_crn_ll],lat_crn[idx_crn_lr],lon_crn[idx_crn_lr],lat_crn[idx_crn_ur],lon_crn[idx_crn_ur],lat_crn[idx_crn_ul],lon_crn[idx_crn_ul]);
    } /* !dbg */

  } /* !flg_grd_crv || flg_1D_psd_rct_bnd */

  if(flg_grd_crv){
    /* Copy centers into empty output array */
    for(idx=0;idx<grd_sz_nbr;idx++){
      grd_ctr_lat[idx]=lat_ctr[idx];
      grd_ctr_lon[idx]=lon_ctr[idx];
    } /* !idx */
    /* Copy inferred or copied (from input) sanity-checked corners into empty output array */
    for(idx=0;idx<grd_sz_nbr*grd_crn_nbr;idx++){
      grd_crn_lat[idx]=lat_crn[idx];
      grd_crn_lon[idx]=lon_crn[idx];
    } /* !idx */
  } /* !flg_grd_crv */

  /* 20150512 Many 2D datasets have bad bounds
     Primary example is Gaussian grids archived by CESM models that use midpoint rule rather than iterate to compute interfaces from quadrature points
     Such files have correct gw arrays and incorrect cell bounds
     flg_dgn_bnd allows nco_grd_nfr() to override faulty boundaries in file with correct bounds */
  const nco_bool flg_dgn_bnd=rgr->flg_dgn_bnd; /* [flg] Diagnose rather than copy inferred bounds */
  const long lat_nbr_hlf=lat_nbr/2L; // [nbr] Half number of latitudes (e.g., lat_nbr_hlf=32 for lat_nbr=64 and 65)
  if(flg_grd_2D){
    if(flg_dgn_bnd || (lat_bnd_id == NC_MIN_INT && lon_bnd_id == NC_MIN_INT)){
      if(nco_dbg_lvl_get() >= nco_dbg_std && flg_dgn_bnd) (void)fprintf(stdout,"%s: INFO %s will diagnose cell boundaries from cell centers...\n",nco_prg_nm_get(),fnc_nm);
      /* Derive interfaces (ntf) and bounds (bnd) from midpoints approximation applied to center data
	 NB: Simplistically derived interfaces (ntf) only valid on some rectangular grids (not on Gaussian grids)
	 These inferred-from-midpoint interfaces/bounds are overwritten in next block once lat grid is known */
      if(flg_s2n) lat_ntf[0L]=lat_ctr[0L]-0.5*(lat_ctr[1L]-lat_ctr[0L]); else lat_ntf[0L]=lat_ctr[0L]+0.5*(lat_ctr[0L]-lat_ctr[1L]);
      if(lat_ntf[0L] < -90.0) lat_ntf[0L]=-90.0; /* NB: lat_ntf[0] can be same as lat_ctr[0] for cap grid */
      if(lat_ntf[0L] > 90.0) lat_ntf[0L]=90.0;
      for(lat_idx=0L;lat_idx<lat_nbr-1L;lat_idx++)
	lat_ntf[lat_idx+1L]=0.5*(lat_ctr[lat_idx]+lat_ctr[lat_idx+1L]);
      if(flg_s2n) lat_ntf[lat_nbr]=lat_ctr[lat_nbr-1L]+0.5*(lat_ctr[lat_nbr-1L]-lat_ctr[lat_nbr-2L]); else lat_ntf[lat_nbr]=lat_ctr[lat_nbr-1L]-0.5*(lat_ctr[lat_nbr-2L]-lat_ctr[lat_nbr-1L]);
      if(lat_ntf[lat_nbr] > 90.0) lat_ntf[lat_nbr]=90.0; /* NB: lat_ntf[lat_nbr] can be same as lat_ctr[lat_nbr-1] for cap grid */
      if(lat_ntf[lat_nbr] < -90.0) lat_ntf[lat_nbr]=-90.0; /* NB: lat_ntf[lat_nbr] can be same as lat_ctr[lat_nbr-1] for cap grid */
      if(flg_s2n) lat_spn=fabs(lat_ntf[lat_nbr]-lat_ntf[0L]); /* fabs() ensures positive-definite span for N->S grids */
      
      lon_ntf[0L]=lon_ctr[0L]-0.5*(lon_ctr[1L]-lon_ctr[0L]);
      for(lon_idx=0;lon_idx<lon_nbr-1L;lon_idx++)
	lon_ntf[lon_idx+1L]=0.5*(lon_ctr[lon_idx]+lon_ctr[lon_idx+1L]);
      lon_ntf[lon_nbr]=lon_ctr[lon_nbr-1L]+0.5*(lon_ctr[lon_nbr-1L]-lon_ctr[lon_nbr-2L]);
      lon_spn=lon_ntf[lon_nbr]-lon_ntf[0L];
      
      for(idx=0;idx<lon_nbr;idx++){
	lon_bnd[2L*idx]=lon_ntf[idx];
	lon_bnd[2L*idx+1L]=lon_ntf[idx+1L];
      } /* !idx */
      for(idx=0;idx<lat_nbr;idx++){
	lat_bnd[2L*idx]=lat_ntf[idx];
	lat_bnd[2L*idx+1L]=lat_ntf[idx+1L];
      } /* !idx */
    }else{ /* !(lat_bnd_id && lon_bnd_id) */
      /* Derive interfaces (ntf) from bounds (bnd) data on disk */
      for(idx=0;idx<lon_nbr;idx++) lon_ntf[idx]=lon_bnd[2L*idx];
      lon_ntf[lon_nbr]=lon_bnd[2L*lon_nbr-1L];
      for(idx=0;idx<lat_nbr;idx++) lat_ntf[idx]=lat_bnd[2L*idx];
      lat_ntf[lat_nbr]=lat_bnd[2L*lat_nbr-1L];
      lat_spn=fabs(lat_ntf[lat_nbr]-lat_ntf[0L]); /* fabs() ensures positive-definite span for N->S grids */
      lon_spn=lon_ntf[lon_nbr]-lon_ntf[0L];
    } /* !(lat_bnd_id && lon_bnd_id) */
  } /* !flg_grd_2D */

  if(flg_grd_2D){
    /* Diagnose type of two-dimensional input grid by testing second latitude center against formulae */
    double lat_ctr_tst_eqa;
    double lat_ctr_tst_fv;
    if(flg_s2n) lat_ctr_tst_eqa=lat_ntf[0L]+lat_spn*1.5/lat_nbr; else lat_ctr_tst_eqa=lat_ntf[0L]-lat_spn*1.5/lat_nbr;
    if(flg_s2n) lat_ctr_tst_fv=lat_ntf[0L]+lat_spn/(lat_nbr-1L); else lat_ctr_tst_fv=lat_ntf[0L]-lat_spn/(lat_nbr-1L);
    double lat_ctr_tst_gss;
    /* In diagnosing grids, agreement with input to single-precision is "good enough for government work"
       Hence some comparisons cast from double to float before comparison
       20150526: T42 grid from SCRIP and related maps are only accurate to ~eight digits
       20150611: map_ne120np4_to_fv801x1600_bilin.150418.nc has yc_b[1600]=-89.775000006 not expected exact value lat_ctr[1]=-89.775000000000006
       20170521: T62 grid from NCEP-NCAR Reanalysis 1 worse than single precision, has yc_[192]=-86.6531 not expected exact value lat_ctr[1]=-86.6531671712612, relative difference is 7.86021e-07
       20191008: T62 grid from NCEP-NCAR Reanalysis 2 worse than single precision, has yc_[92]=-86.6531 not expected exact value lat_ctr[1]=-86.6531671712612, relative difference is 7.86021e-07 */
    if(nco_dbg_lvl_get() >= nco_dbg_scl && !flg_s2n) (void)fprintf(stderr,"%s: INFO %s reports that grid inferral has detected a 2D grid that runs from north-to-south, not south-to-north. Support for creating/inferring 2D N-to-S grids was added in NCO 4.7.7 (September, 2018) and should work fine.\nHINT: If present command fails, report problem to developers and then re-try inferring grid after reversing input dataset's latitude coordinate (with, e.g., ncpdq -a time,-lat,lon in.nc out.nc)\n",nco_prg_nm_get(),fnc_nm);
    if((float)lat_ctr[1L] == (float)lat_ctr_tst_eqa) lat_typ=nco_grd_lat_eqa;
    if((float)lat_ctr[1L] == (float)lat_ctr_tst_fv) lat_typ=nco_grd_lat_fv;
    double *lat_sin=NULL_CEWI; // [frc] Sine of Gaussian latitudes double precision
    double *wgt_Gss=NULL; // [frc] Gaussian weights double precision
    if(lat_typ == nco_grd_lat_nil){
      /* Check for Gaussian grid */
      lat_sin=(double *)nco_malloc(lat_nbr*sizeof(double));
      wgt_Gss=(double *)nco_malloc(lat_nbr*sizeof(double));
      (void)nco_lat_wgt_gss(lat_nbr,flg_s2n,lat_sin,wgt_Gss);
      lat_ctr_tst_gss=rdn2dgr*asin(lat_sin[1L]);
      /* Gaussian weights on output grid will be double-precision accurate
	 Grid itself is kept as user-specified so area diagnosed by ESMF_RegridWeightGen may be slightly inconsistent with weights */
      const double eps_rlt_cnv_gss=1.0e-6; // Convergence criterion (1.0e-7 fails for NCEP NCAR Reanalysis 1!)
      if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: DEBUG %s reports lat_ctr[1]=%g, lat_ctr_tst_gss=%g, fabs(1.0-fabs(lat_ctr[1]/lat_ctr_tst_gss))=%g\n",nco_prg_nm_get(),fnc_nm,lat_ctr[1],lat_ctr_tst_gss,fabs(1.0-fabs(lat_ctr[1]/lat_ctr_tst_gss)));
      if(fabs(1.0-fabs(lat_ctr[1]/lat_ctr_tst_gss)) < eps_rlt_cnv_gss) lat_typ=nco_grd_lat_gss;
    } /* !Gaussian */
    if(lat_typ == nco_grd_lat_nil){
      /* If still of unknown type, this 2D grid may be weird
	 This occurs, e.g., with POP3 destination grid
	 Change gridtype from nil (which means not-yet-set) to unknown (which means none of the others matched) */
      lat_typ=nco_grd_lat_unk;
    } /* !nil */
    
    /* Currently grd_lat_typ and grd_2D_typ are equivalent, though that may be relaxed in future */
    if(lat_typ == nco_grd_lat_unk) grd_typ=nco_grd_2D_unk;
    else if(lat_typ == nco_grd_lat_gss) grd_typ=nco_grd_2D_gss;
    else if(lat_typ == nco_grd_lat_fv) grd_typ=nco_grd_2D_fv;
    else if(lat_typ == nco_grd_lat_eqa) grd_typ=nco_grd_2D_eqa;
    else assert(False);

    /* Diagnose latitude interfaces from gridcell centers (if boundaries not provided) */
    if(flg_dgn_bnd || (lat_bnd_id == NC_MIN_INT && lon_bnd_id == NC_MIN_INT)){
      //if(flg_s2n) lat_nrt=lat_ntf[lat_nbr]; else lat_nrt=lat_ntf[0L];
      lat_spn=fabs(lat_ntf[lat_nbr]-lat_ntf[0L]);
      switch(lat_typ){
      case nco_grd_lat_fv:
	lat_ncr=lat_spn/(lat_nbr-1L);
	if(flg_s2n) lat_ntf[1L]=lat_ntf[0L]+0.5*lat_ncr; else lat_ntf[1L]=lat_ntf[0L]-0.5*lat_ncr;
	for(lat_idx=2;lat_idx<lat_nbr;lat_idx++)
	  if(flg_s2n) lat_ntf[lat_idx]=lat_ntf[1L]+(lat_idx-1L)*lat_ncr; else lat_ntf[lat_idx]=lat_ntf[1L]-(lat_idx-1L)*lat_ncr;
	break;
      case nco_grd_lat_eqa:
	lat_ncr=lat_spn/lat_nbr;
	for(lat_idx=1L;lat_idx<lat_nbr;lat_idx++)
	  if(flg_s2n) lat_ntf[lat_idx]=lat_ntf[0L]+lat_idx*lat_ncr; else lat_ntf[lat_idx]=lat_ntf[0L]-lat_idx*lat_ncr;
	break;
      case nco_grd_lat_gss:
	for(lat_idx=0L;lat_idx<lat_nbr;lat_idx++)
	  lat_ctr[lat_idx]=rdn2dgr*asin(lat_sin[lat_idx]);
	/* First guess for lat_ntf is midway between Gaussian abscissae */
	for(lat_idx=1L;lat_idx<lat_nbr;lat_idx++)
	  lat_ntf[lat_idx]=0.5*(lat_ctr[lat_idx-1L]+lat_ctr[lat_idx]);
	/* Iterate guess until area between interfaces matches Gaussian weight */
	for(lat_idx=1L;lat_idx<lat_nbr_hlf;lat_idx++){
	  double fofx_at_x0; /* [frc] Function to iterate evaluated at current guess */
	  double dfdx_at_x0; /* [frc] Derivative of equation evaluated at current guess */
	  // 20190531: Wuyin Lin reports this convergence criterion fails on ECMWF F640 grid
	  // Probably because latitude iterations assume s2n grid and ECMWF is n2s
	  // Possibly also because latitude coordinates are stored in single precision
	  // Implement precision-dependent convergence criterion, e.g., 1.0e-15 and 1.0e-7 for double- and single-precision, respectively?
	  const double eps_rlt_cnv=1.0e-15; // Convergence criterion (1.0e-16 pushes double precision to the brink)
	  itr_cnt=0;
	  lat_wgt_gss=fabs(sin(dgr2rdn*lat_ntf[lat_idx])-sin(dgr2rdn*lat_ntf[lat_idx-1L]));
	  fofx_at_x0=wgt_Gss[lat_idx-1L]-lat_wgt_gss;
	  while(fabs(fofx_at_x0) > eps_rlt_cnv){
	    /* Newton-Raphson iteration:
	       Let x=lat_ntf[lat_idx], y0=lat_ntf[lat_idx-1], gw = Gaussian weight (exact solution)
	       f(x)=sin(dgr2rdn*x)-sin(dgr2rdn*y0)-gw=0 # s2n grid
	       f(x)=sin(dgr2rdn*y0)-sin(dgr2rdn*x)-gw=0 # n2s grid
	       dfdx(x)= dgr2rdn*cos(dgr2rdn*x) # s2n grid
	       dfdx(x)=-dgr2rdn*cos(dgr2rdn*x) # n2s grid
	       x_better=x0-f(x0)/f'(x0) */
	    dfdx_at_x0=dgr2rdn*cos(dgr2rdn*lat_ntf[lat_idx]);
	    if(!flg_s2n) dfdx_at_x0=-dfdx_at_x0;
	    lat_ntf[lat_idx]+=fofx_at_x0/dfdx_at_x0; /* NB: not sure why this is minus not plus but it works :) */
	    lat_wgt_gss=fabs(sin(dgr2rdn*lat_ntf[lat_idx])-sin(dgr2rdn*lat_ntf[lat_idx-1L]));
	    fofx_at_x0=wgt_Gss[lat_idx-1L]-lat_wgt_gss;
	    if(++itr_cnt > itr_nbr_max){
	      (void)fprintf(stdout,"%s: ERROR %s reports convergence only %g after %d iterations for lat_idx = %ld\n",nco_prg_nm_get(),fnc_nm,fabs(fofx_at_x0),itr_nbr_max,lat_idx);
	      nco_exit(EXIT_FAILURE);
	    } /* endif */
	  } /* !while */
	} /* !lat_idx */
	/* Use Gaussian grid symmetry to obtain same interfaces in both hemispheres (avoids cumulative rounding errors) */
	if(lat_nbr%2){
	  /* lat_nbr is odd */
	  for(lat_idx=1L;lat_idx<=lat_nbr_hlf+1L;lat_idx++) lat_ntf[lat_nbr_hlf+lat_idx]=-lat_ntf[lat_nbr_hlf-lat_idx+1L];
	}else{
	  /* lat_nbr is even */
	  for(lat_idx=1L;lat_idx<lat_nbr_hlf;lat_idx++) lat_ntf[lat_nbr_hlf+lat_idx]=-lat_ntf[lat_nbr_hlf-lat_idx];
	} /* !flg_lat_evn */
	if(lat_sin) lat_sin=(double *)nco_free(lat_sin);
	break;
      case nco_grd_lat_unk:
	/* No generic formula exists so use interfaces already read or diagnosed as midpoints between centers */
	break;
      default:
	nco_dfl_case_generic_err(); break;
      } /* !lat_typ */

      if(lat_typ == nco_grd_lat_gss){
	/* 20170510: First approximation above to exterior interfaces for Gaussian grid are ~ +/-89 degrees
	   Loops below recompute interior interfaces only 
	   Southern- and northern-most interfaces must be explicitly assigned 
	   Inferral test for Gaussian grid _assumes_ global grid 
	   Hence WLOG can assign [-90.0, 90.0] to Gaussian grid exterior boundaries */
	if(flg_s2n) lat_ntf[0L]=-90.0; else lat_ntf[0L]=90.0;
	if(flg_s2n) lat_ntf[lat_nbr]=90.0; else lat_ntf[lat_nbr]=-90.0;
      } /* !nco_grd_lat_gss */

      /* Now that final latitude interfaces are known for all grid-types, assign to boundaries, overwriting provisional values stored there earlier */
      for(idx=0;idx<lat_nbr;idx++){
	lat_bnd[2L*idx]=lat_ntf[idx];
	lat_bnd[2L*idx+1L]=lat_ntf[idx+1L];
      } /* !idx */

    } /* !(lat_bnd_id && lon_bnd_id) */

    /* Use centers and boundaries to diagnose latitude weights */
    switch(lat_typ){
    case nco_grd_lat_eqa:
    case nco_grd_lat_fv:
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=fabs(sin(dgr2rdn*lat_ntf[lat_idx+1L])-sin(dgr2rdn*lat_ntf[lat_idx]));
      break;
    case nco_grd_lat_gss:
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=wgt_Gss[lat_idx];
      break;
    case nco_grd_lat_unk:
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=fabs(sin(dgr2rdn*lat_ntf[lat_idx+1L])-sin(dgr2rdn*lat_ntf[lat_idx]));
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING %s reports unknown input latitude grid-type. Guessing that weights for grid of rectangles is OK.\n",nco_prg_nm_get(),fnc_nm);
      break;
    default:
      nco_dfl_case_generic_err(); break;
    } /* !lat_typ */
    
    /* Diagnose type of longitude grid by testing second longitude center against formulae */
    lon_spn=lon_ntf[lon_nbr]-lon_ntf[0L];
    lat_spn=fabs(lat_ntf[lat_nbr]-lat_ntf[0L]);
    if((float)lon_spn == 360.0f && (float)lat_spn == 180.0f) nco_grd_xtn=nco_grd_xtn_glb; else nco_grd_xtn=nco_grd_xtn_rgn;
    if(lon_typ == nco_grd_lon_nil){
      if(     (float)lon_ctr[0L] ==    0.0f && (float)lon_ctr[1L] == (float)(lon_ctr[0L]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_Grn_ctr;
      else if((float)lon_ctr[0L] == -180.0f && (float)lon_ctr[1L] == (float)(lon_ctr[0L]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_180_ctr;
      else if((float)lon_ntf[0L] ==    0.0f && (float)lon_ntf[1L] == (float)(lon_ntf[0L]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_Grn_wst;
      else if((float)lon_ntf[0L] == -180.0f && (float)lon_ntf[1L] == (float)(lon_ntf[0L]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_180_wst;
      else if((float)lon_ctr[1L] == (float)(lon_ctr[0L]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_bb;
      else lon_typ=nco_grd_lon_unk;
    } /* !lon_typ */

    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s diagnosed input 2D grid-type: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_2D_sng(grd_typ));
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s diagnosed input latitude grid-type: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_lat_sng(lat_typ));
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s diagnosed input longitude grid-type: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_lon_sng(lon_typ));

  } /* !flg_grd_2D */

  if(flg_grd_2D){
    if(nco_dbg_lvl_get() >= nco_dbg_crr){
      for(idx=0;idx<lat_nbr;idx++){
	(void)fprintf(stdout,"lat[%li] = %g, vertices = ",idx,lat_ctr[idx]);
	for(bnd_idx=0;bnd_idx<bnd_nbr;bnd_idx++)
	  (void)fprintf(stdout,"%s%g%s",bnd_idx == 0 ? "[" : "",lat_bnd[bnd_nbr*idx+bnd_idx],bnd_idx == bnd_nbr-1 ? "]\n" : ", ");
      } /* end loop over lat */
      for(idx=0;idx<lon_nbr;idx++){
	(void)fprintf(stdout,"lon[%li] = %g, vertices = ",idx,lon_ctr[idx]);
	for(bnd_idx=0;bnd_idx<bnd_nbr;bnd_idx++)
	  (void)fprintf(stdout,"%s%g%s",bnd_idx == 0 ? "[" : "",lon_bnd[bnd_nbr*idx+bnd_idx],bnd_idx == bnd_nbr-1 ? "]\n" : ", ");
      } /* end loop over lon */
    } /* endif dbg */
    
    /* Fuzzy test of latitude weight normalization */
    //const double eps_rlt_max=1.0e-14; /* [frc] Round-off error tolerance: Used 1.0e-14 until 20180904 */
    const double eps_rlt_max=1.0e-12; /* [frc] Round-off error tolerance: Used 1.0e-12 since 20180904 */
    double lat_wgt_ttl_xpc; /* [frc] Expected sum of latitude weights */
    lat_wgt_ttl=0.0;
    for(idx=0;idx<lat_nbr;idx++) lat_wgt_ttl+=lat_wgt[idx];
    lat_wgt_ttl_xpc=fabs(sin(dgr2rdn*lat_bnd[2*(lat_nbr-1)+1L])-sin(dgr2rdn*lat_bnd[0L]));
    if(grd_typ != nco_grd_2D_unk && 1.0-lat_wgt_ttl/lat_wgt_ttl_xpc > eps_rlt_max){
      (void)fprintf(stdout,"%s: ERROR %s reports grid normalization does not meet precision tolerance eps_rlt_max = %20.15f\nlat_wgt_ttl = %20.15f, lat_wgt_ttl_xpc = %20.15f, lat_wgt_frc = %20.15f, eps_rlt = %20.15f\n",nco_prg_nm_get(),fnc_nm,eps_rlt_max,lat_wgt_ttl,lat_wgt_ttl_xpc,lat_wgt_ttl/lat_wgt_ttl_xpc,1.0-lat_wgt_ttl/lat_wgt_ttl_xpc);
      nco_exit(EXIT_FAILURE);
    } /* !imprecise */
  } /* !flg_grd_2D */

  if(flg_grd_2D){
    assert(grd_crn_nbr == 4);
    if(flg_dgn_bnd || (lat_bnd_id == NC_MIN_INT && lon_bnd_id == NC_MIN_INT)){
      /* If interfaces were diagnosed from centers, copy corners from interfaces */
      for(lon_idx=0;lon_idx<lon_nbr;lon_idx++){
	idx=grd_crn_nbr*lon_idx;
	lon_crn[idx]=lon_ntf[lon_idx]; /* LL */
	lon_crn[idx+1L]=lon_ntf[lon_idx+1L]; /* LR */
	lon_crn[idx+2L]=lon_ntf[lon_idx+1L]; /* UR */
	lon_crn[idx+3L]=lon_ntf[lon_idx]; /* UL */
      } /* !lon_idx */
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
	idx=grd_crn_nbr*lat_idx;
	lat_crn[idx]=lat_ntf[lat_idx]; /* LL */
	lat_crn[idx+1L]=lat_ntf[lat_idx]; /* LR */
	lat_crn[idx+2L]=lat_ntf[lat_idx+1L]; /* UR */
	lat_crn[idx+3L]=lat_ntf[lat_idx+1L]; /* UL */
      } /* !lat_idx */
    }else{ /* !lat_bnd_id */
      /* If boundaries were provided in input dataset, copy corners from boundaries */
      for(lon_idx=0;lon_idx<lon_nbr;lon_idx++){
	idx=grd_crn_nbr*lon_idx;
	lon_crn[idx]=lon_bnd[2*lon_idx]; /* LL */
	lon_crn[idx+1L]=lon_bnd[2*lon_idx+1L]; /* LR */
	lon_crn[idx+2L]=lon_bnd[2*lon_idx+1L]; /* UR */
	lon_crn[idx+3L]=lon_bnd[2*lon_idx]; /* UL */
      } /* !lon_idx */
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
	idx=grd_crn_nbr*lat_idx;
	lat_crn[idx]=lat_bnd[2*lat_idx]; /* LL */
	lat_crn[idx+1L]=lat_bnd[2*lat_idx]; /* LR */
	lat_crn[idx+2L]=lat_bnd[2*lat_idx+1L]; /* UR */
	lat_crn[idx+3L]=lat_bnd[2*lat_idx+1L]; /* UL */
      } /* !lat_idx */
    } /* !lat_bnd_id */
  } /* !flg_grd_2D */

  /* lat/lon_crn will not change anymore so stuff rectangular arrays into unrolled arrays */
  if(flg_grd_1D){
    for(idx=0;idx<grd_sz_nbr;idx++){
      grd_ctr_lat[idx]=lat_ctr[idx];
      grd_ctr_lon[idx]=lon_ctr[idx];
      if(flg_wrt_crn){
	for(crn_idx=0;crn_idx<grd_crn_nbr;crn_idx++){
	  idx2=grd_crn_nbr*idx+crn_idx;
	  grd_crn_lat[idx2]=lat_crn[idx2];
	  grd_crn_lon[idx2]=lon_crn[idx2];
	} /* !crn */
      }else{ /* !flg_wrt_crn */
	/* Defaults for ERWG when corners are unknown */
	for(crn_idx=0;crn_idx<grd_crn_nbr;crn_idx++){
	  idx2=grd_crn_nbr*idx+crn_idx;
	  grd_crn_lat[idx2]=0.0;
	  grd_crn_lon[idx2]=0.0;
	} /* !crn */
      } /* !flg_wrt_crn */
    } /* !col */
  } /* !flg_grd_1D */
  if(flg_grd_2D){
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
      for(lon_idx=0;lon_idx<lon_nbr;lon_idx++){
	idx=lat_idx*lon_nbr+lon_idx;
	grd_ctr_lat[idx]=lat_ctr[lat_idx];
	grd_ctr_lon[idx]=lon_ctr[lon_idx];
	for(crn_idx=0;crn_idx<grd_crn_nbr;crn_idx++){
	  idx2=grd_crn_nbr*idx+crn_idx;
	  lat_idx2=lat_idx*grd_crn_nbr+crn_idx;
	  lon_idx2=lon_idx*grd_crn_nbr+crn_idx;
	  grd_crn_lat[idx2]=lat_crn[lat_idx2];
	  grd_crn_lon[idx2]=lon_crn[lon_idx2];
	} /* !crn */
      } /* !lon */
    } /* !lat */

    /* 20190613: Convert CW quadrilaterals to CCW quadrilaterals so TempestRemap accepts grids
       Default construction/inferral method orders corners CCW and CW for s2n and n2s grids, respectively */
    if(!flg_s2n){
      for(idx=0L;idx<grd_sz_nbr;idx++){
	idx2=grd_crn_nbr*idx;
	flg_ccw=nco_ccw_chk(grd_crn_lat+idx2,grd_crn_lon+idx2,grd_crn_nbr,idx_ccw,rcr_lvl);
      } /* !idx */
    } /* !flg_s2n */
  } /* !flg_grd_2D */
  
  /* Find span of all grids */
  double lat_max; /* [dgr] Maximum latitude */
  double lat_min; /* [dgr] Minimum latitude */
  double lon_max; /* [dgr] Maximum longitude */
  double lon_min; /* [dgr] Minimum longitude */
  idx_ctr=0;
  if(has_mss_val_ctr){
    /* Find first non-missing value center and thus corners */
    for(idx_ctr=0;idx_ctr<grd_sz_nbr;idx_ctr++){
      if(grd_ctr_lat[idx_ctr] != mss_val_ctr_dbl) break;
    } /* !grd_sz_nbr */
    assert(idx_ctr != grd_sz_nbr);
  } /* !has_mss_val_ctr */
  if(flg_wrt_crn){
    /* Grids with corner boundaries supplied or inferred */
    lon_max=grd_crn_lon[idx_ctr*grd_crn_nbr];
    lat_max=grd_crn_lat[idx_ctr*grd_crn_nbr];
    lon_min=grd_crn_lon[idx_ctr*grd_crn_nbr];
    lat_min=grd_crn_lat[idx_ctr*grd_crn_nbr];
    for(idx=1;idx<grd_sz_nbr*grd_crn_nbr;idx++){
      idx_ctr=idx/grd_crn_nbr;
      if(has_mss_val_ctr)
	if(grd_ctr_lat[idx_ctr] == mss_val_ctr_dbl)
	  continue;
      lat_max=(grd_crn_lat[idx] > lat_max) ? grd_crn_lat[idx] : lat_max;
      lon_max=(grd_crn_lon[idx] > lon_max) ? grd_crn_lon[idx] : lon_max;
      lat_min=(grd_crn_lat[idx] < lat_min) ? grd_crn_lat[idx] : lat_min;
      lon_min=(grd_crn_lon[idx] < lon_min) ? grd_crn_lon[idx] : lon_min;
    } /* !idx */
  }else{ /* !flg_wrt_crn */
    /* 20170424: Diagnose grid-extent when corners were not provided or inferred
       This is usually (always?) for 1d unstructured grids with only centers provided */
    lon_max=grd_ctr_lon[idx_ctr];
    lat_max=grd_ctr_lat[idx_ctr];
    lon_min=grd_ctr_lon[idx_ctr];
    lat_min=grd_ctr_lat[idx_ctr];
    for(idx_ctr=1;idx_ctr<grd_sz_nbr;idx_ctr++){
      if(has_mss_val_ctr)
	if(grd_ctr_lat[idx_ctr] == mss_val_ctr_dbl)
	  continue;
      lat_max=(grd_ctr_lat[idx_ctr] > lat_max) ? grd_ctr_lat[idx_ctr] : lat_max;
      lon_max=(grd_ctr_lon[idx_ctr] > lon_max) ? grd_ctr_lon[idx_ctr] : lon_max;
      lat_min=(grd_ctr_lat[idx_ctr] < lat_min) ? grd_ctr_lat[idx_ctr] : lat_min;
      lon_min=(grd_ctr_lon[idx_ctr] < lon_min) ? grd_ctr_lon[idx_ctr] : lon_min;
    } /* !idx_ctr */
  } /* flg_wrt_crn */
  lat_spn=lat_max-lat_min;
  lon_spn=lon_max-lon_min;
  /* Use strict rules for rectangular grids, looser for spans that are inferred, or center-to-center not corner-to-corner */
  if(flg_grd_2D){
    if((float)lon_spn == 360.0f && (float)lat_spn == 180.0f) nco_grd_xtn=nco_grd_xtn_glb; else nco_grd_xtn=nco_grd_xtn_rgn;
  }else{ /* !flg_grd_2D */
    if((float)lon_spn >= 340.0f && (float)lat_spn >= 170.0f) nco_grd_xtn=nco_grd_xtn_glb; else nco_grd_xtn=nco_grd_xtn_rgn;
  } /* flg_wrt_crn */
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s reports grid resolution %li x %li, spans %g x %g degrees: [%g <= lat <= %g], [%g <= lon <= %g]\n",nco_prg_nm_get(),fnc_nm,lat_nbr,lon_nbr,lat_spn,lon_spn,lat_min,lat_max,lon_min,lon_max);
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s diagnosed input grid-extent: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_xtn_sng(nco_grd_xtn));

  /* Write ERWG hints if filenames provided and grid is regional */
  char *fl_hnt=NULL;
  char *fl_hnt_dst=NULL;
  char *fl_hnt_src=NULL;
  if(rgr->fl_hnt_dst) fl_hnt=fl_hnt_dst=rgr->fl_hnt_dst;
  if(rgr->fl_hnt_src) fl_hnt=fl_hnt_src=rgr->fl_hnt_src;
  if(nco_grd_xtn == nco_grd_xtn_rgn && fl_hnt){
    const char *fl_mode="w";
    FILE *fp_hnt; /* [fl] Hint file (for ERWG switches) file handle */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s writing ERWG weight-generation regional hint to file %s\n",nco_prg_nm_get(),fnc_nm,fl_hnt);
    /* Open output file */
    if((fp_hnt=fopen(fl_hnt,fl_mode)) == NULL){
      (void)fprintf(stderr,"%s: ERROR unable to open hint output file %s\n",nco_prg_nm_get(),fl_hnt);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: Opened hint file %s\n",nco_prg_nm_get(),fl_hnt);
    if(fl_hnt_src) (void)fprintf(fp_hnt,"--src_regional");
    if(fl_hnt_dst) (void)fprintf(fp_hnt,"--dst_regional");
    rcd=fclose(fp_hnt);
    if(rcd != 0){
      (void)fprintf(stderr,"%s: ERROR unable to close hint output file %s\n",nco_prg_nm_get(),fl_hnt);
      nco_exit(EXIT_FAILURE);
    } /* end if */
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: Closed hint file %s\n",nco_prg_nm_get(),fl_hnt);
  } /* !nco_grd_xtn */
  
  /* Diagnose area if necessary
     20170510: ALM/CLM "area" is _FillValue=1.0e36f over ocean and total gridcell area in km2 (not multiplied by landfrac) elsewhere
     Writing this ALM/CLM "area" variable to gridfile, then using with ERWG --user_areas could be disastrous (depending on mask array and interpolation type)
     On the other hand CAM "area" variable is exactly what we want for gridfile
     Input areas are considered "untrustworthy" iff they have _and use_ missing value attribute
     Re-diagnose areas considered untrustworthy so output area array does not contain missing values */
  if(flg_wrt_crn && has_mss_val_area){
    const double mss_val_dbl=mss_val_area_dbl;
    for(idx=0;idx<grd_sz_nbr;idx++)
      if(area[idx] == mss_val_dbl) break;
    if(idx < grd_sz_nbr) use_mss_val_area=True;
    if(nco_dbg_lvl_get() >= nco_dbg_fl && use_mss_val_area) (void)fprintf(stdout,"%s: INFO %s reports input area field %s is considered untrustworthy because it uses missing values, will diagnose area from cell boundaries instead...\n",nco_prg_nm_get(),fnc_nm,area_nm_in);
  } /* !has_mss_val_area */
  /* 20170511: There remain a handful of cases when input area should be diagnosed not copied
     These include using ncremap in SGS mode when inferred grids must use sensible area units
     Otherwise an inferred grid with area [km2] from ALM/CLM might be combined with area [sr] from NCO
     This would bias ERWG --user_areas produced values by ~10^10
     Setting flg_dgn_area ensures inferred area uses [sr] */
  const nco_bool flg_dgn_area=rgr->flg_dgn_area; /* [flg] Diagnose rather than copy inferred area */
  if(flg_wrt_crn && /* If bounds are available to compute area and ... */
     (area_id == NC_MIN_INT || /* Area is not in input file ... */
      use_mss_val_area || /* Area is untrustworthy */
      flg_dgn_area)){ /* User/application explicitly requests diagnostic area */
    /* Not absolutely necessary to diagnose area because ERWG will diagnose and output area itself _unless_ --user_areas option is given */
    if(nco_dbg_lvl_get() >= nco_dbg_std && flg_dgn_area) (void)fprintf(stdout,"%s: INFO %s reports diagnosing area from cell boundaries...\n",nco_prg_nm_get(),fnc_nm);
    if(flg_grd_crv || flg_grd_1D){
      /* Area of arbitrary unstructured or curvilinear grids requires spherical trigonometry */
      nco_sph_plg_area(rgr,grd_crn_lat,grd_crn_lon,grd_sz_nbr,grd_crn_nbr,area);
    }else if(flg_grd_2D){
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
	for(lon_idx=0;lon_idx<lon_nbr;lon_idx++)
	  area[lat_idx*lon_nbr+lon_idx]=fabs(dgr2rdn*(lon_bnd[2*lon_idx+1L]-lon_bnd[2*lon_idx])*(sin(dgr2rdn*lat_bnd[2*lat_idx+1L])-sin(dgr2rdn*lat_bnd[2*lat_idx]))); /* fabs() ensures positive area in n2s grids */
    } /* !flg_grd_2D */
  } /* !area_id */

  /* ERWG will fail unless grid file has mask variable
     Use nul-mask (all points included) whenever input mask variable not supplied/detected 
     Define nul-mask true everywhere and overwrite with false below
     Input mask can be any type and output mask will always be NC_INT */
  for(idx=0;idx<grd_sz_nbr;idx++) msk[idx]=1;
  if(msk_id != NC_MIN_INT){
    /* Change missing-value-masked points to 0 integer mask for SCRIP grids (SCRIP has no missing value convention)
       Input mask can be any type and output mask will always be NC_INT
       Applications: 
       ALM/CLM mask (landmask) is NC_FLOAT and defines but does not use NC_FLOAT missing value
       CICE mask (tmask/umask) is NC_FLOAT and defines and uses NC_FLOAT missing value
       AMSR mask is NC_SHORT and has no missing value
       GHRSST mask is NC_BYTE and is a multi-valued surface-type flag with missing value == -1b */
    if(msk_typ != NC_INT){
      if(nco_dbg_lvl_get() == nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s mask variable \"%s\" has odd type = %s. Re-run with higher debugging level for more information.\n",nco_prg_nm_get(),fnc_nm,msk_nm,nco_typ_sng(msk_typ));
      if(nco_dbg_lvl_get() > nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s mask variable \"%s\" has odd type = %s. Regridding weight generators require a mask variable of type NC_INT to specify points to include/exclude as sources/destinations. Points where the mask variable is zero will be excluded (ignored) in regridding, all other points will be included. When inferring gridfiles, NCO assumes the first variable with a \"mask\"-like name (\"mask\", \"Mask\", \"grid_imask\", \"landmask\", or \"tmask\"), or the variable designated by the \"--msk_[src/dst]=msk_nm\" option, is this mask. However the variable \"%s\" in this file is not type NC_INT and so may not be intended as a regridding mask, hence this pleasant informational warning. To prevent NCO from interpreting \"%s\" as a regridding mask, specify \"--msk_src=none\" and/or \"--msk_dst=none\", as appropriate. To utilize some other variable as the mask variable, specify \"--msk_src=msk_nm\" and/or \"--msk_dst=msk_nm\", as appropriate. Mask treatment is subtle, and NCO tries to \"do the right thing\". Whether it does is often easiest to discern by visual inspection of the regridded results.\n",nco_prg_nm_get(),fnc_nm,msk_nm,nco_typ_sng(msk_typ),msk_nm,msk_nm);
    } /* msk_typ */
    switch(msk_typ){
      case NC_FLOAT:
      if(has_mss_val_msk){
	const float mss_val_flt=mss_val_msk_dbl;
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.fp[idx] == mss_val_flt || msk_unn.fp[idx] == 0.0f) msk[idx]=0;
      }else{
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.fp[idx] == 0.0f) msk[idx]=0;
      } /* !mss_val */
      break;
    case NC_DOUBLE:
      if(has_mss_val_msk){
	const double mss_val_dbl=mss_val_msk_dbl;
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.dp[idx] == mss_val_dbl || msk_unn.dp[idx] == 0.0) msk[idx]=0;
      }else{
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.dp[idx] == 0.0) msk[idx]=0;
      } /* !mss_val */
      break;
    case NC_INT:
      if(has_mss_val_msk){
	const int mss_val_int=mss_val_msk_dbl;
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.ip[idx] == mss_val_int || msk_unn.ip[idx] == 0) msk[idx]=0;
      }else{
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.ip[idx] == 0) msk[idx]=0;
      } /* !mss_val */
      break;
    case NC_SHORT:
      /* http://stackoverflow.com/questions/208433/how-do-i-write-a-short-literal-in-c */
      if(has_mss_val_msk){
	const short mss_val_sht=mss_val_msk_dbl;
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.sp[idx] == mss_val_sht || msk_unn.sp[idx] == ((short)0)) msk[idx]=0;
      }else{
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.sp[idx] == ((short)0)) msk[idx]=0;
	/* 20160111: AMSR kludge fxm */
	//	for(idx=0;idx<grd_sz_nbr;idx++) if(msk[idx] == 1) msk[idx]=0;
      } /* !mss_val */
      break;
    case NC_BYTE:
      if(has_mss_val_msk){
	const nco_byte mss_val_byt=mss_val_msk_dbl;
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.bp[idx] == mss_val_byt || msk_unn.bp[idx] == ((nco_byte)0)) msk[idx]=0;
      }else{
	for(idx=0;idx<grd_sz_nbr;idx++)
	  if(msk_unn.bp[idx] == ((nco_byte)0)) msk[idx]=0;
	/* 20170811: GHRSST kludge? */
      } /* !mss_val */
      break;
    default:
      (void)fprintf(stderr,"%s: ERROR %s mask variable \"%s\" has unsupported type = %s\n",nco_prg_nm_get(),fnc_nm,msk_nm,nco_typ_sng(msk_typ));
      nco_dfl_case_generic_err();
      return NCO_ERR;
      break;
    } /* !msk_typ */
    if(msk_unn.vp) msk_unn.vp=(void *)nco_free(msk_unn.vp);
  } /* !msk_id */

  if(nco_dbg_lvl_get() >= nco_dbg_sbr){
    lat_wgt_ttl=0.0;
    area_ttl=0.0;
    if(flg_grd_2D){
      (void)fprintf(stderr,"%s: INFO %s reports destination rectangular latitude grid:\n",nco_prg_nm_get(),fnc_nm);
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
	lat_wgt_ttl+=lat_wgt[lat_idx];
    } /* !flg_grd_2D */
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
      for(lon_idx=0;lon_idx<lon_nbr;lon_idx++)
	area_ttl+=area[lat_idx*lon_nbr+lon_idx];
    (void)fprintf(stdout,"lat_wgt_ttl = %20.15f, frc_lat_wgt = %20.15f, area_ttl = %20.15f, frc_area = %20.15f\n",lat_wgt_ttl,lat_wgt_ttl/2.0,area_ttl,area_ttl/(4.0*M_PI));
    assert(area_ttl > 0.0);
    assert(area_ttl <= 4.0*M_PI);
  } /* endif dbg */

  /* Open grid file */
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

  /* Define dimensions */
  /* 20151230 ERWG appears to require presence of corner arrays in grid file even when they are not used (e.g., bilinear)
     But ERWG will break when corner values are bad. Default is do not write bad corner values. Uncomment next line to write bad corner values. */
  /* flg_wrt_crn=True; */
  if(flg_wrt_crn) rcd=nco_def_dim(out_id,grd_crn_nm,grd_crn_nbr,&dmn_id_grd_crn);
  rcd=nco_def_dim(out_id,grd_sz_nm,grd_sz_nbr,&dmn_id_grd_sz);
  rcd=nco_def_dim(out_id,grd_rnk_nm,grd_rnk_nbr,&dmn_id_grd_rnk);
  
  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */
  deflate=(int)True;
  shuffle=NC_SHUFFLE;

  /* Define variables */
  (void)nco_def_var(out_id,dmn_sz_nm,(nc_type)NC_INT,dmn_nbr_1D,&dmn_id_grd_rnk,&dmn_sz_int_id); /* NB: Too small to deflate */
  (void)nco_def_var(out_id,area_nm,crd_typ,dmn_nbr_1D,&dmn_id_grd_sz,&area_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_id,shuffle,deflate,dfl_lvl);
  (void)nco_def_var(out_id,msk_nm,(nc_type)NC_INT,dmn_nbr_1D,&dmn_id_grd_sz,&msk_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,msk_id,shuffle,deflate,dfl_lvl);
  (void)nco_def_var(out_id,grd_ctr_lat_nm,crd_typ,dmn_nbr_1D,&dmn_id_grd_sz,&grd_ctr_lat_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_ctr_lat_id,shuffle,deflate,dfl_lvl);
  (void)nco_def_var(out_id,grd_ctr_lon_nm,crd_typ,dmn_nbr_1D,&dmn_id_grd_sz,&grd_ctr_lon_id);
  if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_ctr_lon_id,shuffle,deflate,dfl_lvl);
  if(flg_wrt_crn){
    dmn_ids[0]=dmn_id_grd_sz;
    dmn_ids[1]=dmn_id_grd_crn;
    (void)nco_def_var(out_id,grd_crn_lat_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lat_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lat_id,shuffle,deflate,dfl_lvl);
    (void)nco_def_var(out_id,grd_crn_lon_nm,crd_typ,dmn_nbr_2D,dmn_ids,&grd_crn_lon_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,grd_crn_lon_id,shuffle,deflate,dfl_lvl);
  } /* !flg_wrt_crn */
  
  /* Define attributes */
  aed_sct aed_mtd;
  char *att_nm;
  
  if(strstr(rgr->grd_ttl,"None given")){
    const char att_fmt[]="NCO inferred this grid from input file %s";
    att_val=(char *)nco_malloc((strlen(att_fmt)+strlen(rgr->fl_in)+1L)*sizeof(char));
    sprintf(att_val,att_fmt,rgr->fl_in);
  }else{
    att_val=strdup(rgr->grd_ttl);
  } /* !grd_ttl */
  rcd=nco_char_att_put(out_id,NULL,"title",att_val);
  rcd=nco_char_att_put(out_id,NULL,"Conventions","SCRIP");
  const char usr_cpp[]=TKN2SNG(USER); /* [sng] Hostname from C pre-processor */
  rcd=nco_char_att_put(out_id,NULL,"created_by",usr_cpp);
  rcd=nco_char_att_put(out_id,NULL,"grid_generator","NCO");
  (void)nco_hst_att_cat(out_id,rgr->cmd_ln);
  (void)nco_vrs_att_cat(out_id);
  rcd=nco_char_att_put(out_id,NULL,"latitude_grid_type",nco_grd_lat_sng(lat_typ));
  rcd=nco_char_att_put(out_id,NULL,"longitude_grid_type",nco_grd_lon_sng(lon_typ));

  rcd=nco_char_att_put(out_id,dmn_sz_nm,"long_name","Size(s) of horizontal dimensions (in Fortran storage order for historical reasons)");

  if(flg_area_sr){
    rcd=nco_char_att_put(out_id,area_nm,"long_name","Solid Angle Subtended on Source Grid");
    rcd=nco_char_att_put(out_id,area_nm,"standard_name","solid_angle");
    rcd=nco_char_att_put(out_id,area_nm,"units","steradian");
  }else{ /* !flg_area_sr */
    rcd=nco_char_att_put(out_id,area_nm,"long_name","Area on Source Grid");
    //    rcd=nco_char_att_put(out_id,area_nm,"standard_name","solid_angle");
    rcd=nco_char_att_put(out_id,area_nm,"units",area_unt);
  } /* !flg_area_sr */

  rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"long_name","Latitude of Grid Cell Centers");
  rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"standard_name","latitude");
  if(ngl_unt){
    rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,unt_sng,ngl_unt);
  }else{
    /* 20191009: ERWG 7.1.0r- breaks on CF-compliant units strings */
    if(rgr->flg_cf_units) rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"units","degrees_north"); else rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"units","degrees");
  } /* !ngl_unt */

  rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"long_name","Longitude of Grid Cell Centers");
  rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"standard_name","longitude");
  if(ngl_unt){
    rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,unt_sng,ngl_unt);
  }else{
    /* 20191009: ERWG 7.1.0r- breaks on CF-compliant units strings */
    if(rgr->flg_cf_units) rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"units","degrees_east"); else rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"units","degrees"); 
  } /* !ngl_unt */

  if(flg_wrt_crn){
    rcd=nco_char_att_put(out_id,grd_crn_lat_nm,"long_name","Latitude of Grid Cell Vertices");
    if(ngl_unt){
      rcd=nco_char_att_put(out_id,grd_crn_lat_nm,unt_sng,ngl_unt);
    }else{
      /* 20191009: ERWG 7.1.0r- breaks on CF-compliant units strings */
      if(rgr->flg_cf_units) rcd=nco_char_att_put(out_id,grd_crn_lat_nm,"units","degrees_north"); else rcd=nco_char_att_put(out_id,grd_crn_lat_nm,"units","degrees");
    } /* !ngl_unt */
    
    rcd=nco_char_att_put(out_id,grd_crn_lon_nm,"long_name","Longitude of Grid Cell Vertices");
    if(ngl_unt){
      rcd=nco_char_att_put(out_id,grd_crn_lon_nm,unt_sng,ngl_unt);
    }else{
      /* 20191009: ERWG 7.1.0r- breaks on CF-compliant units strings */
      if(rgr->flg_cf_units) rcd=nco_char_att_put(out_id,grd_crn_lon_nm,"units","degrees_north"); else rcd=nco_char_att_put(out_id,grd_crn_lon_nm,"units","degrees");
    } /* !ngl_unt */

    rcd=nco_char_att_put(out_id,grd_ctr_lat_nm,"bounds",grd_crn_lat_nm);
    rcd=nco_char_att_put(out_id,grd_ctr_lon_nm,"bounds",grd_crn_lon_nm);
  } /* !flg_wrt_crn */
  
  rcd=nco_char_att_put(out_id,msk_nm,"long_name","Binary Integer Mask for Grid");
  rcd=nco_char_att_put(out_id,msk_nm,"units","none");

  /* Begin data mode */
  (void)nco_enddef(out_id);
  
  /* Write variables */
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_rnk_nbr;
  rcd=nco_put_vara(out_id,dmn_sz_int_id,dmn_srt,dmn_cnt,dmn_sz_int,(nc_type)NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  rcd=nco_put_vara(out_id,area_id,dmn_srt,dmn_cnt,area,crd_typ);
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  rcd=nco_put_vara(out_id,msk_id,dmn_srt,dmn_cnt,msk,(nc_type)NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  rcd=nco_put_vara(out_id,grd_ctr_lat_id,dmn_srt,dmn_cnt,grd_ctr_lat,crd_typ);
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_nbr;
  rcd=nco_put_vara(out_id,grd_ctr_lon_id,dmn_srt,dmn_cnt,grd_ctr_lon,crd_typ);
  if(flg_wrt_crn){
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=grd_sz_nbr;
    dmn_cnt[1]=grd_crn_nbr;
    rcd=nco_put_vara(out_id,grd_crn_lat_id,dmn_srt,dmn_cnt,grd_crn_lat,crd_typ);
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=grd_sz_nbr;
    dmn_cnt[1]=grd_crn_nbr;
    rcd=nco_put_vara(out_id,grd_crn_lon_id,dmn_srt,dmn_cnt,grd_crn_lon,crd_typ);
  } /* !flg_wrt_crn */
  
  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);
  
  fl_out=rgr->fl_ugrid;
  if(fl_out){
    /* Test UGRID:
       Documentation: https://github.com/ugrid-conventions/ugrid-conventions
       Procedure: Create 1x1 skeleton file, infer UGRID and SCRIP grids from it
       ncks -O -D 1 --rgr ttl='Equiangular grid 180x360' --rgr skl=${HOME}/skl_180x360.nc --rgr scrip=${HOME}/grd_180x360_SCRIP.nc --rgr latlon=180,360#lat_typ=eqa#lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
       ncks -O -D 1 --rgr infer --rgr ugrid=${HOME}/grd_ugrid.nc --rgr scrip=${HOME}/grd_scrip.nc ~/skl_180x360.nc ~/foo.nc
       ncks --cdl -v mesh_node_y ~/grd_ugrid.nc
       ncks --cdl -v mesh_face_nodes,mesh_face_x,mesh_face_y -d nFaces,0 ~/grd_ugrid.nc
       ncks --cdl -v mesh_edge_nodes,mesh_edge_x,mesh_edge_y -d nEdges,0 ~/grd_ugrid.nc
       ncks --cdl -v grid_center_lat,grid_corner_lat -d grid_size,0,,360 -d grid_corners,0,3 ~/grd_scrip.nc
       ncks --cdl -m -M ~/grd_ugrid.nc */

    char *dgx_nm=NULL_CEWI; /* [sng] Name of edge_coordinates x variable */
    char *dgy_nm=NULL_CEWI; /* [sng] Name of edge_coordinates y variable */
    char *dg_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as edges */
    char *dg_nd_nm=NULL_CEWI; /* [sng] Name of edge_node_connectivity variable */
    char *fcx_nm=NULL_CEWI; /* [sng] Name of face_coordinates x variable */
    char *fcy_nm=NULL_CEWI; /* [sng] Name of face_coordinates y variable */
    char *fc_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as faces */
    char *fc_nd_nm=NULL_CEWI; /* [sng] Name of face_node_connectivity variable */
    char *msh_nm=NULL_CEWI; /* [sng] Name of mesh topology variable */
    char *nd_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as nodes */
    char *ndx_nm=NULL_CEWI; /* [sng] Name of node_coordinates x variable */
    char *ndy_nm=NULL_CEWI; /* [sng] Name of node_coordinates y variable */
    char *npe_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as nodes-per-edge */
    char *npf_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as nodes-per-face */
    
    double *dgx=NULL_CEWI; /* [dgr] Characteristic longitude of edges */
    double *dgy=NULL_CEWI; /* [dgr] Characteristic latitude  of edges */
    double *fcx=NULL_CEWI; /* [dgr] Characteristic longitude of faces */
    double *fcy=NULL_CEWI; /* [dgr] Characteristic latitude  of faces */
    double *ndx=NULL_CEWI; /* [dgr] Longitude of nodes */
    double *ndy=NULL_CEWI; /* [dgr] Latitude  of nodes */

    int *dg_nd; /* [idx] edge_node_connectivity variable */
    int *fc_nd; /* [idx] face_node_connectivity variable */

    int dg_nd_id=NC_MIN_INT; /* [id] edge_node_connectivity variable ID */
    int dgx_id=NC_MIN_INT; /* [id] Characteristic longitude of edges variable ID */
    int dgy_id=NC_MIN_INT; /* [id] Characteristic latitude  of edges variable ID */
    int dmn_id_dg=NC_MIN_INT; /* [id] Dimension ID for edges */
    int dmn_id_fc=NC_MIN_INT; /* [id] Dimension ID for faces */
    int dmn_id_nd=NC_MIN_INT; /* [id] Dimension ID for nodes */
    int dmn_id_npe=NC_MIN_INT; /* [id] Dimension ID for nodes-per-edge */
    int dmn_id_npf=NC_MIN_INT; /* [id] Dimension ID for nodes-per-face */
    int fc_nd_id=NC_MIN_INT; /* [id] face_node_connectivity variable ID */
    int fcx_id=NC_MIN_INT; /* [id] Characteristic longitude of faces variable ID */
    int fcy_id=NC_MIN_INT; /* [id] Characteristic latitude  of faces variable ID */
    int msh_id=NC_MIN_INT; /* [id] Mesh topology variable ID */
    int msh_val=42; /* [id] Mesh topology variable value from Monty Python */
    int ndx_id=NC_MIN_INT; /* [id] Longitude of mesh nodes variable ID */
    int ndy_id=NC_MIN_INT; /* [id] Latitude  of mesh nodes variable ID */
    
    const long fc_nbr=grd_sz_nbr; /* [nbr] Number of faces in mesh */
    const long npe_nbr=2; /* [nbr] Number of nodes per edge */
    const long npf_nbr=grd_crn_nbr; /* [nbr] Number of nodes per face */

    long dg_idx; /* [idx] Counting index for edges */
    long dg_nbr=(long)NC_MIN_INT64; /* [nbr] Number of edges in mesh */
    long fc_idx; /* [idx] Counting index for faces */
    long nd_idx; /* [idx] Counting index for nodes */
    long nd_nbr=(long)NC_MIN_INT64; /* [nbr] Number of nodes in mesh */
    long srt_idx=0; /* [idx] start_index (C/Fortran) for edge_nodes, face_nodes */

    if(!dgx_nm) dgx_nm=(char *)strdup("mesh_edge_x");
    if(!dgy_nm) dgy_nm=(char *)strdup("mesh_edge_y");
    if(!dg_dmn_nm) dg_dmn_nm=(char *)strdup("nEdges");
    if(!fcx_nm) fcx_nm=(char *)strdup("mesh_face_x");
    if(!fcy_nm) fcy_nm=(char *)strdup("mesh_face_y");
    if(!fc_dmn_nm) fc_dmn_nm=(char *)strdup("nFaces");
    if(!dg_nd_nm) dg_nd_nm=(char *)strdup("mesh_edge_nodes");
    if(!fc_nd_nm) fc_nd_nm=(char *)strdup("mesh_face_nodes");
    if(!msh_nm) msh_nm=(char *)strdup("mesh");
    if(!nd_dmn_nm) nd_dmn_nm=(char *)strdup("nNodes");
    if(!ndx_nm) ndx_nm=(char *)strdup("mesh_node_x");
    if(!ndy_nm) ndy_nm=(char *)strdup("mesh_node_y");
    if(!npe_dmn_nm) npe_dmn_nm=(char *)strdup("two");
    if(!npf_dmn_nm) npf_dmn_nm=(char *)strdup("maxNodesPerFace");

    if(flg_grd_1D){
      (void)fprintf(stdout,"%s: ERROR %s UGRID output does not yet support 1D grids\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    }else if(flg_grd_2D){
      /* Assume 2D grids are global and comprised of quadrilaterals */
      switch(lat_typ){
      case nco_grd_lat_fv:
	/* Currently all 2D grids are converted to the same UGRID representation 
	   fxm: Cap grids (e.g., FV) should eventually be written with a real cap,
	   rather than as the "polar teeth" representation currently used.
	   Polar teeth convention allows cap grid to be represented as rectangular on disk
	   However, cap grids are better suited to non-rectangular UGRID meshes */
      case nco_grd_lat_eqa:
      case nco_grd_lat_gss:
	/* Numbers of unique edges and nodes counted from South Pole (SP) to North Pole (NP) */
	dg_nbr=lon_nbr*2+ /* SP: cells_per_lat*unique_edges_per_cell */
	  (lat_nbr-2)*lon_nbr*2+ /* Mid: lats*cells_per_lat*unique_edges_per_cell */
	  lon_nbr*1; /* NP: cells_per_lat*unique_edges_per_cell */
	nd_nbr=1+lon_nbr*1+ /* SP: SP+cells_per_lat*unique_nodes_per_cell */
	  (lat_nbr-2)*lon_nbr*1+ /* Mid: lats*cells_per_lat*unique_nodes_per_cell */
	  1; /* NP: NP */
	break;
      case nco_grd_lat_unk:
      case nco_grd_lat_nil:
      default:
	nco_dfl_case_generic_err(); break;
      } /* !lat_typ */
    }else if(flg_grd_crv){
      (void)fprintf(stdout,"%s: ERROR %s UGRID output does not yet support curvilinear grids\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    } /* !flg_grd */
    
    dg_nd=(int *)nco_malloc(dg_nbr*npe_nbr*nco_typ_lng(NC_INT));
    dgx=(double *)nco_malloc(dg_nbr*nco_typ_lng(crd_typ));
    dgy=(double *)nco_malloc(dg_nbr*nco_typ_lng(crd_typ));
    fc_nd=(int *)nco_malloc(fc_nbr*npf_nbr*nco_typ_lng(NC_INT));
    fcx=(double *)nco_malloc(fc_nbr*nco_typ_lng(crd_typ));
    fcy=(double *)nco_malloc(fc_nbr*nco_typ_lng(crd_typ));
    ndx=(double *)nco_malloc(nd_nbr*nco_typ_lng(crd_typ));
    ndy=(double *)nco_malloc(nd_nbr*nco_typ_lng(crd_typ));

    const long int idx_fst_crn_ll=0;
    const long int idx_fst_crn_lr=1;
    const long int idx_fst_crn_ur=2;
    const long int idx_fst_crn_ul=3;

    /* Node Ordering:
       Each interior face requires one new node
       Node 0 at SP
       New latitude row moves next node North
       Add nodes to run West->East */
    /* SP */
    ndx[0]=lon_crn[0]; /* Longitude degenerate at SP, NP, keep same longitude as corner array */
    ndy[0]=lat_crn[0];
    /* Mid */
    for(nd_idx=1;nd_idx<nd_nbr-1L;nd_idx++){
      fc_idx=nd_idx-1L;
      lat_idx=fc_idx/lon_nbr;
      lon_idx=fc_idx%lon_nbr;
      ndx[nd_idx]=lon_crn[lon_idx*grd_crn_nbr+idx_fst_crn_ul];
      ndy[nd_idx]=lat_crn[lat_idx*grd_crn_nbr+idx_fst_crn_ul];
    } /* !nd_idx */
    /* NP */
    ndx[nd_nbr-1L]=lon_crn[(lon_nbr-1)*grd_crn_nbr+idx_fst_crn_ul];
    ndy[nd_nbr-1L]=lat_crn[(lat_nbr-1)*grd_crn_nbr+idx_fst_crn_ul];

    /* Edge Ordering:
       epf_nbr is number of distinct edges-per-face (incremental, for interior cells)
       Each additional interior rectangular gridcell requires two new edges:
       Edge 0 runs South->North for all cells
       Edge 1 runs West->East for all cells
       NP row requires only one new edge per face */
    /* SP */
    const int epf_nbr=2; /* [nbr] Number of distinct edges-per-face (incremental, for interior cells) */
    for(fc_idx=0;fc_idx<lon_nbr;fc_idx++){
      dg_idx=fc_idx*epf_nbr;
      /* Edge 0 */
      dg_nd[(dg_idx+0L)*npe_nbr+0L]=srt_idx;
      dg_nd[(dg_idx+0L)*npe_nbr+1L]=srt_idx+fc_idx+1L;
      /* Edge 1 */
      dg_nd[(dg_idx+1L)*npe_nbr+0L]=srt_idx+fc_idx+1L;
      dg_nd[(dg_idx+1L)*npe_nbr+1L]=srt_idx+fc_idx+2L;
    } /* !fc_idx */
    /* Mid */
    for(fc_idx=lon_nbr;fc_idx<(lat_nbr-1L)*lon_nbr;fc_idx++){
      dg_idx=fc_idx*epf_nbr;
      /* Edge 0 */
      dg_nd[(dg_idx+0L)*npe_nbr+0L]=srt_idx+fc_idx-lon_nbr+1L;
      dg_nd[(dg_idx+0L)*npe_nbr+1L]=srt_idx+fc_idx+1L;
      /* Edge 1 */
      dg_nd[(dg_idx+1L)*npe_nbr+0L]=srt_idx+fc_idx+1L;
      dg_nd[(dg_idx+1L)*npe_nbr+1L]=srt_idx+fc_idx+2L;
    } /* !fc_idx */
    /* NP */
    for(fc_idx=fc_nbr-lon_nbr;fc_idx<fc_nbr;fc_idx++){
      /* Only one new edge per face in last row, easiest to count backwards from last edge */
      dg_idx=dg_nbr-(fc_nbr-fc_idx);
      /* NP faces require only only one new edge, Edge 0 */
      dg_nd[(dg_idx+0L)*npe_nbr+0L]=srt_idx+fc_idx-lon_nbr+1L;
      dg_nd[(dg_idx+0L)*npe_nbr+1L]=srt_idx+nd_nbr-1L;
    } /* !fc_idx */

    /* SP */
    for(fc_idx=0;fc_idx<lon_nbr;fc_idx++){
      fc_nd[fc_idx*npf_nbr+0L]=srt_idx+0L;
      fc_nd[fc_idx*npf_nbr+1L]=srt_idx+fc_idx+2L; /* NB: CCW */
      fc_nd[fc_idx*npf_nbr+2L]=srt_idx+fc_idx+1L; /* NB: CCW */
      fc_nd[fc_idx*npf_nbr+3L]=mss_val_int_out;
    } /* !fc_idx */
    /* Mid */
    for(fc_idx=lon_nbr;fc_idx<fc_nbr-lon_nbr;fc_idx++){
      fc_nd[fc_idx*npf_nbr+idx_fst_crn_ll]=srt_idx+fc_idx-lon_nbr+1L;
      fc_nd[fc_idx*npf_nbr+idx_fst_crn_lr]=srt_idx+fc_idx-lon_nbr+2L;
      fc_nd[fc_idx*npf_nbr+idx_fst_crn_ur]=srt_idx+fc_idx+2L;
      fc_nd[fc_idx*npf_nbr+idx_fst_crn_ul]=srt_idx+fc_idx+1L;
    } /* !fc_idx */
    /* NP */
    for(fc_idx=fc_nbr-lon_nbr;fc_idx<fc_nbr;fc_idx++){
      fc_nd[fc_idx*npf_nbr+0L]=srt_idx+nd_nbr-(fc_nbr-fc_idx)-2L;
      fc_nd[fc_idx*npf_nbr+1L]=srt_idx+nd_nbr-(fc_nbr-fc_idx)-1L;
      fc_nd[fc_idx*npf_nbr+2L]=srt_idx+nd_nbr-1L;
      fc_nd[fc_idx*npf_nbr+3L]=mss_val_int_out;
    } /* !fc_idx */

    /* Characteristic coordinates */
    for(dg_idx=0;dg_idx<dg_nbr-1L;dg_idx++){
      idx=dg_idx*npe_nbr;
      dgx[dg_idx]=0.5*(ndx[dg_nd[idx+0L]]+ndx[dg_nd[idx+1L]]);
      dgy[dg_idx]=0.5*(ndy[dg_nd[idx+0L]]+ndy[dg_nd[idx+1L]]);
    } /* !dg_idx */
    /* Degenerate longitude at SP, NP, causes weird characterisic longitude unless special care taken */
    for(fc_idx=0;fc_idx<fc_nbr-1L;fc_idx++){
      idx=fc_idx*npf_nbr;
      if(fc_idx < lon_nbr){
	fcx[fc_idx]=0.5*(ndx[fc_nd[idx+1L]]+ndx[fc_nd[idx+2L]]);
      }else if(fc_idx >= fc_nbr-lon_nbr-1){
	fcx[fc_idx]=0.5*(ndx[fc_nd[idx+0L]]+ndx[fc_nd[idx+1L]]);
      }else if(fc_nd[idx+3L] != mss_val_int_out){
	/* fxm for fcx use nco_lon_crn_avg_brnch() and 3-node version too */
	fcx[fc_idx]=0.25*(ndx[fc_nd[idx+0L]]+ndx[fc_nd[idx+1L]]+ndx[fc_nd[idx+2L]]+ndx[fc_nd[idx+3L]]);
      }else{
	abort();
      } /* !fc_idx */
      if(fc_nd[idx+3L] != mss_val_int_out) fcy[fc_idx]=0.25*(ndy[fc_nd[idx+0L]]+ndy[fc_nd[idx+1L]]+ndy[fc_nd[idx+2L]]+ndy[fc_nd[idx+3L]]); else fcy[fc_idx]=0.33*(ndy[fc_nd[idx+0L]]+ndy[fc_nd[idx+1L]]+ndy[fc_nd[idx+2L]]);
    } /* !fc_idx */

    fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,SHARE_CREATE,SHARE_OPEN,WRT_TMP_FL,&out_id);

    rcd=nco_def_dim(out_id,dg_dmn_nm,dg_nbr,&dmn_id_dg);
    rcd=nco_def_dim(out_id,fc_dmn_nm,fc_nbr,&dmn_id_fc);
    rcd=nco_def_dim(out_id,nd_dmn_nm,nd_nbr,&dmn_id_nd);
    rcd=nco_def_dim(out_id,npe_dmn_nm,npe_nbr,&dmn_id_npe);
    rcd=nco_def_dim(out_id,npf_dmn_nm,npf_nbr,&dmn_id_npf);

    dmn_ids[0]=dmn_id_dg;
    dmn_ids[1]=dmn_id_npe;
    rcd=nco_def_var(out_id,dg_nd_nm,(nc_type)NC_INT,dmn_nbr_2D,dmn_ids,&dg_nd_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,dg_nd_id,shuffle,deflate,dfl_lvl);
    dmn_ids[0]=dmn_id_fc;
    dmn_ids[1]=dmn_id_npf;
    rcd=nco_def_var(out_id,fc_nd_nm,(nc_type)NC_INT,dmn_nbr_2D,dmn_ids,&fc_nd_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,fc_nd_id,shuffle,deflate,dfl_lvl);
    rcd=nco_def_var(out_id,msh_nm,(nc_type)NC_INT,dmn_nbr_0D,(int *)NULL,&msh_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,msh_id,shuffle,deflate,dfl_lvl);
    rcd=nco_def_var(out_id,ndx_nm,crd_typ,dmn_nbr_1D,&dmn_id_nd,&ndx_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,ndx_id,shuffle,deflate,dfl_lvl);
    rcd=nco_def_var(out_id,ndy_nm,crd_typ,dmn_nbr_1D,&dmn_id_nd,&ndy_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,ndy_id,shuffle,deflate,dfl_lvl);
    rcd=nco_def_var(out_id,dgx_nm,crd_typ,dmn_nbr_1D,&dmn_id_dg,&dgx_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,dgx_id,shuffle,deflate,dfl_lvl);
    rcd=nco_def_var(out_id,dgy_nm,crd_typ,dmn_nbr_1D,&dmn_id_dg,&dgy_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,dgy_id,shuffle,deflate,dfl_lvl);
    rcd=nco_def_var(out_id,fcx_nm,crd_typ,dmn_nbr_1D,&dmn_id_fc,&fcx_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,fcx_id,shuffle,deflate,dfl_lvl);
    rcd=nco_def_var(out_id,fcy_nm,crd_typ,dmn_nbr_1D,&dmn_id_fc,&fcy_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,fcy_id,shuffle,deflate,dfl_lvl);

    if(strstr(rgr->grd_ttl,"None given")){
      const char att_fmt[]="NCO constructed this UGRID grid from scratch";
      att_val=(char *)nco_malloc((strlen(att_fmt)+strlen(rgr->fl_in)+1L)*sizeof(char));
      sprintf(att_val,att_fmt);
    }else{
      att_val=strdup(rgr->grd_ttl);
    } /* !grd_ttl */
    rcd=nco_char_att_put(out_id,NULL,"title",att_val);
    rcd=nco_char_att_put(out_id,NULL,"Conventions","CF-1.6, UGRID-1.0");
    rcd=nco_char_att_put(out_id,NULL,"created_by",usr_cpp);
    rcd=nco_char_att_put(out_id,NULL,"grid_generator","NCO");
    (void)nco_hst_att_cat(out_id,rgr->cmd_ln);
    (void)nco_vrs_att_cat(out_id);

    rcd=nco_char_att_put(out_id,msh_nm,"cf_role","mesh_topology");
    rcd=nco_char_att_put(out_id,msh_nm,"standard_name","mesh_topology");
    rcd=nco_char_att_put(out_id,msh_nm,"long_name","Topology data");
    att_nm=strdup("topology_dimension");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.sz=1;
    aed_mtd.type=NC_INT;
    aed_mtd.val.ip=&val_two;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);

    aed_mtd.sz=strlen(ndx_nm)+strlen(ndy_nm)+1L;
    att_val=(char *)nco_malloc((aed_mtd.sz+1L)*nco_typ_lng(NC_CHAR));
    (void)sprintf(att_val,"%s %s",ndx_nm,ndy_nm);
    rcd=nco_char_att_put(out_id,msh_nm,"node_coordinates",att_val);

    rcd=nco_char_att_put(out_id,msh_nm,"face_node_connectivity",fc_nd_nm);

    aed_mtd.sz=strlen(fcx_nm)+strlen(fcy_nm)+1L;
    att_val=(char *)nco_malloc((aed_mtd.sz+1L)*nco_typ_lng(NC_CHAR));
    (void)sprintf(att_val,"%s %s",fcx_nm,fcy_nm);
    rcd=nco_char_att_put(out_id,msh_nm,"face_coordinates",att_val);

    rcd=nco_char_att_put(out_id,msh_nm,"face_dimension",fc_dmn_nm);
    rcd=nco_char_att_put(out_id,msh_nm,"edge_node_connectivity",dg_nd_nm);

    aed_mtd.sz=strlen(dgx_nm)+strlen(dgy_nm)+1L;
    att_val=(char *)nco_malloc((aed_mtd.sz+1L)*nco_typ_lng(NC_CHAR));
    (void)sprintf(att_val,"%s %s",dgx_nm,dgy_nm);
    rcd=nco_char_att_put(out_id,msh_nm,"edge_coordinates",att_val);

    rcd=nco_char_att_put(out_id,msh_nm,"edge_dimension",dg_dmn_nm);

    rcd=nco_char_att_put(out_id,ndx_nm,"standard_name","longitude");
    rcd=nco_char_att_put(out_id,ndx_nm,"long_name","Longitude of mesh nodes");
    rcd=nco_char_att_put(out_id,ndx_nm,"units","degrees_east");

    rcd=nco_char_att_put(out_id,ndy_nm,"standard_name","latitude");
    rcd=nco_char_att_put(out_id,ndy_nm,"long_name","Latitude of mesh nodes");
    rcd=nco_char_att_put(out_id,ndy_nm,"units","degrees_north");
    
    rcd=nco_char_att_put(out_id,dg_nd_nm,"cf_role","edge_node_connectivity");
    rcd=nco_char_att_put(out_id,dg_nd_nm,"long_name","Maps every edge to the two nodes that it connects");
    att_nm=strdup("start_index");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dg_nd_nm;
    aed_mtd.id=dg_nd_id;
    aed_mtd.sz=1;
    aed_mtd.type=NC_INT;
    aed_mtd.val.ip=&val_zero;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dg_nd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);

    rcd=nco_char_att_put(out_id,fc_nd_nm,"cf_role","face_node_connectivity");
    rcd=nco_char_att_put(out_id,fc_nd_nm,"long_name","Maps every face to its corner nodes");
    att_nm=strdup("start_index");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fc_nd_nm;
    aed_mtd.id=fc_nd_id;
    aed_mtd.sz=1;
    aed_mtd.type=NC_INT;
    aed_mtd.val.ip=&val_zero;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fc_nd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    att_nm=strdup("_FillValue");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fc_nd_nm;
    aed_mtd.id=fc_nd_id;
    aed_mtd.sz=1;
    aed_mtd.type=NC_INT;
    aed_mtd.val.ip=&mss_val_int_out;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fc_nd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);

    rcd=nco_char_att_put(out_id,dgx_nm,"standard_name","longitude");
    rcd=nco_char_att_put(out_id,dgx_nm,"long_name","Characteristic longitude of 2D mesh face");
    rcd=nco_char_att_put(out_id,dgx_nm,"units","degrees_east");

    rcd=nco_char_att_put(out_id,dgy_nm,"standard_name","latitude");
    rcd=nco_char_att_put(out_id,dgy_nm,"long_name","Characteristic latitude of 2D mesh face");
    rcd=nco_char_att_put(out_id,dgy_nm,"units","degrees_north");

    rcd=nco_char_att_put(out_id,fcx_nm,"standard_name","longitude");
    rcd=nco_char_att_put(out_id,fcx_nm,"long_name","Characteristic longitude of 2D mesh edge");
    rcd=nco_char_att_put(out_id,fcx_nm,"units","degrees_east");

    rcd=nco_char_att_put(out_id,fcy_nm,"standard_name","latitude");
    rcd=nco_char_att_put(out_id,fcy_nm,"long_name","Characteristic latitude of 2D mesh edge");
    rcd=nco_char_att_put(out_id,fcy_nm,"units","degrees_north");

    /* Begin data mode */
    (void)nco_enddef(out_id);

    (void)nco_put_vara(out_id,msh_id,dmn_srt,dmn_cnt,&msh_val,(nc_type)NC_INT);
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=dg_nbr;
    dmn_cnt[1]=epf_nbr;
    (void)nco_put_vara(out_id,dg_nd_id,dmn_srt,dmn_cnt,dg_nd,(nc_type)NC_INT);
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=fc_nbr;
    dmn_cnt[1]=npf_nbr;
    (void)nco_put_vara(out_id,fc_nd_id,dmn_srt,dmn_cnt,fc_nd,(nc_type)NC_INT);
    dmn_srt[0]=0L;
    dmn_cnt[0]=nd_nbr;
    (void)nco_put_vara(out_id,ndx_id,dmn_srt,dmn_cnt,ndx,crd_typ);
    dmn_srt[0]=0L;
    dmn_cnt[0]=nd_nbr;
    (void)nco_put_vara(out_id,ndy_id,dmn_srt,dmn_cnt,ndy,crd_typ);
    dmn_srt[0]=0L;
    dmn_cnt[0]=dg_nbr;
    (void)nco_put_vara(out_id,dgx_id,dmn_srt,dmn_cnt,dgx,crd_typ);
    (void)nco_put_vara(out_id,dgy_id,dmn_srt,dmn_cnt,dgy,crd_typ);
    dmn_srt[0]=0L;
    dmn_cnt[0]=fc_nbr;
    (void)nco_put_vara(out_id,fcx_id,dmn_srt,dmn_cnt,fcx,crd_typ);
    (void)nco_put_vara(out_id,fcy_id,dmn_srt,dmn_cnt,fcy,crd_typ);

    /* Close output file and move it from temporary to permanent location */
    (void)nco_fl_out_cls(fl_out,fl_out_tmp,out_id);

    /* Free memory associated with output file */
    if(dgx) dgx=(double *)nco_free(dgx);
    if(dgy) dgy=(double *)nco_free(dgy);
    if(dg_nd) dg_nd=(int *)nco_free(dg_nd);
    if(fcx) fcx=(double *)nco_free(fcx);
    if(fcy) fcy=(double *)nco_free(fcy);
    if(fc_nd) fc_nd=(int *)nco_free(fc_nd);
    if(ndx) ndx=(double *)nco_free(ndx);
    if(ndy) ndy=(double *)nco_free(ndy);

    /* Free strings */
    if(dgx_nm) dgx_nm=(char *)nco_free(dgx_nm);
    if(dgy_nm) dgy_nm=(char *)nco_free(dgy_nm);
    if(dg_dmn_nm) dg_dmn_nm=(char *)nco_free(dg_dmn_nm);
    if(dg_nd_nm) dg_nd_nm=(char *)nco_free(dg_nd_nm);
    if(fcx_nm) fcx_nm=(char *)nco_free(fcx_nm);
    if(fcy_nm) fcy_nm=(char *)nco_free(fcy_nm);
    if(fc_dmn_nm) fc_dmn_nm=(char *)nco_free(fc_dmn_nm);
    if(fc_nd_nm) fc_nd_nm=(char *)nco_free(fc_nd_nm);
    if(msh_nm) msh_nm=(char *)nco_free(msh_nm);
    if(nd_dmn_nm) nd_dmn_nm=(char *)nco_free(nd_dmn_nm);
    if(ndx_nm) ndx_nm=(char *)nco_free(ndx_nm);
    if(ndy_nm) ndy_nm=(char *)nco_free(ndy_nm);
    if(npe_dmn_nm) npe_dmn_nm=(char *)nco_free(npe_dmn_nm);
    if(npf_dmn_nm) npf_dmn_nm=(char *)nco_free(npf_dmn_nm);
  } /* !fl_ugrid */
  
  /* Free memory associated with input file */
  if(dmn_sz_int) dmn_sz_int=(int *)nco_free(dmn_sz_int);
  if(msk) msk=(int *)nco_free(msk);
  if(area) area=(double *)nco_free(area);
  if(grd_ctr_lat) grd_ctr_lat=(double *)nco_free(grd_ctr_lat);
  if(grd_ctr_lon) grd_ctr_lon=(double *)nco_free(grd_ctr_lon);
  if(grd_crn_lat) grd_crn_lat=(double *)nco_free(grd_crn_lat);
  if(grd_crn_lon) grd_crn_lon=(double *)nco_free(grd_crn_lon);
  if(lat_bnd) lat_bnd=(double *)nco_free(lat_bnd);
  if(lat_crn) lat_crn=(double *)nco_free(lat_crn);
  if(lat_ctr) lat_ctr=(double *)nco_free(lat_ctr);
  if(lat_ntf) lat_ntf=(double *)nco_free(lat_ntf);
  if(lat_wgt) lat_wgt=(double *)nco_free(lat_wgt);
  if(lon_bnd) lon_bnd=(double *)nco_free(lon_bnd);
  if(lon_crn) lon_crn=(double *)nco_free(lon_crn);
  if(lon_ctr) lon_ctr=(double *)nco_free(lon_ctr);
  if(lon_ntf) lon_ntf=(double *)nco_free(lon_ntf);
  if(vrt_cll) vrt_cll=(int *)nco_free(vrt_cll);
  if(vrt_lat) vrt_lat=(double *)nco_free(vrt_lat);
  if(vrt_lon) vrt_lon=(double *)nco_free(vrt_lon);

  /* Free strings */
  if(area_nm_in) area_nm_in=(char *)nco_free(area_nm_in);
  if(area_unt) area_unt=(char *)nco_free(area_unt);
  if(bnd_dmn_nm) bnd_dmn_nm=(char *)nco_free(bnd_dmn_nm);
  if(col_dmn_nm) col_dmn_nm=(char *)nco_free(col_dmn_nm);
  if(lat_bnd_nm) lat_bnd_nm=(char *)nco_free(lat_bnd_nm);
  if(lat_dmn_nm) lat_dmn_nm=(char *)nco_free(lat_dmn_nm);
  if(lat_nm_in) lat_nm_in=(char *)nco_free(lat_nm_in);
  if(lon_bnd_nm) lon_bnd_nm=(char *)nco_free(lon_bnd_nm);
  if(lon_dmn_nm) lon_dmn_nm=(char *)nco_free(lon_dmn_nm);
  if(lon_nm_in) lon_nm_in=(char *)nco_free(lon_nm_in);
  if(msk_nm_in) msk_nm_in=(char *)nco_free(msk_nm_in);
  if(ngl_unt) ngl_unt=(char *)nco_free(ngl_unt);
  if(vrt_cll_nm) vrt_cll_nm=(char *)nco_free(vrt_cll_nm);
  if(vrt_lat_nm) vrt_lat_nm=(char *)nco_free(vrt_lat_nm);
  if(vrt_lon_nm) vrt_lon_nm=(char *)nco_free(vrt_lon_nm);
  
  return rcd;

} /* !nco_grd_nfr() */

double /* O [dgr] Longitude difference (lon_r-lon_l) */
nco_lon_dff_brnch_dgr /* [fnc] Subtract longitudes with branch-cut rules */
(double lon_r, /* I [dgr] Longitude on right of gridcell (subtractor) */
 double lon_l) /* I [dgr] Longitude on  left of gridcell (subtractee) */
{
  /* Purpose: Return difference of two longitudes in degrees
     Assume longitudes are within 180 degrees of eachother
     Default orientation is monotonically increasing longitude from left to right */
  const char fnc_nm[]="nco_lon_dff_brnch_dgr()";
  const double lon_dff=lon_r-lon_l; /* [dgr] Longitude difference (lon_r-lon_l) */
  if(lon_dff >= 180.0){
    (void)fprintf(stdout,"%s: WARNING %s reports lon_r, lon_l, lon_dff = %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,lon_r,lon_l,lon_dff);
    return lon_dff-360.0;
  }else if(lon_dff <= -180.0){
    return lon_dff+360.0;
  } /* !lon_dff */

  return lon_dff;
} /* !nco_lon_dff_brnch_dgr() */

double /* O [rdn] Longitude difference (lon_r-lon_l) */
nco_lon_dff_brnch_rdn /* [fnc] Subtract longitudes with branch-cut rules */
(double lon_r, /* I [rdn] Longitude on right of gridcell (subtractor) */
 double lon_l) /* I [rdn] Longitude on  left of gridcell (subtractee) */
{
  /* Purpose: Return difference of two longitudes in radians
     Assume longitudes are within pi radians of eachother
     Default orientation is monotonically increasing longitude from left to right */
  const char fnc_nm[]="nco_lon_dff_brnch_rdn()";
  const double lon_dff=lon_r-lon_l; /* [rdn] Longitude difference (lon_r-lon_l) */
  //nco_bool dbg_prn=False; /* [flg] Print warning when longitude difference is suspicious */
  /* longitudes on different branch cuts are expected when computing polygon area, so warn only if requested with high debugging level */
  if(lon_dff >= M_PI){
    if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: WARNING %s reports lon_r, lon_l, lon_dff = %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,lon_r,lon_l,lon_dff);
    return lon_dff-M_PI-M_PI;
  }else if(lon_dff <= -M_PI){
    if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: WARNING %s reports lon_r, lon_l, lon_dff = %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,lon_r,lon_l,lon_dff);
    return lon_dff+M_PI+M_PI;
  } /* !lon_dff */

  return lon_dff;
} /* !nco_lon_dff_brnch_rdn() */

double /* O [dgr] Longitude average */
nco_lon_crn_avg_brnch /* [fnc] Average quadrilateral longitude with branch-cut rules */
(double lon_ll, /* I [dgr] Longitude at lower left  of gridcell */
 double lon_lr, /* I [dgr] Longitude at lower right of gridcell */
 double lon_ur, /* I [dgr] Longitude at upper right of gridcell */
 double lon_ul) /* I [dgr] Longitude at upper left  of gridcell */
{
  /* Purpose: Return average of four corner longitudes of quadrilateral
     Assume longitudes are within 180 degrees of eachother
     Default orientation is monotonically increasing longitude from left to right 
     WLOG, adjust all longitudes to be on same branch as lon_ll */
  const char fnc_nm[]="nco_lon_crn_avg_brnch()";
  double lon_dff; /* [dgr] Longitude difference */

  lon_dff=lon_lr-lon_ll;
  if(lon_dff >= 180.0){
    if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: INFO %s reports lon_lr, lon_ll, lon_dff = %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,lon_lr,lon_ll,lon_dff);
    lon_lr-=360.0;
  }else if(lon_dff <= -180.0){
    lon_lr+=360.0;
  } /* !lon_dff */

  lon_dff=lon_ur-lon_ll;
  if(lon_dff >= 180.0){
    if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: INFO %s reports lon_ur, lon_ll, lon_dff = %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,lon_ur,lon_ll,lon_dff);
    lon_ur-=360.0;
  }else if(lon_dff <= -180.0){
    lon_ur+=360.0;
  } /* !lon_dff */

  lon_dff=lon_ul-lon_ll;
  if(lon_dff >= 180.0){
    if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: INFO %s reports lon_ul, lon_ll, lon_dff = %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,lon_ul,lon_ll,lon_dff);
    lon_ul-=360.0;
  }else if(lon_dff <= -180.0){
    lon_ul+=360.0;
  } /* !lon_dff */
  
  return 0.25*(lon_ll+lon_lr+lon_ur+lon_ul);
} /* !nco_lon_crn_avg_brnch() */

double /* O [dgr] Longitude average */
nco_lon_ply_avg_brnch_dgr /* [fnc] Average polygon longitude with branch-cut rules */
(double *lon_crn, /* I [dgr] Longitude of gridcell corners */
 long lon_nbr) /* I [nbr] Number of vertices in polygon */
{
  /* Purpose: Return average longitude of polygon vertices, i.e., centroid longitude
     Assume longitudes are within 180 degrees of one another
     Default orientation is monotonically increasing longitude from left to right 
     WLOG, adjust all longitudes to be on same branch as lon_ll */
  //  const char fnc_nm[]="nco_lon_ply_avg_brnch()";
  double lon_dff; /* [dgr] Longitude difference */
  double lon_avg; /* [dgr] Longitude average */
  int lon_idx; /* [idx] Polygon vertex index */
  
  assert(lon_nbr != 0);
  lon_avg=lon_crn[0];
  for(lon_idx=1;lon_idx<lon_nbr;lon_idx++){
    lon_avg+=lon_crn[lon_idx];
    lon_dff=lon_crn[lon_idx]-lon_crn[0];
    if(lon_dff >= 180.0){
      lon_avg-=360.0;
    }else if(lon_dff <= -180.0){
      lon_avg+=360.0;
    } /* !lon_dff */
  } /* !lon_idx */

  return lon_avg/lon_nbr;
} /* !nco_lon_ply_avg_brnch() */

nco_bool /* O [flg] Input corners were CCW */
nco_ccw_chk /* [fnc] Convert quadrilateral gridcell corners to CCW orientation */
(double * const crn_lat, /* [dgr] Latitude corners of gridcell */
 double * const crn_lon, /* [dgr] Latitude corners of gridcell */
 const int crn_nbr, /* [nbr] Number of corners per gridcell */
 int idx_ccw, /* [idx] Index of starting vertice for CCW check (Point A = tail side AB) */
 const int rcr_lvl) /* [nbr] Recursion level */
{
  /* Purpose: Determine whether corner vertices are oriented CCW
     If not, alter order so they are returned in CCW order
     Function can call itself, and rcr_lvl indicates recursion level:
     rcr_lvl=1: Called by host code, i.e., nco_grd_nfr()
     rcr_lvl=2: Called by itself, i.e., nco_ccw_chk()
     Assumptions:
     Quadrilateral vertices are already corrected to obey branch-cut rules, i.e.,
     all vertices are on "same side" of dateline or Greenwich as appropriate
     Algorithm:
     Start crn_idx=0, i.e., quadrilateral LL corner
     Vector A runs from crn_idx=0 to crn_idx=1, i.e., quadrilateral LL->LR
     Vector B runs from crn_idx=1 to crn_idx=2, i.e., quadrilateral LR->UR
     Compute cross-product A x B = C
     C is normal to plane containining A and B
     Dot-product of C with radial vector to head A = tail B is positive if A and B are CCW
     if(ABC is CCW){
       if(CDA is CCW) 
         Done 
       else 
         Copy D:=A (make CDA degenerate, triangularize quadrilateral)
       endif
     }else(ABC is not CCW){
       Assume entire quadrilateral is CW
       Take mirror image of quadrilateral by switching B with D
       If(new ABC is CCW){
         If(CDA is CCW) 
           Done 
	 else 
	   Copy D:=A (make CDA degenerate, triangularize quadrilateral)
         endif
       }else{
         Fail (return False, meaning point should be masked)
     }
     All cases return True (i.e., CCW) from rcr_lvl=1 except last
     Last case returns False, and calling code should mask such an aberrant point */ 
  const char fnc_nm[]="nco_ccw_chk()";

  /* MSVC compiler chokes unless array size is compile-time constant */
  const int CRN_NBR_MSVC=4;
  double sin_lat[CRN_NBR_MSVC];
  double sin_lon[CRN_NBR_MSVC];
  double cos_lat[CRN_NBR_MSVC];
  double cos_lon[CRN_NBR_MSVC];

  double A_tail_x,A_tail_y,A_tail_z;
  double A_head_x,A_head_y,A_head_z;
  double A_x,A_y,A_z;
  double B_tail_x,B_tail_y,B_tail_z;
  double B_head_x,B_head_y,B_head_z;
  double B_x,B_y,B_z;
  double C_x,C_y,C_z;
  double R_x,R_y,R_z;
  double lat_rdn;
  double lon_rdn;
  double dot_prd;
  int crn_idx; /* [idx] Corner idx */
  int A_tail_idx,A_head_idx;
  int B_tail_idx,B_head_idx;
  nco_bool flg_ccw; /* [flg] Input is CCW */

  assert(crn_nbr == CRN_NBR_MSVC);
  for(crn_idx=0;crn_idx<crn_nbr;crn_idx++){
    lat_rdn=crn_lat[crn_idx]*M_PI/180.0;
    lon_rdn=crn_lon[crn_idx]*M_PI/180.0;
    sin_lat[crn_idx]=sin(lat_rdn);
    cos_lat[crn_idx]=cos(lat_rdn);
    sin_lon[crn_idx]=sin(lon_rdn);
    cos_lon[crn_idx]=cos(lon_rdn);
  } /* !crn_idx */

  /* Calls from host code (i.e., nco_grd_nfr()) start at lower-left of quadrilateral ABCD = Point A = vertex 0
     Calls from self can start from quadrilateral Point A or C 
     To check triangle CDA, start at upper-right of quadrilateral ABCD = Point C = vertex 2 */
  A_tail_idx=idx_ccw;
  A_head_idx=B_tail_idx=(A_tail_idx+1)%crn_nbr;
  B_head_idx=(B_tail_idx+1)%crn_nbr;
  A_tail_x=cos_lat[A_tail_idx]*cos_lon[A_tail_idx];
  A_tail_y=cos_lat[A_tail_idx]*sin_lon[A_tail_idx];
  A_tail_z=sin_lat[A_tail_idx];
  A_head_x=B_tail_x=R_x=cos_lat[A_head_idx]*cos_lon[A_head_idx];
  A_head_y=B_tail_y=R_y=cos_lat[A_head_idx]*sin_lon[A_head_idx];
  A_head_z=B_tail_z=R_z=sin_lat[A_head_idx];
  B_head_x=cos_lat[B_head_idx]*cos_lon[B_head_idx];
  B_head_y=cos_lat[B_head_idx]*sin_lon[B_head_idx];
  B_head_z=sin_lat[B_head_idx];
  A_x=A_head_x-A_tail_x;
  A_y=A_head_y-A_tail_y;
  A_z=A_head_z-A_tail_z;
  B_x=B_head_x-B_tail_x;
  B_y=B_head_y-B_tail_y;
  B_z=B_head_z-B_tail_z;
  /* Cross-Product C = A x B */
  C_x=A_y*B_z-B_y*A_z;
  C_y=-A_x*B_z+B_x*A_z;
  C_z=A_x*B_y-B_x*A_y;
  /* Dot-Product R dot C */
  dot_prd=C_x*R_x+C_y*R_y+C_z*R_z;

  if(dot_prd > 0.0) flg_ccw=True; else flg_ccw=False;

  if(flg_ccw && crn_nbr == 4 && rcr_lvl == 1){
    /* Original ABC is CCW, now check CDA */
    idx_ccw=2;
    flg_ccw=nco_ccw_chk(crn_lat,crn_lon,crn_nbr,idx_ccw,rcr_lvl+1);
    if(!flg_ccw && nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: WARNING %s reports triangle ABC is and CDA is not CCW in quadrilateral gridcell with LL (lat,lon) = (%g, %g), dot_prd = %g. Setting D:=A to triangularize quadrilateral.\n",nco_prg_nm_get(),fnc_nm,*crn_lat+0,*crn_lon+0,dot_prd);
    /* Triangularize quadrilateral D:=A */
    crn_lat[3]=crn_lat[0];
    crn_lon[3]=crn_lon[0];
    return True;
  }else if(!flg_ccw && crn_nbr == 4 && rcr_lvl == 1){
    /* Original ABC is not CCW
       20160124: Simplistic fix: reverse gridpoint order
       This only works for quadrilaterals without degenerate points */
    double crn_tmp;
    if(!flg_ccw && nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: INFO %s reports triangle ABC is non-CCW in quadrilateral gridcell with LL (lat,lon) = (%g, %g), dot_prd = %g. Mirror-imaging...\n",nco_prg_nm_get(),fnc_nm,*crn_lat+0,*crn_lon+0,dot_prd);
    crn_tmp=crn_lat[1];
    crn_lat[1]=crn_lat[3];
    crn_lat[3]=crn_tmp;
    crn_tmp=crn_lon[1];
    crn_lon[1]=crn_lon[3];
    crn_lon[3]=crn_tmp;
    /* Check new triangle ABC */
    idx_ccw=0;
    flg_ccw=nco_ccw_chk(crn_lat,crn_lon,crn_nbr,idx_ccw,rcr_lvl+1);
    if(flg_ccw){
      /* Inverted ABC is CCW, now check CDA */
      idx_ccw=2;
      flg_ccw=nco_ccw_chk(crn_lat,crn_lon,crn_nbr,idx_ccw,rcr_lvl+1);
      if(flg_ccw){
	return True;
      }else{
	if(!flg_ccw && nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(stdout,"%s: INFO %s reports triangle ABC is CCW after inversion, but triangle CDA is not at quadrilateral gridcell with LL (lat,lon) = (%g, %g), dot_prd = %g. Setting D:=A to triangularize quadrilateral.\n",nco_prg_nm_get(),fnc_nm,*crn_lat+0,*crn_lon+0,dot_prd);
	/* Triangularize quadrilateral D:=A */
	crn_lat[3]=crn_lat[0];
	crn_lon[3]=crn_lon[0];
	return True;
      } /* flg_ccw */
    }else{
      /* Original and Inverted ABC are not CCW */
      if(!flg_ccw && nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stdout,"%s: WARNING %s reports triangle ABC remains non-CCW after first inversion\n",nco_prg_nm_get(),fnc_nm);
      return False;
    } /* !flg_ccw */
  } /* flg_ccw */
    
  return flg_ccw;
} /* !nco_ccw_chk() */
