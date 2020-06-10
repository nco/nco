/* $Header$ */

/* Purpose: File manipulation */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_fl_utl.h" *//* File manipulation */

#ifndef NCO_FL_UTL_H
#define NCO_FL_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <errno.h> /* system/library error diagnostics, errno */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */
#ifdef HAVE_STRINGS_H
# include <strings.h> /* strcasecmp() */
#endif /* !HAVE_STRINGS_H */
#include <sys/stat.h> /* stat() */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
# include <netinet/in.h> /* needed for _res */
# include <pwd.h> /* password structures for getpwuid() */
#endif /* !_MSC_VER */
#ifdef _MSC_VER
# include <process.h> /* MSVC getpid() */
# include <windows.h> /* MSVC Sleep() */
typedef int pid_t;
#endif /* !_MSC_VER */

#ifndef WIN32
# include <arpa/nameser.h> /* needed for _res */
# include <resolv.h> /* Internet structures for _res */
#endif /* !WIN32 */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_grp_trv.h" /* Group traversal */

/* 20120301: Replace multiple instances of hard-coded retry limit with CPP token NCO_MAX_NBR_USR_INPUT_RETRY */
#ifndef NCO_MAX_NBR_USR_INPUT_RETRY
# define NCO_MAX_NBR_USR_INPUT_RETRY 10
#endif /* NCO_MAX_NBR_USR_INPUT_RETRY */

/* 20120301: Replace multiple instances of hard-coded reply length limit with CPP token NCO_USR_RPL_MAX_LNG */
#ifndef NCO_USR_RPL_MAX_LNG
# define NCO_USR_RPL_MAX_LNG 10
#endif /* NCO_USR_RPL_MAX_LNG */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int /* O [enm] Mode flag for nco_create() call */
nco_create_mode_mrg /* [fnc] Merge clobber mode with user-specified file format */
(const int md_clobber, /* I [enm] Clobber mode (NC_CLOBBER or NC_NOCLOBBER) */
 const int fl_out_fmt); /* I [enm] Output file format */

int /* [rcd] Return code */
nco_create_mode_prs /* [fnc] Parse user-specified file format */
(const char * const fl_fmt_sng, /* I [sng] User-specified file format string */
 int * const fl_fmt_enm); /* O [enm] Output file format */

int /* O [enm] Return code */
nco_fl_dmm_mk /* Create dummy file */
(const char * const fl_out); /* I [sng] Dummy file */

void
nco_fl_sz_est /* [fnc] Estimate RAM size == uncompressed file size */
(char *smr_fl_sz_sng, /* I/O [sng] String describing estimated file size */
 const trv_tbl_sct * const trv_tbl); /* I [sct] Traversal table */

  void
nco_fl_cmp_err_chk(void); /* [fnc] Perform error checking on file */

void
nco_fl_chmod /* [fnc] Ensure file is user/owner-writable */
(const char * const fl_nm); /* I [sng] Name of file */

void
nco_fl_cp /* [fnc] Copy first file to second */
(const char * const fl_src, /* I [sng] Name of source file to copy */
 const char * const fl_dst); /* I [sng] Name of destination file */

void
nco_fl_overwrite_prm /* [fnc] Obtain user consent to overwrite output file */
(const char * const fl_nm); /* I [sng] Name of file */

void
nco_fl_fmt_vet /* [fnc] Verify output file format supports requested actions */
(const int fl_fmt, /* I [enm] Output file format */
 const int cnk_nbr, /* I [nbr] Number of chunksizes specified */
 const int dfl_lvl); /* I [enm] Deflate level [0..9] */

char * /* O [sng] Canonical file name*/
nco_fl_info_get /* [fnc] Determine canonical filename and properties */
(const char * const fl_nm_lcl); /* I [sng] Name of file */

char ** /* O [sng] List of user-specified filenames */
nco_fl_lst_mk /* [fnc] Create file list from command line positional arguments */
(CST_X_PTR_CST_PTR_CST_Y(char,argv), /* I [sng] Argument list */
 const int argc, /* I [nbr] Argument count */
 int arg_crr, /* I [idx] Index of current argument */
 int * const fl_nbr, /* O [nbr] Number of files in input file list */
 char ** const fl_out, /* I/O [sng] Name of output file */
 nco_bool *FL_LST_IN_FROM_STDIN, /* O [flg] fl_lst_in comes from stdin */
 const nco_bool FORCE_OVERWRITE); /* I [flg] Overwrite existing file, if any */

char * /* O [sng] Filename of locally available file */
nco_fl_mk_lcl /* [fnc] Retrieve input file and return local filename */
(char *fl_nm, /* I/O [sng] Current filename, if any (destroyed) */
 const char * const fl_pth_lcl, /* I [sng] Local storage area for files retrieved from remote locations */
 const nco_bool HPSS_TRY, /* I [flg] Search HPSS for unfound files */
 nco_bool * const FL_RTR_RMT_LCN); /* O [flg] File was retrieved from remote location */

void
nco_fl_mv /* [fnc] Move first file to second */
(const char * const fl_src, /* I [sng] Name of source file to move */
 const char * const fl_dst); /* I [sng] Name of destination file */

char * /* O [sng] Name of file to retrieve */
nco_fl_nm_prs /* [fnc] Construct file name from input arguments */
(char *fl_nm, /* I/O [sng] Current filename, if any */
 const int fl_idx, /* I [nbr] Ordinal index of file in input file list */
 int * const fl_nbr, /* I/O [nbr] Number of files to be processed */
 char * const * const fl_lst_in, /* I [sng] User-specified filenames */
 const int abb_arg_nbr, /* I [nbr] Number of abbreviation arguments */
 CST_X_PTR_CST_PTR_CST_Y(char,fl_lst_abb), /* I [sng] NINTAP-style arguments, if any */
 const char * const fl_pth); /* I [sng] Path prefix for files in fl_lst_in */

size_t /* [B] Blocksize */
nco_fl_blocksize /* [fnc] Find blocksize of filesystem will or does contain this file */
(const char * const fl_out); /* [sng] Filename */

char * /* O [sng] Name of temporary file actually opened */
nco_fl_out_open /* [fnc] Open output file subject to availability and user input */
(const char * const fl_out, /* I [sng] Name of file to open */
 nco_bool * const FORCE_APPEND, /* I/O [flg] Append to existing file, if any */
 const nco_bool FORCE_OVERWRITE, /* I [flg] Overwrite existing file, if any */
 const int fl_out_fmt, /* I [enm] Output file format */
 const size_t * const bfr_sz_hnt, /* I [B] Buffer size hint */
 const nco_bool RAM_CREATE, /* I [flg] Create file in RAM */
 const nco_bool RAM_OPEN, /* I [flg] Open (netCDF3) file(s) in RAM */
 const nco_bool SHARE_CREATE, /* I [flg] Create (netCDF3-only) file(s) with unbuffered I/O */
 const nco_bool SHARE_OPEN, /* I [flg] Open (netCDF3-only) file(s) with unbuffered I/O */
 const int WRT_TMP_FL, /* I [flg] Write output to temporary file */
 int * const out_id); /* O [id] File ID */

void
nco_fl_out_cls /* [fnc] Close temporary output file, move it to permanent output file */
(const char * const fl_out, /* I [sng] Name of permanent output file */
 const char * const fl_out_tmp, /* I [sng] Name of temporary output file to close and move to permanent output file */
 const int nc_id); /* I [id] File ID of fl_out_tmp */

void 
nco_fl_rm /* [fnc] Remove file */
(char *fl_nm); /* I [sng] File to be removed */

int /* [rcd] Return code */
nco_fl_open /* [fnc] Open file using appropriate buffer size hints and verbosity */
(const char * const fl_nm, /* I [sng] Name of file to open */
 const int md_open, /* I [enm] Mode flag for nc_open() call */
 const size_t * const bfr_sz_hnt, /* I [B] Buffer size hint */
 int * const nc_id); /* O [id] File ID */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_FL_UTL_H */
