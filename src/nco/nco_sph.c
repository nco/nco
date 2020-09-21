/* $Header$ */

/* Purpose: Spherical polygon coordinates and intersections */

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

#include "nco_sph.h" /* Spherical geometry intersections */

/* All in this file */
int DEBUG_SPH=0;

/* Global variables for latitude, longitude in RADIANS
   these may be set in nco_poly.c or
   should be safe with OPenMP   */

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

double  nco_sph_cross_chk(double *a, double *b, double *c)
{
  const char fnc_nm[]="nco_sph_cross()";
  //
  double n1;

  c[0]=a[1]*b[2]-a[2]*b[1];
  c[1]=a[2]*b[0]-a[0]*b[2];

  /* check if longitude  the same ? - then Z component must be zero */
  c[2]= ( a[3]==b[3] ? 0.0 : a[0]*b[1]-a[1]*b[0] );

  // normalize vector
  n1=sqrt( c[0]*c[0]+c[1]*c[1] + c[2]*c[2] );

  if(0 && DEBUG_SPH)
    printf("%s: n1=%.15f (%.15f, %.15f %.15f)\n",fnc_nm, n1, c[0],c[1], c[2]);


  if( n1 >  0.0 && n1 != 1.0  )
  {
    c[0] /= n1;
    c[1] /= n1;
    c[2] /= n1;
  }

  return n1;
}

/* for vectors a,b close together we get a "better" cross product  -by
 * subtracting them first i.e  (a-b) X b --  mathematically speaking it makes
 * no difference - but this is the finite world of doubles */
double  nco_sph_cross_sub(double *a, double *b, double *c)
{
  //
  double n2;
  double tmp[NBR_SPH];

  /* see if point a and point b on exactly the same same meridian */
  if(a[3]==b[3]) return nco_sph_cross_chk(a, b, c);

   nco_sph_sub(a,b,tmp);

   // nco_sph_add_lonlat(tmp);

   n2=nco_sph_cross(tmp, b, c);

   return n2;
}

/* spherical functions */
int nco_sph_intersect(poly_sct *P, poly_sct *Q, poly_sct *R, int *r, int flg_snp_to, const char *pq_pre)
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

   int pqCross[4]={0,0,0,0};
   /* pqCross before implied facing rules */
   int pqCrossOriginal[4]={0,0,0,0};

   nco_bool isParallel=False;

   double nx1;
   double nx2;
   double nx3;
   double dx1;

   char codes[]="0000";

   double Pcross[NBR_SPH];
   double Qcross[NBR_SPH];
   double Xcross[NBR_SPH];

   double p[NBR_SPH];
   double q[NBR_SPH];

   vrt_info_sct  vrt_info[VP_MAX];

   poly_vrl_flg_enm inflag= poly_vrl_unk;

   nco_edg_typ_enm p_edg_typ=nco_edg_gtc;
   nco_edg_typ_enm q_edg_typ=nco_edg_gtc;

   n=P->crn_nbr;
   m=Q->crn_nbr;

   if(DEBUG_SPH)
      fprintf(stdout, "%s: just entered %s\n", nco_prg_nm_get(), fnc_nm);

   do{
      a1 = (a + n - 1) % n;
      b1 = (b + m - 1) % m;

      /* skip identical points - remember nco_sph_cross_sub - checks longitude */
      while(( nx1= nco_sph_cross_sub(P->shp[a1], P->shp[a], Pcross) <DOT_TOLERANCE) )
	{
       aa++;a++;
       a%=n;

       if(a==a1)
         return EXIT_FAILURE;
     }

     /* skip identical points - remember nco_sph_cross_sub - checks longitude */
     while( (nx2= nco_sph_cross_sub(Q->shp[b1], Q->shp[b], Qcross) < DOT_TOLERANCE) )
     {
       bb++;b++;
       b%=m;
       if(b==b1)
         return EXIT_FAILURE;
     }

     nx3= nco_sph_cross(Pcross, Qcross, Xcross);

     (void)nco_sph_mk_pqcross(P->shp[a1],P->shp[a], Pcross, Q->shp[b1], Q->shp[b], Qcross, pqCross, nco_edg_gtc, &p_edg_typ, &q_edg_typ);

     /* save Cross before implied */
     memcpy(pqCrossOriginal, pqCross, sizeof(pqCross) );

     /* imply facing rules  */
     if(!pqCross[0])
       pqCross[0]=-pqCross[1];
     else if(!pqCross[1])
       pqCross[1]=-pqCross[0];

     if(!pqCross[2])
       pqCross[2]=-pqCross[3];
     else if(!pqCross[3])
       pqCross[3]=-pqCross[2];

     if(!pqCross[0] && !pqCross[1] && !pqCross[2] && !pqCross[3] )
       isParallel=True;
     else
       isParallel=False;

      /* now calculate face rules */
     qpFace = nco_sph_face(pqCross[0], pqCross[1], pqCross[3]);
     pqFace = nco_sph_face(pqCross[2], pqCross[3], pqCross[1]);

     /* Xcross product near zero !! so make it zero*/
      dx1=1.0- nco_sph_dot_nm(Pcross,Qcross );

      /* spans parallel we cannot rely on all pqCross[0..3]=0 so we check again here */
      if(dx1 < DOT_TOLERANCE && !isParallel)
      {
        isParallel=True;
        pqCross[0]=0;
        pqCross[1]=0;
        pqCross[2]=0;
        pqCross[3]=0;
      }
      else if( fabs(dx1-2.0) < SIGMA_TOLERANCE )
      {
        if(nco_dbg_lvl_get() >= nco_dbg_dev)
          (void)fprintf(stderr, "%s:%s() PARALLEL edges in oposite direction a=%d b=%d\n", nco_prg_nm_get(), fnc_nm, a , b );

        return EXIT_FAILURE;
      }

      if( isGeared == False)
      {
        if(  (pqCross[1] == 1 && pqCross[3] == 1) ||  ( qpFace && pqFace )     )
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
        /* pq_pre may be NULL or first char is null */
        if(inflag==poly_vrl_unk && ( pq_pre &&  pq_pre[0]!='\0') )
        {
          if(pq_pre[b]=='i')
            { inflag=poly_vrl_qin; bb=0; }
          else if(  (pq_pre[b]=='e' || pq_pre[b]=='v') && pq_pre[b1]=='i'  )
          { inflag=poly_vrl_qin; bb=0; }

        }

        if(isParallel)
        {
          char pcode[]={'0'};
          nco_bool bInt=False;

          poly_vrl_flg_enm lcl_inflag = poly_vrl_unk;

          if( p_edg_typ == nco_edg_gtc  && q_edg_typ == nco_edg_gtc  )
            bInt = nco_sph_seg_parallel(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q, &lcl_inflag, pcode);
          else if(p_edg_typ == nco_edg_smc && q_edg_typ == nco_edg_smc )
            bInt = nco_rll_seg_parallel(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q, &lcl_inflag, pcode);

          if (bInt &&  !(pcode[0]=='1' && inflag== poly_vrl_unk)   ) {

            /* nco_sph_parallel returns poly_vrl_unk when the vertices q1 and p1 are the same -so we have to peek ahead
             * to determine which flag to set */
            if(pcode[0]=='2' && lcl_inflag==poly_vrl_unk )
            {
              double dx1=0.0;
              double dx2=0.0;

              double Pp[NBR_SPH];
              double Pn[NBR_SPH];
              double Qn[NBR_SPH];

              nco_sph_sub(P->shp[a1], P->shp[a], Pp);
              nco_sph_sub(P->shp[ (a+n+1)%n], P->shp[a], Pn);

              nco_sph_sub(Q->shp[ (b+m+1)%m ],P->shp[a], Qn);

              dx1=1.0-nco_sph_dot(Pp,Pn);
              dx2=1.0-nco_sph_dot(Pp, Qn);

              if(dx1 >dx2)
                lcl_inflag=poly_vrl_qin;
              else
                lcl_inflag=poly_vrl_pin;

              if(DEBUG_SPH)
              {
                printf("%s:nco_sph_seg_parallel() has returned inflag==poly_vrl_unk. Peek ahead done inflag=%s\n",fnc_nm, nco_poly_vrl_flg_sng_get(lcl_inflag)   );
              }
            }

            inflag = lcl_inflag;

            /* there is a subtle  trick here - a point is "force added" by setting the flags pqFace and qpFace */
            //if (pcode[0] == '2')
            //  nco_sph_add_pnt(R->shp, r, p);

            if (inflag == poly_vrl_pin) {

              if(pcode[0]=='2')
                nco_sph_add_pnt_chk(vrt_info, inflag, a1,-1, R->shp, r, p  );

              pqFace = 1;
              qpFace = 0;

            } else if (inflag == poly_vrl_qin) {

              if(pcode[0]=='2')
                nco_sph_add_pnt_chk(vrt_info, inflag, -1, b1,  R->shp, r, p  );

              pqFace = 0;
              qpFace = 1;
            }

            if (numIntersect++ == 0) {
              /* reset counters */
              aa = 0;
              bb = 0;
            }
          }

          if(pcode[0]=='1')
            strcpy(codes,"p0");
          if(pcode[0]=='2')
            strcpy(codes,"pp");
        }

        if( pqCross[0]*pqCross[1]==-1  && pqCross[2]*pqCross[3]==-1)
        {
          nco_bool bInt=False;

          if(p_edg_typ == nco_edg_gtc && q_edg_typ == nco_edg_gtc )
              bInt=nco_sph_seg_int(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b],  p, q, pqCrossOriginal, flg_snp_to, codes);
          else if(p_edg_typ==nco_edg_gtc && q_edg_typ== nco_edg_smc)
              bInt=nco_sph_seg_smc(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b],  p, q, pqCrossOriginal, flg_snp_to, codes);
          else if(p_edg_typ==nco_edg_smc && q_edg_typ== nco_edg_gtc)
              bInt=nco_sph_seg_smc( Q->shp[b1], Q->shp[b], P->shp[a1], P->shp[a],  p, q, pqCrossOriginal, flg_snp_to, codes);

          if(bInt) {
            /* if here then there is some kind of intersection
             * nb IMPORTANT - if inflag is unset then first intersection must be proper to set the inflag
             * i.e NOT an edge or vertex intersection */
            if (inflag != poly_vrl_unk || (codes[0] == '1' && codes[1] == '1'))
              inflag = (pqCross[1] == 1 ? poly_vrl_pin : pqCross[3] == 1 ? poly_vrl_qin : inflag);

            if (inflag != poly_vrl_unk) {
              int p_arg;
              int q_arg;

              p_arg = (codes[0] == 'h' ? a : codes[0] == 't' ? a1 : -1);
              q_arg = (codes[1] == 'h' ? b : codes[1] == 't' ? b1 : -1);

              nco_sph_add_pnt_chk(vrt_info, inflag, p_arg, q_arg, R->shp, r, p);

              if (numIntersect++ == 0) {
                /* reset counters */
                aa = 0;
                bb = 0;
              }
            }

            if (DEBUG_SPH)
              printf("%%InOut sets inflag=%s\n", nco_poly_vrl_flg_sng_get(inflag));
          }
        }

         if(DEBUG_SPH){
            printf("numIntersect=%d codes=%s (ipqLHS=%d, ip1qLHS=%d), (iqpLHS=%d, iq1pLHS=%d), (qpFace=%d pqFace=%d) inflag=%s\n",numIntersect, codes, pqCross[1], pqCross[0],  pqCross[3], pqCross[2], qpFace,pqFace, nco_poly_vrl_flg_sng_get(inflag));

            printf("dx1=%.15f nx3=%.15f\n", dx1, nx3);
            nco_sph_prn_pnt("PCross",Pcross,4,True );
            nco_sph_prn_pnt("QCross",Qcross,4,True );
          }

         if (qpFace && pqFace)  {
            /* Advance either P or Q which has previously arrived ? */
            if(inflag == poly_vrl_pin) nco_sph_add_pnt_chk(vrt_info, inflag, a,-2,  R->shp,r, P->shp[a]);

            aa++;a++;

         } else if (qpFace) {
            if(inflag == poly_vrl_qin) nco_sph_add_pnt_chk(vrt_info, inflag, -2,b, R->shp,r, Q->shp[b]);

            bb++;b++;

            /* advance q */
         } else if (pqFace) {
            /* advance p */
            if(inflag == poly_vrl_pin) nco_sph_add_pnt_chk(vrt_info, inflag, a,-2, R->shp,r,P->shp[a]);

            aa++;a++;

         } else if (pqCross[3] == -1) {
            /* advance q */
            //if(inflag== Qin) sAddPoint(R,r,Q->shp[b]);
            bb++;b++;

            /* cross product zero  */
         } else if(isParallel){
            if(inflag==poly_vrl_pin)
            {bb++;b++;}
            else
            {aa++;a++;}
         }
         else {
            /* catch all */
            if(inflag==poly_vrl_pin) nco_sph_add_pnt_chk(vrt_info, inflag, a,-2,  R->shp,r,P->shp[a]);
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
      //if( *r >3 &&  1.0 - nco_sph_dot_nm(R->shp[0], R->shp[*r-1]) < DOT_TOLERANCE  )
     if( *r >3 &&  (nco_sph_metric(R->shp[0], R->shp[*r-1])==False ||  nco_sph_vrt_info_cmp( &vrt_info[0], &vrt_info[*r-1])  ) )
      {
         --*r;
         break;
      }

   } while ( ((aa < n) || (bb < m)) && (aa < 2*n) && (bb < 2*m) );

   return EXIT_SUCCESS;
}

int
nco_sph_mk_pqcross( double *p0, double *p1, double *pCross, double *q0, double  *q1, double *qCross, int pqCross[], nco_edg_typ_enm rgr_edg_typ,   nco_edg_typ_enm *p_edg_typ, nco_edg_typ_enm *q_edg_typ)
{

  nco_bool isP_LatCircle =False;
  nco_bool isQ_LatCircle =False;

  if(rgr_edg_typ == nco_edg_smc ){
    isP_LatCircle = nco_rll_is_lat_circle(p0, p1);
    isQ_LatCircle = nco_rll_is_lat_circle(q0, q1);
  }


  if (isQ_LatCircle) {

    pqCross[0] = nco_rll_lhs_lat(p0, q0, q1);
    pqCross[1] = nco_rll_lhs_lat(p1, q0, q1);
  }else{
    pqCross[0] = nco_sph_lhs(p0, qCross);
    pqCross[1] = nco_sph_lhs(p1, qCross);

  }

  

  if (isP_LatCircle){
    pqCross[2] = nco_rll_lhs_lat(q0, p0, p1);
    pqCross[3] = nco_rll_lhs_lat(q1, p0, p1);
  }else{
    pqCross[2] = nco_sph_lhs(q0, pCross);
    pqCross[3] = nco_sph_lhs(q1, pCross);
  }


  *p_edg_typ= ( isP_LatCircle ? nco_edg_smc : nco_edg_gtc );
  *q_edg_typ= ( isQ_LatCircle ? nco_edg_smc : nco_edg_gtc );

  return EXIT_SUCCESS;

}




char  nco_sph_seg_int_old(double *a, double *b, double *c, double *d, double *p, double *q)
{
  const char fnc_nm[]="nco_sph_seg_int_old()";

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



  if(DEBUG_SPH)
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



  if(DEBUG_SPH)
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



/* 1.0 - dot product normalized */
double  nco_sph_dot_sp(double *a, double *b)
{
  int idx;
  double sum=0.0;
  double n1;
  double n2;

  const char fnc_nm[]="nco_sph_dot_nm()";

  n1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  n2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);


  sum=n1*n2;

  for(idx=0; idx<3; idx++)
    sum-=a[idx]*b[idx];




  if(sum!=0.0 &&  n1>DOT_TOLERANCE ) sum/=n1;

  if(sum!=0.0 && n2>DOT_TOLERANCE) sum/=n2;


  if(0 && DEBUG_SPH)
    fprintf(stderr,"%s() dt=%f n1=%f %f\n", fnc_nm, sum, n1, n2 );


  return sum;



}

/* function used to compare  points */
/*  returns True if points distinct */
nco_bool
nco_sph_metric( double *p, double *q)
{

  double dist;
  nco_bool flg_old=False;

  if(flg_old)
  {


    if(1.0 - nco_sph_dot_nm(p, q) > DOT_TOLERANCE)
      return True;

    return False;
  }




  /* this is pythagorean  distance */
  dist=sqrt( (p[0]-q[0]) * (p[0]-q[0]) + (p[1]-q[1])*(p[1]-q[1]) + (p[2]-q[2])*(p[2]-q[2]))  ;

  if( dist > DIST_TOLERANCE)
    return True;

  return False;

}




int
nco_sph_metric_int(double *c, double *d, double *Icross)
{

  char fnc_nm[]="nco_sph_metric_int()";
  nco_bool DEBUG_LCL=False;


  nco_bool bInvert=False;

  int iret=0;

  double cIcross[NBR_SPH];
  double cd[NBR_SPH];
  double cd_rad=0.0;
  double i_rad=0.0;
  double idot=0.0;

  //double LCL_TOLERANCE=1.0e-40;


  nco_sph_sub(d,c,cd );

  nco_sph_sub(Icross,c, cIcross);
  cd_rad=nco_sph_rad(cd);

  i_rad=nco_sph_rad(cIcross);




  if(i_rad > 1.0e-10)
  {
    idot=nco_sph_dot(cd, cIcross);
    bInvert=False;
  }
  /* distance between c and Icross is very small so compare Icross with d */
  else
  {
    nco_sph_sub(Icross,d, cIcross);
    i_rad=nco_sph_rad(cIcross);
    idot=nco_sph_dot(cd, cIcross);
    idot*=-1.0;
    bInvert=True;

  }


  if( idot >1.0e-40)
  {
    /* very close to vertex */
    if( fabs(cd_rad-i_rad) <2.0e-14  )
      iret=( bInvert ? 2: 3);
    /* in the middle */
    else if( cd_rad>i_rad)
      iret=1;
    /* off the end i_rad>cd_rad */
    else
      iret=0;
  }
  else
  {
    iret=0;
  }



  if(DEBUG_LCL)
    (void)fprintf(stderr, "%s: Using sph_metric to compare points cd_rad=%.15e ci_rad=%.15e  cdot=%.15e iret=%d\n", fnc_nm, cd_rad, i_rad, idot, iret);



  return iret;


}



/* nb this func assumes that edge p and edge q are NOT parallel and that we have only one or two zero's 
 * pqCross MUST have at least one zero for this to work  */ 
nco_bool
nco_sph_seg_edge(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1, int *pqCross, char *codes)
{

  const char fnc_nm[]="nco_sph_seg_edge()";
  

  int flg_ab=0;
  int flg_cd=0;
  

  if(pqCross[0]==0)
  {
    nco_sph_adi(r0,p0);
    flg_ab=2;
  }  
  else if(pqCross[1]==0)
  {
    nco_sph_adi(r0,p1);
    flg_ab=3;
  }  
  
  else if(pqCross[2]==0)
  {
    nco_sph_adi(r0,q0);
    flg_cd=2;
  }
  
  else if(pqCross[3]==0)
  {
    nco_sph_adi(r0,q1);
    flg_cd=3;
  }  
  
  if(flg_ab )
    flg_cd=nco_sph_metric_int(q0,q1, r0);
  else if(flg_cd)
    flg_ab=nco_sph_metric_int(p0,p1, r0);


  if(DEBUG_SPH ) {
    fprintf(stderr, "%s:  flg_ab=%d flg_cd=%d\n", fnc_nm, flg_ab, flg_cd);
    nco_sph_prn_pnt("nco_sph_seg_edge(): pos point ", r0, 4, True);
  }

  if(!flg_ab || !flg_cd)
     return False;

  codes[0]=( flg_ab==2 ? 't' : flg_ab==3 ? 'h' :'1' );
  codes[1]=( flg_cd==2 ? 't' : flg_cd==3 ? 'h' :'1' );



  if(DEBUG_SPH )
    fprintf(stderr, "%s: codes=%s\n", fnc_nm, codes);


  return True;


}


nco_bool
nco_sph_seg_int(double *p0, double *p1, double *q0, double *q1, double *r0,  double *r1, int *pq_cross, int flg_snp_to, char *codes)
{
  const char fnc_nm[]="nco_sph_seg_int()";

  nco_bool bSwap=False;
  nco_bool bInt=False;
  nco_bool bSetCnd=False;

  nco_bool bTstVertex=False;
  int flg_p=0;
  int flg_q=0;




  double n1;

  /* possible intersection point */
  double pcnd[NBR_SPH];
  /* placeholder ? */
  double pt[NBR_SPH]={0.0,0.0,0.0,0.0,0.0};

  int pq_cross_lcl[4];


  /* set to no intersection */
  codes[0]='0';
  codes[1]='0';


  /* populate pqCross if arg null*/
  if(!pq_cross) {
    double Pcross[NBR_SPH];
    double Qcross[NBR_SPH];

    /* remeber nco_sph_cross_sub - requires presence of londitude - p0[3], p1[3] */
    nco_sph_cross_sub(p0, p1, Pcross);
    nco_sph_cross_sub(q0, q1, Qcross);

    pq_cross_lcl[0] = nco_sph_lhs(p0, Qcross);
    pq_cross_lcl[1] = nco_sph_lhs(p1, Qcross);
    pq_cross_lcl[2] = nco_sph_lhs(q0, Pcross);
    pq_cross_lcl[3] = nco_sph_lhs(q1, Pcross);

    pq_cross=pq_cross_lcl;

  }


  /* pre test for vertex/vertex intersections */
  if(bTstVertex)
  {
    if( !pq_cross[1] && !pq_cross[3] &&  !nco_sph_metric(p1,q1))
    { flg_q=3; flg_p=3; memcpy(r0, p1, sizeof(double)*NBR_SPH); }

    else if( !pq_cross[1] && !pq_cross[2] &&  !nco_sph_metric(p1,q0))
    { flg_q=3; flg_p=2; memcpy(r0, p1, sizeof(double)*NBR_SPH);}

    else if(!pq_cross[0] && !pq_cross[3] && !nco_sph_metric(p0,q1))
    {flg_q=2; flg_p=3; memcpy(r0, p0, sizeof(double)*NBR_SPH); }

    else if(!pq_cross[0] && !pq_cross[2] && !nco_sph_metric(p0,q0))
    {flg_q=2; flg_p=2; memcpy(r0, p0, sizeof(double)*NBR_SPH); }

    if(flg_q && flg_p)
    {

      codes[0]=( flg_q==2 ? 't' : flg_q==3 ? 'h' :'1' );
      codes[1]=( flg_p==2 ? 't' : flg_p==3 ? 'h' :'1' );



      if(DEBUG_SPH)
        fprintf(stderr, "%s: codes=%s - quick vertex return\n", fnc_nm, codes );

      return True;

    }
    else
    {
      flg_q=0; flg_p=0;
    }

  }



  /* The plane / line intersection method doesnt work very well when the end point of the line is on
  * or very near to the plane - so we swap the values around and use  {q0,q1,Origin}  as plane and {p0,p1} as line */
  /*
  if( pq_cross[0]*pq_cross[1] !=0 && pq_cross[2]* pq_cross[3]==0  )
    {
      dswp=p0; p0=q0; q0=dswp;
      dswp=p1; p1=q1; q1=dswp;

      bSwap=True;
    }
  */

  bInt=nco_mat_int_pl(p0, p1, q0, q1, pt);
  /* no intersection */




  if (DEBUG_SPH) {
    fprintf(stderr, "%s: bInt=%s codes=%s tpar=X[0]=%.16f X[1]=%.16f X[2]=%.16f\n", fnc_nm, (bInt ? "True" : "False"),
            codes, pt[0], pt[1], pt[2]);

    /* * if(  ( pq_cross[0]*pq_cross[1] != 0 && pq_cross[2]* pq_cross[3]==0 )  ||  ( pq_cross[0]*pq_cross[1] == 0 && pq_cross[2]* pq_cross[3]!=0 ))
      fprintf(stderr,"WARNING pq_cross[*] swapped\n" );
      */


  }

  if(!bInt )
    return False;

  /* this is quite permissive  as some  returned points maybe in the range <1.0e-14, or 1.0+1.-e-14
     The points are double checked later with flg_p=nco_sph_metric_int(q0,q1, pcnd); */
  /* if(   pt[0] < -1.0e-11  ||  ( pt[0] >1.0 &&  pt[0]-1.0 >1.0e-11) )
     return False; */




  /* from here on we have some kind of intersection */
  if(!bSetCnd)
  {
    /* genuine intersection point not end point */
    pcnd[0] = q0[0] + pt[0] * (q1[0] - q0[0]);
    pcnd[1] = q0[1] + pt[0] * (q1[1] - q0[1]);
    pcnd[2] = q0[2] + pt[0] * (q1[2] - q0[2]);

    n1 = nco_sph_rad(pcnd);
    /* normalise p */
    if (n1 != 0.0 && n1 != 1.0) {

      pcnd[0] /= n1;
      pcnd[1] /= n1;
      pcnd[2] /= n1;
    }
    nco_sph_add_lonlat(pcnd);

    bSetCnd=True;

  }



  flg_q=nco_sph_metric_int(p0,p1, pcnd);

  if(DEBUG_SPH) {
      nco_sph_prn_pnt("nco_sph_seg_int_: pos point ", pcnd, 4, True);
      (void)fprintf(stderr, "%s: flg_ab=%d\n", fnc_nm,  flg_q );
  }



  if(!flg_q)
      return False;

  flg_p=nco_sph_metric_int(q0,q1, pcnd);

  if(!flg_p)
    return False;


  if(bSwap)
  {
    int flg_tmp=flg_q;
    flg_q=flg_p;
    flg_p=flg_tmp;
  }


  codes[0]=( flg_q==2 ? 't' : flg_q==3 ? 'h' :'1' );
  codes[1]=( flg_p==2 ? 't' : flg_p==3 ? 'h' :'1' );


  if(DEBUG_SPH )
      fprintf(stderr, "%s: codes=%s tpar=pt[0]=%.15f\n", fnc_nm, codes, pt[0]  );

  memcpy(r0, pcnd, sizeof(double)*NBR_SPH);

  return True;
}

nco_bool
nco_sph_seg_smc   /* intersect great circles and small circles */
(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1, int *pq_cross, int flg_snp_to, char *codes) {

  /* p0, p1 great circle */
  /* q0 q1 small circle */


  /* Intersection Method
   * we assume  that it has already been confirmed that q0, q1 is a small circle
   *
   * Determine the line of intertection of the plane containing the great arc and the plane
   * containing the small circle.
   *
   * plane of great circle  components ( n0, n1, n2)
   * plane of small circle H components ( 0.0, 0.0, h2)
   * unit vectors (i, j, k)
   *
   * P= h2 / (1 - n2^2) (n2*N +  k )
   * Q= (n1*i - n0*j) / sqrt( n1^2 + n2^2)
   *
   * Line of intersction
   * X=P + sQ  ( s is line parameter  )  and ||Q||=1
   *
   * Find the intersction points with the unit sphere.
   * ( we wish to solve for s )
   * 1=||X||=(P + sQ).(P + sQ)
   *
   * Expands out to a simple quadratic for s
   *  s = - (P.Q) +/-  sqrt(  (P.Q)^2 - (P.P) +1  )
   *
   * We are lucky P.Q =0 so formula simplifies to
   *  s0= sqrt( 1-  (P.P) )
   *  s1= -s0
   *
   *  Put s0/s1 into line formula to get the point(s) of intersection
   *
   * */

  const char fnc_nm[]="nco_sph_seg_smc()";

  nco_bool bSetr0=False;
  nco_bool bSetr1=False;

  int flg_ab=0;
  int flg_cd=0;

  double pqdot=0.0;
  double s0=0.0;
  double s1=0.0;
  double dtmp = 0.0;

  double PCross[NBR_SPH] = {0.0};
  double P[NBR_SPH] = {0.0};
  double Q[NBR_SPH] = {0.0};
  double N[NBR_SPH] = {0.0};

  /* set codes to 0 */
  strcpy(codes,"0000");

  /* nb remember cross normalizes
  nd1 = nco_sph_cross(p0, p1, PCross);
  */


  nco_sph_adi(N, PCross);

  /* determine P */
  nco_sph_adi(P, PCross);
  nco_sph_mlt(P, -1.0*N[2]);
  P[2] += 1.0;

  nco_sph_mlt(P,q0[2] / (1.0 - N[2] * N[2]));

  /* determine Q */
  Q[0] = N[1];
  Q[1] = -1.0* N[0];
  Q[2] = 0.0;

  nco_sph_mlt(Q, 1.0 / sqrt(1.0-N[2]*N[2]));


  /* now solve quadratic for r  pqdot ALWAYS zero */
  //pqdot = nco_sph_dot(P, Q);
  //dtmp = pqdot * pqdot - nco_sph_rad2(P) + 1.0;

  dtmp= 1.0 - nco_sph_rad2(P);

  if(DEBUG_SPH) {
    fprintf(stderr, "%s:%s: dtmp=%f pqdot=%.15f\n", nco_prg_nm_get(), fnc_nm, dtmp, pqdot);
    nco_sph_prn_pnt("nco_sph_seg_smc() - P", P, 4, True);

    nco_sph_prn_pnt("nco_sph_seg_smc() - Q", Q, 4, True);
  }



  if (dtmp < 0.0)
    return False;

  /* from here on we have one or two solutions */
  dtmp = sqrt(dtmp);

  s0=dtmp;
  s1-=dtmp;


  /* now plug s0  into line formula */
  nco_sph_adi(r0, Q);
  nco_sph_mlt(r0, s0 );
  //nco_sph_add(r0, P, r0   );
  r0[0]+=P[0]; r0[1]+=P[1] ; r0[2]+=P[2];

  nco_sph_add_lonlat(r0);
  flg_ab=nco_sph_metric_int(p0,p1, r0);
  flg_cd=nco_sph_metric_int(q0,q1,r0 );

  if(flg_ab && flg_cd )
  {
    codes[0]=( flg_ab==2 ? 't' : flg_ab==3 ? 'h' :'1' );
    codes[1]=( flg_cd==2 ? 't' : flg_cd==3 ? 'h' :'1' );
    bSetr0=True;
  }

  if(dtmp==0.0 )
    return bSetr0;



  /* now plug s0  into line formula */
  if(dtmp>0.0) {
    flg_ab=0;
    flg_cd=0;


    nco_sph_adi(r1, Q);
    nco_sph_mlt(r1, s1);
    nco_sph_add(r1, P, r1);

    nco_sph_add_lonlat(r1);

    flg_ab=nco_sph_metric_int(p0,p1, r1);
    flg_cd=nco_sph_metric_int(q0,q1,r1 );

    if(flg_ab && flg_cd )
    {
      codes[2]=( flg_ab==2 ? 't' : flg_ab==3 ? 'h' :'1' );
      codes[3]=( flg_cd==2 ? 't' : flg_cd==3 ? 'h' :'1' );
      bSetr1=True;
    }


  }


  /* shuffle the codes and vectors if r0 unset and r1 set */
  if(!bSetr0 && bSetr1)
  {
    codes[0]=codes[2];
    codes[1]=codes[3];
    codes[2]='0';
    codes[3]='0';
    nco_sph_adi(r0,r1);
  }

  if(DEBUG_SPH) {
    nco_sph_prn_pnt("nco_sph_seg_smc() - first soln", r0, 4, True);
    fprintf(stderr, "%s: radius r0=%.15f\n",fnc_nm, nco_sph_rad(r0));
    nco_sph_prn_pnt("nco_sph_seg_smc() - second soln", r1, 4, True);
    fprintf(stderr, "%s: radius r1=%.15f\n",fnc_nm, nco_sph_rad(r1));
    fprintf(stderr, "%s: codes=%s\n",fnc_nm, codes);

  }
    return (bSetr0 || bSetr1);
}

nco_bool
nco_sph_seg_int_1(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1, int flg_snp_to, char *codes)
{
  const char fnc_nm[]="nco_sph_seg_int()";



  nco_bool bInt=False;
  nco_bool bValid=False;

  nco_bool bTstVertex=True;
  int flg_cd=0;
  int flg_ab=0;


  double n1;
  /* possible intersection point */
  double pcnd[NBR_SPH];
  /* placeholder ? */
  double pt[NBR_SPH]={0.0,0.0,0.0,0.0,0.0};



  /* set to no intersection */
  codes[0]='0';
  codes[1]='0';


  /* pre test for vertex/vertex intersections */
  if(bTstVertex)
  {
    if(!nco_sph_metric(p1,q1))
    { flg_ab=3; flg_cd=3; memcpy(r0, p1, sizeof(double)*NBR_SPH); }
    else if(!nco_sph_metric(p1,q0))
    { flg_ab=3; flg_cd=2; memcpy(r0, p1, sizeof(double)*NBR_SPH);}
    else if(!nco_sph_metric(p0,q1))
    {flg_ab=2; flg_cd=3; memcpy(r0, p0, sizeof(double)*NBR_SPH); }
    else if(!nco_sph_metric(p0,q0))
    {flg_ab=2; flg_cd=2; memcpy(r0, p0, sizeof(double)*NBR_SPH); }

    if(flg_ab && flg_cd)
    {
      if(flg_ab==2)
        codes[0]='t';
      else if(flg_ab==3)
        codes[0]='h';

      if(flg_cd==2)
        codes[1]='t';
      else if(flg_cd==3)
        codes[1]='h';


      if(DEBUG_SPH)
        fprintf(stderr, "%s: codes=%s - quick vertex return\n", fnc_nm, codes );

      return True;

    }
    else
    {
      flg_ab=0; flg_cd=0;
    }

  }


  bInt=nco_mat_int_pl(p0, p1, q0, q1, pt);
  /* no intersection */




  if (DEBUG_SPH)
    fprintf(stderr, "%s: bInt=%s codes=%s tpar=X[0]=%.16f X[1]=%.16f X[2]=%.16f\n", fnc_nm,  (bInt ?  "True" : "False"), codes, pt[0], pt[1], pt[2] );


  if(!bInt ||  pt[0] < -1.0e-10  ||  ( pt[0] >1.0 &&  pt[0]-1.0 >1.0e-10) )
    return False;

  /* from here on we have some kind of intersection */


  /* genuine intersection point not end point */
  pcnd[0] = q0[0] + pt[0] * (q1[0] - q0[0]);
  pcnd[1] = q0[1] + pt[0] * (q1[1] - q0[1]);
  pcnd[2] = q0[2] + pt[0] * (q1[2] - q0[2]);

  n1 = nco_sph_rad(pcnd);
  /* normalise p */
  if (n1 != 0.0 && n1 != 1.0) {

    pcnd[0] /= n1;
    pcnd[1] /= n1;
    pcnd[2] /= n1;
  }
  nco_sph_add_lonlat(pcnd);

  bValid=nco_sph_metric_int(p0,p1, pcnd);

  if(DEBUG_SPH) {
    nco_sph_prn_pnt("nco_sph_seg_int_: pos point ", pcnd, 4, True);
    (void)fprintf(stderr, "%s: bValid=%s\n", fnc_nm,  (bValid ?  "True" : "False") );
  }

  /*
  if ( pt[0] < -1.0e-10  ||  ( pt[0] >1.0 &&  pt[0]-1.0 >1.0e-10)  )
    return False;
  */


  if(!bValid)
    return False;


  /* from here on we have a valid point */
  if(!nco_sph_metric(pcnd,p0)) {
    flg_ab = 2;
    // memcpy(r0, p0, sizeof(double)*NBR_SPH);

  }else if(!nco_sph_metric(pcnd, p1)) {
    flg_ab = 3;
    // memcpy(r0, p1, sizeof(double)*NBR_SPH);
  }else
    flg_ab=1;


  /* from here on we have a valid point */
  if(!nco_sph_metric(pcnd,q0)) {
    flg_cd = 2;
    // memcpy(r0, q0, sizeof(double)*NBR_SPH);
  }else if(!nco_sph_metric(pcnd, q1)) {
    flg_cd = 3;
    // memcpy(r0, p1, sizeof(double)*NBR_SPH);
  }else
    flg_cd=1;



  if(flg_ab==2)
    codes[0]='t';
  else if(flg_ab==3)
    codes[0]='h';
  else
    codes[0]='1';

  if(flg_cd==2)
    codes[1]='t';
  else if(flg_cd==3)
    codes[1]='h';
  else
    codes[1]='1';


  if(DEBUG_SPH )
    fprintf(stderr, "%s: codes=%s tpar=pt[0]=%.15f\n", fnc_nm, codes, pt[0]  );

  memcpy(r0, pcnd, sizeof(double)*NBR_SPH);

  return True;



}








nco_bool
nco_sph_seg_parallel(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1, poly_vrl_flg_enm *inflag, char *codes )
{

  const char fnc_nm[] = "nco_sph_seg_parallel()";

  //char code='0';
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

  codes[0]='0';
  //codes[1]='0';


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
    return False;

  dx_p1 = 1.0 - nco_sph_dot_nm(p0, p1);

  dx_q0 = 1.0 - nco_sph_dot_nm(p0, q0);

  /*
  if( dx_q0< DOT_TOLERANCE)
    dx_q0=0.0;
  */
  if(dx_q0<0.0)
    dx_q0=fabs(dx_q0);


  if (dx_q0 != 0.0) {
    nx3 = nco_sph_cross(p0, q0, Tcross);

    if (nco_sph_dot_nm(Pcross, Tcross) < 0.0)
      dx_q0 *= -1.0;

  }

  dx_q1 = 1.0 - nco_sph_dot_nm(p0, q1);

  /*
  if(dx_q1 <DOT_TOLERANCE)
    dx_q1=0.0;
  */

  if(dx_q1<0.0)
    dx_q1=fabs(dx_q1);


  if (dx_q1 != 0.0) {
    nx3 = nco_sph_cross(p0, q1, Tcross);

    if (nco_sph_dot_nm(Pcross, Tcross) < 0.0)
      dx_q1 *= -1.0;
  }

  /* we now have 4 "points to order"
  * a=0.0, dx_p0, dx_q0 , dx_q1
   * always dx_p0 > 0.0 and dx_q0 < dx_q1
   * */

  /* no overlap so return */
  if( (dx_q0 < 0.0  && dx_q1 < 0.0) || ( dx_q0 > dx_p1 && dx_q1 > dx_p1  )) {
    codes[0] = '0';
  }

  else if(dx_q0 <0.0 &&  dx_q1 == 0.0   )
  {
    codes[0]='1';
    nco_sph_adi(r0,p0);
    /* not sure which flag to set here */
    *inflag=poly_vrl_qin;
  }
  else if( dx_q0 == dx_p1 && dx_q1 > dx_p1  )
  {
    codes[0]='1';
    nco_sph_adi(r0,p1);
    *inflag=poly_vrl_pin;
  }
    /* LHS overlap */
  else if (dx_q0 <0.0 &&  (dx_q1 >0.0 && dx_q1 <= dx_p1)  ) {
    codes[0]= '2';
    nco_sph_adi(r0, p0);
    nco_sph_adi(r1, q1);
    *inflag=poly_vrl_qin;

  }

    /* RHS overlap */
  else if( dx_q0 >=0.0 &&  dx_q0 < dx_p1 && dx_q1 > dx_p1    )
  {
    codes[0]= '2';
    nco_sph_adi(r0, q0);
    nco_sph_adi(r1, p1);
    *inflag=poly_vrl_pin;

  }
  else if(  dx_q0 >=0.0 && dx_q1 <= dx_p1    ) {
    codes[0]= '2';
    nco_sph_adi(r0, q0);
    nco_sph_adi(r1, q1);
    *inflag=poly_vrl_qin;
  }


  else if( dx_q0 <0.0 && dx_q1 > dx_p1    )
  {
    codes[0]='2';
    nco_sph_adi(r0,p0 );
    nco_sph_adi(r1,p1 );
    *inflag=poly_vrl_pin;
  } else{
    codes[0]='0';
  }


  /* points very close together - so use only first one*/
  if (codes[0] == '2' && !nco_sph_metric(r0, r1))
    codes[0] = '1';

  /* q1 and p1 vertices are the same */
  if(codes[0]=='2' && fabs( dx_q1 -dx_p1 )<DOT_TOLERANCE)
    *inflag=poly_vrl_unk;

  if(DEBUG_SPH ) {
    if (codes[0] >= '1')
      nco_sph_prn_pnt("nco_sph_seg_parallel(): intersect1", r0, 3, True);

    if (codes[0] == '2')
      nco_sph_prn_pnt("nco_sph_seg_parallel(): intersect2", r1, 3, True);

    (void)fprintf(stderr, "%s: dx_p1=%.16f dx_q0=%.16f dx_q1=%.16f\n",fnc_nm, dx_p1, dx_q0, dx_q1 );
    (void)fprintf(stderr, "%s: returning inflag=%s\n", fnc_nm, nco_poly_vrl_flg_sng_get(*inflag));

  }

  if(codes[0]!='0')
    return True;

  return False;
  

}


nco_bool
nco_sph_intersect_pre(poly_sct *sP, poly_sct *sQ, char sq_sng[]) {
  char fnc_nm[]="nco_sph_intersect_pre()";

  int n;
  int m;
  int idx = 0;
  int jdx;
  int jdx1;
  int numIntersect;


  nco_bool DEBUG_LCL=False;
  nco_bool bComplex = False;

  /* if true make control point INSIDE polygon
   * else make OUTSIDE polygon */
  nco_bool bInside=True;

  char codes[] ="00";


  double p[NBR_SPH];
  double q[NBR_SPH];


  double pi[NBR_SPH]={0.0};

  double pControl[NBR_SPH];





  /* fxm:2019-06-07 - there is a problem using the polygon center as
   * for some RLL grids the center of a polar triangle can be the pole itself
   * also there is a situation where for some polygons the center maybe outside polygon ?
   * Also if we use a point outide we dont have to bother (re)calculating the center
   */
    nco_sph_mk_control(sP, bInside, pControl);


  if(DEBUG_SPH)
    nco_sph_prn_pnt("nco_sph_intersect_pre()/control pnt", pControl,3,True);

  n = sP->crn_nbr;
  m = sQ->crn_nbr;



  for (idx = 0; idx < m; idx++) {
    bComplex = False;
    numIntersect = 0;

    memset(pi,0, sizeof(double)* NBR_SPH);

    for (jdx = 0; jdx < n; jdx++) {
      jdx1 = (jdx + n - 1) % n;

      /* check for parallel edges
      if(nco_sph_cross(sP->shp[jdx1], sP->shp[jdx], Pcross)<DOT_TOLERANCE)
        continue;
      */

      if(nco_sph_seg_int(sP->shp[jdx1], sP->shp[jdx], pControl, sQ->shp[idx],  p, q, (int*)NULL,0, codes)) {

        if (DEBUG_LCL) {
          (void) fprintf(stderr, "%s():, idx=%d jdx=%d numIntersect=%d codes=%s\n", __FUNCTION__, idx, jdx, numIntersect, codes);
          nco_sph_prn_pnt("intersect_pre()", p, 4, True);
        }

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
          (void) fprintf(stderr, "%s: idx=%d jdx=%d numIntersect=%d codes=%s\n", fnc_nm, idx, jdx, numIntersect, codes);
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

  int sz;
  int srt=0;
  int idx=0;

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
      if( (sq_sng[idx] == 'i' &&  sq_sng[(idx+1)%sz] =='o') || (sq_sng[idx]=='o' &&  sq_sng[(idx+1)%sz] == 'i')   )
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
  if( (numOutSide && numInside) &&  (  (!numEdges && !numVertex ) ||  (numEdges==1 && !numVertex) || (!numEdges && numVertex==1)  ) )
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

   if(0 && DEBUG_SPH)
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
  double sum = 0.0;
  double n1;
  double n2;

  const char fnc_nm[]="nco_sph_dot_nm()";

  for (idx = 0; idx < 3; idx++)
    sum += a[idx] * b[idx];

  n1 = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  n2 = sqrt(b[0] * b[0] + b[1] * b[1] + b[2] * b[2]);

  if (sum != 0.0 && n1 > DOT_TOLERANCE) sum /= n1;

  if (sum != 0.0 && n2 > DOT_TOLERANCE) sum /= n2;


  if (0 && DEBUG_SPH)
    fprintf(stderr, "%s() dt=%f n1=%f %f\n", fnc_nm, sum, n1, n2);


  return sum;
}


double  nco_sph_cross2(double *a, double *b, double *c)
{
  const char fnc_nm[]="nco_sph_cross2()";
  //
  double n1;

  c[0]=a[1]*b[2]-a[2]*b[1];
  c[1]=a[2]*b[0]-a[0]*b[2];
  c[2]=a[0]*b[1]-a[1]*b[0];

  // normalize vector
  n1=sqrt( c[0]*c[0]+c[1]*c[1] + c[2]*c[2] );


  if(0 && DEBUG_SPH)
    printf("%s: n1=%.15f (%.15f, %.15f %.15f)\n",fnc_nm, n1, c[0],c[1], c[2]);

  return n1;

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

  if(0 && DEBUG_SPH)
    printf("%s: n1=%.15f (%.15f, %.15f %.15f)\n",fnc_nm, n1, c[0],c[1], c[2]);


  if( n1 >  0.0 && n1 != 1.0  )
   {
      c[0] /= n1;
      c[1] /= n1;
      c[2] /= n1;
   }


   return n1;


}

/* return triple product */
double nco_sph_trp(double *a, double *b, double *c)
{
  double tot;

  tot=(a[1]*b[2]-a[2]*b[1])*c[0]+
      (a[2]*b[0]-a[0]*b[2])*c[1]+
      (a[0]*b[1]-a[1]*b[0])*c[2];


   return tot;

}


double nco_sph_rad(double *a){
  double n1;

  n1=sqrt( a[0]*a[0]+a[1]*a[1] + a[2]*a[2] );

  return n1;
}


double
nco_sph_rad2(double *a){
  double n1;

  n1= a[0]*a[0]+a[1]*a[1] + a[2]*a[2];

  return n1;
}


double
Sin(double theta, nco_bool blon){

    double dsign=1.0;
    double ms=0.0;


    if(blon &&  theta > M_PI && LON_MIN_RAD >=0.0 )
      theta-=LON_MAX_RAD;

    //if(theta <0.0 && theta > -1.0*M_PI)
    if(theta<0.0)
        dsign=-1.0;


    if( fabs(fabs(theta)-M_PI_2)<1.0e-2 )
    {
       ms = sqrt(0.5 * (1.0 - cos(2.0 * theta))) ;
       ms=copysign(ms, dsign);
    }
    else
      ms=sin(theta);

    return ms;
}


double
Cos(double theta, nco_bool blon){

  double ms=0.0;


  if(theta==0.0)
    return 1;



  if(blon &&  theta > M_PI && LON_MIN_RAD >=0.0 )
    theta-=LON_MAX_RAD;


  // if( fabs(theta)-M_PI_2<1.0e-6 )
  if( fabs(theta)< 1.0e-2 ||  fabs(fabs(theta)-M_PI)<1.0e-2 )
    ms= 1.0- 2.0*pow( sin(theta/2.0),2.0  );
  else
    ms=cos(theta);

  return ms;
}

/* new method for calculating cross product */
double
nco_sph_sxcross(double *a, double *b, double *c)
{
  //  char fnc_nm[]="nco_sph_sxcross";
  nco_bool bDeg = False;
  double n1;
  double lon1;
  double lon2;

  double lat1;
  double lat2;

  /* don't both with trig stuff if points on same meridian or same parallel

  if(fabs(a[3])==fabs(b[3]) ||  fabs(a[4])== fabs(b[4]))
    return nco_sph_cross2(a, b, c);
 */


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


  double sin_lat1_lat2_plus=Sin(lat1+lat2,False);
  double sin_lat1_lat2_minus=Sin(lat1-lat2,False);

  double cos_lon1_lon2_plus=Cos( (lon1+lon2)/2.0,True );
  double cos_lon1_lon2_minus=Cos( (lon1-lon2)/2.0,True );

  double sin_lon1_lon2_plus=Sin( (lon1+lon2)/2.0,True );
  double sin_lon1_lon2_minus=Sin( (lon1-lon2)/2.0,True );

  c[0]=sin_lat1_lat2_plus * cos_lon1_lon2_plus * sin_lon1_lon2_minus
       - sin_lat1_lat2_minus* sin_lon1_lon2_plus * cos_lon1_lon2_minus;

  c[1]=sin_lat1_lat2_plus * sin_lon1_lon2_plus * sin_lon1_lon2_minus
       + sin_lat1_lat2_minus* cos_lon1_lon2_plus * cos_lon1_lon2_minus;

  c[2]=Cos(lat1, False) * Cos(lat2,False) * Sin(lon2-lon1,True);



  /*
  c[0] =   sin(lat1+lat2) * cos( (lon1+lon2) / 2.0) * sin( (lon1-lon2)/2.0)
            - sin(lat1-lat2) * sin ((lon1+lon2) / 2.0) * cos( (lon1-lon2)/2.0);

   c[1] =   sin(lat1+lat2) * sin( (lon1+lon2) / 2.0) * sin( (lon1-lon2)/2.0)
            + sin(lat1-lat2) * cos ((lon1+lon2) / 2.0) * cos( (lon1-lon2)/2.0);

   c[2]=cos(lat1) * cos(lat2) * sin(lon2-lon1);

 */







   // normalize vector
   n1=sqrt( c[0]*c[0]+c[1]*c[1] + c[2]*c[2] );

   if(0 && n1 != 0.0 && n1 !=1.0  )
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



   /*
   double delta;
   //delta = ( *r==0 ? 0.0 :   2.0 *asin(    sqrt( pow( R[*r-1][0] - P[0],2 ) + pow( R[*r-1][1] - P[1],2 ) + pow( R[*r-1][2] - P[2],2 )  ) /2.0) );
   if(*r >0 )
      delta = 1.0 - nco_sph_dot(R[*r-1], P );

    */

   if(DEBUG_SPH)
      nco_sph_prn_pnt("aAddPoint():", P, 3, True);



   /* only add  point if its distinct from previous point */
   if ( *r==0 || nco_sph_metric(R[*r-1], P )  )
   {

      memcpy(R[*r], P, sizeof(double)*NBR_SPH);
      (*r)++;
   }


}


inline bool
nco_sph_vrt_info_cmp( vrt_info_sct *info_a, vrt_info_sct *info_b)
{
   if( info_a->p_vrt >=0 && info_b->p_vrt >=0 &&  info_a->p_vrt== info_b->p_vrt  )
     return True;

  if( info_a->q_vrt >=0 && info_b->q_vrt >=0 &&  info_a->q_vrt== info_b->q_vrt  )
    return True;

   return False;

}


/* add a vertex or intersection to vrt_info and Points sct
 *  vrt_nbr==-1 indicates an intersection  */

void
nco_sph_add_pnt_chk( vrt_info_sct *vrt_info, poly_vrl_flg_enm in_flag, int p_vrt, int q_vrt,  double **R, int *r, double *P)
{

  /* check if we are adding a vertex already on top of stack */

  if(*r>0 &&  (( p_vrt>=0  && vrt_info[*r-1].p_vrt == p_vrt ) ||  ( q_vrt>=0 && vrt_info[*r-1].q_vrt==q_vrt ))  )
    return;


  /* only add  point if its distinct from previous point */
  if ( *r==0 || nco_sph_metric(R[*r-1], P )  )
  {


    if(DEBUG_SPH)
      nco_sph_prn_pnt("nco_sph_add_pnt_chk():", P, 3, True);


    vrt_info[*r].in_flag=in_flag;
    vrt_info[*r].p_vrt=p_vrt;
    vrt_info[*r].q_vrt=q_vrt;

    memcpy(vrt_info[*r].p0, P, sizeof(double)*NBR_SPH );

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




int idx;
int idx_pre;
int idx_nex;

int neg_cnt=0;
int pos_cnt=0;


double n1;


double dp;



double    Vp[NBR_SPH];
double    Vn[NBR_SPH];
double    Vc[NBR_SPH];

for(idx=0; idx<np;idx++)
{
  idx_pre=(idx + np -1)% np;
  idx_nex=(idx +1)% np;

   nco_sph_sub(sP[idx_pre], sP[idx], Vp);
   nco_sph_sub(sP[idx_nex], sP[idx], Vn);

   n1=nco_sph_cross(Vn, Vp, Vc);


  dp= nco_sph_dot(Vc, sP[idx]);

  (dp>=0.0 ? pos_cnt++ : neg_cnt++);

  if(DEBUG_SPH)
    printf("%s(): idx=%d dp=%g\n", fnc_nm, idx, dp);



}

return neg_cnt==0;


}

/* make a control point that is Inside or Outside of polygon */
int nco_sph_mk_control(poly_sct *sP, nco_bool bInside,  double* pControl  )
{
   /* do stuff in radians */



   double clat=0.0;
   double clon=0.0;

   double lon_fct=2.0;
   double lat_fct=2.0;

   nco_bool bDeg=False;

   /* use Center as point inside polygon */
   if(bInside)
   {
     bDeg=True;
     nco_geo_lonlat_2_sph(sP->dp_x_ctr, sP->dp_y_ctr, pControl, False, bDeg);
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
  nco_geo_lonlat_2_sph(clon, clat, pControl, False, bDeg);

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

    code= nco_sph_seg_int_old(sP[idx1], sP[idx], pControl, pVertex, p, q);

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
      code = nco_sph_seg_int_old(sP->shp[jdx1], sP->shp[jdx], pControl, sQ->shp[idx], p, q);

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

  nco_bool bDeg=False;



  double pMidPoint[NBR_SPH];

  const char fnc_nm[]="nco_sph_centroid_mk()";

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




  double max_dp = -(KD_DBL_MAX);
  double length = 0.0;

  double pMidPoint[NBR_SPH]={0.0};


  const char fnc_nm[]="nco_sph_inside_mk()";

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
void nco_geo_lonlat_2_sph(double lon, double lat, double *b, nco_bool bSimple, nco_bool bDeg)
{

   //char fnc_nm[]="nco_geo_lonlat_2_sph";


   if(bDeg) {
      lon *= M_PI / 180.0;
      lat *= M_PI / 180.0;
   }

   /* really dont like this - this is used for RLL grids*/
   if(bSimple )
   {

     b[0] = cos(lat) * cos(lon);
     b[1] = cos(lat) * sin(lon);
     b[2] = sin(lat);

     b[3]=lon;
     b[4]=lat;


   }
   else
   {


     b[2] = sin(lat);

     if (fabs(b[2]) == 1.0) {
       b[0] = 0.0;
       b[1] = 0.0;
       b[3] = 0.0;
       b[4] = lat;


     }else if( b[2]==0.0 )
     {
       b[0]=cos(lon);
       b[1]=sin(lon);
       b[3]=lon;
       b[4]=0.0;

     } else
     {
       b[0] = cos(lat) * cos(lon);
       b[1] = cos(lat) * sin(lon);

       b[3]=lon;
       b[4]=lat;

     }
   }




}


void  nco_geo_sph_2_lonlat(double *a, double *lon, double *lat, nco_bool bDeg)
{

   /* nb this returns range (-180, 180) */
   *lon = atan2(a[1],a[0]) ;

   if( *lon < 0.0 &&  LON_MIN_RAD >=0.0  )
      *lon+= (LON_MAX_RAD);

   // b[1]= asin(a[2]) * 180.0 /M_PI;
   *lat=atan2( a[2], sqrt( a[0]*a[0]+a[1]*a[1] ) ) ;

   if(fabs(a[2])==1.0)
     *lon=0.0;

   /* convert to degrees if required */
   if(bDeg)
   {
      *lon*=(180.0 / M_PI );
      *lat*=(180.0 / M_PI );

   }

   return;
}


double nco_sph_area_quadrature(
double **sP, int np)
{

  int nTriangles = np - 2;
  int idx=0;
  int nOrder = 6;
  double dG[6];
  double dW[6];

  double dA;
  double dB;
  double dR;
  double dDenomTerm;
  double dJacobian;
  double dFaceArea = 0.0;

  const double *node1;
  const double *node2;
  const double *node3;

  double dF[NBR_SPH]={0.0};
  double dDaF[NBR_SPH]={0.0};
  double dDbF[NBR_SPH]={0.0};

  double dDaG[NBR_SPH]={0.0};
  double dDbG[NBR_SPH]={0.0};

  double nodeCross[NBR_SPH];

  if(np <3 )
     return dFaceArea;


  dG[0] = -0.9324695142031521;
  dG[1] = -0.6612093864662645;
  dG[2] = -0.2386191860831969;
  dG[3] = +0.2386191860831969;
  dG[4] = +0.6612093864662645;
  dG[5] = +0.9324695142031521;

  dW[0] = 0.1713244923791704;
  dW[1] = 0.3607615730481386;
  dW[2] = 0.4679139345726910;
  dW[3] = 0.4679139345726910;
  dW[4] = 0.3607615730481386;
  dW[5] = 0.1713244923791704;


  // Scale quadrature points
  for (idx = 0; idx < nOrder; idx++) {
    dG[idx] = 0.5 * (dG[idx] + 1.0);
    dW[idx] = 0.5 * dW[idx];
  }




  node1 = sP[0];

  // Loop over all sub-triangles of this Face
  for (idx = 0; idx < nTriangles; idx++) {


    // Calculate the area of the modified Face
    //Node node1 = nodes[face[0]];
    //Node node2 = nodes[face[j+1]];
    //Node node3 = nodes[face[j+2]];

    node2 = sP[idx + 1];
    node3 = sP[idx + 2];

    // Calculate area at quadrature node
    for (int p = 0; p < nOrder; p++) {
      for (int q = 0; q < nOrder; q++) {

        dA = dG[p];
        dB = dG[q];

        dF[0] = (1.0 - dB) * ((1.0 - dA) * node1[0] + dA * node2[0]) + dB * node3[0];
        dF[1] = (1.0 - dB) * ((1.0 - dA) * node1[1] + dA * node2[1]) + dB * node3[1];
        dF[2] = (1.0 - dB) * ((1.0 - dA) * node1[2] + dA * node2[2]) + dB * node3[2];

        /*
        Node dDaF(
        (1.0 - dB) * (dnode2[0] - dnode1[0]),
        (1.0 - dB) * (dnode2[1] - dnode1[1]),
        (1.0 - dB) * (dnode2[2] - dnode1[2]));
        */

        dDaF[0] = (1.0 - dB) * (node2[0] - node1[0]);
        dDaF[1] = (1.0 - dB) * (node2[1] - node1[1]);
        dDaF[2] = (1.0 - dB) * (node2[2] - node1[2]);

        /*
        Node dDbF(
        - (1.0 - dA) * dnode1[0] - dA * dnode2[0] + dnode3[0],
        - (1.0 - dA) * dnode1[1] - dA * dnode2[1] + dnode3[1],
        - (1.0 - dA) * dnode1[2] - dA * dnode2[2] + dnode3[2]);
        */

        dDbF[0] = -(1.0 - dA) * node1[0] - dA * node2[0] + node3[0];
        dDbF[1] = -(1.0 - dA) * node1[1] - dA * node2[1] + node3[1];
        dDbF[2] = -(1.0 - dA) * node1[2] - dA * node2[2] + node3[2];


        //dR = sqrt(dF[0] * dF[0] + dF[1] * dF[1] + dF[2] * dF[2]);
        dR=nco_sph_rad(dF);

        /*
        Node dDaG(
        dDaF[0] * (dF[1] * dF[1] + dF[2] * dF[2])- dF[0] * (dDaF[1] * dF[1] + dDaF[2] * dF[2]),
        dDaF[1] * (dF[0] * dF[0] + dF[2] * dF[2])- dF[1] * (dDaF[0] * dF[0] + dDaF[2] * dF[2]),
        dDaF[2] * (dF[0] * dF[0] + dF[1] * dF[1])- dF[2] * (dDaF[0] * dF[0] + dDaF[1] * dF[1]));
        */

        dDaG[0] = dDaF[0] * (dF[1] * dF[1] + dF[2] * dF[2]) - dF[0] * (dDaF[1] * dF[1] + dDaF[2] * dF[2]);
        dDaG[1] = dDaF[1] * (dF[0] * dF[0] + dF[2] * dF[2]) - dF[1] * (dDaF[0] * dF[0] + dDaF[2] * dF[2]);
        dDaG[2] = dDaF[2] * (dF[0] * dF[0] + dF[1] * dF[1]) - dF[2] * (dDaF[0] * dF[0] + dDaF[1] * dF[1]);



        /*
        Node dDbG(
        dDbF[0] * (dF[1] * dF[1] + dF[2] * dF[2])- dF[0] * (dDbF[1] * dF[1] + dDbF[2] * dF[2]),
        dDbF[1] * (dF[0] * dF[0] + dF[2] * dF[2])- dF[1] * (dDbF[0] * dF[0] + dDbF[2] * dF[2]),
        dDbF[2] * (dF[0] * dF[0] + dF[1] * dF[1])- dF[2] * (dDbF[0] * dF[0] + dDbF[1] * dF[1]));
        */

        dDbG[0] = dDbF[0] * (dF[1] * dF[1] + dF[2] * dF[2]) - dF[0] * (dDbF[1] * dF[1] + dDbF[2] * dF[2]);
        dDbG[1] = dDbF[1] * (dF[0] * dF[0] + dF[2] * dF[2]) - dF[1] * (dDbF[0] * dF[0] + dDbF[2] * dF[2]);
        dDbG[2] = dDbF[2] * (dF[0] * dF[0] + dF[1] * dF[1]) - dF[2] * (dDbF[0] * dF[0] + dDbF[1] * dF[1]);


        dDenomTerm = 1.0 / (dR * dR * dR);

        dDaG[0] *= dDenomTerm;
        dDaG[1] *= dDenomTerm;
        dDaG[2] *= dDenomTerm;

        dDbG[0] *= dDenomTerm;
        dDbG[1] *= dDenomTerm;
        dDbG[2] *= dDenomTerm;
        /*
                Node node;
                Node dDx1G;
                Node dDx2G;

                ApplyLocalMap(
                    faceQuad,
                    nodes,
                    dG[p],
                    dG[q],
                    node,
                    dDx1G,
                    dDx2G);
        */
        // Cross product gives local Jacobian
        //Node nodeCross = CrossProduct(dDaG, dDbG);

        /* plain cross product - no normalization */
        nco_sph_cross2(dDaG, dDbG, nodeCross);

        //dJacobian = sqrt(nodeCross[0] * nodeCross[0] + nodeCross[1] * nodeCross[1] + nodeCross[2] * nodeCross[2]);
        dJacobian=nco_sph_rad(nodeCross);

        //dFaceArea += 2.0 * dW[p] * dW[q] * (1.0 - dG[q]) * dJacobian;

        dFaceArea += dW[p] * dW[q] * dJacobian;
      }
    }
  }

  return dFaceArea;
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

  int pqCross[4]={0,0,0,0};

  nco_bool isParallel=False;
  nco_bool isP_LatCircle=False;
  nco_bool isQ_LatCircle=False;


  double nx1;
  double nx2;



  char code[]="00";

  double Pcross[NBR_SPH];
  double Qcross[NBR_SPH];

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


    if (isQ_LatCircle) {

      pqCross[0] = nco_rll_lhs_lat(P->shp[a1], Q->shp[b1], Q->shp[b]);
      pqCross[1] = nco_rll_lhs_lat(P->shp[a], Q->shp[b1], Q->shp[b]);
    }else{
      pqCross[0] = nco_rll_lhs(P->shp[a1], Qcross);
      pqCross[1] = nco_rll_lhs(P->shp[a],  Qcross);

    }



    if (isP_LatCircle){
      pqCross[2] = nco_rll_lhs_lat(Q->shp[b1], P->shp[a1], P->shp[a]);
      pqCross[3] = nco_rll_lhs_lat(Q->shp[b], P->shp[a1], P->shp[a]);
    }else{
      pqCross[2] = nco_rll_lhs(Q->shp[b1], Pcross);
      pqCross[3] = nco_rll_lhs(Q->shp[b],  Pcross);
    }



    /* imply facing rules  */
    if(!pqCross[0])
      pqCross[0]=-pqCross[1];
    else if(!pqCross[1])
      pqCross[1]=-pqCross[0];

    if(!pqCross[2])
      pqCross[2]=-pqCross[3];
    else if(!pqCross[3])
      pqCross[3]=-pqCross[2];


    /* now calculate face rules */
    qpFace = nco_sph_face(pqCross[0], pqCross[1], pqCross[3]);
    pqFace = nco_sph_face(pqCross[2], pqCross[3], pqCross[1]);



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

      pqCross[0]=0;
      pqCross[1]=0;
      pqCross[2]=0;
      pqCross[3]=0;

      qpFace = 0;
      pqFace = 0;

    }


    if( isGeared == False)
    {
      //if(  (ipqLHS == 1 && iqpLHS == 1) ||  ( qpFace && pqFace )     )
      if(  ( pqCross[1]==1   && pqCross[3] == 1) ||  ( qpFace && pqFace )     )
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

         nco_rll_seg_parallel(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q, &lcl_inflag, code);

        if(code[0] == 'X'){
          if(nco_dbg_lvl_get() >= nco_dbg_dev) (void)fprintf(stderr,"%s: ERROR %s() reports %s\n",nco_prg_nm_get(),fnc_nm,"parallel edges in opposite directions\n");
          return EXIT_FAILURE;
        }

        if (lcl_inflag != poly_vrl_unk ) {

          inflag = lcl_inflag;

          /* there is a subtle  trick here - a point is "force added" by setting the flags pqFace and qpFace */
          if (code[0] == '2')
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
        code[0] = nco_rll_seg_int(P->shp[a1], P->shp[a], Q->shp[b1], Q->shp[b], p, q);


        if (code[0] == '1' || code[0] == 'e') {

          nco_sph_add_pnt(R->shp, r, p);

          if (numIntersect++ == 0) {
            /* reset counters */
            aa = 0;
            bb = 0;
          }

          inflag = (pqCross[1] == 1 ? poly_vrl_pin : pqCross[3] == 1 ? poly_vrl_qin : inflag);


          if (DEBUG_SPH)
            printf("%%InOut sets inflag=%s\n", nco_poly_vrl_flg_sng_get(inflag));

        }
      }

      if(DEBUG_SPH)
        printf("numIntersect=%d codes=%s (ipqLHS=%d, ip1qLHS=%d), (iqpLHS=%d, iq1pLHS=%d), (qpFace=%d pqFace=%d) inflag=%s\n",numIntersect, code, pqCross[1], pqCross[0],  pqCross[3], pqCross[2], qpFace,pqFace, nco_poly_vrl_flg_sng_get(inflag));

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

      } else if ( pqCross[3]  == -1) {
        /* advance q */
        //if(inflag== Qin) sAddPoint(R,r,Q->shp[b]);
        bb++;b++;

        /* cross product zero  */
      } else if(  !pqCross[0] && !pqCross[1] && !pqCross[2] && !pqCross[3]     ){
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
nco_rll_seg_int(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1) {

  char code = '0';
  nco_bool bDeg = False;
  nco_bool isP_LatCircle = False;
  nco_bool isQ_LatCircle = False;

  isP_LatCircle = nco_rll_is_lat_circle(p0, p1);
  isQ_LatCircle = nco_rll_is_lat_circle(q0, q1);


  /* longitude P may hit small ciricle Q */
  if (!isP_LatCircle && isQ_LatCircle) {
    /* Check longitude range */
    if (nco_sph_between(q0[3], q1[3], p0[3]) && nco_sph_between(p0[4], p1[4], q0[4])) {
      r0[3] = p0[3];
      r0[4] = q0[4];
      code = '1';
    }

  }
    /* longitude Q may hit small circle P */
  else if (isP_LatCircle && !isQ_LatCircle) {
    /* Check range range */
    if (nco_sph_between(p0[3], p1[3], q0[3]) && nco_sph_between(q0[4], q1[4], p0[4])) {
      r0[3] = q0[3];
      r0[4] = p0[4];
      code = '1';
    }

  }

  if (code == '1') {
    nco_geo_lonlat_2_sph(r0[3], r0[4], r0, True, bDeg);



  }
  return code;


}

nco_bool
nco_rll_seg_parallel(double *p0, double *p1, double *q0, double *q1, double *r0, double *r1, poly_vrl_flg_enm *inflag, char*codes ) {

  char fnc_nm[]="nco_rll_seg_parallel()";

  int id;

  //char code = '0';

  codes[0]='0';

  nco_bool isP_LatCircle = False;
  nco_bool isQ_LatCircle = False;


  isP_LatCircle = nco_rll_is_lat_circle(p0, p1);
  isQ_LatCircle = nco_rll_is_lat_circle(q0, q1);

  /* check for brain dead call */
  if (isP_LatCircle != isQ_LatCircle)
    return False;


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
    codes[0] = '2';
  } else if ( nco_sph_between(p0[id], p1[id], q0[id]) && !nco_sph_between(p0[id], p1[id], q1[id]) ) {
    nco_sph_adi(r0, q0);
    nco_sph_adi(r1, p1);
    *inflag=poly_vrl_pin;
    codes[0] = '2';

  } else if ( nco_sph_between(p0[id], p1[id], q0[id]) && nco_sph_between(p0[id], p1[id], q1[id])) {
    nco_sph_adi(r0, q0);
    nco_sph_adi(r0, q1);
    *inflag=poly_vrl_qin;
    codes[0] = '2';
  } else if (nco_sph_between(q0[id], q1[id], p0[id]) && nco_sph_between(q0[id], q1[id], p1[id])) {
    nco_sph_adi(r0, p0);
    nco_sph_adi(r1, p1);
    *inflag=poly_vrl_pin;
    codes[0] = '2';

  } else
    codes[0] = '0';

  if(DEBUG_SPH)
    printf("%s: isP_LatCircle=%d isQ_LatCircle=%d codes=%s\n",  fnc_nm, isP_LatCircle, isQ_LatCircle,  codes );

  if(codes[0]!='0')
    return True;

  return False;



}


void nco_rll_add_pnt(double **R, int *r, double *P)
{


  if(DEBUG_SPH)
    nco_sph_prn_pnt("nco_rll_add_pnt()", P, 3, True);


  memcpy(R[*r], P, sizeof(double)*NBR_SPH);
  (*r)++;




}

/***************************************************************************************/
/*********************   functions that deal with 3x3 matrix ******************************************************/
/* mat mult */
void
nco_mat_mlt
(double mat[], double vec[], double vec_out[])
{

  vec_out[0] = mat[0]*vec[0] + mat[1]*vec[1] + mat[2]*vec[2];
  vec_out[1]=  mat[3]*vec[0] + mat[4]*vec[1] + mat[5]*vec[2];
  vec_out[2]=  mat[6]*vec[0] + mat[7]*vec[1] + mat[8]*vec[2];

}

/* returns true if matrix is inverted, false otherwise */
nco_bool
nco_mat_inv(double *mat, double *mat_inv)
{
  char fnc_nm[]="nco_mat_inv()";
  double deti;
  double det;

  det =  mat[0] * (mat[4]*mat[8] - mat[5]*mat[7])
        -mat[1] * (mat[3]*mat[8] - mat[5]*mat[6])
        +mat[2] * (mat[3]*mat[7] - mat[4]*mat[6]);


  if(  isnan(det) || !isfinite(det) || det==0.0 )
  {
    if(0 && nco_dbg_lvl_get()>= nco_dbg_dev)
      (void)fprintf(stderr,"%s: det=%.15f\n", fnc_nm, det);
    return False;

  }


  deti = 1.0/det;

  mat_inv[0] = (mat[4]*mat[8] - mat[5]*mat[7]) * deti;
  mat_inv[1] = (mat[2]*mat[7] - mat[1]*mat[8]) * deti;
  mat_inv[2] = (mat[1]*mat[5] - mat[2]*mat[4]) * deti;

  mat_inv[3] = (mat[5]*mat[6] - mat[3]*mat[8]) * deti;
  mat_inv[4] = (mat[0]*mat[8] - mat[2]*mat[6]) * deti;
  mat_inv[5] = (mat[2]*mat[3] - mat[0]*mat[5]) * deti;

  mat_inv[6] = (mat[3]*mat[7] - mat[4]*mat[6]) * deti;
  mat_inv[7] = (mat[1]*mat[6] - mat[0]*mat[7]) * deti;
  mat_inv[8] = (mat[0]*mat[4] - mat[1]*mat[3]) * deti;

  return True;
}


#ifdef ENABLE_GSL


nco_bool
nco_mat_int_pl
(const double *p0, const double *p1, const double *q0, const double *q1, double *r0)
{
  int s;
/*
 double mat[9];
double mat_inv[9];
double V[3];
double X[3];
*/

  int size=3;
  nco_bool bRet=False;


  gsl_matrix *gsl_mat = gsl_matrix_alloc(size, size);
  gsl_permutation *p = gsl_permutation_alloc(size);

  gsl_vector *gsl_vec_b = gsl_vector_alloc(size);
  gsl_vector *gsl_vec_x = gsl_vector_alloc(size);


  /*
  gsl_matrix_long_double *gsl_mat = gsl_matrix_long_double_alloc(size, size);
  gsl_permutation *p = gsl_permutation_alloc(size);

  gsl_vector_long_double *gsl_vec_b = gsl_vector_long_double_alloc(size);
  gsl_vector_long_double *gsl_vec_x = gsl_vector_long_double_alloc(size);
  */




/* set a plane and line into matrix
mat[0]=q0[0]-q1[0];
mat[1]=p1[0]-p0[0];
mat[2]=-p0[0];

mat[3]=q0[1]-q1[1];
mat[4]=p1[1]-p0[1];
mat[5]=-p0[1];

mat[6]=q0[2]-q1[2];
mat[7]=p1[2]-p0[2];
mat[8]=-p0[2];
*/
  /* set a plane and line into matrix */
  gsl_matrix_set( gsl_mat, 0, 0,    q0[0]-q1[0] );
  gsl_matrix_set( gsl_mat, 0, 1,  p1[0]-p0[0] );
  gsl_matrix_set( gsl_mat, 0, 2,  -p0[0]);
  gsl_matrix_set( gsl_mat, 1, 0,  q0[1]-q1[1] );
  gsl_matrix_set( gsl_mat, 1, 1,  p1[1]-p0[1]);
  gsl_matrix_set( gsl_mat, 1, 2,  -p0[1] );
  gsl_matrix_set( gsl_mat, 2, 0,  q0[2]-q1[2]);
  gsl_matrix_set( gsl_mat, 2, 1,  p1[2]-p0[2]);
  gsl_matrix_set( gsl_mat, 2, 2,  -p0[2] );


  gsl_vector_set(gsl_vec_b, 0,q0[0]-p0[0]);
  gsl_vector_set(gsl_vec_b, 1,q0[1]-p0[1]);
  gsl_vector_set(gsl_vec_b, 2,q0[2]-p0[2]);


  /* remember LU decomposition is put into gsl_mat */
  if(!gsl_linalg_LU_decomp(gsl_mat, p, &s) && !gsl_linalg_LU_solve(gsl_mat,p,gsl_vec_b, gsl_vec_x)) {
    bRet = True;

    r0[0]=gsl_vector_get(gsl_vec_x,0);
    r0[1]=gsl_vector_get(gsl_vec_x,1);
    r0[2]=gsl_vector_get(gsl_vec_x,2);

  }

  gsl_permutation_free(p);
  gsl_matrix_free(gsl_mat);
  gsl_vector_free(gsl_vec_b);
  gsl_vector_free(gsl_vec_x);


  return bRet;




}


#else


/* Intersect a line and a plane */
nco_bool
nco_mat_int_pl(const double *p0, const double *p1, const double *q0, const double *q1, double *r0)
               {

  double mat[9];
  double mat_inv[9];
  double V[3];
  double X[3];



  /* set a plane and line into matrix */
  mat[0]=q0[0]-q1[0];
  mat[1]=p1[0]-p0[0];
  mat[2]=-p0[0];

  mat[3]=q0[1]-q1[1];
  mat[4]=p1[1]-p0[1];
  mat[5]=-p0[1];

  mat[6]=q0[2]-q1[2];
  mat[7]=p1[2]-p0[2];
  mat[8]=-p0[2];



  /* find inverse */
  if (nco_mat_inv(mat, mat_inv)==False)
    return False;

  /* set parameters */
  V[0]=q0[0]-p0[0];
  V[1]=q0[1]-p0[1];
  V[2]=q0[2]-p0[2];

  /* solve matrix */
  nco_mat_mlt(mat_inv, V, X);


  r0[0]=X[0];
  r0[1]=X[1];
  r0[2]=X[2];


  if( isnan(X[0]) || !isfinite(X[0]) || isnan(X[1]) || !isfinite(X[1]) || isnan(X[2]) || !isfinite(X[2]) )
    return False;



  return True;
}

#endif


/******************************************************************************************************************/
