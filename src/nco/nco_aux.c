/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_aux.c,v 1.33 2011-02-21 22:38:42 zender Exp $ */

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
  
  const char fnc_nm[]="nco_find_lat_lon()";

  char var_nm[NC_MAX_NAME];
  char value[NC_MAX_NAME];

  int idx;
  int nvars=0;
  int rcd=NC_NOERR;
  int ret=0;
  int var_dimid[NC_MAX_VAR_DIMS]; /* [enm] Dimension ID */
  int var_att_nbr; /* [nbr] Number of attributes */
  int var_dmn_nbr; /* [nbr] Number of dimensions */

  long lenp;

  nc_type var_type; /* [enm] variable type */
  
  /* Make sure CF tag exists. Currently require CF-1.0 value */
  if(NCO_GET_ATT_CHAR(nc_id,NC_GLOBAL,"Conventions",value) || !strstr(value,"CF-1.0")){
    (void)fprintf(stderr,"%s: WARNING %s reports file \"Convention\" attribute is missing or not equal to \"CF-1.0\". Auxiliary coordinate support (i.e., the -X option) cannot be expected to behave well file does not support CF-1.0 metadata conventions. Continuing anyway...\n",prg_nm_get(),fnc_nm);
  } /* !CF */
  
  /* Get number of variables */
  rcd=nco_inq_nvars(nc_id,&nvars);
  
  /* For each variable, see if standard name is latitude or longitude */
  for(idx=0;idx<nvars && ret<2;idx++){
    nco_inq_var(nc_id,idx,var_nm,&var_type,&var_dmn_nbr,var_dimid,&var_att_nbr);
    lenp=0;
    if(!nco_inq_attlen_flg(nc_id,idx,"standard_name",&lenp)){
      NCO_GET_ATT_CHAR(nc_id,idx,"standard_name",value);
      value[lenp]='\0';
      if(strcmp(value,"latitude") == 0){
	strcpy(var_nm_lat,var_nm);
	*lat_id=idx;
	
	/* Get units; assume same for both lat and lon */
	rcd=nco_inq_attlen(nc_id,idx,"units",&lenp);
	if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_find_lat_lon() reports CF convention requires \"latitude\" to have units attribute\n");
	*units=(char *)nco_malloc((lenp+1L)*sizeof(char *));
	NCO_GET_ATT_CHAR(nc_id,idx,"units",*units);
	units[lenp]='\0';
	
	if(var_dmn_nbr > 1) (void)fprintf(stderr,"%s: WARNING %s reports latitude variable %s has %d dimensions. NCO only supports hyperslabbing of auxiliary coordinate variables with a single dimension. Continuing with unpredictable results...\n",prg_nm_get(),fnc_nm,var_nm,var_dmn_nbr);

	/* Assign type; assumed same for both lat and lon */
	*crd_typ=var_type;
	ret++;
      } /* end if var is lattitude */
      
      if(strcmp(value,"longitude") == 0){
	strcpy(var_nm_lon,var_nm);
	*lon_id=idx;
	ret++;
      } /* end if var is longitude */
      
    } /* end if standard_name */
    
  } /* end loop over vars */
  
  // Die if both not found
  if(ret != 2) nco_err_exit(rcd,"nco_find_lat_lon() unable to identify lat/lon auxiliary coordinate variables.");
 
 return rcd;

} /* end nco_find_lat_lon() */

int  /* status code */
nco_get_dmn_info
(int nc_id,
 int var_id,
 char dmn_nm[],
 int *dimid,
 long *dmn_sz)
{
  /* Purpose: Get dimension information associated with specified variable
     In our case, this is lat or lon---they are presumed to be identical */
  
  int rcd=NC_NOERR;
  
  nc_type var_type;                   /* variable type */
  int var_dimid[NC_MAX_VAR_DIMS];    /* dimension ids */
  int var_att_nbr;                      /* number of attributes */
  int var_dmn_nbr;                      /* number of dims */
  
  /* Get dimension information */
  rcd=nco_inq_var(nc_id,var_id,0,&var_type,&var_dmn_nbr,var_dimid,&var_att_nbr);
  if(rcd == NC_NOERR){
    *dimid=var_dimid[0];
    rcd=nco_inq_dimlen(nc_id,var_dimid[0],dmn_sz);
    rcd=nco_inq_dimname(nc_id,var_dimid[0],dmn_nm);
  } /* endif */

  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_get_dmn_info() unable to get dimension information");

  return rcd;
} /* end nco_get_dmn_info() */

lmt_sct **
nco_aux_evl
(int in_id, 
 int aux_nbr, 
 char *aux_arg[],
 int *lmt_nbr
 ){
  /* Purpose: Create lmt structure of slabs of continguous cells that
     match rectangular region specified by -X arguments.
     Intended for use with non-monotonic grids
     Requires CF-1.0 conventions
     Uses lat/lon centers rather than cell_bounds to detect matches
     Code assumes units are degrees if they are not radians */
  
  const char fnc_nm[]="nco_aux_evl()"; /* [sng] Function name */

  char *units=NULL; /* fxm TODO nco925: "units" value needs dynamically allocated size in case value exceeds NC_MAX_NAME */
  char cll_idx_sng[100]; /* Buffer for user-assigned limit names */
  char dmn_nm[NC_MAX_NAME];
  char var_nm_lat[NC_MAX_NAME];
  char var_nm_lon[NC_MAX_NAME];
  
  dmn_sct lat;
  dmn_sct lon;
  
  double lat_crr; /* [dgr] Current cell latitude */
  double lon_crr; /* [dgr] Current cell longitude */

  float lat_ll; /* [dgr] Lower left latitude of bounding rectangle */
  float lat_ur; /* [dgr] Upper right longitude of bounding rectangle */
  float lon_ll; /* [dgr] Lower left longitude of bounding rectangle */
  float lon_ur; /* [dgr] Upper right latitude of bounding rectangle */
  
  int aux_idx; /* [idx] Index over user -X options */
  int cll_grp_nbr=0; /* [nbr] Number of groups of cells within this bounding box */
  int cll_idx; /* [idx] Cell index */
  int cll_idx_min=-1; /* [idx] Minimum index of cell in consecutive cell set */
  int cll_nbr_cns=0; /* [nbr] Number of consecutive cells within current group */
  int cll_nbr_ttl=0; /* [nbr] Total number of cells within this bounding box */
  int dmn_id=int_CEWI;
  int lat_id;
  int lon_id;
  int rcd=NC_NOERR;
  
  lmt_sct **lmt=0; /* [sct] List of returned lmt structures */
  
  long dmn_sz=0;
  
  nc_type crd_typ;
  
  void *vp_lat; /* [dgr] Latitude coordinate array, float or double */
  void *vp_lon; /* [dgr] Longitude coordinate array, float or double */
  
  /* Obtain lat/lon variable names */
  rcd=nco_find_lat_lon(in_id,var_nm_lat,var_nm_lon,&units,&lat_id,&lon_id,&crd_typ);
  
  /* Obtain dimension information of lat/lon coordinates */
  rcd=nco_get_dmn_info(in_id,lat_id,dmn_nm,&dmn_id,&dmn_sz);
  
  /* Load latitude/longitude variables needed to search for region matches */
  lat.type=crd_typ;
  lat.sz=dmn_sz;
  lat.srt=0L;
  vp_lat=(void *)nco_malloc(dmn_sz*nco_typ_lng(lat.type));
  lon.type=crd_typ;
  lon.sz=dmn_sz;
  lon.srt=0L;
  vp_lon=(void *)nco_malloc(dmn_sz*nco_typ_lng(lon.type));
  rcd=nco_get_vara(in_id,lat_id,&lat.srt,&lat.sz,vp_lat,lat.type);
  rcd=nco_get_vara(in_id,lon_id,&lon.srt,&lon.sz,vp_lon,lon.type);
  
  *lmt_nbr=0;
  
  lmt_sct base;
  base.nm=(char *)strdup(dmn_nm);
  base.lmt_typ=lmt_dmn_idx;
  base.is_usr_spc_lmt=True; 
  base.is_usr_spc_min=True; 
  base.is_usr_spc_max=True;
  base.srd_sng=(char *)strdup("1");
  base.is_rec_dmn=0;
  base.id=dmn_id;
  base.min_idx=0;
  base.max_idx=0;
  base.srt=0L;
  base.end=0L;
  base.cnt=0L;
  base.srd=1L;
  
  /* malloc() lmt structure to return
     No way to know exact size in advance though maximum is about dim_sz/2 */
  int MAX_LMT_NBR=dmn_sz/2;
  
  if(aux_nbr > 0) lmt=(lmt_sct **)nco_malloc(MAX_LMT_NBR*sizeof(lmt_sct *));

  /* Loop over user-specified bounding boxes */
  for(aux_idx=0;aux_idx<aux_nbr;aux_idx++){
    /* Parse into lon_ll,lat_ll,lon_ur,lon_ur, accounting for units */
    nco_aux_prs(aux_arg[aux_idx],units,&lon_ll,&lon_ur,&lat_ll,&lat_ur);
    /* Current cell assumed to lay outside current bounding box */
    cll_idx_min=-1;
    /* Initialize number of consecutive cells inside current bounding box */
    cll_nbr_cns=0; /* [nbr] Number of consecutive cells within current group */
    cll_nbr_ttl=0; /* [nbr] Total number of cells within this bounding box */
    cll_grp_nbr=0; /* [nbr] Number of groups of cells within this bounding box */
    /* Loop over auxiliary coordinate cells */
    for(cll_idx=0;cll_idx<dmn_sz;cll_idx++){
      if(lat.type == NC_FLOAT) lat_crr=((float *)vp_lat)[cll_idx]; else lat_crr=((double *)vp_lat)[cll_idx];
      if(lon.type == NC_FLOAT) lon_crr=((float *)vp_lon)[cll_idx]; else lon_crr=((double *)vp_lon)[cll_idx];
      if(lon_crr >= lon_ll && lon_crr <= lon_ur &&
	 lat_crr >= lat_ll && lat_crr <= lat_ur){
	if(cll_idx_min == -1){
	  /* First cell within current bounding box */
	  cll_idx_min=cll_idx;
	  cll_nbr_cns=1;
	}else if(cll_idx == cll_idx_min+cll_nbr_cns){
	  /* Later, contiguous cell within current bounding box */
	  cll_nbr_cns++;
	} /* end found matching cell */
      }else if(cll_idx_min != -1){
	/* Current cell is not within bounding box though immediately previous cell is */
	sprintf(cll_idx_sng,"%d",cll_idx_min);
	base.min_sng=(char *)strdup(cll_idx_sng);
	base.min_idx=base.srt=cll_idx_min;
	sprintf(cll_idx_sng,"%d",cll_idx_min+cll_nbr_cns-1);
	base.max_sng=(char *)strdup(cll_idx_sng);
	base.max_idx=base.end=cll_idx_min+cll_nbr_cns-1;
	base.cnt=cll_nbr_cns;
	(*lmt_nbr)++;
	if(*lmt_nbr > MAX_LMT_NBR) nco_err_exit(-1,"%s: Number of slabs exceeds allocated mamory");
	lmt[(*lmt_nbr)-1]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
	*lmt[(*lmt_nbr)-1]=base;
	cll_grp_nbr++;
	cll_nbr_ttl+=cll_nbr_cns;
	/* Indicate that next cell, if any, in this bounding box requires new limit structure */
	cll_idx_min=-1;
      } /* end if one or more consecutive matching cells */
    } /* end loop over cells */
    if(dbg_lvl_get() > nco_dbg_scl) (void)fprintf(stdout,"%s: %s reports bounding-box %g <= %s <= %g and %g <= %s <= %g brackets %d distinct group(s) comprising %d total gridpoint(s)\n",prg_nm_get(),fnc_nm,lon_ll,var_nm_lon,lon_ur,lat_ll,var_nm_lat,lat_ur,cll_grp_nbr,cll_nbr_ttl); 
  } /* end loop over user supplied -X options */

  /* Free allocated memory */
  if(units) units=(char *)nco_free(units);
  if(vp_lat) vp_lat=nco_free(vp_lat);
  if(vp_lon) vp_lon=nco_free(vp_lon);
  
  /* With some loss of generality, we assume cell-based coordinates are not 
     record coordinates spanning multiple files. Thus finding no cells within
     any bounding box constitutes a domain error. */
  if(*lmt_nbr == 0){
    (void)fprintf(stdout,"%s: ERROR %s reports that none of the %d specified auxiliary-coordinate bounding-box(es) contain any latitude/longitude coordinate pairs. This condition was not flagged as an error until 20110221. Prior to that, when no coordinates were in any of the user-specified auxiliary-coordinate hyperslab(s), NCO mistakenly returned the entire coordinate range as being within the hyperslab(s).\n",prg_nm_get(),fnc_nm,aux_nbr);
    nco_exit(EXIT_FAILURE);
  } /* end if */
  
  return lmt;
} /* end nco_aux_evl() */

void 
nco_aux_prs
(const char *args,
 const char *units,
 float *lon_ll,
 float *lon_ur,
 float *lat_ll,
 float *lat_ur)
{
  /* Purpose: Parse command-line arguments of form:
     min_lon,max_lon,min_lat,max_lat */
  char *tmp_args;
  char *token;
  
  tmp_args=strdup(args);
  
  sscanf(args,"%f,%f,%f,%f",lon_ll,lon_ur,lat_ll,lat_ur);
  token=strtok(tmp_args,", ");
  if(token) sscanf(token,"%f",lon_ll); else nco_err_exit(-1,"nco_aux_prs(): Problem with LL longitude string");
  token=strtok(NULL,", ");
  if(token) sscanf(token,"%f",lon_ur); else nco_err_exit(-1,"nco_aux_prs(): Problem with UR longitude string");
  token=strtok(NULL,", ");
  if(token) sscanf(token,"%f",lat_ll); else nco_err_exit(-1,"nco_aux_prs(): Problem with LL latitude string");
  token=strtok(NULL,", ");
  if(token) sscanf(token,"%f",lat_ur); else nco_err_exit(-1,"nco_aux_prs(): Problem with UR latitude string");
  
  if(tmp_args) tmp_args=(char *)nco_free(tmp_args);
  
  if(strcmp(units,"radians") == 0){
    /* WIN32 math.h does not define M_PI */
#ifndef M_PI
# define M_PI		3.14159265358979323846
#endif /* M_PI */
    *lon_ll*=M_PI/180.0;
    *lon_ur*=M_PI/180.0;
    *lat_ll*=M_PI/180.0;
    *lat_ur*=M_PI/180.0;
  } /* endif radians */
} /* nco_aux_prs */
