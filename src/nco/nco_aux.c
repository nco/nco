/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_aux.c,v 1.28 2011-01-01 19:16:52 zender Exp $ */

/* Copyright (C) 1995--2011 Charlie Zender
   License: GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Purpose: Support hyperslabbing cell-based grids over coordinate ranges 
   This works on datasets that contain CF-convention auxiliary coordinate variables.
   Such datasets contain variables with standard_name's "latitude" and "longitude". 
   Cells that contain a value within the user-requested range are considered a match.
   Could be useful to look at the CF bounds variable instead but harder.
   
   Author: Karen Schuchardt
   
   Example usage:
   ncks -X 0.,45.,0.,90. -X 180.,225.,-90.,0. ~/nco/data/in.nc ~/foo.nc */

#include "nco_aux.h" /* Auxiliary coordinates */

int 
nco_find_lat_lon
(int nc_id,
 char var_nm_lat[], 
 char var_nm_lon[], 
 char **units,
 int *lat_id,
 int *lon_id,
 nc_type *crd_typ
 ){
  /* Purpose: Find auxiliary coordinate variables that map to latitude/longitude 
     Find variables with standard_name = "latitude" and "longitude"
     Return true if both latitude and longitude standard names are found
     Also return needed information about these auxiliary coordinates
     Assumes that units and types for latitude and longitude are identical
     Caller responsible for memory management for variable names
     Memory for unit strings must be freed by caller */
  
  int idx;
  long lenp;
  char name[NC_MAX_NAME+1];
  int nvars=0;
  int rcd=NC_NOERR;
  int ret=0;
  char value[NC_MAX_NAME+1];
  int var_dimids[NC_MAX_VAR_DIMS];  /* dimension ids */
  int var_natts;             /* number of attributes */
  int var_ndims;                      /* number of dims */
  nc_type var_type;                    /* variable type */
  
  /* Make sure CF tag exists. Currently require CF-1.0 value */
  if(NCO_GET_ATT_CHAR(nc_id,NC_GLOBAL,"Conventions",value) || !strstr(value,"CF-1.0")){
    (void)fprintf(stderr,"%s: WARNING nco_find_lat_lon() reports file \"Convention\" attribute is missing or not equal to \"CF-1.0\". Auxiliary coordinate support (i.e., the -X option) cannot be expected to behave well file does not support CF-1.0 metadata conventions. Continuing anyway...\n",prg_nm_get());
  } /* !CF */
  
  /* Get number of variables */
  rcd=nco_inq_nvars(nc_id,&nvars);
  
  /* For each variable, see if standard name is latitude or longitude */
  for(idx=0;idx<nvars && ret<2;idx++){
    nco_inq_var(nc_id,idx,name,&var_type,&var_ndims,var_dimids,&var_natts);
    lenp=0;
    if(!nco_inq_attlen_flg(nc_id,idx,"standard_name",&lenp)){
      NCO_GET_ATT_CHAR(nc_id,idx,"standard_name",value);
      value[lenp]='\0';
      if(strcmp(value,"latitude") == 0){
	strcpy(var_nm_lat,name);
	*lat_id=idx;
	
	/* Get units; assume same for both lat and lon */
	rcd=nco_inq_attlen(nc_id,idx,"units",&lenp);
	if(rcd != NC_NOERR) nco_err_exit(-1,"nco_aux_evl: CF convention requires \"latitude\" to have units attribute\n");
	*units=(char *)nco_malloc((lenp+1L)*sizeof(char *));
	NCO_GET_ATT_CHAR(nc_id,idx,"units",*units);
	units[lenp]='\0';
	
	if(var_ndims > 1) (void)fprintf(stderr,"%s: WARNING nco_aux_evl() reports latitude variable %s has %d dimensions. NCO only supports hyperslabbing of auxiliary coordinate variables with a single dimension. Continuing with unpredictable results...\n",prg_nm_get(),name,var_ndims);

	/* Assign type; assumed same for both lat and lon */
	*crd_typ=var_type;
	ret++;
      } /* end if var is lattitude */
      
      if(strcmp(value,"longitude") == 0){
	strcpy(var_nm_lon,name);
	*lon_id=idx;
	ret++;
      } /* end if var is longitude */
      
    } /* end if standard_name */
    
  } /* end loop over vars */
  
  return ret == 2;  // True if both found
} /* nco_find_lat_lon */

int  /* status code */
nco_get_dmn_info
(int nc_id,
 int varid,
 char dimname[],
 int *dimid,
 long *dmn_sz)
{
  /* Purpose: Get dimension information associated with specified variable
     In our case, this is lat or lon---they are presumed to be identical */
  
  int rcd=NC_NOERR;
  
  nc_type var_type;                   /* variable type */
  int var_dimids[NC_MAX_VAR_DIMS];    /* dimension ids */
  int var_natts;                      /* number of attributes */
  int var_ndims;                      /* number of dims */
  
  /* Get dimension information */
  rcd=nco_inq_var(nc_id,varid,0,&var_type,&var_ndims,var_dimids,&var_natts);
  if(rcd == NC_NOERR) {
    *dimid=var_dimids[0];
    rcd=nco_inq_dimlen(nc_id,var_dimids[0],dmn_sz);
    rcd=nco_inq_dimname(nc_id,var_dimids[0],dimname);
  }
  return rcd;
} /* nco_get_dmn_info */

lmt_sct **
nco_aux_evl
(int in_id, 
 int aux_nbr, 
 char *aux_arg[],
 int *lmt_nbr
 ){
  /* Purpose: Create lmt structure of slabs of continguous cells that
     match the rectangular region specified with -X options.
     Intended for use with non-monotonic grids
     Requires CF-1.0 conventions
     Uses lat/lon centers rather than cell_bounds to detect matches
     Code assumes units must be degrees if they are not radians */
  
  char *units=NULL; /* fxm TODO nco925: "units" value needs dynamically allocated size in case value exceeds NC_MAX_NAME */
  char bfr[100]; /* buffer for making user-assigned limit names */
  char dmn_nm[NC_MAX_NAME+1];
  char var_nm_lat[NC_MAX_NAME+1];
  char var_nm_lon[NC_MAX_NAME+1];
  
  dmn_sct lat;
  dmn_sct lon;
  
  double clat;              /* current cell lat */
  double clon;               /* current cell lon */

  float lllat;                /* lower left lat of bounding rectangle */
  float lllon;                /* lower left lon of bounding rectangle */
  float urlat;                /* upper right lon of bounding rectangle */
  float urlon;                /* upper right lat of bounding rectangle */
  
  int aux_idx;                /* iterator over user -X options */
  int cll_itr;                 /* cell iterator */
  int cll_nbr_cns=0;           /* current number matching consecutive cells */
  int cll_idx_min=-1;           /* min. index of cell in consecutive cell set */
  int dmn_id=int_CEWI;
  int lat_id;
  int lon_id;
  int rcd=NC_NOERR;
  
  lmt_sct **lmt=0;         /* return structure */
  
  long dmn_sz=0;
  
  nc_type crd_typ;
  
  void *vp_lat;                /* lat coordinate array; float or double only */
  void *vp_lon;                /* lon coordinate array; float or double only */
  
  /* Obtain lat/lon variable names */
  if(!nco_find_lat_lon(in_id,var_nm_lat,var_nm_lon,&units,&lat_id,&lon_id,&crd_typ)){
    nco_err_exit(-1,"nco_aux_evl: Unable to identify lat/lon auxiliary coordinate variables.");
  } /* !nco_find_lat_lon() */
  
  if(nco_get_dmn_info(in_id,lat_id,dmn_nm,&dmn_id,&dmn_sz) != NC_NOERR){
    nco_err_exit(-1,"nco_aux_evl: Unable to get dimension information");
  } /* !nco_get_dmn_info() */
  
  /*printf("coords are: %s %s; units are: %s; %s %ld\n",var_nm_lat,var_nm_lon,units,dmn_nm,dmn_sz); */
  
  /* Load latitude/longitude variables needed to search for region matches */
  lat.type=crd_typ;
  lat.sz=dmn_sz;
  lat.srt=0;
  vp_lat=(void *)nco_malloc(dmn_sz*nco_typ_lng(lat.type));
  lon.type=crd_typ;
  lon.sz=dmn_sz;
  lon.srt=0;
  vp_lon=(void *)nco_malloc(dmn_sz*nco_typ_lng(lon.type));
  rcd=nco_get_vara(in_id,lat_id,&lat.srt,&lat.sz,vp_lat,lat.type);
  if(rcd != NC_NOERR) nco_err_exit(-1,"nco_aux_evl");
  rcd=nco_get_vara(in_id,lon_id,&lon.srt,&lon.sz,vp_lon,lon.type);
  if(rcd != NC_NOERR) nco_err_exit(-1,"nco_aux_evl");
  
  *lmt_nbr=0;
  
  lmt_sct base;
  base.nm=(char *)strdup(dmn_nm);
  base.lmt_typ=lmt_dmn_idx;
  base.is_usr_spc_lmt=1; 
  base.is_usr_spc_min=1; 
  base.is_usr_spc_max=1;
  base.srd_sng=(char *)strdup("1");
  base.is_rec_dmn=0;
  base.id=dmn_id;
  base.min_idx=0;
  base.max_idx=0;
  base.srt=0;
  base.end=0;
  base.cnt=0;
  base.srd=1;
  
  /* malloc() lmt structure to return
     No way to know exact size in advance though maximum is about dim_sz/2 */
  int MAX_DMN=dmn_sz/2;
  
  if(aux_nbr > 0) lmt=(lmt_sct **)nco_malloc(MAX_DMN*sizeof(lmt_sct *));
  
  for(aux_idx=0;aux_idx<aux_nbr;aux_idx++){
    /* Parse into lllong,lllat,urlon,urlon, accounting for units */
    nco_aux_prs(aux_arg[aux_idx],units,&lllon,&lllat,&urlon,&urlat);
    /* printf("Box is %f %f %f %f\n",lllon,lllat,urlon,urlat); */
    
    cll_idx_min=-1;
    cll_nbr_cns=0;
    for(cll_itr=0;cll_itr<dmn_sz;cll_itr++){
      if(lat.type == NC_FLOAT) clat=((float *)vp_lat)[cll_itr]; else clat=((double *)vp_lat)[cll_itr];
      if(lon.type == NC_FLOAT) clon=((float *)vp_lon)[cll_itr]; else clon=((double *)vp_lon)[cll_itr];
      if(clon >= lllon && clon <= urlon &&
	 clat >= lllat && clat <= urlat){
	/*printf("match %lf %lf %lf/ %lf %lf %lf\n",clon,lllon,urlon,clat,lllat,urlat); */
	if(cll_idx_min == -1){
	  cll_idx_min=cll_itr;
	  cll_nbr_cns=1;
	}else if(cll_itr == cll_idx_min + cll_nbr_cns){
	  cll_nbr_cns++;
	}else{
	} /* end found matching cell */
      }else if(cll_idx_min != -1){
	sprintf(bfr,"%d",cll_idx_min);
	base.min_sng=(char *)strdup(bfr);
	base.min_idx=base.srt=cll_idx_min;
	sprintf(bfr,"%d",cll_idx_min+cll_nbr_cns-1);
	base.max_sng=(char *)strdup(bfr);
	base.max_idx=base.end=cll_idx_min+cll_nbr_cns-1;
	base.cnt=cll_nbr_cns;
	(*lmt_nbr)++;
	if(*lmt_nbr > MAX_DMN){
	  /*printf("Number of slabs exceeds allocated mamory %d\n",MAX_DMN);*/
	  nco_err_exit(-1,"nco_aux_evl: Number of slabs exceeds allocated mamory.");
	} /* end if too many slabs */
	lmt[(*lmt_nbr)-1]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
	*lmt[(*lmt_nbr)-1]=base;
	cll_idx_min=-1;
      } /* end if one or more consecutive matching cells */
    } /* end loop over cells */
    
  } /* end loop over user supplied -X options */

  if(units) units=(char *)nco_free(units);
  vp_lat=nco_free(vp_lat);
  vp_lon=nco_free(vp_lon);
  
  /* printf ("returning structure %d\n",*lmt_nbr);
    for (aux_idx=0; aux_idx<(*lmt_nbr); aux_idx++)
    printf("LIMIT %ld %ld \n",lmt[aux_idx]->min_idx,lmt[aux_idx]->max_idx); */
  return lmt;
} /* nco_aux_evl */

void 
nco_aux_prs
(const char *args,
 const char *units,
 float *lllon,
 float *lllat,
 float *urlon,
 float *urlat)
{
  /* Purpose: Parse command-line arguments of the form:
     <min_lon,max_lon,min_lat,max_lat> */
  char *tmpargs;
  char *token;
  
  tmpargs=strdup(args);
  
  sscanf(args,"%f,%f,%f,%f",lllon,urlon,lllat,urlat);
  token=strtok(tmpargs,", ");
  if(token) sscanf(token,"%f",lllon); else nco_err_exit(-1,"nco_aux_prs: please specify four points for the slab");
  token=strtok(NULL,", ");
  if(token) sscanf(token,"%f",urlon); else nco_err_exit(-1,"nco_aux_prs: please specify four points for the slab");
  token=strtok(NULL,", ");
  if(token) sscanf(token,"%f",lllat); else nco_err_exit(-1,"nco_aux_prs: please specify four points for the slab");
  token=strtok(NULL,", ");
  if(token) sscanf(token,"%f",urlat); else nco_err_exit(-1,"nco_aux_prs: please specify four points for the slab");
  
  free(tmpargs);
  
  if(strcmp(units,"radians") == 0){
    /* WIN32 math.h does not define M_PI */
#ifndef M_PI
# define M_PI		3.14159265358979323846
#endif /* M_PI */
    *lllon*=M_PI/180.0;
    *lllat*=M_PI/180.0;
    *urlon*=M_PI/180.0;
    *urlat*=M_PI/180.0;
  }
} /* nco_aux_prs */
