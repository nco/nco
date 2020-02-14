/* $Header$ */

/* Purpose: Cartesian coordinate polygon intersections */

/* This file includes BSD-licensed code whose copyright is held by another author
   The copyright owner and license terms for the NCO modifications are
   Copyright (C) 2018--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* This copyright statement and license terms for the original code were agreed
   to by the original author, Joseph O'Rourke, on 20190517:

   This code is described in "Computational Geometry in C" (Second Edition),
   Chapter 7.  It is not written to be comprehensible without the
   explanation in that book.
   
   Written by Joseph O'Rourke.
   Last modified: December 1997
   Questions to jorourke@smith.edu.
   -------------------------------------------------------------------------
   Copyright 1997 by Joseph O'Rourke <jorourke@smith.edu>
   
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
   
   1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
   
   2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
   
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
   TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
   PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   ------------------------------------------------------------------------- */

/* Usage:
   #include "nco_crt.h" *//* Cartesian geometry intersections */

#ifndef NCO_CRT_H /* Contents have not yet been inserted in current source file */
#define NCO_CRT_H

#include        <stdlib.h>
#include        <stdio.h>
#include        <math.h>

/* Personal headers */
#include "nco.h" /* netCDF Operator (NCO) definitions */
#include "nco_mmr.h"     /* Memory management */
#include "nco_omp.h"     /* OpenMP utilities */
#include "nco_rgr.h"     /* Regridding */
#include "nco_sld.h"     /* Swath-Like Data */
#include "nco_sng_utl.h" /* String utilities */
#include "nco_ply.h"     /* poly sct stuff */

/* Dimension of points */
#define DSIGMA (1.0e-14)

/* define minimium area in AreaSign (cross-product) */
#define DAREA  1.0e-28  

#define VP_MAX    1000            /* Max # of pts in polygon */

#define ARC_MIN_LENGTH (1.0e-20)

/* if true then longitude 0-360 */
/* we need this to convert 3D back to 2D */
#define IS_LON_360 (1)

#define NBR_CRT (2)
#define DEBUG_CRT (1)

#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* !__cplusplus */

/*---------------------------------------------------------------------
Function prototypes.
---------------------------------------------------------------------*/

int    nco_crt_intersect(poly_sct *P, poly_sct *Q, poly_sct *R, int *r);

char    nco_crt_seg_int(double *a, double *b, double *c, double *d, double *p, double *q);
char    nco_crt_parallel_int(double *a, double *b, double *c, double *d, double *p, double *q);
int	nco_crt_cross_sgn(double *a, double *b, double *c);
nco_bool  nco_crt_between(double *a, double *b, double *c);

double  nco_crt_dot(double *a, double *b);
void    nco_crt_sub_vec(double *a, double *b, double *c);
void    nco_crt_adi(double *p, double *a);

void nco_crt_add_pnt(double **R, int *r, double *P);

nco_bool  nco_crt_linear(double *a, double *b, double *c);

nco_bool  nco_crt_left_on(double *a, double *b, double *c);
nco_bool  nco_crt_left(double *a, double *b, double *c);

const char * nco_poly_vrl_flg_sng_get(poly_vrl_flg_enm in);

void
nco_crt_set_domain(double lon_min, double lon_max, double lat_min, double lat_max);


nco_bool            /* O [flg] True if point in inside (or on boundary ) of polygon */
nco_crt_pnt_in_poly(
int crn_nbr,
double x_in,
double y_in,
double *lcl_dp_x,
double *lcl_dp_y);

int             /* O [nbr] returns number of points of pl_out that are inside pl_in */
nco_crt_poly_in_poly(
poly_sct *pl_in,
poly_sct *pl_out);


/*-------------------------------------------------------------------*/



#ifdef __cplusplus
} /* end extern "C" */
#endif /* !__cplusplus */

#endif /* NCO_CRT_H */
