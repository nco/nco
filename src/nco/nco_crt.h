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
#include "nco_poly.h"    /* poly sct stuff */


/* Dimension of points */

#define DSIGMA 1.0e-14d

/* define minimium area in AreaSign (cross-product) */
#define DAREA  1.0e-28d  

#define VP_MAX    1000            /* Max # of pts in polygon */

#define ARC_MIN_LENGTH (1.0e-20d)


/* if true then longitude 0-360 */
/* we need this to convert 3D back to 2D */
#define IS_LON_360 (1)

#define NBR_CRT (2)
#define DEBUG_CRT (1)


#ifdef __cplusplus
/* Use C-bindings so C++-compiled and C-compiled libraries are compatible */
extern "C" {
#endif /* !__cplusplus */



typedef enum { Pin, Qin, Unknown } tInFlag;


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

const char * prnInFlag(tInFlag in);


/*-------------------------------------------------------------------*/



#ifdef __cplusplus
} /* end extern "C" */
#endif /* !__cplusplus */

#endif /* NCO_CRT_H */
