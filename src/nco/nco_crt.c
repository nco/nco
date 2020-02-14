/* $Header$ */

/* Purpose: Cartesian polygon coordinates and intersections */

/* This file includes BSD-licensed code whose copyright is held by another author
   The copyright owner and license terms for the NCO modifications to that code are
   Copyright (C) 2018--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* This copyright statement and license terms for modification and redistribution 
   of the original code were agreed to by the original author, Joseph O'Rourke, on 20190517:

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

#include "nco_crt.h" /* Cartesian geometry intersections */

/* global variables for latitude, longitude in RADIANS
   these may be set in nco_poly.c or
   should be safe with OPenMP  ? */

static double LAT_MIN;
static double LAT_MAX;

static double LON_MIN;
static double LON_MAX;

/*---------------------------------------------------------------------
---------------------------------------------------------------------*/
int  nco_crt_intersect(poly_sct *P, poly_sct *Q, poly_sct *R, int *r)
{
  nco_bool FirstPoint=True;    /*s this the first point? (used to initialize).*/
  int     a=0, b=0;            /* indices on P and Q (resp.) */
  int     a1, b1;              /* a-1, b-1 (resp.) */
  int     aa=0, ba=0;          /* # advances on a & b indices (after 1st inter.) */
  int     cross;               /* sign of z-component of A x B */
  int     bHA, aHB;            /* b in H(A); a in H(b). */
  int     code;                /* SegSegInt return code. */

  int n;
  int m;

  double A[NBR_CRT];
  double B[NBR_CRT];                /* directed edges on P and Q (resp.) */
  double Origin[NBR_CRT] = {0.0,0.0};  /* (0,0) */
  double p0[NBR_CRT];                  /* The first point. */
  double p[NBR_CRT];                   /* double point of intersection */
  double q[NBR_CRT];                   /* second point of intersection */



  poly_vrl_flg_enm inflag = poly_vrl_unk; /* {Pin, Qin, Unknown}: which inside */

  n=P->crn_nbr;
  m=Q->crn_nbr;

  do {


    /* Computations of key variables. */
    a1 = (a + n - 1) % n;
    b1 = (b + m - 1) % m;

    nco_crt_sub_vec(P->shp[a], P->shp[a1], A);
    nco_crt_sub_vec(Q->shp[b], Q->shp[b1], B);
    cross = nco_crt_cross_sgn(Origin, A, B);
    aHB   = nco_crt_cross_sgn(Q->shp[b1], Q->shp[b], P->shp[a]);
    bHA   = nco_crt_cross_sgn(P->shp[a1], P->shp[a], Q->shp[b]);

    /* If A & B intersect, update inflag. */
    code = nco_crt_seg_int(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q);

    if(DEBUG_CRT)
      (void)fprintf(stderr, "%s: cross=%d, aHB=%d, bHA=%d code = %c\n", nco_prg_nm_get(),cross, aHB, bHA, code );

    if ( code == '1' || code == 'v' ) {
      if ( inflag == poly_vrl_unk && FirstPoint ) {
        aa = 0;
        ba = 0;
        FirstPoint = False ;
        nco_crt_adi(p0, p);
        nco_crt_add_pnt(R->shp, r, p0);
      }

      inflag = ( aHB >0 ? poly_vrl_pin : bHA >0 ? poly_vrl_qin : inflag );

      nco_crt_add_pnt(R->shp, r, p);

      if(DEBUG_CRT)
        (void)fprintf(stderr, "%s: InOut sets inflag=%s\n", nco_prg_nm_get(), nco_poly_vrl_flg_sng_get(inflag));
    }

    /*-----Advance rules-----*/

    /* Special case: A & B overlap and oppositely oriented. */
    if ( code == 'e' && nco_crt_dot(A, B) < 0  )
    {
      nco_crt_add_pnt(R->shp, r, p);
      nco_crt_add_pnt(R->shp, r, q);
      return EXIT_FAILURE;
    }

    /* Special case: A & B parallel and separated. */
    if ( (cross == 0) && ( aHB < 0) && ( bHA < 0 ) )
    {

      if(DEBUG_CRT)
        (void)fprintf(stderr, "%s: P and Q are disjoint\n", nco_prg_nm_get());

      return EXIT_FAILURE;
    }
      /* Special case: A & B collinear. */
    else if ( (cross == 0) && ( aHB == 0) && ( bHA == 0 ) )
    {
      /* Advance but do not output point. */
      if ( inflag == poly_vrl_pin )
      {
        // b = Advance( b, &ba, m, inflag == Qin, Q[b] );
        b++; ba++;
      }
      else
      {
        //a = Advance( a, &aa, n, inflag == Pin, P[a] );
        a++; aa++;
      }
    }
      /* Generic cases. */
    else if ( cross >= 0 )
    {
      if ( bHA > 0)
      {   //a = Advance( a, &aa, n, inflag == Pin, P[a] );
        if( inflag == poly_vrl_pin ) nco_crt_add_pnt(R->shp, r, P->shp[a]);

        a++; aa++;

      }
      else
      {
        // b = Advance( b, &ba, m, inflag == Qin, Q[b] );
        if( inflag == poly_vrl_qin) nco_crt_add_pnt(R->shp, r, Q->shp[b]);

        b++; ba++;
      }
    }

    else /* if ( cross < 0 ) */
    {
      if ( aHB > 0)
      {
        //b = Advance( b, &ba, m, inflag == Qin, Q[b] );
        if(inflag == poly_vrl_qin ) nco_crt_add_pnt(R->shp, r, Q->shp[b]);

        b++; ba++;
      }
      else
      {
        //a = Advance( a, &aa, n, inflag == Pin, P[a] );
        if( inflag == poly_vrl_pin ) nco_crt_add_pnt(R->shp, r, P->shp[a]);

        a++; aa++;
      }
    }


    /* normalize counters */
    a%=n;
    b%=m;

    if(DEBUG_CRT)
      (void)fprintf(stderr, "%s: Before Advances:a=%d, b=%d; aa=%d, ba=%d; inflag=%s\n", nco_prg_nm_get(),   a, b, aa, ba,
                    nco_poly_vrl_flg_sng_get(inflag));


    /* Quit when both adv. indices have cycled, or one has cycled twice. */
  } while ( ((aa < n) || (ba < m)) && (aa < 2*n) && (ba < 2*m) );

  if ( FirstPoint )
  {
    if(DEBUG_CRT)
      (void)fprintf(stderr, "%s: no points output\n", nco_prg_nm_get());

    return EXIT_FAILURE;

  }

  /* Deal with special cases: not implemented. */
  if ( inflag == poly_vrl_unk)
  {

    if(DEBUG_CRT)
      (void)fprintf(stderr, "%s():The boundaries of P and Q do not cross.\n", nco_prg_nm_get());

    return EXIT_FAILURE;



  }

  return EXIT_SUCCESS;

}


/*---------------------------------------------------------------------
SegSegInt: Finds the point of intersection p between two closed
segments ab and cd.  Returns p and a char with the following meaning:
   'e': The segments collinearly overlap, sharing a point.
   'v': An endpoint (vertex) of one segment is on the other segment,
        but 'e' doesn't hold.
   '1': The segments intersect properly (i.e., they share a point and
        neither 'v' nor 'e' holds).
   '0': The segments do not intersect (i.e., they share no points).
Note that two collinear segments that share just one point, an endpoint
of each, returns 'e' rather than 'v' as one might expect.
---------------------------------------------------------------------*/
char nco_crt_seg_int(double *a, double *b, double *c, double *d, double *p, double *q)
{
   double  s, t;       /* The two parameters of the parametric eqns. */
   double num, denom;  /* Numerator and denoninator of equations. */
   char code = '?';    /* Return char characterizing intersection. */

   /*printf("%%SegSegInt: a,b,c,d: (%d,%d), (%d,%d), (%d,%d), (%d,%d)\n",
	a[0],a[1], b[0],b[1], c[0],c[1], d[0],d[1]);*/

   denom = a[0] * ( d[1] - c[1] ) +
           b[0] * ( c[1] - d[1] ) +
           d[0] * ( b[1] - a[1] ) +
           c[0] * ( a[1] - b[1] );

   /* If denom is zero, then segments are parallel: handle separately. */
   if (denom == 0.0)
      return nco_crt_parallel_int(a, b, c, d, p, q);

   num =    a[0] * ( d[1] - c[1] ) +
            c[0] * ( a[1] - d[1] ) +
            d[0] * ( c[1] - a[1] );
   
   if ( num == 0.0 || num == denom )
     code = 'v';
   
   s = num / denom;
   /*printf("num=%lf, denom=%lf, s=%lf\n", num, denom, s);*/

   num = -( a[0] * ( c[1] - b[1] ) +
            b[0] * ( a[1] - c[1] ) +
            c[0] * ( b[1] - a[1] ) );
   
   if ( num == 0.0 || num == denom )
     code = 'v';
   
   t = num / denom;
   /*printf("num=%lf, denom=%lf, t=%lf\n", num, denom, t);*/

   if(  s >0.0 && s < 1.0  &&  t >0.0 && t < 1.0  )
     code = '1';
   else  if(  s <0.0 || s > 1.0 || t <0.0 || t > 1.0  )
     code = '0';

   p[0] = a[0] + s * ( b[0] - a[0] );
   p[1] = a[1] + s * ( b[1] - a[1] );

   return code;
}
char   nco_crt_parallel_int(double *a, double *b, double *c, double *d, double *p, double *q)
{
/*   
   printf("ParallelInt: a,b,c,d: (%d,%d), (%d,%d), (%d,%d), (%d,%d)\n",
	a[0],a[1], b[0],b[1], c[0],c[1], d[0],d[1]);
*/
  /* Check if collinear */
   if (nco_crt_cross_sgn(a, b, c) == 0  )
      return '0';

   if (nco_crt_between(a, b, c) && nco_crt_between(a, b, d) ) {
     nco_crt_adi(p, c);
     nco_crt_adi(q, d);
      return 'e';
   }
   if (nco_crt_between(c, d, a) && nco_crt_between(c, d, b) ) {
     nco_crt_adi(p, a);
     nco_crt_adi(q, b);
      return 'e';
   }
   if (nco_crt_between(a, b, c) && nco_crt_between(c, d, b) ) {
     nco_crt_adi(p, c);
     nco_crt_adi(q, b);
      return 'e';
   }
   if (nco_crt_between(a, b, c) && nco_crt_between(c, d, a) ) {
     nco_crt_adi(p, c);
     nco_crt_adi(q, a);
      return 'e';
   }
   if (nco_crt_between(a, b, d) && nco_crt_between(c, d, b) ) {
     nco_crt_adi(p, d);
     nco_crt_adi(q, b);
      return 'e';
   }
   if (nco_crt_between(a, b, d) && nco_crt_between(c, d, a) ) {
     nco_crt_adi(p, d);
     nco_crt_adi(q, a);
      return 'e';
   }
   return '0';
}

/*---------------------------------------------------------------------
Returns the dot product of the two input vectors.
---------------------------------------------------------------------*/
double  nco_crt_dot(double *a, double *b)
{
    int i;
    double sum = 0.0;

    for( i = 0; i < NBR_CRT; i++ )
       sum += a[i] * b[i];

    return  sum;
}

/*---------------------------------------------------------------------
a - b ==> c.
---------------------------------------------------------------------*/
void nco_crt_sub_vec(double *a, double *b, double *c)
{
   int i;

   for( i = 0; i < NBR_CRT; i++ )
      c[i] = a[i] - b[i];
}


void  nco_crt_adi(double *p, double *a)
{
  p[0]=a[0];
  p[1]=a[1];
}




int nco_crt_cross_sgn(double *a, double *b, double *c)
{
    double area2;

    area2 = ( b[0] - a[0] ) * ( c[1] - a[1] ) -
            ( c[0] - a[0] ) * ( b[1] - a[1] );

    /* The area should be an integer. */
    /*
    if      ( area2 >  0.5 ) return  1;
    else if ( area2 < -0.5 ) return -1;
    else                     return  0;
    */
    if      ( area2 >  DAREA ) return  1;
    else if ( area2 < -DAREA ) return -1;
    else                       return  0;

    
}


/*
   Returns true iff c is strictly to the left of the directed
   line through a to b.
*/
nco_bool nco_crt_left(double *a, double *b, double *c)
{
        return nco_crt_cross_sgn(a, b, c) > 0;
}

nco_bool nco_crt_left_on(double *a, double *b, double *c)
{
        return nco_crt_cross_sgn(a, b, c) >= 0;
}

nco_bool nco_crt_linear(double *a, double *b, double *c)
{
        return nco_crt_cross_sgn(a, b, c) == 0;
}


/*---------------------------------------------------------------------
Returns TRUE iff point c lies on the closed segement ab.
Assumes it is already known that abc are collinear.
---------------------------------------------------------------------*/
nco_bool nco_crt_between(double *a, double *b, double *c)
{
   /* If ab not vertical, check betweenness on x; else on y. */
   if ( a[0] != b[0] )
     return (a[0] <= c[0] && c[0] <= b[0])  || (a[0] >= c[0] && c[0] >= b[0] ) ;
   else
     return (a[1] <= c[1] && c[1] <= b[1]) || (a[1] >= c[1] && c[1] >= b[1] ) ;

}


void nco_crt_add_pnt(double **R, int *r, double *P)
{
  const char fnc_nm[]="nco_crt_add_pnt()";

  /* only add  point if its distance from  from previous point is more than DSIGMA */
  if ( *r == 0  ||  (  pow( (R[*r-1][0] - P[0]),2.0 )  + pow( (R[*r-1][1] - P[1]),2.0) > DAREA  ) )
  {

    R[*r][0] = P[0];
    R[*r][1] = P[1];
    (*r)++;


  }
  if(DEBUG_CRT)
    fprintf(stderr, "%s: (%f, %f)\n", fnc_nm,P[0], P[1]);



}



const char * nco_poly_vrl_flg_sng_get(poly_vrl_flg_enm in)
{
  if(in == poly_vrl_pin)
    return "Pin";
  else if(in == poly_vrl_qin)
    return "Qin";
  else if(in == poly_vrl_unk)
    return "Unknown";

  return "UnKnown";

}


void
nco_crt_set_domain(double lon_min, double lon_max, double lat_min, double lat_max)
{

  LON_MIN=lon_min;
  LON_MAX=lon_max;

  LAT_MIN=lat_min;
  LAT_MAX=lat_max;

  return;
}

int             /* O [nbr] returns number of points of pl_out that are inside pl_in */
nco_crt_poly_in_poly(
poly_sct *pl_in,
poly_sct *pl_out)
{
  int idx=0;
  int sz;
  int cnt_in=0;

  double *lcl_dp_x;
  double *lcl_dp_y;

  lcl_dp_x=(double*)nco_malloc( sizeof(double)*pl_in->crn_nbr);
  lcl_dp_y=(double*)nco_malloc( sizeof(double)*pl_in->crn_nbr);


  sz= pl_out->crn_nbr;

  for(idx=0; idx < sz ; idx++){

    memcpy(lcl_dp_x, pl_in->dp_x, sizeof(double) * pl_in->crn_nbr);
    memcpy(lcl_dp_y, pl_in->dp_y, sizeof(double) * pl_in->crn_nbr);

    if( nco_crt_pnt_in_poly(pl_in->crn_nbr, pl_out->dp_x[idx], pl_out->dp_y[idx], lcl_dp_x, lcl_dp_y)  )
      cnt_in++;
  }
  lcl_dp_x=(double*)nco_free(lcl_dp_x);
  lcl_dp_y=(double*)nco_free(lcl_dp_y);


  return cnt_in;
}


/*******************************************************************************************************/
/*
  Algorithm  to check that point is in polygon.
  for full details please see :
   http://demonstrations.wolfram.com/AnEfficientTestForAPointToBeInAConvexPolygon

  It assumes that the polygon is convex and point order can be clockwise or counterclockwise.
  If area is almost zero then  the point is on a vertex or an edge or in line with an edge but outside polygon.
  Please note that if two contiguous vertices are identical  then this will also make the area zero
*/

/********************************************************************************************************/

nco_bool            /* O [flg] True if point in inside (or on boundary ) of polygon */
nco_crt_pnt_in_poly(
int crn_nbr,
double x_in,
double y_in,
double *lcl_dp_x,
double *lcl_dp_y)
{
  int idx;
  int idx1;
  nco_bool bret=False;
  nco_bool sign=False;
  nco_bool dsign=False;

  double area=0.0;



  /* make (x_in,y_in) as origin */
  for(idx=0 ; idx < crn_nbr ; idx++)
  {
    lcl_dp_x[idx]-=x_in;
    lcl_dp_y[idx]-=y_in;

  }

  for(idx=0 ; idx < crn_nbr ; idx++)
  {
    /* for full explanation of algo please

    */
    idx1=(idx+1)%crn_nbr;
    area=lcl_dp_x[idx1] * lcl_dp_y[idx] - lcl_dp_x[idx] * lcl_dp_y[idx1];

    /* check betweeness need some fabs and limits here */
    if( fabs(area) <= DAREA ){
      if( lcl_dp_x[idx] != lcl_dp_x[idx1] )
        bret = (  (lcl_dp_x[idx]<=0.0 &&  lcl_dp_x[idx1] >=0.0) ||  (lcl_dp_x[idx]>=0.0 && lcl_dp_x[idx1]<=0.0)  );
      else
        bret = (  (lcl_dp_y[idx]<=0.0 &&  lcl_dp_y[idx1] >=0.0) ||  (lcl_dp_y[idx]>=0.0 && lcl_dp_y[idx1]<=0.0)  );

      break;
    }



    dsign=(area>0.0);

    if(idx==0)
      sign=dsign;

    /* we have a sign change so point NOT in Polygon */
    if(dsign != sign)
    { bret=False; break; }

    bret=True;

  }

  return bret;

}





















