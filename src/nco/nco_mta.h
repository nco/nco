/* $Header$ */

/* Purpose: Multi-argument parsing */

/* Copyright (C) 1995--2016 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_mta.h" *//* Multi-argument parsing */

#ifndef NCO_MTA_H
#define NCO_MTA_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */
#ifdef HAVE_STRINGS_H
# include <strings.h> /* strcasecmp() */
#endif /* !HAVE_STRINGS_H */

/* 3rd party vendors */

/* Personal headers */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  kvm_sct /* O [sct] Key-value pair */
  nco_sng2kvm /* [fnc] Parse string into key-value pair */
  (const char *sng); /* I [sng] String to parse, including "=" */
  
  char * /* O [sng] Stripped-string */
  nco_sng_strip /* [fnc] Strip leading and trailing white space */
  (char *sng); /* I/O [sng] String to strip */
  
  kvm_sct * /* O [sct] Pointer to free'd kvm list */
  nco_kvm_lst_free /* [fnc] Relinquish dynamic memory from list of kvm structures */
  (kvm_sct *kvm, /* I/O [sct] List of kvm structures */
   const int kvm_nbr); /* I [nbr] Number of kvm structures */
  
  void
  nco_kvm_prn /* [fnc] Print kvm contents */
  (kvm_sct kvm); /* [fnc] kvm to print */

  char *
  nco_strip_backslash
  (char* args);

  char** /* O [pointer to sngs] group of splitted sngs*/
  nco_string_split /* [fnc] split the string by delimiter */
  (const char *restrict source, /* I [sng] the source string */
  const char* delimiter);

  int /* O [int] the boolean for the checking result */
  nco_input_check /* [fnc] check whether the input is legal and give feedback accordingly. */
  (const char *restrict args); /* O [sng] input arguments */

  void 
  nco_sng_lst_free_void
  (char **restrict sng_lst,
  const int block_num);

  kvm_sct * /* O [kvm_sct] Pointer to first kvm structure */
  nco_arg_mlt_prs /* [fnc] Main parser, split string and assign to kvm structure */
  (const char *restrict args); /* I [sng] Input string */

  char * /* O [sng] Joined strings */
  nco_join_sng /* [fnc] Join strings with delimiter */
  (const char **restrict sng_lst, /* I [sng] List of strings being connected */
   const int sng_nbr); /* I [int] Number of strings */

  int
  nco_count_blocks
  (const char* args,
   char* delimiter);

  char *
  nco_mlt_arg_dlm_set
  (const char *dlm_sng_usr);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MTA_H */
