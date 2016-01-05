/* $Header$ */

/* Purpose: NCO utilities for Swath-Like Data (SLD) */

/* Copyright (C) 2015--2016 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

#include "nco_sld.h" /* Swath-Like Data */

int /* O [rcd] Return code */
nco_scrip_read /* [fnc] Read, parse, and print contents of SCRIP file */
(char *fl_scrip, /* SCRIP file name with proper path */
 kvm_sct *kvm_scrip)/* structure to hold contents of SCRIP file */ 
{
  char *sng_line;
  
  int icnt;
  int idx=0;

  FILE *fp_scrip;

  fp_scrip=fopen(fl_scrip,"r");

  if(!fp_scrip){
    (void)fprintf(stderr,"%s: ERROR Cannot open SCRIP file %s\n",nco_prg_nm_get(),fl_scrip);
    return NCO_ERR;
  } /* endif */

  sng_line=(char *)nco_malloc(BUFSIZ*sizeof(char));
  while(fgets(sng_line,sizeof(sng_line),fp_scrip)){
    /* Each line must contain "=" */
    if(!strstr(sng_line,"=")){
      (void)fprintf(stderr,"%s: ERROR Invalid line in SCRIP file: %s\n",nco_prg_nm_get(),sng_line);
      fclose(fp_scrip);
      return NCO_ERR;
    } /* endif */
    kvm_scrip[idx]=nco_sng2kvm(sng_line);
    if(!kvm_scrip[idx].key){
      fclose(fp_scrip);
      return NCO_ERR;
    }else{
      idx++;
    } /* end else */
  } /* finish parsing SCRIP file */
  fclose(fp_scrip);
  if(sng_line) sng_line=(char *)nco_free(sng_line);

  for(icnt=0;icnt<idx;icnt++) nco_kvm_prn(kvm_scrip[icnt]);

  return NCO_NOERR;
} /* end nco_scrip_read */

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

const char * /* O [sng] String version of ESMC_PoleKind_Flag enum */
nco_esmf_pll_knd_sng /* [fnc] Convert ESMF pole type enum to string */
(const int nco_esmf_pll_knd) /* I [enm] ESMF pole type enum ESMC_PoleKind_Flag */
{
  /* Purpose: Convert ESMF pole method enum ESMC_PoleKind_Flag to string */
  switch(nco_esmf_pll_knd){
  case ESMC_POLEKIND_NONE: return "ESMC_POLEKIND_NONE";
  case ESMC_POLEKIND_MONOPOLE: return "ESMC_POLEKIND_MONOPOLE";
  case ESMC_POLEKIND_BIPOLE: return "ESMC_POLEKIND_BIPOLE";
  default: abort(); break;
  } /* end switch */

  /* Some compilers: e.g., SGI cc, need return statement to end non-void functions */
  return (char *)NULL;
} /* end nco_esmf_pll_knd_sng() */

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
(rgr_sct * const rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields using ESMF library (actually ESMC interface to ESMF library)
     ESMC is C-interface to ESMF documented at
     http://www.earthsystemmodeling.org/esmf_releases/last_built/ESMC_crefdoc/ESMC_crefdoc.html
     ESMF Developer's Guide
     http://www.earthsystemmodeling.org/documents/dev_guide
     ESMF_RegridWeightGen
     http://www.earthsystemcog.org/projects/regridweightgen
     http://www.earthsystemmodeling.org/python_releases/last_esmpy/esmpy_doc/html/index.html

     ESMF C-interface examples:
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridUTest.C
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridCsrvUTest.C

     Sample call, T42->T42:
     ncks -O --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     ncks -O --rgr_var=LANDFRAC --rgr_grd_src=${DATA}/grids/T42.nc --rgr_grd_dst=${DATA}/grids/T62.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc
     ncks -O --rgr lat_nbr=180 --rgr_grd_src=${DATA}/grids/T62.nc --rgr_grd_dst=${DATA}/grids/T42.nc ${DATA}/dstmch90/dstmch90_clm.nc ~/foo.nc */
  
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
  in_id=rgr->in_id;
  out_id=rgr->out_id;
  
  /* Obtain input longitude type and length */
  char lon_nm_in[]="lon";
  (void)nco_inq_varid(in_id,lon_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr->fl_in);
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
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr->fl_in);
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
    /* 20150621: 
       Prevent clang sometimes uninitialized warnings by using abort() before goto
       All of the following "abort()" statements could be replace "goto rgr_cln:" statements
       gcc is fine with this, but it triggers clang "sometimes uninitialized" warnings
       g++ (without -fpermissive) emits "jump to label" "crosses initialization" warnings 
       when pointers are declared between the goto statment and the label statement 
       Hence we abandon the false premise of recovery after ESMC errors, and just abort() */
    abort();
  } /* endif */
  
  /* Set log to flush after every message */
  rcd_esmf=ESMC_LogSet(ESMF_TRUE);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* Obtain VM */
  ESMC_VM vm;
  vm=ESMC_VMGetGlobal(&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Set-up local PET info */
  /* Source: ${DATA}/esmf/src/Infrastructure/VM/interface/ESMC_VM.C */
  rcd_esmf=ESMC_VMGet(vm,&localPet,&petCount,&peCount,(MPI_Comm *)NULL,&flg_pthreadsEnabledFlag,&flg_openMPEnabledFlag);
  if(rcd_esmf != ESMF_SUCCESS) abort();

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
  grd_dst=ESMC_GridCreateFromFile(rgr->fl_grd_dst,grd_fl_typ,&flg_isSphere,&flg_addCornerStagger,&flg_addUserArea,&flg_addMask,var_nm,crd_nm,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int *msk_dst; /* [enm] Destination grid mask */
  enum ESMC_GridItem_Flag grd_itm=ESMC_GRIDITEM_MASK;
  msk_dst=(int *)ESMC_GridGetItem(grd_dst,grd_itm,stg_lcn_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int *bnd_lwr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_dst; /* [dgr] Destination grid longitude */
  double *lat_dst; /* [dgr] Destination grid latitude */
  /* 20150427: Written documentation is somewhat inadequate or misleading to normal C-programmers
     Some ESMC_Grid***() functions, like this one, return allocated void pointers that must be cast to desired numeric type
     Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  lon_dst=(double *)ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lon_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  lat_dst=(double *)ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lat_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

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
  if(rcd_esmf != ESMF_SUCCESS) abort();

  enum ESMC_CoordSys_Flag crd_sys=ESMC_COORDSYS_SPH_DEG; /* NB: dyw sez ESMF supports ESMC_COORDSYS_SPH_DEG only */
  enum ESMC_TypeKind_Flag typ_knd=ESMC_TYPEKIND_R8; /* NB: NCO default is to use double precision for coordinates */
  ESMC_Grid grd_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  grd_src=ESMC_GridCreateNoPeriDim(max_idx_src,&crd_sys,&typ_knd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* fxm: why destroy this now? */
  ESMC_InterfaceIntDestroy(&max_idx_src);
  /* Define stagger locations on source grid. Necessary for remapping later? */
  rcd_esmf=ESMC_GridAddCoord(grd_src,stg_lcn_src);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  
  /* Allocate */
  int *bnd_lwr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_src; /* [dgr] Source grid longitude */
  double *lat_src; /* [dgr] Source grid latitude  */
  lon_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lon_src,stg_lcn_src,bnd_lwr_src,bnd_upr_src,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  lat_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lat_src,stg_lcn_src,NULL,NULL,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"ESMC_VMGet(): localPet = %d, petCount = %d, peCount = %d, pthreadsEnabledFlag = %s, openMPEnabledFlag = %s\n",localPet,petCount,peCount,flg_pthreadsEnabledFlag ? "Enabled" : "Disabled",flg_openMPEnabledFlag ? "Enabled" : "Disabled");
    (void)fprintf(stderr,"ESMC_GridCreateFromFile(): filename = %s, fileTypeFlag = %s, isSphere = %s, addCornerStagger = %s, addUserArea = %s, addMask = %s, var_nm = %s, crd_nm = %s\n",rgr->fl_grd_dst,nco_esmf_fl_fmt_sng(grd_fl_typ),flg_isSphere ? "Enabled" : "Disabled",flg_addCornerStagger ? "Enabled" : "Disabled",flg_addUserArea ? "Enabled" : "Disabled",flg_addMask ? "Enabled" : "Disabled",var_nm,crd_nm ? "non-NULL" : "NULL");
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
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* Create destination field from destination grid */
  ESMC_Field fld_dst;
  fld_dst=ESMC_FieldCreateGridTypeKind(grd_dst,typ_knd,stg_lcn_dst,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_dst",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Get field pointers */
  double *fld_src_ptr;
  int localDe=0; /* [idx] Local DE for which information is requested [0..localDeCount-1] */
  fld_src_ptr=(double *)ESMC_FieldGetPtr(fld_src,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  double *fld_dst_ptr;
  fld_dst_ptr=(double *)ESMC_FieldGetPtr(fld_dst,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Get variables from input file */
  (void)nco_inq_varid(in_id,rgr->var_nm,&var_in_id);
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
  if(rcd_esmf != ESMF_SUCCESS) abort();

  enum ESMC_Region_Flag rgn_flg=ESMC_REGION_TOTAL; /* [flg] Whether/how to zero input fields before regridding (default ESMC_REGION_TOTAL) */
  rcd_esmf=ESMC_FieldRegrid(fld_src,fld_dst,rte_hnd,&rgn_flg);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Call once on each PET */
  rcd_esmf=ESMC_Finalize();
  if(rcd_esmf != ESMF_SUCCESS) abort();

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
  (void)nco_def_var(out_id,rgr->var_nm,var_typ_out,(int)2,dmn_ids_out,&var_out_id);
  (void)nco_enddef(out_id);
  dmn_srt_out[crd_idx_c_0bs_lat_dst]=0L;
  dmn_srt_out[crd_idx_c_0bs_lon_dst]=0L;
  dmn_cnt_out[crd_idx_c_0bs_lat_dst]=bnd_upr_dst[crd_idx_f_0bs_lat_dst];
  dmn_cnt_out[crd_idx_c_0bs_lon_dst]=bnd_upr_dst[crd_idx_f_0bs_lon_dst];
  (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_out,lat_out,crd_typ_out);
  (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_out,lon_out,crd_typ_out);
  (void)nco_put_vara(out_id,var_out_id,dmn_srt_out,dmn_cnt_out,fld_dst_ptr,var_typ_out);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(rgr->fl_out,rgr->fl_out_tmp,out_id);

  goto rgr_cln; /* Prevent 'rgr_cln' defined but not used warning */

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
} /* !nco_rgr_esmf() */

int /* O [enm] Return code */
nco_rgr_esmf2 /* [fnc] Regrid using ESMF library */
(rgr_sct * const rgr) /* I/O [sct] Regridding structure */
{
  /* Purpose: Regrid fields using ESMF library (actually ESMC interface to ESMF library)
     ESMC is C-interface to ESMF documented at
     http://www.earthsystemmodeling.org/esmf_releases/last_built/ESMC_crefdoc/ESMC_crefdoc.html
     ESMF Developer's Guide
     http://www.earthsystemmodeling.org/documents/dev_guide
     ESMF_RegridWeightGen
     http://www.earthsystemcog.org/projects/regridweightgen
     http://www.earthsystemmodeling.org/python_releases/last_esmpy/esmpy_doc/html/index.html

     ESMF C-interface examples:
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridUTest.C
     ${DATA}/esmf/src/Infrastructure/Field/tests/ESMC_FieldRegridCsrvUTest.C

     Sample call, T42->T42:
     ncks -O --rgr_grd_src=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_grd_dst=${DATA}/scrip/grids/remap_grid_T42.nc --rgr_out=${DATA}/rgr/rgr_out.nc ${DATA}/rgr/essgcm14_clm.nc ~/foo.nc */

  const char fnc_nm[]="nco_rgr_esmf2()"; /* [sng] Function name */
  const char fl_nm_esmf_log[]="nco_rgr_log_foo.txt"; /* [sng] Log file for ESMF routines */
  
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

  double *lon_in;
  double *lat_in;

  enum ESMC_StaggerLoc stg_lcn_src=ESMC_STAGGERLOC_CENTER; /* [enm] Stagger location of source grid */
  enum ESMC_StaggerLoc stg_lcn_dst=ESMC_STAGGERLOC_CENTER; /* [enm] Stagger location of destination grid */

  int *dmn_id;
  int dmn_nbr;
  int dmn_nbr_grd=2;
  int flg_openMPEnabledFlag; /* [flg] ESMF library was compiled with OpenMP enabled */
  int flg_pthreadsEnabledFlag; /* [flg] ESMF library was compiled with Pthreads enabled */
  int idx;
  int in_id; /* I [id] Input netCDF file ID */
  int localPet; /* [id] ID of PET that issued call */
  int out_id; /* I [id] Output netCDF file ID */
  int peCount; /* [nbr] Number of PEs referenced by ESMF_VM */
  int petCount; /* [nbr] Number of PETs referenced by ESMF_VM */
  int rcd=ESMF_SUCCESS;
  int rcd_esmf; /* [enm] Return codes from ESMF functions */
  int var_in_id;
  
  long *dmn_cnt;
  long *dmn_srt;
  
  nc_type var_typ_in;
  nc_type crd_typ_out=NC_DOUBLE;
  nc_type var_typ_out=NC_DOUBLE;
  
  void *void_ptr_var;
  
  /* Initialize */
  in_id=rgr->in_id;
  out_id=rgr->out_id;
  
  /* Obtain input longitude type and length */
  char lon_nm_in[]="lon";
  (void)nco_inq_varid(in_id,lon_nm_in,&var_in_id);
  (void)nco_inq_var(in_id,var_in_id,(char *)NULL,&var_typ_in,&dmn_nbr,(int *)NULL,(int *)NULL);
  if(var_typ_in != NC_DOUBLE){
    assert(var_typ_in == NC_FLOAT);
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr->fl_in);
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
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: INFO WARNING Cheapskate regridding input file %s stores coordinate as NC_FLOAT\n",nco_prg_nm_get(),rgr->fl_in);
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
    /* 20150621: 
       Prevent clang sometimes uninitialized warnings by using abort() before goto
       All of the following "abort()" statements could be replace "goto rgr_cln:" statements
       gcc is fine with this, but it triggers clang "sometimes uninitialized" warnings
       g++ (without -fpermissive) emits "jump to label" "crosses initialization" warnings 
       when pointers are declared between the goto statment and the label statement 
       Hence we abandon the false premise of recovery after ESMC errors, and just abort() */
    abort();
  } /* endif */
  
  /* Set log to flush after every message */
  rcd_esmf=ESMC_LogSet(ESMF_TRUE);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* Obtain VM */
  ESMC_VM vm;
  vm=ESMC_VMGetGlobal(&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Set-up local PET info */
  /* Source: ${DATA}/esmf/src/Infrastructure/VM/interface/ESMC_VM.C */
  rcd_esmf=ESMC_VMGet(vm,&localPet,&petCount,&peCount,(MPI_Comm *)NULL,&flg_pthreadsEnabledFlag,&flg_openMPEnabledFlag);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* 20150718: Create destination grid from user specifications */
  /* First create ESMC_InterfaceInt that describes grid size 
     Then use that ESMC_InterfaceInt to create ESMC_Grid
     ESMC_InterfaceInt stores grid indexing information for all dimensions */
  int *max_idx_dst; /* [nbr] Zero-based array containing dimension sizes in order? */
  int dmn_nbr_grd_dst=2;
  max_idx_dst=(int *)nco_malloc(dmn_nbr_grd_dst*sizeof(int));
  max_idx_dst[crd_idx_f_0bs_lon_src]=rgr->lat_nbr*rgr->lon_nbr; /* [nbr] Number of elements in dimensions */

  ESMC_InterfaceInt grd_ntf_dst;
  /* Source: ${DATA}/esmf/src/Infrastructure/Util/interface/ESMC_Interface.C
     NB: ESMF is fragile in that dynamic memory provided as input to grids cannot be immediately freed
     In other words, ESMF copies the values of pointers but not the contents of pointers to provided arrays
     To be concrete, the max_idx_dst array provided below cannot be freed until after the ESMC_Finalize() is called */
  grd_ntf_dst=ESMC_InterfaceIntCreate(max_idx_dst,dmn_nbr_grd_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int flg_isSphere=1; /* [flg] Set to 1 for spherical grid, or 0 for regional. Defaults to 1 (True). */
  int flg_addCornerStagger=0; /* [flg] Add corner stagger to grid. Defaults to 0 (False). */
  int flg_addUserArea=0; /* [flg] Read cell area from Grid file (instead of calculate it). Defaults to 0 (False). */
  int flg_addMask=0; /* [flg] Generate mask using missing value attribute in var_nm (iff GRIDSPEC) */
  /* 20150424: ESMF library bug at ESMCI_Grid.C line 365 means var_nm must non-NULL so set to blank name */
  char var_nm[]=""; /* [sng] Iff addMask == 1 use this variable's missing value attribute */
  char **crd_nm=NULL; /* [sng] Iff GRIDSPEC use these lon/lat variable coordinates */
  enum ESMC_FileFormat_Flag grd_fl_typ=ESMC_FILEFORMAT_SCRIP;
  enum ESMC_PoleKind_Flag poleKind[2]={ESMC_POLEKIND_MONOPOLE,ESMC_POLEKIND_MONOPOLE}; /* [enm] Pole type */
  enum ESMC_CoordSys_Flag crd_sys=ESMC_COORDSYS_SPH_DEG; /* NB: ESMF supports ESMC_COORDSYS_SPH_DEG only? */
  enum ESMC_TypeKind_Flag typ_knd=ESMC_TYPEKIND_R8; /* NB: NCO default is to use double precision for coordinates */
  ESMC_Grid grd_dst;
  // int periodicDim=crd_idx_f_1bs_lon_dst; /* [idx] Periodic dimension */
  // int poleDim=crd_idx_f_1bs_lat_dst; /* [idx] Pole dimension */
  grd_dst=ESMC_GridCreate1PeriDim(grd_ntf_dst,&crd_sys,&typ_knd,poleKind,&rcd_esmf); // API: /usr/local/include/ESMC_Grid.h
  //  grd_dst=ESMC_GridCreate1PeriDim(grd_ntf_dst,&periodicDim,&poleDim,&crd_sys,&typ_knd,&poleKind,&rcd_esmf); // API: http://www.earthsystemmodeling.org/esmf_releases/last_built/ESMC_crefdoc/node5.html#SECTION05055200000000000000
  //  grd_dst=ESMC_GridCreateFromFile(rgr->fl_grd_dst,grd_fl_typ,&flg_isSphere,&flg_addCornerStagger,&flg_addUserArea,&flg_addMask,var_nm,crd_nm,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int *msk_dst; /* [enm] Destination grid mask */
  enum ESMC_GridItem_Flag grd_itm=ESMC_GRIDITEM_MASK;
  msk_dst=(int *)ESMC_GridGetItem(grd_dst,grd_itm,stg_lcn_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  int *bnd_lwr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_dst=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_dst; /* [dgr] Destination grid longitude */
  double *lat_dst; /* [dgr] Destination grid latitude */
  /* 20150427: Written documentation is somewhat inadequate or misleading to normal C-programmers
     Some ESMC_Grid***() functions, like this one, return allocated void pointers that must be cast to desired numeric type
     Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  lon_dst=(double *)ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lon_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  lat_dst=(double *)ESMC_GridGetCoord(grd_dst,crd_idx_f_1bs_lat_dst,stg_lcn_dst,bnd_lwr_dst,bnd_upr_dst,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

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
  if(rcd_esmf != ESMF_SUCCESS) abort();

  ESMC_Grid grd_src;
  /* Source: ${DATA}/esmf/src/Infrastructure/Grid/interface/ESMC_Grid.C */
  grd_src=ESMC_GridCreateNoPeriDim(max_idx_src,&crd_sys,&typ_knd,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* fxm: why destroy this now? */
  ESMC_InterfaceIntDestroy(&max_idx_src);
  /* Define stagger locations on source grid. Necessary for remapping later? */
  rcd_esmf=ESMC_GridAddCoord(grd_src,stg_lcn_src);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  
  /* Allocate */
  int *bnd_lwr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  int *bnd_upr_src=(int *)nco_malloc(dmn_nbr_grd*sizeof(int));
  double *lon_src; /* [dgr] Source grid longitude */
  double *lat_src; /* [dgr] Source grid latitude  */
  lon_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lon_src,stg_lcn_src,bnd_lwr_src,bnd_upr_src,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  lat_src=(double *)ESMC_GridGetCoord(grd_src,crd_idx_f_1bs_lat_src,stg_lcn_src,NULL,NULL,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  if(nco_dbg_lvl_get() >= nco_dbg_crr){
    (void)fprintf(stderr,"%s: INFO %s reports\n",nco_prg_nm_get(),fnc_nm);
    (void)fprintf(stderr,"ESMC_VMGet(): localPet = %d, petCount = %d, peCount = %d, pthreadsEnabledFlag = %s, openMPEnabledFlag = %s\n",localPet,petCount,peCount,flg_pthreadsEnabledFlag ? "Enabled" : "Disabled",flg_openMPEnabledFlag ? "Enabled" : "Disabled");
    (void)fprintf(stderr,"ESMC_GridCreateFromFile(): filename = %s, fileTypeFlag = %s, isSphere = %s, addCornerStagger = %s, addUserArea = %s, addMask = %s, var_nm = %s, crd_nm = %s\n",rgr->fl_grd_dst,nco_esmf_fl_fmt_sng(grd_fl_typ),flg_isSphere ? "Enabled" : "Disabled",flg_addCornerStagger ? "Enabled" : "Disabled",flg_addUserArea ? "Enabled" : "Disabled",flg_addMask ? "Enabled" : "Disabled",var_nm,crd_nm ? "non-NULL" : "NULL");
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
  if(rcd_esmf != ESMF_SUCCESS) abort();
  /* Create destination field from destination grid */
  ESMC_Field fld_dst;
  fld_dst=ESMC_FieldCreateGridTypeKind(grd_dst,typ_knd,stg_lcn_dst,gridToFieldMap,ungriddedLBound,ungriddedUBound,"fld_dst",&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Get field pointers */
  double *fld_src_ptr;
  int localDe=0; /* [idx] Local DE for which information is requested [0..localDeCount-1] */
  fld_src_ptr=(double *)ESMC_FieldGetPtr(fld_src,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();
  double *fld_dst_ptr;
  fld_dst_ptr=(double *)ESMC_FieldGetPtr(fld_dst,localDe,&rcd_esmf);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Get variables from input file */
  (void)nco_inq_varid(in_id,rgr->var_nm,&var_in_id);
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

  ESMC_LogWrite("nco_rgr_esmf2() invoking ESMC to start regridstore actions",log_msg_typ);
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
  if(rcd_esmf != ESMF_SUCCESS) abort();

  enum ESMC_Region_Flag rgn_flg=ESMC_REGION_TOTAL; /* [flg] Whether/how to zero input fields before regridding (default ESMC_REGION_TOTAL) */
  rcd_esmf=ESMC_FieldRegrid(fld_src,fld_dst,rte_hnd,&rgn_flg);
  if(rcd_esmf != ESMF_SUCCESS) abort();

  /* Call once on each PET */
  rcd_esmf=ESMC_Finalize();
  if(rcd_esmf != ESMF_SUCCESS) abort();

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
  (void)nco_def_var(out_id,rgr->var_nm,var_typ_out,(int)2,dmn_ids_out,&var_out_id);
  (void)nco_enddef(out_id);
  dmn_srt_out[crd_idx_c_0bs_lat_dst]=0L;
  dmn_srt_out[crd_idx_c_0bs_lon_dst]=0L;
  dmn_cnt_out[crd_idx_c_0bs_lat_dst]=bnd_upr_dst[crd_idx_f_0bs_lat_dst];
  dmn_cnt_out[crd_idx_c_0bs_lon_dst]=bnd_upr_dst[crd_idx_f_0bs_lon_dst];
  (void)nco_put_vara(out_id,lat_out_id,dmn_srt_out,dmn_cnt_out,lat_out,crd_typ_out);
  (void)nco_put_vara(out_id,lon_out_id,dmn_srt_out,dmn_cnt_out,lon_out,crd_typ_out);
  (void)nco_put_vara(out_id,var_out_id,dmn_srt_out,dmn_cnt_out,fld_dst_ptr,var_typ_out);

  /* Close output file and move it from temporary to permanent location */
  (void)nco_fl_out_cls(rgr->fl_out,rgr->fl_out_tmp,out_id);

  goto rgr_cln; /* Prevent 'rgr_cln' defined but not used warning */

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
} /* !nco_rgr_esmf2() */
#endif /* !ENABLE_ESMF */
