/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_lst_utl.h,v 1.18 2005-03-27 20:35:16 zender Exp $ */

/* Purpose: List utilities */

/* Copyright (C) 1995--2005 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_lst_utl.h" *//* List utilities */

#ifndef NCO_LST_UTL_H
#define NCO_LST_UTL_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, qsort */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 
indexx /* [fnc] Sort array of integers */
(const int lmn_nbr, /* I [nbr] Number of elements */
 const int * const arr_in, /* I [idx] Array to sort */
 int * const idx); /* O [idx] Indices to sorted array */

void 
indexx_alpha /* [fnc] Sort array of strings */
(const int lmn_nbr, /* I [nbr] Number of elements */
 char * const * const arr_in, /* I [sng] Strings to sort */
 int * const idx); /* O [idx] Indices to sorted array */

char ** /* O [sng] Array of list elements */
lst_prs /* [fnc] Create list of strings from given string and arbitrary delimiter */
(char * const sng_in, /* I/O [sng] Delimited argument list (delimiters are changed to NULL on output */
 const char * const dlm_sng, /* I [sng] delimiter string */
 int * const nbr_lst); /* O [nbr] number of elements in list */

nm_id_sct * /* O [sct] Sorted output list */
lst_heapsort /* [fnc] Heapsort input lists numerically or alphabetically */
(nm_id_sct *lst, /* I/O [sct] Current list (destroyed) */
 const int nbr_lst, /* I [nbr] number of members in list */
 const bool ALPHABETIZE_OUTPUT); /* I [flg] Alphabetize extraction list */

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

void 
nco_lst_comma2hash /* [fnc] Convert {...,...} to {...#...} in regular expressions */
(char * const rx_sng); /* [sng] Regular expression */

nm_id_sct * /* O [sct] Sorted output list */
nco_lst_srt_nm_id /* [fnc] Sort name/ID input list numerically or alphabetically */
(nm_id_sct * const lst, /* I/O [sct] Current list (destroyed) */
 const int nbr_lst, /* I [nbr] number of members in list */
 const bool ALPHABETIZE_OUTPUT); /* I [flg] Alphabetize extraction list */

nm_id_sct * /* O [sct] Pointer to free'd structure list */
nco_nm_id_lst_free /* [fnc] Free memory associated with name-ID structure list */
(nm_id_sct *nm_id_lst, /* I/O [sct] Name-ID struture list to free */
 const int nm_id_nbr); /* I [nbr] Number of name-ID strutures in list */

char * /* O [sng] Concatenated string formed by joining all input strings */
sng_lst_cat /* [fnc] Join list of strings together into one string */
(X_CST_PTR_CST_PTR_Y(char,sng_lst), /* I [sng] List of pointers to strings to join together */
 const long lmn_nbr, /* O [nbr] Number of strings in list */
 const char * const dlm_sng); /* I [sng] delimiter string to use as glue */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_LST_UTL_H */
