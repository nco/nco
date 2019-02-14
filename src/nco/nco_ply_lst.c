
#include "nco_ply_lst.h"


/************************ functions that manipulate lists of polygons ****************************************************/
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




void
nco_poly_re_org_lst_old(  /* for each poly_sct*  in list re-order points so that first point is the leftermost point */
poly_sct **pl_lst,
int arr_nbr)
{
  int idx=0;
  int jdx=0;
  int max_crn_nbr=0;

  double *lcl_dp_x;
  double *lcl_dp_y;


  if( arr_nbr==0)
    return;

  /* max crn_nbr */
  for(idx=0 ; idx<arr_nbr ;idx++)
    if( pl_lst[idx]->crn_nbr > max_crn_nbr )
      max_crn_nbr = pl_lst[idx]->crn_nbr;

  lcl_dp_x=(double*)nco_calloc(max_crn_nbr, sizeof(double));
  lcl_dp_y=(double*)nco_calloc(max_crn_nbr, sizeof(double));


  for(idx=0; idx<arr_nbr;idx++)
    nco_poly_re_org(pl_lst[idx], lcl_dp_x, lcl_dp_y);



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

  int idx=0;
  int idx_cnt=0;
  int cnt_wrp_good=0;

  char *fnc_nm="nco_poly_lst_mk()";

  nco_bool bwrp;

  double lon_wdt;
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
    if( msk[idx]==0 || area[idx] == 0.0d)
      continue;


    pl=nco_poly_init_lst(pl_typ, grd_crn_nbr,0, lon_ptr, lat_ptr);
    lon_ptr+=(size_t)grd_crn_nbr;
    lat_ptr+=(size_t)grd_crn_nbr;

    /* if poly is less  than a triangle then  null is returned*/
    if(!pl)
      continue;


    /* add min max */
    nco_poly_add_minmax(pl);

    nco_poly_re_org(pl, lcl_dp_x, lcl_dp_y);


    if(pl->dp_x_minmax[0] <0.0 || (pl->dp_x_minmax[1] - pl->dp_x_minmax[0]) > 30  )
    {
      pl=nco_poly_free(pl);
      continue;

    }

    fprintf(stdout,"/***** input polygon pl lon center=%f   convex=%s\n    ********************/\n", lon_ctr[idx],   (nco_poly_is_convex(pl) ? "True": "False") );
    nco_poly_prn(pl, 0);



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
        (void)fprintf(stdout, "%s: split wrapping didnt work on this polygon(%d)\n", nco_prg_nm_get(), idx );
        (void)fprintf(stdout, "/********************************/\n");
      }

      pl=nco_poly_free(pl);
    }


  }

  if(nco_dbg_lvl_get() >=  nco_dbg_std )
    (void)fprintf(stdout, "%s:%s: size input list(%d), size output list(%d), num of split polygons(%d)\n", nco_prg_nm_get(),fnc_nm, grd_sz, idx_cnt, cnt_wrp_good, cnt_wrp_good);

  pl_lst=(poly_sct**)nco_realloc( pl_lst, (size_t)idx_cnt * sizeof (poly_sct*) );

  *pl_nbr=idx_cnt;

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
nco_poly_lst_mk_vrl(   /* create overlap mesh  for crt polygons */
poly_sct **pl_lst_in,
int pl_cnt_in,
poly_sct **pl_lst_out,
int pl_cnt_out,
int *pl_cnt_vrl_ret){

/* just duplicate output list to overlap */

  size_t idx;
  size_t jdx;
  int sz;
  int max_nbr_vrl=1000;
  int pl_cnt_vrl=0;

  char *chr_ptr;
  char fnc_nm[]="nco_poly_mk_vrl()";

  /* buffers  used in nco-poly_re_org() */
  double lcl_dp_x[VP_MAX]={0};
  double lcl_dp_y[VP_MAX]={0};


  kd_box size;

  poly_sct ** pl_lst_vrl=NULL_CEWI;

  KDElem *my_elem;
  KDTree *rtree;

  KDPriority *list;

  list = (KDPriority *)nco_calloc(sizeof(KDPriority),(size_t)max_nbr_vrl);

  printf("INFO - entered function nco_poly_mk_vrl\n");

  /* create kd_tree from output polygons */
  rtree=kd_create();

  /* populate kd_tree */
  for(idx=0 ; idx<pl_cnt_out;idx++){


    my_elem=(KDElem*)nco_calloc((size_t)1,sizeof (KDElem) );

    size[KD_LEFT]  =  pl_lst_out[idx]->dp_x_minmax[0];
    size[KD_RIGHT] =  pl_lst_out[idx]->dp_x_minmax[1];

    size[KD_BOTTOM] = pl_lst_out[idx]->dp_y_minmax[0];
    size[KD_TOP]    = pl_lst_out[idx]->dp_y_minmax[1];

    //chr_ptr=(char*)pl_lst_out[idx];

    kd_insert(rtree, (kd_generic)pl_lst_out[idx], size, (char*)my_elem);

  }

  /* rebuild tree for faster access */
  kd_rebuild(rtree);
  kd_rebuild(rtree);


  /* kd_print(rtree); */

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

    cnt_vrl=kd_nearest_intersect(rtree, size, max_nbr_vrl,list );


    /* nco_poly_prn(2, pl_lst_in[idx] ); */


    for(jdx=0; jdx <cnt_vrl ;jdx++){

      poly_sct *pl_vrl=(poly_sct*)NULL_CEWI;
      poly_sct *pl_out=(poly_sct*)list[jdx].elem->item;           ;


      // nco_poly_prn(2, pl_out);

      /* check for polygon in polygon first */
      if( nco_poly_poly_in_poly(pl_lst_in[idx], pl_out) == pl_out->crn_nbr )
      {
        //fprintf(stdout,"%s: using poly_in_poly()\n", fnc_nm);
        pl_vrl=nco_poly_dpl(pl_out);
      }
      else
        pl_vrl=nco_poly_do_vrl(pl_lst_in[idx], pl_out);

      if(pl_vrl){
        nco_poly_re_org(pl_vrl, lcl_dp_x, lcl_dp_y);
        pl_lst_vrl=(poly_sct**)nco_realloc(pl_lst_vrl, sizeof(poly_sct*) * (pl_cnt_vrl+1));
        pl_lst_vrl[pl_cnt_vrl]=pl_vrl;
        pl_cnt_vrl++;
        cnt_vrl_on++;

        if(nco_poly_is_convex(pl_vrl) == False )
        {
          fprintf(stdout,"%s:%s vrl polygon convex=0  vrl ,in convex=%d ,out convex=%d\n", nco_prg_nm_get(), fnc_nm, nco_poly_is_convex(pl_lst_in[idx]), nco_poly_is_convex(pl_out) );
          nco_poly_prn(pl_vrl, 2);
          nco_poly_prn(pl_lst_in[idx], 2);
          nco_poly_prn(pl_out, 2);

        }

        //fprintf(stdout,"Overlap polygon to follow\n");
        //nco_poly_prn(2, pl_vrl);

      }


    }

    if( nco_dbg_lvl_get() >= nco_dbg_dev )
      (void) fprintf(stdout, "%s: total overlaps=%d for polygon %d - potential overlaps=%d actual overlaps=%d\n", nco_prg_nm_get(), pl_cnt_vrl,  idx, cnt_vrl, cnt_vrl_on);


  }


  kd_destroy(rtree,NULL);

  list = (KDPriority *)nco_free(list);

  /* return size of list */
  *pl_cnt_vrl_ret=pl_cnt_vrl;


  return pl_lst_vrl;

}

poly_sct **
nco_poly_lst_mk_vrl_sph(  /* create overlap mesh  for sph polygons */
poly_sct **pl_lst_in,
int pl_cnt_in,
poly_sct **pl_lst_out,
int pl_cnt_out,
int *pl_cnt_vrl_ret){

/* just duplicate output list to overlap */

  size_t idx;
  size_t jdx;
  int sz;
  int max_nbr_vrl=1000;
  int pl_cnt_vrl=0;

  char *chr_ptr;
  char fnc_nm[]="nco_poly_mk_vrl()";

  /* buffers  used in nco-poly_re_org() */
  double lcl_dp_x[VP_MAX]={0};
  double lcl_dp_y[VP_MAX]={0};


  kd_box size;

  poly_sct ** pl_lst_vrl=NULL_CEWI;

  KDElem *my_elem;
  KDTree *rtree;

  KDPriority *list;

  list = (KDPriority *)nco_calloc(sizeof(KDPriority),(size_t)max_nbr_vrl);

  printf("INFO - entered function nco_poly_mk_vrl\n");

  /* create kd_tree from output polygons */
  rtree=kd_create();

  /* populate kd_tree */
  for(idx=0 ; idx<pl_cnt_out;idx++){


    my_elem=(KDElem*)nco_calloc((size_t)1,sizeof (KDElem) );

    size[KD_LEFT]  =  pl_lst_out[idx]->dp_x_minmax[0];
    size[KD_RIGHT] =  pl_lst_out[idx]->dp_x_minmax[1];

    size[KD_BOTTOM] = pl_lst_out[idx]->dp_y_minmax[0];
    size[KD_TOP]    = pl_lst_out[idx]->dp_y_minmax[1];

    //chr_ptr=(char*)pl_lst_out[idx];

    kd_insert(rtree, (kd_generic)pl_lst_out[idx], size, (char*)my_elem);

  }

  /* rebuild tree for faster access */
  kd_rebuild(rtree);
  kd_rebuild(rtree);


  /* kd_print(rtree); */

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

    cnt_vrl=kd_nearest_intersect(rtree, size, max_nbr_vrl,list );


    /* nco_poly_prn(2, pl_lst_in[idx] ); */


    for(jdx=0; jdx <cnt_vrl ;jdx++){

      poly_sct *pl_vrl=(poly_sct*)NULL_CEWI;
      poly_sct *pl_out=(poly_sct*)list[jdx].elem->item;           ;

      if(pl_lst_in[idx]->pl_typ != pl_out->pl_typ)
      {
        fprintf(stdout,"poly type mismatch\n", fnc_nm);
        continue;
      }

      pl_vrl=nco_poly_do_vrl(pl_lst_in[idx], pl_out);

      if(pl_vrl){
        // nco_poly_re_org(pl_vrl, lcl_dp_x, lcl_dp_y);
        pl_lst_vrl=(poly_sct**)nco_realloc(pl_lst_vrl, sizeof(poly_sct*) * (pl_cnt_vrl+1));
        pl_lst_vrl[pl_cnt_vrl]=pl_vrl;
        pl_cnt_vrl++;
        cnt_vrl_on++;

        if(0 && nco_poly_is_convex(pl_vrl) == False )
        {
          fprintf(stdout,"%s:%s vrl polygon convex=0  vrl ,in convex=%d ,out convex=%d\n", nco_prg_nm_get(), fnc_nm, nco_poly_is_convex(pl_lst_in[idx]), nco_poly_is_convex(pl_out) );
          nco_poly_prn(pl_vrl, 2);
          nco_poly_prn(pl_lst_in[idx], 2);
          nco_poly_prn(pl_out, 2);

        }

        //fprintf(stdout,"Overlap polygon to follow\n");
        //nco_poly_prn(2, pl_vrl);

      }


    }

    if( nco_dbg_lvl_get() >= nco_dbg_dev )
      (void) fprintf(stdout, "%s: total overlaps=%d for polygon %d - potential overlaps=%d actual overlaps=%d\n", nco_prg_nm_get(), pl_cnt_vrl,  idx, cnt_vrl, cnt_vrl_on);


  }


  kd_destroy(rtree,NULL);

  list = (KDPriority *)nco_free(list);

  /* return size of list */
  *pl_cnt_vrl_ret=pl_cnt_vrl;


  return pl_lst_vrl;

}

