/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_lst_utl.h,v 1.4 2002-05-07 08:34:15 zender Exp $ */

/* Purpose: List utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_lst_utl.h" *//* List utilities */

#ifndef NCO_LST_UTL_H
#define NCO_LST_UTL_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * /* O [sng] Concatenated string formed by joining all input strings */
sng_lst_prs /* [fnc] Join list of strings together into one string */
(char * const * const sng_lst, /* I [sng] List of pointers to strings to join together */
 const long lmn_nbr, /* O [nbr] Number of strings in list */
 const char * const dlm_sng); /* I [sng] delimiter string to use as glue */

char ** /* O [sng] Array of list elements */
lst_prs /* [fnc] Create list of strings from given string and arbitrary delimiter */
(char * const sng_in, /* I/O [sng] Delimited argument list (delimiters are changed to NULL on output */
 const char * const dlm_sng, /* I [sng] delimiter string */
 int * const nbr_lst); /* O [nbr] number of elements in list */

void 
indexx /* [fnc] Sort array of integers */
(const int n, /* I [nbr] Number of elements */
 const int * const arrin, /* I [idx] Array to sort */
 int * const indx); /* O [idx] Indices to sorted array */

void 
index_alpha /* [fnc] Sort array of strings */
(const int n, /* I [nbr] Number of elements */
 char * const * const arrin, /* I [sng] Strings to sort */
 int * const indx); /* O [idx] Indices to sorted array */

nm_id_sct * /* O [sct] Sorted output list */
lst_heapsort /* [fnc] Heapsort input lists numerically or alphabetically */
(nm_id_sct *lst, /* I/O [sct] Current list (destroyed) */
 const int nbr_lst, /* I [nbr] number of members in list */
 const bool ALPHABETIZE_OUTPUT); /* I [flg] Alphabetize extraction list */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_LST_UTL_H */
