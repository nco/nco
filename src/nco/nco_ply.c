#include "nco_ply.h"

#include "nco_crt.c"
#include "nco_sph.c"

int
nco_poly_typ_sz(poly_typ_enm pl_typ)
{
  if(pl_typ == poly_crt)
    return NBR_CRT;
  else if(pl_typ == poly_sph)
    return NBR_SPH;
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

   return (poly_sct*)NULL;

}  


poly_sct *   
nco_poly_init
(void)
{


  poly_sct *pl;

  pl->pl_typ=poly_none;

  pl=(poly_sct*)nco_malloc( sizeof(poly_sct));

  pl->dp_x=(double*)NULL_CEWI;
  pl->dp_y=(double*)NULL_CEWI;
  pl->dp_xyz=(double*)NULL_CEWI;
  pl->shp=(double**)NULL_CEWI;


  pl->dp_x_minmax[0]=0.0;
  pl->dp_x_minmax[1]=0.0;

  pl->dp_y_minmax[0]=0.0;
  pl->dp_y_minmax[1]=0.0;

  
  pl->stat=0;
  pl->area=0.0;
  pl->crn_nbr=0;
  pl->mem_flg=0;

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


  pl_cpy->stat=pl->stat;
  pl_cpy->area=pl->area;
  pl_cpy->crn_nbr=crn_nbr_in;

  /* mem flag is ALWAYS 0 for a copy  */
  pl_cpy->mem_flg=0;

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
  
poly_sct *
nco_poly_init_crn
(poly_typ_enm pl_typ,
int crn_nbr_in)
{
  poly_sct *pl;

  pl=nco_poly_init();

  pl->pl_typ=pl_typ;

  pl->crn_nbr=crn_nbr_in;

  pl->dp_x=(double*)nco_calloc((size_t)crn_nbr_in, sizeof(double));
  pl->dp_y=(double*)nco_calloc((size_t)crn_nbr_in, sizeof(double));

  pl->mem_flg=0;
  
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
      pl->dp_x[idx]=0.0d;
      pl->dp_y[idx]=0.0d;
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
 double *dp_x_in,
 double *dp_y_in)
{
 int idx;
 int sz;

 poly_sct *pl;


 /* less than a triangle */
 if (arr_nbr <3 )
   return (poly_sct*)NULL_CEWI;   


 /* check repeated points at end of arrray - nb must be an exact match */
 for(idx=1; idx<arr_nbr; idx++ )
   if( dp_x_in[idx] == dp_x_in[idx-1] && dp_y_in[idx] == dp_y_in[idx-1] )
     break;

 if(idx < 3 )
     return (poly_sct*)NULL_CEWI;   

 /* we have at least a triangle */ 

 
 /* dont free  pointers */
 if( mem_flg)
 {

   pl=nco_poly_init();
   pl->pl_typ=pl_typ;

   pl->mem_flg=1;
   pl->crn_nbr=idx;
 
   pl->dp_x=dp_x_in;
   pl->dp_y=dp_y_in;
 
 }
 else
 {
   pl=nco_poly_init_crn(pl_typ, idx);
   memcpy(pl->dp_x, dp_x_in, sizeof(double) *idx);
   memcpy(pl->dp_y, dp_y_in, sizeof(double) *idx);   
   
 }    
 
 return pl;
 

}  

void nco_poly_add_minmax
(poly_sct *pl)
{  
  
  int idx;
  int sz;


  sz=pl->crn_nbr; 
  
  pl->dp_x_minmax[0]=DBL_MAX;
  pl->dp_x_minmax[1]=-DBL_MAX;

  pl->dp_y_minmax[0]=DBL_MAX;
  pl->dp_y_minmax[1]=-DBL_MAX;


  
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

  /* add correction to latitude bounding box */
  if(pl->pl_typ == poly_sph)
  {
    double lat_min;
    double lat_max;
    nco_bool bDeg=1;

    /* do it in degrees for now */

    nco_geo_get_lat_correct(pl->dp_x_minmax[0], pl->dp_y_minmax[1], pl->dp_x_minmax[1], pl->dp_y_minmax[0], &lat_min,
                            &lat_max, bDeg);

    pl->dp_y_minmax[0]=lat_min;
    pl->dp_y_minmax[1]=lat_max;



  }



  return; 
  

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
nco_poly_pnt_in_poly( 
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
	bret = (  lcl_dp_x[idx]<=0.0 &&  lcl_dp_x[idx1] >=0.0 ||  lcl_dp_x[idx]>=0.0 && lcl_dp_x[idx1]<=0.0  ); 
      else
        bret = (  lcl_dp_y[idx]<=0.0 &&  lcl_dp_y[idx1] >=0.0 ||  lcl_dp_y[idx]>=0.0 && lcl_dp_y[idx1]<=0.0  ); 	  

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

int             /* O [nbr] returns number of points of pl_out that are inside pl_in */
nco_poly_poly_in_poly( 
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

    if( nco_poly_pnt_in_poly(pl_in->crn_nbr, pl_out->dp_x[idx], pl_out->dp_y[idx], lcl_dp_x, lcl_dp_y)  )
      cnt_in++;
  } 
  lcl_dp_x=(double*)nco_free(lcl_dp_x);
  lcl_dp_y=(double*)nco_free(lcl_dp_y);
  

  return cnt_in;
}


void
nco_poly_prn
(poly_sct *pl, int style)
{
  int idx;
  int nbr_el;

  nbr_el=nco_poly_typ_sz(pl->pl_typ);

  switch(style){ 

    case 0:
      (void)fprintf(stdout,"\n%s: pl_typ=%d, crn_nbr=%d stat=%d mem_flg=%d area=%f\n", nco_prg_nm_get(),pl->pl_typ, pl->crn_nbr, pl->stat, pl->mem_flg, pl->area);
      (void)fprintf(stdout,"dp_x ");
      for(idx=0; idx<pl->crn_nbr; idx++)
	(void)fprintf(stdout,"%20.14f, ",pl->dp_x[idx]);
      (void)fprintf(stdout,"\n");		  

      (void)fprintf(stdout,"dp_y ");
      for(idx=0; idx<pl->crn_nbr; idx++)
	(void)fprintf(stdout,"%20.14f, ",pl->dp_y[idx]);
      (void)fprintf(stdout,"\n");

      (void)fprintf(stdout,"min/max x( %g, %g) y(%g %g)\n", pl->dp_x_minmax[0], pl->dp_x_minmax[1], pl->dp_y_minmax[0], pl->dp_y_minmax[1]);       
      
      break;

   case 1:  
   default:
     (void)fprintf(stdout,"%s: crn_nbr=%d\n", nco_prg_nm_get(), pl->crn_nbr);
     
     for(idx=0; idx<pl->crn_nbr; idx++)
        (void)fprintf(stdout,"{ %20.14f, %20.14f }\n",pl->dp_x[idx], pl->dp_y[idx]);

     break;

   case 2:  
     (void)fprintf(stdout,"%s: crn_nbr=%d\n", nco_prg_nm_get(), pl->crn_nbr);
     
     for(idx=0; idx<pl->crn_nbr; idx++)
        (void)fprintf(stdout,"%20.16f %20.16f\n",pl->dp_x[idx], pl->dp_y[idx]);

     break;


    case 3:
      (void)fprintf(stdout,"%s: crn_nbr=%d shp follows \n", nco_prg_nm_get(), pl->crn_nbr);

      if(pl->pl_typ == poly_sph)
        for(idx=0; idx<pl->crn_nbr; idx++)
           (void)fprintf(stdout,"x=%f y=%f z=%f lon=%f lat=%f\n",pl->shp[idx][0], pl->shp[idx][1], pl->shp[idx][2], pl->shp[idx][3]*180.0 / M_PI, pl->shp[idx][4]*180.0 /M_PI );

      if(pl->pl_typ == poly_crt)
        for(idx=0; idx<pl->crn_nbr; idx++)
          (void)fprintf(stdout,"x=%f y=%f\n",pl->shp[idx][0], pl->shp[idx][1]);


  }




  return;
     
}



poly_sct*
nco_poly_do_vrl(
poly_sct *pl_in,
poly_sct *pl_out){

  
 int iret=0;

 int nbr_p=pl_in->crn_nbr;
 int nbr_q=pl_out->crn_nbr;
 int nbr_r=0;


 char fnc_nm[]="nco_poly_do_vrl()";
  
 poly_sct *pl_vrl;

 nco_poly_shp_pop(pl_in);
 nco_poly_shp_pop(pl_out);


 pl_vrl=nco_poly_init_crn(pl_in->pl_typ,  ( nbr_p>=nbr_q ? 2*nbr_p: 2*nbr_q ) +1       );
 nco_poly_shp_init(pl_vrl);



 if(pl_in->pl_typ == poly_crt ) {
   iret = nco_crt_intersect(pl_in, pl_out, pl_vrl, &nbr_r);
 }
   else if( pl_in->pl_typ== poly_sph)
 {
   iret = nco_sph_intersect(pl_in, pl_out, pl_vrl, &nbr_r);

 }

   nco_poly_init_crn_re(pl_vrl, nbr_r);
   nco_poly_dp_pop_shp(pl_vrl);

  if (nbr_r < 3)
    pl_vrl = nco_poly_free(pl_vrl);


 return pl_vrl;
  
}

void
nco_poly_use_minmax_crn /* use the values of minmax box as dp_x, dp_y  */
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

  nco_poly_add_minmax(pl_in);     
  
  if( cnt_right == pl_in->crn_nbr || cnt_right==0 ) 
  {
    pl_in=nco_poly_free(pl_in);   
    return NCO_ERR;
  }
 
  
  /*  create right intersection polygon */
  pl_bnds=nco_poly_init_crn(pl->pl_typ, 4);

  pl_bnds->dp_x_minmax[0]=180.0;
  pl_bnds->dp_x_minmax[1]=pl_in->dp_x_minmax[1];
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_use_minmax_crn(pl_bnds);

  /* do overlap */
  *pl_wrp_left=nco_poly_do_vrl(pl_in, pl_bnds);

  /* must subtract  back the 360.0 we subtracted earlier */ 
  if(*pl_wrp_left){
    
    for(idx=0;idx< (*pl_wrp_left)->crn_nbr;idx++)
      (*pl_wrp_left)->dp_x[idx]-=360.0;

    
    nco_poly_add_minmax(*pl_wrp_left);

    
    
  }

  /* now create bound for right polygon */
  pl_bnds->dp_x_minmax[0]=pl_in->dp_x_minmax[0];
  pl_bnds->dp_x_minmax[1]=180.0;
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_use_minmax_crn(pl_bnds);
  
  /* do overlap */
  *pl_wrp_right=nco_poly_do_vrl(pl_in, pl_bnds);

  if(*pl_wrp_right)
  {
    
     nco_poly_add_minmax(*pl_wrp_right);
     

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

  nco_poly_add_minmax(pl_in);     
  
  if( cnt_left == pl_in->crn_nbr || cnt_left==0 ) 
  {
    pl_in=nco_poly_free(pl_in);   
    return NCO_ERR;
  }
 
  
  /*  create left intersection polygon */
  pl_bnds=nco_poly_init_crn(pl->pl_typ, 4);

  pl_bnds->dp_x_minmax[0]=pl_in->dp_x_minmax[0];
  pl_bnds->dp_x_minmax[1]=-1.0e-13;
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_use_minmax_crn(pl_bnds);

  /* do overlap */
  *pl_wrp_left=nco_poly_do_vrl(pl_in, pl_bnds);

  /* must add back the 360.0 we subtracted earlier */ 
  if(*pl_wrp_left){
    
    for(idx=0;idx< (*pl_wrp_left)->crn_nbr;idx++)
      (*pl_wrp_left)->dp_x[idx]+=360.0;

    
    nco_poly_add_minmax(*pl_wrp_left);

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

  nco_poly_use_minmax_crn(pl_bnds);
  
  /* do overlap */
  *pl_wrp_right=nco_poly_do_vrl(pl_in, pl_bnds);

  if(*pl_wrp_right)
  {
    
     nco_poly_add_minmax(*pl_wrp_right);
     
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

  int idx;
  int cnt_left=0;

  poly_sct *pl_in;
  poly_sct *pl_bnds;



  if( grd_lon_typ ==  nco_grd_lon_Grn_wst || grd_lon_typ == nco_grd_lon_Grn_ctr)
    return nco_poly_wrp_splt360(pl, pl_wrp_left, pl_wrp_right); 

  else if( grd_lon_typ ==  nco_grd_lon_180_wst || grd_lon_typ == nco_grd_lon_180_ctr)
    return nco_poly_wrp_splt180(pl, pl_wrp_left, pl_wrp_right); 

  else
    return NCO_ERR;
  
  
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

  nco_poly_add_minmax(pl_in);     
  
  if( cnt_left == pl_in->crn_nbr || cnt_left==0 ) 
  {
    pl_in=nco_poly_free(pl_in);   
    return NCO_ERR;
  }
 
  
  /*  create left intersection polygon */
  pl_bnds=nco_poly_init_crn(pl->pl_typ, 4);

  pl_bnds->dp_x_minmax[0]=pl_in->dp_x_minmax[0];
  pl_bnds->dp_x_minmax[1]=-1.0e-13;
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_use_minmax_crn(pl_bnds);

  /* do overlap */
  *pl_wrp_left=nco_poly_do_vrl(pl_in, pl_bnds);

  /* must add back the 360.0 we subtracted earlier */ 
  if(*pl_wrp_left){
    
    for(idx=0;idx< (*pl_wrp_left)->crn_nbr;idx++)
      (*pl_wrp_left)->dp_x[idx]+=360.0;

    
    nco_poly_add_minmax(*pl_wrp_left);

    /* check overall-extent */
    if(  (*pl_wrp_left)->dp_x_minmax[1] - (*pl_wrp_left)->dp_x_minmax[0] > CELL_LONGITUDE_MAX )
      *pl_wrp_left=nco_poly_free(*pl_wrp_left);
    
    
  }

  /* now create bound for right polygon */
  pl_bnds->dp_x_minmax[0]=0.0;
  pl_bnds->dp_x_minmax[1]=pl_in->dp_x_minmax[1];
  pl_bnds->dp_y_minmax[0]=pl_in->dp_y_minmax[0];
  pl_bnds->dp_y_minmax[1]=pl_in->dp_y_minmax[1];

  nco_poly_use_minmax_crn(pl_bnds);
  
  /* do overlap */
  *pl_wrp_right=nco_poly_do_vrl(pl_in, pl_bnds);

  if(*pl_wrp_right)
  {
    
     nco_poly_add_minmax(*pl_wrp_right);
     
    /* check overall-extent */
    if(  (*pl_wrp_right)->dp_x_minmax[1] - (*pl_wrp_right)->dp_x_minmax[0] > CELL_LONGITUDE_MAX )
      *pl_wrp_right=nco_poly_free(*pl_wrp_right);

  }
     
  

  pl_in=nco_poly_free(pl_in);
  pl_bnds=nco_poly_free(pl_bnds);



  if( *pl_wrp_left ||  *pl_wrp_right )
    return NCO_NOERR;
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
  double x_min=DBL_MAX;

    
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
    pl->shp[idx]= (double*)nco_free(pl->shp[idx]);

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


  if( pl->pl_typ == poly_sph )
  {
    for (idx = 0; idx < pl->crn_nbr; idx++)
      nco_geo_lonlat_2_sph(pl->dp_x[idx], pl->dp_y[idx] , pl->shp[idx]);

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
  if( pl->pl_typ == poly_sph )
  {
    for (idx = 0; idx < pl->crn_nbr; idx++)
      nco_geo_sph_2_lonlat(pl->shp[idx], &pl->dp_x[idx], &pl->dp_y[idx], bDeg);


  }



}










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


      if(pl->dp_x_minmax[0] <0.0 || (pl->dp_x_minmax[1] - pl->dp_x_minmax[1]) > 30  )
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

