/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_dbg.h,v 1.2 2002-05-06 06:51:27 zender Exp $ */

/* Purpose: Debugging */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_dbg.h" *//* Debugging */

#ifndef NCO_DBG_H
#define NCO_DBG_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */

/* 3rd party vendors */

/* Personal headers */
#include "nco.h" /* netCDF operator definitions */

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
