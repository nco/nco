/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_scm.h,v 1.11 2007-01-22 04:04:24 zender Exp $ */

/* Purpose: Software configuration management */

/* Copyright (C) 1995--2007 Charlie Zender
   This software may be modified and/or re-distributed under the terms of the GNU General Public License (GPL) Version 2
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_scm.h" *//* Software configuration management */

#ifndef NCO_SCM_H
#define NCO_SCM_H

#ifdef HAVE_CONFIG_H
#include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */

/* Personal headers */
#include "nco_mmr.h" /* Memory management */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

char * /* O [sng] CVS version string */
cvs_vrs_prs(void); /* [fnc] Return CVS version string */

void
copyright_prn /* [fnc] Print copyright notice */
(const char * const CVS_Id, /* I [sng] CVS identification string */
 const char * const CVS_Revision); /* I [sng] CVS revision string */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SCM_H */
