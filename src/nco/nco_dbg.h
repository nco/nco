/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_dbg.h,v 1.8 2006-01-31 06:42:11 zender Exp $ */

/* Purpose: Debugging */

/* Copyright (C) 1995--2006 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_dbg.h" *//* Debugging */

#ifndef NCO_DBG_H
#define NCO_DBG_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void
err_prn /* [fnc] Print formatted error messages */
(char *err_msg); /* I [sng] Formatted error message to print */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_DBG_H */
