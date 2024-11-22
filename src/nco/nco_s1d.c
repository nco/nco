/* $Header$ */

/* Purpose: NCO utilities for Sparse-1D (S1D) datasets */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_s1d.h" /* Sparse-1D datasets */

const char * /* O [sng] String describing column type */
nco_clm_typ_sng /* [fnc] Convert column-type enum to string */
(const int nco_clm_typ) /* I [enm] Column type enum */
{
  /* Purpose: Convert column-type enum to string */
  switch(nco_clm_typ){
  case nco_clm_icol_nil: return "Unknown, unset, or unclassified landunit type"; /* 0 */
  case nco_clm_icol_vegetated_or_bare_soil: return "Vegetated or bare soil"; /* 1 */
  case nco_clm_icol_crop: return "Crop icol_crop_noncompete: 2*100+m, m=cft_lb,cft_ub"; /* 2 */
  case nco_clm_icol_crop_noncompete_01: return "Crop noncompete type 01"; /* 201 */
  case nco_clm_icol_crop_noncompete_02: return "Crop noncompete type 02"; /* 202 */
  case nco_clm_icol_landice: return "Landice (plain, no MEC)"; /* 3 */
  case nco_clm_icol_landice_multiple_elevation_class_01: return "Landice multiple elevation class 01"; /* 401 */
  case nco_clm_icol_landice_multiple_elevation_class_02: return "Landice multiple elevation class 02"; /* 402 */
  case nco_clm_icol_landice_multiple_elevation_class_03: return "Landice multiple elevation class 03"; /* 403 */
  case nco_clm_icol_landice_multiple_elevation_class_04: return "Landice multiple elevation class 04"; /* 404 */
  case nco_clm_icol_landice_multiple_elevation_class_05: return "Landice multiple elevation class 05"; /* 405 */
  case nco_clm_icol_landice_multiple_elevation_class_06: return "Landice multiple elevation class 06"; /* 406 */
  case nco_clm_icol_landice_multiple_elevation_class_07: return "Landice multiple elevation class 07"; /* 407 */
  case nco_clm_icol_landice_multiple_elevation_class_08: return "Landice multiple elevation class 08"; /* 408 */
  case nco_clm_icol_landice_multiple_elevation_class_09: return "Landice multiple elevation class 09"; /* 409 */
  case nco_clm_icol_landice_multiple_elevation_class_10: return "Landice multiple elevation class 10"; /* 410 */
  case nco_clm_icol_deep_lake: return "Deep lake"; /* 5 */
  case nco_clm_icol_wetland: return "Wetland"; /* 6 */
  case nco_clm_icol_urban_roof: return "Urban roof"; /* 71 */
  case nco_clm_icol_urban_sunwall: return "Urban sunwall"; /* 72 */
  case nco_clm_icol_urban_shadewall: return "Urban shadewall"; /* 73 */
  case nco_clm_icol_urban_impervious_road: return "Urban impervious road"; /* 74 */
  case nco_clm_icol_urban_pervious_road: return "Urban pervious road"; /* 75 */
  default: nco_dfl_case_generic_err((int)nco_clm_typ); break;
  } /* !nco_clm_typ_enm */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_clm_typ_sng() */

const char * /* O [sng] String describing landunit type */
nco_lnd_typ_sng /* [fnc] Convert landunit-type enum to string */
(const int nco_lnd_typ) /* I [enm] Landunit type enum */
{
  /* Purpose: Convert landunit-type enum to string */
  switch(nco_lnd_typ){
  case nco_lnd_ilun_nil: return "Unknown, unset, or unclassified landunit type"; /* 0 */
  case nco_lnd_ilun_vegetated_or_bare_soil: return "Vegetated or bare soil"; /* 1 */
  case nco_lnd_ilun_crop: return "Crop"; /* 2 */
  case nco_lnd_ilun_landice: return "Landice (plain, no MEC)"; /* 3 */
  case nco_lnd_ilun_landice_multiple_elevation_classes: return "Landice multiple elevation classes"; /* 4 */
  case nco_lnd_ilun_deep_lake: return "Deep lake"; /* 5 */
  case nco_lnd_ilun_wetland: return "Wetland"; /* 6 */
  case nco_lnd_ilun_urban_tbd: return "Urban tall building district"; /* 7 */
  case nco_lnd_ilun_urban_hd: return "Urban high density"; /* 8 */
  case nco_lnd_ilun_urban_md: return "Urban medium density"; /* 9 */
  default: nco_dfl_case_generic_err((int)nco_lnd_typ); break;
  } /* !nco_lnd_typ_enm */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_lnd_typ_sng() */

const char * /* O [sng] String describing landunit output type */
nco_lut_out_sng /* [fnc] Convert landunit output type enum to string */
(const int nco_lut_out) /* I [enm] Landunit output type enum */
{
  /* Purpose: Convert landunit output type enum to string */
  switch(nco_lut_out){
  case nco_lut_out_nil: return "Unknown, unset, or unclassified landunit type"; /* 0 */
  case nco_lut_out_vegetated_or_bare_soil: return "Vegetated or bare soil"; /* 1 */
  case nco_lut_out_crop: return "Crop"; /* 2 */
  case nco_lut_out_landice: return "Landice (plain, no MEC)"; /* 3 */
  case nco_lut_out_landice_multiple_elevation_classes: return "Landice multiple elevation classes"; /* 4 */
  case nco_lut_out_deep_lake: return "Deep lake"; /* 5 */
  case nco_lut_out_wetland: return "Wetland"; /* 6 */
  case nco_lut_out_urban_tbd: return "Urban tall building district"; /* 7 */
  case nco_lut_out_urban_hd: return "Urban high density"; /* 8 */
  case nco_lut_out_urban_md: return "Urban medium density"; /* 9 */
  case nco_lut_out_wgt_all: return "Area-weighted average of all landunit types except MEC glaciers"; /* 10 */
  case nco_lut_out_wgt_soi_glc: return "Area-weighted average of soil+(non-MEC) glacier types"; /* 13 */
  default: nco_dfl_case_generic_err((int)nco_lut_out); break;
  } /* !nco_lut_out_enm */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_lut_out_sng() */

char * /* O [sng] String describing PFT type */
nco_pft_typ_sng /* [fnc] Convert PFT-type enum to string */
(const int nco_pft_typ) /* I [enm] PFT type enum */
{
  /* Purpose: Convert PFT-type enum to string */
  switch(nco_pft_typ){
  case nco_pft_ipft_not_vegetated: return "Not vegetated"; /* 0 */
  case nco_pft_ipft_needleleaf_evergreen_temperate_tree: return "Needleleaf evergreen temperate tree"; /* 1 */
  case nco_pft_ipft_needleleaf_evergreen_boreal_tree: return "Needleleaf evergreen boreal tree"; /* 2 */
  case nco_pft_ipft_needleleaf_deciduous_boreal_tree: return "Needleleaf deciduous boreal tree"; /* 3 */
  case nco_pft_ipft_broadleaf_evergreen_tropical_tree: return "Broadleaf evergreen tropical tree"; /* 4 */
  case nco_pft_ipft_broadleaf_evergreen_temperate_tree: return "Broadleaf evergreen temperate tree"; /* 5 */
  case nco_pft_ipft_broadleaf_deciduous_tropical_tree: return "Broadleaf deciduous tropical tree"; /* 6 */
  case nco_pft_ipft_broadleaf_deciduous_temperate_tree: return "Broadleaf deciduous temperate tree"; /* 7 */
  case nco_pft_ipft_broadleaf_deciduous_boreal_tree: return "Broadleaf deciduous boreal tree"; /* 8 */
  case nco_pft_ipft_broadleaf_evergreen_shrub: return "Broadleaf evergreen shrub"; /* 9 */
  case nco_pft_ipft_broadleaf_deciduous_temperate_shrub: return "Broadleaf deciduous temperate shrub"; /* 10 */
  case nco_pft_ipft_broadleaf_deciduous_boreal_shrub: return "Broadleaf deciduous boreal shrub"; /* 11 */
  case nco_pft_ipft_c3_arctic_grass: return "C3 Arctic grass"; /* 12 */
  case nco_pft_ipft_c3_non_arctic_grass: return "C3 non-Arctic grass"; /* 13 */
  case nco_pft_ipft_c4_grass: return "C4 grass"; /* 14 */
    /* 20241017: ELMv3 control includes C3 crop and irrigated in natpft dimension (natpft=17)
       Previously, AFAIK, ELM datasets always had natpft=15
       Also Eva h2, h3 history files still have natpft=15
       These final two enumerates are not accessed unless natpft >= 17 */
  case nco_pft_ipft_c3_crop: return "C3 crop"; /* 15 */
  case nco_pft_ipft_c3_irrigated: return "C3 irrigated"; /* 16 */

  default: nco_dfl_case_generic_err((int)nco_pft_typ); break;
  } /* !nco_pft_typ_enm */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_pft_typ_sng() */

const char * /* O [sng] String describing sparse-type */
nco_s1d_sng /* [fnc] Convert sparse-1D type enum to string */
(const int nco_s1d_typ) /* I [enm] Sparse-1D type enum */
{
  /* Purpose: Convert sparse-type enum to string */
  switch(nco_s1d_typ){
  case nco_s1d_clm: return "Sparse Column (cols1d) format";
  case nco_s1d_grd: return "Sparse Gridcell (grid1d) format";
  case nco_s1d_lnd: return "Sparse Landunit (land1d) format";
  case nco_s1d_pft: return "Sparse PFT (pfts1d) format" ;
  case nco_s1d_tpo: return "Sparse Topounit (topo1d) format" ;
  default: nco_dfl_case_generic_err((int)nco_s1d_typ); break;
  } /* !nco_s1d_typ_enm */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* !nco_s1d_sng() */

int /* O [rcd] Return code */
nco_s1d_unpack /* [fnc] Unpack sparse-1D ELM/CLM variables into full file */
(rgr_sct * const rgr, /* I/O [sct] Regridding structure */
 trv_tbl_sct * const trv_tbl) /* I/O [sct] Traversal Table */
{
  /* Purpose: Read sparse ELM/CLM input file, inflate and write into output file */

  /* Usage:
     ncks -D 1 -O -C --s1d ~/data/bm/elm_mali_bg_hst.nc ~/foo.nc
     ncks -D 1 -O -C --s1d --hrz=${DATA}/bm/elmv3_r05l15.nc ~/data/bm/elmv3_rst_r05l15.nc ~/foo.nc
     ncks -D 1 -O -C --s1d -v cols1d_topoglc --hrz=${DATA}/bm/elm_mali_ig_hst.nc ${DATA}/bm/elm_mali_rst.nc ~/foo.nc
     ncks -D 1 -O -C --s1d -v GPP,pfts1d_wtcell ~/eva_h2.nc ~/foo.nc
     ncremap --dbg=1 --vrb=3 --devnull=No --nco='--dbg=1' -P elm -m ${DATA}/maps/map_ne30np4_to_fv128x256_aave.20160301.nc ~/foo.nc ~/foo_rgr.nc */

  /* 20240920: Unpacking an standard r05 ELM restart file into latxlon format is NOT storage-intensive
     Converting an r05 B simulation on imua took ~1 minute and reduced the filesize from ~14 GB to ~9 GB */

  const char fnc_nm[]="nco_s1d_unpack()"; /* [sng] Function name */

  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped as shared in parallel clause */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  char *fl_in;
  //char *fl_out;
  char *fl_tpl; /* [sng] Template file (contains horizontal grid) */

  char dmn_nm[NC_MAX_NAME]; /* [sng] Dimension name */
  char *grd_nm_in=(char *)strdup("gridcell");
  char *lnd_nm_in=(char *)strdup("landunit"); /* [sng] Canonical landunit sparse dimension name, used in history and restart files */
  // 20241017 fxm: replace dimension name "ltype" with "landunit" in all output files?
  //char *ltype_nm_in=(char *)strdup("ltype"); /* [sng] Gridded landunit dimension name used in history (PCT_LANDUNIT, eva_h2.nc) files */
  char *clm_nm_in=(char *)strdup("column");
  char *mec_nm_in=(char *)strdup("glc_nec"); /* [sng] 20241017: glc_nec is only present in IG/BG restarts as an "orphaned dimension" (no variables use it) so ncks -m does not print/preserve it. Use ncks --orphan to print/preserve it. Or ncdump -h to see it. */
  char *pft_nm_in=(char *)strdup("pft");
  char *pft_ntr_nm_in=(char *)strdup("natpft");
  char *snl_var_nm=(char *)strdup("SNLSNO"); /* [sng] Name of variable containing (negative of) number of snow layers */
  char *tpo_nm_in=(char *)strdup("topounit");

  char *levcan_nm_in=(char *)strdup("levcan");
  char *levgrnd_nm_in=(char *)strdup("levgrnd");
  char *levlak_nm_in=(char *)strdup("levlak");
  char *levsno_nm_in=(char *)strdup("levsno");
  char *levsno1_nm_in=(char *)strdup("levsno1");
  char *levtot_nm_in=(char *)strdup("levtot");
  char *numrad_nm_in=(char *)strdup("numrad");
    
  char *mec_nm_out=(char *)strdup("mecp1");
  char *pft_sng_lng_nm_out=(char *)strdup("pft_sng_lng");
  char *pft_chr_out=NULL; /* [sng] Coordinate array of PFT strings */

  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int in_id; /* I [id] Input netCDF file ID */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;
  int tpl_id; /* [id] Input netCDF file ID (for horizontal grid template) */

  int dmn_id; /* [id] Dimension ID */
  int dmn_idx; /* [idx] Dimension index */
  int dmn_idx_swp; /* [idx] Dimension index */

  long int clm_idx;
  long int clm_idx_out;
  long int grd_idx_out;
  long int idx_s1d_crr; /* [idx] Current valid index into S1D arrays */
  //long int lat_idx;
  //long int lon_idx;
  long int lnd_idx;
  long int lnd_idx_out;
  long int mec_idx_in;
  long int mec_idx_out;
  long int pft_idx;
  long int tpo_idx;

  nco_bool *lut_vld_flg=NULL; /* [flg] Landunit types for which columns in this variable contain valid values */
  nco_string *pft_sng_out=NULL; /* [sng] Coordinate array of PFT strings */
  nco_bool flg_var_mpt; /* [flg] Variable has no valid values */
  nco_bool flg_snw_ocn=rgr->flg_snw_ocn; /* [flg] Unpack S1D snow fields into sane (ocean-like) level order */
  nco_bool flg_snw_rdr; /* [flg] Re-arrange snow layer order for this variable using snl_var */
  nco_lut_out_enm lut_out=(nco_lut_out_enm)(rgr->lut_out); /* [enm] Landunit type(s) for S1D column output */
  
  size_t idx_dbg=rgr->idx_dbg; /* [idx] User-specifiable debugging location */
  size_t idx_in; /* [idx] Input grid index */
  size_t idx_out; /* [idx] Output grid index */
  
  /* Initialize local copies of command-line values */
  fl_in=rgr->fl_in;
  //fl_out=rgr->fl_out;
  in_id=rgr->in_id;
  out_id=rgr->out_id;

  /* Search for horizontal grid */
  char *bnd_nm_in=rgr->bnd_nm; /* [sng] Name to recognize as input horizontal spatial dimension on unstructured grid */
  char *col_nm_in=rgr->col_nm_in; /* [sng] Name to recognize as input horizontal spatial dimension on unstructured grid */
  char *lat_nm_in=rgr->lat_nm_in; /* [sng] Name of input dimension to recognize as latitude */
  char *lon_nm_in=rgr->lon_nm_in; /* [sng] Name of input dimension to recognize as longitude */
  int dmn_id_bnd_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_col_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lat_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lon_in=NC_MIN_INT; /* [id] Dimension ID */

  nco_bool FL_RTR_RMT_LCN;
  nco_bool flg_grd_1D=False; /* [flg] Unpacked data are on unstructured (1D) grid */
  nco_bool flg_grd_2D=False; /* [flg] Unpacked data are on rectangular (2D) grid */
  nco_bool flg_grd_dat=False; /* [flg] Use horizontal grid from required input data file */
  nco_bool flg_grd_tpl=False; /* [flg] Use horizontal grid from optional horizontal grid template file */
  nco_bool flg_nm_hst=False; /* [flg] Names in data file are as in history files ("ltype_"...) */
  nco_bool flg_nm_rst=False; /* [flg] Names in data file are as in restart files ("ilun_"...) */

  /* Does data file have unstructured grid?
     MB: Routine must handle two semantically distinct meanings of "column":
     1. The horizontal dimension in an unstructured grid
     2. A fraction of a landunit, which is itself a fraction of a CTSM/ELM gridcell
        In particular, a column is a fraction of a vegetated, urban, glacier, or crop landunit
     We distinguish these meanings by abbreviating definition (1) as "col" and definition (2) as "clm" 
     This usage maintains the precedent that "col" is the horizontal unstructured dimension in nco_rgr.c
     It is necessary though unintuitive that "cols1d" variable metadata will use the "clm" abbreviation */
  if(col_nm_in && (rcd=nco_inq_dimid_flg(in_id,col_nm_in,&dmn_id_col_in)) == NC_NOERR) /* do nothing */; 
  else if((rcd=nco_inq_dimid_flg(in_id,"lndgrid",&dmn_id_col_in)) == NC_NOERR) col_nm_in=strdup("lndgrid"); /* CLM */
  if(dmn_id_col_in != NC_MIN_INT) flg_grd_1D=True;

  /* Does data file have RLL grid? */
  if(!flg_grd_1D){
    if(lat_nm_in && (rcd=nco_inq_dimid_flg(in_id,lat_nm_in,&dmn_id_lat_in)) == NC_NOERR) /* do nothing */; 
    else if((rcd=nco_inq_dimid_flg(in_id,"latitude",&dmn_id_lat_in)) == NC_NOERR) lat_nm_in=strdup("lndgrid"); /* CF */
    if(lon_nm_in && (rcd=nco_inq_dimid_flg(in_id,lon_nm_in,&dmn_id_lon_in)) == NC_NOERR) /* do nothing */; 
    else if((rcd=nco_inq_dimid_flg(in_id,"longitude",&dmn_id_lon_in)) == NC_NOERR) lon_nm_in=strdup("lndgrid"); /* CF */
  } /* !flg_grd_1D */
  if(dmn_id_lat_in != NC_MIN_INT && dmn_id_lon_in != NC_MIN_INT) flg_grd_2D=True;

  /* Set where to obtain horizontal grid */
  if(flg_grd_1D || flg_grd_2D) flg_grd_dat=True; else flg_grd_tpl=True;

  if(flg_grd_tpl && !rgr->fl_hrz){
    (void)fprintf(stderr,"%s: ERROR %s did not locate horizontal grid in input data file and no optional horizontal gridfile was provided.\nHINT: Use option --hrz to specify file with horizontal grid used by input data.\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !flg_grd_tpl */

  /* Open grid template file iff necessary */
  if(flg_grd_tpl && rgr->fl_hrz){
    char *fl_pth_lcl=NULL;

    nco_bool HPSS_TRY=False; /* [flg] Search HPSS for unfound files */
    nco_bool RAM_OPEN=False; /* [flg] Open (netCDF3-only) file(s) in RAM */
    nco_bool SHARE_OPEN=rgr->flg_uio; /* [flg] Open (netCDF3-only) file(s) with unbuffered I/O */

    size_t bfr_sz_hnt=NC_SIZEHINT_DEFAULT; /* [B] Buffer size hint */
  
    /* Duplicate (because nco_fl_mk_lcl() free()'s its fl_in) */
    fl_tpl=(char *)strdup(rgr->fl_hrz);
    /* Make sure file is on local system and is readable or die trying */
    fl_tpl=nco_fl_mk_lcl(fl_tpl,fl_pth_lcl,HPSS_TRY,&FL_RTR_RMT_LCN);
    /* Open file using appropriate buffer size hints and verbosity */
    if(RAM_OPEN) md_open=NC_NOWRITE|NC_DISKLESS; else md_open=NC_NOWRITE;
    if(SHARE_OPEN) md_open=md_open|NC_SHARE;
    rcd+=nco_fl_open(fl_tpl,md_open,&bfr_sz_hnt,&tpl_id);

    /* Same logic used to search for grid in data file and to search for grid in template file...
       Does template file have unstructured grid? */
    if(col_nm_in && (rcd=nco_inq_dimid_flg(tpl_id,col_nm_in,&dmn_id_col_in)) == NC_NOERR) /* do nothing */; 
    else if((rcd=nco_inq_dimid_flg(tpl_id,"lndgrid",&dmn_id_col_in)) == NC_NOERR) col_nm_in=strdup("lndgrid"); /* CLM */
    if(dmn_id_col_in != NC_MIN_INT) flg_grd_1D=True;

    /* Does template file have RLL grid? */
    if(!flg_grd_1D){
      if(lat_nm_in && (rcd=nco_inq_dimid_flg(tpl_id,lat_nm_in,&dmn_id_lat_in)) == NC_NOERR) /* do nothing */; 
      else if((rcd=nco_inq_dimid_flg(tpl_id,"latitude",&dmn_id_lat_in)) == NC_NOERR) lat_nm_in=strdup("latitude"); /* CF */
      if(lon_nm_in && (rcd=nco_inq_dimid_flg(tpl_id,lon_nm_in,&dmn_id_lon_in)) == NC_NOERR) /* do nothing */; 
      else if((rcd=nco_inq_dimid_flg(tpl_id,"longitude",&dmn_id_lon_in)) == NC_NOERR) lon_nm_in=strdup("longitude"); /* CF */
    } /* !flg_grd_1D */
    if(dmn_id_lat_in != NC_MIN_INT && dmn_id_lon_in != NC_MIN_INT) flg_grd_2D=True;

    /* Set where to obtain horizontal grid */
    if(!flg_grd_1D && !flg_grd_2D){
      (void)fprintf(stderr,"%s: ERROR %s did not locate horizontal grid in input data file %s or in template file %s.\nHINT: One of those files must contain the grid dimensions and coordinates used by the packed data in the input data file.\n",nco_prg_nm_get(),fnc_nm,fl_in,fl_tpl);
      nco_exit(EXIT_FAILURE);
    } /* !flg_grd_1D */

  } /* !flg_grd_tpl */

  int cols1d_active_id=NC_MIN_INT; /* [id] Column active flag (1=active, 0=inactive) */
  int cols1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index (1-based) of column */
  int cols1d_ixy_id=NC_MIN_INT; /* [id] Column 2D longitude index (1-based) */
  int cols1d_jxy_id=NC_MIN_INT; /* [id] Column 2D latitude index (1-based) */
  int cols1d_lat_id=NC_MIN_INT; /* [id] Column latitude */
  int cols1d_lon_id=NC_MIN_INT; /* [id] Column longitude */
  int cols1d_ityp_id=NC_MIN_INT; /* [id] Column type */
  int cols1d_ityplun_id=NC_MIN_INT; /* [id] Column landunit type */
  int cols1d_wtxy_id=NC_MIN_INT; /* [id] Column weight relative to corresponding gridcell */
  int snl_var_id=NC_MIN_INT; /* [id] Negative of number of snow layers */

  int grid1d_ixy_id=NC_MIN_INT; /* [id] Gridcell 2D longitude index (1-based) */
  int grid1d_jxy_id=NC_MIN_INT; /* [id] Gridcell 2D latitude index (1-based) */
  int grid1d_lat_id=NC_MIN_INT; /* [id] Gridcell latitude */
  int grid1d_lon_id=NC_MIN_INT; /* [id] Gridcell longitude */

  int land1d_active_id=NC_MIN_INT; /* [id] Landunit active flag (1=active, 0=inactive) */
  int land1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of landunit (1-based) */
  int land1d_ityplun_id=NC_MIN_INT; /* [id] Landunit type */
  int land1d_ixy_id=NC_MIN_INT; /* [id] Landunit 2D longitude index (1-based) */
  int land1d_jxy_id=NC_MIN_INT; /* [id] Landunit 2D latitude index (1-based) */
  int land1d_lat_id=NC_MIN_INT; /* [id] Landunit latitude */
  int land1d_lon_id=NC_MIN_INT; /* [id] Landunit longitude */
  int land1d_wtxy_id=NC_MIN_INT; /* [id] Landunit weight relative to corresponding gridcell */

  int pfts1d_active_id=NC_MIN_INT; /* [id] PFT active flag (1=active, 0=inactive) */
  int pfts1d_column_index_id=NC_MIN_INT; /* [id] Column index of PFT (1-based) */
  int pfts1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of PFT (1-based) */
  int pfts1d_ityp_veg_id=NC_MIN_INT; /* [id] PFT vegetation type */
  int pfts1d_ityplun_id=NC_MIN_INT; /* [id] PFT landunit type */
  int pfts1d_ixy_id=NC_MIN_INT; /* [id] PFT 2D longitude index (1-based) */
  int pfts1d_jxy_id=NC_MIN_INT; /* [id] PFT 2D latitude index (1-based) */
  int pfts1d_lat_id=NC_MIN_INT; /* [id] PFT latitude */
  int pfts1d_lon_id=NC_MIN_INT; /* [id] PFT longitude */
  //int pfts1d_wtcell_id=NC_MIN_INT; /* [id] PFT weight relative to corresponding gridcell */
  
  int topo1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of topounit (1-based) */
  int topo1d_ixy_id=NC_MIN_INT; /* [id] Topounit 2D longitude index (1-based) */
  int topo1d_jxy_id=NC_MIN_INT; /* [id] Topounit 2D latitude index (1-based) */
  int topo1d_lat_id=NC_MIN_INT; /* [id] Topounit latitude */
  int topo1d_lon_id=NC_MIN_INT; /* [id] Topounit longitude */

  int dmn_id_clm_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_grd_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lnd_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_mec_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_pft_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_pft_ntr_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_tpo_in=NC_MIN_INT; /* [id] Dimension ID */

  nco_bool flg_s1d_clm=False; /* [flg] Dataset contains sparse variables for columns */
  nco_bool flg_s1d_grd=False; /* [flg] Dataset contains sparse variables for gridcells */
  nco_bool flg_s1d_lnd=False; /* [flg] Dataset contains sparse variables for landunits */
  nco_bool flg_s1d_pft=False; /* [flg] Dataset contains sparse variables for PFTs */
  nco_bool flg_s1d_tpo=False; /* [flg] Dataset contains sparse variables for topounits */

  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,"ltype_vegetated_or_bare_soil",(nc_type *)NULL,(long *)NULL);
  if(rcd == NC_NOERR) flg_nm_hst=True;
  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,"ilun_vegetated_or_bare_soil",(nc_type *)NULL,(long *)NULL);
  if(rcd == NC_NOERR) flg_nm_rst=True;
  assert(!(flg_nm_hst && flg_nm_rst));
  if(!flg_nm_hst && !flg_nm_rst){
    (void)fprintf(stderr,"%s: ERROR %s reports input data file lacks expected global attributes\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !flg_nm_hst */
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s will assume input attributes and variables use ELM/CLM %s naming conventions like %s\n",nco_prg_nm_get(),fnc_nm,flg_nm_hst ? "history file" : "restart file",flg_nm_hst ? "\"ltype_...\"" : "\"ilun_...\"");
  if(nco_dbg_lvl_get() >= nco_dbg_std && flg_nm_rst) (void)fprintf(stderr,"%s: INFO %s will output landunit type (LUT) %d = \"%s\" for non-MEC columns and in index 0 of MEC dimension for MEC columns. A single LUT will contain the value of the last column of that LUT in the gridcell. Area-weighted averages will be over all columns of the requested LUTs in the gridcell.\n",nco_prg_nm_get(),fnc_nm,(int)lut_out,nco_lut_out_sng(lut_out));
  if(nco_dbg_lvl_get() >= nco_dbg_std && flg_nm_rst){
    if(flg_snw_ocn) (void)fprintf(stderr,"%s: INFO %s will unpack multi-layer snow fields so the top layer is at index 0 of the levsno dimension in the output file, with deeper layers arranged subsequently. This shifts the empty cells for non-utilized snowpack layers from the top to the bottom of the output (analogous to bottom topography masking the lower layers of z-grid ocean datasets)\n",nco_prg_nm_get(),fnc_nm); else (void)fprintf(stderr,"%s: INFO %s will unpack multi-layer snow fields into their raw storage order, where active snow layers abut the end of the levsno dimension, and any empty, deeper layers appear at the beginning of the levsno dimension. This is non-intuitive, yet ensures the snow hydrological transport is contiguous with the underlying soil levels.\n",nco_prg_nm_get(),fnc_nm);
  } /* !dbg */

  rcd=nco_inq_varid_flg(in_id,"cols1d_lat",&cols1d_lat_id);
  if(cols1d_lat_id != NC_MIN_INT) flg_s1d_clm=True;
  if(flg_s1d_clm){
    rcd=nco_inq_varid(in_id,"cols1d_ixy",&cols1d_ixy_id);
    rcd=nco_inq_varid(in_id,"cols1d_jxy",&cols1d_jxy_id);
    rcd=nco_inq_varid(in_id,"cols1d_lon",&cols1d_lon_id);
    rcd=nco_inq_varid_flg(in_id,"cols1d_active",&cols1d_active_id); /* ELM/MALI restart */
    rcd=nco_inq_varid_flg(in_id,"cols1d_gridcell_index",&cols1d_gridcell_index_id); /* ELM/MALI restart */
    /* 20241105: eva_h3.nc has all cols1d variables _except_ cols1d_ityp...what to do? */
    rcd=nco_inq_varid_flg(in_id,"cols1d_ityp",&cols1d_ityp_id); /* ELM/MALI restart */

    if(flg_nm_hst) rcd=nco_inq_varid(in_id,"cols1d_itype_lunit",&cols1d_ityplun_id); else rcd=nco_inq_varid(in_id,"cols1d_ityplun",&cols1d_ityplun_id);
    if(flg_nm_hst) rcd=nco_inq_varid_flg(in_id,"cols1d_wtlunit",&cols1d_wtxy_id); else rcd=nco_inq_varid_flg(in_id,"cols1d_wtxy",&cols1d_wtxy_id);

  } /* !flg_s1d_clm */
     
  rcd=nco_inq_varid_flg(in_id,"grid1d_lat",&grid1d_lat_id);
  if(grid1d_lat_id != NC_MIN_INT) flg_s1d_grd=True;
  if(flg_s1d_grd){
    rcd=nco_inq_varid(in_id,"grid1d_ixy",&grid1d_ixy_id);
    rcd=nco_inq_varid(in_id,"grid1d_jxy",&grid1d_jxy_id);
    rcd=nco_inq_varid(in_id,"grid1d_lon",&grid1d_lon_id);
    /* NB: 20241017: grid1d_[ij]xy are only needed when topo1d_[ij]xy are invalid
     And in that case, we read grid1d_[ij]xy values directly into topo1d_[ij]xy */
    //int *grid1d_ixy=NULL; /* [idx] Gridcell 2D longitude index */
    //int *grid1d_jxy=NULL; /* [idx] Gridcell 2D latitude index */
  } /* !flg_s1d_grd */
     
  rcd=nco_inq_varid_flg(in_id,"land1d_lat",&land1d_lat_id);
  if(land1d_lat_id != NC_MIN_INT) flg_s1d_lnd=True;
  if(flg_s1d_lnd){
    rcd=nco_inq_varid(in_id,"land1d_ixy",&land1d_ixy_id);
    rcd=nco_inq_varid(in_id,"land1d_jxy",&land1d_jxy_id);
    rcd=nco_inq_varid(in_id,"land1d_lon",&land1d_lon_id);
    rcd=nco_inq_varid_flg(in_id,"land1d_active",&land1d_active_id);
    rcd=nco_inq_varid_flg(in_id,"land1d_gridcell_index",&land1d_gridcell_index_id);

    if(flg_nm_hst) rcd=nco_inq_varid_flg(in_id,"land1d_wtlunit",&land1d_wtxy_id); else rcd=nco_inq_varid_flg(in_id,"land1d_wtxy",&land1d_wtxy_id);
    /* beth_in.nc does not contain land1d_itype_lunit, whose presence should therefore be optional */
    if(flg_nm_hst) rcd=nco_inq_varid(in_id,"land1d_ityplunit",&land1d_ityplun_id); else rcd=nco_inq_varid(in_id,"land1d_ityplun",&land1d_ityplun_id);
  } /* !flg_s1d_lnd */
     
  rcd=nco_inq_varid_flg(in_id,"pfts1d_lat",&pfts1d_lat_id);
  if(pfts1d_lat_id != NC_MIN_INT) flg_s1d_pft=True;
  if(flg_s1d_pft){
    rcd=nco_inq_varid(in_id,"pfts1d_ixy",&pfts1d_ixy_id);
    rcd=nco_inq_varid(in_id,"pfts1d_jxy",&pfts1d_jxy_id);
    rcd=nco_inq_varid(in_id,"pfts1d_lon",&pfts1d_lon_id);
    rcd=nco_inq_varid_flg(in_id,"pfts1d_active",&pfts1d_active_id);
    rcd=nco_inq_varid_flg(in_id,"pfts1d_column_index",&pfts1d_column_index_id);
    rcd=nco_inq_varid_flg(in_id,"pfts1d_gridcell_index",&pfts1d_gridcell_index_id);
    //if(flg_nm_hst) rcd=nco_inq_varid(in_id,"pfts1d_wtxy",&pfts1d_wtxy_id); else rcd=nco_inq_varid(in_id,"pfts1d_wtxy",&pfts1d_wtxy_id);
    if(flg_nm_hst) rcd=nco_inq_varid(in_id,"pfts1d_itype_lunit",&pfts1d_ityplun_id); else rcd=nco_inq_varid(in_id,"pfts1d_ityplun",&pfts1d_ityplun_id);
    if(flg_nm_hst) rcd=nco_inq_varid(in_id,"pfts1d_itype_veg",&pfts1d_ityp_veg_id); else rcd=nco_inq_varid(in_id,"pfts1d_itypveg",&pfts1d_ityp_veg_id);
  } /* !flg_s1d_pft */
  
  rcd=nco_inq_varid_flg(in_id,"topo1d_lat",&topo1d_lat_id);
  if(topo1d_lat_id != NC_MIN_INT) flg_s1d_tpo=True;
  if(flg_s1d_tpo){
    rcd=nco_inq_varid(in_id,"topo1d_ixy",&topo1d_ixy_id);
    rcd=nco_inq_varid(in_id,"topo1d_jxy",&topo1d_jxy_id);
    rcd=nco_inq_varid(in_id,"topo1d_lon",&topo1d_lon_id);
    rcd=nco_inq_varid_flg(in_id,"topo1d_gridcell_index",&topo1d_gridcell_index_id);
  } /* !flg_s1d_tpo */
  
  if(!(flg_s1d_clm || flg_s1d_lnd || flg_s1d_pft || flg_s1d_tpo)){
    (void)fprintf(stderr,"%s: ERROR %s does not detect any of the key variables (currently cols1d_lat, land1d_lat, pfts1d_lat, topo1d_lat) used to indicate presence of sparse-packed (S1D) variables\nHINT: Be sure the target dataset (file) contains S1D variables---not all ELM/CLM history (as opposed to restart) files do. For example, default ELM/CLM h0 (monthly) history files typically contain no S1D variables.\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !flg_s1d_clm... */

  if(flg_s1d_clm) rcd=nco_inq_dimid(in_id,clm_nm_in,&dmn_id_clm_in);
  if(flg_s1d_grd) rcd=nco_inq_dimid(in_id,grd_nm_in,&dmn_id_grd_in);
  if(flg_s1d_lnd) rcd=nco_inq_dimid(in_id,lnd_nm_in,&dmn_id_lnd_in);
  if(flg_s1d_pft) rcd=nco_inq_dimid(in_id,pft_nm_in,&dmn_id_pft_in);
  if(flg_s1d_tpo) rcd=nco_inq_dimid(in_id,tpo_nm_in,&dmn_id_tpo_in);

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s necessary information to unpack cols1d variables\n",nco_prg_nm_get(),flg_s1d_clm ? "Found all" : "Could not find");
    (void)fprintf(stderr,"%s: INFO %s necessary information to unpack land1d variables\n",nco_prg_nm_get(),flg_s1d_lnd ? "Found all" : "Could not find");
    (void)fprintf(stderr,"%s: INFO %s necessary information to unpack pfts1d variables\n",nco_prg_nm_get(),flg_s1d_pft ? "Found all" : "Could not find");
    (void)fprintf(stderr,"%s: INFO %s necessary information to unpack topo1d variables\n",nco_prg_nm_get(),flg_s1d_tpo ? "Found all" : "Could not find");
  } /* !dbg */

  /* Collect other information from data and template files */
  int dmn_nbr_in; /* [nbr] Number of dimensions in input file */
  int dmn_nbr_out; /* [nbr] Number of dimensions in output file */
  int var_nbr; /* [nbr] Number of variables in file */
  rcd=nco_inq(in_id,&dmn_nbr_in,&var_nbr,(int *)NULL,(int *)NULL);

  const unsigned int trv_nbr=trv_tbl->nbr; /* [idx] Number of traversal table entries */
  int var_cpy_nbr=0; /* [nbr] Number of copied variables */
  int var_rgr_nbr=0; /* [nbr] Number of unpacked variables */
  int var_xcl_nbr=0; /* [nbr] Number of deleted variables */
  int var_crt_nbr=0; /* [nbr] Number of created variables */
  unsigned int idx_tbl; /* [idx] Counter for traversal table */

  char *dmn_nm_cp; /* [sng] Dimension name as char * to reduce indirection */
  nco_bool has_clm; /* [flg] Variable contains column dimension */
  nco_bool has_grd; /* [flg] Variable contains gridcell dimension */
  nco_bool has_lnd; /* [flg] Variable contains landunit dimension */
  nco_bool has_mec; /* [flg] Variable contains (implicit, unrolled) MEC dimension */
  nco_bool has_pft; /* [flg] Variable contains PFT dimension */
  nco_bool has_tpo; /* [flg] Variable contains topounit dimension */
  nco_bool need_clm=False; /* [flg] At least one variable to unpack needs column dimension */
  nco_bool need_grd=False; /* [flg] At least one variable to unpack needs gridcell dimension */
  nco_bool need_lnd=False; /* [flg] At least one variable to unpack needs landunit dimension */
  nco_bool need_mec=False; /* [flg] At least one variable to unpack needs MEC dimension */
  nco_bool need_pft=False; /* [flg] At least one variable to unpack needs PFT dimension */ 
  nco_bool need_tpo=False; /* [flg] At least one variable to unpack needs topounit dimension */ 

  nco_bool has_levcan=False; /* [flg] Variable contains levcan dimension */
  nco_bool has_levgrnd=False; /* [flg] Variable contains levgrnd dimension */
  nco_bool has_levlak=False; /* [flg] Variable contains levlak dimension */
  nco_bool has_levsno=False; /* [flg] Variable contains levsno dimension */
  nco_bool has_levsno1=False; /* [flg] Variable contains levsno1 dimension */
  nco_bool has_levtot=False; /* [flg] Variable contains levtot dimension */
  nco_bool has_numrad=False; /* [flg] Variable contains numrad dimension */
  nco_bool need_levcan=False; /* [flg] At least one variable to unpack needs levcan dimension */
  nco_bool need_levgrnd=False; /* [flg] At least one variable to unpack needs levgrnd dimension */
  nco_bool need_levlak=False; /* [flg] At least one variable to unpack needs levlak dimension */
  nco_bool need_levsno=False; /* [flg] At least one variable to unpack needs levsno dimension */
  nco_bool need_levsno1=False; /* [flg] At least one variable to unpack needs levsno1 dimension */
  nco_bool need_levtot=False; /* [flg] At least one variable to unpack needs levtot dimension */
  nco_bool need_numrad=False; /* [flg] At least one variable to unpack needs numrad dimension */

  trv_sct trv; /* [sct] Traversal table object structure to reduce indirection */
  /* Define unpacking flag for each variable */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      dmn_nbr_in=trv_tbl->lst[idx_tbl].nbr_dmn;
      has_clm=has_grd=has_lnd=has_pft=has_tpo=False;
      has_levcan=has_levgrnd=has_levlak=has_levsno=has_levsno1=has_levtot=has_numrad=False;
      for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	/* Pre-determine flags necessary during next loop */
	dmn_nm_cp=trv.var_dmn[dmn_idx].dmn_nm;
	if(!has_clm && clm_nm_in) has_clm=!strcmp(dmn_nm_cp,clm_nm_in);
	if(!has_grd && grd_nm_in) has_grd=!strcmp(dmn_nm_cp,grd_nm_in);
	if(!has_lnd && lnd_nm_in) has_lnd=!strcmp(dmn_nm_cp,lnd_nm_in);
	if(!has_pft && pft_nm_in) has_pft=!strcmp(dmn_nm_cp,pft_nm_in);
	if(!has_tpo && tpo_nm_in) has_tpo=!strcmp(dmn_nm_cp,tpo_nm_in);

	if(!has_levcan && levcan_nm_in) has_levcan=!strcmp(dmn_nm_cp,levcan_nm_in);
	if(!has_levgrnd && levgrnd_nm_in) has_levgrnd=!strcmp(dmn_nm_cp,levgrnd_nm_in);
	if(!has_levlak && levlak_nm_in) has_levlak=!strcmp(dmn_nm_cp,levlak_nm_in);
	if(!has_levsno && levsno_nm_in) has_levsno=!strcmp(dmn_nm_cp,levsno_nm_in);
	if(!has_levsno1 && levsno1_nm_in) has_levsno1=!strcmp(dmn_nm_cp,levsno1_nm_in);
	if(!has_levtot && levtot_nm_in) has_levtot=!strcmp(dmn_nm_cp,levtot_nm_in);
	if(!has_numrad && numrad_nm_in) has_numrad=!strcmp(dmn_nm_cp,numrad_nm_in);
      } /* !dmn_idx */
      /* Unpack variables that contain a sparse-1D dimension */
      if(has_clm || has_grd || has_lnd || has_pft || has_tpo){
	trv_tbl->lst[idx_tbl].flg_rgr=True;
	var_rgr_nbr++;
	if(has_clm) need_clm=True;
	if(has_grd) need_grd=True;
	if(has_lnd) need_lnd=True;
	if(has_pft) need_pft=True;
	if(has_tpo) need_tpo=True;

	if(has_levcan) need_levcan=True;
	if(has_levgrnd) need_levgrnd=True;
	if(has_levlak) need_levlak=True;
	if(has_levsno) need_levsno=True;
	if(has_levsno1) need_levsno1=True;
	if(has_levtot) need_levtot=True;
	if(has_numrad) need_numrad=True;
      } /* !has_clm... */
      /* Copy all variables that are not regridded or omitted */
      if(!trv_tbl->lst[idx_tbl].flg_rgr) var_cpy_nbr++;
    } /* !nco_obj_typ_var */
  } /* !idx_tbl */
  if(!var_rgr_nbr) (void)fprintf(stdout,"%s: WARNING %s reports no variables fit unpacking criteria. The sparse data unpacker expects at least one variable to unpack, and copies other variables straight to output. HINT: If the name(s) of the input sparse-1D dimensions (e.g., \"column\", \"landunit\", \"pft\", and \"topounit\") do not match NCO's preset defaults (case-insensitive unambiguous forms and abbreviations of \"column\", \"landunit\", and/or \"pft\", respectively) then change the dimension names that NCO looks for. Instructions are at http://nco.sf.net/nco.html#sparse. For CTSM/ELM sparse-1D coordinate grids, the \"column\", \"landunit\", \"pft\", and \"topounit\" variable names can be set with, e.g., \"ncks --rgr column_nm=clm#landunit_nm=lnd#pft_nm=pft#topounit_nm=tpo\" or \"ncremap -R '--rgr clm=clm#lnd=lnd#pft=pft#tpo=tpo'\".\n",nco_prg_nm_get(),fnc_nm);
  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
      trv=trv_tbl->lst[idx_tbl];
      if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr) (void)fprintf(stderr,"Unpack %s? %s\n",trv.nm,trv.flg_rgr ? "Yes" : "No");
    } /* !idx_tbl */
  } /* !dbg */

  long clm_nbr_in=NC_MIN_INT; /* [nbr] Number of columns in input data */
  long grd_nbr_in=NC_MIN_INT; /* [nbr] Number of gridcells in input data */
  long lnd_nbr_in=NC_MIN_INT; /* [nbr] Number of landunits in input data */
  long mec_nbr_in=NC_MIN_INT; /* [nbr] Number of MECs in input data */
  long pft_nbr_in=NC_MIN_INT; /* [nbr] Number of PFTs in input data */
  long pft_ntr_nbr_in=NC_MIN_INT; /* [nbr] Number of natural PFTs in input data */
  long tpo_nbr_in=NC_MIN_INT; /* [nbr] Number of topounits in input data */
  long clm_nbr_out=NC_MIN_INT; /* [nbr] Number of columns in output data */
  long grd_nbr_out=NC_MIN_INT; /* [nbr] Number of gridcells in output data */
  long lnd_nbr_out=NC_MIN_INT; /* [nbr] Number of landunits in output data */
  long mec_nbr_out=NC_MIN_INT; /* [nbr] Number of MECs in output data */
  long mec_nbr_outp1=NC_MIN_INT; /* [nbr] Number of MECs+1 in output data */
  long pft_nbr_out=NC_MIN_INT; /* [nbr] Number of PFTs in output data */
  long pft_crp_nbr_out=NC_MIN_INT; /* [nbr] Number of crop PFTs in output data */
  long pft_ntr_nbr_out=NC_MIN_INT; /* [nbr] Number of natural PFTs in output data */
  long pft_sng_lng_out=NC_MIN_INT; /* [nbr] Max length of PFT string in output data */
  long tpo_nbr_out=NC_MIN_INT; /* [nbr] Number of topounits in output data */
  if(need_clm) rcd=nco_inq_dimlen(in_id,dmn_id_clm_in,&clm_nbr_in);
  if(need_grd) rcd=nco_inq_dimlen(in_id,dmn_id_grd_in,&grd_nbr_in);
  if(need_lnd) rcd=nco_inq_dimlen(in_id,dmn_id_lnd_in,&lnd_nbr_in);
  if(need_pft) rcd=nco_inq_dimlen(in_id,dmn_id_pft_in,&pft_nbr_in);
  if(need_tpo) rcd=nco_inq_dimlen(in_id,dmn_id_tpo_in,&tpo_nbr_in);
  /* 20241017: I think topounits are an abstract layer between gridcells and landunits. E3SMv3 introduced topounits with the simplest usage, tpo_nbr_in == grd_nbr_in so that each gridcell is a single topounit. However, in the future some gridcells may have multiple topounits, with each topounit having its own landunits (and so on). I think topounits will generally be used to represent distinct topographic slopes/catchments with a gridcell, e.g., sunlit side of mountains vs shaded side. */
  if(need_tpo && need_grd && tpo_nbr_in != grd_nbr_in) (void)fprintf(stdout,"%s: WARNING %s reports number of topounits and gridcells are unequal. Expect mayhem. tpo_nbr_in = %ld != %ld = grd_nbr_in\n",nco_prg_nm_get(),fnc_nm,tpo_nbr_in,grd_nbr_in);

  int dmn_id_levcan_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levgrnd_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levlak_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levsno_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levsno1_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levtot_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_numrad_in=NC_MIN_INT; /* [id] Dimension ID */

  long levcan_nbr_in=NC_MIN_INT; /* [nbr] Number of canopy layers in input data */
  long levgrnd_nbr_in=NC_MIN_INT; /* [nbr] Number of soil layers in input data */
  long levlak_nbr_in=NC_MIN_INT; /* [nbr] Number of lake layers in input data */
  long levsno_nbr_in=NC_MIN_INT; /* [nbr] Number of snow layers in input data */
  long levsno1_nbr_in=NC_MIN_INT; /* [nbr] Number of snow layers plus one in input data */
  long levtot_nbr_in=NC_MIN_INT; /* [nbr] Number of snow+soil layers in input data */
  long numrad_nbr_in=NC_MIN_INT; /* [nbr] Number of snow+soil layers in input data */
  long levcan_nbr_out=NC_MIN_INT; /* [nbr] Number of canopy layers in output data */
  long levgrnd_nbr_out=NC_MIN_INT; /* [nbr] Number of soil layers in output data */
  long levlak_nbr_out=NC_MIN_INT; /* [nbr] Number of lake layers in output data */
  long levsno_nbr_out=NC_MIN_INT; /* [nbr] Number of snow layers in output data */
  long levsno1_nbr_out=NC_MIN_INT; /* [nbr] Number of snow layers plus one in output data */
  long levtot_nbr_out=NC_MIN_INT; /* [nbr] Number of snow+soil layers in output data */
  long numrad_nbr_out=NC_MIN_INT; /* [nbr] Number of snow+soil layers in output data */
  if(need_levcan){
    rcd=nco_inq_dimid(in_id,levcan_nm_in,&dmn_id_levcan_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_levcan_in,&levcan_nbr_in);
    levcan_nbr_out=levcan_nbr_in;
  } /* !need_levcan */
  if(need_levgrnd){
    rcd=nco_inq_dimid(in_id,levgrnd_nm_in,&dmn_id_levgrnd_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_levgrnd_in,&levgrnd_nbr_in);
    levgrnd_nbr_out=levgrnd_nbr_in;
  } /* !need_levgrnd */
  if(need_levlak){
    rcd=nco_inq_dimid(in_id,levlak_nm_in,&dmn_id_levlak_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_levlak_in,&levlak_nbr_in);
    levlak_nbr_out=levlak_nbr_in;
  } /* !need_levlak */
  if(need_levsno){
    rcd=nco_inq_dimid(in_id,levsno_nm_in,&dmn_id_levsno_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_levsno_in,&levsno_nbr_in);
    levsno_nbr_out=levsno_nbr_in;
  } /* !need_levsno */
  if(need_levsno1){
    rcd=nco_inq_dimid(in_id,levsno1_nm_in,&dmn_id_levsno1_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_levsno1_in,&levsno1_nbr_in);
    levsno1_nbr_out=levsno1_nbr_in;
  } /* !need_levsno1 */
  if(need_levtot){
    rcd=nco_inq_dimid(in_id,levtot_nm_in,&dmn_id_levtot_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_levtot_in,&levtot_nbr_in);
    levtot_nbr_out=levtot_nbr_in;
  } /* !need_levtot */
  if(need_numrad){
    rcd=nco_inq_dimid(in_id,numrad_nm_in,&dmn_id_numrad_in);
    rcd=nco_inq_dimlen(in_id,dmn_id_numrad_in,&numrad_nbr_in);
    numrad_nbr_out=numrad_nbr_in;
  } /* !need_numrad */
  
  int hrz_id; /* [id] Horizontal grid netCDF file ID */
  long bnd_nbr=int_CEWI; /* [nbr] Number of boundaries for output time and rectangular grid coordinates, and number of vertices for output non-rectangular grid coordinates */
  long col_nbr=NC_MIN_INT; /* [nbr] Number of columns */
  long lon_nbr=NC_MIN_INT; /* [nbr] Number of longitudes */
  long lat_nbr=NC_MIN_INT; /* [nbr] Number of latitudes */
  size_t grd_sz_in; /* [nbr] Number of elements in single layer of input grid */
  size_t grd_sz_out; /* [nbr] Number of elements in single layer of output grid */
  if(flg_grd_dat) hrz_id=in_id; else hrz_id=tpl_id;

  /* Locate bounds dimension, if any, in file containing horizontal grid */
  if(bnd_nm_in && (rcd=nco_inq_dimid_flg(hrz_id,bnd_nm_in,&dmn_id_bnd_in)) == NC_NOERR) /* do nothing */; 
  else if((rcd=nco_inq_dimid_flg(hrz_id,"nv",&dmn_id_bnd_in)) == NC_NOERR) bnd_nm_in=strdup("nv"); /* fxm */
  else if((rcd=nco_inq_dimid_flg(hrz_id,"nvertices",&dmn_id_bnd_in)) == NC_NOERR) bnd_nm_in=strdup("nvertices"); /* CICE */
  else if((rcd=nco_inq_dimid_flg(hrz_id,"maxEdges",&dmn_id_bnd_in)) == NC_NOERR) bnd_nm_in=strdup("maxEdges"); /* MPAS */

  if(flg_grd_1D) rcd=nco_inq_dimlen(hrz_id,dmn_id_col_in,&col_nbr);
  if(flg_grd_2D){
    rcd=nco_inq_dimlen(hrz_id,dmn_id_lat_in,&lat_nbr);
    rcd=nco_inq_dimlen(hrz_id,dmn_id_lon_in,&lon_nbr);
  } /* !flg_grd_2D */
  if(dmn_id_bnd_in != NC_MIN_INT) rcd=nco_inq_dimlen(hrz_id,dmn_id_bnd_in,&bnd_nbr);
  int dmn_nbr_hrz_crd; /* [nbr] Number of horizontal dimensions in output grid */
  if(flg_grd_2D) dmn_nbr_hrz_crd=2; else dmn_nbr_hrz_crd=1;
  
  if(grd_nbr_in != NC_MIN_INT){
    grd_sz_in=grd_nbr_in;
  }else{
    grd_sz_in= flg_grd_1D ? col_nbr : lat_nbr*lon_nbr;
  } /* !grd_nbr_in */
  grd_sz_out= flg_grd_1D ? col_nbr : lat_nbr*lon_nbr;
  grd_sz_in+=0*grd_sz_in; /* CEWI */
  grd_nbr_out=grd_sz_out;
  
  /* Lay-out unpacked file */
  char *bnd_nm_out=NULL;
  char *col_nm_out=NULL;
  char *lat_nm_out=NULL;
  char *lon_nm_out=NULL;
  //  char *lat_dmn_nm_out;
  //  char *lon_dmn_nm_out;
  int dmn_id_bnd_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_col_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lat_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lon_out=NC_MIN_INT; /* [id] Dimension ID */

  if(rgr->bnd_nm) bnd_nm_out=rgr->bnd_nm; else bnd_nm_out=bnd_nm_in;
  if(rgr->col_nm_out) col_nm_out=rgr->col_nm_out; else col_nm_out=col_nm_in;
  //  if(rgr->lat_dmn_nm) lat_dmn_nm_out=rgr->lat_dmn_nm; else lat_dmn_nm_out=lat_nm_in;
  //  if(rgr->lon_dmn_nm) lon_dmn_nm_out=rgr->lon_dmn_nm; else lon_dmn_nm_out=lon_nm_in;
  if(rgr->lat_nm_out) lat_nm_out=rgr->lat_nm_out; else lat_nm_out=lat_nm_in;
  if(rgr->lon_nm_out) lon_nm_out=rgr->lon_nm_out; else lon_nm_out=lon_nm_in;

  /* Define horizontal dimensions before all else */
  if(flg_grd_1D){
    rcd=nco_def_dim(out_id,col_nm_out,col_nbr,&dmn_id_col_out);
  } /* !flg_grd_1D */
  if(flg_grd_2D){
    rcd=nco_def_dim(out_id,lat_nm_out,lat_nbr,&dmn_id_lat_out);
    rcd=nco_def_dim(out_id,lon_nm_out,lon_nbr,&dmn_id_lon_out);
  } /* !flg_grd_2D */
  if(dmn_id_bnd_in != NC_MIN_INT) rcd=nco_def_dim(out_id,bnd_nm_out,bnd_nbr,&dmn_id_bnd_out);

  char *clm_nm_out=NULL;
  char *grd_nm_out=NULL;
  char *lnd_nm_out=NULL;
  char *pft_nm_out=NULL;
  char *tpo_nm_out=NULL;
  if(need_clm) clm_nm_out=(char *)strdup(clm_nm_in);
  if(need_grd) grd_nm_out=(char *)strdup(grd_nm_in);
  if(need_lnd) lnd_nm_out=(char *)strdup(lnd_nm_in);
  if(need_pft) pft_nm_out=(char *)strdup(pft_nm_in);
  if(need_tpo) tpo_nm_out=(char *)strdup(tpo_nm_in);
  int dmn_id_clm_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lnd_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_mec_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_pft_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_pft_sng_lng_out=NC_MIN_INT; /* [id] Dimension ID */

  char *levcan_nm_out=NULL;
  char *levgrnd_nm_out=NULL;
  char *levlak_nm_out=NULL;
  char *levsno_nm_out=NULL;
  char *levsno1_nm_out=NULL;
  char *levtot_nm_out=NULL;
  char *numrad_nm_out=NULL;
  if(need_levcan) levcan_nm_out=(char *)strdup(levcan_nm_in);
  if(need_levgrnd) levgrnd_nm_out=(char *)strdup(levgrnd_nm_in);
  if(need_levlak) levlak_nm_out=(char *)strdup(levlak_nm_in);
  if(need_levsno) levsno_nm_out=(char *)strdup(levsno_nm_in);
  if(need_levsno1) levsno1_nm_out=(char *)strdup(levsno1_nm_in);
  if(need_levtot) levtot_nm_out=(char *)strdup(levtot_nm_in);
  if(need_numrad) numrad_nm_out=(char *)strdup(numrad_nm_in);
  int dmn_id_levcan_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levgrnd_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levlak_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levsno_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levsno1_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_levtot_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_numrad_out=NC_MIN_INT; /* [id] Dimension ID */
  
  /* NB: ilun is also enumerated as type nco_lnd_typ_enm */
  int ilun_vegetated_or_bare_soil; /* 1 [enm] */
  int ilun_crop; /* 2 [enm] */
  int ilun_landice; /* 3 [enm] */
  int ilun_landice_multiple_elevation_classes; /* 4 [enm] */
  int ilun_deep_lake; /* 5 [enm] */
  int ilun_wetland; /* 6 [enm] */
  int ilun_urban_tbd; /* 7 [enm] */
  int ilun_urban_hd; /* 8 [enm] */
  int ilun_urban_md; /* 9 [enm] */
  if(flg_nm_hst){
    /* Names in data file are as in history files ("ltype_"...) */
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_vegetated_or_bare_soil",&ilun_vegetated_or_bare_soil,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_crop",&ilun_crop,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_landice",&ilun_landice,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_landice_multiple_elevation_classes",&ilun_landice_multiple_elevation_classes,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_deep_lake",&ilun_deep_lake,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_wetland",&ilun_wetland,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_urban_tbd",&ilun_urban_tbd,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_urban_hd",&ilun_urban_hd,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ltype_urban_md",&ilun_urban_md,NC_INT);
  }else{ /* !flg_nm_hst */
    /* Names in data file are as in restart files ("ilun_"...) */
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_vegetated_or_bare_soil",&ilun_vegetated_or_bare_soil,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_crop",&ilun_crop,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_landice_multiple_elevation_classes",&ilun_landice_multiple_elevation_classes,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_deep_lake",&ilun_deep_lake,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_wetland",&ilun_wetland,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_urban_tbd",&ilun_urban_tbd,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_urban_hd",&ilun_urban_hd,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_urban_md",&ilun_urban_md,NC_INT);
    /* 20241120: NCAR CLM/CTSM deprecates ilun_landice for ilun_UNUSED in at least some simulations, e.g., CESM2 LE */
    rcd=nco_inq_att_flg(in_id,NC_GLOBAL,"ilun_landice",(nc_type *)NULL,(long *)NULL);
    if(rcd == NC_NOERR){
      rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_landice",&ilun_landice,NC_INT);
    }else{
      rcd=nco_inq_att_flg(in_id,NC_GLOBAL,"ilun_UNUSED",(nc_type *)NULL,(long *)NULL);
      if(rcd == NC_NOERR){
	/* For now we treat ilun_UNUSED as if it were ilun_glacier */
	rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_UNUSED",&ilun_landice,NC_INT);
	(void)fprintf(stdout,"%s: INFO Restart dataset appears to be from CTSM not ELM because history global attribute defines 'ilun_UNUSED = 3' not 'ilun_glacier = 3'\n",nco_prg_nm_get());
      }else{
	(void)fprintf(stdout,"%s: ERROR Restart dataset is missing expected landunit type global attribute\n",nco_prg_nm_get());
	nco_exit(EXIT_FAILURE);
      } /* !rcd */
    } /* !rcd */
  } /* !flg_nm_hst */

  /* Determine output Column dimension if needed */
  double *cols1d_wtxy=NULL; /* [frc] Column weight relative to corresponding gridcell */
  double clm_wgt=NC_MIN_DOUBLE; /* [frc] Column weight relative to corresponding gridcell */
  int *cols1d_active=NULL; /* [flg] Column active flag (1=active, 0=inactive) */
  int *cols1d_gridcell_index=NULL; /* [idx] Gridcell index of column */
  int *cols1d_ityp=NULL; /* [enm] Column type */
  int *cols1d_ityplun=NULL; /* [enm] Column landunit type */
  int *cols1d_ixy=NULL; /* [idx] Column 2D longitude index */
  int *cols1d_jxy=NULL; /* [idx] Column 2D latitude index */
  int *snl_var=NULL; /* [nbr] Negative of number of snow layers */
  int clm_typ; /* [enm] Column landunit type */
  const int clm_typ_mec_fst=400; /* [enm] MEC column-type offset from zero */
  if(need_clm){
    if(cols1d_active_id != NC_MIN_INT) cols1d_active=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    if(cols1d_active_id != NC_MIN_INT) rcd=nco_get_var(in_id,cols1d_active_id,cols1d_active,NC_INT);

    if(cols1d_gridcell_index_id != NC_MIN_INT) cols1d_gridcell_index=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    if(cols1d_gridcell_index_id != NC_MIN_INT) rcd=nco_get_var(in_id,cols1d_gridcell_index_id,cols1d_gridcell_index,NC_INT);

    if(cols1d_ityp_id != NC_MIN_INT) cols1d_ityp=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    if(cols1d_ityp_id != NC_MIN_INT) rcd=nco_get_var(in_id,cols1d_ityp_id,cols1d_ityp,NC_INT);

    cols1d_ityplun=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    rcd=nco_get_var(in_id,cols1d_ityplun_id,cols1d_ityplun,NC_INT);

    if(cols1d_wtxy_id != NC_MIN_INT) cols1d_wtxy=(double *)nco_malloc(clm_nbr_in*sizeof(double));
    if(cols1d_wtxy_id != NC_MIN_INT) rcd=nco_get_var(in_id,cols1d_wtxy_id,cols1d_wtxy,NC_DOUBLE);

    mec_nbr_out=0;
    for(clm_idx=0;clm_idx<clm_nbr_in;clm_idx++){
      if(cols1d_ityplun[clm_idx] == ilun_landice_multiple_elevation_classes){
	mec_nbr_in=cols1d_ityp[clm_idx] % clm_typ_mec_fst;
	if(mec_nbr_in > mec_nbr_out) mec_nbr_out=mec_nbr_in;
      } /* !cols1d_ityplun */
    } /* !clm_idx */
    /* Glacier landunits (ilun=4, usually) with active glaciers (e.g., IG cases) have 10 (always, AFAICT) glacier elevation classes
       20241016: IG restart files store the number of elevation classes in orphaned dimension named "glc_nec"
       Restart files without active glaciers do not contain the "glc_nec" dimension
       All ELM restart files also contain an orphaned dimension named "levtrc"---not sure what it is for */
    if(mec_nbr_out > 0) need_mec=True;
    if(need_mec){
      /* Sanity check. Allow for input files that have MECs yet lack "glc_nec" due to NCO skipping orphaned dimensions. */
      rcd=nco_inq_dimid_flg(in_id,mec_nm_in,&dmn_id_mec_in);
      if(rcd == NC_NOERR){
	nco_inq_dimlen(in_id,dmn_id_mec_in,&mec_nbr_in);
	assert(mec_nbr_in == mec_nbr_out);
      } /* !rcd */
      rcd=NC_NOERR;
    } /* !need_mec */
    
    cols1d_ixy=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    rcd=nco_get_var(in_id,cols1d_ixy_id,cols1d_ixy,NC_INT);
    if(flg_grd_2D){
      cols1d_jxy=(int *)nco_malloc(clm_nbr_in*sizeof(int));
      rcd=nco_get_var(in_id,cols1d_jxy_id,cols1d_jxy,NC_INT);
    } /* !flg_grd_2D */
    
    /* 20241030: Grab snl_var to de-mangle levels of snow variables */
    if(need_levsno){
      rcd=nco_inq_varid_flg(in_id,snl_var_nm,&snl_var_id);
      if(rcd == NC_NOERR){
	snl_var=(int *)nco_malloc(clm_nbr_in*sizeof(int));
	rcd=nco_get_var(in_id,snl_var_id,snl_var,NC_INT);
	(void)fprintf(stdout,"%s: INFO Will use %s to assign levsno, levsno1, and levtot snow layers to top-down (ocean-like) vertical grid\n",nco_prg_nm_get(),snl_var_nm);
      }else{ /* !rcd */
	(void)fprintf(stdout,"%s: INFO Snow layer variable %s not in input, unable to assign snow layers to intuitive (top-down) vertical grid\n",nco_prg_nm_get(),snl_var_nm);
      } /* !rcd */
    } /* !need_levsno */

  } /* !need_clm */

  /* Determine output Grid dimension if needed:
     ELM/CLM 'gridcell' dimension counts each gridcell that contains land
     Replace this dimension by horizontal dimension(s) in input data file */
  if(need_grd){
    if(flg_grd_1D) grd_nbr_out=col_nbr;
    if(flg_grd_2D) grd_nbr_out=lat_nbr*lon_nbr;
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO grd_nbr_out = %ld\n",nco_prg_nm_get(),grd_nbr_out);
  } /* !need_grd */

  /* Determine output Landunit dimension if needed */
  double *land1d_wtxy=NULL; /* [frc] Landunit weight relative to corresponding gridcell */
  int *land1d_active=NULL; /* [flg] Landunit active flag (1=active, 0=inactive) */
  int *land1d_gridcell_index=NULL; /* [idx] Gridcell index of landunit */
  int *land1d_ityplun=NULL; /* [enm] Landunit type */
  int *land1d_ixy=NULL; /* [idx] Landunit 2D longitude index */
  int *land1d_jxy=NULL; /* [idx] Landunit 2D latitude index */
  int lnd_typ; /* [enm] Landunit type */
  int lnd_typ_vld_1st; /* [enm] Landunit type of first valid (non-zero, non-missing) column|landunit|pft for variable */
  const int lut_max=9; /* [enm] Maximum value of landunit type (typically 9 for ELM) */
  if(need_lnd){
    if(land1d_active_id != NC_MIN_INT) land1d_active=(int *)nco_malloc(lnd_nbr_in*sizeof(int));
    if(land1d_active_id != NC_MIN_INT) rcd=nco_get_var(in_id,land1d_active_id,land1d_active,NC_INT);

    if(land1d_gridcell_index_id != NC_MIN_INT) land1d_gridcell_index=(int *)nco_malloc(lnd_nbr_in*sizeof(int));
    if(land1d_gridcell_index_id != NC_MIN_INT) rcd=nco_get_var(in_id,land1d_gridcell_index_id,land1d_gridcell_index,NC_INT);

    if(land1d_ityplun_id != NC_MIN_INT) land1d_ityplun=(int *)nco_malloc(lnd_nbr_in*sizeof(int));
    if(land1d_ityplun_id != NC_MIN_INT) rcd=nco_get_var(in_id,land1d_ityplun_id,land1d_ityplun,NC_INT);

    land1d_ixy=(int *)nco_malloc(lnd_nbr_in*sizeof(int));
    rcd=nco_get_var(in_id,land1d_ixy_id,land1d_ixy,NC_INT);
    if(flg_grd_2D){
      land1d_jxy=(int *)nco_malloc(lnd_nbr_in*sizeof(int));
      rcd=nco_get_var(in_id,land1d_jxy_id,land1d_jxy,NC_INT);
    } /* !flg_grd_2D */

    if(land1d_wtxy_id != NC_MIN_INT) land1d_wtxy=(double *)nco_malloc(lnd_nbr_in*sizeof(double));
    if(land1d_wtxy_id != NC_MIN_INT) rcd=nco_get_var(in_id,land1d_wtxy_id,land1d_wtxy,NC_DOUBLE);
  } /* !need_lnd */

  /* Determine output PFT dimension if needed */
  /* ELM history files contain a dimension named "natpft" which contains the number of natural PFTs (including bare ground)
     AFAICT, the variable PCT_NAT_PFT(time,natpft,lat,lon) is the exclusive user of this dimension
     ELM restart files do not contain the natpft dimension
     However, we need natpft for restart files in order to parse crop names in global attributes 
     How to obtain natpft for restart files? */
  //double *pfts1d_wtxy=NULL; /* [frc] PFT weight relative to corresponding gridcell */
  int *pfts1d_active=NULL; /* [flg] PFT active flag (1=active, 0=inactive) */
  int *pfts1d_ityp_veg=NULL; /* [enm] PFT vegetation type */
  int *pfts1d_ityplun=NULL; /* [enm] PFT landunit type */
  int *pfts1d_ixy=NULL; /* [idx] PFT 2D longitude index */
  int *pfts1d_jxy=NULL; /* [idx] PFT 2D latitude index */
  int pft_typ; /* [enm] PFT type */
  if(need_pft){
    
    if(pfts1d_active_id != NC_MIN_INT) pfts1d_active=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    if(pfts1d_active_id != NC_MIN_INT) rcd=nco_get_var(in_id,pfts1d_active_id,pfts1d_active,NC_INT);

    //pfts1d_wtxy=(double *)nco_malloc(pft_nbr_in*sizeof(double));
    pfts1d_ityp_veg=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    pfts1d_ityplun=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    
    //rcd=nco_get_var(in_id,pfts1d_wtxy_id,pfts1d_wtxy,NC_DOUBLE);
    rcd=nco_get_var(in_id,pfts1d_ityp_veg_id,pfts1d_ityp_veg,NC_INT);
    rcd=nco_get_var(in_id,pfts1d_ityplun_id,pfts1d_ityplun,NC_INT);

    pft_nbr_out=0;
    pft_crp_nbr_out=0;
    pft_ntr_nbr_out=0;
    for(pft_idx=0;pft_idx<pft_nbr_in;pft_idx++){
      /* Count total PFTs = Natural + Crops */
      if(pfts1d_ityp_veg[pft_idx] > pft_nbr_out) pft_nbr_out=pfts1d_ityp_veg[pft_idx];
      /* Count natural PFTs only */
      if(pfts1d_ityplun[pft_idx] == ilun_vegetated_or_bare_soil)
	if(pfts1d_ityp_veg[pft_idx] > pft_ntr_nbr_out) pft_ntr_nbr_out=pfts1d_ityp_veg[pft_idx];
      /* Count crop PFTs only */
      if(pfts1d_ityplun[pft_idx] == ilun_crop)
	if(pfts1d_ityp_veg[pft_idx] > pft_crp_nbr_out) pft_crp_nbr_out=pfts1d_ityp_veg[pft_idx];
    } /* !pft_idx */
    /* Add one to account for ityp == 0 for bare_soil */
    pft_nbr_out++;
    pft_ntr_nbr_out++;
    /* Subtract starting offset natural index from crop number */
    if(pft_crp_nbr_out >= pft_ntr_nbr_out) pft_crp_nbr_out=pft_crp_nbr_out-pft_ntr_nbr_out+1;

    /* Sanity check PFT counts */
    if(flg_nm_hst){
      rcd=nco_inq_dimid_flg(in_id,pft_ntr_nm_in,&dmn_id_pft_ntr_in);
      if(rcd == NC_NOERR){
	rcd=nco_inq_dimlen(in_id,dmn_id_pft_ntr_in,&pft_ntr_nbr_in);
	if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO pft_ntr_nbr_in = %ld\n",nco_prg_nm_get(),pft_ntr_nbr_in);
	assert(pft_ntr_nbr_in == pft_ntr_nbr_out);
      }else{ /* !rcd */
	(void)fprintf(stdout,"%s: WARNING %s is unable to find natural PFT dimension \"%s\" in a history-type dataset\n",nco_prg_nm_get(),fnc_nm,pft_ntr_nm_in);
      } /* !rcd */
      
    } /* !flg_nm_hst */

    pfts1d_ixy=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    rcd=nco_get_var(in_id,pfts1d_ixy_id,pfts1d_ixy,NC_INT);
    if(flg_grd_2D){
      pfts1d_jxy=(int *)nco_malloc(pft_nbr_in*sizeof(int));
      rcd=nco_get_var(in_id,pfts1d_jxy_id,pfts1d_jxy,NC_INT);
    } /* !flg_grd_2D */
    
  } /* !need_pft */

  /* Determine output Topounit dimension if needed */
  int *topo1d_ixy=NULL; /* [idx] Topounit 2D longitude index */
  int *topo1d_jxy=NULL; /* [idx] Topounit 2D latitude index */
  if(need_tpo){
    
    topo1d_ixy=(int *)nco_malloc(tpo_nbr_in*sizeof(int));
    rcd=nco_get_var(in_id,topo1d_ixy_id,topo1d_ixy,NC_INT);
    if(flg_grd_2D){
      topo1d_jxy=(int *)nco_malloc(tpo_nbr_in*sizeof(int));
      rcd=nco_get_var(in_id,topo1d_jxy_id,topo1d_jxy,NC_INT);
    } /* !flg_grd_2D */

    if(flg_nm_hst){
      /* 20241017: eva_h2.nc, eva_h3.nc has empty (all values are zero) topo1d_ixy, topo1d_jxy, 
	 These are supposed to be (1-based) Fortran indices to corresponding longitude and latitude gridcells of the topounit
	 If any index is zero, ... fxm */
      nco_bool flg_rpl_tpo_wth_grd=False; /* [flg] Replace topo1d_[ij]xy values with grid1d_[ij]xy values */
      for(tpo_idx=0;tpo_idx<tpo_nbr_in;tpo_idx++){
	if((topo1d_ixy[tpo_idx] <= 0) || (topo1d_jxy[tpo_idx] <= 0)){
	  (void)fprintf(stdout,"%s: WARNING %s reports invalid topounit latitude and/or longitude indices: topo1d_ixy[%ld] = %d, topo1d_jxy[%ld] = %d. These are supposed to be (1-based) Fortran indices to corresponding longitude and latitude gridcells of the topounit. By definition/convention, each Fortran index must be > 0. In other words, this input dataset, presumably produced by ELM, contains invalid variables. Contact the ELM authors and/or upgrade ELM to fix the root cause of this problem.\n",nco_prg_nm_get(),fnc_nm,tpo_idx,topo1d_ixy[tpo_idx],tpo_idx,topo1d_ixy[tpo_idx]);
	  if((grid1d_ixy_id != NC_MIN_INT) && (grid1d_jxy_id != NC_MIN_INT)){
	    (void)fprintf(stdout,"%s: WARNING %s Attempting workaround: Will replace invalid topo1d_[ij]xy values with grid1d_[ij]xy values, respectively.\nCross your fingers...\n",nco_prg_nm_get(),fnc_nm);
	    flg_rpl_tpo_wth_grd=True;
	  }else{
	    (void)fprintf(stdout,"%s: ERROR %s Unable to work around invalid topo1d_[ij]xy values because grid1d_[ij]xy values are not present to be used as replacements\n",nco_prg_nm_get(),fnc_nm);
	    nco_exit(EXIT_FAILURE);
	  } /* !grid1d_ixy_id */
	} /* !topo1d_ixy */
	if(flg_rpl_tpo_wth_grd){
	  /* Overwrite topo1d_[ij]xy with grid1d_[ij]xy */
	  rcd=nco_get_var(in_id,grid1d_ixy_id,topo1d_ixy,NC_INT);
	  if(flg_grd_2D){
	    rcd=nco_get_var(in_id,grid1d_jxy_id,topo1d_jxy,NC_INT);
	  } /* !flg_grd_2D */
	} /* !flg_rpl_tpo_wth_grd */
      } /* !tpo_idx */
      
    } /* !flg_nm_hst */
    
  } /* !need_tpo */
  
  (void)fflush(stdout);

  /* Diagnostic grid variables */
  char frc_column_nm[]="frc_column";
  char frc_landunit_nm[]="frc_landunit";
  int frc_column_out_id=NC_MIN_INT; /* [id] Variable ID for frc_column */
  int frc_landunit_out_id=NC_MIN_INT; /* [id] Variable ID for frc_landunit */
  nco_bool flg_frc_column_out=False; /* [flg] Add frc_column to output */
  nco_bool flg_frc_landunit_out=False; /* [flg] Add frc_landunit to output */
  if(cols1d_ityp && cols1d_ixy && cols1d_wtxy) flg_frc_column_out=True;
  if(land1d_ityplun && land1d_ixy && land1d_wtxy) flg_frc_landunit_out=True;
  switch(lut_out){
  case nco_lut_out_wgt_all: /* Area-weighted average of columns of all landunit types in gridcell */
  case nco_lut_out_wgt_soi_glc: /* Area-weighted average of columns of soil+glacier types in gridcell */
    if(!cols1d_wtxy) (void)fprintf(stdout,"%s: ERROR %s reports requested landunit output type lut_out = %s requires use of unavailable variable cols1d_wtxy\n",nco_prg_nm_get(),fnc_nm,nco_lut_out_sng(lut_out));
    break;
  default:
    break;
  } /* !lut_out */
  
  /* Define unpacked versions of needed dimensions before all else */
  if(clm_nbr_out == NC_MIN_INT) clm_nbr_out=0;
  if(lnd_nbr_out == NC_MIN_INT) lnd_nbr_out=0;
  if(mec_nbr_out == NC_MIN_INT) mec_nbr_out=0;
  if(pft_nbr_out == NC_MIN_INT) pft_nbr_out=pft_ntr_nbr_out=pft_crp_nbr_out=0;
  if(tpo_nbr_out == NC_MIN_INT) tpo_nbr_out=0;
  if(flg_frc_column_out){
    clm_nbr_out=5; /* Soil column, Glaciated column, Lake column, Wetland column, Total column */
    if(need_mec) clm_nbr_out=mec_nbr_out+5; /* Soil column, MECs, Total MEC, Lake column, Wetland column, Total column */
  } /* !flg_frc_column_out */
  if(flg_frc_landunit_out) lnd_nbr_out=1+lut_max; /* Total of all columns in index 0 plus nine input landunits with Fortran-based indexes */
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO clm_nbr_out = %ld\n",nco_prg_nm_get(),clm_nbr_out);
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO lnd_nbr_out = %ld\n",nco_prg_nm_get(),lnd_nbr_out);
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO mec_nbr_out = %ld\n",nco_prg_nm_get(),mec_nbr_out);
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO pft_nbr_out = %ld\n",nco_prg_nm_get(),pft_nbr_out);
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO pft_ntr_nbr_out = %ld\n",nco_prg_nm_get(),pft_ntr_nbr_out);
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO pft_crp_nbr_out = %ld\n",nco_prg_nm_get(),pft_crp_nbr_out);
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO tpo_nbr_out = %ld\n",nco_prg_nm_get(),tpo_nbr_out);
  if((need_clm || flg_frc_column_out) && clm_nbr_out > 0L) rcd=nco_def_dim(out_id,clm_nm_out,clm_nbr_out,&dmn_id_clm_out);
  if((need_lnd || flg_frc_landunit_out) && lnd_nbr_out > 0L) rcd=nco_def_dim(out_id,lnd_nm_out,lnd_nbr_out,&dmn_id_lnd_out);
  if(need_pft && pft_nbr_out > 0L) rcd=nco_def_dim(out_id,pft_nm_out,pft_nbr_out,&dmn_id_pft_out);
#define NCO_PFT_MAX_SNG_LNG 36
  pft_sng_lng_out=NCO_PFT_MAX_SNG_LNG;
  if(need_pft && pft_nbr_out > 0L) rcd=nco_def_dim(out_id,pft_sng_lng_nm_out,pft_sng_lng_out,&dmn_id_pft_sng_lng_out);

  /* MECs can be a new output dimension if they are enumerated in input (MEC is a column-level sub-unit of a glacier landunit)
     mec_nbr_out refers to the number of actual MECs
     mec_nbr_outp1 is the size of the MEC dimension
     Index zero of MEC dimension is used to hold soil values of MEC variables 
     Indices 1-10 of MEC dimension hold actual elevation class data */
  mec_nbr_outp1=mec_nbr_out+1;
  if(need_mec && mec_nbr_out > 0L) rcd=nco_def_dim(out_id,mec_nm_out,mec_nbr_out+1,&dmn_id_mec_out);

  if(need_levcan && levcan_nbr_out > 0L) rcd=nco_def_dim(out_id,levcan_nm_out,levcan_nbr_out,&dmn_id_levcan_out);
  if(need_levgrnd && levgrnd_nbr_out > 0L) rcd=nco_def_dim(out_id,levgrnd_nm_out,levgrnd_nbr_out,&dmn_id_levgrnd_out);
  if(need_levlak && levlak_nbr_out > 0L) rcd=nco_def_dim(out_id,levlak_nm_out,levlak_nbr_out,&dmn_id_levlak_out);
  if(need_levsno && levsno_nbr_out > 0L) rcd=nco_def_dim(out_id,levsno_nm_out,levsno_nbr_out,&dmn_id_levsno_out);
  if(need_levsno1 && levsno1_nbr_out > 0L) rcd=nco_def_dim(out_id,levsno1_nm_out,levsno1_nbr_out,&dmn_id_levsno1_out);
  if(need_levtot && levtot_nbr_out > 0L) rcd=nco_def_dim(out_id,levtot_nm_out,levtot_nbr_out,&dmn_id_levtot_out);
  if(need_numrad && numrad_nbr_out > 0L) rcd=nco_def_dim(out_id,numrad_nm_out,numrad_nbr_out,&dmn_id_numrad_out);

  /* Pre-allocate dimension ID and cnt/srt space */
  char *var_nm; /* [sng] Variable name */
  int *dmn_ids_in=NULL; /* [id] Dimension IDs */
  int *dmn_ids_out=NULL; /* [id] Dimension IDs */
  int *dmn_ids_swp=NULL; /* [id] Dimension IDs swap variable */
  int dmn_nbr_max=5; /* [nbr] Maximum number of dimensions variable can have in input or output */
  int var_id_in; /* [id] Variable ID */
  int var_id_out; /* [id] Variable ID */
  long *dmn_cnt_in=NULL;
  long *dmn_cnt_out=NULL;
  long *dmn_cnt_swp=NULL; /* [id] Dimension count swap variable */
  long *dmn_srt=NULL;
  nc_type var_typ_in; /* [enm] Variable type (same for input and output variable, but keep separate for future extensibility) */
  nc_type var_typ_out;
  nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */

  int dmn_nbr_rec; /* [nbr] Number of unlimited dimensions */
  int *dmn_ids_rec=NULL; /* [id] Unlimited dimension IDs */

  dmn_ids_in=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  dmn_ids_out=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  dmn_ids_swp=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
  if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
  if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
  dmn_cnt_in=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
  dmn_cnt_out=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
  dmn_cnt_swp=(long *)nco_malloc(dmn_nbr_max*sizeof(long));

  /* Obtain record dimension information from data file (restart files have no time dimension) */
  rcd+=nco_inq_unlimdims(in_id,&dmn_nbr_rec,(int *)NULL);
  if(dmn_nbr_rec > 0){
    dmn_ids_rec=(int *)nco_malloc(dmn_nbr_rec*sizeof(int));
    rcd=nco_inq_unlimdims(in_id,&dmn_nbr_rec,dmn_ids_rec);
  } /* !dmn_nbr_rec */

  fl_out_fmt=rgr->fl_out_fmt;

  //const int dmn_nbr_0D=0; /* [nbr] Rank of 0-D grid variables (scalars) */
  const int dmn_nbr_1D=1; /* [nbr] Rank of 1-D grid variables */
  const int dmn_nbr_2D=2; /* [nbr] Rank of 2-D grid variables */
  const int dmn_nbr_3D=3; /* [nbr] Rank of 3-D grid variables */
  nc_type crd_typ_in;
  nc_type crd_typ_out;

  /* Required grid variables */
  int lat_in_id; /* [id] Variable ID for latitude */
  int lat_out_id; /* [id] Variable ID for latitude */
  int lon_in_id; /* [id] Variable ID for longitude */
  int lon_out_id; /* [id] Variable ID for longitude */
  rcd=nco_inq_varid(hrz_id,lat_nm_in,&lat_in_id);
  rcd=nco_inq_varid(hrz_id,lon_nm_in,&lon_in_id);
  rcd=nco_inq_vartype(hrz_id,lat_in_id,&crd_typ_in);
  /* NB: ELM/CLM history files default to NC_FLOAT for most grid variables
     To convert to NC_DOUBLE on output, also convert _FillValue attribute type consistently */
  crd_typ_out=crd_typ_in;
  
  /* Optional grid variables */
  char *area_nm;
  char *lat_bnd_nm;
  char *lon_bnd_nm;
  char *sgs_frc_nm;
  char *sgs_msk_nm;
  int area_in_id=NC_MIN_INT; /* [id] Variable ID for area */
  int area_out_id=NC_MIN_INT; /* [id] Variable ID for area */
  int sgs_frc_in_id=NC_MIN_INT; /* [id] Variable ID for fraction */
  int sgs_frc_out_id=NC_MIN_INT; /* [id] Variable ID for fraction */
  int lat_bnd_in_id=NC_MIN_INT; /* [id] Variable ID for latitude bounds */
  int lat_bnd_out_id=NC_MIN_INT; /* [id] Variable ID for latitude bounds */
  int lon_bnd_in_id=NC_MIN_INT; /* [id] Variable ID for longitude bounds */
  int lon_bnd_out_id=NC_MIN_INT; /* [id] Variable ID for longitude bounds */
  int sgs_msk_in_id=NC_MIN_INT; /* [id] Variable ID for mask */
  int sgs_msk_out_id=NC_MIN_INT; /* [id] Variable ID for mask */
  nco_bool flg_area_out=False; /* [flg] Add area to output */
  nco_bool flg_lat_bnd_out=False; /* [flg] Add latitude bounds to output */
  nco_bool flg_lon_bnd_out=False; /* [flg] Add longitude bounds to output */
  nco_bool flg_sgs_frc_out=False; /* [flg] Add fraction to output */
  nco_bool flg_sgs_msk_out=False; /* [flg] Add mask to output */
  area_nm=rgr->area_nm ? rgr->area_nm : strdup("area");
  lat_bnd_nm=rgr->lat_bnd_nm ? rgr->lat_bnd_nm : strdup("lat_bnd");
  lon_bnd_nm=rgr->lon_bnd_nm ? rgr->lon_bnd_nm : strdup("lon_bnd");
  sgs_frc_nm=rgr->sgs_frc_nm ? rgr->sgs_frc_nm : strdup("landfrac");
  sgs_msk_nm=rgr->sgs_msk_nm ? rgr->sgs_msk_nm : strdup("landmask");
  if((rcd=nco_inq_varid_flg(hrz_id,area_nm,&area_in_id)) == NC_NOERR) flg_area_out=True;
  if((rcd=nco_inq_varid_flg(hrz_id,lat_bnd_nm,&lat_bnd_in_id)) == NC_NOERR) flg_lat_bnd_out=True;
  if((rcd=nco_inq_varid_flg(hrz_id,lon_bnd_nm,&lon_bnd_in_id)) == NC_NOERR) flg_lon_bnd_out=True;
  if((rcd=nco_inq_varid_flg(hrz_id,sgs_frc_nm,&sgs_frc_in_id)) == NC_NOERR) flg_sgs_frc_out=True;
  if((rcd=nco_inq_varid_flg(hrz_id,sgs_msk_nm,&sgs_msk_in_id)) == NC_NOERR) flg_sgs_msk_out=True;

  if(flg_grd_1D){
    rcd+=nco_def_var(out_id,lat_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_col_out,&lat_out_id);
    if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,lat_out_id,NULL,nco_flt_flg_prc_fll);
    (void)nco_att_cpy(hrz_id,out_id,lat_in_id,lat_out_id,PCK_ATT_CPY);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lon_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_col_out,&lon_out_id);
    if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,lon_out_id,NULL,nco_flt_flg_prc_fll);
    (void)nco_att_cpy(hrz_id,out_id,lon_in_id,lon_out_id,PCK_ATT_CPY);
    var_crt_nbr++;
    if(flg_lat_bnd_out){
      dmn_ids_out[0]=dmn_id_col_out;
      dmn_ids_out[1]=dmn_id_bnd_out;
      rcd+=nco_def_var(out_id,lat_bnd_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lat_bnd_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,lat_bnd_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,lat_bnd_in_id,lat_bnd_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_lat_bnd_out */
    if(flg_lon_bnd_out){
      dmn_ids_out[0]=dmn_id_col_out;
      dmn_ids_out[1]=dmn_id_bnd_out;
      rcd+=nco_def_var(out_id,lon_bnd_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lon_bnd_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,lon_bnd_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,lon_bnd_in_id,lon_bnd_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_lon_bnd_out */
    if(flg_frc_column_out){
      dmn_ids_out[0]=dmn_id_clm_out;
      dmn_ids_out[1]=dmn_id_col_out;
      rcd+=nco_def_var(out_id,frc_column_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&frc_column_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,frc_column_out_id,NULL,nco_flt_flg_prc_fll);
      var_crt_nbr++;
      rcd=nco_char_att_put(out_id,frc_column_nm,"long_name","Fraction of gridcell occupied by snow-related columns");
      rcd=nco_char_att_put(out_id,frc_column_nm,"legend","For datasets with Multiple Elevation Classes (MECs): index = 0 is soil column, index = 1 is MEC == 1, indexes 2..10 are remaining MEC columns, index 11 is sub-total of MEC columns, index = 12 is deep lake column, index = 13 is wetland column, and index = 14 is grand total of natural columns. For non-MEC datasets, index = 0 is soil column, index = 1 is glaciated column, index = 2 is deep lake column, index = 3 is wetland column, and index = 4 is grand total of natural columns.");
    } /* !flg_frc_column_out */
    if(flg_frc_landunit_out){
      dmn_ids_out[0]=dmn_id_lnd_out;
      dmn_ids_out[1]=dmn_id_col_out;
      rcd+=nco_def_var(out_id,frc_landunit_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&frc_landunit_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,frc_landunit_out_id,NULL,nco_flt_flg_prc_fll);
      var_crt_nbr++;
      rcd=nco_char_att_put(out_id,frc_landunit_nm,"long_name","Fraction of gridcell occupied by Landunit");
      rcd=nco_char_att_put(out_id,frc_landunit_nm,"legend","landunit index = 0 is sum of all landunit fractions, indexes = 1..9 are standard landunit types");
    } /* !flg_frc_landunit_out */
    if(flg_area_out){
      rcd+=nco_def_var(out_id,area_nm,crd_typ_out,dmn_nbr_1D,&dmn_id_col_out,&area_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,area_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,area_in_id,area_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_area_out */
    if(flg_sgs_frc_out){
      rcd+=nco_def_var(out_id,sgs_frc_nm,crd_typ_out,dmn_nbr_1D,&dmn_id_col_out,&sgs_frc_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,sgs_frc_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,sgs_frc_in_id,sgs_frc_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_sgs_frc_out */
    if(flg_sgs_msk_out){
      rcd+=nco_def_var(out_id,sgs_msk_nm,(nc_type)NC_INT,dmn_nbr_1D,&dmn_id_col_out,&sgs_msk_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,sgs_msk_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,sgs_msk_in_id,sgs_msk_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_sgs_msk_out */
  } /* !flg_grd_1D */
  if(flg_grd_2D){
    rcd+=nco_def_var(out_id,lat_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_lat_out,&lat_out_id);
    if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,lat_out_id,NULL,nco_flt_flg_prc_fll);
    (void)nco_att_cpy(hrz_id,out_id,lat_in_id,lat_out_id,PCK_ATT_CPY);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lon_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_lon_out,&lon_out_id);
    if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,lon_out_id,NULL,nco_flt_flg_prc_fll);
    (void)nco_att_cpy(hrz_id,out_id,lon_in_id,lon_out_id,PCK_ATT_CPY);
    var_crt_nbr++;
    if(flg_lat_bnd_out){
      dmn_ids_out[0]=dmn_id_lat_out;
      dmn_ids_out[1]=dmn_id_bnd_out;
      rcd+=nco_def_var(out_id,lat_bnd_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lat_bnd_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,lat_bnd_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,lat_bnd_in_id,lat_bnd_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_lat_bnd_out */
    if(flg_lon_bnd_out){
      dmn_ids_out[0]=dmn_id_lon_out;
      dmn_ids_out[1]=dmn_id_bnd_out;
      rcd+=nco_def_var(out_id,lon_bnd_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lon_bnd_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,lon_bnd_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,lon_bnd_in_id,lon_bnd_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_lon_bnd_out */
    if(flg_frc_column_out){
      dmn_ids_out[0]=dmn_id_clm_out;
      dmn_ids_out[1]=dmn_id_lat_out;
      dmn_ids_out[2]=dmn_id_lon_out;
      rcd+=nco_def_var(out_id,frc_column_nm,crd_typ_out,dmn_nbr_3D,dmn_ids_out,&frc_column_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,frc_column_out_id,NULL,nco_flt_flg_prc_fll);
      var_crt_nbr++;
    } /* !flg_frc_column_out */
    if(flg_frc_landunit_out){
      dmn_ids_out[0]=dmn_id_lnd_out;
      dmn_ids_out[1]=dmn_id_lat_out;
      dmn_ids_out[2]=dmn_id_lon_out;
      rcd+=nco_def_var(out_id,frc_landunit_nm,crd_typ_out,dmn_nbr_3D,dmn_ids_out,&frc_landunit_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,frc_landunit_out_id,NULL,nco_flt_flg_prc_fll);
      var_crt_nbr++;
    } /* !flg_frc_landunit_out */
    dmn_ids_out[0]=dmn_id_lat_out;
    dmn_ids_out[1]=dmn_id_lon_out;
    if(flg_area_out){
      rcd+=nco_def_var(out_id,area_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&area_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,area_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,area_in_id,area_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_area_out */
    if(flg_sgs_frc_out){
      rcd+=nco_def_var(out_id,sgs_frc_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&sgs_frc_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,sgs_frc_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,sgs_frc_in_id,sgs_frc_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_sgs_frc_out */
    if(flg_sgs_msk_out){
      rcd+=nco_def_var(out_id,sgs_msk_nm,(nc_type)NC_INT,dmn_nbr_2D,dmn_ids_out,&sgs_msk_out_id);
      if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,sgs_msk_out_id,NULL,nco_flt_flg_prc_fll);
      (void)nco_att_cpy(hrz_id,out_id,sgs_msk_in_id,sgs_msk_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_sgs_msk_out */
  } /* !flg_grd_2D */

  /* levgrnd coordinate: ncks -v lev.? ${DATA}/bm/elmv3_r05l15.nc */
  const double levgrnd[15]={0.007100635,0.027925,0.06225858,0.1188651,0.2121934,0.3660658,0.6197585,1.038027,1.727635,2.864607,4.739157,7.829766,12.92532,21.32647,35.17762}; /* [m] Coordinate lake levels */
  int levgrnd_out_id=NC_MIN_INT; /* [id] Variable ID for levgrnd */
  if(need_levgrnd && levgrnd_nbr_out == 15){
    dmn_ids_out[0]=dmn_id_levgrnd_out;
    rcd+=nco_def_var(out_id,levgrnd_nm_out,NC_DOUBLE,dmn_nbr_1D,dmn_ids_out,&levgrnd_out_id);
    if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,levgrnd_out_id,NULL,nco_flt_flg_prc_fll);
    var_crt_nbr++;
    rcd=nco_char_att_put(out_id,levgrnd_nm_out,"long_name","Coordinate lake levels");
    rcd=nco_char_att_put(out_id,levgrnd_nm_out,"units","meter");
  } /* !need_levgrnd */
    
  /* levlak coordinate: ncks -v lev.? ${DATA}/bm/elmv3_r05l15.nc */
  const double levlak[10]={0.05,0.6,2.1,4.6,8.1,12.6,18.6,25.6,34.325,44.775}; /* [m] Coordinate lake levels */
  int levlak_out_id=NC_MIN_INT; /* [id] Variable ID for levlak */
  if(need_levlak && levlak_nbr_out == 10){
    dmn_ids_out[0]=dmn_id_levlak_out;
    rcd+=nco_def_var(out_id,levlak_nm_out,NC_DOUBLE,dmn_nbr_1D,dmn_ids_out,&levlak_out_id);
    if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,levlak_out_id,NULL,nco_flt_flg_prc_fll);
    var_crt_nbr++;
    rcd=nco_char_att_put(out_id,levlak_nm_out,"long_name","Coordinate lake levels");
    rcd=nco_char_att_put(out_id,levlak_nm_out,"units","meter");
  } /* !need_levlak */

  /* MEC coordinate clm5.pdf: "The default is to have 10 elevation classes whose lower limits are 0, 200, 400, 700, 1000, 1300, 1600, 2000, 2500, and 3000 m."
     First value of output MEC coordinate is elevation = -1 to subtly indicate that it represents non-MEC data */
  const double mec[11]={-1,0,200,400,700,1000,1300,1600,2000,2500,3000}; /* [frc] Lowest elevation in each MEC */
  int mec_out_id=NC_MIN_INT; /* [id] Variable ID for MEC */
  if(need_mec && mec_nbr_out == 10){
    dmn_ids_out[0]=dmn_id_mec_out;
    rcd+=nco_def_var(out_id,mec_nm_out,NC_DOUBLE,dmn_nbr_1D,dmn_ids_out,&mec_out_id);
    if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,mec_out_id,NULL,nco_flt_flg_prc_fll);
    var_crt_nbr++;
    rcd=nco_char_att_put(out_id,mec_nm_out,"long_name","Lowest elevation (-1 is non-MEC column data)");
    rcd=nco_char_att_put(out_id,mec_nm_out,"units","meter");
  } /* !need_mec */
    
  /* PFT coordinate  */
  int pft_out_id=NC_MIN_INT; /* [id] Variable ID for PFT coordinate */
  if(need_pft){
    dmn_ids_out[0]=dmn_id_pft_out;
    dmn_ids_out[1]=dmn_id_pft_sng_lng_out;
    /* Assemble PFT coordinate as list of strings */
    if(fl_out_fmt == NC_FORMAT_NETCDF4){
      pft_sng_out=(nco_string *)nco_calloc(pft_nbr_out,sizeof(nco_string));
    }else{
      pft_chr_out=(char *)nco_calloc(pft_nbr_out*pft_sng_lng_out,sizeof(char));
    } /* !fl_out_fmt */
    
    /* Gather natural PFT names
       PFT indexing is complicated, and smacks of Fortran conventions
       PFT ityp = 0 is bare ground/not vegetated 
       PFT ityp = 0 is a valid ityp even though bare ground is has no plants
       PFT ityp = 1 is Needleleaf evergreen temperate tree
       PFT ityp = 14 is C4 grass is often the last natural PFT

       ELMv3 control has natpft = 17 and...
       :ipft_c3_crop = 15 ;
       :ipft_c3_irrigated = 16 ;
       PFT ityp = 15 is C3 crop
       PFT ityp = 16 is C3 irrigated, last "natural PFT"

       Presence of PFT name in global attribute field does not guarantee PFT is active
       ELMv3 control restart files contain global attributes for these seven crops despite absence of these crops in restart fields:
       :ipft_corn = 17 ;
       :ipft_irrigated_corn = 18 ;
       :ipft_spring_temperate_cereal = 19 ;
       :ipft_irrigated_spring_temperate_cereal = 20 ;
       :ipft_winter_temperate_cereal = 21 ;
       :ipft_irrigated_winter_temperate_cereal = 22 ;
       :ipft_soybean = 23 ;
       Be careful NOT to include these names in pft coordinate

       Eva h2 file has natpft = 15 and...
       :cft_c3_crop = 1 ;
       :cft_c3_irrigated = 2 ;

       Our output gridded datasets use the C-convention and are zero-based
       Our output PFT element 0 represents PFT ityp 0 == Not vegetated
       Our output PFT element 1 represents PFT ityp 1 == Needleleaf evergreen temperate tree */
    for(pft_idx=0;pft_idx<pft_ntr_nbr_out;pft_idx++){
      if(fl_out_fmt == NC_FORMAT_NETCDF4){
	pft_sng_out[pft_idx]=nco_pft_typ_sng(pft_idx);
      }else{
	strcpy(pft_chr_out+pft_idx*pft_sng_lng_out,nco_pft_typ_sng(pft_idx));
      } /* !fl_out_fmt */
    } /* !pft_idx */
    /* Restart files enumerate all (natural and crop) PFTs in global attributes
       History files enumerate only crop PFTs (i.e., CFTs) in global attributes
       Prior to 20241022, automatically generate PFT/CFT names in a boring list format
       Uncomment this simple block in case complex gleaning from global attributes (below) becomes problematic later */
#if false
    char *cft_nm_crr=NULL; /* [sng] Crop functional type descriptor */
    for(pft_idx=pft_ntr_nbr_out;pft_idx<pft_nbr_out;pft_idx++){
      cft_nm_crr=(char *)strdup("PFT ityp %02ld, CFT ityp %02ld");
      sprintf(cft_nm_crr,cft_nm_crr,pft_idx,pft_idx-pft_ntr_nbr_out);
      if(fl_out_fmt == NC_FORMAT_NETCDF4){
	pft_sng_out[pft_idx]=cft_nm_crr;
      }else{
	strcpy(pft_chr_out+pft_idx*pft_sng_lng_out,cft_nm_crr);
      } /* !fl_out_fmt */
    } /* !pft_idx */
#endif /* !false */
    if(fl_out_fmt == NC_FORMAT_NETCDF4){
      rcd+=nco_def_var(out_id,pft_nm_out,NC_STRING,dmn_nbr_1D,dmn_ids_out,&pft_out_id);
    }else{
      rcd+=nco_def_var(out_id,pft_nm_out,NC_CHAR,dmn_nbr_2D,dmn_ids_out,&pft_out_id);
    } /* !fl_out_fmt */
    var_crt_nbr++;
    rcd=nco_char_att_put(out_id,pft_nm_out,"long_name","PFT Descriptor");
    rcd=nco_char_att_put(out_id,pft_nm_out,"note","Storage uses C (0-based indexing) convention and includes space for PFT ityp 0 == bare ground/not vegetated. Thus PFT ityp equals the storage index. For example, storage index 0 is PFT type 0 == Not vegetated and storage index 1 is PFT ityp 1 == Needleleaf evergreen temperate tree. Often the last natural PFT is ityp 14 == C4 grass, although sometimes it is ityp 16 == C3 irrigated. The presence of crop PFTs (i.e., CFTs) in data fields is indicated by an extended PFT type index whose enumeration is sequential with natural PFTs. Often the first crop is c3_crop with PFT ityp 15 and CFT ityp = 1, although sometimes it is corn with PFT ityp 17 and CFT ityp = 1. The last CFT ityp is the total PFT dimension size minus the number of natural PFTs.");

    // As of 20241022, glean as many PFT/CFT names as possible from global attributes
    char att_nm[NC_MAX_NAME+1L]; /* [sng] Attribute name */
    int att_glb_nbr; /* [nbr] Number of global attributes */
    rcd=nco_inq(in_id,(int *)NULL,(int *)NULL,&att_glb_nbr,(int *)NULL);
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG att_glb_nbr = %d\n",nco_prg_nm_get(),att_glb_nbr);
    int att_idx;
    long att_sz;
    nc_type att_typ;

    char cft_xpr[]="cft_";
    char pft_xpr[]="ipft_";
    char *cft_ptr=NULL;
    char *pft_ptr=NULL;
    char *cft_crr=NULL;
    char *pft_crr=NULL;
    char *usc_ptr=NULL;
    int cft_nbr_crr=0; /* [nbr] Current number of CFT attributes read */
    int pft_nbr_crr=0; /* [nbr] Current number of PFT attributes read */
    for(att_idx=0;att_idx<att_glb_nbr;att_idx++){
      rcd=nco_inq_attname(in_id,NC_GLOBAL,att_idx,att_nm);
      rcd=nco_inq_att(in_id,NC_GLOBAL,att_nm,&att_typ,&att_sz);
      if(flg_nm_hst){
	if((cft_ptr=strcasestr(att_nm,cft_xpr))){
	  cft_nbr_crr++;
	  /* Add four to eliminate "cft_" portion of attribute name */
	  cft_crr=(char *)strdup(cft_ptr+4);
	  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG %s reports cft_nbr_crr = %d, pft_idx = %ld, cft_crr = %s\n",nco_prg_nm_get(),fnc_nm,cft_nbr_crr,pft_idx,cft_crr);
	  /* Capitalize first word for nicer labels */
	  cft_crr[0]=toupper(cft_crr[0]);
	  /* Remove underscores */
	  usc_ptr=cft_crr;
	  while((usc_ptr=strchr(usc_ptr,'_')) != NULL) *usc_ptr++=' ';
	  pft_idx=pft_ntr_nbr_in-1+cft_nbr_crr;
	  if(fl_out_fmt == NC_FORMAT_NETCDF4){
	    if(pft_idx >= 0) pft_sng_out[pft_idx]=cft_crr;
	  }else{
	    if(pft_idx >= 0) strcpy(pft_chr_out+pft_idx*pft_sng_lng_out,cft_crr);
	  } /* !fl_out_fmt */
	} /* !cft_ptr */
      } /* !flg_nm_hst */
      if(flg_nm_rst){
	if((pft_ptr=strcasestr(att_nm,pft_xpr))){
	  pft_nbr_crr++;
	  pft_idx=pft_nbr_crr-1;
	  /* Restart datasets can contain "ipft_..." attributes for inactive crop types that are not included in pft_nbr_out */
	  if(pft_idx < pft_nbr_out){
	    /* Add five to eliminate "ipft_" portion of attribute name */
	    pft_crr=(char *)strdup(pft_ptr+5);
	    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG %s reports pft_nbr_crr = %d, pft_idx = %ld, pft_crr = %s\n",nco_prg_nm_get(),fnc_nm,pft_nbr_crr,pft_idx,pft_crr);
	    /* Capitalize first word for nicer labels */
	    pft_crr[0]=toupper(pft_crr[0]);
	    /* Remove underscores */
	    usc_ptr=pft_crr;
	    while((usc_ptr=strchr(usc_ptr,'_')) != NULL) *usc_ptr++=' ';
	    if(fl_out_fmt == NC_FORMAT_NETCDF4){
	      if(pft_idx >= 0) pft_sng_out[pft_idx]=pft_crr;
	    }else{
	      if(pft_idx >= 0) strcpy(pft_chr_out+pft_idx*pft_sng_lng_out,pft_crr);
	    } /* !fl_out_fmt */
	  } /* !pft_idx */
	} /* !pft_ptr */
      } /* !flg_nm_rst */
    } /* !att_idx */
    if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO Read cft_nbr_crr = %d CFT attributes and pft_nbr_crr = %d PFT attributes\n",nco_prg_nm_get(),cft_nbr_crr,pft_nbr_crr);
    (void)fflush(stdout);
  } /* !need_pft */
  
  double mss_val_dbl;
  double mss_val_cmp_dbl; /* Missing value for comparison to double precision values */
  
  int flg_pck; /* [flg] Variable is packed on disk  */
  int nco_s1d_typ; /* [enm] Sparse-1D type of input variable */
  
  long mrv_idx; /* [idx] MRV index */
  long mrv_nbr; /* [nbr] Product of sizes of dimensions following (thus MRV than) column|gridcell|landunit|pft|topounit dimension, e.g., lev*|numrad. NB: mrv_nbr >= 1. */

  nco_bool flg_add_spt_crd; /* [flg] Add spatial coordinates to S1D variable */
  nco_bool has_mss_val; /* [flg] Has numeric missing value attribute */

  ptr_unn var_val_in;
  ptr_unn var_val_out;
  
  size_t var_sz_in; /* [nbr] Number of elements in variable (will be self-multiplied) */
  size_t var_sz_out; /* [nbr] Number of elements in variable (will be self-multiplied) */

  val_unn mss_val_unn; /* [frc] Missing value union */

  /* From vertical interpolation...Maybe not needed? */
  float mss_val_flt;
  aed_sct aed_mtd_fll_val;

  /* Define unpacked S1D and copied variables in output file */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      var_nm=trv.nm;
      /* Preserve input type in output type */
      var_typ_in=trv.var_typ;
      var_typ_out=var_typ_in;
      dmn_nbr_in=trv.nbr_dmn;
      dmn_nbr_out=trv.nbr_dmn;
      rcd=nco_inq_varid(in_id,var_nm,&var_id_in);
      rcd=nco_inq_varid_flg(out_id,var_nm,&var_id_out);
      /* If variable has not been defined, define it */
      if(rcd != NC_NOERR){
	if(trv.flg_rgr){
	  /* Unpack */
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_ids_in);
	  rcd=nco_inq_var_packing(in_id,var_id_in,&flg_pck);
	  if(flg_pck) (void)fprintf(stdout,"%s: WARNING %s reports S1D variable \"%s\" is packed so results unpredictable. HINT: If regridded values seems weird, retry after unpacking input file with, e.g., \"ncpdq -U in.nc out.nc\"\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  /* Pre-compute variables that depend on all dimensions and may be needed inside next dimension loop */
	  has_levcan=has_levgrnd=has_levlak=has_levsno=has_levsno1=has_levtot=has_mec=has_numrad=False;
	  var_sz_in=1L;
	  mrv_nbr=1L;
	  flg_add_spt_crd=False;
	  flg_var_mpt=False;
	  nco_s1d_typ=nco_s1d_nil;
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    dmn_id=dmn_ids_in[dmn_idx];
	    rcd=nco_inq_dimlen(in_id,dmn_id,dmn_cnt_in+dmn_idx);
	    var_sz_in*=dmn_cnt_in[dmn_idx];
	    if(dmn_id == dmn_id_clm_in){
	      has_clm=True;
	      nco_s1d_typ=nco_s1d_clm;
	    }else if(dmn_id == dmn_id_grd_in){
	      has_grd=True;
	      nco_s1d_typ=nco_s1d_grd;
	    }else if(dmn_id == dmn_id_lnd_in){
	      has_lnd=True;
	      nco_s1d_typ=nco_s1d_lnd;
	    }else if(dmn_id == dmn_id_pft_in){
	      has_pft=True;
	      nco_s1d_typ=nco_s1d_pft;
	    }else if(dmn_id == dmn_id_tpo_in){
	      has_tpo=True;
	      nco_s1d_typ=nco_s1d_tpo;
	    }else if(dmn_id == dmn_id_levcan_in){
	      has_levcan=True;
	      mrv_nbr*=levcan_nbr_in;
	    }else if(dmn_id == dmn_id_levgrnd_in){
	      has_levgrnd=True;
	      mrv_nbr*=levgrnd_nbr_in;
	    }else if(dmn_id == dmn_id_levlak_in){
	      has_levlak=True;
	      mrv_nbr*=levlak_nbr_in;
	    }else if(dmn_id == dmn_id_levsno_in){
	      has_levsno=True;
	      mrv_nbr*=levsno_nbr_in;
	    }else if(dmn_id == dmn_id_levsno1_in){
	      has_levsno1=True;
	      mrv_nbr*=levsno1_nbr_in;
	    }else if(dmn_id == dmn_id_levtot_in){
	      has_levtot=True;
	      mrv_nbr*=levtot_nbr_in;
	    }else if(dmn_id == dmn_id_numrad_in){
	      has_numrad=True;
	      mrv_nbr*=numrad_nbr_in;
	    } /* !dmn_id */
	  } /* !dmn_idx */
	  if(need_mec && has_clm){
	    /* File contains variables with MEC dimension, so determine has_mec for this variable */
	    var_val_in.vp=(void *)nco_malloc_dbg(var_sz_in*nco_typ_lng(var_typ_in),fnc_nm,"Unable to malloc() input value buffer");
	    rcd=nco_get_var(in_id,var_id_in,var_val_in.vp,var_typ_in);
	    has_mss_val=nco_mss_val_get_dbl(in_id,var_id_in,&mss_val_dbl);
	    if(has_mss_val) mss_val_cmp_dbl=mss_val_dbl; else mss_val_cmp_dbl=NC_FILL_DOUBLE;
	    /* Override float/double value with appropriate default missing value for integers */
	    if(!has_mss_val){
	      switch(var_typ_out){
	      case NC_INT: mss_val_cmp_dbl=NC_FILL_INT; break;
	      case NC_FLOAT: mss_val_cmp_dbl=NC_FILL_FLOAT; break;
	      case NC_DOUBLE: mss_val_cmp_dbl=NC_FILL_DOUBLE; break;
	      default: nco_dfl_case_nc_type_err(); break;
	      } /* !var_typ_out */
	    } /* !has_mss_val */
	    /* Determine what landunit this variable is defined on
	       Ratio of idx_in (an element counter) on first valid landunit by mrv_nbr yields a clm/lnd/pft/tpo index */
	    idx_s1d_crr=NC_MIN_INT;

	    switch(var_typ_in){
	    case NC_FLOAT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
		if(var_val_in.fp[idx_in] != 0.0f && var_val_in.fp[idx_in] != mss_val_cmp_dbl){
		  idx_s1d_crr=idx_in/mrv_nbr;
		  break;
		} /* !var_val_in.fp */
	      break;
	    case NC_DOUBLE: for(idx_in=0;idx_in<var_sz_in;idx_in++)
		if(var_val_in.dp[idx_in] != 0.0 && var_val_in.dp[idx_in] != mss_val_cmp_dbl){
		  idx_s1d_crr=idx_in/mrv_nbr;
		  break;
		} /* !var_val_in.dp */
	      break;
	    case NC_INT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
		if(var_val_in.ip[idx_in] != 0 && var_val_in.ip[idx_in] != mss_val_cmp_dbl){
		  idx_s1d_crr=idx_in/mrv_nbr;
		  break;
		} /* !var_val_in.ip */
	      break;
	    default:
	      (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
	      nco_dfl_case_nc_type_err();
	      break;
	    } /* !var_typ_in */
	    if(idx_in == var_sz_in){
	      (void)fprintf(fp_stdout,"%s: INFO %s reports %s has no valid values, output will be all _FillValue\n",nco_prg_nm_get(),fnc_nm,var_nm);
	      flg_var_mpt=True;
	    } /* !idx_in */
	    lnd_typ_vld_1st=NC_MIN_INT;
	    if(idx_s1d_crr != NC_MIN_INT){
	      switch(nco_s1d_typ){
	      case nco_s1d_clm:	lnd_typ_vld_1st=cols1d_ityplun[idx_s1d_crr]; break;
	      case nco_s1d_lnd:	lnd_typ_vld_1st=land1d_ityplun[idx_s1d_crr]; break;
	      case nco_s1d_pft:	lnd_typ_vld_1st=pfts1d_ityplun[idx_s1d_crr]; break;
	      case nco_s1d_grd: lnd_typ_vld_1st=nco_lnd_ilun_nil; break; /* Gridcell variables may contain multiple landunits */
	      case nco_s1d_tpo: lnd_typ_vld_1st=nco_lnd_ilun_nil; break; /* Topounit variables may contain multiple landunits */
	      default: break;
	      } /* !nco_s1d_typ */
	      /* Is variable is defined on MECs? */
	      if(nco_s1d_typ == nco_s1d_clm){
		nco_bool val_vld=False;
		for(clm_idx=0;clm_idx<clm_nbr_in;clm_idx++){
		  /* Restrict hunt for valid values to MEC LUTs */
		  if(cols1d_ityplun[clm_idx] != ilun_landice_multiple_elevation_classes) continue;
		  for(mrv_idx=0;mrv_idx<mrv_nbr;mrv_idx++){
		    idx_in=clm_idx*mrv_nbr+mrv_idx;
		    /* Break-out of MRV loop at first valid value */
		    switch(var_typ_in){
		    case NC_FLOAT: if(var_val_in.fp[idx_in] != 0.0f && var_val_in.fp[idx_in] != mss_val_cmp_dbl) val_vld=True;
		      break;
		    case NC_DOUBLE:
		      if(var_val_in.dp[idx_in] != 0.0 && var_val_in.dp[idx_in] != mss_val_cmp_dbl) val_vld=True;
		      break;
		    case NC_INT: if(var_val_in.ip[idx_in] != 0 && var_val_in.ip[idx_in] != mss_val_cmp_dbl) val_vld=True;
		      break;
		    default:
		      (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
		      nco_dfl_case_nc_type_err();
		      break;
		    } /* !var_typ_in */
		    /* Break-out of column loop at first valid value */
		    if(val_vld) break;
		  } /* !mrv_idx */
		  if(val_vld) break;
		} /* !clm_idx */
		if(val_vld) has_mec=trv_tbl->lst[idx_tbl].has_mec=True;
		if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_stdout,"%s: DEBUG %s exited MEC search at mrv_idx = %ld, clm_idx = %ld, idx_in = %ld, has_mec = %d\n",nco_prg_nm_get(),var_nm,mrv_idx,clm_idx,idx_in,has_mec);
	      } /* !nco_s1d_typ */
	    } /* !idx_s1d_crr */
	    if(var_val_in.vp) var_val_in.vp=(void *)nco_free(var_val_in.vp);
	  } /* !need_mec */
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    dmn_id=dmn_ids_in[dmn_idx];
	    rcd=nco_inq_dimname(in_id,dmn_id,dmn_nm);
	    if(dmn_id == dmn_id_clm_in){
	      if(has_mec){
		/* Change input column dimension to MEC when present otherwise eliminate this dimension */
		dmn_ids_out[dmn_idx]=dmn_id_mec_out;
		dmn_cnt_out[dmn_idx]=mec_nbr_outp1;
	      }else{ /* !has_mec */
		dmn_nbr_out--;
		dmn_ids_out[dmn_idx]=NC_MIN_INT;
	      } /* !has_mec */
	      flg_add_spt_crd=True;
	      /* !dmn_id_clm_in */
	    }else if(dmn_id == dmn_id_grd_in){
	      /* Gridcell dimension disappears to become spatial dimension in output */
	      dmn_nbr_out--;
	      dmn_ids_out[dmn_idx]=NC_MIN_INT;
	      flg_add_spt_crd=True;
	      /* !dmn_id_grd_in */
	    }else if(dmn_id == dmn_id_lnd_in){
	      dmn_nbr_out--;
	      dmn_ids_out[dmn_idx]=NC_MIN_INT;
	      flg_add_spt_crd=True;
	      /* !dmn_id_lnd_in */
	    }else if(dmn_id == dmn_id_pft_in){
	      if(pft_nbr_out > 0L){
		/* Change input PFT dimension to PFT if present */
		dmn_ids_out[dmn_idx]=dmn_id_pft_out;
		dmn_cnt_out[dmn_idx]=pft_nbr_out;
	      }else{ /* !pft_nbr_out */
		dmn_nbr_out--;
		dmn_ids_out[dmn_idx]=NC_MIN_INT;
	      }	/* !pft_nbr_out */
	      flg_add_spt_crd=True;
	      /* !dmn_id_pft_in */
	    }else if(dmn_id == dmn_id_tpo_in){
	      /* Topounit dimension disappears to become spatial dimension in output */
	      dmn_nbr_out--;
	      dmn_ids_out[dmn_idx]=NC_MIN_INT;
	      flg_add_spt_crd=True;
	      /* !dmn_id_tpo_in */
	    }else{
	      /* Dimensions [clm/lnd/pft]_nm_in were pre-defined above as [clm/lnd/pft]_nm_out, replicate all other explicit dimensions (e.g., time|lev*|numrad) */
	      rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_ids_out+dmn_idx);
	    } /* !dmn_id */
	    if(rcd != NC_NOERR){
	      /* Current input dimension is not yet in output file */
	      rcd=nco_inq_dimlen(in_id,dmn_id,dmn_cnt_out+dmn_idx);
	      /* Check-for and, if found, retain record dimension property */
	      for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		if(dmn_id == dmn_ids_rec[dmn_rec_idx])
		  dmn_cnt_out[dmn_idx]=NC_UNLIMITED;
	      rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt_out[dmn_idx],dmn_ids_out+dmn_idx);
	    } /* !rcd */
	  } /* !dmn_idx */
	  /* Repack output dimension arrays if rank shrank */
	  if(dmn_nbr_out != dmn_nbr_in){
	    assert(dmn_nbr_in-dmn_nbr_out <= 1);
	    dmn_idx_swp=0;
	    for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	      if(dmn_ids_out[dmn_idx] != NC_MIN_INT){
		dmn_ids_swp[dmn_idx_swp]=dmn_ids_out[dmn_idx];
		dmn_cnt_swp[dmn_idx_swp]=dmn_cnt_out[dmn_idx];
		dmn_idx_swp++;
	      } /* !dmn_ids_out */
	    } /* !dmn_idx */
	    memcpy(dmn_ids_out,dmn_ids_swp,dmn_nbr_out*sizeof(int));
	    memcpy(dmn_cnt_out,dmn_cnt_swp,dmn_nbr_out*sizeof(int));
	  } /* !dmn_nbr_out */
	  /* Add trailing spatial dimension(s) (i.e., [lat,lon] or [lndgrid], in MRV position(s) */ 
	  if(flg_add_spt_crd){
	    if(flg_grd_1D){
	      dmn_ids_out[dmn_nbr_out]=dmn_id_col_out;
	      dmn_cnt_out[dmn_nbr_out]=col_nbr;
	      dmn_nbr_out++;
	    } /* !flg_grd_1D */
	    if(flg_grd_2D){
	      dmn_ids_out[dmn_nbr_out]=dmn_id_lat_out;
	      dmn_cnt_out[dmn_nbr_out]=lat_nbr;
	      dmn_ids_out[dmn_nbr_out+1]=dmn_id_lon_out;
	      dmn_cnt_out[dmn_nbr_out+1]=lon_nbr;
	      dmn_nbr_out+=2;
	    } /* !flg_grd_2D */
	    assert(dmn_nbr_out <= dmn_nbr_max);
	  } /* !flg_add_spt_crd */
	}else{ /* !flg_rgr */
	  /* Replicate non-S1D variables */
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_ids_in);
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    dmn_id=dmn_ids_in[dmn_idx];
	    rcd=nco_inq_dimname(in_id,dmn_id,dmn_nm);
	    rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_ids_out+dmn_idx);
	    if(rcd != NC_NOERR){
	      rcd=nco_inq_dimlen(in_id,dmn_id,dmn_cnt_out+dmn_idx);
	      /* Check-for and, if found, retain record dimension property */
	      for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		if(dmn_id == dmn_ids_rec[dmn_rec_idx])
		  dmn_cnt_out[dmn_idx]=NC_UNLIMITED;
	      rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt_out[dmn_idx],dmn_ids_out+dmn_idx);
	    } /* !rcd */
	  } /* !dmn_idx */
	} /* !flg_rgr */
	rcd=nco_def_var(out_id,var_nm,var_typ_out,dmn_nbr_out,dmn_ids_out,&var_id_out);
	/* Duplicate netCDF4 settings when possible */
	if(fl_out_fmt == NC_FORMAT_NETCDF4 || fl_out_fmt == NC_FORMAT_NETCDF4_CLASSIC)
	  if(dmn_nbr_out > 0)
	    rcd=nco_flt_def_wrp(in_id,var_id_in,(char *)NULL,out_id,var_id_out);
	(void)nco_att_cpy(in_id,out_id,var_id_in,var_id_out,PCK_ATT_CPY);
	/* 20240131: This _FillValue block is copied from vertical interpolation, and is not yet (and may never be) needed
	   Variables with subterranean levels and missing-value extrapolation must have _FillValue attribute */
	nco_bool flg_add_msv_att; /* [flg] Extrapolation requires _FillValue */
	flg_add_msv_att=False;
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
  } /* !idx_tbl */

  /* Turn-off default filling behavior to enhance efficiency */
  nco_set_fill(out_id,NC_NOFILL,&fll_md_old);
      
  /* Begin data mode */
  (void)nco_enddef(out_id);

  /* Copy coordinate system before closing template file
     NB: nco_cpy_var_val() cannot be used here when coordinates are in fl_tpl not fl_in */
  (void)nco_cpy_var_val_lmt(hrz_id,out_id,(FILE *)NULL,lat_nm_in,(lmt_sct *)NULL,(int)0);
  (void)nco_cpy_var_val_lmt(hrz_id,out_id,(FILE *)NULL,lon_nm_in,(lmt_sct *)NULL,(int)0);
  if(flg_lat_bnd_out) (void)nco_cpy_var_val_lmt(hrz_id,out_id,(FILE *)NULL,lat_bnd_nm,(lmt_sct *)NULL,(int)0);
  if(flg_lon_bnd_out) (void)nco_cpy_var_val_lmt(hrz_id,out_id,(FILE *)NULL,lon_bnd_nm,(lmt_sct *)NULL,(int)0);
  if(flg_sgs_frc_out) (void)nco_cpy_var_val_lmt(hrz_id,out_id,(FILE *)NULL,sgs_frc_nm,(lmt_sct *)NULL,(int)0);
  if(flg_sgs_msk_out) (void)nco_cpy_var_val_lmt(hrz_id,out_id,(FILE *)NULL,sgs_msk_nm,(lmt_sct *)NULL,(int)0);
  
  if(flg_grd_tpl){
    nco_bool RM_RMT_FL_PST_PRC=True; /* Option R */
    
    /* No further access to template file, close it */
    nco_close(tpl_id);

    /* Remove local copy of file */
    if(FL_RTR_RMT_LCN && RM_RMT_FL_PST_PRC) (void)nco_fl_rm(fl_tpl);
  } /* !flg_grd_tpl */

  if(need_levgrnd && levgrnd_out_id != NC_MIN_INT) rcd=nco_put_var(out_id,levgrnd_out_id,(void *)levgrnd,NC_DOUBLE);
  if(need_levlak && levlak_out_id != NC_MIN_INT) rcd=nco_put_var(out_id,levlak_out_id,(void *)levlak,NC_DOUBLE);
  if(need_mec && mec_out_id != NC_MIN_INT) rcd=nco_put_var(out_id,mec_out_id,(void *)mec,NC_DOUBLE);
  if(need_pft && pft_out_id != NC_MIN_INT){
    if(fl_out_fmt == NC_FORMAT_NETCDF4){
      rcd=nco_put_var(out_id,pft_out_id,(void *)pft_sng_out,NC_STRING);
    }else{
      rcd=nco_put_var(out_id,pft_out_id,(void *)pft_chr_out,NC_CHAR);
    } /* !fl_out_fmt */
  } /* !need_pft */

  /* Diagnose and write area-fractions for column-types of interest */
  if(flg_frc_column_out){
    double *frc_column=NULL; /* [frc] Column weight relative to corresponding gridcell */
    int clm_idx_ttl=NC_MIN_INT; /* [idx] Output column index to store grand total weight of natural columns */
    int clm_idx_mec_ttl=NC_MIN_INT; /* [idx] Output column index to store sub-total weight of MEC columns */
    int idx_ttl_out; /* [idx] Index to store total weight of soil+MEC columns */

    frc_column=(double *)nco_calloc(clm_nbr_out*grd_nbr_out,sizeof(double));

    /* MECs: Store soil column in column index 0, MECs in indexes 1--10, total MECs in 11, lakes in 12, wetlands in 13, and grand total in index 14
       No-MECs: Store soil column in column index 0, glaciated in index 1, lakes in 2, wetlands in 3, and grand total in index 4 */
    clm_idx_mec_ttl=mec_nbr_out+1;
    clm_idx_ttl=clm_nbr_out-1;

    for(clm_idx=0;clm_idx < clm_nbr_in;clm_idx++){
      clm_typ=cols1d_ityp[clm_idx];
      if(clm_typ == nco_clm_icol_vegetated_or_bare_soil) clm_idx_out=0;
      else if(clm_typ == nco_clm_icol_landice) clm_idx_out=1; 
      else if(clm_typ >= nco_clm_icol_landice_multiple_elevation_class_01 && clm_typ <= nco_clm_icol_landice_multiple_elevation_class_10) clm_idx_out=clm_typ % clm_typ_mec_fst;
      else if(clm_typ == nco_clm_icol_deep_lake) clm_idx_out= need_mec ? 12 : 2;
      else if(clm_typ == nco_clm_icol_wetland) clm_idx_out= need_mec ? 13 : 3;
      else continue; /* Ignore Urban columns and Vegetated columns for now */
      /* Subtract one to shift from input 1-based (Fortran) convention to output 0-based (C) convention */
      grd_idx_out= flg_grd_1D ? cols1d_ixy[clm_idx]-1L : (cols1d_jxy[clm_idx]-1L)*lon_nbr+(cols1d_ixy[clm_idx]-1L);
      idx_out=clm_idx_out*grd_nbr_out+grd_idx_out;
      frc_column[idx_out]+=cols1d_wtxy[clm_idx];
      if(clm_typ >= nco_clm_icol_landice_multiple_elevation_class_01 && clm_typ <= nco_clm_icol_landice_multiple_elevation_class_10){
	/* Sub-total over MECs */
	idx_ttl_out=clm_idx_mec_ttl*grd_nbr_out+grd_idx_out;
	frc_column[idx_ttl_out]+=cols1d_wtxy[clm_idx];
      } /* !clm_typ */
      /* Grand total over natural columns */
      idx_ttl_out=clm_idx_ttl*grd_nbr_out+grd_idx_out;
      frc_column[idx_ttl_out]+=cols1d_wtxy[clm_idx];
    } /* !clm_idx */
    if(frc_column_out_id != NC_MIN_INT) rcd=nco_put_var(out_id,frc_column_out_id,(void *)frc_column,NC_DOUBLE);
    if(frc_column) frc_column=(double *)nco_free(frc_column);
  } /* !flg_frc_column_out */

  /* Diagnose and write area-fractions for all landunits */
  if(flg_frc_landunit_out){
    double *frc_landunit=NULL; /* [frc] Landunit weight relative to corresponding gridcell */
    frc_landunit=(double *)nco_calloc(lnd_nbr_out*grd_nbr_out,sizeof(double));

    for(lnd_idx=0;lnd_idx < lnd_nbr_in;lnd_idx++){
      lnd_typ=land1d_ityplun[lnd_idx];
      lnd_idx_out=lnd_typ % lnd_nbr_out;
      /* Subtract one to shift from input 1-based (Fortran) convention to output 0-based (C) convention */
      grd_idx_out= flg_grd_1D ? land1d_ixy[lnd_idx]-1L : (land1d_jxy[lnd_idx]-1L)*lon_nbr+(land1d_ixy[lnd_idx]-1L);
      idx_out=lnd_idx_out*grd_nbr_out+grd_idx_out;
      frc_landunit[idx_out]+=land1d_wtxy[lnd_idx];
      /* Place total area in output landunit index 0 */
      frc_landunit[grd_idx_out]+=land1d_wtxy[lnd_idx];
    } /* !lnd_idx */

    if(frc_landunit_out_id != NC_MIN_INT) rcd=nco_put_var(out_id,frc_landunit_out_id,(void *)frc_landunit,NC_DOUBLE);
    if(frc_landunit) frc_landunit=(double *)nco_free(frc_landunit);
  } /* !flg_frc_landunit_out */
  
  /* Free pre-allocated array space */
  if(dmn_ids_in) dmn_ids_in=(int *)nco_free(dmn_ids_in);
  if(dmn_ids_out) dmn_ids_out=(int *)nco_free(dmn_ids_out);
  if(dmn_ids_swp) dmn_ids_swp=(int *)nco_free(dmn_ids_swp);
  if(dmn_ids_rec) dmn_ids_rec=(int *)nco_free(dmn_ids_rec);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
  if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
  if(dmn_cnt_swp) dmn_cnt_swp=(long *)nco_free(dmn_cnt_swp);

  /* Unpack and copy data from input file */
  int thr_idx; /* [idx] Thread index */

  long levsno_idx_in; /* [idx] Input snowpack level index */
  long levsno_idx_out; /* [idx] Output snowpack level index */

  long lvl_idx; /* [idx] Level index */
  long lvl_nbr; /* [nbr] Number of levels */

  size_t val_in_fst; /* [nbr] Number of elements by which current N-D slab input values are offset from origin */
  size_t val_out_fst; /* [nbr] Number of elements by which current N-D slab output values are offset from origin */

  if(idx_dbg == 0L) idx_dbg=11;
  if(nco_dbg_lvl_get() >= nco_dbg_var){
    if(need_clm){
      for(clm_idx=0;clm_idx<idx_dbg;clm_idx++){
	clm_typ=cols1d_ityp[clm_idx];      
	lnd_typ=cols1d_ityplun[clm_idx];      
	(void)fprintf(fp_stdout,"%s: clm_idx = %ld, lnd_typ = %d %s, clm_typ = %d = %s, active = %d\n",nco_prg_nm_get(),clm_idx,lnd_typ,nco_lnd_typ_sng(lnd_typ),clm_typ,nco_clm_typ_sng(clm_typ),cols1d_active[clm_idx]);
      } /* !clm_idx */
    } /* !need_clm */
    if(need_lnd){
      for(lnd_idx=0;lnd_idx<idx_dbg;lnd_idx++){
	lnd_typ=land1d_ityplun[lnd_idx];      
	(void)fprintf(fp_stdout,"%s: lnd_idx = %ld, lnd_typ = %d = %s, active = %d\n",nco_prg_nm_get(),lnd_idx,lnd_typ,nco_lnd_typ_sng(lnd_typ),land1d_active[lnd_idx]);
      } /* !clm_idx */
    } /* !need_clm */
    if(need_pft){
      for(pft_idx=0;pft_idx<idx_dbg;pft_idx++){
	pft_typ=pfts1d_ityp_veg[pft_idx];      
	lnd_typ=pfts1d_ityplun[pft_idx];      
	(void)fprintf(fp_stdout,"%s: pft_idx = %ld, lnd_typ = %d = %s, pft_typ = %d = %s, active = %d\n",nco_prg_nm_get(),pft_idx,lnd_typ,nco_lnd_typ_sng(lnd_typ),pft_typ,nco_pft_typ_sng(pft_typ),pfts1d_active[pft_idx]);
      } /* !clm_idx */
    } /* !need_clm */
  } /* !dbg */
  
#ifdef __GNUG__
# pragma omp parallel for firstprivate(lut_vld_flg,var_val_in,var_val_out) private(clm_typ,clm_wgt,dmn_cnt_in,dmn_cnt_out,dmn_id,dmn_ids_in,dmn_ids_out,dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dmn_nm,dmn_srt,flg_snw_rdr,flg_var_mpt,has_clm,has_grd,has_levcan,has_levgrnd,has_levlak,has_levsno,has_levsno1,has_levtot,has_lnd,has_mec,has_mss_val,has_numrad,has_pft,has_tpo,idx_in,idx_out,idx_s1d_crr,idx_tbl,in_id,levsno_idx_in,levsno_idx_out,lnd_typ,lnd_typ_vld_1st,lvl_idx,lvl_nbr,mec_idx_in,mec_idx_out,mrv_idx,mrv_nbr,mss_val,mss_val_dbl,mss_val_cmp_dbl,mss_val_unn,nco_s1d_typ,pft_typ,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_in,var_typ_out) shared(clm_nbr_in,clm_nbr_out,clm_typ_mec_fst,cols1d_ityp,cols1d_ityplun,cols1d_ixy,cols1d_jxy,col_nbr,dmn_id_clm_in,dmn_id_clm_out,dmn_id_col_in,dmn_id_col_out,dmn_id_lat_in,dmn_id_lat_out,dmn_id_levcan_in,dmn_id_levgrnd_in,dmn_id_levlak_in,dmn_id_levsno_in,dmn_id_levsno1_in,dmn_id_levtot_in,dmn_id_lnd_in,dmn_id_lnd_out,dmn_id_lon_in,dmn_id_lon_out,dmn_id_numrad_in,dmn_id_pft_in,dmn_id_pft_out,dmn_id_tpo_in,dmn_nbr_hrz_crd,flg_nm_hst,flg_nm_rst,flg_s1d_clm,flg_s1d_pft,flg_snw_ocn,lat_nbr,idx_dbg,ilun_landice_multiple_elevation_classes,land1d_ityplun,lnd_nbr_in,lnd_nbr_out,lon_nbr,lut_out,mec_nbr_out,need_mec,out_id,pft_nbr_in,pft_nbr_out,pfts1d_ityplun,pfts1d_ityp_veg,pfts1d_ixy,pfts1d_jxy,snl_var,tpo_nbr_in,tpo_nbr_out,topo1d_ixy,topo1d_jxy)
#endif /* !__GNUG__ */
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
	/* Unpack variable */
	var_nm=trv.nm;
	var_typ_in=trv.var_typ;
	var_typ_out=var_typ_in;
	var_sz_in=1L;
	var_sz_out=1L;
	rcd=nco_inq_varid(in_id,var_nm,&var_id_in);
	rcd=nco_inq_varid(out_id,var_nm,&var_id_out);
	rcd=nco_inq_varndims(in_id,var_id_in,&dmn_nbr_in);
	rcd=nco_inq_varndims(out_id,var_id_out,&dmn_nbr_out);
	dmn_nbr_max= dmn_nbr_in > dmn_nbr_out ? dmn_nbr_in : dmn_nbr_out;
	dmn_ids_in=(int *)nco_malloc(dmn_nbr_in*sizeof(int));
	dmn_ids_out=(int *)nco_malloc(dmn_nbr_out*sizeof(int));
	dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long)); /* max() for both input and output grids */
	dmn_cnt_in=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
	dmn_cnt_out=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
	rcd=nco_inq_vardimid(in_id,var_id_in,dmn_ids_in);
	rcd=nco_inq_vardimid(out_id,var_id_out,dmn_ids_out);
	has_levcan=has_levgrnd=has_levlak=has_levsno=has_levsno1=has_levtot=has_mec=has_numrad=False;
	has_mec=trv_tbl->lst[idx_tbl].has_mec; /* Use previously diagnosed value */
	flg_snw_rdr=False;
	flg_var_mpt=False;
	/* 20240202: Identify special dimensions in input variable
	   mrv_nbr is product of sizes of dimensions following (thus MRV) column|gridcell|landunit|pft|topounit
	   mrv_nbr == 1L for variables that have no dimensions other than column|gridcell|landunit|pft|topounit */
	mrv_nbr=1L;
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	  dmn_id=dmn_ids_in[dmn_idx];
	  rcd=nco_inq_dimlen(in_id,dmn_id,dmn_cnt_in+dmn_idx);
	  var_sz_in*=dmn_cnt_in[dmn_idx];
	  dmn_srt[dmn_idx]=0L;
	  if(dmn_id == dmn_id_levcan_in){
	    has_levcan=True;
	    mrv_nbr*=levcan_nbr_in;
	  }else if(dmn_id == dmn_id_levgrnd_in){
	    has_levgrnd=True;
	    mrv_nbr*=levgrnd_nbr_in;
	  }else if(dmn_id == dmn_id_levlak_in){
	    has_levlak=True;
	    mrv_nbr*=levlak_nbr_in;
	  }else if(dmn_id == dmn_id_levsno_in){
	    has_levsno=True;
	    mrv_nbr*=levsno_nbr_in;
	  }else if(dmn_id == dmn_id_levsno1_in){
	    has_levsno1=True;
	    mrv_nbr*=levsno1_nbr_in;
	  }else if(dmn_id == dmn_id_levtot_in){
	    has_levtot=True;
	    mrv_nbr*=levtot_nbr_in;
	  }else if(dmn_id == dmn_id_numrad_in){
	    has_numrad=True;
	    mrv_nbr*=numrad_nbr_in;
	  } /* !dmn_id */
	  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_stdout,"%s: INFO define block for %s dmn_idx = %d, dmn_id = %d, mrv_nbr = %ld\n",nco_prg_nm_get(),var_nm,dmn_idx,dmn_id,mrv_nbr);
	} /* !dmn_idx */
	
	for(dmn_idx=0;dmn_idx<dmn_nbr_out;dmn_idx++){
	  rcd=nco_inq_dimlen(out_id,dmn_ids_out[dmn_idx],dmn_cnt_out+dmn_idx);
	  if(dmn_cnt_out[dmn_idx] == 0L){
	    /* No records have been written, so overwrite zero output record size with input record size */
	    char dmn_rec_nm[NC_MAX_NAME]; /* [sng] Record dimension name */
	    int dmn_rec_id_in;
	    rcd=nco_inq_dimname(out_id,dmn_ids_out[dmn_idx],dmn_rec_nm);
	    rcd=nco_inq_dimid(in_id,dmn_rec_nm,&dmn_rec_id_in);
	    rcd=nco_inq_dimlen(in_id,dmn_rec_id_in,dmn_cnt_out+dmn_idx);
	  } /* !dmn_cnt_out */
	  var_sz_out*=dmn_cnt_out[dmn_idx];
	  dmn_srt[dmn_idx]=0L;
	} /* !dmn_idx */

	/* Compute number and size of non-lat/lon or non-col dimensions (e.g., level, time, crop species, wavelength)
	   Denote their convolution by level or 'lvl' for shorthand
	   There are lvl_nbr elements for each lat/lon or col position
	   ELM/CLM dimensions that count in "lvl_nbr" include all lev* dimensions, time, numrad */
	lvl_nbr=1L;
	/* Restart files are generally ordered (column|gridcell|landunit|pft|topounit,lev*|numrad)
	   Restart files unroll the MEC dimension into the column dimension
	   Restart files unroll the PFT dimension into the pft dimension
	   Restart files explicitly retain all lev*,numrad dimensions
	   NB: lev* and numrad are mutually exclusive (never both appear in same variable)
	   
	   History files are generally ordered (time,lev*,[column|pft]|[lat,lon|lndgrid])
	   Most history file fields contain MRV horizontal spatial dimensions
	   However Beth Drewniak's beth_in.nc fields and Eva Sinha's eva_h2.nc/eva_h3.nc fields have no horizontal spatial dimensions,
	   and place lev* as LRV (like normal history fields) not MRV (like normal "vanilla" restart files)
	   Examples of such fields include AR(time,pft), GPP(time,pft) */
	if(flg_nm_hst)
	  for(dmn_idx=0;dmn_idx<dmn_nbr_out-dmn_nbr_hrz_crd;dmn_idx++)
	    lvl_nbr*=dmn_cnt_out[dmn_idx];
	if(flg_nm_rst)
	  for(dmn_idx=0;dmn_idx<dmn_nbr_out-dmn_nbr_hrz_crd;dmn_idx++)
	    lvl_nbr*=1L;

	if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: DEBUG %s lvl_nbr = %ld\n",nco_prg_nm_get(),var_nm,lvl_nbr);

	var_val_in.vp=(void *)nco_malloc_dbg(var_sz_in*nco_typ_lng(var_typ_in),fnc_nm,"Unable to malloc() input value buffer");
	var_val_out.vp=(void *)nco_malloc_dbg(var_sz_out*nco_typ_lng(var_typ_out),fnc_nm,"Unable to malloc() output value buffer");
	
	/* Obtain input variable */
	rcd=nco_get_vara(in_id,var_id_in,dmn_srt,dmn_cnt_in,var_val_in.vp,var_typ_in);

	/* 20240213: Get missing value of appropriate type */
	has_mss_val=nco_mss_val_get_unn(in_id,var_id_in,&mss_val_unn);
	if(!has_mss_val){
	  switch(var_typ_in){
	  case NC_FLOAT: mss_val_unn.f=NC_FILL_FLOAT; break;
	  case NC_DOUBLE: mss_val_unn.d=NC_FILL_DOUBLE; break;
	  case NC_INT: mss_val_unn.i=NC_FILL_INT; break;
	  default:
	    (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
	    nco_dfl_case_nc_type_err();
	    break;
	  } /* !var_typ_in */
	} /* !has_mss_val */
	/* 20210909: Begin new missing value treatment */
	has_mss_val=nco_mss_val_get_dbl(in_id,var_id_in,&mss_val_dbl);
	if(has_mss_val) mss_val_cmp_dbl=mss_val_dbl; else mss_val_cmp_dbl=NC_FILL_DOUBLE;
	/* Override float/double value with appropriate default missing value for integers */
	if(!has_mss_val){
	  switch(var_typ_out){
	  case NC_BYTE: mss_val_cmp_dbl=NC_FILL_BYTE; break;
	  case NC_CHAR: mss_val_cmp_dbl=NC_FILL_CHAR; break;
	  case NC_SHORT: mss_val_cmp_dbl=NC_FILL_SHORT; break;
	  case NC_INT: mss_val_cmp_dbl=NC_FILL_INT; break;
	  case NC_FLOAT: mss_val_cmp_dbl=NC_FILL_FLOAT; break;
	  case NC_DOUBLE: mss_val_cmp_dbl=NC_FILL_DOUBLE; break;
	  case NC_UBYTE: mss_val_cmp_dbl=NC_FILL_UBYTE; break;
	  case NC_USHORT: mss_val_cmp_dbl=NC_FILL_USHORT; break;
	  case NC_UINT: mss_val_cmp_dbl=NC_FILL_UINT; break;
	    /* 20210909: Implicit type conversion generates warnings:
	       'long long' to 'double' changes value from -9223372036854775806 to -9223372036854775808 
	       'unsigned long long' to 'double' changes value from 18446744073709551614 to 18446744073709551616 
	       Warnings can be fixed with -Wimplicit-const-int-float-conversion */
	  case NC_INT64: mss_val_cmp_dbl=NC_FILL_INT64; break;
	  case NC_UINT64: mss_val_cmp_dbl=NC_FILL_UINT64; break;
	  case NC_STRING:
	  default: nco_dfl_case_nc_type_err(); break;
	  } /* !var_typ_out */
	} /* !has_mss_val */
	/* Re-initialize Boolean to True and override with False if variable _uses_ missing values
	   As of 20210909, "has_mss_val" should from here on should be interpreted as "uses_mss_val" */
	has_mss_val=True;
	for(idx_in=0;idx_in<var_sz_in;idx_in++){
	  switch(var_typ_in){
	  case NC_FLOAT: if(var_val_in.fp[idx_in] == mss_val_cmp_dbl) break;
	    break;
	  case NC_DOUBLE: if(var_val_in.dp[idx_in] == mss_val_cmp_dbl) break;
	    break;
	  case NC_INT: if(var_val_in.ip[idx_in] == mss_val_cmp_dbl) break;
	    break;
	  default:
	    (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
	    nco_dfl_case_nc_type_err();
	    break;
	  } /* !var_typ_in */
	} /* !idx_in */
	/* If neither implicit nor explicit missing value is present, treat all values as valid */
	if(idx_in == var_sz_in) has_mss_val=False;
	/* 20210909: End new missing value treatment */
	
	/* Initialize output to missing value (so ocean values are missing not 0.0) */
	(void)memset(var_val_out.vp,0,var_sz_out*nco_typ_lng(var_typ_out));
	switch(var_typ_in){
	case NC_FLOAT: for(idx_out=0;idx_out<var_sz_out;idx_out++) var_val_out.fp[idx_out]=mss_val_unn.f; break;
	case NC_DOUBLE: for(idx_out=0;idx_out<var_sz_out;idx_out++) var_val_out.dp[idx_out]=mss_val_unn.d; break;
	case NC_INT: for(idx_out=0;idx_out<var_sz_out;idx_out++) var_val_out.ip[idx_out]=mss_val_unn.i; break;
	default:
	  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
	  nco_dfl_case_nc_type_err();
	  break;
	} /* !var_typ_in */

	has_clm=has_grd=has_lnd=has_pft=has_tpo=False;
	nco_s1d_typ=nco_s1d_nil;
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	  dmn_id=dmn_ids_in[dmn_idx];
	  /* 20241017: A note of caution
	     This loop overrides the first s1d_typ encountered with the second (if any) encountered
	     History variables, e.g., float PCT_LANDUNIT(time,ltype,topounit), can contain both landunit and topounit dimensions
	     However, in such cases the input landunit dimension uses the gridded name "ltype" not the S1D name "landunit"
	     That should prevent mistakes in confusing s1d_lnd with s1d_tpo variables */
	  if(dmn_id == dmn_id_clm_in) nco_s1d_typ=nco_s1d_clm;
	  else if(dmn_id == dmn_id_grd_in) nco_s1d_typ=nco_s1d_grd;
	  else if(dmn_id == dmn_id_lnd_in) nco_s1d_typ=nco_s1d_lnd;
	  else if(dmn_id == dmn_id_pft_in) nco_s1d_typ=nco_s1d_pft;
	  else if(dmn_id == dmn_id_tpo_in) nco_s1d_typ=nco_s1d_tpo;
	} /* !dmn_idx */

	/* Is this variable empty?
	   If not empty, what is the first valid landunit that this variable is defined on?
	   Ratio of idx_in (an element counter) on first valid landunit by mrv_nbr yields a clm/lnd/pft/tpo index
	   NB: A similar block below determines a valid/invalid flag for which landunits columns are defined on
	   That block should be moved to replace this block if the validity array proves useful for landunit or PFT variables */
	idx_s1d_crr=NC_MIN_INT;
	switch(var_typ_in){
	case NC_FLOAT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	    if(var_val_in.fp[idx_in] != 0.0f && var_val_in.fp[idx_in] != mss_val_cmp_dbl){
	      idx_s1d_crr=idx_in/mrv_nbr;
	      break;
	    } /* !var_val_in.fp */
	  break;
	case NC_DOUBLE: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	    if(var_val_in.dp[idx_in] != 0.0 && var_val_in.dp[idx_in] != mss_val_cmp_dbl){
	      idx_s1d_crr=idx_in/mrv_nbr;
	      break;
	    } /* !var_val_in.dp */
	  break;
	case NC_INT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	    if(var_val_in.ip[idx_in] != 0 && var_val_in.ip[idx_in] != mss_val_cmp_dbl){
	      idx_s1d_crr=idx_in/mrv_nbr;
	      break;
	    } /* !var_val_in.ip */
	  break;
	default:
	  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
	  nco_dfl_case_nc_type_err();
	  break;
	} /* !var_typ_in */
	//(void)fprintf(stdout,"%s: DEBUG quark1\n",nco_prg_nm_get());
	if(idx_in == var_sz_in) flg_var_mpt=True;
	if(idx_s1d_crr != NC_MIN_INT){
	  switch(nco_s1d_typ){
	  case nco_s1d_clm: lnd_typ_vld_1st=cols1d_ityplun[idx_s1d_crr]; break;
	  case nco_s1d_lnd: lnd_typ_vld_1st=land1d_ityplun[idx_s1d_crr]; break;
	  case nco_s1d_pft: lnd_typ_vld_1st=pfts1d_ityplun[idx_s1d_crr]; break;
	  case nco_s1d_grd: lnd_typ_vld_1st=nco_lnd_ilun_nil; break; /* Gridcell variables may contain multiple landunits */
	  case nco_s1d_tpo: lnd_typ_vld_1st=nco_lnd_ilun_nil; break; /* Topounit variables may contain multiple landunits */
	  default: break;
	  } /* !nco_s1d_typ */
	} /* !idx_s1d_crr */

	if(!flg_var_mpt && nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_stdout,"%s: %s, idx_in = %ld, s1d_enm = %d = %s, lnd_typ_vld_1st = %d = %s, has_mec = %d\n",nco_prg_nm_get(),var_nm,idx_in,(int)nco_s1d_typ,nco_s1d_sng(nco_s1d_typ),lnd_typ_vld_1st,nco_lnd_typ_sng(lnd_typ_vld_1st),has_mec);
	  
	if(flg_var_mpt) goto skp_upk;

	/* The Hard Work */
	if(nco_s1d_typ == nco_s1d_pft){
	  /* Turn GPP(time,pft) into GPP(time,pft,lndgrid) or GPP(time,pft,lat,lon)
	     Turn tlai_z(time,pft,levcan) into tlai_z(time,pft,levcan,lndgrid) or tlai_z(time,pft,levcan,lat,lon)
	     Turn fabd_sun(time,pft,numrad) into fabd_sun(time,pft,numrad,lndgrid) or fabd_sun(time,pft,numrad,lat,lon)
	     20240131: This curently works only for single-timestep files
	     In general, must enclose this in outer loop over (time x level) */
	  //val_in_fst=0L;
	  //val_out_fst=0L;
	  for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
	    /* PFT variable dimension ordering, from LRV to MRV:
	       Restart input: PFT, lev*|numrad, e.g., T_REF2M_MIN_INST(pft), fabd_sun(pft,numrad), tlai_z(pft,levcan)
	       History input: time, PFT, lev*|numrad, horizontal
	       NCO Output   : time, PFT, lev*|numrad, horizontal */
	    for(pft_idx=0;pft_idx<pft_nbr_in;pft_idx++){

	      pft_typ=pfts1d_ityp_veg[pft_idx]; /* [0 <= pft_typ <= pft_nbr_out-1] */

	      /* grd_idx is 0-based index relative to the origin of the horizontal grid, pfts1d is 1-based
		 [0 <= grd_idx_out <= col_nbr_out-1L], [1 <= pfts1d_ixy <= col_nbr_out]
		 Storage for Beth's GPP is lon,lat (20241014: I think this was true, but deleted the dataset to test it)
		 Storage for Eva's GPP and h2 history fields lat,lon (20241014: verified)
		 20241014: Difference between Beth's and Eva's GPP makes me worry that history field storage order convention changes
		 Storage order for restart fields (e.g., DZSNO) is lat,lon */
	      //if(flg_nm_hst) grd_idx_out= flg_grd_1D ? pfts1d_ixy[pft_idx]-1L : (pfts1d_ixy[pft_idx]-1L)*lat_nbr+(pfts1d_jxy[pft_idx]-1L);
	      if(flg_nm_hst || flg_nm_rst) grd_idx_out= flg_grd_1D ? pfts1d_ixy[pft_idx]-1L : (pfts1d_jxy[pft_idx]-1L)*lon_nbr+(pfts1d_ixy[pft_idx]-1L);

	      for(mrv_idx=0;mrv_idx<mrv_nbr;mrv_idx++){
		/* Recall that lev*|numrad are MRV in restart input, and are LRV in output where lev*|numrad precedes column,[lat,lon|lndgrid] */
		if(flg_nm_hst) idx_in=pft_idx;
		if(flg_nm_rst) idx_in=pft_idx*mrv_nbr+mrv_idx;
		idx_out=pft_typ*grd_sz_out+grd_idx_out;
		/* memcpy() would allow next statement to work for generic types
		   However, memcpy() is a system call and could be expensive in an innermost loop */
		switch(var_typ_out){
		case NC_FLOAT: var_val_out.fp[idx_out]=var_val_in.fp[idx_in]; break;
		case NC_DOUBLE: var_val_out.dp[idx_out]=var_val_in.dp[idx_in]; break;
		case NC_INT: var_val_out.ip[idx_out]=var_val_in.ip[idx_in]; break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
		  nco_dfl_case_nc_type_err();
		  break;
		} /* !var_typ_out */
	      } /* !mrv_idx */
	    } /* !pft_idx */
	    //val_in_fst+=grd_sz_in; /* fxm */
	    //val_out_fst+=grd_sz_out;
	  } /* !lvl_idx */
	} /* !nco_s1d_pft */
	
	if(nco_s1d_typ == nco_s1d_tpo){
	  /* Turn TS_TOPO(topounit) into TS_TOPO(lndgrid) or TS_TOPO(lat,lon)
	     20240131: This curently works only for single-timestep files
	     In general, must enclose this in outer loop over (time x level) */
	  val_in_fst=0L;
	  val_out_fst=0L;
	  for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
	    /* Topounit variable dimension ordering, from LRV to MRV:
	       Restart input: topounit, e.g., TS_TOPO(topounit)
	       History input: time, ltype, topounit, e.g., PCT_LANDUNIT(time,ltype,topounit), PCT_NAT_PFT(time,natpft,topounit)
	       NCO Output   : time, horizontal */
	    for(tpo_idx=0;tpo_idx<tpo_nbr_in;tpo_idx++){

	      /* grd_idx is 0-based index relative to the origin of the horizontal grid, topo1d is 1-based
		 [0 <= grd_idx_out <= col_nbr_out-1L], [1 <= topo1d_ixy <= col_nbr_out]
		 Storage order for history fields (e.g., PCT_LANDUNIT, PCT_NAT_PFT) is lat,lon 
		 Storage order for restart fields (e.g., TS_TOPO) is lat,lon */
	      // if(flg_nm_hst) grd_idx_out= flg_grd_1D ? topo1d_ixy[tpo_idx]-1L : (topo1d_ixy[tpo_idx]-1L)*lat_nbr+(topo1d_jxy[tpo_idx]-1L);
	      grd_idx_out= flg_grd_1D ? topo1d_ixy[tpo_idx]-1L : (topo1d_jxy[tpo_idx]-1L)*lon_nbr+(topo1d_ixy[tpo_idx]-1L);

	      for(mrv_idx=0;mrv_idx<mrv_nbr;mrv_idx++){
		/* Recall that lev*|numrad are MRV in restart input, and are LRV in output where lev*|numrad precedes column,[lat,lon|lndgrid] */
		if(flg_nm_hst) idx_in=tpo_idx;
		if(flg_nm_rst) idx_in=tpo_idx*mrv_nbr+mrv_idx;
		idx_in=val_in_fst+idx_in;
		idx_out=val_out_fst+grd_idx_out;
		switch(var_typ_out){
		case NC_FLOAT: var_val_out.fp[idx_out]=var_val_in.fp[idx_in]; break;
		case NC_DOUBLE: var_val_out.dp[idx_out]=var_val_in.dp[idx_in]; break;
		case NC_INT: var_val_out.ip[idx_out]=var_val_in.ip[idx_in]; break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
		  nco_dfl_case_nc_type_err();
		  break;
		} /* !var_typ_out */
	      } /* !mrv_idx */
	    } /* !tpo_idx */
	    val_in_fst+=tpo_nbr_in;
	    val_out_fst+=grd_sz_out;
	  } /* !lvl_idx */
	} /* !nco_s1d_tpo */
	
	if(nco_s1d_typ == nco_s1d_clm){
	  /* Turn SNOW_DEPTH(column) into SNOW_DEPTH(mec,lndgrid) or SNOW_DEPTH(mec,lat,lon) 
	     Turn DZSNO(column,levsno) into DZSNO(mec,levsno,lndgrid) or DZSNO(mec,levsno,lat,lon)
	     Support will start for MECs only */

	  /* MEC variable dimension ordering, from LRV to MRV:
	     Restart input: MEC, lev*, e.g., DZSNO(column,levsno), H2OSOI_ICE(column,levtot), T_SOISNO(column,levtot), ZSNO(column,levsno), flx_absdn(column,levsno1), qflx_snofrz_lyr(column,levsno), snw_rds(column,levsno)
	     History input: time, lev*, horizontal, e.g., SNO_BW(time,levsno,lat,lon)
	     NCO Output   : time, MEC, lev*, horizontal
	     ncks --trd -C -d column,0,11 -v DZSNO,cols1d_gridcell_index ${DATA}/bm/elm_mali_rst.nc | m */
	  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_stdout,"%s: INFO unpack block for %s clm_nbr = %ld, mec_nbr = %ld, mrv_nbr = %ld\n",nco_prg_nm_get(),var_nm,clm_nbr_in,(has_mec) ? mec_nbr_out : 0,mrv_nbr);

	  /* Rearrange snow layer data for this variable? */
	  if(snl_var && flg_snw_ocn && (has_levsno || has_levsno1 || has_levtot)) flg_snw_rdr=True;

	  /* Some variables (like DZSNO) defined on lnd_typ=1 (e.g., for non-ice sheet gridcells) and on unrolled dimension (e.g., MEC)
	     If input variable contains valid columns on multiple landunit types, e.g., soil and glacier landunits, then if/where to place column values is user-defined policy implemented by lut_out option
	     lut_out needs to know which landunit types contain valid values for each variable 
	     We store an array of Boolean flags, one per landunit type, in lut_vld_flg(landunit)
	     lut_vld_flg is indexed with 1-based (Fortran) landunit type enums, i.e., lut_vld_flg[1..9] */
	  lut_vld_flg=(nco_bool *)nco_malloc((lut_max+1)*sizeof(nco_bool));
	  for(lnd_idx=0;lnd_idx<=lut_max;lnd_idx++){
	    lut_vld_flg[lnd_idx]=False;
	  } /* !lnd_idx */
	  switch(var_typ_in){
	  case NC_FLOAT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	      if(var_val_in.fp[idx_in] != 0.0f && var_val_in.fp[idx_in] != mss_val_cmp_dbl){
		idx_s1d_crr=idx_in/mrv_nbr;
		lut_vld_flg[cols1d_ityplun[idx_s1d_crr]]=True;
	      } /* !var_val_in.fp */
	    break;
	  case NC_DOUBLE: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	      if(var_val_in.dp[idx_in] != 0.0 && var_val_in.dp[idx_in] != mss_val_cmp_dbl){
		idx_s1d_crr=idx_in/mrv_nbr;
		lut_vld_flg[cols1d_ityplun[idx_s1d_crr]]=True;
	      } /* !var_val_in.dp */
	    break;
	  case NC_INT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	      if(var_val_in.ip[idx_in] != 0 && var_val_in.ip[idx_in] != mss_val_cmp_dbl){
		idx_s1d_crr=idx_in/mrv_nbr;
		lut_vld_flg[cols1d_ityplun[idx_s1d_crr]]=True;
	      } /* !var_val_in.ip */
	    break;
	  default:
	    (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
	    nco_dfl_case_nc_type_err();
	    break;
	  } /* !var_typ_in */
	  
	  for(clm_idx=0;clm_idx<clm_nbr_in;clm_idx++){
	    lnd_typ=cols1d_ityplun[clm_idx]; /* [1 <= lnd_typ <= lnd_nbr_out] */
	    
	    /* Skip columns with LUTs for which this variable is undefined
	       The !has_mec exception is subtle---it ensures MEC fields always pass through
	       Restart files _mostly_ present in landunit order, i.e., LUT=1 columns (i.e., soil landunit) appear before LUT=2, etc.
	       However, we should not (and do not) rely on this ordering
	       In fact, ELM v3 often places lakes and wetlands (LUT=5 and6, respectively) _after_ Urban type (LUT=7--9)
	       Following logic always writes valid MEC landunit gridcells
	       Then it writes any other valid gridcells of the requested non-MEC LUTs
	       We write non-glacier LUTs into the MEC=0 slot for MEC-fields (if present) */
	    
	    clm_wgt=0.0;
	    if(cols1d_wtxy){
	      clm_wgt=cols1d_wtxy[clm_idx];
	      /* It is unwise to skip columns with zero weight (area)
		 One reason is that Antarctic soil columns are all snow-capped even though most have zero weight */
	      //if(clm_wgt <= 0.0) continue;
	    } /* !cols1d */
	    
	    /* grd_idx is 0-based index relative to the origin of the horizontal grid, cols1d is 1-based
	       [0 <= grd_idx_out <= col_nbr_out-1L], [1 <= cols1d_ixy <= col_nbr_out]
	       Storage order for history fields (Beth's GPP, anyway) is lon,lat
	       Storage order for restart fields (e.g., DZSNO) is lat,lon */
	    grd_idx_out= flg_grd_1D ? cols1d_ixy[clm_idx]-1L : (cols1d_jxy[clm_idx]-1L)*lon_nbr+(cols1d_ixy[clm_idx]-1L);
	    
	    /* Process MEC columns
	       Remember that if MECs exist anywhere in the dataset then all column variables must have restart values for MECs 
	       Variables with valid (non-zero, non-missing value) entries in at least one cell have has_mec==True (otherwise False) 
	       Variables with has_mec == True are allocated the MEC dimension
	       Other variables have no allocated memory to receive the zeros or missing values in their MEC cells */
	    if(has_mec && lnd_typ == nco_lnd_ilun_landice_multiple_elevation_classes){
	      /* NB: clm_typ is only needed for, so only access it with, MEC variables
		 cols1d_ityp is not present in some crop history files (e.g., eva_h3.nc) */
	      clm_typ=cols1d_ityp[clm_idx];
	      /* Subtract one because MEC column types are 1-based (401, 402, ... 410) */
	      mec_idx_in=(clm_typ % clm_typ_mec_fst)-1;
	      /* 20241101: Reserve index[0] of MEC dimension for other LUT column data of same variable */
	      mec_idx_out=mec_idx_in+1;
	      for(mrv_idx=0;mrv_idx<mrv_nbr;mrv_idx++){
		/* Recall that lev*|numrad are MRV in restart input, and are LRV in output where lev*|numrad precedes column,[lat,lon|lndgrid] */
		idx_in=clm_idx*mrv_nbr+mrv_idx;
		idx_out=mec_idx_out*mrv_nbr*grd_sz_out+mrv_idx*grd_sz_out+grd_idx_out;
		/* Alter output index for snow layers if desired/possible */
		if(flg_snw_rdr){
		  /* Snow layer dimension has intricate indexing/storage scheme
		     levsno increases downward from top layer (like pressure)
		     The number of snow layers varies according to snow depth
		     The surface snow layer (adjacent to ground) is always index == 0. See clm5.pdf, p. 93, Tbl 8.2.
		     (Since the "starting" index is zero, this is equivalent to a C-based index
		     Thus we do not confuse matters by qualifying input index by Fortran or C in the following
		     They are equivalent for input snow indexes because of the zero-based convention).
		     Additional layers are added as snowpack deepens 
		     Snow layers are stored adjacent to the surface layer so that hydrology can route heat and melt from the snow into the ground/soil layers as vertically adjacent gridcells (this simplifies solution to, e.g., Crank-Nicholson solvers)
		     The notation snl+1 describes the top layer of snow for general snow pack, where snl is the _negative_ of the number of snow layers
		     The maximum number of snow layers (aka, levsno_nbr, the levsno dimension size) can be 16 (with E3SM use_extra_snowlayers), 12 (with CESM2), or 5 (CLM4, E3SMv1-3 default)
		     Therefore where all layers are utilized, the top snow layer (adjacent to atmosphere) is index == -15 for has_deep_snowpack, -11 for CESM2, -4 for default E3SM snowpack
		     A gridcell with only three layers, say, has top snow layer index -2 and surface index 0
		     Unless snl_var is present, S1D will unpack snow variables exactly as stored, so that the top level of a 3-layer snowpack will appear in index -2, aka C-index 13 for the output levsno dimension for E3SM with use_extra_snowlayers
		     This results in a "negative space" above the snow fields in the output file
		     A more intuitive order to view gridded snow is to place the top layer at index 0 of the levsno dimension in the output file, with deeper layers arranged subsequently
		     
		     This shifts the empty cells for non-utilized snowpack layers from the top to the bottom of the output (analogous to bottom topography masking the lower layers of z-grid ocean datasets)
		     This block uses snl_var when present to effect that shift, so that output snow is butted against the beginning rather than the end of the levsno dimension 
		     Expressed mathematically, we want snl+1, which occupies input levsno index levsno_idx_in == [levsno_nbr_in-(-snl)] == [levsno_nbr_in+snl], to have levsno C-index 0 in the output:
		     levsno_idx_out == levsno_nbr_in+snl == -> */
		  levsno_idx_in=mrv_idx;
		  /* Only rearrange snow layers, leave empty snow layers and all soil layers alone
		     First condition is False in soil layers of levtot variables
		     Second condition is False in empty snow layers of all variables */
		  if(levsno_idx_in < levsno_nbr_in && (levsno_idx_in-snl_var[clm_idx] >= levsno_nbr_in)){
		    levsno_idx_out=levsno_idx_in-(levsno_nbr_in+snl_var[clm_idx]);
		    assert(levsno_idx_out >= 0);
		    assert(levsno_idx_out < mrv_nbr);
		    idx_out=mec_idx_out*mrv_nbr*grd_sz_out+levsno_idx_out*grd_sz_out+grd_idx_out;
		  } /* !levsno_idx_in */
		} /* !flg_snw_rdr */
		switch(var_typ_out){
		case NC_FLOAT: var_val_out.fp[idx_out]=var_val_in.fp[idx_in]; break;
		case NC_DOUBLE: var_val_out.dp[idx_out]=var_val_in.dp[idx_in]; break;
		case NC_INT: var_val_out.ip[idx_out]=var_val_in.ip[idx_in]; break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
		  nco_dfl_case_nc_type_err();
		  break;
		} /* !var_typ_out */
		if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(fp_stdout,"%s: INFO %s clm_idx = %ld, mec_idx_in = %ld, mrv_idx = %ld\n",fnc_nm,var_nm,clm_idx,mec_idx_in,mrv_idx);
	      } /* !mrv_idx */
	    }else{ /* !has_mec || !ilun_landice_multiple_elevation_classes */
	      /* In this block we know that column weight > 0, and lnd_typ != ilun_landice_multiple_elevation_classes
		 The variable may have MECs, though not in this particular column */
	      /* Skip column types not requested to be output */
	      switch(lut_out){
	      case nco_lut_out_wgt_all:
		break;
	      case nco_lut_out_wgt_soi_glc:
		if(lnd_typ != nco_lnd_ilun_vegetated_or_bare_soil && (lnd_typ != nco_lnd_ilun_landice)) continue;
		break;
	      default:
		if(lnd_typ != lut_out) continue;
		break;
	      } /* !lutout */
	      /* Skip column types that this variable lack valid values for */
	      if(!lut_vld_flg[lnd_typ]) continue;
	      switch(lnd_typ){
	      case nco_lnd_ilun_landice_multiple_elevation_classes:
		/* Default v3 variables with no valid values on MECs (and thus has_mec==False) include: 
		   FH2OSFC, H2OSFC, LAKE_ICEFRAC, MFLX_SNOW_LYR, SAVEDTKE1, T_GRND_R, T_GRND_U, T_LAKE, URBAN_AC, URBAN_HEAT, USTLAKE 
		   MEC columns are invalid for these variables so do not write anything */
		//(void)fprintf(fp_stdout,"%s: INFO %s reports variable %s has column weight = %f and has_mec = %d on MEC landunit type \n",nco_prg_nm_get(),fnc_nm,var_nm,cols1d_wtxy ? clm_wgt : -737,has_mec);
		break; /* !ilun_landice_multiple_elevation_classes */
	      case nco_lnd_ilun_vegetated_or_bare_soil:
	      case nco_lnd_ilun_crop:
	      case nco_lnd_ilun_landice:
	      case nco_lnd_ilun_deep_lake:
	      case nco_lnd_ilun_wetland:
	      case nco_lnd_ilun_urban_tbd:
	      case nco_lnd_ilun_urban_hd:
	      case nco_lnd_ilun_urban_md:
		for(mrv_idx=0;mrv_idx<mrv_nbr;mrv_idx++){
		  /* Other MRV dimensions (e.g., lake levels) are true second dimensions, not unrolled into cols1d array like MECs
		     Restart arrays are stored as, e.g., [cols1d,levlak] */
		  idx_in=clm_idx*mrv_nbr+mrv_idx;
		  idx_out=mrv_idx*grd_sz_out+grd_idx_out;
		  /* Alter output index for snow layers if desired/possible. See explanation above. */
		  if(flg_snw_rdr){
		    levsno_idx_in=mrv_idx;
		    if(levsno_idx_in < levsno_nbr_in && (levsno_idx_in-snl_var[clm_idx] >= levsno_nbr_in)){
		      levsno_idx_out=levsno_idx_in-(levsno_nbr_in+snl_var[clm_idx]);
		      idx_out=levsno_idx_out*grd_sz_out+grd_idx_out;
		    } /* !levsno_idx_in */
		  } /* !flg_snw_rdr */
		  switch(var_typ_out){
		    /* MEC values are handled separately in above block
		       MEC and non-MEC variables have two options for what to store in all non-MEC columns
		       MEC variables reserve one index (MEC index = 0) of extra space per gridcell for other columns
		       Non-MEC columns use this address (MEC index = 0) to store one value per gridcell
		       Storing all non-MEC column types for each gridcell would require a column dimension for every variable
		       That is too anal and storage-intensive so instead we store only one non-MEC column type per gridcell
		       What column-type or combination of column types  (soil, lake, wetland, etc.) to store there is debatable
		       For example, I am usually interested in glacier and soil columns and/or their area-weighted average
		       To accomodate this, S1D provides the lut_out option to specify which landunit to store in each gridcell:
		       ncks --rgr lut_out=[1..9] specifies a single landunit type to output
		       ncks --rgr lut_out=0      means output an area-weighted average of all landunit types
		       Users can consecutively run S1D with different lut_out options to gain a more comprehensive view */
		  case NC_FLOAT:
		    if(lut_out == nco_lut_out_wgt_all || lut_out == nco_lut_out_wgt_soi_glc){
		      if(var_val_out.fp[idx_out] == mss_val_unn.f) var_val_out.fp[idx_out]=var_val_in.fp[idx_in]*clm_wgt; else var_val_out.fp[idx_out]+=var_val_in.fp[idx_in]*clm_wgt;
		    }else{
		      var_val_out.fp[idx_out]=var_val_in.fp[idx_in];
		    } /* !cols1d_wtxy */
		    break;
		  case NC_DOUBLE: 
		    if(lut_out == nco_lut_out_wgt_all || lut_out == nco_lut_out_wgt_soi_glc){
		      if(var_val_out.dp[idx_out] == mss_val_unn.d) var_val_out.dp[idx_out]=var_val_in.dp[idx_in]*clm_wgt; else var_val_out.dp[idx_out]+=var_val_in.dp[idx_in]*clm_wgt;
		    }else{
		      var_val_out.dp[idx_out]=var_val_in.dp[idx_in];
		    } /* !cols1d_wtxy */
		    break;
		  case NC_INT: 
		    if(lut_out == nco_lut_out_wgt_all || lut_out == nco_lut_out_wgt_soi_glc){
		      if(var_val_out.ip[idx_out] == mss_val_unn.i) var_val_out.ip[idx_out]=var_val_in.ip[idx_in]*clm_wgt; else var_val_out.ip[idx_out]+=var_val_in.ip[idx_in]*clm_wgt;
		    }else{
		      var_val_out.ip[idx_out]=var_val_in.ip[idx_in];
		    } /* !cols1d_wtxy */
		    break;
		  default:
		    (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported numeric type\n",nco_prg_nm_get(),fnc_nm);
		    nco_dfl_case_nc_type_err();
		    break;
		  } /* !var_typ_out */
		} /* !mrv_idx */
		break; /* !ilun_deep_lake */
	      default:
		(void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported landunit type\n",nco_prg_nm_get(),fnc_nm);
		nco_dfl_case_generic_err((int)lnd_typ);
		break; /* !default */
	      } /* !lnd_typ */
	    } /* !(has_mec && ilun_landice_multiple_elevation_classes) */
	  } /* !clm_idx */
	} /* !nco_s1d_typ */
	  
	/* goto skp_upk */
      skp_upk: 

#pragma omp critical
	{ /* begin OpenMP critical */
	  rcd=nco_put_vara(out_id,var_id_out,dmn_srt,dmn_cnt_out,var_val_out.vp,var_typ_out);
	} /* end OpenMP critical */

	/* Free private memory allocated on a per-variable basis */
	if(dmn_ids_in) dmn_ids_in=(int *)nco_free(dmn_ids_in);
	if(dmn_ids_out) dmn_ids_out=(int *)nco_free(dmn_ids_out);
	if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
	if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
	if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
	if(lut_vld_flg) lut_vld_flg=(nco_bool *)nco_free(lut_vld_flg);
	if(var_val_in.vp) var_val_in.vp=(void *)nco_free(var_val_in.vp);
	if(var_val_out.vp) var_val_out.vp=(void *)nco_free(var_val_out.vp);
      }else{ /* !trv.flg_rgr */
	/* Use standard NCO copy routine for variables that are not regridded
	   20190511: Copy them only once */
#pragma omp critical
	{ /* begin OpenMP critical */
	  (void)nco_cpy_var_val(in_id,out_id,(FILE *)NULL,(md5_sct *)NULL,trv.nm,trv_tbl);
	} /* end OpenMP critical */
      } /* !flg_rgr */
    } /* !xtr */
  } /* end (OpenMP parallel for) loop over idx_tbl */
  if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"\n");
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s completion report: Variables unpacked = %d, copied unmodified = %d, omitted = %d, created = %d\n",nco_prg_nm_get(),fnc_nm,var_rgr_nbr,var_cpy_nbr,var_xcl_nbr,var_crt_nbr);

  /* Free output data and shared memory */
  if(cols1d_active) cols1d_active=(int *)nco_free(cols1d_active);
  if(cols1d_gridcell_index) cols1d_gridcell_index=(int *)nco_free(cols1d_gridcell_index);
  if(cols1d_ityp) cols1d_ityp=(int *)nco_free(cols1d_ityp);
  if(cols1d_ityplun) cols1d_ityplun=(int *)nco_free(cols1d_ityplun);
  if(cols1d_ixy) cols1d_ixy=(int *)nco_free(cols1d_ixy);
  if(cols1d_jxy) cols1d_jxy=(int *)nco_free(cols1d_jxy);
  if(cols1d_wtxy) cols1d_wtxy=(double *)nco_free(cols1d_wtxy);
  if(land1d_active) land1d_active=(int *)nco_free(land1d_active);
  if(land1d_gridcell_index) land1d_gridcell_index=(int *)nco_free(land1d_gridcell_index);
  if(land1d_ityplun) land1d_ityplun=(int *)nco_free(land1d_ityplun);
  if(land1d_ixy) land1d_ixy=(int *)nco_free(land1d_ixy);
  if(land1d_jxy) land1d_jxy=(int *)nco_free(land1d_jxy);
  if(land1d_wtxy) land1d_wtxy=(double *)nco_free(land1d_wtxy);
  if(pfts1d_active) pfts1d_active=(int *)nco_free(pfts1d_active);
  if(pfts1d_ityp_veg) pfts1d_ityp_veg=(int *)nco_free(pfts1d_ityp_veg);
  if(pfts1d_ityplun) pfts1d_ityplun=(int *)nco_free(pfts1d_ityplun);
  if(pfts1d_ixy) pfts1d_ixy=(int *)nco_free(pfts1d_ixy);
  if(pfts1d_jxy) pfts1d_jxy=(int *)nco_free(pfts1d_jxy);
  //if(pfts1d_wtxy) pfts1d_wtxy=(double *)nco_free(pfts1d_wtxy);
  if(snl_var) snl_var=(int *)nco_free(snl_var);
  if(topo1d_ixy) topo1d_ixy=(int *)nco_free(topo1d_ixy);
  if(topo1d_jxy) topo1d_jxy=(int *)nco_free(topo1d_jxy);

  if(clm_nm_in) clm_nm_in=(char *)nco_free(clm_nm_in);
  if(grd_nm_in) grd_nm_in=(char *)nco_free(grd_nm_in);
  if(lnd_nm_in) lnd_nm_in=(char *)nco_free(lnd_nm_in);
  if(pft_nm_in) pft_nm_in=(char *)nco_free(pft_nm_in);
  if(tpo_nm_in) tpo_nm_in=(char *)nco_free(tpo_nm_in);
  if(clm_nm_out) clm_nm_out=(char *)nco_free(clm_nm_out);
  if(grd_nm_out) grd_nm_out=(char *)nco_free(grd_nm_out);
  if(lnd_nm_out) lnd_nm_out=(char *)nco_free(lnd_nm_out);
  if(pft_nm_out) pft_nm_out=(char *)nco_free(pft_nm_out);
  if(pft_chr_out) pft_chr_out=(char *)nco_free(pft_chr_out);
  if(pft_sng_out) pft_sng_out=(nco_string *)nco_free(pft_sng_out);
  if(pft_sng_lng_nm_out) pft_sng_lng_nm_out=(char *)nco_free(pft_sng_lng_nm_out);
  if(tpo_nm_out) tpo_nm_out=(char *)nco_free(tpo_nm_out);

  if(levcan_nm_in) levcan_nm_in=(char *)nco_free(levcan_nm_in);
  if(levgrnd_nm_in) levgrnd_nm_in=(char *)nco_free(levgrnd_nm_in);
  if(levlak_nm_in) levlak_nm_in=(char *)nco_free(levlak_nm_in);
  if(levsno_nm_in) levsno_nm_in=(char *)nco_free(levsno_nm_in);
  if(levsno1_nm_in) levsno1_nm_in=(char *)nco_free(levsno1_nm_in);
  if(levtot_nm_in) levtot_nm_in=(char *)nco_free(levtot_nm_in);
  if(numrad_nm_in) numrad_nm_in=(char *)nco_free(numrad_nm_in);

  if(levcan_nm_out) levcan_nm_out=(char *)nco_free(levcan_nm_out);
  if(levgrnd_nm_out) levgrnd_nm_out=(char *)nco_free(levgrnd_nm_out);
  if(levlak_nm_out) levlak_nm_out=(char *)nco_free(levlak_nm_out);
  if(levsno_nm_out) levsno_nm_out=(char *)nco_free(levsno_nm_out);
  if(levsno1_nm_out) levsno1_nm_out=(char *)nco_free(levsno1_nm_out);
  if(levtot_nm_out) levtot_nm_out=(char *)nco_free(levtot_nm_out);
  if(numrad_nm_out) numrad_nm_out=(char *)nco_free(numrad_nm_out);

  return rcd;
} /* !nco_s1d_unpack() */
