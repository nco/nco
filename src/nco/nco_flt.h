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
# include <netcdf_meta.h> /* NC_VERSION_..., NC_HAS_QUANTIZE, NC_HAS_ZSTD */
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
#include "nco_var_utl.h" /* Variable utilities */

#if ENABLE_CCR
# include <ccr.h> /* Community Codec Repository prototypes */
# include <ccr_meta.h> /* CCR_VERSION, CCR_HAS_BITGROOM, ... */
#endif /* !ENABLE_CCR */

/* Quantization tokens introduced in netcdf.h 4.9.0 ~202206 */
#ifndef NC_NOQUANTIZE
# define NC_NOQUANTIZE 0 /**< No quantization in use. */    
#endif
#ifndef NC_QUANTIZE_BITGROOM
# define NC_QUANTIZE_BITGROOM 1 /**< Use BitGroom quantization. */
#endif
#ifndef NC_QUANTIZE_GRANULARBR
# define NC_QUANTIZE_GRANULARBR 2 /**< Use Granular BitRound quantization. */
#endif
#ifndef NC_QUANTIZE_BITROUND
# define NC_QUANTIZE_BITROUND 3 /**< Use BitRound quantization. */
#endif

/* Filter tokens introduced in netcdf_filter.h 4.9.0 ~202206 */
/* Must match values in <H5Zpublic.h> */
#ifndef H5Z_FILTER_DEFLATE
# define H5Z_FILTER_DEFLATE 1
#endif
#ifndef H5Z_FILTER_SHUFFLE
# define H5Z_FILTER_SHUFFLE 2
#endif
#ifndef H5Z_FILTER_FLETCHER32
# define H5Z_FILTER_FLETCHER32 3
#endif
#ifndef H5Z_FILTER_SZIP
# define H5Z_FILTER_SZIP 4
#endif
/* Other Standard Filters */
#ifndef H5Z_FILTER_ZSTD
# define H5Z_FILTER_ZSTD 32015
#endif
#ifndef H5Z_FILTER_BZIP2
# define H5Z_FILTER_BZIP2 307
#endif
#ifndef H5Z_FILTER_BLOSC
# define H5Z_FILTER_BLOSC 32001
#endif
/* 20220617 My understanding of this is not complete
   Thus this compatibility code is not optimal
   All it needs to do is define some tokens so nco_flt.c will build
   It does not need to defines the enums themselves */
# ifndef BLOSC_LZ
#  define BLOSC_LZ 0
#  define BLOSC_LZ4 1
#  define BLOSC_LZ4HC 2
#  define BLOSC_SNAPPY 3
#  define BLOSC_ZLIB 4
#  define BLOSC_ZSTD 5
# endif

/* NCO-specific filter tokens */
#ifndef NCO_FLT_SNG_LNG_MAX
#define NCO_FLT_SNG_LNG_MAX 200L
#endif /* !NCO_FLT_SNG_LNG_MAX */

/* Filter types that NCO knows internally: 
   Convert filter enum to string in nco_flt_enm2sng()
   Convert string to filter enum in nco_flt_sng2enmid() */
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
  nco_flt_bls=9, /* 9 [enm] BLOSC */
  nco_flt_bls_lz=10, /* 10 [enm] BLOSC LZ */
  nco_flt_bls_lz4=11, /* 11 [enm] BLOSC LZ4 */
  nco_flt_bls_lzh=12, /* 12 [enm] BLOSC LZ4 HC */
  nco_flt_bls_snp=13, /* 13 [enm] BLOSC Snappy */
  nco_flt_bls_dfl=14, /* 14 [enm] BLOSC DEFLATE */
  nco_flt_bls_zst=15, /* 15 [enm] BLOSC Zstandard */
  nco_flt_shf=16, /* 16 [enm] Shuffle */
  nco_flt_f32=17, /* 17 [enm] Fletcher32 */
  nco_flt_szp=18, /* 18 [enm] Szip */
  nco_flt_unk=19, /* 19 [enm] Unknown filter (reference by ID not name) */
} nco_flt_typ_enm; /* end nco_flt_typ_enm */

/* Filter flags */
typedef enum nco_flt_flg_enm{ /* [enm] Filter policies */
  nco_flt_flg_nil=0, /* 0 [enm] Filter flag is unset */
  nco_flt_flg_all_ok=1, /* 1 [enm] Any filter is permissible to apply to this variable */
  nco_flt_flg_prc_fll=2, /* 2 [enm] Preserve full precision for this variable (do not apply lossy filters, e.g., for coordinate-like variables) */
  nco_flt_flg_qnt_no=3, /* 3 [enm] Quantization filters not allowed for this variable (e.g., variable is integer or string-valued) */
} nco_flt_flg_enm; /* end nco_flt_flg_enm */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

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

void
nco_dfl_case_flt_enm_err /* [fnc] Print error and exit for illegal switch(nco_flt_enm) case */
(nco_flt_typ_enm nco_flt_enm, /* [enm] Unrecognized enum */
 char *fnc_err); /* [sng] Function where error occurred */

int /* O [enm] Return code */
nco_cmp_prs /* [fnc] Parse user-provided compression specification */
(char * const cmp_sng, /* I/O [sng] Compression specification */
 int * const dfl_lvl, /* I [enm] Deflate level [0..9] */
 int *flt_nbrp, /* [nbr] Number of codecs specified */
 nco_flt_typ_enm **flt_algp, /* [nbr] List of filters specified */
 unsigned int **flt_idp, /* [ID] List of HDF5 filter IDs */
 int **flt_lvlp, /* [nbr] List of compression levels for each filter */
 int **flt_prm_nbrp, /* [nbr] List of parameter numbers for each filter */
 int ***flt_prmp); /* [nbr] List of lists of parameters for each filter */

void
nco_flt_hdf5_prs /* [fnc] Parse user-provided filter string */
(char * const flt_sng); /* I [sng] User-provided filter string */
 // const unsigned int *flt_id, /* O [enm] Compression filter ID */
 // const nco_flt_typ_enm *nco_flt_typ); /* O [enm] Compression filter type */
  
char * /* O [sng] String describing compression filter */
nco_flt_enm2nmid /* [fnc] Convert compression filter enum to string */
(const nco_flt_typ_enm nco_flt_enm, /* I [enm] Compression filter type */
 unsigned int * const flt_idp); /* O [ID] HDF5 filter ID */

char * /* O [sng] String describing compression filter */
nco_flt_id2nm /* [fnc] Convert compression filter ID to name */
(const unsigned int nco_flt_id); /* I [id] Compression filter ID */

nco_flt_typ_enm /* O [enm] Filter enum */
nco_flt_id2enm /* [fnc] Convert HDF5 compression filter ID to enum */
(const unsigned int flt_id); /* I [id] Compression filter ID */

nco_flt_typ_enm /* O [enm] Filter enum */
nco_flt_nm2enmid /* [fnc] Convert user-specified filter name to NCO enum */
(const char * const flt_nm, /* I [sng] User-specified filter name */
 unsigned int * const flt_idp);  /* O [ID] HDF5 filter ID */

int /* O [enm] Return code */
nco_flt_old_wrp /* [fnc] Define output filters based on input filters */
(const int nc_in_id, /* I [id] netCDF input file/group ID */
 const int var_in_id, /* I [id] Variable ID */
 const char * const var_nm_in, /* I [nm] Variable name [optional] */
 const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl); /* I [enm] Deflate level [0..9] */

int /* O [enm] Return code */
nco_flt_old_out /* [fnc] Define filters based on requested  */
(const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const int dfl_lvl); /* I [enm] Deflate level [0..9] */

int /* O [enm] Return code */
nco_flt_def_wrp /* [fnc] Define output filters based on input filters */
(const int nc_in_id, /* I [id] netCDF input file/group ID */
 const int var_in_id, /* I [id] Variable ID */
 const char * const var_nm_in, /* I [nm] Variable name [optional] */
 const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id); /* I [id] Variable ID */

int /* O [enm] Return code */
nco_flt_def_out /* [fnc] Define filters based on requested  */
(const int nc_out_id, /* I [id] netCDF output file/group ID */
 const int var_out_id, /* I [id] Variable ID */
 const char * const cmp_sng, /* I/O [sng] Compression specification */
 const nco_flt_flg_enm flt_flg); /* I [enm] Enumerated flags for fine-grained compression control */

int /* O [enm] Return code */
nco_inq_var_blk_sz
(const int nc_id, /* I [id] netCDF output file/group ID */
 const int var_id, /* I [id] Variable ID */
 unsigned int * const blk_szp); /* O [B] Block size in bytes */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_FLT_H */
