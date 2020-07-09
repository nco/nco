/* $Header$ */

/* Purpose: List utilities */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_lst_utl.h" *//* List utilities */

#ifndef NCO_LST_UTL_H
#define NCO_LST_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, qsort */
#include <string.h> /* strcmp() */

#ifdef HAVE_REGEX_H 
/* 20120213: Linux GCC 4.6 man page says regex.h depends on (non-present) sys/types.h */
# ifdef MACOSX
#  include <sys/types.h> /*19950312: _res, 20040822: Mac OS X off_t required by regex.h */
# endif /* !MACOSX */
# include <regex.h> /* POSIX regular expressions library */
#endif /* HAVE_REGEX_H */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_grp_trv.h" /* Group traversal */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * /* O [nbr] Format string with printf()-formats replaced */
nco_fmt_sng_printf_subst /* [fnc] Replace printf() format statements */
(const char * const fmt_sng); /* I [sng] Format string before processing */

int /* O [nbr] Number of matches found */
nco_lst_rx_search /* [fnc] Search for pattern matches in var string list */
(const int var_nbr_all, /* I [nbr] Size of var_lst_all and var_xtr_rqs */
 nm_id_sct *var_lst_all, /* I [sct] All variables in input file (with IDs) */
 char *rx_sng, /* I [sng] Regular expression pattern */
 nco_bool *var_xtr_rqs); /* O [flg] Matched vars holder */

nco_bool /* O [flg] Both var_nm and bnds_nm are in rgd_arr_lst */
nco_rgd_arr_lst_chk /* [fnc] Check list of ragged arrays for presence of var_nm and bnds_nm */
(char ***rgd_arr_lst, /* I [sct] List of ragged arrays */
 int nbr_lst, /* I [nbr] Number of ragged arrays in list */
 char *var_nm, /* I [sng] Variable name to search for */
 char *bnds_nm); /* I [sng] Bounds name to search for */

void
nco_rgd_arr_lst_free /* [fnc] Free memory associated with rgd_arr_lst, a list of ragged arrays */
(char ***rgd_arr_lst, /* I/O [sct] List of ragged arrays */
 int nbr_lst); /* I [nbr] Number of ragged arrays in list */

void 
nco_srt_ntg /* [fnc] Sort array of integers */
(const int lmn_nbr, /* I [nbr] Number of elements */
 const int * const arr_in, /* I [idx] Array to sort */
 int * const idx); /* O [idx] Indices to sorted array */

void 
nco_srt_lph /* [fnc] Sort array of strings */
(const int lmn_nbr, /* I [nbr] Number of elements */
 char * const * const arr_in, /* I [sng] Strings to sort */
 int * const idx); /* O [idx] Indices to sorted array */

char ** /* O [sng] Array of list elements */
nco_lst_prs_1D /* [fnc] Create 1D array of strings from given string and delimiter */
(char * const sng_in, /* I/O [sng] Delimited argument list (delimiters are changed to NULL on output */
 const char * const dlm_sng, /* I [sng] Delimiter string */
 int * const nbr_lst); /* O [nbr] Number of elements in list */

char ** /* O [sng] List of strings */
nco_lst_prs_2D /* [fnc] Create 2D list of strings from given string and delimiter */
(const char * const sng_in, /* I [sng] Delimited argument list */
 const char * const dlm_sng, /* I [sng] Delimiter string */
 int * const nbr_lst); /* O [nbr] Number of elements in list */

char ** /* O [sng] List of strings */
nco_lst_prs_sgl_2D /* [fnc] Create list of strings from given string and delimiter */
(const char * const sng_in, /* I [sng] Delimited argument list */
 const char * const dlm_sng, /* I [sng] Delimiter string */
 int * const nbr_lst); /* O [nbr] Number of elements in list */

nm_id_sct * /* O [sct] Sorted output list */
lst_heapsort /* [fnc] Heapsort input lists numerically or alphabetically */
(nm_id_sct *lst, /* I/O [sct] Current list (destroyed) */
 const int nbr_lst, /* I [nbr] number of members in list */
 const nco_bool ALPHABETIZE_OUTPUT); /* I [flg] Alphabetize extraction list */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_chr /* [fnc] Compare two characters */
(const void *val_1, /* I [chr] Character to compare */
 const void *val_2); /* I [chr] Character to compare */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_int /* [fnc] Compare two integers */
(const void *val_1, /* I [nbr] Number to compare */
 const void *val_2); /* I [nbr] Number to compare */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_sng /* [fnc] Compare two strings */
(const void *val_1, /* I [sng] String to compare */
 const void *val_2); /* I [sng] String to compare */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_nm_id_nm /* [fnc] Compare two nm_id_sct's by name member */
(const void *val_1, /* I [sct] nm_id_sct to compare */
 const void *val_2); /* I [sct] nm_id_sct to compare */

int /* O [enm] Comparison result [<,=,>] 0 iff val_1 [<,==,>] val_2 */
nco_cmp_nm_id_id /* [fnc] Compare two nm_id_sct's by ID member */
(const void *val_1, /* I [sct] nm_id_sct to compare */
 const void *val_2); /* I [sct] nm_id_sct to compare */

int /* O [enm]  Comparison result [<,=,>] 0 iff op1 [<,==,>] op2 */
nco_cmp_ptr_unn /* Compare values of two pointer unions of same type */
(const nc_type type, /* I [enm] netCDF type of operands */
 const ptr_unn op1, /* I [sct] First operand to compare */
 const ptr_unn op2); /* I [sct] Second operand to compare */

dmn_sct ** /* O [sct] Pointer to free'd structure list */
nco_dmn_lst_free /* [fnc] Free memory associated with dimension structure list */
(dmn_sct **dmn_lst, /* I/O [sct] Dimension structure list to free */
 const int dmn_nbr); /* I [nbr] Number of dimension structures in list */

void 
nco_hash2comma /* [fnc] Replace hashes with commas */
(char * const rx_sng); /* [sng] Regular expression */

void 
nco_rx_comma2hash /* [fnc] Convert {...,...} to {...#...} in regular expressions */
(char * const rx_sng); /* [sng] Regular expression */

nm_id_sct * /* O [sct] Sorted output list */
nco_lst_srt_nm_id /* [fnc] Sort name/ID input list numerically or alphabetically */
(nm_id_sct * const lst, /* I/O [sct] Current list (destroyed) */
 const int nbr_lst, /* I [nbr] number of members in list */
 const nco_bool ALPHABETIZE_OUTPUT); /* I [flg] Alphabetize extraction list */

nm_id_sct * /* O [sct] Pointer to free'd structure list */
nco_nm_id_lst_free /* [fnc] Free memory associated with name-ID structure list */
(nm_id_sct *nm_id_lst, /* I/O [sct] Name-ID structure list to free */
 const int nm_id_nbr); /* I [nbr] Number of name-ID structures in list */

char ** /* O [sng] Pointer to free'd string list */
nco_sng_lst_free /* [fnc] Free memory associated with string list */
(char **sng_lst, /* I/O [sng] String list to free */
 const int sng_nbr); /* I [nbr] Number of strings in list */

char * /* O [sng] Concatenated string formed by joining all input strings */
sng_lst_cat /* [fnc] Join string list together into one string, delete originals */
(char ** const sng_lst, /* I/O [sng] List of pointers to strings to join together */
 const long lmn_nbr, /* I [nbr] Number of strings in list */
 const char * const dlm_sng); /* I [sng] Delimiter string to use as glue */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_LST_UTL_H */
