/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_scm.h,v 1.2 2002-08-22 20:16:54 zender Exp $ */

/* Purpose: Software configuration management */

/* Copyright (C) 1995--2002 Charlie Zender
   This software is distributed under the terms of the GNU General Public License
   See http://www.gnu.ai.mit.edu/copyleft/gpl.html for full license text */

/* Usage:
   #include "nco_scm.h" *//* Software configuration management */

#ifndef NCO_SCM_H
#define NCO_SCM_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, printf */
#include <string.h> /* strcmp. . . */

/* 3rd party vendors */

/* Personal headers */
#ifdef HAVE_CONFIG_H
# include <config.h> /* Autotools tokens */
#endif /* !HAVE_CONFIG_H */
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

char * /* O [sng] Mnemonic that describes current NCO version */
nmn_get(void); /* [fnc] Return mnemonic that describes current NCO version */

#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_SCM_H */
