/* $Header$ */

/* Purpose: Functions that manipulate lists of polygons */

/* Copyright (C) 2018--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

#include "nco_ply_lst.h"

void
nco_poly_re_org_lst(  /* for each poly_sct*  in list re-order points so that first point is the leftermost point */
poly_sct **pl_lst,
int arr_nbr)
{
  int idx=0;
  int jdx=0;
  int max_crn_nbr=0;

  double *lcl_dp_x;
  double *lcl_dp_y;

  /* max crn_nbr */
  for(idx=0 ; idx<arr_nbr ;idx++)
    if( pl_lst[idx]->crn_nbr > max_crn_nbr )
      max_crn_nbr = pl_lst[idx]->crn_nbr;

  lcl_dp_x=(double*)nco_calloc(max_crn_nbr, sizeof(double));
  lcl_dp_y=(double*)nco_calloc(max_crn_nbr, sizeof(double));


  for(idx=0; idx<arr_nbr; idx++)
  {
    int lcl_min=0;
    int crn_nbr=pl_lst[idx]->crn_nbr;
    double x_min=1.0e-30;

    /* de-reference */
    poly_sct *pl=pl_lst[idx];

    /* find index of min X value */
    for(jdx=0; jdx<crn_nbr; jdx++)
      if( pl->dp_x[jdx] < x_min )
      { x_min=pl->dp_x[jdx]; lcl_min=jdx;}

    /* first point already x_min so do nothing */
    if( lcl_min == 0)
      continue;

    for(jdx=0; jdx<crn_nbr; jdx++)
    {
      lcl_dp_x[jdx]=pl->dp_x[(jdx+lcl_min)%crn_nbr];
      lcl_dp_y[jdx]=pl->dp_y[(jdx+lcl_min)%crn_nbr];
    }



    /* copy over values */
    memcpy(pl->dp_x, lcl_dp_x, (size_t)crn_nbr*sizeof(double));
    memcpy(pl->dp_y, lcl_dp_y, (size_t)crn_nbr*sizeof(double));
  }

  lcl_dp_x=(double*)nco_free(lcl_dp_x);
  lcl_dp_y=(double*)nco_free(lcl_dp_y);

  return;

}

poly_sct **             /* [O] [nbr]  size of array */
nco_poly_lst_mk(
double *area, /* I [sr] Area of source grid */
int *msk, /* I [flg] Mask on source grid */
double *lat_ctr, /* I [dgr] Latitude  centers of source grid */
double *lon_ctr, /* I [dgr] Longitude centers of source grid */
double *lat_crn, /* I [dgr] Latitude  corners of source grid */
double *lon_crn, /* I [dgr] Longitude corners of source grid */
size_t grd_sz, /* I [nbr] Number of elements in single layer of source grid */
long grd_crn_nbr, /* I [nbr] Maximum number of corners in source gridcell */
nco_grd_lon_typ_enm grd_lon_typ, /* I [num] if not nil then split cells that straddle Greenwich or Dateline  */
poly_typ_enm pl_typ,
int *pl_nbr)
{
  const char fnc_nm[]="nco_poly_lst_mk()";

  int idx=0;
  int idx_cnt=0;
  int cnt_wrp_good=0;

  nco_bool bwrp;


  double *lat_ptr=lat_crn;
  double *lon_ptr=lon_crn;

  /* buffers  used in nco-poly_re_org() */
  double lcl_dp_x[VP_MAX]={0};
  double lcl_dp_y[VP_MAX]={0};

  poly_sct *pl;
  poly_sct *pl_wrp_left;
  poly_sct *pl_wrp_right;
  poly_sct **pl_lst;

  /* start with twice the grid size as we may be splitting the cells along the Greenwich meridian or dateline */
  /* realloc at the end */
  pl_lst=(poly_sct**)nco_malloc( sizeof (poly_sct*) * grd_sz  *2 );

  // printf("About to print poly sct   grd_sz=%d grd_crn_nbr=%d\n", grd_sz, grd_crn_nbr);
  for(idx=0;idx<grd_sz; idx++)
  {
    /* check mask and area */
    if( msk[idx]==0 || area[idx] == 0.0)
      continue;


    pl=nco_poly_init_lst(pl_typ, grd_crn_nbr,0, idx, lon_ptr, lat_ptr);
    lon_ptr+=(size_t)grd_crn_nbr;
    lat_ptr+=(size_t)grd_crn_nbr;

    /* if poly is less  than a triangle then  null is returned*/
    if(!pl)
      continue;


    /* add min max */
    nco_poly_minmax_add(pl, grd_lon_typ, False);

    nco_poly_re_org(pl, lcl_dp_x, lcl_dp_y);

    /* use Charlie's formula */
    nco_poly_area_add(pl);


    //if(pl->dp_x_minmax[0] <0.0 || (pl->dp_x_minmax[1] - pl->dp_x_minmax[0]) > 30  )
    if( !(pl->dp_x_minmax[1] - pl->dp_x_minmax[0] < 180.0
             &&  lon_ctr[idx] >= pl->dp_x_minmax[0] && lon_ctr[idx] <= pl->dp_x_minmax[1] ))

    {
      (void)fprintf(stdout, "/***%s: %s: invalid polygon to follow *******?", nco_prg_nm_get(), fnc_nm);
      nco_poly_prn(pl, 0);
      pl=nco_poly_free(pl);
      continue;

    }

    //fprintf(stdout,"/***** input polygon pl lon center=%f   convex=%s\n    ********************/\n", lon_ctr[idx],   (nco_poly_is_convex(pl) ? "True": "False") );
    //nco_poly_prn(pl, 0);



    /* check for wrapping -center outside min/max range */
    bwrp=(  lon_ctr[idx] < pl->dp_x_minmax[0] || lon_ctr[idx] > pl->dp_x_minmax[1] );

    if( grd_lon_typ == nco_grd_lon_nil || grd_lon_typ == nco_grd_lon_unk )
    {



      if( !bwrp  )
      {
        pl_lst[idx_cnt++]=pl;
      }
      else
      {
        (void)fprintf(stdout, "%s:  polygon(%d) wrapped - but grd_lon_typ not specified \n", nco_prg_nm_get(), idx);
        (void)fprintf(stdout, "/*******************************************/\n");

        pl=nco_poly_free(pl);

      }
      continue;
    }


    /* if we are here then grd_lon_typ specifys a grid type  */
    if( !bwrp)
    {
      pl_lst[idx_cnt++]=pl;
    }
      /* cell width exceeds max so assume wrapping */
    else if(  nco_poly_wrp_splt(pl, grd_lon_typ, &pl_wrp_left, &pl_wrp_right ) == NCO_NOERR )
    {

      fprintf(stdout,"/***** pl, wrp_left, wrp_right ********************/\n");


      if(pl_wrp_left)
      {
        nco_poly_re_org(pl_wrp_left, lcl_dp_x, lcl_dp_y);
        pl_lst[idx_cnt++]=pl_wrp_left;
        nco_poly_prn(pl_wrp_left, 2);

      }

      if(pl_wrp_right)
      {
        nco_poly_re_org(pl_wrp_right, lcl_dp_x, lcl_dp_y);
        pl_lst[idx_cnt++]=pl_wrp_right;
        nco_poly_prn(pl_wrp_right, 2);

      }

      pl=nco_poly_free(pl);


      fprintf(stdout,"/**********************************/\n");

      cnt_wrp_good++;
    }
    else
    {
      if(nco_dbg_lvl_get() >=  nco_dbg_std ){
        (void)fprintf(stdout, "%s: split wrapping didn't work on this polygon(%d)\n", nco_prg_nm_get(), idx );
        (void)fprintf(stdout, "/********************************/\n");
      }

      pl=nco_poly_free(pl);
    }


  }

  if(nco_dbg_lvl_get() >=  nco_dbg_std )
    (void)fprintf(stdout, "%s: %s size input list(%lu), size output list(%d), num of split polygons(%d)\n", nco_prg_nm_get(),fnc_nm, grd_sz, idx_cnt, cnt_wrp_good);

  pl_lst=(poly_sct**)nco_realloc( pl_lst, (size_t)idx_cnt * sizeof (poly_sct*) );

  *pl_nbr=idx_cnt;

  return pl_lst;

}


poly_sct **             /* [O] [nbr]  size of array */
nco_poly_lst_mk_rll(
double *area, /* I [sr] Area of source grid */
int *msk, /* I [flg] Mask on source grid */
double *lat_ctr, /* I [dgr] Latitude  centers of source grid */
double *lon_ctr, /* I [dgr] Longitude centers of source grid */
double *lat_crn, /* I [dgr] Latitude  corners of source grid */
double *lon_crn, /* I [dgr] Longitude corners of source grid */
size_t grd_sz, /* I [nbr] Number of elements in single layer of source grid */
long grd_crn_nbr, /* I [nbr] Maximum number of corners in source gridcell */
nco_grd_lon_typ_enm grd_lon_typ) /* I [num]  */
{

  int idx=0;
  int wrp_cnt=0;
  int wrp_y_cnt=0;
  int msk_cnt=0;

  const char fnc_nm[]="nco_poly_lst_mk_rll()";

  nco_bool bwrp;
  /* no polar caps on a RLL grid */
  nco_bool bchk_caps=False;

  double tot_area=0.0;

  double *lat_ptr=lat_crn;
  double *lon_ptr=lon_crn;


  poly_sct *pl=(poly_sct*)NULL_CEWI;

  /* contains plain struct  sct with bmsk=False; */
  poly_sct *pl_msk=((poly_sct*)NULL_CEWI);

  poly_sct **pl_lst;



  /* list size is grd_sz - invalid or masked polygons are repesented by a poly_sct->bmsk=False */
  pl_lst=(poly_sct**)nco_malloc( sizeof (poly_sct*) * grd_sz);

  pl_msk=nco_poly_init();
  pl_msk->bmsk=False;


  /* filter out wrapped lon cells */
  if( grd_lon_typ == nco_grd_lon_nil || grd_lon_typ == nco_grd_lon_unk || grd_lon_typ == nco_grd_lon_bb )
    bwrp=False;
  else
    bwrp=True;

  // printf("About to print poly sct   grd_sz=%d grd_crn_nbr=%d\n", grd_sz, grd_crn_nbr);
  for(idx=0;idx<grd_sz; idx++)
  {
    /* check mask and area */
    if( msk[idx]==0 || area[idx] == 0.0 ) {
      pl_lst[idx]= nco_poly_dpl(pl_msk);
      msk_cnt++;
      continue;
    }

    pl=nco_poly_init_lst(poly_rll, grd_crn_nbr,0, idx, lon_ptr, lat_ptr);
    lon_ptr+=(size_t)grd_crn_nbr;
    lat_ptr+=(size_t)grd_crn_nbr;

    /* if poly is less  than a triangle then  null is returned*/
    if(!pl ) {

      if(nco_dbg_lvl_get()>= nco_dbg_dev)
        fprintf(stderr, "%s(): WARNING cell(id=%d) less than a triange\n", fnc_nm, idx);

      pl_lst[idx]= nco_poly_dpl(pl_msk);
      msk_cnt++;
      continue;
    }

    /* add centroid from input  */
    pl->dp_x_ctr=lon_ctr[idx];
    pl->dp_y_ctr=lat_ctr[idx];

    /* pop shp */
    nco_poly_shp_pop(pl);

    /* add min max */
    nco_poly_minmax_add(pl, grd_lon_typ, bchk_caps);

    /* if coords cannot deal with wrapping */
    if( pl->bwrp  && bwrp==False   )
    {
      pl=nco_poly_free(pl);
      pl_lst[idx]= nco_poly_dpl(pl_msk);
      msk_cnt++;
      continue;
    }

    /* The area of an RLL grid needs to be re-calculated  as we have to take account of lines of latitude as great circles */
    nco_poly_area_add(pl);

    /* area NOT set so add to the area - nb this will be eventually written to the map file in nco_map_mk */
    if(area[idx]==-1.0)
      area[idx]=pl->area;


    /* simple center of a rll cell - should always be inside of polygon */
    nco_poly_ctr_add(pl, grd_lon_typ);

    if(nco_dbg_lvl_get()>= nco_dbg_dev  )
      if(pl->bwrp)
        nco_poly_prn(pl,0);

    /* for debugging */
    tot_area+=pl->area;

    /* for debugging total number of wrapped cells */
    wrp_cnt+=pl->bwrp;


    pl_lst[idx]=pl;
  }

  if(nco_dbg_lvl_get() >=  nco_dbg_dev )
    (void)fprintf(stderr, "%s: %s size input list(%lu), size output list(%lu)  total area=%.15e  num wrapped= %d num caps=%d num masked=%d\n", nco_prg_nm_get(),fnc_nm, grd_sz, grd_sz, tot_area, wrp_cnt, wrp_y_cnt, msk_cnt);

  pl_msk=nco_poly_free(pl_msk);

  return pl_lst;

}

poly_sct **             /* [O] [nbr]  size of array */
nco_poly_lst_mk_sph(
double *area, /* I [sr] Area of source grid */
int *msk, /* I [flg] Mask on source grid */
double *lat_ctr, /* I [dgr] Latitude  centers of source grid */
double *lon_ctr, /* I [dgr] Longitude centers of source grid */
double *lat_crn, /* I [dgr] Latitude  corners of source grid */
double *lon_crn, /* I [dgr] Longitude corners of source grid */
size_t grd_sz, /* I [nbr] Number of elements in single layer of source grid */
long grd_crn_nbr, /* I [nbr] Maximum number of corners in source gridcell */
nco_grd_lon_typ_enm grd_lon_typ) /* I [num]  */
{

  int idx=0;
  int wrp_cnt=0;
  int wrp_y_cnt=0;
  int msk_cnt=0;

  const char fnc_nm[]="nco_poly_lst_mk_sph()";

  nco_bool bwrp;
  /* check to see if cell is a polar cap */
  nco_bool bchk_caps=True;

  double tot_area=0.0;

  double *lat_ptr=lat_crn;
  double *lon_ptr=lon_crn;

  /* buffers  used in nco-poly_re_org() */
  


  poly_sct *pl=(poly_sct*)NULL_CEWI;

  /* contains plain struct  sct with bmsk=False; */
  poly_sct *pl_msk=((poly_sct*)NULL_CEWI);

  poly_sct **pl_lst;



  /* list size is grd_sz - invalid or masked polygons are repesented by a poly_sct->bmsk=False */
  pl_lst=(poly_sct**)nco_malloc( sizeof (poly_sct*) * grd_sz);

  pl_msk=nco_poly_init();
  pl_msk->bmsk=False;


  /* filter out wrapped lon cells */
  if( grd_lon_typ == nco_grd_lon_nil || grd_lon_typ == nco_grd_lon_unk || grd_lon_typ == nco_grd_lon_bb )
   bwrp=False;
  else
    bwrp=True;


  // printf("About to print poly sct   grd_sz=%d grd_crn_nbr=%d\n", grd_sz, grd_crn_nbr);
  for(idx=0;idx<grd_sz; idx++)
  {

    /* check mask and area */
    if( msk[idx]==0 || area[idx] == 0.0 ) {
      pl_lst[idx]= nco_poly_dpl(pl_msk);
      msk_cnt++;
      continue;

    }



    pl=nco_poly_init_lst(poly_sph, grd_crn_nbr,0, idx, lon_ptr, lat_ptr);
    lon_ptr+=(size_t)grd_crn_nbr;
    lat_ptr+=(size_t)grd_crn_nbr;

    /* if poly is less  than a triangle then  null is returned*/
    if(!pl ) {

      if(nco_dbg_lvl_get()>= nco_dbg_dev)
         fprintf(stderr, "%s(): WARNING cell(id=%d) less than a triange\n", fnc_nm, idx);

      pl_lst[idx]= nco_poly_dpl(pl_msk);
      msk_cnt++;
      continue;

    }
    
    /* add centroid from input  */
    pl->dp_x_ctr=lon_ctr[idx];
    pl->dp_y_ctr=lat_ctr[idx];

    

    /* pop shp */
    nco_poly_shp_pop(pl);

    /* add min max */
    nco_poly_minmax_add(pl, grd_lon_typ, bchk_caps);


    /* if coords cannot deal with wrapping */
    if( pl->bwrp  && bwrp==False   )
    {
      pl=nco_poly_free(pl);
      pl_lst[idx]= nco_poly_dpl(pl_msk);
      msk_cnt++;
      continue;

    }


    /* The area of an RLL grid needs to be re-calculated  as we have to take account of lines of latitude as great circles */
    nco_poly_area_add(pl);

    /* area NOT set so add to the area - nb this will be eventually written to the map file in nco_map_mk */
    if(area[idx]==-1.0)
       area[idx]=pl->area;

    /* fxm:2019-06-07 - there is a problem using the polygon center  as a control point as
     * for some RLL grids the center of a polar triangle can be the pole */

    /* The centroid can be outside of a convex polygon
     * for nco_sph_intersect_pre() to work correctly it requires a point INSIDE the convex polygon
     * So we use a custom function :
     * just remember FROM HERE on that the pl->dp_x_ctr, pl->dp_y_ctr iS NOT the Centroid
     * */

    /*
     if(nco_sph_inside_mk(pl,pControl ))
     {
       pl->dp_x_ctr=R2D(pControl[3]);
       pl->dp_y_ctr=R2D(pControl[4]);
     }
     else
       nco_poly_ctr_add(pl, grd_lon_typ);
     */



    if(nco_dbg_lvl_get()>= nco_dbg_dev  )
      if(pl->bwrp)
        nco_poly_prn(pl,0);

    /* for debugging */
    tot_area+=pl->area;

    /* for debugging total number of wrapped cells */
    wrp_cnt+=pl->bwrp;
    wrp_y_cnt+=pl->bwrp_y;

    pl_lst[idx]=pl;


  }

  if(nco_dbg_lvl_get() >=  nco_dbg_dev )
    (void)fprintf(stderr, "%s: %s size input list(%lu), size output list(%lu)  total area=%.15e  num wrapped= %d num caps=%d num masked=%d\n", nco_prg_nm_get(),fnc_nm, grd_sz, grd_sz, tot_area, wrp_cnt, wrp_y_cnt, msk_cnt);



  pl_msk=nco_poly_free(pl_msk);

  return pl_lst;

}

poly_sct **
nco_poly_lst_free(
poly_sct **pl_lst,
int arr_nbr)
{
  int idx;

  for(idx=0; idx<arr_nbr; idx++)
    pl_lst[idx]=nco_poly_free(pl_lst[idx]);

  pl_lst=(poly_sct**)nco_free(pl_lst);

  return pl_lst;

}


void
nco_poly_set_priority(
int nbr_lst,
KDPriority *list){

  int idx;

  for(idx=0;idx<nbr_lst;idx++){

    list[idx].dist = 1.1;
    list[idx].elem = (KDElem*)NULL;
  }

  return ;

}

poly_sct **
nco_poly_lst_mk_vrl_crt(   /* create overlap mesh  for crt polygons */
poly_sct **pl_lst_in,
int pl_cnt_in,
KDTree *rtree,
int *pl_cnt_vrl_ret){

/* just duplicate output list to overlap */

  size_t idx;
  size_t jdx;

  int max_nbr_vrl=1000;
  int pl_cnt_vrl=0;

  //nco_bool bSort=True;

  const char fnc_nm[]="nco_poly_mk_vrl_crt()";

  /* buffers  used in nco-poly_re_org() */
  double lcl_dp_x[VP_MAX]={0};
  double lcl_dp_y[VP_MAX]={0};


  kd_box size;

  poly_sct ** pl_lst_vrl=NULL_CEWI;

  KDPriority *list;

  list = (KDPriority *)nco_calloc(sizeof(KDPriority),(size_t)max_nbr_vrl);

  printf("INFO - entered function nco_poly_mk_vrl\n");


/* start main loop over input polygons */
  for(idx=0 ; idx<pl_cnt_in ;idx++ )
  {
    int cnt_vrl=0;
    int cnt_vrl_on=0;

    (void)nco_poly_set_priority(max_nbr_vrl,list);
    /* get bounds of polygon in */
    size[KD_LEFT]  =  pl_lst_in[idx]->dp_x_minmax[0];
    size[KD_RIGHT] =  pl_lst_in[idx]->dp_x_minmax[1];

    size[KD_BOTTOM] = pl_lst_in[idx]->dp_y_minmax[0];
    size[KD_TOP]    = pl_lst_in[idx]->dp_y_minmax[1];

    /* find overlapping polygons */

    // cnt_vrl=kd_nearest_intersect(rtree, size, max_nbr_vrl,list,bSort );

    /* nco_poly_prn(2, pl_lst_in[idx] ); */


    for(jdx=0; jdx <cnt_vrl ;jdx++){

      poly_sct *pl_vrl=(poly_sct*)NULL_CEWI;
      poly_sct *pl_out=(poly_sct*)list[jdx].elem->item;           ;


      // nco_poly_prn(2, pl_out);

      /* check for polygon in polygon first */
      if( nco_crt_poly_in_poly(pl_lst_in[idx], pl_out) == pl_out->crn_nbr )
      {
        //fprintf(stderr,"%s: using poly_in_poly()\n", fnc_nm);
        pl_vrl=nco_poly_dpl(pl_out);
      }
      else
        pl_vrl= nco_poly_vrl_do(pl_lst_in[idx], pl_out, 0,  (char*)NULL);

      if(pl_vrl){
        nco_poly_re_org(pl_vrl, lcl_dp_x, lcl_dp_y);
        /* add area */
        nco_poly_area_add(pl_vrl);
        /* shp not needed */
        nco_poly_shp_free(pl_vrl);

        pl_lst_vrl=(poly_sct**)nco_realloc(pl_lst_vrl, sizeof(poly_sct*) * (pl_cnt_vrl+1));
        pl_lst_vrl[pl_cnt_vrl]=pl_vrl;
        pl_cnt_vrl++;
        cnt_vrl_on++;

        if(nco_poly_is_convex(pl_vrl) == False )
        {
          fprintf(stderr,"%s: %s vrl polygon convex=0  vrl ,in convex=%d ,out convex=%d\n", nco_prg_nm_get(), fnc_nm, nco_poly_is_convex(pl_lst_in[idx]), nco_poly_is_convex(pl_out) );
          nco_poly_prn(pl_vrl, 2);
          nco_poly_prn(pl_lst_in[idx], 2);
          nco_poly_prn(pl_out, 2);

        }

        //fprintf(stderr,"Overlap polygon to follow\n");
        //nco_poly_prn(2, pl_vrl);

      }


    }

    if( nco_dbg_lvl_get() >= nco_dbg_dev )
      (void) fprintf(stderr, "%s: total overlaps=%d for polygon %lu - potential overlaps=%d actual overlaps=%d\n", nco_prg_nm_get(), pl_cnt_vrl,  idx, cnt_vrl, cnt_vrl_on);


  }




  list = (KDPriority *)nco_free(list);

  /* return size of list */
  *pl_cnt_vrl_ret=pl_cnt_vrl;


  return pl_lst_vrl;

}


void **
nco_poly_lst_mk_vrl(  /* create overlap mesh  for sph polygons */
poly_sct **pl_lst_in,
int pl_cnt_in,
nco_grd_lon_typ_enm grd_lon_typ,
poly_typ_enm pl_typ,
KDTree **tree,
int nbr_tr,
int lst_out_typ,
int *pl_cnt_vrl_ret){
  /* just duplicate output list to overlap */
  const char fnc_nm[]="nco_poly_lst_mk_vrl()";

  nco_bool bDirtyRats=False;
  nco_bool bSort=True;


  int pl_cnt_vrl=0;

  int thr_idx=0;
  int pl_cnt_dbg=0;
  int tot_nan_cnt=0;
  int tot_wrp_cnt=0;

  /* approx number of input cells each thread will process */
  int thr_quota;
  /* reporting step */
  int thr_quota_step;


  size_t idx;

  int lcl_thr_nbr;
  omp_mem_sct *mem_lst=NULL_CEWI;



  double tot_area=0.0;


  void** void_lst_vrl=NULL_CEWI;
  poly_sct** pl_lst_dbg=NULL_CEWI;

  FILE * const fp_stderr=stderr;
  


  lcl_thr_nbr=omp_get_max_threads();


  mem_lst=(omp_mem_sct*)nco_malloc(sizeof(omp_mem_sct)*lcl_thr_nbr);

  for(idx=0;idx<lcl_thr_nbr;idx++)
  {
    mem_lst[idx].pl_lst=NULL_CEWI;
    mem_lst[idx].wgt_lst=NULL_CEWI;
    mem_lst[idx].blk_nbr=0;
    mem_lst[idx].pl_cnt=0;
    mem_lst[idx].kd_list=(KDPriority *)nco_calloc(sizeof(KDPriority),(size_t)(NCO_VRL_BLOCKSIZE));
    mem_lst[idx].kd_cnt=0;
    mem_lst[idx].kd_blk_nbr=1;
    mem_lst[idx].idx_cnt=0;
  }

  thr_quota=pl_cnt_in/lcl_thr_nbr;
  thr_quota_step=thr_quota/20;
  if( thr_quota_step <2000 )
    thr_quota_step=2000;

/* NB: "OpenMP notes" section of nco_rgr.c has detailed discussion of these settings
     Henry, please keep the variables in alphabetical order within a clause and remember to update Intel */
#ifdef __GNUG__
  # define GCC_LIB_VERSION ( __GNUC__ * 100 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__ )
# if GCC_LIB_VERSION < 490
#  define GXX_OLD_OPENMP_SHARED_TREATMENT 1
# endif /* 480 */
#endif /* !__GNUC__ */
#if defined(__INTEL_COMPILER)
#  pragma omp parallel for private(idx,thr_idx) schedule(dynamic,40) shared(bDirtyRats,bSort,grd_lon_typ,nbr_tr,pl_cnt_dbg,pl_typ,tree,tot_nan_cnt,tot_wrp_cnt)
#else /* !__INTEL_COMPILER */
# ifdef GXX_OLD_OPENMP_SHARED_TREATMENT
#  pragma omp parallel for default(none) private(idx,thr_idx) shared(bDirtyRats,bSort,grd_lon_typ,nbr_tr,pl_cnt_dbg,pl_typ,tree,tot_nan_cnt,tot_wrp_cnt)
# else /* !old g++ */
#  pragma omp parallel for private(idx,thr_idx) schedule(dynamic,40) shared(bDirtyRats,bSort,grd_lon_typ,nbr_tr,pl_cnt_dbg,pl_typ,tree,tot_nan_cnt,tot_wrp_cnt)
# endif /* !old g++ */
#endif /* !__INTEL_COMPILER */
  for(idx=0 ; idx<pl_cnt_in ;idx++ ) {
    nco_bool bSplit=False;

    int vrl_cnt = 0;
    int vrl_cnt_on = 0;
    int nan_cnt=0;
    int wrp_cnt=0;

    size_t jdx;

    double vrl_area = 0.0;

    kd_box size1;
    kd_box size2;

    // (void) nco_poly_set_priority(max_nbr_vrl, list);

    thr_idx=omp_get_thread_num();


    if (0 && nco_dbg_lvl_get() >= nco_dbg_dev)
      fprintf(fp_stderr, "%s(): idx=%lu thr=%d\n",fnc_nm,  idx, thr_idx);


    if(pl_lst_in[idx]->bmsk==False)
      goto cont_msk;  /* need to iterate mem_lst diagnostics at end of loop*/

    mem_lst[thr_idx].kd_cnt=0;

    if(mem_lst[thr_idx].kd_blk_nbr >1)
    {
      mem_lst[thr_idx].kd_blk_nbr=1;

      mem_lst[thr_idx].kd_list=(KDPriority*)nco_free(mem_lst[thr_idx].kd_list);
      //mem_lst[thr_idx].kd_list=(KDPriority*)nco_realloc(mem_lst[idx].kd_list, sizeof(KDPriority) * NCO_VRL_BLOCKSIZE );
      mem_lst[idx].kd_list=(KDPriority *)nco_calloc(sizeof(KDPriority),(size_t)(NCO_VRL_BLOCKSIZE));


    }



    /* get bounds of polygon in */
    bSplit=nco_poly_minmax_split(pl_lst_in[idx],grd_lon_typ, size1,size2 );


    /* if a wrapped polygon then do two searches */
    if(bSplit)
      vrl_cnt = kd_nearest_intersect_wrp(tree, nbr_tr, size1, size2,  &mem_lst[thr_idx]);
    else
      vrl_cnt = kd_nearest_intersect(tree, nbr_tr, size1, &mem_lst[thr_idx], bSort);

    /* nco_poly_prn(2, pl_lst_in[idx] ); */


    for (jdx = 0; jdx < vrl_cnt; jdx++) {

      poly_sct *pl_vrl = NULL_CEWI;
      poly_sct *pl_out = (poly_sct *) mem_lst[thr_idx].kd_list[jdx].elem->item;

      /* for area debug only */
      mem_lst[thr_idx].kd_list[jdx].area=-1.0;
      mem_lst[thr_idx].kd_list[jdx].dbg_sng[0]='\0';

      /*
      if (pl_lst_in[idx]->pl_typ != pl_out->pl_typ) {
        fprintf(stderr, "%s: %s poly type mismatch\n", nco_prg_nm_get(), fnc_nm);
        continue;
      }
      */




      if(pl_typ== poly_rll)
      {

        pl_vrl = nco_poly_vrl_do(pl_lst_in[idx], pl_out, 0, (char*)NULL);

        /* if pl_vrl is NULL from,  nco_poly_do_vrl()  then there are 3 possible senario's
         *
         *  1) pl_lst_in[idx] and pl_out are seperate and  distinct
         *
         *  2) pl_lst_in[idx] is entirely inside pl_out.
         *     to check for this it is sufficent to check the
         *     the minmax bounds and then also  check that a single vertex
         *     from pl_lst_in[idx] is inside pl_out
         *
         *  3) pl_out is entirely inside pl_lst_in[idx]
         *     check minmax bounds then check a single vertex from
         *     pl_out
         */



        if(!pl_vrl)
        {
          if (nco_poly_in_poly_minmax(pl_lst_in[idx], pl_out))
            pl_vrl = nco_poly_dpl(pl_out);
          else if (nco_poly_in_poly_minmax(pl_out, pl_lst_in[idx]))
            pl_vrl = nco_poly_dpl(pl_lst_in[idx]);

        }

        if(pl_vrl)
        {
          /* add area nb also sets wrapping */
          nco_poly_minmax_add(pl_vrl, grd_lon_typ, False);

          /* REMEMBER  poly_rll area uses minmax limits AND NOT VERTEX's */
          nco_poly_area_add(pl_vrl);
        }



      }

      if(pl_typ== poly_sph ) {


        int lret = 0;
        int lret2=0;

        /* [flg] set by nco_sph_intersect_pre - if True it means that scan hase detected a genuine intersection so
         * so no need to do further processing */
        nco_bool bGenuine=False;
        nco_bool bBadArea=False;
        char in_sng[VP_MAX];
        char out_sng[VP_MAX];

        int flg_snp_to=2;

        in_sng[0]='\0';
        out_sng[0]='\0';

        nco_sph_intersect_pre(pl_lst_in[idx], pl_out, in_sng);

        if(0 && nco_dbg_lvl_get() >= nco_dbg_dev)
          (void)fprintf(stderr,"%s:%s(): sp_sng=%s \n",nco_prg_nm_get(),fnc_nm, in_sng  );


        lret = nco_sph_process_pre(pl_out, in_sng, &bGenuine);

        switch(lret)
        {

          case 1:
            pl_vrl = nco_poly_dpl(pl_out);
            break;


          case 2:
            pl_vrl = nco_poly_vrl_do(pl_lst_in[idx], pl_out, flg_snp_to, (char *)NULL);
            break;

          case 3:
            pl_vrl = nco_poly_vrl_do(pl_lst_in[idx], pl_out, flg_snp_to, in_sng);
            break;


          case 4:
            pl_vrl = nco_poly_vrl_do(pl_lst_in[idx], pl_out, flg_snp_to,  (char*)NULL);
            break;

          case 5:
            pl_vrl = nco_poly_vrl_do(pl_lst_in[idx], pl_out, flg_snp_to, in_sng);
            break;


        }

        if(pl_vrl)
        {
          double min_area= ( pl_out->area < pl_lst_in[idx]->area ? pl_out->area : pl_lst_in[idx]->area  );
          /* add area nb also sets wrapping */
          nco_poly_minmax_add(pl_vrl, grd_lon_typ, False);

          /* REMEMBER  poly_rll area uses minmax limits AND NOT VERTEX's */
          nco_poly_area_add(pl_vrl);

          if( isnan( pl_vrl->area) || pl_vrl->area == 0.0 || (pl_vrl->area - min_area)/ min_area  > 1.0e-04 ) {

            pl_vrl = nco_poly_free(pl_vrl);
            bBadArea=True;
          }

        }



        /* swap args around and try again */
        if(!pl_vrl &&   ( (bGenuine==False && lret !=1 ) || bBadArea ) )
          //if(!pl_vrl &&   ( (bGenuine==False && lret !=1 ) || bBadArea ) )
        {

          flg_snp_to=1;

          nco_sph_intersect_pre(pl_out, pl_lst_in[idx], out_sng);
          lret2 = nco_sph_process_pre(pl_lst_in[idx], out_sng, &bGenuine);


          switch(lret2)
          {

            case 1:
              pl_vrl = nco_poly_dpl(pl_lst_in[idx]);
              break;


            case 2:
              pl_vrl = nco_poly_vrl_do(pl_out, pl_lst_in[idx], flg_snp_to, (char *)NULL);
              break;

            case 3:
              pl_vrl = nco_poly_vrl_do(pl_out, pl_lst_in[idx], flg_snp_to, out_sng);
              break;


            case 4:
              pl_vrl = nco_poly_vrl_do(pl_out, pl_lst_in[idx], flg_snp_to, (char*)NULL);
              break;

            case 5:
              pl_vrl = nco_poly_vrl_do(pl_out, pl_lst_in[idx], flg_snp_to,  out_sng);
              break;


          }

          if(pl_vrl)
          {
            nco_poly_minmax_add(pl_vrl, grd_lon_typ, False);
            nco_poly_area_add(pl_vrl);
          }

        }


        if(bDirtyRats )
          sprintf(mem_lst[thr_idx].kd_list[jdx].dbg_sng, "lret=%d in_sng=%s   lret2=%d  out_sng=%s\n",lret, in_sng, lret2, out_sng);


        if(bDirtyRats && pl_vrl && !nco_sph_is_convex(pl_vrl->shp, pl_vrl->crn_nbr) )
        {

          (void) fprintf(stderr, "/************* concave overlap plygon***********/\n");
          nco_poly_prn(pl_lst_in[idx],0);
          nco_poly_prn(pl_out,0);
          nco_poly_prn(pl_vrl, 0);
          (void) fprintf(stderr, "/***********************************************/\n");

        }




      } /* end if poly_sph */




      if (pl_vrl) {
        // nco_poly_re_org(pl_vrl, lcl_dp_x, lcl_dp_y);

        /* add aprropriate id's */
        pl_vrl->src_id = pl_lst_in[idx]->src_id;
        pl_vrl->dst_id = pl_out->src_id;





        /* shp not needed */
        nco_poly_shp_free(pl_vrl);

        /* calculate weight -simple ratio of areas */
        pl_vrl->wgt=pl_vrl->area / pl_out->area;

        /* add lat/lon centers */
        nco_poly_ctr_add(pl_vrl, grd_lon_typ);


        wrp_cnt+=pl_vrl->bwrp;

        if( isnan(pl_vrl->area) || pl_vrl->area ==0.0 )
        { nan_cnt++;
          pl_vrl->area=0.0;

          pl_vrl=nco_poly_free(pl_vrl);
          continue;
        }

        /* for input polygon wgt is used to calculate frac_a */
        pl_lst_in[idx]->wgt+= ( pl_vrl->area / pl_lst_in[idx]->area );



        /*
#ifdef _OPENMP
#pragma omp critical
#endif
        {
          pl_out->wgt+=pl_vrl->wgt;

        }
*/

        vrl_area += pl_vrl->area;
        mem_lst[thr_idx].kd_list[jdx].area=pl_vrl->area;


        if( mem_lst[thr_idx].blk_nbr * NCO_VRL_BLOCKSIZE <  mem_lst[thr_idx].pl_cnt +1 ){
          if(lst_out_typ==1)
            mem_lst[thr_idx].wgt_lst= (wgt_sct**)nco_realloc(mem_lst[thr_idx].wgt_lst, sizeof(wgt_sct*) * ++mem_lst[thr_idx].blk_nbr * NCO_VRL_BLOCKSIZE );
          else if(lst_out_typ==2)
            mem_lst[thr_idx].pl_lst= (poly_sct**)nco_realloc(mem_lst[thr_idx].pl_lst, sizeof(poly_sct*) * ++mem_lst[thr_idx].blk_nbr * NCO_VRL_BLOCKSIZE );
        }


        if(lst_out_typ==1) {
          wgt_sct *wgt_lcl=(wgt_sct*)nco_calloc(1,sizeof(wgt_sct));
          wgt_lcl->src_id=pl_vrl->src_id;
          wgt_lcl->dst_id=pl_vrl->dst_id;
          wgt_lcl->area=pl_vrl->area;
          wgt_lcl->wgt=pl_vrl->wgt;
          mem_lst[thr_idx].wgt_lst[mem_lst[thr_idx].pl_cnt++] =wgt_lcl;

          pl_vrl=nco_poly_free(pl_vrl);
        }
        else if(lst_out_typ==2 )
          mem_lst[thr_idx].pl_lst[mem_lst[thr_idx].pl_cnt++] = pl_vrl;

        vrl_cnt_on++;

        /* for area debug only */





      }


    } /* end jdx */



    if (nco_dbg_lvl_get() >= nco_dbg_dev) {

#ifdef _OPENMP
#pragma omp critical
#endif
      {
        tot_nan_cnt += nan_cnt;
        tot_wrp_cnt += wrp_cnt;
        tot_area += vrl_area;

      } /* end OMP critical */

      /* area diff by more than 10% */
      int kdx;
      double eps = 1e-8;
      double frc = vrl_area / pl_lst_in[idx]->area;
      if (frc < (1 - eps) || frc > 1 + eps) {
        (void) fprintf(fp_stderr,
                       "%s: polygon %lu - potential overlaps=%d actual overlaps=%d area_in=%.10e vrl_area=%.10e  adiff=%.15e bSplit=%d\n",
                       nco_prg_nm_get(), idx, vrl_cnt, vrl_cnt_on, pl_lst_in[idx]->area, vrl_area,
                       (pl_lst_in[idx]->area - vrl_area), bSplit);


        if (bDirtyRats) {
          //if (pl_lst_in[idx]->bwrp ) {

          if (1) {
            (void) fprintf(fp_stderr, "# /** following pl_lst_in[%lu]  **/\n", idx);
            nco_poly_prn(pl_lst_in[idx], 0);
            (void) fprintf(fp_stderr, "# /** overlaps to  follow  **/\n");
            for (kdx = 0; kdx < vrl_cnt; kdx++) {
              nco_poly_prn((poly_sct *) mem_lst[thr_idx].kd_list[kdx].elem->item, 0);
              (void)fprintf(fp_stderr, "# vrl_area=%.15e\n",mem_lst[thr_idx].kd_list[kdx].area );
              (void)fprintf(fp_stderr, "# dbg_sng=%s\n",mem_lst[thr_idx].kd_list[kdx].dbg_sng );

            }

            (void) fprintf(stderr, "/************* end dirty rats ***************/\n");
          }

          if(1 && vrl_cnt_on>0 && lst_out_typ==2 ) {


            pl_lst_dbg = (poly_sct **) nco_realloc(pl_lst_dbg, sizeof(poly_sct *) * (pl_cnt_dbg + vrl_cnt_on));

            /* write overlaps maybe */
            for (kdx = 0; kdx < vrl_cnt_on; kdx++) {
              poly_sct * lcl_poly=mem_lst[thr_idx].pl_lst[mem_lst[thr_idx].pl_cnt - vrl_cnt_on + kdx];

              if(lcl_poly->src_id== pl_lst_in[idx]->src_id )
                pl_lst_dbg[pl_cnt_dbg++] = nco_poly_dpl(lcl_poly);

            }



          }



        }

      }

    } /* end dbg */

    cont_msk: ;

    /* output some usefull tracking stuff - not debug but informative */
    if (  ++mem_lst[thr_idx].idx_cnt % thr_quota_step == 0 && nco_dbg_lvl_get() >=3   )
      (void)fprintf(fp_stderr, "%s: thread %d  has processed %2.2f%% (%ld) of src cells quota and output %ld overlap cells\n",    nco_prg_nm_get(), thr_idx, (float)mem_lst[thr_idx].idx_cnt/(float)thr_quota *100.0,  mem_lst[thr_idx].idx_cnt, mem_lst[thr_idx].pl_cnt  );

  } /* end for idx */

  /* turn tot_area into a % of 4*PI */
  /* tot_area = tot_area / 4.0 / M_PI *100.0; */

  /* final report */
  if (nco_dbg_lvl_get() >= nco_dbg_dev)
    (void) fprintf(stderr, "%s: total overlaps=%d, total_area=%.15f (area=%3.10f%%) total num wrapped= %d total nan nbr=%d \n", nco_prg_nm_get(), pl_cnt_vrl, tot_area, tot_area /4.0 / M_PI *100.0, tot_wrp_cnt, tot_nan_cnt);

  /* write filtered polygons to file */
  if(bDirtyRats && pl_cnt_dbg)
  {
    nco_msh_poly_lst_wrt("tst-wrt-dbg.nc", pl_lst_dbg, pl_cnt_dbg, grd_lon_typ,NC_FORMAT_NETCDF4);
    pl_lst_dbg=(poly_sct**)nco_poly_lst_free(pl_lst_dbg, pl_cnt_dbg);
  }

  /* concatenate memory list, place results into first member list */
  nco_mem_lst_cat(mem_lst, lcl_thr_nbr);

  /* free up kd_list's */
  for(idx=0;idx<lcl_thr_nbr;idx++)
    mem_lst[idx].kd_list= (KDPriority*) nco_free(mem_lst[idx].kd_list);

  *pl_cnt_vrl_ret=mem_lst[0].pl_cnt;

  if(lst_out_typ==1)
    void_lst_vrl=(void**) mem_lst[0].wgt_lst;
  else if( lst_out_typ==2 )
    void_lst_vrl=(void**) mem_lst[0].pl_lst;

  mem_lst=(omp_mem_sct*)nco_free(mem_lst);

  /* REMEMBER the void type can be a wgt_sct** array or a poly_sct** array
   * wgt_sct is a subset of poly_sct - with simple members */
  return void_lst_vrl;
}

/* check areas - nb WARNING modifies area in pl_lst_in and pl_lst_out */
void nco_poly_lst_chk(
poly_sct **pl_lst_in,
int pl_cnt_in,
poly_sct **pl_lst_out,
int pl_cnt_out,
poly_sct **pl_lst_vrl,
int pl_cnt_vrl)
{
  int id;
  int idx;
  int jdx;

  double epsilon=1.0e-8;

  const char fnc_nm[]="nco_poly_lst_chk()";

  for(idx=0;idx<pl_cnt_vrl;idx++)
  {
    id=pl_lst_vrl[idx]->src_id;
    for(jdx=0;jdx<pl_cnt_in;jdx++)
      if(pl_lst_in[jdx]->src_id==id)
        break;

    if(jdx < pl_cnt_in )
      pl_lst_in[jdx]->area-=pl_lst_vrl[idx]->area;

  }

  fprintf(stderr, "%s():WARNING following is list of incomplete src cells, by src_id no\n",fnc_nm);
  for(idx=0;idx<pl_cnt_in;idx++)
    if( fabs(  pl_lst_in[idx]->area) > epsilon)
      fprintf(stderr, "src_id=%d area=%.10f\n", pl_lst_in[idx]->src_id, pl_lst_in[idx]->area );


  for(idx=0;idx<pl_cnt_vrl;idx++)
  {
    id=pl_lst_vrl[idx]->dst_id;
    for(jdx=0;jdx<pl_cnt_out;jdx++)
      if(pl_lst_out[jdx]->src_id==id)
        break;

    if(jdx < pl_cnt_out )
      pl_lst_out[jdx]->area-=pl_lst_vrl[idx]->area;

  }

  fprintf(stderr, "%s():WARNING following is list of incomplete dst cells, by src_id no\n",fnc_nm);
  for(idx=0;idx<pl_cnt_out;idx++)
    if( fabs(  pl_lst_out[idx]->area) > epsilon)
      fprintf(stderr, "src_id=%d area=%.10f\n", pl_lst_out[idx]->src_id, pl_lst_out[idx]->area );

   return;
}

poly_sct **
nco_poly_lst_chk_dbg(
poly_sct **pl_lst,
int pl_cnt,
poly_sct **pl_lst_vrl,
int pl_cnt_vrl,
int io_flg,  /* [flg] 0 - use src_id from vrl, 1 - use dst_id from vrl */
int *pl_cnt_dbg) /* size of output dbg grid */
{
  int id;
  int idx;
  int jdx;

  int pl_nbr_dbg=0;
  double epsilon=1.0e-12;
  double *area=NULL_CEWI;

  nco_bool is_lst_cnt=False;

  /* if true then pl_cnt matches max src_id There are no missing records from NetCDF SCRIP input */
  is_lst_cnt=( pl_cnt== pl_lst[pl_cnt-1]->src_id +1);

  poly_sct **pl_lst_dbg=NULL_CEWI;

  const char fnc_nm[]="nco_poly_lst_chk_dbg()";

  area=(double*)nco_malloc(sizeof(double)*pl_cnt);
  for(idx=0;idx<pl_cnt;idx++)
    area[idx]=pl_lst[idx]->area;


  for(idx=0;idx<pl_cnt_vrl;idx++)
  {

    id = (io_flg ? pl_lst_vrl[idx]->dst_id : pl_lst_vrl[idx]->src_id);

    if(is_lst_cnt )
      area[id] -= pl_lst_vrl[idx]->area;
    else
    {
      for (jdx = 0; jdx < pl_cnt; jdx++)
        if (pl_lst[jdx]->src_id == id)
          break;

      if (jdx < pl_cnt)
        area[jdx] -= pl_lst_vrl[idx]->area;
    }

  }


  for(idx=0;idx<pl_cnt;idx++) {
    if (fabs(area[idx]) > epsilon) {

      if (nco_dbg_lvl_get() >= nco_dbg_dev)
        fprintf(stderr, "%s() src_id=%d area=%.15e\n", fnc_nm, pl_lst[idx]->src_id, area[idx]);

      pl_lst_dbg = (poly_sct **) nco_realloc(pl_lst_dbg, sizeof(poly_sct*) * (pl_nbr_dbg + 1));
      pl_lst_dbg[pl_nbr_dbg] = nco_poly_dpl(pl_lst[idx]);
      pl_nbr_dbg++;
    }
  }


  area=(double*)nco_free(area);

  *pl_cnt_dbg=pl_nbr_dbg;

  return pl_lst_dbg;
}


wgt_sct **
nco_poly_lst_mk_dwe_sph(
rgr_sct *const rgr_nfo,
poly_sct **pl_lst_out,
int pl_cnt,
nco_grd_lon_typ_enm grd_lon_typ,
KDTree **tree,
int nbr_tr,
int *wgt_cnt_bln_ret) {

  /* just duplicate output list to overlap */
  const char fnc_nm[] = "nco_poly_lst_mk_dwe_sph()";

  int thr_idx = 0;
  /* approx number of input cells each thread will process */
  int thr_quota;
  /* reporting step */
  int thr_quota_step;
  /* max number of nearest neighbours to consider - nr-reference from rgr_nfo */
  int const max_nbr_dwe=20;
  int nbr_dwe=0;

  double pow_dwe=0.0;

  double min_dist=1.0e-12;
  double min_wgt=1.0e-20;

  poly_typ_enm pl_typ;
  size_t idx;

  int lcl_thr_nbr;
  omp_mem_sct *mem_lst = NULL_CEWI;

  wgt_sct **wgt_lst_dwe = NULL_CEWI;

  FILE *const fp_stderr = stderr;

  pl_typ = pl_lst_out[0]->pl_typ;

  lcl_thr_nbr = omp_get_max_threads();

  nbr_dwe= ( rgr_nfo->xtr_nsp > max_nbr_dwe ? max_nbr_dwe : rgr_nfo->xtr_nsp );

  pow_dwe=rgr_nfo->xtr_xpn;

  mem_lst = (omp_mem_sct *) nco_malloc(sizeof(omp_mem_sct) * lcl_thr_nbr);

  for (idx = 0; idx < lcl_thr_nbr; idx++) {
    mem_lst[idx].wgt_lst = NULL_CEWI;
    mem_lst[idx].blk_nbr = 0;
    mem_lst[idx].pl_cnt = 0;
    mem_lst[idx].kd_list = (KDPriority *) nco_calloc(sizeof(KDPriority), (size_t) (NCO_VRL_BLOCKSIZE));
    mem_lst[idx].kd_cnt = 0;
    mem_lst[idx].kd_blk_nbr = 1;
    mem_lst[idx].idx_cnt = 0;
  }

  thr_quota = pl_cnt / lcl_thr_nbr;
  thr_quota_step = thr_quota / 20;
  if (thr_quota_step < 2000)
    thr_quota_step = 2000;
  
  /* NB: "OpenMP notes" section of nco_rgr.c has detailed discussion of these settings
     Henry, please keep the variables in alphabetical order within a clause and remember to update Intel */
#ifdef __GNUG__
  # define GCC_LIB_VERSION ( __GNUC__ * 100 + __GNUC_MINOR__ * 10 + __GNUC_PATCHLEVEL__ )
# if GCC_LIB_VERSION < 490
#  define GXX_OLD_OPENMP_SHARED_TREATMENT 1
# endif /* 480 */
#endif /* !__GNUC__ */
#if defined(__INTEL_COMPILER)
#  pragma omp parallel for private(idx,thr_idx) schedule(dynamic,40) shared(grd_lon_typ,nbr_tr,pl_typ,tree)
#else /* !__INTEL_COMPILER */
# ifdef GXX_OLD_OPENMP_SHARED_TREATMENT
#  pragma omp parallel for default(none) private(idx,thr_idx) shared(bDirtyRats,grd_lon_typ,max_nbr_vrl,nbr_tr,pl_cnt_dbg,pl_typ,tree,tot_nan_cnt,tot_wrp_cnt)
# else /* !old g++ */
#  pragma omp parallel for private(idx,thr_idx) schedule(dynamic,40) shared(grd_lon_typ,nbr_tr,pl_typ,tree)
# endif /* !old g++ */
#endif /* !__INTEL_COMPILER */
  for (idx = 0; idx < pl_cnt; idx++) {

    double dp_x_wrp;   /* used to do a wrapped lon search */
    double wgt_ttl=0.0;

    int nbr_dwe_cnt; /* equal to or less than nbr_nni */

    wgt_sct wgt_pre[max_nbr_dwe];

    wgt_sct * wgt_lcl=NULL_CEWI;
    poly_sct *pl=NULL_CEWI;

    size_t jdx;
    size_t kdx;
    size_t nbr_lst_lcl;

    // (void) nco_poly_set_priority(max_nbr_vrl, list);

    thr_idx = omp_get_thread_num();

    if (0 && nco_dbg_lvl_get() >= nco_dbg_dev)
      fprintf(fp_stderr, "%s(): idx=%lu thr=%d\n", fnc_nm, idx, thr_idx);

    if (pl_lst_out[idx]->bmsk == False)
      continue;

    mem_lst[thr_idx].kd_cnt = 0;

    if (mem_lst[thr_idx].kd_blk_nbr > 1) {
      mem_lst[thr_idx].kd_blk_nbr = 1;

      mem_lst[thr_idx].kd_list = (KDPriority *) nco_free(mem_lst[thr_idx].kd_list);
      //mem_lst[thr_idx].kd_list=(KDPriority*)nco_realloc(mem_lst[idx].kd_list, sizeof(KDPriority) * NCO_VRL_BLOCKSIZE );
      mem_lst[idx].kd_list = (KDPriority *) nco_calloc(sizeof(KDPriority), (size_t) (NCO_VRL_BLOCKSIZE));
    }

    /* get bounds of polygon in */
    //bSplit = nco_poly_minmax_split(pl_lst_out[idx], grd_lon_typ, size1, size2);

    dp_x_wrp=KD_DBL_MAX;

    for(kdx=0;kdx<nbr_tr;kdx++)
      kd_nearest(tree[kdx], pl_lst_out[idx]->dp_x_ctr, pl_lst_out[idx]->dp_y_ctr, pl_typ,  nbr_dwe, mem_lst[thr_idx].kd_list + nbr_dwe *kdx );

    nbr_lst_lcl=nbr_dwe*nbr_tr;

    switch(grd_lon_typ)
    {
      case nco_grd_lon_nil:
      case nco_grd_lon_unk:
      case nco_grd_lon_Grn_ctr:
      case nco_grd_lon_Grn_wst:
      case nco_grd_lon_bb:
        if(pl_lst_out[idx]->dp_x_ctr <180.0)
          dp_x_wrp=pl_lst_out[idx]->dp_x_ctr+360.0;
        else if(pl_lst_out[idx]->dp_x_ctr >180.0)
          dp_x_wrp=pl_lst_out[idx]->dp_x_ctr-360.0;
        break;

      case nco_grd_lon_180_wst:
      case nco_grd_lon_180_ctr:
        if(pl_lst_out[idx]->dp_x_ctr <0.0)
          dp_x_wrp=pl_lst_out[idx]->dp_x_ctr+360.0;
        else if(pl_lst_out[idx]->dp_x_ctr >0.0)
          dp_x_wrp=pl_lst_out[idx]->dp_x_ctr-360.0;
        break;
    }

    if(dp_x_wrp != KD_DBL_MAX)
    {
      for (kdx = 0; kdx < nbr_tr; kdx++)
        kd_nearest(tree[kdx], dp_x_wrp, pl_lst_out[idx]->dp_y_ctr, pl_typ, nbr_dwe, mem_lst[thr_idx].kd_list + nbr_lst_lcl+nbr_dwe * kdx);

      nbr_lst_lcl+=nbr_dwe*nbr_tr;
    }

    if(nbr_tr >1 )
      qsort((void *)mem_lst[thr_idx].kd_list, nbr_lst_lcl,  sizeof(KDPriority), kd_priority_cmp_dist);

    /* remember kd list sorted according to distance */

    /* check first member distance if min then output singleton */
    if(mem_lst[thr_idx].kd_list[0].dist <= min_dist)
    {

      pl=(poly_sct*)mem_lst[thr_idx].kd_list[0].elem->item;

      wgt_lcl=(wgt_sct*)nco_malloc(sizeof(wgt_sct));
      wgt_lcl->src_id=pl->src_id;
      wgt_lcl->dst_id=pl_lst_out[idx]->src_id;
      wgt_lcl->area=pl->area;
      wgt_lcl->dist=mem_lst[thr_idx].kd_list[0].dist;
      wgt_lcl->wgt=1.0;

      if( mem_lst[thr_idx].blk_nbr * NCO_VRL_BLOCKSIZE <  mem_lst[thr_idx].pl_cnt +1 )
        mem_lst[thr_idx].wgt_lst= (wgt_sct**)nco_realloc(mem_lst[thr_idx].wgt_lst, sizeof(wgt_sct*) * ++mem_lst[thr_idx].blk_nbr * NCO_VRL_BLOCKSIZE );

      mem_lst[thr_idx].wgt_lst[mem_lst[thr_idx].pl_cnt++] =wgt_lcl;

      if (nco_dbg_lvl_get() >= nco_dbg_dev)
        (void)fprintf(fp_stderr,"%s:%s: singleton  x_ctr=%f  y_ctr=%f\n", nco_prg_nm_get(), fnc_nm, pl_lst_out[idx]->dp_x_ctr, pl_lst_out[idx]->dp_y_ctr );

    }else{

      /* check for duplicates in first nbr_nni by sorting again with ->item  !!!*/
      kd_priority_list_sort(mem_lst[thr_idx].kd_list,nbr_dwe, nbr_dwe,&nbr_dwe_cnt );

      if (nco_dbg_lvl_get() >= nco_dbg_dev && nbr_dwe_cnt < nbr_dwe )
         (void)fprintf(fp_stderr,"%s:%s: nbr_nni_cnt=%d x_ctr=%f  y_ctr=%f\n", nco_prg_nm_get(), fnc_nm, nbr_dwe_cnt, pl_lst_out[idx]->dp_x_ctr, pl_lst_out[idx]->dp_y_ctr );

      /* output at least one */
      for (jdx = 0; jdx < nbr_dwe_cnt; jdx++) {

        pl = (poly_sct *) mem_lst[thr_idx].kd_list[jdx].elem->item;

        /* remember src is in tree and dst is in list */
        wgt_pre[jdx].src_id = pl->src_id;
        wgt_pre[jdx].dst_id = pl_lst_out[idx]->src_id;
        wgt_pre[jdx].area = pl->area;
        wgt_pre[jdx].dist = mem_lst[thr_idx].kd_list[jdx].dist;
        /* use dist squared */
        wgt_pre[jdx].wgt = 1.0 /  pow(wgt_pre[jdx].dist, pow_dwe);
      }

      /* find weights total */
      for (jdx = 0; jdx < nbr_dwe_cnt; jdx++)
        wgt_ttl += wgt_pre[jdx].wgt;

      /* normalize weights */
      for (jdx = 0; jdx < nbr_dwe_cnt; jdx++)
        wgt_pre[jdx].wgt /= wgt_ttl;

      for (jdx = 0; jdx < nbr_dwe_cnt; jdx++) {

        if (wgt_pre[jdx].wgt < min_wgt)
          continue;

        wgt_lcl = (wgt_sct *) nco_malloc(sizeof(wgt_sct));
        *wgt_lcl = wgt_pre[jdx];

        if (mem_lst[thr_idx].blk_nbr * NCO_VRL_BLOCKSIZE < mem_lst[thr_idx].pl_cnt + 1)
          mem_lst[thr_idx].wgt_lst = (wgt_sct **) nco_realloc(mem_lst[thr_idx].wgt_lst,sizeof(wgt_sct *) * ++mem_lst[thr_idx].blk_nbr *NCO_VRL_BLOCKSIZE);

        mem_lst[thr_idx].wgt_lst[mem_lst[thr_idx].pl_cnt++] = wgt_lcl;

        //(void)fprintf(stderr,"%s: weight(%lu)=%f ",fnc_nm, mem_lst[thr_idx].pl_cnt, wgt_lcl->wgt);

      } /* end jdx */
    }

    /* output some usefull tracking stuff - not debug but informative */
    if (  ++mem_lst[thr_idx].idx_cnt % thr_quota_step == 0 && nco_dbg_lvl_get() >=3   )
      (void)fprintf(fp_stderr, "%s: thread %d  has processed %2.2f%% (%ld) of src cells quota and output %ld overlap cells\n",    nco_prg_nm_get(), thr_idx, (float)mem_lst[thr_idx].idx_cnt/(float)thr_quota *100.0,  mem_lst[thr_idx].idx_cnt, mem_lst[thr_idx].pl_cnt  );
  }  /* end idx */

  nco_mem_lst_cat(mem_lst, lcl_thr_nbr);

  /* free up kd_list's */
  for(idx=0;idx<lcl_thr_nbr;idx++)
    mem_lst[idx].kd_list= (KDPriority*) nco_free(mem_lst[idx].kd_list);

  wgt_lst_dwe=mem_lst[0].wgt_lst;

  *wgt_cnt_bln_ret=mem_lst[0].pl_cnt;

  mem_lst=(omp_mem_sct*)nco_free(mem_lst);

  return wgt_lst_dwe;
} /* !nco_poly_lst_mk_dwe_sph() */

void nco_poly_lst_ctr_add(
poly_sct **pl_lst,
int pl_cnt,
int ctr_typ)
{
  int idx;

  double pControl[NBR_SPH];

  for(idx=0;idx<pl_cnt;idx++)
  {
    if(pl_lst[idx]->crn_nbr <3 || pl_lst[idx]->area==0.0  )
      continue;

    if(ctr_typ==1){
      nco_sph_inside_mk(pl_lst[idx], pControl);
      pl_lst[idx]->dp_x_ctr=R2D(pControl[3]);
      pl_lst[idx]->dp_y_ctr=R2D(pControl[4]);
    }
  }
  return;
} /* !nco_poly_lst_ctr_add() */

void
nco_mem_lst_cat(
omp_mem_sct *mem_lst,
int sz_lst)
{
  int idx;
  int i_typ;
  size_t tot_cnt = 0;

  if(mem_lst->wgt_lst)
    i_typ = 1;
  else if(mem_lst->pl_lst)
    i_typ = 2;
  else
    i_typ=0;

  /* quick return if list empty */
  if(!i_typ)
    return;

  /* find total size of lists */
  for (idx = 0; idx < sz_lst; idx++)
    tot_cnt += mem_lst[idx].pl_cnt;

  if(!tot_cnt)
    return;

  else if( i_typ==1 ){

    wgt_sct **tmp_wgt_lst = NULL;

    tmp_wgt_lst = mem_lst[0].wgt_lst = (wgt_sct **) nco_realloc(mem_lst[0].wgt_lst, sizeof(wgt_sct *) * tot_cnt);

    tmp_wgt_lst += mem_lst[0].pl_cnt;

    for (idx = 1; idx < sz_lst; idx++) {
      if (mem_lst[idx].wgt_lst) {
        memcpy(tmp_wgt_lst, mem_lst[idx].wgt_lst, sizeof(wgt_sct *) * mem_lst[idx].pl_cnt);
        tmp_wgt_lst += mem_lst[idx].pl_cnt;
        /* free up list */
        mem_lst[idx].wgt_lst = (wgt_sct **) nco_free(mem_lst[idx].wgt_lst);
      }
    }
  }

  else if(i_typ==2) {
    poly_sct **tmp_ply_lst = NULL;

    tmp_ply_lst = mem_lst[0].pl_lst = (poly_sct **) nco_realloc(mem_lst[0].pl_lst, sizeof(poly_sct *) * tot_cnt);

    tmp_ply_lst += mem_lst[0].pl_cnt;

    for (idx = 1; idx < sz_lst; idx++) {
      if (mem_lst[idx].pl_lst) {
        memcpy(tmp_ply_lst, mem_lst[idx].pl_lst, sizeof(poly_sct *) * mem_lst[idx].pl_cnt);
        tmp_ply_lst += mem_lst[idx].pl_cnt;
        /* free up list */
        mem_lst[idx].pl_lst = (poly_sct **) nco_free(mem_lst[idx].pl_lst);
      }
    }
  }

  /* update first list with total */
  mem_lst[0].pl_cnt=tot_cnt;

  return;
} /* !nco_mem_lst_cat() */

