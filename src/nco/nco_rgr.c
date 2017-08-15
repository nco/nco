/* $Header$ */

/* Purpose: NCO regridding utilities */

/* Copyright (C) 2015--2017 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_rgr.h" /* Regridding */

int /* O [enm] Return code */
nco_rgr_ctl /* [fnc] Control regridding logic */
(rgr_sct * const rgr, /* I/O [sct] Regridding structure */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal Table */
{
  /* Purpose: Control regridding logic */
  int rcd=NCO_NOERR;
  const char fnc_nm[]="nco_rgr_ctl()";

  nco_bool flg_grd=False; /* [flg] Create SCRIP-format grid file */
  nco_bool flg_nfr=False; /* [flg] Infer SCRIP-format grid file */
  nco_bool flg_map=False; /* [flg] Regrid with external weights */
  nco_bool flg_smf=False; /* [flg] ESMF regridding */
  nco_bool flg_tps=False; /* [flg] Tempest regridding */

  /* Main control branching occurs here
     Branching complexity and utility will increase as regridding features are added */
  if(rgr->flg_grd) flg_grd=True;

  if(rgr->flg_nfr) flg_nfr=True;
  if(rgr->flg_map) flg_map=True;
  if(rgr->flg_grd_src && rgr->flg_grd_dst) flg_smf=True;
  if(rgr->drc_tps && !flg_map) flg_tps=True;
  assert(!(flg_smf && flg_map));
  assert(!(flg_smf && flg_tps));
  assert(!(flg_map && flg_tps));
  
  /* Create SCRIP-format grid file */
  if(flg_grd) rcd=nco_grd_mk(rgr);

  /* Infer SCRIP-format grid file */
  if(flg_nfr) rcd=nco_grd_nfr(rgr);

  /* Regrid using external mapping weights */
  if(flg_map) rcd=nco_rgr_map(rgr,trv_tbl);

  /* Regrid using ESMF library */
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
  
  /* Regrid using Tempest regridding */
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
  if(rgr->fl_in) rgr->fl_in=(char *)nco_free(rgr->fl_in);
  if(rgr->fl_out) rgr->fl_out=(char *)nco_free(rgr->fl_out);
  if(rgr->fl_out_tmp) rgr->fl_out_tmp=(char *)nco_free(rgr->fl_out_tmp);
  if(rgr->fl_map) rgr->fl_map=(char *)nco_free(rgr->fl_map);
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
  if(rgr->lat_bnd_nm) rgr->lat_bnd_nm=(char *)nco_free(rgr->lat_bnd_nm);
  if(rgr->lat_nm_in) rgr->lat_nm_in=(char *)nco_free(rgr->lat_nm_in);
  if(rgr->lat_nm_out) rgr->lat_nm_out=(char *)nco_free(rgr->lat_nm_out);
  if(rgr->lat_vrt_nm) rgr->lat_vrt_nm=(char *)nco_free(rgr->lat_vrt_nm);
  if(rgr->lat_wgt_nm) rgr->lat_wgt_nm=(char *)nco_free(rgr->lat_wgt_nm);
  if(rgr->lon_bnd_nm) rgr->lon_bnd_nm=(char *)nco_free(rgr->lon_bnd_nm);
  if(rgr->lon_nm_in) rgr->lon_nm_in=(char *)nco_free(rgr->lon_nm_in);
  if(rgr->lon_nm_out) rgr->lon_nm_out=(char *)nco_free(rgr->lon_nm_out);
  if(rgr->lon_vrt_nm) rgr->lon_vrt_nm=(char *)nco_free(rgr->lon_vrt_nm);
  if(rgr->msk_nm) rgr->msk_nm=(char *)nco_free(rgr->msk_nm);
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
 char * const rgr_map, /* I [sng] File containing mapping weights from source to destination grid */
 char * const rgr_var, /* I [sng] Variable for special regridding treatment */
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

  rgr->flg_map= rgr_map ? True : False; /* [flg] User-specified mapping weights */
  rgr->fl_map=rgr_map; /* [sng] File containing mapping weights from source to destination grid */

  rgr->var_nm=rgr_var; /* [sng] Variable for special regridding treatment */
  
  rgr->xtn_var=xtn_var; /* [sng] Extensive variables */
  rgr->xtn_nbr=xtn_nbr; /* [nbr] Number of extensive variables */

  /* Did user explicitly request regridding? */
  if(rgr_arg_nbr > 0 || rgr_grd_src != NULL || rgr_grd_dst != NULL || rgr_map != NULL) rgr->flg_usr_rqs=True;

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
    (void)fprintf(stderr,"fl_in = %s, ",rgr->fl_in ? rgr->fl_in : "NULL");
    (void)fprintf(stderr,"fl_out = %s, ",rgr->fl_out ? rgr->fl_out : "NULL");
    (void)fprintf(stderr,"fl_out_tmp = %s, ",rgr->fl_out_tmp ? rgr->fl_out_tmp : "NULL");
    (void)fprintf(stderr,"fl_map = %s, ",rgr->fl_map ? rgr->fl_map : "NULL");
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
  rgr->lat_bnd_nm=NULL; /* [sng] Name of rectangular boundary variable for latitude */
  rgr->lat_dmn_nm=NULL; /* [sng] Name of latitude dimension in inferred grid */
  rgr->lat_nm_in=NULL; /* [sng] Name of input dimension to recognize as latitude */
  rgr->lat_nm_out=NULL; /* [sng] Name of output dimension for latitude */
  rgr->lat_vrt_nm=NULL; /* [sng] Name of non-rectangular boundary variable for latitude */
  rgr->lat_wgt_nm=NULL; /* [sng] Name of variable containing latitude weights */
  rgr->lon_bnd_nm=NULL; /* [sng] Name of rectangular boundary variable for longitude */
  rgr->lon_dmn_nm=NULL; /* [sng] Name of longitude dimension in inferred grid */
  rgr->lon_nm_in=NULL; /* [sng] Name of dimension to recognize as longitude */
  rgr->lon_nm_out=NULL; /* [sng] Name of output dimension for longitude */
  rgr->lon_vrt_nm=NULL; /* [sng] Name of non-rectangular boundary variable for longitude */
  rgr->msk_nm=NULL; /* [sng] Name of variable containing destination mask */
  rgr->vrt_nm=NULL; /* [sng] Name of dimension to employ for vertices */

  /* Initialize key-value properties used in grid generation */
  rgr->fl_grd=NULL; /* [sng] Name of SCRIP grid file to create */
  rgr->fl_hnt_dst=NULL; /* [sng] ERWG hint destination */
  rgr->fl_hnt_src=NULL; /* [sng] ERWG hint source */
  rgr->fl_skl=NULL; /* [sng] Name of skeleton data file to create */
  rgr->fl_ugrid=NULL; /* [sng] Name of UGRID grid file to create */
  rgr->flg_area_out=True; /* [flg] Add area to output */
  rgr->flg_cll_msr=True; /* [flg] Add cell_measures attribute */
  rgr->flg_dgn_area=False; /* [flg] Diagnose rather than copy inferred area */
  rgr->flg_dgn_bnd=False; /* [flg] Diagnose rather than copy inferred bounds */
  rgr->flg_crv=False; /* [flg] Use curvilinear coordinates */
  rgr->flg_grd=False; /* [flg] Create SCRIP-format grid file */
  rgr->flg_msk_out=False; /* [flg] Add mask to output */
  rgr->flg_nfr=False; /* [flg] Infer SCRIP-format grid file */
  rgr->flg_stg=True; /* [flg] Write staggered grid with FV output */
  rgr->grd_ttl=strdup("None given (supply with --rgr grd_ttl=\"Grid Title\")"); /* [enm] Grid title */
  rgr->grd_typ=nco_grd_2D_eqa; /* [enm] Grid type */
  rgr->idx_dbg=0; /* [idx] Index of gridcell for debugging */
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
  rgr->tst=0L; /* [enm] Generic key for testing (undocumented) */
  
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
    if(!strcmp(rgr_lst[rgr_var_idx].key,"infer") || !strcmp(rgr_lst[rgr_var_idx].key,"nfr")){
      rgr->flg_nfr=True;
      continue;
    } /* !infer */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"diagnose_area") || !strcmp(rgr_lst[rgr_var_idx].key,"dgn_area")){
      rgr->flg_dgn_area=True;
      continue;
    } /* !diagnose_area */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"diagnose_bounds") || !strcmp(rgr_lst[rgr_var_idx].key,"dgn_bnd")){
      rgr->flg_dgn_bnd=True;
      continue;
    } /* !diagnose_bounds */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"no_stagger") || !strcmp(rgr_lst[rgr_var_idx].key,"no_stg")){
      rgr->flg_stg=False;
      continue;
    } /* !stagger */
    if(!strcmp(rgr_lst[rgr_var_idx].key,"grd_ttl")){
      if(rgr->grd_ttl) rgr->grd_ttl=(char *)nco_free(rgr->grd_ttl);
      rgr->grd_ttl=(char *)strdup(rgr_lst[rgr_var_idx].val);
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
      if(!strcasecmp(rgr_lst[rgr_var_idx].val,"180_wst"))
	rgr->lon_typ=nco_grd_lon_180_wst;
      else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"180_ctr"))
	rgr->lon_typ=nco_grd_lon_180_ctr;
      else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"Grn_wst"))
	rgr->lon_typ=nco_grd_lon_Grn_wst;
      else if(!strcasecmp(rgr_lst[rgr_var_idx].val,"Grn_ctr"))
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
  if(!rgr->lat_bnd_nm) rgr->lat_bnd_nm=(char *)strdup("lat_bnds"); /* [sng] Name of rectangular boundary variable for latitude */
  if(!rgr->lat_nm_in) rgr->lat_nm_in=(char *)strdup("lat"); /* [sng] Name of input dimension to recognize as latitude */
  if(!rgr->lat_vrt_nm) rgr->lat_vrt_nm=(char *)strdup("lat_vertices"); /* [sng] Name of non-rectangular boundary variable for latitude */
  if(!rgr->lat_wgt_nm) rgr->lat_wgt_nm=(char *)strdup("gw"); /* [sng] Name of variable containing latitude weights */
  if(!rgr->lon_bnd_nm) rgr->lon_bnd_nm=(char *)strdup("lon_bnds"); /* [sng] Name of rectangular boundary variable for longitude */
  if(!rgr->lon_nm_in) rgr->lon_nm_in=(char *)strdup("lon"); /* [sng] Name of dimension to recognize as longitude */
  if(!rgr->lon_vrt_nm) rgr->lon_vrt_nm=(char *)strdup("lon_vertices"); /* [sng] Name of non-rectangular boundary variable for longitude */
  if(!rgr->msk_nm) rgr->msk_nm=(char *)strdup("mask"); /* [sng] Name of variable containing destination mask */
  if(!rgr->vrt_nm) rgr->vrt_nm=(char *)strdup("nv"); /* [sng] Name of dimension to employ for vertices */

  /* Derived from defaults and command-line arguments */
  // On second thought, do no strdup() these here. This way, NULL means user never specified lon/lat-out names
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
nco_rgr_map /* [fnc] Regrid with external weights */
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
     Sparse matrix formulations:

     for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
       // Normalization: fractional area (fracarea)
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0];
       // Normalization: destination area (destarea)
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0]/dst_area[ddr_dst[lnk_idx]];
       // Normalization: none
       dst[ddr_dst[lnk_idx]]+=src[ddr_src[lnk_idx]]*remap_matrix[lnk_idx,0]/(dst_area[ddr_dst[lnk_idx]]*dst_frc[ddr_dst[lnk_idx]);
     } // end loop over lnk

     Documentation:
     NCL special cases described in popRemap.ncl, e.g., at
     https://github.com/yyr/ncl/blob/master/ni/src/examples/gsun/popRemap.ncl

     ESMF Regridding Status:
     http://www.earthsystemmodeling.org/esmf_releases/last/regridding_status.html

     Sample regrid T42->POP43, SCRIP:
     ncks -O --map=${DATA}/scrip/rmp_T42_to_POP43_conserv.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc */

  const char fnc_nm[]="nco_rgr_map()"; /* [sng] Function name */

  char *fl_in;
  char *fl_pth_lcl=NULL;

  const double rdn2dgr=180.0/M_PI;
  const double dgr2rdn=M_PI/180.0;
  const double eps_rlt=1.0e-14; /* [frc] Round-off error tolerance */
  double lat_wgt_ttl=0.0; /* [frc] Actual sum of quadrature weights */
  double area_out_ttl=0.0; /* [frc] Exact sum of area */

  int in_id; /* I [id] Input netCDF file ID */
  int out_id; /* I [id] Output netCDF file ID */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int rcd=NC_NOERR;

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int dst_grid_corners_id; /* [id] Destination grid corners dimension ID */
  int dst_grid_rank_id; /* [id] Destination grid rank dimension ID */
  int dst_grid_size_id; /* [id] Destination grid size dimension ID */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int num_links_id; /* [id] Number of links dimension ID */
  int num_wgts_id; /* [id] Number of weights dimension ID */
  int src_grid_corners_id; /* [id] Source grid corners dimension ID */
  int src_grid_rank_id; /* [id] Source grid rank dimension ID */
  int src_grid_size_id; /* [id] Source grid size dimension ID */

  long int lat_idx;
  long int lon_idx;

  short int bnd_idx;

  nco_bool FL_RTR_RMT_LCN;
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool flg_dgn_area_out=False; /* [flg] Diagnose area_out from grid boundaries */
  nco_bool flg_bnd_1D_usable=False; /* [flg] Usable 1D cell vertices exist */
  nco_bool flg_stg=rgr->flg_stg; /* [flg] Write staggered grid with FV output */
  
  nco_grd_2D_typ_enm nco_grd_2D_typ=nco_grd_2D_nil; /* [enm] Two-dimensional grid-type enum */
  nco_grd_lat_typ_enm nco_grd_lat_typ=nco_grd_lat_nil; /* [enm] Latitude grid-type enum */
  nco_grd_lon_typ_enm nco_grd_lon_typ=nco_grd_lon_nil; /* [enm] Longitude grid-type enum */

  nco_mpf_sct rgr_map;

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(stderr,"%s: INFO %s obtaining mapping weights from %s\n",nco_prg_nm_get(),fnc_nm,rgr->fl_map);

  /* Duplicate (because nco_fl_mk_lcl() free()'s fl_in) */
  fl_in=(char *)strdup(rgr->fl_map);
  /* Make sure file is on local system and is readable or die trying */
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  /* Identify mapping file type using string generated by weight-generator:
     ESMF_Regridder: conventions = "NCAR-CSM"
     NCO: conventions = "fxm" (Currently NCO produces gridfiles, not mapfiles)
     SCRIP: conventions = "SCRIP"
     Tempest: Title = "TempestRemap Offline Regridding Weight Generator" */
  char *att_val;
  char *cnv_sng=NULL_CEWI;
  /* netCDF standard is uppercase Conventions, though some models user lowercase */
  char cnv_sng_UC[]="Conventions"; /* [sng] Unidata standard     string (uppercase) */
  char cnv_sng_LC[]="conventions"; /* [sng] Unidata non-standard string (lowercase) */
  char cnv_sng_tps[]="Title"; /* [sng] Tempest uses "Title" not "Conventions" attribute */
  char cnv_sng_ttl[]="title"; /* [sng] ERWG 7.1 weight_only uses "title" not "Conventions" attribute */
  char name0_sng[]="name0"; /* [sng] Attribute where Tempest stores least-rapidly-varying dimension name */
  
  long att_sz;

  nc_type att_typ;

  nco_rgr_mpf_typ_enm nco_rgr_mpf_typ=nco_rgr_mpf_nil; /* [enm] Type of remapping file */
  nco_rgr_typ_enm nco_rgr_typ=nco_rgr_grd_nil; /* [enm] Type of grid conversion */
  
  /* Look for map-type signature in [cC]onventions or [tT]itle attribute */
  cnv_sng=cnv_sng_UC;
  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd != NC_NOERR){
    /* Re-try with lowercase string because some models, e.g., CLM, user lowercase "conventions" */
    cnv_sng=cnv_sng_LC;
    rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  } /* endif lowercase */
  if(rcd != NC_NOERR){
    /* Re-try with Tempest "Title" */
    cnv_sng=cnv_sng_tps;
    rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  } /* endif Tempest */
  if(rcd != NC_NOERR){
    /* Re-try with ERWG "title" */
    /* ERWG 7.1+ --weight_only has "title" == "ESMF Regrid Weight Generator" and no "Conventions" */
    cnv_sng=cnv_sng_ttl;
    rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  } /* endif Tempest */
  
  /* If "Conventions" attribute was found, it determines map-file type... */
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    att_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
    rcd+=nco_get_att(in_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate attribute before using strstr() */
    att_val[att_sz]='\0';
    /* ESMF conventions */
    if(strstr(att_val,"NCAR-CSM")) nco_rgr_mpf_typ=nco_rgr_mpf_ESMF;
    else if(strstr(att_val,"SCRIP")) nco_rgr_mpf_typ=nco_rgr_mpf_SCRIP;
    else if(strstr(att_val,"Tempest")) nco_rgr_mpf_typ=nco_rgr_mpf_Tempest;
    else if(strstr(att_val,"ESMF Regrid Weight Generator")) nco_rgr_mpf_typ=nco_rgr_mpf_ESMF_weight_only;
    if(nco_rgr_mpf_typ == nco_rgr_mpf_nil) (void)fprintf(stderr,"%s: ERROR %s unrecognized map-type specified in attribute Conventions = %s\n",nco_prg_nm_get(),fnc_nm,att_val);
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
  case nco_rgr_mpf_ESMF_weight_only:
    rcd+=nco_inq_dimid(in_id,"n_s",&num_links_id);
    break;
  case nco_rgr_mpf_ESMF:
  case nco_rgr_mpf_Tempest:
    rcd+=nco_inq_dimid(in_id,"n_a",&src_grid_size_id);
    rcd+=nco_inq_dimid(in_id,"n_b",&dst_grid_size_id);
    rcd+=nco_inq_dimid(in_id,"nv_a",&src_grid_corners_id);
    rcd+=nco_inq_dimid(in_id,"nv_b",&dst_grid_corners_id);
    rcd+=nco_inq_dimid(in_id,"src_grid_rank",&src_grid_rank_id);
    rcd+=nco_inq_dimid(in_id,"dst_grid_rank",&dst_grid_rank_id);
    if(nco_rgr_mpf_typ != nco_rgr_mpf_Tempest) rcd+=nco_inq_dimid(in_id,"num_wgts",&num_wgts_id);
    rcd+=nco_inq_dimid(in_id,"n_s",&num_links_id);
    break;
  default:
    (void)fprintf(stderr,"%s: ERROR %s unknown map-file type\n",nco_prg_nm_get(),fnc_nm);
    nco_dfl_case_generic_err();
    /* NB: This return never executes because nco_dfl_case_generic_err() calls exit()
       Return placed here to suppress clang -Wsometimes-uninitialized warnings
       This is done many other times throughout the code, though explained only once, here */
    return NCO_ERR;
    break;
  } /* end switch */

  /* Use dimension IDs to get dimension sizes */
  rcd+=nco_inq_dimlen(in_id,num_links_id,&rgr_map.num_links);
  if(nco_rgr_mpf_typ != nco_rgr_mpf_ESMF_weight_only){
    rcd+=nco_inq_dimlen(in_id,src_grid_size_id,&rgr_map.src_grid_size);
    rcd+=nco_inq_dimlen(in_id,dst_grid_size_id,&rgr_map.dst_grid_size);
    rcd+=nco_inq_dimlen(in_id,src_grid_corners_id,&rgr_map.src_grid_corners);
    rcd+=nco_inq_dimlen(in_id,dst_grid_corners_id,&rgr_map.dst_grid_corners);
    rcd+=nco_inq_dimlen(in_id,src_grid_rank_id,&rgr_map.src_grid_rank);
    rcd+=nco_inq_dimlen(in_id,dst_grid_rank_id,&rgr_map.dst_grid_rank);
    /* TempestRemap does not generate num_wgts */
    if(nco_rgr_mpf_typ == nco_rgr_mpf_Tempest) rgr_map.num_wgts=int_CEWI; else rcd+=nco_inq_dimlen(in_id,num_wgts_id,&rgr_map.num_wgts); 
    assert(rgr_map.src_grid_size < INT_MAX && rgr_map.dst_grid_size < INT_MAX);
  }else{
    rgr_map.src_grid_size=long_CEWI;
    rgr_map.dst_grid_size=long_CEWI;
    rgr_map.src_grid_corners=long_CEWI;
    rgr_map.dst_grid_corners=long_CEWI;
    rgr_map.src_grid_rank=long_CEWI;
    rgr_map.dst_grid_rank=long_CEWI;
    rgr_map.num_wgts=int_CEWI;
  } /* !ESMF_weight_only */
  
  cnv_sng=strdup("normalization");
  nco_rgr_nrm_typ_enm nco_rgr_nrm_typ=nco_rgr_nrm_nil;
  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1L);
    rcd+=nco_get_att(in_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate attribute before using strstr() */
    att_val[att_sz]='\0';
    if(strstr(att_val,"fracarea")) nco_rgr_nrm_typ=nco_rgr_nrm_fracarea;
    if(strstr(att_val,"destarea")) nco_rgr_nrm_typ=nco_rgr_nrm_destarea;
    if(strstr(att_val,"none")) nco_rgr_nrm_typ=nco_rgr_nrm_none;
    if(att_val) att_val=(char *)nco_free(att_val);
  }else{
    /* 20150712: Tempest does not store a normalization attribute
       20170620: ESMF weight_only does not store a normalization attribute */
    if(nco_rgr_mpf_typ == nco_rgr_mpf_Tempest || nco_rgr_mpf_typ == nco_rgr_mpf_ESMF_weight_only) nco_rgr_nrm_typ=nco_rgr_nrm_unknown;
  } /* endif normalization */
  assert(nco_rgr_nrm_typ != nco_rgr_nrm_nil);
  if(cnv_sng) cnv_sng=(char *)nco_free(cnv_sng);

  cnv_sng=strdup("map_method");
  nco_rgr_mth_typ_enm nco_rgr_mth_typ=nco_rgr_mth_nil;
  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,cnv_sng,&att_typ,&att_sz);
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    /* Add one for NUL byte */
    att_val=(char *)nco_malloc(att_sz*nco_typ_lng(att_typ)+1L);
    rcd+=nco_get_att(in_id,NC_GLOBAL,cnv_sng,att_val,att_typ);
    /* NUL-terminate attribute before using strstr() */
    att_val[att_sz]='\0';
    if(strstr(att_val,"Conservative remapping")) nco_rgr_mth_typ=nco_rgr_mth_conservative;
    if(strstr(att_val,"Bilinear remapping")) nco_rgr_mth_typ=nco_rgr_mth_bilinear;
    if(strstr(att_val,"none")) nco_rgr_mth_typ=nco_rgr_mth_none;
    if(att_val) att_val=(char *)nco_free(att_val);
  }else{
    /* Tempest does not store a map_method attribute */
    if(nco_rgr_mpf_typ == nco_rgr_mpf_Tempest) nco_rgr_mth_typ=nco_rgr_mth_unknown;
  } /* endif */
  assert(nco_rgr_mth_typ != nco_rgr_mth_nil);
  if(cnv_sng) cnv_sng=(char *)nco_free(cnv_sng);

  if(nco_dbg_lvl_get() >= nco_dbg_scl){
    (void)fprintf(stderr,"%s: INFO %s regridding input metadata and grid sizes: ",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"mapfile_generator = %s, map_method = %s, normalization = %s, src_grid_size = n_a = %li, dst_grid_size = n_b = %li, src_grid_corners = nv_a = %li, dst_grid_corners = nv_b = %li, src_grid_rank = %li, dst_grid_rank = %li, num_links = n_s = %li, num_wgts = %li\n",nco_rgr_mpf_sng(nco_rgr_mpf_typ),nco_rgr_mth_sng(nco_rgr_mth_typ),nco_rgr_nrm_sng(nco_rgr_nrm_typ),rgr_map.src_grid_size,rgr_map.dst_grid_size,rgr_map.src_grid_corners,rgr_map.dst_grid_corners,rgr_map.src_grid_rank,rgr_map.dst_grid_rank,rgr_map.num_links,rgr_map.num_wgts);
  } /* endif dbg */
  if(nco_rgr_nrm_typ == nco_rgr_nrm_none){
    (void)fprintf(stdout,"%s: ERROR %s reports requested normalization type = %s is not yet supported. Specifically, masks specified by a mask variable (dst_grid_imask,mask_b) are ignored. More specifically, any destination mask information is assumed to be built into the weight array so that no source points will contribute to masked locations. Talk to Charlie if you want this changed.\n",nco_prg_nm_get(),fnc_nm,nco_rgr_nrm_sng(nco_rgr_nrm_typ));
    nco_exit(EXIT_FAILURE);
  } /* !msk */
    
  /* Got to here in bullet-proofing code for weight-only map-files */
  if(nco_rgr_mpf_typ == nco_rgr_mpf_ESMF_weight_only) (void)fprintf(stderr,"%s: WARNING %s reached end of ESMF_weight_only section\n",nco_prg_nm_get(),fnc_nm);
  assert(nco_rgr_mpf_typ != nco_rgr_mpf_ESMF_weight_only);
 
  /* Set type of grid conversion */
  if(rgr_map.src_grid_rank == 1 && rgr_map.dst_grid_rank == 1) nco_rgr_typ=nco_rgr_grd_1D_to_1D;
  if(rgr_map.src_grid_rank == 1 && rgr_map.dst_grid_rank == 2) nco_rgr_typ=nco_rgr_grd_1D_to_2D;
  if(rgr_map.src_grid_rank == 2 && rgr_map.dst_grid_rank == 1) nco_rgr_typ=nco_rgr_grd_2D_to_1D;
  if(rgr_map.src_grid_rank == 2 && rgr_map.dst_grid_rank == 2) nco_rgr_typ=nco_rgr_grd_2D_to_2D;
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
  case nco_rgr_mpf_Tempest:
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
    (void)fprintf(stderr,"%s: ERROR %s unknown map file type\n",nco_prg_nm_get(),fnc_nm);
    nco_dfl_case_generic_err();
    /* NB: This return never executes because nco_dfl_case_generic_err() calls exit()
       Return placed here to suppress clang -Wsometimes-uninitialized warnings
       This is done many other times throughout the code, though explained only once, here */
    return NCO_ERR;
    break;
  } /* end switch */

  /* Obtain fields whose presence depends on mapfile type */
  nco_bool flg_msk_out=rgr->flg_msk_out; /* [flg] Add mask to output */
  if(flg_msk_out){
    if(nco_rgr_mpf_typ == nco_rgr_mpf_ESMF){
      rcd+=nco_inq_varid(in_id,"mask_b",&msk_dst_id); /* SCRIP: dst_grid_imask */
    }else if(nco_rgr_mpf_typ == nco_rgr_mpf_SCRIP){
      rcd+=nco_inq_varid(in_id,"dst_grid_imask",&msk_dst_id); /* ESMF: mask_b */
    }else{ /* !SCRIP */
      msk_dst_id=NC_MIN_INT;
      flg_msk_out=False;
    } /* !Tempest */
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
      rcd=nco_inq_att_flg(in_id,dmn_sz_in_int_id,name0_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	att_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd+=nco_get_att(in_id,dmn_sz_in_int_id,name0_sng,att_val,att_typ);
	/* NUL-terminate attribute before using strstr() */
	att_val[att_sz]='\0';
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
      rcd=nco_inq_att_flg(in_id,dmn_sz_in_int_id,name0_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	att_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd+=nco_get_att(in_id,dmn_sz_in_int_id,name0_sng,att_val,att_typ);
	/* NUL-terminate attribute before using strstr() */
	att_val[att_sz]='\0';
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
    
  /* Allocate space to hold dimension metadata for rectangular destination grid */
  dmn_srt=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));
  dmn_cnt=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));
  dmn_srd=(long *)nco_malloc(dmn_nbr_grd_max*sizeof(long));

  dmn_srt[0]=0L;
  dmn_cnt[0]=rgr_map.src_grid_rank;
  dmn_sz_in_int=(int *)nco_malloc(rgr_map.src_grid_rank*nco_typ_lng((nc_type)NC_INT));
  rcd=nco_get_vara(in_id,dmn_sz_in_int_id,dmn_srt,dmn_cnt,dmn_sz_in_int,(nc_type)NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=rgr_map.dst_grid_rank;
  dmn_sz_out_int=(int *)nco_malloc(rgr_map.dst_grid_rank*nco_typ_lng((nc_type)NC_INT));
  rcd=nco_get_vara(in_id,dmn_sz_out_int_id,dmn_srt,dmn_cnt,dmn_sz_out_int,(nc_type)NC_INT);

  /* Check-for and workaround faulty Tempest and MPAS-O/I grid sizes */
  if(flg_grd_in_1D && (rgr_map.src_grid_size != dmn_sz_in_int[0])){
    (void)fprintf(stdout,"%s: INFO %s reports input grid dimension sizes disagree: rgr_map.src_grid_size = %ld != %d = dmn_sz_in[0]. Problem may be caused by incorrect src_grid_dims variable. This is a known issue with some TempestRemap mapfiles generated prior to ~20150901, and in some ESMF mapfiles for MPAS-O/I. This problem can be safely ignored if workaround succeeds. Attempting workaround ...\n",nco_prg_nm_get(),fnc_nm,rgr_map.src_grid_size,dmn_sz_in_int[0]);
      dmn_sz_in_int[0]=rgr_map.src_grid_size;
  } /* !bug */
  if(flg_grd_out_1D && (rgr_map.dst_grid_size != dmn_sz_out_int[0])){
    (void)fprintf(stdout,"%s: INFO %s reports output grid dimension sizes disagree: rgr_map.dst_grid_size = %ld != %d = dmn_sz_out[0]. Problem may be caused by incorrect dst_grid_dims variable. This is a known issue with some TempestRemap mapfiles generated prior to ~20150901, and in some ESMF mapfiles for MPAS-O/I. This problem can be safely ignored if workaround succeeds. Attempting workaround ...\n",nco_prg_nm_get(),fnc_nm,rgr_map.dst_grid_size,dmn_sz_out_int[0]);
    dmn_sz_out_int[0]=rgr_map.dst_grid_size;
  } /* !bug */
 
  long col_nbr_in; /* [idx] Number of columns in source grid */
  long lon_nbr_in; /* [idx] Number of longitudes in rectangular source grid */
  long lat_nbr_in; /* [idx] Number of latitudes  in rectangular source grid */
  const size_t grd_sz_in=rgr_map.src_grid_size; /* [nbr] Number of elements in single layer of input grid */
  const size_t grd_sz_out=rgr_map.dst_grid_size; /* [nbr] Number of elements in single layer of output grid */
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
    bnd_nbr_out=rgr_map.dst_grid_corners;
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
  rcd=nco_inq_att_flg(in_id,dst_grd_ctr_lat_id,unt_sng,&att_typ,&att_sz);
  if(rcd == NC_NOERR && att_typ == NC_CHAR){
    att_val=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
    rcd+=nco_get_att(in_id,dst_grd_ctr_lat_id,unt_sng,att_val,att_typ);
    /* NUL-terminate attribute before using strstr() */
    att_val[att_sz]='\0';
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
    lon_crn_out=(double *)nco_malloc(rgr_map.dst_grid_corners*grd_sz_out*nco_typ_lng(crd_typ_out));
    lat_crn_out=(double *)nco_malloc(rgr_map.dst_grid_corners*grd_sz_out*nco_typ_lng(crd_typ_out));

    dmn_srt[0]=0L;
    dmn_cnt[0]=grd_sz_out;
    rcd=nco_get_vara(in_id,dst_grd_ctr_lon_id,dmn_srt,dmn_cnt,lon_ctr_out,crd_typ_out);
    rcd=nco_get_vara(in_id,dst_grd_ctr_lat_id,dmn_srt,dmn_cnt,lat_ctr_out,crd_typ_out);
    dmn_srt[0]=dmn_srt[1]=0L;
    dmn_cnt[0]=grd_sz_out;
    dmn_cnt[1]=rgr_map.dst_grid_corners;
    rcd=nco_get_vara(in_id,dst_grd_crn_lon_id,dmn_srt,dmn_cnt,lon_crn_out,crd_typ_out);
    rcd=nco_get_vara(in_id,dst_grd_crn_lat_id,dmn_srt,dmn_cnt,lat_crn_out,crd_typ_out);

    /* User may specify curvilinear grid (with --rgr crv). If not specified, manually test output grid for curvilinearity... */
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
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO Output grid detected to be %s\n",nco_prg_nm_get(),flg_grd_out_crv ? "Curvilinear" : "Rectangular");
    } /* !flg_grd_out_crv */

    if(flg_grd_out_crv) bnd_nbr_out=rgr_map.dst_grid_corners;
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
    lon_crn_out=(double *)nco_malloc(rgr_map.dst_grid_corners*lon_nbr_out*nco_typ_lng(crd_typ_out));
    lat_crn_out=(double *)nco_malloc(rgr_map.dst_grid_corners*lat_nbr_out*nco_typ_lng(crd_typ_out));
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
    dmn_srd[0]=lon_nbr_out;
    dmn_srt[1]=0L;
    dmn_cnt[1]=rgr_map.dst_grid_corners;
    dmn_srd[1]=1L;
    rcd=nco_get_vars(in_id,dst_grd_crn_lat_id,dmn_srt,dmn_cnt,dmn_srd,lat_crn_out,crd_typ_out);
    if(flg_crd_rdn){
      for(idx=0;idx<lon_nbr_out;idx++) lon_ctr_out[idx]*=rdn2dgr;
      for(idx=0;idx<lat_nbr_out;idx++) lat_ctr_out[idx]*=rdn2dgr;
      for(idx=0;idx<lon_nbr_out*rgr_map.dst_grid_corners;idx++) lon_crn_out[idx]*=rdn2dgr;
      for(idx=0;idx<lat_nbr_out*rgr_map.dst_grid_corners;idx++) lat_crn_out[idx]*=rdn2dgr;
    } /* !rdn */
  } /* !flg_grd_out_rct */
    
  if(flg_grd_out_crv){
    if(flg_crd_rdn){
      for(idx=0;idx<(long)grd_sz_out;idx++) lon_ctr_out[idx]*=rdn2dgr;
      for(idx=0;idx<(long)grd_sz_out;idx++) lat_ctr_out[idx]*=rdn2dgr;
      for(idx=0;idx<(long)grd_sz_out*rgr_map.dst_grid_corners;idx++) lon_crn_out[idx]*=rdn2dgr;
      for(idx=0;idx<(long)grd_sz_out*rgr_map.dst_grid_corners;idx++) lat_crn_out[idx]*=rdn2dgr;
    } /* !rdn */
  } /* !flg_grd_out_crv */

  /* Allocate space for and obtain area, fraction, and mask, which are needed for both 1D and 2D grids */
  area_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ_out));
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_out;
  rcd=nco_get_vara(in_id,area_dst_id,dmn_srt,dmn_cnt,area_out,crd_typ_out);

  frc_out=(double *)nco_malloc(grd_sz_out*nco_typ_lng(crd_typ_out));
  dmn_srt[0]=0L;
  dmn_cnt[0]=grd_sz_out;
  rcd=nco_get_vara(in_id,frc_dst_id,dmn_srt,dmn_cnt,frc_out,crd_typ_out);

  if(msk_dst_id != NC_MIN_INT){
    msk_out=(int *)nco_malloc(grd_sz_out*nco_typ_lng(NC_INT));
    dmn_srt[0]=0L;
    dmn_cnt[0]=grd_sz_out;
    rcd=nco_get_vara(in_id,msk_dst_id,dmn_srt,dmn_cnt,msk_out,NC_INT);
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
     lon_ntf_out[0]=0.5*(lon_ctr_out[0]+lon_ctr_out[lon_nbr_out-1])-180.0; // Extrapolation
     lat_ntf_out[0]=lat_ctr_out[0]-0.5*(lat_ctr_out[1]-lat_ctr_out[0]); // Extrapolation
     for(idx=1;idx<lon_nbr_out;idx++) lon_ntf_out[idx]=0.5*(lon_ctr_out[idx-1]+lon_ctr_out[idx]);
     for(idx=1;idx<lat_nbr_out;idx++) lat_ntf_out[idx]=0.5*(lat_ctr_out[idx-1]+lat_ctr_out[idx]);
     lon_ntf_out[lon_nbr_out]=lon_ntf_out[0]+360.0;
     lat_ntf_out[lat_nbr_out]=lat_ctr_out[lat_nbr_out-1]+0.5*(lat_ctr_out[lat_nbr_out-1]-lat_ctr_out[lat_nbr_out-2]); */

  if(flg_grd_out_rct){
    double lon_spn; /* [dgr] Longitude span */
    double lat_spn; /* [dgr] Latitude span */

    /* Obtain 1-D rectangular interfaces from unrolled 1-D vertice arrays */
    for(idx=0;idx<lon_nbr_out;idx++) lon_ntf_out[idx]=lon_crn_out[rgr_map.dst_grid_corners*idx];
    lon_ntf_out[lon_nbr_out]=lon_crn_out[rgr_map.dst_grid_corners*lon_nbr_out-(rgr_map.dst_grid_corners-1L)];
    lon_spn=lon_ntf_out[lon_nbr_out]-lon_ntf_out[0];
    for(idx=0;idx<lat_nbr_out;idx++) lat_ntf_out[idx]=lat_crn_out[rgr_map.dst_grid_corners*idx];
    lat_ntf_out[lat_nbr_out]=lat_crn_out[rgr_map.dst_grid_corners*lat_nbr_out-1L];
    lat_spn=lat_ntf_out[lat_nbr_out]-lat_ntf_out[0];

    /* Place 1-D rectangular interfaces into 2-D coordinate boundaries */
    for(idx=0;idx<lon_nbr_out;idx++){
      lon_bnd_out[2*idx]=lon_ntf_out[idx];
      lon_bnd_out[2*idx+1]=lon_ntf_out[idx+1];
    } /* end loop over longitude */
    for(idx=0;idx<lat_nbr_out;idx++){
      lat_bnd_out[2*idx]=lat_ntf_out[idx];
      lat_bnd_out[2*idx+1]=lat_ntf_out[idx+1];
    } /* end loop over latitude */

    if(nco_dbg_lvl_get() >= nco_dbg_crr){
      for(idx=0;idx<lon_nbr_out;idx++) (void)fprintf(stdout,"lon[%li] = [%g, %g, %g]\n",idx,lon_bnd_out[2*idx],lon_ctr_out[idx],lon_bnd_out[2*idx+1]);
      for(idx=0;idx<lat_nbr_out;idx++) (void)fprintf(stdout,"lat[%li] = [%g, %g, %g]\n",idx,lat_bnd_out[2*idx],lat_ctr_out[idx],lat_bnd_out[2*idx+1]);
    } /* endif dbg */

    /* Global or regional grid? */
    nco_grd_xtn_enm nco_grd_xtn; /* [enm] Extent of grid */
    if((float)lon_spn == 360.0f && (float)lat_spn == 180.0f) nco_grd_xtn=nco_grd_xtn_glb; else nco_grd_xtn=nco_grd_xtn_rgn;
    /* Diagnose type of latitude output grid by testing second latitude center against formulae */
    const double lat_ctr_tst_eqa=lat_ntf_out[0]+lat_spn*1.5/lat_nbr_out;
    const double lat_ctr_tst_fv=lat_ntf_out[0]+lat_spn/(lat_nbr_out-1);
    double lat_ctr_tst_gss;
    /* In diagnosing grids, agreement to slightly worse than single-precision is "good enough for government work"
       Hence some comparisons cast from double to float before comparison
       20150526: T42 grid from SCRIP and related maps, and NCL-generated Gaussian grids for CESM, are accurate to at most ~eight digits
       20150611: map_ne120np4_to_fv801x1600_bilin.150418.nc has yc_b[1600]=-89.775000006 not expected exact value lat_ctr[1]=-89.775000000000006
       20170521: T62 grid from NCEP-NCAR Reanalysis 1 worse than single precision, has yc_[192]=-86.6531 not expected exact value lat_ctr[1]=-86.6532 */

    if((float)lat_ctr_out[1] == (float)lat_ctr_tst_eqa) nco_grd_lat_typ=nco_grd_lat_eqa;
    if((float)lat_ctr_out[1] == (float)lat_ctr_tst_fv) nco_grd_lat_typ=nco_grd_lat_fv;
    double *wgt_Gss_out=NULL; // [frc] Gaussian weights double precision
    if(nco_grd_lat_typ == nco_grd_lat_nil){
      /* Check for Gaussian grid */
      double *lat_sin_out; // [frc] Sine of Gaussian latitudes double precision
      lat_sin_out=(double *)nco_malloc(lat_nbr_out*sizeof(double));
      wgt_Gss_out=(double *)nco_malloc(lat_nbr_out*sizeof(double));
      (void)nco_lat_wgt_gss(lat_nbr_out,lat_sin_out,wgt_Gss_out);
      lat_ctr_tst_gss=rdn2dgr*asin(lat_sin_out[1]);
      /* Gaussian weights on output grid will be double-precision accurate
	 Grid itself is kept as user-specified so area diagnosed by ESMF_RegridWeightGen may be slightly inconsistent with weights */
      if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stderr,"%s: INFO %s reports lat_ctr_out[1] = %g, lat_ctr_tst_gss = %g\n",nco_prg_nm_get(),fnc_nm,lat_ctr_out[1],lat_ctr_tst_gss);
      if((float)lat_ctr_out[1] == (float)lat_ctr_tst_gss) nco_grd_lat_typ=nco_grd_lat_gss;
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
      if(     (float)lon_ctr_out[0] ==    0.0f && (float)lon_ctr_out[1] == (float)(lon_ctr_out[0]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_Grn_ctr;
      else if((float)lon_ctr_out[0] == -180.0f && (float)lon_ctr_out[1] == (float)(lon_ctr_out[0]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_180_ctr;
      else if((float)lon_ntf_out[0] ==    0.0f && (float)lon_ntf_out[1] == (float)(lon_ntf_out[0]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_Grn_wst;
      else if((float)lon_ntf_out[0] == -180.0f && (float)lon_ntf_out[1] == (float)(lon_ntf_out[0]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_180_wst;
      else if((float)lon_ctr_out[1] == (float)(lon_ctr_out[0]+lon_spn/lon_nbr_out)) nco_grd_lon_typ=nco_grd_lon_bb;
      else nco_grd_lon_typ=nco_grd_lon_unk;
    } /* !nco_grd_lon_typ */

    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s diagnosed output latitude grid-type: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_lat_sng(nco_grd_lat_typ));
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s diagnosed output longitude grid-type: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_lon_sng(nco_grd_lon_typ));
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s diagnosed output grid-extent: %s\n",nco_prg_nm_get(),fnc_nm,nco_grd_xtn_sng(nco_grd_xtn));
    
    if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
      slat_ctr_out=(double *)nco_malloc(slat_nbr_out*nco_typ_lng(crd_typ_out));
      slat_wgt_out=(double *)nco_malloc(slat_nbr_out*nco_typ_lng(crd_typ_out));
      slon_ctr_out=(double *)nco_malloc(slon_nbr_out*nco_typ_lng(crd_typ_out));
      for(idx=0;idx<slat_nbr_out;idx++){
	slat_ctr_out[idx]=lat_ntf_out[idx+1];
	slat_wgt_out[idx]=sin(dgr2rdn*lat_ctr_out[idx+1])-sin(dgr2rdn*lat_ctr_out[idx]);
      } /* !lat_nbr_out */
      for(idx=0;idx<slon_nbr_out;idx++){
	slon_ctr_out[idx]=lon_ntf_out[idx];
      } /* !lat_nbr_out */
    } /* !nco_grd_lat_fv */

    switch(nco_grd_lat_typ){
    case nco_grd_lat_eqa:
    case nco_grd_lat_fv:
      for(idx=0;idx<lat_nbr_out;idx++) lat_wgt_out[idx]=sin(dgr2rdn*lat_bnd_out[2*idx+1])-sin(dgr2rdn*lat_bnd_out[2*idx]);
      break;
    case nco_grd_lat_gss:
      for(idx=0;idx<lat_nbr_out;idx++) lat_wgt_out[idx]=wgt_Gss_out[idx];
      if(wgt_Gss_out) wgt_Gss_out=(double *)nco_free(wgt_Gss_out);
      break;
    case nco_grd_lat_unk:
      for(idx=0;idx<lat_nbr_out;idx++) lat_wgt_out[idx]=0.0;
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING %s reports unknown output latitude grid-type. Unable to guess what latitude weights should be.\n",nco_prg_nm_get(),fnc_nm);
      break;
    default:
      nco_dfl_case_generic_err(); break;
    } /* end nco_grd_lat_typ switch */
    
    /* Fuzzy test of latitude weight normalization */
    double lat_wgt_ttl_xpc; /* [frc] Expected sum of latitude weights */
    lat_wgt_ttl=0.0;
    for(idx=0;idx<lat_nbr_out;idx++) lat_wgt_ttl+=lat_wgt_out[idx];
    lat_wgt_ttl_xpc=sin(dgr2rdn*lat_bnd_out[2*(lat_nbr_out-1)+1])-sin(dgr2rdn*lat_bnd_out[0]);
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
      nco_sph_plg_area(lat_bnd_out,lon_bnd_out,col_nbr_out,bnd_nbr_out,area_out);
    } /* !1D */
    if(flg_grd_out_crv){
      if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"INFO: Diagnosing area_out for curvilinear grid\n");
      /* Area of curvilinear grids requires spherical trigonometry */
      nco_sph_plg_area(lat_crn_out,lon_crn_out,grd_sz_out,bnd_nbr_out,area_out);
    } /* !flg_grd_out_crv */
    if(flg_grd_out_rct && nco_grd_2D_typ != nco_grd_2D_unk){
      /* Mr. Enenstein and George O. Abell taught me the area of spherical zones
	 Spherical zone area is exact and faithful to underlying rectangular equi-angular grid
	 However, ESMF and Tempest approximate spherical polygons as connected by great circle arcs
	 fxm: Distinguish spherical zone shapes (e.g., equi-angular) from great circle arcs (e.g., unstructured polygons) */
      for(lat_idx=0;lat_idx<lat_nbr_out;lat_idx++)
	for(lon_idx=0;lon_idx<lon_nbr_out;lon_idx++)
	  area_out[lat_idx*lon_nbr_out+lon_idx]=dgr2rdn*(lon_bnd_out[2*lon_idx+1]-lon_bnd_out[2*lon_idx])*(sin(dgr2rdn*lat_bnd_out[2*lat_idx+1])-sin(dgr2rdn*lat_bnd_out[2*lat_idx]));
    } /* !spherical zones */
  } /* !flg_dgn_area_out */

  if(rgr->tst == -1){
    /* Passing --rgr tst=-1 causes regridder to fail here 
       This failure should cause host climo script to abort */
    (void)fprintf(stdout,"%s: ERROR %s reports regridder instructed to fail here. This tests failure mode in climo scripts...\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !tst */

  /* Verify frc_out is sometimes non-zero
     ESMF: "The grid frac arrays (frac_a and frac_b) are calculated by ESMF_RegridWeightGen. For conservative remapping, the grid frac array returns the area fraction of the grid cell which participates in the remap- ping. For bilinear and patch remapping, the destination grid frac array [frac_b] is one where the grid point participates in the remapping and zero otherwise. For bilinear and patch remapping, the source grid frac array is always set to zero."
     SCRIP: Similar to ESMF
     For both ESMF+SCRIP frac_[ab] are computed by the weight-generation algorithm and are not specified as part of the input grids
     How does an input ocean grid indicate that, say, half the gridcell is land and half ocean?
     Does it use the area variable to tell the weight generation algorithm that a gridcell is fractional?
     In other words does it use grid_imask=1 and grid_area=0.5*full_gridcell_area and, e.g., T=273.0? */
  for(idx=0;idx<(long)grd_sz_out;idx++)
    if(frc_out[idx] != 0.0) break;
  if(idx == (long)grd_sz_out){
    (void)fprintf(stdout,"%s: ERROR %s reports frc_out contains all zeros\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !always zero */
  /* Test whether frc_out is ever zero... */
  for(idx=0;idx<(long)grd_sz_out;idx++)
    if(frc_out[idx] == 0.0) break;
  if(nco_dbg_lvl_get() >= nco_dbg_std)
    if(idx != (long)grd_sz_out)
      (void)fprintf(stdout,"%s: INFO %s reports frc_out contains zero-elements (e.g., at 1D idx=%ld)\n",nco_prg_nm_get(),fnc_nm,idx);
  /* Normalizing by frc_out is redundant iff frc_out == 1.0, so we can save time without sacrificing accuracy
     However, frc_out is often (e.g., for CS <-> RLL maps) close but not equal to unity (an ESMF_RegridWeightGen issue?)
     Hence, decide whether to normalize by frc_out by diagnosing the furthest excursion of frc_out from unity */
  nco_bool flg_frc_out_one=True;
  nco_bool flg_frc_out_wrt=False;
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
  nco_bool flg_frc_nrm=False; /* [flg] Must normalize by frc_out because frc_out is not always unity and specified normalization is destarea or none */
  if(!flg_frc_out_one && nco_rgr_mth_typ == nco_rgr_mth_conservative && (nco_rgr_nrm_typ == nco_rgr_nrm_destarea || nco_rgr_nrm_typ == nco_rgr_nrm_none)){
    flg_frc_nrm=True;
    /* Avoid writing frc_out unless discrepancies are particularly egregious
       Otherwise would frc_out for standard remaps like ne30->fv129x256 for which eps=2.46e-13 */
    double eps_rlt_wrt_thr=3.0e-13;
    if(frc_out_dff_one_max > eps_rlt_wrt_thr) flg_frc_out_wrt=True;
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO %s reports global metadata specifies conservative remapping with normalization of type = %s. Furthermore, destination fractions frc_dst = dst_frac = frac_b = frc_out contain non-unity elements (maximum deviation from unity of %g exceeds hard-coded (in variable eps_rlt) relative-epsilon threshold of %g for frc_out[%ld] = %g). Thus normalization issues will be explicitly treated. Will apply \'destarea\' normalization (i.e., divide by non-zero frc_out[dst_idx]) to all regridded arrays.\n",nco_prg_nm_get(),fnc_nm,nco_rgr_nrm_sng(nco_rgr_nrm_typ),frc_out_dff_one_max,eps_rlt,idx_max_dvn,frc_out[idx_max_dvn]);
    if(nco_dbg_lvl_get() >= nco_dbg_std && flg_frc_out_wrt) (void)fprintf(stdout,"%s: INFO %s Maximum deviation exceeds threshold of %g that triggers automatic writing of fractional destination area as variable named frac_b in regridded output.\n",nco_prg_nm_get(),fnc_nm,eps_rlt_wrt_thr);
  } /* !sometimes non-unity */
  if(flg_frc_nrm && rgr->flg_rnr){
    (void)fprintf(stdout,"%s: INFO %s reports manual request (with --rnr) to renormalize fields with non-unity frc_dst = dst_frac = frac_b at same time global metadata specifies normalization type = %s. Normalizing twice can be an error, depending on intent of each. Charlie is all ears on how NCO should handle this :)\n",nco_prg_nm_get(),fnc_nm,nco_rgr_nrm_sng(nco_rgr_nrm_typ));
    //nco_exit(EXIT_FAILURE);
  } /* !flg_rnr */

  /* Detailed summary of 2D grids now available including quality-checked coordinates and area */
  if(nco_dbg_lvl_get() >= nco_dbg_sbr){
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
    assert(area_out_ttl <= 4.0*M_PI);
  } /* endif dbg */

  /* Allocate space for and obtain weights and addresses */
  wgt_raw=(double *)nco_malloc_dbg(rgr_map.num_links*nco_typ_lng(NC_DOUBLE),fnc_nm,"Unable to malloc() value buffer for remapping weights");
  col_src_adr=(int *)nco_malloc_dbg(rgr_map.num_links*nco_typ_lng(NC_INT),fnc_nm,"Unable to malloc() value buffer for remapping addresses");
  row_dst_adr=(int *)nco_malloc_dbg(rgr_map.num_links*nco_typ_lng(NC_INT),fnc_nm,"Unable to malloc() value buffer for remapping addresses");

  /* Obtain remap matrix addresses and weights from map file */
  dmn_srt[0]=0L;
  dmn_cnt[0]=rgr_map.num_links;
  rcd=nco_get_vara(in_id,col_src_adr_id,dmn_srt,dmn_cnt,col_src_adr,NC_INT);
  rcd=nco_get_vara(in_id,row_dst_adr_id,dmn_srt,dmn_cnt,row_dst_adr,NC_INT);
  dmn_srt[0]=0L;
  dmn_cnt[0]=rgr_map.num_links;
  if(nco_rgr_mpf_typ != nco_rgr_mpf_SCRIP){
    rcd=nco_get_vara(in_id,wgt_raw_id,dmn_srt,dmn_cnt,wgt_raw,NC_DOUBLE);
  }else if(nco_rgr_mpf_typ == nco_rgr_mpf_SCRIP){
    /* SCRIP mapfiles store 2D weight array remap_matrix[num_links,num_wgts]
       Apply only first weight for first-order conservative accuracy (i.e., area overlap)
       Apply all three weights for second-order conservative accuracy (by including gradients from centroid to vertices) */
    dmn_srd[0]=1L;
    dmn_srt[1]=0L;
    dmn_cnt[1]=1L;
    dmn_srd[1]=rgr_map.num_wgts;
    rcd=nco_get_vars(in_id,wgt_raw_id,dmn_srt,dmn_cnt,dmn_srd,wgt_raw,NC_DOUBLE);
  } /* !SCRIP */

  /* Pre-subtract one from row/column addresses (stored, by convention, as Fortran indices) to optimize access with C indices */
  size_t lnk_nbr; /* [nbr] Number of links */
  size_t lnk_idx; /* [idx] Link index */
  lnk_nbr=rgr_map.num_links;
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
  int dmn_id_col; /* [id] Dimension ID */
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
       ncks -O -D 3 --rgr infer --rgr_var=ALBDO_221_SFC_S113 --rgr grid=~/grd_narr.nc ${DATA}/hdf/narrmon-a_221_20100101_0000_000.nc ~/foo.nc */
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

    rcd=nco_inq_att_flg(in_id,cf->var_id,crd_sng,&att_typ,&att_sz);
    if(rcd == NC_NOERR && att_typ == NC_CHAR){
      cf->crd_sng=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
      rcd+=nco_get_att(in_id,cf->var_id,crd_sng,cf->crd_sng,att_typ);
      /* NUL-terminate attribute before using strstr() */
      cf->crd_sng[att_sz]='\0';
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
	rcd=nco_inq_att_flg(in_id,cf->crd_id[crd_spt],unt_sng,&att_typ,&att_sz);
	if(rcd == NC_NOERR && att_typ == NC_CHAR){
	  cf->unt_sng[crd_spt]=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	  rcd=nco_get_att(in_id,cf->crd_id[crd_spt],unt_sng,cf->unt_sng[crd_spt],att_typ);
	  /* NUL-terminate attribute before using strstr() */
	  *(cf->unt_sng[crd_spt]+att_sz)='\0';
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
      
      rcd=nco_inq_att_flg(in_id,cf->crd_id[0],unt_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	cf->unt_sng[0]=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd=nco_get_att(in_id,cf->crd_id[0],unt_sng,cf->unt_sng[0],att_typ);
	/* NUL-terminate attribute before using strstr() */
	*(cf->unt_sng[0]+att_sz)='\0';
	if(!strcasestr(cf->unt_sng[0],"degrees_")) (void)fprintf(stderr,"%s: WARNING %s reports first coordinates variable %s has weird units attribute = %s. May not detect correct ordering of latitude and longitude coordinates\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0],cf->unt_sng[0]);
      } /* !rcd && att_typ */
      rcd=nco_inq_att_flg(in_id,cf->crd_id[1],unt_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	cf->unt_sng[1]=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd=nco_get_att(in_id,cf->crd_id[1],unt_sng,cf->unt_sng[1],att_typ);
	/* NUL-terminate attribute before using strstr() */
	*(cf->unt_sng[1]+att_sz)='\0';
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
    /* Subtlety: lat_nm_in is coordinate (variable+dimension) name when specified from command-line (as in nco_grd_nfr()), dimension name when found through CF-method (as in nco_rgr_map()). This confusing distinction could be avoided by passing command-line dimension names through-to nco_rgr_map(). However, that route would require complex priorities for what to do when passing command-line coordinate names not dimension names and visa-versa. */
    lat_nm_in=strdup(cf->dmn_nm[0]);
    lon_nm_in=strdup(cf->dmn_nm[1]);
    //lat_nm_in=strdup(cf->crd_nm[idx_lat]);
    //lon_nm_in=strdup(cf->crd_nm[idx_lon]);
    /* Next four lines unnecessary in nco_rgr_map() which only needs dimension names (it reads input coordinates from map- not data-file) */
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
    /* Check default or command-line option first, then search usual suspects */
    if(col_nm_in && (rcd=nco_inq_dimid_flg(in_id,col_nm_in,&dmn_id_col)) == NC_NOERR) /* do nothing */; 
    else if((rcd=nco_inq_dimid_flg(in_id,"lndgrid",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("lndgrid"); /* CLM */
    else if((rcd=nco_inq_dimid_flg(in_id,"nCells",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("nCells"); /* MPAS-O/I */
    else if((rcd=nco_inq_dimid_flg(in_id,"nEdges",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("nEdges"); /* MPAS-O/I */
    else if((rcd=nco_inq_dimid_flg(in_id,"sounding_id",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("sounding_id"); /* OCO2 */
    else{
      (void)fprintf(stdout,"%s: ERROR %s reports datafile does not contain unstructured dimension name that matches a usual suspect (ncol, lndgrid, nCells, nEdges, sounding_id). HINT: Provide horizontal dimension name with \"-r col_nm=foo\"\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    } /* !col_nm_in */
    rcd=nco_inq_dimlen(in_id,dmn_id_col,&col_nbr_in_dat);
    if(col_nbr_in != col_nbr_in_dat){
      (void)fprintf(stdout,"%s: ERROR %s reports mapfile and data file dimension sizes disagree: mapfile col_nbr_in = %ld != %ld = col_nbr_in from datafile. HINT: Check that source grid (i.e., \"grid A\") used to create mapfile matches grid on which data are stored in input datafile.\n",nco_prg_nm_get(),fnc_nm,col_nbr_in,col_nbr_in_dat);
      nco_exit(EXIT_FAILURE);
    } /* !err */
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
    else if((rcd=nco_inq_dimid_flg(in_id,"nscan",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("nscan"); /* AMSR, TRMM */
    else if((rcd=nco_inq_dimid_flg(in_id,"nTimes",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("nTimes"); /* OMI L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"GeoTrack",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("GeoTrack"); /* AIRS L2 DAP NC */
    else if((rcd=nco_inq_dimid_flg(in_id,"GeoTrack:L2_Standard_atmospheric&surface_product",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("GeoTrack:L2_Standard_atmospheric&surface_product"); /* AIRS L2 HDF */
    else if((rcd=nco_inq_dimid_flg(in_id,"Cell_Along_Swath:mod04",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("Cell_Along_Swath:mod04"); /* MODIS MOD04 L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"Cell_Along_Swath_mod04",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("Cell_Along_Swath_mod04"); /* MODIS MOD04 L2 (ncl_convert2nc changes colon to underscore) */
    else if((rcd=nco_inq_dimid_flg(in_id,"CO_Latitude",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("CO_Latitude");
    else if((rcd=nco_inq_dimid_flg(in_id,"j",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("j"); /* CMIP5 NorESM1 ocean */
    else if((rcd=nco_inq_dimid_flg(in_id,"latitude0",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("latitude0"); /* Oxford */
    else if((rcd=nco_inq_dimid_flg(in_id,"y",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("y"); /* NEMO */
    else if((rcd=nco_inq_dimid_flg(in_id,"x",&dmn_id_lat)) == NC_NOERR) lat_nm_in=strdup("x"); /* NSIDC polar stereographic (NB: unfortunate incompatible conflict between NEMO & NSIDC names) */
    else{
      (void)fprintf(stdout,"%s: ERROR %s reports unable to find latitude dimension in input file. Tried the usual suspects. HINT: Inform regridder of input latitude dimension name with --rgr lat_nm_in=name\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    } /* !lat */
    rcd=nco_inq_dimlen(in_id,dmn_id_lat,&lat_nbr_in_dat);
    if(lat_nbr_in != lat_nbr_in_dat){
      (void)fprintf(stdout,"%s: ERROR %s reports mapfile and data file dimension sizes disagree: mapfile lat_nbr_in = %ld != %ld = lat_nbr_in from datafile. HINT: Check that source grid (i.e., \"grid A\") used to create mapfile matches grid on which data are stored in input datafile.\n",nco_prg_nm_get(),fnc_nm,lat_nbr_in,lat_nbr_in_dat);
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
    else if((rcd=nco_inq_dimid_flg(in_id,"npix",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("npix"); /* AMSR */
    else if((rcd=nco_inq_dimid_flg(in_id,"npixel",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("npixel"); /* TRMM */
    else if((rcd=nco_inq_dimid_flg(in_id,"nxtrack",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("nxtrack"); /* MODIS DeepBlue SeaWiFS L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"nXtrack",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("nXtrack"); /* OMI L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"GeoXTrack",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("GeoXTrack"); /* AIRS L2 DAP NC */
    else if((rcd=nco_inq_dimid_flg(in_id,"GeoXTrack:L2_Standard_atmospheric&surface_product",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("GeoXTrack:L2_Standard_atmospheric&surface_product"); /* AIRS L2 HDF */
    else if((rcd=nco_inq_dimid_flg(in_id,"Cell_Across_Swath:mod04",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("Cell_Across_Swath:mod04"); /* MODIS MOD04 L2 */
    else if((rcd=nco_inq_dimid_flg(in_id,"Cell_Across_Swath_mod04",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("Cell_Across_Swath_mod04"); /* MODIS MOD04 L2 (ncl_convert2nc changes colon to underscore) */
    else if((rcd=nco_inq_dimid_flg(in_id,"i",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("i"); /* CMIP5 NorESM1 ocean */
    else if((rcd=nco_inq_dimid_flg(in_id,"longitude0",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("longitude0"); /* Oxford */
    else if((rcd=nco_inq_dimid_flg(in_id,"x",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("x"); /* NEMO */
    else if((rcd=nco_inq_dimid_flg(in_id,"y",&dmn_id_lon)) == NC_NOERR) lon_nm_in=strdup("y"); /* NSIDC polar stereographic (NB: unfortunate incompatible conflict between NEMO & NSIDC names) */
    else{
      (void)fprintf(stdout,"%s: ERROR %s reports unable to find longitude dimension in input file. Tried the usual suspects. HINT: Inform regridder of input longitude dimension name with --rgr lon_nm_in=name\n",nco_prg_nm_get(),fnc_nm);
      nco_exit(EXIT_FAILURE);
    } /* !lat */
    rcd=nco_inq_dimlen(in_id,dmn_id_lon,&lon_nbr_in_dat);
    if(lon_nbr_in != lon_nbr_in_dat){
      (void)fprintf(stdout,"%s: ERROR %s reports mapfile and data file dimension sizes disagree: mapfile lon_nbr_in = %ld != %ld = lon_nbr_in from datafile. HINT: Check that source grid (i.e., \"grid A\") used to create mapfile matches grid on which data are stored in input datafile.\n",nco_prg_nm_get(),fnc_nm,lon_nbr_in,lon_nbr_in_dat);
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
     CAM-SE: area
     CICE: latt_bounds, lont_bounds, latu_bounds, lonu_bounds, TLAT, TLON, ULAT, ULON (NB: CICE uses ?LON and POP uses ?LONG) (aice is ice area, tmask is state-variable mask, both not currently excluded, although all binary masks like tmask should be recomputed on new grid)
     ESMF: gridcell_area
     GPM: S1_Latitude, S1_Longitude
     HIRDLS: Latitude
     MAR/RACMO: LAT, LON
     MLS: CO_Latitude
     MPAS-O/I: areaCell, latCell, lonCell
     NCO: lat_vertices, lon_vertices
     NEMO: nav_lat, nav_lon
     OCO2: latitude_bnds, longitude_bnds
     OMI DOMINO: Latitude, LatitudeCornerpoints, Longitude, LongitudeCornerpoints
     Oxford: global_latitude0, global_longitude0, latitude0, longitude0
     POP: TLAT, TLONG, ULAT, ULONG  (NB: CICE uses ?LON and POP uses ?LONG) (POP does not archive spatial bounds)
     TRMM: Latitude, Longitude
     UV-CDAT regridder: bounds_lat, bounds_lon
     Unknown: XLAT_M, XLONG_M
     WRF: XLAT, XLONG */
  const int var_xcl_lst_nbr=50; /* [nbr] Number of objects on exclusion list */
  const char *var_xcl_lst[]={"/area","/areaCell","/gridcell_area","/gw","/LAT","/lat","/latCell","/Latitude","/latitude","/nav_lat","/global_latitude0","/latitude0","/slat","/TLAT","/ULAT","/XLAT","/XLAT_M","/CO_Latitude","/S1_Latitude","/lat_bnds","/lat_vertices","/latt_bounds","/latu_bounds","/latitude_bnds","/LatitudeCornerpoints","/bounds_lat","/LON","/lon","/lonCell","/Longitude","/longitude","/nav_lon","/global_longitude0","/longitude0","/slon","/TLON","/TLONG","/ULON","/ULONG","/XLONG","/XLONG_M","/CO_Longitude","/S1_Longitude","/lon_bnds","/lon_vertices","/lont_bounds","/lonu_bounds","/longitude_bnds","/LongitudeCornerpoints","/bounds_lon","/w_stag"};
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

  if(False){
    /* 20160228: MPAS has a host of mysterious grid and extensive variables that should probably not be regridded */
    const int mpas_xcl_lst_nbr=18;
    const char *mpas_xcl_lst[]={"cellMask,cellsOnCell,cellsOnEdge,cellsOnVertex,edgeMask,edgesOnCell,edgesOnEdge,edgesOnVertex,indexToCellID,indexToEdgeID,indexToVertexID,maxLevelCell,maxLevelEdgeTop,nEdgesOnCell,nEdgesOnEdge,vertexMask,verticesOnCell,verticesOnEdge"};
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
  } /* !False */
  
  char *dmn_nm_cp; /* [sng] Dimension name as char * to reduce indirection */
  int dmn_idx; /* [idx] Dimension index */
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
	  /* fxm: generalize to include any variable containing two coordinates with "standard_name" = "latitude" and "longitude" */
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
  if(!var_rgr_nbr) (void)fprintf(stdout,"%s: WARNING %s reports no variables fit regridding criteria. The regridder expects something to regrid, and variables not regridded are copied straight to output. HINT: If the name(s) of the input horizontal spatial dimensions to be regridded (e.g., latitude and longitude or column) do not match NCO's preset defaults (case-insensitive unambiguous forms and abbreviations of \"latitude\", \"longitude\", and \"ncol\", respectively) then change the dimension names that NCO looks for. Instructions are at http://nco.sf.net/nco.html#regrid, e.g., \"ncks --rgr col=lndgrid --rgr lat=north ...\".\n",nco_prg_nm_get(),fnc_nm);
  
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
  char *lat_nm_out;
  char *lat_wgt_nm;
  char *lon_bnd_nm_out;
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
      (void)fprintf(stdout,"%s: INFO %s reports spatial and temporal output bounds dimensions are identical (and named \"%s\") by default for rectangular output grids because both can be stored as 2D arrays. That cannot work for this mapping because temporal and spatial bounds dimensions sizes differ (bnd_nbr_out = %d, bnd_tm_nbr_out = %d). Using fall-back spatial bounds name \"%s\" instead. HINT: You may change one or both manually with --rgr bnd_nm=name or --rgr bnd_tm_nm=name.\n",nco_prg_nm_get(),fnc_nm,bnd_tm_nm_out,bnd_nbr_out,bnd_tm_nbr_out,bnd_nm_out);
    } /* !strcmp() */
  } /* !bnd_nbr_out */

  /* Persistent metadata */
  aed_sct aed_mtd_crd;
  char *att_val_crd=NULL;
  char *att_nm_crd=NULL;
  att_nm_crd=strdup("coordinates");
  aed_mtd_crd.att_nm=att_nm_crd;
  if(flg_grd_out_1D) aed_mtd_crd.mode=aed_overwrite; else aed_mtd_crd.mode=aed_delete;
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
    rcd+=nco_def_dim(out_id,lat_nm_out,lat_nbr_out,&dmn_id_lat);
    rcd+=nco_def_dim(out_id,lon_nm_out,lon_nbr_out,&dmn_id_lon);
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

  /* Define new coordinates and variables in regridded file */
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
  //int dmn_idx_in_out_pre_rgr[dmn_nbr_max];
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
	    //dmn_idx_in_out_pre_rgr[dmn_idx]=dmn_idx;
	    /* Is horizontal dimension last, i.e., most-rapidly-varying? */
	    if(flg_grd_in_1D && !strcmp(dmn_nm,col_nm_in)){
	      if(dmn_idx != dmn_nbr_in-1){
		/* Unstructured input grid has col in non-MRV location (expect this with, e.g., MPAS-O/I native grid dimension-ordering */
		(void)fprintf(stdout,"%s: WARNING %s reports unstructured grid spatial coordinate %s is (zero-based) dimension %d of input variable to be regridded %s which has %d dimensions. NCO regridder has only experimental support for unstructured spatial dimensions that are not the last dimension of input variable.\nHINT: Consider re-arranging input file dimensions to place horizontal dimension(s) last with, e.g., \'ncpdq -a time,lev,%s in.nc out.nc\' prior to calling regridder\n",nco_prg_nm_get(),fnc_nm,dmn_nm,dmn_idx,var_nm,dmn_nbr_in,dmn_nm);
		trv_tbl->lst[idx_tbl].flg_mrv=False;
	      } /* !dmn_idx */
	    } /* !flg_grd_in_1D */
	    if(flg_grd_in_2D && (!strcmp(dmn_nm,lat_nm_in) || !strcmp(dmn_nm,lon_nm_in))){
	      /* Are horizontal dimensions most-rapidly-varying? */
	      if(dmn_idx != dmn_nbr_in-1 && dmn_idx != dmn_nbr_in-2){
		/* NB: Lat/lon input grid has lat/lon in non-MRV location (expect this with, e.g., AIRS L2 grid dimension-ordering */
		(void)fprintf(stdout,"%s: WARNING %s reports lat-lon grid spatial coordinate %s is (zero-based) dimension %d of input variable to be regridded %s which has %d dimensions. NCO regridder has only experimental support for lat-lon dimension(s) that are not last two dimensions of input variable.\nHINT: Consider re-arranging input file dimensions to place horizontal dimensions last with, e.g., \'ncpdq -a time,lev,lat,lon in.nc out.nc\' prior to calling regridder\n",nco_prg_nm_get(),fnc_nm,dmn_nm,dmn_idx,var_nm,dmn_nbr_in);
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
		if(!strcmp(dmn_nm,lat_nm_in)) rcd=nco_inq_dimid_flg(out_id,lat_nm_out,dmn_id_out+dmn_idx);
		else if(!strcmp(dmn_nm,lon_nm_in)) rcd=nco_inq_dimid_flg(out_id,lon_nm_out,dmn_id_out+dmn_idx);
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
    att_nm=strdup("long_name");
    att_val=strdup("Solid angle subtended by gridcell");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=area_nm_out;
    aed_mtd.id=area_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,area_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("standard_name");
    att_val=strdup("area");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=area_nm_out;
    aed_mtd.id=area_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,area_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("units");
    att_val=strdup("steradian");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=area_nm_out;
    aed_mtd.id=area_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,area_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("cell_methods");
    att_val=(char *)nco_calloc((strlen(lat_nm_out)+strlen(lon_nm_out)+8L),sizeof(char));
    (void)sprintf(att_val,"%s, %s: sum",lat_nm_out,lon_nm_out);
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=area_nm_out;
    aed_mtd.id=area_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,area_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* !flg_area_out */

  if(flg_frc_out_wrt){
    att_nm=strdup("long_name");
    att_val=strdup("Fraction of gridcell valid on destination grid");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=frc_nm_out;
    aed_mtd.id=frc_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,frc_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("cell_methods");
    att_val=(char *)nco_calloc((strlen(lat_nm_out)+strlen(lon_nm_out)+8L),sizeof(char));
    (void)sprintf(att_val,"%s, %s: sum",lat_nm_out,lon_nm_out);
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=frc_nm_out;
    aed_mtd.id=frc_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,frc_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* !flg_frc_out_wrt */
  
  if(flg_msk_out){
    att_nm=strdup("long_name");
    att_val=strdup("Mask (0 = invalid destination, 1 = valid destination)");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msk_nm_out;
    aed_mtd.id=msk_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msk_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* !flg_msk_out */

  att_nm=strdup("long_name");
  att_val=strdup("Latitude");
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

  att_nm=strdup("units");
  att_val=strdup("degrees_north");
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

  att_nm=strdup("axis");
  att_val=strdup("Y");
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

  double vld_min;
  double vld_max;
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
  
  att_nm=strdup("bounds");
  att_val=lat_bnd_nm_out;
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lat_nm_out;
  aed_mtd.id=lat_out_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lat_out_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);

  att_nm=strdup("long_name");
  if(flg_grd_out_rct) att_val=strdup("Gridcell latitude interfaces"); else att_val=strdup("Gridcell latitude vertices");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lat_bnd_nm_out;
  aed_mtd.id=lat_bnd_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lat_bnd_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  att_nm=strdup("long_name");
  att_val=strdup("Longitude");
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

  att_nm=strdup("standard_name");
  att_val=strdup("longitude");
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

  att_nm=strdup("units");
  att_val=strdup("degrees_east");
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

  att_nm=strdup("axis");
  att_val=strdup("X");
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
    
    att_nm=strdup("topology");
    att_val=strdup("circular");
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

  att_nm=strdup("long_name");
  if(flg_grd_out_rct) att_val=strdup("Gridcell longitude interfaces"); else att_val=strdup("Gridcell longitude vertices");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=lon_bnd_nm_out;
  aed_mtd.id=lon_bnd_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,lon_bnd_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  if(nco_grd_lat_typ == nco_grd_lat_fv && flg_stg){
    att_nm=strdup("long_name");
    att_val=strdup("Latitude for staggered FV grid");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=slat_nm_out;
    aed_mtd.id=slat_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,slat_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("units");
    att_val=strdup("degrees_north");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=slat_nm_out;
    aed_mtd.id=slat_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,slat_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Latitude weights for staggered FV grid");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=slat_wgt_nm_out;
    aed_mtd.id=slat_wgt_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,slat_wgt_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Longitude for staggered FV grid");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=slon_nm_out;
    aed_mtd.id=slon_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,slon_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("units");
    att_val=strdup("degrees_east");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=slon_nm_out;
    aed_mtd.id=slon_out_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,slon_out_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* !nco_grd_lat_fv */

  if(flg_grd_out_rct){
    att_nm=strdup("long_name");
    att_val=strdup("Latitude quadrature weights (normalized to sum to 2.0 on global grids)");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lat_wgt_nm;
    aed_mtd.id=lat_wgt_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lat_wgt_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
  } /* !flg_grd_out_rct */
  
  att_nm=strdup("map_file");
  att_val=strdup(fl_in);
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

  att_nm=strdup("input_file");
  att_val=strdup(rgr->fl_in);
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

  /* Annotate persistent metadata that should appear last in attribute list */
  if(flg_grd_out_1D){
    if(flg_area_out){
      aed_mtd_crd.var_nm=area_nm_out;
      aed_mtd_crd.id=area_out_id;
      (void)nco_aed_prc(out_id,area_out_id,aed_mtd_crd);
    } /* !flg_area_out */
    
    if(flg_frc_out_wrt){
      aed_mtd_crd.var_nm=frc_nm_out;
      aed_mtd_crd.id=frc_out_id;
      (void)nco_aed_prc(out_id,frc_out_id,aed_mtd_crd);
    } /* !flg_frc_out_wrt */

    if(flg_msk_out){
      aed_mtd_crd.var_nm=msk_nm_out;
      aed_mtd_crd.id=msk_out_id;
      (void)nco_aed_prc(out_id,msk_out_id,aed_mtd_crd);
    } /* !flg_msk_out */

    aed_mtd_crd.var_nm=lat_nm_out;
    aed_mtd_crd.id=lat_out_id;
    (void)nco_aed_prc(out_id,lat_out_id,aed_mtd_crd);
    
    aed_mtd_crd.var_nm=lon_nm_out;
    aed_mtd_crd.id=lon_out_id;
    (void)nco_aed_prc(out_id,lon_out_id,aed_mtd_crd);
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
  double *var_val_dbl_in=NULL;
  double *var_val_dbl_out=NULL;
  double *wgt_vld_out=NULL;
  double var_val_crr;
  int *tally=NULL; /* [nbr] Number of valid (non-missing) values */
  size_t idx_in; /* [idx] Input grid index */
  size_t idx_out; /* [idx] Output grid index */
  int lvl_idx; /* [idx] Level index */
  int lvl_nbr; /* [nbr] Number of levels */
  int thr_idx; /* [idx] Thread index */
  size_t dst_idx; 
  size_t var_sz_in; /* [nbr] Number of elements in variable (will be self-multiplied) */
  size_t var_sz_out; /* [nbr] Number of elements in variable (will be self-multiplied) */
  size_t val_in_fst; /* [nbr] Number of elements by which current N-D slab input values are offset from origin */
  size_t val_out_fst; /* [nbr] Number of elements by which current N-D slab output values are offset from origin */
  
  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"Regridding progress: # means regridded, ~ means copied\n");

  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped shared in parallel clause */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  /* OpenMP notes:
     default(): None
     firstprivate(): Pointers that could be inadvertently free()'d if they lost their NULL-initialization
     private(): Almost everything else
     shared(): uggh...shared clause depends on both compiler and compiler-version
     1. All const variables are default shared for gcc >= 4.9.2,
     2. fnc_nm (only!) must be explicit shared for g++ 4.6.3 (travis)
     3. flg_rnr,fnc_nm,wgt_vld_thr must be explicit shared for icc 13.1.3 (rhea) */
#ifdef __GNUG__
# define GCC_LIB_VERSION ( __GNUC__ * 100 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__ )
# if GCC_LIB_VERSION < 480
#  define GXX_OLD_OPENMP_SHARED_TREATMENT 1
# endif /* 480 */
#endif /* !__GNUC__ */
#if defined( __INTEL_COMPILER)
# pragma omp parallel for default(none) firstprivate(dmn_cnt_in,dmn_cnt_out,dmn_srt,dmn_id_in,dmn_id_out,tally,var_val_dbl_in,var_val_dbl_out,wgt_vld_out) private(dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dst_idx,has_mss_val,idx,idx_in,idx_out,idx_tbl,in_id,lnk_idx,lvl_idx,lvl_nbr,mss_val_dbl,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr,var_val_crr) shared(col_src_adr,dmn_nbr_hrz_crd,flg_frc_nrm,flg_rnr,fnc_nm,frc_out,lnk_nbr,out_id,row_dst_adr,wgt_raw,wgt_vld_thr)
#else /* !__INTEL_COMPILER */
# ifdef GXX_OLD_OPENMP_SHARED_TREATMENT
#  pragma omp parallel for default(none) firstprivate(dmn_cnt_in,dmn_cnt_out,dmn_srt,dmn_id_in,dmn_id_out,tally,var_val_dbl_in,var_val_dbl_out,wgt_vld_out) private(dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dst_idx,has_mss_val,idx,idx_in,idx_out,idx_tbl,in_id,lnk_idx,lvl_idx,lvl_nbr,mss_val_dbl,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr,var_val_crr) shared(col_src_adr,dmn_nbr_hrz_crd,flg_frc_nrm,fnc_nm,frc_out,lnk_nbr,out_id,row_dst_adr,wgt_raw)
# else /* !old g++ */
#  pragma omp parallel for default(none) firstprivate(dmn_cnt_in,dmn_cnt_out,dmn_srt,dmn_id_in,dmn_id_out,tally,var_val_dbl_in,var_val_dbl_out,wgt_vld_out) private(dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dst_idx,has_mss_val,idx,idx_in,idx_out,idx_tbl,in_id,lnk_idx,lvl_idx,lvl_nbr,mss_val_dbl,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr,var_val_crr) shared(col_src_adr,dmn_nbr_hrz_crd,flg_frc_nrm,frc_out,lnk_nbr,out_id,row_dst_adr,wgt_raw)
# endif /* !old g++ */
#endif /* !__INTEL_COMPILER */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    thr_idx=omp_get_thread_num();
    in_id=trv_tbl->in_id_arr[thr_idx];
#ifdef _OPENMP
    if(nco_dbg_lvl_get() >= nco_dbg_var && !thr_idx && !idx_tbl) (void)fprintf(fp_stdout,"%s: %s reports regrid loop uses %d thread%s\n",nco_prg_nm_get(),fnc_nm,omp_get_num_threads(),(omp_get_num_threads() > 1) ? "s" : "");
    if(nco_dbg_lvl_get() >= nco_dbg_io) (void)fprintf(fp_stdout,"%s: thread = %d, idx_tbl = %d, nm = %s\n",nco_prg_nm_get(),thr_idx,idx_tbl,trv.nm);
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
	rcd=nco_inq_varndims(out_id,var_id_out,&dmn_nbr_out);
	rcd=nco_inq_varndims(in_id,var_id_in,&dmn_nbr_in);
	dmn_nbr_max= dmn_nbr_in > dmn_nbr_out ? dmn_nbr_in : dmn_nbr_out;
	dmn_id_in=(int *)nco_malloc(dmn_nbr_in*sizeof(int));
	dmn_id_out=(int *)nco_malloc(dmn_nbr_out*sizeof(int));
	dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long)); /* max() for both input and output grids */
	dmn_cnt_in=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
	dmn_cnt_out=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
	rcd=nco_inq_vardimid(out_id,var_id_out,dmn_id_out);
	rcd=nco_inq_vardimid(in_id,var_id_in,dmn_id_in);
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	  rcd=nco_inq_dimlen(in_id,dmn_id_in[dmn_idx],dmn_cnt_in+dmn_idx);
	  var_sz_in*=dmn_cnt_in[dmn_idx];
	  dmn_srt[dmn_idx]=0L;
	} /* end loop over dimensions */
	var_val_dbl_in=(double *)nco_malloc_dbg(var_sz_in*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() input value buffer");
	rcd=nco_get_vara(in_id,var_id_in,dmn_srt,dmn_cnt_in,var_val_dbl_in,var_typ_rgr);

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
	} /* end loop over dimensions */
	var_val_dbl_out=(double *)nco_malloc_dbg(var_sz_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() output value buffer");
	
	/* Initialize output */
	for(dst_idx=0;dst_idx<var_sz_out;dst_idx++) var_val_dbl_out[dst_idx]=0.0;
	/* Missing value setup */
	has_mss_val=nco_mss_val_get_dbl(in_id,var_id_in,&mss_val_dbl);
	if(has_mss_val) tally=(int *)nco_calloc(var_sz_out,nco_typ_lng(NC_INT));
	if(has_mss_val && flg_rnr) wgt_vld_out=(double *)nco_malloc_dbg(var_sz_out*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() input weight buffer");
	if(has_mss_val && flg_rnr) 
	  for(dst_idx=0;dst_idx<var_sz_out;dst_idx++) wgt_vld_out[dst_idx]=0.0;

	int dmn_idx_in; /* [idx] Index to input dimensions */
	int dmn_idx_out; /* [idx] Index to output dimensions */
	int *dmn_idx_out_in=NULL; /* [idx] Dimension correspondence, output->input */
	long *dmn_map_in=NULL; /* [idx] Map for each dimension of input variable */
	long *dmn_map_out=NULL; /* [idx] Map for each dimension of output variable */
	long *dmn_sbs_in=NULL; /* [idx] Dimension subscripts into N-D input array */
	long *dmn_sbs_out=NULL; /* [idx] Dimension subscripts into N-D output array */

	const int dmn_nbr_in_m1=dmn_nbr_in-1; /* [nbr] Number of input dimensions less one (fast) */
	const int dmn_nbr_out_m1=dmn_nbr_out-1; /* [nbr] Number of output dimensions less one (fast) */
	char *var_val_cp_in=NULL; /* [] Non-MRV input values permuted into MRV order */
	char *var_val_cp_out=NULL; /* [] Non-MRV output values permuted into MRV order */
	
	if(!trv.flg_mrv){
	  dmn_idx_out_in=(int *)nco_malloc(dmn_nbr_out*sizeof(int));
	  dmn_map_in=(long *)nco_malloc(dmn_nbr_in*sizeof(long));
	  dmn_map_out=(long *)nco_malloc(dmn_nbr_out*sizeof(long));
	  dmn_sbs_in=(long *)nco_malloc(dmn_nbr_in*sizeof(long));
	  dmn_sbs_out=(long *)nco_malloc(dmn_nbr_out*sizeof(long));

	  /* 20151012: Juggle indices to extent possible before main weight loop */
	  for(dmn_idx_out=0;dmn_idx_out<dmn_nbr_out;dmn_idx_out++)
	    dmn_idx_out_in[dmn_idx_out]=-73;
	  
	  /* Map for each dimension of input variable */
	  for(dmn_idx_in=0;dmn_idx_in<dmn_nbr_in;dmn_idx_in++) dmn_map_in[dmn_idx_in]=1L;
	  for(dmn_idx_in=0;dmn_idx_in<dmn_nbr_in-1;dmn_idx_in++)
	    for(dmn_idx=dmn_idx_in+1;dmn_idx<dmn_nbr_in;dmn_idx++)
	      dmn_map_in[dmn_idx_in]*=dmn_cnt_in[dmn_idx];
  
	  /* Map for each dimension of output variable */
	  for(dmn_idx_out=0;dmn_idx_out<dmn_nbr_out;dmn_idx_out++) dmn_map_out[dmn_idx_out]=1L;
	  for(dmn_idx_out=0;dmn_idx_out<dmn_nbr_out-1;dmn_idx_out++)
	    for(dmn_idx=dmn_idx_out+1;dmn_idx<dmn_nbr_out;dmn_idx++)
	      dmn_map_out[dmn_idx_out]*=dmn_cnt_out[dmn_idx];
	} /* !flg_mrv */
	      
	/* Compute number and size of non-lat/lon or non-col dimensions (e.g., level, time, species, wavelength)
	   Denote their convolution by level or 'lvl' for shorthand
	   There are lvl_nbr elements for each lat/lon or col position
	   20151011: Until today assume lat/lon and col are most-rapidly varying dimensions 
	   20151011: Until today lvl_nbr missed last non-spatial dimension for 1D output */
	lvl_nbr=1;
	/* Simple prescription of lvl_nbr works when horizontal dimension(s) is/are MRV */
	for(dmn_idx=0;dmn_idx<dmn_nbr_out-dmn_nbr_hrz_crd;dmn_idx++) lvl_nbr*=dmn_cnt_out[dmn_idx];
	if(!trv.flg_mrv){
	  /* fxm: 20151011 generalize for non-MRV input */
	  for(dmn_idx=0;dmn_idx<dmn_nbr_out-dmn_nbr_hrz_crd;dmn_idx++) lvl_nbr*=dmn_cnt_out[dmn_idx];
	} /* !flg_mrv */
	
	if(!trv.flg_mrv){
	  /* Nomenclature for var_val_cp buffers is confusing because _in and _out both refer to pre- and post-permutation */
	  var_val_cp_in=(char *)var_val_dbl_in;
	  var_val_cp_out=(char *)nco_malloc_dbg(var_sz_in*nco_typ_lng(var_typ_rgr),fnc_nm,"Unable to malloc() permuted output value buffer");
	
	  for(idx_in=0;idx_in<var_sz_in;idx_in++){

	    /* fxm CEWI fixes uninitialized warning */
	    idx_out=73;
	    
	    /* dmn_sbs_in are corresponding indices (subscripts) into N-D array */
	    dmn_sbs_in[dmn_nbr_in_m1]=idx_in%dmn_cnt_in[dmn_nbr_in_m1];
	    for(dmn_idx_in=0;dmn_idx_in<dmn_nbr_in_m1;dmn_idx_in++){
	      dmn_sbs_in[dmn_idx_in]=(long int)(idx_in/dmn_map_in[dmn_idx_in]);
	      dmn_sbs_in[dmn_idx_in]%=dmn_cnt_in[dmn_idx_in];
	    } /* end loop over dimensions */
	    
	    /* dmn_sbs_out are corresponding indices (subscripts) into N-D array */
	    dmn_sbs_out[dmn_nbr_out_m1]=idx_out%dmn_cnt_out[dmn_nbr_out_m1];
	    for(dmn_idx_out=0;dmn_idx_out<dmn_nbr_out_m1;dmn_idx_out++){
	      dmn_sbs_out[dmn_idx_out]=(long int)(idx_out/dmn_map_out[dmn_idx_out]);
	      dmn_sbs_out[dmn_idx_out]%=dmn_cnt_out[dmn_idx_out];
	    } /* end loop over dimensions */
	    
	    /* Map variable's N-D array indices to get 1-D index into output data */
	    idx_out=0L;
	    for(dmn_idx_out=0;dmn_idx_out<dmn_nbr_out;dmn_idx_out++) 
	      // fxm
	      idx_out+=0*(dmn_sbs_in[dmn_idx_out_in[dmn_idx_out]]*dmn_map_out[dmn_idx_out]);
	    
	    /* Copy current input element into its slot in output array */
	    (void)memcpy(var_val_cp_out+idx_out*sizeof(double),var_val_cp_in+idx_in*sizeof(double),(size_t)sizeof(double));
	  } /* end loop over idx_in */

	  /* Free non-MRV input buffer */
	  if(var_val_dbl_in) var_val_dbl_in=(double *)nco_free(var_val_dbl_in);
	  /* Point input buffer to MRV var_val_cp_out, then regrid that */
	  var_val_dbl_in=(double *)var_val_cp_out;

	  if(dmn_idx_out_in) dmn_idx_out_in=(int *)nco_free(dmn_idx_out_in);
	  if(dmn_map_in) dmn_map_in=(long *)nco_free(dmn_map_in);
	  if(dmn_map_out) dmn_map_out=(long *)nco_free(dmn_map_out);
	  if(dmn_sbs_in) dmn_sbs_in=(long *)nco_free(dmn_sbs_in);
	  if(dmn_sbs_out) dmn_sbs_out=(long *)nco_free(dmn_sbs_out);

	} /* !flg_mrv */
	
	/* 20150914: Intensive variables require normalization, extensive do not
	   Intensive variables (temperature, wind speed, mixing ratio) do not depend on gridcell boundaries
	   Extensive variables (population, counts, numbers of things) depend on gridcell boundaries
	   Extensive variables are the exception in models, yet are commonly used for sampling information, e.g., 
	   number of photons, number of overpasses 
	   Pass extensive variable list to NCO with, e.g., --xtn=TSurfStd_ct,... */
	  
	/* Apply weights */
	if(!has_mss_val){
	  if(lvl_nbr == 1){
	    for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
	      var_val_dbl_out[row_dst_adr[lnk_idx]]+=var_val_dbl_in[col_src_adr[lnk_idx]]*wgt_raw[lnk_idx];
	  }else{
	    val_in_fst=0L;
	    val_out_fst=0L;
	    for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
	      //if(nco_dbg_lvl_get() >= nco_dbg_crr) (void)fprintf(fp_stdout,"%s lvl_idx = %d val_in_fst = %li, val_out_fst = %li\n",trv.nm,lvl_idx,val_in_fst,val_out_fst);
	      for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++)
		var_val_dbl_out[row_dst_adr[lnk_idx]+val_out_fst]+=var_val_dbl_in[col_src_adr[lnk_idx]+val_in_fst]*wgt_raw[lnk_idx];
	      val_in_fst+=grd_sz_in;
	      val_out_fst+=grd_sz_out;
	    } /* end loop over lvl */
	  } /* lvl_nbr > 1 */
	}else{ /* has_mss_val */
	  if(lvl_nbr == 1){
	    if(trv.flg_xtn){
	      /* 20150914: fxm extensive block needs work and once debugged, must be implemented in !has_mss_val branch and in lvl_nbr > 1 branch */
	      for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		idx_in=col_src_adr[lnk_idx];
		idx_out=row_dst_adr[lnk_idx];
		if((var_val_crr=var_val_dbl_in[idx_in]) != mss_val_dbl){
		  var_val_dbl_out[idx_out]+=var_val_crr;
		  if(flg_rnr) wgt_vld_out[idx_out]+=wgt_raw[lnk_idx];
		  tally[idx_out]++;
		} /* endif */
	      } /* end loop over link */
	    }else{
	      for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		idx_in=col_src_adr[lnk_idx];
		idx_out=row_dst_adr[lnk_idx];
		if((var_val_crr=var_val_dbl_in[idx_in]) != mss_val_dbl){
		  var_val_dbl_out[idx_out]+=var_val_crr*wgt_raw[lnk_idx];
		  if(flg_rnr) wgt_vld_out[idx_out]+=wgt_raw[lnk_idx];
		  tally[idx_out]++;
		} /* endif */
	      } /* end loop over link */
	    } /* !flg_xtn */
	  }else{ /* lvl_nbr > 1 */
	    val_in_fst=0L;
	    val_out_fst=0L;
	    if(trv.flg_mrv){
	      for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
		for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		  idx_in=col_src_adr[lnk_idx]+val_in_fst;
		  idx_out=row_dst_adr[lnk_idx]+val_out_fst;
		  if((var_val_crr=var_val_dbl_in[idx_in]) != mss_val_dbl){
		    var_val_dbl_out[idx_out]+=var_val_crr*wgt_raw[lnk_idx];
		    if(flg_rnr) wgt_vld_out[idx_out]+=wgt_raw[lnk_idx];
		    tally[idx_out]++;
		  } /* endif */
		} /* end loop over link */
		val_in_fst+=grd_sz_in;
		val_out_fst+=grd_sz_out;
	      } /* end loop over lvl */
	    }else{ /* !flg_mrv */
	      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(fp_stdout,"INFO: Non-MRV variable %s: lvl_nbr = %d\n",trv.nm,lvl_nbr);
	      /* Algorithm to regrid non-MRV variables:
		 Permute input data until horizontal coordinates are MRV
		 Regrid as usual
		 Permute output data into original non-MRV order */

	      for(lnk_idx=0;lnk_idx<lnk_nbr;lnk_idx++){
		/* Translate col_src/row_dst addresses (which are 1-D offsets) into lon/lat/col indices in src/dst arrays */
		idx_in=col_src_adr[lnk_idx];
		idx_out=row_dst_adr[lnk_idx];

	      } /* end loop over link */
	    } /* !flg_mrv */
	  } /* lvl_nbr > 1 */
	} /* !has_mss_val */

	/* Rounding can be important for integer-type extensive variables */
	if(trv.flg_xtn){
	  if(nco_typ_ntg(var_typ_out)){
	    if(!has_mss_val){
	      for(dst_idx=0;dst_idx<var_sz_out;dst_idx++)
		var_val_dbl_out[dst_idx]=round(var_val_dbl_out[dst_idx]);
	    }else{ /* has_mss_val */
	      for(dst_idx=0;dst_idx<var_sz_out;dst_idx++)
		if(tally[dst_idx]) var_val_dbl_out[dst_idx]=round(var_val_dbl_out[dst_idx]);
	    } /* !has_mss_val */
	  } /* !nco_typ_ntg */
	} /* !flg_xtn */
	  
	if(!has_mss_val){
	  if(flg_frc_nrm){
	    /* frc_dst = frc_out = dst_frac = frac_b contains non-unity elements and normalization type is "destarea" or "none"
	       When this occurs for conservative remapping, follow "destarea" normalization procedure
	       See SCRIP manual p. 11 and http://www.earthsystemmodeling.org/esmf_releases/public/ESMF_6_3_0rp1/ESMF_refdoc/node3.html#SECTION03028000000000000000
	       NB: Non-conservative interpolation methods (e.g., bilinear) should NOT apply this normalization (theoretically there is no danger in doing so because frc_out == 1 always for all gridcells that participate in bilinear remapping and frc_out == 0 otherwise, but still, best not to tempt the Fates)
	       NB: Both frc_out and NCO's renormalization (below) could serve the same purpose
	       Applying both could lead to double-normalizing by missing values!
	       20151018: Be sure this does not occur! currently this is done by only executing flg_frc_nrm block when !has_mss_val
	       and having a separate normalization block for has_mss_val
	       fxm: Use better logic and more metadata information to determine code path */
	    if(lvl_nbr == 1){
	      for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++)
		if(frc_out[dst_idx] != 0.0) var_val_dbl_out[dst_idx]/=frc_out[dst_idx];
	    }else{
	      for(dst_idx=0;dst_idx<grd_sz_out;dst_idx++){
		if(frc_out[dst_idx] != 0.0){
		  for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
		    var_val_dbl_out[dst_idx+lvl_idx*grd_sz_out]/=frc_out[dst_idx];
		  } /* !lvl_idx */
		} /* !frc_out */
	      } /* !dst_idx */
	    } /* lvl_nbr > 1 */
	  } /* flg_frc_nrm */
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
	  for(dst_idx=0;dst_idx<var_sz_out;dst_idx++)
	    if(!tally[dst_idx]) var_val_dbl_out[dst_idx]=mss_val_dbl;
	  if(flg_rnr){
	    if(wgt_vld_thr == 0.0){
	      for(dst_idx=0;dst_idx<var_sz_out;dst_idx++)
		if(tally[dst_idx]) var_val_dbl_out[dst_idx]/=wgt_vld_out[dst_idx];
	    }else{
	      for(dst_idx=0;dst_idx<var_sz_out;dst_idx++)
		if(wgt_vld_out[dst_idx] >= wgt_vld_thr){var_val_dbl_out[dst_idx]/=wgt_vld_out[dst_idx];}else{var_val_dbl_out[dst_idx]=mss_val_dbl;}
	    } /* !wgt_vld_thr */
	  } /* !flg_rnr */

	  /* 20150914: fxm extensive block */
	  if(flg_rnr && trv.flg_xtn){
	    for(dst_idx=0;dst_idx<var_sz_out;dst_idx++){
	      if(tally[dst_idx] > 0){
		if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(fp_stdout,"Extensive variable %s: dst_idx = %li, tally = %d, val_out_b4 = %g, wgt_vld_out = %g, val_out_after = %g\n",trv.nm,dst_idx,tally[dst_idx],var_val_dbl_out[dst_idx],wgt_vld_out[dst_idx],var_val_dbl_out[dst_idx]*wgt_vld_out[dst_idx]);
		var_val_dbl_out[dst_idx]*=wgt_vld_out[dst_idx];
	      } /* !tally */
	    } /* !dst_idx */
	  } /* !flg_xtn */

	} /* !has_mss_val */
	
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
      }else{
	/* Use standard NCO copy routine for variables that are not regridded */
#pragma omp critical
	{ /* begin OpenMP critical */
	  (void)nco_cpy_var_val(in_id,out_id,(FILE *)NULL,(md5_sct *)NULL,trv.nm,trv_tbl);
	} /* end OpenMP critical */
     } /* !flg_rgr */
    } /* !xtr */
  } /* end (OpenMP parallel for) loop over idx_tbl */
  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"\n");
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s completion report. Variables regridded = %d (%d extensive), copied unmodified = %d, omitted = %d, created = %d\n",nco_prg_nm_get(),fnc_nm,var_rgr_nbr,var_xtn_nbr,var_cpy_nbr,var_xcl_nbr,var_crt_nbr);
  
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
  if(wgt_raw) wgt_raw=(double *)nco_free(wgt_raw);
  
  if(False && flg_grd_out_crv){
    /* WRF curvilinear grid: 
       ncks -C -m -v XLAT,XLONG ${DATA}/hdf/wrfout_v2_Lambert.nc # Interrogate file
       ncwa -O -a Time ${DATA}/hdf/wrfout_v2_Lambert.nc ${DATA}/hdf/wrfout_v2_Lambert_notime.nc # Create simpler input
       ncks -C -d south_north,0 -d west_east,0 -v XLAT,XLONG ${DATA}/hdf/wrfout_v2_Lambert_notime.nc # Interrogate file
       ncks -O -D 1 -t 1 -v T --rgr infer --rgr idx_dbg=0 --rgr grid=${DATA}/sld/rgr/grd_wrf.nc ${DATA}/hdf/wrfout_v2_Lambert_notime.nc ~/foo.nc # Infer grid
       ESMF_RegridWeightGen -s ${DATA}/sld/rgr/grd_wrf.nc -d ${DATA}/grids/180x360_SCRIP.20150901.nc -w ${DATA}/sld/rgr/map_wrf_to_dst_aave.nc --method conserve --src_regional --ignore_unmapped # Template map
       ncks -O -D 1 -t 1 -v T --map=${DATA}/sld/rgr/map_wrf_to_dst_aave.nc ${DATA}/hdf/wrfout_v2_Lambert_notime.nc ~/foo.nc # Regrid manually
       ncremap -v T -i ${DATA}/hdf/wrfout_v2_Lambert_notime.nc -g ${DATA}/grids/180x360_SCRIP.20150901.nc -o ${DATA}/sld/rgr # Regrid automatically
       GenerateOverlapMesh --a ${DATA}/sld/rgr/grd_wrf.nc --b ${DATA}/grids/180x360_SCRIP.20150901.nc --out ${DATA}/sld/rgr/msh_ovr_wrf_to_180x360.g */
    if(False) (void)fprintf(stderr,"%s: INFO %s reports curvilinear grid reached end-of-the-line\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !flg_grd_out_crv */

  return rcd;
} /* end nco_rgr_map() */

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
 double * const lat_sin, /* O [frc] Sine of latitudes */
 double * const wgt_Gss) /* O [frc] Gaussian weights */
{  
  /* Purpose: Compute and return sine of Gaussian latitudes and their weights
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
      (void)fprintf(stdout,"%s: ERROR %s reports no convergence in %d iterations for lat_idx = %d\n",nco_prg_nm_get(),fnc_nm,itr_nbr_max,lat_idx);
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
  for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
    lat_sin[lat_idx]=lat_sin_p1[lat_nbr-lat_idx];
    wgt_Gss[lat_idx]=wgt_Gss_p1[lat_nbr-lat_idx];
  } /* end loop over lat */

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
(const double * const lat_bnd, /* [dgr] Latitude  boundaries of rectangular grid */
 const double * const lon_bnd, /* [dgr] Longitude boundaries of rectangular grid */
 const long col_nbr, /* [nbr] Number of columns in grid */
 const int bnd_nbr, /* [nbr] Number of bounds in gridcell */
 double * const area) /* [sr] Gridcell area */
{
  /* Purpose: Compute area of spherical polygon */
  /* In general, prefer L'Huilier's Theorem to Girard's Formula
     http://mathworld.wolfram.com/LHuiliersTheorem.html
     Girard's formula depends on pi-minus-angle and angle is usually quite small in our applications so precision would be lost
     L'Huilier's theorem depends only on angles (a,b,c) and semi-perimeter (s) and is well-conditioned for small angles
     semi-perimeter = half-perimeter of triangle = 0.5*(a+b+c)
     Spherical Excess (SE) difference between the sum of the angles of a spherical triangle area and a planar triangle area with same interior angles (which has sum equal to pi)
     SE is also the solid angle subtended by the spherical triangle and that's, well, astonishing and pretty cool
     Wikipedia shows a better SE formula for triangles which are ill-conditioned for L'Huilier's formula because a = b ~ 0.5c
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
     1. Girard method: Loses precision due to mismatch between pi and small spherical excesses
        A. Find interior angles/arc-lengths (a,b,c,d...) using spherical law of cosines along each edge
        B. Apply generalized Girard formula SE_n = Sum(A_n) - (N-2) - pi
     2. L'Huilier method, N-2 triangle version by Zender: 
        Convert polygon into triangles by cycling spoke through all sides from common apex
        This method requires computation of N-2 (not N) triangles, though fewer sides due to optimization
	It works on all convex polygons (interior angles less than 180) but not, in general, concave polygons
	Whether it works or not on concave polygons depends upon their exact shape and the choice of apex point
        A. First three non-identical points form first triangle with sides A,B,C (first+second point define A, etc.)
	   i. First vertice anchors all triangles
	   ii. Third vertice of preceding triangle becomes second vertice of next triangle
	   iii. Next non-identical point becomes last vertice of next triangle
	   iv. Side C of previous triangle is side A of next triangle
	B. For each triangle, compute area with L'Huilier formula unless A = B ~ 0.5*C
     3. L'Huilier method, N triangle version by Taylor: 
        Compute polygon centroid and treat this as hub from which spokes are drawn to all vertices
        This method requires computation of N triangles, though fewer sides due to optimization
	Moreover, it works on all convex polygons and on slightly concave polygons
	Centroid/hub has clear view of interior of most simple concave polygons
     4. L'Huilier method with exact RLL grids by Zender and Agress 20160918
        A. Decompose polygon into triangles via and method (e.g., method 2 or 3 above)
	B. Determine whether triangle is spherical or contains RLL (constant latitude)
	C. Spherical triangles use L'Huilier, RLL triangles use series expansion */
  const char fnc_nm[]="nco_sph_plg_area()";
  const double dgr2rdn=M_PI/180.0;
  short int bnd_idx;
  long idx; /* [idx] Counting index for unrolled grids */

  double *lat_bnd_rdn=NULL_CEWI; /* [rdn] Latitude  boundaries of rectangular destination grid */
  double *lon_bnd_rdn=NULL_CEWI; /* [rdn] Longitude boundaries of rectangular destination grid */
  double *lat_bnd_sin=NULL_CEWI; /* [frc] Sine of latitude  boundaries of rectangular destination grid */
  double *lon_bnd_sin=NULL_CEWI; /* [frc] Sine of longitude boundaries of rectangular destination grid */
  double *lat_bnd_cos=NULL_CEWI; /* [frc] Cosine of latitude  boundaries of rectangular destination grid */
  double *lon_bnd_cos=NULL_CEWI; /* [frc] Cosine of longitude boundaries of rectangular destination grid */
  lon_bnd_rdn=(double *)nco_malloc(col_nbr*bnd_nbr*sizeof(double));
  lat_bnd_rdn=(double *)nco_malloc(col_nbr*bnd_nbr*sizeof(double));
  lon_bnd_cos=(double *)nco_malloc(col_nbr*bnd_nbr*sizeof(double));
  lat_bnd_cos=(double *)nco_malloc(col_nbr*bnd_nbr*sizeof(double));
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
  double area_crc; /* [sr] Latitude-triangle correction to spherical triangle area */
  double area_ltr; /* [sr] Gridcell area allowing for latitude-triangles */
  double area_ttl; /* [sr] Sphere area assuming spherical triangles */
  double area_ltr_ttl; /* [sr] Sphere area allowing for latitude-triangles */
  double area_crc_ttl; /* [sr] Latitude-triangle correction for whole sphere */
  double area_crc_abs_ttl; /* [sr] Latitude-triangle absolute correction for whole sphere */
  double lat_dlt; /* [rdn] Latitudinal difference */
  double lon_dlt; /* [rdn] Longitudinal difference */
  double ngl_a; /* [rdn] Interior angle/great circle arc a */
  double ngl_b; /* [rdn] Interior angle/great circle arc b */
  double ngl_c; /* [rdn] Interior angle/great circle arc c */
  double prm_smi; /* [rdn] Semi-perimeter of triangle */
  double sin_hlf_tht; /* [frc] Sine of half angle/great circle arc theta connecting two points */
  double xcs_sph; /* [sr] Spherical excess */
  double xcs_sph_qtr_tan; /* [frc] Tangent of one-quarter the spherical excess */
  int tri_nbr; /* [nbr] Number of triangles in polygon */
  long idx_a; /* [idx] Point A 1-D index */
  long idx_b; /* [idx] Point B 1-D index */
  long idx_c; /* [idx] Point C 1-D index */
  nco_bool flg_ltr_cll; /* [flg] Any triangle in cell is latitude-triangle */
  nco_bool flg_ltr_crr; /* [flg] Current triangle is latitude-triangle */
  area_ttl=0.0;
  area_ltr_ttl=0.0;
  area_crc_ttl=0.0;
  area_crc_abs_ttl=0.0;
  for(unsigned int col_idx=0;col_idx<col_nbr;col_idx++){
    flg_ltr_cll=False;
    ngl_c=double_CEWI; /* Otherwise compiler unsure ngl_c is initialized first use */
    area[col_idx]=0.0;
    area_ltr=0.0;
    tri_nbr=0;
    /* A is always first vertice */
    idx_a=bnd_nbr*col_idx; 
    /* Start search for B at next vertice */
    bnd_idx=1;
    /* bnd_idx labels offset from point A of potential location of triangle points B and C 
       We know that bnd_idx(A) == 0, bnd_idx(B) < bnd_nbr-1, bnd_idx(C) < bnd_nbr */
    while(bnd_idx<bnd_nbr-1){
      flg_ltr_crr=False;
      /* Only first triangle must search for B, subsequent triangles recycle previous C as current B */
      if(tri_nbr == 0){
	/* Skip repeated points that must occur when polygon has fewer than allowed vertices */
	while(lon_bnd[idx_a] == lon_bnd[idx_a+bnd_idx] && lat_bnd[idx_a] == lat_bnd[idx_a+bnd_idx]){
	  /* Next vertice may not duplicate A */
	  bnd_idx++;
	  /* If there is no room for C then all triangles found */
	  if(bnd_idx == bnd_nbr-1) break;
	} /* !while */
	/* Jump to next column when all triangles found */
	if(bnd_idx == bnd_nbr-1) break;
      } /* !tri_nbr */
      idx_b=idx_a+bnd_idx;
      /* Search for C at next vertice */
      bnd_idx++;
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
      tri_nbr++;
      /* Compute interior angle/great circle arc a for first triangle; subsequent triangles recycle previous arc c */
      if(tri_nbr == 1){
	/* 20150831: Test by computing ncol=0 area in conus chevrons grid, compare to MAT results
	   ncremap -s ${DATA}/grids/ne30np4_pentagons.091226.nc -g ${DATA}/grids/257x512_SCRIP.20150901.nc -m ${DATA}/maps/map_ne30np4_to_fv257x512_bilin.20150901.nc
	   ncremap -s ${DATA}/grids/257x512_SCRIP.20150901.nc -g ${DATA}/grids/conusx4v1np4_chevrons_scrip_c150815.nc -m ${DATA}/maps/map_fv257x512_to_conusx4v1np4_chevrons_bilin.20150901.nc
	   ncks -O -D 5 -v FSNT --map ${DATA}/maps/map_ne30np4_to_fv257x512_bilin.150418.nc ${DATA}/ne30/rgr/famipc5_ne30_v0.3_00003.cam.h0.1979-01.nc ${DATA}/ne30/rgr/fv_FSNT.nc
	   ncks -O -D 5 -v FSNT --map ${DATA}/maps/map_fv257x512_to_conusx4v1np4_chevrons_bilin.20150901.nc ${DATA}/ne30/rgr/fv_FSNT.nc ${DATA}/ne30/rgr/dogfood.nc
	   ncks -H -s %20.15e, -v area -d ncol,0 ${DATA}/ne30/rgr/dogfood.nc
	   ncks -H -s %20.15e, -v grid_area -d grid_size,0 ${DATA}/grids/conusx4v1np4_chevrons_scrip_c150815.nc
	   
	   ncol=0 on conus chevrons file:
	   3.653857995295246e-05 raw GLL weight
	   3.653857995294302e-05 matlab N-2 triangles (CSZ algorithm)
	   3.653857995294301e-05 matlab N   triangles (MAT algorithm)
	   3.653857995294258e-05 new NCO (haversine)
	   3.653857995289623e-05 old NCO (acos) */
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
      }else{
	ngl_a=ngl_c;
      } /* !tri_nbr */
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
      /* Ill-conditioned? */
      if(((float)ngl_a == (float)ngl_b && (float)ngl_a == (float)(0.5*ngl_c)) || /* c is half a and b */
	 ((float)ngl_b == (float)ngl_c && (float)ngl_b == (float)(0.5*ngl_a)) || /* a is half b and c */
	 ((float)ngl_c == (float)ngl_a && (float)ngl_c == (float)(0.5*ngl_b))){  /* b is half c and a */
	(void)fprintf(stdout,"%s: WARNING %s reports col_idx = %u triangle %d is ill-conditioned. Spherical excess and thus cell area are likely inaccurate. Ask Charlie to implement SAS formula...\n",nco_prg_nm_get(),fnc_nm,col_idx,tri_nbr);
      } /* !ill */
      /* Semi-perimeter */
      prm_smi=0.5*(ngl_a+ngl_b+ngl_c);
      /* L'Huilier's formula */
      xcs_sph_qtr_tan=sqrt(tan(0.5*prm_smi)*tan(0.5*(prm_smi-ngl_a))*tan(0.5*(prm_smi-ngl_b))*tan(0.5*(prm_smi-ngl_c)));
      xcs_sph=4.0*atan(xcs_sph_qtr_tan);
      area[col_idx]+=xcs_sph;
      area_ltr+=xcs_sph;
      area_ttl+=xcs_sph;
      area_ltr_ttl+=xcs_sph;
      /* Begin search for next B at current C */
      bnd_idx=idx_c-idx_a;
      /* 20160918 from here to end of loop is non-spherical work
	 Canonical latitude-triangle geometry has point A at apex and points B and C at same latitude
	 Generate area field for latitude-triangles by fxm
	 ncremap -s ${DATA}/grids/257x512_SCRIP.20150901.nc -g ${DATA}/grids/ne30np4_pentagons.091226.nc -m ${DATA}/maps/map_fv257x512_to_ne30np4_bilin.20150901.nc
	 ncap2 -O -s area_b=0.0 ${DATA}/maps/map_fv257x512_to_ne30np4_bilin.20150901.nc ~/rgr/map_fv257x512_to_ne30np4_bilin.no_area_b.nc
	 ncks -O -D 5 -v FSNT --map ${DATA}/maps/map_ne30np4_to_fv257x512_bilin.150418.nc ${DATA}/ne30/rgr/famipc5_ne30_v0.3_00003.cam.h0.1979-01.nc ${DATA}/ne30/rgr/fv_FSNT.nc
	 ncks -O -D 5 -v FSNT --map ${DATA}/rgr/map_fv257x512_to_ne30np4_bilin.no_area_b.nc ${DATA}/ne30/rgr/fv_FSNT.nc ${DATA}/ne30/rgr/ne30_FSNT.nc > ~/foo.txt 2>&1 */
      if(lat_bnd_rdn[idx_a] == lat_bnd_rdn[idx_b] ||
	 lat_bnd_rdn[idx_b] == lat_bnd_rdn[idx_c] ||
	 lat_bnd_rdn[idx_c] == lat_bnd_rdn[idx_a]){
	flg_ltr_cll=flg_ltr_crr=True;
      } /* endif */
      /* 20170217: Temporarily turn-off latitude circle diagnostics because Sungduk's POP case breaks them */
      flg_ltr_cll=flg_ltr_crr=False;
      if(flg_ltr_crr){
	double ngl_ltr_a; /* [rdn] Interior angle/small circle arc a, canonical latitude-triangle geometry */
	double ngl_ltr_b; /* [rdn] Interior angle/great circle arc b, canonical latitude-triangle geometry */
	double ngl_ltr_c; /* [rdn] Interior angle/great circle arc c, canonical latitude-triangle geometry */
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
	  abort();
	} /* endif */
	/* 20160918: Compute area of latitude triangle wedge exactly */
	double xpn_x; /* [frc] Expansion parameter */
	lon_dlt=fabs(nco_lon_dff_brnch_rdn(lon_bnd_rdn[idx_ltr_b],lon_bnd_rdn[idx_ltr_c]));
	/* Numeric conditioning uncertain. Approaches divide-by-zero when lon_dlt << 1 */
	xpn_x=lat_bnd_sin[idx_ltr_b]*(1.0-cos(lon_dlt))/sin(lon_dlt);
	area_crc=2.0*atan(xpn_x);
	/* 20170217: Sungduk's POP regrid triggers following abort():
	   ncremap -D 1 -i ~/pop_g16.nc -d ~/cam_f19.nc -o ~/foo.nc */
	if(xpn_x < 0.0) abort();
	//if(lat_bnd[idx_ltr_b] > 0.0) area_crc+=-lon_dlt*lat_bnd_sin[idx_ltr_b]; else area_crc+=+lon_dlt*lat_bnd_sin[idx_ltr_b];
	area_crc+=-lon_dlt*lat_bnd_sin[idx_ltr_b];
	area_ltr+=area_crc;
	area_ltr_ttl+=area_crc;
	area_crc_ttl+=area_crc;
	area_crc_abs_ttl+=fabs(area_crc);
	if(0){
	  /* 20160918: Compute area of latitude triangle wedge using power expansion */
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
	  (void)fprintf(stdout,"%s: Latitude-triangle area using series approximation...not implemented yet\n",nco_prg_nm_get());
	} /* !0 */
	if(nco_dbg_lvl_get() >= nco_dbg_scl){
	  (void)fprintf(stdout,"%s: INFO %s col_idx = %u triangle %d spherical area, latitude-triangle area, %% difference: %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,col_idx,tri_nbr,xcs_sph,xcs_sph+area_crc,100.0*area_crc/xcs_sph);
	  if(fabs(area_crc/xcs_sph) > 0.1){
	    (void)fprintf(stdout,"%s: DBG Non-spherical correction exceeds 10%% for current triangle with ABC vertices at lat,lon [dgr] = %g, %g\n%g, %g\n%g, %g\n",nco_prg_nm_get(),lat_bnd[idx_ltr_a],lon_bnd[idx_ltr_a],lat_bnd[idx_ltr_b],lon_bnd[idx_ltr_b],lat_bnd[idx_ltr_c],lon_bnd[idx_ltr_c]);
	  } /* !fabs */
	} /* !dbg */
      } /* !flg_ltr_crr */
    } /* !tri_idx */
    if(flg_ltr_cll){
      /* Current gridcell contained at least one latitude-triangle */
      if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s col_idx = %u spherical area, latitude-gridcell area, %% difference: %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,col_idx,area[col_idx],area_ltr,100.0*(area_ltr-area[col_idx])/area[col_idx]);
    } /* !flg_ltr_cll */    
  } /* !col_idx */
  if(nco_dbg_lvl_get() >= nco_dbg_scl) (void)fprintf(stdout,"%s: INFO %s total spherical area, latitude-gridcell area, %% difference, crc_ttl, crc_abs_ttl: %g, %g, %g, %g, %g\n",nco_prg_nm_get(),fnc_nm,area_ttl,area_ltr_ttl,100.0*(area_ltr_ttl-area_ttl)/area_ttl,area_crc_ttl,area_crc_abs_ttl);
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
  nco_rgr_cmd_typ nco_rgr_cmd; /* [enm] TempestRemap command enum */

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
  nco_rgr_cmd=nco_rgr_GenerateRLLMesh;
  cmd_rgr_fmt=nco_tps_cmd_fmt_sng(nco_rgr_cmd);
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
  case nco_grd_2D_gss: return "Gaussian latitude grid. Used by spectral transform models, e.g., CCM 1-3, CAM 1-3, LSM, MATCH, UCICTM.";
  case nco_grd_2D_fv: return "Cap grid. Uniform/Equi-angle (except at poles) latitude grid. Poles are considered at (and labeled as) centers of first and last gridcells (i.e., lat_ctr[0]=-90). Polar gridcells span half the equi-angular latitude increment. AKA FV-scalar grid (in Lin-Rood representation). Used by CAM FV, GEOS-CHEM, UCICTM, UKMO.";
  case nco_grd_2D_eqa: return "Uniform/Equi-Angular latitude grid. Uniform/Equi-angle (everywhere) latitude grid. When global (not regional) in extent, poles are at edges of first and last gridcells (i.e., lat_ctr[0]=-89.xxx). When global forms valid FV-staggered AKA FV velocity grid (for Lin-Rood representation). Used by CIESIN/SEDAC, IGBP-DIS, TOMS AAI.";
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
  case nco_grd_lat_gss: return "Gaussian latitude grid used by global spectral models: CCM 1-3, CAM 1-3, LSM, MATCH, UCICTM";
  case nco_grd_lat_fv: return "Cap-latitude grid. Uniform/Equi-angle (except at poles) latitude grid. Poles are considered at (and labeled as) centers of first and last gridcells (i.e., lat_ctr[0]=-90). Polar gridcells span half the equi-angular latitude increment. AKA FV-scalar grid (in Lin-Rood representation). Used by: CAM FV, GEOS-CHEM, UCICTM, UKMO";
  case nco_grd_lat_eqa: return "Uniform/Equi-Angular latitude grid. Uniform/Equi-angle (everywhere) latitude grid. When global (not regional) in extent, poles are at edges of first and last gridcells (e.g., lat_ctr[0]=-89.xxx). When global can be latitude-component of a valid FV-staggered grid AKA FV velocity grid (for Lin-Rood representation). Used by: CIESIN/SEDAC, IGBP-DIS, TOMS AAI";
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
  case nco_rgr_nrm_unknown: return "Unknown (TempestRemap or ESMF_weight_only)";
  default: nco_dfl_case_generic_err(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_rgr_nrm_sng() */

const char * /* O [sng] String containing regridding command and format */
nco_tps_cmd_fmt_sng /* [fnc] Convert TempestRemap command enum to command string */
(const nco_rgr_cmd_typ nco_rgr_cmd) /* I [enm] TempestRemap command enum */
{
  /* Purpose: Convert TempestRemap command enum to command string and format */
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
nco_tps_cmd_sng /* [fnc] Convert TempestRemap command enum to command name */
(const nco_rgr_cmd_typ nco_rgr_cmd) /* I [enm] TempestRemap command enum */
{
  /* Purpose: Convert TempestRemap command enum to command string */
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

     Global RLL grids:
     ncks -O -D 1 --rgr grd_ttl='Equiangular grid 180x360' --rgr grid=${DATA}/grids/180x360_SCRIP.20150901.nc --rgr latlon=180,360 --rgr lat_typ=eqa --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='Equiangular grid 90x180' --rgr grid=${DATA}/grids/90x180_SCRIP.20150901.nc --rgr latlon=90,180 --rgr lat_typ=eqa --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc

     Maps for global RLL grids:
     ESMF_RegridWeightGen -s ${DATA}/grids/180x360_SCRIP.20150901.nc -d ${DATA}/grids/90x180_SCRIP.20150901.nc -w ${DATA}/maps/map_180x360_to_90x180.20150901.nc --method conserve
     ESMF_RegridWeightGen -s ${DATA}/grids/90x180_SCRIP.20150901.nc -d ${DATA}/grids/180x360_SCRIP.20150901.nc -w ${DATA}/maps/map_90x180_to_180x360.20150901.nc --method conserve

     ACME grids:
     ncks -O -D 1 --rgr grd_ttl='FV-scalar grid 129x256' --rgr grid=${DATA}/grids/129x256_SCRIP.20150910.nc --rgr latlon=129,256 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr  ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='FV-scalar grid 257x512' --rgr grid=${DATA}/grids/257x512_SCRIP.20150910.nc --rgr latlon=257,512 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr  ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='FV-scalar grid 801x1600' --rgr grid=${DATA}/grids/801x1600_SCRIP.20150910.nc --rgr latlon=801,1600 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr  ~/nco/data/in.nc ~/foo.nc

     ACME maps:
     ESMF_RegridWeightGen -s ${DATA}/grids/ne30np4_pentagons.091226.nc -d ${DATA}/grids/129x256_SCRIP.20150910.nc -w ${DATA}/maps/map_ne30np4_to_fv129x256_aave.20150910.nc --method conserve
     ESMF_RegridWeightGen -s ${DATA}/grids/ne30np4_pentagons.091226.nc -d ${DATA}/grids/257x512_SCRIP.20150910.nc -w ${DATA}/maps/map_ne30np4_to_fv257x512_bilin.20150910.nc --method bilinear
     ESMF_RegridWeightGen -s ${DATA}/grids/ne120np4_pentagons.100310.nc -d ${DATA}/grids/257x512_SCRIP.20150910.nc -w ${DATA}/maps/map_ne120np4_to_fv257x512_aave.20150910.nc --method conserve
     ESMF_RegridWeightGen -s ${DATA}/grids/ne120np4_pentagons.100310.nc -d ${DATA}/grids/801x1600_SCRIP.20150910.nc -w ${DATA}/maps/map_ne120np4_to_fv801x1600_bilin.20150910.nc --method bilinear

     AMWG grids: AMWG diagnostics (until ~2016) mis-diagnose FV grids with odd numbers of latitudes as Gaussian Grids
     ncks -O -D 1 --rgr grd_ttl='CAM FV-scalar grid 96x144 for horizontal resolution 1.9x2.5 degrees' --rgr grid=${DATA}/grids/96x144_SCRIP.20160301.nc --rgr latlon=96,144 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='CAM FV-scalar grid 192x288 for horizontal resolution 0.9x1.25 degrees' --rgr grid=${DATA}/grids/192x288_SCRIP.20160301.nc --rgr latlon=192,288 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='CAM FV-scalar grid 128x256 for horizontal resolution 1.4x1.4 degrees' --rgr grid=${DATA}/grids/128x256_SCRIP.20160301.nc --rgr latlon=128,256 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='CAM FV-scalar grid 256x512 for horizontal resolution 0.7x0.7 degrees' --rgr grid=${DATA}/grids/256x512_SCRIP.20160301.nc --rgr latlon=256,512 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='CAM FV-scalar grid 800x1600 for horizontal resolution 0.225x0.225 degrees' --rgr grid=${DATA}/grids/800x1600_SCRIP.20160301.nc --rgr latlon=800,1600 --rgr lat_typ=cap --rgr lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='Equiangular grid 360x720 produced by RTM' --rgr grid=${DATA}/grids/360x720rtm_SCRIP.20160301.nc --rgr latlon=360,720 --rgr lat_typ=eqa --rgr lon_typ=180_wst ~/nco/data/in.nc ~/foo.nc

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
     ncks -O -D 1 --rgr grd_ttl='Equiangular grid 180x360' --rgr grid=${DATA}/sld/rgr/grd_dst.nc --rgr latlon=100,100 --rgr snwe=30.0,70.0,-120.0,-90.0 ~/nco/data/in.nc ~/foo.nc

     Global RLL skeleton:
     ncks -O -D 1 --rgr grd_ttl='Equiangular grid 180x360' --rgr skl=${DATA}/sld/rgr/skl_180x360.nc --rgr grid=${DATA}/grids/180x360_SCRIP.20150901.nc --rgr latlon=180,360#lat_typ=eqa#lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc

     Curvilinear grids:
     ncks -O -D 1 --rgr grd_ttl='Curvilinear grid 10x20. Degenerate case.' --rgr crv --rgr lon_crv=0.0 --rgr skl=${DATA}/sld/rgr/skl_crv.nc --rgr grid=${DATA}/sld/rgr/grd_crv.nc --rgr latlon=10,20 --rgr snwe=-5.0,5.0,-10.0,10.0 ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='Curvilinear grid 10x20. Curvilinearity = 1.0 lon' --rgr lon_crv=1.0 --rgr skl=${DATA}/sld/rgr/skl_crv.nc --rgr grid=${DATA}/sld/rgr/grd_crv.nc --rgr latlon=10,20 --rgr snwe=-5.0,5.0,-10.0,10.0 ~/nco/data/in.nc ~/foo.nc

     1-D Latitude (no longitude) grids:
     ncks -O -D 1 --rgr grd_ttl='Latitude-only zonal grid' --rgr skl=${DATA}/sld/rgr/skl_lat_10dgr_uni.nc --rgr grid=${DATA}/sld/rgr/grd_lat_10dgr_uni.nc --rgr latlon=18,1 --rgr snwe=-90,90,0,360  ~/nco/data/in.nc ~/foo.nc
     ncks -O -D 1 --rgr grd_ttl='Latitude-only zonal grid' --rgr skl=${DATA}/sld/rgr/skl_lat_05dgr_cap.nc --rgr grid=${DATA}/sld/rgr/grd_lat_05dgr_cap.nc --rgr latlon=37,1 --rgr snwe=-90,90,0,360  ~/nco/data/in.nc ~/foo.nc
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
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */
  nco_bool flg_grd_1D=False;
  nco_bool flg_grd_2D=False;
  nco_bool flg_grd_crv=False;

  nco_grd_2D_typ_enm grd_typ; /* [enm] Grid-type enum */
  nco_grd_lat_typ_enm lat_typ; /* [enm] Latitude grid-type enum */
  nco_grd_lon_typ_enm lon_typ; /* [enm] Longitude grid-type enum */

  size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */

  dfl_lvl=rgr->dfl_lvl;
  grd_typ=rgr->grd_typ; /* [enm] Grid type */
  fl_out=rgr->fl_grd;
  fl_out_fmt=rgr->fl_out_fmt;
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
     NB: This is redundant since it was done in nco_rgr_ini(), but better safe than sorry */
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
  lon_ntf[0]=lon_wst;
  lon_ntf[lon_nbr]=lon_est;

  for(lon_idx=1;lon_idx<lon_nbr;lon_idx++)
    lon_ntf[lon_idx]=lon_ntf[0]+lon_idx*lon_ncr;
  /* Ensure rounding errors do not produce unphysical grid */
  lon_ntf[lon_nbr]=lon_ntf[0]+lon_spn;
  
  /* Finished with longitude, now tackle latitude */
  
  /* Were south/north latitude bounds set explicitly or implicitly? */
  //  if(lat_sth != NC_MAX_DOUBLE || lat_nrt != NC_MAX_DOUBLE) lon_typ=rgr->lat_typ=nco_grd_lat_bb;
  if(lat_sth == NC_MAX_DOUBLE) lat_sth=-90.0;
  if(lat_nrt == NC_MAX_DOUBLE) lat_nrt=90.0;
  
  /* Determine latitude increment from span of pre-centered bounding box (centering will not change span) */
  lat_spn=lat_nrt-lat_sth;
  lat_ncr=lat_spn/lat_nbr;

  double *lat_sin=NULL; // [frc] Sine of Gaussian latitudes double precision
  lat_ntf[0]=lat_sth;
  switch(lat_typ){
  case nco_grd_lat_fv:
    lat_ncr=lat_spn/(lat_nbr-1);
    lat_ntf[1]=lat_ntf[0]+0.5*lat_ncr;
    for(lat_idx=2;lat_idx<lat_nbr;lat_idx++)
      lat_ntf[lat_idx]=lat_ntf[1]+(lat_idx-1)*lat_ncr;
    break;
  case nco_grd_lat_eqa:
    lat_ncr=lat_spn/lat_nbr;
    for(lat_idx=1;lat_idx<lat_nbr;lat_idx++)
      lat_ntf[lat_idx]=lat_ntf[0]+lat_idx*lat_ncr;
    break;
  case nco_grd_lat_gss:
    lat_sin=(double *)nco_malloc(lat_nbr*sizeof(double));
    (void)nco_lat_wgt_gss(lat_nbr,lat_sin,wgt_Gss);
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
      lat_ctr[lat_idx]=rdn2dgr*asin(lat_sin[lat_idx]);
    /* First guess for lat_ntf is midway between Gaussian abscissae */
    for(lat_idx=1;lat_idx<lat_nbr;lat_idx++)
      lat_ntf[lat_idx]=0.5*(lat_ctr[lat_idx-1]+lat_ctr[lat_idx]);
    /* Iterate guess until area between interfaces matches Gaussian weight */
    for(lat_idx=1;lat_idx<lat_nbr;lat_idx++){
      double fofx_at_x0; /* [frc] Function to iterate evaluated at current guess */
      double dfdx_at_x0; /* [frc] Derivation of equation evaluated at current guess */
      const double eps_rlt_cnv=1.0e-15; // Convergence criterion (1.0e-16 pushes double precision to the brink)
      itr_cnt=0;
      lat_wgt_gss=sin(dgr2rdn*lat_ntf[lat_idx])-sin(dgr2rdn*lat_ntf[lat_idx-1]);
      fofx_at_x0=wgt_Gss[lat_idx-1]-lat_wgt_gss;
      while(fabs(fofx_at_x0) > eps_rlt_cnv){
	/* Newton-Raphson iteration:
	   Let x=lat_ntf[lat_idx], y0=lat_ntf[lat_idx-1], gw = Gaussian weight (exact solution)
	   f(x)=sin(dgr2rdn*x)-sin(dgr2rdn*y0)-gw=0 
	   dfdx(x)=dgr2rdn*(dgr2rdn*x)
	   x_better=x0-f(x0)/f'(x0) */
	dfdx_at_x0=dgr2rdn*cos(dgr2rdn*lat_ntf[lat_idx]);
	lat_ntf[lat_idx]+=fofx_at_x0/dfdx_at_x0; /* NB: not sure why this is minus not plus but it works :) */
	lat_wgt_gss=sin(dgr2rdn*lat_ntf[lat_idx])-sin(dgr2rdn*lat_ntf[lat_idx-1]);
	fofx_at_x0=wgt_Gss[lat_idx-1]-lat_wgt_gss;
	if(++itr_cnt > itr_nbr_max){
	  (void)fprintf(stdout,"%s: ERROR %s reports no convergence in %d iterations for lat_idx = %ld\n",nco_prg_nm_get(),fnc_nm,itr_nbr_max,lat_idx);
	  nco_exit(EXIT_FAILURE);
	} /* endif */
      } /* !while */
    } /* !lat */
    break;
  default:
    nco_dfl_case_generic_err(); break;
  } /* !lat_typ */
  /* Ensure rounding errors do not produce unphysical grid */
  lat_ntf[lat_nbr]=lat_nrt;
  
  /* Always define longitude centers midway between interfaces */
  for(lon_idx=0;lon_idx<=lon_nbr-1;lon_idx++)
    lon_ctr[lon_idx]=0.5*(lon_ntf[lon_idx]+lon_ntf[lon_idx+1]);

  /* Many grids have center latitude equally spaced between interfaces */
  for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
    lat_ctr[lat_idx]=0.5*(lat_ntf[lat_idx]+lat_ntf[lat_idx+1]);

  /* Cap grids excepted---they place centers of first/last gridcells at poles */
  if(lat_typ == nco_grd_lat_fv){
    lat_ctr[0]=lat_ntf[0];
    for(lat_idx=1;lat_idx<lat_nbr-1;lat_idx++)
      lat_ctr[lat_idx]=0.5*(lat_ntf[lat_idx]+lat_ntf[lat_idx+1]);
    lat_ctr[lat_nbr-1]=lat_ntf[lat_nbr];
  } /* !cap */
  /* Gaussian grids centerpoints are defined by solutions to Legendre polynomials */
  if(lat_typ == nco_grd_lat_gss){
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
      lat_ctr[lat_idx]=rdn2dgr*asin(lat_sin[lat_idx]);
  } /* !Gaussian */
  
  for(idx=0;idx<lon_nbr;idx++){
    lon_bnd[2*idx]=lon_ntf[idx];
    lon_bnd[2*idx+1]=lon_ntf[idx+1];
  } /* end loop over longitude */
  for(idx=0;idx<lat_nbr;idx++){
    lat_bnd[2*idx]=lat_ntf[idx];
    lat_bnd[2*idx+1]=lat_ntf[idx+1];
  } /* end loop over latitude */
  
  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    for(idx=0;idx<lat_nbr;idx++){
    (void)fprintf(stdout,"lat[%li] = %g, vertices = ",idx,lat_ctr[idx]);
    for(int bnd_idx=0;bnd_idx<bnd_nbr;bnd_idx++)
      (void)fprintf(stdout,"%s%g%s",bnd_idx == 0 ? "[" : "",lat_bnd[bnd_nbr*idx+bnd_idx],bnd_idx == bnd_nbr-1 ? "]\n" : ", ");
    } /* end loop over lat */
  } /* endif dbg */

  /* Use centers and boundaries to diagnose latitude weights */
  switch(lat_typ){
  case nco_grd_lat_eqa:
  case nco_grd_lat_fv:
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=sin(dgr2rdn*lat_ntf[lat_idx+1])-sin(dgr2rdn*lat_ntf[lat_idx]);
    break;
  case nco_grd_lat_gss:
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=wgt_Gss[lat_idx];
    break;
  default:
    nco_dfl_case_generic_err(); break;
  } /* !lat_typ */

  /* Fuzzy test of latitude weight normalization */
  const double eps_rlt_max=1.0e-14; /* [frc] Round-off error tolerance */
  double lat_wgt_ttl_xpc; /* [frc] Expected sum of latitude weights */
  lat_wgt_ttl=0.0;
  for(idx=0;idx<lat_nbr;idx++) lat_wgt_ttl+=lat_wgt[idx];
  lat_wgt_ttl_xpc=sin(dgr2rdn*lat_bnd[2*(lat_nbr-1)+1])-sin(dgr2rdn*lat_bnd[0]);
  if(grd_typ != nco_grd_2D_unk && 1.0-lat_wgt_ttl/lat_wgt_ttl_xpc > eps_rlt_max){
    (void)fprintf(stdout,"%s: ERROR %s reports grid normalization does not meet precision tolerance eps_rlt_max = %20.15f\nlat_wgt_ttl = %20.15f, lat_wgt_ttl_xpc = %20.15f, lat_wgt_frc = %20.15f, eps_rlt = %20.15f\n",nco_prg_nm_get(),fnc_nm,eps_rlt_max,lat_wgt_ttl,lat_wgt_ttl_xpc,lat_wgt_ttl/lat_wgt_ttl_xpc,1.0-lat_wgt_ttl/lat_wgt_ttl_xpc);
    nco_exit(EXIT_FAILURE);
  } /* !imprecise */

  assert(grd_crn_nbr == 4);
  for(lon_idx=0;lon_idx<lon_nbr;lon_idx++){
    idx=grd_crn_nbr*lon_idx;
    lon_crn[idx]=lon_ntf[lon_idx];
    lon_crn[idx+1]=lon_ntf[lon_idx+1];
    lon_crn[idx+2]=lon_ntf[lon_idx+1];
    lon_crn[idx+3]=lon_ntf[lon_idx];
  } /* !lon_idx */

  for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
    idx=grd_crn_nbr*lat_idx;
    lat_crn[idx]=lat_ntf[lat_idx];
    lat_crn[idx+1]=lat_ntf[lat_idx];
    lat_crn[idx+2]=lat_ntf[lat_idx+1];
    lat_crn[idx+3]=lat_ntf[lat_idx+1];
  } /* !lat_idx */
  
  /* Stuff rectangular arrays into unrolled arrays */
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
  
  if(flg_grd_crv){
    /* Impose curvilinearity by adding lon_crv offset to each row relative to previous row, and lat_crv offset to each column relative to previous column */
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++){
      for(lon_idx=0;lon_idx<lon_nbr;lon_idx++){
	idx=lat_idx*lon_nbr+lon_idx;
	grd_ctr_lat[idx]+=lon_idx*lat_crv;
	grd_ctr_lon[idx]+=lat_idx*lon_crv;
	for(crn_idx=0;crn_idx<grd_crn_nbr;crn_idx++){
	  idx2=grd_crn_nbr*idx+crn_idx;
	  lat_idx2=lat_idx*grd_crn_nbr+crn_idx;
	  lon_idx2=lon_idx*grd_crn_nbr+crn_idx;
	  grd_crn_lat[idx2]=lat_crn[lat_idx2];
	  grd_crn_lon[idx2]=lon_crn[lon_idx2];
	  if(crn_idx == 0 || crn_idx == 1){
	    grd_crn_lat[idx2]+=lat_idx*lat_crv; /* LL, LR */
	    grd_crn_lon[idx2]+=lat_idx*lon_crv; /* LL, LR */
	  }else if(crn_idx == 2 || crn_idx == 3){
	    grd_crn_lat[idx2]+=(lat_idx+1)*lat_crv; /* UL, UR */
	    grd_crn_lon[idx2]+=(lat_idx+1)*lon_crv; /* UL, UR */
	  } /* !crn */
	} /* !crn */
      } /* !lon */
    } /* !lat */
  } /* !flg_grd_crv */

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    long int idx_crn_ll;
    long int idx_crn_lr;
    long int idx_crn_ur;
    long int idx_crn_ul;
    long idx_dbg;
    idx_dbg=rgr->idx_dbg;
    idx_crn_ll=grd_crn_nbr*idx_dbg+0;
    idx_crn_lr=grd_crn_nbr*idx_dbg+1;
    idx_crn_ur=grd_crn_nbr*idx_dbg+2;
    idx_crn_ul=grd_crn_nbr*idx_dbg+3;
    (void)fprintf(stderr,"%s: INFO %s idx_dbg = %li, Center [lat,lon]=[%g,%g]; Corners LL [%g,%g] LR [%g,%g] UR [%g,%g] UL [%g,%g]\n",nco_prg_nm_get(),fnc_nm,idx_dbg,grd_ctr_lat[idx_dbg],grd_ctr_lon[idx_dbg],grd_crn_lat[idx_crn_ll],grd_crn_lon[idx_crn_ll],grd_crn_lat[idx_crn_lr],grd_crn_lon[idx_crn_lr],grd_crn_lat[idx_crn_ur],grd_crn_lon[idx_crn_ur],grd_crn_lat[idx_crn_ul],grd_crn_lon[idx_crn_ul]);
  } /* !dbg */

  if(flg_grd_crv){
    /* Area of arbitrary curvilinear grids requires spherical trigonometry */
    nco_sph_plg_area(grd_crn_lat,grd_crn_lon,grd_sz_nbr,grd_crn_nbr,area);
  }else{
    /* Area of rectangular spherical zones from elementary calculus results
       20150906: Half-angle formulae for better conditioning improve area normalization for 801x1600 by 2.0e-15 
       area[lat_idx*lon_nbr+lon_idx]=dgr2rdn*(lon_bnd[2*lon_idx+1]-lon_bnd[2*lon_idx])*2.0*(sin(0.5*dgr2rdn*lat_bnd[2*lat_idx+1])*cos(0.5*dgr2rdn*lat_bnd[2*lat_idx+1])-sin(0.5*dgr2rdn*lat_bnd[2*lat_idx])*cos(0.5*dgr2rdn*lat_bnd[2*lat_idx])); 
       Gain not worth the extra complexity */
    for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
      for(lon_idx=0;lon_idx<lon_nbr;lon_idx++)
	area[lat_idx*lon_nbr+lon_idx]=dgr2rdn*(lon_bnd[2*lon_idx+1]-lon_bnd[2*lon_idx])*(sin(dgr2rdn*lat_bnd[2*lat_idx+1])-sin(dgr2rdn*lat_bnd[2*lat_idx]));
  } /* !flg_grd_2D */

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
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);

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

  /* Define "units" attributes */
  aed_sct aed_mtd;
  char *att_nm;
  char *att_val;
  
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
  att_val=strdup("SCRIP");
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

  att_nm=strdup("latitude_grid_type");
  att_val=strdup(nco_grd_lat_sng(lat_typ));
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
  
  att_nm=strdup("longitude_grid_type");
  att_val=strdup(nco_grd_lon_sng(lon_typ));
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
  
  att_nm=strdup("units");
  att_val=strdup("steradian");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=grd_area_nm;
  aed_mtd.id=area_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,area_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  att_nm=strdup("units");
  att_val=strdup("degrees");
  aed_mtd.att_nm=att_nm;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  /* Add same units attribute to four different variables */
  aed_mtd.var_nm=grd_ctr_lat_nm;
  aed_mtd.id=grd_ctr_lat_id;
  (void)nco_aed_prc(out_id,grd_ctr_lat_id,aed_mtd);
  aed_mtd.var_nm=grd_ctr_lon_nm;
  aed_mtd.id=grd_ctr_lon_id;
  (void)nco_aed_prc(out_id,grd_ctr_lon_id,aed_mtd);
  aed_mtd.var_nm=grd_crn_lat_nm;
  aed_mtd.id=grd_crn_lat_id;
  (void)nco_aed_prc(out_id,grd_crn_lat_id,aed_mtd);
  aed_mtd.var_nm=grd_crn_lon_nm;
  aed_mtd.id=grd_crn_lon_id;
  (void)nco_aed_prc(out_id,grd_crn_lon_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);
  
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
    fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);
    
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
    att_val=strdup("CF-1.6");
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
    
    att_nm=strdup("latitude_grid_type");
    att_val=strdup(nco_grd_lat_sng(lat_typ));
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
    
    att_nm=strdup("longitude_grid_type");
    att_val=strdup(nco_grd_lon_sng(lon_typ));
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
    
    att_nm=strdup("long_name");
    att_val=strdup("Solid angle subtended by gridcell");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=area_nm;
    aed_mtd.id=area_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,area_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("area");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=area_nm;
    aed_mtd.id=area_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,area_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("units");
    att_val=strdup("steradian");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=area_nm;
    aed_mtd.id=area_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,area_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("long_name");
    att_val=strdup("Latitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lat_nm_out;
    aed_mtd.id=lat_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lat_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("latitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lat_nm_out;
    aed_mtd.id=lat_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lat_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("units");
    att_val=strdup("degrees_north");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lat_nm_out;
    aed_mtd.id=lat_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lat_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("axis");
    att_val=strdup("Y");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lat_nm_out;
    aed_mtd.id=lat_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lat_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("bounds");
    att_val=lat_bnd_nm;
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lat_nm_out;
    aed_mtd.id=lat_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lat_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);

    att_nm=strdup("long_name");
    if(flg_grd_2D) att_val=strdup("Gridcell latitude interfaces"); else att_val=strdup("Gridcell latitude vertices");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lat_bnd_nm;
    aed_mtd.id=lat_bnd_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lat_bnd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    if(flg_grd_2D){
      att_nm=strdup("long_name");
      att_val=strdup("Latitude quadrature weights (normalized to sum to 2.0 on global grids)");
      aed_mtd.att_nm=att_nm;
      aed_mtd.var_nm=lat_wgt_nm;
      aed_mtd.id=lat_wgt_id;
      aed_mtd.sz=strlen(att_val);
      aed_mtd.type=NC_CHAR;
      aed_mtd.val.cp=att_val;
      aed_mtd.mode=aed_create;
      (void)nco_aed_prc(out_id,lat_wgt_id,aed_mtd);
      if(att_nm) att_nm=(char *)nco_free(att_nm);
      if(att_val) att_val=(char *)nco_free(att_val);
    } /* !flg_grd_out_rct */
    
    att_nm=strdup("long_name");
    att_val=strdup("Longitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lon_nm_out;
    aed_mtd.id=lon_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lon_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("longitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lon_nm_out;
    aed_mtd.id=lon_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lon_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("units");
    att_val=strdup("degrees_east");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lon_nm_out;
    aed_mtd.id=lon_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lon_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);
    
    att_nm=strdup("axis");
    att_val=strdup("X");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lon_nm_out;
    aed_mtd.id=lon_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lon_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("bounds");
    att_val=lon_bnd_nm;
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lon_nm_out;
    aed_mtd.id=lon_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lon_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);

    att_nm=strdup("long_name");
    if(flg_grd_2D) att_val=strdup("Gridcell longitude interfaces"); else att_val=strdup("Gridcell longitude vertices");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=lon_bnd_nm;
    aed_mtd.id=lon_bnd_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,lon_bnd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

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

  double area_ttl=0.0; /* [frc] Exact sum of area */
  double lat_nrt; /* [dgr] Latitude of northern edge of grid */
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

  long dmn_srt[dmn_nbr_grd_max];
  long dmn_cnt[dmn_nbr_grd_max];

  long att_sz;
  long bnd_idx;
  long bnd_nbr=NC_MIN_INT; /* [nbr] Number of bounds in gridcell */
  long col_idx;
  long col_nbr; /* [nbr] Number of columns in grid */
  long crn_idx; /* [idx] Counting index for corners */
  long dmn_sz; /* [nbr] Size of current dimension */
  long grd_crn_nbr; /* [nbr] Number of corners in gridcell */
  long grd_rnk_nbr=int_CEWI; /* [nbr] Number of dimensions in grid */
  long grd_sz_nbr; /* [nbr] Number of gridcells in grid */
  long idx2; /* [idx] Counting index for unrolled grids */
  long idx; /* [idx] Counting index for unrolled grids */
  long idx_crn;
  long idx_ctr;
  long lat_idx2; /* [idx] Counting index for unrolled latitude */
  long lat_idx;
  long lat_nbr; /* [nbr] Number of latitudes in grid */
  long lon_idx2; /* [idx] Counting index for unrolled longitude */
  long lon_idx;
  long lon_nbr; /* [nbr] Number of longitudes in grid */
  
  long int idx_crn_ll;
  long int idx_crn_lr;
  long int idx_crn_ur;
  long int idx_crn_ul;
  
  nc_type att_typ;

  nco_bool FL_RTR_RMT_LCN;
  nco_bool FORCE_APPEND=False; /* Option A */
  nco_bool FORCE_OVERWRITE=True; /* Option O */
  nco_bool RAM_CREATE=False; /* [flg] Create file in RAM */
  nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
  nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
  nco_bool WRT_TMP_FL=False; /* [flg] Write output to temporary file */
  nco_bool flg_1D_psd_rct_bnd=False; /* [flg] Unstructured input grid with pseudo-rectangular bounds */
  nco_bool flg_grd_1D=False;
  nco_bool flg_grd_2D=False;
  nco_bool flg_grd_crv=False;
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
  fl_in=nco_fl_mk_lcl(fl_in,fl_pth_lcl,&FL_RTR_RMT_LCN);
  /* Open file using appropriate buffer size hints and verbosity */
  if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
  rcd+=nco_fl_open(fl_in,md_open,&bfr_sz_hnt,&in_id);

  char *bnd_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as bounds */
  char *col_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as column */
  char *lat_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as latitude */
  char *lon_dmn_nm=NULL_CEWI; /* [sng] Name of dimension to recognize as longitude */
  char *lat_nm_in=NULL_CEWI; /* [sng] Name of variable to recognize as latitude */
  char *lon_nm_in=NULL_CEWI; /* [sng] Name of variable to recognize as longitude */
  char *lat_bnd_nm=NULL_CEWI; /* [sng] Name of latitude  boundary variable */
  char *lon_bnd_nm=NULL_CEWI; /* [sng] Name of longitude boundary variable */
  int dmn_id_bnd=NC_MIN_INT; /* [id] Dimension ID for spatial bounds */
  int dmn_id_col=NC_MIN_INT; /* [id] Dimension ID for unstructured grids */
  int dmn_id_lat=NC_MIN_INT; /* [id] Dimension ID for latitude */
  int dmn_id_lon=NC_MIN_INT; /* [id] Dimension ID for longitude */

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

    rcd=nco_inq_att_flg(in_id,cf->var_id,crd_sng,&att_typ,&att_sz);
    if(rcd == NC_NOERR && att_typ == NC_CHAR){
      cf->crd_sng=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
      rcd+=nco_get_att(in_id,cf->var_id,crd_sng,cf->crd_sng,att_typ);
      /* NUL-terminate attribute before using strstr() */
      cf->crd_sng[att_sz]='\0';
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
	rcd=nco_inq_att_flg(in_id,cf->crd_id[crd_spt],unt_sng,&att_typ,&att_sz);
	if(rcd == NC_NOERR && att_typ == NC_CHAR){
	  cf->unt_sng[crd_spt]=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	  rcd=nco_get_att(in_id,cf->crd_id[crd_spt],unt_sng,cf->unt_sng[crd_spt],att_typ);
	  /* NUL-terminate attribute before using strstr() */
	  *(cf->unt_sng[crd_spt]+att_sz)='\0';
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

      rcd=nco_inq_att_flg(in_id,cf->crd_id[0],unt_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	cf->unt_sng[0]=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd=nco_get_att(in_id,cf->crd_id[0],unt_sng,cf->unt_sng[0],att_typ);
	/* NUL-terminate attribute before using strstr() */
	*(cf->unt_sng[0]+att_sz)='\0';
	if(!strcasestr(cf->unt_sng[0],"degree")) (void)fprintf(stderr,"%s: WARNING %s reports first coordinates variable %s has weird units attribute = %s. May not detect correct ordering of latitude and longitude coordinates\n",nco_prg_nm_get(),fnc_nm,cf->crd_nm[0],cf->unt_sng[0]);
      } /* !rcd && att_typ */
      rcd=nco_inq_att_flg(in_id,cf->crd_id[1],unt_sng,&att_typ,&att_sz);
      if(rcd == NC_NOERR && att_typ == NC_CHAR){
	cf->unt_sng[1]=(char *)nco_malloc((att_sz+1L)*nco_typ_lng(att_typ));
	rcd=nco_get_att(in_id,cf->crd_id[1],unt_sng,cf->unt_sng[1],att_typ);
	/* NUL-terminate attribute before using strstr() */
	*(cf->unt_sng[1]+att_sz)='\0';
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
    /* Subtlety: lat_nm_in is coordinate (variable+dimension) name when specified from command-line (as in nco_grd_nfr()), dimension name when found through CF-method (as in nco_rgr_map()). This confusing distinction could be avoided by passing command-line dimension names through-to nco_rgr_map(). However, that route would require complex priorities for what to do when passing command-line coordinate names not dimension names and visa-versa. */
    //lat_nm_in=strdup(cf->dmn_nm[0]);
    //lon_nm_in=strdup(cf->dmn_nm[1]);
    lat_nm_in=strdup(cf->crd_nm[idx_lat]);
    lon_nm_in=strdup(cf->crd_nm[idx_lon]);
    /* Next four lines unnecessary in nco_rgr_map() which only needs dimension names (it reads input coordinates from map- not data-file) */
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
    else if((rcd=nco_inq_varid_flg(in_id,"Lat",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("Lat");
    else if((rcd=nco_inq_varid_flg(in_id,"XLAT",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("XLAT"); /* WRF */
    else if((rcd=nco_inq_varid_flg(in_id,"XLAT_M",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("XLAT_M"); /* Unknown */
    else if((rcd=nco_inq_varid_flg(in_id,"LAT",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("LAT"); /* MAR/RACMO */
    else if((rcd=nco_inq_varid_flg(in_id,"TLAT",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("TLAT"); /* CICE, POP */
    else if((rcd=nco_inq_varid_flg(in_id,"ULAT",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("ULAT"); /* CICE, POP */
    else if((rcd=nco_inq_varid_flg(in_id,"latCell",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("latCell"); /* MPAS-O/I */
    else if((rcd=nco_inq_varid_flg(in_id,"nav_lat",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("nav_lat"); /* NEMO */
    else if((rcd=nco_inq_varid_flg(in_id,"global_latitude0",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("global_latitude0"); /* Oxford */
    else if((rcd=nco_inq_varid_flg(in_id,"latitude0",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("latitude0"); /* Oxford NB: Must search for global_* first */
    else if((rcd=nco_inq_varid_flg(in_id,"CO_Latitude",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("CO_Latitude"); /* MLS */
    else if((rcd=nco_inq_varid_flg(in_id,"S1_Latitude",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("S1_Latitude"); /* GPM */
    else if((rcd=nco_inq_varid_flg(in_id,"yc",&lat_ctr_id)) == NC_NOERR) lat_nm_in=strdup("yc"); /* RTM */
  } /* !lat_ctr_id */
  
  if(lon_ctr_id == NC_MIN_INT){
    if(rgr->lon_nm_in && (rcd=nco_inq_varid_flg(in_id,rgr->lon_nm_in,&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup(rgr->lon_nm_in);
    else if((rcd=nco_inq_varid_flg(in_id,"longitude",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("longitude");
    else if((rcd=nco_inq_varid_flg(in_id,"Longitude",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("Longitude"); /* AMSR, TRMM */
    else if((rcd=nco_inq_varid_flg(in_id,"lon",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("lon"); /* CAM */
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
    else if((rcd=nco_inq_varid_flg(in_id,"global_longitude0",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("global_longitude0"); /* Oxford NB: Must search for global_* first */
    else if((rcd=nco_inq_varid_flg(in_id,"longitude0",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("longitude0"); /* Oxford */
    else if((rcd=nco_inq_varid_flg(in_id,"CO_Longitude",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("CO_Longitude"); /* MLS */
    else if((rcd=nco_inq_varid_flg(in_id,"S1_Longitude",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("S1_Longitude"); /* GPM */
    else if((rcd=nco_inq_varid_flg(in_id,"xc",&lon_ctr_id)) == NC_NOERR) lon_nm_in=strdup("xc"); /* RTM */
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
     NB: Unstructure already known */
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
    dmn_cnt[0]=col_nbr;
    if(flg_1D_psd_rct_bnd){
      dmn_cnt[1]=bnd_nbr;
      if(lat_bnd_id != NC_MIN_INT) rcd=nco_get_vara(in_id,lat_bnd_id,dmn_srt,dmn_cnt,lat_bnd,crd_typ);
      if(lon_bnd_id != NC_MIN_INT) rcd=nco_get_vara(in_id,lon_bnd_id,dmn_srt,dmn_cnt,lon_bnd,crd_typ);
    }else{
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
	(void)fprintf(stdout,"%s: WARNING %s confused by dimension-ordering of latitude bounds variable \"%s. Will ignore this bounds variable and attempt to extrapolate vertices from centers internally...\n",nco_prg_nm_get(),fnc_nm,lat_nm_in);
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
    dmn_srt[0]=0L;
    dmn_cnt[0]=lat_nbr;
    rcd=nco_get_vara(in_id,lat_ctr_id,dmn_srt,dmn_cnt,lat_ctr,crd_typ);
    dmn_srt[0]=0L;
    dmn_cnt[0]=lon_nbr;
    rcd=nco_get_vara(in_id,lon_ctr_id,dmn_srt,dmn_cnt,lon_ctr,crd_typ);

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
    (void)fprintf(stdout,"%s: ERROR %s filename for inferred SCRIP grid-file is uninitialized, supply it with --rgr grid=filename.nc\n",nco_prg_nm_get(),fnc_nm);
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
      nco_bool flg_ccw; /* [flg] Gridcell is CCW */
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
  if(flg_grd_2D){
    if(flg_dgn_bnd || (lat_bnd_id == NC_MIN_INT && lon_bnd_id == NC_MIN_INT)){
      if(nco_dbg_lvl_get() >= nco_dbg_std && flg_dgn_bnd) (void)fprintf(stdout,"%s: INFO %s will diagnose cell boundaries from cell centers...\n",nco_prg_nm_get(),fnc_nm);
      /* Derive interfaces (ntf) and bounds (bnd) from midpoints approximation applied to center data
	 NB: Simplistically derived interfaces (ntf) only valid on some rectangular grids (not on Gaussian grids)
	 These inferred-from-midpoint interfaces/bounds are overwritten in next block once lat grid is known */
      lat_ntf[0]=lat_ctr[0]-0.5*(lat_ctr[1]-lat_ctr[0]);
      if(lat_ntf[0] < -90.0) lat_ntf[0]=-90.0; /* NB: lat_ntf[0] can be same as lat_ctr[0] for cap grid */
      for(lat_idx=0;lat_idx<lat_nbr-1;lat_idx++)
	lat_ntf[lat_idx+1]=0.5*(lat_ctr[lat_idx]+lat_ctr[lat_idx+1]);
      lat_ntf[lat_nbr]=lat_ctr[lat_nbr-1]+0.5*(lat_ctr[lat_nbr-1]-lat_ctr[lat_nbr-2]);
      if(lat_ntf[lat_nbr] > 90.0) lat_ntf[lat_nbr]=90.0; /* NB: lat_ntf[lat_nbr] can be same as lat_ctr[lat_nbr-1] for cap grid */
      lat_spn=lat_ntf[lat_nbr]-lat_ntf[0];
      
      lon_ntf[0]=lon_ctr[0]-0.5*(lon_ctr[1]-lon_ctr[0]);
      for(lon_idx=0;lon_idx<lon_nbr-1;lon_idx++)
	lon_ntf[lon_idx+1]=0.5*(lon_ctr[lon_idx]+lon_ctr[lon_idx+1]);
      lon_ntf[lon_nbr]=lon_ctr[lon_nbr-1]+0.5*(lon_ctr[lon_nbr-1]-lon_ctr[lon_nbr-2]);
      lon_spn=lon_ntf[lon_nbr]-lon_ntf[0];
      
      for(idx=0;idx<lon_nbr;idx++){
	lon_bnd[2*idx]=lon_ntf[idx];
	lon_bnd[2*idx+1]=lon_ntf[idx+1];
      } /* !idx */
      for(idx=0;idx<lat_nbr;idx++){
	lat_bnd[2*idx]=lat_ntf[idx];
	lat_bnd[2*idx+1]=lat_ntf[idx+1];
      } /* !idx */
    }else{ /* !(lat_bnd_id && lon_bnd_id) */
      /* Derive interfaces (ntf) from bounds (bnd) data on disk */
      for(idx=0;idx<lon_nbr;idx++) lon_ntf[idx]=lon_bnd[2*idx];
      lon_ntf[lon_nbr]=lon_bnd[2*lon_nbr-1];
      for(idx=0;idx<lat_nbr;idx++) lat_ntf[idx]=lat_bnd[2*idx];
      lat_ntf[lat_nbr]=lat_bnd[2*lat_nbr-1];
      lat_spn=lat_ntf[lat_nbr]-lat_ntf[0];
      lon_spn=lon_ntf[lon_nbr]-lon_ntf[0];
    } /* !(lat_bnd_id && lon_bnd_id) */
  } /* !flg_grd_2D */

  if(flg_grd_2D){
    /* Diagnose type of two-dimensional input grid by testing second latitude center against formulae */
    const double lat_ctr_tst_eqa=lat_ntf[0]+lat_spn*1.5/lat_nbr;
    const double lat_ctr_tst_fv=lat_ntf[0]+lat_spn/(lat_nbr-1);
    double lat_ctr_tst_gss;
    /* In diagnosing grids, agreement with input to single-precision is "good enough for government work"
       Hence some comparisons cast from double to float before comparison
       20150526: T42 grid from SCRIP and related maps are only accurate to ~eight digits
       20150611: map_ne120np4_to_fv801x1600_bilin.150418.nc has yc_b[1600]=-89.775000006 not expected exact value lat_ctr[1]=-89.775000000000006
       20170521: T62 grid from NCEP-NCAR Reanalysis 1 worse than single precision, has yc_[192]=-86.6531 not expected exact value lat_ctr[1]=-86.6532 */
    if(lat_ctr[0] > lat_ctr[lat_nbr-1L]) (void)fprintf(stderr,"%s: WARNING %s reports lat_ctr[0] = %g > %g = lat_ctr[%ld]. Grid inferral currently assumes 2D grids run from south-to-north, not north-to-south. N-to-S grids may not be correctly recognized. Please contact us if full support for inferring N-to-S grids is important to you.\nHINT: If present command fails, re-try inferring grid after reversing input dataset's latitude coordinate (with, e.g., ncpdq -a time,-lat,lon in.nc out.nc)\n",nco_prg_nm_get(),fnc_nm,lat_ctr[0],lat_ctr[lat_nbr-1L],lat_nbr-1L);
    if((float)lat_ctr[1] == (float)lat_ctr_tst_eqa) lat_typ=nco_grd_lat_eqa;
    if((float)lat_ctr[1] == (float)lat_ctr_tst_fv) lat_typ=nco_grd_lat_fv;
    double *lat_sin=NULL_CEWI; // [frc] Sine of Gaussian latitudes double precision
    double *wgt_Gss=NULL; // [frc] Gaussian weights double precision
    if(lat_typ == nco_grd_lat_nil){
      /* Check for Gaussian grid */
      lat_sin=(double *)nco_malloc(lat_nbr*sizeof(double));
      wgt_Gss=(double *)nco_malloc(lat_nbr*sizeof(double));
      (void)nco_lat_wgt_gss(lat_nbr,lat_sin,wgt_Gss);
      lat_ctr_tst_gss=rdn2dgr*asin(lat_sin[1]);
      /* Gaussian weights on output grid will be double-precision accurate
	 Grid itself is kept as user-specified so area diagnosed by ESMF_RegridWeightGen may be slightly inconsistent with weights */
      if(nco_dbg_lvl_get() >= nco_dbg_sbr) (void)fprintf(stderr,"%s: INFO %s reports lat_ctr[1] = %g, lat_ctr_tst_gss = %g\n",nco_prg_nm_get(),fnc_nm,lat_ctr[1],lat_ctr_tst_gss);
      const double eps_rlt_cnv_gss=1.0e-7; // Convergence criterion (1.0e-7 fails for NCEP NCAR Reanalysis 1
      if(fabs(1.0-fabs(lat_ctr[1])/lat_ctr_tst_gss) < eps_rlt_cnv_gss) lat_typ=nco_grd_lat_gss;
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
      lat_nrt=lat_ntf[lat_nbr];
      lat_spn=lat_ntf[lat_nbr]-lat_ntf[0];
      switch(lat_typ){
      case nco_grd_lat_fv:
	lat_ncr=lat_spn/(lat_nbr-1);
	lat_ntf[1]=lat_ntf[0]+0.5*lat_ncr;
	for(lat_idx=2;lat_idx<lat_nbr;lat_idx++)
	  lat_ntf[lat_idx]=lat_ntf[1]+(lat_idx-1)*lat_ncr;
	break;
      case nco_grd_lat_eqa:
	lat_ncr=lat_spn/lat_nbr;
	for(lat_idx=1;lat_idx<lat_nbr;lat_idx++)
	  lat_ntf[lat_idx]=lat_ntf[0]+lat_idx*lat_ncr;
	break;
      case nco_grd_lat_gss:
	for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
	  lat_ctr[lat_idx]=rdn2dgr*asin(lat_sin[lat_idx]);
	/* First guess for lat_ntf is midway between Gaussian abscissae */
	for(lat_idx=1;lat_idx<lat_nbr;lat_idx++)
	  lat_ntf[lat_idx]=0.5*(lat_ctr[lat_idx-1]+lat_ctr[lat_idx]);
	/* Iterate guess until area between interfaces matches Gaussian weight */
	for(lat_idx=1;lat_idx<lat_nbr;lat_idx++){
	  double fofx_at_x0; /* [frc] Function to iterate evaluated at current guess */
	  double dfdx_at_x0; /* [frc] Derivation of equation evaluated at current guess */
	  const double eps_rlt_cnv=1.0e-15; // Convergence criterion (1.0e-16 pushes double precision to the brink)
	  itr_cnt=0;
	  lat_wgt_gss=sin(dgr2rdn*lat_ntf[lat_idx])-sin(dgr2rdn*lat_ntf[lat_idx-1]);
	  fofx_at_x0=wgt_Gss[lat_idx-1]-lat_wgt_gss;
	  while(fabs(fofx_at_x0) > eps_rlt_cnv){
	    /* Newton-Raphson iteration:
	       Let x=lat_ntf[lat_idx], y0=lat_ntf[lat_idx-1], gw = Gaussian weight (exact solution)
	       f(x)=sin(dgr2rdn*x)-sin(dgr2rdn*y0)-gw=0 
	       dfdx(x)=dgr2rdn*(dgr2rdn*x)
	       x_better=x0-f(x0)/f'(x0) */
	    dfdx_at_x0=dgr2rdn*cos(dgr2rdn*lat_ntf[lat_idx]);
	    lat_ntf[lat_idx]+=fofx_at_x0/dfdx_at_x0; /* NB: not sure why this is minus not plus but it works :) */
	    lat_wgt_gss=sin(dgr2rdn*lat_ntf[lat_idx])-sin(dgr2rdn*lat_ntf[lat_idx-1]);
	    fofx_at_x0=wgt_Gss[lat_idx-1]-lat_wgt_gss;
	    if(++itr_cnt > itr_nbr_max){
	      (void)fprintf(stdout,"%s: ERROR %s reports no convergence in %d iterations for lat_idx = %ld\n",nco_prg_nm_get(),fnc_nm,itr_nbr_max,lat_idx);
	      nco_exit(EXIT_FAILURE);
	    } /* endif */
	  } /* !while */
	} /* !lat */
	if(lat_sin) lat_sin=(double *)nco_free(lat_sin);
	break;
      case nco_grd_lat_unk:
	/* No generic formula exists so use interfaces already read or diagnosed as midpoints between centers */
	break;
      default:
	nco_dfl_case_generic_err(); break;
      } /* !lat_typ */
      /* Ensure rounding errors do not produce unphysical grid */
      lat_ntf[lat_nbr]=lat_nrt;

      if(lat_typ == nco_grd_lat_gss){
	/* 20170510: First approximation above to exterior interfaces for Gaussian grid are ~ +/-89 degrees
	   Loops below recompute interior interfaces only 
	   Southern- and northern-most interfaces must be explicitly assigned 
	   Inferral test for Gaussian grid _assumes_ global grid 
	   Hence WLOG can assign [-90.0, 90.0] to Gaussian grid exterior boundaries */
	lat_ntf[0]=-90.0;
	lat_ntf[lat_nbr]=90.0;
      } /* !nco_grd_lat_gss */
    } /* !(lat_bnd_id && lon_bnd_id) */
    
    /* Use centers and boundaries to diagnose latitude weights */
    switch(lat_typ){
    case nco_grd_lat_eqa:
    case nco_grd_lat_fv:
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=sin(dgr2rdn*lat_ntf[lat_idx+1])-sin(dgr2rdn*lat_ntf[lat_idx]);
      break;
    case nco_grd_lat_gss:
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=wgt_Gss[lat_idx];
      break;
    case nco_grd_lat_unk:
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++) lat_wgt[lat_idx]=sin(dgr2rdn*lat_ntf[lat_idx+1])-sin(dgr2rdn*lat_ntf[lat_idx]);
      if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING %s reports unknown input latitude grid-type. Guessing that weights for grid of rectangles is OK.\n",nco_prg_nm_get(),fnc_nm);
      break;
    default:
      nco_dfl_case_generic_err(); break;
    } /* !lat_typ */
    
    /* Diagnose type of longitude grid by testing second longitude center against formulae */
    lon_spn=lon_ntf[lon_nbr]-lon_ntf[0];
    lat_spn=lat_ntf[lat_nbr]-lat_ntf[0];
    if((float)lon_spn == 360.0f && (float)lat_spn == 180.0f) nco_grd_xtn=nco_grd_xtn_glb; else nco_grd_xtn=nco_grd_xtn_rgn;
    if(lon_typ == nco_grd_lon_nil){
      if(     (float)lon_ctr[0] ==    0.0f && (float)lon_ctr[1] == (float)(lon_ctr[0]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_Grn_ctr;
      else if((float)lon_ctr[0] == -180.0f && (float)lon_ctr[1] == (float)(lon_ctr[0]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_180_ctr;
      else if((float)lon_ntf[0] ==    0.0f && (float)lon_ntf[1] == (float)(lon_ntf[0]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_Grn_wst;
      else if((float)lon_ntf[0] == -180.0f && (float)lon_ntf[1] == (float)(lon_ntf[0]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_180_wst;
      else if((float)lon_ctr[1] == (float)(lon_ctr[0]+lon_spn/lon_nbr)) lon_typ=nco_grd_lon_bb;
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
    const double eps_rlt_max=1.0e-14; /* [frc] Round-off error tolerance */
    double lat_wgt_ttl_xpc; /* [frc] Expected sum of latitude weights */
    lat_wgt_ttl=0.0;
    for(idx=0;idx<lat_nbr;idx++) lat_wgt_ttl+=lat_wgt[idx];
    lat_wgt_ttl_xpc=sin(dgr2rdn*lat_bnd[2*(lat_nbr-1)+1])-sin(dgr2rdn*lat_bnd[0]);
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
      nco_sph_plg_area(grd_crn_lat,grd_crn_lon,grd_sz_nbr,grd_crn_nbr,area);
    }else if(flg_grd_2D){
      for(lat_idx=0;lat_idx<lat_nbr;lat_idx++)
	for(lon_idx=0;lon_idx<lon_nbr;lon_idx++)
	  area[lat_idx*lon_nbr+lon_idx]=dgr2rdn*(lon_bnd[2*lon_idx+1]-lon_bnd[2*lon_idx])*(sin(dgr2rdn*lat_bnd[2*lat_idx+1])-sin(dgr2rdn*lat_bnd[2*lat_idx]));
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
  fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);

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
  char *att_val;
  
  att_nm=strdup("title");
  if(strstr(rgr->grd_ttl,"None given")){
    const char att_fmt[]="NCO inferred this grid from input file %s";
    att_val=(char *)nco_malloc((strlen(att_fmt)+strlen(rgr->fl_in)+1L)*sizeof(char));
    sprintf(att_val,att_fmt,rgr->fl_in);
  }else{
    att_val=strdup(rgr->grd_ttl);
  } /* !grd_ttl */
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
  att_val=strdup("SCRIP");
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

  att_nm=strdup("latitude_grid_type");
  att_val=strdup(nco_grd_lat_sng(lat_typ));
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
  
  att_nm=strdup("longitude_grid_type");
  att_val=strdup(nco_grd_lon_sng(lon_typ));
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
  
  att_nm=strdup("units");
  att_val=strdup("steradian");
  aed_mtd.att_nm=att_nm;
  aed_mtd.var_nm=area_nm;
  aed_mtd.id=area_id;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  (void)nco_aed_prc(out_id,area_id,aed_mtd);
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);

  att_nm=strdup("units");
  att_val=strdup("degrees");
  aed_mtd.att_nm=att_nm;
  aed_mtd.sz=strlen(att_val);
  aed_mtd.type=NC_CHAR;
  aed_mtd.val.cp=att_val;
  aed_mtd.mode=aed_create;
  /* Add same units attribute to four different variables */
  aed_mtd.var_nm=grd_ctr_lat_nm;
  aed_mtd.id=grd_ctr_lat_id;
  (void)nco_aed_prc(out_id,grd_ctr_lat_id,aed_mtd);
  aed_mtd.var_nm=grd_ctr_lon_nm;
  aed_mtd.id=grd_ctr_lon_id;
  (void)nco_aed_prc(out_id,grd_ctr_lon_id,aed_mtd);
  if(flg_wrt_crn){
    aed_mtd.var_nm=grd_crn_lat_nm;
    aed_mtd.id=grd_crn_lat_id;
    (void)nco_aed_prc(out_id,grd_crn_lat_id,aed_mtd);
    aed_mtd.var_nm=grd_crn_lon_nm;
    aed_mtd.id=grd_crn_lon_id;
    (void)nco_aed_prc(out_id,grd_crn_lon_id,aed_mtd);
  } /* !flg_wrt_crn */
  if(att_nm) att_nm=(char *)nco_free(att_nm);
  if(att_val) att_val=(char *)nco_free(att_val);
  
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
       ncks -O -D 1 --rgr grd_ttl='Equiangular grid 180x360' --rgr skl=${HOME}/skl_180x360.nc --rgr grid=${HOME}/grd_180x360_SCRIP.nc --rgr latlon=180,360#lat_typ=eqa#lon_typ=Grn_ctr ~/nco/data/in.nc ~/foo.nc
       ncks -O -D 1 --rgr infer --rgr ugrid=${HOME}/grd_ugrid.nc --rgr grid=${HOME}/grd_scrip.nc ~/skl_180x360.nc ~/foo.nc
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
    long dg_nbr=NC_MIN_INT64; /* [nbr] Number of edges in mesh */
    long fc_idx; /* [idx] Counting index for faces */
    long nd_idx; /* [idx] Counting index for nodes */
    long nd_nbr=NC_MIN_INT64; /* [nbr] Number of nodes in mesh */
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
	fcx[fc_idx]=0.5*(ndx[fc_nd[idx+1]]+ndx[fc_nd[idx+2]]);
      }else if(fc_idx >= fc_nbr-lon_nbr-1){
	fcx[fc_idx]=0.5*(ndx[fc_nd[idx+0]]+ndx[fc_nd[idx+1]]);
      }else if(fc_nd[idx+3] != mss_val_int_out){
	/* fxm for fcx use nco_lon_crn_avg_brnch() and 3-node version too */
	fcx[fc_idx]=0.25*(ndx[fc_nd[idx+0]]+ndx[fc_nd[idx+1]]+ndx[fc_nd[idx+2]]+ndx[fc_nd[idx+3]]);
      }else{
	abort();
      } /* !fc_idx */
      if(fc_nd[idx+3] != mss_val_int_out) fcy[fc_idx]=0.25*(ndy[fc_nd[idx+0]]+ndy[fc_nd[idx+1]]+ndy[fc_nd[idx+2]]+ndy[fc_nd[idx+3]]); else fcy[fc_idx]=0.33*(ndy[fc_nd[idx+0]]+ndy[fc_nd[idx+1]]+ndy[fc_nd[idx+2]]);
    } /* !fc_idx */

    fl_out_tmp=nco_fl_out_open(fl_out,&FORCE_APPEND,FORCE_OVERWRITE,fl_out_fmt,&bfr_sz_hnt,RAM_CREATE,RAM_OPEN,WRT_TMP_FL,&out_id);

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

    att_nm=strdup("Conventions");
    att_val=strdup("CF-1.6, UGRID-1.0");
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

    att_nm=strdup("cf_role");
    att_val=strdup("mesh_topology");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("mesh_topology");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Topology data");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

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

    att_nm=strdup("node_coordinates");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.type=NC_CHAR;
    aed_mtd.sz=strlen(ndx_nm)+strlen(ndy_nm)+1L;
    att_val=(char *)nco_malloc((aed_mtd.sz+1L)*nco_typ_lng(aed_mtd.type));
    (void)sprintf(att_val,"%s %s",ndx_nm,ndy_nm);
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("face_node_connectivity");
    att_val=strdup(fc_nd_nm);
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("face_coordinates");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.type=NC_CHAR;
    aed_mtd.sz=strlen(fcx_nm)+strlen(fcy_nm)+1L;
    att_val=(char *)nco_malloc((aed_mtd.sz+1L)*nco_typ_lng(aed_mtd.type));
    (void)sprintf(att_val,"%s %s",fcx_nm,fcy_nm);
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("face_dimension");
    att_val=strdup(fc_dmn_nm);
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("edge_node_connectivity");
    att_val=strdup(dg_nd_nm);
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("edge_coordinates");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.type=NC_CHAR;
    aed_mtd.sz=strlen(dgx_nm)+strlen(dgy_nm)+1L;
    att_val=(char *)nco_malloc((aed_mtd.sz+1L)*nco_typ_lng(aed_mtd.type));
    (void)sprintf(att_val,"%s %s",dgx_nm,dgy_nm);
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("edge_dimension");
    att_val=strdup(dg_dmn_nm);
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=msh_nm;
    aed_mtd.id=msh_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,msh_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("longitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=ndx_nm;
    aed_mtd.id=ndx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,ndx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Longitude of mesh nodes");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=ndx_nm;
    aed_mtd.id=ndx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,ndx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("units");
    att_val=strdup("degrees_east");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=ndx_nm;
    aed_mtd.id=ndx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,ndx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("latitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=ndy_nm;
    aed_mtd.id=ndy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,ndy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Latitude of mesh nodes");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=ndy_nm;
    aed_mtd.id=ndy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,ndy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("units");
    att_val=strdup("degrees_north");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=ndy_nm;
    aed_mtd.id=ndy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,ndy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("cf_role");
    att_val=strdup("edge_node_connectivity");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dg_nd_nm;
    aed_mtd.id=dg_nd_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dg_nd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Maps every edge to the two nodes that it connects");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dg_nd_nm;
    aed_mtd.id=dg_nd_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dg_nd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

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

    att_nm=strdup("cf_role");
    att_val=strdup("face_node_connectivity");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fc_nd_nm;
    aed_mtd.id=fc_nd_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fc_nd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Maps every face to its corner nodes");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fc_nd_nm;
    aed_mtd.id=fc_nd_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fc_nd_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

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

    att_nm=strdup("standard_name");
    att_val=strdup("longitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dgx_nm;
    aed_mtd.id=dgx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dgx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Characteristic longitude of 2D mesh face");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dgx_nm;
    aed_mtd.id=dgx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dgx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("units");
    att_val=strdup("degrees_east");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dgx_nm;
    aed_mtd.id=dgx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dgx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("latitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dgy_nm;
    aed_mtd.id=dgy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dgy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Characteristic latitude of 2D mesh face");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dgy_nm;
    aed_mtd.id=dgy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dgy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("units");
    att_val=strdup("degrees_north");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=dgy_nm;
    aed_mtd.id=dgy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,dgy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("longitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fcx_nm;
    aed_mtd.id=fcx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fcx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Characteristic longitude of 2D mesh edge");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fcx_nm;
    aed_mtd.id=fcx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fcx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("units");
    att_val=strdup("degrees_east");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fcx_nm;
    aed_mtd.id=fcx_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fcx_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("standard_name");
    att_val=strdup("latitude");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fcy_nm;
    aed_mtd.id=fcy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fcy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("long_name");
    att_val=strdup("Characteristic latitude of 2D mesh edge");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fcy_nm;
    aed_mtd.id=fcy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fcy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

    att_nm=strdup("units");
    att_val=strdup("degrees_north");
    aed_mtd.att_nm=att_nm;
    aed_mtd.var_nm=fcy_nm;
    aed_mtd.id=fcy_id;
    aed_mtd.sz=strlen(att_val);
    aed_mtd.type=NC_CHAR;
    aed_mtd.val.cp=att_val;
    aed_mtd.mode=aed_create;
    (void)nco_aed_prc(out_id,fcy_id,aed_mtd);
    if(att_nm) att_nm=(char *)nco_free(att_nm);
    if(att_val) att_val=(char *)nco_free(att_val);

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

  /* Free strings */
  if(col_dmn_nm) col_dmn_nm=(char *)nco_free(col_dmn_nm);
  if(lat_dmn_nm) lat_dmn_nm=(char *)nco_free(lat_dmn_nm);
  if(lon_dmn_nm) lon_dmn_nm=(char *)nco_free(lon_dmn_nm);
  if(bnd_dmn_nm) bnd_dmn_nm=(char *)nco_free(bnd_dmn_nm);
  if(lat_nm_in) lat_nm_in=(char *)nco_free(lat_nm_in);
  if(lon_nm_in) lon_nm_in=(char *)nco_free(lon_nm_in);
  if(lat_bnd_nm) lat_bnd_nm=(char *)nco_free(lat_bnd_nm);
  if(lon_bnd_nm) lon_bnd_nm=(char *)nco_free(lon_bnd_nm);
  if(area_nm_in) area_nm_in=(char *)nco_free(area_nm_in);
  if(msk_nm_in) msk_nm_in=(char *)nco_free(msk_nm_in);
  
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

double /* O [dgr] Longitude average (lon_r-lon_l) */
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
