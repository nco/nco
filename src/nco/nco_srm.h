/* $Header$ */

/* Purpose: Streams */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

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
