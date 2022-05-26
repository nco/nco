/* $Header$ */

/* Purpose: Description (definition) of compression filter functions */

/* Copyright (C) 2020--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_flt.h" *//* Compression filters */

#ifndef NCO_FLT_H
#define NCO_FLT_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#ifdef NC_HAVE_META_H
# include <netcdf_meta.h> /* NC_VERSION_..., HAVE_NC_RENAME_GRP */	 
#endif /* !NC_HAVE_META_H */
#ifndef NC_LIB_VERSION
# define NC_LIB_VERSION ( NC_VERSION_MAJOR * 100 + NC_VERSION_MINOR * 10 + NC_VERSION_PATCH )
#endif /* !NC_LIB_VERSION */
#if NC_LIB_VERSION >= 474
# include <netcdf_filter.h> /* netCDF filter definitions */
#endif /* !4.7.4 */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */

#if ENABLE_CCR
# include <ccr.h> /* Community Codec Repository prototypes */
# include <ccr_meta.h> /* CCR_VERSION, CCR_HAS_BITGROOM, ... */
#endif /* !ENABLE_CCR */

/* Filters types that NCO knows internally: 
   Convert string to filter enum in nco_flt_typ_set() */
typedef enum nco_flt_typ_enm{ /* [enm] Chunking policy */
  nco_flt_nil=0, /* 0 [enm] Filter type is unset */
  nco_flt_dfl=1, /* 1 [enm] DEFLATE */
  nco_flt_bz2=2, /* 2 [enm] Bzip2 */
  nco_flt_lz4=3, /* 3 [enm] LZ4 */
  nco_flt_zst=4, /* 4 [enm] Zstandard */
  nco_flt_bgr=5, /* 5 [enm] BitGroom */
  nco_flt_gbr=6, /* 6 [enm] Granular BitRound */
  nco_flt_dgr=7, /* 7 [enm] DigitRound */
  nco_flt_btr=8, /* 8 [enm] BitRound */
  nco_flt_bls_lz=10, /* 10 [enm] BLOSC LZ */
  nco_flt_bls_lz4=11, /* 11 [enm] BLOSC LZ4 */
  nco_flt_bls_lzh=12, /* 12 [enm] BLOSC LZ4 HC */
  nco_flt_bls_snp=13, /* 13 [enm] BLOSC Snappy */
  nco_flt_bls_dfl=14, /* 14 [enm] BLOSC DEFLATE */
  nco_flt_bls_zst=15, /* 15 [enm] BLOSC Zstandard */
} nco_flt_typ_enm; /* end nco_flt_typ_enm */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Manipulate private compression algorithm parameters through public interfaces */
nco_flt_typ_enm nco_flt_glb_lsl_alg_get(void); /* [enm] Lossless enum */
nco_flt_typ_enm nco_flt_glb_lsy_alg_get(void); /* [enm] Lossy enum */
int nco_flt_glb_lsl_lvl_get(void); /* [enm] Lossless level */
int nco_flt_glb_lsy_lvl_get(void); /* [enm] Lossy level */
void nco_flt_glb_lsl_alg_set(nco_flt_typ_enm nco_flt_lsl_alg); 
void nco_flt_glb_lsy_alg_set(nco_flt_typ_enm nco_flt_lsy_alg); 
void nco_flt_glb_lsl_lvl_set(int nco_flt_lsl_lvl); 
void nco_flt_glb_lsy_lvl_set(int nco_flt_lsy_lvl); 

#if !defined(CCR_HAS_BZIP2) && (NC_LIB_VERSION < 490)
int nc_def_var_bzip2(int ncid, int varid, int level);
int nc_inq_var_bzip2(int ncid, int varid, int *hasfilterp, int *levelp);
#endif /* !CCR_HAS_BZIP2, !490, !4.9.0 */

#if !defined(CCR_HAS_ZSTD) && (NC_LIB_VERSION < 490)
int nc_def_var_zstandard(int ncid, int varid, int level);
int nc_inq_var_zstandard(int ncid, int varid, int *hasfilterp, int *levelp);
#endif /* !CCR_HAS_ZSTD, !490, !4.9.0 */

#if !defined(CCR_HAS_BLOSC) && (NC_LIB_VERSION < 490)
int nc_def_var_blosc(int ncid, int varid, unsigned subcompressor, unsigned level, unsigned blocksize, unsigned addshuffle);
int nc_inq_var_blosc(int ncid, int varid, int *hasfilterp, unsigned *subcompressorp, unsigned *levelp, unsigned *blocksizep, unsigned *addshufflep);
#endif /* !CCR_HAS_BLOSC, !490, !4.9.0 */

int /* O [enm] Return code */
nco_cmp_prs /* [fnc] Parse user-provided compression specification */
(char * const cmp_sng, /* I [sng] Compression specification */
 const int dfl_lvl); /* I [enm] Deflate level [0..9] */

void
nco_flt_hdf5_prs /* [fnc] Parse user-provided filter string */
(char * const flt_sng); /* I [sng] User-provided filter string */
 // const unsigned int *flt_id, /* O [enm] Compression filter ID */
 // const nco_flt_typ_enm *nco_flt_typ); /* O [enm] Compression filter type */
  
char * /* O [sng] Filter string */
nco_flt_enm2sng /* [fnc] Convert compression filter enum to string */
(const nco_flt_typ_enm nco_flt_typ); /* I [enm] Compression filter type */

char * /* O [sng] String describing compression filter */
nco_flt_id2sng /* [fnc] Convert compression filter ID to string */
(const unsigned int nco_flt_id); /* I [id] Compression filter ID */

  nco_flt_typ_enm /* O [enm] Filter enum */
nco_flt_sng2enm /* [fnc] Convert user-specified filter string to NCO enum */
(const char *nco_flt_sng); /* [sng] User-specified filter string */

int /* O [enm] Return code */
nco_flt_def_wrp /* [fnc] Define output filters based on input filters */
(const int nc_in_id, /* I [id] netCDF input file/group ID */
 const int var_in_id, /* I [id] Variable ID */
 const char * const var_nm_in, /* I [nm] Variable name [optional] */
 const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl); /* I [enm] Deflate level [0..9] */

int /* O [enm] Return code */
nco_flt_def_out /* [fnc] Define filters based on requested  */
(const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl); /* I [enm] Deflate level [0..9] */

int /* O [enm] Return code */
nco_tst_def_wrp /* [fnc] Define output filters based on input filters */
(const int nc_in_id, /* I [id] netCDF input file/group ID */
 const int var_in_id, /* I [id] Variable ID */
 const char * const var_nm_in, /* I [nm] Variable name [optional] */
 const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl); /* I [enm] Deflate level [0..9] */

int /* O [enm] Return code */
nco_tst_def_out /* [fnc] Define filters based on requested  */
(const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl); /* I [enm] Deflate level [0..9] */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_FLT_H */
