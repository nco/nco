/* $Header$ */

/* Purpose: Program flow control functions */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_ctl.h" *//* Program flow control functions */

#ifndef NCO_CTL_H
#define NCO_CTL_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <stdlib.h> /* strtod, strtol, malloc, getopt, exit */
#include <string.h> /* strcmp() */
#include <time.h> /* machine time */
#ifndef _MSC_VER
# include <unistd.h> /* POSIX stuff */
#endif

/* 3rd party vendors */
#ifdef ENABLE_MPI
#include <mpi.h> /* MPI definitions */
#endif /* !ENABLE_MPI */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */
#include "nco_pck.h" /* Packing and unpacking variables */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

const char * /* O [sng] Compiler and version */
nco_cmp_get(void); /* [fnc] Return compiler and version */

void
nco_exit /* [fnc] Wrapper for exit() */
(int rcd); /* I [enm] Return code */

void 
nco_exit_gracefully(void); /* [fnc] Clean up timers, file descriptors, memory, then exit */

void
nco_exit_lbr_rcd(void); /* [fnc] Exit with netCDF library version as return code */

nco_bool /* [flg] Program does arithmetic */
nco_is_rth_opr /* [fnc] Query whether program does arithmetic */
(const int nco_prg_id); /* [enm] Program ID */

nco_bool /* [flg] Program does arithmetic and preserves rank */
nco_is_sz_rnk_prv_rth_opr /* [fnc] Is program size and rank-preserving arithmetic operator? */
(const int nco_prg_id, /* I [enm] Program ID */
 const int nco_pck_plc); /* I [enm] Packing policy */

nco_bool /* [flg] Program is multi-file operator */
nco_is_mfo /* [fnc] Query whether program is multi-file operator */
(const int nco_prg_id); /* [enm] Program ID */

int /* O [rcd] Return code */
nco_ddra /* [fnc] Count operations */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const wgt_nm, /* I [sng] Weight name */
 const ddra_info_sct * const ddra_info); /* I [sct] DDRA information */

void 
nco_dfl_case_tmr_typ_err(void); /* [fnc] Print error and exit for illegal switch(tmr_typ) case */

void
nco_cnf_prn(void); /* [fnc] Print NCO configuration and help text */

const char * /* O [sng] MPI implementation */
nco_mpi_get(void); /* [fnc] Return MPI implementation */

char * /* O [sng] nm_in stripped of any path (i.e., program name stub) */ 
nco_prg_prs /* [fnc] Strip program name to stub and set program ID */
(const char * const nm_in, /* I [sng] Name of program, i.e., argv[0] (may include path prefix) */
 int * const nco_prg_lcl); /* O [enm] Enumerated number corresponding to nm_in */

void 
nco_usg_prn(void); /* [fnc] Print correct program usage */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CTL_H */
