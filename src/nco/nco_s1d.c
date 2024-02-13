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
  case nco_clm_icol_nil: return "Unknown, unset, or unclassified landunit type";
  case nco_clm_icol_vegetated_or_bare_soil: return "Vegetated or bare soil";
  case nco_clm_icol_crop: return "Crop icol_crop_noncompete: 2*100+m, m=cft_lb,cft_ub";
  case nco_clm_icol_crop_noncompete_01: return "Crop noncompete type 01";
  case nco_clm_icol_crop_noncompete_02: return "Crop noncompete type 02";
  case nco_clm_icol_landice: return "Landice (plain, no MEC)";
  case nco_clm_icol_landice_multiple_elevation_class_01: return "Landice multiple elevation class 01";
  case nco_clm_icol_landice_multiple_elevation_class_02: return "Landice multiple elevation class 02";
  case nco_clm_icol_landice_multiple_elevation_class_03: return "Landice multiple elevation class 03";
  case nco_clm_icol_landice_multiple_elevation_class_04: return "Landice multiple elevation class 04";
  case nco_clm_icol_landice_multiple_elevation_class_05: return "Landice multiple elevation class 05";
  case nco_clm_icol_landice_multiple_elevation_class_06: return "Landice multiple elevation class 06";
  case nco_clm_icol_landice_multiple_elevation_class_07: return "Landice multiple elevation class 07";
  case nco_clm_icol_landice_multiple_elevation_class_08: return "Landice multiple elevation class 08";
  case nco_clm_icol_landice_multiple_elevation_class_09: return "Landice multiple elevation class 09";
  case nco_clm_icol_landice_multiple_elevation_class_10: return "Landice multiple elevation class 10";
  case nco_clm_icol_deep_lake: return "Deep lake";
  case nco_clm_icol_wetland: return "Wetland";
  case nco_clm_icol_urban_roof: return "Urban roof";
  case nco_clm_icol_urban_sunwall: return "Urban sunwall";
  case nco_clm_icol_urban_shadewall: return "Urban shadewall";
  case nco_clm_icol_urban_impervious_road: return "Urban impervious road";
  case nco_clm_icol_urban_pervious_road: return "Urban pervious road";
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

const char * /* O [sng] String describing PFT type */
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
  case nco_pft_ipft_c3_crop: return "C3 crop"; /* 15 */
  case nco_pft_ipft_c3_irrigated: return "C3 irrigated"; /* 16 */
  case nco_pft_ipft_corn: return "Corn"; /* 17 */
  case nco_pft_ipft_irrigated_corn: return "Irrigated corn"; /* 18 */
  case nco_pft_ipft_spring_temperate_cereal: return "Spring temperate cereal"; /* 19 */
  case nco_pft_ipft_irrigated_spring_temperate_cereal: return "Irrigated spring temperate cereal"; /* 20 */
  case nco_pft_ipft_winter_temperate_cereal: return "Winter temperate cereal"; /* 21 */
  case nco_pft_ipft_irrigated_winter_temperate_cereal: return "Irrigated winter temperate cereal"; /* 22 */
  case nco_pft_ipft_soybean: return "Soybean"; /* 23 */
  case nco_pft_ipft_irrigated_soybean: return "Irrigated soybean"; /* 24 */
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
  default: nco_dfl_case_generic_err((int)nco_s1d_typ); break;
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
     ncks -D 1 -O -C --s1d ~/data/bm/elm_mali_bg_hst.nc ~/foo.nc
     ncks -D 1 -O -C --s1d -v cols1d_topoglc --hrz=${DATA}/bm/elm_mali_ig_hst.nc ${DATA}/bm/elm_mali_rst.nc ~/foo.nc
     ncks -D 1 -O -C --s1d -v GPP,pfts1d_wtgcell ~/beth_in.nc ~/foo.nc
     ncremap --dbg=1 --vrb=3 --devnull=No --nco='--dbg=1' -P elm -m ${DATA}/maps/map_ne30np4_to_fv128x256_aave.20160301.nc ~/foo.nc ~/foo_rgr.nc */

  /* 20240131: Unpacking an r05 ELM restart file into latxlon format is storage-intensive
     Converting an r05 gis_1to10km IG simulation on imua took ~2 hrs and ballooned the filesize from ~7 GB to ~7 TB!
     Always subset before converting the input restart file
     ncks -D 1 -O -C --s1d --hrz=${DATA}/bm/elm_mali_ig_hst.nc ${DATA}/bm/elm_mali_rst.nc ~/foo.nc */

  const char fnc_nm[]="nco_s1d_unpack()"; /* [sng] Function name */

  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped as shared in parallel clause */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

  char *fl_in;
  //char *fl_out;
  char *fl_tpl; /* [sng] Template file (contains horizontal grid) */

  char dmn_nm[NC_MAX_NAME]; /* [sng] Dimension name */
  char *grd_nm_in=(char *)strdup("gridcell");
  char *lnd_nm_in=(char *)strdup("landunit");
  char *clm_nm_in=(char *)strdup("column");
  char *pft_nm_in=(char *)strdup("pft");

  char *levcan_nm_in=(char *)strdup("levcan");
  char *levgrnd_nm_in=(char *)strdup("levgrnd");
  char *levlak_nm_in=(char *)strdup("levlak");
  char *levsno_nm_in=(char *)strdup("levsno");
  char *levsno1_nm_in=(char *)strdup("levsno1");
  char *levtot_nm_in=(char *)strdup("levtot");
  char *numrad_nm_in=(char *)strdup("numrad");
    
  char *mec_nm_out=(char *)strdup("mec");

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
  long int grd_idx_out;
  long int idx_s1d_crr; /* [idx] Current valid index into S1D arrays */
  long int idx_s1d_nxt; /* [idx] Next valid index into S1D arrays */
  //long int lat_idx;
  //long int lon_idx;
  long int lnd_idx;
  long int mec_idx;
  long int pft_idx;

  nco_bool flg_var_mpt; /* [flg] Variable has no valid values */

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
     This routine distinguishes these meanings by abbreviating (1) as "col" and (2) as "clm" 
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

  int grid1d_ixy_id=NC_MIN_INT; /* [id] Gridcell 2D longitude index (1-based) */
  int grid1d_jxy_id=NC_MIN_INT; /* [id] Gridcell 2D latitude index (1-based) */
  int grid1d_lat_id=NC_MIN_INT; /* [id] Gridcell latitude */
  int grid1d_lon_id=NC_MIN_INT; /* [id] Gridcell longitude */

  int land1d_active_id=NC_MIN_INT; /* [id] Landunit active flag (1=active, 0=inactive) */
  int land1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of landunit (1-based) */
  int land1d_ixy_id=NC_MIN_INT; /* [id] Landunit 2D longitude index (1-based) */
  int land1d_jxy_id=NC_MIN_INT; /* [id] Landunit 2D latitude index (1-based) */
  int land1d_lat_id=NC_MIN_INT; /* [id] Landunit latitude */
  int land1d_lon_id=NC_MIN_INT; /* [id] Landunit longitude */
  int land1d_ityplun_id=NC_MIN_INT; /* [id] Landunit type */

  int pfts1d_active_id=NC_MIN_INT; /* [id] PFT active flag (1=active, 0=inactive) */
  int pfts1d_column_index_id=NC_MIN_INT; /* [id] Column index of PFT (1-based) */
  int pfts1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of PFT (1-based) */
  int pfts1d_ityp_veg_id=NC_MIN_INT; /* [id] PFT vegetation type */
  int pfts1d_ityplun_id=NC_MIN_INT; /* [id] PFT landunit type */
  int pfts1d_ixy_id=NC_MIN_INT; /* [id] PFT 2D longitude index (1-based) */
  int pfts1d_jxy_id=NC_MIN_INT; /* [id] PFT 2D latitude index (1-based) */
  int pfts1d_lat_id=NC_MIN_INT; /* [id] PFT latitude */
  int pfts1d_lon_id=NC_MIN_INT; /* [id] PFT longitude */
  //int pfts1d_wtgcell_id=NC_MIN_INT; /* [id] PFT weight relative to corresponding gridcell */
  
  int dmn_id_clm_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_grd_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lnd_in=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_pft_in=NC_MIN_INT; /* [id] Dimension ID */

  nco_bool flg_s1d_clm=False; /* [flg] Dataset contains sparse variables for columns */
  nco_bool flg_s1d_grd=False; /* [flg] Dataset contains sparse variables for gridcells */
  nco_bool flg_s1d_lnd=False; /* [flg] Dataset contains sparse variables for landunits */
  nco_bool flg_s1d_pft=False; /* [flg] Dataset contains sparse variables for PFTs */

  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,"ilun_vegetated_or_bare_soil",(nc_type *)NULL,(long *)NULL);
  if(rcd == NC_NOERR) flg_nm_rst=True;
  rcd=nco_inq_att_flg(in_id,NC_GLOBAL,"ltype_vegetated_or_bare_soil",(nc_type *)NULL,(long *)NULL);
  if(rcd == NC_NOERR) flg_nm_hst=True;
  assert(!(flg_nm_hst && flg_nm_rst));
  if(!flg_nm_hst && !flg_nm_rst){
    (void)fprintf(stderr,"%s: ERROR %s reports input data file lacks expected global attributes\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !flg_nm_hst */
  if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO %s will assume input attributes and variables use CLM/ELM %s naming conventions like %s\n",nco_prg_nm_get(),fnc_nm,flg_nm_hst ? "history file" : "restart file",flg_nm_hst ? "\"ltype_...\"" : "\"ilun_...\"");

  rcd=nco_inq_varid_flg(in_id,"cols1d_lat",&cols1d_lat_id);
  if(cols1d_lat_id != NC_MIN_INT) flg_s1d_clm=True;
  if(flg_s1d_clm){
    rcd=nco_inq_varid(in_id,"cols1d_ixy",&cols1d_ixy_id);
    rcd=nco_inq_varid(in_id,"cols1d_jxy",&cols1d_jxy_id);
    rcd=nco_inq_varid(in_id,"cols1d_lon",&cols1d_lon_id);
    rcd=nco_inq_varid_flg(in_id,"cols1d_active",&cols1d_active_id); /* ELM/MALI restart */
    rcd=nco_inq_varid_flg(in_id,"cols1d_gridcell_index",&cols1d_gridcell_index_id); /* ELM/MALI restart */
    rcd=nco_inq_varid_flg(in_id,"cols1d_ityp",&cols1d_ityp_id); /* ELM/MALI restart */
    if(flg_nm_hst) rcd=nco_inq_varid(in_id,"cols1d_itype_lunit",&cols1d_ityplun_id); else rcd=nco_inq_varid(in_id,"cols1d_ityplun",&cols1d_ityplun_id);
  } /* !flg_s1d_clm */
     
  rcd=nco_inq_varid_flg(in_id,"grid1d_lat",&grid1d_lat_id);
  if(grid1d_lat_id != NC_MIN_INT) flg_s1d_grd=True;
  if(flg_s1d_grd){
    rcd=nco_inq_varid(in_id,"grid1d_ixy",&grid1d_ixy_id);
    rcd=nco_inq_varid(in_id,"grid1d_jxy",&grid1d_jxy_id);
    rcd=nco_inq_varid(in_id,"grid1d_lon",&grid1d_lon_id);
  } /* !flg_s1d_grd */
     
  rcd=nco_inq_varid_flg(in_id,"land1d_lat",&land1d_lat_id);
  if(land1d_lat_id != NC_MIN_INT) flg_s1d_lnd=True;
  if(flg_s1d_lnd){
    rcd=nco_inq_varid(in_id,"land1d_ixy",&land1d_ixy_id);
    rcd=nco_inq_varid(in_id,"land1d_jxy",&land1d_jxy_id);
    rcd=nco_inq_varid(in_id,"land1d_lon",&land1d_lon_id);
    rcd=nco_inq_varid_flg(in_id,"land1d_active",&land1d_active_id);
    rcd=nco_inq_varid_flg(in_id,"land1d_gridcell_index",&land1d_gridcell_index_id);
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
    //if(flg_nm_hst) rcd=nco_inq_varid(in_id,"pfts1d_wtgcell",&pfts1d_wtgcell_id); else rcd=nco_inq_varid(in_id,"pfts1d_wtxy",&pfts1d_wtgcell_id);
    if(flg_nm_hst) rcd=nco_inq_varid(in_id,"pfts1d_itype_lunit",&pfts1d_ityplun_id); else rcd=nco_inq_varid(in_id,"pfts1d_ityplun",&pfts1d_ityplun_id);
    if(flg_nm_hst) rcd=nco_inq_varid(in_id,"pfts1d_itype_veg",&pfts1d_ityp_veg_id); else rcd=nco_inq_varid(in_id,"pfts1d_itypveg",&pfts1d_ityp_veg_id);
  } /* !flg_s1d_pft */
  
  if(!(flg_s1d_clm || flg_s1d_lnd || flg_s1d_pft)){
    (void)fprintf(stderr,"%s: ERROR %s does not detect any of the key variables (currently cols1d_lat, land1d_lat, pfts1d_lat) used to indicate presence of sparse-packed (S1D) variables\nHINT: Be sure the target dataset (file) contains S1D variables---not all CLM/ELM history (as opposed to restart) files do\n",nco_prg_nm_get(),fnc_nm);
    nco_exit(EXIT_FAILURE);
  } /* !flg_s1d_clm... */

  if(flg_s1d_clm) rcd=nco_inq_dimid(in_id,clm_nm_in,&dmn_id_clm_in);
  if(flg_s1d_grd) rcd=nco_inq_dimid(in_id,grd_nm_in,&dmn_id_grd_in);
  if(flg_s1d_lnd) rcd=nco_inq_dimid(in_id,lnd_nm_in,&dmn_id_lnd_in);
  if(flg_s1d_pft) rcd=nco_inq_dimid(in_id,pft_nm_in,&dmn_id_pft_in);

  if(nco_dbg_lvl_get() >= nco_dbg_std){
    (void)fprintf(stderr,"%s: INFO %s necessary information to unpack cols1d variables\n",nco_prg_nm_get(),flg_s1d_clm ? "Found all" : "Could not find");
    (void)fprintf(stderr,"%s: INFO %s necessary information to unpack land1d variables\n",nco_prg_nm_get(),flg_s1d_lnd ? "Found all" : "Could not find");
    (void)fprintf(stderr,"%s: INFO %s necessary information to unpack pfts1d variables\n",nco_prg_nm_get(),flg_s1d_pft ? "Found all" : "Could not find");
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
  //long idx; /* [idx] Generic index */
  unsigned int idx_tbl; /* [idx] Counter for traversal table */

  char *dmn_nm_cp; /* [sng] Dimension name as char * to reduce indirection */
  nco_bool has_clm; /* [flg] Variable contains column dimension */
  nco_bool has_grd; /* [flg] Variable contains gridcell dimension */
  nco_bool has_lnd; /* [flg] Variable contains landunit dimension */
  nco_bool has_mec; /* [flg] Variable contains (implicit, unrolled) MEC dimension */
  nco_bool has_pft; /* [flg] Variable contains PFT dimension */
  nco_bool need_clm=False; /* [flg] At least one variable to unpack needs column dimension */
  nco_bool need_grd=False; /* [flg] At least one variable to unpack needs gridcell dimension */
  nco_bool need_lnd=False; /* [flg] At least one variable to unpack needs landunit dimension */
  nco_bool need_mec=False; /* [flg] At least one variable to unpack needs MEC dimension */
  nco_bool need_pft=False; /* [flg] At least one variable to unpack needs PFT dimension */ 

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
      has_clm=has_grd=has_lnd=has_pft=False;
      has_levcan=has_levgrnd=has_levlak=has_levsno=has_levsno1=has_levtot=has_numrad=False;
      for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	/* Pre-determine flags necessary during next loop */
	dmn_nm_cp=trv.var_dmn[dmn_idx].dmn_nm;
	if(!has_clm && clm_nm_in) has_clm=!strcmp(dmn_nm_cp,clm_nm_in);
	if(!has_grd && grd_nm_in) has_grd=!strcmp(dmn_nm_cp,grd_nm_in);
	if(!has_lnd && lnd_nm_in) has_lnd=!strcmp(dmn_nm_cp,lnd_nm_in);
	if(!has_pft && pft_nm_in) has_pft=!strcmp(dmn_nm_cp,pft_nm_in);

	if(!has_levcan && levcan_nm_in) has_levcan=!strcmp(dmn_nm_cp,levcan_nm_in);
	if(!has_levgrnd && levgrnd_nm_in) has_levgrnd=!strcmp(dmn_nm_cp,levgrnd_nm_in);
	if(!has_levlak && levlak_nm_in) has_levlak=!strcmp(dmn_nm_cp,levlak_nm_in);
	if(!has_levsno && levsno_nm_in) has_levsno=!strcmp(dmn_nm_cp,levsno_nm_in);
	if(!has_levsno1 && levsno1_nm_in) has_levsno1=!strcmp(dmn_nm_cp,levsno1_nm_in);
	if(!has_levtot && levtot_nm_in) has_levtot=!strcmp(dmn_nm_cp,levtot_nm_in);
	if(!has_numrad && numrad_nm_in) has_numrad=!strcmp(dmn_nm_cp,numrad_nm_in);
      } /* !dmn_idx */
      /* Unpack variables that contain a sparse-1D dimension */
      if(has_clm || has_grd || has_lnd || has_pft){
	trv_tbl->lst[idx_tbl].flg_rgr=True;
	var_rgr_nbr++;
	if(has_clm) need_clm=True;
	if(has_grd) need_grd=True;
	if(has_lnd) need_lnd=True;
	if(has_pft) need_pft=True;

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
  if(!var_rgr_nbr) (void)fprintf(stdout,"%s: WARNING %s reports no variables fit unpacking criteria. The sparse data unpacker expects at least one variable to unpack, and copies other variables straight to output. HINT: If the name(s) of the input sparse-1D dimensions (e.g., \"column\", \"landunit\", and \"pft\") do not match NCO's preset defaults (case-insensitive unambiguous forms and abbreviations of \"column\", \"landunit\", and/or \"pft\", respectively) then change the dimension names that NCO looks for. Instructions are at http://nco.sf.net/nco.html#sparse. For CTSM/ELM sparse-1D coordinate grids, the \"column\", \"landunit\", and \"pft\" variable names can be set with, e.g., \"ncks --rgr column_nm=clm#landunit_nm=lnd#pft_nm=pft\" or \"ncremap -R '--rgr clm=clm#lnd=lnd#pft=pft'\".\n",nco_prg_nm_get(),fnc_nm);
  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
      trv=trv_tbl->lst[idx_tbl];
      if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr) (void)fprintf(stderr,"Unpack %s? %s\n",trv.nm,trv.flg_rgr ? "Yes" : "No");
    } /* !idx_tbl */
  } /* !dbg */

  long clm_nbr_in=NC_MIN_INT; /* [nbr] Number of columns in input data */
  long grd_nbr_in=NC_MIN_INT; /* [nbr] Number of gridcells in input data */
  long lnd_nbr_in=NC_MIN_INT; /* [nbr] Number of landunits in input data */
  long pft_nbr_in=NC_MIN_INT; /* [nbr] Number of PFTs in input data */
  long clm_nbr_out=NC_MIN_INT; /* [nbr] Number of columns in output data */
  long grd_nbr_out=NC_MIN_INT; /* [nbr] Number of gridcells in output data */
  long lnd_nbr_out=NC_MIN_INT; /* [nbr] Number of landunits in output data */
  long mec_nbr_out=NC_MIN_INT; /* [nbr] Number of MECs in output data */
  long pft_nbr_out=NC_MIN_INT; /* [nbr] Number of PFTs in output data */
  if(need_clm) rcd=nco_inq_dimlen(in_id,dmn_id_clm_in,&clm_nbr_in);
  if(need_grd) rcd=nco_inq_dimlen(in_id,dmn_id_grd_in,&grd_nbr_in);
  if(need_lnd) rcd=nco_inq_dimlen(in_id,dmn_id_lnd_in,&lnd_nbr_in);
  if(need_pft) rcd=nco_inq_dimlen(in_id,dmn_id_pft_in,&pft_nbr_in);

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
  long col_nbr; /* [nbr] Number of columns */
  long lon_nbr; /* [nbr] Number of longitudes */
  long lat_nbr; /* [nbr] Number of latitudes */
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
  if(need_clm) clm_nm_out=(char *)strdup(clm_nm_in);
  if(need_grd) grd_nm_out=(char *)strdup(grd_nm_in);
  if(need_lnd) lnd_nm_out=(char *)strdup(lnd_nm_in);
  if(need_pft) pft_nm_out=(char *)strdup(pft_nm_in);
  int dmn_id_clm_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lnd_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_mec_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_pft_out=NC_MIN_INT; /* [id] Dimension ID */

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
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_vegetated_or_bare_soil",&ilun_vegetated_or_bare_soil,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_crop",&ilun_crop,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_landice",&ilun_landice,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_landice_multiple_elevation_classes",&ilun_landice_multiple_elevation_classes,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_deep_lake",&ilun_deep_lake,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_wetland",&ilun_wetland,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_urban_tbd",&ilun_urban_tbd,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_urban_hd",&ilun_urban_hd,NC_INT);
    rcd=nco_get_att(in_id,NC_GLOBAL,"ilun_urban_md",&ilun_urban_md,NC_INT);
  } /* !flg_nm_hst */

  /* Determine output Column dimension if needed */
  int *cols1d_active=NULL; /* [flg] Column active flag (1=active, 0=inactive) */
  int *cols1d_ityp=NULL; /* [enm] Column type */
  int *cols1d_ityplun=NULL; /* [enm] Column landunit type */
  int *cols1d_ixy=NULL; /* [idx] Column 2D longitude index */
  int *cols1d_jxy=NULL; /* [idx] Column 2D latitude index */
  int clm_typ; /* [enm] Column landunit type */
  if(need_clm){
    if(cols1d_active_id != NC_MIN_INT) cols1d_active=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    if(cols1d_active_id != NC_MIN_INT) rcd=nco_get_var(in_id,cols1d_active_id,cols1d_active,NC_INT);

    if(cols1d_ityp_id != NC_MIN_INT) cols1d_ityp=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    if(cols1d_ityp_id != NC_MIN_INT) rcd=nco_get_var(in_id,cols1d_ityp_id,cols1d_ityp,NC_INT);

    cols1d_ityplun=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    rcd=nco_get_var(in_id,cols1d_ityplun_id,cols1d_ityplun,NC_INT);

    mec_nbr_out=0;
    for(clm_idx=0;clm_idx<clm_nbr_in;clm_idx++){
      if(cols1d_ityplun[clm_idx] != ilun_landice_multiple_elevation_classes) continue;
      while(cols1d_ityplun[clm_idx++] == ilun_landice_multiple_elevation_classes) mec_nbr_out++;
      break;
    } /* !clm_idx */
    /* NB: landice_MEC (ilun=4, usually) landunits have 10 (always, AFAICT) glacier elevation classes */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO mec_nbr_out = %ld\n",nco_prg_nm_get(),mec_nbr_out);
    if(mec_nbr_out > 0) need_mec=True;
    
    cols1d_ixy=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    rcd=nco_get_var(in_id,cols1d_ixy_id,cols1d_ixy,NC_INT);
    if(flg_grd_2D){
      cols1d_jxy=(int *)nco_malloc(clm_nbr_in*sizeof(int));
      rcd=nco_get_var(in_id,cols1d_jxy_id,cols1d_jxy,NC_INT);
    } /* !flg_grd_2D */
    
  } /* !need_clm */
    
  /* Determine output Grid dimension if needed:
     CLM/ELM 'gridcell' dimension counts each gridcell that contains land
     Replace this dimension by horizontal dimension(s) in input data file */
  if(need_grd){
    if(flg_grd_1D) grd_nbr_out=col_nbr;
    if(flg_grd_2D) grd_nbr_out=lat_nbr*lon_nbr;
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO grd_nbr_out = %ld\n",nco_prg_nm_get(),grd_nbr_out);
  } /* !need_grd */

  /* Determine output Landunit dimension if needed */
  int *land1d_active=NULL; /* [flg] Landunit active flag (1=active, 0=inactive) */
  int *land1d_ityplun=NULL; /* [enm] Landunit type */
  int lnd_typ; /* [enm] Landunit type */
  int lnd_typ_crr; /* [enm] Landunit type of current column|landunit|pft */
  int lnd_typ_nxt; /* [enm] Landunit type of next column|landunit|pft */
  if(need_lnd){
    if(land1d_active_id != NC_MIN_INT) land1d_active=(int *)nco_malloc(lnd_nbr_in*sizeof(int));
    if(land1d_active_id != NC_MIN_INT) rcd=nco_get_var(in_id,land1d_active_id,land1d_active,NC_INT);

    if(land1d_ityplun_id != NC_MIN_INT) land1d_ityplun=(int *)nco_malloc(lnd_nbr_in*sizeof(int));
    if(land1d_ityplun_id != NC_MIN_INT) rcd=nco_get_var(in_id,land1d_ityplun_id,land1d_ityplun,NC_INT);

    lnd_nbr_out=9; /* fxm: determine this automagically? */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO lnd_nbr_out = %ld\n",nco_prg_nm_get(),lnd_nbr_out);
  } /* !need_lnd */

  /* Determine output PFT dimension if needed */
  //double *pfts1d_wtgcell=NULL; /* [frc] PFT weight relative to corresponding gridcell */
  int *pfts1d_active=NULL; /* [flg] PFT active flag (1=active, 0=inactive) */
  int *pfts1d_ityp_veg=NULL; /* [enm] PFT vegetation type */
  int *pfts1d_ityplun=NULL; /* [enm] PFT landunit type */
  int *pfts1d_ixy=NULL; /* [idx] PFT 2D longitude index */
  int *pfts1d_jxy=NULL; /* [idx] PFT 2D latitude index */
  int pft_typ; /* [enm] PFT type */
  if(need_pft){
    
    if(pfts1d_active_id != NC_MIN_INT) pfts1d_active=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    if(pfts1d_active_id != NC_MIN_INT) rcd=nco_get_var(in_id,pfts1d_active_id,pfts1d_active,NC_INT);

    //pfts1d_wtgcell=(double *)nco_malloc(pft_nbr_in*sizeof(double));
    pfts1d_ityp_veg=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    pfts1d_ityplun=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    
    //rcd=nco_get_var(in_id,pfts1d_wtgcell_id,pfts1d_wtgcell,NC_DOUBLE);
    rcd=nco_get_var(in_id,pfts1d_ityp_veg_id,pfts1d_ityp_veg,NC_INT);
    rcd=nco_get_var(in_id,pfts1d_ityplun_id,pfts1d_ityplun,NC_INT);
    
    pft_nbr_out=0;
    for(pft_idx=0;pft_idx<pft_nbr_in;pft_idx++){
      if(pfts1d_ityp_veg[pft_idx] > pft_nbr_out) pft_nbr_out=pfts1d_ityp_veg[pft_idx];
      // Slightly more convoluted method originally used on beth_in.nc (to count only crops, I think
      //      if((pfts1d_ityplun[pft_idx] != ilun_vegetated_or_bare_soil) && (pfts1d_ityplun[pft_idx] != ilun_crop)) continue;
      //      /* Skip bare ground, skip MECs */
      //while(pfts1d_ityp_veg[++pft_idx] != 0) pft_nbr_out++;
      //      break;
    } /* !pft_idx */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO pft_nbr_out = %ld\n",nco_prg_nm_get(),pft_nbr_out);

    pfts1d_ixy=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    rcd=nco_get_var(in_id,pfts1d_ixy_id,pfts1d_ixy,NC_INT);
    if(flg_grd_2D){
      pfts1d_jxy=(int *)nco_malloc(pft_nbr_in*sizeof(int));
      rcd=nco_get_var(in_id,pfts1d_jxy_id,pfts1d_jxy,NC_INT);
    } /* !flg_grd_2D */
    
  } /* !need_pft */

  /* Define unpacked versions of needed dimensions before all else */
  if(need_clm && clm_nbr_out > 0L) rcd=nco_def_dim(out_id,clm_nm_out,clm_nbr_out,&dmn_id_clm_out);
  if(need_lnd && lnd_nbr_out > 0L) rcd=nco_def_dim(out_id,lnd_nm_out,lnd_nbr_out,&dmn_id_lnd_out);
  if(need_pft && pft_nbr_out > 0L) rcd=nco_def_dim(out_id,pft_nm_out,pft_nbr_out,&dmn_id_pft_out);

  /* MECs can be a new output dimension if they are enumerated in input (MEC is a column-level sub-unit of a glacier landunit) */
  if(need_mec && mec_nbr_out > 0L) rcd=nco_def_dim(out_id,mec_nm_out,mec_nbr_out,&dmn_id_mec_out);

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
  char *sgs_frc_nm;
  char *lat_bnd_nm;
  char *lon_bnd_nm;
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

  /* clm5.pdf: "The default is to have 10 elevation classes whose lower limits are 0, 200, 400, 700, 1000, 1300, 1600, 2000, 2500, and 3000 m." */
  const double mec[10]={0,200,400,700,1000,1300,1600,2000,2500,3000}; /* [frc] Lowest elevation in each MEC */
  int mec_out_id=NC_MIN_INT; /* [id] Variable ID for MEC */
  if(need_mec){
    dmn_ids_out[0]=dmn_id_mec_out;
    rcd+=nco_def_var(out_id,mec_nm_out,crd_typ_out,dmn_nbr_1D,dmn_ids_out,&mec_out_id);
    if(nco_cmp_glb_get()) rcd+=nco_flt_def_out(out_id,mec_out_id,NULL,nco_flt_flg_prc_fll);
    var_crt_nbr++;
    rcd=nco_char_att_put(out_id,mec_nm_out,"long_name","Lowest elevation");
    rcd=nco_char_att_put(out_id,mec_nm_out,"units","meter");
  } /* !need_mec */
    
  double mss_val_dbl;
  double mss_val_cmp_dbl; /* Missing value for comparison to double precision values */

  int flg_pck; /* [flg] Variable is packed on disk  */
  int nco_s1d_typ; /* [enm] Sparse-1D type of input variable */

  long mrv_idx; /* [idx] MRV index */
  long mrv_nbr; /* [nbr] Product of sizes of dimensions following (thus MRV) column|gridcell|landunit|pft dimension, e.g., lev*|numrad */

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
	       Ratio of idx_in (an element counter) by mrv_nbr yields a clm/lnd/pft index
	       For unrolled dimension (e.g., MEC), must check next landunit as well since some (all?) MEC variables (like DZSNO) are 
	       initially defined on lnd_typ=1 (presumably to be used for non-ice sheet gridcells) and very next column is actual MEC */
	    idx_s1d_crr=NC_MIN_INT;
	    idx_s1d_nxt=NC_MIN_INT;
	    switch(var_typ_in){
	      case NC_FLOAT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
		if(var_val_in.fp[idx_in] != mss_val_cmp_dbl){
		  idx_s1d_crr=idx_in/mrv_nbr;
		  if(idx_in+mrv_nbr < var_sz_in && var_val_in.fp[idx_in+mrv_nbr] != mss_val_cmp_dbl) idx_s1d_nxt=idx_s1d_crr+1L;
		  break;
		} /* !var_val_in.fp */
	      break;
	    case NC_DOUBLE: for(idx_in=0;idx_in<var_sz_in;idx_in++)
		if(var_val_in.dp[idx_in] != mss_val_cmp_dbl){
		  idx_s1d_crr=idx_in/mrv_nbr;
		  if(idx_in+mrv_nbr < var_sz_in && var_val_in.dp[idx_in+mrv_nbr] != mss_val_cmp_dbl) idx_s1d_nxt=idx_s1d_crr+1L;
		  break;
		} /* !var_val_in.dp */
	      break;
	    case NC_INT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
		if(var_val_in.ip[idx_in] != mss_val_cmp_dbl){
		  idx_s1d_crr=idx_in/mrv_nbr;
		  if(idx_in+mrv_nbr < var_sz_in && var_val_in.ip[idx_in+mrv_nbr] != mss_val_cmp_dbl) idx_s1d_nxt=idx_s1d_crr+1L;
		  break;
		} /* !var_val_in.ip */
	      break;
	    default:
	      (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
	      nco_dfl_case_nc_type_err();
	      break;
	    } /* !var_typ_in */
	      //(void)fprintf(stdout,"%s: DEBUG quark1\n",nco_prg_nm_get());
	    if(idx_in == var_sz_in) (void)fprintf(fp_stdout,"%s: INFO %s reports %s has no valid values in input, so will be all missing values in output too...\n",nco_prg_nm_get(),fnc_nm,var_nm);
	    lnd_typ_crr=NC_MIN_INT;
	    lnd_typ_nxt=NC_MIN_INT;
	    if(idx_s1d_crr != NC_MIN_INT){
	      switch(nco_s1d_typ){
	      case nco_s1d_clm:
		lnd_typ_crr=cols1d_ityplun[idx_s1d_crr];
		if(idx_s1d_nxt != NC_MIN_INT) lnd_typ_nxt=cols1d_ityplun[idx_s1d_nxt];
		break;
	      case nco_s1d_lnd:
		lnd_typ_crr=land1d_ityplun[idx_s1d_crr];
		if(idx_s1d_nxt != NC_MIN_INT) lnd_typ_nxt=land1d_ityplun[idx_s1d_nxt];
		break;
	      case nco_s1d_pft:
		lnd_typ_crr=pfts1d_ityplun[idx_s1d_crr];
		if(idx_s1d_nxt != NC_MIN_INT) lnd_typ_nxt=pfts1d_ityplun[idx_s1d_nxt];
		break;
	      case nco_s1d_grd:
	      default: break;
	      } /* !nco_s1d_typ */
	    } /* !idx_s1d_crr */
	    if(lnd_typ_nxt == ilun_landice_multiple_elevation_classes) has_mec=True;
	    if(var_val_in.vp) var_val_in.vp=(void *)nco_free(var_val_in.vp);
	  } /* !need_mec */
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    dmn_id=dmn_ids_in[dmn_idx];
	    rcd=nco_inq_dimname(in_id,dmn_id,dmn_nm);
	    if(dmn_id == dmn_id_clm_in){
	      if(has_mec){
		/* Change input column dimension to MEC when present otherwise eliminate this dimension */
		dmn_ids_out[dmn_idx]=dmn_id_mec_out;
		dmn_cnt_out[dmn_idx]=mec_nbr_out;
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

  if(need_mec) rcd=nco_put_var(out_id,mec_out_id,(void *)mec,crd_typ_out);

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
  //int dmn_idx_col=int_CEWI; /* [idx] Index of column dimension */
  //int dmn_idx_lat=int_CEWI; /* [idx] Index of latitude dimension */
  //int dmn_idx_lon=int_CEWI; /* [idx] Index of longitude dimension */
  int thr_idx; /* [idx] Thread index */
  //int var_id; /* [id] Current variable ID */

  long lvl_idx; /* [idx] Level index */
  long lvl_nbr; /* [nbr] Number of levels */

  size_t val_in_fst; /* [nbr] Number of elements by which current N-D slab input values are offset from origin */
  size_t val_out_fst; /* [nbr] Number of elements by which current N-D slab output values are offset from origin */

  if(idx_dbg == 0L) idx_dbg=11;
  if(nco_dbg_lvl_get() >= nco_dbg_fl){
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
# pragma omp parallel for firstprivate(var_val_in,var_val_out) private(clm_typ,dmn_cnt_in,dmn_cnt_out,dmn_id,dmn_ids_in,dmn_ids_out,dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dmn_nm,dmn_srt,flg_var_mpt,has_clm,has_grd,has_levcan,has_levgrnd,has_levlak,has_levsno,has_levsno1,has_levtot,has_lnd,has_mec,has_pft,has_mss_val,has_numrad,idx_in,idx_out,idx_s1d_crr,idx_s1d_nxt,idx_tbl,in_id,lnd_typ,lnd_typ_crr,lnd_typ_nxt,lvl_idx,lvl_nbr,mrv_idx,mrv_nbr,mss_val,mss_val_dbl,mss_val_cmp_dbl,mss_val_unn,nco_s1d_typ,pft_typ,rcd,thr_idx,trv,val_in_fst,val_out_fst,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_in,var_typ_out) shared(clm_nbr_in,clm_nbr_out,cols1d_ityplun,cols1d_ixy,cols1d_jxy,col_nbr,dmn_id_clm_in,dmn_id_clm_out,dmn_id_col_in,dmn_id_col_out,dmn_id_lat_in,dmn_id_lat_out,dmn_id_levcan_in,dmn_id_levgrnd_in,dmn_id_levlak_in,dmn_id_levsno_in,dmn_id_levsno1_in,dmn_id_levtot_in,dmn_id_lnd_in,dmn_id_lnd_out,dmn_id_lon_in,dmn_id_lon_out,dmn_id_numrad_in,dmn_id_pft_in,dmn_id_pft_out,dmn_nbr_hrz_crd,flg_nm_hst,flg_nm_rst,flg_s1d_clm,flg_s1d_pftlat_nbr,ilun_landice_multiple_elevation_classes,land1d_ityplun,lnd_nbr_in,lnd_nbr_out,lon_nbr,mec_nbr_out,need_mec,out_id,pft_nbr_in,pft_nbr_out,pfts1d_ityplun,pfts1d_ityp_veg,pfts1d_ixy,pfts1d_jxy)
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
	flg_var_mpt=False;
	/* 20240202: Identify special dimensions in input variable
	   mrv_nbr is product of sizes of dimensions following (thus MRV) column|gridcell|landunit|pft */
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
	  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_stdout,"%s: INFO %s dmn_idx = %d, dmn_id = %d, mrv_nbr = %ld\n",fnc_nm,var_nm,dmn_idx,dmn_id,mrv_nbr);	  
	} /* !dmn_idx */
	
	/* Compute number and size of non-lat/lon or non-col dimensions (e.g., level, time, species, wavelength)
	   Denote their convolution by level or 'lvl' for shorthand
	   There are lvl_nbr elements for each lat/lon or col position
	   Dimensions CLM/ELM dimensions that count in "lvl_nbr" include all lev* dimensions, time, numrad */
	lvl_nbr=1L;
	/* Restart files are generally ordered (column|gridcell|landunit|pft,lev*|numrad)
	   Restart files unroll the MEC dimension into the column dimension
	   Restart files unroll the PFT dimension into the pft dimension
	   Restart files explicitly retain all lev*,numrad dimensions
	   NB: lev* and numrad are mutually exclusive (never both appear in same variable)
	   
	   History files are generally ordered (time,lev*,[column|pft]|[lat,lon|lndgrid])
	   Most history file fields contain MRV horizontal spatial dimensions
	   However Beth Drewniak's beth_in.nc fields have no horizontal spatial dimensions,
	   and place lev* as LRV (like normal history fields) not MRV (like normal "vanilla" restart files) */
	if(flg_nm_hst)
	  for(dmn_idx=0;dmn_idx<dmn_nbr_out-dmn_nbr_hrz_crd;dmn_idx++)
	    lvl_nbr*=dmn_cnt_out[dmn_idx];
	if(flg_nm_rst)
	  for(dmn_idx=0;dmn_idx<dmn_nbr_out-dmn_nbr_hrz_crd;dmn_idx++)
	    lvl_nbr*=1L;

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
	    (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
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
	    (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
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
	  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
	  nco_dfl_case_nc_type_err();
	  break;
	} /* !var_typ_in */

	has_clm=has_grd=has_lnd=has_pft=False;
	nco_s1d_typ=nco_s1d_nil;
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	  dmn_id=dmn_ids_in[dmn_idx];
	  if(dmn_id == dmn_id_clm_in) nco_s1d_typ=nco_s1d_clm;
	  else if(dmn_id == dmn_id_grd_in) nco_s1d_typ=nco_s1d_grd;
	  else if(dmn_id == dmn_id_lnd_in) nco_s1d_typ=nco_s1d_lnd;
	  else if(dmn_id == dmn_id_pft_in) nco_s1d_typ=nco_s1d_pft;
	} /* !dmn_idx */

	/* Determine what landunit this variable is defined on
	   Ratio of idx_in (an element counter) by mrv_nbr yields a clm/lnd/pft index
	   For unrolled dimension (e.g., MEC), must check next landunit as well since some (all?) MEC variables (like DZSNO) are 
	   initially defined on lnd_typ=1 (presumably to be used for non-ice sheet gridcells) and very next column is actual MEC */
	idx_s1d_crr=NC_MIN_INT;
	idx_s1d_nxt=NC_MIN_INT;
	switch(var_typ_in){
	case NC_FLOAT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	    if(var_val_in.fp[idx_in] != mss_val_cmp_dbl){
	      idx_s1d_crr=idx_in/mrv_nbr;
	      if(idx_in+mrv_nbr < var_sz_in && var_val_in.fp[idx_in+mrv_nbr] != mss_val_cmp_dbl) idx_s1d_nxt=idx_s1d_crr+1L;
	      break;
	    } /* !var_val_in.fp */
	  break;
	case NC_DOUBLE: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	    if(var_val_in.dp[idx_in] != mss_val_cmp_dbl){
	      idx_s1d_crr=idx_in/mrv_nbr;
	      if(idx_in+mrv_nbr < var_sz_in && var_val_in.dp[idx_in+mrv_nbr] != mss_val_cmp_dbl) idx_s1d_nxt=idx_s1d_crr+1L;
	      break;
	    } /* !var_val_in.dp */
	  break;
	case NC_INT: for(idx_in=0;idx_in<var_sz_in;idx_in++)
	    if(var_val_in.ip[idx_in] != mss_val_cmp_dbl){
	      idx_s1d_crr=idx_in/mrv_nbr;
	      if(idx_in+mrv_nbr < var_sz_in && var_val_in.ip[idx_in+mrv_nbr] != mss_val_cmp_dbl) idx_s1d_nxt=idx_s1d_crr+1L;
	      break;
	    } /* !var_val_in.ip */
	  break;
	default:
	  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
	  nco_dfl_case_nc_type_err();
	  break;
	} /* !var_typ_in */
	if(idx_in == var_sz_in) flg_var_mpt=True;
	lnd_typ_crr=NC_MIN_INT;
	lnd_typ_nxt=NC_MIN_INT;
	if(idx_s1d_crr != NC_MIN_INT){
	  switch(nco_s1d_typ){
	  case nco_s1d_clm:
	    lnd_typ_crr=cols1d_ityplun[idx_s1d_crr];
	    if(idx_s1d_nxt != NC_MIN_INT) lnd_typ_nxt=cols1d_ityplun[idx_s1d_nxt];
	    break;
	  case nco_s1d_lnd:
	    lnd_typ_crr=land1d_ityplun[idx_s1d_crr];
	    if(idx_s1d_nxt != NC_MIN_INT) lnd_typ_nxt=land1d_ityplun[idx_s1d_nxt];
	    break;
	  case nco_s1d_pft:
	    lnd_typ_crr=pfts1d_ityplun[idx_s1d_crr];
	    if(idx_s1d_nxt != NC_MIN_INT) lnd_typ_nxt=pfts1d_ityplun[idx_s1d_nxt];
	    break;
	  case nco_s1d_grd:
	  default: break;
	  } /* !nco_s1d_typ */
	} /* !idx_s1d_crr */
	if(lnd_typ_crr == ilun_vegetated_or_bare_soil && lnd_typ_nxt == ilun_landice_multiple_elevation_classes) has_mec=True;
	
	if(!flg_var_mpt) (void)fprintf(fp_stdout,"%s: %s, idx_in = %ld, s1d_enm = %d = %s, lnd_typ = %d = %s\n",nco_prg_nm_get(),var_nm,idx_in,(int)nco_s1d_typ,nco_s1d_sng(nco_s1d_typ),lnd_typ,nco_lnd_typ_sng(lnd_typ));
	  
	/* The Hard Work */
	if(nco_s1d_typ == nco_s1d_pft){
	  /* Turn GPP(time,pft) into GPP(time,pft,lndgrid) or GPP(time,pft,lat,lon)
	     Turn tlai_z(time,pft,levcan) into tlai_z(time,pft,levcan,lndgrid) or tlai_z(time,pft,levcan,lat,lon)
	     Turn fabd_sun(time,pft,numrad) into fabd_sun(time,pft,numrad,lndgrid) or fabd_sun(time,pft,numrad,lat,lon)
	     20240131: This curently works only for single-timestep files
	     In general, must enclose this in outer loop over (time x level) */
	  val_in_fst=0L;
	  val_out_fst=0L;
	  for(lvl_idx=0;lvl_idx<lvl_nbr;lvl_idx++){
	    /* PFT variable dimension ordering, from LRV to MRV:
	       Restart input: PFT, lev*|numrad, e.g., T_REF2M_MIN_INST(pft), fabd_sun(pft,numrad), tlai_z(pft,levcan)
	       History input: time, PFT, lev*|numrad, spatial
	       NCO Output   : time, PFT, lev*|numrad, spatial */
	    for(pft_idx=0;pft_idx<pft_nbr_in;pft_idx++){

	      pft_typ=pfts1d_ityp_veg[pft_idx]; /* [1 <= pft_typ <= pft_nbr_out] */
	      /* Skip bare ground so output array contains only vegetated types */
	      if(!pft_typ) continue;

	      /* grd_idx is 0-based index relative to the origin of the horizontal grid, pfts1d is 1-based
		 [0 <= grd_idx_out <= col_nbr_out-1L], [1 <= pfts1d_ixy <= col_nbr_out]
		 Storage order for history fields (Beth's GPP, anyway) is lon,lat
		 Storage order for restart fields (e.g., DZSNO) is lat,lon */
	      if(flg_nm_hst) grd_idx_out= flg_grd_1D ? pfts1d_ixy[pft_idx]-1L : (pfts1d_ixy[pft_idx]-1L)*lat_nbr+(pfts1d_jxy[pft_idx]-1L);
	      if(flg_nm_rst) grd_idx_out= flg_grd_1D ? pfts1d_ixy[pft_idx]-1L : (pfts1d_jxy[pft_idx]-1L)*lon_nbr+(pfts1d_ixy[pft_idx]-1L);

	      for(mrv_idx=0;mrv_idx<mrv_nbr;mrv_idx++){
		/* Recall that lev*|numrad are MRV in restart input, and are LRV in output where lev*|numrad precedes column,[lat,lon|lndgrid] */
		if(flg_nm_hst) idx_in=pft_idx;
		if(flg_nm_rst) idx_in=pft_idx*mrv_nbr+mrv_idx;
		idx_out=(pft_typ-1)*grd_sz_out+grd_idx_out;
		/* memcpy() would allow next statement to work for generic types
		   However, memcpy() is a system call and could be expensive in an innermost loop */
		switch(var_typ_out){
		case NC_FLOAT: var_val_out.fp[idx_out]=var_val_in.fp[idx_in]; break;
		case NC_DOUBLE: var_val_out.dp[idx_out]=var_val_in.dp[idx_in]; break;
		case NC_INT: var_val_out.ip[idx_out]=var_val_in.ip[idx_in]; break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
		  nco_dfl_case_nc_type_err();
		  break;
		} /* !var_typ_out */
	      } /* !mrv_idx */
	    } /* !pft_idx */
	    val_in_fst+=grd_sz_in; /* fxm */
	    val_out_fst+=grd_sz_out;
	  } /* !lvl_idx */
	} /* !nco_s1d_typ */
	
	if(nco_s1d_typ == nco_s1d_clm){
	  /* Turn SNOW_DEPTH(column) into SNOW_DEPTH(mec,lndgrid) or SNOW_DEPTH(mec,lat,lon) 
	     Turn DZSNO(column,levsno) into DZSNO(mec,levsno,lndgrid) or DZSNO(mec,levsno,lat,lon)
	     Support will start for MECs only */

	  /* MEC variable dimension ordering, from LRV to MRV:
	     Restart input: MEC, lev*, e.g., DZSNO(column,levsno), H2OSOI_ICE(column,levtot), T_SOISNO(column,levtot), ZSNO(column,levsno), flx_absdn(column,levsno1), qflx_snofrz_lyr(column,levsno), snw_rds(column,levsno)
	     History input: time, lev*, spatial, e.g., SNO_BW(time,levsno,lat,lon)
	     NCO Output   : time, MEC, lev*, spatial
	     ncks --trd -C -d column,0,11 -v DZSNO,cols1d_gridcell_index ${DATA}/bm/elm_mali_rst.nc | m */
	  mec_idx=0;
	  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(fp_stdout,"%s: INFO %s clm_nbr = %ld, mec_nbr = %ld, mrv_nbr = %ld\n",fnc_nm,var_nm,clm_nbr_in,mec_nbr_out,mrv_nbr);
	  for(clm_idx=0;clm_idx<clm_nbr_in;clm_idx++){
	    lnd_typ=cols1d_ityplun[clm_idx]; /* [1 <= lnd_typ <= lnd_nbr_out] */

	    /* Skip columns with LUTs for which this variable is undefined
	       The !has_mec exception is subtle---it ensures MEC fields alway pass through 
	       NB: LUT=1 values for MEC fields (e.g., DZSNO) are written into MEC=1 slot by non-MEC block
	       The MEC=1 value for the same gridcell overwrites the LUT=1 value in the MEC=1 slot */
	    if(lnd_typ != lnd_typ_crr && !has_mec) continue;
	    
	    /* grd_idx is 0-based index relative to the origin of the horizontal grid, cols1d is 1-based
	       [0 <= grd_idx_out <= col_nbr_out-1L], [1 <= cols1d_ixy <= col_nbr_out]
	       Storage order for history fields (Beth's GPP, anyway) is lon,lat
	       Storage order for restart fields (e.g., DZSNO) is lat,lon */
	    grd_idx_out= flg_grd_1D ? cols1d_ixy[clm_idx]-1L : (cols1d_jxy[clm_idx]-1L)*lon_nbr+(cols1d_ixy[clm_idx]-1L);
	    
	    /* Process MEC fields */
	    switch(lnd_typ){
	    case nco_lnd_ilun_landice_multiple_elevation_classes:
	      for(mrv_idx=0;mrv_idx<mrv_nbr;mrv_idx++){
		/* Recall that lev*|numrad are MRV in restart input, and are LRV in output where lev*|numrad precedes column,[lat,lon|lndgrid] */
		idx_in=clm_idx*mrv_nbr+mrv_idx;
		idx_out=mec_idx*mrv_nbr*grd_sz_out+mrv_idx*grd_sz_out+grd_idx_out;
		switch(var_typ_out){
		case NC_FLOAT: var_val_out.fp[idx_out]=var_val_in.fp[idx_in]; break;
		case NC_DOUBLE: var_val_out.dp[idx_out]=var_val_in.dp[idx_in]; break;
		case NC_INT: var_val_out.ip[idx_out]=var_val_in.ip[idx_in]; break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
		  nco_dfl_case_nc_type_err();
		  break;
		} /* !var_typ_out */
		if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(fp_stdout,"%s: INFO %s clm_idx = %ld, mec_idx = %ld, mrv_idx = %ld\n",fnc_nm,var_nm,clm_idx,mec_idx,mrv_idx);
	      } /* !mrv_idx */
	      /* Increment MEC counter and reset to zero if highest MEC */
	      if(++mec_idx == mec_nbr_out) mec_idx=0;
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
		switch(var_typ_out){
		case NC_FLOAT: var_val_out.fp[idx_out]=var_val_in.fp[idx_in]; break;
		case NC_DOUBLE: var_val_out.dp[idx_out]=var_val_in.dp[idx_in]; break;
		case NC_INT: var_val_out.ip[idx_out]=var_val_in.ip[idx_in]; break;
		default:
		  (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
		  nco_dfl_case_nc_type_err();
		  break;
		} /* !var_typ_out */
	      } /* !mrv_idx */
	      break; /* !ilun_deep_lake */
	    default:
	      (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
	      nco_dfl_case_generic_err((int)lnd_typ);
	      break; /* !default */
	    } /* !lnd_typ */

	  } /* !clm_idx */
	} /* !nco_s1d_typ */
	
#pragma omp critical
	{ /* begin OpenMP critical */
	  rcd=nco_put_vara(out_id,var_id_out,dmn_srt,dmn_cnt_out,var_val_out.vp,var_typ_out);
	} /* end OpenMP critical */
	
	if(dmn_ids_in) dmn_ids_in=(int *)nco_free(dmn_ids_in);
	if(dmn_ids_out) dmn_ids_out=(int *)nco_free(dmn_ids_out);
	if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
	if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
	if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
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
  if(nco_dbg_lvl_get() >= nco_dbg_fl) (void)fprintf(stdout,"%s: INFO %s completion report: Variables interpolated = %d, copied unmodified = %d, omitted = %d, created = %d\n",nco_prg_nm_get(),fnc_nm,var_rgr_nbr,var_cpy_nbr,var_xcl_nbr,var_crt_nbr);

  /* Free output data memory */
  if(cols1d_active) cols1d_active=(int *)nco_free(cols1d_active);
  if(cols1d_ityp) cols1d_ityp=(int *)nco_free(cols1d_ityp);
  if(cols1d_ityplun) cols1d_ityplun=(int *)nco_free(cols1d_ityplun);
  if(cols1d_ixy) cols1d_ixy=(int *)nco_free(cols1d_ixy);
  if(cols1d_jxy) cols1d_jxy=(int *)nco_free(cols1d_jxy);
  if(land1d_active) land1d_active=(int *)nco_free(land1d_active);
  if(land1d_ityplun) land1d_ityplun=(int *)nco_free(land1d_ityplun);
  if(pfts1d_active) pfts1d_active=(int *)nco_free(pfts1d_active);
  if(pfts1d_ityp_veg) pfts1d_ityp_veg=(int *)nco_free(pfts1d_ityp_veg);
  if(pfts1d_ityplun) pfts1d_ityplun=(int *)nco_free(pfts1d_ityplun);
  if(pfts1d_ixy) pfts1d_ixy=(int *)nco_free(pfts1d_ixy);
  if(pfts1d_jxy) pfts1d_jxy=(int *)nco_free(pfts1d_jxy);
  //if(pfts1d_wtgcell) pfts1d_wtgcell=(double *)nco_free(pfts1d_wtgcell);

  if(clm_nm_in) clm_nm_in=(char *)nco_free(clm_nm_in);
  if(grd_nm_in) grd_nm_in=(char *)nco_free(grd_nm_in);
  if(lnd_nm_in) lnd_nm_in=(char *)nco_free(lnd_nm_in);
  if(pft_nm_in) pft_nm_in=(char *)nco_free(pft_nm_in);
  if(clm_nm_out) clm_nm_out=(char *)nco_free(clm_nm_out);
  if(grd_nm_out) grd_nm_out=(char *)nco_free(grd_nm_out);
  if(lnd_nm_out) lnd_nm_out=(char *)nco_free(lnd_nm_out);
  if(pft_nm_out) pft_nm_out=(char *)nco_free(pft_nm_out);

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
