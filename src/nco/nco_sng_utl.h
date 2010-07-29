/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sng_utl.h,v 1.23 2010-07-29 20:23:33 zender Exp $ */

/* Purpose: String utilities */

/* Copyright (C) 1995--2010 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_sng_utl.h" *//* String utilities */

#ifndef NCO_SNG_UTL_H
#define NCO_SNG_UTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */
#ifdef NEED_STRCASECMP
#include <ctype.h> /* tolower. . . */
#endif /* !NEED_STRCASECMP */

/* 3rd party vendors */

/* Personal headers */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef NEED_STRCASECMP
int /* O [enm] [-1,0,1] sng_1 [<,=,>] sng_2 */
strcasecmp /* [fnc] Lexicographical case-insensitive string comparison */
(const char *sng_1, /* I [sng] First string */
 const char *sng_2); /* I [sng] Second string */
#endif /* !NEED_STRCASECMP */

#ifdef NEED_STRDUP
char * /* O [sng] Copy of input string */
strdup /* [fnc] Duplicate string */
(const char *sng_in); /* I [sng] String to duplicate */
#endif /* !NEED_STRDUP */

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
