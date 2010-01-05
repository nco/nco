/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_aux.c,v 1.23 2010-01-05 20:02:17 zender Exp $ */

/* Copyright (C) 1995--2010 Charlie Zender
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
 char latvar[], 
 char lonvar[], 
 char **units,
 int *latid,
 int *lonid,
 nc_type *coordtype
 ){
  /* Purpose: Find auxillary coordinate variables that map to latitude/longitude 
     Find variables with standard_name = "latitude" and "longitude"
     Return true if both latitude and longitude standard names are found
     Also return needed information about these auxiliary coordinates
     Assumes that units and types for latitude and longitude are identical
     Caller responsible for memory management for variable names.
     Memory for unit strings must be freed bay caller */
  
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
    nco_err_exit(-1,"nco_aux_evl: CF-1.0 Convention attribute is required for -X option.");
  }
  
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
	strcpy(latvar,name);
	*latid=idx;
	
	/* Get units; assume same for both lat and lon */
	rcd=nco_inq_attlen(nc_id,idx,"units",&lenp);
	if (rcd != NC_NOERR) nco_err_exit(-1,"nco_aux_evl: CF convention requires \"latitude\" to have units attribute\n");
	*units=(char *)nco_malloc((lenp+1L)*sizeof(char *));
	NCO_GET_ATT_CHAR(nc_id,idx,"units",*units);
	units[lenp]='\0';
	
	/* Assign type; assumed same for both lat and lon */
	*coordtype=var_type;
	ret++;
      } /* end if var is lattitude */
      
      if(strcmp(value,"longitude") == 0){
	strcpy(lonvar,name);
	*lonid=idx;
	ret++;
      } /* end if var is longitude */
      
    } /* end if standard_name */
    
  } /* end loop over vars */
  
  return ret == 2;  // true if both found
} /* nco_find_lat_lon */

int  /* status code */
nco_getdmninfo
(int nc_id,
 int varid,
 char dimname[],
 int *dimid,
 long *dmnsz)
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
  if (rcd == NC_NOERR) {
    *dimid=var_dimids[0];
    rcd=nco_inq_dimlen(nc_id,var_dimids[0],dmnsz);
    rcd=nco_inq_dimname(nc_id,var_dimids[0],dimname);
  }
  return rcd;
} /* nco_getdmninfo */

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
  
  char *units=0; /* fxm TODO nco925: "units" value needs dynamically allocated size in case value exceeds NC_MAX_NAME */
  char buf[100]; /* buffer for making user-assigned limit names */
  char dmnname[NC_MAX_NAME+1];
  char latvar[NC_MAX_NAME+1];
  char lonvar[NC_MAX_NAME+1];
  
  dmn_sct lat;
  dmn_sct lon;
  
  double clat;              /* current cell lat */
  
  float clon;               /* current cell lon */
  float lllat;                /* lower left lat of bounding rectangle */
  float lllon;                /* lower left lon of bounding rectangle */
  float urlat;                /* upper right lon of bounding rectangle */
  float urlon;                /* upper right lat of bounding rectangle */
  
  int cell;                 /* cell iterator */
  int consec=0;           /* current number matching consecutive cells */
  int curit;                /* iterator over user -X options */
  int dmnid=int_CEWI;
  int latid;
  int lonid;
  int mincell=-1;           /* min. index of cell in consecutive cell set */
  int rcd=NC_NOERR;
  
  lmt_sct **lmts=0;         /* return structure */
  
  long dmnsz=0;
  
  nc_type coordtype;
  
  void *latvp;                /* lat coordinate array; float or double only */
  void *lonvp;                /* lon coordinate array; float or double only */
  
  /* Obtain lat/lon variable names */
  if(!nco_find_lat_lon(in_id,latvar,lonvar,&units,&latid,&lonid,&coordtype)){
    nco_err_exit(-1,"nco_aux_evl: Unable to indentify lat/lon auxillary coordinate variables.");
  } /* end nco_find_lat_lon fails */
  
  if(nco_getdmninfo(in_id,latid,dmnname,&dmnid,&dmnsz) != NC_NOERR){
    nco_err_exit(-1,"nco_aux_evl: Unable to get dimension information");
  } /* end nco_getdmninfo fails */
  
  /*printf("coords are: %s %s; units are: %s; %s %ld\n",latvar,lonvar,units,dmnname,dmnsz); */
  
  /* load the lat/lon vars needed to search for region matches. */
  lat.type=coordtype;
  lat.sz=dmnsz;
  lat.srt=0;
  latvp=(void *)nco_malloc(dmnsz*nco_typ_lng(lat.type));
  lon.type=coordtype;
  lon.sz=dmnsz;
  lon.srt=0;
  lonvp=(void *)nco_malloc(dmnsz*nco_typ_lng(lon.type));
  rcd=nco_get_vara(in_id,latid,&lat.srt,&lat.sz,latvp,lat.type);
  if (rcd != NC_NOERR) nco_err_exit(-1,"nco_aux_evl");
  rcd=nco_get_vara(in_id,lonid,&lon.srt,&lon.sz,lonvp,lon.type);
  if (rcd != NC_NOERR) nco_err_exit(-1,"nco_aux_evl");
  
  *lmt_nbr=0;
  
  lmt_sct base;
  base.nm=(char *)strdup(dmnname);
  base.lmt_typ=lmt_dmn_idx;
  base.is_usr_spc_lmt=1; 
  base.is_usr_spc_min=1; 
  base.is_usr_spc_max=1;
  base.srd_sng=(char *)strdup("1");
  base.is_rec_dmn=0;
  base.id=dmnid;
  base.min_idx=0;
  base.max_idx=0;
  base.srt=0;
  base.end=0;
  base.cnt=0;
  base.srd=1;
  
  /* malloc() the return lmt structure
     No way to know exact size in advance but maximum is about dimsz/2 */
  int MAXDMN=dmnsz/2;
  
  if(aux_nbr > 0) lmts=(lmt_sct **)nco_malloc(MAXDMN*sizeof(lmt_sct *));
  
  for(curit=0;curit<aux_nbr;curit++){
    /* Parse into lllong,lllat,urlon,urlon, accounting for units */
    nco_aux_prs(aux_arg[curit],units,&lllon,&lllat,&urlon,&urlat);
    /* printf("Box is %f %f %f %f\n",lllon,lllat,urlon,urlat); */
    
    mincell=-1;
    consec=0;
    for(cell=0; cell<dmnsz; cell++){
      if(lat.type == NC_FLOAT) clat=((float*)latvp)[cell]; else clat=((double*)latvp)[cell];
      if(lon.type == NC_FLOAT) clon=((float*)lonvp)[cell]; else clon=((double*)lonvp)[cell];
      if(clon >= lllon && clon <= urlon &&
	 clat >= lllat && clat <= urlat){
	/*printf("match %lf %lf %lf/ %lf %lf %lf\n",clon,lllon,urlon,clat,lllat,urlat); */
	if(mincell == -1){
	  mincell=cell;
	  consec=1;
	}else if(cell == mincell + consec){
	  consec++;
	}else{
	} /* end found matching cell */
      }else if(mincell != -1){
	sprintf(buf,"%d",mincell);
	base.min_sng=(char *)strdup(buf);
	base.min_idx=base.srt=mincell;
	sprintf(buf,"%d",mincell+consec-1);
	base.max_sng=(char *)strdup(buf);
	base.max_idx=base.end=mincell+consec-1;
	base.cnt=consec;
	(*lmt_nbr)++;
	if(*lmt_nbr > MAXDMN){
	  /*printf("Number of slabs exceeds allocated mamory %d\n",MAXDMN);*/
	  nco_err_exit(-1,"nco_aux_evl: Number of slabs exceeds allocated mamory.");
	} /* end if too many slabs */
	lmts[(*lmt_nbr)-1]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
	*lmts[(*lmt_nbr)-1]=base;
	mincell=-1;
      } /* end if one or more consecutive matching cells */
    } /* end loop over cells */
    
  } /* end loop over user supplied -X options */

  /* fxm: this is weird */
  if (units != 0) units=(char *)nco_free(units);
  latvp=nco_free(latvp);
  lonvp=nco_free(lonvp);
  
  /* printf ("returning structure %d\n",*lmt_nbr);
    for (curit=0; curit<(*lmt_nbr); curit++)
    printf("LIMIT %ld %ld \n",lmts[curit]->min_idx,lmts[curit]->max_idx); */
  return lmts;
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
