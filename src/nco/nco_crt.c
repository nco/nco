/*
This code is described in "Computational Geometry in C" (Second Edition),
Chapter 7.  It is not written to be comprehensible without the
explanation in that book.

Written by Joseph O'Rourke.
Last modified: December 1997
Questions to orourke@cs.smith.edu.
--------------------------------------------------------------------
This code is Copyright 1997 by Joseph O'Rourke.  It may be freely
redistributed in its entirety provided that this copyright notice is
not removed.
--------------------------------------------------------------------
*/

#include "nco_crt.h"

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
  int lcl_dbg=0;
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



  tInFlag inflag = Unknown; /* {Pin, Qin, Unknown}: which inside */

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
      (void)fprintf(stdout, "%s: cross=%d, aHB=%d, bHA=%d code = %c\n", nco_prg_nm_get(),cross, aHB, bHA, code );

    if ( code == '1' || code == 'v' ) {
      if ( inflag == Unknown && FirstPoint ) {
        aa = 0;
        ba = 0;
        FirstPoint = False ;
        nco_crt_adi(p0, p);
        nco_crt_add_pnt(R->shp, r, p0);
      }

      inflag = ( aHB >0 ? Pin : bHA >0 ? Qin : inflag );

      nco_crt_add_pnt(R->shp, r, p);

      if(DEBUG_CRT)
        (void)fprintf(stdout, "%s: InOut sets inflag=%d\n", nco_prg_nm_get(),  inflag);
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
        (void)fprintf(stdout, "%s: P and Q are disjoint\n", nco_prg_nm_get());

      return EXIT_FAILURE;
    }
      /* Special case: A & B collinear. */
    else if ( (cross == 0) && ( aHB == 0) && ( bHA == 0 ) )
    {
      /* Advance but do not output point. */
      if ( inflag == Pin )
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
        if( inflag == Pin ) nco_crt_add_pnt(R->shp, r, P->shp[a]);

        a++; aa++;

      }
      else
      {
        // b = Advance( b, &ba, m, inflag == Qin, Q[b] );
        if( inflag == Qin) nco_crt_add_pnt(R->shp, r, Q->shp[b]);

        b++; ba++;
      }
    }

    else /* if ( cross < 0 ) */
    {
      if ( aHB > 0)
      {
        //b = Advance( b, &ba, m, inflag == Qin, Q[b] );
        if(inflag == Qin ) nco_crt_add_pnt(R->shp, r, Q->shp[b]);

        b++; ba++;
      }
      else
      {
        //a = Advance( a, &aa, n, inflag == Pin, P[a] );
        if( inflag == Pin ) nco_crt_add_pnt(R->shp, r, P->shp[a]);

        a++; aa++;
      }
    }


    /* normalize counters */
    a%=n;
    b%=m;

    if(DEBUG_CRT)
      (void)fprintf(stdout, "%s: Before Advances:a=%d, b=%d; aa=%d, ba=%d; inflag=%d\n", nco_prg_nm_get(),   a, b, aa, ba, inflag);


    /* Quit when both adv. indices have cycled, or one has cycled twice. */
  } while ( ((aa < n) || (ba < m)) && (aa < 2*n) && (ba < 2*m) );

  if ( !FirstPoint )
  {
    if(DEBUG_CRT)
      (void)fprintf(stdout, "%s: no points output\n", nco_prg_nm_get());

    return EXIT_FAILURE;

  }

  /* Deal with special cases: not implemented. */
  if ( inflag == Unknown)
  {

    if(DEBUG_CRT)
      (void)fprintf(stdout, "The boundaries of P and Q do not cross.\n", nco_prg_nm_get());

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


  /* only add  point if its distance from  from previous point is more than DSIGMA */
  if ( *r == 0  ||  (  pow( (R[*r-1][0] - P[0]),2.0 )  + pow( (R[*r-1][1] - P[1]),2.0) > DAREA  ) )
  {

    R[*r][0] = P[0];
    R[*r][1] = P[1];
    (*r)++;

  }

}



const char * prnInFlag(tInFlag in)
{
  if(in == Pin)
    return "Pin";
  else if(in == Qin)
    return "Qin";
  else if(in == Unknown)
    return "Unknown";
}
























