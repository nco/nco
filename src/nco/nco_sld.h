/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_sld.h,v 1.1 2015-01-20 21:57:13 zender Exp $ */

/* Purpose: Description (definition) of Swath-Like Data (SLD) functions */

/* Copyright (C) 2015--2015 Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_sld.h" *//* Swath-Like Data */

#ifndef NCO_SLD_H
#define NCO_SLD_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
  char *key;
  char *value;
} kvmap;
int hdlscrip(char *scripflnm, kvmap *smps); 
kvmap sng2map(char *str,  kvmap sm); /* parse a line return a name-value pair kvmap */
int sng2array(const char *delim, const char *str, char **sarray); /* split str by delim to sarray returns size of sarray */
char * strip(char *str); /* remove heading and trailing blanks */
void freekvmaps(kvmap *kvmaps); /* release memory */
void prtkvmap (kvmap sm);  /* print kvmap contents */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SLD_H */
