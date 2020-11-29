/* $Header$ */

/* Purpose: NCO utilities for Sparse-1D (S1D) datasets */

/* Copyright (C) 2015--present Charlie Zender
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
     ncks -O -C --s1d -v cols1d_topoglc ~/data/bm/elm_mali_rst.nc ~/foo.nc
     ncks -O -C --s1d -v cols1d_topoglc ~/data/bm/elm_mali_rst.nc ~/foo.nc */

  const char fnc_nm[]="nco_s1d_unpack()"; /* [sng] Function name */

  char var_nm[NC_MAX_NAME+1L];
  char *fl_in;
  char *fl_out;

  int dfl_lvl=NCO_DFL_LVL_UNDEFINED; /* [enm] Deflate level */
  int fl_out_fmt=NCO_FORMAT_UNDEFINED; /* [enm] Output file format */
  int fll_md_old; /* [enm] Old fill mode */
  int in_id; /* I [id] Input netCDF file ID */
  int md_open; /* [enm] Mode flag for nc_open() call */
  int out_id; /* I [id] Output netCDF file ID */
  int rcd=NC_NOERR;

  int dmn_idx; /* [idx] Dimension index */

  nco_bool flg_grd_in_1D=False;
  nco_bool flg_grd_in_2D=False;
  nco_bool flg_grd_out_1D=False;
  nco_bool flg_grd_out_2D=False;

  int cols1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of column */
  int cols1d_ixy_id=NC_MIN_INT; /* [id] Column 2D longitude index */
  int cols1d_jxy_id=NC_MIN_INT; /* [id] Column 2D latitude index */
  int cols1d_lat_id=NC_MIN_INT; /* [id] Column latitude */
  int cols1d_lon_id=NC_MIN_INT; /* [id] Column longitude */

  int grid1d_ixy_id=NC_MIN_INT; /* [id] Gridcell 2D longitude index */
  int grid1d_jxy_id=NC_MIN_INT; /* [id] Gridcell 2D latitude index */
  int grid1d_lat_id=NC_MIN_INT; /* [id] Gridcell latitude */
  int grid1d_lon_id=NC_MIN_INT; /* [id] Gridcell longitude */

  int land1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of landunit */
  int land1d_ixy_id=NC_MIN_INT; /* [id] Landunit 2D longitude index */
  int land1d_jxy_id=NC_MIN_INT; /* [id] Landunit 2D latitude index */
  int land1d_lat_id=NC_MIN_INT; /* [id] Landunit latitude */
  int land1d_lon_id=NC_MIN_INT; /* [id] Landunit longitude */

  int pfts1d_gridcell_index_id=NC_MIN_INT; /* [id] Gridcell index of PFT */
  int pfts1d_column_index_id=NC_MIN_INT; /* [id] Column index of PFT */
  int pfts1d_ixy_id=NC_MIN_INT; /* [id] PFT 2D longitude index */
  int pfts1d_jxy_id=NC_MIN_INT; /* [id] PFT 2D latitude index */
  int pfts1d_lat_id=NC_MIN_INT; /* [id] PFT latitude */
  int pfts1d_lon_id=NC_MIN_INT; /* [id] PFT longitude */
  
  int dmn_id_gridcell=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_landunit=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_column=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_pft=NC_MIN_INT; /* [id] Dimension ID */

  char dmn_nm[NC_MAX_NAME]; /* [sng] Dimension name */
  char gridcell_nm[]="gridcell";
  char landunit_nm[]="landunit";
  char column_nm[]="column";
  char pft_nm[]="pft";

  nco_bool flg_s1d_col=False; /* [flg] Dataset contains sparse variables for columns */
  nco_bool flg_s1d_grd=False; /* [flg] Dataset contains sparse variables for gridcells */
  nco_bool flg_s1d_lnd=False; /* [flg] Dataset contains sparse variables for landunits */
  nco_bool flg_s1d_pft=False; /* [flg] Dataset contains sparse variables for PFTs */

  /* Initialize local copies of command-line values */
  dfl_lvl=rgr->dfl_lvl;
  fl_in=rgr->fl_in;
  fl_out=rgr->fl_out;
  in_id=rgr->in_id;
  out_id=rgr->out_id;

  /* Sanity check that input data file matches expectations from mapfile */
  char *col_nm_in=rgr->col_nm_in; /* [sng] Name to recognize as input horizontal spatial dimension on unstructured grid */
  char *lat_nm_in=rgr->lat_nm_in; /* [sng] Name of input dimension to recognize as latitude */
  char *lon_nm_in=rgr->lon_nm_in; /* [sng] Name of input dimension to recognize as longitude */
  int dmn_id_col=NC_MIN_INT; /* [id] Dimension ID */
  int dmn_id_lat; /* [id] Dimension ID */
  int dmn_id_lon; /* [id] Dimension ID */

  char *fl_tpl; /* [sng] Template file (horizontal grid file) */
  char *fl_pth_lcl=NULL;
  int tpl_id; /* [id] Input netCDF file ID (for horizontal grid template) */
  
  if(col_nm_in && (rcd=nco_inq_dimid_flg(in_id,col_nm_in,&dmn_id_col)) == NC_NOERR) /* do nothing */; 
  else if((rcd=nco_inq_dimid_flg(in_id,"lndgrid",&dmn_id_col)) == NC_NOERR) col_nm_in=strdup("lndgrid"); /* CLM */

  rcd=nco_inq_varid_flg(in_id,"cols1d_gridcell_index",&cols1d_gridcell_index_id);
  if(cols1d_gridcell_index_id != NC_MIN_INT) flg_s1d_col=True;
  if(flg_s1d_col){
    rcd=nco_inq_varid(in_id,"cols1d_ixy",&cols1d_ixy_id);
    rcd=nco_inq_varid(in_id,"cols1d_jxy",&cols1d_jxy_id);
    rcd=nco_inq_varid(in_id,"cols1d_lat",&cols1d_lat_id);
    rcd=nco_inq_varid(in_id,"cols1d_lon",&cols1d_lon_id);
  } /* !flg_s1d_col */
     
  rcd=nco_inq_varid_flg(in_id,"grid1d_ixy",&grid1d_ixy_id);
  if(grid1d_ixy_id != NC_MIN_INT) flg_s1d_grd=True;
  if(flg_s1d_grd){
    rcd=nco_inq_varid(in_id,"grid1d_jxy",&grid1d_jxy_id);
    rcd=nco_inq_varid(in_id,"grid1d_lat",&grid1d_lat_id);
    rcd=nco_inq_varid(in_id,"grid1d_lon",&grid1d_lon_id);
  } /* !flg_s1d_grd */
     
  rcd=nco_inq_varid_flg(in_id,"land1d_gridcell_index",&land1d_gridcell_index_id);
  if(land1d_gridcell_index_id != NC_MIN_INT) flg_s1d_lnd=True;
  if(flg_s1d_lnd){
    rcd=nco_inq_varid(in_id,"land1d_ixy",&land1d_ixy_id);
    rcd=nco_inq_varid(in_id,"land1d_jxy",&land1d_jxy_id);
    rcd=nco_inq_varid(in_id,"land1d_lat",&land1d_lat_id);
    rcd=nco_inq_varid(in_id,"land1d_lon",&land1d_lon_id);
  } /* !flg_s1d_lnd */
     
  rcd=nco_inq_varid_flg(in_id,"pfts1d_gridcell_index",&pfts1d_gridcell_index_id);
  if(pfts1d_gridcell_index_id != NC_MIN_INT) flg_s1d_pft=True;
  if(flg_s1d_pft){
    rcd=nco_inq_varid(in_id,"pfts1d_column_index",&pfts1d_column_index_id);
    rcd=nco_inq_varid(in_id,"pfts1d_ixy",&pfts1d_ixy_id);
    rcd=nco_inq_varid(in_id,"pfts1d_jxy",&pfts1d_jxy_id);
    rcd=nco_inq_varid(in_id,"pfts1d_lat",&pfts1d_lat_id);
    rcd=nco_inq_varid(in_id,"pfts1d_lon",&pfts1d_lon_id);
  } /* !flg_s1d_pft */
  
  assert(flg_s1d_col || flg_s1d_pft);

  rcd=nco_inq_dimid(in_id,gridcell_nm,&dmn_id_gridcell);
  rcd=nco_inq_dimid(in_id,landunit_nm,&dmn_id_landunit);
  rcd=nco_inq_dimid(in_id,column_nm,&dmn_id_column);
  rcd=nco_inq_dimid(in_id,pft_nm,&dmn_id_pft);

  if(flg_s1d_col && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Found all necessary information for unpacking cols1d variables\n",nco_prg_nm_get());
  if(flg_s1d_pft && nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO Found all necessary information for unpacking pfts1d variables\n",nco_prg_nm_get());

#ifdef ENABLE_S1D

  int dmn_idx_col=int_CEWI; /* [idx] Index of column dimension */
  int dmn_idx_lat=int_CEWI; /* [idx] Index of latitude dimension */
  int dmn_idx_lon=int_CEWI; /* [idx] Index of longitude dimension */
  int dmn_id_col; /* [id] Column dimension ID */
  int dmn_id_lon; /* [id] Longitude dimension ID */
  int dmn_id_lat; /* [id] Latitude dimension ID */
  int var_id; /* [id] Current variable ID */

  const int dmn_nbr_max=3; /* [nbr] Maximum number of dimensions input or output variables */

  int dmn_ids[dmn_nbr_max]; /* [id] Dimension IDs array for output variable */

  long dmn_srt[dmn_nbr_max];
  long dmn_cnt[dmn_nbr_max];

  long col_nbr; /* [nbr] Number of columns */
  long lon_nbr; /* [nbr] Number of longitudes */
  long lat_nbr; /* [nbr] Number of latitudes */
  long var_sz; /* [nbr] Size of variable */

  nc_type var_typ_in; /* [enm] NetCDF type of input data */
  nc_type var_typ_out; /* [enm] NetCDF type of data in output file */

  nco_s1d_typ_enm nco_s1d_typ; /* [enm] Sparse-1D type of input variable */

  ptr_unn var_val_in;
  ptr_unn var_val_out;
  
#ifdef __GNUG__
# pragma omp parallel for firstprivate(has_column,has_pft,var_val_in,var_val_out) private(dmn_cnt_in,dmn_cnt_out,dmn_id_in,dmn_id_out,dmn_idx,dmn_nbr_in,dmn_nbr_out,dmn_nbr_max,dmn_nm,dmn_srt,grd_idx,has_mss_val,idx_in,idx_out,idx_tbl,in_id,lvl_idx_in,lvl_idx_out,lvl_nbr_in,lvl_nbr_out,mss_val_dbl,rcd,thr_idx,trv,var_id_in,var_id_out,var_nm,var_sz_in,var_sz_out,var_typ_out,var_typ_rgr) shared(dmn_id_column_in,dmn_id_column_out,dmn_id_pft_in,dmn_id_pft_out,dmn_id_tm_in,flg_s1d_col,flg_s1d_pft,grd_nbr,idx_dbg,column_nbr_in,column_nbr_out,pft_nbr_in,pft_nbr_out,out_id,xtr_mth)
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
	/* Interpolate variable */
	
	if(strstr("cols1d",var_nm)){
	  nco_s1d_typ=nco_s1d_clm;
	}else if(strstr("pfts1d",var_nm)){
	  nco_s1d_typ=nco_s1d_pft;
	}else{
	  (void)fprintf(stderr,"%s: ERROR %s reports variable %s does not appear to be sparse\n",nco_prg_nm_get(),fnc_nm,var_nm);
	  nco_exit(EXIT_FAILURE);
	} /* !strstr() */

	if(nco_dbg_lvl_get() >= nco_dbg_std){
	  (void)fprintf(stderr,"%s: INFO %s reports variable %s is sparse type %s",nco_prg_nm_get(),fnc_nm,var_nm,nco_s1d_sng(nco_s1d_typ));
	} /* !dbg */
	
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
#endif /* !ENABLE_S1D */

  return rcd;
} /* !nco_s1d_unpack() */
