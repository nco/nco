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

#include "nco_sph.h"

/* global variables for latitude, longitude in RADIANS
   these may be set in nco_poly.c or
   should be safe with OPenMP  ? */

static double LAT_MIN_RAD;
static double LAT_MAX_RAD;

static double LON_MIN_RAD;
static double LON_MAX_RAD;






void nco_sph_prn(double **sR, int r, int istyle)
{
  int idx;


  printf("\nSpherical Polygon\n");

  for( idx = 0; idx < r; idx++ )
  for( idx = 0; idx < r; idx++ )
     nco_sph_prn_pnt(">", sR[idx], istyle, True);
    //printf("%20.14f %20.14f\n", sR[idx][0], sR[idx][1]);

  printf("End Polygon\n");


}


/* spherical functions */
int nco_sph_intersect(poly_sct *P, poly_sct *Q, poly_sct *R, int *r)
{

   nco_bool flg_dbg=True;

   nco_bool qpFace = False;
   nco_bool pqFace = False;
   nco_bool isGeared = False;

   int numIntersect=0;

   int n;
   int m;

   int a = 0, a1 = 0, aa=0;
   int b = 0, b1 = 0, bb=0;


   int ipqLHS = 0;
   int ip1qLHS = 0 ;
   int iqpLHS = 0;
   int iq1pLHS = 0 ;

   double nx1;
   double nx2;
   double nx3;


   char code='0';

   double Pcross[NBR_SPH];
   double Qcross[NBR_SPH];
   double Xcross[NBR_SPH];

   double p[NBR_SPH];
   double q[NBR_SPH];

   tInFlag inflag= Unknown;

   n=P->crn_nbr;
   m=Q->crn_nbr;

   if(DEBUG_SPH)
      fprintf(stdout, "%s: just entered %s()\n", nco_prg_nm_get(), __FUNCTION__ );


   do{


      a1 = (a + n - 1) % n;
      b1 = (b + m - 1) % m;



      nx1= nco_sph_cross(P->shp[a1], P->shp[a], Pcross);
      nx2= nco_sph_cross(Q->shp[b1], Q->shp[b], Qcross);

      nx3= nco_sph_cross(Pcross, Qcross, Xcross);


      ipqLHS = nco_sph_lhs(P->shp[a], Qcross);
      ip1qLHS = nco_sph_lhs(P->shp[a1], Qcross);


      /* imply rules facing if 0 */

      if(ipqLHS==0 && ip1qLHS!=0)
         ipqLHS=ip1qLHS*-1;
      else if( ipqLHS != 0 && ip1qLHS == 0 )
         ip1qLHS=ipqLHS*-1;


      iqpLHS = nco_sph_lhs(Q->shp[b], Pcross);
      iq1pLHS = nco_sph_lhs(Q->shp[b1], Pcross);

      /* imply rules facing if 0 */

      if(iqpLHS == 0 && iq1pLHS != 0)
         iqpLHS=iq1pLHS*-1;
      else if(iqpLHS != 0 && iq1pLHS == 0)
         iq1pLHS=iqpLHS*-1;


      /* now calculate face rules */
      qpFace = nco_sph_face(ip1qLHS, ipqLHS, iqpLHS);
      pqFace = nco_sph_face(iq1pLHS, iqpLHS, ipqLHS);

      /* Xcross product near zero !! so make it zero*/
      if(nx3< 1.0e-10)
      {
         ip1qLHS=0;
         ipqLHS=0;
         iq1pLHS=0;
         iqpLHS=0;
         qpFace=0;
         pqFace=0;
      }



      if( isGeared == False)
      {
         if(  (ipqLHS == 1 && iqpLHS == 1) ||  ( qpFace && pqFace )     )
         {
            aa++;a++;
         }
         else
         {
            isGeared = True;
         }
      }

      if(isGeared) {
         code = nco_sph_seg_int(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q);


         if (code == '1' || code == 'e') {
            if(DEBUG_SPH)
               nco_sph_prn_pnt("(): intersect", p, 3, True);

            nco_sph_add_pnt(R->shp, r, p);

            /*
            if(code=='e')
              sAddPoint(R, r, q);
            */

            if (numIntersect++ == 0) {
               /* reset counters */
               aa = 0;
               bb = 0;
            }



            inflag = ( ipqLHS ==1 ? Pin : iqpLHS ==1 ? Qin : inflag );


            if(DEBUG_SPH)
               printf("%%InOut sets inflag=%s\n", prnInFlag(inflag));

         }

         if(DEBUG_SPH)
            printf("numIntersect=%d code=%c (ipqLHS=%d, ip1qLHS=%d), (iqpLHS=%d, iq1pLHS=%d), (qpFace=%d pqFace=%d)\n",numIntersect, code, ipqLHS, ip1qLHS,  iqpLHS,iq1pLHS, qpFace,pqFace);



         if (qpFace && pqFace)  {

            /* Advance either P or Q which has previously arrived ? */
            if(inflag == Pin) nco_sph_add_pnt(R->shp,r, P->shp[a]);

            aa++;a++;


         } else if (qpFace) {
            if(inflag == Qin) nco_sph_add_pnt(R->shp,r, Q->shp[b]);

            bb++;b++;


            /* advance q */
         } else if (pqFace) {
            /* advance p */
            if(inflag == Pin) nco_sph_add_pnt(R->shp,r,P->shp[a]);

            aa++;a++;

         } else if (iqpLHS == -1) {
            /* advance q */
            //if(inflag== Qin) sAddPoint(R,r,Q->shp[b]);
            bb++;b++;

            /* cross product zero  */
         } else if( ipqLHS==0 && ip1qLHS==0 && iq1pLHS ==0 && iqpLHS ==0   ){
            if(inflag==Pin)
            {bb++;b++;}
            else
            {aa++;a++;}

         }



         else {
            /* catch all */
            if(inflag==Pin) nco_sph_add_pnt(R->shp,r,P->shp[a]);
            aa++;a++;

         }

      }

      a%=n;
      b%=m;

      if(DEBUG_SPH)
         fprintf(stdout, "\ndebug isGeared=%d a=%d aa=%d b=%d bb=%d \n",isGeared, a, aa, b, bb);

      /* quick exit if current point is same a First point  - nb an exact match ?*/
      if( *r >3 &&  R->shp[0][3]==R->shp[*r-1][3] && R->shp[0][4]==R->shp[*r-1][4] )
      {
         --*r;
         break;
      }


   } while ( ((aa < n) || (bb < m)) && (aa < 2*n) && (bb < 2*m) );

   return EXIT_SUCCESS;

}





char  nco_sph_seg_int(double *a, double *b, double *c, double *d, double *p, double *q)
{
   int flg_sx=0;

   double nx1;
   double nx2;
   double nx3;

   double darc;

   double  Pcross[NBR_SPH]={0};
   double  Qcross[NBR_SPH]={0};
   double  Icross[NBR_SPH]={0};



   if(flg_sx) {
      nx1= nco_sph_sxcross(a, b, Pcross);
      nx2= nco_sph_sxcross(c, d, Qcross);

     nco_sph_add_lonlat(Pcross);
     nco_sph_add_lonlat(Qcross);

      nx3= nco_sph_cross(Pcross, Qcross, Icross);
     nco_sph_add_lonlat(Icross);
   }
   else
   {
      nx1= nco_sph_cross(a, b, Pcross);
      nx2= nco_sph_cross(c, d, Qcross);

      nx3= nco_sph_cross(Pcross, Qcross, Icross);
     nco_sph_add_lonlat(Icross);
   }

   darc=atan(nx3);

   if(DEBUG_SPH) {
      nco_sph_prn_pnt("nco_sph_seg_int(): intersection", Icross, 3, True);
      printf("%s(): ||Pcross||=%.20g ||Qcross||=%.20g ||Icross||=%.20g arc=%.20g\n",__FUNCTION__,  nx1, nx2, nx3, darc);
   }

   /* Icross is zero, should really have a range rather than an explicit zero */
   if( nx3 < 1.0e-15)
      return nco_sph_parallel(a, b, c, d, p, q);


   if(nco_sph_lonlat_between(a, b, Icross) && nco_sph_lonlat_between(c, d, Icross) )
   {
      memcpy(p,Icross, sizeof(double)*NBR_SPH);
      return '1';
   }

   /* try antipodal point */
   Icross[0]*= -1.0;
   Icross[1]*= -1.0;
   Icross[2]*= -1.0;

   nco_sph_add_lonlat(Icross);

   if(nco_sph_lonlat_between(a, b, Icross) && nco_sph_lonlat_between(c, d, Icross) )
   {

      memcpy(p,Icross, sizeof(double)*NBR_SPH);
      return '1';
   }

   return '0';





}


/* takes a point and a cross product representing the normal to the arc plane */
/* returns 1 if point on LHS of arc plane */
/* returns -1 if point on RHS of arc plane */
/* return 0 if point on the arc - (given suitable tolerances ) */
int nco_sph_lhs(double *Pi, double *Qi)
{
   double ds;

   ds= nco_sph_dot(Pi, Qi);

   if(ds  > 0.0)
      return 1;
   else if(ds <0.0)
      return -1;
   else
      return 0;


   /*
   ds=acos( sDot(Pi,Qcross) );

   if( ds < M_PI_2 - ARC_MIN_LENGTH )
     return 1;
   else if ( ds > M_PI_2 + ARC_MIN_LENGTH )
     return -1;
   else
     return 0;

   */
}

/* implement face rules */
nco_bool nco_sph_face(int iLHS, int iRHS, int jRHS)
{
   if( iLHS == 1 && iRHS == -1 && jRHS == -1 )
      return True;

   if( iLHS == -1 && iRHS == 1 && jRHS == 1  )
      return True;

   return False;


}



double  nco_sph_dot(double *a, double *b)
{
   int idx;
   double sum=0.0;

   for(idx=0; idx<3; idx++)
      sum+=a[idx]*b[idx];

   return sum;


}

double  nco_sph_cross(double *a, double *b, double *c)
{
   //
   double n1;

   c[0]=a[1]*b[2]-a[2]*b[1];
   c[1]=a[2]*b[0]-a[0]*b[2];
   c[2]=a[0]*b[1]-a[1]*b[0];

   // normalize vector
   n1=sqrt( c[0]*c[0]+c[1]*c[1] + c[2]*c[2] );

   if( n1 >  0.0 && n1 != 1.0  )
   {
      c[0] /= n1;
      c[1] /= n1;
      c[2] /= n1;
   }

   if(DEBUG_SPH)
      printf("%s(): n1=%f (%f, %f %f)\n",__FUNCTION__, n1, c[0],c[1], c[2]);

   return n1;

}

double nco_sph_rad(double *a){
  double n1;

  n1=sqrt( a[0]*a[0]+a[1]*a[1] + a[2]*a[2] );

  return n1;
}


/* new method for calculating cross product */
double nco_sph_sxcross(double *a, double *b, double *c)
{
   double n1;
   double lon1;
   double lon2;

   double lat1;
   double lat2;

   lon1=a[3] * M_PI /180.0;
   lat1=a[4] * M_PI /180.0;

   lon2=b[3] * M_PI /180.0;
   lat2=b[4] * M_PI /180.0;



   c[0] =   sin(lat1+lat2) * cos( (lon1+lon2) / 2.0) * sin( (lon1-lon2)/2.0)
            - sin(lat1-lat2) * sin ((lon1+lon2) / 2.0) * cos( (lon1-lon2)/2.0);

   c[1] =   sin(lat1+lat2) * sin( (lon1+lon2) / 2.0) * sin( (lon1-lon2)/2.0)
            + sin(lat1-lat2) * cos ((lon1+lon2) / 2.0) * cos( (lon1-lon2)/2.0);



   c[2]=cos(lat1) * cos(lat2) * sin(lon2-lon1);


   // normalize vector
   n1=sqrt( c[0]*c[0]+c[1]*c[1] + c[2]*c[2] );

   if( n1 != 0.0 && n1 !=1.0  )
   {
      c[0] /= n1;
      c[1] /= n1;
      c[2] /= n1;
   }

   if(DEBUG_SPH)
      printf("sxCross(): n1=%f (%f, %f %f)\n", n1, c[0],c[1], c[2]);

   return n1;

}


void  nco_sph_adi(double *a, double *b)
{
   (void)memcpy(a,b, sizeof(double)* NBR_SPH);
}






void nco_sph_add_pnt(double **R, int *r, double *P)
{

   double delta;

   delta = ( *r==0 ? 0.0 :   2.0 *asin(    sqrt( pow( R[*r-1][0] - P[0],2 ) + pow( R[*r-1][1] - P[1],2 ) + pow( R[*r-1][2] - P[2],2 )  ) /2.0) );

   if(DEBUG_SPH)
      nco_sph_prn_pnt("aAddPoint():", P, 3, True);



   /* only add  point if its distinct from previous point */
   if ( *r==0 ||  delta > ARC_MIN_LENGTH_RAD )
   {

      memcpy(R[*r], P, sizeof(double)*NBR_SPH);
      (*r)++;
   }


}


nco_bool nco_sph_between(double a, double b, double x)
{

   nco_bool sdiff=False;

   if(DEBUG_SPH)
      printf("iBetween(): a=%.20f, b=%.20f, x=%.20f\n", a, b, x);

   if(fabs(b-a) < DSIGMA  )
      sdiff=True;
   else
      sdiff=False;

   if(sdiff) {
      if (fabs(x - a) < DSIGMA || fabs(x - b) < DSIGMA)
         return True;
      else
         return False;
   }

   if(  b >a &&  x>= a && x<=b  || b<a && x>=b && x<=a    )
      return True;
   else
      return False;

}





/* use crt coords to check bounds */
nco_bool nco_sph_lonlat_between(double *a, double *b, double *x)
{

   /* working in radians here */
   nco_bool bDeg=False;

   double lat_min;
   double lat_max;

   if(nco_sph_between(a[3], b[3], x[3]) == False )
      return False;

   /* special lat check */
   //getLatCorrect(a,b, &lat_min,&lat_max);
  nco_geo_get_lat_correct(a[3], a[4], b[3], b[4], &lat_min, &lat_max, bDeg);


   if(DEBUG_SPH)
      printf("sBetween(): lat_min=%f lat_max=%f lat=%f\n", lat_min, lat_max, x[4]);

   if( x[4]>=lat_min && x[4]<=lat_max )
      return True;
   else
      return False;

   return False;


}

nco_bool sxBetween(double *a, double *b, double *c)
{

   if ( a[3] != b[3] )
      return (  c[3] >= a[3] && c[3] <=b[3] ||  c[3] <= a[3] && c[3] >= b[3] ) ;
   else
      return (  c[4] >= a[4] && c[4] <=b[4] ||  c[4] <= a[4] && c[4] >= b[4] ) ;


   /*
   if ( a[3] != b[3] )
     return (  a[3] <= c[3] && b[3] >= c[3] ||  a[3] >= c[3] && b[3] <= c[3] ) ;
   else
     return (  a[4] <= c[4] && b[4] >=c[4]   ||  a[4] >= c[4] && b[4] <= c[4] ) ;
   */

}


char nco_sph_parallel(double *a, double *b, double *c, double *d, double *p, double *q)
{

   char code='0';
   char *ptype="none";

   if( sxBetween( a, b, c ) && sxBetween( a, b, d ) ) {
      nco_sph_adi(p, c);
      nco_sph_adi(q, d);
      ptype="abc-abd";
      code= 'e';
   }
   else if( sxBetween( c, d, a ) && sxBetween( c, d, b ) ) {
      nco_sph_adi(p, a);
      nco_sph_adi(q, b);
      ptype="cda-cdb";
      code= 'e';
   }
   else if( sxBetween( a, b, c ) && sxBetween( c, d, b ) ) {
      nco_sph_adi(p, c);
      nco_sph_adi(q, b);
      ptype="abc-cdb";
      code= 'e';
   }
   else if( sxBetween( a, b, c ) && sxBetween( c, d, a ) ) {
      nco_sph_adi(p, c);
      nco_sph_adi(q, a);
      ptype="abc-cda";
      code= 'e';
   }
   else if( sxBetween( a, b, d ) && sxBetween( c, d, b ) ) {
      nco_sph_adi(p, d);
      nco_sph_adi(q, b);
      ptype="abd-cdb";
      code= 'e';
   }
   else if( sxBetween( a, b, d ) && sxBetween( c, d, a ) ) {
      nco_sph_adi(p, d);
      nco_sph_adi(q, a);
      ptype="abd-cda";
      code= 'e';
   }

   if(DEBUG_SPH)
      printf("sParallelDouble(): code=%c type=%s\n", code, ptype);

   return code;
}



void nco_sph_prn_pnt(const char *sMsg, double *p, int style, nco_bool bRet)
{

   printf("%s ", sMsg);

   switch(style)
   {
      case 0:
      default:
         printf( "(dx=%.20f, dy=%.20f, dz=%.20f), (lon=%.20f,lat=%.20f)",p[0], p[1], p[2], p[3], p[4] );
       break;

      case 1:
         printf( "(dx=%.20f, dy=%.20f, dz=%.20f)",p[0], p[1], p[2] );
       break;

      case 2:
         printf( "(lon=%.20f,lat=%.20f)",p[3], p[4] );
       break;

      case 3:
         printf( "(lon=%.20f,lat=%.20f)",p[3] *180.0/M_PI,  p[4]*180/M_PI );
       break;

      case 4:
         printf( "(dx=%.20f, dy=%.20f, dz=%.20f), (lon=%.20f,lat=%.20f)",p[0], p[1], p[2], p[3] *180.0/M_PI,  p[4]*180/M_PI);
       break;

      case 5:
         printf( "(dx=%f, dy=%f, dz=%f), (lon=%f,lat=%f)",p[0], p[1], p[2], p[3] *180.0/M_PI,  p[4]*180/M_PI);
       break;



   }

   if(bRet)
      printf("\n");
   else
      printf(" * ");

}

nco_bool nco_sph_is_convex(double **sP, int np)
{

int idx;
int idx_pre;
int idx_nex;


double n1;
double n2;

double dp;
double theta;
double rad1=1.0;
double rad=1.0;

double  aCross[NBR_SPH];
double  bCross[NBR_SPH];

for(idx=0; idx<np;idx++)
{
  idx_pre=(idx + np -1)% np;
  idx_nex=(idx + np +1)% np;

  n1= nco_sph_sxcross(sP[idx], sP[idx_pre], aCross);
  n2= nco_sph_sxcross(sP[idx], sP[idx_nex], bCross);

  //rad1 = sRadius(aCross);
  //rad  = sRadius(bCross);
  dp= nco_sph_dot(aCross, bCross);


  // dp=sDot(sP[idx1], sP[idx]) / rad1 /rad;
  theta=acos(dp);

  if(DEBUG_SPH)
    printf("sConvex():, %d angle=%f n1=%.15g n2=%.15g\n", idx, theta*180.0/M_PI, n1, n2);

  if(theta > 2.0*M_PI   )
     return False;

}

return True;


}


/* works by counting the number of intersections of the
   line (pControl, pVertex) and each edge in sP
   pControl is chosen so that it is OUTSIDE sP
 */
nco_bool nco_sph_pnt_in_poly(double **sP, int n, double *pControl, double *pVertex)
{

  char code;
  int idx;
  int idx1=0;
  int numIntersect=0;

  double  p[NBR_SPH];
  double  q[NBR_SPH];


  /* count number of intersections */
  for(idx=0; idx< n ; idx++)
  {
    idx1=(idx+n -1) % n ;

    code= nco_sph_seg_int(sP[idx1], sP[idx], pControl, pVertex, p, q);

    if(code=='1' || code=='v' || code == 'e')
      numIntersect++;


  }

  /* for any polygon (convex or concave)
    an odd  number of crossings means that the point is inside
    while an even number means that it is outside */

  return (numIntersect % 2  );


}




/* set static globals */
void nco_sph_set_limits(double lon_min_rad, double lon_max_rad, double lat_min_rad, double lat_max_rad   )
{

  LON_MIN_RAD=lon_min_rad;
  LON_MAX_RAD=lon_max_rad;

  LAT_MIN_RAD=lat_min_rad;
  LAT_MAX_RAD=lat_max_rad;

  return;

}


void
nco_sph_add_lonlat(double *ds)
{
 nco_bool bDeg=False;

 nco_geo_sph_2_lonlat(ds, &ds[3], &ds[4], bDeg);

}



/*------------------------ nco_geo functions manipulate lat & lon  ----------------------------------*/






void nco_geo_get_lat_correct(double lon1, double lat1, double lon2, double lat2, double *dp_min, double *dp_max,
                             nco_bool bDeg)
{


   if( lat2 >lat1 )
   {
      double dswp;

      dswp=lat1;
      lat1=lat2;
      lat2=dswp;

   }

   if(bDeg)
   {
      lat1 *= M_PI / 180.0;
      lat2 *= M_PI / 180.0;
      lon1 *= M_PI / 180.0;
      lon2 *= M_PI / 180.0;
   }




   /* lat1 & lat2 >0.0 */
   if( lat1>0.0 && lat2 >=0.0)
   {
      *dp_max = nco_geo_lat_correct(lat1, lon1, lon2);
      *dp_min = lat2;
   }
   else if( lat1 <= 0.0 && lat2<0.0 )
   {
      *dp_max = lat1;
      *dp_min = nco_geo_lat_correct(lat2, lon1, lon2);
   }

   else if( lat1 >0.0 && lat2  < 0.0)
   {
      *dp_max= nco_geo_lat_correct(lat1, lon1, lon2);
      *dp_min= nco_geo_lat_correct(lat2, lon1, lon2);

   }
   else
   {
      *dp_max=0.0;
      *dp_min=0.0;

   }

   /* convert back to degrees */
   if(bDeg)
   {
      *dp_max *= 180.0 / M_PI;
      *dp_min *= 180.0 / M_PI;
   }

   return;



}


/* assume latitude -90,90 */
double nco_geo_lat_correct(double lat1, double lon1, double lon2)
{

   double dp;

   if( lon1 == lon2  || lat1==0.0 || lat1 == M_PI /2.0   || lat1 == -M_PI/2.0  )
      return lat1;

   //lat1=lat1*M_PI / 180.0;

   dp= tan(lat1) / cos ( lon2-lon1 ) ;

   dp=atan(dp);


   return dp;


}

void nco_geo_lonlat_2_sph(double lon, double lat, double *b)
{
   lon *= M_PI / 180.0;
   lat *= M_PI / 180.0;


   b[0] = cos(lat) * cos(lon);
   b[1] = cos(lat) * sin(lon);
   b[2] = sin(lat);

   /* lat lon - we need this for bounding box */
   b[3] = lon;
   b[4] = lat;

}


void  nco_geo_sph_2_lonlat(double *a, double *lon, double *lat, nco_bool bDeg)
{

   /* nb this returns range (-180, 180) */
   *lon = atan2(a[1],a[0]) ;
   if( *lon < 0.0 && IS_LON_360)
      *lon+= (M_PI*2);

   // b[1]= asin(a[2]) * 180.0 /M_PI;
   *lat=atan2( a[2], sqrt( a[0]*a[0]+a[1]*a[1] ) ) ;

   /* convert to degrees if required */
   if(bDeg)
   {
      *lon*=(180.0 / M_PI );
      *lat*=(180.0 / M_PI );

   }

   return;
}