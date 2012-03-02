/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_fl_utl.h,v 1.48 2012-03-02 04:02:46 zender Exp $ */

/* Purpose: File manipulation */

/* Copyright (C) 1995--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_fl_utl.h" *//* File manipulation */

#ifndef NCO_FL_UTL_H
#define NCO_FL_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <math.h> /* sin cos cos sin 3.14159 */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */
#include <sys/stat.h> /* stat() */
#include <unistd.h> /* POSIX stuff */

#include <sys/types.h> /* needed for _res */
#include <netinet/in.h> /* needed for _res */
#include <pwd.h> /* password structures for getpwuid() */
#ifndef WIN32
#include <arpa/nameser.h> /* needed for _res */
#include <resolv.h> /* Internet structures for _res */
#endif /* !WIN32 */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int /* O [enm] Mode flag for nco_create() call */
nco_create_mode_mrg /* [fnc] Merge clobber mode with user-specified file format */
(const int clobber_mode, /* I [enm] Clobber mode (NC_CLOBBER or NC_NOCLOBBER) */
 const int fl_out_fmt); /* I [enm] Output file format */

int /* [rcd] Return code */
nco_create_mode_prs /* [fnc] Parse user-specified file format */
(const char * const fl_fmt_sng, /* I [sng] User-specified file format string */
 int * const fl_fmt_enm); /* O [enm] Output file format */

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
 nco_bool *FL_LST_IN_FROM_STDIN); /* O [flg] fl_lst_in comes from stdin */

char * /* O [sng] Filename of locally available file */
nco_fl_mk_lcl /* [fnc] Retrieve input file and return local filename */
(char *fl_nm, /* I/O [sng] Current filename, if any (destroyed) */
 const char * const fl_pth_lcl, /* I [sng] Local storage area for files retrieved from remote locations */
 nco_bool * const FILE_RETRIEVED_FROM_REMOTE_LOCATION); /* O [flg] File was retrieved from remote location */

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

char * /* O [sng] Name of temporary file actually opened */
nco_fl_out_open /* [fnc] Open output file subject to availability and user input */
(const char * const fl_out, /* I [sng] Name of file to open */
 const nco_bool FORCE_APPEND, /* I [flg] Append to existing file, if any */
 const nco_bool FORCE_OVERWRITE, /* I [flg] Overwrite existing file, if any */
 const int fl_out_fmt, /* I [enm] Output file format */
 int * const out_id); /* O [id] File ID */

void
nco_fl_out_cls /* [fnc] Close temporary output file, move it to permanent output file */
(const char * const fl_out, /* I [sng] Name of permanent output file */
 const char * const fl_out_tmp, /* I [sng] Name of temporary output file to close and move to permanent output file */
 const int nc_id); /* I [id] File ID of fl_out_tmp */

void 
nco_fl_rm /* [fnc] Remove file */
(char *fl_nm); /* I [sng] File to be removed */


#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_FL_UTL_H */
