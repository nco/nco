/* $Header$ */

/* Purpose: NCO utilities for Sparse-1D (S1D) datasets */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_s1d.h" /* Sparse-1D datasets */

const char * /* O [sng] String describing sparse-type */
nco_s1d_sng /* [fnc] Convert sparse-1D type enum to string */
(const nco_s1d_typ_enm nco_s1d_typ) /* I [enm] Sparse-1D type enum */
{
  /* Purpose: Convert sparse-type enum to string */
  switch(nco_s1d_typ){
  case nco_s1d_clm: return "Sparse Column (cols1d) format";
  case nco_s1d_grd: return "Sparse Gridcell (grid1d) format";
  case nco_s1d_lnd: return "Sparse Landunit (land1d) format";
  case nco_s1d_pft: return "Sparse PFT (pfts1d) format" ;
  default: nco_dfl_case_generic_err(); break;
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

  const char fnc_nm[]="nco_s1d_unpack()"; /* [sng] Function name */

  char *fl_in;
  char *fl_out;
  char *fl_tpl; /* [sng] Template file (contains horizontal grid) */

  char dmn_nm[NC_MAX_NAME]; /* [sng] Dimension name */
  char *grd_nm_in=(char *)strdup("gridcell");
  char *lnd_nm_in=(char *)strdup("landunit");
  char *clm_nm_in=(char *)strdup("column");
  char *pft_nm_in=(char *)strdup("pft");

  char *mec_nm_out=(char *)strdup("mec");

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int in_id; /* I [id] Input netCDF file ID */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;
  int tpl_id; /* [id] Input netCDF file ID (for horizontal grid template) */

  long int clm_idx;
  long int grd_idx_out;
  long int idx_out;
  //long int lat_idx;
  //long int lon_idx;
  long int pft_idx;
  int dmn_idx; /* [idx] Dimension index */

  /* Initialize local copies of command-line values */
  dfl_lvl=rgr->dfl_lvl;
  fl_in=rgr->fl_in;
  fl_out=rgr->fl_out;
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
     2. A fraction of a landunit, which is a fraction of a CTSM/ELM gridcell
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
      else if((rcd=nco_inq_dimid_flg(tpl_id,"latitude",&dmn_id_lat_in)) == NC_NOERR) lat_nm_in=strdup("lndgrid"); /* CF */
      if(lon_nm_in && (rcd=nco_inq_dimid_flg(tpl_id,lon_nm_in,&dmn_id_lon_in)) == NC_NOERR) /* do nothing */; 
      else if((rcd=nco_inq_dimid_flg(tpl_id,"longitude",&dmn_id_lon_in)) == NC_NOERR) lon_nm_in=strdup("lndgrid"); /* CF */
    } /* !flg_grd_1D */
    if(dmn_id_lat_in != NC_MIN_INT && dmn_id_lon_in != NC_MIN_INT) flg_grd_2D=True;

    /* Set where to obtain horizontal grid */
    if(!flg_grd_1D && !flg_grd_2D){
      (void)fprintf(stderr,"%s: ERROR %s did not locate horizontal grid in input data file %s or in template file %s.\nHINT: One of those files must contain the grid dimensions and coordinates used by the packed data in the input data file.\n",nco_prg_nm_get(),fnc_nm,fl_in,fl_tpl);
      nco_exit(EXIT_FAILURE);
    } /* !flg_grd_1D */

  } /* !flg_grd_tpl */

  int cols1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of column */
  int cols1d_ixy_id=NC_MIN_INT; /* [id] Column 2D longitude index */
  int cols1d_jxy_id=NC_MIN_INT; /* [id] Column 2D latitude index */
  int cols1d_lat_id=NC_MIN_INT; /* [id] Column latitude */
  int cols1d_lon_id=NC_MIN_INT; /* [id] Column longitude */
  int cols1d_ityp_id=NC_MIN_INT; /* [id] Column type */
  int cols1d_ityplun_id=NC_MIN_INT; /* [id] Column landunit type */

  int grid1d_ixy_id=NC_MIN_INT; /* [id] Gridcell 2D longitude index */
  int grid1d_jxy_id=NC_MIN_INT; /* [id] Gridcell 2D latitude index */
  int grid1d_lat_id=NC_MIN_INT; /* [id] Gridcell latitude */
  int grid1d_lon_id=NC_MIN_INT; /* [id] Gridcell longitude */

  int land1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of landunit */
  int land1d_ixy_id=NC_MIN_INT; /* [id] Landunit 2D longitude index */
  int land1d_jxy_id=NC_MIN_INT; /* [id] Landunit 2D latitude index */
  int land1d_lat_id=NC_MIN_INT; /* [id] Landunit latitude */
  int land1d_lon_id=NC_MIN_INT; /* [id] Landunit longitude */

  int pfts1d_column_index_id=NC_MIN_INT; /* [id] Column index of PFT */
  int pfts1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of PFT */
  int pfts1d_ityp_veg_id=NC_MIN_INT; /* [id] PFT vegetation type */
  int pfts1d_ityplun_id=NC_MIN_INT; /* [id] PFT landunit type */
  int pfts1d_ixy_id=NC_MIN_INT; /* [id] PFT 2D longitude index */
  int pfts1d_jxy_id=NC_MIN_INT; /* [id] PFT 2D latitude index */
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
    rcd=nco_inq_varid_flg(in_id,"land1d_gridcell_index",&land1d_gridcell_index_id);
    rcd=nco_inq_varid(in_id,"land1d_ixy",&land1d_ixy_id);
    rcd=nco_inq_varid(in_id,"land1d_jxy",&land1d_jxy_id);
    rcd=nco_inq_varid(in_id,"land1d_lon",&land1d_lon_id);
  } /* !flg_s1d_lnd */
     
  rcd=nco_inq_varid_flg(in_id,"pfts1d_lat",&pfts1d_lat_id);
  if(pfts1d_lat_id != NC_MIN_INT) flg_s1d_pft=True;
  if(flg_s1d_pft){
    rcd=nco_inq_varid(in_id,"pfts1d_ixy",&pfts1d_ixy_id);
    rcd=nco_inq_varid(in_id,"pfts1d_jxy",&pfts1d_jxy_id);
    rcd=nco_inq_varid(in_id,"pfts1d_lon",&pfts1d_lon_id);
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
  nco_bool has_clm; /* [flg] Contains column dimension */
  nco_bool has_grd; /* [flg] Contains gridcell dimension */
  nco_bool has_lnd; /* [flg] Contains landunit dimension */
  nco_bool has_pft; /* [flg] Contains PFT dimension */
  nco_bool need_clm=False; /* [flg] At least one variable to unpack needs column dimension */
  nco_bool need_grd=False; /* [flg] At least one variable to unpack needs gridcell dimension */
  nco_bool need_lnd=False; /* [flg] At least one variable to unpack needs landunit dimension */
  nco_bool need_mec=False; /* [flg] At least one variable to unpack needs MEC dimension */
  nco_bool need_pft=False; /* [flg] At least one variable to unpack needs PFT dimension */ 
  trv_sct trv; /* [sct] Traversal table object structure to reduce indirection */
  /* Define unpacking flag for each variable */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      dmn_nbr_in=trv_tbl->lst[idx_tbl].nbr_dmn;
      has_clm=has_grd=has_lnd=has_pft=False;
      for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	/* Pre-determine flags necessary during next loop */
	dmn_nm_cp=trv.var_dmn[dmn_idx].dmn_nm;
	if(!has_clm && clm_nm_in) has_clm=!strcmp(dmn_nm_cp,clm_nm_in);
	if(!has_grd && grd_nm_in) has_grd=!strcmp(dmn_nm_cp,grd_nm_in);
	if(!has_lnd && lnd_nm_in) has_lnd=!strcmp(dmn_nm_cp,lnd_nm_in);
	if(!has_pft && pft_nm_in) has_pft=!strcmp(dmn_nm_cp,pft_nm_in);
      } /* !dmn_idx */
      /* Unpack variables that contain a sparse-1D dimension */
      if(has_clm || has_grd || has_lnd || has_pft){
	trv_tbl->lst[idx_tbl].flg_rgr=True;
	var_rgr_nbr++;
	if(has_clm) need_clm=True;
	if(has_grd) need_grd=True;
	if(has_lnd) need_lnd=True;
	if(has_pft) need_pft=True;
      } /* endif */
      /* Copy all variables that are not regridded or omitted */
      if(!trv_tbl->lst[idx_tbl].flg_rgr) var_cpy_nbr++;
    } /* end nco_obj_typ_var */
  } /* end idx_tbl */
  if(!var_rgr_nbr) (void)fprintf(stdout,"%s: WARNING %s reports no variables fit unpacking criteria. The sparse data unpacker expects at least one variable to unpack, and variables not unpacked are copied straight to output. HINT: If the name(s) of the input sparse-1D dimensions (e.g., \"column\", \"landunit\", and \"pft\") do not match NCO's preset defaults (case-insensitive unambiguous forms and abbreviations of \"column\", \"landunit\", and/or \"pft\", respectively) then change the dimension names that NCO looks for. Instructions are at http://nco.sf.net/nco.html#sparse. For CTSM/ELM sparse-1D coordinate grids, the \"column\", \"landunit\", and \"pft\" variable names can be set with, e.g., \"ncks --rgr column_nm=clm#landunit_nm=lnd#pft_nm=pft\" or \"ncremap -R '--rgr clm=clm#lnd=lnd#pft=pft'\".\n",nco_prg_nm_get(),fnc_nm);
  if(nco_dbg_lvl_get() >= nco_dbg_fl){
    for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
      trv=trv_tbl->lst[idx_tbl];
      if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr) (void)fprintf(stderr,"Unpack %s? %s\n",trv.nm,trv.flg_rgr ? "Yes" : "No");
    } /* end idx_tbl */
  } /* end dbg */

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
  char *lat_dmn_nm_out;
  char *lon_dmn_nm_out;
  int dmn_id_bnd_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_col_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lat_out=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lon_out=NC_MIN_INT; /* [id] Dimension ID */

  if(rgr->bnd_nm) bnd_nm_out=rgr->bnd_nm; else bnd_nm_out=bnd_nm_in;
  if(rgr->col_nm_out) col_nm_out=rgr->col_nm_out; else col_nm_out=col_nm_in;
  if(rgr->lat_dmn_nm) lat_dmn_nm_out=rgr->lat_dmn_nm; else lat_dmn_nm_out=lat_nm_in;
  if(rgr->lon_dmn_nm) lon_dmn_nm_out=rgr->lon_dmn_nm; else lon_dmn_nm_out=lon_nm_in;
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

  /* fxm: make an ilun enumerated type? */
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
  int *cols1d_ityp=NULL; /* [id] Column type */
  int *cols1d_ityplun=NULL; /* [id] Column landunit type */
  if(need_clm){
    if(cols1d_ityp_id != NC_MIN_INT) cols1d_ityp=(int *)nco_malloc(clm_nbr_in*sizeof(int));
    cols1d_ityplun=(int *)nco_malloc(clm_nbr_in*sizeof(int));

    if(cols1d_ityp_id != NC_MIN_INT) rcd=nco_get_var(in_id,cols1d_ityp_id,cols1d_ityp,NC_INT);
    rcd=nco_get_var(in_id,cols1d_ityplun_id,cols1d_ityplun,NC_INT);

    mec_nbr_out=0;
    for(clm_idx=0;clm_idx<clm_nbr_in;clm_idx++){
      if(cols1d_ityplun[clm_idx] != ilun_landice_multiple_elevation_classes) continue;
      while(cols1d_ityplun[clm_idx++] == ilun_landice_multiple_elevation_classes) mec_nbr_out++;
      break;
    } /* !clm_idx */
    /* NB: landice_MEC (ilun=4, usually) landunits have 10 (always, AFAICT) glacier elevation classes */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO mec_nbr_out = %ld\n",nco_prg_nm_get(),mec_nbr_out);
  } /* !need_clm */
    
  /* Determine output Grid dimension if needed:
     CLM/ELM 'gridcell' dimension counts each gridcell that contains land
     Replace this dimension by horizontal dimension(s) in input data file */
  if(need_clm){
    if(flg_grd_1D) grd_nbr_out=col_nbr;
    if(flg_grd_2D) grd_nbr_out=lat_nbr*lon_nbr;
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO grd_nbr_out = %ld\n",nco_prg_nm_get(),grd_nbr_out);
  } /* !need_grd */

  /* Determine output Landunit dimension if needed */
  if(need_lnd){
    lnd_nbr_out=3; /* fxm: Based on TBUILD variable for 3 urban landunit types */
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: INFO lnd_nbr_out = %ld\n",nco_prg_nm_get(),lnd_nbr_out);
  } /* !need_lnd */

  /* Determine output PFT dimension if needed */
  //double *pfts1d_wtgcell=NULL; /* [id] PFT weight relative to corresponding gridcell */
  int *pfts1d_ityp_veg=NULL; /* [id] PFT vegetation type */
  int *pfts1d_ityplun=NULL; /* [id] PFT landunit type */
  int *pfts1d_ixy=NULL; /* [id] PFT 2D longitude index */
  int *pfts1d_jxy=NULL; /* [id] PFT 2D latitude index */
  int pft_typ; /* [enm] PFT type */
  if(need_pft){
    
    //pfts1d_wtgcell=(double *)nco_malloc(pft_nbr_in*sizeof(double));
    pfts1d_ityp_veg=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    pfts1d_ityplun=(int *)nco_malloc(pft_nbr_in*sizeof(int));
    
    //rcd=nco_get_var(in_id,pfts1d_wtgcell_id,pfts1d_wtgcell,NC_DOUBLE);
    rcd=nco_get_var(in_id,pfts1d_ityp_veg_id,pfts1d_ityp_veg,NC_INT);
    rcd=nco_get_var(in_id,pfts1d_ityplun_id,pfts1d_ityplun,NC_INT);
    
    pft_nbr_out=0;
    for(pft_idx=0;pft_idx<pft_nbr_in;pft_idx++){
      if((pfts1d_ityplun[pft_idx] != ilun_vegetated_or_bare_soil) && (pfts1d_ityplun[pft_idx] != ilun_crop)) continue;
      /* Skip bare ground */
      while(pfts1d_ityp_veg[++pft_idx] != 0) pft_nbr_out++;
      break;
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
  (void)fprintf(stdout,"%s: DEBUG quark1\n",nco_prg_nm_get());
  if(need_clm && clm_nbr_out > 0L) rcd=nco_def_dim(out_id,clm_nm_out,clm_nbr_out,&dmn_id_clm_out);
  if(need_lnd && lnd_nbr_out > 0L) rcd=nco_def_dim(out_id,lnd_nm_out,lnd_nbr_out,&dmn_id_lnd_out);
  if(need_pft && pft_nbr_out > 0L) rcd=nco_def_dim(out_id,pft_nm_out,pft_nbr_out,&dmn_id_pft_out);

  /* Assume MECs are new output dimension if they are enumerated in input */
  if(mec_nbr_out > 0L) rcd=nco_def_dim(out_id,mec_nm_out,mec_nbr_out,&dmn_id_mec_out);

  /* Pre-allocate dimension ID and cnt/srt space */
  char *var_nm; /* [sng] Variable name */
  int *dmn_ids_in=NULL; /* [id] Dimension IDs */
  int *dmn_ids_out=NULL; /* [id] Dimension IDs */
  int dmn_nbr_max; /* [nbr] Maximum number of dimensions variable can have in input or output */
  int var_id_in; /* [id] Variable ID */
  int var_id_out; /* [id] Variable ID */
  long *dmn_cnt_in=NULL;
  long *dmn_cnt_out=NULL;
  long *dmn_srt=NULL;
  nc_type var_typ; /* [enm] Variable type (same for input and output variable) */
  nco_bool PCK_ATT_CPY=True; /* [flg] Copy attributes "scale_factor", "add_offset" */

  int dmn_in_fst; /* [idx] Offset of input- relative to output-dimension due to non-MRV dimension insertion */
  int dmn_nbr_rec; /* [nbr] Number of unlimited dimensions */
  int *dmn_ids_rec=NULL; /* [id] Unlimited dimension IDs */

  rcd+=nco_inq_ndims(in_id,&dmn_nbr_max);
  dmn_ids_in=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  dmn_ids_out=(int *)nco_malloc(dmn_nbr_max*sizeof(int));
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  dmn_srt=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
  if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
  if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);
  dmn_cnt_in=(long *)nco_malloc(dmn_nbr_max*sizeof(long));
  dmn_cnt_out=(long *)nco_malloc(dmn_nbr_max*sizeof(long));

  /* Obtain record dimension information from data file (restart files have no time dimension) */
  rcd+=nco_inq_unlimdims(in_id,&dmn_nbr_rec,(int *)NULL);
  if(dmn_nbr_rec > 0){
    dmn_ids_rec=(int *)nco_malloc(dmn_nbr_rec*sizeof(int));
    rcd=nco_inq_unlimdims(in_id,&dmn_nbr_rec,dmn_ids_rec);
  } /* !dmn_nbr_rec */

  int shuffle; /* [flg] Turn-on shuffle filter */
  int deflate; /* [flg] Turn-on deflate filter */
  deflate=(int)True;
  shuffle=NC_SHUFFLE;
  dfl_lvl=rgr->dfl_lvl;
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

  (void)fprintf(stdout,"%s: DEBUG quark2\n",nco_prg_nm_get());
  if(flg_grd_1D){
    rcd+=nco_def_var(out_id,lat_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_col_out,&lat_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_out_id,shuffle,deflate,dfl_lvl);
    (void)nco_att_cpy(hrz_id,out_id,lat_in_id,lat_out_id,PCK_ATT_CPY);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lon_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_col_out,&lon_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_out_id,shuffle,deflate,dfl_lvl);
    (void)nco_att_cpy(hrz_id,out_id,lon_in_id,lon_out_id,PCK_ATT_CPY);
    var_crt_nbr++;
    if(flg_lat_bnd_out){
      dmn_ids_out[0]=dmn_id_col_out;
      dmn_ids_out[1]=dmn_id_bnd_out;
      rcd+=nco_def_var(out_id,lat_bnd_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lat_bnd_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_bnd_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,lat_bnd_in_id,lat_bnd_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_lat_bnd_out */
    if(flg_lon_bnd_out){
      dmn_ids_out[0]=dmn_id_col_out;
      dmn_ids_out[1]=dmn_id_bnd_out;
      rcd+=nco_def_var(out_id,lon_bnd_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lon_bnd_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_bnd_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,lon_bnd_in_id,lon_bnd_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_lon_bnd_out */
    if(flg_area_out){
      rcd+=nco_def_var(out_id,area_nm,crd_typ_out,dmn_nbr_1D,&dmn_id_col_out,&area_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,area_in_id,area_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_area_out */
    if(flg_sgs_frc_out){
      rcd+=nco_def_var(out_id,sgs_frc_nm,crd_typ_out,dmn_nbr_1D,&dmn_id_col_out,&sgs_frc_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,sgs_frc_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,sgs_frc_in_id,sgs_frc_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_sgs_frc_out */
    if(flg_sgs_msk_out){
      rcd+=nco_def_var(out_id,sgs_msk_nm,(nc_type)NC_INT,dmn_nbr_1D,&dmn_id_col_out,&sgs_msk_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,sgs_msk_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,sgs_msk_in_id,sgs_msk_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_sgs_msk_out */
  } /* !flg_grd_1D */
  if(flg_grd_2D){
    rcd+=nco_def_var(out_id,lat_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_lat_out,&lat_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_out_id,shuffle,deflate,dfl_lvl);
    (void)nco_att_cpy(hrz_id,out_id,lat_in_id,lat_out_id,PCK_ATT_CPY);
    var_crt_nbr++;
    rcd+=nco_def_var(out_id,lon_nm_out,crd_typ_out,dmn_nbr_1D,&dmn_id_lon_out,&lon_out_id);
    if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_out_id,shuffle,deflate,dfl_lvl);
    (void)nco_att_cpy(hrz_id,out_id,lon_in_id,lon_out_id,PCK_ATT_CPY);
    var_crt_nbr++;
    if(flg_lat_bnd_out){
      dmn_ids_out[0]=dmn_id_lat_out;
      dmn_ids_out[1]=dmn_id_bnd_out;
      rcd+=nco_def_var(out_id,lat_bnd_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lat_bnd_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lat_bnd_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,lat_bnd_in_id,lat_bnd_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_lat_bnd_out */
    if(flg_lon_bnd_out){
      dmn_ids_out[0]=dmn_id_lon_out;
      dmn_ids_out[1]=dmn_id_bnd_out;
      rcd+=nco_def_var(out_id,lon_bnd_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&lon_bnd_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,lon_bnd_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,lon_bnd_in_id,lon_bnd_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_lon_bnd_out */
    dmn_ids_out[0]=dmn_id_lat_out;
    dmn_ids_out[1]=dmn_id_lon_out;
    if(flg_area_out){
      rcd+=nco_def_var(out_id,area_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&area_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,area_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,area_in_id,area_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_area_out */
    if(flg_sgs_frc_out){
      rcd+=nco_def_var(out_id,sgs_frc_nm,crd_typ_out,dmn_nbr_2D,dmn_ids_out,&sgs_frc_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,sgs_frc_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,sgs_frc_in_id,sgs_frc_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_sgs_frc_out */
    if(flg_sgs_msk_out){
      rcd+=nco_def_var(out_id,sgs_msk_nm,(nc_type)NC_INT,dmn_nbr_2D,dmn_ids_out,&sgs_msk_out_id);
      if(dfl_lvl > 0) (void)nco_def_var_deflate(out_id,sgs_msk_out_id,shuffle,deflate,dfl_lvl);
      (void)nco_att_cpy(hrz_id,out_id,sgs_msk_in_id,sgs_msk_out_id,PCK_ATT_CPY);
      var_crt_nbr++;
    } /* !flg_sgs_msk_out */
  } /* !flg_grd_2D */
  
  int flg_pck; /* [flg] Variable is packed on disk  */
  nco_bool has_mss_val; /* [flg] Has numeric missing value attribute */
  nco_bool flg_add_spc_crd; /* [flg] Add spatial coordinates to S1D variable */
  float mss_val_flt;
  double mss_val_dbl;
  nco_s1d_typ_enm nco_s1d_typ; /* [enm] Sparse-1D type of input variable */
  aed_sct aed_mtd_fll_val;

  (void)fprintf(stdout,"%s: DEBUG quark3\n",nco_prg_nm_get());
  /* Define unpacked S1D and copied variables in output file */
  for(idx_tbl=0;idx_tbl<trv_nbr;idx_tbl++){
    trv=trv_tbl->lst[idx_tbl];
    if(trv.nco_typ == nco_obj_typ_var && trv.flg_xtr){
      var_nm=trv.nm;
      /* Preserve input type in output type */
      var_typ=trv.var_typ;
      dmn_nbr_in=trv.nbr_dmn;
      dmn_nbr_out=trv.nbr_dmn;
      rcd=nco_inq_varid(in_id,var_nm,&var_id_in);
      rcd=nco_inq_varid_flg(out_id,var_nm,&var_id_out);
      /* If variable has not been defined, define it */
      if(rcd != NC_NOERR){
	if(trv.flg_rgr){
	  /* Unpack */
	  (void)fprintf(stdout,"%s: DEBUG quark4\n",nco_prg_nm_get());
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_ids_in);
	  dmn_in_fst=0;
	  flg_add_spc_crd=False;
	  rcd=nco_inq_var_packing(in_id,var_id_in,&flg_pck);
	  if(flg_pck) (void)fprintf(stdout,"%s: WARNING %s reports S1D variable \"%s\" is packed so results unpredictable. HINT: If regridded values seems weird, retry after unpacking input file with, e.g., \"ncpdq -U in.nc out.nc\"\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    rcd=nco_inq_dimname(in_id,dmn_ids_in[dmn_idx],dmn_nm);
	    if(clm_nm_in && !strcmp(dmn_nm,clm_nm_in)){
	      if(mec_nbr_out > 0L){
		/* Change input column dimension to MEC if present */
		dmn_ids_out[dmn_idx]=dmn_id_mec_out;
		dmn_cnt_out[dmn_idx]=mec_nbr_out;
		dmn_in_fst++;
		dmn_nbr_out++;
	      } /* !mec_nbr_out */
	      flg_add_spc_crd=True;
	    }else if(!strcmp(dmn_nm,grd_nm_in)){
	      /* Gridcell dimension disappears to become spatial dimension in output */
	      flg_add_spc_crd=True;
	    }else if(!strcmp(dmn_nm,lnd_nm_in)){
	      /* Change landunit dimension */
	      dmn_ids_out[dmn_idx]=dmn_id_lnd_out;
	      dmn_cnt_out[dmn_idx]=lnd_nbr_out;
	      flg_add_spc_crd=True;
	    }else if(!strcmp(dmn_nm,pft_nm_in)){
	      if(pft_nbr_out > 0L){
		/* Change input PFT dimension to PFT if present */
		dmn_ids_out[dmn_idx]=dmn_id_pft_out;
		dmn_cnt_out[dmn_idx]=pft_nbr_out;
		dmn_in_fst++;
		dmn_nbr_out++;
	      } /* !pft_nbr_out */
	      flg_add_spc_crd=True;
	    }else{
	      /* Dimensions [clm/lnd/pft]_nm_in were pre-defined above as [clm/lnd/pft]_nm_out, replicate all other dimensions */
	      rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_ids_out+dmn_idx);
	    } /* !clm */
	    if(rcd != NC_NOERR){
	      /* Current input dimension is not yet in output file */
	      (void)fprintf(stdout,"%s: DEBUG var_nm = %s, dmn_nm = %s\n",nco_prg_nm_get(),var_nm,dmn_nm);
	      rcd=nco_inq_dimlen(in_id,dmn_ids_in[dmn_idx],dmn_cnt_out+dmn_idx);
	      /* Check-for and, if found, retain record dimension property */
	      for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		if(dmn_ids_in[dmn_idx] == dmn_ids_rec[dmn_rec_idx])
		  dmn_cnt_out[dmn_idx]=NC_UNLIMITED;
	      rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt_out[dmn_idx],dmn_ids_out+dmn_idx);
	    } /* !rcd */
	    if(flg_add_spc_crd){
	      /* Follow by spatial dimension(s) */ 
	      if(flg_grd_1D){
		dmn_ids_out[dmn_idx+dmn_in_fst]=dmn_id_col_out;
		dmn_cnt_out[dmn_idx+dmn_in_fst]=col_nbr;
	      } /* !flg_grd_1D */
	      if(flg_grd_2D){
		dmn_ids_out[dmn_idx+dmn_in_fst]=dmn_id_lat_out;
		dmn_cnt_out[dmn_idx+dmn_in_fst]=lat_nbr;
		dmn_in_fst++;
		dmn_nbr_out++;
		dmn_ids_out[dmn_idx+dmn_in_fst]=dmn_id_lon_out;
		dmn_cnt_out[dmn_idx+dmn_in_fst]=lon_nbr;
	      } /* !flg_grd_2D */
	    } /* !flg_add_spc_crd */
	  } /* !dmn_idx */
	}else{ /* !flg_rgr */
	  /* Replicate non-S1D variables */
	  (void)fprintf(stdout,"%s: DEBUG quark5\n",nco_prg_nm_get());
	  rcd=nco_inq_vardimid(in_id,var_id_in,dmn_ids_in);
	  for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	    rcd=nco_inq_dimname(in_id,dmn_ids_in[dmn_idx],dmn_nm);
	    rcd=nco_inq_dimid_flg(out_id,dmn_nm,dmn_ids_out+dmn_idx);
	    if(rcd != NC_NOERR){
	      rcd=nco_inq_dimlen(in_id,dmn_ids_in[dmn_idx],dmn_cnt_out+dmn_idx);
	      /* Check-for and, if found, retain record dimension property */
	      for(int dmn_rec_idx=0;dmn_rec_idx < dmn_nbr_rec;dmn_rec_idx++)
		if(dmn_ids_in[dmn_idx] == dmn_ids_rec[dmn_rec_idx])
		  dmn_cnt_out[dmn_idx]=NC_UNLIMITED;
	      rcd=nco_def_dim(out_id,dmn_nm,dmn_cnt_out[dmn_idx],dmn_ids_out+dmn_idx);
	    } /* !rcd */
	  } /* !dmn_idx */
	} /* !flg_rgr */
	(void)fprintf(stdout,"%s: DEBUG quark6 defining %s...\n",nco_prg_nm_get(),var_nm);
	rcd=nco_def_var(out_id,var_nm,var_typ,dmn_nbr_out,dmn_ids_out,&var_id_out);
	(void)fprintf(stdout,"%s: DEBUG quark7 defined %s\n",nco_prg_nm_get(),var_nm);
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
	(void)fprintf(stdout,"%s: DEBUG quark8\n",nco_prg_nm_get());
	nco_bool flg_add_msv_att; /* [flg] Extrapolation requires _FillValue */
	flg_add_msv_att=False;
	if(flg_add_msv_att && trv.flg_rgr){
	  has_mss_val=nco_mss_val_get_dbl(in_id,var_id_in,&mss_val_dbl);
	  if(!has_mss_val){
	    nco_bool flg_att_chg; /* [flg] _FillValue attribute was written */
	    aed_mtd_fll_val.var_nm=var_nm;
	    aed_mtd_fll_val.id=var_id_out;
	    aed_mtd_fll_val.type=var_typ;
	    if(var_typ == NC_FLOAT) aed_mtd_fll_val.val.fp=&mss_val_flt;
	    else if(var_typ == NC_DOUBLE) aed_mtd_fll_val.val.dp=&mss_val_dbl;
	    flg_att_chg=nco_aed_prc(out_id,var_id_out,aed_mtd_fll_val);
	    if(!flg_att_chg && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: WARNING %s reports unsuccessful attempt to create _FillValue attribute for variable %s\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  } /* !has_mss_val */
	} /* !flg_add_msv_att */
      } /* !rcd */
    } /* !var */
  } /* end idx_tbl */
  (void)fprintf(stdout,"%s: DEBUG quark9\n",nco_prg_nm_get());

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

  /* Free pre-allocated array space */
  if(dmn_ids_in) dmn_ids_in=(int *)nco_free(dmn_ids_in);
  if(dmn_ids_out) dmn_ids_out=(int *)nco_free(dmn_ids_out);
  if(dmn_ids_rec) dmn_ids_rec=(int *)nco_free(dmn_ids_rec);
  if(dmn_srt) dmn_srt=(long *)nco_free(dmn_srt);
  if(dmn_cnt_in) dmn_cnt_in=(long *)nco_free(dmn_cnt_in);
  if(dmn_cnt_out) dmn_cnt_out=(long *)nco_free(dmn_cnt_out);

  /* Unpack and copy data from input file */
  //int dmn_idx_col=int_CEWI; /* [idx] Index of column dimension */
  //int dmn_idx_lat=int_CEWI; /* [idx] Index of latitude dimension */
  //int dmn_idx_lon=int_CEWI; /* [idx] Index of longitude dimension */
  int thr_idx; /* [idx] Thread index */
  //int var_id; /* [id] Current variable ID */

  size_t var_sz_in; /* [nbr] Number of elements in variable (will be self-multiplied) */
  size_t var_sz_out; /* [nbr] Number of elements in variable (will be self-multiplied) */

  ptr_unn var_val_in;
  ptr_unn var_val_out;
  
  /* Using naked stdin/stdout/stderr in parallel region generates warning
     Copy appropriate filehandle to variable scoped as shared in parallel clause */
  FILE * const fp_stdout=stdout; /* [fl] stdout filehandle CEWI */

#ifdef __GNUG__
# pragma omp parallel for firstprivate(var_val_in,var_val_out) private(dmn_cnt_in,dmn_cnt_out,dmn_ids_in,dmn_ids_out,dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dmn_nm,dmn_srt,has_clm,has_grd,has_lnd,has_pft,has_mss_val,idx_out,idx_tbl,in_id,mss_val_dbl,rcd,thr_idx,trv,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ) shared(dmn_id_clm_in,dmn_id_clm_out,dmn_id_col_in,dmn_id_col_out,dmn_id_lat_in,dmn_id_lat_out,dmn_id_lnd_in,dmn_id_lnd_out,dmn_id_lon_in,dmn_id_lon_out,dmn_id_pft_in,dmn_id_pft_out,flg_s1d_clm,flg_s1d_pft,clm_nbr_in,clm_nbr_out,col_nbr,lat_nbr,lnd_nbr_in,lnd_nbr_out,lon_nbr,pft_nbr_in,pft_nbr_out,out_id,pfts1d_ixy,pfts1d_jxy)
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
	var_typ=trv.var_typ; /* NB: Output type in file is same as input type */
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
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	  rcd=nco_inq_dimlen(in_id,dmn_ids_in[dmn_idx],dmn_cnt_in+dmn_idx);
	  var_sz_in*=dmn_cnt_in[dmn_idx];
	  dmn_srt[dmn_idx]=0L;
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

	var_val_in.vp=(void *)nco_malloc_dbg(var_sz_in*nco_typ_lng(var_typ),fnc_nm,"Unable to malloc() input value buffer");
	var_val_out.vp=(void *)nco_malloc_dbg(var_sz_out*nco_typ_lng(var_typ),fnc_nm,"Unable to malloc() output value buffer");
	/* Initialize output */
	(void)memset(var_val_out.vp,0,var_sz_out*nco_typ_lng(var_typ));
	
	/* Obtain input variable */
	rcd=nco_get_vara(in_id,var_id_in,dmn_srt,dmn_cnt_in,var_val_in.vp,var_typ);

	has_clm=has_grd=has_lnd=has_pft=False;
	nco_s1d_typ=nco_s1d_nil;
	for(dmn_idx=0;dmn_idx<dmn_nbr_in;dmn_idx++){
	  dmn_nm_cp=trv.var_dmn[dmn_idx].dmn_nm;
	  if(!has_clm && clm_nm_in) has_clm=!strcmp(dmn_nm_cp,clm_nm_in);
	  if(!has_grd && grd_nm_in) has_grd=!strcmp(dmn_nm_cp,grd_nm_in);
	  if(!has_lnd && lnd_nm_in) has_lnd=!strcmp(dmn_nm_cp,lnd_nm_in);
	  if(!has_pft && pft_nm_in) has_pft=!strcmp(dmn_nm_cp,pft_nm_in);
	} /* !dmn_idx */
	if(has_clm) nco_s1d_typ=nco_s1d_clm;
	else if(has_grd) nco_s1d_typ=nco_s1d_grd;
	else if(has_lnd) nco_s1d_typ=nco_s1d_lnd;
	else if(has_pft) nco_s1d_typ=nco_s1d_pft;
	else{
	  (void)fprintf(stderr,"%s: ERROR %s reports variable %s does not appear to be sparse\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  nco_exit(EXIT_FAILURE);
	} /* !strstr() */
	if(nco_dbg_lvl_get() >= nco_dbg_std){
	  (void)fprintf(stderr,"%s: INFO %s reports variable %s is sparse type %s\n",nco_prg_nm_get(),fnc_nm,var_nm,nco_s1d_sng(nco_s1d_typ));
	} /* !dbg */

	/* The Hard Work */
	if(nco_s1d_typ == nco_s1d_pft){
	  /* Turn GPP(time,pft) into GPP(time,pft,lndgrid) */
	  for(pft_idx=0;pft_idx<pft_nbr_in;pft_idx++){
	    pft_typ=pfts1d_ityp_veg[pft_idx]; /* [1 <= pft_typ <= pft_nbr_out] */
	    /* Skip bare ground, output array contains only vegetated types */
	    if(!pft_typ) continue;
	    /* grd_idx is the index relative to the origin of the horizontal grid for a given level
	       [0 <= grd_idx_out <= col_nbr_out-1L], [1 <= pfts1d_ixy <= col_nbr_out] */
	    grd_idx_out= flg_grd_1D ? pfts1d_ixy[pft_idx]-1L : (pfts1d_ixy[pft_idx]-1L)*lat_nbr+(pfts1d_jxy[pft_idx]-1L);
	    idx_out=(pft_typ-1)*grd_sz_out+grd_idx_out;
	    /* memcpy() would allow next statement to work for generic types
	       However, memcpy() is a system call and could be expensive in an innermost loop */
	    switch(var_typ){
	    case NC_FLOAT: var_val_out.fp[idx_out]=var_val_in.fp[pft_idx]; break;
	    case NC_DOUBLE: var_val_out.dp[idx_out]=var_val_in.dp[pft_idx]; break;
	    case NC_INT: var_val_out.ip[idx_out]=var_val_in.ip[pft_idx]; break;
	    default:
	      (void)fprintf(fp_stdout,"%s: ERROR %s reports unsupported type\n",nco_prg_nm_get(),fnc_nm);
	      nco_dfl_case_nc_type_err();
	      break;
	    } /* !var_typ */
	  } /* !idx */
	} /* !nco_s1d_typ */
	  
#pragma omp critical
	{ /* begin OpenMP critical */
	  rcd=nco_put_vara(out_id,var_id_out,dmn_srt,dmn_cnt_out,var_val_out.vp,var_typ);
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
  if(cols1d_ityp) cols1d_ityp=(int *)nco_free(cols1d_ityp);
  if(cols1d_ityplun) cols1d_ityplun=(int *)nco_free(cols1d_ityplun);
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

  return rcd;
} /* !nco_s1d_unpack() */
