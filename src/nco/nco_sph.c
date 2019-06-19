/* $Header$ */

/* Purpose: Spherical polygon coordinates and intersections */

/* This file includes BSD-licensed code whose copyright is held by another author
   The copyright owner and license terms for the NCO modifications to that code are
   Copyright (C) 2018--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   GNU General Public License (GPL) Version 3 with exceptions described in the LICENSE file */

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

#include "nco_sph.h" /* Spherical geometry intersections */

/* global variables for latitude, longitude in RADIANS
   these may be set in nco_poly.c or
   should be safe with OPenMP   */

int DEBUG_SPH=0;

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
int nco_sph_intersect(poly_sct *P, poly_sct *Q, poly_sct *R, int *r, const char *pq_pre)
{
  const char fnc_nm[]="nco_sph_intersect()";

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

   nco_bool isParallel=False;

   double nx1;
   double nx2;
   double nx3;
   double dx1;


   char codes[]="00";

   double Pcross[NBR_SPH];
   double Qcross[NBR_SPH];
   double Xcross[NBR_SPH];

   double p[NBR_SPH];
   double q[NBR_SPH];

   poly_vrl_flg_enm inflag= poly_vrl_unk;

   n=P->crn_nbr;
   m=Q->crn_nbr;

   if(DEBUG_SPH)
      fprintf(stdout, "%s: just entered %s\n", nco_prg_nm_get(), fnc_nm);


   do{


      a1 = (a + n - 1) % n;
      b1 = (b + m - 1) % m;


     /* skip identical points */
     while( nx1= nco_sph_cross(P->shp[a1], P->shp[a], Pcross) <DOT_TOLERANCE   )
     {
       aa++;a++;
       a%=n;

       if(a==a1)
         return EXIT_FAILURE;
     }


     /* skip identical points */
     while(  nx2= nco_sph_cross(Q->shp[b1], Q->shp[b], Qcross) < DOT_TOLERANCE )
     {
       bb++;b++;
       b%=m;
       if(b==b1)
         return EXIT_FAILURE;
     }


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
      dx1=1.0- nco_sph_dot_nm(Pcross,Qcross );

      /* spans parallel but in oposite directions */
      if( fabs(dx1-2.0) < SIGMA_TOLERANCE )
      {
        if(nco_dbg_lvl_get() >= nco_dbg_dev)
          (void)fprintf(stderr, "%s:%s() PARALLEL edges in oposite direction\n", nco_prg_nm_get(), fnc_nm );

        return EXIT_FAILURE;

      }

      /*
      if( dx1  <DOT_TOLERANCE )
      {

         ip1qLHS=0;
         ipqLHS=0;
         iq1pLHS=0;
         iqpLHS=0;
         qpFace=0;
         pqFace=0;
         isParallel=True;

      } else
        isParallel=False;
      */

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





      if(isGeared)
      {

        if(inflag==poly_vrl_unk && pq_pre)
        {
          if(pq_pre[b]=='i')
            inflag=poly_vrl_qin;
          else if(  (pq_pre[b]=='e' || pq_pre[b]=='v') && pq_pre[b1]=='i'  )
            inflag=poly_vrl_qin;


        }



        //if(isParallel)
        if(ipqLHS==0 && ip1qLHS==0 && iqpLHS==0 && iq1pLHS==0)
        {
          char pcode='0';
          poly_vrl_flg_enm lcl_inflag = poly_vrl_unk;

          pcode = nco_sph_seg_parallel(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q, &lcl_inflag);



          if (  !(pcode=='1' && inflag== poly_vrl_unk)  &&   lcl_inflag != poly_vrl_unk   ) {

            inflag = lcl_inflag;

            /* there is a subtle  trick here - a point is "force added" by setting the flags pqFace and qpFace */
            if (pcode == '2')
              nco_sph_add_pnt(R->shp, r, p);

            if (inflag == poly_vrl_pin) {
              pqFace = 1;
              qpFace = 0;

            } else if (inflag == poly_vrl_qin) {
              pqFace = 0;
              qpFace = 1;
            }

            if (numIntersect++ == 0) {
              /* reset counters */
              aa = 0;
              bb = 0;
            }
          }

          if(pcode=='1')
            strcpy(codes,"p0");
          if(pcode=='2')
            strcpy(codes,"pp");

        }

        if(ip1qLHS*ipqLHS==-1 && iq1pLHS*iqpLHS==-1  && nco_sph_seg_int_nw(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q,codes) )
        {
          /* if here then there is some kind of intersection */


          if(inflag== poly_vrl_unk )
          {
            /* no problem with genuine intersection */
            if(codes[0]=='1' && codes[1]=='1' )
              inflag = (ipqLHS == 1 ? poly_vrl_pin : iqpLHS == 1 ? poly_vrl_qin : inflag);

            /*
            else if(  pq_pre &&  (codes[0]=='h' || codes[0] =='t'  ))
            {

              if(pq_pre[b]=='v' || pq_pre[b] == 'e')
                inflag=poly_vrl_pin;

            }

            else if(  pq_pre && codes[0]=='1')
            {

             if(  codes[1]=='h' &&  ( pq_pre[b1]=='v' ||  pq_pre[b1]=='i'  ) )
               inflag=poly_vrl_pin;

            }
            */


          }
          else
            inflag = (ipqLHS == 1 ? poly_vrl_pin : iqpLHS == 1 ? poly_vrl_qin : inflag);

          if(inflag != poly_vrl_unk)
          {
            nco_sph_add_pnt(R->shp, r, p);

            if (numIntersect++ == 0) {
              /* reset counters */
              aa = 0;
              bb = 0;
            }



          }


          if (DEBUG_SPH)
              printf("%%InOut sets inflag=%s\n", nco_poly_vrl_flg_sng_get(inflag));


        }

         if(DEBUG_SPH)
            printf("numIntersect=%d codes=%s (ipqLHS=%d, ip1qLHS=%d), (iqpLHS=%d, iq1pLHS=%d), (qpFace=%d pqFace=%d)\n",numIntersect, codes, ipqLHS, ip1qLHS,  iqpLHS,iq1pLHS, qpFace,pqFace);



         if (qpFace && pqFace)  {

            /* Advance either P or Q which has previously arrived ? */
            if(inflag == poly_vrl_pin) nco_sph_add_pnt(R->shp,r, P->shp[a]);

            aa++;a++;


         } else if (qpFace) {
            if(inflag == poly_vrl_qin) nco_sph_add_pnt(R->shp,r, Q->shp[b]);

            bb++;b++;


            /* advance q */
         } else if (pqFace) {
            /* advance p */
            if(inflag == poly_vrl_pin) nco_sph_add_pnt(R->shp,r,P->shp[a]);

            aa++;a++;

         } else if (iqpLHS == -1) {
            /* advance q */
            //if(inflag== Qin) sAddPoint(R,r,Q->shp[b]);
            bb++;b++;

            /* cross product zero  */
         } else if( ipqLHS==0 && ip1qLHS==0 && iq1pLHS ==0 && iqpLHS ==0   ){
            if(inflag==poly_vrl_pin)
            {bb++;b++;}
            else
            {aa++;a++;}

         }



         else {
            /* catch all */
            if(inflag==poly_vrl_pin) nco_sph_add_pnt(R->shp,r,P->shp[a]);
            aa++;a++;

         }

      }

      a%=n;
      b%=m;
      strcpy(codes,"00");

      if(DEBUG_SPH)
         fprintf(stdout, "\ndebug isGeared=%d a=%d aa=%d b=%d bb=%d \n",isGeared, a, aa, b, bb);

      /* quick exit if current point is same a First point  - nb an exact match ?*/
      //if( *r >3 &&  R->shp[0][3]==R->shp[*r-1][3] && R->shp[0][4]==R->shp[*r-1][4] )
      if( *r >3 &&  1.0 - nco_sph_dot_nm(R->shp[0], R->shp[*r-1]) < DOT_TOLERANCE  )
      {
         --*r;
         break;
      }


   } while ( ((aa < n) || (bb < m)) && (aa < 2*n) && (bb < 2*m) );

   return EXIT_SUCCESS;

}


char  nco_sph_seg_int(double *a, double *b, double *c, double *d, double *p, double *q)
{
  const char fnc_nm[]="nco_sph_seg_int()";

  int flg_sx=0;

  
  double nx1;
  double nx2;
  double nx3;
  double nx_ai;
  double nx_ci;

  double dx_ab;
  double dx_ai;

  double dx_cd;
  double dx_ci;

  double darc;

  double  Pcross[NBR_SPH]={0};
  double  Qcross[NBR_SPH]={0};
  double  Icross[NBR_SPH]={0};
  double   ai[NBR_SPH]={0};
  double   ci[NBR_SPH]={0};



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

  /*
  if(DEBUG_SPH) {
    nco_sph_prn_pnt("nco_sph_seg_int(): intersection", Icross, 3, True);
    printf("%s: ||Pcross||=%.20g ||Qcross||=%.20g ||Icross||=%.20g arc=%.20g\n",fnc_nm,  nx1, nx2, nx3, darc);
  }
  */

  /* Icross is zero, should really have a range rather than an explicit zero */
  /* use dot product to se if Pcross and QCross parallel */
  if(  1.0- nco_sph_dot_nm(Pcross,Qcross )  <DOT_TOLERANCE  )
    //return nco_sph_parallel(a, b, c, d, p, q);
    return '0';



  dx_ab=1.0 - nco_sph_dot_nm(a,b);


  dx_cd=1.0 - nco_sph_dot_nm(c,d);

  dx_ai=1.0-  nco_sph_dot_nm(a,Icross);

  if(dx_ai <= DOT_TOLERANCE )
     dx_ai=0.0;
  else
     nx_ai=nco_sph_cross(a, Icross, ai);

  dx_ci= 1.0- nco_sph_dot_nm(c,Icross);

  if(dx_ci <= DOT_TOLERANCE )
    dx_ci=0.0;
  else
    nx_ci=nco_sph_cross(c, Icross, ci);



  if(0 && DEBUG_SPH)
    fprintf(stderr,"%s(): dx_ab=%2.10f dx_ai=%2.10f  nx1=%2.20f nx_ai=%2.10f   \n", fnc_nm, dx_ab, dx_ai, nx1, nx_ai );

  if(  ( dx_ai==0.0 ||  (  nco_sph_dot_nm(ai, Pcross) >0.99 && dx_ai>= 0.0 && dx_ai<=dx_ab  )) &&
       ( dx_ci==0.0 ||  (  nco_sph_dot_nm(ci, Qcross) >0.99 && dx_ci>0.0 && dx_ci <= dx_cd  ) )
    )
  {
    nco_sph_add_lonlat(Icross);

    if(DEBUG_SPH)
      nco_sph_prn_pnt("nco_sph_seg_int(): intersection", Icross, 3, True);

    memcpy(p,Icross, sizeof(double)*NBR_SPH);


    if(dx_ai==0 || fabs(dx_ai-dx_ab)<=DOT_TOLERANCE ||  dx_ci==0.0 || fabs(dx_ci-dx_cd)<=DOT_TOLERANCE )
      return 'v';
    else
      return '1';

  }


  /* try antipodal point */
  Icross[0]*= -1.0;
  Icross[1]*= -1.0;
  Icross[2]*= -1.0;


  dx_ai=1.0-  nco_sph_dot_nm(a,Icross);

  if(dx_ai <=DOT_TOLERANCE )
    dx_ai=0.0;
  else
    nx_ai=nco_sph_cross(a, Icross, ai);

  dx_ci= 1.0- nco_sph_dot_nm(c,Icross);

  if(dx_ci <=DOT_TOLERANCE )
    dx_ci=0.0;
  else
    nx_ci=nco_sph_cross(c, Icross, ci);



  if(0 && DEBUG_SPH)
    fprintf(stderr,"%s(): dx_ab=%2.10f dx_ai=%2.10f  nx1=%2.20f nx_ai=%2.10f   \n", fnc_nm, dx_ab, dx_ai, nx1, nx_ai );

  if(  ( dx_ai==0.0 ||  (  nco_sph_dot_nm(ai, Pcross) >0.99 && dx_ai>= 0.0 && dx_ai<=dx_ab  )) &&
       ( dx_ci==0.0 ||  (  nco_sph_dot_nm(ci, Qcross) >0.99 && dx_ci>0.0 && dx_ci <= dx_cd  ) )
  )
  {
    nco_sph_add_lonlat(Icross);
    if(DEBUG_SPH)
      nco_sph_prn_pnt("nco_sph_seg_int(): intersect-antipodal", Icross, 3, True);

    memcpy(p,Icross, sizeof(double)*NBR_SPH);
    


    if(dx_ai==0 || fabs(dx_ai-dx_ab)<=DOT_TOLERANCE ||  dx_ci==0.0 || fabs(dx_ci-dx_cd)<=DOT_TOLERANCE )
      return 'v';
    else
      return '1';

  }





  return '0';


}


nco_bool
nco_sph_seg_int_nw(double *a, double *b, double *c, double *d, double *p, double *q, char codes[])
{
  const char fnc_nm[]="nco_sph_seg_int_nw()";

  int flg_sx=False;

  int DEBUG_LCL=0;

  double nx1;
  double nx2;
  double nx3;
  double nx_ai;
  double nx_ci;

  double dx_ab;
  double dx_ai;

  double dx_cd;
  double dx_ci;

  double  Pcross[NBR_SPH]={0};
  double  Qcross[NBR_SPH]={0};
  double  Icross[NBR_SPH]={0};
  double   ai[NBR_SPH]={0};
  double   ci[NBR_SPH]={0};

  double X_TOLERANCE=0.99999;


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


  codes[0]='0';
  codes[1]='0';


  /* Icross is zero, should really have a range rather than an explicit zero */
  /* use dot product to se if Pcross and QCross parallel */
  if(  1.0- nco_sph_dot_nm(Pcross,Qcross )  <DOT_TOLERANCE  )
    //return nco_sph_parallel(a, b, c, d, p, q);
    return False;



  dx_ab=1.0 - nco_sph_dot_nm(a,b);


  dx_cd=1.0 - nco_sph_dot_nm(c,d);

  dx_ai=1.0-  nco_sph_dot_nm(a,Icross);

  if(dx_ai <= DOT_TOLERANCE )
    dx_ai=0.0;
  else
    nx_ai=nco_sph_cross(a, Icross, ai);

  dx_ci= 1.0- nco_sph_dot_nm(c,Icross);

  if(dx_ci <= DOT_TOLERANCE )
    dx_ci=0.0;
  else
    nx_ci=nco_sph_cross(c, Icross, ci);


  if(DEBUG_LCL)
    nco_sph_prn_pnt("nco_sph_seg_int_nw(): possible: ", Icross, 3, True);


  if(DEBUG_LCL) {
    fprintf(stderr, "%s(): dx_ab=%2.15f dx_ai=%2.15f  dx_cd=%2.15f dx_ci=%2.15f\n", fnc_nm, dx_ab, dx_ai, dx_cd, dx_ci);
    fprintf(stderr,"%s: diff ab=%.15f   cd=%.15e\n", fnc_nm, dx_ab-dx_ai,  dx_cd-dx_ci);
  }


  if(  ( dx_ai==0.0 ||  (  nco_sph_dot_nm(ai, Pcross) >X_TOLERANCE && dx_ai>= 0.0 && dx_ai<=dx_ab +DOT_TOLERANCE)) &&
       ( dx_ci==0.0 ||  (  nco_sph_dot_nm(ci, Qcross) >X_TOLERANCE && dx_ci>= 0.0 && dx_ci <= dx_cd + DOT_TOLERANCE ) )
  )
  {



    /* determine if head or tail is intersection point */
    if(dx_ai==0.0)
    {
      codes[0] = 't';
      memcpy(p,a, sizeof(double)*NBR_SPH);

    }
    else if( fabs(dx_ai-dx_ab)<=DOT_TOLERANCE  ) {
      codes[0] = 'h';
      memcpy(p,b, sizeof(double)*NBR_SPH);
    }
    else {
      codes[0] = '1';
      //nco_sph_add_lonlat(Icross);
      memcpy(p,Icross, sizeof(double)*NBR_SPH);

    }

    /* determine if head or tail is intersection point */
    if(dx_ci==0.0)
    {
      codes[1] = 't';
      if(codes[0]=='1')
        memcpy(p,c, sizeof(double)*NBR_SPH);
    }
    else if(fabs(dx_ci-dx_cd)<=DOT_TOLERANCE) {
      codes[1] = 'h';
      if(codes[0]=='1')
        memcpy(p,d, sizeof(double)*NBR_SPH);
    }
    else
      {
      codes[1] = '1';
      /*  not needed */
      //memcpy(p,Icross, sizeof(double)*NBR_SPH);

    }
    if(DEBUG_LCL)
      fprintf(stderr,"%s(): code=%sn", fnc_nm, codes);

    /*
    if(dx_ai==0 || fabs(dx_ai-dx_ab)<=DOT_TOLERANCE ||  dx_ci==0.0 || fabs(dx_ci-dx_cd)<=DOT_TOLERANCE )
      return 'v';
    else
      return '1';
    */

    return True;
  }


  /* try antipodal point */
  Icross[0]*= -1.0;
  Icross[1]*= -1.0;
  Icross[2]*= -1.0;
  nco_sph_add_lonlat(Icross);

  dx_ai=1.0-  nco_sph_dot_nm(a,Icross);

  if(dx_ai <=DOT_TOLERANCE )
    dx_ai=0.0;
  else
    nx_ai=nco_sph_cross(a, Icross, ai);

  dx_ci= 1.0- nco_sph_dot_nm(c,Icross);

  if(dx_ci <=DOT_TOLERANCE )
    dx_ci=0.0;
  else
    nx_ci=nco_sph_cross(c, Icross, ci);

  if(DEBUG_LCL)
    nco_sph_prn_pnt("nco_sph_seg_int_nw(): poss-antipodal", Icross, 3, True);

  if(DEBUG_LCL){
    fprintf(stderr,"%s(): dx_ab=%2.15f dx_ai=%2.15f  dx_cd=%2.15f dx_ci=%2.15f   \n", fnc_nm, dx_ab, dx_ai, dx_cd, dx_ci );
    fprintf(stderr,"%s: diff ab=%.15f   cd=%.15e\n", fnc_nm, dx_ab-dx_ai,  dx_cd-dx_ci);
  }

  if(  ( dx_ai==0.0 ||  (  nco_sph_dot_nm(ai, Pcross) >X_TOLERANCE && dx_ai>= 0.0 && dx_ai<= dx_ab + DOT_TOLERANCE  )) &&
       ( dx_ci==0.0 ||  (  nco_sph_dot_nm(ci, Qcross) >X_TOLERANCE && dx_ci>0.0 && dx_ci <= dx_cd  + DOT_TOLERANCE ) )
  )
  {




    /* determine if head or tail is intersection point */
    if(dx_ai==0.0)
    {
      codes[0] = 't';
      memcpy(p,a, sizeof(double)*NBR_SPH);

    }
    else if( fabs(dx_ai-dx_ab)<=DOT_TOLERANCE  ) {
      codes[0] = 'h';
      memcpy(p,b, sizeof(double)*NBR_SPH);
    }
    else {
      codes[0] = '1';
      // nco_sph_add_lonlat(Icross);
      memcpy(p,Icross, sizeof(double)*NBR_SPH);

    }

    /* determine if head or tail is intersection point */
    if(dx_ci==0.0)
    {
      codes[1] = 't';
      if(codes[0]=='1')
        memcpy(p,c, sizeof(double)*NBR_SPH);
    }
    else if(fabs(dx_ci-dx_cd)<=DOT_TOLERANCE) {
      codes[1] = 'h';
      if(codes[0]=='1')
        memcpy(p,d, sizeof(double)*NBR_SPH);
    }
    else
    {
      codes[1] = '1';
      /*  not needed !?
      nco_sph_add_lonlat(Icross);
      memcpy(p,Icross, sizeof(double)*NBR_SPH);
       */
    }

    if(DEBUG_LCL)
      fprintf(stderr,"%s(): code=%s\n", fnc_nm, codes);

    /*
    if(dx_ai==0 || fabs(dx_ai-dx_ab)<=DOT_TOLERANCE ||  dx_ci==0.0 || fabs(dx_ci-dx_cd)<=DOT_TOLERANCE )
      return 'v';
    else
      return '1';
    */

    return True;

  }

  if(DEBUG_LCL)
    fprintf(stderr,"%s(): code=%s\n", fnc_nm, codes);



  return False;


}





char
nco_sph_seg_parallel(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1, poly_vrl_flg_enm *inflag )
{

  const char fnc_nm[] = "nco_sph_seg_parallel()";

  char code;
  int flg_sx = 0;

  double nx1;
  double nx2;
  double nx3;

  double dx_p1;
  double dx_q0;
  double dx_q1;


  double Pcross[NBR_SPH] = {0};
  double Qcross[NBR_SPH] = {0};
  double Tcross[NBR_SPH] = {0};


  if (flg_sx) {
    nx1 = nco_sph_sxcross(p0, p1, Pcross);
    nx2 = nco_sph_sxcross(q0, q1, Qcross);

    nco_sph_add_lonlat(Pcross);
    nco_sph_add_lonlat(Qcross);


  } else {
    nx1 = nco_sph_cross(p0, p1, Pcross);
    nx2 = nco_sph_cross(q0, q1, Qcross);

  }

  /* check points in the same direction */
  if (nco_sph_dot_nm(Pcross, Qcross) < 0.99)
    return '0';

  dx_p1 = 1.0 - nco_sph_dot_nm(p0, p1);

  dx_q0 = 1.0 - nco_sph_dot_nm(p0, q0);

  if( dx_q0< DOT_TOLERANCE)
    dx_q0=0.0;


  if (dx_q0 != 0.0) {
    nx3 = nco_sph_cross(p0, q0, Tcross);

    if (nco_sph_dot_nm(Pcross, Tcross) < 0.0)
      dx_q0 *= -1.0;

  }

  dx_q1 = 1.0 - nco_sph_dot_nm(p0, q1);

  if(dx_q1 <DOT_TOLERANCE)
    dx_q1=0.0;

  if (dx_q1 != 0.0) {
    nx3 = nco_sph_cross(p0, q1, Tcross);

    if (nco_sph_dot_nm(Pcross, Tcross) < 0.0)
      dx_q1 *= -1.0;
  }

  /* we now have 4 "points to order"
  * a=0.0, dx_ab, dx_ac , dx_ad
   * always dx_ab > 0.0 and dx_ac < dx_ad
   * */

  /* no overlap so return */
  if( (dx_q0 < 0.0  && dx_q1 < 0.0) || ( dx_q0 > dx_p1 && dx_q1 > dx_p1  )) {
    code = '0';
  }

  else if(dx_q0 <0.0 &&  dx_q1 == 0.0   )
  {
    code='1';
    nco_sph_adi(r0,p0);
    /* not sure which flag to set here */
    *inflag=poly_vrl_qin;
  }
  else if( dx_q0 == dx_p1 && dx_q1 > dx_p1  )
  {
    code='1';
    nco_sph_adi(r0,p1);
    *inflag=poly_vrl_pin;
  }
    /* LHS overlap */
  else if (dx_q0 <0.0 &&  (dx_q1 >0.0 && dx_q1 <= dx_p1)  ) {
    code= '2';
    nco_sph_adi(r0, p0);
    nco_sph_adi(r1, q1);
    *inflag=poly_vrl_qin;

  }
    /* RHS overlap */
  else if( dx_q0 >=0.0 &&  dx_q0 < dx_p1 && dx_q1 > dx_p1    )
  {
    code= '2';
    nco_sph_adi(r0, q0);
    nco_sph_adi(r1, p1);
    *inflag=poly_vrl_pin;

  }
  else if(  dx_q0 >=0.0 && dx_q1 <= dx_p1    ) {
    code= '2';
    nco_sph_adi(r0, q0);
    nco_sph_adi(r1, q1);
    *inflag=poly_vrl_qin;
  }
  else if( dx_q0 <0.0 && dx_q1 > dx_p1    )
  {
    code='2';
    nco_sph_adi(r0,p0 );
    nco_sph_adi(r0,p1 );
    *inflag=poly_vrl_pin;
  } else{
    code='0';
  }

  if(DEBUG_SPH )
  {
    if (code >= '1')
      nco_sph_prn_pnt("nco_sph_seg_parallel(): intersect1", r0, 3, True);

    if (code == '2')
      nco_sph_prn_pnt("nco_sph_seg_parallel(): intersect2", r1, 3, True);

  }


  return code;
}


nco_bool
nco_sph_intersect_pre(poly_sct *sP, poly_sct *sQ, char sq_sng[]) {

  int n;
  int m;
  int idx = 0;
  int jdx;
  int jdx1;
  int numIntersect;
  int numVertex = 0;

  nco_bool DEBUG_LCL=False;
  nco_bool bComplex = False;

  /* if true make control point INSIDE polygon
   * else make OUTSIDE polygon */
  nco_bool bInside=True;

  char codes[] ="00";


  double p[NBR_SPH];
  double q[NBR_SPH];

  double nx1;
  double pi[NBR_SPH]={0.0};

  double pControl[NBR_SPH];

  double Pcross[NBR_SPH];



  /* fxm:2019-06-07 - there is a problem using the polygon center as
   * for some RLL grids the center of a polar triangle can be the pole itself
   * also there is a situation where for some polygons the center maybe outside polygon ?
   * Also if we use a point outide we dont have to bother (re)calculating the center
   */
    nco_sph_mk_control(sP, bInside, pControl);


  if(DEBUG_LCL)
    nco_sph_prn_pnt("nco_sph_intersect_pre()/control pnt", pControl,3,True);

  n = sP->crn_nbr;
  m = sQ->crn_nbr;



  for (idx = 0; idx < m; idx++) {
    bComplex = False;
    numIntersect = 0;

    memset(pi,0, sizeof(double)* NBR_SPH);

    for (jdx = 0; jdx < n; jdx++) {
      jdx1 = (jdx + n - 1) % n;

      /* check for parallel edges */

      if( nco_sph_seg_int_nw(sP->shp[jdx1], sP->shp[jdx], pControl, sQ->shp[idx], p, q, codes)) {
        if (codes[1] != '0' && codes[1] != '1') {
          bComplex = True;
          break;
        }

        if (codes[1] == '1')
          /* check for consecutive points */
          if (numIntersect == 0 || 1.0 - nco_sph_dot_nm(pi, p) > DOT_TOLERANCE) {
            //if(numIntersect==0 ||  pi[0]!=p[0] && pi[1]!=p[1] )
            numIntersect++;

            /* save intersection point */
            nco_sph_adi(pi, p);
          }


        if (DEBUG_LCL) {
          (void) fprintf(stderr, "%s():, idx=%d jdx=%d numIntersect=%d codes=%s\n", __FUNCTION__, idx, jdx, numIntersect, codes);
          nco_sph_prn_pnt("intersect_pre()", p, 4, True);
          // nco_sph_prn_pnt("intersect_pre()", p, 3, True);
        }
      }

      /*
      if(numIntersect >= 2)
        break;
      */

    }



    if (bComplex) {
      /* head of q is on an edge of p */
      if (codes[0] == '1' && (codes[1] == 'h' || codes[1] == 't'))
        sq_sng[idx] = 'e';
        /* head of q is on vertex of p */
      else if ((codes[0] == 'h' || codes[0] == 't') && codes[1] == 'h')
        sq_sng[idx] = 'v';

    } else
      if(bInside)
        sq_sng[idx]= ( numIntersect==0 ? 'i': 'o' );
      else
        sq_sng[idx]= ( numIntersect %2 ? 'i' : 'o' );


  }

  sq_sng[m]='\0';

  return True;

}

int
nco_sph_process_pre(poly_sct *sQ, char *sq_sng, nco_bool *bGenuine)
{
  int idx=0;
  int sz;
  int srt;

  int numInside=0;
  int numOutSide=0;
  int numEdges=0;
  int numVertex=0;

  sz=sQ->crn_nbr;

  for(idx=0;idx<sz;idx++)
  {
    switch(sq_sng[idx])
    {
      case 'i':
        numInside++;
        break;

      case 'o':
        numOutSide++;
        break;

      case 'e':
        numEdges++;
        break;

      case 'v':
        numVertex++;
        break;


    }

  }


  *bGenuine=False;
  /* check for a "GENUINE" intersection ie ('i' to 'o') or ('o' to 'i') */
  if(numOutSide && numInside )
    for(idx=0;idx<sz;idx++)
      if( sq_sng[idx] == 'i' &&  sq_sng[(idx+1)%sz] =='o' || sq_sng[idx]=='o' &&  sq_sng[(idx+1)%sz] == 'i'   )
      {
        *bGenuine=True;
        break;
      }


  /* fold edges and/or vertex(s) */
  if(numOutSide &&  (numEdges || numVertex)) {

    char *sq=NULL_CEWI;

    for (idx = 0; idx < sz; idx++)
      if (sq_sng[idx] == 'o') {
        srt = idx;
        break;
      }


    sq = strdup(sq_sng);

    for (idx = 0; idx < sz; idx++) {
      int lx = (srt + idx) % sz;
      char trg = sq[lx];
      /* next char */
      char trgN = sq[(lx + 1) % sz];
      /* previous char */
      char trgP = sq[(srt + idx + sz - 1) % sz];

      if (trg == 'i' || trg == 'o')
        continue;

      if (trgP != 'o' && (trg == 'e' || trg == 'v') && trgN != 'o')
        sq[lx] = 'i';


    }
    strcpy(sq_sng, sq);
    if (sq)
      sq = (char *) nco_free(sq);

  }



  /* all of Q is inside of P  or on vertex/edges  */
  if(numOutSide==0 )
     return 1;


  /* all of Q vetex's outside of P */
  if(numOutSide &&  !numInside  &&  !numEdges && !numVertex)
    return 2;


  if(numOutSide && numInside && bGenuine)
    return 3;


  /* need to do overlap  func but dont process sq_sng */
  if(numOutSide && numInside &&  (  (!numEdges && !numVertex ) ||  numEdges==1 && !numVertex || !numEdges && numVertex==1  ) )
    return 4;


   return 5;


}







/* returns true if vertex is on edge (a,b) */
nco_bool
nco_sph_seg_vrt_int(double *a, double *b, double *vtx)
{
  double nx_ab;
  double nx_av;

  double dx_ab;
  double dx_av;

  double  Pcross[NBR_SPH]={0};
  double  Vcross[NBR_SPH]={0};


  nx_ab=nco_sph_sxcross(a, b, Pcross);

  dx_ab=1.0 - nco_sph_dot_nm(a,b);

  dx_av=1.0 - nco_sph_dot_nm(a,vtx);

  if(dx_av >0.0  )
    nx_av=nco_sph_cross(a, vtx, Vcross );


  if( nco_sph_dot_nm(Pcross, Vcross) >0.9999 && dx_av >=0.0 && dx_av <= dx_ab )
    return True;

  return False;


}


/* takes a point and a cross product representing the normal to the arc plane */
/* returns 1 if point on LHS of arc plane */
/* returns -1 if point on RHS of arc plane */
/* return 0 if point on the arc - (given suitable tolerances ) */
int nco_sph_lhs(double *Pi, double *Qi)
{
   const char *fnc_nm="nco_sph_lhs()";
   double ds;

   ds= nco_sph_dot_nm(Pi, Qi);

   if(DEBUG_SPH)
     (void)fprintf(stderr,"%s: ds=%.15f  lon=%.15f lat=%.15f \n",fnc_nm, ds, R2D(Pi[3]), R2D(Pi[4]) );

   if( fabs(ds)<=DOT_TOLERANCE)
     return 0;


   if(ds  > 0.0 )
      return 1;
   else if( ds< 0.0)
      return -1;



   return 1;

   /*
   ds=acos( nco_sph_dot(Pi,Qi) );

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

/* dot product normalized */
double  nco_sph_dot_nm(double *a, double *b)
{
  int idx;
  double sum=0.0;
  double n1;
  double n2;

  const char fnc_nm[]="nco_sph_dot_nm()";

  for(idx=0; idx<3; idx++)
    sum+=a[idx]*b[idx];

  if(sum!=0.0) {
    n1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    n2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);
    sum = (sum / n1) / n2;
  }

  if(0 && DEBUG_SPH)
    fprintf(stderr,"%s() dt=%f n1=%f %f\n", fnc_nm, sum, n1, n2 );


  return sum;


}




double  nco_sph_cross(double *a, double *b, double *c)
{
  const char fnc_nm[]="nco_sph_cross()";
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

   if(0 && DEBUG_SPH)
      printf("%s: n1=%f (%f, %f %f)\n",fnc_nm, n1, c[0],c[1], c[2]);

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
  nco_bool bDeg = False;
  double n1;
  double lon1;
  double lon2;

  double lat1;
  double lat2;

  if (bDeg) {
    lon1 = a[3] * M_PI / 180.0;
    lat1 = a[4] * M_PI / 180.0;

    lon2 = b[3] * M_PI / 180.0;
    lat2 = b[4] * M_PI / 180.0;
  } else{
    lon1 = a[3];
    lat1 = a[4];

    lon2 = b[3];
    lat2 = b[4];

  }


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


void
nco_sph_add(double *a, double *b, double *c)
{
  c[0]=a[0]+b[0];
  c[1]=a[1]+b[1];
  c[2]=a[2]+b[2];

  return;

}

void
nco_sph_sub(double *a, double *b, double *c)
{
  c[0]=a[0]-b[0];
  c[1]=a[1]-b[1];
  c[2]=a[2]-b[2];

  return;

}

void
nco_sph_mlt(double *a, double m)
{
  a[0]*=m;
  a[1]*=m;
  a[2]*=m;

  return;


}

double
nco_sph_dist(double *p0, double *p1)
{

  double r0[NBR_SPH];

  r0[0]=p0[0]-p1[0];
  r0[1]=p0[1]-p1[1];
  r0[2]=p0[2]-p1[2];

  return r0[0]*r0[0]+ r0[1]*r0[1]+ r0[2]*r0[2];



}





void  nco_sph_adi(double *a, double *b)
{
   (void)memcpy(a,b, sizeof(double)* NBR_SPH);
}






void nco_sph_add_pnt(double **R, int *r, double *P)
{

   double delta;

   //delta = ( *r==0 ? 0.0 :   2.0 *asin(    sqrt( pow( R[*r-1][0] - P[0],2 ) + pow( R[*r-1][1] - P[1],2 ) + pow( R[*r-1][2] - P[2],2 )  ) /2.0) );
   if(*r >0 )
      delta = 1.0 - nco_sph_dot(R[*r-1], P );

   if(DEBUG_SPH)
      nco_sph_prn_pnt("aAddPoint():", P, 3, True);



   /* only add  point if its distinct from previous point */
   if ( *r==0 ||  delta > DOT_TOLERANCE )
   {

      memcpy(R[*r], P, sizeof(double)*NBR_SPH);
      (*r)++;
   }


}


nco_bool nco_sph_between(double a, double b, double x)
{
  const char fnc_nm[]="nco_sph_between()";


   nco_bool bret=False;

   double diff;

   diff=fabs(b-a);



   if(diff==0.0 )
   {


     if( fabs(x-a)<= SIGMA_RAD )
        bret=True;
   }
   else if( diff <= SIGMA_RAD )
   {

      if(  ( b >a &&  x>= a && x<=b ) || ( b<a && x>=b && x<=a )   )
         bret= True;

   }
   else if( diff < M_PI )
   {

      if(  ( b >a &&  x>= a && x<=b ) || ( b<a && x>=b && x<=a )   )
         bret=True;

   }
   /* this indicates a wrapped cell (or edge )
    * this same code works for domains (0-360) and (-180,180) */
   else if( diff >  M_PI )
   {

     if( (b>a && (x>=b || x<=a)) || ( b<a && (x<=b ||  x>=a)) )
        bret= True;
   }




  if(DEBUG_SPH)
    printf("%s: a=%.20f, b=%.20f, x=%.20f %s \n",fnc_nm, a, b, x, (bret==True ? "True":"False"));

   return bret;


}





/* use crt coords to check bounds */
nco_bool nco_sph_lonlat_between(double *a, double *b, double *x)
{
  const char fnc_nm[]="nco_sph_lonlat_between()";

   /* working in radians here */
   nco_bool bDeg=False;
   nco_bool bRet=False;

   double lat_min;
   double lat_max;

   if(nco_sph_between(a[3], b[3], x[3]) == False )
      return False;

   /* special lat check */
   //getLatCorrect(a,b, &lat_min,&lat_max);
  nco_geo_get_lat_correct(a[3], a[4], b[3], b[4], &lat_min, &lat_max, bDeg);



   if( x[4]>=lat_min && x[4]<=lat_max )
      bRet=True;
   else
      bRet=False;

  if(DEBUG_SPH)
    printf("%s: lat_min=%.20f lat_max=%.20f lat=%.20f %s\n",fnc_nm, lat_min, lat_max, x[4],
           (bRet ? "True" : "False") );




  return bRet;


}

nco_bool sxBetween(double *a, double *b, double *c)
{

   if ( a[3] != b[3] )
      return (  ( c[3] >= a[3] && c[3] <=b[3] ) || ( c[3] <= a[3] && c[3] >= b[3] )) ;
   else
      return (  ( c[4] >= a[4] && c[4] <=b[4] ) || ( c[4] <= a[4] && c[4] >= b[4] )) ;


   /*
   if ( a[3] != b[3] )
     return (  a[3] <= c[3] && b[3] >= c[3] ||  a[3] >= c[3] && b[3] <= c[3] ) ;
   else
     return (  a[4] <= c[4] && b[4] >=c[4]   ||  a[4] >= c[4] && b[4] <= c[4] ) ;
   */

}


int
nco_sph_parallel_lat(double *p1, double *p2, double *q1, double *q2, double *a, double *b)
{
  bool bdir=False;

  /* check sense of direction */
  bdir=( p2[4] - p1[4] >0.0);

  if( (q2[4] - q1[4] >0.0) != bdir  )
     return 0;


  return 0;



}




int nco_sph_parallel(double *a, double *b, double *c, double *d, double *p, double *q)
{

   char code='0';
   const char *ptype="none";

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

   fprintf(stderr, "%s ", sMsg);

   switch(style)
   {
      case 0:
      default:
         fprintf(stderr,  "(dx=%.15f, dy=%.15f, dz=%.15f), (lon=%.15f,lat=%.15f)",p[0], p[1], p[2], p[3], p[4] );
       break;

      case 1:
         fprintf(stderr,  "(dx=%.15f, dy=%.15f, dz=%.15f)",p[0], p[1], p[2] );
       break;

      case 2:
         fprintf(stderr,  "(lon=%.15f,lat=%.15f)",p[3], p[4] );
       break;

      case 3:
         fprintf(stderr,  "(lon=%.15f,lat=%.15f)",p[3] *180.0/M_PI,  p[4]*180.0/M_PI );
       break;

      case 4:
         fprintf(stderr,  "(dx=%.15f, dy=%.15f, dz=%.15f), (lon=%.15f,lat=%.15f)",p[0], p[1], p[2], p[3] *180.0/M_PI,  p[4]*180.0/M_PI);
       break;

      case 5:
         fprintf(stderr,  "(dx=%f, dy=%f, dz=%f), (lon=%f,lat=%f)",p[0], p[1], p[2], p[3] *180.0/M_PI,  p[4]*180.0/M_PI);
       break;



   }

   if(bRet)
      fprintf(stderr, "\n");
   else
      printf(" * ");

}

nco_bool nco_sph_is_convex(double **sP, int np)
{
  const char fnc_nm[]="nco_sph_is_convex()";


nco_bool flg_sx=0;

int idx;
int idx_pre;
int idx_nex;


double n1;
double n2;

double dp;
double theta;
double rad1_nco=1.0;
double rad=1.0;

double  aCross[NBR_SPH];
double  bCross[NBR_SPH];

for(idx=0; idx<np;idx++)
{
  idx_pre=(idx + np -1)% np;
  idx_nex=(idx +1)% np;

  if(flg_sx) {
    n1 = nco_sph_sxcross(sP[idx], sP[idx_pre], aCross);
    n2 = nco_sph_sxcross(sP[idx], sP[idx_nex], bCross);
  } else {
    n1 = nco_sph_cross(sP[idx], sP[idx_pre], aCross);
    n2 = nco_sph_cross(sP[idx], sP[idx_nex], bCross);


  }

  //rad1_nco = sRadius(aCross);
  //rad  = sRadius(bCross);
  dp= nco_sph_dot(aCross, bCross);

  //dp= dp / nco_sph_dot(sP[idx], sP[idx_pre]) / nco_sph_dot(sP[idx], sP[idx_nex]);


  // dp=sDot(sP[idx1], sP[idx]) / rad1_nco /rad;
  if(0) {
    dp= nco_sph_dot(aCross, bCross);
    theta = acos(dp);
  }

  if(DEBUG_SPH)
    printf("%s():, %d angle=%f, dp=%f, n1=%.15g n2=%.15g\n", fnc_nm, idx, theta*180.0/M_PI, dp, n1, n2);

   //if(dp<0.0)
   //  return False;
  //if( fabs(theta - M_PI) >SIGMA_RAD )
  //   return False;


}

return True;


}

/* make a control point that is Inside or Outside of polygon */
int nco_sph_mk_control(poly_sct *sP, nco_bool bInside,  double* pControl  )
{
   /* do stuff in radians */


   int iret=NCO_ERR;
   double clat=0.0;
   double clon=0.0;

   double lon_fct=2.0;
   double lat_fct=2.0;

   nco_bool bDeg=False;

   /* use Center as point inside polygon */
   if(bInside)
   {
     bDeg=True;
     nco_geo_lonlat_2_sph(sP->dp_x_ctr,sP->dp_y_ctr , pControl, bDeg);
     return NCO_NOERR;
   }



   /* convert limits to radians */
   double lon_min=D2R( sP->dp_x_minmax[0]);
   double lon_max=D2R( sP->dp_x_minmax[1]);
   double lat_min=D2R( sP->dp_y_minmax[0]);
   double lat_max=D2R( sP->dp_y_minmax[1]);

   double xbnd=D2R(8.0);


   /* polar cap */
   if( sP->bwrp && sP->bwrp_y )
   {
     /* get latitude of equator */
     double lat_eq= (LAT_MAX_RAD-LAT_MIN_RAD) /2.0;

     /* choose an arbitary lon  */
     clon=D2R(20);

     /* check if we have an north or south pole *
      * nb a north polar cap - all points in nothern hemisphere
      *    a south polar cap all point in southern hemisphere */

     if(lat_min >=lat_eq && lat_max > lat_eq  )
       clat=lon_min-xbnd / 2.0;
     else if( lat_min < lat_eq && lat_max <= lat_eq  )
       clat=lon_max+xbnd / 2.0;
     else
       return NCO_ERR;

   }

   /* just longitude wrapping */
   else if(sP->bwrp)
   {
      /* nb distance between lmin and lmax  >180.0 */
      clon=lon_min+xbnd / lon_fct;
      clat=( lat_min+lat_max ) / lat_fct;
   }
   /* no wrapping x */
   else {
       /* choose left or right hand size */
       if (lon_min - LON_MIN_RAD > xbnd) {
         clon = lon_min - xbnd / lon_fct;
         clat = (lat_min + lat_max) / lat_fct;

       } else if (LON_MAX_RAD - lon_max > xbnd) {

         clon = lon_max + xbnd / lon_fct;
         clat = (lat_min + lat_max) / lat_fct;

       }
         /* choose below or above */
       else if (lat_min - LAT_MIN_RAD > xbnd) {
         clat = lat_min - xbnd / lat_fct;
         /* choose centre */
         clon = (lon_min + lon_max) / lon_fct;

       } else if (LAT_MAX_RAD - lat_max > xbnd) {
         clat = lat_max + xbnd / lat_fct;
         clon = (lon_min + lon_max) / lon_fct;


       } else {
         return NCO_ERR;
       }
   }

   /* remember clat, clon in radians */
   nco_geo_lonlat_2_sph(clon, clat, pControl, bDeg );

   return NCO_NOERR;


}


/* nb doesnt work if polygon spans more than 180.0
 * works by counting the number of intersections of the
   line (pControl, pVertex) and each edge in sP
   pControl is chosen so that it is OUTSIDE of sP
 */
int nco_sph_pnt_in_poly(double **sP, int n, double *pControl, double *pVertex)
{

  char code;
  int idx;
  int idx1=0;
  int numIntersect=0;

  double  p[NBR_SPH];
  double  q[NBR_SPH];

  const char *fnc_nm="nco_sph_pnt_in_poly()";

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

  if(DEBUG_SPH)
     fprintf(stderr,"%s:%s: num intersections=%d\n ",nco_prg_nm_get(),fnc_nm, numIntersect );

  //return (numIntersect % 2  );
  return numIntersect;


}

/* This function is normally called AFTER nco_sph_intersect()
 * So We KNOW now that there are no PROPER INTERSECTIONS
 * So maybe P Inside Q or Q inside of P
 * It is possible that some vertex are on edge's - so were count them */
nco_bool
nco_sph_poly_in_poly(
poly_sct *sP,
poly_sct *sQ
)
{
  int n;
  int m;
  int idx=0;
  int jdx;
  int jdx1;
  int numIntersect;
  int numVertex=0;

  nco_bool bVertex=False;

  char code='0';

  double p[NBR_SPH];
  double q[NBR_SPH];

  double pControl[NBR_SPH];


  /* make control point center of sP */
  nco_sph_mk_control(sP, True, pControl);

  n=sP->crn_nbr;
  m=sQ->crn_nbr;


  for(idx=0; idx<m ; idx++ )
  {
    bVertex=False;
    numIntersect=0;

    for (jdx = 0; jdx < n; jdx++)
    {
      jdx1 = (jdx + n - 1) % n;
      code = nco_sph_seg_int(sP->shp[jdx1], sP->shp[jdx], pControl, sQ->shp[idx], p, q);

      if (code == '1')
        numIntersect++;

      if (code == 'v')
        bVertex=True;

    }

    numVertex+=bVertex;

    if(numVertex >= 3 )
      return True;


    if(!bVertex  )
    {
      if(numIntersect==0)
        return True;


      if( numIntersect > 0)
        return False;

    }



  }

  return False;


}




/* set static globals */
void nco_sph_set_domain(double lon_min_rad, double lon_max_rad, double lat_min_rad, double lat_max_rad)
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


/* calculates "centroid of a polygon using "average of vertex(s) */
/* we need a center inside so that  nco_sph_intersect_pre() can work corectly */
void
nco_sph_centroid_mk(poly_sct *sP, double *pControl)
{
  int idx;
  int idx1;
  int sz;
  int min_idx=0;
  int max_idx=0;

  nco_bool bDeg=False;

  double pC[NBR_SPH];

  double pMidPoint[NBR_SPH];

  const char fnc_nm="nco_sph_centroid_mk()";

  if(!sP->shp) {
    (void) fprintf(stderr, "%s:%s func has been called with sP->sph as null\n", nco_prg_nm_get(), fnc_nm);
     nco_err_exit(1, fnc_nm);
  }




  pMidPoint[0]=0.0;
  pMidPoint[1]=0.0;
  pMidPoint[2]=0.0;

  /* sum vertx(s) and do "average" */
  for(idx=0;idx<sP->crn_nbr;idx++)
    nco_sph_add(sP->shp[idx],pMidPoint,pMidPoint);


  /* normalize pMidPoint */
  nco_sph_mlt( pMidPoint, 1.0/sqrt( pMidPoint[0]*pMidPoint[0]+ pMidPoint[1]*pMidPoint[1] + pMidPoint[2]*pMidPoint[2]  ) );

  if(DEBUG_SPH)
    nco_sph_prn_pnt("nco_sph_centroid_mk - Centroid",pMidPoint,3,True );



  nco_geo_sph_2_lonlat(pMidPoint, &pMidPoint[3], &pMidPoint[4], bDeg);

  nco_sph_adi(pControl, pMidPoint);

  return;



}


/* creates a point that is guaranteed to be inside ANY polygon */
/* it may break down if there are multiple coincident vertex  */
nco_bool
nco_sph_inside_mk(poly_sct *sP, double *pControl)
{
  int idx;
  int sz;
  int mid_idx=-1;
  int mid_idx1=-1;
  int max_idx=-1;

  nco_bool bDeg=False;


  double max_dp = -(DBL_MAX);
  double length = 0.0;

  double pMidPoint[NBR_SPH]={0.0};


  const char fnc_nm="nco_sph_inside_mk()";

  sz=sP->crn_nbr;

  if(!sP->shp) {
    (void) fprintf(stderr, "%s:%s func has been called with sP->sph as null\n", nco_prg_nm_get(), fnc_nm);
    nco_err_exit(1, fnc_nm);
  }


  for (idx = 0; idx < sz; idx++)
  {
    length = nco_sph_dist(sP->shp[(idx + sz - 1) % sz], sP->shp[idx]);
    if (length > max_dp) {
      max_dp = length;
      mid_idx = idx;
    }

  }

  mid_idx1=(mid_idx+sz-1)%sz;
  /* start VECTOR MID-Point */
  nco_sph_add( sP->shp[mid_idx1], sP->shp[mid_idx], pMidPoint);
  /* half of vector */
  nco_sph_mlt(pMidPoint,0.5);

  /* normalize vector (we are on a sphere) */
  nco_sph_mlt( pMidPoint, 1.0/sqrt( pMidPoint[0]*pMidPoint[0]+ pMidPoint[1]*pMidPoint[1] + pMidPoint[2]*pMidPoint[2]  ) );



  length=0.0;
  max_dp=0.0;

  /* find vertex with greatest distance from pC */
  for(idx=0;idx<sz;idx++)
  {

    if(idx==mid_idx || (idx + sz - 1) % sz == mid_idx1   )
      continue;

    length=nco_sph_dist(sP->shp[idx],pMidPoint);
    if(length>max_dp){
      max_dp=length;
      max_idx=idx;

    }


  }

  if(length==0.0 || max_idx==-1)
    return False;

  /* find midpoint between pMidPoint an Vertex of interest */
  nco_sph_add(pMidPoint, sP->shp[max_idx], pControl );
  /* divide by two */
  nco_sph_mlt(pControl,0.5);
  /*normalize */
  nco_sph_mlt( pControl, 1.0/sqrt( pControl[0]*pControl[0]+ pControl[1]*pControl[1] + pControl[2]*pControl[2]  ) );

  /* add lat/lon */
  nco_sph_add_lonlat(pControl);


  return True;

}




/*------------------------ nco_geo functions manipulate lat & lon  ----------------------------------*/

/* assume latitude -90,90 */
double nco_geo_lat_correct(double lat1, double lon1, double lon2)
{

   double dp;

   if( fabs(lon1 - lon2) <= SIGMA_RAD || fabs(lat1) <= SIGMA_RAD || lat1 >= LAT_MAX_RAD - SIGMA_RAD   || lat1 <= LAT_MIN_RAD + SIGMA_RAD  )
      return lat1;

   //lat1=lat1*M_PI / 180.0;

   /* exact constant is  is 2.0 but lets be a bit generous */
   dp= tan(lat1) / cos ( fabs(lon2-lon1) / 2.0 ) ;

   dp=atan(dp);


   return dp;


}





void nco_geo_get_lat_correct(double lon1, double lat1, double lon2, double lat2, double *dp_min, double *dp_max,
                             nco_bool bDeg)
{

  double dswp;

   if( lat2 >lat1 )
   {
      dswp=lat1;
      lat1=lat2;
      lat2=dswp;
   }

   if(lon1>lon2)
   {

     dswp=lon1;
     lon1=lon2;
     lon2=dswp;

   }


   if(bDeg)
   {
      lat1 *= M_PI / 180.0;
      lat2 *= M_PI / 180.0;
      lon1 *= M_PI / 180.0;
      lon2 *= M_PI / 180.0;
   }

  /* deal with wrpping . nb this code works for (-180, 180 ) ( 0 - 360) */
  if(lon2-lon1 >= M_PI)
    lon2-=2*M_PI;



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


/* assumes lon, lat in degrees */
void nco_geo_lonlat_2_sph(double lon, double lat, double *b, nco_bool bDeg)
{

   if(bDeg) {
      lon *= M_PI / 180.0;
      lat *= M_PI / 180.0;
   }

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

   if( *lon < 0.0 &&  LON_MIN_RAD >=0.0  )
      *lon+= (LON_MAX_RAD);

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



/****************  functions for RLL grids *******************************************/

int nco_rll_intersect(poly_sct *P, poly_sct *Q, poly_sct *R, int *r)
{
  const char fnc_nm[]="nco_rll_intersect()";

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

  nco_bool isParallel=False;
  nco_bool isP_LatCircle=False;
  nco_bool isQ_LatCircle=False;

  double nx1;
  double nx2;
  double nx3;


  char code='0';

  double Pcross[NBR_SPH];
  double Qcross[NBR_SPH];
  double Xcross[NBR_SPH];

  double p[NBR_SPH];
  double q[NBR_SPH];

  poly_vrl_flg_enm inflag= poly_vrl_unk;

  n=P->crn_nbr;
  m=Q->crn_nbr;

  if(DEBUG_SPH)
    fprintf(stdout, "%s: just entered %s\n", nco_prg_nm_get(), fnc_nm);


  do {


    a1 = (a + n - 1) % n;
    b1 = (b + m - 1) % m;

    isP_LatCircle = nco_rll_is_lat_circle(P->shp[a1], P->shp[a]);
    isQ_LatCircle = nco_rll_is_lat_circle(Q->shp[b1], Q->shp[b]);

    nx1 = nco_sph_cross(P->shp[a1], P->shp[a], Pcross);
    nx2 = nco_sph_cross(Q->shp[b1], Q->shp[b], Qcross);

    //nx3= nco_sph_cross(Pcross, Qcross, Xcross);

    if (isQ_LatCircle) {

      ip1qLHS = nco_rll_lhs_lat(P->shp[a1], Q->shp[b1], Q->shp[b]);
      ipqLHS = nco_rll_lhs_lat(P->shp[a], Q->shp[b1], Q->shp[b]);
    } else {
      ip1qLHS = nco_rll_lhs(P->shp[a1], Qcross);
      ipqLHS = nco_rll_lhs(P->shp[a], Qcross);

    }

    /* imply rules facing if 0 */

    if (ipqLHS == 0 && ip1qLHS != 0)
      ipqLHS = ip1qLHS * -1;
    else if (ipqLHS != 0 && ip1qLHS == 0)
      ip1qLHS = ipqLHS * -1;


    if (isP_LatCircle) {
      iq1pLHS = nco_rll_lhs_lat(Q->shp[b1], P->shp[a1], P->shp[a]);
      iqpLHS = nco_rll_lhs_lat(Q->shp[b], P->shp[a1], P->shp[a]);


    } else {

      iq1pLHS = nco_rll_lhs(Q->shp[b1], Pcross);
      iqpLHS = nco_rll_lhs(Q->shp[b], Pcross);

    }

    /* imply rules facing if 0 */
    if (iqpLHS == 0 && iq1pLHS != 0)
      iqpLHS = iq1pLHS * -1;
    else if (iqpLHS != 0 && iq1pLHS == 0)
      iq1pLHS = iqpLHS * -1;


    /* now calculate face rules */
    qpFace = nco_sph_face(ip1qLHS, ipqLHS, iqpLHS);
    pqFace = nco_sph_face(iq1pLHS, iqpLHS, ipqLHS);


    /* check for parallel segments */


    /* see if arcs are parallel */
    if (isP_LatCircle && isQ_LatCircle && P->shp[a1][4] == Q->shp[b1][4])
    {
      isParallel = True;
    }
    else if (!isP_LatCircle && !isQ_LatCircle && P->shp[a1][3] == Q->shp[b1][3])
    {
      /* check arc both in same "direction" */
      if (  (P->shp[a][4] > P->shp[a1][4]) != (Q->shp[b][4] > P->shp[b1][4])   )
        return EXIT_FAILURE;
      else
        isParallel = True;
    }
    else
      isParallel = False;

    if (isParallel) {

      ip1qLHS = 0;
      ipqLHS = 0;
      iq1pLHS = 0;
      iqpLHS = 0;
      qpFace = 0;
      pqFace = 0;


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





    if(isGeared)
    {

      if(isParallel)
      {
        poly_vrl_flg_enm lcl_inflag = poly_vrl_unk;

        code = nco_rll_seg_parallel(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q, &lcl_inflag);

        if(code == 'X'){
          if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stderr,"%s: ERROR %s() reports %s\n",nco_prg_nm_get(),fnc_nm,"parallel edges in opposite directions\n");
          return EXIT_FAILURE;
        }

        if (lcl_inflag != poly_vrl_unk ) {

          inflag = lcl_inflag;

          /* there is a subtle  trick here - a point is "force added" by setting the flags pqFace and qpFace */
          if (code == '2')
            nco_sph_add_pnt(R->shp, r, p);

          if (inflag == poly_vrl_pin) {
            pqFace = 1;
            qpFace = 0;

          } else if (inflag == poly_vrl_qin) {
            pqFace = 0;
            qpFace = 1;
          }

          if (numIntersect++ == 0) {
            /* reset counters */
            aa = 0;
            bb = 0;
          }
        }


      }

      if(!isParallel) {
        code = nco_rll_seg_int(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q);


        if (code == '1' || code == 'e') {

          nco_sph_add_pnt(R->shp, r, p);

          if (numIntersect++ == 0) {
            /* reset counters */
            aa = 0;
            bb = 0;
          }

          inflag = (ipqLHS == 1 ? poly_vrl_pin : iqpLHS == 1 ? poly_vrl_qin : inflag);


          if (DEBUG_SPH)
            printf("%%InOut sets inflag=%s\n", nco_poly_vrl_flg_sng_get(inflag));

        }
      }

      if(DEBUG_SPH)
        printf("numIntersect=%d code=%c (ipqLHS=%d, ip1qLHS=%d), (iqpLHS=%d, iq1pLHS=%d), (qpFace=%d pqFace=%d)\n",numIntersect, code, ipqLHS, ip1qLHS,  iqpLHS,iq1pLHS, qpFace,pqFace);



      if (qpFace && pqFace)  {

        /* Advance either P or Q which has previously arrived ? */
        if(inflag == poly_vrl_pin) nco_sph_add_pnt(R->shp,r, P->shp[a]);

        aa++;a++;


      } else if (qpFace) {
        if(inflag == poly_vrl_qin) nco_sph_add_pnt(R->shp,r, Q->shp[b]);

        bb++;b++;


        /* advance q */
      } else if (pqFace) {
        /* advance p */
        if(inflag == poly_vrl_pin) nco_sph_add_pnt(R->shp,r,P->shp[a]);

        aa++;a++;

      } else if (iqpLHS == -1) {
        /* advance q */
        //if(inflag== Qin) sAddPoint(R,r,Q->shp[b]);
        bb++;b++;

        /* cross product zero  */
      } else if( ipqLHS==0 && ip1qLHS==0 && iq1pLHS ==0 && iqpLHS ==0   ){
        if(inflag==poly_vrl_pin)
        {bb++;b++;}
        else
        {aa++;a++;}

      }



      else {
        /* catch all */
        if(inflag==poly_vrl_pin) nco_sph_add_pnt(R->shp,r,P->shp[a]);
        aa++;a++;

      }

    }

    a%=n;
    b%=m;

    if(DEBUG_SPH)
      fprintf(stdout, "\ndebug isGeared=%d a=%d aa=%d b=%d bb=%d \n",isGeared, a, aa, b, bb);

    /* quick exit if current point is same a First point  - nb an exact match ?*/
    //if( *r >3 &&  R->shp[0][3]==R->shp[*r-1][3] && R->shp[0][4]==R->shp[*r-1][4] )
    if( *r >3 &&  1.0 - nco_sph_dot_nm(R->shp[0], R->shp[*r-1]) < DOT_TOLERANCE  )
    {
      --*r;
      break;
    }


  } while ( ((aa < n) || (bb < m)) && (aa < 2*n) && (bb < 2*m) );

  return EXIT_SUCCESS;

}

nco_bool
nco_rll_is_lat_circle(double *p0, double *p1) {

  if (p0[3] != p1[3] && p0[4] == p1[4])
    return True;

  return False;

}


/* this function is really superflous but nco_sph_lhs
 * breaks nco_rll_interesect */
int nco_rll_lhs(double *Pi, double *Qi)
{
  double ds;

  ds= nco_sph_dot_nm(Pi, Qi);



  if(ds  > 0.0 )
    return 1;
  else if(ds <0.0)
    return -1;
  else
    return 0;



}




int
nco_rll_lhs_lat(double *p0, double *q0, double *q1)
{

  int iret;
  double nx=1;



  if(q0[3] > q1[3] )
    nx=-1;

  if(fabs(q0[3] - q1[3]) > M_PI)
    nx*=-1.0;

  if(p0[4] > q0[4])
    iret=1;
  else if(p0[4] < q0[4] )
    iret=-1;
  else
    iret=0;

  /* reverse direction maybe*/
  return iret*nx;

}





char
nco_rll_seg_int(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1)
{

  char code='0';
  nco_bool bDeg=False;
  nco_bool isP_LatCircle=False;
  nco_bool isQ_LatCircle=False;

  isP_LatCircle=nco_rll_is_lat_circle(p0, p1);
  isQ_LatCircle=nco_rll_is_lat_circle(q0, q1);


  /* longitude P may hit small ciricle Q */
  if(!isP_LatCircle && isQ_LatCircle )
  {
    /* Check longitude range */
    if( nco_sph_between(q0[3], q1[3], p0[3] )  &&  nco_sph_between(p0[4], p1[4], q0[4] ) ) {
      r0[3] = p0[3];
      r0[4] = q0[4];
      code ='1';
    }

  }
    /* longitude Q may hit small circle P */
  else if(isP_LatCircle && !isQ_LatCircle)
  {
    /* Check range range */
    if( nco_sph_between(p0[3], p1[3], q0[3] )  &&  nco_sph_between(q0[4], q1[4], p0[4] ) ) {
      r0[3] = q0[3];
      r0[4] = p0[4];
      code='1';
    }

  }

  if(code =='1')
    nco_geo_lonlat_2_sph(r0[3], r0[4], r0, bDeg);


  return code;


}

char
nco_rll_seg_parallel(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1, poly_vrl_flg_enm *inflag ) {

  int id;

  char code = '0';

  int p_sgn=1;
  int q_sgn=1;

  nco_bool isP_LatCircle = False;
  nco_bool isQ_LatCircle = False;


  isP_LatCircle = nco_rll_is_lat_circle(p0, p1);
  isQ_LatCircle = nco_rll_is_lat_circle(q0, q1);

  /* check for brain dead call */
  if (isP_LatCircle != isQ_LatCircle)
    return code;


  if (isP_LatCircle)
    id = 3;
  else
    id = 4;

  /* check sense of direction - they must be the same for both spans */
  /* check a longitude span
  if(!isP_LatCircle){

     if( ! (p0[id] > p1[id] != q0[id] > q1[id]) )
       return 'X';

  }
  else{

     if(p0[id] < p1[id])
        p_sgn*=-1;

     if(fabs(p1[id] - p0[id]) > M_PI)
        p_sgn*=-1.0;

     if(q0[id] < p1[id] )
        q_sgn*=-1.0;

    if(fabs(q1[id] - q0[id]) > M_PI)
      p_sgn*=-1.0;


    if(p_sgn != q_sgn )
      return 'X';

  }
  */


  if ( !nco_sph_between(p0[id], p1[id], q0[id]) && nco_sph_between(p0[id], p1[id], q1[id]) ) {
    nco_sph_adi(r0, p0);
    nco_sph_adi(r1, q1);
    *inflag=poly_vrl_qin;
    code = '2';
  } else if ( nco_sph_between(p0[id], p1[id], q0[id]) && !nco_sph_between(p0[id], p1[id], q1[id]) ) {
    nco_sph_adi(r0, q0);
    nco_sph_adi(r1, p1);
    *inflag=poly_vrl_pin;
    code = '2';

  } else if ( nco_sph_between(p0[id], p1[id], q0[id]) && nco_sph_between(p0[id], p1[id], q1[id])) {
    nco_sph_adi(r0, q0);
    nco_sph_adi(r0, q1);
    *inflag=poly_vrl_qin;
    code = '2';
  } else if (nco_sph_between(q0[id], q1[id], p0[id]) && nco_sph_between(q0[id], q1[id], p1[id])) {
    nco_sph_adi(r0, p0);
    nco_sph_adi(r1, p1);
    *inflag=poly_vrl_pin;
    code = '2';

  } else
    code = '0';

  return code;

}


void nco_rll_add_pnt(double **R, int *r, double *P)
{


  if(DEBUG_SPH)
    nco_sph_prn_pnt("nco_rll_add_pnt()", P, 3, True);


  memcpy(R[*r], P, sizeof(double)*NBR_SPH);
  (*r)++;




}

/*****************************************************************************************/
