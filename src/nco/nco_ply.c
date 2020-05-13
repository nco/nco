/* $Header$ */

/* Purpose: Polygon structure & utilities */

/* Copyright (C) 2019--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_ply.h" /* Polygon structure & utilities */

int
nco_poly_typ_sz(poly_typ_enm pl_typ)
{
  if(pl_typ == poly_crt)
    return NBR_CRT;
  else if(pl_typ == poly_sph)
    return NBR_SPH;
  else if(pl_typ == poly_rll)
    return NBR_RLL;

  return 0;
}

poly_sct *
nco_poly_free
(poly_sct *pl)
{

  /* mem flag set -so pointers from external array */
  if( pl->mem_flg ==1 )
  {
    pl->dp_x=(double*)NULL_CEWI;
    pl->dp_y=(double*)NULL_CEWI;

  }
  else
  {  
    pl->dp_x=(double*)nco_free(pl->dp_x);
    pl->dp_y=(double*)nco_free(pl->dp_y);
  }

  if(pl->dp_xyz)
    pl->dp_xyz=(double*)nco_free(pl->dp_xyz);

  if(pl->shp)
    nco_poly_shp_free(pl);

  pl=(poly_sct*)nco_free(pl);

   return pl;

}  





poly_sct *   
nco_poly_init
(void)
{


  poly_sct *pl;

  pl=(poly_sct*)nco_malloc( sizeof(poly_sct));

  pl->pl_typ=poly_none;

  pl->dp_x=(double*)NULL_CEWI;
  pl->dp_y=(double*)NULL_CEWI;
  pl->dp_xyz=(double*)NULL_CEWI;
  pl->shp=(double**)NULL_CEWI;


  pl->dp_x_minmax[0]=0.0;
  pl->dp_x_minmax[1]=0.0;

  pl->dp_y_minmax[0]=0.0;
  pl->dp_y_minmax[1]=0.0;

  pl->dp_x_ctr=KD_DBL_MAX;
  pl->dp_y_ctr=KD_DBL_MAX;

  pl->wgt=0.0;
  pl->stat=0;
  pl->area=0.0;
  pl->crn_nbr=0;
  pl->mem_flg=0;

  pl->src_id=-1;
  pl->dst_id=-1;

  pl->bwrp=False;
  pl->bwrp_y=False;

  /* defaults to true */
  pl->bmsk=True;


  return pl;
}




poly_sct*
nco_poly_dpl
(poly_sct *pl)
{

  int idx;
  int crn_nbr_in;
  poly_sct *pl_cpy;

  
  pl_cpy=nco_poly_init();

  pl_cpy->pl_typ=pl->pl_typ;

  crn_nbr_in=pl->crn_nbr;

  pl_cpy->src_id=pl->src_id;
  pl_cpy->dst_id=pl->dst_id;


  pl_cpy->stat=pl->stat;
  pl_cpy->area=pl->area;
  pl_cpy->crn_nbr=crn_nbr_in;

  /* mem flag is ALWAYS 0 for a copy  */
  pl_cpy->mem_flg=0;

  pl_cpy->bwrp=pl->bwrp;
  pl_cpy->bwrp_y=pl->bwrp_y;
  pl_cpy->bmsk=pl->bmsk;

  pl_cpy->dp_x=(double*)nco_malloc((size_t)crn_nbr_in* sizeof(double));
  pl_cpy->dp_y=(double*)nco_malloc((size_t)crn_nbr_in* sizeof(double));

  memcpy(pl_cpy->dp_x, pl->dp_x, (size_t)crn_nbr_in* sizeof(double));
  memcpy(pl_cpy->dp_y, pl->dp_y, (size_t)crn_nbr_in* sizeof(double));  

  /*
  pl->dp_x_minmax[0];
  pl->dp_x_minmax[1]=0.0;

  pl->dp_y_minmax[0]=0.0;
  pl->dp_y_minmax[1]=0.0;
  */

  pl_cpy->dp_x_minmax[0] = pl->dp_x_minmax[0];
  pl_cpy->dp_x_minmax[1] = pl->dp_x_minmax[1];

  pl_cpy->dp_y_minmax[0] = pl->dp_y_minmax[0];
  pl_cpy->dp_y_minmax[1] = pl->dp_y_minmax[1];

  pl_cpy->dp_x_ctr=pl->dp_x_ctr;
  pl_cpy->dp_y_ctr=pl->dp_y_ctr;
  pl_cpy->wgt=pl->wgt;

  /* do a deep copy of this list */
  if(pl->shp)
  {
    int typ_sz=nco_poly_typ_sz(pl->pl_typ);

    nco_poly_shp_init(pl_cpy);

    //memcpy( pl_cpy->shp,  pl->shp,  crn_nbr_in *sizeof(double) * nco_poly_typ_sz(pl->pl_typ) );
    for(idx=0 ; idx< crn_nbr_in;idx++)
      memcpy(pl_cpy->shp[idx], pl->shp[idx], sizeof(double) * typ_sz  );


  }
  


  
  return pl_cpy;
  
} 

size_t
nco_poly_sizeof(poly_sct *pl)
{

  size_t ttl_sz=0;
  size_t arr_sz;

  if(!pl)
    return ttl_sz;


  ttl_sz=sizeof(poly_sct);



  if(pl->crn_nbr==0)
    return ttl_sz;

  arr_sz=sizeof(double)*pl->crn_nbr;

  if(pl->dp_x)ttl_sz+=arr_sz;

  if(pl->dp_y)ttl_sz+=arr_sz;

  if(pl->shp)ttl_sz+=arr_sz*3;

  if(pl->dp_xyz)ttl_sz+=arr_sz;

  return ttl_sz;


}

poly_sct *
nco_poly_init_crn
(poly_typ_enm pl_typ,
int crn_nbr_in,
int src_id)
{
  poly_sct *pl;

  pl=nco_poly_init();

  pl->pl_typ=pl_typ;
  pl->crn_nbr=crn_nbr_in;
  pl->src_id=src_id;
  pl->dst_id=-1;

  pl->dp_x=(double*)nco_calloc((size_t)crn_nbr_in, sizeof(double));
  pl->dp_y=(double*)nco_calloc((size_t)crn_nbr_in, sizeof(double));

  pl->mem_flg=0;
  pl->bwrp=False;
  pl->bwrp_y =False;
  pl->bmsk=True;

  return pl;
}



void
nco_poly_init_crn_re  /* re malloc crn_nbr to a new size */
(poly_sct *pl,
 int new_crn_nbr)
{
  int idx;


  /* if same size then do nothing */
  if( new_crn_nbr ==  pl->crn_nbr )
    return;

  pl->dp_x=(double*)nco_realloc(pl->dp_x, sizeof(double) * new_crn_nbr);
  pl->dp_y=(double*)nco_realloc(pl->dp_y, sizeof(double) * new_crn_nbr);

  if(new_crn_nbr> pl->crn_nbr)
    for(idx=pl->crn_nbr;idx<new_crn_nbr;idx++ )
    {
      pl->dp_x[idx]=0.0;
      pl->dp_y[idx]=0.0;
    }



  if(pl->shp)
  {
    size_t nbr_el;

    nbr_el=nco_poly_typ_sz(pl->pl_typ);

    if(new_crn_nbr < pl->crn_nbr)
       for(idx=new_crn_nbr; idx<pl->crn_nbr; idx++ )
           pl->shp[idx]=(double*)nco_free(pl->shp[idx]);

    pl->shp=(double**)nco_realloc(pl->shp, sizeof(double*)*new_crn_nbr);

    if(new_crn_nbr > pl->crn_nbr)
        for(idx=pl->crn_nbr; idx<new_crn_nbr;idx++ )
          pl->shp[idx]=(double*)nco_calloc( sizeof(double), nbr_el);

  }


  pl->crn_nbr=new_crn_nbr;

  return;

}






poly_sct *
nco_poly_init_lst
(poly_typ_enm pl_typ,
 int arr_nbr,
 int mem_flg,
 int src_id,
 double *dp_x_in,
 double *dp_y_in)
{


 int idx;
 int lcl_crn_nbr=0;

 double *lcl_x_in=NULL_CEWI;
 double *lcl_y_in=NULL_CEWI;

 poly_sct *pl;



 /* less than a triangle */
 if (arr_nbr <3 )
   return (poly_sct*)NULL_CEWI;

 lcl_x_in=(double*)nco_malloc(sizeof(double)*arr_nbr);
 lcl_y_in=(double*)nco_malloc(sizeof(double)*arr_nbr);



 /* ignore repeated vertex in X and Y  */
 for(idx=0; idx<arr_nbr; idx++ )
 {
   /* ignore repeated vertex */
   if (idx > 0 && dp_x_in[idx] == dp_x_in[idx - 1] && dp_y_in[idx] == dp_y_in[idx - 1])
     continue;

   lcl_x_in[lcl_crn_nbr] = dp_x_in[idx];
   lcl_y_in[lcl_crn_nbr] = dp_y_in[idx];
   lcl_crn_nbr++;
 }




 if( lcl_crn_nbr < 3 ) {

   lcl_x_in=(double*)nco_free(lcl_x_in);
   lcl_y_in=(double*)nco_free(lcl_y_in);
   return (poly_sct *) NULL_CEWI;

 }
 /* we have at least a triangle */ 

 
 /* dont free  pointers */
 if( mem_flg)
 {

   pl=nco_poly_init();
   pl->pl_typ=pl_typ;

   pl->mem_flg=1;
   pl->crn_nbr=lcl_crn_nbr;
 
   pl->dp_x=dp_x_in;
   pl->dp_y=dp_y_in;

   pl->src_id=src_id;
 
 }
 else
 {
   pl=nco_poly_init_crn(pl_typ, lcl_crn_nbr, src_id);
   memcpy(pl->dp_x, lcl_x_in, sizeof(double) * lcl_crn_nbr);
   memcpy(pl->dp_y, lcl_y_in, sizeof(double) * lcl_crn_nbr);
   
 }

 lcl_x_in=(double*)nco_free(lcl_x_in);
 lcl_y_in=(double*)nco_free(lcl_y_in);

  return pl;
 

}



void nco_poly_minmax_add
(poly_sct *pl,
 nco_grd_lon_typ_enm grd_lon_typ,
 nco_bool bchk_caps)
{

  int idx;
  int sz;


  sz=pl->crn_nbr;

  pl->dp_x_minmax[0]=KD_DBL_MAX;
  pl->dp_x_minmax[1]=-KD_DBL_MAX;

  pl->dp_y_minmax[0]=KD_DBL_MAX;
  pl->dp_y_minmax[1]=-KD_DBL_MAX;



  for(idx=0; idx<sz;idx++)
  {
    /* min */
    if( pl->dp_x[idx] < pl->dp_x_minmax[0] )
      pl->dp_x_minmax[0] = pl->dp_x[idx];

    /* max */
    if( pl->dp_x[idx] > pl->dp_x_minmax[1] )
      pl->dp_x_minmax[1] = pl->dp_x[idx];

    /* min */
    if( pl->dp_y[idx] < pl->dp_y_minmax[0] )
      pl->dp_y_minmax[0] = pl->dp_y[idx];

    /* max */
    if( pl->dp_y[idx] > pl->dp_y_minmax[1] )
      pl->dp_y_minmax[1] = pl->dp_y[idx];



  }

  /* nothing else to do */
  if(pl->pl_typ == poly_crt )
    return;


  /* add correction to latitude bounding box */
  if(pl->pl_typ == poly_sph || pl->pl_typ == poly_rll ){

    double lat_min;
    double lat_max;
    nco_bool bDeg = 1;
    nco_bool is_caps=False;

    /* add wrap flag */
    pl->bwrp=(pl->dp_x_minmax[1] - pl->dp_x_minmax[0] >= 180.0);

    /* rll grid no polar cap */
    if( pl->pl_typ == poly_rll )
      bchk_caps=False;

    /* check for a polar cap */
    if (pl->bwrp &&  bchk_caps)
    {

      /* polar cap must all be northern or southern hemisphere */
      if(  ! ( ( pl->dp_y_minmax[0] > 0.0 && pl->dp_y_minmax[1] > 0.0 )  ||
                (pl->dp_y_minmax[0] < 0.0 && pl->dp_y_minmax[1] < 0.0 ) ) )
        is_caps=False;
      /* see if its a polar triangle */
      else if(pl->dp_y_minmax[0] == -90.0 || pl->dp_y_minmax[1] == 90.0  )
         is_caps=False;
        /* this  works for some corner cases  */
      else if (pl->dp_y_minmax[0] == pl->dp_y_minmax[1])
        is_caps = True;
      else
      {

        nco_bool bDeg=True;


        double pControl[NBR_SPH];
        double pPole[NBR_SPH];

        /* for a polar must all be in nothern or souther hemisphere */
        if( pl->dp_y_minmax[1] >0.0 ) {
          pControl[0]=20.0;
          pControl[1]=pl->dp_y_minmax[0]-2.0;
          pPole[0] = 0.0;
          pPole[1] = 90.0;
        }else{
          pControl[0]=20.0;
          pControl[1]=pl->dp_y_minmax[1]+2.0;
          pPole[0] = 0.0;
          pPole[1] = -90.0;
        }


        nco_geo_lonlat_2_sph(pPole[0], pPole[1], pPole, False, bDeg);
        nco_geo_lonlat_2_sph(pControl[0], pControl[1], pControl, False, bDeg);

        /* see if pole is inside polygon */
        is_caps=( nco_sph_pnt_in_poly(pl->shp, pl->crn_nbr,pControl, pPole ) %2);



        }
    }


    /* deal with a polar cap here */
    if ( pl->bwrp && is_caps) {

        if (pl->dp_y_minmax[0] > 0.0)
          pl->dp_y_minmax[1] = 90.0;
        else
          pl->dp_y_minmax[0] = -90.0;


        /* change longitude limits so they include the whole range */
        switch (grd_lon_typ) {

          default:
          case nco_grd_lon_Grn_ctr:
          case nco_grd_lon_Grn_wst:

            pl->dp_x_minmax[0] = 0.0;
            pl->dp_x_minmax[1] = 360.0;
            break;

          case nco_grd_lon_180_ctr:
          case nco_grd_lon_180_wst:
            pl->dp_x_minmax[0] = -180.0;
            pl->dp_x_minmax[1] = 180.0;

        }
        pl->bwrp_y = True;


    }else{

      pl->bwrp_y=False;
    }


    if(pl->bwrp && is_caps == False)
    {

        double *lcl_dp_x = NULL_CEWI;

        lcl_dp_x = (double *) nco_malloc(sizeof(double) * sz);

        /* make local copy of longitudes */
        memcpy(lcl_dp_x, pl->dp_x, sizeof(double) * sz);

        /* convert from  (-180,180) to (0,360) or (0, 360)-> (-180, 180) */
        switch (grd_lon_typ) {

          default:
          case nco_grd_lon_Grn_wst:
          case nco_grd_lon_Grn_ctr:
            nco_msh_lon_crr(lcl_dp_x, sz, 1, grd_lon_typ, nco_grd_lon_180_ctr);

            pl->dp_x_minmax[0] = KD_DBL_MAX;
            pl->dp_x_minmax[1] = -KD_DBL_MAX;

            /* find new min-max */
            for (idx = 0; idx < sz; idx++) {
              if (lcl_dp_x[idx] < pl->dp_x_minmax[0])
                pl->dp_x_minmax[0] = lcl_dp_x[idx];

              if (lcl_dp_x[idx] > pl->dp_x_minmax[1])
                pl->dp_x_minmax[1] = lcl_dp_x[idx];
            }

            /* convert back to original lon typ */
            nco_msh_lon_crr(pl->dp_x_minmax, 2, 1, nco_grd_lon_180_ctr, grd_lon_typ);

            break;


          case nco_grd_lon_180_wst:
          case nco_grd_lon_180_ctr:
            nco_msh_lon_crr(lcl_dp_x, sz, 1, grd_lon_typ, nco_grd_lon_Grn_ctr);
            pl->dp_x_minmax[0] = KD_DBL_MAX;
            pl->dp_x_minmax[1] = -KD_DBL_MAX;

            /* find new min-max */
            for (idx = 0; idx < sz; idx++) {
              if (lcl_dp_x[idx] < pl->dp_x_minmax[0])
                pl->dp_x_minmax[0] = lcl_dp_x[idx];

              if (lcl_dp_x[idx] > pl->dp_x_minmax[1])
                pl->dp_x_minmax[1] = lcl_dp_x[idx];
            }

            nco_msh_lon_crr(pl->dp_x_minmax, 2, 1, nco_grd_lon_Grn_ctr, grd_lon_typ);

            break;

        }

        /* max == 360.0 has been converted to 0.0 we want the 360 value for the limits *
         * as the kd tree manages wrapping by splitting the limits into 2 *
         * dont wish to edit the actual vertices */
        if(grd_lon_typ == nco_grd_lon_Grn_wst || grd_lon_typ ==  nco_grd_lon_Grn_ctr)
        {
          if( pl->dp_x_minmax[0] == 0.0 &&  pl->dp_x_minmax[1] >180.0   )
            pl->dp_x_minmax[0]=360.0;

          else if( pl->dp_x_minmax[0] >180.0  &&  pl->dp_x_minmax[1] == 0.0 )
            pl->dp_x_minmax[1]=360.0;

        }



        /* swap values if necessasary */
        if (pl->dp_x_minmax[0] > pl->dp_x_minmax[1]) {
          double stmp;
          stmp = pl->dp_x_minmax[0];
          pl->dp_x_minmax[0] = pl->dp_x_minmax[1];

          pl->dp_x_minmax[1] = stmp;

        }


       /* double check wrapping again */
       if(pl->dp_x_minmax[1] - pl->dp_x_minmax[0] < 180.0)
         pl->bwrp=False;


        lcl_dp_x = (double *) nco_free(lcl_dp_x);

      }

      /* nb no need to add correction for Rll grid */
      if( pl->pl_typ == poly_sph &&  !pl->bwrp_y)
      {
        nco_geo_get_lat_correct(pl->dp_x_minmax[0], pl->dp_y_minmax[1], pl->dp_x_minmax[1], pl->dp_y_minmax[0],
                                &lat_min,
                                &lat_max, bDeg);

        pl->dp_y_minmax[0] = lat_min;
        pl->dp_y_minmax[1] = lat_max;

      }

  } /* end nco_sph */

  return;


}


/* Uses min-max limits only no serious polygon stuff */
/* should really only be used after nco_sph_intersect */
/* remember that for polY_sph - dp_y_minmax[0,1] a latitude correction has been added */

nco_bool                  /* [flg] return True if pl_out inside pl_in */
nco_poly_in_poly_minmax(
poly_sct *pl_in,
poly_sct *pl_out)
{
  /* y/lon first */
  if( !(pl_out->dp_y_minmax[0] >= pl_in->dp_y_minmax[0] &&
        pl_out->dp_y_minmax[1] <= pl_in->dp_y_minmax[1])
    )
    return False;

  if(pl_in->bwrp_y )
    return True;

  if(!pl_in->bwrp_y && pl_out->bwrp_y )
    return False;

  /* no wrapping */
  if(pl_in->bwrp == False && pl_out->bwrp == False)
    return  (pl_out->dp_x_minmax[0] >= pl_in->dp_x_minmax[0] && pl_out->dp_x_minmax[1] <= pl_in->dp_x_minmax[1]);

  /* either all of pl_out is to left of GM or all of polygon is to the right of GM */
  if(pl_in->bwrp == True && pl_out->bwrp == False)
    return (  ( pl_out->dp_x_minmax[0] >= pl_in->dp_x_minmax[1] && pl_out->dp_x_minmax[1] >= pl_in->dp_x_minmax[1] ) ||
              ( pl_out->dp_x_minmax[0] <= pl_in->dp_x_minmax[0] && pl_out->dp_x_minmax[1] <= pl_in->dp_x_minmax[0] ) );

  /* obviously a wrapped polygon cannot be within the limits of an un-wrapped plolygon */
  if(pl_in->bwrp == False && pl_out->bwrp== True)
    return False;


  if(pl_in->bwrp == True && pl_out->bwrp == True )
    return (  pl_out->dp_x_minmax[1] >= pl_in->dp_x_minmax[1] && pl_out->dp_x_minmax[0] <= pl_in->dp_x_minmax[0] );

  return False;

}


void
nco_poly_prn
(poly_sct *pl, int style)
{
  int idx;

  /*
  int nbr_el;
  nbr_el=nco_poly_typ_sz(pl->pl_typ);
  */

  switch(style){ 

    case 0:
      (void)fprintf(stderr,"\n# %s: pl_typ=%d, crn_nbr=%d bwrp=%d bwrp_y=%d mem_flg=%d area=%.20e src_id=%d dst_id=%d x_ctr=%f y_ctr=%f\n", nco_prg_nm_get(),pl->pl_typ, pl->crn_nbr, pl->bwrp, pl->bwrp_y, pl->mem_flg, pl->area, pl->src_id, pl->dst_id, pl->dp_x_ctr, pl->dp_y_ctr);
      (void)fprintf(stderr, "%d\n", pl->crn_nbr);
      for(idx=0; idx<pl->crn_nbr; idx++)
	    (void)fprintf(stderr,"%3.15f %3.15f\n",pl->dp_x[idx], pl->dp_y[idx]);
      (void)fprintf(stderr,"#\n");

      /*
      (void)fprintf(stderr,"dp_y ");
      for(idx=0; idx<pl->crn_nbr; idx++)
	(void)fprintf(stderr,"%20.14f, ",pl->dp_y[idx]);
      (void)fprintf(stderr,"\n");
       */
      (void)fprintf(stderr,"# min/max x( %g, %g) y(%g %g)\n", pl->dp_x_minmax[0], pl->dp_x_minmax[1], pl->dp_y_minmax[0], pl->dp_y_minmax[1]);
      
      break;

   case 1:  
   default:
     (void)fprintf(stderr,"%s: crn_nbr=%d src_id=%d\n", nco_prg_nm_get(), pl->crn_nbr, pl->src_id);
     
     for(idx=0; idx<pl->crn_nbr; idx++)
        (void)fprintf(stderr,"%3.15f %3.15f\n",pl->dp_x[idx], pl->dp_y[idx]);

     break;

   case 2:  
     (void)fprintf(stderr,"%s: crn_nbr=%d\n", nco_prg_nm_get(), pl->crn_nbr);
     
     for(idx=0; idx<pl->crn_nbr; idx++)
        (void)fprintf(stderr,"%20.15f %20.15f\n",pl->dp_x[idx], pl->dp_y[idx]);

     break;


    case 3:
      (void)fprintf(stderr,"%s: crn_nbr=%d shp follows \n", nco_prg_nm_get(), pl->crn_nbr);

      if(pl->pl_typ == poly_sph)
        for(idx=0; idx<pl->crn_nbr; idx++)
           (void)fprintf(stderr,"x=%f y=%f z=%f lon=%f lat=%f\n",pl->shp[idx][0], pl->shp[idx][1], pl->shp[idx][2], pl->shp[idx][3]*180.0 / M_PI, pl->shp[idx][4]*180.0 /M_PI );

      if(pl->pl_typ == poly_rll)
        for(idx=0; idx<pl->crn_nbr; idx++)
          (void)fprintf(stderr,"x=%f y=%f z=%f lon=%f lat=%f\n",pl->shp[idx][0], pl->shp[idx][1], pl->shp[idx][2], pl->shp[idx][3]*180.0 / M_PI, pl->shp[idx][4]*180.0 /M_PI );


      if(pl->pl_typ == poly_crt)
        for(idx=0; idx<pl->crn_nbr; idx++)
          (void)fprintf(stderr,"x=%f y=%f\n",pl->shp[idx][0], pl->shp[idx][1]);


       break;

      /* output kml polygon */
      case 10:

        (void)fprintf(stderr, "<Placemark>\n<Polygon><outerBoundaryIs> <LinearRing>\n<coordinates>\n" );

        for(idx=0; idx<pl->crn_nbr; idx++)
          //(void)fprintf(stderr,"%2.15f,%2.15f,0\n",pl->shp[idx][0]*180.0 / M_PI, pl->shp[idx][1]*180.0  / M_PI);
          (void)fprintf(stderr,"%2.15f,%2.15f,0\n",pl->dp_x[idx] , pl->dp_y[idx]);

       /* repeat first point */
       (void)fprintf(stderr,"%2.15f,%2.15f,0\n",pl->dp_x[0] , pl->dp_y[0]);
        (void)fprintf(stderr, "</coordinates>\n</LinearRing></outerBoundaryIs></Polygon>\n</Placemark>\n");

      break;

  }




  return;
     
}



poly_sct*
nco_poly_vrl_do(
poly_sct *pl_in,
poly_sct *pl_out,
int flg_snp_to,
char *sp_sng){

 int iret;
 int nbr_p=pl_in->crn_nbr;
 int nbr_q=pl_out->crn_nbr;
 int nbr_r=0;

 poly_sct *pl_vrl;

 pl_vrl=nco_poly_init_crn(pl_in->pl_typ,  ( nbr_p>=nbr_q ? 2*nbr_p: 2*nbr_q ) +1, pl_in->src_id      );
 /* manually set dst_id in struct */
 pl_vrl->dst_id=pl_out->src_id;

 nco_poly_shp_init(pl_vrl);


 // iret=nco_poly_intersect(pl_in, pl_out, pl_vrl, &nbr_r);

  switch(pl_in->pl_typ)
  {

    case poly_crt:
      iret = nco_crt_intersect(pl_in, pl_out, pl_vrl, &nbr_r);
      break;

    case poly_rll:
      iret = nco_rll_intersect(pl_in, pl_out, pl_vrl, &nbr_r);
      break;

    default:
    case poly_sph:
      iret = nco_sph_intersect(pl_in, pl_out, pl_vrl, &nbr_r, flg_snp_to, sp_sng );
      break;

  }




  if(iret!=EXIT_SUCCESS || nbr_r <3  )
 {
   pl_vrl = nco_poly_free(pl_vrl);

   //return pl_vrl;
   return (poly_sct*)NULL;

 }


 nco_poly_init_crn_re(pl_vrl, nbr_r);

 /* for production this should not be needed */
 nco_poly_dp_pop_shp(pl_vrl);

 return pl_vrl;
  
}

void
nco_poly_minmax_use_crn /* use the values of minmax box as dp_x, dp_y  */
(poly_sct *pl){

  pl->dp_x[0]=pl->dp_x_minmax[0];
  pl->dp_y[0]=pl->dp_y_minmax[0];

  pl->dp_x[1]=pl->dp_x_minmax[1];
  pl->dp_y[1]=pl->dp_y_minmax[0];

  pl->dp_x[2]=pl->dp_x_minmax[1];
  pl->dp_y[2]=pl->dp_y_minmax[1];

  pl->dp_x[3]=pl->dp_x_minmax[0];
  pl->dp_y[3]=pl->dp_y_minmax[1];
  
  return; 
  
}  


nco_bool
nco_poly_wrp_splt180(
poly_sct  *pl,
poly_sct **pl_wrp_left,
poly_sct ** pl_wrp_right)
{

  int idx;
  int cnt_right=0;

  poly_sct *pl_in;
  poly_sct *pl_bnds;
  // double dbl_le vft_thr_360=320.0;

  /* for wrapping width must be longer than max */
  if( pl->dp_x_minmax[1] - pl->dp_x_minmax[0] <  CELL_LONGITUDE_MAX )
     return NCO_ERR;

   
  
  /* deal with 0-360 grid for starters */
  pl_in=nco_poly_dpl(pl);

  /* make longitudes  RHS of dateline  positive */
  for(idx=0; idx<pl_in->crn_nbr; idx++)
  {
    if(pl_in->dp_x[idx] <  0.0){

      pl_in->dp_x[idx]+=360.0;
      cnt_right++;
    }
  }

  nco_poly_minmax_add(pl_in, nco_grd_lon_180_ctr, False);
  
  if( cnt_right == pl_in->crn_nbr || cnt_right==0 ) 
  {
    pl_in=nco_poly_free(pl_in);   
    return NCO_ERR;
  }
 
  
  /*  create right intersection polygon */
  pl_bnds=nco_poly_init_crn(pl->pl_typ, 4, pl->src_id );

  pl_bnds->dp_x_minmax[0]=180.0;
  pl_bnds->dp_x_minmax[1]=pl_in->dp_x_minmax[1];
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_minmax_use_crn(pl_bnds);

  /* do overlap */
  *pl_wrp_left= nco_poly_vrl_do(pl_in, pl_bnds, 0, (char*)NULL);

  /* must subtract  back the 360.0 we subtracted earlier */ 
  if(*pl_wrp_left){
    
    for(idx=0;idx< (*pl_wrp_left)->crn_nbr;idx++)
      (*pl_wrp_left)->dp_x[idx]-=360.0;


    nco_poly_minmax_add(*pl_wrp_left, nco_grd_lon_180_ctr, False);

    
    
  }

  /* now create bound for right polygon */
  pl_bnds->dp_x_minmax[0]=pl_in->dp_x_minmax[0];
  pl_bnds->dp_x_minmax[1]=180.0;
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_minmax_use_crn(pl_bnds);
  
  /* do overlap */
  *pl_wrp_right= nco_poly_vrl_do(pl_in, pl_bnds, 0, (char*)NULL);

  if(*pl_wrp_right)
  {

    nco_poly_minmax_add(*pl_wrp_right, nco_grd_lon_180_ctr, False);
     

  }
     
  

  pl_in=nco_poly_free(pl_in);
  pl_bnds=nco_poly_free(pl_bnds);



  if( *pl_wrp_left ||  *pl_wrp_right )
    return NCO_NOERR;
  else
    return NCO_ERR;
  
}





nco_bool
nco_poly_wrp_splt360(
poly_sct  *pl,
poly_sct **pl_wrp_left,
poly_sct ** pl_wrp_right)
{

  int idx;
  int cnt_left=0;

  poly_sct *pl_in;
  poly_sct *pl_bnds;

  
  
  /* deal with 0-360 grid for starters */
  pl_in=nco_poly_dpl(pl);

  /* make longitudes on LHS of GMT negative */
  for(idx=0; idx<pl_in->crn_nbr; idx++)
  {
    if(pl_in->dp_x[idx] > 180.0){

      pl_in->dp_x[idx]-=360.0;
      cnt_left++;
    }
  }

  nco_poly_minmax_add(pl_in, nco_grd_lon_Grn_ctr, True);
  
  if( cnt_left == pl_in->crn_nbr || cnt_left==0 ) 
  {
    pl_in=nco_poly_free(pl_in);   
    return NCO_ERR;
  }
 
  
  /*  create left intersection polygon */
  pl_bnds=nco_poly_init_crn(pl->pl_typ, 4, pl->src_id);

  pl_bnds->dp_x_minmax[0]=pl_in->dp_x_minmax[0];
  pl_bnds->dp_x_minmax[1]=-1.0e-13;
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_minmax_use_crn(pl_bnds);

  /* do overlap */
  *pl_wrp_left= nco_poly_vrl_do(pl_in, pl_bnds, 0,  (char*)NULL);

  /* must add back the 360.0 we subtracted earlier */ 
  if(*pl_wrp_left){
    
    for(idx=0;idx< (*pl_wrp_left)->crn_nbr;idx++)
      (*pl_wrp_left)->dp_x[idx]+=360.0;


    nco_poly_minmax_add(*pl_wrp_left, nco_grd_lon_Grn_ctr, True);

    /*
    if(  (*pl_wrp_left)->dp_x_minmax[1] - (*pl_wrp_left)->dp_x_minmax[0] > CELL_LONGITUDE_MAX )
      *pl_wrp_left=nco_poly_free(*pl_wrp_left);
     */
    
  }

  /* now create bound for right polygon */
  pl_bnds->dp_x_minmax[0]=0.0;
  pl_bnds->dp_x_minmax[1]=pl_in->dp_x_minmax[1];
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_minmax_use_crn(pl_bnds);
  
  /* do overlap */
  *pl_wrp_right= nco_poly_vrl_do(pl_in, pl_bnds, 0, (char*)NULL);

  if(*pl_wrp_right)
  {

    nco_poly_minmax_add(*pl_wrp_right, nco_grd_lon_Grn_ctr, True);
     
    /* 
    if(  (*pl_wrp_right)->dp_x_minmax[1] - (*pl_wrp_right)->dp_x_minmax[0] > CELL_LONGITUDE_MAX )
      *pl_wrp_right=nco_poly_free(*pl_wrp_right);
    */
  }
     
  

  pl_in=nco_poly_free(pl_in);
  pl_bnds=nco_poly_free(pl_bnds);



  if( *pl_wrp_left ||  *pl_wrp_right )
    return NCO_NOERR;
  else
    return NCO_ERR;
  
}




nco_bool
nco_poly_wrp_splt(
poly_sct  *pl,
nco_grd_lon_typ_enm grd_lon_typ,
poly_sct **pl_wrp_left,
poly_sct ** pl_wrp_right)
{




  if( grd_lon_typ ==  nco_grd_lon_Grn_wst || grd_lon_typ == nco_grd_lon_Grn_ctr)
    return nco_poly_wrp_splt360(pl, pl_wrp_left, pl_wrp_right); 

  else if( grd_lon_typ ==  nco_grd_lon_180_wst || grd_lon_typ == nco_grd_lon_180_ctr)
    return nco_poly_wrp_splt180(pl, pl_wrp_left, pl_wrp_right); 

  else
    return NCO_ERR;
  
  

}
  

void
nco_poly_re_org(
poly_sct *pl, 		  
double *lcl_dp_x,
double *lcl_dp_y)
{

  int idx;
  int idx1;
  
  int lcl_min=0;
  int crn_nbr=pl->crn_nbr;
  double x_min=KD_DBL_MAX;

    
  /* find index of min X value */
  for(idx=0; idx<crn_nbr; idx++)
  {  
    if( pl->dp_x[idx] < x_min )
      { x_min=pl->dp_x[idx]; lcl_min=idx;} 
  }

  /* first point already x_min so do nothing */
  if( lcl_min == 0)
    return;

  for(idx=0; idx<crn_nbr; idx++)
  {
     idx1=(idx+lcl_min)%crn_nbr;
     lcl_dp_x[idx]=pl->dp_x[idx1];
     lcl_dp_y[idx]=pl->dp_y[idx1];
  }  

  

    
  /* copy over values */
  memcpy(pl->dp_x, lcl_dp_x, sizeof(double) * crn_nbr);
  memcpy(pl->dp_y, lcl_dp_y, sizeof(double) * crn_nbr);    


  return;
  


}  


nco_bool  /* 0 [flg] True if polygon is convex */
nco_poly_is_convex(
poly_sct *pl)
{  

    nco_bool sign=False;
    nco_bool sign_init=False;
    
    int idx;
    int idx1;
    int idx2;
    int sz;
    double area;

    sz=pl->crn_nbr;

    for(idx=0;idx<sz; idx++)
    {
      idx1=(idx+1)%sz;
      idx2=(idx+2)%sz;
      area= ( pl->dp_x[idx1] -pl->dp_x[idx] )  *  (pl->dp_y[idx2] -pl->dp_y[idx1] )  -  (pl->dp_x[idx2] - pl->dp_x[idx1]) * (pl->dp_y[idx1] -pl->dp_y[idx]) ; 

      /* skip contiguous identical vertex */ 
      if(area==0.0)
	continue;
      
            
      if(!sign_init)
      {	
	sign=(area>0.0);
        sign_init=True;
      }
      
      if(sign != (area>0.0))
	return False;
      
      
    }
    return True;   





}

void
nco_poly_shp_init(  /* allocate shp to sph[crn_nbr][2] or shp[crn_nbr][5] */
poly_sct *pl)
{
  int idx;
  int nbr_al;

  nbr_al=nco_poly_typ_sz(pl->pl_typ);

  pl->shp=(double**) nco_calloc( pl->crn_nbr, sizeof(double*) );


  for(idx=0;idx<pl->crn_nbr;idx++)
    pl->shp[idx]= (double*) nco_calloc( sizeof(double), nbr_al );


}

void
nco_poly_shp_free(  /* allocate shp to sph[crn_nbr][2] or shp[crn_nbr][5] */
poly_sct *pl)
{
  int idx;

  for(idx=0;idx<pl->crn_nbr;idx++)
    if(pl->shp[idx]) pl->shp[idx]= (double*)nco_free(pl->shp[idx]);

  pl->shp=(double**)nco_free(pl->shp);

}

void
nco_poly_shp_pop(  /* fill out sph with values from dp_x, and dp_y */
poly_sct *pl)
{

  int idx;

  /* auto initialize - NOT a good idea ? */
  if(pl->shp == NULL_CEWI)
    nco_poly_shp_init(pl);

  if(pl->pl_typ == poly_crt) {
    for (idx = 0; idx < pl->crn_nbr; idx++) {
      pl->shp[idx][0] = pl->dp_x[idx];
      pl->shp[idx][1] = pl->dp_y[idx];
    }
  }


  if( pl->pl_typ == poly_sph)
  {
    nco_bool bDeg=True;
    for (idx = 0; idx < pl->crn_nbr; idx++)
      nco_geo_lonlat_2_sph(pl->dp_x[idx], pl->dp_y[idx], pl->shp[idx], False, bDeg);

  }

  else if(pl->pl_typ == poly_rll)
  {
    nco_bool bDeg=True;
    for (idx = 0; idx < pl->crn_nbr; idx++) {
      nco_geo_lonlat_2_sph(pl->dp_x[idx], pl->dp_y[idx], pl->shp[idx], True, bDeg);
    }

  }


} /* end */


void
nco_poly_dp_pop_shp(  /* fill out dp_x, dp_y with values from shp */
poly_sct *pl
)
{
   nco_bool bDeg=True;
   int idx;


  if(pl->pl_typ == poly_crt) {
    for (idx = 0; idx < pl->crn_nbr; idx++) {
      pl->dp_x[idx] = pl->shp[idx][0];
      pl->dp_y[idx] = pl->shp[idx][1];
    }
  }

  /* convert from vector to (lon,lat) - degrees */
  if( pl->pl_typ == poly_sph  )
  {
    for (idx = 0; idx < pl->crn_nbr; idx++)
      nco_geo_sph_2_lonlat(pl->shp[idx], &pl->dp_x[idx], &pl->dp_y[idx], bDeg);

  }

  /* remember RLL intersection uses lon/lat so use these values */
  else if(pl->pl_typ==poly_rll )
  {
    for (idx = 0; idx < pl->crn_nbr; idx++){
      pl->dp_x[idx]=R2D(pl->shp[idx][3] );
      pl->dp_y[idx]=R2D(pl->shp[idx][4] );

    }
  }


}


nco_bool
nco_poly_minmax_split(
poly_sct *pl,
nco_grd_lon_typ_enm nco_grd_typ,
kd_box size1,
kd_box size2)
{


  /* regular limits */
  if (pl->bwrp == False ) {
    /* choose whole range for a "polar triange " */
      size1[KD_LEFT] = pl->dp_x_minmax[0];
      size1[KD_RIGHT] = pl->dp_x_minmax[1];
      size1[KD_BOTTOM] = pl->dp_y_minmax[0];
      size1[KD_TOP] = pl->dp_y_minmax[1];

    return False;
  }

  /* This is a polar cap - remember the longitudes have been set to the WHOLE RANGE of Longitude in
   * nco_poly_minmax_add() */

  if(pl->bwrp==True && pl->bwrp_y==True)
  {
    size1[KD_LEFT] = pl->dp_x_minmax[0];
    size1[KD_RIGHT] = pl->dp_x_minmax[1];
    size1[KD_BOTTOM] = pl->dp_y_minmax[0];
    size1[KD_TOP] = pl->dp_y_minmax[1];

    return False;
  }





  /* regular limits */
  if (pl->bwrp == True) {


    switch (nco_grd_typ) {

      case nco_grd_lon_180_ctr:
      case nco_grd_lon_180_wst:
        size1[KD_LEFT] = pl->dp_x_minmax[1];
        size1[KD_RIGHT] = 180.0;

        size2[KD_LEFT] = -180.0 + 1.0e-20;
        size2[KD_RIGHT] = pl->dp_x_minmax[0];

        break;

      case nco_grd_lon_Grn_ctr:
      case nco_grd_lon_Grn_wst:
      default:
        size1[KD_LEFT] = pl->dp_x_minmax[1];
        size1[KD_RIGHT] = (360.0 - 1e-20);

        size2[KD_LEFT] = 0.0;
        size2[KD_RIGHT] = pl->dp_x_minmax[0];

        break;

    } /* end switch */

    /* regular limits for lat */
    size1[KD_BOTTOM] = pl->dp_y_minmax[0];
    size1[KD_TOP] = pl->dp_y_minmax[1];

    size2[KD_BOTTOM] = pl->dp_y_minmax[0];
    size2[KD_TOP] = pl->dp_y_minmax[1];

    return True;
  }

  return False;
}

/* simple temporary - function unless charlie has something better
 * looks at minmax and determin's domain (0,360) or (-180,80)
 */
nco_grd_lon_typ_enm
nco_poly_minmax_2_lon_typ
(poly_sct *pl)
{
  double lon_min;
  double lon_max;

  lon_min=pl->dp_x_minmax[0];
  lon_max=pl->dp_x_minmax[1];

  if( lon_max >180.0 )
    return nco_grd_lon_Grn_ctr;

  if(lon_max >0.0 && lon_max <=180.0 )
    return nco_grd_lon_180_ctr;

  if( (lon_min >=-180.0 && lon_min<0.0) || (lon_max >=-180.0 && lon_max<0.0) )
    return nco_grd_lon_180_ctr;



  return nco_grd_lon_nil;

}


/* simple evaluation of lat/lon centroid using averages  of dp_x and dp_y */
/* OK for now 20190307 */
void
nco_poly_ctr_add
(poly_sct *pl,
 nco_grd_lon_typ_enm lon_typ)
 {
   int idx;
   int sz;


   sz=pl->crn_nbr;

   double tot_lon=0.0;
   double tot_lat=0.0;
   double tmp_lon_ctr=0.0;


   for(idx=0; idx<sz; idx++)
   {
     tot_lon += pl->dp_x[idx];
     tot_lat += pl->dp_y[idx];

   }

   pl->dp_y_ctr=tot_lat / sz;

   tmp_lon_ctr=tot_lon / sz;

   /* no wrapping so use value of lon total above and return */
   if( !pl->bwrp )
   {
     pl->dp_x_ctr= tmp_lon_ctr;
     return;
   }

   /* here now so lon is wrapped */
   switch(lon_typ)
   {
     case nco_grd_lon_nil:
     case nco_grd_lon_bb:
     case nco_grd_lon_unk:
       pl->dp_x_ctr=tmp_lon_ctr;

     case nco_grd_lon_Grn_ctr:
     case nco_grd_lon_Grn_wst:
     /* create two averages one for lon(180,359.999) another for lon(0,179.99999) */
     {
       int cnt_0=0;
       int cnt_180=0;
       double tot_0=0.0;
       double tot_180=0.0;

       for(idx=0;idx<sz;idx++) {
         if (pl->dp_x[idx] >= 180.0) {
           tot_180 += pl->dp_x[idx];
           cnt_180++;
         } else if (pl->dp_x[idx] >= 0.0) {
           tot_0 += pl->dp_x[idx];
           cnt_0++;
         }
       }

       /* now do averages of average */
       if(cnt_0 || cnt_180)
         tmp_lon_ctr= (cnt_0 ? tot_0 / cnt_0 : 0.0 )  +  (cnt_180 ? tot_180/ cnt_180 -360.0 :0.0 ) ;

       if(cnt_0 && cnt_180)
         tmp_lon_ctr /= 2.0;

       if(tmp_lon_ctr <=0.0)
         tmp_lon_ctr+=360.0;

       }
       pl->dp_x_ctr=tmp_lon_ctr;
       break;

     case nco_grd_lon_180_ctr:
     case nco_grd_lon_180_wst:
       /* create two averages one for lon(180,359.999) another for lon(0,179.99999) */
     {
       int cnt_0=0;
       int cnt_180=0;
       double tot_0=0.0;
       double tot_180=0.0;

       for(idx=0;idx<sz;idx++) {
         if (pl->dp_x[idx] >= 0.0) {
           tot_0 += pl->dp_x[idx];
           cnt_0++;
         } else if (pl->dp_x[idx] < 0.0) {
           tot_180 += pl->dp_x[idx];
           cnt_180++;
         }
       }

       /* now do averages of average */
       if(cnt_0 || cnt_180)
         tmp_lon_ctr= (cnt_0 ? tot_0 / cnt_0 : 0.0 )  +  (cnt_180 ? tot_180/ cnt_180 + 360.0 :0.0 ) ;

       if(cnt_0 && cnt_180)
         tmp_lon_ctr /= 2.0;

       if(tmp_lon_ctr >180.0)
         tmp_lon_ctr-=360.0;

     }
       pl->dp_x_ctr=tmp_lon_ctr;
       break;




       break;



   }




}

int
nco_poly_intersect(
poly_sct *pl_in,
poly_sct *pl_out,
poly_sct *pl_vrl,
int *nbr_r){


  int iret;

  switch(pl_in->pl_typ)
  {

    case poly_crt:
      iret = nco_crt_intersect(pl_in, pl_out, pl_vrl, nbr_r);
      break;

    case poly_rll:
      iret = nco_rll_intersect(pl_in, pl_out, pl_vrl, nbr_r);
      break;

    default:
    case poly_sph:
      iret = nco_sph_intersect(pl_in, pl_out, pl_vrl, nbr_r,0, (char*)NULL );
      break;

  }

  return iret;




}


const char *
nco_poly_typ_sng_get(
poly_typ_enm pl_typ
)
{

  const char *sng=NULL;

  switch(pl_typ) {

    case poly_none:
      sng="poly_none";
      break;

    case poly_sph:
      sng="poly_sph";
      break;

    case poly_crt:
      sng="poly_crt";
      break;

    case poly_rll:
      sng="poly_rll";
      break;


  }

  return sng;


}




