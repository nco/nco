/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_ctl.h,v 1.7 2002-09-03 01:19:54 zender Exp $ */

/* Purpose: Program flow control functions */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_ctl.h" *//* Program flow control functions */

#ifndef NCO_CTL_H
#define NCO_CTL_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp. . . */
#include <time.h> /* machine time */
#include <unistd.h> /* POSIX stuff */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions */
#include "nco_netcdf.h" /* netCDF3.0 wrapper functions */

/* Personal headers */
#include "nco.h" /* NCO definitions */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
nco_exit /* [fnc] Wrapper for exit() */
(int rcd); /* I [enm] Return code */

void 
nco_exit_gracefully(void); /* [fnc] Clean up timers, file descriptors, then exit */

bool /* [flg] Program does arithmetic */
is_rth_opr /* [fnc] Query whether program does arithmetic */
(const int prg_id); /* [enm] Program ID */

void
nco_lib_vrs_prn(void); /* [fnc] Print netCDF library version */

char * /* O [sng] nm_in stripped of any path (i.e., program name stub) */ 
prg_prs /* [fnc] Strip program name to stub and return program ID */
(const char * const nm_in, /* I [sng] Name of program, i.e., argv[0] (may include path prefix) */
 int * const prg); /* O [enm] Enumerated number corresponding to nm_in */

void 
nco_usg_prn(void); /* [fnc] Print correct program usage */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CTL_H */
