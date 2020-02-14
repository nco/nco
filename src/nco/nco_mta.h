/* $Header$ */

/* Purpose: Multi-argument parsing */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

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

/* 3rd party vendors */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_lst_utl.h" /* List utilities */
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

  char * /* O/I [sng] string that has backslash(es) */
  nco_remove_backslash
  (char *args); /* O/I [sng] string that had already been got rid of backslash(es) */

  char * /* O [sng] the flag that has no hyphens */
  nco_remove_hyphens /* [fnc] Remove the hyphens come before the flag */
  (char* args); /* I [sng] the flag that has hyphens in it*/

  char ** /* O [sng] Group of split strings */
  nco_sng_split /* [fnc] Split string by delimiter */
  (const char *source, /* I [sng] Source string */
   const char *delimiter); /* I [char] Delimiter */

  int /* O [flg] Option is flag */
  nco_opt_is_flg /* [fnc] Check whether option is registered as NCO flag */
  (const char* flag); /* I [sng] Input string */

  int /* O [int] the boolean for the checking result */
  nco_input_check /* [fnc] check whether the input is legal and give feedback accordingly. */
  (const char *args); /* O [sng] input arguments */

  kvm_sct * /* O [kvm_sct] Pointer to first kvm structure */
  nco_arg_mlt_prs /* [fnc] Main parser, split string and assign to kvm structure */
  (const char *args); /* I [sng] Input string */

  char * /* O [sng] Joined strings */
  nco_join_sng /* [fnc] Join strings with delimiter */
  (X_CST_PTR_CST_PTR_Y(char,sng_lst), /* I [sng] List of strings to join */
   const int sng_nbr); /* I [int] Number of strings */

  int // O [int] Number of string blocks that will be split with delimiter
  nco_count_blocks // [fnc] Check number of string blocks that will be split with delimiter
  (const char *args, // I [sng] String to be split
   const char *delimiter); // I [sng] Delimiter

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MTA_H */
