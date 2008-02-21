/* $Header: /data/zender/nco_20150216/nco/src/nco/nco_aux.c,v 1.10 2008-02-21 23:40:11 karkn Exp $ */

/* Copyright (C) 1995--2008 Charlie Zender and Karen Schuchardt
   You may copy, distribute, and/or modify this software under the terms of the GNU General Public License (GPL) Version 3
   See http://www.gnu.org/copyleft/gpl.html for full license text */

/* Purpose: Support hyperslabbing cell-based grids over coordinate ranges 
   This works on datasets that contain CF-convention auxiliary coordinate variables.
   Such datasets contain variables with standard_name's "latitude" and "longitude". 
   Cells that contain a value within the user-requested range are considered a match.
   Could be useful to look at the CF bounds variable instead.

   Author: Karen Schuchardt */

#include "nco_aux.h" /* Auxiliary coordinates */

int 
nco_find_lat_lon
(int nc_id,
 char latvar[], 
 char lonvar[], 
 char units[],
 int *latid,
 int *lonid,
 nc_type *coordtype
 ){
  /* Purpose: Find the auxillary coordinate variables that map to lat/lon 
   by looking through the data set. 
   Returns true if both latitude and longitude standard names are found.
   Also returns needed information about the variables.
   ASSUME that units and types for lat/lon are identical.
   Caller responsible for allocating enough memory for variable names and unit
   strings. */

  int rcd=NC_NOERR;
  int ret = 0;

   /* Get number of variables */
   int nvars = 0;
   rcd=nco_inq_nvars(nc_id,&nvars);

   /* For each variable, see if its standard name is latitude or longitude */
   nc_type var_type;                   /* variable type */
   int var_ndims;                      /* number of dims */
   int var_dimids[NC_MAX_VAR_DIMS];    /* dimension ids */
   int var_natts;                       /* number of attributes */
   char name[NC_MAX_NAME+1];
   long lenp;
   char value[NC_MAX_NAME+1];
   for (int idx=0; idx<nvars && ret<2; idx++){
      nco_inq_var(nc_id,idx,name,&var_type,&var_ndims,var_dimids,&var_natts);
      lenp=0;
      if(!nco_inq_attlen_flg(nc_id,idx,"standard_name",&lenp)){
         NCO_GET_ATT_CHAR(nc_id,idx,"standard_name",value);
         value[lenp]='\0';
         if(strcmp(value,"latitude") == 0){
            strcpy(latvar,name);
            *latid = idx;

            /* Get units; assume same for both lat and lon */
            nco_inq_attlen(nc_id,idx,"units",&lenp);
            NCO_GET_ATT_CHAR(nc_id,idx,"units",units); /* fxm: bug TODO nco925: "units" value needs dynamically allocated size in case value exceeds NC_MAX_NAME */
            units[lenp] = '\0';

            /* Assign type; assumed same for both lat and lon */
            *coordtype = var_type;

            ret++;
         }
         if(strcmp(value,"longitude") == 0){
            strcpy(lonvar,name);
            *lonid = idx;
            ret++;
         }
      }
   }
   return ret == 2;  // true if both found
}

int 
nco_getdmninfo
(int nc_id,
 int varid,
 char dimname[],
 int *dimid,
 long *dmnsz
){
   /* Purpose: Get dimension information associated with specified variable
      In our case, this is lat or lon---they are presumed to be identical. */

   /* Note: currently no error handling on calls. */
   int ret = 1; 

   /* Get information about the dimension */
   nc_type var_type;                   /* variable type */
   int var_ndims;                      /* number of dims */
   int var_dimids[NC_MAX_VAR_DIMS];    /* dimension ids */
   int var_natts;                       /* number of attributes */
   nco_inq_var(nc_id,varid,0,&var_type,&var_ndims,var_dimids,&var_natts);

   *dimid = var_dimids[0];
   (void)nco_inq_dimlen(nc_id,var_dimids[0],dmnsz);
   (void)nco_inq_dimname(nc_id,var_dimids[0],dimname);
   return ret;
}

lmt_sct **
nco_aux_evl
(int in_id, 
int aux_nbr, 
char *aux_arg[],
int *lmt_nbr
){
   lmt_sct **lmts = 0;

   /* Obtain lat/lon variable names */
   char latvar[NC_MAX_NAME+1];
   char lonvar[NC_MAX_NAME+1];
   char units[NC_MAX_NAME+1]; /* fxm: bug TODO nco925: "units" value needs dynamically allocated size in case value exceeds NC_MAX_NAME */
   char dmnname[NC_MAX_NAME+1];
   int latid, lonid;
   long dmnsz = 0;
   int dmnid;
   nc_type coordtype;
   if(!nco_find_lat_lon(in_id, latvar, lonvar, units, &latid, &lonid, &coordtype)){
      printf("Unable to indentify lat/lon auxillary coordinate variables.");
      exit(-1);
   }
   if(!nco_getdmninfo(in_id, latid, dmnname, &dmnid, &dmnsz)){
      printf("Unable to get dimension inforamtion\n.");
      exit(-1);
   }
/*   printf("coords are: %s %s; units are: %s; %s %ld\n",latvar,lonvar,units,dmnname,dmnsz); */

   dmn_sct lat;
   dmn_sct lon;
   float *latvp;
   float *lonvp;

   /* load the lat/lon vars needed to search for region matches. */
   lat.type = coordtype;
   lat.sz = dmnsz;
   lat.srt = 0;
   latvp=(void *)nco_malloc(dmnsz*nco_typ_lng(lat.type));
   lon.type = coordtype;
   lon.sz = dmnsz;
   lon.srt = 0;
   lonvp=(void *)nco_malloc(dmnsz*nco_typ_lng(lon.type));
   nco_get_vara(in_id,latid,&lat.srt,&lat.sz,latvp,lat.type);
   nco_get_vara(in_id,lonid,&lon.srt,&lon.sz,lonvp,lon.type);

   *lmt_nbr = 0;

   lmt_sct base;
   base.nm = (char *)strdup(dmnname);
   base.lmt_typ = lmt_dmn_idx;
   base.is_usr_spc_lmt = 1; 
   base.is_usr_spc_min = 1; 
   base.is_usr_spc_max = 1;
   base.srd_sng = (char *)strdup("1");
   base.is_rec_dmn = 0;
   base.id = dmnid;
   base.min_idx = 0;
   base.max_idx = 0;
   base.srt = 0;
   base.end = 0;
   base.cnt = 0;
   base.srd = 1;

   /*
      malloc the return lmt structure
      No way to know the right size but...
      Absolute max is something like dimsz/2 - not sure what the formula is
      but this should work most/all of the time
   */
   int MAXDMN = dmnsz/4;

   if(aux_nbr > 0) lmts=(lmt_sct **)nco_malloc(MAXDMN*sizeof(lmt_sct *));

   int cur;
   float lllon, lllat, urlon, urlat;
   float clat, clon;
   int cell;
   for (cur=0; cur<aux_nbr; cur++){
      /* Parse into lllong,lllat,urlon,urlon, accounting for units */
      nco_aux_prs(aux_arg[cur],units, &lllon, &lllat, &urlon, &urlat);
      /* printf("Box is %f %f %f %f\n",lllon, lllat, urlon, urlat); */

      int mincell = -1;
      int consec = 0;
      for (cell=0; cell<dmnsz; cell++){
         clat = latvp[cell];
         clon = lonvp[cell];
         /* printf("looking at coord %f %f\n",clat,clon); */
         if(clon >= lllon && clon <= urlon &&
               clat >= lllat && clat <= urlat ){
            /* printf("**matched a cell %d \n",cell); */
            if(mincell == -1){
               mincell = cell;
               consec = 1;
            } else if(cell == mincell + consec){
               consec++;
            } else {
            }
         } else if(mincell != -1){
            char buf[100];
            /* printf("Have a pairing %d %d\n",mincell, mincell+consec-1); */
            sprintf(buf,"%d",mincell);
            base.min_sng = (char *)strdup(buf);
            base.min_idx = base.srt = mincell;
            sprintf(buf,"%d",mincell+consec-1);
            base.max_sng = (char *)strdup(buf);
            base.max_idx = base.end = mincell+consec-1;
            base.cnt = consec;
            (*lmt_nbr)++;
            if(*lmt_nbr > MAXDMN){
               printf("Number of slabs exceeds allocated mamory %d\n",MAXDMN);
               exit(-1);
            }
            lmts[(*lmt_nbr)-1] = (lmt_sct *)nco_malloc(sizeof(lmt_sct));
            *lmts[(*lmt_nbr)-1] = base;
            mincell = -1;
         }
      }
   }
/*
     printf ("returning structure %d\n",*lmt_nbr);
     for (cur=0; cur<(*lmt_nbr); cur++){
        printf("LIMIT %ld %ld \n",lmts[cur]->min_idx,lmts[cur]->max_idx);
     }
*/
   return lmts;
}

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
   <min_lon,max_lat,min_lon,max_lat> */
   sscanf(args,"%f,%f,%f,%f",lllon,urlon,lllat,urlat);
   if(strcmp(units,"radians") == 0){
     /* WIN32 math.h does not define M_PI */
#ifndef M_PI
# define M_PI		3.14159265358979323846
#endif /* M_PI */
      *lllon *= M_PI / 180.0;
      *lllat *= M_PI / 180.0;
      *urlon *= M_PI / 180.0;
      *urlat *= M_PI / 180.0;
   }
}

