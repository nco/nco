/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_ctl.h,v 1.22 2006-04-30 21:13:22 zender Exp $ */

/* Purpose: Program flow control functions */

/* Copyright (C) 1995--2006 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
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
#ifdef ENABLE_MPI
#include <mpi.h> /* MPI definitions */
#endif /* !ENABLE_MPI */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */

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

nco_bool /* [flg] Program does arithmetic */
nco_is_rth_opr /* [fnc] Query whether program does arithmetic */
(const int prg_id); /* [enm] Program ID */

nco_bool /* [flg] Program is multi-file operator */
nco_is_mlt_fl_opr /* [fnc] Query whether program is multi-file operator */
(const int prg_id); /* [enm] Program ID */

int /* O [rcd] Return code */
nco_ddra /* [fnc] Count operations */
(const char * const var_nm, /* I [sng] Variable name */
 const char * const wgt_nm, /* I [sng] Weight name */
 const int nco_op_typ, /* I [enm] Operation type */
 const int rnk_avg, /* I [nbr] Rank of averaging space */
 const int rnk_var, /* I [nbr] Variable rank (in input file) */
 const int rnk_wgt, /* I [nbr] Rank of weight */
 const int var_idx, /* I [enm] Index */
 const int wrd_sz, /* I [B] Bytes per element */
 const long long lmn_nbr, /* I [nbr] Variable size */
 const long long lmn_nbr_avg, /* I [nbr] Averaging block size */
 const long long lmn_nbr_wgt); /* I [nbr] Weight size */

void
nco_lbr_vrs_prn(void); /* [fnc] Print netCDF library version */

const char * /* O [sng] MPI implementation */
nco_mpi_get(void); /* [fnc] Return MPI implementation */

char * /* O [sng] nm_in stripped of any path (i.e., program name stub) */ 
prg_prs /* [fnc] Strip program name to stub and return program ID */
(const char * const nm_in, /* I [sng] Name of program, i.e., argv[0] (may include path prefix) */
 int * const prg_lcl); /* O [enm] Enumerated number corresponding to nm_in */

const char * /* O [sng] Mnemonic that describes current NCO version */
nco_nmn_get(void); /* [fnc] Return mnemonic that describes current NCO version */

void 
nco_usg_prn(void); /* [fnc] Print correct program usage */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_CTL_H */
