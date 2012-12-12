/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_grp_trv.h,v 1.20 2012-12-12 02:49:47 pvicente Exp $ */

/* Purpose: netCDF4 traversal storage */

/* Copyright (C) 2011--2012 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_grp_trv.h" *//* Group traversal */

#ifndef NCO_GRP_TRV_H
#define NCO_GRP_TRV_H

#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h" /* Memory management */

void                         
trv_tbl_init
(trv_tbl_sct **tbl); /* I/O [sct] Traversal table */
/* end trv_tbl_init */

void 
trv_tbl_free
(trv_tbl_sct *tbl); /* I [sct] Traversal table */
/* end trv_tbl_free */

void 
trv_tbl_add
(trv_sct obj,        /* I [sct] Object to store */
 trv_tbl_sct *tbl);  /* I/O [sct] Traversal table */
/* end trv_tbl_add */

#endif /* NCO_GRP_TRV_H */
