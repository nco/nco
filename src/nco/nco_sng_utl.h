/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sng_utl.h,v 1.6 2002-09-09 04:14:07 zender Exp $ */

/* Purpose: String utilities */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_sng_utl.h" *//* String utilities */

#ifndef NCO_SNG_UTL_H
#define NCO_SNG_UTL_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */

/* Personal headers */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * /* O [sng] Parsed command line */
nco_cmd_ln_sng /* [fnc] Re-construct command line from arguments */
(const int argc, /* I [nbr] Argument count */
 CST_X_PTR_CST_PTR_CST_Y(char,argv)); /* I [sng] Command line argument values */
 /* char **argv); *//* I [sng] Command line argument values */

int /* O [nbr] Number of escape sequences translated */
sng_ascii_trn /* [fnc] Replace C language '\X' escape codes in string with ASCII bytes */
(char * const sng); /* I/O [sng] String to process */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SNG_UTL_H */
