/* $Header$ */

/* Copyright (C) 1995--present Charlie Zender
   This file is part of NCO, the netCDF Operators. NCO is free software.
   You may redistribute and/or modify NCO under the terms of the 
   3-Clause BSD License with exceptions described in the LICENSE file */

/* Purpose: Support hyperslabbing cell-based grids over coordinate ranges 
   This works on datasets that contain CF-convention auxiliary coordinate variables.
   Such datasets contain variables with standard_name's "latitude" and "longitude". 
   Cells that contain a value within the user-requested range are considered a match.
   Could be useful to look at the CF bounds variable instead but harder.
   
   Original Author: Karen Schuchardt, PNNL
   
   Example usage:
   ncks -v gds_3dvar -X 0.,45.,0.,90. -X 180.,225.,-90.,0. ~/nco/data/in.nc ~/foo.nc
   ncks -X -99,-93,28,32 ${DATA}/hdf/nwm.t18z.short_range.channel_rt.f015.conus.nc_georeferenced.nc ~/foo.nc
   ncatted -O -a standard_name,latitude,o,c,latitude -a standard_name,longitude,o,c,longitude ${DATA}/hdf/nwm.t18z.short_range.channel_rt.f015.conus.nc_georeferenced.nc ~/foo.nc
   ncks -O -D 3 -X -99,-93,28,32 ~/foo.nc ~/foo2.nc */

#include "nco_aux.h" /* Auxiliary coordinates */

nco_bool 
nco_find_lat_lon
(int nc_id,
 char var_nm_lat[], 
 char var_nm_lon[], 
 char **units,
 int *lat_id,
 int *lon_id,
 nc_type *crd_typ)
{
  /* Purpose: Find auxiliary coordinate variables that map to latitude/longitude 
     Find variables with standard_name = "latitude" and "longitude"
     Return true if both latitude and longitude standard names are found
     Also return needed information about these auxiliary coordinates
     Assumes that units and types for latitude and longitude are identical
     Caller responsible for memory management for variable names
     Memory for unit strings must be freed by caller */
  
  const char fnc_nm[]="nco_find_lat_lon()";

  char var_nm[NC_MAX_NAME];
  char att_val[NC_MAX_NAME];

  int idx;
  int var_nbr=0;
  int rcd=NC_NOERR;
  int crd_nbr=0;
  int var_dmn_nbr; /* [nbr] Number of dimensions */

  long att_lng;

  nc_type var_typ; /* [enm] variable type */

  /* Initialize outputs */
  *lat_id=NC_MIN_INT;
  *lon_id=NC_MIN_INT;

  /* Make sure CF tag exists. Currently require CF-1.X value */
  if(NCO_GET_ATT_CHAR(nc_id,NC_GLOBAL,"Conventions",att_val) || !strstr(att_val,"CF-1."))
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stderr,"%s: WARNING %s reports file \"Convention\" attribute is missing or is present but not of the form \"CF-1.X\". Auxiliary coordinate support (i.e., the -X option) works best when file complies with CF-1.X metadata conventions. Continuing anyway...\n",nco_prg_nm_get(),fnc_nm);

  /* Get number of variables */
  rcd=nco_inq_nvars(nc_id,&var_nbr);

  /* For each variable, see if standard name is latitude or longitude */
  for(idx=0;idx<var_nbr && crd_nbr<2;idx++){
    nco_inq_var(nc_id,idx,var_nm,NULL,NULL,NULL,NULL);
    att_lng=0L;

    if(!nco_inq_attlen_flg(nc_id,idx,"standard_name",&att_lng)){
      NCO_GET_ATT_CHAR(nc_id,idx,"standard_name",att_val);
      att_val[att_lng]='\0';
      if(!strcmp(att_val,"latitude")){
        strcpy(var_nm_lat,var_nm);
        *lat_id=idx;
        crd_nbr++;
      } /* endif latitude */
      
      if(!strcmp(att_val,"longitude")){
        strcpy(var_nm_lon,var_nm);
        *lon_id=idx;
        crd_nbr++;
      } /* endif longitude */
    } /* endif standard_name */

  } /* !idx */
  

  if((*lat_id == NC_MIN_INT) || (*lon_id == NC_MIN_INT)){
    if(nco_dbg_lvl_get() >= nco_dbg_std) (void)fprintf(stdout,"%s: %s unable to identify lat/lon auxiliary coordinate variables.\n",nco_prg_nm_get(),fnc_nm);
    return False;
  } /* !lat_id, !lon_id */

  nco_inq_var(nc_id,*lat_id,NULL,&var_typ,&var_dmn_nbr,NULL,NULL);

  /* Assign type, assumed same for both lat and lon */
  *crd_typ=var_typ;

  /* Get units, assume same for both lat and lon */
  rcd=nco_inq_attlen(nc_id,*lat_id,"units",&att_lng);
  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_find_lat_lon() reports CF convention requires \"latitude\" to have units attribute\n");
  *units=(char *)nco_malloc((att_lng+1L)*sizeof(char));
  NCO_GET_ATT_CHAR(nc_id,*lat_id,"units",*units);
  (*units)[att_lng]='\0';

  if(var_dmn_nbr > 1) (void)fprintf(stderr,"%s: WARNING %s reports latitude variable %s has %d dimensions. NCO only supports hyperslabbing of auxiliary coordinate variables with a single dimension. Continuing with unpredictable results...\n",nco_prg_nm_get(),fnc_nm,var_nm,var_dmn_nbr);

  return True;

} /* end nco_find_lat_lon() */

int /* [enm] Return status */
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
  
  nc_type var_typ;                   /* variable type */
  int var_dimid[NC_MAX_VAR_DIMS];    /* dimension ids */
  int var_att_nbr;                      /* number of attributes */
  int var_dmn_nbr;                      /* number of dims */
  
  /* Get dimension information */
  rcd=nco_inq_var(nc_id,var_id,0,&var_typ,&var_dmn_nbr,var_dimid,&var_att_nbr);
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
 int *lmt_nbr,
 char *nm_dmn)                     /* O [sng] Dimension name */ 
{
  /* Purpose: Create lmt structure of slabs of continguous cells that match rectangular region specified by -X arguments
     Intended for use with non-monotonic grids
     Requires CF-1.0 conventions
     Uses latitude/longitude centers rather than cell_bounds to detect matches
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

  double lat_min; /* [dgr] Lower left latitude of bounding rectangle */
  double lat_max; /* [dgr] Upper right longitude of bounding rectangle */
  double lon_min; /* [dgr] Lower left longitude of bounding rectangle */
  double lon_max; /* [dgr] Upper right latitude of bounding rectangle */

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

  lmt_sct **lmt=NULL; /* [sct] List of returned lmt structures */

  long dmn_sz=0;

  nc_type crd_typ;

  void *vp_lat; /* [dgr] Latitude coordinate array, float or double */
  void *vp_lon; /* [dgr] Longitude coordinate array, float or double */

  nco_bool has_lat_lon;

  *lmt_nbr=0;

  /* Obtain lat/lon variable names */
  has_lat_lon=nco_find_lat_lon(in_id,var_nm_lat,var_nm_lon,&units,&lat_id,&lon_id,&crd_typ);

  if(!has_lat_lon) return NULL;

  /* Obtain dimension information of lat/lon coordinates */
  rcd+=nco_get_dmn_info(in_id,lat_id,dmn_nm,&dmn_id,&dmn_sz);

  if(rcd != NC_NOERR) nco_err_exit(rcd,"nco_aux_evl() unable get past nco_get_dmn_info()\n");

  /* Load latitude/longitude variables needed to search for region matches */
  lat.type=crd_typ;
  lat.sz=dmn_sz;
  lat.srt=0L;
  vp_lat=(void *)nco_malloc(dmn_sz*nco_typ_lng(lat.type));
  lon.type=crd_typ;
  lon.sz=dmn_sz;
  lon.srt=0L;
  vp_lon=(void *)nco_malloc(dmn_sz*nco_typ_lng(lon.type));
  rcd+=nco_get_vara(in_id,lat_id,&lat.srt,&lat.sz,vp_lat,lat.type);
  rcd+=nco_get_vara(in_id,lon_id,&lon.srt,&lon.sz,vp_lon,lon.type);

  lmt_sct lmt_tpl;
  (void)nco_lmt_init(&lmt_tpl);
  lmt_tpl.nm=(char *)strdup(dmn_nm);
  lmt_tpl.lmt_typ=lmt_dmn_idx;
  lmt_tpl.is_usr_spc_lmt=True; 
  lmt_tpl.is_usr_spc_min=True; 
  lmt_tpl.is_usr_spc_max=True;
  lmt_tpl.flg_mro=False;
  lmt_tpl.flg_ilv=False;
  lmt_tpl.srd_sng=(char *)strdup("1");
  lmt_tpl.ssc_sng=NULL;
  lmt_tpl.ssc_sng=NULL;
  lmt_tpl.ilv_sng=NULL;
  lmt_tpl.is_rec_dmn=0;
  lmt_tpl.id=dmn_id;
  lmt_tpl.min_idx=0;
  lmt_tpl.max_idx=0;
  lmt_tpl.srt=0L;
  lmt_tpl.end=0L;
  lmt_tpl.cnt=0L;
  lmt_tpl.srd=1L;
  lmt_tpl.ssc=1L;
  lmt_tpl.ilv=1L;

  /* malloc() lmt structure to return
     No way to know exact size in advance though maximum is about dim_sz/2 */
  int MAX_LMT_NBR=dmn_sz/2;

  if(aux_nbr > 0) lmt=(lmt_sct **)nco_malloc(MAX_LMT_NBR*sizeof(lmt_sct *));

  /* Loop over user-specified bounding boxes */
  for(aux_idx=0;aux_idx<aux_nbr;aux_idx++){
    /* Parse into lon_min,lat_min,lon_max,lon_max, accounting for units */
    nco_aux_prs(aux_arg[aux_idx],units,&lon_min,&lon_max,&lat_min,&lat_max);
    /* Current cell assumed to lay outside current bounding box */
    cll_idx_min=-1;
    /* Initialize number of consecutive cells inside current bounding box */
    cll_nbr_cns=0; /* [nbr] Number of consecutive cells within current group */
    cll_nbr_ttl=0; /* [nbr] Total number of cells within this bounding box */
    cll_grp_nbr=0; /* [nbr] Number of groups of cells within this bounding box */
    if(lon_min == lon_max){
      (void)fprintf(stderr,"%s: ERROR %s reports degenerate auxiliary coordinate hyperslab with lon_min = lon_max = %g. Auxiliary coordinates do not support degenerate hyperlabs yet. This is TODO nco1010. If this feature is important to you, post your vexation to sourceforge and we will work on it.\n",nco_prg_nm_get(),fnc_nm,lon_min);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    if(lat_min == lat_max){
      (void)fprintf(stderr,"%s: ERROR %s reports degenerate auxiliary coordinate hyperslab with lat_min = lat_max = %g. Auxiliary coordinates do not support degenerate hyperlabs yet. This is TODO nco1010. If this feature is important to you, post your vexation to sourceforge and we will work on it.\n",nco_prg_nm_get(),fnc_nm,lat_min);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    /* Loop over auxiliary coordinate cells */
    for(cll_idx=0;cll_idx<dmn_sz;cll_idx++){
      if(lat.type == NC_FLOAT) lat_crr=((float *)vp_lat)[cll_idx]; else lat_crr=((double *)vp_lat)[cll_idx];
      if(lon.type == NC_FLOAT) lon_crr=((float *)vp_lon)[cll_idx]; else lon_crr=((double *)vp_lon)[cll_idx];
      if(lon_crr >= lon_min && lon_crr <= lon_max &&
        lat_crr >= lat_min && lat_crr <= lat_max){
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
        lmt_tpl.min_sng=(char *)strdup(cll_idx_sng);
        lmt_tpl.min_idx=lmt_tpl.srt=cll_idx_min;
        sprintf(cll_idx_sng,"%d",cll_idx_min+cll_nbr_cns-1);
        lmt_tpl.max_sng=(char *)strdup(cll_idx_sng);
        lmt_tpl.max_idx=lmt_tpl.end=cll_idx_min+cll_nbr_cns-1;
        lmt_tpl.cnt=cll_nbr_cns;
        (*lmt_nbr)++;
        if(*lmt_nbr > MAX_LMT_NBR) nco_err_exit(0,"%s: Number of slabs exceeds allocated memory");
        lmt[(*lmt_nbr)-1]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
        *lmt[(*lmt_nbr)-1]=lmt_tpl;
        cll_grp_nbr++;
        cll_nbr_ttl+=cll_nbr_cns;
        /* Indicate that next cell, if any, in this bounding box requires new limit structure */
        cll_idx_min=-1;
      } /* end if one or more consecutive matching cells */
    } /* end loop over cells */
    if(nco_dbg_lvl_get() > nco_dbg_scl && nco_dbg_lvl_get() != nco_dbg_dev){
      (void)fprintf(stdout,"%s: %s reports bounding-box %g <= %s <= %g and %g <= %s <= %g brackets %d distinct group(s) comprising %d total gridpoint(s)\n",nco_prg_nm_get(),fnc_nm,lon_min,var_nm_lon,lon_max,lat_min,var_nm_lat,lat_max,cll_grp_nbr,cll_nbr_ttl); 
    }
  } /* end loop over user supplied -X options */

  /* Free allocated memory */
  if(units) units=(char *)nco_free(units);
  if(vp_lat) vp_lat=nco_free(vp_lat);
  if(vp_lon) vp_lon=nco_free(vp_lon);

  /* With some loss of generality, we assume cell-based coordinates are not 
     record coordinates spanning multiple files. Thus finding no cells within
     any bounding box constitutes a domain error. */
  if(*lmt_nbr == 0){
    (void)fprintf(stdout,"%s: ERROR %s reports that none of the %d specified auxiliary-coordinate bounding-box(es) contain any latitude/longitude coordinate pairs. This condition was not flagged as an error until 20110221. Prior to that, when no coordinates were in any of the user-specified auxiliary-coordinate hyperslab(s), NCO mistakenly returned the entire coordinate range as being within the hyperslab(s).\n",nco_prg_nm_get(),fnc_nm,aux_nbr);
    nco_exit(EXIT_FAILURE);
  } /* end if */

  lmt=(lmt_sct **)nco_realloc(lmt,(*lmt_nbr)*sizeof(lmt_sct *));

  /* Export dimension name */
  strcpy(nm_dmn,dmn_nm);

  return lmt;
} /* end nco_aux_evl() */

void 
nco_aux_prs
(const char *bnd_bx_sng,
 const char *units,
 double *lon_min,
 double *lon_max,
 double *lat_min,
 double *lat_max)
{
  /* Purpose: Parse command-line arguments of form:
     lon_min,lon_max,lat_min,lat_max */
  char *bnd_bx_sng_tmp;
  char *crd_tkn;
  
  bnd_bx_sng_tmp=strdup(bnd_bx_sng);
  
  sscanf(bnd_bx_sng,"%lf,%lf,%lf,%lf",lon_min,lon_max,lat_min,lat_max);
  crd_tkn=strtok(bnd_bx_sng_tmp,", ");
  if(crd_tkn) sscanf(crd_tkn,"%lf",lon_min); else nco_err_exit(0,"nco_aux_prs(): Problem with LL longitude string");
  crd_tkn=strtok(NULL,", ");
  if(crd_tkn) sscanf(crd_tkn,"%lf",lon_max); else nco_err_exit(0,"nco_aux_prs(): Problem with UR longitude string");
  crd_tkn=strtok(NULL,", ");
  if(crd_tkn) sscanf(crd_tkn,"%lf",lat_min); else nco_err_exit(0,"nco_aux_prs(): Problem with LL latitude string");
  crd_tkn=strtok(NULL,", ");
  if(crd_tkn) sscanf(crd_tkn,"%lf",lat_max); else nco_err_exit(0,"nco_aux_prs(): Problem with UR latitude string");
  
  if(bnd_bx_sng_tmp) bnd_bx_sng_tmp=(char *)nco_free(bnd_bx_sng_tmp);
  
  if(!strcmp(units,"radians")){
    const double dgr2rdn=M_PI/180.0;
    *lon_min*=dgr2rdn;
    *lon_max*=dgr2rdn;
    *lat_min*=dgr2rdn;
    *lat_max*=dgr2rdn;
  } /* endif radians */
} /* nco_aux_prs */

lmt_sct **                           /* O [lst] Auxiliary coordinate limits */
nco_aux_evl_trv
(const int nc_id,                    /* I [ID] netCDF file ID */
 int aux_nbr,                        /* I [sng] Number of auxiliary coordinates */
 char *aux_arg[],                    /* I [sng] Auxiliary coordinates */
 trv_sct *lat_trv,                   /* I [sct] "latitude" variable */
 trv_sct *lon_trv,                   /* I [sct] "longitude" variable */
 const nc_type crd_typ,              /* I [nbr] netCDF type of both "latitude" and "longitude" */
 const char * const units,           /* I [sng] Units of both "latitude" and "longitude" */
 int *aux_lmt_nbr)                   /* I/O [nbr] Number of coordinate limits */
{
  /* Purpose: Create lmt structure of slabs of continguous cells that match rectangular region specified by -X arguments
     Intended for use with non-monotonic grids
     Requires CF-1.0 conventions
     Uses latitude/longitude centers rather than cell_bounds to detect matches
     Code assumes units are degrees if they are not radians */

  const char fnc_nm[]="nco_aux_evl_trv()";

  char cll_idx_sng[100]; /* Buffer for user-assigned limit names */
  char dmn_nm[NC_MAX_NAME];
  char var_nm_lat[NC_MAX_NAME];
  char var_nm_lon[NC_MAX_NAME];

  dmn_sct lat;
  dmn_sct lon;

  double lat_crr; /* [dgr] Current cell latitude */
  double lon_crr; /* [dgr] Current cell longitude */

  double lat_min; /* [dgr] Lower left latitude of bounding rectangle */
  double lat_max; /* [dgr] Upper right longitude of bounding rectangle */
  double lon_min; /* [dgr] Lower left longitude of bounding rectangle */
  double lon_max; /* [dgr] Upper right latitude of bounding rectangle */

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
  int grp_id_lat;               /* [id] Group ID */
  int grp_id_lon;               /* [id] Group ID */

  lmt_sct **lmt=NULL; /* [sct] List of returned lmt structures */

  long dmn_sz=0;

  void *vp_lat; /* [dgr] Latitude coordinate array, float or double */
  void *vp_lon; /* [dgr] Longitude coordinate array, float or double */

  *aux_lmt_nbr=0;

  /* Obtain group ID of 'latitude' and 'longitude' */
  (void)nco_inq_grp_full_ncid(nc_id,lat_trv->grp_nm_fll,&grp_id_lat);
  (void)nco_inq_grp_full_ncid(nc_id,lon_trv->grp_nm_fll,&grp_id_lon);

  /* Obtain variable ID of 'latitude' and 'longitude' */
  (void)nco_inq_varid(grp_id_lat,lat_trv->nm,&lat_id);
  (void)nco_inq_varid(grp_id_lon,lon_trv->nm,&lon_id);

  /* Obtain dimension information of lat/lon coordinates */
  (void)nco_get_dmn_info(grp_id_lat,lat_id,dmn_nm,&dmn_id,&dmn_sz);

   /* Load latitude/longitude variables needed to search for region matches */
  lat.type=crd_typ;
  lat.sz=dmn_sz;
  lat.srt=0L;
  vp_lat=(void *)nco_malloc(dmn_sz*nco_typ_lng(lat.type));
  lon.type=crd_typ;
  lon.sz=dmn_sz;
  lon.srt=0L;
  vp_lon=(void *)nco_malloc(dmn_sz*nco_typ_lng(lon.type));
  rcd+=nco_get_vara(grp_id_lat,lat_id,&lat.srt,&lat.sz,vp_lat,lat.type);
  rcd+=nco_get_vara(grp_id_lon,lon_id,&lon.srt,&lon.sz,vp_lon,lon.type);

  lmt_sct lmt_tpl;
  (void)nco_lmt_init(&lmt_tpl);
  lmt_tpl.nm=(char *)strdup(dmn_nm);
  lmt_tpl.lmt_typ=lmt_dmn_idx;
  lmt_tpl.is_usr_spc_lmt=True; 
  lmt_tpl.is_usr_spc_min=True; 
  lmt_tpl.is_usr_spc_max=True;
  lmt_tpl.flg_mro=False;
  lmt_tpl.flg_ilv=False;
  lmt_tpl.srd_sng=(char *)strdup("1");
  lmt_tpl.ssc_sng=NULL;
  lmt_tpl.ssc_sng=NULL;
  lmt_tpl.ilv_sng=NULL;
  lmt_tpl.is_rec_dmn=0;
  lmt_tpl.id=dmn_id;
  lmt_tpl.min_idx=0;
  lmt_tpl.max_idx=0;
  lmt_tpl.srt=0L;
  lmt_tpl.end=0L;
  lmt_tpl.cnt=0L;
  lmt_tpl.srd=1L;
  lmt_tpl.ssc=1L;
  lmt_tpl.ilv=1L;

  /* malloc() lmt structure to return
     No way to know exact size in advance though maximum is about dim_sz/2 */
  int MAX_LMT_NBR=dmn_sz/2;

  if(aux_nbr > 0) lmt=(lmt_sct **)nco_malloc(MAX_LMT_NBR*sizeof(lmt_sct *));

  /* Loop over user-specified bounding boxes */
  for(aux_idx=0;aux_idx<aux_nbr;aux_idx++){
    /* Parse into lon_min,lat_min,lon_max,lon_max, accounting for units */
    nco_aux_prs(aux_arg[aux_idx],units,&lon_min,&lon_max,&lat_min,&lat_max);
    /* Current cell assumed to lay outside current bounding box */
    cll_idx_min=-1;
    /* Initialize number of consecutive cells inside current bounding box */
    cll_nbr_cns=0; /* [nbr] Number of consecutive cells within current group */
    cll_nbr_ttl=0; /* [nbr] Total number of cells within this bounding box */
    cll_grp_nbr=0; /* [nbr] Number of groups of cells within this bounding box */
    if(lon_min == lon_max){
      (void)fprintf(stderr,"%s: ERROR %s reports degenerate auxiliary coordinate hyperslab with lon_min = lon_max = %g. Auxiliary coordinates do not support degenerate hyperlabs yet. This is TODO nco1010. If this feature is important to you, post your vexation to sourceforge and we will work on it.\n",nco_prg_nm_get(),fnc_nm,lon_min);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    if(lat_min == lat_max){
      (void)fprintf(stderr,"%s: ERROR %s reports degenerate auxiliary coordinate hyperslab with lat_min = lat_max = %g. Auxiliary coordinates do not support degenerate hyperlabs yet. This is TODO nco1010. If this feature is important to you, post your vexation to sourceforge and we will work on it.\n",nco_prg_nm_get(),fnc_nm,lat_min);
      nco_exit(EXIT_FAILURE);
    } /* endif */
    /* Loop over auxiliary coordinate cells */
    for(cll_idx=0;cll_idx<dmn_sz;cll_idx++){
      if(lat.type == NC_FLOAT) lat_crr=((float *)vp_lat)[cll_idx]; else lat_crr=((double *)vp_lat)[cll_idx];
      if(lon.type == NC_FLOAT) lon_crr=((float *)vp_lon)[cll_idx]; else lon_crr=((double *)vp_lon)[cll_idx];
      if(lon_crr >= lon_min && lon_crr <= lon_max && lat_crr >= lat_min && lat_crr <= lat_max){
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
        lmt_tpl.min_sng=(char *)strdup(cll_idx_sng);
        lmt_tpl.min_idx=lmt_tpl.srt=cll_idx_min;
        sprintf(cll_idx_sng,"%d",cll_idx_min+cll_nbr_cns-1);
        lmt_tpl.max_sng=(char *)strdup(cll_idx_sng);
        lmt_tpl.max_idx=lmt_tpl.end=cll_idx_min+cll_nbr_cns-1;
        lmt_tpl.cnt=cll_nbr_cns;
        (*aux_lmt_nbr)++;
        if(*aux_lmt_nbr > MAX_LMT_NBR) nco_err_exit(0,"%s: Number of slabs exceeds allocated memory");
        lmt[(*aux_lmt_nbr)-1]=(lmt_sct *)nco_malloc(sizeof(lmt_sct));
        *lmt[(*aux_lmt_nbr)-1]=lmt_tpl;
        cll_grp_nbr++;
        cll_nbr_ttl+=cll_nbr_cns;
        /* Indicate that next cell, if any, in this bounding box requires new limit structure */
        cll_idx_min=-1;
      } /* end if one or more consecutive matching cells */
    } /* end loop over cells */
    if(nco_dbg_lvl_get() > nco_dbg_scl && nco_dbg_lvl_get() != nco_dbg_dev) (void)fprintf(stdout,"%s: %s reports bounding-box %g <= %s <= %g and %g <= %s <= %g brackets %d distinct group(s) comprising %d total gridpoint(s)\n",nco_prg_nm_get(),fnc_nm,lon_min,var_nm_lon,lon_max,lat_min,var_nm_lat,lat_max,cll_grp_nbr,cll_nbr_ttl); 
  } /* end loop over user supplied -X options */

  /* Free allocated memory */
  if(vp_lat) vp_lat=nco_free(vp_lat);
  if(vp_lon) vp_lon=nco_free(vp_lon);

  /* No limits found */
  if(*aux_lmt_nbr == 0) return NULL;

  lmt=(lmt_sct **)nco_realloc(lmt,(*aux_lmt_nbr)*sizeof(lmt_sct *));

  return lmt;

} /* nco_aux_evl_trv */

nco_bool 
nco_find_lat_lon_trv
(const int nc_id,                    /* I [ID] netCDF file ID */
 const trv_sct * const var_trv,      /* I [sct] Variable to search for "standard_name" attribute */
 const char * const att_val_trg,     /* I [sng] Attribute value to find ("latitude" or "longitude") */
 char **var_nm_fll,                  /* I/O [sng] Full name of variable that has "latitude" or "longitude" attributes */
 int *dmn_id,                        /* I/O [id] Dimension ID of "latitude" and "longitude" */
 nc_type *crd_typ,                   /* I/O [enm] netCDF type of both "latitude" and "longitude" */
 char units[])                       /* I/O [sng] Units of both "latitude" and "longitude" */
{
  /* Purpose: Find auxiliary coordinate variables that map to latitude/longitude 
     Find variables with standard_name = "latitude" and "longitude"
     Return true if both latitude and longitude standard names are found
     Also return needed information about these auxiliary coordinates
     Assumes that units and types for latitude and longitude are identical
     Caller responsible for memory management for variable names
     Memory for unit strings must be freed by caller */
  
  const char fnc_nm[]="nco_find_lat_lon_trv()";

  char att_nm[NC_MAX_NAME]; /* [sng] Attribute name */
  char var_nm[NC_MAX_NAME];

  int grp_id;               /* [id] Group ID */
  int var_id;               /* [id] Variable ID */
  int var_dimid[NC_MAX_VAR_DIMS]; /* [enm] Dimension ID */
  int var_att_nbr;          /* [nbr] Number of attributes */
  int var_dmn_nbr;          /* [nbr] Number of dimensions */

  nc_type var_typ;          /* [enm] variable type */

  assert(var_trv->nco_typ == nco_obj_typ_var);

  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);

  /* Obtain variable ID */
  (void)nco_inq_varid(grp_id,var_trv->nm,&var_id);

  /* Find number of attributes */
  (void)nco_inq_var(grp_id,var_id,var_nm,&var_typ,&var_dmn_nbr,var_dimid,&var_att_nbr);

  assert(var_att_nbr == var_trv->nbr_att);

  for(int idx_att=0;idx_att<var_att_nbr;idx_att++){

    /* Skip attribute if not "standard_name" */
    (void)nco_inq_attname(grp_id,var_id,idx_att,att_nm);
    if(strcmp(att_nm,"standard_name")) continue;

    char att_val[NC_MAX_NAME+1];
    long att_lng;
    (void)nco_inq_attlen(grp_id,var_id,"standard_name",&att_lng);
    (void)NCO_GET_ATT_CHAR(grp_id,var_id,"standard_name",att_val);
    att_val[att_lng]='\0';

    /* Match parameter name to find ("latitude" or "longitude") */
    if(!strcmp(att_val,att_val_trg)){

      /* Assume same units (degrees or radians) for both lat and lon */
      int rcd=nco_inq_attlen_flg(grp_id,var_id,"units",&att_lng);
      if(rcd != NC_NOERR){
        if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: %s reports CF convention requires \"latitude\" to have units attribute\n",nco_prg_nm_get(),fnc_nm);
        return False;
      } /* endif */
      NCO_GET_ATT_CHAR(grp_id,var_id,"units",units);
      units[att_lng]='\0';

      if(var_dmn_nbr > 1) (void)fprintf(stderr,"%s: WARNING %s reports latitude variable %s has %d dimensions. NCO only supports hyperslabbing of auxiliary coordinate variables with a single dimension. Continuing with unpredictable results...\n",nco_prg_nm_get(),fnc_nm,var_nm,var_dmn_nbr);

      /* Copy values to export */
      *var_nm_fll=(char *)strdup(var_trv->nm_fll);
      *crd_typ=var_typ;
      *dmn_id=var_dimid[0];

      return True;
    } /* strcmp() */
  } /* end loop over attributes */

  return False;

} /* end nco_find_lat_lon_trv() */

nco_bool                             /* O [flg] True if all criteria met */  
nco_check_nm_aux
(const int nc_id,                    /* I [ID] netCDF file ID */
 const trv_sct * const var_trv,      /* I [sct] Variable to search for "standard_name" attribute */
 int *dmn_id,                        /* I/O [id] Dimension ID of "latitude" and "longitude" */
 nc_type *crd_typ,                   /* I/O [enm] netCDF type of both "latitude" and "longitude" */
 char units[])                       /* I/O [sng] Units of both "latitude" and "longitude" */
{
  /* Purpose: Check that variable var_trv fits the criteria for being an auxiliary coordinate  
     Must be 1D and NOT a record var and NOT a standard coordinate variable
     Also must have a "units" attribute   
     If all critera met - then return some information about variable */

  const char fnc_nm[]="nco_check_nm_aux()";

  char var_nm[NC_MAX_NAME];

  int rcd;
  int grp_id;               /* [id] Group ID */
  int var_id;               /* [id] Variable ID */
  int var_dimid[NC_MAX_VAR_DIMS]; /* [enm] Dimension ID */
  int var_att_nbr;          /* [nbr] Number of attributes */
  int var_dmn_nbr;          /* [nbr] Number of dimensions */

  long att_lng; 

  nc_type var_typ;          /* [enm] variable type */

  assert(var_trv->nco_typ == nco_obj_typ_var);

  if(False ||
     //     var_trv->is_1D_crd || /* 20180807: Not yet defined */
     //     var_trv->is_1D_rec_crd || /* 20180807: Not yet defined */
     var_trv->is_crd_var)
    return False;
  
  /* Obtain group ID */
  (void)nco_inq_grp_full_ncid(nc_id,var_trv->grp_nm_fll,&grp_id);

  /* Obtain variable ID */
  (void)nco_inq_varid(grp_id,var_trv->nm,&var_id);

  /* Obtain variable meta-data  */
  (void)nco_inq_var(grp_id,var_id,var_nm,&var_typ,&var_dmn_nbr,var_dimid,&var_att_nbr);

  assert(var_att_nbr == var_trv->nbr_att);

  /* Assume same units (degrees or radians) for both lat and lon */
  rcd=nco_inq_attlen_flg(grp_id,var_id,"units",&att_lng);
  
  if(rcd != NC_NOERR){
    if(nco_dbg_lvl_get() >= nco_dbg_var) (void)fprintf(stdout,"%s: %s reports CF convention requires \"%s\" to have units attribute\n",nco_prg_nm_get(),fnc_nm,var_nm);
    return False;
  } /* endif */
  
  NCO_GET_ATT_CHAR(grp_id,var_id,"units",units);
  units[att_lng]='\0';

  /* Auxiliary coordinates MUST be 1D */
  if(var_dmn_nbr != 1) return False;
  
  /* Copy values to export */
  *crd_typ=var_typ;
  *dmn_id=var_dimid[0];

  return True;

} /* !nco_check_nm_aux() */

