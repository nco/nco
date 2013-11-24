/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_srm.h,v 1.1 2013-11-24 04:06:09 zender Exp $ */

/* Purpose: Streams */

/* Copyright (C) 1995--2013 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_srm.h" *//* Streams */

#ifndef NCO_SRM_H
#define NCO_SRM_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_cnf_typ.h" /* Conform variable types */
#include "nco_ctl.h" /* Program flow control functions */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */

typedef unsigned char nco_srm_mgc_t; /* ncstream MAGIC primitives are 4-bytes long */
typedef struct nco_srm_sct{
  nco_srm_mgc_t mgc_srt[4];
  nco_srm_mgc_t mgc_end[4];
} nco_srm_t;

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void 
nco_srm_hdr /* [fnc] Write header to stream */
(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SRM_H */
