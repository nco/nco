/* $Header$ */

/* Purpose: Software configuration management */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_scm.h" *//* Software configuration management */

#ifndef NCO_SCM_H
#define NCO_SCM_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp() */

/* 3rd party vendors */

/* Personal headers */
#include "nco_mmr.h" /* Memory management */
#include "nco_sng_utl.h" /* String utilities */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * /* O [sng] CVS version string */
cvs_vrs_prs(void); /* [fnc] Return CVS version string */

void
nco_cpy_prn(void); /* [fnc] Print copyright notice */

void
nco_lbr_vrs_prn(void); /* [fnc] Print netCDF library version */

const char * /* O [sng] Mnemonic that describes current NCO version */
nco_nmn_get(void); /* [fnc] Return mnemonic that describes current NCO version */

void
nco_vrs_prn /* [fnc] Print NCO version */
(const char * const CVS_Id, /* I [sng] CVS identification string */
 const char * const CVS_Revision); /* I [sng] CVS revision string */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SCM_H */
