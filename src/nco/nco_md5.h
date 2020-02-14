/* $Header$ */

/* Purpose: Description (definition) of MD5 digest functions */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Usage:
   #include "nco_md5.h" *//* MD5 digests */

/* This NCO file contains the entirety of the MD5 implementation by
   L. Peter Deutsch of Aladdin Software (aka author of Ghostscript).
   NCO-specific functions are defined first, then LPD's md5.h is 
   included in a nearly unaltered state. */

#ifndef NCO_MD5_H
#define NCO_MD5_H

/* Standard header files */
#include <stdio.h> /* stderr, FILE, NULL, etc. */
#include <string.h> /* strcmp() */

/* 3rd party vendors */
#include <netcdf.h> /* netCDF definitions and C library */
#include "nco_netcdf.h" /* NCO wrappers for netCDF C library */

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_att_utl.h" /* Attribute utilities */
#include "nco_mmr.h" /* Memory management */

/* MD5 specification defines digest as a 16 byte array 
   Each byte represents a value [0..255] which requires two hexadecimal characters to print 
   Hence MD5 digest string is 2*16+1 characters long (extra one is for NUL-terminator */
#define NCO_MD5_DGS_SZ 16

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
  
  md5_sct * /* [sct] MD5 configuration */
  nco_md5_ini(void); /* [fnc] Initialize and return MD5 configuration structure */

  md5_sct * /* [sct] MD5 configuration */
  nco_md5_free /* [fnc] Free MD5 configuration structure */
  (md5_sct * const md5); /* [sct] MD5 configuration structure to free */

  void
  nco_md5_chk /* [fnc] Perform and optionally compare MD5 digest(s) on hyperslab */
  (const md5_sct * const md5, /* I [sct] MD5 Configuration */
   const char * const var_nm, /* I [sng] Input variable name */
   const long var_sz_byt, /* I [nbr] Size (in bytes) of hyperslab in RAM */
   const int nc_id, /* I [id] netCDF file ID */
   const long * const dmn_srt, /* I [idx] Contiguous vector of indices to start of hyperslab on disk */
   const long * const dmn_cnt, /* I [nbr] Contiguous vector of lengths of hyperslab on disk */
   void * const vp); /* I/O [val] Values to digest */
  
  void
  nco_md5_chk_ram /* [fnc] Perform MD5 digest on hyperslab in RAM */
  (const long var_sz_byt, /* I [nbr] Size (in bytes) of hyperslab */
   const void * const vp, /* I [val] Values to digest */
   char md5_dgs_hxd_sng[NCO_MD5_DGS_SZ*2+1]); /* O [sng] MD5 digest */
  
#ifdef __cplusplus
} /* end extern "C" */
#endif /* __cplusplus */

#endif /* NCO_MD5_H */

/* Begin md5.h by LPD: */

/*
  Copyright (C) 1999, 2002 Aladdin Enterprises.  All rights reserved.
  
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  
  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
  
  L. Peter Deutsch
  ghost@aladdin.com
  
*/
/* $Id$ */
/*
  Independent implementation of MD5 (RFC 1321).
  
  This code implements the MD5 Algorithm defined in RFC 1321, whose
  text is available at
  http://www.ietf.org/rfc/rfc1321.txt
  The code is derived from the text of the RFC, including the test suite
  (section A.5) but excluding the rest of Appendix A.  It does not include
  any code or documentation that is identified in the RFC as being
  copyrighted.
  
  The original and principal author of md5.h is L. Peter Deutsch
  <ghost@aladdin.com>.  Other authors are noted in the change history
  that follows (in reverse chronological order):
  
  2002-04-13 lpd Removed support for non-ANSI compilers; removed
  references to Ghostscript; clarified derivation from RFC 1321;
  now handles byte order either statically or dynamically.
  1999-11-04 lpd Edited comments slightly for automatic TOC extraction.
  1999-10-18 lpd Fixed typo in header comment (ansi2knr rather than md5);
  added conditionalization for C++ compilation from Martin
  Purschke <purschke@bnl.gov>.
  1999-05-03 lpd Original version.
*/

#ifndef md5_INCLUDED
#  define md5_INCLUDED

/*
 * This package supports both compile-time and run-time determination of CPU
 * byte order.  If ARCH_IS_BIG_ENDIAN is defined as 0, the code will be
 * compiled to run only on little-endian CPUs; if ARCH_IS_BIG_ENDIAN is
 * defined as non-zero, the code will be compiled to run only on big-endian
 * CPUs; if ARCH_IS_BIG_ENDIAN is not defined, the code will be compiled to
 * run on either big- or little-endian CPUs, but will run slightly less
 * efficiently on either one than if ARCH_IS_BIG_ENDIAN is defined.
 */

typedef unsigned char md5_byte_t; /* 8-bit byte */
typedef unsigned int md5_word_t; /* 32-bit word */

/* Define the state of the MD5 Algorithm. */
typedef struct md5_state_s {
  md5_word_t count[2];	/* message length in bits, lsw first */
  md5_word_t abcd[4];		/* digest buffer */
  md5_byte_t buf[64];		/* accumulate block */
} md5_state_t;

#ifdef __cplusplus
extern "C" 
{
#endif
  
  /* Initialize the algorithm. */
  void md5_init(md5_state_t *pms);
  
  /* Append a string to the message. */
  void md5_append(md5_state_t *pms, const md5_byte_t *data, int nbytes);
  
  /* Finish the message and return the digest. */
  void md5_finish(md5_state_t *pms, md5_byte_t digest[16]);
  
#ifdef __cplusplus
}  /* end extern "C" */
#endif

#endif /* md5_INCLUDED */

