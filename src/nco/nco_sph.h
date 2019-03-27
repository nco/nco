#ifndef NCO_SPH_H /* Contents have not yet been inserted in current source file */
#define NCO_SPH_H


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
#include "nco_ply.h"    /* poly sct stuff */
#include "nco_crt.h"

#define NBR_SPH (5)

#define VP_MAX    1000            /* Max # of pts in polygon */


/* this is 1.0e-20 * PI / 180.0 */
#define ARC_MIN_LENGTH_RAD (1.0e-15)

/* smallest RADIAN */
#define SIGMA_RAD (1.0e-12)

/* convert Degrees to Radians */
#define D2R(x)  ((x) * M_PI / 180.0)
/* convert Radians to degrees */
#define R2D(x)  ((x) * 180.0 / M_PI)



/* if true then longitude 0-360 */
/* we need this to convert 3D back to 2D */
#define IS_LON_360 (1)

// #define DEBUG_SPH (1)


#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* !__cplusplus */



/*---------------------------------------------------------------------
Function prototypes.
---------------------------------------------------------------------*/


int
nco_sph_intersect(poly_sct *P, poly_sct *Q, poly_sct *R, int *r);

char
nco_sph_seg_int(double *a, double *b, double *c, double *d, double *p, double *q);

int
nco_sph_lhs(double *Pi, double *Qi);

nco_bool
nco_sph_face(int iLHS, int iRHS, int jRHS);

double
nco_sph_dot(double *a, double *b);

double
nco_sph_dot_nm(double *a, double *b);

double
nco_sph_cross(double *a, double *b, double *c);

double
nco_sph_rad(double *a);

double
nco_sph_sxcross(double *a, double *b, double *c);

void
nco_sph_adi(double *a, double *b);

void
nco_sph_add_pnt(double **R, int *r, double *P);

nco_bool
nco_sph_between(double a, double b, double x);

nco_bool
nco_sph_lonlat_between(double *a, double *b, double *x);

int
nco_sph_parallel(double *a, double *b, double *c, double *d, double *p, double *q);

int
nco_sph_parallel_lat(double *a, double *b, double *c, double *d, double *p, double *q);


void
nco_sph_prn_pnt(const char *sMsg, double *p, int style, nco_bool bRet);

nco_bool
nco_sph_is_convex(double **sP, int np);

void
nco_sph_prn(double **sR, int r, int istyle);

nco_bool
nco_sph_pnt_in_poly(double **sP, int n, double *pControl, double *pVertex);

void
nco_sph_set_domain(double lon_min_rad, double lon_max_rad, double lat_min_rad, double lat_max_rad);

void
nco_sph_add_lonlat(double *ds);

int
nco_sph_mk_control(poly_sct *sP, double* pControl  ); /* make a control point that is outside polygon */


/*------------------------ nco_geo functions these manimpulate lat & lon  ----------------------------------*/
void
nco_geo_sph_2_lonlat(double *a, double *lon, double *lat, nco_bool bDeg);

void
nco_geo_lonlat_2_sph(double lon, double lat, double *b, nco_bool bDeg);

double
nco_geo_lat_correct(double lat1, double lon1, double lon2);

void
nco_geo_get_lat_correct(double lon1, double lat1, double lon2, double lat2, double *dp_min, double *dp_max, nco_bool bDeg);


#ifdef __cplusplus
} /* end extern "C" */
#endif /* !__cplusplus */

#endif /* NCO_SPH_H */
